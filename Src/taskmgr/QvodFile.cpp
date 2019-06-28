#include "QvodFile.h"
#include "qvod_time.h"
#include "qvod_error.h"
#include "common-commonfun.h"

CQvodFile::CQvodFile(std::string strFile, QVOD_INT64 len, bool bIsShare)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO,"$$$$$$$$$$$$$File size:%lld\n",len);
#endif
	m_iCurDownloadLen = 0;
	m_iTotalDownloadLen = 0;
	m_lFileLen    = len;
	m_strFileName = strFile;
	if(!bIsShare)
	{
		m_strFileName = m_strFileName+std::string(".!mv");
	}

#ifdef WIN32
	m_hFile = INVALID_HANDLE_VALUE ;
	m_hFileMap = NULL;
#else
	m_hFile = NULL;
#endif
}

CQvodFile::~CQvodFile()
{
	//关闭句柄
	Close();
}


#ifdef WIN32
int CQvodFile::Create()
{
	CAutoLock lock(m_lock);
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	size_t pos = m_strFileName.find("/",0);
	while((pos = m_strFileName.find("/",pos+1)) != std::string::npos)
	{
		std::string path = m_strFileName.substr(0,pos);
		mkdir(path.c_str());
	} 
	bool bIsExist = IsExist();
	m_hFile = CreateFile(m_strFileName.c_str(),   
		GENERIC_WRITE|GENERIC_READ, 
		FILE_SHARE_READ|FILE_SHARE_WRITE,   
		NULL,   
		OPEN_ALWAYS,   
		FILE_ATTRIBUTE_NORMAL,   
		NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_DISK_FULL)
		{
			return -2;
		}
		return -1;
	}
	//若文件不存在重新创建文件
	if(!bIsExist)
	{
		if(!AllocateSize())
		{
			// 			Close();
			// 			DeleteFileW(m_strFileName.c_str());
			// 			return false;
		}
	}
#ifndef SUPERNODE
	LARGE_INTEGER liOffset = {0};
	liOffset.QuadPart = m_lFileLen;
	m_hFileMap = CreateFileMapping(m_hFile,0,PAGE_READWRITE,liOffset.HighPart,liOffset.LowPart,NULL);
	if(m_hFileMap == NULL)
	{
		QVOD_DWORD error = GetLastError();
		Close();
		DeleteFile(m_strFileName.c_str());
		if ( ERROR_DISK_FULL == error)
		{
			return -2;
		}
		return -3;
	}
#endif
	return 0;
}
#else
int CQvodFile::Create()
{
	CAutoLock lock(m_lock);
	if(m_hFile != NULL)
	{
		return 0;
	}
	size_t pos = m_strFileName.rfind("/");
	if(std::string::npos != pos)
	{
		std::string dir = m_strFileName.substr(0,pos);
		QvodMkdir(dir.c_str());
	}
	bool bFileExist = IsExist();
	//若文件不存在重新创建文件
	if(!bFileExist)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "file not exist and create it\n", m_strFileName.c_str());
#endif		
		AllocateSize();
	}
	//打开写句柄

	m_hFile = fopen(m_strFileName.c_str(),"rb+");
	if(m_hFile == NULL)
	{
		return -1;
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CQvodFile::Create() ok %s\n", m_strFileName.c_str());
#endif
	return 0;
}
#endif

void CQvodFile::Close()
{
#ifdef WIN32
#ifndef SUPERNODE
	if(m_hFileMap != NULL)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}
#endif
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
#else
	if(m_hFile != NULL)
	{
		fclose(m_hFile);
		m_hFile = NULL;
	}
#endif
}

bool CQvodFile::Remove()
{
	CAutoLock lock(m_lock);

	//关闭句柄
	Close();

#ifdef WIN32
	if(DeleteFile(m_strFileName.c_str()))
	{
		return true;
	}
#else
	if(remove(m_strFileName.c_str()))
	{
		return true;
	}
#endif

	return false;
}

