#ifndef MEDIA_H_
#define MEDIA_H_

using namespace std;


extern long g_nFileID;
extern long g_nImgID;
extern long g_nVideo;
extern Gdiplus::Bitmap* g_pMusicImg;
extern Gdiplus::Bitmap* g_pFilmImg;
extern TCHAR g_szPrivateUserName[256];
typedef LONGLONG REFERENCE_TIME;

#define QVODPLAYER			_T("Qvod Player")
#define MC_FILM				-2
#define MC_MUSIC			-3
#define MC_SERCH			-4

#define IMGCOUNT			5
#define SORTCOUNT           3
#define SHOWIMG				0
#define MINIIMG_W			96
#define MINIIMG_H			72

#define M_FILESIZE		0x00000001
#define M_DURATION		0x00000002
#define	M_PLAYTIME		0x00000004
#define M_ADDTIME		0x00000008
#define M_DOWNSIZE		0x00000010
#define M_VIDEOSIZE		0x00000020
#define M_RATE			0x00000040
#define M_TYPE			0x00000080
#define M_ID			0x00000100
#define M_TITLE			0x00000200
#define	M_AUTHOR		0x00000400
#define M_ALBUM			0x00000800
#define M_FILEPATH		0x00001000
#define M_IMGPATH		0x00002000
#define M_IMG			0x00004000
#define M_IMGID			0x00008000
#define M_NAME			0x00010000
#define M_ERROR			0x00020000
#define M_BEGINTIME		0x00040000
#define M_ENDTIME		0x00080000
#define M_PRIVATE		0x00100000
#define M_PLAYEND		0x00200000
#define M_ALL			0xffffffff
#define M_MAININFO		M_FILESIZE|M_DOWNSIZE|M_DURATION|M_RATE|M_ADDTIME|M_VIDEOSIZE|M_TYPE|M_NAME|M_TITLE|M_AUTHOR|M_ALBUM|M_ERROR

#define M_CRASH_IMAGE
#define M_CRASH_INFO

enum SERACH_MSG{
QCM_PL_FIND_FIRSTMEDIA,
QCM_PL_FIND_FIRSTMEDIA_EXSIT,
QCM_PL_FIND_MEDIA,
QCM_PL_FIND_END,
QCM_MC_FIND_MEDIA,
QCM_MC_FIND_FOLDER,
QCM_MC_FIND_END,
QCM_NEW_IMAGE};

enum SORTTYPE{
	sort_Name = 1, 
	sort_Format,
	sort_Author, 
	sort_Album,
	sort_Duration, 
	sort_Rate,
	sort_FileSize,
	sort_Type,
	sort_TypeEx,
	sort_FilePath,
	sort_StartPos, 
	sort_AddTime, 
	sort_Width, 
	sort_Height,
	sort_Progress,
	sort_Downrate,
	sort_Unknow};


#define ME_NOPLAY		0x00000001
#define ME_NOFILE		0x00000002
#define	ME_ADDERROR		0x00000004
#define ME_IMAGECRASH	0x00000008
#define ME_INFOCRASH	0x00000010


typedef struct s_TitleInfo
{
	CString strPath;
	CString strKey;
	int		nNum;
	s_TitleInfo()
	{
		nNum = -1;
	}
}TitleInfo;

typedef struct s_ListInfo
{
	long nOrder;						// 循环方式 0:单曲 1:单曲循环 2:顺序播放 3:循环播放 4:随机播放
	long nSkipStart;					// 跳过开始时间  默认 0
	long nSkipEnd;						// 跳过结束时间  默认 0
	long nTimeShow;						// 时间显示方式  0 - 分秒  1 - 时分秒 默认 0
	long nTitleShow;					// 标题显示方式
	long nID;
	BOOL bMCenterShowMode;
	BOOL bPlayListShowMode;
	int Width[7];
	CString strTitle;					// 列表名
	s_ListInfo()
	{
		nOrder = 0;
		nSkipStart = 0;
		nSkipEnd = 0;
		nTimeShow = 0;
		nID = 0;
#ifdef RDSP
		bMCenterShowMode = 1;
		bPlayListShowMode = 1;
#else
		bMCenterShowMode = 0;
		bPlayListShowMode = 0;
#endif
		nTitleShow = 4;
		memset(&Width, 0, 7 * sizeof(int));
	}
}ListInfo;


typedef struct s_MediaClassInfo
{
	CString strClass;					//媒体类型名
	SORTTYPE sortType;
	BOOL bAcc;
	s_MediaClassInfo()
	{
		sortType = (SORTTYPE)0;
		bAcc = 0;
	}
}MediaClassInfo;

