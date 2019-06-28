#ifndef __QVOD_MEM_H__
#define __QVOD_MEM_H__

#include <string>
#include "qvod_lock.h"
using namespace std;
#define  NPICECTOWMEM	20
class CMem
{
public:
	CMem(const char *filename);

	~CMem();

	//初始化
	bool Init(QVOD_UINT64 filesize, unsigned int piecesize);
	
	//判断是否存在某个mem文件 return : true 有 false 没有
	bool IsExist();

	//判断某一块数据是否下载完成 return : true 完成 false 没有完成
	bool CheckIndex(unsigned int index);

	unsigned int GetBitFieldLen();
	unsigned char* GetMemBit(void);
	bool ClearBitField(unsigned int index);
	bool SetBitField(unsigned int index);
	int  SetBitField(QVOD_INT64 fileSize,unsigned int piecesize);
	bool SetDownLength(QVOD_INT64 length);
	bool ClearDownLength(QVOD_INT64 length);
	int  WriteMemFile();
	bool IsDownFinish();//是否全下载完成
	int  WriteToDisk();//将内存中的mem写入硬盘
	QVOD_INT64 GetDownLoadedlen()
	{
		return m_downsize;
	}

	int SetBitField();
	void Clear();
	void Remove();
	QVOD_INT64 GetContinueLen(QVOD_INT64 pos);
	QVOD_INT64 GetFilesizeFromMemfile();
private:
	string			m_filename;		//mem文件名
	QVOD_INT64			m_filesize;		//下载文件大小
	unsigned int	m_piecesize;	//下载文件分块大小
	unsigned int	m_piecenum;		//下载文件总块数
	unsigned int	m_piecetmpdown; //临时下载片计数
	QVOD_INT64			m_downsize;		//已经完成下载的大小
	unsigned char*	m_pBitField;	//bit信息
	unsigned int	m_pBitFieldLen;	//bit信息长度
	char*           m_pFileData;
	CLock			m_lock;			
};

#endif