#ifndef _COPYTASKFILE_H_
#define _COPYTASKFILE_H_

#include "common-commonstruct.h"
#include "qvod_lock.h"
#include "qvod_thread.h"
#include "qvod_basetypes.h"
#include <map>

using namespace std;

struct SCopyPara
{
	bool* pCopy;
	string wstrFilepath;
};
class CCopyTaskFile
{
public:
	static CCopyTaskFile* Instance();
	static void FreeInstance();
	bool Run();
	bool Stop();
	bool AddOneTask(HASH& hash,string strFilepath);
	void DeleteTask(HASH& hash);

private:
	CCopyTaskFile();
	~CCopyTaskFile();
	static void QVOD_WINAPI Routine(void* pVoid);
	void DoRoutine();
	void DeleteOneElement(HASH& hash);

	static CCopyTaskFile* m_instance;
	bool		m_bRunning;
	qvod_thread_t		m_hThread;
	map<HASH,SCopyPara> m_taskMap;
	CLock	  m_lock;
};
#endif