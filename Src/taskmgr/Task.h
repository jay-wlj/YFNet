#ifndef __TASK_H__
#define __TASK_H__

#pragma warning(disable: 4786)
#ifdef WIN32
#include <io.h>
#include <direct.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <time.h>

#include <fcntl.h>

#include <sys/stat.h>
using namespace std;

#include "qvod_lock.h"
#include "common-piece.h"
#include "protocol.h"
#include "common-commonstruct.h"
#include "terminal.h"
#include "BinaryWriter.h"
#include "common-RefCountedObject.h"
#include "common-AutoPtr.h"

#include "Mem.h"
#include "QvodFile.h"
#include "Peer.h"
#include "Seed.h"
#include "PeerGroup.h"
#include "M3u8.h"

#ifdef NETBAR
struct PieceBuffer
{
	int len;//piece len
	char* buf;
};
#endif

// Task的当前错误状态
#define TASK_NO_ERROR						(0x00000000)						//初始值,无异常
#define TASK_WRITE_FAILED					(0x00000001)						//写入失败
#define TASK_FILE_NOT_FOUND					(TASK_WRITE_FAILED << 1)			//找不到文件路径
#define TASK_WRITE_NO_SPACE					(TASK_WRITE_FAILED << 2)			//磁盘空间不足

class CTask : public RefCountedObject
{
public:
	// 任务状态
	enum Status
	{
		RUNNING	= 1,	//运行
		PAUSE	= 2,	//暂停
		STOP	= 3,	//停止
		PLAY	= 4,	//播放
		WAIT	= 5		//等待
	};

	// 下载模式
	enum DownloadMode
	{
		MODE_CDN,
		MODE_PAUSE
	};

	CTask();
	virtual ~CTask();
	
public:	
	virtual int					run(bool bload=0) = 0;

public:
	virtual void				Start();
	virtual void				Pause();
	virtual void				Stop();

	virtual void				UpdatePieceFinish(int iIndex);

	virtual bool				IsIndexDownload(int index);
	virtual int					DownNextPiece(const KEY& key,request& req);
	virtual bool				DownNextPiece(const KEY& key,httpiecereq& hreq);//HTTP FOR P2P

	virtual bool				IsExistEmergencyPiece();
	//virtual bool				GetEmergencyPiece(request& req);

	virtual int					GetNextIndex(CPeer* pPeer);
	virtual int					GetNextIndexEx(CPeer* pPeer);
	virtual	bool				SetDownQue(unsigned int index);
	virtual bool				CancelDownQue(int index);
	virtual unsigned char*		GetHash();

	virtual bool				TaskIsFinish(bool bCheckFileExist = false);
	virtual bool				Rename();

 	virtual unsigned char		GetSeedLimit(void) {return m_Para.seedslmt ;}
	virtual unsigned char		GetAssertdownload(void){return m_Para.bcheckkey & 0x01;}
	virtual unsigned char		GetAssertlink(void){return m_Para.bcheckkey & 0x02;}

	virtual int					WritePiece(const KEY& key,const char *buf, int len, unsigned int index, unsigned int start);
	virtual int					ReadData(char *buf, UINT len, unsigned int index, unsigned int start,bool bIsUpload) { return 0;}
	virtual int					WriteData(const char *buf, UINT len, unsigned int index, unsigned int start)	{return 0;}

	virtual void				SetStatus(Status status) {m_Para.status = status;}
	virtual Status				GetStatus(){return (Status)m_Para.status;}

	CPeerGroup*					ProcessQueryPeers(){return m_peers;}
	virtual CSeed*				GetSeed(void);
	virtual TaskType			GetTaskType();
	virtual string				GetFileName() {return m_strFileName == ""?m_seed->GetFileName():m_strFileName;}

	virtual CMem*				GetMemFile(void);
	virtual CQvodFile*			GetFile(unsigned int pos = 0){ return m_vctFile.size() > pos ? m_vctFile[pos] : NULL;}

