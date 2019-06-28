#include "Seed.h"
#include "qvod_core.h"
#include "common-utility.h"
#include "common-commonfun.h"

#include <iostream>
#include <algorithm>

CSeed::CSeed(const char *filename)
{
	m_hash_list = NULL;
	m_infohash = NULL;
	m_torrent = NULL;	//种子文件内容
	m_torrentlen = 0;	//种子文件长度
	m_strSeedName = string(filename); //下载文件名
	m_fileLen = 0; //下载文件长度
	m_piecenum = 0; //下载文件分片数
	m_piecesize = 0;	//下载文件分片大小
	m_createtime = 0;	//创建时间
	m_TrackerIP = 0;	//tracker ip
	m_TrackerPort = 0;	//tracker port
	m_downfilesize = 0;
	m_trackerlist.clear();
	m_bIsSeedParse = false;
}

CSeed::~CSeed()
{
	if(m_torrent != NULL)
	{
		delete [] m_torrent;
		m_torrent = NULL;
	}
}

bool CSeed::Read()
{
	string fn = g_strtorrentpath + m_strSeedName + ".torrent";
	FILE *fp;
	fp = fopen(fn.c_str(), "rb");
	if(NULL == fp)
	{
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "CSeed::Read can not find fn: %s\n", fn.c_str());
#endif
		return false;
	}

	qvod_stat_t buffer;
	if(0 != QVOD_STAT(fn.c_str(), &buffer))
	{
		fclose(fp);
#ifdef QVODDEBUG
		_OUTPUT(FAIL, "CSeed::Read stat fail: %s\n", fn.c_str());
#endif
		return false;
	}
	
	m_torrentlen = buffer.st_size;
	//种子文件为空的情况
	if ( 0 == m_torrentlen )
	{
		fclose(fp);
		return false;
	}

	if(NULL != m_torrent)
	{
		delete[] m_torrent;
	}
	m_torrent = new char[m_torrentlen + 1];
	if(m_torrent == NULL)
	{
		fclose(fp);
		return false;
	}
	memset(m_torrent,0,m_torrentlen+1);
	
	int ret = fread(m_torrent, 1, m_torrentlen, fp);
	if(ret < 0 )
	{
		fclose(fp);
		return false;
	}	
	fclose(fp);
	return true;
}

bool CSeed::Read(const char *filename)
{
	if(NULL == filename)
	{
		return false;
	}

	string fn = string(filename);
	
	FILE *fp;
	fp = fopen(fn.c_str(), "rb");
	if(NULL == fp)
	{
		return false;
	}

	qvod_stat_t buffer;
	if(0 != QVOD_STAT(fn.c_str(), &buffer))
	{
		fclose(fp);
		return false;
	}
	
	m_torrentlen = buffer.st_size;
	//种子文件为空的情况
	if ( 0 == m_torrentlen )
	{
		fclose(fp);
		return false;
	}
	if ( m_torrent != NULL )
	{
		delete[] m_torrent;
		m_torrent = NULL;
	}

	m_torrent = new char[m_torrentlen + 1];
	if(m_torrent == NULL)
	{
		fclose(fp);
		return false;
	}
	memset(m_torrent,0,m_torrentlen+1);
	int ret = fread(m_torrent, 1, m_torrentlen, fp);
	if(ret < 0 )
	{
		fclose(fp);
		return false;
	}	
	fclose(fp);	
	return true;
}

bool CSeed::Init(const char *buf, int len)
{
	//若m_torrentlen已经初始化过，且长度和现在赋予的长度不等，则返回失败
	if(m_torrentlen!=0&&m_torrentlen!=len)
		return false;
	m_torrentlen = len;
	if(NULL == m_torrent)
	{
		m_torrent = new char[m_torrentlen + 1];
		if(m_torrent == NULL)
		{
			return false;
		}
	}
	memset((char *)m_torrent, 0, m_torrentlen);
	memcpy((char *)m_torrent, buf, m_torrentlen);
	return true;
}

