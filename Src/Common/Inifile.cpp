#include "qvod_string.h"
#include "common-inifile.h"

CIniFile::CIniFile(const char* Filename)
{
	m_Filename = NULL;
	Load(Filename);
}

void CIniFile::Load(const char* Filename)
{
	if(NULL == Filename)return;
	Free();
	int nLen = strlen(Filename) + 1;
	m_Filename = new char[nLen];
	memset(m_Filename, 0, nLen);
	strcpy_s(m_Filename,nLen,Filename);
}

int CIniFile::ReadString(const char* m_Sec, const char* m_Ident, char* cValue, int nLenValue)
{
	int nResult = 0;
	nResult = ::QvodGetPrivateProfileString(m_Sec, m_Ident, "", cValue, nLenValue, m_Filename);
	return nResult;
}

int CIniFile::ReadInt(const char* lpAppName, const char* lpKeyName,int nDefault)
{
	int nResult = nDefault;
	nResult = ::QvodGetPrivateProfileInt(lpAppName, lpKeyName, nDefault, m_Filename);
	return nResult;
}

bool CIniFile::WriteString(const char* m_Sec, const char* m_Ident, const char* m_Val)
{
	if(::QvodWritePrivateProfileString(m_Sec, m_Ident, m_Val, m_Filename))
		return true;
	return false;
}

