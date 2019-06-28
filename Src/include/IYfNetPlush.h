#ifndef _IYFNETPLUSH_H_
#define _IYFNETPLUSH_H_

#include <string>
#ifdef WIN32
#else
#ifndef MACOSX
#include "YfNetJni.h"		// edit by 魏理坚
#endif
#endif

//错误码的定义
typedef int NET_ERROR;
#define E_NET_TASK_FINISH				 1	//任务已经下载完成
#define E_NET_OK						 0	//成功
#define E_NET_FALSE						-1	//失败
#define E_NET_SEND_ERROR				-2	//发送请求失败
#define E_NET_RECV_ERROR				-3	//接收请求失败
#define E_NET_CON_ERROR					-4	//连接请求失败
#define E_NET_ERROR_PARA				-5	//参数初始化错误或者无效参数
#define E_NET_NO_FILE					-6	//文件不存在
#define E_NET_TIME_OUT					-7	//超时
#define E_NET_NO_DATA					-8	//未下载到数据
#define E_NET_DISK_NOT_ENOUGH			-9	//磁盘空间不足
#define E_NET_CREATE_TASK_FAILED		-10	//创建任务失败
#define E_NET_BUFFER_NOT_ENOUGH			-11	//传递进来的buffer空间大小不够
#define E_NET_NO_TASK_BY_HASH			-12 //没有hash对应的任务
#define E_NET_PATH_NOT_ACCESS			-13	//路径不可访?
#define E_NET_NO_INIT					-14	// 网络模块还未初始化
#define E_NET_BIND_LOCAL_HTTP_PORT_FAIL	-15 // 绑定本地http端口失败
#define E_NET_CREATE_FOLDER_FAIL		-16	//创建目录失败
#define E_NET_AUTH_FAIL					-17 //认证失败，创建任务会失败
#define E_NET_AUTH_EXCEED_CLIENT_NUM	-18 //使用sdk的用户数超额，创建任务会失败
#define E_NET_AUTH_DISABLE				-19 //sdk禁用了，创建任务会失败
#define E_NET_DOMAIN_NOT_IN_WHITELIST	-20 //创建任务的url，域名不在白名单中
#define E_NET_DOWNTASK_QUEUE_FULL			-21	// 当前离线任务数已满，超过设定值		

struct IYfNetPlush;

#define MAX_FILE_PATH_LEN	511

#pragma pack(push)//保存对齐状态
#pragma pack(1)  //设置为1字节对齐
struct STaskInfo
{
	char hash[40];							//任务的hash
	long long iFileLen;						//文件的大小，Byte
	long long iDownloadLen;					//已下载的大小,Byte
	long long iUploadLen;					//已上传的大小,Byte
	unsigned int iDownloadSpeed;			//当前的下载速度，B/s
	unsigned int iUploadSpeed;				//当前的上传速度，B/s
	unsigned char status;					//当前任务的状态，1:下载/上传 2:暂停下载/上传 若任务已下载完，则此状态无意义  3:下载/上传完成
	unsigned char tasktype;					//当前任务的类型，升级任务为9，其他为普通任务
	bool	bDownloadAndCopy;				//当前任务是否设置了“下载”
	char	szCopyPath[MAX_FILE_PATH_LEN];	//若bDownloadAndCopy为true，则该字段不为空，存储下载完的拷贝目的路径，包括文件名
	char	szFilepath[MAX_FILE_PATH_LEN];	//任务的文件路径，包括文件名
	STaskInfo()
	{
		memset(this,0,sizeof(STaskInfo));
	}
};

struct SBitInfo
{
	unsigned int iBlockSize;	//分块大小
	unsigned int iBlockNum;		//分块数
	char szBitField[1];			//分块的下载位图表，变长结构体，长度为 iBlockNum/8 + (iBlockNum%8==0?0:1)
	SBitInfo()
	{
		memset(this,0,sizeof(SBitInfo));
	}
};
struct SCallBackPara
{
	int id;				//事件id
	int result;			//事件的结果或原因
	const char* buf;	//事件产生的数据，可为NULL，UI需要自己拷贝一份
	int buflen;			//数据的长度
	SCallBackPara()
	{
		memset(this,0,sizeof(SCallBackPara));
	}
};
#pragma pack(pop)//恢复对齐状态

