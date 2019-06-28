/*
 * @project :MangoTv
 * @author  :Jijia.Deng 
 * @date    :2015-1-8
 */
package com.yunfan.net;

/**
 *
 */
public class Yfnet {
	
	static{
		System.loadLibrary("YfNetPlush");
	}
	
	public Yfnet(){
		
	}

	// 错误码的定义
	/**
	 * 任务已经完成
	 */
	public static final int E_NET_TASK_FINISH = 1;	
	/**
	 * 成功
	 */
	public static final int E_NET_OK = 0;
	/**
	 * 失败
	 */
	public static final int E_NET_FALSE = -1;
	/**
	 * 发送请求失败
	 */
	public static final int E_NET_SEND_ERROR = -2;
	/**
	 * 接收请求失败
	 */
	public static final int E_NET_RECV_ERROR = -3;
	/**
	 * 连接请求失败
	 */
	public static final int E_NET_CON_ERROR = -4;
	/**
	 * 参数初始化错误或者无效参数
	 */
	public static final int E_NET_ERROR_PARA = -5;
	/**
	 * 文件不存在
	 */
	public static final int E_NET_NO_FILE = -6;
	/**
	 * 超时
	 */
	public static final int E_NET_TIME_OUT = -7;
	/**
	 * 未下载到数据
	 */
	public static final int E_NET_NO_DATA = -8;
	/**
	 * 磁盘空间不足
	 */
	public static final int E_NET_DISK_NOT_ENOUGH = -9;
	/**
	 * 创建任务失败
	 */
	public static final int E_NET_CREATE_TASK_FAILED = -10;
	/**
	 * 传递进来的buffer空间大小不够
	 */
	public static final int E_NET_BUFFER_NOT_ENOUGH = -11;
	/**
	 * 没有hash对应的任务
	 */
	public static final int E_NET_NO_TASK_BY_HASH = -12;
	/**
	 * 路径不可访问
	 */
	public static final int E_NET_PATH_NOT_ACCESS = -13;
	/**
	 * 网络模块还未初始化完
	 */
	public static final int E_NET_NO_INIT = -14;
	/**
	 * 获取java回调函数失败
	 */
	public static final int E_NET_GET_JAVA_METHOD_FAIL = -15;
	/**
	 * 获取jvm失败
	 */
	public static final int E_NET_GET_JVM_FAIL = -16;
	/**
	 * 绑定本地http代理端口失败
	 */
	public static final int E_NET_BIND_LOCAL_HTTP_PORT_FAIL = -17;
	
	/**
	 * 当前离线任务数已满，超过设定值
	 */
	public static final int E_NET_DOWNTASK_QUEUE_FULL = -21;
	
	/**
	 * 初始化云帆网络模块，程序运行的生命周期内只需要初始化一次，只有初始化成功后才能调用下面的 接口
	 * 
	 * @param strConfigPath
	 *            [IN]:云帆产生配置文件及任务信息文件的路径
	 * @param strCachePath
	 *            [IN]:缓存视频文件的路径
	 * @param httpport
	 *            [IN]:本地http代理的端口号，创建任务和读取数据也可通过给本地端口发送http请求实现 GET
	 *            http://vodipad
	 *            .wasu.cn/201407021742/3cc51ccdac8e4d8ca896152a95415347
	 *            /pcsan08/
	 *            mams/vod/201404/24/20/20140424202708118c8104efd_dc5607f3.mp4
	 *            HTTP/1.1 Range: bytes=xxx-xxx
	 * @param maxusespace
	 *            [IN]:可用磁盘空间大小，所有文件大小不能超过此值，超过则删除最旧的任务，0为不限大小，单位为MB
	 * @return 返回值 ：见错误码
	 */
	//public static native int Init(String strConfigPath, String strCachePath, int httpport, int maxusespace);
	public static native int Init(String strConfigPath, String strCachePath, String strLogPath, Object cls, String strMethod);

	/**************************************************************************************
	GetPrivateVersion		获取内部版本号
	version				[IN/OUT]非空内存，用于存储版本号信息，至少50字节
	***************************************************************************************/
	public static native int GetPrivateVersion(byte[] version);


	/**
	SetConfig				设置配置信息
	pstrJson					[IN]配置参数,json串,如'{"data":{"maxcachetime":40}}'
	*/
	/**
	 * 设置配置信息
	 * @param strJson
	 *	[IN]:json配置信息
	 * @return
	 */
	public static native int SetConfig(String strJson);

