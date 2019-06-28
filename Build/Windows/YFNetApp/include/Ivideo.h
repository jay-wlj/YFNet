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
		//int  nParam;		//ͨ�ò���
	};
#define		NOEFFECT		   0x00000000    //ȥ������Ч��
#define		REDUCENOICE	       0x00000001	//ȥ��
#define		VIDEOENHANCED	   0x00000002	//��Ƶ��ǿ
#define		GAMMA			   0x00000004	//GAMMAУ��
#define		SHARPEN		       0x00000008	//��
#define		HORIZONFLIP		   0x00000010	//ˮƽ��ת
#define		VERTICALFLIP	   0x00000020	//��ֱ��ת
#define		ANGLEFLIP		   0x00000040	//��ת
#define		BOB			       0x00000080	//bob������ 
#define		MOCOMP2			   0x00000100	//MoComp2������
#define		TOMSMOCOMP		   0x00000200	//TomsMoComp������


	const IID IID_IPostVideoEffect = {0xc8300751, 0x2e65, 0x45a9, {0xbb, 0x12, 0xd8, 0x99, 0xd3, 0xe8, 0x6c, 0x35}};
	DECLARE_INTERFACE_(IPostVideoEffect, IUnknown)
	{
		/*************************************************
		Function:      // SetEffect
		Description:   // ������ƵЧ��
		Input:      
		int effect,	 //32λ������ѡ���Ч�����ͣ���Ӧλ����ٿ���Ч���Ŀ���
		tagPOSTVIDEOPARAM *param	//Ч�������ṹ��ָ��      
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
		Description:   // ץͼ        
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
		Description:   // ����ץͼ����ͼƬ·��
		Input:			
		szPath  ���õ�·��ָ��
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
	//����
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