//回调函数
typedef void (*EventCallBackFunc)(SCallBackPara& para);
/*
具体回调事件如下:
//////////////////
下载完拷贝文件到下载目录
id:1
result:0，拷贝完成；1、创建目标文件失败；2、读数据失败；3、下载目录空间不足；4、下载目录没有写权限；5、写失败
buf:40字节的hash
buflen：40
/////////////////
临时缓存目录没空间了，请释放空间后重新开始任务
id:2
result:xxx
buf:NULL
buflen:0
////////////////
http下载错误
id:3
result:40X, 50X				具体值参见:http://baike.baidu.com/link?url=i4E82Vpjrh_LYgW2XVPzXi0I0sdICgjCPLr7x-e1hzPARhY3kW2AMP6UFN681vQM8ZrJjU24oKFpinIEw8y-37a4sBT96BMYX7BS6zcS-mmj9c2tyZGr49q6gLwH5T85
400:Bad Request
401:未授权
403:请求被服务器拒绝
404:服务器没找到资源
406:不可接受
407:需要代理身份验证
50X:服务器错误
500:服务器内部错误
501:未实现
502:网关出错
buf:40字节的hash
buflen：40
id:4
////////////////
通知UI下载connect重试次数
id:5
result:times,重试次数
buf:40字节的hash
*/

enum ELimitUploadSpeedMode
{
	LIMIT_UPLOAD_SPEED_DEFAULT = 0,	//默认限速模式
	LIMIT_UPLOAD_SPEED_INTERNET,	//上网模式
	LIMIT_UPLOAD_SPEED_MOVIE,		//观影模式
	LIMIT_UPLOAD_SPEED_GAME			//游戏模式	
};

enum ENetType
{
	NETYPTE_DEFAULT = 0, // 默认未知
	NETTYPE_WIFI = 1,         //  wifi网络
	NETTYPE_4G = 2,			  //  4G移动网络
};

#pragma pack(push)//保存对齐状态
#pragma pack(1)  //设置为1字节对齐
struct InitPara
{
	const char* pTempCachePath;		//播放任务临时缓存目录
	const char* pDownloadPath;		//下载任务存储的路径
	const char* pConfigPath;			//p2p配置文件的路径
	const char*	pAppToken;			//sdk认证的参数
	unsigned int maxspace;		//缓存任务最大可使用空间，单位MB，超过则删除旧任务，0为无限制
	EventCallBackFunc callback;	//回调函数
	bool	bUpgradeProc;		//是否升级程序
	InitPara()
	{
		memset(this,0,sizeof(InitPara));
	}
};
#pragma pack(pop)//恢复对齐状态

#ifdef MACOSX
#define YFNETPLUSHAPI extern "C"
#else
#ifdef YFNETPLUSH_EXPORTS
#define YFNETPLUSHAPI extern "C" __declspec(dllexport) 
#else
#define YFNETPLUSHAPI extern "C" __declspec(dllimport) 
#endif
#endif

#ifdef MACOSX
enum ECurrentScene
{
	SCENE_BACKGROUND = 0,	//切换到后台,IOS会隔约60s后会关闭链接
	SCENE_FOREGROUND				//切换到前台
};
#endif


#ifdef WIN32
#define WINAPI	__stdcall
#else
#define WINAPI
#endif

//初始化网络模块,返回E_NET_OK表示成功,返回其他值表示失败;
//pYfNet [OUT] : 存储创建的对象指针
//para [IN] : 初始化的参数
//lPointer [OUT] : 初始化后的对象指针地址，释放网络模块时需要传入该值
YFNETPLUSHAPI NET_ERROR WINAPI _InitYfNet(IYfNetPlush **pYfNet,InitPara para,long& lPointer);

//释放网络模块,lYfNetID为_InitYfNet的lPointer
YFNETPLUSHAPI void WINAPI _ClearYfNet(long lYfNetID);


typedef NET_ERROR (WINAPI  *PInitYfNet)(IYfNetPlush **pYfNet,InitPara para,long& lPointer);
typedef NET_ERROR (WINAPI  *PClearYfNet)(long lYfNetID);


struct IYfNetPlush
{
	/**************************************************************************************
	GetPrivateVersion		获取内部版本号
	pVersion				[IN/OUT]非空内存，用于存储版本号信息
	***************************************************************************************/
	virtual NET_ERROR GetPrivateVersion(char* pVersion) = 0;

