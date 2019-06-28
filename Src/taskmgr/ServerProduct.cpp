// ServerProduct.cpp: implementation of the CServerProduct class.
//
//////////////////////////////////////////////////////////////////////
#include "ServerProduct.h"
#include "common-utility.h"
#include "common-inifile.h"
#include "common-commonfun.h"
#include "stdafx.h"
#include "TaskMgrInterface.h"
#include "MsgPoolInterface.h"
#include "qvod_string.h"
#include "qvod_thread.h"
#include "sha1.h"
#include "DbTaskConfig.h"

extern qvod_thread_event_t g_shareEvent;


int CServerProduct::CreateTaskHash(string &strKeyUrl, unsigned char *data)
{
	//��������hash
	CSHA1 sha1;
	sha1.write(strKeyUrl.c_str(),strKeyUrl.size());
	sha1.read((void*)data);

	return 1;
}


int CServerProduct::CreateYfTask(const HASH& hash,std::string& strFileName,int type,string& strHttpOrgUrl,string& strHttpKeyUrl,string& strHttpReferer )
{
	int result = CTaskMgrInterFace::Instance()->CreateYfTask(hash,strFileName,type,strHttpOrgUrl,strHttpKeyUrl,strHttpReferer,false,NULL);
	switch(result)
	{
	case 0:
		{
			//�����ɹ���ȥ����
			AutoPtr<CTask> pTask;
			if ( CTaskMgrInterFace::Instance()->FindTask(hash,&pTask) )
			{
				// �����ļ��������·��
				if (pTask->m_Para.bCopyFullFile && pTask->m_Para.strCopyPath.empty())
				{
					pTask->SetCopyFullFile(pTask->m_Para.filepath);
				}
				//�ļ�·��Ҫ�����·����ȥ��g_media_dir
				int pos = pTask->m_Para.filepath.find(g_media_dir);
				if (pos != string::npos)
				{
					pos += g_media_dir.size();
					pTask->m_Para.filepath = pTask->m_Para.filepath.substr(pos);
				}

				if ( type < HTTPUPTASK)		// �ϴ����񲻱���������Ϣ
				{
 					if (CDbTaskConfig::Instance()->AddTask(pTask->m_Para) != 0)
 					{
	#ifdef QVODDEBUG
						_OUTPUT(ERROR1,"CDbTaskConfig::Instance()->AddTask(pTask->m_Para) != 0");
	#endif
 						//д���ݿ�ʧ�ܣ�ɾ���ڴ��е�����
 						//CTaskMgrInterFace::Instance()->SetTaskStatus(hash,CTask::STOP);
 						//return -1;
 					}
				}
			}
		}
		break;
	case 1:
		{

		}
		break;
	case 2:
		{

		}
		break;
	}	
	//�жϵ�ǰ�ļ��Ļ����С�Ƿ񳬹����ɻ����С��������ɾ����ɵ�����
	if (g_iMaxUseSpace != 0)
	{
		ClearSpaceExceptHash(hash);
	}	
	return result;
}

int CServerProduct::LoadYfTask( HASH& hash,const std::string& strFileName,TaskCfg* cfg )
{
	if (NULL == cfg)
	{
		return -1;
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO,"~~~~~~~Load Yf Task,%s hash:%s key:%s url:%s\n",strFileName.c_str(), Hash2Char(hash.data).c_str(),
		cfg->strHttpKeyUrl.c_str(), cfg->strHttpOrgUrl.c_str());
#endif // QVODDEBUG
	int ret = CTaskMgrInterFace::Instance()->CreateYfTask(hash,strFileName,cfg->type,cfg->strHttpOrgUrl,cfg->strHttpKeyUrl,cfg->strRefUrl,true,cfg);
#ifdef QVODDEBUG
	_OUTPUT(INFO,"Load Yf Task ret=%d\n", ret);
#endif // QVODDEBUG
	switch(ret)
	{
	case 0:
		{

		}
		break;
	case 1:
		{
			//������
			//if (HTTPP2PDOWNTASK == cfg->type || HLSP2PDOWNTASK == cfg->type)
			//{
			//	CMsgPoolInterface::Instance()->Share(hash);
			//}
		}
		break;
	case 2:
		{
		}
		break;
	default:
		return -1;
	}

	return 0;
}

