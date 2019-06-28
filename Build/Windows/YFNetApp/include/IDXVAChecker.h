#ifndef IDXVACHECKER_H_
#define IDXVACHECKER_H_
//硬件加速所支持的编码类型
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
	wchar_t VideoCardDisp[512];//显卡配置信息
	bool bHD_Acceleration;//硬件加速开启状态
	bool bDD_Acceleration;//DirectDraw开启状态
	VideoCardInfo():bHD_Acceleration(false),bDD_Acceleration(true)
	{
		memset(VideoCardDisp, 0, sizeof(wchar_t) * 512);
	}
};
interface IDXVAChecker
{
	//获取显卡信息
	virtual QVOD_ERROR GetVideoCardInfo(VideoCardInfo *pVideoCardInfo) = 0;

	//获取显卡支持的DXVA编码子类型
	virtual bool GetDXVASupport(HD_CODEC encode, //[in],视频编码类型
		                        DWORD *pCount = NULL, //[in],Guid数组中可容纳的最大guid数(建议值：30),[out]函数返回Guid数组中的有效guid数
		                        GUID* pGUIDArray = NULL) = 0;//[in],已经分配好内存的guid数组指针,[out],函数返回指定编码类型显卡所支持的硬解子类型
	
	//获取显卡支持的DXVA编码类型
	virtual bool GetDXVASupport(DWORD &Dxvaflag) = 0;//[in, out],与上面所定义的宏相对应

	//打开DirectDraw的硬件加速
	virtual bool OpenDirectDrawAcceleration() = 0;

	//是否打开硬件加速
	virtual void SetDXVAOpen(bool bIsOpen) = 0;

	//重新检测显卡
	virtual bool ReCheck() = 0;

	//检测DXVASubType
	virtual bool CheckDXVAGUID(GUID* pSubGuid, GUID *pFormat = NULL) = 0;

};

#endif