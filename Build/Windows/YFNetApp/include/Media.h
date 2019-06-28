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
	long nOrder;						// ѭ����ʽ 0:���� 1:����ѭ�� 2:˳�򲥷� 3:ѭ������ 4:�������
	long nSkipStart;					// ������ʼʱ��  Ĭ�� 0
	long nSkipEnd;						// ��������ʱ��  Ĭ�� 0
	long nTimeShow;						// ʱ����ʾ��ʽ  0 - ����  1 - ʱ���� Ĭ�� 0
	long nTitleShow;					// ������ʾ��ʽ
	long nID;
	BOOL bMCenterShowMode;
	BOOL bPlayListShowMode;
	int Width[7];
	CString strTitle;					// �б���
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
	CString strClass;					//ý��������
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

/*ö��������ʾ�ķ���*/
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

/* ö��������ʾ�µĽڵ����*/
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
	int nClass;//0-���磬1-��Ӱ��2-���� 4-�����б�
	BOOL bFirst;//�ص�һ��/��n��
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
	//�Ƿ������Ʊ���
	BOOL IsSimilarTitle(TitleInfo* t1, TitleInfo* t2);	
	//���������ļ�
	BOOL FindSimilarMedia(vector<CString>& vPath, int* pIndex = NULL);
	//��ȡ������Ϣ
	void GetTitleInfo(LPCTSTR lpPath, TitleInfo* pInfo);
	static bool CmpTitle(TitleInfo* t1, TitleInfo* t2);
	CString GetLnkTarget(LPCTSTR lpLnkName);

	//��ͼ���
	void FreeImg(Media* pm);
	BOOL LoadImg(Media* pm, int nImage = SHOWIMG);
	static BOOL TransToMini(Gdiplus::Image* pSrc, Gdiplus::Image* pDest, UINT uWidth = MINIIMG_W, UINT uHeight = MINIIMG_H);
	//��¡һ���µ�Bitmap���󣬲���ʱ����ʹ��delete�ͷš�
	static Gdiplus::Bitmap *Clone(Gdiplus::Bitmap *pBitmap); 
	REFERENCE_TIME ReadyToCut(CString& strPath);
	void OverCut();
	Gdiplus::Bitmap* CutImage(REFERENCE_TIME nTime);
	REFERENCE_TIME GetKeyPos(REFERENCE_TIME nTime);

	BOOL FilterXmlErrorChar(LPCTSTR lpXmlPath);
	void RecordCrash(CString& strPath, int nOperate);
	void ClearCrash(CString& strPath, int nOperate);
	BOOL ExistCrash(CString& strPath, int dwOperate);	
	BOOL ExistHaveAnyCrash(CString& strPath);			//�Ƿ��Ѿ����������ڽ�ͼ����Ϣ�ӿ���
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
	__int64		nFileSize;					//ý���ļ���С
	__int64     nDownSize;					//�����ش�С
	SIZE		videoSize;					//ͼ���С
	time_t		tmCreateTime;				//���ý���ʱ��	
	long		lPlayTime;					//�ϴ�ֹͣλ��(ms)
	long		lDuration;					//����ʱ��(s)
	long		lBeginTime;					//��ʼʱ��
	long		lEndTime;					//����ʱ��
	long		lRate;						//����	
	BOOL		bPlayEnd;					//�Ƿ񲥷����
	BOOL		bPrivate;					//�Ƿ�Ϊ˽���ļ�
	int			nType;						//�ļ�����(0-�ļ��У�1-��Ӱ��2-���֣�3-ͼƬ��4-δ֪��
	int			nID;						//ID��(Ψһ��ʶ)
	int			nError;						//������Ϣ
	int			nImgID[IMGCOUNT];			//����ͼID����
	Gdiplus::Image*		pImage[IMGCOUNT];	//����ͼָ������

	CString		strPath;					//�ļ�·��(Ψһ��ʶ)
	CString     strName;					//�ļ���
	CString		strTitle;					//����
	CString		strAuthor;					//������
	CString		strAlbum;					//ר��	

