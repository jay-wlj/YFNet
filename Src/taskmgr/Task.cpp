#include "task.h"
#include "stdafx.h"
#include "protocol.h"
#include "sha1.h"
#include "common-inifile.h"
#include "common-utility.h"
#include "common-commonfun.h"
#include "Iointerface.h"
#include "stund.h"
#include "global.h"
#include "common-utility.h"
#include "DbTaskConfig.h"
#include "CopyTaskFile.h"
#include "qvod_sock.h"
#include "qvod_time.h"
#include "CacheDataInMem.h"

CTask::CTask()
{
	m_curUpSize = 0;
	m_curDownSize = 0;
	m_bIsHide = false;
	m_bIsReservedFile = false;
	m_bIsUpdate = true;
	m_fileindex = 0;

	m_limitDownSpeed = 0;
	m_xSecDownBufSize = 0;
	m_xSecUpBufSize = 0;
	m_downQueueTime = 0;

	m_nextDownTime = 0;
	m_buffingpos = 0;
	m_dragindex = 0;
	m_dragpos = 0;
	m_Para.status = RUNNING;

	m_playfileIndex = 0;
	m_downQue = NULL;
	m_nlocalseeds = 0;

	m_lastDownSize = BLOCK_SIZE + 13;
	m_Para.uploadlimit = g_task_UpRateLimit;
	m_Para.seedslmt = g_task_SeedsLimit;
	m_Para.peerslmt = g_task_PeersLimit;
	m_Para.bcheckkey = 0;
	m_Para.maketime = (long)time(NULL);
	m_lastStartTime = time(NULL);

	m_httpDownLen = 0;

	m_accHttpReqStatus = false;

	m_bIsPlaying = false;
	m_bBuffering = false;

#ifdef NETBAR
	m_MaxPieces = 0;
	m_MaxIndexPieces = 0;
	m_playerIndex = 0;
	m_isNeedDownload = true;
#endif

	m_iPlayPos = 0;
	m_iPlayFileRate = 0;	
	m_iVideoDuration = 0;
	m_bOptimizeUpload = false;

	m_iErrorMask = TASK_NO_ERROR;
	m_iWriteFailReason = 0;
	m_iEncodeKey = 0;
	m_bHeaderIsFinished = false;
	m_bEndingIsFinished = false;


	m_iDownloadMode = MODE_CDN;
	m_iHttpDownloadingBeginIndex = -1;
	m_iHttpDownloadingEndIndex = -1;

#ifdef MONGOTV
	memset(m_vid, 0, sizeof(m_vid));
	m_bIsOtherCdn = true;
	m_bIsStartDoSended =false;
#endif

	m_peers = new CPeerGroup;

	m_seed = NULL;
	m_mem = NULL;
}

CTask::~CTask()
{
#ifdef QVOD_USE_CACHEMEN
	CCacheDataInMem::Instance().ClearCacheByHash((HASH&)m_Para.hash);
#endif

#ifdef NETBAR
	m_lockMemPiece.Lock();
	map<int, PieceBuffer>::iterator itr = m_mapMemPiece.begin();
	for(; itr != m_mapMemPiece.end(); itr++)
	{
		SafeDeleteArray(itr->second.buf);
	}
	m_mapMemPiece.clear();
	m_lockMemPiece.Unlock();

	m_lockMemIndex.Lock();
	itr = m_mapMemIndexPieces.begin();
	for(; itr != m_mapMemIndexPieces.end(); itr++)
	{
		SafeDeleteArray(itr->second.buf);
	}
	m_mapMemIndexPieces.clear();
	m_lockMemIndex.Unlock();
#else
	for ( unsigned int i  = 0 ; i < m_vctFile.size() ; i++)
	{
		SafeDelete(m_vctFile[i]);
	}
	m_vctFile.clear();
#endif
	SafeDelete(m_seed);
	SafeDelete(m_mem);
	SafeDeleteArray(m_downQue);

	ReleasePieces();
}

bool CTask::FindPiece(int index,CPiece** piece)
{
	CAutoLock lock(m_lockPiece);
	m_mapPieceItr = m_mapPiece.find(index);
	if ( m_mapPieceItr != m_mapPiece.end())
	{
		*piece = m_mapPieceItr->second;
		(*piece)->duplicate();
		return true;
	}
	return false;
}

bool CTask::DelPiece(int index)
{
	CAutoLock lock(m_lockPiece);
	m_mapPieceItr = m_mapPiece.find(index);
	if ( m_mapPieceItr != m_mapPiece.end())
	{
		m_mapPieceItr->second->release();
		m_mapPiece.erase(m_mapPieceItr);
		return true;
	}
	return false;
}

//«Âø’PEICE
void CTask::ReleasePieces()
{	
	CAutoLock lock(m_lockPiece);
	m_mapPieceItr = m_mapPiece.begin();
	for(; m_mapPieceItr != m_mapPiece.end(); m_mapPieceItr ++)
	{
		CPiece *piece = m_mapPieceItr->second;
		piece->release();		
	}
	m_mapPiece.clear();
}

void CTask::Start()
{
	if(m_Para.status == RUNNING)
	{
		return ;
	}
	m_lastStartTime = time(NULL);
	m_Para.status = RUNNING;
	SetStatusInDataBase(RUNNING);

	if (HTTPDOWNTASK == m_Para.type || HLSDOWNTASK == m_Para.type || HLSP2PDOWNTASK == m_Para.type)
	{
		// ��������ģʽ
		UpdateDownloadMode();
	}	
}

void CTask::Pause()
{
	if(m_Para.status == PAUSE)
		return ;
	m_Para.status = PAUSE;
	SetStatusInDataBase(PAUSE);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "CTask::Pause()");
#endif
}

void CTask::Stop()
{
	if(m_Para.status == STOP)
		return ;

	m_Para.status = STOP;
	SetStatusInDataBase(STOP);
}

unsigned char *CTask::GetHash()
{
	return m_Para.hash;
}

bool CTask::TaskIsFinish(bool bCheckFileExist)
{
	if(m_mem)
	{
		return m_mem->IsDownFinish();
	}

	return false;
}

bool CTask::Rename()
{
	vector<CQvodFile *>::iterator it = m_vctFile.begin();
	 if ( BTDOWNLOADTASK == m_Para.type )
	{
		for( ;it != m_vctFile.end();it++ )
		{
			(*it)->Rename();
		}
	}
	else
	{
		return (*it)->Rename();
	}
	return true;
}

