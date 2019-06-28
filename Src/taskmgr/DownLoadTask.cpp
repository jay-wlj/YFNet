// DownLoadTask.cpp: implementation of the CDownLoadTask class.
//
//////////////////////////////////////////////////////////////////////

#include "DownLoadTask.h"
#include "global.h"
#include "sha1.h"
#include "stund.h"
#include "common-commonfun.h"
#include "TaskMgrInterface.h"
#include "HttpAgentInterface.h"
#include "common-utility.h"
#include "CacheDataInMem.h"
#include "CopyTaskFile.h"
#include "MD5Checksum.h"
#include "../Common/CfgHost.h"
#include "AutoPtr.h"
#include "ServerProduct.h"

CDownLoadTask::CDownLoadTask( const unsigned char* hash,std::string strSaveFile,int type,string& strHttpOrgUrl,string& strHttpKeyUrl,string& strHttpReferer )
{
	m_bBuildSeed = false;
	m_Para.type = type;
	m_Para.strHttpOrgUrl = strHttpOrgUrl;
	m_Para.strHttpKeyUrl = strHttpKeyUrl;
	m_Para.strRefUrl = strHttpReferer;
	m_Para.filepath = strSaveFile;
	if(hash)
	{	
		memcpy(m_Para.hash, hash, 20);
	}

	size_t pos = strSaveFile.rfind("/")+1;
	m_strPath = strSaveFile.substr(0,pos);
	m_strFileName = strSaveFile.substr(pos,strSaveFile.size() - pos);


	int i;
	int len = (int)strSaveFile.size();
    len = len > 41? len: 41;
	char* strSeedName = new char[len+1];

	strSeedName[len] = 0;
	const char* pstr = strSaveFile.c_str();
	for(i = 0;i< len;i++)
	{
		if(pstr[i] == ':'|| pstr[i] == '\\' || pstr[i] == '/')
		{
			strSeedName[i] ='_';
		}
		else
		{
			strSeedName[i] = pstr[i];
		}
	}
    
    string strfile = g_strtorrentpath+string(strSeedName)+".mem";
    if (QVOD_ACCESS(strfile.c_str(), 0) == -1) //不存在则更新为简洁路径
    {
        strcpy(strSeedName, Hash2Char(hash).c_str());
    }

	m_seed = new CSeed(strSeedName);
	m_mem = new CMem(strSeedName);

	SafeDeleteArray(strSeedName);
}

CDownLoadTask::~CDownLoadTask()
{
	//add by wangxijie
	if(m_Para.status == CTask::STOP)
	{
		bool bDeleteAll = !m_bIsReservedFile;
		RemoveTmpFile(bDeleteAll);
	}
}

void CDownLoadTask::RemoveTmpFile(bool bDeleteAll)
{
	m_seed->Remove();
	m_mem->Remove();

	unsigned int i;
	for (i = 0;i<m_vctFile.size();i++)
	{
		CQvodFile *pFile = m_vctFile[i];
 		pFile->RemoveTempFile();
		if (bDeleteAll)
		{
			pFile->Remove();
		}		
	}
	if (bDeleteAll)
	{
		QvodRemoveFolder(m_strPath.c_str());	// 删除文件夹
	}
}

bool CDownLoadTask::TaskIsFinish(bool bCheckFileExist)
{
	bool bRet = CTask::TaskIsFinish(bCheckFileExist);
	if (bRet && bCheckFileExist)			// 任务已完成 检测文件
	{
		bRet = IsFileSizeMatching();
		if (! bRet && m_mem)					// 进度下载完成，但文件被删除，则重置下载进度
		{
#ifdef QVODDEBUg
			_OUTPUT(INFO, "CDownLoadTask::TaskIsFinish IsFileSizeMatching false clear m_mem");
#endif
			m_mem->Clear();		
		}
	}
	return bRet;
}

bool CDownLoadTask::ValidateHash()
{
	return true;
}


bool CDownLoadTask::LoadMemFile()
{
	//没有种子的任务，由mem文件中的文件大小来计算块大小和块数并初始化seed的部分变量
	if (HTTPDOWNTASK == m_Para.type ||
		HLSP2PDOWNTASK == m_Para.type)
	{
		if (!m_seed->IsExist())
		{
			QVOD_INT64 iFilesize = m_mem->GetFilesizeFromMemfile();
			m_seed->SetFileLength(iFilesize);
		}
	}
	if ( !m_mem->Init(m_seed->GetDownFileSize(), m_seed->GetPieceSize()) )
	{
		return false;
	}
	m_downQue = new unsigned char[m_mem->GetBitFieldLen()];	
	memset(m_downQue,0,m_mem->GetBitFieldLen());
	return true;
}

