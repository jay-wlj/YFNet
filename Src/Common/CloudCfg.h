#ifndef _CLOUDCFG_H_
#define _CLOUDCFG_H_
#include <string>
#include <map>
#include <vector>
#include "qvod_lock.h"
#include "qvod_sock.h"
#include "qvod_thread.h"
using namespace std;

enum CloudCfgKey
{
	KEY_GET_TIME = 17,								//获取服务器时间
	KEY_AUTO_UPLOAD_LIMIT_PERCENT = 101,			//正常模式上传限制百分比
	KEY_INTERNET_UPLOAD_LIMIT_PERCENT = 102,		//上网模式上传限制百分比
	KEY_MOVIE_UPLOAD_LIMIT_PERCENT = 103,			//观影模式上传限制百分比
	KEY_GAME_UPLOAD_LIMIT_PERCENT = 104				//游戏模式上传限制百分比
};

struct CloudCfgReq
{
	unsigned short len;//请求包长度
	unsigned short version;//客户端版本号
	unsigned short key;//请求的ID
	unsigned char  reserved[2];
};
struct CloudCfgRsp
{
	unsigned short len;//该响应包的长度
	unsigned short key;
	unsigned int   timeout;//超时时间，单位秒
	//char*          val;//key对应的值
};

class CCloudCfg
{
public:
	CCloudCfg();
	virtual ~CCloudCfg();

	int GetCloudCfg(unsigned short key,int& value);
	int GetCloudCfg(unsigned short iBeginKey,unsigned short iEndKey,int* pValueArray);
	int GetCloudCfg( unsigned short key,string& strValue );

	void Init();
	void UnInit();

	static void QVOD_WINAPI ThreadProc(QVOD_LPVOID lpParam);

	static CCloudCfg* Instance();
	static void FreeInstance();
private:
	bool OpenTcpSocket(qvod_sock_t& sock);
	bool CloseTcpSocket(qvod_sock_t& sock);
	bool GetConfigValue(qvod_sock_t& sock, unsigned short key,int& value);
	int GetIpFromDomain(unsigned long& ip);
	static CCloudCfg*	m_instance;

	qvod_thread_t	m_hThread;
	bool	m_bExit;
};
#endif