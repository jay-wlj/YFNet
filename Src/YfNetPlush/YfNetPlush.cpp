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
	std::replace(g_media_dir.begin(), g_media_dir.end(), '\\', '/');		// ·��ͳһת��/

	g_strFullFileCopyPath = para.pDownloadPath;
	std::replace(g_strFullFileCopyPath.begin(), g_strFullFileCopyPath.end(), '\\', '/');		// ·��ͳһת��/

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
	std::replace(g_strCurrentRoot.begin(), g_strCurrentRoot.end(), '\\', '/');		// ·��ͳһת��/

	//�ж�Ŀ¼�Ƿ���ڣ��������򴴽�	
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
		//��������������ʼ��
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
		//��������������ע���ӿ�
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
GetPrivateVersion		��ȡ�ڲ��汾��
pVersion				[IN/OUT]�ǿ��ڴ棬���ڴ洢�汾����Ϣ
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
GetTaskInfo			��ѯ����ǰ��ϸ��Ϣ
pstrHash			[IN]  Ҫ��ѯ���ļ�hash
pBuf				[IN]  ������Ļ���������json��ʽ���
iBufLen				[IN/OUT] json��С,���������ش���E_NET_BUFFER_NOT_ENOUGHʱ,
					����pBuf�Ŀռ��С����,��ʱ��iBufLen������Ҫ����С����
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
CreateTask			������������
pstrSrcUrl			[IN]  Ҫ�������������,��ֱ������http�����ļ���url���磺
http://vodipad.wasu.cn/201407021742/3cc51ccdac8e4d8ca896152a95415347/pcsan08/mams/vod/201404/24/20/20140424202708118c8104efd_dc5607f3.mp4?version=P2PPlayer_V.1.5.16
pstrKeyurl			[IN]  pstrSrcUrl����ȡ�����Ĺ̶���url����
http://wasu.cn/20140424202708118c8104efd_dc5607f3.mp4
pstrReferer			[IN]  ����ҳ����ΪNULL
pstrHash			[OUT] pstrKeyurl��Ӧ��p2p hashֵ,40�ֽڵ��ַ�������������������Ĳ������Ը�ֵ��Ϊ����
bIsOtherCdn			[IN]  �Ƿ������CDN
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
	strOrgUrl = QvodFilterCharacters(strOrgUrl.c_str(), "\r\n ");		//���������ַ�

#ifndef SUPPORT_HTTPS
	if (strOrgUrl.find("http://") == string::npos)		// ֻ֧��httpЭ��
	{
		return E_NET_ERROR_PARA;
	}
#endif

	HASH hash;
	//��������hash
	CServerProduct::CreateTaskHash(strKeyUrl,hash.data);
	if (!(CTaskMgrInterFace::Instance()->FindHash(strKeyUrl,hash) &&
		CTaskMgrInterFace::Instance()->TaskIsExist(hash)))
	{
		//�����ڸ����񣬴�������
		QVOD_INT64 size = 0;
		int type = HTTPDOWNTASK;

		if (strOrgUrl.find(".m3u8") != string::npos)
		{
			type = HLSDOWNTASK;
		}

		//��ȡ����·��
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
			//��������ʧ��
			return E_NET_CREATE_TASK_FAILED;
		}
	}

	//���Դurl��ͬ�������url
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

	CTaskMgrInterFace::Instance()->SetAesEncrypt(hash, !bIsOtherCdn);		// ������CDN������

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CYfNetPlush::CreateTask pstrSrcUrl=%s key=%s hash=%s\n", pstrSrcUrl, pstrKeyurl, Hash2Char(hash.data).c_str());
#endif
	return E_NET_OK;
}

