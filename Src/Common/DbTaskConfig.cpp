// DbTaskConfig.cpp: implementation of the CDbTaskConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "DbTaskConfig.h"
#include <sys/stat.h>
#include <stdio.h>
#include "stdafx.h"
#include "common-commonfun.h"
#include "common-utility.h"
#include <time.h>
#include "common-AutoPtr.h"
#include "qvod_file.h"
#include "qvod_error.h"
#include "AutoPtr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDbTaskConfig *CDbTaskConfig::m_pDBConfig = NULL;
CDbTaskConfig::CDbTaskConfig()
{
	m_db = NULL;
}

CDbTaskConfig::~CDbTaskConfig()
{

}

CDbTaskConfig *CDbTaskConfig::Instance()
{
	if (NULL == m_pDBConfig)
	{
		m_pDBConfig = new CDbTaskConfig;
	}
	return m_pDBConfig;
}

void CDbTaskConfig::FreeInstance()
{
	if ( m_pDBConfig != NULL )
	{
		delete m_pDBConfig;
		m_pDBConfig = NULL;
	}
}

bool CDbTaskConfig::TransactionBegin()	//开始事务
{
	if (NULL == m_db)
	{
		return false;
	}
	CAutoLock lock(m_lock);
	char *pError = NULL;
	int iResult = sqlite3_exec(m_db,"BEGIN",0,NULL,&pError);
	if ( iResult != SQLITE_OK )
	{
		sqlite3_free(pError);
		return false;
	}
	return true;
}

bool CDbTaskConfig::TransactionCommit()	//提交事务
{
	if (NULL == m_db)
	{
		return false;
	}
	CAutoLock lock(m_lock);
	char *pError = NULL;
	int iResult = sqlite3_exec(m_db,"COMMIT",0,NULL,&pError);
	if ( iResult != SQLITE_OK )
	{
		sqlite3_free(pError);
		return false;
	}
	return true;
}

bool CDbTaskConfig::TransactionRollback()
{
	if (NULL == m_db)
	{
		return false;
	}
	CAutoLock lock(m_lock);
	char *pError = NULL;
	int iResult = sqlite3_exec(m_db,"ROLLBACK",0,NULL,&pError);
	if ( iResult != SQLITE_OK )
	{
		sqlite3_free(pError);
		return false;
	}
	return true;
}

std::string  CDbTaskConfig::AnsiToUtf8(const char *pBuf)
{
#ifdef WIN32 /* WIN32 */

	//现将Ansi转化为Unicode
	int len = ::MultiByteToWideChar(CP_ACP, 0, pBuf, -1, NULL, 0);
	if (len == 0) 
	{
		return "";
	}
	std::vector<wchar_t> unicode(len);
	::MultiByteToWideChar(CP_ACP, 0, pBuf, -1, &unicode[0], len);

	//将Unicode转化为UTF-8
	len = ::WideCharToMultiByte(CP_UTF8, 0, &unicode[0], -1, NULL, 0, NULL, NULL);
	if (len == 0) 
	{
		return "";
	}
	std::vector<char> utf8(len);
	::WideCharToMultiByte(CP_UTF8, 0, &unicode[0], -1, &utf8[0], len, NULL, NULL);

	return &utf8[0];
#else /* posix */

	char utf8_str[QVOD_MAX_PATH] = {0};

	/* ANSI -->UTF-8 */
/*
	iconv_t cd = iconv_open("UTF-8", "CN-GB");
	if(cd == (iconv_t)-1)
	{	
		return "";
	}

	char *in = (char *)pBuf;
	char *out = utf8_str;
	size_t ilen = strlen(in);
	size_t olen = QVOD_MAX_PATH;
	size_t	size;

	size = iconv(cd, &in, &ilen, &out, &olen);
	iconv_close(cd);

	if(size == (size_t)-1) {
		return "";
	}
*/
	strcpy(utf8_str, pBuf);

	return utf8_str;

#endif /* posix end */
}

