// ServerProduct.h: interface for the CServerProduct class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QVODSERVER_H__DE9A1D66_6BC1_4DED_B6A4_BB66DBEF9CC9__INCLUDED_)
#define AFX_QVODSERVER_H__DE9A1D66_6BC1_4DED_B6A4_BB66DBEF9CC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "qvod_thread.h"
#include "qvod_time.h"
#include "common-commonstruct.h"
#include "terminal.h"

class CServerProduct  
{
public:
	static int CreateYfTask(const HASH& hash,std::string& strFileName,int type,string& strHttpOrgUrl,string& strHttpKeyUrl,string& strHttpReferer);
	
	static int CreateTaskHash(string &strKeyUrl, unsigned char *data);

	static int LoadYfTask(HASH& hash,const std::string& strFileName,TaskCfg* cfg);
	
	static int SetTaskStatus(HASH& hash,int iStatus,bool bIsAll=false,bool bIsLoad = false);
	static int DeleteOldTaskExceptHash(const HASH& hash,QVOD_INT64* pReleaseSpace=NULL);
	static int ClearSpaceExceptHash(const HASH& hash);		// 清除缓存，删除老旧任务等
};

#endif // !defined(AFX_QVODSERVER_H__DE9A1D66_6BC1_4DED_B6A4_BB66DBEF9CC9__INCLUDED_)

