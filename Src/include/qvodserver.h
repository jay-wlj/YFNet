#ifndef _TERMINAL_H_
#define _TERMINAL_H_



typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int	UINT;


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

	char keycode[20];


};
struct TaskCfg
{
	std::string filepath;
	std::string metafile;
	unsigned char hash[20];
	long maketime;
	UCHAR uploadlimit;//上传限速
	UCHAR peerslmt;//连接数限制
	UCHAR seedslmt;//外部种子数限制
	UCHAR status;//0:stop 1:run 2:pause
	UCHAR type;
	UCHAR bcheckkey;//0:no 1:yes      bit0 防盗链开关；bit1 文件不保存开关
	
};

struct TASKINFO
{
	char hash[40];
	UINT taskid;
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
	UCHAR status;//0:stop 1:run 2:pause
	UCHAR nseeds;//当前种子数
	UCHAR npeers;//当前连接数
	UCHAR tasktype;//任务类型，3：上传，其它下载
	UCHAR bshared;//是否被共享
	USHORT files;//文件个数
	UCHAR bcheckkey;//0:no 1:yes
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
struct CmdQueryFolder
{
	CmdHeader header;
	char reserved[3];
	char folder[1];
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
struct CmdQueryFolderRsp
{
	CmdHeader header;
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
	CmdHeader header;
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

struct CmdRsp
{
	CmdHeader header;
	char result;
	char reserved[3];
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
struct CmdSetUploadPara
{
	CmdHeader header;
	UINT taskid;//0 表示设置一个目录下所有任务，否则设置该ID下的任务
	UCHAR uploadlimit;
	UCHAR maxpeers;
	UCHAR maxseedsrefer;
	char path[1];//目录名

};
struct CmdSetDefaultDownPath
{
	CmdHeader header;
	UCHAR connectinglmt;
	char reserved[2];
	char path[1];
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
	CmdHeader header;
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
	CmdHeader header;
	UINT taskid;
	char result;
	char reserved[3];
	
};
struct CmdSetTaskStatus
{
	CmdHeader header;
	UINT taskid;
	UCHAR status; //0:离线 非0：在线
	UCHAR reserved[3];
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
	CmdHeader header;
	//TASKINFO task[];
};
struct CmdQueryFiles
{
	CmdHeader header;
	UINT taskid;
};
struct CmdQueryFilesRsp
{
	CmdHeader header;
	//FILEINFO file[];
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
#endif