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
	//ɾ������
	int Delete(const unsigned char* hash);
		
	//��������
	int AddTask(const TaskCfg &taskCfg);

	//��������
	int UpdateTask(TaskCfg& taskCfg);
	
	//��������key��Ϣ
	int UpdateTaskKey(TaskCfg& taskCfg);

	//��������״̬
	int SetStatus(const unsigned char* hash,int iStatus);	

	//������е�����
	int GetAllTask(vector<TaskCfg> &vecTask);

	//���������������/��������,flag 0:��������; 1:�ϴ�����
	int GetAllTask(vector<TaskCfg> &vecTask,int flag);

	//���ָ��Ŀ¼�µ������ļ�,strMedia,Ŀ¼��,UTF-8��ʽ
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