bool CDownLoadTask::InitFileList(bool bIsShare)
{
#if SUPERNODE
	//修正改名失败
	bool bNeedFix = false;
	if((!bIsShare || GetStatus() == CTask::PAUSE) && m_mem->IsDownFinish())
	{
		bNeedFix = true;
	}
#endif

#ifndef NETBAR
	if (HTTPDOWNTASK == m_Para.type ||
		HLSP2PDOWNTASK == m_Para.type)
	{
		CQvodFile* pFile = NULL;
		pFile = new CQvodFile(m_strPath + m_strFileName ,m_seed->GetFileTotleLen(),bIsShare);
		if(pFile == NULL)
		{
			return false;
		}
		m_vctFile.push_back(pFile);
	}
	else
	{
		return false;
	}
#endif
	return true;
}

int CDownLoadTask::RemoveFile(std::string &strFile)
{
	if ( QVOD_ACCESS(strFile.c_str(),0) != -1 )
	{
		bool bRet = 0 == QVOD_REMOVE(strFile.c_str());
#ifdef _DEBUG
		if ( ! bRet )
		{
			//delete failed!
			int iError = GetLastError();
			_OUTPUT(INFO,"Delete File Failed!!!File: %s,Error Code:%d\n",strFile.c_str(),iError);
		}
#endif // _DEBUG
		return bRet;
	}
	return 0;
}

#ifdef NETBAR
bool CDownLoadTask::IsBufferFull()
{
	CAutoLock lock(m_lockMemPiece);
	if (m_mapMemPiece.empty())
	{
		return false;
	}
	if (m_mapMemPiece.size() >= m_MaxPieces)
	{
		return true;
	}
	return false;
}

bool CDownLoadTask::DelMemPiece()
{
	CAutoLock lock(m_lockMemPiece);
	if (m_mapMemPiece.size() < m_MaxPieces)//未满不用删
	{
		return false;
	}
	map<int, PieceBuffer>::iterator it = m_mapMemPiece.begin();
	if (it->first < m_playerIndex)
	{
		if (m_mem->ClearBitField(it->first))//清除比特位
		{
			m_mem->ClearDownLength(it->second.len);
			m_curDownSize -= it->second.len;
		}
#ifdef QVODDEBUG
		_OUTPUT(INFO, "clear begin index %d\n", it->first);
#endif
		delete[] it->second.buf;
		m_mapMemPiece.erase(it);
		return true;
	}
	else
	{
		it = m_mapMemPiece.end();
		it--;
		if (it->first >= m_playerIndex + m_MaxPieces  )
		{
			if (m_mem->ClearBitField(it->first))//清除比特位
			{
				m_mem->ClearDownLength(it->second.len);
				m_curDownSize -= it->second.len;
			}
#ifdef QVODDEBUG
			_OUTPUT(INFO, "clear last index %d\n", it->first);
#endif
			delete[] it->second.buf;
			m_mapMemPiece.erase(it);
			return true;
		}
		else
		{
			m_isNeedDownload = false;//已经下完连续的且未读的5M数据，不用下载了
		}
	}
	return false;
}
#endif

//modified by tangtao
//-1: 写硬盘失败
// 0: 错误数据
//>0: 写成功
int CDownLoadTask::WriteData(const char *buf, UINT len, unsigned int index, unsigned int start)
{
	//分片检测
	char data[20];
	CSHA1 sha;
	sha.write(buf,len);
	sha.read(data);
	HASH hash;
	memcpy(hash.data,data,20);
	if(!m_seed->CheckHash(index,hash))//check piece content for error
	{
#ifdef QVODDEBUG
		_OUTPUT(SERIOUS,"Write file hash error,index: %d,len %d!!!\n",index,len);
#endif // _DEBUG
		CancelDownQue(index);
		DelPiece(index);
		return -101;
	}


	QVOD_INT64 offset = (QVOD_INT64)index*(QVOD_INT64)m_seed->GetPieceSize()+(QVOD_INT64)start;
	unsigned int i = 0;

	while (m_vctFile.size() > i && m_vctFile[i]->GetFileLen() <= offset)
	{
		offset -= m_vctFile[i++]->GetFileLen();
	}

	if(m_vctFile.size() == i) 
	{
		//写入失败,删除分片,重新下载
		DelPiece(index);
		if(!TaskIsFinish())
		{
			CancelDownQue(index);
		}
		return 0;
	}

	int iResult = 1;
	char* pdatabuf = (char *)buf;
	int tlen = len;
	while(tlen > 0 &&  i < m_vctFile.size())
	{
		int nlen = m_vctFile[i++]->Write(offset,pdatabuf,tlen);
		if(nlen < 0)
		{
			//写入失败,删除分片,重新下载
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "写入失败:%s%s,%d!\n",m_strPath.c_str(),m_strFileName.c_str(),nlen);
#endif // _DEBUG

			DelPiece(index);
			if(!TaskIsFinish())
			{
				CancelDownQue(index);
			}	
			if (-2 == nlen)
			{
				m_iErrorMask |= TASK_FILE_NOT_FOUND;
			}
			else	if (-3 == nlen)
			{
				m_iErrorMask |= TASK_WRITE_NO_SPACE;
			}
			else
			{
				m_iErrorMask |= TASK_WRITE_FAILED;
			}
			return nlen;
		}
		tlen -= nlen;
		pdatabuf += nlen;
		offset = 0;
	}

	//更新进读
	if(m_mem)
	{
		if(m_mem->SetBitField(index))
		{
			m_curDownSize += len;
			m_mem->SetDownLength(len);
#ifndef NETBAR
			m_mem->WriteToDisk();
#endif
		}

#ifndef NETBAR
		if(m_mem->IsDownFinish() && m_bIsUpdate)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO,"%s p2p下载完了\n",Hash2Char(m_Para.hash).c_str());