	/**
	 * 释放云帆网络模块
	 * 
	 * @return
	 */
	public static native int Clear();

	/**
	 * 开启或关闭p2p网络模块，在网络环境发生变化时调用，断开网络和连上网络
	 * 
	 * @param status
	 *            [IN]:true为开启，false为关闭
	 * @return
	 */
	public static native int SetNetworkStatus(boolean status);

	/**
	 * 创建播放&下载任务
	 * 
	 * @param strOrgUrl
	 *            [IN] 要创建任务的链接,可直接用来http下载文件的url，如：
	 *            http://pcfastvideo.imgo.tv/6753
	 *            cb9c1f89d1d74e4200fe9f4c678f/54a371da
	 *            /c1/2014/dianying/yibuzhiyao
	 *            /20141212bc7f0e67-8e8c-41cd-a14b-068372ddf83d.fhv
	 * @param strKeyUrl
	 *            [IN] strSrcUrl中提取出来组成的固定url，如
	 *            http://hunantv.com/c1/2014/dianying
	 *            /yibuzhiyao/20141212bc7f0e67-8e8c-41cd-a14b-068372ddf83d.fhv
	 * @param strReferer
	 *            [IN] 湖南tv链接填hunantv.com
	 * @param hashArray
	 *            [OUT] strKeyurl对应的p2p hash值,40字节的字符串，创建任务后对任务的操作都以该值作为索引
	 * @param httpProxyUrl
	 *            [OUT] strKeyurl对应的播放代理地址,至少100字节的字符串，创建任务后用该代理地址播放
	 * @return
	 */
	public static native int CreateTask(String strOrgUrl, String strKeyUrl, String strReferer, byte[] hashArray, byte[] httpProxyUrl);

	/**
	 * 创建m3u8播放任务
	 *
	 * @param strOrgUrl
	 *            [IN] 要创建任务的链接,可直接用来http下载文件的url，如：
	 *            http://pcfastvideo.imgo.tv/6753
	 *            cb9c1f89d1d74e4200fe9f4c678f/54a371da
	 *            /c1/2014/dianying/yibuzhiyao
	 *            /20141212bc7f0e67-8e8c-41cd-a14b-068372ddf83d.fhv
	 * @param strKeyUrl
	 *            [IN] strSrcUrl中提取出来组成的固定url，如
	 *            http://hunantv.com/c1/2014/dianying
	 *            /yibuzhiyao/20141212bc7f0e67-8e8c-41cd-a14b-068372ddf83d.fhv
	 * @param hashArray
	 *            [OUT] strKeyurl对应的p2p hash值,40字节的字符串，创建任务后对任务的操作都以该值作为索引
	 * @param httpProxyUrl
	 *            [OUT] strKeyurl对应的播放代理地址,至少100字节的字符串，创建任务后用该代理地址播放
	 * @return
	 */
	public static native int CreateHlsTask(String strOrgUrl, String strKeyUrl, boolean bIsOtherCdn, byte[] hashArray, byte[] httpProxyUrl);


	/**************************************************************************************
	CreateUpdateTask	创建上传任务
	nType					[IN]  上传任务的类型 0:默认	1:耐飞文件上传
	pstrSrcUrl			[IN]  上传任务的url
	pstrfilepath		[IN]  上传的文件路径
	pstrHash			[OUT]  任务的p2p hash值，40字节的字符串，创建任务后对任务的操作都以该值作为索引
	***************************************************************************************/
	public static native int CreateUploadTask(int type, String strOrgUrl, String strFilePath, byte[] hashArray);


	/**************************************************************************************
	SetHeadValue	设置任务请求头信息
	pstrHash			[IN]  要设置的任务hash,在调用RunTask前设置
	pstHead				[IN]  请求head头
	pstrValue			[IN]  请求head的值
	***************************************************************************************/
	public static native int SetHeadValue(String strHash, String strHead, String strValue);

	/**
	 * 删除任务
	 * 
	 * @param strHash
	 *            [IN] 要删除的任务的hash
	 * @return
	 */
	public static native int DeleteTask(String strHash);

	/**
	 * 开启下载任务
	 * 
	 * @param strHash
	 *            [IN] 要开启下载的任务的hash
	 * @return
	 */
	public static native int RunTask(String strHash);

	/**
	 * 暂停下载任务
	 * 
	 * @param strHash
	 *            [IN] 要暂停下载的任务的hash
	 * @return
	 */
	public static native int PauseTask(String strHash);


