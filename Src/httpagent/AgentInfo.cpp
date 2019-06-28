#include "AgentInfo.h"
#include "global.h"

CAgentInfo::CAgentInfo()
{
	pos=0;
	fp=NULL;
	filelen=0;
	contentlen = 0;
	wholeFilelen=0;
	downloadlen=0;
	type=_NONE;
	status=_INIT;
	ip=0;
	port=0;
	//	time=0;
	sock=0;
	bshare=false;
	bGzip = false;
	bThunked = false;
	connectimes=0;
	pdatabuf = NULL;
	piecelen = 0;
	metabuf = NULL;
	metalen = 0;
	xsecdownlen = 0;
	metadownlen = 0;
	downloadtime = 0;
	connecttime = 0;
	sendlen = 0;
	senddatalen = 0;
	connected = false;
	errcode = 0;
	memset(acchash,0,20);
	memset(md5,0,20);
	memset(rawbuf,0,2*1024);
	endpos = 0;
	memset(referer, 0, sizeof(referer));
	reuse = false;
	last_recv_tick = 0;
	iMaxDownloadTime = 5;
	bRemoveInvalidHead = false;
}

CAgentInfo::~CAgentInfo()
{
	//¹Ø±ÕÎÄ¼þ¾ä±ú
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	SafeDeleteArray(pdatabuf);
	SafeDeleteArray(metabuf);

	strHost.clear();
	strErrBuf.clear();
	strOrgUrl.clear();
	strUrl.clear();
	strSavePath.clear();
	strXml.clear();
	strDataBuf.clear();
}

CAgentInfo& CAgentInfo::operator = (CAgentInfo const& _A) 
{
	fp=_A.fp;
	pos=_A.pos;
	wholeFilelen=_A.wholeFilelen;
	filelen=_A.filelen;
	contentlen = _A.contentlen;
	downloadlen=_A.downloadlen;
	type=_A.type;
	status=_A.status;
	ip=_A.ip;
	port=_A.port;
	//	time=_A.time;
	sock = _A.sock;
	bshare=_A.bshare;
	bGzip = _A.bGzip;
	bThunked = _A.bThunked;
	connectimes=_A.connectimes;
	downloadtime=_A.downloadtime;
	connecttime = _A.connecttime;
	connected = _A.connected;
	pdatabuf = _A.pdatabuf;
	piecelen = _A.piecelen;
	metabuf = _A.metabuf;
	metalen = _A.metalen;
	xsecdownlen = _A.xsecdownlen;
	metadownlen = _A.metadownlen;
	errcode = _A.errcode;
	cachekey = _A.cachekey;
	sendlen = _A.sendlen;
	senddatalen = _A.senddatalen;
	strcpy(rawbuf,_A.rawbuf);
	memcpy(acchash,_A.acchash,20);
	memcpy(md5,_A.md5,20);
	bHaveRange = _A.bHaveRange;
	endpos = _A.endpos;
	strcpy(referer, _A.referer);
	bRemoveInvalidHead = _A.bRemoveInvalidHead;

	strHost = _A.strHost;
	strErrBuf = _A.strErrBuf;
	strOrgUrl = _A.strOrgUrl;
	strUrl = _A.strUrl;
	strSavePath = _A.strSavePath;
	strXml = _A.strXml;
	strDataBuf = _A.strDataBuf;
	strSendDataBuf = _A.strSendDataBuf;
	return *this;
}
