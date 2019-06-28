//-------------------------------------------------
//	Desc: IVideo.h
//  Date: 2010.06.30
//	Author: zhaokui
//-------------------------------------------------
  
//#ifndef __IVIDEO_H__
//#define __IVIDEO_H__
#pragma once
#include "streams.h"
#ifdef __cplusplus
extern "C" {
#endif

	struct tagPOSTVIDEOPARAM		 
	{																//范围			默认值
		int nReduceNoise;	    //降噪数量							20-150			35
		int nGamma;				//gama数量							500--2000		35
		int nSharpenCount;		//锐化数量							0--100			50
		int nSharpenThreshold;  //锐化阈值							0--20			5	

		//反交错
		int nBob;		 //bob反交错									0--5			0
		int nTomsMoComp; //TOMSMOCOMP 反交错参数						0--25			3
		BOOL bTomsMoCompSmooth;//TOMSMOCOMP 的滤波效果开关							TRUE	

		int  nBrightness;//亮度										-127--127		0	
		int  nSaturation;//饱和										-127--127		0	
		int  nContrast;	 //对比										-127--127		0
		int  nChoma;		 //色调										0--616			0
	};
//  以下效果 分YUY2 、YV12下只实现了部分				    YUY2       YV12						 
#define		POST_NOEFFECT		   0x00000000   //去掉所有效果		 						
#define		POST_REDUCENOICE	   0x00000001	//去噪				 Y			N   0		 
#define		POST_VIDEOENHANCED	   0x00000002	//视频增强			 Y			Y   1		 	
#define		POST_GAMMA			   0x00000004	//GAMMA校正			 Y			N	2		 
#define		POST_SHARPEN		   0x00000008	//锐化				 Y			Y	3		
#define		POST_BOB			   0x00000100	//bob反交错           Y			N	8
#define		POST_MOCOMP2		   0x00000200	//MoComp2反交错       Y			N	9
#define		POST_TOMSMOCOMP		   0x00000400	//TomsMoComp反交错	  Y			N	10
#define		POST_BRIGHTNESS		   0x00000800	//亮度				  Y			Y	11
#define     POST_SATURATION		   0x00001000   //饱和				  Y			Y	12	
#define     POST_CONTRAST		   0x00002000	//对比度				  Y			Y	13	
#define     POST_CHROMA			   0x00004000   //色调				  Y			Y	14	
	DECLARE_INTERFACE_(IPostVideoEffect, IUnknown)
	{
		////////////////////////filter 参数设置//////////////////////////////////////
		//说明：须在fitler未连接之前设置
		STDMETHOD(SetBgrSnapShotMode) (THIS_ BOOL bBgrMode) PURE; 
		STDMETHOD(GetBgrSnapShotMode) (THIS_ BOOL *bBgrMode) PURE;


		/////////////////////////图像效果设置///////////////////////////////////////
		//效果开关和参数设置
		//[in] effect
		//[in] param
		STDMETHOD(SetEffect) (THIS_ unsigned long   effect,	tagPOSTVIDEOPARAM *param) PURE;
		//[out] effect
		//[out] param
		STDMETHOD(GetEffect) (THIS_ unsigned long   *effect,tagPOSTVIDEOPARAM *param) PURE;

		////////////////////////翻转和旋转///////////////////////////////////////
		//SetFlip
		//bHorizontal:水平翻转  	默认 FALSE
		//bVertical  ：垂直翻转	默认 FALSE
		//nAngel	 :旋转角度  取值范围：90的倍数 可正数或负数，默认值 0	
		STDMETHOD(SetFlip)(THIS_ BOOL bHorizontal,BOOL bVertical,int nAngel);
		STDMETHOD(GetFlip)(THIS_ BOOL *bHorizontal,BOOL *bVertical,int *nAngel);


		/////////////////////////////抓图////////////////////////////////////////
		//先抓图到内存，接着可以获取图像的数据，或者保存图像到文件。
		//抓到的图像数据一直保存，直到下一次抓图被覆盖 ,也可以手动清空

		//抓图到内存
		//[in] nTimeout  单位：毫秒 前目前可接受的最大值3000毫秒  推荐值为100毫秒以上
		//[out]	outBitmapInfo 用来填充图像头信息,		可为NULL
		//[out]	nBufferLen    用来填充图像数据大小	可为NULL
		STDMETHOD(SnapShot)(THIS_ unsigned long nTimeout,BITMAPINFOHEADER * outBitmapInfo,int *nBufferLen) PURE;	//抓图

		//获取数据(RGB32)
		//[out] outBuffer 用来填充数据的内存	
		//[in]	nBufferLen 已分配内存大小 （为了防止写越界）
		STDMETHOD(GetSnapshotBuffer)(THIS_ unsigned char *outBuffer,int nBufferLen)PURE;//最后获取图像的数据

		//保存到文件
		//[in] szPath 保存图的图径
		STDMETHOD(SaveSnapshot) (THIS_ wchar_t *szPath) PURE;	

		//手动清空抓图时保存的图像数据，以节省内存  在下一次抓图时也会自动覆盖
		STDMETHOD(ClearSnapshotBuffer) (THIS_) PURE;
	};


	DECLARE_INTERFACE_(IPostVideoSetting, IUnknown)
	{
		//指定输入pin的媒体类型，
		STDMETHOD(SetForceMediaType)(THIS_ BOOL bForce, GUID mtIn); 
		STDMETHOD(GetForceMediaType)(THIS_ BOOL *bForce, GUID *mtIn); 

	};

#ifdef __cplusplus
}
#endif

//#endif 
