#include "cs.h"

CCriticalSection::CCriticalSection(LPCRITICAL_SECTION  pCriticalSection)
{
	m_pCriticalSection = pCriticalSection;
}
CCriticalSection::~CCriticalSection()
{
	if(m_pCriticalSection)
		LeaveCriticalSection(m_pCriticalSection);
}
void CCriticalSection::Lock()
{
	EnterCriticalSection(m_pCriticalSection);
}
void CCriticalSection::UnLock()
{
		if(m_pCriticalSection)
		{
			LeaveCriticalSection(m_pCriticalSection);
			m_pCriticalSection = NULL;
		}
}
BOOL CCriticalSection::TryLock()
{
	if(!TryEnterCriticalSection(m_pCriticalSection))
	{
		m_pCriticalSection = NULL;
		return FALSE;
	}
	return TRUE;
}