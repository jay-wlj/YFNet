#ifdef WIN32 /* WIN32 */
#pragma warning(disable:4786)
#else /* posix */
#include <sys/param.h>
#include <sys/mount.h>
#endif /* posix end */

#include <stdio.h>
#include "qvod_core.h"
#include "stun.h"
#include "common-utility.h"
#include "sha1.h"
#include <fcntl.h> /* silver delete */
#include <sys/types.h> /* silver delete */
#include <sys/stat.h> /* silver delete */
//#include <io.h> /* silver delete */
#include <math.h>
#include <time.h> /* silver delete */
#include <set>
#include <vector>
#include <map>
#include <string>

#ifdef WIN32 /* WIN32 */
#include <Sddl.h>
#elif defined MACOSX
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#endif

#include "qvod_file.h"
//#include "upnp.h"
//#include "msgpool-MsgPoolInterface.h"
//#include "taskmgr-TaskMgrInterface.h"
#include "common-inifile.h"
//#include <direct.h> /* silver delete */
//#include "trackerinterface.h"
#include "global.h"
//#include "Terminal-ServerProduct.h"
#include "stdafx.h"
#include "common-commonfun.h"
#include "common-commonstruct.h"
#include "AutoPtr.h"
//#include <Wininet.h>//共享IE临时文件夹的线程需要该头文件 /* silver delete */
//UserID
unsigned int g_iSeedEncKey = 0xFDC27AE2;	//种子加密key
char         g_szSPeerID[24];
unsigned int g_iPeerIDkey = 952780801;

bool          g_bEnableUpload = true;
unsigned char g_protocol;//协议类型
bool          g_isForceUdp; //是否强制采用UDP
unsigned int g_iUpRateLimit;//上传速度限制
unsigned int g_iDownRateLimit;//下载速度限制
std::string   g_media_dir;//媒体文件路径
std::string	  g_strtorrentpath;//种子文件路径
std::string   g_strCurrentRoot;//当前根目录
std::string	  g_strConfigRoot; /* silver add */
std::string  g_strFullFileCopyPath;	//文件下载完整后拷贝的路径
std::string   g_strLogPath;					// 日志目录
std::string g_strhttpurl;
unsigned short g_httplimited;
char g_keycode[20];
bool g_isUpdateThreadRun=false;
bool g_bRunning = true;
qvod_thread_event_t g_UpdateEvent;
pthread_mutex_t		g_UpdateEventLock;

unsigned short       g_httpport = 0;
unsigned int         g_cacheAgentIP;

QVOD_INT64		g_iMaxUseSpace = 0;//最大可用空间，0为无限制，创建任务时判断若超过则删除旧任务
// unsigned char        g_cPeersLimit=35;//用户数限制
// unsigned char        g_cSeedsLimit=100;//种子数限制
// unsigned char        g_cConnectLimit=0;//连接数限制

time_t g_starttime = 0;
//默认参数设置
unsigned char  g_task_PeersLimit = 35;
unsigned char  g_task_SeedsLimit = 100;
unsigned char  g_task_UpRateLimit = 0;
unsigned short g_outdebuginfo = 2;
int            g_iEnableDelTask = 0;//是否允许删除网络任务
int			   g_iMinValue = 0;     //最小进度为多少
int			   g_speedlimit = 0;
int			   g_userSpeedlimit=0;
int			   g_lastSpeedlimit = 0;
bool		   g_bquickly;
int			   g_iKeepTerminal = 0; //Terminal是否退出,0:退出,1:不退出
bool 		   g_bEncodeFile = false;//是否加密保存文件
bool 		   g_bNetworkRun = true; //是否开启网络

unsigned int   g_accBlockSize = 3*1024*1024;
time_t		   g_iDefaultIPLastWriteTime = 0;		//默认IP列表文件最后修改时间	

int            g_iWebInvoke = 0;   //terminal是否由web调起来
QVOD_DWORD		   g_httpLastConnTime = QvodGetTime(); //web最后断开的时间
unsigned int   g_webAccSize = 2*1024*1024;
QVOD_UINT64         g_webCacheSpace = 2147483648;

bool			g_bUseP2p=true;	//是否使用p2p混合下载
bool			g_bIsUpgradeProc = false;	//是否升级程序，升级程序不需要一些模块

//QvodType:pc 21,ios 22,iosb 23,android 24,dapingmu 25,mac 26
#ifdef MONGOTV
unsigned int g_iQvodType = 29;
#define ORIGIN	"MongoTvIos"
unsigned short g_iTerminalProtocol = 152;
#else
unsigned int g_iQvodType = 29;
#define ORIGIN	"NaifeiPC"
unsigned short g_iTerminalProtocol = 152;
#endif	//MONGOTV

unsigned short g_iMaxTerminalVersion = 0;
unsigned short g_iMinTerminalVersion = 0;

#if QVOD_DEBUG_SCREEN
ZConsole* pConsole = NULL;
#endif

#ifdef QVOD_DEBUG_FILE
FILE* debug_file = NULL;
#endif

#ifdef SUPERNODE
bool g_benablewebservice = false;
#endif

//登录成功后,保存用户的isp分组号
unsigned char g_szIspID[5] = {0};



#ifdef KUAIWAN
	std::string g_strExpnameFillter = ".rmvb.rm.rar."; 
#else
	std::string g_strExpnameFillter = ".wav.wmv.asf.rm" \
		".rmvb.mov.avi.mpg.mpeg.mpe.3gp.mp4.flv.wm.rpm.rt.rsc.smi.smil.tim.swf.csf.scm.ts.tp.mkv.dat.vob.qt.ram.qmv" \
		".3gp2.3gpp.acc.aif.amr.aifc.aiff.asx.bik.flc.roq.d2v.dsa.dss.dsv.m1v.f4v" \
		".m2p.m2v.m4b.m4v.mkv.mod.ogm.pmp.pm2.pss.m4p.mv.m2ts.evo.3g2.wmp.wm.ivf" \
		".rmm.mjp.pva.rat.dvd.smk.tpr.vg2.vid.vp6.vp7.amv.fli.pmp2.mts.drc.fsp" \
		".smv.spx.au.flac.ra.m4a.mp2.mpga.mp5.dsm.mpa.mac.dts.mpga.wma.mp3.midi.ac3.aac.ogg.mid.ape.cda.rmi.tta.tta.hlv."; 
#endif

bool        g_bIsRunBuild = false;

bool          g_bDownRateReact = false;
bool          g_bUpRateReact = false;

#pragma obfuscate on

//文件分块大小范围  picsize>=16KB && picsize<= 4MB  picsize=2^n
unsigned int CalcPieceSize(QVOD_INT64 filesize)
{
	if (filesize <= 2097152)		 // 大小小于2M 默认分块大小为16KB
		return 16384;
	float f = sqrt((float)filesize/4096);			// 除以4K的平方根,以下算法为 2^n <= sqrt(filesize/4096), 其中2^n为最大块大小
	unsigned short n = (unsigned short)f;
	int i = 3;
	while(n && i<16)										
	{
		if( n & (0x8000>>i))
			return (0x8000 >> i)*1024;
		i++;
	}
	return 16384;											// (文件大小大于256GB?),块大小超过4M,则默认分块大小为16KB
}

