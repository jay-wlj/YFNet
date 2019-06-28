// PeerEx.cpp: implementation of the CPeer class.
//
//////////////////////////////////////////////////////////////////////]
#include "sha1.h"
#include "common-commonfun.h"
#include "global.h"
#include "Peer.h"
//#include "QvodMinerStc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CPeer::CPeer(unsigned int PublicIP,unsigned short PublicPort,
			 bool isDownPeer,unsigned char nattype,unsigned char nettype, char PeerID[20],
			 PEERTYPE peertype)
{
	m_status=UNHANDSHAKE;
	m_PublicIP = PublicIP;
	m_PublicPort = PublicPort;
	m_nattype = nattype;
	m_nettype = nettype;
	m_bSuperNode = false;
	m_bCacheServer = false;
	m_bIsHttpPeer = false;
	m_UserType = TORRENT;
	m_isDownPeer=isDownPeer;
	m_downLen = 0;
	m_downRate = 0;
	m_upLen = 0;
	m_upRate = 0;
	m_reqDownNum=0;
	m_xSecDownBufSize=0;
	m_xSecUpBufsize=0;
	m_bitfiledLen=0;
	m_bitfield=NULL;
	m_timeoutCount=0;
	m_peerType = peertype;
	if (PeerID)
	{
		SetPeerID(PeerID);
	}
	else
	{
		memset(m_PeerID,'0',20);
		m_UserType = TORRENT;
	}
	m_version = Str2Int(m_PeerID,4,7);
	for(int i=0;i<REQUEST_NUM;i++)
	{
		m_reqIndex[i]=-1;
		m_reqBegin[i]=-1;
	}
	memset(m_szIspID,0,sizeof(m_szIspID));
	m_iP2PShareStcType = 0;
	m_iUserID = 0;
}

CPeer::CPeer(unsigned int publicIP,unsigned short publicPort,bool isDownPeer,unsigned char natType,unsigned char netType, 
			 char szPeerID[20],QVOD_UINT64 iBeginPos,unsigned int iDataLen,PEERTYPE peerType)
{
	m_status=UNHANDSHAKE;
	m_PublicIP = publicIP;
	m_PublicPort = publicPort;
	m_nattype = natType;
	m_nettype = netType;
	m_bSuperNode = false;
	m_bCacheServer = false;
	m_bIsHttpPeer = false;
	m_UserType = TORRENT;
	m_isDownPeer=isDownPeer;
	m_downLen = 0;
	m_downRate = 0;
	m_upLen = 0;
	m_upRate = 0;
	m_reqDownNum=0;
	m_xSecDownBufSize=0;
	m_xSecUpBufsize=0;
	m_bitfiledLen=0;
	m_bitfield=NULL;
	m_timeoutCount=0;
	m_peerType = peerType;
	if (szPeerID)
	{
		SetPeerID(szPeerID);
	}
	else
	{
		memset(m_PeerID,'0',20);
		m_UserType = TORRENT;
	}
	m_version = Str2Int(m_PeerID,4,7);
	for(int i=0;i<REQUEST_NUM;i++)
	{
		m_reqIndex[i]=-1;
		m_reqBegin[i]=-1;
	}
	memset(m_szIspID,0,sizeof(m_szIspID));
	m_iP2PShareStcType = 0;
	m_iUserID = 0;
	m_iBeginPos = iBeginPos;
	m_iDataLen = iDataLen;
}

CPeer::~CPeer()
{
	if(m_bitfield)
		delete[] m_bitfield;
}
void CPeer::SetPeerID(char PeerID[20])
{
	memcpy(m_PeerID,PeerID,20);
	m_version = Str2Int(m_PeerID,4,7);
	
	if (memcmp(PeerID, "QVOD0000HTTP", 12) == 0)
	{
		m_bIsHttpPeer = true;
	}
// 	char hash[20];
// 	CSHA1 sha;
// 	sha.write(&m_PublicIP,4);
// 	sha.read(hash);
	if (m_UserType == QVOD || memcmp("QVOD",PeerID,4)== 0)
	{
		m_UserType = QVOD;
		m_bSuperNode = false;
#ifdef NETBAR
		if (memcmp(PeerID+4,"001",3) == 0)//10-19为网吧版服务器
		{
			m_bSuperNode = true;
		}
#else
		if (memcmp(PeerID+4,"000",3) == 0)//10以下版本为qvodserver保留
		{
			m_bSuperNode = true;
		}
#endif
	}
	else
	{
		m_UserType = TORRENT;
	}


	if ( (m_version > 21 && m_version < 29) || (m_version > 31 && m_version < 39) )
	{
		//移动端
		m_iP2PShareStcType = 3;
	}
	else if ( m_version > 102 && m_version < 109 )
	{
		//360原生态版本
		m_iP2PShareStcType = 2;
	}
	else if ( m_version > 75 && m_version < 99 )
	{
		//PC端
		m_iP2PShareStcType = 1;
	}	
}

void CPeer::SetPeerCacheServer()
{
	m_bCacheServer = true;
}

int CPeer::QueryStatus()
{
	return m_status;
}

