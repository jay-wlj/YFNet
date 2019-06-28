#pragma once
#ifdef PLAYCTRL_EXPORTS
#define PLAYCRTL_API extern "C" __declspec(dllexport)
#else
#define PLAYCRTL_API extern "C" __declspec(dllimport)
#endif


enum e_PlayControl
{
	PLAYCTL_PLAY,			//����
	PLAYCTL_PAUSE,			//��ͣ
	PLAYCTL_STOP,			//ֹͣ����
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
	T3D_NO3D,					//�ޱ仯
	T3D_2T3D,					//2Dת3D
	T3D_3DPLAY,				//3D����
	T3D_3DT2D,					//3Dת2D

	T3D_Max,
};
enum e_2T3DMode
{
	T3D_REDBLUE,				//����
	T3D_REDGREEN,			//����
	T3D_BROWNBLUE,		//����

	Mode_Max
};

enum e_2T3DViewAngle
{
	T3D_LEFTRIGHT,				//����
	T3D_RIGHTLEFT,				//����
	T3D_UPDOWN,				//����
	T3D_DOWNUP,				//����

	Angle_Max
};

enum e_2T3DViewChannel
{
	T3D_LEFT,				//��
	T3D_RIGHT,			//��
	T3D_UP,				//��	
	T3D_DOWN,			//��

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

//ɫ��
enum e_VideoColorSet
{
	VIDEO_BRIGHTNESS = 1,		//����   (-127--127)
	VIDEO_SATURATION = 2,		//���Ͷ� (-127--127)
	VIDEO_CONTRAST = 4,			//�Աȶ� (-127--127)
	VIDEO_HUE = 8,				//ɫ�� (0--616)
};

enum e_FlipReverse
{
	VIDEO_FLIP = 1,					//��ת
	VIDEO_HREVERSE = 2,		//ˮƽ��ת
	VIDEO_VREVERSE = 4,		//��ֱ��ת
};

//��Ļ
enum e_Subtitle
{
	SUBTITLE_FONT = 1,				//����
	SUBTITLE_CLR = 2,					//������ɫ
	SUBTITLE_OUTLINE_CLR = 4,	//����������ɫ
	SUBTITLE_DELAY = 8,				//��Ļ�ӳ�(��ǰ)
};

//��Ƶ����
enum e_AudioSet
{
	AUDIO_CHANNEL = 1,				//��Ƶ����(���ҡ��������������)
	AUDIO_CHANNEL_NUM = 2,		//��ǰ������
	AUDIO_DELAY = 4,						//�����ӳ�
	AUDIO_ENLARGE = 8,					//�����Ŵ�
};

//��Ч����
enum e_Equalization
{
	EQ_ENALBE = 1,	   //�Ƿ�����Ч����
	EQ_EFFECT = 2,	   //��Ч��������
	EQ_BALANCE = 4, //��Чƽ��
};

enum e_DvdAudioSub
{
	DVD_AUDIO = 1,			//��Ƶ��Ϣ
	DVD_SUBTITLE = 2,		//��Ļ��Ϣ
};

enum e_DvdPlayPreNext
{
	DVD_PLAY_PRE,				//��ǰ
	DVD_PLAY_NEXT,				//���
};

enum e_PlayMediaInfo
{
	MEDIA_FILE_TYPE = 1,				//�ļ�ý������
	MEDIA_FRAME_RATE = 2,				//�ļ�ý��֡��
	MEDIA_VIDEOCODEC_TYPE = 4,			//�ļ�ý����Ƶ��������
};

//��ͼ����
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

//bit��Ϣ���ݣ�ͨ��QCM_PLAYCTRL_SETLOADINFO�ص���UI����
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

// ý���ļ�����
enum e_MediaType
{
	mt_unknow = 0,	// δ֪����
	mt_movie,		// ����Ƶ
	mt_music,		// ����Ƶ
	mt_image,		// ͼƬ
	mt_CD,			// CD��Ƭ
	mt_DVD,		// DVD���
	mt_flash,		// flash
	mt_other		// ����(����)
};

// ý���ļ���Դ
enum e_MediaSource
{
	ms_unknow = 0,	// δ֪����
	ms_p2p,			// P2P��Դ
	ms_live,		// ֱ��
	ms_stream,		// ������
	ms_hdisk,		// ����Ӳ��
	ms_cdrom,		// ���ع���
	ms_LAN,			// ������
	ms_other		// ����(����)
};


interface PlayCtrl :public IQvodCallBack
{
public:
	/*********************************play******************************
		������:     SetParentWnd
		��������:   ������Ƶ���Ŵ��ڵĸ����ھ��
		����1:      HWND hParentWnd�����룬�����ھ��
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetParentWnd(HWND hParentWnd) = 0;

	/***************************************************************
		������:     SetCallBack
		��������:   ���ûص��Ķ���
		����1:		 IQvodCallBack *pQvodCallBack�����룬�ص�����
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetCallBack(IQvodCallBack *pQvodCallBack) = 0;
	
	/***************************************************************
		������:     QueryInterface
		��������:   ��ѯ�ӿ�
		����1��     REFIID riid�� ���룬�ӿ�IID���ɹ���ѯ����IID_IColortrans,IID_ISoundtrans,IID_ISubtrans�����ӿ�
		����2:      void **ppvIF, ������ӿ�ָ��
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL __stdcall QueryInterface(REFIID riid, void **ppvIF) = 0;

public:
	//���ſ��ƽӿ�
	/***********************************************************************
		���ܣ�����
		����1�� lpszURL,ý���ļ����ء�����·��
		����2�� E_MEDIA_TYPE mt = E_MT_VIDEO������ý�岥�����ͣ�Ĭ��Ϊ����Ƶ
		����3�� rBeginPos,��ָ��λ�ÿ�ʼ����
		����4�� e_MediaSource ms�� ý���ļ���Դ��Ĭ��δ֪
		����5�� e_MediaType m��// ý���ļ����ͣ�Ĭ��δ֪
		����ֵ���ɹ�����E_QVOD_S_OK,����Ϊʧ��
	************************************************************************/
	virtual QVOD_ERROR PlaySource(LPCTSTR lpszURL, E_MEDIA_TYPE mt = E_MT_VIDEO, REFERENCE_TIME rBeginPos = 0, e_MediaSource ms = ms_unknow,  e_MediaType m=mt_unknow) = 0;

