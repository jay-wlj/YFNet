#include "StdAfx.h"
#include "NetUrl.h"
#include "qvod_sock.h"
#include "AutoPtr.h"
#include "qvod_error.h"
#include "HttpSock.h"
#include "HttpSSLSock.h"
#include "common-commonfun.h"
#include "common-DNSCache.h"

NetUrl::NetUrl(std::string &url, int nTimeout):
m_url(url),
m_nTimeout(nTimeout)
{
}


NetUrl::~NetUrl(void)
{
}

void NetUrl::SetHeaderString(const std::string& strHeader)
{
	m_strHeader = strHeader;
}

std::string NetUrl::GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams)										// 获得URL的host和请求子页面
{
	std::string strRet;
	std::string str = strURL;

	if (0 == QvodStrInCmp(strURL.c_str(), "http://", strlen("http://")))
	{
		str = strURL.substr(strlen("http://"));
	}
	else if (0 == QvodStrInCmp(strURL.c_str(), "https://", strlen("https://")))
	{
		str = strURL.substr(strlen("https://"));
	}

	int nSubURL = str.find("/");
	if(nSubURL < 0)
	{
		strHost = str;
		strRet = "/";
	}
	else
	{
		strHost = str.substr(0, nSubURL);
		strRet = str.substr(nSubURL);
	}
	int nParamsPos = strRet.find("?");
	if (nParamsPos != std::string::npos)
	{
		strParams = strRet.substr(nParamsPos+1);
		strRet = strRet.substr(0, nParamsPos);
	}
	return strRet;
}

bool NetUrl::GetRequest(std::string & strData)		// 请求并获取内容
{
	bool ret = false;
	return ret;
}

qvod_sock_t NetUrl::CreateSocket(bool isvip6)
{
	if (isvip6)
	{
		return socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	}
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

bool NetUrl::PostRequest(const std::string& strPost, std::string& strData)	// POST请求并获取内容
{
	bool ret = false;

	string strHost;
	string strParams;
	string strURL = GetURLHost(m_url, strHost, strParams);

	string strcdnhost = strHost;
	int nPos = strHost.find(":");
	if (nPos != string::npos)
	{
		unsigned short port = atoi(strHost.c_str()+nPos+1);
		strcdnhost = strHost.substr(0, nPos);
	}
	//int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	const IPAddrInfo ipaddr = GetIpByHost(strcdnhost.c_str());
	qvod_sock_t sock = CreateSocket(ipaddr.isIpv6);
	if (QVOD_INVALID_SOCKET == sock)
	{
		return ret;
	}
	int iTimeout = 2000;
	QvodSetSockTimeout(sock, SOL_SOCKET, SO_RCVTIMEO, m_nTimeout);
	QvodSetSockTimeout(sock, SOL_SOCKET, SO_SNDTIMEO, m_nTimeout);

	bool bHttps = m_url.find("https://") != string::npos;
	unsigned short port = 80;
	if (strHost.find(":") != string::npos)
	{
		port = atoi(strHost.substr(strHost.find(":")+1).c_str());
	}
	else
	{
		port = bHttps? 443: port;
	}
	QPtr::AutoPtr<CHttpSock>  pSock = bHttps? new CHttpSSLSock(sock): new CHttpSock(sock);

	//qvod_sockaddr_t addr = {0};
	//addr.sin_addr.s_addr = GetHostByName(strHost.c_str());
	//addr.sin_port = htons(port);
	//addr.sin_family = AF_INET;
	if (! pSock->Connect(ipaddr.ipv4, ipaddr.ipv6, htons(port), ipaddr.isIpv6))
	{
		int err =  QvodGetLastError();
		QvodCloseSocket(sock);
		return ret;
	}

	char buf[102400] = {0};
	sprintf_s(buf,sizeof(buf),"POST %s HTTP/1.1\r\nAccept: */*\r\nConnection: Keep-Alive\r\nContent-Type: application/x-www-form-urlencoded\r\nHost: %s\r\nContent-Length: %d\r\n%s\r\n%s",strURL.c_str(),strHost.c_str(), strPost.length(), m_strHeader.c_str(), strPost.c_str());

	if (! OnSend(pSock, buf, strlen(buf)))
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "NetUrl OnSend failed err=%d", QvodGetLastError());
#endif
	}
	if (! OnRecv(pSock, strData))
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "NetUrl OnRecv failed err=%d", QvodGetLastError());
#endif
	}
	pSock->Close();

	return true;
}

