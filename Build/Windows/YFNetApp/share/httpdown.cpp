#ifndef WINVER 
#define WINVER	0x0500
#endif
#ifndef _WIN32_WINNT 
#define _WIN32_WINNT	0x0500
#endif
#ifndef _WIN32_IE 
#define _WIN32_IE		0x0510
#endif
#include <WINSOCK2.H>
#include <tchar.h>
#include <stdio.h>
#include <Iphlpapi.h>
//#include <string>
//using namespace std;
#include "..\QvodPlayer\stdafx.h"
#include "..\QvodPlayer\Define.h"
#include "..\QvodPlayer\sha1.h"
#include "httpdown.h"

CHttpDown::CHttpDown(char *url, char *savefile)
{
	m_strurl = NULL;
	int nlen = strlen(url) + 1;
	if(nlen > 1)
	{
		m_strurl = new char[nlen];
		memset(m_strurl, 0, nlen);
		memcpy(m_strurl, url, nlen - 1);
	}
	m_strsavefile = NULL;
	nlen = strlen(savefile) + 1;
	if(nlen > 1)
	{
		m_strsavefile = new char[nlen];
		memset(m_strsavefile, 0, nlen);
		memcpy(m_strsavefile, savefile, nlen - 1);
	}
	m_sockt = INVALID_SOCKET;
	
}

CHttpDown::~CHttpDown()
{
	if(m_sockt != INVALID_SOCKET)
		closesocket(m_sockt);
	m_sockt = INVALID_SOCKET;
	SafeDelete(m_strurl);
	SafeDelete(m_strsavefile);
}

#define HTTPLINE _T("http://")

#define BUFSIZE 1024*16//1024*1024

int CHttpDown::Run()
{
const char* fmtget =  "GET /%s HTTP/1.1\r\nAccept: application/vnd.ms-powerpoint, application/msword, */*\r\n" \
"Accept-Language: zh-cn\r\n" \
"Accept-Encoding: deflate\r\n" \
"User-Agent: QvodPlayer\r\n" \
"Host: %s\r\n" \
"Connection: Keep-Alive\r\n\r\n";

	SOCKADDR_IN RemoteAddr;
	memset(&RemoteAddr, 0, sizeof(RemoteAddr));
	unsigned long  ip = 0;
	unsigned short port = 0;
	if(0 != GetIpFromUrl(m_strurl, ip, port))
	{
		return -1;
	}
	RemoteAddr.sin_family = AF_INET;
	RemoteAddr.sin_port = port;
	RemoteAddr.sin_addr.S_un.S_addr = ip;
	m_sockt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int nMs = 3000;
	setsockopt(m_sockt,SOL_SOCKET, SO_SNDTIMEO, (char*)&nMs, sizeof(int));
	setsockopt(m_sockt,SOL_SOCKET, SO_RCVTIMEO, (char*)&nMs, sizeof(int));
	if(0 != connect(m_sockt, (SOCKADDR *)&RemoteAddr, sizeof(RemoteAddr)))
	{
		return -1;
	}
	const char *pline = strstr((m_strurl + 7), "/");
	int nlen = pline - (m_strurl + 7) + 1;
	char *pstrhost = new char[nlen];
	memset(pstrhost, 0, nlen);
	strncpy(pstrhost, (m_strurl + 7), (nlen - 1));
	nlen = strlen(fmtget) + strlen(m_strurl) + MAX_PATH;
	char *sendbuf = new char[nlen];
	memset(sendbuf, 0, nlen);
	nlen = sprintf(sendbuf, fmtget, pline+1, pstrhost);
	SafeDelete(pstrhost);
	if(0 != SendMsg(m_sockt, sendbuf, nlen))
	{
		SafeDelete(sendbuf);
		return -1;	
	}
	SafeDelete(sendbuf);
	nlen = BUFSIZE;
	char *recvbuf = new char[nlen];
	if(!recvbuf)
	{
		nlen = 1024*8;
		recvbuf = new char[nlen];
		if(!recvbuf)
			return -1;
	}
	if(0 != RecvMsg(m_sockt, recvbuf, nlen, m_strsavefile))
	{
		SafeDelete(recvbuf);
		return -1;
	}
	SafeDelete(recvbuf);
	return 0;
}

