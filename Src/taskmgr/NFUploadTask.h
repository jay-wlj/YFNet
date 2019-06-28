#pragma once
#include "task.h"
#include "NetSign.h"
#include <set>
#include <vector>
using namespace std;

class CNFUploadTask :
	public CTask
{
public:
	CNFUploadTask(const unsigned char* hash, int type, const string& strOrgUrl, const string& strFilePath);
	~CNFUploadTask(void);

public:
	virtual int run(bool bload=0);
	virtual void Start();
	//virtual int WriteData(const char *buf, UINT len, unsigned int index, unsigned int start);
	virtual int WriteHttpData(const char *buf, QVOD_UINT64 start,UINT len);
	virtual void UpdateDownloadMode();
	virtual void DownHandle();
	virtual int DownNextHttpPiece(HttpPostReq& req);
	virtual bool TaskIsFinish(bool bCheckFileExist = false);
	QVOD_INT64 GetFileSize();
	virtual QVOD_INT64 GetTotalUploadLen();
	virtual int SetHeadValue(const char* strHead, const char* strValue);
	virtual int SetHeadValue(const char *pstrHeadAdd);
	virtual void					Pause();
	virtual void NotifyDownload(const int &nId);
private:

	void								SetTsIndexUploading(const unsigned int &index);
	void								ReSetTsIndexUploading(const unsigned int &index);
	bool								IsTsIndexUploading(const unsigned int& index);
private:
	string GetHeadAddString();
private:
	string m_strFilePath;

	string m_strCheckUploadUrl;					// 上传文件初始化请求url
	unsigned int m_uchunksize;						// 块大小
	QVOD_INT64 m_uHasUploadSize;			// 已上传的大小
	QVOD_INT64		m_filesize;							// 上传的文件大小

	map<string, string>	m_mHeadVals;
	bool	m_bFinishUpload;							// 上传完成
	net::CNetSign	m_sign;
	set<unsigned int>	m_vNeedChunkdx;		// 需要上传的块索引
	set<unsigned int> m_vHasChunkdx;			// 已经上传完的块索引

	CLock							m_tsIndexLock;																			// 当前下载ts索引锁
	set<int>						m_tsIndexUploading;																// 当前正在上传的片索引
};

