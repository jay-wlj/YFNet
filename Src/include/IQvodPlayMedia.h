#ifndef _QVOD_PLAYMEDIA_H__
#define _QVOD_PLAYMEDIA_H__

#include "QvodError.h"
#include "CallbackDefine.h"
// #include "windows.h"
// #include "strmif.h"
#include "IColorFilter.h"

#ifndef interface
#define interface struct
#endif

enum DVD_MENU_ID;
#ifndef byte
typedef unsigned char byte;
#endif
typedef LONGLONG REFERENCE_TIME;
//播放文件的类型 DS SWF
enum e_FileType
{
	_FT_Unknow = 0,
	_FT_LocalFile,
	_FT_DvdFile,
	_FT_ImageFile,
	_FT_TS_STREAM,
	_FT_Equipment,
	_FT_SwfFile   = 0x10,
	_FT_SwfF4v
};
//媒体文件类型
enum e_CONTAINER
{
	_E_CON_UNKNOW = 0,

	_E_CON_RMVB = 1,
	_E_CON_AVI,
	_E_CON_MKV,
	_E_CON_MP4,		//MPEG-4 Part 14,3GP系
	_E_CON_3GP,		//3GP

	_E_CON_MOV,		//Apple系 包含MOV，QT
	//_E_CON_QT,

	_E_CON_MPEG,	//MPEG1/2/4 标准容器格式
	_E_CON_DAT,
	_E_CON_MPEG4ES,	//MPEG4基本流
	_E_CON_MPEGTS,	//TS
	_E_CON_MPEGPS,	//VOB EVO HD-DVD
	_E_CON_HKMPG,	//Hikvision mp4
	_E_CON_DIVX,	//Divx Media Format(DMF)
	_E_CON_WM,		//ASF/WMV/WMA/WMP
	_E_CON_TS,		//TS
	_E_CON_M2TS,	//M2TS/MTS,支持DBAV 与 AVCHD
	_E_CON_DVD,		//DVD盘，特殊处理（DVD导航）

	_E_CON_QMV,
	_E_CON_VP5,
	_E_CON_VP6,
	_E_CON_VP7,
	_E_CON_WEBM,//VP8

	_E_CON_FLV,
	_E_CON_PMP,
	_E_CON_BIK,
	_E_CON_CSF,
	_E_CON_DV,

	_E_CON_XLMV,//迅雷格式

	_E_CON_M2V,
	_E_CON_SWF,
	_E_CON_AMV, //Anime Music Video File
	_E_CON_IVF,//Indeo Video Format file
	_E_CON_IVM,

	_E_CON_SCM,
	_E_CON_AOB,

	_E_CON_AMR,
	_E_CON_DC,
	_E_CON_DSM,
	_E_CON_MP2,
	_E_CON_MP3,
	_E_CON_OGG,
	_E_CON_DTS,
	_E_CON_MIDI,
	_E_CON_WVP,
	_E_CON_AAC,
	_E_CON_AC3,
	_E_CON_AIFF,
	_E_CON_ALAW,
	_E_CON_APE,
	_E_CON_AU,
	_E_CON_AWB,//AMR-WB
	_E_CON_CDA,
	_E_CON_FLAC,
	_E_CON_MOD,
	_E_CON_MPA,//MPEG-2 Audio File
	_E_CON_MPC,//Musepack Audio
	_E_CON_RA,
	_E_CON_TTA,
	_E_CON_WAV,
	_E_CON_CDXA,

	_E_CON_SKM,
	_E_CON_MTV,
	_E_CON_MXF,
	_E_CON_QMVB,
	_E_CON_OTHER = 0x100
};

//视频编码类型
enum e_V_ENCODER
{
	/*MPEG 系列*/
	_V_MPEG_1,
	_V_MPEG_2,

	_V_MPEG_4_Visual,//Simple
	_V_MPEG_4_Advanced_Simple,//Advanced Simple

	_V_XVID,
	_V_DIVX,
	//_V_AVC,//AVC1/H264/X264

	/*ITU 系列*/
	_V_H261,
	_V_H263,
	_V_H263_PLUS,
	_V_H264,

	/*Real Media系列*/
	_V_RV10,
	_V_RV20,
	_V_RV30,
	_V_RV40,

	/*Windows 系列*/
	_V_WM_MPEG4_V1,//Microsoft ISO MPEG-4 Video Codec
	_V_WM_MPEG4_V2,//Microsoft MPEG-4 Video Codec v2	MP42
	_V_WM_MPEG4_V3,//Microsoft MPEG-4 Video Codec v3	MP43

	_V_WMV7,//Windows Media Video v7	WMV1
	_V_WMV8,//Windows Media Video v8	WMV2
	_V_WMV9,//Windows Media Video v9	WMV3
	_V_WVC1,//Windows Media Video v9 Advanced Profile	WVC1
	_V_MSVIDEO1,//MS VIDEO 1
	/*Google(ON2)*/
	_V_VP3,
	_V_VP4,
	_V_VP5,
	_V_VP6,
	_V_VP60,
	_V_VP61,
	_V_VP62,
	_V_VP7,
	_V_VP8,

	/*其他*/
	_V_HFYU,		//Huffyuv
	_V_YUV,			//YUV
	_V_YV12,		//YV12
	_V_UYVY,		//UYVY
	_V_JPEG,        //JPEG11
	_V_RGB,
	_V_PNG,

	_V_IV31,		//IndeoV31
	_V_IV32,

	//_V_FLV,			//FLV
	_V_MJPG,		//MOTION JPEG
	_V_LJPG,		//LossLess JPEG

	_V_ZLIB,		//AVIZlib
	_V_TSCC,		//tscc
	_V_CVID,		//Cinepak
	_V_RAW,			//uncompressed Video
	_V_SMC,			//Graphics
	_V_RLE,			//Animation
	_V_APPV,		//Apple Video
	_V_KPCD,		//Kodak Photo CD
	_V_SORV,		//Sorenson video
	_V_SCRN,		//Screen video
	_V_THEORA,		//Ogg Theora
	_V_ZMP5,
	_V_CSF,
	_V_OTHER	= 0x100	//未知
};