	virtual int					SetFileIndex(unsigned char index);
	virtual void				SetBuffingPos(QVOD_INT64 setpos);
	virtual void				UpdateBuffingPos(QVOD_INT64 setpos);

	virtual string				GetFilePath(){return m_strPath;}
	virtual void				RemoveTmpFile(){return;}

	virtual void				CheckTimeStamp();

	int							GetPeerIspID(const KEY &key,unsigned char *pIspID);
	int							SetPeerIspID(const KEY &key,unsigned char *pIspID);
	bool						IsOptimizeUpload();

#ifdef NETBAR
	// 内存版播放位置之后是否已经有5M内容
	virtual bool				IsNeedDownload();
	virtual void				DelAllMemPieces();
	virtual QVOD_INT64				GetSuperNodeDownLen();
#endif

	// 设置累加缓冲尺寸
	virtual void				InterDownSize(int size,const KEY &key);
	virtual void				InterUpSize(int size,const KEY &key);
	virtual bool				IsNeedCacheDownRequest();
	virtual bool				IsOvertakeDownSpeed();
	virtual void				UpdateNextDownTime(int lastSize = 0);
	virtual int					GetDownSpeed();
	virtual void				SetDownSpeed(int speed);
	virtual int					GetUploadSpeed();
	virtual unsigned int		GetCurDownNum(KEY& key);
	virtual bool				CancelTimeoutReq(KEY& key,int index,int begin,int length);

	virtual bool				CancelPeerIndexInfo(KEY& key);
	virtual bool				CancelIndexInfo(KEY& key,int index,int begin);
	virtual bool				IsValidPiecePacket(KEY& key,int index,int begin,int length);
	virtual void				SetReqPiecePacketInfo(CPeer* pPeer,int index,int begin);

	virtual unsigned int		GetPlayfileIndex(void) {return m_playfileIndex;}
	virtual void				SetPlayFileIndex(void);

	virtual bool				FindPiece(int index,CPiece** piece);
	virtual bool				DelPiece(int  index);
	virtual void				SetHide() { m_bIsHide = true; }
	virtual bool				GetHide() { return m_bIsHide; }

	virtual QVOD_INT64				GetCurDownLen() { return m_curDownSize;}

	virtual int					GetBufPieceSize();
	
	// 改变在数据库中的状态
	virtual bool				SetStatusInDataBase(unsigned char status);

	virtual int					GetHttpDownLen(){ return m_httpDownLen;}
	virtual void				InterHttpDownLen(int len){ m_httpDownLen += len;}

	virtual QVOD_INT64				GetFileSize();
	virtual int					GetPieceNum();
	virtual bool				FullPeerBitField(KEY& key);
	virtual int					SetReferUrl(const std::string &strRef);
	virtual string				GetReferUrl(){return m_Para.strRefUrl;}
	virtual long				GetCreateTime(){return m_Para.maketime;}

	virtual QVOD_INT64				GetDragPos(){return m_dragpos;}
	virtual int					SetDownloadPos(QVOD_INT64 iPos,bool fromUser);
	virtual int					GetDownloadPos(QVOD_UINT64& iPos);
	virtual int					SetPlayFileIndexPos(QVOD_INT64 iPos);

	// 如果任务没有下载完,直接删除任务和缓存文件;
	virtual inline void			SetReservedFile(){m_bIsReservedFile = true;}
	virtual int					GetDownloadingIndex(CPeer *pPeer);

	virtual int					SetStopPlay();
	virtual int					SetStartPlay();

	// 该任务是否正在播放
	virtual int					SetPlaying(bool bPlaying);
	virtual bool				IsPlaying();	
	virtual QVOD_INT64				QueryContinueLen(QVOD_INT64 pos);

	virtual int					SetPlayFileRate(unsigned int iRate);
	virtual unsigned int		GetPlayFileRate();
	virtual void				SetVideoDuration(int iDuration){m_iVideoDuration = iDuration;}
	virtual void				NotifyVideoBuffering(bool bBuffering) { m_bBuffering = bBuffering;}
	virtual int					GetVideoDuration(){return m_iVideoDuration;}
	virtual int					SetPlayFilePos(QVOD_INT64 pos);
	virtual QVOD_INT64				GetPlayFilePos(){return m_iPlayPos;}
	virtual QVOD_INT64				GetUnDownPosNearPlayPos();


