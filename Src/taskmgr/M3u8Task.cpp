#include "M3u8Task.h"
#include "global.h"
#include "sha1.h"
#include "stund.h"
#include "common-commonfun.h"
#include "TaskMgrInterface.h"
#include "HttpAgentInterface.h"
#include "MsgPoolInterface.h"
#include "common-utility.h"
#include "CacheDataInMem.h"
#include "ServerProduct.h"
#include "CopyTaskFile.h"
#include "../Common/CfgHost.h"
#include "AutoPtr.h"
#include "json.h"
#include "agentinfo.h"
#include "neturl.h"
#include "DbTaskConfig.h"
#include "common-DNSCache.h"

CM3u8Task::CM3u8Task( const unsigned char* hash,std::string strSaveFile,int type,string& strHttpOrgUrl,string& strHttpKeyUrl,string& strHttpReferer, TaskCfg *cfg):
CDownLoadTask(hash, strSaveFile, type, strHttpOrgUrl, strHttpKeyUrl, strHttpReferer),
m_m3u8(NULL),
m_bResetIndex(true)
{
	if (cfg) // 加载任务
	{
		m_Para.strKey = cfg->strKey;
	}

	m_bNeedAes = true;
	m_bAes = false;
	m_m3u8 = new CM3u8(Hash2Char(hash).c_str(),m_strPath.c_str());
    string strPreUrl;
    int nPos = strHttpOrgUrl.rfind("/");
    if (nPos != string::npos)
    {
        strPreUrl = strHttpOrgUrl.substr(0, nPos+1);
    }
	m_m3u8->SetM3u8PreUrl(strPreUrl);
	m_m3u8->m_hash = hash;
	m_m3u8->m_strM3u8Url = strHttpOrgUrl;

	string strKey;
	if (! m_Para.strKey.empty())	// 加载配置key密钥信息
	{
		int nAesVer = 0;
		if (! DecodeAesJson(m_Para.strKey, strKey, nAesVer))
		{
			m_Para.strKey.clear();
			_OUTPUT(INFO, "DecodeAesJson fail! m_Para.strKey=%s", m_Para.strKey.c_str());
		}
	}

	if (m_Para.strKey.empty())	// 无aes密钥则查找目录下是否已有密钥文件
	{
		int nAesVer = 0;
		if (! ReadAesKey(strKey, nAesVer))		// 判断目录下面是已有key
		{
			if (m_bNeedAes)		// 任务文件是否需要加密处理
			{
				unsigned char key[16] = {0};
				strKey = GenerateAesKey(key);		// 判断此时是否已有ts片
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CM3u8Task GenerateAesKey hash=%s  key=%s", Hash2Char(hash).c_str(), strKey.c_str());
#endif
			}
		}
		string strInfo = EncodeAesJson(strKey, nAesVer);
		m_Para.strKey = strInfo;		// 保存加密信息
		if (cfg)
		{
			cfg->strKey = strInfo;
			CDbTaskConfig::Instance()->UpdateTaskKey(m_Para);	// 将key更新到数据库
		}
	}

	if (! m_Para.strKey.empty())
	{
		GenerateAesKeyFile(m_Para.strKey);	// 生成密钥文件
		
		unsigned char key[16] = {0};
		Char2Hash(strKey.c_str(), key, 16);

		string keystr((const char*)key, 16);	
		m_aesdata.SetKey(keystr, g_iMinTerminalVersion);							// 设置密钥
		m_m3u8->m_aesdata.SetKey(keystr, g_iMinTerminalVersion);		// ts片也需加密存储
		m_bAes = true;	 // 设置是否aes ceb模式数据加密
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8Task hash:[%s] m_aesdata.SetKey key=%s", Hash2Char(hash).c_str(), m_Para.strKey.c_str());
#endif
	}
	//if (!ReadAesKey())
	//{
	//	if (m_bNeedAes)		
	//	{
	//		GenerateAesKey();
	//	}
	//}
}

CM3u8Task::~CM3u8Task(void)
{
	if(m_Para.status == CTask::STOP)
	{
		m_m3u8->Remove();
	}
}

bool CM3u8Task::ReadAesKey(string & strAesKey, int &nAesver)
{
	bool bRet = false;
	string strKeyFile = m_strPath + Hash2Char(m_Para.hash) + ".key";
	FILE *fp = fopen(strKeyFile.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0L, SEEK_END);
		long nFileLen = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		if (nFileLen > 0)
		{
			string strKey;
			int nVer = 0;

			bool bExistKey = false;
			QPtr::AutoArrayPtr<char> buf = new char[nFileLen];
			if (1 == fread(buf,nFileLen, 1, fp))
			{
				std::string strInfo(buf, nFileLen);
				if (DecodeAesJson(strInfo, strAesKey, nAesver))
				{
					bExistKey = true;
				}
			}
			if (bExistKey)			
			{
				bRet = true;
	#ifdef QVODDEBUG
				_OUTPUT(INFO, "exist key file!");
	#endif
			}
		}

		fclose(fp);
	}
	return bRet;
}

string	CM3u8Task::GenerateAesKey(unsigned char key[16])
{
	for (unsigned int i=0; i<16; i++)
	{
		key[i] = stunRand() % 0xFF;
	}	
	return Hash2Char(key, 16);
}

string	CM3u8Task::EncodeAesJson(const string& strAesKey,  const int nVer)
{
	Json::Value root;
	Json::FastWriter writer;
	root["key"] = strAesKey;
	root["ver"] = 0 == nVer? g_iMinTerminalVersion: nVer;
	string strInfo = writer.write(root);
	return strInfo;
}

bool CM3u8Task::DecodeAesJson(const string& strInfo, string& strAesKey, int &nVer)
{
	bool bRet = false;
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(strInfo, root) &&
		root.isObject() &&
		root.isMember("key") &&
		root["key"].isString())
	{
		string sKey = root["key"].asString();

		if (sKey.length() >= 32)	// key存在且合法
		{
			bRet = true;
			strAesKey = sKey;
		}
		if (root.isMember("ver") && root["ver"].isInt())
		{
			nVer = root["ver"].asInt();
		}
	}
	return bRet;
}

void CM3u8Task::GenerateAesKeyFile(const string& strInfo)
{
	if (strInfo.empty())
	{
		return;
	}
	string strKeyFile = m_strPath + Hash2Char(m_Para.hash) + ".key";
	if ( QVOD_ACCESS(strKeyFile.c_str(),0) == -1 )	// 不存在key文件 则生成 
	{
		QvodMkdir(m_strPath.c_str());
		FILE *fp = fopen(strKeyFile.c_str(), "wb");
		if (fp)
		{
			fwrite(strInfo.c_str(), strInfo.length(), 1, fp);			// 先保存密钥文件
			fclose(fp);
		}
		else
		{
	#ifdef QVODDEBUG
			_OUTPUT(INFO, "fopen failed! path=%s", strKeyFile.c_str());
	#endif				
		}
	}
}

int CM3u8Task::run(bool bLoad)
{
	int ret = 0;
	if (HLSDOWNTASK == m_Para.type)
	{
		if (bLoad)
		{
			m_m3u8->m_bSpaceMode = ! m_Para.bCopyFullFile;
			if (m_m3u8->IsExist() && m_m3u8->Load())
			{
				return 0;
			}
			else
			{
				m_mem->Remove();
				m_m3u8->Clear();
				m_m3u8->Remove();
				if (m_Para.bCopyFullFile && m_Para.copystatus != 1)
				{
					//任务需要下载完拷贝、且未拷贝，需要重新下载
					return 0;
				}
			}
			return -1;
		}
		else
		{
			m_m3u8->Remove();		// 删除m3u8文件，重新下载
			m_m3u8->Clear();
			return 0;
		}
	}
	else
	{
		ret = CDownLoadTask::run(bLoad);
		if (ret <= 0)
		{
			m_m3u8->Remove();
			m_m3u8->Clear();
#ifdef QVODDEBUG
            _OUTPUT(INFO, "CM3u8Task::run need redownload");
#endif
		}
	}
	return ret;
}