// 2: –¥≥…π¶«“Œƒº˛ÕÍ≥�?
// 1: –¥»Î”≤≈Ã≥…π¶
// 0: –¥»Îƒ⁄¥Ê≥…π¶
//-1: –¥”≤≈Ã ß∞�?
//-2: ÷ÿ∏¥ ˝æ�?
//-3: ¥ÌŒÛ ˝æ�?
//-4: Œﬁ–ß ˝æ�?
int CTask::WritePiece(const KEY& key,const char *buf, int len, unsigned int index, unsigned int start)
{
	//∑÷∆¨Œ¥±ªœ¬‘�?
	if(m_mem && !m_mem->CheckIndex(index))
	{
		AutoPtr<CPiece> pPiece;
		if(FindPiece(index,&pPiece)) //∑÷∆¨‘⁄œ¬‘ÿª∫≥Â∂”¡�?
		{
			//∑÷∆¨œ¬‘ÿÕÍ≥…ºÏ≤�?∑¿÷π÷ÿ∏¥∂‡¥Œ–¥»ÎÕ¨øÈ ˝æ�?
			if(pPiece->IsFullFilled())
			{
				return 0;
			}
//			m_lastDownSize = len + 13;//∏¸–¬œ¬‘ÿ∑÷∆¨¥Û–�?œﬁÀŸƒ£øÈ
			//≈–∂œ «∑Òvip∑˛ŒÒ∆˜
			bool bIsVipPeer = false;
			pPiece->Write(start,(char*)buf,len,bIsVipPeer);
			if(pPiece->IsFullFilled())//∑÷∆¨“—¬�?
			{
				//“ª∆¨œ¬‘ÿÕÍ±�?
#ifdef QVODDEBUG
				_OUTPUT(INFO,"one piece download,index:%d\n",index);
#endif // _DEBUG
				int tlen = pPiece->GetBufLen();
				char* pbuf = new char[tlen];
				pPiece->CopyBuffer(pbuf,0,tlen);

				int iResult = WriteData(pbuf,tlen,index,0);
				SafeDeleteArray(pbuf);
				//delete[] pbuf;

#ifdef NETBAR
				CancelDownQue(index);
#endif
				return iResult;
			}
		}
		else	//!FindPiece(index,&pPiece) ∑÷∆¨≤ª‘⁄œ¬‘ÿª∫≥Â∂”¡�?
		{
			return 0;
		}
	}
	else	//m_mem->CheckIndex(index),Index“—æ≠œ¬‘�?
	{
		return 0;
	}
	return 0;
}

void CTask::CheckTimeStamp()
{
	//ºÏ≤Èœ¬‘ÿ∂”¡–”–√ª”–≥¨ ±µƒ
	CAutoLock lock(m_lockPiece);
	if(!m_mapPiece.size())
		return ;
	std::map<int,CPiece*>::iterator it;
	for(it = m_mapPiece.begin();it!=m_mapPiece.end();)
	{
		CPiece* pPiece = it->second;
		//»Áπ˚“—æ≠Õ®π˝∏ﬂÀŸÕ®µ¿ªÚ’ﬂ∆‰À˚Õææ∂œ¬‘ÿÕÍ≥�?
		if(m_mem->CheckIndex(pPiece->GetIndex()))
		{
			it->second->release();
			m_mapPiece.erase(it++);
		}
		else
		{
			if(!pPiece->IsFullFilled() && pPiece->HaveTimeOut())
			{
				CancelDownQue(pPiece->GetIndex());
			}
			it++;
		}
	}
}

bool CTask::IsIndexDownload(int index)
{
	if (m_mem && m_mem->CheckIndex(index))
	{
		return true;
	}

	return false;
}

int CTask::GetOrderIndex()
{
	int index = 0;
	if (m_seed)
	{
		index = (unsigned int)(m_buffingpos/m_seed->GetPieceSize()) ;
	}

	return index;
}

int CTask::GetRandomIndex()
{
	int index = 0;
	if (NULL == m_seed)
	{
		return index;
	}
	QVOD_INT64 iContinueLen = 0;
	unsigned int iPieceCount = m_seed->GetPieceNum();
	unsigned int iPieceSize = m_seed->GetPieceSize();
	QVOD_INT64 iFileSize = m_seed->GetDownFileSize();
	if (iFileSize < 10485760)
	{
		//Œƒº˛–°”⁄10MB,÷±Ω”ÀÊª˙œ¬‘�?
		return stunRand() % iPieceCount;
	}

	if (!m_bHeaderIsFinished)
	{
		//Œƒº˛«�?0%ªπŒ¥œ¬‘ÿÕ�?¥”Õ∑ø™ ºÀ≥–Úœ¬‘�?
		iContinueLen = m_mem->GetContinueLen(0);
		if (iContinueLen * 10 < iFileSize)
		{
			for ( int i = 0; i < iPieceCount; i++)
			{
				if (!m_mem->CheckIndex(i))
				{
					return i;
				}
			}
		}
		else
		{
			m_bHeaderIsFinished = true;
#ifdef QVODDEBUG
			_OUTPUT(INEED,"********Œƒº˛«�?0% ˝æ›œ¬‘ÿÕÍ±�?!\n********");
#endif
		}
	}
	else if (!m_bEndingIsFinished)
	{
		//Œƒº˛Œ�?MB ˝æ›Œ¥œ¬‘ÿÕ�?À≥–Úœ¬‘ÿÕÍ∫Û1MB ˝æ�?
		QVOD_INT64 pos = iFileSize - 1048576;
		iContinueLen = m_mem->GetContinueLen(pos);
		if (iContinueLen != 1048576)
		{
			index = (pos / iPieceSize) - 1;
			for (; index < iPieceCount; index++)
			{
				if (!m_mem->CheckIndex(index))
				{
					return index;
				}
			}
		}
		else
		{
			m_bEndingIsFinished = true;
#ifdef QVODDEBUG
			_OUTPUT(INEED,"********Œƒº˛Œ�?MB ˝æ›œ¬‘ÿÕÍ±�?!\n********");
#endif
		}
	}
	
	index = (int)(stunRand() % iPieceCount);
	return index;
}

