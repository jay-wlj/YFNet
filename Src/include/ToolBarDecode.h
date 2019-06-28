#pragma once

#ifdef __cplusplus  
extern "C" {  
#endif  
	/**
	 *对unc进行解码操作  
	 *@param encodeUnc 被编码的unc字符串 cstr
	 *@param decodeUnc 解码成功返回的unc串 cstr
	 *@return true false
	 */
   bool uin_decode(const char* encodeUnc, char * decodeUnc);
#ifdef __cplusplus  
}  
#endif  
