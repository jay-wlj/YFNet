#include "common-commonfun.h"
#include <iostream>
#include <stdarg.h>
#define WIN32_LEAN_AND_MEAN
#include "qvod_string.h"
#include "qvod_time.h"
#include "qvod_sock.h"

#define  LOG_TAG    "p2p_session"
#ifdef WIN32 /* WIN32 */
#include "Console.h"
#elif defined MACOSX
#define LOGI(...) printf("%s %s", LOG_TAG, __VA_ARGS__)
#else
#include <android/log.h>
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif /* WIN32 end */

#include "common-utility.h"
#include "common-commonstruct.h"
#include "global.h"
#include "common-DNSCache.h"
#include "IYfNetPlush.h"

using namespace std;
#ifdef QVOD_DEBUG_FILE
extern FILE* debug_file;
#endif

extern EventCallBackFunc g_eventCallBackFunc;

_LOG_LEVEL g_log_level;

bool GetPrivateVersion(char* pszVersion)
{
	bool bRet = false;
	if (pszVersion)
	{
		strcpy(pszVersion,LIB_VERSION);
#ifdef _DEBUG 
		strcat(pszVersion,"_d");
#elif DEBUG
		strcat(pszVersion,"_d");
#elif ANDROID_DEBUG
        strcat(pszVersion,"_d");
#endif
		bRet = true;
	}
	return bRet;
}

void SetLogPath(const char* path)
{
	if (path)
	{
		g_strLogPath = path;
		if (g_strLogPath[g_strLogPath.length()-1] != '/')
		{
			g_strLogPath += '/';
		}
	}
}
void IpInt2Str(int iIp,char* strIp)
{
#ifdef WIN32 /* WIN32 */
	unsigned char ip[4];
	memcpy(ip,(void*)&iIp,4);
	int pos=0;
	for(int i=0;i<4;i++)
	{
		pos+=strlen(_itoa(ip[i],strIp+pos,10));
		if(i!=3)
		{
			strIp[pos]='.';
			pos+=1;		
		}
	}
#else /* posix */

	inet_ntop(AF_INET, &iIp, strIp, 16); /* should less than 16 bytes */

#endif /* posix end */
}
void PrintHash(char* hash,int len)
{
	for(int i=0;i<len;i++)
		printf("%02X",0x000000ff&hash[i]);
	printf("\n");
}
unsigned int Str2Int(const char* chars,int begin,int end)
{
	int i = begin;
	unsigned int hash = 0;
	while (i <= end) 
	{
		hash *= 10;
		if('9'>= chars[i] && chars[i]>= '0' ) 
			hash += (chars[i] - '0');
		else
			return 0;
		i++;
	}
	return hash;
}

void GetLogInfo(_LOG_LEVEL level,char* str)
{
	switch(level)
	{
	case  INFO:
		sprintf(str,"INFO ");
		break;		
	case  LOG_DEBUG:
		sprintf(str,"DEBUG ");
		break;		
	case  FAIL:
		sprintf(str,"FAIL ");
		break;		
	case  STOP:
		sprintf(str,"STOP ");
		break;
	case SERIOUS: /* silver add */
		sprintf(str,"SERIOUS ");
		break;
	case ERROR1:
		sprintf(str,"ERROR1 ");
		break;
	case INEED: /* silver add */
		sprintf(str,"INEED ");
		break;
	}
}

void Printf(_LOG_LEVEL level,char* fmt,...)
{	 	
	Printf(level,(const char*)fmt);
}

void Printf( _LOG_LEVEL level,const char* fmt,... )
{
	if (level < g_log_level)	// 打印日志级别低于设定级别，则忽略打印
	{
		return;
	}
	char str[102400]={0};
	GetLogInfo(level,str);
	GetCurTime(str+strlen(str));

	try {
		va_list argptr;
		va_start(argptr, fmt);
		vsprintf(str+strlen(str),fmt, argptr);
		va_end(argptr);
	} 
	catch (...)
	{
		Printf(ERROR1, "catch Prinf  fmt=%s ", fmt);
		return;
	}
	// 末尾是否已有换行
	const int len = strlen(str);
	if (str[len-1] != '\n' && len+1 < 102400)
	{
		str[len] = '\r';
		str[len+1] = '\n';
	}

#if QVOD_DEBUG_SCREEN
		if(g_outdebuginfo > 0 && g_outdebuginfo < 3)//1 or 2
		{
			//printf(str);
			if(pConsole != NULL && g_outdebuginfo > 0 && g_outdebuginfo < 3)//1 or 2
			{
				(*pConsole)<<str;
			}
		}
#else
#ifndef WIN32
	LOGI(str);
#endif
#endif

#ifdef QVOD_DEBUG_FILE
	if(level >= g_log_level && g_outdebuginfo > 1)
	{
        if(NULL == debug_file)
        {
            if (!g_strLogPath.empty())
            {
                //int ret = QvodMkdir(g_strLogPath.c_str());		// 创建失败的话会循环创建导致堆栈溢出
                char curTime[255] = {0};
                GetCurTime((char*)curTime,1);
                string strLogPath = g_strLogPath;
				strLogPath += curTime;
                strLogPath += ".txt";
                debug_file=fopen(strLogPath.c_str(),"w+");
				if (debug_file)
				{
#ifdef QVODDEBUG
					char szVersion[50] = {0};
					GetPrivateVersion(szVersion);
					Printf(INFO, "-----------------BEGIN ver:%s---------------------", szVersion);
#endif
                }
	         }            
        }
		if(debug_file != NULL)//2 or 3
		{
			int len = fwrite(str,strlen(str),1,debug_file);
			fflush(debug_file);
		}
	}
#endif
}

