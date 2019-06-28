
/*
 * QVOD base types.
 */

#ifndef _QVOD_BASETYPES_H_
#define _QVOD_BASETYPES_H_


#include <iostream>


#ifdef WIN32 /* WIN32 */

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>

#define QVOD_WINAPI			__cdecl
#define QVOD_INFINITE		INFINITE
#define QVOD_MAX_PATH		MAX_PATH
#define QVOD_TRUE			TRUE
#define QVOD_FALSE			FALSE
#define QVOD_IN				IN
#define QVOD_OUT			OUT

typedef BOOL				QVOD_BOOL;
typedef BYTE				QVOD_BYTE;
typedef UCHAR				QVOD_UCHAR;
typedef INT8				QVOD_INT8;
typedef UINT8				QVOD_UINT8;
typedef INT16				QVOD_INT16;
typedef UINT16				QVOD_UINT16;
typedef WORD				QVOD_WORD; /* 16-bit */
typedef USHORT				QVOD_USHORT;
typedef DWORD				QVOD_DWORD; /* 32-bit */
typedef INT					QVOD_INT;
typedef UINT				QVOD_UINT;
typedef INT32				QVOD_INT32;
typedef UINT32				QVOD_UINT32;
typedef LONG				QVOD_LONG;
typedef ULONG				QVOD_ULONG;
typedef INT64				QVOD_INT64;
typedef UINT64				QVOD_UINT64;
typedef LONGLONG			QVOD_LONGLONG;
typedef ULONGLONG			QVOD_ULONGLONG;

typedef LARGE_INTEGER		QVOD_LARGE_INTEGER;
typedef PLARGE_INTEGER		QVOD_PLARGE_INTEGER;
typedef ULARGE_INTEGER		QVOD_ULARGE_INTEGER;
typedef PULARGE_INTEGER		QVOD_PULARGE_INTEGER;

typedef GUID				QVOD_GUID;


#else /* posix */


#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>
#include <stdlib.h>


#define QVOD_WINAPI
#define QVOD_INFINITE		0
#define QVOD_MAX_PATH		260
#define QVOD_TRUE			1
#define QVOD_FALSE			0
#define QVOD_IN
#define QVOD_OUT

typedef int					QVOD_BOOL;
typedef unsigned char		QVOD_BYTE;
typedef unsigned char		QVOD_UCHAR;
typedef int8_t				QVOD_INT8;
typedef uint8_t				QVOD_UINT8;
typedef int16_t				QVOD_INT16;
typedef uint16_t			QVOD_UINT16;
typedef unsigned short		QVOD_WORD; /* 16-bit */
typedef unsigned short		QVOD_USHORT;
typedef unsigned long		QVOD_DWORD; /* 32-bit */
typedef int					QVOD_INT;
typedef unsigned int		QVOD_UINT;
typedef int32_t				QVOD_INT32;
typedef uint32_t			QVOD_UINT32;
typedef long				QVOD_LONG;
typedef unsigned long		QVOD_ULONG;
typedef int64_t				QVOD_INT64;
typedef uint64_t			QVOD_UINT64;
typedef int64_t				QVOD_LONGLONG;
typedef uint64_t			QVOD_ULONGLONG;

typedef union _LARGE_INTEGER {
	struct {
		QVOD_DWORD LowPart;
		QVOD_LONG  HighPart;
	};
	struct {
		QVOD_DWORD LowPart;
		QVOD_LONG  HighPart;
	} u;
	QVOD_LONGLONG QuadPart;
} QVOD_LARGE_INTEGER, *QVOD_PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
	struct {
		QVOD_DWORD LowPart;
		QVOD_DWORD HighPart;
	} ;
	struct {
		QVOD_DWORD LowPart;
		QVOD_DWORD HighPart;
	} u;
	QVOD_ULONGLONG QuadPart;
} QVOD_ULARGE_INTEGER, *QVOD_PULARGE_INTEGER;


typedef struct _GUID {
    QVOD_DWORD Data1;
    QVOD_WORD  Data2;
    QVOD_WORD  Data3;
    QVOD_BYTE  Data4[8];
} QVOD_GUID;


#endif /* posix end */

typedef QVOD_DWORD* LPQVOD_DWORD;


#define QVOD_OK    		0
#define QVOD_ERROR 		-1
#define QVOD_FOUND 		-2
#define QVOD_NOFOUND 	-3
#define QVOD_EROFS		-4

#define QVOD_MIN2(a,b) (((a)>(b))?(b):(a))
#define QVOD_MAX2(a,b) (((a)>(b))?(a):(b))


#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif


#endif /* _QVOD_BASETYPES_H_ */

