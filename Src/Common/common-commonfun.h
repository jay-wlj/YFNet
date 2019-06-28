#ifndef _OTHERFUN
#define _OTHERFUN

#define _OUTPUT Printf
#define IN
#define OUT

#define LIB_VERSION	"1.2.11.81"

#include <string>
using std::string;

enum _LOG_LEVEL
{
	INFO,LOG_DEBUG,FAIL,STOP,SERIOUS,ERROR1,INEED,FORECE
};

struct	HString
{
	int length;
	const char* pstr;
};

enum _NOTIFY_ID {
	NOTIFY_DOWN_FINISH=0,				// 任务下载完成
	NOTIFY_WRITE_ERROR,					// 文件写入错误通知
	NOTIFY_NO_SPACE,					// 缓存目录没空间
	NOTIFY_DWON_ERROR,					// 下载链接出错或失效等403
	NOTIFY_PORT_BIND,					// 代理端口绑定通知
	NOTIFY_DOWN_RETRY,					// 下载的数据重复请求次数
	NOTIFY_SIGN_ERROR=6,				// 下载的数据签名校验失败
};


bool GetPrivateVersion(char* pszVersion);
void SetLogPath(const char* path);
void IpInt2Str(int iIp,char* strIp);
void PrintHash(char* hash,int len=20);
unsigned int Str2Int(const char* chars,int begin,int end);
void Printf(_LOG_LEVEL level,char* fmt,...);
void Printf(_LOG_LEVEL level,const char* fmt,...);
void GetCurTime(char* szDTime,unsigned char mode=0);
void Setloglevel(_LOG_LEVEL level);
void SetGlobalSpeed(int mode);
void GetNameFromUrl(IN char* url,OUT char* name);
char* GetPreviewPoint(char* url);
int ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr,bool once=false);
bool KmpStrExist(HString* source,char* target,int pos=0);
bool IsQvodTorrent(const char* torrent,int torrentlen);
unsigned long GetHostByName(const char* name, bool bDelExist = false);

string ParseHostIp(const char *host);
int GetIpFromDomain(unsigned long& ip,char* host);
void CallBackToUI(int id,int result,const char* buf, int len);
#endif


