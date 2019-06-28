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
//�����ļ������� DS SWF
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
//ý���ļ�����
enum e_CONTAINER
{
	_E_CON_UNKNOW = 0,

	_E_CON_RMVB = 1,
	_E_CON_AVI,
	_E_CON_MKV,
	_E_CON_MP4,		//MPEG-4 Part 14,3GPϵ
	_E_CON_3GP,		//3GP

	_E_CON_MOV,		//Appleϵ ����MOV��QT
	//_E_CON_QT,

	_E_CON_MPEG,	//MPEG1/2/4 ��׼������ʽ
	_E_CON_DAT,
	_E_CON_MPEG4ES,	//MPEG4������
	_E_CON_MPEGTS,	//TS
	_E_CON_MPEGPS,	//VOB EVO HD-DVD
	_E_CON_HKMPG,	//Hikvision mp4
	_E_CON_DIVX,	//Divx Media Format(DMF)
	_E_CON_WM,		//ASF/WMV/WMA/WMP
	_E_CON_TS,		//TS
	_E_CON_M2TS,	//M2TS/MTS,֧��DBAV �� AVCHD
	_E_CON_DVD,		//DVD�̣����⴦��DVD������

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

	_E_CON_XLMV,//Ѹ�׸�ʽ

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

//��Ƶ��������
enum e_V_ENCODER
{
	/*MPEG ϵ��*/
	_V_MPEG_1,
	_V_MPEG_2,

	_V_MPEG_4_Visual,//Simple
	_V_MPEG_4_Advanced_Simple,//Advanced Simple

	_V_XVID,
	_V_DIVX,
	//_V_AVC,//AVC1/H264/X264

	/*ITU ϵ��*/
	_V_H261,
	_V_H263,
	_V_H263_PLUS,
	_V_H264,

	/*Real Mediaϵ��*/
	_V_RV10,
	_V_RV20,
	_V_RV30,
	_V_RV40,

	/*Windows ϵ��*/
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

	/*����*/
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
	_V_OTHER	= 0x100	//δ֪
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
	_A_PCM,				//����������
	_A_DPCM,			//�������������
	_A_APCM,			//����Ӧ����������
	_A_ADPCM,			//����Ӧ�������������
	_A_PCM_LE,			//PCM Little Endian
	_A_PCM_BE,			//PCM Big	 Endian

	_A_COOK,			//rmvb AUDIO
	_A_SIPR,			//rmvb AUDIO
	_A_TSCC,			//tscc