int CTask::GetNextIndex(CPeer* pPeer)
{
	if ( m_downQue == NULL || NULL==m_mem || NULL==m_seed)
	{
		return -1;
	}
	int slen = m_mem->GetBitFieldLen();
	int plen = pPeer->GetBitFieldLen();
		
	if(slen != plen)
	{
#ifdef QVODDEBUG
		if(plen != 0)
		{
			_OUTPUT(ERROR1,"GetNextIndex bitfield length err,%d %d!!\n",slen,plen);
		}
#endif // _DEBUG
		//∂‘∑Ωªπ√ª”–¥´π˝¿¥BITFIELD
		return -1;
	}
	int PieceNum = m_seed->GetPieceNum();
	if(!PieceNum)
	{
		return -1;
	}
	
	//À≥–Ú¥”≤•∑≈Œª÷√œ¬‘�?
	int index = 0;
	int i = 0;
	int pos = 0;

	{
#ifdef NETBAR
		//¥”∂¡»°Œƒº˛Œª÷√œ¬ £”‡≤ø∑�?
		index = m_buffingpos/m_seed->GetPieceSize();
		i = index >> 3;
		pos = index % 8;
		while(i < slen)
		{
			unsigned char bit = 0;
			if (pPeer->isSuperNode())
			{
				bit =(~pSBitField[i]) & (~pSDownQue[i]);
			}
			else
			{
				bit =(~pSBitField[i]) & (~pSDownQue[i]) & pPBitField[i] ;
			}

			while(bit && pos < 8)
			{
				if (bit & (0x80 >> pos))
				{
					int tmp = 8*i +pos;
					if(tmp < PieceNum && tmp < m_playerIndex + m_MaxPieces && tmp >= m_playerIndex)
					{
						//_OUTPUT(INFO, "in getnextpos down next index %d, tmp %d, bufpos %u, playerindex %d\n", index, tmp, m_buffingpos, m_playerIndex);
						return tmp;
					}
					break;
				}
				pos++;
			}
			i++;
			pos =0;
		}

		m_isNeedDownload = false;
#else
#ifndef SUPERNODE
		if (m_bIsPlaying)
		{
			//≤•∑≈»ŒŒÒ∫Õ‘∆∑´º”ÀŸ»ŒŒÒ,À≥–Úœ¬‘�?
			index = GetOrderIndex();
		}
		else
		{
			//∑«≤•∑≈»ŒŒÒªÚ’ﬂ“˛≤ÿ»ŒŒÒ,ÀÊª˙œ¬‘�?
			index = GetRandomIndex();
		}
#endif		
#endif
	}

	unsigned char* pSBitField = m_mem->GetMemBit();
	unsigned char* pSDownQue = m_downQue;
	unsigned char* pPBitField =(unsigned char*) pPeer->GetBitField();

	i = index >> 3;
	pos = index % 8;
	while(i < slen)
	{
		unsigned char bit = (~pSBitField[i]) & (~pSDownQue[i]) & pPBitField[i];
		while(bit && pos < 8)
		{
			if(bit & (0x80 >> pos))
			{
				int tmp = 8*i +pos;
				{
					if(tmp < PieceNum)
						return tmp;
					break;
				}				
			}
			pos++;
		}
		i++;
		pos = 0;
	}

	//¥”Õ∑œ¬‘ÿ £”‡≤ø∑�?
	i = 0;
	pos = 0;	
	while(i < slen)
	{
		unsigned char bit = (~pSBitField[i]) & (~pSDownQue[i]) & pPBitField[i];
		while(bit && pos < 8)
		{
			if (bit & (0x80 >> pos))
			{
				int tmp = 8*i +pos;
				{
					if(tmp < PieceNum)
						return tmp;
					break;
				}			
			}
			pos++;
		}
		i++;
		pos = 0;
	}

	//¥”Õ∑≤È’“œ¬‘ÿ∂”¡–÷–µƒ∑÷∆�?
	i = 0;
	pos = 0;
	while(i < slen)
	{
		unsigned char bit = (~pSBitField[i]) & pSDownQue[i] & pPBitField[i];
		while(bit && pos < 8)
		{
			if(bit & (0x80 >> pos))
			{
				int tmp = 8*i +pos;
				{
					if(tmp < PieceNum)
						return tmp;
					break;
				}				
			}
			pos++;
		}
		i++;
		pos = 0;
	}
	return -1; 
}

int CTask::GetNextIndexEx(CPeer* pPeer)
{
	if ( m_downQue == NULL || NULL==m_mem || NULL==m_seed)
	{
		return -1;
	}
	int slen = m_mem->GetBitFieldLen();
	int plen = pPeer->GetBitFieldLen();

	if(slen != plen)
	{
		if (0 == plen && pPeer->IsHttpPeer())
		{
			pPeer->FullBitField(slen);
			plen = pPeer->GetBitFieldLen();
			if (plen != slen)
			{
				return -1;
			}
		}
		else
		{
#ifdef QVODDEBUG
			if(plen != 0)
			{
				_OUTPUT(ERROR1,"GetNextIndex bitfield length err,%d %d!!\n",slen,plen);
			}
#endif // _DEBUG
			//∂‘∑Ωªπ√ª”–¥´π˝¿¥BITFIELD
			return -1;
		}
	}
	int PieceNum = m_seed->GetPieceNum();
	if(!PieceNum)
	{
		return -1;
	}
	unsigned char* pSBitField = m_mem->GetMemBit();
	unsigned char* pSDownQue = m_downQue;
	unsigned char* pPBitField =(unsigned char*) pPeer->GetBitField();

	int index = (unsigned int)(m_buffingpos/m_seed->GetPieceSize());
	int i = index >> 3;
	int pos = index % 8;	

	while(i < slen )
	{
		unsigned char bit =(~pSBitField[i]) & (~pSDownQue[i]) & pPBitField[i] ;
		while(bit && pos < 8)
		{
			if (bit & (0x80 >> pos))
			{
				int tmp = 8*i +pos;
				if(tmp < PieceNum)
					return tmp;
				break;
			}
			pos++;
		}
		i++;
		pos =0;
	}

	//¥”Õ∑œ¬‘ÿ £”‡≤ø∑�?
	i = 0;
	pos = 0;
	while(i < slen )
	{
		unsigned char bit =(~pSBitField[i]) & (~pSDownQue[i]) & pPBitField[i] ;
		while(bit && pos < 8)
		{
			if (bit & (0x80 >> pos))
			{
				int tmp = 8*i +pos;
				if(tmp < PieceNum)
					return tmp;
				break;
			}
			pos++;
		}
		i++;
		pos =0;
	}

	//¥”Õ∑≤È’“œ¬‘ÿ∂”¡–÷–µƒ∑÷∆�?
	i = 0;
	pos = 0;
	while(i < slen )
	{
		unsigned char bit =(~pSBitField[i]) & pSDownQue[i] & pPBitField[i] ;
		while(bit && pos < 8)
		{
			if (bit & (0x80 >> pos))
			{
				int tmp = 8*i +pos;
				if(tmp < PieceNum)
					return tmp;
				break;
			}
			pos++;
		}
		i++;
		pos =0;
	}
	return -1;
}

int CTask::GetDownloadingIndex(CPeer *pPeer)
{
	CAutoLock lock(m_lockPiece);
	m_mapPieceItr = m_mapPiece.begin();
	unsigned char *pBitfield = pPeer->GetBitField();
	if ( pBitfield == NULL )
	{
		//have∞¸‘⁄±»ÃÿŒª∞¸÷Æ«∞ ’µ�?
		return -1;
	}
	while ( m_mapPieceItr != m_mapPiece.end() )
	{
		if ( !m_mapPieceItr->second->IsFullFilledOrDownloading() )
		{
			int index = m_mapPieceItr->first;
			int pos = index % 8;
			if ( pBitfield[index>>3] & (0x80 >> pos))
			{
				return index;
			}
		}
		m_mapPieceItr++;
	}
	return -1;
}

bool CTask::IsExistEmergencyPiece()
{
	if (NULL == m_seed)
	{
		return false;
	}
	unsigned int iPieceSize = m_seed->GetPieceSize();
	if (0 == iPieceSize)
	{
		return false;
	}

	unsigned int index = m_iPlayPos/iPieceSize;
	if (m_mem && m_mem->CheckIndex(index))
	{
		return false;
	}

	return false;
}

