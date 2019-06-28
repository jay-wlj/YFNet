#ifndef _QVOD_CALLBACK_MESSAGE_DEFINE_H_
#define _QVOD_CALLBACK_MESSAGE_DEFINE_H_
//QVOD Message Definition
#define QVOD_MESSAGE_BEGIN     0x001            //QVOD 消息取值范围（WM_USER， WM_USER + 0x2000）
#define QVOD_MESSAGE_END       0x2000

#define QVOD_MEDIA_BASE        QVOD_MESSAGE_BEGIN
#define QVOD_UI_BASE           QVOD_MESSAGE_BEGIN + 0x1000
#define WM_GRAPHNOTIFY				WM_USER + 0x100
#define QVOD_DVD_BASE          QVOD_MESSAGE_BEGIN + 0x100

#define QVOD_DVD_EVENT         QVOD_DVD_BASE
//DVD事件中的WPARAM													LPARAM

#define QM_DVD_TITLENUMS							QVOD_DVD_BASE+1		//标题数改变--	ULONG nums
#define QM_DVD_TITLECHANGE							QVOD_DVD_BASE+2		//标题改变		DVD_TITLE_CHANGE------SENDMESSAGE
#define QM_DVD_CHAPTERSCHANGE						QVOD_DVD_BASE+3		//章节改变------ULONG chapters
#define QM_DVD_AUDIO								QVOD_DVD_BASE+4		//配音改变------ULONG
#define QM_DVD_SUBTITLE							    QVOD_DVD_BASE+5		//字幕改变------ULONG
#define QM_DVD_ANGLES								QVOD_DVD_BASE+6		//视角改变------ULONG
#define QM_DVD_DVDPRE								QVOD_DVD_BASE+7		//前一段
#define QM_DVD_DVDNEXT								QVOD_DVD_BASE+8		//后一段
#define QM_DVD_DVDMAINMENU							QVOD_DVD_BASE+9		//主菜单
#define QM_DVD_TITLEMENU							QVOD_DVD_BASE+10		//标题菜单
#define QM_DVD_WORDSMENU							QVOD_DVD_BASE+11		//字幕菜单
#define QM_DVD_AUDIOMENU							QVOD_DVD_BASE+12		//音频菜单
#define QM_DVD_CHAPTERSMENU						    QVOD_DVD_BASE+13		//章节菜单
#define QM_DVD_ANGLEMENU							QVOD_DVD_BASE+14		//视角菜单
#define QM_DVD_CLOSEMENU							QVOD_DVD_BASE+15		//DVD关闭