	_A_QCELP,			//Qualcomm��˾��1993������˿ɱ����ʵ�CELP������ΪQCELP,QCELP ����ѹ�������㷨����8K��13K����

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
	_A_OTHER   = 0x100	//δ֪
};

//����ѡ��
enum e_Play_Option
{
	E_INTELLIGENCE, //����ģʽ
	E_DXVA_ACCEL,   //��������
	E_VIDEO_POST    //��������
};

struct PlayOption_Param
{
	DWORD std_min_height;//������С�߶ȣ����أ���Ĭ��ֵ480p
	DWORD std_max_height;//�������߶ȣ����أ���Ĭ��ֵ720p
	DWORD std_min_bitrate;//������С���ʣ�bps����Ĭ��ֵ450000bps
	DWORD low_min_bitrate;//������С���ʣ�bps����Ĭ��ֵ300000bps
};
//�����ļ���Э��
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
//�ļ�����״̬
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

//ȫ��ģʽ
enum e_VFULLFRAME
{
	_VFF_DEFAULT,
	_VFF_FILM
};

//��ͼģʽ
enum e_CAPTUREMODE
{
	_E_CM_CURRENT,
	_E_CM_GENNER,
	_E_CM_FILTER,
	_E_CM_NULL
};
//��Ƶ���
enum e_Viewframe
{
	_VF_SF = 0,
	_VF_Fill,
	_VF_43,
	_VF_54,
	_VF_169,
	_VF_FILM
};
//����
enum e_SoundChannel
{
    _SC_DEFAULT = 0,	//Ĭ��
	_SC_LEFT,			//������
	_SC_RIGHT,			//������
	_SC_MONO,			//�����
	_SC_LTOALL,			//��������չ��ȫ��
	_SC_RTOALL			//��������չ��ȫ��
};

#define	 D_VOLUME	80

//������ת
enum E_FlipType
{
	LeftFlip = 1,         //��ת90��
	RightFlip,            //��ת90��
    HorizonalFlip,        //ˮƽ��ת
	VerticalFlip,         //��ֱ��ת
	NoFlip                //����ת
};

//¼����
enum E_RVQuality
{
	E_RQ_Low,          //��
	E_RQ_Medium,       //��
	E_RQ_Hight         //��
};

//ý������
enum E_MEDIA_TYPE
{
	E_MT_VIDEO,  //��Ƶ
	E_MT_AUDIO,  //��Ƶ
	E_MT_VIDEO_ONLY,//������Ƶ
	E_MT_CSF,
	E_MT_SWF,
	E_MT_UNKNOWN
};

////�ļ���Ϣ
//typedef struct  s_FileInfo
//{
//	__int64		lFileSize;          //ý���ļ���С(��λ���ֽ�)
//	long		lDuration;          //����ʱ��(��λ������)
//	long		lWidth;             //ͼ���(��λ������)
//	long		lHeight;            //ͼ���
//	long		lRate;              //����(��λ��kbps)
//	long		lIndex;
//	char		szHash[41];         //��ϣֵ
//	e_V_ENCODER eVEncoder;          //��Ƶ��������
//	e_A_ENCODER eAEncoder;          //��Ƶ������
//	long        nTitleSize;         //���ⳤ��
//	long        nAuthorSize;        //�����ҳ���
//	long        nAlbumSize;         //ר������
//	char		szTitle[MAX_PATH];  //����
//	char		szAuthor[MAX_PATH]; //������
//	char		szAlbum[MAX_PATH];  //ר��
//	wchar_t		szfilepath[MAX_PATH];
//} s_FileInfo;

enum E_InfoType  //��Ϣ����
{
	E_ST_SubTitle,                 //��Ļ
	E_ST_Video,                    //��Ƶ��
	E_ST_Audio,                    //��Ƶ��
	E_ST_FilterNAME,               //Filter����
	E_ST_Unknown                   //δ֪
};

struct S_NotifyInfo               //ý�岥��ģ��ص���Ϣ�ṹ��
{
	WCHAR Name[128];              //����(��Ϣ����)
	E_InfoType type;              //��Ϣ����
	int Index;                    //��Ϣ�������������ø�����Ч��
	int StreamTip;                //��ؽӿ��±꣬�ڲ�ʹ��
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

struct DVDTITLE//DVD������Ϣ
{
    S_DVDINFO DVDAudio;       //��Ƶ����Ϣ
	S_DVDINFO DVDSubTitle;    //��Ļ��Ϣ
	ULONG Anglenum;           //�ӽ���
	int curAngle;           //��ǰ�ӽ�
	ULONG Chapternum;         //�½���
	int curChapter;         //��ǰ�����½�
	ULONG Titlenum;           //������
	int curTitle;           //��ǰ����
	DVDTITLE():Anglenum(0),curAngle(-1),Chapternum(0),
		       curChapter(-1),Titlenum(0),curTitle(-1){}
};
struct S_StreamInfo
{
	int SubTitleNum;          //��Ļ��
	int AudioNum;             //��Ƶ��
	int CurAudio;             //��ǰ��Ƶ��
	int CurSubTitle;          //��ǰ��Ļ
	int StreamSelectNum;      //��ǰIAMStreamSelect�Ľӿ���
	S_NotifyInfo SubTitleinfo[10]; //��Ļ��Ϣ
	S_NotifyInfo AudioInfo[10];    //��Ƶ��Ϣ
	interface IAMStreamSelect *pStreamSelect[10]; //IAMStreamSelect�ӿ�
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
	ULONG numtitles;									//������
	ULONG numchapters;									//�½���
}DVDTITLES;

typedef struct _dvdstruct
{
	ULONG numtitles;									//������
	ULONG numchapters;									//�½���
	ULONG numangles;									//�ӽ���
	ULONG numaudios;									//����������
	ULONG numwordlangs;									//��Ļ��
	//���������е�����
	//LCID SSS
	//LCID SSS
}DVDSTRUCT;

typedef struct _dvd_title_change
{
	ULONG curtitle;										//��ǰ����
	ULONG numchapters;									//�½���
	ULONG numangles;									//�ӽ���
	ULONG numaudios;									//����������
	ULONG numwordlangs;									//��Ļ��
	//���������е�����
	//LCID SSS
	//LCID SSS
}DVD_TITLE_CHANGE;

extern GUID IID_IQvodPlayMedia;// = {0xce0e9322, 0x3559, 0x417f, 0xa9, 0xba, 0xae, 0xfe, 0x73, 0xb1, 0xbf, 0x75};

typedef void (*_pfnUserFilter) (void *filter, void *pparam );

interface IQvodPlayMedia : public IQvodCallBack
{
/*********************************************************************
        ������:     PlaySource
        ��������:   ����ý��
        ����1:      wchar_t *pFileUrl ���룬ý���ļ����ء�����·��
		����2:      E_MEDIA_TYPE mt = E_MT_VIDEO������ý�岥�����ͣ�Ĭ��Ϊ����Ƶ
		����3:      LONG lStart = 0�����룬��ʼ����ʱ�䣬��λΪ����
		����ֵ��    �ɹ�����E_QVOD_S_OK,����Ϊʧ��
 *********************************************************************/
	virtual QVOD_ERROR PlaySource(wchar_t *pFileUrl, E_MEDIA_TYPE mt = E_MT_VIDEO, LONGLONG lStart = 0) = 0;

/*********************************************************************
        ������:     PlayAd
        ��������:   ���Ź��
        ����1:      wchar_t *pAdUrl = NULL ���룬flash����
                    ����·������ΪNULL��Ϊ��˾Ĭ�ϵ���վ���
		����ֵ��    �ɹ�����E_QVOD_S_OK,����Ϊ����
**********************************************************************/

	     virtual QVOD_ERROR PlayAd(wchar_t *pAdUrl, bool bfixsize, RECT *prect) = 0;

/*********************************************************************
         ������:     StopAd
         ��������:   ֹͣ��沥�ţ����ñ����������ڲ���
		             ����flash��滹����վ��棬����ֹͣ
         ����ֵ��    �ɹ�����E_QVOD_S_OK,����Ѿ�ֹͣ����E_QVOD_S_FALSE������Ϊʧ��
 *********************************************************************/

	      virtual QVOD_ERROR StopAd() = 0;

/*********************************************************************
          ������:     SetPauseAdEnable
          ��������:   �����Ƿ����������ļ�����ͣ���
          ����1:      bool bAdEnable ���룬TRUEΪ���ã�����Ϊ������
**********************************************************************/

	      virtual void SetPauseAdEnable(bool bAdEnable) = 0;

/*********************************************************
          ������:     Play
          ��������:   ������ֹͣ����ͣ�Ĳ�������
          ����ֵ��    �ɹ�����E_QVOD_S_OK,ý���Ѿ��ڲ��ŷ���E_QVOD_S_FALSE������Ϊʧ��
**********************************************************/

