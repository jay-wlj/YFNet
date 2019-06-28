#include "MD5Checksum.h"
#ifndef WIN32
#include "global.h"
#include <sys/time.h>
#endif


typedef unsigned char       BYTE;
typedef unsigned int        UINT;
typedef unsigned short 		USHORT;

#ifdef MONGOTV
 int g_version = 152;		//芒果tv
#else
 int g_version = 100;
#endif

char m_peerID[24];

#pragma obfuscate on

#define MyRotateLeft( x,  n)	(((unsigned int)(x) << (n)) | ((unsigned int)(x) >> (32-n)))

void FF( unsigned int& A, unsigned int B, unsigned int C, unsigned int D, unsigned int X, unsigned int S, unsigned int T)
{
	unsigned int F = (B & C) | (~B & D);
	A += F + X + T;
	A = MyRotateLeft(A, S);
	A += B;
}
void GG( unsigned int& A, unsigned int B, unsigned int C, unsigned int D, unsigned int X, unsigned int S, unsigned int T)
{
	unsigned int G = (B & D) | (C & ~D);
	A += G + X + T;
	A = MyRotateLeft(A, S);
	A += B;
}
void HH( unsigned int& A, unsigned int B, unsigned int C, unsigned int D, unsigned int X, unsigned int S, unsigned int T)
{
	unsigned int H = (B ^ C ^ D);
	A += H + X + T;
	A = MyRotateLeft(A, S);
	A += B;
}
void II( unsigned int& A, unsigned int B, unsigned int C, unsigned int D, unsigned int X, unsigned int S, unsigned int T)
{
	unsigned int I = (C ^ (B | ~D));
	A += I + X + T;
	A = MyRotateLeft(A, S);
	A += B;
}
unsigned int MyChar2Hash(const char* chars,int begin,int end)
{
	int i = begin;
	unsigned int hash = 0;
	while (i <= end) 
	{
		if('9'>= chars[i] && chars[i]>= '0' ) 
			hash += (chars[i] - '0')<< 4*(end-i);
		else if('F'>= chars[i] && chars[i]>= 'A' )
			hash += (chars[i] - 'A'+10)<< 4*(end-i);
		else
			return 0;
		i++;
	}
	return hash;
}
void makePeerID()
{
	char strtime[30];
#ifdef WIN32	
	LARGE_INTEGER lint;
	QueryPerformanceCounter(&lint);
	sprintf(strtime,"QVOD%04d%012I64X",g_version,lint);
#else
	struct timeval tmptime;
	gettimeofday(&tmptime, NULL);
	long long longtime = ((long long)tmptime.tv_sec) * 1000000 + tmptime.tv_usec;
	long long tmp = 0xffffffff;
	tmp = (tmp << 16) + 0xffff + 1;
	longtime = longtime % tmp;
	sprintf(strtime,"QVOD%04d%012llX",g_version, longtime);
#endif
	memcpy(m_peerID ,strtime,20);
	unsigned int a = MD5_INIT_STATE_0;
	unsigned int b = MD5_INIT_STATE_1;
	unsigned int c = MD5_INIT_STATE_2;
	unsigned int d = MD5_INIT_STATE_3;

	unsigned int X = MyChar2Hash(strtime+8,4,11);

	FF (a, b, c, d, X, MD5_S11, MD5_T01); 
	GG (a, b, c, d, X, MD5_S21, MD5_T17); 
	HH (a, b, c, d, X, MD5_S31, MD5_T33); 
	II (a, b, c, d, X, MD5_S41, MD5_T49); 

	unsigned short Y = (a ^(a >> 16));
	unsigned char T = Y>>12;
	m_peerID[8] = T > 9 ? (T-10+'A') :(T+'0');
	T = (Y&0x0F00)>>8;
	m_peerID[9] = T > 9 ? (T-10+'A') :(T+'0');
	T = (Y&0x00F0)>>4;
	m_peerID[10] = T > 9 ? (T-10+'A') :(T+'0');
	T = Y&0x000F;
	m_peerID[11] = T > 9 ? (T-10+'A') :(T+'0');
}
const char key[32] = {12,29,3,7,15,17,24,9
,2,2,11,8,31,5,20,16
,1,28,27,30,21,18,13,10
,23,22,14,4,6,19,25,26};//0< x <32

