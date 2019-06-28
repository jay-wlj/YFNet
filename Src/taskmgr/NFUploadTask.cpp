#include "NFUploadTask.h"
#include "common-utility.h"
#include "common-commonfun.h"
#include "common-callback.h"
#include "HttpAgentInterface.h"
#include "json.h"
#include "AutoPtr.h"
#include "HttpUploadInterface.h"
#include "sha1.h"
#include "qvod_time.h"
#include "cfghost.h"
#include "ServerProduct.h"
#include "openssl/md5.h"
#include "cfghost.h"
#include <algorithm>  
#include<iostream> 
#include <iterator>
#include <set> 
using namespace std; 


CNFUploadTask::CNFUploadTask(const unsigned char* hash, int type, const string& strOrgUrl, const string& strFilePath):
m_uchunksize(0),
m_uHasUploadSize(0),
m_filesize(0)
{
	m_Para.strHttpOrgUrl = strOrgUrl;
	memcpy(m_Para.hash, hash, 20);
	m_Para.type = type;
	int pos = strFilePath.rfind("\\")+1;
	m_strPath = strFilePath.substr(0,pos);
	m_strFileName = strFilePath.substr(pos,strFilePath.length() - pos);
	m_bFinishUpload = false;
	m_strFilePath = strFilePath;

	//string strSeedName = strFilePath;
	//std::replace(strSeedName.begin(), strSeedName.end(), ':', '_');
	//std::replace(strSeedName.begin(), strSeedName.end(), '/', '_');
	//std::replace(strSeedName.begin(), strSeedName.end(), '\\', '_');
	//m_seed = new CSeed(strSeedName.c_str());
	//m_mem = new CMem(strSeedName.c_str());

	qvod_stat_t buffer;
	if(0 != QVOD_STAT(m_strFilePath.c_str(), &buffer))
	{
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "CNFUploadTask::GetFileSize stat fail: %s\n", m_strFilePath.c_str());
#endif
	}
	m_filesize = buffer.st_size;
	string strPlatfrom = "pc";
#ifdef WIN32
	strPlatfrom = "pc";
#elif MACOSX
	strPlatfrom = "ios";
#else
	strPlatfrom = "android";
#endif
	m_sign.AddHead("X-YF-Platform", strPlatfrom);
	m_sign.AddHead("X-YF-Version", g_cfg.m_strAppVer);
	m_sign.AddHead("X-YF-filesize", m_filesize);
	m_sign.AddHead("X-YF-width", 0);
	m_sign.AddHead("X-YF-height", 0);
	m_sign.AddHead("X-YF-duration", 0);
}


CNFUploadTask::~CNFUploadTask(void)
{
	Pause();
}

int CNFUploadTask::run(bool bload)
{
	//检查文件是否存在
	if( QVOD_ACCESS( (m_strPath + m_strFileName).c_str(),0 ) == -1 )
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CNFUploadTask Task:File Not exist!Delete Task(%s)!!\n",m_strFileName.c_str());
#endif // _DEBUG
		return -1;
	}
    return 0;
}

void CNFUploadTask::Start()
{
	CTask::Start();
	UpdateDownloadMode();
}

