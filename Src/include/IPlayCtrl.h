#pragma once
#ifdef PLAYCTRL_EXPORTS
#define PLAYCRTL_API extern "C" __declspec(dllexport)
#else
#define PLAYCRTL_API extern "C" __declspec(dllimport)
#endif


enum e_PlayControl
{
	PLAYCTL_PLAY,			//播放
	PLAYCTL_PAUSE,			//暂停
	PLAYCTL_STOP,			//停止播放
};
enum e_T3DSet
{
	T3D_FORMAT =1,
	T3D_MODE = 2,
	T3D_ANGLE =4,
	T3D_PARALAXPOS = 8,
	T3D_CHANNEL = 16,
};

enum e_2T3DFormat
{
	T3D_NO3D,					//无变化
	T3D_2T3D,					//2D转3D
	T3D_3DPLAY,				//3D播放
	T3D_3DT2D,					//3D转2D

	T3D_Max,
};
enum e_2T3DMode
{
	T3D_REDBLUE,				//红蓝
	T3D_REDGREEN,			//红绿
	T3D_BROWNBLUE,		//棕蓝

	Mode_Max
};

enum e_2T3DViewAngle
{
	T3D_LEFTRIGHT,				//左右
	T3D_RIGHTLEFT,				//右左
	T3D_UPDOWN,				//上下
	T3D_DOWNUP,				//下上

	Angle_Max
};

enum e_2T3DViewChannel
{
	T3D_LEFT,				//左
	T3D_RIGHT,			//右
	T3D_UP,				//上	
	T3D_DOWN,			//下

	Channel_Max
};

typedef struct _3DSet_
{
	_3DSet_()
	{
		ReSet();
	}

	void ReSet()
	{
		eMode = T3D_REDBLUE;
		fParalaxPos = 0.0f;
		eAngle = T3D_LEFTRIGHT;
		eChannel =T3D_LEFT;
	}
	e_2T3DMode eMode;
	float				fParalaxPos;
	e_2T3DViewAngle	eAngle;
	e_2T3DViewChannel	eChannel;
}T3DSet, *PT3DSet;

//色彩
enum e_VideoColorSet
{
	VIDEO_BRIGHTNESS = 1,		//亮度   (-127--127)
	VIDEO_SATURATION = 2,		//饱和度 (-127--127)
	VIDEO_CONTRAST = 4,			//对比度 (-127--127)
	VIDEO_HUE = 8,				//色调 (0--616)
};

enum e_FlipReverse
{
	VIDEO_FLIP = 1,					//旋转
	VIDEO_HREVERSE = 2,		//水平翻转
	VIDEO_VREVERSE = 4,		//垂直翻转
};

//字幕
enum e_Subtitle
{
	SUBTITLE_FONT = 1,				//字体
	SUBTITLE_CLR = 2,					//字体颜色
	SUBTITLE_OUTLINE_CLR = 4,	//字体轮廓颜色
	SUBTITLE_DELAY = 8,				//字幕延迟(超前)
};

//音频调节
enum e_AudioSet
{
	AUDIO_CHANNEL = 1,				//音频声道(左、右、混合声道的设置)
	AUDIO_CHANNEL_NUM = 2,		//当前声道数
	AUDIO_DELAY = 4,						//声音延迟
	AUDIO_ENLARGE = 8,					//声音放大
};

//音效均衡
enum e_Equalization
{
	EQ_ENALBE = 1,	   //是否开启音效均衡
	EQ_EFFECT = 2,	   //音效均衡设置
	EQ_BALANCE = 4, //音效平衡
};

enum e_DvdAudioSub
{
	DVD_AUDIO = 1,			//音频信息
	DVD_SUBTITLE = 2,		//字幕信息
};

enum e_DvdPlayPreNext
{
	DVD_PLAY_PRE,				//向前
	DVD_PLAY_NEXT,				//向后
};

enum e_PlayMediaInfo
{
	MEDIA_FILE_TYPE = 1,				//文件媒体类型
	MEDIA_FRAME_RATE = 2,				//文件媒体帧率
	MEDIA_VIDEOCODEC_TYPE = 4,			//文件媒体视频编码类型
};

//截图数据
typedef struct _CUT_IMG 
{
	_CUT_IMG()
	{
		lTime = 0;
		pBitmapData = NULL;
	}

	REFERENCE_TIME lTime;
	PBYTE pBitmapData;
}CutImg, *PCutImg;

//bit信息数据，通过QCM_PLAYCTRL_SETLOADINFO回调给UI处理
typedef struct _PROGESS_BITINFO
{
	_PROGESS_BITINFO()
	{
		memset(this, 0, sizeof(_PROGESS_BITINFO));
	}
	char *pBitInfo;
	int nPicNum;
	int nBuffer;
}ProgressBitInfo;

// 媒体文件类型
enum e_MediaType
{
	mt_unknow = 0,	// 未知类型
	mt_movie,		// 音视频
	mt_music,		// 纯音频
	mt_image,		// 图片
	mt_CD,			// CD唱片
	mt_DVD,		// DVD光碟
	mt_flash,		// flash
	mt_other		// 其他(保留)
};

// 媒体文件来源
enum e_MediaSource
{
	ms_unknow = 0,	// 未知类型
	ms_p2p,			// P2P来源
	ms_live,		// 直播
	ms_stream,		// 数据流
	ms_hdisk,		// 本地硬盘
	ms_cdrom,		// 本地光盘
	ms_LAN,			// 局域网
	ms_other		// 其他(保留)
};