//bool CTask::GetEmergencyPiece(request& req)
//{
//	unsigned int iPieceSize = m_seed->GetPieceSize();
//	unsigned int index = m_iPlayPos/iPieceSize;
//	AutoPtr<CPiece> pPiece;
//	if(!FindPiece(index,&pPiece))
//	{
//		return false;
//	}
//	unsigned int subindex = 0;
//	int nsubindex = pPiece->GetEmergencySubField(subindex);
//	if (nsubindex > 0)
//	{
//		req.len = htonl(13);
//		req.id = REQUEST;
//		req.index= htonl(index);
//		req.begin = htonl(subindex*BLOCK_SIZE);
//		QVOD_INT64 len;//±æ∆¨Œƒº˛ £”‡≥§∂�?
//		if(index == m_seed->GetPieceNum() -1)//◊Ó∫Û“ª∆¨
//			len = m_seed->GetDownFileSize() - (QVOD_INT64)index*iPieceSize - subindex*BLOCK_SIZE;
//		else
//			len = iPieceSize - subindex*BLOCK_SIZE;
//		req.length = htonl(min((unsigned int)len,unsigned int(nsubindex*BLOCK_SIZE)));
//		return true;
//	}
//	return false;
//}

int CTask::DownNextPiece(const KEY& key,request& req)
{
	AutoPtr<CPeer> pPeer;
	if(!m_peers->FindPeer(key,&pPeer))
	{
		return -1;
	}
	int index = -1;
	{
		if (pPeer->IsHttpPeer())
		{
			return -1;
		}

		if( m_bIsHide || !m_bIsPlaying )
		{
			index = GetDownloadingIndex(pPeer);
		}
		else
		{
			//À≥–Úœ¬‘�?æ°øÏ∂œø™≤ª‘⁄µ±«∞œ¬‘ÿ∑∂Œßƒ⁄µƒ∑˛ŒÒ∆˜
			bool bCache = pPeer->IsCacheServer();
			bool bCloud = pPeer->IsVipAccServer();
			bool bMiner = pPeer->IsMinerServer();
			QVOD_INT64 iBeginPos = pPeer->GetServerFileBeginPos();
			unsigned int iDataLen = pPeer->GetServerFileDataLen();
			if ((bCache || bCloud || bMiner) && 
				((iBeginPos > m_buffingpos) || (iBeginPos+iDataLen <= m_buffingpos)))
			{
#ifdef QVODDEBUG
				_OUTPUT(INEED,"QVODTRACKER_%s: Server No Data Can Download!!begin:%I64u,len:%u,buffingPos:%I64d\n",
					bCache?"Cache":(bCloud?"Cloud":(bMiner?"Miner":"Unknown")),iBeginPos,iDataLen,m_buffingpos);
#endif
				return -4;
			}
		}
	}
	
	//ªÒ»°œ¬“ª∏ˆø…œ¬‘ÿ∑÷∆�?œ»¥”ª∫≥ÂŒª÷√œÚ∫Û’“ø…œ¬‘ÿ∆¨
	if(index == -1)
	{
		index = GetNextIndex(pPeer);
		if(index < 0)
		{
			return index;
		}
	}

#ifdef NETBAR
	if (index >= m_playerIndex + m_MaxPieces || index < m_playerIndex)
	{
		return false;
	}
#endif
	AutoPtr<CPiece> pPiece;
	if(!FindPiece(index,&pPiece))
	{
		//±£÷§ª∫¥Ê∑÷∆¨ƒ⁄¥Ê’º”√≤ª¥Û”⁄30M
		int memsize = GetBufPieceSize();
		if(memsize >= 30*1024*1024)
		{
			//cache request
			return -2;
		}
		unsigned int iPieceSize = m_seed->GetPieceSize();
		QVOD_INT64 maxlen = QVOD_INT64(m_seed->GetDownFileSize())-QVOD_INT64(index)*QVOD_INT64(iPieceSize);
		UINT len = (UINT)min(QVOD_INT64(iPieceSize),maxlen);
		int timeout = PIECE_TIMEOUT;
		if ( !m_bIsPlaying )
		{
			//∑«µ„≤•»ŒŒÒªÚ’ﬂµ±«∞Œ™ÀÊª˙p2pœ¬‘ÿµƒ≥¨ ± ±º‰—”≥§1±�?
			timeout <<= 1;
		}

		CPiece* piece = new CPiece(len,index,timeout);
		if(piece == NULL)
		{
			return -1;
		}
		m_lockPiece.Lock();
		m_mapPiece.insert(std::make_pair(index,piece));
		m_lockPiece.Unlock();
		if(!FindPiece(index,&pPiece))
		{
			return -1;
		}
	}

	bool bfull;
	int nsubindex = 1;
	if(pPeer->isSuperNode() && pPeer->GetNatType() == 0)
	{
		nsubindex = 65536/BLOCK_SIZE;
	}
	int subindex = pPiece->GetNextSubField(bfull,nsubindex);
	if(subindex == -1) 
	{
		//≤ª¥Ê‘⁄ø…“‘œ¬‘ÿµƒ∑÷∆�?cache request
		return -2;
	}
	if(bfull)//À˘”–◊”∆¨“—œ¬‘ÿªÚ’˝‘⁄œ¬‘ÿ÷�?
	{
		SetDownQue(index);
	}

	req.len = htonl(13);
	req.id = REQUEST;
	req.index= htonl(index);
	req.begin = htonl(subindex*BLOCK_SIZE);
	QVOD_INT64 len;//±æ∆¨Œƒº˛ £”‡≥§∂�?
	if(index == m_seed->GetPieceNum() -1)//◊Ó∫Û“ª∆¨
		len = m_seed->GetDownFileSize() - (QVOD_INT64)index*m_seed->GetPieceSize() - subindex*BLOCK_SIZE;
	else
		len = m_seed->GetPieceSize() - subindex*BLOCK_SIZE;
	req.length = htonl(min((unsigned int)len,(unsigned int)(nsubindex*BLOCK_SIZE)));
	//±£¥Ê«Î«ÛøÈ–≈œ¢,“‘π©∫Û–¯piece∑µªÿ ±∫Ú—È÷§ «∑Ò «Œ“√««Î«ÛµƒøÈ
	SetReqPiecePacketInfo(pPeer,index,subindex*BLOCK_SIZE);
	return 0;
}

