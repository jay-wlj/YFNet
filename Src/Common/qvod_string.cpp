
/*
 * QVOD String.
 */

#include "qvod_string.h"
#include <errno.h>
#include <string.h>
#include "common-commonfun.h"
#include "AutoPtr.h"


#ifdef WIN32 /* WIN32 */

//nothing to do

#else /* posix */


char* itoa(int value, char *str, int radix)
{
    static char r[1024] = {0};
	int  rem = 0;
	int  pos = 0;
	char ch  = '!';

	do{
		rem = value % radix;
		value /= radix;

		if(16 == radix)
        {
			if(rem >= 10 && rem <= 15)
            {
				switch( rem ) {
				case 10: ch = 'a'; break;
				case 11: ch = 'b'; break;
				case 12: ch = 'c'; break;
				case 13: ch = 'd'; break;
				case 14: ch = 'e'; break;
				case 15: ch = 'f'; break;
				}
			}
		}

		if('!' == ch)
        {
			str[pos++] = (char)(rem + 0x30);
		}
        else
        {
			str[pos++] = ch;
		}

	} while(value != 0);

	str[pos] = '\0';

    /* reverse */
    int i = strlen(str) - 1;
    int j = 0;
    for(; i >= 0; i--, j++)
    {
        r[j] = str[i];
    }
    r[j] = '\0';

    /* copy r to str */
    memcpy(str, r, strlen(r));

	return r;
}


int memcpy_s(void *dest, size_t numberOfElements, const void *src, size_t count)
{
    if(numberOfElements >= count) {
        memcpy(dest, src, count);
        return 0;
    }

	return -1;
}

int strcpy_s(char *dest, size_t numberOfElements, const char *src)
{
    size_t len = strlen(src) + 1; /* include 0 */
    if(numberOfElements >= len) {
        strcpy(dest, src);
        return 0;
    }

    return -1;
}



#endif /* posix end */




int qvod_strupr(char *str)
{
#ifdef WIN32 /* WIN32 */

    _strupr(str);

#else /* posix */

    char *tmp = str;

    while(*tmp != '\0') {

        if(*tmp > 96 && *tmp < 123) {

            *tmp = *tmp - 32;
        }

        ++tmp;
    }

#endif /* posix end */

    return QVOD_OK;
}

int qvod_lstrlenW(QVOD_LPCWSTR str)
{
#ifdef WIN32 /* WIN32 */

    return lstrlenW(str);

#else /* posix */

    return wcslen(str);

#endif /* posix end */
}

int qvod_lstrlen(QVOD_LPCTSTR str)
{
#ifdef WIN32 /*** WIN32 ***/

    return lstrlen(str);

#else /*** posix ***/

#ifdef _UNICODE  /* _UNICODE  */

    return wcslen(str);

#else /* non _UNICODE  */

    return strlen(str);

#endif /* non _UNICODE  end */

#endif /*** posix end ***/
}




/* ini file */
void LTruncate(char *pString, char *szFill)
{
    if(NULL == pString)
    {
        return;
    }

    int i, len;
    len = strlen(pString);

	QPtr::AutoArrayPtr<char> pTemp = new char[len + 1];
    strcpy(pTemp, pString);

    for(i = 0; i < len; i++) {

        if(!strchr(szFill, pTemp[i])) {
            break;
        }
    }

    strcpy(pString, pTemp + i);
}

void RTruncate(char *pString, char *szFill)
{
    if(NULL == pString)
    {
        return;
    }
    int i, len;
    len = strlen(pString);

    for(i = len; i > 0; i--) {

        if(!strchr(szFill, pString[i - 1])) {
            break;
        }
    }

    pString[i] = 0;
}


/*
 * error: return 0
 */
QVOD_DWORD QvodGetPrivateProfileString(QVOD_LPCTSTR lpAppName,
                                       QVOD_LPCTSTR lpKeyName,
                                       QVOD_LPCTSTR lpDefault,
                                       QVOD_LPTSTR lpReturnedString,
                                       QVOD_DWORD nSize,
                                       QVOD_LPCTSTR lpFileName)
{
#ifdef WIN32 /* WIN32 */

    return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);

