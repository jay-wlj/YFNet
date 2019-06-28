#include "StdAfx.h"
#include "StatMgr.h"
#include "StatUpload.h"

CStatMgr::CStatMgr(void)
{
}


CStatMgr::~CStatMgr(void)
{
}

CStatMgr& CStatMgr::Instance()
{
	static StatUpload s_Instance;
	return s_Instance;
}