bool CTask::DownNextPiece(const KEY& key,httpiecereq& hreq)
{
	AutoPtr<CPeer> pPeer;
	if ( !m_peers->FindPeer(key,&pPeer))
	{
		return false;
	}
	//ªÒ»°œ¬“ª∏ˆø…œ¬‘ÿ∑÷∆�?œ»¥”ª∫≥ÂŒª÷√œÚ∫Û’“ø…œ¬‘ÿ∆¨
	int index = GetNextIndexEx(pPeer);
	if (index != -1)
	{
		AutoPtr<CPiece> pPiece;
		if( !FindPiece(index,&pPiece))
		{
			//±£÷§ª∫¥Ê∑÷∆¨ƒ⁄¥Ê’º”√≤ª¥Û”⁄30M
			int memsize = GetBufPieceSize();
			if(memsize >= 30*1024*1024)
			{
				return false;
			}
			QVOD_INT64 maxlen = m_seed->GetDownFileSize()-(QVOD_INT64)index*m_seed->GetPieceSize();
			UINT len = min(m_seed->GetPieceSize(),(unsigned int)maxlen);
			int timeout = PIECE_TIMEOUT;
			if ( !m_bIsPlaying )
			{
				timeout <<= 1;
			}
			CPiece* piece = new CPiece(len,index,timeout);
			if(!piece)
			{
				return false;
			}
			CAutoLock lock(m_lockPiece);
			m_mapPiece.insert(std::map<int,CPiece*>::value_type(index,piece));
			if(!FindPiece(index,&pPiece))
			{
				return false;
			}
		}
		bool bfull;
		int nsubindex = 1;
		if(pPeer->isSuperNode() && pPeer->GetNatType() == 0)
		{
			nsubindex = 65536/BLOCK_SIZE;
		}
		int subindex = pPiece->GetNextSubField(bfull,nsubindex);
		if(bfull)//À˘”–◊”∆¨“—œ¬‘ÿªÚ’˝‘⁄œ¬‘ÿ÷�?
		{
			SetDownQue(index);
		}
		if(subindex == -1) 
		{
			subindex = pPiece->GetTimeoutSubField();
#ifdef QVODDEBUG
			_OUTPUT(INFO, "http index %d, GetTimeoutSubPiece %d\n", index, subindex);
#endif // _DEBUG
			if (subindex == -1)
			{
				//≤ª”¶∏√Ω¯»Î’‚¿�?≤ª¥Ê‘⁄ø…“‘œ¬‘ÿµƒ∑÷∆�?
				return false;
			}
		}
		hreq.start = index*m_seed->GetPieceSize() + subindex*BLOCK_SIZE;
		QVOD_INT64 len;//±æ∆¨Œƒº˛ £”‡≥§∂�?
		if(index == m_seed->GetPieceNum() -1)//◊Ó∫Û“ª∆¨
			len = m_seed->GetDownFileSize() - (QVOD_INT64)index*m_seed->GetPieceSize() - subindex*BLOCK_SIZE;
		else
			len = m_seed->GetPieceSize() - subindex*BLOCK_SIZE;
		hreq.length = min((unsigned int)len,(unsigned int)(nsubindex*BLOCK_SIZE));
		//±£¥Ê«Î«ÛøÈ–≈œ¢,“‘π©∫Û–¯piece∑µªÿ ±∫Ú—È÷§ «∑Ò «Œ“√««Î«ÛµƒøÈ
		SetReqPiecePacketInfo(pPeer,index,subindex*BLOCK_SIZE);
		return true;
	}
	else
	{
		return false;
	}
}

bool CTask::SetDownQue(unsigned int index)
{
	if(index<0 ||index >= m_seed->GetPieceNum() ||!m_downQue)
	{
		return false;
	}
	int Pos= index >> 3;
	int re = index & 0x07;
	if (m_downQue[Pos] & 0x80>>re)//“—æ≠œ¬‘ÿ÷–¡À
	{
		return false;
	}
	m_downQue[Pos] |= 0x80>>re;//º”»Îœ¬‘ÿ÷–∂”¡�?
	return true;
}

bool CTask::CancelDownQue(int index)
{
	if(!m_mem || !m_mem->GetMemBit() || index < 0 || index >= int(m_mem->GetBitFieldLen()<<3))
	{
		return false;
	}
	int Pos= index >> 3;
	int re = index & 0x07;
	m_downQue[Pos] &= ~(0x80>>re);
	return true;
}

int CTask::SetFileIndex(unsigned char index)
{
	if(index > m_vctFile.size())
	{
		return 2;
	}
	m_fileindex = index;
	m_buffingpos = 0;
	for(int i =0 ;i<m_fileindex;i++)
	{
		m_buffingpos += m_vctFile[i]->GetFileLen();
	}
	UpdateBuffingPos(m_buffingpos);
	SetPlayFileIndex();
	return 0;
}

void CTask::SetBuffingPos(QVOD_INT64 setpos)
{
#ifndef NETBAR
	for(int i =0;i<m_fileindex;i++)
	{
		setpos += m_vctFile[i]->GetFileLen();
	}
#endif
	UpdateBuffingPos(setpos);
}

void CTask::UpdateBuffingPos(QVOD_INT64 setpos)
{
	if (NULL==m_seed || NULL==m_mem)
	{
		return;
	}
	int piecelen = m_seed->GetPieceSize();
	int piecenum = m_seed->GetPieceNum();
	if(piecelen == 0)
	{
		return;
	}
	int index = int(setpos/piecelen);
	setpos = (QVOD_INT64)index * (QVOD_INT64)piecelen;
	for(;index < piecenum;index++)
	{
		if(m_mem->CheckIndex(index))
		{
			setpos += piecelen;
		}
		else
		{
			break;
		}
	}
	if(setpos >= m_seed->GetFileTotleLen())
	{
		m_buffingpos = 0;
	}
	else
	{
		m_buffingpos = setpos;
	}
}

TaskType CTask::GetTaskType(void)
{
	return (TaskType)m_Para.type;
}

CSeed *CTask::GetSeed(void)
{
	if(m_seed)
	{
		return m_seed;
	}

	return NULL;
}

CMem *CTask::GetMemFile(void)
{
	if(m_mem)
	{
		return m_mem;
	}

	return NULL;
}

void CTask::SetPlayFileIndex()
{
	if(!m_seed)
	{
		return ;
	}

	int file_num = m_vctFile.size();

	if(file_num == 1)
	{
		if(m_seed->GetPieceSize() == 0)
		{
			return ;
		}

		unsigned int index = (UINT)(m_seed->GetDownFileSize() % m_seed->GetPieceSize());		
		if(index == 0)
		{
			index = (UINT)(m_seed->GetDownFileSize() / m_seed->GetPieceSize());
		}
		else
		{
			index = (UINT)(m_seed->GetDownFileSize() / m_seed->GetPieceSize() + 1);
		}		
		m_playfileIndex = index;
	}
	else if(file_num > 1)
	{
		if(m_seed->GetPieceSize() == 0)
		{
			return ;
		}
		
		QVOD_INT64 len = 0;
		for(int i=0; i<=m_fileindex; i++)
		{
			len += m_seed->GetFileLen(i);
		}
		unsigned int index = (UINT)(len % m_seed->GetPieceSize());		
		if(index == 0)
		{
			index = (UINT)(len / m_seed->GetPieceSize());
		}
		else
		{
			index = (UINT)(len / m_seed->GetPieceSize() + 1);
		}
		m_playfileIndex = index;
	}
}

void CTask::InterDownSize(int size, const KEY &key)
{
	AutoPtr<CPeer> pPeer;
	if ( ProcessQueryPeers()->FindPeer(key,&pPeer))
	{
		pPeer->InterDownSize(size);
	}
	m_xSecDownBufSize += size;
}

void CTask::InterUpSize(int size,const KEY &key)
{
	m_xSecUpBufSize += size;
}

bool CTask::IsNeedCacheDownRequest()
{
	if(m_limitDownSpeed==0)
	{
		return false;
	}
	return true;
}


bool CTask::IsOvertakeDownSpeed()
{
	if(m_limitDownSpeed == 0)
	{
		return false;
	}
	else
	{
		if(m_nextDownTime > QvodGetTime())
		{
			return true;
		}
	}
	return false;
}

