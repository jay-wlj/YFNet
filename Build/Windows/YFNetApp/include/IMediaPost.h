#pragma once
#ifndef _IMEDIAPOST_H_
#define _IMEDIAPOST_H_
#include "..\include\QvodError.h"
#define CHANNEAL_TRANSLATION
#define USE_D3D_PIXEL_SHADER
#define VIDEO_ENHANCE_WIPER
enum DEINTERLACE_MODE
{
	DTL_BOB,		//BOB反交错
	DTL_MOCOMP2,	//MoComp2反交错
	DTL_TOMSMOCOMP, //TomsMoComp反交错
	DTL_NULL		//不做反交错处理
};

enum SubTimeType
{
	SUB_TIME_NOUSER = 0,	//不使用
	SUB_TIME_CUR,			//当前时间
	SUB_TIME_END,			//剩余时间
	SUB_TIME_SYSTEM			//系统时间
};

enum MULTIP_SUB
{
	MAJOR_SUB = 0,
	MINOR_SUB,
	THIRD_SUB
};

#ifndef __IPOSTPROCESS_HH__
// 3D Options
typedef enum
{
    No3DView = 0,

    // 2D转3D播放, 视差系数 k =(0.0~ 0.1)  默认0.015
    To3D_RedBlue_LeftRight,       // 2D转3D_红蓝_左右,
    To3D_RedBlue_RightLeft,       // 2D转3D_红蓝_右左
    To3D_RedGreen_LeftRight,      // 2D转3D_红绿_左右
    To3D_RedGreen_RightLeft,      // 2D转3D_红绿_右左
    To3D_BrownBlue_LeftRight,     // 2D转3D_棕蓝_左右
    To3D_BrownBlue_RightLeft,     // 2D转3D_棕蓝_右左

    // 3D视频播放, 视差系数 k = (0.0~0.05) 默认0.0
    RedBlue_LeftRight,            // 左右转红蓝
    RedBlue_RightLeft,            // 右左转红蓝
    RedGreen_LeftRight,           // 左右转红绿
    RedGreen_RightLeft,           // 右左转红绿
    BrownBlue_LeftRight,          // 左右转棕蓝
    BrownBlue_RightLeft,          // 右左转棕蓝

    RedBlue_TopBottom,            // 上下转红蓝
    RedBlue_BottomTop,            // 下上转红蓝
    RedGreen_TopBottom,           // 上下转红绿
    RedGreen_BottomTop,           // 下上转红绿
    BrownBlue_TopBottom,          // 上下转棕蓝
    BrownBlue_BottomTop,          // 下上转棕蓝

    // 3D转2D播放, 视差系数将忽略
    To2D_LeftOnly,                // 左右转左画面
    To2D_RightOnly,               // 左右转左画面
    To2D_TopOnly,                 // 上下转上画面
    To2D_BottomOnly               // 上下转上画面
}OPTION3DVIEW;
#endif
enum ESubPlace
{
	P_SUB_LEFT_TOP = 1,
	P_SUB_MIDLLE_TOP,
	P_SUB_RIGHT_TOP,
	P_SUB_LEFT_MID,
	P_SUB_MIDLLE_MID,
	P_SUB_RIGHT_MID,
	P_SUB_LEFT_BOTTOM,
	P_SUB_MIDLLE_BOTTOM,
	P_SUB_RIGHT_BOTTOM,
	P_SUB_USER,
	P_SUB_LEFT,
	P_SUB_MIDLLE, //X方向居中 Y方向自定义
	P_SUB_RIGHT
};

// Enhance Options
typedef enum
{
    EnhanceEffect_None = 0,
    EnhanceEffect_Line,         // Refreshing with a line from left to right, and stay for 2 seconds in middle
    EnhanceEffect_Rolling       // Refreshing quickly from left to right, don't show line and stay
}OPTIONENHANCE;

