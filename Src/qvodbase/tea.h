#ifndef TEA_H
#define TEA_H


class Tea {
public:
	Tea(){}
	~Tea(){}
public:

	//for QvodNet 
	void encrypt(unsigned char *pData,int len,bool bUsingPublicKey = false,bool bLimitLen = true);
	void decrypt(unsigned char *pData,int len,bool bUsingPublicKey = false,bool bLimitLen = true);
	void SetKey(unsigned char *key);
	void RetKey();

	//for Default IP list
	void Encrypt(unsigned char *pData,int len);
	void Decrypt(unsigned char *pData,int len);

	//for Radar
	void EncryptTask(unsigned char *pData,int len,char *pKey);
	void DecryptTask(unsigned char *pData,int len,char *pKey);
private:
	unsigned char m_szKey[16];
};

#endif/*TEA_H*/