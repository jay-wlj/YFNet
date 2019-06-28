#include "StdAfx.h"
#include <time.h>
#include "QhtpEn_De.h"
static const std::string base64_chars = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
CQhtpEn_De::CQhtpEn_De()
{
	ptemp = NULL;
}

CQhtpEn_De::~CQhtpEn_De()
{
	if(ptemp)
		delete []ptemp;
	ptemp = NULL;
}

#define   MAX		9 
#define   INSERT_	20

char *CQhtpEn_De::QhtpDecode(char *pstr)
{
	int nlen = strlen(pstr);
	destr(pstr, nlen);
	std::string decode = base64_decode(pstr);
	nlen = decode.size() + 1;
	ptemp = new char[nlen];
	memset(ptemp, 0, nlen);
	memcpy(ptemp, decode.c_str(), nlen -1 );
	char buffer2[2];
	memset(buffer2, 0, 2);
	memcpy(buffer2, (ptemp + INSERT_), 1);
	m_nDe = atoi(buffer2);
	char *p1 = ptemp+INSERT_+1;
	nlen = strlen(p1);
	char *p2 = new char[nlen+1];
	memset(p2, 0, nlen+1);
	memcpy(p2, p1, nlen);
	//memmove(ptemp+INSERT_, ptemp+INSERT_+1, strlen(ptemp+INSERT_+1));
	p1 = ptemp + INSERT_;
	int nlen1 = strlen(p1);
	memset(p1, 0, nlen1);
	strncpy(p1, p2, nlen);
	De_(ptemp);
	decode = base64_decode(ptemp);
	delete []ptemp;
	nlen1 = decode.size();
	ptemp = new char[nlen1+1];
	memset(ptemp, 0, nlen1+1);
	memcpy(ptemp, decode.c_str(), nlen1);
	delete []p2;
	return ptemp;
}

char *CQhtpEn_De::QhtpEncode(char *pstr)
{
	int nlen = strlen(pstr);
	std::string encode = base64_encode((unsigned char *)pstr, nlen);
	nlen = encode.size();
	char *p1 = new char[nlen + 2];
	memset(p1, 0, nlen+2);
	memcpy(p1, encode.c_str(), nlen);
	srand((unsigned)time(NULL));
	int nrand = rand();
	m_nDe = nrand % MAX;
	if(m_nDe < 3)
		m_nDe = 5;
	En_(p1);
	int nlen1 = strlen((p1 + INSERT_));
	char *p2 = new char[nlen1+1];
	memset(p2, 0, nlen1+1);
	memcpy(p2, (p1 + INSERT_), nlen1);
	char buffer2[2];
	memset(buffer2, 0, 2);
	itoa(m_nDe, buffer2, 10);
	strcpy((p1 + INSERT_), buffer2);
	strcpy((p1 + INSERT_ + 1), p2);
	nlen = strlen(p1);//strlen(pstr);//
	encode = base64_encode((unsigned char *)p1, nlen);
	nlen = encode.size();
	ptemp = new char[nlen + 1];
	memset(ptemp, 0, nlen+1);
	strcpy(ptemp, encode.c_str());
	enstr(ptemp, nlen);
	delete []p2;
	delete []p1;
	return ptemp;
}

bool CQhtpEn_De::is_base64(unsigned char c) 
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string CQhtpEn_De::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
			ret += '=';
	}
	return ret;
}

std::string CQhtpEn_De::base64_decode(std::string const& encoded_string)
{
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
	{
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4)
		{
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}
	return ret;
}

const char ab[10] = {2,1,3,4,2,1,2,0,1,4};

void CQhtpEn_De::enstr(char *pstr, int nlen)
{
	for (int i = 0; i<nlen; i++)
	{
		pstr[i] += ab[i%10];
		if(pstr[i] == '\\')
			pstr[i] = '_';
		else if(pstr[i] == '<')
			pstr[i] = '(';
		else if(pstr[i] == '>')
			pstr[i] = ')';
	}
}

