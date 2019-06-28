#ifndef _TERMINAL_H
#define _TERMINAL_H


#define PEER_QUERY_FOLDER				0x401//	��ѯ�����Ŀ¼
#define PEER_SHARE_RESOURCE		    	0x402//	�����ļ���Ŀ¼
#define PEER_UNSHARE_RESOURCE			0x403//	ȡ���ļ���Ŀ¼����
#define SERVER_RELAOD_IPRULES			0x404//	��������IP����
#define SERVER_SET_DEFAULT_UPLOAD_PARA	0x405//	�����ļ���Ĭ���ϴ�����
#define SERVER_SET_UPLOAD_PARA			0x406//	�����ļ����ϴ�����
#define PEER_CREATE_TVVODTASK	        0x407//	�������ֺ�HASH����TV����
#define PEER_CREATE_BTVODTASK	    	0x408//	���������ļ�����BT����
#define PEER_SET_TASK_STATUS			0x409//	������������״̬
#define PEER_QUERY_UPLOADING			0x40A//	��ѯ�����ϴ�������
#define PEER_QUERY_TASK_PEERS			0x40B
#define SERVER_EXPORT_FILE				0x40C//
#define SERVER_QUERY_GLOBALINFO	    	0x40D//	��ѯȫ��������Ϣ
#define PEER_QUERY_DOWNLOADING          0x40E
#define PEER_QUEYR_FILEINFO             0x40F//��ѯ�����ļ���ϸ��Ϣ
#define PEER_SET_FILEINDEX              0x410//���ò����ļ������
#define PEER_QUERY_TASK_BYPATH			0x411
#define PEER_CREATE_BTSHARETASK			0x412
#define PEER_QUERY_BTSHARETASK          0x413
#define SERVER_SET_ADURL                0x414//�滻�������
#define PEER_SET_DEFAULT_DOWNPATH       0X415//Ĭ������Ŀ¼
#define SERVER_SET_KEYCODE              0x416//���÷���������
#define PEER_SET_RATE                   0x417//�����ϴ���������
#define PEER_QUERY_TASK_BYHASH          0x418
#define PEER_QUERY_BITFIELD             0x419
#define PEER_COPY_FIRSTPIECE            0x41A//������һƬ����
#define PEER_QUERY_TASK_BYID			0x41B
#define SERVER_RELAOD_DNRULES			0x41C//	��������DNS����
#define PEER_QUERY_TORRENT_BYID			0X41D



#define EXPORT_BY_PATH			0x01
#define EXPORT_BY_DATE			0x02
#define EXPORT_BY_NAME			0x04
#define EXPORT_ALL				0x00
#define PEER_QUERY_TASK_BYFOLDER		0x41F
#define PEER_QUERY_BUILD_PROCESS		0x421//��ѯ������ȱ�׼���� �ظ�struct CmdQueryBulidProcessRsp


#define NETBAR_QUERY_FLEX				0x501
#define NETBAR_UPDATE_INI				0x502

#define NETBAR_NETREAD					0x503
#define NETBAR_NETWRITE					0x504

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

};
//struct TaskCfg
//{
//	std::string filepath;
//	std::string metafile;
//	unsigned char hash[20];
//	long maketime;
//	UCHAR uploadlimit;//�ϴ�����
//	UCHAR peerslmt;//����������
//	UCHAR seedslmt;//�ⲿ����������
//	UCHAR status;//0:stop 1:run 2:pause
//	UCHAR type;
//	UCHAR bcheckkey;//0:no 1:yes      bit0 ���������أ�bit1 �ļ������濪��
//	
//};

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
	UCHAR status;//1:�ָ���2:��ͣ 3:ȡ��, 4:���ò���-�Ѿ�������Ĳ���Ҫ�����óɲ���
	UCHAR nseeds;//��ǰ������
	UCHAR npeers;//��ǰ������
	UCHAR tasktype;//�������ͣ�3���ϴ�����������
	UCHAR bshared;//�Ƿ񱻹���
	USHORT files;//�ļ�����
	UCHAR bcheckkey;//0:no 1:yes

	//UCHAR reserved;
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
struct CmdSetUploadPara
{
	CmdHeader header;
	UINT taskid;//0 ��ʾ����һ��Ŀ¼���������񣬷������ø�ID�µ�����
	UCHAR uploadlimit;
	UCHAR maxpeers;
	UCHAR maxseedsrefer;
	char path[1];//Ŀ¼��

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
	CmdRsp rsp;	
	UINT taskid;
};
struct CmdSetTaskStatus
{
	CmdHeader header;
	UINT taskid;
	UCHAR status; //1:�ָ���2:��ͣ 3:ȡ��, 4:���ò���-�Ѿ�������Ĳ���Ҫ�����óɲ���
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