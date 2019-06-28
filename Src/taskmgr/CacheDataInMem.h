#pragma once
#include "common-commonstruct.h"
#include "qvod_lock.h"
#include "global.h"
#include <map>
#include <vector>
using std::map;
using std::vector;

typedef struct _CACHE_DATA_INFO
{
	char *pBuf;
	QVOD_INT64 start;
	QVOD_INT64 len;

	_CACHE_DATA_INFO()
	{
		memset(this, 0x0, sizeof(_CACHE_DATA_INFO));
		pBuf = NULL;
	}
	~_CACHE_DATA_INFO()
	{
		SafeDeleteArray(pBuf);
	}
}CacheDataInfo, *PCacheDataInfo;

class CCacheDataInMem
{
public:
	CCacheDataInMem(void);
	~CCacheDataInMem(void);

	static CCacheDataInMem& Instance();

	bool CacheM3u8Data(const HASH& hash, const string& strTsName, const char* pBuf, int pos, int len, int filesize);
	int ReadM3u8Data(const HASH& hash, const string& strTsName, char* pBuf, int pos, int readlen);
	bool CheckM3u8Ts(const HASH& hash, const string& strTsName);
	QVOD_INT64 GetM3u8TsSize(const HASH& hash, const string& strTsName);

	int CacheData(const HASH& hash, const char* pBuf, int pos, int len);
	int ReadData(const HASH& hash, char* pBuf, int pos, int readlen);

	void ClearCacheByHash(const HASH& hash, const string& strTsName);
	void ClearCacheByHash(const HASH& hash);
	void ClearCache();


	bool IsOutofMem();		// 数据是否超过设定的内存大小
private:
	void AddMemSize(int len);
private:
	CLock m_DataLock;
	map<HASH, vector<PCacheDataInfo> > m_mCacheData;
	map<HASH, map<string, PCacheDataInfo> > m_m3u8CacheData;
	int	m_nCacheSize;
};

