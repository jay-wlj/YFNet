#include "file.h"
#include <string>
#include "common-utility.h"
#include "global.h"
#include "qvod_time.h"

CFile::CFile(std::string strFile,QVOD_INT64 len)
{
	m_strFileName = strFile;
	m_filelen =len;
	m_downloadlen = 0;
	pf = NULL;
	//fh = -1;
	m_status = OPEN;
	m_openmode = READONLY;
}
CFile::~CFile()
{
	Close();
}
int CFile::Create()
{

	if (pf)
	//if(fh != -1)
		return 0;
	int pos=m_strFileName.find("\\",0);
	while((pos= m_strFileName.find("\\",pos+1)) != std::string::npos)//a: from ">0" to "!= npos"
	{
		std::string path = m_strFileName.substr(0,pos);
		QvodMkdir(path.c_str());
		
	} 
	pos = m_strFileName.rfind("\\",m_strFileName.size()) +1;
	std::string filename = m_strFileName.substr(pos ,m_strFileName.size() - pos);
	//防止第二次下载把第一次的清除掉
	pf = fopen((m_strFileName+".!mv").c_str(),"r+b");
	//fh = _open((m_strFileName+".!mv").c_str(),_O_RDWR |_O_BINARY , _S_IREAD | _S_IWRITE);

	//原来没有下载过,则创建文件来下载
	if(pf == NULL)
		pf = fopen((m_strFileName+".!mv").c_str(),"w+b");
	//if(fh == -1)
		//fh = _open((m_strFileName+".!mv").c_str(),_O_RDWR | _O_CREAT |_O_BINARY , _S_IREAD | _S_IWRITE);
	m_openmode = READWRITE;
	return pf==NULL?-1:0;
	//return fh == -1?-1:0;
}
int CFile::Open()
{
	if (pf)
	//if(fh != -1)
		return 0;
	//如果自己正在下载,则要打开临时文件
	pf = fopen((m_strFileName+".!mv").c_str(),"r+b");
	//fh = _open((m_strFileName+".!mv").c_str(),_O_RDWR|_O_BINARY  , _S_IREAD| _S_IWRITE);

	//没有临时文件,读取正式文件
	if(pf == NULL)
		pf = fopen((m_strFileName).c_str(),"rb");
	//if(fh == -1)
		//fh = _open((m_strFileName).c_str(),_O_RDONLY|_O_BINARY  , _S_IREAD);
	else
		m_openmode = READWRITE;

	return pf==NULL?-1:0;
	//return fh == -1?-1:0;
}

