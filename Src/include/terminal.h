#ifndef _TERMINAL_H_2_
#define _TERMINAL_H_2_

#include <string>
#include "qvod_string.h"
using namespace std;
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
#ifdef SUPERNODE
	char keycode[20];
#else
	unsigned char upratelimit;
	unsigned char dnratelimit;
	unsigned short reserved1;
	unsigned int reserved2[4];
#endif
//	string	path;	//系统默认保存路径
};

struct TaskCfg
{
	string filepath;
	string metafile;
	unsigned char hash[20];
	long maketime;
	UCHAR uploadlimit;//上传限速
	UCHAR peerslmt;//连接数限制
	UCHAR seedslmt;//外部种子数限制
	UCHAR status;//0:stop 1:run 2:pause
	UCHAR type;
	UCHAR bcheckkey;//0:no 1:yes bit0 防盗链开关;bit1 文件不保存开关
	string strRefUrl;	//引用页地址	
	unsigned int vipacclen;		//高速通道下载长度
	unsigned int vipaccstatus;	//高速通道服务器状态
	string strHttpOrgUrl;	//Http源下载地址
	string strHttpKeyUrl;	//Http源中固定的部分，可以标识一个http视频
	bool	bCopyFullFile;	//下载完后拷贝到下载路径
	UCHAR	copystatus;		//拷贝的状态，0：未拷贝；1：拷贝成功；2：拷贝失败
	string strCopyPath;		//拷贝的路径
	string strKey;				// 任务文件密钥key信息
	TaskCfg()
	{
		maketime = 0;
		uploadlimit = 0;
		peerslmt = 0;
		seedslmt = 0;
		status = 0;
		type = 0;
		bcheckkey = 0;
		vipacclen = 0;
		vipaccstatus = 0;
		bCopyFullFile = false;
		copystatus = 0;
	}
};

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
struct CmdBOBOHUSetDefaultDownPath
{
	CmdHeader header;
	char path[256];
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
struct CmdCreateQVODTask
{
	CmdHeader header;
	char hash[40];
	char reserved[3];//C字符串形式，下同
	char filename[1];//C字符串形式，下同
};
struct CmdCreateBOBOHUQVODTask
{
	CmdHeader header;
	char hash[40];
	int filesize;
	int type;//1 qvod，2 http
	char filename[256];//C字符串形式，下同
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
struct CmdBOBOHUDeleteTask
{
	CmdHeader header;
	char hash[40];
	int type;
};
struct CmdBOBOHUDeleteTaskRsp
{
	CmdRsp rsp;	
	char hash[40];
	int type;
};
struct CmdSetTaskStatus
{
	CmdHeader header;
	UINT taskid;
	UCHAR status; //0:离线 非0：在线
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
	UINT taskid;
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
	//UINT downloadrate;
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
struct CmdBOBOHUSetRateReq
{
	CmdHeader header;
	int rate;//1(20 5),2(40 10),3,4,5,6,7,8,9(180,45),0(无限制)
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
	wchar_t path[1];
};

struct CmdExportFileEx
{
	CmdHeader header;
	UINT begindate;
	UINT enddate;
	UCHAR flag;
	wchar_t path[256];
	wchar_t serchkey[256];
	CmdExportFileEx()
	{
		header.cmd = 0;
		header.cmdlen = 0;
		begindate = 0;
		enddate = 0;
		flag = 0;
		memset(path,0,256);
		memset(serchkey,0,256);
	} 
};

struct CmdExportFileRsp
{
	CmdHeader header;
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
#endif