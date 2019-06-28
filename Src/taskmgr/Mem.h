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

	//��ʼ��
	bool Init(QVOD_UINT64 filesize, unsigned int piecesize);
	
	//�ж��Ƿ����ĳ��mem�ļ� return : true �� false û��
	bool IsExist();

	//�ж�ĳһ�������Ƿ�������� return : true ��� false û�����
	bool CheckIndex(unsigned int index);

	unsigned int GetBitFieldLen();
	unsigned char* GetMemBit(void);
	bool ClearBitField(unsigned int index);
	bool SetBitField(unsigned int index);
	int  SetBitField(QVOD_INT64 fileSize,unsigned int piecesize);
	bool SetDownLength(QVOD_INT64 length);
	bool ClearDownLength(QVOD_INT64 length);
	int  WriteMemFile();
	bool IsDownFinish();//�Ƿ�ȫ�������
	int  WriteToDisk();//���ڴ��е�memд��Ӳ��
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
	string			m_filename;		//mem�ļ���
	QVOD_INT64			m_filesize;		//�����ļ���С
	unsigned int	m_piecesize;	//�����ļ��ֿ��С
	unsigned int	m_piecenum;		//�����ļ��ܿ���
	unsigned int	m_piecetmpdown; //��ʱ����Ƭ����
	QVOD_INT64			m_downsize;		//�Ѿ�������صĴ�С
	unsigned char*	m_pBitField;	//bit��Ϣ
	unsigned int	m_pBitFieldLen;	//bit��Ϣ����
	char*           m_pFileData;
	CLock			m_lock;			
};

#endif