enum e_A_ENCODER
{
	_A_AAC	= 0,		//LC-AAC/HE-AAC
	_A_AC3,				//AC3
	_A_E_AC3,			//E-AC-3
	_A_MP2,				//MPEG1 Layer 2
	_A_MP3,				//MPEG1 Layer 3
	_A_MP4,             //MPEG4 Audio
	_A_AMR_NB,			//AMR
	_A_AMR_WB,
	_A_DTS,
	_A_MIDI,
	_A_PCM,				//脉冲编码调制
	_A_DPCM,			//差分脉冲编码调制
	_A_APCM,			//自适应脉冲编码调制
	_A_ADPCM,			//自适应差分脉冲编码调制
	_A_PCM_LE,			//PCM Little Endian
	_A_PCM_BE,			//PCM Big	 Endian

	_A_COOK,			//rmvb AUDIO
	_A_SIPR,			//rmvb AUDIO
	_A_TSCC,			//tscc

	_A_QCELP,			//Qualcomm公司于1993年提出了可变速率的CELP，常称为QCELP,QCELP 语音压缩编码算法包括8K和13K两种

	_A_WMA1,
	_A_WMA2,
	_A_WMA3,

	_A_WMA4,
	_A_WMA5,
	_A_WMA6,

	_A_WMA7,
	_A_WMA8,
	_A_WMA9,            //windows media audio 9.0

	_A_RAW,             //uncompressed audio
	_A_MAC,             //kMACE3Compression
	_A_IMA,             //kIMACompression, Samples have been compressed using IMA 4:1.
	_A_LAW,             //kULawCompression,kALawCompression, uLaw2:1
	_A_ACM,             //kMicrosoftADPCMFormat: Microsoft ADPCM-ACM code 2;kDVIIntelIMAFormat:DVI/Intel IMAADPCM-ACM code 17
	_A_DVA,             //kDVAudioFormat:DV Audio
	_A_QDM,             //kQDesignCompression:QDesign music
	_A_QCA,             //kQUALCOMMCompression:QUALCOMM PureVoice
	_A_G711,            //G.711 A-law logarithmic PCM and G.711 mu-law logarithmic PCM
	_A_NYM,             //Nellymoser
	_A_SPX,             //Speex
	_A_VORBIS,          //Ogg vorbis
	_A_APE,				//Monkey's audio
	_A_FLAC,
	_A_ALAC,
	_A_MLP,				//DVD-Audio MLP
	_A_CSF,
	_A_AIFF,			//AIFF
	_A_TTA,				//True Type Audio
	_A_VP7,
	_A_MPC,				//Musepack audio
	_A_TRUEHD,			//Dolby True HD
	_A_WVP,				//Wavpack
	_A_OTHER   = 0x100	//未知
};

//播放选项
enum e_Play_Option
{
	E_INTELLIGENCE, //智能模式
	E_DXVA_ACCEL,   //性能优先
	E_VIDEO_POST    //画质优先
};

struct PlayOption_Param
{
	DWORD std_min_height;//标清最小高度（像素），默认值480p
	DWORD std_max_height;//标清最大高度（像素），默认值720p
	DWORD std_min_bitrate;//标清最小码率（bps），默认值450000bps
	DWORD low_min_bitrate;//低清最小码率（bps），默认值300000bps
};
//播放文件的协议
enum e_FileProtocol
{
	_FP_Unknow = 0,
	_FP_LocalFile = 1,
	_FP_P2PFile,
	_FP_Torrent,
	_FP_HttpFile,
	_FP_FtpFile,
	_FP_TSFile,
	_FP_RtspFile,
	_FP_MmsFile,
	_FP_TSSTREAM
};
//文件播放状态
enum e_PlayState
{
	_PS_Init = 0,
	_PS_Stop,
	_PS_Pause,
	_PS_Playing,
	_PS_Buffering,
	_PS_Waiting,
	_PS_MediaEnded,
	_PS_Ready,
	_PS_Conning,
	_PS_BTDowning,
	_PS_CreateDS,
	_PS_Stoping,
	_PS_PlayAd,
	_PS_PlayLocalAd,
	_PS_PlayPauseAd,
	_PS_PlayBufferingAd,
	_PS_Recording
};

//全屏模式
enum e_VFULLFRAME
{
	_VFF_DEFAULT,
	_VFF_FILM
};

//截图模式
enum e_CAPTUREMODE
{
	_E_CM_CURRENT,
	_E_CM_GENNER,
	_E_CM_FILTER,
	_E_CM_NULL
};
//视频框架
enum e_Viewframe
{
	_VF_SF = 0,
	_VF_Fill,
	_VF_43,
	_VF_54,
	_VF_169,
	_VF_FILM
};
//声道
enum e_SoundChannel
{
    _SC_DEFAULT = 0,	//默认
	_SC_LEFT,			//左声道
	_SC_RIGHT,			//右声道
	_SC_MONO,			//混合音
	_SC_LTOALL,			//左声道扩展到全部
	_SC_RTOALL			//右声道扩展到全部
};

#define	 D_VOLUME	80

//画面旋转
enum E_FlipType
{
	LeftFlip = 1,         //左转90度
	RightFlip,            //右转90度
    HorizonalFlip,        //水平翻转
	VerticalFlip,         //垂直翻转
	NoFlip                //不翻转
};

//录像画质
enum E_RVQuality
{
	E_RQ_Low,          //低
	E_RQ_Medium,       //中
	E_RQ_Hight         //高
};

//媒体类型
enum E_MEDIA_TYPE
{
	E_MT_VIDEO,  //视频
	E_MT_AUDIO,  //音频
	E_MT_VIDEO_ONLY,//仅仅视频
	E_MT_CSF,
	E_MT_SWF,
	E_MT_UNKNOWN
};

////文件信息
//typedef struct  s_FileInfo
//{
//	__int64		lFileSize;          //媒体文件大小(单位：字节)
//	long		lDuration;          //播放时长(单位：毫秒)
//	long		lWidth;             //图像宽(单位：像素)
//	long		lHeight;            //图像高
//	long		lRate;              //码率(单位：kbps)
//	long		lIndex;
//	char		szHash[41];         //哈希值
//	e_V_ENCODER eVEncoder;          //视频编码类型
//	e_A_ENCODER eAEncoder;          //音频编码器
//	long        nTitleSize;         //标题长度
//	long        nAuthorSize;        //艺术家长度
//	long        nAlbumSize;         //专辑长度
//	char		szTitle[MAX_PATH];  //标题
//	char		szAuthor[MAX_PATH]; //艺术家
//	char		szAlbum[MAX_PATH];  //专辑
//	wchar_t		szfilepath[MAX_PATH];
//} s_FileInfo;

