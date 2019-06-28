#pragma once
#include "ShareData.h"

namespace sharedata
{
class CShareData 
{
public:
	static CShareData& Instance();

	BOOL Init() ;
	void UnInit();

public:
	BOOL GetShareData(std::string &strInfo);
	BOOL SetShareData(const std::string &strInfo);
	BOOL AddShareData(const std::string& strKey, const std::string& strVal);
private:
	CShareData(void);
	~CShareData(void);
private:
	void LockData();							//锁数据
	void UnLockData();						//解锁
private:
	HANDLE m_hMutex;						//全局锁句柄
	HANDLE m_hMapFile;					//共享内存句柄
};


}