bool CSeed::Save()
{
	if(NULL == m_torrent)
	{
		return false;
	}
	string fn = g_strtorrentpath + m_strSeedName + ".torrent"; //种子文件保存全路径
#ifdef QVODDEBUG
	_OUTPUT(INFO, "torrent save path: %s\n", fn.c_str());
#endif
	FILE *fp;
	fp = fopen(fn.c_str(), "wb");
	if(NULL == fp)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "%s(%d)-%s: fopen error: %d\n", __FILE__, __LINE__, __FUNCTION__, QvodGetLastError());
#endif
		return false;
	}	
	int ret = fwrite((char *)m_torrent, 1, m_torrentlen, fp);
	if(ferror(fp))
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "%s(%d)-%s: fwrite error: %d\n", __FILE__, __LINE__, __FUNCTION__, QvodGetLastError());
#endif
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "torrent m_torrentlen = %d\n", m_torrentlen);
#endif
	fflush(fp);
	fclose(fp);
	return true;
}

bool CSeed::IsExist()
{
	std::string file = g_strtorrentpath + m_strSeedName + ".torrent"; //种子文件保存全路径
	if ( QVOD_ACCESS( file.c_str(),0) != -1 )
	{
		return true;
	}
	return false;
}

bool CSeed::Remove()
{
	string strSeedPath = g_strtorrentpath + m_strSeedName + ".torrent"; //种子文件保存全路径
	if ( access(strSeedPath.c_str(),0) != -1 )
	{
		if(remove(strSeedPath.c_str()) == 0)
		{
			return true;
		}
	}
	return false;
}

bool CSeed::Load()
{
	if ( !Read() || !ParseTorrentFile())
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CSeed::Load false\n");
#endif
		return false;
	}
	return true;
}

void CSeed::Reset()
{
	if(m_torrent)
	{
		delete m_torrent;
		m_torrent = NULL;		
	}

	m_torrentlen = 0;	//种子文件长度
	m_fileLen = 0; //下载文件长度
	m_piecenum = 0; //下载文件分片数
	m_piecesize = 0;	//下载文件分片大小
	m_createtime = 0;	//创建时间
	m_TrackerIP = 0;	//tracker ip
	m_TrackerPort = 0;	//tracker port
	m_downfilesize = 0;
	m_hash_list = NULL;
	m_infohash = NULL;
	m_trackerlist.clear();
}

char* CSeed::GetPieceBuf(unsigned int index)
{
	if(m_hash_list == NULL)
	{
		return NULL;
	}

	return (char*)(m_hash_list+index);
}

void CSeed::SetFileLength(QVOD_UINT64 iLen)
{
	m_downfilesize = iLen;
	m_piecesize = CalcPieceSize(iLen);
	m_piecenum = iLen/m_piecesize + (iLen%m_piecesize == 0?0:1);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CSeed::SetFileLength iLen=%lld piecesize=%d piecenum=%d", iLen, m_piecesize, m_piecenum);
#endif
}

bool CSeed::ParseTorrentFile()
{
	if(m_torrent == NULL)
	{
		return false;
	}
	parseDictionary((char *)m_torrent, (char *)(m_torrent + m_torrentlen));
	if(m_hash_list == NULL)
	{
		return false;
	}
	else
	{
		m_bIsSeedParse = true;
		return true;
	}
}

string &CSeed::GetTorrentFileName(void)
{
	return m_strSeedName;
}

QVOD_UINT64 CSeed::GetDownFileSize(void)
{
	return m_downfilesize;
}

unsigned int CSeed::GetPieceNum(void)
{
	return m_piecenum;
}

unsigned int CSeed::GetPieceSize(void)
{
	return m_piecesize;
}

unsigned int CSeed::GetCreateTime(void)
{
	return m_createtime;
}

unsigned int CSeed::GetRrackerIP(void)
{
	return m_TrackerIP;
}

unsigned short CSeed::GetTrackerPort(void)
{
	return m_TrackerPort;
}

bool CSeed::CheckHash(unsigned int index, HASH hash)
{
	if(memcmp(hash.data, m_hash_list[index].data, 20) == 0)
	{
		return true;
	}

	return false;
}

string& CSeed::GetAdUrl(void)
{
	return m_adurl;
}

string& CSeed::GetTextAdUrl(void)
{
	return m_TextAdUrl;
}

string& CSeed::GetPauseAdUrl(void)
{
	return m_PauseAdUrl;
}

unsigned char*CSeed::GetTorrent(void)
{
	if(m_torrent)
	{
		return (unsigned char*)m_torrent;
	}
	return NULL;
}