enum E_InfoType  //信息类型
{
	E_ST_SubTitle,                 //字幕
	E_ST_Video,                    //视频流
	E_ST_Audio,                    //音频流
	E_ST_FilterNAME,               //Filter名称
	E_ST_Unknown                   //未知
};

struct S_NotifyInfo               //媒体播放模块回调信息结构体
{
	WCHAR Name[128];              //名称(信息内容)
	E_InfoType type;              //信息类型
	int Index;                    //信息索引（用于设置该项有效）
	int StreamTip;                //相关接口下标，内部使用
	S_NotifyInfo():type(E_ST_Unknown),Index(-1),StreamTip(-1)
	{
		memset((void*)Name, 0, 128*sizeof(WCHAR));
	}
};

struct S_DVDStreamInfo
{
	LCID lcid;
	TCHAR info[30];
	S_DVDStreamInfo():lcid(0)
	{
		memset(info, 0, 30);
	}
};

struct S_DVDINFO
{
	ULONG size;
	int curValue;
	S_DVDStreamInfo dvdinfo[32];
	S_DVDINFO():size(0),curValue(0){}
};

struct DVDTITLE//DVD标题信息
{
    S_DVDINFO DVDAudio;       //音频流信息
	S_DVDINFO DVDSubTitle;    //字幕信息
	ULONG Anglenum;           //视角数
	int curAngle;           //当前视角
	ULONG Chapternum;         //章节数
	int curChapter;         //当前播放章节
	ULONG Titlenum;           //标题数
	int curTitle;           //当前标题
	DVDTITLE():Anglenum(0),curAngle(-1),Chapternum(0),
		       curChapter(-1),Titlenum(0),curTitle(-1){}
};
struct S_StreamInfo
{
	int SubTitleNum;          //字幕数
	int AudioNum;             //音频数
	int CurAudio;             //当前音频流
	int CurSubTitle;          //当前字幕
	int StreamSelectNum;      //当前IAMStreamSelect的接口数
	S_NotifyInfo SubTitleinfo[10]; //字幕信息
	S_NotifyInfo AudioInfo[10];    //音频信息
	interface IAMStreamSelect *pStreamSelect[10]; //IAMStreamSelect接口
	S_StreamInfo():SubTitleNum(0),AudioNum(0),StreamSelectNum(0),CurAudio(-1),CurSubTitle(-1)
	{
		memset((void*)pStreamSelect, 0, sizeof(IAMStreamSelect *)*10);
	}
};

//VideoRender
enum e_VideoRender
{
	_VR_Default = 0, //_VR_VMR7Windowed = 0, _VR_OverLay = 1,
	_VR_Normal,
#ifdef _WIN32
	_VR_VMR7Windowless,   //VMR7
	_VR_VMR9Windowed,     //VMR9
	_VR_VMR9Windowless,   //
	_VR_EVRRender,
	//_VR_EVRVista_CP,
#endif
	_VR_RENDER_AUTO,
	_VR_NONE
};

//DVD
typedef struct _dvd_titles
{
	ULONG numtitles;									//标题数
	ULONG numchapters;									//章节数
}DVDTITLES;

typedef struct _dvdstruct
{
	ULONG numtitles;									//标题数
	ULONG numchapters;									//章节数
	ULONG numangles;									//视角数
	ULONG numaudios;									//配音语言数
	ULONG numwordlangs;									//字幕数
	//下面是所有的内容
	//LCID SSS
	//LCID SSS
}DVDSTRUCT;

typedef struct _dvd_title_change
{
	ULONG curtitle;										//当前标题
	ULONG numchapters;									//章节数
	ULONG numangles;									//视角数
	ULONG numaudios;									//配音语言数
	ULONG numwordlangs;									//字幕数
	//下面是所有的内容
	//LCID SSS
	//LCID SSS
}DVD_TITLE_CHANGE;

extern GUID IID_IQvodPlayMedia;// = {0xce0e9322, 0x3559, 0x417f, 0xa9, 0xba, 0xae, 0xfe, 0x73, 0xb1, 0xbf, 0x75};

typedef void (*_pfnUserFilter) (void *filter, void *pparam );

interface IQvodPlayMedia : public IQvodCallBack
{
/*********************************************************************
        函数名:     PlaySource
        函数描述:   播放媒体
        参数1:      wchar_t *pFileUrl 输入，媒体文件本地、网络路径
		参数2:      E_MEDIA_TYPE mt = E_MT_VIDEO，播放媒体播放类型，默认为音视频
		参数3:      LONG lStart = 0，输入，开始播放时间，单位为毫秒
		返回值：    成功返回E_QVOD_S_OK,否则为失败
 *********************************************************************/
	virtual QVOD_ERROR PlaySource(wchar_t *pFileUrl, E_MEDIA_TYPE mt = E_MT_VIDEO, LONGLONG lStart = 0) = 0;

/*********************************************************************
        函数名:     PlayAd
        函数描述:   播放广告
        参数1:      wchar_t *pAdUrl = NULL 输入，flash广告的
                    本地路径，若为NULL则为公司默认的网站广告
		返回值：    成功返回E_QVOD_S_OK,否则为错误
**********************************************************************/

	     virtual QVOD_ERROR PlayAd(wchar_t *pAdUrl, bool bfixsize, RECT *prect) = 0;

/*********************************************************************
         函数名:     StopAd
         函数描述:   停止广告播放，调用本函数无论在播放
		             本地flash广告还是网站广告，都将停止
         返回值：    成功返回E_QVOD_S_OK,广告已经停止返回E_QVOD_S_FALSE，否则为失败
 *********************************************************************/

	      virtual QVOD_ERROR StopAd() = 0;

/*********************************************************************
          函数名:     SetPauseAdEnable
          函数描述:   设置是否启动网络文件的暂停广告
          参数1:      bool bAdEnable 输入，TRUE为设置，否则为不设置
**********************************************************************/

	      virtual void SetPauseAdEnable(bool bAdEnable) = 0;

/*********************************************************
          函数名:     Play
          函数描述:   播放已停止或暂停的播放任务
          返回值：    成功返回E_QVOD_S_OK,媒体已经在播放返回E_QVOD_S_FALSE，否则为失败
**********************************************************/

