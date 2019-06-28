//
// 图像 网络 工具函数类
//
//////////////////////////////////////////////////////////////////////////

#ifndef _MY_TOOL_H_
#define _MY_TOOL_H_

#include "xmlparse.h"
#include "tools.h"
#include "mscrypto.h"
#include "function.h"
#include "Registry.h"
#include "../Common/version.h"

#ifdef USES_GZIP
#include "../share/zlib.h"
#pragma comment(lib, "../share/zlib.lib")
#include <vector>
#endif

class MyTool
{
public:
	static BOOL WINAPI BitmapToFile(HBITMAP hBitmap,  LPCTSTR lpFile)
	{
		HDC                hDC;
		int                iBits;
		WORD               wBitCount;
		BITMAP             Bitmap;
		BITMAPFILEHEADER   bmfHdr;
		BITMAPINFOHEADER   bi;
		LPBITMAPINFOHEADER lpbi;
		HPALETTE           hOldPal = NULL;
		HANDLE             fh, hDib, hPal;
		DWORD              dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;

		hDC = CreateDC(_T("DISPLAY"), 0, 0, 0);
		iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
		DeleteDC(hDC);

		if(iBits <= 1)       wBitCount =  1;
		else if(iBits <=  4) wBitCount =  4;
		else if(iBits <=  8) wBitCount =  8;
		else if(iBits <= 24) wBitCount = 24;
		else                 wBitCount = 24;

		if(wBitCount <= 8) dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
		bi.biSize        = sizeof(BITMAPINFOHEADER);
		bi.biWidth       = Bitmap.bmWidth;
		bi.biHeight      = Bitmap.bmHeight;
		bi.biPlanes      = 1;
		bi.biBitCount    = wBitCount;
		bi.biCompression = BI_RGB;
		bi.biSizeImage   = bi.biXPelsPerMeter = bi.biYPelsPerMeter = 0;
		bi.biClrUsed     = bi.biClrImportant = 0;
		dwBmBitsSize     = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

		if(!(hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER))))
			return FALSE;

		*(lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib)) = bi;

		if((hPal = GetStockObject(DEFAULT_PALETTE)))
		{
			hDC = GetDC(0);
			hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
			RealizePalette(hDC);
		}
		GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

		if(hOldPal)
		{
			SelectPalette(hDC, hOldPal, TRUE);
			RealizePalette(hDC);
			ReleaseDC(0, hDC);
		}

		fh = CreateFile(lpFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
		if(fh != INVALID_HANDLE_VALUE)
		{
			bmfHdr.bfType = 0x4D42;
			dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
			bmfHdr.bfSize = dwDIBSize;
			bmfHdr.bfReserved1 = bmfHdr.bfReserved2 = 0;
			bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
			WriteFile(fh, &bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
			WriteFile(fh, lpbi, dwDIBSize, &dwWritten, NULL);
			FlushFileBuffers(fh);
			CloseHandle(fh);
		}

		GlobalUnlock(hDib);
		GlobalFree(hDib);
		return TRUE;
	}

	static long Exponential(long nBase) // 计算数的比特位数,不大于2的n次方
	{
		if(nBase == 0) return 0;
		long nExp = 0;
		nBase--;
		while(nBase>>nExp)
		{
			nExp++;
		}
		return 1<<nExp;
	}

	static BOOL Gif2Bmp(CString &path_gif, CString &path_bmp)
	{
		return FALSE;
	}

	static CString Uint64ToStr(DWORD nFileSizeLow, DWORD nFileSizeHigh, long &nDivided)
	{
		double fData = nFileSizeHigh;
		fData *= 0xffffffff;
		fData += nFileSizeHigh;
		fData += nFileSizeLow;
		nDivided = 0;
		while(fData>1024)
		{
			fData /= 1024;
			nDivided++;
		}
		CString str, strExt;
		switch(nDivided)
		{
		case 0:
			strExt = _T(" Bytes");
			break;
		case 1:
			strExt = _T(" KB");
			break;
		case 2:
			strExt = _T(" MB");
			break;
		case 3:
			strExt = _T(" GB");
			break;
		case 4:
			strExt = _T(" TB");
			break;
		case 5:
			strExt = _T(" PB");
			break;
		case 6:
			strExt = _T(" EB");
			break;
		case 7:
			strExt = _T(" ZB");
			break;
		case 8:
			strExt = _T(" YB");
			break;
		}
		str.Format(_T("%f"), fData);
		long nData = (long)fData;
		if(nData >= 100)
		{
			str.Format(_T("%d"), nData);
		}
		else if(nData >= 10)
		{
			str.Format(_T("%.1f"), fData);
		}
		else if(nData)
		{
			if(nDivided >= 1)
			{
				str.Format(_T("%.2f"), fData);
			}
			else
			{
				str.Format(_T("%d"), (long)fData);
			}
		}
		else
		{
			str = _T("0");
		}
		str = str + strExt;
		return str;
	}

	static CString Uint64ToStr(__int64 nSize, long &nDivided)
	{
		int nFileSizeLow = nSize & 0xffffffff;
		int nFileSizeHigh = (nSize >> 32) & 0xffffffff;
		return Uint64ToStr(nFileSizeLow, nFileSizeHigh, nDivided);
	}

	static CString Time2Str(DWORD second)
	{
		DWORD minute = second / 60;
		second %= 60;
		DWORD hour = minute / 60;
		minute %= 60;
		CString str;
		str.Format(_T("%2d:%02d:%02d"), hour, minute, second);
		return str;
	}

	static CString Time2Str_Short(DWORD second)
	{
		DWORD minute = second / 60;
		second %= 60;
		CString str;
		str.Format(_T("%d:%02d"), minute, second);
		return str;
	}

	static CString TimeToStr(DWORD second)
	{
		if(second >= 3600)
		{
			return MyTool::Time2Str(second);
		}
		else
		{
			return MyTool::Time2Str_Short(second);
		}
	}

	static CString DateTime2Str(const SYSTEMTIME & stime)
	{
		CString str;
		str.Format(_T("%hu-%02hu-%02hu %2hu:%02hu:%02hu"), stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute, stime.wSecond);
		return str;
	}

	static CString DateTime2Str_Short(const SYSTEMTIME & stime)
	{
		CString str;
		str.Format(_T("%2hu:%02hu:%02hu"), stime.wHour, stime.wMinute, stime.wSecond);
		return str;
	}

	static void Str2DateTime(LPCTSTR pStr, SYSTEMTIME & stime)
	{
		memset(&stime, 0, sizeof(SYSTEMTIME));
		if ( !pStr || !_tcslen(pStr) )
		{
			return;
		}
		swscanf_s(pStr, _T("%hu-%02hu-%02hu %2hu:%02hu:%02hu"), &stime.wYear, &stime.wMonth, &stime.wDay, &stime.wHour, &stime.wMinute, &stime.wSecond);
	}

	static BOOL IsLeapYear(long nYear) // 年份中是否有闰月
	{
		if(nYear%400 == 0) return TRUE;
		if(nYear%100 == 0) return FALSE;
		if(nYear%4 == 0) return TRUE;
		return FALSE;
	}

	static long DaysBetween(const SYSTEMTIME & stimeEnd, const SYSTEMTIME & stimeStart) // 计算两个时间的天数差
	{
		long nDay1 = 0, nDay2 = 0;
		long nMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		long i;
		for(i=0; i<stimeEnd.wMonth-1; i++)
		{
			nDay2 += nMonth[i];
			if(i==1)
			{
				if(IsLeapYear(stimeEnd.wYear)) nDay2++;
			}
		}
		for(i=0; i<stimeStart.wMonth-1; i++)
		{
			nDay1 += nMonth[i];
			if(i==1)
			{
				if(IsLeapYear(stimeStart.wYear)) nDay1++;
			}
		}
		long nDay = 0;
		for(i=stimeStart.wYear; i<stimeEnd.wYear; i++)
		{
			nDay += 365;
			if(IsLeapYear(i)) nDay++;
		}
		nDay += nDay2;
		nDay -= nDay1;
		nDay += stimeEnd.wDay;
		nDay -= stimeStart.wDay;
		return nDay;
	}

	static float HoursBetween(const SYSTEMTIME & stimeEnd, const SYSTEMTIME & stimeStart) // 计算两个时间的小时差
	{
		long nDay = DaysBetween(stimeEnd, stimeStart);
		float fHour = nDay * 24.0f;
		fHour += stimeEnd.wHour;
		fHour -= stimeStart.wHour;
		fHour += stimeEnd.wMinute / 60.0f;
		fHour -= stimeStart.wMinute / 60.0f;
		return fHour;
	}

	static BOOL GuidEqual(GUID *guid1, GUID *guid2)
	{
		if(guid1 == NULL) return FALSE;
		if(guid2 == NULL) return FALSE;
		return !memcmp(guid1, guid2, sizeof(GUID));
	}

	static CString GetDoMain(LPCTSTR pURL)
	{
		CString strURL = pURL;
		strURL.MakeLower();
		// 去协议头
		//strURL.TrimLeft(_T("http://"));
		if(strURL.Find(_T("http://")) == 0)
		{
			strURL = strURL.Right(strURL.GetLength() - 7);
		}

		// 去子页面
		int t = strURL.Find(_T("/"));
		if(t >= 0)
		{
			strURL = strURL.Left(t);
		}

		// 去端口号
		t = strURL.Find(_T(":"));
		if(t >= 0)
		{
			strURL = strURL.Left(t);
		}

		// TODO: 是否是纯IP
		int nRDot = strURL.ReverseFind('.');
		if(nRDot>0)
		{
			nRDot++;
			CString str = strURL.Right(strURL.GetLength() - nRDot);
			int t = _ttol(str);
			if(t > 0)
			{
				return strURL;
			}
		}

		CString strHead = strURL;
		const TCHAR szDomainExt[][8] = {
			// 二级后缀
			_T(".cn"),
			_T(".jp"),
			_T(".ru"),
			_T(".hk"),
			_T(".cc"),
			_T(".me"),
			_T(".la"),
			_T(".tw"),
			_T(".asia"),

			// 一级后缀
			_T(".com"),
			_T(".net"),
			_T(".info"),
			_T(".gov"),
			_T(".org"),
			_T(".biz"),
			_T(".edu"),
			_T(".tv")
		};
		for(long i=0; i<sizeof(szDomainExt)/sizeof(TCHAR[8]); i++)
		{
			strHead.TrimRight(szDomainExt[i]);
		}
		t = strHead.ReverseFind('.');
		if(t >= 0)
		{
			strURL = strURL.Right(strURL.GetLength()-t-1);
		}
		return strURL;
	}

	static long GetPictureFormat(const TCHAR *szPath, SIZE *pSize)
	{
		long nRet = 0;
		HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwSize = GetFileSize(hFile, NULL);
			if(dwSize == 0)
			{
				CloseHandle(hFile);
				return 0;
			}

			BYTE buff[16];
			DWORD dwRead, dwFlag = 0;
			ReadFile(hFile, &dwFlag, 3, &dwRead, NULL);
			if(dwRead<3)
			{
				CloseHandle(hFile);
				return 0;
			}

			if(pSize) pSize->cx = pSize->cy = 0;

			switch(dwFlag)
			{
			case 0x4e5089: // png
				ReadFile(hFile, buff, 15, &dwRead, NULL);
				if(dwRead < 15) return 0;
				nRet = 1;
				if(pSize)
				{
					dwFlag = 0;
					ReadFile(hFile, &dwFlag, 2, &dwRead, NULL);
					if(dwRead < 2) return 0;
					dwFlag = ((dwFlag&0xff)<<8) | ((dwFlag>>8)&0xff);
					pSize->cx = dwFlag;
					ReadFile(hFile, &dwFlag, 2, &dwRead, NULL);
					if(dwRead < 2) return 0;
					dwFlag = 0;
					ReadFile(hFile, &dwFlag, 2, &dwRead, NULL);
					if(dwRead < 2) return 0;
					dwFlag = ((dwFlag&0xff)<<8) | ((dwFlag>>8)&0xff);
					pSize->cy = dwFlag;
				}
				break;
			case 0x464947: // gif
				ReadFile(hFile, buff, 3, &dwRead, NULL);
				if(dwRead < 3) return 0;
				nRet = 2;
				if(pSize)
				{
					ReadFile(hFile, &pSize->cx, 2, &dwRead, NULL);
					if(dwRead < 2) return 0;
					ReadFile(hFile, &pSize->cy, 2, &dwRead, NULL);
					if(dwRead < 2) return 0;
				}
				break;
			case 0x535746: // swf
//				ReadFile(hFile, buff, 5, &dwRead, NULL);
//				nRet = 3;
//				{
//					long nBits, totalBits, totalBytes;
//					BYTE byteTemp = 0;
//					ReadFile(hFile, &byteTemp, 1, &dwRead, NULL);
//					nBits = byteTemp>>3;
//					totalBits = nBits * 4;
//				}
//            mStream.Read (5)
//
//            binData = mStream.Read(1) ; 读一个字节
//            sConv = Num2Str(AscB(binData), 2, 8)
//            nBits = Str2Num(Left(sConv, 5), 2)  ; 取高位 5 bit
//            sConv = Mid(sConv, 6)               ; 保留低位 3 bit
//            While (Len(sConv) < nBits * 4)
//                binData = mStream.Read(1)
//                sConv = sConv & Num2Str(AscB(binData), 2, 8)
//            Wend
//            ret(0) = "SWF"
//            ret(1) = Int(Abs(Str2Num(Mid(sConv, 1 * nBits + 1, nBits), 2) - Str2Num(Mid(sConv, 0 * nBits + 1, nBits), 2)) / 20)
//            ret(2) = Int(Abs(Str2Num(Mid(sConv, 3 * nBits + 1, nBits), 2) - Str2Num(Mid(sConv, 2 * nBits + 1, nBits), 2)) / 20)
				break;
			case 0xffd8ff: // jpg
				while(1)
				{
					BYTE bTemp = 255;
					do
					{
						ReadFile(hFile, &bTemp, 1, &dwRead, NULL);
						if(dwRead < 1) return 0;
					}while((bTemp==255)&&(dwRead>0));

					if((bTemp > 191)&&(bTemp <196)) break;
					else
					{
						dwFlag = 0;
						ReadFile(hFile, &dwFlag, 2, &dwRead, NULL);
						if(dwRead < 2) return 0;
						dwFlag = ((dwFlag&0xff)<<8) | ((dwFlag>>8)&0xff);
						dwFlag -= 2;
						for(long i=0; i<(long)dwFlag; i++)
						{
							ReadFile(hFile, &bTemp, 1, &dwRead, NULL);
							if(dwRead < 1) return 0;
						}
					}

					do
					{
						ReadFile(hFile, &bTemp, 1, &dwRead, NULL);
						if(dwRead < 1) return 0;
					}while((bTemp<255)&&(dwRead>0));
				}

				ReadFile(hFile, buff, 3, &dwRead, NULL);
				if(dwRead < 3) return 0;
				nRet = 3;
				if(pSize)
				{
					dwFlag = 0;
					ReadFile(hFile, &dwFlag, 2, &dwRead, NULL);
					if(dwRead < 2) return 0;
					dwFlag = ((dwFlag&0xff)<<8) | ((dwFlag>>8)&0xff);
					pSize->cy = dwFlag;
					dwFlag = 0;
					ReadFile(hFile, &dwFlag, 2, &dwRead, NULL);
					if(dwRead < 2) return 0;
					dwFlag = ((dwFlag&0xff)<<8) | ((dwFlag>>8)&0xff);
					pSize->cx = dwFlag;
				}
				break;
			default:
				dwFlag &= 0xffff;
				if(dwFlag == 0x4d42)
				{
					ReadFile(hFile, buff, 15, &dwRead, NULL);
					if(dwRead < 15) return 0;
					nRet = 4;
					if(pSize)
					{
						ReadFile(hFile, &pSize->cx, 4, &dwRead, NULL);
						if(dwRead < 4) return 0;
						ReadFile(hFile, &pSize->cy, 4, &dwRead, NULL);
						if(dwRead < 4) return 0;
					}
				}
			}

			CloseHandle(hFile);
		}
		return nRet;
	}

	static void AssertPath(CString &strPath)
	{
		TCHAR *pStr = strPath.LockBuffer();
		TCHAR *pDir = _tcschr(pStr, '\\');
		if(pDir)
		{
			TCHAR *pNS = _tcschr(pDir, ':');
			if(pNS)
			{
				*pNS = 0;
			}
		}
		strPath.UnlockBuffer();
	}

	static void StringToHex(const CString &strHex, std::string &strData)
	{
		int nLen = strHex.GetLength();
		int dwLen = nLen/2;

		BYTE *pBuf = new BYTE[dwLen];

		for (int i=0; i<dwLen; i++)
		{
			CString str = strHex.Mid(2*i, 2);
			*(pBuf+i) = _tcstol(str, NULL, 16);
		}
		strData.clear();
		strData.assign((char*)pBuf, dwLen);
		SafeDeleteArray(pBuf);
	}

	static CString HexToString(BYTE *pData, DWORD dwLen)
	{
		CString strRet;
		char szTemp[3];
		BYTE *p = pData;
		for(DWORD i=0; i<dwLen; i++)
		{
			wsprintfA(szTemp, "%02X", *p++);
			strRet += szTemp;
		}
		return strRet;
	}

	//src:长度16的MD5码
	//dest:长度8的密码
	static void Encode(char *src, char *dest)
	{
		for (int i = 0; i < 8; i++)
		{
			unsigned int a = (src[i] + i);
			unsigned int b = (src[15-i] - i);
			unsigned int c = ((a*b+7)>>i)%16;
			sprintf(&(dest[i]),"%x",c);
		}
	}

	static CString GetPlayerVersion()
	{
		CString strVer;
		HKEY hKey = NULL;
		CRegistry reg(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\HunanTV"));
		if (reg.GetString(NULL, _T("version"), strVer))
		{
			TCHAR szPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szPath, MAX_PATH );
			PathRemoveFileSpec(szPath);
			CString strPlayPath = szPath;
			strPlayPath += _T("\\YFPlayer.exe");
			strVer = QVersion::Tool::GetFileVersionString(strPlayPath);
		}
		return strVer;
	}
	static CString GetUnionID()
	{
		CString strRet;
		HKEY hKey = NULL;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\HunanTV"), 0, KEY_READ, &hKey);
		if(hKey)
		{
			TCHAR szUnion[MAX_PATH] = {0};
			DWORD dwSize = MAX_PATH;
			DWORD dwType = REG_SZ;
			RegQueryValueEx(hKey, _T("unionid"), NULL, &dwType, (BYTE*)szUnion, &dwSize);
			RegCloseKey(hKey);

			//对联盟ID进行解密
			//解密处理
			std::string strUnionID;
			MyTool::StringToHex(szUnion, strUnionID);
			char szKey[] = "!MangoYunfan@";
			DWORD dwLen = 0;

			BOOL bRet = CryptTool::Crypto_KeyEncDec(CALG_DES, szKey, strlen(szKey), (BYTE*)strUnionID.c_str(), strUnionID.length(), NULL, &dwLen, TRUE);
			if(bRet && dwLen > 0)
			{
				BYTE *pBuff = new BYTE[dwLen];
				bRet = CryptTool::Crypto_KeyEncDec(CALG_DES, szKey, strlen(szKey), (BYTE*)strUnionID.c_str(), strUnionID.length(), pBuff, &dwLen, TRUE);
				if (bRet)
				{
					strUnionID.assign((char*)pBuff, dwLen);
					strRet = Fun::ANSIToUnicode(strUnionID);
				}
				SafeDeleteArray(pBuff);
			}
		}
		return strRet;
	}

