#include "global.h"
#ifdef SUPPORT_HTTPS
#include "HttpSSLSock.h"
#include "common-commonfun.h"
#include "qvod_time.h"
#include "qvod_error.h"

SSL_CTX* CHttpSSLSock::s_sslCtx = NULL;
const SSL_METHOD* CHttpSSLSock::s_sslMethod = NULL;

bool CHttpSSLSock::InitSSLContext()
{
	bool bRet = false;
	if (s_sslCtx && s_sslMethod)
	{
		return bRet;
	}
	
#ifdef QVODDEBUG
	QVOD_DWORD dwTick = QvodGetTime();
#endif
	 /* SSL 库初始化 */  
	//SSL_library_init();  
	//OpenSSL_add_all_algorithms();  
	//SSL_load_error_strings();  

	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();
	s_sslMethod = SSLv23_client_method();

	if (s_sslMethod)
	{
		s_sslCtx = SSL_CTX_new(s_sslMethod);
		if (s_sslCtx)
		{
			bRet = true;
		}
		else
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpSSLSock::InitSlsContext() SSL_CTX_new failed! err=%d", errno);
#endif
		}
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CHttpSSLSock::InitSSLContext() ret=%d tick=%d", bRet, QvodGetTime()-dwTick);

#endif
	return bRet;
}

void CHttpSSLSock::UnInitSSLContext()
{
	if (NULL != s_sslCtx)
	{
		SSL_CTX_free(s_sslCtx);
		s_sslCtx = NULL;
	}
	s_sslMethod = NULL;
}

CHttpSSLSock::CHttpSSLSock(void):m_ssl(NULL)
{
}
CHttpSSLSock::CHttpSSLSock(qvod_sock_t sock):
CHttpSock(sock),
m_ssl(NULL)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CHttpSSLSock sock=%d", sock);
#endif
}

CHttpSSLSock::~CHttpSSLSock(void)
{
	//!清理打开的句柄
	if (NULL != m_ssl)
	{
		SSL_shutdown(m_ssl);
		Close();
		SSL_free(m_ssl);
	}
	m_ssl = NULL;
}



CHttpSSLSock& CHttpSSLSock::operator=(const CHttpSSLSock& rhs)
{
	if (this == &rhs)
	{
		return *this;
	}
	if (m_ssl)
	{
		if (NULL != m_ssl)
		{
			SSL_shutdown(m_ssl);
			Close();
			SSL_free(m_ssl);
		}	
		m_ssl = NULL;
	}
	CHttpSock::operator=(rhs);
	//m_socket = rhs.m_socket;
	m_ssl = rhs.m_ssl;
	
	return *this;
}

