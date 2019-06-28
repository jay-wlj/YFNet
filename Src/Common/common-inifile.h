//Inifile.h
#ifndef _INIFILE_H_
#define _INIFILE_H_

#ifndef SafeDelete
#define SafeDelete(pObject)				if(pObject) {delete pObject; pObject = NULL;}
#endif
#ifndef SafeDeleteArray
#define SafeDeleteArray(p)					if(p){delete []p; p = NULL;}
#endif

class CIniFile
{
public:
	CIniFile() { m_Filename = NULL;}
	CIniFile(const char* Filename);
	virtual ~CIniFile() { Free(); }
	void Load(const char* Filename);
	int ReadString(const char* m_Sec, const char* m_Ident, char* cValue, int nLenValue);
	int ReadInt(const char* lpAppName, const char* lpKeyName, int nDefault = 0);
	bool WriteString(const char* m_Sec, const char* m_Ident, const char* m_Val);
private:
	void Free()
	{
		SafeDeleteArray(m_Filename);
	}
private:
	char* m_Filename;
};
#endif