void CM3u8Task::Pause()
{
	CTask::Pause();
	ReSetTsIndexDownloading(-1);
}
void CM3u8Task::UpdateDownloadMode()
{
	if (RUNNING != m_Para.status || TaskIsFinish())
	{
//#ifdef QVODDEBUG
//		_OUTPUT(INFO, "CM3u8Task::UpdateDownloadMode [%s] status:%d ", Hash2Char(m_Para.hash).c_str(), m_Para.status);
//#endif
		return;
	}

	//先下m3u8文件
	if ( !IsExistM3u8() )
	{
		m_iDownloadMode = MODE_CDN;
	}
	else
	{
		bool bOutMem = false;
#ifdef QVOD_USE_CACHEMEM
		if (IsPlaying())		// 只缓存播放的任务到内存
		{
			bOutMem = CCacheDataInMem::Instance().IsOutofMem();
			if (bOutMem && !CCacheDataInMem::Instance().CheckM3u8Ts(m_Para.hash, m_m3u8->GetTsName(m_m3u8->m_iPlayTsIndex)))	// 内存不足但还没缓存当前ts片则清空缓存
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CM3u8Task::UpdateDownloadMode bOutMem=%d clear cachedata 没有缓存当前ts片 m_iPlayTsIndex=%d", m_m3u8->m_iPlayTsIndex);
#endif
				CCacheDataInMem::Instance().ClearCache();
				bOutMem = false;
			}
		}
#endif
		if (bOutMem)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CM3u8Task::UpdateDownloadMode [%s] error out of cachemem, limit download", Hash2Char(m_Para.hash).c_str());
#endif
			m_iDownloadMode = MODE_PAUSE;	 // 内存不足限制下载
		}
		else if (m_Para.bCopyFullFile)
		{
			m_iDownloadMode = MODE_CDN;		// 下载任务不限制
		}
		else
		{
			//缓存当前播放ts后的40秒数据
			unsigned int iContinueTsTime = 0;
			if (m_m3u8->m_bCacheTimeCalBySetTime)
			{
				iContinueTsTime = GetContinuesTime(m_m3u8->m_iCurPlayTime)/1000;
			}
			else 
			{
				iContinueTsTime = m_m3u8->GetContinueTsTimeByIndex(m_m3u8->m_iCurPlayIndex+1) / 1000;
			}
			//if (HLSDOWNTASK == m_Para.type)
			//{
			//	//缓存当前播放ts后的40秒数据
			//	iContinueTsTime = m_m3u8->GetContinueTsTime(m_m3u8->m_iCurPlayIndex+1) / 1000;		// 从当前播放位置点后的ts片算
			//}
			//else
			//{
			//	iContinueTsTime = GetContinuesTime(m_m3u8->m_iCurPlayTime);
			//}

			if ((iContinueTsTime <= g_cfg.m_lMaxCacheTime) && !bOutMem)
			{
				m_iDownloadMode = MODE_CDN;
			}
			else
			{
				m_iDownloadMode = MODE_PAUSE;
			}

#ifdef QVODDEBUG
			int iP2pSpeed = ProcessQueryPeers()->GetSomePeerDownSpeed(NORMAL)/1024;
			int iDcdnSpeed = ProcessQueryPeers()->GetSomePeerDownSpeed(MINER)/1024;
			_OUTPUT(INFO,"PlayTs = %d,ContinueTs = %d,TsDuration = %d,mode = %d,copy = %d,copystatus = %d,p2pspeed = %dKB/s,"\
				"dcdnspeed = %dKB/s,totalspeed = %dKB/s bOutMem=%d\n",m_m3u8->m_iPlayTsIndex,iContinueTsTime,g_cfg.m_lMaxCacheTime,m_iDownloadMode,
				m_Para.bCopyFullFile,m_Para.copystatus,iP2pSpeed,iDcdnSpeed,GetDownSpeed()/1024, bOutMem);
#endif
		}
	}
	
#ifdef QVODDEBUG
		_OUTPUT(INFO, "UpdateDownloadMode [%s] m_iDownloadMode = %d", Hash2Char(m_Para.hash).c_str(), m_iDownloadMode);
#endif
	if (MODE_CDN == m_iDownloadMode)
	{
		// 下载任务
		if (m_Para.bCopyFullFile)
		{
			SM3u8TsReq req;
			int ret = DownNextM3u8Ts(req);
			bool bDownOk = true;
			if (m_tsIndexDownloading.size() < g_cfg.m_uConcurrentNum)
			{
				if (CHttpAgentInterface::Instance()->GetTaskInfo((HASH&)m_Para.hash,NULL) != 0)		// httpagent中没有该hash任务
				{
					bDownOk = CHttpAgentInterface::Instance()->AddM3u8TsTask((HASH&)m_Para.hash,req) > 0;
					if (bDownOk)
					{
						SetTsIndexDownloading(ret);
					}
				}
			}
			while (bDownOk && ret>0 && m_tsIndexDownloading.size() < g_cfg.m_uConcurrentNum)
			{
				if ((ret=DownNextM3u8Ts(req, ret+1)) < 0)
				{
					break;
				}
				if (IsTsIndexDownloading(ret))		// 已在下载序列中
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CM3u8Task::UpdateDownloadMode IsTsIndexDownloading idx=%d", ret);
#endif
					break;
				}

				unsigned char szhash[20] = {0};
				CServerProduct::CreateTaskHash(req.strOrlurl, szhash);
				HASH hash(szhash);
				req.strHash.append((char*)m_Para.hash, 20);		// 需要以ts片的url创建hash放到下载逻辑中
				bDownOk = CHttpAgentInterface::Instance()->AddM3u8TsTask(hash, req) > 0;
				if (bDownOk)
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "AddM3u8TsTask map hash=%s tshash=%s index=%d", Hash2Char(m_Para.hash).c_str(), Hash2Char(hash.data).c_str(), ret);
#endif
					SetTsIndexDownloading(ret);
				}
			}
		}
		else if (CHttpAgentInterface::Instance()->GetTaskInfo((HASH&)m_Para.hash,NULL) != 0)	//若当前没有http任务，则添加任务
		{
			SM3u8TsReq req;
			int ret = DownNextM3u8Ts(req);
			if (ret >= 0)
			{
				if (CHttpAgentInterface::Instance()->AddM3u8TsTask((HASH&)m_Para.hash,req) > 0)
				{
					SetTsIndexDownloading(ret);
				}
			}
			else
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "UpdateDownloadMode error DownNextM3u8Ts=%d", ret);
#endif
			}
		}
	}
}

void CM3u8Task::DownEmergencyData()
{
	if (m_Para.status != RUNNING || TaskIsFinish())
	{
		return ;
	}

	if (m_m3u8->CheckIndex(m_m3u8->m_iPlayTsIndex))
	{
		return ;
	}				

	//当前播放位置没有数据，把状态直接改为MODE_CDN
	m_iDownloadMode = MODE_CDN;
	SM3u8TsReq req;
	if ( DownNextM3u8Ts(req, m_m3u8->m_iPlayTsIndex) < 0)
	{
		return ;
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO,"DownEmergencyData %s start = %lld,len = %d m_iPlayTsIndex=%d\n",req.strOrlurl.c_str(),req.start,req.length, m_m3u8->m_iPlayTsIndex);
#endif
	CHttpAgentInterface::Instance()->AddM3u8TsTask((HASH&)m_Para.hash,req);
    ReSetTsIndexDownloading(-1);
}

