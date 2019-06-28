#ifndef MESSAGE_H
#define MESSAGE_H
//typedef unsigned int UINT; /* silver delete */
//typedef unsigned char BYTE; /* silver delete */
//typedef unsigned long QVOD_DWORD; /* silver delete */
#include "qvod_string.h"

#define TRACKER_LOGIN	        0x301
#define TRACKER_LOGIN_RESP  	0x301

#define TRACKER_TERMINATE     	0x302

#define TRACKER_ENQUIRE	0x303
#define TRACKER_ENQUIRE_RESP	0x303

#define TRACKER_ADDRESOURCE	       0x304
#define TRACKER_ADDRESOURCE_RESP	0x304

#define TRACKER_DELRESOURCE	     0x305
#define TRACKER_DELRESOURCE_RESP	0x305

#define TRACKER_GETPEERS    	0x306
#define TRACKER_GETPEERS_RESP	0x306

#define TRACKER_GETFILESUSERS	0x307
#define TRACKER_GETFILESUSERS_RESP	0x307


#define TRACKER_STUN	       0x311//��͸����
#define TRACKER_STUN_RESP	   0x311//��͸�����Ӧ
#define TRACKER_STUNNOTIFY  	0x312//��͸֪ͨ��

#define TRACKER_AGENT_GETPEERS	0x326//��ȡ��Դ�û�,ͬʱ�Ǽ��Լ�Ҳ�����ظ���Դ

#define CACHE_GETCACHE		0x351
#define CACHE_GETCACHE_RSP	0x351

#define TRACKER_QUERY_ISP_BYIP        0x501//��ѯip��Ӧisp

struct peerid
{
	char checksum[4];
	char version[4];
	char random[12];
};

struct msghead
{

	unsigned short len;
	unsigned short command;
	unsigned int sequence;
};

struct msgheadreq
{
	msghead head;
	char Peer_id[20];
};

struct LoginReq
{
	msghead head;
	char Peer_id[20];
	unsigned int ip;
	unsigned short listenport;
	char NAT_Type;	
	char Reserved;
};
struct LoginRsp
{
	msghead head;
	unsigned int Peer_ip;
	unsigned short Peer_port;
	unsigned char Result;
	unsigned char version;

};
struct LogoutReq
{
	msghead head;
	char Peer_id[20];
};

struct HandShakeReq
{
	msghead head;
	char Peer_id[20];
};

struct HandShakeRsp
{
	msghead head;
	unsigned char Result;
	unsigned char reserved1; 
	unsigned short reserved2;
};


struct ShareReq
{
	msghead head;
	char Peer_id [20];
	unsigned char Filehash[20];

};

#pragma pack(1)
struct ShareRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned char Result;
	unsigned char reserved1; 
	unsigned short reserved2;
};
#pragma pack()

struct UnShareReq
{
	msghead head;
	char Peer_id [20];
	unsigned char Filehash[20];

};

#pragma pack(1)
struct UnShareRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned char Result;
	unsigned char reserved1; 
	unsigned short reserved2;
};
#pragma pack()

struct DownLoadReq
{
	msghead head;
	char Peer_id [20];
	unsigned char FileHash[20];
};

#pragma pack(1)
struct DownLoadRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned char Result;
	unsigned char reserved;//�����ڵ���		
	unsigned char Peer_count;//�Ժ��ΪUNSIGNED CHAR
	unsigned char reserved2;//���Ӹ���
};
#pragma pack()

#pragma pack(1)
struct AgentGetPeerReq//��������ṹ
{
	msghead head;
	peerid id;
	unsigned char hash[20];
	unsigned int publicip;
	unsigned short listenport;//�����˿�
	unsigned char nattype;
	unsigned char reserved;
};
#pragma pack()

struct AgentGetPeerRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned int publicip;
	unsigned char result;//0:�ɹ�,1:��Դ������, 4:�汾̫��;
	unsigned char version;//0:���°汾,1:���°汾
	unsigned char peers;//peer��
	unsigned char reserved;
};


struct Peer
{
	char random[12];
	unsigned int ip;
	unsigned short port;
	unsigned char nattype;
	unsigned char protocol;//Э��汾		
};