bool CQvodFile::RemoveTempFile()
{
	CAutoLock lock(m_lock);

	//关闭句柄
	Close();

	if(m_strFileName.find(".!mv") != std::string::npos)
	{
#ifdef WIN32
		if( ::DeleteFile(m_strFileName.c_str()) )
		{
			return true;
		}
#else
		if(remove(m_strFileName.c_str()))
		{
			return true;
		}
#endif
	}

	return false;
}

bool CQvodFile::IsComplete()
{
	int iPos = m_strFileName.find(".!mv");
	if(iPos != std::string::npos)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CQvodFile::IsExist()
{
	if( QVOD_ACCESS(m_strFileName.c_str(),0) != -1)
	{
		return true;
	}
	return false;
}

bool CQvodFile::AllocateSize()
{
#ifdef WIN32
	QVOD_DWORD time = GetTickCount();
	LARGE_INTEGER liOffset = {0};
	liOffset.QuadPart = m_lFileLen;
	//设置到文件末尾
	if(SetFilePointerEx(m_hFile,liOffset,NULL,FILE_BEGIN) == 0)
	{
		return false;
	}
	//定义文件结束符
	BOOL result = SetEndOfFile(m_hFile);
#ifndef SUPERNODE
	result = SetFileValidData(m_hFile,liOffset.QuadPart);
	if(result == 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(SERIOUS,"###SetFileValidData Err %d\n",GetLastError());
#endif // _DEBUG
	}
#endif

#ifdef QVODDEBUG
	QVOD_DWORD invaltime = GetTickCount() - time;
	if(invaltime >= 1000)
	{
		_OUTPUT(ERROR1,"++++++++++++++++AllocateSize Total Time:%d\n",invaltime);
	}
#endif // _DEBUG
	return true;
#else
	FILE* hFile = fopen(m_strFileName.c_str(),"wb+");
	if(hFile != NULL)
	{
#ifdef C2_EMBEDED
		c2_fat_fs_enlarge_file(fileno(hFile), m_lFileLen);
#endif
		//fseeko(hFile, -1, SEEK_END);
		fwrite("0",1,1,hFile);
		fflush(hFile); 
		fclose(hFile);
		return true;
	}
	return false;
#endif
}

int CQvodFile::Write(QVOD_INT64 offset,char* buf,unsigned int len)
{
#ifdef WIN32
	CAutoLock lock(m_lock);
	QVOD_DWORD time = GetTickCount();
	//加载任务时候,第一次写入打开文件句柄
	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		int ret = Create();
		if (ret < 0)
		{
			if (-2 == ret)
			{
				//没空间了
				return -3;
			}
			return -1;
		}
	}
	if(!IsExist())
	{
		//关闭句柄
		Close();
		return -2;
	}
	LARGE_INTEGER liOffset = {0};
	unsigned int tLen = (unsigned int)min(len,m_lFileLen - offset);
	liOffset.QuadPart = offset;
	//写入文件操作
	if(SetFilePointerEx(m_hFile,liOffset,NULL,FILE_BEGIN) == 0)
	{
		return -1;
	}
	char* pbuf = buf;
	unsigned int iLen = 0;
	while(iLen < tLen)
	{
		QVOD_DWORD dwWrite = 0;
		BOOL result = WriteFile(m_hFile,pbuf,tLen-iLen,(LPQVOD_DWORD)&dwWrite,NULL);
		if(result == 0)
		{
			//空间不足返回-3
			QVOD_DWORD iError = GetLastError();
			if (ERROR_DISK_FULL == iError)
			{
				return -3;
			}
			return -1;
		}
		iLen += dwWrite;
		pbuf += dwWrite;
	}
#ifdef QVODDEBUG
	QVOD_DWORD invaltime = GetTickCount()-time;
	if(invaltime >= 1000)
	{
		_OUTPUT(ERROR1,"++++++++++++++++++++Write time:%d\n",invaltime);
	}
#endif // _DEBUG
	m_iCurDownloadLen += tLen;
	m_iTotalDownloadLen += tLen;
	return tLen;
#else
	CAutoLock lock(m_lock);
	if(NULL == m_hFile)
	{
		int ret = Create();
		if (ret < 0)
		{
			if (-2 == ret)
			{
				//没空间了
				return -3;
			}
			return -1;
		}
	}
	if(QVOD_SEEK(m_hFile, offset, SEEK_SET) != 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(SERIOUS,"##################CQvodFile::seek Fail:offset:%u,error = %d!!!\n",offset,errno);
#endif
		Close();
		return -1;
	}
	int iLen = fwrite(buf, len, 1, m_hFile);
	if(iLen > 0)
	{
		m_iCurDownloadLen += len;
		m_iTotalDownloadLen += len;
		if(fflush(m_hFile) != 0)
		{
#ifdef QVODDEBUG
			_OUTPUT(FAIL, "fflush file fail %d, len %d, offset %lld\n", errno, len, offset);
#endif
			Close();
			return -1;
		}
	}
	else
	{
		/* error */
		int error = errno;
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "%s(%d)-%s: fwrite %lld error, write %d, len %d! error: %d\n", __FILE__, __LINE__, __FUNCTION__, offset,
			iLen, len, error);
#endif // QVODDEBUG
		Close();
		if(ENOENT == error)// No such file or directory
		{
			return -2;
		}
		else if(ENOSPC == error)//No space left on device 
		{
			return -3;
		}
		return -1;
	}
	return iLen;	
