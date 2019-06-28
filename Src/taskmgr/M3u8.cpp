#include "M3u8.h"

#include "common-commonfun.h"
#include "common-utility.h"
#include "qvod_file.h"
#include <stdio.h>
#include "qvod_time.h"
#include "CfgHost.h"
#include "CacheDataInMem.h"
#include "AutoPtr.h"
#include "sha1.h"
#include "openssl/hmac.h"
#include "openssl/md5.h"
#include "base64.h"
#include "function.h"
#include "NetSign.h"
#include "MD5Checksum.h"
#include "json.h"
#include "NetUrl.h"
#include <algorithm>

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define MAX_RECORD_CONTINUE_PLAY_TS_NUM	4		//最大记录连续播放ts数

#define CHAR_SIZE 8																						// 1字节8bit
#define BITSLOT(b)  ((b) / CHAR_SIZE)														// 计算该bit所在字节索引
#define BITMASK(b) (0x80 >> ((b)%CHAR_SIZE))										// 位从右到左  ( 1 << ((b)%CHAR_SIZE)),这个是位从左到右
#define BITSET(arr, b) ((arr)[BITSLOT(b)] |= BITMASK(b))						// 设置该bit状态
#define BITCLEAR(arr, b) ((arr)[BITSLOT(b)] &= ~BITMASK(b))				// 重置该bit状态
#define BITTEST(arr, b) ((arr)[BITSLOT(b)] & BITMASK(b))						// 检测该bit状态

static const int nAesPieceSize = 16384;				// aes加密数据块为16KB

CM3u8::CM3u8( const char *pFilename,const char* pTsPath ):
m_iPushTsIndex(-2)
{
	m_strM3u8Name = pFilename;
	m_strTsPath = pTsPath;
	m_pBitField = NULL;
	m_iPlayPreTsIndex = 0;
	m_iPlayTsIndex = 0;
	m_iCurPlayIndex = 0;
	m_iTsDuration = 0;
	m_iTsAverageRate = 0;
	m_iTotalDownloadSize = 0;
	m_iTotalDownloadDuration = 0;
	m_bSpaceMode = true;
	m_bCacheTimeCalBySetTime = false;
	m_iCurPlayTime = 0;

	m_bReadM3u8 = false;
}

CM3u8::~CM3u8()
{
	SafeDeleteArray(m_pBitField);
	ClearM3u8TsData();
}

#include <fstream>
using namespace std;

bool CM3u8::Init(std::string &strContent, bool bLoad)
{
    CAutoLock lock(m_lock);
	Clear();
	if (strContent.rfind("#EXT-X-ENDLIST") == string::npos)	// 没有找到m3u8末尾标记
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CM3u8::Init not find #EXT-X-ENDLIST\n");
#endif
		return false;	
	}

	bool bNeedSignTs = false;	// 是否需要签名处理
	string strKeyID;
	string strKeySecret;
	string strExpires;
	if (! bLoad)		// 下载过来的m3u8进行ts签名逻辑
	{
		map<string, string> mParams;
		net::CUrl::ParseParams(m_strM3u8Url, mParams);			// 解析m3u8url参数

		strKeyID = mParams["OSSAccessKeyId"];
		strKeySecret = g_cfg.m_mSignKeys[strKeyID];
		strExpires = mParams["Expires"];
		bNeedSignTs = ! (strKeyID.empty() || strKeySecret.empty());			// 是否需要进行签名
	}

#ifdef QVODDEBUG
	QVOD_DWORD dwTick = QvodGetTime();
	_OUTPUT(INFO, "CM3u8::Init start parse m3u8!  bNeedSignTs=%d strKeyID=%s", bNeedSignTs, strKeyID.c_str());
#endif
	if (bNeedSignTs)
	{	
		strContent.reserve(strContent.length()*3);		// 扩大容量以减少接下来的ts签名串替换不断修改容量问题
	}

	int index = 0;						//遍历m3u8文件
	unsigned int nMinTsDu = 0 ;			//记录最小ts时长

	unsigned int nTotalDuration = 0;
	QVOD_INT64 nTotalSize = 0;
	string strLine;
	int nPos = 0;
	int nBeginPos = 0;
	int nEndPos = 0;
	const char *TAG_FILE_SIZE = "#EXTINF-X-SIZE:";
	const char *TAG_FILE_HASH = "#EXTINF-X-MD5:";
	const int EXT_FILE_SIZE = strlen(TAG_FILE_SIZE);
	const int EXT_FILE_HASH = strlen(TAG_FILE_HASH);
	const int EXT_INF_SIZE = strlen("#EXTINF:");
	bool bFindTargetDurationTs = true;
	while (nEndPos != string::npos)
	{
		nEndPos = strContent.find('\n', nBeginPos);
		strLine = strContent.substr(nBeginPos, (nEndPos != string::npos)? nEndPos-nBeginPos: -1);
		char ch = strLine[0];

		if (bFindTargetDurationTs && (nPos=strLine.find("#EXT-X-TARGETDURATION:")) != string::npos)	// 只找一次
		{
			bFindTargetDurationTs = false;
			m_iTsDuration = atoi(strLine.c_str()+nPos+strlen("#EXT-X-TARGETDURATION:"));

			nMinTsDu	  = m_iTsDuration ;

#ifdef QVODDEBUG
			_OUTPUT(INFO,"Init m3u8 duration = %d second\n",m_iTsDuration);
#endif
		}
		else if ((nPos = strLine.find(TAG_FILE_SIZE)) != string::npos)
		{
			unsigned int nTsSize = atoi(strLine.c_str()+nPos+EXT_FILE_SIZE);
			m_tsSizeVec.push_back(nTsSize);
			nTotalSize += nTsSize;
			//m_offsetVec.push_back(nTotalSize);			// 先不走HLSP2P_DOWNTASK逻辑
		}
		else if ((nPos = strLine.find(TAG_FILE_HASH)) != string::npos)
		{
			string strFileHash = strLine.substr(nPos+EXT_FILE_HASH);
			m_tsHashVec.push_back(strFileHash);
		}
		else	if ((nPos=strLine.find("#EXTINF:")) != string::npos)
		{
			double fDuration = atof(strLine.c_str()+nPos+EXT_INF_SIZE);
			unsigned int iDuration = (unsigned int)(fDuration*1000);
			//unsigned short iDuration = atoi(pLine+strlen("#EXTINF:"));
			if (iDuration != 0)
			{
				m_tsDurationVec.push_back(iDuration);
				nTotalDuration += iDuration;
				m_tsOffsetDur.push_back(nTotalDuration);

				nMinTsDu = QVOD_MIN2(nMinTsDu,iDuration) ;
			}
		}
		else	if (ch != '#' && ch != '\n' && ch != ' ' && ch != '\t' && ch != '\0')
		{
			string strUrl = strLine ;					//是ts文件

			if (bNeedSignTs)	// 需要对ts进行签名
			{
				int len = strUrl.length();
				bool bRelativeUrl = false;
				if (strUrl.find("://") == string::npos)	// 是否相对路径
				{
					bRelativeUrl = true;
					strUrl = m_strM3u8PreUrl+strUrl;
				}

				strUrl = SignUrl(strUrl, strKeyID, strKeySecret, strExpires);

				if (bRelativeUrl)
				{
					strUrl = strUrl.substr(m_strM3u8PreUrl.length(), -1);
				}
				strContent.replace(nBeginPos, len, strUrl);
				nEndPos += strUrl.length()-len;			// 相应修改nEndPos位置
			}
			
			m_tsNameVec.push_back(strUrl);
            
			string strFileName;
            const char* pFileName = strrchr(strUrl.c_str(), '/');
            if (pFileName)
            {
                strFileName = pFileName+1;
            }
			else
			{
				strFileName = strUrl;
			}
            int idx = index++;
			int pos = strFileName.find("?") ;		// 去掉url后面参数
			if ( pos != string::npos)
			{
				strFileName = strFileName.substr(0,pos) ;
			}
			            
			m_tsFileNameVec.push_back( strFileName ) ;
			m_tsNameIndexMap.insert(make_pair(strFileName,idx));
		}
		else	if ((nPos=strLine.find("#EXT-X-ENDLIST")) != string::npos)
		{
			break;
		}
		nBeginPos = nEndPos+1;
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8::Init finish parse m3u8! tick=%d", QvodGetTime()-dwTick);
#endif

	if (m_tsNameVec.size() > 0)
	{
		m_iTsDuration = nTotalDuration / m_tsNameVec.size();
	}
	//m_iTsDuration = (nMinTsDu+m_iTsDuration)/2 ;

	if (m_offsetVec.size() > 0 && m_offsetVec.size() != m_tsNameVec.size())
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"Init %s.m3u8 %d not match %d\n",m_strM3u8Name.c_str(),m_offsetVec.size(),m_tsNameVec.size());
#endif
		return false;
	}
	if (m_tsNameVec.size() == 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CM3u8::Init m_tsNameVec.size()==0\n");
#endif
		return false;
	}
	if (m_offsetVec.size() > 0 && nTotalDuration > 0)
	{
		unsigned int iTotalDuration = nTotalDuration/1000;			// ms->s
		if (iTotalDuration > 0)
		{
			m_iTsAverageRate = m_offsetVec[m_offsetVec.size()-1]*8/iTotalDuration;
		}
	}
	//查找已经下载完的ts，记录到m_pBitField
	m_pBitField = new char[(m_tsNameVec.size()+7)/8];
    memset(m_pBitField, 0, (m_tsNameVec.size()+7)/8);

