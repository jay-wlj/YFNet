#ifndef __M3U8_H__
#define __M3U8_H__

#include <vector>
#include <map>
#include <string>
#include <set>
using namespace std;

#include "qvod_basetypes.h"
#include "qvod_lock.h"
#include "common-commonstruct.h"
#include "AESData.h"

struct SM3u8TsSurplusData
{
	char* buf;
	QVOD_UINT64 start;
	unsigned int len;
	SM3u8TsSurplusData()
	{
		buf = NULL;
		start = 0;
		len = 0;
	}
};
class CM3u8
{
public:
	CM3u8(const char *pFilename,const char* pTsPath);
	~CM3u8();
	//bool Init(const char *buf, int len);
	bool Init(std::string &strContent, bool bLoad = true);
	bool IsExist();
	bool IsFinish(bool bCheckFileExist = false);
	bool Load();
	bool Save(const char *buf, int len);
	bool Remove();
	void SetM3u8Cdn(string strUrl){m_strM3u8Cdn = strUrl;}
	void SetM3u8Orgcdn(string strUrl){m_strM3u8Orgcdn = strUrl;}
	void SetM3u8PreUrl(string strPreUrl) {m_strM3u8PreUrl = strPreUrl;}
	int GetTsNum(){return m_tsNameVec.size();}
	int SetBitField(unsigned int index);
	bool CheckIndex(unsigned int index);
	int WriteTs(unsigned int index,const char* buf,int len);
	bool CheckTsSign(unsigned int index,const char* buf,int len);
	QVOD_INT64 GetGlobalOffset(unsigned int index);
	QVOD_INT64 GetAllTsSize();
	QVOD_INT64 GetTotalDownloadSize(){return m_iTotalDownloadSize;}
	string GetUnhaveTsFromIndex(unsigned int& index);
	string GetTsName(const unsigned int& index);
	void SetPlayingTs(string strTsName);						// 开始推流
	void ResetPlayingTs(const string& strTsName);	// 推流完毕
	int GetPushIndex();														// 获取当前推流索引
	bool IsPlayTsContinue();
	bool IsPlayTsContinueIn2Ts();
	void ClearPlayingTsVec();
	int GetTsIndex(string strTsName);
	int GetTsIndexByFileName(string strTsFileName);
	int GetTsUrlIndex(const string& strTsUrl);
	QVOD_INT64 GetM3u8Size();
	QVOD_INT64 GetTsSize(string strTsName);
	QVOD_INT64 GetTsOffset(const unsigned int& index);
	QVOD_INT64 GetTsSize(const unsigned int& index);
	int FindTsByOffset(QVOD_INT64 offset);
	int ReadM3u8(char* buf,int start,int len);
	int ReadTs(string strTsName,char* buf,int start,int len);
	int GetTsDuration(){return m_iTsDuration;}
	unsigned int GetDuration();		// 获取视频总时长 单位:ms
	unsigned int GetTsCount();		// 获取ts总数
	unsigned int GetPrevTsDuration(const unsigned int& nIndex);		// 获取索引nIndex及之前的时长总和, ms
	int GetTsIntervalDuration(const unsigned int& nBeginIndex, const unsigned int& nEndIndex);	// 获取ts索引之间的时长
	int GetTsIndexByDuration(const unsigned int& nDuration);	//	通过时间点获取ts片索引位置
	int GetContinueTsNum(const unsigned int& nIndex);
	unsigned int GetContinueTsTimeByIndex(const unsigned int& nIndex);			// 获取当前位置后缓存的时长，ms
	unsigned int GetContinueTsTime(const unsigned int& timepoint);					// 获取当前时间点后的缓存时长，ms
	void SetPlayingTimepoint(const unsigned int& timepoint,bool bSeek);
	void Clear();
	void SaveM3u8Data(SM3u8TsSurplusData& data);
	void ClearM3u8TsData();
	void RemoveM3u8TsData(const QVOD_INT64& iStart);													// 删除iStart位置的ts部分数据
	bool FindM3u8TsData(const QVOD_INT64& iStart, SM3u8TsSurplusData& data);	// 查找iStart位置的ts部分数据
	int ReadM3u8TsData(const QVOD_INT64& iStart, char *buf, const unsigned int& len);	// 读取保存的ts部分数据	