#ifdef QVODID
	//ret : HRESULT == S_OK , success
	static HRESULT CreateGUID(CString &strUID)
	{
		HRESULT hResult = S_FALSE;
		strUID.Empty();
		GUID guID;
		TCHAR szToolAddKey[MAX_PATH] = {0};
		hResult = CoCreateGuid(&guID);
		if( S_OK != hResult)
		{
			return hResult;
		}
		int nResult = StringFromGUID2(guID,szToolAddKey,MAX_PATH);
		if( 0 < nResult )
		{
			strUID = szToolAddKey;
			hResult = S_OK;
		}
		else
		{
			hResult = S_FALSE;
		}
		return hResult;
	}

	static void CreatePureGUID(CString &strUID)
	{
		strUID.Empty();
		CreateGUID(strUID);
		strUID.Replace(_T("{"), _T(""));
		strUID.Replace(_T("}"), _T(""));
		strUID.Replace(_T("-"), _T(""));
	}
	static BOOL GetQvodUID(CString &strUID)
	{
		strUID.Empty();
		CRegistry regLCM( HKEY_LOCAL_MACHINE, _T(""));
		BOOL bOpenKey = FALSE;
		TCHAR szToolAddKey[MAX_PATH] = {0};
		bOpenKey = regLCM.GetString(_T("SOFTWARE\\QvodPlayer"),_T("UID"),szToolAddKey,MAX_PATH);
		if( !bOpenKey || !_tcslen(szToolAddKey) )
		{
			CString uid;
			CreateGUID(uid);
			if ( uid.GetLength() && regLCM.SetString(_T("SOFTWARE\\QvodPlayer"),_T("UID"),uid) )
			{
				strUID = uid;
				return TRUE;
			}
			return FALSE;
		}
		else
		{
			strUID = szToolAddKey;
			return TRUE;
		}
	}

	// 33字节 字符串表示
	static BOOL GetUIDString33(CString &strUID)
	{
		if ( !GetQvodUID(strUID) )
		{
			return FALSE;
		}
		strUID.Replace(_T("{"), _T(""));
		strUID.Replace(_T("}"), _T(""));
		strUID.Replace(_T("-"), _T(""));
		return TRUE;
	}

