// InitApp.cpp: implementation of the CInitApp class.
//
#include "InitApp.h"


#include "ServerProduct.h"
#include "TaskMgrInterface.h"
#include "HttpAgentInterface.h"
#include "HttpUploadInterface.h"
#include "MsgPoolInterface.h"
#include "CopyTaskFile.h"
#include "httpserver.h"
#include "CloudCfg.h"
#include "upnp.h"
// #include "dispatch-DispatchMgrInterface.h"
// #include "trackerinterface.h"
// #include "msgpool-MsgPoolInterface.h"
// #include "LanSearch.h"
// #include "MinerTracker.h"
#include "DbTaskConfig.h"

#ifdef SUPPORT_HTTPS
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif

// #ifdef MONGOTV
// #include "CopyTaskFile.h"
// #include "Statis.h"
// #endif
// 
// #include <dlfcn.h>
// #include "QvodJsonStatistic.h"

// extern char g_peerID[24];

 CHttpServerMgr g_httpServer;

CInitApp::CInitApp()
{
	m_bCloseNetwork = false;
	m_bNonBlockInitFinish = false;
}

CInitApp::~CInitApp()
{

}

int CInitApp::Init()
{
	g_bRunning = true;	
	m_bCloseNetwork = false;
	m_bNonBlockInitFinish = false;

#ifdef QVODDEBUG
    _OUTPUT(INFO,"CInitApp::Init()!\n");
#endif // QVODDEBUG
    
	//初始化全局信息
	InitGlobalInfo();

#ifdef QVODDEBUG
	_OUTPUT(INFO,"____Qvod's Info Init is Successed!\n");
#endif // QVODDEBUG	

	
//	CCloudCfg::Instance()->Init();
//#ifdef QVODDEBUG
//	_OUTPUT(INFO,"CCloudCfg::Instance() Init!\n");
//#endif // _DEBUG

	CHttpAgentInterface::Instance();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CHttpAgentInterface::Instance()!\n");
#endif // _DEBUG
	CHttpUploadInterface::Instance();

	CMsgPoolInterface::Instance();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CMsgPoolInterface::Instance()!\n");
#endif // _DEBUG

	// 解析后面用到的一些域名
	CDNSCache::Instance()->Init();

	bool bRet = LoadTask();

	if(!bRet)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"Load Task Failed!\n");
#endif // QVODDEBUG
		//return -1;
	}

#ifdef QVODDEBUG
    _OUTPUT(INFO,"LoadTask finish!\n");
#endif // _DEBUG
    
	g_httpServer.init(g_httpport);

	if(g_httpServer.run() != 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"_____Init CHttpServerMgr is Fail!\n");
#endif // QVODDEBUG
		return -2;
	}

#ifdef QVODDEBUG
    _OUTPUT(INFO,"g_httpServer finish!\n");
#endif // _DEBUG
	CHttpAgentInterface::Instance()->Run();

	//单独启动Common线程,加快5秒加速的响应速度
	CMsgPoolInterface::Instance()->Run(true);

	//CCopyTaskFile::Instance()->Run();

	CHttpUploadInterface::Instance()->Run();
#ifdef QVODDEBUG
    _OUTPUT(INFO, "CInitApp::Init() finish!");
#endif

#ifdef QVOD_P2P
	QvodCreateThread(NULL,NonBlockInit,(void*)this);
#endif

	return 1;
}

void CInitApp::Close()
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "into CInitApp::Close\n");
#endif // QVODDEBUG

	g_bRunning = false;
	g_bNetworkRun = false;	

	CHttpAgentInterface::Instance()->Stop();
	CHttpUploadInterface::Instance()->Stop();

	//CCopyTaskFile::Instance()->Stop();
	g_httpServer.stop();

#ifdef QVODDEBUG
	_OUTPUT(INFO,"CHttpAgentInterface::Instance()->Stop()!\n");
#endif // _DEBUG

	CMsgPoolInterface::Instance()->Stop();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CMsgPoolInterface::Instance()->Stop()!\n");
#endif // _DEBUG

	CDbTaskConfig::Instance()->CloseDataBase();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CDbTaskConfig::Instance()->CloseDataBase()!\n");
