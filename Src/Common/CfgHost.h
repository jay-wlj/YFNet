#pragma once
#include "qvod_string.h"
#include <map>
#include <string>
using std::map;
using std::string;

class CfgHost
{
public:
	CfgHost(void);
	~CfgHost(void);

	bool SetConfig(const string & strCfg);
public:
	int m_lMaxCacheTime;									// 最大预下载时长，默认:60s
	int m_lMaxDownTimeFromCDN;								// 从cdn一次http请求最大的数据时长，默认:25s
	unsigned int m_uMaxConnectTime;							// CDN请求socket connect的超时时间，默认:5s
	unsigned int m_uMaxRecvTime;							// socket recv 间隔超时时间，默认:10s:
	int m_limitSpeedMode;									// 限速模式,0:不限速 1:上网自动模式 2:观影模式 3:限制传输
	unsigned int m_uMaxDownTaskCount;						// 同时最大离线任务数，默认:-1 不限制

	unsigned int m_uConcurrentNum;							// 离线任务最大并发下载数
	unsigned int m_uUpConcurrentNum;						// 上传任务最大并发数

	int	m_nDownPlayingTaskCount;							// 当前下载播放的任务数
	bool	m_bVideoBuffering;								// 当前播放视频是否正在缓冲

	int m_nCurNetType;											// 当前网络类型，参考ENetType
	string m_strAppVer;											// 当前app版本号

	// 耐飞文件上传
	std::string strUploadInitUrl;							// 文件上传init检测接口
	std::string strUploadUrl;								// 文件上传upload接口
	std::map<string, string>	m_mSignKeys;				// 耐飞ts文件签名key桶
	//bool	m_bUseCacheMem;									// 下载数据是否缓存到内存(仅缓存当前播放任务)
};

extern CfgHost g_cfg;