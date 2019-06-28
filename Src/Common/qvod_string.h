
/*
 * QVOD String.
 */

#ifndef _QVOD_STRING_H_
#define _QVOD_STRING_H_



#include "qvod_basetypes.h"


#ifdef WIN32 /***************** WIN32 *****************/


#include <string.h>
#include <string>
#include <stdlib.h>
#include <tchar.h>
using std::string;

#define QVOD_ATOI			atoi
#define QVOD_ATOI64			_atoi64

/* TCHAR */
typedef LPVOID				QVOD_LPVOID;
typedef LPCVOID 			QVOD_LPCVOID;
typedef LPCSTR				QVOD_LPCSTR;
typedef LPCTSTR				QVOD_LPCTSTR;
typedef LPTSTR				QVOD_LPTSTR;
typedef LPSTR				QVOD_LPSTR;
typedef LPCWSTR				QVOD_LPCWSTR;


#else /***************** posix *****************/


#include <string.h>
#include <string>
#include <stdlib.h>
#include <wchar.h>
#include <ctype.h>
using std::string;
//#include <iconv.h>

#define QVOD_ATOI			atoi
#define QVOD_ATOI64			atoll


#ifdef _UNICODE  /* _UNICODE  */

typedef wchar_t 			TCHAR;
typedef void*				QVOD_LPVOID;
typedef const void* 		QVOD_LPCVOID;
typedef const char*			QVOD_LPCSTR;
typedef const wchar_t*		QVOD_LPCTSTR;
typedef wchar_t*			QVOD_LPTSTR;
typedef char*				QVOD_LPSTR;

#else /* non _UNICODE  */

typedef char 				TCHAR;
typedef void*				QVOD_LPVOID;
typedef const void* 		QVOD_LPCVOID;
typedef const char*			QVOD_LPCSTR;
typedef const char*			QVOD_LPCTSTR;
typedef char*				QVOD_LPTSTR;
typedef char*				QVOD_LPSTR;

#endif /* non _UNICODE  end */


typedef const wchar_t*		QVOD_LPCWSTR;


char* itoa(int value, char* str, int radix);
int memcpy_s(void *dest, size_t numberOfElements, const void *src, size_t count);
int strcpy_s(char *dest, size_t numberOfElements, const char *src);

#define sprintf_s snprintf /* need modify */


#endif /***************** posix end *****************/



int qvod_strupr(char *str);
int qvod_lstrlenW(QVOD_LPCWSTR str);
int qvod_lstrlen(QVOD_LPCTSTR str);
int QvodWriteToOddAddress(unsigned char *oddAddress, unsigned char *data, int length);
int QvodReadFromOddAddress(unsigned char *oddAddress, unsigned char *data, int length);

int QvodStrICmp(const char * src, const char* dest);		// 忽略大小写比较
int QvodStrInCmp(const char * src, const char* dest, const int &len);		// 忽略大小写比较
string QvodFilterCharacters(const char *src, const char *pDelCharacters); // 删除特定字符
string string_trim(std::string& str, const char *strTrim=" ");
/* INI file */
void LTruncate(char *pString, char *szFill);
void RTruncate(char *pString, char *szFill);
QVOD_DWORD QvodGetPrivateProfileString(QVOD_LPCTSTR lpAppName,
									   QVOD_LPCTSTR lpKeyName,
									   QVOD_LPCTSTR lpDefault,
									   QVOD_LPTSTR lpReturnedString,
									   QVOD_DWORD nSize,
									   QVOD_LPCTSTR lpFileName);
QVOD_UINT QvodGetPrivateProfileInt(QVOD_LPCTSTR lpAppName,
								   QVOD_LPCTSTR lpKeyName,
								   int nDefault,
								   QVOD_LPCTSTR lpFileName);
QVOD_BOOL QvodWritePrivateProfileString(QVOD_LPCTSTR lpAppName,
                                        QVOD_LPCTSTR lpKeyName,
                                        QVOD_LPCTSTR lpString,
                                        QVOD_LPCTSTR lpFileName);



#endif /* _QVOD_STRING_H_ */



