#include "qvod_string.h"
#include "qvod_time.h"
#include <stdio.h>
#include "common-piece.h"
#include "global.h"

CPiece::CPiece(unsigned int nlen,unsigned int index,int timeout)
{
	m_index = index;
	m_buflen = nlen;
	m_vipBufLen = 0;
	m_iTimeOut = timeout;
	//m_pbuf = NULL;//data buffer
	m_subPieceLen = (m_buflen + BLOCK_SIZE - 1) / BLOCK_SIZE;//>>14;
	m_pbuf = new char*[m_subPieceLen];
	memset(m_pbuf,0,m_subPieceLen*sizeof(char*));
	m_pDownQue = new char[m_subPieceLen];
	m_ptimestamp = new QVOD_DWORD[m_subPieceLen];
	memset(m_pDownQue,0,m_subPieceLen);//ÿ���Լ�����һ��С��Ƭ,0δ����,1������,2,��������
	memset(m_ptimestamp,0,m_subPieceLen*sizeof(unsigned int));
}

CPiece::~CPiece()
{
	unsigned int i;
	for(i = 0;i<m_subPieceLen;i++)
	{
		if(m_pbuf[i])
			delete []m_pbuf[i];
	}
	if (m_pbuf) 
	{
		delete []m_pbuf;
		m_pbuf = NULL;
	}
	if(m_pDownQue)
	{
		delete []m_pDownQue;
		m_pDownQue = NULL;
	}
	if(m_ptimestamp)
	{
		delete []m_ptimestamp;
		m_ptimestamp = NULL;
	}	
}

int CPiece::Write(unsigned int offset,char* buf,unsigned int nlen,bool isVip)
{
	//if(!m_pbuf)
	//	m_pbuf = new char[m_buflen];

	if(!buf || offset + nlen > m_buflen)
	{
		return 0;
	}
	//memcpy(m_pbuf+offset,buf,nlen);
	int subindex = offset / BLOCK_SIZE;//>>14;
	int tlen = nlen;
	int writelen = 0;
	while(tlen > 0)
	{
		int subpiecelen = tlen>=BLOCK_SIZE?BLOCK_SIZE:tlen ;
		if(!m_pbuf[subindex])
		{
			m_pbuf[subindex] = new char[subpiecelen];
			memcpy(m_pbuf[subindex],buf +(subindex*BLOCK_SIZE - offset),subpiecelen);
			writelen += subpiecelen;
		}
		m_downQueLock.Lock();
		m_pDownQue[subindex++] = 1;
		m_downQueLock.Unlock();
		tlen -= subpiecelen;
	}
	//vip������Ϣ
	if(isVip)
	{
		m_vipBufLen += nlen;
	}
	return writelen;
}

int CPiece::GetMemSize()
{
	int memsize = 0;
	unsigned int i;
	CAutoLock lock(m_downQueLock);
	if(m_buflen % BLOCK_SIZE)
	{
		for(i = 0;i < m_subPieceLen-1;i++)
		{
			if(m_pDownQue[i] == 1)
			{
				memsize += BLOCK_SIZE;
			}
		}
		if(m_pDownQue[m_subPieceLen-1] == 1)
		{
			memsize += m_buflen%BLOCK_SIZE;
		}
	}
	else
	{
		for(i = 0;i < m_subPieceLen;i++)
		{
			if(m_pDownQue[i] == 1)
			{
				memsize += BLOCK_SIZE;
			}
		}
	}
	return memsize;
}

bool CPiece::IsFullFilled()
{
	//������Ƭ�������˲�����
	CAutoLock lock(m_downQueLock);
	unsigned int i;
	for(i = 0;i < m_subPieceLen;i++)
	{
		if(m_pDownQue[i] != 1)
		{
			return false;
		}
	}
	return true;
}

bool CPiece::IsFullFilledOrDownloading()
{
	//������Ƭ�������˲�����
	CAutoLock lock(m_downQueLock);
	unsigned int i;
	for(i = 0;i < m_subPieceLen;i++)
	{
		if(m_pDownQue[i] != 1 && m_pDownQue[i] != 2)
		{
			return false;
		}
	}
	return true;
}

bool CPiece::CheckSubIndex(unsigned int subindex)//?
{
	CAutoLock lock(m_downQueLock);
	if(subindex < m_subPieceLen && m_pDownQue[subindex] == 1)
		return true;
	return false;
}

bool CPiece::CancelSubIndex(unsigned int subindex)
{
	//unsigned int subindex=offset>>14;
	CAutoLock lock(m_downQueLock);
	if(subindex < m_subPieceLen && m_pDownQue[subindex] != 0)
	{
		m_pDownQue[subindex]=0;
		return true;
	}
	else
	{
		return false;
	}
}

