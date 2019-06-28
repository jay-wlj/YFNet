#include "tea.h"
#include <cstring> //for memcpy,memset

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

using namespace std;
static unsigned char g_szDefaultKey[16] = {
	0xDE,0x49,0x7D,0x83,
	0xF9,0xAE,0x34,0xB8,
	0xC4,0x95,0x24,0xFF,
	0x00,0x18,0xAF,0xDE
};

static unsigned char g_szIpListKey[16] = {
	0x17,0x85,0x74,0x56,
	0x98,0x12,0x94,0xfa,
	0xa1,0xe7,0x2b,0x45,
	0xcd,0x1c,0x32,0x4a
};

#define MAX_CODE_LENGTH		256		//最大加解密长度

void Tea::SetKey(unsigned char *key)
{
	memcpy(m_szKey,key,16);
}

void Tea::RetKey()
{
	memcpy(m_szKey,g_szDefaultKey,16);
}

void Tea::encrypt(unsigned char *pData,int len,bool bUsingPublicKey,bool bLimitLen)
{
	unsigned long *k = 0;
	if ( bUsingPublicKey )
	{
		k = (unsigned long *)g_szDefaultKey;
	}
	else
	{
		k = (unsigned long*)m_szKey;
	}
	register unsigned long a = k[0];
	register unsigned long b = k[1];
	register unsigned long c = k[2];
	register unsigned long d = k[3];
	register unsigned long delta = 0x9E3779B9; /* (sqrt(5)-1)/2*2^32 */

	unsigned char *pPos = pData;
	int iMinLen = sizeof(long) * 2;
	if (bLimitLen)
	{
		len = min(len,MAX_CODE_LENGTH);
	}
	while ( len >= iMinLen )
	{
		register unsigned long sum = 0;
		register unsigned long y = *(unsigned long *)pPos;
		register unsigned long z = *(unsigned long *)(pPos+sizeof(unsigned long));
		register int round = 16;

		while (round--) 
		{	/* basic cycle start */
			sum += delta;
			y += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
			z += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
		}	/* end cycle */
		memcpy(pPos,&y,sizeof(y));
		memcpy(pPos+sizeof(y),&z,sizeof(y));
		pPos = pPos + sizeof(y) + sizeof(z);
		len -= sizeof(y)+sizeof(z);
	}
}

void Tea::decrypt(unsigned char *pData,int len,bool bUsingPublicKey,bool bLimitLen)
{
	unsigned long *k = 0;
	if ( bUsingPublicKey )
	{
		k = (unsigned long *)g_szDefaultKey;
	}
	else
	{
		k = (unsigned long*)m_szKey;
	}
	register unsigned long a = k[0];
	register unsigned long b = k[1];
	register unsigned long c = k[2];
	register unsigned long d = k[3];
	register unsigned long delta = 0x9E3779B9;

	unsigned char *pPos = pData;
	int iMinLen = sizeof(long) * 2;
	if (bLimitLen)
	{
		len = min(len,MAX_CODE_LENGTH);
	}
	while ( len >= iMinLen )
	{
		register int round = 16;
		register unsigned long sum = 0xE3779B90;
		register unsigned long y = *(unsigned long *)pPos;
		register unsigned long z = *(unsigned long *)(pPos+sizeof(unsigned long));

		while (round--)
		{
			z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
			y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
			sum -= delta;
		}
		memcpy(pPos,&y,sizeof(y));
		memcpy(pPos+sizeof(y),&z,sizeof(z));

		pPos += sizeof(y) + sizeof(z);
		len -= sizeof(y) + sizeof(z);
	}
}

