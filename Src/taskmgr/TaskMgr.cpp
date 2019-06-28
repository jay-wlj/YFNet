#include "TaskMgr.h"
//#include "Config.h"
#include "global.h"
#include "trackerinterface.h"
#include "Hash2ID.h"
#include "common-utility.h"
//#include "../msgpool/MsgPoolInterface.h"
#include "Iointerface.h"
#include "common-commonfun.h"
//#include "../terminal/DbTaskConfig.h"
#include "../httpserver/httpserver.h"
#include "../httpagent/HttpAgentInterface.h"
#include <time.h>
//#include "../Terminal/AppFrame.h"
#ifdef MONGOTV
#include "MongoTvCdn.h"
#endif

#include "DownLoadTask.h"
//#include "UpLoadTask.h"
#include "ServerProduct.h"
#include "qvod_sock.h"
#include "AutoPtr.h"
#include "M3u8Task.h"
#include "NFUploadTask.h"
#include "CfgHost.h"

extern CHttpServerMgr g_httpServer;

struct __ExportTask
{
	UINT filesize;
	UINT createtime;
	char hash[40];
	char reserved[3];
	std::string filename;
};

CTaskMgr::CTaskMgr()
{
}

CTaskMgr::~CTaskMgr()
{
	CAutoLock lock(m_task_map_lock);
	map<HASH, CTask *>::iterator iter;
	iter = m_task_map.begin();
	for ( ; iter != m_task_map.end() ; iter++)
	{
		CTask* pTask = iter->second;
		if ( pTask != NULL )
		{
			pTask->release();
		}
	}
	m_task_map.clear();
}


//∑µªÿ 0--≥…π¶ 1--¥À»ŒŒÒ≤ª¥Ê‘⁄ 2-- ß∞‹
int CTaskMgr::DelOneTask(HASH& hash)
{
	//‘⁄¬∑æ∂¡–±Ì÷–…æ≥˝
	std::map<string, VCTTASK>::iterator it2;
	m_task_path_lock.Lock();
	for(it2 = m_task_path.begin(); it2 != m_task_path.end(); it2 ++)
	{
		VCTTASK::iterator it3;
		for(it3 = it2->second.begin(); it3 != it2->second.end(); it3 ++)
		{
			if((*it3) && (memcmp((*it3)->GetHash(), hash.data, 20) == 0))
			{
				it2->second.erase(it3);
				break;
			}
		}
	}	
	m_task_path_lock.Unlock();
	DelTask(hash);
	return 0;
}

bool CTaskMgr::GetTorrentFile(HASH& hash, char*torrent, unsigned int &len)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		if(NULL == torrent) //÷ª»°÷÷◊”Œƒº˛≥§∂»
		{
			len = task->GetSeed()->GetTorrentLen();
			return true;
		}
		else //»°÷÷◊”Œƒº˛ƒ⁄»›
		{
			if(NULL == task->GetSeed()->GetTorrent())
			{
				return false;
			}
			else
			{
				memcpy(torrent, task->GetSeed()->GetTorrent(), task->GetSeed()->GetTorrentLen());
				return true;
			}
		}
	}

	return false;
}

bool CTaskMgr::GetAllTaskHashAndTradkerAddr(vector<Hash_Addr> &vec)
{
	map<HASH, CTask *>::iterator it;
	CAutoLock Lock(m_task_map_lock);
	for(it = m_task_map.begin(); it != m_task_map.end(); it ++)
	{
		Hash_Addr ha;
		memcpy(ha.hash, it->second->GetHash(), 20);
		ha.addr = it->second->GetSeed()->GetAnnounces();

		vec.push_back(ha);
	}
	return true;
}

bool CTaskMgr::GetCacheHash( set<HASH>& hashSet )
{
	map<HASH, CTask *>::iterator it;
	CAutoLock Lock(m_task_map_lock);
	for(it = m_task_map.begin(); it != m_task_map.end(); it ++)
	{
		if (it->second->m_Para.bCopyFullFile/* && it->second->m_Para.copystatus != 1*/)  // bCopyFullFile为离线任务
		{
			// 下载任务不清除
			continue;
		}
		if (it->second->IsPlaying())		// 正在播放的任务不清除
		{
			continue;
		}
		hashSet.insert(it->first);
	}
	return true;
}

//p2pµ˜”√µƒ–¥»Î ˝æ›∫Ø ˝
//modified by tangtao
// 2: –¥≥…π¶«“Œƒº˛ÕÍ≥…
// 1: –¥»Î”≤≈Ã≥…π¶
// 0: –¥»Îƒ⁄¥Ê≥…π¶
//-1: –¥”≤≈Ã ß∞‹
//-2: ÷ÿ∏¥ ˝æ›
//-3: ¥ÌŒÛ ˝æ›
//-4: Œﬁ–ß ˝æ›
//-5: Œﬁ¥À»ŒŒÒ
int CTaskMgr::WritePiece(const HASH& hash, KEY& key,const char* buf, QVOD_UINT64 len, unsigned int index, QVOD_UINT64 pos)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		if(task->IsValidPiecePacket(key,index,pos,len))
		{
			return task->WritePiece(key,buf, len, index, pos);
		}
		else
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1,"¥ÌŒÛ∞°,≤ª «Œ““‘«∞«Î«Ûµƒ∞¸\n");
#endif // _DEBUG
			return 0;
		}
	}
	return 0;
}

#ifdef SUPERNODE
//Server Cache”√µƒ∂¡»° ˝æ›
bool CTaskMgr::ReadPiece(const HASH& hash, KEY& key,char *buf, int len, unsigned int index, unsigned int start)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		IO* pIO = new IO;
		pIO->buf = buf;
		pIO->len  = len;
		pIO->flag = READ;
		pIO->key = key;
		pIO->hash = hash;
		pIO->pos = QVOD_INT64(index)*task->GetSeed()->GetPieceSize()+start;

		AutoPtr<CStringConvert> pStringCon = Wchar2Char(task->GetFilePath().c_str());
		char *pPath = pStringCon->GetAnsiData();

		bool result = IIoInterface::Instance()->AddIo(pIO,pPath[0]);
		if(result)
		{
			return true;
		}
		else
		{
			delete pIO;
			return false;
		}
	}
	return false;
}
#endif

//–¥”≤≈ÃΩ”ø⁄
int CTaskMgr::WritePiece(const HASH& hash, const char* buf, UINT len, QVOD_UINT64 pos)
{
 	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		const unsigned int nPieceSize = task->GetSeed()->GetPieceSize();
		if(nPieceSize == 0)
		{
			return 0;
		}
		int index = (int)(pos/nPieceSize);
		int start = (int)(pos%nPieceSize);
// 		if (0 != start)
// 		{
// 			return task->WriteAccPiece(buf,len,index,start);
// 		}
// 		else
// 		{
			return task->WriteData(buf, len, index, start);
// 		}
	}
	return -1;
}

//∂¡”≤≈ÃΩ”ø⁄
int CTaskMgr::ReadPiece(const HASH& hash, char *buf, UINT len, QVOD_UINT64 pos,bool bIsUpload)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		if(task->GetSeed()->GetPieceSize())
		{
			const unsigned int nPieceSize = task->GetSeed()->GetPieceSize();
			if (nPieceSize > 0)
			{
				return task->ReadData(buf, len, (UINT)(pos/nPieceSize), (UINT)(pos%nPieceSize),bIsUpload);
			}
		}
	}
	return -1;
}
#ifdef SUPERNODE
int CTaskMgr::DiskIoFinished(HASH& hash, KEY& key,char*& buf, int& len, unsigned int& index, unsigned int& start,int& iFlag)
{
	int iResult = -1;
	IO* pIO = NULL;
	bool bFlag = IIoInterface::Instance()->GetIo(pIO);
	if ( bFlag && pIO )	
	{
		hash = pIO->hash;
		AutoPtr<CTask> task;
		unsigned int piecelen = 0;
		if (FindTask(hash,&task) && (piecelen = task->GetSeed()->GetPieceSize()) != 0)
		{
			index = unsigned int(pIO->pos/piecelen);
			start = unsigned int(pIO->pos%piecelen);
			len = (int)pIO->len;
			key  = pIO->key;
			if(pIO->flag == WRITE)
			{
				delete[] pIO->buf;
			}
			else
			{
				buf = pIO->buf;	
			}
			iFlag = pIO->flag;
			iResult = pIO->result;
		}
		else
		{
			delete[] pIO->buf;
			iResult = -1;
		}
		delete pIO;		
	}
	return iResult;
}
#endif

int CTaskMgr::DownNextPiece(const HASH& hash,KEY& key,request& req)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->DownNextPiece(key,req);
	}
	return -1;
}

bool CTaskMgr::DownNextPiece(const HASH& hash,KEY& key,httpiecereq& hreq)//HTTP FOR P2P
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->DownNextPiece(key,hreq);
	}
	return false;
}

#ifdef NETBAR
bool CTaskMgr::IsNeedDownload(HASH& hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->IsNeedDownload();
	}
	return false;
}
#endif

bool CTaskMgr::TaskIsFinish(const HASH& hash, bool bCheckFileExist)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->TaskIsFinish(bCheckFileExist);
	}

	return false;
}

bool CTaskMgr::GetTaskBitField(const HASH& hash, unsigned char*bitFiled, unsigned int &len)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		if(NULL == bitFiled)
		{
			len = task->GetMemFile()->GetBitFieldLen();
		}
		else
		{
			unsigned char* pMemBit = task->GetMemFile()->GetMemBit();
			unsigned int iMembitLen = task->GetMemFile()->GetBitFieldLen();
			if (NULL == pMemBit || len < iMembitLen)
			{
				return false;
			}
			memcpy(bitFiled, pMemBit,iMembitLen );
		}
		return true;
	}	
	return false;	
}

