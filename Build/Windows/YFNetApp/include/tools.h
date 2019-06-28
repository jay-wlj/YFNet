#ifndef _TOOLS_H_H_
#define _TOOLS_H_H_

#include <atlstr.h>
class ExtensionTool
{
public:
	static CString GetExtension(CString str)
	{
		CString extension;
		int npos = str.ReverseFind(_T('.'));
		if(npos > -1)
		{
			extension = str.Right(str.GetLength() - npos);
		}
		return extension;
	}

	static CString ModifyExtension(CString str, CString newExtension)
	{
		CString strNew;
		int npos = str.ReverseFind(_T('.'));
		if(npos > -1)
		{
			strNew = str.Left(npos) + newExtension;
		}
		return strNew;
	}

	static CString RemoveExtension(CString str)
	{
		CString strNew;
		int npos = str.ReverseFind(_T('.'));
		if(npos > -1)
		{
			strNew = str.Left(npos) ;
		}
		return strNew;
	}

	static BOOL IsJpg(CString strFile)
	{
		CString extension = GetExtension(strFile);
		if(extension.CompareNoCase(_T(".jpg")) == 0 || extension.CompareNoCase(_T(".jpeg")) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	static BOOL IsBmp(CString strFile)
	{
		if(GetExtension(strFile).CompareNoCase(_T(".bmp")) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	static BOOL IsGif(CString strFile)
	{
		if(GetExtension(strFile).CompareNoCase(_T(".gif")) == 0)
		{
			return TRUE;
		}
		return FALSE;
	}
};
#endif

