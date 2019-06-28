// 描述    ：INI文件读写类
// 当前版本：
// 作    者：匡晓林
// 完成日期：

#ifndef _INI_H_H__
#define _INI_H_H__

#include "StringTool.h"

namespace common{

// INI配置文件中的section
class IniSection
{
public:
	IniSection(LPCTSTR lpFilePath, LPCTSTR lpSectionName)
	{
		m_filePath = lpFilePath;
		m_sectionName = lpSectionName;
	}

public:
	int	GetKeyValueInt32(const CString& _Key, int _Default = 0)
	{
		return GetPrivateProfileInt(m_sectionName, _Key, _Default, m_filePath);
	}

	__int64 GetKeyValueInt64(const CString& _Key, __int64 _Default = 0)
	{
		CString str64 =  GetKeyValueString(_Key);
		if(str64.GetAllocLength() > 0)
		{
			return Convert::ToInt64(str64);
		}
		return _Default;
	}

	CString GetKeyValueString(const CString& _Key,const CString& _Default = _T(""))
	{
		TCHAR tzBuffer[MAX_PATH+1];
		::GetPrivateProfileString(m_sectionName, _Key, NULL, tzBuffer, MAX_PATH, m_filePath);
		if(!tzBuffer[0])
		{
			return _Default;
		}
		return CString(tzBuffer);
	}

	void SetKeyValue(const CString& _Key, int _Value)
	{
		SetKeyValue(_Key, Convert::ToString(_Value));
	}

	void SetKeyValue(const CString& _Key, __int64 _Value)
	{
		SetKeyValue(_Key, Convert::ToString(_Value));
	}

	void SetKeyValue(const CString& _Key,const CString& _Value)
	{
		::WritePrivateProfileString(m_sectionName, _Key, _Value, m_filePath);
	}
	void DeleteKey(const CString& _Key)
	{
		::WritePrivateProfileString(m_sectionName, _Key, NULL, m_filePath);
	}
	void Delete()
	{
		::WritePrivateProfileString(m_sectionName, NULL, NULL, m_filePath);
	}
private:
	CString m_filePath;
	CString m_sectionName;
};
}

#endif