#pragma once
#include <queue>
#include "qvod_lock.h"
#include "common-callback.h"
#include "common-commonstruct.h"

using namespace std;

typedef void (*CallBackFun)(SPEEDATA& sdata);

class CSpeedCounter
{
public:
	CSpeedCounter(void);
	virtual ~CSpeedCounter(void);
	static CSpeedCounter* Instance();
	static void FreeInstance();
	void PushSpeedData(const HASH* hash,const KEY* key,unsigned int size,CallBackFun callBack);
	void DelSpeedData(HASH& hash, CallBackFun callBack);
	void DelSpeedData(HASH& hash,KEY &key,CallBackFun callBack);
	void PopSpeedData();
	static CSpeedCounter* m_instance;
private:
	CLock m_lock;
	vector<SPEEDATA> m_vector;
};