#endif
};

class FileTool
{
public:
	static void RemoveDirectoryFiles_(const TCHAR *szPath)
	{
		CString strPath;
		strPath.Format(_T("%s\\*"), szPath);
		WIN32_FIND_DATA fdata;
		HANDLE hFind = FindFirstFile(strPath, &fdata);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			DeleteDirFile_(szPath, &fdata);
			while(FindNextFile(hFind, &fdata))
			{
				DeleteDirFile_(szPath, &fdata);
			}
			FindClose(hFind);
			RemoveDirectory(szPath);
		}
	}

	static BOOL MoveDir(CString strSrcPath, CString strDstPath, LPCTSTR lpFilter = NULL)
	{
		// 创建目标文件夹
		CreateDirectory(strDstPath,NULL);

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile(strSrcPath + _T("\\*"), &FindFileData);
		if(hFind == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		else
		{
			do
			{
				CString strp = FindFileData.cFileName;
				CString strSrc = strSrcPath + _T("\\") + strp;
				CString strDst = strDstPath + _T("\\") + strp;
				if((strp.CompareNoCase(_T(".")) == 0)
					|| (strp.CompareNoCase(_T("..")) == 0))
				{
					continue;
				}

				if(lpFilter && _tcsstr(lpFilter, strp))
				{
					continue;
				}

				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					MoveDir(strSrc, strDst);
				}
				else
				{
					MoveFileEx(strSrc, strDst, /*MOVEFILE_REPLACE_EXISTING|*/MOVEFILE_COPY_ALLOWED);
				}

			}while(FindNextFile(hFind, &FindFileData));
			::FindClose(hFind);
			::RemoveDirectory(strSrcPath);
		}
		return TRUE;
	}

	static BOOL GetFileVerInfo( LPCTSTR pszFile, LPTSTR pszName, CString &strInfo/*[out]*/)
	{
		_ASSERT( pszName );
		DWORD dwHandle;
		DWORD size = ::GetFileVersionInfoSize( pszFile , &dwHandle );
		if( size == 0 )
			return FALSE;

		LPVOID lpData = new BYTE[size];
		if( lpData == NULL )
			return FALSE;
		BOOL ret = FALSE;

		if( ::GetFileVersionInfo( pszFile , dwHandle , size , lpData ) )
		{
			LPVOID pLanguage;
			UINT uLen;
			if( ::VerQueryValue( lpData , _T("\\VarFileInfo\\Translation"),		// read lang code page
				(LPVOID *)&pLanguage,&uLen) )
			{
				TCHAR szSubBlock[ 128 ];
				_stprintf( szSubBlock ,
					_T("\\StringFileInfo\\%04x%04x\\%s") ,
					LOWORD( *(DWORD *)pLanguage ) ,
					HIWORD( *(DWORD *)pLanguage ) ,
					pszName );

				LPTSTR pszValue;
				if( ::VerQueryValue( lpData , szSubBlock , (LPVOID *)&pszValue , &uLen ) )
				{
					strInfo = pszValue;
					ret = TRUE;
				}
			}
		}

		delete []lpData;
		return ret;
	}
private:
	static void DeleteDirFile_(const TCHAR *szPath, WIN32_FIND_DATA *pData)
	{
		if(pData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(lstrcmp(_T("."), pData->cFileName) == 0) return;
			if(lstrcmp(_T(".."), pData->cFileName) == 0) return;
			CString strPath;
			strPath.Format(_T("%s\\%s"), szPath, pData->cFileName);
			RemoveDirectoryFiles_(strPath);
		}
		else
		{
			CString strPath2;
			strPath2.Format(_T("%s\\%s"), szPath, pData->cFileName);
			DeleteFile(strPath2);
		}
	}
};


