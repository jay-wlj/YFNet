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

	//��ʼ�������ļ���2�з�ʽ 1��ֱ�Ӷ��ļ���2�����紫��
	bool Read();
	bool Read(const char *filename);

	//�����ʼ��һ������
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

	//��ʱ�ӿ�
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
		std::string strname;//�����ļ���
		QVOD_INT64 filelen;//����
	};

	std::vector<fileinfo> m_vctfile;//?

	std::vector<std::string> m_trackerlist;

	char*			m_torrent;	//�����ļ�����
	int				m_torrentlen;	//�����ļ�����
	ENCODING		m_encoding;	//�����ļ����뷽ʽ

	string			m_strSeedName; //�����ļ���
	string			m_downFileName; //�����ļ���
	QVOD_UINT64		m_downfilesize; //�����ļ�����
	unsigned int	m_piecenum;		//�����ļ���Ƭ��
	unsigned int	m_piecesize;	//�����ļ���Ƭ��С
	unsigned int	m_createtime;	//����ʱ��
	unsigned int	m_TrackerIP;	//tracker ip
	unsigned short	m_TrackerPort;	//tracker port
	string			m_adurl;		//ad ��ַ
	string			m_announce;
	string			m_TextAdUrl;
	string			m_PauseAdUrl;
	bool			m_bIsSeedParse;		//�����ļ��Ƿ񱻽�������

	HASH *			m_hash_list;	//���ϣ�б�
	char *			m_infohash;
	int				m_infohashlen;
	QVOD_UINT64		m_fileLen;
};

#endif

