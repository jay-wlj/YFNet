// UpLoadTask.cpp: implementation of the CUpLoadTask class.
//
//////////////////////////////////////////////////////////////////////

#include "UpLoadTask.h"
#include "global.h"
#include "common-utility.h"
#include "sha1.h"
#include "TaskMgrInterface.h"
#include "common-commonfun.h"
#include "ServerProduct.h"
#include "qvod_file.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUpLoadTask::CUpLoadTask(std::string strfile,std::string strAdUrl,bool bIsBaiduFile)
{
	m_Para.type = TVUPLOADTASK;
	int pos = strfile.rfind("\\")+1;
	m_strPath = strfile.substr(0,pos);
	m_strFileName = strfile.substr(pos,strfile.size() - pos);

	//计算qdata文件的加密key
	if(m_strFileName.length() >= 46 && m_strFileName.find(".qdata") != string::npos)
	{	
		char szHash[40] = {0};
		unsigned char tmphash[20] = {0};
		//WideCharToMultiQVOD_BYTE(CP_ACP,0,m_strFileName.c_str(),40,szHash,40,NULL,NULL);
		memcpy(szHash, m_strFileName.c_str(), 40);
		Char2Hash(szHash,tmphash);
		memcpy(&m_iEncodeKey,tmphash,sizeof(m_iEncodeKey));
	}

	m_strAdUrl = strAdUrl;

	string strSeedName = strfile;
	std::replace(strSeedName.begin(), strSeedName.end(), ':', '_');
	std::replace(strSeedName.begin(), strSeedName.end(), '\\', '_');

	m_seed = new CSeed(strSeedName.c_str());	
	m_mem = new CMem(strSeedName.c_str());	

	if (bIsBaiduFile)
	{
		m_iBaiduFileFlag = 1;
	}
}

CUpLoadTask::~CUpLoadTask()
{
#ifndef SUPERNODE
	if ( m_Para.status == CTask::STOP )
	{
		if ( !m_bIsReservedFile )
		{
			m_seed->Remove();
			m_mem->Remove();
		}
	}
#endif
}

int CUpLoadTask::WriteData(const char *buf, UINT len, unsigned int index, unsigned int start)
{
	return -1;
}

//从指定片的指定位置读数据
int CUpLoadTask::ReadData(char *buf, UINT len, unsigned int index, unsigned int start,bool bIsUpload)
{
	int totallen = 0;
	QVOD_UINT64 filelen = m_seed->GetFileTotleLen();
	QVOD_UINT64 offset = QVOD_UINT64(index) * (QVOD_UINT64)m_seed->GetPieceSize() + (QVOD_UINT64)start;

	//如果超过文件大小,返回错误
	len = (UINT)(len > (filelen - offset) ? (filelen-offset) : len);
	if(offset >= filelen || len == 0)
	{
		return -2;
	}

	//未下载到种子没解析出长度
	if(filelen == 0)
	{
		return 0;
	}

	unsigned int i =0;					
	while(m_vctFile.size() >i && (QVOD_UINT64)(m_vctFile[i]->GetFileLen()) <= offset)
	{
		offset -= m_vctFile[i++]->GetFileLen();
	}

	if(m_vctFile.size() == i) 
	{
		return 0;
	}

	char* pdatabuf = buf;
	UINT tlen = len;

	char* readbuf = NULL;
	int remains = 0;
	if(m_iEncodeKey != 0)
	{
		int encodeblock = 8*1024;
		remains = start%encodeblock;
		if (remains != 0 || len%encodeblock != 0)
		{
			unsigned readstart = start - remains;
			unsigned int readlen = len + remains;
			if (readlen%encodeblock != 0)
			{
				readlen += encodeblock - readlen%encodeblock;
			}
			QVOD_UINT64 readoffset = QVOD_UINT64(index * m_seed->GetPieceSize() + readstart);
			if(readoffset + readlen > filelen)
			{
				readlen = filelen - readoffset;
			}
			readbuf = new (std::nothrow) char[readlen];
			if (NULL == readbuf)
			{
				return 0;
			}
			tlen = readlen;
			pdatabuf = readbuf;
			offset = readoffset;
		}
	}

	while(tlen>0 &&  i < m_vctFile.size())
	{
		int nlen = m_vctFile[i++]->Read(offset, pdatabuf ,tlen);
		if(nlen < 0)
		{
			if (NULL != readbuf)
			{
				delete[] readbuf;
			}
			return nlen;
		}		
		tlen -= nlen;
		pdatabuf += nlen;
		totallen += nlen;
		offset = 0;
	}

	if (m_iEncodeKey != 0)
	{
		int j=0;
		pdatabuf -= totallen;
		while(j*8*1024 < totallen)
		{
			unsigned char tmp[8*1024];
			Dec2((QVOD_BYTE*)pdatabuf+j*8*1024,tmp,min(8*1024,(totallen-j*8*1024)),m_iEncodeKey);
			memcpy(pdatabuf+j*8*1024,tmp,min(8*1024,(totallen-j*8*1024)));
			j++;
		}
		if (readbuf)
		{
			memcpy(buf, readbuf+remains, len);
			totallen = len;
			delete[] readbuf;
		}
	}

	if (bIsUpload)
	{
//		m_lastUpSize = totallen+13;//更新上一次上传片大小,限速模块
		m_curUpSize += totallen;
	}
	return totallen;
}

