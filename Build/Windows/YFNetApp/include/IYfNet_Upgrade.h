#ifndef _IYFNET_H_
#define _IYFNET_H_
#include <string.h>

//错误码的定义
typedef int NET_ERROR;
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
#define E_NET_PATH_NOT_ACCESS			-13	//路径不可访问
#define E_NET_CREATE_FOLDER_FAIL		-14	//创建目录失败

struct IYfNet;

#define MAX_FILE_PATH_LEN	511

#pragma pack(push)//保存对齐状态
#pragma pack(1)  //设置为1字节对齐
struct STaskInfo
{
	char hash[40];							//任务的hash
	__int64 iFileLen;						//文件的大小，Byte
	__int64 iDownloadLen;					//已下载的大小,Byte
	unsigned int iDownloadSpeed;			//当前的下载速度，B/s
	unsigned char status;					//当前任务的状态，1:下载 2:暂停下载，若任务已下载完，则此状态无意义 
	unsigned char tasktype;					//当前任务的类型，升级任务为9，其他为普通任务
	bool	bDownloadAndCopy;				//当前任务是否设置了“下载”
	wchar_t szCopyPath[MAX_FILE_PATH_LEN];	//若bDownloadAndCopy为true，则该字段不为空，存储下载完的拷贝目的路径，包括文件名
	wchar_t szFilepath[MAX_FILE_PATH_LEN];	//任务的文件路径，包括文件名
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
result:xxx
buf:40字节的hash
buflen：40
*/

#pragma pack(push)//保存对齐状态
#pragma pack(1)  //设置为1字节对齐
struct InitPara
{
	wchar_t* pTempCachePath;	//播放任务临时缓存目录
	wchar_t* pDownloadPath;		//下载任务存储的路径
	wchar_t* pConfigPath;		//p2p配置文件的路径
	unsigned int maxspace;		//缓存任务最大可使用空间，单位MB，超过则删除旧任务，0为无限制
	EventCallBackFunc callback;	//回调函数
	InitPara()
	{
		memset(this,0,sizeof(InitPara));
	}
};
#pragma pack(pop)//恢复对齐状态

#ifdef YFNETEXPORT
#define YFNETAPI extern "C" __declspec(dllexport) 
#else
#define YFNETAPI extern "C" __declspec(dllimport) 
#endif

//初始化网络模块,返回E_NET_OK表示成功,返回其他值表示失败;
//pYfNet [OUT] : 存储创建的对象指针
//para [IN] : 初始化的参数
//lPointer [OUT] : 初始化后的对象指针地址，释放网络模块时需要传入该值
YFNETAPI NET_ERROR _InitYfNet(IYfNet **pYfNet,InitPara para,long& lPointer);

//释放网络模块,lYfNetID为_InitYfNet的lPointer
YFNETAPI void  _ClearYfNet(long lYfNetID);


struct IYfNet
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
	***************************************************************************************/
	virtual NET_ERROR CreateTask(const char *pstrSrcUrl,const char* pstrKeyurl,const char* pstrReferer,char *pstrHash) = 0;

	/**************************************************************************************
	CreateUpdateTask	创建升级任务
	pstrSrcUrl			[IN]  要创建任务的链接,可直接用来http下载文件的url
	pstrHash			[OUT]  任务的p2p hash值，40字节的字符串，创建任务后对任务的操作都以该值作为索引
	***************************************************************************************/
	virtual NET_ERROR CreateUpdateTask(const char *pstrSrcUrl,char* pstrHash) = 0;

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
 	virtual NET_ERROR SetTaskDownload(const char *pstrHash,bool bOption,const wchar_t* pFilename) = 0;

	/***************************************************************************************	
	SetTempCachePath	设置任务的临时存储目录
	pstrPath			[IN]  绝对路径值
	***************************************************************************************/
	virtual NET_ERROR SetTempCachePath(const wchar_t *pstrPath) = 0;

	/***************************************************************************************	
	SetDefaultDownPath	设置下载任务的存储路径
	pstrPath			[IN]  绝对路径值
	***************************************************************************************/
	virtual NET_ERROR SetDefaultDownPath(const wchar_t *pstrPath) = 0;

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
	PlayTask			设置任务是否正在播放
	pstrHash			[IN]  要播放的任务的hash
	bPlaying			[IN]  true为播放，false为停止播放
	***************************************************************************************/
	virtual NET_ERROR SetTaskPlaying(const char *pstrHash,bool bPlaying) = 0;

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
	virtual NET_ERROR ReadFile(const char *pstrSrcUrl,const char *pstrHash,char *pbuf,__int64 nstart,long nlen,long *pnreadlen) = 0;
	
	/***************************************************************************************	
	QueryFileSize		查询文件大小
	pstrHash			[IN]  要查询的文件hash
	iFilesize			[OUT] 存储查询到的文件大小，当YfNet模块未从网络获取到时则iFilesize为0
	***************************************************************************************/
	virtual NET_ERROR QueryFileSize(const char *pstrHash,__int64& iFilesize) = 0;

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
	virtual NET_ERROR SetPlayingPos(const char *pstrHash,__int64 pos) = 0;

	/***************************************************************************************	
	SetVideoDuration		设置视频的时长
	pstrHash			[IN]  要设置的任务hash
	duration			[IN]  视频的时长，单位秒
	***************************************************************************************/
	virtual NET_ERROR SetVideoDuration(const char *pstrHash,int duration) = 0;
};
#endif