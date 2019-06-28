#ifndef __PEEREX_H__
#define __PEEREX_H__
#include "stdafx.h"
#include "common-commonstruct.h"
#include "common-RefCountedObject.h"
#include "global.h"

class CPeer :public RefCountedObject
{
public:
	enum Status
	{
		UNHANDSHAKE,
		HANDSHAKEEXPECT,
		RUNNING,
		WAIT,
		STOP
	};
	CPeer(unsigned int PublicIP, unsigned short PublicPort,
		bool isDownPeer,unsigned char nattype,unsigned char nettype, char PeerID[20],PEERTYPE peertype);

	// QvodServer�Ĺ��캯�� [2013/12/31 tangtao]
	CPeer(unsigned int publicIP,unsigned short publicPort,bool isDownPeer,unsigned char natType,unsigned char netType,
		char szPeerID[20],QVOD_UINT64 iBeginPos,unsigned int iDataLen,PEERTYPE peerType);
	virtual ~CPeer();
	unsigned int GetPublicIP(){return m_PublicIP;}
	unsigned short GetPublicPort(){return m_PublicPort;}
	bool isSuperNode(){return m_bSuperNode;}
	USERTYPE GetUserType(){return m_UserType;}
	char* GetPeerID(){return m_PeerID;}
	unsigned char GetNatType(){return m_nattype;}
	unsigned char GetNetType(){return m_nettype;}
	void UpdateNetType(unsigned char nettype){m_nettype = nettype;}
	int GetVersion(){return m_version;}
	int GetBitFieldLen(){return m_bitfiledLen;}
	unsigned char* GetBitField();
	bool IsDownMode(){return m_isDownPeer;}
	void CreateBitField(unsigned char*bit, int length);
	void  SetBitField(unsigned int index);
	void ResetBitField(unsigned int index);
	int QueryStatus();
	bool UpdateStatus(int status);
	void SetPeerID(char PeerID[20]);
	void SetPeerType(USERTYPE usertype){m_UserType = usertype;}
	void SetPeerCacheServer();
	bool IsCacheServer(){return m_bCacheServer;}
	bool IsMinerServer(){return m_peerType==MINER;}

	bool IsHttpPeer(){return m_bIsHttpPeer;}
	void ClearBitField();
	bool ZeroBitField(int bitlen);
	bool FullBitField(int bitlen);	
	void UpdateTimeStamp();
	unsigned int QueryTimeStamp();
	unsigned char GetTimeoutCount(){return ++m_timeoutCount;}
	bool CheckIndex(unsigned int index);
	bool IsSameRandomValue(char random[12]);
	//���ٲ���add by wangxijie
	void InterDownSize(int size);
	void InterUpSize(int size);

	int GetDownRate();
	int GetUpRate();
	int GetDownBufSize();
	int GetUpBufSize();
	QVOD_INT64 GetDownLen() {return m_downLen;}
	QVOD_INT64 GetUpLen() { return m_upLen;}
	
	bool NeedSendHaveMsg(unsigned int index);

	PEERTYPE GetPeerType() {return m_peerType;}
	//�û���VIP��ز���
	//////////////////////////////////////////////////////////////////////////
	bool IsVipAccServer(){return false;}

	//////////////////////////////////////////////////////////////////////////
	int GetPeerIspID(unsigned char *pIspID);
	int SetPeerIspID(unsigned char *pIspID);

	//P2P������ͳ���е�����;
	int GetP2PShareStcType();

	//�����ȿͻ��˵�UserID
	int SetUserID(unsigned int userID);

	//���CacheServer,MinerServer,CloudServer���ļ���ʼλ����Ϣ
	QVOD_UINT64 GetServerFileBeginPos();

	//���CacheServer,MinerServer,CloudServer���ļ����ݳ�����Ϣ
	unsigned int GetServerFileDataLen();
private:
	unsigned char m_nattype;
	unsigned char m_nettype;
	Status m_status;
	unsigned int m_PublicIP;
	unsigned short m_PublicPort;//NATӰ��˿�
	char m_PeerID[20];
	
	bool m_bIsHttpPeer;
	USERTYPE m_UserType;
	bool m_bSuperNode;
	bool m_bCacheServer;
	bool m_isDownPeer;
	int m_version;
	unsigned char* m_bitfield;
	int m_bitfiledLen;

	QVOD_INT64 m_downLen;
	QVOD_INT64 m_upLen;
	unsigned int m_upRate;
	unsigned int m_downRate;

	//���ٲ���,add by wangxijie
	unsigned int m_xSecUpBufsize,m_xSecDownBufSize;
	unsigned char m_timeoutCount;

	PEERTYPE m_peerType;
	unsigned char m_szIspID[5];			//peer��isp�����

	int m_iP2PShareStcType;				//P2P������ͳ���е�����

	unsigned int m_iUserID;

	//��¼CacheServer,MinerServer,CloudServer����Ϣ
	QVOD_UINT64	m_iBeginPos;		//ӵ�����ݵ���ʼλ��
	unsigned int		m_iDataLen;			//ӵ�����ݵĳ���
public:
	//�����ݼ��,add by wangxijie
	int m_reqIndex[REQUEST_NUM];
	int m_reqBegin[REQUEST_NUM];
	unsigned int m_reqDownNum;
};

#endif
