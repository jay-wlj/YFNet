#pragma once
#ifndef _IMEDIAPOST_H_
#define _IMEDIAPOST_H_
#include "..\include\QvodError.h"
#define CHANNEAL_TRANSLATION
#define USE_D3D_PIXEL_SHADER
#define VIDEO_ENHANCE_WIPER
enum DEINTERLACE_MODE
{
	DTL_BOB,		//BOB������
	DTL_MOCOMP2,	//MoComp2������
	DTL_TOMSMOCOMP, //TomsMoComp������
	DTL_NULL		//������������
};

enum SubTimeType
{
	SUB_TIME_NOUSER = 0,	//��ʹ��
	SUB_TIME_CUR,			//��ǰʱ��
	SUB_TIME_END,			//ʣ��ʱ��
	SUB_TIME_SYSTEM			//ϵͳʱ��
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

    // 2Dת3D����, �Ӳ�ϵ�� k =(0.0~ 0.1)  Ĭ��0.015
    To3D_RedBlue_LeftRight,       // 2Dת3D_����_����,
    To3D_RedBlue_RightLeft,       // 2Dת3D_����_����
    To3D_RedGreen_LeftRight,      // 2Dת3D_����_����
    To3D_RedGreen_RightLeft,      // 2Dת3D_����_����
    To3D_BrownBlue_LeftRight,     // 2Dת3D_����_����
    To3D_BrownBlue_RightLeft,     // 2Dת3D_����_����

    // 3D��Ƶ����, �Ӳ�ϵ�� k = (0.0~0.05) Ĭ��0.0
    RedBlue_LeftRight,            // ����ת����
    RedBlue_RightLeft,            // ����ת����
    RedGreen_LeftRight,           // ����ת����
    RedGreen_RightLeft,           // ����ת����
    BrownBlue_LeftRight,          // ����ת����
    BrownBlue_RightLeft,          // ����ת����

    RedBlue_TopBottom,            // ����ת����
    RedBlue_BottomTop,            // ����ת����
    RedGreen_TopBottom,           // ����ת����
    RedGreen_BottomTop,           // ����ת����
    BrownBlue_TopBottom,          // ����ת����
    BrownBlue_BottomTop,          // ����ת����

    // 3Dת2D����, �Ӳ�ϵ��������
    To2D_LeftOnly,                // ����ת����
    To2D_RightOnly,               // ����ת����
    To2D_TopOnly,                 // ����ת�ϻ���
    To2D_BottomOnly               // ����ת�ϻ���
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
	P_SUB_MIDLLE, //X������� Y�����Զ���
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
	//��Ƶ��ǿ
#ifdef VIDEO_ENHANCE_WIPER
	virtual QVOD_ERROR SetVideoEnhance(bool enable,  OPTIONENHANCE effect) = 0;
#else
	virtual QVOD_ERROR SetVideoEnhance(bool enable) = 0;
#endif
	virtual QVOD_ERROR GetVideoEnhance(bool &enable) = 0;

	//ȥ��
	virtual QVOD_ERROR ReduceNoise(bool enable,		//������ر�
								   int ReduceNum) = 0;//������������Χ20-150,35��
	virtual QVOD_ERROR GetReduceNoiseFlag(bool &enable,
								   int &ReduceNum) = 0;

	//GAMMAУ��
	virtual QVOD_ERROR Gamma(bool enable,		//������ر�
							 int nGamma) = 0;//������������Χ20-150,35��
	virtual QVOD_ERROR GetGammaFlag(bool &enable, 
									int &nGamma) = 0;

	//��
	virtual QVOD_ERROR Sharpen(bool enable,			//������ر�
							   int nSharpenCount,	//������(��Χ0-100��Ĭ��ֵ50)
							   int nSharpenThreshold) = 0;//����ֵ����Χ0-20��Ĭ��ֵ5��
	virtual QVOD_ERROR GetSharpenFlag(bool &enable, //������ر�
									  int &nSharpenCount, //������ر�
									  int &nSharpenThreshold) = 0;

