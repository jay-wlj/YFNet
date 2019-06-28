#ifndef _TERMINAL_H
#define _TERMINAL_H


#define PEER_QUERY_FOLDER				0x401//	查询共享的目录
#define PEER_SHARE_RESOURCE		    	0x402//	共享文件或目录
#define PEER_UNSHARE_RESOURCE			0x403//	取消文件或目录共享
#define SERVER_RELAOD_IPRULES			0x404//	重新设置IP规则
#define SERVER_SET_DEFAULT_UPLOAD_PARA	0x405//	设置文件的默认上传参数
#define SERVER_SET_UPLOAD_PARA			0x406//	设置文件的上传参数
#define PEER_CREATE_TVVODTASK	        0x407//	根据名字和HASH进行TV下载
#define PEER_CREATE_BTVODTASK	    	0x408//	根据种子文件进行BT下载
#define PEER_SET_TASK_STATUS			0x409//	设置任务运行状态
#define PEER_QUERY_UPLOADING			0x40A//	查询正在上传的任务
#define PEER_QUERY_TASK_PEERS			0x40B
#define SERVER_EXPORT_FILE				0x40C//
#define SERVER_QUERY_GLOBALINFO	    	0x40D//	查询全局任务信息
#define PEER_QUERY_DOWNLOADING          0x40E
#define PEER_QUEYR_FILEINFO             0x40F//查询任务文件详细信息
#define PEER_SET_FILEINDEX              0x410//设置播放文件的序号
#define PEER_QUERY_TASK_BYPATH			0x411
#define PEER_CREATE_BTSHARETASK			0x412
#define PEER_QUERY_BTSHARETASK          0x413
#define SERVER_SET_ADURL                0x414//替换广告链接
#define PEER_SET_DEFAULT_DOWNPATH       0X415//默认下载目录
#define SERVER_SET_KEYCODE              0x416//设置防盗链口令
#define PEER_SET_RATE                   0x417//设置上传下载速率
#define PEER_QUERY_TASK_BYHASH          0x418
#define PEER_QUERY_BITFIELD             0x419
#define PEER_COPY_FIRSTPIECE            0x41A//拷贝第一片数据
#define PEER_QUERY_TASK_BYID			0x41B
#define SERVER_RELAOD_DNRULES			0x41C//	重新设置DNS规则
#define PEER_QUERY_TORRENT_BYID			0X41D



#define EXPORT_BY_PATH			0x01
#define EXPORT_BY_DATE			0x02
#define EXPORT_BY_NAME			0x04
#define EXPORT_ALL				0x00
#define PEER_QUERY_TASK_BYFOLDER		0x41F
#define PEER_QUERY_BUILD_PROCESS		0x421//查询共享进度标准请求 回复struct CmdQueryBulidProcessRsp


#define NETBAR_QUERY_FLEX				0x501
#define NETBAR_UPDATE_INI				0x502

#define NETBAR_NETREAD					0x503
#define NETBAR_NETWRITE					0x504

struct GlobalCfg
{
	char peerid[20];
	UINT trackerip;
	UCHAR uploadlimit;//上传限速
	UCHAR peerslmt;//连接数限制
	UCHAR seedslmt;//外部种子数限制
	UCHAR connectinglmt;
	USHORT httpport;//HTTP监听端口
	USHORT httplimited;//HTTP限速
#ifdef SUPERNODE
	char keycode[20];
#else
	unsigned char upratelimit;
	unsigned char dnratelimit;
	unsigned short reserved1;
	unsigned int reserved2[4];
#endif

};
//struct TaskCfg
//{
//	std::string filepath;
//	std::string metafile;
//	unsigned char hash[20];
//	long maketime;
//	UCHAR uploadlimit;//上传限速
//	UCHAR peerslmt;//连接数限制
//	UCHAR seedslmt;//外部种子数限制
//	UCHAR status;//0:stop 1:run 2:pause
//	UCHAR type;
//	UCHAR bcheckkey;//0:no 1:yes      bit0 防盗链开关；bit1 文件不保存开关
//	
//};

