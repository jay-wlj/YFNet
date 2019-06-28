
#ifndef __TASK_INTERFACE_H__
#define __TASK_INTERFACE_H__

#pragma warning(disable : 4786)

#include <vector>
#include <string>
#include <list>
#include <set>
using namespace std;

#include "qvod_lock.h"
#include "protocol.h"
#include "terminal.h"
#include "IYfNetPlush.h"
#include "common-commonstruct.h"
#include "BinaryWriter.h"
#include "Task.h"
#include "qvod_string.h"
#include "common-callback.h"

class CPeer;

class CTaskMgrInterFace:public ISpeedCallBack
{
public:
	static CTaskMgrInterFace*	Instance();
	static void					FreeInstance();

protected:
	CTaskMgrInterFace(){};
	virtual ~CTaskMgrInterFace(){};

private:
	static CTaskMgrInterFace * _instance;

public:
	//��ȡ���������hash�������������ߵ�����
	virtual bool GetCacheHash(set<HASH>& hashSet) = 0;
	virtual int DeleteTaskByFolder(std::string strfolder) = 0;

	//��д�����ļ�
	virtual int WritePiece(const HASH& hash, KEY& key,const char* buf, QVOD_UINT64 len, unsigned int index, QVOD_UINT64 pos) = 0;
	virtual int WritePiece(const HASH& hash, const char* buf, UINT len, QVOD_UINT64 pos) = 0;
	virtual int ReadPiece(const HASH& hash, char *buf, UINT len, QVOD_UINT64 pos,bool bIsUpload = true) = 0;

#ifdef SUPERNODE
	virtual bool ReadPiece(const HASH& hash, KEY& key,char *buf, int len, unsigned int index ,unsigned int start) = 0;
	virtual int DiskIoFinished(HASH& hash, KEY& key,char*& buf, int& len, unsigned int& index, unsigned int& start,int& iFlag) = 0;
#endif	

#ifdef NETBAR
	virtual bool IsNeedDownload(HASH& hash) = 0;//�ڴ���ж������Ƿ���Ҫ���㹻�ռ������
#endif

	virtual int  DownNextPiece(const HASH& hash,KEY& key,request& req)=0;
	virtual bool DownNextPiece(const HASH& hash,KEY& key,httpiecereq& hreq)=0;//HTTP FOR P2P
	
	//���� true ��� false ʧ��
	virtual bool TaskIsFinish(const HASH& hash, bool bCheckFileExist = false) = 0;


	//bitFiled=NULL �˺���Ϊֻȡ����
	virtual bool GetTaskBitField(const HASH& hash, unsigned char*bitFiled, unsigned int &len) = 0; 	

	//����ɾ��һ��PEERS��Ϣ
	virtual int AddOnePeer(const HASH& hash, const KEY& key, unsigned int PublicIP,unsigned short PublicPort,
							bool isDownPeer,unsigned char nattype,unsigned char nettype,char PeerID[20],
							PEERTYPE peertype = NORMAL) = 0;