#include <WinInet.h>
#pragma comment(lib,"Wininet.lib")
class InternetTool
{
public:
	static BOOL IsConnected(CString lpTestUrl)
	{
		HINTERNET hInternet = InternetOpen(L"Example",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
		HINTERNET hNewInternet = InternetOpenUrl(hInternet, lpTestUrl,  NULL, 0, INTERNET_FLAG_RELOAD , 0);
		if ( NULL == hNewInternet )
		{
			InternetCloseHandle(hInternet);
			return FALSE;
		}
		InternetCloseHandle(hInternet);
		InternetCloseHandle(hNewInternet);
		return TRUE;
	}
};

/*
#include <GdiPlus.h>
#include "StringTool.h"
using Gdiplus::Rect;
using Gdiplus::RectF;
using Gdiplus::Image;
using Gdiplus::Bitmap;
using Gdiplus::Font;
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
class GdiPlusTool
{
public:
	//克隆一个新的Bitmap对象，不用时，需使用delete释放。
	static Gdiplus::Bitmap *CloneBtimap(Gdiplus::Bitmap *pBitmap)
	{
		Gdiplus::Bitmap* pCloneObj = NULL;
		if(pBitmap)
		{
			int nW = pBitmap->GetWidth();
			int nH = pBitmap->GetHeight();
			if(nW > 0 &&  nH> 0)
			{
				Gdiplus::Rect rt(0, 0, nW, nH);
				pCloneObj = new Gdiplus::Bitmap(rt.Width, rt.Height);
				Gdiplus::Graphics gra(pCloneObj);
				gra.DrawImage(pBitmap, rt);
			}
		}
		return pCloneObj;
	}

	static Bitmap* LoadFile(const CString& strFile)
	{
		Bitmap *pBitmap = NULL;
		CStringW wstrFile = Convert::ToWString(strFile);
		if(GetFileAttributesW(wstrFile) != INVALID_FILE_ATTRIBUTES)
		{
			Bitmap*	pTmp = Bitmap::FromFile(wstrFile);
			pBitmap = GdiPlusTool::CloneBtimap(pTmp);
			delete pTmp;
		}
		return pBitmap;
	}

	static int MeasureStringWidth(HWND hWnd, const CString& strTxt)
	{
		Gdiplus::Graphics gra(hWnd);
		Gdiplus::PointF origin;
		HDC hDC = GetDC(hWnd);
		HFONT hfont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
		RectF boundingBox;
		gra.MeasureString(Convert::ToWString(strTxt), -1, &Gdiplus::Font(hDC, hfont), origin, &boundingBox);
		ReleaseDC(hWnd, hDC);
		return (long)boundingBox.Width;
	}

	static BOOL SaveBmp(const CString& path, Bitmap* pBmp)
	{
		if (pBmp)
		{
			CLSID clsid;
			if(GetEncoderClsid(L"image/bmp", &clsid))
			{
				if(pBmp->Save(Convert::ToWString(path), &clsid, 0) == Ok)
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	static BOOL SaveJpgByQuality(const CString& path, Bitmap *pBmp,long lQuality = 90)
	{
		BOOL bRet = FALSE;
		if ( pBmp)
		{
			CLSID clsid;
			Gdiplus::EncoderParameters eps;
			eps.Count = 1;
			eps.Parameter[0].Guid = Gdiplus::EncoderQuality;
			eps.Parameter[0].NumberOfValues = 1;
			eps.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			eps.Parameter[0].Value = &lQuality;		
			if(GetEncoderClsid(L"image/jpeg", &clsid))
			{
				bRet = pBmp->Save(Convert::ToWString(path), &clsid, &eps) == Gdiplus::Ok;
			}
		}
		return bRet;
	}

	static BOOL SaveJpgByCompression(const CString& path, Bitmap *pBmp,long Compression = 0)
	{
		BOOL bRet = FALSE;
		if ( pBmp)
		{
			CLSID clsid;
			Gdiplus::EncoderParameters eps;
			eps.Count = 1;
			eps.Parameter[0].Guid = Gdiplus::EncoderCompression;
			eps.Parameter[0].NumberOfValues = 1;
			eps.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			eps.Parameter[0].Value = &Compression;
			if(GetEncoderClsid(L"image/jpeg", &clsid))
			{
				bRet = pBmp->Save(Convert::ToWString(path), &clsid, &eps) == Gdiplus::Ok;
			}
		}
		return bRet;
	}

	static BOOL GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		if(format == NULL) return FALSE;
		UINT  numEncoders = 0;        
		UINT  size = 0;        
		Gdiplus::GetImageEncodersSize(&numEncoders, &size);
		if(size == 0) return FALSE;  

		Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(new char[size]);
		if(pImageCodecInfo == NULL) return FALSE;  

		Gdiplus::GetImageEncoders(numEncoders, size, pImageCodecInfo);
		for(UINT j = 0; j < numEncoders; ++j)
		{
			if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				delete []pImageCodecInfo;
				return TRUE;  
			}
		}
		delete []pImageCodecInfo;
		return FALSE; 
	}
};
*/

#ifdef USE_QHTP

#include "..\share\QhtpEn_De.h"

class En_DeFile
{//XML文件加解密
public:
	static BOOL EncodeFile(LPCTSTR lpFileName, void* const bufIn, UINT nBufInSize)
	{
		const UINT pPwd[] = {0x73D35BE6}; //暂时放在这里
		const UINT pwdSize = 1;
		if(bufIn != NULL && nBufInSize != 0)
		{
			BYTE *bufOut = NULL;
			UINT nBufOutSize;
			/*********************开 始 加 密*************************/
			UINT nResidue=nBufInSize%4;
			nBufOutSize=nBufInSize+3;
			UINT i=nBufInSize/4;
			bufOut=new BYTE[nBufOutSize];
			if(bufOut==NULL)
			{
				return FALSE;
			}
			memset(bufOut, 0, nBufOutSize);
			*bufOut=nResidue;
			bufOut[nBufOutSize-2]=nResidue;
			UINT *p=(UINT *)bufIn;
			BYTE *pTmp=bufOut;
			pTmp++;
			UINT *pOut=(UINT *)pTmp;
			UINT iTmp=0;
			UINT *bit=new UINT[pwdSize];
			UINT *lr=new UINT[pwdSize];
			for(UINT k=0;k<pwdSize;k++)
			{
				lr[k]=pPwd[k]>>31;
				iTmp=(pPwd[k]>>16)+(pPwd[k]|0x0000ffff);
				iTmp%=28;
				if(iTmp<5)
					iTmp+=5;
				bit[k]=iTmp;
			}
			UINT j=0;
			while(i--)
			{
				j=0;
				*pOut=*p++;
				while(j<pwdSize)
				{
					lr[j]==1 ? (*pOut=(*pOut>>(32-bit[j]))|(*pOut<<bit[j])) : (*pOut=(*pOut<<(32-bit[j]))|(*pOut>>bit[j]));
					*pOut=*pOut^pPwd[j];
					j++;
				}
				pOut++;
			}
			BYTE *t1=(BYTE*)pOut;
			BYTE *t2=(BYTE*)p;
			while(nResidue--)
			{
				*t1++=*t2++;
			}
			t1 = NULL;
			t2 = NULL;
			pOut = NULL;
			p = NULL;
			nBufOutSize-=1;
			delete []bit;
			delete []lr;
			/*********************加 密 完 成*************************/
			USES_CONVERSION;
			FILE *pf = NULL;
			fopen_s(&pf, T2CA(lpFileName), "wb");
			if(pf)
			{
				fwrite(bufOut, 1, nBufOutSize, pf);
				fclose(pf);
			}
			delete []bufOut;
			return TRUE;
		}
		return FALSE;
	}
	static BOOL DecodeFile(LPCTSTR lpFileName, BYTE *bufOut, UINT *nBufOutSize)
	{
		if(lpFileName == NULL || bufOut == NULL || nBufOutSize == NULL)
		{
			return FALSE;
		}
		const UINT pPwd[] = {0x73D35BE6}; //暂时放在这里
		const UINT pwdSize = 1;
		USES_CONVERSION;
		FILE *pf = NULL;
		pf=_fsopen(T2CA(lpFileName), "rb", _SH_DENYWR);
		if(pf)
		{
			UINT nBufInSize = GetFileSize(lpFileName);
			if(nBufInSize == 0)
			{
				fclose(pf);
				return FALSE;
			}
			BYTE *bufIn=new BYTE[nBufInSize];
			if(bufIn == NULL)
			{
				fclose(pf);
				return FALSE;
			}
			memset(bufIn, 0, nBufInSize);
			if(!fread(bufIn, 1, nBufInSize, pf))
			{
				fclose(pf);
				return FALSE;
			}
			fclose(pf);
			/*********************解 密 开 始*************************/
			UINT nAddTail=*bufIn;
			if(nAddTail<0 || nAddTail>=4)
			{
				nAddTail=*(bufIn+nBufInSize-1);
			}
			if(nAddTail<0 || nAddTail>=4)
			{
				return FALSE;
			}
			*nBufOutSize=nBufInSize-2;
			UINT i=*nBufOutSize/4;
			BYTE *pTmp=bufIn;
			pTmp++;
			UINT *p=(UINT *)pTmp;
			UINT *pOut=(UINT *)(bufOut);
			UINT iTmp=0;
			UINT *bit=new UINT[pwdSize];
			UINT *lr=new UINT[pwdSize];
			for(UINT k=0;k<pwdSize;k++)
			{
				lr[k]=pPwd[k]>>31;
				iTmp=(pPwd[k]>>16)+(pPwd[k]|0x0000ffff);
				iTmp%=28;
				if(iTmp<5)
				{
					iTmp+=5;
				}
				bit[k]=iTmp;
			}
			int j=0;
			while(i--)
			{
				j=(int)(pwdSize-1);
				*pOut=*p++;
				while(j>=0)
				{
					*pOut=*pOut^pPwd[j];
					lr[j]==0 ? (*pOut=(*pOut>>(32-bit[j]))|(*pOut<<bit[j])) : (*pOut=(*pOut<<(32-bit[j]))|(*pOut>>bit[j]));
					j--;
				}
				pOut++;
			}
			BYTE *t1=(BYTE*)pOut;
			BYTE *t2=(BYTE*)p;
			while(nAddTail--)
			{
				*t1++=*t2++;
			}
			while((*t1 != 0x3E)&&(t1-bufOut > 0))//尾部清0，防止XML加载失败
			{
				*t1-- = 0;
			}
			*nBufOutSize = t1-bufOut;
			*nBufOutSize +=2;
			t1 = NULL;
			t2 = NULL;
			delete []bit;
			delete []lr;
			pOut = NULL;
			p = NULL;
			/*********************解 密 完 成*************************/
			delete []bufIn;
			return TRUE;
		}
		return FALSE;
	}
	//XML文件专用
	static BOOL EncodeF2F(LPCTSTR lpFileNameDes, LPCTSTR lpFileNameSrc)
	{
		WIN32_FIND_DATA fileInfo;
		HANDLE hFind;
		UINT nBufInSize = 0;
		hFind = FindFirstFile(lpFileNameSrc,&fileInfo);
		if(hFind != INVALID_HANDLE_VALUE && hFind != INVALID_HANDLE_VALUE)
		{
			nBufInSize = fileInfo.nFileSizeLow; //获得文件的大小(字节数）
		}
		FindClose(hFind);
		if(nBufInSize == 0)
		{
			return FALSE;
		}
		CXMLDoc doc;
		if(!doc.Init_())
		{
			return FALSE;
		}
		BOOL bRet = FALSE;
		if(doc.Load_(lpFileNameSrc))
		{
			CString strXml = doc.GetXml_();
			UINT nBufInSize = strXml.GetLength();
			TCHAR* bufIn = new TCHAR[nBufInSize*2+1];
			if(bufIn == NULL)
			{
				return FALSE;
			}
			memset(bufIn,0,nBufInSize*2+1);
			lstrcpy(bufIn,strXml);
			if(En_DeFile::EncodeFile(lpFileNameDes,bufIn,nBufInSize*2))
			{
				bRet = TRUE;
			}
			//SafeDelete(bufIn);
			if (bufIn)
			{
				delete []bufIn;
				bufIn = NULL;
			}
		}
		doc.Clear_();
		return bRet;
	}
	static UINT GetFileSize(LPCTSTR lpFileName)
	{
		WIN32_FIND_DATA fileInfo;
		HANDLE hFind;
		UINT nBufInSize = 0;
		hFind = FindFirstFile(lpFileName,&fileInfo);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			nBufInSize = fileInfo.nFileSizeLow; //获得文件的大小(字节数）
		}
		FindClose(hFind);
		return nBufInSize;
	}
private:
};

