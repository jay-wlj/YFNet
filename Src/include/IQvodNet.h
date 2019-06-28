#ifndef	_IQVOD_NET_H__
#define _IQVOD_NET_H__

#include <Windows.h>

#ifdef QVODNET_EXPORTS
#define QVODNET_API __declspec(dllexport)
#else
#define QVODNET_API __declspec(dllimport)
#endif

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
#define E_NET_LINK_FORMAT_ERROR			-8	//链接格式错误
#define E_NET_MEM_NOT_CLEAR				-9	//没有清除内存
#define E_NET_NO_INIT					-10	//任务未初始化
#define E_NET_NO_DATA					-11	//未下载到数据
#define E_NET_SEED_ERROR				-12	//种子错误
#define E_NET_DISK_NOT_ENOUGH			-13	//磁盘空间不足
#define E_NET_CREATE_TASK_FAILED		-14	//创建任务失败
#define E_NET_BUFFER_NOT_ENOUGH			-15	//传递进来的buffer空间大小不够
#define E_NET_FILE_ALREADY_EXIST		-16	//文件已经存在
#define E_NET_TASK_ALREADY_EXIST		-17	//任务已经存在
#define E_NET_TERMINAL_VERSION_ERROR	-18	//Terminal和QvodNet.dll的版本不一致
#define E_NET_NO_CHANNEL				-19 //直播频道已经被删除了
#define E_NET_ASSERT_LINK				-20 //该任务已经设置了防盗链
#define E_NET_NO_BITFIELD				-21 //该任务非P2P任务,没有比特位信息
#define E_NET_FAT_NOT_SUPPORT_FILE		-22 //FAT格式不支持大于4G的文件
#define E_NET_NO_SEED_FILE				-23 //种子文件不存在
#define E_NET_NO_TASK_BY_HASH			-24 //没有hash对应的任务
#define E_NET_START_TERMINAL_FAILED		-25 //启动Terminal失败
#define E_NET_USER_NO_LOGIN				-26	//用户未登录
#define E_NET_USER_PASSWORD_INCORRECT	-27 //用户密码错误
#define E_NET_USER_LOGINNING			-28	//用户正在登录中
#define E_NET_USER_CANCEL_LOGIN			-29 //用户取消登录
#define E_NET_USER_KEEPLIVEING			-30 //和服务器失去联系,保活中
#define E_NET_USER_LOGIN_FAILED			-31 //用户登录失败
#define E_NET_USER_CONNECT_SERVER_FAIL	-32 //连接服务器失败
#define E_NET_USER_INVALID_SESSION_ID	-33 //无效的session id
#define E_NET_USER_INVALID_EMAIL		-34 //未验证的邮箱
#define E_NET_USER_OFF_LINE				-35 //用户掉线,不是异地登录
#define E_NET_USER_KUAIWAN_PASSWORD		-36 //需要快玩用户密码
#define E_NET_USER_INVALID_SETTING		-37	//无效设置
#define E_NET_NO_DEVICE					-38 //没有设备可查询
#define	E_NET_NO_LAN_TASK				-39 //没有任务
#define E_NET_REFUSE_CON				-40 //拒绝连接

#define E_NET_LAN_OPEN					-41 //跨终端已经打开
interface IQvodNet;

//初始化网络模块,返回0表示失败,返回其他值表示成功;
//初始化时需记录该返回值,释放网络模块时需要传入该值;
extern "C" QVODNET_API long	 _InitQvodNet(IQvodNet **pQvodNet);

//释放网络模块,lQvodNetID,_InitQvodNet的返回值;
//bTerminalClose为真,表示同时也退出terminal
extern "C" QVODNET_API void  _ClearQvodNet(long lQvodNetID,bool bTerminalClose=false);

#define QLIVE_PIECE_SIZE	(43*188)
#define TASK_FILE_NANE_LEN	511
#define FILE_INFO_NAME_LEN	255


#pragma pack(push)
#pragma pack(1)

interface TASKINFO
{
	char hash[40];
	unsigned int hifilelen;
	unsigned int lofilelen;						//字节
	unsigned int htotaldownload;				//字节
	unsigned int ltotaldownload;				//字节
	unsigned int uploadlen;						//字节
	unsigned int downloadlen;					//字节
	unsigned int uploadrate;					//字节/秒
	unsigned int downloadrate;					//字节/秒
	unsigned int iVipCurDownloadLen;			//(当前下载长度),从高速通道服务器下载的数据长度,KB
	unsigned int iVipDownloadLen;				//(累计下载长度),该任务通过高速通道服务器下载的数据长度,单位KB
	unsigned int iVipDownloadSpeed;				//该任务从高速通道服务器下载的速度,单位字节/秒(B/S)
	unsigned char uploadlimit;					//最大上传多少K
	unsigned char status;						//1:run 2:pause 3:stop 4:play 5:wait
	unsigned char npeers;						//当前连接数
	unsigned char tasktype;						//任务类型，TVDOWNLOADTASK=1,BTDOWNLOADTASK=2
	unsigned long CreateTime;					//文件创建的时间
	unsigned short files;						//文件个数
	unsigned char bCheckKey;					//(bCheckKey & 0x01 != 0)防盗链标志;(bCheckKey & 0x02 != 0)防下载标志
	unsigned char bVipAccStatus;				//高速通道服务器状态,0->空状态;1->正在运行;2->关闭;3->就绪(未连接状态);4->正在向tracker请求;5->不能加速
	unsigned char bCanAcc;						//高速通道是否支持该文件,0->不支持; 1->支持; 2->未知,可调用接口进行查询
	unsigned char chErrorMask;					//错误掩码 (chErrorMask & 0x01)为真,表示文件写入失败;(chErrorMask & 0x02)为真,表示文件路径不存在,如移动硬盘被拔走等;
	WCHAR wszFileName[TASK_FILE_NANE_LEN];		//文件名
};

