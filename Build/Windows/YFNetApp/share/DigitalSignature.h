// DigitalSignature.cpp : 定义控制台应用程序的入口点。
//
#ifndef	DIGITALSIGNATURE_H
#define DIGITALSIGNATURE_H

#include "stdafx.h"
#include <Wintrust.h>
#include <Softpub.h>
#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)


#define  verified_OK       1
#define  verified_UNKOWN  -1
#define  verified_FAILED   0
typedef struct _SIGNATURE
{
	short bSigned;								//是否签名
	LPWSTR lpszSubjectName;			//签名名称
	SYSTEMTIME timestamp;				//签名时间
	_SIGNATURE()
	{
		memset(this, 0, sizeof(SIGNATURE));
	}
}SIGNATURE;

static short VerifyEmbeddedSignature(LPCWSTR pwszSourceFile)
{
	LONG lStatus = -1 ;

	// Initialize the WINTRUST_FILE_INFO structure.

	WINTRUST_FILE_INFO FileData;
	memset(&FileData, 0, sizeof(FileData));
	FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.pcwszFilePath = pwszSourceFile;
	FileData.hFile = NULL;
	FileData.pgKnownSubject = NULL;

	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	WINTRUST_DATA WinTrustData;

	// Initialize the WinVerifyTrust input data structure.

	// Default all fields to 0.
	memset(&WinTrustData, 0, sizeof(WinTrustData));

	WinTrustData.cbStruct = sizeof(WinTrustData);

	// Use default code signing EKU.
	WinTrustData.pPolicyCallbackData = NULL;

	// No data to pass to SIP.
	WinTrustData.pSIPClientData = NULL;

	// Disable WVT UI.
	WinTrustData.dwUIChoice = WTD_UI_NONE;

	// No revocation checking.
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;

	// Verify an embedded signature on a file.
	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;

	// Default verification.
	WinTrustData.dwStateAction = 0;

	// Not applicable for default verification of embedded signature.
	WinTrustData.hWVTStateData = NULL;

	// Not used.
	WinTrustData.pwszURLReference = NULL;

	// Default.
	WinTrustData.dwProvFlags = 0;

	// This is not applicable if there is no UI because it changes
	// the UI to accommodate running applications instead of
	// installing applications.
	// WinTrustData.dwUIContext = 0;

	// Set pFile.
	WinTrustData.pFile = &FileData;



	// WinVerifyTrust verifies signatures as specified by the GUID
	// and Wintrust_Data.
	lStatus = WinVerifyTrust(
		NULL,
		&WVTPolicyGUID,
		&WinTrustData);

// 	TRUST_E_SUBJECT_NOT_TRUSTED
// 		The subject failed the specified verification action. Most trust providers return a more detailed error code that describes the reason for the failure.
// 	TRUST_E_PROVIDER_UNKNOWN
// 		The trust provider is not recognized on this system.
// 	TRUST_E_ACTION_UNKNOWN
// 		The trust provider does not support the specified action.
// 	TRUST_E_SUBJECT_FORM_UNKNOWN
// 		The trust provider does not support the form specified for the subject.


// 	if(lStatus==ERROR_SUCCESS )
// 	{
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
	switch( lStatus )
	{
	case ERROR_SUCCESS:
		{
			return verified_OK ;
		}
	case  TRUST_E_SUBJECT_NOT_TRUSTED :
		{
			//The subject failed the specified verification action. Most trust providers return a more detailed error code that describes the reason for the failure.
			return verified_UNKOWN;
		}
	case TRUST_E_PROVIDER_UNKNOWN :
		{
			//The trust provider is not recognized on this system.
			return verified_UNKOWN ;
		}
	case TRUST_E_ACTION_UNKNOWN :
		{
			//The trust verification action specified is not supported by the specified trust provider.
			return verified_UNKOWN ;
		}
	case  TRUST_E_SUBJECT_FORM_UNKNOWN :
		{
			//The form specified for the subject is not one supported or known by the specified trust provider.
			return verified_UNKOWN ;
		}
	case TRUST_E_NOSIGNATURE :
		{
			//  No signature was present in the subject.
			return verified_FAILED ;
		}
	case TRUST_E_BAD_DIGEST :
		{
			//  The digital signature of the object did not verify.
			return verified_FAILED ;
		}
	case  TRUST_E_SYSTEM_ERROR :
		{
			//  A system-level error occurred while verifying trust.
			return verified_UNKOWN ;
		}
	case TRUST_E_EXPLICIT_DISTRUST :
		{
			//  The certificate was explicitly marked as untrusted by the user.
			return verified_UNKOWN ;
		}
	case CERT_E_EXPIRED:
		{
			//  A required certificate is not within its validity period
			//  when verifying against the current system clock or the timestamp in the signed file.
			return verified_OK ;
		}
	}

	return verified_UNKOWN;
}

static BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext,SIGNATURE & signature)
{
	DWORD dwData;
	// Get Subject name size.
	if (!(dwData = CertGetNameString(pCertContext,
		CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, NULL, 0)))
	{
		return FALSE;
	}

	// Allocate memory for subject name.
	signature.lpszSubjectName = (LPWSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
	if (!signature.lpszSubjectName)
	{
		return FALSE;
	}

	// Get subject name.
	if (!(CertGetNameString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE,
				0, NULL, (LPTSTR)signature.lpszSubjectName, dwData)))
	{
		return FALSE;
	}

	if (signature.lpszSubjectName == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

static LPWSTR AllocateAndCopyWideString(LPCWSTR inputString)
{
	LPWSTR outputString = NULL;

	outputString = (LPWSTR)LocalAlloc(LPTR,
		(wcslen(inputString) + 1) * sizeof(WCHAR));
	if (outputString != NULL)
	{
		lstrcpyW(outputString, inputString);
	}
	return outputString;
}


static BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME *st)
{
	BOOL fResult;
	FILETIME lft, ft;
	DWORD dwData;
	BOOL fReturn = FALSE;

	// Loop through authenticated attributes and find
	// szOID_RSA_signingTime OID.
	for (DWORD n = 0; n < pSignerInfo->AuthAttrs.cAttr; n++)
	{
		if (lstrcmpA(szOID_RSA_signingTime,
			pSignerInfo->AuthAttrs.rgAttr[n].pszObjId) == 0)
		{
			// Decode and get FILETIME structure.
			dwData = sizeof(ft);
			fResult = CryptDecodeObject(ENCODING,
				szOID_RSA_signingTime,
				pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
				pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
				0,
				(PVOID)&ft,
				&dwData);
			if (!fResult)
			{
				_tprintf(_T("CryptDecodeObject failed with %x\n"),
					GetLastError());
				break;
			}

			// Convert to local time.
			FileTimeToLocalFileTime(&ft, &lft);
			FileTimeToSystemTime(&lft, st);

			fReturn = TRUE;

			break; // Break from for loop.

		} //lstrcmp szOID_RSA_signingTime
	} // for

	return fReturn;
}

static BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo, PCMSG_SIGNER_INFO *pCounterSignerInfo)
{
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fReturn = FALSE;
	BOOL fResult;
	DWORD dwSize;

	__try
	{
		*pCounterSignerInfo = NULL;

		// Loop through unathenticated attributes for
		// szOID_RSA_counterSign OID.
		for (DWORD n = 0; n < pSignerInfo->UnauthAttrs.cAttr; n++)
		{
			if (lstrcmpA(pSignerInfo->UnauthAttrs.rgAttr[n].pszObjId,
				szOID_RSA_counterSign) == 0)
			{
				// Get size of CMSG_SIGNER_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					PKCS7_SIGNER_INFO,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					NULL,
					&dwSize);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}

				// Allocate memory for CMSG_SIGNER_INFO.
				*pCounterSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSize);
				if (!*pCounterSignerInfo)
				{
					_tprintf(_T("Unable to allocate memory for timestamp info.\n"));
					__leave;
				}

				// Decode and get CMSG_SIGNER_INFO structure
				// for timestamp certificate.
				fResult = CryptDecodeObject(ENCODING,
					PKCS7_SIGNER_INFO,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					(PVOID)*pCounterSignerInfo,
					&dwSize);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}

				fReturn = TRUE;

				break; // Break from for loop.
			}
		}
	}
	__finally
	{
		// Clean up.
		if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
	}

	return fReturn;
}