	/**************************************************************************************
	CreateTask			创建播放任务
	pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http下载文件的url，如：
						http://vodipad.wasu.cn/201407021742/3cc51ccdac8e4d8ca896152a95415347/pcsan08/mams/vod/201404/24/20/20140424202708118c8104efd_dc5607f3.mp4?version=P2PPlayer_V.1.5.16
	pstrKeyurl			[IN]  pstrSrcUrl中提取出来的固定的url，如
						http://wasu.cn/20140424202708118c8104efd_dc5607f3.mp4
	pstrReferer			[IN]  引用页，可为NULL
	pstrHash			[OUT] pstrKeyurl对应的p2p hash值,40字节的字符串，创建任务后对任务的操作都以该值作为索引
	pstrHttpProxyUrl	[OUT] 该视频对应的本地http代理地址，把该地址给播放器进行播放，内存大小至少100字节
	bIsOtherCdn			[IN]  是否第三方CDN
	***************************************************************************************/
	virtual NET_ERROR CreateTask(const char *pstrSrcUrl,const char* pstrKeyurl,const char* pstrReferer,char *pstrHash, char *pstrHttpProxyUrl = NULL,bool bIsOtherCdn = false) = 0;

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
	virtual NET_ERROR CreateHlsTask(const char *pstrSrcUrl,const char* pstrKeyurl,bool bIsOtherCdn,char *pstrHash,char* pstrHttpProxyUrl) = 0;

	/**************************************************************************************
	CreateUpdateTask	创建升级任务
	pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http下载文件的url
	pstrHash			[OUT]  任务的p2p hash值，40字节的字符串，创建任务后对任务的操作都以该值作为索引
	***************************************************************************************/
	virtual NET_ERROR CreateUpdateTask(const char *pstrSrcUrl,char* pstrHash) = 0;

	/**************************************************************************************
	CreateUpdateTask	创建上传任务
	nType					[IN]  上传任务的类型 0:默认	1:耐飞文件上传
	pstrSrcUrl			[IN]  上传任务的url
	pstrfilepath		[IN]  上传的文件路径
	pstrHash			[OUT]  任务的p2p hash值，40字节的字符串，创建任务后对任务的操作都以该值作为索引
	***************************************************************************************/
	virtual NET_ERROR CreateUploadTask(int nType, const char *pstrSrcUrl, const char* pstrfilepath, char *pstrHash) = 0;

	/**************************************************************************************
	SetHeadValue	设置任务请求头信息
	pstrHash			[IN]  要设置的任务hash,在调用RunTask前设置
	pstHead				[IN]  请求head头
	pstrValue			[IN]  请求head的值
	***************************************************************************************/
	virtual NET_ERROR SetHeadValue(const char *pstrHash, const char *pstHead, const char* pstrValue) = 0;

	/**************************************************************************************
	SetHeadValue	设置任务请求头信息
	pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http上传文件的url
	pstrHeadAdd	[IN]  请求头head附加信息,如"head1: val1\nhead2: val2"
	pstrValue		[IN]  head值
	***************************************************************************************/
	//virtual NET_ERROR SetHeadValue(const char *pstrHash, const char *pstrHeadAdd) = 0;

	/***************************************************************************************	
	DeleteTask			删除任务
	pstrHash			[IN] 要删除的任务的hash
	***************************************************************************************/
	virtual NET_ERROR DeleteTask(const char *pstrHash) = 0;

	/***************************************************************************************	
	SetTaskDownload		设置任务为下载模式，该模式在任务下载完后会拷贝到指定存储路径
	pstrHash			[IN] 要设置的任务的hash
	bOption				[IN] true or false
	pFilename			[IN] 若bOption为true，则需要设置存储的文件名
	***************************************************************************************/
 	virtual NET_ERROR SetTaskDownload(const char *pstrHash,bool bOption,const char* pFilename) = 0;

	/***************************************************************************************	
	SetTempCachePath	设置任务的临时存储目录
	pstrPath			[IN]  绝对路径值
	***************************************************************************************/
	virtual NET_ERROR SetTempCachePath(const char *pstrPath) = 0;

	/***************************************************************************************	
	SetDefaultDownPath	设置下载任务的存储路径
	pstrPath			[IN]  绝对路径值
	***************************************************************************************/
	virtual NET_ERROR SetDefaultDownPath(const char *pstrPath) = 0;

	/***************************************************************************************	
	PauseTask			暂停任务
	pstrHash			[IN]  要暂停的任务的hash
	bIsAll				[IN]  是否暂停所有任务,如果是,pstrHash = NULL
	***************************************************************************************/
	virtual NET_ERROR PauseTask(const char *pstrHash,bool bIsAll=false) = 0;


