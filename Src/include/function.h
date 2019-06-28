#ifndef _FUNCTIONAL_H_
#define _FUNCTIONAL_H_

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


#ifndef BYTE
#define BYTE unsigned char
#endif

class Fun
{
public:
	// 转换为UTF-8
	static string EncodeToUTF8(const char* szSource)   
	{
		string   sFinal; 
		if (NULL == szSource)
		{
			return sFinal;
		}
		char sTemp[50] = {0};
		unsigned short    ch;  
		unsigned char   bt1,   bt2,   bt3,   bt4,   bt5,   bt6;
		int   n,   nMax   =   strlen(szSource);   
		
		for(n = 0; n < nMax; ++n)   
		{   
			ch   =   (unsigned short )szSource[n];  
			if   (ch   <   128)   
			{
				if((ch >= 48 && ch <= 57) || (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 121) || ch == 46 )
				{
					sFinal   +=   szSource[n];
				}
				else
				{
					sprintf(sTemp, "%%%02X", ch);
					sFinal   +=   sTemp;   
				}
			}   
			else   if   (ch   <=   2047)
			{   
				bt1   =   (unsigned char)(192   +   (ch   /   64));  
				bt2   =   (unsigned char)(128   +   (ch   %   64));  
				sprintf(sTemp, "%%%02X%%%02X",   bt1,   bt2);
				sFinal   +=   sTemp;   
			}   
			else   if   (ch   <=   65535)   
			{   
				bt1   =   (unsigned char)(224   +   (ch   /   4096));   
				bt2   =   (unsigned char)(128   +   ((ch   /   64)   %   64));   
				bt3   =   (unsigned char)(128   +   (ch   %   64));   
				sprintf(sTemp, "%%%02X%%%02X%%%02X",   bt1,   bt2,   bt3);   
				sFinal   +=   sTemp;   
			}   
			else   if   (ch   <=   2097151)   
			{   
				bt1   =   (unsigned char)(240   +   (ch   /   262144));   
				bt2   =   (unsigned char)(128   +   ((ch   /   4096)   %   64));   
				bt3   =   (unsigned char)(128   +   ((ch   /   64)   %   64));   
				bt4   =   (unsigned char)(128   +   (ch   %   64));   
				sprintf(sTemp, "%%%02X%%%02X%%%02X",   bt1,   bt2,   bt3);   
				sFinal   +=   sTemp;   
			}   
			else   if   (ch   <=67108863)   
			{   
				bt1   =   (unsigned char)(248   +   (ch   /   16777216));   
				bt2   =   (unsigned char)(128   +   ((ch   /   262144)   %   64));   
				bt3   =   (unsigned char)(128   +   ((ch   /   4096)   %   64));   
				bt4   =   (unsigned char)(128   +   ((ch   /   64)   %   64));   
				bt5   =   (unsigned char)(128   +   (ch   %   64));   
				sprintf(sTemp, "%%%02X%%%02X%%%02X%%%02X%%%02X",   bt1,   bt2,   bt3,   bt4,   bt5);   
				sFinal   +=   sTemp;   
			}   
			else   if   (ch   <=2147483647)   
			{   
				bt1   =   (unsigned char)(252   +   (ch   /   1073741824));   
				bt2   =   (unsigned char)(128   +   ((ch   /   16777216)   %   64));   
				bt3   =   (unsigned char)(128   +   ((ch   /   262144)   %   64));   
				bt4   =   (unsigned char)(128   +   ((ch   /   4096)   %   64));   
				bt5   =   (unsigned char)(128   +   ((ch   /   64)   %   64));   
				bt6   =   (unsigned char)(128   +   (ch   %   64));   
				sprintf(sTemp, "%%%02X%%%02X%%%02X%%%02X%%%02X%%%02X",     
					bt1,   bt2,   bt3,   bt4,   bt5,   bt6);   
				sFinal   +=   sTemp;   
			}
		}   
		return   sFinal;   
	} 
	

	template<class T> static bool MoveElement(T& vSrc, std::vector<unsigned int>& vMoveIndex, int nPos)
	{
		//int nSize = vSrc.size();
		if (nPos >= vSrc.size())
		{
			return false;
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
	template<class T> static bool Remove(std::vector<T>& v, T t)
	{
		for (int i = 0; i < v.size(); i++)
		{
			if (v[i] == t)
			{
				v.erase(v.begin() + i);
				return true;
			}
		}
		return false;
	}


	static std::string URLEncode(const char* str, const int len)
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
			else
			{
				sprintf(szTemp, "%%%02X", (unsigned char)ch);
				strRet += szTemp;
			}
		}

		return strRet;
	}

	static std::string URLDecode(const char* str, const unsigned int len)
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
};

#endif