#endif // QVODDEBUG
	CHttpAgentInterface::FreeInstance();

#ifdef QVODDEBUG
	_OUTPUT(INFO,"CHttpAgentInterface::FreeInstance()!\n");
#endif // _DEBUG

// 上传
	CHttpUploadInterface::FreeInstance();

	CMsgPoolInterface::FreeInstance();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CMsgPoolInterface::FreeInstance()!\n");
#endif // _DEBUG

	CTaskMgrInterFace::FreeInstance();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CTaskMgrInterFace::FreeInstance()!\n");
#endif // QVODDEBUG

	CDbTaskConfig::FreeInstance();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CDbTaskConfig::FreeInstance()!\n");
#endif // QVODDEBUG

	CDNSCache::FreeInstance();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CDNSCache::FreeInstance()!\n");
#endif // QVODDEBUG

	CSpeedCounter::FreeInstance();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CSpeedCounter::FreeInstance()!\n");
#endif // QVODDEBUG
//	CCloudCfg::Instance()->UnInit();
//	CCloudCfg::Instance()->FreeInstance();
//#ifdef QVODDEBUG
//	_OUTPUT(INFO,"CCloudCfg::FreeInstance() UnInit!\n");
//#endif // _DEBUG

#if QVOD_DEBUG_SCREEN
	SafeDelete(pConsole);
#endif

	QVOD_SLEEP(200);
}

bool CInitApp::LoadTask()
{
	string strDBPath = g_strCurrentRoot + "TaskCfg.info";
	if(!CDbTaskConfig::Instance()->OpenDataBase(strDBPath))
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"Open DataBase Failed!\n");
#endif // QVODDEBUG
		return false;
	}
    bool bRet = LoadNewTaskConfig();
	return bRet;
}

bool CInitApp::InitGlobalInfo()
{
	g_protocol = 3;
	g_listenPort = randomPort();
	g_httpport = randomPort();
	g_isForceUdp = false;
	g_iUpRateLimit = 0;
	g_iDownRateLimit = 0;
	g_task_PeersLimit = 0; 
	g_task_SeedsLimit = 0;
	g_task_UpRateLimit = 0;

	g_uHostIP = GetLocalIP();

	if (0 == g_uHostIP)
	{
		g_uHostIP = inet_addr("127.0.0.1");
	}
    
	InitQvod();

	if(!InitRunDirectory())
	{
		return false;
	}

	CalcSoftVersion();

	if (0 == g_uHostIP) 
	{
		g_uHostIP = inet_addr("127.0.0.1");
	}
#ifdef QVOD_P2P
	if(g_uHostIP != 16777343 && g_uHostIP != 0 && 17303818 != g_uHostIP)
	{
		CUpnp::Instance()->Init(g_iLocalIP,(char *)QVODPLAYER_NAME);
	}
#endif
	return true;
}

bool CInitApp::InitRunDirectory()
{
// 	char   path[QVOD_MAX_PATH];
// 	QvodGetModuleFileName(NULL,path,QVOD_MAX_PATH);
	//g_strCurrentRoot = path;

	//if(*(g_strCurrentRoot.c_str() + g_strCurrentRoot.length() - 1) != '/') {
	//	g_strCurrentRoot += "/";
	//}

#ifdef QVODDEBUG
	_OUTPUT(INFO,"currentroot = %s\n",g_strCurrentRoot.c_str());
#endif

	g_strConfigRoot = g_strCurrentRoot;
	QvodMkdir(g_strConfigRoot.c_str());

	g_strtorrentpath = g_strCurrentRoot + "Data/";

#ifdef QVODDEBUG
	_OUTPUT(INFO, "g_strtorrentpath: %s\n", g_strtorrentpath.c_str());
#endif // QVODDEBUG

	QvodMkdir(g_strtorrentpath.c_str());

	std::string strConfigPath = g_strConfigRoot + "YfNetCfg.ini";

#ifdef QVODDEBUG
	_OUTPUT(INFO, "strConfigPath: %s\n", strConfigPath.c_str());
#endif // QVODDEBUG

	ReadConfig(strConfigPath);

#if QVOD_DEBUG_SCREEN
	if(g_outdebuginfo > 0)
	{
		pConsole = new ZConsole;
	}
#endif

#ifdef QVOD_DEBUG_FILE
	if (g_strLogPath.empty())	// 默认日志路径
	{
		g_strLogPath = g_strConfigRoot + "log/";
//		QvodRemoveFolder(g_strLogPath.c_str());
//        if (debug_file)
//        {
//            fclose(debug_file);
//        }
	}
	QvodMkdir(g_strLogPath.c_str());
//	char curTime[255] = {0};
//	GetCurTime((char*)curTime,1);
//	strLogPath += curTime;
//	strLogPath += ".txt";
//	if(g_outdebuginfo > 1 && NULL == debug_file)
//	{
//		debug_file=fopen(strLogPath.c_str(),"w+");
//	}
#endif
	return true;
}

