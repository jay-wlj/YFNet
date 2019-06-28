#include "upnp.h"
#include <stdio.h>
#include "common-commonfun.h"
#include "tinyxml.h"
#include "common-utility.h"
#include "global.h"
#include "qvod_time.h"

#ifdef WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>

#else

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#endif

#define UPNP_SERVICE_WANIP "urn:schemas-upnp-org:service:WANIPConnection:1"
#define UPNP_SERVICE_WANPPP "urn:schemas-upnp-org:service:WANPPPConnection:1"

//#define UPNP_SERVICE_WANPPP "upnp:rootdevice"

const std::string IntToString(int i)
{
	char buf[16]={0};
	itoa(i,buf,10);
	return std::string(buf);
}

CUpnp* CUpnp::m_instance = NULL;
CUpnp::CUpnp(void)
{
	m_status = STOP;
	m_sock = QVOD_INVALID_SOCKET;
}

CUpnp::~CUpnp(void)
{
	if(m_sock != QVOD_INVALID_SOCKET)
	{
		QvodCloseSocket(m_sock);
		m_sock = QVOD_INVALID_SOCKET;
	}
}

CUpnp* CUpnp::Instance()
{
	if(m_instance == NULL)
	{
		m_instance = new CUpnp();
	}
	return m_instance;
}

void CUpnp::FreeInstance()
{
	if(m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

bool CUpnp::Init(unsigned int localip,char* description,bool strict)
{
	if(m_status == RUNNING)
	{
		return false;
	}
	m_bStrict = strict;
	strcpy(m_description,description);
	//localip
	IpInt2Str(localip,m_localip);

	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(m_sock == QVOD_INVALID_SOCKET)
	{
		return false;
	}

	//TTL	每经过路由器后递减1，直到0才被Discard了
	int nRouteTTL = 6;
	if(QVOD_SOCKET_ERROR == setsockopt(m_sock, IPPROTO_IP, IP_MULTICAST_TTL, \
		(char *)&nRouteTTL, sizeof(nRouteTTL)))
	{
		QvodCloseSocket(m_sock);
		return false;
	}

	int loopback = 0;	// 当接收者加入到一个多播组以后，再向这个多播组发送数据，这个字段的设置是否允许再返回到本身
	if(QVOD_SOCKET_ERROR == setsockopt(m_sock, IPPROTO_IP, IP_MULTICAST_LOOP, \
		(char*)&loopback, sizeof(loopback)))
	{
		QvodCloseSocket(m_sock);
		return false;
	}

	//set addr
	int socklen = sizeof(sockaddr);
	memset(&m_sockaddr, 0, socklen);
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_addr.s_addr = inet_addr(UPNP_MULTICAST);
	m_sockaddr.sin_port = htons(UPNP_MULTICAST_PORT);

	QVOD_DWORD dwMs = 100;
	QvodSetSockTimeout(m_sock,SOL_SOCKET,SO_RCVTIMEO, dwMs);
	QvodSetSockTimeout(m_sock,SOL_SOCKET,SO_SNDTIMEO, dwMs);
	return true;
}

bool CUpnp::Close()
{
	m_status = STOP;
	m_gatewaySet.clear();
	if(m_sock != QVOD_INVALID_SOCKET)
	{
		QvodCloseSocket(m_sock);
		m_sock = QVOD_INVALID_SOCKET;
		return true;
	}
	return false;
}

int hexchar2int(char cHex)
{
	if (cHex >= '0' && cHex <= '9')
		return (cHex - '0');
	else if (cHex >= 'a' && cHex <= 'f')
		return (cHex - 'a' + 10);
	else if (cHex >= 'A' && cHex <= 'F')
		return (cHex - 'A' + 10);
	else
		return -1;
}

int hex_to_ipaddr(const char *hexString, char *destIPAddress)
{
	if (NULL == hexString || NULL == destIPAddress) {
		return -1;
	}

	//first verify hexString
	if (8 != strlen(hexString)) {
		return -1;
	}

	destIPAddress[0] = '\0';

	int ipSeg;
	//convert Hex string to IPAddress
	int i;
	for (i = 3; i > -1; i--) {
		ipSeg = 0;
		if (-1 != hexchar2int(hexString[i *2 +1] ) )
			ipSeg += hexchar2int(hexString[i *2 + 1]);
		if (-1 != hexchar2int(hexString[i * 2]))
			ipSeg += 16 * hexchar2int(hexString[i * 2]);

		char ipSegChar[5];
		sprintf(ipSegChar, "%d.", ipSeg);
		strcat(destIPAddress, ipSegChar);
	}

	//remove last dot
	destIPAddress[strlen(destIPAddress) -1] = '\0';
	return 0;
}

bool CUpnp::GetGateway()
{
	bool bRes = false;
#ifdef WIN32
	DWORD dwOutBufLen = 0; 
	PIP_ADAPTER_INFO pAdapterInfo = NULL,pAdapter = NULL; 
	int iReturn = GetAdaptersInfo(pAdapterInfo,&dwOutBufLen); 
	if(iReturn != ERROR_BUFFER_OVERFLOW) 
	{ 
		return false; 
	} 

	pAdapterInfo = (PIP_ADAPTER_INFO) HeapAlloc(GetProcessHeap(), 0, dwOutBufLen); 
	iReturn = GetAdaptersInfo(pAdapterInfo,&dwOutBufLen); 
	if(iReturn != ERROR_SUCCESS) 
	{ 
		HeapFree(GetProcessHeap(), 0, pAdapterInfo); 
		return false; 
	} 

	pAdapter = pAdapterInfo; 
	while(pAdapter != NULL ) //取所有网卡用while
	{
		_IP_ADDR_STRING* pipaddress = &pAdapter->GatewayList;
		while(pipaddress)
		{
			if(strlen(pipaddress->IpAddress.String))
			{
				if(strcmp(pipaddress->IpAddress.String,"0.0.0.0") != 0)
				{
					m_gatewaySet.insert(pipaddress->IpAddress.String);
					bRes = true;
				}
			}
			pipaddress = pipaddress->Next;
		}
		pAdapter = pAdapter->Next; 
	}
	HeapFree(GetProcessHeap(), 0, pAdapterInfo); 
#else 

#ifdef QVODDEBUG
	_OUTPUT(INEED,"start get gateway!\n");
#endif // QVODDEBUG

	FILE *pfile;
	pfile = fopen("/proc/net/route", "r");
	if (NULL == pfile)
	{
		return false;
	}

	//this length is enough for each line
	char buffer[16 * 12];
	memset(buffer,0,16 *12);
	char interfaceNameInFile[260];
	char destIPHexString[16];
	char gatewayHexString[16];
	char gatewayIPAddress[16];
	while (fgets(buffer,16 * 12,pfile))
	{
		sscanf(buffer, "%s\t%s\t%s\t", interfaceNameInFile, destIPHexString, gatewayHexString);
		if (0 == strcmp(destIPHexString, "00000000"))
		{
			if (0 == hex_to_ipaddr(gatewayHexString, gatewayIPAddress))
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "insert gateway: %s\n", gatewayIPAddress);
#endif // QVODDEBUG
				m_gatewaySet.insert(gatewayIPAddress);
				bRes = true;
			}
			else
			{
#ifdef QVODDEBUG
				_OUTPUT(INEED,"get gateway faild!\n");
#endif // QVODDEBUG
			}
			break;
		}
		memset(buffer,0,16 *12);
	}
	fclose(pfile);
#endif
	return bRes;
}

