#pragma once
#include <wtypes.h>

//������
typedef int STAT_ERROR;
#define STAT_ERROR_OK				0
#define STAT_ERROR_TIMEOUT			1
#define STAT_ERROR_UNVALID_NET		2
#define STAT_ERROR_UNVALID_FORMART	3
#define STAT_ERROR_GETUID_FAILED	4
#define STAT_ERROR_OTHER			5

//ͳ������
#define ST_KB_INSTALL				1
#define ST_KB_UNINSTALL				2
#define ST_KB_BUNDLED				3
#define ST_KB_HOMEPAGE_SET			4
#define ST_KB_HOMEPAGE_SET_OK		5
#define ST_BUFFERING				6
#define ST_STARTUP_TERMINAL			7
#define ST_STARTUP_PLAYER			8
#define ST_STARTUP_PLUGIN			9
#define ST_STARTUP_DAILY			10
#define ST_PLAYINFO					11
#define ST_MOBILE_PLAYINFO			12
#define ST_MOBILE_AVG_SPEED			13
#define ST_MOBILE_PAUSE				14
#define ST_MOBILE_BUFFERING			15
#define ST_LOGIN					16
#define ST_VANE						17
#define ST_SUSPEND_WIN				18

//�ֶ�����
#define FT_IP						8
#define FT_DATATIME					9
#define FT_UID						10
#define FT_KB_VER					11
#define FT_OFFLINE					12
#define FT_INSTALL_TYPE				14
#define FT_BUNDLED_ID				15
#define FT_HP_SET					16
#define FT_HP_OLD					17
#define FT_HP_CUR					18
#define FT_HASH						20
#define FT_FILM_TITLE				21
#define FT_BUF_TYPE					22
#define FT_BUF_TIME					23
#define FT_TERM_VER					24
#define FT_STARTUP_MODE				25
#define FT_BROWSER					26
#define FT_BRO_VER					27
#define FT_PLAY_TYPE				28
#define FT_VIDEO_RATE				29
#define FT_AUDIO_RATE				30
#define FT_PLAY_TIME				31
#define FT_AVG_SPEED				32
#define FT_PAUSE_TYPE				33
#define FT_SVR_IP					34
#define FT_USERNAME					35
#define FT_USED_TIME				36
#define FT_RETRYS					37
#define FT_REASON					38
#define FT_CLI_TYPE					39
#define FT_LOGIN_STATUS				44
#define FT_LOGIN_TYPE				47			//��¼����

#pragma pack(push)
#pragma pack(1)
struct Value
{
	VARTYPE			vType;	//��������
	union
	{
		ULONGLONG	ullVal; //VT_UI8
		ULONG		ulVal;	//VT_UI4
		char*		pVal;	//VT_LPSTR
	};
	
	Value()
	{
		memset(this, 0, sizeof(Value));
	}
	Value(ULONGLONG val)
	{
		vType = VT_UI8;
		ullVal = val;
	}
	Value(ULONG val)
	{
		vType = VT_UI4;
		ulVal = val;
	}
	Value(int val)
	{
		vType = VT_UI4;
		ulVal = val;
	}
	Value(char* val)
	{
		vType = VT_LPSTR;
		pVal = val;
	}
	Value& operator=(Value& val)
	{
		memcpy(this, &val, sizeof(val));
		return *this;
	}
	Value& operator=(ULONGLONG val)
	{
		vType = VT_UI8;
		ullVal = val;
		return *this;
	}
	Value& operator=(ULONG val)
	{
		vType = VT_UI4;
		ulVal = val;
		return *this;
	}
	Value& operator=(int val)
	{
		vType = VT_UI4;
		ulVal = val;
		return *this;
	}
	Value& operator=(char* val)
	{
		vType = VT_LPSTR;
		pVal = val;
		return *this;
	}
};

//ͳ���ֶ���Ϣ
struct STAT_FIELD
{
	DWORD  nID;				//�ֶ�ID
	Value  val;				//ֵ
};
#pragma pack(pop)


#define STATAPI extern "C" __declspec(dllexport) 

/************************************************************************
SendStat			����ͳ��
nStatType			[IN] ͳ������
arrStat				[IN] ����ͳ���ֶ���Ϣ������
nArrLen				[IN] ����ĳ���
bAsy				[IN] �Ƿ�Ҫ�첽����
nTimeOut			[IN] ��ʱʱ��
��ע�� "UID"��"�첥�汾��"��"IP"��"ʱ��" "�Ƿ�����"�⼸���ֶ����贫�롣������֮����������
	  �ֶΣ���arrStat��nArrLenӦ����ֵ���˽ӿڽ�֧�ֵ���ͳ�ơ�
************************************************************************/
STATAPI STAT_ERROR SendStat(DWORD nStatType, STAT_FIELD arrStat[] = NULL, DWORD nArrLen = 0, BOOL bAsy = TRUE, DWORD nTimeOut = 2000);

typedef STAT_ERROR (_cdecl  *SendStatis)(DWORD nStatType, STAT_FIELD arrStat[] , DWORD nArrLen , BOOL bAsy , DWORD nTimeOut );

/************************************************************************
SendLastStat		���͹�ȥ��ͳ��
bAsy				[IN] �Ƿ�Ҫ�첽����
nTimeOut			[IN] ��ʱʱ��
��ע���˽ӿڽ����·��͹�ȥ��������ԭ����ʧ�ܵ�����ͳ��
************************************************************************/
STATAPI	STAT_ERROR SendLastStat(BOOL bAsy = TRUE, DWORD nTimeOut = 2000);














