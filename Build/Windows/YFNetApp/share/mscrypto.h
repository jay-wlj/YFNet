#ifndef _MY_CryptTool_H_
#define _MY_CryptTool_H_

#include <wincrypt.h>
#include "../share/rijndael.h"
#include "..\share\StringCode.h"

class CryptTool
{
public:
//////////////////////////////////////////////////////////////////////////
// 	加密、解密函数，使用MS_ENHANCED_PROV模式
// 
// 	参数：
//  algID       加解密算法，MD2、MD5、SHA、MAC、HMAC、SSL3_SHAMD5
//                          RSA、RC2/4、DES、3DES_112、3DES
// 	pbKey       DES密钥
// 	cbKeyLen    pbKey字节长度
// 	pbData      要加密、解密的数据
// 	cbDataLen   pbData字节长度
// 	pbBuf       输出缓冲区，输出加密后、解密后的数据，可以为NULL
// 	pcbBufLen   pbBuf字节长度
// 	            当pbBuf不为NULL时，返回实际复制到pbBuf的字节长度
// 	            当pbBuf为NULL时，返回需要的pbBuf字节长度
// 	bIsDecrypt  为TRUE时执行解密操作，否则执行加密操作
// 
// 	返回：成功返回TRUE，否则返回FALSE

	static BOOL Crypto_KeyEncDec(__in ALG_ID algID,
		__in const char* pbKey,
		__in DWORD cbKeyLen,
		__in const BYTE* pbData,
		__in DWORD cbDataLen,
		__out_opt BYTE* pbBuf,
		__inout DWORD* pcbBufLen,
		__in_opt BOOL bIsDecrypt = FALSE)
	{
		//结构参考MSDN - CryptImportKey
		struct
		{
			BLOBHEADER hdr;
			DWORD cbKeySize;
			BYTE rgbKeyData[24];
		} keyBlob;

		keyBlob.hdr.bType = PLAINTEXTKEYBLOB;
		keyBlob.hdr.bVersion = CUR_BLOB_VERSION;
		keyBlob.hdr.reserved = 0;
		keyBlob.hdr.aiKeyAlg = algID;
		switch(algID)
		{
		case CALG_DES:
			keyBlob.cbKeySize = 8;
			break;
		case CALG_3DES_112:
			keyBlob.cbKeySize = 16;
			break;
		case CALG_3DES:
			keyBlob.cbKeySize = 24;
			break;
		default:
			break;
		}
		ZeroMemory(keyBlob.rgbKeyData, keyBlob.cbKeySize);
		CopyMemory(keyBlob.rgbKeyData, pbKey, cbKeyLen > keyBlob.cbKeySize ? keyBlob.cbKeySize : cbKeyLen);

		HCRYPTPROV hProv;
		if (!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
		{
			if(GetLastError() == 0x80090016)
			{
				// 重建密钥集
				CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_DELETEKEYSET);
				CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET);
				if (!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}

		HCRYPTKEY hKey;
		if (!CryptImportKey(hProv, (BYTE*)&keyBlob, sizeof(keyBlob), 0, 0, &hKey))
		{
			CryptReleaseContext(hProv, 0);
			return FALSE;
		}

		// 默认的CBC模式需要一个分组表示padding
		DWORD dwMode = CRYPT_MODE_ECB; 
		if(!CryptSetKeyParam(hKey, KP_MODE, (BYTE*)(&dwMode), 0))
		{
			CryptDestroyKey(hKey);
			CryptReleaseContext(hProv, 0);
			return FALSE;
		}

		// 	// 设置加密PADDING
		// 	dwMode = ZERO_PADDING; 
		// 	if(!CryptSetKeyParam(hKey, KP_PADDING, (BYTE*)(&dwMode), 0))
		// 	{
		// 		CryptDestroyKey(hKey);
		// 		CryptReleaseContext(hProv, 0);
		// 		return FALSE;
		// 	}

		BOOL bRet = FALSE;
		const DWORD cbBlock = 1024;    //分组加密、解密，每组1024字节
		BYTE block[cbBlock];
		DWORD dwBlock, dwOut = 0;
		BOOL bEOF;
		for(DWORD i=0; i<cbDataLen; i+=cbBlock)
		{
			bEOF = cbDataLen - i <= cbBlock;    //是否为最后一组
			dwBlock = bEOF ? cbDataLen - i : cbBlock;

			CopyMemory(block, pbData + i, dwBlock);
			if (bIsDecrypt)
			{
				bRet = CryptDecrypt(hKey, NULL, bEOF, 0, block, &dwBlock);
			}
			else
			{
				bRet = CryptEncrypt(hKey, NULL, bEOF, 0, block, &dwBlock, cbBlock);
			}

			if (!bRet)
			{
				break;
			}

			if(pbBuf)
			{
				if(dwOut + dwBlock > *pcbBufLen)
				{
					bRet = FALSE;
					break;
				} 
				else
				{
					CopyMemory(pbBuf + dwOut, block, dwBlock);
					dwOut += dwBlock;
				}
			} 
			else    //当pbBuf为NULL时，仅计算需要多大的pbBuf
			{
				dwOut += dwBlock;
			}
		}

		*pcbBufLen = dwOut;
		CryptDestroyKey(hKey);
		CryptReleaseContext(hProv, 0);
		return bRet;
	}

	static BOOL GetHash(const BYTE *pbData, __int64 dwDataLen, ALG_ID algId, LPSTR pszHash)
	{
		HCRYPTPROV hProv;
		if(!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return FALSE;

		HCRYPTHASH hHash;
		if(!CryptCreateHash(hProv, algId, 0, 0, &hHash)) //Alg Id:CALG_MD5,CALG_SHA
		{
			CryptReleaseContext(hProv, 0);
			return FALSE;
		}

		unsigned __int64 dwDataToRead = dwDataLen;
		while(dwDataToRead)
		{
			DWORD dwReadLen = 4096000000;
			if(dwReadLen > dwDataToRead) dwReadLen = (DWORD)dwDataToRead;
			if(!CryptHashData(hHash, pbData, dwReadLen, 0))
			{
				CryptDestroyHash(hHash);
				CryptReleaseContext(hProv, 0);
				return FALSE;
			}
			dwDataToRead -= dwReadLen;
			pbData += dwReadLen;
		}

		DWORD dwSize;
		DWORD dwLen = sizeof(DWORD);
		CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)(&dwSize), &dwLen, 0);

		BYTE* pHash = new BYTE[dwSize];
		dwLen = dwSize;
		CryptGetHashParam(hHash, HP_HASHVAL, pHash, &dwLen, 0);

		pszHash[0] = 0;
		char szTemp[3];
		for(DWORD i=0; i<dwLen; i++)
		{
			wsprintfA(szTemp, "%02X", pHash[i]);
			lstrcatA(pszHash, szTemp);
		}
		delete[] pHash;

		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return TRUE;
	}