int CUpnp::Search(unsigned short port,unsigned short extport)
{
	if(m_status == RUNNING)
	{
		return -1;
	}
	m_status = RUNNING;
	m_bTcpOk = false;
	m_bUdpOK = false;
	m_port = port;
	m_extport = extport;
	m_bRetry = true;
	
	//重新获取网关
	if(!GetGateway())
	{
		m_status = STOP;
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "upnp get gateway fail\n");
#endif // QVODDEBUG
		return -1;
	}

	//创建处理线程
	QvodCreateThread(NULL, (qvod_routine_pt)CUpnp::Routine,this);

	//发送组播搜索路由器
	int runTimes = 20;
	int i = 3;
	while(i > 0 && m_bRetry)
	{
		//完成映射
		if(m_bTcpOk && m_bUdpOK)
		{
			break;
		}
		if(runTimes >= 20)
		{
			//未完成继续查找
			char sendBuf[1024] ={0};
			sprintf(sendBuf,"M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 6\r\nST: %s\r\n\r\n",UPNP_SERVICE_WANPPP);
			int iLen = sendto(m_sock,sendBuf,strlen(sendBuf),0,(sockaddr*)&m_sockaddr,sizeof(sockaddr));
			if(iLen < 0 )
			{
				m_status = STOP;
#ifdef QVODDEBUG
				_OUTPUT(INFO,"sendto failed,%d\n",errno);
#endif // QVODDEBUG
				return -1;
			}
			set<string>::iterator it = m_gatewaySet.begin();
			for(;it != m_gatewaySet.end(); it++)
			{
				qvod_sockaddr_t addr;
				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = inet_addr(it->c_str());
				addr.sin_port = htons(UPNP_MULTICAST_PORT);
				int iLen = sendto(m_sock,sendBuf,strlen(sendBuf),0,(sockaddr*)&addr,sizeof(sockaddr));
				if(iLen < 0 )
				{
					m_status = STOP;
#ifdef QVODDEBUG
					_OUTPUT(INFO,"sendto failed,%d\n",errno);
#endif // QVODDEBUG
					return -1;
				}
			}

			memset(sendBuf,0,1024);
			sprintf(sendBuf,"M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 6\r\nST: %s\r\n\r\n",UPNP_SERVICE_WANIP);
			iLen = sendto(m_sock,sendBuf,strlen(sendBuf),0,(sockaddr*)&m_sockaddr,sizeof(sockaddr));
			if(iLen < 0 )
			{
				m_status = STOP;
#ifdef QVODDEBUG
				_OUTPUT(INFO,"sendto failed,%d\n",errno);
#endif // QVODDEBUG
				return -1;
			}
			for(it = m_gatewaySet.begin();it != m_gatewaySet.end(); it++)
			{
				qvod_sockaddr_t addr;
				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = inet_addr(it->c_str());
				addr.sin_port = htons(UPNP_MULTICAST_PORT);
				int iLen = sendto(m_sock,sendBuf,strlen(sendBuf),0,(sockaddr*)&addr,sizeof(sockaddr));
				if(iLen < 0 )
				{
					m_status = STOP;
#ifdef QVODDEBUG
					_OUTPUT(INFO,"sendto failed,%d\n",errno);
#endif // QVODDEBUG
					return -1;
				}
			}
			i--;
			runTimes = 0;
		}
		runTimes++;
		QVOD_SLEEP(100);
	}

	//超时,强制停止映射
	m_status = STOP;

	//若开启严格模式,必须tcp和udp同时映射成功
	if(g_isForceUdp)
	{
		if(!m_bTcpOk && m_bStrict || !m_bUdpOK)
		{
			return -1;
		}
	}
	else
	{
		if(!m_bUdpOK && m_bStrict || !m_bTcpOk)
		{
			return -1;
		}
	}
