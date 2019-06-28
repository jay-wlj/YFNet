#include <WINSOCK2.H>
#include "socks5.h"

struct checkreq
{
	unsigned char ver;
	unsigned char cmd;
	unsigned char reserved;
	unsigned char addrtype;
	unsigned long localip;
	unsigned short localport;
	
};
struct checkrsp
{
	unsigned char ver;
	unsigned char result;
	unsigned char reserved;
	unsigned char addrtype;
	unsigned long agentip;
	unsigned short agentport;	
};
CSocks5::CSocks5(UINT agentip,USHORT agentport)
:m_agentip(agentip),
m_agentport(agentport),
m_udpagentip(0),
m_udpagentport(0)
{
	m_binited = false;
	m_s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
}
CSocks5::~CSocks5()
{
	closesocket(m_s);
}
int CSocks5::init(UINT localip,USHORT localport,char* name,char* pwd)
{
	if(m_binited)
		return 0;
	m_binited = true;
	char sendbuf[1024];
	char recvbuf[1024];
	SOCKADDR_IN agentaddr;
	agentaddr.sin_family = AF_INET;
	agentaddr.sin_addr.S_un.S_addr = m_agentip;
	agentaddr.sin_port = m_agentport;
	
	int r = connect(m_s,(sockaddr*)&agentaddr,sizeof(agentaddr));
	if(r !=0)
		return -1;
	char* pbuf = sendbuf;
	*pbuf++ = 0x05;
	*pbuf++ = 0x02;
	*pbuf++ = 0x00;
	*pbuf++ = 0x02;
	r = ::send(m_s,sendbuf,4,0);
	if(r<=0)
		return -1;
	r = ::recv(m_s,recvbuf,1024,0);
	if(r != 2 || *recvbuf != 0x05)
		return -1;
	if(*(recvbuf+1) == 0x02)//需要验证
	{
		char* pbuf = sendbuf;
		*pbuf++ = 0x01;
		*pbuf++ = strlen(name);
		strcpy(pbuf,name);
		pbuf += strlen(name);
		*pbuf++ = strlen(pwd);
		strcpy(pbuf,pwd);
		pbuf += strlen(pwd);
		r = ::send(m_s,sendbuf,pbuf-sendbuf,0);
		if(r<=0)
			return -1;
		r = ::recv(m_s,recvbuf,1024,0);
		if(r != 2 || *recvbuf != 0x01 || *(recvbuf+1) != 0x00)
			return -1;

	}
	else if(*(recvbuf+1) != 0x00)
	{
		return -1;
	}
	/*checkreq req;
	req.ver = 0x05;
	req.cmd = 0x03;//Socks命令有3种：CONNECT （编号0x01） BIND (0x02) UDP(编号0x03)
	req.reserved = 0;
	req.addrtype = 1;//0x01 该地址是IPv4地址，长4个8bit字节。0X03　　该地址包含一个完全的域名。第一个8bit字节包含了后面名称的8bit的数目，没有中止的\0。0X04 该地址是IPv6地址，长16个8bit字节。
	
	req.localip = localip;
	req.localport = localport;
	r = ::send(m_s,(char*)&req,sizeof(checkreq),0);
	if(r<=0)
		return -1;
	r = ::recv(m_s,recvbuf,1024,0);
	if(r <= 0)
		return -1;
	checkrsp* prs = (checkrsp*)recvbuf;
	if(prs->result != 0)
		return -1;
	m_udpagentip = prs->agentip;
	m_udpagentport = prs->agentport;*/
	return 0;
}

