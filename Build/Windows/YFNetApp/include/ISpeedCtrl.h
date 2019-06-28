#pragma once
#include "IQvodNet.h"

class  ISpeedCtrl
{
public:
	static ISpeedCtrl& GetInstance();
	
	virtual BOOL Init(HWND hWnd, IQvodNet* pQvodNet) = 0;
	virtual void UnInit() = 0;

	virtual BOOL IsGlobalSpeedOpen() = 0;		//ȫ�ֿ����Ƿ��
	//���ü��ٿ���
	virtual BOOL SetServerAccelerate(const bool bOpenAcc, const bool bOpenCache, const bool bOpenMiner) = 0;
	
	//��ȡ���ٴ�С
	virtual BOOL GetServerAccSpeed(const char *pstrHash, unsigned int &nAccSpeed, unsigned int &nCacheSpeed, unsigned int &nMinerSpeed, unsigned int &nCloudSpeed) = 0;

	virtual BOOL SetServerAccSpeed(BOOL bOpenAcc) = 0;
	virtual BOOL SetServerCacheSpeed(BOOL  bOpenCache) = 0;
	virtual BOOL SetServerMinerSpeed(BOOL bOpenMiner) = 0;
};
