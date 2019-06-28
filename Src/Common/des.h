/*********************************************************
 *  des.h
 *  用户使用des算法头文件
 *	
 *********************************************************/
#ifndef _OPENDESS_H_
#define _OPENDESS_H_

class OpenDes
{
public:
	//用户使用的函数
	static int DesEnc(
			unsigned char *pInData,
			int            nInDataLen,
			unsigned char *pOutData,
			int           *pOutDataLen,
			unsigned char *pKey = NULL,
			int			   nKeyLen = 8);

	//用户使用函数des解密
	static int DesDec(
		   unsigned char *pInData,
		   int            nInDataLen,
		   unsigned char *pOutData,
		   int           *pOutDataLen,
		   unsigned char *pKey = NULL,
		   int			   nKeyLen = 8);

};
#endif