#ifdef QVODDEBUG
	_OUTPUT(SERIOUS,"*****port mapping ok!!\n");
#endif // QVODDEBUG
	return 0;
}

bool CUpnp::AddPortMapping(char* gateway,char* protocol,char* service,unsigned short serport,char* controlurl)
{
	if(m_status != RUNNING)
	{
		return false;
	}
	char gatewayhost[255] = {0};
	sprintf(gatewayhost,"%s:%u",gateway,serport);

	std::string args;
	args.append("<NewRemoteHost></NewRemoteHost>\r\n");
	args.append("<NewExternalPort>");
	args.append(IntToString(m_extport));
	args.append("</NewExternalPort>\r\n");
	args.append("<NewProtocol>");
	args.append(protocol);
	args.append("</NewProtocol>\r\n");
	args.append("<NewInternalPort>");
	args.append(IntToString(m_port));
	args.append("</NewInternalPort>\r\n");
	args.append("<NewInternalClient>");
	args.append(m_localip);
	args.append("</NewInternalClient>\r\n");
	args.append("<NewEnabled>1</NewEnabled>\r\n");
	args.append("<NewPortMappingDescription>");
	args.append(m_description);
	args.append("</NewPortMappingDescription>\r\n");
	args.append("<NewLeaseDuration>0</NewLeaseDuration>\r\n");

	std::string xmldata;
	xmldata.append("<?xml version=\"1.0\"?>\r\n");
	xmldata.append("<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "\
		"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n");
	xmldata.append("<s:Body>\r\n");
	xmldata.append("<u:AddPortMapping xmlns:u=\"");
	xmldata.append(service);
	xmldata.append("\">\r\n");
	xmldata.append(args);
	xmldata.append("</u:AddPortMapping>\r\n");
	xmldata.append("</s:Body>\r\n");
	xmldata.append("</s:Envelope>\r\n\r\n");

	std::string httpdata;
	httpdata.append("POST ");
	httpdata.append(controlurl);
	httpdata.append(" HTTP/1.1\r\n");
	httpdata.append("HOST: ");
	httpdata.append(gatewayhost);
	httpdata.append("\r\n");
	httpdata.append("Content-Length: ");
	httpdata.append(IntToString(xmldata.length()));
	httpdata.append("\r\n");
	httpdata.append("CONTENT-TYPE: text/xml;charset=\"utf-8\"\r\n");
	httpdata.append("SOAPACTION: \"");
	httpdata.append(service);
	httpdata.append("#AddPortMapping\"\r\n\r\n");
	httpdata.append(xmldata);

	qvod_sock_t sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock == QVOD_INVALID_SOCKET)
	{
		return false;
	}
	qvod_sockaddr_t SvrAddr;
	SvrAddr.sin_addr.s_addr = inet_addr(gateway);
	SvrAddr.sin_port = htons(serport);
	SvrAddr.sin_family = AF_INET;
	int result = connect(sock,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
	if(result == QVOD_SOCKET_ERROR)
	{
		QvodCloseSocket(sock);
		return false;
	}
	QvodSetSockTimeout(sock, SOL_SOCKET, SO_RCVTIMEO, 5000);

	int iLen = send(sock,httpdata.c_str(),httpdata.size(),0);
	if(iLen < 0)
	{
		QvodCloseSocket(sock);
		return false;
	}

	char recvbuf[2048] = {0};
	int recvlen = RecvHttpRsp(sock,recvbuf,sizeof(recvbuf));	
	QvodCloseSocket(sock);

	if(strstr(recvbuf,"200 OK") == NULL)
	{
		return false;
	}
	return true;
}

