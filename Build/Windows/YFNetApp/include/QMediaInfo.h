#pragma  once
#include "windows.h"
#include <wchar.h>
struct IAsyncReader;
#define NEW_MEDIA_INFO_INTERFACE
#ifdef DLL_MEIDAINFO
#else
#define DLL_MEIDAINFO  extern"C" _declspec(dllimport)
#endif



#define  MAX_MIINFO_SIZE 100
#define  MAX_VIDEO_COUNT 8
#define  MAX_AUDIO_COUNT 8
#define  MAX_SUBTITLE_COUNT 8

struct Qvod_MainMediaInfo
{
	int nContainerCount;
	int nVideoCount;
	int nAudioCount;
	int nSubtitleCount;
	wchar_t container[MAX_MIINFO_SIZE];
	wchar_t videoCodec[MAX_VIDEO_COUNT][MAX_MIINFO_SIZE];
	wchar_t audioCodec[MAX_AUDIO_COUNT][MAX_MIINFO_SIZE];
	wchar_t subtitleName[MAX_SUBTITLE_COUNT][MAX_MIINFO_SIZE];

	Qvod_MainMediaInfo()
	{
		memset(this,0,sizeof(Qvod_MainMediaInfo));
	}
};

struct VideoStreamInfo
{
	int  nID;
	wchar_t Codec[MAX_MIINFO_SIZE];
	int nBitrate;		//bps
	float nFrameRate;		//
#ifdef NEW_MEDIA_INFO_INTERFACE
	int nRefFrames;
#endif
	int nWidth;
	int nHeigth;
	double fAspectRatioX;
	double fAspectRatioY;

	VideoStreamInfo()
	{
		memset(this,0,sizeof(VideoStreamInfo));
	}
};

struct AudioStreamInfo
{
	int  nID;
	wchar_t Codec[MAX_MIINFO_SIZE];
	int nBitrate;//	   码率  kbps
	int nChannel;//    声道数
	int nSamplingRate;//采样率 khz
	int nBitDepth;//位数  bit
#ifdef NEW_MEDIA_INFO_INTERFACE
	wchar_t language[MAX_MIINFO_SIZE];//语言
#endif
	AudioStreamInfo()
	{
		memset(this,0,sizeof(AudioStreamInfo));
	}
};

struct SubtitleStreamInfo
{
	int  nID;
	wchar_t Codec[MAX_MIINFO_SIZE];//字幕编码
	wchar_t language[MAX_MIINFO_SIZE];//语言

	SubtitleStreamInfo()
	{
		memset(this,0,sizeof(SubtitleStreamInfo));
	}
};

struct Qvod_DetailMediaInfo
{
	int nContainerCount;
	int nVideoCount;
	int nAudioCount;
	int nSubtitleCount;
#ifdef NEW_MEDIA_INFO_INTERFACE
	__int64  nDuration;//播放时长 (ms)
#else
	int  nDuration;//播放时长 (s)
#endif
	int  nOverAllBitrate;//总码率 kbps
	__int64   fileSize;
	wchar_t container[MAX_MIINFO_SIZE];
	
	VideoStreamInfo*  videoInfo[MAX_VIDEO_COUNT];//指针数组
	AudioStreamInfo*  audioInfo[MAX_AUDIO_COUNT];
	SubtitleStreamInfo*  subtitleInfo[MAX_SUBTITLE_COUNT];

public:
	void init()
	{
		nContainerCount = 0;
		nVideoCount= 0;
		nAudioCount = 0;
		nSubtitleCount = 0;
		nDuration = 0;//播放时长 (s)
		nOverAllBitrate = 0;//总码率 kbps
		fileSize = 0;
		memset(container, 0, sizeof(wchar_t) * MAX_MIINFO_SIZE);

		for (int i = 0; i <MAX_VIDEO_COUNT ; ++i)
		{
			if(videoInfo[i])
			{
				memset(videoInfo[i], 0, sizeof(VideoStreamInfo));
			}
		}
		for (int i = 0; i <MAX_AUDIO_COUNT ; ++i)
		{
			memset(audioInfo[i], 0, sizeof(AudioStreamInfo));//audioInfo[i] = new AudioStreamInfo;
		}
		for (int i = 0; i <MAX_SUBTITLE_COUNT ; ++i)
		{
			memset(subtitleInfo[i], 0, sizeof(SubtitleStreamInfo));//subtitleInfo[i] = new SubtitleStreamInfo;
		}
	}
	Qvod_DetailMediaInfo()
	{
		memset(this,0,sizeof(Qvod_DetailMediaInfo));
		for (int i = 0; i <MAX_VIDEO_COUNT ; ++i)
		{
			videoInfo[i] = new VideoStreamInfo;
		}
		for (int i = 0; i <MAX_AUDIO_COUNT ; ++i)
		{
			audioInfo[i] = new AudioStreamInfo;
		}
		for (int i = 0; i <MAX_SUBTITLE_COUNT ; ++i)
		{
			subtitleInfo[i] = new SubtitleStreamInfo;
		}
		init();
	}

