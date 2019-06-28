#ifndef __MGR_TASK_H__
#define __MGR_TASK_H__

#pragma warning(disable : 4786)

#include <vector>
#include <set>
#include <map>
#include <string>
#include "Task.h"
#include "qvod_lock.h"
#include "protocol.h"
#include "TaskMgrInterface.h"
#include "terminal.h"
#include "BinaryWriter.h"
#include "common-commonstruct.h"
using namespace std;

typedef vector<CTask*> VCTTASK;


class CTaskMgr : public CTaskMgrInterFace
{
public:
	CTaskMgr();
	~CTaskMgr();
	
	//返回 0--成功 1--此任务不存在 2--失败
	virtual int DelOneTask(HASH& hash);
	
	virtual bool GetTorrentFile(HASH& hash, char*torrent, unsigned int &len);

	virtual bool GetAllTaskHashAndTradkerAddr(vector<Hash_Addr> &vec);
	virtual bool GetCacheHash(set<HASH>& hashSet);
	
	//读写数据文件
	virtual int WritePiece(const HASH& hash, KEY& key,const char* buf, QVOD_UINT64 len, unsigned int index, QVOD_UINT64 pos);
	virtual int WritePiece(const HASH& hash, const char* buf, UINT len, QVOD_UINT64 pos);
	virtual int ReadPiece(const HASH& hash, char *buf, UINT len, QVOD_UINT64 pos,bool bIsUpload = true);
#ifdef SUPERNODE
	virtual bool ReadPiece(const HASH& hash, KEY& key,char *buf, int len, unsigned int index, unsigned int start);
	virtual int DiskIoFinished(HASH& hash, KEY& key,char*& buf, int& len, unsigned int& index, unsigned int& start,int& iFlag) ;
#endif
#ifdef NETBAR
	virtual bool IsNeedDownload(HASH& hash);//内存版判断任务是否需要有足够空间可下载
#endif
	virtual int  DownNextPiece(const HASH& hash,KEY& key,request& req);
	virtual bool DownNextPiece(const HASH& hash,KEY& key,httpiecereq& hreq);

	virtual int DeleteTaskByFolder(std::string strfolder);

	virtual bool GetTaskBitField(const HASH& hash, unsigned char*bitFiled, unsigned int &len); 
	virtual bool TaskIsFinish(const HASH& hash, bool bCheckFileExist=false);
	virtual int AddOnePeer(const HASH& hash, const KEY& key, unsigned int PublicIP,unsigned short PublicPort,
	bool isDownPeer,unsigned char nattype,unsigned char nettype,char PeerID[20],PEERTYPE peertype = NORMAL);

	// 增加CacheServer,CloudServer,MinerServer等服务器 [2013/12/31 tangtao]
	virtual int AddOneQvodServer(const HASH &hash,KEY& key,unsigned char natType,unsigned char netType,char szPeerID[20],
		QVOD_UINT64 iBeginPos,unsigned int iDataLen,PEERTYPE peerType = NORMAL);
	virtual int DelOnePeer(const HASH& hash, KEY& key);
	virtual int CreatePeerBitField(const HASH& hash, KEY& key, unsigned char* bit, int len);
	virtual int SetPeerBitField(const HASH& hash, KEY& key, unsigned int index);
	virtual int ResetPeerBitField(const HASH &hash,KEY &key,unsigned int index);

	virtual bool SetPeerCacheServer(const HASH& hash, const KEY& key);
	virtual bool IsCacheServer(const HASH& hash, const KEY& key);
	virtual bool IsMinerServer(const HASH& hash, const KEY& key);
	virtual int SetPeerID(const HASH& hash, const KEY& key, char pid[20]);
	virtual bool SetPeerType(const HASH& hash, const KEY& key,USERTYPE type);
	virtual int GetPeerID(const HASH& hash, const KEY& key, char pid[20]);
	virtual int GetPeerType(const HASH& hash, const KEY& key);
	virtual unsigned char GetPeerNatType(const HASH& hash, const KEY& key);
	virtual unsigned char GetPeerNetType(const HASH& hash, const KEY& key);
	virtual unsigned char GetPeerTimeoutCount(const HASH& hash, const KEY& key);
	virtual int GetPeerVersion(const HASH& hash, const KEY& key);
	virtual bool IsDownPeer(const HASH& hash, const KEY& key);
	virtual int GetPeerNum(const HASH & hash,PEERTYPE type = ALLTYPE);
	virtual bool IsConnect(const HASH & hash, unsigned int ip, short port);
	virtual bool IsSuperPeerNode(const HASH& hash, const KEY& key);
	virtual int  QueryPeerStatus(const HASH& hash,const KEY& key);
	virtual bool UpdatePeerStatus(const HASH& hash,const KEY& key,int status);