int Build(std::string strSourcePath,std::string strName,std::string strSavePath,std::string strAdUrl,
		  std::string strTracker,std::string strTextAdUrl,std::string strPauseAdUrl)
{
	//检查是否有源文件
	int encodetype = 0;
	char extname[20]={0};
	size_t pos = strName.rfind(".");
	if(pos == -1)
	{
		return -1;
	}
	memcpy(extname,strName.c_str()+pos,QVOD_MIN2(7,strName.size()-pos));
	int j;
	for(j = 0;j <7 ;j++)
	{
		char c = *(extname+j);
		if(c == '!') 
			return -1;
		if(c >= 'A' && c <='Z')
			*(extname+j) = c-'A'+'a';
	}
	strcat(extname,".");
	if(std::string::npos == g_strExpnameFillter.find(extname))//过虑扩展名
	{
		if(strName.size() == 45 && strcmp(extname,".data.") == 0)
		{
			encodetype = 1;
		}
		else if (strName.size() == 46 && strcmp(extname, ".qdata.") == 0)
		{
			encodetype = 2; 
		}
		else
		{
			return -1;
		}
	}

	int fh = QVOD_OPEN((strSourcePath).c_str(),QVOD_O_RDONLY ,QVOD_S_IREAD );
	if (-1 == fh) 
	{
		//源文件不存在，删除种子文件
		remove((strSavePath+".torrent").c_str());
		return -1;
	}
	QVOD_INT64 fileLen = QvodFileLength(fh);
	if(fileLen == 0)
	{
		QVOD_CLOSE(fh);
		return -1;
	}
	QVOD_INT64 LenLimit = 0x7FFFFFFF;
#ifdef SUPERNODE
	LenLimit = 0xFFFFFFFF;
#endif

#ifdef KUAIWAN
	//更改快玩文件大小限制
	LenLimit = 0x0F00FF00FF00FF00;
#endif
	//对大于LenLimit的文件,直接返回错误;
	if ( fileLen >= LenLimit )
	{
		QVOD_CLOSE(fh);
		return -1;
	}
	QVOD_CLOSE(fh);
	FILE* pf = fopen((strSourcePath).c_str(),"rb");
	if (!pf) 
	{
		return -1;	
	}

	int pieceLen = CalcPieceSize(fileLen);
	int pieceNum = (fileLen+pieceLen-1)/pieceLen;
	
	char* buf = new char[pieceLen];
	Piece* pPiece = new Piece[pieceNum];	
	CSHA1 sha;
	QVOD_INT64 readlen=0;
	int nlen = fread(buf,sizeof(char),pieceLen,pf);
	int i=0;
	while (nlen >0)
	{
		if (!g_bIsRunBuild)		//终止时退出循环
		{
			fclose(pf);
			remove((strSavePath+".torrent").c_str());
			return -1;
		}

		if(1 == encodetype)
		{
			int i=0;
			unsigned char hash[20];
			QVOD_UINT key;
			Char2Hash(strName.c_str(),hash);
			memcpy(&key,hash,sizeof(key));
			while(i*16*1024 < nlen)
			{
				unsigned char tmp[16*1024];
				Dec2((QVOD_BYTE*)buf+i*16*1024,tmp,QVOD_MIN2(16*1024,(nlen-i*16*1024)),key);
				memcpy(buf+i*16*1024,tmp,QVOD_MIN2(16*1024,(nlen-i*16*1024)));
				i++;
			}
		}
		else if (2 == encodetype)
		{
			unsigned int i=0;
			unsigned char hash[20];
			QVOD_UINT key;
			Char2Hash(strName.c_str(),hash);
			memcpy(&key,hash,sizeof(key));
			while(i*8*1024 < nlen)
			{
				unsigned char tmp[8*1024];
				Dec2((QVOD_BYTE*)buf+i*8*1024,tmp,QVOD_MIN2(8*1024,(nlen-i*8*1024)),key);
				memcpy(buf+i*8*1024,tmp,QVOD_MIN2(8*1024,(nlen-i*8*1024)));
				i++;
			}
		}

		sha.reset();
		sha.write(buf,nlen);
		sha.read(&pPiece[i]);
		i++;
		readlen += nlen;
		nlen = fread(buf,sizeof(char),pieceLen,pf);
	}
	fclose(pf);

	if(encodetype > 0)
	{
		char hash[20];
		CSHA1 sha;
		sha.write(pPiece,20*pieceNum);
		sha.read(hash);
		if(memcmp(Hash2Char((const unsigned char *)hash).c_str(),strName.c_str(),40) != 0)
		{
			remove(strSourcePath.c_str());
			delete []pPiece;
			delete []buf;
			return -1;
		}
	}

	FILE* ps = fopen((strSavePath+".torrent").c_str(),"wb");
	if(!ps) 
	{
		delete []pPiece;
		delete []buf;
		return -1;
	}

	fprintf(ps,"d8:encoding3:GBK4:infod");
	fprintf(ps,"6:lengthi%llde",fileLen); /* silver, %u --> %lld */
	fprintf(ps,"4:name%d:%s12:piece lengthi%de6:pieces%d:",strName.size(),	strName.c_str(),pieceLen,20*pieceNum);
	fwrite(pPiece,sizeof(char),20*pieceNum ,ps);
	char timebuf[16]={0};
	sprintf(timebuf,"%u",time(NULL));
	std::string strtime(timebuf);
	std::string strtracker = strTracker + "/";
	fprintf(ps, "e8:announce%d:%s5:adurl%d:%s9:textadurl%d:%s10:pauseadurl%d:%s13:creation date%d:%se",
		strtracker.size(),strtracker.c_str(),strAdUrl.size(),strAdUrl.c_str(),strTextAdUrl.size(),strTextAdUrl.c_str(),
		strPauseAdUrl.size(),strPauseAdUrl.c_str(),strtime.size(),strtime.c_str());
	fclose(ps);
	//if(strSavePath != "")
	//	CopyFile((g_strtorrentpath+strSeedName+".torrent").c_str(),(strSavePath+strName+".torrent").c_str(),FALSE);
	delete []pPiece;
	delete []buf;
	return 0;
} 
#pragma obfuscate off

