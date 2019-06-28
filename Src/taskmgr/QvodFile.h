#pragma once

#include "qvod_lock.h"
#include "qvod_string.h"
#include "qvod_file.h"


class CQvodFile  
{
public:
	CQvodFile(std::string strFile,QVOD_INT64 len,bool bIsShare = false);
	virtual ~CQvodFile();
public:
	std::string GetFileName();
	QVOD_INT64 GetCurDownloadLen();	
	QVOD_INT64 GetTotalDownloadLen();
	void SetTotalDownloadLen(QVOD_INT64 iDownloadLen);
	virtual QVOD_INT64 GetFileLen();
	virtual int Write(QVOD_INT64 offset,char* buf,unsigned int len);
	virtual int Read(QVOD_INT64 offset,char* buf,unsigned int len);
	int Create();
	void Close();
	bool Rename();
	bool IsExist();
	bool IsComplete();
	bool Remove();
	bool RemoveTempFile();
private:
	bool AllocateSize();
protected:
	std::string		m_strFileName;
	QVOD_INT64		m_lFileLen;
	QVOD_INT64		m_iCurDownloadLen;
	QVOD_INT64		m_iTotalDownloadLen;

#ifdef WIN32
	HANDLE		   m_hFile;
#else
	FILE*			m_hFile;
#endif
	CLock			m_lock;
#ifndef SUPERNODE
#ifdef WIN32
	HANDLE			m_hFileMap;
#endif
#endif
};

class CTmpQvodFile :public CQvodFile
{
private:
	QVOD_INT64 m_offset;
	char* m_pbuf;
public:
	CTmpQvodFile(std::string strFile,QVOD_INT64 len,QVOD_INT64 offset);
	virtual ~CTmpQvodFile();
	virtual QVOD_INT64 GetFileLen();
	virtual int Write(QVOD_INT64 offset,char* buf,unsigned int len);
	virtual int Read(QVOD_INT64 offset,char* buf,unsigned int len);
};