unsigned int CSeed::GetTorrentLen(void)
{
	return m_torrentlen;
}

unsigned int CSeed::GetTrackerIP()
{
	return m_TrackerIP;
}

string &CSeed::GetDownFileName(void)
{
	return m_downFileName;
}

string CSeed::GetFileName(unsigned int pos)
{
	return m_vctfile.size() >pos?(m_downFileName+m_vctfile[pos].strname):m_downFileName;
}

//=========================解析bt种子文件实现==============================================
char* CSeed::parseDictionary(char* start,char* end)
{
	char* buf = start;
	if (*buf++ != 'd') 
	{
		return 0;
	}
	while(buf < end && *buf != 'e')
	{
		if (buf[0] == 'd'||buf[0] == 'l'||buf[0]=='i')//嵌套字典
		{
			return 0;
		}

		std::string paraname;
		std::string paravalue;
		char* pbuf = buf;
		while(*buf != ':' && *buf!='e')
		{
			//种子解析合理性修正-wangxijie 20110131
			if(buf - m_torrent >= m_torrentlen)
			{
				return 0;
			}
			buf++;
		}
		if (*buf =='e')
		{
			return 0;//结束
		}
		QVOD_INT64 num = QVOD_ATOI64(pbuf);
		buf++;
		//对num数据合理性判断-wangxijie 20110131
		if(num <= 0 || m_torrent + m_torrentlen  - buf < num)
		{
			return 0;
		}
		paraname.append(buf,size_t(num));
		buf += num;
		if (buf[0] == 'd')
		{
			if(paraname == "info")
			{
				char* infohash = buf;
				buf = parseDictionary(buf,end);
				if (!buf) 
				{
					return 0;
				}
				m_infohashlen = buf -infohash;
				m_infohash = infohash;
			}
			else
			{
				buf = parseDictionary(buf,end);
				if (!buf) 
				{
					return 0;
				}
			}
		}
		else if(buf[0] == 'l')
		{
			if (paraname == "files")
			{
				buf = parseFiles(buf,end);
				if (!buf) 
				{
					return 0;
				}
			}
			else if(paraname == "announce-list")
			{
				buf = parseAnnounces(buf);
				if (!buf) 
				{
					return 0;
				}
			}
			else
			{
				buf = parseList(buf,end);
				if (!buf) 
				{
					return 0;
				}
			}
		}
		else if (buf[0]=='i')//数值
		{
			buf++;
			num = QVOD_ATOI64(buf);
			while(*buf != 'e' )
			{
				//种子解析合理性修正-wangxijie 20110131
				if(buf - m_torrent >= m_torrentlen)
				{
					return 0;
				}
				buf++;
			}
			buf++;
			if (paraname == "piece length") 
			{
				m_piecesize = (unsigned int)num;
			}
			else if (paraname == "length") 
			{
				if(m_downfilesize && m_downfilesize != num)
				{
					return 0;
				}
				m_downfilesize = num; 
			}
			
		}
		else //字符串
		{
			pbuf = buf;
			while(*buf != ':' && *buf!='e')
			{
				//种子解析合理性修正-wangxijie 20110131
				if(buf - m_torrent >= m_torrentlen)
				{
					return 0;
				}
				buf++;
			}
			if (*buf =='e')
			{
				return 0;//结束
			}
			num = QVOD_ATOI64(pbuf);
			if ( num >= m_torrentlen )
			{
				return 0;
			}
			buf++;
			//对num数据合理性判断-wangxijie 20110131
// 			if(num <= 0 || m_torrent + m_torrentlen  - buf < num)
// 			{
// 				return 0;
// 			}
			paravalue.append(buf,(size_t)num);
			buf += num;
			if (paraname == "pieces")
			{
				m_piecenum = (unsigned int)(num/20);
				if (m_piecenum <=0)
				{
					return 0;//没有分片错误
				}
				char *pos = buf - num;
				//哈希串
				m_hash_list = (HASH *)pos;
			}
			else if(paraname == "announce")
			{
				m_announce = paravalue;
				m_trackerlist.push_back(m_announce);
			}
			else if(paraname == "encoding")
			{
				if(paravalue == "UTF-8")
					m_encoding = UTF_8;
				else if(paravalue == "Big5")
					m_encoding = BIG5;
				else if(paravalue == "GBK")
					m_encoding = GBK;
			}
			else if (paraname == "name")
			{
				if(m_encoding == UTF_8)
				{
					char namebuf[512];
					int buflen = 511;
					utf8Decode2((char*)paravalue.c_str(),paravalue.size(),namebuf,buflen); /* silver, char* */
					namebuf[buflen]=0;
					m_downFileName = namebuf;
				}
				else
				{
					m_downFileName = paravalue; //待下载文件文件名
				}
				ReviseFileName(m_downFileName);
			}
			else if (paraname == "name.utf-8")
			{
				if(m_encoding != GBK)
				{
					char namebuf[512];
					int buflen = 511;
					utf8Decode2((char*)paravalue.c_str(),paravalue.size(),namebuf,buflen); /* silver, char* */
					namebuf[buflen]=0;
					m_downFileName = namebuf;
					ReviseFileName(m_downFileName);
				}
			}
			else if(paraname == "adurl")
			{
				m_adurl = paravalue; //广告连接
			}
			else if (paraname == "textadurl")
			{
				m_TextAdUrl = paravalue;
			}
			else if (paraname == "pauseadurl")
			{
				m_PauseAdUrl = paravalue;
			}
			else if(paraname == "creation date")
			{
				m_createtime = (unsigned int)QVOD_ATOI64(paravalue.c_str());
			}
		}
	}
	return ++buf;
}
char* CSeed::parseFiles(char* start,char* end)
{
	//ASSERT( start);
	char* buf = start;
	//ASSERT(*buf == 'l');
	buf++;
	while(*buf !='e')
	{
		buf = parseFile(buf,end);
		if (!buf) 
		{
			return 0;
		}
	}
	return ++buf;
}
char* CSeed::parseFile(char* start,char* end)
{
//	ASSERT( start);
	char* buf = start;
	fileinfo fi;

	//ASSERT(*buf == 'd');
	buf++;
	while(buf < end && *buf !='e')
	{
		std::string paraname;
		char* pbuf = buf;
		while(*buf != ':' && *buf!='e')
		{
			//种子解析合理性修正-wangxijie 20110131
			if(buf - m_torrent >= m_torrentlen)
			{
				return 0;
			}
			buf++;
		}
		if (*buf =='e')
		{
			return 0;//结束
		}
		QVOD_INT64 num = atoi(pbuf);
		buf++;
		//对num数据合理性判断-wangxijie 20110131
		if(num <= 0 || m_torrent + m_torrentlen  - buf < num)
		{
			return 0;
		}
		paraname.append(buf,(unsigned int)num);
		buf += num;
		if(buf[0] == 'l')
		{
			if (paraname == "path")
			{
				std::string strpath;
				buf = parsePath(buf,strpath);
				if(!buf)
				{
					return 0;
				}
				if(m_encoding == UTF_8)
				{
					char namebuf[1448];
					int buflen = 1446;
					utf8Decode2((char*)strpath.c_str(),strpath.size(),namebuf,buflen); /* silver, char* */
					namebuf[buflen]=0;
					strpath = namebuf;
				}
			/*	std::replace(strpath.begin(),strpath.end(),'?',' ');
				std::replace(strpath.begin(),strpath.end(),'/',' ');
				//std::replace(strpath.begin(),strpath.end(),'\\',' ');
				std::replace(strpath.begin(),strpath.end(),'|',' ');
				std::replace(strpath.begin(),strpath.end(),':',' ');
				std::replace(strpath.begin(),strpath.end(),'*',' ');
				std::replace(strpath.begin(),strpath.end(),'<',' ');
				std::replace(strpath.begin(),strpath.end(),'>',' ');
				std::replace(strpath.begin(),strpath.end(),'\"',' '); */
				fi.strname = strpath;
			}
			else
			{
				buf =  parseList(buf,end);
				if (!buf) 
				{
					return 0;
				}
			}
		}
		else if (buf[0]=='i')//数值
		{
			buf++;
			num = QVOD_ATOI64(buf);
			while(*buf != 'e' )
			{
				//种子解析合理性修正-wangxijie 20110131
				if(buf - m_torrent >= m_torrentlen)
				{
					return 0;
				}
				buf++;
			}
			buf++;
			if (paraname == "length") 
			{
				fi.filelen = (unsigned int)(num);
				m_downfilesize += num;
			}
		}
		else//字串或其他
		{
			pbuf = buf;
			while(*buf != ':' && *buf!='e')
			{
				//种子解析合理性修正-wangxijie 20110131
				if(buf - m_torrent >= m_torrentlen)
				{
					return 0;
				}
				buf++;
			}
			if (*buf =='e')
			{
				return 0;//结束
			}
			num = QVOD_ATOI64(pbuf);
			buf++;
			//对num数据合理性判断-wangxijie 20110131
			if(num <= 0 || m_torrent + m_torrentlen  - buf < num)
			{
				return 0;
			}
			buf += num;
		}
	}
	m_vctfile.push_back(fi);
	return ++buf;
}
char* CSeed::parsePath(char* start,std::string &strpath)
{
//	ASSERT( start);
	char* buf = start;
//	ASSERT(*buf == 'l');
	buf++;		
	while(*buf !='e')
	{
		char* pbuf = buf;
		while(*buf != ':' && *buf!='e')
		{
			//种子解析合理性修正-wangxijie 20110131
			if(buf - m_torrent >= m_torrentlen)
			{
				return 0;
			}
			buf++;
		}
		if (*buf =='e')
		{
			return 0;//结束
		}
		int num = atoi(pbuf);
		buf++;
		//对num数据合理性判断-wangxijie 20110131
		if(num <= 0 || m_torrent + m_torrentlen  - buf < num)
		{
			return 0;
		}

		strpath.append("/");

		strpath.append(buf,num);
		buf += num;
	}
	return ++buf;
}

