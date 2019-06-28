#pragma once
#include "qvod_string.h"
#include <map>
#include <string>
using std::map;
using std::string;

class CfgHost
{
public:
	CfgHost(void);
	~CfgHost(void);

	bool SetConfig(const string & strCfg);
public:
	int m_lMaxCacheTime;									// ���Ԥ����ʱ����Ĭ��:60s
	int m_lMaxDownTimeFromCDN;								// ��cdnһ��http������������ʱ����Ĭ��:25s
	unsigned int m_uMaxConnectTime;							// CDN����socket connect�ĳ�ʱʱ�䣬Ĭ��:5s
	unsigned int m_uMaxRecvTime;							// socket recv �����ʱʱ�䣬Ĭ��:10s:
	int m_limitSpeedMode;									// ����ģʽ,0:������ 1:�����Զ�ģʽ 2:��Ӱģʽ 3:���ƴ���
	unsigned int m_uMaxDownTaskCount;						// ͬʱ���������������Ĭ��:-1 ������

	unsigned int m_uConcurrentNum;							// ����������󲢷�������
	unsigned int m_uUpConcurrentNum;						// �ϴ�������󲢷���

	int	m_nDownPlayingTaskCount;							// ��ǰ���ز��ŵ�������
	bool	m_bVideoBuffering;								// ��ǰ������Ƶ�Ƿ����ڻ���

	int m_nCurNetType;											// ��ǰ�������ͣ��ο�ENetType
	string m_strAppVer;											// ��ǰapp�汾��

	// �ͷ��ļ��ϴ�
	std::string strUploadInitUrl;							// �ļ��ϴ�init���ӿ�
	std::string strUploadUrl;								// �ļ��ϴ�upload�ӿ�
	std::map<string, string>	m_mSignKeys;				// �ͷ�ts�ļ�ǩ��keyͰ
	//bool	m_bUseCacheMem;									// ���������Ƿ񻺴浽�ڴ�(�����浱ǰ��������)
};

extern CfgHost g_cfg;