//获取数字签名信息
static int GetSignatrueInfo(LPCWSTR szFileName,SIGNATURE &signature)
{
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fResult;
	DWORD dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	PCMSG_SIGNER_INFO pCounterSignerInfo = NULL;
	DWORD dwSignerInfo;
	CERT_INFO CertInfo;

	ZeroMemory(&signature,sizeof(SIGNATURE));


	__try
	{

		signature.bSigned=VerifyEmbeddedSignature(szFileName);//////////////////////////////判断签名是否正常


		// Get message handle and store handle from the signed file.
		fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
			szFileName,
			CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
			CERT_QUERY_FORMAT_FLAG_BINARY,
			0,
			&dwEncoding,
			&dwContentType,
			&dwFormatType,
			&hStore,
			&hMsg,
			NULL);
		if (!fResult)
		{
//			_tprintf(_T("CryptQueryObject failed with %x\n"), GetLastError());
			__leave;
		}

		// Get signer information size.
		fResult = CryptMsgGetParam(hMsg,
			CMSG_SIGNER_INFO_PARAM,
			0,
			NULL,
			&dwSignerInfo);
		if (!fResult)
		{
//			_tprintf(_T("CryptMsgGetParam failed with %x\n"), GetLastError());
			__leave;
		}

		// Allocate memory for signer information.
		pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
		if (!pSignerInfo)
		{
//			_tprintf(_T("Unable to allocate memory for Signer Info.\n"));
			__leave;
		}

		// Get Signer Information.
		fResult = CryptMsgGetParam(hMsg,
			CMSG_SIGNER_INFO_PARAM,
			0,
			(PVOID)pSignerInfo,
			&dwSignerInfo);
		if (!fResult)
		{
//			_tprintf(_T("CryptMsgGetParam failed with %x\n"), GetLastError());
			__leave;
		}
		CertInfo.Issuer = pSignerInfo->Issuer;
		CertInfo.SerialNumber = pSignerInfo->SerialNumber;

		pCertContext = CertFindCertificateInStore(hStore,
			ENCODING,
			0,
			CERT_FIND_SUBJECT_CERT,
			(PVOID)&CertInfo,
			NULL);
		if (!pCertContext)
		{
//			_tprintf(_T("CertFindCertificateInStore failed with %x\n"),GetLastError());
			__leave;
		}


		//  Signer certificate information.
		PrintCertificateInfo(pCertContext,signature);

		// Get the timestamp certificate signerinfo structure.
		if (GetTimeStampSignerInfo(pSignerInfo, &pCounterSignerInfo))
		{
			// Find Date of timestamp.
			GetDateOfTimeStamp(pCounterSignerInfo, &signature.timestamp);
		}
	}
	__finally
	{
		// Clean up.

		if (pSignerInfo != NULL) LocalFree(pSignerInfo);
		if (pCounterSignerInfo != NULL) LocalFree(pCounterSignerInfo);
		if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
		if (hStore != NULL) CertCloseStore(hStore, 0);
		if (hMsg != NULL) CryptMsgClose(hMsg);
	}
	return 0;
}

static BOOL IsNetBarVer(LPCTSTR szExeName)
{
	BOOL bRet = FALSE;
	DWORD  dwSize  =  GetFileVersionInfoSize(szExeName,NULL);
	if (dwSize > 0)
	{
		TCHAR* pInfo = new TCHAR[dwSize];
		if (GetFileVersionInfo(szExeName, 0, dwSize, pInfo))
		{
			TCHAR*   pVerValue   =   NULL;
			UINT   nSize   =   0;
			VerQueryValue(pInfo,_T("\\VarFileInfo\\Translation"), (LPVOID*)&pVerValue,&nSize);
			CString   strSubBlock,strTranslation,strTemp;
			strTemp.Format(_T("000%x"),*((unsigned short int *)pVerValue));
			strTranslation   =   strTemp.Right(4);
			strTemp.Format(_T("000%x"),*((unsigned short int*)&pVerValue[1]));
			strTranslation   +=   strTemp.Right(4);
			//080404b0为中文，040904E4为英文

			//文件描述
			strSubBlock.Format(_T("\\StringFileInfo\\%s\\FileDescription"),strTranslation);
			VerQueryValue(pInfo,strSubBlock.GetBufferSetLength(256),(LPVOID*)&pVerValue,&nSize);
			if (_tcscmp(pVerValue, _T("QvodNbTerminal")) == 0)
			{
				SIGNATURE signature;
				USES_CONVERSION;
				GetSignatrueInfo((LPWSTR)T2CW(szExeName),signature);
				TCHAR* pQvod = _T("Shenzhen QVOD Technology Co.,Ltd");
				if((signature.bSigned == verified_OK || signature.bSigned == verified_UNKOWN)
					&& wcscmp(T2CW(pQvod),signature.lpszSubjectName) == 0)
				{
					bRet = TRUE;
				}
			}
		}
		delete[] pInfo;
	}
	return bRet;
}
#endif
