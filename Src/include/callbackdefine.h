#ifndef _QVOD_CALLBACK_MESSAGE_DEFINE_H_
#define _QVOD_CALLBACK_MESSAGE_DEFINE_H_
//QVOD Message Definition
#define QVOD_MESSAGE_BEGIN     0x001            //QVOD ��Ϣȡֵ��Χ��WM_USER�� WM_USER + 0x2000��
#define QVOD_MESSAGE_END       0x2000

#define QVOD_MEDIA_BASE        QVOD_MESSAGE_BEGIN
#define QVOD_UI_BASE           QVOD_MESSAGE_BEGIN + 0x1000
#define WM_GRAPHNOTIFY				WM_USER + 0x100
#define QVOD_DVD_BASE          QVOD_MESSAGE_BEGIN + 0x100

#define QVOD_DVD_EVENT         QVOD_DVD_BASE
//DVD�¼��е�WPARAM													LPARAM

#define QM_DVD_TITLENUMS							QVOD_DVD_BASE+1		//�������ı�--	ULONG nums
#define QM_DVD_TITLECHANGE							QVOD_DVD_BASE+2		//����ı�		DVD_TITLE_CHANGE------SENDMESSAGE
#define QM_DVD_CHAPTERSCHANGE						QVOD_DVD_BASE+3		//�½ڸı�------ULONG chapters
#define QM_DVD_AUDIO								QVOD_DVD_BASE+4		//�����ı�------ULONG
#define QM_DVD_SUBTITLE							    QVOD_DVD_BASE+5		//��Ļ�ı�------ULONG
#define QM_DVD_ANGLES								QVOD_DVD_BASE+6		//�ӽǸı�------ULONG
#define QM_DVD_DVDPRE								QVOD_DVD_BASE+7		//ǰһ��
#define QM_DVD_DVDNEXT								QVOD_DVD_BASE+8		//��һ��
#define QM_DVD_DVDMAINMENU							QVOD_DVD_BASE+9		//���˵�
#define QM_DVD_TITLEMENU							QVOD_DVD_BASE+10		//����˵�
#define QM_DVD_WORDSMENU							QVOD_DVD_BASE+11		//��Ļ�˵�
#define QM_DVD_AUDIOMENU							QVOD_DVD_BASE+12		//��Ƶ�˵�
#define QM_DVD_CHAPTERSMENU						    QVOD_DVD_BASE+13		//�½ڲ˵�
#define QM_DVD_ANGLEMENU							QVOD_DVD_BASE+14		//�ӽǲ˵�
#define QM_DVD_CLOSEMENU							QVOD_DVD_BASE+15		//DVD�ر�

//IQvodCallBack message
//#define CBM_BASEMESSAGE			0x001
#define QM_MEDIA_PLAYSTATE			QVOD_MEDIA_BASE + 0		//wparam old e_PlayState lparam new e_PlayState
//#define CBM_PLAYNETSTART		QVOD_MEDIA_BASE + 1		//�յ�����Ϣ��Ҫ�����̷߳���һ����������������
#define QM_MEDIA_LACKDECODER         QVOD_MEDIA_BASE + 2     //��������Ҳ��������Filter��������ʾ��Ϣ��wparamΪE_InfoType��ָʾ����Ƶ������ƵFilter��lparamΪ���������ƣ�LPCTSTRָ�룬ָ��һ���ַ���
//#define CBM_MESSAGENET			0x100
#define QM_MEDIA_MENUUPDATE          QVOD_MEDIA_BASE + 3 //�˵�������Ϣ
#define QM_MEDIA_FILTERNAMELIST      QVOD_MEDIA_BASE + 4 //Filter�˾��˵���Ϣ
#define QM_MEDIA_SUBTITLE            QVOD_MEDIA_BASE + 5 //��Ļ��Ϣ
#define QM_MEDIA_AUDIOCHANNEL        QVOD_MEDIA_BASE + 6 //��Ƶ��Ϣ
#define QM_MEDIA_GET_MAINHWND        QVOD_MEDIA_BASE + 7//��ȡ���������ھ��
#define QM_MEDIA_HD_START            QVOD_MEDIA_BASE + 8//Ӳ����������
#define QM_MEDIA_HD_STARTFAILED      QVOD_MEDIA_BASE + 9//Ӳ����������ʧ��
#define QM_MEDIA_BUFFER_BEGIN        QVOD_MEDIA_BASE + 10 //�����л��忪ʼ
#define QM_MEDIA_BUFFER_END          QVOD_MEDIA_BASE + 11 //�����л������
#define QM_MEDIA_RECORDING_END       QVOD_MEDIA_BASE + 12 //�ļ�¼�ƽ���
#define QM_MEDIA_FILE_INDEX          QVOD_MEDIA_BASE + 13 //����������ʼλ�ã���һ������Ϊ��ʼλ�ã��ڶ�������Ϊ���ȣ�
#define QM_MEDIA_RECORD_PER          QVOD_MEDIA_BASE + 14 //��Ƶ¼�ƽ���
#define QM_MEDIA_RECORD_ERROR        QVOD_MEDIA_BASE + 15 //��Ƶ¼�ƴ���
#define QM_MEDIA_RECORD_GRAPHREADY   QVOD_MEDIA_BASE + 16 //��Ƶ¼�ƽ�ͼOK
#define QM_MEDIA_VIDEO_SIZE          QVOD_MEDIA_BASE + 17 //��ǰ�����ļ�����Ƶ�ߴ�
#define QM_MEDIA_FILE_PLAY_FAILED    QVOD_MEDIA_BASE + 18 //ý���ļ�����ʧ��
#define QM_MEDIA_RBUTTONUP           QVOD_MEDIA_BASE + 19 //WM_RBUTTONUP
#define QM_MEDIA_RBUTTONDOWN         QVOD_MEDIA_BASE + 20 //WM_RBUTTONDOWN
#define QM_MEDIA_MOUSEMOVE           QVOD_MEDIA_BASE + 21 //WM_MOUSEMOVE
#define QM_MEDIA_LBUTTONDBLCLK       QVOD_MEDIA_BASE + 22 //WM_LBUTTONDBLCLK
#define QM_MEDIA_LBUTTONUP           QVOD_MEDIA_BASE + 23 //WM_LBUTTONUP
#define QM_MEDIA_HOTKEY              QVOD_MEDIA_BASE + 24 //WM_HOTKEY
#define QM_MEDIA_NOAUDIORENDER       QVOD_MEDIA_BASE + 25 //û����Ƶ����豸
#define QM_MEDIA_KEYDOWN             QVOD_MEDIA_BASE + 26 //WM_KEYDOWN
#define QM_MEDIA_LACK_FILTER         QVOD_MEDIA_BASE + 27 //filterȱʧ
#define QM_MEDIA_BUFFERING_PER       QVOD_MEDIA_BASE + 28 //�����л���ٷ���
#define QM_MEDIA_FRAME_DRAW			 QVOD_MEDIA_BASE + 29 //ˢ������
#define QM_RQ_D3DVIDEO_DETECT		 QVOD_MEDIA_BASE + 30 //3D��Ƶ���
#define QM_MEDIA_LBUTTONDOWN		 QVOD_MEDIA_BASE + 31 //�������

#define	QM_MEDIA_OPEN_TIMEOUT		 QVOD_MEDIA_BASE + 32  //����Ƶ�ļ���ʱ
#define	QM_MEDIA_INVALID_URL		 QVOD_MEDIA_BASE + 33  //��Ч������
#define	QM_MEDIA_BUFFERING_TIMEOUT	 QVOD_MEDIA_BASE + 34  //���Ż��峬ʱ

#define	QM_BARR_GETFACEPATH			 QVOD_MEDIA_BASE + 50  //��ȡ����·��

//#ifdef INTEL_CUSTOM_DXVA_VERSION
//#define QM_MEDIA_FILE_PLAY_END       QVOD_MEDIA_BASE + 30 //�ļ����Ž���
//#else
#define QM_MEDIA_FILE_PLAY_END       QVOD_MEDIA_BASE + 8 + 0x400
//#endif

#define  QM_MEDIA_QVODSRC               QVOD_MEDIA_BASE + WM_USER   //QvodSource�ص���Ϣ��ʼֵ
#define  QM_UI_MEDIA_MSG_BASE			QM_MEDIA_QVODSRC + 0x100
#define  QM_UI_MEDIA_GET_PARSER_STATE	QM_UI_MEDIA_MSG_BASE + 0x1 //��ȡ��ý����·����״̬��Ϣ

