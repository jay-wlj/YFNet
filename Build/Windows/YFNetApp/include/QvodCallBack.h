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
#define   QVOD_MEDIA_BASE                  0x01       //媒体部分消息起始值
#define   QM_MEDIA_QVODSRC                QVOD_MEDIA_BASE + WM_USER   //QvodSource回调消息起始值
#define   QM_MS_PLAYSTATE                 QVOD_MEDIA_BASE           //播放状态改变消息
#define   QM_MS_SETSRCMODE                QM_MEDIA_QVODSRC + 0x01  //设置QvodSource的驱动模式
#define   QM_MS_PLAYSOURCE                QM_MEDIA_QVODSRC + 0x02 //播放媒体源
#define   QM_MS_SETQVODNET                QM_MEDIA_QVODSRC + 0x03  //设置网络模块指针
#define   QM_MS_BUFFERBEGIN               QM_MEDIA_QVODSRC + 0x04
#define   QM_MS_SEEKING                   QM_MEDIA_QVODSRC + 0x05  //设置拖拽
#define   QM_MS_BUFFERPER                 QM_MEDIA_QVODSRC + 0x06
#define   QM_MS_SETPREBUFFERLEN           QM_MEDIA_QVODSRC + 0x07   //播放前缓冲
#define   QM_MS_SETPLAYBUFFERLEN          QM_MEDIA_QVODSRC + 0x08   //播放中缓冲设置
#define   QM_M_GETSOURCEMODE              QM_MEDIA_SRC_QVOD + 0x09       //获取当前Qvodsource的模式
#define   QM_MS_SETMEDIACALLBACK          QM_MEDIA_SRC_QVOD + 0x10       //设置媒体处理模块回调接口
#define   QM_MS_SETBUFFEREVENT            QM_MEDIA_SRC_QVOD + 0x11       //设置媒体处理缓冲事件句柄
#define   QM_MS_SETDSCREATEEVENT          QM_MEDIA_SRC_QVOD + 0x12       //设置Graph创建事件句柄
#define   QM_MS_SETQLIVENET               QM_MEDIA_QVODSRC + 0x13  //设置Qliv接口指针
#define   QM_MS_ISBUFFEREMPTY             QM_MEDIA_QVODSRC + 0x14  //查询缓冲是否为空
#define   QM_MS_GETCURFILELEN             QM_MEDIA_QVODSRC + 0x15  //获取当前文件长度
#define   QM_MS_SETINDEXPOS               QM_MEDIA_QVODSRC + 0x16  //设置当前网络文件索引位置
#define   QM_SM_PLAYING_BUFFERPER         QM_MEDIA_QVODSRC + 0x17  //播放中缓冲百分比
#define   QM_MS_TEMPSTOP                  QM_MEDIA_QVODSRC + 0x18  //暂时停止，用于动态加载postvideo和subtitle
#define   QM_MS_QUERY_DOWNLOAD_LEN        QM_MEDIA_QVODSRC + 0x19  //查询文件下载长度




#define   QM_MEDIA_SRC_QVOD      QM_MEDIA_QVODSRC + 0x50  //SRC -> Qvod Media
//#define   QM_SM_BUFFERPER        QM_MEDIA_SRC_QVOD
#define   QM_SM_BUFFERBEGIN        QM_MEDIA_SRC_QVOD + 0x01
#define   QM_SM_BUFFEREND          QM_MEDIA_SRC_QVOD + 0x02
#define   QM_SM_BUFFERING          QM_MEDIA_SRC_QVOD + 0x03
#define   QM_SM_QSOURCE_READY      QM_MEDIA_SRC_QVOD + 0x04       //Qvod source 缓冲完成
#define   QM_SM_CONTAINER_TYPE     QM_MEDIA_SRC_QVOD + 0x05  //Qvod source返回媒体容器类型
#define   QM_SM_GRAPH_STOP         QM_MEDIA_SRC_QVOD + 0x06  //停止Graph
#define   QM_SM_FILE_INDEX         QM_MEDIA_SRC_QVOD + 0x07  //停止Graph

enum StreamMode{
	NULL_Mode,
	Push_Mode,     //推模式
	Pull_Mode      //拉模式
};