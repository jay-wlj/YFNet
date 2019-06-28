#ifndef _HTTPDOWN_H
#define _HTTPDOWN_H

class CHttpDown
{
public:
	CHttpDown(char *url, char *savefile);
	~CHttpDown();
public:
	int		GetIpFromUrl(const char *url, unsigned long& ip, unsigned short& port);
	int		SendMsg(SOCKET s, char *buffer, int len);
	int		RecvMsg(SOCKET s, char *buffer, int len, char *psavefile);
	int		Run();
	int     RunCheckTim(char *pmodifytime);
protected:
	
private:
	SOCKET	m_sockt;
	char	*m_strurl;
	char	*m_strsavefile;
};

class CHttpSend
{
public:
	CHttpSend(char *url);
	~CHttpSend();
public:
	int		GetIpFromUrl(const char *url, unsigned long& ip, unsigned short& port);
	int		SendMsg(SOCKET s, char *buffer, int len);
	int		RecvMsg(SOCKET s, char *buffer, int len);
	int		Run();
	int		GetMacAdder(char *pmac);
	BOOL	GetMacByCmd(char *lpszMac);
	CString QueryCpuID(int nreturntype);
	CString	CpuID();
	CString	MacID();
	CString	DiskID();
	BOOL	DoIDENTIFY(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
		PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
							PDWORD lpcbBytesReturned);
	CString Byte8To32Str(BYTE *psrcbyte);

protected:
	
private:
	SOCKET	m_sockt;
	char	*m_strurl;
};

#endif


