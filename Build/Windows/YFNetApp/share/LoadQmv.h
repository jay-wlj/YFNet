#pragma once

#include <vector>
#include <string>
using namespace std;
#include "..\dblite\dblite.h"
#include "..\share\QmvTool.h"

const TCHAR * const QMV_DB = _T("qvt.db");
const TCHAR * const QMV_DLL = _T("dblite.dll");
const TCHAR * const QMV_TABLE_Rec = _T("qvtRecord");
const TCHAR * const QMV_TABLE_FL = _T("qvtFilmList");
const char business_tag_count = 6;
const char knowledge_tag_count = 4;

class CMakeJesonString
{
public:
	CMakeJesonString(LPCTSTR szTagName) : m_nPos(-1), m_bCurItemEmpty(true)
	{
		m_str.Empty();
		if ( !szTagName || !szTagName[0] )
		{
			return;
		}
		m_str.Format(_T("{%s:[]}"), szTagName);
	}
	const CString & GetString(void) const
	{
		return m_str;
	}

	void NewItem(void)
	{
		if ( -1 != m_nPos )
		{
			m_nPos = m_str.GetLength() - 2;
			m_str.Insert(m_nPos, _T(','));
		}
		m_nPos = m_str.GetLength() - 2;
		m_str.Insert(m_nPos, _T("{}"));
		m_bCurItemEmpty = true;
	}

	void AddPair(LPCTSTR szKey, LPCTSTR szValue)
	{
		if ( !szKey || !szKey[0] || !szValue )
		{
			return;
		}
		if ( !m_bCurItemEmpty )
		{
			m_nPos = m_str.GetLength() - 3;
			m_str.Insert(m_nPos, _T(','));
		}
		CString pair;
		m_nPos = m_str.GetLength() - 3;
		pair.Format(_T("%s:%s"), szKey, szValue);
		m_str.Insert(m_nPos, pair);
		m_bCurItemEmpty = false;
	}
private:
	CMakeJesonString(void) : m_nPos(-1), m_bCurItemEmpty(true){}
private:
	int m_nPos;
	CString m_str;
	bool m_bCurItemEmpty;
};

#include <map>
#include <time.h>
#include <string>
#include <vector>
#include <algorithm>
#include <winsock2.h>

namespace QmvBrImgSvr
{
#define THREADKILL_TO		1000
#define THREADWAIT_TO		300
#define TICK				100
#define THREADEXIT_SUCCESS	0x1234
#define MAX_BUFFER			10240
#define DEF_PORT			49037
#define OpenDB(dllpath, dbpath, hdb, readonly) \
	if ( dblite::pfOpenDataBase pf = GetDllProc<dblite::pfOpenDataBase>( CT2A(dllpath), "OpenDataBase", false ) ) \
	{ \
		hdb = pf( dbpath, readonly ); \
	}

#define ExeDB(dllpath, hdb, query, ret) \
	if ( dblite::pfExecute pf = GetDllProc<dblite::pfExecute>( CT2A(dllpath), "Execute", false ) ) \
	{ \
		ret = pf( hdb, CT2W(query), 0, 0, 0 ); \
	}

#define ShutDB(dllpath, hdb) \
	if ( dblite::pfCloseDataBase pf = GetDllProc<dblite::pfCloseDataBase>( CT2A(dllpath), "CloseDataBase", false ) ) \
	{ \
		pf( hdb ); \
	}

	using namespace std;
	struct NewConnectionTag;

	typedef map<DWORD, HANDLE>				IDHANDLE;
	typedef vector<HANDLE>					HANDLES;
	typedef vector<string>					STRVECT;

	template<class T>
	class CThisFunToStaticFun
	{
	public:
		// 联合类，用于转换类成员方法指针到普通函数指针（试过编译器不允许在这两种函数之间强制转换），不知道有没有更好的方法。
		union
		{
			DWORD (WINAPI *ThreadProc)(void *);
			DWORD (WINAPI T::*MemberProc)(void);
		}Proc;
	};

	class CGenericServer;
	struct NewConnectionTag
	{
		CGenericServer			*pThis;
		SOCKET					s;
	};

	class CGenericServer : public CThisFunToStaticFun<CGenericServer>
	{
	public:
		CGenericServer(void) : m_bRun(false){}
		virtual ~CGenericServer(void)
		{
			Shutdown();
		}
	public:

		// 查询端口
		int GetPort(void)
		{
			return m_ServerPort;
		}

