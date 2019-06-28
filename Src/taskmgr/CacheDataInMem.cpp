#include "CacheDataInMem.h"
#include "common-commonfun.h"
#include "common-utility.h"

static const int MAX_CACHESIZE = 15*1024*1024;		// 缓存大小默认15M
CCacheDataInMem::CCacheDataInMem(void)
{
	m_nCacheSize = 0;
}


CCacheDataInMem& CCacheDataInMem::Instance()
{
	static CCacheDataInMem s_Instance;
	return s_Instance;
}

CCacheDataInMem::~CCacheDataInMem(void)
{
	ClearCache();
}

void CCacheDataInMem::ClearCacheByHash(const HASH& hash, const string& strTsName)
{
	CAutoLock lock(m_DataLock);

	// 删除m3u8ts内存数据
	map<HASH, map<string, PCacheDataInfo> >::iterator itr2=m_m3u8CacheData.find(hash);
	if ( itr2 != m_m3u8CacheData.end())
	{
		map<string, PCacheDataInfo>& mCacheData = (itr2->second);
		for (map<string, PCacheDataInfo>::iterator itTs=mCacheData.begin(); itTs!=mCacheData.end(); itTs++)
		{
			if (itTs->second)
			{
				AddMemSize(-itTs->second->len);
			}
			PCacheDataInfo &pInfo = (itTs->second);
			SafeDelete(pInfo);
		}
		mCacheData.clear();
		m_m3u8CacheData.erase(itr2);
	}
}
void CCacheDataInMem::ClearCacheByHash(const HASH& hash)
{
	CAutoLock lock(m_DataLock);
	map<HASH, vector<PCacheDataInfo> >::iterator itr=m_mCacheData.find(hash);
	if ( itr != m_mCacheData.end() )
	{
		vector<PCacheDataInfo>& vCacheData = (itr->second);
		const int nSize = vCacheData.size();
		for (int i=0; i<nSize; i++)
		{
			if (vCacheData[i])
			{
				AddMemSize(-vCacheData[i]->len);
			}
			SafeDelete(vCacheData[i]);
		}
		vCacheData.clear();
		m_mCacheData.erase(itr);
	}


	// 删除m3u8ts内存数据
	try {
		map<HASH, map<string, PCacheDataInfo> >::iterator itr2=m_m3u8CacheData.find(hash);
		if ( itr2 != m_m3u8CacheData.end())
		{
			map<string, PCacheDataInfo>& mCacheData = (itr2->second);
			for (map<string, PCacheDataInfo>::iterator itTs=mCacheData.begin(); itTs!=mCacheData.end(); itTs++)
			{
				if (itTs->second)
				{
					AddMemSize(-itTs->second->len);
				}
				PCacheDataInfo &pInfo = (itTs->second);
				SafeDelete(pInfo);
			}
			mCacheData.clear();
			m_m3u8CacheData.erase(itr2);
		}
	} catch (...) {
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "catch CCacheDataInMem ClearCacheByHash hash=%s\n", Hash2Char(hash.data).c_str());
#endif
	}
}

void CCacheDataInMem::ClearCache()
{
	CAutoLock lock(m_DataLock);
	for (map<HASH, vector<PCacheDataInfo> >::iterator itr=m_mCacheData.begin(); itr!=m_mCacheData.end(); itr++)
	{
		vector<PCacheDataInfo>& vCacheData = (itr->second);
		const int nSize = vCacheData.size();
		for (int i=0; i<nSize; i++)
		{
			if (vCacheData[i])
			{
				AddMemSize(-vCacheData[i]->len);
			}
			SafeDelete(vCacheData[i]);
		}
		vCacheData.clear();
	}
	m_mCacheData.clear();

	try {
		// 删除m3u8ts内存数据
		for (map<HASH, map<string, PCacheDataInfo> >::iterator itr=m_m3u8CacheData.begin(); itr!=m_m3u8CacheData.end(); itr++)
		{
			map<string, PCacheDataInfo>& mCacheData = (itr->second);
			for (map<string, PCacheDataInfo>::iterator itTs=mCacheData.begin(); itTs!=mCacheData.end(); itTs++)
			{
				PCacheDataInfo &pInfo = (itTs->second);
				SafeDelete(pInfo);
			}
			mCacheData.clear();
		}
	} catch (...) {
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "catch CCacheDataInMem ClearCache()\n");
#endif
	}
	m_m3u8CacheData.clear();
	m_nCacheSize = 0;
}