int CTaskMgr::AddOnePeer(const HASH& hash, const KEY& key, unsigned int PublicIP,unsigned short PublicPort
						 ,bool isDownPeer,unsigned char nattype,unsigned char nettype,char PeerID[20]
						 ,PEERTYPE peertype)
{
	AutoPtr<CTask> pTask;
	if(FindTask(hash,&pTask))
	{
		return pTask->ProcessQueryPeers()->AddPeer(key, PublicIP, PublicPort,isDownPeer, nattype,nettype, PeerID,peertype);
	}
	return -1;	
}

int CTaskMgr::AddOneQvodServer(const HASH &hash,KEY& key,unsigned char natType,unsigned char netType,char szPeerID[20],
							   QVOD_UINT64 iBeginPos,unsigned int iDataLen,PEERTYPE peerType /*= NORMAL*/ )
{
	AutoPtr<CTask> pTask;
	if (FindTask(hash,&pTask))
	{
		return pTask->ProcessQueryPeers()->AddServer(key,true,natType,netType,szPeerID,iBeginPos,iDataLen,peerType);
	}
	return -1;
}

int CTaskMgr::DelOnePeer(const HASH& hash, KEY& key)
{
	AutoPtr<CTask> pTask;
	if(FindTask(hash,&pTask))
	{
		AutoPtr<CPeer> pPeer;
		if ( pTask->ProcessQueryPeers()->FindPeer(key,&pPeer) )
		{
			pTask->CancelPeerIndexInfo(key);

			unsigned int xSecUpBufsize = 0 ;
			return pTask->ProcessQueryPeers()->DelPeer(key,pTask->m_xSecDownBufSize,xSecUpBufsize);
		}
	}
	return -1;
}

int CTaskMgr::QueryPeerStatus(const HASH& hash,const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->QueryStatus();
		}
	}
	return -1;
}
bool CTaskMgr::UpdatePeerStatus(const HASH& hash,const KEY& key,int status)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->UpdateStatus(status);
		}
	}
	return false;
}
int CTaskMgr::CreatePeerBitField(const HASH& hash, KEY& key, unsigned char*bit, int len)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			pPeer->CreateBitField(bit, len);
			return 0;
		}
	}
	return -1;
}

int CTaskMgr::SetPeerBitField(const HASH& hash, KEY& key, unsigned int index)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			pPeer->SetBitField(index);
			return 0;
		}
	}
	return -1;
}

int CTaskMgr::ResetPeerBitField(const HASH &hash,KEY &key,unsigned int index)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			pPeer->ResetBitField(index);
			return 0;
		}
	}
	return -1;
}

int CTaskMgr::GetBitFieldLen(const HASH& hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->GetMemFile()->GetBitFieldLen();
	}
	return -1;
}

int CTaskMgr::SetPeerID(const HASH& hash, const KEY& key, char pid[20])
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			pPeer->SetPeerID(pid);
			return 0;
		}
	}
	return -1;
}

bool CTaskMgr::SetPeerType(const HASH& hash, const KEY& key,USERTYPE type)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			pPeer->SetPeerType(type);
			return 0;
		}
	}

	return false;
}

bool CTaskMgr::SetPeerCacheServer(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			pPeer->SetPeerCacheServer();
			return true;
		}
	}
	return false;
}

bool CTaskMgr::IsCacheServer(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->IsCacheServer();
		}
	}
	return false;
}
bool CTaskMgr::IsMinerServer(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->IsMinerServer();
		}
	}
	return false;
}
int CTaskMgr::GetPeerID(const HASH& hash, const KEY& key, char pid[20])
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			char* peerid = pPeer->GetPeerID();
			memcpy(pid,peerid,20);
			return 0;
		}
	}
	return -1;
}

int CTaskMgr::GetPeerType(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->GetUserType();
		}
	}

	return -1;
}

unsigned char CTaskMgr::GetPeerTimeoutCount(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->GetTimeoutCount();
		}
	}
	return 0;
}

unsigned char CTaskMgr::GetPeerNatType(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->GetNatType();
		}
	}
	return 0;
}

unsigned char CTaskMgr::GetPeerNetType(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->GetNetType();
		}
	}
	return 0;
}

int CTaskMgr::GetPeerVersion(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->GetVersion();
		}
	}
	return 0;
}

bool CTaskMgr::IsDownPeer(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->IsDownMode();
		}
	}
	return false;
}

bool CTaskMgr::IsSuperPeerNode(const HASH& hash, const KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->isSuperNode();
		}
	}
	return false;
}

int CTaskMgr::GetPeerNum(const HASH & hash,PEERTYPE type/* = ALLTYPE*/)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->ProcessQueryPeers()->GetPeerNum(type);
	}

	return 0;
}

bool CTaskMgr::IsConnect(const HASH & hash, unsigned int ip, short port)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> peer;
		if(task->ProcessQueryPeers()->FindPeer(ip, port,&peer))
		{				
			return true;
		}
	}
	return false;
}

int CTaskMgr::IsDownLoadingTask(HASH& hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		if(task->m_Para.type == BTUPLOADTASK || task->m_Para.type == TVUPLOADTASK
			||((task->m_Para.type == BTDOWNLOADTASK || task->m_Para.type == TVDOWNLOADTASK) && task->TaskIsFinish()))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return -1;
}

int CTaskMgr::SetPlayFileIndex(HASH& hash, int index)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->SetFileIndex(index);
	}
	return -1;
}

bool CTaskMgr::SetPeerBitFiled(const HASH &hash, const KEY &key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->ZeroBitField(task->GetMemFile()->GetBitFieldLen());
		}
	}
	return false;
}

bool CTaskMgr::CheckPeerHashIndex(HASH &hash, KEY &key, unsigned int index)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->CheckIndex(index);
		}
	}
	return false;
}

bool CTaskMgr::PeerNeedSendHaveMsg(HASH &hash, KEY &key, unsigned int index)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->NeedSendHaveMsg(index);
		}
	}
	return false;
}