int CUpLoadTask::run(bool bload)
{
	//检查文件是否存在
	if( QVOD_ACCESS( (m_strPath + m_strFileName).c_str(),0 ) == -1 )
	{
		m_seed->Remove();
		m_mem->Remove();
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"Upload Task:File Not exist!Delete Task(%s)!!\n",m_strFileName.c_str());
#endif // _DEBUG
		return -1;
	}
	if(!m_seed->Load())//种子文件不存在，则生成一个种子文件
	{
		if(bload)
		{
			m_seed->Remove();
			m_mem->Remove();
			return -1;
		}
		int result = Build(m_strPath+m_strFileName,m_strFileName,g_strtorrentpath+m_seed->GetMetafile(),m_strAdUrl, HTTPTRACKER);
		if(0 != result || !m_seed->Load())
		{
			m_seed->Remove();
			m_mem->Remove();
#ifdef QVODDEBUG
			_OUTPUT(ERROR1,"%s Build seed failed!\n",m_strFileName.c_str());
#endif // _DEBUG
			return -1;
		}
	}
	else
	{
		if(!IsFileSizeMatching())
		{
			m_seed->Remove();
			m_mem->Remove();
			int result = Build(m_strPath+m_strFileName,m_strFileName,g_strtorrentpath+m_seed->GetMetafile(),m_strAdUrl, HTTPTRACKER);
			if(0 != result)
			{
#ifdef QVODDEBUG
				_OUTPUT(ERROR1,"%s ReBuild seed failed!\n",m_strFileName.c_str());
#endif // _DEBUG
				return -1;
			}
			m_seed->Reset();
			if ( !m_seed->Load() )
			{
				m_seed->Remove();
				m_mem->Remove();
#ifdef QVODDEBUG
				_OUTPUT(ERROR1,"%s UploadTask Load seed Failed!\n",m_strFileName.c_str());
#endif // _DEBUG
				return -1;
			}
		}
	}
	unsigned char hash[20];
	CSHA1 sha;
	std::string strAnnounces = m_seed->GetAnnounces();
	if(strAnnounces.find(HTTPTRACKER) == std::string::npos)
	{
		sha.write(m_seed->GetInfoHash(),m_seed->GetInfoHashlen());
	}
	else
	{
		sha.write(m_seed->GetHashList(),m_seed->GetPieceNum()*20);
	}
	sha.read(hash);

	if(CTaskMgrInterFace::Instance()->FindTask(hash))
	{
		// 共享任务时,发现同一个文件被百度先共享了,删除百度共享,采用Qvod共享 [12/21/2013 tangtao]
		HASH hashQvod(hash);

		//如果m_iBaiduFileFlag==1,则表示百度目录下有两个相同的文件,经测试他们的确会这么干
		if (/*CTaskMgrInterFace::Instance()->GetBaiduFileFlag(hashQvod) == 1 &&*/ this->m_iBaiduFileFlag != 1)
		{
#ifdef QVODDEBUG
			_OUTPUT(INEED,"Delete Baidu and Share with Qvod!<%s>\n",m_strFileName.c_str());
#endif
			CServerProduct::SetTaskStatus(hashQvod,CTask::STOP,false,true);
		}
		else
		{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"%s,Task has already existed!Share failed!\n\t\t\n",m_strFileName.c_str());
#endif // _DEBUG
		return -2;
		}
	}

	m_mem->SetBitField( m_seed->GetFileTotleLen(), m_seed->GetPieceSize() );	
	memcpy(m_Para.hash,hash,20);

	CQvodFile* pFile = NULL;
	if(strAnnounces.find(HTTPTRACKER) != std::string::npos)
	{
		//QVOD任务
		pFile = new CQvodFile(m_strPath + m_strFileName,m_seed->GetFileLen(),true);
		if ( pFile == NULL )
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1,"%s,New QvodFile failed!share failed!\n",m_strFileName.c_str());
#endif // _DEBUG
			return -1;
		}
		m_vctFile.push_back(pFile);	
	}
	else
	{
#ifdef SUPERNODE
		//BT任务,一个文件
		pFile = new CQvodFile(m_strPath + m_strFileName,m_seed->GetFileLen(),true);
		if ( pFile == NULL )
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1,L"%s,BT Task New QvodFile failed!share failed!\n",m_strFileName.c_str());
#endif
			return -1;
		}
		m_vctFile.push_back(pFile);	
#else
		for( int i =0;i<m_seed->GetFileNum();i++)
		{
			pFile = new CQvodFile(m_strPath + m_seed->GetFileName(i),m_seed->GetFileLen(i),true);
			if ( pFile == NULL )
			{
				return -1;
			}
			m_vctFile.push_back(pFile);	
		}
#endif
	}

	m_Para.status = RUNNING;
	if(strAnnounces.find(HTTPTRACKER) == std::string::npos)//共享到其他tracker
	{
		m_Para.type = BTUPLOADTASK;
	}
	if ( !bload )
	{
		//如果不是加载的共享任务,共享完成后,删除mem文件!
		std::string file = g_strtorrentpath + m_seed->GetMetafile() + ".mem";
		QvodRemoveFile(file.c_str());
	}
	return 0;
}

void CUpLoadTask::RemoveTmpFile()
{
	if(m_Para.type == BTUPLOADTASK)
	{
		std::string filepath = g_strtorrentpath + m_seed->GetMetafile();
		remove((filepath+std::string(".torrent")).c_str());
		remove((filepath+std::string(".mem")).c_str());
	}
#ifndef SUPERNODE
#ifndef NETBAR
	std::string filepath = g_strtorrentpath + m_seed->GetMetafile();
	remove((filepath+std::string(".torrent")).c_str());
	remove((filepath+std::string(".mem")).c_str());
#endif	
#endif 
}

int CUpLoadTask::WriteHttpData( const char *buf, QVOD_UINT64 start,UINT len )
{
	return -1;
}