bool CM3u8Task::TaskIsFinish(bool bCheckFileExist)
{
	bool bRet = false;

	switch (GetTaskType())
	{
	case HLSDOWNTASK:
		{
			if (m_m3u8)
			{
				bRet =  m_m3u8->IsFinish(bCheckFileExist);
			}
		}
		break;
	default:
		{
			bRet = CDownLoadTask::TaskIsFinish(bCheckFileExist);
		}
		break;
	}

	return bRet;
}

int CM3u8Task::SetPlaying( bool bPlaying )
{
	m_bIsPlaying = bPlaying;

	if (!bPlaying && m_m3u8)
	{
		m_m3u8->ClearPlayingTsVec();
	}

	return 0;
}

unsigned int CM3u8Task::GetPlayFileRate()
{
	return m_m3u8->m_iTsAverageRate;
}

QVOD_INT64 CM3u8Task::GetDownloadedSize()
{
	if (HLSDOWNTASK == GetTaskType())
	{
		if ( m_m3u8 )
		{
			return m_m3u8->GetTotalDownloadSize();
		}
	}
	else
	{
		return CDownLoadTask::GetDownloadedSize();
	}
	return 0;
}

QVOD_INT64 CM3u8Task::GetTotalDownloadLen()
{
	return GetDownloadedSize();
}

void CM3u8Task::SetM3u8PlayingPos( const string& strTsName, int pos )
{
	static bool s_bOnce = false;
	if (! s_bOnce)
	{
		s_bOnce = true;
		CCacheDataInMem::Instance().ClearCache();		// 新点播任务需要清空内存
	}
    m_bIsPlaying = true;
	m_m3u8->SetPlayingTs(strTsName);	
#ifdef QVOD_USE_CACHEMEM
	if (! m_m3u8->IsPlayTsContinueIn2Ts())		// 推流的ts片不连续说明有seek了，需要清空内存中的数据
	{
		CCacheDataInMem::Instance().ClearCache();
	}
#endif

	if (! m_m3u8->IsPlayTsContinueIn2Ts())		// 推流seek了，紧急下载当前片
	{
		m_m3u8->m_iCurPlayIndex = m_m3u8->m_iPlayTsIndex;
		DownEmergencyData();
	}

#ifdef QVODDEBUG
	if (HLSP2PDOWNTASK == m_Para.type)
	{
		if (m_m3u8->m_iPlayTsIndex < m_m3u8->GetTsCount())
		{
			QVOD_INT64 nStart = m_m3u8->GetTsOffset(m_m3u8->m_iPlayTsIndex-1);
			QVOD_INT64 nLen = m_m3u8->GetTsSize(m_m3u8->m_iPlayTsIndex);
			unsigned int nPiecesize = m_seed->GetPieceSize();
			if (nPiecesize)
			{
				_OUTPUT(INFO, "CM3u8Task::SetM3u8PlayingPos pic=[%lld, %lld]", nStart/nPiecesize, (nStart+nLen)/nPiecesize);
			}
		}
	}
#endif
}

bool CM3u8Task::IsPlayTsContinue()
{
	if (m_m3u8->m_iCurPlayTime > 0)
	{
		if (m_m3u8->m_iCurPlayIndex == m_m3u8->m_iPlayTsIndex)	// 当前播放的是推流的片则直接返回false，不限速推送，避免引起卡顿
		{
			return false;
		}
	}
	return m_m3u8->IsPlayTsContinue();
}

QVOD_INT64 CM3u8Task::GetM3u8TsSize( const string& strName )
{
	if (strName.find(".m3u8") != string::npos)
	{
		return m_m3u8->GetM3u8Size();
	}
	else
	{
		return m_m3u8->GetTsSize(strName);
	}
}

bool CM3u8Task::IsExistM3u8Ts(string strTsUrl)
{
    bool bRet = (-1 != m_m3u8->GetTsUrlIndex(strTsUrl));
    return bRet;
}