#ifdef SUPERNODE
bool CTaskMgr::SetUploadPara(HASH *pHash,UCHAR UploadLmt,UCHAR MaxPeers,UCHAR MaxSeedsRef,UCHAR bSetAssertdownload,
							 UCHAR bSetAssertlink,std::string &strPath)
{
	if ( pHash != NULL )
	{
		AutoPtr<CTask> pTask;
		if ( CTaskMgrInterFace::Instance()->FindTask(*pHash,&pTask) )
		{
			pTask->SetUploadPara(UploadLmt,MaxPeers,MaxSeedsRef,bSetAssertdownload,bSetAssertlink);
			if ( !g_pcachelib )
			{
				pTask->SetUpSpeed(UploadLmt);
			}
			CDbTaskConfig::Instance()->UpdateTask(pTask->m_Para);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		CAutoLock lock(m_task_map_lock);
		std::map<string, VCTTASK>::iterator mapIter;
		std::vector<CTask*>::iterator vecIter;
		mapIter = m_task_path.begin();
		CDbTaskConfig::Instance()->TransactionBegin();
		while ( mapIter != m_task_path.end() )
		{
			if ( mapIter->first.find(strPath) == 0 )
			{
				vecIter = mapIter->second.begin();
				while (vecIter != mapIter->second.end())
				{
					(*vecIter)->SetUploadPara(UploadLmt,MaxPeers,MaxSeedsRef,bSetAssertdownload,bSetAssertlink);
					if ( !g_pcachelib )
					{
						(*vecIter)->SetUpSpeed(UploadLmt);
					}
					CDbTaskConfig::Instance()->UpdateTask((*vecIter)->m_Para);
					vecIter++;
				}
			}
			mapIter++;
		}
		CDbTaskConfig::Instance()->TransactionCommit();
	}
	return true;
}

int CTaskMgr::UpdateAllTaskUploadLimit()
{
	if ( g_pcachelib )
	{
		CAutoLock lock(m_task_map_lock);
		map<string,CTask*>::iterator iter = m_task_map.begin();
#ifdef KUAIWAN
		CDbTaskConfig::Instance()->TransactionBegin();
#endif
		while ( iter != m_task_map.end() )
		{
			CTask * pTask = iter->second;
			pTask->SetUpSpeed(g_task_UpRateLimit<<2);
#ifdef KUAIWAN
			pTask->m_Para.uploadlimit = g_task_UpRateLimit;
			CDbTaskConfig::Instance()->UpdateTask(pTask->m_Para);
#endif
			iter++;
		}
#ifdef KUAIWAN
		CDbTaskConfig::Instance()->TransactionCommit();
#endif
	}
	return 0;
}
#endif

////////////////////////==≤È—ØΩ”ø⁄ µœ÷==//////////////////////////////////////////////////////////////
int CTaskMgr::ProcessQueryTaskBypath(string path, BinaryWriter &bw)
{
	map<string, VCTTASK>::iterator it;

	CAutoLock Lock(m_task_path_lock);

	it = m_task_path.begin();
	for (  ; it != m_task_path.end() ; it++ )
	{
		if ( path.find(it->first) != std::string::npos)
		{
			VCTTASK &vt = it->second;
			vector<CTask *>::iterator vit;
			for(vit = vt.begin(); vit != vt.end(); vit ++)
			{
				CTask *pTask = *vit;
				if ( pTask->GetFilePath() + pTask->GetFileName() == path)
				{
#ifdef SUPERNODE
					SerializeTaskInfoEx(&pTask,bw);
#else
					SerializeTaskInfo(&pTask,bw);
#endif
				}				
			}
		}
	}

	return 0;
}

int CTaskMgr::ProcessQueryDownloadTasks(BinaryWriter &bw)
{
	map<HASH, CTask*>::iterator it;
	CAutoLock Lock(m_task_map_lock);
	unsigned int iTaskNum = 0;
	for(it = m_task_map.begin(); it != m_task_map.end(); it++)
	{
		CTask *pTask = it->second;
		if (pTask->GetFileName().find(".qdata") != std::string::npos)
		{
			continue;
		}
		if( TVDOWNLOADTASK == pTask->GetTaskType() && !pTask->GetHide() || 
			BTDOWNLOADTASK == pTask->GetTaskType() )
		{
			iTaskNum++;
		}
	} 
	//œ»∞—Taskµƒ∏ˆ ˝∑¢π˝»•
	bw << htonl(iTaskNum);
	for(it = m_task_map.begin(); it != m_task_map.end(); it ++)
	{
		CTask *pTask =  it->second;
		if (pTask->GetFileName().find(".qdata") != std::string::npos)
		{
			continue;
		}
		if(TVDOWNLOADTASK == pTask->GetTaskType() && !pTask->GetHide() || BTDOWNLOADTASK == pTask->GetTaskType())
		{
			SerializeTaskInfo(&pTask,bw);
		}
	}
	return 0;
}


int CTaskMgr::ProcessQueryPeers(HASH& hash, BinaryWriter &bw)
{
	AutoPtr<CTask> pTask;
	if ( !FindTask(hash,&pTask) )
	{
		return -1;
	}
	CPeerGroup* pPeers = pTask->ProcessQueryPeers();
	unsigned int iNum = QVOD_MIN2(pPeers->GetPeerNum(),255);
	for (  unsigned int i = 0 ; i < iNum ; i ++ )
	{
		AutoPtr<CPeer> pPeer;
		if ( pPeers->GetPeer(i,&pPeer))
		{
			bw.writeRaw(pPeer->GetPeerID(),20);
			unsigned int iLen = (unsigned int)(pPeer->GetUpLen() >> 32);
			bw << htonl(iLen);
			bw << htonl((unsigned int)(pPeer->GetUpLen()));

			iLen = (unsigned int) (pPeer->GetDownLen() >> 32);
			bw << htonl(iLen);
			bw << htonl((unsigned int)(pPeer->GetDownLen()));

			bw << htonl(pPeer->GetUpRate());
			bw << htonl(pPeer->GetDownRate());
			bw << htonl(pPeer->GetPublicIP());

			//”…”⁄peerµƒ∂Àø⁄∫≈ «Õ¯¬Á◊÷Ω⁄–Úµƒ,¥À¥¶≤ªƒ‹‘⁄µ˜”√htons¡À,¡Ω¥Œhtons∫Û,‘Ÿ¡Ω¥Œntohs,≤ª“ª∂®ƒ‹ªπ‘≠ ˝æ›
			bw << pPeer->GetPublicPort();
			bw << char(pPeer->GetNatType());
			bw << char(0);
		}		
	}
	return 0;
}

int CTaskMgr::ProcessQueryFilesInfo(HASH& hash, BinaryWriter &bw)
{
	AutoPtr<CTask> pTask;
	if ( !FindTask(hash,&pTask) )
	{
		return -1;
	}
	
	//∑¢ÀÕ ˝æ›÷Æ«∞œ»∑¢ÀÕŒƒº˛∏ˆ ˝
	unsigned int iFileNum = pTask->m_vctFile.size();
	bw << htonl(iFileNum);
	//∑¢ÀÕ µÃÂ ˝æ›
	for( unsigned int i = 0;i< pTask->m_vctFile.size()&& i < 255;i++)
	{
		CQvodFile* pFile = pTask->m_vctFile[i];

		//Œƒº˛≥§∂»
		bw << htonl((unsigned long)(pFile->GetFileLen() >> 32));
		bw << htonl((unsigned long)pFile->GetFileLen());

		//µ±«∞µƒœ¬‘ÿ≥§∂»
		bw << htonl((unsigned long)(pFile->GetCurDownloadLen() >> 32));
		bw << htonl((unsigned long)pFile->GetCurDownloadLen());

		//◊‹œ¬‘ÿ≥§∂»
		bw << htonl((unsigned long)(pFile->GetTotalDownloadLen() >> 32));
		bw << htonl((unsigned long)pFile->GetTotalDownloadLen());

		//Œƒº˛–Ú∫≈,¥”0ø™ ºº∆À„;∂‘”⁄Qvod»ŒŒÒ,∫„Œ™0
		bw << ntohs(i);

		//±£¡Ù◊÷∂Œ
		bw << char(0);

		//Œƒº˛√˚
		std::string strfilename = "";
		strfilename = pFile->GetFileName();
		bw.writeRaw(strfilename.c_str(),strfilename.length()+1);
	}
	return 0;
}

int CTaskMgr::ProcessQueryBitfield(HASH& hash, QVOD_UINT64 playpos, BinaryWriter &bw)
{
	AutoPtr<CTask> pTask;
	if ( !FindTask(hash,&pTask) )
	{
		return -1;
	}

	if(pTask->GetMemFile()->GetMemBit())
	{
		bw << htonl(pTask->GetSeed()->GetPieceSize());
		bw << htonl(pTask->GetSeed()->GetPieceNum());
#ifdef NETBAR
		bw << htonl((unsigned long)(pTask->GetSuperNodeDownLen()>>32));
		bw << htonl((unsigned long)(pTask->GetSuperNodeDownLen()));
#else
		bw << htonl((unsigned long)(pTask->GetMemFile()->GetDownLoadedlen()>>32));
		bw << htonl((unsigned long)(pTask->GetMemFile()->GetDownLoadedlen()));
#endif
		bw << htonl(pTask->GetDownSpeed());
		
		//»˝∏ˆ±£¡Ù◊÷∂Œ,≤•∑≈∆˜≤…”√µƒ4◊÷Ω⁄∂‘∆Î
		bw << char(0);
		bw << char(0);
		bw << char(0);

#ifdef NETBAR
		AutoPtr<CPeer> pPeer;
		if (pTask->ProcessQueryPeers()->GetSuperNode(&pPeer))
		{
			int len = pPeer->GetBitFieldLen();
			bw.writeRaw((char *)(pPeer->GetBitField()), len);
		}
		else
		{
			int len = pTask->GetMemFile()->GetBitFieldLen();
			bw.writeRaw((char *)(pTask->GetMemFile()->GetMemBit()), len);
		}
#else
		int len = pTask->GetMemFile()->GetBitFieldLen();
		bw.writeRaw((char *)(pTask->GetMemFile()->GetMemBit()), len);
#endif
		//pTask->SetBuffingPos(playpos);
	}
	else
	{
		bw << 0;
		bw << 0;
		bw << 0;
		bw << 0;
		bw << 0;
		bw << 0;
	}
	return 0;
}

int CTaskMgr::ProcessCopyFirstPiece(HASH& hash, BinaryWriter &bw)
{
	AutoPtr<CTask> pTask;
	if ( !FindTask(hash,&pTask) )
	{
		return -1;
	}
	if(pTask->GetMemFile()->CheckIndex(0))
	{
		int blen = pTask->GetSeed()->GetPieceSize();
		QPtr::AutoArrayPtr<char> tmp = new char[blen + 1];
		if(NULL == tmp)
		{
			return -1;
		}
		pTask->ReadData(tmp, blen, 0, 0, false);
		//modified by tangtao 2010-06-22
		bw << htonl(blen);
		bw.writeRaw(tmp,blen);
	}
	else
	{
		bw << 0;
	}

	return 0;
}

int CTaskMgr::ProcessQueryTaskByHash(HASH& hash, BinaryWriter &bw)
{
	AutoPtr<CTask> task;
	if ( !FindTask(hash,&task) )
	{
		return -1;
	}
	SerializeTaskInfo(&task,bw);
	return 0;
}


int CTaskMgr::PrecessQueryTorrentByHash(HASH& hash,BinaryWriter &bw)
{
	CAutoLock Lock(m_task_map_lock);
	map<HASH, CTask*>::iterator it = m_task_map.find(hash);
	if(it == m_task_map.end())
	{
		return -1;
	}
	return 0;
}

void CTaskMgr::SerializeTaskInfo(CTask **pTask, BinaryWriter &bw)
{	
	HASH hash;
	memcpy(hash.data, (*pTask)->GetHash(), 20);
	std::string str = Hash2Char((*pTask)->GetHash());
	bw.writeRaw(str.c_str(),40);
	int tid = CHash2ID::Instance()->Get(hash);
	bw <<htonl(tid);
#ifndef SUPERNODE
	bw<<0;
	bw<<0;
#endif
	unsigned int tlen = (unsigned int)((*pTask)->GetSeed()->GetFileTotleLen()>>32);
	bw << htonl(tlen);
	tlen = (unsigned int)(*pTask)->GetSeed()->GetFileTotleLen();
	bw << htonl(tlen);
#ifdef NETBAR
	tlen = unsigned int((*pTask)->GetSuperNodeDownLen()>>32);
	bw << htonl(tlen);
	tlen = (unsigned int)(*pTask)->GetSuperNodeDownLen();
	bw << htonl(tlen);
#else
	QVOD_INT64 iDownLen = (*pTask)->GetMemFile()->GetDownLoadedlen();
	tlen = (unsigned int)(iDownLen>>32);
	bw << htonl(tlen);
	tlen = (unsigned int)(iDownLen);
	bw << htonl(tlen);
#endif
	bw << 0;//htonl((unsigned int)(*pTask)->GetCurUplen());//htonl((*pTask)->GetSeed()->GetCurUplen());
	tlen = (unsigned int)(*pTask)->GetCurDownLen();
	tlen -= 0;//(unsigned int)((QVOD_INT64)(*pTask)->GetCloudServerCurrentDownload() << 10);
	bw << htonl(tlen);//htonl((*pTask)->GetSeed()->GetCurDownLen());
	bw << 0;//htonl((*pTask)->GetUpSpeed());
 	tlen = 0;//CHttpAgentInterface::Instance()->GetGlobalDownSpeed((*pTask)->GetHash());
 	tlen += (*pTask)->GetDownSpeed();
	bw << htonl(tlen);
	bw << 0;//htonl((*pTask)->GetCloudServerCurrentDownload());
	bw << 0;//htonl((*pTask)->GetCloudServerTotalDownload());
	bw << 0;//htonl((*pTask)->GetCloudServerDownSpeed());
	bw << 0;//char((*pTask)->GetUpLoadLimit());
	bw << 0;//char((*pTask)->GetPeerLimit());
	bw << char((*pTask)->GetSeedLimit());
	bw << char((*pTask)->GetStatus());
	//bw << char((*pTask)->GetSeed());
	bw << char(QVOD_MIN2((*pTask)->ProcessQueryPeers()->GetPeerNum(),255));//ªÒ»°¡¨Ω” ˝
	bw << char((*pTask)->GetTaskType());//char((*pTask)->GetBaiduFileFlag() == 1 ? BDUPLOADTASK:(*pTask)->GetTaskType());
	bw << 0;//char((*pTask)->m_bShared);
	bw << short(htons((*pTask)->m_vctFile.size()));
	bw << char((*pTask)->m_Para.bcheckkey);
	bw << 0;//char((*pTask)->GetCloudServerStatus());
	bw << 0;//char((*pTask)->GetCloudFileExistFlag());
	bw << (*pTask)->GetTaskErrorMask();				//
	bw << htons(0);									//◊÷Ω⁄∂‘∆Îµƒ±£¡Ù◊÷∂Œ;
	bw << htonl((*pTask)->GetCreateTime());
	std::string strfilename;
	if((*pTask)->GetTaskType() == BTDOWNLOADTASK)
	{
		if((*pTask)->m_vctFile.size() == 1)
		{
			strfilename = (*pTask)->GetFilePath() + (*pTask)->GetSeed()->GetFileName();
		}
		else
		{
			strfilename = (*pTask)->GetFilePath() + (*pTask)->GetSeed()->GetDownFileName();
		}
	}
	else
	{
		strfilename = (*pTask)->GetFilePath()+(*pTask)->GetFileName();
	}

	strfilename += "|";
	strfilename += (*pTask)->GetSeed()->GetAdUrl();
	bw.writeRaw(strfilename.c_str(),strfilename.length()+1);
}

void CTaskMgr::SerializeTaskInfoEx(CTask **pTask, BinaryWriter &bw)
{ 
	HASH hash;
	memcpy(hash.data, (*pTask)->GetHash(), 20);
	std::string str = Hash2Char((*pTask)->GetHash());
	bw.writeRaw(str.c_str(),40);
	int tid = CHash2ID::Instance()->Get(hash);
	bw <<htonl(tid);
#ifndef SUPERNODE
	bw<<0;
	bw<<0;
#endif
	unsigned int tlen = (unsigned int)((*pTask)->GetSeed()->GetFileTotleLen()>>32);
	bw << htonl(tlen);
	tlen = (unsigned int)(*pTask)->GetSeed()->GetFileTotleLen();
	bw << htonl(tlen);
	tlen = (unsigned int)((*pTask)->GetMemFile()->GetDownLoadedlen()>>32);
	bw << htonl(tlen);
	tlen = (unsigned int)(*pTask)->GetMemFile()->GetDownLoadedlen();
	bw << htonl(tlen);
	bw << 0;//htonl((unsigned int)((*pTask)->GetCurUplen())); //htonl((*pTask)->GetSeed()->GetCurUplen());
	bw << htonl((unsigned int)((*pTask)->GetCurDownLen()));
	bw << 0;//htonl((*pTask)->GetUpSpeed());
	bw << htonl((*pTask)->GetDownSpeed());
	bw << 0;//char((*pTask)->GetUpLoadLimit());
	bw << 0;//char((*pTask)->GetPeerLimit());
	bw << char((*pTask)->GetSeedLimit());
	bw << char((*pTask)->GetStatus());
	bw << char((*pTask)->m_nlocalseeds);
	bw << char((*pTask)->ProcessQueryPeers()->GetPeerNum());//ªÒ»°¡¨Ω” ˝
	bw << char((*pTask)->GetTaskType());
	bw << 0;//char((*pTask)->m_bShared);
	bw << short(htons((*pTask)->m_vctFile.size()));
	bw << char(0); //char((*pTask)->m_taskcfg.bcheckkey);
#ifdef QVODNET
	bw << char(0);
#endif
#ifdef SUPERNODE 
	bw << char((*pTask)->GetAssertdownload());
	bw << char((*pTask)->GetAssertlink());
#endif
	std::string strfilename = (*pTask)->GetFileName();
	strfilename += "|";
	strfilename += (*pTask)->GetSeed()->GetAdUrl();
	bw.writeRaw(strfilename.c_str(),strfilename.length()+1);
	//bw.writeRaw(strfilename.c_str(),strfilename.size()+1);
	// bw << strfilename;
}

/////////////////////////////////////////
CTask * CTaskMgr::FindTask(const unsigned char *hash)
{
	CTask *task = NULL;
	HASH key;
	memcpy(key.data, hash, 20);
	CAutoLock Lock(m_task_map_lock);
	map<HASH, CTask *>::iterator it = m_task_map.find(key);
	if(it != m_task_map.end())
	{
		task = it->second;
	}
	return task;
}


bool CTaskMgr::TaskIsExist(HASH &hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return true;
	}
	return false;
}

