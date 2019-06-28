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
	void LockData();							//������
	void UnLockData();						//����
private:
	HANDLE m_hMutex;						//ȫ�������
	HANDLE m_hMapFile;					//�����ڴ���
};


}