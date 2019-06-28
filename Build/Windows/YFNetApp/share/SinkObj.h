// SinkObj.h: Definition of the CSinkObj class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINKOBJ_H__4B096477_0BB3_4680_A46B_233AD15C070B__INCLUDED_)
#define AFX_SINKOBJ_H__4B096477_0BB3_4680_A46B_233AD15C070B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "resource.h"       // main symbols

extern const IID DIID_IREALPLAYEvents;// = {0x6BF52A51,0x394A,0x11D3,{0xB1,0x53,0x00,0xC0,0x4F,0x79,0xFA,0xA6}};
//                                       ---B153-00C04F79FAA6  6BF52A51-394A-11D3-B153-00C04F79FAA6
extern const IID LIBID_REALPLAYLib;// = {0x6BF52A50,0x394A,0x11D3,{0xB1,0x53,0x00,0xC0,0x4F,0x79,0xFA,0xA6}};


#define IDC_MEDIAPLAY                      103
#define IDC_REALPLAY					   104 
#define IDC_QUICKPLAY					   105		
/////////////////////////////////////////////////////////////////////////////
// CSinkObj

class CSinkObj2 : 
	public IDispatchImpl<ISinkObj2, &IID_ISinkObj2, &LIBID_TVINSTERLib>, 
	public CComObjectRoot,
	public CComCoClass<CSinkObj2,&CLSID_SinkObj2>,
	public IDispEventImpl<IDC_REALPLAY, CSinkObj2, &DIID_IREALPLAYEvents, &LIBID_REALPLAYLib, 1, 0>
{
public:
	CSinkObj2() {}
	HWND m_hWnd;
BEGIN_COM_MAP(CSinkObj2)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISinkObj2)
END_COM_MAP()
BEGIN_SINK_MAP(CSinkObj2)
	//Make sure the Event Handlers have __stdcall calling convention
	SINK_ENTRY_EX(IDC_REALPLAY, DIID_IREALPLAYEvents, 1013, OnPlayStateChange)
END_SINK_MAP()
//DECLARE_NOT_AGGREGATABLE(CSinkObj2) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_REGISTRY_RESOURCEID(IDR_SinkObj2)

// ISinkObj2
public:
	void __stdcall OnPlayStateChange(long lNewState)
	{
		::SendMessage(m_hWnd, WM_REALPLAYSTATE, (WPARAM)lNewState, 0);
	}
	void Init(HWND hWnd) { m_hWnd = hWnd; }
};


#endif // !defined(AFX_SINKOBJ_H__4B096477_0BB3_4680_A46B_233AD15C070B__INCLUDED_)