bool CTaskMgr::IsCopyFullFile(const HASH& hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->m_Para.bCopyFullFile;
	}
	return false;
}
int	CTaskMgr::GetTaskStatus(HASH &hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->GetStatus();
	}
	return -1;
}

int CTaskMgr::SetTaskStatus(HASH& hash, unsigned char status,bool bIsDeleteFile/* = false*/)
{
	AutoPtr<CTask> task;
	if ( !FindTask(hash,&task) )
	{
		return -1;
	}
#ifdef MONGOTV
	//»Ù «‘›Õ£ªÚ…æ≥˝£¨¡¥Ω”Œ™√¢π˚tv◊‘”–cdn£¨‘Ú∑¢ÀÕ◊¢œ˙∞¸
	if (CTask::STOP == status || CTask::PAUSE == status)
	{
		CMongoTvCdn::Instance()->AddRequest(task->m_Para.strHttpOrgUrl.c_str(),task->IsUrlOtherCdn(),CMongoTvCdn::LOGOUT);
	}
#endif
	switch(status)
	{
	case CTask::STOP:
		{
			SetTaskPlaying(hash, false);
			if ( !bIsDeleteFile )
			{
				task->SetReservedFile();
			}
			task->Stop();
			DelOneTask(hash);
			break;
		}
	case CTask::PAUSE:
		{
			task->Pause();
			break;
		}
	case CTask::RUNNING:
		{
			task->Start();
			break;
		}
	case CTask::PLAY:
		{
			task->Start();
			task->SetStartPlay();
			break;
		}
	case CTask::WAIT:
		{
			task->m_Para.status = CTask::WAIT;
		}
		break;
	default:
		return -1;
	}
	return 0;
}

//∆Ù∂Ø/‘›Õ£À˘”–µƒ»ŒŒÒ
void CTaskMgr::StartAllTask()
{
	map<HASH, CTask *>::iterator it;
	CAutoLock Lock(m_task_map_lock);

	for(it = m_task_map.begin(); it != m_task_map.end(); it++)
	{
		CTask* pTask = it->second;
		if (pTask->GetFileName().find(".qdata") != std::string::npos)
		{
			continue;
		}
		pTask->Start();
	}
}

void CTaskMgr::PauseAllTask()
{
	map<HASH, CTask *>::iterator it;
	CAutoLock Lock(m_task_map_lock);

	for(it = m_task_map.begin(); it != m_task_map.end(); it++)
	{
		CTask* pTask = it->second;
		if ( !pTask->GetHide() /*&& !pTask->IsAccTask()*/)
		{
			pTask->Pause();
		}
	}
}

void CTaskMgr::SetDownSpeed(HASH &hash,  int speed)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->SetDownSpeed(speed);
	}
}

string CTaskMgr::GetReferUrl(const HASH &hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->GetReferUrl();
	}
	return 0;
}

void CTaskMgr::CheckTimeStamp()
{
	CAutoLock lock(m_task_path_lock);
	CTask* pTask;
	int ndownloadingtask = 0;
	int nrate = 0;
	int waitingtasks = 0;
	std::vector<CTask*>::iterator it;
	std::map<std::string,VCTTASK>::iterator mapit;
	static QVOD_DWORD dwTimestamp = QvodGetTime();
	QVOD_DWORD dwNow = QvodGetTime();
	for(mapit = m_task_path.begin();mapit != m_task_path.end();mapit++)
	{
		for(it = mapit->second.begin();it!=mapit->second.end();)
		{
			pTask = *it;
			if (pTask->GetTaskType()>=HTTPUPTASK)	// 上传任务忽略
			{
				it++;
				continue;
			}
			if (g_cfg.m_bVideoBuffering)		// 有播放任务正在缓冲，则忽略本次非播放任务
			{
				if (! pTask->IsPlaying())
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CTaskMgr::CheckTimeStamp() Ignore download! hash=%s", Hash2Char(pTask->m_Para.hash).c_str());
#endif
					continue;
				}
			}
			pTask->CheckTimeStamp();
			pTask->UpdateDownloadMode();
#ifndef _360MOBILEMGR			
			if (pTask->GetStatus() == CTask::RUNNING )
			{
				// 根据时长大小设置文件码率
				if (pTask->GetPlayFileRate() == 0 && pTask->GetFileSize() > 0 && pTask->GetVideoDuration() > 0)
				{
					pTask->SetPlayFileRate(pTask->GetFileSize()*8/pTask->GetVideoDuration());
				}
#ifdef MONGOTV
				//»Ù»ŒŒÒµ±«∞‘⁄‘À––£¨Œ¥œ¬‘ÿÕÍ£¨¡¥Ω”÷–Œ™√¢π˚tv◊‘”–cdn£¨‘ÚœÚcdn∑¢–ƒÃ¯∞¸
				if (dwNow - dwTimestamp > 60000)
				{
					if (!pTask->TaskIsFinish())
					{
						//œÚ“Ï≤Ωœﬂ≥Ãº”»Î“ªÃı–ƒÃ¯«Î«Û
						CMongoTvCdn::Instance()->AddRequest(pTask->m_Para.strHttpOrgUrl.c_str(),pTask->IsUrlOtherCdn(),CMongoTvCdn::KEEPALIVE);
					}
				}
#endif
			}
			
#endif
			it++;
		}
	}
	if (dwNow - dwTimestamp > 60000)
	{
		dwTimestamp = dwNow;
	}
}