#ifdef QVODDEBUG
	dwTick = QvodGetTime();
#endif
	const int nSize = m_tsNameVec.size();
	const int nTsDurSize = m_tsDurationVec.size();
	
	m_setExistIndex.clear();
	// 检测已下载的ts片
	vector<string> vFileNames;
	if (QVOD_OK == QvodGetFilesInFolder(m_strTsPath.c_str(), vFileNames))
	{
		CopyOldMapFiles(vFileNames);		// 先将已映射的切片文件改名

		int nSize = vFileNames.size();
		for (int i=0; i<nSize; i++)
		{
			int iIndex = GetTsIndexByFileName(vFileNames[i]);
			if (iIndex < 0)
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "GetTsIndexByFileName fail! name=%s", vFileNames[i].c_str());
#endif
				continue;
			}

			qvod_stat_t buffer;
			if(0 == QVOD_STAT((m_strTsPath+vFileNames[i]).c_str(), &buffer))
			{
				if (0 == buffer.st_size)	// 文件为0需删除
				{
#ifdef QVODDEBUG
					_OUTPUT(ERROR1,"CM3u8::Init file is zeor need del %s\n", (m_strTsPath+vFileNames[i]).c_str());
#endif
					QvodRemoveFile((m_strTsPath+vFileNames[i]).c_str());
					continue;
				}
				if (iIndex < m_tsSizeVec.size() && m_tsSizeVec[iIndex]!=buffer.st_size)
				{
#ifdef QVODDEBUG
					_OUTPUT(INFO, "CM3u8::Init ts filesize not match,need adjust index:%d before size:%lld after:%lld", iIndex, m_tsSizeVec[iIndex], buffer.st_size);
#endif
					m_tsSizeVec[iIndex] = buffer.st_size;
				}
				if (2 == SetBitField(iIndex))
				{
					m_setExistIndex.insert(iIndex);

					m_iTotalDownloadSize += buffer.st_size;				
					if (iIndex < nTsDurSize)
					{
						m_iTotalDownloadDuration += m_tsDurationVec[iIndex];					
					}
				}
			}
			else
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "M3u8 Init QVOD_STAT fail path=%s",(m_strTsPath+vFileNames[i]).c_str());
#endif
			}
		}

#ifdef QVODDEBUG
		if (! m_tsNameIndexMapOld.empty())
		{
			string strRet;
			char szTmp[10] = {0};
			for (std::set<int>::iterator itr=m_setExistIndex.begin(); itr!=m_setExistIndex.end(); itr++)
			{
				strRet += itoa((*itr), szTmp, 10);
				strRet += " ";
			}
			_OUTPUT(INFO, "M3u8 Init exist index: %s", strRet.c_str());
		}
#endif
	}
	else
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "M3u8 Init  open dir fail! ");
#endif
	}
	if (! bLoad)
	{
		set<int>::reverse_iterator itr = m_setExistIndex.rbegin();
		if (itr!=m_setExistIndex.rend())
		{
			m_nReseveMaxIndex = *itr;
		}
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "m3u8 init ticket=%d ts count=%d exit=%d isFinish=%d\n", QvodGetTime()-dwTick, nSize, vFileNames.size(), IsFinish(true));
#endif

	//若没有所有ts的偏移位置，则根据现有的ts文件来计算平均码率
	unsigned int iTotalDownloadDuration = m_iTotalDownloadDuration/1000;		// ms->s
	if (m_offsetVec.size() == 0 && m_iTotalDownloadSize > 0 && iTotalDownloadDuration > 0)
	{
		m_iTsAverageRate = m_iTotalDownloadSize*8/iTotalDownloadDuration;
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO,"m3u8 total download = %lld,total download duration = %d,average rate = %d bps\n",m_iTotalDownloadSize,
		iTotalDownloadDuration,m_iTsAverageRate);
#endif

	if (m_bReadM3u8 && !m_tsNameIndexMap.empty())
	{
		m_bReadM3u8 = false;
		m_tsNameIndexMapOld = m_tsNameIndexMap;		// 保存推给播放引擎的ts url
	}
	return true;
}

bool CM3u8::IsExist()
{
    CAutoLock lock(m_lock);
	std::string file = g_strtorrentpath + m_strM3u8Name + ".m3u8"; 
	bool bRet = QVOD_ACCESS( file.c_str(),0) != -1;
	return bRet;
}

bool CM3u8::IsFinish(bool bCheckFileExist)
{
    CAutoLock lock(m_lock);
	if (!m_pBitField || m_tsNameVec.size() == 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "IsFinish false");
#endif
		return false;
	}
	unsigned int i = 0;
	const unsigned int nTsCount = m_tsNameVec.size();
	for (;i < nTsCount/8; ++i)
	{
        unsigned char bit = (unsigned char)m_pBitField[i];
		if (bit != 0xFF)
		{
			return false;
		}
	}
	if (nTsCount%8 != 0)
	{
		i *= 8;
		for (;i < m_tsNameVec.size(); ++i)
		{
			if (!CheckIndex(i))
			{
				return false;
			}
		}
	}

	if (bCheckFileExist)		// 检测每个ts片文件是否存在，耗时操作，非必要不要调用
	{
#ifdef QVODDEBUG
		QVOD_DWORD dwTick = QvodGetTime();
#endif
		for (unsigned int i=0; i<nTsCount; i++)
		{
			if (! CheckIndex(i))
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CM3u8::IsFinish check ts file not exist! index=%d, tick=%d", i, QvodGetTime()-dwTick);
#endif
				return false;
			}
		}
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8::IsFinish check all ts file exist! tick=%d", QvodGetTime()-dwTick);
#endif
	}

	return true;
}

