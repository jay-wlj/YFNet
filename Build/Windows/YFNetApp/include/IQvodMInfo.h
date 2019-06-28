#ifndef _IQVOD_MINFO_H__
#define _IQVOD_MINFO_H__

typedef struct MediaInfo
{
	wchar_t strfilename[MAX_PATH*2];	//文件全路径
	wchar_t strauthor[MAX_PATH];		//作者
	wchar_t strspecial[MAX_PATH];		//专辑
	__int64 	nsize;					//文件大小 字节
	__int64  ndur;						//文件播放时长-豪秒级
	long    nwidth;						//视频宽，如音频为0
	long    nheight;					//视频高，如音频为0
	long    nbits;						//文件bits; 都以 nsize*8/(ndur/1000)
	long		nvideocoder;			//视频编码器ID
	long		naudiocoder;			//音频编码器ID
	//关于音/视频编码器ID需自定义，
	//如 H264 = 1,AVC1=2,MPG-3, 
	//REALVIDEO=4,REALAUDIO=5类似
}MediaInfo_s;


interface IQvodMInfo
{
	BOOL LoadFile(wchar_t *pstrfilename);
	BOOL GetMediaInfo(MediaInfo_s *pMediaInfo);
	void Clear();
};


#endif