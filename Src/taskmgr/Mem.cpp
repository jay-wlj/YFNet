#include "Mem.h"
#include "qvod_core.h"
#include "stdafx.h"
#include "common-commonfun.h"
#include <algorithm>
#include "common-utility.h"
#include "qvod_file.h"
#include "stdio.h"

#ifdef WIN32
#include <atlconv.h>
#endif

using namespace std;

extern string g_strtorrentpath;


CMem::CMem(const char *filename)
{
	m_filename = string(filename);//mem文件
	m_filesize = 0;				  //下载文件大小
	m_piecesize = 0;			  //下载文件分块数
	m_piecenum = 0;
	m_downsize = 0;				  //已经完成下载的大小
	m_pBitField = NULL;			  //mem文件内容
	m_pBitFieldLen = 0;			  //mem文件内容长度
	m_pFileData = NULL;
	m_piecetmpdown = 0;
}

CMem::~CMem()
{
	if ( m_pFileData != NULL )
	{
		delete[] m_pFileData;
		m_pFileData = NULL;
	}
}

void CMem::Remove()
{
	CAutoLock lock(m_lock);
	if ( m_pFileData != NULL )
	{
		delete[] m_pFileData;
		m_pFileData = NULL;
	}
	m_pBitField = NULL;
	m_downsize= 0;
	string strFile = g_strtorrentpath + m_filename + string(".mem");
	if ( QVOD_ACCESS(strFile.c_str(),0) != -1)
	{
		bool bResult = 0 == QvodRemoveFile(strFile.c_str());

		if ( ! bResult)
		{
			//delete failed!
		
#ifdef _DEBUG
			int iError = GetLastError();
			_OUTPUT(INEED, "Delete File Failed!!!File: %s,Error Code:%d\n", strFile.c_str(), iError);
#endif // _DEBUG
		}
	}
}

QVOD_INT64 CMem::GetContinueLen(QVOD_INT64 pos)
{
	CAutoLock lock(m_lock);
	if ( pos < 0 || pos >= m_filesize )
	{
		return 0;
	}

	if ( m_piecesize == 0 )
	{
		return 0;
	}

	unsigned int index = (unsigned int)(pos / m_piecesize);
	unsigned int offset = (unsigned int)(pos % m_piecesize);
	unsigned int iLastPieceSize = (unsigned int)(m_filesize % m_piecesize);
	QVOD_INT64 len = (QVOD_INT64)0 - (QVOD_INT64)offset;
	for ( unsigned int i = index; i < m_piecenum; i++ )
	{
		if ( !CheckIndex(i) )
		{
			break;
		}
		else
		{
			if ( i == m_piecenum-1 )
			{
				len += iLastPieceSize;
				return len;
			}
			else
			{
				len += m_piecesize;
			}
		}
	}
	if (len < 0)
	{
		len = 0;
	}
	return len;
}

