#pragma once
//#include "stdafx.h"
#include "ExifStruct.h"

class CJpgExif
{
public:
	CJpgExif();
	~CJpgExif();
	//���������Դ���ʵ�ʱ���ã�����GetExifInfo, WriteExifInfoǰInit��û�е��ã����Զ�����
	E_EXIFERROR Init(LPTSTR szJpgPath);
	//�ͷ���Դ��������Ҫ�ļ����exif��Ϣʱ����(������ã�������Դֻ�г������ʱ���ͷ�)
	void Clear();

	//BOOL GetExifInfo(LPTSTR szJpgPath, LPTSTR  pQueryedInfo, LPTSTR pExifInfo )
	// parms:
	//			LPTSTR szJpgPath :			 *.jpg�ļ��ľ���·��
	//			LPTSTR  pQueryedInfo :		��Ҫ��ѯ����Ϣ
	//			LPTSTR pExifInfo :			��ѯ���Ľ��
	//			maxDesSize					�ɿ�����pExifInfo������С��= -1�򲻿�������ʵ�ʳ��ȸ���maxDesSize
	int GetExifInfo(LPCTSTR szJpgPath, LPCTSTR  pQueryedType, LPSTR pExifInfo, int &maxDesSize  );

	//parms:
	//				BOOL bSave : �Ƿ񱣴��ļ�
	//				TRUE �����ļ���FALSE�����棨�������޸��ļ������TRUE�������ļ����棩
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
