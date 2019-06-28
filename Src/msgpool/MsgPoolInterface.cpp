// MsgPoolInterface.cpp: implementation of the CMsgPoolInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "MsgPoolInterface.h"
#include "MsgPool.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMsgPoolInterface* CMsgPoolInterface::m_instance=NULL;
CMsgPoolInterface::CMsgPoolInterface()
{

}

CMsgPoolInterface::~CMsgPoolInterface()
{

}

CMsgPoolInterface* CMsgPoolInterface::Instance()
{
	if(m_instance == NULL)
	{
		m_instance = new CMsgPool();
		//CMsgLimitLayer::Instance();
	}
	return m_instance;
}

void CMsgPoolInterface::FreeInstance()
{
	if(m_instance)
	{
		delete m_instance;
		m_instance = NULL;
		//CMsgLimitLayer::FreeInstance();
	}
}