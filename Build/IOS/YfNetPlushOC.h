//
//  YfNetPlush.h
//  YfNetPlush
//
//  Created by 陈志标 on 2016/10/13.
//  Copyright © 2016年 chenzhibiao. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, YFNET_ERROR)
{
    YF_NET_OK = 0,                       //成功
    YF_NET_TASK_FINISH = 1,                  //任务已经完成
    YF_NET_FALSE = -1,                        //失败
    YF_NET_SEND_ERROR =-2,                   //发送请求失败
    YF_NET_RECV_ERROR =-3,                   //接收请求失败
    YF_NET_CON_ERROR =-4,                    //连接请求失败
    YF_NET_ERROR_PARA = -5,                   //参数初始化错误或者无效参数
    YF_NET_NO_FILE = -6,                      //文件不存在
    YF_NET_TIME_OUT = -7,                     //超时
    YF_NET_NO_DATA = -8,                      //未下载到数据
    YF_NET_DISK_NOT_ENOUGH = -9,              //磁盘空间不足
    YF_NET_CREATE_TASK_FAILED = -10,           //创建任务失败
    YF_NET_BUFFER_NOT_ENOUGH = -11,            //传递进来的buffer空间大小不够
    YF_NET_NO_TASK_BY_HASH = -12,              //没有hash对应的任务
    YF_NET_PATH_NOT_ACCESS = -13,              //路径不可访问
    YF_NET_NO_INIT = -14,                      //网络模块还未初始化完
    YF_NET_BIND_LOCAL_HTTP_PORT_FAIL = -15,    //绑定本地http代理端口失败
    YF_NET_CREATE_FOLDER_FAIL = -16,           //创建目录失败
    YF_NET_DOWNTASK_QUEUE_FULL = -21           //当前离线任务数队列已满，超过设定值（需暂停当前任务才能RunTask其它任务）
};

typedef NS_ENUM(NSInteger, YFNET_SCENE)
{
    YFSCENE_BACKGROUND = 0,			//切换到后台
    YFSCENE_FOREGROUND              //切换到前台
};
typedef NS_ENUM(NSInteger, YFNET_NETYPE)
{
    YFNETYPE_DEFALT = 0,			//默认未知
    YFNETYPE_WIFI=1,					//wifi网络
	YFNETYPE_4G=2,						//4G移动网络
};

#pragma pack(push)
#pragma pack(1)     // 设置位1字节对齐


struct YFSTaskInfo
{
    char hash[40];								//任务的hash
    long long  filelen;							//文件的大小，Byte
    long long   downloadlen;						//已下载的大小,Byte
    long long  p2pdownlen;						//p2p方式下载的数据量，Byte
    long long  httpdownlen;						//http方式下载的数据量，Byte
    long long  uploadlen;						//上传的大小，Byte
    unsigned int downloadspeed;					//下载速度，字节/秒
    unsigned int uploadspeed;					//上传速度，字节/秒
    unsigned char status;						//当前任务的状态，1:下载/上传 2:暂停下载/上传，若任务已下载完，则此状态无意义 3:下载/上传完成
    char filepath[256];							//任务的文件路径，包括文件名
};
#pragma pack(pop)

//回调函数的参数
struct YFSCallBackPara
{
    int id;				//事件id
    int result;			//事件的结果或原因
    const char* buf;	//事件产生的数据，可为NULL，UI需要自己拷贝一份
    int buflen;			//数据的长度
};
/*
 具体回调事件如下:
 //////////////////
 任务已下载缓存完成
 id:0
 result:xxx
 buf:40字节的hash
 //////////////////
 下载完拷贝文件到下载目录
 id:1
 result:0，拷贝完成；1、创建目标文件失败；2、读数据失败；3、下载目录空间不足；4、下载目录没有写权限；5、写失败
 buf:40字节的hash
 /////////////////
 临时缓存目录没空间了，网络模块会暂停该任务，请释放空间后重新开始任务
 id:2
 result:xxx
 buf:NULL
 ////////////////
 http下载错误，链接失效了，网络模块会暂停该任务
 id:3
 result:xxx
 buf:40字节的hash
 ////////////////
 通知UI本地代理端口号
 id:4
 result:端口号
 buf:NULL
////////////////
通知UI下载connect重试次数
id:5
result:times,重试次数
buf:40字节的hash
 */

//定义协议
@protocol YunFanNetDelegate

//默认@required
-(void)callback: (struct YFSCallBackPara*) pCallBackPara;

@end

@interface YfNetPlushOC : NSObject