class Media;
struct s_MediaNode
{
	Media* pMedia;
	s_MediaNode* pNext;
	s_MediaNode* pChild;
};
typedef s_MediaNode MediaNode;

/*枚举虚拟显示的分类*/
enum  _MEDIAFILE_TYPE
{
	type_all,
	type_video,
	type_audio,
	type_image,
	type_other,

	type_web,
	type_audio_cut,
	type_exit_play
};

/* 枚举虚拟显示下的节点分类*/
enum _MEDIA_CHILD_TYPE
{
	author = 1,
	album,
	title,
	addtime,
	all,
	usertypeall,
	classall//show all _MEDIATYPE
};

typedef struct DEAL_MEDIA_INFO_
{
	int nID;
	int nClass;//0-网络，1-电影，2-音乐 4-播放列表
	BOOL bFirst;//截第一张/后n张
	DEAL_MEDIA_INFO_()
	{
		nID = 0;
		nClass = 0;
		bFirst = TRUE;
	}
}DEAL_MEDIA_INFO;

typedef struct CRASH_INFO_
{
	int nOperate;
	CString strPath;
	CRASH_INFO_()
	{
		nOperate = 0;
	}
}CRASH_INFO;

class IMediaCenter;
class IPlayList;

class MediaHelper
{
public:

	~MediaHelper();
	static MediaHelper* GetMediaHelper();

public:
	CString ShortName(CString& fullpath);
	void GetMediaName(Media* pMedia, int nType = 4);
	void FInfo2Media(s_FileInfo* pF, Media* pM);
	void FilterChars(CString& str);
	int CheckMedia(CString& strTitle);
	void SortMedia(vector<Media*>& vMedia, SORTTYPE nType, BOOL bOrder = FALSE);
	BOOL FindFileExt(LPCTSTR pExt, LPCTSTR pExtList);
	BOOL FindFileExt1(LPCTSTR pPath, LPCTSTR pExtList);
	void StartSearchThread();
	void StartImageThread();
	void StartInfoThread();
	void PushDealMedia(int nClass, Media* pMedia, BOOL bHead = FALSE);
	void PushDealMedia(int nClass, int nID, BOOL bFirst = TRUE,BOOL bHead = FALSE);
	void PushDealMediaInfo(int nClass, Media* pMedia, BOOL bHead = FALSE);
	
	BOOL StopCurCut(int nID);
	void AddMedia(Media* pMedia);
	void RemoveMedia(Media* pMedia);
	Media* FindMedia(CString& strPath, int nBeginTime = 0, int nEndTime = 0);
	BOOL IsLocalMedia(int nID);
	int ListAddToList(int nType, int nID, int nDestList);
	int MCenterAddToList(int nType, int nID, int nDestList);
	BOOL SaveToDisk(Gdiplus::Image* pBmp, CString& sPath, UINT nType = 0);
	BOOL SaveToDisk(Gdiplus::Image* pBmp, int nImgID);
	BOOL IsSamePath(CString& str1, CString& str2);
	BOOL IsIgnoreFmt(LPCTSTR lpPath);
	void ExitImageThread();
	void ExitInfoThread();
	Gdiplus::Bitmap* GetMiniBmp(LPCTSTR sPath);
	BOOL GetMiniBmp(int nID, int nClass, int nCount = IMGCOUNT);
	BOOL GetMiniBmp(CString strPath ,CString strSaveFile);
	BOOL GetFileInfo(int nID, int nClass);
	__int64 GetFileSize(CString& sPath);
	BOOL SetMediaInfo(Media* pMedia, int nClass, int nIndex, LONG nMask = -1);
	BOOL GetMediaInfo(Media* pMedia, int nClass, int nIndex);
	BOOL SetMediaInfoEx(Media* pMedia, int nClass, int nID, LONG nMask = -1);
	BOOL GetMediaInfoEx(Media* pMedia, int nClass, int nID);
	BOOL GetMediaInfoEx(Media* pMedia, int nClass, int nID, int& nIndex);
	BOOL DeleteMediaEx(int nClass, int nID);
	BOOL SaveData(int nClass, LPCTSTR lpUserName = NULL);
	void XMLToMedia(CXMLElement& xmlChild, Media* pm);
	void MediaToXML(Media* pm, CXMLElement& xmlChild);
	//是否是相似标题
	BOOL IsSimilarTitle(TitleInfo* t1, TitleInfo* t2);	
	//查找相似文件
	BOOL FindSimilarMedia(vector<CString>& vPath, int* pIndex = NULL);
	//获取标题信息
	void GetTitleInfo(LPCTSTR lpPath, TitleInfo* pInfo);
	static bool CmpTitle(TitleInfo* t1, TitleInfo* t2);
	CString GetLnkTarget(LPCTSTR lpLnkName);

