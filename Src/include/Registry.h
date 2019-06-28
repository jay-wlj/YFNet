#ifndef __Registry_H__
#define __Registry_H__

#include <Windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <vector>
using std::vector;

typedef struct s_REGSTRUCT{
	HKEY  hRootKey;
	LPTSTR szSubKey;
	LPTSTR lpszValueName;
	LPTSTR szData;
}REGSTRUCT, *LPREGSTRUCT;

class CRegistry
{
public:
#define DEFAULTKEY HKEY_CURRENT_USER
	CRegistry()
	{
		hKey_ = DEFAULTKEY;
	}
	CRegistry(HKEY hKey)
	{
		hKey_ = hKey;
	}
	CRegistry(LPCTSTR prefix)
	{
		hKey_ = DEFAULTKEY;
		// ÎªÉ¶ÕâÑù¿¼×Ö·û´®£¿
		if (NULL != prefix)
		{
			m_strPreFixKey = prefix;
		}
	}
	CRegistry(HKEY hKey, LPCTSTR prefix)
	{
		hKey_ = hKey;
		if (NULL != prefix)
		{
			m_strPreFixKey = prefix;
		}
	}

	BOOL IsKeyExist(LPCTSTR pszSection = NULL)
	{
		LONG nErrorCode;
		HKEY hKey;
		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}
		nErrorCode = ::RegOpenKeyEx( hKey_, (LPCTSTR)strKey, 0, KEY_READ, &hKey );
		if ( nErrorCode == ERROR_SUCCESS )
		{
			::RegCloseKey( hKey );
			return TRUE;
		}
		return FALSE;
	}

	BOOL GetString(LPCTSTR pszSection, LPCTSTR pszName, CString& strOut )
	{
		BOOL blnResult = FALSE;
		LONG nErrorCode;
		HKEY hKey;
		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}

		nErrorCode = ::RegOpenKeyEx( hKey_, strKey, 0, KEY_READ, &hKey );
		if ( nErrorCode == ERROR_SUCCESS )
		{
			DWORD nType = 0;
			DWORD nSize = 0;
			nErrorCode = ::RegQueryValueEx( hKey, pszName, 0, &nType, NULL, &nSize);
			TCHAR *szBuf = new TCHAR[nSize+1]();
			if ( nErrorCode == ERROR_SUCCESS && (nType == REG_SZ || nType == REG_EXPAND_SZ || nType == REG_MULTI_SZ)&& nSize >= sizeof(TCHAR) )
			{
				nErrorCode = ::RegQueryValueEx( hKey, pszName, 0, &nType, (PBYTE)szBuf, &nSize );
				if ( nErrorCode == ERROR_SUCCESS )
				{
					strOut = szBuf;
					blnResult = TRUE;
				}
			}
			SafeDeleteArray(szBuf);
			::RegCloseKey( hKey );
		}
		return blnResult;
	}
	
	BOOL GetString(LPCTSTR pszSection, LPCTSTR pszName, LPTSTR pszBufferOut, DWORD nLen )
	{
		BOOL blnResult = FALSE;
		LONG nErrorCode;
		HKEY hKey;

		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}

		nErrorCode = ::RegOpenKeyEx( hKey_, strKey, 0, KEY_READ, &hKey );
		if ( nErrorCode == ERROR_SUCCESS )
		{
			DWORD nType = 0;
			DWORD nSize = 0;
			nErrorCode = ::RegQueryValueEx( hKey, pszName, 0, &nType, NULL, &nSize);
			if ( nErrorCode == ERROR_SUCCESS && (nType == REG_SZ || nType == REG_EXPAND_SZ || nType == REG_MULTI_SZ)&& nSize >= sizeof(TCHAR) )
			{
				nErrorCode = ::RegQueryValueEx( hKey, pszName, 0, &nType, (PBYTE)pszBufferOut, &nLen );
				if ( nErrorCode == ERROR_SUCCESS )
				{
					blnResult = TRUE;
				}
			}
			::RegCloseKey( hKey );
		}
		return blnResult;
	}

	// CRegistry read an integer value
	int GetInt(LPCTSTR pszSection, LPCTSTR pszName, int nDefault)
	{
		int nValue = nDefault;
		DWORD nErrorCode;
		HKEY hKey;

		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}

		nErrorCode = ::RegOpenKeyEx( hKey_, strKey, 0, KEY_READ, &hKey );
		if ( nErrorCode == ERROR_SUCCESS )
		{
			DWORD nType = 0, nSize = sizeof(nValue);
			nErrorCode = ::RegQueryValueEx( hKey, pszName, 0, &nType, (PBYTE)&nValue, &nSize );
			if ( nType != REG_DWORD || nSize != sizeof(nValue) )
			{
				nErrorCode = ERROR_MORE_DATA;
				nValue = nDefault;
			}
			::RegCloseKey( hKey );
		}
		return nValue;
	}

	BOOL SetInt(LPCTSTR pszSection, LPCTSTR pszName, int nValue)
	{
		DWORD nErrorCode;
		HKEY hKey;

		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}

		nErrorCode = ::RegCreateKeyEx( hKey_, strKey, 0,
			NULL, 0, KEY_WRITE, NULL, &hKey, NULL );
		if ( nErrorCode == ERROR_SUCCESS )
		{
			nErrorCode = RegSetValueEx( hKey, pszName, 0,
				REG_DWORD,
				(const BYTE *)&nValue,
				sizeof(nValue) );

			::RegCloseKey( hKey );
		}
		return ERROR_SUCCESS == nErrorCode ? TRUE : FALSE;
	}

	BOOL DropSubKey(LPCTSTR pszSection)
	{
		return DropSubKey( pszSection, _T("") );
	}

	BOOL DropSubKey(LPCTSTR pszSection, LPCTSTR pszSubKey)
	{
		DWORD nErrorCode;
		HKEY hKey;

		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}

		nErrorCode = ::RegOpenKeyEx( hKey_, strKey, 0, KEY_WRITE, &hKey );
		if ( nErrorCode == ERROR_SUCCESS )
		{
			nErrorCode = ::RegDeleteKey( hKey, pszSubKey );
			::RegCloseKey( hKey );
		}
		return ERROR_SUCCESS == nErrorCode ? TRUE : FALSE;
	}

	BOOL DropValue(LPCTSTR pszSection, LPCTSTR pszName)
	{
		DWORD nErrorCode;
		HKEY hKey;

		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}

		nErrorCode = ::RegOpenKeyEx( hKey_, strKey, 0, KEY_WRITE, &hKey );
		if ( nErrorCode == ERROR_SUCCESS )
		{
			nErrorCode = ::RegDeleteValue( hKey, pszName );
			::RegCloseKey( hKey );
		}
		return ERROR_SUCCESS == nErrorCode ? TRUE : FALSE;
	}

	BOOL SetString(LPCTSTR pszSection, LPCTSTR pszName, int nValue)
	{
		TCHAR szBuffer[100] = {0};
		_stprintf_s( szBuffer, _T("%i"), nValue );
		return SetString( pszSection, pszName, szBuffer );
	}

	BOOL SetString(LPCTSTR pszSection, LPCTSTR pszName, LPCTSTR pszValue)
	{
		DWORD nErrorCode;
		HKEY hKey;

		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}

		nErrorCode = ::RegCreateKeyEx( hKey_, strKey, 0,
			NULL, 0, KEY_WRITE, NULL, &hKey, NULL );
		if ( nErrorCode == ERROR_SUCCESS )
		{
			nErrorCode = ::RegSetValueEx( hKey, pszName, 0,
				REG_SZ,
				(const BYTE *)pszValue,
				_tcslen(pszValue) * sizeof(TCHAR) + sizeof(TCHAR) );
			::RegCloseKey( hKey );
		}
		return ERROR_SUCCESS == nErrorCode ? TRUE : FALSE;
	}

	BOOL CreateSubKey(LPCTSTR pszSection, LPCTSTR pszSubKey)
	{
		DWORD nErrorCode;
		HKEY hKey;

		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}

		nErrorCode = ::RegOpenKeyEx( hKey_, strKey, 0, KEY_WRITE, &hKey );
		if ( nErrorCode == ERROR_SUCCESS )
		{
			HKEY hKeyCreate;
			nErrorCode = ::RegCreateKey( hKey, pszSubKey, &hKeyCreate );
			if( ERROR_SUCCESS == nErrorCode )
			{
				::RegCloseKey( hKeyCreate );
			}
			::RegCloseKey( hKey );
		}
		return ERROR_SUCCESS == nErrorCode ? TRUE : FALSE;
	}

	BOOL GetEnumValues(LPCTSTR pszSection, vector<CString> &vValues)
	{
		DWORD nErrorCode = 0;
		HKEY hKey;

		CString strKey = m_strPreFixKey;
		if (NULL != pszSection)
		{
			strKey += pszSection;
		}

		nErrorCode = ::RegOpenKeyEx( hKey_, strKey, 0, KEY_READ, &hKey);
		if ( nErrorCode == ERROR_SUCCESS )
		{
			TCHAR szValues[MAX_PATH] = {0};
			DWORD dwValuesLen = MAX_PATH;
			DWORD dwIndex = -1;
			long lResult = 0;
			while (1)
			{
				dwIndex++;

				dwValuesLen = MAX_PATH;
				lResult = ::RegEnumValue(hKey, dwIndex, szValues, &dwValuesLen, NULL, NULL, NULL, NULL);
				if (lResult != ERROR_SUCCESS)
				{
					break;	//if the End then break;
				}

				vValues.push_back(szValues);
			}

			::RegCloseKey( hKey );
		}
		return ERROR_SUCCESS == nErrorCode ? true : false;
	}

protected:
	HKEY	hKey_;
	CString m_strPreFixKey;
};

#endif //__Registry_H__