#ifndef QVODPLAYMEDIA_EXPORTS
static GUID IID_IPostVideoEffect = {0xc8300751, 0x2e65, 0x45a9, {0xbb, 0x12, 0xd8, 0x99, 0xd3, 0xe8, 0x6c, 0x35}};
#else
EXTERN_GUID(IID_IPostVideoEffect, 0xc8300751, 0x2e65, 0x45a9, 0xbb, 0x12, 0xd8, 0x99, 0xd3, 0xe8, 0x6c, 0x35);
#endif
interface IVideoPost
{
	//视频增强
#ifdef VIDEO_ENHANCE_WIPER
	virtual QVOD_ERROR SetVideoEnhance(bool enable,  OPTIONENHANCE effect) = 0;
#else
	virtual QVOD_ERROR SetVideoEnhance(bool enable) = 0;
#endif
	virtual QVOD_ERROR GetVideoEnhance(bool &enable) = 0;

	//去噪
	virtual QVOD_ERROR ReduceNoise(bool enable,		//开启或关闭
								   int ReduceNum) = 0;//降噪数量（范围20-150,35）
	virtual QVOD_ERROR GetReduceNoiseFlag(bool &enable,
								   int &ReduceNum) = 0;

	//GAMMA校正
	virtual QVOD_ERROR Gamma(bool enable,		//开启或关闭
							 int nGamma) = 0;//降噪数量（范围20-150,35）
	virtual QVOD_ERROR GetGammaFlag(bool &enable, 
									int &nGamma) = 0;

	//锐化
	virtual QVOD_ERROR Sharpen(bool enable,			//开启或关闭
							   int nSharpenCount,	//锐化数量(范围0-100，默认值50)
							   int nSharpenThreshold) = 0;//锐化阈值（范围0-20，默认值5）
	virtual QVOD_ERROR GetSharpenFlag(bool &enable, //开启或关闭
									  int &nSharpenCount, //开启或关闭
									  int &nSharpenThreshold) = 0;

	//反交错
	virtual QVOD_ERROR Deinterlace(DEINTERLACE_MODE Dtlmode,//反交错模式，见DEINTERLACE_MODE定义
								   int Dtlparam) = 0;//反交错参数BOB模式（范围0-5，默认值0），TomsMoComp模式（范围0-25，默认值3）
	virtual QVOD_ERROR GetDeinterlaceFlag(DEINTERLACE_MODE &Dtlmode,
										  int &Dtlparam) = 0;

	//亮度
	virtual QVOD_ERROR SetBrightness(int nBrightness) = 0;//范围-127-127,默认值0
	virtual QVOD_ERROR GetBrightness(int &nBrightness) = 0;

	//饱和度
	virtual QVOD_ERROR SetSaturation(int nSaturation) = 0;//范围-127-127,默认值0
	virtual QVOD_ERROR GetSaturation(int &nSaturation) = 0;

	//对比度
	virtual QVOD_ERROR SetContrast(int nContrast) = 0;//范围-127-127,默认值0
	virtual QVOD_ERROR GetContrast(int &nContrast) = 0;

	//色调
	virtual QVOD_ERROR SetHue(int nHue) = 0;//范围0-616,默认值0
	virtual QVOD_ERROR GetHue(int &nHue) = 0;

	////翻转
	//virtual QVOD_ERROR Flip(int nAngel) = 0;//范围0-616,默认值0
	//virtual QVOD_ERROR GetFlip(int &nAngel) = 0;

	//旋转
	virtual QVOD_ERROR Flip(int nAngel) = 0;//范围90的倍数,默认值0
	virtual QVOD_ERROR GetFlip(int &nAngel) = 0;

	//水平翻转
	virtual QVOD_ERROR HorizontalReverse(BOOL bHorizontal) = 0;
	virtual QVOD_ERROR GetHorizontalReverse(BOOL &bHorizontal) = 0;

