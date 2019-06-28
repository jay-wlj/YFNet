#ifndef _WEAKHINT_H_H__
#define _WEAKHINT_H_H__

class WearHint
{
public:
	WearHint();
	virtual ~WearHint();
	virtual BOOL Init_(CSkinWindow *pWeakHintWnd );
	virtual void Enable(BOOL bEnable);
	virtual void Hide();
	virtual BOOL IsVisible();
protected:
	BOOL		 m_bEnable;		
	BOOL		 m_bInitialized;
	CSkinWindow* m_pWeakHintWnd;
};

//////////////////////////////////////////////////////////////////////////
class LockTask
{
public:
	LockTask();
	~LockTask();	
	BOOL Init_(CString strSaveDir);
	void Clear_();
	BOOL IsLock(string hash ,CString strRefUrl);
	void QueryStatus(string hash ,CString strRefUrl);
	static LockTask& Instance();
private:
	void Load();
	void Save();
	int GetHashStatus(string hash);
	int GetUrlStatus(CString strUrl);
	void SetHasStatus(string hash, int status);
	void SetUrlStatus(CString strRefUrl, int status);
	BOOL IsHasLock(string hash);
	BOOL IsUrlLock(CString strUrl);
	BOOL IsNeedQuery(string hash, CString strRefUrl);
	void Parse(string &hash, CString &strRefUrl);
	int QueryInSrv(string hash ,CString strRefUrl, DWORD dwTimeOut = ATL_SOCK_TIMEOUT);
	void OnQueryStatus(string hash ,CString strRefUrl, DWORD dwTimeOut = ATL_SOCK_TIMEOUT);
	static DWORD WINAPI Thread_QueryStatus(LPVOID p);
	BOOL IsNeedUpdate(CString updateTag);
	CString GetTodayString();

	//////////////////////////////////////////////////////////////////////////
	// 360版本
#ifdef RDSP360
public:
	BOOL IsLockIn360Version(string hash ,CString strRefUrl);
private:
	BOOL IsHasLock360Version(string hash);
#endif

private:
	// 私人列表弱提示相关
	enum file_status
	{
		fs_null,
		fs_url_lock,
		fs_normal,
		fs_hash_lock,
		fs_notfind,
	};

	typedef struct _QueryPara 
	{
		LockTask		*pThis;
		string			 hash;
		CString			 strRefUrl;
	}QueryPara;

private:
	BOOL					m_bInitialize;
	CString					m_strSavePath;
	map<string ,int>		m_mHashStatus;
	map<CString ,int>		m_mUrlStatus;	 
	CRITICAL_SECTION		m_csLock;
};

//////////////////////////////////////////////////////////////////////////
// 私人弱提示
class PrivateWeakHint
{
public:
	PrivateWeakHint();
	~PrivateWeakHint();
	BOOL Init_(CSkinWindow *pWeakHintWnd ,CString strSaveDir);
	void Clear_();
	void Show(void);
	BOOL Show(const string& hash);
	BOOL Show(LPCTSTR szNomalText);
	void SetNormalText(LPCTSTR szNomalText);
	void Hide();
	BOOL IsVisible();
	void Enable(BOOL bEnable);
	BOOL IsEnable();
	BOOL IsLock(string hash ,CString strRefUrl);
	void QueryStatus(string hash ,CString strRefUrl);
	void ResetSkinElement( void );

	//////////////////////////////////////////////////////////////////////////
	// 360版本
#ifdef RDSP360
public:
	BOOL IsLockIn360Version(string hash ,CString strRefUrl);
#endif

private:
	int	 TransPercent(unsigned char c); 
private:
	CSkinWindow*			m_pWeakHintWnd;
	BOOL					m_bInitialize;
	BOOL					m_bEnable;	 
	BOOL					m_bForNomalTips;
	CString					m_strText;
};

// 缓存弱提示
class BufferWeakHint
{
public:
	BufferWeakHint();
	~BufferWeakHint();
	BOOL Init_(CSkinWindow *pWeakHintWnd );
	BOOL BeginBuffer();
	BOOL EndBuffer();
	void Enable(BOOL bEnable);
	void Show(const CString& txt,const CString& btnTxt);
	void Hide();
	BOOL IsVisible();
	void Reset();
private:
	BOOL		 m_bEnable;			
	time_t		 m_beginBufferingTime;	// 缓冲开始时间
	time_t		 m_fristBufferingTime;	// 第一次缓冲时间
	short        m_nBufferingCount;		// 缓冲次数
	long		 m_nValidInterval;		// 缓冲的有效间隔。
	CSkinWindow* m_pWeakHintWnd;
};

#ifdef RDSP360
// 使用标准版提示
class STDVerWeakhint		
{
public:
	STDVerWeakhint();
	~STDVerWeakhint();
	BOOL Init_(CSkinWindow *pWeakHintWnd);
	void Clear_();
	BOOL Show();
	void Hide();
	BOOL IsVisible();
	void Enable(BOOL bEnable);
	BOOL IsEnable();

	void EnableTimeTick(BOOL bEnable);
	BOOL IsEnableTimeTick();
	BOOL IsEnableTimeTick(REFERENCE_TIME totalTime);
	BOOL TimeTick(REFERENCE_TIME curTime);
	CSkinWindow* GetWindow();
private:
	BOOL  m_bInitialize;
	BOOL  m_bEnable;
	BOOL  m_bEnabTimeTick;
	CSkinWindow *m_pWeakHintWnd;
	REFERENCE_TIME  m_rtTriggerTime;
};
#endif

#endif