interface HTTPTASKINFO
{
	char		 szHash[40];
	unsigned int HiFileLen;				//字节
	unsigned int LoFileLen;				//字节
	unsigned int HiTotalDownloadLen;	//字节
	unsigned int LoTotalDownloadLen;	//字节
	unsigned int iDownloadSpeed;		//字节/秒
	WCHAR		 szFilePathName[MAX_PATH];
};

//BITINFO的结构 
interface BITINFO
{ 
	unsigned int BlockSize;
	unsigned int BlockNum;
	unsigned int HiDownLoadlen;
	unsigned int LoDownLoadLen;
	int DownLoadSpeed;
	char szReserved[3];
	char szBitField[1];
};	

interface FILEINFO
{
	unsigned int hifilelen;
	unsigned int lofilelen;
	unsigned int hidownloadlen;
	unsigned int lodownloadlen;
	unsigned int hiTotalDownloadLen;
	unsigned int loTotalDownloadLen;
	unsigned short index;
	WCHAR wszFileName[FILE_INFO_NAME_LEN];
};

struct QLiveInfo
{
	char szHash[40];					//直播hash,是通过Hash2Char函数转换过的
	unsigned int  HiUploadLen;			//字节
	unsigned int  LoUploadLen;			//字节
	unsigned int  HiDownloadLen;		//字节	
	unsigned int  LoDownloadLen;		//字节
	unsigned int  iUploadSpeed;			//上传速度,B/S
	unsigned int  iDownloadSpeed;		//下载速度,B/S
	unsigned int  iPeersNum;			//连接数
};

//用户信息
interface  QUSER_INFO
{
	unsigned char	szSessionID[8];			//session id
	unsigned char	szCloudSessionID[8];	//cloud session id
	unsigned int	iQvodLevel;				//快播等级
	unsigned int	iQvodScores;			//快播积分
	unsigned int	iVipLevel;				//vip等级
	unsigned int	iSurplusAccFlow;		//剩余加速流量(MB)
	unsigned int	iVipExpiredTime;		//vip的剩余时间
	unsigned char	szUserName[32];			//用户名,最长15个字节
	unsigned char   szPetName[32];			//用户昵称,最长15个字节
	unsigned int	iKey;					//加密key
	unsigned char	bIsVip;					//是否是VIP用户,0:否,1:是,2:过期
	unsigned char	chStatus;				//用户登录状态
	unsigned char	chIsCheckedEmail;		//邮箱是否验证过,0:未验证;1:已验证
	unsigned char	chVipType;				//Vip用户类型,0:月;1:季度;2:半年度;3:年费
	unsigned int	iTotalAccFlow;			//高速通道总流量(MB)
	unsigned int	iQvodCurLevelExp;		//快播升级到当前等级的经验值
	unsigned int	iQvodNextLevel;			//快播升级所需要的积分
	unsigned int	iVipCurrentExp;			//VIP用户当前经验值
	unsigned int	iVipNextLevel;			//VIP用户升级所需要的经验值
	unsigned char	szUserEmail[52];		//用户邮箱
	unsigned int	iUserID;				//用户ID
	unsigned char	szReserved[4];			//保留字段
};

//全局高速通道服务器相关信息
interface GLOBAL_ACCELERATE_INFO
{
	unsigned int iDownloadLen;			//全局高速通道下载的数据,单位KB
	unsigned int iSurplusLen;			//全局高速通道剩余流量,单位KB
	unsigned int iDwonloadSpeed;		//全局高速通道下载速度,单位字节/秒(B/S)
	unsigned char szReserved[4];		//保留字段
};

struct TIPS_INFO 
{
	unsigned long		iRecvTime;			//该Tips从服务器取回来的时间,GetTickCount()
	unsigned __int64	iTipsID;				//该Tips的ID号,清除Tips及上报Tips的弹出结果需要用到
	unsigned int		iLeftTime;			//该Tips的有效剩余时间,单位:秒
	unsigned char		szTipsUrl[1];		//该Tips的Url地址
};

interface PEERINFO
{
	char peerid[20];
	unsigned int HiUploadLen;//字节
	unsigned int LoUploadLen;//字节
	unsigned int HiDownloadLen;//字节
	unsigned int LoDownloadLen;//字节
	unsigned int uploadrate;//字节/秒
	unsigned int downloadrate;//字节/秒
	unsigned int ip;
	unsigned short port;
	char nattype;
	char reserved;
};

interface GLOBALINFO
{
	char peerid[20];
	unsigned int uploadlen;		//上传大小
	unsigned int downloadlen;	//下载大小
	unsigned int uploadrate;	//总上传速度
	unsigned int downloadrate;	//总下载速度
	unsigned int tasknum;		//总任务数
	unsigned int sharetasknum;	//共享任务数
	unsigned short httpport;	//服务器http端口
	unsigned short httplimit;	//服务器http通讯单文件带宽限制
	unsigned char upratelimit;	//上传速度限制
	unsigned char peerslimit;	//peer数限制
	unsigned char seedslimit;	//种子数限制
	char trackerstatus;			//tracker返回的状态
	char keycode[20];			//防盗链code		
	unsigned int reserved;		//保留
};

