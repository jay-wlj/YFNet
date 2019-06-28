#ifndef IDXVACHECKER_H_
#define IDXVACHECKER_H_
//Ӳ��������֧�ֵı�������
#define MARK_HD_H261      1    //H261
#define MARK_HD_H263      2    //H263
#define MARK_HD_MPEG1     4    //MPEG1
#define MARK_HD_MPEG2     8    //MPEG2_VIDEO
#define MARK_HD_H264      16   //H264, AVC1, x264
#define MARK_HD_WMV8      32   //WMV8
#define MARK_HD_WMV9      64   //WMV9
#define MARK_HD_VC_1      128  //WVC1
enum HD_CODEC
{
	HD_H261,
	HD_H263,
	HD_MPEG1,
	HD_MPEG2,
	HD_H264,
	HD_WMV8,
	HD_WMV9,
	HD_VC_1,
	HD_ALL
};

enum DXVA_TYPE
{
	DXVA_TYPE_NULL,
	DXVA_TYPE1,
	DXVA_TYPE2,
	DXVA_TYPE1_2
};
struct VideoCardInfo
{
	wchar_t VideoCardDisp[512];//�Կ�������Ϣ
	bool bHD_Acceleration;//Ӳ�����ٿ���״̬
	bool bDD_Acceleration;//DirectDraw����״̬
	VideoCardInfo():bHD_Acceleration(false),bDD_Acceleration(true)
	{
		memset(VideoCardDisp, 0, sizeof(wchar_t) * 512);
	}
};
interface IDXVAChecker
{
	//��ȡ�Կ���Ϣ
	virtual QVOD_ERROR GetVideoCardInfo(VideoCardInfo *pVideoCardInfo) = 0;

	//��ȡ�Կ�֧�ֵ�DXVA����������
	virtual bool GetDXVASupport(HD_CODEC encode, //[in],��Ƶ��������
		                        DWORD *pCount = NULL, //[in],Guid�����п����ɵ����guid��(����ֵ��30),[out]��������Guid�����е���Чguid��
		                        GUID* pGUIDArray = NULL) = 0;//[in],�Ѿ�������ڴ��guid����ָ��,[out],��������ָ�����������Կ���֧�ֵ�Ӳ��������
	
	//��ȡ�Կ�֧�ֵ�DXVA��������
	virtual bool GetDXVASupport(DWORD &Dxvaflag) = 0;//[in, out],������������ĺ����Ӧ

	//��DirectDraw��Ӳ������
	virtual bool OpenDirectDrawAcceleration() = 0;

	//�Ƿ��Ӳ������
	virtual void SetDXVAOpen(bool bIsOpen) = 0;

	//���¼���Կ�
	virtual bool ReCheck() = 0;

	//���DXVASubType
	virtual bool CheckDXVAGUID(GUID* pSubGuid, GUID *pFormat = NULL) = 0;

};

#endif