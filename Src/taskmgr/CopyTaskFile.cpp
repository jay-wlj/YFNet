#include <errno.h>
#include "CopyTaskFile.h"
#include "../taskmgr/TaskMgrInterface.h"

#include "common-utility.h"
#include "common-commonfun.h"
#include "qvod_time.h"
#include "qvod_file.h"
#include "AutoPtr.h"

#ifdef WIN32
#include <atlconv.h>
#endif

CCopyTaskFile* CCopyTaskFile::m_instance = NULL;

CCopyTaskFile* CCopyTaskFile::Instance()
{
	if (NULL == m_instance)
	{
		m_instance = new CCopyTaskFile();
	}
	return m_instance;
}

void CCopyTaskFile::FreeInstance()
{
	if (m_instance != NULL)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

CCopyTaskFile::CCopyTaskFile()
{
	m_bRunning = false;
	m_hThread = NULL;
}

CCopyTaskFile::~CCopyTaskFile()
{

}

bool CCopyTaskFile::Run()
{
	if (m_bRunning)
	{
		return true;
	}
	m_bRunning = true;
	QvodCreateThread(&m_hThread,(qvod_routine_pt)Routine,(void*)this);
	if (0 == m_hThread)
	{
		m_bRunning = false;
		return false;
	}
	return true;
}

bool CCopyTaskFile::Stop()
{
	if (!m_bRunning)
	{
		return true;
	}
	m_bRunning = false;
	if (m_hThread)
	{
		QvodWaitForThread(m_hThread, NULL, 100);
		//WaitForSingleObject(m_hThread,3000);
		m_hThread = NULL;
	}
	return true;
}

void QVOD_WINAPI CCopyTaskFile::Routine( void* pVoid )
{
	CCopyTaskFile* pThis = (CCopyTaskFile*)pVoid;
	pThis->DoRoutine();
	return ;
}

void CCopyTaskFile::DoRoutine()
{
	while (m_bRunning)
	{
		if (m_taskMap.size() == 0)
		{
			QvodSleep(500);
			continue;
		}
		HASH hash;
		bool* pCopy = NULL;
		string wstrFilepath;
		{
			//从任务列表中取出一个任务
			CAutoLock lock(m_lock);			
			map<HASH,SCopyPara>::iterator iter = m_taskMap.begin();
			if (m_taskMap.end() == iter)
			{
				continue;
			}
			hash = iter->first;
			pCopy = iter->second.pCopy;
			wstrFilepath = iter->second.wstrFilepath;
		}
		int result = 0;
		QVOD_INT64 iFilesize = CTaskMgrInterFace::Instance()->GetFileSize(hash);
		if (!CTaskMgrInterFace::Instance()->TaskIsFinish(hash) || !pCopy || 0 == iFilesize)
		{
			DeleteOneElement(hash);
			continue;
		}		
		if (QVOD_ACCESS(wstrFilepath.c_str(),0) == 0)
		{
			//目标文件已经存在，检查文件大小是否和任务大小一样，是则认为已经拷贝
			qvod_stat_t st;
			//_stat64(wstrFilepath.c_str(),&st);		
			QVOD_STAT(wstrFilepath.c_str(),&st);
			if (iFilesize > 0 && iFilesize == st.st_size)
			{
				DeleteOneElement(hash);
				CTaskMgrInterFace::Instance()->SetCopyFullFileStatus(hash,1);
				CallBackToUI(1,result,Hash2Char(hash.data).c_str(),40);
				//通知UI
				continue;
			}
			//删除该文件，重新拷贝
			QVOD_REMOVE(wstrFilepath.c_str());
		}

#ifdef WIN32
		USES_CONVERSION;
#endif

		//创建目录		
		FILE* fp = fopen((wstrFilepath+".!tmp").c_str(),"wb");
		if (!fp)
		{
			if (errno == EACCES)
			{
				//没有权限
				result = 4;
			}
			else
			{
				result = 1;
			}
			DeleteOneElement(hash);
			CTaskMgrInterFace::Instance()->SetCopyFullFileStatus(hash,2);
			CallBackToUI(1,result,Hash2Char(hash.data).c_str(),40);
			//通知UI
			continue;
		}
		QPtr::AutoArrayPtr<char> pBuf = new char[1048576];
		
		QVOD_INT64 iTotalCopy = 0;
		int iReadlen = 1048576;
		
		while (m_bRunning && iTotalCopy < iFilesize && *pCopy)
		{
			iReadlen = iFilesize - iTotalCopy > 1048576?1048576:iFilesize - iTotalCopy;
			if (CTaskMgrInterFace::Instance()->ReadPiece(hash,pBuf,iReadlen,iTotalCopy,false) != iReadlen)
			{
				//读数据失败了
				result = 2;
				break;
			}
			if (fwrite(pBuf,iReadlen,1,fp) != 1)
			{				
				if (errno == ENOSPC)
				{
					//空间不足
					result = 3;
				}
				else	if (errno == EACCES)
				{
					//没有权限
					result = 4;
				}
				else
				{
					//其他错误
					result = 5;
				}				
				break;
			}
			iTotalCopy += iReadlen;
		}
		pBuf = NULL;
		fclose(fp);
		if (false == *pCopy)
		{
			//拷贝被UI取消了，删除临时文件
			QVOD_REMOVE((wstrFilepath+".!tmp").c_str());
		}
		else
		{
			if (m_bRunning)
			{
				//进程正常运行中，继续正常流程
				if (iTotalCopy == iFilesize)
				{
					//已经拷贝完成，改名
					QVOD_RENAME((wstrFilepath+".!tmp").c_str(),wstrFilepath.c_str());
					CTaskMgrInterFace::Instance()->SetCopyFullFileStatus(hash,1);					
				}
				else
				{
					//拷贝失败，删除临时文件
					QVOD_REMOVE((wstrFilepath+".!tmp").c_str());
					CTaskMgrInterFace::Instance()->SetCopyFullFileStatus(hash,2);
				}
				//通知UI结果
				CallBackToUI(1,result,Hash2Char(hash.data).c_str(),40);
			}
		}		
		DeleteOneElement(hash);
		
	}
}

bool CCopyTaskFile::AddOneTask( HASH& hash,string strFilepath )
{
	if (strFilepath.empty())
	{
		return false;
	}
	CAutoLock lock(m_lock);
	if (m_taskMap.find(hash) == m_taskMap.end())
	{
		bool* pCopy = new bool;
		*pCopy = true;
		SCopyPara para;
		para.pCopy = pCopy;
		para.wstrFilepath = strFilepath;
		m_taskMap.insert(make_pair(hash,para));
	}
	return true;
}

void CCopyTaskFile::DeleteTask( HASH& hash )
{
	CAutoLock lock(m_lock);
	map<HASH,SCopyPara>::iterator iter = m_taskMap.find(hash);
	if (iter != m_taskMap.end())
	{
		*(iter->second.pCopy) = false;
	}
}

void CCopyTaskFile::DeleteOneElement( HASH& hash )
{
	CAutoLock lock(m_lock);
	map<HASH,SCopyPara>::iterator iter = m_taskMap.find(hash);
	if (iter != m_taskMap.end())
	{
		if (iter->second.pCopy)
		{
			delete iter->second.pCopy;
		}		
		m_taskMap.erase(iter);
	}
}