		//启动服务器
		bool Run(int Port, int PersTO)
		{
			if ( m_bRun ) return false;
			m_ServerPort = Port;
			m_PersistenceTO = PersTO;

			InitializeCriticalSection(&m_cs);
			InitializeCriticalSection(&m_cs_);
			m_ThreadLaunchedEvent = CreateEvent(0, FALSE, TRUE, _T("QmvImgSvr"));

			// 启动接收线程
			ResetEvent(m_ThreadLaunchedEvent);
			Proc.MemberProc = &CGenericServer::AcceptThread;
			m_ThreadA = CreateThread(0, 0, Proc.ThreadProc, this, 0, &m_ThreadA_ID);
			if ( !m_ThreadA )
			{
				return false;
			}

			if ( WaitForSingleObject(m_ThreadLaunchedEvent, THREADWAIT_TO) != WAIT_OBJECT_0 )
			{
				CloseHandle(m_ThreadLaunchedEvent);
				return false;
			}

			// 启动帮助线程
			ResetEvent(m_ThreadLaunchedEvent);
			Proc.MemberProc = &CGenericServer::HelperThread;
			m_ThreadC = CreateThread(0, 0, Proc.ThreadProc, this, 0, &m_ThreadC_ID);
			if ( !m_ThreadC )
			{
				return false;
			}

			if ( WaitForSingleObject(m_ThreadLaunchedEvent, THREADWAIT_TO) != WAIT_OBJECT_0 )
			{
				CloseHandle(m_ThreadLaunchedEvent);
				return false;
			}

			CloseHandle(m_ThreadLaunchedEvent);
			m_bRun = true;
			return true;
		}

		//关闭服务
		bool Shutdown(void)
		{
			if ( !m_bRun ) return false;
			bool bResult = false;
			HANDLE	hArray[2] = {m_ThreadA, m_ThreadC};

			// 关闭接收和helper线程
			SetEvent(m_ShutdownEvent);
			DWORD n = WaitForMultipleObjects(2, hArray, TRUE, THREADKILL_TO);

			if ( WAIT_TIMEOUT == n || WAIT_FAILED == n )
			{
				bResult = false;
			}

			CloseHandle(m_ThreadA);
			CloseHandle(m_ThreadC);

			// 所有的客户线程都结束
			IDHANDLE::iterator it;
			while ( m_ThreadList.size() )
			{
				Sleep(100);
			}
			DeleteCriticalSection(&m_cs);
			DeleteCriticalSection(&m_cs_);
			m_bRun = false;
			return bResult;
		}

	protected:
		virtual bool IsComplete(string) = 0;
		virtual bool ParseRequest(string, string&, bool&) = 0;
	private:
		// 设置可用端口
		int SetPort(SOCKET s, sockaddr_in & addr)
		{
			int e = 0;
			addr.sin_port = htons(m_ServerPort);
			while ( ( e = bind( s, (struct sockaddr *)&addr, sizeof(addr) ) ) && WSAEADDRINUSE == WSAGetLastError() )
			{
				addr.sin_port = htons(++m_ServerPort);
				if ( m_ServerPort - DEF_PORT > 10 )
				{
					break;
				}
			}
			return e;
		}

		// 服务器线程
		DWORD WINAPI AcceptThread(void)
		{
			SOCKET s;
			WORD wVersionRequested;
			WSADATA wsaData;
			sockaddr_in saLocal;
			WSAEVENT Handles[2];
			WSANETWORKEVENTS NetworkEvents;
			sockaddr ClientAddr;
			INT addrlen = sizeof(ClientAddr);
			sockaddr_in sain;
			char cAddr[50];
			int result = 0;

			saLocal.sin_family		= AF_INET;
			saLocal.sin_addr.s_addr	= INADDR_ANY;
			wVersionRequested		= MAKEWORD(2, 2);
			if ( result = WSAStartup(wVersionRequested, &wsaData)
				|| LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2
				|| INVALID_SOCKET == ( s = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, WSA_FLAG_OVERLAPPED) ) )
			{
				CleanupThread(0, 0, 0);
				return THREADEXIT_SUCCESS;
			}

			// 绑定并监听
			if ( SetPort( s, saLocal ) || listen(s, SOMAXCONN) )
			{
				CleanupThread(0, 0, s);
				return THREADEXIT_SUCCESS;
			}

			m_ShutdownEvent = WSACreateEvent();
			if ( WSA_INVALID_EVENT == m_ShutdownEvent )
			{
				CleanupThread(0, 0, 0, s);
				return THREADEXIT_SUCCESS;
			}

			WSAEVENT Event = WSACreateEvent();
			if ( WSA_INVALID_EVENT == Event )
			{
				CleanupThread(0, m_ShutdownEvent, s);
				return THREADEXIT_SUCCESS;
			}

			Handles[0] = m_ShutdownEvent;
			Handles[1] = Event;

			result = WSAEventSelect(s, Event, FD_ACCEPT);
			if ( result == SOCKET_ERROR )
			{
				CleanupThread(Event, m_ShutdownEvent, s);
				return THREADEXIT_SUCCESS;
			}

			SetEvent(m_ThreadLaunchedEvent);

