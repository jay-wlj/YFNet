#ifndef _QVOD_Media_Format_H__
#define _QVOD_Media_Format_H__

// ͷ�ļ��İ汾��
// DLL��������
// �����д������

#ifdef ������DLL��Ŀ�ĺ궨��
#define DLLAPI extern "C" __declspec(dllexport)
#else
#define DLLAPI extern "C" __declspec(dllimport)
#endif


typedef struct QvodMediaFormat
{
	long nSize;							//�ṹ��С������ָ��
	wchar_t szFileName[MAX_PATH*2];		//�ļ�ȫ·��������ָ��
	wchar_t szTitle[MAX_PATH];			//����
	wchar_t szArtist[MAX_PATH];			//����
	wchar_t szAlbum[MAX_PATH];			//ר��
	__int64 nFileSize;					//�ļ���С ��λ�ֽ�
	__int64 nDuration;					//�ļ�����ʱ��-���뼶
	long    nFrameWidth;				//��Ƶ������ƵΪ0
	long    nFrameHeight;				//��Ƶ�ߣ�����ƵΪ0
	long    nbps;						//bit�ʣ����� nFileSize*8/(ndur/1000)
	long    nVideoIncoderID;			//��Ƶ������ID
	long    nAudioIncoderID;			//��Ƶ������ID
										//������/��Ƶ������ID���Զ��壬
										//�� H264 = 1,AVC1=2,MPG-3, 
										//REALVIDEO=4,REALAUDIO=5����
	long nReserved;						//�����ֶ�
}QvodMediaFormat_s;



DLLAPI BOOL GetQvodMediaFormat(QvodMediaFormat *pFormat); // ��ȡý���ļ���ʽ��Ϣ������FALSE��ʾʧ��


#endif