void urlEncode(const char *src, int nLenSrc, char *dest, int& nLenDest)
{
	static char unreserved[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, /* 0123456789 */
			0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* ABCDEFGHIJKLMNO */
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, /* PQRSTUVWXYZ*/
			0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* abcdefghijklmnop */
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, /* qrstuvwxyz */
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		nLenDest = 0;
		for (int i=0;  i<nLenSrc; src++,i++)
		{
			if (unreserved[(unsigned char) *src])
			{
				*dest++ = *src;
				nLenDest++;
			}
			else
			{
				sprintf(dest, "%%%02X", (unsigned char) *src), dest += 3;
				nLenDest+=3;
			}
			*dest = 0;
		}
}

void utf8Decode(const char *src, int nLenSrc, char* dest, int& nLenDest)
{
	unsigned int c = 0;
	unsigned short w= 0;
	const char* s = src;
	char* d = dest;
	nLenDest = 0;

	while(s - src < nLenSrc )
	{
		if (*s != '%')
		{
			*d++ = *s++;
			nLenDest++;
		}
		else
		{
		/*当要表示的内容是7位的时候就用一个字节:0*******第一个0为标志位,剩下的空间正好可以表示ASCII0－127的内容
		当要表示的内容在8到11位的时候就用两个字节:110*****10******第一个字节的110和第二个字节的10为标志位
		当要表示的内容在12到16位的时候就用三个字节:1110*****10******10******和上面一样,第一个字节的1110和第二\三个
		字节的10都是标志位,剩下的空间正好可以表示汉字*/
			sscanf(s, "%%%2X", &c);
			s += 3;

			if(c >= 0xE0)//3char
			{
				w = ((short)(c&0x0F))<<12;
				sscanf(s, "%%%2X", &c);
				s += 3;
				w += ((short)(c&0x3F))<<6;
				sscanf(s, "%%%2X", &c);
				s += 3;
				w += ((short)(c&0x3F));
			}
			else if(c >= 0xC0)
			{
				w = ((short)(c&0x1F))<<6;
				sscanf(s, "%%%2X", &c);
				s += 3;
				w += ((short)(c&0x3F));

			}
			else
			{
				*d++ = c;
				nLenDest ++;
				continue;
			}


#ifdef WIN32 /* WIN32 */

			WideCharToMultiByte(CP_ACP,0,(LPCWSTR)&w,2,d,4,NULL,NULL);

#else /* posix */

			/* UNICODE --> CN-GB */
		/*
			iconv_t cd = iconv_open("CN-GB", "UNICODE");
			if(cd == (iconv_t)-1) {
				QVOD_DEBUG("%s(%d)-%s: iconv_open error: %s\n", __FILE__, __LINE__, __FUNCTION__, strerror(errno));
				return;
			}

			char *in = (char*)&w;
			char *out = d;
			size_t ilen = 2;
			size_t olen = 4;

			size_t size = iconv(cd, &in, &ilen, &out, &olen);
			iconv_close(cd);

			if(size == (size_t)-1) {
				QVOD_DEBUG("iconv error\n");
				return;
			}
		*/
#endif /* posix end */

			d +=2;
			nLenDest +=2;

		}

	}
}

void utf8Decode2(const char *src, int nLenSrc, char* dest, int& nLenDest)
{
	unsigned int c = 0;
	unsigned short w= 0;
	const char* s = src;
	char* d = dest;
	nLenDest = 0;

	while(s - src < nLenSrc )
	{
		if (*s > 0)
		{
			*d++ = *s++;
			nLenDest++;
		}
		else
		{
			c = *s++;

			if(c >= 0xE0)//3char
			{
				w = ((short)(c&0x0F))<<12;
				c = *s++;
				w += ((short)(c&0x3F))<<6;
				c = *s++;
				w += ((short)(c&0x3F));
			}
			else if(c >= 0xC0)
			{
				w = ((short)(c&0x1F))<<6;
				c = *s++;
				w += ((short)(c&0x3F));

			}
			else
			{
				*d++ = c;
				nLenDest ++;
				continue;
			}

#ifdef WIN32 /* WIN32 */

			WideCharToMultiByte(CP_ACP,0,(LPCWSTR)&w,2,d,4,NULL,NULL);

#else /* posix */

			/* UNICODE --> CN-GB */
		/*
			iconv_t cd = iconv_open("CN-GB", "UNICODE");
			if(cd == (iconv_t)-1) {
				QVOD_DEBUG("%s(%d)-%s: iconv_open error: %s\n", __FILE__, __LINE__, __FUNCTION__, strerror(errno));
				return;
			}

			char *in = (char*)&w;
			char *out = d;
			size_t ilen = 2;
			size_t olen = 4;

			size_t size = iconv(cd, &in, &ilen, &out, &olen);
			iconv_close(cd);

			if(size == (size_t)-1) {
				QVOD_DEBUG("%s(%d)-%s: iconv error: %s\n", __FILE__, __LINE__, __FUNCTION__, strerror(errno));
				return;
			}
	*/
#endif /* posix end */

			d +=2;
			nLenDest +=2;

		}

	}
}

void urlDecode(const char *src, int nLenSrc, char* dest, int& nLenDest)
{
	unsigned int c = 0;
	const char* s = src;
	char* d = dest;
	int nMaxLenth = (0 == nLenDest?1024:nLenDest);
	nLenDest = 0;

	while ( s - src < nLenSrc  && nMaxLenth-- >= 0 )
	{
		if (*s != '%')
		{
			//if (*s != '+')
				*d++ = *s++;
			//else
			//{
			//	*d++ = ' ';
			//	s++;
			//}

		}
		else
		{
			sscanf(s, "%%%2X", &c);
			s += 3;
			*d++ = (unsigned char) c;

		}

		nLenDest++;
	}
}

std::string URLDecode(const char* str, const unsigned len)
{
	std::string strRet;
	if ((str==NULL)  || (len<=0))
	{
		return strRet;
	}

	char szTemp[3] = {0};
	for (size_t i = 0; i < len; i++)  
	{  
		if (str[i] == '+') 
		{
			strRet += ' ';  
		}
		else if (str[i] == '%')  
		{  
			if (i+2 < len)
			{
				szTemp[0] = str[++i];
				szTemp[1] = str[++i];

				strRet += (unsigned char)strtol(szTemp, NULL, 16);
			}
		}  
		else
		{
			strRet += str[i];  
		}
	}  
	return strRet; 
}

std::string Hash2Char(const unsigned char* hash, const unsigned int uLen)
{
	char buf[41]={0};
	unsigned int i,j=0;
	unsigned char c;
	for(i=0;i<uLen;)
	{
		c = (hash[i]>>4) & 0x0f;
		if(c > 9)
			buf[j++] = 'A'+ (c - 10);
		else
			buf[j++] = '0' + c;
		c = hash[i] & 0x0f;
		if(c > 9)
			buf[j++] = 'A'+ (c - 10);
		else
			buf[j++] = '0' + c;
		i++;
	}
	return buf;
}

void Char2Hash(const char* chars,unsigned char* hash, const unsigned int nLen)
{
	unsigned int i ,j=0;
	unsigned char c;

	const unsigned int len = nLen*2;
	if (chars && strlen(chars) < len)
	{
		return;
	}

	for (i=0;i<len;)
	{
		if('9'>= chars[i]&& chars[i]>= '0' )
			c = (chars[i] - '0')<< 4;
		else if('F'>= chars[i]&& chars[i]>= 'A' )
			c = (chars[i] - 'A'+10)<< 4;
		else if('f'>= chars[i]&& chars[i]>= 'a')
			c = (chars[i] - 'a'+10)<< 4;
		else
			return ;
		i++;
		if('9'>= chars[i]&& chars[i]>= '0' )
			c += (chars[i] - '0');
		else if('F'>= chars[i]&& chars[i]>= 'A' )
			c += (chars[i] - 'A'+10);
		else if('f'>= chars[i]&& chars[i]>= 'a')
			c += (chars[i] - 'a'+10);
		else
			return ;
		i++;
		hash[j]= c;
		j++;
	}
}

bool GetHttpUrl(const char* url,const char* savefilename)
{
	char buf[102400];
	char tmpname[256] = "";
	char* pbuf = buf;
	char* dataoffset = NULL;
	int datalen = 0;
	int filelen = 0;
	int recvlen = 0;

	int nMs = 10000;

	url += strlen("http://");
	QvodGetTempPath(256,tmpname);
#ifdef WIN32
	const char* pname = strrchr(savefilename,'\\');
#else
	const char* pname = strrchr(savefilename,'/');
#endif
	if(pname)
		strcat(tmpname,pname+1);
	else
		strcat(tmpname,savefilename);

	const char* fmtget =  "GET /%s HTTP/1.1\r\nAccept: application nd.ms-powerpoint, application/msword, */*\r\n" \
	"Accept-Language: zh-cn\r\n" \
	"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n" \
	"Host: %s\r\n" \
	"Connection: Keep-Alive\r\n\r\n";
	const char* purl = strstr(url,"/");
	char host[32] = {0};
	if(!purl)
	{
		pbuf += sprintf(pbuf,fmtget,"",url);
		strcpy(host,url);
	}
	else
	{
		memcpy(host,url,purl-url);
		purl++;
		pbuf += sprintf(pbuf,fmtget,purl,host);
	}
	QVOD_UINT remoteip;
	QVOD_USHORT remoteport;
	std::string strurl = "http://";
	strurl += host;
	if( 0 != GetAddrFromHTTP(strurl,remoteip,remoteport))
	{
		return false;
	}
	qvod_sock_t s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	QvodSetSockTimeout(s, SOL_SOCKET, SO_RCVTIMEO, nMs);

	qvod_sockaddr_t addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = remoteip;
	addr.sin_port = remoteport;
	if(0 != connect(s,(sockaddr*)&addr,sizeof(addr)))
	{
		QvodCloseSocket(s);
		return false;
	}
	datalen = send(s,buf,pbuf-buf,0);
	if(datalen <= 0)
	{
		QvodCloseSocket(s);
		return false;
	}
	pbuf = buf;
	datalen = recv(s,buf,1024*16,0);
	if(datalen <= 0)
	{
		QvodCloseSocket(s);
		return false;
	}
	dataoffset = strstr(buf,"\r\n\r\n");
	if(!dataoffset)
	{
		QvodCloseSocket(s);
		return false;
	}
	*dataoffset = 0;
	dataoffset += 4;
	if(!strstr(buf,"200 OK"))
	{
		QvodCloseSocket(s);
		return false;
	}
	pbuf = strstr(buf,"Content-Length: ");
	if(!pbuf)
	{
		FILE* pf = fopen(tmpname,"w+b");
		if(!pf)
		{
#ifdef QVODDEBUG
			_OUTPUT(FAIL, "GetHttpUrl open file1 %s fail: %d\n", tmpname, errno);
#endif // QVODDEBUG
			QvodCloseSocket(s);
			return false;
		}
		recvlen = datalen - (dataoffset - buf);
		fwrite(dataoffset,sizeof(char),recvlen,pf);
		datalen = recv(s,buf,1024*16,0);
		while(datalen > 0)
		{
			fwrite(buf,sizeof(char),datalen,pf);
			datalen = recv(s,buf,1024*16,0);
		}
		fclose(pf);
	}
	else
	{
		pbuf += strlen("Content-Length: ");
		filelen = atoi(pbuf);
		recvlen = datalen - (dataoffset - buf);
		FILE* pf = fopen(tmpname,"w+b");
		if(!pf)
		{
#ifdef QVODDEBUG
			_OUTPUT(FAIL, "GetHttpUrl open file2 %s fail: %d\n", tmpname, errno);
#endif // QVODDEBUG
			QvodCloseSocket(s);
			return false;
		}
		fwrite(dataoffset,sizeof(char),recvlen,pf);
		while(recvlen < filelen)
		{
			datalen = recv(s,buf,1024*16,0);
			if(datalen <= 0)
			{
#ifdef QVODDEBUG
				_OUTPUT(FAIL, "GetHttpUrl recv datalen %d fail: %d\n", datalen, errno);
#endif // QVODDEBUG
				QvodCloseSocket(s);
				return false;
			}
			fwrite(buf,sizeof(char),datalen,pf);
			recvlen += datalen;
		}
		fclose(pf);
	}
	QvodCloseSocket(s);
	remove(savefilename);
	if(QvodMoveFile(tmpname,savefilename) == QVOD_OK)
	{
		return true;
	}
#ifdef QVODDEBUG
	_OUTPUT(FAIL, "GetHttpUrl QvodMoveFile fail %d: from %s to %s\n", errno, tmpname, savefilename);
#endif // QVODDEBUG
	return false;
}

bool GetHttpUrl(const char* url,std::string& strRet)
{
	char buf[102400];
	char tmpname[256] = "";
	char* pbuf = buf;
	char* dataoffset = NULL;
	int datalen = 0;
	int filelen = 0;
	int recvlen = 0;

	int nMs = 10000;

	url += strlen("http://");

	const char* fmtget =  "GET /%s HTTP/1.1\r\nAccept: application nd.ms-powerpoint, application/msword, */*\r\n" \
	"Accept-Language: zh-cn\r\n" \
	"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n" \
	"Host: %s\r\n" \
	"Connection: Keep-Alive\r\n\r\n";
	const char* purl = strstr(url,"/");
	char host[32] = {0};
	if(!purl)
	{
		pbuf += sprintf(pbuf,fmtget,"",url);
		strcpy(host,url);
	}
	else
	{
		memcpy(host,url,purl-url);
		purl++;
		pbuf += sprintf(pbuf,fmtget,purl,host);
	}
	QVOD_UINT remoteip;
	QVOD_USHORT remoteport;
	std::string strurl = "http://";
	strurl += host;
	if( 0 != GetAddrFromHTTP(strurl,remoteip,remoteport))
	{
		return false;
	}
	qvod_sock_t s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	QvodSetSockTimeout(s, SOL_SOCKET, SO_RCVTIMEO, nMs);

	qvod_sockaddr_t addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = remoteip;
	addr.sin_port = remoteport;
	if(0 != connect(s,(sockaddr*)&addr,sizeof(addr)))
	{
		QvodCloseSocket(s);
		return false;
	}
	datalen = send(s,buf,pbuf-buf,0);
	if(datalen <= 0)
	{
		QvodCloseSocket(s);
		return false;
	}
	pbuf = buf;
	datalen = recv(s,buf,1024*16,0);
	if(datalen <= 0)
	{
		QvodCloseSocket(s);
		return false;
	}
	dataoffset = strstr(buf,"\r\n\r\n");
	if(!dataoffset)
	{
		QvodCloseSocket(s);
		return false;
	}
	*dataoffset = 0;
	dataoffset += 4;
	if(!strstr(buf,"200 OK"))
	{
		QvodCloseSocket(s);
		return false;
	}
	pbuf = strstr(buf,"Content-Length: ");
	if(!pbuf)
	{
		
		bool bChunked = (NULL != strstr(buf, "Transfer-Encoding: chunked"));
		bChunked |= (NULL != strstr(buf, "transfer-encoding: chunked"));

		if (bChunked)
		{
			std::string strInfo;

			recvlen = datalen - (dataoffset - buf);
			strInfo.append(dataoffset, recvlen);

			while (! strstr(dataoffset, "0\r\n\r\n"))
			{
				datalen = recv(s,buf,1024*16,0);
				if(datalen <= 0)
				{
	#ifdef QVODDEBUG
					_OUTPUT(FAIL, "GetHttpUrl recv datalen %d fail: %d\n", datalen, errno);
	#endif // QVODDEBUG
					QvodCloseSocket(s);
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

					strRet.append(dataoffset, len);
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
			strRet.append(dataoffset, recvlen);
			datalen = recv(s,buf,1024*16,0);
			while(datalen > 0)
			{
				strRet.append(buf, datalen);
				datalen = recv(s,buf,1024*16,0);
			}
		}
	}
	else
	{
		pbuf += strlen("Content-Length: ");
		filelen = atoi(pbuf);
		recvlen = datalen - (dataoffset - buf);
		strRet.append(dataoffset, recvlen);
		while(recvlen < filelen)
		{
			datalen = recv(s,buf,1024*16,0);
			if(datalen <= 0)
			{
#ifdef QVODDEBUG
				_OUTPUT(FAIL, "GetHttpUrl recv datalen %d fail: %d\n", datalen, errno);
#endif // QVODDEBUG
				QvodCloseSocket(s);
				return false;
			}
			strRet.append(buf, datalen);
			recvlen += datalen;
		}
	}
	QvodCloseSocket(s);

#ifdef QVODDEBUG
				_OUTPUT(FAIL, "GetHttpUrl ok '%s'\n", url);
#endif // QVODDEBUG
	return true;
}

bool loadadmoviecfg(std::string strcfgfile,std::map<std::string,std::string>& maphash)
{
	FILE* pf = fopen(strcfgfile.c_str(),"rt");
	if(!pf)
		return false;
	//QVOD_BOOL result = QVOD_TRUE; /* silver delete, no use */
	int len = 10240;
	char buf[10240];
	char* pbuf = buf;
	int filelen = fread(buf,sizeof(char),len,pf);
	buf[filelen] = '\0';//字符串末尾置0
	char* left;
	char* right;
	char* str = GetLine_(pbuf);
	while(*pbuf || *str)
	{
		if(*str)
		{
			char* pos = strstr(str,"=");
			if(pos)
			{
				left = str;
				pos[0] = 0;
				right = pos+1;

				//去掉空格
				while(*(left) == ' ')
					*left++ = 0;
				while(*(--pos) == ' ')
					*pos = 0;
				while(*(right) == ' ')
					*right++ = 0;
				pos = pbuf - 1;
				while(*(--pos) == ' ')
					*pos = 0;

				if(strlen(left))
				{
					maphash[left]= right;
				}
				else
				{
					break;
				}
			}
		}
		str = GetLine_(pbuf);
	}

	fclose(pf);
	return true;
}

std::string GetKeyCode(char* hash)
{
	CSHA1 sha;
	unsigned char tmphash[20] = {0};
	sha.write(hash+15, 15);
	sha.read(tmphash);
	return Hash2Char(tmphash).substr(0, 5);
}

std::string GetPwdCode(char* hash, QVOD_UINT64 pos, char* date)
{
	CSHA1 sha1;
	char strpos[30] = {0};
	sprintf(strpos, "%llu", pos);
	unsigned char tmphash[20] = {0};
	sha1.write(hash+15, 15);
	sha1.write(date, strlen(date));
	sha1.write(strpos, strlen(strpos));
	sha1.read(tmphash);
	CSHA1 sha2;
	std::string strtmphash = Hash2Char(tmphash).substr(11, 10);
	sha2.write(strtmphash.c_str(), strtmphash.size());
	std::string key = GetKeyCode(hash);
	sha2.write(key.c_str(), key.size());
	sha2.read(tmphash);
	return Hash2Char(tmphash).substr(11, 16);
}

#ifdef MACOSX
bool GetMacAddr(char* mac)
{
	int                    mib[6];
	size_t                len;
	unsigned char        *ptr;
	struct if_msghdr    *ifm;
	struct sockaddr_dl    *sdl;
	
	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_LINK;
	mib[4] = NET_RT_IFLIST;
	
	if ((mib[5] = if_nametoindex("en0")) == 0)
    {
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "GetMacAddr Error: if_nametoindex error %d\n", errno);
#endif
		return false;
	}
	
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0)
    {
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "GetMacAddr Error: sysctl, take 1 error %d\n", errno);
#endif
		return false;
	}
	if (len <= 0)
	{
		return false;
	}
	QPtr::AutoArrayPtr<char> buf = new char[len];
	if (buf == NULL)
    {
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "GetMacAddr Could not allocate memory. error %d\n", errno);
#endif
		return false;
	}
	
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0)
    {
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "GetMacAddr Error: sysctl, take 2 error %d\n", errno);
#endif
		return false;
	}
	
	ifm = (struct if_msghdr *)(char*)buf;
	sdl = (struct sockaddr_dl *)(ifm + 1);
	ptr = (unsigned char *)LLADDR(sdl);
	sprintf(mac, "%02X%02X%02X%02X%02X%02X", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));
    return true;
}
#endif

