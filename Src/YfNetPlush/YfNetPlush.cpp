#include "YfNetPlush.h"
#include "InitApp.h"
#include "TaskMgrInterface.h"
#include "ServerProduct.h"
#include "common-utility.h"
#include "CfgHost.h"
#include "json.h"
#include "HttpAgentInterface.h"
#include "CacheDataInMem.h"
#include "httpserver.h"
#include "MsgPoolInterface.h"
#include "NetUrl.h"

using namespace std;


CInitApp App;

IYfNetPlush*     g_pIYfNetPlush = NULL;

EventCallBackFunc g_eventCallBackFunc = NULL;

extern CHttpServerMgr g_httpServer;
#ifdef MACOSX
ECurrentScene g_currentScene = SCENE_FOREGROUND;
#endif

NET_ERROR WINAPI _InitYfNet(IYfNetPlush **pYfNet,InitPara para,long& lPointer)
{
	if (NULL == para.pConfigPath || NULL == para.pTempCachePath || NULL == para.pDownloadPath)
	{
		return E_NET_ERROR_PARA;
	}


	g_media_dir = para.pTempCachePath;
	std::replace(g_media_dir.begin(), g_media_dir.end(), '\\', '/');		// 路径统一转成/

	g_strFullFileCopyPath = para.pDownloadPath;
	std::replace(g_strFullFileCopyPath.begin(), g_strFullFileCopyPath.end(), '\\', '/');		// 路径统一转成/

	g_iMaxUseSpace = para.maxspace;
	g_iMaxUseSpace <<= 20;

	if (g_media_dir[g_media_dir.size()-1] != L'/' && g_media_dir[g_media_dir.size()-1] != L'\\')
	{
		g_media_dir += "/";
	}
#ifdef QVODDEBUG
    _OUTPUT(INFO, "g_media_dir=[%s]", g_media_dir.c_str());
#endif

	if (g_strFullFileCopyPath[g_strFullFileCopyPath.size()-1] != L'/' && g_strFullFileCopyPath[g_strFullFileCopyPath.size()-1] != L'\\')
	{
		g_strFullFileCopyPath += "/";
	}

	if (!CreateFolderForAllUser(g_media_dir.c_str()) ||
		!CreateFolderForAllUser(g_strFullFileCopyPath.c_str()))
	{
		return E_NET_CREATE_FOLDER_FAIL;
	}

#ifdef WIN32
	SetFileAttributes(g_media_dir.c_str(),FILE_ATTRIBUTE_HIDDEN);
#endif
	if (para.callback)
	{
		g_eventCallBackFunc = para.callback;
	}

	g_strCurrentRoot = para.pConfigPath;	
	std::replace(g_strCurrentRoot.begin(), g_strCurrentRoot.end(), '\\', '/');		// 路径统一转成/

	//判断目录是否存在，不存在则创建	
	if (g_strCurrentRoot[g_strCurrentRoot.size()-1] != L'/' && g_strCurrentRoot[g_strCurrentRoot.size()-1] != L'\\')
	{
		g_strCurrentRoot += "/";
	}

	if (!CreateFolderForAllUser(g_strCurrentRoot.c_str()))
	{
		return E_NET_CREATE_FOLDER_FAIL;
	}

	g_bIsUpgradeProc = para.bUpgradeProc;

	if (g_bIsUpgradeProc)
	{
		//升级程序调另外初始化
		if ( !App.Init() )
		{
			return E_NET_FALSE;
		}
	}
	else
	{
		if ( !App.Init() )
		{
			return E_NET_FALSE;
		}
	}

	CYfNetPlush* pNet = new CYfNetPlush(para);
	g_pIYfNetPlush = *pYfNet = pNet;
	lPointer = (long)pNet;

	return E_NET_OK;
}

void WINAPI _ClearYfNet(long lYfNetID)
{
	if (g_bIsUpgradeProc)
	{
		//升级程序调另外的注销接口
	}
	else
	{
		App.Close();
	}	

    g_pIYfNetPlush = NULL;
	if (lYfNetID != 0)
	{
		CYfNetPlush* pNet = (CYfNetPlush*)lYfNetID;
		SafeDelete(pNet);
	}
#ifdef QVODDEBUG
    _OUTPUT(INFO, "_ClearYfNet finish!");
	_OUTPUT(INFO, "-----------------END---------------------");
#endif

#ifdef QVOD_DEBUG_FILE
	if(debug_file)
	{
		fclose(debug_file);
		debug_file = NULL;
	}
#endif
}

CYfNetPlush::CYfNetPlush(InitPara & para)
{
	m_InitPara.callback = para.callback ;
}

CYfNetPlush::~CYfNetPlush()
{
}

/**************************************************************************************
GetPrivateVersion		获取内部版本号
pVersion				[IN/OUT]非空内存，用于存储版本号信息
***************************************************************************************/
NET_ERROR CYfNetPlush::GetPrivateVersion(char* pVersion)
{
	if (pVersion)
	{
		strcpy(pVersion,LIB_VERSION);
#ifdef QVOD_DEBUG_FILE
		strcat(pVersion,"_d");
#endif
	}
	return E_NET_OK;
}