	/***************************************************************************************	
	RunTask				运行任务(开始下载)
	pstrHash			[IN]  要运行的任务的hash
	bIsAll				[IN]  是否运行所有任务,如果是,pstrHash = NULL
	***************************************************************************************/
	virtual NET_ERROR RunTask(const char *pstrHash,bool bIsAll=false) = 0;

	/***************************************************************************************	
	SetTaskPlaying			设置任务是否正在播放
	pstrHash			[IN]  要播放的任务的hash
	bPlaying			[IN]  true为播放，false为停止播放
	***************************************************************************************/
	virtual NET_ERROR SetTaskPlaying(const char *pstrHash,bool bPlaying) = 0;

	/***************************************************************************************	
	TaskIsPlaying			任务是否正在播放
	pstrHash			[IN]  要查询的文件hash
	bool					[OUT]	true:正在播放 false:没有播放
	***************************************************************************************/
	virtual NET_ERROR TaskIsPlaying(const char *pstrHash,bool& bPlaying) = 0;

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
	virtual NET_ERROR ReadFile(const char *pstrSrcUrl,const char *pstrHash,char *pbuf,long long nstart,long nlen,long *pnreadlen) = 0;
	
	/***************************************************************************************	
	QueryFileSize		查询文件大小
	pstrHash			[IN]  要查询的文件hash
	iFilesize			[OUT] 存储查询到的文件大小，当YfNet模块未从网络获取到时则iFilesize为0
	***************************************************************************************/
	virtual NET_ERROR QueryFileSize(const char *pstrHash,long long& iFilesize) = 0;

	/***************************************************************************************	
	TaskIsFinished		查询任务是否下载/上传完成（耗时操作，避免频繁调用）
	pstrHash			[IN]  要查询的文件hash
	bool					[OUT]	true:完成 false:未完成
	***************************************************************************************/
	virtual NET_ERROR TaskIsFinished(const char *pstrHash, bool& bFinish) = 0;


	/*****************************************************************************************
	QueryTaskInfo		通过Hash查询任务信息
	pstrHash			[IN]  要查询的文件hash
	pTaskInfo			[OUT] 保存查询到的任务信息,函数不负责空间的申请和释放
	****************************************************************************************/
	virtual NET_ERROR QueryTaskInfo(const char *pstrHash,STaskInfo *pTaskInfo) = 0;

	/***************************************************************************************	
	QueryBitInfo		查询Bit位下载信息，刚创建任务时未获取到文件大小，会返回E_NET_FALSE
	pstrHash			[IN]  要查询的文件hash
	pBuf				[IN]  是输入的缓冲区，以结构体SBitInfo填充
	iBufLen				[IN/OUT] 保存比特位信息的空间大小,当函数返回错误E_NET_BUFFER_NOT_ENOUGH时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度
	***************************************************************************************/
	virtual NET_ERROR QueryTaskBitInfo(const char *pstrHash, char *pBuf,int &iBufLen) = 0;

	/***********************************************************************
	SetSpaceCanUse			设置可用磁盘空间大小，所有文件大小
							不能超过此值，程序启动后设置，0为不限大小
	spacecanuse:			[IN] 单位为MB
	*******************************************************************/
	virtual NET_ERROR SetSpaceCanUse(unsigned int spacecanuse) = 0;

	/***************************************************************************************	
	SetPlayingPos		设置当前的播放位置
	pstrHash			[IN]  要设置的任务hash
	pos					[IN]  要设置的位置，单位为Byte
	***************************************************************************************/
	virtual NET_ERROR SetPlayingPos(const char *pstrHash,long long pos) = 0;
	
	/***************************************************************************************	
	SetPlayingTimepoint		设置当前的播放时间位置
	pstrHash			[IN]  要设置的任务hash
	timepoint			[IN]  要设置的时间位置，单位为ms
	bSeek				[IN]  是否seek引起的播放位置变动
	iCacheTime			[OUT] 保存缓存的时间，单位为ms
	***************************************************************************************/
	virtual NET_ERROR SetPlayingTimepoint(const char *pstrHash,int timepoint,bool bSeek,int& iCacheTime) = 0;

	/***************************************************************************************	
	SetVideoDuration		设置视频的时长
	pstrHash			[IN]  要设置的任务hash
	duration			[IN]  视频的时长，单位秒
	***************************************************************************************/
	virtual NET_ERROR SetVideoDuration(const char *pstrHash,int duration) = 0;
	