void CTask::SetDownSpeed(int speed)
{
	if(speed == m_limitDownSpeed)
	{
		return ;
	}
	m_limitDownSpeed=speed;
	if ( m_limitDownSpeed != 0 )
	{
		m_downQueueTime = m_lastDownSize / m_limitDownSpeed;
	}
}

int CTask::GetUploadSpeed()
{
	int speed=(int)m_xSecUpBufSize/SPCINTERVALTIME;
	if(speed>0)
		return speed;
	else
		return 0;
}

int CTask::GetDownSpeed(void)
{
	int speed=(int)m_xSecDownBufSize/SPCINTERVALTIME;
	if(speed>0)
		return speed;
	else
		return 0;
}


void CTask::UpdateNextDownTime(int lastSize)
{
	//∏¸–¬◊ÓΩ¸œ¬‘ÿ ˝æ›∞¸¥Û–�?
	if( lastSize > 0 )
	{
		m_lastDownSize = lastSize;
	}
	//∏¸–¬œ¬‘ÿº‰∏Ù ±º�?
	if( m_limitDownSpeed == 0 )
	{
		return ;
	}
	else
	{
		m_downQueueTime = m_lastDownSize / m_limitDownSpeed;
	}

	QVOD_DWORD curtime=QvodGetTime();
	if( m_nextDownTime==0 || (curtime > m_nextDownTime + int(m_downQueueTime*2)))
	{
		m_nextDownTime = curtime + m_downQueueTime;
	}
	else
	{
		m_nextDownTime += m_downQueueTime;
	}
}

int CTask::GetBufPieceSize()
{
	int bufsize=0;
	CAutoLock lock(m_lockPiece);
	std::map<int,CPiece*>::iterator it = m_mapPiece.begin();
	for(;it!=m_mapPiece.end();++it)
	{
		bufsize+=it->second->GetMemSize();
	}
	return bufsize;
}

unsigned int CTask::GetCurDownNum(KEY& key)
{
	CAutoLock lock(m_downReqInfoLock);
	AutoPtr<CPeer> pPeer;
	if ( m_peers->FindPeer(key,&pPeer))	
	{
		return pPeer->m_reqDownNum;
	}
	return 0;
}

bool CTask::CancelTimeoutReq(KEY& key,int index,int begin,int length)
{
	AutoPtr<CPiece> pPiece;
	if(FindPiece(index,&pPiece)) 
	{
		int len = length + BLOCK_SIZE - 1;
		for(int i = 0;i < (len/BLOCK_SIZE);i++)
		{
			unsigned int subindex = (begin + i*BLOCK_SIZE) / BLOCK_SIZE;//>> 14;
			pPiece->CancelSubIndex(subindex);
		}
		CancelDownQue(index);
		CancelIndexInfo(key,index,begin);
		return true;
	}
	return false;
}

bool CTask::CancelIndexInfo(KEY& key,int index,int begin)
{
	bool result = false;
	AutoPtr<CPeer> pPeer;
	if ( m_peers->FindPeer(key,&pPeer))	
	{
		CAutoLock lock(m_downReqInfoLock);
		for( unsigned int i=0;i<pPeer->m_reqDownNum;i++)
		{
			if(pPeer->m_reqIndex[i] == index && pPeer->m_reqBegin[i] == begin)
			{
				if(i != pPeer->m_reqDownNum - 1)
				{
					pPeer->m_reqIndex[i] = pPeer->m_reqIndex[pPeer->m_reqDownNum - 1];
					pPeer->m_reqBegin[i] = pPeer->m_reqBegin[pPeer->m_reqDownNum - 1];

				}
				pPeer->m_reqIndex[pPeer->m_reqDownNum - 1] = -1;
				pPeer->m_reqBegin[pPeer->m_reqDownNum - 1] = -1;
				if(pPeer->m_reqDownNum > 0 && pPeer->m_reqDownNum <= REQUEST_NUM)
				{
					pPeer->m_reqDownNum--;
				}
				result = true;
				break;
			}
		}
		if(result)
		{
			pPeer->UpdateStatus(CPeer::RUNNING);
		}
	}
	return result;
}

bool CTask::CancelPeerIndexInfo(KEY& key)
{
	AutoPtr<CPeer> pPeer;
	if ( m_peers->FindPeer(key,&pPeer))	
	{
		CAutoLock lock(m_downReqInfoLock);
		for( unsigned int i=0;i<pPeer->m_reqDownNum;i++)
		{
			int index = pPeer->m_reqIndex[i];
			int begin = pPeer->m_reqBegin[i];
			if (index > -1)
			{
				AutoPtr<CPiece> pPiece;
				if(FindPiece(index,&pPiece)) 
				{
					unsigned int subindex = begin / BLOCK_SIZE;
					pPiece->CancelSubIndex(subindex);
					CancelDownQue(index);
				}
			}
		}
		return true;
	}
	return false;
}

bool CTask::IsValidPiecePacket(KEY& key,int index,int begin,int length)
{
	bool result=false;
	AutoPtr<CPeer> pPeer;
	if ( m_peers->FindPeer(key,&pPeer))	
	{
		CAutoLock lock(m_downReqInfoLock);
		for( unsigned int i=0;i<pPeer->m_reqDownNum;i++)
		{	
			if(pPeer->m_reqIndex[i] == index && pPeer->m_reqBegin[i] == begin)
			{
				//◊Ó∫Û“ª∆¨ªÚ’ﬂƒ‹±�?BLOCK_SIZE ’˚≥˝
				if((index==m_seed->GetPieceNum()-1) || !(length % BLOCK_SIZE))
				{
					if(i != pPeer->m_reqDownNum - 1)
					{
						pPeer->m_reqIndex[i] = pPeer->m_reqIndex[pPeer->m_reqDownNum - 1];
						pPeer->m_reqBegin[i] = pPeer->m_reqBegin[pPeer->m_reqDownNum - 1];
					}
					pPeer->m_reqIndex[pPeer->m_reqDownNum - 1] = -1;
					pPeer->m_reqBegin[pPeer->m_reqDownNum - 1] = -1;
					if(pPeer->m_reqDownNum > 0 && pPeer->m_reqDownNum <= REQUEST_NUM)
					{
						pPeer->m_reqDownNum--;
					}
					result=true;
					break;
				}
			}
		}
		if(result)
		{
			pPeer->UpdateStatus(CPeer::RUNNING);
		}
	}
	return result;
}

void CTask::SetReqPiecePacketInfo(CPeer* pPeer,int index,int begin)
{
	CAutoLock lock(m_downReqInfoLock);
	if(pPeer->m_reqDownNum >= 0 && pPeer->m_reqDownNum < REQUEST_NUM)
	{
		pPeer->m_reqDownNum++;
	}
	pPeer->m_reqIndex[pPeer->m_reqDownNum-1]=index;
	pPeer->m_reqBegin[pPeer->m_reqDownNum-1]=begin;
	if(pPeer->m_reqDownNum == REQUEST_NUM)
	{
		pPeer->UpdateStatus(CPeer::WAIT);
	}
}