NET_ERROR CYfNetPlush::SetConfig(const char* pstrJson)
{
	if (NULL == pstrJson)
	{
		return E_NET_ERROR_PARA;
	}
	bool bRet = g_cfg.SetConfig(string(pstrJson));

	return bRet? E_NET_OK: E_NET_ERROR_PARA;
}

NET_ERROR CYfNetPlush::SetLogLevel(int nLevel)
{
#ifdef QVODDEBUG
    _OUTPUT(INFO, "SetLogLevel nLevel=%d", nLevel);
#endif
	Setloglevel((_LOG_LEVEL)nLevel);
	return E_NET_OK;
}

/**************************************************************************************
GetTaskInfo			查询任务当前详细信息
pstrHash			[IN]  要查询的文件hash
pBuf				[IN]  是输入的缓冲区，以json格式填充
iBufLen				[IN/OUT] json大小,当函数返回错误E_NET_BUFFER_NOT_ENOUGH时,
					表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度
***************************************************************************************/
NET_ERROR CYfNetPlush::GetTaskInfo(const char *pstrHash, char *pBuf,int &iBufLen)
{
#ifdef QVODDEBUG
    _OUTPUT(INFO, "begin %s %d", __FUNCTION__, __LINE__);
#endif
	if (NULL == pstrHash || NULL == pBuf)
	{
		return E_NET_ERROR_PARA;
	}

	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}

#ifdef QVODDEBUG
    _OUTPUT(INFO, "begin CTaskMgrInterFace::Instance()->QueryTaskInfo");
#endif
    
	NET_ERROR err = CTaskMgrInterFace::Instance()->QueryTaskInfo(hash, pBuf, iBufLen);
#ifdef QVODDEBUG
    _OUTPUT(INFO, "end %s %d", __FUNCTION__, __LINE__);
#endif
}
/**************************************************************************************
CreateTask			创建播放任务
pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http下载文件的url，如：
http://vodipad.wasu.cn/201407021742/3cc51ccdac8e4d8ca896152a95415347/pcsan08/mams/vod/201404/24/20/20140424202708118c8104efd_dc5607f3.mp4?version=P2PPlayer_V.1.5.16
pstrKeyurl			[IN]  pstrSrcUrl中提取出来的固定的url，如
http://wasu.cn/20140424202708118c8104efd_dc5607f3.mp4
pstrReferer			[IN]  引用页，可为NULL
pstrHash			[OUT] pstrKeyurl对应的p2p hash值,40字节的字符串，创建任务后对任务的操作都以该值作为索引
bIsOtherCdn			[IN]  是否第三方CDN
***************************************************************************************/
NET_ERROR CYfNetPlush::CreateTask(const char *pstrSrcUrl,const char* pstrKeyurl,const char* pstrReferer,char *pstrHash,char *pstrHttpProxyUrl,bool bIsOtherCdn)
{
	if (pstrSrcUrl == NULL || pstrHash == NULL || pstrKeyurl == NULL) 
	{
		return E_NET_ERROR_PARA;
	}
	if (0 == strlen(pstrSrcUrl) || 0 == strlen(pstrKeyurl))
	{
		return E_NET_ERROR_PARA;
	}

	string strOrgUrl,strKeyUrl,strReferer;
	strOrgUrl = pstrSrcUrl;
	strKeyUrl = pstrKeyurl;
	if (pstrReferer != NULL)
	{
		strReferer = pstrReferer;
	}
	//int nPos  = strOrgUrl.find_first_not_of(' ');
	//if (nPos != string::npos)
	//{
	//	strOrgUrl = strOrgUrl.substr(nPos);
	//}
	strOrgUrl = QvodFilterCharacters(strOrgUrl.c_str(), "\r\n ");		//过滤特殊字符

#ifndef SUPPORT_HTTPS
	if (strOrgUrl.find("http://") == string::npos)		// 只支持http协议
	{
		return E_NET_ERROR_PARA;
	}
#endif

	HASH hash;
	//生成任务hash
	CServerProduct::CreateTaskHash(strKeyUrl,hash.data);
	if (!(CTaskMgrInterFace::Instance()->FindHash(strKeyUrl,hash) &&
		CTaskMgrInterFace::Instance()->TaskIsExist(hash)))
	{
		//不存在该任务，创建任务
		QVOD_INT64 size = 0;
		int type = HTTPDOWNTASK;

		if (strOrgUrl.find(".m3u8") != string::npos)
		{
			type = HLSDOWNTASK;
		}

		//获取下载路径
		string strFilepath = g_media_dir + Hash2Char(hash.data) + "/";
		QvodMkdir(strFilepath.c_str());
		if (QVOD_ACCESS(strFilepath.c_str(),0) != 0)
		{			
			//return E_NET_PATH_NOT_ACCESS;
		}

		strFilepath += Hash2Char(hash.data) + ".vdata";
		int ret = CServerProduct::CreateYfTask(hash,strFilepath,type,strOrgUrl,strKeyUrl,strReferer);
		if (ret < 0)
		{
			//创建任务失败
			return E_NET_CREATE_TASK_FAILED;
		}
	}

	//如果源url不同，则更新url
	CTaskMgrInterFace::Instance()->UpdateHttpOrgUrl(hash,strOrgUrl);
	memcpy(pstrHash,Hash2Char(hash.data).c_str(),40);	

	if (pstrHttpProxyUrl)
	{
        string strExtType = GetPathSuffix(strOrgUrl.c_str());
        if (strExtType.length() > 10)
        {
            strExtType = "";
        }
		sprintf(pstrHttpProxyUrl, "http://127.0.0.1:%d/yfhttpagent/%s/playfile%s", g_httpport, string(pstrHash,40).c_str(), strExtType.c_str()) ;
	}

	if (CTaskMgrInterFace::Instance()->TaskIsFinish(hash))
	{
		return E_NET_TASK_FINISH;
	}

	CTaskMgrInterFace::Instance()->SetAesEncrypt(hash, !bIsOtherCdn);		// 第三方CDN不加密

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CYfNetPlush::CreateTask pstrSrcUrl=%s key=%s hash=%s\n", pstrSrcUrl, pstrKeyurl, Hash2Char(hash.data).c_str());
#endif
	return E_NET_OK;
}