	//������
	virtual QVOD_ERROR Deinterlace(DEINTERLACE_MODE Dtlmode,//������ģʽ����DEINTERLACE_MODE����
								   int Dtlparam) = 0;//���������BOBģʽ����Χ0-5��Ĭ��ֵ0����TomsMoCompģʽ����Χ0-25��Ĭ��ֵ3��
	virtual QVOD_ERROR GetDeinterlaceFlag(DEINTERLACE_MODE &Dtlmode,
										  int &Dtlparam) = 0;

	//����
	virtual QVOD_ERROR SetBrightness(int nBrightness) = 0;//��Χ-127-127,Ĭ��ֵ0
	virtual QVOD_ERROR GetBrightness(int &nBrightness) = 0;

	//���Ͷ�
	virtual QVOD_ERROR SetSaturation(int nSaturation) = 0;//��Χ-127-127,Ĭ��ֵ0
	virtual QVOD_ERROR GetSaturation(int &nSaturation) = 0;

	//�Աȶ�
	virtual QVOD_ERROR SetContrast(int nContrast) = 0;//��Χ-127-127,Ĭ��ֵ0
	virtual QVOD_ERROR GetContrast(int &nContrast) = 0;

	//ɫ��
	virtual QVOD_ERROR SetHue(int nHue) = 0;//��Χ0-616,Ĭ��ֵ0
	virtual QVOD_ERROR GetHue(int &nHue) = 0;

	////��ת
	//virtual QVOD_ERROR Flip(int nAngel) = 0;//��Χ0-616,Ĭ��ֵ0
	//virtual QVOD_ERROR GetFlip(int &nAngel) = 0;

	//��ת
	virtual QVOD_ERROR Flip(int nAngel) = 0;//��Χ90�ı���,Ĭ��ֵ0
	virtual QVOD_ERROR GetFlip(int &nAngel) = 0;

	//ˮƽ��ת
	virtual QVOD_ERROR HorizontalReverse(BOOL bHorizontal) = 0;
	virtual QVOD_ERROR GetHorizontalReverse(BOOL &bHorizontal) = 0;

	//��ֱ��ת
	virtual QVOD_ERROR VerticalReverse(BOOL bVertical) = 0;
	virtual QVOD_ERROR GetVerticalReverse(BOOL &bVertical) = 0;

	//2Dת3D
	virtual QVOD_ERROR Set3DView(OPTION3DVIEW opt3DView, float kView) PURE;//OPTION3DVIEW opt3DView:������λ�ã�float kView���Ӳ�ϵ�������OPTION3DVIEWע��
	virtual QVOD_ERROR Get3DView(OPTION3DVIEW* opt3DView, float* kView) PURE;
	//��ͼ
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

