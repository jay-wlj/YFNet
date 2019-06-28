#ifndef _TERMINAL_H_
#define _TERMINAL_H_



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

	char keycode[20];


};
struct TaskCfg
{
	std::string filepath;
	std::string metafile;
	unsigned char hash[20];
	long maketime;
	UCHAR uploadlimit;//�ϴ�����
	UCHAR peerslmt;//����������
	UCHAR seedslmt;//�ⲿ����������
	UCHAR status;//0:stop 1:run 2:pause
	UCHAR type;
	UCHAR bcheckkey;//0:no 1:yes      bit0 ���������أ�bit1 �ļ������濪��
	
};

struct TASKINFO
{
	char hash[40];
	UINT taskid;
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
	//C�ַ�����ʽ����
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
	char resource[1];//C�ַ�����ʽ����ͬ
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
	USHORT httpport;//HTTP�����˿�
	USHORT httplimited;//HTTP����
	char keycode[20];
};
struct CmdSetUploadPara
{
	CmdHeader header;
	UINT taskid;//0 ��ʾ����һ��Ŀ¼���������񣬷������ø�ID�µ�����
	UCHAR uploadlimit;
	UCHAR maxpeers;
	UCHAR maxseedsrefer;
	char path[1];//Ŀ¼��

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
	char reserved[3];//C�ַ�����ʽ����ͬ
	char filename[1];//C�ַ�����ʽ����ͬ
};
struct CmdBTVODTask
{
	CmdHeader header;
	char reserved[2];//C�ַ�����ʽ����ͬ
	char filename[2];//C�ַ�����ʽ����ͬ
	
};
struct CmdCreateTaskRsp//���������Ӧ
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
	UCHAR status; //0:���� ��0������
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
	char path[1];
};
struct CmdExportFileRsp
{
	CmdHeader header;
};
#endif