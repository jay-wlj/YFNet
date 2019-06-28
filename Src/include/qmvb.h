/*! @file
********************************************************************************
<PRE>
Module       : 
File         : qmvb.h
Intro        : 
Version      : 1.0
--------------------------------------------------------------------------------
Notes        : 
--------------------------------------------------------------------------------
Change History :
Date         Version  Changed By      Changes 
2013/07/16   1.0      <Wang Yungang>  Create
</PRE>
********************************************************************************

* Copyright(c) Qvod Technology, by <Wang Yungang>, All rights reserved

*******************************************************************************/

#ifndef __QMVB_HH__
#define __QMVB_HH__


#define     ENABLE_MKV_FLAG         0
#define     ENABLE_WRITE_TESTDATA   0

//
// QMVB EBML Definition
#if ENABLE_MKV_FLAG
#define     EBML_ID_QMVB        0x1A45DFA3
#define     DOCTYPE_QMVB        "matroska"
#else
#define     EBML_ID_QMVB        0x1B514D56 //'QMVB'
#define     DOCTYPE_QMVB        "qmvb"
#endif

#define     QMVB_MUXING_APP     TEXT("qvod qmvb muxer (dshow)")
#define     QMVB_CUEA_DURATION  ((__int64)10*60*10000000)  // 10 minutes in 100-nanosecond //millisecond
#define     QMVB_PREVIEWA_SPAN  (3*60*1000)                // 3 minutes in millisecond

#define     ID_VIDEOBPS         0x63D0
#define     ID_AUDIOBPS         0x63D1
#define     ID_DATATAGSIZE      0x63D2
#define     ID_PREVIEWASIZE     0x63D3
#define     ID_PREVIEWSIZE      0x63D4
#define     ID_PREVIEWFRAME     0x1941A470
#define     ID_FRAMEPICTURE     0xC0   

#define     ID_PICT_TYPE        0x5001
#define     ID_PICT_WIDTH       0x5002
#define     ID_PICT_HEIGHT      0x5003
#define     ID_PICT_TIME        0x5004
#define     ID_PICT_DATA        0x5005
#define     ID_PICT_SPAN        0x5006

#define     ID_DATATAG          0x1941A471
#define     ID_TAGITEM          0xC1
#define     ID_TAGITEM_ID       0x5010
#define     ID_TAGITEM_DATA     0x5011


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

// mux status
#define     QMVB_MUX_STATUS_IDLE        0x00000001
#define     QMVB_MUX_STATUS_PROCESSING  0x00000002
#define     QMVB_MUX_STATUS_ERROR       0x00000003
#define     QMVB_MUX_STATUS_COMPLETE    0x00000004


//
// IQMVBMuxer
interface __declspec(uuid("B479BB50-5CDA-4556-9828-97B596D7D2AE"))
IQMVBMuxer :public IUnknown 
{
    //
    // Get mux status
    STDMETHOD (GetMuxStatus)() = 0;

    //
    // Put preview pictures
    STDMETHOD (PutPreviewFrame) (DWORD dwPictType, DWORD dwTimeSpan, DWORD dwWidth, DWORD dwHeight) = 0;
    STDMETHOD (PutPicture) (DWORD dwTimeInMillis, BYTE* pData, DWORD dwDataSize) = 0;

    //
    // Put data tags, the data format is defined by APP
    STDMETHOD (PutDataTag) (DWORD dwTagType, BYTE* pData, DWORD dwDataSize) = 0;

    //
    // Put outer subtitle file, Make sure invoke this after InputPins has been connected and before running
    STDMETHOD (PutSubtitleFile) (LPCWSTR lpszFile, DWORD* dwTrackID) = 0;
};