int CPiece::GetNextSubField(bool& bfull,int& nsubfield)
{
	unsigned int i;
	QVOD_DWORD timestamp = QvodGetTime();
	bfull = false;
	int count = 0;
	unsigned int maxpos = 0;
	int maxspace = 0;
	int pos;
	int space;
	CAutoLock lock(m_downQueLock);
	for(i = 0;i < m_subPieceLen;)
	{ 
		if(m_pDownQue[i] == 0 )
		{
			count++;
			pos = i;
			space = 0;
			for(;space < nsubfield && i < m_subPieceLen;i++)
			{
				//���nsubfieldΪ1�Ͳ������
				if(m_pDownQue[i] == 1 || m_pDownQue[i] == 2 && (timestamp <= m_ptimestamp[i] + m_iTimeOut))
				{
					break;
				}
				space++;
			}
			if(space > maxspace)
			{
				maxspace = space;//ȡ��Χ���
				maxpos = pos;
			}
		}
		else
		{
			i++;
		}
	}
	if(maxspace > 0)
	{
		nsubfield = maxspace;
		for(i = maxpos;i < maxpos+maxspace;i++)
		{
			m_pDownQue[i] = 2;
			m_ptimestamp[i] = timestamp;
		}
		if(count == 1)
			bfull = true;
		return maxpos;
	}

	//�������������еķ�Ƭ
	bfull = true;
	int subfield = -1;
	int longsubfield = 0;
	for(i = 0;i < m_subPieceLen;i++)
	{
		if(m_pDownQue[i] == 2 && (timestamp > m_ptimestamp[i] + m_iTimeOut))//���س���10��û���ص���Ƭ
		{
			m_ptimestamp[i] = timestamp;
			subfield = i;
			//��ѯ�����Ƿ��г�ʱ��Ƭ
			for(++i;i < m_subPieceLen;i++)
			{
				if(m_pDownQue[i] == 2 && (timestamp > m_ptimestamp[i] + m_iTimeOut))//���س���10��û���ص���Ƭ
				{
					bfull = false;
					break;
				}
			}	
			return subfield;
		}
	}
	return -1;
}

int CPiece::GetTimeoutSubField()
{
	int i = 0;
	QVOD_DWORD timestamp = QvodGetTime();
	CAutoLock lock(m_downQueLock);
	for(i = 0;i < m_subPieceLen;i++)
	{
		if(m_pDownQue[i] != 1)
		{
			m_ptimestamp[i] = timestamp;
			m_pDownQue[i] = 2;
			return i;
		}
	}
	return -1;
}

int CPiece::GetEmergencySubField(unsigned int& subindex)
{
	int i = 0;
	int num = 0;
	int nsubindex = 65536/BLOCK_SIZE;
	QVOD_DWORD timestamp = QvodGetTime();
	CAutoLock lock(m_downQueLock);
	for(i = 0;i < m_subPieceLen;i++)
	{
		if(m_pDownQue[i] == 2 && timestamp > m_ptimestamp[i] + PIECE_EMERG_TIMEOUT)
		{
			m_ptimestamp[i] = timestamp;
			subindex = i;
			num++;
			i++;
			while(i < m_subPieceLen && num < nsubindex && m_pDownQue[i] == 2 && timestamp > m_ptimestamp[i] + PIECE_EMERG_TIMEOUT)
			{
				m_ptimestamp[i] = timestamp;
				num++;
				i++;
			}
			return num;
		}
	}
	return -1;
}

bool CPiece::HaveTimeOut()
{
	QVOD_DWORD timestamp = QvodGetTime();
	unsigned int i;
	bool ret=false;
	CAutoLock lock(m_downQueLock);
	for(i = 0;i < m_subPieceLen;i++)
	{
		if(m_pDownQue[i] == 2 && timestamp > m_ptimestamp[i] + m_iTimeOut)//���س���10��û���ص���Ƭ
		{
			m_pDownQue[i] = 0;
			ret = true;
			//break;
		}
	}
	return ret;
}

void CPiece::CopyBuffer(char* buf,unsigned int from,unsigned int len)
{
	//���ݿ�������
	if(from + len > m_buflen)
		return;
	int start = from/BLOCK_SIZE;
	int end = (from+len)/BLOCK_SIZE;
	int i;
	char* pbuf = buf;
	//������ĵ���������һƬ
	int startfromlen = from%BLOCK_SIZE;
	if(startfromlen)
	{
		if(!m_pbuf[start])
			return;
		memcpy(pbuf,m_pbuf[start]+startfromlen,BLOCK_SIZE-startfromlen);
		pbuf += BLOCK_SIZE-startfromlen;
		start++;
	}
	//�����м�����
	for(i = start;i<end;i++)
	{
		if(!m_pbuf[i])
			return;
		memcpy(pbuf,m_pbuf[i],BLOCK_SIZE);
		pbuf +=BLOCK_SIZE;
	}
	//����ĩβ
	int endlen = (from+len)%BLOCK_SIZE;
	if(endlen)
	{
		if(!m_pbuf[end])
			return;
		memcpy(pbuf,m_pbuf[end],endlen);
	}
}