int CM3u8Task::DeleteHasPlayingData(QVOD_UINT64 iCurCacheSize) 	// 删除当前播放位置前的缓存数据
{
	if ( HLSDOWNTASK == GetTaskType() )
	{
		return m_m3u8->ClearHasPlayingTsData(iCurCacheSize);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
//m3u8文件逻辑

void CM3u8Task::NotifyDownM3u8Ts( const string& strFilename)
{
	int iTsIndex = m_m3u8->GetTsIndex(strFilename);
	ReSetTsIndexDownloading(iTsIndex);
#ifdef QVODDEBUG
	//_OUTPUT(INFO, "CM3u8Task::NotifyDownM3u8Ts index=%d tsname=%s", iTsIndex, strFilename.c_str());
#endif
}

int CM3u8Task::WriteM3u8Ts( const string& strFilename,const char* buf,int start,int len,int filesize )
{	
	int ret = -1;

	int iTsIndex = m_m3u8->GetTsIndex(strFilename);
	ReSetTsIndexDownloading(iTsIndex);
	if (iTsIndex < 0)
	{
		if (! m_m3u8->IsExist())	// m3u8文件被删除了，找不到索引，默为成功继续下载
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CM3u8Task::WriteM3u8Ts error m3u8 file not exist! strFilename=%s ", strFilename.c_str());
#endif
			ret = 0;
		}
#ifdef QVODDEBUG
		if (ret < 0)
		{
			_OUTPUT(ERROR1, "CM3u8Task::WriteM3u8Ts error! m3u8 has no file", strFilename.c_str());
		}
#endif
		return ret;
	}

	switch (GetTaskType())
	{
	case HLSDOWNTASK:
		{
			if ( len != filesize && 0 == start )
			{
				//完整ts才写入文件
				return 0;
			}

			ret = m_m3u8->WriteTs(iTsIndex,buf,len);
	#ifdef QVODDEBUG
			if (ret < 0)
			{
				_OUTPUT(ERROR1, "WriteM3u8Ts ret=%d,iTsIndex=%d, len=%d,  strfilename=%s", ret, iTsIndex, len,  strFilename.c_str());
			}
	#endif
			if ( m_m3u8->IsFinish() )
			{
				if (m_m3u8->IsFinish(true))		// 检测所有ts文件是否存在
				{
					HASH hash(m_Para.hash);
					CServerProduct::SetTaskStatus(hash,CTask::PAUSE);	// 离线完了就暂停任务
					g_cfg.m_nDownPlayingTaskCount = CTaskMgrInterFace::Instance()->GetDownloadingAndPlayingTaskNum();
					CallBackToUI(0, 0, Hash2Char(m_Para.hash).c_str(), 	40 );
				}
			}
		}
		break;
	case HLSP2PDOWNTASK:
		{
			ret = 0;		// 数据写入缓存也正确
			if (0 == start && len == filesize)
			{
				//完整ts才校验签名
				if (0==start && !m_m3u8->CheckTsSign(iTsIndex, buf, len))
				{
	#ifdef QVODDEBUG
					_OUTPUT(ERROR1, "WriteM3u8Ts failed! index=%d start=%d len=%d", iTsIndex, start, len);
	#endif
					return -1;
				}
			}
			CAutoLock lock(m_downNextHttpPieceLock);		// 确保数据写完才下载

			QVOD_INT64 nStartPos = m_m3u8->GetTsOffset(iTsIndex-1);;
			nStartPos += start;
	
			ret = WriteM3u8Data(buf, nStartPos, len);
		}
		break;
	}

	return ret;
}

int CM3u8Task::WriteM3u8Data(const char *buf, const QVOD_UINT64& start, const UINT& len)
{
	int ret = 0;		// 数据写入缓存也正确
	CAutoLock lock(m_downNextHttpPieceLock);		// 确保数据写完才下载

	QVOD_UINT64 nStartPos = start;
	const QVOD_INT64 nEndPos = start + len;

	const QVOD_UINT64 nFileLen = GetFileSize();
	const unsigned int nPieceSize = m_seed->GetPieceSize();
	const unsigned int iStartMod = nStartPos % nPieceSize;
	
	if (iStartMod != 0)		// 写入位置不在分片起始位置上，往前配成分片大小位置，查找是否有下载
	{
		unsigned int nWrite = nPieceSize-iStartMod;
		SM3u8TsSurplusData data;
		if (m_m3u8->FindM3u8TsData(nStartPos-iStartMod, data))		// 查找到当前位置前的分片位置
		{
			if (data.len <= nPieceSize)
			{
				const unsigned int nBufLen = len >= (nPieceSize-data.len) ? nPieceSize-data.len: len;
				const unsigned int nBufSize = nBufLen + data.len;
				if (nBufSize < nPieceSize)		// 不足一个分片数据，则合并数据
				{
					SM3u8TsSurplusData da;
					da.start = data.start;
					da.buf = new char[nBufSize];
					memcpy(da.buf, data.buf, data.len);
					memcpy(da.buf+data.len, buf, nBufLen);
					da.len = nBufSize;

					if (da.start+da.len >= nFileLen)		// 当前是文件末尾位置
					{
						ret = WriteData(da.buf, da.start, nFileLen-da.start);// 写入文件末尾不足分片数据
						//ret = WriteHttpData(da.buf, da.start, nFileLen-da.start);	// 写入文件末尾不足分片数据
						SafeDeleteArray(da.buf);
						return ret;
					}
					m_m3u8->SaveM3u8Data(da);
					return ret;
				}
				QPtr::AutoArrayPtr<char> pBuf = new char[nPieceSize];	// 凑成一个分片数据
				memcpy(pBuf, data.buf, data.len);
				memcpy(pBuf+data.len, buf, nBufLen);

				ret = WriteData(pBuf, data.start, nPieceSize);			// 只写入片数的整数倍
				if (ret > 0)
				{
					m_m3u8->RemoveM3u8TsData(data.start);				// 写入到文件删除缓存数据
				}
			}
		}
		else			// 保存start部分当前不足块的数据
		{
			const unsigned int nBufLen = len >= (nWrite) ? nWrite: len;
			SM3u8TsSurplusData data;
			data.start = nStartPos;
			data.buf = new char[nBufLen];
			memcpy(data.buf, buf, nBufLen);
			data.len = nBufLen;
			m_m3u8->SaveM3u8Data(data);
		}
		nStartPos += nWrite;		// 起始位置固定到分片倍数上
		buf += nWrite;			// buf相应变化

		if (nStartPos >= nFileLen)	// 没有数据或超出文件大小,退出
		{
			return ret;
		}
	}
	unsigned int iModEnd = nEndPos % nPieceSize;
	if (0 !=iModEnd && (nEndPos >= nFileLen))		// 最后一块数据直接写入
	{
		iModEnd = 0;
	}

	QVOD_INT64 bufLen = (nEndPos-nStartPos)- iModEnd;
	if (bufLen > 0)
	{
		if (m_bAes)
		{					
			QPtr::AutoArrayPtr<char> pBuf = new char[bufLen];
			memcpy(pBuf, buf, bufLen);

			ret = WriteData(pBuf, nStartPos, bufLen);			// 只写入片数的整数倍
		}
		else
		{
			ret = WriteHttpData(buf, nStartPos, bufLen);	// 只写入片数的整数倍
		}
	}
	if (ret < 0)	// 写失败
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CM3u8Task WriteHttpData ret=%d", ret);
#endif
		return ret;
	}

	const int nLen = nEndPos - nStartPos;
	if (nLen>0 && iModEnd>0)		// 写入成功，保存剩余不足分片大小的数据
	{
		SM3u8TsSurplusData data;
		if (m_m3u8->FindM3u8TsData(nEndPos, data))		// end部分查找到当前位置前的分片位置
		{
			if (data.len >= nPieceSize-iModEnd)
			{
				QPtr::AutoArrayPtr<char> pBuf = new char[nPieceSize];	// 凑成一个分片数据
				memcpy(pBuf, buf+nLen-iModEnd, iModEnd);
				memcpy(pBuf+iModEnd, data.buf, nPieceSize-iModEnd);

				ret = WriteData(pBuf, nEndPos-iModEnd, nPieceSize);			// 只写入片数的整数倍
				if (ret > 0)
				{
					m_m3u8->RemoveM3u8TsData(data.start);								// 写入到文件删除缓存数据
				}
			}
			else		// 不足一个分片数据，则合并数据
			{
				const unsigned int nBufLen = data.len+iModEnd;
				SM3u8TsSurplusData da;
				da.start = nEndPos-iModEnd;
				da.len = nBufLen;
				da.buf = new char[nBufLen];
				memcpy(da.buf, buf+nLen-iModEnd, iModEnd);
				memcpy(da.buf+iModEnd, data.buf, data.len);

				if (da.start+da.len >= nFileLen)		// 当前是文件末尾位置
				{
					ret = WriteData(da.buf, da.start, nFileLen-da.start);// 写入文件末尾不足分片数据
					//ret = WriteHttpData(da.buf, da.start, nFileLen-da.start);	// 写入文件末尾不足分片数据
					SafeDeleteArray(da.buf);
					return ret;
				}
				m_m3u8->SaveM3u8Data(da);
				return ret;
			}
		}
		else				// 保存当前ts片末尾不足块的数据
		{
			SM3u8TsSurplusData data;
			data.start = nEndPos-iModEnd;
			data.len = iModEnd;
			data.buf = new char[data.len];
			memcpy(data.buf, buf+nLen-data.len, data.len);		
			m_m3u8->SaveM3u8Data(data);
		}
	}
	return ret;
}

int CM3u8Task::ReadM3u8Ts( const string& strFilename, char* buf, int start, int len, bool bIsUpload )
{
	int ret = -1;
	if ( strFilename.find(".m3u8") != string::npos )
	{
		return m_m3u8->ReadM3u8(buf,start,len);
	}
	else
	{
		switch (GetTaskType())
		{
		case HLSDOWNTASK:
			ret = m_m3u8->ReadTs(strFilename,buf,start,len);
			break;
		case HLSP2PDOWNTASK:
			{
				int iIndex = m_m3u8->GetTsIndex(strFilename);
				if (iIndex < 0)
				{
					return ret;
				}
				QVOD_INT64 nStartTsOffset = m_m3u8->GetTsOffset(iIndex-1);
				QVOD_INT64 nStart = nStartTsOffset + start;
				const unsigned int nPieceSize = m_seed->GetPieceSize();
				if (nPieceSize > 0)
				{
					ret = ReadData(buf, len, nStart / nPieceSize, nStart%nPieceSize, false);
					if (ret <= 0)
					{
#ifdef QVODDEBUG
					QVOD_INT64 nEndPos = nStartTsOffset + m_m3u8->GetTsSize(iIndex);
					_OUTPUT(ERROR1, "CM3u8Task::ReadM3u8Ts Error HLSP2PDOWNTASK! index=%d [%lld-%lld]",iIndex, nStartTsOffset, nEndPos);
#endif
					}
				}
				else
				{
#ifdef QVODDEBUG
					//_OUTPUT(ERROR1, "CM3u8Task::ReadM3u8Ts Error HLSP2PDOWNTASK!");
#endif
				}
			}
			break;
		}
	}
	return ret;
}

