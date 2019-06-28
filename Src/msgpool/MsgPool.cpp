#include "MsgPool.h"
#include "qvod_core.h"

#include "global.h"
#include "sha1.h"
#include "TaskMgrInterface.h"
#include "Peer.h"
#include "stund.h"
#include "common-DNSCache.h"
#include "HttpAgentInterface.h"
#include "HttpUploadInterface.h"
#include "ServerProduct.h"
#include "Cfghost.h"
//#include "Upnp.h"

#ifdef MONGOTV
#include "../common/MongoTvCdn.h"
#include "../common/CloudCfg.h"
#endif

CMsgPool::CMsgPool()
{
	m_status = STOP;
	m_priorRoutineStatus = STOP;
	m_msgHandle = new CMsgHandle();
	m_xSecUpBufsize = 0;
	m_xSecDownBufSize = 0;
	m_downloadSpeed = 0;
	m_uploadSpeed = 0;
	m_downQueueTime = 0;
	m_upQueueTime = 0;
	m_limitUpSpeed = 0;
	m_limitDownSpeed = 0;
	m_nextDownTime = 0;
	m_nextUpTime = 0;
	m_upTotalSize = 0;
	m_downTotalSize = 0;
	m_lastUpSize = BLOCK_SIZE + 13;
	m_lastDownSize = BLOCK_SIZE + 13;

#ifndef FOREIGNER
	//高速通道服务器相关
	//////////////////////////////////////////////////////////////////////////
	m_iVipAccServerDownSpeed = 0;
	m_xSecVipAccServerDownSize = 0;
	m_iVipAccServerTotalDownSize = 0;
	//////////////////////////////////////////////////////////////////////////
#endif
#if SUPERNODE
	m_initMaxdownTask = g_maxdownloadingtask;
#endif
	for(int i = 0;i < sizeof(m_secPos)/sizeof(m_secPos[0]);i++)
	{
		m_secPos[i] = QvodGetTime();
	}

#ifdef SUPERNODE
	for ( int i = 0; i < 5; i++ )
#else
	for ( int i = 0; i < 4; i++ )
#endif
	{
		m_handles[i] = NULL;
	}

	memset((char*)&m_redundancyData, 0, sizeof(m_redundancyData));
	m_downlentotal = 0;
	m_uplentotal = 0;

#if 0
	m_dwLastReportTime = QvodGetTime();
	m_iStcSpeedDownSize = 0;
	m_iStcSpeedUpSize = 0;
#endif

}

CMsgPool::~CMsgPool()
{
	for(int i=0;i<4;i++)
	{
		if ( m_handles[i] )
		{
			QvodWaitForThread(m_handles[i], NULL, 500);
			//CloseHandle(m_handles[i]);
		}
	}
#ifdef SUPERNODE
	//CloseHandle(m_handles[4]);
#endif
    //SafeDelete(m_msgHandle);
}

void CMsgPool::Run(bool bOnlyPrior)
{
	if (bOnlyPrior)
	{
		if (m_priorRoutineStatus == RUNNING)
		{
			return;
		}
        
		m_priorRoutineStatus = RUNNING;
        m_status = RUNNING;
#ifndef NETBAR

		QvodCreateThread(&m_handles[0], (qvod_routine_pt)Routine, this);
		QvodCreateThread(&m_handles[1], (qvod_routine_pt)CommonRoutine, this);
		QvodCreateThread(&m_handles[2], (qvod_routine_pt)UpRoutine, this);
		//m_handles[0] = CreateThread(NULL,0,Routine,this,0,0);
		//m_handles[3] = CreateThread(NULL,0,CommonRoutine,this,0,0);
#endif
    }
}

void CMsgPool::Stop()
{
	m_priorRoutineStatus = STOP;
	if (m_status == STOP)
	{
		return ;
	}
	m_status = STOP;	

	QvodWaitForMultiThreads(5,m_handles);
#ifdef MONGOTV
	CMongoTvCdn::Instance()->Stop();
#endif
}

CMsgHandle* CMsgPool::GetMsgHandle()
{
	return m_msgHandle;
}

QVOD_THREAD_RETURN CMsgPool::CommonRoutine(void* param)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO,"Notice:CommonRoutine run!\n");
#endif // _DEBUG
	CMsgPool* msgpool=(CMsgPool*)param;
	while(msgpool->m_priorRoutineStatus == RUNNING)
	{
		msgpool->DoCommonAction();
		QvodSleep(16);
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO,"Notice:CommonRountine stop!\n");
#endif // _DEBUG
#ifndef WIN32
    return 0;
