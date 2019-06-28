#pragma once
#include <vector>
#include "Media.h"
#define ETASKTYPE
enum e_TaskType
{
	tt_qvod,					//��׼qvod����
	tt_bt,						//bt����
	tt_qlive,					//ֱ������
	tt_http,					//http����
	tt_swf,						//swf����
	tt_series,					//�缯����
	tt_yunfan,					//�Ʒ�http��Դ
	tt_web_media,				//ý�����ҳ�����ſᣬ���ӣ������յȣ�
	tt_web_list,				//�б��е���ҳ����
};

enum CreateTaskSource
{
	kTaskFromNull				= 0,
	kTaskFromRadar				= 1, // �״�
	kTaskFromPrivate			= 2, // ˽���� 
	kTaskFromInsert				= 3, // �������
	kTaskFromOnlineMedia		= 4, // ������Ƶ
	kTaskFromTuitui				= 5, // ����
	kTaskFromBarrage			= 6, // ��Ļ
	kTaskFromSeries				= 7, // �����缯
	kTaskFromQRcode				= 8, // ��ά��
	kTaskFromSeed				= 9, // �����ļ�
	kTaskFromUrl				= 10,// �����в�������URL/hash����������
};

typedef struct _TASKITEM
{
	long	nTaskID;			// �����ID�� ��������Ч
	char	szHash[41];			// ��������hash
	WORD	nTagID;				// ��������ı�ǩid
	BOOL	bRealHash;			// �Ƿ���QLiveFsOpen�ӿڷ���hashֵ

	// TODO: ֱ����������terminal��������Ҫ���´���
	e_TaskType	nType;				// ������������ 0:��ͨP2P���� 1: BT���� 2: ֱ�� 3: ftp,http,mms�� 4:�����˵��ļ� 6:swf����
	BOOL	bShare;				// �Ƿ��ǹ�������
	BOOL	bNoSave;			// �Ƿ�Ϊ������
	BOOL	bFold;				// �������б����Ƿ�����£״̬
	BOOL	bPrivate;			// �Ƿ�Ϊ˽���ļ�
	BOOL    bIsPCloud;			// ˽�������� 
	long	nFileCount;			// �ļ�����
	__int64 nTotalSize;			// ���������ܴ�С
	__int64 nDownloadSize;		// �����ش�С
	netstate netState;			// ����״̬ ��1:���� 2:���� 3:��ͣ 4:��� 5:�ȴ�)
	long	nDownrate;			// �����ٶ� �ֽ�
	long	nUprate;			// �ϴ��ٶ� �ֽ�
	long	nBitLen;			// ����λ���� ��������С
	long    lLastPlayIndex;		// ���һ�β����ļ�index
	int		nSeriesRelatedId;	// �����缯ID
	BOOL	bInsertOwner;		// ������Դ,FALSE:��TRUE,��
	__int64 nLastRecieveTime;		//���һ�ν��յ�ʱ��
	//==============================================================================
	//============================== ����ͨ��������  ================================
	long	nAccDownsize;		// (�ۼ����س���),������ͨ������ͨ�����������ص����ݳ���,��λKB
	long    nCurAccDownsize;	// (��ǰ���س���),�Ӹ���ͨ�����������ص����ݳ���,KB
	long	nAccDownrate;		// ������Ӹ���ͨ�����������ص��ٶ�,��λ�ֽ�/��(B/S)
	BOOL	bOpenHighSpeed;		// �Ƿ���������ͨ��
	int		nSpeedUsableState;	// ����ͨ������״̬ ֵΪspeed_usable_state
	CreateTaskSource	source;// ������Դ

	//net_quality netQuality;	// �������� ֻ����ʱ����
	std::vector<Media*> vpFile;	// �����ļ���Ϣ���У�ֻ�����ʶ���׺���ļ�
	CString strName;			// ������ʾ��
	CString strUrl;				// ����URL
	CString strWebTaskSeriesClassID;
	CString strWebTaskSeriesDocID;
	CString docID;
	DWORD dwWebTaskSeriesNum;
	_TASKITEM()
	{
		ZeroMemory(this, sizeof(TASKITEM) - sizeof(std::vector<Media*>) - sizeof(CString) * 2);
		bFold = TRUE;
		netState = ns_noncreate;
	}
}TASKITEM; // ����������

typedef struct _TASKITEM_SERIES : public TASKITEM
{
	_TASKITEM_SERIES()
	{
		nSeriesId = 0;
	}
	long   nSeriesId;			//�缯ID
}TASKITEM_SERIES;


typedef struct _DLLTASKINFO
{
	TCHAR			szSrcUrl[MAX_PATH*4];
	TCHAR			bstr_url[MAX_PATH];
	char			szHash[40];
	REFERENCE_TIME	lDur;
	long			lRate;
	long			lWidth;
	long			lHeight;
	DWORD			dwBufferTime;
}DLLTASKINFO;

typedef struct s_PLAY_DATA
{
	BOOL			bMute;
	int				nScale;
	int				nVolume;
	int				nSoundChannel;
	int				nStreamID; //��Ƶ���˵�ID
	BOOL			bVideoEnhance; //������ǿ
}PLAY_DATA;

enum OpenURL_type
{
	openURL_default = 0,
	openURL_localfile,
	openURL_qlive,
	openURL_DVD,
	openURL_list,
	openURL_netfile
};

typedef struct _TOPLAYER
{
	TCHAR			szURL[MAX_PATH];
	char			szHash[40];
	TCHAR			szSrcUrl[MAX_PATH];
	REFERENCE_TIME	lCtime;
	OpenURL_type	type;		// 0,��� 1,������ 2,ֱ��
	PLAY_DATA		playdata;
	TCHAR			szImgURL[MAX_PATH]; //ͼƬ��url
}TOPLAYER;

typedef struct _WEB_PLAYER_QMVDATA
{
	char			type;					// ���ݰ����ࡢ��;
	int				index;					// ���
	char			szHash[40];				// ����hash
	TCHAR			szFName[_MAX_PATH];		// ����ý���ļ�����
	TCHAR			szLPath[_MAX_PATH];		// ����ý��·��
	TCHAR			szNPath[_MAX_PATH];		// ���粥����������
	WORD			qmvTagID;				// ��Ƶ��ǩid
	TCHAR			szTitle[_MAX_PATH];		// ����
	TCHAR			szDetail[_MAX_PATH];	// ����
	TCHAR			szUrl[1024];			// ����
	__int64			llTagBegin;				// ��ǩ��ʼ������ʱ�䡱
	__int64			llTagEnd;				// ��ǩ����������ʱ�䡱
	int				imgLen;
	char			imgBuf[1];
}WEB_PLAYER_QMVDATA;

struct YunfanHttpHeader
{
	std::map<CString,CString> fields_; 
};
struct YunfanSourceInfo
{
	CString sourceWebName_;
	CString mediaTitle_;
	CString mediaSource_;
	CString dragType_;
	DWORD mediaTime_;
	__int64 mediaSize_; 
};

struct YunfanMediaInfo
{
	CString mainUrl_;
	CString mediaDur_;
	CString mediaSize_;
};
struct YunfanParseInfo
{
	YunfanHttpHeader httpHeader_;
	YunfanSourceInfo source_;
	vector<YunfanMediaInfo> medias_;
};