#include <atlrx.h>
class CheckString
{
public:
	/*****************************************************
	* 函数名称：IsIllegal
	* 功能说明: 检查字符串是否含有关键字
	* 输入参数：LPCTSTR lpFileName[in]: XML keywords文件名
				CString strToCheck[in]: 待检查的字符串
				int		type[in]:检查类型，0：文件名和hash, 1：文字链广告, 2:引用页域名
	********************************************************/
	static BOOL IsIllegal(LPCTSTR lpFileName, CString strToCheck, int type = 0)//110系统 检查函数
	{
		CString strWords = _T("");	//文件名或文字广告，用关键字匹配
		CString hash = _T("");	//Hash，用hash匹配
		CString webUrl= _T(""); //引用页域名，用webUrl匹配
		if(type == 0)
		{
			CString strUrl(strToCheck);
			strUrl.MakeLower();

			// 标准化
			if(strUrl.Left(7).CompareNoCase(_T("http://")) == 0)
			{
				// P2P:  http://go103.qvodzy.com.cn:8032/233614692/1BD4EC86A85632651E9EB2DE29D9DC5224F1DC9A/武林女大学生01.rmvb
				int index2 = strToCheck.ReverseFind(_T('/'));
				CString strTmp = strToCheck.Left(index2);
				int index1 = strTmp.ReverseFind(_T('/'));
				if(index1 != -1 && index2 != -1 && index2-index1 == 41)
				{
					USES_CONVERSION;
					char pHashNew[41] = {0};
					CString strHashOld = strToCheck.Mid(index1+1,40);
					HashF2T(T2A(strHashOld), pHashNew);
					strToCheck.Replace(strHashOld,A2T(pHashNew));
				}
			}
			else if(strUrl.Left(7).CompareNoCase(_T("qhtp://")) == 0)
			{
				CQhtpEn_De decode;
				USES_CONVERSION;
				strToCheck=strToCheck.Mid(7);
				char *pStr = T2A(strToCheck);
				char *strdestr = decode.QhtpDecode(pStr);
				strToCheck = _T("http://");
				strToCheck += A2T(strdestr);
			}
			ParseUrl(strToCheck, hash, strWords);
			strWords.MakeLower();
			hash.MakeLower();
		}
		else if(type == 1)
		{
			strWords = strToCheck.MakeLower();
		}
		else if(type == 2)
		{
			webUrl = GetMainUrl(strToCheck);
			webUrl.MakeLower();
		}
		BOOL bRet = FALSE;
		BOOL bFlag = FALSE;
		CString strKeyWords;
		CXMLDoc doc;
		CXMLNodeList xmlList;
		CXMLElement xmlRoot;
		if(!doc.Init_())
		{
			return FALSE;
		}
		//解密文件
		UINT newSize = En_DeFile::GetFileSize(lpFileName)+1;
		BYTE *bufOut = new BYTE[newSize];
		memset(bufOut,0,newSize);
		UINT nBufInSize;
		if(En_DeFile::DecodeFile(lpFileName, bufOut, &nBufInSize))
		{
			if(doc.LoadXml_((TCHAR*)bufOut))
			{
				bFlag = TRUE;
			}
		}
		if(bFlag)
		{
			doc.GetDocumentElement_(xmlRoot);
			xmlRoot.GetChildNodes_(xmlList);
			long count = xmlList.GetItemCount_();
			for(long i=0; i<count; i++)
			{
				xmlList.GetItemDisp_(i, xmlRoot.m_Node);
				strKeyWords.Empty();
				xmlRoot.GetText_(strKeyWords);
				CString strTag = xmlRoot.GetTagName_();
				CString checkStrTmp;
				if(strTag.Find(_T("content")) != -1)//关键字
				{
					checkStrTmp = strWords;
				}
				else if(strTag == _T("hash") && hash.GetLength() > 0)//hash
				{
					checkStrTmp = hash;
				}
				else if(strTag == _T("weburl") && webUrl.GetLength() > 0)//weburl
				{
					checkStrTmp = webUrl;
				}
				else
				{
					continue;
				}
				int indexHead = 0;
				int indexTail = -1;
				//开始循环匹配
				do
				{
					indexHead = indexTail+1;
					indexTail = strKeyWords.Find(_T(';'),indexHead);
					CString strTmp;
					if (indexTail == -1)
					{
						strTmp = strKeyWords.Mid(indexHead);
					}
					else
					{
						strTmp = strKeyWords.Mid(indexHead, indexTail-indexHead);
					}
					strTmp.Trim();
					strTmp.MakeLower();
					if(strTag == _T("weburl"))//域名过滤
					{
						if(strTmp[0] == _T('.'))//容错处理
						{
							strTmp = strTmp.Mid(1);
						}
						strTmp.Replace(_T(".") ,_T("\\."));
						CString strParse1 = _T('^') + strTmp + _T('$');
						CString strParse2 = _T(".*\\.") + strTmp + _T('$');
						CAtlRegExp<> reUrl;
						REParseError status = reUrl.Parse(strParse1);//abc.sina.com.cn
						if (REPARSE_ERROR_OK == status)
						{
							CAtlREMatchContext<> mcUrl;
							if (reUrl.Match(checkStrTmp, &mcUrl))
							{
								bRet = TRUE;
								break;
							}
						}
						status = reUrl.Parse(strParse2);//*.abc.sina.com.cn
						if (REPARSE_ERROR_OK == status)
						{
							CAtlREMatchContext<> mcUrl;
							if (reUrl.Match(checkStrTmp, &mcUrl))
							{
								bRet = TRUE;
								break;
							}
						}
					}
					else if(strTmp.GetLength()>0 && -1 != checkStrTmp.Find(strTmp))//hash和文件名过滤
					{
						bRet = TRUE;
						break;
					}
				} while (indexTail != -1);
				if (bRet)
				{
					break;
				}
			}
		}
		doc.Clear_();
		if(bufOut != NULL)
			delete []bufOut;
		return bRet;
	}