public:
	void Copy(Media* pDest);
	void Copy(Media* pDest, int nMask);
	void AddRef();
	void Release();
	void Release1();//��ɾ��Ӳ������ͼ�ļ�
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

	/*************ע�����´�Ex�ĺ���������ID�ŷ���Ԫ�أ�������������*********************/

	//Description:    ����ý����Ϣ
	//Input:          pMedia Ҫ���õ�ý����Ϣ nList	�б�����  nItem�б�������	
	//Output:         
	//Return:		  BOOL
	BOOL SetMediaInfo(Media* pMedia, UINT nList, UINT nItem);
	BOOL SetMediaInfoEx(Media* pMedia, UINT nList, UINT nID);

	//Description:    ��ȡý����Ϣ
	//Input:          nList	�б�����  nItem�б�������
	//Output:         pMedia ý����Ϣ
	//Return:		  BOOL
	BOOL GetMediaInfo(Media* pMedia, UINT nList, UINT nItem)const;
	BOOL GetMediaInfoEx(Media* pMedia, UINT nList, UINT nID);
	BOOL GetMediaInfoEx(Media* pMedia, UINT nList, UINT nID, int& nPos);

	//Description:    ��ȡý����Ϣ
	//Input:          nList	�б�����  nItem�б�������
	//Output:         pMedia ý����Ϣ
	//Return:		  BOOL
	BOOL SetMediaInfo(Media* pMedia, UINT nList, UINT nItem, LONG nMask);
	BOOL SetMediaInfoEx(Media* pMedia, UINT nList, UINT nID, LONG nMask);
	//Description:    ����ý��·��
	//Input:          strPath ý��·�� nList �б�����  nItem�б�������	
	//Output:         
	//Return:		  BOOL
	BOOL SetMediaPath(CString& strPath, UINT nList, UINT nItem);
	BOOL SetMediaPathEx(CString& strPath, UINT nList, UINT nID);

	//Description:    ��ȡý��·��
	//Input:          nList	�б�����  nItem�б�������	
	//Output:         strPath ý��·��
	//Return:		  BOOL
	BOOL GetMediaPath(CString& strPath, UINT nList, UINT nItem)const;
	BOOL GetMediaPathEx(CString& strPath, UINT nList, UINT nID);

	//Description:    ��ȡĳ���б���ý������
	//Input:          nList �б�����
	//Output:         
	//Return:		  ý������
	int GetMediaCount(UINT nList)const;

	//Description:    ��ȡ��\��һ��ý���ļ�λ��
	//Input:          nItem ��ǰ�������������û�в�����-1��ʾ�� nCount ������ nMode: ����ģʽnType 0-��һ����1-��һ����2-�Զ�����
	//Output:         
	//Return:		  ��\��һ��ý���λ��
	LONG GetNextMediaIndex(LONG nItem, UINT nCount, UINT nMode, UINT nType);

	//Description:    ��ȡý�����б��е�λ��
	//Input:          strPathý��·��  nList �б�����
	//Output:         
	//Return:		  �ļ�λ��
	UINT GetMediaPos(CString& strPath, UINT nList)const;

	//Description:    ��ȡ�б������
	//Input:          
	//Output:         
	//Return:		  �б�����
	UINT GetListCount()const;

	//Description:    �����б���Ϣ
	//Input:          pListInfo �б���Ϣ nList�б�����
	//Output:         
	//Return:		  BOOL
	BOOL SetListInfo(ListInfo* pListInfo, UINT nList);

	//Description:    ��ȡ�б���Ϣ
	//Input:          nList�б�����
	//Output:         pListInfo �б���Ϣ
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

	//Description:    ��ʼ������
	//Input:          
	//Output:         
	//Return:		  BOOL
	BOOL Init_();

	//Description:    ���湤��
	//Input:          
	//Output:         
	//Return:		  BOOL
	void Save_(LPCTSTR lpUserName = NULL);

	//Description:    ����˽������
	//Input:          lpUserName:�û���
	//Output:         pMergeCount:�ϲ���Ŀ��
	//Return:		  BOOL
	BOOL LoadPrivateData(LPCTSTR lpUserName, int* pMergeCount = NULL);

	//Description:    �Ƴ�˽������
	//Input:          
	//Output:         
	//Return:		  BOOL
	void RemovePrivateData();

	//Description:    ���ý�嵽ĳ���б���
	//Input:          strPath ý��·�� nList �б�����  bDetect �Ƿ����ļ�����
	//Output:         
	//Return:		  ý��id�ţ���ʧ�ܷ���0
	int AddMedia(CString& strPath, UINT nList, BOOL bDetect = TRUE, __int64 nSize = 0);

	//Description:    ���ý�嵽ĳ���б���
	//Input:          pMedia ý��ṹָ�� nList �б�����  
	//Output:         
	//Return:		  BOOL
