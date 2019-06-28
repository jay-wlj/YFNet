#include "TaskMgrInterface.h"
#include "TaskMgr.h"

CTaskMgrInterFace * CTaskMgrInterFace::_instance = NULL;

CTaskMgrInterFace *CTaskMgrInterFace::Instance()
{
	if(NULL == _instance)
	{
		_instance = new CTaskMgr();
	}
	return _instance;
}

void CTaskMgrInterFace::FreeInstance()
{
	if ( NULL != _instance )
	{
		delete _instance;
		_instance = NULL;
	}
}