int CCacheDataInMem::CacheData(const HASH& hash, const char* pBuf, int pos, int len)
{
	int ret = 0;

	CAutoLock lock(m_DataLock);
	if (m_nCacheSize >= MAX_CACHESIZE)		// 超过所设缓存大小
	{
		// ClearCache();
	}

	map<HASH, vector<PCacheDataInfo> >::iterator itr = m_mCacheData.find(hash);
	if (itr != m_mCacheData.end())
	{
		PCacheDataInfo pInfo = new CacheDataInfo;
		pInfo->pBuf = (char*)pBuf;
		pInfo->start = pos;
		pInfo->len = len;

		AddMemSize(len);
		((itr->second)).push_back(pInfo);
	}
	return ret;
}

int CCacheDataInMem::ReadData(const HASH& hash, char* pBuf, int pos, int readlen)
{
	int ret = 0;
	
	CAutoLock lock(m_DataLock);
	map<HASH, vector<PCacheDataInfo> >::iterator itr = m_mCacheData.find(hash);
	if (itr != m_mCacheData.end())
	{
		vector<PCacheDataInfo>& vCacheData = (itr->second);
		const int nSize = vCacheData.size();
		for (int i=0; i<nSize; i++)
		{
			if (vCacheData[i])
			{
				if (pos >= vCacheData[i]->start && pos < (vCacheData[i]->start+vCacheData[i]->len))
				{
					int minlen = QVOD_MIN2(readlen, (vCacheData[i]->len-pos));
					memcpy(pBuf, vCacheData[i]->pBuf+pos-vCacheData[i]->start, minlen);
					ret = minlen;
					break;
				}
			}
		}
	}
	return ret;
}