	virtual int AddOneQvodServer(const HASH &hash,KEY& key,unsigned char natType,unsigned char netType,char szPeerID[20],
								 QVOD_UINT64 iBeginPos,unsigned int iDataLen,PEERTYPE peerType = NORMAL) = 0;
	virtual int DelOnePeer(const HASH& hash, KEY& key) = 0;
	virtual int CreatePeerBitField(const HASH& hash, KEY& key, unsigned char*bit, int len) = 0;
	virtual int SetPeerBitField(const HASH& hash, KEY& key, unsigned int index) = 0;
	virtual int ResetPeerBitField(const HASH& hash,KEY& key,unsigned int index) = 0;
	virtual int SetPeerID(const HASH& hash, const KEY& key, char pid[20]) = 0;
	virtual bool SetPeerType(const HASH& hash, const KEY& key,USERTYPE type) = 0;
	virtual int GetPeerID(const HASH& hash, const KEY& key, char pid[20]) = 0;
	virtual int GetPeerType(const HASH& hash, const KEY& key) = 0;
 	virtual unsigned char GetPeerNatType(const HASH& hash, const KEY& key) = 0;
 	virtual unsigned char GetPeerNetType(const HASH& hash, const KEY& key) = 0;
	virtual unsigned char GetPeerTimeoutCount(const HASH& hash, const KEY& key) = 0;
	virtual int GetPeerVersion(const HASH& hash, const KEY& key) = 0;
	virtual bool IsSuperPeerNode(const HASH& hash, const KEY& key) = 0;
	virtual bool IsDownPeer(const HASH& hash, const KEY& key)=0;
	virtual int GetPeerNum(const HASH & hash,PEERTYPE type = ALLTYPE) = 0;
	virtual bool IsConnect(const HASH & hash, unsigned int ip, short port) = 0;
	virtual bool SetPeerCacheServer(const HASH& hash, const KEY& key) = 0;
	virtual bool IsCacheServer(const HASH& hash, const KEY& key) = 0;
	virtual bool IsMinerServer(const HASH& hash, const KEY& key) = 0;

	virtual int  QueryPeerStatus(const HASH& hash,const KEY& key)=0;
	virtual bool UpdatePeerStatus(const HASH& hash,const KEY& key,int status)=0;

	virtual void CheckTimeStamp()=0;

	virtual int GetBitFieldLen(const HASH& hash) = 0;
	virtual bool SetPeerBitFiled(const HASH &hash, const KEY &key) = 0;
	virtual string GetReferUrl(const HASH &hash) = 0;

	//���ٸ�����������
	virtual int GetDownTaskNum() = 0;
	virtual int GetRunTaskNum() = 0;
	virtual int GetTotalTaskNum() = 0;

	virtual int SetTaskReferUrl(const HASH &hash,const std::string &strRef) = 0;

	//����ļ���Ӧ��Ϣ,0:failed
	virtual int GetFilePieceSize(const HASH &hash) = 0;
	virtual QVOD_UINT64 GetFileSize(const HASH& hash) = 0;
	virtual QVOD_INT64	GetDownloadedSize(const HASH& hash )=0;
	virtual int GetFilePieceNum(HASH& hash) = 0;
	//�鿴ĳһ��PEERĳһƬ�Ƿ�����
	virtual bool CheckPeerHashIndex(HASH &hash, KEY &key, unsigned int index) = 0;
	virtual bool PeerNeedSendHaveMsg(HASH &hash, KEY &key, unsigned int index) = 0;

	//================�ṩ���ⲿ��ѯ�ӿ�===============================================================================
	////////////////////////////////////==��ѯ�ӿ�==//////////////////////////////////////////////////////////////////
	//����Ŀ¼��������Ϣ
	virtual int ProcessQueryTaskBypath(string path, BinaryWriter &bw) = 0;
	//������������
	virtual int ProcessQueryDownloadTasks(BinaryWriter &bw) = 0;
	//���ݹ�ϣ����ĳһ��������Ϣ
	virtual int ProcessQueryTaskByHash(HASH& hash, BinaryWriter &bw) = 0;
	//����ĳ�������peer��Ϣ
	virtual int ProcessQueryPeers(HASH& hash, BinaryWriter &bw) = 0;
	//��ѯ�ļ���Ϣ
	virtual int ProcessQueryFilesInfo(HASH& hash, BinaryWriter &bw) = 0 ;
	//����ĳ���ļ���BIT��Ϣ
	virtual int ProcessQueryBitfield(HASH& hash,QVOD_UINT64 playpos, BinaryWriter &bw) = 0;
	//��ѯƬ
	virtual int ProcessCopyFirstPiece(HASH& hash, BinaryWriter &bw) = 0;
	//��ѯ����
	virtual int PrecessQueryTorrentByHash(HASH& hash,BinaryWriter &bw) = 0;
	