	//根据播放链接提取出 hash 和 文件名
	static void ParseUrl(CString url, CString &hash, CString &fileName)
	{
		CString strUrl = url;
		CString strUrlLower = strUrl.MakeLower();
		if(strUrlLower.Left(7).Compare(_T("qvod://")) == 0)
		{
			CString strTmp1 = strUrl.Left(strUrl.ReverseFind(_T('|')));
			int index1 = strTmp1.ReverseFind(_T('|'));
			fileName = strTmp1.Mid(strTmp1.ReverseFind(_T('|'))+1);
			CString strTmp2 = strTmp1.Left(strTmp1.ReverseFind(_T('|')));
			hash = strTmp2.Mid(strTmp2.ReverseFind(_T('|'))+1);
		}
		else if(strUrlLower.Left(7).CompareNoCase(_T("http://")) == 0)
		{
			int index2 = strUrl.ReverseFind(_T('/'));
			CString strTmp = strUrl.Left(index2);
			int index1 = strTmp.ReverseFind(_T('/'));
			if(index1 != -1 && index2 != -1 && index2-index1 == 41)
			{
				fileName = strUrl.Mid(index2+1);
				hash = strUrl.Mid(index1+1, 40);
			}
			else
			{
				CString strNameTmp = strUrl.Mid(strUrl.ReverseFind(_T('/'))+1);
				int i = strNameTmp.ReverseFind(_T('='));
				if(i != -1)
				{
					fileName = strNameTmp.Mid(i+1);
				}
				else
				{
					fileName = strNameTmp;
				}
				hash = _T("");
			}
		}
		else if(strUrlLower.Left(5).CompareNoCase(_T("qlive")) == 0)
		{
			strUrl = strUrl.Left(strUrl.GetLength()-1);
			int index = strUrl.ReverseFind(_T('|'));
			fileName = strUrl.Mid(index + 1);
			CString strTmp = strUrl.Left(index);
			int index1 = strTmp.ReverseFind(_T('/'));
			hash = strTmp.Mid(index1+1);
		}
		int exFileName = fileName.ReverseFind('.');//去文件扩展名
		if (exFileName != -1)
		{
			fileName = fileName.Left(exFileName);
		}
	}

	static void Char2Hash(const UCHAR* chars,  UCHAR *hash)
	{
		int i ,j=0;
		UCHAR c;
		for (i=0;i<40;)
		{
			if('9'>= chars[i]&& chars[i]>= '0' )
				c = (chars[i] - '0')<< 4;
			else if('F'>= chars[i]&& chars[i]>= 'A' )
				c = (chars[i] - 'A'+10)<< 4;
			else if('f'>= chars[i]&& chars[i]>= 'a')
				c = (chars[i] - 'a'+10)<< 4;
			else
				return ;
			i++;
			if('9'>= chars[i]&& chars[i]>= '0' )
				c += (chars[i] - '0');
			else if('F'>= chars[i]&& chars[i]>= 'A' )
				c += (chars[i] - 'A'+10);
			else if('f'>= chars[i]&& chars[i]>= 'a')
				c += (chars[i] - 'a'+10);
			else
				return ;
			i++;
			hash[j]= c;
			j++;
		}
	}

	static void Hash2Char(const UCHAR* hash,   char *pchars)
	{
		char buf[41]={0};
		int i,j=0;
		UCHAR c;
		for(i=0;i<20;)
		{
			c = (hash[i]>>4) & 0x0f;
			if(c > 9)
				buf[j++] = 'A'+ (c - 10);
			else
				buf[j++] = '0' + c;
			c = hash[i] & 0x0f;
			if(c > 9)
				buf[j++] = 'A'+ (c - 10);
			else
				buf[j++] = '0' + c;
			i++;
		}
		memcpy(pchars, buf, 40);
	}

	static void HashF2T(const char *poldhash, char *pnewhash)//http
	{
		UCHAR hash[20];
		Char2Hash((const UCHAR*)poldhash, hash);
		unsigned int *phash = (unsigned int*)hash;
		int m,n = 0;
		const char key[8] = {1,9,7,8,0,9,1,5};
		for(m = 0;m<5;m++)
		{
			hash[0+m*4] ^= 0x69;
			hash[1+m*4] ^= 0x4A;
			hash[2+m*4] ^= 0x87;
			hash[3+m*4] ^= 0x3C;
			phash[m] = (phash[m]<<key[n]) + (phash[m]>>(32-key[n]));
			int range = hash[2]%4+1;
			n = (n+1)%range;
		}
		Hash2Char(hash, pnewhash);
	}

	//此函数获得的URL包括子域名，例如：www.
	static CString GetMainUrl(LPCTSTR pURL)
	{
		CString strURL = pURL;
		strURL.MakeLower();
		// 去协议头
		//strURL.TrimLeft(_T("http://"));
		if(strURL.Find(_T("http://")) == 0)
		{
			strURL = strURL.Right(strURL.GetLength() - 7);
		}

		// 去子页面
		int t = strURL.Find(_T("/"));
		if(t >= 0)
		{
			strURL = strURL.Left(t);
		}

		// 去端口号
		t = strURL.Find(_T(":"));
		if(t >= 0)
		{
			strURL = strURL.Left(t);
		}

		// TODO: 是否是纯IP
		int nRDot = strURL.ReverseFind('.');
		if(nRDot>0)
		{
			nRDot++;
			CString str = strURL.Right(strURL.GetLength() - nRDot);
			int t = _ttol(str);
			if(t > 0)
			{
				return strURL;
			}
		}
		return strURL;
	}
};
#endif

