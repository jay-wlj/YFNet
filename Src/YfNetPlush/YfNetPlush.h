#pragma once

#include "IYfNetPlush.h"
#include "qvod_string.h"


class CYfNetPlush : public IYfNetPlush
{
public:
    CYfNetPlush(InitPara & para);
    ~CYfNetPlush();

public:
	/**************************************************************************************
	GetPrivateVersion		获取内部版本号
	pVersion				[IN/OUT]非空内存，用于存储版本号信息
	***************************************************************************************/
	NET_ERROR GetPrivateVersion(char* pVersion);

	/**************************************************************************************
	SetConfig				设置配置信息
	pstrJson					[IN]配置参数,json串,如'{"maxcachetime":40}'
	***************************************************************************************/
	NET_ERROR SetConfig(const char* pstrJson);

	NET_ERROR SetLogLevel(int nLevel);
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
	NET_ERROR CreateTask(const char *pstrSrcUrl,const char* pstrKeyurl,const char* pstrReferer,char *pstrHash,char *pstrHttpProxyUrl = NULL,bool bIsOtherCdn = false);

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
	NET_ERROR CreateHlsTask(const char *pstrSrcUrl,const char* pstrKeyurl,bool bIsOtherCdn,char *pstrHash,char* pstrHttpProxyUrl);

	/**************************************************************************************
	CreateUpdateTask	创建升级任务
	pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http下载文件的url
	pstrHash			[OUT]  任务的p2p hash值，40字节的字符串，创建任务后对任务的操作都以该值作为索引
	***************************************************************************************/
	NET_ERROR CreateUpdateTask(const char *pstrSrcUrl,char* pstrHash);

	/**************************************************************************************
	CreateUpdateTask	创建上传任务
	nType					[IN]  上传任务的类型 0:默认	1:耐飞文件上传
	pstrSrcUrl			[IN]  上传任务的url
	pstrfilepath		[IN]  上传的文件路径
	pstrHash			[OUT]  任务的p2p hash值，40字节的字符串，创建任务后对任务的操作都以该值作为索引
	***************************************************************************************/
	NET_ERROR CreateUploadTask(int nType, const char *pstrSrcUrl, const char* pstrfilepath, char *pstrHash);

	NET_ERROR SetHeadValue(const char *pstrHash, const char *pstrHeadAdd);
	NET_ERROR SetHeadValue(const char *pstrHash, const char *pstHead, const char* pstrValue);
	/***************************************************************************************	
	DeleteTask			删除任务
	pstrHash			[IN] 要删除的任务的hash
	***************************************************************************************/
	NET_ERROR DeleteTask(const char *pstrHash);

	/***************************************************************************************	
	SetTaskDownload		设置任务为下载模式，该模式在任务下载完后会拷贝到指定存储路径
	pstrHash			[IN] 要设置的任务的hash
	bOption				[IN] true or false
	pFilename			[IN] 若bOption为true，则需要设置存储的文件名
	***************************************************************************************/
 	NET_ERROR SetTaskDownload(const char *pstrHash,bool bOption,const char* pFilename);

	/***************************************************************************************	
	SetTempCachePath	设置任务的临时存储目录
	pstrPath			[IN]  绝对路径值
	***************************************************************************************/
	NET_ERROR SetTempCachePath(const char *pstrPath);

	/***************************************************************************************	
	SetDefaultDownPath	设置下载任务的存储路径
	pstrPath			[IN]  绝对路径值
	***************************************************************************************/
	NET_ERROR SetDefaultDownPath(const char *pstrPath);

	/***************************************************************************************	
	PauseTask			暂停任务
	pstrHash			[IN]  要暂停的任务的hash
	bIsAll				[IN]  是否暂停所有任务,如果是,pstrHash = NULL
	***************************************************************************************/
	NET_ERROR PauseTask(const char *pstrHash,bool bIsAll=false);


	/***************************************************************************************	
	RunTask				运行任务(开始下载)
	pstrHash			[IN]  要运行的任务的hash
	bIsAll				[IN]  是否运行所有任务,如果是,pstrHash = NULL
	***************************************************************************************/
	NET_ERROR RunTask(const char *pstrHash,bool bIsAll=false);

	/***************************************************************************************	
	PlayTask			设置任务是否正在播放
	pstrHash			[IN]  要播放的任务的hash
	bPlaying			[IN]  true为播放，false为停止播放
	***************************************************************************************/
	NET_ERROR CleanCache();
	NET_ERROR SetTaskPlaying(const char *pstrHash,bool bPlaying);
	NET_ERROR TaskIsPlaying(const char *pstrHash,bool& bPlaying);
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
	NET_ERROR ReadFile(const char *pstrSrcUrl,const char *pstrHash,char *pbuf,long long nstart,long nlen,long *pnreadlen);
	
	/***************************************************************************************	
	QueryFileSize		查询文件大小
	pstrHash			[IN]  要查询的文件hash
	iFilesize			[OUT] 存储查询到的文件大小，当YfNet模块未从网络获取到时则iFilesize为0
	***************************************************************************************/
	NET_ERROR QueryFileSize(const char *pstrHash,long long& iFilesize);

