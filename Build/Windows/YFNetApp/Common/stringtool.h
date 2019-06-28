// 描述    ：字符串处理类
// 当前版本：
// 作    者：
// 完成日期：
#ifndef _STRING_HANDLE_H_H__
#define _STRING_HANDLE_H_H__


#include <atlstr.h>
#include <string>
class Convert
{
public:
	static CString ToString(bool _Value)
	{
		return (_Value ? _T("1"): _T("0"));
	}

	static CString ToString(char _Value, int _Radix = 10)
	{
		return ToString(_Radix==10 ? (int)_Value : (int)(unsigned char)_Value, _Radix); // 仅10进制区分符号
	}

	static CString ToString(short _Value, int _Radix = 10)
	{
		return ToString(_Radix==10 ? (int)_Value : (int)(unsigned short)_Value, _Radix);
	}

	static CString ToString(long _Value, int _Radix = 10)
	{
		TCHAR tcResult[35] = {0};
		_ltot_s(_Value, tcResult, 35, _Radix);
		return CString(tcResult);
	}

	static CString ToString(int _Value, int _Radix = 10)
	{
		TCHAR tcResult[35] = {0};
		_itot_s(_Value, tcResult, 35, _Radix);
		return CString(tcResult);
	}

	static CString ToString(__int64 _Value, int _Radix = 10)
	{
		TCHAR tcResult[70] = {0};
		_i64tot_s(_Value, tcResult, 35, _Radix);
		return CString(tcResult);
	}

	static CString ToString(float _Value)
	{
		CString _Result;
		_Result.Format(_T("%f"), _Value);
		return _Result;
	}

	static CString ToString(float _Value, int _DecimalPos)
	{
		CString _Result, _Format;
		_Format.Format(_T("%%.%uf"), _DecimalPos);
		_Result.Format(_Format, _Value);
		return _Result;
	}

	static CString ToString(double _Value)
	{
		CString _Result;
		_Result.Format(_T("%lf"), _Value);
		return _Result;
	}

	static CString ToString(double _Value, int _DecimalPos)
	{
		CString _Result, _Format;
		_Format.Format(_T("%%.%ulf"), _DecimalPos);
		_Result.Format(_Format, _Value);
		return _Result;
	}

	static CString ToString(unsigned char _Value, int _Radix = 10)
	{
		return ToString((int)_Value, _Radix);
	}

	static CString ToString(unsigned short _Value, int _Radix = 10)
	{
		return ToString((int)_Value, _Radix);
	}

	static CString ToString(unsigned long _Value, int _Radix = 10)
	{
		TCHAR tcResult[35] = {0};
		_ultot_s(_Value, tcResult, 35, _Radix);
		return CString(tcResult);
	}

	static CString ToString(unsigned int _Value, int _Radix = 10)
	{
		return ToString((unsigned long)_Value, _Radix);
	}

	static CString ToString(unsigned __int64 _Value, int _Radix = 10)
	{
		TCHAR tcResult[70] = {0};
		_ui64tot_s(_Value, tcResult, 70, _Radix);
		return CString(tcResult);
	}
	
	static CString ToString(const std::string& string_)
	{
		return ToString(CStringA(string_.c_str(),string_.size()));
	}
	

#ifdef _UNICODE
	static CString ToString(const CStringA& _AString)
	{
		return ToWString(_AString);
	}

	static CString ToString(const CStringW& _WString)
	{
		return _WString;
	}

#else // !defined(_UNICODE)
	static CString ToString(const CStringA& _AString)
	{
		return _AString;
	}

	static CString ToString(const CStringW& _WString)
	{
		return ToAString(_WString);
	}
#endif

	static CStringA ToAString(const CStringW& _WString)
	{
		CStringA strMultiByte;
		if(_WString.GetLength() > 0)
		{
			int nLength = WideCharToMultiByte(CP_ACP, NULL, (LPCWSTR)_WString, -1, NULL, 0, NULL, NULL);
			char* pBuffer = new char[nLength+1];
			ZeroMemory(pBuffer, nLength+1);
			WideCharToMultiByte(CP_OEMCP, 0, (LPCWSTR)_WString, -1, pBuffer, nLength, NULL, NULL);
			strMultiByte = pBuffer;
			delete[] pBuffer;
		}
		return strMultiByte;
	}

	static CStringA ToAString(const CStringA& _AString)
	{
		return _AString;
	}

