#ifndef _QVOD_Media_Format_H__
#define _QVOD_Media_Format_H__

// 头文件的版本号
// DLL发布日期
// 代码编写者名字

#ifdef 这里是DLL项目的宏定义
#define DLLAPI extern "C" __declspec(dllexport)
#else
#define DLLAPI extern "C" __declspec(dllimport)
#endif


typedef struct QvodMediaFormat
{
	long nSize;							//结构大小，必须指定
	wchar_t szFileName[MAX_PATH*2];		//文件全路径，必须指定
	wchar_t szTitle[MAX_PATH];			//标题
	wchar_t szArtist[MAX_PATH];			//作者
	wchar_t szAlbum[MAX_PATH];			//专辑
	__int64 nFileSize;					//文件大小 单位字节
	__int64 nDuration;					//文件播放时长-豪秒级
	long    nFrameWidth;				//视频宽，如音频为0
	long    nFrameHeight;				//视频高，如音频为0
	long    nbps;						//bit率，都以 nFileSize*8/(ndur/1000)
	long    nVideoIncoderID;			//视频编码器ID
	long    nAudioIncoderID;			//音频编码器ID
										//关于音/视频编码器ID需自定义，
										//如 H264 = 1,AVC1=2,MPG-3, 
										//REALVIDEO=4,REALAUDIO=5类似
	long nReserved;						//保留字段
}QvodMediaFormat_s;



DLLAPI BOOL GetQvodMediaFormat(QvodMediaFormat *pFormat); // 获取媒体文件格式信息，返回FALSE表示失败


#endif