int CNFUploadTask::WriteHttpData(const char *buf, QVOD_UINT64 start,UINT len)
{
	int ret = 0;
	string strBuf(buf, len);

	bool bFinish = false;

	m_vNeedChunkdx.clear();

	Json::Reader reader;
	Json::Value root;
	Json::Value data;
	if (reader.parse(strBuf, root) &&
		root.isObject())
	{
		bool bTrue = true;
		if (root.isMember("ok") && root["ok"].isBool())
		{
			bTrue = root["ok"].asBool();
		}
		if (bTrue)
		{
			if (root.isMember("data") && root["data"].isObject())
			{
				Json::Value data = root["data"];
				if (data.isMember("chunksize") && data["chunksize"].isInt())
				{
					m_uchunksize = data["chunksize"].asInt();
				}
				if (data.isMember("notcompleted_chunks") && data["notcompleted_chunks"].isArray())
				{
					Json::Value vChunks = data["notcompleted_chunks"];
					for (int i=0; i< vChunks.size(); i++)
					{
						if (vChunks[i].isIntegral())
						{
							m_vNeedChunkdx.insert(vChunks[i].asUInt());
						}
					}

					if (0 == m_vNeedChunkdx.size())
					{
						m_uHasUploadSize = m_filesize;
						bFinish = true;
					}
				}
				if (data.isMember("completed_chunks") && data["completed_chunks"].isArray())
				{
					Json::Value vChunks = data["completed_chunks"];
					for (int i=0; i< vChunks.size(); i++)
					{
						m_vHasChunkdx.insert(vChunks[i].asUInt());
					}
				}
			}
		}
	}
	
	set<unsigned int> diff;
	set_difference(m_vNeedChunkdx.begin(), m_vNeedChunkdx.end(),
		m_vHasChunkdx.begin(), m_vHasChunkdx.end(), inserter(diff, diff.begin()));
	m_vNeedChunkdx = diff;

	if (0 == m_vNeedChunkdx.size() || bFinish)	// 没有需要上传了
	{
		m_bFinishUpload = bFinish;

		Json::FastWriter writer;
		Json::Value req;
		req["hash"] = Hash2Char(m_Para.hash);
		req["data"] =root;
		std::string strOut = writer.write(req);

		CallBackToUI(8, !bFinish, strOut.c_str(), strOut.length());	
		SetStatus(CTask::PAUSE);		// 暂停该任务
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CNFUploadTask::WriteHttpData ret=%d info=[%s]", bFinish, strOut.c_str());
#endif
		return ret;
	}
	else
	{
		m_uHasUploadSize = m_filesize - m_uchunksize*m_vNeedChunkdx.size();
		m_uHasUploadSize = m_uHasUploadSize >0? m_uHasUploadSize: 0;
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CNFUploadTask::WriteHttpData uploadsize=%lld m_uchunksize=%d needcount=%d", m_uHasUploadSize, m_uchunksize, m_vNeedChunkdx.size());
#endif
		//UpdateDownloadMode();		// 上传下一片
	}
	
	return ret;
}

void CNFUploadTask::UpdateDownloadMode()
{
	if (RUNNING != m_Para.status)
	{
		return;
	}
	DownHandle();
	//HttpPostReq req;
	//if (DownNextHttpPiece(req) >= 0)
	//{
	//	CHttpUploadInterface::Instance()->AddTask((HASH&)m_Para.hash, req);
	//}
}

int CNFUploadTask::SetHeadValue(const char* pstrHead, const char* pstrValue)
{
	if (NULL == pstrHead || 0==strlen(pstrHead) || NULL == pstrValue)
	{
		return -1;
	}
	string strHead = pstrHead;
	string strValue = pstrValue;
	m_mHeadVals[strHead] = strValue;
	m_sign.AddHead(strHead, strValue);
    return 0;
}

int CNFUploadTask::SetHeadValue(const char *pstrHeadAdd)
{
	m_sign.AddHead(pstrHeadAdd);
	return 0;
}

QVOD_INT64 CNFUploadTask::GetFileSize()
{
	return m_filesize;
}

QVOD_INT64 CNFUploadTask::GetTotalUploadLen()
{
	return m_uHasUploadSize;
}

string CNFUploadTask::GetHeadAddString()
{
	string strRet;
	for (map<string, string>::iterator itr=m_mHeadVals.begin(); itr!= m_mHeadVals.end(); itr++)
	{
		strRet += itr->first;
		strRet += ": ";
		strRet += itr->second;
		strRet += "\r\n";
	}
	return strRet;
}

bool CNFUploadTask::TaskIsFinish(bool bCheckFileExist)
{
	return m_bFinishUpload;
}