bool CHttpSSLSock::InitializeSslContext()                           //!初始化SSL环境
{
	//!SSL通信初始化
	bool bRet = false;

	do 
	{
		if (NULL == s_sslCtx)			// SSL还没初始化完
		{
			break;
		}

		//// 要求校验对方证书  
		//SSL_CTX_set_verify(sslCtx, SSL_VERIFY_NONE, NULL);  

		//// 加载CA的证书  
		//if(!SSL_CTX_load_verify_locations(sslCtx, CA_CERT_FILE, NULL))
		//{
		//	printf("SSL_CTX_load_verify_locations error!\n");
		//	return NULL;
		//}

		//// 加载自己的证书  
		//if(SSL_CTX_use_certificate_file(sslCtx, SERVER_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
		//{
		//	printf("SSL_CTX_use_certificate_file error!\n");
		//	return NULL;
		//}

		//// 加载自己的私钥  
		//if(SSL_CTX_use_PrivateKey_file(sslCtx, SERVER_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
		//{
		//	printf("SSL_CTX_use_PrivateKey_file error!\n");
		//	return NULL;
		//}

		//// 判定私钥是否正确  
		//if(!SSL_CTX_check_private_key(sslCtx))
		//{
		//	printf("SSL_CTX_check_private_key error!\n");
		//	return NULL;
		//}

		m_ssl = SSL_new(s_sslCtx);		 // 基于 ctx 产生一个新的 SSL
		if (NULL == m_ssl)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpSSLSock::InitializeSslContext() SSL_new failed! err=%d", errno);
#endif
			break;
		}

		SSL_set_fd(m_ssl, m_socket);		/* attach the socket descriptor */
		SSL_set_connect_state(m_ssl);

		bRet = true;
	} while (false);

	return bRet;
}
bool CHttpSSLSock::SslConnect()                                         //!SSL连接
{
	//!SSL绑定原生socket,并连接服务器
	bool bRet = false;

	do 
	{
		int ssl_conn_ret = SSL_connect(m_ssl);
		if (1 == ssl_conn_ret) 
		{
			// 开始和对端交互
			bRet = true;
		}
		else if (-1 == ssl_conn_ret)
		{
			int ssl_conn_err = SSL_get_error(m_ssl, ssl_conn_ret);
			int i = 0;
			if (SSL_ERROR_WANT_READ == ssl_conn_err || SSL_ERROR_WANT_WRITE == ssl_conn_err) 
			{

				int j =0;
				//需要更多时间来进行握手
			}
			else
			{
				
			}
			int ret = ERR_get_error();
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpSSLSock::SslConnect() Error code is %d，Error messages are '%s'\n", errno, strerror(errno));
#endif
		}
		else
		{
			// 连接失败了，做必要处理
			//if (0 != ssl_conn_ret)
			//{
			//	SSL_shutdown(m_ssl);
			//}
			//SSL_free(m_ssl);
		}

	} while (false);

	return bRet;
}
bool CHttpSSLSock::SslGetCipherAndCertification()		//获取密码和服务器端证
{
	bool bRet = false;

	//do 
	//{
	//	cstrSslCipher = SSL_get_cipher(ssl);
	//	serverCertification = SSL_get_certificate(ssl);

	//	if (NULL == serverCertification)
	//	{
	//		break;
	//	}

	//	cstrSslSubject = X509_NAME_oneline(X509_get_subject_name(serverCertification), 0, 0);
	//	cstrSslIssuer = X509_NAME_oneline(X509_get_issuer_name(serverCertification), 0, 0);

	//	X509_free(serverCertification);

	//	bRet = TRUE;
	//} while (FALSE);

	return bRet;
}

int CHttpSSLSock::Connect(const qvod_sockaddr_t& SvrAddr)
{
	bool bRet = false;

	do 
	{
		if (QVOD_SOCKET_ERROR == CHttpSock::Connect(SvrAddr))
		{
			int netErr = QvodGetLastError();
			if (netErr != QVOD_NET_EWOULDBLOCK && netErr != EINPROGRESS)
			{
#ifdef QVODDEBUG
				_OUTPUT(ERROR1, "CHttpSSLSock CHttpSock::Connect err sock=%d error=%d", m_socket, QvodGetLastError());
#endif
				break;
			}
		}

		bRet = SSLConnect();
	} while (false);
	return bRet;
}

int CHttpSSLSock::Connect(const unsigned long ipv4, const in6_addr& ipv6, int nPort, bool isipv6)
{
	if (QVOD_SOCKET_ERROR == CHttpSock::Connect(ipv4, ipv6, nPort, isipv6))
	{
		int netErr = QvodGetLastError();
		if (netErr != QVOD_NET_EWOULDBLOCK && netErr != EINPROGRESS)
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CHttpSSLSock CHttpSock::Connect err sock=%d error=%d", m_socket, QvodGetLastError());
#endif
			return 0;
		}
	}
	return SSLConnect();
}