bool CUpnp::DelPortMapping(char* gateway,char* protocol,char* service,unsigned short serport,char* controlurl)
{
	if(m_status != RUNNING)
	{
		return false;
	}
	char gatewayhost[255] = {0};
	sprintf(gatewayhost,"%s:%u",gateway,serport);

	std::string args;
	args.append("<NewRemoteHost></NewRemoteHost>\r\n");
	args.append("<NewExternalPort>");
	args.append(IntToString(m_extport));
	args.append("</NewExternalPort>\r\n");
	args.append("<NewProtocol>");
	args.append(protocol);
	args.append("</NewProtocol>\r\n");

	std::string xmldata;
	xmldata.append("<?xml version=\"1.0\"?>\r\n");
	xmldata.append("<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "\
		"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n");
	xmldata.append("<s:Body>\r\n");
	xmldata.append("<u:DeletePortMapping xmlns:u=\"");
	xmldata.append(service);
	xmldata.append("\">\r\n");
	xmldata.append(args);
	xmldata.append("</u:DeletePortMapping>\r\n");
	xmldata.append("</s:Body>\r\n");
	xmldata.append("</s:Envelope>\r\n\r\n");

	std::string httpdata;
	httpdata.append("POST ");
	httpdata.append(controlurl);
	httpdata.append(" HTTP/1.1\r\n");
	httpdata.append("HOST: ");
	httpdata.append(gatewayhost);
	httpdata.append("\r\n");
	httpdata.append("Content-Length: ");
	httpdata.append(IntToString(xmldata.length()));
	httpdata.append("\r\n");
	httpdata.append("CONTENT-TYPE: text/xml;charset=\"utf-8\"\r\n");
	httpdata.append("SOAPACTION: \"");
	httpdata.append(service);
	httpdata.append("#DeletePortMapping\"\r\n\r\n");
	httpdata.append(xmldata);

	qvod_sock_t sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock == QVOD_INVALID_SOCKET)
	{
		return false;
	}
	qvod_sockaddr_t SvrAddr;
	SvrAddr.sin_addr.s_addr = inet_addr(gateway);
	SvrAddr.sin_port = htons(serport);
	SvrAddr.sin_family = AF_INET;
	int result = connect(sock,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
	if(result == QVOD_SOCKET_ERROR)
	{
		QvodCloseSocket(sock);
		return false;
	}
	
	QvodSetSockTimeout(sock, SOL_SOCKET, SO_RCVTIMEO, 5000);

	int iLen = send(sock,httpdata.c_str(),httpdata.size(),0);
	if(iLen < 0)
	{
		QvodCloseSocket(sock);
		return false;
	}

	char recvbuf[2048] = {0};
	int recvlen = RecvHttpRsp(sock,recvbuf,sizeof(recvbuf));	
	QvodCloseSocket(sock);
	//部分路由器在删除的时候返回错误500,当不开启严格模式下忽略此错误
	if(m_bStrict && strstr(recvbuf,"200 OK") == NULL)
	{
		return false;
	}
	return true;
}

bool CUpnp::CheckPortMapping(char* gateway,char* protocol,char* service,unsigned short serport,char* controlurl)
{
	if(m_status != RUNNING)
	{
		return false;
	}
	char gatewayhost[255] = {0};
	sprintf(gatewayhost,"%s:%u",gateway,serport);

	std::string args;
	args.append("<NewRemoteHost></NewRemoteHost>\r\n");
	args.append("<NewExternalPort>");
	args.append(IntToString(m_extport));
	args.append("</NewExternalPort>\r\n");
	args.append("<NewProtocol>");
	args.append(protocol);
	args.append("</NewProtocol>\r\n");

	std::string xmldata;
	xmldata.append("<?xml version=\"1.0\"?>\r\n");
	xmldata.append("<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "\
		"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n");
	xmldata.append("<s:Body>\r\n");
	xmldata.append("<u:GetSpecificPortMappingEntry xmlns:u=\"");
	xmldata.append(service);
	xmldata.append("\">\r\n");
	xmldata.append(args);
	xmldata.append("</u:GetSpecificPortMappingEntry>\r\n");
	xmldata.append("</s:Body>\r\n");
	xmldata.append("</s:Envelope>\r\n\r\n");

	std::string httpdata;
	httpdata.append("POST ");
	httpdata.append(controlurl);
	httpdata.append(" HTTP/1.1\r\n");
	httpdata.append("HOST: ");
	httpdata.append(gatewayhost);
	httpdata.append("\r\n");
	httpdata.append("Content-Length: ");
	httpdata.append(IntToString(xmldata.length()));
	httpdata.append("\r\n");
	httpdata.append("CONTENT-TYPE: text/xml;charset=\"utf-8\"\r\n");
	httpdata.append("SOAPACTION: \"");
	httpdata.append(service);
	httpdata.append("#GetSpecificPortMappingEntry\"\r\n\r\n");
	httpdata.append(xmldata);

	qvod_sock_t sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock == QVOD_INVALID_SOCKET)
	{
		return false;
	}
	qvod_sockaddr_t SvrAddr;
	SvrAddr.sin_addr.s_addr = inet_addr(gateway);
	SvrAddr.sin_port = htons(serport);
	SvrAddr.sin_family = AF_INET;
	int result = connect(sock,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
	if(result == QVOD_SOCKET_ERROR)
	{
		QvodCloseSocket(sock);
		return false;
	}

	QvodSetSockTimeout(sock, SOL_SOCKET, SO_RCVTIMEO, 5000);

	int iLen = send(sock,httpdata.c_str(),httpdata.size(),0);
	if(iLen < 0)
	{
		QvodCloseSocket(sock);
		return false;
	}

	char recvbuf[2048] = {0};
	int recvlen = RecvHttpRsp(sock,recvbuf,sizeof(recvbuf));	
	QvodCloseSocket(sock);
	if(strstr(recvbuf,"200 OK") == NULL)
	{
		return false;
	}
	char* pos = NULL;
	char client[255] = {0};
	if((pos = strstr(recvbuf,"<NewInternalClient>")) != NULL)
	{
		pos += strlen("<NewInternalClient>");
		char* pos1 = strstr(pos,"</NewInternalClient>");
		if (!pos1)
		{
			return false;
		}
		memcpy(client,pos,pos1 - pos);
	}

	char description[255] = {0};
	if((pos = strstr(recvbuf,"<NewPortMappingDescription>")) != NULL)
	{
		pos += strlen("<NewPortMappingDescription>");
		char* pos2 = strstr(pos,"</NewPortMappingDescription>");
		if (!pos2)
		{
			return false;
		}
		memcpy(description,pos, pos2 - pos);
	}
	
	if(strcmp(m_localip,client) != 0 || strcmp(m_description,description) != 0)
	{
		return false;
	}

	return true;
}