	virtual void CheckTimeStamp();
	virtual int GetTaskType(const HASH& hash);
	virtual string GetReferUrl(const HASH &hash);
	virtual int GetDownTaskNum();
	virtual int GetRunTaskNum();
	virtual int GetTotalTaskNum();

	virtual int GetBitFieldLen(const HASH& hash);
	virtual bool SetPeerBitFiled(const HASH &hash, const KEY &key);

	virtual bool CheckPeerHashIndex(HASH &hash, KEY &key, unsigned int index);
	virtual bool PeerNeedSendHaveMsg(HASH &hash, KEY &key, unsigned int index);
	////////////////////////////////////==查询接口==//////////////////////////////////////////////////////////////////
	//根据目录查任务信息
	virtual int ProcessQueryTaskBypath(string path, BinaryWriter &bw);
	//查找下载任务
	virtual int ProcessQueryDownloadTasks(BinaryWriter &bw);
	//根据哈希查找某一个任务信息
	virtual int ProcessQueryTaskByHash(HASH& hash, BinaryWriter &bw);
	//查找某个任务的peer信息
	virtual int ProcessQueryPeers(HASH& hash, BinaryWriter &bw);
	//查询文件信息
	virtual int ProcessQueryFilesInfo(HASH& hash, BinaryWriter &bw);
	//查找某个文件的BIT信息
	virtual int ProcessQueryBitfield(HASH& hash,QVOD_UINT64 playpos, BinaryWriter &bw);
	//查询片
	virtual int ProcessCopyFirstPiece(HASH& hash, BinaryWriter &bw);
	//查询种子
	virtual int PrecessQueryTorrentByHash(HASH& hash,BinaryWriter &bw);
	virtual void ProcessQueryReferUrl(HASH &hash,BinaryWriter &Out) ;

	virtual void SerializeTaskInfo(CTask **pTask, BinaryWriter &bw);
	virtual void SerializeTaskInfoEx(CTask **pTask, BinaryWriter &bw);

	virtual int GetBtTrackerAnnouse(HASH &hash, std::string &annouse, std::vector<std::string> &annous_map);

	virtual int SetPlayFileIndex(HASH& hash, int index);

	//设置任务状态
	virtual int SetTaskStatus(HASH& hash, unsigned char status,bool bIsDeleteFile);
	virtual int	GetTaskStatus(HASH& hash);
	virtual bool TaskIsExist(HASH &hash);
	virtual bool IsCopyFullFile(const HASH& hash);
	virtual QVOD_INT64 GetTotalAccFileSize();

	virtual int SetHeadValue(const HASH& hash, const char *pstHead, const char* pstrValue);
	virtual int SetHeadValue(const HASH& hash, const char *pstrHeadAdd);
	//启动、暂停所有的任务
	virtual void StartAllTask();
	virtual void PauseAllTask();

	virtual void SetDownSpeed(HASH &hash,  int speed);
	virtual int GetDownSpeed(HASH &hash);

	virtual unsigned int GetPeerCanReqNum(HASH& hash,KEY& key);
	virtual bool CancelTimeoutReq(HASH& hash,KEY& key,int index,int begin,int length);

	//限速、测速部分,add by wangxijie
	virtual void InterDownSize(const SPEEDATA& sdata);
	virtual void InterUpSize(const SPEEDATA& sdata);

	virtual bool IsOvertakeDownSpeed(HASH& hash);
	virtual void UpdateNextDownTime(HASH& hash,int lastSize);
	virtual bool IsNeedCacheDownRequest(HASH& hash);
	virtual int GetPeerDownSpeed(HASH& hash,KEY& key);
	virtual int GetSomePeerDownSpeed(HASH& hash,PEERTYPE type);

	virtual int  IsDownLoadingTask(HASH& hash);
	virtual bool IsPeerAlreadyExist(HASH &hash,char random[12]);
	virtual bool GetPeerKeyByRandom(HASH &hash,char random[12],KEY& key);