	~Qvod_DetailMediaInfo()
	{
		for (int i = 0; i <MAX_VIDEO_COUNT ; ++i)
		{
			if(videoInfo[i])
			{
				delete videoInfo[i];
				videoInfo[i] = NULL;
			}
		}
		for (int i = 0; i <MAX_AUDIO_COUNT ; ++i)
		{
			if(audioInfo[i])
			{
				delete audioInfo[i];
				audioInfo[i] = NULL;
			}
		}
		for (int i = 0; i <MAX_SUBTITLE_COUNT ; ++i)
		{
			if(subtitleInfo[i])
			{
				delete subtitleInfo[i];
				subtitleInfo[i] = NULL;
			}
		}
	}

	Qvod_DetailMediaInfo& operator = (const Qvod_DetailMediaInfo& c)
	{
		if (this == &c)
		{
			return *this;
		}

		this->fileSize = c.fileSize;
		this->nAudioCount = c.nAudioCount;
		this->nContainerCount = c.nContainerCount;
		this->nDuration = c.nDuration;
		this->nOverAllBitrate = c.nOverAllBitrate;
		this->nSubtitleCount = c.nSubtitleCount;
		this->nVideoCount = c.nVideoCount;
		//wmemcpy_s(this->container,MAX_MIINFO_SIZE,c.container,MAX_MIINFO_SIZE);
		wcscpy(this->container, c.container);
		
		for (int i = 0; i <MAX_VIDEO_COUNT ; ++i)
		{
			/*if(videoInfo[i])
			{
				delete videoInfo[i];
			}
			videoInfo[i] = new VideoStreamInfo;*/
			*videoInfo[i] = *c.videoInfo[i];
		}
		for (int i = 0; i <MAX_AUDIO_COUNT ; ++i)
		{
			/*if(audioInfo[i])
			{
				delete audioInfo[i];
			}
			audioInfo[i] =new AudioStreamInfo;*/
			*audioInfo[i] = *c.audioInfo[i];
		}
		for (int i = 0; i <MAX_SUBTITLE_COUNT ; ++i)
		{
			/*if(subtitleInfo[i])
			{
				delete subtitleInfo[i];
			}
			subtitleInfo[i] =new SubtitleStreamInfo;*/
			*subtitleInfo[i] = *c.subtitleInfo[i];
		}
		return * this;
	}
	Qvod_DetailMediaInfo(const Qvod_DetailMediaInfo & c)
	{
		memset(this,0,sizeof(Qvod_DetailMediaInfo));
		for (int i = 0; i <MAX_VIDEO_COUNT ; ++i)
		{
			videoInfo[i] = new VideoStreamInfo;
		}
		for (int i = 0; i <MAX_AUDIO_COUNT ; ++i)
		{
			audioInfo[i] = new AudioStreamInfo;
		}
		for (int i = 0; i <MAX_SUBTITLE_COUNT ; ++i)
		{
			subtitleInfo[i] = new SubtitleStreamInfo;
		}
		init();
		*this = c;
	}
private:
	
};  

//struct LabelItem
//{
//	size_t  size;
//	char text[MAX_PATH * 2];
//};
//struct Qvod_Label
//{
//	LabelItem title;
//	LabelItem artist;
//	LabelItem album;
//public:
//	Qvod_Label()
//	{
//		memset(this,0,sizeof(Qvod_Label));
//	}
//};
struct  s_FileInfo
{
	__int64		lFileSize;          //媒体文件大小
#ifdef NEW_MEDIA_INFO_INTERFACE
	__int64		lDuration;          //播放时长		  ms
#else
	long		lDuration;
#endif
	long		lWidth;             //图像宽
	long		lHeight;            //图像高
	long		lRate;              //码率 bps

	long        nTitleSize;         //标题长度
	long        nAuthorSize;        //艺术家长度
	long        nAlbumSize;         //专辑长度
	char		szTitle[MAX_PATH];  //标题
	char		szAuthor[MAX_PATH]; //艺术家
	char		szAlbum[MAX_PATH];  //专辑
};

//读取本地文件的主要媒体信息
//wchar_t* strURL     文件路径
DLL_MEIDAINFO BOOL GetFileMediaInfo(wchar_t* strURL,Qvod_MainMediaInfo *pMainInfo);

//IAsyncReader  *pReader  读文件接口
DLL_MEIDAINFO BOOL GetStreamMediaInfo(IAsyncReader *pReader,Qvod_MainMediaInfo *pMainInfo);


//读取本地文件的详细媒体信息
//wchar_t* strURL     文件路径
DLL_MEIDAINFO BOOL GetFileDetailMediaInfo(wchar_t* strURL,Qvod_DetailMediaInfo *pDetailInfo);

//读取本地文件的标签
//wchar_t* strURL     文件路径
DLL_MEIDAINFO BOOL GetFileLabel(wchar_t* strURL,s_FileInfo *pLabelInfo);

typedef BOOL (*GETFILEINFO)(wchar_t* strURL,Qvod_MainMediaInfo *pMainInfo);
typedef BOOL (*GETSTREAMINFO)(IAsyncReader *pReader,Qvod_MainMediaInfo *pMainInfo);
typedef BOOL (*GETFILEDETAILINFO)(wchar_t *pReader,Qvod_DetailMediaInfo *pDetailInfo);
typedef BOOL (*GETFILELABEL)(wchar_t* strURL,s_FileInfo *pLabelInfo);