interface PlayCtrl :public IQvodCallBack
{
public:
	/*********************************play******************************
		函数名:     SetParentWnd
		函数描述:   设置视频播放窗口的父窗口句柄
		参数1:      HWND hParentWnd，输入，父窗口句柄
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetParentWnd(HWND hParentWnd) = 0;

	/***************************************************************
		函数名:     SetCallBack
		函数描述:   设置回调的对象
		参数1:		 IQvodCallBack *pQvodCallBack，输入，回调对象
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetCallBack(IQvodCallBack *pQvodCallBack) = 0;
	
	/***************************************************************
		函数名:     QueryInterface
		函数描述:   查询接口
		参数1：     REFIID riid， 输入，接口IID，可供查询的有IID_IColortrans,IID_ISoundtrans,IID_ISubtrans三个接口
		参数2:      void **ppvIF, 输出，接口指针
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL __stdcall QueryInterface(REFIID riid, void **ppvIF) = 0;

public:
	//播放控制接口
	/***********************************************************************
		功能：播放
		参数1： lpszURL,媒体文件本地、网络路径
		参数2： E_MEDIA_TYPE mt = E_MT_VIDEO，播放媒体播放类型，默认为音视频
		参数3： rBeginPos,从指定位置开始播放
		参数4： e_MediaSource ms， 媒体文件来源，默认未知
		参数5： e_MediaType m，// 媒体文件类型，默认未知
		返回值：成功返回E_QVOD_S_OK,否则为失败
	************************************************************************/
	virtual QVOD_ERROR PlaySource(LPCTSTR lpszURL, E_MEDIA_TYPE mt = E_MT_VIDEO, REFERENCE_TIME rBeginPos = 0, e_MediaSource ms = ms_unknow,  e_MediaType m=mt_unknow) = 0;

	/***********************************************************************
		功能：播放
		返回值：TRUE,成功，FALS,失败
	************************************************************************/
	virtual BOOL Play() = 0;

	/***********************************************************************
		功能：暂停播放
		参数：无
		返回值：TRUE,成功，FALS,失败
	************************************************************************/
	virtual BOOL Pause() = 0;

	/***********************************************************************
		功能：播放暂停，若当前为播放状态，则调用后暂停状态，反之一样
		参数：无
		返回值：TRUE,成功，FALS,失败
	************************************************************************/
	virtual BOOL PlayPause() = 0;

	/***********************************************************************
		功能：停止播放
		参数：无
		返回值：TRUE,成功，FALS,失败
	************************************************************************/
	virtual BOOL Stop() = 0;

	/************************************************************************
		功能：获得当前播放状态
		参数：e_PlayState *playState，获得当前播放状态
		返回值：BOOL，TRUE：成功，FALS，失败
	************************************************************************/
	virtual BOOL GetPlayState(e_PlayState *playState) = 0;

	/**********************************************************
		功能:		设置(获取)当前媒体播放的时间点
		参数1:    long  lValue， 输入，要设置到的媒体文件播放时间点,单位为毫秒
		返回值：成功返回TRU,失败FALSE
	************************************************************/
	/*************************************************************
		函数名:     GetCurrentPos  
		函数描述:   获取当前播放位置(和播放时长)
		参数1:        REFERENCE_TIME *lValue, 输出，返回当前播放位置
		参数2:        REFERENCE_TIME *lDuraton, 输出，不为NULL时返回当前播放时长
		返回值：    成功返回TRUE，否则失败
	***************************************************************/
	virtual BOOL GetCurrentPos(REFERENCE_TIME *lValue, REFERENCE_TIME *lDuraton) = 0;
	virtual BOOL GetCurrentPos(REFERENCE_TIME *lValue) = 0;

	/*************************************************************
		函数名:     SetMute GetMute 
		函数描述:   设置(获取)播放器的静音标识
		参数1:      bool bValue，输入，静音为true，否则为false
		返回值：    成功返回TRUE，否则失败
	***************************************************************/
	virtual BOOL SetMute(bool bValue) = 0;
	virtual BOOL GetMute(bool *bValue) = 0;

	/***************************************************************
	函数名:     Restore
	函数描述:   视频窗口还原为初始状态
	返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL Restore() = 0;

	/***************************************************************
	函数名		AllocateColokey
	函数描述:	分配色键
	参数1：		DWORD dwColorkey， 输入，设置播放窗口的色键
	返回值：	无
	*****************************************************************/
	virtual void AllocateColokey(DWORD dwColorkey) = 0;

	virtual BOOL IsAudioEnable() = 0;			//是否支持音频调节（包括声道，音量放大设置等）
	
	/***************************************************************
	函数名:     SetBufferingTime                                
	函数描述:   设置播放缓冲数据长度
	参数1：     int nPreBufferTime, 播放前预缓冲数据的播放时长（单位：秒，最大长度为20s）
	参数1：     int nPlayBufferTime, 播放中缓冲数据的播放时长（单位：秒，最大长度为20s）
	返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetBufferingTime(int nPreBufferTime, int nPlayBufferTime) = 0;

	/***************************************************************
	函数名:     GetBufferingTime                                
	函数描述:   获取播放缓冲数据长度
	参数1：     int nPreBufferTime, 播放前预缓冲数据的播放时长（单位：秒，最大长度为20s）
	参数1：     int nPlayBufferTime, 播放中缓冲数据的播放时长（单位：秒，最大长度为20s）
	返回值：    无
	*****************************************************************/
	virtual void GetBufferingTime(int* pnPreBufferTime, int* pnPlayBufferTime) = 0;

public:	//媒体信息设置
	/***************************************************************
		函数名:     GetVolume
		函数描述:  设置（获取）播放器的音量
		参数1:      long lValue， 输入，0-100的音量级别
		返回值：   成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetVolume(long lValue) = 0;
	virtual BOOL GetVolume(long *lValue) = 0;

	/***************************************************************
		函数名:     SetPlaySpeed GetPlaySpeed
		函数描述:   设置（获取）播放器的播放速率
		参数1:      long lSpeed，输入，1：加速0.1个级别，-1，减速0.1个级别，取值范围为0.11-3.0，大于1表明在加速播放，小于1表明在减速播放，等于1正常播放
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetPlaySpeed(double lSpeed) = 0;
	virtual BOOL GetPlaySpeed(double *lSpeed) = 0;

	/***************************************************************
		函数名:     SetPlayFrame
		函数描述:   设置播放帧数
		参数1：     long lFrameNums， 输入，取值1，-1,1前进一帧，-1后退一帧
		参数1：     bool bsync， 输入，是否同步
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetPlayFrame(long lFrameNums, bool bsync = false) = 0;

	/***************************************************************
		函数名:     SetPlayOption
		函数描述:   设置（获取）播放选项（启	动硬件加速、启动视频调节、不启动硬件加速和视频调节）
		参数1:      e_Play_Option eOption，输入，启动硬件加速、启动视频后处理、不启动硬件加速和视频后处理
		返回值：    无
	*****************************************************************/
	virtual void SetPlayOption(e_Play_Option eOption, PlayOption_Param* pParam = NULL) = 0;
	virtual void GetPlayOption(e_Play_Option *eOption, PlayOption_Param* pParam = NULL) = 0;