-(YFNET_ERROR) InitYunFanNet: (NSString*)pConfigPath pCachePath: (NSString*)pCachePath maxspace: (int)maxspace delegate: (id) delegate;

-(void)ClearYfNet;

/**************************************************************************************
	YFGetPrivateVersion		获取内部版本号
	pVersion				[IN/OUT]非空内存，用于存储版本号信息
	***************************************************************************************/

-(YFNET_ERROR) YFGetPrivateVersion: (NSString**)pVersion;


/**************************************************************************************
	SetConfig				设置配置信息
	pstrJson					[IN]配置参数,json串,如'{"data":{"maxcachetime":40}}'
	***************************************************************************************/
-(YFNET_ERROR) YFSetConfig: (NSString*)pstrJson;

/**************************************************************************************
	YFCreateTask			创建播放任务
	pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http下载文件的url，如：
 http://pchlsws4.imgo.tv/6a42314c0847986756a8775148803a78/54ffa651/c1/2012/xinwen/xinwendashijian/2015030924bdaf00-641c-4e14-8ae0-57055bb8b96a.fhv/playlist.m3u8
	pstrKeyurl			[IN]  pstrSrcUrl中提取出来的固定的url，视频唯一标识，如
 http://hunantv.com/c1/2012/xinwen/xinwendashijian/2015030924bdaf00-641c-4e14-8ae0-57055bb8b96a.fhv/playlist.m3u8
	pstrReferer			[IN]  无特殊要求可填""
	pstrHash			[OUT] pstrKeyurl对应的p2p hash值,40字节的字符串，创建任务后对任务的操作都以该值作为索引
	***************************************************************************************/
-(YFNET_ERROR) YFCreateTask: (NSString*)pstrSrcUrl pstrKeyurl:(NSString*)pstrKeyurl pstrReferer:(NSString*) pstrReferer pstrHash:(NSString**)pstrHash playURL:(NSString**)playurl;

/**************************************************************************************
	CreateUpdateTask	创建上传任务
	nType				[IN]  上传任务的类型 0:默认	1:耐飞文件上传
	pstrSrcUrl			[IN]  上传任务的url
	pstrfilepath		[IN]  上传的文件路径
	pstrHash			[OUT]  任务的p2p hash值，40字节的字符串，创建任务后对任务的操作都以该值作为索引
	***************************************************************************************/
-(YFNET_ERROR) YFCreateUploadTask: (int)type pstrSrcUrl:(NSString*)pstrSrcUrl pstrFilePath:(NSString*)pstrFilePath pstrHash:(NSString**)pstrHash;

/**************************************************************************************
	SetHeadValue	设置任务请求头信息
	pstrHash			[IN]  要设置的任务hash,在调用RunTask前设置
	pstHead				[IN]  请求head头
	pstrValue			[IN]  请求head的值
	***************************************************************************************/
-(YFNET_ERROR) YFSetHeadValue: (NSString*)pstrHash pstrHead:(NSString*)pstrHead pstrValue:(NSString*)pstrValue;

/***************************************************************************************
	YFDeleteTask			删除任务
	pstrHash,			[IN] 要删除的任务的hash,40位
	***************************************************************************************/
-(YFNET_ERROR) YFDeleteTask: (NSString*)pstrHash;

/***************************************************************************************
	YFPauseTask			暂停任务
	pstrHash,			[IN]  要暂停的任务的hash,40位
	***************************************************************************************/
-(YFNET_ERROR) YFPauseTask: (NSString*) pstrHash;

/***************************************************************************************
	YFRunTask				运行任务(开始下载)
	pstrHash			[IN]  要运行的任务的hash,40位
	***************************************************************************************/
-(YFNET_ERROR) YFRunTask: (NSString*) pstrHash;

/***************************************************************************************
	YFSetTaskPlaying		设置任务是否正在播放
	pstrHash			[IN]  要播放的任务的hash
	bPlaying			[IN]  true为播放，false为停止播放
	***************************************************************************************/
-(YFNET_ERROR) YFSetTaskPlaying: (NSString*)pstrHash bPlaying: (BOOL)bPlaying;


/***************************************************************************************
	YFTaskIsPlaying		查询任务是否正在播放
	pstrHash			[IN]  要设置的任务hash
	bFinish             [OUT] true:完成 false:未完成
	***************************************************************************************/

-(YFNET_ERROR) YFTaskIsPlaying: (NSString*) pstrHash bPlaying:(BOOL*)bPlaying;

/***************************************************************************************
	YFNotifyVideoBuffering	设置任务是否正在缓冲
	pstrHash			[IN]  要播放的任务的hash
	bBuffering			[IN]  true为正在缓冲，false为结束缓冲
	***************************************************************************************/