#if 0
std::string  CDbTaskConfig::UnicodeToUtf8(const wchar_t *wpBuf)
{
#ifdef WIN32
	int iLen = ::WideCharToMultiByte(CP_UTF8, 0, wpBuf, -1, NULL, 0, NULL, NULL);
	if ( iLen == 0 )
	{
		return "";
	}

	std::vector<char> utf8(iLen);
	::WideCharToMultiByte(CP_UTF8, 0, wpBuf, -1, &utf8[0], iLen, NULL, NULL);
	return &utf8[0];
#else
/*
 	size_t inBytes = strlen(wpBuf);
   	size_t outBytes = 3* inBytes ;
	char szTemp[outBytes] = {0};
   	iconv_t  conv;
   	conv = iconv_open("UTF-8", "WCHAR_T");  //WCHAR_T即unicode
   	iconv(conv, (char **) &szBuffer, &inBytes, (char **) &szTemp, &outBytes);
   	iconv_close(conv);
*/
#endif
}
#endif

bool CDbTaskConfig::OpenDataBase(const std::string& strPath)
{
	CAutoLock lock(m_lock);
	int iResult = 0;
	char* pstrSql = NULL;
	char *pErrorMsg = NULL;

	//sqlite3_open()默认UTF-8,不支持中文,进行转化
	string strUtf8Path = AnsiToUtf8((char*)strPath.c_str());
	if (strUtf8Path.empty())
	{
		return false;
	}
	if ( sqlite3_open(strUtf8Path.c_str(),&m_db) != SQLITE_OK )
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1, "CDbTaskConfig::OpenDataBase sqlite3_open failed! path=[%s]", strUtf8Path.c_str());
#endif
		return false;
	}

//	qvod_stat_t stat_info = {0};
//	int iRet = QVOD_STAT( strPath.c_str(), &stat_info );
//	if ( iRet == -1 || stat_info.st_size == 0  )
	{
		pstrSql = "pragma auto_vacuum=1;";
		iResult = sqlite3_exec(m_db,pstrSql,0,0,&pErrorMsg);
		if ( iResult != SQLITE_OK )
		{
	#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CDbTaskConfig::OpenDataBase sqlite3_exec failed! sql=[%s] iResult=%d path=[%s] m_db=%d", pstrSql, iResult, strUtf8Path.c_str(), m_db);
	#endif
			sqlite3_free(pErrorMsg);
		}

		pstrSql = "create table if not exists TaskCfg (hash varchar(41) PRIMARY KEY,maketime int,"\
			"status int,type int,filepath varchar(512),metafile varchar(512),bckeckkey int,refurl varchar(512),"\
			"httporgurl varchar(512),httpkeyurl varchar(512),copyfullfile smallint,copystatus smallint,copypath varchar(512), key varchar(512) not null default'');";
		iResult = sqlite3_exec(m_db,pstrSql,0,0,&pErrorMsg);
		if ( iResult != SQLITE_OK )
		{
	#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CDbTaskConfig::OpenDataBase sqlite3_exec failed! sql=[%s] iResult=%d", pstrSql, iResult);
	#endif
			sqlite3_free(pErrorMsg);
			sqlite3_close(m_db);
			m_db = NULL;
			return false;
		}

		pstrSql = "alter table TaskCfg add key varchar(512) not null default ''";
		iResult = sqlite3_exec(m_db,pstrSql,0,0,&pErrorMsg);
		if ( iResult != SQLITE_OK )
		{
	#ifdef QVODDEBUG
			_OUTPUT(ERROR1, "CDbTaskConfig::OpenDataBase sqlite3_exec failed! sql=[%s] iResult=%d err=%s", pstrSql, iResult, pErrorMsg);
	#endif
		}
	}
#ifdef QVODDEBUG
	_OUTPUT(LOG_DEBUG,"Open DataBase successfully!!\n");
#endif // QVODDEBUG
	return true;
}

void CDbTaskConfig::CloseDataBase()
{
	CAutoLock lock(m_lock);
	if (m_db)
	{
		sqlite3_close(m_db);	
		m_db = NULL;
	}

#ifdef QVODDEBUG
	_OUTPUT(SERIOUS,"CloseDataBase\n");
#endif // _DEBUG
}

