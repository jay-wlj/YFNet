#ifndef _IYFNET_H_
#define _IYFNET_H_
#include <string.h>
#include <string>

//������Ķ���
typedef int NET_ERROR;
#define E_NET_OK						 0	//�ɹ�
#define E_NET_FALSE						-1	//ʧ��
#define E_NET_SEND_ERROR				-2	//��������ʧ��
#define E_NET_RECV_ERROR				-3	//��������ʧ��
#define E_NET_CON_ERROR					-4	//��������ʧ��
#define E_NET_ERROR_PARA				-5	//������ʼ�����������Ч����
#define E_NET_NO_FILE					-6	//�ļ�������
#define E_NET_TIME_OUT					-7	//��ʱ
#define E_NET_NO_DATA					-8	//δ���ص�����
#define E_NET_DISK_NOT_ENOUGH			-9	//���̿ռ䲻��
#define E_NET_CREATE_TASK_FAILED		-10	//��������ʧ��
#define E_NET_BUFFER_NOT_ENOUGH			-11	//���ݽ�����buffer�ռ��С����
#define E_NET_NO_TASK_BY_HASH			-12 //û��hash��Ӧ������
#define E_NET_PATH_NOT_ACCESS			-13	//·�����ɷ���
#define E_NET_CREATE_FOLDER_FAIL		-14	//����Ŀ¼ʧ��
#define E_NET_AUTH_FAIL					-15 //��֤ʧ�ܣ����������ʧ��
#define E_NET_AUTH_EXCEED_CLIENT_NUM	-16 //ʹ��sdk���û���������������ʧ��
#define E_NET_AUTH_DISABLE				-17 //sdk�����ˣ����������ʧ��
#define E_NET_DOMAIN_NOT_IN_WHITELIST	-18 //���������url���������ڰ�������

struct IYfNet;

#define MAX_FILE_PATH_LEN	511

#pragma pack(push)//�������״̬
#pragma pack(1)  //����Ϊ1�ֽڶ���
struct STaskInfo
{
	char hash[40];							//�����hash
	__int64 iFileLen;						//�ļ��Ĵ�С��Byte
	__int64 iDownloadLen;					//�����صĴ�С,Byte
	unsigned int iDownloadSpeed;			//��ǰ�������ٶȣ�B/s
	unsigned char status;					//��ǰ�����״̬��1:���� 2:��ͣ���أ��������������꣬���״̬������ 
	unsigned char tasktype;					//��ǰ��������ͣ���������Ϊ9������Ϊ��ͨ����
	bool	bDownloadAndCopy;				//��ǰ�����Ƿ������ˡ����ء�
	wchar_t szCopyPath[MAX_FILE_PATH_LEN];	//��bDownloadAndCopyΪtrue������ֶβ�Ϊ�գ��洢������Ŀ���Ŀ��·���������ļ���
	wchar_t szFilepath[MAX_FILE_PATH_LEN];	//������ļ�·���������ļ���
	STaskInfo()
	{
		memset(this,0,sizeof(STaskInfo));
	}
};
struct SBitInfo
{
	unsigned int iBlockSize;	//�ֿ��С
	unsigned int iBlockNum;		//�ֿ���
	char szBitField[1];			//�ֿ������λͼ���䳤�ṹ�壬����Ϊ iBlockNum/8 + (iBlockNum%8==0?0:1)
	SBitInfo()
	{
		memset(this,0,sizeof(SBitInfo));
	}
};
struct SCallBackPara
{
	int id;				//�¼�id
	int result;			//�¼��Ľ����ԭ��
	const char* buf;	//�¼����������ݣ���ΪNULL��UI��Ҫ�Լ�����һ��
	int buflen;			//���ݵĳ���
	SCallBackPara()
	{
		memset(this,0,sizeof(SCallBackPara));
	}
};
#pragma pack(pop)//�ָ�����״̬

//�ص�����
typedef void (*EventCallBackFunc)(SCallBackPara& para);
/*
����ص��¼�����:
//////////////////
�����꿽���ļ�������Ŀ¼
id:1
result:0��������ɣ�1������Ŀ���ļ�ʧ�ܣ�2��������ʧ�ܣ�3������Ŀ¼�ռ䲻�㣻4������Ŀ¼û��дȨ�ޣ�5��дʧ��
buf:40�ֽڵ�hash
buflen��40
/////////////////
��ʱ����Ŀ¼û�ռ��ˣ����ͷſռ�����¿�ʼ����
id:2
result:xxx
buf:NULL
buflen:0
////////////////
http���ش���
id:3
result:xxx
buf:40�ֽڵ�hash
buflen��40
*/

