#pragma once

#ifndef __IBULLETSUBTITLE_HH__
#define __IBULLETSUBTITLE_HH__

typedef enum enumBulletScale
{
    BULLET_SCALE_DISABLED       = 0x0,      // Disabled scale bullet text when video window's size changed
    BULLET_SCALE_VIDEOWINDOW    = 0x1       // Auto scale bullet text to fit video window
}BulletScale;

typedef enum eBulletFontSize
{
    BULLET_SIZE_SMALL           = 19,       // Small font
    BULLET_SIZE_MIDDLE          = 22,       // middle font
    BULLET_SIZE_LARGE           = 25,       // large font
    BULLET_SIZE_BIGGEST         = 28,       // VIP Enable Bigest font
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

typedef struct tagBulletText
{
    tagBulletText()
    {
        memset(this,0,sizeof(tagBulletText));
        dwColor = 0xff0000ff;
    }

    LONGLONG        llID;           // The bullet ID
    TCHAR           szText[256];    // Text
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
}BulletText;
#endif

//
// IBarrageSubtitle

interface __declspec(uuid("B5B8FBDE-E1FB-4391-936D-426D4196E40B"))
IBarrageSubtitle:IUnknown
{
    //
    // put/get visable attribute
    virtual HRESULT put_Enable(BOOL bEnable) PURE;
    virtual HRESULT get_Enable(BOOL* bEnable) PURE;

    //
    // put/get scale attribute
    virtual HRESULT put_BulletScale(DWORD dwScaleStyle) PURE;
    virtual HRESULT get_BulletScale(DWORD* dwScaleStyle) PURE;

    //
    // put/get font attribute that will apply to all bullet-texts, the font-size item will be ignored
    virtual HRESULT put_BulletFont(LOGFONT* pFont) PURE;
    virtual HRESULT get_BulletFont(LOGFONT* pFont) PURE;

    //
    // set transparent 0-100(100% absolutely transparent)
    virtual HRESULT put_Transparent(BYTE) PURE;
    virtual HRESULT get_Transparent(BYTE& t) PURE;

    //
    // set density(0-100)
    virtual HRESULT put_Density(BYTE d) PURE;
    virtual HRESULT get_Density(BYTE& d) PURE;

    //
    // enable effects
    virtual HRESULT put_Effects_Enable(BOOL bEnable) PURE;
    virtual HRESULT get_Effects_Enable(BOOL* bEnable) PURE;


    //
    // add one BulletText item
    virtual HRESULT put_BulletText(const BulletText* bt) PURE;


    // remove all from videorenders
    virtual HRESULT remove_all() PURE;


    virtual HRESULT put_paint_stat(BOOL bEnable) PURE;
};

interface __declspec(uuid("42FF3E6B-4D73-442f-AF63-9DEEF2439BFF"))
IQmvTagRender : public IUnknown
{
    // Video Rotation
    virtual HRESULT InitVideoInsideTag( void * pIVideoInsideQmvPlus, void ** ppDevice ) PURE;
    virtual HRESULT GetRawVideoWH( DWORD * pW, DWORD * pH ) PURE;
    virtual HRESULT GetRenderWindow( HWND * pHwnd ) PURE;
};
