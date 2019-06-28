#include "MsgHandle.h"
#include "MsgPool.h"
#include "MsgLimitLayer.h"
#include "Peer.h"
#include "global.h"
#include "sha1.h"
#include "TaskMgrInterface.h"
#include "ServerProduct.h"
#include "HttpAgentInterface.h"
#include "AutoPtr.h"
#include "CacheDataInMem.h"
#include "HttpUploadInterface.h"
#include "cfghost.h"

#ifdef MONGOTV
//#include "../common/Statis.h"
#endif
#include <iostream>
using namespace std;

CMsgHandle::CMsgHandle()
{
}

CMsgHandle::~CMsgHandle()
{
}

void CMsgHandle::DownNextPiece(KEY& key,int count)
{
	switch(DownNextPieceAction(key,count))
	{
	case -2:
	case -3:
		break;
	default:
		break;
	}
}

int CMsgHandle::DownNextPieceAction(KEY& key,int count)
{
	//CAutoLock lock(m_downLock);
	if(CMsgPoolInterface::Instance()->KeyHashExist(key))
	{
		//任务停止
		HASH& hash = CMsgPoolInterface::Instance()->GetKeyHash(key);
		if(CTaskMgrInterFace::Instance()->GetTaskStatus(hash) != CTask::RUNNING)
		{
			return -1;
		}

		//分片数限制
		if(CTaskMgrInterFace::Instance()->QueryPeerStatus(hash,key) != CPeer::RUNNING)
		{
			return -1;
		}

		//判断是否下载完成
		if(CTaskMgrInterFace::Instance()->TaskIsFinish(hash))
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO,"终于全部下载完了!\n");
#endif // _DEBUG

			return -1;
		}

		//分片数量
		int loopTimes = CTaskMgrInterFace::Instance()->GetPeerCanReqNum(hash,key);
		if(loopTimes > count)
		{
			loopTimes = count;
		}
		while(loopTimes > 0)
		{
			//取新的分片
			request req;
			int res = CTaskMgrInterFace::Instance()->DownNextPiece(hash,key,req);
			if(res < 0)
			{
#ifndef FOREIGNER
				//-1表示错误,-2表示无可下分片,-4表示当前的服务器无数据可下载
				if (-4 == res)
				{
					CTaskMgrInterFace::Instance()->DelOnePeer(hash,key);
					CMsgPoolInterface::Instance()->DelKeyHash(key);
				}
#endif
				return res;
			}
			//判断是否超速
			if(CMsgPoolInterface::Instance()->IsNeedCacheDownRequest(hash) && 
				!IsLANIP(key.ip))
			{
				DATA copyData;
				copyData.key = key;
				copyData.protocol = QVODBTPRO;
				copyData.len = sizeof(request);
				copyData.packet = new char[copyData.len];//该内存在msgpool删除
				memcpy(copyData.packet,(void*)&req,copyData.len);
				if(!CMsgLimitLayer::Instance()->AddMsgToDownQueue(hash,copyData,QVODBTPRO))
				{
					CTaskMgrInterFace::Instance()->CancelTimeoutReq(hash,key,ntohl(req.index),
						ntohl(req.begin),ntohl(req.length));
					SafeDeleteArray(copyData.packet);
					//delete[] copyData.packet;
					return -2;
				}
			}
			loopTimes--;
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

void CMsgHandle::HdHttpAgentReqLinkRsp(char* packet)  
{
	agentreqlinkrsp* parlinkrsp=(agentreqlinkrsp*)packet;
	HASH hash = parlinkrsp->hash;
	HASH acchash = parlinkrsp->acchash;

	//判断任务是否存在
	if(CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		if (CTaskMgrInterFace::Instance()->TaskIsFinish(hash))
		{
			CHttpAgentInterface::Instance()->DelTask(acchash);
		}
		else
		{
			CServerProduct::SetTaskStatus(hash, CTask::PLAY);
		}
	}
	else
	{
		QVOD_DWORD curtime = QvodGetTime();
		QVOD_INT64 totalAccSize = CTaskMgrInterFace::Instance()->GetTotalAccFileSize();
#ifdef QVODDEBUG
		_OUTPUT(INFO, "HdHttpAgentReqLinkRsp now acc space %lld, maxspace %lld\n", totalAccSize, g_webCacheSpace);
#endif // _DEBUG
	}
}

void CMsgHandle::HdHttpAgentReqBlock(char* packet)
{
	agentreqblock* arblock = (agentreqblock*)packet;
	if (3 == arblock->type && arblock->result != 10)//FILEINDEX do NOT need restore speed
	{
#ifdef QVODDEBUG
		_OUTPUT(INEED,"########还原限速了!!\n");
#endif // _DEBUG
		//还原限速
		g_speedlimit = g_userSpeedlimit;
		SetGlobalSpeed(g_speedlimit);//!
	}

	//添加统计,5秒加速先默认都是网内的数据
// #ifdef VERSION_360
// 	CCloudDB::Instance()->UpdateRecords(g_szIspID,2,STC_5SEC,arblock->len);
// #else
// 	CCloudDB::Instance()->UpdateRecords(g_szIspID,1,STC_5SEC,arblock->len);
// #endif

	//判断任务是否存在
	HASH hash(arblock->hash);
	if(CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		QVOD_UINT64 pos = arblock->pos;
		QVOD_UINT64 writedlen = 0;
		unsigned int piecesize = CTaskMgrInterFace::Instance()->GetFilePieceSize(hash);
		unsigned int piecenum = CTaskMgrInterFace::Instance()->GetFilePieceNum(hash);
		char* pbuf = arblock->pbuf;
		if(piecesize > 0 && arblock->len > 0)
		{
			if (NULL != pbuf)
			{
				unsigned int leftlen = arblock->len;
				while(0 < leftlen)
				{
					int writeIndex = pos/piecesize;
					if (leftlen >= piecesize)
					{
						int len = piecesize - pos%piecesize;
						unsigned int reallen = CTaskMgrInterFace::Instance()->WritePiece(hash,pbuf+writedlen,len,pos);
						if (reallen > 0)
						{
							CMsgPoolInterface::Instance()->BroadCastHave(hash,writeIndex);
						}
						writedlen += len ;
						pos += len;
						leftlen -= len;
					}
					else if (piecenum - 1 == writeIndex)
					{
						unsigned int reallen = CTaskMgrInterFace::Instance()->WritePiece(hash,pbuf+writedlen,leftlen,pos);
						if (reallen > 0)
						{
							CMsgPoolInterface::Instance()->BroadCastHave(hash,writeIndex);
						}
						break;
					}
					else
					{
						break;
					}
				}
			}
		}

		//关闭5秒加速中的任务状态
		bool bAccsucc = true;
		if(arblock->result == 2)
		{
			bAccsucc = false;
		}

		KEY key;
		key.ip = arblock->ip;
		key.port = arblock->port;
		key.sock = arblock->sock;
		CMsgPoolInterface::Instance()->DelKeyHash(key);
		CTaskMgrInterFace::Instance()->DelOnePeer(hash,key);
		
#if _DEBUG
		char ip[16];
		IpInt2Str(arblock->ip,ip);
		_OUTPUT(INFO,"*********CMsgHandle::HdHttpAgentReqBlock,From %s:%u!!\n",ip,arblock->port);
#endif
	}

	SafeDeleteArray(arblock->pbuf);
}

void CMsgHandle::HdHttpAgentReqMeta(char* packet)
{
	agentreqmeta* armeta = (agentreqmeta*)packet;
	AutoPtr<CTask> task;
	if(CTaskMgrInterFace::Instance()->FindTask((HASH&)armeta->hash,&task) )
	{
#ifdef QVODDEBUG
		_OUTPUT(INEED,"HdHttpAgentReqMeta!!\n");
#endif // _DEBUG

		//任务暂停
		if(task->GetStatus() != CTask::RUNNING)
			return ;
	}
	//!!静态库,内存在httpagent中分配
	SafeDeleteArray(armeta->metabuf);

	//delete[] armeta->metabuf;
}

void CMsgHandle::HdHttpAgentReqBlockInfo(char* packet)
{
	agentreqblockinfo* arblockinfo = (agentreqblockinfo*)packet;
	//判断任务是否存在
	HASH& hash = (HASH&)*arblockinfo->hash;
	if(CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		KEY key;
		key.ip = arblockinfo->ip;
		key.port = arblockinfo->port;
		key.sock = arblockinfo->sock;
		char szPeerID[24] = {0};
		sprintf_s(szPeerID,sizeof(szPeerID),"QVOD0004HTTP%04X%04X",(unsigned short)rand(),(unsigned short)rand());
		CMsgPoolInterface::Instance()->InsertKeyHash(key,hash);
		CTaskMgrInterFace::Instance()->AddOnePeer(hash,key,key.ip,key.port,true,0,TCP,szPeerID);

#ifdef QVODDEBUG
		_OUTPUT(INEED,"@@@@@@@@@@@@@@@@ Add One 5s Server!ip=%s,id=%s\n",inet_ntoa(*(in_addr*)&(key.ip)),szPeerID);
#endif

		//  [2014/1/15 tangtao]
		if (g_speedlimit != -2 && arblockinfo->type == 3)
		{
			g_speedlimit = -2;
			SetGlobalSpeed(g_speedlimit);
		}
	}
}

void CMsgHandle::HdHttpAgentReqPiece(char* packet)
{
	agentrequestpiece* arpiece = (agentrequestpiece*)packet;
	KEY key;
	key.ip = arpiece->ip;
	key.port = arpiece->port;
	key.sock = arpiece->sock;
	HASH hash = CMsgPoolInterface::Instance()->GetKeyHash(key);
	if(!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return ;
	}

	int piecesize = CTaskMgrInterFace::Instance()->GetFilePieceSize(hash);
	if (piecesize > 0)
	{
		int index =  arpiece->start/piecesize;
		int start = arpiece->start%piecesize;
		if (arpiece->length > 0)
		{
			int iResult = CTaskMgrInterFace::Instance()->WritePiece(hash,key,arpiece->buf,arpiece->length,index,start);
			if (iResult > 0)
			{
				CMsgPoolInterface::Instance()->BroadCastHave(hash,index);
			}
#ifdef QVODDEBUG
			char ip[16];
			IpInt2Str(arpiece->ip,ip);
			_OUTPUT(INFO,"++++++AgentReqPiece,begin:%d length:%d from %s:%u!!\n",arpiece->start,arpiece->length,ip,arpiece->port);
#endif // _DEBUG
		}
		else
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "request piece timeout start %d\n", arpiece->start);
#endif // _DEBUG
			CTaskMgrInterFace::Instance()->CancelTimeoutReq(hash,key,index,start,arpiece->piecelen);
		}
	}
	string orgurl = CTaskMgrInterFace::Instance()->GetReferUrl(hash);


	CMsgPoolInterface::Instance()->InterDownSize(arpiece->length);
	CSpeedCounter::Instance()->PushSpeedData(&hash,&key,arpiece->length,GlobalDownSpeedCallBack);
	SPEEDATA data;
	data.hash = hash;
	data.size = arpiece->length;
	CTaskMgrInterFace::Instance()->InterDownSize(data);
}

