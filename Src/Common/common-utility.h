
#ifndef _COMMON_UTILITY_H
#define _COMMON_UTILITY_H

#define WIN32_LEAN_AND_MEAN

#include <string>
#include <queue>
#include <list>
#include <map>
#include <vector>
#include <set>
using namespace std;

#include "qvod_string.h"
#include "qvod_event.h"
#include "qvod_thread.h"
#include "qvod_sock.h"
#include "common-commonstruct.h"
#include "protocol.h"
#include "global.h"

#ifdef WIN32
#include "Console.h"
#endif

#define EXPORT_BY_PATH			0x01
#define EXPORT_BY_DATE			0x02
#define EXPORT_BY_NAME			0x04
#define EXPORT_ALL				0x00

#define DEFUPLAODLIMIT 0
#define DEFPEERSLIMIT  35
#define DEFSEEDSLIMIT  100
#define DEFCONNECTINGLIMIT 10


#define FILE_ATTRIBUTE_ARCHIVE_NORMAL 0x000000A0
#define FILE_ATTRIBUTE_HIDDEN_SYSTEM  0x00000006
//UserID
extern unsigned int g_iSeedEncKey;//种子加密key
extern char         g_szSPeerID[24];
extern unsigned int g_iPeerIDkey;

extern bool        g_bEnableUpload;
extern bool        g_bIsRunBuild;
extern unsigned char g_protocol;//协议类型
extern bool          g_isForceUdp; //是否强制采用UDP
extern bool          g_bDownRateReact;//快速读取一个下载限速包
extern bool			g_bUpRateReact;
extern unsigned int g_iUpRateLimit;//上传速度限制
extern unsigned int g_iDownRateLimit;//下载速度限制
extern std::string   g_media_dir;
extern std::string	 g_strtorrentpath;
extern std::string   g_strCurrentRoot;
extern std::string	  g_strConfigRoot; /* silver add */
extern std::string  g_strFullFileCopyPath;
extern std::string   g_strLogPath;
extern unsigned int  g_iLocalIP;
extern unsigned short g_httplimited;
extern unsigned short g_httpport;
extern char g_keycode[20];
extern bool g_bRunning;
extern bool g_isUpdateThreadRun;
extern qvod_thread_event_t g_UpdateEvent;
extern time_t g_starttime;
extern pthread_mutex_t	   g_UpdateEventLock;

extern unsigned int 	g_cacheAgentIP;

extern QVOD_INT64		g_iMaxUseSpace;
// unsigned char        g_cPeersLimit=35;//用户数限制
// unsigned char        g_cSeedsLimit=100;//种子数限制
// unsigned char        g_cConnectLimit=0;//连接数限制

//默认参数设置
extern unsigned char  g_task_PeersLimit;
extern unsigned char  g_task_SeedsLimit;
extern unsigned char  g_task_UpRateLimit;
extern unsigned short g_outdebuginfo;
extern int            g_iEnableDelTask;//是否允许删除网络任务
extern int			  g_iMinValue;     //最小进度为多少
extern int			  g_speedlimit;
extern int			  g_userSpeedlimit;
extern int			  g_lastSpeedlimit;
extern bool g_bquickly;
extern int  g_iKeepTerminal;
extern bool g_bEncodeFile;//网吧服务器是否加密保存文件
extern unsigned short g_iMaxTerminalVersion;
extern unsigned short g_iMinTerminalVersion;
extern unsigned short g_iTerminalProtocol;

extern unsigned int	  g_iQvodType;
extern unsigned int   g_accBlockSize;
extern time_t g_iDefaultIPLastWriteTime;		//默认IP列表文件最后修改时间

extern int			   g_iPowerboot;
extern int             g_iWebInvoke;
extern QVOD_DWORD		   g_httpLastConnTime;
extern unsigned int    g_webAccSize;
extern QVOD_UINT64          g_webCacheSpace;

extern bool				g_bUseP2p;
extern bool				g_bIsUpgradeProc;


extern bool g_benablewebservice;
extern FILE* debug_file;
extern bool g_bNetworkRun;//是否开启网络

#if QVOD_DEBUG_SCREEN
extern ZConsole* pConsole;
#endif

#ifdef SUPERNODE
extern bool g_benablewebservice;
#endif