	//��������״̬
	virtual bool TaskIsExist(HASH &hash) = 0;
	virtual int SetTaskStatus(HASH& hash, unsigned char status,bool bIsDeleteFile = false) = 0;
	virtual int	GetTaskStatus(HASH &hash) = 0;
	virtual bool IsCopyFullFile(const HASH& hash) = 0;
	//���ò����ļ�����
	virtual int SetPlayFileIndex(HASH& hash, int index) = 0;

	//����/��ͣ���е�����
	virtual void StartAllTask() = 0;
	virtual void PauseAllTask() = 0;

	//���� ��ȡ taks���ٶ�
	virtual void SetDownSpeed(HASH &hash,  int speed) = 0;
	virtual int GetDownSpeed(HASH &hash) = 0;

	virtual int GetTaskType(const HASH& hash)=0;
	virtual unsigned int GetPeerCanReqNum(HASH& hash,KEY& key) = 0;
	virtual bool CancelTimeoutReq(HASH& hash,KEY& key,int index,int begin,int length) = 0;
	
	//���ٲ���,add by wangxijie
	//virtual void InterDownSize(HASH& hash,KEY& key,int size)=0;
	virtual bool IsOvertakeDownSpeed(HASH& hash)=0;
	virtual void UpdateNextDownTime(HASH& hash,int lastSize)=0;
	virtual bool IsNeedCacheDownRequest(HASH& hash)=0;

	//ȡpeer�ٶ�,���ܵ�piece��ʱ��,ȫ���ٶ�>150,peer�ٶ�<3,��downnextpiece
	virtual int GetPeerDownSpeed(HASH& hash,KEY& key)=0;
	virtual int GetSomePeerDownSpeed(HASH& hash,PEERTYPE type)=0;
	
	//PEER_QUERY_BUILD_PROCESS modified by tangtao
	virtual CTask *FindTaskByPath(const std::string &strPath) = 0;
	virtual bool GetTaskPathName(HASH& hash,string& pathname)=0;
	virtual string GetTaskPath(HASH& hash) = 0;

	//��ȡBT�����ļ���TRACKER��������ַ�б� 0 -- ��Ԫ�� NUM -- Ԫ�ظ���
	// annouse --����ַ
	//annous_map -- ���õ��б�
	virtual int GetBtTrackerAnnouse(HASH &hash, std::string &annouse, std::vector<std::string> &annous_map) = 0;

	//��ѯHASH���--share
	virtual int  IsDownLoadingTask(HASH& hash)=0;
	virtual bool IsPeerAlreadyExist(HASH &hash,char random[12]) = 0;
	virtual bool GetPeerKeyByRandom(HASH &hash,char random[12],KEY& key) = 0;
	virtual bool FindTask(const HASH& hash,CTask** task) = 0;
	virtual CTask *FindTask(const unsigned char *hash) = 0;

	virtual QVOD_INT64 GetTotalAccFileSize()=0;

	virtual bool InterTaskHttpDownLen(HASH& hash,int len)=0;

	virtual bool FullPeerBitField(HASH& hash,KEY& key)=0;
	virtual void ProcessQueryReferUrl(HASH &hash,BinaryWriter &Out) = 0;
	virtual QVOD_UINT64 QueryFileLen(const HASH &hash,int index) = 0;
	virtual long GetCreateTime(const HASH &hash) = 0;
	virtual int SetDownloadPos(const HASH &hash,QVOD_UINT64 iPos,bool fromUser = false) = 0;
	virtual int QueryDownloadPos(const HASH &hash,QVOD_UINT64 &iPos) = 0;
	virtual int SetPlayFileIndexPos(const HASH &hash,QVOD_UINT64 iPos) = 0;
	virtual QVOD_INT64 GetTaskTotalDownloadLen(const HASH& hash) = 0;
	
	virtual bool IsIndexDownload(const HASH& hash, int index) = 0;
	virtual QVOD_INT64 GetDragPos(const HASH& hash) = 0;

	virtual int ProcessSetStopPlay(const HASH &hash) = 0;
	virtual QVOD_INT64 ProcessPeerQueryContinueLen(const HASH &hash,QVOD_INT64 pos) = 0;
	virtual bool IsTaskPlaying(const HASH &hash) = 0;