bool CMem::Init(QVOD_UINT64 filesize, unsigned int piecesize)
{
	CAutoLock lock(m_lock);
	if(filesize == 0 || piecesize == 0)
	{
		return false;
	}

	m_downsize = 0;

	m_filesize = filesize;
	m_piecesize = piecesize;

	if(m_filesize % m_piecesize == 0)
	{
		m_piecenum = (unsigned int)(m_filesize / m_piecesize);
	}
	else
	{
		m_piecenum = (unsigned int)(m_filesize / m_piecesize + 1);
	}

	//计算文件大小
	int len = 0;
	if(0 == m_piecenum % 8)
	{
		len = m_piecenum / 8;
	}
	else
	{
		len = m_piecenum / 8 + 1; 
	}
	m_pBitFieldLen = len;
#ifdef NETBAR
	//Mem文件总字节数
	int iFileLen = m_pBitFieldLen +16;

	//Mem文件内容
	m_pFileData = new char[iFileLen];
	memset(m_pFileData,0,iFileLen);

	//比特位信息
	m_pBitField = (unsigned char*)m_pFileData + 8; 
	memcpy(m_pFileData,&m_filesize,sizeof(m_filesize));
#else

	bool bIsOldMem = IsExist();
#ifdef WIN32 /* WIN32 */
	HANDLE hMemFile = CreateFile((g_strtorrentpath + m_filename + string(".mem") ).c_str(),   
		GENERIC_WRITE|GENERIC_READ,   
		FILE_SHARE_READ|FILE_SHARE_WRITE,   
		NULL,   
		OPEN_ALWAYS,   
		FILE_ATTRIBUTE_NORMAL,   
		NULL);   
	if( hMemFile == INVALID_HANDLE_VALUE)   
	{    
		return false;
	}
#else /* posix */
	FILE* hMemFile = fopen((g_strtorrentpath + m_filename + string(".mem") ).c_str(), "a+b");
	if(hMemFile == NULL)
	{
#ifdef QVODDEBUG
		_OUTPUT(LOG_DEBUG, "CMem::Init fopen error\n");
#endif // QVODDEBUG
		return false;
	}

	/* make sure read from start */
	fseek(hMemFile, 0, SEEK_SET);
#endif /* posix end */

	//Mem文件总字节数
	int iFileLen = m_pBitFieldLen +16;

	//Mem文件内容
	if (m_pFileData)
	{
		delete[] m_pFileData;
		m_pFileData = NULL;
	}
	m_pFileData = new char[iFileLen];
	memset(m_pFileData,0,iFileLen);

	QVOD_DWORD dwRealRead = 0;
	if(bIsOldMem )
	{
#ifdef WIN32 /* WIN32 */
		DWORD dwRealRead = 0;
		if(!ReadFile(hMemFile,m_pFileData,iFileLen,&dwRealRead,NULL))
		{
			CloseHandle(hMemFile);
			return false;
		}
#else /* posix */
		int realRead = fread(m_pFileData, 1, iFileLen, hMemFile);
		if (realRead != iFileLen)
		{
			/* end or error */
			if(ferror(hMemFile))
			{
				/* error, return false */
#ifdef QVODDEBUG
				_OUTPUT(LOG_DEBUG, "CMem::Init fread error\n");
#endif // QVODDEBUG
				fclose(hMemFile);
				return false;
			}
#ifdef QVODDEBUG
			_OUTPUT(LOG_DEBUG, "%s(%d)-%s: need fread again errno %d, realRead %d, iFileLen %d\n", __FILE__, __LINE__, __FUNCTION__, errno, realRead, iFileLen);
#endif // QVODDEBUG
		}
#endif /* posix end */
	}

	//比特位信息
	m_pBitField = (unsigned char*)m_pFileData + 8; 
	memcpy(m_pFileData,&m_filesize,sizeof(m_filesize));
	memcpy((void*)&m_downsize, (const void*)(m_pFileData + m_pBitFieldLen + 8), sizeof(m_downsize));
#endif
#ifdef WIN32 /* WIN32 */
	CloseHandle(hMemFile);
#else /* posix */
	fclose(hMemFile);
#endif /* posix end */

	return true;
}

int CMem::WriteToDisk()
{
	CAutoLock lock(m_lock);
	return WriteMemFile();
}

bool CMem::IsExist()
{	
#ifdef NETBAR
	if (NULL != m_pFileData)
	{
		return true;
	}
#else
	string file = g_strtorrentpath + m_filename + ".mem";

	if ( QVOD_ACCESS(file.c_str(),0) != -1 )
	{
		return true;
	}
#endif
	return false;
}

//判断某一块数据是否下载完成 return : true 完成 false 没有完成
bool CMem::CheckIndex(unsigned int index)
{
	CAutoLock lock(m_lock);
	//检查是否拥有该分片
	if (index < 0 || index >= m_piecenum || !m_pBitField)
	{
		return false;
	}
	int Pos= index >> 3;
	int re = index & 0x07;
	
	return  (m_pBitField[Pos]&0x80>>re)==0?false:true;
}

