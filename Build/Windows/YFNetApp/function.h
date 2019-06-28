#ifndef _FUNCTIONAL_H_
#define _FUNCTIONAL_H_

#include <atlstr.h>
#include <string>
#include <fstream>
#include <time.h>
#include <math.h>
#include <vector>

using namespace std;

#ifndef ZeroTChar
	#ifdef _UNICODE
		#define ZeroTChar(pStr, cbSize) memset(pStr, 0, cbSize*2)
	#else
		#define ZeroTChar(pStr, cbSize) memset(pStr, 0, cbSize)
	#endif
#endif

#ifndef SafeDelete
#define SafeDelete(pObject)		 if(pObject) {delete pObject; pObject = NULL;}
#endif
#ifndef SafeDeleteArray
#define SafeDeleteArray(p)		 if(p){delete []p; p = NULL;}
#endif

class Fun
{
public:
	// 查询目录下的format后缀的所有文件
	static void AnalysisiDir(CString fullpath, CString format, std::vector<CString> &vfile)
	{
		DWORD at = GetFileAttributes(fullpath);
		if( !(GetFileAttributes(fullpath)& FILE_ATTRIBUTE_DIRECTORY) )
		{
			return;
		}
		if(fullpath.Right(1).Compare(_T("\\")) != 0)
		{
			fullpath += _T("\\");
		}
		CString strpath_ = fullpath + _T("*");
		TCHAR lpwindow[MAX_PATH*3] = {0};
//		ZeroTChar(lpwindow, 0, MAX_PATH*3);
		::GetWindowsDirectory(lpwindow, MAX_PATH*3);
		std::vector<CString> vstrpathlist;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile(strpath_, &FindFileData);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			CString strposin_video = format;
			do 
			{
				CString strp = FindFileData.cFileName;
				if((strp.CompareNoCase(_T(".")) == 0) || (strp.CompareNoCase(_T("..")) == 0))
					continue;
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM 
					|| FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
					continue;
				if(!(FindFileData.dwFileAttributes  & FILE_ATTRIBUTE_DIRECTORY))			//file
				{
					if(fullpath.GetLength() <= 1)
						continue;

					CString filename(FindFileData.cFileName);
					CString name;
					name.Format(_T("%s%s"), fullpath, filename);
					//filename.MakeLower();
					int npos = filename.ReverseFind('.');
					if(npos == -1)
						continue;//break;
					filename = filename.Right(filename.GetLength() - npos);
					filename += _T(";");
					int nposvideo = strposin_video.Find(filename);
					if(nposvideo > -1)
					{
						vfile.push_back(name);
					}

				}
				else
				{
					CString strchildpath;												//path
					strchildpath.Format(_T("%s%s"), fullpath, strp);
					if(strchildpath.CompareNoCase(lpwindow) != 0)
					{
						strchildpath += '\\';
						vstrpathlist.push_back(strchildpath);
					}
				}
			}while(FindNextFile(hFind, &FindFileData));

			for (unsigned i = 0; i< vstrpathlist.size(); i++)
			{
				CString strchildpath = vstrpathlist[i];
				AnalysisiDir(strchildpath,format, vfile);
			}
		}
		::FindClose(hFind);
	}

	static BOOL GetIcon(CString file, HICON &hico)
	{
		if (file.IsEmpty())
		{
			return FALSE;
		}
		CString format;
		int len = file.ReverseFind('.');
		format = file.Right(file.GetLength() - len);
		TCHAR fileBuff[MAX_PATH];
		ZeroTChar(fileBuff, MAX_PATH);
		_tcsncpy_s(fileBuff, format, MAX_PATH - 1);
		HMODULE hModule = LoadLibrary(_T("user32.dll"));
		if(hModule == NULL) 
			return 0;
		typedef UINT (_stdcall PrivateExtractIcons_t)(IN LPCSTR szFileName, IN int nIconIndex, IN int cxIcon,
			IN int cyIcon, OUT HICON   *phicon, OUT UINT *piconid, IN UINT nIcons, IN UINT flags);
		PrivateExtractIcons_t *pPrivateExtractIcons = NULL;
		if(hModule)
		{
			pPrivateExtractIcons = (PrivateExtractIcons_t*)GetProcAddress(hModule, "PrivateExtractIconsW");
		}

		TCHAR szBuff[MAX_PATH] = {0};
		lstrcpy(szBuff, fileBuff);
		//	con<<szBuff<<"\r\n";

		HKEY hKey = NULL;
		RegOpenKeyEx(HKEY_CLASSES_ROOT, szBuff, 0, KEY_READ, &hKey);
		long cbValue = sizeof(TCHAR)*MAX_PATH;
		memset(szBuff, 0, cbValue);
		if(hKey)
		{
			RegQueryValue(hKey, NULL, szBuff, &cbValue);
			//		con<<szBuff<<"\n";
			RegCloseKey(hKey);
			hKey = NULL;
		}

		if(lstrlen(szBuff))
		{
			_tcscat_s(szBuff, _T("\\DefaultIcon"));
			RegOpenKeyEx(HKEY_CLASSES_ROOT, szBuff, 0, KEY_READ, &hKey);
			cbValue = sizeof(TCHAR)*MAX_PATH;
			memset(szBuff, 0, cbValue);
			if(hKey)
			{
				RegQueryValue(hKey, NULL, szBuff, &cbValue);
				//			con<<szBuff<<"\n";
				RegCloseKey(hKey);
			}
		}
		if(lstrlen(szBuff))
		{
			CString str;
			long index = 0;
			TCHAR *pDt = _tcschr(szBuff, _T(','));
			if(pDt)
			{
				*pDt = 0;
				index = _ttol(pDt+1);
			}
			TCHAR *p = szBuff;
			if(p[0] == '\"')
			{
				p ++;
				szBuff[lstrlen(szBuff)-1] = 0;
			}
			str = p;

			HICON hIconLarge = NULL;

			UINT rs_id = 0;
			long count = 0;
			if(pPrivateExtractIcons)
			{
				count = pPrivateExtractIcons((LPCSTR)((LPCTSTR)str), index, 48, 48, &hIconLarge, &rs_id, 1, LR_LOADFROMFILE);
			}
			hico = hIconLarge;
		}
		FreeLibrary(hModule);
		return TRUE;
	}

	//字符串转换到宽字符
	//
	static WCHAR* ToWChar(const char* lpszMulti)
	{
		static	WCHAR	wszChar[MAX_PATH] ;
		int	nLen = 0 ;

		lstrcpyW(wszChar, L"") ;

		if(lpszMulti == NULL)
			return wszChar ;

		nLen = MultiByteToWideChar(CP_ACP, 0, lpszMulti, -1, wszChar, MAX_PATH) ;

		return wszChar ;
	}

	//字符串转换到多字节
	//
	static const char* ToMulti(WCHAR* wlpszChar) 
	{
		static char szMulti[MAX_PATH] ;

		int nLen = WideCharToMultiByte(CP_ACP, 0, wlpszChar, lstrlenW(wlpszChar), szMulti, MAX_PATH, NULL, FALSE) ;
		szMulti[nLen] = '\0' ;

		return szMulti ;
	}

	static CString EncodeToUTF8(LPCSTR szSource)   
	{
		CString strSource = Fun::ANSIToUnicode(szSource);
		return EncodeToUTF8(strSource);
	}
	// 转换为UTF-8
	static CString EncodeToUTF8(LPCTSTR szSource)   
	{
		CString   sFinal,   sTemp; 
		if (NULL == szSource)
		{
			return sFinal;
		}
		WORD   ch;  
		BYTE   bt1,   bt2,   bt3,   bt4,   bt5,   bt6;
		int   n,   nMax   =   _tcslen(szSource);   
		
		for(n = 0; n < nMax; ++n)   
		{   
			ch   =   (WORD)szSource[n];  
			if   (ch   <   128)   
			{
				if((ch >= 48 && ch <= 57) || (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 121) || ch == 46 )
				{
					sFinal   +=   szSource[n];
				}
				else
				{
					sTemp.Format(_T("%%%02X"),   ch);  
					sFinal   +=   sTemp;   
				}
			}   
			else   if   (ch   <=   2047)
			{   
				bt1   =   (BYTE)(192   +   (ch   /   64));  
				bt2   =   (BYTE)(128   +   (ch   %   64));  
				sTemp.Format(_T("%%%02X%%%02X"),   bt1,   bt2);
				sFinal   +=   sTemp;   
			}   
			else   if   (ch   <=   65535)   
			{   
				bt1   =   (BYTE)(224   +   (ch   /   4096));   
				bt2   =   (BYTE)(128   +   ((ch   /   64)   %   64));   
				bt3   =   (BYTE)(128   +   (ch   %   64));   
				sTemp.Format(_T("%%%02X%%%02X%%%02X"),   bt1,   bt2,   bt3);   
				sFinal   +=   sTemp;   
			}   
			else   if   (ch   <=   2097151)   
			{   
				bt1   =   (BYTE)(240   +   (ch   /   262144));   
				bt2   =   (BYTE)(128   +   ((ch   /   4096)   %   64));   
				bt3   =   (BYTE)(128   +   ((ch   /   64)   %   64));   
				bt4   =   (BYTE)(128   +   (ch   %   64));   
				sTemp.Format(_T("%%%02X%%%02X%%%02X%%%02X"),   bt1,   bt2,   bt3,   bt4);   
				sFinal   +=   sTemp;   
			}   
			else   if   (ch   <=67108863)   
			{   
				bt1   =   (BYTE)(248   +   (ch   /   16777216));   
				bt2   =   (BYTE)(128   +   ((ch   /   262144)   %   64));   
				bt3   =   (BYTE)(128   +   ((ch   /   4096)   %   64));   
				bt4   =   (BYTE)(128   +   ((ch   /   64)   %   64));   
				bt5   =   (BYTE)(128   +   (ch   %   64));   
				sTemp.Format(_T("%%%02X%%%02X%%%02X%%%02X%%%02X"),   bt1,   bt2,   bt3,   bt4,   bt5);   
				sFinal   +=   sTemp;   
			}   
			else   if   (ch   <=2147483647)   
			{   
				bt1   =   (BYTE)(252   +   (ch   /   1073741824));   
				bt2   =   (BYTE)(128   +   ((ch   /   16777216)   %   64));   
				bt3   =   (BYTE)(128   +   ((ch   /   262144)   %   64));   
				bt4   =   (BYTE)(128   +   ((ch   /   4096)   %   64));   
				bt5   =   (BYTE)(128   +   ((ch   /   64)   %   64));   
				bt6   =   (BYTE)(128   +   (ch   %   64));   
				sTemp.Format(_T("%%%02X%%%02X%%%02X%%%02X%%%02X%%%02X"),     
					bt1,   bt2,   bt3,   bt4,   bt5,   bt6);   
				sFinal   +=   sTemp;   
			}
		}   
		return   sFinal;   
	} 

	static CString DecodeFromUTF8(LPCWSTR pStrUtf8)
	{
		std::string str = Fun::UnicodeToANSI(pStrUtf8);
		return DecodeFromUTF8(str.c_str());
	}
	//UTF8转UNICODE
	static CString DecodeFromUTF8(const char* pUTF8)
	{
		CString str;
		int UCSlen = 0, i;
		char *pTempUCS = NULL;
		BYTE *pTempUTF8 = NULL;

		if(pUTF8 == NULL)
		{
			return str;
		}
		UCSlen = MultiByteToWideChar(CP_UTF8,0,pUTF8,-1,NULL,0);
		wchar_t *pUCS = new wchar_t[UCSlen +1];
		memset((char*)pUCS, 0, sizeof(wchar_t)*(UCSlen+1));
		UCSlen--;
		pTempUCS = (char*)pUCS;
		pTempUTF8 = (BYTE*)pUTF8;
		for(i = 0; i < UCSlen; i++)
		{
			if (pTempUTF8 - (BYTE*)pUTF8 > UCSlen)
			{
				break;
			}
			if((*pTempUTF8) == '%') // 转义
			{
				pTempUTF8++;
				if((*pTempUTF8) == '%') // 还原 %
				{
					*(pTempUCS + 1) = 0x00;
					*pTempUCS = '%';
					pTempUCS += 2;
					pTempUTF8++;
				}
				if((*pTempUTF8) == 'u')
				{
					*(pTempUCS + 1) = 0x00;
					*pTempUCS = '%';
					pTempUCS += 2;
					*(pTempUCS + 1) = 0x00;
					*pTempUCS = 'u';
					pTempUCS += 2;
					pTempUTF8++;
				}
				else
				{
					if((*pTempUTF8) >= '0' && (*pTempUTF8) <= '7')// 1 byte
					{
						*(pTempUCS + 1) = 0x00;

						char *p;
						char buff[3] = {0};
						memcpy(buff, pTempUTF8, 2);
						*pTempUCS = (BYTE)strtoul((char*)buff, &p, 16);
						pTempUTF8 += 2;
						pTempUCS += 2;
					}
					else if((*pTempUTF8) >= 'C' && (*pTempUTF8) <= 'D')//2 bytes
					{
						BYTE temp[2];
						char *p;
						char buff[3] = {0};
						memcpy(buff, pTempUTF8, 2);
						temp[0] = (BYTE)strtoul((char*)buff, &p, 16);
						pTempUTF8 += 2;
						if((*pTempUTF8) != '%')
						{
							break;
						}
						pTempUTF8++;
						memcpy(buff, pTempUTF8, 2);
						temp[1] = (BYTE)strtoul((char*)buff, &p, 16);

						*(pTempUCS + 1) = (temp[0]>> 2) & 0x07;
						*pTempUCS = (temp[0] << 6) | (temp[1] & 0x3F);
						pTempUTF8 += 2;
						pTempUCS += 2;
					}
					else//3 bytes
					{
						BYTE temp[3];
						char *p;
						char buff[3] = {0};
						memcpy(buff, pTempUTF8, 2);
						temp[0] = (BYTE)strtoul((char*)buff, &p, 16);
						pTempUTF8 += 2;
						if((*pTempUTF8) != '%')
						{
							break;
						}
						pTempUTF8++;
						memcpy(buff, pTempUTF8, 2);
						temp[1] = (BYTE)strtoul((char*)buff, &p, 16);
						pTempUTF8 += 2;
						if((*pTempUTF8) != '%')
						{
							break;
						}
						pTempUTF8++;
						memcpy(buff, pTempUTF8, 2);
						temp[2] = (BYTE)strtoul((char*)buff, &p, 16);

						*(pTempUCS + 1) = (temp[0] << 4) | ((temp[1] >> 2) & 0x0F);
						*pTempUCS = (temp[1] << 6) | (temp[2] & 0x3F);
						pTempUTF8 += 2;
						pTempUCS += 2;
					}
				}
			}
			else if((*pTempUTF8) <= 0x7F)  //1 byte
			{
				*(pTempUCS + 1) = 0x00;
				*pTempUCS = *(pTempUTF8++);
				pTempUCS += 2;
			}
			else if((*pTempUTF8) >= 0xC0 && (*pTempUTF8) <= 0xDF)//2 bytes
			{
				*(pTempUCS + 1) = ((*pTempUTF8) >> 2) & 0x07;
				*pTempUCS = ((*pTempUTF8) << 6) | (*(pTempUTF8 + 1) & 0x3F);
				pTempUTF8 += 2;
				pTempUCS += 2;
			}
			else//3 bytes
			{
				*(pTempUCS + 1) = ((*pTempUTF8) << 4) | ((*(pTempUTF8 + 1) >> 2) & 0x0F);
				pTempUTF8++;
				*pTempUCS = ((*pTempUTF8) << 6) | (*(pTempUTF8+1) & 0x3F);
				pTempUTF8 += 2;
				pTempUCS += 2;
			}
		}

		str = pUCS;
		delete[]pUCS;
		return str;
	}

	static PSTR FromUnicode(UINT uCodepage, LPCWSTR szConvert, int * pConvertLen = 0)
	{
		if ( !szConvert || !wcslen(szConvert) ) return 0;

		//预转换，得到所需空间的大小
		int u8Len = ::WideCharToMultiByte(uCodepage, NULL, szConvert, wcslen(szConvert), 0, 0, 0, 0);

		//分配空间要给'\0'留个空间
		//UTF8虽然是Unicode的压缩形式，但也是多字节字符串，所以可以以char的形式保存
		char* szU8 = new char[u8Len + 1]();
		if ( pConvertLen ) *pConvertLen = u8Len;

		//转换
		if ( szU8 )
			::WideCharToMultiByte(uCodepage, NULL, szConvert, wcslen(szConvert), szU8, u8Len, 0, 0);
		return szU8;
	}

	// 转为GB2312
	static int ToGB2312(TCHAR *pDes, LPCTSTR szSource, int nLen)
	{
		TCHAR szTempSource[MAX_PATH*2] = {0};
		_tcsncpy_s(szTempSource, szSource, MAX_PATH*2 - 1);
		char szcResult[MAX_PATH*2] = {0};

		int nSize = 0;
		char * szcMulti = FromUnicode(936, szTempSource, &nSize);
		if (NULL == szcMulti)
		{
			pDes[0] = 0;
			return 0;
		}
		USES_CONVERSION;
		LPCTSTR szTemp = NULL;

		int i = 0;
		while(szcMulti[i] == 63)
		{
			i++;
		}
		nSize -= i;

		memcpy(szcResult, szcMulti+i, nSize);
		szTemp = (LPCTSTR)A2T(szcResult);

		int ncount = _tcslen(szTemp);
		memcpy(pDes, szTemp, ncount*sizeof(TCHAR));
		delete [] szcMulti;
		return ncount;
	}

	static CString EncodeToGB23(LPCTSTR szSource, int nLen)
	{
		TCHAR szTempSource[MAX_PATH*2] = {0};
		WCHAR szwChar[MAX_PATH*2] = {0};
		wcscpy_s(szwChar, szSource);
		int nBegin = 0;
		if((szwChar[0] == 0xfeff) || (szwChar[0] == 0xfffe))
		{
			nBegin++;
		}

		int nSize = 0;
		CString   sFinal;
		char * szcMulti = FromUnicode(936, szwChar+nBegin, &nSize);
		if (szcMulti)
		{
			WORD   ch;
			int   n,   nMax = strlen(szcMulti);
			CString sTemp; 
			for(n = 0; n < nMax; ++n)   
			{   
				ch   =   (WORD)szcMulti[n];
				if(ch   <   128)   
				{
					if((ch >= 48 && ch <= 57) || (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 121) || ch == 46 )
					{
						sFinal   +=   szcMulti[n];
					}
					else
					{
						sTemp.Format(_T("%%%02X"),   ch);  
						sFinal   +=   sTemp;   
					}
				}
				else
				{
					WORD bL = (WORD)(ch&0x00ff);
					sTemp.Format(_T("%%%02X"),bL);
					sFinal += sTemp;
				}
			}
			delete [] szcMulti;
		}
		return   sFinal;
	}

	static DWORD Str2Time(LPCTSTR lpTime)
	{
		DWORD dtime = 0;
		CString strTime = lpTime;
		CString tempH, tempM, tempS;
		tempH = strTime.Left(strTime.Find(':'));
		dtime = _ttoi(tempH);
		dtime *= 3600;
		tempH = strTime.Right(strTime.GetLength() - strTime.Find(':') - 1);
		tempM = tempH.Left(tempH.Find(':'));
		dtime += (60 * _ttoi(tempM));
		tempS = tempH.Right(tempH.GetLength() - tempH.Find(':') - 1);
		dtime += (_ttoi(tempS));
		return dtime;		
	}

	static float GetPer(DWORD hitotal, DWORD lototal, DWORD hidown, DWORD lodown)
	{
		float frate = 0.000f;
		double idownlen = hidown;
		idownlen *= 0xffffffff;
		idownlen += hidown;
		idownlen += lodown;

		double itotallen = hitotal;
		itotallen *= 0xffffffff;
		itotallen += hitotal;
		itotallen += lototal;

		if(itotallen > 0)
		{
			frate = (float)((idownlen*1.00) / (itotallen*1.00));
		}

		return frate;
	}

	static float GetPer(__int64 nTotal, __int64 nDown)
	{
		DWORD hidown = (DWORD)(nDown >> 32);
		DWORD lodown = (DWORD)nDown;
		DWORD hitotal = (DWORD)(nTotal >> 32);
		DWORD lototal = (DWORD)nTotal;
		return GetPer(hitotal, lototal, hidown, lodown);
	}

	static int GetPerInt(__int64 nTotal, __int64 nDown)
	{
		if (nTotal == 0)
		{
			return 0;
		}
		
		return min((int)(nDown * 1000 / nTotal), 1000);
	}

	static bool GetNewHash(char *pNewhash, char *pHash, int nBtid)
	{
		memcpy(pNewhash, pHash, 40);
		strcat(pNewhash, "|");
		char szbtid[8] = {0};
		_itoa(nBtid, szbtid, 10);
		strcat(pNewhash, szbtid);
		strcat(pNewhash, "|");
		return true;
	}
	/*****************************************************
	*
	* 函数名称：IsAsc
	* 功能说明: 分析是否是ASCII
	*
	* 输入参数：char *pSource：		[in] 输入的字符信息
				int nlen：			[in] 字符长度
	*
	* 输出参数：如果检测到是GBK，就返回为TRUE, 否则返回为FALSE。
	*
	********************************************************/
	static bool IsAsc(char *pSource, int nlen)
	{
		unsigned char*pTempSource = (unsigned char*)pSource;
		int len = nlen;//strlen(pSource);
		bool bAsc = true;
		while(len > 0)
		{
			if( *pTempSource > 0x80) // (10000000): 值小于0x80的为ASCII字符
			{
				bAsc = false;
				break;
			}
			pTempSource++;
			len--;
		}
		return bAsc;
	}
	
	/*****************************************************
	*
	* 函数名称：AnalysisCN
	* 功能说明: 分析中文编码
	*
	* 输入参数：char *pbuffer：		[in] 输入的字符信息
				int &nCNCount：		[out] 有效的中文数
				int &nGBCount：		[out] 简体中文数
				int &nBigCount:		[out] 繁体中文数
	*
	* 输出参数：如果检测到是GBK，就返回为TRUE, 否则返回为FALSE。
	*
	********************************************************/
	static int AnalysisCN(char *pbuffer, int nlen, int &nCNCount, int &nGBCount, int &nBigCount)
	{
		int nRet = 0;
		char *pTemSource = pbuffer;
		int len = nlen;

		nCNCount = 0;
		nGBCount = 0;
		nBigCount = 0;
		bool bGBK = FALSE;
		bool bBig = FALSE;

		BYTE temp = (BYTE)*pTemSource;
		while(temp < 0x81)
		{
			len--;
			pTemSource++;
			temp = (BYTE)*pTemSource;
		}

		while(len > 0)
		{
			BYTE low = 0, hi = 0;
			hi = (BYTE)*pTemSource;
			pTemSource++;
			len--;
			if(hi < 0x80)
			{
				continue;
			}

			if(pTemSource == '\0')
			{
				break;
			}
			low = (BYTE)*pTemSource;

			pTemSource++;

			if( hi>=0x81 && hi<= 0xa0 && low >= 0x40 && low<=0xfe)
			{
				bGBK = true; //gbk
				break;
			}
			else if(hi >= 0xa4 && hi<= 0xa9)// big5 gbk 特殊符号
			{
				if( (low >= 0x40 && low <= 0x7e) ||
					(low >= 0xa1 && low <= 0xfe))
				{
					nBigCount += 2;
					nGBCount += 2;
					nCNCount++;
				}
			}
			else if( hi >= 0xaa && hi<= 0xaf)
			{
				if( low >= 0x40 && low<= 0x7e)// gbk big5
				{
					nBigCount += 2;
					nGBCount++;
					nCNCount++;
				}
				else if(low>= 0x80 && low<= 0xa0)//gbk
				{
					bGBK = true;
					break;
				}
				else if(low >= 0xa0 && low <= 0xfe) // big5
				{
					nBigCount += 2; 
					nCNCount++;
				}
			}
			else if( hi >= 0xb0 && hi<= 0xc5)
			{
				if( low >= 0x40 && low <= 0x7e )// gbk big5
				{
					nBigCount += 2;
					nGBCount++;
					nCNCount++;
				}
				else if(low >= 0xa1 && low<=0xfe)// gbk big5 gb2312 ???
				{
					nGBCount++;
					nBigCount += 2;
					nCNCount++;
					if(hi <= 0xd7)
					{
						nGBCount++;
					}
				}
				else if(low >= 0x80 && low <= 0x9f)// gbk
				{
					bGBK = true;
					break;
				}
			}
			else if(hi >= 0xc6 && hi<= 0xc8)
			{
				if(hi == 0xc6 && low >=0x40 && low <= 0x7e) // gbk big5 gb2312
				{
					nGBCount += 2;
					nBigCount++;
					nCNCount++;
				}
				else if(low >= 0x80 && low <= 0x9f)// gbk
				{
					bGBK = true;
					break;
				}
				else if(low >= 0xa1 && low <= 0xfe ) // gbk gb2312
				{
					bGBK = true;
					break;
				}
			}
			else if( hi >= 0xc9 && hi <= 0xf7)
			{
				if(low >= 0x40 && low <= 0x7e)
				{
					nBigCount++;
					nGBCount++;
					nCNCount++;
				}
				else if(low >= 0xA1 && low<= 0xFE)
				{
					nBigCount++;
					nGBCount++;
					nCNCount++;
					if(hi<= 0xd7)
					{
						nGBCount++;
					}
				}
				else if(low >= 0x80 && low <= 0x9f)// gbk
				{
					bGBK = true;
					break;
				}
			}
			else if( hi>= 0xf8 && hi<=0xf9)
			{
				if( low >= 0x40 && low <= 0x7e )// gbk big5
				{
					nBigCount++;
					nGBCount++;
					nCNCount++;
				}
				else if(low >= 0x80 && low <= 0xa0 ) // gbk
				{
					bGBK = true;
					break;
				}
				else if( low >= 0xa1 && low <= 0xfe) // big5
				{
					nBigCount += 2;
					nCNCount++;
				}
			}
			else if(hi >= 0xfa && hi <= 0xfe )
			{
				if( (low >= 0x40 && low <= 0x7e) || (low>= 0x80 && low<= 0xa0) ) //gbk
				{
					bGBK = true;
					break;
				}
			}
			len--;
		}

		if(bGBK)
		{
			nRet = 1;
		}

		if(bBig)
		{
			nRet = 2;
		}
		return nRet;			
	}

	// 统计结果，判断出中文字符编码
	static UINT GetResult(int nCNCount, int nGBCount, int nBigCount)
	{
		UINT codepage = CP_ACP;
		if(nCNCount <= 0)
			return codepage;
		float fGB = (nGBCount - nCNCount)*1.0f / nCNCount*1.0f;
		float fBig = (nBigCount - nCNCount)*1.0f / nCNCount*1.0f;
		float fFlag = 1.0f / nCNCount*1.0f;
		if(fFlag < 0.2f)
		{
			fFlag = 0.2f;
		}

		if(fBig > (fGB+fFlag) )
		{
			codepage = 950;
		}
		else
		{
			codepage = 936;
		}
		return codepage;
	}

	// 获取文件的CodePage
	static int GetFileCodePage(LPCTSTR strFile)
	{
		int codepage = CP_ACP;

		// 检测是否是UTF8
		std::ifstream infile(strFile, std::ios::in);
		char buffer[MAX_PATH*3] = {0};
		char tempBuffer[MAX_PATH*3] = {0};

		int nCountACS = 0;
		int nCountUTF = 0;
		int nCountUni = 0;
		int nCount =0 ;

		while(infile.getline(buffer, MAX_PATH*3) >0) // 读取每行信息
		{
			const char cEmpty[] = "]";

			char *pdest = NULL;
			int  nSize = 0;
			pdest = strstr(buffer, cEmpty);
			nSize = pdest - buffer + 1;

			memset(tempBuffer, 0, MAX_PATH*3);

			if(nSize > 0)
			{
				if(nSize == (int)strlen(buffer) )
				{
					continue;
				}
				memcpy(tempBuffer, buffer+nSize, MAX_PATH*3);
			}
			else
			{
				memcpy(tempBuffer, buffer, MAX_PATH*3);	
			}			

			int nre = isUTF8(tempBuffer, strlen(tempBuffer)+1); //是否是UTF8
			if(nre == 1)
			{
				nCountUTF++;
			}

			bool bAcs = IsAsc(tempBuffer, MAX_PATH*3);// 判断是否是ASCII
			if(bAcs)
			{
				nCountACS++;
			}

			bool bCJK = IsCJK(tempBuffer, strlen(tempBuffer)+1);// 判断是否是CJK
			if(bCJK)
			{
				nCountUni++;
			}
			nCount++;
			if(nCount >= 10) //如果到一定程度，某一种编码有绝对优势，那么就跳出检测。
			{
				if(nCountUTF > nCountUni*3 && nCountUni > 10)
				{
					break;
				}
				if(nCountUni > nCountUTF*3 && nCountUTF > 0)
				{
					break;
				}
				if(nCount >= 20)
				{
					if( nCountACS > (nCountUni+nCountUTF)*3 && nCountUni > nCountUTF*3)
					{
						break;
					}
				}
			}
		}
		infile.close();

		if( (nCountUni > nCountUTF*3 && nCountUTF > 0) || (nCountUni > (nCount/2) && nCountUTF==0 && nCountACS == 0) )
		{
			codepage = -1;	//Unicode 
			return codepage;
		}

		if((nCountUTF > nCountUni*3) && (nCountUTF >= nCount/2))
		{
			codepage = CP_UTF8;	//UTF-8
			return codepage;
		}

		if(nCountACS > 0 && nCountUni==0 && nCountUTF == 0)
		{
			return codepage; // ACSII
		}

		// 如果不是UTF8 并且也不是ASCII，那么进一步检测。检测GB2312 与 Big5
		std::ifstream infile1(strFile, std::ios::in);
		memset(buffer, 0, MAX_PATH*3);
		bool bEnd = FALSE;
		int nCNCount = 0;
		int nGBCount = 0;
		int nBigCount = 0;
		while(infile1.getline(buffer, MAX_PATH*3) > 0 && !bEnd)
		{
			int nC = 0;
			int nG = 0;
			int nB = 0;
			int nEnd = AnalysisCN(buffer, MAX_PATH, nC, nG, nB);
			if(nEnd == 1)
			{
				codepage = 936; //GBK;
				break;
			}
			else if(nEnd == 2)
			{
				codepage = 950; // big5
				break;
			}
			else
			{
				nCNCount += nC;
				nGBCount += nG;
				nBigCount += nB;
			}
		}
		infile1.close();

		if(codepage == CP_ACP)
		{
			if(nCNCount > 0)
			{
				codepage = GetResult(nCNCount, nGBCount, nBigCount);
			}
		}
		return codepage;
	}

	// 检测是否是UTF8
	static bool isUTF8(char* pBuffer, int size)
	{
		if(pBuffer == NULL)
			return false;
		unsigned char ch = (unsigned char)(*pBuffer);
		int rawtextlen = 0;
		while(ch&0x80)
		{
			ch = ch<<1;
			rawtextlen++;
		}

		if(rawtextlen >= (size-1))
		{
			return false;
		}
		bool IsUTF8 = true;
		unsigned char* start = (unsigned char*)pBuffer;/*+rawtextlen;*/
		unsigned char* end = (unsigned char*)pBuffer + size;
		int nlen = 0;
		while (start < end)
		{
			if (*start < 0x80) // (10000000): 值小于0x80的为ASCII字符
			{
				start++;
				nlen++;
			}
			else if (*start < (0xC0)) // (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符
			{
				IsUTF8 = false;
				break;
			}
			else if (*start < (0xE0)) // (11100000): 此范围内为2字节UTF-8字符
			{
				if (start >= end - 1)
					break;
				if ((start[1] & (0xC0)) != 0x80)
				{
					IsUTF8 = false;
					break;
				}
				start += 2;
			}
			else if (*start < (0xF0)) // (11110000): 此范围内为3字节UTF-8字符
			{
				if (start >= end - 2)
					break;
				if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
				{
					IsUTF8 = false;
					break;
				}
				start += 3;
			}
			else
			{
				IsUTF8 = false;
				break;
			}
		}

		if(nlen == size)
		{
			IsUTF8 = false;
		}

		if( (size<= 8) && IsUTF8 )
		{
			start = (unsigned char*)pBuffer;/*+rawtextlen;*/
			end = (unsigned char*)pBuffer + size;
			char uchar[MAX_PATH] = {0};
			BOOL bCheck = TRUE;
			int k = 0;
 
			while(start < end) // 检测是否需要检测是否是中文的UTF_8
			{
				if( (*start >= 0x80) && (*start < 0xE0))
				{
					uchar[k+1] = ((start[0] & 0x0F) << 4) + ((start[1] >> 2) & 0x0F); 
					uchar[k+0] = ((start[1] & 0x03) << 6) + (start[2] & 0x3F);
					start += 2;
					k += 2;
				}
				else
				{
					bCheck = FALSE;
					break;
				}
			}
			if(bCheck)
			{
				bool b = IsCJK(uchar, size);
				if(!b)
				{
					IsUTF8 = FALSE;
				}
			}
		}
		return IsUTF8; 
	} 

	// 是否是 CJK
	static bool IsCJK(char *pStr, int nlen)
	{
		bool bRet = false;
		if(pStr == NULL)
		{
			return bRet;
		}
		int len= nlen;
		int size = len/2;
		if(len%2)
		{
			return bRet;
		}

		short *psSource = new short[size];
		memset(psSource, 0, size*sizeof(short));
		memcpy(psSource, pStr, len);
		int nUnicode = 0;
		int nResult = 0;
		int nCount = size;
		int nEn = 0;
		for(int i=0; i<size; i++)
		{
			unsigned short temp = (unsigned short)psSource[i];
			if(i==0 && temp == 0xFEFF)
			{
				bRet = TRUE;
				break;
			}
			if(i==(size-1) && temp==0)
			{
				nCount--;
			}
			if(temp > 0x0000 && temp<= 0x007F) //英文
			{
				nUnicode++;
			}
			else if(temp>=0x4e00 && temp<=0x9fcb) // 中文
			{
				if((temp>=0x4141 && temp<= 0x5a5a) || (temp>=0x6161 && temp<=0x7a7a) )
				{
					nEn++;
				}
				nUnicode++;
			}
			else if(temp>=0x3000 && temp<=0x30FF) // 符号temp<=0x303F 日文平假名 日文片假名
			{
				nUnicode++;
			}
			else if(temp>= 0x3300 && temp<=0x33FF) // 兼容
			{
				nUnicode++;
			}
			else if(temp>=0xF900 && temp<=0xFA2D) //象形文字
			{
				nUnicode++;
			}
			else if(temp>=0xFE30 && temp<=0xFE4F)//兼任形式
			{
				nUnicode++;
			}
			else if(temp>=0xff00 && temp<=0xffef) // 全角和半角形式
			{
				nUnicode++;
			}
		}
		float f = nUnicode*1.0f / (nCount*1.0f);
		float f1 = 0.0f;
		if(nUnicode > 0 && nEn > 0 && size > 5)
		{
			f1 = nEn*1.0f / (nUnicode*1.0f);
		}
		if( (f > 0.599999f && f1 < 0.7f) || (f>0.99999f) )
		{
			bRet = TRUE;
		}
		SafeDelete(psSource);
		return bRet;
	}

	// 获取Str的CodePage
	static int GetStrCodePage(char *pStr, int nlen)
	{
		const char cEmpty[] = "    ";

		char *pdest = NULL;
		int  nSize = 0;
		pdest = strstr(pStr, cEmpty);
		nSize = pdest - pStr;

		if(nSize > 0)
		{
			memset(pStr+nSize, 0, nlen-nSize);
			nlen = nSize;
		}

		bool bUTF8 = isUTF8(pStr, nlen);
		if(bUTF8)
		{
			return CP_UTF8;
		}

		bool bCJK = IsCJK(pStr, nlen);
		bool bAsc =  IsAsc(pStr, nlen);
		int nCount = strlen(pStr);
		char t = *(pStr+(nlen-1));
		if( t== 0)
		{
			nCount++;
		}
		if(bCJK && ( !bAsc || ( bAsc && (nCount!=nlen)) ) )
		{
			return -1;
		}

		if(bAsc)
		{
			return CP_ACP;
		}

		int nc = 0;
		int ng = 0;
		int nb = 0;
		int nEnd = AnalysisCN(pStr, nlen, nc, ng, nb);

		int codepage = CP_ACP;
		if(nEnd == 1)
		{
			codepage = 936; //GBK;
		}
		else if(nEnd == 2)
		{
			codepage = 950; // big5
		}
		else
		{
			codepage = GetResult(nc, ng, nb);
		}
		return codepage;
	}

	// 获取Str的CodePage(优先判断是否是CJK)
	static int GetStrCodePage1(char *pStr, int nlen)
	{
		const char cEmpty[] = "    ";

		char *pdest = NULL;
		int  nSize = 0;
		pdest = strstr(pStr, cEmpty);
		nSize = pdest - pStr;

		if(nSize > 0)
		{
			memset(pStr+nSize, 0, nlen-nSize);
			nlen = nSize;
		}
		bool bCJK = IsCJK(pStr, nlen);
		if (bCJK)
		{
			return -1;
		}
		bool bUTF8 = isUTF8(pStr, nlen);
		if(bUTF8)
		{
			return CP_UTF8;
		}

		bool bAsc =  IsAsc(pStr, nlen);
		int nCount = strlen(pStr);
		char t = *(pStr+(nlen-1));
		if( t== 0)
		{
			nCount++;
		}
		if(bCJK && ( !bAsc || ( bAsc && (nCount!=nlen)) ) )
		{
			return -1;
		}

		if(bAsc)
		{
			return CP_ACP;
		}

		int nc = 0;
		int ng = 0;
		int nb = 0;
		int nEnd = AnalysisCN(pStr, nlen, nc, ng, nb);

		int codepage = CP_ACP;
		if(nEnd == 1)
		{
			codepage = 936; //GBK;
		}
		else if(nEnd == 2)
		{
			codepage = 950; // big5
		}
		else
		{
			codepage = GetResult(nc, ng, nb);
		}
		return codepage;
	}
	// 按uncodePage转换
	static bool TurnTo(wchar_t *pDes, char *pSource, int oldLen, int uncodePage)
	{
		bool bRet = false;
		const char cEs[] = "&#";

		char *pTempSource = pSource;
		wchar_t *pTempDes = pDes;

		char *pdest = NULL;
		int  nSize = 0;
		pdest = strstr(pTempSource, cEs);
		nSize = pdest - pTempSource;

		if(nSize < 0)
		{
			memcpy(pDes, pSource, oldLen);
			return bRet;
		}

		const char cSe[] = ";";
		while(oldLen > 0)
		{
			if(nSize > 0)
			{
				char sztemp[MAX_PATH*2] = {0};
				memcpy(sztemp, pTempSource, nSize);
				int len = MAX_PATH*2;
				bRet = TurnToUnicode(pTempDes, sztemp, len, uncodePage);
				if(!bRet)
				{
					return bRet;
				}

				if(!(*(pTempDes+len-1)))
				{
					len--;
				}
				pTempDes += len;


				pTempSource += nSize;
				oldLen -= nSize;
				if(*pTempSource == ' ')
				{
					pTempSource++;
					oldLen--;
				}

				pdest = strstr(pTempSource, cEs);
				nSize = pdest - pTempSource;
			}
			
			while(nSize == 0 && oldLen > 0) // 处理所有需要转换的字符。
			{
				pdest = strstr(pTempSource, cSe); // 遇到转换后的16进制数，转换为内码进行识别。
				int nNewSize = pdest - pTempSource + 1;
				if(nNewSize == 8)
				{
					pTempSource += 2;
					WORD w = atoi(pTempSource);
 					*pTempDes = w;
 					pTempDes++;
					oldLen -= nNewSize;
					pTempSource += (nNewSize - 2);
					if(*pTempSource == ' ')
					{
						pTempSource++;
						oldLen--;
					}
					bRet = true;
				}
				else
				{
					pdest = strstr(pTempSource, cEs);
					nSize = pdest - pTempSource;
					break;
				}	
				pdest = strstr(pTempSource, cEs);
				nSize = pdest - pTempSource;
			}

			
			if(nSize <= 0)
			{	
				char sztemp[MAX_PATH*2] = {0};
				int len = 0;
				if(nSize == 0)
				{
					memcpy(sztemp, pTempSource, 2);
					len = 2;
				}
				else
				{
					memcpy(sztemp, pTempSource, oldLen);
					len = oldLen;
				}

				int nl = len;
				bRet = TurnToUnicode(pTempDes, sztemp, len, uncodePage);
				if(!bRet)
				{
					return bRet;
				}
				if(!(*(pTempDes+len-1)))
				{
					len--;
				}
				pTempDes += len;

				pTempSource += nl;
				oldLen -= nl;
			}

			pdest = strstr(pTempSource, cEs);
			nSize = pdest - pTempSource;
		}
		
		return bRet;
	}

	static bool IsNeedTurn(char *pSource)
	{
		bool bRet = false;
		const char cEs[] = "&#";
		char *pTempSource = pSource;
		if(!pTempSource)
		{
			return bRet;
		}

		char *pdest = NULL;
		int  nSize = 0;
		pdest = strstr(pTempSource, cEs);
		nSize = pdest - pTempSource;

		if(nSize >= 0)
		{
			const char cSe[] = ";";
			pdest = strstr(pTempSource, cSe);
			int nNewSize = pdest - pTempSource + 1;
			if( (nNewSize - nSize) == 8)
			{
				bRet = true;
			}
		}
		return bRet;
	}

	//返回pDes需要的长度
	static bool ToUnicode(wchar_t *pDes, char *pSource, int oldLen, int uncodePage)
	{
		bool bRet = false;
		bool b = IsNeedTurn(pSource);
		if(b)
		{
			bRet = TurnTo(pDes, pSource, oldLen, uncodePage);
		}
		else
		{
			bRet = TurnToUnicode(pDes, pSource, oldLen, uncodePage);
		}

		return bRet;
	}

	//根据uncodePage 转为Unicode。
	static wstring TurnToUnicode(const char* pSource, int uncodePage)
	{
		wstring strRet  = L"";
		if (uncodePage < 0)
		{
			int nLen = strlen(pSource);
			wchar_t *szDes = new wchar_t[nLen+1];
			ZeroMemory(szDes, wcslen(szDes)*sizeof(wchar_t));
			memcpy(szDes, pSource, strlen(pSource));
			strRet = szDes;
			delete [] szDes;
			return strRet;
		}
		int nLength=MultiByteToWideChar(uncodePage, 0, pSource, -1, NULL, 0);
		wchar_t* pBuffer = new wchar_t[nLength+1];
		ZeroMemory(pBuffer, sizeof(wchar_t)*(nLength+1));
		MultiByteToWideChar(uncodePage,0,pSource,-1,pBuffer,nLength);

		strRet = pBuffer;
		delete []pBuffer;
		return strRet;
	}

	static bool TurnToUnicode(wchar_t *pDes, char *pSource, int &oldLen, int uncodePage)
	{
		if(uncodePage < 0)
		{
			memcpy(pDes, pSource, oldLen);
			return true;
		}

		int nLength=MultiByteToWideChar(uncodePage, 0, pSource, -1, NULL, 0);
		if(oldLen < (nLength+1))
		{
			oldLen = nLength+1;
			//return false;
		}
		oldLen = nLength;

		wchar_t* pBuffer = new wchar_t[nLength+1];
		ZeroMemory(pBuffer, nLength+1);

		MultiByteToWideChar(uncodePage,0,pSource,-1,pBuffer,nLength);
		memcpy(pDes, pBuffer, nLength*sizeof(wchar_t));
		SafeDelete(pBuffer);
		return true;
	}

	static CString GetUnicode(char *pStr, int nlen)
	{
		int nCodePage = GetStrCodePage(pStr, nlen);
		wchar_t szUnicode[1024] = {0};
		ToUnicode(szUnicode, pStr, nlen, nCodePage);
		CString strUnicode = szUnicode;
		strUnicode.Trim();
		int pos =strUnicode.Find(_T("    "));
		if(pos > 0)
		{
			strUnicode = strUnicode.Left(pos);
		}
		return szUnicode;
	}

	static CString ANSIToUnicode(const std::string &str)
	{
		return ANSIToUnicode(str.c_str());
	}
	static CString ANSIToUnicode(const char* szStr)
	{
		CString strDes;
		if (NULL == szStr)
		{
			return strDes;
		}
		int nLength = MultiByteToWideChar(CP_ACP, 0, szStr, -1, NULL, 0);
		wchar_t* pBuffer = new wchar_t[nLength+1];
		ZeroMemory(pBuffer, nLength+1);
		MultiByteToWideChar(CP_ACP, 0, szStr, -1, pBuffer, nLength);
		strDes = pBuffer;
		delete[] pBuffer;
		return strDes;
	}

	static std::string UnicodeToANSI(LPCTSTR pStr)
	{
		std::string strDes;
		if (NULL == pStr)
		{
			return strDes;
		}
		int nLength = WideCharToMultiByte(CP_ACP, NULL, (LPCTSTR)pStr, -1, NULL, 0, NULL, NULL);
		char* pBuffer = new char[nLength+1];
		ZeroMemory(pBuffer, nLength+1);
		WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)pStr, -1, pBuffer, nLength, NULL, NULL);
		strDes = pBuffer;
		delete[] pBuffer;
		return strDes;
	}

	static string UnicodeToUtf8(const wchar_t *pszInfo)
	{
		string strRet = "";
		if (NULL == pszInfo)	
		{
			return strRet;
		}

		int nLength = WideCharToMultiByte(CP_UTF8, NULL, pszInfo, -1, NULL, 0, NULL, NULL);
		char* pBuffer = new char[nLength+1];
		memset(pBuffer, 0, nLength+1);
		WideCharToMultiByte(CP_UTF8, NULL, pszInfo, -1, pBuffer, nLength, NULL, NULL);
		strRet = pBuffer;
		delete[] pBuffer;
		return strRet;
	}

	static CString Utf8ToUnicode(const std::string &str)
	{
		CString strDes;
		int nLength = MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), -1, NULL, 0);
		wchar_t* pBuffer = new wchar_t[nLength+1];
		ZeroMemory(pBuffer, (nLength+1)*2);
		MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), -1, pBuffer, nLength);
		strDes = pBuffer;
		delete[] pBuffer;
		return strDes;
	}

	static std::string Utf8ToANSI(const std::string& str)
	{
		return UnicodeToANSI(Utf8ToUnicode(str));
	}

	static std::string ANSIToUtf8(const std::string& str)
	{
		return UnicodeToUtf8(ANSIToUnicode(str));
	}
	// 
