// UpLoadTask.h: interface for the CUpLoadTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPLOADTASK_H__C5699F82_7009_4986_93FB_C7545573389A__INCLUDED_)
#define AFX_UPLOADTASK_H__C5699F82_7009_4986_93FB_C7545573389A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable:4786)
#include "Task.h"

class CUpLoadTask :public CTask
{
public:
	CUpLoadTask(std::string strfile,std::string strAdUrl = "",bool bIsBaiduFile=false);
	virtual ~CUpLoadTask();
	virtual void RemoveTmpFile();
	virtual int run(bool bload=0);
	virtual int ReadData(char *buf, UINT len, unsigned int index, unsigned int start,bool bIsUpload);
	virtual int WriteData(const char *buf, UINT len, unsigned int index, unsigned int start);
	virtual int WriteHttpData(const char *buf, QVOD_UINT64 start,UINT len);

private:
	string m_strAdUrl;
	bool	m_iBaiduFileFlag;
};
#endif // !defined(AFX_UPLOADTASK_H__C5699F82_7009_4986_93FB_C7545573389A__INCLUDED_)