int CM3u8Task::ReadData(char *buf, UINT len, unsigned int index, unsigned int start,bool bIsUpload)
{
	int ret = 0;
	if (m_bAes)		// 数据需解密
	{
		const int nAesPieceSize = 16384;				// aes加密数据块为16KB	
		unsigned int iStartMod = start % nAesPieceSize;
		unsigned int iLenMod = len % nAesPieceSize;
				
		QVOD_INT64 nStartOffset = start;
		QVOD_INT64 nLen = len;

		if (iStartMod != 0 || iLenMod != 0)		// 判断nStart是否在16KB倍数位置上，及len是否为块倍数
		{
			if (iStartMod != 0)		// 读取位置不在16KB倍数位置上，则向前多读取16KB数据以获取当前解密数据
			{
				nStartOffset -= iStartMod;
				nLen += iStartMod;
				iLenMod = nLen % nAesPieceSize;
			}
			if (iLenMod != 0)
			{
				//if (nStartOffset+nLen+(nAesPieceSize-iLenMod)<= GetFileSize())	// 不是文件最后一块，则向后多读取16KB数据
				{
					nLen += (nAesPieceSize-iLenMod);
				}
			}
		}

		if (nStartOffset != start || nLen != len)
		{
			QPtr::AutoArrayPtr<char> pBuf = new char[nLen];	
			ret = CDownLoadTask::ReadData(pBuf, nLen, index, nStartOffset, false);
			if (ret > 0)
			{
				for (int i=0; i<nLen/nAesPieceSize; i++)
				{
					char *pos = pBuf+i*nAesPieceSize;
					m_aesdata.Decrpyt(pos, pos, nAesPieceSize);
				}
				memcpy(buf, pBuf+iStartMod, len);		// 拷贝解密后的数据
				ret = len;
			}
			else
			{
//#ifdef QVODDEBUG
//				const unsigned int nPieceSize = m_seed->GetPieceSize();
//				_OUTPUT(ERROR1, "CM3u8Task::ReadM3u8Ts Error startidx=%d endidx=%d start=%d len=%d!", index, (index*nPieceSize+start+len)/nPieceSize, index*nPieceSize+start, len);
//#endif
			}
		}
		else
		{
			ret = CDownLoadTask::ReadData(buf, len, index, start, false);
			if (ret > 0)
			{
				for (int i=0; i<len/nAesPieceSize; i++)
				{
					char *pos = buf+i*nAesPieceSize;
					m_aesdata.Decrpyt(pos, pos, nAesPieceSize);
				}
			}
			else
			{
//#ifdef QVODDEBUG
//				const unsigned int nPieceSize = m_seed->GetPieceSize();
//				_OUTPUT(ERROR1, "CM3u8Task::ReadM3u8Ts Error startidx=%d endidx=%d start=%d len=%d!", index, (index*nPieceSize+start+len)/nPieceSize, index*nPieceSize+start, len);
//#endif
			}
		}
	}
	else
	{
		ret = CDownLoadTask::ReadData(buf, len, index, start, bIsUpload);
	}

	if (ret <= 0)	// 查找是否在缓存里
	{
		const unsigned int nPieceSize = m_seed->GetPieceSize();
		QVOD_INT64 nStart = index*nPieceSize+start;
		ret = m_m3u8->ReadM3u8TsData(nStart, buf, len);			// 查找到当前位置前的分片位置

#ifdef QVODDEBUG
		if (ret>0)
		{
			_OUTPUT(INFO, "CM3u8Task::ReadData  from M3u8TsData start=%lld len=%d", nStart, ret);
			//if ( len > ret)
			//{
			//	nStart += ret;
			//	ret += ReadData(buf+ret, len-ret,  nStart / nPieceSize, nStart%nPieceSize, false);
			//}
		}
		else
		{
			_OUTPUT(INFO, "CM3u8Task::ReadData ret=%d picec range[%lld,%lld]", ret, index, (nStart+len)/nPieceSize);
		}
#endif

	}
	return ret;
}

int CM3u8Task::WriteData(char *buf,  QVOD_UINT64 start,UINT len)
{
	// 加密操作
	if (m_bAes)
	{
		const int nAesPieceSize = 16384;				// aes加密数据块为16KB
		for (int i=0; i<len/nAesPieceSize; i++)		// 每16KB分块加密
		{
			char *pos = buf+i*nAesPieceSize;
			m_aesdata.Encrpyt(pos, pos, nAesPieceSize);
		}
#ifdef QVODDEBUG
		if (start % nAesPieceSize != 0)
		{
			_OUTPUT(INFO, "CM3u8Task::WriteData start not in 16kb! start=%lld iMod=%lld", start, start%nAesPieceSize);
		}
		if (len % nAesPieceSize != 0)
		{
			_OUTPUT(INFO, "CM3u8Task::WriteData len not in 16kb! len=%lld iMod=%lld", len, len%nAesPieceSize);
		}
#endif
	}
	return  WriteHttpData(buf, start, len);				// 只写入片数的整数倍
}