bool CUpnp::GetExternalIpAddress( char* gateway,unsigned short serport,char* service,char* controlurl,unsigned int& ip )
{
	if(m_status != RUNNING)
	{
		return false;
	}
	char gatewayhost[255] = {0};
	sprintf(gatewayhost,"%s:%u",gateway,serport);

	std::string args;

	std::string xmldata;
	xmldata.append("<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "\
		"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n");
	xmldata.append("<s:Body>\r\n");
	xmldata.append("<u:GetExternalIPAddress xmlns:u=\"");
	xmldata.append(service);
	xmldata.append("\">\r\n");
	xmldata.append("</u:GetExternalIPAddress>\r\n");
	xmldata.append("</s:Body>\r\n");
	xmldata.append("</s:Envelope>\r\n\r\n");

	std::string httpdata;
	httpdata.append("POST ");
	httpdata.append(controlurl);
	httpdata.append(" HTTP/1.1\r\n");
	httpdata.append("HOST: ");
	httpdata.append(gatewayhost);
	httpdata.append("\r\n");
	httpdata.append("Content-Length: ");
	httpdata.append(IntToString(xmldata.length()));
	httpdata.append("\r\n");
	httpdata.append("CONTENT-TYPE: text/xml;charset=\"utf-8\"\r\n");
	httpdata.append("SOAPACTION: \"");
	httpdata.append(service);
	httpdata.append("#GetExternalIPAddress\"\r\n\r\n");
	httpdata.append(xmldata);

	qvod_sock_t sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock == QVOD_INVALID_SOCKET)
	{
		return false;
	}
	qvod_sockaddr_t SvrAddr;
	SvrAddr.sin_addr.s_addr = inet_addr(gateway);
	SvrAddr.sin_port = htons(serport);
	SvrAddr.sin_family = AF_INET;
	int result = connect(sock,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
	if(result == QVOD_SOCKET_ERROR)
	{
		QvodCloseSocket(sock);
		return false;
	}
	QvodSetSockTimeout(sock, SOL_SOCKET, SO_RCVTIMEO, 5000);

	int iLen = send(sock,httpdata.c_str(),httpdata.size(),0);
	if(iLen < 0)
	{
		QvodCloseSocket(sock);
		return false;
	}

	char recvbuf[2048] = {0};
	int recvlen = RecvHttpRsp(sock,recvbuf,sizeof(recvbuf));	
	QvodCloseSocket(sock);

	if(strstr(recvbuf,"200 OK") == NULL)
	{		
		return false;
	}
	char* pos = NULL;
	char szIp[255] = {0};
	if((pos = strstr(recvbuf,"<NewExternalIPAddress>")) != NULL)
	{
		pos += strlen("<NewExternalIPAddress>");
		char* pos1 = strstr(pos,"</NewExternalIPAddress>");
		if (!pos1)
		{
			return false;
		}
		memcpy(szIp,pos,pos1 - pos);
		ip = inet_addr(szIp);
	}

	return true;
}