/**************************************************************************************
CreateHlsTask			创建HLS播放任务
pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http下载文件的url，如：
http://113.107.109.90/mp4/2015/dianshiju/yzg_22956/0627123DA36F6FC111496DAF3C7FC297_20151123_1_1_385.mp4/playlist.m3u8?uuid=d1597dfc4a5e4be38938161902298bc9&t=566ffbb2&win=300&pno=1120&srgid=12&urgid=372&srgids=12&nid=869&payload=usertoken%3duuid%3dd1597dfc4a5e4be38938161902298bc9%5eruip%3d3395848389%5ehit%3d1&rdur=21600&arange=0&limitrate=0&fid=0627123DA36F6FC111496DAF3C7FC297&sign=cd62bbe75ba76f50a8a6057dd1c331dc&ver=0x03
pstrKeyurl			[IN]  视频的唯一标识，要和其他平台的一致，目前规则为
http://hunantv.com/hunantv.imgo.tv/1846354_1.m3u8
http://hunantv.com/pcvideodn.titan.imgo.tv/1846354_1.m3u8

http://hunantv.com/ 是固定的，后面加上CDN域名，如果是芒果自有CDN，就以hunantv.imgo.tv代替，其他第三方CDN则填原域名。
最后是“视频标识+下划线+清晰度类型+.m3u8”，如上两个是同个标清视频不同CDN链接的keyurl。

bIsOtherCdn			[IN]  是否第三方CDN
pstrHash			[OUT] pstrKeyurl对应的p2p hash值,40字节的字符串，创建任务后对任务的操作都以该值作为索引
pstrHttpProxyUrl	[OUT] 该视频对应的本地http代理地址，把该地址给播放器进行播放，内存大小至少100字节
***************************************************************************************/
NET_ERROR CYfNetPlush::CreateHlsTask(const char *pstrSrcUrl,const char* pstrKeyurl,bool bIsOtherCdn,char *pstrHash,char* pstrHttpProxyUrl)
{
	if (pstrSrcUrl == NULL || pstrHash == NULL || pstrKeyurl == NULL) 
	{
		return E_NET_ERROR_PARA;
	}
	if (0 == strlen(pstrSrcUrl) || 0 == strlen(pstrKeyurl))
	{
		return E_NET_ERROR_PARA;
	}

	string strOrgUrl,strKeyUrl,strReferer;
	strOrgUrl = pstrSrcUrl;
	strKeyUrl = pstrKeyurl;

	//strOrgUrl="http://vodpub.oss-cn-hangzhou.aliyuncs.com/720p/16_9/917d/45bb/917d45bbbbf6abfeca8668d8eadc86e7.m3u8";
	//strKeyUrl = strOrgUrl;
	//g_media_dir = "F:/";

	// 去掉url前后空格
	int nPos  = strOrgUrl.find_first_not_of(' ');
	if (nPos != string::npos)
	{
		strOrgUrl = strOrgUrl.substr(nPos);
		nPos = strOrgUrl.find_first_of(' ');
		if (nPos != string::npos)
		{
			strOrgUrl = strOrgUrl.substr(0, nPos);
		}
	}

#ifndef SUPPORT_HTTPS
	if (strOrgUrl.find("http://") == string::npos)		// 只支持http协议
	{
		return E_NET_ERROR_PARA;
	}
#endif


	HASH hash;
	//生成任务hash
	CServerProduct::CreateTaskHash(strKeyUrl,hash.data);

	if (!(CTaskMgrInterFace::Instance()->FindHash(strKeyUrl,hash) &&
		CTaskMgrInterFace::Instance()->TaskIsExist(hash)))
	{
		//不存在该任务，创建任务
		QVOD_INT64 size = 0;
		int type = HLSDOWNTASK;

		//获取下载路径
		string strFilepath = g_media_dir + Hash2Char(hash.data) + "/";

		QvodMkdir(strFilepath.c_str());
		if (QVOD_ACCESS(strFilepath.c_str(),0) != 0)
		{			
//#ifdef QVODDEBUG
//			_OUTPUT(INFO, " CreateHlsTask failed! ret=E_NET_PATH_NOT_ACCESS path=[%s]", strFilepath.c_str());
//#endif
//			return E_NET_PATH_NOT_ACCESS;
		}

		strFilepath += Hash2Char(hash.data) + ".vdata";
		int ret = CServerProduct::CreateYfTask(hash,strFilepath,type,strOrgUrl,strKeyUrl,strReferer);
		if (ret < 0)
		{
			//创建任务失败
			return E_NET_CREATE_TASK_FAILED;
		}
	}

	//如果源url不同，则更新url
	CTaskMgrInterFace::Instance()->UpdateHttpOrgUrl(hash,strOrgUrl);
	memcpy(pstrHash,Hash2Char(hash.data).c_str(),40);	

	//代理地址
	char strip[16] = {0};
	IpInt2Str(g_iLocalIP, strip);
	
	if (pstrHttpProxyUrl)
	{
		sprintf(pstrHttpProxyUrl, "http://127.0.0.1:%d/yfhttpagent/%s/playlist.m3u8", g_httpport, string(pstrHash,40).c_str()) ;
	}
	
	if (CTaskMgrInterFace::Instance()->TaskIsFinish(hash))
	{
		return E_NET_TASK_FINISH;
	}
	CTaskMgrInterFace::Instance()->SetAesEncrypt(hash, !bIsOtherCdn);		// 第三方CDN不加密

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CYfNetPlush::CreateHlsTask pstrSrcUrl=%s key=%s hash=%.40s\n", pstrSrcUrl, pstrKeyurl, pstrHash);
#endif
	return E_NET_OK;
}

