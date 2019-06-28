#include "BinaryReader.h"
#include <cstring>
#include "global.h"

BinaryReader::BinaryReader(char* pszIn, int length, qvod_file_t handle)
{	
	m_pHead = new char[length+1];
	memcpy(m_pHead, pszIn, length);
	m_pPos = m_pHead;
	m_iLen = length;
	m_bError = false;
	m_handle = handle;	
}

BinaryReader::~BinaryReader()
{
	SafeDeleteArray(m_pHead);
}

BinaryReader& BinaryReader::operator >> (bool& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (char& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (unsigned char& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (signed char& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (short& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (unsigned short& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (int& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (unsigned int& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (long& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (unsigned long& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (float& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}


BinaryReader& BinaryReader::operator >> (double& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}

BinaryReader& BinaryReader::operator >> (long long& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}

BinaryReader& BinaryReader::operator >> (unsigned long long& value)
{
	if ( !m_bError )
	{
		memcpy((char*) &value,m_pPos, sizeof(value));
		m_pPos += sizeof(value);
	}
	return *this;
}

BinaryReader& BinaryReader::operator >> (std::string& value)
{
	if ( !m_bError )
	{
		while(*m_pPos!='\0')
		{
			value += *m_pPos;
			m_pPos++;
		}
		m_pPos++;
	}
	return *this;
}



void BinaryReader::readRaw(char* value,int length)
{
	if ( !m_bError )
	{
		memcpy(value,m_pPos, length);
		m_pPos += length;
	}
}
