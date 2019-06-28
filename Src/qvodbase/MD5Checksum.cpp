// MD5Checksum.cpp: implementation of the MD5Checksum class.


#include "MD5Checksum.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

/*****************************************************************************************
FUNCTION: CMD5Checksum::GetMD5
DETAILS: static, public
DESCRIPTION: Gets the MD5 checksum for data in a BYTE array
RETURNS: std::string : the hexadecimal MD5 checksum for the specified data
ARGUMENTS: BYTE* pBuf : pointer to the BYTE array
UINT nLength : number of BYTEs of data to be checksumed
NOTES: Provides an interface to the CMD5Checksum class. Any data that can
be cast to a BYTE array of known length can be checksummed by this
function. Typically, std::string and char arrays will be checksumed, 
although this function can be used to check the integrity of any BYTE array. 
A buffer of zero length can be checksummed; all buffers of zero length 
will return the same checksum. 
*****************************************************************************************/
std::string CMD5Checksum::GetMD5(QVOD_BYTE* pBuf, QVOD_UINT nLength)
{
	
	//calculate and return the checksum
	CMD5Checksum MD5Checksum;
	MD5Checksum.Update( pBuf, nLength );
	return MD5Checksum.Final();
}


/*****************************************************************************************
FUNCTION: CMD5Checksum::RotateLeft
DETAILS: private
DESCRIPTION: Rotates the bits in a 32 bit DWORD left by a specified amount
RETURNS: The rotated DWORD 
ARGUMENTS: DWORD x : the value to be rotated
int n : the number of bits to rotate by
*****************************************************************************************/
QVOD_DWORD CMD5Checksum::RotateLeft(QVOD_DWORD x, int n)
{
	
	//rotate and return x
	return (x << n) | (x >> (32-n));
}
void CMD5Checksum::FF( QVOD_DWORD& A, QVOD_DWORD B, QVOD_DWORD C, QVOD_DWORD D, QVOD_DWORD X, QVOD_DWORD S, QVOD_DWORD T)
{
QVOD_DWORD F = (B & C) | (~B & D);
A += F + X + T;
A = RotateLeft(A, S);
A += B;
}
void CMD5Checksum::GG( QVOD_DWORD& A, QVOD_DWORD B, QVOD_DWORD C, QVOD_DWORD D, QVOD_DWORD X, QVOD_DWORD S, QVOD_DWORD T)
{
QVOD_DWORD G = (B & D) | (C & ~D);
A += G + X + T;
A = RotateLeft(A, S);
A += B;
}
void CMD5Checksum::HH( QVOD_DWORD& A, QVOD_DWORD B, QVOD_DWORD C, QVOD_DWORD D, QVOD_DWORD X, QVOD_DWORD S, QVOD_DWORD T)
{
QVOD_DWORD H = (B ^ C ^ D);
A += H + X + T;
A = RotateLeft(A, S);
A += B;
}
void CMD5Checksum::II( QVOD_DWORD& A, QVOD_DWORD B, QVOD_DWORD C, QVOD_DWORD D, QVOD_DWORD X, QVOD_DWORD S, QVOD_DWORD T)
{
QVOD_DWORD I = (C ^ (B | ~D));
A += I + X + T;
A = RotateLeft(A, S);
A += B;
}
void CMD5Checksum::ByteToDWord(QVOD_DWORD* Output, QVOD_BYTE* Input, QVOD_UINT nLength)
{
//entry invariants

//initialisations
QVOD_UINT i=0; //index to Output array
QVOD_UINT j=0; //index to Input array

//transfer the data by shifting and copying
for ( ; j < nLength; i++, j += 4)
{
Output[i] = (QVOD_ULONG)Input[j] |
(QVOD_ULONG)Input[j+1] << 8 |
(QVOD_ULONG)Input[j+2] << 16 |
(QVOD_ULONG)Input[j+3] << 24;
}
}
void CMD5Checksum::Transform(QVOD_BYTE Block[64])
{
//initialise local data with current checksum
QVOD_ULONG a = m_lMD5[0];
QVOD_ULONG b = m_lMD5[1];
QVOD_ULONG c = m_lMD5[2];
QVOD_ULONG d = m_lMD5[3];

//copy BYTES from input 'Block' to an array of ULONGS 'X'
QVOD_ULONG X[16];
ByteToDWord( X, Block, 64 );

//Perform Round 1 of the transformation
FF (a, b, c, d, X[ 0], MD5_S11, MD5_T01); 
FF (d, a, b, c, X[ 1], MD5_S12, MD5_T02); 
FF (c, d, a, b, X[ 2], MD5_S13, MD5_T03); 
FF (b, c, d, a, X[ 3], MD5_S14, MD5_T04); 
FF (a, b, c, d, X[ 4], MD5_S11, MD5_T05); 
FF (d, a, b, c, X[ 5], MD5_S12, MD5_T06); 
FF (c, d, a, b, X[ 6], MD5_S13, MD5_T07); 
FF (b, c, d, a, X[ 7], MD5_S14, MD5_T08); 
FF (a, b, c, d, X[ 8], MD5_S11, MD5_T09); 
FF (d, a, b, c, X[ 9], MD5_S12, MD5_T10); 
FF (c, d, a, b, X[10], MD5_S13, MD5_T11); 
FF (b, c, d, a, X[11], MD5_S14, MD5_T12); 
FF (a, b, c, d, X[12], MD5_S11, MD5_T13); 
FF (d, a, b, c, X[13], MD5_S12, MD5_T14); 
FF (c, d, a, b, X[14], MD5_S13, MD5_T15); 
FF (b, c, d, a, X[15], MD5_S14, MD5_T16); 

//Perform Round 2 of the transformation
GG (a, b, c, d, X[ 1], MD5_S21, MD5_T17); 
GG (d, a, b, c, X[ 6], MD5_S22, MD5_T18); 
GG (c, d, a, b, X[11], MD5_S23, MD5_T19); 
GG (b, c, d, a, X[ 0], MD5_S24, MD5_T20); 
GG (a, b, c, d, X[ 5], MD5_S21, MD5_T21); 
GG (d, a, b, c, X[10], MD5_S22, MD5_T22); 
GG (c, d, a, b, X[15], MD5_S23, MD5_T23); 
GG (b, c, d, a, X[ 4], MD5_S24, MD5_T24); 
GG (a, b, c, d, X[ 9], MD5_S21, MD5_T25); 
GG (d, a, b, c, X[14], MD5_S22, MD5_T26); 
GG (c, d, a, b, X[ 3], MD5_S23, MD5_T27); 
GG (b, c, d, a, X[ 8], MD5_S24, MD5_T28); 
GG (a, b, c, d, X[13], MD5_S21, MD5_T29); 
GG (d, a, b, c, X[ 2], MD5_S22, MD5_T30); 
GG (c, d, a, b, X[ 7], MD5_S23, MD5_T31); 
GG (b, c, d, a, X[12], MD5_S24, MD5_T32); 

//Perform Round 3 of the transformation
HH (a, b, c, d, X[ 5], MD5_S31, MD5_T33); 
HH (d, a, b, c, X[ 8], MD5_S32, MD5_T34); 
HH (c, d, a, b, X[11], MD5_S33, MD5_T35); 
HH (b, c, d, a, X[14], MD5_S34, MD5_T36); 
HH (a, b, c, d, X[ 1], MD5_S31, MD5_T37); 
HH (d, a, b, c, X[ 4], MD5_S32, MD5_T38); 
HH (c, d, a, b, X[ 7], MD5_S33, MD5_T39); 
HH (b, c, d, a, X[10], MD5_S34, MD5_T40); 
HH (a, b, c, d, X[13], MD5_S31, MD5_T41); 
HH (d, a, b, c, X[ 0], MD5_S32, MD5_T42); 
HH (c, d, a, b, X[ 3], MD5_S33, MD5_T43); 
HH (b, c, d, a, X[ 6], MD5_S34, MD5_T44); 
HH (a, b, c, d, X[ 9], MD5_S31, MD5_T45); 
HH (d, a, b, c, X[12], MD5_S32, MD5_T46); 
HH (c, d, a, b, X[15], MD5_S33, MD5_T47); 
HH (b, c, d, a, X[ 2], MD5_S34, MD5_T48); 

//Perform Round 4 of the transformation
II (a, b, c, d, X[ 0], MD5_S41, MD5_T49); 
II (d, a, b, c, X[ 7], MD5_S42, MD5_T50); 
II (c, d, a, b, X[14], MD5_S43, MD5_T51); 
II (b, c, d, a, X[ 5], MD5_S44, MD5_T52); 
II (a, b, c, d, X[12], MD5_S41, MD5_T53); 
II (d, a, b, c, X[ 3], MD5_S42, MD5_T54); 
II (c, d, a, b, X[10], MD5_S43, MD5_T55); 
II (b, c, d, a, X[ 1], MD5_S44, MD5_T56); 
II (a, b, c, d, X[ 8], MD5_S41, MD5_T57); 
II (d, a, b, c, X[15], MD5_S42, MD5_T58); 
II (c, d, a, b, X[ 6], MD5_S43, MD5_T59); 
II (b, c, d, a, X[13], MD5_S44, MD5_T60); 
II (a, b, c, d, X[ 4], MD5_S41, MD5_T61); 
II (d, a, b, c, X[11], MD5_S42, MD5_T62); 
II (c, d, a, b, X[ 2], MD5_S43, MD5_T63); 
II (b, c, d, a, X[ 9], MD5_S44, MD5_T64); 

//add the transformed values to the current checksum
m_lMD5[0] += a;
m_lMD5[1] += b;
m_lMD5[2] += c;
m_lMD5[3] += d;
}
CMD5Checksum::CMD5Checksum()
{
// zero members
memset( m_lpszBuffer, 0, 64 );
m_nCount[0] = m_nCount[1] = 0;

// Load magic state initialization constants
m_lMD5[0] = MD5_INIT_STATE_0;
m_lMD5[1] = MD5_INIT_STATE_1;
m_lMD5[2] = MD5_INIT_STATE_2;
m_lMD5[3] = MD5_INIT_STATE_3;
}
void CMD5Checksum::DWordToByte(QVOD_BYTE* Output, QVOD_DWORD* Input, QVOD_UINT nLength )
{
	//entry invariants
	
	//transfer the data by shifting and copying
	QVOD_UINT i = 0;
	QVOD_UINT j = 0;
	for ( ; j < nLength; i++, j += 4) 
	{
		Output[j] = (QVOD_UCHAR)(Input[i] & 0xff);
		Output[j+1] = (QVOD_UCHAR)((Input[i] >> 8) & 0xff);
		Output[j+2] = (QVOD_UCHAR)((Input[i] >> 16) & 0xff);
		Output[j+3] = (QVOD_UCHAR)((Input[i] >> 24) & 0xff);
	}
}
std::string CMD5Checksum::Final()
{
	//Save number of bits
	QVOD_BYTE Bits[8];
	DWordToByte( Bits, m_nCount, 8 );
	
	//Pad out to 56 mod 64.
	QVOD_UINT nIndex = (QVOD_UINT)((m_nCount[0] >> 3) & 0x3f);
	QVOD_UINT nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
	Update( PADDING, nPadLen );
	
	//Append length (before padding)
	Update( Bits, 8 );
	
	//Store final state in 'lpszMD5'
	const int nMD5Size = 16;
	unsigned char lpszMD5[ nMD5Size ];
	DWordToByte( lpszMD5, m_lMD5, nMD5Size );
	
	//Convert the hexadecimal checksum to a std::string
	std::string strMD5;
	for ( int i=0; i < nMD5Size; i++) 
	{
		char str[8];
		if (lpszMD5[i] == 0) {
			strcpy(str,"00");
		}
		else if (lpszMD5[i] <= 15) {
			sprintf(str,"0%x",lpszMD5[i]);
		}
		else {
			sprintf(str,"%x",lpszMD5[i]);
		}
		
		strMD5 += str;
	}
	return strMD5;
}
void CMD5Checksum::Update( QVOD_BYTE* Input, QVOD_ULONG nInputLen )
{
	//Compute number of bytes mod 64
	QVOD_UINT nIndex = (QVOD_UINT)((m_nCount[0] >> 3) & 0x3F);
	
	//Update number of bits
	if ( ( m_nCount[0] += nInputLen << 3 ) < ( nInputLen << 3) )
	{
		m_nCount[1]++;
	}
	m_nCount[1] += (nInputLen >> 29);
	
	//Transform as many times as possible.
	QVOD_UINT i=0;
	QVOD_UINT nPartLen = 64 - nIndex;
	if (nInputLen >= nPartLen) 
	{
		memcpy( &m_lpszBuffer[nIndex], Input, nPartLen );
		Transform( m_lpszBuffer );
		for (i = nPartLen; i + 63 < nInputLen; i += 64) 
		{
			Transform( &Input[i] );
		}
		nIndex = 0;
	} 
	else 
	{
		i = 0;
	}
	
	// Buffer remaining input
	memcpy( &m_lpszBuffer[nIndex], &Input[i], nInputLen-i);
}