bool CPeer::UpdateStatus(int status)
{
	m_status=(CPeer::Status)status;
	return true;
}

unsigned char* CPeer::GetBitField()
{
	if(m_bitfield)
		return m_bitfield;
	else
		return NULL;
}

void CPeer::CreateBitField(unsigned char*bit, int length)
{
	if(m_bitfield)
		delete[] m_bitfield;
	m_bitfield = new unsigned char[length];
	memcpy(m_bitfield,bit,length);
	m_bitfiledLen=length;
}

void CPeer::SetBitField(unsigned int index)
{
	if(!m_bitfield || m_bitfiledLen<=0 || index < 0 || index >= (unsigned int)(m_bitfiledLen<<3))
	{
		//_OUTPUT(ERROR1,"peer setbitfield fail,m_bitfiledLen = %d\n",m_bitfiledLen);
		return ;
	}
	int Pos= index >> 3;
	int re = index & 0x07;
	m_bitfield[Pos] |= (0x80>>re);
}

void CPeer::ResetBitField(unsigned int index)
{
	if(!m_bitfield || m_bitfiledLen<=0 || index < 0 || index >= (unsigned int)(m_bitfiledLen<<3))
	{
		return ;
	}
	int Pos= index >> 3;
	int re = index & 0x07;
	m_bitfield[Pos] &= ~(0x80>>re);
}

void CPeer::InterDownSize(int size)
{
	if(size > 0)
	{
		m_downLen += size;
		m_timeoutCount = 0;
		if (m_iUserID != 0)
		{
			//CQvodMinerStc::Instance()->AddMinerIncome(m_iUserID,size);
		}
	}
	m_xSecDownBufSize += size;
}

void CPeer::InterUpSize(int size)
{
	if(size > 0)
	{
		m_upLen += size;
	}
	m_xSecUpBufsize += size;
}

int CPeer::GetDownRate()
{
	int speed=(int)m_xSecDownBufSize/SPCINTERVALTIME;
	if(speed>0)
		return speed;
	else
		return 0;
}
int CPeer::GetUpRate()
{
	int speed=(int)m_xSecUpBufsize/SPCINTERVALTIME;
	if(speed>0)
		return speed;
	else
		return 0;
}
void CPeer::ClearBitField()
{
	if(NULL != m_bitfield)
	{
		memset(m_bitfield, 0, m_bitfiledLen);
	}
}
int CPeer::GetDownBufSize()
{
	return m_xSecDownBufSize;
}
int CPeer::GetUpBufSize()
{
	return m_xSecUpBufsize;
}

bool CPeer::ZeroBitField(int bitlen)
{
	if(m_bitfield==NULL)
	{
		m_status=RUNNING;
		m_bitfiledLen=bitlen;
		m_bitfield=new unsigned char[m_bitfiledLen];
		memset(m_bitfield, 0x00, m_bitfiledLen);
		return true;
	}
	return false;
}

bool CPeer::FullBitField(int bitlen)
{
	if(m_bitfield==NULL && bitlen > 0)
	{
		m_status=RUNNING;
		m_bitfiledLen=bitlen;
		m_bitfield=new unsigned char[m_bitfiledLen];
		memset(m_bitfield, 0xff, m_bitfiledLen);
		return true;
	}
	return false;
}

bool CPeer::CheckIndex(unsigned int index)
{
	//检查是否拥有该分片
	if (index < 0 || !m_bitfield || m_bitfiledLen<=0 || index >= (unsigned int)(m_bitfiledLen<<3))
	{
		//_OUTPUT(ERROR1,"peer checkindex fail,m_bitfiledLen = %d\n",m_bitfiledLen);
		return false;
	}
	
	int Pos= index >> 3;
	int re = index & 0x07;
	
	return  (m_bitfield[Pos]&0x80>>re)==0?false:true;
}
bool CPeer::IsSameRandomValue(char random[12])
{
	if(memcmp(m_PeerID+8,random,12) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CPeer::NeedSendHaveMsg(unsigned int index)
{
	bool result = false;
	if(m_status == RUNNING && m_UserType != TORRENT && m_bitfield != NULL && !CheckIndex(index) &&
		m_peerType != MINER)
	{
		result = true;
	}
	return result;
}

int CPeer::GetPeerIspID(unsigned char *pIspID)
{
	memcpy(pIspID,m_szIspID,4);
	pIspID[4] = 0;
	return 0;
}

int CPeer::SetPeerIspID(unsigned char *pIspID)
{
	memcpy(m_szIspID,pIspID,4);
	return 0;
}

int CPeer::GetP2PShareStcType()
{
	return m_iP2PShareStcType;
}

int CPeer::SetUserID( unsigned int userID )
{
	m_iUserID = userID;
	//CQvodMinerStc::Instance()->CreateMiner(userID);
	m_peerType = MINER;
	return 0;
}

QVOD_UINT64 CPeer::GetServerFileBeginPos()
{
	return m_iBeginPos;
}

unsigned int CPeer::GetServerFileDataLen()
{
	return m_iDataLen;
}
