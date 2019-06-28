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
	{																//��Χ			Ĭ��ֵ
		int nReduceNoise;	    //��������							20-150			35
		int nGamma;				//gama����							500--2000		35
		int nSharpenCount;		//������							0--100			50
		int nSharpenThreshold;  //����ֵ							0--20			5	

		//������
		int nBob;		 //bob������									0--5			0
		int nTomsMoComp; //TOMSMOCOMP ���������						0--25			3
		BOOL bTomsMoCompSmooth;//TOMSMOCOMP ���˲�Ч������							TRUE	

		int  nBrightness;//����										-127--127		0	
		int  nSaturation;//����										-127--127		0	
		int  nContrast;	 //�Ա�										-127--127		0
		int  nChoma;		 //ɫ��										0--616			0
	};
//  ����Ч�� ��YUY2 ��YV12��ֻʵ���˲���				    YUY2       YV12						 
#define		POST_NOEFFECT		   0x00000000   //ȥ������Ч��		 						
#define		POST_REDUCENOICE	   0x00000001	//ȥ��				 Y			N   0		 
#define		POST_VIDEOENHANCED	   0x00000002	//��Ƶ��ǿ			 Y			Y   1		 	
#define		POST_GAMMA			   0x00000004	//GAMMAУ��			 Y			N	2		 
#define		POST_SHARPEN		   0x00000008	//��				 Y			Y	3		
#define		POST_BOB			   0x00000100	//bob������           Y			N	8
#define		POST_MOCOMP2		   0x00000200	//MoComp2������       Y			N	9
#define		POST_TOMSMOCOMP		   0x00000400	//TomsMoComp������	  Y			N	10
#define		POST_BRIGHTNESS		   0x00000800	//����				  Y			Y	11
#define     POST_SATURATION		   0x00001000   //����				  Y			Y	12	
#define     POST_CONTRAST		   0x00002000	//�Աȶ�				  Y			Y	13	
#define     POST_CHROMA			   0x00004000   //ɫ��				  Y			Y	14	
	DECLARE_INTERFACE_(IPostVideoEffect, IUnknown)
	{
		////////////////////////filter ��������//////////////////////////////////////
		//˵��������fitlerδ����֮ǰ����
		STDMETHOD(SetBgrSnapShotMode) (THIS_ BOOL bBgrMode) PURE; 
		STDMETHOD(GetBgrSnapShotMode) (THIS_ BOOL *bBgrMode) PURE;


		/////////////////////////ͼ��Ч������///////////////////////////////////////
		//Ч�����غͲ�������
		//[in] effect
		//[in] param
		STDMETHOD(SetEffect) (THIS_ unsigned long   effect,	tagPOSTVIDEOPARAM *param) PURE;
		//[out] effect
		//[out] param
		STDMETHOD(GetEffect) (THIS_ unsigned long   *effect,tagPOSTVIDEOPARAM *param) PURE;

		////////////////////////��ת����ת///////////////////////////////////////
		//SetFlip
		//bHorizontal:ˮƽ��ת  	Ĭ�� FALSE
		//bVertical  ����ֱ��ת	Ĭ�� FALSE
		//nAngel	 :��ת�Ƕ�  ȡֵ��Χ��90�ı��� ������������Ĭ��ֵ 0	
		STDMETHOD(SetFlip)(THIS_ BOOL bHorizontal,BOOL bVertical,int nAngel);
		STDMETHOD(GetFlip)(THIS_ BOOL *bHorizontal,BOOL *bVertical,int *nAngel);


		/////////////////////////////ץͼ////////////////////////////////////////
		//��ץͼ���ڴ棬���ſ��Ի�ȡͼ������ݣ����߱���ͼ���ļ���
		//ץ����ͼ������һֱ���棬ֱ����һ��ץͼ������ ,Ҳ�����ֶ����

		//ץͼ���ڴ�
		//[in] nTimeout  ��λ������ ǰĿǰ�ɽ��ܵ����ֵ3000����  �Ƽ�ֵΪ100��������
		//[out]	outBitmapInfo �������ͼ��ͷ��Ϣ,		��ΪNULL
		//[out]	nBufferLen    �������ͼ�����ݴ�С	��ΪNULL
		STDMETHOD(SnapShot)(THIS_ unsigned long nTimeout,BITMAPINFOHEADER * outBitmapInfo,int *nBufferLen) PURE;	//ץͼ

		//��ȡ����(RGB32)
		//[out] outBuffer ����������ݵ��ڴ�	
		//[in]	nBufferLen �ѷ����ڴ��С ��Ϊ�˷�ֹдԽ�磩
		STDMETHOD(GetSnapshotBuffer)(THIS_ unsigned char *outBuffer,int nBufferLen)PURE;//����ȡͼ�������

		//���浽�ļ�
		//[in] szPath ����ͼ��ͼ��
		STDMETHOD(SaveSnapshot) (THIS_ wchar_t *szPath) PURE;	

		//�ֶ����ץͼʱ�����ͼ�����ݣ��Խ�ʡ�ڴ�  ����һ��ץͼʱҲ���Զ�����
		STDMETHOD(ClearSnapshotBuffer) (THIS_) PURE;
	};


	DECLARE_INTERFACE_(IPostVideoSetting, IUnknown)
	{
		//ָ������pin��ý�����ͣ�
		STDMETHOD(SetForceMediaType)(THIS_ BOOL bForce, GUID mtIn); 
		STDMETHOD(GetForceMediaType)(THIS_ BOOL *bForce, GUID *mtIn); 

	};

#ifdef __cplusplus
}
#endif

//#endif 
