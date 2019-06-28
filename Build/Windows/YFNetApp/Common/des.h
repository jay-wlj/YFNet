/*********************************************************
 *  des.h
 *  �û�ʹ��des�㷨ͷ�ļ�
 *	
 *********************************************************/
#ifndef _OPENDESS_H_
#define _OPENDESS_H_

class OpenDes
{
public:
	//�û�ʹ�õĺ���
	static int DesEnc(
			unsigned char *pInData,
			int            nInDataLen,
			unsigned char *pOutData,
			int           *pOutDataLen,
			unsigned char *pKey = NULL,
			int			   nKeyLen = 8);

	//�û�ʹ�ú���des����
	static int DesDec(
		   unsigned char *pInData,
		   int            nInDataLen,
		   unsigned char *pOutData,
		   int           *pOutDataLen,
		   unsigned char *pKey = NULL,
		   int			   nKeyLen = 8);

};
#endif

