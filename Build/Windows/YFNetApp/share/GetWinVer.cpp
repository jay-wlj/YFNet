// GetWinVer.cpp  Version 1.1
//
// Copyright (C) 2001-2003 Hans Dietrich
//
// This software is released into the public domain.  
// You are free to use it in any way you like, except
// that you may not sell this source code.
//
// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 
//
///////////////////////////////////////////////////////////////////////////////

//#ifdef _DEBUG_LOG_CRASH_

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <tchar.h>

#include "GetWinVer.h"

// from winbase.h
#ifndef VER_PLATFORM_WIN32s
#define VER_PLATFORM_WIN32s             0
#endif
#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS      1
#endif
#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT           2
#endif
#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE           3
#endif


/*
    This table has been assembled from Usenet postings, personal
    observations, and reading other people's code.  Please feel
    free to add to it or correct it.


         dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
95             1              4               0             950
95 SP1         1              4               0        >950 && <=1080
95 OSR2        1              4             <10           >1080
98             1              4              10            1998
98 SP1         1              4              10       >1998 && <2183
98 SE          1              4              10          >=2183
ME             1              4              90            3000

NT 3.51        2              3              51
NT 4           2              4               0            1381
2000           2              5               0            2195
XP             2              5               1            2600
2003 Server    2              5               2            3790

CE             3

*/

///////////////////////////////////////////////////////////////////////////////
// GetWinVer
namespace QVODRE
{
	namespace WINVER_RE
	{
BOOL GetWinVer(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild, int &nSPack)
{
	if (!pszVersion || !nVersion || !pszMajorMinorBuild)
		return FALSE;
	lstrcpy(pszVersion, WUNKNOWNSTR);
	*nVersion = WUNKNOWN;

	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo))
		return FALSE;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this

	wsprintf(pszMajorMinorBuild, _T("%u.%u.%u"), dwMajorVersion, dwMinorVersion, dwBuildNumber);