#endif
			iResult = 2;
			Rename();
			m_bIsUpdate = false;

			//if (m_Para.bCopyFullFile)
			//{
			//	//把文件拷贝一份到下载目录
			//	CCopyTaskFile::Instance()->AddOneTask((HASH&)m_Para.hash,m_Para.strCopyPath);
			//}
		}
#endif
	}
	//写入成功,删除分片,更新下载pos
	UpdatePieceFinish(index);

	//写入成功,清除错误掩码位置
	if ( m_iErrorMask & TASK_WRITE_FAILED )
	{
		m_iErrorMask &= ~(TASK_WRITE_FAILED);
	}
	if ( m_iErrorMask & TASK_FILE_NOT_FOUND )
	{
		m_iErrorMask &= ~(TASK_FILE_NOT_FOUND);
	}
	if (m_iErrorMask & TASK_WRITE_NO_SPACE)
	{
		m_iErrorMask &= ~(TASK_WRITE_NO_SPACE);
	}
	return iResult;
}

int CDownLoadTask::ReadData(char *buf, UINT len, unsigned int index, unsigned int start,bool bIsUpload)
{
	int totallen = 0;
	 QVOD_UINT64 filelen = m_seed->GetFileTotleLen();
	 QVOD_UINT64 offset =  QVOD_UINT64(index) *  QVOD_UINT64(m_seed->GetPieceSize()) +  QVOD_UINT64(start);

	//如果超过文件大小,返回错误
	len = (UINT)(len > (filelen - offset) ? (filelen-offset) : len);
	if(offset >= filelen || len == 0)
	{
		return -2;
	}

	//未下载到种子没解析出长度
	if(filelen == 0)
	{
		return 0;
	}

	//判断文件是否已经下载
	unsigned int endindex = (unsigned int)((offset+len-1)/m_seed->GetPieceSize());

	for(unsigned int i = index;i <= endindex;i++)
	{
		if(!m_mem->CheckIndex(i))
		{
			if (bIsUpload)
			{
				//用于上传的，需要数据量完全满足
				return 0;
			}
			else
			{
				//非上传的，有多少数据读多少数据
				if (i > index)
				{
					len = ( QVOD_UINT64)i*m_seed->GetPieceSize() - offset;
				}
				else
				{
					return 0;
				}
			}
		}
	}
	unsigned int i = 0;					
	while(m_vctFile.size() >i && ( QVOD_UINT64)(m_vctFile[i]->GetFileLen()) <= offset)
	{
		offset -= m_vctFile[i++]->GetFileLen();
	}

	if(m_vctFile.size() == i) 
	{
		return 0;
	}

	char* pdatabuf = buf;
	UINT tlen = len;


	while(tlen>0 &&  i < m_vctFile.size())
	{
		int nlen = m_vctFile[i++]->Read(offset, pdatabuf ,tlen);
		if(nlen < 0)
		{
			return nlen;
		}		
		tlen -= nlen;
		pdatabuf += nlen;
		totallen += nlen;
		offset = 0;
	}
	
	if (bIsUpload)
	{
		m_curUpSize += totallen;
	}
	return totallen;
}