#ifdef NETTOOL
class NetTool
{
public:
#ifdef USES_GZIP
	static BOOL UnGzip(const BYTE* gzdata, size_t gzdata_len, HGLOBAL *phMem)
	{
		unsigned long out_count = 0;
		z_stream d_stream = {0};

		const long buff_len_ = 102400;

		BYTE* uncompress_buff_ = (BYTE*)malloc(buff_len_);

		d_stream.zalloc = (alloc_func)0;
		d_stream.zfree = (free_func)0;
		d_stream.opaque = (voidpf)0;
		d_stream.next_in = (BYTE*)gzdata;
		d_stream.avail_in = gzdata_len;
		d_stream.avail_out = buff_len_;
		d_stream.next_out = uncompress_buff_;

		if(inflateInit2(&d_stream, 47) != Z_OK)
			return false;

		if(*phMem)
		{
			GlobalFree(*phMem);
			*phMem = NULL;
		}

		int err = Z_DATA_ERROR;
		std::vector<HGLOBAL> vhMem;
		std::vector<DWORD> vSize;
		while (d_stream.total_in < gzdata_len)
		{
			if((err = inflate(&d_stream, Z_SYNC_FLUSH)) == Z_STREAM_END)
			{
				DWORD dwLen = d_stream.total_out - out_count;
				if(dwLen)
				{
					HGLOBAL hTemp = GlobalAlloc(GHND, dwLen);
					BYTE *pBuff = (BYTE*)GlobalLock(hTemp);
					memcpy(pBuff, uncompress_buff_, dwLen);
					GlobalUnlock(hTemp);
					vhMem.push_back(hTemp);
					vSize.push_back(dwLen);
				}

				err = inflateEnd(&d_stream);
				break;
			}

			if(err == Z_OK)
			{
				DWORD dwLen = d_stream.total_out - out_count;
				if(dwLen)
				{
					HGLOBAL hTemp = GlobalAlloc(GHND, dwLen);
					BYTE *pBuff = (BYTE*)GlobalLock(hTemp);
					memcpy(pBuff, uncompress_buff_, dwLen);
					vhMem.push_back(hTemp);
					GlobalUnlock(hTemp);
					vSize.push_back(dwLen);
				}
				out_count = d_stream.total_out;
				d_stream.avail_out = buff_len_;
				d_stream.next_out = uncompress_buff_;
			}
			else
			{
				break;
			}
		}
		if(vhMem.size() > 0)
		{
			DWORD dwLen = 0;
			for(DWORD i=0, size = vSize.size(); i<size; ++i)
			{
				dwLen += vSize[i];
			}
			if(dwLen)
			{
				*phMem = GlobalAlloc(GHND, dwLen);
				BYTE *pDest = (BYTE*)GlobalLock(*phMem);
				for(DWORD i=0, size = vhMem.size(); i<size; ++i)
				{
					BYTE *pSrc = (BYTE*)GlobalLock(vhMem[i]);
					memcpy(pDest, pSrc,  vSize[i]);
					pDest += vSize[i];
					GlobalUnlock(vhMem[i]);
					GlobalFree(vhMem[i]);
				}
				GlobalUnlock(*phMem);
			}
		}
		if((err == Z_OK)&&(*phMem == NULL))
		{
			*phMem = GlobalAlloc(GHND, 0);
		}
		return err == Z_OK;
	}
#endif

static int GetAddrFromHTTP(CString& HTTPaddr,unsigned int& ip,unsigned short& port)
{
	USES_CONVERSION;
	int seppos;
	CString strip;
	CString strport;
	seppos = HTTPaddr.Find(_T("http://"));
	if (seppos >= 0)
	{
		CString str = HTTPaddr;
		HTTPaddr = (LPCTSTR)str + 7;
	}

	else
	{
		return -1;
	}
	seppos = HTTPaddr.Find('/');
	if (seppos >0)
	{
		*(TCHAR*)((LPCTSTR)HTTPaddr + seppos) = 0;
	}
	seppos= HTTPaddr.ReverseFind(':');
	if (seppos == -1)
	{
		port = htons(80);
		strip = HTTPaddr;
	}
	else
	{
		strip = HTTPaddr;
		*(TCHAR*)((LPCTSTR)strip + seppos) = 0;
		strport = (LPCTSTR)HTTPaddr + seppos+1;
		*(TCHAR*)((LPCTSTR)strip + seppos) = 0;
		port = htons(_ttoi(strport));
	}

	hostent* phost = gethostbyname(T2CA(strip));
	if (!phost)
	{
		return -1;
	}
	memcpy(&ip,phost->h_addr_list[0],min(sizeof(int),phost->h_length));

	return 0;

}
//发送本地数据给网站，或请求数据 //下载方式2 NetTool
// 不使用GZIB
static bool GetDownloadURL2(const TCHAR* url_t, HGLOBAL *phBuffer, long *outSize, TCHAR *pLastModified=NULL,DWORD sockettimeout = ATL_SOCK_TIMEOUT)
{
	if(NULL == phBuffer || NULL == url_t)
	{
		return false;
	}

	if(outSize)
	{
		*outSize = 0;
	}
	CAtlHttpClient httpClient;
	CAtlNavigateData navData;
	navData.SetSocketTimeout(sockettimeout);
	CString strFmtget;
	CString strTmp;
	if (httpClient.Navigate(url_t, &navData))
	{
		long len = httpClient.GetBodyLength();
		if(len > 0)
		{
			if(pLastModified != NULL)
			{
				strTmp.Empty();
				httpClient.GetHeaderValue(_T("Last-Modified"),strTmp);
				lstrcpy(pLastModified,strTmp);
			}
			CString strCoding;
			char *pBuff = NULL;
			httpClient.GetHeaderValue(_T("Content-Encoding"), strCoding);//获得文档的编码
			if(strCoding.Find(_T("gzip")) != -1)//gzip压缩文档
			{
				return false;
			}
			else
			{
				if(outSize)
				{
					*outSize = len;
				}
				*phBuffer = GlobalAlloc(GHND, len);
				if(phBuffer)
				{
					const BYTE *pBody = httpClient.GetBody();
					if(pBody)
					{
						BYTE *pBuff = (BYTE*)GlobalLock(*phBuffer);
						memcpy(pBuff, pBody, len);
						GlobalUnlock(*phBuffer);
					}
				}
			}
		}
	}
	bool bRet = httpClient.GetStatus() == 200 || httpClient.GetStatus() == 304;
	httpClient.Close();
	return bRet;
}

static bool GetDownloadURL(const TCHAR* url_t, HGLOBAL *phBuffer, long *outSize, TCHAR *pLastModified=NULL,DWORD sockettimeout = ATL_SOCK_TIMEOUT, const TCHAR* pPostData = NULL)//发送本地数据给网站，或请求数据 //下载方式2 NetTool
{
	if(NULL == phBuffer || NULL == url_t)
	{
		return false;
	}

	if(outSize)
	{
		*outSize = 0;
	}
	CAtlHttpClient httpClient;
	CAtlNavigateData navData;
	navData.SetSocketTimeout(sockettimeout);
	CString strFmtget;
	CString strTmp;
#ifdef USES_GZIP
	if(pLastModified != NULL && _tcslen(pLastModified) > 0)
	{
		strTmp.Format(_T("%s"),pLastModified);
		strFmtget=_T("Accept: */*\r\nAccept-Encoding: gzip, deflate\r\n")\
					 _T("Connection: Keep-Alive\r\n");//If-Modified-Since: ");
		navData.SetExtraHeaders(strFmtget);
	}
	else
	{
		const TCHAR* fmtget = _T("Accept: */*\r\n")\
			_T("Accept-Encoding: gzip, deflate\r\n")\
			_T("Connection: Keep-Alive\r\n");
		navData.SetExtraHeaders(fmtget);
	}
#else
	if(pLastModified != NULL && _tcslen(pLastModified) > 0)
	{
		strTmp.Format(_T("%s"),pLastModified);
		//strFmtget=_T("Accept: */*\r\nConnection: Keep-Alive\r\nIf-Modified-Since: %s\r\n");
		//strFmtget+=_T("\r\n");
		strFmtget.Format(_T("Accept: */*\r\nConnection: Keep-Alive\r\nIf-Modified-Since: %s\r\n"), strTmp);
		navData.SetExtraHeaders(strFmtget);
	}
#endif
	if (pPostData != NULL)
	{
		navData.SetMethod(_T("POST"));
		navData.SetPostData((BYTE*)pPostData, _tcslen(pPostData) * 2, _T("application/json"));
	}
	if (httpClient.Navigate(url_t, &navData))
	{
		long len = httpClient.GetBodyLength();
		if(len > 0)
		{
			if(pLastModified != NULL)
			{
				strTmp.Empty();
				httpClient.GetHeaderValue(_T("Last-Modified"),strTmp);
				lstrcpy(pLastModified,strTmp);
			}
			CString strCoding;
			char *pBuff = NULL;
			httpClient.GetHeaderValue(_T("Content-Encoding"), strCoding);//获得文档的编码
			if(strCoding.Find(_T("gzip")) != -1)//gzip压缩文档
			{
#ifdef USES_GZIP
				if(UnGzip(httpClient.GetBody(), len, phBuffer))
				{
					if(*phBuffer)
					{
						if(outSize)
						{
							*outSize = GlobalSize(*phBuffer);
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
#else
				return false;
#endif
			}
			else
			{
				if(outSize)
				{
					*outSize = len;
				}
				*phBuffer = GlobalAlloc(GHND, len);
				if(phBuffer)
				{
					const BYTE *pBody = httpClient.GetBody();
					if(pBody)
					{
						BYTE *pBuff = (BYTE*)GlobalLock(*phBuffer);
						if(pBuff)
						{
							memcpy(pBuff, pBody, len);
							GlobalUnlock(*phBuffer);
						}
					}
				}
			}
		}
	}
	bool bRet = httpClient.GetStatus() == 200 || httpClient.GetStatus() == 304;
	httpClient.Close();
	return bRet;
}

static bool DownloadURL(const TCHAR *pURL, const TCHAR *path, TCHAR *ppLastModified=NULL,DWORD sockettimeout = ATL_SOCK_TIMEOUT)
{
	bool bRet = false;
	if(lstrlen(pURL) == 0) return bRet;
	long nCopied;
	HGLOBAL hBuffer = NULL;
	if(GetDownloadURL(pURL, &hBuffer, &nCopied, ppLastModified,sockettimeout))
	{
		if(nCopied>0)
		{
			BYTE *pData = (BYTE*)GlobalLock(hBuffer);
			if(pData)
			{
				USES_CONVERSION;
				FILE *pf = NULL;
				fopen_s(&pf, T2CA(path), "wb");
				if(pf)
				{
// 					if (NULL != ppLastModified)
// 					{
// 						unsigned nLen = strlen(T2A(ppLastModified));
// 						fwrite(T2A(ppLastModified), 1, nLen, pf); //勾图相关	
// 					}
					fwrite("\r\n", 1, strlen("\r\n"), pf);	
					fwrite(pData, 1, nCopied, pf);
					fclose(pf);
					bRet = true;
				}
				GlobalUnlock(hBuffer);
			}
		}
		else
		{
			bRet = true;
		}
		if(hBuffer) GlobalFree(hBuffer);
	}
	return bRet;
}

static bool PostForm(const TCHAR *url_t, const TCHAR *pData)
{
	bool bRet = false;

	const char* fmtPost =
	"POST /%s HTTP/1.1\r\n" \
	"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/QVOD, */*\r\n" \
	"Referer: %s/\r\n" \
	"Accept-Language: zh-cn\r\n" \
	"Content-Type: " \
	"application/x-www-form-urlencoded\r\n" \
	"UA-CPU: x86\r\n" \
	"User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.2; Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1) ;  Embedded Web Browser from: http://bsalsa.com/; .NET CLR 1.1.4322; .NET CLR 2.0.50727)\r\n" \
	"Host: %s\r\n" \
	"Content-Length: %d\r\n" \
	"Connection: Keep-Alive\r\n" \
	"Cache-Control: no-cache\r\n\r\n";

	char url_c[MAX_PATH*2] = {0};
	USES_CONVERSION;
	strncpy_s(url_c, T2CA(url_t), MAX_PATH*2);
	char *url = url_c;
	url += strlen("http://");
	char host[260];
	memset(host, 0, 260);
	char* purl = strstr(url,"/");
	memcpy(host, url, purl-url);
	if(purl)
	{
		*purl = 0;
		purl++;
		char buf[102400];
		char *pbuf = buf;
		char buf2[102400];
		strncpy_s(buf2, T2CA(pData), 102400);
		long len = (long)strlen(buf2);
		pbuf += sprintf_s(pbuf, buf-pbuf+102400, fmtPost, purl, url_c, url, len);
		strncpy_s(pbuf, buf-pbuf+102400, buf2, len);
		pbuf += len;
		//////////////////////////////////////////////////////
		char* dataoffset = NULL;
		int nMs = 10000;
		int recvlen = 0;
		UINT remoteip;
		USHORT remoteport;
		CString strurl = _T("http://");
		strurl += host;
		if( 0 != GetAddrFromHTTP(strurl,remoteip,remoteport))
		{
			return false;
		}
		SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char*)&nMs,sizeof(int));
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = remoteip;
		addr.sin_port = remoteport;
		if(0 != connect(s,(sockaddr*)&addr,sizeof(addr)))
		{
			closesocket(s);
			return false;
		}
		int datalen = send(s,buf,pbuf-buf,0);
		bRet = true;
		if(datalen <= 0)
		{
			bRet = false;
		}
		datalen = recv(s,buf,102400,0);
		closesocket(s);
	}
	return bRet;
}

};

class CDownloader
{
public:
CString m_strURL;
CString m_strPath;
CString m_strPostData;
CAtlHttpClient m_httpClient;

CDownloader(const TCHAR *szName = NULL)
{
//	s = 0;
	m_isRequireCheckTime = false;
	m_Status = -1;
	m_hBuffer = NULL;
	m_recvlen = 0;
	m_isDown = m_bExit = m_bPost = false;
	m_hEvent = CreateEvent(NULL, TRUE, TRUE, szName);
}

~CDownloader()
{
	if(m_hEvent)
	{
	CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

void Clear()
{
	Abort();
	m_Status = -1;
	m_strModifyTime.Empty();
	m_isRequireCheckTime = false;
	WaitThread();
	m_bExit = m_isDown = false;
	if(m_hBuffer)
	{
		GlobalFree(m_hBuffer);
	}
	m_hBuffer = NULL;
	m_isDown = false;
//	m_blockSize = 0;
	m_recvlen = 0;
}

void SetURL(const TCHAR *szURL, const TCHAR *szPath, const TCHAR *szData = NULL)
{
	Abort();
	m_strURL = _T("");
	m_strPath = _T("");
	if(szURL)
	{
		m_strURL = szURL;
	}
	if(szPath)
	{
		m_strPath = szPath;
	}
	if(szData)
	{
		m_bPost = TRUE;
		m_strPostData = szData;
	}
}

void SetModifyTime(CString strModifyTime)
{
	m_strModifyTime = strModifyTime;
	m_isRequireCheckTime = true;
}

void GetLastModified(CString &str)
{
	str=m_strLastModified;
}

int GetStatus()
{
	return m_Status;
}

void Download()
{
	Clear();
	if(m_strURL.GetLength() == 0) return;
	DWORD threadID;
	HANDLE hThread = CreateThread(0, 0, Thread_Download, (LPVOID)this, 0, &threadID); // CREATE_SUSPENDED
	if(hThread)
	{
		if(m_hEvent)
		{
			ResetEvent(m_hEvent);
		}
		//m_bThread = true;
		//SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);//THREAD_PRIORITY_LOWEST
		//ResumeThread(hThread);
		CloseHandle(hThread);
	}
}
/*
float GetProgressRate()
{
	if(m_isDown) return 1;
	if(m_blockSize == 0) return 0;
	return m_recvlen*1.0f/m_blockSize;
}*/

void Abort()
{
	m_bExit = true;
	m_httpClient.Close();
//	if(s) closesocket(s);
//	s = 0;
}

void WaitThread()
{
	if(m_hEvent)
	{
		WaitForSingleObject(m_hEvent, INFINITE);
	}
}

bool IsFinished()
{
	return m_isDown;
}

HGLOBAL GetBuffer()
{
	return m_hBuffer;
}

// void Pause() Continue()

private:
//SOCKET s;
HGLOBAL m_hBuffer;
//long m_blockSize,
long m_recvlen;
bool m_isDown, m_bExit;//, m_bThread;
HANDLE m_hEvent;
CString m_strModifyTime;
CString m_strLastModified;
bool m_isRequireCheckTime;
int m_Status;
bool m_bPost;

bool MemToFile()
{
	bool bRet = false;
	BYTE *pData = (BYTE*)GlobalLock(m_hBuffer);
	if(pData)
	{
		USES_CONVERSION;
		FILE *pf = NULL;
		fopen_s(&pf, T2CA(m_strPath), "wb");
		if(pf)
		{
			fwrite(pData, 1, m_recvlen, pf);
			fclose(pf);
			bRet = true;
		}
		GlobalUnlock(m_hBuffer);
	}
	if(bRet)
	{
		GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	return bRet;
}


bool OnDownload()
{
	bool bRet = false;
	CAtlNavigateData navData;
	CString strFmtget;
	if(m_bPost)
	{
		navData.SetMethod(ATL_HTTP_METHOD_POST);
		USES_CONVERSION;
		char *p = T2A(m_strPostData);
		navData.SetPostData((BYTE*)p, strlen(p), NULL);
	}
#ifdef USES_GZIP
	if(m_isRequireCheckTime)
	{
		strFmtget=_T("Accept: */*\r\nAccept-Encoding: gzip, deflate\r\n")\
			_T("Connection: Keep-Alive\r\n");
		//strFmtget=strFmtget+m_strModifyTime;
		//strFmtget+=_T("\r\n");
		navData.SetExtraHeaders(strFmtget);
	}
	else
	{
		const TCHAR* fmtget = _T("Accept: */*\r\n")\
			_T("Accept-Encoding: gzip, deflate\r\n")\
			_T("Connection: Keep-Alive\r\n");
		navData.SetExtraHeaders(fmtget);
	}
#else
	if(m_isRequireCheckTime)
	{
		strFmtget=_T("Accept: */*\r\nConnection: Keep-Alive\r\n");
		//	_T("If-Modified-Since: ");
		//strFmtget=strFmtget+m_strModifyTime;
		//strFmtget+=_T("\r\n");
		navData.SetExtraHeaders(strFmtget);
	}
#endif
	if(m_httpClient.Navigate(m_strURL, &navData))
	{
		m_recvlen = m_httpClient.GetBodyLength();
		m_httpClient.GetHeaderValue(_T("Last-Modified"),m_strLastModified);
		m_Status = m_httpClient.GetStatus();
		if((m_recvlen > 0) && (m_Status == 200))
		{
			CString strCoding;
			m_httpClient.GetHeaderValue(_T("Content-Encoding"), strCoding);//获得文档的编码
			if(strCoding.Find(_T("gzip")) != -1)//gzip压缩文档
			{
#ifdef USES_GZIP
				if(NetTool::UnGzip(m_httpClient.GetBody(), m_recvlen, &m_hBuffer))
				{
					if(m_hBuffer)
					{
						m_recvlen = GlobalSize(m_hBuffer);
						bRet = true;
					}
				}
#endif
			}
			else
			{
				m_hBuffer = GlobalAlloc(GPTR, m_recvlen);
				if(m_hBuffer)
				{
					BYTE *pBuffer = (BYTE*)GlobalLock(m_hBuffer);
					if(pBuffer)
					{
						memcpy(pBuffer, m_httpClient.GetBody(), m_recvlen);
						bRet = true;
					}
					GlobalUnlock(m_hBuffer);
				}
			}
		}
	}
	return bRet;
}

static DWORD WINAPI Thread_Download(LPVOID lp)
{
	CDownloader *pThis = (CDownloader*)lp;
	if(pThis->OnDownload())
	{
		pThis->m_isDown = true;

		if(pThis->m_strPath.GetLength())
		{
			if(!pThis->m_bExit)
			{
				pThis->MemToFile();
			}
		}
	}
	pThis->m_strModifyTime.Empty();
	pThis->m_isRequireCheckTime = false;
	pThis->m_httpClient.Close();
	if(pThis->m_hEvent)
	{
		SetEvent(pThis->m_hEvent);
	}
	return 0;
}
};

#endif


#endif