#else /* posix */

    char *token;
    FILE *stream;
    char szBuffer[1024] = {0};
    char szAppSeps[] = "[";
    char szKeySeps[] = "=";
    QVOD_DWORD nLen;

    QVOD_BOOL bSectionFound = QVOD_FALSE;
    QVOD_BOOL bKeyFound = QVOD_FALSE;

    if((stream = fopen(lpFileName, "rb")) == NULL)
    {
#ifdef QVODDEBUG
        _OUTPUT(INFO, "%s(%d)-%s: fopen error %s %d\n", __FILE__, __LINE__, __FUNCTION__, lpFileName, errno);
#endif
        return 0; /* error */
    }


    while(!feof(stream))
    {

        if(fgets(szBuffer, 1024, stream) == NULL)
            break;

        LTruncate(szBuffer, " \t");/*去掉前导空格和TAB*/

        if(!bSectionFound)
        {
            token = strtok(szBuffer, szAppSeps);

            if(token == szBuffer || NULL == token) {
                continue; /* 继续寻找 */
            }

            LTruncate(token, " \t");
            RTruncate(token, " ]\t\r\n");

            if(strcmp(token, lpAppName)) {
                continue;  /* 找下一个匹配Section */
            } else {
                bSectionFound = QVOD_TRUE;
            }

        } else {

            nLen = strlen(szBuffer);
            token = strtok(szBuffer, szKeySeps);
            if(NULL == token)
            {
                continue;
            }
            if(strlen(token) == nLen) {   /* 判断是否离开指定的Section */
                token = strtok(szBuffer, szAppSeps);
                if(token != szBuffer) {
                    break;                /* 已离开，退出查找 */
                } else {
                    continue;        /* 未离开，继续寻找 */
                }
            }

            LTruncate(szBuffer, " \t");
            RTruncate(szBuffer, " \t");

            if(strcmp(szBuffer, lpKeyName)) {
                continue;

            } else {   /* 找到 */

                bKeyFound = QVOD_TRUE;

                token = strtok(NULL, szKeySeps);
                if(NULL == token)
                {
                    continue;
                }

                LTruncate(token, " \t");
                RTruncate(token, " ;,\t\n\r");
                nLen = strlen(token);

                if(nLen <  nSize) {
                    strcpy((char*)lpReturnedString, token);

                } else {

                    nLen = nSize - 1;
                    strncpy((char*)lpReturnedString, token, nSize-1);
                    lpReturnedString[nSize-1] = 0;
                }

                break;
            }
        }
    } /* while */

    fclose(stream);
    if(bSectionFound && bKeyFound)
    {
        return nLen; /* ok */
    }

    if(lpDefault!=NULL) {

        nLen = strlen(lpDefault);

        if(nLen < nSize) {
            strcpy((char*)lpReturnedString, lpDefault);

        } else {
            strncpy((char*)lpReturnedString, lpDefault, nSize-1);
            lpReturnedString[nSize-1] = 0;
        }
    }

    return 0; /* error */

#endif /* posix end */
}

QVOD_UINT QvodGetPrivateProfileInt(QVOD_LPCTSTR lpAppName,
								   QVOD_LPCTSTR lpKeyName,
								   int nDefault,
								   QVOD_LPCTSTR lpFileName)
{
#ifdef WIN32 /* WIN32 */

    return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, lpFileName);

#else /* posix */

    int res;
    QVOD_UINT ret;
    char buf[256] = {0};

    if(nDefault < 0) {
        ret = 0;

    } else {
        ret = nDefault;
    }

    memset(buf, 0, 256);
    res = QvodGetPrivateProfileString(lpAppName, lpKeyName, "-1", buf, 256, lpFileName);
    if(res == 0 && strcmp(buf, "-1") == 0) 
    {
        return nDefault;
    }

    ret = atoi(buf);

    return ret;

