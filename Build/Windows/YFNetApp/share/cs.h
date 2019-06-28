#ifndef CS_H
#define CS_H
#ifndef _WIN32_WINNT 
#define _WIN32_WINNT 0x0500
#endif
#include <windows.h>
class CCriticalSection
{
private:
	LPCRITICAL_SECTION m_pCriticalSection;
public:
	CCriticalSection(LPCRITICAL_SECTION );
	~CCriticalSection();
	void Lock();
	void UnLock();
	BOOL TryLock();
};


#endif