bool CM3u8::Load()
{
    CAutoLock lock(m_lock);
	std::string file = g_strtorrentpath + m_strM3u8Name + ".m3u8"; 
	FILE *fp;
	fp = fopen(file.c_str(), "rb");
	if(NULL == fp)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CM3u8::Load can not find fn: %s\n", file.c_str());
#endif
		return false;
	}
#ifdef QVODDEBUG
	_OUTPUT(ERROR1, "CM3u8::Load  file: %s\n", file.c_str());
#endif
	qvod_stat_t buffer;
	if(0 != QVOD_STAT(file.c_str(), &buffer))
	{
		fclose(fp);
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CM3u8::Load stat fail: %s\n", file.c_str());
#endif
		return false;
	}
	if ( 0 == buffer.st_size )
	{
		fclose(fp);
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CM3u8::Load stat fail: %s buffer.st_size=0\n", file.c_str());
#endif
		return false;
	}
	QPtr::AutoArrayPtr<char> pBuf = new char[buffer.st_size+1];
	memset(pBuf, 0, buffer.st_size+1);
	int iRead = fread(pBuf, 1, buffer.st_size, fp);
	if (iRead != buffer.st_size)
	{
		fclose(fp);
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CM3u8::Load stat fail: %s buffer.st_size=%d iRead=%d\n", file.c_str(), buffer.st_size, iRead);
#endif
		return false;
	}
	fclose(fp);
	std::string strBuf = (const char*)pBuf;
	pBuf = NULL;
	if (!Init(strBuf))
	{
		Clear();
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CM3u8::Load stat fail: %s Init\n", file.c_str());
#endif
		return false;
	}
	return true;
}

bool CM3u8::Save( const char *buf, int len )
{
    CAutoLock lock(m_lock);
	if (!buf || len <= 0)
	{
		return false;
	}
	std::string strBuf(buf, len);
	if (!Init(strBuf, false))
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CM3u8::Save Init failed!");
#endif
		Clear();
		return false;
	}
	m_strM3u8FileContent = strBuf;
	if (access(g_strtorrentpath.c_str(),0) != 0)
	{
		QvodMkdir(g_strtorrentpath.c_str());
	}
	string strFilePath = g_strtorrentpath + m_strM3u8Name + ".m3u8";
	FILE* fp = fopen(strFilePath.c_str(),"wb");
	if (!fp)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CM3u8::Save fopen %s error = %d\n",strFilePath.c_str(),errno);
#endif
		return false;
	}
	int iWrite = fwrite(strBuf.c_str(),1,strBuf.length(),fp);
	if (iWrite != strBuf.length())
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CM3u8::Save %s fwrite %d != %d,error = %d\n",strFilePath.c_str(),iWrite,len,errno);
#endif
		fclose(fp);
		return false;
	}
	fclose(fp);
#ifdef QVODDEBUG
	_OUTPUT(INFO,"CM3u8::Save end len=%d path=%s\n", len, strFilePath.c_str());
#endif
	return true;
}

bool CM3u8::Remove()
{
	string strFilePath = g_strtorrentpath + m_strM3u8Name + ".m3u8";
	if ( access(strFilePath.c_str(),0) != -1 )
	{
		if(remove(strFilePath.c_str()) == 0)
		{
			return true;
		}
	}
	return false;
}

void CM3u8::Clear()
{
    CAutoLock lock(m_lock);
	m_iTsDuration = 0;
	m_iTsAverageRate = 0;
	m_iTotalDownloadSize = 0;
	m_iTotalDownloadDuration = 0;
	//m_iPlayTsIndex = 0;
	m_iPlayPreTsIndex = 0;
	m_iCurPlayTime = 0;
	//m_iCurPlayIndex = 0;
	m_offsetVec.clear();
	m_tsSizeVec.clear();
	m_tsNameVec.clear();
	m_tsFileNameVec.clear();
	m_tsDurationVec.clear();
	m_tsOffsetDur.clear();
	m_tsNameIndexMap.clear();
	m_playTsIndexVec.clear();
	m_strM3u8FileContent.clear();
	m_tsHashVec.clear();

	SafeDeleteArray(m_pBitField);
	ClearM3u8TsData();
}

int CM3u8::SetBitField( unsigned int index )
{
    CAutoLock lock(m_lock);
	if(index < 0 || index >= m_tsNameVec.size() || !m_pBitField)
	{
		return 0;
	}
	if (BITTEST(m_pBitField, index) > 0) 	//已经下载了
	{
		return 1;
	}
	BITSET(m_pBitField, index);
	return 2;
}

bool CM3u8::CheckIndex( unsigned int index )
{
    CAutoLock lock(m_lock);
	if(index < 0 || index >= m_tsNameVec.size() || !m_pBitField)
	{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CheckIndex false! ");
#endif
		return false;
	}

	bool bRet = BITTEST(m_pBitField, index)==0?false:true;
	if (bRet)		// 检测该片文件是否存在
	{
        string strTsPath = m_strTsPath + m_tsFileNameVec[index];
		qvod_stat_t buffer;
		if(0!=QVOD_STAT(strTsPath.c_str(), &buffer) || 0==buffer.st_size)	// edit by weilijian,此处判断可能会降低效?
		//if (-1 == access(strTsPath.c_str(), 0))			
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO, "CheckIndex false! path=%s", strTsPath.c_str());
#endif
			BITCLEAR(m_pBitField, index);	// 重置该片下载位置
			bRet = false;
		}
	}

	return  bRet;
}

