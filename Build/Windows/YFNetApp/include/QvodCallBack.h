#pragma once
#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE_(IQSourceCallBack, IUnknown)
//interface IQvodCallBack
{
public:
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam)=0;
};

#ifdef __cplusplus
}
#endif
#ifndef _IQVOD_CALLBACK_
#define _IQVOD_CALLBACK_
static GUID IID_IQVODCallBack = {'iqcb', 0x9f0f, 0x4824, {0x8f, 0xc8, 0xe3, 0x2c, 0xa0, 0x49, 0x30, 0xc2}};
interface IQvodCallBack
{
public:
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam)=0;
};
#endif
#define   QVOD_MEDIA_BASE                  0x01       //ý�岿����Ϣ��ʼֵ
#define   QM_MEDIA_QVODSRC                QVOD_MEDIA_BASE + WM_USER   //QvodSource�ص���Ϣ��ʼֵ
#define   QM_MS_PLAYSTATE                 QVOD_MEDIA_BASE           //����״̬�ı���Ϣ
#define   QM_MS_SETSRCMODE                QM_MEDIA_QVODSRC + 0x01  //����QvodSource������ģʽ
#define   QM_MS_PLAYSOURCE                QM_MEDIA_QVODSRC + 0x02 //����ý��Դ
#define   QM_MS_SETQVODNET                QM_MEDIA_QVODSRC + 0x03  //��������ģ��ָ��
#define   QM_MS_BUFFERBEGIN               QM_MEDIA_QVODSRC + 0x04
#define   QM_MS_SEEKING                   QM_MEDIA_QVODSRC + 0x05  //������ק
#define   QM_MS_BUFFERPER                 QM_MEDIA_QVODSRC + 0x06
#define   QM_MS_SETPREBUFFERLEN           QM_MEDIA_QVODSRC + 0x07   //����ǰ����
#define   QM_MS_SETPLAYBUFFERLEN          QM_MEDIA_QVODSRC + 0x08   //�����л�������
#define   QM_M_GETSOURCEMODE              QM_MEDIA_SRC_QVOD + 0x09       //��ȡ��ǰQvodsource��ģʽ
#define   QM_MS_SETMEDIACALLBACK          QM_MEDIA_SRC_QVOD + 0x10       //����ý�崦��ģ��ص��ӿ�
#define   QM_MS_SETBUFFEREVENT            QM_MEDIA_SRC_QVOD + 0x11       //����ý�崦�����¼����
#define   QM_MS_SETDSCREATEEVENT          QM_MEDIA_SRC_QVOD + 0x12       //����Graph�����¼����
#define   QM_MS_SETQLIVENET               QM_MEDIA_QVODSRC + 0x13  //����Qliv�ӿ�ָ��
#define   QM_MS_ISBUFFEREMPTY             QM_MEDIA_QVODSRC + 0x14  //��ѯ�����Ƿ�Ϊ��
#define   QM_MS_GETCURFILELEN             QM_MEDIA_QVODSRC + 0x15  //��ȡ��ǰ�ļ�����
#define   QM_MS_SETINDEXPOS               QM_MEDIA_QVODSRC + 0x16  //���õ�ǰ�����ļ�����λ��
#define   QM_SM_PLAYING_BUFFERPER         QM_MEDIA_QVODSRC + 0x17  //�����л���ٷֱ�
#define   QM_MS_TEMPSTOP                  QM_MEDIA_QVODSRC + 0x18  //��ʱֹͣ�����ڶ�̬����postvideo��subtitle
#define   QM_MS_QUERY_DOWNLOAD_LEN        QM_MEDIA_QVODSRC + 0x19  //��ѯ�ļ����س���




#define   QM_MEDIA_SRC_QVOD      QM_MEDIA_QVODSRC + 0x50  //SRC -> Qvod Media
//#define   QM_SM_BUFFERPER        QM_MEDIA_SRC_QVOD
#define   QM_SM_BUFFERBEGIN        QM_MEDIA_SRC_QVOD + 0x01
#define   QM_SM_BUFFEREND          QM_MEDIA_SRC_QVOD + 0x02
#define   QM_SM_BUFFERING          QM_MEDIA_SRC_QVOD + 0x03
#define   QM_SM_QSOURCE_READY      QM_MEDIA_SRC_QVOD + 0x04       //Qvod source �������
#define   QM_SM_CONTAINER_TYPE     QM_MEDIA_SRC_QVOD + 0x05  //Qvod source����ý����������
#define   QM_SM_GRAPH_STOP         QM_MEDIA_SRC_QVOD + 0x06  //ֹͣGraph
#define   QM_SM_FILE_INDEX         QM_MEDIA_SRC_QVOD + 0x07  //ֹͣGraph

enum StreamMode{
	NULL_Mode,
	Push_Mode,     //��ģʽ
	Pull_Mode      //��ģʽ
};