			while ( 1 )
			{
				DWORD EventCaused = WSAWaitForMultipleEvents(2, Handles, FALSE, WSA_INFINITE, FALSE);
				if ( WAIT_FAILED == EventCaused || WAIT_OBJECT_0 == EventCaused )
				{
					CleanupThread(Event, m_ShutdownEvent, s);
					return THREADEXIT_SUCCESS;
				}

				result = WSAEnumNetworkEvents(s, Event, &NetworkEvents);
				if ( SOCKET_ERROR == result )
				{
					CleanupThread(Event, m_ShutdownEvent, s);
					return THREADEXIT_SUCCESS;
				}

				if ( FD_ACCEPT == NetworkEvents.lNetworkEvents )
				{
					SOCKET ClientSocket = WSAAccept( s, &ClientAddr, &addrlen, 0, 0 );
					memcpy( &sain, &ClientAddr, addrlen );
					sprintf( cAddr, "%d.%d.%d.%d", sain.sin_addr.S_un.S_un_b.s_b1, sain.sin_addr.S_un.S_un_b.s_b2,
						sain.sin_addr.S_un.S_un_b.s_b3, sain.sin_addr.S_un.S_un_b.s_b4 );

					if ( INVALID_SOCKET == ClientSocket )
					{
						// 有一个文件错误
						continue;
					}
					else
					{
						if ( !AddClient(ClientSocket, cAddr, sain.sin_port) )
						{
							// 单个连接失败，暂时不关闭服务器
							continue;
						}
					}
				}
			}

			CleanupThread(Event, m_ShutdownEvent, s);
			return THREADEXIT_SUCCESS; 
		}

		//客户端线程
		static DWORD WINAPI ClientThread(LPVOID lpv)
		{
			NewConnectionTag *pNewConn = (NewConnectionTag*)lpv;
			CGenericServer *pThis = pNewConn->pThis;
			SOCKET s = pNewConn->s;

			int					result;
			WSAEVENT			EventArray[2];
			WSANETWORKEVENTS	NetworkEvents;
			bool				bResend = false;
			WSABUF				Buffer;
			DWORD				NumberOfBytesSent;
			DWORD				dwBytesSent;
			bool				bKeepAlive = false;
			string				szRequest;
			string				szResponse;
			WSAEVENT			Event = WSACreateEvent();

			if ( WSA_INVALID_EVENT == Event )
			{
				pThis->CleanupThread(0, s, pNewConn, GetCurrentThreadId());
				return THREADEXIT_SUCCESS;
			}

			result = WSAEventSelect(s, Event, FD_READ | FD_WRITE | FD_CLOSE);
			if(result == SOCKET_ERROR)
			{
				pThis->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
				return THREADEXIT_SUCCESS;
			}

			EventArray[0] = Event;
			EventArray[1] = pThis->m_ShutdownEvent;

			while ( 1 )
			{
				// 检查是否服务器任务已经完成
				DWORD EventCaused = WSAWaitForMultipleEvents( 2, EventArray, 0, pThis->m_PersistenceTO ? pThis->m_PersistenceTO : WSA_INFINITE, FALSE );
				if ( WSA_WAIT_FAILED == EventCaused || 1 == EventCaused || WSA_WAIT_TIMEOUT == EventCaused )
				{
					pThis->CleanupThread( Event, s, pNewConn, GetCurrentThreadId() );
					return THREADEXIT_SUCCESS;
				}

				//	分析什么网络事件产生
				if ( SOCKET_ERROR == ( result = WSAEnumNetworkEvents( s, Event, &NetworkEvents ) )
					|| !NetworkEvents.lNetworkEvents )
				{
					continue;
				}

				// 处理事件
				if ( NetworkEvents.lNetworkEvents & FD_READ )
				{
					//	不需要接收接传入的数据，只需要传给继承类
					DWORD NumberOfBytesRecvd;
					WSABUF Buffers;
					DWORD dwBufferCount = 1;
					char szBuffer[MAX_BUFFER];
					DWORD Flags = 0;
					Buffers.buf = szBuffer;
					Buffers.len = MAX_BUFFER;

					if ( SOCKET_ERROR != ( result = WSARecv(s, &Buffers, dwBufferCount, &NumberOfBytesRecvd, &Flags, 0, 0) ) )
					{
						// 检测是否获得完整的请求
						szRequest += string(szBuffer, NumberOfBytesRecvd);
						if ( !pThis->IsComplete(szRequest) )
							continue;

						if ( !pThis->ParseRequest(szRequest, szResponse, bKeepAlive) )
						{
							pThis->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
							return THREADEXIT_SUCCESS;
						}

						// 发送响应倒客户端
						NumberOfBytesSent = 0;
						dwBytesSent = 0;
						do
						{
							Buffer.len = (szResponse.size() - dwBytesSent) >= MAX_BUFFER ? MAX_BUFFER : szResponse.size() - dwBytesSent;	
							Buffer.buf = (char*)((DWORD)szResponse.c_str() + dwBytesSent);

							result = WSASend(s, &Buffer, 1, &NumberOfBytesSent, 0, 0, 0);
							if ( SOCKET_ERROR != result )
								dwBytesSent += NumberOfBytesSent;
						}
						while ( ( dwBytesSent < szResponse.size() ) && SOCKET_ERROR != result );

						if ( WSAEWOULDBLOCK == WSAGetLastError() )
						{
							bResend = true;
							continue;
						}

						if ( SOCKET_ERROR == result )
						{
							bKeepAlive = false;
						}

						if ( !bKeepAlive )
						{
							pThis->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
							return THREADEXIT_SUCCESS;
						}

						// 完成请求，清除缓冲区
						szRequest.erase(0, string::npos);
					}
				}

				if ( ( NetworkEvents.lNetworkEvents & FD_WRITE ) && bResend )
				{
					// 发送响应倒客户端
					do
					{
						Buffer.len = (szResponse.size() - dwBytesSent) >= MAX_BUFFER ? MAX_BUFFER : szResponse.size() - dwBytesSent;	
						Buffer.buf = (char*)((DWORD)szResponse.c_str() + dwBytesSent);

						result = WSASend(s, &Buffer, 1, &NumberOfBytesSent, 0, 0, 0);
						if ( SOCKET_ERROR != result )
							dwBytesSent += NumberOfBytesSent;
					}
					while ( ( dwBytesSent < szResponse.size() ) && SOCKET_ERROR != result );

					if ( WSAGetLastError() == WSAEWOULDBLOCK )
					{
						bResend = TRUE;
						continue;
					}

					if ( SOCKET_ERROR == result )
					{
						bKeepAlive = false;
					}
					if ( !bKeepAlive )
					{
						pThis->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
						return THREADEXIT_SUCCESS;
					}
					bResend = false;

					// 清除缓冲区
					szRequest.erase(0, string::npos);
				}
				if ( NetworkEvents.lNetworkEvents & FD_CLOSE )
				{
					pThis->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
					return THREADEXIT_SUCCESS;
				}
			}
			return THREADEXIT_SUCCESS;
		}