void CMsgHandle::HdHttpAgentRemoveFile(char* packet)
{
	agentremovefile* pagrmfile = (agentremovefile*)packet;
	HASH hash;
	memcpy(hash.data,pagrmfile->hash,20);
	AutoPtr<CTask> task;
	if ( CTaskMgrInterFace::Instance()->FindTask(hash,&task) )
	{
		if( !task->TaskIsFinish() )
		{
			CTaskMgrInterFace::Instance()->SetTaskStatus(hash,CTask::STOP);
		}
	}
}

void CMsgHandle::HdHttpAgentRange( char* packet )
{
	agentrange* pRange = (agentrange*)packet;
	//从cdn获取到数据了
	HASH hash = pRange->hash;
	QVOD_UINT64 pos = pRange->start;
	int len = pRange->len;
	char* pBuf = pRange->buf;
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		//关掉httpagent中的任务
		CHttpAgentInterface::Instance()->DelTask(hash);
		SafeDeleteArray(pBuf);
		return ;
	}
	//任务还没大小，初始化大小、块大小及块数
	if (CTaskMgrInterFace::Instance()->GetFileSize(hash) == 0)
	{
		QVOD_INT64 iFilesize = pRange->filesize;
		if (iFilesize > 0)
		{
			CTaskMgrInterFace::Instance()->SetFilesize(hash,iFilesize);
		}		
	}
	if (3 == pRange->result)
	{
		//http下载错误，暂停任务，通知UI
		CServerProduct::SetTaskStatus(hash,CTask::PAUSE);
		CallBackToUI(3, pRange->errcode, Hash2Char(hash.data).c_str(),40);
	}
	if (pBuf != NULL && len > 0)
	{
		int iRet = CTaskMgrInterFace::Instance()->WriteHttpData(hash,pBuf,pos,len);
		//写文件失败，若是360则暂停任务；若是芒果tv则空间不足删除旧任务，删除失败暂停任务并通知UI
		if (iRet < 0)
		{
#ifndef _360MOBILEMGR
			if (-3 == iRet && CServerProduct::DeleteOldTaskExceptHash(hash) == -1)
#endif
			{				
				CServerProduct::SetTaskStatus(hash,CTask::PAUSE);
				string 	strCallBackBuf = Hash2Char(hash.data);
				CallBackToUI(2,0,strCallBackBuf.c_str(), 40);
			}
			SafeDeleteArray(pBuf);

			//if (pBuf)
			//{
			//	delete[] pBuf;
			//}
			return;
		}
		else	if (iRet > 0)
		{
#ifdef MONGOTV
			char szVid[33] = {0};
			CTaskMgrInterFace::Instance()->GetVid(hash,szVid);
			//CStatis::Instance()->ShareRateAddLen(szVid,Hash2Char(hash.data).c_str(),len,0,0,0);
#endif
		}
	}
	SafeDeleteArray(pBuf);

	//触发下载下一块
	if (0 == g_cfg.m_nDownPlayingTaskCount || 
		CTaskMgrInterFace::Instance()->IsTaskPlaying(hash))	// 没有下载的播放任务或当前播放任务才自动下载下一片数据
	{
		DownNextHttpPiece(hash);
	}
}

