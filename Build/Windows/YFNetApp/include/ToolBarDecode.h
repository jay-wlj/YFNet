#pragma once

#ifdef __cplusplus  
extern "C" {  
#endif  
	/**
	 *��unc���н������  
	 *@param encodeUnc �������unc�ַ��� cstr
	 *@param decodeUnc ����ɹ����ص�unc�� cstr
	 *@return true false
	 */
   bool uin_decode(const char* encodeUnc, char * decodeUnc);
#ifdef __cplusplus  
}  
#endif  
