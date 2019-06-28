#pragma once

#ifndef QMV_INTERFACE
#define QMV_INTERFACE
#include <atlstr.h>

// 版本升级记录 
#define QMV_CURRENT_VERSION_1		(1)	// 原始版本 勿删 方便追述

//每次升级版本递增此值
#define QMV_CURRENT_VERSION_2		(2) // 无损支持

#define QMV_CURRENT_VERSION_3		(3) //扩展信息

#define QMV_CURRENT_VERSION_4		(4) //扩展信息 修改位置的组织方式

#define QMV_CURRENT_VERSION_5 		5	//增加模板图片
#define QMV_CURRENT_VERSION	QMV_CURRENT_VERSION_5

#define QMV_HASH_INFO_SIZE	40
#define QMV_FILM_NAME_SIZE	256

//
// Qmvb mux/demux definition

// picture type in qmvb previewframe box
#define     QMVB_PICT_TYPE_PNG      0x00000001
#define     QMVB_PICT_TYPE_JPG      0x00000002
#define     QMVB_PICT_TYPE_BMP      0x00000003
#define     QMVB_PICT_TYPE_GIF      0x00000004

// datatag type in qmvb datatag box
#define     QMVB_DATATAG_VIDEOTAG   0x00000001
#define     QMVB_DATATAG_ACTIONS    0x00000002

typedef struct QMVDemuxConfig_s
{
    QMVDemuxConfig_s()
    {
        memset(hash,0,QMV_HASH_INFO_SIZE);
        memset(filename,0,QMV_FILM_NAME_SIZE);
    }
    unsigned char hash[QMV_HASH_INFO_SIZE];
    wchar_t filename[QMV_FILM_NAME_SIZE/2];
} QMVDemuxConfig_t;

enum e_PIXMAP_TYPE
{
    E_PIX_NONE,
    E_PIX_PNG,
    E_PIX_GIF,
    E_PIX_BMP,
    E_PIX_JPG,
};

#ifndef REFERENCE_TIME
#define REFERENCE_TIME LONGLONG
#endif
struct POSITION_MAP
{
    REFERENCE_TIME	start;
    REFERENCE_TIME	end;
    POINT		xy;
};

typedef struct QMV_VideoTag
{
    CString			title;
    CString			description;
    CString			author;
    CString			url;

    REFERENCE_TIME	rtStart;
    REFERENCE_TIME	rtStop;
    DWORD			rating;//用户评价

    e_PIXMAP_TYPE	pixType;
    DWORD			pixmapBytes;
    BYTE*			pPixmapData;
    int				posNum;
    BYTE*			pointArray;

    DWORD			pixTempBytes;//模板图片大小
    BYTE*			pPixTemplData;//模板数据

    QMV_VideoTag(void) : rtStart( 0 ), rtStop( 0 ), rating( 0 ), pPixmapData( 0 ), pixType( E_PIX_NONE ),
        pixmapBytes( 0 ), pointArray( 0 ), posNum( 0 ), pPixTemplData( 0 ), pixTempBytes( 0 ){}
    QMV_VideoTag &QMV_VideoTag::operator =(const QMV_VideoTag & info)
    {
        pixType = info.pixType;
        title = info.title;
        description = info.description;
        url = info.url;
        author = info.author;
        pixmapBytes = info.pixmapBytes;
        if ( pPixmapData )
        {
            delete [] pPixmapData;
        }
        if ( pixmapBytes )
        {
            pPixmapData = new BYTE[pixmapBytes];
            memcpy( pPixmapData, info.pPixmapData, pixmapBytes );
        }

        //QMV VERSION 5 增加模板图片
        pixTempBytes = info.pixTempBytes;
        if ( pPixTemplData )
        {
            delete [] pPixTemplData;
        }
        if ( pixTempBytes )
        {
            pPixTemplData = new BYTE[pixTempBytes];
            memcpy( pPixTemplData, info.pPixTemplData, pixTempBytes);
        }
        posNum = info.posNum;
        if ( pointArray )
        {
            delete [] pointArray;
        }

        pointArray = new BYTE[posNum*sizeof(POSITION_MAP)];
        memcpy(pointArray, info.pointArray, posNum*sizeof(POSITION_MAP));
        rtStart = info.rtStart;
        rtStop = info.rtStop;
        return *this;
    }

    ~QMV_VideoTag(void)
    {
        if ( pPixmapData )
        {
            delete [] pPixmapData;
        }
        if ( pPixTemplData )
        {
            delete[] pPixTemplData;
        }
        if ( pointArray )
        {
            delete [] pointArray;
        }
    }
} QMV_AttachEx_t, QMV_QML_NODE;

typedef struct _TQMVTAGINFO
{
    explicit _TQMVTAGINFO(void) : tagID( 0 ), rewriteToDB( false )
    {
        hash[ 0 ] = 0;
    }
    _TQMVTAGINFO & _TQMVTAGINFO::operator =(const _TQMVTAGINFO & info)
    {
        base = info.base;
        tagID = info.tagID;
        film = info.film;
        lPath = info.lPath;
        nPath = info.nPath;
        memcpy( hash, info.hash, sizeof( hash ) );
        rewriteToDB = info.rewriteToDB;
        return *this;
    }
    QMV_VideoTag    base;
    WORD            tagID;
    CString         film;
    CString         lPath;
    CString         nPath;
    TCHAR           hash[ 41 ];
    bool            rewriteToDB;
} QMVTAGINFO;

#endif

