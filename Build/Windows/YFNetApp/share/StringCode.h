#pragma once

class Base64
{
public:
	static const std::string base64_chars;
	static bool is_base64(unsigned char c) 
	{
		return (isalnum(c) || (c == '+') || (c == '/'));
	}
	static std::string base64_encode(char const* bytes_to_encode, unsigned int in_len)
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
				{
					ret += base64_chars[char_array_4[i]];
				}
				i = 0;
			}
		}

		if(i)
		{
			for(j = i; j < 3; j++)
			{
				char_array_3[j] = '\0';
			}

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
			{
				ret += base64_chars[char_array_4[j]];
			}

			while((i++ < 3))
			{
				ret += '=';
			}
		}
		return ret;
	}

	static std::string base64_decode(std::string const& encoded_string)
	{
		int in_len = encoded_string.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
		{
			char_array_4[i++] = encoded_string[in_];
			in_++;
			if (i ==4)
			{
				for (i = 0; i <4; i++)
				{
					char_array_4[i] = base64_chars.find(char_array_4[i]);
				}

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
				{
					ret += char_array_3[i];
				}
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j <4; j++)
			{
				char_array_4[j] = 0;
			}

			for (j = 0; j <4; j++)
			{
				char_array_4[j] = base64_chars.find(char_array_4[j]);
			}

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++)
			{
				ret += char_array_3[j];
			}
		}
		return ret;
	}
};

__declspec(selectany) const std::string Base64::base64_chars = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static std::string EscapeUTF8ToUnicode(std::string &src)
{
	std::string result;
	int len = src.length();
	const char *p = src.c_str();
	const char *pEnd = p + len;
	BYTE uChar;
	char szUnicode[8] = {0};

	while(p<pEnd)
	{
		const char *pHead = p;
		while(((BYTE)*p < 0xC0) && (p<pEnd))
		{
			//result += *p;
			p++;
		}
		if(p != pHead)
		{
			result.append(pHead, p - pHead);
		}

		if(p>=pEnd)
		{
			break;
		}

		if((BYTE)*p >= 0xC0)
		{
			result.append("\\u");
			if((BYTE)*p <= 0xDF) // 2 bytes
			{
				uChar = ((*p) >> 2) & 0x07;
				sprintf_s(szUnicode, "%02X", uChar);
				result += szUnicode;

				uChar = ((*p) << 6) | (*(p + 1) & 0x3F);
				sprintf_s(szUnicode, "%02X", uChar);
				result += szUnicode;
				p += 2;
			}
			else // 3 bytes
			{
				uChar = ((*p) << 4) | ((*(p + 1) >> 2) & 0x0F);
				sprintf_s(szUnicode, "%02X", uChar);
				result += szUnicode;
				p++;

				uChar = ((*p) << 6) | (*(p+1) & 0x3F);
				sprintf_s(szUnicode, "%02X", uChar);
				result += szUnicode;
				p += 2;
			}
		}
	}
	return result;
}