bool CM3u8::CheckTsSign(unsigned int index,const char* buf,int len)
{
	bool bRet = true;
	CAutoLock lock(m_lock);
	if (index < m_tsHashVec.size())
	{
		string strHash = m_tsHashVec[index];
		if (! strHash.empty())
		{
			unsigned char md5[20] = {0};
			//MD5_CTX ctx;
			//MD5_Init(&ctx);
			//MD5_Update(&ctx, (const void*)buf, len);
			//MD5_Final(md5,  &ctx);
			//string strMd5 = Hash2Char(md5, MD5_DIGEST_LENGTH);	// CMD5Checksum::GetMD5((QVOD_BYTE*)buf, len); 因在android下计算的md5是错误的

			MD5((const unsigned char*)buf, len, md5);
			string strMd5 = Hash2Char(md5, MD5_DIGEST_LENGTH);
            ::transform(strMd5.begin(), strMd5.end(), strMd5.begin(), ::tolower);
			if (0 != QvodStrICmp(strHash.c_str(), strMd5.c_str()))
			{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "CM3u8::CheckTsSign error! md5 not right index=%d orgmd5=%s  calmd5=%s", index, strHash.c_str(), strMd5.c_str());
#endif
				bRet = false;

				Json::Value root;
				Json::Value data;
				root["hash"] = Hash2Char(m_hash.data);
				if (index < m_tsNameVec.size())
				{
					data["url"] = m_strM3u8PreUrl+m_tsNameVec[index];
				}
				std::string strHost, strParams;
				NetUrl::GetURLHost(m_strM3u8PreUrl, strHost, strParams);
				data["cdn_host"] = strHost;
				data["cdn_ip"] = ParseHostIp(strHost.c_str());	
				data["sign_md5"] = strHash;
				data["real_md5"] = strMd5;
				if (index < m_tsSizeVec.size())
				{
					data["sign_size"] = m_tsSizeVec[index];
				}
				data["real_size"] = len;
				data["index"] = index;	
				root["data"] = data;

				Json::FastWriter writer;
				std::string strInfo = writer.write(root);
				CallBackToUI(NOTIFY_SIGN_ERROR, 0,  strInfo.c_str(), strInfo.length());		// 将签名错误信息回调给上层

				if (index < m_tsSizeVec.size())
				{
					if (m_tsSizeVec[index] != len)		// 效正该片大小
					{
						m_tsSizeVec[index] = len;
					}
				}
			}
		}		
	}
	return bRet;
}
int CM3u8::WriteTs( unsigned int index,const char* buf,int len )
{
    CAutoLock lock(m_lock);
	if (index >= m_tsNameVec.size() || !buf || len <= 0)
	{
#ifdef QVODDEBUG
        _OUTPUT(ERROR1,"CM3u8::WriteTs error! %d param error",__LINE__);
#endif
		return -1;
	}
	if (0 == m_iTsAverageRate && index < m_tsDurationVec.size())
	{
		unsigned int tsDuration = m_tsDurationVec[index]/1000;
		if (tsDuration)
		{
			m_iTsAverageRate = len*8/ tsDuration;			// 以防除0异常
		}
	}

    if (access(m_strTsPath.c_str(), 0) != 0)
    {
        QvodMkdir(m_strTsPath.c_str());
    }

	// 对ts片数据进行签名处理
	if (! CheckTsSign(index, buf, len))
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "CM3u8::WriteTs error! md5 not right index=%d ", index);
#endif
		//return -1;		// 暂时只进行上报处理
	}

	string strTsPath = m_strTsPath + m_tsFileNameVec[index];
	string strTsTempPath = strTsPath+".!mv";
	FILE* fp = fopen(strTsTempPath.c_str(),"wb");
	if (!fp)
	{
		int ret = -1;
		if (ENOSPC == errno)
		{
			ret = -3;
		}
#ifdef QVODDEBUG
        _OUTPUT(ERROR1,"CM3u8::WriteTs error! %d fopen %s error = %d\n",__LINE__,(strTsPath+".!mv").c_str(),errno);
#endif
		return ret;
	}
	int ret = 0;
	if (m_aesdata.ExistKey())		// 是否需要加密
	{
		QPtr::AutoArrayPtr<char> pBuf = new char[len+1];
		memcpy(pBuf, buf, len);
		for (int i=0; i<len/nAesPieceSize; i++)		// 每16KB分块加密
		{
			char *pos = pBuf+i*nAesPieceSize;
			m_aesdata.Encrpyt(pos, pos, nAesPieceSize);
		}
		ret = fwrite(pBuf,len,1,fp);
	}
	else
	{
		ret = fwrite(buf,len,1,fp);
	}

	if (ret != 1)
	{
		int ret = -1;
		if (ENOSPC == errno)
		{
			ret = -3;
		}
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CM3u8::WriteTs error! %d error = %d\n",__LINE__,errno);
#endif
		fclose(fp);
		QvodRemoveFile(strTsTempPath.c_str());		// 写失败删除临时文件
		return ret;
	}
	fclose(fp);

	if (QVOD_ACCESS(m_strTsPath.c_str(), 0) == 0)
	{
		//删除已下载文件
		QvodRemoveFile(strTsPath.c_str());
	}

	if (rename(strTsTempPath.c_str(),strTsPath.c_str()) != 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CM3u8::WriteTs error! %d error = %d\n",__LINE__,errno);
#endif
		return -1;
	}

	qvod_stat_t buffer;
	if(0==QVOD_STAT(strTsPath.c_str(), &buffer) && 0==buffer.st_size)		// 检验文件大小不为0
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"CM3u8::WriteTs error! %d error = %d\n",__LINE__,errno);
#endif
		//删除已下载文件
		QvodRemoveFile(strTsPath.c_str());
		return -1;
	}

	int bit = SetBitField(index);
	if (2 == bit)
	{
		m_iTotalDownloadSize += len;
		if (index < m_tsDurationVec.size())
		{
			m_iTotalDownloadDuration += m_tsDurationVec[index];
			//非p2p的m3u8，没有各个ts的大小，在每得到一个ts之后计算平均码率
			int iTotalDuration = m_iTotalDownloadDuration/1000;
			if (m_offsetVec.size() == 0 && iTotalDuration > 0)
			{
				m_iTsAverageRate = m_iTotalDownloadSize*8/iTotalDuration;
	#ifdef QVODDEBUG
				_OUTPUT(INFO,"Adjust m3u8 average rate %d bps\n",m_iTsAverageRate);
	#endif
			}
		}
	}
#ifdef QVODDEBUG
	else if (1 == bit)
	{
		_OUTPUT(INFO, "CM3u8::WriteTs duplicate index=%d ", index);
	}
#endif

	
	return len;
}

QVOD_INT64 CM3u8::GetGlobalOffset( unsigned int index )
{
    CAutoLock lock(m_lock);
	if (index >= m_offsetVec.size())
	{
		return -1;
	}
	QVOD_INT64 iOffset = 0;
	if (index > 0)
	{
		iOffset = m_offsetVec[index-1];
	}
	return iOffset;
}


QVOD_INT64 CM3u8::GetAllTsSize()
{
    CAutoLock lock(m_lock);
	if (m_offsetVec.size() > 0)
	{
		return m_offsetVec[m_offsetVec.size()-1];
	}
	return 0;
}

std::string CM3u8::GetUnhaveTsFromIndex( unsigned int& index )
{
    CAutoLock lock(m_lock);
	if(index < 0 || index >= m_tsNameVec.size() || !m_pBitField)
	{
		return string("");
	}
	for (;index < m_tsNameVec.size();++index)
	{
		if (!CheckIndex(index))
		{
			return m_tsNameVec[index];
		}
	}
	return string("");
}

string CM3u8::GetTsName(const unsigned int& index)
{
	CAutoLock lock(m_lock);
	if(index >= m_tsNameVec.size())
	{
		return string("");
	}
	return m_tsNameVec[index];
}

void CM3u8::SetPlayingTs( string strTsName )
{
	CAutoLock lock(m_lock);
	const int iIndex = GetTsIndex(strTsName);
	if (iIndex >= 0)
	{
		if (m_bReadM3u8 && !m_tsNameIndexMap.empty())
		{
			m_bReadM3u8 = false;
			m_tsNameIndexMapOld = m_tsNameIndexMap;		// 保存推给播放引擎的ts url
		}
		m_iPushTsIndex = iIndex;
		m_iPlayPreTsIndex = m_iPlayTsIndex;
		m_iPlayTsIndex = iIndex;
		if (! m_bCacheTimeCalBySetTime)
		{
			m_iCurPlayIndex = m_iPlayTsIndex;
		}
		while (m_playTsIndexVec.size() >= MAX_RECORD_CONTINUE_PLAY_TS_NUM)
		{
			m_playTsIndexVec.erase(m_playTsIndexVec.begin());
		}
		m_playTsIndexVec.push_back(m_iPlayTsIndex);
		
		// 删除m_iPlayTsIndex之前的ts片及ts后设定的缓存时长后的数据
		if (m_bSpaceMode)		// 节约空间模式
		{
			int nIndex = m_iCurPlayIndex;
			if (! IsPlayTsContinueIn2Ts())	// 推流在不连续的片，则认为有seek操作, 需要删除m_iPlayTsIndex位置前后数据
			{
				nIndex = m_iPlayTsIndex;
				m_iCurPlayIndex = m_iPlayTsIndex;
			}
			ClearHasPlayingTsData(nIndex);
		}
	}
	else if (string::npos != strTsName.find(".m3u8"))
	{
		m_bPushM3u8 = true;
		m_iCurPlayIndex = m_iPlayTsIndex = 0;
		m_iPushTsIndex = -1;
		m_bReadM3u8 = true;
		m_tsNameIndexMapOld.clear();		// 每一次播放清空旧的tsurl
	}
}