void Tea::Encrypt(unsigned char *pData,int len)
{
	unsigned long *k =(unsigned long*)g_szIpListKey;
	register unsigned long a = k[0];
	register unsigned long b = k[1];
	register unsigned long c = k[2];
	register unsigned long d = k[3];
	register unsigned long delta = 0x9E3779B9; /* (sqrt(5)-1)/2*2^32 */

	unsigned char *pPos = pData;
	int iMinLen = sizeof(long) * 2;
	while ( len >= iMinLen )
	{
		register unsigned long sum = 0;
		register unsigned long y = *(unsigned long *)pPos;
		register unsigned long z = *(unsigned long *)(pPos+sizeof(unsigned long));
		register int round = 16;

		while (round--) 
		{	/* basic cycle start */
			sum += delta;
			y += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
			z += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
		}	/* end cycle */
		memcpy(pPos,&y,sizeof(y));
		memcpy(pPos+sizeof(y),&z,sizeof(y));
		pPos = pPos + sizeof(y) + sizeof(z);
		len -= sizeof(y)+sizeof(z);
	}
	while(len)
	{
		*pPos -= 0x40;
		++pPos;
		--len;
	}
}

void Tea::Decrypt(unsigned char *pData,int len)
{
	unsigned long *k = (unsigned long*)g_szIpListKey;
	register unsigned long a = k[0];
	register unsigned long b = k[1];
	register unsigned long c = k[2];
	register unsigned long d = k[3];
	register unsigned long delta = 0x9E3779B9;

	unsigned char *pPos = pData;
	int iMinLen = sizeof(long) * 2;
	while ( len >= iMinLen )
	{
		register int round = 16;
		register unsigned long sum = 0xE3779B90;
		register unsigned long y = *(unsigned long *)pPos;
		register unsigned long z = *(unsigned long *)(pPos+sizeof(unsigned long));

		while (round--)
		{
			z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
			y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
			sum -= delta;
		}
		memcpy(pPos,&y,sizeof(y));
		memcpy(pPos+sizeof(y),&z,sizeof(z));

		pPos += sizeof(y) + sizeof(z);
		len -= sizeof(y) + sizeof(z);
	}
	while(len)
	{
		*pPos += 0x40;
		++pPos;
		--len;
	}
}

void Tea::EncryptTask(unsigned char *pData,int len,char *pKey)
{
	unsigned long *k =(unsigned long*)pKey;
	register unsigned long a = k[0];
	register unsigned long b = k[1];
	register unsigned long c = k[2];
	register unsigned long d = k[3];
	register unsigned long delta = 0x9E3779B9; /* (sqrt(5)-1)/2*2^32 */

	len = min(len,1024);
	unsigned char *pPos = pData;
	int iMinLen = sizeof(long) * 2;
	while ( len >= iMinLen )
	{
		register unsigned long sum = 0;
		register unsigned long y = *(unsigned long *)pPos;
		register unsigned long z = *(unsigned long *)(pPos+sizeof(unsigned long));
		register int round = 16;

		while (round--) 
		{	/* basic cycle start */
			sum += delta;
			y += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
			z += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
		}	/* end cycle */
		memcpy(pPos,&y,sizeof(y));
		memcpy(pPos+sizeof(y),&z,sizeof(y));
		pPos = pPos + sizeof(y) + sizeof(z);
		len -= sizeof(y)+sizeof(z);
	}
	while(len)
	{
		*pPos -= 0x40;
		++pPos;
		--len;
	}
}

void Tea::DecryptTask(unsigned char *pData,int len,char *pKey)
{
	unsigned long *k = (unsigned long*)pKey;
	register unsigned long a = k[0];
	register unsigned long b = k[1];
	register unsigned long c = k[2];
	register unsigned long d = k[3];
	register unsigned long delta = 0x9E3779B9;

	len = min(len,1024);
	unsigned char *pPos = pData;
	int iMinLen = sizeof(long) * 2;
	while ( len >= iMinLen )
	{
		register int round = 16;
		register unsigned long sum = 0xE3779B90;
		register unsigned long y = *(unsigned long *)pPos;
		register unsigned long z = *(unsigned long *)(pPos+sizeof(unsigned long));

		while (round--)
		{
			z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
			y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
			sum -= delta;
		}
		memcpy(pPos,&y,sizeof(y));
		memcpy(pPos+sizeof(y),&z,sizeof(z));

		pPos += sizeof(y) + sizeof(z);
		len -= sizeof(y) + sizeof(z);
	}
	while(len)
	{
		*pPos += 0x40;
		++pPos;
		--len;
	}
}