	      virtual QVOD_ERROR Play() = 0;

/**********************************************************
          ������:     Stop
          ��������:   ֹͣ���ڲ��Ż���ͣ�Ĳ�������
		  ����ֵ��    �ɹ�����E_QVOD_S_OK,ý���Ѿ�ֹͣ����E_QVOD_S_FALSE������Ϊʧ��
***********************************************************/

	   virtual QVOD_ERROR Stop() = 0;

/**********************************************************
		������:     Pause
		��������:   ��ͣ���ڲ��ŵĲ�������
		����ֵ��    �ɹ�����E_QVOD_S_OK,ý���Ѿ���ͣ��ֹͣ����E_QVOD_S_FALSE������Ϊʧ��
***********************************************************/

	virtual QVOD_ERROR Pause() = 0;

/**********************************************************
		������:     GetPlayState
		��������:   ��ȡ��ǰý�岥��ģ��Ĳ���״̬
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
***********************************************************/

	virtual QVOD_ERROR GetPlayState(e_PlayState *pPlayState) = 0;

/**********************************************************
		������:     GetCurrentPos
		��������:   ��ȡ��ǰý�岥�Ž��ȵ�ʱ���
		����1:      long *pValue�� �����ý���ļ���ǰ���ŵ�ʱ���,��λΪ����
		����ֵ��    �ɹ�����E_QVOD_S_OK����ǰ���ڲ���״̬����E_QVOD_S_FALSE������Ϊʧ��
************************************************************/

	virtual	QVOD_ERROR GetCurrentPos(REFERENCE_TIME *pValue) = 0;

/**********************************************************
		������:     SetCurrentPos
		��������:   ���õ�ǰý�岥�ŵ�ʱ���
		����1:      long  lValue�� ���룬Ҫ���õ���ý���ļ�����ʱ���,��λΪ����
		����ֵ��    �ɹ�����E_QVOD_S_OK����ǰ���ڲ���״̬����E_QVOD_S_FALSE������Ϊʧ��
************************************************************/

	virtual QVOD_ERROR SetCurrentPos(REFERENCE_TIME  lValue) = 0;

/**********************************************************
������:     CurrentMediaCanSeek
��������:   ��ǰý���Ƿ����ק
����ֵ��    ����ק����true������Ϊfalse
************************************************************/

	virtual bool CurrentMediaCanSeek() = 0;

/**********************************************************
������:     GetMediaFileType
��������:   ��ȡָ���ļ����ļ�����
����1:      wchar_t *pFileUrl �ļ�·��
����ֵ��    ָ���ļ����ļ�����
************************************************************/
	virtual e_CONTAINER GetMediaFileType(wchar_t *pFileUrl) = 0;
	//e_CONTAINER GetMediaFileType(wchar_t *pFileUrl);
/**********************************************************
		������:     GetDuration
		��������:   ��ȡ��ǰý���ļ��Ĳ���ʱ��
		����1:      long *pValue�� �����ý���ļ��Ĳ���ʱ��,��λΪ����
		����ֵ��    �ɹ�����E_QVOD_S_OK����ǰ���ڲ���״̬����E_QVOD_S_FALSE������Ϊʧ��
************************************************************/
	virtual QVOD_ERROR GetDuration(REFERENCE_TIME *pValue) = 0;

//ȫ���ӿڣ�������Ŀǰδʹ�ã�
	//virtual QVOD_ERROR GetFull(bool *pValue) = 0;
	//virtual QVOD_ERROR SetFull(bool bValue)  = 0;

/*************************************************************
		������:     GetMute
		��������:   ��ȡ�������ľ�����ʶ
		����1:      long *pValue�� ����������þ���Ϊtrue������Ϊfalse
		����ֵ��    �ɹ�����E_QVOD_S_OK������ָ��ΪNULL����E_QVOD_S_FALSE
***************************************************************/

	virtual QVOD_ERROR GetMute(bool *pValue) = 0;

/***************************************************************
		������:     SetMute
		��������:   ���ò������ľ�����ʶ
		����1:      bool bValue�� ���룬������ʶ�����þ���Ϊtrue������Ϊfalse
		����ֵ��    �ɹ�����E_QVOD_S_OK�������÷���E_QVOD_S_FALSE������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetMute(bool bValue) = 0;

/***************************************************************
		������:     GetVolume
		��������:   ��ȡ������������
		����1:      long *pValue�� ����� ��ǰ������������0 - 100����������
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetVolume(long *pValue) = 0;

/***************************************************************
		������:     SetVolume
		��������:   ���ò�����������
		����1:      bool lValue�� ���룬0-100����������
		����ֵ��    �ɹ�����E_QVOD_S_OK������ָ��ΪNULL����E_QVOD_S_FALSE
*****************************************************************/

	virtual QVOD_ERROR SetVolume(long lValue) = 0;

/***************************************************************
		������:     SetPlaySpeed
		��������:   ���ò������Ĳ�������
		����1:      long lSpeed�����룬1������0.1������-1������0.1������
		����ֵ��    �ɹ�����E_QVOD_S_OK�������÷���E_QVOD_S_FALSE������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetPlaySpeed(double lSpeed) = 0;

/***************************************************************
		������:     GetPlaySpeed
		��������:   ���ò������Ĳ�������
		����1:      long *plSpeed�������ȡֵ��ΧΪ0.11-3.0�� ����1�����ڼ��ٲ��ţ�С��1�����ڼ��ٲ��ţ�����1��������
*****************************************************************/

