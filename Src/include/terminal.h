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
	UCHAR uploadlimit;//�ϴ�����
	UCHAR peerslmt;//����������
	UCHAR seedslmt;//�ⲿ����������
	UCHAR connectinglmt;
	USHORT httpport;//HTTP�����˿�
	USHORT httplimited;//HTTP����
#ifdef SUPERNODE
	char keycode[20];
#else
	unsigned char upratelimit;
	unsigned char dnratelimit;
	unsigned short reserved1;
	unsigned int reserved2[4];
#endif
//	string	path;	//ϵͳĬ�ϱ���·��
};

struct TaskCfg
{
	string filepath;
	string metafile;
	unsigned char hash[20];
	long maketime;
	UCHAR uploadlimit;//�ϴ�����
	UCHAR peerslmt;//����������
	UCHAR seedslmt;//�ⲿ����������
	UCHAR status;//0:stop 1:run 2:pause
	UCHAR type;
	UCHAR bcheckkey;//0:no 1:yes bit0 ����������;bit1 �ļ������濪��
	string strRefUrl;	//����ҳ��ַ	
	unsigned int vipacclen;		//����ͨ�����س���
	unsigned int vipaccstatus;	//����ͨ��������״̬
	string strHttpOrgUrl;	//HttpԴ���ص�ַ
	string strHttpKeyUrl;	//HttpԴ�й̶��Ĳ��֣����Ա�ʶһ��http��Ƶ
	bool	bCopyFullFile;	//������󿽱�������·��
	UCHAR	copystatus;		//������״̬��0��δ������1�������ɹ���2������ʧ��
	string strCopyPath;		//������·��
	string strKey;				// �����ļ���Կkey��Ϣ
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
	UINT lofilelen;//�ֽ�
	UINT htotaldownload;//�ֽ�
	UINT ltotaldownload;//�ֽ�
	UINT uploadlen;//�ֽ�
	UINT downloadlen;//�ֽ�
	UINT uploadrate;//�ֽ�/��
	UINT downloadrate;//�ֽ�/��
	UCHAR uploadlimit;//����ϴ�����K
	UCHAR maxpeers;//�������������
	UCHAR maxseedsrefer;//�ⲿ�������������
	UCHAR status;//0:stop 1:run 2:pause
	UCHAR nseeds;//��ǰ������
	UCHAR npeers;//��ǰ������
	UCHAR tasktype;//�������ͣ�3���ϴ�����������
	UCHAR bshared;//�Ƿ񱻹���
	USHORT files;//�ļ�����
	UCHAR bcheckkey;//0:no 1:yes
	char filename[1];//C�ַ�����ʽ����ͬ
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
	UINT uploadlen;//�ֽ�
	UINT downloadlen;//�ֽ�
	UINT uploadrate;//�ֽ�/��
	UINT downloadrate;//�ֽ�/��
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
	//C�ַ�����ʽ����
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
	char resource[1];//C�ַ�����ʽ����ͬ
};

struct CmdSetDefaultUploadPara
{
	CmdHeader header;
	UCHAR uploadlimit;
	UCHAR maxpeers;
	UCHAR maxseedsrefer;
	UCHAR reserved1;
	USHORT httpport;//HTTP�����˿�
	USHORT httplimited;//HTTP����
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
	char reserved[3];//C�ַ�����ʽ����ͬ
	char filename[1];//C�ַ�����ʽ����ͬ
};
struct CmdCreateBOBOHUQVODTask
{
	CmdHeader header;
	char hash[40];
	int filesize;
	int type;//1 qvod��2 http
	char filename[256];//C�ַ�����ʽ����ͬ
};
struct CmdBTVODTask
{
	CmdHeader header;
	char reserved[2];//C�ַ�����ʽ����ͬ
	char filename[2];//C�ַ�����ʽ����ͬ
	
};
struct CmdCreateTaskRsp//���������Ӧ
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
	UCHAR status; //0:���� ��0������
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
	int rate;//1(20 5),2(40 10),3,4,5,6,7,8,9(180,45),0(������)
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
	USHORT httpport;//HTTP�����˿�
	USHORT httplimited;//HTTP����
	UCHAR uploadlimit;//����ϴ�����K
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