struct TASKINFO
{
	char hash[40];
	UINT taskid;
	UINT ip;
	UINT hits;
	UINT hifilelen;
	UINT lofilelen;//字节
	UINT htotaldownload;//字节
	UINT ltotaldownload;//字节
	UINT uploadlen;//字节
	UINT downloadlen;//字节
	UINT uploadrate;//字节/秒
	UINT downloadrate;//字节/秒
	UCHAR uploadlimit;//最大上传多少K
	UCHAR maxpeers;//最大连接数限制
	UCHAR maxseedsrefer;//外部最大种子数限制
	UCHAR status;//1:恢复，2:暂停 3:取消, 4:设置播放-已经有任务的播放要先设置成播放
	UCHAR nseeds;//当前种子数
	UCHAR npeers;//当前连接数
	UCHAR tasktype;//任务类型，3：上传，其它下载
	UCHAR bshared;//是否被共享
	USHORT files;//文件个数
	UCHAR bcheckkey;//0:no 1:yes

	//UCHAR reserved;
	char filename[1];//C字符串形式，下同
};
struct FILEINFO
{
	UINT filelen;
	UINT downloadlen;
	USHORT index;
	char reserved;
	char filename[1];

};
struct PEERINFO
{
	char peerid[20];
	UINT uploadlen;//字节
	UINT downloadlen;//字节
	UINT uploadrate;//字节/秒
	UINT downloadrate;//字节/秒
	UINT ip;
	USHORT port;
	char nattype;
	char reserved[1];
	
};
struct ExportTask
{
	UINT filesize;
	UINT createtime;
	char hash[40];
	char reserved[3];
	char filename[1];
};
struct CmdHeader
{
	UINT cmdlen;
	UINT cmd;
};
struct CmdRsp
{
	CmdHeader header;
	char result;
	char reserved[3];
};
struct CmdSetKeycode
{
	CmdHeader header;
	UINT taskid;
	char mask;
	char bcheckkey;
	char reserved;
	char path[1];
};
struct CmdQueryFolder
{
	CmdHeader header;
	char reserved[3];
	char folder[1];
};
struct CmdQueryFolderRsp
{
	CmdRsp rsp;
	UINT filenum;
	//C字符串形式序列
};
struct CmdQueryTorrent
{
	CmdHeader header;
	UINT taskid;
};
struct CmdQueryTorrentRsp
{
	CmdRsp rsp;
	UINT taskid;
	char reserved[3];
	char path[1];
};
struct CmdReloadIPRules
{
	CmdHeader header;
};
struct CmdShareResource
{
	CmdHeader header;
	char reserved[3];
	char resource[1];//C字符串形式，下同
};

struct CmdSetDefaultUploadPara
{
	CmdHeader header;
	UCHAR uploadlimit;
	UCHAR maxpeers;
	UCHAR maxseedsrefer;
	UCHAR reserved1;
	USHORT httpport;//HTTP监听端口
	USHORT httplimited;//HTTP限速
	char keycode[20];
};
struct CmdSetDefaultDownPath
{
	CmdHeader header;
	UCHAR connectinglmt;
	char reserved[2];
	char path[1];
};
struct CmdSetUploadPara
{
	CmdHeader header;
	UINT taskid;//0 表示设置一个目录下所有任务，否则设置该ID下的任务
	UCHAR uploadlimit;
	UCHAR maxpeers;
	UCHAR maxseedsrefer;
	char path[1];//目录名

};
struct CmdSetADURL
{
	CmdHeader header;
	UINT taskid;
	char reserved[2];
	char pathurl[2];
};
struct CmdQueryPeers
{
	CmdHeader header;
	UINT taskid;
	char HASH[40];
};
struct CmdQueryPeersRsp
{
	CmdRsp rsp;	
	UINT taskid;
	//PEERINFO peer[];
};
struct CmdTVVODTask
{
	CmdHeader header;
	char hash[40];
	char reserved[3];//C字符串形式，下同
	char filename[1];//C字符串形式，下同
};
struct CmdBTVODTask
{
	CmdHeader header;
	char reserved[2];//C字符串形式，下同
	char filename[2];//C字符串形式，下同
	
};
struct CmdCreateTaskRsp//创建任务回应
{
	CmdRsp rsp;	
	UINT taskid;
};
struct CmdSetTaskStatus
{
	CmdHeader header;
	UINT taskid;
	UCHAR status; //1:恢复，2:暂停 3:取消, 4:设置播放-已经有任务的播放要先设置成播放
	UCHAR reserved[3];
};

struct CmdSetTaskStatusRsp
{
	CmdRsp rsp;	
	UINT taskid;
};