	/***************************************************************
		函数名:     GetCurAudioRender
		函数描述:   设置（获取）当前使用的Audiorender
		参数1:      long lAudiooRender，输入，Audiorender索引（根据上面得到的）
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetCurAudioRender(long lAudiooRender) = 0;
	virtual BOOL GetCurAudioRender(long *lAudioRender) = 0;

	/***************************************************************
		函数名:     SetSubTitleStream
		函数描述:   设置字幕流
		参数1：     int iIndex， 输入，要设置的字幕流索引号
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetSubTitleStream(int iIndex) = 0;

	/***************************************************************
		函数名:     SetAudioStream
		函数描述:   设置音频流
		参数1：     int iIndex， 输入，要设置的音频流索引号
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetAudioStream(int iIndex) = 0;

	///***************************************************************
	//函数名:     SetAddMediaFilter
	//函数描述:   音视频调节设置接口
	//参数1：     bool isadd， 输入，是否添加音视频调节接口
	//返回值：    成功返回E_QVOD_S_OK，否则为失败
	//*****************************************************************/
	//virtual BOOL DynamicAddVideoPostFilter(bool bAddsub = true, bool bAddPostVideo = true) = 0;

public:		//媒体属性
	/**********************************************************
		函数名:     CanSeek
		函数描述:   当前媒体是否可拖拽
		返回值：    可拖拽返回TRUE，否则为FALSE
	************************************************************/
	virtual BOOL CanSeek() = 0;

	/**********************************************************
	函数名:     GetContainerType
	函数描述:   获取指定文件的文件类型
	参数1:      LPCWSTR pFileUrl 文件路径，为NULL则指当前播放的文件
	返回值：    指定文件的文件类型
	************************************************************/
	virtual e_CONTAINER GetContainerType(LPCWSTR pStrFileUrl = NULL) = 0;

	/***************************************************************
		函数名:     Detect3dVideo       若为3D，则会收到回调消息：QM_RQ_D3DVIDEO_DETECT                         
		函数描述:   检测d3d视频排列性
		返回值：    无	
	*****************************************************************/
	virtual void Detect3DVideo() = 0;

	/**********************************************************
		函数名:     GetDuration
		函数描述:   获取当前媒体文件的播放时长
		参数1:      long *lValue， 输出，媒体文件的播放时长,单位为毫秒
		返回值：    成功返回TRUE，否则为失败
	************************************************************/
	virtual BOOL GetDuration(REFERENCE_TIME *lValue) = 0;