//IQvodCallBack message
//#define CBM_BASEMESSAGE			0x001
#define QM_MEDIA_PLAYSTATE			QVOD_MEDIA_BASE + 0		//wparam old e_PlayState lparam new e_PlayState
//#define CBM_PLAYNETSTART		QVOD_MEDIA_BASE + 1		//收到该消息，要从主线程发送一个播放网络程序过来
#define QM_MEDIA_LACKDECODER         QVOD_MEDIA_BASE + 2     //解码器（也包括其他Filter）下载提示消息，wparam为E_InfoType：指示是音频还是视频Filter，lparam为解码器名称，LPCTSTR指针，指向一个字符串
//#define CBM_MESSAGENET			0x100
#define QM_MEDIA_MENUUPDATE          QVOD_MEDIA_BASE + 3 //菜单更新消息
#define QM_MEDIA_FILTERNAMELIST      QVOD_MEDIA_BASE + 4 //Filter滤镜菜单消息
#define QM_MEDIA_SUBTITLE            QVOD_MEDIA_BASE + 5 //字幕消息
#define QM_MEDIA_AUDIOCHANNEL        QVOD_MEDIA_BASE + 6 //音频消息
#define QM_MEDIA_GET_MAINHWND        QVOD_MEDIA_BASE + 7//获取播放主窗口句柄
#define QM_MEDIA_HD_START            QVOD_MEDIA_BASE + 8//硬件加速启动
#define QM_MEDIA_HD_STARTFAILED      QVOD_MEDIA_BASE + 9//硬件加速启动失败
#define QM_MEDIA_BUFFER_BEGIN        QVOD_MEDIA_BASE + 10 //播放中缓冲开始
#define QM_MEDIA_BUFFER_END          QVOD_MEDIA_BASE + 11 //播放中缓冲结束
#define QM_MEDIA_RECORDING_END       QVOD_MEDIA_BASE + 12 //文件录制结束
#define QM_MEDIA_FILE_INDEX          QVOD_MEDIA_BASE + 13 //索引长度起始位置（第一个参数为开始位置，第二个参数为长度）
#define QM_MEDIA_RECORD_PER          QVOD_MEDIA_BASE + 14 //视频录制进度
#define QM_MEDIA_RECORD_ERROR        QVOD_MEDIA_BASE + 15 //视频录制错误
#define QM_MEDIA_RECORD_GRAPHREADY   QVOD_MEDIA_BASE + 16 //视频录制建图OK
#define QM_MEDIA_VIDEO_SIZE          QVOD_MEDIA_BASE + 17 //当前播放文件的视频尺寸
#define QM_MEDIA_FILE_PLAY_FAILED    QVOD_MEDIA_BASE + 18 //媒体文件播放失败
#define QM_MEDIA_RBUTTONUP           QVOD_MEDIA_BASE + 19 //WM_RBUTTONUP
#define QM_MEDIA_RBUTTONDOWN         QVOD_MEDIA_BASE + 20 //WM_RBUTTONDOWN
#define QM_MEDIA_MOUSEMOVE           QVOD_MEDIA_BASE + 21 //WM_MOUSEMOVE
#define QM_MEDIA_LBUTTONDBLCLK       QVOD_MEDIA_BASE + 22 //WM_LBUTTONDBLCLK
#define QM_MEDIA_LBUTTONUP           QVOD_MEDIA_BASE + 23 //WM_LBUTTONUP
#define QM_MEDIA_HOTKEY              QVOD_MEDIA_BASE + 24 //WM_HOTKEY
#define QM_MEDIA_NOAUDIORENDER       QVOD_MEDIA_BASE + 25 //没有音频输出设备
#define QM_MEDIA_KEYDOWN             QVOD_MEDIA_BASE + 26 //WM_KEYDOWN
#define QM_MEDIA_LACK_FILTER         QVOD_MEDIA_BASE + 27 //filter缺失
#define QM_MEDIA_BUFFERING_PER       QVOD_MEDIA_BASE + 28 //播放中缓冲百分率
#define QM_MEDIA_FRAME_DRAW			 QVOD_MEDIA_BASE + 29 //刷出画面
#define QM_RQ_D3DVIDEO_DETECT		 QVOD_MEDIA_BASE + 30 //3D视频检测
#define QM_MEDIA_LBUTTONDOWN		 QVOD_MEDIA_BASE + 31 //左键按下

#define	QM_MEDIA_OPEN_TIMEOUT		 QVOD_MEDIA_BASE + 32  //打开视频文件超时
#define	QM_MEDIA_INVALID_URL		 QVOD_MEDIA_BASE + 33  //无效的链接
#define	QM_MEDIA_BUFFERING_TIMEOUT	 QVOD_MEDIA_BASE + 34  //播放缓冲超时

#define	QM_BARR_GETFACEPATH			 QVOD_MEDIA_BASE + 50  //获取表情路径

//#ifdef INTEL_CUSTOM_DXVA_VERSION
//#define QM_MEDIA_FILE_PLAY_END       QVOD_MEDIA_BASE + 30 //文件播放结束
//#else
#define QM_MEDIA_FILE_PLAY_END       QVOD_MEDIA_BASE + 8 + 0x400
//#endif

#define  QM_MEDIA_QVODSRC               QVOD_MEDIA_BASE + WM_USER   //QvodSource回调消息起始值
#define  QM_UI_MEDIA_MSG_BASE			QM_MEDIA_QVODSRC + 0x100
#define  QM_UI_MEDIA_GET_PARSER_STATE	QM_UI_MEDIA_MSG_BASE + 0x1 //获取流媒体链路解析状态信息

