// Hash2ID.cpp: implementation of the CHash2ID class.
//
//////////////////////////////////////////////////////////////////////

#include "Hash2ID.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHash2ID* g_hash2ID;
CHash2ID::CHash2ID()
{

}

CHash2ID::~CHash2ID()
{

}

CHash2ID* CHash2ID::Instance()
{
	if ( g_hash2ID == NULL )
	{
		g_hash2ID = new CHash2ID;
	}
	return g_hash2ID;
}

void CHash2ID::FreeInstance()
{
	if ( g_hash2ID != NULL )
	{
		delete g_hash2ID;
		g_hash2ID = NULL;
	}
}
