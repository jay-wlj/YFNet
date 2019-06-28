
#pragma once

#include "qvod_string.h"
#include <string>

using namespace std;


#ifndef MACOSX
#if defined(BSD) || defined(WIN32)
typedef unsigned int uint32_t;
typedef int int_least16_t;
typedef unsigned char uint8_t;
#endif
#endif

/*
 * If you do not have the ISO standard stdint.h header file, then you
 * must typdef the following:
 *    name              meaning
 *  uint32_t         unsigned 32 bit integer
 *  uint8_t          unsigned 8 bit integer (i.e., unsigned char)
 *  int_least16_t    integer of >= 16 bits
 *
 */

#ifndef _SHA_enum_
#define _SHA_enum_
enum
{
    shaSuccess = 0,
    shaNull,            /* Null pointer parameter */
    shaInputTooLong,    /* input data too long */
    shaStateError       /* called Input after Result */
};
#endif
#define SHA1HashSize 20

/*
 *  This structure will hold context information for the SHA-1
 *  hashing operation
 */
typedef struct SHA1Context
{
    QVOD_UINT32 Intermediate_Hash[SHA1HashSize/4]; /* Message Digest  */

    QVOD_UINT32 Length_Low;            /* Message length in bits      */
    QVOD_UINT32 Length_High;           /* Message length in bits      */

                               /* Index into message block array   */
    QVOD_INT Message_Block_Index;
    QVOD_UINT8 Message_Block[64];      /* 512-bit message blocks      */

    int Computed;               /* Is the digest computed?         */
    int Corrupted;             /* Is the message digest corrupted? */
} SHA1Context;

/*
 *  Function Prototypes
 */

int SHA1Reset(  SHA1Context *);
int SHA1Input(  SHA1Context *,
							const void *,
							unsigned int);
int SHA1Result( SHA1Context *,
							QVOD_UINT8 Message_Digest[SHA1HashSize]);

class  CSHA1
{
public:
	CSHA1();
	CSHA1(const void*, int);
public:
	int reset();
	void read(void*);
	string read();
	string readHex();
	void write(const void*, int);
private:
	SHA1Context m_context;
};