// 	static bool StringToUnicode(wchar_t *pDes, char *pSource, int oldLen)
// 	{
// 		int len = strlen(pSource)+1;
// 		if(len <= 1)
// 		{
// 			return false;
// 		}
// 
// 		char *pNewBuffer = new char[len];
// 		memset(pNewBuffer, 0, len);
// 		memcpy(pNewBuffer, pSource, len);
// 
// 		int nre = isUTF8(pNewBuffer, len);
// 		UINT codepage = CP_ACP;
// 		if(nre == 2)
// 		{
// 			codepage = CP_UTF8;
// 		}
// 		else if(nre == 1)
// 		{
// 			int nc = 0;
// 			int ng = 0;
// 			int nb = 0;
// 			int nEnd = 1;//AnalysisCN(pNewBuffer, nc, ng, nb);
// 			if(nEnd == 1)
// 			{
// 				codepage = 936; //GBK;
// 			}
// 			else if(nEnd == 2)
// 			{
// 				codepage = 950; // big5
// 			}
// 			else
// 			{
// 				codepage = GetResult(nc, ng, nb);
// 			}
// 		}
// 
// 		SafeDelete(pNewBuffer);
// 
// 		if(ToUnicode(pDes, pSource,oldLen, codepage))
// 		{
// 			return true;
// 		}
// 		return false;
// 	}

	static CString GetKeyStr(char p)
	{
		CString str;
		str.Empty();
		switch(p)
		{
		case VK_F1: str = _T("F1"); break;
		case VK_F2: str = _T("F2"); break;
		case VK_F3: str = _T("F3"); break;
		case VK_F4: str = _T("F4"); break;
		case VK_F5: str = _T("F5"); break;
		case VK_F6: str = _T("F6"); break;
		case VK_F7: str = _T("F7"); break;
		case VK_F8: str = _T("F8"); break;
		case VK_F9: str = _T("F9"); break;
		case VK_F10: str = _T("F10"); break;
		case VK_F11: str = _T("F11"); break;
		case VK_F12: str = _T("F12"); break;
		case VK_LEFT: str = _T("←"); break;
		case VK_UP: str = _T("↑"); break;
		case VK_RIGHT: str = _T("→"); break;
		case VK_DOWN: str = _T("↓"); break;
		case VK_ESCAPE: str = _T("Esc"); break;
		case VK_RETURN: str = _T("Enter"); break;
		case VK_NUMPAD0: str = _T("0"); break;
		case VK_NUMPAD1: str = _T("1"); break;
		case VK_NUMPAD2: str = _T("2"); break;
		case VK_NUMPAD3: str = _T("3"); break;
		case VK_NUMPAD4: str = _T("4"); break;
		case VK_NUMPAD5: str = _T("5"); break;
		case VK_NUMPAD6: str = _T("6"); break;
		case VK_NUMPAD7: str = _T("7"); break;
		case VK_NUMPAD8: str = _T("8"); break;
		case VK_NUMPAD9: str = _T("9"); break;
		case VK_SPACE:	 str = _T("Space"); break;
		case VK_NEXT:	 str = _T("PageDown");break;
		case VK_PRIOR:	 str = _T("PageUp");break;
		default:
			if('0' <= p && p <= '9' || 'A' <= p && p <= 'Z')
				str.Format(_T("%c"), p);
		}
		return str;
	}

	static CString GetHotKeyStr(ACCEL* pAcce)
	{
		if (pAcce == NULL || pAcce->cmd == 0)
		{
			return _T("");
		}
		CString szCtrl, szValue;
		if(pAcce->fVirt & FALT )
		{
			szCtrl = _T("Alt+");
		}
		if(pAcce->fVirt & FCONTROL)
		{
			szCtrl += _T("Ctrl+");
		}
		if(pAcce->fVirt & FSHIFT)
		{
			szCtrl += _T("Shift+");
		}
		szValue = GetKeyStr((char)pAcce->key);

		return szCtrl + szValue;
	}

	static BOOL IsValidPath(LPTSTR szPath)
	{
		int nLength = _tcslen(szPath);
		if (nLength > 255 || nLength < 2)
		{
			return FALSE;
		}
		
		if (nLength == 2)
		{
			_tcscat(szPath, _T("\\"));
		}

		//判断盘符是否有效
		TCHAR szHead[4] = {0};
		memcpy(szHead, szPath, 3 * sizeof(TCHAR));
		int nLen = GetLogicalDriveStrings(0, NULL);
		TCHAR* cDisk = new TCHAR[nLen];
		TCHAR* pDisk = cDisk;
		GetLogicalDriveStrings(nLen, cDisk);
		int i = 0;
		for (; i < nLen / 4; i++)
		{
			if (_tcsicmp(pDisk,  szHead) == 0)
			{
				break;
			}
			pDisk += 4;
		}
		delete cDisk;

		if (i == nLen / 4)
		{
			return FALSE;
		}

		//判断有无无效字符
		TCHAR* pChar = szPath + 3;	// 去掉盘符
		TCHAR* pFilter = _T("/\\?*:\"|<>");
		BOOL bSlash = TRUE; //bSlash：前一个是否为'\'或'/'
		while(*pChar)
		{	
			TCHAR* pTmp = pFilter;
			if (!bSlash )
			{
				if ((*pChar) == '\\')
				{
					bSlash = TRUE;
					pChar++;
					continue;
				}
			}
			while(*pTmp)
			{
				if (*pChar == *pTmp)
				{
					return FALSE;
				}
				pTmp++;
			}
			bSlash = FALSE;
			pChar++;
		}
		return TRUE;
	}

	// 根据所给的路径，检测是否该路径是否存在，如果不存在就创建此路径。
	static BOOL MyCreateDirector(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
	{
		TCHAR lpFile[MAX_PATH*3] = {0};
		_tcsncpy(lpFile, lpPathName, MAX_PATH*3 - 1);
		BOOL bValid = IsValidPath(lpFile);
		if(!bValid)
		{
			return FALSE;
		}

		BOOL bRet = FALSE;

		DWORD dwStyle = GetFileAttributes(lpPathName);
		if(dwStyle == INVALID_FILE_ATTRIBUTES || (!(dwStyle&FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString strName = lpPathName;
			CString strFlag = strName;
			if(strName.Right(1).Compare(_T("\\")) == 0)
			{
				strFlag = strName.Left(strName.GetLength()-1);
			}

			if(GetFileAttributes(strFlag)!=INVALID_FILE_ATTRIBUTES)
			{
				bRet = TRUE;
				return bRet;
			}

			strFlag += _T("\\");

			std::vector<CString> vCreatedDir;	// 已创建的文件夹
			int nPos = strFlag.Find('\\');

			while(nPos > 0) // 逐层检测文件夹是否存在，如果不存在就创建
			{
				CString strTemp = strName.Left(nPos);
				DWORD dw = GetFileAttributes(strTemp);
				if(dw == INVALID_FILE_ATTRIBUTES)
				{
					if(CreateDirectory(strTemp, lpSecurityAttributes))
					{
						vCreatedDir.push_back(strTemp);
					}
					else	// 如果创建文件夹失败，删除前面已经创建的上层文件夹
					{
						int err = GetLastError();
						for(int i = vCreatedDir.size() -1; i >= 0 ; i--)
						{
							RemoveDirectory(vCreatedDir[i]);
						}
						break;
					}
				}
				
				if(strFlag.GetLength() == nPos + 1)
				{
					bRet = TRUE;
				}

				nPos = strFlag.Find('\\', nPos + 1);
			}
		}
		else
		{
			bRet = TRUE;
		}
		return bRet;
	}


	static CString GetUsabelFileName(CString strRef)
	{
		CString strEx;
		CString strUsable = strRef;
		int nPos = strRef.ReverseFind('.');
		if(nPos > 0)
		{
			strEx = strRef.Right(strRef.GetLength() - nPos);
			strRef = strRef.Left(nPos);
		}

		for (int i = 1; i < 1000 && GetFileAttributes(strUsable) != INVALID_FILE_ATTRIBUTES; i++)
		{
			strUsable.Format(_T("%s(%d)%s"), strRef, i, strEx);
		}
		return strUsable;
	}

	static BOOL HideFile(CString strPath)
	{
		DWORD dwAttr = GetFileAttributes(strPath);
		if (dwAttr != INVALID_FILE_ATTRIBUTES)
		{
			return SetFileAttributes(strPath, dwAttr|FILE_ATTRIBUTE_HIDDEN);
		}
		return FALSE;
	}

	static CString GetBit(int nBit)
	{
		CString str;
		if (nBit <= 0)
		{
			str = _T("   -- --");
		}
		else if(nBit > 1000)
		{
			str.Format(_T("%5d Kbps"), nBit/1000);
		}
		else
		{
			str.Format(_T("%5d bps"), nBit);
		}
		return str;
	}

	static CString GetDownrate(int nDownrate)
	{
		CString str;
		if (nDownrate < 1024)
		{
			str.Format(_T("%d B/S "),nDownrate );
		}
		else if (nDownrate < 1024*1024)
		{
			str.Format(_T("%d KB/S "),nDownrate/1024);
		}
		else 
		{
			str.Format(_T("%d MB/S "),nDownrate/(1024*1024));

		}
		return str;
	}

	// 将fProgress转化为String的百分比
	static CString GetProgress(float fProgress)
	{
		CString str;
		if (fProgress < 0.999999)
		{
			str.Format(_T("%d.%d"), (int)(fProgress * 100), ((int)(fProgress * 1000) % 10));
		}
		else
		{
			str = _T("100");
		}
		str += _T("%");
		return str;
	}

	static CString GetFrameRate(float fRate)
	{
		CString str;
		if(fRate > 1024.0)
		{
			str.Format(_T("%.2f Kfps"), fRate/1024);
		}
		else
		{
			str.Format(_T("%.2f fps"), fRate);
		}
		return str;
	}

	static CString ShortName(CString& fullpath)
	{
		CString temp;
		temp = fullpath.Right(fullpath.GetLength() - fullpath.ReverseFind('\\')-1);
		int nPos = temp.ReverseFind('.');
		if (nPos >= 0)
		{
			temp  = temp.Left(nPos);
		}
		return temp;
	}

	static CString GetStrTime(long time, int nType = 0)
	{
		CString strtime;
		if (time <= 0)
		{
			if (nType == 0)
			{
				strtime = _T("   --:--");
			}
			else
			{
				strtime = _T(" --:--");
			}
			return strtime;
		}

		CString temp;
		if(nType == 0)
		{
			if (time >= 3600)
			{
				strtime.Format(_T(" %d:"), time/3600);
			}
			else
			{
				strtime = _T("   ");
			}
			time %= 3600;
		}
		int d = time/60;
		if (nType == 0)
		{
			temp.Format(_T("%02d:"), d);
		}
		else
		{
			temp.Format(_T(" %d:"), d);
		}
		strtime += temp;
		time %= 60;
		temp.Format(_T("%02d"), time);
		strtime += temp;
		return strtime;
	}

	// 获取整型版本号：strVersion must be “x.x.x.x”
	static bool IntVersion(CString strVersion, long *vers, UINT num)
	{
		ZeroMemory( vers, num * sizeof(long) );
		if (strVersion.IsEmpty() || 4 > num) return false;
		strVersion.Remove(' ');
		strVersion.Replace(',', '.');
		for ( int i = 0; i < num; ++i )
		{
			long val = _ttol( (LPCTSTR)strVersion );
			if (val < 0) return false;
			*vers++ = val;
			if (i == (int)num -1) return true;
			val = strVersion.GetLength();
			strVersion = strVersion.Right(strVersion.GetLength()-strVersion.Find('.')-1);
			if (strVersion.GetLength() == val) return false;
		}
		return true;
	}

	// 比较两个整型版本号：must be “x.x.x.x”
	static int ComVersion(const long *src, UINT srcnum, const long *des, UINT desnum)
	{
		if (4 > srcnum || 4 > desnum || srcnum != desnum) return -2;
		for ( UINT i = 0; i < srcnum; ++i )
		{
			if ( src[i] > des[i] ) return 1;
			if ( src[i] == des[i] ) continue;
			if ( src[i] < des[i] ) return -1;
		}
		return 0;
	}

	static CString GetFormat(CString& strPath)
	{
		int nPos1 = strPath.ReverseFind(_T('\\'));
		int nPos2 = strPath.ReverseFind(_T('.'));
		if(nPos2 > nPos1)
		{
			return strPath.Right(strPath.GetLength() - nPos2 - 1);
		}
		return _T("");
	}

	static CString FilterChar(LPCTSTR lpStr, LPCTSTR lpFilter = _T("/\\?*:\"|<>"))
	{
		CString strDest = lpStr;
		CString strFilter = lpFilter;
		for(int i = 0; i < strDest.GetLength(); i++)
		{
			if (strFilter.Find(strDest.GetAt(i)) >= 0)
			{
				strDest.Delete(i);
				--i;
			}
		}
		return strDest;
	}

	static BOOL IsThreadAlive(HANDLE& hThread)
	{
		DWORD dwValue = 0;
		if(hThread && GetExitCodeThread(hThread, &dwValue) && (dwValue == STILL_ACTIVE))
		{
			return TRUE;
		}
		if (hThread)
		{
			CloseHandle(hThread);
			hThread = NULL;
		}
		return FALSE;
	}

	static void LoopMessage(int nSleep=20)
	{
		MSG msg ;
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
		}

		if ( nSleep > 0 )
		{
			Sleep(nSleep);
		}
	}

	static DWORD WaitThreadMsgLoopExit(HANDLE& hThread, DWORD dwMilliseconds = INFINITE)
	{
		DWORD result = 0;

		DWORD dwTick = GetTickCount() ;
		while( hThread )
		{ 
			if ( dwMilliseconds > 0 && (GetTickCount()-dwTick)>dwMilliseconds )
			{
				return WAIT_TIMEOUT ;
			}

			DWORD err = 0;
			BOOL bRet = GetExitCodeThread(hThread, &err);
			if (bRet && STILL_ACTIVE != err)
			{
				CloseHandle(hThread)  ;
				hThread = NULL;
				return 0;
			}
			
			result = MsgWaitForMultipleObjects(1, &hThread, FALSE, 500, QS_ALLINPUT); 

			if ( result == WAIT_OBJECT_0 || result == WAIT_TIMEOUT )
			{
				CloseHandle(hThread)  ;
				hThread = NULL		;

				return result ;
			} 
			else
			{ 
				Fun::LoopMessage();
			} 

			result = 0;
		}

		return result ;
	}

	static DWORD WaitThreadExit(HANDLE& hThread, DWORD dwMilliseconds = INFINITE)
	{
		DWORD dwRes = 0;
		if (hThread)
		{
			dwRes = WaitForSingleObject(hThread, dwMilliseconds);
			if(WAIT_TIMEOUT != dwRes)
			{
				CloseHandle(hThread);
				hThread = NULL;
			}
		}
		return dwRes;
	}

	template<class T> static BOOL MoveElement(T& vSrc, std::vector<UINT>& vMoveIndex, int nPos)
	{
		//int nSize = vSrc.size();
		if (nPos >= vSrc.size())
		{
			return FALSE;
		}
		T vCheckedItem;
		int j = 0;
		for(int i = 0; i < vMoveIndex.size(); i++)
		{
			int n = vMoveIndex[i] - i;
			if (n >= vSrc.size())
			{
				continue;
			}
			vCheckedItem.push_back(vSrc[n]);
			vSrc.erase(vSrc.begin() + n);
			if (vMoveIndex[i] <= nPos)
			{
				j++;
			}
		}
		if (nPos != -1)
		{
			nPos -= j - 1;
		}
		else
		{
			nPos = 0;
		}
		vSrc.insert(vSrc.begin() + nPos, vCheckedItem.begin(), vCheckedItem.end());
		return 0;
	}

	//删除指定值的元素
	template<class T> static BOOL Remove(std::vector<T>& v, T t)
	{
		for (int i = 0; i < v.size(); i++)
		{
			if (v[i] == t)
			{
				v.erase(v.begin() + i);
				return TRUE;
			}
		}
		return FALSE;
	}

	static CString MakeQvodURL(LPCSTR lpHash, LPCTSTR lpName, __int64 iSize)
	{
		
		CString str;
		if ( !lpHash || !lpName || !_tcslen(lpName) )
		{
			return str;
		}

		if(iSize == 0)
		{
			iSize = 1;
		}

		CStringA hash(lpHash,40);
		USES_CONVERSION;
		str.Format(_T("qvod://%I64u|%s|%s|"), iSize, A2T(hash), lpName);
		return str;
	}

	static BOOL ParseQvodUrl(LPCTSTR lpUrl, CString& strHash, CString& strName, __int64& iSize)
	{
		if (lpUrl == NULL)
		{
			return FALSE;
		}
		CString strUrl(lpUrl);
		if (strUrl.Left(7) != _T("qvod://"))
		{
			return FALSE;
		}
		int nPos1 = strUrl.Find(_T('|'));
		if (nPos1 < 0)
		{
			return FALSE;
		}
		iSize = _ttoi64(strUrl.Mid(7, nPos1 - 7));
		++nPos1;
		int nPos2 = strUrl.Find(_T('|'), nPos1);
		if (nPos2 < 0)
		{
			return FALSE;
		}
		strHash = strUrl.Mid(nPos1, nPos2 - nPos1);
		if (strHash.GetLength() != 40)
		{
			return FALSE;
		}
		++nPos2;
		int nPos3 = strUrl.Find(_T('|'), nPos2);
		if (nPos3 >= 0)
		{
			strName = strUrl.Mid(nPos2, nPos3 - nPos2);
		}
		return TRUE;
	}
//从html文本中找到图片的地址（若有多张图片取第一张）
	static CString GetImageUrlFromHtml(LPCTSTR lpHtml)
	{
		CString strUrl;
		CString strHtml = lpHtml;
		int nTagPos = strHtml.Find(_T("<img"));
		if (nTagPos > -1)
		{
			int nTagEndPos = strHtml.Find(_T('>'), nTagPos + 1);
			if (nTagEndPos > -1)
			{
				CString strImage = strHtml.Mid(nTagPos, nTagEndPos - nTagPos);
				int nSrcPos = strImage.Find(_T("src=\""));
				if (nSrcPos > -1)
				{
					nSrcPos += 5;
					int nSrcEndPos = strImage.Find(_T('\"'), nSrcPos);
					if (nSrcEndPos > 0)
					{
						strUrl = strImage.Mid(nSrcPos, nSrcEndPos - nSrcPos);
					}
				}

			}
		}
		return strUrl;
	}

	static std::string URLEncode(const char* str, const int len, bool encodeSlash=true)		// encodeSlash是否对'/'进行编码
	{
		std::string strRet;
		if ((str==NULL)  || (len<=0))
		{
			return strRet;
		}

		char ch;
		char szTemp[5] = {0};
		for (int i=0; i<len; ++i)
		{
			ch = str[i];
			if (((ch>='A') && (ch<='Z')) ||
				((ch>='a') && (ch<='z')) ||
				((ch>='0') && (ch<='9'))) 
			{
				strRet += ch;
			}
			else if (ch == ' ') 
			{
				strRet += '+';
			}
			else if (ch == '.' || ch == '-' || ch == '_' || ch == '~')
			{
				strRet += ch;
			} 
			else if (ch == '/' && !encodeSlash)
			{
				strRet += ch;
			}
			else
			{
				sprintf(szTemp, "%%%02X", (unsigned char)ch);
				strRet += szTemp;
			}
		}

		return strRet;
	}

	static std::string URLDecode(const char* str, const UINT len)
	{
		std::string strRet;
		if ((str==NULL)  || (len<=0))
		{
			return strRet;
		}

		char szTemp[3] = {0};
		for (size_t i = 0; i < len; i++)  
		{  
			if (str[i] == '+') 
			{
				strRet += ' ';  
			}
			else if (str[i] == '%')  
			{  
				if (i+2 < len)
				{
					szTemp[0] = str[++i];
					szTemp[1] = str[++i];

					strRet += (unsigned char)strtol(szTemp, NULL, 16);
				}
			}  
			else
			{
				strRet += str[i];  
			}
		}  
		return strRet; 
	}

	static std::string URLEncodeCN(const char* str, const int strSize)
	{
		int i;
		int j = 0;//for result index
		char ch;
		char szTemp[5] = {0};
		std::string strRet;

		if ((str==NULL) ||  (strSize<=0)) {
			return 0;
		}

		for ( i=0; i<strSize; ++i) 
		{
			ch = str[i];
			if (((ch>='A') && (ch<='Z')) ||
				((ch>='a') && (ch<='z')) ||
				((ch>='0') && (ch<='9'))) {
					strRet += ch;
			} else if (ch == ' ') {
				strRet += '+';
			} else if (ch == '.' || ch == '-' || ch == '_' || ch == '*' || ch == '/' || ch == ':' || ch=='&' || ch=='=' || ch=='?' || ch=='%' || ch=='+') {
				strRet += ch;
			} else {
				sprintf(szTemp, "%%%02X", (unsigned char)ch);
				strRet += szTemp;
				}
		}

		return strRet;
	}

	static string random_string(int length)
	{
		static bool bSetRand = false ;
		char *szbuff ;
		szbuff = new char[length+1] ;
		szbuff[length] = '\0' ;

		if ( !bSetRand )
		{
			bSetRand = true ;
			srand( (unsigned)time(0) ) ;
		}

		for (int i = 0; i < length; i++)
		{
			szbuff[i] = rand() % (122 - 48) + 48 ;
		}

		string str = szbuff ;
		delete szbuff ;

		return str ;
	}

	static string random_numbers(int length)
	{
		char *szbuff ;
		szbuff = new char[length+1] ;
		szbuff[length] = '\0' ;

		srand( (unsigned)time(0) ) ;

		for (int i = 0; i < length; i++)
		{
			szbuff[i] = rand() % 9 + 48 ;
		}

		string str = szbuff ;
		delete szbuff ;

		return str ;
	}

	static CString FormatLLTime(LONGLONG dwTime, BOOL bCNMod=FALSE)
	{
		CString strTime ;
		DWORD dwDuration = (DWORD)(dwTime / (10000*1000));
		if(dwDuration > 0)
		{
			CString strHour(_T("00:"));
			int     nHour = 0 ;
			if(dwDuration > 3600)
			{
				nHour = dwDuration/3600 ;
				strHour.Format(_T("%d:"), nHour);
				dwDuration %= 3600;
			}

			if ( bCNMod )
			{
				if ( nHour > 0 )
				{
					strTime.Format(_T("%d小时%d分"), nHour, dwDuration / 60);
				}
				else
				{
					strTime.Format(_T("%d分%d秒"), dwDuration / 60, dwDuration % 60);
				}
			}
			else
			{
				strTime.Format(_T("%s%02d:%02d"), strHour, dwDuration / 60, dwDuration % 60);
			}
		}
		return strTime ;
	}

	static void CopyString(TCHAR **src, const TCHAR *des)
	{
		SafeDeleteArray(*src)
		if ( !des || lstrlen(des)<=0 )
		{
			return ;
		}
		int nLen = lstrlen(des) ;
		*src = new TCHAR[nLen+1] ;
		lstrcpy(*src, des) ;
		(*src)[nLen] = _T('\0') ;
	}

	static void CopyString(TCHAR **src, CString & des)
	{
		SafeDeleteArray(*src)
		if ( des.GetLength() <=0 )
		{
			return ;
		}
		int nLen = des.GetLength() ;
		*src = new TCHAR[nLen+1] ;
		LPTSTR pszText = des.GetBuffer(nLen+1);
		lstrcpy(*src, pszText) ;
		des.ReleaseBuffer(nLen) ;
		(*src)[nLen] = _T('\0') ;
	}

	static void CopyStringA(char **src, const char *des)
	{
		SafeDeleteArray(*src)
		if ( !des || strlen(des)<=0 )
		{
			return ;
		}
		int nLen = strlen(des) ;
		*src = new char[nLen+1] ;
		strcpy(*src, des) ;
		(*src)[nLen] = '\0' ;
	}

	static void CopyStringA(char **src, string & des)
	{
		SafeDeleteArray(*src)
		if ( des.length() <=0 )
		{
			return ;
		}
		int nLen = des.length() ;
		*src = new char[nLen+1] ;
		strcpy(*src, des.c_str()) ;
		(*src)[nLen] = '\0' ;
	}
};

// 获取模块函数地址
template<class T>
class GetDllProc
{
public:
	GetDllProc(LPCSTR szDllPath, LPCSTR szProcName, bool bAutoFreeLibrary = true)
		: m_proc(0), m_hm(0), m_bAutoFree(bAutoFreeLibrary)
	{
		CStringA dllName = szDllPath;
		dllName = dllName.Right( dllName.GetLength() - dllName.ReverseFind('\\') - 1 );
		m_hm = GetModuleHandleA( dllName );
		if ( !m_hm ) m_hm = LoadLibraryA( szDllPath );
		if ( !m_hm ) return;
		m_proc = (T)GetProcAddress( m_hm, szProcName );
	}
	operator T(void)
	{
		return m_proc;
	}
	~GetDllProc(void)
	{
		if ( m_hm && m_bAutoFree )
		{
			FreeLibrary(m_hm);
		}
	}
private:
	GetDllProc(void){}
private:
	HMODULE	m_hm;
	T		m_proc;
	bool	m_bAutoFree;
};
#endif