char* CSeed::parseList(char* start,char* end)
{
//	ASSERT( start);
	char* buf = start;
	//ASSERT(*buf == 'l');
	buf++;
	while(start < end && *buf !='e')
	{
		if(*buf == 'l')
		{
			buf = parseList(buf,end);
			if (!buf) 
			{
				return 0;
			}
		}
		else if(*buf == 'd')
		{
			buf = parseDictionary(buf,end);
			if (!buf) 
			{
				return 0;
			}
		}
		else if(*buf == 'i')
		{
			while(*buf!='e')
			{
				//种子解析合理性修正-wangxijie 20110131
				if(buf - m_torrent >= m_torrentlen)
				{
					return 0;
				}
				buf++;
			}
			buf++;
		}
		else
		{
			char* pbuf = buf;
			while(*buf != ':' && *buf!='e')
			{
				//种子解析合理性修正-wangxijie 20110131
				if(buf - m_torrent >= m_torrentlen)
				{
					return 0;
				}
				buf++;
			}
			if (*buf =='e')
			{
				return 0;//结束
			}
			int num = atoi(pbuf);
			buf++;
			//对num数据合理性判断-wangxijie 20110131
			if(num <= 0 || m_torrent + m_torrentlen  - buf < num)
			{
				return 0;
			}
			buf += num;
		}
	}
	return ++buf;
}
char* CSeed::parseAnnounces(char* start)
{
	if(NULL == start)
	{
		return 0;
	}

	char* buf = start;

	if(*buf != 'l')
	{
		return 0;
	}

	buf++;
	while(*buf !='e')
	{
		if(*buf == 'l')
		{
			buf = parseAnnounces(buf);		
			if (!buf) 
			{
				return 0;
			}
		}
		else
		{
			char* pbuf = buf;
			while(*buf != ':' && *buf!='e')
			{
				//种子解析合理性修正-wangxijie 20110131
				if(buf - m_torrent >= m_torrentlen)
				{
					return 0;
				}
				buf++;
			}
			if (*buf =='e')
			{
				return 0;//结束
			}
			int num = atoi(pbuf);
			buf++;
			//对num数据合理性判断-wangxijie 20110131
			if(num <= 0 || m_torrent + m_torrentlen  - buf < num)
			{
				return 0;
			}
			std::string announce;
			announce.append(buf,num);
			m_trackerlist.push_back(announce);			
			buf += num;
		}
	}
	return ++buf;	
}

string  CSeed::GetAnnounces(void)
{
	return m_announce;
}

