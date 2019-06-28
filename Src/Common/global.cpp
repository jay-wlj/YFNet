#include "qvod_string.h"
#include "qvod_sock.h"
#include <stdio.h>

#include "global.h"
#include "common-commonfun.h"
//#include "../include/stund.h"
#include "stund.h"
#include "common-utility.h"
#include "AutoPtr.h"
#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#endif

long				 g_ConnectingSock;
QVOD_DWORD				 g_dwWindowsMajorVersion;
QVOD_DWORD				 g_dwWindowsMinorVersion;
QVOD_DWORD				 g_dwPacketVersion;
unsigned char		 g_WindowsVersionCode;
unsigned short		 g_TerminalMinVersion = 0;
unsigned short		 g_TerminalProtocol = 0;
long				 g_connectinglmt;
QVOD_UINT g_uHostIP;
QVOD_USHORT g_listenPort;
int					 g_NATtype;
unsigned int		 g_iLocalIP;

#ifdef WIN32 /* WIN32 */
HRESULT WindowsFirewallAddApp( IN LPCTSTR fwProcessImageFileName, IN LPCTSTR fwName )
{
	return 0;
}


/****************************************************
*获取机器IP地址
*输入参数1：机器名称
*输出参数：机器32位IP地址
*返回值:true,成功，false失败
*
*
*********************************************************/
bool Qvod_Gethostbyname(IN char* name,OUT unsigned long& value)
{
#ifdef WIN32

#else
	struct addrinfo *pai = NULL;
	if(!getaddrinfo(name,NULL,NULL,&pai))
	{
		if(pai->ai_family == AF_INET)
		{
			value = ((struct sockaddr_in*)pai->ai_addr)->sin_addr.s_addr;
			return true;	
		}
	}
#endif
	return false;
}

void GetOperationVersion()
{
	UINT nSize;
	void* hMem,* pBuf;
	UINT pBufLen;
	QVOD_DWORD v1, v2, v3, v4;
	VS_FIXEDFILEINFO vs;

	// Get the Windows version.
	OSVERSIONINFO OSVersion;
	memset(&OSVersion,0,sizeof(OSVersion));
	OSVersion.dwOSVersionInfoSize = sizeof(OSVersion);
	BOOL re = GetVersionEx(&OSVersion);
	g_dwWindowsMajorVersion = OSVersion.dwMajorVersion; 
	g_dwWindowsMinorVersion = OSVersion.dwMinorVersion;
	g_dwPacketVersion = atoi(OSVersion.szCSDVersion+12);

	if (5 == g_dwWindowsMajorVersion && 0 == g_dwWindowsMinorVersion
		|| g_dwWindowsMajorVersion < 5)
	{
		g_WindowsVersionCode = WINDOWS_2000;	//win 2000及以下
	}
	if (5 == g_dwWindowsMajorVersion && 1 == g_dwWindowsMinorVersion)
	{
		switch(g_dwPacketVersion)
		{
		case 1:
			g_WindowsVersionCode = WINDOWS_XP_SP1;
			break;
		case 2:
			g_WindowsVersionCode = WINDOWS_XP_SP2;
			break;
		case 3:
			g_WindowsVersionCode = WINDOWS_XP_SP3;
			break;
		default:
			g_WindowsVersionCode = WINDOWS_XP;
		}
	}
	if (5 == g_dwWindowsMajorVersion && 2 == g_dwWindowsMinorVersion)
	{
		g_WindowsVersionCode = WINDOWS_2003;
	}
	if(6 == g_dwWindowsMajorVersion && 0 == g_dwWindowsMinorVersion)
	{
		switch(g_dwPacketVersion)
		{
		case 1:
			g_WindowsVersionCode = WINDOWS_VISTA_SP1;
			break;
		case 2:
			g_WindowsVersionCode = WINDOWS_VISTA_SP2;
			break;
		default:
			g_WindowsVersionCode = WINDOWS_VISTA;
		}
	}
	if (6 == g_dwWindowsMajorVersion && 1 == g_dwWindowsMinorVersion)
	{
		if (1 == g_dwPacketVersion)		
			g_WindowsVersionCode = WINDOWS_7_SP1;		
		else
			g_WindowsVersionCode = WINDOWS_7;			
	}

	//只取XP SP2、SP3和Vista SP1的半开连接数
	if(g_dwWindowsMajorVersion == 5 
		&& g_dwWindowsMinorVersion == 1 
		&& g_dwPacketVersion >= 2 ||
		g_dwWindowsMajorVersion == 6 
		&& g_dwWindowsMinorVersion == 0
		&& g_dwPacketVersion <= 1)
	{
		// Get tcpip.sys version.
		char syspath[260]={0};
		if(GetSystemDirectory(syspath,260) == 0) 
			return;
		strcat(syspath,"\\drivers\\tcpip.sys");

		nSize = GetFileVersionInfoSize(syspath, 0);
		if (nSize == 0) 
			return;

		hMem = LocalAlloc(0, nSize);
		if (GetFileVersionInfo(syspath, 0, nSize, hMem) )
		{

#ifdef WIN32 /* WIN32 */
			if (VerQueryValue(hMem, "\\", &pBuf, &pBufLen))
#else /* posix */
			if (VerQueryValue(hMem, "/", &pBuf, &pBufLen))
#endif /* posix end */

			{	
				memcpy(&vs, pBuf, pBufLen);

				v1 = vs.dwFileVersionMS>>16;
				v2 = vs.dwFileVersionMS&0x0000FFFF;
				v3 = vs.dwFileVersionLS>>16;
				v4 = vs.dwFileVersionLS&0x0000FFFF;
			}
		}
		LocalFree(hMem);
		
		int offset = 0;
		switch(v4)
		{
		case 2892:
			offset = 0x0004F5A2;
			break;
		case 2827:
			offset = 0x0004F5A2;
			break;
		case 2685:
			offset = 0x0004F5A2;
			break;
		case 2096:
			offset = 0X4EAB7;
			break;
		case 2149:
			offset = 0X4F4A2;
			break;
		case 2162:
			offset = 0X4F822;
			break;
		case 2180:
			offset = 0X4F322;
			break;
		case 3244:
			offset = 0x4F8C6;
			break;
		case 3394:				//XP SP2
			offset = 0x4F7A2;
			break;
		case 5512:				//XP SP3
			offset = 0x4FA46;
			break;
		case 5625:				//XP SP3
			offset = 0x4FB46;
			break;
		case 18000:				//Vista Ultimate SP1
			offset = 0x4C59D;
			break;			
		default:
			g_connectinglmt = 10;		//找不到对应版本，默认为10
			return;
		}
		FILE* pf = fopen(syspath,"rb");
		if(!pf) 
			return;
		UINT connectinglmt = 0;
		fseek(pf,offset,SEEK_SET);
		if (v4 == 18000)
		{
			fread(&connectinglmt,1,1,pf);
		}
		else
		{
			fread(&connectinglmt,1,sizeof(UINT),pf);
		}
		fclose(pf);
		g_connectinglmt = min(connectinglmt,255);
		printf("tcp connecting limit is %d\n",g_connectinglmt);
	}
}
void GetTerminalVersion()
{
	QVOD_DWORD v1, v2, v3, v4;
	char strModule[MAX_PATH]={0};
	GetModuleFileName(NULL, strModule, MAX_PATH);
	QVOD_DWORD size=GetFileVersionInfoSize(strModule,NULL);
	char *buffer = new char[size+1];		// 分配足够大内存储存信息
	if(!GetFileVersionInfo(strModule,NULL,size,buffer))
	{
		delete[] buffer;
		return ;
	}
	VS_FIXEDFILEINFO *FixDataInfo;
	UINT len;
	if(VerQueryValue(buffer,"\\",(LPVOID *)&FixDataInfo,&len))
	{
		v1 = FixDataInfo->dwFileVersionMS >> 16;
		v2 = FixDataInfo->dwFileVersionMS & 0x0000FFFF;
		v3 = FixDataInfo->dwFileVersionLS>>16;
		v4 = FixDataInfo->dwFileVersionLS & 0x0000FFFF;
		g_TerminalMinVersion = (unsigned short)v4;
	}
	delete[] buffer;
}