#endif
}

QVOD_THREAD_RETURN CMsgPool::Routine(void* param)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO,"Notice:Routine run!\n");
#endif // _DEBUG
	CMsgPool* msgpool=(CMsgPool*)param;
	int msglen = 100*1024;
	char msgpacket[100*1024] = {0};
	int result = 0;
	while(msgpool->m_priorRoutineStatus == RUNNING)
	{
		//下载数据缓存池 轮循消息
		if(!msgpool->IsOvertakeDownSpeed())
		{
			HASHDATA hashdata;			
			result = CMsgLimitLayer::Instance()->GetMsgFromDownQueue(hashdata);
			if(0 == result)
			{
				msgpool->DoDownLimitAction(hashdata);
				//该内存在msghandle里申请
				SafeDeleteArray(hashdata.data.packet);

				//if (hashdata.data.packet)
				//{
				//	delete[] hashdata.data.packet;
				//}
			}
		}
		
		if(CHttpAgentInterface::Instance()->RecvPacket(msgpacket,msglen))
		{
			agentmsghead* pagentmsg=(agentmsghead*)msgpacket;
			msgpool->ProcessHttpAgentPacket(msgpacket);
			//重置变量
			msglen = 100*1024;
			memset(msgpacket,0,msglen);
		}
		QvodSleep(10);		
	}
#ifdef QVODDEBUG
	_OUTPUT(LOG_DEBUG,"MsgPool Rountine exit!\n");
#endif // _DEBUG
#ifndef WIN32
    return 0;
#endif
}

QVOD_THREAD_RETURN CMsgPool::UpRoutine(void* param)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO,"Notice:UpRoutine run!\n");
#endif // _DEBUG
	CMsgPool* msgpool=(CMsgPool*)param;
	int msglen = 100*1024;
	char msgpacket[100*1024] = {0};
	int result = 0;
	while(msgpool->m_priorRoutineStatus == RUNNING)
	{
		// 上传数据轮询
		//上传数据缓存池 轮循消息,上传速度限制
		if(!msgpool->IsOvertakeUpSpeed())
		{
			if(CHttpUploadInterface::Instance()->RecvPacket(msgpacket,msglen))
			{
				msgpool->DoUpLimitAction();		// 限速处理

				agentmsghead* pagentmsg=(agentmsghead*)msgpacket;
				msgpool->ProcessHttpAgentPacket(msgpacket);
				//重置变量
				msglen = 100*1024;
				memset(msgpacket,0,msglen);
			}
		}

		QvodSleep(30);		
	}
#ifdef QVODDEBUG
	_OUTPUT(LOG_DEBUG,"MsgPool UpRoutine exit!\n");
#endif // _DEBUG
#ifndef WIN32
    return 0;
#endif
}
void CMsgPool::ProcessHttpAgentPacket(char* packet)
{
	agentmsghead* agentmsg = (agentmsghead*)packet;
	switch(agentmsg->id)
	{
	case AGENTSHAREFILE:
		break;
	case AGENTREQLINKRSP:
		m_msgHandle->HdHttpAgentReqLinkRsp(packet);
		break;
	case AGENTREQUESTPIECE:
		m_msgHandle->HdHttpAgentReqPiece(packet);
		break;
	case AGENTREQUESTBLOCK:
		m_msgHandle->HdHttpAgentReqBlock(packet);
		break;
	case AGENTREMOVEFILE:
		m_msgHandle->HdHttpAgentRemoveFile(packet);
		break;
	case AGENTREQUESTBLOCKINFO:
		m_msgHandle->HdHttpAgentReqBlockInfo(packet);
		break;
	case AGENTREQUESTMETA:
		m_msgHandle->HdHttpAgentReqMeta(packet);
		break;
	case AGENTRANGE:
		m_msgHandle->HdHttpAgentRange(packet);
		break;
	case AGENTM3U8:
		m_msgHandle->HdHttpM3u8(packet);
		break;
	case AGENTM3U8TS:
		m_msgHandle->HdHttpM3u8Ts(packet);
		break;
	case AGENTUPLOAD:
		m_msgHandle->HdHttpUpload(packet);
		break;
	default:
		break;
	}
}