	virtual int GetDownloadingTasks(list<HASH>& hashList) = 0;

	virtual void GetPlayingTasks(list<HASH>& hashList) = 0;
	virtual bool IsExistEmergencyPiece(const HASH& hash) = 0;

	virtual QVOD_INT64 GetAllTaskSize() = 0;

	// ��������ͷ��Ϣ
	virtual int SetHeadValue(const HASH& hash, const char *pstHead, const char* pstrValue) = 0;
	virtual int SetHeadValue(const HASH& hash, const char *pstrHeadAdd) = 0;
	//////////////////////////////////////////////////////////////////////////
	virtual int SetPlayFileRate(const HASH &hash,unsigned int iRate) = 0;
	virtual int GetPlayFileRate(const HASH& hash) = 0;
	virtual int SetPlayFilePos(const HASH &hash,QVOD_INT64 pos) = 0;
	virtual QVOD_INT64 GetPlayFilePos(const HASH &hash) = 0;
	virtual QVOD_INT64 GetUnDownPosNearPlayPos(const HASH& hash) = 0;

	//��ȡ�����ñ���isp��
	virtual int GetPeerIspID(const HASH &hash,const KEY &key,unsigned char *pIspID) = 0;
	virtual int SetPeerIspID(const HASH &hash,const KEY &key,unsigned char *pIspID) = 0;

	virtual bool IsPlayTaskOptimizeUpload() = 0;

	//�ж��Ƿ��еȴ�״̬������,������,�򷵻�һ������hash
	virtual int GetOneWaitingTaskHash(HASH &hash) = 0;


//����QvodServer��Ӧ�ӿ�
//////////////////////////////////////////////////////////////////////////
#ifdef SUPERNODE
	virtual int ProcessQueryFolder(char* folder,UINT& num) = 0;
	virtual int ProcessSetADURL(const char *pPath,const char* pURL,const char* strHash) = 0;
	virtual int ProcessSetKeycode(char mask,char bcheckkey,string &strPath,const char* strHash) = 0;
	virtual int ProcessQueryTaskByType(TaskType type,const std::string& strPath,BinaryWriter &bw) = 0;
	//�����ϴ�����
	virtual bool SetUploadPara(HASH *pHash,UCHAR UploadLmt,UCHAR MaxPeers,UCHAR MaxSeedsRef,UCHAR bSetAssertdownload,
		UCHAR bSetAssertlink,std::string &strPath) = 0;
	//��������������ϴ�����,cacheר��
	virtual int UpdateAllTaskUploadLimit() = 0;
#endif
//////////////////////////////////////////////////////////////////////////

//�������ɰ���Ӧ�ӿ�
//////////////////////////////////////////////////////////////////////////
#ifdef NETBAR
	//virtual	void PauseByIP(UINT ip) = 0;
//	virtual void ComputeFlex(std::map<UINT ,IpInfo> &mapinfo ) = 0;
#endif
//////////////////////////////////////////////////////////////////////////

	//��ȡ��ǰ���������Ҳ��ŵ��������
	virtual int GetDownloadingAndPlayingTaskNum() = 0;

	//�Ʒ�����ģʽ����ӿ�
	virtual bool	FindHash(string strKeyurl,HASH& hash) = 0;
    virtual bool    FindHashByUrl(string strUrl, HASH& hash) = 0;
	virtual bool	FindM3u8Keyurl(string strFuzzyKeyurl,string& strKeyurl) = 0;
	virtual string	GetOrgurl(const HASH& hash) = 0;
	virtual int		CreateYfTask(const HASH& hash, const std::string strFile,int type,string& strHttpOrgUrl,string& strHttpKeyUrl,string& strHttpReferer,bool bIsLoad,TaskCfg* cfg=NULL) = 0;
	virtual int		WriteHttpData(HASH& hash,const char* buf,QVOD_UINT64 pos, int len) = 0;
	virtual void	SetFilesize(HASH& hash,QVOD_INT64 iFilesize) = 0;
	virtual int		DownNextHttpPiece(HASH& hash,YfHttpPieceReq& req) = 0;
	virtual void	UpdateDownloadMode(HASH& hash) = 0;
	virtual int		GetDownloadMode(HASH& hash) = 0;

