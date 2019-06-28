#ifndef _SOCKS5_H
#define _SOCKS5_H

class CSocks5
{
public:
	CSocks5(UINT agentip,USHORT agentport);
	~CSocks5();
	int init(UINT localip,USHORT localport,char* name,char* pwd);
	UINT getudpagentip(){return m_udpagentip;}
	USHORT getudpagentport(){return m_udpagentport;}
private:
	SOCKET m_s;
	UINT m_agentip;
	USHORT m_agentport;
	UINT m_udpagentip;
	USHORT m_udpagentport;
	bool m_binited;
};




#endif