void CQhtpEn_De::destr(char *pstr, int nlen)
{
	for (int i = 0; i<nlen; i++)
	{
		if(pstr[i] == '_')
			pstr[i] = '\\';
		else if(pstr[i] == '(')
			pstr[i] = '<';
		else if(pstr[i] == ')')
			pstr[i] = '>';
		pstr[i] -= ab[i%10];
	}
}

 

int CQhtpEn_De::in_(int   n)   
{   
	int   f1=1,f2=2,i=1;   
	while(f2+=f1,i++<n)
		f1=f2-f1;   
    return   f1;   
}   
void CQhtpEn_De::En_(char   *src)   
{   
	int   i,j=1,k=strlen(src),l=1;   
	char   *p,e;   
	for(i=1;i<=m_nDe;i++)   
	{   
		p=src;j=i;   
		while(1)   
		{   
			int   temp=in_(j++);   
            if(j>9)
				j=1;   
            if(p-src+temp>k)
				temp=k+src-p;   
            e=*(p+temp),*(p+temp)=0;   
            strrev(p);   
            while(*p)   
            {   
				if(temp>10)
					temp%=10;   
                if(i%2)
					*p+=temp;   
                else   
					*p-=temp;   
                p++;   
			}           
			*p=e;   
			if(!*p)
				break;   
		}           
	}     
    for(j=0;j<k;j++)   
    {   
		j%2?src[j]+=in_(l):src[j]-=in_(l);   
		if(++l==4)
			l=1;   
    }   
}       
void CQhtpEn_De::De_(char *src)                 
{   
	int   i,j=1,k=strlen(src),l=1;   
	char   *p=src,e,*q;   
	for(j=0;j<k;j++)   
	{   
		j%2?src[j]-=in_(l):src[j]+=in_(l);   
		if(++l==4)
			l=1;   
	}     
	j=1;   
	for(i=m_nDe;i>=1;i--)   
	{   
		p=src;j=i;   
		while(1)   
		{   
			int   temp=in_(j++);   
			if(j>9)
				j=1;   
			if(p-src+temp>k)
				temp=k+src-p;   
			q=p;   
			e=*(p+temp),*(p+temp)=0;   
			while(*p)   
			{   
				if(temp>10)
					temp%=10;   
				if(i%2)
					*p-=temp;   
				else   
					*p+=temp;   
				p++;   
			}   
			strrev(q);           
			*p=e;   
			if(!*p)
				break;   
		}           
	}   
} 

BOOL CQhtpEn_De::DecodeFile(LPCTSTR lpFileName, char *bufOut, UINT *nBufOutSize)
{
	WIN32_FIND_DATA fileInfo; 
	HANDLE hFind; 
	UINT nBufInSize=0;
	hFind = FindFirstFile(lpFileName,&fileInfo); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		nBufInSize = fileInfo.nFileSizeLow; //获得文件的大小(字节数）
	}
	FindClose(hFind);
	if(nBufInSize == 0)
	{
		return FALSE;
	}
	char *bufIn=new char[nBufInSize];
	if(bufIn == NULL)
	{
		return FALSE;
	}
	memset(bufIn, 0, nBufInSize);
	USES_CONVERSION;
	FILE *pf = NULL;
	fopen_s(&pf, T2CA(lpFileName), "rb");
	if(pf)
	{
		if(!fread(bufIn, 1, nBufInSize, pf))
		{
			fclose(pf);
			return FALSE;
		}
		fclose(pf);
		destr(bufIn, nBufInSize);
		std::string strde = bufIn;
		std::string strdestr = base64_decode(strde);
		*nBufOutSize = strdestr.length();
		strcpy(bufOut,(const char*)strdestr.c_str());
//		char *str = (char*)bufOut; //调试用
		delete []bufIn;
		return TRUE;
	}
	if(bufIn != NULL)
	{
		delete []bufIn;
	}
	return FALSE;
}