	// 获取当前任务的错误掩码值
	virtual unsigned int		GetTaskErrorMask();
	virtual void				ResetTaskErrorMask(){m_iErrorMask = 0;}
	
	//获取和设置任务收到piece后写入失败的原因集合
	virtual unsigned int		GetWriteFailReason(){return m_iWriteFailReason;}
	virtual void				SetWriteFailReason(int pos);
	virtual void				ResetWriteFailReason();


	// 云帆下载模式任务接口
	virtual void				SetFilesize(QVOD_INT64 iFilesize);
	virtual QVOD_INT64			GetDownloadedSize();
	virtual int					GetDownloadMode(){return m_iDownloadMode;}

	virtual void				ChangeTaskType(int type);
	virtual void				UpdateHttpOrgUrl(string& strHttpOrgUrl);
    virtual int	                UpdateClarityUrl(string& strHttpOrgUrl, string& strHash, string& strNewPath, int nClarity) { return -1;};
	virtual int					SetHeadValue(const char *pstHead, const char* pstrValue) {return -1;}
	virtual int					SetHeadValue(const char *pstrHeadAdd) {return -1;};
	virtual int					SetCopyFullFile(string& strCopyPath);
	virtual int					UnsetCopyFullFile();
	virtual bool				IsDownlloadTask();		
	virtual void				SetCopyFullFileStatus(int status);
	virtual QVOD_INT64				GetTotalDownloadLen();
	virtual QVOD_INT64				GetTotalUploadLen();

	virtual void				UpdateDownloadMode(){};
	virtual void				DownEmergencyData(){};
	virtual int					WriteHttpData(const char *buf, QVOD_UINT64 start,UINT len ){return 0;}
	virtual int					DownNextHttpPiece(YfHttpPieceReq& req,bool bRandom = false){return 0;};
	virtual string				QueryTaskInfo() { return "";}
	virtual void				NotifyDownload(const int &nId) {return;}
#ifdef MONGOTV
	virtual char*				GetVid(){return m_vid;}
	virtual bool				IsUrlOtherCdn(){return m_bIsOtherCdn;}
	virtual void				SetUrlCdnType(bool bIsOtherCdn);
	virtual bool				IsStartDoSended(){return m_bIsStartDoSended;}
	virtual void				SetStartDoSended(bool bIsStartDoSended){m_bIsStartDoSended = bIsStartDoSended;}
#endif

	virtual int					DeleteHasPlayingData(QVOD_UINT64 iCurCacheSize) {return 0;};		// 删除当前播放位置前的缓存数据

	//m3u8逻辑
	virtual int					WriteM3u8Ts(const string& strFilename,const char* buf,int start,int len,int filesize){ return 0;}
	virtual int					ReadM3u8Ts(const string& strFilename,char* buf,int start,int len,bool bIsUpload){ return 0;}
	virtual int					DownNextM3u8Ts(SM3u8TsReq& req, const int& iTsIndex = -1) { return 0; }
	virtual void				NotifyDownM3u8Ts(const string& strFilename) { }
	virtual void				SetM3u8PlayingPos(const string& strTsName,int pos) {}
	virtual bool				IsPlayTsContinue() {return false;} 
	virtual QVOD_INT64			GetM3u8TsSize(const string& strName) {return 0;}
	virtual bool                IsExistM3u8Ts(string strTsUrl){return false;}
	virtual void				ClosePush(const string& strFilename){}
protected:
	// 释放分片信息
	virtual void				ReleasePieces();
	// 获取顺序下载的下一个分片
	virtual bool				IsFileSizeMatching();
	virtual int					GetOrderIndex();	
	// 获取随机下载的下一个分片
	virtual int					GetRandomIndex();

public:
	time_t						m_lastStartTime;
	TaskCfg						m_Para;					//Task信息集合
	UINT						m_nlocalseeds;			//
	int							m_fileindex;			//播放文件的索引号
	unsigned int				m_playfileIndex;		//当前播放文件的最后一片
	vector<CQvodFile*>	m_vctFile;				//文件列表
	CLock						m_vctFileLock;			//文件列表锁