#endif /* WIN32 end */
bool IsLANIP(QVOD_DWORD nIP)
{
	// filter LAN IP's
	// -------------------------------------------
	// 0.*
	// 10.0.0.0 - 10.255.255.255  class A
	// 172.16.0.0 - 172.31.255.255  class B
	// 192.168.0.0 - 192.168.255.255 class C
	
	unsigned char nFirst = (unsigned char)nIP;
	unsigned char nSecond = (unsigned char)(nIP >> 8);
	
	if (nFirst==192 && nSecond==168) // check this 1st, because those LANs IPs are mostly spreaded
		return true;
	
	if (nFirst==172 && nSecond>=16 && nSecond<=31)
		return true;
	
	if (nFirst==0 || nFirst==10)
		return true;
	
	if(nFirst==127)
		return true;
	
	if(nFirst==169 && nSecond==254)
		return true;
	
	return false; 
}
QVOD_UINT GetLocalIP()
{
#ifdef WIN32 /* WIN32 */
#if 1
	IP_ADAPTER_INFO *pInfo = NULL;
	IP_ADAPTER_INFO *pNICInfo = NULL;
	unsigned long ulLength = 0;
	GetAdaptersInfo(pInfo,&ulLength);
	if ( ulLength == 0 )
	{
		//所有网卡都被禁用,或者笔记本的无线网卡还没启动
		return 0;
	}

	QPtr::AutoArrayPtr<char> pBuf = new char[ulLength];
	pInfo = (IP_ADAPTER_INFO*)(char*)pBuf;
	GetAdaptersInfo(pInfo,&ulLength);
	pNICInfo = pInfo;
	do 
	{
		if ( strstr(pNICInfo->Description,"Microsoft Loopback Adapter") != NULL )
		{
			//微软本地回环网卡
			pNICInfo = pNICInfo->Next;
		}
		else if ( strstr(pNICInfo->Description,"VMware Virtual") != NULL )
		{
			//VMware 虚拟网卡
			pNICInfo = pNICInfo->Next;
		}
		else if (memcmp(pNICInfo->IpAddressList.IpAddress.String,"0.0.0.0",7) == 0)
		{
			pNICInfo = pNICInfo->Next;
		}
		else
		{
			break;
		}
	} while (pNICInfo);
	
	if ( pNICInfo == NULL )
	{
		//只有回环或者虚拟网卡
		g_iLocalIP = 0;
	}
	else
	{
		g_iLocalIP = inet_addr(pNICInfo->IpAddressList.IpAddress.String);
	}
#else
	hostent* phost;
	char buf[256]={0};
	int buflen = 256;
	if (SOCKET_ERROR == gethostname(buf,buflen))
		return 0;
	phost = gethostbyname(buf);
	if (!phost)
		return 0;
	int naddr = 0;
	while (phost->h_addr_list[naddr])
	{
		if(!IsLANIP(*(UINT*)phost->h_addr_list[naddr]))
		{
			memcpy(&g_iLocalIP,phost->h_addr_list[naddr],phost->h_length);
			break;
		}
		naddr++;
	}
	if(!phost->h_addr_list[naddr])
	{
		memcpy(&g_iLocalIP,phost->h_addr_list[0],phost->h_length);
	}
#endif
	return g_iLocalIP;

#else /* posix */

	int res, sockfd;
	char ip_str[32] = {0};
	const int MAXINTERFACES = 16;
	struct ifreq buf[MAXINTERFACES];
	struct sockaddr_in *host = NULL;

	sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "%s(%d)-%s: socket error %d!\n", __FILE__, __LINE__, __FUNCTION__, errno);