		DWORD WINAPI HelperThread(void)
		{
			SetEvent(m_ThreadLaunchedEvent);
			while ( 1 )
			{
				if ( WaitForSingleObject( m_ShutdownEvent, TICK ) != WAIT_TIMEOUT )
				{
					return THREADEXIT_SUCCESS;
				}
				EnterCriticalSection(&m_cs_);
				while ( m_HandleList.size() )
				{
					HANDLE h = *m_HandleList.begin();
					if ( WAIT_TIMEOUT == WaitForSingleObject(h, THREADKILL_TO) )
					{
						TerminateThread(h, THREADEXIT_SUCCESS);
					}
					CloseHandle(h);
					m_HandleList.erase(m_HandleList.begin());
				}
				LeaveCriticalSection(&m_cs_);
			}
			return THREADEXIT_SUCCESS;
		}

		//增加新的连接
		bool AddClient(SOCKET s, char* ClientAddress, int port)
		{
			if ( find(m_Visitors.begin(), m_Visitors.end(), ClientAddress) == m_Visitors.end() )
				m_Visitors.push_back(ClientAddress);

			DWORD threadID = 0;
			EnterCriticalSection(&m_cs);
			NewConnectionTag *NewConn	= new NewConnectionTag;
			NewConn->pThis				= this;
			NewConn->s					= s;
			if ( HANDLE hThread = CreateThread( 0, 0, ClientThread, NewConn, 0, &threadID ) )
			{
				m_ThreadList[threadID] = hThread;
			}
			LeaveCriticalSection(&m_cs);
			return true;
		}

		//清除线程
		void CleanupThread(WSAEVENT Event, SOCKET s, NewConnectionTag* pNewConn, DWORD dwThreadID)
		{
			if ( Event ) WSACloseEvent(Event);
			closesocket(s);
			EnterCriticalSection(&m_cs);
			delete pNewConn;
			LeaveCriticalSection(&m_cs);

			IDHANDLE::iterator it = m_ThreadList.find(dwThreadID);
			if ( it != m_ThreadList.end() )
			{
				EnterCriticalSection(&m_cs_);
				m_HandleList.push_back(it->second);
				m_ThreadList.erase(it);
				LeaveCriticalSection(&m_cs_);
			}
		}

		//清除线程
		void CleanupThread(WSAEVENT Event, WSAEVENT ShutdownEvent, SOCKET s)
		{
			if ( Event ) WSACloseEvent(Event);
			if ( ShutdownEvent ) WSACloseEvent(ShutdownEvent);
			if ( s ) closesocket(s);
			//WSACleanup();
		}

	private:
		HANDLE					m_ThreadA; // Accept Thread
		DWORD					m_ThreadA_ID;
		HANDLE					m_ThreadC; // Cleanup Thread
		DWORD					m_ThreadC_ID;
		WSAEVENT				m_ShutdownEvent;
		HANDLE					m_ThreadLaunchedEvent;
		HANDLE					m_WaitForCloseEvent;
		IDHANDLE				m_ThreadList;
		HANDLES					m_HandleList;
		CRITICAL_SECTION		m_cs;
		CRITICAL_SECTION		m_cs_;
		int						m_ServerPort;
		int						m_PersistenceTO;
		bool					m_bRun;
	protected:
		STRVECT					m_Visitors;
	};