/**************************************************************************************
CreateUpdateTask	创建升级任务
pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http下载文件的url
pstrHash			[OUT]  任务的p2p hash值，40字节的字符串，创建任务后对任务的操作都以该值作为索引
***************************************************************************************/
NET_ERROR CYfNetPlush::CreateUpdateTask(const char *pstrSrcUrl,char* pstrHash)
{
	return E_NET_OK;
}

NET_ERROR CYfNetPlush::CreateUploadTask(int nType, const char *pstrSrcUrl, const char* pstrfilepath, char *pstrHash)
{
	if (NULL == pstrSrcUrl || 0==strlen(pstrSrcUrl) || NULL == pstrfilepath || NULL == pstrHash)
	{
		return E_NET_ERROR_PARA;
	}

	string strOrgUrl,strKeyUrl,strReferer;
	string strFilepath = pstrfilepath;
	std::replace(strFilepath.begin(), strFilepath.end(), '\\', '/');		// 路径统一转成/
	strOrgUrl = pstrSrcUrl;
	strKeyUrl = strOrgUrl+strFilepath;

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CYfNetPlush::CreateUploadTask pstrSrcUrl=%s\n", pstrSrcUrl);
#endif

	// 去掉url前后空格
	int nPos  = strOrgUrl.find_first_not_of(' ');
	if (nPos != string::npos)
	{
		strOrgUrl = strOrgUrl.substr(nPos);
		nPos = strOrgUrl.find_first_of(' ');
		if (nPos != string::npos)
		{
			strOrgUrl = strOrgUrl.substr(0, nPos);
		}
	}

#ifndef SUPPORT_HTTPS
	if (strOrgUrl.find("http://") == string::npos)		// 只支持http协议
	{
		return E_NET_ERROR_PARA;
	}
#endif


	HASH hash;
	//生成任务hash
	CServerProduct::CreateTaskHash(strKeyUrl,hash.data);

	if (!(CTaskMgrInterFace::Instance()->FindHash(strKeyUrl,hash) &&
		CTaskMgrInterFace::Instance()->TaskIsExist(hash)))
	{
		//不存在该任务，创建任务
		QVOD_INT64 size = 0;

		int type = HTTPUPTASK+nType;

		if (QVOD_ACCESS(strFilepath.c_str(),0) != 0)
		{			
			return E_NET_PATH_NOT_ACCESS;
		}

		int ret = CServerProduct::CreateYfTask(hash,strFilepath,type,strOrgUrl,strKeyUrl,strReferer);
		if (ret < 0)
		{
			//创建任务失败
			return E_NET_CREATE_TASK_FAILED;
		}
	}

	//如果源url不同，则更新url
	CTaskMgrInterFace::Instance()->UpdateHttpOrgUrl(hash,strOrgUrl);
	memcpy(pstrHash,Hash2Char(hash.data).c_str(),40);
    return E_NET_OK;
}

NET_ERROR CYfNetPlush::SetHeadValue(const char *pstrHash, const char *pstHead, const char* pstrValue)
{
	if (NULL == pstrHash || (0 == strlen(pstrHash)))
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	return CTaskMgrInterFace::Instance()->SetHeadValue(hash,pstHead, pstrValue);
    
}

NET_ERROR CYfNetPlush::SetHeadValue(const char *pstrHash, const char *pstrHeadAdd)
{
	if (NULL == pstrHash || (0 == strlen(pstrHash)))
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	return CTaskMgrInterFace::Instance()->SetHeadValue(hash, pstrHeadAdd);
}
/***************************************************************************************	
DeleteTask			删除任务
pstrHash			[IN] 要删除的任务的hash
***************************************************************************************/
NET_ERROR CYfNetPlush::DeleteTask(const char *pstrHash)
{
	if (NULL == pstrHash || (0 == strlen(pstrHash)))
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	CServerProduct::SetTaskStatus(hash,CTask::STOP);
#ifdef QVODDEBUG
    _OUTPUT(INFO, "CYfNetPlush::DeleteTask %s", pstrHash);
#endif
	return E_NET_OK;
}