bool CMsgPool::IsValidData(KEY& key)
{
	if(KeyHashExist(key))
	{
		HASH hash;
		if(GetKeyHash(key,hash))
		{
			if(CTaskMgrInterFace::Instance()->TaskIsExist(hash)
				&& CTaskMgrInterFace::Instance()->GetTaskStatus(hash) != CTask::STOP)
			{
				return true;
			}
		}
	}
	return false;
}

//对key,hash保存map的操作代码段,begin
bool CMsgPool::KeyHashExist(KEY& key)
{
	CAutoLock lock(m_keyHashMapLock);
	map<KEY,HASH>::iterator pos=m_keyHashMap.find(key);
	if(pos==m_keyHashMap.end())
		return false;
	else
		return true;
}

void CMsgPool::DelKeyHash(KEY& key)
{
	HASH hash;
	m_keyHashMapLock.Lock();
	map<KEY,HASH>::iterator itor=m_keyHashMap.find(key);
	if(itor!=m_keyHashMap.end())
	{
		hash=itor->second;
		m_keyHashMap.erase(itor);
	}
	else
	{
		m_keyHashMapLock.Unlock();
		return ;
	}
	m_keyHashMapLock.Unlock();

	m_hashKeyMapLock.Lock();
	map<HASH,set<KEY> >::iterator itorHk=m_hashKeyMap.find(hash);
	if(itorHk!=m_hashKeyMap.end())
	{
		set<KEY>& keySet=itorHk->second;
		keySet.erase(key);
		if(keySet.empty())
		{
			m_hashKeyMap.erase(itorHk);
		}
	}
	m_hashKeyMapLock.Unlock();
}

void CMsgPool::DelOneKey(KEY &key)
{
	DelKeyHash(key);
}

void CMsgPool::DelOtherTaskPeers(HASH& excepthash)
{
	if ( CTaskMgrInterFace::Instance()->TaskIsFinish(excepthash) )
	{
		return;
	}

	m_hashKeyMapLock.Lock();
	map<HASH,set<KEY> >::iterator itorHk = m_hashKeyMap.begin();
	for(;itorHk != m_hashKeyMap.end();)
	{
		HASH hash = itorHk->first;
		if(excepthash == hash)
		{
			itorHk++;
		}
		else
		{
			set<KEY>& keySet = itorHk->second;
			set<KEY>::iterator itorKs=keySet.begin();
			for(;itorKs != keySet.end();)
			{
				KEY key = (*itorKs) ;
				if (CTaskMgrInterFace::Instance()->GetPeerDownSpeed(hash, key) > 0)
				{
					m_keyHashMapLock.Lock();
					map<KEY,HASH>::iterator itorKh = m_keyHashMap.find(key);
					if (itorKh != m_keyHashMap.end())
					{
						m_keyHashMap.erase(itorKh);
					}
					m_keyHashMapLock.Unlock();

					CTaskMgrInterFace::Instance()->DelOnePeer(hash, key);
					keySet.erase(itorKs++);
				}
				else
				{
					itorKs++;
				}
			}
			if (keySet.empty())
			{
				m_hashKeyMap.erase(itorHk++);
			}
			else
			{
				itorHk++;
			}
		}
	}
	m_hashKeyMapLock.Unlock();
}

bool CMsgPool::GetKeyHash(KEY& key,HASH& hash)
{
	CAutoLock lock(m_keyHashMapLock);
	map<KEY,HASH>::iterator itor=m_keyHashMap.find(key);
	if(itor!=m_keyHashMap.end())
	{
		memcpy(hash.data,itor->second.data,20);
		return true;
	}
	return false;
}

HASH& CMsgPool::GetKeyHash(KEY& key)
{
	CAutoLock lock(m_keyHashMapLock);
	return m_keyHashMap[key];
}

bool CMsgPool::InsertKeyHash(KEY& key,HASH& hash)
{
	m_keyHashMapLock.Lock();
	map<KEY,HASH>::iterator itor=m_keyHashMap.find(key);
	if(itor==m_keyHashMap.end())
	{
		m_keyHashMap.insert(pair<KEY,HASH>(key, hash));
	}
	else
	{
		//已经存在则返回
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"InsertKeyHash already exist!!\n");
#endif // _DEBUG
		m_keyHashMapLock.Unlock();
		return false;
	}
	m_keyHashMapLock.Unlock();


	m_hashKeyMapLock.Lock();
	map<HASH,set<KEY> >::iterator itorHk=m_hashKeyMap.find(hash);
	if(itorHk==m_hashKeyMap.end())
	{
		set<KEY> keySet;
		keySet.insert(key);
		m_hashKeyMap.insert(pair<HASH,set<KEY> >(hash,keySet));
	}
	else
	{
		set<KEY>& keySet=itorHk->second;
		keySet.insert(key);
	}
	m_hashKeyMapLock.Unlock();
	return true;
}
//对key,hash保存map的操作代码段,end