	//垂直翻转
	virtual QVOD_ERROR VerticalReverse(BOOL bVertical) = 0;
	virtual QVOD_ERROR GetVerticalReverse(BOOL &bVertical) = 0;

	//2D转3D
	virtual QVOD_ERROR Set3DView(OPTION3DVIEW opt3DView, float kView) PURE;//OPTION3DVIEW opt3DView:左右眼位置，float kView：视差系数，详见OPTION3DVIEW注释
	virtual QVOD_ERROR Get3DView(OPTION3DVIEW* opt3DView, float* kView) PURE;
	//截图
	virtual QVOD_ERROR GetPostVideoImage(int WaitTime, BYTE** ppByte, int *pnBuffersize) = 0;
};

#ifndef QVODPLAYMEDIA_EXPORTS
static GUID IID_COLORFILTER_SUB ={0x7e68b1a4, 0x306c, 0x4ed4, {0xb2, 0x62, 0x31, 0x95, 0xa, 0x83, 0x15, 0x29}};
#else
EXTERN_GUID(IID_COLORFILTER_SUB, 0x7e68b1a4, 0x306c, 0x4ed4, 0xb2, 0x62, 0x31, 0x95, 0xa, 0x83, 0x15, 0x29);
#endif
interface ISubtitle
{
#ifdef USE_D3D_PIXEL_SHADER
	virtual QVOD_ERROR PutSubPath(WCHAR* wszPath/*, int& SubIndex*/) = 0;
	virtual QVOD_ERROR GetSubPath(/*int SubIndex, */WCHAR* wszPath, int &buffersize) = 0;

