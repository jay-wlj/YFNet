#include "global.h"
#ifdef SUPPORT_HTTPS					// 是否支持openssl
#include "httpsock.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

class CHttpSSLSock :
	public CHttpSock
{
public:
	CHttpSSLSock(void);
	CHttpSSLSock(qvod_sock_t sock);
	virtual ~CHttpSSLSock(void);

public:
	static bool InitSSLContext();
	static void UnInitSSLContext();
public:
	virtual int Connect(const unsigned long ipv4, const struct in6_addr& ipv6, int nPort, bool isipv6);
	virtual int Connect(const struct sockaddr_in6& svrAddr);
	virtual int Connect(const qvod_sockaddr_t& SvrAddr);
	virtual int SendData(const char* pBuf, const unsigned int& len);
	virtual int RecvData(char* pBuf, unsigned int len);
private:
	bool SSLConnect();
protected:
	bool InitializeSslContext();                           //!初始化SSL环境
	bool SslConnect();                                         //!SSL连接
	bool SslGetCipherAndCertification();		//获取密码和服务器端证
public:
	CHttpSSLSock& operator=(const CHttpSSLSock& rhs);

private:
	static SSL_CTX *s_sslCtx;
	static const SSL_METHOD *s_sslMethod;

protected:
	SSL *m_ssl;
};
#endif