int CNFUploadTask::DownNextHttpPiece(HttpPostReq& req)
{
	switch (m_iDownloadMode)
	{
	case MODE_CDN:
		{
			CAutoLock lock(m_downNextHttpPieceLock);

			if (0 == m_uchunksize || 0 == m_vNeedChunkdx.size())		// 检测文件是否上传完成
			{
				req.strOrlurl = g_cfg.strUploadInitUrl;
				m_sign.GenerateSign(req.strOrlurl);
				req.strHeadAdd = m_sign.GetHeadAddString();
				req.id = (unsigned int)-1;
				return 0;
			}

			req.strOrlurl = m_Para.strHttpOrgUrl;
			unsigned int uCurChunkIdx = *m_vNeedChunkdx.begin();
			int i =0;
			const int nSize = m_vNeedChunkdx.size();
			set<unsigned int>::iterator itr=m_vNeedChunkdx.begin();
			for (; itr != m_vNeedChunkdx.end(); itr++)				// 查找哪些片没有在上传队列中
			{
				if (!IsTsIndexUploading(*itr))
				{
					uCurChunkIdx = *itr;
					break;
				}
			}

			if ( itr == m_vNeedChunkdx.end())
			{
				return -1;
			}

			QVOD_INT64 start = uCurChunkIdx * m_uchunksize;

			FILE *fp = fopen(m_strFilePath.c_str(), "rb");
			if (fp)
			{
				QPtr::AutoArrayPtr<char> buf = new char[m_uchunksize];
				fseek(fp, start, SEEK_SET);
				long nReadLen = fread(buf, 1, m_uchunksize, fp);
				fclose(fp);
				if (m_uchunksize != nReadLen)	// 判断是否为最后一块数据
				{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CNFUploadTask readfile len=%d", nReadLen);
#endif
				}

				CSHA1 sha( buf, nReadLen);
				string strHash = sha.readHex();
				m_sign.AddHead("X-YF-chunkhash", strHash);
				req.strSendData.append(buf, nReadLen);
			}
			else
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CNFUploadTask fopen failed!", errno);
#endif			
				CallBackToUI(8, 2, Hash2Char(m_Para.hash).c_str(),40);
				return -1;
			}
			
			m_sign.AddHead("X-YF-chunksize", req.strSendData.length());
			m_sign.AddHead("X-YF-chunkindex", uCurChunkIdx);

			m_sign.GenerateSign(req.strOrlurl, req.strSendData);
			req.strHeadAdd = m_sign.GetHeadAddString();

			req.id = uCurChunkIdx;
			return uCurChunkIdx;
		}

		break;
	}
	return -1;
}

void CNFUploadTask::DownHandle()
{
	int ret = 1;
	//int ret = DownNextHttpPiece(req);
	//if ( ret >= 0)
	//{
	//	bool bRet = CHttpUploadInterface::Instance()->AddTask((HASH&)m_Para.hash, req);
	//	if (bRet)
	//	{
	//		SetTsIndexUploading(ret);
	//	}
	//}

	bool bRet = true;

	while (bRet && ret>0 && m_tsIndexUploading.size() < g_cfg.m_uConcurrentNum)
	{
		HttpPostReq req;
		if ((ret=DownNextHttpPiece(req)) < 0)
		{
			break;
		}
		if (IsTsIndexUploading(ret))		// 已在上传序列中
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CM3u8Task::UpdateDownloadMode IsTsIndexDownloading idx=%d", ret);
#endif
			break;
		}

		unsigned char szhash[20] = {0};
		//CServerProduct::CreateTaskHash(req.strOrlurl, szhash); // 由于req.strOrlurl每次都一样的，所以应以strhead来生成hash
		CServerProduct::CreateTaskHash(req.strHeadAdd, szhash);
		HASH hash(szhash);
		req.strHash.append((char*)m_Para.hash, 20);		// 需要以ts片的url创建hash放到下载逻辑中
		bRet = CHttpUploadInterface::Instance()->AddTask(hash, req) > 0;
		if (bRet)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CNFUploadTask map hash=%s tshash=%s index=%d", Hash2Char(m_Para.hash).c_str(), Hash2Char(hash.data).c_str(), ret);
#endif
			SetTsIndexUploading(ret);
		}
	}
}

void CNFUploadTask::SetTsIndexUploading(const unsigned int &index)
{
	if (index >= 0)
	{
		CAutoLock lock(m_tsIndexLock);
		m_tsIndexUploading.insert(index);
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8Task::SetTsIndexDownloading index=%d count=%d", index, m_tsIndexUploading.size());
#endif
	}
}
void CNFUploadTask::ReSetTsIndexUploading(const unsigned int &index)
{
	CAutoLock lock(m_tsIndexLock);
	m_tsIndexUploading.erase(index);
	if (-1 == index)
	{
		m_tsIndexUploading.clear();
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8Task::ReSetTsIndexDownloading index=%d count=%d", index, m_tsIndexUploading.size());
#endif
}

bool CNFUploadTask::IsTsIndexUploading(const unsigned int& index)
{
	CAutoLock lock(m_tsIndexLock);
	return m_tsIndexUploading.find(index) != m_tsIndexUploading.end() ;
}

void CNFUploadTask::NotifyDownload(const int &nId)
{
	int iTsIndex = nId;
	ReSetTsIndexUploading(iTsIndex);
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CNFUploadTask::NotifyDownM3u8Ts index=%d ", iTsIndex);
#endif
}

void CNFUploadTask::Pause()
{
	m_vHasChunkdx.clear();
	m_vNeedChunkdx.clear();
	m_tsIndexUploading.clear();
	CTask::Pause();
}