	/***************************************************************
		函数名:     Move
		函数描述:   调整视频窗口
		参数1：     int x， 输入，x轴方向调整值
		参数2:      int y, 输入，y轴方向调整值
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL Move(int x, int y ) = 0;

	/***************************************************************
	函数名:     GetVideoFrameNum
	函数描述:  获取到达视频渲染的总帧数
	参数1:      int *pframenum，输入输出，视频渲染的总帧数
	返回值：  成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetVideoFrameNum(int *lframenum) = 0;

	/***************************************************************
	函数名:     RepaintVideo
	函数描述:  视频窗口重绘函数
	参数1：    CRect *prect， 输出，视频窗口位置指针
	返回值：  成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL RepaintVideo(CRect *prect = NULL) = 0;

	/***************************************************************
		函数名:     GetRate
		函数描述:  获取媒体播放的码率
		参数1:      long *pRate，输出，媒体播放的码率
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetRate(long *lRate) = 0;

	/***************************************************************
		函数名:     GetCurrentImage
		函数描述:  获取当前视频图像
		参数1:      PBYTE *ppbuf，输出，BITMAPINFOHEADER+位图数据
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetCurrentImage(PBYTE *ppbuf, e_CAPTUREMODE eCaptrueMode = _E_CM_CURRENT, REFERENCE_TIME* psampletime = NULL) = 0;

	/***************************************************************
		函数名:     GetBufferPer
		函数描述:  获取网络文件缓冲百分比
		参数1:      long *pBufferPer，输出，网络文件缓冲百分比（缓冲时间为10s）
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetBufferPer(long *lBufferPer) = 0;

		/***************************************************************
		函数名:     GetClipSize
		函数描述:  获取图像的宽高
		参数1:      long *lWidht，输出，视频宽
		参数2：     long *lHeight，输出，视频高
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetClipSize(long *lWidht, long *lHeight) = 0;

	/***************************************************************
		函数名:     GetAudioRenderCount
		函数描述:   获取AudioRender数
		参数1:      long *lAudioCount，输出，AudioRender数
		参数2：     long *lBufSize，输出，已使用的buffer大小
		参数3：     wchar_t *pAudioText，输入、输出，音频Render名称
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetAudioRenderCount(long *lAudioCount, long *lBufSize, wchar_t *pAudioText) = 0;


	/***************************************************************
		函数名:     SetSoundChannel
		函数描述:   设置（获取）音频声道模式
		参数1:      e_SoundChannel eSoundChannel，输入，音频声道模式
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetSoundChannel(e_SoundChannel eSoundChannel) = 0;
	virtual BOOL GetSoundChannel(e_SoundChannel *eSoundChannel) = 0;

	/***************************************************************
		函数名:     GetFileInfo
		函数描述:   获取媒体文件的媒体信息
		参数1:      wchar_t *pFileUrl，输入，文件路径
		参数2:      s_FileInfo	*ps_FileInfo，输出，文件信息结构体
		参数2:      E_MEDIA_TYPE* mt，输出，媒体类型
		返回值：   成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetFileInfo(const wchar_t *pFileUrl, struct s_FileInfo  *ps_FileInfo, E_MEDIA_TYPE* mt) = 0;

	/***************************************************************
		函数名:     GetMediaInfo
		函数描述:   获取媒体文件的媒体信息(使用mediainfo库)
		参数1:      wchar_t *pFileUrl，输入，文件路径
		参数2:      Qvod_MainMediaInfo	*ps_FileInfo，输出，文件信息结构体
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetMediaInfo(const wchar_t *pFileUrl, struct Qvod_DetailMediaInfo	*ps_MediaInfo) = 0;

	/***************************************************************
		函数名:     GetCurrentMediaInfo
		函数描述:   获取文件媒体类型，帧率及视频编码类型
		参数1：     DWORD dwFlag 输入，参见e_PlayMediaInfo，可｜操作
		参数2：     E_MEDIA_TYPE *mt，输出，dwFlag&MEDIA_FILE_TYPE为真，此参返回当前文件媒体类型，否则忽略此参
		参数3：     int *pavgFrameRate，输出，dwFlag&MEDIA_FRAME_RATE为真，此参返回当前文件媒体帧率，否则忽略此参
		参数4：     e_V_ENCODER *pEncoder，输出，dwFlag&MEDIA_VIDEOCODEC_TYPE为真，此参返回当前文件媒体视频编码类型，否则忽略此参
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetCurrentMediaInfo(DWORD dwFlag, E_MEDIA_TYPE *mt = NULL, int *pavgFrameRate = NULL, e_V_ENCODER *pEncoder = NULL) = 0;

	/***************************************************************
		函数名:     SetQvodNet 旧接口，主要给播放引擎调用
		函数描述:  设置QvodNet接口指针
		参数1:       IQvodNet *pIQvodNet，QvodNet接口指针
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetQvodNet(void **pIQvodNet) = 0;


		/***************************************************************
		函数名:     SetIYfNet 旧接口，给PlayCtrl模块用
		函数描述:  设置接口指针
		参数1:       IYfNet *pIYfNet，pIYfNet接口指针
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual void SetQvodNet(IYfNet *pIYfNet) = 0;

public:	//DVD 播放控制接口
	/***************************************************************
		函数名:     DvdSelectAtPosition
		函数描述:   获取DVD鼠标移动消息
		参数1：     POINT pt， 输入，鼠标的当前位置
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL DvdSelectAtPosition(const POINT *pt) = 0;

	/***************************************************************
		函数名:     DvdSetCurtitle
		函数描述:   设置DVD当前播放标题
		参数1：     ULONG uCurtitle， 输入，标题有效值为（1-99）
		参数2：     BOOL bEvent，输入，如果是系统标题改变消息，此标志为TRUE，否则为FALSE
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL DvdSetCurtitle(int uCurtitle, BOOL bEvent) = 0;

	/***************************************************************
		函数名:     DvdGetTitles
		函数描述:   获取DVD标题数和当前播放标题
		参数1：     ULONG *pTitleNum = NULL， 输出，标题数
		参数2：     ULONG *pCurTitle = NULL，输出，当前播放标题
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL	DvdGetTitles(ULONG *pTitleNum, int *pCurTitle) = 0;

	/***************************************************************
		函数名:     DvdSetCurChapter
		函数描述:   播放指定DVD章节
		参数1：     ULONG nCurchapters， 输入，标题，有效值为（1-999）
		参数2：     BOOL bEvent，输入，如果是系统标题改变消息，此标志为TRUE，否则为FALSE
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL DvdSetCurChapter(int nCurchapters, BOOL bEvent) = 0;

	/***************************************************************
		函数名:     DvdGetChapters
		函数描述:   获取DVD指定标题的章节数，及当前播放章节
		参数1：     ULONG uTitle， 输入，标题，有效值为（1-999）
		参数2：     ULONG *ulChapterNum，输出，章节数
		参数3：     ULONG *iCurChapter，输出，当前章节数
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL	DvdGetChapters(ULONG uTitle,  ULONG *ulChapterNum, int *iCurChapter) = 0;

	/***************************************************************
		函数名:     DvdSetAngle
		函数描述:   设置DVD视角
		参数1：     ULONG uCurangle， 输出，视角，有效值为（1-9）
		参数2：     BOOL bEvent，输入，如果是系统标题改变消息，此标志为TRUE，否则为FALSE
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL DvdSetAngle(int uCurangle, BOOL bEvent) = 0;

	/***************************************************************
		函数名:     DvdGetAngle
		函数描述:   获取DVD视角
		参数1：     ULONG *ulAngNum， 输出，视角，有效值为（1-9）
		参数2：     int *iCurAngle，输出，当前视角
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL DvdGetAngle(ULONG *ulAngNum, int *iCurAngle) = 0;

	/***************************************************************
		函数名:     DvdSetAudioSub
		函数描述:   设置DVD字幕、音频信息
		参数1：     DWORD dwFlag，参见e_DvdAudioSub,可进行｜操作设置（获取）不同数据
		参数2：     DWORD dwFlag，dwFlag&DVD_SUBTITLE为真，则设置当前音频流索引(有效值为0-7)，否则忽略此参
		参数3：     int iCurSubtitle，dwFlag&DVD_AUDIO为真，则设置当前字幕流索引(有效值为-1-31， 注：-1为关闭字幕，0-31为设置字幕流)，否则忽略此参
		参数4：     BOOL bEvent， 输入，如果是系统标题改变消息，此标志为TRUE，否则为FALSE
		返回值：    成功返回TRUE，否则为失败
	***************************************************************/
	virtual BOOL DvdSetAudioSub(DWORD dwFlag, int uCuraudio = 0, int iCurSubtitle = 0, BOOL bEvent = FALSE) = 0;