bool IsDiskSpaceEnough(char chDisk,QVOD_UINT64 MediaSize)
{
#ifdef WIN32 /* WIN32 */

	char path[MAX_PATH] = {0};
	char folder[4]="C:\\";
	folder[0] = chDisk;
	QVOD_UINT64 minspace = 0;
	GetSystemDirectory(path,MAX_PATH);
	ULARGE_INTEGER i64FreeBytesToCaller,i64TotalBytes,i64FreeBytes;
	BOOL fResult = FALSE;

	if(path[0] == folder[0])	//如果所给的磁盘为系统盘,预留空间为2G
	{
		minspace = 0x80000000+MediaSize;
	}
	else
	{
		minspace = 512*1024*1024+MediaSize;		//非系统盘预留空间为512M
	}

	fResult = GetDiskFreeSpaceEx(folder,
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);
	if(fResult)
	{
		if(i64FreeBytes.QuadPart > minspace)
		{
			return true;
		}
	}
	return false;

#else /* posix */

	/* query "/proc/mounts" */
	//char maxdisk = 'R';
	//QVOD_INT64 minspace = 0;
	FILE *fp;
	char text[256];
	long res;

	char file_dev[32];
	char mount_on[64];
	char file_system[32];
	char permission[128];
	char num1[4];
	char num2[4];

	fp = fopen("/proc/mounts", "r");
	if(fp == NULL) {
		/* error */
#ifdef QVODDEBUG
		_OUTPUT(INFO, "%s(%d)-%s: fopen() failed", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
		return 0;
	}

	/* read line */
	while(fgets(text, 256, fp)) {

		sscanf(text, "%s %s %s %s %s %s",
					 file_dev, mount_on, file_system, permission, num1, num2); /* string add '\0' automatically */

		/* check mount_on */
		if(*mount_on == '/' && *(mount_on + 1) == '\0' && *file_dev == 'r') {
			/* rootfs, system disk, free space should be > 2G */
#ifdef QVODDEBUG
			_OUTPUT(INFO,"IsDiskSpaceEnough find rootfs '/', dev: %s\n", file_dev);
#endif // QVODDEBUG
		/*
			struct statfs64 stat;
			res = statfs64(mount_on, &stat);
			if(res != 0) {
				
#ifdef QVODDEBUG
				_OUTPUT(INFO,"%s(%d)-%s: statfs64() failed", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
				fclose(fp);
				return false;
			}

			if((QVOD_INT64)stat.f_bfree * (QVOD_INT64)stat.f_bsize > (QVOD_INT64)0x80000000 + (QVOD_INT64)MediaSize) {
				fclose(fp);
				return true;
			}
	*/
		} else {
			/* other disk, free space should be > 512M */

			char *pos =	strcasestr(mount_on, "/mnt/");
			if(pos != NULL &&
			   *(pos + sizeof("/mnt/")) == '\0' &&
			   *(pos + sizeof("/mnt/") - 1) > 64 &&
			   *(pos + sizeof("/mnt/") - 1) < 91) {
				/* example: /mnt/D */
#ifdef QVODDEBUG
				_OUTPUT(INFO,"find /mnt/: %s\n", pos);
#endif // QVODDEBUG
			/*
				struct statfs64 stat;
				res = statfs64(pos, &stat);
				if(res != 0) {
					
#ifdef QVODDEBUG
					_OUTPUT(INFO,"%s(%d)-%s: statfs64() failed", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
					fclose(fp);
					return false;
				}

				if((QVOD_INT64)stat.f_bfree * (QVOD_INT64)stat.f_bsize > (QVOD_INT64)(512*1024*1024) + (QVOD_INT64)MediaSize) {
					fclose(fp);
					return true;
				}
			*/
			}
		}
	} /* while */

	fclose(fp);
	return false;

#endif /* posix end */
}

bool  GetSavePath(std::string& strPath,QVOD_INT64 iFileLen)
{
#ifdef WIN32 /* WIN32 */
	char drive[4] = {0};
	char disk = g_media_dir[0];
	sprintf(drive,"%c:\\",disk);
	ULARGE_INTEGER i64FreeBytesToCaller;
	ULARGE_INTEGER i64TotalBytes;
	ULARGE_INTEGER i64FreeBytes;
	BOOL fResult = GetDiskFreeSpaceEx(drive,
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);
	if ( !fResult )
	{
		return false;
	}
	char szSysPath[MAX_PATH] = {0};
	GetSystemDirectory(szSysPath,MAX_PATH);
	if ( !( disk != szSysPath[0] && (QVOD_INT64)i64FreeBytes.QuadPart >  0x20000000 + iFileLen) &&
		!( disk == szSysPath[0] && (QVOD_INT64)i64FreeBytes.QuadPart >  0x80000000 + iFileLen))
	{
		disk = GetNextFreeDisk(iFileLen);
		if ( 0 == disk )
		{
			return false;
		}
		strPath = disk;
		strPath += ":\\Media\\";
		_mkdir(strPath.c_str());
	}
	return true;

#else /* posix */

#ifdef MACOSX
	struct statfs pstat;
	int res = statfs(g_media_dir.c_str(), &pstat);
	if(res != 0) 
	{
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "%s(%d)-%s: statfs() failed", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
		return false;
	}
	QVOD_INT64 freespace = (QVOD_INT64)pstat.f_bfree * (QVOD_INT64)pstat.f_bsize;
#ifdef QVODDEBUG
	_OUTPUT(INFO, "space %lld, %s\n", freespace, g_media_dir.c_str());
#endif // QVODDEBUG
	if(freespace > iFileLen)
	{
		strPath = g_media_dir;
		return true;
	}
#endif
	/* this func is not for cache, only for client */
	return false;

#endif /* posix end */
}

/************************************************************************/
/* 获得处系统盘以外的最空磁盘空间
/************************************************************************/
char GetMaxDisk()
{
#ifdef WIN32 /* WIN32 */

	char drive[4] = {0};
	char tmpDisk = 'C';
	char locDisk = 'C';

	char systemDir[MAX_PATH] = {0};
	GetSystemDirectory(systemDir,MAX_PATH);
	char sysDisk = systemDir[0];

	ULARGE_INTEGER i64FreeBytesToCaller;
	ULARGE_INTEGER i64TotalBytes;
	ULARGE_INTEGER i64FreeBytes;
	ULARGE_INTEGER localBytes ;
	memset(&localBytes,0,sizeof(ULARGE_INTEGER));

	QVOD_DWORD dwRtn = GetLogicalDrives();
	dwRtn >>= 2;//从C盘起

	while(dwRtn)
	{
		if (dwRtn&1 && tmpDisk!=sysDisk)
		{
			sprintf_s(drive,4,"%C:\\",tmpDisk);
			if ( GetDriveType(drive) == DRIVE_FIXED)
			{
				BOOL fResult = GetDiskFreeSpaceEx(drive,
					(PULARGE_INTEGER)&i64FreeBytesToCaller,
					(PULARGE_INTEGER)&i64TotalBytes,
					(PULARGE_INTEGER)&i64FreeBytes);
				if ( fResult  )
				{
					if ( i64FreeBytes.QuadPart > localBytes.QuadPart)
					{
						localBytes = i64FreeBytes;
						locDisk    = tmpDisk;
					}
				}
			}
		}
		tmpDisk++;
		dwRtn >>= 1;
	}
	return locDisk;

#else /* posix */

	/* query "/proc/mounts" */
	char maxdisk = 'R';
	QVOD_INT64 maxdisksize = 0;
	FILE *fp;
	char text[256];
	long res;

	char file_dev[32];
	char mount_on[64];
	char file_system[32];
	char permission[128];
	char num1[4];
	char num2[4];

	fp = fopen("/proc/mounts", "r");
	if(fp == NULL) {
		/* error */
#ifdef QVODDEBUG
		_OUTPUT(INFO,"%s(%d)-%s: fopen() failed", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
		return 0;
	}

	/* read line */
	while(fgets(text, 256, fp)) {

		sscanf(text, "%s %s %s %s %s %s",
					 file_dev, mount_on, file_system, permission, num1, num2); /* string add '\0' automatically */

		/* check mount_on */
		if(*mount_on == '/' && *(mount_on + 1) == '\0' && *file_dev == 'r') {
			/* rootfs */
#ifdef QVODDEBUG
			_OUTPUT(INFO,"GetMaxDisk find rootfs '/', dev: %s\n", file_dev);
#endif // QVODDEBUG
		
			struct statfs stat;
			res = statfs(mount_on, &stat);
			if(res != 0) {
				
#ifdef QVODDEBUG
				_OUTPUT(INFO,"%s(%d)-%s: statfs64() failed", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
				fclose(fp);
				return 0;
			}

			if((QVOD_INT64)stat.f_bfree * (QVOD_INT64)stat.f_bsize > maxdisksize) {
				maxdisk = 'R';
				maxdisksize = (QVOD_INT64)stat.f_bfree * (QVOD_INT64)stat.f_bsize;
			}
		
		} else {

			char *pos =	strcasestr(mount_on, "/mnt/");
			if(pos != NULL &&
			   *(pos + sizeof("/mnt/")) == '\0' &&
			   *(pos + sizeof("/mnt/") - 1) > 64 &&
			   *(pos + sizeof("/mnt/") - 1) < 91) {
				/* example: /mnt/D */
#ifdef QVODDEBUG
				_OUTPUT(INFO,"find /mnt/: %s\n", pos);
#endif // QVODDEBUG
		
				struct statfs stat;
				res = statfs(pos, &stat);
				if(res != 0) {
					
#ifdef QVODDEBUG
				_OUTPUT(INFO,"%s(%d)-%s: statfs64() failed", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
					
					fclose(fp);
					return 0;
				}

				if((QVOD_INT64)stat.f_bfree * (QVOD_INT64)stat.f_bsize > maxdisksize) {
					maxdisk = *(pos + sizeof("/mnt/") - 1);
					maxdisksize = (QVOD_INT64)stat.f_bfree * (QVOD_INT64)stat.f_bsize;
				}
			
			}
		}
	} /* while */

	fclose(fp);

	return maxdisk;

#endif /* posix end */
}

/************************************************************************/
/* 获得空间大于freespace的磁盘,
/* 返回'\0'表示当前没有任何磁盘空间比freespace大
/************************************************************************/
char GetMaxDisk(QVOD_UINT64& freespace)
{
#ifdef WIN32 /* WIN32 */

	char drive[4] = {0};
	char tmpDisk = 'C';
	char locDisk = 'C';

	char systemDir[MAX_PATH] = {0};
	GetSystemDirectory(systemDir,MAX_PATH);
	char sysDisk = systemDir[0];
	freespace = 0;

	ULARGE_INTEGER i64FreeBytesToCaller;
	ULARGE_INTEGER i64TotalBytes;
	ULARGE_INTEGER i64FreeBytes;

	QVOD_DWORD dwRtn = GetLogicalDrives();
	dwRtn >>= 2;//从C盘起

	while(dwRtn)
	{
		if (dwRtn&1 && tmpDisk!=sysDisk)
		{
			sprintf_s(drive,4,"%C:\\",tmpDisk);
			if ( GetDriveType(drive) == DRIVE_FIXED)
			{
				BOOL fResult = GetDiskFreeSpaceEx(drive,
					(PULARGE_INTEGER)&i64FreeBytesToCaller,
					(PULARGE_INTEGER)&i64TotalBytes,
					(PULARGE_INTEGER)&i64FreeBytes);
				if ( fResult  )
				{
					if ( i64FreeBytes.QuadPart > freespace)
					{
						freespace = i64FreeBytes.QuadPart;
						locDisk    = tmpDisk;
					}
				}
			}
		}
		tmpDisk++;
		dwRtn >>= 1;
	}
	return locDisk;

#else /* posix */

	/* query "/proc/mounts" */
	char maxdisk = 'R';
	QVOD_INT64 maxdisksize = 0;
	FILE *fp;
	char text[256];
	int res;

	char file_dev[32];
	char mount_on[64];
	char file_system[32];
	char permission[128];
	char num1[4];
	char num2[4];

	fp = fopen("/proc/mounts", "r");
	if(fp == NULL) {
		/* error */
		
#ifdef QVODDEBUG
		_OUTPUT(INFO,"%s(%d)-%s: fopen() failed", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
		return 0;
	}

	/* read line */
	while(fgets(text, 256, fp)) {

		sscanf(text, "%s %s %s %s %s %s",
					 file_dev, mount_on, file_system, permission, num1, num2); /* string add '\0' automatically */

		/* check mount_on */
		if(*mount_on == '/' && *(mount_on + 1) == '\0' && *file_dev == 'r') {
			/* rootfs */
			
#ifdef QVODDEBUG
			_OUTPUT(INFO,"GetMaxDisk find rootfs '/', dev: %s\n", file_dev);	
#endif // QVODDEBUG
		} else {

			char *pos =	strcasestr(mount_on, "/mnt/");
			if(pos != NULL &&
			   *(pos + sizeof("/mnt/")) == '\0' &&
			   *(pos + sizeof("/mnt/") - 1) > 64 &&
			   *(pos + sizeof("/mnt/") - 1) < 91) {
				/* example: /mnt/D */
#ifdef QVODDEBUG
				_OUTPUT(INFO,"find /mnt/: %s\n", pos);
#endif // QVODDEBUG		
			}
		}
	} /* while */

	fclose(fp);

	freespace = maxdisksize;
	return maxdisk;

#endif /* posix end */
}

char GetNextFreeDisk(QVOD_INT64 iFileLen)
{
#ifdef WIN32 /* WIN32 */

	char drive[4] = {0};
	char disk = g_media_dir[0];
	disk = toupper(disk);
	char SysPath[MAX_PATH] = {0};
	QVOD_DWORD iLogicalDriverMask = 0;
	GetSystemDirectory(SysPath,MAX_PATH);
	iLogicalDriverMask = GetLogicalDrives();

	ULARGE_INTEGER i64FreeBytesToCaller;
	ULARGE_INTEGER i64TotalBytes;
	ULARGE_INTEGER i64FreeBytes;
	ULARGE_INTEGER localBytes ;
	memset(&i64FreeBytesToCaller,0,sizeof(ULARGE_INTEGER));
	memset(&i64TotalBytes,0,sizeof(ULARGE_INTEGER));
	memset(&i64FreeBytes,0,sizeof(ULARGE_INTEGER));
	memset(&localBytes,0,sizeof(ULARGE_INTEGER));

	QVOD_INT64 MinSpace = 0;
	QVOD_DWORD iLoop = iLogicalDriverMask;
	if((iLoop >> ( disk - 'A' +1) )  != 0)//当前设定的盘符后面还有别的盘符
	{
		iLoop = iLoop >> (disk - 'A' + 1);
		do
		{
			sprintf_s(drive,4,"%C:\\",++disk);
			if ( DRIVE_FIXED == GetDriveType(drive) )
			{
				if ( drive[0] == SysPath[0] )
				{
					MinSpace = 0x80000000 + iFileLen;
				}
				else
				{
					MinSpace = 0x20000000 + iFileLen;
				}
				GetDiskFreeSpaceEx(drive,
					(PULARGE_INTEGER)&i64FreeBytesToCaller,
					(PULARGE_INTEGER)&i64TotalBytes,
					(PULARGE_INTEGER)&i64FreeBytes);
				 if ( (QVOD_INT64)i64FreeBytes.QuadPart > MinSpace )
				 {
					 return drive[0];
				 }
			}
			iLoop = iLoop>>1;
		}while( iLoop);
	}

	//当前设定的盘符后面没有其他逻辑盘了,或者其他逻辑盘容量也不符合条件
	//从C盘开始,再次遍历除系统盘外的其他盘,找到第一个符合条件的磁盘.
	iLoop = iLogicalDriverMask;
	iLoop = iLoop >> 2;
	disk = 'C';
	MinSpace = 0x20000000 + iFileLen;
	while(iLoop)
	{
		sprintf_s(drive,4,"%C:\\",disk++);
		if(DRIVE_FIXED == GetDriveType(drive) && drive[0] != SysPath[0])
		{
			//除系统盘外的所有盘容量都不够了,退出循环
			if(drive[0] == g_media_dir[0])
			{
				break;
			}
			GetDiskFreeSpaceEx(drive,
				(PULARGE_INTEGER)&i64FreeBytesToCaller,
				(PULARGE_INTEGER)&i64TotalBytes,
				(PULARGE_INTEGER)&i64FreeBytes);
			if((QVOD_INT64)i64FreeBytes.QuadPart > MinSpace)
			{
				return drive[0];
			}
		}
		iLoop = iLoop>>1;
	}

	//只剩下系统盘了,其他盘都不符合条件
	//判断系统盘容量是否大于2G
	sprintf_s(drive,4,"%C:\\",SysPath[0]);
	MinSpace = 0x80000000 + iFileLen;
	GetDiskFreeSpaceEx(drive,
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);
	if((QVOD_INT64)i64FreeBytes.QuadPart > MinSpace)
	{
		return drive[0];
	}
	return 0;

#else /* posix */

	/* posix do not use this function */
	return 0;

#endif /* posix end */
}

void ProgramQuit()
{
#ifdef WIN32 /* WIN32 */

#if SUPERNODE
	exit(1);
#else
	//PostMessage(g_hWnd,WM_DESTROY,0,0);
#endif

#else /* posix */

	//nothing

#endif /* posix end */
}

//全角字符的第一个字节总是被置为163，而第二个字节则是相同半角字符码加上128（不包括空格）
void ReviseFileName(std::string &strFile)
{
	strFile.replace(strFile.begin(),strFile.end(),'?',' ');
	strFile.replace(strFile.begin(),strFile.end(),'/',' ');
	strFile.replace(strFile.begin(),strFile.end(),'\\',' ');
	strFile.replace(strFile.begin(),strFile.end(),'|',' ');
	strFile.replace(strFile.begin(),strFile.end(),':',' ');
	strFile.replace(strFile.begin(),strFile.end(),'*',' ');
	strFile.replace(strFile.begin(),strFile.end(),'<',' ');
	strFile.replace(strFile.begin(),strFile.end(),'>',' ');
	strFile.replace(strFile.begin(),strFile.end(),'\"',' ');
}

bool isQvodLink(char* req,char* boundary)
{
	char* pos = strstr(req,boundary);
	if(pos)
	{
		pos += 1;
		char* _pos = strstr(pos,boundary);
		if(_pos)
		{
			_pos += 1;
			char* __pos = strstr(_pos,boundary);
			if(__pos)
			{
				if((__pos - _pos) == 40)
				{
					return true;
				}
			}
		}
	}
	return false;
}

char *to_lower(char *pstr)
{
	if ( pstr == NULL )
	{
		return NULL;
	}
	char *p = pstr;
	while ( *p != 0 )
	{
		if ( *p >= 'A' && *p <= 'Z' )
		{
			(*p) = (*p) + 32; 
		}
		p++;
	}
	return pstr;
}

bool IsQhttpTask(const char *pstrSrcUrl)
{
	if (pstrSrcUrl == NULL)
	{
		return false;
	}
	std::string strUrl = pstrSrcUrl;
	strUrl = to_lower((char *)pstrSrcUrl);
	std::string strHead = "http://";
	size_t iPosHead = strUrl.find(strHead);
	if ( iPosHead == std::string::npos )
	{
		return false;
	}
	iPosHead += strHead.size();
	iPosHead++;
	size_t iPosFileLen = strUrl.find('/',iPosHead);
	if ( std::string::npos == iPosFileLen )
	{
		return false;
	}
	iPosFileLen ++;
	size_t iPosHash = strUrl.find('/',iPosFileLen);
	if ( std::string::npos == iPosHash )
	{
		return false;
	}
	iPosHash ++;
	size_t iPosFileName = strUrl.find('/',iPosHash);
	if ( std::string::npos == iPosFileName )
	{
		return false;
	}
	if ( iPosFileName - iPosHash != 40 )
	{
		return false;
	}
	return true;
}

std::string GetPathSuffix(const char* pstrSrcUrl)
{
	string strRet;
	if (NULL == pstrSrcUrl)
	{
		return strRet;
	}
	const char* pstr = strrchr(pstrSrcUrl, '.');
	if (pstr)
	{
		strRet = pstr;
	}
	int npos = strRet.find('?');	// 去掉后缀名后的参数
	if (npos != string::npos)
	{
		strRet = strRet.substr(0, npos);
	}

	return strRet;
}

char* GetLine_(char*& pstr)
{
	char* str;
	char* ptail = strstr(pstr,"\n");
	if(!ptail)
	{
		str = pstr;
		pstr += strlen(pstr);
	}
	else
	{
		ptail[0] = '\0';
		str = pstr;
		pstr = ptail+1;
	}
	return str;
}

string GetLine_(const char* &pstr)
{
	string str;
	const char* ptail = strstr(pstr,"\n");
	if(!ptail)
	{
		str = pstr;
		pstr += strlen(pstr);
	}
	else
	{
		str.assign(pstr, ptail-pstr);
		pstr = ptail+1;
	}
	return str;
}


int GetAddrFromHost(const char* buf,unsigned int& ip,unsigned short& port)
{
	if(strstr(buf,"HTTP/1."))
	{
		const char* pos = strstr(buf,"Host:");
		if(pos)
		{
			pos += 6;
			char host[32] = {0};
			memcpy(host,pos,strstr(pos,"\r\n")-pos);
			char* _pos = strstr(host,":");
			if(_pos)
			{
				port = atoi(_pos+1);
				*_pos = '\0';
			}
			else
			{
				port = 80;
			}
			hostent* phost = gethostbyname(host);
			if (!phost)
			{
				return -1;
			}
			port = htons(port);
			memcpy(&ip,phost->h_addr_list[0],phost->h_length);
			return 0;
		}
	}
	return -1;
}

int GetAddrFromHttp(std::string& HTTPaddr,unsigned int& ip,unsigned short& port)
{
	int seppos;
	std::string strip;
	std::string strport;
	if ((seppos = HTTPaddr.find("http://")) != std::string::npos)//a: from ">0" to "!=npos"
	{
		HTTPaddr = HTTPaddr.substr(seppos+7,HTTPaddr.size() - 7);
	}
	else
	{
		return -1;
	}
	seppos = HTTPaddr.find("/");
	if (seppos != std::string::npos)//a: from ">0" to "!=npos"
		HTTPaddr = HTTPaddr.substr(0,seppos);
	seppos= HTTPaddr.rfind(":",HTTPaddr.size());
	if (seppos == -1)
	{
		port = htons(80);
		strip = HTTPaddr;
	}
	else
	{
		strip = HTTPaddr.substr(0,seppos);
		strport = HTTPaddr.substr(seppos+1,HTTPaddr.size() - seppos-1);
		port = htons(atoi(strport.c_str()));
	}
	hostent* phost = gethostbyname(strip.c_str());
	if (!phost)
	{
		return -1;
	}
	memcpy(&ip,phost->h_addr_list[0],phost->h_length);
	return 0;
}

int GetAddrFromHTTP(std::string& HTTPaddr,unsigned int& ip,unsigned short& port)
{
	int seppos;
	int protocol;
	std::string strip;
	std::string strport;
	enum prototype
	{
		HTTP,
		UDP
	};
	if ((seppos = HTTPaddr.find("http://")) != std::string::npos)//a: from ">0" to "!=npos"
	{
		HTTPaddr = HTTPaddr.substr(seppos+7,HTTPaddr.size() - 7);
		protocol = HTTP;
	}
	//处理bt的特殊tracker地址
	else if((seppos = HTTPaddr.find("udp://")) != std::string::npos)//a: from ">0" to "!=npos"
	{
		//暂时这么写,不没有影响
		HTTPaddr = HTTPaddr.substr(seppos+6,HTTPaddr.size() - 6);
		protocol = UDP;
	}
	else
	{
		return -1;
	}
	seppos = HTTPaddr.find("/");
	if (seppos != std::string::npos)//a: from ">0" to "!=npos"
		HTTPaddr = HTTPaddr.substr(0,seppos);
	seppos= HTTPaddr.rfind(":",HTTPaddr.size());
	if (seppos == -1)
	{
		if(protocol == HTTP)
			port = htons(80);
		else
			port = htons(8080);//udptracker端口
		strip = HTTPaddr;
	}
	else
	{
		strip = HTTPaddr.substr(0,seppos);
		strport = HTTPaddr.substr(seppos+1,HTTPaddr.size() - seppos-1);
		port = htons(atoi(strport.c_str()));
	}
	if(strip == TRACKER)
	{
		//ip = CTrackerInterface::Instance()->GetTrackerIP();
	}
	else
	{
		hostent* phost = gethostbyname(strip.c_str());
		if (!phost)
		{
			return -1;
		}
		memcpy(&ip,phost->h_addr_list[0],QVOD_MIN2(sizeof(int),phost->h_length));
	}
	return 0;

}

void ChangeName(std::string& name, int& count)
{
	char newFileName[512] = {0};
	int pos = name.rfind(".");
	if (-1 == pos)
	{
		sprintf_s(newFileName,512,"%s(%d)", name.c_str(), ++count);
	}
	else
	{
		strcpy(newFileName, name.substr(0, pos).c_str());
		sprintf(newFileName+strlen(newFileName), "(%d)", ++count);
		strcpy(newFileName+strlen(newFileName), name.substr(pos).c_str());
	}
	name = newFileName;
}

void CalcSoftVersion()
{
	int n1,n2,n3,n4;
	sscanf(LIB_VERSION,"%d.%d.%d.%d",&n1,&n2,&n3,&n4);
	g_iMaxTerminalVersion = n1;
	g_iMinTerminalVersion = n4;

#ifdef QVODDEBUG
	_OUTPUT(INFO,"MaxVersion = %d,MinVersion = %d,ProtocolVersion = %d\n",g_iMaxTerminalVersion,g_iMinTerminalVersion,g_iTerminalProtocol);
#endif // QVODDEBUG
}



#ifdef WIN32 /* WIN32 */
#ifdef SUPERNODE /* non-SUPERNODE */
bool IsFileExistOnCache(const char *pFilePath,char *pExistPath,int iBufLen)
{
	if ( NULL == pFilePath )
	{
		return false;
	}
	int iLen = strlen(pFilePath);
	if ( iLen >= MAX_PATH )
	{
		return false;
	}
	char szFilePath[MAX_PATH] = {0};
	char szRootPathName[4] = {0};
	memcpy(szFilePath,pFilePath,iLen);
	memcpy(szRootPathName,"C:\\",3);
	QVOD_DWORD dwBitMask = GetLogicalDrives();
	int iCount = 0;
	int iLoop = dwBitMask >> iCount;
	while ( iLoop )
	{
		char ch = 'A' + iCount;
		szRootPathName[0] = ch;
		if ( DRIVE_FIXED == GetDriveType(szRootPathName))
		{
			szFilePath[0] = ch;
			if ( _access(szFilePath,0) != -1 )
			{
				if ( iLen > iBufLen )
				{
					return false;
				}
				memcpy(pExistPath,szFilePath,iLen);
				return true;
			}
		}
		iCount++;
		iLoop = dwBitMask >> iCount;
	}
	return false;
}
#endif /* SUPERNODE end */

#else /* posix */


#ifdef SUPERNODE /* SUPERNODE */

bool IsFileExistOnCache(const char *pFilePath,char *pExistPath,int iBufLen)
{
	if(NULL == pFilePath)
	{
		return false;
	}

	int iLen = strlen(pFilePath);
	char szFilePath[QVOD_MAX_PATH] = {0};
	memcpy(szFilePath,pFilePath,iLen);

	/* query "/proc/mounts" */
	char maxdisk = 'R';
	QVOD_INT64 maxdisksize = 0;
	FILE *fp;
	char text[256];
	long res;

	char file_dev[32];
	char mount_on[64];
	char file_system[32];
	char permission[128];
	char num1[4];
	char num2[4];

	fp = fopen("/proc/mounts", "r");
	if(fp == NULL) {
		/* error */
#ifdef QVODDEBUG
		_OUTPUT(INFO,"%s(%d)-%s: fopen() failed", __FILE__, __LINE__, __FUNCTION__);
#endif // QVODDEBUG
		return 0;
	}

	/* read line */
	while(fgets(text, 256, fp)) {

		sscanf(text, "%s %s %s %s %s %s",
					 file_dev, mount_on, file_system, permission, num1, num2); /* string add '\0' automatically */

		/* check mount_on */
		if(*mount_on == '/' && *(mount_on + 1) == '\0' && *file_dev == 'r') {
			/* rootfs */
#ifdef QVODDEBUG
			_OUTPUT(INFO,"IsFileExistOnCache find rootfs '/', dev: %s\n", file_dev);
#endif // QVODDEBUG

			/* check if file exist on "R" */
			*(szFilePath + 5) = 'R';
			if(QVOD_ACCESS(szFilePath, 0) == 0)
			{
				/* file exist */
				if(iLen > iBufLen)
				{
					fclose(fp);
					return false;
				}
				memcpy(pExistPath, szFilePath, iLen);
				fclose(fp);
				return true;
			}

		} else {

			char *pos =	strcasestr(mount_on, "/mnt/");
			if(pos != NULL &&
			   *(pos + sizeof("/mnt/")) == '\0' &&
			   *(pos + sizeof("/mnt/") - 1) > 64 &&
			   *(pos + sizeof("/mnt/") - 1) < 91) {
				/* example: /mnt/D */
#ifdef QVODDEBUG
				_OUTPUT(INFO,"find /mnt/: %s\n", pos);
#endif // QVODDEBUG

				/* check if file exist on "X" */
				*(szFilePath + 5) = (*(pos + sizeof("/mnt/") - 1));
				if(QVOD_ACCESS(szFilePath, 0) == 0)
				{
					/* file exist */
					if(iLen > iBufLen)
					{
						fclose(fp);
						return false;
					}
					memcpy(pExistPath, szFilePath, iLen);
					fclose(fp);
					return true;
				}
			}
		}
	} /* while */

	fclose(fp);
	return false;
}

#else /* non-SUPERNODE */

//nothing

#endif /* non-SUPERNODE end */

int GetFoldersAndFiles(std::string strfolder, std::vector<std::string>& vctfile)
{
	//int res;
	DIR *pDir = NULL;
	struct dirent *ent;

	/* judge if strfolder has '/' at the end of it */
	if(*(strfolder.c_str() + strfolder.length() - 1) != '/') {
		/* have no '/', add '/' at the end */
		strfolder += "/";
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO,"opendir %s\n", strfolder.c_str());
#endif // QVODDEBUG
	pDir = opendir(strfolder.c_str());
	if (!pDir)
	{
		return QVOD_ERROR;
	}
	while((ent = readdir(pDir)) != NULL) {

		if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, "lost+found") == 0) {
			continue;
		}

		std::string	ent_name = std::string(ent->d_name);
		vctfile.push_back(strfolder + ent_name);
#ifdef QVODDEBUG
		_OUTPUT(INFO,"vctfile.push_back %s\n", (strfolder + ent_name).c_str());
#endif // QVODDEBUG
	}

	closedir(pDir);
	return QVOD_OK;
}

int MoveFolder(string strfolder, string dest)
{
    int res;
	DIR *pDir;
	struct dirent *ent;
    
	/* judge if strfolder has '/' at the end of it */
	if(*(strfolder.c_str() + strfolder.length() - 1) != '/') {
		/* have no '/', add '/' at the end */
		strfolder += "/";
	}
	if(*(dest.c_str() + dest.length() - 1) != '/') {
		/* have no '/', add '/' at the end */
		dest += "/";
	}
#ifdef QVODDEBUG    
    _OUTPUT(INFO, "Begin move folder %s\n", strfolder.c_str());
#endif
    QvodMkdir(dest.c_str());
	pDir = opendir(strfolder.c_str());
    if (!pDir)
	{
		QVOD_FALSE;
	}
	while((ent = readdir(pDir)) != NULL) {
        
		if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, "lost+found") == 0) {
			continue;
		}
        
		std::string	ent_name = std::string(ent->d_name);
        
		if(ent->d_type == DT_DIR) {
			/* directory, recursion */
			res = MoveFolder(strfolder + ent_name, dest);
			if(res != QVOD_OK) {
				return QVOD_ERROR;
			}
		}
        else
        {
            string filename = strfolder + ent_name;
            string destname = dest + ent_name;
            res = rename(filename.c_str(), destname.c_str());
#ifdef QVODDEBUG
            _OUTPUT(INFO, "move file %s to %s,res %d, errno %d\n", filename.c_str(), destname.c_str(),res, errno);
#endif
        }
	}
    
	closedir(pDir);
    return 0;
}

#endif /* posix end */

#pragma obfuscate on
bool IsUseP4(int version)
{
	if ((version > 75 && version < 99) || (version > 31 && version < 39) 
		|| (version > 21 && version < 29) || (version > 102 && version < 109) 
		|| (version > 41 && version < 45))
	{
		return true;
	}
	return false;
}
#pragma obfuscate off

QVOD_ULONGLONG htonl64(QVOD_ULONGLONG hostlong)   
{   
	QVOD_ULONGLONG ret = 0;   
	QVOD_ULONG   high,low;

	low = hostlong & 0xFFFFFFFF;
	high = (hostlong >> 32) & 0xFFFFFFFF;
	low = htonl(low);   
	high = htonl(high);   
	ret = low;
	ret <<= 32;   
	ret |= high;   
	return ret;   
}

QVOD_ULONGLONG ntohl64(QVOD_ULONGLONG netlong)   
{   
	QVOD_ULONGLONG ret = 0;   
	QVOD_ULONG   high,low;

	low = netlong & 0xFFFFFFFF;
	high = (netlong >> 32) & 0xFFFFFFFF;
	low = ntohl(low);   
	high = ntohl(high);   
	ret = low;
	ret <<= 32;   
	ret |= high;   
	return ret;   
}
bool QueryP2pHash( const char* pOrgurl,const char* pKeyurl,SQueryUrltrackerResult& result )
{
	qvod_sockaddr_t addr = {0};
	unsigned long ip = 0;
	GetIpFromDomain(ip,URLTRACKER);
	if(0 == ip)
	{
		ip = inet_addr(URLTRACKERIP);
	}
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(URLTRACKERPORT);
	addr.sin_family = AF_INET;
	qvod_sock_t sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(-1 == sock)
	{
		return false;
	}
	if (!AsynConnect(sock,addr,2))
	{
		close(sock);
		return false;
	}
	//struct timeval timeout = {0};
	//timeout.tv_sec = 3;
	//timeout.tv_usec = 0;
	//setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(const char *)&timeout,sizeof(timeout));
	//setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(const char *)&timeout,sizeof(timeout));

	QvodSetSockTimeout(sock, SOL_SOCKET, SO_RCVTIMEO, 3000);
	QvodSetSockTimeout(sock, SOL_SOCKET, SO_SNDTIMEO, 3000);


#ifdef WIN32
	//发送查询请求
	char szXml[2048] = {0};
	sprintf_s(szXml,sizeof(szXml),"<Acc url=\"%s\" keyurl=\"%s\" origin=\"%s\"/>",pOrgurl,pKeyurl,ORIGIN);
	char szSend[2500] = {0};
	sprintf_s(szSend,sizeof(szSend),"POST /?opt=2 HTTP/1.1\r\nHost: %s:%d\r\nContent-Length: %d\r\n" \
		"Content-Type: text/xml\r\nAccept: */*\r\n\r\n%s",
		URLTRACKER,URLTRACKERPORT,strlen(szXml),szXml);
#else
	char szXml[2048] = {0};
	snprintf(szXml,sizeof(szXml),"<Acc url=\"%s\" keyurl=\"%s\" origin=\"%s\"/>",pOrgurl,pKeyurl,ORIGIN);
	char szSend[2500] = {0};
	snprintf(szSend,sizeof(szSend),"POST /?opt=2 HTTP/1.1\r\nHost: %s:%d\r\nContent-Length: %d\r\n" \
		"Content-Type: text/xml\r\nAccept: */*\r\n\r\n%s",
		URLTRACKER,URLTRACKERPORT,strlen(szXml),szXml);
#endif

#ifdef QVODDEBUG
	_OUTPUT(INFO,"QueryP2pHash request = %s\n",szSend);
#endif
	if (send(sock, szSend, strlen(szSend), 0) != strlen(szSend))
	{
		close(sock);
		return false;
	}
	char szRecv[4096] = {0};
	int iTotalRecv = 0;
	do 
	{
		int iRecv = recv(sock, szRecv + iTotalRecv, sizeof(szRecv) - iTotalRecv, 0);
		if (iRecv <= 0)
		{
			close(sock);
			return false;
		}
		iTotalRecv += iRecv;
	} while (strstr(szRecv,"</Acc>") == NULL);
	close(sock);
#ifdef QVODDEBUG
	_OUTPUT(INFO,"QueryP2pHash response = %s\n",szRecv);
#endif
	char* pPos = strstr(szRecv,"<Acc");
	if (strstr(szRecv,"200 OK") == NULL || NULL == pPos)
	{
		return false;
	}
	char* pXml = pPos;
	if (!strstr(pXml,"result=\"1\""))
	{
		return false;
	}
	pPos = strstr(pXml,"hash=\"");
	if (!pPos)
	{
		return false;
	}
	pPos += strlen("hash=\"");
	if (strstr(pPos,"\"") - pPos != 40)
	{
		return false;
	}
	string strHash = string(pPos,40);

	pPos = strstr(pXml,"size=\"");
	if (!pPos)
	{
		return false;
	}
	pPos += strlen("size=\"");
	if (!strstr(pPos,"\""))
	{
		return false;
	}
	string strSize = string(pPos, strstr(pPos,"\"")-pPos);

	pPos = strstr(pXml," cdn=\"");
	if (pPos)
	{
		pPos += strlen(" cdn=\"");
		if (strstr(pPos,"\""))
		{
			result.strCdn = string(pPos,strstr(pPos,"\"")-pPos);
		}
	}

	pPos = strstr(pXml," orgcdn=\"");
	if (pPos)
	{
		pPos += strlen(" orgcdn=\"");
		if (strstr(pPos,"\""))
		{
			result.strOrgCdn = string(pPos,strstr(pPos,"\"")-pPos);
		}
	}
	Char2Hash(strHash.c_str(),result.hash.data);

	result.size = QVOD_ATOI64(strSize.c_str());

#ifdef QVODDEBUG
	_OUTPUT(INFO,"QueryP2pHash hash = %s,size = %lld\n",strHash.c_str(),result.size);
#endif
	return true;
}
string HttpGetValue(const char* buf, const char* key)
{
	char val[1024] = {0};
	const char* pos = strstr(buf, key);
	if (pos)
	{
		pos += strlen(key);
		const char* endpos = strstr(pos, "\r\n");
		if (endpos)
		{
			memcpy(val, pos, endpos-pos);
		}
	}
	return val;
}

void Fake2Real( const unsigned char *pFake,unsigned char *pReal )
{
	unsigned char szHash[20] = {0};
	memcpy(szHash,pFake,20);

	//假HASH转化为真HASH
	unsigned int *phash = (unsigned int*)szHash;
	int m = 0;
	int n = 0;
	const char key[4] = {1,9,8,7};
	for(m = 0;m<5;m++)
	{
		szHash[0+m*4] ^= 0x69;
		szHash[1+m*4] ^= 0x4A;
		szHash[2+m*4] ^= 0x87;
		szHash[3+m*4] ^= 0x3C;
		phash[m] = (phash[m]<<key[n]) + (phash[m]>>(32-key[n]));
		int range = szHash[2]%4+1;
		n = (n+1)%range;
	}
	memcpy(pReal,szHash,20);
}

void Real2Fake( const unsigned char *pReal,unsigned char *pFake )
{
	unsigned char szHash[20] = {0};
	memcpy(szHash,pReal,20);
	unsigned int *phash = (unsigned int*)szHash;

	//真HASH转化为假HASH
	int m,n = 0;
	const char key[4] = {1,9,8,7};
	int range = szHash[2]%4+1;

	for(m = 0;m<5;m++)
	{
		phash[m]  = (phash[m]>>key[n]) + (phash[m]<<(32-key[n]));
		szHash[0+m*4] ^= 0x69;
		szHash[1+m*4] ^= 0x4A;
		szHash[2+m*4] ^= 0x87;
		szHash[3+m*4] ^= 0x3C;
		n = (n+1)%range;
	}
	memcpy(pFake,szHash,20);
}

void CreateDirectoryForAllUser(const char *wpDir)
{
	if ( wpDir == NULL )
	{
		return ;
	}
#ifdef WIN32
	CreateDirectory(wpDir, NULL);
	if(GetFileAttributes(wpDir) != INVALID_FILE_ATTRIBUTES)
	{
		SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
		// 获得权限
		PSECURITY_DESCRIPTOR psd = new BYTE[4096];
		QVOD_DWORD dwLen, dwLenNeed;
		dwLen = 4096;
		GetFileSecurity(wpDir, si, psd, dwLen, &dwLenNeed);

		// 转换到string
		char *wpDescriptor = NULL;
		ConvertSecurityDescriptorToStringSecurityDescriptor(psd, SDDL_REVISION_1, si, &wpDescriptor, NULL);
		delete []psd;
		if(!wpDescriptor)
		{
			return;
		}

		// 获得新权限
		string strDescriptor = wpDescriptor;
		LocalFree(wpDescriptor);
		if(strDescriptor.find("(A;OICI;FA;;;BU)") == std::string::npos)
		{
			int n = strDescriptor.find('(');
			if(n != std::wstring::npos )
			{
				strDescriptor.insert(n, "(A;OICI;FA;;;BU)");
				PSECURITY_DESCRIPTOR psd_new = NULL;
				ConvertStringSecurityDescriptorToSecurityDescriptor(strDescriptor.c_str(), SDDL_REVISION_1, &psd_new, NULL);
				// 调整权限
				SetFileSecurity(wpDir, si, psd_new);
				LocalFree(psd_new);
			}
		}
	}
#else
	QvodMkdir(wpDir);
#endif
}

bool CreateFolderForAllUser( const char* pFolder )
{
	if (NULL == pFolder)
	{
		return false;
	}
	QvodMkdir(pFolder);

	if (QVOD_ACCESS(pFolder,0) == 0)
	{
		return true;
	}
	string wstrFolder = pFolder;
	int pos = wstrFolder.find("/",0);
	while (pos != wstring::npos )
	{		
		string wstrPreFolder = wstrFolder.substr(0,pos);
		if (QVOD_ACCESS(wstrPreFolder.c_str(),0) != 0)
		{
			CreateDirectoryForAllUser(wstrPreFolder.c_str());
		}
		++pos;
		if (wstrFolder.size() == pos)
		{
			break;
		}
		pos = wstrFolder.find("/",pos);
	}
	if (QVOD_ACCESS(pFolder,0) == 0)
	{
		return true;
	}
#ifdef _QVODDEBUG
	_OUTPUT(ERROR1,"CreateFolderForAllUser %s fail\n",pFolder);
#endif
	return false;
}

bool SetNonBlock( int fd )
{
#ifdef WIN32
	unsigned long ul = 0;
	int iOptions = ioctlsocket(fd,FIONBIO,&ul);

	return (iOptions >= 0) ;
#else
	int iOptions = -1;
	iOptions = fcntl(fd, F_GETFL);
	if(iOptions < 0)
	{
		return false;
	}

	iOptions = (iOptions | O_NONBLOCK);
	if(0 > fcntl(fd, F_SETFL, iOptions))
	{
		return false;
	}
	return true;
#endif
}

bool AsynConnect( int sock,qvod_sockaddr_t addr,int timeout )
{
#ifdef WIN32
	bool bRet = false;
	unsigned long ul = 1;
	ioctlsocket(sock,FIONBIO,&ul);
	if (connect(sock,(PSOCKADDR)&addr,sizeof(addr)) < 0)
	{
		struct timeval tv = {0};
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		fd_set fs;
		FD_ZERO(&fs);
		FD_SET(sock,&fs);
		int ret = select(sock+1,NULL,&fs,NULL,&tv);
		if (ret <= 0)
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1,"%s_%d ret = %d,error = %d\n",__FUNCTION__,__LINE__,ret,WSAGetLastError());
#endif
			closesocket(sock);
			bRet = false;
		}
		else
		{
			bRet = true;
		}
	}
	ul = 0;
	ioctlsocket(sock,FIONBIO,&ul);

	return bRet;
