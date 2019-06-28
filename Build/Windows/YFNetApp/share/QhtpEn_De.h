#ifndef _QHTPEN_DE_H__
#define _QHTPEN_DE_H__

#include <string>

class CQhtpEn_De
{
public:
	CQhtpEn_De();
	~CQhtpEn_De();
	char *QhtpDecode(char *pstr);
	char *QhtpEncode(char *pstr);
	static bool is_base64(unsigned char c);
	std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	std::string base64_decode(std::string const& encoded_string);
	void enstr(char *pstr, int nlen);
	void destr(char *pstr, int nlen);

	BOOL DecodeFile(LPCTSTR lpFileName, char *bufOut, UINT *nBufOutSize);
private:
	int  in_(int n);
	void En_(char *src);
	void De_(char *src);
private:
	char *ptemp;
	int  m_nDe;
};
#endif