#define SERVERNAME			"InnerImgServer/0.1b"
	typedef map<string, string>	MIMETYPES;

	class CInnerImgServer : public CGenericServer
	{
	public:
		CInnerImgServer(void)
		{
			// 初始化 MIME 类型，这里只需支持图片即可
			MimeTypes["bmp"]	= "image/bmp";
			MimeTypes["gif"]	= "image/gif";
			MimeTypes["jpeg"]	= "image/jpeg";
			MimeTypes["jpg"]	= "image/jpeg";
			MimeTypes["tif"]	= "image/tiff";
			MimeTypes["tiff"]	= "image/tiff";
			MimeTypes["xbm"]	= "image/xbm";
		}
		virtual ~CInnerImgServer(void){}

		//启动服务，重要函数，分析根目录
		bool Start(const CString & appPath, const CString & dataPath, int Port, int PersTO)
		{
			m_appPath = appPath;
			m_dataPath = dataPath;
			return Run(Port, PersTO);
		}

		//是否完成
		bool IsComplete(string szRequest)
		{
			return ( szRequest.substr( szRequest.size() - 4, 4 ) == "\r\n\r\n" );
		}

		//分析请求数据
		bool ParseRequest(string szRequest, string &szResponse, bool &bKeepAlive)
		{
			string szMethod;
			string szFileName;
			string szFileExt;
			string szStatusCode("200 OK");
			string szContentType("text/html");
			string szConnectionType("close");
			string szNotFoundMessage;
			string szDateTime;
			char pResponseHeader[2048];
			fpos_t lengthActual = 0, length = 0;
			using namespace dblite;

			// 检查提交方法
			int n = szRequest.find(" ", 0);
			if ( string::npos == n )
			{
				return false;
			}
			if ( szRequest.substr(0, n) != "GET" )
			{
				szStatusCode = "501 Not Implemented";
				szFileName = "";
			}
			else
			{
				// 获取文件名
				int n1 = szRequest.find(" ", n + 1);
				if ( string::npos == n1 )
				{
					return false;
				}
				szFileName = szRequest.substr(n + 1, n1 - n - 1);
				if ( "/" == szFileName )
				{
					szFileName = "";
				}
				if ( '/' == szFileName[0] )
				{
					szFileName.erase(0, 1);
				}
			}

			// 分析链接类型
			if ( ( n = szRequest.find( "\nConnection: Keep-Alive", 0 ) ) != string::npos )
				bKeepAlive = true;

			// 分析内容类型
			int nPointPos = szFileName.rfind(".");
			if ( string::npos != nPointPos )
			{
				szFileExt = szFileName.substr( nPointPos + 1, szFileName.size() );
				strlwr( (char*)szFileExt.c_str() );
				MIMETYPES::iterator it = MimeTypes.find(szFileExt);
				if ( it != MimeTypes.end() )
					szContentType = (*it).second;
			}

			//得到目前的时间
			char szDT[128];
			struct tm *newtime;
			time_t ltime;

			time(&ltime);
			newtime = gmtime(&ltime);
			strftime( szDT, 128, "%a, %d %b %Y %H:%M:%S GMT", newtime );

			// 读取文件
			HANDLE hdb = 0;
			dblite::Variant vt;
			if ( m_appPath.GetLength() && m_dataPath.GetLength() && szFileName.size() >= 36 )
			{
				// 获得文件大小
				dblite::IDataSet * pds = 0;
				OpenDB(m_appPath, m_dataPath, hdb, true);
				CStringW sql_Query, hash = CA2W( szFileName.substr( 0, nPointPos ).c_str() );
				sql_Query.Format( L"select img from %s where hash='%s';", QMV_TABLE_Rec, hash );
				ExeDB(m_appPath, hdb, sql_Query, pds);
				if ( pds )
				{
					vt = pds->Read();
				}

				// 返回响应
				sprintf(pResponseHeader, "HTTP/1.0 %s\r\nDate: %s\r\nServer: %s\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\nConnection: %s\r\nContent-Type: %s\r\n\r\n",
					szStatusCode.c_str(), szDT, SERVERNAME, vt.value.streamValue.len, bKeepAlive ? "Keep-Alive" : "close", szContentType.c_str());
			}
			else
			{
				szStatusCode = "404 Resource not found";
				sprintf(pResponseHeader, "HTTP/1.0 %s\r\nContent-Length: 0\r\nContent-Type: text/html\r\nDate: %s\r\nServer: null\r\n\r\nnull",
					szStatusCode.c_str(), szDT);
				bKeepAlive = false;
			}

			szResponse = string(pResponseHeader);
			if ( dblite::VariantType_Stream == vt.type && vt.value.streamValue.buf && vt.value.streamValue.len )
				szResponse += string(vt.value.streamValue.buf, vt.value.streamValue.len);
			ShutDB(m_appPath, hdb);
			return true;
		}

	private:
		MIMETYPES		MimeTypes;
		CString			m_appPath;
		CString			m_dataPath;
	};
}

