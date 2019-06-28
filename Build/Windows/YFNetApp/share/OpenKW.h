#pragma once

#include <Shellapi.h>
#pragma comment(lib, "Version.lib")

class COpenKW
{
public:
    enum KW_BROWSER_MODE
    {
        bm_simple = 1, // 普通模式
        bm_game,       // 游戏模式
        bm_unkown,     // 未定义模式
    };

    COpenKW(HINSTANCE hInst = 0)
    {
        if ( m_wSwitchStatus & 1 ) return;
        //get data path
        TCHAR dPath[_MAX_PATH] = {0};
        OSVERSIONINFO osvi = { sizeof(OSVERSIONINFO) };
        if ( GetVersionEx(&osvi) && 6 <= osvi.dwMajorVersion )
        {
            GetEnvironmentVariable( _T("ALLUSERSPROFILE"), dPath, _MAX_PATH );
            _tcsncat(dPath, _T("\\QvodPlayer\\"), 12);
        }
        else
        {
            GetModuleFileName(hInst, dPath, MAX_PATH);
            *(_tcsrchr(dPath, '\\')+1)=0;
        }
        CString path = dPath;
        path += _T("Playlist\\Text.xml");
        CXMLDoc doc;
        CXMLElement root, startkw;
        if ( PreEncodedXml(path, doc, root) && root.SelectSingleNode_(_T("StartupKW"), startkw) )
        {
            m_wSwitchStatus = startkw.GetAttribute_bool(_T("enable"), TRUE) ? 0x0100 : 0;
        }
        m_wSwitchStatus |= 1;
        doc.Clear_();
    }

    HRESULT KWOpenURL(LPCTSTR lpURL, KW_BROWSER_MODE mode = bm_simple)
    {
        if ( ( m_wSwitchStatus & 0xff00 ) && bm_unkown != mode && KWIsEnablePopWeb() )
        {
            CString strParam; //使用API方式打开页面
            strParam.Format(_T("-apinameparam.OpenSimpleURL|%s|%d"), lpURL, mode);
            return StartKW(strParam);
        }
        OpenURL(lpURL);
        return S_OK;
    }
	BOOL RunKWAndOpenUrl(LPCTSTR lpURL)
	{
		if (KWIsEnablePopWeb() )
		{
			CString strParam; //使用API方式打开页面
			strParam.Format(_T("-apinameparam.OpenSimpleURL|%s|%d"), lpURL, bm_simple);
			::ShellExecute( 0, _T("open"), KWGetPath(), strParam, 0, SW_SHOW );
			return TRUE;
		}
		return FALSE;
	}

    //启动快玩
    HRESULT StartKW(LPCTSTR lpParam)
    {
        if ( m_wSwitchStatus & 0xff00 )
        {
            CString strPath = KWGetPath();
            if ( strPath.IsEmpty() ) return FALSE;
            return (HRESULT)ShellExecute( 0, _T("open"), strPath, lpParam, 0, SW_SHOW );
        }
        return S_OK;
    }

    //获取快玩路径
    static CString KWGetPath(void)
    {
        TCHAR szPath[MAX_PATH] = {0};
        HKEY hKey = NULL;
        RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Kuaiwan"), 0, KEY_READ, &hKey);
        if ( hKey )
        {
            DWORD dwSize = sizeof(TCHAR)*MAX_PATH;
            DWORD dwType = REG_SZ;
            RegQueryValueEx(hKey, _T("path"), 0, &dwType, (BYTE*)szPath, &dwSize);
            RegCloseKey(hKey);
        }
        long len = lstrlen(szPath);
        if ( 0 == len ) return _T("");
        if ( szPath[len - 1] != '\\' )
        {
            lstrcat(szPath, _T("\\"));
        }
        lstrcat(szPath, _T("Kuaiwan.exe"));
        CString strRet;
        if ( INVALID_FILE_ATTRIBUTES != GetFileAttributes(szPath) )
        {
            strRet = szPath;
        }
        return strRet;
    }

    static float KWGetLfVersion(LPCTSTR pFilePath)
    {
        // 信息块大小
        float lfVersion = 0;
        DWORD dwSize = GetFileVersionInfoSize(pFilePath, 0);
        if ( 0 == dwSize ) return lfVersion;
        if ( BYTE *lpData = new BYTE[dwSize] )
        {
            // 查询版本号信息
            UINT unInfoLen = 0;
            VS_FIXEDFILEINFO* vsFileInfo = 0;
            if ( GetFileVersionInfo(pFilePath, 0, dwSize, lpData)
                && VerQueryValue(lpData, _T("\\"), (LPVOID*)&vsFileInfo, &unInfoLen)
                && sizeof(VS_FIXEDFILEINFO) == unInfoLen )
            {
                // 版本信息 a.b.c.d
                DWORD dwm = vsFileInfo->dwFileVersionMS;
                DWORD dwl = vsFileInfo->dwFileVersionLS;
                CString szVersion;
                szVersion.Format(_T("%d.%d%d%d"), HIWORD(dwm), LOWORD(dwm), HIWORD(dwl), LOWORD(dwl));
                lfVersion = _tstof(szVersion);
            }
            delete []lpData;
        }
        return lfVersion;
    }

    static CString KWGetSoftWareVersion(LPCTSTR pFilePath)
    {
        CString szVersion;
        // 信息块大小
        DWORD dwSize = GetFileVersionInfoSize(pFilePath, 0);
        if ( 0 == dwSize ) return szVersion;
        if( BYTE *lpData = new BYTE[dwSize] )
        {
            // 查询版本号信息
            UINT unInfoLen = 0;
            VS_FIXEDFILEINFO* vsFileInfo = 0;
            if ( GetFileVersionInfo(pFilePath, 0, dwSize, lpData)
                && VerQueryValue(lpData, _T("\\"), (LPVOID*)&vsFileInfo, &unInfoLen)
                && sizeof(VS_FIXEDFILEINFO) == unInfoLen )
            {
                // 版本信息 a.b.c.d
                DWORD dwm = vsFileInfo->dwFileVersionMS;
                DWORD dwl = vsFileInfo->dwFileVersionLS;
                szVersion.Format(_T("%d.%d.%d.%d"), HIWORD(dwm), LOWORD(dwm), HIWORD(dwl), LOWORD(dwl));
            }
            delete []lpData;
        }
        return szVersion;
    }

    static BOOL KWIsEnablePopWeb(void)
    {
        CString strPath = KWGetPath();
        if ( strPath.IsEmpty() ) return FALSE;
        return ( 0 <= KWGetSoftWareVersion(strPath).Compare( _T("3.1.0.2") ) ) ? TRUE : FALSE;
    }

protected:
    BOOL PreEncodedXml(CString & path, CXMLDoc & doc, CXMLElement & root)
    {
        // Text.xml解密
        if ( path.IsEmpty() || INVALID_FILE_ATTRIBUTES == GetFileAttributes(path) || !doc.Init_() ) return FALSE;
        BYTE * pbyte = new BYTE[En_DeFile::GetFileSize(path)+2]();
        if ( !pbyte ) return FALSE;
        UINT outSize = 0;
        BOOL b = En_DeFile::DecodeFile(path, pbyte, &outSize) && doc.LoadXml_((TCHAR*)pbyte) && doc.GetDocumentElement_(root);
        delete []pbyte;
        return b;
    }
    static WORD m_wSwitchStatus;
};
__declspec(selectany) WORD COpenKW::m_wSwitchStatus = 0x0100;