int CM3u8Task::DownNextM3u8Ts( SM3u8TsReq& req, const int& iTsIndex)
{
	CAutoLock lock(m_downNextHttpPieceLock);

	int iDownloadMode = m_iDownloadMode;

	//当前播放ts，当前播放位置
	unsigned int iPlayTsIndex = iTsIndex>0? iTsIndex: m_m3u8->m_iCurPlayIndex;	// iTsIndex<0‘Úƒ¨»œ¥”µ±«∞≤•∑≈µƒts∆¨Õ˘∫Ûœ¬‘ÿ
	
	if (m_Para.bCopyFullFile)	//下载任务，非播放状态，从0片连续下载
	{
        iPlayTsIndex = IsPlaying()? m_m3u8->m_iPlayTsIndex: 0;
	}
	else if (!m_m3u8->CheckIndex(m_m3u8->m_iPlayTsIndex))	// 推流的片没有下载
	{
		int tsTime = m_m3u8->GetTsIntervalDuration(m_m3u8->m_iCurPlayIndex, m_m3u8->m_iPlayTsIndex)/1000; // ms->s
		if (tsTime <= g_cfg.m_lMaxCacheTime)			// 缓冲时间小于设定值，优先下载推流的ts片
		{
			iPlayTsIndex = m_m3u8->m_iPlayTsIndex;
		}
	}
	switch (iDownloadMode)
	{
	case MODE_CDN:
		{
			req.strReferer = m_Para.strRefUrl;

			if (! IsExistM3u8())
			{
				//m3u8文件不存在，下载m3u8文件
				req.type = 0;
				req.length = 0;
				req.start = 0;

				if (!m_m3u8->m_strM3u8Cdn.empty())
				{
					//只用一次，失败则使用m_m3u8->m_strM3u8Orgcdn
					req.strOrlurl = m_m3u8->m_strM3u8Cdn;
					m_m3u8->m_strM3u8Cdn.clear();
				}
				else if (!m_m3u8->m_strM3u8Orgcdn.empty())
				{
					//只用一次，失败则使用strHttpOrgUrl
					req.strOrlurl = m_m3u8->m_strM3u8Orgcdn;
					m_m3u8->m_strM3u8Orgcdn.clear();
				}
				else
				{
					req.strOrlurl = m_Para.strHttpOrgUrl;
				}
#ifdef QVODDEBUG
				_OUTPUT(INFO, "DownNextM3u8Ts %s", req.strOrlurl.c_str());
#endif
				return 0;
			}

			req.type = 1;
			req.start = 0;
			req.length = 0;

			if (! m_Para.bCopyFullFile && IsPlaying())
			{
				m_m3u8->ClearReseveTs();
			}

			string strUnhaveTsName;

			if (HLSDOWNTASK == m_Para.type)
			{
				strUnhaveTsName = m_m3u8->GetUnhaveTsFromIndex(iPlayTsIndex);
				if (strUnhaveTsName.empty() && m_Para.bCopyFullFile)	// 下载任务重新检测一遍
				{
					iPlayTsIndex = 0;
					strUnhaveTsName = m_m3u8->GetUnhaveTsFromIndex(iPlayTsIndex);

					if (m_bResetIndex)			// 重新检测一遍是否已下载完
					{
						m_bResetIndex = false;
						ReSetTsIndexDownloading(-1);
					}
				}
				if (m_Para.bCopyFullFile)		// 离线任务判断是否在已下队列中
				{
					while (IsTsIndexDownloading(iPlayTsIndex) || m_m3u8->CheckIndex(iPlayTsIndex))
					{
						iPlayTsIndex ++;
					}
					strUnhaveTsName = m_m3u8->GetTsName(iPlayTsIndex);
				}
#ifdef QVOD_USE_CACHEMEM
				if (IsPlaying())		// 只缓存播放的任务到内存
				{
                    while (CCacheDataInMem::Instance().CheckM3u8Ts(m_Para.hash, strUnhaveTsName)|| m_m3u8->CheckIndex(iPlayTsIndex))
					{
						strUnhaveTsName = m_m3u8->GetTsName(++iPlayTsIndex);
					}
				}
#endif
			}
			else
			{
				int ret = 0;
				YfHttpPieceReq yfreq;
				if (m_iPlayPos > 0 || 0 == iPlayTsIndex)
				{
					ret = DownNextHttpPiece(yfreq);
					if (0 == ret)
					{
						SM3u8TsSurplusData data;
						if (m_m3u8->FindM3u8TsData(yfreq.start, data))
						{
							yfreq.start += data.len;
						}

						iPlayTsIndex = m_m3u8->FindTsByOffset(yfreq.start);
					}
				}

				if (iPlayTsIndex >= m_m3u8->GetTsCount())
				{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "DownNextM3u8Ts error iPlayTsIndex=%d count=%d", iPlayTsIndex, m_m3u8->GetTsCount());
#endif
					return -1;
				}
				const unsigned int iPlayTsOrgIndex = iPlayTsIndex;
				strUnhaveTsName = GetUnhaveTsFromIndex(iPlayTsIndex, req.start, req.length);
			}
			
			if (strUnhaveTsName.empty())
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "DownNextM3u8Ts strUnhaveTsName is empty,  iPlayTsIndex=%d", iPlayTsIndex);
#endif
				return -1;
			}
            req.uFilePos = m_m3u8->GetTsOffset(iPlayTsIndex);
            if (0 == strUnhaveTsName.find("http://")) // ts片是绝对路径
            {
                req.strOrlurl = strUnhaveTsName;
            }
            else
            {
				string strPreUrl = m_m3u8->m_strM3u8PreUrl;
                req.strOrlurl = strPreUrl + strUnhaveTsName;
            }
			return iPlayTsIndex;
		}
		break;
	default:
		{
			return -2;
		}
	}

	return -1;
}

bool CM3u8Task::WriteM3u8File(const char* buf, const int& len)
{
	bool bRet = false;
	if (m_m3u8)
	{
		bRet = m_m3u8->Save(buf,len);
		if (! bRet)
		{
			m_m3u8->Remove();
			return bRet;
		}

		if (HLSP2PDOWNTASK == m_Para.type)					// 当前任务为P2PHLS任务，判断是否有文件下载
		{
			if (GetFileSize()>0 && (GetFileSize() != m_m3u8->GetAllTsSize()))		// 更新url后文件大小不对，则认为是不同的文件，需删除已下载的文件重新下载
			{
				RemoveTmpFile(true);										// 删除缓存文件夹及种子进度文件
			}
		}
		SetDownload(/*m_m3u8->GetAllTsSize() > 0 &&*/ m_Para.bCopyFullFile);	// 有文件大小并且为下载任务，则修改为HLSP2P任务
	}
	return bRet;
}

unsigned int	CM3u8Task::GetContinuesTime(const unsigned int& nPlayTime)
{
	unsigned ret = 0 ;
	if (HLSDOWNTASK == m_Para.type)
	{
		if (m_m3u8)
		{
			ret = m_m3u8->GetContinueTsTime(nPlayTime);
		}
	}
	else 
	{
		if (TaskIsFinish())
		{
			return m_m3u8->GetDuration();
		}
		if (m_m3u8->m_iCurPlayIndex > 0)
		{
			QVOD_INT64 iPlayFileRate = GetPlayFileRate()/8;
			iPlayFileRate = (0 == iPlayFileRate)?131072:iPlayFileRate;							//若码率为0，则默认为1Mbps
			//QVOD_INT64 iPlayPos = iPlayFileRate*(nPlayTime/1000);						// 从当前播放时间得出当前播放点的位置 Byte
			QVOD_INT64 iTsOffsetPos = m_m3u8->GetTsOffset(m_m3u8->m_iCurPlayIndex-1);
			QVOD_INT64 iContinueLen = m_mem->GetContinueLen(iTsOffsetPos);
			iContinueLen += iTsOffsetPos;

			int iEndIndex = m_m3u8->FindTsByOffset(iContinueLen);
			ret = m_m3u8->GetPrevTsDuration(iEndIndex);
			ret = ret > nPlayTime? ret-nPlayTime: 0;
			//ret = m_m3u8->GetPrevTsDuration(iEndIndex) - nPlayTime;
			//ret = (unsigned)(iContinueLen/iPlayFileRate)*1000;								// 得到当前位置下的缓存时长
			//ret -= nPlayTime;

			//QVOD_INT64 iContinueLen = m_mem->GetContinueLen(iPlayPos);
			//ret = (unsigned)(iContinueLen/iPlayFileRate)*1000;								// 得到当前位置下的缓存时长
		}
		else
		{
			QVOD_INT64 iPlayFileRate = GetPlayFileRate()/8;
			iPlayFileRate = (0 == iPlayFileRate)?131072:iPlayFileRate;							//若码率为0，则默认为1Mbps
			QVOD_INT64 iPlayPos = iPlayFileRate*(nPlayTime/1000);							// 从当前播放时间得出当前播放点的位置 Byte
			QVOD_INT64 iContinueLen = m_mem->GetContinueLen(iPlayPos);
			ret = (unsigned)(iContinueLen/iPlayFileRate)*1000;								// 得到当前位置下的缓存时长
		}

	}
	return ret;
}

unsigned int CM3u8Task::SetPlayingTimepoint(const unsigned int& timepoint,bool bSeek)
{
	if (m_m3u8)
	{
		m_m3u8->SetPlayingTimepoint(timepoint, bSeek);
	}
	//m_iPlayPos = GetPlayFileRate() * (timepoint/1000) / 8;
	unsigned ret = GetContinuesTime(timepoint);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8Task::SetPlayingTimepoint timepoint:%d, cachetime:%d curplayidx=%d playidx=%d m_iPlayPos=%lld", timepoint, ret, m_m3u8->m_iCurPlayIndex, m_m3u8->m_iPlayTsIndex, m_iPlayPos);
#endif
	return ret;
}

