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

struct S_Record_Param  //录制参数配置结构体
{
	S_Record_Param()
	{
		FileProtocol = _FP_LocalFile;//文件协议，本地文件，网络文件
		srccon = _E_CON_UNKNOW;//输入文件类型
		dstcon = _E_CON_UNKNOW;//输出文件类型
		starttime = 0;//录制开始时间
		stoptime = 3600;//录制结束时间
		width = 0;//目标视频宽
		height = 0;//目标视频高
		VQuality = HEIGHT;//码率
		nType = 0;
	    islostless = TRUE;//是否是无损截取
		isSaveAudio = FALSE;
		m_pQvodNet = NULL;
		m_pQvodCallBack = NULL;
		memset(InputPath,0,512);
		memset(OutputPath,0,512);
		memset(videocodecinfo,0,512);
		memset(audiocodecinfo,0,512);
	}
	e_FileProtocol FileProtocol;//文件协议，本地文件，网络文件
	e_CONTAINER    srccon;//输入文件类型
	e_CONTAINER    dstcon;//输出文件类型
	LONGLONG    starttime;//录制开始时间
	LONGLONG     stoptime;//录制结束时间
	long     width;//目标视频宽
	long    height;//目标视频高
	VideoQuality    VQuality;//码率
	int          nType; 
	bool    islostless;//是否是无损截取
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
	virtual QVOD_ERROR RecordVideo() = 0;//录制视频
	virtual QVOD_ERROR StartRecord() = 0;//录制视频
	virtual QVOD_ERROR StopRecord() = 0;//停止录制
	virtual QVOD_ERROR GetRecordPer(double *nProgress,LONGLONG nStartTime,LONGLONG nEndTime) = 0;//获取录制进度
	virtual QVOD_ERROR GetRecordPerError(double *nProgress,LONGLONG nStartTime,LONGLONG nEndTime) = 0;//获取录制进度
	virtual QVOD_ERROR ReleaseSource() = 0;
};

extern IVideoRecord *g_IVideoRecord; 

#ifdef __cplusplus
}
#endif

#endif