	//��ʾ�����أ���Ļ
	virtual QVOD_ERROR PutSubVisableFlag(bool bVisable, MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetSubVisableFlag(bool &bVisable,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//�����Ļ
	virtual QVOD_ERROR ClearSubtitle() = 0;

	//���ã���ȡ����Ļ����
	virtual QVOD_ERROR PutSubFont(LOGFONT sf,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetSubFont(LOGFONT &sf,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//���ã���ȡ��������ɫ
	virtual QVOD_ERROR PutFontColor(DWORD fcolor,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetFontColor(DWORD &fcolor,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//���ã���ȡ������������ɫ
	virtual QVOD_ERROR PutOutLineColor(DWORD fcolor,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetOutlineColor(DWORD &fcolor,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//���ã���ȡ����Ļλ��
	virtual QVOD_ERROR PutSubPlace(ESubPlace place, int cx, int cy,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetSubPlace(ESubPlace &place, int &cx, int &cy,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//���ã���ȡ����Ļ�ӳ�(��ǰ)
	virtual QVOD_ERROR PutSubDelay(int delaytime,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual QVOD_ERROR GetSubDelay(int &delaytime,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	//��ʾ��ǰ��ʣ�ࡢϵͳ��ʱ��
	virtual QVOD_ERROR PutSubTime(SubTimeType subTimeType) = 0;
	virtual QVOD_ERROR GetSubTime(SubTimeType &subTimeType) = 0;

	//���ã���ȡ����ǰ��ʣ�ࡢϵͳ��ʱ������
	virtual QVOD_ERROR PutTimeFont(LOGFONT lf) = 0;
	virtual QVOD_ERROR GetTimeFont(LOGFONT &lf) = 0;
#else
	//���ã���ȡ����Ļ·��
	virtual QVOD_ERROR PutSubPath(WCHAR* wszPath) = 0;
	virtual QVOD_ERROR GetSubPath(WCHAR* wszPath, int &pbuffersize) = 0;

	//��ʾ�����أ���Ļ
	virtual QVOD_ERROR PutSubVisableFlag(bool bVisable) = 0;
	virtual QVOD_ERROR GetSubVisableFlag(bool &bVisable) = 0;
	
	//�����Ļ
	virtual QVOD_ERROR ClearSubtitle() = 0;

	//���ã���ȡ����Ļ����
	virtual QVOD_ERROR PutSubFont(LOGFONT sf) = 0;
	virtual QVOD_ERROR GetSubFont(LOGFONT &sf) = 0;

	//���ã���ȡ��������ɫ
	virtual QVOD_ERROR PutFontColor(DWORD fcolor) = 0;
	virtual QVOD_ERROR GetFontColor(DWORD &fcolor) = 0;

	//���ã���ȡ������������ɫ
	virtual QVOD_ERROR PutOutLineColor(DWORD fcolor) = 0;
	virtual QVOD_ERROR GetOutlineColor(DWORD &fcolor) = 0;

	//���ã���ȡ����Ļλ��
	virtual QVOD_ERROR PutSubPlace(ESubPlace place, int cx, int cy) = 0;
	virtual QVOD_ERROR GetSubPlace(ESubPlace &place, int &cx, int &cy) = 0;

	//���ã���ȡ����Ļ�ӳ�(��ǰ)
	virtual QVOD_ERROR PutSubDelay(int delaytime) = 0;
	virtual QVOD_ERROR GetSubDelay(int &delaytime) = 0;

	//��ʾ��ǰ��ʣ�ࡢϵͳ��ʱ��
	virtual QVOD_ERROR PutSubTime(SubTimeType subTimeType) = 0;
	virtual QVOD_ERROR GetSubTime(SubTimeType &subTimeType) = 0;

	//���ã���ȡ����Ļ�ӳ�
	virtual QVOD_ERROR PutTimeFont(LOGFONT lf) = 0;
	virtual QVOD_ERROR GetTimeFont(LOGFONT &lf) = 0;
#endif
	//���ò���ʱ��
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
	//���ã���ȡ����Ƶ����(���ҡ��������������)
	virtual QVOD_ERROR PutChannel(Channel channel) = 0;
	virtual QVOD_ERROR GetChannel(Channel &channel) = 0;

	//���ã���ȡ��������ʶ
	virtual QVOD_ERROR PutMute(bool bmute) = 0;
	virtual QVOD_ERROR GetMute(bool &bmute) = 0;

	//���ã���ȡ�������ӳ�
	virtual QVOD_ERROR PutSoundDelay(LONGLONG delaytime) = 0;//��λms
	virtual QVOD_ERROR GetSoundDelay(LONGLONG &delaytime) = 0;

	//���ã���ȡ�������ӳ�
	virtual QVOD_ERROR PutSoundEnlarge(long lenlarge) = 0;
	virtual QVOD_ERROR GetSoundEnlarge(long &lenlarge) = 0;

	//���ã���ȡ����Ч����
	virtual QVOD_ERROR PutEqualization(int *plenlarge) = 0;
	virtual QVOD_ERROR GetEqualization(int *plenlarge) = 0;

	//�������رգ���Ч����
	virtual QVOD_ERROR PutEqualizationEnable(bool enable) = 0;
	virtual QVOD_ERROR GetEqualizationEnable(bool &enable) = 0;

	//���ã���ȡ����Чƽ��
	virtual QVOD_ERROR PutVolumeBalance(LONG balance) = 0;
	virtual QVOD_ERROR GetVolumeBalance(LONG &balance) = 0;

#ifdef CHANNEAL_TRANSLATION
	//���ã���ȡ����ǰ������
	virtual QVOD_ERROR PutChannelNum(float channel) = 0;
	virtual QVOD_ERROR GetChannelNum(float* pchannel) = 0;
#endif
};

#endif