	static BOOL GetMD5(const BYTE *pbData, __int64 dwDataLen, char *pRet)
	{
		if(pRet == NULL) return FALSE;
		char szMD5[60] = {0};
		if(!GetHash(pbData, dwDataLen, CALG_MD5, szMD5)) return FALSE;
		memcpy(pRet, szMD5, 60);
		return TRUE;
	}

	static BOOL GetFileMD5(TCHAR *path, char *szMD5)
	{
		HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwHiSize;
			DWORD dwLoSize = GetFileSize(hFile, &dwHiSize);
			unsigned __int64 dwFileSize = dwHiSize;
			dwFileSize <<= 32;
			dwFileSize += dwLoSize;
			HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, dwHiSize, dwLoSize, NULL);
			if(hFileMapping)
			{
				BYTE *pByte = (BYTE*)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
				if(pByte)
				{
					GetMD5(pByte, dwFileSize, szMD5);
					UnmapViewOfFile(pByte);
				}
				CloseHandle(hFileMapping);
			}
			CloseHandle(hFile);
			return TRUE;
		}
		return FALSE;
	}
	static CString GetMD5(LPCTSTR lpData)
	{
		CString strRet;
		if(lpData == NULL)		return strRet;
		int nLen = 2*_tcslen(lpData) + 1;
		char *pData = new char[nLen+1];
		memset(pData, 0, sizeof(char)*(nLen+1));
		WideCharToMultiByte(CP_ACP, 0, lpData, _tcslen(lpData), pData, (nLen+1), NULL, NULL);
		char szMD5[MAX_PATH] = {0};
		GetMD5((BYTE *)pData, strlen(pData), szMD5);
		strRet = szMD5;
		delete []pData;
		return strRet;
	}
	static void RijndaelBase64Decode(const char *pKey, const char *pIV, char *pIn, LPBYTE pOut)
	{
		std::string str = Base64::base64_decode(std::string((char*)pIn));
		CRijndael aes;
		aes.MakeKey(pKey, pKey);
		int len = str.length();
		int blockSize = aes.GetBlockSize();
		int k = len%blockSize;
		char *pBuff = (char*)str.c_str();
		BOOL bNewMem = FALSE;
		if(k >= 0)
		{
			len += blockSize - k;
			pBuff = new char[len];
			memset(pBuff, 0, len);
			memcpy(pBuff, str.c_str(), str.length());
			bNewMem = TRUE;
		}
		aes.Decrypt(pBuff, (char*)pOut, len, CRijndael::CBC);
		if(bNewMem)
		{
			delete []pBuff;
		}
	}
	static void RijndaelBase64Encode(const char *pKey, const char *pIV, char *pIn, std::string &strOut)
	{
		std::string str = pIn;
		CRijndael aes;
		aes.MakeKey(pKey, pKey);
		int len = str.length();
		int blockSize = aes.GetBlockSize();
		int k = len%blockSize;
		char *pBuff = (char*)str.c_str();
		BOOL bNewMem = FALSE;
		char *pTemp = NULL;
		if(k >= 0) //hl ，若数据是blocksize的整数也应该能加密 
		{
			len += blockSize - k;
			pBuff = new char[len];
			memset(pBuff, 0, len);
			memcpy(pBuff, str.c_str(), str.length());
			pTemp = new char[len];
			memset(pTemp, 0, len);
			bNewMem = TRUE;
		}
		aes.Encrypt(pBuff, (char*)pTemp, len, CRijndael::CBC);
		strOut = Base64::base64_encode((char*)pTemp, len);
		if(bNewMem)
		{
			delete []pBuff;
			delete []pTemp;
		}
	}
// 	static CString GetHash()
// 	{
// 		CStringA strTime;
// 		strTime.Format("%lld",time(0));
// 		BYTE *pByte = new BYTE[strTime.GetLength()+1];
// 		memcpy(pByte,strTime,strTime.GetLength());
// 		char buf[60]={0};
// 		GetMD5(pByte,strTime.GetLength(),buf);
// 		delete[]pByte;
// 		return CString(buf);
// 	}
};

#endif