string	CM3u8Task::GetUnhaveTsFromIndex(unsigned int& index, QVOD_UINT64& start, unsigned int& len)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "GetUnhaveTsFromIndex pre idx=%d", index);
#endif
	const unsigned int iOrgIndex = index;
	start = 0;
	len = 0;
	if (HLSP2PDOWNTASK == m_Para.type)
	{
		unsigned int nSize = m_m3u8->GetTsCount();
		unsigned idx = index;
		for (; idx<nSize; idx++)
		{
			if (! CheckTsInMem(idx))
			{
				index = idx;
				break;
			}
		}

		if (idx >= nSize)
		{
			if (m_Para.bCopyFullFile)		// 下载任务则重头检测
			{
				for (idx=0; idx<index; idx++)
				{
					if (! CheckTsInMem(idx))
					{
						index = idx;
						break;
					}
				}

				if (idx > index)		// 整个文件已下载完成
				{
					return string("");
				}
			}
			else
			{
				return string("");
			}
		}
	}

	if (m_Para.bCopyFullFile)
	{
		while (IsTsIndexDownloading(index) )
		{
			index ++;
		}
	}
#ifdef QVODDEBUG
	const unsigned int nPieceSize = m_seed->GetPieceSize(); 
	QVOD_INT64 nstart = m_m3u8->GetTsOffset(index-1); ;		// 相对于ts片的起始位置
	QVOD_INT64 length = m_m3u8->GetTsSize(index);
	_OUTPUT(INFO, "GetUnhaveTsFromIndex after idx=%d piece range[%lld,%lld]", index, nstart/nPieceSize, (nstart+length)/nPieceSize);
#endif
	return m_m3u8->GetTsName(index);
}

bool CM3u8Task::CheckTsInMem(unsigned int& index)
{
	bool bRet = false;

	YfHttpPieceReq yfreq;
	yfreq.start = m_m3u8->GetTsOffset(index-1); ;		// 相对于ts片的起始位置
	yfreq.length = m_m3u8->GetTsSize(index);

	const unsigned int nPieceSize = m_seed->GetPieceSize();
	if (0 == nPieceSize)
	{
		return bRet;
	}
	unsigned int iStartMod = yfreq.start % nPieceSize;														// 开始位置的块数据偏移
	unsigned int iEndMod = (yfreq.start+yfreq.length) % nPieceSize;							// 末尾一块数据的偏移
						
	unsigned int iStartIndex = (yfreq.start-iStartMod)/nPieceSize;
	unsigned int iEndIndex =  (yfreq.start+yfreq.length)/nPieceSize;

	unsigned int idx = iStartIndex;
	for (; idx <= iEndIndex; idx++)
	{
		if (! m_mem->CheckIndex(idx))
		{
			if (idx == iStartIndex)				// 当前ts片的第一个块数据没有下载
			{
				SM3u8TsSurplusData data;
				if (! m_m3u8->FindM3u8TsData(yfreq.start-iStartMod, data))								// 当前ts片的第一个块没有下载，则下载前一个ts片的数据
				{
					if (m_m3u8->FindM3u8TsData(yfreq.start, data))													// 当前ts片的第一部分块已下载
					{
						continue;
					}
					//index = index > 1? index-1: 0;
					//QVOD_UINT64 nStart = yfreq.start-iStartMod;
					//while ( index>0 && (nStart < m_m3u8->GetTsOffset(index-1)))						// «∞“ªts∆¨≥§∂»≤ª◊„nLen£¨µ›πÈ«∞“ªts∆¨
					//{
					//	nStart -= m_m3u8->GetTsSize(index);
					//	index = index > 1? index-1: 0;
					//}
					break;
				}
				else
				{
					if (data.start + data.len >= yfreq.start+yfreq.length)				// µ±«∞tsµƒ ˝æ›“—æ≠‘⁄ª∫¥Ê÷–
					{
						bRet = true;
						break;
					}
				}
			}
			else if (idx == iEndIndex)
			{
				SM3u8TsSurplusData data;
				if (m_m3u8->FindM3u8TsData(yfreq.start+yfreq.length-iEndMod, data) && (data.len>=iEndMod))			// ≈–∂œµ±«∞ts∆¨µƒ◊Ó∫Û“ªøÈ”–œ¬‘ÿ£¨‘Úœ¬‘ÿœ¬“ª∏ˆts∆¨
				{
					bRet = true;
					break;
				}
                if (m_m3u8->FindM3u8TsData(yfreq.start, data) && (data.len>=iEndMod)) // 该片已在缓存中 无需再下载
                {
                    bRet = true;
                    break;
                }
			}
			break;
		}
	}
	if (! bRet)
	{
		bRet = idx > iEndIndex;
	}
	return bRet;	
}

void CM3u8Task::SetAesEncrypt(bool bEncrypt)
{
	m_bNeedAes = bEncrypt;
}

void CM3u8Task::SetDownload(bool bDownload)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8Task::SetDownload bDownload=%d", bDownload);
#endif
	m_m3u8->m_bSpaceMode = !bDownload;
	if (bDownload)
	{
		if (m_m3u8->GetAllTsSize() > 0)		// 没有ts大小 则下载所有ts片文件
		{
			SetFilesize(m_m3u8->GetAllTsSize());
			ChangeTaskType(HLSP2PDOWNTASK);
			// 删除ts片文件
			m_m3u8->ClearTsData(0, -1);
		}
	}
	else
	{
		ChangeTaskType(HLSDOWNTASK);
		if (m_m3u8->GetAllTsSize() > 0)
		{
			// 删除合并文件
			RemoveTmpFile(true);
		}
	}
	if (m_bNeedAes)	// 下载文件需要加密
	{
		// 将密钥文件写在当前任务文件夹下
		if (! m_Para.strKey.empty())
		{
			GenerateAesKeyFile(m_Para.strKey);
		}
	}
}

void CM3u8Task::UpdateHttpOrgUrl( string& strHttpOrgUrl )
{
	if (m_Para.strHttpOrgUrl != strHttpOrgUrl)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8Task UpdateHttpOrgUrl orgurl=[%s] new url=[%s]", m_Para.strHttpOrgUrl.c_str(), strHttpOrgUrl.c_str());
#endif
		CDownLoadTask::UpdateHttpOrgUrl(strHttpOrgUrl);

		if (TaskIsFinish())	// 下载完成直接返回
		{
			return;
		}
		//Pause();										// 先暂停任务下载

        UpdateM3u8OrgUrl(strHttpOrgUrl);
	}
}

int CM3u8Task::UpdateClarityUrl(string& strHttpOrgUrl,  string& strNewHash, string& strNewClarityPath, int nClarity)
{
	if (m_Para.bCopyFullFile)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8Task UpdateClarityUrl fail!  [%s] is download task newurl=%s", Hash2Char(m_Para.hash).c_str(), strHttpOrgUrl.c_str());
#endif
		return -1;
	}
    if (m_Para.strHttpOrgUrl != strHttpOrgUrl)
    {
#ifdef QVODDEBUG
        _OUTPUT(INFO, "CM3u8Task UpdateClarityUrl orgurl=[%s] new url=[%s]", m_Para.strHttpOrgUrl.c_str(), strHttpOrgUrl.c_str());
#endif
        CDownLoadTask::UpdateHttpOrgUrl(strHttpOrgUrl);
        
        if (TaskIsFinish())	// 下载完成直接返回
        {
            return 0;
        }
        //Pause();										// 先暂停任务下载
        int iPushIndex = m_m3u8->GetPushIndex();
        // 无播放或无推流状态或当前推流片已下载 直接更新M3U8 URL
        if (! IsPlaying() || -2 == iPushIndex /*|| (iPushIndex>0 && m_m3u8->CheckIndex(iPushIndex))*/)
        {
            UpdateM3u8OrgUrl(strHttpOrgUrl, true);
        }
        else
        {
            m_m3u8->ClearPlayingTsVec();		// 加快推送速度，不限速，加快当前推流片结束 以更新M3U8 URL
        }
    }
	return 0;
}