/**************************************************************************************
CreateHlsTask			����HLS��������
pstrSrcUrl			[IN]  Ҫ�������������,��ֱ������http�����ļ���url���磺
http://113.107.109.90/mp4/2015/dianshiju/yzg_22956/0627123DA36F6FC111496DAF3C7FC297_20151123_1_1_385.mp4/playlist.m3u8?uuid=d1597dfc4a5e4be38938161902298bc9&t=566ffbb2&win=300&pno=1120&srgid=12&urgid=372&srgids=12&nid=869&payload=usertoken%3duuid%3dd1597dfc4a5e4be38938161902298bc9%5eruip%3d3395848389%5ehit%3d1&rdur=21600&arange=0&limitrate=0&fid=0627123DA36F6FC111496DAF3C7FC297&sign=cd62bbe75ba76f50a8a6057dd1c331dc&ver=0x03
pstrKeyurl			[IN]  ��Ƶ��Ψһ��ʶ��Ҫ������ƽ̨��һ�£�Ŀǰ����Ϊ
http://hunantv.com/hunantv.imgo.tv/1846354_1.m3u8
http://hunantv.com/pcvideodn.titan.imgo.tv/1846354_1.m3u8

http://hunantv.com/ �ǹ̶��ģ��������CDN�����������â������CDN������hunantv.imgo.tv���棬����������CDN����ԭ������
����ǡ���Ƶ��ʶ+�»���+����������+.m3u8��������������ͬ��������Ƶ��ͬCDN���ӵ�keyurl��

bIsOtherCdn			[IN]  �Ƿ������CDN
pstrHash			[OUT] pstrKeyurl��Ӧ��p2p hashֵ,40�ֽڵ��ַ�������������������Ĳ������Ը�ֵ��Ϊ����
pstrHttpProxyUrl	[OUT] ����Ƶ��Ӧ�ı���http�����ַ���Ѹõ�ַ�����������в��ţ��ڴ��С����100�ֽ�
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

	// ȥ��urlǰ��ո�
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
	if (strOrgUrl.find("http://") == string::npos)		// ֻ֧��httpЭ��
	{
		return E_NET_ERROR_PARA;
	}
#endif


	HASH hash;
	//��������hash
	CServerProduct::CreateTaskHash(strKeyUrl,hash.data);

	if (!(CTaskMgrInterFace::Instance()->FindHash(strKeyUrl,hash) &&
		CTaskMgrInterFace::Instance()->TaskIsExist(hash)))
	{
		//�����ڸ����񣬴�������
		QVOD_INT64 size = 0;
		int type = HLSDOWNTASK;

		//��ȡ����·��
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
			//��������ʧ��
			return E_NET_CREATE_TASK_FAILED;
		}
	}

	//���Դurl��ͬ�������url
	CTaskMgrInterFace::Instance()->UpdateHttpOrgUrl(hash,strOrgUrl);
	memcpy(pstrHash,Hash2Char(hash.data).c_str(),40);	

	//�����ַ
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
	CTaskMgrInterFace::Instance()->SetAesEncrypt(hash, !bIsOtherCdn);		// ������CDN������

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CYfNetPlush::CreateHlsTask pstrSrcUrl=%s key=%s hash=%.40s\n", pstrSrcUrl, pstrKeyurl, pstrHash);
#endif
	return E_NET_OK;
}

/**************************************************************************************
CreateUpdateTask	������������
pstrSrcUrl			[IN]  Ҫ�������������,��ֱ������http�����ļ���url
pstrHash			[OUT]  �����p2p hashֵ��40�ֽڵ��ַ�������������������Ĳ������Ը�ֵ��Ϊ����
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
	std::replace(strFilepath.begin(), strFilepath.end(), '\\', '/');		// ·��ͳһת��/
	strOrgUrl = pstrSrcUrl;
	strKeyUrl = strOrgUrl+strFilepath;

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CYfNetPlush::CreateUploadTask pstrSrcUrl=%s\n", pstrSrcUrl);
#endif

	// ȥ��urlǰ��ո�
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
	if (strOrgUrl.find("http://") == string::npos)		// ֻ֧��httpЭ��
	{
		return E_NET_ERROR_PARA;
	}
#endif


	HASH hash;
	//��������hash
	CServerProduct::CreateTaskHash(strKeyUrl,hash.data);

	if (!(CTaskMgrInterFace::Instance()->FindHash(strKeyUrl,hash) &&
		CTaskMgrInterFace::Instance()->TaskIsExist(hash)))
	{
		//�����ڸ����񣬴�������
		QVOD_INT64 size = 0;

		int type = HTTPUPTASK+nType;

		if (QVOD_ACCESS(strFilepath.c_str(),0) != 0)
		{			
			return E_NET_PATH_NOT_ACCESS;
		}

		int ret = CServerProduct::CreateYfTask(hash,strFilepath,type,strOrgUrl,strKeyUrl,strReferer);
		if (ret < 0)
		{
			//��������ʧ��
			return E_NET_CREATE_TASK_FAILED;
		}
	}

	//���Դurl��ͬ�������url
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
DeleteTask			ɾ������
pstrHash			[IN] Ҫɾ���������hash
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
SetTaskDownload		��������Ϊ����ģʽ����ģʽ�������������´����ָ���洢·��
pstrHash			[IN] Ҫ���õ������hash
bOption				[IN] true or false
pFilename			[IN] ��bOptionΪtrue������Ҫ���ô洢���ļ���
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
SetTempCachePath	�����������ʱ�洢Ŀ¼
pstrPath			[IN]  ����·��ֵ
***************************************************************************************/
NET_ERROR CYfNetPlush::SetTempCachePath(const char *pstrPath)
{
	if (NULL == pstrPath || 0 == strlen(pstrPath))
	{
		return E_NET_ERROR_PARA;
	}
	g_media_dir = pstrPath;
	std::replace(g_media_dir.begin(), g_media_dir.end(), '\\', '/');		// ·��ͳһת��/
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
SetDefaultDownPath	������������Ĵ洢·��
pstrPath			[IN]  ����·��ֵ
***************************************************************************************/
NET_ERROR CYfNetPlush::SetDefaultDownPath(const char *pstrPath)
{
	return E_NET_OK;
}

/***************************************************************************************	
PauseTask			��ͣ����
pstrHash			[IN]  Ҫ��ͣ�������hash
bIsAll				[IN]  �Ƿ���ͣ��������,�����,pstrHash = NULL
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
RunTask				��������(��ʼ����)
pstrHash			[IN]  Ҫ���е������hash
bIsAll				[IN]  �Ƿ�������������,�����,pstrHash = NULL
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
			!(CTaskMgrInterFace::Instance()->GetTaskStatus(hash) == CTask::RUNNING) &&			// �ų���������״̬������
			(CTaskMgrInterFace::Instance()->GetDownloadingTasks(downList) >= g_cfg.m_uMaxDownTaskCount))	// ��ǰ�ǲ��������ҳ�����������������������
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
PlayTask			���������Ƿ����ڲ���
pstrHash			[IN]  Ҫ���ŵ������hash
bPlaying			[IN]  trueΪ���ţ�falseΪֹͣ����
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
ReadFile			��ȡ�ļ�����
pstrSrcUrl			[IN]  Ҫ�������������,��ֱ������http�����ļ���url���磺
http://vodipad.wasu.cn/201407021742/3cc51ccdac8e4d8ca896152a95415347/pcsan08/mams/vod/201404/24/20/20140424202708118c8104efd_dc5607f3.mp4,
�ò�����Ϊ�˸������ӣ���ֹ����ʧЧ���޷����ص�����
pstrHash			[IN]  Ҫ��ȡ���ļ�hash
pbuf				[OUT] �洢��ȡ���ݵĿռ�,�ɵ�����������ͷ�
nstart				[IN]  ��ʼλ��
nlen				[IN]  ��ȡ�ĳ���
pnreadlen			[OUT] ʵ�ʶ�ȡ�ĳ���
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
QueryFileSize		��ѯ�ļ���С
pstrHash			[IN]  Ҫ��ѯ���ļ�hash
iFilesize			[OUT] �洢��ѯ�����ļ���С����YfNetģ��δ�������ȡ��ʱ��iFilesizeΪ0
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
QueryTaskInfo		ͨ��Hash��ѯ������Ϣ
pstrHash			[IN]  Ҫ��ѯ���ļ�hash
pTaskInfo			[OUT] �����ѯ����������Ϣ,����������ռ��������ͷ�
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
QueryBitInfo		��ѯBitλ������Ϣ���մ�������ʱδ��ȡ���ļ���С���᷵��E_NET_FALSE
pstrHash			[IN]  Ҫ��ѯ���ļ�hash
pBuf				[IN]  ������Ļ��������Խṹ��SBitInfo���
iBufLen				[IN/OUT] �������λ��Ϣ�Ŀռ��С,���������ش���E_NET_BUFFER_NOT_ENOUGHʱ,
����pBuf�Ŀռ��С����,��ʱ��iBufLen������Ҫ����С����
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
SetSpaceCanUse			���ÿ��ô��̿ռ��С�������ļ���С
���ܳ�����ֵ���������������ã�0Ϊ���޴�С
spacecanuse:			[IN] ��λΪMB
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
SetPlayingPos		���õ�ǰ�Ĳ���λ��
pstrHash			[IN]  Ҫ���õ�����hash
pos					[IN]  Ҫ���õ�λ�ã���λΪByte
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
	//�����ǰ����λ��û�����ݣ�������������
	CTaskMgrInterFace::Instance()->DownEmergencyData(hash);


	return E_NET_OK;
}

/***************************************************************************************	
SetPlayingTimepoint		���õ�ǰ�Ĳ���ʱ��λ��
pstrHash			[IN]  Ҫ���õ�����hash
timepoint			[IN]  Ҫ���õ�ʱ��λ�ã���λΪms
bSeek				[IN]  �Ƿ�seek����Ĳ���λ�ñ䶯
iCacheTime			[OUT] ���滺���ʱ�䣬��λΪms
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
SetVideoDuration		������Ƶ��ʱ��
pstrHash			[IN]  Ҫ���õ�����hash
duration			[IN]  ��Ƶ��ʱ������λ��
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
SetLimitUploadSpeedMode		�����ϴ�����ģʽ
mode				[IN]  Ҫ��������ģʽ
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
SetLimitUploadSpeedPercentage		�����ϴ����ٰٷֱ�
percentage				[IN]  Ҫ�������ٰٷֱ�
***************************************************************************************/
NET_ERROR CYfNetPlush::SetLimitUploadSpeedPercentage(int percentage)
{
	return E_NET_OK;
}

/***************************************************************************************	
GetSampleSpeed			[IN]  ��ȡ10�����ڵ�ȡ��ƽ���ٶ�
speed					[IN]  �洢�ٶ�ֵ����λ Byte/s
***************************************************************************************/
NET_ERROR CYfNetPlush::GetSampleSpeed(int& speed)
{
	return E_NET_OK;
}

/***************************************************************************************	
NotifyVideoBuffering		֪ͨ��Ƶ�����ˣ����ڻ���
pstrHash			[IN]  Ҫ֪ͨ������hash
bBuffering 			[IN]  true:���ڻ���  false:�������
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
		// IOS���л�����̨һ��ʱ��󣬻�ر������ӣ�������Ҫ���л���ǰ̨���������������
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
	// ���½�������IP
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
	//��������hash
	string strUrlKey = pstrKey;
	CServerProduct::CreateTaskHash(strUrlKey, newhash.data);
	string strNewHash = Hash2Char(newhash.data);


	string strClarityPath;
	if (CTaskMgrInterFace::Instance()->TaskIsExist(newhash) && CTaskMgrInterFace::Instance()->IsCopyFullFile(newhash))
	{
		strClarityPath = CTaskMgrInterFace::Instance()->GetTaskPath(newhash);
		if (! strClarityPath.empty() && access(strClarityPath.c_str(), 0) != 0)				// ��ȡ���л�������������Ŀ¼
		{
			strClarityPath = "";
		}
	}
    string strOrgUrl = pstrSrcUrl;
    // ���½�������IP
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
		// ����״̬�б仯�����dns����

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