void CTaskMgr::InterDownSize(const SPEEDATA& sdata)
{
	AutoPtr<CTask> task;
	if(FindTask(sdata.hash,&task))
	{
		task->InterDownSize(sdata.size, sdata.key);
	}
}

void CTaskMgr::InterUpSize(const SPEEDATA& sdata)
{
	AutoPtr<CTask> task;
	if(FindTask(sdata.hash,&task))
	{
		task->InterUpSize(sdata.size, sdata.key);
	}	
}



bool CTaskMgr::IsNeedCacheDownRequest(HASH& hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->IsNeedCacheDownRequest();
	}
	return false;
}

bool CTaskMgr::IsOvertakeDownSpeed(HASH& hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->IsOvertakeDownSpeed();
	}
	return false;
}

void CTaskMgr::UpdateNextDownTime(HASH& hash,int lastSize)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->UpdateNextDownTime(lastSize);
	}
}

int CTaskMgr::GetTaskType(const HASH& hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->GetTaskType();
	}
	return 0;
}

int CTaskMgr::GetPeerDownSpeed(HASH& hash,KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(key,&pPeer))
		{				
			return pPeer->GetDownRate();
		}
	}
	return 0;
}

int CTaskMgr::GetSomePeerDownSpeed( HASH& hash,PEERTYPE type )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->ProcessQueryPeers()->GetSomePeerDownSpeed(type);
	}
	return 0;
}

int CTaskMgr::GetDownSpeed(HASH &hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		if(memcmp(hash.data, task->GetHash(), 20) == 0)
		{
			return task->GetDownSpeed();
		}
	}
	return 0;
}


int CTaskMgr::GetBtTrackerAnnouse(HASH &hash, std::string &annouse, std::vector<std::string> &annous_map)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		annouse = task->GetSeed()->GetAnnounces();
		annous_map = task->GetSeed()->m_trackerlist;
		return (int)annous_map.size();
	}

	return 0;
}
bool CTaskMgr::GetPeerKeyByRandom(HASH &hash,char random[12],KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if (task->ProcessQueryPeers()->GetPeerKey(random,key))
		{
			return true;
		}
	}
	return false;
}
bool CTaskMgr::IsPeerAlreadyExist(HASH &hash,char random[12])
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		AutoPtr<CPeer> pPeer;
		if ( task->ProcessQueryPeers()->FindPeer(random,&pPeer))
			return true;
	}
	return false;
}

bool CTaskMgr::FindTask(const HASH& hash,CTask** task)
{
	CAutoLock Lock(m_task_map_lock);
	map<HASH, CTask *>::iterator iter = m_task_map.find(hash);
	if(iter != m_task_map.end())
	{
		*task = iter->second;
		(*task)->duplicate();
		return true;
	}
	return false;
}
bool CTaskMgr::GetTaskPathName(HASH& hash,string& pathname)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		pathname = task->GetFilePath()+task->GetFileName();
		return true;
	}
	return false;
}

string CTaskMgr::GetTaskPath(HASH& hash)
{
	string strPath ;
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		strPath = task->GetFilePath();
	}
	return strPath;
}

//added by tangtao 2010-03-29//////
CTask* CTaskMgr::FindTaskByPath( const std::string &strPath)
{
	CAutoLock lock(m_task_path_lock);
	std::map<std::string,VCTTASK>::iterator mapIter;
	std::vector<CTask*>::iterator vecIter;
	for(mapIter = m_task_path.begin();mapIter != m_task_path.end();mapIter++ )
	{
		if(strPath.find(mapIter->first) != 0 )
		{
			continue;
		}
		VCTTASK &vecTask = mapIter->second;
		for ( vecIter = vecTask.begin();vecIter != vecTask.end();vecIter++ )
		{
			if ( ((*vecIter)->GetFilePath() + (*vecIter)->GetFileName()) == strPath )
			{
				return *vecIter;
			}
		}
	}
	return NULL;
}
//ended////////////////////

bool CTaskMgr::DelTask(HASH& hash)
{
    
	CAutoLock Lock(m_task_map_lock);
	map<HASH, CTask *>::iterator iter;
	iter = m_task_map.find(hash);
	if(iter != m_task_map.end())
	{
		m_keyurl_hash_map_lock.Lock();
		map<string,HASH>::iterator itor = m_keyurl_hash_map.find(iter->second->m_Para.strHttpKeyUrl);
		if (itor != m_keyurl_hash_map.end())
		{
			m_keyurl_hash_map.erase(itor);
		}
		m_keyurl_hash_map_lock.Unlock();

		iter->second->release();
		m_task_map.erase(iter);
        
#ifdef QVODDEBUG
        _OUTPUT(INFO, "CTaskMgr::DelTask hash=%s",Hash2Char(hash.data).c_str());
#endif
		return true;
	}
	return false;
}

int CTaskMgr::GetTotalTaskNum()
{
	return (int)m_task_map.size();
}

int CTaskMgr::GetDownTaskNum()
{
	int num = 0;
	map<HASH, CTask *>::iterator it;
	CAutoLock Lock(m_task_map_lock);
	for(it = m_task_map.begin(); it != m_task_map.end(); it ++)
	{
		if((it->second->GetTaskType() == TVDOWNLOADTASK || it->second->GetTaskType() == BTDOWNLOADTASK) &&
			!it->second->TaskIsFinish() && it->second->GetStatus() == CTask::RUNNING && !it->second->GetHide() )		
		{
			num++;
		}
	}
	return num;
}

int CTaskMgr::GetRunTaskNum()
{
	int num = 0;
	map<HASH, CTask *>::iterator it;
	CAutoLock Lock(m_task_map_lock);
	for(it = m_task_map.begin(); it != m_task_map.end(); it ++)
	{
		if(CTask::RUNNING == it->second->GetStatus())
		{
			num ++;
		}
	}
	return num;
}

int CTaskMgr::DeleteTaskByFolder(std::string strfolder)
{
	std::vector<HASH> vecHash;
	CAutoLock lock(m_task_path_lock);
	map<std::string, VCTTASK>::iterator iter=m_task_path.begin();
	for ( ; iter != m_task_path.end() ;++iter )
	{
		if ( iter->first.find(strfolder) != 0 )
		{
			continue;
		}
		VCTTASK &vecTask = iter->second;
		VCTTASK::iterator	vecIter;
		for ( vecIter = vecTask.begin();vecIter != vecTask.end();vecIter++ )
		{
			if ( TVUPLOADTASK == (*vecIter)->GetTaskType() )
			{
				HASH hash( (*vecIter)->GetHash());
				vecHash.push_back(hash) ;
			}
		}
	}

	for ( unsigned int i = 0 ; i < vecHash.size() ; i++ )
	{
		SetTaskStatus(vecHash[i],CTask::STOP,false);
	}
	
	iter = m_task_path.begin();
	while ( iter != m_task_path.end() )
	{
		if ( iter->first.find(strfolder) != 0 )
		{
			++iter;
			continue;
		}
		VCTTASK &vecTask = iter->second;
		if ( 0 == vecTask.size() )
		{
			m_task_path.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
	return 0;
}

unsigned int CTaskMgr::GetPeerCanReqNum(HASH& hash,KEY& key)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return REQUEST_NUM - task->GetCurDownNum(key);
	}
	return 0;
}

bool CTaskMgr::CancelTimeoutReq(HASH& hash,KEY& key,int index,int begin,int length)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->CancelTimeoutReq(key,index,begin,length);
	}
	return false;
}

QVOD_INT64 CTaskMgr::GetTotalAccFileSize()
{
	CAutoLock Lock(m_task_map_lock);
	QVOD_INT64 totalAccTaskSize = 0;
	std::map<HASH,CTask*>::iterator mapIter = m_task_map.begin();
	while(mapIter != m_task_map.end())
	{
		if (mapIter->second->GetFileName().find(".qdata") != std::string::npos)
		{
			totalAccTaskSize += mapIter->second->GetFileSize();
		}
		mapIter++;
	}
	return totalAccTaskSize;
}

bool CTaskMgr::InterTaskHttpDownLen(HASH& hash,int len)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->InterHttpDownLen(len);
		return true;
	}
	return false;
}

bool CTaskMgr::FullPeerBitField(HASH& hash,KEY& key)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->FullPeerBitField(key);
	}
	return false;
}

int CTaskMgr::GetFilePieceSize(const HASH &hash)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetSeed()->GetPieceSize();
	}
	return 0;
}

QVOD_UINT64 CTaskMgr::GetFileSize(const HASH& hash)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetFileSize();
	}
	return 0;
}

QVOD_INT64 CTaskMgr::GetDownloadedSize(const HASH& hash )
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetDownloadedSize() ;
	}
	return 0;
}