	/***********************************************************************
		���ܣ�����
		����ֵ��TRUE,�ɹ���FALS,ʧ��
	************************************************************************/
	virtual BOOL Play() = 0;

	/***********************************************************************
		���ܣ���ͣ����
		��������
		����ֵ��TRUE,�ɹ���FALS,ʧ��
	************************************************************************/
	virtual BOOL Pause() = 0;

	/***********************************************************************
		���ܣ�������ͣ������ǰΪ����״̬������ú���ͣ״̬����֮һ��
		��������
		����ֵ��TRUE,�ɹ���FALS,ʧ��
	************************************************************************/
	virtual BOOL PlayPause() = 0;

	/***********************************************************************
		���ܣ�ֹͣ����
		��������
		����ֵ��TRUE,�ɹ���FALS,ʧ��
	************************************************************************/
	virtual BOOL Stop() = 0;

	/************************************************************************
		���ܣ���õ�ǰ����״̬
		������e_PlayState *playState����õ�ǰ����״̬
		����ֵ��BOOL��TRUE���ɹ���FALS��ʧ��
	************************************************************************/
	virtual BOOL GetPlayState(e_PlayState *playState) = 0;

	/**********************************************************
		����:		����(��ȡ)��ǰý�岥�ŵ�ʱ���
		����1:    long  lValue�� ���룬Ҫ���õ���ý���ļ�����ʱ���,��λΪ����
		����ֵ���ɹ�����TRU,ʧ��FALSE
	************************************************************/
	/*************************************************************
		������:     GetCurrentPos  
		��������:   ��ȡ��ǰ����λ��(�Ͳ���ʱ��)
		����1:        REFERENCE_TIME *lValue, ��������ص�ǰ����λ��
		����2:        REFERENCE_TIME *lDuraton, �������ΪNULLʱ���ص�ǰ����ʱ��
		����ֵ��    �ɹ�����TRUE������ʧ��
	***************************************************************/
	virtual BOOL GetCurrentPos(REFERENCE_TIME *lValue, REFERENCE_TIME *lDuraton) = 0;
	virtual BOOL GetCurrentPos(REFERENCE_TIME *lValue) = 0;

	/*************************************************************
		������:     SetMute GetMute 
		��������:   ����(��ȡ)�������ľ�����ʶ
		����1:      bool bValue�����룬����Ϊtrue������Ϊfalse
		����ֵ��    �ɹ�����TRUE������ʧ��
	***************************************************************/
	virtual BOOL SetMute(bool bValue) = 0;
	virtual BOOL GetMute(bool *bValue) = 0;

	/***************************************************************
	������:     Restore
	��������:   ��Ƶ���ڻ�ԭΪ��ʼ״̬
	����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL Restore() = 0;

	/***************************************************************
	������		AllocateColokey
	��������:	����ɫ��
	����1��		DWORD dwColorkey�� ���룬���ò��Ŵ��ڵ�ɫ��
	����ֵ��	��
	*****************************************************************/
	virtual void AllocateColokey(DWORD dwColorkey) = 0;

	virtual BOOL IsAudioEnable() = 0;			//�Ƿ�֧����Ƶ���ڣ����������������Ŵ����õȣ�
	
	/***************************************************************
	������:     SetBufferingTime                                
	��������:   ���ò��Ż������ݳ���
	����1��     int nPreBufferTime, ����ǰԤ�������ݵĲ���ʱ������λ���룬��󳤶�Ϊ20s��
	����1��     int nPlayBufferTime, �����л������ݵĲ���ʱ������λ���룬��󳤶�Ϊ20s��
	����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetBufferingTime(int nPreBufferTime, int nPlayBufferTime) = 0;

	/***************************************************************
	������:     GetBufferingTime                                
	��������:   ��ȡ���Ż������ݳ���
	����1��     int nPreBufferTime, ����ǰԤ�������ݵĲ���ʱ������λ���룬��󳤶�Ϊ20s��
	����1��     int nPlayBufferTime, �����л������ݵĲ���ʱ������λ���룬��󳤶�Ϊ20s��
	����ֵ��    ��
	*****************************************************************/
	virtual void GetBufferingTime(int* pnPreBufferTime, int* pnPlayBufferTime) = 0;

public:	//ý����Ϣ����
	/***************************************************************
		������:     GetVolume
		��������:  ���ã���ȡ��������������
		����1:      long lValue�� ���룬0-100����������
		����ֵ��   �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetVolume(long lValue) = 0;
	virtual BOOL GetVolume(long *lValue) = 0;

	/***************************************************************
		������:     SetPlaySpeed GetPlaySpeed
		��������:   ���ã���ȡ���������Ĳ�������
		����1:      long lSpeed�����룬1������0.1������-1������0.1������ȡֵ��ΧΪ0.11-3.0������1�����ڼ��ٲ��ţ�С��1�����ڼ��ٲ��ţ�����1��������
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetPlaySpeed(double lSpeed) = 0;
	virtual BOOL GetPlaySpeed(double *lSpeed) = 0;

	/***************************************************************
		������:     SetPlayFrame
		��������:   ���ò���֡��
		����1��     long lFrameNums�� ���룬ȡֵ1��-1,1ǰ��һ֡��-1����һ֡
		����1��     bool bsync�� ���룬�Ƿ�ͬ��
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetPlayFrame(long lFrameNums, bool bsync = false) = 0;

	/***************************************************************
		������:     SetPlayOption
		��������:   ���ã���ȡ������ѡ���	��Ӳ�����١�������Ƶ���ڡ�������Ӳ�����ٺ���Ƶ���ڣ�
		����1:      e_Play_Option eOption�����룬����Ӳ�����١�������Ƶ����������Ӳ�����ٺ���Ƶ����
		����ֵ��    ��
	*****************************************************************/
	virtual void SetPlayOption(e_Play_Option eOption, PlayOption_Param* pParam = NULL) = 0;
	virtual void GetPlayOption(e_Play_Option *eOption, PlayOption_Param* pParam = NULL) = 0;

