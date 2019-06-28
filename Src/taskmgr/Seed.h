#ifndef __QVOD_SEED_H__
#define __QVOD_SEED_H__

#ifdef WIN32 /* WIN32 */
#pragma warning(disable : 4786)
#else /* posix */
//nothing
#endif /* posix end */

#include <string>
#include <vector>
#include <map>
#include "qvod_lock.h"
#include "common-commonstruct.h"

using namespace std;

class CSeed
{
public:
	CSeed(const char *filename);

	~CSeed();

	//初始化种子文件有2中方式 1：直接读文件，2：网络传来
	bool Read();
	bool Read(const char *filename);

	//网络初始化一个种子
	bool Init(const char *buf, int len);
	bool Save();

	bool IsExist();
	bool Load();
	bool Remove();
	void Reset();

	bool ParseTorrentFile();

	string &GetDownFileName(void);
	string &GetTorrentFileName(void);
	QVOD_UINT64 GetDownFileSize(void);
	unsigned int GetPieceNum(void);
	unsigned int GetPieceSize(void);
	unsigned int GetCreateTime(void);
	unsigned int GetRrackerIP(void);
	unsigned short GetTrackerPort(void);
	bool CheckHash(unsigned int index, HASH hash);
	string  GetAnnounces(void);
	string& GetAdUrl(void);
	string& GetTextAdUrl(void);
	string& GetPauseAdUrl(void);
	unsigned char*GetTorrent(void);
	unsigned int GetTorrentLen(void);

	string GetMetafile() {return m_strSeedName; }
	char* GetPieceBuf(unsigned int index);
	QVOD_UINT64 GetFileTotleLen(){return m_downfilesize;}
	int GetFileNum(){return (m_vctfile.size()) ? ((int)m_vctfile.size()):1;}
	std::string GetFileName(unsigned int pos = 0);
	QVOD_UINT64 GetFileLen(unsigned int pos = 0){return m_vctfile.size() >pos?m_vctfile[pos].filelen:m_downfilesize;}
	unsigned int GetTrackerIP();
	char* GetInfoHash(){return m_infohash;}
	int GetInfoHashlen(){return m_infohashlen;}
	char *GetHashList() {return (char *)m_hash_list;}
	bool IsSeedParsed(){return m_bIsSeedParse;}

	//临时接口
	void SetFileLength(QVOD_UINT64 iLen);
private:
	char* parseFiles(char* start,char* end);
	char* parseFile(char* start,char* end);
	char* parseDictionary(char* start,char* end);
	char* parsePath(char* start,std::string &strpath);
	char* parseList(char* start,char* end);
	char* parseAnnounces(char* start);
public:
	enum ENCODING
	{
		GBK,BIG5,UTF_8
	};

	struct fileinfo
	{
		std::string strname;//保存文件名
		QVOD_INT64 filelen;//长度
	};

	std::vector<fileinfo> m_vctfile;//?

	std::vector<std::string> m_trackerlist;

	char*			m_torrent;	//种子文件内容
	int				m_torrentlen;	//种子文件长度
	ENCODING		m_encoding;	//种子文件编码方式

	string			m_strSeedName; //种子文件名
	string			m_downFileName; //下载文件名
	QVOD_UINT64		m_downfilesize; //下载文件长度
	unsigned int	m_piecenum;		//下载文件分片数
	unsigned int	m_piecesize;	//下载文件分片大小
	unsigned int	m_createtime;	//创建时间
	unsigned int	m_TrackerIP;	//tracker ip
	unsigned short	m_TrackerPort;	//tracker port
	string			m_adurl;		//ad 地址
	string			m_announce;
	string			m_TextAdUrl;
	string			m_PauseAdUrl;
	bool			m_bIsSeedParse;		//种子文件是否被解析过了

	HASH *			m_hash_list;	//块哈希列表
	char *			m_infohash;
	int				m_infohashlen;
	QVOD_UINT64		m_fileLen;
};

#endif

