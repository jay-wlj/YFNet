#ifndef BinaryWriter_INCLUDED
#define BinaryWriter_INCLUDED

#include "qvod_sock.h"
#include "qvod_basetypes.h"
#include <ostream>
using namespace std;

class  BinaryWriter
{
public:
	
	BinaryWriter();

	~BinaryWriter();

	BinaryWriter& operator << (bool value);
	BinaryWriter& operator << (char value);
	BinaryWriter& operator << (unsigned char value);
	BinaryWriter& operator << (signed char value);
	BinaryWriter& operator << (short value);
	BinaryWriter& operator << (unsigned short value);
	BinaryWriter& operator << (int value);
	BinaryWriter& operator << (unsigned int value);
	BinaryWriter& operator << (long value);
	BinaryWriter& operator << (unsigned long value);
	BinaryWriter& operator << (float value);
	BinaryWriter& operator << (double value);
	BinaryWriter& operator << (long long value);
	BinaryWriter& operator << (unsigned long long value);


	BinaryWriter& operator << (const std::string& value);
	BinaryWriter& operator << (const std::wstring& value);
	BinaryWriter& operator << (const char* value);
	BinaryWriter& operator << (const wchar_t *value);

	void writeRaw(const char* value,int length);
	void writeRaw(const wchar_t* value,int length);
	void Realloc(const char* pBuf,int iSize);

	char* GetData() 
	{ 
		int iTmpLen =htonl(m_iLen);
		memcpy(m_pHead,&iTmpLen,sizeof(iTmpLen));
		return m_pHead;
	}

	int GetLength() 
	{
		return m_iLen;
	}
	
	void SetLength()
	{
		int iTmpLen =htonl(m_iLen);
		memcpy(m_pHead,&iTmpLen,sizeof(iTmpLen));
	}
private:
	char* m_pHead;
	char* m_pPos;
	int   m_iBufSize;
	int   m_iLen;
};




#endif 