struct GetUser
{
	msghead head;
	char Peer_id[20];
	unsigned char Filehash[20];

};
struct GetUserRsp
{
	msghead head;
	unsigned char hash[20];
	unsigned char result; // 0:�ɹ�;1:��Դ������; 2:δ��¼; 3:��Ϣ��ʽ��
	unsigned char supers;//�����ڵ���
	unsigned char count;//peer��
	unsigned char seeds;//���Ӹ���

};

//����������Ϣ���������
struct StunReq
{
	msghead head;
	peerid sourceid;//����peerid
	peerid targetid;//�Է�peerid
	unsigned char hash[20];//��ԴHASH
};
struct StunRsp
{
	msghead head; //������Ϣ�ͱ�ʾ�Է�������
	unsigned char hash[20];//��ԴHASH
	unsigned char result; // 1:�Է�������;2:δ��¼; 3:��Ϣ��ʽ��
	unsigned char reserved1; 
	unsigned short reserved2;

};
struct StunNotify
{
	msghead head;
	unsigned char hash[20];
	peerid sourceid;//Դpeerid
	unsigned int ip;//����IP
	unsigned short port; //���𷽶˿�
	unsigned char nattype;//�Է���������
	unsigned char reserved;
};

//��ѯISP��
struct GetIspID
{
	msghead header;
	char Peer_id[20];
	unsigned int ip;		//����ip
};

struct GetIspIDRsp
{
	msghead header;
	unsigned int ip;
	unsigned char isp[4];
};

//���������յ�������,Ҫ�ϱ���ͳ�Ʒ�����
struct userid
{
	unsigned int len;//�������ݵĴ�С��������������ͬ
	unsigned char id ;//Э�������
	unsigned char reserved[3];
	unsigned int uid; //�û�id
};

//����ΪBT��׼Э��
#define CHOKE 0
#define UNCHOKE 1
#define INTERESTED 2
#define NOT_INTRESTED 3
#define HAVE 4
#define BITFIELD 5
#define REQUEST 6
#define PIECE 7
#define CANCEL 8
#define METAFILEREQ 9
#define METAFILERSP 10
#define REQ_TIMEOUT 11
#define VIP_AUTHREQ 12
#define VIP_AUTHRSP 13
#define UNHAVE 14
#define USERID		109

//#define BTTRACKERREFERTO 101
//#define BTTRACKERRSP     102
//#define UDPTRACKERRSP    103

#define CMDDECLARE       0xA0    //160
#define FIXEDCMD         0xA1    //161
#define NOTIFYPEERS      0xA2    //162
#define BCUNKNOWN        0xA3    //163

#define BTGETPEER         20
#define BTTKHTTPCONNECTED 21 
#define BTTKUDPCONNECTED  22
#define BTTKUDPRESPONCE   23

#define CONNECTED	  	  30
#define DISCONNECTED  	  31
#define ACCEPTED		  32


//handshake
//�ṹ��[protocol string length] + [protocol string] + [reserved] + [info_hash] + [peer_id]

//protocol string length��1�ֽ���������Ӧ����protocol string�ĳ��ȡ�
//protocol string��Э���ʶ�ַ�����BitTorrentЭ��1.0�汾�д��ַ���Ϊ
//"BitTorrent protocol"�����Զ�Ӧ��protocol string lengthΪ19��
//reserved��8�������ֽڡ���ǰ����BitTorrentʵ�ֶ�����Ϊ0��
//info_hash��20�ֽ�SHA1 hashֵ���������ļ���info_hashһ�¡�
//peer_id��20�ֽ��ַ��������ͷ��ն�����ı�ʶ��


struct handshake
{
	unsigned char length;
	char protocol[19];//protocol ex��ʾ����
	unsigned char checkbit;//��1λ����crcУ��,����7λ�ݿ�
	unsigned char isp[4];
	char reserved[3];
	unsigned char info_hash[20];
	char peer_id[20];
};


//keep-alive
//�ṹ��[len=0000]
//û�����ݣ�ֻ��4���ֽڵ�������ֵΪ0��ÿ��2���ӷ�һ�Ρ�
#define keep_alive "\0\0\0\0"

//choke
//�ṹ��[len=0001] + [id=0]
//��ʾ��·�������޷���Ӧ��������
//ֱ�ӷ��Ͳ����ṹ
#pragma pack(1)
struct encodehead
{
	unsigned int key;//���ڽ��ܺ��沿��
	unsigned int key2;
	unsigned char paddinglen;//padding�ĳ��ȣ�0-15
	//unsigned char padding[] //���Ȳ��̶�
	//unsigned int checkkey;//���ܺ����key
	//unsigned int checkkey2;//���ܺ����key2
	//packet
};
#pragma pack()