int CTaskMgr::GetFilePieceNum(HASH &hash)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetPieceNum();
	}
	return 0;
}

QVOD_UINT64 CTaskMgr::QueryFileLen(const HASH &hash,int index)
{
	AutoPtr<CTask> pTask;
	if ( FindTask( hash,&pTask) )
	{
		return pTask->GetSeed()->GetFileLen(index);
	}
	return -1;
}


void CTaskMgr::ProcessQueryReferUrl(HASH &hash,BinaryWriter &Out)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		string strRefer = pTask->GetReferUrl();
		if ( strRefer != "")
		{
			Out.writeRaw(strRefer.c_str(),strRefer.length()+1);
		}
	}
}

int CTaskMgr::SetTaskReferUrl(const HASH &hash,const std::string &strRef)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->SetReferUrl(strRef);
	}
	return -1;
}

long CTaskMgr::GetCreateTime(const HASH &hash)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetCreateTime();
	}
	return -1;
}

int CTaskMgr::SetDownloadPos(const HASH &hash,QVOD_UINT64 iPos,bool fromUser)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->SetDownloadPos(iPos,fromUser);
	}
	return -1;
}

int CTaskMgr::QueryDownloadPos(const HASH &hash,QVOD_UINT64 &iPos)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetDownloadPos(iPos);
	}
	return -1;
}

int CTaskMgr::SetPlayFileIndexPos(const HASH &hash,QVOD_UINT64 iPos)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->SetPlayFileIndexPos(iPos);
	}
	return -1;
}

bool CTaskMgr::IsIndexDownload(const HASH& hash, int index)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->IsIndexDownload(index);
	}
	return false;
}

QVOD_INT64 CTaskMgr::GetDragPos(const HASH& hash)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetDragPos();
	}
	return 0;
}


int CTaskMgr::ProcessSetStopPlay(const HASH &hash)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->SetStopPlay();
	}
	return -1;
}

QVOD_INT64 CTaskMgr::ProcessPeerQueryContinueLen(const HASH &hash,QVOD_INT64 pos)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->QueryContinueLen(pos);
	}
	return 0;
}

bool CTaskMgr::IsTaskPlaying(const HASH &hash)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->IsPlaying();
	}
	return false;
}

int CTaskMgr::SetPlayFileRate(const HASH &hash,unsigned int iRate)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->SetPlayFileRate(iRate);
	}
	return -1;
}

int CTaskMgr::GetPlayFileRate(const HASH& hash )
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetPlayFileRate();
	}
	return 0;
}

int CTaskMgr::SetPlayFilePos(const HASH &hash,QVOD_INT64 pos)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->SetPlayFilePos(pos);
	}
	return -1;
}

QVOD_INT64 CTaskMgr::GetPlayFilePos(const HASH &hash )
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetPlayFilePos();
	}
	return 0;
}

QVOD_INT64 CTaskMgr::GetUnDownPosNearPlayPos(const HASH& hash )
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->GetUnDownPosNearPlayPos();
	}
	return 0;
}

int CTaskMgr::GetDownloadingTasks( list<HASH>& hashList )
{
	CAutoLock lock(m_task_map_lock);
	for (map<HASH, CTask *>::iterator mapIter = m_task_map.begin(); mapIter != m_task_map.end(); ++mapIter)
	{
		if (mapIter->second->GetTaskType() != BTUPLOADTASK &&	//∑«π≤œÌ»ŒŒÒ
			!mapIter->second->GetHide() &&						//∑«“˛≤ÿ»ŒŒÒ
			mapIter->second->IsDownlloadTask() &&			// 是下载任务
			!mapIter->second->TaskIsFinish() &&					// 任务是否下载完成
			mapIter->second->GetStatus() == CTask::RUNNING)		// 是否正在下载
		{
			hashList.push_back(mapIter->first);
		}
	}
	return hashList.size();
}

void CTaskMgr::GetPlayingTasks(list<HASH>& hashList)
{
	CAutoLock lock(m_task_map_lock);
	for (map<HASH, CTask *>::iterator mapIter = m_task_map.begin(); mapIter != m_task_map.end(); ++mapIter)
	{
		if (mapIter->second->GetTaskType() != BTUPLOADTASK &&	// BT任务
			!mapIter->second->GetHide() &&						// 是否隐藏任务
			!mapIter->second->TaskIsFinish() &&					// 是否完成任务
			mapIter->second->GetStatus() == CTask::RUNNING &&   // 是否下载状态
			mapIter->second->IsPlaying())						
		{
			hashList.push_back(mapIter->first);
		}
	}
}

bool CTaskMgr::IsExistEmergencyPiece(const HASH& hash)
{
	AutoPtr<CTask> pTask;
	if ( FindTask(hash,&pTask) )
	{
		return pTask->IsExistEmergencyPiece();
	}
	return false;
}

int CTaskMgr::GetPeerIspID(const HASH &hash,const KEY &key,unsigned char* pIspID)
{
	AutoPtr<CTask> pTask;
	if( FindTask(hash,&pTask) )
	{
		return pTask->GetPeerIspID(key,pIspID);
	}
	return 1;
}

int CTaskMgr::SetPeerIspID(const HASH &hash,const KEY &key,unsigned char *pIspID)
{
	AutoPtr<CTask> pTask;
	if( FindTask(hash,&pTask) )
	{
		return pTask->SetPeerIspID(key,pIspID);
	}
	return 1;
}

bool CTaskMgr::IsPlayTaskOptimizeUpload()
{
	map<HASH, CTask *>::iterator it;
	CAutoLock Lock(m_task_map_lock);
	for(it = m_task_map.begin(); it != m_task_map.end(); it ++)
	{
		if(!it->second->TaskIsFinish() && it->second->GetStatus() == CTask::RUNNING && !it->second->GetHide() &&
			(it->second->GetTaskType() == TVDOWNLOADTASK || it->second->GetTaskType() == BTDOWNLOADTASK))
		{
			if (it->second->IsOptimizeUpload())
			{
				return true;
			}
		}
	}
	return false;
}

//ªÒ»°taskœ¬‘ÿ≥§∂»
QVOD_INT64 CTaskMgr::GetTaskTotalDownloadLen(const HASH& hash)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->GetMemFile()->GetDownLoadedlen();
	}
	return 0;
}

int CTaskMgr::GetTaskCacheServerDownloadSpeed(HASH &hash,map<KEY,int> &mapCacheSpeed)
{
	int speed = 0;
	AutoPtr<CTask> pTask;
	if (FindTask(hash,&pTask))
	{
		speed = pTask->ProcessQueryPeers()->GetCacheServerDownloadSpeed(mapCacheSpeed);
	}
	return speed;
}

int CTaskMgr::GetTaskCacheServerDownloadSpeed(HASH &hash)
{
	int speed = 0;
	AutoPtr<CTask> pTask;
	if (FindTask(hash,&pTask))
	{
		speed = pTask->ProcessQueryPeers()->GetCacheServerDownloadSpeed();
	}
	return speed;
}

int CTaskMgr::GetDownloadingAndPlayingTaskNum()
{
	int iNum = 0;
	map<HASH, CTask *>::iterator iter;
	CAutoLock lock(m_task_map_lock);
	for(iter = m_task_map.begin(); iter != m_task_map.end(); iter ++)
	{
		if ((iter->second->IsPlaying()) && !(iter->second->TaskIsFinish()))
		{
			iNum++;
		}
	}
	return iNum;
}

int CTaskMgr::GetOneWaitingTaskHash(HASH &hash)
{
// 	if (IAppFrame::Instance()->GetConnections() > 0)
// 	{
// 		//»Áπ˚≤•∑≈∆˜ΩÁ√Ê¥Ê‘⁄,‘ÚΩ´∆Ù∂Øœ¬“ª∏ˆ»ŒŒÒµƒ¬ﬂº≠Ωª”–≤•∑≈∆˜∏∫‘,∑Ò‘Ú≥ˆœ÷»ŒŒÒ¡–±Ì≤ª∞¥’’À≥–Úœ¬‘ÿµƒbug;
// 		return 1;
// 	}
	int result = -1;
	map<HASH,CTask* >::iterator iter;
	m_task_map_lock.Lock();
	iter = m_task_map.begin();
	while ( iter != m_task_map.end())
	{
		if (iter->second->m_Para.status == CTask::WAIT)
		{
			hash = iter->first;
			result = 0;
			break;
		}
		iter++;
	}
	m_task_map_lock.Unlock();
	return result;
}

bool CTaskMgr::FindHash( string strKeyurl,HASH& hash )
{
	CAutoLock lock(m_keyurl_hash_map_lock);
	map<string, HASH>::iterator itor = m_keyurl_hash_map.find(strKeyurl);
	if (itor != m_keyurl_hash_map.end())
	{
		hash = itor->second;
		return true;
	}
	return false;
}