	virtual void	ChangeTaskType(const HASH& hash,int type) = 0;
	virtual void	UpdateHttpOrgUrl(HASH& hash,string& strHttpOrgUrl) = 0;
    virtual int	UpdateClarityUrl(HASH& hash,string& strHttpOrgUrl, string& strNewHash, string& strFilePath, int nClarity) = 0;
	virtual void	DownEmergencyData(HASH& hash) = 0;

	virtual void        NotifyDownload(const HASH& hash, const int &nId) = 0;

	//���ú�ȡ������������󿽱�
	virtual int		SetCopyFullFile(HASH& hash,string& strCopyPath) = 0;
	virtual int		UnsetCopyFullFile(HASH& hash) = 0;

	//����������󿽱���״̬�仯
	virtual void	SetCopyFullFileStatus(HASH& hash,int status) = 0;
	//���������Ƿ����ڲ���
	virtual void	SetTaskPlaying(HASH& hash,bool bPlaying) = 0;
	
	//�ṩ��UI�Ĳ�ѯ�ӿ�
	virtual int QueryTaskInfo(HASH& hash,STaskInfo* pTaskInfo) = 0;
	virtual int QueryBitInfo(HASH& hash,SBitInfo* pBitInfo,int& len) = 0;
	virtual int QueryTaskInfo(HASH& hash,char* pBitInfo,int& len) = 0;

	//������Ƶʱ��
	virtual int SetVideoDuration(HASH& hash,int iDuration) = 0;	
	virtual int NotifyVideoBuffering(HASH& hash, bool bBuffering) = 0;
#ifdef MONGOTV
	virtual int GetVid(HASH& hash,char* pVid) = 0;
	//���������ǵ���������â������cdn
	virtual void SetUrlCdnType(HASH& hash,bool bIsOtherCdn) = 0;
#endif
	// ɾ�����������е�ǰ���ŵ�ǰ�����ݣ��ڳ�����ռ�
	virtual int        DeleteHasPlayingData(const HASH& hash, QVOD_UINT64 iCurCacheSize) = 0;		
	virtual void		SetAesEncrypt(HASH& hash, bool bEncrypt) = 0;			// �����������ݼ�������
	//m3u8��ؽӿ�
	virtual void        SetPlayingTimepoint(HASH& hash,int timepoint,bool bSeek,int& iCacheTime) = 0;
	virtual void		SetM3u8CdnUrl(HASH& hash,const string& strUrl) = 0;
	virtual void		SetM3u8OrgcdnUrl(HASH& hash,const string& strUrl) = 0;
	virtual bool		SaveM3u8File(HASH& hash,const char* buf,int len) = 0;
	virtual bool		M3u8IsExist(HASH& hash) = 0;
	virtual void        NotifyDownM3u8Ts(const HASH& hash, const string& strFilename) = 0;			// ֪ͨ�Ѵ������Ƭ������
	virtual int			WriteM3u8Ts(HASH& hash,const string& strFilename,const char* buf,int start,int len,int filesize) = 0;
	virtual int			ReadM3u8Ts(HASH& hash,const string& strFilename,char* buf,int start,int len,bool bIsUpload) = 0;
	virtual int			DownNextM3u8Ts(HASH& hash,SM3u8TsReq& req) = 0;
	virtual void		SetPlayingTs(HASH& hash,const string& strName) = 0;
	virtual void		SetM3u8PlayingPos(HASH& hash,const string& strName,int pos) = 0;
	virtual bool		IsPlayTsContinue(HASH& hash) = 0;
	virtual void		ClearContinuePlayTs(HASH& hash) = 0;
	virtual void		ClosePush(HASH& hash, const string& strFilename) = 0;
	virtual QVOD_INT64	GetM3u8TsSize(HASH& hash,const string& strName) = 0;
};

#endif
