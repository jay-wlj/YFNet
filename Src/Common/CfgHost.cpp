#include "CfgHost.h"
#include "json.h"
#include "common-commonfun.h"

CfgHost g_cfg;

CfgHost::CfgHost(void):
m_uConcurrentNum(1),
m_uUpConcurrentNum(1)
{  
	m_lMaxCacheTime = 60;
	m_lMaxDownTimeFromCDN = 25;
	m_uMaxConnectTime = 5;
	m_uMaxRecvTime = 10;

	m_limitSpeedMode = 0;

	strUploadInitUrl = "https://upload.nicefilm.com/upload/chunk/init";			// 耐飞文件上传init接口

	m_nDownPlayingTaskCount = 0;
	m_bVideoBuffering = false;

	m_uMaxDownTaskCount = -1;
	m_nCurNetType = 0;

	m_strAppVer = LIB_VERSION;		// 默认为下载库当前版本号
}


CfgHost::~CfgHost(void)
{
}


bool CfgHost::SetConfig(const string & strCfg)
{
	bool bRet = false;

	Json::Reader reader;
	Json::Value	root;
	if (reader.parse(strCfg, root) &&
		root.isObject())
	{
		bRet = true;
		if (root.isMember("data") && root["data"].isObject())
		{
			Json::Value data = root["data"];
			if (data.isMember("maxcachetime") && data["maxcachetime"].isInt())
			{
				g_cfg.m_lMaxCacheTime = data["maxcachetime"].asInt();
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CYfNetPlush::SetConfig g_cfg.m_lMaxCacheTime=%d", g_cfg.m_lMaxCacheTime);
#endif
			}
			if (data.isMember("maxplaydowntime") && data["maxplaydowntime"].isInt())
			{
				g_cfg.m_lMaxDownTimeFromCDN = data["maxplaydowntime"].asInt();
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CYfNetPlush::SetConfig g_cfg.m_lMaxDownTimeFromCDN=%d", g_cfg.m_lMaxDownTimeFromCDN);
#endif
			}
			if (data.isMember("maxconnecttime") && data["maxconnecttime"].isIntegral())
			{
				g_cfg.m_uMaxConnectTime = data["maxconnecttime"].asUInt();
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CYfNetPlush::SetConfig g_cfg.m_uMaxConnectTime=%d", g_cfg.m_uMaxConnectTime);
#endif
			}
			if (data.isMember("maxrecvtime") && data["maxrecvtime"].isIntegral())
			{
				g_cfg.m_uMaxRecvTime = data["maxrecvtime"].asUInt();
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CYfNetPlush::SetConfig g_cfg.m_uMaxRecvTime=%d", g_cfg.m_uMaxRecvTime);
#endif
			}
			if (data.isMember("maxdowncount") && data["maxdowncount"].isInt())
			{
				g_cfg.m_uMaxDownTaskCount = data["maxdowncount"].asInt();
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CYfNetPlush::SetConfig g_cfg.m_uMaxDownTaskCount=%d", g_cfg.m_uMaxDownTaskCount);
#endif
			}
			if (data.isMember("concurrentnum") && data["concurrentnum"].isInt())
			{
				g_cfg.m_uConcurrentNum = data["concurrentnum"].asInt();
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CYfNetPlush::SetConfig g_cfg.m_uConcurrentNum=%d", g_cfg.m_uConcurrentNum);
#endif
			}
			if (data.isMember("upconcurrentnum") && data["upconcurrentnum"].isInt())
			{
				g_cfg.m_uUpConcurrentNum = data["upconcurrentnum"].asInt();
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CYfNetPlush::SetConfig g_cfg.m_uConcurrentNum=%d", g_cfg.m_uConcurrentNum);
#endif
			}
			if (data.isMember("app_ver") && data["app_ver"].isString())
			{
				g_cfg.m_strAppVer = data["app_ver"].asString();
			}
			if (data.isMember("uploadiniturl") && data["uploadiniturl"].isString())
			{
				g_cfg.strUploadInitUrl = data["uploadiniturl"].asString();
			}
			if (data.isMember("uploadurl") && data["uploadurl"].isString())
			{
				g_cfg.strUploadUrl = data["uploadurl"].asString();
			}
			if (data.isMember("alisign") && data["alisign"].isObject())		// 阿里云key解析
			{
				Json::Value alisign = data["alisign"];
				Json::Value::Members vMembers = alisign.getMemberNames();
				for (int i=0; i<vMembers.size(); i++)
				{
					if (alisign[vMembers[i]].isString())
					{
						g_cfg.m_mSignKeys[vMembers[i]] = alisign[vMembers[i]].asString();
					}
				}
#ifdef QVODDEBUG
			string str;
			for (std::map<string, string>::iterator itr=g_cfg.m_mSignKeys.begin();itr!=g_cfg.m_mSignKeys.end();itr++)
			{
				str += itr->first;
				str += ":";
				str += itr->second;
				str += "\t";
			}
			_OUTPUT(ERROR1, "CYfNetPlush::SetConfig g_cfg.m_mSignKeys=%s", str.c_str());
#endif
			}
		}
	}
	else
	{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CYfNetPlush::SetConfig failed! json=[%s]", strCfg.c_str());
#endif
	}
	return bRet;
}