	//截图相关
	void FreeImg(Media* pm);
	BOOL LoadImg(Media* pm, int nImage = SHOWIMG);
	static BOOL TransToMini(Gdiplus::Image* pSrc, Gdiplus::Image* pDest, UINT uWidth = MINIIMG_W, UINT uHeight = MINIIMG_H);
	//克隆一个新的Bitmap对象，不用时，需使用delete释放。
	static Gdiplus::Bitmap *Clone(Gdiplus::Bitmap *pBitmap); 
	REFERENCE_TIME ReadyToCut(CString& strPath);
	void OverCut();
	Gdiplus::Bitmap* CutImage(REFERENCE_TIME nTime);
	REFERENCE_TIME GetKeyPos(REFERENCE_TIME nTime);

	BOOL FilterXmlErrorChar(LPCTSTR lpXmlPath);
	void RecordCrash(CString& strPath, int nOperate);
	void ClearCrash(CString& strPath, int nOperate);
	BOOL ExistCrash(CString& strPath, int dwOperate);	
	BOOL ExistHaveAnyCrash(CString& strPath);			//是否已经崩溃过，在截图或信息接口中
	BOOL GetLastCrashInfo(CRASH_INFO* pInfo);
	IQvodPlayMedia* GetIPlayMedia();
	void Clear();
	void ExitThread();
	static BOOL m_bImageThread;
	CString m_sImgPath;
	CString m_strLogPath;
	CXMLDoc m_CrashXmlDoc;
	CRASH_INFO* m_pCrashInfo;

	static IMediaCenter* m_pCenter;
	static IPlayList* m_pList;

	CString GetPathByName(const CString& strFileName);
private:
	static DWORD WINAPI SearchProc(void* pPram);
	static DWORD WINAPI ImageProc(void* pPram);
	static DWORD WINAPI InfoProc(void* pPram);
	static bool CmpSize(Media* s1, Media* s2);
	static bool CmpDur(Media* s1, Media* s2);
	static bool CmpPos(Media* s1, Media* s2);
	static bool CmpAddTime(Media* s1, Media* s2);
	static bool CmpRate(Media* s1, Media* s2);
	static bool CmpType(Media* s1, Media* s2);
	static bool CmpName(Media* s1, Media* s2);
	static bool CmpAuthor(Media* s1, Media* s2);
	static bool CmpAlbum(Media* s1, Media* s2);
	static bool CmpPath(Media* s1, Media* s2);
	static bool CmpFormat(Media* s1, Media* s2);
	static bool CmpTypeEx(Media* s1, Media* s2);
	static bool CmpSize1(Media* s1, Media* s2);
	static bool CmpDur1(Media* s1, Media* s2);
	static bool CmpPos1(Media* s1, Media* s2);
	static bool CmpAddTime1(Media* s1, Media* s2);
	static bool CmpRate1(Media* s1, Media* s2);
	static bool CmpType1(Media* s1, Media* s2);
	static bool CmpName1(Media* s1, Media* s2);
	static bool CmpAuthor1(Media* s1, Media* s2);
	static bool CmpAlbum1(Media* s1, Media* s2);
	static bool CmpPath1(Media* s1, Media* s2);
	static bool CmpFormat1(Media* s1, Media* s2);
	static bool CmpTypeEx1(Media* s1, Media* s2);
private:
	MediaHelper();
	DWORD OnImageProc();
	DWORD OnSearchProc();
	DWORD OnInfoProc();
	static MediaHelper* m_pHelper;
	static BOOL m_bSearchThread;
	
	BOOL m_bExitTimeOut;
	BOOL m_bImageExit;
	BOOL m_bSearchExit;
	BOOL m_bInfoExit;
	HANDLE m_hSearchThread;
	HANDLE m_hImageThread;
	HANDLE m_hInfoThread;
	CRITICAL_SECTION m_Lock;
	CRITICAL_SECTION m_InfoLock;

	CRITICAL_SECTION m_CrashLock;
	vector<Media*> m_vMedia;
	vector<DEAL_MEDIA_INFO> m_vDealMedia;
	vector<DEAL_MEDIA_INFO> m_vDealMediaInfo;
};