int CUpnp::Reslove()
{
	if(m_status != RUNNING)
	{
		return -1;
	}
	sockaddr_in addr;
	memset(&addr,0,sizeof(sockaddr_in));
	qvod_socklen_t  addrlen = sizeof(sockaddr_in);
	char buf[1024] = {0};
	int rlen = recvfrom(m_sock,buf,1024,0,(sockaddr*)&addr,&addrlen);
	if(rlen > 0)
	{
#ifdef QVODDEBUG
		//信息打印
 		char recvip[16];
 		IpInt2Str(addr.sin_addr.s_addr,recvip);
 		_OUTPUT(SERIOUS,"router:%s!!\r\n",recvip);
#endif // QVODDEBUG
		
		//确定数据包
		if((strstr(buf,"200 OK")/* || strstr(recvbuf,"NOTIFY * HTTP/1.1")*/)
			&& (strstr(buf,UPNP_SERVICE_WANIP) || strstr(buf,UPNP_SERVICE_WANPPP)))
		{
			char* pos = NULL;
			char location[255] = {0};
#ifdef WIN32
			if((pos = strstr(buf,"Location:")) != NULL)		// 忽略大小写?
#else
			if((pos = strcasestr(buf,"LOCATION:")) != NULL)
#endif
			{
				pos += strlen("Location:");
				if(*pos == 0x20)
				{
					pos++;
				}
				memcpy(location,pos,strstr(pos,"\r\n") - pos);
			}
			else
			{
				return -2;
			}

			char strip[16] = {0};
			if((pos = strstr(location + 7,":")) != NULL)
			{
				memcpy(strip,location + 7,pos - location - 7);
			}
			else
			{
				return -2;
			}

			//网关不对
			if(m_gatewaySet.find(strip) == m_gatewaySet.end())
			{
				return -2;
			}

			unsigned short serport = 1900;
			if((pos = strstr(location + 7,":")) != NULL)
			{
				serport = atoi(pos + 1);
			}
			else
			{
				return -2;
			}

			char service[255] = {0};
#ifdef WIN32
			if((pos = strstr(buf,"ST:")) != NULL)
#else
			if((pos = strcasestr(buf,"ST:")) != NULL)
#endif
			{
				pos += strlen("ST:");
				if(*pos == 0x20)
				{
					pos++;
				}
				memcpy(service,pos,strstr(pos,"\r\n") - pos);
			}
			else
			{
				return -2;
			}

			char host[255] = {0};
			char path[255] = {0};
			char* pathpos = strstr(location + 7,"/");
			if(pathpos == NULL)
			{
				return -2;
			}
			memcpy(host,location+7,pathpos - location - 7);
			strcpy(path,pathpos);
#ifdef QVODDEBUG
			_OUTPUT(SERIOUS,"router:%s____ok__%.15s!!\r\n",recvip,buf);
#endif // QVODDEBUG

			//get xml
			qvod_sock_t sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
			if(sock == QVOD_INVALID_SOCKET)
			{
				return -1;
			}
			qvod_sockaddr_t SvrAddr;
			SvrAddr.sin_addr.s_addr = addr.sin_addr.s_addr;
			SvrAddr.sin_port = htons(serport);
			SvrAddr.sin_family = AF_INET;
			int result = connect(sock,(sockaddr*)&SvrAddr,sizeof(SvrAddr));
			if(result == QVOD_SOCKET_ERROR)
			{
				QvodCloseSocket(sock);
				return -1;
			}

			QvodSetSockTimeout(sock, SOL_SOCKET, SO_RCVTIMEO, 5000);

			char sendbuf[255] = {0};
			sprintf(sendbuf,"GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",path,host);
			rlen = send(sock,sendbuf,strlen(sendbuf),0);
			if(rlen < 0)
			{
				QvodCloseSocket(sock);
				return -2;
			}

			char recvbuf[8192] = {0};
			int recvlen = RecvHttpRsp(sock,recvbuf,sizeof(recvbuf));	
			QvodCloseSocket(sock);
			if(strstr(recvbuf,"200 OK") == NULL)
			{				
				return -2;
			}

			char urlbase[255] = {0};
			char ctrlurl[255] = {0};
			GetCtrlURLAndBaseURL(recvbuf, ctrlurl, urlbase);
			unsigned int iExternalIp = 0;
			if (!GetExternalIpAddress(strip,serport,service,ctrlurl,iExternalIp) || IsLANIP(iExternalIp))
			{
				//获取路由器wan口ip失败或者wan口ip为内网ip(有双层路由)，无需再upnp
#ifdef QVODDEBUG
				_OUTPUT(INFO,"router External Ip = %s\n",inet_ntoa(*(in_addr*)&iExternalIp));
#endif				
				return -3;
			}
#ifdef QVODDEBUG
			_OUTPUT(INFO,"router ip = %s\n",strip);
#endif			
			bool bRes = false;
			if(g_isForceUdp)
			{
				if(!m_bUdpOK)
				{
					//UDP
					bRes = CheckPortMapping(strip,"UDP",service,serport,ctrlurl);
					if(bRes)
					{
#ifdef QVODDEBUG
						_OUTPUT(SERIOUS,"###udp port mapping ok!!!\n");
#endif // QVODDEBUG
						m_bUdpOK = true;
					}
					else
					{
						bRes = DelPortMapping(strip,"UDP",service,serport,ctrlurl);
						if(!bRes)
						{
#ifdef QVODDEBUG
							_OUTPUT(INEED,"DelPortMapping Fail!!\n");
#endif // QVODDEBUG
							return -2;
						}

						bRes = AddPortMapping(strip,"UDP",service,serport,ctrlurl);
						if(!bRes)
						{
#ifdef QVODDEBUG
							_OUTPUT(INEED,"AddPortMapping Fail!!\n");
#endif // QVODDEBUG
							return -2;
						}
						else
						{
#ifdef QVODDEBUG
							_OUTPUT(SERIOUS,"###udp port mapping finish!!!\n");
#endif // QVODDEBUG
							m_bUdpOK = true;
						}
					}
				}

				if(!m_bTcpOk)
				{
					//TCP
					bRes = CheckPortMapping(strip,"TCP",service,serport,ctrlurl);
					if(bRes)
					{
#ifdef QVODDEBUG
						_OUTPUT(SERIOUS,"###tcp port mapping ok!!!\n");
#endif // QVODDEBUG
						m_bTcpOk = true;
					}
					else
					{
						bRes = DelPortMapping(strip,"TCP",service,serport,ctrlurl);
						if(!bRes)
						{
#ifdef QVODDEBUG
							_OUTPUT(INEED,"DelPortMapping Fail!!\n");
#endif // QVODDEBUG
							return -2;
						}

						bRes = AddPortMapping(strip,"TCP",service,serport,ctrlurl);
						if(!bRes)
						{
#ifdef QVODDEBUG
							_OUTPUT(INEED,"AddPortMapping Fail!!\n");
#endif // QVODDEBUG
							return -2;
						}
						else
						{
#ifdef QVODDEBUG
							_OUTPUT(SERIOUS,"###tcp port mapping finish!!!\n");
#endif // QVODDEBUG
							m_bTcpOk = true;
						}
					}
				}
			}
			else
			{
				if(!m_bTcpOk)
				{
					//TCP
					bRes = CheckPortMapping(strip,"TCP",service,serport,ctrlurl);
					if(bRes)
					{
#ifdef QVODDEBUG
						_OUTPUT(SERIOUS,"###tcp port mapping ok!!!\n");
#endif // QVODDEBUG
						m_bTcpOk = true;
					}
					else
					{
						bRes = DelPortMapping(strip,"TCP",service,serport,ctrlurl);
						if(!bRes)
						{
#ifdef QVODDEBUG
							_OUTPUT(INEED,"DelPortMapping Fail!!\n");
#endif // QVODDEBUG
							return -2;
						}

						bRes = AddPortMapping(strip,"TCP",service,serport,ctrlurl);
						if(!bRes)
						{
#ifdef QVODDEBUG
							_OUTPUT(INEED,"AddPortMapping Fail!!\n");
#endif // QVODDEBUG
							return -2;
						}
						else
						{
#ifdef QVODDEBUG
							_OUTPUT(SERIOUS,"###tcp port mapping finish!!!\n");
#endif // QVODDEBUG
							m_bTcpOk = true;
						}
					}
				}

				if(!m_bUdpOK)
				{
					//UDP
					bRes = CheckPortMapping(strip,"UDP",service,serport,ctrlurl);
					if(bRes)
					{
#ifdef QVODDEBUG
						_OUTPUT(SERIOUS,"###udp port mapping ok!!!\n");
#endif // QVODDEBUG
						m_bUdpOK = true;
					}
					else
					{
						bRes = DelPortMapping(strip,"UDP",service,serport,ctrlurl);
						if(!bRes)
						{
#ifdef QVODDEBUG
							_OUTPUT(INEED,"DelPortMapping Fail!!\n");
#endif // QVODDEBUG
							return -2;
						}

						bRes = AddPortMapping(strip,"UDP",service,serport,ctrlurl);
						if(!bRes)
						{
#ifdef QVODDEBUG
							_OUTPUT(INEED,"AddPortMapping Fail!!\n");
#endif // QVODDEBUG
							return -2;
						}
						else
						{
#ifdef QVODDEBUG
							_OUTPUT(SERIOUS,"###udp port mapping finish!!!\n");
#endif // QVODDEBUG
							m_bUdpOK = true;
						}
					}
				}
			}
			return 0;
		}
		return -2;
	}
	return -1;
}