enum ELimitUploadSpeedMode
{
	LIMIT_UPLOAD_SPEED_DEFAULT = 0,	//Ĭ������ģʽ
	LIMIT_UPLOAD_SPEED_INTERNET,	//����ģʽ
	LIMIT_UPLOAD_SPEED_MOVIE,		//��Ӱģʽ
	LIMIT_UPLOAD_SPEED_GAME			//��Ϸģʽ	
};
#pragma pack(push)//�������״̬
#pragma pack(1)  //����Ϊ1�ֽڶ���
struct InitPara
{
	wchar_t* pTempCachePath;	//����������ʱ����Ŀ¼
	wchar_t* pDownloadPath;		//��������洢��·��
	wchar_t* pConfigPath;		//p2p�����ļ���·��
	char*	pAppToken;			//sdk��֤�Ĳ���
	unsigned int maxspace;		//������������ʹ�ÿռ䣬��λMB��������ɾ��������0Ϊ������
	EventCallBackFunc callback;	//�ص�����
	bool	bUpgradeProc;		//�Ƿ���������
	InitPara()
	{
		memset(this,0,sizeof(InitPara));
	}
};
#pragma pack(pop)//�ָ�����״̬

#ifdef YFNETEXPORT
#define YFNETAPI extern "C" __declspec(dllexport) 
#else
#define YFNETAPI extern "C" __declspec(dllimport) 
#endif

//��ʼ������ģ��,����E_NET_OK��ʾ�ɹ�,��������ֵ��ʾʧ��;
//pYfNet [OUT] : �洢�����Ķ���ָ��
//para [IN] : ��ʼ���Ĳ���
//lPointer [OUT] : ��ʼ����Ķ���ָ���ַ���ͷ�����ģ��ʱ��Ҫ�����ֵ
YFNETAPI NET_ERROR _InitYfNet(IYfNet **pYfNet,InitPara para,long& lPointer);

//�ͷ�����ģ��,lYfNetIDΪ_InitYfNet��lPointer
YFNETAPI void  _ClearYfNet(long lYfNetID);


struct IYfNet
{
	/**************************************************************************************
	GetPrivateVersion		��ȡ�ڲ��汾��
	pVersion				[IN/OUT]�ǿ��ڴ棬���ڴ洢�汾����Ϣ
	***************************************************************************************/
	virtual NET_ERROR GetPrivateVersion(char* pVersion) = 0;

	/**************************************************************************************
	CreateTask			������������
	pstrSrcUrl			[IN]  Ҫ�������������,��ֱ������http�����ļ���url���磺
						http://vodipad.wasu.cn/201407021742/3cc51ccdac8e4d8ca896152a95415347/pcsan08/mams/vod/201404/24/20/20140424202708118c8104efd_dc5607f3.mp4?version=P2PPlayer_V.1.5.16
	pstrKeyurl			[IN]  pstrSrcUrl����ȡ�����Ĺ̶���url����
						http://wasu.cn/20140424202708118c8104efd_dc5607f3.mp4
	pstrReferer			[IN]  ����ҳ����ΪNULL
	pstrHash			[OUT] pstrKeyurl��Ӧ��p2p hashֵ,40�ֽڵ��ַ�������������������Ĳ������Ը�ֵ��Ϊ����
	bIsOtherCdn			[IN]  �Ƿ������CDN
	***************************************************************************************/
	virtual NET_ERROR CreateTask(const char *pstrSrcUrl,const char* pstrKeyurl,const char* pstrReferer,char *pstrHash,bool bIsOtherCdn) = 0;

	/**************************************************************************************
	CreateHlsTask			����HLS��������
	pstrSrcUrl			[IN]  Ҫ�������������,��ֱ������http�����ļ���url���磺
						http://113.107.109.90/mp4/2015/dianshiju/yzg_22956/0627123DA36F6FC111496DAF3C7FC297_20151123_1_1_385.mp4/playlist.m3u8?uuid=d1597dfc4a5e4be38938161902298bc9&t=566ffbb2&win=300&pno=1120&srgid=12&urgid=372&srgids=12&nid=869&payload=usertoken%3duuid%3dd1597dfc4a5e4be38938161902298bc9%5eruip%3d3395848389%5ehit%3d1&rdur=21600&arange=0&limitrate=0&fid=0627123DA36F6FC111496DAF3C7FC297&sign=cd62bbe75ba76f50a8a6057dd1c331dc&ver=0x03
	pstrKeyurl			[IN]  ��Ƶ��Ψһ��ʶ��Ҫ������ƽ̨��һ�£�Ŀǰ����Ϊ
						http://hunantv.com/hunantv.imgo.tv/1846354_1.m3u8
						http://hunantv.com/pcvideodn.titan.imgo.tv/1846354_1.m3u8

						http://hunantv.com/ �ǹ̶��ģ��������CDN�����������â������CDN������hunantv.imgo.tv���棬����������CDN����ԭ������
						����ǡ���Ƶ��ʶ+�»���+����������+.m3u8��������������ͬ��������Ƶ��ͬCDN���ӵ�keyurl��

	bIsOtherCdn			[IN]  �Ƿ������CDN
	pstrHash			[OUT] pstrKeyurl��Ӧ��p2p hashֵ,40�ֽڵ��ַ�������������������Ĳ������Ը�ֵ��Ϊ����
	pstrHttpProxyUrl	[OUT] ����Ƶ��Ӧ�ı���http�����ַ���Ѹõ�ַ�����������в��ţ��ڴ��С����100�ֽ�
	***************************************************************************************/
	virtual NET_ERROR CreateHlsTask(const char *pstrSrcUrl,const char* pstrKeyurl,bool bIsOtherCdn,char *pstrHash,char* pstrHttpProxyUrl) = 0;