int CHttpDown::RunCheckTim(char *pmodifytime)
{
const char* fmtget =  "GET /%s HTTP/1.1\r\nAccept: application/vnd.ms-powerpoint, application/msword, */*\r\n" \
"Accept-Language: zh-cn\r\n" \
"Accept-Encoding: deflate\r\n" \
"If-Modified-Since: %s\r\n" \
"User-Agent: QvodPlayer\r\n" \
"Host: %s\r\n" \
"Connection: Keep-Alive\r\n\r\n";

	SOCKADDR_IN RemoteAddr;
	memset(&RemoteAddr, 0, sizeof(RemoteAddr));
	unsigned long  ip = 0;
	unsigned short port = 0;
	if(0 != GetIpFromUrl(m_strurl, ip, port))
	{
		return -1;
	}
	RemoteAddr.sin_family = AF_INET;
	RemoteAddr.sin_port = port;
	RemoteAddr.sin_addr.S_un.S_addr = ip;
	m_sockt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int nMs = 3000;
	setsockopt(m_sockt,SOL_SOCKET, SO_SNDTIMEO, (char*)&nMs, sizeof(int));
	setsockopt(m_sockt,SOL_SOCKET, SO_RCVTIMEO, (char*)&nMs, sizeof(int));
	if(0 != connect(m_sockt, (SOCKADDR *)&RemoteAddr, sizeof(RemoteAddr)))
	{
		return -1;
	}
	const char *pline = strstr((m_strurl + 7), "/");
	int nlen = pline - (m_strurl + 7) + 1;
	char *pstrhost = new char[nlen];
	memset(pstrhost, 0, nlen);
	strncpy(pstrhost, (m_strurl + 7), (nlen - 1));
	nlen = strlen(fmtget) + strlen(m_strurl) + MAX_PATH;
	char *sendbuf = new char[nlen];
	memset(sendbuf, 0, nlen);
	nlen = sprintf(sendbuf, fmtget, pline+1, pmodifytime, pstrhost);
	SafeDelete(pstrhost);
	if(0 != SendMsg(m_sockt, sendbuf, nlen))
	{
		SafeDelete(sendbuf);
		return -1;	
	}
	SafeDelete(sendbuf);
	nlen = BUFSIZE;
	char *recvbuf = new char[nlen];
	if(!recvbuf)
	{
		nlen = 1024*8;
		recvbuf = new char[nlen];
		if(!recvbuf)
			return -1;
	}
	int nresult = RecvMsg(m_sockt, recvbuf, nlen, m_strsavefile);
	SafeDelete(recvbuf);
	return nresult;

}

int CHttpDown::SendMsg(SOCKET s, char *buffer, int len)
{

	int nlen = len;
	const char* pbuf = buffer;
	int sendlen;
	while (nlen >0)
	{
		sendlen =send(s,pbuf, nlen, 0);
		while (sendlen == SOCKET_ERROR && WSAEWOULDBLOCK == WSAGetLastError())
		{
			return -1;		
		}
		if (sendlen <= 0)
		{
			return -1;
		}
		nlen -= sendlen;
		pbuf += sendlen;
	}
	return 0;
}

int CHttpDown::RecvMsg(SOCKET s, char *buffer, int len, char *psavefile)
{
	int ntotallen = len;
	int nlen = 0;
	char *pbuf = buffer;
	nlen = recv(s, pbuf, ntotallen, 0);
	if(nlen <= 0)
		return -1;
	pbuf += nlen;
	ntotallen -= nlen;
	int nrecvlen = pbuf - buffer;
	if(nrecvlen <= 0)
		return -1;
	buffer[nrecvlen] = 0;
	char *buf = buffer;
	char *pconlen = strstr(buf, "Content-Length: ");
	if(strstr(buf, "200 OK\r\n") && pconlen)
	{
		pconlen += strlen("Content-Length: ");
		char *plenend = strstr(pconlen, "\r\n");
		int   nconlen = plenend - pconlen + 1;
		char *pcon = new char[nconlen];
		memset(pcon, 0, nconlen);
		memcpy(pcon, pconlen, (nconlen - 1));
		int ndatatotal = atoi(pcon);
		SafeDelete(pcon);
		char *ptowline = strstr(buf, "\r\n\r\n");
		ptowline += 4;
		int nheadlen = ptowline - buffer;
		int ndatalen = pbuf - ptowline;
		int nrecvdatalen = ndatatotal - ndatalen;
		FILE *file = fopen(psavefile, "w+b");
		if(file && ndatalen > 0)
		{
			fwrite(ptowline, 1, ndatalen, file);
		}
		while (nrecvdatalen > 0 && ntotallen > 0)
		{
			pbuf = buffer;
			nlen = recv(s, pbuf, ntotallen, 0);
			if(nlen == 0)
				break;
			if(nlen < 0 && WSAETIMEDOUT == WSAGetLastError())
			{
				Sleep(500);
				continue;
			}
			if(file)
				fwrite(pbuf, 1, nlen, file);
			//pbuf += nlen;
			//ntotallen -= nlen;
			nrecvdatalen -= nlen;
			//ndatalen += nlen;
		}
		if(file)
		{
			fclose(file);
			file = NULL;
		}		

		return 0;
	}
	else if(strstr(buf, "404 Not Found\r\n"))
	{
		return -2;
	}
	else if(strstr(buf, "304 Not Modified\r\n"))
	{
		return -4;
	}
	return -3;
}