enum ADTYPE {BUFFER_AD = 1,PAUSE_AD,TEXT_AD};
//跨平台文件传输枚举类型
struct  QVOD_DEVICE_INFO
{
	char		guid[40];	    //设备标识
	char		bBusy;			//是否可以连接
	char		type;			//iPhone,android,pc
	char		status;			//空,发起连接,连接成功,连接失败,对方拒绝,
	char		reserved;
	wchar_t		wszUserName[64];	//对方用户名
};

struct QVOD_TASK_INFO 
{
	unsigned int speed;				//发送/接收速度
	__int64		fileLen;			//发送/接收文件的总长度
	__int64		finishLen;			//已经发送/接收的总长度
	char		type;				//类型,P2P任务,或者本地文件
	char		status;				//发送中,接收中,等待发送,等待接收
	char		flag;				//DEVICE_FILE_STATUS_WAIT_SEND表示等待发送的文件，DEVICE_FILE_STATUS_WAIT_RECV等待接收的文件
	char		szReserved;
	wchar_t		wszFilePath[260];	//文件路径
};


struct LOCALDEVICEINFO 
{
	char chStatus;					//本机当前状态
	char chPeerType;				//对方设备类型
	char chPeerStatus;				//对方是否关闭窗口，DEVICE_DIALOG_OPEN 打开，DEVICE_DIALOG_CLOSE关闭
	char chInitiative;				//主被动标志 0 空，1主动，2被动
	char szHostGuid[40];			//本机GUID
	char szHostName[64];			//本机名字
	char szPeerGuid[40];			//对方GUID
	char szPeerName[64];			//对方名字
};
#pragma pack(pop)

enum DEVICE_FILE_ACTION
{
	DEVICE_ADD_FILE = 1,	//添加一个文件
	DEVICE_SEND_FILE,	//开始发送文件
	DEVICE_CANCEL_FILE	//取消发送文件
};

enum QVOD_BUSY_FLAG
{
	DEVICE_FLAG_BUSY = 1,	//设备忙碌
	DEVICE_FLAG_FREE		//设备空闲
};

enum QVOD_DEVICE_TYPE
{
	DEVICE_TYPE_PC = 1,
	DEVICE_TYPE_IPHONE,
	DEVICE_TYPE_ANDROID

};

enum QVOD_DEVICE_STATUS
{
	DEVICE_STATUS_NONE = 0,				//空
	DEVICE_STATUS_CONNECT_REQUEST,		//发起连接
	DEVICE_STATUS_CONNECT_FAIL,			//连接失败
	DEVICE_STATUS_CONNECT_REFUSE,		//对方拒绝连接
	DEVICE_STATUS_CONNECT_WAIT,			//有一个对方的连接请求,等待用户选择
	DEVICE_STATUS_CONNECT_DISABLE,		//对方主动断开连接
	DEVICE_STATUS_CONNECT_INVALID,		//连接不同步，导致连接无效
	DEVICE_STATUS_CONNECT_SUCCESS,		//连接成功
	DEVICE_STATUS_FILE_PEER_WANT,		//对方希望发送一个文件 被动方
	DEVICE_STATUS_FILE_WAIT_RECV,		//等待接收,当调用StartRecvFile接口后,进入该状态 主动方
	//心跳
	DEVICE_STATUS_NET_BROKEN			//对方已以断网	
};


enum QVOD_TRANSFILE_TYPE
{
	DEVICE_TRANSFILE_PREVIEW = 0,	//略缩图
	DEVICE_TRANSFILE_VIDEO,			//视频文件
	DEVICE_TRANSFILE_AUDIO,			//音频文件
	DEVICE_TRANSFILE_SEED			//种子文件
};


enum QVOD_DEVICE_TASK_STATUS
{
	DEVICE_FILE_STATUS_SENDING = 1,	//发送中
	DEVICE_FILE_STATUS_RECVING,		//接收中
	DEVICE_FILE_STATUS_WAIT_SEND,	//等待发送
	DEVICE_FILE_STATUS_WAIT_RECV,	//等待接收
	DEVICE_FILE_STATUS_FINISHED,	//接收或者发送完成
	DEVICE_FILE_STATUS_INITIATIVE_INTERRUPT,	//传输过程主动断开文件的传输或发送
	DEVICE_FILE_STATUS_PASSIVITY_INTERRUPT,		////文件传输过程意外中断
	DEVICE_FILE_STATUS_DELETE			//界面显示文件被删除
};


enum QVOD_DEVICE_DIALOG
{
	DEVICE_DIALOG_OPEN = 1,
	DEVICE_DIALOG_CLOSE
};

interface IQvodNet
{
	/************************************************************************/
	/* 获取一条优先级最高的Tips信息
	/* pInfo	[OUT] 传出参数,返回Tips的信息
	/* 若*pInfo返回空,则表示没有任何Tips信息
	/************************************************************************/
	virtual NET_ERROR GetTipsInfo(TIPS_INFO **pInfo) = 0;


	/************************************************************************/
	/* 删除一条已获取的Tips信息
	/* pTipsID	[IN]  传入参数,可以从GetTipsInfo()获得
	/************************************************************************/
	virtual NET_ERROR RemoveTipsInfo(unsigned __int64 iTipsID) = 0;


	/************************************************************************/
	/* 查询hash在高速通道服务器上是否存在,即是否可以开启高速通道
	/* pHash			[IN] 要查询的hash
	/* 返回结果可以在TASKINFO中查询
	/************************************************************************/
	virtual NET_ERROR SearchHashOnAccServer(char *pHash) = 0;


