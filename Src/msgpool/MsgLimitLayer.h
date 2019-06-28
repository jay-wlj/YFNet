// MsgLimitLayer.h: interface for the CMsgLimitLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGLIMITLAYER_H__4D7B2A28_B239_4A5A_924C_3AB00A1E9077__INCLUDED_)
#define AFX_MSGLIMITLAYER_H__4D7B2A28_B239_4A5A_924C_3AB00A1E9077__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMsgLimitLayer 
{
public:
	CMsgLimitLayer();
	virtual ~CMsgLimitLayer();

	virtual int  GetMsgFromUpQueue(HASHDATA& hashdata);
	virtual int  GetMsgFromDownQueue(HASHDATA& hashdata);
	virtual bool AddMsgToUpQueue(HASH& hash,DATA& data,PROTOCOL protocol,bool optimize,bool priority = false);
	virtual bool AddMsgToDownQueue(HASH& hash,DATA& data,PROTOCOL protocol,bool priority = false);
	bool ClearAllQueue();
	static CMsgLimitLayer* Instance();
	static void FreeInstance();

	static CMsgLimitLayer* m_instance;
private:
	CLock m_upLock, m_downLock;
	list<HASHDATA> m_downList,m_upList;
};

#endif // !defined(AFX_MSGLIMITLAYER_H__4D7B2A28_B239_4A5A_924C_3AB00A1E9077__INCLUDED_)