#else
	bool bRet = false;
	int iOldOptions = fcntl(sock, F_GETFL);
	if(iOldOptions < 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO,"AsynConnect F_GETFL error = %d\n",errno);
#endif // QVODDEBUG

		return false;
	}
	//设置为异步
	int iOptions = (iOldOptions | O_NONBLOCK);
	if(0 > fcntl(sock, F_SETFL, iOptions))
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO,"AsynConnect F_SETFL error = %d\n",errno);
#endif // QVODDEBUG
		return false;
	}
	if(0 != connect(sock,(sockaddr*)&addr,sizeof(addr)))
	{
		struct timeval tv = {0};
		tv.tv_sec = timeout;
		fd_set set = {0};
		FD_ZERO(&set);
		FD_SET(sock,&set);
		int error = -1;
		socklen_t len = sizeof(error); 
		if (select(sock+1,NULL,&set,NULL,&tv) > 0)
		{				
			getsockopt(sock,SOL_SOCKET,SO_ERROR,(char*)&error,&len);				
			if (0 == error)
			{
				bRet = true;	//连接成功
			}
			else
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO,"AsynConnect getsockopt error = %d\n",error);
#endif // QVODDEBUG
			}
		}	
	}
	else
	{
		bRet = true;
	}
	//设置回原来的文件属性
	if (fcntl(sock, F_SETFL, iOldOptions) < 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO,"AsynConnect F_SETFL error = %d\n",errno);
