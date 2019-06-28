// SpeedCtrl.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "ShareData.h"
#include "../include/json/json.h"
#include "../include/function.h"


#define SHARE_DATA_MAPFILE  _T( "Local\\MangoWeb_ShareData")
#define GLOBAL_DATA_EVENT	_T("Global\\MangoWeb_ShareData_Event")


using namespace sharedata;
CShareData& CShareData::Instance()
{
	static CShareData s_ShareData;
	return s_ShareData;
}

CShareData::CShareData(void)
{
	m_hMutex = NULL;
	m_hMapFile  = NULL;
}

CShareData::~CShareData(void)
{
	SafeDelHandle(m_hMutex);
	SafeDelHandle(m_hMapFile);
}

BOOL CShareData::Init() 
{
	m_hMutex = CreateMutex(NULL, FALSE, GLOBAL_DATA_EVENT);		//全局事件对象

	//默认10K的共享内存
	m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,  0x10240, SHARE_DATA_MAPFILE);
	if (NULL == m_hMapFile || INVALID_HANDLE_VALUE == m_hMapFile)
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	return TRUE;
}

void CShareData::UnInit()
{
	SafeDelHandle(m_hMutex);
	SafeDelHandle(m_hMapFile);
}

void CShareData::LockData()							//锁数据
{
	WaitForSingleObject(m_hMutex, INFINITE);
}

void CShareData::UnLockData()						//解锁
{
	ReleaseMutex(m_hMutex);
}

BOOL CShareData::GetShareData(std::string &strInfo)
{
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARE_DATA_MAPFILE);
	if (NULL == hMapFile || INVALID_HANDLE_VALUE == hMapFile)
	{
		return FALSE;
	}

	LPBYTE pVoid = (LPBYTE)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	LockData();
	
	DWORD dwSize = *(DWORD*)pVoid;			//数据缓冲大小
	if (dwSize > 0)
	{
		LPBYTE pData = (pVoid + sizeof(DWORD));

		strInfo.assign((char*)pData, dwSize);
	}
	
	UnLockData();

	UnmapViewOfFile(pVoid);			//卸载映像
	CloseHandle(hMapFile);				//关闭映射文件句柄

	return TRUE;
}


BOOL CShareData::SetShareData(const std::string &strInfo)
{
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARE_DATA_MAPFILE);
	if (NULL == hMapFile || INVALID_HANDLE_VALUE == hMapFile)
	{
		return FALSE;
	}

	LPBYTE pVoid = (LPBYTE)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	
	LockData();

	*(DWORD*)pVoid = strInfo.size();		//数据缓冲区大小
	LPBYTE pData = (pVoid + sizeof(DWORD));
	memcpy((void*)pData, (void*)strInfo.c_str(), strInfo.length());

	UnLockData();

	UnmapViewOfFile(pVoid);			//卸载映像
	CloseHandle(hMapFile);				//关闭映射文件句柄
	return TRUE;
}

BOOL CShareData::AddShareData(const std::string& strKey, const std::string& strVal)
{
	std::string strRet;
	sharedata::CShareData::Instance().GetShareData(strRet);
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(strRet, root))
	{
	}
	root[strKey] = strVal;
	
	Json::FastWriter writer;
	strRet = writer.write(root);
	BOOL bRet = sharedata::CShareData::Instance().SetShareData(strRet);
	return bRet;
}