	/***************************************************************
		������:     GetCurAudioRender
		��������:   ���ã���ȡ����ǰʹ�õ�Audiorender
		����1:      long lAudiooRender�����룬Audiorender��������������õ��ģ�
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetCurAudioRender(long lAudiooRender) = 0;
	virtual BOOL GetCurAudioRender(long *lAudioRender) = 0;

	/***************************************************************
		������:     SetSubTitleStream
		��������:   ������Ļ��
		����1��     int iIndex�� ���룬Ҫ���õ���Ļ��������
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetSubTitleStream(int iIndex) = 0;

	/***************************************************************
		������:     SetAudioStream
		��������:   ������Ƶ��
		����1��     int iIndex�� ���룬Ҫ���õ���Ƶ��������
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetAudioStream(int iIndex) = 0;

	///***************************************************************
	//������:     SetAddMediaFilter
	//��������:   ����Ƶ�������ýӿ�
	//����1��     bool isadd�� ���룬�Ƿ��������Ƶ���ڽӿ�
	//����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
	//*****************************************************************/
	//virtual BOOL DynamicAddVideoPostFilter(bool bAddsub = true, bool bAddPostVideo = true) = 0;

public:		//ý������
	/**********************************************************
		������:     CanSeek
		��������:   ��ǰý���Ƿ����ק
		����ֵ��    ����ק����TRUE������ΪFALSE
	************************************************************/
	virtual BOOL CanSeek() = 0;

	/**********************************************************
	������:     GetContainerType
	��������:   ��ȡָ���ļ����ļ�����
	����1:      LPCWSTR pFileUrl �ļ�·����ΪNULL��ָ��ǰ���ŵ��ļ�
	����ֵ��    ָ���ļ����ļ�����
	************************************************************/
	virtual e_CONTAINER GetContainerType(LPCWSTR pStrFileUrl = NULL) = 0;

	/***************************************************************
		������:     Detect3dVideo       ��Ϊ3D������յ��ص���Ϣ��QM_RQ_D3DVIDEO_DETECT                         
		��������:   ���d3d��Ƶ������
		����ֵ��    ��	
	*****************************************************************/
	virtual void Detect3DVideo() = 0;

	/**********************************************************
		������:     GetDuration
		��������:   ��ȡ��ǰý���ļ��Ĳ���ʱ��
		����1:      long *lValue�� �����ý���ļ��Ĳ���ʱ��,��λΪ����
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	************************************************************/
	virtual BOOL GetDuration(REFERENCE_TIME *lValue) = 0;