	/************************************************************************
	* UserLogout		用户退出登录,当查询用户信息返回未登录时,表明退出成功;
						函数返回后,不用等待可直接再次调用登录函数
	* pUserName			用户名,'\0'结束
	* pSessionID		会话ID,8字节
	************************************************************************/
	virtual NET_ERROR UserLogout(char *pUserName,char *pSessionID) = 0;


	/************************************************************************
	* GetGlobalAccelerateInfo 查询全局高速通道信息
	* pInfo				[OUT] 返回全局信息,不能为空
	************************************************************************/
	virtual NET_ERROR GetGlobalAccelerateInfo(GLOBAL_ACCELERATE_INFO *pInfo) = 0;

	
	/************************************************************************
	* SetTaskAccelerate 设置单个任务是否开启高速通道
	* pHash				[IN] 任务hash,40位
	* bIsAccelerate		[IN] 是否开启高速通道,true:开启;false:关闭
	************************************************************************/
	virtual NET_ERROR SetTaskAccelerate(char *pHash,bool bIsAccelerate) = 0;


	/************************************************************************
	SetUserInfo			设置用户信息,用户名、邮箱或者手机号码,密码
	pUserName			[IN] 用户名
	pUserEmail			[IN] 邮箱
	pUserCellphone		[IN] 手机号码,用户名、邮箱和手机号,必须有一个不为空
	pPassword			[IN] 密码,不能为空
	备注:由于没有长度,所有的字符串必须以'\0'结束
	************************************************************************/
	virtual NET_ERROR SetUserInfo(char *pUserName,char *pUserEmail,char *pUserCellphone,char *pPassword) = 0;


	/************************************************************************
	SetKuaiwanPassword  设置快玩用户密码
	pPassword			[IN] 快玩密码,加密过后的数据
	len					[IN] 长度
	备注:只有当GetUserInfo返回E_NET_USER_KUAIWAN_PASSWORD错误时,才需要设置快玩用户密码;
		 否则将返回E_NET_USER_INVALID_SETTING错误
	************************************************************************/
	virtual NET_ERROR SetKuaiwanPassword(char *pPassword,int len) = 0;


	/************************************************************************
	GetUserInfo			查询当前登录用户的信息
	pUserInfo			[OUT]传出参数,返回UserInfo,不能为空
	************************************************************************/
	virtual NET_ERROR GetUserInfo(QUSER_INFO *pUserInfo) = 0;


	/************************************************************************/
	/* ConnectTerminal	连接terminal操作,0表示成功,其他表示失败
	/* bStartTerminal	[IN] 是否要启动Terminal
	/************************************************************************/
	virtual NET_ERROR ConnectTerminal(bool bStartTerminal = true) = 0;


	/**************************************************************************************
	CreateTask			创建播放任务,包括http,qvod,BT等
	wpstrSrcUrl			[IN]  要创建任务的链接,包括qvod,http链接,BT种子路径等,以'\0'结束
	pstrSrcUrl			[IN]  引用页地址
	pstrHash			[OUT] hash值,40位
	flag				[IN]  创建任务标志,flag=1表示正常创建;2表示只下载,不播放;3表示只创建,即创建了就是暂停状态;
	***************************************************************************************/
	virtual NET_ERROR CreateTask(const wchar_t *wpstrSrcUrl,char *pstrHash,char *pstrSrcUrl,char flag=1) = 0;



	/************************************************************************/
	/* CreateHideTask	创建隐藏任务
	/* pstrHash			[IN] 隐藏任务的hash
	/* pSavePath		[IN] 保存任务的路径,绝对路径,包括文件名
	/************************************************************************/
	virtual NET_ERROR CreateHideTask(const char *pstrHash,const wchar_t *pSavePath) = 0;



	/***************************************************************************************	
	ShareResources		共享任务
	pstrPath,			传入参数,要共享的文件名或者目录,'\0'结束;
						如果是文件,则为文件的绝对路径;如果是目录,则需要在末尾添加"\",如D:\Media\
	pstrUrl,			传入参数,广告地址,可以为NULL
	***************************************************************************************/
	virtual NET_ERROR ShareResources(const wchar_t *wpstrPath,const wchar_t *wpstrUrl) = 0;


	/***************************************************************************************	
	UnshareResource		取消共享任务
	pstrPath			传入参数,要取消共享的目录,'\0结束'
	***************************************************************************************/
	virtual NET_ERROR UnshareResource(const wchar_t *wpstrPath) = 0;


	/***************************************************************************************
	QueryGlobalInfo		查询全局信息
	pGlobalInfo			[OUT] GLOBALINFO结构体指针
	***************************************************************************************/
	virtual NET_ERROR QueryGlobalInfo(GLOBALINFO *pGlobalInfo) = 0;


	/***************************************************************************************	
	SetDefaultDownPath	设置默认下载目录
	pstrPath			[IN]  绝对路径值
	***************************************************************************************/
	virtual NET_ERROR SetDefaultDowmPath(const wchar_t *wpstrPath) = 0;


	/***************************************************************************************
	QueryFileInfo		查询文件信息
	pstrHash,			[IN]  Hash值,40位
	pBuf,				[OUT] 返回查询到的FileInfo的信息,函数不负责申请空间和释放空间
	iBufLen				[IN/OUT] pBuf的长度,当函数返回错误-15(E_NET_BUFFER_NOT_ENOUGH)时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度;对于Buffer的长度,
						普通Qvod只有一个FileInfo信息,而对于BT任务,可能就有多个
	iCount				[OUT] FileInfo的个数,函数会先将iCount赋值为0
	***************************************************************************************/
	virtual NET_ERROR QueryFileInfo(const char *pstrHash,char *pBuf,int &iBufLen,int &iCount) = 0;


