// DownLoadTask.h: interface for the CDownLoadTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNLOADTASK_H__EF3ACC09_2D81_42E7_97CE_D3AFA8D2D543__INCLUDED_)
#define AFX_DOWNLOADTASK_H__EF3ACC09_2D81_42E7_97CE_D3AFA8D2D543__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Task.h"

class CDownLoadTask :public CTask
{
public:
	CDownLoadTask(const unsigned char* hash,std::string strSaveFile,int type,string& strHttpOrgUrl,string& strHttpKeyUrl,string& strHttpReferer);//yf����
	virtual ~CDownLoadTask();

	 int		run(bool bload=false);
	 virtual  void	RemoveTmpFile(bool bDeleteAll=false);
	 virtual bool TaskIsFinish(bool bCheckFileExist = false);
	 bool		InitFileList(bool bIsShare);
	 bool		LoadMemFile();
	 bool		ValidateHash();

	 int		ReadData(char *buf, UINT len, unsigned int index, unsigned int start,bool bIsUpload);
	 int		WriteData(const char *buf, UINT len, unsigned int index, unsigned int start);

	 void		UpdateDownloadMode();
	 void		DownEmergencyData();
	 int		WriteHttpData(const char *buf,  QVOD_UINT64 start,UINT len);
	 int		DownNextHttpPiece(YfHttpPieceReq& req,bool bRandom = false);

	//int			WriteM3u8Ts(string strFilename,const char* buf,int start,int len,int filesize);
	//int			ReadM3u8Ts(string strFilename,char* buf,int start,int len,bool bIsUpload);
	//int			DownNextM3u8Ts(SM3u8TsReq& req);

	int			DeleteHasPlayingData(QVOD_UINT64 iCurCacheSize);		// ɾ����ǰ����λ��ǰ�Ļ�������
	static unsigned long QVOD_WINAPI BuildSeed(void* para);

#ifdef NETBAR
	virtual bool		IsBufferFull();//�ڴ���ж��Ƿ��пռ��д
	virtual bool		DelMemPiece();//�ڴ��ɾ��һƬ
#endif

protected:
	virtual int			RemoveFile(std::string &strFile);//0 success -1 fail
	//virtual void		UpdateM3u8TaskMode();

protected:
	bool m_bBuildSeed;
};

#endif // !defined(AFX_DOWNLOADTASK_H__EF3ACC09_2D81_42E7_97CE_D3AFA8D2D543__INCLUDED_)
