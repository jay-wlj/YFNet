#pragma once
#include "HttpSock.h"
#include <string>
using std::string;

class NetUrl
{
public:
	NetUrl(std::string &url, int nTimeout=5000);
	~NetUrl(void);
	
public:	
	void SetHeaderString(const std::string& strHeader);
	bool GetRequest(std::string & strData);		// ���󲢻�ȡ����
	bool PostRequest(const std::string& strPost, std::string& strData);	// POST���󲢻�ȡ����

	static std::string GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams);										// ���URL��host��������ҳ��
	qvod_sock_t CreateSocket(bool isvip6);
private:
	
	bool OnSend(CHttpSock* pSock, const char *buf, const int& len);
	bool OnRecv(CHttpSock* pSock, std::string& strData);		// sock recv
private:
	std::string m_url;
	std::string m_strHeader;
	int m_nTimeout;
};