bool CTaskMgr::FindHashByUrl(string strUrl, HASH& hash)
{
    // 判断类型
	bool bTs  = false;
	const char *pFileName = strrchr(strUrl.c_str(), '/');
	if (pFileName)
	{
		bTs = NULL != strstr(pFileName, ".ts");
	}
 //   bool bM3u8File = strUrl.rfind(".m3u8") != string::npos;
	//bool bTs = strUrl.rfind(".ts") != string::npos;

    if (! bTs)
    {
		CAutoLock lock(m_task_map_lock);
		for (map<HASH, CTask *>::iterator itor=m_task_map.begin(); itor!=m_task_map.end(); itor++)
		{
			if (itor->second)
			{
				if (itor->second->m_Para.strHttpOrgUrl == strUrl)
				{
					hash = itor->first;
					return true;
				}
				else if (0 == memcmp(itor->second->m_Para.strHttpOrgUrl.c_str(), "https://", 8))	// 兼容https播放
				{
					string strOrgUrl;
					string strTag = "http://";
					int pos = strUrl.find(strTag);
					if (pos != string::npos)
					{
						pos += strTag.length();
						strOrgUrl =strUrl.c_str() + pos;
					}
					if (itor->second->m_Para.strHttpOrgUrl.find(strOrgUrl) != string::npos)
					{
						hash = itor->first;
						return true;
					}
				}
			}
		}

        return FindHash(strUrl, hash);
    }
    else
    {
        //bool bM3u8Ts   = strUrl.rfind(".ts") != string::npos;
        //if (!bM3u8Ts)   // 非Ts片文件
        //{
        //    return false;
        //}
        // 查找属于哪一个任务
		
        CAutoLock lock(m_task_map_lock);

        for (map<HASH, CTask *>::iterator itr=m_task_map.begin(); itr!=m_task_map.end(); itr++)
        {
            if (! itr->second->IsPlaying()) // 优先查找播放的任务
            {
                continue;
            }
            if (itr->second->IsExistM3u8Ts(strUrl)) // 找到了属于那一个任务
            {
                hash = itr->first;
                return true;
            }
        }
		for (map<HASH, CTask *>::iterator itr=m_task_map.begin(); itr!=m_task_map.end(); itr++)
        {
            if (itr->second->IsPlaying()) // 优先查找播放的任务
            {
                continue;
            }
            if (itr->second->IsExistM3u8Ts(strUrl)) // 找到了属于那一个任务
            {
                hash = itr->first;
                return true;
            }
        }
    }
    return false;
}

bool CTaskMgr::FindM3u8Keyurl( string strFuzzyKeyurl,string& strKeyurl )
{
	CAutoLock lock(m_keyurl_hash_map_lock);
	map<string, HASH>::iterator itor = m_keyurl_hash_map.begin();
	while (itor != m_keyurl_hash_map.end())
	{
		string strFisrt = itor->first;
		int pos = strFisrt.rfind("/");
		if (pos != string::npos)
		{
			strFisrt = strFisrt.substr(0,pos+1);
		}
		if (strFisrt == strFuzzyKeyurl)
		{
			strKeyurl = itor->first;
			return true;
		}
		++itor;
	}
	return false;
}

std::string CTaskMgr::GetOrgurl(const HASH& hash )
{
	AutoPtr<CTask> pTask;
	if (FindTask(hash,&pTask))
	{
		return pTask->m_Para.strHttpOrgUrl;
	}
	return string("");
}

int CTaskMgr::CreateYfTask(const HASH& hash, const std::string strFile,int type,string& strHttpOrgUrl,string& strHttpKeyUrl,string& strHttpReferer,bool bIsLoad, TaskCfg* cfg/*=NULL*/ )
{
	CTask* pTask = FindTask(hash.data);
	if(pTask) 
	{
		//”¶∏√ «√ª’“µΩkeyurl->hashµƒº«¬º£¨À˘“‘÷ÿ–¬¥¥Ω®¡À»ŒŒÒ£¨∞—∂‘”¶πÿœµº”…œ£¨∏¸–¬url
		m_keyurl_hash_map_lock.Lock();
		m_keyurl_hash_map.insert(make_pair(strHttpKeyUrl,hash));
		m_keyurl_hash_map_lock.Unlock();
		pTask->m_Para.strHttpOrgUrl = strHttpOrgUrl;
		pTask->m_Para.strHttpKeyUrl = strHttpKeyUrl;
		pTask->m_Para.strRefUrl = strHttpReferer;
		return 2;
	}

	switch (type)
	{
	case HLSDOWNTASK:
	case HLSP2PDOWNTASK:
		pTask = new CM3u8Task(hash.data,strFile,type,strHttpOrgUrl,strHttpKeyUrl,strHttpReferer, cfg);
		break;
	case HTTPUPTASK:
		break;
	case HTTPNFUPTASK:
		pTask = new CNFUploadTask(hash.data, type, strHttpKeyUrl, strFile);
		break;
	default:
		pTask = new CDownLoadTask(hash.data,strFile,type,strHttpOrgUrl,strHttpKeyUrl,strHttpReferer);
		break;
	}
	//pTask = new CDownLoadTask(hash.data,strFile,type,strHttpOrgUrl,strHttpKeyUrl,strHttpReferer);
	if ( cfg != NULL)
	{
		pTask->m_Para = *cfg;
		pTask->m_lastStartTime = time(NULL);
	}
	int ret = pTask->run(bIsLoad);
	if(ret < 0)
	{
		delete pTask;
		return ret;
	}
	//if (!bIsLoad)
	//{
	//	// ¥¥Ω®»ŒŒÒ∫Ûƒ¨»œ‘›Õ£◊¥Ã¨
	//	pTask->m_Para.status = CTask::PAUSE;
	//}
	pTask->m_Para.status = CTask::PAUSE;
	m_task_map_lock.Lock();
	m_task_map[hash] = pTask;
	m_task_map_lock.Unlock();

	size_t pos = strFile.rfind("/");
	std::string strPath = strFile.substr(0,pos+1);

	m_task_path_lock.Lock();
	m_task_path[strPath].push_back(pTask);
	m_task_path_lock.Unlock();

	m_keyurl_hash_map_lock.Lock();
	m_keyurl_hash_map.insert(make_pair(strHttpKeyUrl,hash));
	m_keyurl_hash_map_lock.Unlock();
	return ret;
}

int CTaskMgr::WriteHttpData( HASH& hash,const char* buf, QVOD_UINT64 pos, int len )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
#ifdef MONGOTV
		//httpœ¬‘ÿµΩ ˝æ›¡À£¨»Ù «µ⁄»˝∑ΩCDN£¨‘⁄µ⁄“ª¥Œœ¬‘ÿµΩ ˝æ›∫Û“™∑¢start.do
		if (task->IsUrlOtherCdn() && !task->IsStartDoSended())
		{
			task->SetStartDoSended(true);
			CMongoTvCdn::Instance()->AddRequest(task->m_Para.strHttpOrgUrl.c_str(),true,CMongoTvCdn::START);
		}
#endif
		return task->WriteHttpData(buf,pos,len);
	}
	return -1;
}

void CTaskMgr::SetFilesize( HASH& hash,QVOD_INT64 iFilesize )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->SetFilesize(iFilesize);
	}
}

void CTaskMgr::UpdateDownloadMode( HASH& hash )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->UpdateDownloadMode();
	}
}

void CTaskMgr::ChangeTaskType(const HASH& hash,int type )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->ChangeTaskType(type);
	}
}

int CTaskMgr::DownNextHttpPiece( HASH& hash,YfHttpPieceReq& req )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->DownNextHttpPiece(req);
	}
	return -1;
}

int CTaskMgr::GetDownloadMode( HASH& hash )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->GetDownloadMode();
	}
	return CTask::MODE_CDN;
}

void CTaskMgr::UpdateHttpOrgUrl( HASH& hash,string& strHttpOrgUrl )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->UpdateHttpOrgUrl(strHttpOrgUrl);
	}
}

int CTaskMgr::UpdateClarityUrl(HASH& hash,string& strHttpOrgUrl, string& strNewHash, string& strFilePath, int nClarity)
{
    AutoPtr<CTask> task;
    if(FindTask(hash,&task))
    {
        return task->UpdateClarityUrl(strHttpOrgUrl, strNewHash, strFilePath, nClarity);
    }
	return -1;
}

void CTaskMgr::DownEmergencyData( HASH& hash )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->DownEmergencyData();
	}
}

void CTaskMgr::NotifyDownload(const HASH& hash, const int &nId)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->NotifyDownload(nId);
	}
}

QVOD_INT64 CTaskMgr::GetAllTaskSize()
{
	CAutoLock Lock(m_task_map_lock);
	std::map<HASH,CTask*>::iterator mapIter;
	QVOD_UINT64 totalsize = 0;
	mapIter = m_task_map.begin();

	while(mapIter != m_task_map.end())
	{
		totalsize += (mapIter->second)->GetDownloadedSize();
		mapIter++;
	}

	return totalsize;
}

int CTaskMgr::SetCopyFullFile( HASH& hash,string& strCopyPath )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->SetCopyFullFile(strCopyPath);
	}
	return -1;
}

int CTaskMgr::UnsetCopyFullFile( HASH& hash )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->UnsetCopyFullFile();
	}
	return -1;
}

void CTaskMgr::SetCopyFullFileStatus( HASH& hash,int status )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->SetCopyFullFileStatus(status);
	}
}

void CTaskMgr::SetTaskPlaying( HASH& hash,bool bPlaying )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		if (bPlaying)
		{
			task->SetStartPlay();
		}
		else
		{
			task->SetStopPlay();
			if (-1 != g_cfg.m_uMaxDownTaskCount)
			{		
				list<HASH> downList;
				if (! task->IsDownlloadTask() ||
					(CTaskMgrInterFace::Instance()->GetDownloadingTasks(downList) > g_cfg.m_uMaxDownTaskCount))	// 判断当前任务是否需要暂停下载
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "SetTaskPlaying false! need pause task=%s", Hash2Char(hash.data).c_str());
#endif
					task->Pause();
				}
			}
		}
	}

	g_cfg.m_nDownPlayingTaskCount = GetDownloadingAndPlayingTaskNum();
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CTaskMgr::SetTaskPlaying hash=%s g_cfg.m_nDownPlayingTaskCount=%d", Hash2Char(hash.data).c_str(), g_cfg.m_nDownPlayingTaskCount);
#endif
}

