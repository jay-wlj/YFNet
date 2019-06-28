#ifdef _DEBUG
#include "auth.h"
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include "../include/md5.h"
#include "../common/inifile.h"
#include "../common/utility.h"
#include "../common/commonfun.h"

bool TeaEncrypt( char* data, unsigned int len, unsigned int *k )
{
	if (len%8 != 0 || 0 == len)
	{
		return false;
	}
	char* pos = data;
	unsigned int delta=0x3de6e675;                 /* a key schedule constant */
	unsigned int a=k[0], b=k[1], c=k[2], d=k[3];   /* cache key */
	while (pos < data + len)
	{
		unsigned int* v = (unsigned int*)pos;
		unsigned int y=v[0], z = v[1], sum=0, i;         /* set up */
		for (i=0; i < 32; i++) 
		{                        /* basic cycle start */
			sum += delta;
			y += ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
			z += ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);/* end cycle */
		}
		v[0]=y;
		v[1]=z;	
		pos += 8;
	}
	return true;
}


bool TeaDecrypt( char* data, unsigned int len, unsigned int *k )
{
	if (len%8 != 0 || 0 == len)
	{
		return false;
	}
	char* pos = data;
	unsigned int delta=0x3de6e675;                 /* a key schedule constant */
	unsigned int a=k[0], b=k[1], c=k[2], d=k[3];   /* cache key */
	while (pos < data + len)
	{
		unsigned int* v = (unsigned int*)pos;
		unsigned int y=v[0], z=v[1], sum=0xbcdccea0, i; /* set up */
		for(i=0; i<32; i++) 
		{                            /* basic cycle start */
			z -= ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);
			y -= ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
			sum -= delta;                                /* end cycle */
		}
		v[0]=y;
		v[1]=z;	
		pos += 8;
	}
	return true;
}

CAuth *CAuth::m_instance = NULL;

CAuth* CAuth::Instance()
{
	if(NULL == m_instance)
	{
		m_instance = new CAuth;
	}
	return m_instance;
}

void CAuth::FreeInstance()
{
	if(m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

CAuth::CAuth(void)
{
	if(IsAble())
	{
		//�Ѿ���֤ͨ��
		return ;
	}
	m_isAble = 0;
	//������֤������
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock)
	{
		return ;
	}
	//ȡĬ����������
	unsigned int ip = inet_addr(DEBUGAUTHSERVERIP);
	//���»�ȡ��������
	GetIpFromDomain(ip,DEBUGAUTHSERVER);

	//debugauth.kuaibo.com
	int iTimeout = 3000;
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeout, sizeof(iTimeout));
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeout, sizeof(iTimeout));
	SOCKADDR_IN addr = {0};
	addr.sin_family = AF_INET;	
	addr.sin_port = htons(5555);
	addr.sin_addr.S_un.S_addr = ip;
	if(connect(sock, (PSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		return ;
	}

	//��ȡ�û���������
	char userKey[255] = {0};
	int userKeyLen = 0;
	CIniFile iniConfig((g_strCurrentRoot+L"QvodCfg.ini").c_str());
	userKeyLen += iniConfig.ReadString("AUTH","user",userKey,255);
	userKeyLen += iniConfig.ReadString("AUTH","passwd",userKey+userKeyLen,255-userKeyLen);
	std::string strMD5 = CMD5Checksum::GetMD5((BYTE*)userKey,userKeyLen);	


	char szSendData[48] = {0};
	memcpy(szSendData, strMD5.c_str(), 32);
	//��ȡ4��������Ͳ��洢��key������,�����������Ͱ�
	int* pInt = (int*)(szSendData + 32);
	int key[4];
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 4; i++)
	{
		pInt[i] = rand();
		key[i] = pInt[i];		
	}
	unsigned int key2[4] = {0x02B1FA11,0x046CD0EE,0x0A42F823,0x767FDA52};
	TeaEncrypt(szSendData, sizeof(szSendData), key2);
	if (send(sock, szSendData, sizeof(szSendData), 0) != sizeof(szSendData))
	{
		closesocket(sock);
		sock = INVALID_SOCKET;
		return ;
	}
	char szRecvData[8] = {0};
	int iRecvNum = recv(sock, szRecvData, sizeof(szRecvData), 0);
	if (iRecvNum <= 0)
	{
		closesocket(sock);
		sock = INVALID_SOCKET;
		return ;
	}
	closesocket(sock);
	sock = INVALID_SOCKET;
	//����֮ǰ�洢��key������֤���ذ�
	TeaDecrypt(szRecvData, iRecvNum, (unsigned int*)key);
	pInt = (int*)szRecvData;
	if (0 == ntohl(*pInt))
	{
// 		g_iLoginLevel = ntohl(*++pInt);
// 		if (g_iLoginLevel <= 0)
// 		{
// 			//Ȩ�޴���
// 			return ;
// 		}
// 		if (g_iLoginLevel > 3)
// 		{
// 			//Ȩ�޴���
// 			return ;
// 		}
	}
	else
	{
		return ;
	}
	m_isAble = 43800;
	printf("hello world!");
}

CAuth::~CAuth(void)
{
	m_isAble = 33800;
}

bool CAuth::IsAble()
{
	if(m_isAble == 43800)
	{
		return true;
	}
	return false;
}
#endif