	/***************************************************************************************	
	DeleteTask			删除任务
	pstrHash,			[IN] 要删除的任务的hash,40位
	bIsDeleteFile		[IN] 是否删除文件(如果用户选择"删除任务与文件",则该值需赋值为true,否则为false)
	***************************************************************************************/
	virtual NET_ERROR DeleteTask(const char *pstrHash,bool bIsDeleteFile = false) = 0;


	/***************************************************************************************	
	PauseTask			暂停任务
	pstrHash,			[IN]  要暂停的任务的hash,40位
	bIsAll,				[IN]  是否暂停所有任务,如果是,pstrHash = NULL
	***************************************************************************************/
	virtual NET_ERROR PauseTask(const char *pstrHash,bool bIsAll=false) = 0;


	/***************************************************************************************	
	RunTask				运行任务(开始下载)
	pstrHash,			[IN]  要运行的任务的hash,40位
	bIsAll,				[IN]  是否运行所有任务,如果是,pstrHash = NULL
	***************************************************************************************/
	virtual NET_ERROR RunTask(const char *pstrHash,bool bIsAll=false) = 0;


	/***************************************************************************************	
	PlayTask			播放任务
	pstrHash,			[IN]  要播放的任务的hash,40位
	iIndex,				[IN]  当前播放文件的序号,从0开始计数;如果是Qvod任务,为0
	***************************************************************************************/
	virtual NET_ERROR PlayTask(const char *pstrHash,int iIndex) = 0;


	/*************************************************************************************
	StopPlay			停止播放任务
	pstrHash			[IN]  停止播放任务的hash
	**************************************************************************************/
	virtual NET_ERROR StopPlay(const char *pstrHash) = 0;


	/***************************************************************************************	
	QueryTaskInfo		查询所有任务信息
	pBuf				[OUT] 保存查询到的TaskInfo的信息,函数不负责空间的申请和释放
	iBufLen				[IN/OUT] pBuf的大小,当函数返回错误-15(E_NET_BUFFER_NOT_ENOUGH)时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度;
	iCount				[OUT] 返回TaskInfo的个数,函数会先将iCount赋值为0
	***************************************************************************************/
	virtual NET_ERROR QueryTaskInfo(char *pBuf,int &iBufLen,int &iCount) = 0;


	/***************************************************************************************	
	QueryPeers			查询Peer信息
	pstrHash,			[IN]  要查询的任务的hash,40位,直播查询Peer信息也是调用该接口
	pBuf,				[OUT] 保存查询到的Peers信息,函数不负责空间的申请和释放
	iBufLen				[IN/OUT] pBuf的大小,当函数返回错误-15(E_NET_BUFFER_NOT_ENOUGH)时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度;
	iCount				传出参数,Peer的个数,函数首先会将iCount赋值为0
	***************************************************************************************/
	virtual NET_ERROR QueryPeers(const char *pstrHash,char *pBuf,int &iBufLen,int &iCount) = 0;


	/***************************************************************************************	
	ReadFile			读取文件内容
	pstrHash			[IN]  要读取的文件hash,40位
	nindex				[IN]  文件序号,如果是qvod,则为0;必须和PlayTask中设置的一致
	pbuf				[OUT] 存储读取内容的空间,由调用者申请和释放
	nstart				[IN]  起始位置
	nlen				[IN]  读取的长度
	pnreadlen			[OUT] 实际读取的长度
	***************************************************************************************/
	virtual NET_ERROR ReadFile(const char *pstrHash, int nindex, 
		char *pbuf, __int64 nstart, long nlen, long *pnreadlen) = 0;


	/***************************************************************************************	
	QueryBitInfo		查询Bit位下载信息 
	pstrHash			[IN]  是指定的查询的HASH内容,40位
	pBuf				[IN]  是输入的缓冲区，
	iBufLen				[IN/OUT] 保存比特位信息的空间大小,当函数返回错误-15(E_NET_BUFFER_NOT_ENOUGH)时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度
	***************************************************************************************/
	virtual NET_ERROR QueryBitInfo(const char *pstrHash, char *pBuf,int &iBufLen) = 0;



	/****************************************************************************************
	SetRateLimit		设置上传下载速度限制,单位KB
	iSpeed				[IN]  限速的等级
	**************************************************
	* iSpeed	说明		down		up
	*  -1:		自动模式	0/135		40
	*   0:		限制传输	2			1
	* 1~9:		限速模式	iSpeed*20	iSpeed*5
	*  10:		无限制		0			0
	**************************************************
	****************************************************************************************/
	virtual NET_ERROR SetRateLimit( int iSpeed ) = 0;


	/*****************************************************************************************
	QueryTaskByHash		通过Hash查询任务信息
	pstrHash			[IN]  要查询文件的Hash值,40位
	pTaskInfo			[OUT] 保存查询到的任务信息,函数不负责空间的申请和释放
	****************************************************************************************/
	virtual NET_ERROR QueryTaskByHash(const char *pstrHash,TASKINFO *pTaskInfo) = 0;


	/*****************************************************************************************
	QueryTaskReferUrl	查询任务的引用页地址
	pstrHash			[IN]  Hash值,40位
	pBuf				[OUT] 保存查询到的引用页地址,函数不负责空间的申请和释放
	iBufLen				[IN/OUT] pBuf的大小,当函数返回错误-15(E_NET_BUFFER_NOT_ENOUGH)时,
						表明pBuf的空间大小不够,此时的iBufLen等于需要的最小长度
	*/
	virtual NET_ERROR QueryTaskReferUrl(const char *pstrHash,char *pBuf,int &iBufLen) = 0;


