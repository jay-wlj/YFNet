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

// Task�ĵ�ǰ����״̬
#define TASK_NO_ERROR						(0x00000000)						//��ʼֵ,���쳣
#define TASK_WRITE_FAILED					(0x00000001)						//д��ʧ��
#define TASK_FILE_NOT_FOUND					(TASK_WRITE_FAILED << 1)			//�Ҳ����ļ�·��
#define TASK_WRITE_NO_SPACE					(TASK_WRITE_FAILED << 2)			//���̿ռ䲻��

class CTask : public RefCountedObject
{
public:
	// ����״̬
	enum Status
	{
		RUNNING	= 1,	//����
		PAUSE	= 2,	//��ͣ
		STOP	= 3,	//ֹͣ
		PLAY	= 4,	//����
		WAIT	= 5		//�ȴ�
	};

	// ����ģʽ
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
	// �ڴ�沥��λ��֮���Ƿ��Ѿ���5M����
	virtual bool				IsNeedDownload();
	virtual void				DelAllMemPieces();
	virtual QVOD_INT64				GetSuperNodeDownLen();
#endif

	// �����ۼӻ���ߴ�
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
	
	// �ı������ݿ��е�״̬
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

	// �������û��������,ֱ��ɾ������ͻ����ļ�;
	virtual inline void			SetReservedFile(){m_bIsReservedFile = true;}
	virtual int					GetDownloadingIndex(CPeer *pPeer);

	virtual int					SetStopPlay();
	virtual int					SetStartPlay();

	// �������Ƿ����ڲ���
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


	// ��ȡ��ǰ����Ĵ�������ֵ
	virtual unsigned int		GetTaskErrorMask();
	virtual void				ResetTaskErrorMask(){m_iErrorMask = 0;}
	
	//��ȡ�����������յ�piece��д��ʧ�ܵ�ԭ�򼯺�
	virtual unsigned int		GetWriteFailReason(){return m_iWriteFailReason;}
	virtual void				SetWriteFailReason(int pos);
	virtual void				ResetWriteFailReason();


	// �Ʒ�����ģʽ����ӿ�
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

	virtual int					DeleteHasPlayingData(QVOD_UINT64 iCurCacheSize) {return 0;};		// ɾ����ǰ����λ��ǰ�Ļ�������

	//m3u8�߼�
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
	// �ͷŷ�Ƭ��Ϣ
	virtual void				ReleasePieces();
	// ��ȡ˳�����ص���һ����Ƭ
	virtual bool				IsFileSizeMatching();
	virtual int					GetOrderIndex();	
	// ��ȡ������ص���һ����Ƭ
	virtual int					GetRandomIndex();

public:
	time_t						m_lastStartTime;
	TaskCfg						m_Para;					//Task��Ϣ����
	UINT						m_nlocalseeds;			//
	int							m_fileindex;			//�����ļ���������
	unsigned int				m_playfileIndex;		//��ǰ�����ļ������һƬ
	vector<CQvodFile*>	m_vctFile;				//�ļ��б�
	CLock						m_vctFileLock;			//�ļ��б���

	QVOD_INT64						m_curDownSize;			//��ǰ���س���
	QVOD_INT64						m_curUpSize;			//��ǰ�ϴ�����

#ifdef NETBAR
	map<int, PieceBuffer>		m_mapMemPiece;			//�ڴ��洢Ƭ
	map<int, PieceBuffer>		m_mapMemIndexPieces;	//�ڴ��洢�ļ�������
	int							m_MaxPieces;			//�ڴ�������ɴ洢��Ƭ��
	int							m_MaxIndexPieces;		//�ڴ�������������Ƭ��
	bool						m_isNeedDownload;		//�ڴ���б�ʾ�Ƿ���Ҫ��������
	int							m_playerIndex;			//�ڴ�沥������ȡ����λ��
	CLock						m_lockMemPiece;
	CLock						m_lockMemIndex;
#endif
	CPeerGroup*					m_peers;
	map<int,CPiece*>::iterator  m_mapPieceItr;
	map<int,CPiece*>			m_mapPiece;
	CLock						m_lockPiece;
	
	std::string					m_strPath;				//�ļ�����λ��
	std::string					m_strFileName;			//������

	//CM3u8 *						m_m3u8;					//m3u8�ļ�
	CSeed *						m_seed;					//�����ļ���Ϣ
	CMem *						m_mem;					//���ؽ����ļ�
	CLock						m_seedMemLock;

	unsigned char*				m_downQue;				//���ض���
	QVOD_INT64						m_buffingpos;			//����λ��
	unsigned int				m_dragindex;			//�������϶�Ƭλ��
	QVOD_INT64						m_dragpos;				//�������϶�λ��

	// ���ٲ���
	unsigned long				m_nextDownTime;
	unsigned int				m_downQueueTime;

	bool						m_accHttpReqStatus;
	bool						m_bIsUpdate;
	int							m_httpDownLen;
	bool						m_bIsReservedFile;		//�Ƿ����ļ�,���Ϊtrue,������ɺ�,ɾ������ʱ��ɾ�����Ӻ��ļ�
	bool						m_bIsHide;
	CLock						m_downReqInfoLock;
	unsigned int				m_iErrorMask;			//��ǰ����Ĵ�������:0x01=>д��ʧ��;(0x01 << 1)=>�ļ�������

	unsigned int				m_lastDownSize;
	unsigned int				m_limitDownSpeed;		//task���ٶ�,Ĭ��0,������
	unsigned int				m_xSecDownBufSize;
	unsigned int				m_xSecUpBufSize;
	
	bool						m_bIsPlaying;			//�������Ƿ����ڲ���
	bool						m_bBuffering;			// ��ǰ���񲥷��Ƿ����ڿ��ٻ���
	unsigned int				m_iPlayFileRate;		//�����ļ�������,��λKbps
	int							m_iVideoDuration;		//��Ƶʱ������λ��
	bool						m_bOptimizeUpload;		//����λ���Ƿ񳬹�����λ��10����

	bool						m_bHeaderIsFinished;	//�ļ�ͷ(ǰ10%������)�Ƿ��������
	bool						m_bEndingIsFinished;	//�ļ�β(��1MB������)�Ƿ��������

	QVOD_INT64			m_iPlayPos;				//��ǰ����λ��


	unsigned int				m_iWriteFailReason;		//�յ�Piece��дʧ�ܵ�ԭ�򼯺�

	unsigned int				m_iEncodeKey;			//.qdata�ļ��ļ���key

#ifdef MONGOTV
	char						m_vid[33];
	bool						m_bIsOtherCdn;
	bool						m_bIsStartDoSended;		//���ǵ�����cdn���������ܹ����ص����ݺ�Ҫ����start.do
#endif

	int							m_iDownloadMode;				//����ģʽ
	int							m_iHttpDownloadingBeginIndex;	//http��ʽ�������ص���ʼ���
	int							m_iHttpDownloadingEndIndex;		//http��ʽ�������صĽ������
	CLock						m_downNextHttpPieceLock;		//DownNextHttpPiece����

};
#endif