void CTask::UpdatePieceFinish(int iIndex)
{
	UpdateBuffingPos(m_buffingpos);//∏¸–¬ª∫≥ÂŒª÷√
	DelPiece(iIndex);
}

//Ω´ ˝æ›ø‚µƒ≤Ÿ◊˜Õ≥“ª∏¯Task
bool CTask::SetStatusInDataBase(unsigned char status)
{
#ifndef NETBAR
	int iRet1 = 0;
	switch( status )
	{
	case CTask::RUNNING:
	case CTask::PAUSE:
		iRet1 = CDbTaskConfig::Instance()->SetStatus(m_Para.hash,status);
		break;
	case CTask::STOP:
		iRet1 = CDbTaskConfig::Instance()->Delete(m_Para.hash);
		break;
	case CTask::PLAY:
		iRet1 = CDbTaskConfig::Instance()->SetStatus(m_Para.hash,CTask::RUNNING);
		break;
	default:
		break;
	}
	if ( iRet1 != 0 )
	{
		return false;
	}
#endif //NETBAR
	return true;
}

bool CTask::FullPeerBitField(KEY& key)
{
	AutoPtr<CPeer> pPeer;
	if ( m_peers->FindPeer(key,&pPeer))	
	{
		if(m_mem && m_mem->GetBitFieldLen() > 0)
		{
			pPeer->FullBitField(m_mem->GetBitFieldLen());
			return true;
		}
	}
	return false;
}

QVOD_INT64 CTask::GetFileSize()
{
	if(m_seed)
	{
		return m_seed->GetFileTotleLen();
	}
	return 0;
}

int CTask::GetPieceNum()
{
	if (m_seed)
	{
		return m_seed->GetPieceNum();
	}
	return 0;
}

int CTask::SetReferUrl(const std::string &strRef)
{
	m_Para.strRefUrl = strRef;

#ifndef NETBAR
	CDbTaskConfig::Instance()->UpdateTask(m_Para);
#endif //NETBAR

	return 0;
}

int CTask::SetDownloadPos(QVOD_INT64 iPos,bool fromUser)
{
	SetBuffingPos(iPos);
	if(fromUser)
	{
		unsigned int iPieceLen = m_seed->GetPieceSize();
		m_dragpos = iPos;
		if (iPieceLen > 0)
		{
			m_dragindex = (unsigned int)(iPos / iPieceLen);
		}
	}
	return 0;
}

int CTask::GetDownloadPos(QVOD_UINT64& iPos)
{
	iPos = m_buffingpos;
	return 0;
}

int CTask::SetPlayFileIndexPos(QVOD_INT64 iPos)
{
	if(!m_seed)
	{
		return 0;
	}
	unsigned int iPieceLen = m_seed->GetPieceSize();
	int iFileNum = m_seed->GetFileNum();
	if ( iPieceLen == 0 )
	{
		return -1;
	}
	if ( iFileNum > 1)
	{
		for(int i = 0;i <= m_fileindex;i++)
		{
			iPos += m_seed->GetFileLen(i);
		}		
	}
	unsigned int index = (unsigned int)(iPos / iPieceLen);
	if ( (iPos % iPieceLen) == 0 )
	{
		m_playfileIndex = index;
	}
	else
	{
		m_playfileIndex = index + 1;
	}
	return 0;
}

#ifdef NETBAR
bool CTask::IsNeedDownload()
{
	return m_isNeedDownload;
}

void CTask::DelAllMemPieces()
{
	m_lockMemPiece.Lock();
	map<int, PieceBuffer>::iterator itr = m_mapMemPiece.begin();
	for(; itr != m_mapMemPiece.end(); itr++)
	{
		if (m_mem->ClearBitField(itr->first))//«Â≥˝±»ÃÿŒ�?
		{
			m_mem->ClearDownLength(itr->second.len);
			m_curDownSize -= itr->second.len;
		}
		delete[] itr->second.buf;
	}
	m_mapMemPiece.clear();
	m_lockMemPiece.Unlock();

	m_lockMemIndex.Lock();
	itr = m_mapMemIndexPieces.begin();
	for(; itr != m_mapMemIndexPieces.end(); itr++)
	{
		if (m_mem->ClearBitField(itr->first))//«Â≥˝±»ÃÿŒ�?
		{
			m_mem->ClearDownLength(itr->second.len);
			m_curDownSize -= itr->second.len;
		}
		delete[] itr->second.buf;
	}
	m_mapMemIndexPieces.clear();
	m_lockMemIndex.Unlock();

	memset(m_downQue,0,m_mem->GetBitFieldLen());
	ReleasePieces();
}

QVOD_INT64 CTask::GetSuperNodeDownLen()
{
	AutoPtr<CPeer> pPeer;
	QVOD_INT64 iDownlen = 0;
	QVOD_INT64 iFileLen = m_seed->GetFileTotleLen();
	unsigned int piecenum = m_seed->GetPieceNum();
	if (m_peers->GetSuperNode(&pPeer) && piecenum > 0)
	{	
		int i = 0;
		int pos = 0;
		unsigned int downpieces = 0;
		int iBitFieldLen = pPeer->GetBitFieldLen();
		unsigned char* pBitField = pPeer->GetBitField();
		while(i < iBitFieldLen)
		{
			unsigned char bit = pBitField[i];
			while(pos < 8)
			{
				if (bit & 0x01)
				{
					downpieces++;
				}
				pos++;
				bit >>= 1;
			}
			i++;
			pos = 0;
		}
		iDownlen = iFileLen * downpieces/piecenum;
	}
	return iDownlen;
}
#endif

bool CTask::IsFileSizeMatching()
{
	if(!m_seed)
	{
		return false;
	}
	qvod_stat_t stat_info;
	string strPath = m_strPath + m_strFileName;
	int iRet = QVOD_STAT(strPath.c_str(), &stat_info);
	if (iRet != 0)
	{
		return false;
	}

	if (stat_info.st_size != m_seed->GetFileTotleLen())
	{
		return false;
	}
	return true;
}

unsigned int CTask::GetTaskErrorMask()
{
	return m_iErrorMask;
}

int CTask::SetStartPlay()
{
	m_bIsPlaying = true;
	return 0;
}

int CTask::SetStopPlay()
{
	m_bIsPlaying = false;
	return 0;
}

int CTask::SetPlaying( bool bPlaying )
{
	m_bIsPlaying = bPlaying;
	return 0;
}

bool CTask::IsPlaying()
{
	return m_bIsPlaying;
}

QVOD_INT64 CTask::QueryContinueLen(QVOD_INT64 pos)
{
	if(m_mem)
	{
		return m_mem->GetContinueLen(pos);
	}
	return 0;
}

int CTask::SetPlayFileRate(unsigned int iRate)
{
	m_iPlayFileRate = iRate;

#ifdef QVODDEBUG
	_OUTPUT(INFO,"%s SetPlayFileRate %u\n",Hash2Char(m_Para.hash).c_str(),iRate);
#endif

	return 0;
}

unsigned int CTask::GetPlayFileRate()
{
	return m_iPlayFileRate;
}

