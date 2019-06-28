// PeerGroup.h: interface for the CPeerGroup class.
//
//////////////////////////////////////////////////////////////////////
#ifdef WIN32 /* WIN32 */
#pragma warning(disable:4786)
#else /* posix */
//nothing
#endif /* posix end */

#if !defined(AFX_PEERGROUP_H__746901B8_15D9_43D0_BAB3_29EB538BE051__INCLUDED_)
#define AFX_PEERGROUP_H__746901B8_15D9_43D0_BAB3_29EB538BE051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <map>
#include "Peer.h"
#include "common-RefCountedObject.h"
#include "qvod_lock.h"
#include "qvod_string.h"

class CPeerGroup
{
public:
	unsigned int GetPeerNum(PEERTYPE type = ALLTYPE);
	int AddPeer(const KEY& key, unsigned int PublicIP,unsigned short PublicPort,
		bool isDownPeer,unsigned char nattype,unsigned char nettype,char PeerID[20],PEERTYPE peertype);

	// ���CacheServer,MinerServer,CloudServer�ȷ����� [2013/12/31 tangtao]
	int AddServer(KEY &key,bool isDownPeer,unsigned char natType,unsigned char netType,char szPeerID[20],
		QVOD_UINT64 iBeginPos,unsigned int iDataLen,PEERTYPE peerType);

	int DelPeer(KEY& key,unsigned int& iDownSize,unsigned int& iUpSize);
	bool GetPeer(int index,CPeer** peer);
	bool FindPeer(const KEY& key,CPeer** peer);
	bool FindPeer(unsigned int ip, short port,CPeer** peer);
	bool FindPeer(char random[12],CPeer** peer);
	bool GetPeerKey(char random[12],KEY& key);

	//��ȡCacheServer�������ٶ�
	int GetCacheServerDownloadSpeed(map<KEY,int> &mapCacheSpeed);
	int GetCacheServerDownloadSpeed();

	//��ȡMinerServer�������ٶ�
	int GetMinerServerDownloadSpeed();

	//��ȡĳһ��peer�������ٶ�
	int GetSomePeerDownSpeed(PEERTYPE type);
	//��ȡCacheServer,MinerServer,CloudServer����Ϣ
	QVOD_UINT64 GetServerFileBeginPos(unsigned int ip,unsigned short port);
#ifdef NETBAR
	bool GetSuperNode(CPeer** peer);
#endif
public:
	CPeerGroup();
	virtual ~CPeerGroup();
private:
	map<KEY,CPeer*>	m_peerMap;//PEER�б�
	CLock			m_peerMapLock;
};

#endif // !defined(AFX_PEERGROUP_H__746901B8_15D9_43D0_BAB3_29EB538BE051__INCLUDED_)

