// Hash2ID.h: interface for the CHash2ID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HASH2ID_H__AE8BB032_8BD1_4ED8_AE07_6B5157BBFFF8__INCLUDED_)
#define AFX_HASH2ID_H__AE8BB032_8BD1_4ED8_AE07_6B5157BBFFF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common-commonstruct.h"
#include "qvod_lock.h"
#include <vector>

class CHash2ID  
{
public:
	static CHash2ID* Instance();
	static void FreeInstance();

	void Push(HASH& hash)
	{
		CAutoLock lock(m_lock);
		for ( unsigned int i= 0 ; i < m_hashs.size() ;i++)
		{
			if ( memcmp(hash.data,m_hashs[i].data,sizeof(hash.data)) == 0)
			{
				return;
			}
		}
		m_hashs.push_back(hash);
	}

	bool  Get(int iTaskID,HASH& hash)
	{
		iTaskID = iTaskID -1000;
		if ( (iTaskID >=0)  && ((unsigned int)iTaskID < m_hashs.size()))
		{
			hash = m_hashs[iTaskID];
			return true;
		}			
		return false;
	}

	int Get(HASH& hash)
	{
		for ( unsigned int i= 0 ; i < m_hashs.size() ;i++)
		{
			if ( memcmp(hash.data,m_hashs[i].data,sizeof(hash.data)) == 0)
			{
				return i+1000;
			}
		}
		return -1;
	}
public:
	CHash2ID();
	virtual ~CHash2ID();
private:
	std::vector<HASH> m_hashs;
	CLock            m_lock;
};

#endif // !defined(AFX_HASH2ID_H__AE8BB032_8BD1_4ED8_AE07_6B5157BBFFF8__INCLUDED_)