int CDownLoadTask::run(bool bLoad)
{
	if ( m_strFileName.find('?') != std::wstring::npos )
	{
		//数据库中记录文件名在转换是乱码了,可能该系统不支持中文
		//删除任务,因为已经无法写入文件了.
		//其他文件都不用删除了,因为没办法了...已经丢失了...
		return -1;
	}
#ifdef MONGOTV
	//放在这里计算是因为maketime在Run之前才确定
	char szBuf[100] = {0};
	sprintf(szBuf,"%s%s%u",g_szSPeerID,Hash2Char(m_Para.hash).c_str(),m_Para.maketime);
	string strMd5 = CMD5Checksum::GetMD5((unsigned char*)szBuf,strlen(szBuf));
	memcpy(m_vid, strMd5.c_str(), 32);
#endif

	if (HTTPDOWNTASK == m_Para.type ||
		HLSP2PDOWNTASK == m_Para.type)
	{
		//http任务没有torrent，但有mem文件
		//文件存在
		if ( QVOD_ACCESS((m_strPath + m_strFileName).c_str(),0) != -1 )
		{
			if (m_mem->IsExist() && LoadMemFile() && m_mem->IsDownFinish() && IsFileSizeMatching() && InitFileList(true))
			{
				return 1;
			}
			else
			{
				m_mem->Remove();
				remove((m_strPath + m_strFileName).c_str());
				if ( bLoad )
				{
					if ((m_Para.bCopyFullFile && m_Para.copystatus != 1))
					{
						//任务需要下载完拷贝、且未拷贝，或为升级任务，需要重新下载
						return 0;
					}
					//任务是加载的，直接删除					
					return -1;
				}
				else
				{
					//重新下载
					return 0;
				}
			}
		}
		else	if( QVOD_ACCESS((m_strPath + m_strFileName + string(".!mv")).c_str(),0) != -1 )
		{
			if (m_mem->IsExist() && LoadMemFile() && InitFileList(false))
			{
				return 0;
			}
			else
			{
				m_mem->Remove();
				remove((m_strPath + m_strFileName + string(".!mv")).c_str());
				if ( bLoad )
				{					
					if ((m_Para.bCopyFullFile && m_Para.copystatus != 1))
					{
						//任务需要下载完拷贝、且未拷贝，或为升级任务，需要重新下载
						return 0;
					}
					//任务是加载的，直接删除					
					return -1;
				}
				else
				{
					//重新下载
					return 0;
				}
			}
		}
		else
		{
			m_mem->Remove();
			if ( bLoad )
			{
				if ((m_Para.bCopyFullFile && m_Para.copystatus != 1))
				{
					//任务需要下载完拷贝、且未拷贝，或为升级任务，需要重新下载
					return 0;
				}
				//任务是加载的，直接删除				
				return -1;
			}
			else
			{
				//重新下载
				return 0;
			}
		}
		return 0;
	}

	return 0;
}

void CDownLoadTask::UpdateDownloadMode()
{
	if (RUNNING == m_Para.status && !TaskIsFinish())
	{
		QVOD_UINT64	iPlayPos		= m_iPlayPos;
		int		iOldDownloadMode = m_iDownloadMode;

		if (HTTPDOWNTASK == m_Para.type)
		{	
			QVOD_INT64 iContinueLen = m_mem->GetContinueLen(iPlayPos);
			m_buffingpos = m_iPlayPos + iContinueLen;
			QVOD_INT64 iPlayFileRate = m_iPlayFileRate/8;
			iPlayFileRate = (0 == iPlayFileRate)?131072:iPlayFileRate;	//若码率为0，则默认为1Mbps
	//		if (/*iCacheContinueLen > 0 && */iContinueLen/*+iCacheContinueLen*/ > (m_iMValue+15)*iPlayFileRate)
	//		{
	//#ifdef QVODDEBUG
	//			_OUTPUT(INFO,"UpdateDownloadMode playpos = %lld,continuelen = %lld,cachecontinuelen = %lld\n",iPlayPos,
	//				iContinueLen,iCacheContinueLen);
	//#endif
	//			m_iDownloadMode = MODE_PAUSE;
	//			return ;
	//		}
			//不使用p2p，只有CDN模式
			m_iDownloadMode = MODE_CDN;

#ifdef QVODDEBUG
			int iP2pSpeed = ProcessQueryPeers()->GetSomePeerDownSpeed(NORMAL)/1024;
			int iDcdnSpeed = ProcessQueryPeers()->GetSomePeerDownSpeed(MINER)/1024;		
			_OUTPUT(INFO,"PlayPos = %I64d,VideoRate = %I64d bps,continuelen = %I64d KB,mode = %d,copy = %d,copystatus = %d,p2pspeed = %dKB/s,"\
				"dcdnspeed = %dKB/s,totalspeed = %dKB/s\n",iPlayPos,iPlayFileRate*8,iContinueLen/1024,m_iDownloadMode,m_Para.bCopyFullFile,
				m_Para.copystatus,iP2pSpeed,iDcdnSpeed,GetDownSpeed()/1024);
#endif

			if (MODE_CDN == m_iDownloadMode)
			{
				//若当前没有http任务，则添加任务
 				if (CHttpAgentInterface::Instance()->GetTaskInfo((HASH&)m_Para.hash,NULL) != 0)
				{
					YfHttpPieceReq req;
					int ret = DownNextHttpPiece(req);
					if (0 == ret)
					{
 						CHttpAgentInterface::Instance()->AddTask((HASH&)m_Para.hash,req.strOrlurl,req.strReferer,req.start,(QVOD_INT64)req.length);
					}
				}	
			}
		}
	}
	else
	{
#ifdef QVODDEBUG
		//_OUTPUT(INFO, "CDownLoadTask::UpdateDownloadMode status:%d ", m_Para.status);
#endif
	}
	
}