/***************************************************************************************	
SetTaskDownload		设置任务为下载模式，该模式在任务下载完后会拷贝到指定存储路径
pstrHash			[IN] 要设置的任务的hash
bOption				[IN] true or false
pFilename			[IN] 若bOption为true，则需要设置存储的文件名
***************************************************************************************/
NET_ERROR CYfNetPlush::SetTaskDownload(const char *pstrHash,bool bOption,const char* pFilename)
{
	if (NULL == pstrHash /*|| (bOption && NULL == pFilename)*/)
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	if (bOption)
	{
		string strFilepath;
		if (pFilename)
		{
			strFilepath = g_strFullFileCopyPath + pFilename;
		}
		CTaskMgrInterFace::Instance()->SetCopyFullFile(hash,strFilepath);
	}
	else
	{
		CTaskMgrInterFace::Instance()->UnsetCopyFullFile(hash);
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "SetTaskDownload hash=%s down=%d", pstrHash, bOption);
#endif
	return E_NET_OK;
}

/***************************************************************************************	
SetTempCachePath	设置任务的临时存储目录
pstrPath			[IN]  绝对路径值
***************************************************************************************/
NET_ERROR CYfNetPlush::SetTempCachePath(const char *pstrPath)
{
	if (NULL == pstrPath || 0 == strlen(pstrPath))
	{
		return E_NET_ERROR_PARA;
	}
	g_media_dir = pstrPath;
	std::replace(g_media_dir.begin(), g_media_dir.end(), '\\', '/');		// 路径统一转成/
	if (g_media_dir[g_media_dir.size()-1] != L'/' && g_media_dir[g_media_dir.size()-1] != L'\\')
	{
		g_media_dir += "/";
	}
	if (!CreateFolderForAllUser(g_media_dir.c_str()))
	{
		return E_NET_CREATE_FOLDER_FAIL;
	}
#ifdef WIN32
	SetFileAttributes(g_media_dir.c_str(),FILE_ATTRIBUTE_HIDDEN);
#endif
#ifdef QVODDEBUG
	_OUTPUT(INFO, "SetTempCachePath path=%s", g_media_dir.c_str());
#endif
	return E_NET_OK;
}

/***************************************************************************************	
SetDefaultDownPath	设置下载任务的存储路径
pstrPath			[IN]  绝对路径值
***************************************************************************************/
NET_ERROR CYfNetPlush::SetDefaultDownPath(const char *pstrPath)
{
	return E_NET_OK;
}

/***************************************************************************************	
PauseTask			暂停任务
pstrHash			[IN]  要暂停的任务的hash
bIsAll				[IN]  是否暂停所有任务,如果是,pstrHash = NULL
***************************************************************************************/
NET_ERROR CYfNetPlush::PauseTask(const char *pstrHash,bool bIsAll/*=false*/)
{
	if (NULL == pstrHash)
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	CServerProduct::SetTaskStatus(hash,CTask::PAUSE,bIsAll);
#ifdef QVODDEBUG
	_OUTPUT(INFO,"PauseTask %.40s\n",pstrHash);
#endif
	return E_NET_OK;
}


/***************************************************************************************	
RunTask				运行任务(开始下载)
pstrHash			[IN]  要运行的任务的hash
bIsAll				[IN]  是否运行所有任务,如果是,pstrHash = NULL
***************************************************************************************/
NET_ERROR CYfNetPlush::RunTask(const char *pstrHash,bool bIsAll/*=false*/)
{
	if (NULL == pstrHash)
	{
		return E_NET_ERROR_PARA;
	}

	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	if (CTaskMgrInterFace::Instance()->TaskIsFinish(hash, true))
	{
		return E_NET_TASK_FINISH;
	}

	if (-1 != g_cfg.m_uMaxDownTaskCount)
	{
		list<HASH> downList;
		if (!CTaskMgrInterFace::Instance()->IsTaskPlaying(hash) &&
			!(CTaskMgrInterFace::Instance()->GetTaskStatus(hash) == CTask::RUNNING) &&			// 排除已在下载状态的任务
			(CTaskMgrInterFace::Instance()->GetDownloadingTasks(downList) >= g_cfg.m_uMaxDownTaskCount))	// 当前非播放任务，且超过最大离线任务数，则忽略
		{
	#ifdef QVODDEBUG
			string str;
			for (list<HASH>::const_iterator itr=downList.begin();itr!=downList.end();itr++)
			{
				string strHash = Hash2Char((*itr).data);
				str += strHash;
				str += " ";
			}
			
			_OUTPUT(INFO, "CYfNetPlush::RunTask failed err=E_NET_DOWNTASK_QUEUE_FULL!  bPlaying=%d g_cfg.m_uMaxDownTaskCount=%d curdownHashs:[%s] needdownhash:[%s]", CTaskMgrInterFace::Instance()->IsTaskPlaying(hash), g_cfg.m_uMaxDownTaskCount, str.c_str(), pstrHash);
	#endif
			return E_NET_DOWNTASK_QUEUE_FULL;
		}
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO,"RunTask %.40s\n",pstrHash);
#endif
	CServerProduct::SetTaskStatus(hash,CTask::RUNNING,bIsAll);
	return E_NET_OK;
}