	virtual QVOD_ERROR GetPlaySpeed(double *plSpeed) = 0;

/***************************************************************
������:     GetVideoFrameNum
��������:   ��ȡ������Ƶ��Ⱦ����֡��
����1:      int *pframenum�������������Ƶ��Ⱦ����֡��
*****************************************************************/
	virtual QVOD_ERROR GetVideoFrameNum(int *pframenum) = 0;

/***************************************************************
		������:     GetViewFrame
		��������:   ��ȡ�������Ĳ�������
		����1:      e_Viewframe *pViewFrame��������Ƶ��ʾ���ģʽ
*****************************************************************/

	virtual	QVOD_ERROR GetViewFrame(e_Viewframe *pviewframe, e_VFULLFRAME *pFFrame) = 0;

/***************************************************************
		������:     SetViewFrame
		��������:   ������Ƶ�����ʾģʽ
		����1:      e_Viewframe eViewFrame�����룬��Ƶ��ʾ���ģʽ
		����ֵ��    �ɹ�����E_QVOD_S_OK�������÷���E_QVOD_S_FALSE������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetViewFrame(e_Viewframe eViewframe, e_VFULLFRAME eFFrame) = 0;

/***************************************************************
		������:     GetSoundChannel
		��������:   ��ȡ��Ƶ����ģʽ
		����1:      e_SoundChannel *pSoundChannel���������ǰ��Ƶ����ģʽ
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetSoundChannel(e_SoundChannel *pSoundChannel) = 0;

/***************************************************************
		������:     SetSoundChannel
		��������:   ������Ƶ����ģʽ
		����1:      e_SoundChannel eSoundChannel�����룬��Ƶ����ģʽ
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetSoundChannel(e_SoundChannel eSoundChannel) = 0;

/***************************************************************
	������:     SetVideoRender
	��������:   ����VideoRender����
	����1:      e_VideoRender eVideoRender�����룬VideoRender����
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetVideoRender(e_VideoRender eVideoRender) = 0;

/***************************************************************
		������:     GetVideoRender
		��������:   ��ȡVideoRender����
		����1:      e_VideoRender *pVideoRender���������ǰVideoRender����
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetVideoRender(e_VideoRender *pVideoRender) = 0;

/***************************************************************
		������:     GetAudioRenderCount
		��������:   ��ȡAudioRender��
		����1:      long *pAudioCount�������AudioRender��
		����2��     long *pBufSize���������ʹ�õ�buffer��С
		����3��     wchar_t *pAudioText�����롢�������ƵRender����
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetAudioRenderCount(long *plAudioCount, long *plBufSize, wchar_t *pAudioText) = 0;

/***************************************************************
		������:     GetCurAudioRender
		��������:   ��ȡ��ǰʹ�õ�Audiorender
		����1:      long *pAudioRender���������ǰʹ�õ�Audiorender
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetCurAudioRender(long *plAudioRender) = 0;

/***************************************************************
		������:     SetCurAudioRender
		��������:   ����AudioRender
		����1:      long lAudiooRender�����룬Audiorender��������������õ��ģ�
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetCurAudioRender(long lAudiooRender) = 0;

/***************************************************************
		������:     GetClipSize
		��������:  ��ȡͼ��Ŀ��
		����1:      long *pWidht���������Ƶ��
		����2��     long *pHeight���������Ƶ��
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetClipSize(long *pWidht, long *pHeight) = 0;

	/***************************************************************
	������:     GetCurrentMediaType
	��������:  ��ȡý������Ƶ����
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
	*****************************************************************/

	//virtual QVOD_ERROR GetCurrentMediaType(long *pWidht, long *pHeight) = 0;
/***************************************************************
		������:     GetRate
		��������:  ��ȡý�岥�ŵ�����
		����1:      long *pRate�������ý�岥�ŵ�����
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetRate(long *pRate) = 0;

/***************************************************************
		������:     GetCurrentImage
		��������:  ��ȡ��ǰ��Ƶͼ��
		����1:      PBYTE *ppbuf�������BITMAPINFOHEADER+λͼ����
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetCurrentImage(PBYTE *ppbuf, e_CAPTUREMODE eCaptrueMode = _E_CM_CURRENT, REFERENCE_TIME* psampletime = NULL) = 0;

/***************************************************************
		������:     GetBufferPer
		��������:  ��ȡ�����ļ�����ٷֱ�
		����1:      long *pBufferPer������������ļ�����ٷֱȣ�����ʱ��Ϊ10s��
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetBufferPer(long *pBufferPer) = 0;

//filter order
#ifdef  _WIN32
/***************************************************************
		������:     EnumFilters
		��������:   ö��Graph�е�Filter
		����1:      _pfnUserFilter fnUserFileter�������ö�ٳ�Filter��Ĳ�������
		����2:      void *pVoid�� ָ�뺯���Ĳ���
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR EnumFilters(_pfnUserFilter fnUserFileter, void *pVoid) = 0;

/***************************************************************
	������:     ShowFilter
	��������:   ��ʾָ��Filter������ҳ
	����1:      wchar_t *pFilterName�����룬Filter����
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR ShowFilter(wchar_t *pFilterName) = 0;
#endif
/***************************************************************
	������:     GetFileInfo
	��������:   ��ȡý���ļ���ý����Ϣ
	����1:      wchar_t *pFileUrl�����룬�ļ�·��
	����2:      s_FileInfo	*ps_FileInfo��������ļ���Ϣ�ṹ��
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetFileInfo(wchar_t *pFileUrl, struct s_FileInfo	*ps_FileInfo, E_MEDIA_TYPE& mt) = 0;

/***************************************************************
������:     GetMediaInfo
��������:   ��ȡý���ļ���ý����Ϣ(ʹ��mediainfo��)
����1:      wchar_t *pFileUrl�����룬�ļ�·��
����2:      Qvod_MainMediaInfo	*ps_FileInfo��������ļ���Ϣ�ṹ��
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

    virtual QVOD_ERROR GetMediaInfo(wchar_t *pFileUrl, struct Qvod_DetailMediaInfo	*ps_MediaInfo) = 0;

/***************************************************************
		������:     GetFileType
		��������:   ��ȡ��ǰ����ý���ļ����ļ�����
		����1:      e_FileType   *pFileType��������ļ�·��
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetFileType(e_FileType   *pFileType) = 0;

/***************************************************************
	������:     GetFileProtocol
	��������:   ��ȡ��ǰ����ý���ļ����ļ�Э��
	����1:     e_FileProtocol *pFileProtocol��������ļ�Э��
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetFileProtocol(e_FileProtocol *pFileProtocol) = 0;

/***************************************************************
	������:     GetFileDownloadLen
	��������:   ��ȡ�����ļ��ĵ�ǰ���س���
	����1:      INT64& iDownLoadLen����������ص�ǰ���س���
	����ֵ��    �ɹ�����E_QVOD_S_OK����ǰ���ŵĲ��������ļ�
	            ����E_QVOD_S_FALSE������Ϊʧ��
*****************************************************************/

