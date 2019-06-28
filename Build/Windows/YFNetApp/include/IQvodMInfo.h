#ifndef _IQVOD_MINFO_H__
#define _IQVOD_MINFO_H__

typedef struct MediaInfo
{
	wchar_t strfilename[MAX_PATH*2];	//�ļ�ȫ·��
	wchar_t strauthor[MAX_PATH];		//����
	wchar_t strspecial[MAX_PATH];		//ר��
	__int64 	nsize;					//�ļ���С �ֽ�
	__int64  ndur;						//�ļ�����ʱ��-���뼶
	long    nwidth;						//��Ƶ������ƵΪ0
	long    nheight;					//��Ƶ�ߣ�����ƵΪ0
	long    nbits;						//�ļ�bits; ���� nsize*8/(ndur/1000)
	long		nvideocoder;			//��Ƶ������ID
	long		naudiocoder;			//��Ƶ������ID
	//������/��Ƶ������ID���Զ��壬
	//�� H264 = 1,AVC1=2,MPG-3, 
	//REALVIDEO=4,REALAUDIO=5����
}MediaInfo_s;


interface IQvodMInfo
{
	BOOL LoadFile(wchar_t *pstrfilename);
	BOOL GetMediaInfo(MediaInfo_s *pMediaInfo);
	void Clear();
};


#endif