// 任务操作代码段,begin
void CMsgPool::StartALLTask()
{
	m_keyHashMapLock.Lock();
	map<KEY,HASH> keyHashMap = m_keyHashMap;
	m_keyHashMapLock.Unlock();
	map<KEY,HASH>::iterator itor = keyHashMap.begin();
	for(;itor!=keyHashMap.end();itor++)
	{
		KEY key=itor->first;
		m_msgHandle->DownNextPiece(key);
	}
#ifdef NETBAR
	CLanServerSearch::Instance()->ActiveAllTask();
#endif
}

bool CMsgPool::SendPacket(KEY& key,char* packet,int length)
{
	return true;
}

void CMsgPool::PauseAllTask()
{
#ifdef NETBAR
	m_keyHashMap.clear();
#endif

	//删除5秒加速下载任务
	m_hashKeyMapLock.Lock();
	set<HASH> hashSet;
	map<HASH,set<KEY> >::iterator itorHk = m_hashKeyMap.begin();
	for(;itorHk != m_hashKeyMap.end();)
	{
#ifdef NETBAR
		set<KEY>& keySet = itorHk->second;
		set<KEY>::iterator itorKs = keySet.begin();
		for(;itorKs != keySet.end();itorKs++)
		{
			KEY& key = *itorKs;
			CDispatchMgrInterface::Instance()->RemoveSock(key);
			CTaskMgrInterFace::Instance()->DelOnePeer(hash, key);
		}
		keySet.clear();
		itorHk = m_hashKeyMap.erase(itorHk);
#else
		HASH hash = itorHk->first;
		hashSet.insert(hash);
		itorHk++;
#endif
	}
	m_hashKeyMapLock.Unlock();
	for (set<HASH>::iterator hashItor = hashSet.begin();hashItor != hashSet.end();++hashItor)
	{
		//删除5秒加速下载任务
		HASH hash = *hashItor;
		CHttpAgentInterface::Instance()->DelTask(hash);
	}
}

void CMsgPool::PauseOtherTask(HASH& excepthash)
{
#ifdef NETBAR
	m_keyHashMapLock.Lock();
	map<KEY,HASH>::iterator itorKh = m_keyHashMap.begin();
	for(;itorKh != m_keyHashMap.end();)
	{
		HASH hash = itorKh->second;
		if(excepthash == hash)
			itorKh++;
		else
			itorKh = m_keyHashMap.erase(itorKh);
	}
	m_keyHashMapLock.Unlock();
#endif
	//删除5秒加速下载任务及高速通道
	set<HASH> hashSet;
	m_hashKeyMapLock.Lock();
	map<HASH,set<KEY> >::iterator itorHk = m_hashKeyMap.begin();
	for(;itorHk != m_hashKeyMap.end();)
	{
		HASH hash = itorHk->first;
		if(excepthash == hash)
		{
			itorHk++;
		}
		else
		{
#ifdef NETBAR
			set<KEY>& keySet = itorHk->second;
			set<KEY>::iterator itorKs=keySet.begin();
			for(;itorKs != keySet.end();itorKs++)
			{
				KEY key = *itorKs;
				CDispatchMgrInterface::Instance()->RemoveSock(key);
				CTaskMgrInterFace::Instance()->DelOnePeer(hash, key);
			}
			keySet.clear();
			itorHk = m_hashKeyMap.erase(itorHk);
#else
			hashSet.insert(hash);
			itorHk++;
#endif
		}
	}
	m_hashKeyMapLock.Unlock();
	for (set<HASH>::iterator hashItor = hashSet.begin();hashItor != hashSet.end();++hashItor)
	{
		//删除5秒加速下载任务
		HASH hash = *hashItor;
		CHttpAgentInterface::Instance()->DelTask(hash);
	}
	StartOneTask(excepthash);
#ifdef NETBAR
	CLanServerSearch::Instance()->ActiveOneTask(excepthash);
#endif
}