int CHttpDown::GetIpFromUrl(const char *url, unsigned long& ip, unsigned short& port)
{
	if(url == NULL)
		return -1;
	CString strurl = url;
	if(strurl.Find(HTTPLINE) != 0)
		return -1;
	CString stripport  = strurl.Right(strurl.GetLength() - 7);
	int npos = stripport.Find('/');
	CString stripport1;
	if(npos > 0)
		stripport1 = stripport.Left(npos);
	else
		stripport1 = stripport;
	npos = stripport1.Find(':');
	CString strport;
	CString strip;
	if(npos > 0)
	{
		strport = stripport1.Right(stripport1.GetLength() - npos - 1);
		strip = stripport1.Left(npos);
	}
	else
	{
		strport = "80";
		strip = stripport1;
	}
	USES_CONVERSION;
	char *p = T2A(strport);
	port = htons(atoi(p));
	p = T2A(strip);
	hostent *phost = gethostbyname(p);
	if(!phost)
	{
		return -1;
	}
	in_addr ipaddr;
	memcpy(&ip, phost->h_addr_list[0], phost->h_length);
	ipaddr.S_un.S_addr = ip;
	return 0;
}

CHttpSend::CHttpSend(char *url)
{
	m_strurl = NULL;
	char pmac[MAX_PATH];
	memset(pmac, 0, MAX_PATH);
	GetMacAdder(pmac);
	CString strID = QueryCpuID(0);
	USES_CONVERSION;
	char *pstrid = T2A(strID);
	int nlen = strlen(url) + 1 + strlen(pmac) + strlen(pstrid) + 100;
	if(nlen > 1)
	{
		m_strurl = new char[nlen];
		memset(m_strurl, 0, nlen);
		//memcpy(m_strurl, url, nlen - 1);
		sprintf(m_strurl, url, pmac, pstrid);
	}
	m_sockt = INVALID_SOCKET;
	
}

CHttpSend::~CHttpSend()
{
	if(m_sockt != INVALID_SOCKET)
		closesocket(m_sockt);
	m_sockt = INVALID_SOCKET;
	SafeDelete(m_strurl);
}

#define HTTPLINE _T("http://")

#define BUFSIZE_LEN 1024*4//1024*1024

