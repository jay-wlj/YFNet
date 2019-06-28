#ifndef _COMMON_FILE_H
#define _COMMON_FILE_H

#include <windows.h>
#include <WinBase.h>
#include <atlstr.h>
#include <ShlObj.h>
#include <CommDlg.h>
#include <vector>
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

#define QFILE_STREAM_ERROR  -1
#define MAKEI64(h, l) (((__int64)(h) << 32) | (__int64)(l))

namespace QFile{

	

class Tool
{
public:
	static BOOL IsFileExist(CString fileFullPath)
	{
		if(fileFullPath.GetLength() > 0)
		{
			DWORD dwAttr = ::GetFileAttributes(fileFullPath);
			if( INVALID_FILE_ATTRIBUTES != dwAttr && !(FILE_ATTRIBUTE_DIRECTORY&dwAttr))
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	static BOOL IsFileExist(std::string &fileFullPath)
	{
		if(fileFullPath.size() > 0)
		{
			DWORD dwAttr = ::GetFileAttributesA(fileFullPath.c_str());
			if( INVALID_FILE_ATTRIBUTES != dwAttr && !(FILE_ATTRIBUTE_DIRECTORY&dwAttr))
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	static BOOL IsFileExist(const char *fileFullPath)
	{
		if(fileFullPath && strlen(fileFullPath) > 0)
		{
			DWORD dwAttr = ::GetFileAttributesA(fileFullPath);
			if( INVALID_FILE_ATTRIBUTES != dwAttr && !(FILE_ATTRIBUTE_DIRECTORY&dwAttr))
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	static BOOL IsDirExist(CString dirFullPath)
	{
		if(dirFullPath.GetLength() > 0)
		{
			DWORD dwAttr = ::GetFileAttributes(dirFullPath);
			if( INVALID_FILE_ATTRIBUTES != dwAttr && (FILE_ATTRIBUTE_DIRECTORY&dwAttr))
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	static BOOL DeleteFile(CString fileFullPath)
	{
		return ::DeleteFile(fileFullPath);
	}
	static BOOL DeleteEmptyDir(CString dirFullPath)
	{
		return ::RemoveDirectory(dirFullPath);
	}
	static void EmptyDir(CString dirFullPath) // 清空文件夹
	{
		if(!IsDirExist(dirFullPath)) return ;
		if(dirFullPath[dirFullPath.GetLength()-1] != '\\')
		{
			dirFullPath += _T("\\");
		}
		std::vector<CString> vFileName;
		FindFileName(dirFullPath,vFileName);
		for(int i = 0; i < vFileName.size();i++)
		{
			CString tmp = dirFullPath + vFileName[i];
			if(IsFileExist(tmp))
			{
				DeleteFile(tmp);
			}
			else
			{
				DeleteDir(tmp);
			}
		}
		return ;
	}
	static BOOL DeleteDir(CString dirFullPath)
	{
		EmptyDir(dirFullPath);
		DeleteEmptyDir(dirFullPath);
		return TRUE;
	}
	static BOOL IsDirPathValid(CString strDirPath)
	{
		if(!IsDriveStringValid(strDirPath.Left(3)))
			return FALSE;
		CString strSubDriveString = strDirPath.Right(strDirPath.GetLength() - 3);
		if(strSubDriveString.Find(_T("\\\\")) > 0)
			return FALSE;
		CString strFilter = _T("/?*:\"|<>");
		for(int i = 0; i <strFilter.GetLength();i++ )
		{
			if(strSubDriveString.Find(strFilter.GetAt(i)) > 0)
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	static BOOL HideFile(CString filePath)
	{
		DWORD dwAttr = ::GetFileAttributes(filePath);
		if (dwAttr != INVALID_FILE_ATTRIBUTES)
		{
			if(dwAttr&FILE_ATTRIBUTE_HIDDEN)
			{
				return TRUE;
			}
			return 0!=SetFileAttributes(filePath, dwAttr|FILE_ATTRIBUTE_HIDDEN);
		}
		return FALSE;
	}
	static BOOL ShowFile(CString filePath)
	{
		DWORD dwAttr = ::GetFileAttributes(filePath);
		if (dwAttr != INVALID_FILE_ATTRIBUTES)
		{
			if(!(dwAttr&FILE_ATTRIBUTE_HIDDEN))
			{
				return TRUE;
			}
			return 0!=SetFileAttributes(filePath, dwAttr&~FILE_ATTRIBUTE_HIDDEN);
		}
		return FALSE;
	}
	static BOOL IsDriveStringValid(CString driveString)
	{
		BOOL bRet = FALSE;
		if(driveString.GetLength() == 3)
		{			
			int nLen = GetLogicalDriveStrings(0, NULL);
			TCHAR* szDriveStrings = new TCHAR[nLen];
			GetLogicalDriveStrings(nLen, szDriveStrings);
			for (int i = 0;i < nLen; i+=4)
			{
				if(driveString.CompareNoCase(szDriveStrings+i) == 0)
				{
					bRet = TRUE;
				}
			}
			delete []szDriveStrings;
		}
		return bRet;
	}

	static BOOL CreateDirectory(CString strDirPath)
	{
		if(strDirPath.GetLength() <= 0 || !IsDirPathValid(strDirPath))
		{
			return FALSE;
		}

		if(IsDirExist(strDirPath))
		{
			return TRUE;
		}

		if(strDirPath[strDirPath.GetLength()-1] != '\\')
		{
			strDirPath += _T("\\");
		}
		std::vector<CString> vCreatedDir;	
		for(int nPos = strDirPath.Find('\\'); nPos > 0; nPos = strDirPath.Find('\\', nPos)) 
		{
			CString strDirTemp = strDirPath.Left(nPos++);
			if(IsDirExist(strDirTemp))
				continue;
			if(::CreateDirectory(strDirTemp ,NULL))
			{
				vCreatedDir.push_back(strDirTemp);
			}
			else
			{
				for(int i = vCreatedDir.size() -1; i >= 0 ; i--)
				{
					RemoveDirectory(vCreatedDir[i]);
				}
				vCreatedDir.clear();
				return FALSE;
			}

		}
		return TRUE;
	}

	static BOOL CopyFile(const CString &strFrom, const CString &strTo)
	{
		TCHAR szPathFrom[MAX_PATH] = {0};
		TCHAR szPathTo[MAX_PATH] = {0};

		_tcscpy(szPathFrom, (LPCTSTR)strFrom);
		_tcscpy(szPathTo, (LPCTSTR)strTo);

		SHFILEOPSTRUCT sh;
		memset(&sh, 0x0, sizeof(sh));
		sh.pFrom = szPathFrom;
		sh.pTo = szPathTo;
		sh.fFlags = FOF_NOERRORUI|FOF_SILENT|FOF_NOCONFIRMATION;
		sh.wFunc = FO_COPY;
		BOOL bRet = SHFileOperation(&sh);
		return bRet;
	}
	static CString GetExtension(CString fileName)
	{
		CString extension;
		int npos = fileName.ReverseFind(_T('.'));
		int slantPos = fileName.ReverseFind(_T('\\'));
		if(npos > -1 && npos > slantPos)
		{
			extension = fileName.Right(fileName.GetLength() - npos);
		}
		return extension;
	}
	static CString GetFileName(CString path)
	{
		CString name;
		int npos = path.ReverseFind(_T('\\'));
		if(npos > -1 )
		{
			name = path.Right(path.GetLength() - npos-1);
		}
		return name;
	}

	static CString RemoveExtension(CString fileName)
	{
		int npos = fileName.ReverseFind(_T('.'));
		int slantPos = fileName.ReverseFind(_T('\\'));
		if(npos > -1 && npos > slantPos)
		{
			fileName = fileName.Left(npos) ;
		}
		return fileName;
	}

	static CString ModifyExtension(CString fileName, CString newExtension)
	{
		int npos = fileName.ReverseFind(_T('.'));
		if(npos > -1)
		{
			fileName = fileName.Left(npos) + newExtension;
		}
		return fileName;
	}

	static BOOL FindFileExt(LPCTSTR pExt, LPCTSTR pExtList)
	{
		long lenSrc = lstrlen(pExt);
		if(lenSrc == 0) return FALSE;
		LPCTSTR p = _tcsstr(pExtList, pExt);
		while(p)
		{
			p += lenSrc;
			if((*p == ';')||(*p == 0))
			{
				return TRUE;
			}
			p = _tcsstr(p, pExt);
		}

		return FALSE;
	}

	static CString GetDirPath(CString fileFullPath)
	{
		CString dirPath;
		int npos = fileFullPath.ReverseFind(_T('\\'));
		if(npos > -1 )
		{
			dirPath = fileFullPath.Left(npos);
			if(dirPath.GetLength() < 3)
			{
				dirPath += _T("\\");
			}
		}
		return dirPath;
	}

	static CString GetUsabelFileName(CString strFile)
	{
		CString strUsable = strFile;
		CString strExtension = GetExtension(strFile);
		strFile = RemoveExtension(strFile);
		for (int i = 1; i < 1000 && GetFileAttributes(strUsable) != INVALID_FILE_ATTRIBUTES; i++)
		{
			strUsable.Format(_T("%s(%d)%s"), strFile, i, strExtension);
		}
		return strUsable;
	}
	static CString GetTempFileName()
	{
		TCHAR szTmpDir[MAX_PATH] = {0};
		TCHAR szTmpName[MAX_PATH] = {0};
		TCHAR szPrefix[4] = _T("~QP");
		::GetTempPath(MAX_PATH,szTmpDir);
		::GetTempFileName(szTmpDir,szPrefix,0,szTmpName);
		return szTmpName;
	}
	static BOOL IsJpg(CString strFile)
	{
		CString extension = GetExtension(strFile);
		if(extension.CompareNoCase(_T(".jpg")) == 0 || extension.CompareNoCase(_T(".jpeg")) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	static BOOL IsBmp(CString strFile)
	{
		if(GetExtension(strFile).CompareNoCase(_T(".bmp")) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	static BOOL IsGif(CString strFile)
	{
		if(GetExtension(strFile).CompareNoCase(_T(".gif")) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}
	static BOOL FindFileName(CString strDirPath, std::vector<CString> &fileNames)
	{
		if(strDirPath[strDirPath.GetLength()-1] != '\\')
		{
			strDirPath += _T("\\");
		}
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(strDirPath+_T("*.*"),&findFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return FALSE;
		}

		if( !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			fileNames.push_back(findFileData.cFileName);
		}
		while (::FindNextFile(hFind, &findFileData))
		{
			if( !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				fileNames.push_back(findFileData.cFileName);
			}
		}
		::FindClose(hFind);
		return TRUE;
	}

	static BOOL FindSubDirName(const CString &strDirPath, std::vector<CString> &vSubDirName)
	{
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(strDirPath+_T("*.*"),&findFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return FALSE;
		}

		if( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (0 != lstrcmp(findFileData.cFileName, _T(".")) && 0 != lstrcmp(findFileData.cFileName, _T("..")))
			{
				vSubDirName.push_back(findFileData.cFileName);
			}
		}
		while (::FindNextFile(hFind, &findFileData))
		{
			if((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (0 != lstrcmp(findFileData.cFileName, _T(".")) && 0 != lstrcmp(findFileData.cFileName, _T("..")))
				{
					vSubDirName.push_back(findFileData.cFileName);
				}
			}
		}
		::FindClose(hFind);
		return TRUE;

	}
	static ULONGLONG GetDirectorySize(CString strDir)		//获取目录占用大小
	{
		strDir.TrimRight(_T("\\"));
		strDir += _T("\\");

		ULONGLONG lSize = 0;
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(strDir+_T("*.*"), &findFileData);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return lSize;
		}
		if( !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (0 != lstrcmp(findFileData.cFileName, _T(".")) && 0 != lstrcmp(findFileData.cFileName, _T("..")))
			{
				lSize += GetDirectorySize(strDir + findFileData.cFileName);
			}
		}
		else
		{
			lSize += MAKEI64(findFileData.nFileSizeHigh, findFileData.nFileSizeLow);
		}
		while (::FindNextFile(hFind, &findFileData))
		{
			if( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (0 != lstrcmp(findFileData.cFileName, _T(".")) && 0 != lstrcmp(findFileData.cFileName, _T("..")))
				{
					lSize += GetDirectorySize(strDir + findFileData.cFileName);
				}
			}
			else
			{
				lSize += MAKEI64(findFileData.nFileSizeHigh, findFileData.nFileSizeLow);
			}
		}
		::FindClose(hFind);
		return lSize;
	}

	typedef struct _DISKINFO
	{
		TCHAR cDisk;
		__int64 iSpace;
	}DISKINFO;
	//获取剩余空间最大的磁盘
	/*****************************************************
	*
	*Description:  GetMaxDisk: 若只有两个分区，当系统盘符剩余空间大于另一盘符剩余空间4倍的，则为系统盘符，否则设为另一盘符
							   若有两个以上分区，获得除系统盘外剩余空间最大的磁盘

	*Input:
	*Output	 pSpace: 剩余空间
	*Return: 盘符
	*
	********************************************************/
	static TCHAR GetMaxDisk(__int64* pSpace)
	{
		TCHAR drive[4] = {0};
		TCHAR tmpDisk = _T('A');
		TCHAR locDisk = _T('A');
		TCHAR systemDir[MAX_PATH] = {0};
		GetSystemDirectory(systemDir,MAX_PATH);
		TCHAR sysDisk = systemDir[0];

		int nSys = 0;
		ULARGE_INTEGER nDiskSize[2] = {0};
		ULARGE_INTEGER i64FreeBytesToCaller;
		ULARGE_INTEGER i64TotalBytes;
		ULARGE_INTEGER i64FreeBytes;
		__int64 iLocalSpace = 0;

		std::vector<DISKINFO> vDisk;
		DWORD dwRtn = GetLogicalDrives();

		while ( dwRtn )
		{
			if ( dwRtn & 1)
			{
				_stprintf_s(drive,4,_T("%C:\\"),tmpDisk);
				if (GetDriveType(drive) == DRIVE_FIXED)
				{
					BOOL fResult = GetDiskFreeSpaceEx(drive,
						(PULARGE_INTEGER)&i64FreeBytesToCaller,
						(PULARGE_INTEGER)&i64TotalBytes,
						(PULARGE_INTEGER)&i64FreeBytes);
		
					if (fResult)
					{
						TCHAR szFileSystem[100];
						if (0 != GetVolumeInformation(drive, NULL, 0, NULL, 0, NULL , szFileSystem, 100))
						{
							CString strFileSystem = szFileSystem;
							strFileSystem.MakeUpper();
							if (-1 == strFileSystem.Find(_T("FAT")) && -1== strFileSystem.Find(_T("NTFS")))	//排除非windows文件系统格式的磁盘
							{
								fResult = FALSE;
							}
						}
					}
					if (fResult)
					{
						DISKINFO disk;
						disk.cDisk = tmpDisk;
						disk.iSpace = MAKEI64(i64FreeBytes.HighPart, i64FreeBytes.LowPart);
						if (tmpDisk == sysDisk)
						{
							nSys = vDisk.size();
						}
						vDisk.push_back(disk);
					}
				}
			}
			tmpDisk++;
			dwRtn >>= 1;
		}

		int nDiskCount = vDisk.size();
		if (nDiskCount == 1)
		{
			locDisk = vDisk[0].cDisk;
			iLocalSpace = vDisk[0].iSpace;
		}
		else if (nDiskCount == 2)
		{
			int nDest = (vDisk[nSys].iSpace > 4 * vDisk[!nSys].iSpace) ? nSys : !nSys;
			locDisk = vDisk[nDest].cDisk;
			iLocalSpace = vDisk[nDest].iSpace;
		}
		else if (nDiskCount > 2)
		{
			for (int i = 0; i < nDiskCount; i++)
			{
				if (vDisk[i].cDisk != sysDisk)
				{
					if (vDisk[i].iSpace > iLocalSpace)
					{
						locDisk = vDisk[i].cDisk;
						iLocalSpace = vDisk[i].iSpace;
					}
				}
			}
		}
		if (pSpace )
		{
			*pSpace = iLocalSpace;
		}
		return locDisk;
	}

	static INT64 GetDiskFreeSpace(LPCTSTR lpDriver)
	{
		INT64 iSpace = 0;
		if (NULL == lpDriver || 0 == _tcslen(lpDriver)) return 0;
		CString strDriver;
		strDriver.Format(_T("%C:\\"), lpDriver[0]);


		ULARGE_INTEGER i64FreeBytesToCaller;
		ULARGE_INTEGER i64TotalBytes;
		ULARGE_INTEGER i64FreeBytes;

		BOOL fResult = GetDiskFreeSpaceEx(strDriver,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,
			(PULARGE_INTEGER)&i64FreeBytes);
		if (fResult)
		{
			iSpace = MAKEI64(i64FreeBytes.HighPart, i64FreeBytes.LowPart);
		}

		return iSpace;
	}
};


class Stream
{
public:
	Stream(CString filePath,
		   DWORD dwDesiredAccess = GENERIC_READ|GENERIC_WRITE,
		   DWORD dwShareMode = 0,
		   DWORD dwCreationDisposition = OPEN_ALWAYS,
		   DWORD dwFlagsAndAttributes =0)
	{
		m_hFile = ::CreateFile(filePath,
							   dwDesiredAccess, 
							   dwShareMode, 
							   NULL,
							   dwCreationDisposition,
							   dwFlagsAndAttributes,
							   NULL);
	}
	~Stream()
	{
		Close();
	}
	BOOL IsOpen()
	{
		return (INVALID_HANDLE_VALUE != m_hFile);
	}
	UINT64 GetSize()
	{
		UINT64 fileSize = 0;
		if(IsOpen())
		{
			DWORD highSize = 0;
			DWORD lowSize = GetFileSize(m_hFile, &highSize);
			fileSize = lowSize + (((UINT64)highSize)<<32);
		}
		return fileSize	;
	}
	__int64 ReadFile(std::string& buffer)
	{
		if(!IsOpen())
		{
			return QFILE_STREAM_ERROR;
		}
		UINT64 size = GetSize(); 
		char *szBuffer = new char[size];
		ReadBlock(szBuffer,size);
		buffer.assign(szBuffer,size);
		delete []szBuffer;
		return size;
	}
	__int64 ReadBlock(char *buffer, DWORD size)
	{
		if(!IsOpen() || !buffer)
		{
			return QFILE_STREAM_ERROR;
		}
		DWORD dwNumberOfBytesRead = 0;
		::ReadFile(m_hFile,buffer,size,&dwNumberOfBytesRead,NULL);
		return dwNumberOfBytesRead;
	}
	__int64 WriteBlock(std::string &buffer)
	{
		return WriteBlock(buffer.c_str(), buffer.length());
	}
	__int64 WriteBlock(const char* buffer, const DWORD& len)
	{
		if(!IsOpen() || len <= 0)
		{
			return QFILE_STREAM_ERROR;
		}
		DWORD dwNumberOfBytesWrite = 0;
		::WriteFile(m_hFile,buffer, len,&dwNumberOfBytesWrite,NULL);
		return dwNumberOfBytesWrite;
	}
	void Seek(__int64 offset, DWORD way)
	{
		if(IsOpen())
		{
			LARGE_INTEGER liCurrentPosition = {0};
			liCurrentPosition.QuadPart = offset;
			SetFilePointerEx(m_hFile,liCurrentPosition,&liCurrentPosition,way);
		}
	}
	void Close()
	{
		if (NULL != m_hFile)
		{
			CloseHandle(m_hFile);
		}	
		m_hFile = NULL;
	}
private:
	CString m_filePath;
	HANDLE  m_hFile;
};
}



#endif