bool CMem::ClearBitField(unsigned int index)
{
	CAutoLock lock(m_lock);
	//index的范围不在0-m_piecenum之间,或者比特位信息还没被初始化
	if(index<0 || index >= m_piecenum || !m_pBitField)
	{
		return false;
	}
	int Pos= index >> 3;
	int re = index & 0x07;
	if(m_pBitField[Pos] & 0x80>>re)
	{
		m_pBitField[Pos] &= ~(0x80>>re); //加入未下载队列
		m_piecetmpdown--;//临时下载计数加一
		return true;
	}	
	return false;
}

bool CMem::ClearDownLength(QVOD_INT64 length)
{
	CAutoLock lock(m_lock);
#ifdef QVODDEBUG
	if (m_downsize < length)
	{
		_OUTPUT(INFO, "clear length %d is bigger than downsize %d\n", length, m_downsize);
	}
#endif // _DEBUG
	m_downsize -= length;
	memcpy(m_pFileData + m_pBitFieldLen + 8,&m_downsize,8);
	return true;
}

bool CMem::SetBitField(unsigned int index)
{
	CAutoLock lock(m_lock);
	//index的范围不在0-m_piecenum之间,或者比特位信息还没被初始化
	if(index<0 || index >= m_piecenum || !m_pBitField)
	{
		return false;
	}
	int Pos= index >> 3;
	int re = index & 0x07;
	if(m_pBitField[Pos] & 0x80>>re)//已经下载了
	{
		return false;
	}
	m_pBitField[Pos] |= 0x80>>re;//加入已下载队列
	m_piecetmpdown++;//临时下载计数加一
	return true;
}

int CMem::SetBitField(QVOD_INT64 fileSize,unsigned int piecesize)
{
	CAutoLock lock(m_lock);
	if(fileSize == 0 || piecesize == 0)
	{
		return false;
	}
	m_filesize = fileSize;
	m_piecesize = piecesize;
	if(m_filesize % m_piecesize == 0)
	{
		m_piecenum = (unsigned int)(m_filesize / m_piecesize);
	}
	else
	{
		m_piecenum = (unsigned int)(m_filesize / m_piecesize + 1);
	}
	//计算文件大小
	int len = 0;
	if(0 == m_piecenum % 8)
	{
		len = m_piecenum / 8;
	}
	else
	{
		len = m_piecenum / 8 + 1; 
	}
	m_pBitFieldLen = len;
	int iFileLen = len + 16;
	m_pFileData = new char[iFileLen];
	memset(m_pFileData,0,iFileLen);
	memcpy(m_pFileData,&m_filesize,sizeof(m_filesize));
	m_pBitField = (unsigned char*)m_pFileData + 8;
	memset(m_pBitField,0xff,m_pBitFieldLen);
	memcpy(m_pFileData+8+m_pBitFieldLen,&m_filesize,sizeof(m_filesize));
	m_downsize = m_filesize;
	int re = m_piecenum & 0x07;
	if (re != 0)
	{
		m_pBitField[m_pBitFieldLen-1] = 0xff << (8-re);
	}
	return 0;
}

bool CMem::SetDownLength(QVOD_INT64 length)
{
	CAutoLock lock(m_lock);
#ifdef QVODDEBUG
	if((m_downsize + length) > m_filesize)
	{
		_OUTPUT(INFO,"######~下载的文件大小超过了实际大小啦!~~~~~~\n");
		//return false;
	}
#endif // _DEBUG
	m_downsize += length;		
	memcpy(m_pFileData + m_pBitFieldLen + 8,&m_downsize,8);
	return true;
}

//是否全下载完成
bool CMem::IsDownFinish()
{
	CAutoLock lock(m_lock);
	if(m_downsize == 0)
	{
		return false;
	}
	if(m_downsize > m_filesize)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO,"!~下载的文件大小比实际文件大!~~~~~~\n");
