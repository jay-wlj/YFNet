#include "common-callback.h"
#include "../taskmgr/TaskMgrInterface.h"
#include "../msgpool/MsgPoolInterface.h"
#include "../httpagent/HttpAgentInterface.h"
#include "../httpserver/httpserver.h"

extern CHttpServerMgr	g_httpServer;

//CallBack Function
void GlobalDownSpeedCallBack(SPEEDATA& sdata)
{
	sdata.size = -sdata.size;
	CMsgPoolInterface::Instance()->InterDownSize(sdata.size);
	CTaskMgrInterFace::Instance()->InterDownSize(sdata);
	//CChannelMgrInterface::Instance()->InterDownSize(sdata.hash, sdata.key, -sdata.size);
}

#ifndef FOREIGNER
void VipAccServerGlobalDwonSpeedCallBack(SPEEDATA &sdata)
{

}
#endif

void GlobalUpSpeedCallBack(SPEEDATA& sdata)
{
	sdata.size = -sdata.size;
	CMsgPoolInterface::Instance()->InterUpSize(sdata.size);
	CTaskMgrInterFace::Instance()->InterUpSize(sdata);
	//CChannelMgrInterface::Instance()->InterUpSize(sdata.hash, sdata.key, -sdata.size);
}

void HttpAgentDownCallBack(SPEEDATA& sdata)
{

}

void HttpAgentGlobalDownCallBack(SPEEDATA &sdata)
{
	CHttpAgentInterface::Instance()->InterDownSize(sdata.hash,-sdata.size);	
}

void HttpServerPushCallBack(SPEEDATA& sdata)
{
	g_httpServer.InterPushSize(-sdata.size);
}