void CDownLoadTask::DownEmergencyData()
{
	if (m_Para.status != RUNNING)
	{
		return ;
	}

	int iPieceSize = m_seed->GetPieceSize();
	int iPieceNum = m_seed->GetPieceNum();
	if (0 == iPieceSize || 0 == iPieceNum)
	{
		return ;
	}
	int index = m_iPlayPos/iPieceSize;
	if ((index >= m_iHttpDownloadingBeginIndex && index <= m_iHttpDownloadingEndIndex && 
		CHttpAgentInterface::Instance()->GetTaskInfo((HASH&)m_Para.hash,NULL) == 0) || index >= iPieceNum)
	{
		return ;
	}
	if (m_mem->CheckIndex(index))
	{
		return ;
	}
	//当前播放位置没有数据，把状态直接改为MODE_CDN
	m_iDownloadMode = MODE_CDN;
	QVOD_INT64 start = (QVOD_INT64)index*iPieceSize;
	int length = iPieceSize;
	if (index == iPieceNum-1)
	{
		//最后一块了，大小需要调整
		length = m_seed->GetFileTotleLen() - start;
	}	
#ifdef QVODDEBUG
	_OUTPUT(INFO,"DownEmergencyData pos = %lld,len = %d\n",start,length);
#endif
	CHttpAgentInterface::Instance()->AddTask((HASH&)m_Para.hash,m_Para.strHttpOrgUrl,m_Para.strRefUrl,start,(QVOD_INT64)length);
}

int CDownLoadTask::WriteHttpData( const char *buf,  QVOD_UINT64 start,UINT len )
{
	unsigned int iPieceSize = m_seed->GetPieceSize();
	if (0 == iPieceSize)
	{
		return 0;
	}
	unsigned int iModNum = start%iPieceSize;
	if ( iModNum != 0)
	{
		//不在块的起始位置，理论上不可能
		start += (iPieceSize - iModNum);
		len -= (iPieceSize - iModNum);
	}
	int index = start/iPieceSize;
	if (len < iPieceSize && start + len != m_seed->GetFileTotleLen())
	{
		//小于块大小的必定为最后一块
		return 0;
	}
	//若文件还没初始化，在此初始化
	{
		//因为p2p下到种子后也会初始化文件，所以加锁，防止两边同时初始化
		CAutoLock lock(m_vctFileLock);
		if (m_vctFile.size() == 0)
		{
			CQvodFile* pFile = new CQvodFile(m_strPath + m_strFileName, m_seed->GetFileTotleLen());
			if(NULL == pFile)
			{
				return 0;
			}
			int ret = pFile->Create();
			if(ret < 0)
			{
				delete pFile;
				if (-2 == ret)
				{
					//空间不足
					return -3;
				}
				return ret;
			}
			m_vctFile.push_back(pFile);	
		}
	}
	
	QVOD_INT64 offset = start;
	unsigned int i = 0;

	int iResult = 1;
	char* pdatabuf = (char *)buf;
	int tlen = len;
	while(tlen > 0 &&  i < m_vctFile.size())
	{
		int nlen = m_vctFile[i++]->Write(offset,pdatabuf,tlen);
		if(nlen < 0)
		{
			//写入失败
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "写入失败:%s%s,%d!\n",m_strPath.c_str(),m_strFileName.c_str(),nlen);
#endif // _DEBUG		
			if (-2 == nlen)
			{ 
				m_iErrorMask |= TASK_FILE_NOT_FOUND;
			}
			else	if (-3 == nlen)
			{
				m_iErrorMask |= TASK_WRITE_NO_SPACE;
			}
			else
			{
				m_iErrorMask |= TASK_WRITE_FAILED;
			}
			return nlen;
		}
		tlen -= nlen;
		pdatabuf += nlen;
		offset = 0;
	}

	//更新进读
	if(m_mem)
	{
		//刚开始不知道块大小，可能会下载多块
		unsigned int iTmpLen = len;
		for (i = 0; i < len; i += iPieceSize )
		{	
			int iSetDownLength = iPieceSize;
			if (iTmpLen < iPieceSize)
			{
				iSetDownLength = iTmpLen;
			}
			if (iSetDownLength < iPieceSize && index+1 != m_seed->GetPieceNum())
			{
				//小于块大小且非最后一块，可能发生在请求0位置且长度为1M的时候，未下载完1M会发生，丢掉最后不到一块的数据
				break;
			}
#ifdef QVODDEBUG
			_OUTPUT(INFO,"WriteHttpData index = %d piecesize=%d\n",index, iPieceSize);
#endif	
			iTmpLen -= iSetDownLength;
			if (m_mem->SetBitField(index))
			{				
				m_curDownSize += iSetDownLength;
				m_mem->SetDownLength(iSetDownLength);
			}
			++index;
		}
		m_mem->WriteToDisk();

		if(m_mem->IsDownFinish() && m_bIsUpdate)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO,"%s http下载完了",Hash2Char(m_Para.hash).c_str());
