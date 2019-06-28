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
	KEY_GET_TIME = 17,								//��ȡ������ʱ��
	KEY_AUTO_UPLOAD_LIMIT_PERCENT = 101,			//����ģʽ�ϴ����ưٷֱ�
	KEY_INTERNET_UPLOAD_LIMIT_PERCENT = 102,		//����ģʽ�ϴ����ưٷֱ�
	KEY_MOVIE_UPLOAD_LIMIT_PERCENT = 103,			//��Ӱģʽ�ϴ����ưٷֱ�
	KEY_GAME_UPLOAD_LIMIT_PERCENT = 104				//��Ϸģʽ�ϴ����ưٷֱ�
};

struct CloudCfgReq
{
	unsigned short len;//���������
	unsigned short version;//�ͻ��˰汾��
	unsigned short key;//�����ID
	unsigned char  reserved[2];
};
struct CloudCfgRsp
{
	unsigned short len;//����Ӧ���ĳ���
	unsigned short key;
	unsigned int   timeout;//��ʱʱ�䣬��λ��
	//char*          val;//key��Ӧ��ֵ
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