//	BOOL AddMedia(Media* pMedia, UINT nList);

	BOOL AddMediaDirect(Media* pMedia, UINT nList);
	//Description:    ����ļ����ļ��е�ĳ���б���
	//Input:          strPath �ļ����ļ��е�·��  nList �б�����  bDetect �Ƿ����ļ�����
	//Output:         
	//Return:		  BOOL
	BOOL AddFile(CString& strPath, UINT nList, BOOL bDetect = TRUE, BOOL bAddSimilar = FALSE);
	BOOL AddFile(vector<CString>& vPath, UINT nList, BOOL bDetect = TRUE, BOOL bAddSimilar = FALSE);

	//Description:    ɾ��ĳ���б��е�ý��
	//Input:          nList �б�����  nItem �б�������
	//Output:         
	//Return:		  BOOL
	BOOL DeleteMedia(UINT nList, UINT nItem);
	BOOL DeleteMediaEx(UINT nList, UINT nID);

	//Description:    ɾ�������ý��
	//Input:         
	//Output:         
	//Return:		  BOOL
	BOOL DeleteErrorMedia();

	//Description:    �ƶ��б��е�ý��
	//Input:         vMedia Ҫ�ƶ���ý����������  nPos �ƶ����ĸ�λ��  nList �б�����
	//Output:         
	//Return:		  BOOL
	BOOL MoveMedia(vector<UINT>& vMedia, int nPos, UINT nList);

	//Description:    ����һ�����б�
	//Input:         
	//Output:         
	//Return:		  BOOL
	BOOL CreateList(CString& sListName, int nList = -1);

	//Description:    ���һ���б�
	//Input:          listPath �б��·�� nList��ӵ��ĸ��б�֮�£�-1��ʾ����һ���б�
	//Output:         
	//Return:		  BOOL
	int AddList(CString& listPath, int nList);

	//Description:    ���һ���б�
	//Input:          nList �б�����
	//Output:         
	//Return:		  BOOL
	BOOL ClearList(UINT nList,  BOOL bReservePrivate = TRUE);

	//Description:    ���б��е�ý���������
	//Input:          nList �б�����  nType ����ʽ  bOrder ˳/����  p
	//Output:         pHot  �����ý��λ��
	//Return:		  BOOL
	BOOL SortList(UINT nList, SORTTYPE nType, BOOL bOrder = TRUE);

	//Description:    ɾ��һ���б�
	//Input:          nList �б�����
	//Output:         
	//Return:		  BOOL
	BOOL DeleteList(UINT nList, BOOL bReservePrivate = TRUE);

	//Description:    �����б�
	//Input:          nList �б����� lpszPath Ҫ�����·����,��Ϊ���򱣴浽playlistĿ¼��,��Ϊ�û����򱣴浽��playlist_�û�������  
	//Output:         
	//Return:		  BOOL
	BOOL SaveList(UINT nList, LPCTSTR lpPath = NULL);

	//Description:    ����ý��·������ĳһ��ý�����Ϣ
	//Input:          strpath Ҫ���ҵ�ý��·�� nList �б����� 
	//Output:         
	//Return:		   ý��ID,δ�ҵ�����-1
	int FindMedia(CString& strPath, int nList,  int nBeginTime = 0, int nEndTime = 0);

	BOOL LoadMediaImage(UINT nList, int nItem, int nImage = SHOWIMG);

	void FreeMediaImage(UINT nList, int nItem);

	BOOL LoadMediaImageEx(UINT nList, int nID, int nImage = SHOWIMG);

	void FreeMediaImageEx(UINT nList, int nID);

	BOOL IsFirstCutOver(UINT nList);

	BOOL m_bExit;

	//Description:  �������ؼ�������������
	//Input:        strFileNameΪ�ļ�����������չ�� �ļ�ID���б�ID
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
	BOOL LoadList(CString& sListPath);// �����б�
	void SetRand();//�����������
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
	vector<ListInfo*> m_vList; //�б�����
	vector<int> m_rand;//�������
	int m_nRandIndex;//��ǰ������������
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

	//Description:    ��ʼ������
	//Input:          
	//Output:         
	//Return:		  BOOL
	BOOL Init_();

	//Description:    ���湤��
	//Input:          
	//Output:         
	//Return:		  BOOL
	BOOL Save_(LPCTSTR lpUserName = NULL);


	//Description:    ����˽������
	//Input:          lpUserName:�û���
	//Output:         pMergeCount:�ϲ���Ŀ��
	//Return:		  BOOL
	BOOL LoadPrivateData(LPCTSTR lpUserName, int* pMergeCount = NULL);

	//Description:    �Ƴ�˽������
	//Input:          
	//Output:         
	//Return:		  BOOL
	void RemovePrivateData();

	//Description:    ����ļ����ļ��е�ý����У���ɸѡ�ļ����ͣ�
	//Input:          strpath:�ļ����ļ��е�·��
	//Output:         
	//Return:		  BOOL
	BOOL AddFile(CString& strPath, UINT nType);
	BOOL AddFile(vector<CString>& vPath, UINT nType, BOOL bAddSimilar = FALSE);

	//Description:    ��������ý���ļ����ļ���
	//Input:          strpath:�ļ����ļ��е�·����
	//Output:         
	//Return:		  BOOL
	//void SearchFolder();
	//BOOL SearchFolder_(CString& strPath);


	//Description:    ��ȡý������
	//Input:          nType ý������ 0-���У�1-��Ƶ��2-��Ƶ��3-�����Ľ�� 
	//Output:         
	//Return:		  ý������
	int GetMediaCount(UINT nType);

	//Description:    ��ȡý����Ϣ
	//Input:          nType: ý������ 0-���У�1-��Ƶ��2-��Ƶ��3-�����Ľ��   nItem:ý������
	//Output:         pMedia: ý����Ϣָ��		  	
	//Return:		  
	BOOL GetMediaInfo(Media* pMedia, UINT nType, UINT nItem);

	//Description:    ��ȡý����Ϣ
	//Input:          pMedia: ý����Ϣָ��	nType: ý������ 0-���У�1-��Ƶ��2-��Ƶ��3-�����Ľ��   nItem:ý������
	//Output:         	  	
	//Return:		  ý������
	BOOL SetMediaInfo(Media* pMedia, UINT nType, UINT nItem);

	//Description:    ��ȡý����Ϣ
	//Input:          pMedia: ý����Ϣָ��	nType: ý������ 0-���У�1-��Ƶ��2-��Ƶ��3-�����Ľ��   nItem:ý������ nMask:������Щ����
	//Output:         	  	
	//Return:		  ý������
	BOOL SetMediaInfo(Media* pMedia, UINT nType, UINT nItem, UINT nMask);

	//Description:    ɾ��һ��ý��
	//Input:          nType��ý������ 0-���У�1-��Ƶ��2-��Ƶ��3-�����Ľ��   nItem:ý������
	//Output:         		  	
	//Return:		  BOOL 
	BOOL DeleteMedia(UINT nType, UINT nItem);

	//Description:    ɾ��һ��ý���һ���ļ����µ�����ý��
	//Input:         strPath�� �ļ����ļ��е�·��
	//Output:         		  	
	//Return:		  BOOL
	//BOOL DeleteMedia(CString& strPath);

	//Description:    ɾ��һ�����͵�����ý��
	//Input:		  nType��ý������ 0-���У�1-��Ƶ��2-��Ƶ��3-�����Ľ��
	//Output:         		  	
	//Return:		  BOOL
	BOOL DeleteType(UINT nType, BOOL bReservPrivate = TRUE);

	//Description:    �ƶ��б��е�ý��
	//Input:         vMedia Ҫ�ƶ���ý����������  nPos �ƶ����ĸ�λ��  nType ����
	//Output:         		  	
	//Return:		  BOOL
	BOOL MoveMedia(vector<UINT>& vMedia, int nPos, UINT nType);
	//Description:    ɾ��һ��ý��
	//Input:          nType��ý������ 0-���У�1-��Ƶ��2-��Ƶ��3-�����Ľ��   sType:����ʽ  bOrder:˳\����
	//Output:         		  	
	//Return:		  BOOL
	BOOL SortMedia(UINT nType, SORTTYPE sType, BOOL bOrder = TRUE);


	//Description:    ���ݹؼ�����ý�����������ص�ý��
	//Input:          strKey: �ؼ���
	//Output:         		  	
	//Return:		  ���ҵ�������ݷ���TURE,���򷵻�FALSE
	BOOL SearchMedia(CString strKey);

	//Description:    ��ȡһ�������Ϣ
	//Input:          mType ������ʾ�ķ���  mChildType ������ʾ�ڵ�ķ���
	//Output:         vClassInfo һ��������Ϣ����		  	
	//Return:		  BOOL
	BOOL GetClassInfo(_MEDIAFILE_TYPE mType, _MEDIA_CHILD_TYPE mChildType, vector<MediaClassInfo*>& vClassInfo);

	//Description:    ɾ��ý��
	//Input:          nClass ��������  nID ý���ļ���ID��
	//Output:         	
	//Return:		  BOOL
	BOOL DeleteMediaEx(UINT nClass, UINT nID);

	//Description:    ɾ��һ�����͵�ý��
	//Input:          nClass ��������  
	//Output:         	  	
	//Return:		  BOOL
	BOOL DeleteClass(UINT nClass);

	BOOL GetMediaPath(CString& strPath, UINT nClass, UINT nItem);

	BOOL GetMediaPathEx(CString& strPath, UINT nClass, UINT nID);

	//Description:    ����ý����Ϣ
	//Input:          pMedia Ҫ���õ�ý����Ϣ  nClass ��������  nID ý���ļ���ID��
	//Output:         			  	
	//Return:		  BOOL
	BOOL SetMediaInfoEx(Media* pMedia, UINT nClass, UINT nID);

	//Description:    ��ȡý����Ϣ
	//Input:          nClass ��������  nID ý���ļ���ID��	
	//Output:         pMedia ý����Ϣ		  	
	//Return:		  BOOL
	BOOL GetMediaInfoEx(Media* pMedia, UINT nClass, UINT nID);

	BOOL GetMediaInfoEx(Media* pMedia, UINT nClass, UINT nID, int& nIndex);

	//Description:    ����ý����Ϣ
	//Input:          pMedia Ҫ���õ�ý����Ϣ  nClass ��������  nID ý���ļ���ID�� nMask Ҫ������Щ����
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