	/**************************************************************************************
	CreateUpdateTask	������������
	pstrSrcUrl			[IN]  Ҫ�������������,��ֱ������http�����ļ���url
	pstrHash			[OUT]  �����p2p hashֵ��40�ֽڵ��ַ�������������������Ĳ������Ը�ֵ��Ϊ����
	***************************************************************************************/
	virtual NET_ERROR CreateUpdateTask(const char *pstrSrcUrl,char* pstrHash) = 0;

	/***************************************************************************************	
	DeleteTask			ɾ������
	pstrHash			[IN] Ҫɾ���������hash
	***************************************************************************************/
	virtual NET_ERROR DeleteTask(const char *pstrHash) = 0;

	/***************************************************************************************	
	SetTaskDownload		��������Ϊ����ģʽ����ģʽ�������������´����ָ���洢·��
	pstrHash			[IN] Ҫ���õ������hash
	bOption				[IN] true or false
	pFilename			[IN] ��bOptionΪtrue������Ҫ���ô洢���ļ���
	***************************************************************************************/
 	virtual NET_ERROR SetTaskDownload(const char *pstrHash,bool bOption,const wchar_t* pFilename) = 0;

	/***************************************************************************************	
	SetTempCachePath	�����������ʱ�洢Ŀ¼
	pstrPath			[IN]  ����·��ֵ
	***************************************************************************************/
	virtual NET_ERROR SetTempCachePath(const wchar_t *pstrPath) = 0;

	/***************************************************************************************	
	SetDefaultDownPath	������������Ĵ洢·��
	pstrPath			[IN]  ����·��ֵ
	***************************************************************************************/
	virtual NET_ERROR SetDefaultDownPath(const wchar_t *pstrPath) = 0;

	/***************************************************************************************	
	PauseTask			��ͣ����
	pstrHash			[IN]  Ҫ��ͣ�������hash
	bIsAll				[IN]  �Ƿ���ͣ��������,�����,pstrHash = NULL
	***************************************************************************************/
	virtual NET_ERROR PauseTask(const char *pstrHash,bool bIsAll=false) = 0;


	/***************************************************************************************	
	RunTask				��������(��ʼ����)
	pstrHash			[IN]  Ҫ���е������hash
	bIsAll				[IN]  �Ƿ�������������,�����,pstrHash = NULL
	***************************************************************************************/
	virtual NET_ERROR RunTask(const char *pstrHash,bool bIsAll=false) = 0;

	/***************************************************************************************	
	PlayTask			���������Ƿ����ڲ���
	pstrHash			[IN]  Ҫ���ŵ������hash
	bPlaying			[IN]  trueΪ���ţ�falseΪֹͣ����
	***************************************************************************************/
	virtual NET_ERROR SetTaskPlaying(const char *pstrHash,bool bPlaying) = 0;

	/***************************************************************************************	
	ReadFile			��ȡ�ļ�����
	pstrSrcUrl			[IN]  Ҫ�������������,��ֱ������http�����ļ���url���磺
	http://vodipad.wasu.cn/201407021742/3cc51ccdac8e4d8ca896152a95415347/pcsan08/mams/vod/201404/24/20/20140424202708118c8104efd_dc5607f3.mp4,
							  �ò�����Ϊ�˸������ӣ���ֹ����ʧЧ���޷����ص�����
	pstrHash			[IN]  Ҫ��ȡ���ļ�hash
	pbuf				[OUT] �洢��ȡ���ݵĿռ�,�ɵ�����������ͷ�
	nstart				[IN]  ��ʼλ��
	nlen				[IN]  ��ȡ�ĳ���
	pnreadlen			[OUT] ʵ�ʶ�ȡ�ĳ���
	***************************************************************************************/
	virtual NET_ERROR ReadFile(const char *pstrSrcUrl,const char *pstrHash,char *pbuf,__int64 nstart,long nlen,long *pnreadlen) = 0;
	