	/***************************************************************
		������:     Move
		��������:   ������Ƶ����
		����1��     int x�� ���룬x�᷽�����ֵ
		����2:      int y, ���룬y�᷽�����ֵ
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL Move(int x, int y ) = 0;

	/***************************************************************
	������:     GetVideoFrameNum
	��������:  ��ȡ������Ƶ��Ⱦ����֡��
	����1:      int *pframenum�������������Ƶ��Ⱦ����֡��
	����ֵ��  �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetVideoFrameNum(int *lframenum) = 0;

	/***************************************************************
	������:     RepaintVideo
	��������:  ��Ƶ�����ػ溯��
	����1��    CRect *prect�� �������Ƶ����λ��ָ��
	����ֵ��  �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL RepaintVideo(CRect *prect = NULL) = 0;

	/***************************************************************
		������:     GetRate
		��������:  ��ȡý�岥�ŵ�����
		����1:      long *pRate�������ý�岥�ŵ�����
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetRate(long *lRate) = 0;

	/***************************************************************
		������:     GetCurrentImage
		��������:  ��ȡ��ǰ��Ƶͼ��
		����1:      PBYTE *ppbuf�������BITMAPINFOHEADER+λͼ����
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetCurrentImage(PBYTE *ppbuf, e_CAPTUREMODE eCaptrueMode = _E_CM_CURRENT, REFERENCE_TIME* psampletime = NULL) = 0;

	/***************************************************************
		������:     GetBufferPer
		��������:  ��ȡ�����ļ�����ٷֱ�
		����1:      long *pBufferPer������������ļ�����ٷֱȣ�����ʱ��Ϊ10s��
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetBufferPer(long *lBufferPer) = 0;

		/***************************************************************
		������:     GetClipSize
		��������:  ��ȡͼ��Ŀ��
		����1:      long *lWidht���������Ƶ��
		����2��     long *lHeight���������Ƶ��
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetClipSize(long *lWidht, long *lHeight) = 0;

	/***************************************************************
		������:     GetAudioRenderCount
		��������:   ��ȡAudioRender��
		����1:      long *lAudioCount�������AudioRender��
		����2��     long *lBufSize���������ʹ�õ�buffer��С
		����3��     wchar_t *pAudioText�����롢�������ƵRender����
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetAudioRenderCount(long *lAudioCount, long *lBufSize, wchar_t *pAudioText) = 0;


	/***************************************************************
		������:     SetSoundChannel
		��������:   ���ã���ȡ����Ƶ����ģʽ
		����1:      e_SoundChannel eSoundChannel�����룬��Ƶ����ģʽ
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetSoundChannel(e_SoundChannel eSoundChannel) = 0;
	virtual BOOL GetSoundChannel(e_SoundChannel *eSoundChannel) = 0;

	/***************************************************************
		������:     GetFileInfo
		��������:   ��ȡý���ļ���ý����Ϣ
		����1:      wchar_t *pFileUrl�����룬�ļ�·��
		����2:      s_FileInfo	*ps_FileInfo��������ļ���Ϣ�ṹ��
		����2:      E_MEDIA_TYPE* mt�������ý������
		����ֵ��   �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetFileInfo(const wchar_t *pFileUrl, struct s_FileInfo  *ps_FileInfo, E_MEDIA_TYPE* mt) = 0;

	/***************************************************************
		������:     GetMediaInfo
		��������:   ��ȡý���ļ���ý����Ϣ(ʹ��mediainfo��)
		����1:      wchar_t *pFileUrl�����룬�ļ�·��
		����2:      Qvod_MainMediaInfo	*ps_FileInfo��������ļ���Ϣ�ṹ��
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetMediaInfo(const wchar_t *pFileUrl, struct Qvod_DetailMediaInfo	*ps_MediaInfo) = 0;

	/***************************************************************
		������:     GetCurrentMediaInfo
		��������:   ��ȡ�ļ�ý�����ͣ�֡�ʼ���Ƶ��������
		����1��     DWORD dwFlag ���룬�μ�e_PlayMediaInfo���ɣ�����
		����2��     E_MEDIA_TYPE *mt�������dwFlag&MEDIA_FILE_TYPEΪ�棬�˲η��ص�ǰ�ļ�ý�����ͣ�������Դ˲�
		����3��     int *pavgFrameRate�������dwFlag&MEDIA_FRAME_RATEΪ�棬�˲η��ص�ǰ�ļ�ý��֡�ʣ�������Դ˲�
		����4��     e_V_ENCODER *pEncoder�������dwFlag&MEDIA_VIDEOCODEC_TYPEΪ�棬�˲η��ص�ǰ�ļ�ý����Ƶ�������ͣ�������Դ˲�
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetCurrentMediaInfo(DWORD dwFlag, E_MEDIA_TYPE *mt = NULL, int *pavgFrameRate = NULL, e_V_ENCODER *pEncoder = NULL) = 0;

	/***************************************************************
		������:     SetQvodNet �ɽӿڣ���Ҫ�������������
		��������:  ����QvodNet�ӿ�ָ��
		����1:       IQvodNet *pIQvodNet��QvodNet�ӿ�ָ��
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetQvodNet(void **pIQvodNet) = 0;


		/***************************************************************
		������:     SetIYfNet �ɽӿڣ���PlayCtrlģ����
		��������:  ���ýӿ�ָ��
		����1:       IYfNet *pIYfNet��pIYfNet�ӿ�ָ��
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual void SetQvodNet(IYfNet *pIYfNet) = 0;

public:	//DVD ���ſ��ƽӿ�
	/***************************************************************
		������:     DvdSelectAtPosition
		��������:   ��ȡDVD����ƶ���Ϣ
		����1��     POINT pt�� ���룬���ĵ�ǰλ��
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL DvdSelectAtPosition(const POINT *pt) = 0;

	/***************************************************************
		������:     DvdSetCurtitle
		��������:   ����DVD��ǰ���ű���
		����1��     ULONG uCurtitle�� ���룬������ЧֵΪ��1-99��
		����2��     BOOL bEvent�����룬�����ϵͳ����ı���Ϣ���˱�־ΪTRUE������ΪFALSE
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL DvdSetCurtitle(int uCurtitle, BOOL bEvent) = 0;

	/***************************************************************
		������:     DvdGetTitles
		��������:   ��ȡDVD�������͵�ǰ���ű���
		����1��     ULONG *pTitleNum = NULL�� �����������
		����2��     ULONG *pCurTitle = NULL���������ǰ���ű���
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL	DvdGetTitles(ULONG *pTitleNum, int *pCurTitle) = 0;

	/***************************************************************
		������:     DvdSetCurChapter
		��������:   ����ָ��DVD�½�
		����1��     ULONG nCurchapters�� ���룬���⣬��ЧֵΪ��1-999��
		����2��     BOOL bEvent�����룬�����ϵͳ����ı���Ϣ���˱�־ΪTRUE������ΪFALSE
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL DvdSetCurChapter(int nCurchapters, BOOL bEvent) = 0;

	/***************************************************************
		������:     DvdGetChapters
		��������:   ��ȡDVDָ��������½���������ǰ�����½�
		����1��     ULONG uTitle�� ���룬���⣬��ЧֵΪ��1-999��
		����2��     ULONG *ulChapterNum��������½���
		����3��     ULONG *iCurChapter���������ǰ�½���
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL	DvdGetChapters(ULONG uTitle,  ULONG *ulChapterNum, int *iCurChapter) = 0;

	/***************************************************************
		������:     DvdSetAngle
		��������:   ����DVD�ӽ�
		����1��     ULONG uCurangle�� ������ӽǣ���ЧֵΪ��1-9��
		����2��     BOOL bEvent�����룬�����ϵͳ����ı���Ϣ���˱�־ΪTRUE������ΪFALSE
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL DvdSetAngle(int uCurangle, BOOL bEvent) = 0;

	/***************************************************************
		������:     DvdGetAngle
		��������:   ��ȡDVD�ӽ�
		����1��     ULONG *ulAngNum�� ������ӽǣ���ЧֵΪ��1-9��
		����2��     int *iCurAngle���������ǰ�ӽ�
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL DvdGetAngle(ULONG *ulAngNum, int *iCurAngle) = 0;

	/***************************************************************
		������:     DvdSetAudioSub
		��������:   ����DVD��Ļ����Ƶ��Ϣ
		����1��     DWORD dwFlag���μ�e_DvdAudioSub,�ɽ��У��������ã���ȡ����ͬ����
		����2��     DWORD dwFlag��dwFlag&DVD_SUBTITLEΪ�棬�����õ�ǰ��Ƶ������(��ЧֵΪ0-7)��������Դ˲�
		����3��     int iCurSubtitle��dwFlag&DVD_AUDIOΪ�棬�����õ�ǰ��Ļ������(��ЧֵΪ-1-31�� ע��-1Ϊ�ر���Ļ��0-31Ϊ������Ļ��)��������Դ˲�
		����4��     BOOL bEvent�� ���룬�����ϵͳ����ı���Ϣ���˱�־ΪTRUE������ΪFALSE
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	***************************************************************/
	virtual BOOL DvdSetAudioSub(DWORD dwFlag, int uCuraudio = 0, int iCurSubtitle = 0, BOOL bEvent = FALSE) = 0;