void CMsgPool::StartOneTask(HASH& hash)
{
	m_hashKeyMapLock.Lock();
	map<HASH,set<KEY> >::iterator itorHk=m_hashKeyMap.find(hash);
	set<KEY> keySet;
	if(itorHk!=m_hashKeyMap.end())
	{
		keySet=itorHk->second;		
	}
	// 放到下一个if判断之后容易死锁 [10/28/2013 tangtao]
	m_hashKeyMapLock.Unlock();

	KEY httpKey;
	bool bHaveHttp = false;

	//modified by linyanxin on 20120703
	//prevent deadlock in CMsgHandle::DownNextPiece with CMsgHandle::m_downLock.
	set<KEY>::iterator itorKs=keySet.begin();
	for(;itorKs!=keySet.end();itorKs++)
	{
		KEY key = *itorKs;
		if (bHaveHttp && key == httpKey)
		{
			continue;
		}
		m_msgHandle->DownNextPiece(key);
	}
#ifdef NETBAR
	CLanServerSearch::Instance()->ActiveOneTask(hash);
#endif
}

void CMsgPool::PauseOneTask(HASH& hash)
{
	//删除http下载任务
	CHttpAgentInterface::Instance()->DelTask(hash);
}

void CMsgPool::DelOneTask(HASH& hash)
{
	m_keyHashMapLock.Lock();
	map<KEY,HASH>::iterator itor=m_keyHashMap.begin();
	for(;itor!=m_keyHashMap.end();)
	{
		if(memcmp(itor->second.data,hash.data,20)==0)
			m_keyHashMap.erase(itor++); /* silver modify */
		else
			itor++;
	}
	m_keyHashMapLock.Unlock();

	m_hashKeyMapLock.Lock();
	map<HASH,set<KEY> >::iterator itorHk=m_hashKeyMap.find(hash);
	if(itorHk!=m_hashKeyMap.end())
	{
		set<KEY>& keySet=itorHk->second;
		set<KEY>::iterator itorKs=keySet.begin();
		for(;itorKs!=keySet.end();itorKs++)
		{
			KEY key = *itorKs;
		}
		keySet.clear();
		m_hashKeyMap.erase(itorHk);
	}
	m_hashKeyMapLock.Unlock();

	CSpeedCounter::Instance()->DelSpeedData(hash,GlobalDownSpeedCallBack);
#ifndef FOREIGNER
	CSpeedCounter::Instance()->DelSpeedData(hash,VipAccServerGlobalDwonSpeedCallBack);
#endif

	CHttpAgentInterface::Instance()->DelTask(hash);
}
// 任务操作代码段,end

//速度限制操作代码段,begin
void CMsgPool::SetGlobalUpSpeed()
{
	//无限制模式和自动模式,如果有下载中的播放任务,上传限制35KB
	if(0 == g_cfg.m_limitSpeedMode || 1 == g_cfg.m_limitSpeedMode)
	{
		if (CTaskMgrInterFace::Instance()->GetDownloadingAndPlayingTaskNum() > 0)
		{
			m_limitUpSpeed = 35;

			//下载速度小于120KB/S,上传速度限制为30KB/S
			if (m_downloadSpeed < 120000)
			{
				m_limitUpSpeed = 30;
			}
			m_upQueueTime = m_lastUpSize/m_limitUpSpeed;
		}
		else
		{
			m_limitUpSpeed = 0;
			m_upQueueTime = 0;
		}
	}
	else
	{
		m_limitUpSpeed = g_iUpRateLimit;
		m_upQueueTime = m_lastUpSize/m_limitUpSpeed;
	}	

	m_nextUpTime = QvodGetTime() + m_upQueueTime;
}

void CMsgPool::SetGlobalDownSpeed()
{
	
}

unsigned int CMsgPool::GetGlobalDownSpeed()
{
	//更新一次速度
	m_downloadSpeed = m_xSecDownBufSize/SPCINTERVALTIME;
	//m_downloadSpeed += CHttpAgentInterface::Instance()->GetGlobalDownSpeed();
	return m_downloadSpeed;
}

