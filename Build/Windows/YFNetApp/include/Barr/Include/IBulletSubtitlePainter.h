/*! @file
********************************************************************************
<PRE>
Module       : 
File         : IBulletSubtitle.h
Intro        : 
Version      : 1.0
--------------------------------------------------------------------------------
Notes        : 
--------------------------------------------------------------------------------
Change History :
Date         Version  Changed By      Changes 
2013/06/18   1.0      <Wang Yungang>  Create
2013/06/18   2.0      <Zhong weiyuan> VIP Bullet
</PRE>
********************************************************************************

* Copyright(c) Qvod Technology, by <Wang Yungang>, All rights reserved

*******************************************************************************/

#ifndef __IBULLETSUBTITLEPAINTER_HH__
#define __IBULLETSUBTITLEPAINTER_HH__

#include "../../include/CallbackDefine.h"

#ifndef __IBULLETSUBTITLE_HH__
typedef enum enumBulletScale
{
    BULLET_SCALE_DISABLED       = 0x0,      // Disabled scale bullet text when video window's size changed
    BULLET_SCALE_VIDEOWINDOW    = 0x1       // Auto scale bullet text to fit video window
}BulletScale;

typedef enum eBulletFontSize
{
    BULLET_SIZE_SMALL           = 15,       // Small font
    BULLET_SIZE_MIDDLE          = 18,       // middle font
    BULLET_SIZE_LARGE           = 21,       // large font
	BULLET_SIZE_BIGGEST         = 24,       // VIP Enable Bigest font
}BulletFontSize;

typedef enum eBulletAuthor
{
    BULLET_AUTHOR_OTHERUSER     = 0x0,      // Send by other user
    BULLET_AUTHOR_OWN           = 0x1,      // Send by own
}BulletAuthor;

typedef enum eBulletStyle					//Display model
{
    BULLET_STYLE_TOP            = 0x0,      // Fixed on top
    BULLET_STYLE_MIDDLE         = 0x01,     // Fixed on middle
    BULLET_STYLE_BOTTOM         = 0x02,     // Fixed on bottom
    BULLET_STYLE_STATIC         = 0x03,     // Fixed position
    BULLET_STYLE_ROLL           = 0x04,     // From right to left
    BULLET_STYLE_REVERSE_ROLL   = 0x05,     // From left to right
}BulletStyle;

typedef enum eBulletEffect					//Display effects
{
	BULLET_EFFECT_NONE           = 0x0,      // None
	BULLET_EFFECT_FLIP_SHADOW    = 0x01,     // Flip Shadow
	BULLET_EFFECT_GRADUAL        = 0x02,     // Color Changing
	BULLET_EFFECT_SIZE_ADJUST    = 0x03,     // Size Changing(small to big)
	BULLET_EFFECT_KARAOK         = 0x04,     // KARA OK
	BULLET_EFFECT_GLOW			= 0x05,     // Glow
}BulletEffect;

#define BULLET_SUBTITLE_SHOW_DEFAULT  0
#define BULLET_SUBTITLE_SHOW_ONCE     1

#define     REFERENCE_TIME_5S   (5*10000000)
#define     REFERENCE_TIME_8S   (8*10000000)
#define     REFERENCE_TIME_10S  (10*10000000)
#define     REFERENCE_TIME_12S  (12*10000000)

typedef struct tagBulletText
{
	tagBulletText()
	{
		memset(this,0,sizeof(tagBulletText));

		dwAuthor			= BULLET_AUTHOR_OTHERUSER;
		dwColor				= 0xffffffff;
		dwEffect			= BULLET_EFFECT_NONE;
		dwFontSize			= BULLET_SIZE_MIDDLE;
		dwOutlineClr		= 0xe81d1d1d;
		dwShadowClr			= 0xffffffff;
		dwStyle				= BULLET_STYLE_ROLL;
		dwOutlineThickness  = 3 ;
	}

	~tagBulletText()
	{
		if ( pText )
		{
			delete []pText;
			pText = NULL;
		}
	}

	LONGLONG        llID;           // The bullet ID
	TCHAR           *pText;			// Text	
	TCHAR           szUserName[20]; // Text
	DWORD           dwColor;        // Color of ARGB such as 0xAARRGGBB
	DWORD           dwFontSize;     // Font size, only support these values: (BULLET_SIZE_SMALL, BULLET_SIZE_MIDDLE, BULLET_SIZE_LARGE)
	LONGLONG        llStartTime;    // Start time
	LONGLONG        llEndTime;      // End time
	DWORD           dwAuthor;       // Author of this
	DWORD           dwStyle;        // Style of BulletStyle
	POINT           showPoint;      // Show Point, if dwStyle is BULLET_STYLE_STATIC
	RECT            showRect;       // Show Rect, 

	DWORD			dwOutlineClr;		//Outline Color
	DWORD			dwOutlineThickness;	//Outline Thickness

	DWORD			dwShadowClr;	//Font Shadow Color
	DWORD			dwEffect;		// Effect
	DWORD			dwShowStyle;	//show style
}BulletText;

#endif
//
// IBulletSubtitle

class IBulletSubtitlePainter
{
public:
	STDMETHOD (InitBulletSubtitlePainter)(HWND hwnd, IQvodCallBack *pEventCallback) PURE;
	STDMETHOD (DeinitBulletSubtitlePainter)() PURE;

	STDMETHOD (PresentBulletSubtitle)(LONGLONG llTimeNow, CSize SrcSize, HDC pHDCMem) PURE;
	STDMETHOD (PresentBulletSubtitle)(LONGLONG llTimeNow, void* pHDCMem, CSize SrcSize,  CSize DstSize) PURE;

	//STDMETHOD (InitBulletSubtitlePainterEx)(void *pDev, CSize size) PURE;
	//STDMETHOD (PresentBulletSubtitleEx)(LONGLONG llTimeNow, void *pSurface) PURE;

    // put/get visable attribute
    STDMETHOD(put_Enable)(BOOL bEnable) PURE;
    STDMETHOD(get_Enable)(BOOL* bEnable) PURE;

    //
    // put/get scale attribute
    STDMETHOD(put_BulletScale)(DWORD dwScaleStyle) PURE;
    STDMETHOD(get_BulletScale)(DWORD* dwScaleStyle) PURE;

    //
    // put/get font attribute that will apply to all bullet-texts, the font-size item will be ignored
    STDMETHOD(put_BulletFont)(LOGFONT* pFont) PURE;
    STDMETHOD(get_BulletFont)(LOGFONT* pFont) PURE;

	//
	// set transparent 0-100(100% absolutely transparent)
	STDMETHOD(put_Transparent)(BYTE) PURE;
	STDMETHOD(get_Transparent)(BYTE& t) PURE;

	//
	// set density(0-100)
	STDMETHOD(put_Density)(BYTE d) PURE;
	STDMETHOD(get_Density)(BYTE& d) PURE;

	//
	// enable effects 
	STDMETHOD(put_Effects_Enable)(BOOL bEnable) PURE;
	STDMETHOD(get_Effects_Enable)(BOOL* bEnable) PURE;
    //
    // add one BulletText item
    STDMETHOD(put_BulletText)(const BulletText* bt) PURE;
	STDMETHOD(put_BulletTextList)(CAtlList<BulletText*> & listData) PURE;

	//set barrages show time which efect the seed
	STDMETHOD (put_BarrShowTime)(int nShowTime) PURE;

    // remove all from videorenders
    STDMETHOD(remove_all)() PURE;

	STDMETHOD(put_paint_stat)(BOOL bEnable) PURE;
	
	STDMETHOD(Flush)() PURE;
};

#endif