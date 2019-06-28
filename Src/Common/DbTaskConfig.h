// DbTaskConfig.h: interface for the CDbTaskConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBTASKCONFIG_H__CB03F78D_918B_429E_8576_2498324BE94E__INCLUDED_)
#define AFX_DBTASKCONFIG_H__CB03F78D_918B_429E_8576_2498324BE94E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sqlite3.h"
#include <string>
#include <vector>
#include "terminal.h"
#define WIN32_LEAN_AND_MEAN

#include "common-commonstruct.h"
#include "qvod_lock.h"

class CDbTaskConfig
{
public:	
	//删除任务
	int Delete(const unsigned char* hash);
		
	//增加任务
	int AddTask(const TaskCfg &taskCfg);

	//更新任务
	int UpdateTask(TaskCfg& taskCfg);
	
	//更新任务key信息
	int UpdateTaskKey(TaskCfg& taskCfg);

	//设置任务状态
	int SetStatus(const unsigned char* hash,int iStatus);	

	//获得所有的任务
	int GetAllTask(vector<TaskCfg> &vecTask);

	//获得所有下载任务/共享任务,flag 0:下载任务; 1:上传任务
	int GetAllTask(vector<TaskCfg> &vecTask,int flag);

	//获得指定目录下的所有文件,strMedia,目录名,UTF-8格式
	int GetMediaTask(vector<TaskCfg> &vecTask,char *strMedia);

	int GetTasksNum();
public:
	int SetAllStatus(int iStatus);
private:
	CDbTaskConfig();
	virtual ~CDbTaskConfig();
//2010-03-18 modified by tangtao///////////////////
private:
	static CDbTaskConfig *m_pDBConfig;
public:
	static CDbTaskConfig *Instance();
	static void FreeInstance();
	bool TransactionBegin();
	bool TransactionCommit();
	bool TransactionRollback();
	bool OpenDataBase(const std::string& strPath);
	void CloseDataBase();
	void HandleSingleQuotes(const std::string &strSource,char *pDst);
private:
	std::string  AnsiToUtf8(const char *pBuf);
	std::string  UnicodeToUtf8(const wchar_t *wpBuf);
///////////////////////////////////////////////////
private:
	sqlite3		 *m_db;
	CLock		m_lock;
};

#endif // !defined(AFX_DBTASKCONFIG_H__CB03F78D_918B_429E_8576_2498324BE94E__INCLUDED_)
