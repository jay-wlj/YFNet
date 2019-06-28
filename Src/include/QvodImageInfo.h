#ifndef _QVODIMAGEINFO_
#define _QVODIMAGEINFO_

#pragma once

//��������
#define IMG_ERROR_OK					0
#define IMG_ERROR_INVALID_PARAM			1
#define IMG_ERROR_INVALID_PATH			2
#define IMG_ERROR_INVALID_FORMART		3
#define IMG_ERROR_CANNOT_FIND_HASH		4
#define IMG_ERROR_DEPTH_FAILED			5	
#define IMG_ERROR_OTHER					100

//��������
#define MAX_NAME_LEN					256
#define MAX_REFPAGE_LEN					1024

//��������
#ifndef ETASKTYPE
enum e_TaskType
{
	tt_qvod,									//��׼qvod����
	tt_bt,										//bt����
	tt_qlive,									//ֱ������
	tt_http,									//http����
	tt_swf,										//swf����
};
#endif

enum e_OptionType
{
	ot_exif,
	ot_depth,
	ot_exifanddepth,
};

//������Ϣ
struct IMAGE_TASK_INFO
{
	ULONGLONG	uSize;							//�����С
	e_TaskType	eType;							//��������
	DWORD		dwCreateTime;					//ͼƬ����ʱ��(s)
	DWORD		dwPlayStartTime;				//����ʼ����ʱ��(ms)
	char		szHash[41];						//hash �������Ϊѡ��
	char		szQmvTag[5];					//qmvtag
	WCHAR		szName[MAX_NAME_LEN];			//������
	WCHAR		szRefPage[MAX_REFPAGE_LEN];		//����ҳ
	WCHAR		szShowName[MAX_NAME_LEN];		//��ʾ��
	IMAGE_TASK_INFO()
	{
		memset(this, 0, sizeof(IMAGE_TASK_INFO));
	}
};

// ֧�ֶ�ά����Ϣ
struct QRCODE_TASK_INFO
{
	ULONGLONG	uSize;							//�����С
	e_TaskType	eType;							//��������
	DWORD		dwPlayStartTime;				//����ʼ����ʱ��(ms)
	char		szHash[41];						//hash �������Ϊѡ��
	WCHAR		szShowName[MAX_NAME_LEN];		//��ʾ��
	WCHAR		szSeriesName[MAX_NAME_LEN];		//�缯����
	int			nSeriesID;						//�缯ID
	int			nSeriesIndex;					//�缯����
	QRCODE_TASK_INFO()
	{
		memset(this, 0, sizeof(QRCODE_TASK_INFO));
	}
};
#ifdef IMAGEUSELIB // lib
#define IMGINFOAPI extern "C"
#else
#ifdef IMAGEINFOEXPORT
#define IMGINFOAPI extern "C" __declspec(dllexport) 
#else
#define IMGINFOAPI extern "C" __declspec(dllimport) 
#endif
#endif

/************************************************************************
GetImageTaskInfo	��ȡͼƬ�е�������Ϣ
lpPath				[IN] ͼƬ·��(Ŀǰֻ֧��JPG��BMP��GIF��ʽ)
pTaskInfo			[OUT] ������Ϣ
bDepthDecode		[IN] �Ƿ���Ҫ��Ƚ���hash��Ϣ(Ŀǰֻ֧��JPG��ʽ��

��ע����������bDepthDecodeΪTRUE�����ڳ���λ�ö�ȡʧ�ܵ�����£���������Ƚ���hash������ͼƬָ�ƽӿڣ�
����ʱ�����0-3�롣�����ڳ���λ�ö�ȡ����������Ϣ������Դ˲�����
************************************************************************/
IMGINFOAPI int GetImageTaskInfo(LPCWSTR lpPath, IMAGE_TASK_INFO* pTaskInfo, e_OptionType option);
  
/************************************************************************
SetImageTaskInfo	����������Ϣ��ͼƬ��
lpPath				[IN] ͼƬ·��(Ŀǰֻ֧��JPG��BMP��GIF��ʽ)
pTaskInfo			[IN] ������Ϣ
bDepthDecode		[IN] �Ƿ���Ҫ��ȱ���hash��Ϣ(Ŀǰֻ֧��JPG��ʽ��

��ע����������bDepthDecodeΪTRUE������ȱ���hash��ͼƬ�У�����ͼƬָ�ƽӿڣ�����ʱ�����1-3�롣
************************************************************************/
IMGINFOAPI int SetImageTaskInfo(LPCWSTR lpPath, IMAGE_TASK_INFO* pTaskInfo, e_OptionType option);

#endif