int CServerProduct::SetTaskStatus(HASH& hash,int iStatus,bool bIsAll,bool bIsLoad)
{
	if ( bIsAll ==  true)
	{
		if ( iStatus == CTask::RUNNING )
		{	
			CTaskMgrInterFace::Instance()->StartAllTask();	
			CMsgPoolInterface::Instance()->StartALLTask();	
		}
		else if ( iStatus == CTask::PAUSE )
		{
			CTaskMgrInterFace::Instance()->PauseAllTask();
			CMsgPoolInterface::Instance()->PauseAllTask();
		}
	}
	else
	{
		CTaskMgrInterFace::Instance()->SetTaskStatus(hash,iStatus,(CTask::STOP==iStatus));	
		switch( iStatus )
		{
		case CTask::RUNNING:
			CMsgPoolInterface::Instance()->StartOneTask(hash);
			break;
		case CTask::PAUSE:
			CMsgPoolInterface::Instance()->PauseOneTask(hash);
			break;
		case CTask::STOP:
			{
				CMsgPoolInterface::Instance()->DelOneTask(hash);
			}
			break;
		case CTask::PLAY:
			break;
		case CTask::WAIT:
			{
				if (CTaskMgrInterFace::Instance()->GetTaskStatus(hash) == CTask::RUNNING)
				{
					CMsgPoolInterface::Instance()->PauseOneTask(hash);
				}
			}
			break;
		default:
			break;
		}	
	}

	return 0;
}

int CServerProduct::DeleteOldTaskExceptHash(const HASH& hash,QVOD_INT64* pReleaseSpace/*=NULL */)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO,"DeleteOldTaskExceptHash %s\n",Hash2Char(hash.data).c_str());
#endif
	vector<TaskCfg> taskVec;
	if (CDbTaskConfig::Instance()->GetAllTask(taskVec) == 0)
	{
		HASH delHash;
		long maketime = 0;
		for (vector<TaskCfg>::iterator iter = taskVec.begin(); iter != taskVec.end(); ++iter)
		{
			if (CTaskMgrInterFace::Instance()->IsTaskPlaying(HASH(iter->hash)))	// ��ɾ����ǰ��������
			{
				continue;
			}
			if (memcmp(iter->hash,hash.data,20) != 0 && (maketime > iter->maketime || 0 == maketime) &&
				(!iter->bCopyFullFile /*|| iter->copystatus == 1*/))	// ɾ������ʱ����ɵķ���������
			{
				memcpy(delHash.data, iter->hash, 20);
				maketime = iter->maketime;
			}
		}
		if (CTaskMgrInterFace::Instance()->TaskIsExist(delHash))
		{
			if (pReleaseSpace)
			{
				if (CTaskMgrInterFace::Instance()->GetTaskType(delHash) == HLSDOWNTASK)
				{
					*pReleaseSpace = CTaskMgrInterFace::Instance()->GetDownloadedSize(delHash);
				}
				else
				{
					*pReleaseSpace = CTaskMgrInterFace::Instance()->GetFileSize(delHash);
				}				
			}
			SetTaskStatus(delHash,CTask::STOP);
#ifdef QVODDEBUG
			_OUTPUT(INFO,"DeleteOldTaskExceptHash del:[%s]  maketime=%u ReleaseSpace=%llu\n",Hash2Char(delHash.data).c_str(), maketime, *pReleaseSpace);
#endif
			return 0;
		}
	}
	return -1;
}

int CServerProduct::ClearSpaceExceptHash(const HASH& hash)
{
	int ret = -1;
	//�жϵ�ǰ�ļ��Ļ����С�Ƿ񳬹����ɻ����С��������ɾ����ɵ�����
	if (g_iMaxUseSpace != 0)
	{
		QVOD_INT64 iTotalSize = CTaskMgrInterFace::Instance()->GetAllTaskSize();
#ifdef QVODDEBUG
		_OUTPUT(INFO,"All task size = %lld,maxusespace = %lld\n",iTotalSize,g_iMaxUseSpace);
#endif
		while (iTotalSize > g_iMaxUseSpace)
		{
			QVOD_INT64 iReleaseSpace = 0;
			if (DeleteOldTaskExceptHash(hash,&iReleaseSpace) != 0)
			{
				//û�п�ɾ�������˳�ѭ��
				break;
			}
			iTotalSize -= iReleaseSpace;
		}

		if (iTotalSize > g_iMaxUseSpace)
		{
			if (! CTaskMgrInterFace::Instance()->IsCopyFullFile(hash))
			{
				// ɾ�������߱����񲥷ŵ�ǰ������
				iTotalSize -= CTaskMgrInterFace::Instance()->DeleteHasPlayingData(hash, iTotalSize-g_iMaxUseSpace-10240);
			}
		}
		ret = (iTotalSize < g_iMaxUseSpace)? 0: -1;
	}

	return ret;
}