int CHttpSend::Run()
{
const char* fmtget =  "GET /%s HTTP/1.1\r\nAccept: application/vnd.ms-powerpoint, application/msword, */*\r\n" \
"Accept-Language: zh-cn\r\n" \
"Accept-Encoding: deflate\r\n" \
"User-Agent: QvodPlayer\r\n" \
"Host: %s\r\n" \
"Connection: Keep-Alive\r\n\r\n";
	SOCKADDR_IN RemoteAddr;
	memset(&RemoteAddr, 0, sizeof(RemoteAddr));
	unsigned long  ip = 0;
	unsigned short port = 0;
	if(0 != GetIpFromUrl(m_strurl, ip, port))
	{
		return -1;
	}
	RemoteAddr.sin_family = AF_INET;
	RemoteAddr.sin_port = port;
	RemoteAddr.sin_addr.S_un.S_addr = ip;
	m_sockt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int nMs = 4000;
	setsockopt(m_sockt,SOL_SOCKET, SO_SNDTIMEO, (char*)&nMs, sizeof(int));
	setsockopt(m_sockt,SOL_SOCKET, SO_RCVTIMEO, (char*)&nMs, sizeof(int));
	if(0 != connect(m_sockt, (SOCKADDR *)&RemoteAddr, sizeof(RemoteAddr)))
	{
		return -1;
	}
	const char *pline = strstr((m_strurl + 7), "/");
	int nlen = pline - (m_strurl + 7) + 1;
	char *pstrhost = new char[nlen];
	memset(pstrhost, 0, nlen);
	strncpy(pstrhost, (m_strurl + 7), (nlen - 1));
	nlen = strlen(fmtget) + strlen(m_strurl) + MAX_PATH;
	char *sendbuf = new char[nlen];
	memset(sendbuf, 0, nlen);
	nlen = sprintf(sendbuf, fmtget, pline+1, pstrhost);
	SafeDelete(pstrhost);
#if _DEBUG
	con<<_T("\r\nsendbuf\t")<<sendbuf;
#endif
	if(0 != SendMsg(m_sockt, sendbuf, nlen))
	{
		SafeDelete(sendbuf);
		return -1;	
	}
	SafeDelete(sendbuf);
	nlen = BUFSIZE_LEN;
	char *recvbuf = new char[nlen];
	if(!recvbuf)
	{
		nlen = 1024*2;
		recvbuf = new char[nlen];
		if(!recvbuf)
			return -1;
	}
	if(0 != RecvMsg(m_sockt, recvbuf, nlen))
	{
		SafeDelete(recvbuf);
		return -1;
	}
	SafeDelete(recvbuf);
	return 0;
}

int CHttpSend::SendMsg(SOCKET s, char *buffer, int len)
{

	int nlen = len;
	const char* pbuf = buffer;
	int sendlen;
	while (nlen >0)
	{
		sendlen =send(s,pbuf, nlen, 0);
		while (sendlen == SOCKET_ERROR && WSAEWOULDBLOCK == WSAGetLastError())
		{
			return -1;		
		}
		if (sendlen <= 0)
		{
			return -1;
		}
		nlen -= sendlen;
		pbuf += sendlen;
	}
	return 0;
}

int CHttpSend::RecvMsg(SOCKET s, char *buffer, int len)
{
	int ntotallen = len - 1;
	int nlen = 0;
	char *pbuf = buffer;
	nlen = recv(s, pbuf, ntotallen, 0);
	if(nlen <= 0)
		return -1;
	pbuf += nlen;
	ntotallen -= nlen;
	int nrecvlen = pbuf - buffer;
	if(nrecvlen <= 0)
		return -1;
	buffer[nrecvlen] = 0;
	return 0;
}

int CHttpSend::GetIpFromUrl(const char *url, unsigned long& ip, unsigned short& port)
{
	if(url == NULL)
		return -1;
	CString strurl = url;
	if(strurl.Find(HTTPLINE) != 0)
		return -1;
	CString stripport  = strurl.Right(strurl.GetLength() - 7);
	int npos = stripport.Find('/');
	CString stripport1;
	if(npos > 0)
		stripport1 = stripport.Left(npos);
	else
		stripport1 = stripport;
	npos = stripport1.Find(':');
	CString strport;
	CString strip;
	if(npos > 0)
	{
		strport = stripport1.Right(stripport1.GetLength() - npos - 1);
		strip = stripport1.Left(npos);
	}
	else
	{
		strport = "80";
		strip = stripport1;
	}
	USES_CONVERSION;
	char *p = T2A(strport);
	port = htons(atoi(p));
	p = T2A(strip);
	hostent *phost = gethostbyname(p);
	if(!phost)
	{
		return -1;
	}
	in_addr ipaddr;
	memcpy(&ip, phost->h_addr_list[0], phost->h_length);
	ipaddr.S_un.S_addr = ip;
	return 0;
}