#endif /* posix end */
}

/*
 * error: return 0
 */
QVOD_BOOL QvodWritePrivateProfileString(QVOD_LPCTSTR lpAppName,
                                        QVOD_LPCTSTR lpKeyName,
                                        QVOD_LPCTSTR lpString,
                                        QVOD_LPCTSTR lpFileName)
{
#ifdef WIN32 /* WIN32 */

    return WritePrivateProfileString(lpAppName, lpKeyName, lpString, lpFileName);

#else /* posix */

    char *token, *pLineToken;
    FILE *stream;
    char szBuffer[1024] = {0};
    char szAppSeps[] = "[";
    char szKeySeps[] = "=";
    QVOD_DWORD nLen;
    char *pBuf;
    char *pSectionPos, *pKeyPos;
    unsigned long ulFileLen, i, k;

    QVOD_BOOL bSectionFound = QVOD_FALSE;
    QVOD_BOOL bKeyFound = QVOD_FALSE;

    if((stream = fopen(lpFileName, "r+b")) == NULL)
    {
        /* file not exist, create it */
		stream = fopen(lpFileName, "w+b");
        if(stream == NULL)
        {
#ifdef QVODDEBUG
            _OUTPUT(FAIL, "%s(%d)-%s: fopen %s error %d\n", __FILE__, __LINE__, __FUNCTION__, lpFileName, errno);
#endif
            return 0;
        }
    }

    pBuf = szBuffer;

    fseek(stream, 0, SEEK_END);
    ulFileLen = ftell(stream);

    QPtr::AutoArrayPtr<char> pFileBuf =new char[ulFileLen + 1];
    memset(pFileBuf, 0, ulFileLen + 1);
    fseek(stream, 0, SEEK_SET);
    fread(pFileBuf, ulFileLen, 1, stream);
    fclose(stream);

    pLineToken = pFileBuf;
    //printf("pLineToken = %s\n", pLineToken);
    while(pLineToken) {

        if(!bSectionFound) {
            pSectionPos = pLineToken;

        } else {
            pKeyPos = pLineToken;
        }

        pLineToken = strtok(pLineToken, "\n");

        if(!pLineToken) {
            break;
        }
        strcpy(pBuf, pLineToken);

        pLineToken += strlen(pLineToken) + 1;

        LTruncate(pBuf, " \t");

        token = strstr(szBuffer, "//");
        if(token) token[0] = 0;

        if((pBuf = strchr(szBuffer, '\\')) != NULL)
        {
            if(pBuf[1]!='x' && pBuf[1]!='X') {
                continue;
            }
        }
        pBuf = szBuffer;

        if(strlen(szBuffer)==0) {
            continue;
        }

        if(!bSectionFound)
        {
            token = strtok(szBuffer, szAppSeps);

            if(token == szBuffer || NULL == token) {
                continue;
            }

            LTruncate(token, " \t");
            RTruncate(token, " ]\t\r\n");
            if(strcasecmp(token, lpAppName)) { /* strcompi, silver */
                continue;
            } else {
                bSectionFound = QVOD_TRUE;
            }

        } else {

            nLen = strlen(szBuffer);
            token = strtok(szBuffer, szKeySeps);
            if(NULL == token)
            {
                continue;
            }
            if(strlen(token) == nLen)
            {   /* 判断是否离开指定的Section */
                token = strtok(szBuffer, szAppSeps);
                if(token != szBuffer) {
                    break;
                } else {
                    continue;
                }
            }

            LTruncate(szBuffer, " \t");
            RTruncate(szBuffer, " \t");

            if(strcasecmp(szBuffer, lpKeyName)) { /* strcompi, silver */
                continue;

            } else {   /* 找到 */

                bKeyFound = QVOD_TRUE;
                break;
            }
        }
    } /* while */

    if((stream = fopen( lpFileName, "wb" )) == NULL)
    {
        /* error */
        return 0;
    }

    if(!bSectionFound && !bKeyFound) {

        for(i = 0; i < ulFileLen; i++) {
            if(pFileBuf[i]==0) pFileBuf[i]='\n';
        }
        fwrite(pFileBuf, i, 1, stream);

        fseek(stream, 0, SEEK_END);
        sprintf(szBuffer, "[%s]\n", lpAppName);
        fwrite(szBuffer, strlen(szBuffer), 1, stream);
        sprintf(szBuffer, "%s=%s\n", lpKeyName, lpString);
        fwrite(szBuffer, strlen(szBuffer), 1, stream);

    } else if(bSectionFound && bKeyFound) {

        for(i=0;pFileBuf+i<pKeyPos;i++) {
            if(pFileBuf[i]==0) pFileBuf[i]='\n';
        }
        fwrite(pFileBuf, i, 1, stream);

        sprintf(szBuffer, "%s=%s\n", lpKeyName, lpString);
        fwrite(szBuffer, strlen(szBuffer), 1, stream);

        k=0;
        for(i += (strlen(pKeyPos) + 1); i < ulFileLen; i++) {
            if(pFileBuf[i]==0) pFileBuf[i]='\n';
            k++;
        }
        fwrite(pKeyPos + strlen(pKeyPos) + 1, k, 1, stream);

    } else if(bSectionFound && !bKeyFound) {

        for(i = 0; pFileBuf + i < pKeyPos; i++)
        {
            if(pFileBuf[i]==0) pFileBuf[i]='\n';
        }
        fwrite(pFileBuf, i, 1, stream);

        sprintf(szBuffer, "%s=%s\n", lpKeyName, lpString);
        fwrite(szBuffer, strlen(szBuffer), 1, stream);

        k=0;
        for(;i<ulFileLen;i++,k++) {
            if(pFileBuf[i]==0) pFileBuf[i]='\n';
        }
        fwrite(pKeyPos, k, 1, stream);
    }

    fclose(stream);
    return 1; /* ok */

#endif /* posix end */
}