#pragma pack(1)
typedef struct _head
{
	unsigned int len;//�������ݵĴ�С��������������ͬ
	unsigned char id ;//Э�������
}stdhead,agentmsghead,interested,not_interested,choke,unchoke;
#pragma pack()


// struct btpeers
// {
// 	stdhead head;
// 	unsigned char hash[20];
// };
#pragma pack(1)
typedef struct 
{
	stdhead head;
	unsigned char status;
}connected,accepted,disconnected;
#pragma pack()

#pragma pack(1)
struct estdhead
{
	unsigned char sign;
	unsigned int len;
	unsigned char id ;
};
#pragma pack()

#pragma pack(1)
struct econnected
{
	estdhead head;
	unsigned char status;
};
#pragma pack()
//unchoke
//�ṹ��[len=0001] + [id=1]
//��ʾ��·ͨ��������Ӧ��������
//ֱ�ӷ��Ͳ����ṹ


//interested
//�ṹ��[len=0001] + [id=2]
//��ʾ�ԶԶ˹�������ݸ���Ȥ��


//not_interested
//�ṹ��[len=0001] + [id=3]
//��interested�෴��


//have
//�ṹ��[len=0005] + [id=4] + [piece index]
//���������Զˣ��Լ�ӵ��ĳ����Ƭ��
#pragma pack(1)
struct vipauthreq
{
	unsigned int len;
	unsigned char id ;
	QVOD_INT64 sessionid;
	char username[16];
};
#pragma pack()

#pragma pack(1)
struct vipauthrsp
{
	unsigned int len;
	unsigned char id ;
	unsigned char result;//���ؽ��,0�ɹ�,1��֤ʧ��,2��������,3��������������
};
#pragma pack()

#pragma pack(1)
struct have
{
	unsigned int len;//�������ݵĴ�С��������������ͬ
	unsigned char id ;//Э�������
	unsigned int piece_index;
};
#pragma pack()

#pragma pack(1)
struct unhave
{
	unsigned int len;//�������ݵĴ�С��������������ͬ
	unsigned char id ;//Э�������
	unsigned int piece_index;
};
#pragma pack()
//bitfield
//�ṹ��[len=0001+X] + [id=5] + [bitfield]
//���������֮�󣬴���Ϣ��Ӧ�ñ����͡���������ն�û���κη�Ƭ���ɲ��ط��ʹ���Ϣ��
//ÿ��bitλ����һ����Ƭ������һ���ֽڶ�Ӧ8����Ƭ��
//��һ���ֽ����bitλ�����0����Ƭ���������ơ������ӦbitΪ1����ʾ�и÷�Ƭ��0���ʾû�С�
//���bitfieldĩβ�ж����bits����Ϊ0��bitfield���ֽڳ������Ƭ�Ķ������

#pragma pack(1)
struct bitfield
{
	unsigned int len;//�������ݵĴ�С��������������ͬ
	unsigned char id ;//Э�������
	QVOD_BYTE bit[1];
};
#pragma pack()

//request
//�ṹ��[len=0013] + [id=6] + [index] + [begin] + [length]
//��������һ����Ƭ�е�һ�����ݿ顣indexָ��һ����Ƭ��beginָ����ʼ�ڸ÷�Ƭ�ĵڼ���byteλ�ã�
//lengthָ������೤�����ݡ������length����С�ڷ�Ƭ�Ĵ�С��
//���͵ķ�Ƭ��СΪ256K������length����С�ڸ�ֵ��һ��ָ��Ϊ32K��
//index,begin,length����4���ֽ�������

#pragma pack(1)
struct request
{
	unsigned int len;//�������ݵĴ�С��������������ͬ
	unsigned char id ;//Э�������
	unsigned int index;
	unsigned int begin;
	unsigned int length;
};
#pragma pack()

//piece
//�ṹ��[len=0009+X] + [id=7] + [index] + [begin] + [block]
//���ڷ���һ�����ݿ顣index,begin,length����4���ֽ�������index��begin������request��Ӧ��
//block���ļ���һ�����ݣ���Ȼ�����ݿ���index��Ƭ��һ���֡�

#pragma pack(1)
struct piece
{
	unsigned int len;
	unsigned char id ;
	unsigned int index;
	unsigned int begin;

};
#pragma pack()

//cancel
//�ṹ��[len=0013] + [id=8] + [index] + [begin] + [length]
//��request��Ϣ��ԣ����߶Զ˷���ĳ�����ݿ�����ء�index��begin, length�����Ӧ�Ѿ����͵�request��Ϣ��

#pragma pack(1)
struct cancel
{
	unsigned int len;
	unsigned char id ;
	unsigned int index;
	unsigned int begin;
	unsigned int length;
};
#pragma pack()

//��չЭ��
//request metafile
//�ṹ��[len=0021] + [id=9] + [info_hash]
//�ͻ���֮�䣬ʹ�ô���չ��Ϣ�����ȡmetafile�ļ���

#pragma pack(1)
struct metafilereq
{
	unsigned int len;
	unsigned char id ;
	char info_hash[20];
};
#pragma pack()

//metafile
//�ṹ��[len=0001+X] + [id=10] + [content]
//�ظ�request metafile�������ļ����ݡ����޴��ļ�������Ϊ�ա�
#pragma pack(1)
struct metafilersp
{
	unsigned int len;
	unsigned char id ;
	char content[1];
};
#pragma pack()

#pragma pack(1)
struct requesttimeout
{
	unsigned int len;
	unsigned char id ;
	request req;

};
#pragma pack()

//ֱ��Э��
#define LIVEHANDSHAKE 201
#define LIVEBITFIELD  202
#define LIVEREQUEST   203
#define LIVEPIECE     204
#define LIVETIMEOUT   205

#pragma pack(1)
struct livehandshake//size 52
{
	stdhead header;
	char protocol[19];//qlive protocl...
	unsigned char hash[20];
	char peerid[20];//QLVE0064XXXXXXXXXXXX
	unsigned char reserved[8];
};
#pragma pack()

#pragma pack(1)
struct livebitfield
{
	stdhead header;
	unsigned int first;
	unsigned int curpos;
	unsigned int last;
	unsigned int orginstarttime;
	//char* bit;
};
#pragma pack()

#pragma pack(1)
struct liverequest
{
	stdhead header;
	unsigned int index;
};
#pragma pack()

#pragma pack(1)
struct livepiece
{
	stdhead header;
	unsigned int index;
	char sha1[20];
	//char* data;
};
#pragma pack()

#pragma pack(1)
struct livetimeout
{
	stdhead header;
	unsigned int index;
};
#pragma pack()

//bitcomet��չЭ��
#pragma pack(1)
struct cmddeclare
{
	unsigned int len;
	unsigned char id;
	unsigned char code[12]; 
};
#pragma pack()

#pragma pack(1)
struct fixedcmd
{
	unsigned int len;
	unsigned char id;
	unsigned int fixed1;//Ϊ0
	unsigned int fixed2;//0x1C000000
	unsigned int fixed3;//Ϊ0
};
#pragma pack()

#pragma pack(1)
struct btpeerinfo
{
	char peerid[20];
	unsigned int ip;//0Ϊ����
	unsigned short port;
	unsigned short reserved;//Ϊ0
};
#pragma pack()

#pragma pack(1)
struct notifypeers
{
	unsigned int len;
	unsigned char id;
	unsigned int fixed1;//Ϊ0
	unsigned int fixed2;//0x1C000000
	char peerid[20];
	unsigned int ip;//0Ϊ����
	unsigned short port;
	unsigned short reserved;//Ϊ0
};
#pragma pack()

#define CONNECT 0 
#define ANNOUNCE 1
#define SCRAPE 2
#define TRACKERERROR 3

typedef struct _btconnect 
{
	QVOD_INT64 connid;
	QVOD_UINT action;
	QVOD_UINT transid;
}bthead,btconnect;
struct btconnectrsp
{
	QVOD_UINT action;
	QVOD_UINT transid;
	QVOD_INT64 connid;
};

#pragma pack(1)
struct btannounce
{
	bthead head;
	unsigned char hash[20];
	char peerid[20];
	QVOD_INT64 downloaded ;
	QVOD_INT64 left;
	QVOD_INT64 uploaded;
	int event;
	int ip; 
	int key; 
	int num_want; 
	short port;  
};
#pragma pack()

