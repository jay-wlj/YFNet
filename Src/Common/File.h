#ifndef FILE_H
#define FILE_H

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include "qvod_file.h"
#include "qvod_lock.h"
#include "qvod_string.h"

class CFile
{
public:
	enum Status
	{
		OPEN,
		CLOSE
	};
	enum OpenMode
	{
		READONLY,
		READWRITE
	};

protected:
	std::string m_strFileName;
	FILE* pf;
	//int fh;
	QVOD_INT64 m_filelen;
	QVOD_INT64 m_downloadlen;
	CLock m_lock;
	Status m_status;
	OpenMode m_openmode;
	int Create();
	int Open();
public:
	CFile(std::string strFile,QVOD_INT64 len);
	virtual ~CFile();
	int IsExist();
	std::string GetFileName(){return m_strFileName;}
	virtual QVOD_INT64 GetFileLen(){return m_filelen;}
	QVOD_UINT64 GetDownloadLen(){return m_downloadlen;	}
	virtual int Write(QVOD_INT64 offset,char* buf,unsigned int len);
	virtual int Read(QVOD_INT64 offset,char* buf,unsigned int len);
	int Close();
	int Rename();
};

class CTmpFile :public CFile
{
private:
	int m_offset;
	char* m_pbuf;
public:
	CTmpFile(std::string strFile,QVOD_INT64 len,QVOD_INT64 offset);
	virtual ~CTmpFile();
	virtual QVOD_INT64 GetFileLen(){return m_filelen + m_offset;}
	virtual int Write(QVOD_INT64 offset,char* buf,unsigned int len);
	virtual int Read(QVOD_INT64 offset,char* buf,unsigned int len);
};

#endif