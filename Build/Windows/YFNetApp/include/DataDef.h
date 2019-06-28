#pragma once
#include <vector>
#include "Media.h"
#define ETASKTYPE
enum e_TaskType
{
	tt_qvod,					//标准qvod任务
	tt_bt,						//bt任务
	tt_qlive,					//直播任务
	tt_http,					//http任务
	tt_swf,						//swf任务
	tt_series,					//剧集任务
	tt_yunfan,					//云帆http资源
	tt_web_media,				//媒体库网页任务（优酷，乐视，爱奇艺等）
	tt_web_list,				//列表中的网页任务
};

enum CreateTaskSource
{
	kTaskFromNull				= 0,
	kTaskFromRadar				= 1, // 雷达
	kTaskFromPrivate			= 2, // 私有云 
	kTaskFromInsert				= 3, // 插件播放
	kTaskFromOnlineMedia		= 4, // 在线视频
	kTaskFromTuitui				= 5, // 推推
	kTaskFromBarrage			= 6, // 弹幕
	kTaskFromSeries				= 7, // 关联剧集
	kTaskFromQRcode				= 8, // 二维码
	kTaskFromSeed				= 9, // 种子文件
	kTaskFromUrl				= 10,// 命令行参数（含URL/hash创建的任务）
};

typedef struct _TASKITEM
{
	long	nTaskID;			// 任务的ID号 进程内有效
	char	szHash[41];			// 网络任务hash
	WORD	nTagID;				// 网络任务的标签id
	BOOL	bRealHash;			// 是否由QLiveFsOpen接口返回hash值

	// TODO: 直播和流任务，terminal重启后需要重新创建
	e_TaskType	nType;				// 网络任务类型 0:普通P2P任务 1: BT任务 2: 直播 3: ftp,http,mms流 4:共享了的文件 6:swf任务
	BOOL	bShare;				// 是否是共享任务
	BOOL	bNoSave;			// 是否为防下载
	BOOL	bFold;				// 任务在列表中是否处于收拢状态
	BOOL	bPrivate;			// 是否为私人文件
	BOOL    bIsPCloud;			// 私有云任务 
	long	nFileCount;			// 文件总数
	__int64 nTotalSize;			// 任务数据总大小
	__int64 nDownloadSize;		// 已下载大小
	netstate netState;			// 网络状态 （1:创建 2:接收 3:暂停 4:完成 5:等待)
	long	nDownrate;			// 下载速度 字节
	long	nUprate;			// 上传速度 字节
	long	nBitLen;			// 比特位长度 缓冲区大小
	long    lLastPlayIndex;		// 最后一次播的文件index
	int		nSeriesRelatedId;	// 所属剧集ID
	BOOL	bInsertOwner;		// 任务来源,FALSE:否，TRUE,是
	__int64 nLastRecieveTime;		//最近一次接收的时间
	//==============================================================================
	//============================== 高速通道的数据  ================================
	long	nAccDownsize;		// (累计下载长度),该任务通过高速通道服务器下载的数据长度,单位KB
	long    nCurAccDownsize;	// (当前下载长度),从高速通道服务器下载的数据长度,KB
	long	nAccDownrate;		// 该任务从高速通道服务器下载的速度,单位字节/秒(B/S)
	BOOL	bOpenHighSpeed;		// 是否启动高速通道
	int		nSpeedUsableState;	// 高速通道可用状态 值为speed_usable_state
	CreateTaskSource	source;// 任务来源

	//net_quality netQuality;	// 网络质量 只需临时计算
	std::vector<Media*> vpFile;	// 任务文件信息队列，只保存可识别后缀的文件
	CString strName;			// 任务显示名
	CString strUrl;				// 任务URL
	CString strWebTaskSeriesClassID;
	CString strWebTaskSeriesDocID;
	CString docID;
	DWORD dwWebTaskSeriesNum;
	_TASKITEM()
	{
		ZeroMemory(this, sizeof(TASKITEM) - sizeof(std::vector<Media*>) - sizeof(CString) * 2);
		bFold = TRUE;
		netState = ns_noncreate;
	}
}TASKITEM; // 网络任务项

typedef struct _TASKITEM_SERIES : public TASKITEM
{
	_TASKITEM_SERIES()
	{
		nSeriesId = 0;
	}
	long   nSeriesId;			//剧集ID
}TASKITEM_SERIES;


typedef struct _DLLTASKINFO
{
	TCHAR			szSrcUrl[MAX_PATH*4];
	TCHAR			bstr_url[MAX_PATH];
	char			szHash[40];
	REFERENCE_TIME	lDur;
	long			lRate;
	long			lWidth;
	long			lHeight;
	DWORD			dwBufferTime;
}DLLTASKINFO;

typedef struct s_PLAY_DATA
{
	BOOL			bMute;
	int				nScale;
	int				nVolume;
	int				nSoundChannel;
	int				nStreamID; //音频流菜单ID
	BOOL			bVideoEnhance; //画质增强
}PLAY_DATA;

enum OpenURL_type
{
	openURL_default = 0,
	openURL_localfile,
	openURL_qlive,
	openURL_DVD,
	openURL_list,
	openURL_netfile
};

typedef struct _TOPLAYER
{
	TCHAR			szURL[MAX_PATH];
	char			szHash[40];
	TCHAR			szSrcUrl[MAX_PATH];
	REFERENCE_TIME	lCtime;
	OpenURL_type	type;		// 0,插件 1,播放器 2,直播
	PLAY_DATA		playdata;
	TCHAR			szImgURL[MAX_PATH]; //图片的url
}TOPLAYER;

typedef struct _WEB_PLAYER_QMVDATA
{
	char			type;					// 数据包种类、用途
	int				index;					// 序号
	char			szHash[40];				// 播放hash
	TCHAR			szFName[_MAX_PATH];		// 本地媒体文件名称
	TCHAR			szLPath[_MAX_PATH];		// 本地媒体路径
	TCHAR			szNPath[_MAX_PATH];		// 网络播放种子链接
	WORD			qmvTagID;				// 视频标签id
	TCHAR			szTitle[_MAX_PATH];		// 标题
	TCHAR			szDetail[_MAX_PATH];	// 描述
	TCHAR			szUrl[1024];			// 链接
	__int64			llTagBegin;				// 标签起始“播放时间”
	__int64			llTagEnd;				// 标签结束“播放时间”
	int				imgLen;
	char			imgBuf[1];
}WEB_PLAYER_QMVDATA;

struct YunfanHttpHeader
{
	std::map<CString,CString> fields_; 
};
struct YunfanSourceInfo
{
	CString sourceWebName_;
	CString mediaTitle_;
	CString mediaSource_;
	CString dragType_;
	DWORD mediaTime_;
	__int64 mediaSize_; 
};

struct YunfanMediaInfo
{
	CString mainUrl_;
	CString mediaDur_;
	CString mediaSize_;
};
struct YunfanParseInfo
{
	YunfanHttpHeader httpHeader_;
	YunfanSourceInfo source_;
	vector<YunfanMediaInfo> medias_;
};