	/***************************************************************
		������:     DvdGetAudioSub
		��������:   ��ȡDVD��Ļ����Ƶ��Ϣ
		����1��     DWORD dwFlag���μ�e_DvdAudioSub,�ɽ��У��������ã���ȡ����ͬ����
		����2��     S_DVDINFO* pAudio���������Ƶ��Ϣ�ṹ�壬dwFlag&DVD_SUBTITLEΪ�棬�˲���Ч��������Դ˲�
		����3��     S_DVDINFO* pSubTitle���������Ļ��Ϣ�ṹ�壬dwFlag&DVD_AUDIOΪ�棬�����Ч��������Դ˲�
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL DvdGetAudioSub(DWORD dwFlag, S_DVDINFO* pAudio = NULL, S_DVDINFO* pSubTitle = NULL) = 0;

	/***************************************************************
		������:     DvdPlayChapter
		��������:   ������/��һ�½�
		����1��     e_DvdPlayPreNext ePlayChapter��DVD_PLAY_PRE����һ�½ڣ�DVD_PLAY_NEXT����һ�½�
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL DvdPlayChapter(e_DvdPlayPreNext ePlayChapter) = 0;

	/***************************************************************
		������:     DvdSetMenu
		��������:   ���ò��Ų˵�
		����1��     DVD_MENU_ID eMenu�� ���룬DVD_MENU_ID�˵�
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL DvdSetMenu(DVD_MENU_ID eMenu) = 0;

public:	//��Ƶ��ת�����ţ���ת��
	/**********************************************************
		����:   ��Ƶ��ת��ˮƽ����ֱ��ת
		����1:     DWORD dwFlag���μ�e_FlipReverse,�ɽ��У�������ȡ��ͬ����
		����2:     int nAngle��dwFlag&VIDEO_FLIPΪ�棬��������ת�Ƕȣ�������Դ˲Σ�Ĭ��0
		����3:     DWORD dwFlag��dwFlag&VIDEO_HREVERSEΪ�棬������ˮƽ��ת��������Դ˲Σ�Ĭ��FALSE
		����4:     DWORD dwFlag��dwFlag&VIDEO_VREVERSEΪ�棬�����ô�ֱ��ת��������Դ˲Σ�Ĭ��FALSE
		����ֵ��    �ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetFlipReverse(DWORD dwFlag, int nAngle = 0, BOOL bHorizontal = FALSE, BOOL bVertical = FALSE) = 0;
	virtual BOOL GetFlipReverse(DWORD dwFlag, int *pAngle = NULL, BOOL *pBHorizontal = NULL, BOOL *bVertical = NULL) = 0;

	/***************************************************************
	������:     SetVideoZoom
	��������:   ��Ƶ�Ŵ�ӿ�
	����1��     POINT pt�� ���룬��Ƶ�Ŵ��λ��
	����2��     float zoomparam�� ���룬��Ƶ�Ŵ���
	����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetVideoZoom(POINT pt, float zoomparam) = 0;

	/***************************************************************
	������:     SetZoomMove
	��������:   ��Ƶ�Ŵ�ӿ�
	����1��     int x����Ƶ�ƶ���X����
	����2��     int y����Ƶ�ƶ���Y����
	����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetZoomMove(int x, int y) = 0;

	/***************************************************************
	������:     DestopPlay
	��������:   ���沥��
	����1��     bool btype�� ���룬�Ƿ����沥��
	����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL DestopPlay(bool bDestopplay) = 0;

	/***************************************************************
	������:     GetLastDestopPlay
	��������:   ��ȡ��һ�����沥��
	����1��     bool* bfalg�� ���룬��������沥���Ƿ�δ����
	����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL GetLastDestopPlay(bool* bflag) = 0;

	/***************************************************************
	������:     SetDestopRecover
	��������:   �ָ����汳��
	����1��     bool btype�� ���룬�Ƿ�ָ����汳��
	����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual BOOL SetDestopRecover(bool bflag) = 0;

	/***************************************************************
		������:     SetViewFrame
		��������:   ���ã���ȡ����Ƶ�����ʾģʽ
		����1:      e_Viewframe eViewFrame�����룬��Ƶ��ʾ���ģʽ
		����2:      e_VFULLFRAME eFFrame�����룬ȫ��ģʽ����
		����ֵ��    �ɹ�����TRUE������ʧ��
	*****************************************************************/
	virtual BOOL SetViewFrame(e_Viewframe eViewframe, e_VFULLFRAME eFFrame) = 0;
	virtual BOOL GetViewFrame(e_Viewframe *pviewframe, e_VFULLFRAME *pFFrame) = 0;

public:	//����ɫ������
	/**********************************************************
		����:   ���ã���ȡ�����ȣ��Աȶȣ����Ͷȣ�ɫ��
		����1:      DWORD dwFlag�� �μ�e_VideoColorSe,�ɽ��У�������ȡ��ͬ����
		����2:      int nBrightness�� dwFlag&VIDEO_BRIGHTNESSΪ�棬���������ȣ�������Դ˲�
		����3:      int nSaturation��dwFlag&VIDEO_SATURATIONΪ�棬�����öԱȶȣ�������Դ˲�
		����4:      int nContrast�� dwFlag&VIDEO_CONTRASTΪ�棬�����ñ��Ͷȣ�������Դ˲�
		����5:      int nHue�� dwFlag&VIDEO_HUEΪ�棬������ɫ�ȣ���������Դ˲���
		����ֵ��    �ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetVideoColor(DWORD dwFlag, int nBrightness = 0, int nSaturation = 0, int nContrast = 0, int nHue = 0) = 0;
	virtual BOOL GetVideoColor(DWORD dwFlag, int* pBrightness = NULL, int* pSaturation = NULL, int* pContrast = NULL, int* pHue = NULL) = 0;

	//��Ƶ��ǿ
	/**********************************************************
		����:   ���ã���ȡ����Ƶ��ǿ
		����1:      bool enable�� true������Ƶ��ǿģʽ��false:�ر�
		����2:      OPTIONENHANCE effect��ѡ��
		����ֵ��    �ɹ�����TRUE��ʧ��FALSE
	************************************************************/
	virtual BOOL SetVideoEnhance(bool enable,  OPTIONENHANCE effect = EnhanceEffect_None) = 0;
	virtual BOOL GetVideoEnhance(bool *enable) = 0;

public:	//��������
	/**********************************************************
		����:   ���ã���ȡ�������������ӳټ���ǰ������
		����1:     DWORD dwFlag�� �μ�e_AudioSet���ɽ��У���������(��ȡ)��ͬ����
		����2:     int channel�� �μ�Channel��dwFlag&AUDIO_CHANNELΪ�棬������������������Դ˲�
		����3:     LONGLONG delaytime��dwFlag&AUDIO_DELAYΪ�棬������(��ȡ)�����ӳ٣�������Դ�
		����4:     long lenlarge��ȡֵ��[0�� 100]��dwFlag&AUDIO_ENLARGEΪ�棬������(��ȡ)�����Ŵ�ֵ��������Դ˲�
		����5:     float channelNum��dwFlag&AUDIO_CHANNEL_NUMΪ�棬������(��ȡ)��ǰ��������������Դ˲�
		����ֵ��    �ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetAudio(DWORD dwFlag, int channel = 0, LONGLONG delaytime = 0, long lenlarge = 0, float channelNum = 0.0f) = 0;
	virtual BOOL GetAudio(DWORD dwFlag, Channel *pChannel = NULL, LONGLONG *pDelaytime = NULL, long *pLenlarge = NULL, float *pchannelNum = NULL) = 0;

	//��Ч��������
	/**********************************************************
		����:   ���ã���ȡ����Ч�����Ƿ����ã���Чƽ�⼰��Ч��������
		����1:     DWORD dwFlag�� �μ�e_Equalization���ɽ��У���������(��ȡ)��ͬ����
		����2:     bool enable�� true:������false:�رգ�dwFlag&EQ_ENALBEΪ�棬�������Ƿ�������Ч���⣬������Դ˲�
		����3:     LONG balance��ȡֵ��[0��100]��dwFlag&EQ_BALANCEΪ�棬������(��ȡ)��Чƽ�⣬������Դ˲�
		����4:     int *pEqualization��dwFlag&EQ_EFFECTΪ�棬��������Ч���⣬������Դ˲Σ�Ĭ��NULL
		����ֵ��    �ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetEqualization(DWORD dwFlag, bool enable = true, LONG balance = 0, int *pEqualization = NULL) = 0;
	virtual BOOL GetEqualization(DWORD dwFlag, bool* pEnable = NULL, LONG* pBalance = NULL, int *pEqualization = NULL) = 0;

public:	//��Ļ����
	/**********************************************************
		����:   ���ã���ȡ����Ļ�ļ�·��
		����1:      ���������ļ�
		����ֵ��    �ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetSubtitlePath(const wchar_t* lpszPath/*, int& SubIndex*/) = 0;
	virtual BOOL GetSubtitlePath(LPWSTR lpString, int nMaxCount = MAX_PATH) = 0;