void Enc2(const unsigned char* pSrc,unsigned char* pDes,int len,unsigned int key2)
{
	int i,j,n=0;
	unsigned int k;
	
	memcpy(pDes,pSrc,len);
	unsigned char* pchr = pDes;
	unsigned int* PUINT = (unsigned int*)pchr;
	k = key2;
	for(i = 0;i<len/4;i++)
	{
		PUINT[i] = (PUINT[i]>>key[n]) + (PUINT[i]<<(32-key[n]));
		PUINT[i] ^= k;
		k = PUINT[i];
		n = pchr[3+i*4]%32;
	}
	unsigned char* pkey2 = (unsigned char*)&key2;
	n%=8;
	for(j = i*4;j<len;j++)
	{
		pchr[j] = (pchr[j] >> n) + (pchr[j] << (8-n));
		pchr[j] ^= *pkey2++;
		n = pchr[j]%8;
	}
				
}

void Dec2(const unsigned char* pSrc,unsigned char* pDes,int len,unsigned int key2)
{
	
	int i,j,nn,n = 0;
	unsigned char* pchr = pDes;
	memcpy(pchr,pSrc,len);
	unsigned int k,nk;
	k = key2;
	
	unsigned int* PUINT = (unsigned int*)pchr;	
	for(i = 0;i<len/4;i++)
	{
		nn = pchr[3+i*4]%32;
		nk = PUINT[i];
		PUINT[i] ^= k;
		PUINT[i] = (PUINT[i]<<key[n]) + (PUINT[i]>>(32-key[n]));
		n = nn;
		k = nk;
	}
	unsigned char* pkey2 = (unsigned char*)&key2;
	n%=8;
	for(j = i*4;j<len;j++)
	{
		nn = pchr[j]%8;
		pchr[j] ^= *pkey2++;
		pchr[j] = (pchr[j] << n) + (pchr[j] >> (8-n));
		n = nn;
	}
				
}

const char key3[32] = {
	2,10,8,30,5,20,16,1,29,
	12,28,3,7,18,17,24,9,2,
	27,31,25,15,26,11,23,22,
	14,4,6,19,21,13};
void Enc3(const BYTE* pSrc,BYTE* pDes,int len,unsigned int key)
{
	int i,j,n,m;
	UINT k;
	USHORT a,b;
	memcpy(pDes,pSrc,len);
	BYTE* pchr = pDes;
	UINT* puint = (UINT*)pchr;
	k = key;
	n = 0;
	for(i = 0;i<len/4;i++)
	{
		puint[i] = (puint[i]>>key3[n]) + (puint[i]<<(32-key3[n]));
		puint[i] ^= k;
		m = puint[i];//以中间值作为下一个加密串的key,下一个key被察觉的问题
		memcpy((void*)&a, (void*)(pchr+i*4), 2);
		memcpy((void*)&b, (void*)(pchr+i*4+2), 2);
		a += b>>(k%16);//增加一种变化原理,增加一次变化
		b += a>>(16-k%16);
		memcpy( (void*)(pchr+i*4),(void*)&a, 2);
		memcpy((void*)(pchr+i*4+2), (void*)&b, 2);
		k = m;//做到每一轮的key不一样
		n = pchr[3+i*4]%32;
	}
	BYTE* pkey2 = (BYTE*)&key;
	n%=8;
	for(j = i*4;j<len;j++)
	{
		pchr[j] = (pchr[j]>>n) + (pchr[j]<<(8-n));
		pchr[j] ^= *pkey2++;
		n = pchr[j]%8;
	}
}