void CM3u8::ResetPlayingTs(const string& strTsName)
{
	CAutoLock lock(m_lock);
	m_iPushTsIndex = -2;
}

int CM3u8::GetPushIndex()
{
	CAutoLock lock(m_lock);
	return m_iPushTsIndex;
}
bool CM3u8::IsPlayTsContinue()
{
    CAutoLock lock(m_lock);
	vector<int> vec = m_playTsIndexVec;
	if (vec.size() < MAX_RECORD_CONTINUE_PLAY_TS_NUM)
	{
		return false;
	}
	vector<int>::iterator iter = vec.begin();
	int iTsIndex = *iter;
	for (++iter; iter != vec.end(); ++iter)
	{
		if (++iTsIndex != *iter)
		{
			return false;
		}
	}
	return true;
}

bool CM3u8::IsPlayTsContinueIn2Ts()
{
	return (m_iPlayTsIndex == m_iPlayPreTsIndex+1);
}

void CM3u8::ClearPlayingTsVec()
{
    CAutoLock lock(m_lock);
	m_playTsIndexVec.clear();
}

int CM3u8::GetTsIndex( string strTsName )
{
	int pos = strTsName.find("?") ;		// 去掉url后面参数
	if ( pos != -1)
	{
		strTsName = strTsName.substr(0,pos) ;
	}

    CAutoLock lock(m_lock);
    map<string,int>::iterator iter2 = m_tsNameIndexMap.find(strTsName);
    if (iter2 != m_tsNameIndexMap.end())
    {
        return iter2->second;
    }
    iter2 = m_tsNameIndexMapOld.find(strTsName);	// 映射已推流给播放引擎的m3u8
    if (iter2 != m_tsNameIndexMapOld.end())
    {
        return iter2->second;
    }
	return -1;
}

int CM3u8::GetTsIndexByFileName(string strTsFileName)
{
	CAutoLock lock(m_lock);
	const int nSize = m_tsFileNameVec.size();
	for (int i=0; i<nSize; i++)
	{
		if (m_tsFileNameVec[i] == strTsFileName)
		{
			return i;
		}
	}
	//vector<string>::iterator itr = std::find(m_tsFileNameVec.begin(), m_tsFileNameVec.end(), strTsFileName);
	//if (itr != m_tsFileNameVec.end())
	//{
	//	return itr - itr->begin();
	//}
   
	return -1;
}

int CM3u8::GetTsUrlIndex(const string& strTsUrl)
{
	CAutoLock lock(m_lock);
	
	const char *pFileUrl = strrchr(strTsUrl.c_str(), '/');
	if (pFileUrl)
	{
			return GetTsIndex(pFileUrl+1);
	}
	return -1;
}