	//显示（隐藏）字幕
	virtual QVOD_ERROR PutSubVisableFlag(bool bVisable, MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetSubVisableFlag(bool &bVisable,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//清除字幕
	virtual QVOD_ERROR ClearSubtitle() = 0;

	//设置（获取）字幕字体
	virtual QVOD_ERROR PutSubFont(LOGFONT sf,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetSubFont(LOGFONT &sf,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//设置（获取）字体颜色
	virtual QVOD_ERROR PutFontColor(DWORD fcolor,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetFontColor(DWORD &fcolor,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//设置（获取）字体轮廓颜色
	virtual QVOD_ERROR PutOutLineColor(DWORD fcolor,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetOutlineColor(DWORD &fcolor,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//设置（获取）字幕位置
	virtual QVOD_ERROR PutSubPlace(ESubPlace place, int cx, int cy,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetSubPlace(ESubPlace &place, int &cx, int &cy,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//设置（获取）字幕延迟(超前)
	virtual QVOD_ERROR PutSubDelay(int delaytime,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetSubDelay(int &delaytime,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//显示当前（剩余、系统）时间
	virtual QVOD_ERROR PutSubTime(SubTimeType subTimeType) = 0;
	virtual QVOD_ERROR GetSubTime(SubTimeType &subTimeType) = 0;

	//设置（获取）当前（剩余、系统）时间字体
	virtual QVOD_ERROR PutTimeFont(LOGFONT lf) = 0;
	virtual QVOD_ERROR GetTimeFont(LOGFONT &lf) = 0;
#else
	//设置（获取）字幕路径
	virtual QVOD_ERROR PutSubPath(WCHAR* wszPath) = 0;
	virtual QVOD_ERROR GetSubPath(WCHAR* wszPath, int &pbuffersize) = 0;

	//显示（隐藏）字幕
	virtual QVOD_ERROR PutSubVisableFlag(bool bVisable) = 0;
	virtual QVOD_ERROR GetSubVisableFlag(bool &bVisable) = 0;
	
	//清除字幕
	virtual QVOD_ERROR ClearSubtitle() = 0;

	//设置（获取）字幕字体
	virtual QVOD_ERROR PutSubFont(LOGFONT sf) = 0;
	virtual QVOD_ERROR GetSubFont(LOGFONT &sf) = 0;

	//设置（获取）字体颜色
	virtual QVOD_ERROR PutFontColor(DWORD fcolor) = 0;
	virtual QVOD_ERROR GetFontColor(DWORD &fcolor) = 0;

	//设置（获取）字体轮廓颜色
	virtual QVOD_ERROR PutOutLineColor(DWORD fcolor) = 0;
	virtual QVOD_ERROR GetOutlineColor(DWORD &fcolor) = 0;

	//设置（获取）字幕位置
	virtual QVOD_ERROR PutSubPlace(ESubPlace place, int cx, int cy) = 0;
	virtual QVOD_ERROR GetSubPlace(ESubPlace &place, int &cx, int &cy) = 0;

	//设置（获取）字幕延迟(超前)
	virtual QVOD_ERROR PutSubDelay(int delaytime) = 0;
	virtual QVOD_ERROR GetSubDelay(int &delaytime) = 0;

	//显示当前（剩余、系统）时间
	virtual QVOD_ERROR PutSubTime(SubTimeType subTimeType) = 0;
	virtual QVOD_ERROR GetSubTime(SubTimeType &subTimeType) = 0;

	//设置（获取）字幕延迟
	virtual QVOD_ERROR PutTimeFont(LOGFONT lf) = 0;
	virtual QVOD_ERROR GetTimeFont(LOGFONT &lf) = 0;
#endif
	//设置播放时长
	//virtual QVOD_ERROR SetDuration(__int64 ltime) = 0;
};

enum Channel
{
	CH_DEFAULT,
	CH_LEFTCHANNEL,
	CH_RIGHT_CHANNEL,
	CH_MONOCHANNEL,
	CH_LEFTTOALL,
	CH_RIGHTTOALL
};

#ifndef QVODPLAYMEDIA_EXPORTS
static GUID IID_ISoundtrans = {0x7da9cece, 0xa81a, 0x471d, {0x8c, 0xd3, 0x98, 0x73, 0x6c, 0xba, 0x5, 0xdc}};
#else
EXTERN_GUID(IID_ISoundtrans, 0x7da9cece, 0xa81a, 0x471d, 0x8c, 0xd3, 0x98, 0x73, 0x6c, 0xba, 0x5, 0xdc);
#endif
interface ISoundPost
{
	//设置（获取）音频声道(左、右、混合声道的设置)
	virtual QVOD_ERROR PutChannel(Channel channel) = 0;
	virtual QVOD_ERROR GetChannel(Channel &channel) = 0;

	//设置（获取）静音标识
	virtual QVOD_ERROR PutMute(bool bmute) = 0;
	virtual QVOD_ERROR GetMute(bool &bmute) = 0;

	//设置（获取）声音延迟
	virtual QVOD_ERROR PutSoundDelay(LONGLONG delaytime) = 0;//单位ms
	virtual QVOD_ERROR GetSoundDelay(LONGLONG &delaytime) = 0;

	//设置（获取）声音延迟
	virtual QVOD_ERROR PutSoundEnlarge(long lenlarge) = 0;
	virtual QVOD_ERROR GetSoundEnlarge(long &lenlarge) = 0;

	//设置（获取）音效均衡
	virtual QVOD_ERROR PutEqualization(int *plenlarge) = 0;
	virtual QVOD_ERROR GetEqualization(int *plenlarge) = 0;

	//开启（关闭）音效均衡
	virtual QVOD_ERROR PutEqualizationEnable(bool enable) = 0;
	virtual QVOD_ERROR GetEqualizationEnable(bool &enable) = 0;

	//设置（获取）音效平衡
	virtual QVOD_ERROR PutVolumeBalance(LONG balance) = 0;
	virtual QVOD_ERROR GetVolumeBalance(LONG &balance) = 0;

#ifdef CHANNEAL_TRANSLATION
	//设置（获取）当前声道数
	virtual QVOD_ERROR PutChannelNum(float channel) = 0;
	virtual QVOD_ERROR GetChannelNum(float* pchannel) = 0;
#endif
};

#endif