void Dec3(const BYTE* pSrc,BYTE* pDes,int len,unsigned int key)
{
	int i,j,nn,n;
	USHORT a,b;
	BYTE* pchr = pDes;
	memcpy(pchr,pSrc,len);
	UINT k,nk;
	k = key;
	n = 0;
	UINT* puint = (UINT*)pchr; 
	for(i = 0;i<len/4;i++)
	{
		nn = pchr[3+i*4]%32;
		memcpy((void*)&a, (void*)(pchr+i*4), 2);
		memcpy((void*)&b, (void*)(pchr+i*4+2), 2);
		b -= a>>(16-k%16);
		a -= b>>(k%16);
		memcpy( (void*)(pchr+i*4),(void*)&a, 2);
		memcpy((void*)(pchr+i*4+2), (void*)&b, 2);
		nk = puint[i];
		puint[i] ^= k;
		puint[i] = (puint[i]<<key3[n]) + (puint[i]>>(32-key3[n]));
		n = nn;
		k = nk;
	}
	BYTE* pkey2 = (BYTE*)&key;
	n%=8;
	for(j = i*4;j<len;j++)
	{
		nn = pchr[j]%8;
		pchr[j] ^= *pkey2++;
		pchr[j] = (pchr[j]<<n) + (pchr[j]>>(8-n));
		n = nn;
	}
}

const char key4[32] = {
27,31,25,15,26,11,23,22,
12,28,3,7,18,17,24,9,
2,2,10,8,30,5,20,16,
1,29,14,4,6,19,21,13};

void Enc4(const BYTE* pSrc,BYTE* pDes,int len,unsigned int key)
{
	int i,j,n,m;
	UINT k;
	USHORT a,b;
	memcpy(pDes,pSrc,len);
	BYTE* pchr = pDes;
	UINT* puint = (UINT*)pchr;
	k = key;
	n = 0;
	for(i = 0;i<len/4;i++)
	{
		puint[i] = (puint[i]>>key4[n]) + (puint[i]<<(32-key4[n]));
		puint[i] ^= k;
		m = puint[i];//以中间值作为下一个加密串的key,下一个key被察觉的问题
		memcpy((void*)&a, (void*)(pchr+i*4), 2);
		memcpy((void*)&b, (void*)(pchr+i*4+2), 2);
		a += b>>(k%16);//增加一种变化原理,增加一次变化
		b += a>>(16-k%16);
		memcpy( (void*)(pchr+i*4),(void*)&a, 2);
		memcpy((void*)(pchr+i*4+2), (void*)&b, 2);
		k = m;//做到每一轮的key不一样
		n = pchr[3+i*4]%32;
	}
	
	BYTE* pkey2 = (BYTE*)&key;
	n%=8;
	for(j = i*4;j<len;j++)
	{
		pchr[j] = (pchr[j]>>n) + (pchr[j]<<(8-n));
		pchr[j] ^= *pkey2++;
		n = pchr[j]%8;
	}
}

void Dec4(const BYTE* pSrc,BYTE* pDes,int len,unsigned int key)
{
	int i,j,nn,n;
	USHORT a,b;
	BYTE* pchr = pDes;
	memcpy(pchr,pSrc,len);
	UINT k,nk;
	k = key;
	n = 0;
	UINT* puint = (UINT*)pchr; 
	for(i = 0;i<len/4;i++)
	{
		nn = pchr[3+i*4]%32;
		memcpy((void*)&a, (void*)(pchr+i*4), 2);
		memcpy((void*)&b, (void*)(pchr+i*4+2), 2);
		b -= a>>(16-k%16);
		a -= b>>(k%16);
		memcpy( (void*)(pchr+i*4),(void*)&a, 2);
		memcpy((void*)(pchr+i*4+2), (void*)&b, 2);
		nk = puint[i];
		puint[i] ^= k;
		puint[i] = (puint[i]<<key4[n]) + (puint[i]>>(32-key4[n]));
		n = nn;
		k = nk;
	}
	BYTE* pkey2 = (BYTE*)&key;
	n%=8;
	for(j = i*4;j<len;j++)
	{
		nn = pchr[j]%8;
		pchr[j] ^= *pkey2++;
		pchr[j] = (pchr[j]<<n) + (pchr[j]>>(8-n));
		n = nn;
	}
}
#pragma obfuscate off