bool NetUrl::OnSend(CHttpSock* pSock,  const char *buf, const int& len)
{
	bool ret = false;
	if (NULL == pSock)
	{
		return ret;
	}
	int datalen = pSock->SendData(buf, len);
	if (datalen <= 0)
	{
		return ret;
	}

	ret = datalen >= len;
	return ret;
}

bool NetUrl::OnRecv(CHttpSock* pSock, std::string& strData)		// sock recv
{
	bool ret = false;
	if (NULL == pSock)
	{
		return ret;
	}
	const int nBufLen = 102400;
	char buf[nBufLen] = {0};
	int datalen = pSock->RecvData(buf, nBufLen);
	if (QVOD_SOCKET_ERROR == datalen)
	{
		int err =  QvodGetLastError();
		return ret;
	}
	char* dataoffset = NULL;
	char *pbuf = NULL;
	int recvlen = 0;
	int filelen =  0;
	dataoffset = strstr(buf,"\r\n\r\n");
	if(!dataoffset)
	{
		return false;
	}
	*dataoffset = 0;
	dataoffset += 4;
	if(!strstr(buf,"200 OK"))
	{
		return false;
	}
	pbuf = strstr(buf,"Content-Length: ");
	if(!pbuf)
	{
		bool bChunked = (NULL != strstr(buf, "Transfer-Encoding: chunked"));
		bChunked |= (NULL != strstr(buf, "transfer-encoding: chunked"));

		if (bChunked)		// chunk分块传输
		{
			std::string strInfo;

			recvlen = datalen - (dataoffset - buf);
			strInfo.append(dataoffset, recvlen);

			while (! strstr(dataoffset, "0\r\n\r\n"))
			{
				datalen = pSock->RecvData(buf, nBufLen);
				if(datalen <= 0)
				{
	#ifdef QVODDEBUG
					_OUTPUT(FAIL, "NetUrl recv datalen %d fail: %d\n", datalen, errno);
	#endif // QVODDEBUG
					return false;
				}

				strInfo.append(buf, datalen);
				dataoffset = buf;
			}

			dataoffset = (char*)strInfo.c_str();
			const char *dataEnd = strInfo.c_str()+strInfo.size();

			int nPos = 0;
			do
			{
				int len = strtol(dataoffset, NULL, 16);
				if (0 == len)
				{
					break;
				}
				dataoffset = strstr(dataoffset, "\r\n");
				if (dataoffset)
				{
					dataoffset += 2;

					strData.append(dataoffset, len);
					dataoffset += len;

					dataoffset = strstr(dataoffset, "\r\n");
					if (dataoffset)
					{
						dataoffset += 2;
					}
				}
			} while (dataoffset < dataEnd);
		}
		else
		{
			recvlen = datalen - (dataoffset - buf);
			strData.append(dataoffset, recvlen);
			datalen =  pSock->RecvData(buf,nBufLen);
			while(datalen > 0)
			{
				strData.append(buf, datalen);
				datalen = pSock->RecvData(buf, nBufLen);
			}
		}
	}
	else
	{
		pbuf += strlen("Content-Length: ");
		filelen = atoi(pbuf);
		recvlen = datalen - (dataoffset - buf);
		recvlen = recvlen > filelen? filelen: recvlen;
		strData.append(dataoffset, recvlen);
		while(recvlen < filelen)
		{
			datalen = pSock->RecvData(buf,nBufLen);
			if(datalen <= 0)
			{
#ifdef QVODDEBUG
				_OUTPUT(FAIL, "GetHttpUrl recv datalen %d fail: %d\n", datalen, errno);
#endif // QVODDEBUG
				return false;
			}
			strData.append(buf, datalen);
			recvlen += datalen;
		}
	}

	return true;
}