interface __declspec(uuid("D2C53E82-6685-4C8F-BDCA-2E4408E1896E"))
IQMVDemuxConfig:IUnknown
{
    virtual HRESULT GetQMVDemuxConfig(struct QMVDemuxConfig_t *cfg) = 0;
    virtual HRESULT SetQMVDemuxConfig(struct QMVDemuxConfig_t *cfg) = 0;
};

interface __declspec(uuid("95D6A776-F2B8-4929-8B1A-38CEE8132639"))
IQMVideoTagQuery:IUnknown
{
    virtual HRESULT GetVideoTagCount(DWORD& count) = 0;
    virtual HRESULT GetVideoTagByIndex(int index, QMV_VideoTag* pInfo) = 0;
};

interface __declspec(uuid("C0B1F5FF-0E83-4116-BBC2-EB49DED48436"))
IQMVBPreview :public IUnknown 
{
    //
    // Get seek position for qmvb tags
    // nTagPosition [OUT] start position of dataTag(or is previewFrame if not exist dataTag)
    // nTagSize     [OUT] all blocks(dataTag, previewFrames) size
    // qmvb file struct: |--fileHeader--|--cueA--|--mediaData--|--dataTag--|--previewFrame--|--cueB--|
    virtual HRESULT GetTagPosition(unsigned __int64* nTagPosition, unsigned __int64* nTagSize) = 0;

    //
    // Get Preview Pictures Attributes
    // dwPictType:  [OUT] is picture type of QMVB_PICT_TYPE_PNG, QMVB_PICT_TYPE_JPG...., Set as NULL if needn't this value.
    // dwTimeSpan:  [OUT] interval in mills of every pictures , Set as NULL if needn't this value.
    // dwWidth:     [OUT] picture width, Set as NULL if needn't this value.
    // dwHeight:    [OUT] picture height, Set as NULL if needn't this value.
    virtual HRESULT GetPreviewFrame(DWORD* dwPictType, DWORD* dwTimeSpan, DWORD* dwWidth, DWORD* dwHeight) = 0;

    // Get Picture Counts of a range segments
    // dwCounts:    [OUT] Current exist counts 
    // dwStartTime: [IN] the range start time in millisecond,
    // dwEndtime:   [IN] the range end time, if set 0, the returned 'dwCounts' is all counts from dwStartTime to End
    virtual HRESULT GetPictureCount(DWORD* dwCounts, DWORD dwStartTime, DWORD dwEndTime) = 0;

    // Get One Picture, 
    // Before calling this, you should invoked GetPictureCount() to get dwCounts, otherwise, the default dwCounts is total counts exist
    // nIndex:          [IN] the index based dwCounts
    // dwTimeInMillis:  [OUT] the picture's time
    // pData:           [IN,OUT] the buffer, if needn't data, set as NULL
    // dwSizeInBytes:   [IN,OUT] the buffer size, after invoked this, the value is actually size of picture .
    virtual HRESULT GetPicture(int nIndex, DWORD* dwTimeInMillis, BYTE* pData, DWORD* dwSizeInBytes) = 0;

    //
    // Get DataTags

    // GetDataTagCount
    // dwTagType:   [IN] the data tag 's type, such as QMVB_DATATAG_VIDEOTAG
    // dwCount:     [OUT] the tag's item count
    virtual HRESULT GetDataTagCount(DWORD dwTagType, DWORD* dwCount) = 0;

    // Get ONE item DataTag
    // dwTagType    [IN] the data tag's type, such as QMVB_DATATAG_VIDEOTAG
    // nIndex:      [IN] the item index
    // pData:       [IN,OUT] the buffer
    // dwSize:      [IN,OUT] the buffer size, after invoked this, the value is actually size of this datatag item. .
    virtual HRESULT GetDataTag(DWORD dwTagType, int nIndex, BYTE* pData, DWORD* dwSize) = 0;

    //
    // Search subtitles

    // Set selected subtitle
    // nIndex:      [IN] the subtitle track's index, start from 0
    virtual HRESULT SetSelectedSubtitle(int nIndex) = 0;

    // Get Subtitle Track Count
    // nCount:      [OUT] the subtitle track's count
    // trackIDs:    [OUT] All subtitle track's IDs, contains 8 IDs max, each byte represent ONE trackID
    //              Such as 0x00000405 contains two IDs: 0x04 and 0x05.
    virtual HRESULT GetSubtitleCount(int* nCount, unsigned __int64* trackIDs) = 0;

    // Search Subtitle by keyword text
    // nTrackID:    [IN] specify the track ID to searched
    // lpszText:    [IN] keyword text
    // llTimeCode:  [OUT] is the keyword text's time in mills, if success, 
    virtual HRESULT SearchSubtitle(BYTE nTrackID, LPCWSTR lpszText, LONGLONG* llTimeCode) = 0;

    // Search Subtitle by time range
    // nTrackID:    [IN] specify the track ID to searched
    // dwStartTime: [IN] 
    // dwEndTime:   [IN] 
    // szText:      [OUT] text buffer for retrieves searched text.
    // dwTextSize   [IN] text buffer size
    virtual HRESULT SearchSubtitle(BYTE nTrackID, DWORD dwStartTime, DWORD dwEndTime, WCHAR* szText, DWORD dwTextSize) = 0;

    // Search Subtitle in current selected subtitle-track
    // dwStartTime: [IN] 
    // dwEndTime:   [IN] 
    // szText:      [OUT] text buffer for retrieves searched text.
    // dwTextSize   [IN] text buffer size
    virtual HRESULT SearchSubtitle(DWORD dwStartTime, DWORD dwEndTime, WCHAR* szText, DWORD dwTextSize) = 0;
};