QVOD_INT64 CM3u8::GetM3u8Size()
{
	CAutoLock lock(m_lock);
	if (! m_strM3u8FileContent.empty())
	{
		return m_strM3u8FileContent.length();
	}
	std::string file = g_strtorrentpath + m_strM3u8Name + ".m3u8"; 
	qvod_stat_t buffer;
	if(0 != QVOD_STAT(file.c_str(), &buffer))
	{
//#ifdef QVODDEBUG
//		_OUTPUT(FAIL, "CM3u8::GetM3u8Size stat fail: %s\n", file.c_str());
//#endif
		return 0;
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8::GetM3u8Size %d", buffer.st_size);
#endif
	return buffer.st_size;
}

QVOD_INT64 CM3u8::GetFileSize(const string& strFilePath)
{
	qvod_stat_t buffer;
	if(0 == QVOD_STAT(strFilePath.c_str(), &buffer))
	{
		return buffer.st_size;
	}
	return 0;
}

QVOD_INT64 CM3u8::GetTsSize( string strTsName )
{
    CAutoLock lock(m_lock);
	int index = GetTsIndex(strTsName);
	if (index < 0)
	{
		return 0;
	}
	if (! CheckIndex(index))
	{
		return 0;
	}
	if (m_offsetVec.size() == 0)
	{
		if (index <m_tsFileNameVec.size())
		{
			string strTsPath = m_strTsPath + m_tsFileNameVec[index];
			QVOD_INT64 size = GetFileSize(strTsPath);
			return size;
		}
	}
	else
	{
		if (index >= 0 && index < m_tsSizeVec.size())
		{
			return m_tsSizeVec[index];
		}
		else if (index >= 0 && index < m_offsetVec.size())
		{
			if (0 == index)
			{
				return m_offsetVec[0];
			}
			else
			{
				return m_offsetVec[index]-m_offsetVec[index-1];
			}
		}
	}
	return 0;
}

QVOD_INT64 CM3u8::GetTsOffset(const unsigned int& index)
{
	CAutoLock lock(m_lock);
	if (index >= m_offsetVec.size())
	{
		return 0;
	}
	return m_offsetVec[index];
}

QVOD_INT64 CM3u8::GetTsSize(const unsigned int& index)
{
	CAutoLock lock(m_lock);
	if (index >= m_tsSizeVec.size())
	{
		string strTsName = GetTsName(index);
		return GetTsSize(strTsName);
	}
	return m_tsSizeVec[index];
}

int CM3u8::FindTsByOffset( QVOD_INT64 offset )
{
    CAutoLock lock(m_lock);
	int size = m_offsetVec.size();
	if ( 0 == size || offset > m_offsetVec[size-1])
	{
		return -1;
	}
	//使用二分查找算法来找
	int iBeginIndex = 0,iEndIndex = size-1,iMiddleIndex = size/2;
	int index = -1;
	do 
	{
		if (offset == m_offsetVec[iMiddleIndex])
		{
			index = iMiddleIndex;
			break;
		}
		if (offset < m_offsetVec[iMiddleIndex])
		{
			if (iMiddleIndex > 0 )
			{
				if (offset > m_offsetVec[iMiddleIndex-1])
				{
					index = iMiddleIndex;
				}
				else
				{
					iEndIndex = iMiddleIndex-1;
					iMiddleIndex = (iEndIndex+iBeginIndex)/2;
				}
			}
			else
			{
				index = iMiddleIndex;
			}
		}
		else	//offset > m_offsetVec[iMiddleIndex]
		{
			if (iMiddleIndex < size-1)
			{
				if (offset <= m_offsetVec[iMiddleIndex+1] )
				{
					index = iMiddleIndex+1;
					break;
				}
				else
				{
					iBeginIndex = iMiddleIndex+1;
					iMiddleIndex = (iEndIndex+iBeginIndex)/2;
				}
			}
		}
	} while (index == -1);
#ifdef QVODDEBUG
	_OUTPUT(INFO,"FindTsByOffset offset = %lld,TsOffset = %lld idx=%d\n",offset,m_offsetVec[index], index);
#endif
	return index;
}

int CM3u8::ReadM3u8( char* buf,int start,int len )
{
	if (! m_strM3u8FileContent.empty())
	{
		const int nLen = m_strM3u8FileContent.length();
		if (start < nLen)
		{
			const int nMaxLen = (start+len) < nLen? len: (nLen-start);
			memcpy(buf, m_strM3u8FileContent.c_str()+start, nMaxLen);
			return nMaxLen;
		}
	}
	std::string file = g_strtorrentpath + m_strM3u8Name + ".m3u8"; 
	if (access(file.c_str(),0) != 0)
	{
		return -1;
	}
	FILE* fp = fopen(file.c_str(),"rb");
	if (!fp)
	{
		return -1;
	}

	//文件没有加载
	if ( m_tsNameVec.size() <= 0 )
	{
		const QVOD_INT64 i64Len = GetM3u8Size() ;
		if ( i64Len > 0 )
		{
			QPtr::AutoArrayPtr<char> pM3u8 = new char[i64Len+1] ;
			fread(pM3u8,1,i64Len,fp) ;
			pM3u8[i64Len] = '/0' ;
			string strBuf = (char*)pM3u8;
			pM3u8 = NULL;
			Init(strBuf);
			//Init(pM3u8, i64Len) ;
		}
	}


	fseek(fp, start, SEEK_SET);
	int iRead = fread(buf, 1, len, fp);
	fclose(fp);

	return iRead;
}

int CM3u8::ReadTs( string strTsName,char* buf,int start,int len )
{
	CAutoLock lock(m_lock);
	std::string strFileName;
	int index = GetTsIndex(strTsName);
	if (index < 0)
	{
		return -1;
	}
	if (!CheckIndex(index))
	{
		if (0 == m_iPlayTsIndex)
		{
			m_iPlayTsIndex = index;
			if (! m_bCacheTimeCalBySetTime)
			{
				m_iCurPlayIndex = m_iPlayTsIndex;
			}
		}

		return -1;
	}		

	string strTs = m_strTsPath + m_tsFileNameVec[index] ;
	if (access(strTs.c_str(),0) != 0)
	{
		return -1;
	}

	FILE* fp = fopen(strTs.c_str(),"rb");
	if (!fp)
	{
		return -1;
	}

	int iRead = 0;
	int nStart = start;
	int nLen = len;
	const int nStartMod = start%nAesPieceSize;
	if (m_aesdata.ExistKey())
	{
		nStart = start - nStartMod;
		nLen += nStartMod;
		if (nLen % nAesPieceSize)
		{
			nLen = nLen - nLen%nAesPieceSize+nAesPieceSize;
		}
	}

	if (nStart != start || nLen != len)
	{
		QPtr::AutoArrayPtr<char> pBuf = new char[nLen];
		fseek(fp, nStart, SEEK_SET);
		iRead = fread(pBuf, 1, nLen, fp);
		fclose(fp);

		for (int i=0; i<iRead/nAesPieceSize; i++)
		{
			char *pos = pBuf+i*nAesPieceSize;
			m_aesdata.Decrpyt(pos, pos, nAesPieceSize);
		}
		if (len > iRead-nStartMod)
		{
			len = iRead-nStartMod;
		}
		//len = len <= iRead-nStartMod? len: iRead-nStartMod;
		if (len > 0)
		{
			memcpy(buf, pBuf+nStartMod, len);
			iRead = len;
		}
	}
	else
	{
		fseek(fp, nStart, SEEK_SET);
		iRead = fread(buf, 1, nLen, fp);
		fclose(fp);

		if (m_aesdata.ExistKey())
		{
			for (int i=0; i<iRead/nAesPieceSize; i++)
			{
				char *pos = buf+i*nAesPieceSize;
				m_aesdata.Decrpyt(pos, pos, nAesPieceSize);
			}		
		}
	}

    return iRead;
}

unsigned int CM3u8::GetDuration()
{
	CAutoLock lock(m_lock);
	if (m_tsOffsetDur.size() == 0)
	{
		return 0;
	}
	return m_tsOffsetDur[m_tsOffsetDur.size()-1];
}

unsigned int CM3u8::GetTsCount()
{
	CAutoLock lock(m_lock);
	if (m_tsNameVec.size() == 0)
	{
		return 0;
	}
	return m_tsNameVec.size();
}
int CM3u8::GetContinueTsNum(const unsigned int& nIndex)
{
	if (!m_pBitField)
	{
		return 0;
	}
	CAutoLock lock(m_lock);
	int num = 0;
	unsigned int index = nIndex;
	for (;index < m_tsNameVec.size();++index)
	{
		if (CheckIndex(index))
		{
			++num;
		}
		else
		{
#ifdef QVOD_USE_CACHEMEM
			if (!CCacheDataInMem::Instance().CheckM3u8Ts(m_hash, m_tsNameVec[index]))
			{
				break;
			}
			++num;
#else
			break;
#endif
		}
	}
	return num;
}

unsigned int CM3u8::GetContinueTsTimeByIndex(const unsigned int& nIndex)
{
	if (!m_pBitField)
	{
		return 0;
	}
	CAutoLock lock(m_lock);
	//int iPlayTsDur = GetPrevTsDuration(m_iPlayTsIndex);
	//int time = iPlayTsDur > m_iCurPlayTime? iPlayTsDur-m_iCurPlayTime: 0;
	unsigned int time = 0;
	unsigned int index = nIndex;
	const unsigned int nSize = m_tsNameVec.size();
	for (;index < nSize;++index)
	{
		if (CheckIndex(index))
		{
			time += m_tsDurationVec[index];
		}
		else 
		{
#ifdef QVOD_USE_CACHEMEM
			if (!CCacheDataInMem::Instance().CheckM3u8Ts(m_hash, m_tsNameVec[index]))
			{
				break;
			}
			time += m_tsDurationVec[index];
#else
			break;
#endif
		}
	}
	return time;
}

unsigned int CM3u8::GetContinueTsTime(const unsigned int& timepoint)
{
	CAutoLock lock(m_lock);
	if (!m_pBitField)
	{
		return 0;
	}

	int nIndex = GetTsIndexByDuration(timepoint);
	if (nIndex < 0)
	{
		return 0;
	}
	nIndex += GetContinueTsNum(nIndex+1); // 注意是从下一片开始
	
	unsigned int time = GetPrevTsDuration(nIndex) - timepoint;
	return time;
}

void CM3u8::SaveM3u8Data( SM3u8TsSurplusData& data )
{
	CAutoLock lock(m_m3u8TsDataLock);
	for (vector<SM3u8TsSurplusData>::iterator iter = m_m3u8TsData.begin(); iter != m_m3u8TsData.end(); ++iter)
	{
		if (iter->start == data.start)		// 删除已存在该片位置的数据
		{
			SafeDeleteArray(iter->buf);
			iter = m_m3u8TsData.erase(iter);
			break;
		}

		if (m_m3u8TsData.size() >= 10)
		{
#ifdef QVODDEBUG
			_OUTPUT(INFO,"Erase M3u8Data start = %lld,len = %d\n",iter->start,iter->len);
#endif
			SafeDeleteArray(iter->buf);
			iter = m_m3u8TsData.erase(iter);
		}
	}
	m_m3u8TsData.push_back(data);
#ifdef QVODDEBUG
	_OUTPUT(INFO,"SaveM3u8Data start = %lld,len = %d\n",data.start,data.len);
#endif
}

void CM3u8::ClearM3u8TsData()
{
#ifdef QVODDEBUG
	_OUTPUT(INFO,"ClearM3u8TsData %d\n",m_m3u8TsData.size());
#endif
	CAutoLock lock(m_m3u8TsDataLock);
	for (vector<SM3u8TsSurplusData>::iterator iter = m_m3u8TsData.begin(); iter != m_m3u8TsData.end(); ++iter)
	{
		SafeDeleteArray(iter->buf);
	}
	m_m3u8TsData.clear();
}

void CM3u8::RemoveM3u8TsData(const QVOD_INT64& iStart)
{
	CAutoLock lock(m_m3u8TsDataLock);
	for (vector<SM3u8TsSurplusData>::iterator iter = m_m3u8TsData.begin(); iter != m_m3u8TsData.end(); ++iter)
	{
		if (iter->start == iStart)
		{
			SafeDeleteArray(iter->buf);
			m_m3u8TsData.erase(iter);
			break;
		}
	}
}
bool CM3u8::FindM3u8TsData(const QVOD_INT64& iStart, SM3u8TsSurplusData&data)
{
	bool bRet = false;

	CAutoLock lock(m_m3u8TsDataLock);
	for (vector<SM3u8TsSurplusData>::iterator iter = m_m3u8TsData.begin(); iter != m_m3u8TsData.end(); ++iter)
	{
		if (iter->start == iStart)
		{
			data = (*iter);
			bRet = true;		
			break;
		}
	}
	return bRet;
}

int CM3u8::ReadM3u8TsData(const QVOD_INT64& iStart, char *buf, const unsigned int& len)
{
	CAutoLock lock(m_m3u8TsDataLock);
	for (vector<SM3u8TsSurplusData>::iterator iter = m_m3u8TsData.begin(); iter != m_m3u8TsData.end(); ++iter)
	{
		if (iStart >= iter->start && iStart < (iter->start+iter->len))
		{
			unsigned int uMinLen = iter->len-(iStart-iter->start);
			uMinLen = uMinLen>len? len: uMinLen;
			memcpy(buf, iter->buf+iStart-iter->start, uMinLen);
			return uMinLen;
		}
	}
	return 0;
}

unsigned int CM3u8::GetPrevTsDuration(const unsigned int& nIndex)
{
	CAutoLock lock(m_lock);
	int nDur = 0;
	const unsigned int nSize = m_tsOffsetDur.size();
	if (nIndex >= nSize)
	{
		return nDur;
	}
	return m_tsOffsetDur[nIndex];
}

int CM3u8::GetTsIntervalDuration(const unsigned int& nBeginIndex, const unsigned int& nEndIndex)
{
	CAutoLock lock(m_lock);
	int nDur = 0;
	const unsigned int nSize = m_tsOffsetDur.size();
	if (nBeginIndex >= nSize || nEndIndex>=nSize)
	{
		return nDur;
	}

	nDur = m_tsOffsetDur[nEndIndex] - m_tsOffsetDur[nBeginIndex];
	return nDur;
}

int CM3u8::GetTsIndexByDuration(const unsigned int& nDuration)
{
	CAutoLock lock(m_lock);
	int idx = -1;
	const int nSize = m_tsOffsetDur.size();
	int i = 0;
	for (i=0; i<nSize; i++)			// 找到该时间点对应的ts片index
	{
		if (m_tsOffsetDur[i] >= nDuration)
		{
			idx = i;
			break;
		}
	}
	if (i >= nSize)
	{
		idx = nSize-1;
	}
	return idx;
}

int CM3u8::ClearTsData(unsigned int nBeginIndex, unsigned int nEndIndex)
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8::ClearTsData [%d-%d]", nBeginIndex, nEndIndex);
#endif
	CAutoLock lock(m_lock);
	const unsigned int nSize = m_tsDurationVec.size();
	if (nBeginIndex >= nSize)
	{
		return 0;
	}
	if (nEndIndex >= nSize)
	{
		nEndIndex = nSize-1;
	}
	int nDelSize = 0;
	int nTsDuration = 0;
	for (unsigned int index=nBeginIndex; index<=nEndIndex; )
	{
		if (0x0 == m_pBitField[index>>3])
		{
			index += 8;
			continue;
		}
        else if (index == m_iPlayTsIndex)   // 当前推流的片不能删除
        {
#ifdef QVODDEBUG
            _OUTPUT(INFO, "CM3u8::ClearTsData index == m_iPlayTsIndex %d", m_iPlayTsIndex);
#endif
        }
		else if (CheckIndex(index))
		{
            
			string strTsFile = m_strTsPath + m_tsFileNameVec[index] ;
			if (access(strTsFile.c_str(),0) == 0)
			{
				qvod_stat_t buffer;
				QVOD_STAT(strTsFile.c_str(), &buffer);
				if (QVOD_OK == QvodRemoveFile(strTsFile.c_str()))
				{
					BITCLEAR(m_pBitField, index);	// 重置该片下载位置

					nDelSize += buffer.st_size;
					nTsDuration += m_tsDurationVec[index];
				}
			}
		}
		index ++;
	}

	m_iTotalDownloadSize -= nDelSize;
	m_iTotalDownloadDuration -= nTsDuration;
	return nDelSize;
}