	QVOD_INT64						m_curDownSize;			//当前下载长度
	QVOD_INT64						m_curUpSize;			//当前上传长度

#ifdef NETBAR
	map<int, PieceBuffer>		m_mapMemPiece;			//内存版存储片
	map<int, PieceBuffer>		m_mapMemIndexPieces;	//内存版存储文件的索引
	int							m_MaxPieces;			//内存版中最多可存储的片数
	int							m_MaxIndexPieces;		//内存版中索引的最多片数
	bool						m_isNeedDownload;		//内存版中标示是否需要继续下载
	int							m_playerIndex;			//内存版播放器读取到的位置
	CLock						m_lockMemPiece;
	CLock						m_lockMemIndex;
#endif
	CPeerGroup*					m_peers;
	map<int,CPiece*>::iterator  m_mapPieceItr;
	map<int,CPiece*>			m_mapPiece;
	CLock						m_lockPiece;
	
	std::string					m_strPath;				//文件保存位置
	std::string					m_strFileName;			//任务名

	//CM3u8 *						m_m3u8;					//m3u8文件
	CSeed *						m_seed;					//种子文件信息
	CMem *						m_mem;					//下载进度文件
	CLock						m_seedMemLock;

	unsigned char*				m_downQue;				//下载队列
	QVOD_INT64						m_buffingpos;			//下载位置
	unsigned int				m_dragindex;			//播放器拖动片位置
	QVOD_INT64						m_dragpos;				//播放器拖动位置

	// 限速部分
	unsigned long				m_nextDownTime;
	unsigned int				m_downQueueTime;

	bool						m_accHttpReqStatus;
	bool						m_bIsUpdate;
	int							m_httpDownLen;
	bool						m_bIsReservedFile;		//是否保留文件,如果为true,下载完成后,删除任务时不删除种子和文件
	bool						m_bIsHide;
	CLock						m_downReqInfoLock;
	unsigned int				m_iErrorMask;			//当前任务的错误掩码:0x01=>写入失败;(0x01 << 1)=>文件不存在

	unsigned int				m_lastDownSize;
	unsigned int				m_limitDownSpeed;		//task的速度,默认0,不限制
	unsigned int				m_xSecDownBufSize;
	unsigned int				m_xSecUpBufSize;
	
	bool						m_bIsPlaying;			//该任务是否正在播放
	bool						m_bBuffering;			// 当前任务播放是否正在卡顿缓冲
	unsigned int				m_iPlayFileRate;		//播放文件的码率,单位Kbps
	int							m_iVideoDuration;		//视频时长，单位秒
	bool						m_bOptimizeUpload;		//下载位置是否超过播放位置10分钟

	bool						m_bHeaderIsFinished;	//文件头(前10%的数据)是否下载完成
	bool						m_bEndingIsFinished;	//文件尾(后1MB的数据)是否下载完成

	QVOD_INT64			m_iPlayPos;				//当前播放位置


	unsigned int				m_iWriteFailReason;		//收到Piece后写失败的原因集合

	unsigned int				m_iEncodeKey;			//.qdata文件的加密key

#ifdef MONGOTV
	char						m_vid[33];
	bool						m_bIsOtherCdn;
	bool						m_bIsStartDoSended;		//若是第三方cdn，在链接能够下载到数据后要发送start.do
#endif

	int							m_iDownloadMode;				//下载模式
	int							m_iHttpDownloadingBeginIndex;	//http方式正在下载的起始块号
	int							m_iHttpDownloadingEndIndex;		//http方式正在下载的结束块号
	CLock						m_downNextHttpPieceLock;		//DownNextHttpPiece的锁

};
#endif