#endif
			iResult = 2;
			Rename();
			m_bIsUpdate = false;
			//if (m_Para.bCopyFullFile)
			//{
			//	if (! m_Para.strCopyPath.empty())
			//	{
			//		//把文件拷贝一份到下载目录
			//		CCopyTaskFile::Instance()->AddOneTask((HASH&)m_Para.hash,m_Para.strCopyPath);
			//	}
			//	else
			//	{					
			//		CallBackToUI(0, 0, Hash2Char(m_Para.hash).c_str(),40);
			//	}
			//}
			HASH hash(m_Para.hash);
			CServerProduct::SetTaskStatus(hash,CTask::PAUSE);	// 离线完了就暂停任务
			g_cfg.m_nDownPlayingTaskCount = CTaskMgrInterFace::Instance()->GetDownloadingAndPlayingTaskNum();
			CallBackToUI(0, 0, Hash2Char(m_Para.hash).c_str(), 	40 );
		}
	}
	//写入成功,清除错误掩码位置
	if ( m_iErrorMask & TASK_WRITE_FAILED )
	{
		m_iErrorMask &= ~(TASK_WRITE_FAILED);
	}
	if ( m_iErrorMask & TASK_FILE_NOT_FOUND )
	{
		m_iErrorMask &= ~(TASK_FILE_NOT_FOUND);
	}
	if (m_iErrorMask & TASK_WRITE_NO_SPACE)
	{
		m_iErrorMask &= ~(TASK_WRITE_NO_SPACE);
	}
	return iResult;
}

unsigned long QVOD_WINAPI CDownLoadTask::BuildSeed( void* para )
{
	AutoPtr<CDownLoadTask> pTask = (CDownLoadTask*)para;
	pTask->duplicate();
	if (pTask->m_bBuildSeed)
	{
		return 0;
	}
	pTask->m_bBuildSeed = true;
	HASH hash;
	memcpy(hash.data,pTask->m_Para.hash,20);

	QVOD_INT64 fileLen = pTask->m_seed->GetFileTotleLen();
	if(0 == fileLen || pTask->m_vctFile.empty())
	{
		return -1;
	}

	int pieceLen = CalcPieceSize(fileLen);
	int pieceNum = (int)((fileLen+pieceLen-1)/pieceLen);

	QPtr::AutoArrayPtr<char> buf = new char[pieceLen];
	QPtr::AutoArrayPtr<Piece> pPiece = new Piece[pieceNum];
	CSHA1 sha;
	QVOD_INT64 readlen=0;
	QVOD_DWORD dwRead = 0;	
	int i=0;

	for (;readlen < fileLen && i < pieceNum;)
	{
		if (!g_bRunning || CTask::STOP == pTask->m_Para.status)	//终止时或删除任务时退出循环
		{
			return -1;
		}
		dwRead = fileLen-readlen;
		dwRead = dwRead > pieceLen?pieceLen:dwRead;
		if (pTask->m_vctFile[0]->Read(readlen,buf,dwRead) != dwRead)
		{
			return -1;
		}
		sha.reset();
		sha.write(buf,dwRead);
		sha.read(&pPiece[i]);
		i++;
		readlen += dwRead;
	}
	buf = NULL;
	if (readlen != fileLen)
	{
		return -1;
	}	
	//校验hash是否一致
	HASH buildHash;
	sha.reset();
	sha.write(pPiece,pieceNum*20);	
	sha.read(buildHash.data);
	if (memcmp(hash.data, buildHash.data, 20) != 0)
	{		
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"BuildSeed hash not match,%s:%s\n",Hash2Char(hash.data).c_str(),Hash2Char(buildHash.data).c_str());
#endif
		//把任务类型改为非p2p的
		if (HTTPP2PDOWNTASK == pTask->m_Para.type)
		{
			pTask->ChangeTaskType(HTTPDOWNTASK);
		}
		return -1;
	}

