#ifndef PIECE_H
#define PIECE_H

#include "qvod_lock.h"
#include "common-RefCountedObject.h"

class CPiece : public RefCountedObject
{
private:
	int m_index;
	char** m_pbuf;
	char* m_pDownQue;
	unsigned int m_buflen;
	unsigned int m_vipBufLen;
	unsigned int m_subPieceLen;
	QVOD_DWORD* m_ptimestamp;
	bool m_urgent;
	int m_iTimeOut;
	CLock m_downQueLock;
public:
	CPiece(unsigned int nlen,unsigned int index,int timeout);
	~CPiece();
	int Write(unsigned int offset,char* buf,unsigned int nlen,bool isVip=false);
	int GetMemSize();
	void CopyBuffer(char* buf,unsigned int from,unsigned int len);
	int GetNextSubField(bool& bfull,int& nsubfield);
	int GetTimeoutSubField();
	int GetBufLen(){return m_buflen;}
	int GetVipBufLen(){return m_vipBufLen;}
	int GetIndex(){return m_index;}
	bool IsFullFilled();
	bool IsFullFilledOrDownloading();
	bool CheckSubIndex(unsigned int subindex);
	bool CancelSubIndex(unsigned int subindex);
	bool HaveTimeOut();
	int GetEmergencySubField(unsigned int& subindex);
};



#endif