bool CMsgPool::IsBlockUpSpeed()
{
	if(g_iUpRateLimit == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CMsgPool::IsBlockDownSpeed()
{
	if(g_iDownRateLimit == 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CMsgPool::UpdateNextDownTime(int lastSize)
{
	//更新最近下载数据包大小
	if(lastSize > 0)
	{
		m_lastDownSize = lastSize;
	}
	//更新下载间隔时间
	if(m_limitDownSpeed == 0)
	{
		m_downQueueTime = 0;
	}
	else
	{
		m_downQueueTime = m_lastDownSize/m_limitDownSpeed;
	}
	QVOD_DWORD curtime=QvodGetTime();
	if(m_nextDownTime == 0 || (lastSize >= 0 && (curtime > m_nextDownTime + m_downQueueTime*5)))
	{
		m_nextDownTime = curtime + m_downQueueTime;
	}
	else
	{
		m_nextDownTime += m_downQueueTime;
	}
}

void CMsgPool::UpdateNextUpTime(int lastSize)
{
	//更新最近上传数据包大小
	if(lastSize > 0)
	{
		m_lastUpSize = lastSize;
	}
	//更新上传间隔时间
	if(m_limitUpSpeed == 0)
	{
		m_upQueueTime = 0;
	}
	else
	{
		m_upQueueTime = m_lastUpSize*1000/(m_limitUpSpeed*1024);
		//m_upQueueTime = m_upQueueTime-50;
	}
	unsigned long curtime=QvodGetTime();
	if(m_nextUpTime == 0 || (lastSize >=0 && (curtime > m_nextUpTime + m_upQueueTime*5)))
	{
		m_nextUpTime = curtime + m_upQueueTime;
	}
	else
	{
		m_nextUpTime += m_upQueueTime;
	}
	m_lastUpSize = 0;
}

bool CMsgPool::IsNeedCacheDownRequest()
{
	if(m_limitDownSpeed == 0)
	{
		return false;
	}
	else
	{
		return true;	
	}
}

bool CMsgPool::IsNeedCacheDownRequest(HASH& hash)
{
	if(m_limitDownSpeed == 0)
	{
		if(CTaskMgrInterFace::Instance()->IsNeedCacheDownRequest(hash))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

bool CMsgPool::IsOvertakeDownSpeed()
{
	if (0 == m_limitDownSpeed)
	{
		return false;
	}
	if( m_nextDownTime > QvodGetTime() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CMsgPool::IsOvertakeUpSpeed()
{
	if (0 == m_limitUpSpeed)
	{
		return false;
	}
	//加快速度调高过程
	if(g_bUpRateReact)
	{
		SetGlobalUpSpeed();
		g_bUpRateReact = false;
		return false;
	}	
	if(m_nextUpTime > QvodGetTime())
	{
		return true;
	}
	else
	{
		return false;
	}
}

//速度限制操作代码段,end
void CMsgPool::BroadCastHave(HASH& hash,int index)
{
	have he;
	he.id = HAVE;
	he.len = htonl(5);
	he.piece_index = htonl(index);
	CAutoLock lock(m_hashKeyMapLock);
	map<HASH,set<KEY> >::iterator itorHk = m_hashKeyMap.find(hash);
	if(itorHk != m_hashKeyMap.end())
	{
		set<KEY> keySet = itorHk->second;
		set<KEY>::iterator itorKs = keySet.begin();
		for(;itorKs != keySet.end();itorKs++)
		{
			KEY key=*itorKs;
			//qvod客户端,判断是否对方已下载过该片,是则不发送have消息
			if(CTaskMgrInterFace::Instance()->PeerNeedSendHaveMsg(hash,key,index))
			{
				SendPacket(key,(char*)&he,sizeof(have));
			}
		}
	}
}

void CMsgPool::SendTimeoutMsg(HASH& hash,KEY& key,request* reqData)
{
	requesttimeout rtimeout;
	rtimeout.id=REQ_TIMEOUT;
	rtimeout.len=htonl(sizeof(request)+1);
	rtimeout.req.begin=reqData->begin;
	rtimeout.req.id=reqData->id;
	rtimeout.req.index=reqData->index;
	rtimeout.req.len=reqData->len;
	rtimeout.req.length=reqData->length;
	SendPacket(key,(char*)&rtimeout,sizeof(requesttimeout));
}


void CMsgPool::DoCommonAction()
{
	QVOD_DWORD curtime=QvodGetTime();
	//查询速度的更新操作
	if(curtime - m_secPos[0] >= 500)
	{
		//直播_发送bitfield信息
// 		CChannelMgrInterface::Instance()->SendHeartBeat();
		m_secPos[0]=QvodGetTime();
	}

	//约1秒位置
	if(curtime - m_secPos[1] >= 1000)
	{
		//处理超时请求包
		PopTimeoutReq();

		//重新计算获取速度
		GetGlobalDownSpeed();

		m_secPos[1]=QvodGetTime();
	}

#ifdef _DEBUG
	//约5秒位置
	if(curtime - m_secPos[2] >= 5000)
	{
// 		if(m_downloadSpeed > 1 || m_uploadSpeed > 1)
		{
			HANDLE consolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(consolehwnd,11); //设置颜色为绿色
			_OUTPUT(INFO,"_____NOTICE:cur dspeed:%.1fkb,uspeed:%.1fkb,uploadlimit:%uKB/s!!!______\n",
				(float)m_downloadSpeed/1024,(float)m_uploadSpeed/1024,m_limitUpSpeed);
			SetConsoleTextAttribute(consolehwnd,7); 
		}
		m_secPos[2] = QvodGetTime();
	}
#endif

#ifdef SUPERNODE
	//约10秒位置
	if(curtime - m_secPos[3] >= 10000)
	{
		CTaskMgrInterFace::Instance()->CheckTimeStamp();
		m_secPos[3] = QvodGetTime();
	}
#else
	//约3秒位置
	if(curtime-m_secPos[3] >= 3000)
	{
#ifdef NETBAR
		SendPieceRequest();
#endif
		//CheckEmergencyData();
		//速度计算
		CSpeedCounter::Instance()->PopSpeedData();

		//检查请求队列是否超时
		CTaskMgrInterFace::Instance()->CheckTimeStamp();
		m_secPos[3] = QvodGetTime();
	}
#endif

	//约60秒位置
	if(curtime - m_secPos[4] >= 60000)
	{
		m_secPos[4] = QvodGetTime();
	}

	if(curtime - m_secPos[6] >= 1800000)
	{
#ifdef MONGOTV
		if (!g_bIsUpgradeProc)
		{
			//每半小时重新获取限速的百分比
			int iUploadPercentArray[4] = {0};
			if (CCloudCfg::Instance()->GetCloudCfg(KEY_AUTO_UPLOAD_LIMIT_PERCENT,KEY_GAME_UPLOAD_LIMIT_PERCENT,iUploadPercentArray) == 1)
			{
				for (int i = 0; i < 4; ++i)
				{
					if (iUploadPercentArray[i] >= 0 && iUploadPercentArray[i] <= 100)
					{
						g_iUploadLimitPercentArray[i] = iUploadPercentArray[i];
					}
				}
			}
		}		
#endif
		m_secPos[6] = QvodGetTime();
	}

	if ( curtime - m_secPos[8] >= 3600000 )
	{
		m_secPos[8] = QvodGetTime();
	}
}

#ifndef SUPERNODE
void CMsgPool::UpdateUpnp()
{
	m_secPos[5] = 0;
}
#endif

bool CMsgPool::IsLimitDownSpeed()
{
	if(m_limitDownSpeed==0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


int CMsgPool::GetLimitDownSpeed()
{
	return m_limitDownSpeed;
}

bool CMsgPool::IsLimitUpSpeed()
{
	return (0 != m_limitUpSpeed);
}
void CMsgPool::SetLimitUpSpeed(int speed)
{
	m_limitUpSpeed = speed;
}

int CMsgPool::GetLimitUpSpeed()
{
	return m_limitUpSpeed;
}

QVOD_UINT64 CMsgPool::GetGlobalDownSize()
{
	return m_downTotalSize;
}

unsigned int CMsgPool::GetGlobalUpSpeed()
{
	//更新一次速度
	m_uploadSpeed = m_xSecUpBufsize/SPCINTERVALTIME;
	return m_uploadSpeed;
}
QVOD_UINT64 CMsgPool::GetGlobalUpSize()
{
	return m_upTotalSize;	
}

bool CMsgPool::InsertToRefuseSet(HASH& hash)
{
	CAutoLock lock(m_refuseHashLock);
	set<HASH>::iterator it=m_refuseHash.find(hash);
	if(it==m_refuseHash.end())
	{
		m_refuseHash.insert(hash);
		return true;
	}
	else
	{
		return false;
	}
}

bool CMsgPool::IsRefuseHash(HASH& hash)
{
	CAutoLock lock(m_refuseHashLock);
	set<HASH>::iterator it=m_refuseHash.find(hash);
	if(it==m_refuseHash.end())
		return false;
	else
		return true;
}

void CMsgPool::InterDownSize(int size)
{
	CAutoLock lock(m_downTotalSizeLock);
	if(size > 0)
	{
		m_downTotalSize += size;
#if 0
		m_lockSpeedStc.Lock();
		m_iStcSpeedDownSize += size;
		m_lockSpeedStc.Unlock();
#endif
	}
	m_xSecDownBufSize += size;
}

void CMsgPool::InterUpSize(int size)
{
	CAutoLock lock(m_upTotalSizeLock);
	if(size > 0)
	{
		m_upTotalSize += size;
		m_lastUpSize += size;
	}
	m_xSecUpBufsize += size;
}

void CMsgPool::PushTimeoutReq(KEY& key,PROTOCOL protocol)
{
	CAutoLock lock(m_timeoutQueLock);
	TIMEOUTREQ timeoutReq;
	timeoutReq.key = key;
	timeoutReq.protocol = protocol;
	timeoutReq.timestamp = QvodGetTime();
	m_timeoutReqQue.push(timeoutReq);
}

void CMsgPool::PopTimeoutReq()
{
	CAutoLock lock(m_timeoutQueLock);
	bool bRun = true;
	while(bRun)
	{
		if(m_timeoutReqQue.empty())
		{
			return ;
		}
		QVOD_DWORD timestamp = QvodGetTime();
		TIMEOUTREQ& timeoutReq = m_timeoutReqQue.front();
		if(timestamp - timeoutReq.timestamp > 5000)
		{
			//数据拷贝
			if(timeoutReq.protocol == QVODBTPRO)
			{
				m_msgHandle->DownNextPiece(timeoutReq.key,1);
			}
			m_timeoutReqQue.pop();
		}
		else
		{
			bRun = false;
		}
	}
}

void CMsgPool::DoDownLimitAction( HASHDATA& hashdata )
{
	if(hashdata.data.protocol == QVODBTPRO)
	{
		//若任务不存在直接删除
		if(!CTaskMgrInterFace::Instance()->TaskIsExist(hashdata.hash))
		{
			return ;
		}

		//任务暂停
		if(CTaskMgrInterFace::Instance()->GetTaskStatus(hashdata.hash) == CTask::PAUSE)
		{
			//取消peer的请求位置
			request* req = (request*)hashdata.data.packet;
			CTaskMgrInterFace::Instance()->CancelTimeoutReq(hashdata.hash,hashdata.data.key,ntohl(req->index),
				ntohl(req->begin),ntohl(req->length));
			return ;
		}

		//请求4s超时,防止长时间在上传队列中,导致超时
		QVOD_DWORD curTime = QvodGetTime();
		if(curTime - hashdata.timestamp > 4000)
		{
			//取消peer的请求位置
			request* req = (request*)hashdata.data.packet;
			CTaskMgrInterFace::Instance()->CancelTimeoutReq(hashdata.hash,hashdata.data.key,ntohl(req->index),
				ntohl(req->begin),ntohl(req->length));

			return ;
		}

	}
	//限速包处理
	if(CMsgPoolInterface::Instance()->KeyHashExist(hashdata.data.key))
	{		
		int reqsize;
		if(hashdata.data.protocol == QVODBTPRO)
		{
			//点播
			request* req = (request*)hashdata.data.packet;
			if (SendPacket(hashdata.data.key,hashdata.data.packet,hashdata.data.len))
			{
				reqsize = ntohl(((request*)hashdata.data.packet)->length)+13;
				CTaskMgrInterFace::Instance()->UpdateNextDownTime(hashdata.hash,reqsize);
				UpdateNextDownTime(reqsize);
			}
			else
			{
				CTaskMgrInterFace::Instance()->CancelTimeoutReq(hashdata.hash,hashdata.data.key,ntohl(req->index),
					ntohl(req->begin),ntohl(req->length));
				CTaskMgrInterFace::Instance()->DelOnePeer(hashdata.hash,hashdata.data.key);
				CMsgPoolInterface::Instance()->DelKeyHash(hashdata.data.key);
			}
		}		
		else
		{
			//直播
			reqsize = sizeof(livepiece) + BLOCK_LEN;
			SendPacket(hashdata.data.key,hashdata.data.packet,hashdata.data.len);
			UpdateNextDownTime(reqsize);
		}
	}
}

void CMsgPool::DoUpLimitAction()
{
	UpdateNextUpTime(m_lastUpSize);
}

void CMsgPool::AddOneYfDownload( HASH& hash )
{
}

void CMsgPool::SetLimitDownSpeed( int speed )
{
	m_limitDownSpeed = speed;
}