//#define   QVOD_MEDIA_BASE                  0x01       //媒体部分消息起始值
//#define   QM_MEDIA_QVODSRC                QVOD_MEDIA_BASE + 0x200   //QvodSource回调消息起始值
//#define   QM_MS_PLAYSTATE                 QVOD_MEDIA_BASE           //播放状态改变消息
//#define   QM_MS_SETSRCMODE                QM_MEDIA_QVODSRC + 0x01  //设置QvodSource的驱动模式
//#define   QM_MS_PLAYSOURCE                QM_MEDIA_QVODSRC + 0x02 //播放媒体源
//#define   QM_MS_SETQVODNET                QM_MEDIA_QVODSRC + 0x03  //设置网络模块指针
//#define   QM_MS_BUFFERBEGIN               QM_MEDIA_QVODSRC + 0x04
//#define   QM_MS_SEEKING                   QM_MEDIA_QVODSRC + 0x05  //设置拖拽
//#define   QM_MS_BUFFERPER                 QM_MEDIA_QVODSRC + 0x06 //播放前缓冲百分比
//#define   QM_MS_SETPREBUFFERLEN           QM_MEDIA_QVODSRC + 0x07   //播放前缓冲
//#define   QM_MS_SETPLAYBUFFERLEN          QM_MEDIA_QVODSRC + 0x08   //播放中缓冲设置
//#define   QM_M_GETSOURCEMODE              QM_MEDIA_SRC_QVOD + 0x09       //获取当前Qvodsource的模式
//#define   QM_MS_SETMEDIACALLBACK          QM_MEDIA_SRC_QVOD + 0x10       //设置媒体处理模块回调接口
//#define   QM_MS_SETBUFFEREVENT            QM_MEDIA_SRC_QVOD + 0x11       //设置媒体处理缓冲事件句柄
//#define   QM_MS_SETDSCREATEEVENT          QM_MEDIA_SRC_QVOD + 0x12       //设置Graph创建事件句柄
//#define   QM_MS_SETQLIVENET               QM_MEDIA_QVODSRC + 0x13  //设置Qliv接口指针
//#define   QM_MS_ISBUFFEREMPTY             QM_MEDIA_QVODSRC + 0x14  //查询缓冲是否为空
//#define   QM_MS_GETCURFILELEN             QM_MEDIA_QVODSRC + 0x15  //获取当前文件长度
//#define   QM_MS_SETINDEXPOS               QM_MEDIA_QVODSRC + 0x16  //设置当前网络文件索引位置
//#define   QM_SM_PLAYING_BUFFERPER         QM_MEDIA_QVODSRC + 0x17  //播放中缓冲百分比
//#define   QM_MS_TEMPSTOP                  QM_MEDIA_QVODSRC + 0x18  //暂时停止，用于动态加载postvideo和subtitle
//
//#define   QM_MEDIA_SRC_QVOD      QM_MEDIA_QVODSRC + 0x50  //SRC -> Qvod Media
////#define   QM_SM_BUFFERPER        QM_MEDIA_SRC_QVOD
//#define   QM_SM_BUFFERBEGIN        QM_MEDIA_SRC_QVOD + 0x01
//#define   QM_SM_BUFFEREND          QM_MEDIA_SRC_QVOD + 0x02
//#define   QM_SM_BUFFERING          QM_MEDIA_SRC_QVOD + 0x03
//#define   QM_SM_QSOURCE_READY      QM_MEDIA_SRC_QVOD + 0x04       //Qvod source 缓冲完成
//#define   QM_SM_CONTAINER_TYPE     QM_MEDIA_SRC_QVOD + 0x05  //Qvod source返回媒体容器类型
//#define   QM_SM_GRAPH_STOP         QM_MEDIA_SRC_QVOD + 0x06  //停止Graph
//#define   QM_SM_FILE_INDEX         QM_MEDIA_SRC_QVOD + 0x07  //索引长度起始位置

#ifndef INTERFACE_IQVODCALLBACE
#define INTERFACE_IQVODCALLBACE
interface IQvodCallBack
{
public:
	virtual long EventNotify_(UINT uMsg, long wparam = 0, long lparam = 0) = 0;
};
#endif


#endif