	/***************************************************************************************	
	SetLimitUploadSpeedMode		设置上传限速模式
	mode				[IN]  要设置限速模式
	***************************************************************************************/
	virtual NET_ERROR SetLimitUploadSpeedMode(ELimitUploadSpeedMode mode) = 0;
		
	/***************************************************************************************	
	SetLimitUploadSpeedPercentage		设置上传限速百分比
	percentage				[IN]  要设置限速百分比
	***************************************************************************************/
	virtual NET_ERROR SetLimitUploadSpeedPercentage(int percentage) = 0;
			
	/***************************************************************************************	
	GetSampleSpeed			[IN]  获取10分钟内的取样平均速度
	speed					[IN]  存储速度值，单位 Byte/s
	***************************************************************************************/
	virtual NET_ERROR GetSampleSpeed(int& speed) = 0;

	/***************************************************************************************	
	NotifyVideoBuffering		通知视频卡顿了，正在缓冲
	pstrHash			[IN]  要通知的任务hash
	bBuffering 			[IN]  true:正在缓冲  false:缓冲结束
	***************************************************************************************/
	virtual NET_ERROR NotifyVideoBuffering(const char *pstrHash, bool bBuffering) = 0;
	
	/***************************************************************************************	
	CleanCache			删除缓存文件，正在离线的缓存文件不会被删
	***************************************************************************************/
	virtual NET_ERROR CleanCache() = 0;

	/**************************************************************************************
	SetConfig				设置配置信息
	pstrJson					[IN]配置参数,json串,如'{"maxcachetime":40}'
	***************************************************************************************/
	virtual NET_ERROR SetConfig(const char* pstrJson) = 0;

	/**************************************************************************************
	SetLogLevel				设置日志信息输出(Debug库版本才有效)
	nLevel					[IN] 日志级别（大于此级别的日志才输出）[0-8]
	***************************************************************************************/
	virtual NET_ERROR SetLogLevel(int nLevel) = 0;

	/**************************************************************************************
	GetTaskInfo			查询任务当前详细信息
	pstrHash			[IN]  要查询的文件hash
	pBuf				[IN]  是输入的缓冲区，以json格式填充
	iBufLen				[IN/OUT] json大小,当函数返回错误E_NET_BUFFER_NOT_ENOUGH时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度
	***************************************************************************************/
	virtual NET_ERROR GetTaskInfo(const char *pstrHash, char *pBuf,int &iBufLen) = 0;
#ifdef MACOSX
	/***************************************************************************************
	YFSetCurrentScene		设置当前的场景
	scene				[IN] 具体枚举见定义
	***************************************************************************************/
	virtual NET_ERROR SetCurrentScene(ECurrentScene scene) = 0;
#endif
    
    /**************************************************************************************
     UpdateHttpOrgUrl	更新指定任务的源URL
     pstrHash			[IN]  要更新URL的任务hash
     pstrSrcUrl			[IN]  新的任务URL
    ***************************************************************************************/
    virtual NET_ERROR UpdateHttpOrgUrl(const char *pstrHash, const char *pstrSrcUrl) = 0;

    /**************************************************************************************
      GetDownTaskLists				获取所有正在下载缓存的任务
	pBuf					[IN/OUT]  是输入的缓冲区，以json格式填充
	iBufLen				[IN/OUT] json大小,当函数返回错误E_NET_BUFFER_NOT_ENOUGH时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度
    ***************************************************************************************/
    virtual NET_ERROR GetDownloadingTaskLists(char *pBuf,int &iBufLen) = 0;

    /**************************************************************************************
     UpdateClarityUrl	更新指定任务的源URL,更新清晰度调用此接口(IOS硬解不支持此接口)
     pstrHash			[IN]  要更新URL的任务hash
     pstrSrcUrl			[IN]  新的清晰度URL
	 pstrKey			[IN]  pstrSrcUrl中提取出来的固定的url
	 nClarity	        [IN] 要切换的清晰度[0:标清 1:高清  2:超清  3:蓝光]
     ***************************************************************************************/
    virtual NET_ERROR UpdateClarityUrl(const char *pstrHash, const char *pstrSrcUrl, const char *pstrKey, int nClarity) = 0;

	 /**************************************************************************************
     UpdateNetType	更新网络类型(网络状态变化后调用)
     ENetType			[IN]  变化后的网络类型[0:移动网络 1:wifi]
     ***************************************************************************************/
	virtual NET_ERROR UpdateNetType(const ENetType& nNetType) = 0;
};
#endif