int CHttpSend::GetMacAdder(char *pmac)
{
	DWORD dwOutBufLen=0; 
	PIP_ADAPTER_INFO pAdapterInfo=NULL; 
	int iReturn; 
	iReturn=GetAdaptersInfo(pAdapterInfo,&dwOutBufLen); 
	if(iReturn!=ERROR_BUFFER_OVERFLOW) 
	{ 
		strcpy(pmac, "00-00-00-00-00-00");
		return -1; 
	} 
	pAdapterInfo =(PIP_ADAPTER_INFO) HeapAlloc(GetProcessHeap(), 0, dwOutBufLen); 
	iReturn=GetAdaptersInfo(pAdapterInfo,&dwOutBufLen); 
	if(iReturn!=ERROR_SUCCESS) 
	{ 
		strcpy(pmac, "00-00-00-00-00-00");
		HeapFree(GetProcessHeap(), 0, pAdapterInfo); 
		return -1; 
	} 
	if(!pAdapterInfo)
	{
		strcpy(pmac, "00-00-00-00-00-00");
		HeapFree(GetProcessHeap(), 0, pAdapterInfo); 
		return -1;

	}
	sprintf(pmac, "%02X-%02X-%02X-%02X-%02X-%02X", 
		pAdapterInfo->Address[0], 
		pAdapterInfo->Address[1], 
		pAdapterInfo->Address[2], 
		pAdapterInfo->Address[3], 
		pAdapterInfo->Address[4], 
		pAdapterInfo->Address[5] ); 
	HeapFree(GetProcessHeap(), 0, pAdapterInfo); 
	return 0;	
}

CString CHttpSend::QueryCpuID(int nreturntype)
{
	CString strreturn;
	CString strCpuID = CpuID();
	CString strMacID = MacID();
	CString strDiskID = DiskID();
	CString strID = strCpuID + strMacID + strDiskID;
	USES_CONVERSION;
	char *pstrID = T2A(strID);
	int nlen = strlen(pstrID);
	BYTE *pbyteID = new BYTE[nlen];
	memcpy(pbyteID, pstrID, nlen);
	CSHA1 sha1;
	sha1.write(pbyteID, nlen);
	BYTE pbyte[20];
	sha1.read(pbyte);
	CString strIDtemp = Byte8To32Str(pbyte);
	return strIDtemp;
}

CString CHttpSend::CpuID()
{
	unsigned   long   s1,s2;     
	CString   VernderID;   
	UCHAR	  vendor_id[]="------------";
	CString   MyCpuID,CPUID1,CPUID2;    
	__asm{   
			xor   eax,eax   
			cpuid   
			mov   dword   ptr   vendor_id,ebx   
			mov   dword   ptr   vendor_id[+4],edx   
			mov   dword   ptr   vendor_id[+8],ecx   
	}   
	VernderID.Format(_T("%s-"),vendor_id);   
	__asm{   
			mov   eax,01h   
			xor   edx,edx   
			cpuid   
			mov   s1,edx   
			mov   s2,eax   
	}   
	CPUID1.Format(_T("%08X%08X"),s1,s2);   
	__asm{   
			mov   eax,03h   
			xor   ecx,ecx   
			xor   edx,edx   
			cpuid   
			mov   s1,edx   
			mov   s2,ecx   
	}   
	CPUID2.Format(_T("%08X%08X"),s1,s2);   
	MyCpuID   =   CPUID1+CPUID2;   
	return MyCpuID;
}

CString CHttpSend::MacID()
{
	CString   MyMacID;
	DWORD dwOutBufLen=0; 
	PIP_ADAPTER_INFO pAdapterInfo=NULL; 
	int iReturn; 
	iReturn=GetAdaptersInfo(pAdapterInfo,&dwOutBufLen); 
	if(iReturn!=ERROR_BUFFER_OVERFLOW) 
	{ 
		MyMacID = _T("000000000000");
		return MyMacID; 
	} 
	pAdapterInfo =(PIP_ADAPTER_INFO) HeapAlloc(GetProcessHeap(), 0, dwOutBufLen); 
	iReturn=GetAdaptersInfo(pAdapterInfo,&dwOutBufLen); 
	if(iReturn!=ERROR_SUCCESS) 
	{ 
		MyMacID = _T("000000000000");
		HeapFree(GetProcessHeap(), 0, pAdapterInfo); 
		return MyMacID; 
	} 
	if(!pAdapterInfo)
	{
		MyMacID = _T("000000000000");
		HeapFree(GetProcessHeap(), 0, pAdapterInfo); 
		return MyMacID;
	}
	MyMacID.Format(_T("%02X%02X%02X%02X%02X%02X"), 
		pAdapterInfo->Address[0], 
		pAdapterInfo->Address[1], 
		pAdapterInfo->Address[2], 
		pAdapterInfo->Address[3], 
		pAdapterInfo->Address[4], 
		pAdapterInfo->Address[5] ); 
	HeapFree(GetProcessHeap(), 0, pAdapterInfo); 
	return	  MyMacID;
}

typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

#define  DFP_GET_VERSION          0x00074080
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088
//  Valid values for the bCommandReg member of IDEREGS.
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.

CString CHttpSend::DiskID()
{
	CString strDiskID;
	int drive = 0;
	HANDLE hPhysicalDriveIOCTL = 0;
	TCHAR driveName [256];
	_stprintf (driveName, _T("\\\\.\\PhysicalDrive%d"), drive);
	hPhysicalDriveIOCTL = CreateFile (driveName,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, 0, NULL);
	if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		GETVERSIONOUTPARAMS VersionParams;
		DWORD               cbBytesReturned = 0;
        // Get the version, etc of PhysicalDrive IOCTL
		memset ((void*) &VersionParams, 0, sizeof(VersionParams));
		
		if ( ! DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
			NULL, 
			0,
			&VersionParams,
			sizeof(VersionParams),
			&cbBytesReturned, NULL) )
		{         
			
		}
		if (VersionParams.bIDEDeviceMap > 0)
		{
			BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
			SENDCMDINPARAMS  scip;
			BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
			bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
			
			memset (&scip, 0, sizeof(scip));
			memset (IdOutCmd, 0, sizeof(IdOutCmd));
			
			if ( DoIDENTIFY (hPhysicalDriveIOCTL, 
				&scip, 
				(PSENDCMDOUTPARAMS)&IdOutCmd, 
				(BYTE) bIDCmd,
				(BYTE) drive,
				&cbBytesReturned))
			{
				USHORT *pIdSector = (USHORT *)
					((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;
				pIdSector += 27;
				char *p = (char *)pIdSector;
				char p1[13] = {0};
				memcpy(p1, p, 12);
				strDiskID = p1;
			}
		}
		CloseHandle (hPhysicalDriveIOCTL);
	}
	return strDiskID;
}

BOOL CHttpSend::DoIDENTIFY(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
							PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
							PDWORD lpcbBytesReturned)
{
	// Set up data structures for IDENTIFY command.
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP -> irDriveRegs.bFeaturesReg = 0;
	pSCIP -> irDriveRegs.bSectorCountReg = 1;
	pSCIP -> irDriveRegs.bSectorNumberReg = 1;
	pSCIP -> irDriveRegs.bCylLowReg = 0;
	pSCIP -> irDriveRegs.bCylHighReg = 0;
	
	// Compute the drive number.
	pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);
	
	// The command can either be IDE identify or ATAPI identify.
	pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
	pSCIP -> bDriveNumber = bDriveNum;
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
	
	return( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		(LPVOID) pSCIP,
		sizeof(SENDCMDINPARAMS) - 1,
		(LPVOID) pSCOP,
		sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL) );
}

CString CHttpSend::Byte8To32Str(BYTE *psrcbyte)
{//8字节转成32字节显示
	BYTE b0 = psrcbyte[0], b1 = psrcbyte[1] ,b2 = psrcbyte[2] ,b3 = psrcbyte[3] ,b4 = psrcbyte[4] ,b5 = psrcbyte[5] ,b6 = psrcbyte[6] ,b7 = psrcbyte[7] ;
	int  i[13];
	memset(i, 0, sizeof(int)*8);
	i[0] = b0>>3;
	i[1] = (b0 & 0x7)<<2; i[1] += (b1 >> 6);
	i[2] = (b1 & 0x3F)>>1; 
	i[3] = (b1 & 0x1)<<4; i[3] += (b2 >> 4);
	i[4] = (b2 & 0xF)<<1; i[4] += (b3 >> 7);
	i[5] = (b3 & 0x7F)>>2;
	i[6] = (b3 & 0x3)<<3; i[6] += (b4 >> 5);
	i[7] = b4 &0x1F;
	i[8] = b5>>3;
	i[9] = (b5 & 0x7)<<2; i[9] += (b6 >> 6);
	i[10] = (b6 & 0x3F)>>1;
	i[11] = (b6 & 0x1)<<4; i[11] += (b7 >> 4);
	i[12] = b7&0xF;
	CString str1;
	CString strtemp;
	for (int k = 0; k < 13; k++)
	{
		strtemp.Empty();
		if(i[k] >= 0 && i[k] <= 9)
		{
			strtemp.Format(_T("%d"), i[k]);
		}
		else if(i[k] >= 10 && i[k] <= 31)
		{
			strtemp.Format(_T("%c"), i[k] + 55);
		}
		str1 += strtemp;
	}
	return str1;
}