struct CmdQueryTasks
{
	CmdHeader header;
	UINT taskid;
	UCHAR reserved[3];
	char path[1];
};
struct CmdQueryTaskByHash
{
	CmdHeader header;
	char hash[40];
};
struct CmdQueryTasksRsp
{
	CmdRsp rsp;		
	UINT   taskid;
	//TASKINFO task[];
};
struct CmdQueryFiles
{
	CmdHeader header;
	UINT taskid;
};
struct CmdQueryFilesRsp
{
	CmdRsp rsp;		
	UINT taskid;
	//FILEINFO file[];
};
struct CmdQueryBitfield
{
	CmdHeader header;
	UINT taskid;
	UINT playpos;
};
struct CmdQueryBitfieldRsp
{
	CmdRsp rsp;
	UINT taskid;	
	//UINT piecelen;
	//UINT piecenum;
	//UINT htotaldownload;
	//UINT ltotaldownload;
	//UINT downrate;
	//UCHAR bitfield[];
};
struct CmdCopyFirstPiece
{
	CmdHeader header;
	UINT taskid;
};
struct CmdCopyFirstPieceRsp
{
	CmdRsp rsp;
	UINT taskid;
	//UINT piecelen;
	//char buf[];
};
struct CmdSetFileIndex
{
	CmdHeader header;
	UINT taskid;
	UCHAR index;
	UCHAR reserved[3];
};
struct CmdSetRateReq
{
	CmdHeader header;
	UCHAR dnratelimit;
	UCHAR upratelimit;
	UCHAR reserved[2];

};
struct CmdQueryGlobalInfo
{
	CmdHeader header;
};
struct CmdQueryGlobalInfoRsp
{
	CmdHeader header;
	char peerid[20];
	UINT uploadlen;
	UINT downloadlen;
	UINT uploadrate;
	UINT downloadrate;
	UINT tasknum;
	UINT sharednum;
	USHORT httpport;//HTTP监听端口
	USHORT httplimited;//HTTP限速
	UCHAR uploadlimit;//最大上传多少K
	UCHAR maxpeers;
	UCHAR maxseedsrefer;
	UCHAR status;//0:stop 1:run 2:pause
	char keycode[20];
	UINT hitrate;
};
struct CmdExportFile
{
	CmdHeader header;
	UINT begindate;
	UINT enddate;
	UCHAR flag;
	char reserved[2];
	char path[1];
};
struct CmdExportFileRsp
{
	CmdHeader header;
};
struct CmdQueryTorrentpath
{
	CmdHeader header;
	UINT	  taskid;
};
struct CmdQueryTorrentpathRsp
{
	CmdRsp	  header;
	UINT	  taskid;
	char	  reserved[3];
	char	  path[1];
};

struct IpInfo
{
	UINT ip;
	UINT download;
	UINT upload;
	UINT downrate;
	UINT uprate;
	UINT logtime;
	IpInfo(){
		ip = 0;
		download = upload = 0;
		downrate = uprate = 0;
	}
	IpInfo& operator +=(IpInfo& ipinfo)
	{
		ip = ipinfo.ip;
		download += ipinfo.download;
		upload += ipinfo.upload;
		downrate += ipinfo.downrate;
		uprate += ipinfo.uprate;
		return *this;
	}
};
struct CmdQueryFlexRsp
{
	CmdRsp rsp;
	UINT totaldownload;
	UINT totalupload;
	UINT totaldownrate;
	UINT totaluprate;
	UINT ipnum;
	//IpInfo[]
};

struct CmdQueryTaskByIP
{
	CmdHeader header;
	UINT ip;

};
struct CmdQueryTaskByIPRsp
{
	CmdRsp rsp;
	UINT ip;
	//TASKINFO task[];
	
};

struct NetReadFile
{
	CmdHeader head;
	UINT from;
	UINT len;
	char filename[256];
	
};
struct NetReadFileRsp
{
	CmdRsp rsp;
	UINT readlen;
	//char data[]
};
struct NetWriteFile
{
	CmdHeader head;
	UINT from;
	UINT len;
	char filename[256];
	char mode[4];
	//char data[]
	
};
struct NetWriteFileRsp
{
	CmdRsp rsp;
	UINT writelen;
};

struct CmdQueryBulidProcess
{
	CmdHeader header;
	char filename[260];
};

struct CmdQueryBulidProcessRsp
{
	CmdHeader header;
	USHORT percent;
	USHORT reserved;
	char filename[260];
};

struct CmdCreateQVODTask
{
	CmdHeader header;
	char hash[40];
	char reserved[3];
	char filename[1];
};



#endif