	      virtual QVOD_ERROR Play() = 0;

/**********************************************************
          函数名:     Stop
          函数描述:   停止正在播放或暂停的播放任务
		  返回值：    成功返回E_QVOD_S_OK,媒体已经停止返回E_QVOD_S_FALSE，否则为失败
***********************************************************/

	   virtual QVOD_ERROR Stop() = 0;

/**********************************************************
		函数名:     Pause
		函数描述:   暂停正在播放的播放任务
		返回值：    成功返回E_QVOD_S_OK,媒体已经暂停或停止返回E_QVOD_S_FALSE，否则为失败
***********************************************************/

	virtual QVOD_ERROR Pause() = 0;

/**********************************************************
		函数名:     GetPlayState
		函数描述:   获取当前媒体播放模块的播放状态
		返回值：    成功返回E_QVOD_S_OK，否则为失败
***********************************************************/

	virtual QVOD_ERROR GetPlayState(e_PlayState *pPlayState) = 0;

/**********************************************************
		函数名:     GetCurrentPos
		函数描述:   获取当前媒体播放进度的时间点
		参数1:      long *pValue， 输出，媒体文件当前播放的时间点,单位为毫秒
		返回值：    成功返回E_QVOD_S_OK，当前不在播放状态返回E_QVOD_S_FALSE，否则为失败
************************************************************/

	virtual	QVOD_ERROR GetCurrentPos(REFERENCE_TIME *pValue) = 0;

/**********************************************************
		函数名:     SetCurrentPos
		函数描述:   设置当前媒体播放的时间点
		参数1:      long  lValue， 输入，要设置到的媒体文件播放时间点,单位为毫秒
		返回值：    成功返回E_QVOD_S_OK，当前不在播放状态返回E_QVOD_S_FALSE，否则为失败
************************************************************/

	virtual QVOD_ERROR SetCurrentPos(REFERENCE_TIME  lValue) = 0;

/**********************************************************
函数名:     CurrentMediaCanSeek
函数描述:   当前媒体是否可拖拽
返回值：    可拖拽返回true，否则为false
************************************************************/

	virtual bool CurrentMediaCanSeek() = 0;

/**********************************************************
函数名:     GetMediaFileType
函数描述:   获取指定文件的文件类型
参数1:      wchar_t *pFileUrl 文件路径
返回值：    指定文件的文件类型
************************************************************/
	virtual e_CONTAINER GetMediaFileType(wchar_t *pFileUrl) = 0;
	//e_CONTAINER GetMediaFileType(wchar_t *pFileUrl);
/**********************************************************
		函数名:     GetDuration
		函数描述:   获取当前媒体文件的播放时长
		参数1:      long *pValue， 输出，媒体文件的播放时长,单位为毫秒
		返回值：    成功返回E_QVOD_S_OK，当前不在播放状态返回E_QVOD_S_FALSE，否则为失败
************************************************************/
	virtual QVOD_ERROR GetDuration(REFERENCE_TIME *pValue) = 0;

//全屏接口（保留，目前未使用）
	//virtual QVOD_ERROR GetFull(bool *pValue) = 0;
	//virtual QVOD_ERROR SetFull(bool bValue)  = 0;

/*************************************************************
		函数名:     GetMute
		函数描述:   获取播放器的静音标识
		参数1:      long *pValue， 输出，已设置静音为true，否则为false
		返回值：    成功返回E_QVOD_S_OK，传入指针为NULL返回E_QVOD_S_FALSE
***************************************************************/

	virtual QVOD_ERROR GetMute(bool *pValue) = 0;

/***************************************************************
		函数名:     SetMute
		函数描述:   设置播放器的静音标识
		参数1:      bool bValue， 输入，静音标识，设置静音为true，否则为false
		返回值：    成功返回E_QVOD_S_OK，已设置返回E_QVOD_S_FALSE，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetMute(bool bValue) = 0;

/***************************************************************
		函数名:     GetVolume
		函数描述:   获取播放器的音量
		参数1:      long *pValue， 输出， 当前播放器的音量0 - 100的音量级别
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetVolume(long *pValue) = 0;

/***************************************************************
		函数名:     SetVolume
		函数描述:   设置播放器的音量
		参数1:      bool lValue， 输入，0-100的音量级别
		返回值：    成功返回E_QVOD_S_OK，传入指针为NULL返回E_QVOD_S_FALSE
*****************************************************************/

	virtual QVOD_ERROR SetVolume(long lValue) = 0;

/***************************************************************
		函数名:     SetPlaySpeed
		函数描述:   设置播放器的播放速率
		参数1:      long lSpeed，输入，1：加速0.1个级别，-1，减速0.1个级别
		返回值：    成功返回E_QVOD_S_OK，已设置返回E_QVOD_S_FALSE，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetPlaySpeed(double lSpeed) = 0;

/***************************************************************
		函数名:     GetPlaySpeed
		函数描述:   设置播放器的播放速率
		参数1:      long *plSpeed，输出，取值范围为0.11-3.0， 大于1表明在加速播放，小于1表明在减速播放，等于1正常播放
*****************************************************************/

	virtual QVOD_ERROR GetPlaySpeed(double *plSpeed) = 0;

/***************************************************************
函数名:     GetVideoFrameNum
函数描述:   获取到达视频渲染的总帧数
参数1:      int *pframenum，输入输出，视频渲染的总帧数
*****************************************************************/
	virtual QVOD_ERROR GetVideoFrameNum(int *pframenum) = 0;

/***************************************************************
		函数名:     GetViewFrame
		函数描述:   获取播放器的播放速率
		参数1:      e_Viewframe *pViewFrame，返回视频显示框架模式
*****************************************************************/