	/***************************************************************************************	
	QueryFileSize		��ѯ�ļ���С
	pstrHash			[IN]  Ҫ��ѯ���ļ�hash
	iFilesize			[OUT] �洢��ѯ�����ļ���С����YfNetģ��δ�������ȡ��ʱ��iFilesizeΪ0
	***************************************************************************************/
	virtual NET_ERROR QueryFileSize(const char *pstrHash,__int64& iFilesize) = 0;

	/*****************************************************************************************
	QueryTaskInfo		ͨ��Hash��ѯ������Ϣ
	pstrHash			[IN]  Ҫ��ѯ���ļ�hash
	pTaskInfo			[OUT] �����ѯ����������Ϣ,����������ռ��������ͷ�
	****************************************************************************************/
	virtual NET_ERROR QueryTaskInfo(const char *pstrHash,STaskInfo *pTaskInfo) = 0;

	/***************************************************************************************	
	QueryBitInfo		��ѯBitλ������Ϣ���մ�������ʱδ��ȡ���ļ���С���᷵��E_NET_FALSE
	pstrHash			[IN]  Ҫ��ѯ���ļ�hash
	pBuf				[IN]  ������Ļ��������Խṹ��SBitInfo���
	iBufLen				[IN/OUT] �������λ��Ϣ�Ŀռ��С,���������ش���E_NET_BUFFER_NOT_ENOUGHʱ,
						����pBuf�Ŀռ��С����,��ʱ��iBufLen������Ҫ����С����
	***************************************************************************************/
	virtual NET_ERROR QueryTaskBitInfo(const char *pstrHash, char *pBuf,int &iBufLen) = 0;

	/***********************************************************************
	SetSpaceCanUse			���ÿ��ô��̿ռ��С�������ļ���С
							���ܳ�����ֵ���������������ã�0Ϊ���޴�С
	spacecanuse:			[IN] ��λΪMB
	*******************************************************************/
	virtual NET_ERROR SetSpaceCanUse(unsigned int spacecanuse) = 0;

	/***************************************************************************************	
	SetPlayingPos		���õ�ǰ�Ĳ���λ��
	pstrHash			[IN]  Ҫ���õ�����hash
	pos					[IN]  Ҫ���õ�λ�ã���λΪByte
	***************************************************************************************/
	virtual NET_ERROR SetPlayingPos(const char *pstrHash,__int64 pos) = 0;
	
	/***************************************************************************************	
	SetPlayingTimepoint		���õ�ǰ�Ĳ���ʱ��λ��
	pstrHash			[IN]  Ҫ���õ�����hash
	timepoint			[IN]  Ҫ���õ�ʱ��λ�ã���λΪms
	bSeek				[IN]  �Ƿ�seek����Ĳ���λ�ñ䶯
	iCacheTime			[OUT] ���滺���ʱ�䣬��λΪms
	***************************************************************************************/
	virtual NET_ERROR SetPlayingTimepoint(const char *pstrHash,int timepoint,bool bSeek,int& iCacheTime) = 0;

	/***************************************************************************************	
	SetVideoDuration		������Ƶ��ʱ��
	pstrHash			[IN]  Ҫ���õ�����hash
	duration			[IN]  ��Ƶ��ʱ������λ��
	***************************************************************************************/
	virtual NET_ERROR SetVideoDuration(const char *pstrHash,int duration) = 0;
	
	/***************************************************************************************	
	SetLimitUploadSpeedMode		�����ϴ�����ģʽ
	mode				[IN]  Ҫ��������ģʽ
	***************************************************************************************/
	virtual NET_ERROR SetLimitUploadSpeedMode(ELimitUploadSpeedMode mode) = 0;
		
	/***************************************************************************************	
	SetLimitUploadSpeedPercentage		�����ϴ����ٰٷֱ�
	percentage				[IN]  Ҫ�������ٰٷֱ�
	***************************************************************************************/
	virtual NET_ERROR SetLimitUploadSpeedPercentage(int percentage) = 0;
			
	/***************************************************************************************	
	GetSampleSpeed			[IN]  ��ȡ10�����ڵ�ȡ��ƽ���ٶ�
	speed					[IN]  �洢�ٶ�ֵ����λ Byte/s
	***************************************************************************************/
	virtual NET_ERROR GetSampleSpeed(int& speed) = 0;

	/***************************************************************************************	
	NotifyVideoBuffering		֪ͨ��Ƶ�����ˣ����ڻ���
	pstrHash			[IN]  Ҫ֪ͨ������hash
	***************************************************************************************/
	virtual NET_ERROR NotifyVideoBuffering(const char *pstrHash) = 0;

	/***************************************************************************************	
	NotifyVideoFinishBuffering		֪ͨ��Ƶ�Ѿ������꣬�������ٵ�״̬��
	pstrHash			[IN]  Ҫ֪ͨ������hash
	***************************************************************************************/
	virtual NET_ERROR NotifyVideoFinishBuffering(const char *pstrHash) = 0;
};
#endif