-(YFNET_ERROR) YFNotifyVideoBuffering: (NSString*)pstrHash bBuffering: (BOOL)bBuffering;

/***************************************************************************************
	YFSetTaskDownload		设置任务是否下载任务
	pstrHash			[IN]  要播放的任务的hash
	bPlaying			[IN]  true为下载，false为取消下载
	***************************************************************************************/
-(YFNET_ERROR) YFSetTaskDownload: (NSString*)pstrHash bDownload: (BOOL)bDownload;


/***************************************************************************************
	YFUpdateHttpOrgUrl		更新任务的URL
	pstrHash			[IN]  任务的hash
	pstrSrcUrl			[IN]  新的播放URL
	***************************************************************************************/
-(YFNET_ERROR) YFUpdateHttpOrgUrl: (NSString*)pstrHash pstrSrcUrl:(NSString*)pstrSrcUrl;

/***************************************************************************************
	YFQueryTaskByHash		获取任务信息
	pstrHash			[IN]  任务的hash
	YFSTaskInfo＊       [OUT]  任务信息
	***************************************************************************************/
-(YFNET_ERROR) YFQueryTaskByHash: (NSString*)pstrHash TaskInfo: (struct YFSTaskInfo*)pTaskInfo;
/***************************************************************************************
	YFSetVideoDuration		设置视频的时长
	pstrHash			[IN]  要设置的任务hash
	duration			[IN]  视频的时长，单位秒
	***************************************************************************************/
-(YFNET_ERROR) YFSetVideoDuration: (NSString*)pstrHash duration: (int)duration;

/***************************************************************************************
	SetPlayingTimepoint		设置当前播放位置并获取其缓存时长
	pstrHash			[IN]  要设置的任务hash
	timepoint			[IN]  要设置的当前时间位置，单位为ms
	bSeek				[IN]  是否seek引起的播放位置变动
	iCacheTime			[OUT] timepoint位置后的缓存时长，单位为ms
	***************************************************************************************/
-(YFNET_ERROR) YFSetPlayingTimepoint: (NSString*) pstrHash timepoint:(int)timepoint bSeek:(BOOL)bSeek iCacheTime:(int*)iCacheTime;

/***************************************************************************************
	YFTaskIsFinished		查询任务是否下载/上传完成
	pstrHash			[IN]  要设置的任务hash
	bFinish             [OUT] true:完成 false:未完成
	***************************************************************************************/

-(YFNET_ERROR) YFTaskIsFinished: (NSString*) pstrHash bFinish:(BOOL*)bFinish;
/***************************************************************************************
	YFSetUploadStatus		是否允许p2p上传
	enableUpload		[IN] true:允许上传；false:禁止上传
	***************************************************************************************/
-(YFNET_ERROR) YFSetUploadStatus: (BOOL)enableUpload;

/***************************************************************************************
	YFCleanCache			删除缓存文件，正在离线的缓存文件不会被删
	***************************************************************************************/
-(YFNET_ERROR) YFCleanCache;

/***************************************************************************************
	YFSetCurrentScene		设置当前的场景
	scene				[IN] 具体枚举见定义
	***************************************************************************************/
-(YFNET_ERROR) YFSetCurrentScene: (YFNET_SCENE)scene;

/***************************************************************************************
	YFSetCurrentScene		设置日志信息输出(Debug库版本才有效)
	nLevel				[IN] 日志级别（大于此级别的日志才输出）[0-8]
	***************************************************************************************/
-(YFNET_ERROR) YFSetLogLevel: (int)nLevel;

/**************************************************************************************
	GetTaskInfo			查询任务当前详细信息
	pstrHash			[IN]  要查询的文件hash
	pstrBuf			[IN/OUT]  是输入的缓冲区，以json格式填充
	***************************************************************************************/
-(YFNET_ERROR) YFGetTaskInfo:(NSString*) pstrHash pstrBuf:(NSString**)pstrBuf;

/**************************************************************************************
 YFGetDownloadingTaskLists      获取所有正在下载缓存的任务
	pstrBuf			[IN/OUT]  是输入的缓冲区，以json格式填充
 ***************************************************************************************/
-(YFNET_ERROR) YFGetDownloadingTaskLists:(NSString**) pstrBuf;

/**************************************************************************************
 YFUpdateNetType      更新网络类型(网络状态变化后调用)
	 YFNET_NETYPE			[IN]  变化后的网络类型
 ***************************************************************************************/
-(YFNET_ERROR) YFUpdateNetType:(YFNET_NETYPE) nettype;
@end
