
#include "BinaryWriter.h"
#include <cstring>
#include "global.h"

BinaryWriter::BinaryWriter()
{
	m_pHead = new char[1024];
	m_pPos = m_pHead+4;
	m_iBufSize = 1024;
	m_iLen = 4;
}


BinaryWriter::~BinaryWriter()
{
	SafeDeleteArray(m_pHead);
}

void BinaryWriter::Realloc(const char* pBuf,int iSize)
{
	while ( m_iLen + iSize > m_iBufSize )
	{
		m_iBufSize+=1024;
		m_pHead = (char*)::realloc(m_pHead,m_iBufSize);
	}
	m_pPos = m_pHead + m_iLen;
	memcpy(m_pPos,pBuf,iSize);
	m_pPos+=iSize;
	m_iLen+=iSize;
}

BinaryWriter& BinaryWriter::operator << (bool value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (char value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (unsigned char value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (signed char value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (short value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (unsigned short value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (int value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (unsigned int value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (long value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (unsigned long value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (float value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}


BinaryWriter& BinaryWriter::operator << (double value)
{
	Realloc((const char*)&value,sizeof(value));
	return *this;
}

BinaryWriter& BinaryWriter::operator << (long long value)
{
	Realloc((const char *)&value,sizeof(value));
	return *this;
}

BinaryWriter& BinaryWriter::operator << (unsigned long long value)
{
	Realloc((const char *)&value,sizeof(value));
	return *this;
}

BinaryWriter& BinaryWriter::operator << (const std::string& value)
{
	int length = (int) value.size();
	Realloc(value.data(),length+1);
	return *this;
}

BinaryWriter& BinaryWriter::operator << (const std::wstring& value)
{
	int length = (int) value.size();
	Realloc((const char *)value.c_str(),(length+1)<<1);
	return *this;
}

BinaryWriter& BinaryWriter::operator << (const char* value)
{
	int length = (int) strlen(value);
	Realloc(value,length+1);
	return *this;
}

BinaryWriter& BinaryWriter::operator << (const wchar_t* value)
{
	int length = (int) wcslen(value);
	Realloc((const char *)value,length<<1);
	return *this;
}

void BinaryWriter::writeRaw(const char* value,int length)
{
	Realloc(value,length);
}

void BinaryWriter::writeRaw(const wchar_t* value,int length)
{
	Realloc((const char *)value,(length)<<1);
}