	/*****************************************************************************************
	GetLocalIP			获得本地IP地址,点分十进制表示
	LocalIP				[OUT] 用来保存IP地址的字符串,函数不负责空间的申请和释放
	*/
	virtual NET_ERROR GetLocalIP(char *pLocalIP) = 0;


	/************************************************************************/
	/* GetTorrentInfo	获得一个BT种子的文件信息
	/* wpstrPath		[IN]  BT种子的绝对路径
	/* pstrHash			[OUT] 保存Hash的buffer,调用者申请,函数不负责内存分配和释放
	/* pInfoBuf			[OUT] 保存BT种子里面每个文件的信息,调用者申请内存,函数不负责内存的分配和释放
	/* iCount			[IN/OUT] FILEINFO的数组的元素个数,如果函数返回E_NET_BUFFER_NOT_ENOUGH,iCount为最小FILEINFO的元素个数
	/************************************************************************/
	virtual NET_ERROR GetTorrentInfo(const wchar_t *wpstrPath,char *pstrHash,FILEINFO *pInfoBuf,int &iCount) = 0;


	/************************************************************************/
	/* GetFileLength	获取文件的长度
	/* pstrHash			[IN]  任务Hash,40位
	/* pFileLen			[OUT] 返回的保存文件长度的指针
	/* index			[IN]  文件的索引号,BT文件存在多个文件的情况
	/* 返回值:
	/* E_NET_OK表示查询文件长度成功,*pFileLen为文件长度,但是有可能等于0
	/* 原因是Terminal暂时还没收到关于文件长度的信息
	/************************************************************************/
	virtual NET_ERROR QueryFileLength(const char *pstrHash,unsigned __int64 *pFileLen,int index = 0) = 0;

	/************************************************************************/
	/* QueryContinueLen	查询pos位置之后,连续下载长度
	/* pstrHash			[IN]  任务hash,40位
	/* pos				[IN]  计算连续长度的起始位置
	/* pLen				[OUT] 返回连续长度
	/************************************************************************/
	virtual NET_ERROR QueryContinueLen(const char *pstrHash,__int64 pos,__int64 *pLen) = 0;


	/************************************************************************/
	/* SetDownloadPos	设置下载位置
	/* pstrHash			[IN]  任务HASH,40位
	/* iDownloadPos		[IN]  下载位置
	/* iIsFromUser		[IN]  是否是用户拖动导致,0->否,1->是
	/************************************************************************/
	virtual NET_ERROR SetDownloadPos(const char *pstrHash,unsigned __int64 iDownloadPos,int iIsFromUser = 0) = 0;


	/************************************************************************/
	/* SetNetPara		设置网络参数
	/* bIsUdpOnly		[IN]  (true表示)尝试只以UDP协议传输数据
	/* bIsHttpProtocol	[IN]  (true表示)尝试以HTTP协议传输数据
	/* bIsKeepTerminal	[IN]  (true表示)加入快播缓存优化计划,不自动退出terminal
	/* bReserved		保留字段
	/************************************************************************/
	virtual NET_ERROR SetNetPara(bool bIsUdpOnly,bool bIsHttpProtocol,bool bIsKeepTerminal,bool bReserved = false) = 0;


	/************************************************************************/
	/* SetFileIndex		设置当前播放文件索引位置
	/* pstrHash			[IN] 任务hash,40位
	/* iPos				文件索引的起始位置
	/************************************************************************/
	virtual NET_ERROR SetFileIndexPosition(const char *pstrHash,unsigned __int64 iPos) = 0;


	/************************************************************************/
	/* QueryAdUrl		查询广告地址
	/* type				[IN]  广告类型,BUFER_AD:缓冲广告,PAUSE_AD:暂停广告,TEXT_AD:文字链广告
	/* pstrHash			[IN]  任务Hash,40位
	/* wpUrlBuffer		[OUT] 保存广告地址的空间
	/* iLen				[IN/OUT] wpUrlBuffer的长度,当返回E_NET_BUFFER_NOT_ENOUGH错误时,iLen为wpUrlBuffer的最小长度
	/************************************************************************/
	virtual NET_ERROR QueryAdUrl(ADTYPE type,const char *pstrHash,wchar_t *wpUrlBuffer,int &iLen) = 0;


	/************************************************************************/
	/* 查询HTTP任务信息
	/* pstrHash			[IN]  HTTP任务的hash,40位
	/* pHttpInfo		[OUT] 保存查询到的HTTP任务信息
	/************************************************************************/
	virtual NET_ERROR QueryHttpTaskInfo(const char *pstrHash,HTTPTASKINFO *pHttpInfo) = 0;



	/************************************************************************/
	/* 设置和统计相关的信息,需要发送统计信息才调用该接口
	/* pstrFlag			[IN] 标识符,16字节
	/* pstrHardwareID   [IN] 硬件ID,40字节
	/* iStartTime		[IN] 播放器启动的时间
	/************************************************************************/
//	virtual NET_ERROR SetStatisticHeader(char *pstrFlag,char *pstrHardwareID,unsigned int iStartTime) = 0;