/*
 * save data to odd address, for embeded systems.
 */
int QvodWriteToOddAddress(unsigned char *oddAddress, unsigned char *data, int length)
{
    int i = 0;

    for(i = 0; i < length; ++i)
    {
        *(unsigned char*)(oddAddress + i) = *(unsigned char*)(data + i);
    }

    return 0;
}


/*
 * read data from odd address, for embeded systems.
 */
int QvodReadFromOddAddress(unsigned char *oddAddress, unsigned char *data, int length)
{
    int i = 0;

    for(i = 0; i < length; ++i)
    {
        *(unsigned char*)(data + i) = *(unsigned char*)(oddAddress + i);
    }

    return 0;
}

int QvodStrICmp(const char * src, const char* dest)
{
#ifdef WIN32
	return stricmp(src, dest);
#else
	return strcasecmp(src, dest);
#endif
}

int QvodStrInCmp(const char * src, const char* dest, const int &len)
{
#ifdef WIN32
	return strnicmp(src, dest, len);
#else
	return strncasecmp(src, dest, len);
#endif
}
string QvodFilterCharacters(const char *src, const char *pDelCharacters)
{
	string strRet;
	if (NULL == src) {
		return strRet;
	}
	if (NULL == pDelCharacters) {
		return string(src);
	}
	strRet = src;
	const char *pDel = pDelCharacters;
	while ((*pDel) != '\0') {
		const char c = *pDel;
		int npos = strRet.find(c);
		while (npos != string::npos)
		{
			strRet.erase(strRet.begin()+npos);
			npos = strRet.find(c);
		}
		pDel++;
	}
	return strRet;
}

string string_trim(std::string& str, const char *strTrim)
{
	if(str.empty())
	{
		return str;
	}
	str.erase(0, str.find_first_not_of(strTrim));
	str.erase(str.find_last_not_of(strTrim)+1);
	return str;
}