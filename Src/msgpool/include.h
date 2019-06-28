#pragma once
#pragma warning(disable: 4786)

#include "common-utility.h"
#include "common-commonfun.h"
#include "MsgHandle.h"

class CSock;

enum StcPeerType{STC_CACHE,STC_CLOUD,STC_SERVER,STC_5SEC,STC_P2P,STC_UP_SERVER,STC_UP_P2P};
enum RedundancyType{DOWN_SERVER=1,DOWN_CACHE,DOWN_CLIENT,UP_SERVER,UP_CLIENT};