int CTaskMgr::QueryTaskInfo( HASH& hash,STaskInfo* pTaskInfo )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		memcpy(pTaskInfo->hash, Hash2Char(task->m_Para.hash).c_str(), 40);
		pTaskInfo->iFileLen = task->GetFileSize();
		pTaskInfo->iDownloadLen = task->GetTotalDownloadLen();
		pTaskInfo->iDownloadSpeed = task->GetDownSpeed();
		pTaskInfo->iUploadLen = task->GetTotalUploadLen();
		pTaskInfo->iUploadSpeed = task->GetUploadSpeed();
		pTaskInfo->status = task->m_Para.status;	


		if (task->m_Para.type >= HTTPUPTASK &&  task->m_Para.type <= HTTPNFUPTASK)
		{
			pTaskInfo->iUploadLen = task->GetTotalUploadLen();
			pTaskInfo->iUploadSpeed = task->GetUploadSpeed();		
			pTaskInfo->status = task->TaskIsFinish()? 3: pTaskInfo->status;
		}
//#ifdef _360MOBILEMGR
//		pTaskInfo->iUploadLen = task->GetCurUplen();
//		pTaskInfo->iUploadSpeed = task->GetUpSpeed();
//		pTaskInfo->iError = task->GetTaskErrorMask();
//		task->ResetTaskErrorMask();
//#else
		pTaskInfo->bDownloadAndCopy = task->m_Para.bCopyFullFile;
		pTaskInfo->tasktype = task->m_Para.type;
		if (pTaskInfo->bDownloadAndCopy)
		{
			if (task->TaskIsFinish())
			{
				pTaskInfo->status = 3;	// 任务完成将状态置为3
				if (0 == pTaskInfo->iFileLen)
				{
					pTaskInfo->iFileLen = pTaskInfo->iDownloadLen;
				}
			}

			if (task->m_Para.strCopyPath.length() < sizeof(pTaskInfo->szCopyPath)/sizeof(char) )
			{
				memcpy(pTaskInfo->szCopyPath, task->m_Para.strCopyPath.c_str(), task->m_Para.strCopyPath.length() );
			}
		}
//#endif
			
		string wstrFilepath = task->GetFilePath() + task->GetFileName();
		if(wstrFilepath.length() < sizeof(pTaskInfo->szFilepath)/sizeof(char))
		{
			string strpath = wstrFilepath;
			int nPos = wstrFilepath.rfind('/');
			if (nPos != string::npos)
			{
				strpath = wstrFilepath.substr(0, nPos);
			}
			memcpy(pTaskInfo->szFilepath, strpath.c_str(), strpath.length() );
		}
		
		return 0;
	}
	return -1;
}

int CTaskMgr::QueryTaskInfo(HASH& hash,char* buf,int& len)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		string str = task->QueryTaskInfo();
		if (str.length() >= len)
		{
			len = str.length()+1;	// 返回所需最小buffer
			return E_NET_BUFFER_NOT_ENOUGH;
		}
		if (! str.empty())
		{
			memcpy(buf, str.c_str(), str.length());
			len = str.length();
		}
		return 0;
	}
	return -1;
}

int CTaskMgr::QueryBitInfo( HASH& hash,SBitInfo* pBitInfo,int& len )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		pBitInfo->iBlockNum = task->GetSeed()->GetPieceNum();
		pBitInfo->iBlockSize = task->GetSeed()->GetPieceSize();
		int iBitFieldLen = task->GetMemFile()->GetBitFieldLen();
		if (len < iBitFieldLen + sizeof(SBitInfo) -1)
		{
			len = iBitFieldLen + sizeof(SBitInfo) -1;
			return -2;
		}
		if (0 == iBitFieldLen || task->GetMemFile()->GetMemBit() == NULL)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO,"QueryBitInfo len = 0 or BitMem Null\n");
#endif
			return 0;
		}
		memcpy(pBitInfo->szBitField, task->GetMemFile()->GetMemBit(), iBitFieldLen);
		return 0;
	}
	return -1;
}

int CTaskMgr::SetVideoDuration( HASH& hash,int iDuration )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		if (iDuration >= 0)
		{
			task->SetVideoDuration(iDuration);
			return 0;
		}
	}
	return -1;
}
int CTaskMgr::NotifyVideoBuffering(HASH& hash, bool bBuffering)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->NotifyVideoBuffering(bBuffering);
		return 0;
	}
	return -1;
}
#ifdef MONGOTV
int CTaskMgr::GetVid( HASH& hash,char* pVid )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		memcpy(pVid, task->GetVid(), 32);
		return 0;
	}
	return -1;
}

void CTaskMgr::SetUrlCdnType( HASH& hash,bool bIsOtherCdn )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->SetUrlCdnType(bIsOtherCdn);
	}
}
#endif

void CTaskMgr::SetPlayingTimepoint(HASH& hash,int timepoint,bool bSeek,int& iCacheTime)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		CM3u8Task* pTask = dynamic_cast<CM3u8Task*>((CTask*)task);
		if (pTask && pTask->m_m3u8)
		{
			iCacheTime = pTask->SetPlayingTimepoint(timepoint, bSeek);
		}
	}
}

void CTaskMgr::SetM3u8CdnUrl( HASH& hash,const string& strUrl )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		CM3u8Task* pTask = dynamic_cast<CM3u8Task*>((CTask*)task);
		if (pTask && pTask->m_m3u8)
		{
			pTask->m_m3u8->SetM3u8Cdn(strUrl);
		}
	}
}

void CTaskMgr::SetM3u8OrgcdnUrl( HASH& hash,const string& strUrl )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		CM3u8Task* pTask = dynamic_cast<CM3u8Task*>((CTask*)task);
		if (pTask && pTask->m_m3u8)
		{
			pTask->m_m3u8->SetM3u8Orgcdn(strUrl);
		}
	}
}

bool CTaskMgr::SaveM3u8File( HASH& hash,const char* buf,int len )
{
	bool bRet = false;

	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		CM3u8Task* pTask = dynamic_cast<CM3u8Task*>((CTask*)task);
		if (pTask && pTask->m_m3u8)
		{
			bRet = pTask->WriteM3u8File(buf,len);
			if (bRet)
			{
				// 若当前为HLSP2PDOWNTASK任务，则判断是否能获取所有ts大小
				if (HLSP2PDOWNTASK == task->GetTaskType())
				{
					if (pTask->m_m3u8->GetAllTsSize() == 0)
					{
						ChangeTaskType(hash,HLSDOWNTASK);
					}
				}
			}
		}
	}

	return bRet;
}

bool CTaskMgr::M3u8IsExist( HASH& hash )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		CM3u8Task* pTask = dynamic_cast<CM3u8Task*>((CTask*)task);
		if (pTask && pTask->m_m3u8)
		{
			return pTask->IsExistM3u8();
		}
	}
	return false;
}
void CTaskMgr::NotifyDownM3u8Ts(const HASH& hash, const string& strFilename)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{		
		return task->NotifyDownM3u8Ts(strFilename);		
	}
}
int CTaskMgr::WriteM3u8Ts( HASH& hash,const string& strFilename,const char* buf,int start,int len,int filesize )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{		
		return task->WriteM3u8Ts(strFilename,buf,start,len,filesize);		
	}
	return -1;
}

int CTaskMgr::ReadM3u8Ts( HASH& hash,const string& strFilename,char* buf,int start,int len,bool bIsUpload )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{		
		return task->ReadM3u8Ts(strFilename,buf,start,len,bIsUpload);		
	}
	return -1;
}

int CTaskMgr::DownNextM3u8Ts( HASH& hash,SM3u8TsReq& req )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->DownNextM3u8Ts(req);
	}
	return -1;
}

void CTaskMgr::SetPlayingTs( HASH& hash,const string& strName )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		CM3u8Task* pTask = dynamic_cast<CM3u8Task*>((CTask*)task);
		if (pTask && pTask->m_m3u8)
		{
			pTask->m_m3u8->SetPlayingTs(strName);
		}
	}
}

void CTaskMgr::SetM3u8PlayingPos( HASH& hash,const string& strName,int pos )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->SetM3u8PlayingPos(strName,pos);
	}
}

bool CTaskMgr::IsPlayTsContinue( HASH& hash )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->IsPlayTsContinue();
	}
	return false;
}

void CTaskMgr::ClearContinuePlayTs( HASH& hash )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		CM3u8Task* pTask = dynamic_cast<CM3u8Task*>((CTask*)task);
		if (pTask && pTask->m_m3u8)
		{
			pTask->m_m3u8->ClearPlayingTsVec();
		}
	}
}

int CTaskMgr::DeleteHasPlayingData(const HASH& hash, QVOD_UINT64 iCurCacheSize)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->DeleteHasPlayingData(iCurCacheSize);
	}
	return 0;
}

void CTaskMgr::SetAesEncrypt(HASH& hash, bool bEncrypt)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		if (HLSDOWNTASK == task->GetTaskType() ||
			HLSP2PDOWNTASK == task->GetTaskType())
		{
			CM3u8Task *pTask = dynamic_cast<CM3u8Task*>((CTask*)task);
			if (pTask)
			{
				pTask->SetAesEncrypt(bEncrypt);
			}
		}
	}
}
QVOD_INT64 CTaskMgr::GetM3u8TsSize( HASH& hash,const string& strName )
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->GetM3u8TsSize(strName);
	}
	return 0;
}

int CTaskMgr::SetHeadValue(const HASH& hash, const char *pstHead, const char* pstrValue)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->SetHeadValue(pstHead, pstrValue);		
	}
	return 0;
}

int CTaskMgr::SetHeadValue(const HASH& hash, const char *pstrHeadAdd)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		return task->SetHeadValue(pstrHeadAdd);		
	}
	return 0;
}

void CTaskMgr::ClosePush(HASH& hash, const string& strFilename)
{
	AutoPtr<CTask> task;
	if(FindTask(hash,&task))
	{
		task->ClosePush(strFilename);		
	}
}
