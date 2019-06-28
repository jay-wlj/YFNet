#pragma once
//#include "stdafx.h"
#include "ExifStruct.h"

class CJpgExif
{
public:
	CJpgExif();
	~CJpgExif();
	//申请相关资源，适当时适用，调用GetExifInfo, WriteExifInfo前Init若没有调用，会自动调用
	E_EXIFERROR Init(LPTSTR szJpgPath);
	//释放资源，不再需要文件相关exif信息时调用(必须调用，否则资源只有程序结束时才释放)
	void Clear();

	//BOOL GetExifInfo(LPTSTR szJpgPath, LPTSTR  pQueryedInfo, LPTSTR pExifInfo )
	// parms:
	//			LPTSTR szJpgPath :			 *.jpg文件的绝对路径
	//			LPTSTR  pQueryedInfo :		需要查询的信息
	//			LPTSTR pExifInfo :			查询到的结果
	//			maxDesSize					可拷贝到pExifInfo的最大大小，= -1则不拷贝，将实际长度赋给maxDesSize
	int GetExifInfo(LPCTSTR szJpgPath, LPCTSTR  pQueryedType, LPSTR pExifInfo, int &maxDesSize  );

	//parms:
	//				BOOL bSave : 是否保存文件
	//				TRUE 保存文件，FALSE不保存（可连续修改文件后，最后传TRUE，进行文件保存）
	int WriteExifInfo(LPCTSTR szJpgPath, LPCTSTR pQueryedType, LPSTR pExifInfo, int lenth, BOOL bSave = TRUE, LPTSTR lpSavePath = _T(""));

private:
	int InitJpegMap(LPCTSTR szJpgPath);
	int InitJpegMapByState(FILE * &ostreamFile, E_EXIFSTATE &exifState);
	E_EXIFERROR InitIFD(FILE * &ostreamFile);
	int ParseIFDEnry(EXIFIFD * pCurrentIFDs, int nIFDIndex, char szIFD[], FILE *&ostreamFile);
	void ParseExifInfo(FILE * &ostreamFile);
	int GetExifInfoByType(LPCTSTR pQueryedType, LPSTR pExifInfo, int &maxDesSize);
	void ResetWrite();
	E_EXIFERROR ModifyIFD(LPCTSTR pQueryedType, LPSTR pExifInfo, int nLength);
	E_EXIFERROR SaveToFile(LPCTSTR szJpgPath, LPTSTR lpSavePath = _T(""));
	void ReleaseIFDs(CExifIFD * lpIFDs);

private:
	CExifTagMap m_mapTag;
	CExifMap m_mapExifInfo;
	CExifHead m_exifHead;
	CExifIFD m_IFDs;
	EXIFFEX  m_fex;
	_tstring m_initJpgPath;
	long	 m_fileStartPos;
};