	virtual	QVOD_ERROR GetViewFrame(e_Viewframe *pviewframe, e_VFULLFRAME *pFFrame) = 0;

/***************************************************************
		函数名:     SetViewFrame
		函数描述:   设置视频框架显示模式
		参数1:      e_Viewframe eViewFrame，输入，视频显示框架模式
		返回值：    成功返回E_QVOD_S_OK，已设置返回E_QVOD_S_FALSE，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetViewFrame(e_Viewframe eViewframe, e_VFULLFRAME eFFrame) = 0;

/***************************************************************
		函数名:     GetSoundChannel
		函数描述:   获取音频声道模式
		参数1:      e_SoundChannel *pSoundChannel，输出，当前音频声道模式
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetSoundChannel(e_SoundChannel *pSoundChannel) = 0;

/***************************************************************
		函数名:     SetSoundChannel
		函数描述:   设置音频声道模式
		参数1:      e_SoundChannel eSoundChannel，输入，音频声道模式
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetSoundChannel(e_SoundChannel eSoundChannel) = 0;

/***************************************************************
	函数名:     SetVideoRender
	函数描述:   设置VideoRender类型
	参数1:      e_VideoRender eVideoRender，输入，VideoRender类型
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetVideoRender(e_VideoRender eVideoRender) = 0;

/***************************************************************
		函数名:     GetVideoRender
		函数描述:   获取VideoRender类型
		参数1:      e_VideoRender *pVideoRender，输出，当前VideoRender类型
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetVideoRender(e_VideoRender *pVideoRender) = 0;

/***************************************************************
		函数名:     GetAudioRenderCount
		函数描述:   获取AudioRender数
		参数1:      long *pAudioCount，输出，AudioRender数
		参数2：     long *pBufSize，输出，已使用的buffer大小
		参数3：     wchar_t *pAudioText，输入、输出，音频Render名称
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetAudioRenderCount(long *plAudioCount, long *plBufSize, wchar_t *pAudioText) = 0;

/***************************************************************
		函数名:     GetCurAudioRender
		函数描述:   获取当前使用的Audiorender
		参数1:      long *pAudioRender，输出，当前使用的Audiorender
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetCurAudioRender(long *plAudioRender) = 0;

/***************************************************************
		函数名:     SetCurAudioRender
		函数描述:   设置AudioRender
		参数1:      long lAudiooRender，输入，Audiorender索引（根据上面得到的）
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetCurAudioRender(long lAudiooRender) = 0;

/***************************************************************
		函数名:     GetClipSize
		函数描述:  获取图像的宽高
		参数1:      long *pWidht，输出，视频宽
		参数2：     long *pHeight，输出，视频高
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetClipSize(long *pWidht, long *pHeight) = 0;

	/***************************************************************
	函数名:     GetCurrentMediaType
	函数描述:  获取媒体音视频类型
	返回值：    成功返回E_QVOD_S_OK，否则为失败
	*****************************************************************/

	//virtual QVOD_ERROR GetCurrentMediaType(long *pWidht, long *pHeight) = 0;
/***************************************************************
		函数名:     GetRate
		函数描述:  获取媒体播放的码率
		参数1:      long *pRate，输出，媒体播放的码率
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetRate(long *pRate) = 0;

/***************************************************************
		函数名:     GetCurrentImage
		函数描述:  获取当前视频图像
		参数1:      PBYTE *ppbuf，输出，BITMAPINFOHEADER+位图数据
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetCurrentImage(PBYTE *ppbuf, e_CAPTUREMODE eCaptrueMode = _E_CM_CURRENT, REFERENCE_TIME* psampletime = NULL) = 0;

/***************************************************************
		函数名:     GetBufferPer
		函数描述:  获取网络文件缓冲百分比
		参数1:      long *pBufferPer，输出，网络文件缓冲百分比（缓冲时间为10s）
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetBufferPer(long *pBufferPer) = 0;

//filter order
#ifdef  _WIN32
/***************************************************************
		函数名:     EnumFilters
		函数描述:   枚举Graph中的Filter
		参数1:      _pfnUserFilter fnUserFileter，输出，枚举出Filter后的操作函数
		参数2:      void *pVoid， 指针函数的参数
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR EnumFilters(_pfnUserFilter fnUserFileter, void *pVoid) = 0;

/***************************************************************
	函数名:     ShowFilter
	函数描述:   显示指定Filter的属性页
	参数1:      wchar_t *pFilterName，输入，Filter名称
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR ShowFilter(wchar_t *pFilterName) = 0;
#endif
/***************************************************************
	函数名:     GetFileInfo
	函数描述:   获取媒体文件的媒体信息
	参数1:      wchar_t *pFileUrl，输入，文件路径
	参数2:      s_FileInfo	*ps_FileInfo，输出，文件信息结构体
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetFileInfo(wchar_t *pFileUrl, struct s_FileInfo	*ps_FileInfo, E_MEDIA_TYPE& mt) = 0;

/***************************************************************
函数名:     GetMediaInfo
函数描述:   获取媒体文件的媒体信息(使用mediainfo库)
参数1:      wchar_t *pFileUrl，输入，文件路径
参数2:      Qvod_MainMediaInfo	*ps_FileInfo，输出，文件信息结构体
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

    virtual QVOD_ERROR GetMediaInfo(wchar_t *pFileUrl, struct Qvod_DetailMediaInfo	*ps_MediaInfo) = 0;

/***************************************************************
		函数名:     GetFileType
		函数描述:   获取当前播放媒体文件的文件类型
		参数1:      e_FileType   *pFileType，输出，文件路径
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetFileType(e_FileType   *pFileType) = 0;

/***************************************************************
	函数名:     GetFileProtocol
	函数描述:   获取当前播放媒体文件的文件协议
	参数1:     e_FileProtocol *pFileProtocol，输出，文件协议
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetFileProtocol(e_FileProtocol *pFileProtocol) = 0;

/***************************************************************
	函数名:     GetFileDownloadLen
	函数描述:   获取网络文件的当前下载长度
	参数1:      INT64& iDownLoadLen，输出，返回当前下载长度
	返回值：    成功返回E_QVOD_S_OK，当前播放的不是网络文件
	            返回E_QVOD_S_FALSE，否则为失败
*****************************************************************/