extern unsigned char g_szIspID[5];

typedef struct _piece
{
    char hash[20];
} Piece;

void ProgramQuit();
std::string Hash2Char(const unsigned char* hash, const unsigned int nLen=20);
void Char2Hash(const char* chars,unsigned char* hash,  const unsigned int nLen=20);
int GetAddrFromHTTP(std::string&,unsigned int&,unsigned short& port);
int GetAddrFromHttp(std::string& HTTPaddr,unsigned int& ip,unsigned short& port);
int GetAddrFromHost(const char* buf,unsigned int& ip,unsigned short& port);
void urlEncode(const char *src, int nLenSrc, char *dest, int& nLenDest);
void urlDecode(const char *src, int nLenSrc, char* dest, int& nLenDest);
std::string URLDecode(const char* str, const unsigned len);
void utf8Decode2(const char *src, int nLenSrc, char* dest, int& nLenDest);
void utf8Decode(const char *src, int nLenSrc, char* dest, int& nLenDest);
unsigned int CalcPieceSize(QVOD_INT64 filesize);
int Build(std::string strSourcePath,std::string strName,std::string strSavePath,std::string strAdUrl,std::string strTracker,
		  std::string strTextAdUrl="",std::string strPauseAdUrl="");
bool GetHttpUrl(const char* url,const char* savefilename);
bool GetHttpUrl(const char* url,std::string& strRet);
bool PostHttpUrl(const char* url, const std::string& postdata, string& strRet, const std::string& strHeaders="", const int& nTimeout=5000);
int GetFolderFiles(std::string strfolder, std::vector<std::string>& vctfile,std::string stradurl);
char* GetLine_(char*& pstr);
string GetLine_(const char* &pstr);
char GetFreeDisk(char srcdick,QVOD_UINT mediesize);
char GetMaxDisk(QVOD_UINT64& freespace);
char  GetMaxDisk();
void ChangeName(std::string& name, int& count);
bool  GetSavePath(std::string& strPath,QVOD_INT64 iFileLen);
char GetNextFreeDisk(QVOD_INT64 iFileLen);
std::string GetKeyCode(char* hash);
std::string GetPwdCode(char* hash, QVOD_UINT64 pos, char* date);
bool GetMacAddr(char* mac);
bool isQvodLink(char* req,char* boundary="|");
char *to_lower(char *pstr);
bool IsQhttpTask(const char *pstrSrcUrl);
std::string GetPathSuffix(const char* pstrSrcUrl);

//判断所给磁盘的空间能不能再下载一个MediaSize大小的任务
//chDisk:盘符,MediaSize:下载文件占用的磁盘大小
bool IsDiskSpaceEnough(char chDisk, QVOD_UINT64 MediaSize);

//修正文件名,把文件名里面的非常规字符换成空格
//added by tangtao 2010-04-09
void ReviseFileName(std::string &strFile);

void CalcSoftVersion();
//根据版本号判断是否使用4号协议
bool IsUseP4(int version);
#ifndef SUPERNODE
//共享IE临时文件夹下面的音频或视频文件
#else
//cache下载一个文件前,判断所有磁盘对应目录下是否存在这个文件
bool IsFileExistOnCache(const char *pFilePath,char *pExistPath,int iBufLen);
#endif

void CreateDirectoryForAllUser(const char *wpDir);
bool CreateFolderForAllUser( const char* pFolder );

#ifdef WIN32 /* WIN32 */
#else /* posix */
int GetFoldersAndFiles(std::string strfolder, std::vector<std::string>& vctfile);
#endif /* posix end */

QVOD_ULONGLONG htonl64(QVOD_ULONGLONG hostlong);
QVOD_ULONGLONG ntohl64(QVOD_ULONGLONG netlong);
bool QueryP2pHash( const char* pOrgurl,const char* pKeyurl,SQueryUrltrackerResult& result );
std::string HttpGetValue(const char* buf, const char* key);
bool SetNonBlock( int fd );
bool AsynConnect(int sock,qvod_sockaddr_t addr,int timeout);
#endif
int MoveFolder(std::string strfolder, std::string dest);
void Fake2Real( const unsigned char *pFake,unsigned char *pReal );
void Real2Fake( const unsigned char *pReal,unsigned char *pFake );