int CDbTaskConfig::Delete(const unsigned char* hash)
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	char *sErrMsg = NULL;
	char strSql[256] = {0};
	std::string strHash = Hash2Char(hash);
	char szHash[41] = {0};
	int iResult = 0;
	memcpy(szHash,strHash.c_str(),40);
	sprintf_s(strSql,256,"delete from TaskCfg where hash = '%s'",szHash);
	iResult = sqlite3_exec(m_db,strSql,0,0,&sErrMsg);
	if ( iResult != SQLITE_OK )
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"____Genius____Delete__Failed!____Hash:%s,ErrMsg:%s\n",szHash,sErrMsg);
#endif // _DEBUG
		sqlite3_free(sErrMsg);
		return iResult;
	}

#ifdef QVODDEBUG
	_OUTPUT(ERROR1,"____Genius____Delete____Hash:%s\n",szHash);
#endif // _DEBUG
	return iResult;
}

void CDbTaskConfig::HandleSingleQuotes(const std::string &strSource,char *pDst)
{
	if (strSource.length() >= 512)
	{
		return;
	}
	//pDst分配了512字节,足够引用页的长度了
	if ( strSource.find('\'') == std::string::npos )
	{
		memcpy(pDst,strSource.c_str(),strSource.size());
	}
	else
	{
		int j = 0;
		int iLen = (int)strSource.size();
		for ( int i = 0; i < iLen; i++ )
		{
			pDst[j++] = strSource[i];
			if ( strSource[i] == '\'' )
			{
				pDst[j++] = '\'';
			}
		}
	}
}

int CDbTaskConfig::AddTask(const TaskCfg &taskCfg)
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	char *sErrMsg = NULL;
	char strSql[2048] = {0};
	std::string strHash = Hash2Char(taskCfg.hash);
	char szHash[41] = {0};
	QPtr::AutoArrayPtr<char> pFilePath = new char[QVOD_MAX_PATH*2];
	QPtr::AutoArrayPtr<char> pMetaFile = new char[QVOD_MAX_PATH*2];
	QPtr::AutoArrayPtr<char> pRefUrl = new char[QVOD_MAX_PATH*2];
	QPtr::AutoArrayPtr<char> pHttpOrgUrl = new char[QVOD_MAX_PATH*2];
	QPtr::AutoArrayPtr<char> pHttpKeyUrl = new char[QVOD_MAX_PATH*2];
	QPtr::AutoArrayPtr<char> pCopypath = new char[QVOD_MAX_PATH*2];
	QPtr::AutoArrayPtr<char> pKey = new char[QVOD_MAX_PATH*2]();

	memset(pFilePath, 0x0, QVOD_MAX_PATH*2);
	memset(pMetaFile, 0x0, QVOD_MAX_PATH*2);
	memset(pRefUrl, 0x0, QVOD_MAX_PATH*2);
	memset(pHttpOrgUrl, 0x0, QVOD_MAX_PATH*2);
	memset(pHttpKeyUrl, 0x0, QVOD_MAX_PATH*2);
	memset(pCopypath, 0x0, QVOD_MAX_PATH*2);
	memset(pKey, 0x0, QVOD_MAX_PATH*2);

	memcpy(szHash,strHash.c_str(),40);

	//如果文件名里面含有单引号,需要把一个单引号变成两个!
	//sqlite会在内部将两个单引号还原成一个;
	//如果只写一个,sqlite默认字符串结束,判断Sql语句错误!
	HandleSingleQuotes(taskCfg.filepath,pFilePath);
	HandleSingleQuotes(taskCfg.metafile,pMetaFile);
	HandleSingleQuotes(taskCfg.strRefUrl,pRefUrl);
	HandleSingleQuotes(taskCfg.strHttpOrgUrl,pHttpOrgUrl);
	HandleSingleQuotes(taskCfg.strHttpKeyUrl,pHttpKeyUrl);
	HandleSingleQuotes(taskCfg.strCopyPath,pCopypath);
	HandleSingleQuotes(taskCfg.strKey,pKey);

	//end modify
	char szKey[64] = {0};
	memcpy(szKey, taskCfg.strKey.c_str(), taskCfg.strKey.length());
	sprintf_s(strSql,2048,"insert or ignore into TaskCfg(hash,maketime,status,type,filepath,metafile,bckeckkey,refurl,httporgurl,httpkeyurl,copyfullfile,copystatus,copypath, key)"\
				   "values ('%s',%d,%d,%d,'%s','%s',%d,'%s','%s','%s',%d,%d,'%s','%s');",
		szHash,taskCfg.maketime,taskCfg.status,taskCfg.type,(const char*)pFilePath,(const char*)pMetaFile,taskCfg.bcheckkey,(const char*)pRefUrl,(const char*)pHttpOrgUrl,(const char*)pHttpKeyUrl,
		taskCfg.bCopyFullFile,taskCfg.copystatus,(const char*)pCopypath, (const char*)pKey);

	int iResult = sqlite3_exec(m_db,strSql,0,0,&sErrMsg);

	if ( iResult != SQLITE_OK )
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"____Genius____AddTask____Failed!!Hash:%s,ErrMsg:%s\n",szHash,sErrMsg);
#endif // _DEBUG
		sqlite3_free(sErrMsg);
		return iResult;
	}