	/***************************************************************************************	
	TaskIsFinished		查询任务是否下载/上传完成
	pstrHash			[IN]  要查询的文件hash
	bool				[OUT]	true:完成 false:未完成
	***************************************************************************************/
	virtual NET_ERROR TaskIsFinished(const char *pstrHash, bool& bFinish) ;

	/*****************************************************************************************
	QueryTaskInfo		通过Hash查询任务信息
	pstrHash			[IN]  要查询的文件hash
	pTaskInfo			[OUT] 保存查询到的任务信息,函数不负责空间的申请和释放
	****************************************************************************************/
	NET_ERROR QueryTaskInfo(const char *pstrHash,STaskInfo *pTaskInfo);

	/***************************************************************************************	
	QueryBitInfo		查询Bit位下载信息，刚创建任务时未获取到文件大小，会返回E_NET_FALSE
	pstrHash			[IN]  要查询的文件hash
	pBuf				[IN]  是输入的缓冲区，以结构体SBitInfo填充
	iBufLen				[IN/OUT] 保存比特位信息的空间大小,当函数返回错误E_NET_BUFFER_NOT_ENOUGH时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度
	***************************************************************************************/
	NET_ERROR QueryTaskBitInfo(const char *pstrHash, char *pBuf,int &iBufLen);

	/***********************************************************************
	SetSpaceCanUse			设置可用磁盘空间大小，所有文件大小
							不能超过此值，程序启动后设置，0为不限大小
	spacecanuse:			[IN] 单位为MB
	*******************************************************************/
	NET_ERROR SetSpaceCanUse(unsigned int spacecanuse);

	/***************************************************************************************	
	SetPlayingPos		设置当前的播放位置
	pstrHash			[IN]  要设置的任务hash
	pos					[IN]  要设置的位置，单位为Byte
	***************************************************************************************/
	NET_ERROR SetPlayingPos(const char *pstrHash,long long pos);
	
	/***************************************************************************************	
	SetPlayingTimepoint		设置当前的播放时间位置
	pstrHash			[IN]  要设置的任务hash
	timepoint			[IN]  要设置的时间位置，单位为ms
	bSeek				[IN]  是否seek引起的播放位置变动
	iCacheTime			[OUT] 保存缓存的时间，单位为ms
	***************************************************************************************/
	NET_ERROR SetPlayingTimepoint(const char *pstrHash,int timepoint,bool bSeek,int& iCacheTime);

	/***************************************************************************************	
	SetVideoDuration		设置视频的时长
	pstrHash			[IN]  要设置的任务hash
	duration			[IN]  视频的时长，单位秒
	***************************************************************************************/
	NET_ERROR SetVideoDuration(const char *pstrHash,int duration);
	
	/***************************************************************************************	
	SetLimitUploadSpeedMode		设置上传限速模式
	mode				[IN]  要设置限速模式
	***************************************************************************************/
	NET_ERROR SetLimitUploadSpeedMode(ELimitUploadSpeedMode mode);
		
	/***************************************************************************************	
	SetLimitUploadSpeedPercentage		设置上传限速百分比
	percentage				[IN]  要设置限速百分比
	***************************************************************************************/
	NET_ERROR SetLimitUploadSpeedPercentage(int percentage);
			
	/***************************************************************************************	
	GetSampleSpeed			[IN]  获取10分钟内的取样平均速度
	speed					[IN]  存储速度值，单位 Byte/s
	***************************************************************************************/
	NET_ERROR GetSampleSpeed(int& speed);

	/***************************************************************************************	
	NotifyVideoBuffering		通知视频卡顿了，正在缓冲
	pstrHash			[IN]  要通知的任务hash
	bBuffering 		[IN]  true:正在缓冲  false:缓冲结束
	***************************************************************************************/
	NET_ERROR NotifyVideoBuffering(const char *pstrHash, bool bBuffering);

	NET_ERROR GetTaskInfo(const char *pstrHash, char *pBuf,int &iBufLen);
#ifdef MACOSX
	/***************************************************************************************
	YFSetCurrentScene		设置当前的场景
	scene				[IN] 具体枚举见定义
	***************************************************************************************/
	NET_ERROR SetCurrentScene(ECurrentScene scene);
#endif
    
    virtual NET_ERROR UpdateHttpOrgUrl(const char *pstrHash, const char *pstrSrcUrl);

	virtual NET_ERROR GetDownloadingTaskLists(char *pBuf,int &iBufLen);
    

    virtual NET_ERROR UpdateClarityUrl(const char *pstrHash, const char *pstrSrcUrl, const char *pstrKey, int nClarity);

    virtual NET_ERROR UpdateNetType(const ENetType& nNetType);
public:

#ifndef WIN32
    friend NET_ERROR _InitYfNet(IYfNetPlush **pYfNet,InitPara para,long& lPointer);
    friend void _ClearYfNet(long lQvodNetID);
#endif

private:
	InitPara			  m_InitPara;
};