QVOD_THREAD_RETURN CUpnp::Routine(void* param)
{
	CUpnp* pUpnp = (CUpnp*)param;
	while(pUpnp->m_status == RUNNING)
	{
		int result = pUpnp->Reslove();
		if( -1 == result)
		{
			QVOD_SLEEP(15);
		}
		else	if (-3 == result)
		{
			pUpnp->m_bRetry = false;
			break;
		}
		if(pUpnp->m_bTcpOk && pUpnp->m_bUdpOK)
		{
			break;
		}
	}
#ifndef WIN32
    return 0;
#endif
}

char* GetBaseURL(TiXmlDocument* xmlDocument, char* baseURL)
{
	if(NULL == baseURL)
	{
		return NULL;
	}
	TiXmlElement* rootElement = xmlDocument->RootElement();
	if (rootElement == NULL)
	{
		return NULL;
	}
	TiXmlNode* childNode = rootElement->FirstChild("URLBase");
	if (childNode)
	{
		if (childNode->FirstChild())
		{
			strcpy(baseURL, childNode->FirstChild()->Value());
		}
	}
	return baseURL;
}

//查找属性为itemName且其值为itemValue的节点，返回该节点下属性为findItem的子节点
TiXmlNode* FindNode(TiXmlNode* node, const char* itemName, const char* itemValue, const char* findItem)
{
	TiXmlNode* childNode = node->FirstChild(itemName);
	while (childNode)
	{
		if (strcmp(childNode->FirstChild()->Value(), itemValue) == 0)
		{
			return node->FirstChild(findItem);
		}
		else
		{
			childNode = childNode->NextSibling(itemName);
		}
	}
	return childNode;
}