#ifdef WIN32
	HANDLE hFile = CreateFile((g_strtorrentpath+pTask->m_seed->GetMetafile()+".torrent").c_str(),GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_HIDDEN,NULL);
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		return -1;
	}

	QVOD_DWORD dwWrite = 0;
	char szData[512] = {0};
	int iLen = sprintf_s(szData,512,"d8:encoding3:GBK4:infod6:lengthi%I64de4:name%d:%s12:piece lengthi%de6:pieces%d:",
		fileLen,pTask->m_strFileName.length(),pTask->m_strFileName.c_str(),pieceLen,20*pieceNum);

	WriteFile(hFile,szData,iLen,&dwWrite,NULL);
	WriteFile(hFile,pPiece,20*pieceNum,&dwWrite,NULL);

	char timebuf[16]={0};
	sprintf_s(timebuf,16,"%u",time(NULL));
	std::string strtime(timebuf);
	std::string strtracker = HTTPTRACKER;	
	strtracker += "/";
	char *pAdUrl = NULL;

	memset(szData,0,512);
	iLen = sprintf_s(szData,512,"e8:announce%d:%s13:creation date%d:%se",
		strtracker.size(),strtracker.c_str(),strtime.size(),strtime.c_str());
	WriteFile(hFile,szData,iLen,&dwWrite,NULL);
	CloseHandle(hFile);
#endif

	if (!pTask->m_seed->Load())
	{
		pTask->m_seed->Remove();
	}
	else
	{
		//CMsgPoolInterface::Instance()->Share(hash);
	}
	return 0;
}