	/**********************************************************
		����:   ��ʾ�����أ���Ļ
		����1:      bool��true����ʾ��false������
		����ֵ��    �ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetSubtitleVisible(bool bVisable, MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual BOOL GetSubtitleVisible(bool *bVisable,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	/**********************************************************
		����:   ���ã���ȡ����Ļλ��
		����1:      ESubPlace place���μ�ESubPlaceö��
		����2:      int cx��X���򳤶�
		����3:      int cy��Y���򳤶�
		����ֵ��    �ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetSubtitlePos(ESubPlace place, int cx, int cy,  MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual BOOL GetSubtitlePos(ESubPlace *place, int *cx, int *cy,  MULTIP_SUB msub = MAJOR_SUB) = 0;

	/**********************************************************
		����:      ��ʾ��ǰ��ʣ�ࡢϵͳ��ʱ��
		����1:    SubTimeType subTimeType���μ�SubTimeTypeö��
		����ֵ�� �ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetSubTimeType(SubTimeType subTimeType) = 0;
	virtual BOOL GetSubTimeType(SubTimeType *subTimeType) = 0;

	/**********************************************************
		����:      ���ã���ȡ����ǰ��ʣ�ࡢϵͳ��ʱ������
		����1:    LOGFONT lf�����õ�����
		����ֵ���ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetSubTimeFont(LOGFONT lf) = 0;
	virtual BOOL GetSubTimeFont(LOGFONT *lf) = 0;

	/**********************************************************
		����:   ���ã���ȡ����Ļ���壬��ɫ��������ɫ���ӳ�
		����1:      DWORD dwFlag�� �μ�e_Subtitle���ɽ��У�������ȡ��ͬ����
		����2:      LOGFONT *psf �� dwFlag&SUBTITLE_FONTΪ�棬���������壬������Դ˲Σ�Ĭ��NULL
		����3:     DWORD dwFontClr��dwFlag&SUBTITLE_CLRΪ�棬������������ɫ��������Դ˲Σ�Ĭ��0
		����4:      int nContrast�� dwFlag&SUBTITLE_OUTLINE_CLRΪ�棬������������ɫ��������Դ˲Σ�Ĭ��0
		����5:      int nHue�� dwFlag&SUBTITLE_DELAYΪ�棬��������Ļ�ӳ٣�������Դ˲Σ�Ĭ��0
		����6:     MULTIP_SUB msub�� ��ǰ��ȡ����Ļ���ͣ��μ�MULTIP_SUB��Ĭ��MAJOR_SUB
		����ֵ��  �ɹ�����TRUE,ʧ��FALSE
	************************************************************/
	virtual BOOL SetSubtitle(DWORD dwFlag, const LOGFONT *psf = NULL, DWORD dwFontClr = 0, DWORD dwOutlineClr = 0, int delaytime = 0, MULTIP_SUB msub = MAJOR_SUB) = 0;
	virtual BOOL GetSubtitle(DWORD dwFlag, LOGFONT *psf = NULL, DWORD *pFontClr = NULL, DWORD *pOutlineClr = NULL, int *pDelaytime = NULL, MULTIP_SUB msub = MAJOR_SUB) =0;