int CMsgHandle::DownNextHttpPiece( HASH& hash )
{
	if (CTaskMgrInterFace::Instance()->GetTaskStatus(hash) != CTask::RUNNING || !CTaskMgrInterFace::Instance()->TaskIsExist(hash) 
		|| CTaskMgrInterFace::Instance()->TaskIsFinish(hash))
	{
		return -1;
	}
	YfHttpPieceReq req;
	int ret = CTaskMgrInterFace::Instance()->DownNextHttpPiece(hash,req);
	if (0 == ret)
	{
		CHttpAgentInterface::Instance()->AddTask(hash,req.strOrlurl,req.strReferer,req.start,(QVOD_INT64)req.length);
	}
	return ret;
}

void CMsgHandle::HdHttpM3u8( char* packet )
{	
	agentm3u8* pM3u8 = (agentm3u8*)packet;
	HASH hash = pM3u8->hash;
	{
		bool bWriteMem = false;
		//QPtr::AutoArrayPtr<char> pBuf = pM3u8->buf;
		char* pBuf = pM3u8->buf;
		if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
		{
	#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "HdHttpM3u8 TaskIsExist failed!");
	#endif
			SafeDeleteArray(pBuf);
			// 任务已不存在
			CHttpAgentInterface::Instance()->DelTask(hash);
			return ;
		}
		if (3 == pM3u8->result)
		{
			//http下载出错，通知UI
			// 请求的url是orgurl 则判断为失效连接
			if (pM3u8->buf)
			{
				string strOrgurl = CTaskMgrInterFace::Instance()->GetOrgurl(hash);

				if (strOrgurl == pM3u8->buf)
				{
					CServerProduct::SetTaskStatus(hash,CTask::PAUSE);
					CallBackToUI(3,pM3u8->errcode,Hash2Char(hash.data).c_str(),40);
				}
			}	
		}
		//m3u8 文件内容解析及保存
		if (!CTaskMgrInterFace::Instance()->M3u8IsExist(hash) && 0 == pM3u8->result)
		{
			bool bRet = CTaskMgrInterFace::Instance()->SaveM3u8File(hash,pBuf,pM3u8->len);
			if (! bRet)
			{
#ifdef QVOD_USE_CACHEMEM
				if (CTaskMgrInterFace::Instance()->IsTaskPlaying(hash))		// 播放任务才写入内存
				{
					//写入硬盘不成功，尝试写入内存
					bWriteMem = CCacheDataInMem::Instance().CacheM3u8Data(hash,"playlist.m3u8",(const char*)pBuf, 0, pM3u8->len, pM3u8->len);
					if (! bWriteMem)
					{
#ifdef QVODDEBUG
						_OUTPUT(INFO, "CCacheDataInMem CacheM3u8Data failed! playlist.m3u8\n");
#endif
					}		
				}			
#endif
			}
		}
		if (! bWriteMem)		// 没有写入内存则删除数据
		{	
			SafeDeleteArray(pBuf);
		}
	}

	if (0 == g_cfg.m_nDownPlayingTaskCount || 
		CTaskMgrInterFace::Instance()->IsTaskPlaying(hash))	// 没有下载的播放任务或当前播放任务才自动下载下一片数据
	{
		DownNextM3u8Ts(hash);
	}
}