	/***************************************************************
		函数名:     DvdGetAudioSub
		函数描述:   获取DVD字幕、音频信息
		参数1：     DWORD dwFlag，参见e_DvdAudioSub,可进行｜操作设置（获取）不同数据
		参数2：     S_DVDINFO* pAudio，输出，音频信息结构体，dwFlag&DVD_SUBTITLE为真，此参有效，否则忽略此参
		参数3：     S_DVDINFO* pSubTitle，输出，字幕信息结构体，dwFlag&DVD_AUDIO为真，则参有效，否则忽略此参
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL DvdGetAudioSub(DWORD dwFlag, S_DVDINFO* pAudio = NULL, S_DVDINFO* pSubTitle = NULL) = 0;

	/***************************************************************
		函数名:     DvdPlayChapter
		函数描述:   播放上/下一章节
		参数1：     e_DvdPlayPreNext ePlayChapter，DVD_PLAY_PRE：上一章节，DVD_PLAY_NEXT：下一章节
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL DvdPlayChapter(e_DvdPlayPreNext ePlayChapter) = 0;

	/***************************************************************
		函数名:     DvdSetMenu
		函数描述:   设置播放菜单
		参数1：     DVD_MENU_ID eMenu， 输入，DVD_MENU_ID菜单
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL DvdSetMenu(DVD_MENU_ID eMenu) = 0;

public:	//视频旋转，绽放，翻转等
	/**********************************************************
		功能:   视频旋转，水平，垂直翻转
		参数1:     DWORD dwFlag，参见e_FlipReverse,可进行｜操作获取不同数据
		参数2:     int nAngle，dwFlag&VIDEO_FLIP为真，则设置旋转角度，否则忽略此参，默认0
		参数3:     DWORD dwFlag，dwFlag&VIDEO_HREVERSE为真，则设置水平翻转，否则忽略此参，默认FALSE
		参数4:     DWORD dwFlag，dwFlag&VIDEO_VREVERSE为真，则设置垂直翻转，否则忽略此参，默认FALSE
		返回值：    成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetFlipReverse(DWORD dwFlag, int nAngle = 0, BOOL bHorizontal = FALSE, BOOL bVertical = FALSE) = 0;
	virtual BOOL GetFlipReverse(DWORD dwFlag, int *pAngle = NULL, BOOL *pBHorizontal = NULL, BOOL *bVertical = NULL) = 0;

	/***************************************************************
	函数名:     SetVideoZoom
	函数描述:   视频放大接口
	参数1：     POINT pt， 输入，视频放大的位置
	参数2：     float zoomparam， 输入，视频放大倍数
	返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetVideoZoom(POINT pt, float zoomparam) = 0;

	/***************************************************************
	函数名:     SetZoomMove
	函数描述:   视频放大接口
	参数1：     int x，视频移动的X向量
	参数2：     int y，视频移动的Y向量
	返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetZoomMove(int x, int y) = 0;

	/***************************************************************
	函数名:     DestopPlay
	函数描述:   桌面播放
	参数1：     bool btype， 输入，是否桌面播放
	返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL DestopPlay(bool bDestopplay) = 0;

	/***************************************************************
	函数名:     GetLastDestopPlay
	函数描述:   获取上一次桌面播放
	参数1：     bool* bfalg， 输入，输出，桌面播放是否未结束
	返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL GetLastDestopPlay(bool* bflag) = 0;

	/***************************************************************
	函数名:     SetDestopRecover
	函数描述:   恢复桌面背景
	参数1：     bool btype， 输入，是否恢复桌面背景
	返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual BOOL SetDestopRecover(bool bflag) = 0;

	/***************************************************************
		函数名:     SetViewFrame
		函数描述:   设置（获取）视频框架显示模式
		参数1:      e_Viewframe eViewFrame，输入，视频显示框架模式
		参数2:      e_VFULLFRAME eFFrame，输入，全屏模式类型
		返回值：    成功返回TRUE，否则失败
	*****************************************************************/
	virtual BOOL SetViewFrame(e_Viewframe eViewframe, e_VFULLFRAME eFFrame) = 0;
	virtual BOOL GetViewFrame(e_Viewframe *pviewframe, e_VFULLFRAME *pFFrame) = 0;

public:	//画面色彩设置
	/**********************************************************
		功能:   设置（获取）亮度，对比度，饱和度，色度
		参数1:      DWORD dwFlag， 参见e_VideoColorSe,可进行｜操作获取不同数据
		参数2:      int nBrightness， dwFlag&VIDEO_BRIGHTNESS为真，则设置亮度，否则忽略此参
		参数3:      int nSaturation，dwFlag&VIDEO_SATURATION为真，则设置对比度，否则忽略此参
		参数4:      int nContrast， dwFlag&VIDEO_CONTRAST为真，则设置饱和度，否则忽略此参
		参数5:      int nHue， dwFlag&VIDEO_HUE为真，则设置色度，，否则忽略此参数
		返回值：    成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetVideoColor(DWORD dwFlag, int nBrightness = 0, int nSaturation = 0, int nContrast = 0, int nHue = 0) = 0;
	virtual BOOL GetVideoColor(DWORD dwFlag, int* pBrightness = NULL, int* pSaturation = NULL, int* pContrast = NULL, int* pHue = NULL) = 0;

	//视频增强
	/**********************************************************
		功能:   设置（获取）视频增强
		参数1:      bool enable， true：打开视频增强模式，false:关闭
		参数2:      OPTIONENHANCE effect，选项
		返回值：    成功返回TRUE，失败FALSE
	************************************************************/
	virtual BOOL SetVideoEnhance(bool enable,  OPTIONENHANCE effect = EnhanceEffect_None) = 0;
	virtual BOOL GetVideoEnhance(bool *enable) = 0;

public:	//声道设置
	/**********************************************************
		功能:   设置（获取）声道，声音延迟及当前声道数
		参数1:     DWORD dwFlag， 参见e_AudioSet，可进行｜操作设置(获取)不同数据
		参数2:     int channel， 参见Channel，dwFlag&AUDIO_CHANNEL为真，则设置声道，否则忽略此参
		参数3:     LONGLONG delaytime，dwFlag&AUDIO_DELAY为真，则设置(获取)声音延迟，否则忽略此
		参数4:     long lenlarge，取值：[0， 100]，dwFlag&AUDIO_ENLARGE为真，则设置(获取)声音放大值，否则忽略此参
		参数5:     float channelNum，dwFlag&AUDIO_CHANNEL_NUM为真，则设置(获取)当前声道数，否则忽略此参
		返回值：    成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetAudio(DWORD dwFlag, int channel = 0, LONGLONG delaytime = 0, long lenlarge = 0, float channelNum = 0.0f) = 0;
	virtual BOOL GetAudio(DWORD dwFlag, Channel *pChannel = NULL, LONGLONG *pDelaytime = NULL, long *pLenlarge = NULL, float *pchannelNum = NULL) = 0;

	//音效均衡设置
	/**********************************************************
		功能:   设置（获取）音效均衡是否启用，音效平衡及音效均衡设置
		参数1:     DWORD dwFlag， 参见e_Equalization，可进行｜操作设置(获取)不同数据
		参数2:     bool enable， true:开启，false:关闭，dwFlag&EQ_ENALBE为真，则设置是否启用音效均衡，否则忽略此参
		参数3:     LONG balance，取值：[0，100]，dwFlag&EQ_BALANCE为真，则设置(获取)音效平衡，否则忽略此参
		参数4:     int *pEqualization，dwFlag&EQ_EFFECT为真，则设置音效均衡，否则忽略此参，默认NULL
		返回值：    成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetEqualization(DWORD dwFlag, bool enable = true, LONG balance = 0, int *pEqualization = NULL) = 0;
	virtual BOOL GetEqualization(DWORD dwFlag, bool* pEnable = NULL, LONG* pBalance = NULL, int *pEqualization = NULL) = 0;

public:	//字幕设置
	/**********************************************************
		功能:   设置（获取）字幕文件路径
		参数1:      设置字体文件
		返回值：    成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetSubtitlePath(const wchar_t* lpszPath/*, int& SubIndex*/) = 0;
	virtual BOOL GetSubtitlePath(LPWSTR lpString, int nMaxCount = MAX_PATH) = 0;

	/**********************************************************
		功能:   显示（隐藏）字幕
		参数1:      bool，true：显示，false：隐藏
		返回值：    成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetSubtitleVisible(bool bVisable, MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual BOOL GetSubtitleVisible(bool *bVisable,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	/**********************************************************
		功能:   设置（获取）字幕位置
		参数1:      ESubPlace place，参见ESubPlace枚举
		参数2:      int cx，X方向长度
		参数3:      int cy，Y方向长度
		返回值：    成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetSubtitlePos(ESubPlace place, int cx, int cy,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual BOOL GetSubtitlePos(ESubPlace *place, int *cx, int *cy,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	/**********************************************************
		功能:      显示当前（剩余、系统）时间
		参数1:    SubTimeType subTimeType，参见SubTimeType枚举
		返回值： 成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetSubTimeType(SubTimeType subTimeType) = 0;
	virtual BOOL GetSubTimeType(SubTimeType *subTimeType) = 0;

	/**********************************************************
		功能:      设置（获取）当前（剩余、系统）时间字体
		参数1:    LOGFONT lf，设置的字体
		返回值：成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetSubTimeFont(LOGFONT lf) = 0;
	virtual BOOL GetSubTimeFont(LOGFONT *lf) = 0;

	/**********************************************************
		功能:   设置（获取）字幕字体，颜色，轮廓颜色及延迟
		参数1:      DWORD dwFlag， 参见e_Subtitle，可进行｜操作获取不同数据
		参数2:      LOGFONT *psf ， dwFlag&SUBTITLE_FONT为真，则设置字体，否则忽略此参，默认NULL
		参数3:     DWORD dwFontClr，dwFlag&SUBTITLE_CLR为真，则设置字体颜色，否则忽略此参，默认0
		参数4:      int nContrast， dwFlag&SUBTITLE_OUTLINE_CLR为真，则设置轮廓颜色，否则忽略此参，默认0
		参数5:      int nHue， dwFlag&SUBTITLE_DELAY为真，则设置字幕延迟，否则忽略此参，默认0
		参数6:     MULTIP_SUB msub， 当前获取的字幕类型，参见MULTIP_SUB，默认MAJOR_SUB
		返回值：  成功返回TRUE,失败FALSE
	************************************************************/
	virtual BOOL SetSubtitle(DWORD dwFlag, const LOGFONT *psf = NULL, DWORD dwFontClr = 0, DWORD dwOutlineClr = 0, int delaytime = 0, MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual BOOL GetSubtitle(DWORD dwFlag, LOGFONT *psf = NULL, DWORD *pFontClr = NULL, DWORD *pOutlineClr = NULL, int *pDelaytime = NULL, MULTIP_SUB msub = MAJOR_SUB) =0;

	/**********************************************************
		功能:		 清除字幕
		返回值：BOOL，TRUE：成功，FALSE：失败
	************************************************************/
	virtual BOOL ClearSubtitle() = 0;

public:	//3D变换

	/************************************************************************
	 功能：2D转3D效果设置
	 参数：
		参数1: DWORD dw3DSet		3D设置选项
		参数2: int nMode		dw3DSet&T3D_MODE为真，此设置当前格式，否则忽略此参
		参数3: int nAngle		dw3DSet&T3D_ANGLE为真，此参有效，值为e_2T3DMode，否则忽略此参
		参数4: float fParalPos	dw3DSet&T3D_PARALAXPOS为真，此参有效，否则忽略此参
		参数5: int eChannel		dw3DSet&T3D_CHANNEL为真，此参有效，值为e_2T3DViewChannel，否则忽略此参
		参数6: e_2T3DFormat eType  dw3DSet为T3D_Max，则对当前选择格式进行上以参数设置，否则以指定eType格式设置
										若dw3DSet包含T3D_FORMAT，则设置当前格式为eType
	返回值：无
	************************************************************************/
	virtual BOOL SetT3D(DWORD dwFlag, int nMode = 0, int nAngle=0, float fParalPos = 0.0f, int nChannel= 0, e_2T3DFormat eType=T3D_Max) = 0;
	virtual BOOL GetT3D(DWORD dwFlag, e_2T3DMode* pMode = NULL, e_2T3DViewAngle* pAngle = NULL, float *pfParalPos = NULL, e_2T3DViewChannel* pChannel = NULL, e_2T3DFormat* peType=NULL, e_2T3DFormat eType=T3D_Max) = 0;
	virtual BOOL GetT3D(T3DSet *pOutSet, e_2T3DFormat eType=T3D_Max) = 0;

public:	//视频墙(注意，视频墙的以下四个函数需要打开VIDEOWALL宏才能调用)
	/***************************************************************
	函数名:     ResetMonitor                                
	函数描述:   重置监视器
	返回值：    成功返回E_QVOD_S_OK，否则为失败
	*****************************************************************/
	virtual BOOL ResetMonitor() = 0;

	/***************************************************************
	函数名:     IsVideoWallAvailable                                
	函数描述:   视频墙是否可用
	返回值：    可用返回true，否则为不可用
	*****************************************************************/
	virtual BOOL IsVideoWallAvailable() = 0;

	/***************************************************************
	函数名:     SetVideoWallMode                                
	函数描述:   设置进入退出视频墙模式标识
	返回值：    true为进入，false为退出
	*****************************************************************/
	virtual BOOL SetVideoWallMode(bool flag) = 0;

	/***************************************************************
	函数名:     GetVideoWallMode                                
	函数描述:   获取进入退出视频墙模式标识
	返回值：    true为进入，false为退出
	*****************************************************************/
	virtual BOOL GetVideoWallMode(bool* flag) = 0;

public:	//截图
	/************************************************************************
	 功能：播放器截图（指定时间点），此函数会阻塞，线程内调用
	 参数1：PCutImg pCutIm，输入pCutImg->dwTime，输出pCutImg->pBitmap
	 参数2： int nCount ,指传入PCutImg的个数
	 返回值：BOOL，TRUE,截图成功，否则（部分截图）失败
	 ************************************************************************/
	virtual BOOL CaptureImg(PCutImg pCutImg, int nCount = 1, LPCTSTR lpszMediaFile = NULL,IQvodCallBack* pCallBack = NULL) = 0;

	/************************************************************************
	 功能：播放器截图（平均截取N张），此函数会阻塞，线程内调用
	 参数1：int nCuts，需要截取的图片数目，输入
	 参数2：PCutImg *pCutImg，返回截取到的图片数据，输出
	 参数3：int *pCounts，返回截取的图片个数，输出
	 参数4：LPCTSTR lpszMediaFile，指定媒体文件截取，输入，默认为NULL，当前播放的文件
	 返回值：BOOL，TRUE,截图成功，否则（部分截图）失败
	 ************************************************************************/
	virtual BOOL CaptureImg(int nCutNum, PCutImg *pOutCutImg, int *pOutCutNum = NULL, LPCTSTR lpszMediaFile = NULL, IQvodCallBack* pCallBack = NULL) = 0;

	virtual void RleaseCutImgs() = 0;
public:
	/************************************************************************
		功能：设置播放模式
		参数1：BOOL bAuto，是否自动设置播放模式
		参数2：BOOL bSetEnhance，bAuto为TRUE时，bSetEnhance将被忽略
		*       当手动设置视频增强时，bAuto为FALSE，bSetEnhance为TRUE
		*       当手动关闭视频增强时，bAuto为FALSE，bSetEnhance为FALSE
		参数3：int nVideoSet,  新含义：0.智能模式；1.性能优先；2.画质优先
		参数4：BOOL bRdsp，是否为精简版，默认否
		返回值：BOOL，TRUE：成功，FALS，失败
	************************************************************************/
	virtual UINT SetPlayMode(BOOL bAuto, BOOL bSetEnhance, int nVideoSet = 0, BOOL bRdsp = FALSE) = 0;

	/***************************************************************
		函数名:     SetVideoStronger
		函数描述:  设置视频增强属性
		参数1:      BOOL bSet，是否为视频增强，TRUE，是，FALSE，否
		返回值：   无
	*****************************************************************/
	virtual void SetVideoStronger(BOOL bSet) = 0;
	virtual BOOL GetVideoStronger() = 0;

	/***************************************************************
		函数名:     SetInsertVideoStrong
		函数描述:  插件的画质增强,
		参数1:      int nType，-1：无效，1：画质增强，0：没有画质增强
		返回值：   无
	*****************************************************************/

	/***************************************************************
		函数名:     GetBuffering
		函数描述:  视频是否正在缓冲,
		返回值：   无
	*****************************************************************/
	virtual BOOL GetBuffering() = 0;

	/***************************************************************
		函数名:     GetNearestFrameOnDirection
		函数描述:  定位关键帧
		参数1：	    REFERENCE_TIME *newTime，输入输出，返回TRUE时获取附近关键帧
		参数2：	    int nDirection，关键帧定位方向,(-1:靠前取关键帧 0:默认 1:靠后取关键帧)
		返回值：   BOOL，TRUE，成功，FALSE：获取关键帧失败
	*****************************************************************/
	virtual BOOL GetKeyPos(REFERENCE_TIME *newTime, int nDirection = 0) = 0;

	/***************************************************************
		函数名:     MapNetBitNs
		函数描述:  更新网络位信息(暂时只给插件调用)
		参数1：	    const char* szplayHash，当前网络任务的hash
		返回值：   NET_ERROR类型
	*****************************************************************/
	virtual NET_ERROR UpdateBitInfo(const char* szplayHash) = 0;
	/***************************************************************
		函数名:     ClearMapBits
		函数描述:  清除网络位设置信息等
		返回值：   无
	*****************************************************************/
	virtual void ClearMapBits() = 0;
	virtual void ReSetLoadInfo(BOOL bClearLoadInfo = TRUE) = 0;														// 重新设置下载进度
	
	/***************************************************************

	/***************************************************************
		函数名:     IsDownloadPos
		函数描述:  检测fPos位置的片数是否已经下载到
		参数1：		float fPos,
		返回值：   BOOL是否已下载，TRUE：已下载，FALSE：未被下载
		返回值：   无
	*****************************************************************/
	virtual BOOL IsDownloadPos(float fPos) = 0;									

	/***************************************************************
		函数名:     Seek
		函数描述:  文件定位播放
		参数1：		REFERENCE_TIME seekPos，定位播放位置
		参数2：		BOOL bKeyFrame，是否定位到关键帧，TRUE：定位，FALSE：不定位，默认为FALSE
		参数3：    BOOL bSync，是否同步，TRUE：是否同步（阻塞），FALSE：异步，默认为FALSE
		返回值：   BOOL, TRUE,成功，FALSE,失败
	*****************************************************************/
	virtual BOOL Seek(REFERENCE_TIME seekPos, BOOL bKeyFrame = FALSE, BOOL bSync = FALSE) = 0;


	/***************************************************************
		函数名:     IsVideoType
		函数描述:  媒体是否为视频，即有图像和尺寸
		返回值：   BOOL，TRUE：是视频，否则不是
	*****************************************************************/
	virtual BOOL IsVideoType() = 0;

	/***************************************************************
	函数名:     GetCrashLog                               
	函数描述:   获取崩溃时播放引擎模块的运行日志
	参数1：     BYTE** ppbyte, 输出，Log buffer指针
	参数2：     int& len,	   输出，Logbuffer大小
	返回值：    无
	*****************************************************************/
	virtual void GetCrashLog(BYTE** ppbyte, int& len) = 0;

	virtual BOOL SetVideoRender(e_VideoRender eVideoRender) = 0;
};