	if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
	{
		if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
		{
			lstrcpy(pszVersion, W95STR);
			*nVersion = W95;
		}
		else if ((dwMinorVersion < 10) && 
				((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
		{
			lstrcpy(pszVersion, W95SP1STR);
			*nVersion = W95SP1;
		}
		else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
		{
			lstrcpy(pszVersion, W95OSR2STR);
			*nVersion = W95OSR2;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
		{
			lstrcpy(pszVersion, W98STR);
			*nVersion = W98;
		}
		else if ((dwMinorVersion == 10) && 
				((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
		{
			lstrcpy(pszVersion, W98SP1STR);
			*nVersion = W98SP1;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
		{
			lstrcpy(pszVersion, W98SESTR);
			*nVersion = W98SE;
		}
		else if (dwMinorVersion == 90)
		{
			lstrcpy(pszVersion, WMESTR);
			*nVersion = WME;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
		{
			lstrcpy(pszVersion, WNT351STR);
			*nVersion = WNT351;
		}
		else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
		{
			lstrcpy(pszVersion, WNT4STR);
			*nVersion = WNT4;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
		{
			lstrcpy(pszVersion, W2KSTR);
			*nVersion = W2K;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
		{
			lstrcpy(pszVersion, WXPSTR);
			*nVersion = WXP;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
		{
			lstrcpy(pszVersion, W2003SERVERSTR);
			*nVersion = W2003SERVER;
		}
		else if ((dwMajorVersion == 6) && (dwMinorVersion == 0))
		{
			lstrcpy(pszVersion, WVISTA);
			*nVersion = WVI;
		}
		else// if ((dwMajorVersion == 6) && (dwMinorVersion == 1))//win 7
		{
			lstrcpy(pszVersion, WINDOWS7);
			*nVersion = W7;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
	{
		lstrcpy(pszVersion, WCESTR);
		*nVersion = WCE;
	}
	if(_tcscmp(_T("Service Pack 3"), osinfo.szCSDVersion) == 0)
	{
		nSPack = SP3;
	}
	else if(_tcscmp(_T("Service Pack 2"), osinfo.szCSDVersion) == 0)
	{
		nSPack = SP2;
	}
	else if(_tcscmp(_T("Service Pack 1"), osinfo.szCSDVersion) == 0)
	{
		nSPack = SP1;
	}
	else
		nSPack = SP0;
	return TRUE;
}





HRESULT DisableWindowsXPFirewall( INetSharingManager* pNSM, BOOL bTestopen/* = FALSE*/)
{
	HRESULT hr = S_FALSE;
	
	INetSharingEveryConnectionCollection* pNSECC = NULL;
	hr = pNSM->get_EnumEveryConnection( &pNSECC );
	
	if( pNSECC )
	{
		IEnumVARIANT * pEV = NULL;
		IUnknown * pUnk = NULL;
		hr = pNSECC->get__NewEnum( &pUnk );
		if(pUnk) 
		{
			HRESULT hr2 = pUnk->QueryInterface (__uuidof(IEnumVARIANT), (void**)&pEV);
			pUnk->Release();
		}
		
		if (pEV) 
		{
			VARIANT v;
			VariantInit(&v);
			HRESULT hrEV = pEV->Next (1, &v, NULL);
			
			while( S_OK == hrEV ) 
			{
				if (V_VT (&v) == VT_UNKNOWN) 
				{
					INetConnection* pNC = NULL;
					V_UNKNOWN (&v)->QueryInterface (__uuidof(INetConnection), (void**)&pNC);
					if(pNC) 
					{
						INetSharingConfiguration* pFirewallConfig = NULL;
						HRESULT hr3 = pNSM->get_INetSharingConfigurationForINetConnection( pNC, &pFirewallConfig );
						if( pFirewallConfig )
						{
							VARIANT_BOOL bEnabled;
							HRESULT hr4 = pFirewallConfig->get_InternetFirewallEnabled( &bEnabled );
							if( S_OK == hr4 ) 
							{
								if(!bTestopen)
								{
									if(VARIANT_TRUE == bEnabled)
										hr = pFirewallConfig->DisableInternetFirewall();
								}
								else
								{
									hr = (VARIANT_TRUE == bEnabled) ? S_OK:S_FALSE;
								}
								pFirewallConfig->Release();
							}
						}
						
						pNC->Release();
					}
				}
				VariantClear (&v);
				hrEV = pEV->Next (1, &v, NULL);
			}
			pEV->Release();
		}
		pNSECC->Release();
	}
	return hr;
}

HRESULT DisableWindowsXPSP1Firewall()
{
	HRESULT hr = S_FALSE;
	
	hr = ::CoInitialize (NULL);
	
	hr = CoInitializeSecurity (NULL, -1, NULL, NULL, 
			RPC_C_AUTHN_LEVEL_PKT, 
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL, EOAC_NONE, NULL);
	
	
	INetSharingManager* pNSM = NULL;
	hr = ::CoCreateInstance (__uuidof(NetSharingManager),
		NULL,
		CLSCTX_ALL,
		__uuidof(INetSharingManager),
		(void**)&pNSM);
	if( !pNSM )
	{
		return hr;
	}
	hr = DisableWindowsXPFirewall( pNSM );
	
	pNSM->Release();

	CoUninitialize();
	return hr;
}
//判断XPSP1的防火墙是否已经打开S_OK;打开 S_FALSE 关闭的
HRESULT WindowsXPSP1FirewallIsOpen()
{
	HRESULT hr = S_FALSE;
	hr = ::CoInitialize(NULL);
	hr = CoInitializeSecurity (NULL, -1, NULL, NULL, 
			RPC_C_AUTHN_LEVEL_PKT, 
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL, EOAC_NONE, NULL);
	INetSharingManager* pNSM = NULL;
	hr = ::CoCreateInstance (__uuidof(NetSharingManager),
		NULL,
		CLSCTX_ALL,
		__uuidof(INetSharingManager),
		(void**)&pNSM);
	if( !pNSM )
	{
		CoUninitialize();
		hr = E_NOINTERFACE ;
		return hr;
	}
	hr = DisableWindowsXPFirewall( pNSM, TRUE);
	pNSM->Release();
	CoUninitialize();
	return hr;
}
	};
};
//#endif // _DEBUG_LOG_CRASH_