	/************************************************************************/
	/* 设置N+可播放文件路径
	/* pstrFilePath		[IN]  N+播放文件路径,不可以为NULL,必须以'\0'结束
	/************************************************************************/
	virtual NET_ERROR SetNPlusFilePath(char *pstrFilePath) = 0;


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////以下接口为直播专用///////////////////////////
	/************************************************************************/
	/* 开启直播频道
	/* program_hash		[IN]  直播链接地址,以qlive://开头,以'\0'结束
	/* pQliveHash		[OUT] 直播任务返回的hash,40位
	/* flag				[IN]  创建任务标志,flag=1表示正常创建,创建后暂停其他任务,并下载
							  2表示只下载,不暂停其他;
							  3表示只创建,即创建了就是暂停状态,不下载;
	/************************************************************************/
	virtual NET_ERROR QLiveFsOpen(char *program_hash,char *pQliveHash,char flag=1) = 0;


	/************************************************************************
	* 读取直播数据
	* pQliveHash QLiveFsOpen后得到的直播频道hash,40位
	* buf 为用来读取数据的内存空间，调用前需要先分配好，函数不负责申请空间，也不负责释放空间
	* buf_size 为 buf 的大小，单位：bytes,直播大小至少为43*188(约为8K),即每次读取一片的数据
	* iReadLen 为实际读取的数据长度
	* timeout_in_ms 为无数据时等待的超时值，-1 为阻塞，0 为立即返回，>0 为无数据等待数据的毫秒数，单位：毫秒
	* is_continue 为输出参数，标志是否连续，1 为当前读取到的 buf 与上一次调用本函数时读取的 buf 是连续的
	* index	   传入参数时,需要读取的数据块编号;传出参数时,返回的下一次读取的数据块编号;第一次读取时,填0
	* iQliveID 直播频道的节目ID,如果直播源服务器重启了,该ID会发生变化,当前播放的直播频道跟源服务器的ID不一样时,需要index从0开始
	*          第一次读取数据时,填0
	************************************************************************/
	virtual NET_ERROR QLiveFsRead(char *pQliveHash,void *buf,int buf_size,int &iReadLen,int timeout_in_ms,int &is_continue,int &index,unsigned int &iQliveID) = 0;


	/************************************************************************
	* 关闭直播
	* pQliveHash 直播频道的hash,40位,调用QLiveFsOpen后得到
	************************************************************************/
	virtual NET_ERROR QLiveFsClose(char *pQliveHash) = 0;

	/************************************************************************/
	/* 查询直播任务信息
	/* pQliveHash 直播频道的hash,40位,调用QLiveFsOpen后得到
	/* pQliveInfo 直播任务信息的结构体指针
	/************************************************************************/
	virtual NET_ERROR QLiveFsQuery(char *pQliveHash,QLiveInfo *pQliveInfo) = 0;



	/************************************************************************/
	/* 将播放文件的码率通知到QvodTerminal
	/* pHash	 [IN]  播放文件的hash
	/* iRate	 [IN]  播放文件的码率,单位:Kbps
	/************************************************************************/
	virtual NET_ERROR SetPlayFileRate(const char *pHash,unsigned int iRate) = 0;

	/************************************************************************/
	/* 发送局域网广播
	/************************************************************************/
	/************************************************************************/
	/*开启跨终端传输功能，返回E_NET_LAN_OPEN表示已经打开（一般是后台运行），E_NET_OK表示打开成功，返回 E_NET_NO_INIT 表示还没与terminal连上，其它为失败
	/************************************************************************/
	virtual NET_ERROR LanOpen() = 0;

	/************************************************************************/
	/*关闭跨终端传输功能 ，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanClose() = 0;


	/************************************************************************/
	/* 发送广播
	/* ，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanSendBroadcast() = 0;

	/************************************************************************/
	/* 查询本机设备状态
	/* pLocalDeciveInfo		 [OUT] 用于接收返回的本机状态
	/* 返回值    返回0表示成功；，返回 E_NET_NO_INIT 表示还没与terminal连上，其他值表示失败 
	/************************************************************************/
	virtual NET_ERROR LanQueryHostDeviceInfo(LOCALDEVICEINFO* pLocalDeciveInfo) = 0;


