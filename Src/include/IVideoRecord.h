#ifndef _IVIDEORECORD_H_
#define _IVIDEORECORD_H_

#include "QvodError.h"
//#include "QvodStructure.h"


STDAPI CreateObject(HWND hwnd,const CLSID& clisd ,const IID &iid,void **ppv);

enum VideoQuality 
{
	HEIGHT,
	MEDIUM,
	LOW
};

enum E_SaveType
{
	_E_SaveAudio,
	_E_SaveloselessVideo,
	_E_SaveVidoe
}; 

struct S_Record_Param  //¼�Ʋ������ýṹ��
{
	S_Record_Param()
	{
		FileProtocol = _FP_LocalFile;//�ļ�Э�飬�����ļ��������ļ�
		srccon = _E_CON_UNKNOW;//�����ļ�����
		dstcon = _E_CON_UNKNOW;//����ļ�����
		starttime = 0;//¼�ƿ�ʼʱ��
		stoptime = 3600;//¼�ƽ���ʱ��
		width = 0;//Ŀ����Ƶ��
		height = 0;//Ŀ����Ƶ��
		VQuality = HEIGHT;//����
		nType = 0;
	    islostless = TRUE;//�Ƿ��������ȡ
		isSaveAudio = FALSE;
		m_pQvodNet = NULL;
		m_pQvodCallBack = NULL;
		memset(InputPath,0,512);
		memset(OutputPath,0,512);
		memset(videocodecinfo,0,512);
		memset(audiocodecinfo,0,512);
	}
	e_FileProtocol FileProtocol;//�ļ�Э�飬�����ļ��������ļ�
	e_CONTAINER    srccon;//�����ļ�����
	e_CONTAINER    dstcon;//����ļ�����
	LONGLONG    starttime;//¼�ƿ�ʼʱ��
	LONGLONG     stoptime;//¼�ƽ���ʱ��
	long     width;//Ŀ����Ƶ��
	long    height;//Ŀ����Ƶ��
	VideoQuality    VQuality;//����
	int          nType; 
	bool    islostless;//�Ƿ��������ȡ
	bool    isSaveAudio;
	interface IQvodNet *m_pQvodNet;
	interface IQvodCallBack *m_pQvodCallBack;
	wchar_t InputPath[512];
	wchar_t OutputPath[512];
	wchar_t videocodecinfo[512];
	wchar_t audiocodecinfo[512];
};

#ifdef __cplusplus
extern "C" {
#endif

	// {3A1F784F-9F54-4d23-A0FF-340AAC7E45CE}
	const GUID GUID_VideoRecord = {'QDVR', 0x9f54, 0x4d23, {0xa0, 0xff, 0x34, 0xa, 0xac, 0x7e, 0x45, 0xce}};
	const GUID IID_IVideoRecord = {0x31187840, 0x9f54, 0x4d23, {0xa0, 0xff, 0x34, 0xa, 0xac, 0x7e, 0x45, 0xce}};

DECLARE_INTERFACE_(IVideoRecord, IUnknown)
{
	virtual QVOD_ERROR SetConfigure(S_Record_Param *pRecordParam) = 0;
	virtual QVOD_ERROR GetConfigure(S_Record_Param& RecordParam) = 0;
	virtual QVOD_ERROR RecordVideo() = 0;//¼����Ƶ
	virtual QVOD_ERROR StartRecord() = 0;//¼����Ƶ
	virtual QVOD_ERROR StopRecord() = 0;//ֹͣ¼��
	virtual QVOD_ERROR GetRecordPer(double *nProgress,LONGLONG nStartTime,LONGLONG nEndTime) = 0;//��ȡ¼�ƽ���
	virtual QVOD_ERROR GetRecordPerError(double *nProgress,LONGLONG nStartTime,LONGLONG nEndTime) = 0;//��ȡ¼�ƽ���
	virtual QVOD_ERROR ReleaseSource() = 0;
};

extern IVideoRecord *g_IVideoRecord; 

#ifdef __cplusplus
}
#endif

#endif