#pragma once
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 QRCODE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// QRCODE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef QRCODE_EXPORTS
#define QRCODE_API __declspec(dllexport) 
#else
#define QRCODE_API  __declspec(dllimport) 
#endif

interface  IQRCode
{
public:
	// 生成二维码
	/***************************************************************
		函数名:     QREncode
		函数描述:   生成二维码
		参数1：     const char* str, 二维码内容
		参数2：     int nSize，二维码大小（像素）
		参数3：     CString strLogoPath, 图标路径
		参数4：     int nLevel，容错等级
		参数5：     int nVersion，容量等级，默认
		参数6：     BOOL bAutoExtent，是否自动扩展容量等级
		参数1：     int nMaskingNo，
		参数2：     int ncSource
		返回值：    成功返回TRUE，否则为失败
	*****************************************************************/
	virtual Bitmap* QREncode(const char* str, int nSize = 0, CString strLogoPath = _T(""), 
		int nLevel = 0, int nVersion = 0, BOOL bAutoExtent = 1, int nMaskingNo = -1, int ncSource = 0) = 0;
	virtual Bitmap* QREncodeDefault(const char* str, int nSize) = 0; 
	// 解析二维码
	virtual CString QRDecodeByBmp(Bitmap* pBmp) = 0;
	virtual CString QRDecodeByPath(CString strPath) = 0;
	virtual void Release() = 0;
};


QRCODE_API IQRCode* CreateQRCode();

QRCODE_API void ReleaseQRCode(IQRCode* pIQR);