class CLoadQmv
{
public:
	CLoadQmv(const CString & appPath, const CString & dataPath)
	{
		m_dataPath = dataPath + QMV_DB;
		m_appPath = appPath + _T("\\") + QMV_DLL;
		m_simgSvr.Start(m_appPath, m_dataPath, DEF_PORT, THREADKILL_TO*5);
	}
	void UnLoad(void)
	{
		m_simgSvr.Shutdown();
	}

	/*
	** 接口名：QmvReadTag
	** 功能描述：读取标签数据，
	**     1、请求某部电影的所有标签，参数是"one|电影名|"；
	**     2、请求标签总体的某部分内容，参数是"all|起始索引|结束索引|"；
	** 返回值：jeson格式串
	** {
	**   tag:
	**   [
	**     {
	**      tag_hash:'32字节数字和字母',movie_name:'异形(htmlUnicode编码)',
	**      tag_title:'花裙(htmlUnicode编码)',tag_des:'美剧(htmlUnicode编码)',
	**      tag_link:'http://taobao...',img_url:'http://localhost:8888/tag_hash.jpg',
	**      start_time:1200,end_time:1220,qvod_link:'qvod:|长度|40字节字母数字|名称|'
	**     }
	**   ]
	** {
	*/
	HRESULT QmvReadTag(LPCTSTR szName, int start, int end, VARIANT *pVal)
	{
		bool bValidRange = ( -1 < start && start <= end );
		if ( ( !szName || !szName[0] ) && !bValidRange )
		{
			return KnowledgeTag(pVal);
		}

		pVal->bstrVal = 0;
		pVal->vt = VT_BSTR;
		using namespace QmvTool;
		CStringW sql_Query, val, hash;
		if ( szName && szName[0] )
		{
			val = CA2W( QmvTool::CQmvTool::encode( CString(szName) ).c_str() );
			sql_Query.Format( L"select * from %s where fname='%s';", QMV_TABLE_Rec, val );
		}
		else if ( start < end && 0 < start && 0 < end )
		{
			sql_Query.Format( L"select * from %s order by id desc;", QMV_TABLE_Rec );
		}
		HANDLE hdb = 0;
		dblite::IDataSet * pds = 0;
		OpenDB(m_appPath, m_dataPath, hdb, true);
		ExeDB(m_appPath, hdb, sql_Query, pds);
		if ( pds )
		{
			dblite::Variant vt;
			CMakeJesonString jstring(_T("tag"));
			for ( int i = 0, row = pds->GetRowCount(); i < row; ++i )
			{
				if ( bValidRange && ( i < start - 1 || i > end - 1 ) )
				{
					continue;
				}
				jstring.NewItem();
				for ( int j = 0, col = pds->GetColumnCount(); j < col; ++j )
				{
					vt = pds->Read(i, j);
					Variant2JString(jstring, vt, hash, j);
				}
			}
			pVal->bstrVal = SysAllocString(jstring.GetString());
		}
		ShutDB(m_appPath, hdb);
		return 0;
	}

	HRESULT QmvReadKnowledgeMovieList(VARIANT *pVal)
	{
		CString val;
		VARIANT vt = {0};
		int n = KnowledgeTag(&vt);
		SysFreeString(vt.bstrVal);
		CMakeJesonString jstring(_T("data"));
		jstring.NewItem();
		jstring.AddPair( _T("movie_name"), _T("''") );
		val.Format( _T("%d"), n );
		jstring.AddPair( _T("tag_count"), val );
		jstring.AddPair( _T("cur_index"), _T("1") );
		pVal->vt = VT_BSTR;
		pVal->bstrVal = SysAllocString(jstring.GetString());
		return 0;
	}