	//virtual QVOD_ERROR GetFileDownloadLen(INT64& iDownLoadLen) = 0;

/***************************************************************
	函数名:     SetBufferTime
	函数描述:   设置网络文件播放时的缓冲时间
	参数1:      int nTime，输入，缓冲时间
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	//virtual QVOD_ERROR SetBufferTime(int nTime) = 0;
	 //QVOD_ERROR SetBufferTime(int nTime);

/***************************************************************
	函数名:     SetParentWnd
	函数描述:   设置视频播放窗口的父窗口句柄
	参数1:      long lParentWnd，输入，父窗口句柄
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetParentWnd(long lParentWnd) = 0;		//HWND

/***************************************************************
		函数名:     SetQvodNet
		函数描述:   设置QvodNet接口指针
		参数1:      void **pIQvodNet，输入，QvodNet接口指针
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetQvodNet(void **pIQvodNet) = 0;

/***************************************************************
函数名:     SetQlive
函数描述:   设置Qlive接口指针
参数1:      void **pIQlive，输入，Qlive接口指针
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	//virtual QVOD_ERROR SetQlive(void **pIQlive) = 0;

/***************************************************************
函数名:     IsOpenDxva
函数描述:   是否打开硬件加速
参数1:      bool isopen，输入，是否启动硬件加速，ture为启动，false为不启动
返回值：    无
*****************************************************************/

	//virtual void IsOpenDxva(bool isopen) = 0;

/***************************************************************
函数名:     SetPlayOption
函数描述:   设置播放选项（启
动硬件加速、启动视频调节、不启动硬件加速和视频调节）
参数1:      e_Play_Option eOption，输入，启动硬件加速、启动视频后处理、不启动硬件加速和视频后处理
返回值：    无
*****************************************************************/
     virtual HRESULT SetPlayOption(e_Play_Option eOption, PlayOption_Param* pParam = NULL) = 0;

/***************************************************************
函数名:     GetPlayOption
函数描述:   获取播放选项设置
参数1:      e_Play_Option eOption，输入，输出，启动硬件加速、启动视频后处理、不启动硬件加速和视频后处理
返回值：    无
*****************************************************************/
	virtual void GetPlayOption(e_Play_Option &eOption, PlayOption_Param* pParam = NULL) = 0;

/***************************************************************
函数名:     RecordVideo
函数描述:   录制asf视频
参数1:      wchar_t *pPath录制视频的保存路径
参数2：     int quality 录制视频质量（ 可分为高中低三种）
参数3：     E_MEDIA_TYPE mt 媒体类型（分为视频，音频，仅仅视频三种）
参数4：     long rtStrat 录制开始时间(单位为：ms)
参数5：     long rtStop  录制结束时间(单位为：ms)
参数6：     long lwidth 录制图像宽
参数6：     long lheight 录制图像高
返回值：    成功返回S_OK，否则为失败
remark:     本接口对音视频提供三种不同的媒体质量录制：音频录制时低码率为8Kbps，中码率为32Kbps，高码率为44Kbps
            视频录制时低码率为185Kbps，中码率为386Kbps，高码率为1272Kbps
*****************************************************************/

	//virtual	QVOD_ERROR RecordVideo( wchar_t *pPath,
	//								E_RVQuality quality,
	//								E_MEDIA_TYPE mt,
	//								long rtStrat,
	//								long rtStop,
	//								long lwidth = 0,
	//								long lheight = 0) = 0;

/***************************************************************
函数名:     StopRecord
函数描述:   停止录制视频
参数1:      bool ContinuePlay停止后是否继续播放（true为继续，false为不继续播放）
返回值：    成功返回S_OK，否则为失败
*****************************************************************/

