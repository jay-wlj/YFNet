#ifndef COMMON_VERSION_H
#define COMMON_VERSION_H
#pragma comment(lib, "Version.lib")
namespace QVersion{

class Tool
{
public:
	static BOOL GetFileVersionInt(CString filePath, int version[4])
	{
		CString szVersion;
		DWORD dwHandle = 0;

		DWORD dwFileVersionInfoSize = GetFileVersionInfoSize(filePath, &dwHandle);
		if(dwFileVersionInfoSize == 0)
		{
			return FALSE;
		}
		BYTE *lpData = new BYTE[dwFileVersionInfoSize];
		if(lpData == NULL)
		{
			return FALSE;
		}
		if (!GetFileVersionInfo(filePath, dwHandle, dwFileVersionInfoSize, lpData))
		{
			delete []lpData;
			return FALSE;
		}
		UINT unInfoLen = 0;
		VS_FIXEDFILEINFO* vsFileInfo = 0;
		if (! VerQueryValue(lpData, _T("\\"), (LPVOID*)&vsFileInfo, &unInfoLen))
		{
			delete []lpData;
			return FALSE;
		}
		if (unInfoLen != sizeof(VS_FIXEDFILEINFO))
		{
			delete []lpData;
			return FALSE;
		}
		// 版本信息 a.b.c.d
		version[0] = HIWORD(vsFileInfo->dwFileVersionMS);
		version[1] = LOWORD(vsFileInfo->dwFileVersionMS);
		version[2] = HIWORD(vsFileInfo->dwFileVersionLS);
		version[3] = LOWORD(vsFileInfo->dwFileVersionLS);
		delete []lpData;
		return TRUE;
	}
	static CString GetFileVersionString(CString filePath)
	{
		CString strVer;
		int intVer[4] = {0};
		if(GetFileVersionInt(filePath,intVer))
		{
			strVer.Format(_T("%d.%d.%d.%d"),intVer[0],intVer[1],intVer[2],intVer[3]);
		}
		return strVer;
	}
	static BOOL GetProcessVersionInt(int version[4])
	{
		TCHAR szProcessPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szProcessPath, MAX_PATH);
		return GetFileVersionInt(szProcessPath,version);
	}
	static CString GetProcessVersionString()
	{
		int intVer[4] = {0};
		GetProcessVersionInt(intVer);
		CString version;
		version.Format(_T("%d.%d.%d.%d"),intVer[0],intVer[1],intVer[2],intVer[3]);
		return version;
	}

	static CString GetSystemVersion()
	{
		CString version;
		OSVERSIONINFOEX	V = {sizeof(OSVERSIONINFOEX)};	//EX for NT 5.0 and later

		if (!GetVersionEx((POSVERSIONINFO)&V))
		{
			ZeroMemory(&V, sizeof(V));
			V.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx((POSVERSIONINFO)&V);
		}

		if (V.dwPlatformId != VER_PLATFORM_WIN32_NT)
			V.dwBuildNumber = LOWORD(V.dwBuildNumber);	//for 9x HIWORD(dwBuildNumber) = 0x04xx

		version.Format(_T("%d.%d.%d"),V.dwMajorVersion, V.dwMinorVersion, V.dwBuildNumber);
		return version;
	}

	// 比较版本号，大于0表示pszVer1大于版本2
	static int CompareVersion(LPCTSTR pszVer1, LPCTSTR pszVer2,int nStartIndex = 0,int nCount = 4)
	{
		if( 0 > nStartIndex || nStartIndex >= 4 || 0 > nCount || nCount + nStartIndex >= 4)
		{
			nStartIndex = 0;
			nCount = 4;
		}
		int ver1[4] = {0};
		int ver2[4] = {0};
		_stscanf_s(pszVer1, _T("%d.%d.%d.%d"), &ver1[0], &ver1[1], &ver1[2], &ver1[3]);
		_stscanf_s(pszVer2, _T("%d.%d.%d.%d"), &ver2[0], &ver2[1], &ver2[2], &ver2[3]);
		for(long i=nStartIndex;i<nCount;i++)
		{
			if(ver1[i]>ver2[i])
			{
				return 1;
			}
			if(ver1[i]<ver2[i])
			{
				return -1;
			}
		}
		return 0;
	}
};

} // namespace QVersion
#endif