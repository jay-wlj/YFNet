//Inifile.h
#ifndef _INIFILE_H_
#define _INIFILE_H_
class CIniFile
{
public:
	CIniFile() {/* m_Filename = NULL;*/}
	CIniFile(const TCHAR* Filename);
//	virtual ~CIniFile() { Free(); }
	void Load(const TCHAR* Filename);
	int ReadString(const TCHAR* m_Sec, const TCHAR* m_Ident, TCHAR* cValue, int nLenValue);
	int ReadInt(const TCHAR* lpAppName, const TCHAR* lpKeyName, int nDefault = 0);
	bool WriteString(const TCHAR* m_Sec, const TCHAR* m_Ident, const TCHAR* m_Val);
private:
// 	void Free()
// 	{
// 		if(m_Filename != NULL)
// 			delete []m_Filename;
// 		m_Filename = NULL;
// 	}
private:
	CString m_Filename;
};
#endif