	/**********************************************************
		����:		 �����Ļ
		����ֵ��BOOL��TRUE���ɹ���FALSE��ʧ��
	************************************************************/
	virtual BOOL ClearSubtitle() = 0;

public:	//3D�任

	/************************************************************************
	 ���ܣ�2Dת3DЧ������
	 ������
		����1: DWORD dw3DSet		3D����ѡ��
		����2: int nMode		dw3DSet&T3D_MODEΪ�棬�����õ�ǰ��ʽ��������Դ˲�
		����3: int nAngle		dw3DSet&T3D_ANGLEΪ�棬�˲���Ч��ֵΪe_2T3DMode��������Դ˲�
		����4: float fParalPos	dw3DSet&T3D_PARALAXPOSΪ�棬�˲���Ч��������Դ˲�
		����5: int eChannel		dw3DSet&T3D_CHANNELΪ�棬�˲���Ч��ֵΪe_2T3DViewChannel��������Դ˲�
		����6: e_2T3DFormat eType  dw3DSetΪT3D_Max����Ե�ǰѡ���ʽ�������Բ������ã�������ָ��eType��ʽ����
										��dw3DSet����T3D_FORMAT�������õ�ǰ��ʽΪeType
	����ֵ����
	************************************************************************/
	virtual BOOL SetT3D(DWORD dwFlag, int nMode = 0, int nAngle=0, float fParalPos = 0.0f, int nChannel= 0, e_2T3DFormat eType=T3D_Max) = 0;
	virtual BOOL GetT3D(DWORD dwFlag, e_2T3DMode* pMode = NULL, e_2T3DViewAngle* pAngle = NULL, float *pfParalPos = NULL, e_2T3DViewChannel* pChannel = NULL, e_2T3DFormat* peType=NULL, e_2T3DFormat eType=T3D_Max) = 0;
	virtual BOOL GetT3D(T3DSet *pOutSet, e_2T3DFormat eType=T3D_Max) = 0;

public:	//��Ƶǽ(ע�⣬��Ƶǽ�������ĸ�������Ҫ��VIDEOWALL����ܵ���)
	/***************************************************************
	������:     ResetMonitor                                
	��������:   ���ü�����
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
	*****************************************************************/
	virtual BOOL ResetMonitor() = 0;

	/***************************************************************
	������:     IsVideoWallAvailable                                
	��������:   ��Ƶǽ�Ƿ����
	����ֵ��    ���÷���true������Ϊ������
	*****************************************************************/
	virtual BOOL IsVideoWallAvailable() = 0;

	/***************************************************************
	������:     SetVideoWallMode                                
	��������:   ���ý����˳���Ƶǽģʽ��ʶ
	����ֵ��    trueΪ���룬falseΪ�˳�
	*****************************************************************/
	virtual BOOL SetVideoWallMode(bool flag) = 0;

	/***************************************************************
	������:     GetVideoWallMode                                
	��������:   ��ȡ�����˳���Ƶǽģʽ��ʶ
	����ֵ��    trueΪ���룬falseΪ�˳�
	*****************************************************************/
	virtual BOOL GetVideoWallMode(bool* flag) = 0;

public:	//��ͼ
	/************************************************************************
	 ���ܣ���������ͼ��ָ��ʱ��㣩���˺������������߳��ڵ���
	 ����1��PCutImg pCutIm������pCutImg->dwTime�����pCutImg->pBitmap
	 ����2�� int nCount ,ָ����PCutImg�ĸ���
	 ����ֵ��BOOL��TRUE,��ͼ�ɹ������򣨲��ֽ�ͼ��ʧ��
	 ************************************************************************/
	virtual BOOL CaptureImg(PCutImg pCutImg, int nCount = 1, LPCTSTR lpszMediaFile = NULL,IQvodCallBack* pCallBack = NULL) = 0;

	/************************************************************************
	 ���ܣ���������ͼ��ƽ����ȡN�ţ����˺������������߳��ڵ���
	 ����1��int nCuts����Ҫ��ȡ��ͼƬ��Ŀ������
	 ����2��PCutImg *pCutImg�����ؽ�ȡ����ͼƬ���ݣ����
	 ����3��int *pCounts�����ؽ�ȡ��ͼƬ���������
	 ����4��LPCTSTR lpszMediaFile��ָ��ý���ļ���ȡ�����룬Ĭ��ΪNULL����ǰ���ŵ��ļ�
	 ����ֵ��BOOL��TRUE,��ͼ�ɹ������򣨲��ֽ�ͼ��ʧ��
	 ************************************************************************/
	virtual BOOL CaptureImg(int nCutNum, PCutImg *pOutCutImg, int *pOutCutNum = NULL, LPCTSTR lpszMediaFile = NULL, IQvodCallBack* pCallBack = NULL) = 0;

	virtual void RleaseCutImgs() = 0;
public:
	/************************************************************************
		���ܣ����ò���ģʽ
		����1��BOOL bAuto���Ƿ��Զ����ò���ģʽ
		����2��BOOL bSetEnhance��bAutoΪTRUEʱ��bSetEnhance��������
		*       ���ֶ�������Ƶ��ǿʱ��bAutoΪFALSE��bSetEnhanceΪTRUE
		*       ���ֶ��ر���Ƶ��ǿʱ��bAutoΪFALSE��bSetEnhanceΪFALSE
		����3��int nVideoSet,  �º��壺0.����ģʽ��1.�������ȣ�2.��������
		����4��BOOL bRdsp���Ƿ�Ϊ����棬Ĭ�Ϸ�
		����ֵ��BOOL��TRUE���ɹ���FALS��ʧ��
	************************************************************************/
	virtual UINT SetPlayMode(BOOL bAuto, BOOL bSetEnhance, int nVideoSet = 0, BOOL bRdsp = FALSE) = 0;

	/***************************************************************
		������:     SetVideoStronger
		��������:  ������Ƶ��ǿ����
		����1:      BOOL bSet���Ƿ�Ϊ��Ƶ��ǿ��TRUE���ǣ�FALSE����
		����ֵ��   ��
	*****************************************************************/
	virtual void SetVideoStronger(BOOL bSet) = 0;
	virtual BOOL GetVideoStronger() = 0;

