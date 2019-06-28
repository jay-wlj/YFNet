// InitApp.h: interface for the CInitApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITAPP_H__4D694D17_7C2A_4C3C_9DF3_25563C32BA5F__INCLUDED_)
#define AFX_INITAPP_H__4D694D17_7C2A_4C3C_9DF3_25563C32BA5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32 /* WIN32 */
#pragma warning(disable:4786)
#pragma warning(disable:4305)
#pragma warning(disable:4309)
#else /* posix */
//nothing
#endif /* posix end */

#include "global.h"
#include "terminal.h"

#include "qvod_string.h"
#include "qvod_file.h"
#include "stund.h"

#include "common-inifile.h"
#include "common-utility.h"
#include "common-DNSCache.h"
#include "common-SpeedCounter.h"
#include "common-commonfun.h"
#include "common-commonstruct.h"

struct LoadTaskPara
{
	int iBeginPos;
	int iEndPos;
	void *pVecTask;
	void *pIniApp;
};

class CInitApp  
{
public:
	CInitApp();
	virtual ~CInitApp();

	int			Init();
	void		Close();
	
	bool		DoListenAction();
    
    bool		TrySetNetworkStatus(bool bStatus);
	inline bool IsNonBlockInitFinish(){return m_bNonBlockInitFinish;}
	
private:
	bool		InitRunDirectory( );										//初始运行目录
	bool		InitGlobalInfo( );											//初始化全局信息
	bool		LoadNewTaskConfig();
	void		ReadConfig(const std::string& strConfigPath);				//读取配置文件
	bool		LoadTask();													//加载任务
	int		LoadTask(int iBegin,int iEnd,vector<TaskCfg> *pVecTask);
	static		QVOD_THREAD_RETURN NonBlockInit(void* pPara);
private:
	bool		m_bCloseNetwork;
	bool		m_bNonBlockInitFinish;
	CLock		m_lock;
};

#endif // !defined(AFX_INITAPP_H__4D694D17_7C2A_4C3C_9DF3_25563C32BA5F__INCLUDED_)