int CM3u8::ClearHasPlayingTsData(QVOD_UINT64 iCurCacheSize)	// 删除当前播放位置前的ts片，返回删除的大小
{
	CAutoLock lock(m_lock);
	int nDelSize = 0;

	const int nSize = m_tsFileNameVec.size();
	nDelSize += ClearTsData(1, m_iPlayTsIndex-1);	// 保留第一片

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8::ClearHasPlayingTsData() size=%d", nDelSize);
#endif

	if (iCurCacheSize > nDelSize)		// 删除播放点前数据后空间仍不足，则需要删除当前播放点后40s的数据
	{
		int nEndIndx = m_iPlayTsIndex+2;
		if (m_iTsDuration)
		{
			nEndIndx += 30 / m_iTsDuration;
		}
		nDelSize += ClearTsData(nEndIndx, nSize-1);

		if (iCurCacheSize > nDelSize)		// 删除播放点后30s数据后空间仍不足，则需要删除除当前片所有的片
		{
			nDelSize += ClearTsData(m_iPlayTsIndex+1, nEndIndx-1);
		}
	}
	return nDelSize;
}

void CM3u8::ClearReseveTs()
{
	if (m_nReseveMaxIndex<=0)
	{
		return;
	}
	CAutoLock lock(m_lock);

	if (m_iPlayTsIndex < m_nReseveMaxIndex)			// 切换清晰度后，从后向前依次删除已下载的上个清晰度ts片，以缩短清晰度切换时间
	{
#ifdef QVODDEBUG
		_OUTPUT(INFO, "DownNextM3u8Ts m_iPlayTsIndex:%d m_nReseveMaxIndex:%d remove ", m_iPlayTsIndex, m_nReseveMaxIndex);
#endif
		int nRemoveIdx = m_nReseveMaxIndex;
		ClearTsData(nRemoveIdx, nRemoveIdx);
		m_setExistIndex.erase(nRemoveIdx);
		std::set<int>::reverse_iterator itr = m_setExistIndex.rbegin();
		if (itr != m_setExistIndex.rend())
		{
			m_nReseveMaxIndex = *itr;
		}
		else
		{
			m_nReseveMaxIndex = 0;
		}
	}
	else
	{
		m_nReseveMaxIndex = 0;
	}
}

void CM3u8::ClearHasPlayingTsData(int nIndex)
{
	CAutoLock lock(m_lock);

	const int nSize = m_tsFileNameVec.size();
	if (nIndex > 1)
	{
		ClearTsData(1, nIndex-1);													// 删除当前位置前的数据，保留第一片
	}

	//int iMaxCacheTsNum = (0 == m_iTsDuration?4:(g_cfg.m_lMaxCacheTime+m_iTsDuration-1)/m_iTsDuration);
	int nDuration = GetPrevTsDuration(nIndex);
	nDuration += g_cfg.m_lMaxCacheTime * 1000;		// s->ms
	int nEndIndex = GetTsIndexByDuration(nDuration);
	ClearTsData(nEndIndex+1,  nSize-1);		// 删除当前位置g_cfg.m_lMaxCacheTime后的数据
#ifdef QVODDEBUG
	_OUTPUT(INFO, "CM3u8::ClearHasPlayingTsData [%d-%d] [%d-%d] m_curplayindex=%d m_playidx=%d", 0, nIndex-1, nEndIndex+1, nSize-1, m_iCurPlayIndex, m_iPlayTsIndex);
#endif
}