/***************************************************************
函数名:     InitPlayCtrl UnInitPlayCtrl
函数描述:  初始化和反初始化模块
返回值：    无
*****************************************************************/
PLAYCRTL_API BOOL WINAPI InitPlayCtrl();
PLAYCRTL_API void WINAPI UnInitPlayCtrl();

/***************************************************************
函数名:     CreatePlayCtrl
函数描述:   创建一个播放器功能设置
参数1：     BOOL bDefaultMediaPlay，为TRUE，设为当前默认的播放器功能
返回值：    成功返回创建成功的播放器关联id，失败返回-1，
*****************************************************************/
PLAYCRTL_API int WINAPI CreatePlayCtrl(bool bDefaultMediaPlay = false, bool bCreateByInsert = false);

/***************************************************************
函数名:     SetCurPlayCtrlId
函数描述:   设置（获取）当前播放器id
参数1：     long lMediaID，关联的播放器ID，设置当前的播放器
返回值：    成功返回PlayCtrl*，失败返回NULL
*****************************************************************/
PLAYCRTL_API BOOL WINAPI SetCurPlayCtrlId(long lcurMediaID);		
PLAYCRTL_API long WINAPI GetCurPlayCtrlId();

/***************************************************************
函数名:     GetPlayCtrl
函数描述:   获取某个播放器功能设置
参数1：     long lMediaID，参数为-1，则获取当前设置的播放器
返回值：    成功返回PlayCtrl*，失败返回NULL
*****************************************************************/
PLAYCRTL_API PlayCtrl* WINAPI GetPlayCtrl(long lMediaID = -1);		

/***************************************************************
函数名:     ReleasePlayCtrl
函数描述:   释放关闭创建的播放器设置
参数1：     long lMediaID，参数为-1，则释放所有创建的播放器功能设置，否则只释放指定关联id的播放器
参数2：     BOOL bResMediaModule，参数为TRUE，则释放 加载的QvodPlayMedia 模块句柄，否则不释放
返回值：    无
*****************************************************************/
PLAYCRTL_API void WINAPI ReleasePlayCtrl(long lMediaID = -1);
