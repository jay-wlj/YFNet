// MsgHandle.h: interface for the CMsgHandle class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable: 4786)

#if !defined(AFX_MSGHANDLE_H__5B734B35_A51B_4562_B619_7E18A8CA0FC7__INCLUDED_)
#define AFX_MSGHANDLE_H__5B734B35_A51B_4562_B619_7E18A8CA0FC7__INCLUDED_

#include "include.h"
#include "common-SpeedCounter.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMsgHandle  
{
public:
	CMsgHandle();
    ~CMsgHandle();

	void DownNextPiece(KEY& key,int count = 2);
	int  DownNextPieceAction(KEY& key,int count = 2);
	int  DownNextHttpPiece(HASH& hash);
	

	void HdHttpAgentShareFile(char* packet);
	void HdHttpAgentReqLinkRsp(char* packet);
	void HdHttpAgentReqPiece(char* packet);
	void HdHttpAgentReqBlock(char* packet);
	void HdHttpAgentRemoveFile(char* packet);
	void HdHttpAgentReqBlockInfo(char* packet);
	void HdHttpAgentReqMeta(char* packet);
	void HdHttpAgentRange(char* packet);
	void HdHttpUpload(char* packet);

	int  DownNextM3u8Ts(HASH& hash);
	void HdHttpM3u8(char* packet);
	void HdHttpM3u8Ts(char* packet);

#ifdef SUPERNODE
	void HdWritePieceSucc(HASH& hash,KEY& key,int index);
	void HdReadPieceSucc( HASH& hash,KEY& key,char* buf, int& len, unsigned int& index, unsigned int& start);
#endif

private:
	//CLock m_downLock;
};

#endif // !defined(AFX_MSGHANDLE_H__5B734B35_A51B_4562_B619_7E18A8CA0FC7__INCLUDED_)
