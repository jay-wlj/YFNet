/*! @file
********************************************************************************
<PRE>
Module       : 
File         : CustomPresenterCommon.h
Intro        : 
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

#ifndef __CUSTOMPRESENTERCOMMON_HH__
#define __CUSTOMPRESENTERCOMMON_HH__

#include "ISubPicAllocatorPresenter.h"


// {495CF191-810D-44c7-92C5-E7D46AE00F44}
DEFINE_GUID(CLSID_VMR7AllocatorPresenter,
            0x495cf191, 0x810d, 0x44c7, 0x92, 0xc5, 0xe7, 0xd4, 0x6a, 0xe0, 0xf, 0x44);

// {4E4834FA-22C2-40e2-9446-F77DD05D245E}
DEFINE_GUID(CLSID_VMR9AllocatorPresenter,
            0x4e4834fa, 0x22c2, 0x40e2, 0x94, 0x46, 0xf7, 0x7d, 0xd0, 0x5d, 0x24, 0x5e);


DEFINE_GUID(CLSID_EVRAllocatorPresenter,
            0x7612b889, 0xe070, 0x4bcc, 0xb8, 0x8, 0x91, 0xcb, 0x79, 0x41, 0x74, 0xab);


#ifdef VIDEORENDER_DLL_EXPORTS
    extern "C"
    VIDEORENDER_DLL_API HRESULT CreatePresenter(const CLSID& clsid, HWND hWnd, ISubPicAllocatorPresenter** ppAP);
#else
    HRESULT CreatePresenter(const CLSID& clsid, HWND hWnd, ISubPicAllocatorPresenter** ppAP);
#endif


#endif