class Media
{
public:
	Media()
	{
		memset(this, 0, (int)&strPath - (int)this);
		lRate = -1;
		m_nRef = 1;	
	}
private:
	int m_nRef;
public:
	__int64		nFileSize;					//媒体文件大小
	__int64     nDownSize;					//已下载大小
	SIZE		videoSize;					//图像大小
	time_t		tmCreateTime;				//添加媒体的时间	
	long		lPlayTime;					//上次停止位置(ms)
	long		lDuration;					//播放时长(s)
	long		lBeginTime;					//开始时间
	long		lEndTime;					//结束时间
	long		lRate;						//码率	
	BOOL		bPlayEnd;					//是否播放完成
	BOOL		bPrivate;					//是否为私人文件
	int			nType;						//文件类型(0-文件夹，1-电影，2-音乐，3-图片，4-未知）
	int			nID;						//ID号(唯一标识)
	int			nError;						//错误信息
	int			nImgID[IMGCOUNT];			//缩略图ID数组
	Gdiplus::Image*		pImage[IMGCOUNT];	//缩略图指针数组

	CString		strPath;					//文件路径(唯一标识)
	CString     strName;					//文件名
	CString		strTitle;					//标题
	CString		strAuthor;					//艺术家
	CString		strAlbum;					//专辑	

public:
	void Copy(Media* pDest);
	void Copy(Media* pDest, int nMask);
	void AddRef();
	void Release();
	void Release1();//会删除硬盘缩略图文件
};


class IPlayList : public IQvodCallBack
{
public:
	~IPlayList();
	static IPlayList* GetInstance();
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);
	friend class MediaHelper;
public:
//Attributes

	/*************注：以下带Ex的函数均是以ID号访问元素，否则以索引！*********************/

	//Description:    设置媒体信息
	//Input:          pMedia 要设置的媒体信息 nList	列表索引  nItem列表项索引	
	//Output:         
	//Return:		  BOOL
	BOOL SetMediaInfo(Media* pMedia, UINT nList, UINT nItem);
	BOOL SetMediaInfoEx(Media* pMedia, UINT nList, UINT nID);

	//Description:    获取媒体信息
	//Input:          nList	列表索引  nItem列表项索引
	//Output:         pMedia 媒体信息
	//Return:		  BOOL
	BOOL GetMediaInfo(Media* pMedia, UINT nList, UINT nItem)const;
	BOOL GetMediaInfoEx(Media* pMedia, UINT nList, UINT nID);
	BOOL GetMediaInfoEx(Media* pMedia, UINT nList, UINT nID, int& nPos);

	//Description:    获取媒体信息
	//Input:          nList	列表索引  nItem列表项索引
	//Output:         pMedia 媒体信息
	//Return:		  BOOL
	BOOL SetMediaInfo(Media* pMedia, UINT nList, UINT nItem, LONG nMask);
	BOOL SetMediaInfoEx(Media* pMedia, UINT nList, UINT nID, LONG nMask);
	//Description:    设置媒体路径
	//Input:          strPath 媒体路径 nList 列表索引  nItem列表项索引	
	//Output:         
	//Return:		  BOOL
	BOOL SetMediaPath(CString& strPath, UINT nList, UINT nItem);
	BOOL SetMediaPathEx(CString& strPath, UINT nList, UINT nID);

	//Description:    获取媒体路径
	//Input:          nList	列表索引  nItem列表项索引	
	//Output:         strPath 媒体路径
	//Return:		  BOOL
	BOOL GetMediaPath(CString& strPath, UINT nList, UINT nItem)const;
	BOOL GetMediaPathEx(CString& strPath, UINT nList, UINT nID);

	//Description:    获取某个列表中媒体总数
	//Input:          nList 列表索引
	//Output:         
	//Return:		  媒体总数
	int GetMediaCount(UINT nList)const;

	//Description:    获取上\下一个媒体文件位置
	//Input:          nItem 当前播放项的索引（没有播放用-1表示） nCount 项总数 nMode: 播放模式nType 0-上一个，1-下一个，2-自动播放
	//Output:         
	//Return:		  上\下一个媒体的位置
	LONG GetNextMediaIndex(LONG nItem, UINT nCount, UINT nMode, UINT nType);

	//Description:    获取媒体在列表中的位置
	//Input:          strPath媒体路径  nList 列表索引
	//Output:         
	//Return:		  文件位置
	UINT GetMediaPos(CString& strPath, UINT nList)const;

	//Description:    获取列表的总数
	//Input:          
	//Output:         
	//Return:		  列表总数
	UINT GetListCount()const;

	//Description:    设置列表信息
	//Input:          pListInfo 列表信息 nList列表索引
	//Output:         
	//Return:		  BOOL
	BOOL SetListInfo(ListInfo* pListInfo, UINT nList);

	//Description:    获取列表信息
	//Input:          nList列表索引
	//Output:         pListInfo 列表信息
	//Return:		  BOOL
	BOOL GetListInfo(ListInfo* pListInfo, UINT nList)const;

	BOOL SetListMode(UINT nMode, UINT nList);

	int GetListMode(UINT nList);

	int GetTimeMode(UINT nList);

	int GetIndexFromID(UINT nList, int nID, int nPos = 0);

	int GetIDFromIndex(UINT nList, int nItem);

	int GetSearchingList();

	void SetEventParent(HWND hWnd);

	void SetEventObj(IQvodCallBack* pObj);

	void DetectListError();

	void DetectListError(Media* pm);

	void SetInsertPos(int nPos);

	void SetLastPlayID(int nID);