bool CInitApp::LoadNewTaskConfig()
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "LoadNewTaskConfig start");
#endif
	std::vector<TaskCfg> vecTaskCfg;
	std::vector<TaskCfg>::iterator it; /* silver add */
	if(CDbTaskConfig::Instance()->GetAllTask(vecTaskCfg) != 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CDbTaskConfig::Instance()->GetAllTask error\n");
#endif // QVODDEBUG
		return false;
	}
	for(it = vecTaskCfg.begin(); it < vecTaskCfg.end(); it++)
	{
#ifdef QVODDEBUG
		_OUTPUT(LOG_DEBUG, "------------------------\n");
		_OUTPUT(LOG_DEBUG, "taskCfg->filepath = %s\n", (it->filepath).c_str());
		_OUTPUT(LOG_DEBUG, "taskCfg->type = %d\n", it->type);
		_OUTPUT(LOG_DEBUG, "taskCfg->status = %d\n", it->status);
		_OUTPUT(LOG_DEBUG, "taskCfg->bCopyFull= %d\n", it->bCopyFullFile);
		_OUTPUT(LOG_DEBUG, "taskCfg->CopyPath= %s\n", (it->strCopyPath).c_str());
		_OUTPUT(LOG_DEBUG, "taskCfg->strKey= %s\n", (it->strKey).c_str());
#endif // QVODDEBUG

		if(it->status == CTask::RUNNING)
		{
			it->status = CTask::PAUSE;
#ifdef QVODDEBUG
			_OUTPUT(LOG_DEBUG, "set status to CTask::PAUSE\n");
#endif // QVODDEBUG
		}
	} /* for */
	size_t iFileNum = vecTaskCfg.size();
	int nLoadTaskSuccess = LoadTask(0,iFileNum,&vecTaskCfg);
#ifdef QVODDEBUG
	_OUTPUT(INFO, "LoadNewTaskConfig end, nLoadTaskSuccess=%d", nLoadTaskSuccess);
#endif
	return true;
}

int CInitApp::LoadTask(int iBegin,int iEnd,vector<TaskCfg> *pVecTask)
{
	int nLoadTaskSuccess = 0;
	for ( int i = iBegin;i < iEnd;i++ )
	{
		if ( !g_bRunning )
		{
			return nLoadTaskSuccess;
		}
		TaskCfg &cfg = (*pVecTask)[i];
		HASH hash;		
		memcpy(hash.data,cfg.hash,20);
		cfg.status = CTask::PAUSE;
		int iTaskRet = 0;
		string strFilepath = g_media_dir + cfg.filepath;
#ifndef MACOSX
		if (cfg.bCopyFullFile)
		{
			if (!cfg.strCopyPath.empty() && cfg.strCopyPath != strFilepath)
			{
				if (string::npos != cfg.strCopyPath.find(Hash2Char(hash.data).c_str()))
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "LoadTask change path [%s] to [%s]", strFilepath.c_str(), cfg.strCopyPath.c_str());
#endif
					strFilepath =  cfg.strCopyPath;					
				}
				else
				{
					cfg.strCopyPath.clear();
				}
			}
		}
