#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "lock.h"
#include <string>
#include <vector>
using namespace std;
// CUploadPage 对话框


class CUploadPage : public CDialog
{
	DECLARE_DYNAMIC(CUploadPage)

public:
	CUploadPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUploadPage();

// 对话框数据
	enum { IDD = IDD_DLG_UPLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_listCtrl;
	afx_msg void OnBnClickedStartUpload();

	void UpdateUrlData(const string strInfo);
	void UploadFiles(vector<string> vFiles);
	void CaclFileHash();
private:
	void CreateUploadTask(string strfilePath);
	void UpdateData();
	
	void CopyToClipBoard(string url);
private:
	char					m_szUploadHash[41];
	vector<PItemData>	m_vItems;
	bool				m_bPause;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnCbnSelchangeComboLimitMode();
	LRESULT OnGetFileHash(WPARAM wParam, LPARAM lParam);
	CComboBox m_comboLimit;
	CButton m_btnPauseResume;
public:
	HANDLE		m_hThread;
	QLock::CriticalSection				m_fileLock;
	vector<string>				m_vFilePaths;		// 上传的文件路径
	map<string, string>	m_mFileHash;		// 文件hash

	BOOL	m_bExit;
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLvnItemchangedListUpfile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListUpfile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