	//PEER_QUERY_BUILD_PROCESS modified by tangtao
	virtual CTask *FindTaskByPath(const std::string &strPath);
	virtual bool GetTaskPathName(HASH& hash,string& pathname);
	virtual string GetTaskPath(HASH& hash);

	virtual bool InterTaskHttpDownLen(HASH& hash,int len);
	virtual bool FullPeerBitField(HASH& hash,KEY& key);
	virtual int SetTaskReferUrl(const HASH &hash,const std::string &strRef);

	virtual int			GetFilePieceSize(const HASH &hash);
	virtual QVOD_UINT64		GetFileSize(const HASH& hash);
	virtual QVOD_INT64	GetDownloadedSize(const HASH& hash );
	virtual int			GetFilePieceNum(HASH& hash);

	QVOD_UINT64 QueryFileLen(const HASH &hash,int index);
	long GetCreateTime(const HASH &hash);
	int SetDownloadPos(const HASH &hash,QVOD_UINT64 iPos,bool fromUser);
	int QueryDownloadPos(const HASH &hash,QVOD_UINT64 &iPos);
	int SetPlayFileIndexPos(const HASH &hash,QVOD_UINT64 iPos);

	virtual bool IsIndexDownload(const HASH& hash, int index);
	virtual QVOD_INT64 GetDragPos(const HASH& hash);

	//暂停的点播下载的长度
	virtual QVOD_INT64 GetTaskTotalDownloadLen(const HASH& hash);
	virtual int ProcessSetStopPlay(const HASH &hash);
	virtual bool IsTaskPlaying(const HASH &hash);
	virtual QVOD_INT64 ProcessPeerQueryContinueLen(const HASH &hash,QVOD_INT64 pos);
	
	virtual int GetDownloadingTasks(list<HASH>& hashList);

	virtual void GetPlayingTasks(list<HASH>& hashList);
	virtual bool IsExistEmergencyPiece(const HASH& hash);
	virtual QVOD_INT64 GetAllTaskSize();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	virtual int SetPlayFileRate(const HASH &hash,unsigned int iRate);
	virtual int GetPlayFileRate(const HASH& hash);
	virtual int SetPlayFilePos(const HASH &hash,QVOD_INT64 pos);
	virtual QVOD_INT64 GetPlayFilePos(const HASH &hash);
	virtual QVOD_INT64 GetUnDownPosNearPlayPos(const HASH& hash);

	virtual int GetPeerIspID(const HASH &hash,const KEY &key,unsigned char *pIspID);
	virtual int SetPeerIspID(const HASH &hash,const KEY &key,unsigned char *pIspID);

	virtual bool IsPlayTaskOptimizeUpload();

	//获取CacheServer的下载速度
	virtual int GetTaskCacheServerDownloadSpeed(HASH &hash,map<KEY,int> &mapCacheSpeed);
	virtual int GetTaskCacheServerDownloadSpeed(HASH &hash);

	virtual int GetOneWaitingTaskHash(HASH &hash);

	//////////////////////////////////////////////////////////////////////////
#ifdef SUPERNODE
	int ProcessQueryFolder(char* folder,UINT& num) ;
	int ProcessSetADURL(const char *pPath,const char* pURL,const char* strHash);
	int ProcessSetKeycode(char mask,char bcheckkey,string &strPath,const char* strHash);
	int ProcessQueryTaskByType(TaskType type,const std::string& strPath,BinaryWriter &bw);
	bool SetUploadPara(HASH *pHash,UCHAR UploadLmt,UCHAR MaxPeers,UCHAR MaxSeedsRef,UCHAR bSetAssertdownload,
		UCHAR bSetAssertlink,std::string &strPath);
	int UpdateAllTaskUploadLimit();
#endif
	//////////////////////////////////////////////////////////////////////////

//增加网吧版相应接口
//////////////////////////////////////////////////////////////////////////
#ifdef NETBAR
	//void PauseByIP(UINT ip);
//	void ComputeFlex(std::map<UINT ,IpInfo> &mapinfo );
#endif

	virtual int		GetDownloadingAndPlayingTaskNum();