	/**
	 * 设置任务的播放状态
	 * 
	 * @param strHash
	 *            [IN] 要设置播放状态的任务hash
	 * @param bPlaying
	 *            [IN] true:播放 false:未播放
	 * @return
	 */
	public static native int SetTaskPlaying(String strHash, boolean bPlaying);
	
	/**
	 * 读取文件内容
	 * 
	 * @param strHash
	 *            [IN] 要读取的文件hash
	 * @param dataArray
	 *            [OUT] 存储读取内容的空间,由调用者申请和释放
	 * @param start
	 *            [IN] 要读取的起始位置，单位byte
	 * @param len
	 *            [IN] 要读取的长度，单位byte
	 * @param readlen
	 *            [OUT] 实际读取的长度，单位byte
	 * @return
	 */
	public static native int ReadFile(String strHash, byte[] dataArray, long start, int len, int[] readlen);

	/**
	 * 设置任务缓存路径
	 * 
	 * @param strCachePath
	 *            [IN] 缓存路径
	 * @return
	 */
	public static native int SetCachePath(String strCachePath);

	/**
	 * 设置播放任务的当前播放位置，不使用本地代理方式创建任务和读取数据时，需要设置播放位置，用于 调整云帆下载模式 具体设置为：
	 * 1、定时(1秒)设置；
	 * 2、seek时立即设置；
	 * 
	 * @param strHash
	 *            [IN] 要设置的任务hash
	 * @param pos
	 *            [IN] 要设置的位置，单位为byte
	 * @return
	 */
	public static native int SetPlayingPos(String strHash, long pos);

	/**
	 * 设置播放任务的当前播放位置，使用本地代理方式创建任务和读取数据时，需要设置播放位置，用于 调整云帆下载模式 具体设置为：
	 * 1、定时(1秒)设置；
	 * 2、seek时立即设置；
	 * 
	 * @param strHash
	 *            [IN] 要设置的任务hash
	 * @param timepoint
	 *            [IN] 要设置的位置，单位为ms
	 * @param bSeek
	 *            [IN] 是否seek引起的播放位置变动
	 * @return 返回缓存的时间，单位为ms
	 */
	public static native int SetPlayingTimepoint(String strHash, int timepoint, boolean bSeek, int[] cacheTime);
	/**
	 * 设置任务是否在播放状态
	 * 
	 * @param strHash
	 *            [IN] 要设置的任务hash
	 * @param status
	 *            [IN]  true为正在播放，false为停止播放
	 * @return
	 */
	public static native int SetPlayingStatus(String strHash,boolean status);
	
	/**
	 * 设置可用磁盘空间大小，所有文件大小不能超过此值，程序启动后设置，0为不限大小
	 * 
	 * @param space
	 *            : [IN] 单位为MB
	 * @return
	 */
	public static native int SetSpaceCanUse(int space);

	/**
	 * 设置回调java函数，用于通知java一些重要事件的发生
	 * 
	 * @param cls
	 *            [IN] java函数所在的类对象
	 * @param strMethod
	 *            [IN] java函数的函数名
	 * @param strMethodPara
	 *            [IN] java函数的函数参数表，须为"(IILjava/lang/String;)V"
	 * @return
	 * 具体回调事件如下:
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
	public static native int SetCallbackFunction(Object cls, String strMethod, String strMethodPara);

	/**
	 * 查询任务信息
	 * 
	 * @param strHash
	 *            [IN] 要查询的任务hash
	 * @param info
	 *            [OUT] 保存任务信息，结构体如下<br>
	 *            struct TASKINFO<br>
	 *            {<br>
	 *            char hash[40]; //任务的hash<br>
	 *            int64_t filelen; //文件的大小，Byte<br>
	 *            int64_t downloadlen; //已下载的大小,Byte<br>
	 *            unsigned int downloadspeed; //下载速度，字节/秒<br>
	 *            unsigned int uploadspeed; //上传速度，字节/秒<br>
	 *            unsigned char status; //当前任务的状态，1:下载 2:暂停下载，若任务已下载完，则此状态无意义<br>
	 *            char filepath[256]; //文件的存储路径<br>
	 *            };
	 * @return
	 */
	public static native int QueryTaskInfo(String strHash, TaskInfo info);

	
	/**
	 * 查询任务是否下载/上传完成（耗时操作，避免频繁调用）
	 * 
	 * @param strHash   [IN] 要查询的任务hash
	 * @param bFinish	[OUT]true:完成 false:未完成
	 * @return
	 */
	public static native int TaskIsFinished(String strHash, boolean[] bFinish);
	
