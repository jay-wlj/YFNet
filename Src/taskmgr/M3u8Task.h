#pragma once
#include "downloadtask.h"
#include "M3u8.h"
#include "AESData.h"
#include <set>
using std::set;

class CM3u8Task :
	public CDownLoadTask
{
public:
	CM3u8Task( const unsigned char* hash,std::string strSaveFile,int type,string& strHttpOrgUrl,string& strHttpKeyUrl,string& strHttpReferer, TaskCfg *cfg = NULL);
	virtual ~CM3u8Task(void);

public:
	virtual int					run(bool bLoad);
	virtual bool				TaskIsFinish(bool bCheckFileExist = false);
	virtual int					SetPlaying(bool bPlaying);
	virtual unsigned int		GetPlayFileRate();
	virtual QVOD_INT64	GetDownloadedSize();
	virtual QVOD_INT64	GetTotalDownloadLen();
	virtual void					SetM3u8PlayingPos(const string& strTsName,int pos);
	virtual bool					IsPlayTsContinue();
	virtual QVOD_INT64	GetM3u8TsSize(const string& strName);
	 virtual bool					IsExistM3u8Ts(string strTsUrl);
	 virtual void					ClosePush(const string& strFilename);
	 virtual int						DownNextM3u8Ts(SM3u8TsReq& req, const int& iTsIndex = -1);
	 virtual int						ReadM3u8Ts(const string& strFilename,char* buf,int start,int len,bool bIsUpload);
	 virtual int						WriteM3u8Ts(const string& strFilename,const char* buf,int start,int len,int filesize);
	 virtual void					NotifyDownM3u8Ts(const string& strFilename);
	 virtual int						ReadData(char *buf, UINT len, unsigned int index, unsigned int start,bool bIsUpload);
	 virtual int						WriteData(char *buf,  QVOD_UINT64 start,UINT len);
	 virtual int						DeleteHasPlayingData(QVOD_UINT64 iCurCacheSize);
	 virtual void					UpdateHttpOrgUrl(string& strHttpOrgUrl);
     virtual int	                    UpdateClarityUrl(string& strHttpOrgUrl, string& strNewHash, string& strNewClarityPath, int nClarity);
	 virtual int						SetCopyFullFile( string& strCopyPath );
	 virtual int						UnsetCopyFullFile();
	 virtual void					DownEmergencyData();
	 virtual void					UpdateDownloadMode();
	 virtual void					NotifyVideoBuffering(bool bBuffering);
	 virtual void					Pause();
	 bool								IsExistM3u8();

	 unsigned int					GetContinuesTime(const unsigned int& nPlayTime);		// 获取当前时间后的缓存时间，ms
	 unsigned int					SetPlayingTimepoint(const unsigned int& timepoint,bool bSeek);
	 bool								WriteM3u8File(const char* buf, const int& len);
	 string								GetUnhaveTsFromIndex(unsigned int& index, QVOD_UINT64& start, unsigned int& len);
	 bool								CheckTsInMem(unsigned int& index);								// 查找ts片是否已下载到mem文件中

	 void								SetDownload(bool bDownload);
	 void								SetAesEncrypt(bool bEncrypt);												// 设置该下载任务是否需要加密(仅对能整合的文件)
	 string								QueryTaskInfo();
private:
	void								UpdateM3u8OrgUrl(string& strHttpOrgUrl, bool bChangeClarity=false);
	void								SetTsIndexDownloading(const unsigned int &index);
	void								ReSetTsIndexDownloading(const unsigned int &index);
	bool								IsTsIndexDownloading(const unsigned int& index);
	int									GetFirstTsIndexDownloading();
	int									WriteM3u8Data(const char *buf, const QVOD_UINT64& start, const UINT& len);	// 写入数据
	bool								ReadAesKey(string & strAesKey, int &nAesver);																			// 读取key
	void								GenerateAesKeyFile(const string& strAesJson="");																	// 随机生成aeskey文件
	string								GenerateAesKey(unsigned char key[16]);																							// 随机生成aeskey, 返回密钥
	bool								DecodeAesJson(const string& strInfo, string& strAesKey, int &nVer);													// 解析Json串中的aeskey
	string								EncodeAesJson(const string& strAesKey, const int nVer=0);																							// 生成aeskey串
public:
	CM3u8 *						m_m3u8;																						//m3u8文件

	bool							m_bNeedAes;																				// 是否需要加密
	bool							m_bAes;																							// 是否加密
	CAESData					m_aesdata;																					// aes加解密
private:
	bool							m_bResetIndex;																			// 是否清空正在下载索引
	CLock							m_tsIndexLock;																			// 当前下载ts索引锁
	set<int>						m_tsIndexDownloading;																// 当前正在下载的ts片索引
};