#endif // QVODDEBUG
        return 0;
    }

	struct ifconf ifc;
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;

	/* get all the interfaces */
	res = ioctl(sockfd, SIOCGIFCONF, (char*)&ifc);
	if(res < 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "%s(%d)-%s: ioctl error!\n", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
		return 0;
	}
	int num = ifc.ifc_len / sizeof(struct ifreq);

	while(num-- > 0)
	{
		/* get ip of the interface */
		res = ioctl(sockfd, SIOCGIFADDR, (char*)&buf[num]);
		if(res < 0)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "%s(%d)-%s: ioctl error!\n", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
			return 0;
		}
		memset(ip_str, 0, sizeof(ip_str));
		host = (struct sockaddr_in*)(&buf[num].ifr_addr);
		inet_ntop(AF_INET, &host->sin_addr, ip_str, sizeof(ip_str));
		if(strcmp(ip_str, "127.0.0.1") != 0 && strcmp(ip_str, "10.9.8.1") != 0)
		{
			/* not 127.0.0.1 */
#ifdef QVODDEBUG
			_OUTPUT(INFO, "get a local ip %s\n", ip_str);
#endif // QVODDEBUG
			break;
		}
	}

	g_iLocalIP = host->sin_addr.s_addr;
	g_uHostIP = g_iLocalIP;
	memset(ip_str, 0, sizeof(ip_str));
	inet_ntop(AF_INET, &host->sin_addr, ip_str, sizeof(ip_str));
#ifdef QVODDEBUG
	_OUTPUT(INFO, "----------g_uHostIP = %s\n", ip_str);
#endif // QVODDEBUG

	close(sockfd);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "----------close socket %d\n", sockfd);
#endif // QVODDEBUG

    return g_iLocalIP;

#endif /* posix end */
}

int GetNatType()
{
	int iNatType = 0;
	iNatType = stun(STUNSERVER,0); 
	iNatType &= 0x00000007;
	if(iNatType >= NatTypeBlocked)
	{
		iNatType = stun(STUN1SERVER,0); 
		iNatType &= 0x00000007;
	}
	g_NATtype = iNatType;
	return g_NATtype;
}

int InitQvod()
{
#ifdef WIN32 /* WIN32, silver */
	WSADATA wsatata;
	WSAStartup(MAKEWORD(2,2),&wsatata);
	
	//获取设置系统信息
	GetOperationVersion();
	//获取terminal版本号
	GetTerminalVersion();
	char strModule[MAX_PATH]={0};
	GetModuleFileName(NULL, strModule, MAX_PATH);
	
	//初始化防火墙
	if(g_dwWindowsMajorVersion == 5 && g_dwWindowsMinorVersion == 1 && g_dwPacketVersion <= 1)//XP SP1及以下
	{
		//DisableWindowsXPSP1Firewall();
	}
	else if(g_dwWindowsMajorVersion == 5 && g_dwWindowsMinorVersion >= 1)//XP SP2及2003 SERVER以上
	{
		WindowsFirewallAddApp(strModule,QVODPLAYER_NAME);
	}
	else if ( g_dwWindowsMajorVersion == 6 && (g_dwWindowsMinorVersion == 0 || g_dwWindowsMinorVersion == 1) )
	{
		//Windows vista or Windows 7,简体中文
		WindowsFirewallAddApp(strModule,QVODPLAYER_NAME);
	}
	else
	{
		//2000及以下无防火墙
	}
#endif /* WIN32 end */
	
	//GetNatType();
	return 0;
}