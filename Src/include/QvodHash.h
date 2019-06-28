#ifndef __QVOD_HASH_H__
#define __QVOD_HASH_H__

#define QVOD_ERROR_SUCCESS			 0			//�ɹ�
#define QVOD_ERROR_INVALID_PARA		-1			//������������
#define QVOD_ERROR_FILE_NOT_EXIST	-2			//�ļ�������
#define QVOD_ERROR_FILE_OPEN_FAIL	-3			//�ļ���ʧ��
#define QVOD_ERROR_FILE_LEN_ZERO	-4			//�ļ�����Ϊ0
#define QVOD_ERROR_INTERNAL_FAIL	-5			//�����ڲ�����ʧ��

//hash������ȵĻص�����
//wpFilePath:�ļ�·��
//iProgress: ����hash�Ľ���,���55%,�򷵻�55;
//bError   : �Ƿ�������
typedef void (*QVODCALCHASHPROC)(wchar_t *wpFilePath,int iProgress,bool bError);

//hash����ɹ��Ļص�����
//wpFilePath: �ļ�·��
//szHash[41]: �ļ���hash
typedef void (*QVODCALCHASHRESULT)(wchar_t *wpFilePath,char szHash[41]);

//��ʼ�����ļ���hash,���������ID��
//wpFilePath: �ļ�·��
//pCalcProc : ������ȵĻص�����
//pGetResult: ����������ȡ����Ļص�����
//bFast		: Ĭ��Ϊfalse;�����̼߳���hash;��Ϊtrue,�����CPU���������ٵĽ���hash�ļ���;
int StartCalcHash(wchar_t *wpFilePath,QVODCALCHASHPROC pCalcProc,QVODCALCHASHRESULT pGetResult,bool bFast = false);


//��ֹhash�ļ���;
//iTaskID: StartCalcHash���ص�����ID��
//�ú��������ͷ��ڴ�Ĺ���
void StopCalcHash(int iTaskID);
#endif