	/************************************************************************/
	/* 查询设备信息
	/* guid		 [IN]  40位设备标识码，当其值为NULL时查询所有设备；不为NULL查询指定标识码的设备，此时必须以'\0'结束
	/* buf		 [OUT] 用于接收返回的设备信息缓存区
	/* len		 [IN/OUT]  缓存区的长度
	/* 返回值    返回0表示成功，且len返回设备个数；返回E_NET_BUFFER_NOT_ENOUGH（-15）表示缓冲区大小不够，并且len将返回所需缓冲区大小；返回E_NET_NO_DEVICE（-39），表示没有设备可查询；其他值表示失败，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanQueryDevices(char* guid, char* buf, int &len) = 0;

	/************************************************************************/
	/* 查询任务信息
	/* path		 [IN]  任务路径，当其值为NULL时查询所有任务；不为NULL查询指定路径的任务，此时必须以'\0'结束
	/* buf		 [OUT] 用于接收返回的任务信息缓存区
	/* len		 [IN/OUT]  缓存区的长度
	/* 返回值    返回0表示成功，返回E_NET_BUFFER_NOT_ENOUGH（-15）表示缓冲区大小不够，并且len将返回所需缓冲区大小；返回E_NET_NO_LAN_TASK（-39），表示没有设备可查询；其他值表示失败，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanQueryTasks(wchar_t* path, char* buf, int &len) = 0;

	/************************************************************************/
	/* 响应连接请求
	/* guid		 [IN]  40位的设备唯一标识符，以'\0'结束
	/* bAction	 [OUT] 响应动作，true表示接受连接，false表示拒绝连接
	/* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanResponseAction(char* guid,bool bAction = false) = 0;

	/************************************************************************/
	/* 发送请求连接
	/* guid		 [IN]  40位的设备唯一标识符，以'\0'结束
	/* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanRequestConnect(char* guid) = 0;

	/************************************************************************/
	/* 添加文件
	/* guid		 [IN]  40位的设备唯一标识符，以'\0'结束
	/* path		 [IN]  文件的路径，此时必须以'\0'结束
	/* type		 [OUT] 文件類型
	/* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanAddFile(char* guid,wchar_t* path,int type) = 0;

	/************************************************************************/
	/* 处理发送文件
	/* guid		 [IN]  40位的设备唯一标识符，以'\0'结束
	/* path		 [IN]  文件任务路径，必须以'\0'结束
	/* bAction	 [OUT] 响应动作，DEVICE_SEND_FILE表示发送文件，DEVICE_CANCEL_FILE表示取消发送文件
	/* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanHandleSendFile(char* guid,wchar_t* path,int bAction) = 0;

	/************************************************************************/
	/* 取消接收文件
	/* guid		 [IN]  40位的设备唯一标识符，以'\0'结束
	/* path		 [IN]  指定要取消的文件（可能是路径名），此时必须以'\0'结束
	/* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual	NET_ERROR LanCancelRecvFile(char* guid,wchar_t* path/*,bool bAction = true*/) = 0;

	/************************************************************************/
	/* 断开连接
	/* guid		 [IN]  40位的设备唯一标识符，以'\0'结束
	/* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanDisconnect(char* guid) = 0;

	///************************************************************************/
	///* 发送图标
	///* buf		 [IN]  图标数据缓存区
	///* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	///************************************************************************/
	//virtual NET_ERROR LanSendIcon(char*buf,int len) = 0;

	/************************************************************************/
	/* 主动方开始接收一个文件
	/* guid		 [IN]  40位的设备唯一标识符，以'\0'结束
	/* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual	NET_ERROR LanStartRecvFile(char* guid) = 0;


	/************************************************************************/
	/* 删除一个文件
	/* guid		 [IN]  40位的设备唯一标识符，以'\0'结束
	/* path		 [IN]  文件任务路径，必须以'\0'结束
	/* type		 [IN]  type为1表示删除下载文件，type为0表示删除上传文件
	/* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	virtual NET_ERROR LanDeleteFile(char* guid,wchar_t* path,int type) = 0;

	/************************************************************************/
	/* 关闭后台运行 -- 当调用 LanOpen 返回 E_NET_LAN_OPEN（-41） 检测到后台运行时，如果要打开跨终端界面，必须要调用 LanCloseRunBackrground 关闭后台之后，才能显示界面
	/* 返回值    返回E_NET_OK（0）表示成功，返回E_NET_FALSE表示失败，E_NET_ERROR_PARA表示参数错误，返回 E_NET_NO_INIT 表示还没与terminal连上
	/************************************************************************/
	//virtual NET_ERROR LanCloseRunBackrground() = 0;


	/************************************************************************/
	/* 刷新用户信息
	/* 
	/************************************************************************/
	virtual NET_ERROR RefreshUserInfo() = 0;



	/************************************************************************/
	/* 设置Task的状态为等待                                         */
	/************************************************************************/
	virtual NET_ERROR SetTaskWaiting(const char *pHash) = 0;


	/************************************************************************/
	/* 第三方帐号登录快播
	/* pUserName	[IN] 用户名,不超过32字节
	/* pSessionID	[IN] 临时Session ID,固定8字节
	/* iType		[IN] 第三方APP类型:1=>QQ,2=>新浪微博
	/************************************************************************/
	virtual NET_ERROR UserLoginViaThirdApp(const char *pUserName,const char *pSessionID,int iType) = 0;


	/************************************************************************/
	/* 设置5秒加速开关
	/* bOpenAcc		[IN] 是否使用5秒加速
	/************************************************************************/
	virtual NET_ERROR SetOpenAccelerate(const bool bOpenAcc) = 0;


	/************************************************************************/
	/* 设置Cache服务开关
	/* bOpenCache	[IN] 是否使用CacheServer
	/************************************************************************/
	virtual NET_ERROR SetOpenCache(const bool bOpenCache) = 0;


	/************************************************************************/
	/* 设置Miner服务开关
	/* bOpenMiner	[IN] 是否使用MinerServer
	/************************************************************************/
	virtual NET_ERROR SetOpenMiner(const bool bOpenMiner) = 0;

	/************************************************************************/
	/* 查询Qvod服务速度
	/* pstrHash		[IN]  任务hash,40位
	/* nAccSpeed	[OUT] 5秒加速下载速度
	/* nCacheSpeed	[OUT] CacheServer下载速度
	/* nMinerSpeed	[OUT] MinerServer下载速度
	/* nCloudSpeed	[OUT] 云加速下载速度
	/************************************************************************/
	virtual NET_ERROR GetServerAccSpeed(const char *pstrHash, unsigned int &nAccSpeed, unsigned int &nCacheSpeed, unsigned int &nMinerSpeed, unsigned int &nCloudSpeed) = 0;

	/***************************************************************************************	
	SetPlayingPos		设置当前的播放位置
	pstrHash			[IN]  要设置的任务hash
	pos					[IN]  要设置的位置，单位为Byte
	***************************************************************************************/
	virtual NET_ERROR SetPlayingPos(const char *pstrHash,__int64 pos) = 0;
};
#endif