public:
	//Operations

	//Description:    初始化工作
	//Input:          
	//Output:         
	//Return:		  BOOL
	BOOL Init_();

	//Description:    保存工作
	//Input:          
	//Output:         
	//Return:		  BOOL
	void Save_(LPCTSTR lpUserName = NULL);

	//Description:    加载私人数据
	//Input:          lpUserName:用户名
	//Output:         pMergeCount:合并条目数
	//Return:		  BOOL
	BOOL LoadPrivateData(LPCTSTR lpUserName, int* pMergeCount = NULL);

	//Description:    移除私人数据
	//Input:          
	//Output:         
	//Return:		  BOOL
	void RemovePrivateData();

	//Description:    添加媒体到某个列表中
	//Input:          strPath 媒体路径 nList 列表索引  bDetect 是否检测文件类型
	//Output:         
	//Return:		  媒体id号，若失败返回0
	int AddMedia(CString& strPath, UINT nList, BOOL bDetect = TRUE, __int64 nSize = 0);

	//Description:    添加媒体到某个列表中
	//Input:          pMedia 媒体结构指针 nList 列表索引  
	//Output:         
	//Return:		  BOOL
//	BOOL AddMedia(Media* pMedia, UINT nList);

	BOOL AddMediaDirect(Media* pMedia, UINT nList);
	//Description:    添加文件或文件夹到某个列表中
	//Input:          strPath 文件或文件夹的路径  nList 列表索引  bDetect 是否检测文件类型
	//Output:         
	//Return:		  BOOL
	BOOL AddFile(CString& strPath, UINT nList, BOOL bDetect = TRUE, BOOL bAddSimilar = FALSE);
	BOOL AddFile(vector<CString>& vPath, UINT nList, BOOL bDetect = TRUE, BOOL bAddSimilar = FALSE);

	//Description:    删除某个列表中的媒体
	//Input:          nList 列表索引  nItem 列表项索引
	//Output:         
	//Return:		  BOOL
	BOOL DeleteMedia(UINT nList, UINT nItem);
	BOOL DeleteMediaEx(UINT nList, UINT nID);

	//Description:    删除错误的媒体
	//Input:         
	//Output:         
	//Return:		  BOOL
	BOOL DeleteErrorMedia();

	//Description:    移动列表中的媒体
	//Input:         vMedia 要移动的媒体索引数组  nPos 移动到哪个位置  nList 列表索引
	//Output:         
	//Return:		  BOOL
	BOOL MoveMedia(vector<UINT>& vMedia, int nPos, UINT nList);

	//Description:    创建一个新列表
	//Input:         
	//Output:         
	//Return:		  BOOL
	BOOL CreateList(CString& sListName, int nList = -1);

	//Description:    添加一个列表
	//Input:          listPath 列表的路径 nList添加到哪个列表之下，-1表示新增一个列表
	//Output:         
	//Return:		  BOOL
	int AddList(CString& listPath, int nList);

	//Description:    清空一个列表
	//Input:          nList 列表索引
	//Output:         
	//Return:		  BOOL
	BOOL ClearList(UINT nList,  BOOL bReservePrivate = TRUE);

	//Description:    对列表中的媒体进行排序
	//Input:          nList 列表索引  nType 排序方式  bOrder 顺/逆序  p
	//Output:         pHot  排序后媒体位置
	//Return:		  BOOL
	BOOL SortList(UINT nList, SORTTYPE nType, BOOL bOrder = TRUE);

	//Description:    删除一个列表
	//Input:          nList 列表索引
	//Output:         
	//Return:		  BOOL
	BOOL DeleteList(UINT nList, BOOL bReservePrivate = TRUE);

	//Description:    保存列表
	//Input:          nList 列表索引 lpszPath 要保存的路径名,若为空则保存到playlist目录下,若为用户名则保存到（playlist_用户名）下  
	//Output:         
	//Return:		  BOOL
	BOOL SaveList(UINT nList, LPCTSTR lpPath = NULL);

	//Description:    根据媒体路径查找某一个媒体的信息
	//Input:          strpath 要查找的媒体路径 nList 列表索引 
	//Output:         
	//Return:		   媒体ID,未找到返回-1
	int FindMedia(CString& strPath, int nList,  int nBeginTime = 0, int nEndTime = 0);

	BOOL LoadMediaImage(UINT nList, int nItem, int nImage = SHOWIMG);

	void FreeMediaImage(UINT nList, int nItem);

	BOOL LoadMediaImageEx(UINT nList, int nID, int nImage = SHOWIMG);

	void FreeMediaImageEx(UINT nList, int nID);

	BOOL IsFirstCutOver(UINT nList);

	BOOL m_bExit;

	//Description:  第三方控件，搜索并播放
	//Input:        strFileName为文件名，不含拓展名 文件ID，列表ID
	//Output:       
	//Return:		
	void SearchIDInLocalList(CString& strFileName, int& nID, int& nListID);