	//virtual QVOD_ERROR GetFileDownloadLen(INT64& iDownLoadLen) = 0;

/***************************************************************
	������:     SetBufferTime
	��������:   ���������ļ�����ʱ�Ļ���ʱ��
	����1:      int nTime�����룬����ʱ��
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	//virtual QVOD_ERROR SetBufferTime(int nTime) = 0;
	 //QVOD_ERROR SetBufferTime(int nTime);

/***************************************************************
	������:     SetParentWnd
	��������:   ������Ƶ���Ŵ��ڵĸ����ھ��
	����1:      long lParentWnd�����룬�����ھ��
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetParentWnd(long lParentWnd) = 0;		//HWND

/***************************************************************
		������:     SetQvodNet
		��������:   ����QvodNet�ӿ�ָ��
		����1:      void **pIQvodNet�����룬QvodNet�ӿ�ָ��
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetQvodNet(void **pIQvodNet) = 0;

/***************************************************************
������:     SetQlive
��������:   ����Qlive�ӿ�ָ��
����1:      void **pIQlive�����룬Qlive�ӿ�ָ��
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	//virtual QVOD_ERROR SetQlive(void **pIQlive) = 0;

/***************************************************************
������:     IsOpenDxva
��������:   �Ƿ��Ӳ������
����1:      bool isopen�����룬�Ƿ�����Ӳ�����٣�tureΪ������falseΪ������
����ֵ��    ��
*****************************************************************/

	//virtual void IsOpenDxva(bool isopen) = 0;

/***************************************************************
������:     SetPlayOption
��������:   ���ò���ѡ���
��Ӳ�����١�������Ƶ���ڡ�������Ӳ�����ٺ���Ƶ���ڣ�
����1:      e_Play_Option eOption�����룬����Ӳ�����١�������Ƶ����������Ӳ�����ٺ���Ƶ����
����ֵ��    ��
*****************************************************************/
     virtual HRESULT SetPlayOption(e_Play_Option eOption, PlayOption_Param* pParam = NULL) = 0;

/***************************************************************
������:     GetPlayOption
��������:   ��ȡ����ѡ������
����1:      e_Play_Option eOption�����룬���������Ӳ�����١�������Ƶ����������Ӳ�����ٺ���Ƶ����
����ֵ��    ��
*****************************************************************/
	virtual void GetPlayOption(e_Play_Option &eOption, PlayOption_Param* pParam = NULL) = 0;

/***************************************************************
������:     RecordVideo
��������:   ¼��asf��Ƶ
����1:      wchar_t *pPath¼����Ƶ�ı���·��
����2��     int quality ¼����Ƶ������ �ɷ�Ϊ���е����֣�
����3��     E_MEDIA_TYPE mt ý�����ͣ���Ϊ��Ƶ����Ƶ��������Ƶ���֣�
����4��     long rtStrat ¼�ƿ�ʼʱ��(��λΪ��ms)
����5��     long rtStop  ¼�ƽ���ʱ��(��λΪ��ms)
����6��     long lwidth ¼��ͼ���
����6��     long lheight ¼��ͼ���
����ֵ��    �ɹ�����S_OK������Ϊʧ��
remark:     ���ӿڶ�����Ƶ�ṩ���ֲ�ͬ��ý������¼�ƣ���Ƶ¼��ʱ������Ϊ8Kbps��������Ϊ32Kbps��������Ϊ44Kbps
            ��Ƶ¼��ʱ������Ϊ185Kbps��������Ϊ386Kbps��������Ϊ1272Kbps
*****************************************************************/