//#define   QVOD_MEDIA_BASE                  0x01       //ý�岿����Ϣ��ʼֵ
//#define   QM_MEDIA_QVODSRC                QVOD_MEDIA_BASE + 0x200   //QvodSource�ص���Ϣ��ʼֵ
//#define   QM_MS_PLAYSTATE                 QVOD_MEDIA_BASE           //����״̬�ı���Ϣ
//#define   QM_MS_SETSRCMODE                QM_MEDIA_QVODSRC + 0x01  //����QvodSource������ģʽ
//#define   QM_MS_PLAYSOURCE                QM_MEDIA_QVODSRC + 0x02 //����ý��Դ
//#define   QM_MS_SETQVODNET                QM_MEDIA_QVODSRC + 0x03  //��������ģ��ָ��
//#define   QM_MS_BUFFERBEGIN               QM_MEDIA_QVODSRC + 0x04
//#define   QM_MS_SEEKING                   QM_MEDIA_QVODSRC + 0x05  //������ק
//#define   QM_MS_BUFFERPER                 QM_MEDIA_QVODSRC + 0x06 //����ǰ����ٷֱ�
//#define   QM_MS_SETPREBUFFERLEN           QM_MEDIA_QVODSRC + 0x07   //����ǰ����
//#define   QM_MS_SETPLAYBUFFERLEN          QM_MEDIA_QVODSRC + 0x08   //�����л�������
//#define   QM_M_GETSOURCEMODE              QM_MEDIA_SRC_QVOD + 0x09       //��ȡ��ǰQvodsource��ģʽ
//#define   QM_MS_SETMEDIACALLBACK          QM_MEDIA_SRC_QVOD + 0x10       //����ý�崦��ģ��ص��ӿ�
//#define   QM_MS_SETBUFFEREVENT            QM_MEDIA_SRC_QVOD + 0x11       //����ý�崦�����¼����
//#define   QM_MS_SETDSCREATEEVENT          QM_MEDIA_SRC_QVOD + 0x12       //����Graph�����¼����
//#define   QM_MS_SETQLIVENET               QM_MEDIA_QVODSRC + 0x13  //����Qliv�ӿ�ָ��
//#define   QM_MS_ISBUFFEREMPTY             QM_MEDIA_QVODSRC + 0x14  //��ѯ�����Ƿ�Ϊ��
//#define   QM_MS_GETCURFILELEN             QM_MEDIA_QVODSRC + 0x15  //��ȡ��ǰ�ļ�����
//#define   QM_MS_SETINDEXPOS               QM_MEDIA_QVODSRC + 0x16  //���õ�ǰ�����ļ�����λ��
//#define   QM_SM_PLAYING_BUFFERPER         QM_MEDIA_QVODSRC + 0x17  //�����л���ٷֱ�
//#define   QM_MS_TEMPSTOP                  QM_MEDIA_QVODSRC + 0x18  //��ʱֹͣ�����ڶ�̬����postvideo��subtitle
//
//#define   QM_MEDIA_SRC_QVOD      QM_MEDIA_QVODSRC + 0x50  //SRC -> Qvod Media
////#define   QM_SM_BUFFERPER        QM_MEDIA_SRC_QVOD
//#define   QM_SM_BUFFERBEGIN        QM_MEDIA_SRC_QVOD + 0x01
//#define   QM_SM_BUFFEREND          QM_MEDIA_SRC_QVOD + 0x02
//#define   QM_SM_BUFFERING          QM_MEDIA_SRC_QVOD + 0x03
//#define   QM_SM_QSOURCE_READY      QM_MEDIA_SRC_QVOD + 0x04       //Qvod source �������
//#define   QM_SM_CONTAINER_TYPE     QM_MEDIA_SRC_QVOD + 0x05  //Qvod source����ý����������
//#define   QM_SM_GRAPH_STOP         QM_MEDIA_SRC_QVOD + 0x06  //ֹͣGraph
//#define   QM_SM_FILE_INDEX         QM_MEDIA_SRC_QVOD + 0x07  //����������ʼλ��

#ifndef INTERFACE_IQVODCALLBACE
#define INTERFACE_IQVODCALLBACE
interface IQvodCallBack
{
public:
	virtual long EventNotify_(UINT uMsg, long wparam = 0, long lparam = 0) = 0;
};
#endif


#endif