#ifdef QVODDEBUG
	_OUTPUT(ERROR1,"____Genius____AddTask____Hash:%s,Status: %d\n",szHash,taskCfg.status);
#endif // _DEBUG
	return iResult;
}

int CDbTaskConfig::SetStatus(const unsigned char* hash,int iStatus)
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	char *sErrMsg = NULL;
	char strSql[256] = {0};
	std::string strHash = Hash2Char(hash);
	char szHash[41] = {0};
	int iResult = 0;
	memcpy(szHash,strHash.c_str(),40);
	sprintf_s(strSql,256,"update TaskCfg set status=%d where hash = '%s'",iStatus,szHash);
	iResult = sqlite3_exec(m_db,strSql,0,0,&sErrMsg);
	if ( iResult != SQLITE_OK )
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"SetStatus Failed!!!Hash:%s,Status: %d,ErrMsg:%s\n",szHash,iStatus,sErrMsg);
#endif // _DEBUG
		sqlite3_free(sErrMsg);
		return iResult;
	}
#ifdef QVODDEBUG
	_OUTPUT(SERIOUS,"____Genius____SetStatus____Hash:%s,Status: %d\n",szHash,iStatus);
#endif
	return iResult;
}

int CDbTaskConfig::UpdateTask(TaskCfg& taskCfg)
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	char *sErrMsg = NULL;
	char strSql[1024] = {0};
	std::string strHash = Hash2Char(taskCfg.hash);
	char szHash[41] = {0};
	int iResult = 0;
	char *pRefUrl = new char[QVOD_MAX_PATH*2];
	char* pOrgUrl = new char[QVOD_MAX_PATH*2];
	char* pCopypath = new char[QVOD_MAX_PATH*2];
	memset(pRefUrl, 0x0, QVOD_MAX_PATH*2);
	memset(pOrgUrl, 0x0, QVOD_MAX_PATH*2);
	memset(pCopypath, 0x0, QVOD_MAX_PATH*2);
	memcpy(szHash,strHash.c_str(),40);
	HandleSingleQuotes(taskCfg.strRefUrl,pRefUrl);
	HandleSingleQuotes(taskCfg.strHttpOrgUrl,pOrgUrl);
	HandleSingleQuotes(taskCfg.strCopyPath,pCopypath);
	sprintf_s(strSql,1024,"update TaskCfg set status=%d,bckeckkey=%d,refurl='%s',type=%d,httporgurl='%s',copyfullfile=%d,copystatus=%d,copypath='%s' where hash='%s'",
		taskCfg.status,taskCfg.bcheckkey,pRefUrl,taskCfg.type,pOrgUrl,taskCfg.bCopyFullFile,taskCfg.copystatus,pCopypath,szHash);
	delete[] pRefUrl;
#ifdef QVODDEBUG
	_OUTPUT(INFO,"UpdateTask sql = %s\n",strSql);