private:
	typedef struct
	{
		CString sPath;
		int nList;
		BOOL bDetect;
	}DEALINFO, *LPDEALINFO;

	IPlayList();
	BOOL LoadList(CString& sListPath);// 加载列表
	void SetRand();//设置随机数组
	Media* GetMediaFromID(UINT nList, int nID);
	Media* FindMedia_(int nList, int nID);
	Media* FindMedia_(CString& strPath, int nList, int nBeginTime = 0, int nEndTime = 0, int nMax = -1);
	BOOL AddFile_();
	int  AddList_(CString& listPath, int nList);
	BOOL AddFile_(CString& strPath, UINT nList, UINT nMax = -1, BOOL bDetect = TRUE, __int64 nSize = 0);
	BOOL AddFolder(CString& strPath, UINT nList,  UINT nMax = -1, BOOL bDetect = TRUE);
	BOOL AddSimilarFiles(vector<CString>& vPath, UINT nList, int nPlay, BOOL bDetect = TRUE);
	void AnalysisiWpl(CString filename, CString& title, std::vector<CString> &vfile);
	void AnalysisiM3u(CString filename, CString& title, std::vector<CString> &vfile);
	int AnalysisiQpl(CString filename, int nList,  BOOL bPrivate = FALSE, int* pMergeCount = NULL);
	int AnalysisiCue(CString& filename, int nList);
	
	void InsertMedia(Media* pMedia, UINT nList);
	typedef vector<vector<Media*>> VMEDIA;

	IQvodCallBack *m_pEventObj;
	VMEDIA m_vMedia;
	vector<ListInfo*> m_vList; //列表数组
	vector<int> m_rand;//随机数组
	int m_nRandIndex;//当前随机数组的索引
	MediaHelper* m_pHelper;
	
	long m_nListID;
	int m_nSearchingList;
	BOOL m_bSearch;
	CString m_sListPath;
	CXMLDoc m_doc;
	int m_nLastPlayID;
	
	CRITICAL_SECTION m_Lock;
	vector<DEALINFO> m_vDeal;
	BOOL m_bFirstFind;
	HWND m_hWnd;
	int m_nInsertPos;
private:
	static IPlayList* m_pList;
};


class IMediaCenter : public IQvodCallBack

{
public:
	~IMediaCenter();
	static IMediaCenter* GetInstance();
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);
	friend class MediaHelper;
