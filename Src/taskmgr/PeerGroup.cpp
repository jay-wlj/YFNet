// PeerGroup.cpp: implementation of the CPeerGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "PeerGroup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPeerGroup::CPeerGroup()
{
}

CPeerGroup::~CPeerGroup()
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator it = m_peerMap.begin();
	for(;it != m_peerMap.end(); it++)
	{
		CPeer *pPeer = it->second;
		if ( pPeer != NULL)
		{
			delete pPeer;
		}				
	}
	m_peerMap.clear();
}

int CPeerGroup::AddPeer(const KEY& key, unsigned int PublicIP,unsigned short PublicPort,
						bool isDownPeer,unsigned char nattype,unsigned char nettype, char PeerID[20]
						,PEERTYPE peertype)
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator it = m_peerMap.find(key);
	if(it != m_peerMap.end())
	{
		//删除
		CPeer* pPeer = it->second;
		pPeer->release();
		m_peerMap.erase(it);
	}
	CPeer *pPeer = new CPeer(PublicIP, PublicPort, isDownPeer,nattype,nettype, PeerID,peertype);
	m_peerMap.insert(make_pair(key,pPeer));	
	return 0;
}

int CPeerGroup::AddServer(KEY &key,bool isDownPeer,unsigned char natType,unsigned char netType,char szPeerID[20],
						  QVOD_UINT64 iBeginPos,unsigned int iDataLen,PEERTYPE peerType)
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator iter = m_peerMap.find(key);
	if (iter != m_peerMap.end())
	{
		CPeer *pPeer = iter->second;
		pPeer->release();
		m_peerMap.erase(iter);
	}
	CPeer *pPeer = new CPeer(key.ip,key.port,isDownPeer,natType,netType,szPeerID,iBeginPos,iDataLen,peerType);
	m_peerMap.insert(make_pair(key,pPeer));
	return 0;
}

int CPeerGroup::DelPeer(KEY& key,unsigned int& iDownSize,unsigned int& iUpSize)
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator it = m_peerMap.find(key);
	if(it != m_peerMap.end())
	{
		CPeer *pPeer = it->second;
		//删除占用的bufsize
		if ( !pPeer->IsVipAccServer() )
		{
			iDownSize -= pPeer->GetDownBufSize();
			iUpSize -= pPeer->GetUpBufSize();
		}
		//释放删除
		pPeer->release();
		m_peerMap.erase(it);
		return 0;
	}
	return -1; //
}
bool CPeerGroup::GetPeerKey(char random[12],KEY& key)
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator it = m_peerMap.begin();
	for(;it != m_peerMap.end();it++)
	{
		CPeer* pPeer = it->second;
		if(pPeer->IsSameRandomValue(random))
		{
			key = it->first;
			return true;
		}
	}
	return false;
}
bool CPeerGroup::FindPeer(char random[12],CPeer** peer)
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator it=m_peerMap.begin();
	for(;it!=m_peerMap.end();++it)
	{
		CPeer* pPeer = it->second;
		if(pPeer->IsSameRandomValue(random))
		{
			pPeer->duplicate();
			*peer = pPeer;
			return true;
		}
	}
	return false;
}	

bool CPeerGroup::FindPeer(const KEY& key,CPeer** peer)
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator it = m_peerMap.find(key);
	if(it != m_peerMap.end())
	{
		CPeer* pPeer = it->second;
		pPeer->duplicate();
		*peer = pPeer;
		return true;
	}
	return false;
}	

bool CPeerGroup::FindPeer(unsigned int ip, short port,CPeer** peer)
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator it = m_peerMap.begin();
	for(; it != m_peerMap.end(); it++)
	{
		if(it->first.ip == ip && it->first.port == port)
		{
			CPeer* pPeer = it->second;
			pPeer->duplicate();
			*peer = pPeer;
			return true;
		}
	}
	return false;
}

bool CPeerGroup::GetPeer(int index,CPeer** peer)
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator it = m_peerMap.begin();
	if ( index >= int(m_peerMap.size()))
	{
		return false;
	}

	for ( int i = 0 ; i < index ; i++ )
	{
		if ( it == m_peerMap.end())
		{
			return false;
		}
		it++;
	}
	CPeer* pPeer = it->second;
	pPeer->duplicate();
	*peer = pPeer;
	return true;
}

unsigned int CPeerGroup::GetPeerNum(PEERTYPE type/* = ALLTYPE*/)
{
	unsigned int num = 0;
	CAutoLock lock(m_peerMapLock);
	if (ALLTYPE == type)
	{
		num = m_peerMap.size();
	}
	else
	{
		//需要遍历累计特定类型的peer个数
		for (map<KEY,CPeer*>::iterator iter = m_peerMap.begin(); iter != m_peerMap.end(); ++iter)
		{
			if (iter->second->GetPeerType() == type)
			{
				++num;
			}
		}
	}
	return num;
}

int CPeerGroup::GetCacheServerDownloadSpeed(map<KEY,int> &mapCacheSpeed)
{
	int speed = 0;
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer* >::iterator iter = m_peerMap.begin();
	while (iter != m_peerMap.end())
	{
		if (iter->second->IsCacheServer())
		{
			int cacheSpeed = iter->second->GetDownRate();
			mapCacheSpeed.insert(make_pair(iter->first,cacheSpeed));
			speed += cacheSpeed;
		}
		iter++;
	}
	return speed;
}

int CPeerGroup::GetCacheServerDownloadSpeed()
{
	int speed = 0;
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer* >::iterator iter = m_peerMap.begin();
	while (iter != m_peerMap.end())
	{
		if (iter->second->IsCacheServer())
		{
			speed += iter->second->GetDownRate();
		}
		iter++;
	}
	return speed;
}

int CPeerGroup::GetMinerServerDownloadSpeed()
{
	int speed = 0;
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer* >::iterator iter = m_peerMap.begin();
	while (iter != m_peerMap.end())
	{
		if (iter->second->IsMinerServer())
		{
			speed += iter->second->GetDownRate();
		}
		iter++;
	}
	return speed;
}

int CPeerGroup::GetSomePeerDownSpeed( PEERTYPE type )
{
	int speed = 0;
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer* >::iterator iter = m_peerMap.begin();
	while (iter != m_peerMap.end())
	{
		if (ALLTYPE == type || iter->second->GetPeerType() == type)
		{
			speed += iter->second->GetDownRate();
		}
		iter++;
	}
	return speed;
}


QVOD_UINT64 CPeerGroup::GetServerFileBeginPos(unsigned int ip,unsigned short port)
{
	QVOD_UINT64 result = -1;
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator iter = m_peerMap.begin();
	while (iter != m_peerMap.end())
	{
		if(iter->first.ip == ip && iter->first.port == port)
		{
			result = iter->second->GetServerFileBeginPos();
			break;
		}
		iter++;
	}
	return result;
}

#ifdef NETBAR
bool CPeerGroup::GetSuperNode(CPeer** peer)
{
	CAutoLock lock(m_peerMapLock);
	map<KEY,CPeer*>::iterator it = m_peerMap.begin();
	for(; it != m_peerMap.end(); it++)
	{
		if(it->second->isSuperNode())
		{
			CPeer* pPeer = it->second;
			pPeer->duplicate();
			*peer = pPeer;
			return true;
		}
	}
	return false;
}
#endif