	static CStringW ToWString(const CStringA& _AString)
	{
		CStringW strWideChar;
		if(_AString.GetLength() > 0)
		{
			int nLength = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)_AString, -1, NULL, 0);
			wchar_t* pwBuffer = new wchar_t[nLength+1];
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)_AString, -1, pwBuffer, nLength);
			pwBuffer[nLength] = 0;
			strWideChar = pwBuffer;
			delete[] pwBuffer;
		}
		return strWideChar;
	}

	static CStringW ToWString(const CStringW& _WString)
	{
		return _WString;
	}

	template<typename T>
	static CString ToHexString(T _Value, bool isUpper = true)
	{
		CString _Result = ToString(_Value, 16);
		if(isUpper)
		{
			_Result.MakeUpper();
		}
		else
		{
			_Result.MakeLower();
		}
		return _Result;
	}

	static int ToInt32(CString _Value, int _Radix = 10)
	{
		return _tcstol(_Value, NULL, _Radix);
	}

	static __int64 ToInt64(const CString& _Value, int _Radix = 10)
	{
		return _tcstoi64(_Value, NULL, _Radix);
	}

	static unsigned int ToUint32(const CString& _Value, int _Radix = 10)
	{
		return _tcstoul(_Value, NULL, _Radix);
	}

	static unsigned int ToUint32(const TCHAR* _Value, int _Radix = 10)
	{
		return _tcstoul(_Value, NULL, _Radix);
	}

	static unsigned __int64 ToUint64(const CString& _Value, int _Radix = 10)
	{
		return _tcstoui64(_Value, NULL, _Radix);
	}
};

class StringCode
{
public:
	static CStringW DecodeUtf8(const CStringA &utf8)
	{
		CStringW unicode;
		if(utf8.GetLength() > 0)
		{
			int nLength = MultiByteToWideChar(CP_UTF8, NULL, (LPCSTR)utf8, -1, NULL, 0);
			wchar_t* pwBuffer = new wchar_t[nLength+1];
			ZeroMemory(pwBuffer, (nLength+1)*2);
			MultiByteToWideChar(CP_UTF8, NULL, (LPCSTR)utf8, -1, pwBuffer, nLength);
			unicode = pwBuffer;
			delete[] pwBuffer;
		}
		return unicode;
	}

	static CStringW DecodeUrlOfUtf8(const CStringA& urlOfUtf8)
	{
		CStringW unicode;
		if(urlOfUtf8.GetLength() > 0)
		{
			CStringA uft8;
			for(int i = 0; i < urlOfUtf8.GetLength() ; i++)
			{
				char c = urlOfUtf8.GetAt(i);
				if('%' == c)
				{
					CString strHex = Convert::ToString(urlOfUtf8.Mid(i+1, 2));
					uft8 += (char)Convert::ToInt32(strHex,16);
					i += 2;
				}
				else
				{
					uft8 += c;
				}
			}
			unicode = DecodeUtf8(uft8);
		}
		return unicode;
	}

	static CStringA ToUtf8(const CStringW &unicode)
	{
		CStringA utf8;
		if(unicode.GetLength() > 0)
		{
			int nLength = WideCharToMultiByte(CP_UTF8, NULL, (LPCWSTR)unicode, -1, NULL, 0 ,NULL ,NULL);
			char* pBuffer = new char[nLength+1];
			ZeroMemory(pBuffer, nLength+1);
			WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)unicode, -1, pBuffer, nLength ,NULL ,NULL);
			utf8 = pBuffer;
			delete[] pBuffer;
		}
		return utf8;
	}

	static CStringA ToUrlOfUtf8(const CStringW& source, char *szExcept = NULL)   
	{
		CStringA url, temp;
		CStringA uft8 = ToUtf8(source);
		CStringA except = szExcept;
		for(int i = 0; i < uft8.GetLength(); i++)
		{
			unsigned char uc = uft8.GetAt(i);
			if(isalnum(uc) || '.' == uc || '/' == uc || except.Find(uc) > -1)
			{
				url += CStringA((char)uc,1);
			}
			else
			{
				temp.Format("%%%02X", uc);
				url += temp;
			}
		}
		return url;   
	}

	static std::string URLEncode(const std::string & input)
	{
		std::string ret;
		char ch, per[4] = {0};
		if ( input.empty() )
		{
			return ret;
		}

		for ( int i = 0, size = input.size(); i < size; ++i )
		{
			ch = input[i];
			if ( ch == '.' || ch == '-' || ch == '_' || ch == '*'
				|| ( (ch>='A') && (ch<='Z') ) || ( (ch>='a') && (ch<='z') ) || ( (ch>='0') && (ch<='9') ) )
			{
				ret += ch;
			}
			else if (ch == ' ')
			{
				ret += '+';
			}
			else
			{
				sprintf( per, "%%%02X", (unsigned char)ch);
				ret += per;
			}
		}
		return ret;
	}
};

#endif