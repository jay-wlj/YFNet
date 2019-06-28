#pragma once
#define UPNP_MULTICAST "239.255.255.250" 
#define UPNP_MULTICAST_PORT 1900
#include "qvod_sock.h"
#include "qvod_thread.h"
#include <string>
#include <set>
using namespace std;
class CUpnp
{
public:
	CUpnp(void);
	virtual ~CUpnp(void);
	static CUpnp* Instance();
	static void FreeInstance();
	bool Init(unsigned int localip,char* description,bool strict = false);
	bool Close();
	int  Search(unsigned short port,unsigned short extport);
	int  Reslove();
	bool GetGateway();
	bool AddPortMapping(char* gateway,char* protocol,char* service,unsigned short serport,char* controlurl);
	bool DelPortMapping(char* gateway,char* protocol,char* service,unsigned short serport,char* controlurl);
	bool CheckPortMapping(char* gateway,char* protocol,char* service,unsigned short serport,char* controlurl);
	static QVOD_THREAD_RETURN Routine(void* param);
	void GetCtrlURLAndBaseURL(char* buf, char* controlURL, char* baseURL);
	bool GetExternalIpAddress(char* gateway,unsigned short serport,char* service,char* controlurl,unsigned int& ip);
private:
	int RecvHttpRsp(qvod_sock_t sock,char* pBuf,int iBufLen);
	static CUpnp* m_instance;
	enum Status{RUNNING,STOP};
	bool m_bTcpOk;
	bool m_bUdpOK;
	bool m_bStrict;
	qvod_sock_t m_sock;
	Status m_status;
	qvod_sockaddr_t m_sockaddr;
	set<string> m_gatewaySet;
	unsigned short m_port;//本地映射端口
	unsigned short m_extport;//外网映射端口
	char m_localip[16];
	char m_description[255];
	bool m_bRetry;
};
