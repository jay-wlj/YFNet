//-------------------------------------------------
//	Desc: IVideo.h
//  Date: 2010.06.30
//	Author: zhaokui
//-------------------------------------------------

#ifndef __IVIDEO_H__
#define __IVIDEO_H__
//#include <streams.h>
#ifdef __cplusplus
extern "C" {
#endif

	struct tagPOSTVIDEOPARAM
	{
		int nGamma;
		int nReduceNoise;
		int nSharpenCount;
		int nSharpenThreshold;
		int nBob;
		int nTomsMoComp;
		bool bVerticalSmooth;
		//int  nParam;		//通用参数
	};
#define		NOEFFECT		   0x00000000    //去掉所有效果
#define		REDUCENOICE	       0x00000001	//去噪
#define		VIDEOENHANCED	   0x00000002	//视频增强
#define		GAMMA			   0x00000004	//GAMMA校正
#define		SHARPEN		       0x00000008	//锐化
#define		HORIZONFLIP		   0x00000010	//水平翻转
#define		VERTICALFLIP	   0x00000020	//垂直翻转
#define		ANGLEFLIP		   0x00000040	//旋转
#define		BOB			       0x00000080	//bob反交错 
#define		MOCOMP2			   0x00000100	//MoComp2反交错
#define		TOMSMOCOMP		   0x00000200	//TomsMoComp反交错


	const IID IID_IPostVideoEffect = {0xc8300751, 0x2e65, 0x45a9, {0xbb, 0x12, 0xd8, 0x99, 0xd3, 0xe8, 0x6c, 0x35}};
	DECLARE_INTERFACE_(IPostVideoEffect, IUnknown)
	{
		/*************************************************
		Function:      // SetEffect
		Description:   // 设置视频效果
		Input:      
		int effect,	 //32位二进制选择的效果类型，相应位的真假控制效果的开关
		tagPOSTVIDEOPARAM *param	//效果参数结构体指针      
		*************************************************/
		STDMETHOD(SetEffect) (THIS_
			unsigned long   effect,								
			tagPOSTVIDEOPARAM *param	
			) PURE;
		STDMETHOD(GetEffect) (THIS_
			unsigned long   *effect,								
			tagPOSTVIDEOPARAM *param	
			) PURE;
		/*************************************************
		Function:      // SnapShot
		Description:   // 抓图        
		*************************************************/
		STDMETHOD(SnapShot) (THIS_
			) PURE;

		STDMETHOD(GetFrameSize) (THIS_
			long * outFrameSize
			) PURE;
		STDMETHOD(GetBitmapInfoHeader) (THIS_
			BITMAPINFOHEADER * outBitmapInfo
			) PURE;
		/*************************************************
		Function:      // SetSnapPath
		Description:   // 设置抓图保存图片路径
		Input:			
		szPath  设置的路径指针
		Output:              
		*************************************************/
		STDMETHOD(SetSnapPath) (THIS_
			wchar_t *szPath
			) PURE;

		STDMETHOD(GetSnapPath) (THIS_
			wchar_t *szPath
			) PURE;

		STDMETHOD(GetRGB32Buffer)(THIS_
			unsigned char *outBuffer
			,unsigned long nTimeout
			)PURE;
	//缩放
		STDMETHOD(SetFormat)(THIS_
			)PURE;
		STDMETHOD(Scale)(THIS_
			int width,
			int height
			)PURE;
		STDMETHOD(GetScaleParam)(THIS_
			int *width,
			int *height
			)PURE;
		STDMETHOD(ResetAll)(THIS_
			)PURE;
	};
#ifdef __cplusplus
}
#endif

#endif 
