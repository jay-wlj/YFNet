#include "AESData.h"
#include "qvod_time.h"
#include "common-commonfun.h"

static unsigned char s_vector[16]={ 0x1c,0x42,0x53,0xbd,0xaf,0x4b,0xcd,0x2d,
	0x3b,0xda,0x13,0x6e,0x9b,0x41,0x5c,0x7b };

CAESData::CAESData(void)
{
}


CAESData::~CAESData(void)
{
}

void CAESData::SetKey(unsigned char key[16])										// 采用ECB模式加密
{
	m_strKey.assign((char*)key, 16);

	AES_set_encrypt_key(key, 128, &m_encryptKey);
	AES_set_decrypt_key(key, 128, &m_decryptKey);
	
	//AesSetKey( &m_context , BLOCKMODE_ECB, key , s_vector );
	//AesSetFeedbackSize( &m_context , 1);
}

void CAESData::SetKey(const string& strKey, int nVer)
{
	m_strKey = strKey;

	AES_set_encrypt_key((const unsigned char*)strKey.c_str(), 128, &m_encryptKey);
	AES_set_decrypt_key((const unsigned char*)strKey.c_str(), 128, &m_decryptKey);
}

string CAESData::GetKey()
{
	return m_strKey;
}

bool CAESData::ExistKey()
{
	return !m_strKey.empty();
}
void CAESData::Encrpyt(void *pInBuf, void *pOutBuf, int nSize)			// 加密数据
{
	//AesEncryptECB(&m_context , pInBuf, pOutBuf, nSize );
	int en_len = 0;
	while (en_len < nSize)
	{
		AES_encrypt((const unsigned char*)pInBuf+en_len, (unsigned char*)pOutBuf+en_len, &m_encryptKey);
		en_len += AES_BLOCK_SIZE;
	}
	
}

void CAESData::Decrpyt(void *pInBuf, void *pOutBuf, int nSize)			// 解密数据
{
	//AesDecryptECB(&m_context, pInBuf, pOutBuf, nSize);
	int en_len = 0;
	while (en_len < nSize)
	{
		AES_decrypt((const unsigned char*)pInBuf+en_len, (unsigned char*)pOutBuf+en_len, &m_decryptKey);
		en_len += AES_BLOCK_SIZE;
	}
}