#endif
		if ( HTTPDOWNTASK == cfg.type ||  HLSDOWNTASK  == cfg.type || HLSP2PDOWNTASK  == cfg.type)
		{
			iTaskRet = CServerProduct::LoadYfTask( hash, strFilepath,&cfg );
		}

		//任务加载成功
		if(iTaskRet >= 0)
		{
			nLoadTaskSuccess ++;
			// 下载文件保存绝对路径, ios不用保存
#ifndef MACOSX
			if (cfg.bCopyFullFile && cfg.strCopyPath.empty())
			{
				CTaskMgrInterFace::Instance()->SetCopyFullFile(hash, strFilepath);
			}
#endif
			//if (cfg.bCopyFullFile && cfg.copystatus != 1 && CTaskMgrInterFace::Instance()->TaskIsFinish(hash))
			//{
			//	//任务已经下载完了，需要拷贝且未拷贝，添加拷贝任务
			//	CCopyTaskFile::Instance()->AddOneTask(hash,cfg.strCopyPath);
			//}
		}
		else
		{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "LoadTask failed! hash=%s", Hash2Char(hash.data).c_str());
#endif
			CDbTaskConfig::Instance()->Delete(hash.data);
		}
	}
	return nLoadTaskSuccess;
}

void CInitApp::ReadConfig(const std::string& strConfigPath)
{
	CIniFile iniConfig;
	iniConfig.Load(strConfigPath.c_str());

	//加入启动时间
	char timebuf[16]={0};
	g_starttime = time(NULL);
	sprintf(timebuf,"%u",g_starttime);
	iniConfig.WriteString("GLOBALINFO","starttime",timebuf);

	//peerid
	memset(g_szSPeerID, 0, sizeof(g_szSPeerID));
	char szPeerID[41] = {0};
	if( iniConfig.ReadString("TERMINAL","UserID",szPeerID,41) )
	{
		Char2Hash(szPeerID,(unsigned char*)g_szSPeerID);
		Dec2((QVOD_BYTE*)g_szSPeerID,(QVOD_BYTE*)g_szSPeerID,20,g_iPeerIDkey);
	}

	//loglevel
	int iLogLevel = iniConfig.ReadInt("GLOBALINFO","loglevel",-1);
	if(iLogLevel == -1)
	{
		iLogLevel = 0;
		iniConfig.WriteString("GLOBALINFO","loglevel","0");
	}
	Setloglevel((_LOG_LEVEL)iLogLevel);

	//g_outdebuginfo
	int outdebuginfo = iniConfig.ReadInt("GLOBALINFO","outdebuginfo",-1);
	if(outdebuginfo == -1)
	{
		g_outdebuginfo = 2;
		iniConfig.WriteString("GLOBALINFO","outdebuginfo","2");
	}
	else
	{
		g_outdebuginfo = outdebuginfo;
	}	
}

QVOD_THREAD_RETURN CInitApp::NonBlockInit( void* pPara )
{	
	//解析一些后面要用到的域名
	CDNSCache::Instance()->Inquire(URLTRACKER);
	if (!g_bRunning)
	{
		//退出了
		CMsgPoolInterface::Instance()->Stop();
#ifndef WIN32
    return 0;
#else 
	return;
#endif
	}
	CInitApp* pApp = (CInitApp*)pPara;
	GetNatType();
#ifdef QVODDEBUG
	_OUTPUT(INFO,"g_NATtype:%d!\n",g_NATtype);
#endif // QVODDEBUG
	
	if(pApp->DoListenAction())
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO,"Listenport: %d,Nattype: %d\n",g_listenPort,g_NATtype);
#endif // QVODDEBUG
	}
	else
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO,"WARRING:Listen port be occupyed,listen fail!!!\n");
#endif
	}
#ifndef WIN32
    return 0;
#else 
	return;
#endif
}

bool CInitApp::DoListenAction()
{
	qvod_sock_t sock = QVOD_INVALID_SOCKET;

	//upnp映射
	if(0 == CUpnp::Instance()->Search(g_listenPort,g_listenPort))
	{
		if(g_isForceUdp)
		{
			g_NATtype = NatTypeConeNat;
		}
		else
		{
			g_NATtype = NatTypeOpen;
		}
	}

	if(sock == QVOD_INVALID_SOCKET)
	{
		return false;
	}
	//sock = CDispatchMgrInterface::Instance()->AddListenSock(sock);
	return true;
}