int CDownLoadTask::DownNextHttpPiece( YfHttpPieceReq& req,bool bRandom/* = false*/ )
{	
	CAutoLock lock(m_downNextHttpPieceLock);

#ifdef _360MOBILEMGR
	//360手机助手使用随机下载
	bRandom = true;
#endif

	int iDownloadMode = m_iDownloadMode;
	switch(iDownloadMode)
	{
	case MODE_CDN:
		{		
			unsigned char* pSBitField = m_mem->GetMemBit();
			int iPieceNum = m_seed->GetPieceNum();
			int iPieceSize = m_seed->GetPieceSize();
			int iBitFieldLen = m_mem->GetBitFieldLen();
			//若还没有mem，说明文件还没下载到数据，也未获取到文件大小，请求数据大小为1M
			if (NULL == pSBitField || 0 == iPieceSize || 0 == iPieceNum)
			{
				m_iHttpDownloadingBeginIndex = 0;
				m_iHttpDownloadingEndIndex = 0;
				req.strOrlurl = m_Para.strHttpOrgUrl;
				req.strReferer = m_Para.strRefUrl;
				req.start = 0;
				req.length = 1048576;
				return 0;
			}	
			req.strOrlurl = m_Para.strHttpOrgUrl;
			req.strReferer = m_Para.strRefUrl;
			req.length = iPieceSize;

			unsigned char* pSDownQue = m_downQue;
			int index = m_iPlayPos/iPieceSize;
			if (bRandom)
			{
				index = stunRand()%iPieceNum;
			}
			int i = index >> 3;
			int pos = index % 8;	
			bool bGetIndex = false;
			int iPlayFileRate = (0 == m_iPlayFileRate)?1048576:m_iPlayFileRate;
			iPlayFileRate /= 8;
			//最多下载MAX_DOWN_DATATIME_FROM_CDN秒数据，从播放点开始的25秒数据用大块下载，超过了则小块
			//离线下载的任务按大块下载			
			int iMaxLength = iPlayFileRate*g_cfg.m_lMaxDownTimeFromCDN;	
			if (!m_Para.bCopyFullFile)
			{
				QVOD_INT64 iContinueLen = m_mem->GetContinueLen(m_iPlayPos);
				iMaxLength = iMaxLength > iContinueLen?iMaxLength-iContinueLen:iPieceSize;
			}
			while(i < iBitFieldLen)
			{
				unsigned char bit;
				if (NULL == pSDownQue)
				{
					bit = ~pSBitField[i];
				}
				else
				{
					//下载大块时，直接覆盖p2p正在下载的块
					//非离线任务，因为离线任务的实时性要求不高，所以不用覆盖正在下载的p2p块
					if (iMaxLength > iPlayFileRate*10 && !m_Para.bCopyFullFile)
					{
						bit = ~pSBitField[i];
					}
					else
					{
						//存在p2p下载队列，需要排除p2p当前正在下载的块
						bit = (~pSBitField[i]) & (~pSDownQue[i]);
					}
				}
				while(bit && pos < 8)
				{
					if(bit & (0x80 >> pos))
					{
						int tmp = 8*i +pos;						
						if(tmp < iPieceNum)
						{
							if (!bGetIndex)
							{
								//取起始位置
								bGetIndex = true;
								m_iHttpDownloadingBeginIndex = tmp;
								m_iHttpDownloadingEndIndex = tmp;
								req.start = (QVOD_INT64)tmp*iPieceSize;
								if (tmp == iPieceNum - 1)
								{
									//最后一块，大小不一定为块大小
									req.length = m_seed->GetDownFileSize() - req.start;
									return 0;
								}
								iMaxLength -= iPieceSize;
								if (iMaxLength < iPieceSize)
								{
									return 0;
								}
							}							
							else
							{
								//取结束位置
								//继续遍历后面未下载的连续块，计算http要下载的大小
								if (tmp != m_iHttpDownloadingEndIndex+1)
								{
									return 0;
								}
								else
								{
									++m_iHttpDownloadingEndIndex;
									req.length += iPieceSize;
									if (tmp == iPieceNum - 1)
									{
										//最后一块，大小不一定为块大小
										req.length = m_seed->GetDownFileSize() - req.start;
										return 0;
									}	
									iMaxLength -= iPieceSize;
									if (iMaxLength < iPieceSize)
									{
										return 0;
									}
								}
							}
						}
					}
					else
					{
						//已经下载的，或者正在p2p下载的
						if (bGetIndex)
						{
							//无连续的块了，直接返回
							return 0;
						}
					}
					pos++;
				}
				i++;
				pos = 0;
			}
			if (bGetIndex)
			{
				return 0;
			}
#ifndef _360MOBILEMGR
			if (m_Para.bCopyFullFile)
#endif
			{
				//若设置了下载，则要从头下载剩余部分，直到任务下载完成
				i = 0;
				pos = 0;	
				while(i < iBitFieldLen)
				{
					unsigned char bit;
					if (NULL == pSDownQue)
					{
						bit = ~pSBitField[i];
					}
					else
					{
						//下载大块时，直接覆盖p2p正在下载的块
						//非离线任务，因为离线任务的实时性要求不高，所以不用覆盖正在下载的p2p块
						if (iMaxLength > iPlayFileRate*10 && !m_Para.bCopyFullFile)
						{
							bit = ~pSBitField[i];
						}
						else
						{
							//存在p2p下载队列，需要排除p2p当前正在下载的块
							bit = (~pSBitField[i]) & (~pSDownQue[i]);
						}
					}
					while(bit && pos < 8)
					{
						if(bit & (0x80 >> pos))
						{
							int tmp = 8*i +pos;						
							if(tmp < iPieceNum)
							{
								if (!bGetIndex)
								{
									//取起始位置
									bGetIndex = true;
									m_iHttpDownloadingBeginIndex = tmp;
									m_iHttpDownloadingEndIndex = tmp;
									req.start = (QVOD_INT64)tmp*iPieceSize;
									if (tmp == iPieceNum - 1)
									{
										//最后一块，大小不一定为块大小
										req.length = m_seed->GetDownFileSize() - req.start;
										return 0;
									}
									iMaxLength -= iPieceSize;
									if (iMaxLength < iPieceSize)
									{
										return 0;
									}
								}							
								else
								{
									//取结束位置
									//继续遍历后面未下载的连续块，计算http要下载的大小
									if (tmp != m_iHttpDownloadingEndIndex+1)
									{
										return 0;
									}
									else
									{
										++m_iHttpDownloadingEndIndex;
										req.length += iPieceSize;
										if (tmp == iPieceNum - 1)
										{
											//最后一块，大小不一定为块大小
											req.length = m_seed->GetDownFileSize() - req.start;
											return 0;
										}	
										iMaxLength -= iPieceSize;
										if (iMaxLength < iPieceSize)
										{
											return 0;
										}
									}
								}
							}							
						}
						else
						{
							//已经下载的，或者正在p2p下载的
							if (bGetIndex)
							{
								//无连续的块了，直接返回
								return 0;
							}
						}
						pos++;
					}
					i++;
					pos = 0;
				}
			}
			if (bGetIndex)
			{
				return 0;
			}
			return -2;
		}
		break;
	default:
		{
			return -2;
		}
		break;
	}
	return -1;
}

int CDownLoadTask::DeleteHasPlayingData(QVOD_UINT64 iCurCacheSize) 	// 删除当前播放位置前的缓存数据
{
	return 0;
}
