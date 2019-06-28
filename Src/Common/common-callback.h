#pragma once
#include "common-commonstruct.h"

struct ISpeedCallBack
{
	virtual void InterDownSize(const SPEEDATA& sdata) = 0;
	virtual void InterUpSize(const SPEEDATA& sdata) = 0;
};
//CallBack Function
void GlobalDownSpeedCallBack(SPEEDATA& sdata);
void GlobalUpSpeedCallBack(SPEEDATA& sdata);
void HttpAgentDownCallBack(SPEEDATA& sdata);
void HttpAgentGlobalDownCallBack(SPEEDATA &data);
void HttpServerPushCallBack(SPEEDATA& sdata);

#ifndef FOREIGNER
//Hi-Speed Server Global DownSpeed
void VipAccServerGlobalDwonSpeedCallBack(SPEEDATA &sdata);
#endif