	//virtual	QVOD_ERROR StopRecord(bool bContinuePlay = true) = 0;

/***************************************************************
函数名:     SetVideoEnhance
函数描述:   视频增强
参数1:      bool IsEnhance视频增强（true为增强，false为不增强）
返回值：    无
*****************************************************************/
	//virtual QVOD_ERROR SetVideoEnhance(bool IsEnhance) = 0;

/***************************************************************
函数名:     GetVideoEnhance
函数描述:   视频增强
参数1:      bool IsEnhance视频增强（true为增强，false为不增强）
返回值：    无
*****************************************************************/
	//virtual QVOD_ERROR GetVideoEnhance(bool &IsEnhance) = 0;

/***************************************************************
	函数名:     SetCallBack
	函数描述:   设置主窗口消息回调函数
	参数1:      void **pIQvodNet，输入，QvodNet接口指针
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual	QVOD_ERROR SetCallBack(IQvodCallBack *pQvodCallBack) = 0;

/***************************************************************
	函数名:     MoveWindow
	函数描述:   视频窗口调整函数
	参数1:      void **pIQvodNet，输入，QvodNet接口指针
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	//virtual	QVOD_ERROR MoveWindow(CRect *prect = NULL) = 0;

/***************************************************************
	函数名:     RepaintVideo
	函数描述:   视频窗口重绘函数
	参数1：     prect ， 输出，视频窗口位置指针
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR RepaintVideo(CRect *prect = NULL) = 0;

/***************************************************************
		函数名:     GetStreamInfo
		函数描述:   音频字幕流信息查看接口
		参数1：     S_StreamInfo *pStreamInfo， 输出，流信息描述结构体
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

//	virtual QVOD_ERROR GetStreamInfo(S_StreamInfo *pStreamInfo) = 0;

/***************************************************************
		函数名:     SetSubTitle
		函数描述:   设置字幕
		参数1：     int iIndex， 输入，要设置的字幕流索引号
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetSubTitle(int iIndex) = 0;

/***************************************************************
		函数名:     SetAudioChannel
		函数描述:   设置音频流
		参数1：     int iIndex， 输入，要设置的音频流索引号
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetAudioChannel(int iIndex) = 0;

/***************************************************************
		函数名:     SetPlayFrame
		函数描述:   设置播放帧数
		参数1：     long lFrameNums， 输入，取值1，-1,1前进一帧，-1后退一帧
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetPlayFrame(long lFrameNums, bool bsync = false) = 0;

/***************************************************************
		函数名:     QueryInterface
		函数描述:   设置播放帧数
		参数1：     REFIID riid， 输入，接口IID，可供查询的有IID_IColortrans,IID_ISoundtrans,IID_ISubtrans三个接口
		参数2:      void **ppvIF, 输出，接口指针
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR __stdcall QueryInterface(REFIID riid, void **ppvIF) = 0;

/***************************************************************
	函数名:     GetAdjustRect
	函数描述:   获取视频窗口调整值接口
	参数1：     long *lLeft， 输出，left调整值
	参数2:      long *lTop, 输出，top调整值
	参数3：     long *lWidth， 输出，width调整值
	参数4:      long *lHeight，输出，Height 调整值
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR GetAdjustRect(long *lLeft, long *lTop, long *lWidth, long *lHeight) = 0;

/***************************************************************
		函数名:     SetAdjustRect
		函数描述:   设置视频窗口调整值接口
		参数1：     long lLeft， 输入，left调整值
		参数2:      long lTop, 输入，top调整值
		参数3：     long lWidth， 输入，width调整值
		参数4:      long lHeight，输入，Height 调整值
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR SetAdjustRect(long  lLeft, long  lTop, long  lWidht, long lHeight) = 0;

/***************************************************************
		函数名:     Move
		函数描述:   调整视频窗口
		参数1：     int x， 输入，x轴方向调整值
		参数2:      int y, 输入，y轴方向调整值
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR Move(int x, int y ) = 0;

/***************************************************************
		函数名:     Zoom
		函数描述:   缩放视频窗口
		参数1：     int iParmer， 输入，取值1，-1， 1 为放大一圈，-1为缩小一圈（只能做一次该操作）
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

    virtual QVOD_ERROR Zoom(int iParmer) = 0;

/***************************************************************
	函数名:     Scale
	函数描述:   视频窗口自适应调整接口
	参数1：     int x， 输入，单位像素
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR Scale(int x) = 0;

/***************************************************************
	函数名:     Scale
	函数描述:   视频窗口还原为初始状态
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR Restore() = 0;
	//90度翻转
	//virtual QVOD_ERROR rotate(E_FlipType fliptype) = 0;//x= 1, -1
/***************************************************************
	函数名:     flip
	函数描述:   视频图像翻转
	参数1：     E_FlipType eFliptype， 输入，翻转类型
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	//virtual QVOD_ERROR flip(E_FlipType eFliptype) = 0;

/***************************************************************
	函数名:     DestopPlay
	函数描述:   桌面播放
	参数1：     bool btype， 输入，是否桌面播放
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR DestopPlay(bool bDestopplay) = 0;

/***************************************************************
	函数名:     GetLastDestopPlay
	函数描述:   获取上一次桌面播放
	参数1：     bool& bfalg， 输入，输出，桌面播放是否未结束
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR GetLastDestopPlay(bool& bflag) = 0;

/***************************************************************
	函数名:     SetDestopRecover
	函数描述:   恢复桌面背景
	参数1：     bool btype， 输入，是否恢复桌面背景
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR SetDestopRecover(bool bflag) = 0;

/***************************************************************
	函数名		AllocateColokey
	函数描述:	分配色键
	参数1：		DWORD dwColorkey， 输入，设置播放窗口的色键
	返回值：	无
*****************************************************************/
	virtual void AllocateColokey(DWORD dwColorkey) = 0;

/***************************************************************
函数名:     SetAddMediaFilter
函数描述:   音视频调节设置接口
参数1：     bool isadd， 输入，是否添加音视频调节接口
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR DynamicAddVideoPostFilter(bool bAddsub = true, bool bAddPostVideo = true) = 0;
/***************************************************************
函数名:     SetAddMediaFilter
函数描述:   音视频调节设置接口
参数1：     bool isadd， 输入，是否添加音视频调节接口
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	//virtual QVOD_ERROR GetAddMediaFilter(bool* pisadd) = 0;

/***************************************************************
函数名:     SetSourceVideo
函数描述:   视频放大接口
参数1：     RECT *prect， 输入，视频放大的位置
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR SetVideoZoom(POINT pt, float zoomparam) = 0;

/***************************************************************
函数名:     SetZoomMove
函数描述:   视频放大接口
参数1：     RECT *prect， 输入，视频放大的位置
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR SetZoomMove(int x, int y) = 0;

/***************************************************************
函数名:     ProductVersion
函数描述:   简易版设置
参数1：     long version 简易版为0，增强版为1，默认为增强版。
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR ProductVersion(long version) = 0;

/***************************************************************
函数名:     GetMediaType
函数描述:   获取文件媒体类型
参数1：     E_MEDIA_TYPE& mt 返回值，媒体类型音频、视频，其他。
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR GetCurrentMediaType(E_MEDIA_TYPE& mt) = 0;

/***************************************************************
函数名:     GetCurrentavgFrameNum
函数描述:   获取文件媒体帧率
参数1：
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR GetCurrentavgFramerate(int* pavgframerate) = 0;

/***************************************************************
函数名:     GetCurrenVideoCoderType
函数描述:   获取媒体文件视频编码类型
参数1：     e_V_ENCODER* pvideocodectype 输出，视频编码类型。
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR GetCurrenVideoCodecType(e_V_ENCODER *pvideocodectype) = 0;

/***************************************************************
函数名:     ShallVideoEnhanceOpen
函数描述:   是否应该打开画质增强
返回值：    打开返回true，否则返回false
*****************************************************************/
	virtual BOOL ShallVideoEnhanceOpen() = 0;

/***************************************************************
函数名:     SetBufferingTime                                
函数描述:   设置播放缓冲数据长度
参数1：     int nPreBufferTime, 播放前预缓冲数据的播放时长（单位：秒，最大长度为20s）
参数1：     int nPlayBufferTime, 播放中缓冲数据的播放时长（单位：秒，最大长度为20s）
返回值：    成功返回S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR SetBufferingTime(int nPreBufferTime, int nPlayBufferTime) = 0;

/***************************************************************
函数名:     GetBufferingTime                                
函数描述:   获取播放缓冲数据长度
参数1：     int nPreBufferTime, 播放前预缓冲数据的播放时长（单位：秒，最大长度为20s）
参数1：     int nPlayBufferTime, 播放中缓冲数据的播放时长（单位：秒，最大长度为20s）
返回值：    无
*****************************************************************/
	virtual void GetBufferingTime(int* pnPreBufferTime, int* pnPlayBufferTime) = 0;


/***************************************************************
函数名:     Detect3dVideo                                
函数描述:   检测d3d视频排列性
返回值：    无
*****************************************************************/
	virtual void Detect3DVideo() = 0;
/***************************************************************
函数名:     GetCrashLog                               
函数描述:   获取崩溃时播放引擎模块的运行日志
参数1：     BYTE** ppbyte, 输出，Log buffer指针
参数2：     int& len,	   输出，Logbuffer大小
返回值：    无
*****************************************************************/
	virtual void GetCrashLog(BYTE** ppbyte, int& len) = 0;
//DVD 播放控制接口

/***************************************************************
函数名:     DvdSelectAtPosition
函数描述:   获取DVD鼠标移动消息
参数1：     POINT pt， 输入，鼠标的当前位置
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

virtual QVOD_ERROR DvdSelectAtPosition(POINT pt) = 0;

/***************************************************************
函数名:     DvdActivateAtPosition
函数描述:   获取DVD鼠标点击消息
参数1：     POINT pt， 输入，鼠标的当前位置
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

virtual QVOD_ERROR DvdActivateAtPosition(POINT pt) = 0;
	//获取DVD的标题数
/***************************************************************
		函数名:     DvdGetTitles
		函数描述:   获取DVD标题数和当前播放标题
		参数1：     ULONG *pTitleNum = NULL， 输出，标题数
		参数2：     ULONG *pCurTitle = NULL，输出，当前播放标题
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR	DvdGetTitles(ULONG *pTitleNum = NULL, int *pCurTitle = NULL) = 0;

/***************************************************************
		函数名:     DvdSetCurtitle
		函数描述:   设置DVD当前播放标题
		参数1：     ULONG uCurtitle， 输入，标题有效值为（1-99）
		参数2：     BOOL bEvent，输入，如果是系统标题改变消息，此标志为TRUE，否则为FALSE
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	//设置标题(有效值为1-99)
	virtual QVOD_ERROR DvdSetCurtitle(int uCurtitle, BOOL bEvent) = 0;

/***************************************************************
	函数名:     DvdGetChapters
	函数描述:   获取DVD指定标题的章节数，及当前播放章节
	参数1：     ULONG uTitle， 输入，标题，有效值为（1-999）
	参数2：     ULONG *pChapterNum = NULL，输出，章节数
	参数3：     ULONG *pCurChapter = NULL，输出，当前章节数
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR	DvdGetChapters(ULONG uTitle,  ULONG *pChapterNum = NULL, int *pCurChapter = NULL) = 0;

/***************************************************************
		函数名:     DvdSetCurChapter
		函数描述:   播放指定DVD章节
		参数1：     ULONG nCurchapters， 输入，标题，有效值为（1-999）
		参数2：     BOOL bEvent，输入，如果是系统标题改变消息，此标志为TRUE，否则为FALSE
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR DvdSetCurChapter(int nCurchapters, BOOL bEvent) = 0;
	//获取视角
/***************************************************************
		函数名:     DvdGetAngle
		函数描述:   获取DVD视角
		参数1：     ULONG* pAngNum = NULL， 输出，视角，有效值为（1-9）
		参数2：     ULONG* pCurAngle = NULL，输出，当前视角
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual ULONG DvdGetAngle(ULONG* pAngNum = NULL, int* pCurAngle = NULL) = 0;
	//设置视角(有效值为1-9)
/***************************************************************
		函数名:     DvdSetAngle
		函数描述:   设置DVD视角
		参数1：     ULONG uCurangle， 输出，视角，有效值为（1-9）
		参数2：     BOOL bEvent，输入，如果是系统标题改变消息，此标志为TRUE，否则为FALSE
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR DvdSetAngle(int uCurangle, BOOL bEvent) = 0;

/***************************************************************
		函数名:     DvdGetSubTitle
		函数描述:   获取DVD字幕信息
		参数1：     S_DVDINFO* pSubTitle， 输出，字幕流信息结构体
		返回值：    成功返回E_QVOD_S_OK，否则为失败
***************************************************************/