#endif // QVODDEBUG
	}
	return bRet;
#endif
}

#ifndef WIN32
int GetFolderFiles(std::string strfolder, std::vector<std::string>& vctfile,std::string stradurl)
{
	int res;
	DIR *pDir;
	struct dirent *ent;

	/* judge if strfolder has '/' at the end of it */
	if(*(strfolder.c_str() + strfolder.length() - 1) != '/') 
	{
		/* have no '/', add '/' at the end */
		strfolder += "/";
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO,"GetFolderFiles opendir %s\n", strfolder.c_str());
#endif	
	pDir = opendir(strfolder.c_str());
    if (!pDir)
    {
        return QVOD_ERROR;
    }
	while((ent = readdir(pDir)) != NULL) 
	{

		if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, "lost+found") == 0) 
		{
			continue;
		}

		std::string	ent_name = std::string(ent->d_name);

		if(ent->d_type == DT_DIR) 
		{
			/* directory, recursion */
			res = GetFolderFiles(strfolder + ent_name, vctfile, stradurl);
			if(res != QVOD_OK) 
			{
				return QVOD_ERROR;
			}
		}
		else
		{
			if(stradurl == "")
			{
				vctfile.push_back(strfolder + ent_name);
#ifdef QVODDEBUG
				_OUTPUT(INFO,"GetFolderFiles %s\n", (strfolder + ent_name).c_str());
#endif				
			}
			else
			{
				vctfile.push_back(strfolder + ent_name + "|" + stradurl);
			}
		}
	}
	closedir(pDir);
	return QVOD_OK;

}
#endif
