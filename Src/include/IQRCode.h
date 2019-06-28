#pragma once
// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� QRCODE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// QRCODE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef QRCODE_EXPORTS
#define QRCODE_API __declspec(dllexport) 
#else
#define QRCODE_API  __declspec(dllimport) 
#endif

interface  IQRCode
{
public:
	// ���ɶ�ά��
	/***************************************************************
		������:     QREncode
		��������:   ���ɶ�ά��
		����1��     const char* str, ��ά������
		����2��     int nSize����ά���С�����أ�
		����3��     CString strLogoPath, ͼ��·��
		����4��     int nLevel���ݴ�ȼ�
		����5��     int nVersion�������ȼ���Ĭ��
		����6��     BOOL bAutoExtent���Ƿ��Զ���չ�����ȼ�
		����1��     int nMaskingNo��
		����2��     int ncSource
		����ֵ��    �ɹ�����TRUE������Ϊʧ��
	*****************************************************************/
	virtual Bitmap* QREncode(const char* str, int nSize = 0, CString strLogoPath = _T(""), 
		int nLevel = 0, int nVersion = 0, BOOL bAutoExtent = 1, int nMaskingNo = -1, int ncSource = 0) = 0;
	virtual Bitmap* QREncodeDefault(const char* str, int nSize) = 0; 
	// ������ά��
	virtual CString QRDecodeByBmp(Bitmap* pBmp) = 0;
	virtual CString QRDecodeByPath(CString strPath) = 0;
	virtual void Release() = 0;
};


QRCODE_API IQRCode* CreateQRCode();

QRCODE_API void ReleaseQRCode(IQRCode* pIQR);