int CTask::SetPlayFilePos(QVOD_INT64 pos)
{
	m_iPlayPos = pos;

#ifdef QVODDEBUG
	const unsigned int nPieceSize = m_seed->GetPieceSize();  
	_OUTPUT(INFO,"SetPlayFilePos %lld  picec=%lld\n",pos, nPieceSize? m_iPlayPos/nPieceSize: 0);
#endif	

	return 0;
}

int CTask::GetPeerIspID(const KEY &key,unsigned char *pIspID)
{
	AutoPtr<CPeer> pPeer;
	if ( m_peers->FindPeer(key,&pPeer) )
	{
		return pPeer->GetPeerIspID(pIspID);
	}
	return 0;
}

int CTask::SetPeerIspID(const KEY &key,unsigned char *pIspID)
{
	AutoPtr<CPeer> pPeer;
	if ( m_peers->FindPeer(key,&pPeer) )
	{
		return pPeer->SetPeerIspID(pIspID);
	}
	return 0;
}

bool CTask::IsOptimizeUpload()
{
	if ( m_bOptimizeUpload)
	{
		return true;
	}
	return false;
}

void CTask::SetWriteFailReason(int pos)
{
	unsigned int bit = 0x00000001;
	bit <<= pos;
	m_iWriteFailReason |= bit;
}

void CTask::ResetWriteFailReason()
{
	m_iWriteFailReason = 0;
}

void CTask::SetFilesize( QVOD_INT64 iFilesize )
{
	if (m_seed)
	{
		m_seed->SetFileLength(iFilesize);
	}
	if (m_mem)
	{
		m_mem->Init(m_seed->GetDownFileSize(), m_seed->GetPieceSize());
	}
}

QVOD_INT64 CTask::GetDownloadedSize()
{
	if (m_mem)
		return m_mem->GetDownLoadedlen();
	return 0;
}

void CTask::ChangeTaskType( int type )
{
	if (type == m_Para.type)
	{
		return ;
	}
	switch(type)
	{
		case HTTPDOWNTASK:
		case HLSDOWNTASK:
		case HLSP2PDOWNTASK:
			{
				m_Para.type = type;
				CDbTaskConfig::Instance()->UpdateTask(m_Para);
				//CMsgPoolInterface::Instance()->UnShare((HASH&)m_Para.hash);
			}
			break;
		default:
			break;
	}
}

void CTask::UpdateHttpOrgUrl( string& strHttpOrgUrl )
{
	if (m_Para.strHttpOrgUrl != strHttpOrgUrl)
	{
#ifdef QVODDEBUG
		_OUTPUT(SERIOUS,"CTask::UpdateHttpOrgUrl %s to %s\n",m_Para.strHttpOrgUrl.c_str(),strHttpOrgUrl.c_str());
#endif
		m_Para.strHttpOrgUrl = strHttpOrgUrl;
		CDbTaskConfig::Instance()->UpdateTask(m_Para);
	}
}

QVOD_INT64 CTask::GetUnDownPosNearPlayPos()
{
	QVOD_INT64 pos = m_iPlayPos + m_mem->GetContinueLen(m_iPlayPos);
	if (pos >= m_seed->GetFileTotleLen())
	{
		pos = 0;
	}
	return pos;
}

int CTask::SetCopyFullFile( string& strCopyPath )
{
	// ��http��p2p����
	if (! (m_Para.type >=HTTPDOWNTASK && m_Para.type <= HLSP2PDOWNTASK))
	{
		return -1;
	}
	m_Para.bCopyFullFile = true;
	m_Para.strCopyPath = strCopyPath;
#ifndef MACOSX
	if (strCopyPath.empty())
	{
		m_Para.strCopyPath = m_strPath +m_strFileName;
	}
#endif
#ifdef QVODDEBUG
    _OUTPUT(INFO, "SetCopyFullFile path=[%s]", m_Para.strCopyPath.c_str());
#endif
	m_Para.copystatus = 0;
	CDbTaskConfig::Instance()->UpdateTask(m_Para);

	// ��������ɣ���ֱ�ӿ���������Ŀ¼��
	//if (m_Para.type >=HTTPDOWNTASK && m_Para.type <= HTTPP2PDOWNTASK)
	//{
	//	if (TaskIsFinish())
	//	{
	//		CCopyTaskFile::Instance()->AddOneTask((HASH&)m_Para.hash,strCopyPath);
	//	}
	//}
	return 0;
}

int CTask::UnsetCopyFullFile()
{
	// ��http��p2p����
	if (! (m_Para.type >=HTTPDOWNTASK && m_Para.type <= HLSP2PDOWNTASK))
	{
		return -1;
	}
	// ɾ�����������̵߳�����
	//CCopyTaskFile::Instance()->DeleteTask((HASH&)m_Para.hash);
	m_Para.bCopyFullFile = false;
	m_Para.strCopyPath = "";
	m_Para.copystatus = 0;
	CDbTaskConfig::Instance()->UpdateTask(m_Para);	
	return 0;
}

bool CTask::IsDownlloadTask()
{
	return	m_Para.bCopyFullFile;
}

void CTask::SetCopyFullFileStatus( int status )
{
	m_Para.copystatus = status;
	CDbTaskConfig::Instance()->UpdateTask(m_Para);	
}

QVOD_INT64 CTask::GetTotalDownloadLen()
{
	if (m_mem)
	{
		return m_mem->GetDownLoadedlen();
	}
	return 0;
}

QVOD_INT64 CTask::GetTotalUploadLen()
{
	return 0;
}
#ifdef MONGOTV
void CTask::SetUrlCdnType( bool bIsOtherCdn )
{
	m_bIsOtherCdn = bIsOtherCdn;
	m_bIsStartDoSended = false;
}
#endif

//void CTask::SetM3u8PlayingPos( string strTsName, int pos )
//{
//	static bool s_bOnce = false;
//	if (! s_bOnce)
//	{
//		s_bOnce = true;
//		CCacheDataInMem::Instance().ClearCache();		// �µ㲥������Ҫ����ڴ�
//	}
//    m_bIsPlaying = true;
//	m_m3u8->SetPlayingTs(strTsName);	
//#ifdef QVOD_USE_CACHEMEM
//	if (! m_m3u8->IsPlayTsContinueIn2Ts())		// ������tsƬ������˵����seek�ˣ���Ҫ����ڴ��е�����
//	{
//		CCacheDataInMem::Instance().ClearCacheByHash(m_Para.hash);
//	}
//#endif
//}
//
//bool CTask::IsPlayTsContinue()
//{
//	return m_m3u8->IsPlayTsContinue();
//}
//
//QVOD_INT64 CTask::GetM3u8TsSize( string strName )
//{
//	if (strName.find(".m3u8") != string::npos)
//	{
//		return m_m3u8->GetM3u8Size();
//	}
//	else
//	{
//		return m_m3u8->GetTsSize(strName);
//	}
//}
//
//bool CTask::IsExistM3u8Ts(string strTsUrl)
//{
//    bool bRet = (-1 != m_m3u8->GetTsUrlIndex(strTsUrl));
//    return bRet;
//}