void CMsgHandle::HdHttpM3u8Ts( char* packet )
{
	bool bWriteMem = false;
	agentm3u8ts* pTs = (agentm3u8ts*)packet;
	HASH hash = pTs->hash;

	//QPtr::AutoArrayPtr<char> pBuf = pTs->buf;
	char* pBuf = pTs->buf;
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		SafeDeleteArray(pBuf);
		// 任务已不存在
		CHttpAgentInterface::Instance()->DelTask(hash);
		return ;
	}
	if (3 == pTs->result)
	{
		//http下载出错，通知UI
		CServerProduct::SetTaskStatus(hash,CTask::PAUSE);
		CallBackToUI(3,pTs->errcode,Hash2Char(hash.data).c_str(),40);
	}
	if (pBuf != NULL && pTs->len > 0)
	{
		int iRet = CTaskMgrInterFace::Instance()->WriteM3u8Ts(hash,pTs->filename,pBuf,pTs->start,pTs->len,pTs->filesize);
		
		if (-3 == iRet)
		{
			if (0 == CServerProduct::ClearSpaceExceptHash(hash))	// 清理其它缓存任务再尝试写入一次
			{
				iRet = CTaskMgrInterFace::Instance()->WriteM3u8Ts(hash,pTs->filename,pBuf,pTs->start,pTs->len,pTs->filesize);
			}
		}
		if(iRet < 0)
		{
#ifdef QVOD_USE_CACHEMEM
			if (CTaskMgrInterFace::Instance()->IsTaskPlaying(hash))		// 播放任务才写入内存
			{
				//写入硬盘不成功，尝试写入内存
				bWriteMem = CCacheDataInMem::Instance().CacheM3u8Data(hash,pTs->filename,pBuf,pTs->start, pTs->len, pTs->filesize);
				if (! bWriteMem)
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CCacheDataInMem CacheM3u8Data failed!\n");
#endif
					SafeDeleteArray(pBuf);
					return ;
				}		
			}
			else	if (CTaskMgrInterFace::Instance()->IsCopyFullFile(hash))		// 是下载任务
			{
				if (-3 != iRet)
				{
					//写文件失败，暂停任务并回调通知UI
					CServerProduct::SetTaskStatus(hash,CTask::PAUSE);
					string 	strCallBackBuf = Hash2Char(hash.data);
					CallBackToUI(1,5,strCallBackBuf.c_str(), 40);
				}				
			}	
			if (-3 == iRet && !bWriteMem)
			{
				// 缓存空间不足，通知UI
				CServerProduct::SetTaskStatus(hash,CTask::PAUSE);
				string 	strCallBackBuf = Hash2Char(hash.data);
				CallBackToUI(2,0,strCallBackBuf.c_str(),40);
			}
#else
				SafeDeleteArray(pBuf);
				return;
#endif
		}
		else	if (iRet > 0)
		{
#ifdef MONGOTV
			char szVid[33] = {0};
			CTaskMgrInterFace::Instance()->GetVid(hash,szVid);
			//CStatis::Instance()->ShareRateAddLen(szVid,Hash2Char(hash.data).c_str(),pTs->len,0,0,0);
#endif
		}
	}
	else
	{
		CTaskMgrInterFace::Instance()->NotifyDownM3u8Ts(hash, pTs->filename);		// 通知该片已请求完
	}
	if (! bWriteMem)		// 没有写入内存则删除数据
	{	
		SafeDeleteArray(pBuf);
	}
	
	if (0 == g_cfg.m_nDownPlayingTaskCount || 
		CTaskMgrInterFace::Instance()->IsTaskPlaying(hash))	// 没有下载的播放任务或当前播放任务才自动下载下一片数据
	{
		DownNextM3u8Ts(hash);
	}
}