/***************************************************************************************	
PlayTask			设置任务是否正在播放
pstrHash			[IN]  要播放的任务的hash
bPlaying			[IN]  true为播放，false为停止播放
***************************************************************************************/
NET_ERROR CYfNetPlush::SetTaskPlaying(const char *pstrHash,bool bPlaying)
{
	if (NULL == pstrHash)
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	CTaskMgrInterFace::Instance()->SetTaskPlaying(hash, bPlaying);
	return E_NET_OK;
}

NET_ERROR CYfNetPlush::TaskIsPlaying(const char *pstrHash,bool& bPlaying)
{
	bPlaying = false;
	if (NULL == pstrHash)
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	bPlaying = CTaskMgrInterFace::Instance()->IsTaskPlaying(hash);
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CYfNetPlush::TaskIsPlaying hash=%s bPlaying=%d", pstrHash, bPlaying);
#endif
	return E_NET_OK;
}
/***************************************************************************************	
ReadFile			读取文件内容
pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http下载文件的url，如：
http://vodipad.wasu.cn/201407021742/3cc51ccdac8e4d8ca896152a95415347/pcsan08/mams/vod/201404/24/20/20140424202708118c8104efd_dc5607f3.mp4,
该参数是为了更新链接，防止链接失效后无法下载的问题
pstrHash			[IN]  要读取的文件hash
pbuf				[OUT] 存储读取内容的空间,由调用者申请和释放
nstart				[IN]  起始位置
nlen				[IN]  读取的长度
pnreadlen			[OUT] 实际读取的长度
***************************************************************************************/
NET_ERROR CYfNetPlush::ReadFile(const char *pstrSrcUrl,const char *pstrHash,char *pbuf,long long nstart,long nlen,long *pnreadlen)
{
	if (NULL == pstrHash || NULL==pbuf || NULL==pnreadlen)
	{
		return E_NET_ERROR_PARA;
	}

	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	*pnreadlen = CTaskMgrInterFace::Instance()->ReadPiece(hash, pbuf, nlen, nstart, false);
	if (*pnreadlen <= 0)
	{
		*pnreadlen = CHttpAgentInterface::Instance()->ReadPiece(hash,pbuf,nlen,nstart);
#ifdef QVOD_USE_CACHEMEM
		if (*pnreadlen <= 0)
		{
			*pnreadlen = CCacheDataInMem::Instance().ReadData(hash,pbuf,nstart,nlen);
		}
#endif
	}
	return E_NET_OK;
}

/***************************************************************************************	
QueryFileSize		查询文件大小
pstrHash			[IN]  要查询的文件hash
iFilesize			[OUT] 存储查询到的文件大小，当YfNet模块未从网络获取到时则iFilesize为0
***************************************************************************************/
NET_ERROR CYfNetPlush::QueryFileSize(const char *pstrHash,long long& iFilesize)
{
	if (NULL == pstrHash)
	{
		return E_NET_ERROR_PARA;
	}

	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	iFilesize = CTaskMgrInterFace::Instance()->QueryFileLen(hash, 0);
	return E_NET_OK;
}


NET_ERROR CYfNetPlush::TaskIsFinished(const char *pstrHash, bool& bFinish) 
{
	bFinish = false;
	if (NULL == pstrHash)
	{
		return E_NET_ERROR_PARA;
	}	

	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	bFinish = CTaskMgrInterFace::Instance()->TaskIsFinish(hash, true);	
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CYfNetPlush::TaskIsFinished begin! hash=%s bFinish=%d", pstrHash, bFinish);
#endif
	return E_NET_OK;
}
/*****************************************************************************************
QueryTaskInfo		通过Hash查询任务信息
pstrHash			[IN]  要查询的文件hash
pTaskInfo			[OUT] 保存查询到的任务信息,函数不负责空间的申请和释放
****************************************************************************************/
NET_ERROR CYfNetPlush::QueryTaskInfo(const char *pstrHash,STaskInfo *pTaskInfo)
{
	if (NULL == pstrHash || NULL == pTaskInfo)
	{
		return E_NET_ERROR_PARA;
	}

	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	CTaskMgrInterFace::Instance()->QueryTaskInfo(hash,pTaskInfo);

	return E_NET_OK;
}

/***************************************************************************************	
QueryBitInfo		查询Bit位下载信息，刚创建任务时未获取到文件大小，会返回E_NET_FALSE
pstrHash			[IN]  要查询的文件hash
pBuf				[IN]  是输入的缓冲区，以结构体SBitInfo填充
iBufLen				[IN/OUT] 保存比特位信息的空间大小,当函数返回错误E_NET_BUFFER_NOT_ENOUGH时,
表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度
***************************************************************************************/
NET_ERROR CYfNetPlush::QueryTaskBitInfo(const char *pstrHash, char *pBuf,int &iBufLen)
{
	if (NULL == pstrHash || NULL == pBuf)
	{
		return E_NET_ERROR_PARA;
	}

	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	
	if (-2 == CTaskMgrInterFace::Instance()->QueryBitInfo(hash, (SBitInfo*)pBuf, iBufLen))
	{
		return E_NET_BUFFER_NOT_ENOUGH;
	}
	return E_NET_OK;
}

