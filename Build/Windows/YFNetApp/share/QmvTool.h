#pragma once

#include <string>
#include "QhtpEn_De.h"
#include "../include/mscrypto.h"
namespace QmvTool
{
	using namespace std;
	const char * const QMV_KEY = "q5vtag@13425byck";

	class CQmvTool
	{
	public:
		static PSTR FromUnicode(UINT uCodepage, LPCWSTR szConvert, int * pConvertLen = 0)
		{
			if ( !szConvert || !szConvert[0] ) return 0;

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

		static PWSTR ToUnicode(UINT uCodepage, LPCSTR szConvert, int * pConvertLen = 0)
		{
			if ( !szConvert || !szConvert[0] ) return 0;

			//预转换，得到所需空间的大小
			int ucLen = ::MultiByteToWideChar(uCodepage, NULL, szConvert, strlen(szConvert), 0, 0);

			//分配空间要给'\0'留个空间
			//UTF8虽然是Unicode的压缩形式，但也是多字节字符串，所以可以以char的形式保存
			wchar_t* szUnic = new wchar_t[ucLen + 1]();
			if ( pConvertLen ) *pConvertLen = ucLen;

			//转换
			if ( szUnic )
				::MultiByteToWideChar(uCodepage, NULL, szConvert, strlen(szConvert), szUnic, ucLen);
			return szUnic;
		}

		static string encode(CString & input)
		{
			string out;
			if ( !input.GetLength() )
			{
				return out;
			}
			CQhtpEn_De encoder;
			PSTR pAcp = FromUnicode(CP_ACP, input);
			out = pAcp;
			delete [] pAcp;
			for ( int i = 0, klen = strlen(QMV_KEY), size = out.size(); i < size; ++i )
			{
				out[i] ^= QMV_KEY[i%klen];
			}
			return encoder.base64_encode((unsigned char*)out.c_str(), out.size());
		}

		static CString decode(string & input)
		{
			CString tout;
			if ( !input.length() )
			{
				return tout;
			}
			CQhtpEn_De encoder;
			string out = encoder.base64_decode(input);
			for ( int i = 0, klen = strlen(QMV_KEY), size = out.size(); i < size; ++i )
			{
				out[i] ^= QMV_KEY[i%klen];
			}
			PWSTR pUcp = ToUnicode(CP_ACP, out.c_str());
			tout = pUcp;
			delete [] pUcp;
			return tout;
		}

		static CString to_html_unicode(CString & input)
		{
			WORD w = 0;
			CString out, val;
			CStringW strW = CT2CW(input);
			for ( int i = 0, size = strW.GetLength(); i < size; ++i )
			{
				w = strW[i];
				out += _T("\\u");
				val.Format(_T("%02x%02x"), HIBYTE(w), LOBYTE(w));
				out += val;
			}
			return out;
		}

		static CStringA GetTagMD5(const CString & title, const CString & url, const BYTE * pixel, const UINT pixLen)
		{
			CStringA hash;
			if ( title.IsEmpty() || url.IsEmpty() || ( !pixel && pixLen ) )
			{
				return hash;
			}
			int urlLen = 0, titleLen = 0;
			char * purl = FromUnicode(CP_UTF8, url, &urlLen);
			char * ptitle = FromUnicode(CP_UTF8, title, &titleLen);
			DWORD len = urlLen + pixLen + titleLen;
			if ( char * pbuf = new char[len] )
			{
				memcpy(pbuf, ptitle, titleLen);
				if ( pixel && pixLen )
				{
					memcpy(pbuf + titleLen, pixel, pixLen);
				}
				memcpy(pbuf + titleLen + pixLen, purl, urlLen);
				char md5[61] = {0};
				CryptTool::GetMD5((const BYTE *)pbuf, len, md5);
				delete [] pbuf;
				hash = md5;
				hash.MakeLower();
			}
			delete [] purl;
			delete [] ptitle;
			return hash;
		}
	};
}