struct btannouncersp
{
	QVOD_UINT action;
	QVOD_UINT transid;
	QVOD_UINT interval;
	QVOD_UINT leechers;
	QVOD_UINT seeds;
	//	20 + 6 * n  32-bit integer  IP address  
	//	24 + 6 * n  16-bit integer  TCP port  

};

struct bterror
{
	QVOD_UINT action;
	QVOD_UINT transid;
	//	8  string  message  

};

#define AGENTSHAREFILE			0xA3
#define AGENTREQLINKRSP			0xA4
#define AGENTSHARELINKRSP		0xA5
#define AGENTREQUESTPIECE		0xA5
#define AGENTREQUESTBLOCK		0xA6
#define AGENTREMOVEFILE			0xA7
#define AGENTREQUESTBLOCKINFO   0xA8
#define AGENTREQUESTMETA	    0xA9
#define AGENTREQUESTCACHE		0xAA
#define AGENTRANGE				0xAB
#define AGENTM3U8				0xAC
#define AGENTM3U8TS				0xAD
#define AGENTUPLOAD				0xAE

#pragma pack(1)
typedef struct _agentmsgshare
{
	agentmsghead head;
	unsigned char result;//0,1,2
	unsigned int ip;
	unsigned short port;
	unsigned int sock;
	unsigned char hash[20];
	char fileurl[1024];
	char filepath[255];
	QVOD_UINT64 start;
	QVOD_UINT64 filelen;
	QVOD_UINT64 downloadlen;
}agentmsgshare;
#pragma pack()

#pragma pack(1)
typedef struct _agentreqblock
{
	agentmsghead head;
	unsigned char result;//0,1,2
	unsigned int ip;
	unsigned short port;
	unsigned int sock;
	unsigned char hash[20];
	char* pbuf;
	QVOD_UINT64 pos;
	QVOD_UINT64 len;
	unsigned char type; //1:block;2:index
}agentreqblock;
#pragma pack()

#pragma pack(1)
struct agentreqlinkrsp
{
	agentmsghead head;
	unsigned char hash[20];
	char acchash[20];
	char url[1024];
	unsigned int filelen;
};
#pragma pack()

#pragma pack(1)
struct agentrequestpiece
{
	agentmsghead head;
	unsigned char result;//0,1
	unsigned int ip;
	unsigned short port;
	unsigned int sock;
	int start;
	int length;
	int piecelen;
	char buf[65536];
};
#pragma pack()

#pragma pack(1)
struct agentrequestcache
{
	agentmsghead head;
	unsigned char result;//0,1
	unsigned int ip;
	unsigned short port;
	unsigned int sock;
	unsigned int piecelen;
	char hash[40];
	unsigned int start;
	unsigned int downlen;
	char* buf;
};
#pragma pack()

#pragma pack(1)
struct agentremovefile
{
	agentmsghead head;
	unsigned char hash[20];
};

struct agentreqblockinfo
{
	agentmsghead head;
	unsigned int ip;
	unsigned short port;
	unsigned char type;
	unsigned char reserved;
	unsigned int sock;
	unsigned char hash[20];
};
#pragma pack()

#pragma pack(1)
struct agentreqmeta
{
	agentmsghead head;
	unsigned char hash[20];
	unsigned int metalen;
	char* metabuf;
};
#pragma pack()

#pragma pack(1)
struct agentrange
{
	agentmsghead head;
	int id;
	unsigned char result;			// 0-complete, 1-partial, 2-no data, 3-http���ز���200���ض�������ʧЧ�� 4-�������Դ���
	unsigned  short errcode;	// ������
	unsigned char hash[20];
	char *buf;
	QVOD_INT64 start;
	QVOD_INT64 len;
	QVOD_INT64 filesize;
};
#pragma pack()

#pragma pack(1)
struct agentm3u8
{
	agentmsghead head;
	unsigned char result;
	unsigned short errcode;
	unsigned char hash[20];
	char *buf;
	QVOD_INT64 len;
};
#pragma pack()

#pragma pack(1)
struct agentm3u8ts
{
	agentmsghead head;
	unsigned char result;
	unsigned short errcode;
	unsigned char hash[20];
	std::string filename;
	char *buf;
	QVOD_INT64 start;
	QVOD_INT64 len;
	QVOD_INT64 filesize;
};
#pragma pack()
#endif