void CM3u8::UpdateM3u8OrgUrl(const string& strHttpOrgUrl, bool bDownload, bool bPlaying, bool bChangeClarity)
{
	CAutoLock lock(m_lock);
	m_strM3u8Url = strHttpOrgUrl;

	string strPreUrl;
	int nPos = strHttpOrgUrl.rfind("/");
	if (nPos != string::npos)
	{
		strPreUrl = strHttpOrgUrl.substr(0, nPos+1);
	}
	SetM3u8PreUrl(strPreUrl);

	m_tsFileNameIndexMapOld.clear();

	try
	{
		if (! bDownload && bChangeClarity)      // 非离线清晰度有切换逻辑
		{
			if (bPlaying /*&& (m_iCurPlayIndex ==  m_iPlayTsIndex)*/)
			{
				// 当推流的片正在播放时，则延迟一个ts片切换
				m_nReseveMaxIndex = m_iPlayTsIndex+4;
				ClearTsData(1, m_iPlayTsIndex-1);
				ClearTsData(m_nReseveMaxIndex+1, -1);

				m_nReseveMaxIndex = m_nReseveMaxIndex >= m_tsFileNameVec.size()? m_tsFileNameVec.size()-1: m_iPlayTsIndex;
				for (int i=m_iPlayTsIndex; i<=m_nReseveMaxIndex; i++)		// 将当前几片文件名映射到index上
				{
					string strTsName = m_tsFileNameVec[i];
					m_tsFileNameIndexMapOld[strTsName] = i;
				}
			}
			else
			{
				ClearTsData(0, -1);
			}
		}
	}
	catch (...) {
#ifdef QVODDEBUG
		_OUTPUT(INFO, "catch CM3u8::UpdateM3u8OrgUrl");
#endif
	}

	
	Remove();				// 删除m3u8文件
	m_strM3u8FileContent.clear();
	//Clear();				// 清空m3u8属性
}

void CM3u8::SetPlayingTimepoint(const unsigned int& timepoint,bool bSeek)
{
	CAutoLock lock(m_lock);
	if (! m_bCacheTimeCalBySetTime)
	{
		m_bCacheTimeCalBySetTime = true;
	}

	m_iCurPlayTime = timepoint;
	m_iCurPlayIndex = GetTsIndexByDuration(timepoint);
}

void CM3u8::SetAesKey(unsigned char key[16])
{
	m_aesdata.SetKey(key);
}
string CM3u8::SignUrl(const std::string& strHttpUrl, const string& strAccessKeyID, const string& strAccessSecret, const string& strExpires)
{
	string strSignUrl = strHttpUrl;

	string strMethod = "GET";
	string strHost;				// 域名
	string strUrl;					// 相对路径
	string strParams;			// 参数

	const char *p = strstr(strHttpUrl.c_str(), "://");
	if (NULL == p)
	{
		return strSignUrl;
	}
	p += strlen("://");
	const char *p2 = strstr(p, "/");
	if (NULL == p2)
	{
		strHost = p;
	}
	else
	{
		strHost.assign(p, p2-p);
		p = p2;
		p2 = strstr(p, "?");
		if (NULL != p2)
		{
			strParams = p2+1;
			strUrl.assign(p, p2-p);			
		}
		else
		{
			strUrl = p;
		}
	}

	p = strstr(strHost.c_str(), ".");
	if (NULL == p)
	{
		return strSignUrl;
	}
	string strCanoicalizedResource = "/";		
	strCanoicalizedResource += strHost.substr(0, p-strHost.c_str());
	strCanoicalizedResource += strUrl;

	string strSign = strMethod;
	strSign += "\n\n\n";
	strSign += strExpires;
	strSign += "\n";
	strSign += strCanoicalizedResource;

#ifdef QVODDEBUG
	QVOD_DWORD dwTick = QvodGetTime();
#endif
	HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, strAccessSecret.c_str(), strAccessSecret.length(), EVP_sha1(), NULL);
	HMAC_Update(&ctx, (unsigned char*)strSign.c_str(), strSign.length());        // input is OK; &input is WRONG !!!

	unsigned char output[EVP_MAX_MD_SIZE] = {0};  
	unsigned int output_length = 0;
    HMAC_Final(&ctx, output, &output_length);
    HMAC_CTX_cleanup(&ctx);
	string strHmacSha1 = Base64::base64_encode((const char*)output, output_length);
	strHmacSha1 = Fun::URLEncode(strHmacSha1.c_str(), strHmacSha1.length());

#ifdef QVODDEBUG
	QVOD_DWORD dwTick2 = QvodGetTime();
	if (dwTick2 != dwTick)
	{
		_OUTPUT(INFO, "CM3u8::SignUrl tick=%d", dwTick2-dwTick);
	}
#endif
	strSignUrl += strParams.empty()? "?": "&";
	strSignUrl += "OSSAccessKeyId=";
	strSignUrl += strAccessKeyID;
	strSignUrl += "&Signature=";
	strSignUrl += strHmacSha1;
	strSignUrl += "&Expires=";
	strSignUrl += strExpires;

	return strSignUrl;
}


void CM3u8::CopyOldMapFiles(vector<string> &vFileNames)
{
	if (! m_tsNameIndexMapOld.empty() || ! m_tsFileNameIndexMapOld.empty())
	{
		for (unsigned int i=0; i<vFileNames.size(); i++)
		{
			int nIndex = -1;
			map<string,int>::iterator itr = m_tsNameIndexMapOld.find(vFileNames[i]);
			if (itr != m_tsNameIndexMapOld.end())
			{
				nIndex = itr->second;
			}
			else
			{
				itr = m_tsFileNameIndexMapOld.find(vFileNames[i]);
				if (itr != m_tsFileNameIndexMapOld.end())	// 从这里面找
				{
					nIndex = itr->second;
				}
			}
			if (nIndex >=0)
			{
				const string strTsName = vFileNames[i];
				const unsigned int nIndex = itr->second;
				if (nIndex < m_tsFileNameVec.size())
				{
					if ( strTsName != m_tsFileNameVec[nIndex])
					{
						string strTs = m_strTsPath + strTsName;
						string strNewTsPath = m_strTsPath+m_tsFileNameVec[nIndex];
						if (rename(strTs.c_str(), strNewTsPath.c_str()) == 0)							// 清晰度ts片文件名不一样，重命名到当前清晰度上来
						{
							vFileNames[i] = m_tsFileNameVec[nIndex];									// 修正vFileNames中的该片文件名
#ifdef QVODDEBUG
							_OUTPUT(ERROR1,"CM3u8::CopyOldMapFiles %s to %s \n",strTs.c_str(), strNewTsPath.c_str());
#endif				
						}
					}
				}
			}
			else
			{
#ifdef QVODDEBUG
				_OUTPUT(ERROR1,"CM3u8::CopyOldMapFiles has no filename %s! \n", vFileNames[i].c_str());
#endif
			}
		}
	}
	else
	{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1,"CM3u8::CopyOldMapFiles is empty! \n");
#endif			
	}
}