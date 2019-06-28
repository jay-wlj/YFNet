#pragma once

#define DISPID_ONSTARTADD    1
#define DISPID_ONSTOPADD     2

#define SOURCEID             1 

class CRealSink;
#ifndef QVOD_M_BASE
#define QVOD_M_BASE								WM_USER+0x110
#define QVOD_M_REALPLAYSTATE					QVOD_M_BASE+20
#endif


extern const IID DIID_IREALPLAYEvents;// = {0x6BF52A51,0x394A,0x11D3,{0xB1,0x53,0x00,0xC0,0x4F,0x79,0xFA,0xA6}};
//                                       ---B153-00C04F79FAA6  6BF52A51-394A-11D3-B153-00C04F79FAA6
extern const IID LIBID_REALPLAYLib;// = {0x6BF52A50,0x394A,0x11D3,{0xB1,0x53,0x00,0xC0,0x4F,0x79,0xFA,0xA6}};

typedef IDispEventImpl<SOURCEID,CRealSink,&DIID_IREALPLAYEvents,&LIBID_REALPLAYLib,1,0> CRealEventSink;

//typedef IDispatchImpl<_ITestOBJEvents, &__uuidof(_ITestOBJEvents), &LIBID_TestCOMLib, /* wMajor = */ 1, /* wMinor = */ 0> CEventSink;

class ATL_NO_VTABLE CRealSink :
    public CComObjectRoot,
    public CRealEventSink
{
public:
    CRealSink(void);
    ~CRealSink(void);
	HWND m_hWnd;
    BEGIN_COM_MAP(CRealSink)
    END_COM_MAP()  

    BEGIN_SINK_MAP(CRealSink)
        SINK_ENTRY_EX(SOURCEID, DIID_IREALPLAYEvents, 1013, OnPlayStateChange)
    END_SINK_MAP()
public:
	void __stdcall OnPlayStateChange(long lNewState)
	{
		::SendMessage(m_hWnd, QVOD_M_REALPLAYSTATE, (WPARAM)lNewState, 0);
	}
	void Init(HWND hWnd) { m_hWnd = hWnd; }
};