char* GetControlURL(TiXmlDocument* xmlDocument, char* controlURL)
{
	if(NULL == controlURL)
	{
		return NULL;
	}
	TiXmlElement* rootElement = xmlDocument->RootElement();
	if (rootElement == NULL)
	{
		return NULL;
	}
	TiXmlNode* childNode = rootElement->FirstChild("device");
	const char* str1 = "urn:schemas-upnp-org:device:InternetGatewayDevice:1";
	const char* str2 = "urn:schemas-upnp-org:device:WANDevice:1";
	const char* str3 = "urn:schemas-upnp-org:device:WANConnectionDevice:1";
	const char* str4 = "urn:schemas-upnp-org:service:WANIPConnection:1";
	const char* str5 = "urn:schemas-upnp-org:service:WANPPPConnection:1";

	while(childNode)
	{
		TiXmlNode* node = FindNode(childNode, "deviceType", str1, "deviceList");
		if (node)
		{
			childNode = node;
			break;
		}
		else
		{
			childNode = childNode->NextSibling("device");
		}
	}
	if (NULL == childNode)
	{
		return NULL;
	}

	childNode = childNode->FirstChild("device");
	while(childNode)
	{
		TiXmlNode* node = FindNode(childNode, "deviceType", str2, "deviceList");
		if (node)
		{
			childNode = node;
			break;
		}
		else
		{
			childNode = childNode->NextSibling("device");
		}
	}
	if (NULL == childNode)
	{
		return NULL;
	}

	childNode = childNode->FirstChild("device");
	while(childNode)
	{
		TiXmlNode* node = FindNode(childNode, "deviceType", str3, "serviceList");
		if (node)
		{
			childNode = node;
			break;
		}
		else
		{
			childNode = childNode->NextSibling("device");
		}
	}
	if (NULL == childNode)
	{
		return NULL;
	}

	TiXmlNode* serviceList = childNode;
	childNode = serviceList->FirstChild("service");
	while(childNode)
	{
		TiXmlNode* node = FindNode(childNode, "serviceType", str4, "controlURL");
		if (node)
		{
			childNode = node;
			break;
		}
		else
		{
			childNode = childNode->NextSibling("service");
		}
	}
	if (NULL == childNode)
	{
		childNode = serviceList->FirstChild("service");
		while(childNode)
		{
			TiXmlNode* node = FindNode(childNode, "serviceType", str5, "controlURL");
			if (node)
			{
				childNode = node;
				break;
			}
			else
			{
				childNode = childNode->NextSibling("service");
			}
		}
		if (NULL == childNode)
		{
			return NULL;
		}
	}
	strcpy(controlURL, childNode->FirstChild()->Value());
	return controlURL;
}

void CUpnp::GetCtrlURLAndBaseURL(char* buf, char* controlURL, char* baseURL)
{
	char* pbuf = strstr(buf,"\r\n\r\n");
	if(pbuf != NULL)
	{
		pbuf += 4;
		TiXmlDocument* xmlDocument = new TiXmlDocument();
		xmlDocument->Parse(pbuf);
		GetControlURL(xmlDocument, controlURL);
		GetBaseURL(xmlDocument, baseURL);
		delete xmlDocument;
	}
}

int CUpnp::RecvHttpRsp( qvod_sock_t sock,char* pBuf,int iBufLen )
{
	if (QVOD_INVALID_SOCKET == sock || !pBuf || iBufLen <= 0)
	{
		return 0;
	}
	
	int iTotalRecv = 0;
	int iContentLen = 0;
	char* pHttpHead = NULL;
	while (!pHttpHead)
	{
		int iRecv = recv(sock,pBuf+iTotalRecv,iBufLen-iTotalRecv,0);
		if (iRecv <= 0)
		{
			return iTotalRecv;
		}
		iTotalRecv += iRecv;
		pHttpHead = strstr(pBuf,"\r\n\r\n");
	}
	pHttpHead += 4;
	int iRecvContentLen = iTotalRecv - (pHttpHead-pBuf);
	char* pos = strstr(pBuf,"Content-Length: ");
	if (pos)
	{
		pos += strlen("Content-Length: ");
		iContentLen = atoi(pos);
	}
	
	while (true)
	{
		if (iContentLen > 0 && iRecvContentLen == iContentLen)
		{
			break;
		}
		int iRecv = recv(sock,pBuf+iTotalRecv,iBufLen-iTotalRecv,0);
		if (iRecv <= 0)
		{
			return iTotalRecv;
		}
		iTotalRecv += iRecv;
		iRecvContentLen += iRecv;
	}
	return iTotalRecv;
}
