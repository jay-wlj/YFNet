#pragma once
//---------------------------------------------------------------------------
//creat by yh
//2013/1/24
//for 2d to 3d
//--------------------------------------------------------------------------

class CToolTips
{
public:
	CToolTips();
	~CToolTips() {};
	BOOL CreateToolTips(HWND hParent);
	void AddTool(HWND hContainWnd,UINT uid,CString strTip,RECT *rcTip);
	BOOL ActiveTool();
	void AlterTipText(HWND hContainWnd,UINT uid,CString strTip );
	void Clear();

private:
	BOOL		m_bHasTips;
	HWND	m_hTipWnd;
	TOOLINFO m_toolInfo;
	map<CString,int> m_mapToolText;
};