	virtual QVOD_ERROR DvdGetSubTitle(S_DVDINFO* pSubTitle) = 0;

/***************************************************************
		函数名:     DvdSetSubTitle
		函数描述:   设置DVD字幕信息
		参数1：     int iCurWords， 输入，字幕流索引(有效值为-1-31， 注：-1为关闭字幕，0-31为设置字幕流)
		参数2：     BOOL bEvent，输入，如果是系统标题改变消息，此标志为TRUE，否则为FALSE
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR DvdSetSubTitle(int iCurWords, BOOL bEvent) = 0;
	//获取音频信息
/***************************************************************
		函数名:     DvdGetAudios
		函数描述:   获取DVD音频信息
		参数1：     S_DVDINFO* pAudio， 输出，音频信息结构体
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR DvdGetAudios(S_DVDINFO* pAudio) = 0;
	//设置音频(有效值为0-7)

/***************************************************************
		函数名:     DvdSetAudio
		函数描述:   获取DVD音频信息
		参数1：     ULONG uCuraudio， 输入，音频流索引(有效值为0-7)
		参数2：     BOOL bEvent， 输入，如果是系统标题改变消息，此标志为TRUE，否则为FALSE
		返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR	DvdSetAudio(int uCuraudio, BOOL bEvent) = 0;

/***************************************************************
函数名:     PlayPrevChapter
函数描述:   播放上一章节
返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR PlayPrevChapter() = 0;

/***************************************************************
	函数名:     PlayNextChapter
	函数描述:   播放下一章节
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/

	virtual QVOD_ERROR PlayNextChapter() = 0;

/***************************************************************
	函数名:     DvdSetMenu
	函数描述:   设置播放菜单
	参数1：     DVD_MENU_ID eMenu， 输入，DVD_MENU_ID菜单
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR DvdSetMenu(DVD_MENU_ID eMenu) = 0;
#ifdef VIDEOWALL
/***************************************************************
	函数名:     ResetMonitor                                
	函数描述:   重置监视器
	返回值：    成功返回E_QVOD_S_OK，否则为失败
*****************************************************************/
	virtual QVOD_ERROR ResetMonitor() = 0;

/***************************************************************
	函数名:     IsVideoWallAvailable                                
	函数描述:   视频墙是否可用
	返回值：    可用返回true，否则为不可用
*****************************************************************/
	virtual bool IsVideoWallAvailable() = 0;

/***************************************************************
	函数名:     SetVideoWallMode                                
	函数描述:   设置进入退出视频墙模式标识
	返回值：    true为进入，false为退出
*****************************************************************/
	virtual QVOD_ERROR SetVideoWallMode(bool flag) = 0;

/***************************************************************
	函数名:     GetVideoWallMode                                
	函数描述:   获取进入退出视频墙模式标识
	返回值：    true为进入，false为退出
*****************************************************************/
	virtual QVOD_ERROR GetVideoWallMode(bool* flag) = 0;

#endif
};

#endif