#endif
}

int CQvodFile::Read(QVOD_INT64 offset,char* buf,unsigned int len)
{
#ifdef WIN32
	QVOD_DWORD time = GetTickCount();
	LARGE_INTEGER liOffset = {0};
#ifdef SUPERNODE
	liOffset.QuadPart = offset;
	//打开句柄
	HANDLE hFile = CreateFile(m_strFileName.c_str(),   
		GENERIC_READ,   
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,   
		NULL,   
		OPEN_EXISTING,   
		FILE_ATTRIBUTE_NORMAL,   
		NULL); 
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	if(SetFilePointerEx(hFile,liOffset,NULL,FILE_BEGIN) == 0)
	{
		CloseHandle(hFile);
		return -1;
	}
	unsigned int iLen = 0;
	BOOL result = ReadFile(hFile,buf,len,(LPQVOD_DWORD)&iLen,NULL);
	if(!result)
	{
		iLen = -1;
	}
	//关闭句柄
	CloseHandle(hFile);
	return iLen;
#else
	CAutoLock lock(m_lock);
	bool bIsComplete = IsComplete();
	if(bIsComplete)
	{
		liOffset.QuadPart = offset;
		//打开句柄
		HANDLE hFile = CreateFile(m_strFileName.c_str(),   
			GENERIC_READ,   
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,   
			NULL,   
			OPEN_EXISTING,   
			FILE_ATTRIBUTE_NORMAL,   
			NULL); 
		if(hFile == INVALID_HANDLE_VALUE)
		{
			return -1;
		}
		if(SetFilePointerEx(hFile,liOffset,NULL,FILE_BEGIN) == 0)
		{
			CloseHandle(hFile);
			return -1;
		}
		unsigned int iLen = 0;
		BOOL result = ReadFile(hFile,buf,len,(LPQVOD_DWORD)&iLen,NULL);
		if(!result)
		{
			iLen = -1;
		}
		//关闭句柄
		CloseHandle(hFile);
		return iLen;
	}
	else
	{
		//加载任务时候,第一次写入打开文件句柄
		if(m_hFile == INVALID_HANDLE_VALUE)
		{
			int ret = Create();
			if (ret < 0)
			{
				if (-2 == ret)
				{
					//没空间了
					return -3;
				}
				return -1;
			}
		}
		if(!IsExist())
		{
			//关闭句柄
			Close();
			return -1;
		}
		liOffset.QuadPart = offset;
		if(SetFilePointerEx(m_hFile,liOffset,NULL,FILE_BEGIN) == 0)
		{
			return -1;
		}
		unsigned int iLen = 0;
		BOOL result = ReadFile(m_hFile,buf,len,(LPQVOD_DWORD)&iLen,NULL);
		if(!result)
		{
			return -1;
		}
	}
#ifdef QVODDEBUG
	QVOD_DWORD invaltime = GetTickCount() - time;
	if(invaltime >= 1000)
	{
		_OUTPUT(ERROR1,"++++++++++++++++++++Read time:%d\n",invaltime);
	}
#endif // _DEBUG
	return len;
#endif
#else
	//打开句柄
	FILE* hFile = fopen(m_strFileName.c_str(),"rb");
	if(hFile == NULL)
	{
		return -1;
	}
	if(QVOD_SEEK(hFile, offset, SEEK_SET) != 0)
	{
		fclose(hFile);
		return -1;
	}
	int iLen = fread(buf, sizeof(char), len, hFile);
	//关闭句柄
	fclose(hFile);
	return iLen;
#endif
}