public:
//Operations

	//Description:    初始化工作
	//Input:          
	//Output:         
	//Return:		  BOOL
	BOOL Init_();

	//Description:    保存工作
	//Input:          
	//Output:         
	//Return:		  BOOL
	BOOL Save_(LPCTSTR lpUserName = NULL);


	//Description:    加载私人数据
	//Input:          lpUserName:用户名
	//Output:         pMergeCount:合并条目数
	//Return:		  BOOL
	BOOL LoadPrivateData(LPCTSTR lpUserName, int* pMergeCount = NULL);

	//Description:    移除私人数据
	//Input:          
	//Output:         
	//Return:		  BOOL
	void RemovePrivateData();

	//Description:    添加文件或文件夹到媒体库中（会筛选文件类型）
	//Input:          strpath:文件或文件夹的路径
	//Output:         
	//Return:		  BOOL
	BOOL AddFile(CString& strPath, UINT nType);
	BOOL AddFile(vector<CString>& vPath, UINT nType, BOOL bAddSimilar = FALSE);

	//Description:    搜索带有媒体文件的文件夹
	//Input:          strpath:文件或文件夹的路径。
	//Output:         
	//Return:		  BOOL
	//void SearchFolder();
	//BOOL SearchFolder_(CString& strPath);


	//Description:    获取媒体总数
	//Input:          nType 媒体类型 0-所有，1-视频，2-音频，3-搜索的结果 
	//Output:         
	//Return:		  媒体总数
	int GetMediaCount(UINT nType);

	//Description:    获取媒体信息
	//Input:          nType: 媒体类型 0-所有，1-视频，2-音频，3-搜索的结果   nItem:媒体索引
	//Output:         pMedia: 媒体信息指针		  	
	//Return:		  
	BOOL GetMediaInfo(Media* pMedia, UINT nType, UINT nItem);

	//Description:    获取媒体信息
	//Input:          pMedia: 媒体信息指针	nType: 媒体类型 0-所有，1-视频，2-音频，3-搜索的结果   nItem:媒体索引
	//Output:         	  	
	//Return:		  媒体总数
	BOOL SetMediaInfo(Media* pMedia, UINT nType, UINT nItem);

	//Description:    获取媒体信息
	//Input:          pMedia: 媒体信息指针	nType: 媒体类型 0-所有，1-视频，2-音频，3-搜索的结果   nItem:媒体索引 nMask:设置哪些属性
	//Output:         	  	
	//Return:		  媒体总数
	BOOL SetMediaInfo(Media* pMedia, UINT nType, UINT nItem, UINT nMask);

	//Description:    删除一个媒体
	//Input:          nType：媒体类型 0-所有，1-视频，2-音频，3-搜索的结果   nItem:媒体索引
	//Output:         		  	
	//Return:		  BOOL 
	BOOL DeleteMedia(UINT nType, UINT nItem);

	//Description:    删除一个媒体或一个文件夹下的所有媒体
	//Input:         strPath： 文件或文件夹的路径
	//Output:         		  	
	//Return:		  BOOL
	//BOOL DeleteMedia(CString& strPath);

	//Description:    删除一个类型的所有媒体
	//Input:		  nType：媒体类型 0-所有，1-视频，2-音频，3-搜索的结果
	//Output:         		  	
	//Return:		  BOOL
	BOOL DeleteType(UINT nType, BOOL bReservPrivate = TRUE);

	//Description:    移动列表中的媒体
	//Input:         vMedia 要移动的媒体索引数组  nPos 移动到哪个位置  nType 类型
	//Output:         		  	
	//Return:		  BOOL
	BOOL MoveMedia(vector<UINT>& vMedia, int nPos, UINT nType);
	//Description:    删除一个媒体
	//Input:          nType：媒体类型 0-所有，1-视频，2-音频，3-搜索的结果   sType:排序方式  bOrder:顺\逆序
	//Output:         		  	
	//Return:		  BOOL
	BOOL SortMedia(UINT nType, SORTTYPE sType, BOOL bOrder = TRUE);


	//Description:    根据关键字在媒体库中搜索相关的媒体
	//Input:          strKey: 关键字
	//Output:         		  	
	//Return:		  若找到相关数据返回TURE,否则返回FALSE
	BOOL SearchMedia(CString strKey);

	//Description:    获取一组分类信息
	//Input:          mType 虚拟显示的分类  mChildType 虚拟显示节点的分类
	//Output:         vClassInfo 一个分类信息数组		  	
	//Return:		  BOOL
	BOOL GetClassInfo(_MEDIAFILE_TYPE mType, _MEDIA_CHILD_TYPE mChildType, vector<MediaClassInfo*>& vClassInfo);

	//Description:    删除媒体
	//Input:          nClass 类型索引  nID 媒体文件的ID号
	//Output:         	
	//Return:		  BOOL
	BOOL DeleteMediaEx(UINT nClass, UINT nID);

	//Description:    删除一个类型的媒体
	//Input:          nClass 类型索引  
	//Output:         	  	
	//Return:		  BOOL
	BOOL DeleteClass(UINT nClass);

	BOOL GetMediaPath(CString& strPath, UINT nClass, UINT nItem);

	BOOL GetMediaPathEx(CString& strPath, UINT nClass, UINT nID);

	//Description:    设置媒体信息
	//Input:          pMedia 要设置的媒体信息  nClass 类型索引  nID 媒体文件的ID号
	//Output:         			  	
	//Return:		  BOOL
	BOOL SetMediaInfoEx(Media* pMedia, UINT nClass, UINT nID);

	//Description:    获取媒体信息
	//Input:          nClass 类型索引  nID 媒体文件的ID号	
	//Output:         pMedia 媒体信息		  	
	//Return:		  BOOL
	BOOL GetMediaInfoEx(Media* pMedia, UINT nClass, UINT nID);

	BOOL GetMediaInfoEx(Media* pMedia, UINT nClass, UINT nID, int& nIndex);

	//Description:    设置媒体信息
	//Input:          pMedia 要设置的媒体信息  nClass 类型索引  nID 媒体文件的ID号 nMask 要设置哪些属性
	//Output:         			  	
	//Return:		  BOOL
	BOOL SetMediaInfoEx(Media* pMedia, UINT nClass, UINT nID, LONG nMask);


	int GetIndexFromID(UINT nType, int nID, int nPos = 0);

	int GetIDFromIndex(UINT nType, int nItem);

	void SetEventParent(HWND hWnd);

	BOOL LoadMediaImage(UINT nType, int nItem, int nImage);

	void FreeMediaImage(UINT nType, int nItem);

	BOOL LoadMediaImageEx(UINT nType, int nID,  int nImage);

	void FreeMediaImageEx(UINT nType, int nID);

	BOOL IsFirstCutOver();

	void SetEventObj(IQvodCallBack* pObj);

	MediaNode* m_pRoot;// a tree

	BOOL m_bExit;