	//virtual	QVOD_ERROR RecordVideo( wchar_t *pPath,
	//								E_RVQuality quality,
	//								E_MEDIA_TYPE mt,
	//								long rtStrat,
	//								long rtStop,
	//								long lwidth = 0,
	//								long lheight = 0) = 0;

/***************************************************************
������:     StopRecord
��������:   ֹͣ¼����Ƶ
����1:      bool ContinuePlayֹͣ���Ƿ�������ţ�trueΪ������falseΪ���������ţ�
����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/

	//virtual	QVOD_ERROR StopRecord(bool bContinuePlay = true) = 0;

/***************************************************************
������:     SetVideoEnhance
��������:   ��Ƶ��ǿ
����1:      bool IsEnhance��Ƶ��ǿ��trueΪ��ǿ��falseΪ����ǿ��
����ֵ��    ��
*****************************************************************/
	//virtual QVOD_ERROR SetVideoEnhance(bool IsEnhance) = 0;

/***************************************************************
������:     GetVideoEnhance
��������:   ��Ƶ��ǿ
����1:      bool IsEnhance��Ƶ��ǿ��trueΪ��ǿ��falseΪ����ǿ��
����ֵ��    ��
*****************************************************************/
	//virtual QVOD_ERROR GetVideoEnhance(bool &IsEnhance) = 0;

/***************************************************************
	������:     SetCallBack
	��������:   ������������Ϣ�ص�����
	����1:      void **pIQvodNet�����룬QvodNet�ӿ�ָ��
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual	QVOD_ERROR SetCallBack(IQvodCallBack *pQvodCallBack) = 0;

/***************************************************************
	������:     MoveWindow
	��������:   ��Ƶ���ڵ�������
	����1:      void **pIQvodNet�����룬QvodNet�ӿ�ָ��
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	//virtual	QVOD_ERROR MoveWindow(CRect *prect = NULL) = 0;

/***************************************************************
	������:     RepaintVideo
	��������:   ��Ƶ�����ػ溯��
	����1��     prect �� �������Ƶ����λ��ָ��
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR RepaintVideo(CRect *prect = NULL) = 0;

/***************************************************************
		������:     GetStreamInfo
		��������:   ��Ƶ��Ļ����Ϣ�鿴�ӿ�
		����1��     S_StreamInfo *pStreamInfo�� ���������Ϣ�����ṹ��
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

//	virtual QVOD_ERROR GetStreamInfo(S_StreamInfo *pStreamInfo) = 0;

/***************************************************************
		������:     SetSubTitle
		��������:   ������Ļ
		����1��     int iIndex�� ���룬Ҫ���õ���Ļ��������
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetSubTitle(int iIndex) = 0;

/***************************************************************
		������:     SetAudioChannel
		��������:   ������Ƶ��
		����1��     int iIndex�� ���룬Ҫ���õ���Ƶ��������
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetAudioChannel(int iIndex) = 0;

/***************************************************************
		������:     SetPlayFrame
		��������:   ���ò���֡��
		����1��     long lFrameNums�� ���룬ȡֵ1��-1,1ǰ��һ֡��-1����һ֡
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetPlayFrame(long lFrameNums, bool bsync = false) = 0;

/***************************************************************
		������:     QueryInterface
		��������:   ���ò���֡��
		����1��     REFIID riid�� ���룬�ӿ�IID���ɹ���ѯ����IID_IColortrans,IID_ISoundtrans,IID_ISubtrans�����ӿ�
		����2:      void **ppvIF, ������ӿ�ָ��
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR __stdcall QueryInterface(REFIID riid, void **ppvIF) = 0;

/***************************************************************
	������:     GetAdjustRect
	��������:   ��ȡ��Ƶ���ڵ���ֵ�ӿ�
	����1��     long *lLeft�� �����left����ֵ
	����2:      long *lTop, �����top����ֵ
	����3��     long *lWidth�� �����width����ֵ
	����4:      long *lHeight�������Height ����ֵ
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR GetAdjustRect(long *lLeft, long *lTop, long *lWidth, long *lHeight) = 0;

/***************************************************************
		������:     SetAdjustRect
		��������:   ������Ƶ���ڵ���ֵ�ӿ�
		����1��     long lLeft�� ���룬left����ֵ
		����2:      long lTop, ���룬top����ֵ
		����3��     long lWidth�� ���룬width����ֵ
		����4:      long lHeight�����룬Height ����ֵ
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR SetAdjustRect(long  lLeft, long  lTop, long  lWidht, long lHeight) = 0;

/***************************************************************
		������:     Move
		��������:   ������Ƶ����
		����1��     int x�� ���룬x�᷽�����ֵ
		����2:      int y, ���룬y�᷽�����ֵ
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR Move(int x, int y ) = 0;

/***************************************************************
		������:     Zoom
		��������:   ������Ƶ����
		����1��     int iParmer�� ���룬ȡֵ1��-1�� 1 Ϊ�Ŵ�һȦ��-1Ϊ��СһȦ��ֻ����һ�θò�����
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

    virtual QVOD_ERROR Zoom(int iParmer) = 0;

/***************************************************************
	������:     Scale
	��������:   ��Ƶ��������Ӧ�����ӿ�
	����1��     int x�� ���룬��λ����
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR Scale(int x) = 0;

/***************************************************************
	������:     Scale
	��������:   ��Ƶ���ڻ�ԭΪ��ʼ״̬
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR Restore() = 0;
	//90�ȷ�ת
	//virtual QVOD_ERROR rotate(E_FlipType fliptype) = 0;//x= 1, -1
/***************************************************************
	������:     flip
	��������:   ��Ƶͼ��ת
	����1��     E_FlipType eFliptype�� ���룬��ת����
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	//virtual QVOD_ERROR flip(E_FlipType eFliptype) = 0;

/***************************************************************
	������:     DestopPlay
	��������:   ���沥��
	����1��     bool btype�� ���룬�Ƿ����沥��
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR DestopPlay(bool bDestopplay) = 0;

/***************************************************************
	������:     GetLastDestopPlay
	��������:   ��ȡ��һ�����沥��
	����1��     bool& bfalg�� ���룬��������沥���Ƿ�δ����
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR GetLastDestopPlay(bool& bflag) = 0;

/***************************************************************
	������:     SetDestopRecover
	��������:   �ָ����汳��
	����1��     bool btype�� ���룬�Ƿ�ָ����汳��
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR SetDestopRecover(bool bflag) = 0;

/***************************************************************
	������		AllocateColokey
	��������:	����ɫ��
	����1��		DWORD dwColorkey�� ���룬���ò��Ŵ��ڵ�ɫ��
	����ֵ��	��
*****************************************************************/
	virtual void AllocateColokey(DWORD dwColorkey) = 0;

/***************************************************************
������:     SetAddMediaFilter
��������:   ����Ƶ�������ýӿ�
����1��     bool isadd�� ���룬�Ƿ��������Ƶ���ڽӿ�
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR DynamicAddVideoPostFilter(bool bAddsub = true, bool bAddPostVideo = true) = 0;
/***************************************************************
������:     SetAddMediaFilter
��������:   ����Ƶ�������ýӿ�
����1��     bool isadd�� ���룬�Ƿ��������Ƶ���ڽӿ�
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	//virtual QVOD_ERROR GetAddMediaFilter(bool* pisadd) = 0;

/***************************************************************
������:     SetSourceVideo
��������:   ��Ƶ�Ŵ�ӿ�
����1��     RECT *prect�� ���룬��Ƶ�Ŵ��λ��
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR SetVideoZoom(POINT pt, float zoomparam) = 0;

/***************************************************************
������:     SetZoomMove
��������:   ��Ƶ�Ŵ�ӿ�
����1��     RECT *prect�� ���룬��Ƶ�Ŵ��λ��
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR SetZoomMove(int x, int y) = 0;

/***************************************************************
������:     ProductVersion
��������:   ���װ�����
����1��     long version ���װ�Ϊ0����ǿ��Ϊ1��Ĭ��Ϊ��ǿ�档
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR ProductVersion(long version) = 0;

/***************************************************************
������:     GetMediaType
��������:   ��ȡ�ļ�ý������
����1��     E_MEDIA_TYPE& mt ����ֵ��ý��������Ƶ����Ƶ��������
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR GetCurrentMediaType(E_MEDIA_TYPE& mt) = 0;

/***************************************************************
������:     GetCurrentavgFrameNum
��������:   ��ȡ�ļ�ý��֡��
����1��
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR GetCurrentavgFramerate(int* pavgframerate) = 0;

/***************************************************************
������:     GetCurrenVideoCoderType
��������:   ��ȡý���ļ���Ƶ��������
����1��     e_V_ENCODER* pvideocodectype �������Ƶ�������͡�
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR GetCurrenVideoCodecType(e_V_ENCODER *pvideocodectype) = 0;

/***************************************************************
������:     ShallVideoEnhanceOpen
��������:   �Ƿ�Ӧ�ô򿪻�����ǿ
����ֵ��    �򿪷���true�����򷵻�false
*****************************************************************/
	virtual BOOL ShallVideoEnhanceOpen() = 0;

/***************************************************************
������:     SetBufferingTime                                
��������:   ���ò��Ż������ݳ���
����1��     int nPreBufferTime, ����ǰԤ�������ݵĲ���ʱ������λ���룬��󳤶�Ϊ20s��
����1��     int nPlayBufferTime, �����л������ݵĲ���ʱ������λ���룬��󳤶�Ϊ20s��
����ֵ��    �ɹ�����S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR SetBufferingTime(int nPreBufferTime, int nPlayBufferTime) = 0;

/***************************************************************
������:     GetBufferingTime                                
��������:   ��ȡ���Ż������ݳ���
����1��     int nPreBufferTime, ����ǰԤ�������ݵĲ���ʱ������λ���룬��󳤶�Ϊ20s��
����1��     int nPlayBufferTime, �����л������ݵĲ���ʱ������λ���룬��󳤶�Ϊ20s��
����ֵ��    ��
*****************************************************************/
	virtual void GetBufferingTime(int* pnPreBufferTime, int* pnPlayBufferTime) = 0;


/***************************************************************
������:     Detect3dVideo                                
��������:   ���d3d��Ƶ������
����ֵ��    ��
*****************************************************************/
	virtual void Detect3DVideo() = 0;
/***************************************************************
������:     GetCrashLog                               
��������:   ��ȡ����ʱ��������ģ���������־
����1��     BYTE** ppbyte, �����Log bufferָ��
����2��     int& len,	   �����Logbuffer��С
����ֵ��    ��
*****************************************************************/
	virtual void GetCrashLog(BYTE** ppbyte, int& len) = 0;
//DVD ���ſ��ƽӿ�

/***************************************************************
������:     DvdSelectAtPosition
��������:   ��ȡDVD����ƶ���Ϣ
����1��     POINT pt�� ���룬���ĵ�ǰλ��
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

virtual QVOD_ERROR DvdSelectAtPosition(POINT pt) = 0;

/***************************************************************
������:     DvdActivateAtPosition
��������:   ��ȡDVD�������Ϣ
����1��     POINT pt�� ���룬���ĵ�ǰλ��
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

virtual QVOD_ERROR DvdActivateAtPosition(POINT pt) = 0;
	//��ȡDVD�ı�����
/***************************************************************
		������:     DvdGetTitles
		��������:   ��ȡDVD�������͵�ǰ���ű���
		����1��     ULONG *pTitleNum = NULL�� �����������
		����2��     ULONG *pCurTitle = NULL���������ǰ���ű���
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR	DvdGetTitles(ULONG *pTitleNum = NULL, int *pCurTitle = NULL) = 0;

/***************************************************************
		������:     DvdSetCurtitle
		��������:   ����DVD��ǰ���ű���
		����1��     ULONG uCurtitle�� ���룬������ЧֵΪ��1-99��
		����2��     BOOL bEvent�����룬�����ϵͳ����ı���Ϣ���˱�־ΪTRUE������ΪFALSE
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	//���ñ���(��ЧֵΪ1-99)
	virtual QVOD_ERROR DvdSetCurtitle(int uCurtitle, BOOL bEvent) = 0;

/***************************************************************
	������:     DvdGetChapters
	��������:   ��ȡDVDָ��������½���������ǰ�����½�
	����1��     ULONG uTitle�� ���룬���⣬��ЧֵΪ��1-999��
	����2��     ULONG *pChapterNum = NULL��������½���
	����3��     ULONG *pCurChapter = NULL���������ǰ�½���
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR	DvdGetChapters(ULONG uTitle,  ULONG *pChapterNum = NULL, int *pCurChapter = NULL) = 0;

/***************************************************************
		������:     DvdSetCurChapter
		��������:   ����ָ��DVD�½�
		����1��     ULONG nCurchapters�� ���룬���⣬��ЧֵΪ��1-999��
		����2��     BOOL bEvent�����룬�����ϵͳ����ı���Ϣ���˱�־ΪTRUE������ΪFALSE
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR DvdSetCurChapter(int nCurchapters, BOOL bEvent) = 0;
	//��ȡ�ӽ�
/***************************************************************
		������:     DvdGetAngle
		��������:   ��ȡDVD�ӽ�
		����1��     ULONG* pAngNum = NULL�� ������ӽǣ���ЧֵΪ��1-9��
		����2��     ULONG* pCurAngle = NULL���������ǰ�ӽ�
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual ULONG DvdGetAngle(ULONG* pAngNum = NULL, int* pCurAngle = NULL) = 0;
	//�����ӽ�(��ЧֵΪ1-9)
/***************************************************************
		������:     DvdSetAngle
		��������:   ����DVD�ӽ�
		����1��     ULONG uCurangle�� ������ӽǣ���ЧֵΪ��1-9��
		����2��     BOOL bEvent�����룬�����ϵͳ����ı���Ϣ���˱�־ΪTRUE������ΪFALSE
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR DvdSetAngle(int uCurangle, BOOL bEvent) = 0;

/***************************************************************
		������:     DvdGetSubTitle
		��������:   ��ȡDVD��Ļ��Ϣ
		����1��     S_DVDINFO* pSubTitle�� �������Ļ����Ϣ�ṹ��
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
***************************************************************/

