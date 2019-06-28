/*! @file
********************************************************************************
<PRE>
Module       : 
File         : ISubPicAllocatorPresenter.h
Intro        : The interface of ISubPicAllocatorPresenter
Version      : 1.0
--------------------------------------------------------------------------------
Notes        : 
--------------------------------------------------------------------------------
Change History :
Date         Version  Changed By      Changes 
2012/01/13   1.0      <Wang Yungang>  Create
</PRE>
********************************************************************************

* Copyright(c) Qvod Technology, by <Wang Yungang>, All rights reserved

*******************************************************************************/


#ifndef __ISUBPICALLOCATORPRESENTER_HH__
#define __ISUBPICALLOCATORPRESENTER_HH__
//#include <strmif.h>


//
// ISubPicAllocatorPresenter
//
interface __declspec(uuid("CF75B1F0-535C-4074-8869-B15F177F944E"))
ISubPicAllocatorPresenter :public IUnknown 
{
	STDMETHOD (CreateRenderer) (IUnknown** ppRenderer) PURE;
	STDMETHOD_(bool, ResetDevice) () PURE;

    // Generic
    STDMETHOD (GetDIB) (BYTE* lpDib, DWORD* size) PURE;
    STDMETHOD_(SIZE, GetVideoSize) (bool fCorrectAR = true) PURE;
    STDMETHOD_(SIZE, GetVisibleVideoSize) () PURE;
    STDMETHOD_(void, SetPosition) (RECT w, RECT v) PURE;
    STDMETHOD_(bool, Paint) (bool fAll, bool fTimeSub = true) PURE;
    STDMETHOD_(void, SetTime) (REFERENCE_TIME rtNow) PURE;
    STDMETHOD_(double, GetFPS) () PURE;
    STDMETHOD_(void, Invalidate) (REFERENCE_TIME rtInvalidate = -1) PURE;
    STDMETHOD_(bool, IsFirstFrameRendered) () PURE;
    STDMETHOD (SetSeekTag)(bool bAbsSeek) PURE;

    // Video Rotation
    //STDMETHOD (SetVideoAngle) (int x, int y, int z, bool fRepaint = true) PURE;
    //STDMETHOD (GetVideoAngle) (int* x, int* y, int* z) PURE;

    // Subtitle, moved into interface ISubtitleControl.h
    //STDMETHOD (AddSubtitleFile)(LPCTSTR lpszPathName) PURE;
    //STDMETHOD (CreateSubtitleFilter)(IUnknown** ppSubtitleFilter) PURE;
    //STDMETHOD (GetSubtitleCount)(int* nSubtitleCount) PURE;
    //STDMETHOD (GetSubtitle)(int nSubtitleIndex, SUBTITLE_INFO* subtitleInfo) PURE;
    //STDMETHOD (EnableSubtitle)(int nSubtitleIndex, bool bEnable) PURE;

    // ColorControl
    //STDMETHOD (SetColorControl)(COLORCONTROL* pColorControl) PURE;
	//STDMETHOD (SetVideoEnhance)(bool fEnable) PURE;

    // 3D View
    //STDMETHOD (Set3DView)(OPTION3DVIEW opt3DView, float kView) PURE;
};


#endif