#pragma once
//#include "EfAes.H"
#include "openssl/aes.h"
#include "qvod_string.h"

class CAESData
{
public:
	CAESData(void);
	~CAESData(void);

	void SetKey(unsigned char key[16]);												// ����ECBģʽ����
	void SetKey(const string& strKey, int nVer = 0);							//	���ü�����Կ���汾��

	string GetKey();
	bool ExistKey();
	void Encrpyt(void *pInBuf, void *pOutBuf, int nSize);			// ��������

	void Decrpyt(void *pInBuf, void *pOutBuf, int nSize);			// ��������

private:
	//AesCtx m_context;
	string m_strKey;
	AES_KEY	m_encryptKey;
	AES_KEY	m_decryptKey;
};