	int ClearTsData(unsigned int nBeginIndex, unsigned int nEndIndex);							// 删除ts片区
	int ClearHasPlayingTsData(QVOD_UINT64 iCurCacheSize);			// 删除当前播放位置前的ts片，返回删除的大小
	void ClearHasPlayingTsData(int nIndex);											// 删除除nIndex位置缓存iCacheTime时间外的数据
	void UpdateM3u8OrgUrl(const string& strOrgUrl, bool bDownload = false, bool bPlaying=false, bool bChangeClarity=false);						// 更新m3u8的url 并重新下载
	void SetAesKey(unsigned char key[16]);											// 设置存储key

	void ClearReseveTs();
protected:
	static string SignUrl(const std::string& strHttpUrl, const string& strAccessKeyID, const string& strAccessSecret, const string& strExpires);
private:
	QVOD_INT64 GetFileSize(const string& strFilePath);

	// 无缝切换
private:
	void CopyOldMapFiles(vector<string> &vFileNames);
public:
	HASH				m_hash;
	string				m_strM3u8Name;			//m3u8文件名
	string				m_strTsPath;						//ts路径
	string				m_strTsMapPath;				// ts映射路径
	string				m_strM3u8Url;					// m3u8 url
	string				m_strM3u8Cdn;				//￥”urltracker?°μΩμ?m3u8 cdn￡¨μ?“a”≈???π”√
	string				m_strM3u8Orgcdn;			//￥”urltracker?°μΩμ?m3u8 cdn￡¨μ???”≈???π”√
	string				m_strM3u8PreUrl;			// m3u8 url上一级路径
	string				m_strM3u8FileContent;	// m3u8 内容
	CAESData		m_aesdata;						// aes加解密
	bool				m_bPushM3u8;					// 当前是否正在推m3u8
	int					m_iPushTsIndex;				// 当前推流的ts索引 -2:未推流 -1:推m3u8文件 0-n:推ts索引
	int					m_iPlayTsIndex;				// 当前播放推流的ts片索引(该索引会大于当前播放位置点对应的ts片索引)
	int					m_iPlayPreTsIndex;			// 当前播放推流的前一个ts片的索引
	unsigned int	m_iCurPlayTime;				// 当前播放位置点(由上层定时1s传入)，ms
	int					m_iCurPlayIndex;				// 当前播放位置点的ts索引(由上层定时1s传入)
	bool				m_bSpaceMode;				// 是否节省空间
	bool				m_bCacheTimeCalBySetTime;	// 缓存时长计算通过传进来的位置计算
	int					m_iTsDuration;					// ts片的平均时长 单位:秒
	int					m_iTsAverageRate;			// ts片平均码率bps
	int					m_nReseveMaxIndex;		// 保留的最大ts片
private:
	char*									m_pBitField;				// 下载信息区块
	int										m_iTotalDownloadDuration;		// ms
	QVOD_INT64						m_iTotalDownloadSize;
	vector<QVOD_INT64>	m_offsetVec;				// ts片对应的文件位置
	vector<unsigned int>		m_tsSizeVec;				// ts片对应的文件大小
	vector<string>					m_tsNameVec;					//ts片的url
	vector<string>					m_tsHashVec;			// ts片对应的数据md5
	vector<string>					m_tsFileNameVec;			//保存在本地的文件名
	vector<unsigned int>		m_tsDurationVec;		//ts->duration  ms
	vector<unsigned int>		m_tsOffsetDur;				// ts->dration	ts片对应的当前播放位置时间点 ms
	vector<int>						m_playTsIndexVec;			//记录最近3个播放的ts，根据是否连续判断是否发生了seek
	vector<SM3u8TsSurplusData> m_m3u8TsData;		//HLSP2PDOWNTASK–￥??o??±a·”–≤–?ù≤a???èμ?
	
    map<string,int>				m_tsNameIndexMap;          // ts片"/"后的文件名 不包含后面的参数

	map<string,int>				m_tsNameIndexMapOld;          // ts片"/"后的文件名 不包含后面的参数, 推流时的ts片映射关系
	map<string,int>				m_tsFileNameIndexMapOld;			// 切换清晰度时保存没删的几片ts文件名索引映射关系

	CLock									m_lock;
	CLock									m_m3u8TsDataLock;

	std::set<int>						m_setExistIndex;				// 加载m3u8时已存在的ts片
	
	bool									m_bNeedSignTs;				// ts片需要签名
	std::string							m_strAccessKeyID;			// 访问ID
	std::string							m_strAcessKeySecret;	// 访问密码

private:
	bool									m_bReadM3u8;				// 是否推m3u8流
};
#endif