int CFile::IsExist()
{
	qvod_stat_t stat_info;
	if ( QVOD_STAT( (m_strFileName+".!mv").c_str(), &stat_info ) != -1 )
	{
		return 1;
	}
	if (QVOD_STAT( (m_strFileName).c_str(), &stat_info )!= -1 )
	{
		return 2;
	}
	return 0;
}
int CFile::Close()
{
	CAutoLock lock(m_lock);
	m_status = CLOSE;
	if (!pf)
	//if(fh == -1)
	{
		return -1;//已经关闭并改名，直接退出
	}
	fclose(pf);
	pf = 0;
	//_close(fh);
	//fh = -1;
	return 0;
}
int CFile::Rename()
{
	CAutoLock lock(m_lock);
	if (pf)
	//if(fh != -1)
	{
		//保证只改名一次
		fclose(pf);
		pf = 0;	
		//_close(fh);
		//fh = -1;
		remove(m_strFileName.c_str());
		int re = rename((m_strFileName+".!mv").c_str(),m_strFileName.c_str());
		int trytime = 5;
		while(re != 0 && trytime)
		{
			QvodSleep(1);
			re = rename((m_strFileName+".!mv").c_str(),m_strFileName.c_str());
			trytime--;
		}
		//if(re !=0)
		//	MoveFileEx((m_strFileName+".!mv").c_str(),m_strFileName.c_str(),MOVEFILE_DELAY_UNTIL_REBOOT);
		m_openmode = READONLY;
		return 0;
	}
	else
	{
		//BT下载时，已经创建过大小为0的文件，由于文件大小为0，则文件不会被打开，指针为-1，这里也要该名字
		int re = QVOD_RENAME((m_strFileName+".!mv").c_str(),m_strFileName.c_str());
		return -1;
	}
}
int CFile::Write(QVOD_INT64 offset,char* buf,unsigned int len)
{
	if(offset > m_filelen)//偏移在文件范围之内
		return -2;
	CAutoLock lock(m_lock);
	if(	m_status == CLOSE) 
		return -2;
	if (!pf &&  -1==Create()) 
	//if(fh == -1 &&  -1==Create())
		return -2;

	fseek(pf,offset,SEEK_SET );
	//_lseeki64(fh,offset,SEEK_SET);
	
	int totallen = QVOD_MIN2(len,m_filelen-offset);
#ifdef SUPERNODE
	const char* pname = strrchr(m_strFileName.c_str(),'\\');
	if(!pname)
		return -3;
	pname++;
	if(g_pcachelib && strlen(pname) == 45)
	{
		int i=0;
		unsigned char hash[20];
		unsigned int key;
		Char2Hash(pname,hash);
		memcpy(&key,hash,sizeof(key));
		while(i*16*1024 < totallen)
		{
			unsigned char tmp[16*1024];
			Enc2((BYTE*)buf+i*16*1024,tmp,min(16*1024,(totallen-i*16*1024)),key);
			memcpy(buf+i*16*1024,tmp,min(16*1024,(totallen-i*16*1024)));
			i++;
		}
	}
#endif
#ifdef NETBAR
	{
		char *pname = strrchr(m_strFileName.c_str(),'\\');
		if(!pname)
			return -3;
		pname++;
		int i=0;
		unsigned char hash[20]={0};
		Char2Hash(pname,hash);
		while(i < totallen)
		{
			unsigned char tmp = *(buf+i);
			*(buf+i) = tmp+hash[0];
			i++;
		}
	}
#endif

	int nlen = 0;
	while (nlen < totallen)
	{
		int ilen = fwrite(buf+nlen,sizeof(char),totallen-nlen,pf);
		//int ilen = _write(fh,buf+nlen,totallen-nlen);
		if(ilen <= 0)
			break;
		nlen += ilen;
	}
	if(totallen == nlen)
		fflush(pf);
		//_commit(fh);
	else 
		nlen = -1;
	return nlen;
	
}
int CFile::Read(QVOD_INT64 offset,char* buf,unsigned int len)
{
	CAutoLock lock(m_lock);
	if(	m_status == CLOSE) 
		return -2;
	if (!pf && -1==Open())
	//if(fh == -1 && -1==Open())
		return -1;
	if(offset + len > m_filelen)
	{
		len = m_filelen - offset;
	}	
	fseek(pf,offset,SEEK_SET );
	//_lseeki64(fh,offset,SEEK_SET);
	int totallen = len;
	int nlen = 0;
	while(nlen < totallen)
	{
		int ilen = fread(buf+nlen,sizeof(char),totallen-nlen,pf);
		//int ilen = _read(fh,buf+nlen,totallen-nlen);
		if(ilen <= 0)
			break;
		nlen += ilen;

	}
	if(totallen != nlen)
		nlen = -1;
#ifdef SUPERNODE
	const char* pname = strrchr(m_strFileName.c_str(),'\\');
	if(!pname)
		return -3;
	pname++;
	if(g_pcachelib && pname && strlen(pname) == 45)
	{
		int i=0;
		unsigned char hash[20];
		unsigned int key;
		Char2Hash(pname,hash);
		memcpy(&key,hash,sizeof(key));
		while(i*16*1024 < nlen)
		{
			unsigned char tmp[16*1024];
			Dec2((BYTE*)buf+i*16*1024,tmp,min(16*1024,(nlen-i*16*1024)),key);
			memcpy(buf+i*16*1024,tmp,min(16*1024,(nlen-i*16*1024)));
			i++;
		}
	}
#endif
#ifdef NETBAR
	{
		char *pname = strrchr(m_strFileName.c_str(),'\\');
		if(!pname)
			return -3;
		pname++;
		int i=0;
		unsigned char hash[20]={0};
		Char2Hash(pname,hash);
		while(i < nlen)
		{
			unsigned char tmp = *(buf+i);
			*(buf+i) = tmp-hash[0];
			i++;
		}
	}
#endif	
	if(m_openmode == READONLY)
	{
		//_close(fh);
		//fh = -1;
		fclose(pf);
		pf = NULL;
	}
	return nlen;
}

CTmpFile::CTmpFile(std::string strFile,QVOD_INT64 len,QVOD_INT64 offset)
:CFile(strFile,len)
{
	m_offset = offset;
	m_offset = QVOD_MIN2(m_offset,len);
	m_filelen = len - m_offset;
	m_pbuf = new char[m_offset];
	Create();//创建临时文件，即使异常退出，下次启动也会删除该文件
}
CTmpFile::~CTmpFile()
{
}

int CTmpFile::Write(QVOD_INT64 offset,char* buf,unsigned int len)
{
	if(offset+len <= m_offset)
	{
		memcpy(m_pbuf+offset,buf,len);
		return len;
	}
	else if(offset+len > offset && offset < m_offset)
	{
		memcpy(m_pbuf+offset,buf,m_offset - offset);
		return CFile::Write(0,buf+m_offset-offset,len+offset-m_offset)+ m_offset - offset;
	}
	else
	{
		return CFile::Write(offset-m_offset,buf,len);
	}
}
int CTmpFile::Read(QVOD_INT64 offset,char* buf,unsigned int len)
{
	if(offset+len <= m_offset)
	{
		memcpy(buf,m_pbuf+offset,len);
		return len;
	}
	else if(offset+len > offset && offset < m_offset)
	{
		memcpy(buf,m_pbuf+offset,m_offset - offset);
		return CFile::Read(0,buf+m_offset-offset,len+offset-m_offset) + m_offset - offset;
	}
	else
	{
		return CFile::Read(offset-m_offset,buf,len);
	}

}