//
// IQMVBMuxer
interface __declspec(uuid("C0B1F5FF-0E83-4116-BBC2-EB49DED48436"))
IQMVBDemuxer :public IUnknown 
{
    //
    // Get seek position for qmvb tags
    // nTagPosition [OUT] start position of dataTag(or is previewFrame if not exist dataTag)
    // nTagSize     [OUT] all blocks(dataTag, previewFrames) size
    // qmvb file struct: |--fileHeader--|--cueA--|--mediaData--|--dataTag--|--previewFrame--|--cueB--|
    STDMETHOD (GetTagPosition)(unsigned __int64* nTagPosition, unsigned __int64* nTagSize) = 0;

    //
    // Get Preview Pictures Attributes
    // dwPictType:  [OUT] is picture type of QMVB_PICT_TYPE_PNG, QMVB_PICT_TYPE_JPG...., Set as NULL if needn't this value.
    // dwTimeSpan:  [OUT] interval in mills of every pictures , Set as NULL if needn't this value.
    // dwWidth:     [OUT] picture width, Set as NULL if needn't this value.
    // dwHeight:    [OUT] picture height, Set as NULL if needn't this value.
    STDMETHOD (GetPreviewFrame) (DWORD* dwPictType, DWORD* dwTimeSpan, DWORD* dwWidth, DWORD* dwHeight) = 0;


    // Get Picture Counts of a range segments
    // dwCounts:    [OUT] Current exist counts 
    // dwStartTime: [IN] the range start time in millisecond,
    // dwEndtime:   [IN] the range end time, if set 0, the returned 'dwCounts' is all counts from dwStartTime to End
    STDMETHOD (GetPictureCount) (DWORD* dwCounts, DWORD dwStartTime, DWORD dwEndTime) = 0;


    // Get One Picture, 
    // Before calling this, you should invoked GetPictureCount() to get dwCounts, otherwise, the default dwCounts is total counts exist
    // nIndex:          [IN] the index based dwCounts
    // dwTimeInMillis:  [OUT] the picture's time
    // pData:           [IN,OUT] the buffer, if needn't data, set as NULL
    // dwSizeInBytes:   [IN,OUT] the buffer size, after invoked this, the value is actually size of picture .
    STDMETHOD (GetPicture) (int nIndex, DWORD* dwTimeInMillis, BYTE* pData, DWORD* dwSizeInBytes) = 0;



    //
    // Get DataTags

    // GetDataTagCount
    // dwTagType:   [IN] the data tag 's type, such as QMVB_DATATAG_VIDEOTAG
    // dwCount:     [OUT] the tag's item count
    STDMETHOD (GetDataTagCount) (DWORD dwTagType, DWORD* dwCount) = 0;

    // Get ONE item DataTag
    // dwTagType    [IN] the data tag's type, such as QMVB_DATATAG_VIDEOTAG
    // nIndex:      [IN] the item index
    // pData:       [IN,OUT] the buffer
    // dwSize:      [IN,OUT] the buffer size, after invoked this, the value is actually size of this datatag item. .
    STDMETHOD (GetDataTag) (DWORD dwTagType, int nIndex, BYTE* pData, DWORD* dwSize) = 0;




    //
    // Search subtitles

    // Set selected subtitle
    // nIndex:      [IN] the subtitle track's index, start from 0
    STDMETHOD (SetSelectedSubtitle)(int nIndex) = 0;


    // Get Subtitle Track Count
    // nCount:      [OUT] the subtitle track's count
    // trackIDs:    [OUT] All subtitle track's IDs, contains 8 IDs max, each byte represent ONE trackID
    //              Such as 0x00000405 contains two IDs: 0x04 and 0x05.
    STDMETHOD (GetSubtitleCount)(int* nCount, unsigned __int64* trackIDs) = 0;

    // Search Subtitle by keyword text
    // nTrackID:    [IN] specify the track ID to searched
    // lpszText:    [IN] keyword text
    // llTimeCode:  [OUT] is the keyword text's time in mills, if success, 
    STDMETHOD (SearchSubtitle)(BYTE nTrackID, LPCWSTR lpszText, LONGLONG* llTimeCode) = 0;

    // Search Subtitle by time range
    // nTrackID:    [IN] specify the track ID to searched
    // dwStartTime: [IN] 
    // dwEndTime:   [IN] 
    // szText:      [OUT] text buffer for retrieves searched text.
    // dwTextSize   [IN] text buffer size
    STDMETHOD (SearchSubtitle)(BYTE nTrackID, DWORD dwStartTime, DWORD dwEndTime, WCHAR* szText, DWORD dwTextSize) = 0;

    // Search Subtitle in current selected subtitle-track
    // dwStartTime: [IN] 
    // dwEndTime:   [IN] 
    // szText:      [OUT] text buffer for retrieves searched text.
    // dwTextSize   [IN] text buffer size
    STDMETHOD (SearchSubtitle)(DWORD dwStartTime, DWORD dwEndTime, WCHAR* szText, DWORD dwTextSize) = 0;

};

#endif