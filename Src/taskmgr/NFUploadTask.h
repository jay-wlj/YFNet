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

	string m_strCheckUploadUrl;					// �ϴ��ļ���ʼ������url
	unsigned int m_uchunksize;						// ���С
	QVOD_INT64 m_uHasUploadSize;			// ���ϴ��Ĵ�С
	QVOD_INT64		m_filesize;							// �ϴ����ļ���С

	map<string, string>	m_mHeadVals;
	bool	m_bFinishUpload;							// �ϴ����
	net::CNetSign	m_sign;
	set<unsigned int>	m_vNeedChunkdx;		// ��Ҫ�ϴ��Ŀ�����
	set<unsigned int> m_vHasChunkdx;			// �Ѿ��ϴ���Ŀ�����

	CLock							m_tsIndexLock;																			// ��ǰ����ts������
	set<int>						m_tsIndexUploading;																// ��ǰ�����ϴ���Ƭ����
};