/***********************************************************************
SetSpaceCanUse			设置可用磁盘空间大小，所有文件大小
不能超过此值，程序启动后设置，0为不限大小
spacecanuse:			[IN] 单位为MB
*******************************************************************/
NET_ERROR CYfNetPlush::SetSpaceCanUse(unsigned int spacecanuse)
{
	g_iMaxUseSpace = spacecanuse;
	return E_NET_OK;
}

NET_ERROR CYfNetPlush::CleanCache()
{
	//CAutoLock lock(m_lock);
#ifdef QVODDEBUG
	_OUTPUT(INFO,"*****CleanCache*****\n");
#endif
	set<HASH> hashSet;
	CTaskMgrInterFace::Instance()->GetCacheHash(hashSet);
	for (set<HASH>::iterator iter = hashSet.begin(); iter != hashSet.end(); ++iter)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO,"*****CleanCache Delete %s*****\n",Hash2Char(iter->data).c_str());
#endif
		CServerProduct::SetTaskStatus((HASH&)*iter,CTask::STOP);
	}
	return E_NET_OK;
}

/***************************************************************************************	
SetPlayingPos		设置当前的播放位置
pstrHash			[IN]  要设置的任务hash
pos					[IN]  要设置的位置，单位为Byte
***************************************************************************************/
NET_ERROR CYfNetPlush::SetPlayingPos(const char *pstrHash,long long pos)
{
	if (NULL == pstrHash )
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}

	CTaskMgrInterFace::Instance()->SetPlayFilePos(hash,pos);
	//如果当前播放位置没有数据，紧急下载数据
	CTaskMgrInterFace::Instance()->DownEmergencyData(hash);


	return E_NET_OK;
}

/***************************************************************************************	
SetPlayingTimepoint		设置当前的播放时间位置
pstrHash			[IN]  要设置的任务hash
timepoint			[IN]  要设置的时间位置，单位为ms
bSeek				[IN]  是否seek引起的播放位置变动
iCacheTime			[OUT] 保存缓存的时间，单位为ms
***************************************************************************************/
NET_ERROR CYfNetPlush::SetPlayingTimepoint(const char *pstrHash,int timepoint,bool bSeek,int& iCacheTime)
{
	if (NULL == pstrHash)
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}

	CTaskMgrInterFace::Instance()->SetPlayingTimepoint(hash, timepoint, bSeek, iCacheTime);
	return E_NET_OK;
}

/***************************************************************************************	
SetVideoDuration		设置视频的时长
pstrHash			[IN]  要设置的任务hash
duration			[IN]  视频的时长，单位秒
***************************************************************************************/
NET_ERROR CYfNetPlush::SetVideoDuration(const char *pstrHash,int duration)
{
	if (NULL == pstrHash )
	{
		return E_NET_ERROR_PARA;
	}
	HASH hash;
	Char2Hash(pstrHash,hash.data);
	if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
	{
		return E_NET_NO_TASK_BY_HASH;
	}
	CTaskMgrInterFace::Instance()->SetVideoDuration(hash, duration);
	return E_NET_OK;
}

/***************************************************************************************	
SetLimitUploadSpeedMode		设置上传限速模式
mode				[IN]  要设置限速模式
***************************************************************************************/
NET_ERROR CYfNetPlush::SetLimitUploadSpeedMode(ELimitUploadSpeedMode mode)
{
	g_cfg.m_limitSpeedMode = mode;
	SetGlobalSpeed(g_cfg.m_limitSpeedMode);
	CMsgPoolInterface::Instance()->SetLimitUpSpeed(g_iUpRateLimit);
	CMsgPoolInterface::Instance()->SetLimitDownSpeed(g_iDownRateLimit);
	return E_NET_OK;
}

/***************************************************************************************	
SetLimitUploadSpeedPercentage		设置上传限速百分比
percentage				[IN]  要设置限速百分比
***************************************************************************************/
NET_ERROR CYfNetPlush::SetLimitUploadSpeedPercentage(int percentage)
{
	return E_NET_OK;
}

/***************************************************************************************	
GetSampleSpeed			[IN]  获取10分钟内的取样平均速度
speed					[IN]  存储速度值，单位 Byte/s
***************************************************************************************/
NET_ERROR CYfNetPlush::GetSampleSpeed(int& speed)
{
	return E_NET_OK;
}

/***************************************************************************************	
NotifyVideoBuffering		通知视频卡顿了，正在缓冲
pstrHash			[IN]  要通知的任务hash
bBuffering 			[IN]  true:正在缓冲  false:缓冲结束
***************************************************************************************/
NET_ERROR CYfNetPlush::NotifyVideoBuffering(const char *pstrHash, bool bBuffering)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "NotifyVideoBuffering begin hash=%s bBuffering=%d", pstrHash, bBuffering);
#endif
	g_cfg.m_bVideoBuffering = bBuffering;
	HASH hash;
    Char2Hash(pstrHash,hash.data);
    if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
    {
        return E_NET_NO_TASK_BY_HASH;
    }
	CTaskMgrInterFace::Instance()->NotifyVideoBuffering(hash, bBuffering);
	return E_NET_OK;
}