bool CCacheDataInMem::CacheM3u8Data(const HASH& hash, const string& strTsName, const char* pBuf, int pos, int len, int filesize)
{
	if ( len != filesize && 0 == pos)
	{
		//完整ts才写入文件
		return false;
	}

	CAutoLock lock(m_DataLock);
	if (m_nCacheSize >= MAX_CACHESIZE)		// 超过所设缓存大小
	{
		return false;
	}

	PCacheDataInfo pInfo = new CacheDataInfo;
	pInfo->pBuf = (char*)pBuf;
	pInfo->start = pos;
	pInfo->len = len;

	AddMemSize(len);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CCacheDataInMem CacheM3u8Data hash=[%s] strTsName=%s len=%d m_nCacheSize=%d\n", Hash2Char(hash.data).c_str(), strTsName.c_str(), len, m_nCacheSize);
#endif

	map<HASH, map<string, PCacheDataInfo> >::iterator itr = m_m3u8CacheData.find(hash);
	if (itr != m_m3u8CacheData.end())
	{
		map<string, PCacheDataInfo> &mCacheData = (itr->second);
		map<string, PCacheDataInfo>::iterator itTs = mCacheData.find(strTsName);
		if (itTs != mCacheData.end())
		{
			PCacheDataInfo pData = itTs->second;
			if (pData)
			{
				AddMemSize(-pData->len);
			}
			mCacheData.erase(itTs);
			SafeDelete(pData);
		}
		mCacheData.insert(make_pair(strTsName, pInfo));
	}
	else
	{
		map<string, PCacheDataInfo> mCacheData;
		mCacheData.insert(make_pair(strTsName, pInfo));
		m_m3u8CacheData.insert(make_pair(hash, mCacheData));
	}
	return true;
}
int CCacheDataInMem::ReadM3u8Data(const HASH& hash, const string& strTsName, char* pBuf, int pos, int readlen)
{
	int ret = 0;
	if (readlen <= 0 || !pBuf)
	{
		return ret;
	}
	CAutoLock lock(m_DataLock);
	map<HASH, map<string, PCacheDataInfo> >::iterator itr = m_m3u8CacheData.find(hash);
	if (itr != m_m3u8CacheData.end())
	{
		map<string, PCacheDataInfo>& mCacheData = (itr->second);
		map<string, PCacheDataInfo>::iterator itTs = mCacheData.find(strTsName);		// 查找当指定ts片
		if (itTs != mCacheData.end())
		{
			PCacheDataInfo &pInfo = (itTs->second);
	
			if (pInfo)
			{
				if (pos >= pInfo->start && pos < (pInfo->start+pInfo->len))							// 拷贝数据
				{
					//int minlen = QVOD_MIN2(readlen, (pInfo->len-pos));
					size_t minlen = QVOD_MIN2(readlen,  pInfo->start+pInfo->len-pos);
					try {
						if (minlen > 0)
						{
							memcpy(pBuf, pInfo->pBuf+pos-pInfo->start, minlen);
						}
					}
					catch (...) {
#ifdef QVODDEBUG
					_OUTPUT(ERROR1,"catch CCacheDataInMem memcpy pInfo->pBuf=%0x pInfo->len=%d pos=%d minlen=%d\n", pInfo->pBuf, pInfo->len, pos, minlen);
#endif
					}
					ret = minlen;
				}
				if ((pos+ret) >= (pInfo->start+pInfo->len))		// 该片ts拷贝完成
				{
					AddMemSize(-pInfo->len);

					try {
						SafeDelete(pInfo);
					}
					catch (...) {
#ifdef QVODDEBUG
					_OUTPUT(ERROR1,"catch CCacheDataInMem ReadM3u8Data tsname=%s\n", strTsName.c_str());
#endif
					}

					mCacheData.erase(itTs);
					
#ifdef QVODDEBUG
					_OUTPUT(INFO,"CCacheDataInMem ReadM3u8Data finished! release:[%s]\n", strTsName.c_str());
#endif
				}
			}
		}
	}
	return ret;
}

bool CCacheDataInMem::CheckM3u8Ts(const HASH& hash, const string& strTsName)
{
	bool ret = false;
	
	CAutoLock lock(m_DataLock);
	map<HASH, map<string, PCacheDataInfo> >::iterator itr = m_m3u8CacheData.find(hash);
	if (itr != m_m3u8CacheData.end())
	{
		map<string, PCacheDataInfo>& mCacheData = (itr->second);
		map<string, PCacheDataInfo>::iterator itTs = mCacheData.find(strTsName);		// 查找当指定ts片
		if (itTs != mCacheData.end())
		{
			ret = true;
		}
	}
	return ret;
}

QVOD_INT64 CCacheDataInMem::GetM3u8TsSize(const HASH& hash, const string& strTsName)
{
	QVOD_INT64 ret = 0;

	CAutoLock lock(m_DataLock);
	map<HASH, map<string, PCacheDataInfo> >::iterator itr = m_m3u8CacheData.find(hash);
	if (itr != m_m3u8CacheData.end())
	{
		map<string, PCacheDataInfo>& mCacheData = (itr->second);
		map<string, PCacheDataInfo>::iterator itTs = mCacheData.find(strTsName);		// 查找当指定ts片
		if (itTs != mCacheData.end())
		{
			if (itTs->second)
			{
				ret = itTs->second->len;
			}
		}
	}
	return ret;
}

void CCacheDataInMem::AddMemSize(int len)
{
	m_nCacheSize += len;
}

bool CCacheDataInMem::IsOutofMem()
{
	return (m_nCacheSize >= MAX_CACHESIZE);
}
