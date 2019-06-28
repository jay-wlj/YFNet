#pragma once
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"

// CDownloadPage �Ի���



class CDownloadPage : public CDialog
{
	DECLARE_DYNAMIC(CDownloadPage)

public:
	CDownloadPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDownloadPage();

// �Ի�������
	enum { IDD = IDD_DLG_DOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTaskCreate();
	afx_msg void OnBnClickedTaskDown();
	afx_msg void OnBnClickedAesEncrypt();
	afx_msg void OnBnClickedAesDecrypt();
	afx_msg void OnBnClickedTaskDelete();

	void UpdateData();

private:
	string ReadAesKey(string strKeyPath);
private:
	char					m_szHash[41];
	unsigned int m_curtime;			// s
	vector<PItemData>	m_vItems;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedTaskDown2();
	CListCtrl m_listCtrl;
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnNMRClickListDownlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnCachepath();
	afx_msg void OnBnClickedTaskUpdateurl();
	afx_msg void OnEnChangeLimitDowncount();
};
