#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "inifile.H"

CIniFile::CIniFile(const TCHAR* Filename)
{
//	m_Filename = NULL;
	Load(Filename);
}

void CIniFile::Load(const TCHAR* Filename)
{
	if(NULL == Filename)return;
	m_Filename = Filename;
//	Free();
//	int nLen = _tcslen(Filename) + 1;
	
//	m_Filename = new TCHAR[nLen];
//	memset(m_Filename, 0, nLen);
//	_tcscpy(m_Filename, Filename);
}

int CIniFile::ReadString(const TCHAR* m_Sec, const TCHAR* m_Ident, TCHAR* cValue, int nLenValue)
{
	int nResult = 0;
	nResult = ::GetPrivateProfileString(m_Sec, m_Ident, _T(""), cValue, nLenValue, m_Filename);
	return nResult;
}

int CIniFile::ReadInt(const TCHAR* lpAppName, const TCHAR* lpKeyName,int nDefault)
{
	int nResult = nDefault;
	nResult = ::GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, m_Filename);
	return nResult;
}

bool CIniFile::WriteString(const TCHAR* m_Sec, const TCHAR* m_Ident, const TCHAR* m_Val)
{
	if(::WritePrivateProfileString(m_Sec, m_Ident, m_Val, m_Filename))
		return true;
	return false;
}