void Setloglevel(_LOG_LEVEL level)
{
	g_log_level = level;
}

void GetCurTime(char* szDTime,unsigned char mode)
{
#ifdef WIN32 /* WIN32 */

    SYSTEMTIME st;
    GetLocalTime(&st);// gets current time
	switch(mode)
	{
	case 0:
		sprintf(szDTime,"%04d-%02d-%02d %02d:%02d:%02d:%03d ",
			st.wYear,st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		break;
	case 1:
		sprintf(szDTime,"%04d-%02d-%02d-%02d-%02d-%02d",
			st.wYear,st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		break;
	}
// 	sprintf(szDTime," %04d-%02d-%02d %02d:%02d:%02d.%03d ",
// 		st.wYear, st.wMonth, st.wDay, st.wHour, 
// 		st.wMinute, st.wSecond, st.wMilliseconds);

#else /* posix */

	time_t t;
	struct tm st;

	t = time(NULL);
	localtime_r(&t, &st);

	switch(mode)
	{
	case 0:
		{
			struct timeval now;
			gettimeofday(&now, NULL);
			int msec = now.tv_usec/1000;

			sprintf(szDTime,"%04d-%02d-%02d %02d:%02d:%02d:%03d ",
			st.tm_year+1900,st.tm_mon+1, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec, msec);
		}
		break;
	case 1:
		{
			sprintf(szDTime,"%04d-%02d-%02d-%02d-%02d-%02d",
			st.tm_year+1900, st.tm_mon+1, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec);
		}
		break;
	}

//	sprintf(szDTime,"%04d-%02d-%02d %02d:%02d:%02d ",
//			st.tm_year+1900, st.tm_mon+1, st.tm_wday, st.tm_hour,
//			st.tm_min, st.tm_sec);

#endif /* posix end */
}

void GetNameFromUrl(IN char* url,OUT char* name)
{
	if (url == NULL || strlen(url) == 0)
	{
		return;
	}
	char tempUrl[1024] = {0};
	strcpy(tempUrl, url);
	char* pEnd = strrchr(tempUrl, '?');
	if (pEnd)
	{
		*pEnd  = '\0';
	}
	pEnd = strrchr(tempUrl,'/');
	if(pEnd)
	{
		strcpy(name,pEnd+1);
		pEnd = strstr(name,"?");
		if(pEnd)
		{
			*pEnd = '\0';
		}
	}
	else
	{
		strcpy(name,tempUrl);
	}
}

char* GetPreviewPoint(char* url)
{
	char* preview = strstr(url, "act=snap");//for ku6 snap
	if (preview)
	{
		return preview;
	}

	preview = strstr(url, "preview_num=");//for youku preview 
	if (preview)
	{
		return preview;
	}
	return NULL;
}

// 鏇挎崲瀛楃涓蹭腑鐗瑰緛瀛楃涓蹭负鎸囧畾瀛楃涓?
int ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr,bool once)
{
	int  StringLen;
	char caNewString[2048];

	char *FindPos = strstr(sSrc, sMatchStr);
	if( (!FindPos) || (!sMatchStr) )
		return -1;

	while( FindPos )
	{
		memset(caNewString, 0, sizeof(caNewString));
		StringLen = FindPos - sSrc;
		strncpy(caNewString, sSrc, StringLen);
		strcat(caNewString, sReplaceStr);
		strcat(caNewString, FindPos + strlen(sMatchStr));
		strcpy(sSrc, caNewString);
		if(once)
		{
			break;
		}
		FindPos = strstr(sSrc, sMatchStr);
	}

	return 0;
}

void SetGlobalSpeed(int mode)
{
	bool bDownRateReact = false;
	bool bUpRateReact = false;
	unsigned int iDownRate = 0;
	unsigned int iUpRace   = 0;
	switch(mode)
	{
	case 0://无限制
		iDownRate = 0;
		iUpRace = 0;
		break;
	case 1:	// 上网模式
		iDownRate = 500;
		iUpRace = 200;
		break;
	case 2:	// 观影模式
		iDownRate = 1024;
		iUpRace = 100;
		break;
	case 3:	//游戏模式,限制传输		
		iDownRate = 2;
		iUpRace = 1;
		break;
	//case 1://加速模式
	//	iDownRate = 40;
	//	iUpRace = 10;
	//	break;
	//case -1://自动模式
	//	iDownRate = 145;
	//	iUpRace = 255;
	//	break;
	//case 1:case 2:case 3:
	//case 4:case 5:case 6:
	//case 7:case 8:case 9:
	//	iDownRate = 20*speed;
	//	iUpRace = 5*speed;
	//	break;

	default://无限制
		iDownRate = 0;
		iUpRace = 0;
	}
	if(g_iDownRateLimit < iDownRate || iDownRate == 0)
	{
		bDownRateReact = true;
		bUpRateReact = true;
	}
	g_iDownRateLimit = iDownRate;
	g_iUpRateLimit = iUpRace;
	g_bDownRateReact = bDownRateReact;
	g_bUpRateReact = bUpRateReact;
}

void KmpGetNextVal(HString* target,int nextval[])
{
	int i = 1;
	int j = 0;
	nextval[1] = 0;
	while(i < target->length)
	{
		if(j==0 || target->pstr[i-1]==target->pstr[j-1])
		{
			++i;
			++j;
			if(target->pstr[i-1]==target->pstr[j-1])
			{
				nextval[i] = nextval[j];
			}
			else
			{
				nextval[i] = j;
			}
		}
		else
		{
			j = nextval[j];
		}
	}
}

int KmpIndex(HString* source,HString* target,int pos)
{
	//璁＄畻nextval
	int* nextval = new int[target->length+1];
	KmpGetNextVal(target,nextval);

	//鏁版嵁姣旇緝
	int i = pos;
	int j = 0;
	while(i<=source->length && j<target->length)//<=!
	{
		if(j==0 || source->pstr[i]==target->pstr[j])
		{
			++i;
			++j;
		}
		else
		{
			j = nextval[j];
		}
	}

	//鍒犻櫎nextval
	delete[] nextval;

	//鍒ゆ柇缁撴灉
	if(j >= target->length)//>=!
	{
		return i-target->length;
	}
	else
	{
		return 0;
	}
}

bool KmpStrExist(HString* source,char* target,int pos)
{
	HString hTarget;
	hTarget.length = strlen(target);
	hTarget.pstr = target;
	if(KmpIndex(source,&hTarget,pos) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsQvodTorrent(const char* torrent,int torrentlen)
{
	char buf[256] = {0};
	memcpy(buf,torrent+torrentlen-255,255);
	const char* pbuf = buf;
	const char* pos = NULL;
	while(pbuf - buf < 255)
	{
		pos = strstr(pbuf,HTTPTRACKER);
		if(pos == NULL)
		{
			pbuf += strlen(pbuf)+1;
		}
		else
		{
			return true;
		}
	}
	return false;
}

string ParseHostIp(const char *host)
{
	string strRet;
	if (NULL == host)
	{
		return strRet;
	}
	qvod_sockaddr_t SvrAddr;
	SvrAddr.sin_addr.s_addr = GetHostByName(host);
	strRet = inet_ntoa(SvrAddr.sin_addr);	
	return strRet;
}

//建立客户端socket  
unsigned long GetHostByName(const char* name, bool bDelExist)
{
	unsigned long ip1 = 0;
	CDNSCache::Instance()->Resolve("api.nicefilm.com", ip1);
	if (NULL == name)
	{
		return 0;
	}
	unsigned long ip = 0;        // 这里一定要为下面引用的long类型
	string strName = name;
	string strHostName = name;
	int nPos = strName.find(":");
	if (nPos != string::npos)
	{
		strHostName = strName.substr(0, nPos);
	}
	
	//char* pos = (char*)strstr(name,":");
	//if(pos)
	//{
	//	*pos = '\0';			// 这里导致堆栈问题
	//}
	ip = inet_addr(strHostName.c_str());		
	//if (pos)
	//{
	//	*pos = ':';
	//}	
	if (ip != INADDR_NONE)
	{
		return ip;
	}
	if (bDelExist)
	{
		CDNSCache::Instance()->Remove(strHostName);
	}
	ip = 0;
	CDNSCache::Instance()->Inquire(strName);
	for (int i = 0; i < 500; i++)
	{
		if(CDNSCache::Instance()->Resolve(strName, ip) != CDNSCache::SUCCESS)
		{
			QvodSleep(10);
		}
		else
		{
			if (0 == ip)
			{
#ifdef QVODDEBUG
				_OUTPUT(ERROR1, "DNS parse %s failed!\n", strName.c_str());
#endif // QVODDEBUG
				return 0;
			}
			return ip;
		}
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "DNS parse %s failed!\n", strName.c_str());
#endif
	return 0;
}

int GetIpFromDomain(unsigned long& ip,char* host)
{
	unsigned long iIP = GetHostByName(host);
	if (0 == iIP)
	{
		return -1;
	}
	ip = iIP;
	return 0;
}


void CallBackToUI( int id,int result,const char* buf, int len )
{
	if (g_eventCallBackFunc)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO,"CallBackToUI id = %d,ret = %d hash=[%s]\n",id,result, buf);
#endif
		SCallBackPara para;

		para.id			= id;
		para.result		= result;
		para.buf		= buf;
		para.buflen		= len;

		g_eventCallBackFunc(para);
	}
}