private:
	IMediaCenter();

	Media* AddMedia(CString& strPath, MediaNode* pFather, MediaNode* pPrv = NULL, UINT nType = 1, __int64 nSize = 0);

	BOOL AddFile_();
	BOOL AddFile_(CString& strPath, MediaNode* pNode, UINT nType);

	void AddNode(MediaNode* pNode, MediaNode* pFather, MediaNode* pPrv = NULL);

	void DeleteNode(MediaNode* pNode, MediaNode* pPrv, MediaNode* pFather);
	void DelNodeByPath(MediaNode* pNode, CString& strPath, MediaNode* pFather = NULL);
	void DelNodeByMedia(MediaNode* pNode, Media* pMedia, MediaNode* pFather = NULL);
	void DeleteVMedia(Media* pMedia);

	void DeleteAllEmptyNode();
	BOOL DeleteEmptyNode(MediaNode* pRoot, MediaNode* pPrv = NULL, MediaNode* pFather = NULL);
	/*void FindNode(MediaNode* pNode, CString& strPath, MediaNode*& pFind);*/
	BOOL FindNode(MediaNode* pNode, CString& strPath, MediaNode*& pFind,  int nBeginTime = 0, int nEndTime = 0, int nPos = 0);
	void FindNode(MediaNode* pNode, Media* pMedia, MediaNode*& pFind);
	BOOL FindNode(CString& strPath, int nType = 0);
	MediaNode* FindNodeInLayer(MediaNode* pFather, CString& strPath);
	//BOOL FindFather(MediaNode* pNode, CString& strPath,  MediaNode*& pFind, UINT nPos = 0);
	//BOOL DeleteClassNode(MediaNode* pNode, CString& strPath);
	

	BOOL DeleteChildsNode(MediaNode* pNode, BOOL bDelDisk = FALSE);
	
// 	BOOL LoadVMedia();
// 	BOOL LoadVMedia_(MediaNode* pNode);
	BOOL LoadPrivateMedia(CXMLElement &xmlElement);
	BOOL LoadMedia(CXMLElement &xmlElement, MediaNode* pNode, BOOL bPrivate = FALSE);
	BOOL SaveMedia(CXMLElement &xmlElement, MediaNode* pNode, CXMLDoc& doc, BOOL bPrivate = FALSE);

	BOOL Sort(_MEDIAFILE_TYPE mType, _MEDIA_CHILD_TYPE mChildType);

	IQvodCallBack *m_pEventObj;
	vector<MediaClassInfo*> m_vClassInfo;
	typedef vector<vector<Media*>> VMEDIA;
	VMEDIA m_vMedia;

	int m_nSortType;
	_MEDIAFILE_TYPE m_mType;
	_MEDIA_CHILD_TYPE m_mChildType;
	SORTTYPE m_sType;
	CRITICAL_SECTION m_Lock;


	HANDLE m_hSearch;
	HANDLE m_hStartEvent;
	MediaHelper* m_pHelper;
	CString m_csPath;
	BOOL m_bSearch;

	typedef struct
	{
		MediaNode* pNode;
		CString sPath;
		UINT nType;
	}DEALINFO,*LPDEAILINFO;

	vector<CString> m_sFolder;
	vector<DEALINFO> m_vDeal;
	HWND m_hWnd;
	static IMediaCenter* m_pCenter;

};

#endif