#endif
	iResult = sqlite3_exec(m_db,strSql,0,0,&sErrMsg);
	if ( iResult != SQLITE_OK )
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"UpdateTask Failed!!!Hash:%s,Status: %d,ErrMsg:%s\n",szHash,taskCfg.status,sErrMsg);
#endif // _DEBUG
		sqlite3_free(sErrMsg);
		return iResult;
	}
#ifdef QVODDEBUG
	_OUTPUT(ERROR1,"____Genius____UpdateTask____Hash:%s,Status: %d\n",szHash,taskCfg.status);
#endif // _DEBUG
	return iResult;
}

int CDbTaskConfig::UpdateTaskKey(TaskCfg& taskCfg)
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	char *sErrMsg = NULL;
	char strSql[1024] = {0};
	std::string strHash = Hash2Char(taskCfg.hash);
	char szHash[41] = {0};
	int iResult = 0;
	QPtr::AutoArrayPtr<char> pKey = new char[QVOD_MAX_PATH*2]();

	memcpy(szHash,strHash.c_str(),40);
	HandleSingleQuotes(taskCfg.strKey, pKey);

	sprintf_s(strSql,1024,"update TaskCfg set key='%s' where hash='%s'", (const char*)pKey, szHash);

#ifdef QVODDEBUG
	_OUTPUT(INFO,"UpdateTask sql = %s\n",strSql);
#endif
	iResult = sqlite3_exec(m_db,strSql,0,0,&sErrMsg);
	if ( iResult != SQLITE_OK )
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"UpdateTask Failed!!!Hash:%s, ErrMsg:%s\n",szHash, sErrMsg);
#endif // _DEBUG
		sqlite3_free(sErrMsg);
		return iResult;
	}
#ifdef QVODDEBUG
	_OUTPUT(ERROR1,"UpdateTaskKey:%s, key=%s\n",szHash, taskCfg.strKey.c_str());
#endif // _DEBUG
	return iResult;
}

int CDbTaskConfig::SetAllStatus(int iStatus)
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	char *sErrMsg = NULL;
	char strSql[256] = {0};
	int iResult = 0;
	sprintf_s(strSql,256,"update TaskCfg set status=%d;",iStatus);
	iResult = sqlite3_exec(m_db,strSql,0,0,&sErrMsg);
	if ( iResult != SQLITE_OK )
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"SetAllStatus Failed!!!Status: %d,ErrMsg:%s\n",iStatus,sErrMsg);
#endif // _DEBUG
		sqlite3_free(sErrMsg);
		return iResult;
	}
#ifdef QVODDEBUG
	_OUTPUT(ERROR1,"____Genius____SetAllStatus____Status: %d\n",iStatus);
#endif // _DEBUG
	return iResult;
}

int CDbTaskConfig::GetAllTask(vector<TaskCfg> &vecTask)
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	const char *pTail = NULL;
	char* pSql = "select hash,maketime,status,type,filepath,metafile,bckeckkey,refurl,httporgurl,"\
		"httpkeyurl,copyfullfile,copystatus,copypath,key from TaskCfg;";
	sqlite3_stmt *stmt = NULL;
	int state = 0;
	int iRet = sqlite3_prepare(m_db, pSql, -1, &stmt, /*(const void **)*/&pTail);
	if ( iRet != SQLITE_OK )
	{
		sqlite3_finalize(stmt);
		return iRet;
	}
	state = sqlite3_step(stmt);

	const char *pValue = NULL;
	while ( state == SQLITE_ROW )
	{
		TaskCfg taskCfg;
		const char* strHash	 = (const char*)sqlite3_column_text(stmt, 0);
		Char2Hash(strHash,taskCfg.hash);
		taskCfg.maketime = sqlite3_column_int(stmt, 1);
		taskCfg.status = sqlite3_column_int(stmt, 2);
		taskCfg.type = sqlite3_column_int(stmt, 3);
		taskCfg.filepath = (const char*)sqlite3_column_text(stmt, 4);
		taskCfg.metafile = (const char*)sqlite3_column_text(stmt, 5);
		taskCfg.bcheckkey = sqlite3_column_int(stmt,6);
		taskCfg.strRefUrl = (const char *)sqlite3_column_text(stmt,7);
		taskCfg.strHttpOrgUrl = (const char *)sqlite3_column_text(stmt,8);
		taskCfg.strHttpKeyUrl = (const char *)sqlite3_column_text(stmt,9);
		taskCfg.bCopyFullFile = sqlite3_column_int(stmt,10);
		taskCfg.copystatus = sqlite3_column_int(stmt,11);
		taskCfg.strCopyPath = (const char *)sqlite3_column_text(stmt,12);
		pValue = (const char *)sqlite3_column_text(stmt,13);
		if (pValue)
		{
			taskCfg.strKey = pValue;
		}
		vecTask.push_back(taskCfg);
		state = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	return 0;
}