	/**
	 * 设置上传开关
	 * 
	 * @param status
	 *            [IN] true:允许p2p上传；false:禁止p2p上传
	 * @return
	 */
	public static native int EnableUpload(boolean status);

	/**
	 * 设置p2p下载开关
	 * 
	 * @param status
	 *            [IN] true:允许p2p下载；false:禁止p2p下载
	 * @return
	 */
	public static native int EnableP2pDownload(boolean status);

	/**
	 * 设置任务为下载模式，该模式在任务下载完后会拷贝到指定存储路径
	 * 
	 * @param strHash
	 *            [IN] 要设置的任务hash
	 * @param bOption
	 *            [IN] true:下载模式；false:取消下载模式
	 * @return
	 */
	public static native int SetTaskDownload(String strHash, boolean bOption);
	
	/**
	 * 设置视频的时长
	 * 
	 * @param strHash
	 *            [IN] 要设置的任务hash
	 * @param duration
	 *            [IN] 视频的时长，单位秒
	 * @return
	 */
	public static native int SetVideoDuration(String strHash, int duration);

	/**
	 * 删除缓存文件，正在离线的缓存文件不会被删
	 * 
	 * 	 * @return
	 */
	public static native int CleanCache();

	/**
	 * 更新下载链接，因为有些链接有时效性
	 *	 @param pstrHash			
	 *				[IN]  要设置的任务hash
	 *	 @param pstrUrl				
	 *				[IN]  http链接
	 * 
	 * 	 * @return
	 */
	public static native int UpdateHttpUrl(String strHash, String strUrl);

	/**
	 * 通知视频卡顿了，正在缓冲，结束缓冲
	 *	 @param pstrHash			
	 *				[IN]  要设置的任务hash
	 *	 @param buffering				
	 *				[IN]  bBuffering true:正在缓冲 false:结束缓冲
	 */
	 public static native int NotifyVideoBuffering(String strHash, boolean bBuffering);

	/**
	 * 设置当前设备类型
	 * 
	 * 	 @param type
	 *				[IN]	设备类型值，0为phone，1为pad
	 */
	public static native int SetDeviceType(int type);

	/**
	 * 设置当前限速模式
	 * 
	 * 	 @param limitMode
	 *				[IN]	限速模式(0:默认不限速 1:上网模式(自动模式) 2:观影模式 3:游戏模式)
	 */
	 public static native int SetLimitUploadSpeedMode(int limitMode);

	 /**
	 * 获取任务详细信息
	 * 
	 * @param strHash
	 *            [IN] 要读取的文件hash
	 * @param dataArray
	 *            [OUT] 存储读取内容的空间,由调用者申请和释放
	 * @param readlen
	 *            [IN] 要读取的缓冲区
	 * @param outlen
	 *           [OUT] 要读取的长度，单位byte,当函数返回错误E_NET_BUFFER_NOT_ENOUGH时,
						表明pBuf的空间大小不够,此时的readlen等于需要的最小长度
	 */
	 public static native int GetTaskInfo(String strHash, byte[] dataArray, int readlen, int[] outlen);
	 
	/**************************************************************************************
	GetDownloadingTaskLists			获取所有正在下载缓存的任务
	pBuf				[IN]  是输入的缓冲区，以json格式填充
	iBufLen				[IN/OUT] json大小,当函数返回错误E_NET_BUFFER_NOT_ENOUGH时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度
	***************************************************************************************/
	public static native int GetDownloadingTaskLists(byte[] pBuf, int[] iBufLen);
	
	/**
	 * 更新不同清晰度
	 *	 @param pstrHash			
	 *				[IN]  要设置的任务hash
	 *	 @param pstrUrl				
	 *				[IN]  清晰度http链接
	 *	 @param strUrlKey				
	 *				[IN]  清晰度http链接key
	 *	 @param nClarity				
	 *				[IN]  要切换的清晰度[0:标清 1:高清  2:超清  3:蓝光]
	 * 
	 * 	 * @return
	 */
	public static native int UpdateClarityUrl(String strHash, String strUrl, String strUrlKey, int nClarity);

	/**
	 * 更新网络类型(网络状态变化后调用)
	 *	 @param nMode				
	 *				[IN]  网络类型(0:默认未知 1:wifi网络 2:4G网络)
	 * 
	 * 	 * @return
	 */
	public static native int UpdateNetType(int nMode);
	
}