/*int CHttpSend::GetMacAdder(char *pmac)
{
	typedef struct _ASTAT_ 
	{ 
		ADAPTER_STATUS adapt; 
		NAME_BUFFER NameBuff [30]; 
	}ASTAT, * PASTAT; 
	NCB ncb; 
	UCHAR uRetCode; 
	ASTAT Adapter; 
	memset( &ncb, 0, sizeof(ncb) ); 
	ncb.ncb_command = NCBRESET; 
	ncb.ncb_lana_num = 0; 
	uRetCode = Netbios( &ncb ); 
	memset( &ncb, 0, sizeof(ncb) ); 
	ncb.ncb_command = NCBASTAT; 
	ncb.ncb_lana_num = 0;
	strcpy( (char *)ncb.ncb_callname, "* " ); 
	ncb.ncb_buffer = (unsigned char *)&Adapter; 
	ncb.ncb_length = sizeof(Adapter); 
	uRetCode = Netbios( &ncb ); 
	if ( uRetCode == 0 ) 
	{ 
		sprintf(pmac, "%02X-%02X-%02X-%02X-%02X-%02X", 
			Adapter.adapt.adapter_address[0], 
			Adapter.adapt.adapter_address[1], 
			Adapter.adapt.adapter_address[2], 
			Adapter.adapt.adapter_address[3], 
			Adapter.adapt.adapter_address[4], 
			Adapter.adapt.adapter_address[5] ); 
		return 0;
	} 
	else
	{
		//strcpy(pmac, "00-00-00-00-00-00");
		char lpszMac[128];
		memset(lpszMac, 0, 128);
		if(GetMacByCmd(lpszMac))
		{
			strncpy(pmac, lpszMac, 17);
		}
		else
		{
			strcpy(pmac, "00-00-00-00-00-00");
		}
	}
	return -1;
}

const long MAX_COMMAND_SIZE = 10000;
TCHAR szFetCmd[] = _T("ipconfig /all");
const string str4Search = "Physical Address. . . . . . . . . : ";

BOOL CHttpSend::GetMacByCmd(char *lpszMac)
{
	BOOL bret; 
	SECURITY_ATTRIBUTES sa; 
    HANDLE hReadPipe,hWritePipe; 
    sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
    sa.lpSecurityDescriptor = NULL; 
    sa.bInheritHandle = TRUE; 
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if(!bret)
	{
		return FALSE;
	}
	STARTUPINFO si; 
    PROCESS_INFORMATION pi; 
    si.cb = sizeof(STARTUPINFO); 
    GetStartupInfo(&si); 
    si.hStdError = hWritePipe; 
    si.hStdOutput = hWritePipe; 
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	bret = CreateProcess (NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ); 
	char szBuffer[MAX_COMMAND_SIZE+1];
	string strBuffer;
	if (bret) 
	{ 
		WaitForSingleObject (pi.hProcess, INFINITE); 
		unsigned long count;
		CloseHandle(hWritePipe);
		memset(szBuffer, 0x00, sizeof(szBuffer));
		bret  =  ReadFile(hReadPipe,  szBuffer,  MAX_COMMAND_SIZE,  &count,  0);
		if(!bret)
		{
			//CloseHandle(hWritePipe);
			CloseHandle(pi.hProcess); 
			CloseHandle(pi.hThread); 
			//CloseHandle(hReadPipe);
			return FALSE;
		}
		else
		{
			strBuffer = szBuffer;
			long ipos;
			ipos = strBuffer.find(str4Search);
			if(ipos < 0)
			{
				CloseHandle(pi.hProcess); 
				CloseHandle(pi.hThread); 
				return FALSE;
			}
			//提取MAC地址串
			strBuffer = strBuffer.substr(ipos+str4Search.length());
			ipos = strBuffer.find("\n");
			strBuffer = strBuffer.substr(0, ipos);
		}
		
	}
	strcpy(lpszMac, strBuffer.c_str());
	CloseHandle(pi.hProcess); 
	CloseHandle(pi.hThread); 
	//CloseHandle(hReadPipe);
	//CloseHandle(hWritePipe);
	return TRUE;
}*/