	/***************************************************************
		������:     SetInsertVideoStrong
		��������:  ����Ļ�����ǿ,
		����1:      int nType��-1����Ч��1��������ǿ��0��û�л�����ǿ
		����ֵ��   ��
	*****************************************************************/

	/***************************************************************
		������:     GetBuffering
		��������:  ��Ƶ�Ƿ����ڻ���,
		����ֵ��   ��
	*****************************************************************/
	virtual BOOL GetBuffering() = 0;

	/***************************************************************
		������:     GetNearestFrameOnDirection
		��������:  ��λ�ؼ�֡
		����1��	    REFERENCE_TIME *newTime���������������TRUEʱ��ȡ�����ؼ�֡
		����2��	    int nDirection���ؼ�֡��λ����,(-1:��ǰȡ�ؼ�֡ 0:Ĭ�� 1:����ȡ�ؼ�֡)
		����ֵ��   BOOL��TRUE���ɹ���FALSE����ȡ�ؼ�֡ʧ��
	*****************************************************************/
	virtual BOOL GetKeyPos(REFERENCE_TIME *newTime, int nDirection = 0) = 0;

	/***************************************************************
		������:     MapNetBitNs
		��������:  ��������λ��Ϣ(��ʱֻ���������)
		����1��	    const char* szplayHash����ǰ���������hash
		����ֵ��   NET_ERROR����
	*****************************************************************/
	virtual NET_ERROR UpdateBitInfo(const char* szplayHash) = 0;
	/***************************************************************
		������:     ClearMapBits
		��������:  �������λ������Ϣ��
		����ֵ��   ��
	*****************************************************************/
	virtual void ClearMapBits() = 0;
	virtual void ReSetLoadInfo(BOOL bClearLoadInfo = TRUE) = 0;														// �����������ؽ���
	
	/***************************************************************

	/***************************************************************
		������:     IsDownloadPos
		��������:  ���fPosλ�õ�Ƭ���Ƿ��Ѿ����ص�
		����1��		float fPos,
		����ֵ��   BOOL�Ƿ������أ�TRUE�������أ�FALSE��δ������
		����ֵ��   ��
	*****************************************************************/
	virtual BOOL IsDownloadPos(float fPos) = 0;									

	/***************************************************************
		������:     Seek
		��������:  �ļ���λ����
		����1��		REFERENCE_TIME seekPos����λ����λ��
		����2��		BOOL bKeyFrame���Ƿ�λ���ؼ�֡��TRUE����λ��FALSE������λ��Ĭ��ΪFALSE
		����3��    BOOL bSync���Ƿ�ͬ����TRUE���Ƿ�ͬ������������FALSE���첽��Ĭ��ΪFALSE
		����ֵ��   BOOL, TRUE,�ɹ���FALSE,ʧ��
	*****************************************************************/
	virtual BOOL Seek(REFERENCE_TIME seekPos, BOOL bKeyFrame = FALSE, BOOL bSync = FALSE) = 0;


	/***************************************************************
		������:     IsVideoType
		��������:  ý���Ƿ�Ϊ��Ƶ������ͼ��ͳߴ�
		����ֵ��   BOOL��TRUE������Ƶ��������
	*****************************************************************/
	virtual BOOL IsVideoType() = 0;

	/***************************************************************
	������:     GetCrashLog                               
	��������:   ��ȡ����ʱ��������ģ���������־
	����1��     BYTE** ppbyte, �����Log bufferָ��
	����2��     int& len,	   �����Logbuffer��С
	����ֵ��    ��
	*****************************************************************/
	virtual void GetCrashLog(BYTE** ppbyte, int& len) = 0;

	virtual BOOL SetVideoRender(e_VideoRender eVideoRender) = 0;
};



/***************************************************************
������:     InitPlayCtrl UnInitPlayCtrl
��������:  ��ʼ���ͷ���ʼ��ģ��
����ֵ��    ��
*****************************************************************/
PLAYCRTL_API BOOL WINAPI InitPlayCtrl();
PLAYCRTL_API void WINAPI UnInitPlayCtrl();

/***************************************************************
������:     CreatePlayCtrl
��������:   ����һ����������������
����1��     BOOL bDefaultMediaPlay��ΪTRUE����Ϊ��ǰĬ�ϵĲ���������
����ֵ��    �ɹ����ش����ɹ��Ĳ���������id��ʧ�ܷ���-1��
*****************************************************************/
PLAYCRTL_API int WINAPI CreatePlayCtrl(bool bDefaultMediaPlay = false, bool bCreateByInsert = false);

/***************************************************************
������:     SetCurPlayCtrlId
��������:   ���ã���ȡ����ǰ������id
����1��     long lMediaID�������Ĳ�����ID�����õ�ǰ�Ĳ�����
����ֵ��    �ɹ�����PlayCtrl*��ʧ�ܷ���NULL
*****************************************************************/
PLAYCRTL_API BOOL WINAPI SetCurPlayCtrlId(long lcurMediaID);		
PLAYCRTL_API long WINAPI GetCurPlayCtrlId();

/***************************************************************
������:     GetPlayCtrl
��������:   ��ȡĳ����������������
����1��     long lMediaID������Ϊ-1�����ȡ��ǰ���õĲ�����
����ֵ��    �ɹ�����PlayCtrl*��ʧ�ܷ���NULL
*****************************************************************/
PLAYCRTL_API PlayCtrl* WINAPI GetPlayCtrl(long lMediaID = -1);		

/***************************************************************
������:     ReleasePlayCtrl
��������:   �ͷŹرմ����Ĳ���������
����1��     long lMediaID������Ϊ-1�����ͷ����д����Ĳ������������ã�����ֻ�ͷ�ָ������id�Ĳ�����
����2��     BOOL bResMediaModule������ΪTRUE�����ͷ� ���ص�QvodPlayMedia ģ�����������ͷ�
����ֵ��    ��
*****************************************************************/
PLAYCRTL_API void WINAPI ReleasePlayCtrl(long lMediaID = -1);
