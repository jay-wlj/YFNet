#pragma once
//#include "EfAes.H"
#include "openssl/aes.h"
#include "qvod_string.h"

class CAESData
{
public:
	CAESData(void);
	~CAESData(void);

	void SetKey(unsigned char key[16]);												// 采用ECB模式加密
	void SetKey(const string& strKey, int nVer = 0);							//	设置加密密钥及版本号

	string GetKey();
	bool ExistKey();
	void Encrpyt(void *pInBuf, void *pOutBuf, int nSize);			// 加密数据

	void Decrpyt(void *pInBuf, void *pOutBuf, int nSize);			// 解密数据

private:
	//AesCtx m_context;
	string m_strKey;
	AES_KEY	m_encryptKey;
	AES_KEY	m_decryptKey;
};