#ifdef MACOSX
NET_ERROR CYfNetPlush::SetCurrentScene(ECurrentScene scene)
{
	//CAutoLock lock(m_lock);
#ifdef QVODDEBUG
    _OUTPUT(INFO,"*****SetCurrentScene %d*****\n",scene);
#endif
	g_currentScene = scene;
	if (SCENE_FOREGROUND == scene)
	{
		// IOS端切换到后台一段时间后，会关闭其连接，所以需要在切换到前台后重启代理服务器
		g_httpServer.ReInit();
        // CTaskMgrInterFace::Instance()->ResetAllGettingPeerFlag();
	}
	else	if (SCENE_BACKGROUND == scene)
	{
		
	}
	return E_NET_OK;
}
#endif

NET_ERROR CYfNetPlush::UpdateHttpOrgUrl(const char *pstrHash, const char *pstrSrcUrl)
{
    if (NULL == pstrHash || NULL == pstrSrcUrl)
    {
        return E_NET_ERROR_PARA;
    }
    HASH hash;
    Char2Hash(pstrHash,hash.data);
    if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
    {
        return E_NET_NO_TASK_BY_HASH;
    }
    string strOrgUrl = pstrSrcUrl;
	// 重新解析域名IP
	std::string strHost, strParams;
	std::string strUrl = NetUrl::GetURLHost(strOrgUrl, strHost, strParams);
	//CDNSCache::Instance()->Remove(strHost);
	CDNSCache::Instance()->Inquire(strHost, true);

    CTaskMgrInterFace::Instance()->UpdateHttpOrgUrl(hash, strOrgUrl);

    return E_NET_OK;
}

NET_ERROR CYfNetPlush::UpdateClarityUrl(const char *pstrHash, const char *pstrSrcUrl, const char *pstrKey, int nClarity)
{
    if (NULL == pstrHash || NULL == pstrSrcUrl || NULL == pstrKey)
    {
        return E_NET_ERROR_PARA;
    }
    HASH hash;
    Char2Hash(pstrHash,hash.data);
    if (!CTaskMgrInterFace::Instance()->TaskIsExist(hash))
    {
        return E_NET_NO_TASK_BY_HASH;
    }

	HASH newhash;
	//生成任务hash
	string strUrlKey = pstrKey;
	CServerProduct::CreateTaskHash(strUrlKey, newhash.data);
	string strNewHash = Hash2Char(newhash.data);


	string strClarityPath;
	if (CTaskMgrInterFace::Instance()->TaskIsExist(newhash) && CTaskMgrInterFace::Instance()->IsCopyFullFile(newhash))
	{
		strClarityPath = CTaskMgrInterFace::Instance()->GetTaskPath(newhash);
		if (! strClarityPath.empty() && access(strClarityPath.c_str(), 0) != 0)				// 获取该切换的清晰度所在目录
		{
			strClarityPath = "";
		}
	}
    string strOrgUrl = pstrSrcUrl;
    // 重新解析域名IP
    std::string strHost, strParams;
    std::string strUrl = NetUrl::GetURLHost(strOrgUrl, strHost, strParams);
    //CDNSCache::Instance()->Remove(strHost);
    CDNSCache::Instance()->Inquire(strHost, true);
    
	int ret = CTaskMgrInterFace::Instance()->UpdateClarityUrl(hash, strOrgUrl, strNewHash, strClarityPath, nClarity);
	if (ret < 0)
	{
		return E_NET_FALSE;
	}
    return E_NET_OK;
}

NET_ERROR CYfNetPlush::UpdateNetType(const ENetType& nNetType)
{
	if (g_cfg.m_nCurNetType != nNetType)
	{
		g_cfg.m_nCurNetType = (int)nNetType;
		// 网络状态有变化，清空dns解析

		CDNSCache::Instance()->ReParseDnsAll();
	}
	return E_NET_OK;
}

NET_ERROR CYfNetPlush::GetDownloadingTaskLists(char *pBuf, int &iBufLen)
{
    if (NULL == pBuf )
    {
        return E_NET_ERROR_PARA;
    }

	list<HASH> downhashlists;
    int nDownTaskCount = CTaskMgrInterFace::Instance()->GetDownloadingTasks(downhashlists);
	Json::Value root;
	Json::Value data;
	for (list<HASH>::const_iterator itr=downhashlists.begin(); itr!=downhashlists.end(); itr++)
	{
		data.append(Hash2Char((*itr).data).c_str());	
	}
	root["hashlist"] = data;
	Json::FastWriter writer;
	std::string strInfo = writer.write(root);
	if (iBufLen <= strInfo.length())
	{
		iBufLen = strInfo.length()+1;
		return E_NET_BUFFER_NOT_ENOUGH;
	}
	strncpy(pBuf, strInfo.c_str(), strInfo.length());
    return E_NET_OK;
}
