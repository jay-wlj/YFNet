#ifndef _QVODIMAGEINFO_
#define _QVODIMAGEINFO_

#pragma once

//错误类型
#define IMG_ERROR_OK					0
#define IMG_ERROR_INVALID_PARAM			1
#define IMG_ERROR_INVALID_PATH			2
#define IMG_ERROR_INVALID_FORMART		3
#define IMG_ERROR_CANNOT_FIND_HASH		4
#define IMG_ERROR_DEPTH_FAILED			5	
#define IMG_ERROR_OTHER					100

//长度限制
#define MAX_NAME_LEN					256
#define MAX_REFPAGE_LEN					1024

//任务类型
#ifndef ETASKTYPE
enum e_TaskType
{
	tt_qvod,									//标准qvod任务
	tt_bt,										//bt任务
	tt_qlive,									//直播任务
	tt_http,									//http任务
	tt_swf,										//swf任务
};
#endif

enum e_OptionType
{
	ot_exif,
	ot_depth,
	ot_exifanddepth,
};

//任务信息
struct IMAGE_TASK_INFO
{
	ULONGLONG	uSize;							//任务大小
	e_TaskType	eType;							//任务类型
	DWORD		dwCreateTime;					//图片创建时间(s)
	DWORD		dwPlayStartTime;				//任务开始播放时间(ms)
	char		szHash[41];						//hash 必填。其他为选填
	char		szQmvTag[5];					//qmvtag
	WCHAR		szName[MAX_NAME_LEN];			//任务名
	WCHAR		szRefPage[MAX_REFPAGE_LEN];		//引用页
	WCHAR		szShowName[MAX_NAME_LEN];		//显示名
	IMAGE_TASK_INFO()
	{
		memset(this, 0, sizeof(IMAGE_TASK_INFO));
	}
};

// 支持二维码信息
struct QRCODE_TASK_INFO
{
	ULONGLONG	uSize;							//任务大小
	e_TaskType	eType;							//任务类型
	DWORD		dwPlayStartTime;				//任务开始播放时间(ms)
	char		szHash[41];						//hash 必填。其他为选填
	WCHAR		szShowName[MAX_NAME_LEN];		//显示名
	WCHAR		szSeriesName[MAX_NAME_LEN];		//剧集名称
	int			nSeriesID;						//剧集ID
	int			nSeriesIndex;					//剧集索引
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
GetImageTaskInfo	获取图片中的任务信息
lpPath				[IN] 图片路径(目前只支持JPG、BMP、GIF格式)
pTaskInfo			[OUT] 任务信息
bDepthDecode		[IN] 是否需要深度解码hash信息(目前只支持JPG格式）

备注：若设置了bDepthDecode为TRUE，则在常规位置读取失败的情况下，将进行深度解码hash（调用图片指纹接口）
，用时大概在0-3秒。若已在常规位置读取到了任务信息，则忽略此参数。
************************************************************************/
IMGINFOAPI int GetImageTaskInfo(LPCWSTR lpPath, IMAGE_TASK_INFO* pTaskInfo, e_OptionType option);
  
/************************************************************************
SetImageTaskInfo	设置任务信息到图片中
lpPath				[IN] 图片路径(目前只支持JPG、BMP、GIF格式)
pTaskInfo			[IN] 任务信息
bDepthDecode		[IN] 是否需要深度编码hash信息(目前只支持JPG格式）

备注：若设置了bDepthDecode为TRUE，将深度编码hash到图片中（调用图片指纹接口），用时大概在1-3秒。
************************************************************************/
IMGINFOAPI int SetImageTaskInfo(LPCWSTR lpPath, IMAGE_TASK_INFO* pTaskInfo, e_OptionType option);

#endif