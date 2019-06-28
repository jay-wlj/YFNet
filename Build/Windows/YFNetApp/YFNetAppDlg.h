
// YFNetAppDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include "afxcmn.h"
#include "UploadPage.h"
#include "DownloadPage.h"
#include <string>
using std::string;

// CYFNetAppDlg 对话框
class CYFNetAppDlg : public CDialogEx
{
// 构造
public:
	CYFNetAppDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_YFNETAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	void PutString(CString& str);
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedInit();
	afx_msg void OnBnClickedUninit();
	void EventCallBackFunc(SCallBackPara& para);
private:
	HMODULE		m_hModule;

	long				m_lPointer;



public:
	afx_msg void OnLbnSelchangeList1();
	CButton m_initBtn;
	CButton m_uninitBtn;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedStartUpload();
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnDropFiles(HDROP hDropInfo);

private:
	void CreateUploadTask(string strfilePath);

private:
	CDownloadPage	m_pageDownload;
	CUploadPage	m_pageUpload;
public:
	afx_msg void OnBnClickedPageUpload();
	afx_msg void OnBnClickedPageDownload();
	CListBox m_listBox;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
};