	virtual QVOD_ERROR DvdGetSubTitle(S_DVDINFO* pSubTitle) = 0;

/***************************************************************
		������:     DvdSetSubTitle
		��������:   ����DVD��Ļ��Ϣ
		����1��     int iCurWords�� ���룬��Ļ������(��ЧֵΪ-1-31�� ע��-1Ϊ�ر���Ļ��0-31Ϊ������Ļ��)
		����2��     BOOL bEvent�����룬�����ϵͳ����ı���Ϣ���˱�־ΪTRUE������ΪFALSE
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR DvdSetSubTitle(int iCurWords, BOOL bEvent) = 0;
	//��ȡ��Ƶ��Ϣ
/***************************************************************
		������:     DvdGetAudios
		��������:   ��ȡDVD��Ƶ��Ϣ
		����1��     S_DVDINFO* pAudio�� �������Ƶ��Ϣ�ṹ��
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR DvdGetAudios(S_DVDINFO* pAudio) = 0;
	//������Ƶ(��ЧֵΪ0-7)

/***************************************************************
		������:     DvdSetAudio
		��������:   ��ȡDVD��Ƶ��Ϣ
		����1��     ULONG uCuraudio�� ���룬��Ƶ������(��ЧֵΪ0-7)
		����2��     BOOL bEvent�� ���룬�����ϵͳ����ı���Ϣ���˱�־ΪTRUE������ΪFALSE
		����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR	DvdSetAudio(int uCuraudio, BOOL bEvent) = 0;

/***************************************************************
������:     PlayPrevChapter
��������:   ������һ�½�
����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR PlayPrevChapter() = 0;

/***************************************************************
	������:     PlayNextChapter
	��������:   ������һ�½�
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/

	virtual QVOD_ERROR PlayNextChapter() = 0;

/***************************************************************
	������:     DvdSetMenu
	��������:   ���ò��Ų˵�
	����1��     DVD_MENU_ID eMenu�� ���룬DVD_MENU_ID�˵�
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR DvdSetMenu(DVD_MENU_ID eMenu) = 0;
#ifdef VIDEOWALL
/***************************************************************
	������:     ResetMonitor                                
	��������:   ���ü�����
	����ֵ��    �ɹ�����E_QVOD_S_OK������Ϊʧ��
*****************************************************************/
	virtual QVOD_ERROR ResetMonitor() = 0;

/***************************************************************
	������:     IsVideoWallAvailable                                
	��������:   ��Ƶǽ�Ƿ����
	����ֵ��    ���÷���true������Ϊ������
*****************************************************************/
	virtual bool IsVideoWallAvailable() = 0;

/***************************************************************
	������:     SetVideoWallMode                                
	��������:   ���ý����˳���Ƶǽģʽ��ʶ
	����ֵ��    trueΪ���룬falseΪ�˳�
*****************************************************************/
	virtual QVOD_ERROR SetVideoWallMode(bool flag) = 0;

/***************************************************************
	������:     GetVideoWallMode                                
	��������:   ��ȡ�����˳���Ƶǽģʽ��ʶ
	����ֵ��    trueΪ���룬falseΪ�˳�
*****************************************************************/
	virtual QVOD_ERROR GetVideoWallMode(bool* flag) = 0;

#endif
};

#endif