int CDbTaskConfig::GetAllTask(vector<TaskCfg> &vecTask,int flag)
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	const char *pTail = NULL;
	char* pSql = NULL;
	if ( flag == 0 )
	{
		pSql = "select hash,maketime,status,filepath,refurl from TaskCfg where type <= 2;";
	}
	else if ( flag == 1 )
	{
		pSql = "select hash,maketime,status,filepath,refurl from TaskCfg where type = 3;";
	}
	else
	{
		return 1;
	}
	sqlite3_stmt *stmt = NULL;
	int state = 0;
	int iRet = sqlite3_prepare(m_db, pSql, -1, &stmt, /*(const void **)*/&pTail);
	if ( iRet != SQLITE_OK )
	{
		sqlite3_finalize(stmt);
		return iRet;
	}		
	state = sqlite3_step(stmt);
	while ( state == SQLITE_ROW )
	{
		TaskCfg taskCfg;
		const char* strHash	 = (const char*)sqlite3_column_text(stmt, 0);
		Char2Hash(strHash,taskCfg.hash);
		taskCfg.maketime = sqlite3_column_int(stmt, 1);
		taskCfg.status = sqlite3_column_int(stmt, 2);
		taskCfg.filepath = (const char*)sqlite3_column_text(stmt, 3);
		taskCfg.strRefUrl = (const char *)sqlite3_column_text(stmt,4);
		vecTask.push_back(taskCfg);
		state = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	return 0;
}

int CDbTaskConfig::GetMediaTask(vector<TaskCfg> &vecTask,char *strMedia)
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	const char *pTail = NULL;
	char* pSql = "select hash,maketime,status,filepath,refurl from TaskCfg where filepath like '%s%%';";
	char szPath[256] = {0};
	sprintf_s(szPath,256,pSql,strMedia);

	sqlite3_stmt *stmt = NULL;
	int state = 0;
	int iRet = sqlite3_prepare(m_db, szPath, -1, &stmt, /*(const void **)*/&pTail);
	if ( iRet != SQLITE_OK )
	{
		sqlite3_finalize(stmt);
		return iRet;
	}		
	state = sqlite3_step(stmt);
	while ( state == SQLITE_ROW )
	{
		TaskCfg taskCfg;
		const char* strHash	 = (const char*)sqlite3_column_text(stmt, 0);
		Char2Hash(strHash,taskCfg.hash);
		taskCfg.maketime = sqlite3_column_int(stmt, 1);
		taskCfg.status = sqlite3_column_int(stmt, 2);
		taskCfg.filepath = (const char*)sqlite3_column_text(stmt, 3);
		taskCfg.strRefUrl = (const char *)sqlite3_column_text(stmt,4);
		vecTask.push_back(taskCfg);
		state = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	return 0;
}

int CDbTaskConfig::GetTasksNum()
{
	if (NULL == m_db)
	{
		return SQLITE_ERROR;
	}
	CAutoLock lock(m_lock);
	const char *pTail = NULL;
	char* pSql = NULL;
	pSql = "select count(*) from TaskCfg";
	sqlite3_stmt *stmt = NULL;
	int state = 0;
	int iRet = sqlite3_prepare(m_db, pSql, -1, &stmt, /*(const void **)*/&pTail);
	if ( iRet != SQLITE_OK )
	{
		sqlite3_finalize(stmt);
		return iRet;
	}		
	state = sqlite3_step(stmt);
	int iNums = 0;
	while ( state == SQLITE_ROW )
	{
		iNums = sqlite3_column_int(stmt, 0);
		state = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	return iNums;
}