	/*
	** 接口名：QmvReadMovieList
	** 功能描述：读取本地所有具有标签的电影名称列表，
	** 返回值：jeson格式串
	** {
	**   data:
	**   [
	**     {movie_name:'异形前传',movie_count:5,cur_index:2},
	**     {movie_name:'异形',movie_count:2,cur_index:2}
	**   ]
	** }
	*/
	HRESULT QmvReadMovieList(LPCTSTR szOneOrAll, LPCTSTR szCurName, int curIndex, VARIANT *pVal)
	{
		bool bOne = ( szOneOrAll && szOneOrAll[0] );
		if ( bOne && 0 > curIndex )
		{
			return 0;
		}
		HANDLE hdb = 0;
		pVal->bstrVal = 0;
		pVal->vt = VT_BSTR;
		CString sql_Query, val;
		using namespace QmvTool;
		vector<string> fnames_en;
		vector<CString> fnames_de;
		dblite::IDataSet * pds = 0;
		OpenDB(m_appPath, m_dataPath, hdb, true);
		if ( bOne )
		{
			fnames_de.push_back( CString(szOneOrAll) );
			fnames_en.push_back( CQmvTool::encode( fnames_de.back() ) );
		}
		else
		{
			sql_Query.Format( _T("select fname from %s order by id desc;"), QMV_TABLE_FL );
			ExeDB( m_appPath, hdb, sql_Query, pds );
			if ( pds )
			{
				dblite::Variant vt;
				for ( int i = 0, row = pds->GetRowCount(); i < row; ++i )
				{
					vt = pds->Read();
					if ( dblite::VariantType_Stream == vt.type && vt.value.streamValue.buf && vt.value.streamValue.len )
					{
						fnames_en.push_back( string( vt.value.streamValue.buf, vt.value.streamValue.len ) );
						fnames_de.push_back( CQmvTool::decode( fnames_en.back() ) );
					}
				}
			}
		}
		CMakeJesonString jstring(_T("data"));
		CString curName = ( (szCurName && szCurName[0]) ? szCurName : _T(""));
		for ( int i = 0, size = fnames_en.size(); i < size; ++i )
		{
			val = CA2CT(fnames_en[i].c_str());
			sql_Query.Format( _T("select count(*) from %s where fname='%s';"), QMV_TABLE_Rec, val );
			ExeDB( m_appPath, hdb, sql_Query, pds );
			if ( pds )
			{
				dblite::Variant vt = pds->Read();
				jstring.NewItem();
				val.Format( _T("'%s'"), CQmvTool::to_html_unicode( fnames_de[i] ) );
				jstring.AddPair( _T("movie_name"), val );
				val.Format( _T("%d"), vt.value.int32Value );
				jstring.AddPair( _T("tag_count"), val );
				val.Format( _T("%d"), ( bOne || fnames_de[i] == curName ) ? curIndex : 0 );
				jstring.AddPair( _T("cur_index"), val );
			}
		}
		ShutDB(m_appPath, hdb);
		pVal->bstrVal = SysAllocString(jstring.GetString());
		return 0;
	}

	HRESULT QmvGetDataCount(VARIANT *pVal)
	{
		HANDLE hdb = 0;
		pVal->vt = VT_I4;
		pVal->intVal = 0;
		CString sql_Query;
		dblite::IDataSet * pds = 0;
		OpenDB(m_appPath, m_dataPath, hdb, true);
		sql_Query.Format( _T("select count(*) from %s;"), QMV_TABLE_Rec );
		ExeDB( m_appPath, hdb, sql_Query, pds );
		if ( pds )
		{
			dblite::Variant vt = pds->Read();
			pVal->intVal = vt.value.int32Value;
		}
		ShutDB(m_appPath, hdb);
		return 0;
	}

private:
	CLoadQmv(void){}
	HRESULT KnowledgeTag(VARIANT *pVal)
	{
		HANDLE hdb = 0;
		int needRead = 0;
		pVal->bstrVal = 0;
		pVal->vt = VT_BSTR;
		using namespace QmvTool;
		dblite::IDataSet * pds = 0;
		CString sql_Query, hash, val;
		const int defReadCount = 512;
		OpenDB(m_appPath, m_dataPath, hdb, true);
		sql_Query.Format( _T("select * from %s order by id desc limit %d;"), QMV_TABLE_Rec, defReadCount );
		ExeDB( m_appPath, hdb, sql_Query, pds );
		if ( pds )
		{
			dblite::Variant vt;
			bool bb[defReadCount] = {0};
			CMakeJesonString jstring(_T("tag"));
			int total = pds->GetRowCount(), businessCount = 0;
			needRead = min(total, knowledge_tag_count + business_tag_count);

			// 先标记所有标签类型，并统计商业标签个数
			for ( int i = 0; i < total; ++i )
			{
				vt = pds->Read(i, 11/*link*/);
				bb[i] = IsLinkBusiness( CQmvTool::decode( string( vt.value.streamValue.buf, vt.value.streamValue.len ) ) );
				businessCount += ( bb[i] ? 1 : 0 );
			}

			// 计算读取个数
			int read_knowledge_tag_count = min(total - businessCount, knowledge_tag_count), read_business_tag_count = min(businessCount, business_tag_count);
			// 商业标签个数够不够
			if ( businessCount < business_tag_count )
			{
				// 不够就用知识标签补充
				read_knowledge_tag_count += (business_tag_count - businessCount);
				// 补充后不能大于总数
				if ( read_knowledge_tag_count > needRead )
				{
					read_knowledge_tag_count = needRead;
				}
			}
			// 知识标签个数够不够
			if ( total - businessCount < knowledge_tag_count )
			{
				// 不够就用商业标签补充
				read_business_tag_count += (knowledge_tag_count - total + businessCount);
				// 补充后不能大于总数
				if ( read_business_tag_count > needRead )
				{
					read_business_tag_count = needRead;
				}
			}

			// 读知识性标签
			for ( int i = 0, j = 0; i < read_knowledge_tag_count && j < total; ++j )
			{
				if ( bb[j] )
				{
					continue;
				}
				jstring.NewItem();
				for ( int k = 0, col = pds->GetColumnCount(); k < col; ++k )
				{
					vt = pds->Read(j, k);
					Variant2JString(jstring, vt, hash, k);
				}
				++i;
			}

			// 再读商业标签
			for ( int i = 0, j = 0; i < read_business_tag_count && j < total; ++j )
			{
				if ( !bb[j] )
				{
					continue;
				}
				jstring.NewItem();
				for ( int k = 0, col = pds->GetColumnCount(); k < col; ++k )
				{
					vt = pds->Read(j, k);
					Variant2JString(jstring, vt, hash, k);
				}
				++i;
			}

			pVal->bstrVal = SysAllocString(jstring.GetString());
		}
		ShutDB(m_appPath, hdb);
		return needRead;
	}