	//云帆下载模式任务接口
	virtual bool	FindHash(string strKeyurl,HASH& hash);
    virtual bool    FindHashByUrl(string strUrl, HASH& hash);
	virtual bool	FindM3u8Keyurl(string strFuzzyKeyurl,string& strKeyurl);
	virtual string	GetOrgurl(const HASH& hash);
	virtual int		CreateYfTask(const HASH& hash, const std::string strFile,int type,string& strHttpOrgUrl,string& strHttpKeyUrl,string& strHttpReferer,bool bIsLoad,TaskCfg* cfg=NULL);
	virtual int		WriteHttpData(HASH& hash,const char* buf, QVOD_UINT64 pos, int len);
	virtual void	SetFilesize(HASH& hash,QVOD_INT64 iFilesize);
	virtual int		DownNextHttpPiece(HASH& hash,YfHttpPieceReq& req);
	virtual void	UpdateDownloadMode(HASH& hash);
	virtual int		GetDownloadMode(HASH& hash);

	virtual void	ChangeTaskType(const HASH& hash,int type);
	virtual void	UpdateHttpOrgUrl(HASH& hash,string& strHttpOrgUrl);
    virtual int		UpdateClarityUrl(HASH& hash,string& strHttpOrgUrl, string& strNewHash, string& strFilePath,  int nClarity);
	virtual void	DownEmergencyData(HASH& hash);
	virtual void    NotifyDownload(const HASH& hash, const int &nId);

	virtual int		SetCopyFullFile(HASH& hash,string& strCopyPath);
	virtual int		UnsetCopyFullFile(HASH& hash);
	virtual void	SetCopyFullFileStatus(HASH& hash,int status);
	virtual void	SetTaskPlaying(HASH& hash,bool bPlaying);
	virtual int		QueryTaskInfo(HASH& hash,STaskInfo* pTaskInfo);
	virtual int		QueryTaskInfo(HASH& hash,char* pBitInfo,int& len);
	virtual int		QueryBitInfo(HASH& hash,SBitInfo* pBitInfo,int& len);
	virtual int		SetVideoDuration(HASH& hash,int iDuration);
	virtual int		NotifyVideoBuffering(HASH& hash, bool bBuffering);
#ifdef MONGOTV
	virtual int GetVid(HASH& hash,char* pVid);
	virtual void SetUrlCdnType(HASH& hash,bool bIsOtherCdn);
#endif
	virtual int        DeleteHasPlayingData(const HASH& hash, QVOD_UINT64 iCurCacheSize);		
	virtual void	SetAesEncrypt(HASH& hash, bool bEncrypt);

	//m3u8相关接口
	virtual void        SetPlayingTimepoint(HASH& hash,int timepoint,bool bSeek,int& iCacheTime);
	virtual void		SetM3u8CdnUrl(HASH& hash,const string& strUrl);
	virtual void		SetM3u8OrgcdnUrl(HASH& hash,const string& strUrl);
	virtual bool		SaveM3u8File(HASH& hash,const char* buf,int len);
	virtual bool		M3u8IsExist(HASH& hash);
	virtual void        NotifyDownM3u8Ts(const HASH& hash, const string& strFilename);
	virtual int			WriteM3u8Ts(HASH& hash,const string& strFilename,const char* buf,int start,int len,int filesize);
	virtual int			ReadM3u8Ts(HASH& hash,const string& strFilename,char* buf,int start,int len,bool bIsUpload);
	virtual int			DownNextM3u8Ts(HASH& hash,SM3u8TsReq& req);
	virtual void		SetPlayingTs(HASH& hash,const string& strName);
	virtual void		SetM3u8PlayingPos(HASH& hash,const string& strName,int pos);
	virtual bool		IsPlayTsContinue(HASH& hash);
	virtual void		ClearContinuePlayTs(HASH& hash);
	virtual QVOD_INT64	GetM3u8TsSize(HASH& hash,const string& strName);
	virtual void		ClosePush(HASH& hash, const string& strFilename);
private:
	CTask * FindTask(const unsigned char *hash);
    bool FindTask(const HASH& hash,CTask** task);
	bool DelTask(HASH& hash);
private:
	map<string, HASH>   m_keyurl_hash_map;		//http视频标识url对应p2p hash表
	CLock				m_keyurl_hash_map_lock;

	map<HASH, CTask *>	m_task_map;				//任务列表--根据哈希
	CLock				m_task_map_lock;			

	map<string, VCTTASK> m_task_path;			//任务列表--根据目录
	CLock				 m_task_path_lock;
	
	set<HASH>			 m_accTaskSet;				//保存QvodAcc任务的hash
	CLock				 m_accTaskSetLock;
};

#endif
