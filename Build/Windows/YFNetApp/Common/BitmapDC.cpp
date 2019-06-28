/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*			¨q¡Ð¨r©©~ ¡è¡¡ ¨q¡Ð¨r ¨q¡Ð¨r 
*			¨q¡Ð¨q¡Ð¨r¨q¡Ð¨r¡«¨q¡Ð¨r¦á¦á,¡¡¦á¦á 
*			,¦á¦á¦á¦á,''¦á~~ ,''~¦á¦á¡¡ ,'' 
*			
*			
*				  ("`-''-/").___..--''"`-._
*				   `6_ 6  )   `-.  (     ).`-.__.`)
*				   (_Y_.)'  ._   )  `._ `. ``-..-'
*				 _..`--'_..-_/  /--'_.' ,'
*				(il),-'   (li),'  ((!.-'  
*
* 	SHINE Himself Is Not Enigma ... 
* 	RYU's Yokel Union
\ * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* File Name:	BitmapDC.cpp
* Date:			06/09/2007
* Author:		Shine Ryu
* Email:		ShineRyu@126.com
* Description:		
* 	
* -------------------------------------------------------------------------
* Copyright (c) 2007, Shine Ryu <ShineRyu@126.com>, P.R.China.
* All rights reserved.
*
* LICENSE TERMS
*
* The free distribution and use of this software in both source and binary 
* form is allowed (with or without changes) provided that:
*
*   1. distributions of this source code include the above copyright 
*      notice, this list of conditions and the following disclaimer;
*
*   2. distributions in binary form include the above copyright
*      notice, this list of conditions and the following disclaimer
*      in the documentation and/or other associated materials;
*
*   3. the copyright holder's name is not used to endorse products 
*      built using this software without specific written permission. 
*
* DISCLAIMER
*
* This software is provided 'as is' with no explcit or implied warranties
* in respect of any properties, including, but not limited to, correctness 
* and fitness for purpose.
* -------------------------------------------------------------------------
* Issue Date: 06/09/2007
** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2000-2005 Intel Corporation. All Rights Reserved.
//
*/

// IppiImageDC.cpp: implementation of the CBitmapDC class.
//
//////////////////////////////////////////////////////////////////////

#include "BitmapDC.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitmapDC::CBitmapDC()
{
    m_pBits = NULL;
    m_hBmp  = NULL;
	m_hDC	= NULL;
}

CBitmapDC::~CBitmapDC()
{
   Delete();
}

void CBitmapDC::Create(int nWidth, int nHeight)
{
	BITMAPINFOHEADER	bih;
	memset(&bih, 0, sizeof(BITMAPINFOHEADER));
	bih.biSize		=	sizeof(BITMAPINFOHEADER);
	bih.biWidth		=	nWidth;
	bih.biHeight	=	nHeight;
	bih.biPlanes	=	1;
	bih.biBitCount	=	32;
	bih.biCompression	=	BI_RGB;
	m_hBmp	=	NULL;
	m_hDC	=	CreateCompatibleDC(NULL);
	m_hBmp	= ::CreateDIBSection(
		GetSafeHdc(), (BITMAPINFO*)&bih,
		DIB_RGB_COLORS, (void**)(&m_pBits), NULL, 0);
	SelectObject(m_hDC, m_hBmp);
}

void CBitmapDC::Delete()
{
	if (m_hBmp)
		DeleteObject(m_hBmp);
	if (m_hDC)
		DeleteDC(m_hDC);
}