	bool IsLinkBusiness(CString & link)
	{
		const TCHAR * business_domain[] =
		{
			_T("taobao"), _T("tmall"), _T("tianmao"), _T("360buy")
		};
		const TCHAR **p = business_domain;
		for (int i = 0, size = sizeof(business_domain)/sizeof(*business_domain); i < size; ++p, ++i)
		{
			if ( -1 != link.Find(*p) )
			{
				return true;
			}
		}
		return false;
	}

	void Variant2JString(CMakeJesonString & jstring, dblite::Variant & vt, CString & hash, int col)
	{
		using namespace dblite;
		using namespace QmvTool;
		CString val;
		switch ( col )
		{
		case 1: // hash
			if ( VariantType_Stream == vt.type && vt.value.streamValue.buf && vt.value.streamValue.len )
			{
				hash = CA2CT( string(vt.value.streamValue.buf, vt.value.streamValue.len).c_str() );
				val.Format( _T("'%s'"), hash );
				jstring.AddPair( _T("tag_hash"), val );
			}
			break;
		case 2: // fname
			if ( VariantType_Stream == vt.type && vt.value.streamValue.buf && vt.value.streamValue.len )
			{
				val.Format( _T("'%s'"), CQmvTool::to_html_unicode( CQmvTool::decode( string( vt.value.streamValue.buf, vt.value.streamValue.len ) ) ) );
				jstring.AddPair( _T("movie_name"), val );
			}
			break;
		case 5: // start
			if ( VariantType_Int64 == vt.type && vt.value.int64Value )
			{
				val.Format( _T("%I64d"), vt.value.int64Value );
				jstring.AddPair( _T("start_time"), val );
			}
			break;
		case 6: // end
			if ( VariantType_Int64 == vt.type && vt.value.int64Value )
			{
				val.Format( _T("%I64d"), vt.value.int64Value );
				jstring.AddPair( _T("end_time"), val );
			}
			break;
		case 8: // netpath
			val = _T("''");
			if ( VariantType_Stream == vt.type && vt.value.streamValue.buf && vt.value.streamValue.len )
			{
				val.Format( _T("'%s'"), CQmvTool::to_html_unicode( CQmvTool::decode( string( vt.value.streamValue.buf, vt.value.streamValue.len ) ) ) );
			}
			jstring.AddPair( _T("qvod_link"), val );
			break;
		case 9: // title
			if ( VariantType_Stream == vt.type && vt.value.streamValue.buf && vt.value.streamValue.len )
			{
				val.Format( _T("'%s'"), CQmvTool::to_html_unicode( CQmvTool::decode( string( vt.value.streamValue.buf, vt.value.streamValue.len ) ) ) );
				jstring.AddPair( _T("tag_title"), val );
			}
			break;
		case 10: // detail
			val = _T("''");
			if ( VariantType_Stream == vt.type && vt.value.streamValue.buf && vt.value.streamValue.len )
			{
				val.Format( _T("'%s'"), CQmvTool::to_html_unicode( CQmvTool::decode( string( vt.value.streamValue.buf, vt.value.streamValue.len ) ) ) );
			}
			jstring.AddPair( _T("tag_des"), val );
			break;
		case 11: // link
			if ( VariantType_Stream == vt.type && vt.value.streamValue.buf && vt.value.streamValue.len )
			{
				CString filter = CQmvTool::decode( string( vt.value.streamValue.buf, vt.value.streamValue.len ) );
				filter.Replace(_T("'"), _T(""));
				filter.Replace(_T("\r\n"), _T(""));
				val.Format( _T("'%s'"), filter );
				jstring.AddPair( _T("tag_link"), val );
				jstring.AddPair( _T("business"), IsLinkBusiness(val) ? _T("1") : _T("0") );
			}
			break;
		case 12: // img
			if ( VariantType_Stream == vt.type && vt.value.streamValue.buf && vt.value.streamValue.len )
			{
				val.Format( _T("'http://localhost:%d/%s.jpg'"), m_simgSvr.GetPort(), hash );
				jstring.AddPair( _T("img_url"), val );
			}
			break;
		}
	}

private:
	CString							m_appPath;
	CString							m_dataPath;
	QmvBrImgSvr::CInnerImgServer	m_simgSvr;
};