bool CQvodFile::Rename()
{
	CAutoLock lock(m_lock);
	//关闭句柄
	Close();

	//改名过程
	std::string strFile = m_strFileName;
	unsigned int iPos = m_strFileName.find(".!mv");
	if(iPos != std::string::npos)
	{
		strFile = m_strFileName.substr(0,iPos);		
	}
	else
	{
		//找不到"!mv"的后缀
		return false;
	}
	int iRes = rename(m_strFileName.c_str(),strFile.c_str());
	if(iRes == 0)
	{
		m_strFileName = strFile;
		return true;
	}
	return false;
}

std::string CQvodFile::GetFileName()
{
	return m_strFileName;
}

QVOD_INT64 CQvodFile::GetFileLen()
{
	return m_lFileLen;
}

QVOD_INT64 CQvodFile::GetCurDownloadLen()
{
	return m_iCurDownloadLen;	
}

QVOD_INT64 CQvodFile::GetTotalDownloadLen()
{
	return m_iTotalDownloadLen;
}

void CQvodFile::SetTotalDownloadLen(QVOD_INT64 iDownloadLen)
{
	m_iTotalDownloadLen = iDownloadLen;
}
CTmpQvodFile::CTmpQvodFile(std::string strFile,QVOD_INT64 len,QVOD_INT64 offset)
	:CQvodFile(strFile,len)
{
	m_offset = offset > len ? len : offset;
	m_lFileLen = len - m_offset;
	m_pbuf = new char[m_offset];
}

CTmpQvodFile::~CTmpQvodFile()
{
}

int CTmpQvodFile::Write(QVOD_INT64 offset,char* buf,unsigned int len)
{
	if(offset+len <= m_offset)
	{
		memcpy(m_pbuf+offset,buf,len);
		return len;
	}
	else if(offset+len > m_offset && offset < m_offset)
	{
		memcpy(m_pbuf+offset,buf,int(m_offset - offset));
		int iLen = CQvodFile::Write(0,buf+m_offset-offset,(unsigned int)(len+offset-m_offset));
		iLen += int(m_offset - offset);
		return iLen;
	}
	else
	{
		return CQvodFile::Write(offset-m_offset,buf,len);
	}
}

int CTmpQvodFile::Read(QVOD_INT64 offset,char* buf,unsigned int len)
{
	if(offset+len <= m_offset)
	{
		memcpy(buf,m_pbuf+offset,len);
		return len;
	}
	else if(offset+len > m_offset && offset < m_offset)
	{
		memcpy(buf,m_pbuf+offset,size_t(m_offset - offset));
		int iLen = CQvodFile::Read(0,buf+m_offset-offset,(unsigned int)(len+offset-m_offset));
		iLen += int(m_offset - offset);
		return iLen;
	}
	else
	{
		return CQvodFile::Read(offset-m_offset,buf,len);
	}
}

QVOD_INT64 CTmpQvodFile::GetFileLen()
{
	return m_lFileLen + m_offset;
}