int CMsgHandle::DownNextM3u8Ts( HASH& hash )
{
	if (CTaskMgrInterFace::Instance()->GetTaskStatus(hash) != CTask::RUNNING || !CTaskMgrInterFace::Instance()->TaskIsExist(hash) 
		|| CTaskMgrInterFace::Instance()->TaskIsFinish(hash) || CHttpAgentInterface::Instance()->GetTaskInfo(hash, NULL) == 0)
	{
		return -1;
	}

	//SM3u8TsReq req;
	//int ret = CTaskMgrInterFace::Instance()->DownNextM3u8Ts(hash,req);
	//if (0 == ret)
	//{
	//	return CHttpAgentInterface::Instance()->AddM3u8TsTask(hash,req);
	//}
	//return ret;
	CTaskMgrInterFace::Instance()->UpdateDownloadMode(hash);		// 控制下载缓存
	return 0;
}

void CMsgHandle::HdHttpUpload(char* packet)
{
	agentrange* pRange = (agentrange*)packet;
	//从cdn获取到数据了
	HASH hash = pRange->hash;
	QVOD_UINT64 pos = pRange->start;
	int len = pRange->len;
	QPtr::AutoArrayPtr<char> pBuf = pRange->buf;
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		//关掉httpagent中的任务
		CHttpUploadInterface::Instance()->DelTask(hash);
		return ;
	}

	if (3 == pRange->result)
	{
		//http下载错误，暂停任务，通知UI
		CServerProduct::SetTaskStatus(hash,CTask::PAUSE);
		CallBackToUI(3, pRange->errcode, Hash2Char(hash.data).c_str(),40);
	}

	if (pBuf != NULL && len > 0)
	{
		int iRet = CTaskMgrInterFace::Instance()->WriteHttpData(hash,pBuf,pos,len);
		//写文件失败，若是360则暂停任务；若是芒果tv则空间不足删除旧任务，删除失败暂停任务并通知UI
		if (iRet < 0)
		{		
			CServerProduct::SetTaskStatus(hash,CTask::PAUSE);
			string 	strCallBackBuf = Hash2Char(hash.data);
			CallBackToUI(2,0,strCallBackBuf.c_str(),40);
			return;
		}
	}
	
	CTaskMgrInterFace::Instance()->NotifyDownload(hash, pRange->id);		// 通知该片已请求完

	//if (2 == pRange->result)	// 无数据，可能连接失败等情况
	//{
	//	return;
	//}
	if (4 == pRange->result)	// 重试超过5次,可能网络不通等原因
	{
		QVOD_SLEEP(20);
	}
	//触发上传下一块
	CTaskMgrInterFace::Instance()->UpdateDownloadMode(hash);
}