#endif // _DEBUG
		m_downsize = m_filesize;
		return true;
	}
	if(m_downsize == m_filesize)
	{
		return true;
	}
	return false;
}

unsigned int CMem::GetBitFieldLen()
{
	return m_pBitFieldLen;
}

unsigned char* CMem::GetMemBit()
{
	if(m_pBitField)
	{
		return m_pBitField;
	}
	return NULL;
}

int CMem::SetBitField()
{
	CAutoLock lock(m_lock);
	if(!m_pBitField)
	{
		return -1;
	}
	memset(m_pBitField,0xff,m_pBitFieldLen);
	m_downsize = m_filesize;
	int re = m_piecenum & 0x07;
	if (re != 0)
	{
		m_pBitField[m_pBitFieldLen-1] = 0xff << (8-re);
	}
	return 0;
}

void CMem::Clear()
{
	CAutoLock lock(m_lock);
	memset(m_pBitField,0,m_pBitFieldLen);
	m_downsize= 0 ;
}

int CMem::WriteMemFile()
{
	CAutoLock lock(m_lock);
	if (NULL == m_pFileData)
	{
		return false;
	}
	FILE* hMemFile = fopen((g_strtorrentpath + m_filename + string(".mem") ).c_str(),"wb");
	if(hMemFile == NULL)
	{
#ifdef QVODDEBUG
		_OUTPUT(LOG_DEBUG, "CMem::GetFilesizeFromMemfile fopen error %d\n",errno);
#endif // QVODDEBUG
		return false;
	}
	int dwRealWrite = fwrite(m_pFileData, 1, m_pBitFieldLen+16, hMemFile);
#ifdef QVODDEBUG
	if(dwRealWrite!= m_pBitFieldLen+16)
	{
		_OUTPUT(SERIOUS,"WriteMemFile Fail %d!!!\n",dwRealWrite);
	}
#endif // _DEBUG
	//FlushFileBuffers(hMemFile);
	//CloseHandle(hMemFile);
	fclose(hMemFile);
	return dwRealWrite;
}

QVOD_INT64 CMem::GetFilesizeFromMemfile()
{
	CAutoLock lock(m_lock);
	QVOD_INT64 iFilesize = 0;
	FILE* hMemFile = fopen((g_strtorrentpath + m_filename + string(".mem") ).c_str(),"rb");
	if(hMemFile == NULL)
	{
#ifdef QVODDEBUG
		_OUTPUT(LOG_DEBUG, "CMem::GetFilesizeFromMemfile fopen error %d\n",errno);
#endif // QVODDEBUG
		return false;
	}
	fseek(hMemFile, 0, SEEK_END);
	long iMemFileSize = ftell(hMemFile);
	if ( iMemFileSize <= 16 + 8)
	{
		fclose(hMemFile);
		return iFilesize;
	}
	fseek(hMemFile, 0, SEEK_SET);
	char* pBuf = new char[iMemFileSize];
	if (NULL == pBuf)
	{
		fclose(hMemFile);
		return iFilesize;
	}
	int iRealReadLen = 0;
	iRealReadLen = fread(pBuf, 1, iMemFileSize, hMemFile);
	fclose(hMemFile);
	if (iMemFileSize != iRealReadLen)
	{
		delete[] pBuf;
		return iFilesize;
	}
	QVOD_INT64 iTmpFilesize = 0;
	memcpy(&iTmpFilesize, pBuf, 8);
	delete[] pBuf;
	int iBitLen = iMemFileSize - 16;
	if (0 == iTmpFilesize)
	{
		return iFilesize;
	}
	unsigned int iPieceSize = CalcPieceSize(iTmpFilesize);

	int iPieceNum = iTmpFilesize/iPieceSize + (iTmpFilesize%iPieceSize == 0?0:1);
	if (iPieceNum/8 + (iPieceNum%8 == 0?0:1) == iBitLen)
	{
		iFilesize = iTmpFilesize;
	}

	return iFilesize;
}