void CM3u8Task::UpdateM3u8OrgUrl(string& strHttpOrgUrl, bool bChangeClarity)
{
	if (m_m3u8->m_strM3u8Url != strHttpOrgUrl)
	{
		if (TaskIsFinish())	// 下载完成直接返回
        {
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8Task::UpdateM3u8OrgUrl fail! task is finished! newurl: %s bCopyFullFile:%d m_bIsPlaying=%d bChangeClarity=%d", strHttpOrgUrl.c_str(), m_Para.bCopyFullFile, m_bIsPlaying, bChangeClarity);
#endif
            return ;
        }
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8Task::UpdateM3u8OrgUrl newurl: %s bCopyFullFile:%d m_bIsPlaying=%d bChangeClarity=%d", strHttpOrgUrl.c_str(), m_Para.bCopyFullFile, m_bIsPlaying, bChangeClarity);
#endif
		HASH hash(m_Para.hash);
		CMsgPoolInterface::Instance()->DelOneTask(hash);			// 删除下载

		{
			CAutoLock lock(m_downNextHttpPieceLock);
			m_m3u8->UpdateM3u8OrgUrl(strHttpOrgUrl, m_Para.bCopyFullFile, m_bIsPlaying, bChangeClarity);
		}		
#ifdef QVOD_USE_CACHEMEM
		CCacheDataInMem::Instance().ClearCacheByHash(hash);		// 清空该任务的内存缓存		
#endif
		UpdateDownloadMode(); 
	}
}

int CM3u8Task::SetCopyFullFile( string& strCopyPath )
{
	int ret = CDownLoadTask::SetCopyFullFile(strCopyPath);
	if (m_Para.type == HLSP2PDOWNTASK && m_Para.bCopyFullFile)		// 已经是下载任务
	{
		return ret;
	}
	SetDownload(/*m_m3u8->GetAllTsSize() > 0 &&*/ m_Para.bCopyFullFile);	// m3u8文件带有ts片大小,则设置为HLSP2PDOWNTASK任务
	return ret;
}

int CM3u8Task::UnsetCopyFullFile()
{
	int ret = CDownLoadTask::UnsetCopyFullFile();
	//if (m_Para.type == HLSDOWNTASK)		// 已经是下载任务
	//{
	//	return ret;
	//}
	SetDownload(false);	// 取消下载
	return ret;
}

bool CM3u8Task::IsExistM3u8()
{
	bool bExist = m_m3u8->IsExist() || !m_m3u8->m_strM3u8FileContent.empty();
	return bExist;
}

void CM3u8Task::NotifyVideoBuffering(bool bBuffering)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8Task::NotifyVideoBuffering [%s] bBuffering = %d", Hash2Char(m_Para.hash).c_str());
#endif
	CTask::NotifyVideoBuffering(bBuffering);
}

void CM3u8Task::SetTsIndexDownloading(const unsigned int &index)
{
	if (index > 0)
	{
		CAutoLock lock(m_tsIndexLock);
		m_tsIndexDownloading.insert(index);
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8Task::SetTsIndexDownloading index=%d count=%d", index, m_tsIndexDownloading.size());
#endif
	}
}
void CM3u8Task::ReSetTsIndexDownloading(const unsigned int &index)
{
	CAutoLock lock(m_tsIndexLock);
	m_tsIndexDownloading.erase(index);
	if (-1 == index)
	{
		m_tsIndexDownloading.clear();
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8Task::ReSetTsIndexDownloading index=%d count=%d", index, m_tsIndexDownloading.size());
#endif
}
bool CM3u8Task::IsTsIndexDownloading(const unsigned int& index)
{
	CAutoLock lock(m_tsIndexLock);
	return m_tsIndexDownloading.find(index) != m_tsIndexDownloading.end() ;
}
int CM3u8Task::GetFirstTsIndexDownloading()
{
	int idx = -1;
	CAutoLock lock(m_tsIndexLock);
	if (m_tsIndexDownloading.size() > 0 )
	{
		set<int>::const_iterator itr = m_tsIndexDownloading.begin();
		if (itr != m_tsIndexDownloading.end() )
		{
			idx = *itr;
		}
	}
	return idx;
}

string CM3u8Task::QueryTaskInfo()
{
#ifdef QVODDEBUG
    _OUTPUT(INFO, "begin %s %d", __FUNCTION__, __LINE__);
#endif
    
	std::string str = "{}";
	if (NULL == m_m3u8)
	{
		return str;
	}
	Json::Value ext;
	try {
        
#ifdef QVODDEBUG
        _OUTPUT(INFO, " %s %d", __FUNCTION__, __LINE__);
#endif
        
		ext["download_mode"] = 0==m_iDownloadMode?"download":"suspend";
		ext["current_task_state"] = m_Para.status;
		ext["push_index"] = m_m3u8->m_bPushM3u8?-1:m_m3u8->m_iPlayTsIndex;
		ext["current_ts_index"] = m_m3u8->m_iCurPlayIndex;
		ext["avg_speed"] = GetDownSpeed();
		ext["total_download"] = (unsigned int)GetDownloadedSize();

#ifdef QVODDEBUG
        _OUTPUT(INFO, " %s %d", __FUNCTION__, __LINE__);
#endif
		string strUrl = m_Para.strHttpOrgUrl;
		int downindex = GetFirstTsIndexDownloading();

		if (downindex >= 0)
		{
			string strUnhaveTsName = m_m3u8->GetTsName(downindex);
			string strPreUrl = m_m3u8->m_strM3u8PreUrl;
			strUrl = strPreUrl + strUnhaveTsName;			
		}

#ifdef QVODDEBUG
        _OUTPUT(INFO, " %s %d", __FUNCTION__, __LINE__);
#endif
        
		ext["download_index"] = downindex;
		std::string strHost, strParams;
		NetUrl::GetURLHost(strUrl, strHost, strParams);

		ext["url"] = strUrl;
		ext["cdn_host"] = strHost;
		IPAddrInfo addr;
		CDNSCache::Instance()->Resolve(strHost, addr);
		ext["cdn_ip"] =addr.strIP;	

#ifdef QVODDEBUG
        _OUTPUT(INFO, " %s %d", __FUNCTION__, __LINE__);
#endif
		//if (downindex >= 0)
		//{
		//	int downloadlen, totalen = 0;
		//	CHttpAgentInterface::Instance()->GetAgentDownload(HASH(m_Para.hash), downloadlen, totalen);
		//	ext["download_url_length"] = (unsigned)downloadlen;
		//	ext["current_url_length"] = (unsigned )totalen;
		//}

		Json::FastWriter writer;
		str = writer.write(ext);
	} catch (...){
#ifdef QVODDEBUG
		_OUTPUT(INFO, "catch QueryTaskInfo");
#endif
	}
    
#ifdef QVODDEBUG
    _OUTPUT(INFO, "end %s %d", __FUNCTION__, __LINE__);
#endif
	return str;
}


// 推流socket已断开
void CM3u8Task::ClosePush( const string& strFilename)
{
	// 在这里做url更新操作
	m_m3u8->ResetPlayingTs(strFilename);
	// 注意 osx平台没有清晰度切换操作 不需要切换
#ifndef MACOSX
	if (0 != m_Para.strHttpOrgUrl.compare(m_m3u8->m_strM3u8Url))			// 新的url在推送结束后更新
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8Task::ClosePush updatem3u8 orgurl=%s", m_Para.strHttpOrgUrl.c_str());
#endif
		UpdateM3u8OrgUrl(m_Para.strHttpOrgUrl, true);
	}
#endif
}