int CHttpSSLSock::Connect(const struct sockaddr_in6& svrAddr)
{
	if (QVOD_SOCKET_ERROR == CHttpSock::Connect(svrAddr))
	{
		int netErr = QvodGetLastError();
		if (netErr != QVOD_NET_EWOULDBLOCK && netErr != EINPROGRESS)
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CHttpSSLSock CHttpSock::Connect err sock=%d error=%d", m_socket, QvodGetLastError());
#endif
			return 0;
		}
	}
	return SSLConnect();	
}

bool CHttpSSLSock::SSLConnect()
{
	bool bRet = false;

	do 
	{
		if (!InitializeSslContext())
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CHttpSSLSock InitializeSslContext err sock=%d error=%d", m_socket, QvodGetLastError());
#endif
			break;
		}

		if (!SslConnect())
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CHttpSSLSock SslConnect err sock=%d error=%d", m_socket, QvodGetLastError());
#endif
			break;
		}

		bRet = true;
	} while (false);
	return bRet;
}

int CHttpSSLSock::SendData(const char* pBuf, const unsigned int& len)
{
	int ret = 0;

	if (NULL == pBuf || len <= 0)
	{
		return ret;
	}

	int nSendLen = 0;
	int ires = 0;

	QVOD_DWORD dwTick = QvodGetTime();

	if (NULL == m_ssl)
	{
		InitializeSslContext();
		SslConnect();
	}
#ifdef WIN32
	
	while (true)
	{
		if (QvodGetTime() - dwTick > 10000)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpSSLSock::SendData failure! timeout\n");
#endif
			break;
		}
		int iRes = SSL_do_handshake(m_ssl);
		if (1 ==iRes)
		{
			break;
		}
		else
		{
			int ssl_handshake_err = SSL_get_error(m_ssl, ret);
			if (SSL_ERROR_WANT_READ == ssl_handshake_err ||  SSL_ERROR_WANT_WRITE == ssl_handshake_err)
			{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpSSLSock::SendData failure! Error code is %d，Error messages are '%s'\n", errno, strerror(errno));
#endif
				break;
			}
		}
		QVOD_SLEEP(1);
	}
#else
	int iRes = SSL_do_handshake(m_ssl);	
	if (1 != iRes)		// =1 等待握手完成
	{
//#ifdef QVODDEBUG
//		_OUTPUT(INFO, "CHttpSSLSock SSL_do_handshake ing...");
//#endif
		return -1;
	}
#endif
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CHttpSSLSock SSL_do_handshake ok, tick=%d", QvodGetTime()-dwTick);
#endif

	do 
	{
		int datalen = SSL_write(m_ssl, (void*)pBuf, len);
		if (-1 == datalen)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CHttpSSLSock::SendData failure! Error code is %d，Error messages are '%s'\n", errno, strerror(errno));
#endif
			//int ssl_recv_err = SSL_get_error(m_ssl, ret);
			//if (SSL_ERROR_WANT_WRITE == ssl_recv_err) 
			//{
			//	break;
			//}
			break;
		}
		nSendLen += datalen;
	} while (nSendLen < len);

	return nSendLen;
}

int CHttpSSLSock::RecvData(char* pBuf, unsigned int len)
{
	if (NULL == pBuf || 0 == len)
	{
		return 0;
	}
	int ret = 0;
	do 
	{
		ret = SSL_read(m_ssl, (void*)pBuf, len);
		if(ret == 0)	// complete
		{
			break;
		}
		else if (-1 == ret)
		{
			//int ssl_recv_err = SSL_get_error(m_ssl, ret);
			//if (SSL_ERROR_WANT_READ == ssl_recv_err) 
			//{
			//	break;
			//}

#ifdef QVODDEBUG
			//_OUTPUT(INFO, "CHttpSSLSock::RecvData Failure to receive message ! Error code is %d，sslerr=%d Error messages are '%s'\n", errno, ssl_recv_err, strerror(errno));  
#endif		
			break;
		}
	} while (false);

#ifdef QVODDEBUG
		_OUTPUT(INFO, "CHttpSSLSock::RecvDataSSL_read	ret=%d\n", ret);  
#endif
	return ret;
}

#endif
