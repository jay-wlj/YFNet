// * ##################################################################################################
// *                                          QMV type
// * ##################################################################################################
// * $Id: QMVType.h,v 1.7 2009/11/02 10:19:57 Administrator Exp $
// * ##################################################################################################

#ifndef _QMV_TYPE_H_
#define _QMV_TYPE_H_

#ifdef WIN32
	#define LITTLE_ENDIAN (1)
	typedef unsigned int uint32_t;
	typedef unsigned short uint16_t;
	typedef unsigned __int64 uint64_t;
	typedef unsigned char uint8_t;
	typedef int int32_t;
#else
	#pragma message("SYSTEM UNKNOWN")
#endif

#if LITTLE_ENDIAN
	#define QMV_MAKE_SIGNATURE(__s) ((((uint32_t)(((uint8_t*)(__s))[3]))<<24) | (((uint32_t)(((uint8_t*)(__s))[2]))<<16) | (((uint32_t)(((uint8_t*)(__s))[1]))<<8) | (((uint32_t)(((uint8_t*)(__s))[0]))<<0))
#else
	#pragma message("BIG_ENDIAN NOT SUPPORTED")
#endif

#define QMV_SIG_FILE QMV_MAKE_SIGNATURE("QMV ")
#define QMV_SIG_DATA QMV_MAKE_SIGNATURE("DAT ")
#define QMV_SIG_STRM QMV_MAKE_SIGNATURE("STRM")
#define QMV_SIG_INDX QMV_MAKE_SIGNATURE("INDX")

#define QMV_CURRENT_VERSION     (1)

enum
{
	QMV_FLAGS_NONE = 0x00000000,
	QMV_FLAGS_KEY = 0x00000001,
	QMV_FLAGS_DISCONTINUITY = 0x00000002,
	QMV_FLAGS_INDEX = 0x00000004,
	QMV_FLAGS_MASK = 0x00000007
};

enum
{
	QMV_CAT_NONE = 0,
	QMV_CAT_VIDEO,
	QMV_CAT_AUDIO,
	QMV_CAT_MAX
};

#pragma pack (push)
#pragma pack (1)
typedef struct
{
	uint32_t num;
	uint32_t den;
} QMVTimeScale_t;

typedef struct
{
	uint64_t nTimeStamp;
	uint16_t nStreamId;
	uint16_t nFlags;
	uint64_t nOffset;
	uint64_t nSampleIndex;
} QMVIndexRecord_t;


typedef struct
{
	uint32_t nSignature;
	uint64_t nLength;
	uint8_t cVersion;
	uint8_t reserved1[3];
} QMVChunk_t;

typedef struct
{
	QMVChunk_t chunk;
	uint32_t nBitRate;
	QMVTimeScale_t oTimeScale;
	uint64_t nTimeBase;
	uint64_t nDuration;
	uint32_t nStreamCount;
	uint64_t nDataOffset;
	uint16_t nMasterIndexRecordCount;
	QMVIndexRecord_t aMasterIndex[1];
} QMVFileHeader_t;

typedef struct
{
	QMVChunk_t chunk;
	uint64_t nSampleCount;
} QMVData_t;

typedef struct
{
	uint32_t nSize;
	uint16_t nStreamId;
	uint16_t nFlags;
	uint64_t nTimeStamp;
} QMVSample_t;


typedef struct
{
	QMVChunk_t chunk;
	uint16_t nStreamId;
	uint16_t nStreamCat;
	uint32_t nBitRate;
	QMVTimeScale_t oTimeScale;
	uint64_t nDuration;
	uint32_t nStreamType;
} QMVStreamInfo_t;

typedef struct
{
	QMVStreamInfo_t info;
	uint32_t width;
	uint32_t height;
	uint32_t arx;
	uint32_t ary;
	uint32_t bit_count;
	uint32_t fcc;
	uint32_t size_image;
	uint32_t frame_duration;
	uint32_t codec_data_len;
	uint8_t codec_data[1];
} QMVVideoStreamInfo_t;

typedef struct
{
	QMVStreamInfo_t info;
	uint32_t sample_rate;
	uint16_t channels;
	uint16_t sample_size;
	uint32_t frame_duration;
	uint32_t codec_data_len;
	uint8_t codec_data[1];
} QMVAudioStreamInfo_t;

typedef struct
{
	QMVChunk_t chunk;
	uint64_t nIndexCount;
	QMVIndexRecord_t aIndexRecords[1];
} QMVIndexChunk_t;

#pragma pack (pop)
#endif
// * end of file //
