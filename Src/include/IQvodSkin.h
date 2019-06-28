/*
* Copyright(c) 2010, �����п첥�Ƽ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�IQvodSkin.h
* ժ    Ҫ�������ͷ�ļ��Ķ�������
*
* ��ǰ�汾��1.0
* ��    �ߣ����� �ĳɴ� ������
* ������ڣ�
*/

#include "..\Common\timer.h"
#include "..\QvodPlayer\skin\Layout.h"

#ifndef	_IQVOD_SKIN_H__
#define _IQVOD_SKIN_H__

//�ؼ���״̬
#define  SKIN_STATE_NUM          5                 //�ؼ�״̬��
#define  SKIN_STATE_NL           0                 //����       normal
#define  SKIN_STATE_HV           1                 //����       hover
#define  SKIN_STATE_DN           2                 //����       down
#define  SKIN_STATE_DS           3                 //������     disable
#define  SKIN_STATE_GY           4                 //�һ�       gray
class CSkinWindow;
/******************
* ��ǰ�汾��1.0
* ��    �ߣ�����
* ������ڣ�
********************/
interface ISkin
{
public:
	virtual ~ISkin(){};
	// ����
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry)=0;				// ��ʼ��
	virtual void Clear_()=0;															// ����ʼ��
	virtual void ExitThread()=0;														// �˳��߳�
	//	virtual ISkin* CreateSkinObj(CXMLElement &xmlNode, IQvodCallBack *pEventEntry)=0;	// ������Ԫ��
	//virtual void AppendChild(ISkin* pSkinObj)=0;										// �����Ԫ��
	virtual BOOL Layout(short nWidth, short nHeight)=0;						// ����
	virtual BOOL ReLayout() = 0;
	virtual ISkin* FindChild(LPCTSTR szID)=0;								// ����Ƥ������
	virtual void Paint(Graphics *pGraph)=0;									// ��Ƥ������

	// ����
	virtual void SetState(skin_state state)=0;								// ����״̬
	virtual void SetChildState(LPCTSTR szID ,skin_state state)=0;
	virtual BOOL SetChildText(LPCTSTR szID, LPCTSTR szText)=0;
	virtual long Method_(LPCTSTR szMethod, long para1, long para2)=0;		// ���÷���

	// ����
	virtual skin_type GetSkinType()=0;						// �ÿؼ�����
	virtual CString GetID()=0;								// ��ID
	virtual BOOL CheckCursor(short x, short y)=0;			// ̽�����
	virtual BOOL GetCtrlRect(LPCTSTR szID, Rect &rect)=0;	// �ÿؼ�λ��
	virtual BOOL IsVisible()=0;								// �Ƿ�ɼ�
	virtual BOOL IsEnable() = 0;

	// ��Ϣ
	virtual BOOL OnTimer()=0;							// ��ʱ��
	virtual BOOL OnMouseMoveIn(short x, short y)=0;		// ����ƽ�
	virtual BOOL OnMouseMoveOut()=0;					// ����Ƴ�
	virtual BOOL OnMouseDown(short x, short y)=0;		// ��������
	virtual BOOL OnMouseUp(short x, short y)=0;			// ��������
	virtual BOOL OnMouseDrag(short x, short y)=0;		// ����϶�
	virtual BOOL OnRButtonUp(short x, short y)=0;		// ����Ҽ���
	virtual BOOL OnRButtonDown(short x, short y)=0;		// ����Ҽ���
	virtual BOOL OnMouseMove(short x, short y)=0;		// ����ƶ�
	virtual BOOL OnDblClick(short x, short y)=0;		// ���˫��
	virtual BOOL OnWheel(short nStep)=0;				// ������
	virtual BOOL OnKey(WORD nChar)=0;					// ���̰���
	virtual long OnDrag(UINT uMsg, long wparam, long lparam)=0; //��Ӧ�϶��йص���Ϣ
	virtual BOOL DragCheckData(IDataObject *pObject)=0;
	virtual BOOL IsShowRectContain(short x, short y)=0;

	virtual CString GetTipsID() = 0;									//��ȡ��Ҫ��ʾtips�Ŀؼ�ID

	// DragDrop���
	virtual BOOL DragMouseIn(short x, short y)=0;
	virtual BOOL DragMouseOut()=0;
	virtual BOOL DragMouseMove(short x, short y)=0;
	virtual BOOL DragMouseUp(short x, short y)=0;
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�����
* ������ڣ�
********************/
class CSkinCtrl : public ISkin
{
public:

	CSkinCtrl();
	virtual ~CSkinCtrl(){};
	static BOOL BitmapFromFile(LPCTSTR path, Bitmap ** pRefBmp); // ��ȡͼƬ�ļ�
	skin_type GetSkinType(){return m_type;}
	void SetSkinType(skin_type sType){m_type = sType;}
	CString GetID(){return m_strID;}

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual void ExitThread();
	virtual void Clear_();
	virtual BOOL Layout(short nWidth, short nHeight);
	virtual BOOL ReLayout();
	virtual void Paint(Graphics *pGraph);
	virtual void Updata();

	//�����ػ档 ��bRepaintWndΪTrue,���ػ�ؼ����ڵ��������ڣ�͸���ؼ�����true),����ֻ�ػ��Լ���
	virtual void Paint(BOOL bRepaintWnd = FALSE);					

	virtual void SetState(skin_state state);
	virtual BOOL IsVisible(){return m_bShow;}
	virtual BOOL IsEnable(){ return !m_bDisable;}
	virtual void EnableChildCtrls(BOOL bEnable);

	virtual void SetChildState(LPCTSTR szID ,skin_state state);
	virtual BOOL SetChildText(LPCTSTR szID, LPCTSTR szText);
	ISkin* CreateSkinObj(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void LoadChildren(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void SetUpdateTag( void );
	void SetEventObj(IQvodCallBack *pEventEntry);
	ISkin* FindChild(LPCTSTR szID);
	BOOL CheckCursor(short x, short y);
	BOOL GetCtrlRect(LPCTSTR szID, Rect &rect);
	BOOL IsCanShow();
	HWND GetParentWnd();

	virtual long Method_(LPCTSTR szMethod, long para1, long para2);
	virtual BOOL OnTimer();
	virtual BOOL OnMouseMoveIn(short x, short y);
	virtual BOOL OnMouseMoveOut();
	virtual BOOL OnMouseDown(short x, short y);
	virtual BOOL OnMouseUp(short x, short y);
	virtual BOOL OnMouseDrag(short x, short y);
	virtual BOOL OnDblClick(short x, short y);
	virtual BOOL OnWheel(short nStep);
	virtual BOOL OnKey(WORD nChar);
	virtual BOOL OnMouseMove(short x, short y){return FALSE;};
	virtual BOOL OnRButtonUp(short x, short y);		// ����Ҽ���
	virtual BOOL OnRButtonDown(short x, short y);	// ����Ҽ���
	virtual long OnDrag(UINT uMsg, long wparam, long lparam); //��Ӧ�϶��йص���Ϣ
	virtual BOOL DragCheckData(IDataObject *pObject);
	virtual BOOL IsShowRectContain(short x, short y);
	virtual CString GetTipsID();									//��ȡ��Ҫ��ʾtips�Ŀؼ�ID
	
	// DragDrop���
	virtual BOOL DragMouseIn(short x, short y);
	virtual BOOL DragMouseOut();
	virtual BOOL DragMouseMove(short x, short y);
	virtual BOOL DragMouseUp(short x, short y);

public:
	std::vector<ISkin*> m_pvSkinObj;
	BOOL m_bSkinObjConstructOK;		//Ƥ���ؼ����������
	SKIN_LAYOUT m_layout;
	BOOL m_bOver;					//����ڿؼ���
	CString m_strID;
	Rect m_rect;
	SIZE m_oldSize;
	Bitmap *m_pBmp;
	RECT	m_rcCanShow;

	ui::Layout m_showRectLayout;


protected:
	skin_type m_type;
	cursor_type m_cursor;
	CString m_strTip;
	skin_state m_state;
	DWORD m_bgColor;			// ����ɫ
	DWORD m_transColor;			// ͸��ɫ
	BOOL m_bShow;

	BOOL m_bDown;
	BOOL m_bDisable;
	BOOL m_bDirty;  //�ж��Ƿ���Ҫ�ػ��ˡ��Ƕ���ģʽ�滭��ֻ��һ��
	IQvodCallBack *m_pEventObj;

	float m_alpha;
	enum Ctrl_State
	{
		Ctrl_Valide		= 1,
		Ctrl_Normal		= 2,
		Ctrl_Lighting	= 4,
		Ctrl_FocusIn	= 8,
		Ctrl_Glassing	= 0x10,
		Ctrl_Show		= 0x20,
	};
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�
********************/
class CSkinButton : public CSkinCtrl // ��ͨ��ť, ����ͼ�����֣�ͼ������������ʽ
{
public:
	CSkinButton();
	virtual ~CSkinButton();
	void Paint(Graphics *pGraph);
	void Clear();
	void SetImage(LPCTSTR imagepath);
	void SetImage(Bitmap *bmp);
	void IsAuto(BOOL autoTure);
	virtual BOOL CheckCursor(short x, short y);
	virtual BOOL OnTimer();
	virtual BOOL OnMouseMoveIn(short x, short y);
	virtual BOOL OnMouseMoveOut();
	virtual BOOL OnMouseMove(short x, short y);
	virtual BOOL OnMouseDown(short x, short y);
	virtual BOOL OnMouseUp(short x, short y);
	virtual BOOL OnDblClick(short x, short y);
	virtual BOOL OnMouseDrag(short x, short y);
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void SetChoose(BOOL bChoose);
	void SetBkColor(DWORD bkcolor);
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	DWORD GetBkColor();
	void ReNormal();
	ISkin *GetSkinObj(short x, short y);

public :
	DWORD m_state;
	BOOL m_btValid;
	DWORD m_alphaSteps;
	DWORD m_step;
	DWORD m_dframColor;
	BOOL m_btChange; //��ť�ж���ʱΪTrue
	BOOL m_auto; // ��ť�Ƿ��Ƕ���Ч����TRUE�Ƕ���Ч����
	DWORD m_crBk;
	BOOL m_bChoose;
	Bitmap *m_pLeftImg;
	Bitmap *m_pRightImg;
	BOOL  m_bCanDrag;			// �Ƿ���Ҫ���϶�ʱ��������Ϣ��

	CString m_strImgPath;
	CString m_strSecondImgPath;
	BOOL	m_CanClickAlpha;		//͸���ȿ��Ե��


	BOOL	 m_bUseClrFillBk;		//�Ƿ���ô�ɫ��䱳��ɫ
	DWORD m_dwNormalClr;			//��ɫ��䣬���������ɫ
	DWORD m_dwOverClr;				//��ɫ��䣬���������ɫ
	DWORD m_dwDownClr;				//��ɫ��䣬���µ����ɫ

protected:
	BOOL m_bMoveShow;
	BOOL m_bMultiImg;
	int  m_nDownPos;
	// �����Ƿ�ƫ��
	BOOL m_bDownMove;

	ISkin *m_pSkinOver;
	ISkin *m_pSkinDown;
	DWORD m_dwTextOverColor;
	DWORD m_dwTextColor;
};


/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2010.03.03
* ����˵�����������������񱾵���Ϣ�ȿؼ����л���ť��
*           ʵ��Tab�а�ť�е�ѡ���....��֪�����������
********************/
class CSkinTabButton : public CSkinButton
{
public:
	CSkinTabButton();
	virtual ~CSkinTabButton();

	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL CheckCursor(short x, short y);
	virtual BOOL OnTimer();
	virtual BOOL OnMouseMove(short x, short y);
	virtual BOOL OnMouseDown(short x, short y);
	virtual BOOL OnMouseUp(short x, short y);
	virtual BOOL OnMouseMoveIn(short x, short y);
	virtual BOOL OnMouseMoveOut();

	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Paint(Graphics *pGraph);
	BOOL CheckOptionBt(short x, short y);

	void SetState(skin_state state);

private:
	BOOL m_showBt;		//Option��ť�Ƿ���ʾ
	BOOL m_bAlwaysSendMSG ; //�Ƿ�һֱ������Ϣ
	BOOL m_optionOver; //�����option��ť
	CString m_OptionBtstrID;
	BOOL m_bChangeFont;
	BOOL m_bInnermsg;
	
	BOOL  m_bTabMsg ;			//�Ƿ���TabFrame����
	DWORD m_dwFramOverColor   ; //��߿�����ƶ���ȥ��ɫ
	DWORD m_dwFramChooseColor ;	//��߿�ѡ����ɫ
	BOOL  m_bDrawFrame ;		//�Ƿ���ʾ��߿�
	BOOL  m_bVip ;				//�Ƿ�Vip���� 
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�
* ������ڣ�
********************/
class CSkinText : public CSkinCtrl 
{
public:
	CSkinText();
	virtual ~CSkinText(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Paint(Graphics *pGraph);
	long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL OnTimer();
	CString GetStr();
	void SetBigFont(BOOL bBig);
	void SetEventParent(IQvodCallBack *pEventEntry);
	void SetLineSpace(int nSpace);				//�����о�
	void SetMultiLineByChar(BOOL bByChar);		//�����Ƿ��ַ�����
	BOOL ShowOmitPlayTime(Graphics *pGraph);	//��ʾʡ�ԵĲ���ʱ��
	virtual BOOL IsShowRectContain(short x, short y);

	CString m_strText;
	CString m_face;
	float	m_size;
	DWORD	m_color;
	DWORD   m_offcolor;
	DWORD   m_colorDN;
	DWORD	m_colorHover;			// ��ͣ��ɫ
	DWORD	m_colorMouseDown;		// ��갴����ɫ
	DWORD	m_colorDisable;
	BOOL    m_bShowDNclr;
	int		m_style;
	long	m_textWidth;
	float   m_tempsize;
	DWORD	m_alignment;
	BOOL	m_bAutoShow;		// �Զ�������ʾЧ��
	BOOL	m_bHotkeyPrefixShow;// �Ƿ񽫡�&��ת��Ϊ�»���
	int		m_nOffsize_y;
	int     m_nOffsize_x;
	DWORD	m_effect;			// ����Ч��
	BOOL	m_bMultiLineByChar;	//���ַ����У����򰴴ʻ���
	int		m_nLineSpace;		// �о�
	BOOL	m_bEllipsisChar;	// ��ʾ����ȫ������ʡ�Ժ���ʾ
	BOOL	m_bEllipsisAnimation; // �����е�ʡ�ԺŶ�̬��ʾ
	BOOL	m_bAutoEllipsis;	// �Զ�ʡ�ԺŶ�̬��ʾ
	int		m_nShowDotCount;
	DWORD	m_dwTick;			// ��ʱ��
	BOOL	m_bLinkTextStyle;	// ��������ʽ
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2010.04.20
********************/
class CSkinLinkText: public CSkinText
{
public :
	CSkinLinkText();
	virtual ~CSkinLinkText();

	virtual BOOL OnMouseDown(short x, short y);
	virtual BOOL OnMouseUp(short x, short y);
	virtual BOOL OnMouseMoveIn(short x, short y);
	virtual BOOL OnMouseMoveOut();
	virtual BOOL CheckCursor(short x, short y);
	virtual BOOL OnTimer();

	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	long Method_(LPCTSTR szAttrib, long para1, long para2);
	void SetTitleAndURL(LPCTSTR szTitle, LPCTSTR strURL);
	void Paint(Graphics *pGraph);

	CString m_strURL;
	DWORD	m_color;
	DWORD	m_overColor;
	DWORD	m_disableColor;
	BOOL    m_bUnderLine;
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ���־��
* ������ڣ�2014.07.02
********************/
struct tagRollText
{
	CString strText;
	CString strURL ;
};

class CSkinRollLinkText: public CSkinLinkText
{
public :
	CSkinRollLinkText();
	virtual ~CSkinRollLinkText();

	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Paint(Graphics *pGraph);
	BOOL Layout(short nWidth, short nHeight);
	void Clear_();

	void InserItem(CString strText, CString strUrl) ;
	void RemoveAllItem();

private:
	static DWORD WINAPI ThreadRoll(LPVOID lp);
	void DoRollProcess();

protected:
	vector<tagRollText*> m_vtLinkItems ;
	int					 m_nIndexRoll  ;  //��ǰ������Item����
	DWORD				 m_dwRollTimes ;  //�л���������ʱ��Ƶ��
	int					 m_nRollSpeed ;  //�����ٶ�, ����һ�ε�ʱ��
	int					 m_nRollStep;  ;  //�����Ĳ���
	int					 m_nRollTotal ;	  //��������
	BOOL				 m_bNeedRoll;	  //�Ƿ���Ҫ����
	BOOL				 m_bExit;
	HANDLE				 m_hThread;
	CRITICAL_SECTION     m_rslock ;
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�wlj
* ������ڣ�2010.04.20
********************/
class CSkinHtmlText: public CSkinText
{
public :
	CSkinHtmlText();
	virtual ~CSkinHtmlText();
	void Paint(Graphics *pGraph);

protected:
	COLORREF StrToRGB(const wchar_t* strClr);			//�����ַ����е�rgbֵ
	void RenderHtml(HDC hDC, LPCTSTR lpszContent, Rect rcText, int nLineHeight = 0);
};
/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�
* ������ڣ�
********************/
class CSkinImage : public CSkinCtrl	// ͼƬ
{
public:
	CSkinImage();
	virtual ~CSkinImage(){};
	void Paint(Graphics *pGraph);
	void Clear();
	void ShowPart(int part, int total);
	void SetImag(Bitmap *bmp); //ljm
	virtual BOOL OnTimer();
	virtual BOOL CheckCursor(short x, short y);
	virtual void SetImage(LPCTSTR imagepath);
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void EnableAuto(BOOL bAuto);
	long Method_(LPCTSTR szMethod, long para1, long para2);
protected:
	void PaintBloom(Graphics *pGraph);		//��������ͼƬ

private:
	void BLoomImg();
public :
	DWORD m_state; 
	BOOL m_btDown;
	BOOL m_btChange;
	BOOL m_btOver;
	BOOL m_btValid;
	DWORD m_alphaSteps;
	DWORD m_step;
	long m_offset;
	long m_total;
	BOOL m_bShowOnce ;		//ֻ��ʾһ��

	BOOL	m_bAnimation;	//�Ƿ��Ƕ���
	int		m_iDelayTime;	//�ӳ���ʱ��
	int		m_iClockTime;	//�ӳ�ʱ���ʱ��
	BOOL	m_bStatic;		//�Ƿ�Ϊ��̬ͼƬ
	BOOL	m_bAuto;		//������ʾ
	BOOL	m_bSingleStretch;//����ͼƬ,�����췽���С����
	BOOL	m_bBloom;		//�ȱ����ţ�ͼƬ��������Ӧ����
};

class CSkinAnimation : public CSkinCtrl
{
public:
	CSkinAnimation();
	virtual ~CSkinAnimation();
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual BOOL OnTimer();
	void Paint(Graphics *pGraph); 
	void Clear_();
//private:
	BOOL LoadImage(CString path);
public :
	vector<int>			m_vDelay;
	vector<Bitmap*>		m_vpFrameBmp;
	int					m_nCurrentFrameIndex;
	int					m_nTickcount;
};
/******************
* ��ǰ�汾��1.0
* ��    �ߣ�wlj
* ������ڣ�
********************/
class CSkinTextureImg : public CSkinImage	// ͼƬ
{
public:
	CSkinTextureImg();
	virtual ~CSkinTextureImg(){};
	void Paint(Graphics *pGraph);
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
public :
	int m_nHCount;		//ˮƽ������ͼ����
	int m_nVCount;		//��ֱ������ͼ����
};


class CSkinLinkImage : public CSkinImage	// ͼƬ
{
public:
	CSkinLinkImage();
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual BOOL CheckCursor(short x, short y);
	virtual BOOL OnMouseDown(short x, short y);
	virtual BOOL OnMouseUp(short x, short y);
	virtual long Method_(LPCTSTR szMethod, long para1, long para2);
	virtual void SetImage(LPCTSTR imagepath);
	virtual BOOL Layout(short nWidth, short nHeight);

	CString			m_strURL;		// ����URL
	Rect			m_rcLink;		// ���Ӹ�Ӧ��
	BOOL			m_bEnableLink;	// �����Ƿ����
	BOOL			m_bAutoLinkRc;	//  �Զ�������������
	
};

/******************
* ��ǰ�汾��
* ��    �ߣ�������
* ˵    ��: ���ݲ�ͬ״̬����ʾ��ͬͼƬ
			״̬��ӦͼƬ�����
* ������ڣ�
********************/
class CSkinStateImage : public CSkinImage
{
public:
	CSkinStateImage();
	void Paint(Graphics *pGraph);
	virtual long Method_(LPCTSTR szMethod, long para1, long para2);
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
public:
	int		m_nStates;				// �����״̬
	int		m_nStatesImgNum;		// ״̬��
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�
* ���ܣ�
********************/
class CSkinCtrlForm : public CSkinCtrl
{
public:
	CSkinCtrlForm();
	virtual BOOL OnTimer();
	void Paint(Graphics *pGraph);
	virtual BOOL CheckCursor(short x, short y);
	virtual BOOL OnMouseDown(short x, short y);								    //��괦����Ϣ-����
	virtual BOOL OnMouseUp(short x, short y);
	virtual BOOL OnMouseMoveIn(short x, short y);
	virtual BOOL OnMouseMoveOut();
	virtual BOOL OnMouseMove(short x, short y);
	ISkin* GetSkinObj(short x, short y);
	virtual BOOL Layout(short nWidth, short nHeight);
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	long Method_(LPCTSTR szMethod, long para1, long para2);
    
	void LayoutLoginForm();
	CString GetSelCtrlID();

	// ��ؼ�
	ISkin *m_pSkinOver;
	ISkin *m_pSkinDown;
	ISkin *m_pSkinSel; // ��ǰѡ��Ŀؼ�
private:
	
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�
* ������ڣ�3.17
* ���ܣ�����static�ؼ�
********************/
class CSkinStatic_Text : public CSkinText
{
public:
	CSkinStatic_Text();
	virtual ~CSkinStatic_Text(){};
	void Paint(Graphics *pGraph);
	COLORREF m_crBorder;
};
/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�
* ������ڣ�
********************/
class CSkinButton_Text : public CSkinButton
{
public:
	CSkinButton_Text();
	virtual ~CSkinButton_Text(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL OnRButtonUp(short x, short y);
	CString GetStr();
	void SetFontColor(DWORD fontcolor);
public:
	CString m_txtStyle;
};

class CSkinButton_LinkText : public CSkinButton_Text
{
public:
	virtual BOOL OnMouseMoveIn(short x, short y);
	virtual BOOL OnMouseMoveOut();
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�����
* ������ڣ�
* ˵��������ý����б�İ�ť
********************/
class CSkinButton_TextEx : public CSkinButton_Text
{
public:
	CSkinButton_TextEx();
	virtual ~CSkinButton_TextEx();
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual BOOL OnMouseUp(short x, short y);
	virtual BOOL OnDblClick(short x, short y);
	virtual void Paint(Graphics *pGraph);
	void SetFold(BOOL bFold);
private:
	Bitmap* m_pArrowBmp;
	Bitmap* m_pArrowBmp2;
	BOOL m_bFold;
	BOOL m_bSelectState;
	int m_nArrowX;
	int m_nArrowY;
	int m_nArrowX2;
	int m_nArrowY2;
	float m_fTextSize;
	
};

class CSkinButton_TextSelect : public CSkinButton_Text
{
public:
	CSkinButton_TextSelect();
	virtual ~CSkinButton_TextSelect();
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual BOOL OnMouseUp(short x, short y);
	virtual BOOL OnDblClick(short x, short y);
	virtual void Paint(Graphics *pGraph);
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
private:
	BOOL m_bSelectState;
	DWORD m_dSelectColor;
	DWORD m_dNormalColor;
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�
* ������ڣ�
********************/
class CSkinButton_Step : public CSkinButton_Text // ������ť
{
public:
	CSkinButton_Step();
	virtual ~CSkinButton_Step(){};
	BOOL OnMouseDown(short x, short y);
	BOOL OnMouseUp(short x, short y);
	void static CALLBACK MyTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
public:
	DWORD		m_state;
	UINT		m_nAlphaSteps;
	UINT		m_nStep;
	static UINT m_nTimerID;
	static UINT m_nElapse;
	static UINT m_nCount;

};

/*
**     ������CSkinMenuBar
** ����˵�����˵�������갴ť�����������������ȣ���ɰ�ť
**           �Ķ�̬������㣬��̬���֣���¼�˵����˵��Ƿ�
**           ���������˵���ţ���ż�¼Ϊ�˵���ť��Ӧ����
**           ��Ӧ���Ҽ���Ϣ�����ղ˵���ʧ��Ϣ���仯��ť״̬��
**           ��ӦESC������ӦAlt������Ӧ�˵��ȼ�����Ӧ�س����ȡ�
*/
class CSkinColorMenu;
class CSkinMenuBar : public CSkinCtrl
{
public:
	CSkinMenuBar();
	virtual ~CSkinMenuBar(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	BOOL Layout(short nWidth, short nHeight);
	BOOL OnTimer();
	BOOL OnKey(WORD nChar);
	void ManualPaint(int btnId);
	void Paint(Graphics *pGraph);
	BOOL CheckCursor(short x, short y);
	BOOL OnMouseMoveIn(short x, short y);
	BOOL OnMouseMove(short x, short y);
	BOOL OnMouseMoveOut();
	BOOL OnMouseDown(short x, short y);
	INT WillAutoSwithMenu(short x, short y);
	ISkin* GetSkinObj(short x, short y);
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	void PopMenu(long btnId);
	void UnPopMenu(long btnId);
	static LRESULT CALLBACK MenuInputFilter(int code, WPARAM wp, LPARAM lp);
	static DWORD WINAPI Thread_lrMove(LPVOID lp);
public:
	UINT					m_Alternation;	// �Ƿ��Զ���ʾ��һ���˵���0���Զ���1����ƶ���2���Ҽ���3��ݼ���
	INT						m_nowmenuId;	// �����˵�id
	INT						m_lastmenuId;	// �����˵�id
	std::map<int, HMENU>	m_btnId2Hmenu;	// �˵�Id�Ͳ˵������
	ISkin*					m_pOver;		// ������ť
	ISkin*					m_pDown;		// ���°�ť
	INT						m_rightEdge;	// �������ұ�С����
	INT						m_interPixel;	// ���ֵ���ť�������
	static CSkinMenuBar*	_pInstance;		// ���浱ǰʵ�������ڻص����̴���
	HHOOK					m_hMsgHook;		// �˵����õ��Ĳ˵�����
	CSkinColorMenu*			m_pColorMenu;	// colormenuʵ��
	CString					m_strStyle;		// �˵����˵��ķ��
	HWND					m_hParent;		// sysBtns���
	HWND					m_hMainWnd;		// �����ھ�������ڴ���˵���Ϣ
	HMENU					m_curHmenu;		// ��ǰ�˵����
	UINT					m_lastItemID;	// �˵���ID
	UINT					m_nextbtnId;	// ��һ��ѡ�еĲ˵���ťid���������Ҽ�
	BOOL					m_bHiliteFirst;	// �Ƿ������һ����
	CPoint					m_cpMousePos;	// ��¼�˵����������λ��
};

/*
**     ������CSkinMenuButton
** ����˵�����˵���ť����갴�º󵯳��˵����ܹ�����ͨ��normal����
**           ������hover�������£�down���������ã�disable������״̬��
**           ���а���״̬һֱ���˵����߻���ʧ��֮��ָ���ͨ״̬��
*/
class CSkinMenuButton : public CSkinButton_Text
{
public:
	CSkinMenuButton();
	virtual ~CSkinMenuButton(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Paint(Graphics *pGraph);
	BOOL OnMouseMoveIn(short x, short y);
	BOOL OnMouseDown(short x, short y);
	BOOL OnMouseMoveOut();
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
public:
	UINT			m_btnId;
	COLORREF		m_normCol;
	COLORREF		m_overCol;
	COLORREF		m_downCol;
	COLORREF		m_disaCol;
	CSkinMenuBar	*m_pMenuBar;
};

/*
**     ������CSkinVolumeCtrls
** ����˵���������ؼ�������������ť���ֱ��ǡ���������
**           ��ȡ����������һ���������鰴ť����ť��϶
**           ��ͨ�������ļ����������������ť�Ĳ��֣�
**           ��Ϣ���ݵȹ��ܡ�
*/
class CSkinVolumeCtrls : public CSkinCtrl, public IQvodCallBack
{
public:
	CSkinVolumeCtrls();
	virtual ~CSkinVolumeCtrls(){};
	virtual void Clear_();
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	BOOL Layout(short nWidth, short nHeight);
	BOOL OnTimer();
	BOOL OnKey(WORD nChar);
	void ManualPaint(int btnId);
	void Paint(Graphics *pGraph);
	BOOL CheckCursor(short x, short y);
	BOOL OnMouseMove(short x, short y);
	BOOL OnMouseMoveIn(short x, short y);
	BOOL OnMouseMoveOut();
	BOOL OnMouseDrag(short x, short y);
	BOOL OnMouseUp(short x, short y);
	BOOL OnMouseDown(short x, short y);
	void SetState(skin_state state);
	Bitmap* GetImage(int pos);
	ISkin* GetSkinObj(short x, short y);
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);
public:
	enum volum_img
	{
		volum_thrumpet0,
		volum_thrumpet1,		
		volum_thrumpet2,	
		volum_thrumpet3,		
		volum_thrumpet_mute	
	};

	Bitmap  *m_vBmp[5];
	int						m_numImg;		// ͼƬ��
	ISkin*					m_pOver;		// ������ť
	ISkin*					m_pDown;		// ���°�ť
	ISkin*					m_pVolume;		// �����ϸ�
	ISkin*					m_pMute;		// ������ť
	ISkin*					m_pUnMute;		// ȡ��������ť
	CSkinWindow*			m_pWnd;			// ��������
	BOOL					m_bVolumeShow;	// ���������Ƿ�������ʾ
	int						m_nWndOffsetX;
	int						m_nWndOffsetY;
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�
* ������ڣ�
********************/
class CSkinButton_Check : public CSkinButton_Text //check��ť
{
public:
	CSkinButton_Check();
	virtual ~CSkinButton_Check(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Paint(Graphics *pGraph);
	BOOL CheckCursor(short x, short y);
	BOOL OnMouseUp(short x, short y);
	long Method_(LPCTSTR szAttrib, long para1, long para2); //�����������Ա��������check״̬��para1--0,1
	BOOL m_bCheck;				//check��ť��Ĭ��״̬
	BOOL m_bHalfCheck;			//��ѡ״̬
	BOOL m_bAdaptWords;		//�Ƿ���Ӧ���ֿ��
	UINT m_nTxtWidth;
	int m_childNum;
	int m_checkNum;
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ���־��
* ������ڣ�2013.12.10
* ����˵������ѡ��ť����ͨ��������TabFrameʵ���Զ����ϡ�
********************/
class CSkinRadioBox : public CSkinButton_Check
{
public:
	CSkinRadioBox();
	virtual ~CSkinRadioBox(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	BOOL OnMouseUp(short x, short y);
	long Method_(LPCTSTR szAttrib, long para1, long para2); //�����������Ա��������check״̬��para1--0,1

private:
	BOOL m_bTabMsg ;
	BOOL m_bVip ;
};


/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�
* ������ڣ�
********************/
class CSkinTableImage: public CSkinCtrl
{
public:
	CSkinTableImage();
	virtual ~CSkinTableImage(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Clear();
	void Paint(Graphics *pGraph);

	UINT m_nSrcx;			//ͼƬ���ϵ�X����
	UINT m_nSrcy;			//ͼƬ���ϵ�Y����
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�
* ������ڣ�
********************//*
class CSkinTable : public CSkinCtrl
{
public:
	CSkinTable();
	virtual ~CSkinTable()
	{
		con<<"Destruct CSkinTable\r\n";
	};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Paint(Graphics *pGraph);
	//	BOOL CheckCursor(short x, short y);
	//	BOOL OnMouseDown(short x, short y);
	//	BOOL OnMouseUp(short x, short y);
	long Method_(LPCTSTR szAttrib, long para1, long para2);//para1--�±꣬ para2: 0--up, 1--down
	virtual BOOL OnTimer();
	UINT	m_nDownIndex;		//���±�ǩ���±�
	int		m_nIndex;			//��ǰ��ǩ���±�
	int		m_nPreIndex;		//���л���ǩ���±�
	UINT	m_nSonWidth;		//�ӱ�ǩ�Ŀ��
	UINT	m_nSonHeight;		//�ӱ�ǩ�ĸ߶�
	BOOL	m_bVerical;		//�Ƿ��Ǵ�ֱ����
	BOOL	m_bHorizontal;		//�Ƿ���ˮƽ����
	BOOL	m_bBkMove;		//�����Ƿ��ƶ�
	UINT	m_nCount;		//����ͼ�ƶ���λ��
	COLORREF m_crBord;		//�߿���ɫ
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�
* ������ڣ�
********************//*
class CSkinTableItem: public CSkinButton
{
public:
	CSkinTableItem();
	virtual ~CSkinTableItem();
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL OnMouseDown(short x, short y);
	virtual BOOL OnMouseUp(short x, short y);
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�
* ����˵����CSkinTabBtns������ʵ�ֶ����ť�����л�ʱ����̬Ч��
*           
********************/
class CSkinTabBtns: public CSkinCtrl
{
public:
	CSkinTabBtns();
	virtual ~CSkinTabBtns();
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL OnTimer();
	BOOL OnMouseDown(short x, short y);
	BOOL OnMouseUp(short x, short y);
	BOOL OnMouseMoveIn(short x, short y);
	BOOL OnMouseMoveOut();
	BOOL OnMouseMove(short x, short y);
	virtual BOOL OnMouseDrag(short x, short y);								//��괦����Ϣ-�϶�
	int  CheckObj(short x, short y);
	BOOL CheckCursor(short x, short y);
	void Paint(Graphics *pGraph);

	void SetChangeColor(BOOL bChange);
	void SetFocusColor();
	void SetItem(int nitem);
	// �Ƿ���ק
	BOOL GetbDrag() ;

private:
	BOOL m_bHorizontal;	//�Ƿ���ˮƽ�ƶ�
	int  m_nOffsetper;	//ÿ���ƶ���ƫ����
	int  m_nOffset;		// �ܹ���ƫ����
	int	 m_ntotal;		//�����ˮƽ�ƶ�����ô�����ܿ��������ܸߡ�
	DWORD m_dfocusColor;
	DWORD m_dnormalColor;
	ISkin *m_pSkin;
	ISkin *m_pHotSkin;
	int m_nCurItem;	//��ǰ���µ��ӿؼ���
	int   m_nOldItem;	//֮ǰ���µ��ӿؼ���
	BOOL  m_nOutSetItem;	//�ⲿ����
	BOOL  m_bChangeColor;//��Ҫ�޸���ɫͨ��
	BOOL m_bDrag;
	BOOL m_bMouseDnChangeItem ;
};


/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�
* ����˵����Edit�е�����(����)����(����)��ť
*           
********************/
class CUpDownCtrlEx: public CUpDownCtrl, public CSkinCtrl
{
public:
	CUpDownCtrlEx();
	virtual ~CUpDownCtrlEx();
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual void Clear_();

	void ReSetProc();
	static LRESULT UpDownProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);		//�������໯������
	void OnPaint(HWND hWnd);

	BOOL			m_bFocus;			//�Ƿ�Ϊfocus״̬
private:
	WNDPROC			m_UpDownOldProc;

	Bitmap *		m_pBtImg;			//��ťͼ��
	COLORREF		m_clrNormalFrame;	//normal�߿���ɫ						
	COLORREF		m_clrHoverFrame;	//hover�߿���ɫ		
	COLORREF		m_clrBK;			//������ɫ

	int				m_nHeight;
	int				m_nWidth;
	BOOL			m_bUpOver;
	BOOL			m_bDownOver;
	BOOL			m_bUpDown;
	BOOL			m_bDownDown;
	HDC				m_memDC;										//��ͼʱ�õ��ڴ�DC
	HBITMAP			m_memBmp;										//��ͼʱ�õ��ڴ�λͼ
};

//////////////////////////////////////////////////////////////////////////////////////
//
// ��ǰ�汾��1.0
// ��    �ߣ���ΰ��
// ������ڣ�
// ����˵����HotKey�Ի�ؼ�
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinHotKey : public CWindowImpl <CSkinHotKey, CHotKeyCtrl>, public CSkinCtrl
{

public:
	BEGIN_MSG_MAP(CSkinHotKey)
	END_MSG_MAP()


	CSkinHotKey();
	virtual ~CSkinHotKey();
	
	//
	//�ⲿ�ӿڹ�����
	//
	void SetModHotKey(BYTE fVirt, WORD key);											//����Mod�ȼ�
	void GetModHotKey(BYTE & fVirt, WORD & key);										//��ȡMod�ȼ�
	void GetAccelKey(BYTE & fVirt, WORD & key);											//����Ϊ���ص�fVirt������ϼ�ֵ, key����ֵ --�ο��ṹ:ACCEL
	void SetAccelKey(BYTE fVirt, WORD key);												//����Ϊ: fVirt������ϼ�ֵ(FALT|FCONTROL|FSHIFT), key����ֵ --�ο��ṹ:ACCEL
	void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);								//HotKeyCtrl�ı�׼�ȼ����ýӿ�
	void GetHotKey(WORD & wVirtualKeyCode, WORD & wModifiers);							//��ȡ�ȼ�ֵ
	void SetPos(int x, int y);															//���ô���λ��
	void SetState(skin_state state);													//���ÿؼ�״̬

	void OnPaint(void);																	//�ؼ����ƺ���
	void EnterNofity(UINT uMsg, WPARAM wp);											    //�Իس���Ӧ����--֪ͨ������
	//
	//�ڲ�������
	//
protected:
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);				//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();																//����ʼ��
	virtual BOOL Layout(short nWidth, short nHeight);									//����չ�ֿؼ�--�ú����ɸı䴰�ڴ�Сλ�õ�ʱ�����WM_SIZE
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);

	static LRESULT SubClass_WndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);		//���໯���ڴ�����
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);									//��XML�ļ�����Ƥ��Ԫ�ص�����
	void     OnNcPaint(void);															//�ؼ�NC���ƺ���
	COLORREF Change2RGB(COLORREF clr);													//�ɴ�ͳ��ɫ˳��ARGBת��GBR�ֽ�λ˳��
	void	 SetHotKey_Fix_Work(void);													//�����ȼ���Ĵ�����
	CString  GetStringFromVk(UINT vk);													//��Vk����ֵ��ȡ��Ӧ���ַ����

protected:
	CString			m_strHotKey;									//�ȼ���ʾ������

	WNDPROC			m_OldWndPrc;									//�������໯ǰ�Ĵ��ڴ���
	HDC				m_memDC;										//��ͼʱ�õ��ڴ�DC
	HBITMAP			m_memBmp;										//��ͼʱ�õ��ڴ�λͼ
	CRect           m_oldMemRect;									//���ڱ�ʾ���ڴ�С(�ڴ�λͼ�������·���)
	HFONT			m_hTextFont;									//������ʽ���
	COLORREF		m_clrBK;										//������ɫ			
	COLORREF		m_clrNormalFrame;								//normal�߿���ɫ						
	COLORREF		m_clrHoverFrame;								//hover�߿���ɫ							
	COLORREF		m_clrNormalText;								//normal������ɫ						
	COLORREF		m_clrHoverText;									//hover������ɫ	
	COLORREF		m_clrHighLightBK;								//����ѡ�����ֱ�����ɫ	
	HBRUSH			m_hBrBK;										//������ɫ
	HBRUSH			m_hBrNormalFrame;								//normal�߿���ɫ
	HBRUSH			m_hBrHoverFrame;								//hover�߿���ɫ	
	HBRUSH			m_hBrNormalText;								//normal������ɫ
	HBRUSH			m_hBrHoverText;									//hover������ɫ		
	HBRUSH			m_hBrHighLightBK;								//hover������ɫ		
	HPEN			m_hFramePen;									//�߿�Ļ���
	HPEN			m_hFrameHoverPen;								//�߿�Ļ���
	BOOL			m_bFocus;										//�Ƿ�Focus״̬
	BOOL			m_bMouseDown;									//�Ƿ�Focus״̬
	BOOL			m_bEnterNotify;									//�Ƿ�Իس���Ӧ����--֪ͨ������
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ��ĳɴ�   �޸� by ��ΰ��
* ������ڣ�
********************/
//Ŀǰֻ�޸���Ĭ�ϱ�����Ĭ��������ɫ����ѡ�е�������ɫ��ѡ�б�����ɫ�������װ��֡�δ�����
class CSkinEdit : public CWindowImpl <CSkinEdit, CEdit>, public CSkinCtrl
//class CSkinEdit : public CRichEditCtrl, public CSkinCtrl
{			
public:
	DECLARE_WND_CLASS(_T("QVODEDITCTRL"))
	BEGIN_MSG_MAP(CSkinEdit)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
	END_MSG_MAP()
	CSkinEdit();
	virtual ~CSkinEdit();
	//
	//�ⲿ�ӿڹ�����
	//

	void SetPos(int x, int y);															//���ô���λ��
	BOOL IsUpDownStyle(void);															//�ж��Ƿ�upDown��ʽ
	void SetUpDownStepLen(int nStepLen);												//����UpDown�ؼ��Ĳ���
	int  GetEditStyle(void);															//��ȡEdit����ʽ���� 0 Text, 1 num ,2 time, 3 updown
	void SetState(skin_state state);													//���ÿؼ�״̬
	void SetTextFont(const LOGFONT & lf);												//����������ʽ
	HFONT GetTextFont(void);															//��ȡ������ʽ
	void SetUpDownNofityFlag(bool bAuto);												//������SetWindowTextʱ�Ƿ�ص�֪ͨ
	bool GetUpDownNofityFlag(void);														//��ȡ��SetWindowTextʱ�Ƿ�ص�֪ͨ
	BOOL Create_SimpleMode(HWND hParentWnd, RECT & rtCtrl,DWORD dwStyle, 
						   IQvodCallBack *pEventEntry,DWORD dwExStyle=0);									//�����ؼ�  -ֻ�޸ļ򵥵���Ϣ�ص�֪ͨ������
	void SetText(LPCTSTR lpText);
	CUpDownCtrlEx	m_UpDownCtrl;
	
	void SetBkColor(DWORD dwARGB);

	//unDown�ؼ�
	//
	//�ڲ�������
	//	
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	void LegalNumber();
	virtual BOOL Layout(short nWidth, short nHeight);									//����չ�ֿؼ�--�ú����ɸı䴰�ڴ�Сλ�õ�ʱ�����WM_SIZE
protected:
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);				//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();



	static LRESULT SubClass_WndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);		   //���໯���ڴ�����
	static LRESULT SimPleMode_SubClass_WndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);//�������໯������--��ģʽ
	void	 LoadSkinConfigFromXML(CXMLElement &xmlNode);								//��XML�ļ�����Ƥ��Ԫ�ص�����
	LRESULT  OnChar(UINT nMsgType, WPARAM wParam, LPARAM lParam, BOOL& bHandled);		//�������봦����
	void     OnPaint(void);																//�ؼ����ƺ���
	void     OnNcPaint(void);															//�ؼ�NC���ƺ���
	COLORREF Change2RGB(COLORREF clr);													//�ɴ�ͳ��ɫ˳��ARGBת��GBR�ֽ�λ˳��
	void	 ArrowLeft_Work(void);														//���ͷ��������	--����WTL��װ��CEditû�жԼ�ͷ����,������ʵ��
	void	 ArrowRight_Work(void);														//�Ҽ�ͷ��������	--����WTL��װ��CEditû�жԼ�ͷ����,������ʵ��
	BOOL	 EnterNofity(UINT uMsg, WPARAM wp,LPARAM lp);								//�Իس���Ӧ����--֪ͨ������
	void	 DealWithDelete(WPARAM wParam, LPARAM lParam, BOOL& bHandled);				//����ʱ��״̬ʱ��back �� delete��Ϣ
	void	 DealTimeCaret(WPARAM wParam, BOOL& bHandled);								//����ʱ��״̬ʱ�����
	void	 LegalTime(WPARAM wParam, LPARAM lParam, BOOL& bHandled);					//ʱ���Ƿ�Ϸ�
	void	 DealLButtonDown(LPARAM lParam);											//ʱ��״̬ʱ�����

protected:
	WNDPROC			m_OldWndPrc;									//�������໯ǰ�Ĵ��ڴ���
	HDC				m_memDC;										//��ͼʱ�õ��ڴ�DC
	HBITMAP			m_memBmp;										//��ͼʱ�õ��ڴ�λͼ
	CRect           m_oldMemRect;									//���ڱ�ʾ���ڴ�С(�ڴ�λͼ�������·���)

	HFONT			m_hTextFont;									//������ʽ���
	COLORREF		m_clrBK;										//������ɫ			
	COLORREF		m_clrNormalFrame;								//normal�߿���ɫ						
	COLORREF		m_clrHoverFrame;								//hover�߿���ɫ							
	COLORREF		m_clrNormalText;								//normal������ɫ						
	COLORREF		m_clrHoverText;									//hover������ɫ	
	COLORREF		m_clrHighLightBK;								//����ѡ�����ֱ�����ɫ	
	HBRUSH			m_hBrBK;										//������ɫ
	HBRUSH			m_hBrNormalFrame;								//normal�߿���ɫ
	HBRUSH			m_hBrHoverFrame;								//hover�߿���ɫ	
	HBRUSH			m_hBrNormalText;								//normal������ɫ
	HBRUSH			m_hBrHoverText;									//hover������ɫ		
	HBRUSH			m_hBrHighLightBK;								//hover������ɫ		
	HPEN			m_hFramePen;									//�߿�Ļ���
	HPEN			m_hFrameHoverPen;								//�߿�Ļ���
	int             m_nStyle;										//edit����ʽ
	int				m_nMaxLen;										//�ַ��������������
	int				m_nInterval;									//unDown�ؼ���ÿ�������Ĳ���(һ��������)
	int				m_nNumRangeMin;									//unDown�ؼ��ķ�Χ��Сֵ
	int				m_nNumRangeMax;									//unDown�ؼ��ķ�Χ���ֵ
	int				m_nUpdownCtrlPos;								//unDown�ؼ���ֵ
	int				m_nAuto;										//�ؼ���Auto����
	BOOL			m_bBorder;										//�Ƿ��б߿�
	BOOL			m_bFocus;										//�Ƿ�Focus״̬
	// �Ƿ�������븺��
	BOOL            m_bExistNegativesign;
	BOOL			m_bMouseDown;									//�Ƿ�Focus״̬
	BOOL			m_bEnterNotify;									//�Ƿ�Իس���Ӧ����--֪ͨ������
	bool			m_bUpDownNofity;								//�Ƿ�����updown����edit���ݻص�
	CString			m_strText;
	int				m_nOldNum;
	BOOL			m_bLocked;
};


class CSkinLoginEdit : public CSkinEdit
{
public:
	CSkinLoginEdit();
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	
	virtual BOOL Layout(short nWidth, short nHeight);
	virtual void Paint(Graphics *pGraph);
	virtual BOOL OnTimer();
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	CString GetNewText(TCHAR);
	virtual void SetEditShowText(CString strShowText);


protected:
	static LRESULT WndProc_LoginEdit(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);
	void DrawDefText();
public:
	BOOL				m_bFilterSpace;			// ���˿ո�
	ISkin*				m_pOverText;
	HWND				m_hOwn;
	BOOL				m_bChildWnd;			//�̳и���������
	BOOL				m_bIsChild;		// �Ӵ���
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�
* ����˵���������������б����е�һ���ؼ�������ʾ�������е�
*           �ļ����ͣ��Լ��ļ����صĽ��ȡ������ļ����ء�
********************//*
class CSkinProgress : public CSkinCtrl	// ������
{
public:
	CSkinProgress();
	virtual ~CSkinProgress();

	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);//{return FALSE;}
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL OnTimer();
	void SetFile(TCHAR* file);
	BOOL DrawFileLogo();
	void Paint(Graphics *pGraph);
	void Clear();
	void SetPos(float hasDown);

	enum Pos_Img
	{
		pro_left = 0,
		pro_mid = 1, 
		pro_has = 2,
		pro_right = 3, 
		pro_hasfinished
	};

	float  m_progress; //progress
	Bitmap  *m_vBmp[5]; //0, pro-left; 1, pro-mid; 2, pro-yes; 3, pro-right;
	int m_proX;	//��������ʾ��Xλ��
	int m_proY;	//��������ʾ��Yλ��

	int m_proBmpW; //���������ܿ��
	int m_imgsW; //��������ͼ���ܿ��
	int m_hasimgW; //�����ص�ͼƬ�Ŀ��
	Rect *m_pProRect;
	TCHAR m_fileBuff[255];
	HICON m_hico;
	int m_logoH; //ϵͳ�ļ�ICON�ĸ߶� 
	int m_logoW; //ϵͳ�ļ�ICON�Ŀ��
	BOOL m_isFinished;

	BOOL m_stopover; //�������ͣ/ȡ����ť�ϡ�
};*/

class CSkinProgressBar : public CSkinCtrl	// ������
{
public:
	CSkinProgressBar();
	virtual ~CSkinProgressBar();
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);//{return FALSE;}
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL OnTimer();
	void Paint(Graphics *pGraph);
	void SetPos(int pos);
	int GetPos();
	void Clear();
	void SetState(skin_state state);

	int   m_pos; //progress
	Bitmap  *m_vBmp[4];

	BOOL  m_bWaterStyle;		//�Ƿ�ˮ�����ͽ���
	BOOL  m_bHProgress;		    //�Ƿ�ˮƽˮ������
};

/****
*
*�˽ṹ��Ҫ��Ϊ�˲����ļ��������ضΡ�
*
************/
struct _TIMEPOS
{	
	float beginPos;
	float endPos;
	struct _TIMEPOS *NextNode;
};

// CSkinSlider��Ϣ
enum SliderMsg
{
	slider_msg_out, 
	slider_msg_in,
	slider_msg_down,
	slider_msg_up,
	slider_msg_drag,
	slider_msg_noncanseek,
};


/******************
* �ؼ�����: CSkinSlider,������/�ϸ�
* ��ǰ�汾��1.01
* ��    �ߣ�������, �޸� ��ΰ��
* ������ڣ�
* ����˵������������ʾ�Ϳ�����Ƶ�ļ����ų̶ȵĿؼ���
********************/
class CSkinSlider : public CSkinCtrl	// �ϸ�
{
	//
	//�ⲿ�ӿڹ�����
	//
public:
	void  SetPos(float pos);													//���ò���λ�ã�pos�ǰٷֱ�
	float GetPos();																//��ȡ����λ�ã�pos�ǰٷֱ�
	void  ClearPos();															//���ò���λ��
	void  SetCanDrag(BOOL bCanDrag);											//���ÿؼ��Ƿ�����϶�
	float GetCurrPos();		
	void  SetAlpha(float alpha);					// ����͸����

public :

	//
	//ѭ�����Ź�����
	//
	//˵��:����float���;��Ǳ�ʶλ�ý���,���ֵΪ����1.0

	//�β��ź�������������
	typedef void (*SecPlayPrc)(float);
	void SetSecPlayCallBackFun(SecPlayPrc  pCallBackFun);							//���öβ��Ų���Խ��ʱ��Ļص�����

#define SLIDER_SEC_PLAY_BORDER_NUM                            2                     //ѭ�����Ŷε�2����
#define SLIDER_SEC_PLAY_POINT_S                               0						//ѭ�����Ŷε����
#define SLIDER_SEC_PLAY_POINT_E                               1						//ѭ�����Ŷε��յ�
#define SLIDER_SEC_PLAY_INVALID                               -1.0					//ѭ�����Ŷ�λ����Чֵ



	//
	//�ڲ�ʵ�ֹ�����
	//
public:
	CSkinSlider();
	virtual ~CSkinSlider();

	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);				//��ʼ��

	virtual BOOL OnTimer();														//��ʱ��������
	virtual BOOL Layout(short nWidth, short nHeight);							//����չ�ֿؼ�

	virtual BOOL OnMouseMoveIn(short x, short y);								//��괦����Ϣ-�ƽ��ؼ�
	virtual BOOL OnMouseMoveOut();												//��괦����Ϣ-�Ƴ��ؼ�
	virtual BOOL OnMouseDown(short x, short y);								    //��괦����Ϣ-����
	virtual BOOL OnMouseUp(short x, short y);									//��괦����Ϣ-�ͷ�
	virtual BOOL OnMouseDrag(short x, short y);									//��괦����Ϣ-�϶�
	virtual BOOL OnMouseMove(short x, short y);									//��괦����Ϣ-�ƶ�

	virtual BOOL CheckCursor(short x, short y);									//���(x,y)�Ƿ��ڿؼ���
	virtual BOOL CheckBlockCursor(short x, short y);							//���(x,y)�Ƿ���Block��

	void Paint(Graphics *pGraph);												//���ƿؼ�
	void Clear_();																//������Դ �ͷſؼ���Դ
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);							//��XML�ļ�����Ƥ��Ԫ�ص�����
	ISkin * GetSkinObj(short x, short y);

	SecPlayPrc  m_pCallBackFun;													//�β��Żص�����ָ��
	int	  m_bSetPlayCheckTimer;													//�Ƿ������β��ż�ⶨʱ��

	//����ͼƬ����������λ��
	enum Pos_Img
	{
		slider_block      = 0,
		slider_bar,		
		slider_bar_left,
		slider_bar_right,
		slider_head, 
		slider_hasload,
		slider_hasplay,
		slider_ab_bg,
		slider_ab_side,
		slider_hasplay_left,
		slider_hasplay_right,
	};

//����������갴��ʱ���ƶ�����ʽ
#define SLIDER_MOVING_STYLE_DIRECT_MOVE                  0							//ֱ����ת�������λ��				
#define SLIDER_MOVING_STYLE_STEP_MOVE                    1							//�𲽿���������ƶ�λ��

	float m_fStepMoveLen;                                                       //���ƶ���ʽ--ÿ���ƶ��Ĳ��� MAX=100
	int m_nBlockMoveStyle;                                                      //����������갴��ʱ���ƶ�����ʽ  

	Bitmap  *m_vBmp[11];														//�洢����״̬�Ļ���λͼ
	float m_fPos;																//��Ƶ���ŵİٷֱ�
	BOOL m_moveIn;																//������Slider
	BOOL m_blockOver;															//�����Block��
	BOOL m_downBlockOver;														//��갴����Block��
	BOOL m_blockDown;															//������Block����
	BOOL m_flagBlock;															//�����Block��
	BOOL m_bDrag;																//��ק״̬
//	struct _TIMEPOS *m_structTime;												//ʱ��ṹָ��
	int m_NodeNum;																//�ڵ�Ԫ����
	int m_validWidth;															//�����Ŀ��
	int m_validHeight;															//�����ĸ߶�
	int m_invalidHeadLen;                                                       // ��������β����������䳤��
	BOOL m_bVertical;															//�ؼ��ڷ�����-��ֱ/ˮƽ
	DWORD m_filltype;															//�м����������컹��ֱ����ͼ
	int m_tileddistance;														//ֱ����ͼ��������϶�������С�ı���
	int m_nImgNumber;															//ͼ����
	int m_nOldDistance;															//���Down��λ�ú�ʵ��Posλ�õĲ�ֵ��

	int m_nDownMove;															//���������л����λ�����м�λ�õ�ƫ��X/Yֵ
	int m_nOldBlockMidPos;														//���������л����λ��X/Yֵ
	BOOL m_bCanSeek;															//�Ƿ����϶�

	BOOL m_bDownInAdhereArea;													//�Ƿ���ʱ��������
	float m_fCurrPos;															//��굱ǰ���ڵ�λ��
	float m_fAlpha;			// ͸����ֵ
	BOOL  m_nAutoHide;		// 0:�����أ�1:�Զ����أ�2:������Ч�����Զ�����
	int	  m_nBgOffX;
	int	  m_nBgOffY;
	BOOL  m_bShowBlock;

	// ��ؼ�
	ISkin *m_pSkinOver;
	ISkin *m_pSkinDown;
};


/******************
* �ؼ�����: CSkinCutSlider, ��Ƶ��ȡ������
* ��ǰ�汾��1.01
* ��    �ߣ�������
* ������ڣ�
* ����˵������������ʾ�Ϳ�����Ƶ��ȡ���ų̶ȵĿؼ���
********************/
class CSkinCutSlider : public CSkinCtrl	// �ϸ�
{
public:				//�ⲿ�ӿڹ�����
	void  SetBeginPos(float pos, BOOL bSetMsg);			// ������ʼλ��
	float GetBeginPos();					// ��ȡ��ʼλ��
	void  SetEndPos(float pos, BOOL bSetMsg);				// ������ֹλ��
	float GetEndPos();						// ��ȡ��ֹλ��
	void  SetPreViewPos(float pos);			// ����Ԥ��λ��
	void  StartPreView(BOOL bStart);			// ֹͣԤ��

public:
	CSkinCutSlider();
	virtual ~CSkinCutSlider();

	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);				//��ʼ��
	void Clear_();																//������Դ �ͷſؼ���Դ

	void Paint(Graphics *pGraph);												//���ƿؼ�
	BOOL OnTimer();																//��ʱ��������
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL CheckCursor(short x, short y);									//���(x,y)�Ƿ��ڿؼ���
	void CheckBlock(short x, short y);											// ���(x, y)�Ƿ������ҵĿ���
 
 	virtual BOOL OnMouseMoveIn(short x, short y);								//��괦����Ϣ-�ƽ��ؼ�
 	virtual BOOL OnMouseMoveOut();												//��괦����Ϣ-�Ƴ��ؼ�
	virtual BOOL OnMouseMove(short x, short y);									//��괦����Ϣ-�ƶ�
 	virtual BOOL OnMouseDown(short x, short y);								    //��괦����Ϣ-����
 	virtual BOOL OnMouseUp(short x, short y);									//��괦����Ϣ-�ͷ�
 	virtual BOOL OnMouseDrag(short x, short y);									//��괦����Ϣ-�϶�




	//����ͼƬ����������λ��
	enum Pos_Img
	{
		slider_bg      = 0,
		slider_leftblock,		
		slider_rightblock,
		slider_playblock
	};

	Bitmap  *m_vBmp[4];															//�洢����״̬�Ļ���λͼ
	float	m_fBeginPos;														// ��ߵ�λ��
	float   m_fEndPos;															// �ұߵ�λ��
	int		m_nBMove;															// ��߿�ʼ��λ��
	int		m_nEMove;															// �ұ߽�����λ��
	BOOL    m_bLOver;															// �Ƿ�����߿���
	BOOL	m_bLDown;															// �Ƿ�����ߵĿ�İ���
	BOOL	m_bROver;															// �Ƿ����ұ߿���
	BOOL	m_bRDown;															// �Ƿ����ұ߿��ϰ���
	BOOL	m_bThumbOver;														// �Ƿ��ڽ�������
	int		m_nOldX;															// �����
	float	m_fProPos;															// ��ȡ����
	BOOL	m_bNeedProgress;													// 

	BOOL    m_bPreview;															// ��ʼ����
//	float   m_fPlayPos;															// ���Ž��ȵ����λ��
	float   m_fAbPlayPos;														// ���Ž��ȵľ���λ��
	BOOL    m_bBlockOver;														// ���Ž��ȿ�
	BOOL    m_bBlockDown;														// ���Ž��ȿ鰴��


	float m_fStepMoveLen;                                                       //���ƶ���ʽ--ÿ���ƶ��Ĳ��� MAX=100
	int m_nBlockMoveStyle;                                                          //����������갴��ʱ���ƶ�����ʽ  

	
	float m_fPos;																//��Ƶ���ŵİٷֱ�
	BOOL m_moveIn;																//������Slider
	BOOL m_blockOver;															//�����Block��
	BOOL m_downBlockOver;														//��갴����Block��
	BOOL m_blockDown;															//������Block����
	BOOL m_flagBlock;															//�����Block��
	BOOL m_bDrag;																//��ק״̬
	int m_NodeNum;																//�ڵ�Ԫ����
	int m_validWidth;															//�����Ŀ��
	int m_validHeight;															//�����ĸ߶�
	BOOL m_bVertical;															//�ؼ��ڷ�����-��ֱ/ˮƽ
	int m_nImgNumber;															//ͼ����
	int m_nOldDistance;															//���Down��λ�ú�ʵ��Posλ�õĲ�ֵ��

	int m_nDownMove;															//���������л����λ�����м�λ�õ�ƫ��X/Yֵ
	int m_nOldBlockMidPos;														//���������л����λ��X/Yֵ
	BOOL m_bCanSeek;															//�Ƿ����϶�

	BOOL m_bDownInAdhereArea;													//�Ƿ���ʱ��������
	float m_fCurrPos;															//��굱ǰ���ڵ�λ��
};

/******************
* �ؼ�����: ����������
* ��ǰ�汾��1.01
* ��    �ߣ���ΰ��
* ������ڣ�
* ����˵�����϶�ʱ�ӽ��м���Զ��������е�Ļ�����
********************/
class CSkinAdhereSlider:public CSkinSlider
{

	//
	//�ⲿ���ܽӿ���
	//
public:
	void SetAdhereRange(int nLowProcent, int nHightProcent);					//��������������Χ -����Ϊ�ٷֱ�λ��
	void GetAdhereRange(int & nLowProcent, int & nHightProcent);				//��ȡ����������Χ -����Ϊ�ٷֱ�λ��



	//
	//�ڲ�����ʵ����
	//
public:
	CSkinAdhereSlider();
	virtual ~CSkinAdhereSlider();


	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);		//��ʼ��
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);							//��XML�ļ�����Ƥ��Ԫ�ص�����


	virtual BOOL OnMouseDrag(short x, short y);									//��괦����Ϣ-�϶�
	virtual BOOL OnMouseDown(short x, short y);								    //��괦����Ϣ-����
	virtual BOOL OnMouseUp(short x, short y);									//��괦����Ϣ-�ͷ�


	float m_nLowProcent;														//����������Сֵ
	float m_nHightProcent;														//�����������ֵ
	float m_fOldSAdherePos;														//��¼����ǰ��λ��
};

/******************
* �ؼ�����: CSkinSlider,������/�ϸ�
* ��ǰ�汾��1.01
* ��    �ߣ�������
* ������ڣ�
* ����˵������������ʾ�Ϳ�����Ƶ�ļ����ų̶ȵĿؼ���
********************/
class CSkinPlaySlider:public CSkinSlider
{
	//
	//�ⲿ���ܽӿ���
	//
public:
	BOOL OnMouseMove(short x, short y);									            //��괦����Ϣ-�ƶ�
	BOOL OnMouseMoveOut(void);												        //��괦����Ϣ-�Ƴ��ؼ�
	void  SetSectionPoint_S(float fPos);											//���ö��䲥�����
	void  SetSectionPoint_E(float fPos);											//���ö��䲥���յ�
	float GetSectionPoint_S();														//��ȡ���䲥�����
	float GetSectionPoint_E();														//��ȡ���䲥���յ�

	BOOL  SectionPlay_Start();														//��ʼѭ�����Ŷ�
	BOOL  SectionPlay_Stop();														//ȡ��ѭ�����Ŷ�
	BOOL  IsSectionPlayNow();														//�жϵ�ǰʩ��ѭ������״̬

	long GetValideWidth();															//��ȡ��Ч������
	void SetLoadInfo(char *pBitInfo, int npicNum, int nBuffer, int nIndexBegin, int nIdexLen);		// ����������Ϣ ֻ������ݵĸ���
	void ClearLoadInfo();															//���������Ϣ
	void DrawBitBmp();																//���������ص�
	bool DrawVTBmp(Graphics & gra);													// ������Ƶ��ǩ�ı��

	//
	//�ڲ�����ʵ����
	//
public:
	CSkinPlaySlider();
	virtual ~CSkinPlaySlider();

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);			//��ʼ��
	virtual void Clear_();															//����ʼ��-������Դ
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);					//����Com���ͽӿ�
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);								//��XML�ļ�����Ƥ��Ԫ�ص�����
	void Paint(Graphics *pGraph);													//���ƿؼ�
	int  GetMiniH();
public:
	bool m_bSurportQmv;																// ��ǰ�������費��Ҫ֧��QMV

protected:
	BOOL CheckIndex(int index);														//��ѯ��Ƭ�Ƿ�������

	int m_nMiniH;
	float m_fStartPoint;															//ѭ�����Ŷ����λ��
	float m_fEndPoint;																//ѭ�����Ŷ��յ�λ��
	BOOL  m_bSectionPlay;															//��ǰ�Ƿ�����ѭ��������
	BOOL  m_bSectionPlayStyle;														//�Ƿ�ӵ��ѭ��������ʽ	
	BOOL  m_bHi;																	//��ǰ�Ƿ�Ϊ����(��)״̬

	char *m_pOldBitInfo;			// Bit λ��һ�η��͵���Ϣ
	char *m_pNewBitInfo;			// Bit λ���µ�һ�η��͵���Ϣ
	int	  m_nPicNum;				// Ƭ��
	int	  m_nBuffer;				// m_pNewBitInfo�Ĵ�С
	Bitmap *m_pLoadBmp;				// ���ؽ���ͼ
	BOOL  m_bBeginLoad;				// ��һ�ο�ʼ����Bitλ��
	int	  m_nBeginPic;				// �ļ�������ʼ
	int   m_nIndexLen;				// ��������

	CRITICAL_SECTION m_csBitInfo;	// ��Ϊ���ƺ͸���Bitλ��Ϣ��������ͬ���̲߳���m_pNewBitInfo.���Լ�����
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�
* �޸�����: 2010.3.31
* ����˵������������ʾ�Ϳ��������Ŀؼ���
********************
class CSkinVolume:public CSkinCtrl
{
public:
	CSkinVolume();
	virtual ~CSkinVolume();
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL OnTimer();

	virtual BOOL OnMouseMoveIn(short x, short y);
	virtual BOOL OnMouseMoveOut();
	virtual BOOL OnMouseDown(short x, short y);	// ��갴��
	virtual BOOL OnMouseUp(short x, short y);		// ��갴��
	virtual BOOL OnMouseDrag(short x, short y);	
	virtual BOOL OnMouseMove(short x, short y);
	virtual BOOL CheckCursor(short x, short y);
	virtual BOOL Layout(short nWidth, short nHeight);

	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Paint(Graphics *pGraph);
	void Clear();
	BOOL CheckBlockCursor(short x, short y);	//�������Ƿ����п���
	BOOL CheckThrumpet(short x, short y);		// ���С����
	BOOL CheckDragBar(short x, short y);		// ����ƶ���
	void SetPos(int pos);						// ����λ��
	int GetPos();								// ��ȡλ��
	void SetMute(BOOL isMute);					// ���þ���
	Bitmap * GetImg(int pos);					// ��ȡ��������Ӧ������
	void SetAlpha(float alpha);					// ����͸����

	enum volum_img
	{
		volum_block			= 0,
		volum_leftimg		= 1,
		volum_midimg		= 2,
		volum_rightimg		= 3,
		volum_hasplayimg	= 4,
		volum_thrumpet0     = 5,
		volum_thrumpet1		= 6,
		volum_thrumpet2		= 7,
		volum_thrumpet3		= 8,
		volum_thrumpet_mute	= 9
	};

	int m_vPos;				//�����Ĵ�С��Χ��(0-100)
	Bitmap  *m_vBmp[10];
	int m_dowPos;			// ��갴��ʱ�����п�����x���λ�á�
	int m_numImg;			// ͼƬ��
	BOOL m_hasBtn;			// �Ƿ���Btn
	BOOL m_mute;			// ����
	int m_validLen;			// ���϶�����Ч���ȡ�
	int m_bX;				//�϶����Ŀ�ʼλ�á�
	BOOL m_onBtn;			//�����С������
	float m_fAlpha;			// ͸����ֵ
	BOOL  m_bAutoHide;		// �Զ�����
	BOOL  m_bVertical;      // ��ֱ/ˮƽ


};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�
* ����˵���������ǹ������ؼ���
********************/
class CSkinScroll : public CSkinCtrl	// ������
{
public:
	CSkinScroll();
	virtual ~CSkinScroll();

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();													//����ʼ��
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//�����ӿ�--����COM��ѯ�ӿ�
	virtual BOOL OnTimer();													//�Զ����ö�ʱ��--�鿴�Ƿ���Ҫ�ػ�

	virtual BOOL OnMouseMoveIn(short x, short y);			//����ƽ�
	virtual BOOL OnMouseMoveOut();							//����Ƴ�
	virtual BOOL OnMouseMove(short x, short y);				//����ƶ�
	virtual BOOL OnMouseDown(short x, short y);				//��갴��
	virtual BOOL OnMouseUp(short x, short y);				//����ͷ�
	virtual BOOL OnMouseDrag(short x, short y);				//����϶�
	virtual BOOL CheckCursor(short x, short y);				//���Թ��


	void SetVertical(BOOL bVertical);						//���ô�ֱ/ˮƽ��ʽ-Ĭ��������Ǵ�ֱ�Ĺ��������������bVerticalΪFALSE���������Ϊˮƽ
	void SetPageHeight(long nPageHeight);					//������ʾ�Ľ���߶�(�û����Կ��õ���)
	void SetDuration(long dur);								//��Ҫ��ʾ�Ľ�����ܸ�
	void SetPos(float pos);									//���ù������ƶ���λ��
	void SetPosDirect(float pos);
	void SetBtnDownScrollPixel(UINT pixel);					//���ð�ť����һ����Ҫ����������ֵ
	//��ȡ�������ƶ���λ�ðٷֱ�
	float GetPos();
	float GetControlMove();
	void Paint(Graphics *pGraph);							//���ƿؼ�

	//ˮƽ������
	void SetPageWidth(long nPageWidth);						//������ʾ����Ŀ��
	void SetEventEntry(IQvodCallBack *pEventEntry);			//���ûص����
	int  GetWitdh(void);									//��ȡ�������Ŀ��(ͼ��)

protected:
	BOOL CheckThumb(const short x, const short y);			//��������Ƿ��ڻ�������
	BOOL CheckUpBtn(const short x, const short y);			//��������Ƿ���Up/Left��ť����
	BOOL CheckDownBtn(const short x, const short y);		//��������Ƿ���Down/Right��ť����
	void UpdateScrollState(int x, int y);					//���¿ؼ�״̬
	int  UpdateBlockSideMove(short x, short y);				//��ȡָ�������뻬�����Ͻ������ƶ�����ƫ��

	// ���廬�����С�߶�
#define MinHeight_Slider		((m_bVertical ? m_nScrollBtnHeight : m_nScrollBtnWidth)*2)

	enum scroll_img
	{
		scroll_up = 0,			// ���ϰ�ť
		scroll_down,			// ���°�ť
		scroll_bg,				// ��ͼ
		scroll_roundtop,		// Բ�ǵĶ���
		scroll_roundmid,		// �ƶ���ĵ�ͼ
		scroll_midblock,		// �ƶ����м�������ͼ
		SCROLL_IMG_NUM          // �ؼ�ͼ����-�����
	};

	Bitmap  *m_vBmp[SCROLL_IMG_NUM];							//����״̬�µ�λͼ
	Bitmap  *m_pMemBmp;											//�ڴ�λͼ
	BOOL	 m_bVertical;										//�Ƿ�Ϊ��ֱ����
	long	 m_pageHeight;										//ÿҳ��ʾ�ĸ߶�
	UINT	 m_btnDownScrollPixel;								//��ť����һ��ʱ�Ĺ�������ֵ
	long	 m_nAllHeight;										//scroll����ʾ���ܹ��ĸ߶�/���
	float	 m_fPos;											//��������λ��ƫ�Ʊ��� 
	float	 m_fDownPos;										//��¼��갴��ʱ��λ��
	int		 m_nWidth;											//�������Ŀ��
 	int		 m_nDownMoveUp;										//��갴��ʱ�뻬�鶥��/����Ե��ƫ��
 	int		 m_nDownMoveDown;									//��갴��ʱ�뻬��ײ�/�Ҳ��Ե��ƫ��
	int      m_nOldDragPos;                                     //��һ���϶�ǰ��λ��

	Rect	 m_paintRect;										//����ͼ��ľ�������-�ؼ���С

	int      m_ScrollState;										//��������ǰ״̬

	int      m_nScrollBtnWidth;									//��������ť���
	int      m_nScrollBtnHeight;								//��������ť�߶�
	int      m_nScrollBgWidth;									//�������������
	int      m_nScrollBgHeight;									//�����������߶�

	int      m_nScrollRoundTopWidth;							//��������ť���
	int      m_nScrollRoundTopHeight;							//��������ť�߶�
	int      m_nScrollRoundMidWidth;							//�������������
	int      m_nScrollRoundMidHeight;							//�����������߶�
	int      m_nScrollMidLockWidth;								//�������������
	int      m_nScrollMidLockHeight;							//�����������߶�

	int		 m_nCallPressBtCounter;								//��ʱ�������°�ť����������
};

//��������������ݽṹ
enum netstate
{
	ns_unknow = 0,
	ns_creating,
	ns_recieving,
	ns_pausing,
	ns_finished,
	ns_waitrecv,
	ns_noncreate,
};

enum net_quality
{
	nq_none = 0,
	nq_poor,
	nq_normal,
	nq_good,
	nq_verygood
};

//UI��������Ҫ����Ϣ
// typedef struct _LISTITEMINFO
// {
// 	long	nType;				// �ļ����� 0: ���� 1: ����
// 	long	nTaskID;			// ����ID
// 	long	nFileID;			// �ļ�ID
// 	TCHAR   szName[MAX_PATH];	// ��ʾ��
// 	netstate netState;			// ����״̬ ����1 ��ͣ2 ����3 ���4
// 	float	fProgress;			// ���ؽ��� 0 �� 100
// 	net_quality quality;		// ��������
// 	BOOL	bPlaying;			// �Ƿ����ڲ���
// 	// Bitmap* ��ͼָ��
// 	_LISTITEMINFO()
// 	{
// 		memset(this, 0, sizeof(_LISTITEMINFO));
// 	}
// }LISTITEMINFO;

// enum _NETBTN_TYPE
// {
// 	NBT_Finished = 1, //���������
// 	NBT_StopLoad,	//ֹͣ����
// 	NBT_StartLoad,	//��������
// 	NBT_Unknown
// };

// enum _CLICK_TYPE
// {
// 	ct_LClick = 1,
// 	ct_RClick,
// 	ct_DBClick,
// 	ct_Drag,
// 	ct_NetBtnClick//,	
// 	//	cf_SeedBtnClick
// };

// typedef struct _TLVCMESSAGE
// {
// 	_CLICK_TYPE clickType; //��Ϣ����
// 	int			item;		//�ڼ���������
// 	int			itemID;		//�������ID��
// 	netstate     itemnetsate; //����״̬
// 	_NETBTN_TYPE	netbtnState;		//�������ذ�ť��״̬
// 	_TLVCMESSAGE()
// 	{
// 		clickType = ct_LClick;
// 		item  = -1;
// 		itemID = 0;
// 	}
// 
// }TLVCMESSAGE;

//�ڵ�����
enum item_property
{
	ITEM_NORMAL = 0,   //����
	ITEM_FOLDER,	   //����չ�ڵ�
	ITEM_CHILD,		   //�ӽڵ�
	ITEM_DIRECTORY	   //Ŀ¼
};

#define MAX_ICON_COUNT  5
//��ý���б�item�ṹ
struct MUL_ITEM_INFO
{
	int       nID;				//�ڵ�Ԫ�صı��
	int		  nListIndex;		//�б����
	int		nSeriesIdx;		//�缯�ļ������
	CString   strName;			//��ʾ�ı�-����
	CString   strTime;			//��ʾ�ı�-ʱ��
	Image *   pImgBmp;			//ͼƬ����ͼ��
	int       nPgPecent;		//�������ٷ��� MAX=100
	BOOL      bSimpleShowTime;	//���ģʽ���Ƿ���ʾʱ��/����Ϊ��ʾ���� 
	int       nIconShowState;   //��ʾ���С����ͼ��״̬
	int       nState;			//��ǰ�ڵ��״̬
	BOOL	  bShowGrayPg;      //�Ƿ���ƻһ�������
	netstate  netState;			// ����״̬
	Rect	  rtText;			// ���ֵľ�������
	item_property nProterty;    //�ڵ�����  0���� 1����չ�ڵ� 2�ӽڵ�
	BOOL      bFold;            //�Ƿ�Ϊ����״̬,����Ϊչ��״̬ --����ITEM_FOLDER��չ���ڵ���Ч
	MUL_ITEM_INFO * pParent;    //���ڵ�ָ�� --����ITEM_CHILD��չ���ڵ���Ч
	std::vector<MUL_ITEM_INFO*> vtChildItemList;	//�ӽڵ��б�--����ITEM_FOLDER��չ���ڵ���Ч
	int		nType;			//��ǰ�������
	BOOL  bOver;			//�缯�Ƿ������ɣ����Ծ缯��Ч
	int      nCount;			//�缯�ܼ��������Ծ缯��Ч
	int       nVisualPos;       //��¼�ڵ����ʾλ��
	BOOL	  bShowRightIcon[MAX_ICON_COUNT];   //�Ƿ���ʾ�ұߵ�ͼ��
	BOOL	bCloud;			//�Ƿ�Ϊ˽���Ʋ���
	Rect	rectCloud;
	MUL_ITEM_INFO()
	{
		nID				= 0;
		nListIndex			= 0;
		pImgBmp			= NULL;
		nPgPecent		= 0;
		bSimpleShowTime = TRUE;
		nIconShowState	= -1;
		nState			= 0;
		bShowGrayPg		= TRUE;
		netState        = ns_unknow;
		nProterty       = ITEM_NORMAL;
		pParent			= NULL;
		nVisualPos		= 0;
		bFold			= TRUE;
		bCloud		= FALSE;

		nType     =  0;
		bOver     =  FALSE;
		nCount = 0;
		memset(bShowRightIcon, 0, sizeof(bShowRightIcon));
		vtChildItemList.clear();
	}
	~MUL_ITEM_INFO()
	{
		for (int i=0; i<vtChildItemList.size(); i++)			//�ͷŷ��������
		{
			delete vtChildItemList[i];
			vtChildItemList[i] = NULL;
		}
		vtChildItemList.clear();
	}
};

typedef   MUL_ITEM_INFO TASKITEMINFO;

//ý����-item�� �ṹ
struct MEDIA_TABLE_ITEM
{
	int       nID;				//�ڵ�Ԫ�صı��
	int		nSeriesIdx;		//�缯���
	CString   strName;			//��ʾ�ı�-����
	CString   strTime;			//��ʾ�ı�-ʱ��
	Image *   pImgBmp;			//ͼƬ����ͼ��
	int       nPgPecent;		//�������ٷ��� MAX=100
	BOOL      bSimpleShowTime;	//���ģʽ���Ƿ���ʾʱ��/����Ϊ��ʾ���� 
	int       nIconShowState;   //��ʾ���С����ͼ��״̬
	int       nState;			//��ǰ�ڵ��״̬
	BOOL	  bShowGrayPg;      //�Ƿ���ƻһ�������
	netstate  netState;			// ����״̬
	BOOL      bShowPlayBtn;     //�Ƿ���ʾ���Ű�ť
	BOOL	  bFileExist;		//�ļ��Ƿ����
	BOOL	  bHalfTrans;		//�Ƿ��͸
	Rect	  rtText;
	BOOL      bIsEnableSel;	 // �ܷ�ѡ��
	item_property nProterty;

	MEDIA_TABLE_ITEM()
	{
		nID				= 0;
		pImgBmp			= NULL;
		nPgPecent		= 0;
		bSimpleShowTime = TRUE;
		nIconShowState	= 0;
		nState			= 0;
		bShowGrayPg		= TRUE;
		netState        = ns_unknow;
		bShowPlayBtn    = TRUE;
		bFileExist		= TRUE;
		bHalfTrans		= FALSE;
		nProterty       = ITEM_NORMAL;
		bIsEnableSel    = TRUE;
	}
};

//��ý���б�ؼ������Ϣ��������
enum MULTI_LIST_MSG_TYPE
{
	ML_MSG_LBUTTONCLICK ,		//����������
	ML_MSG_RBUTTONCLICK ,		//����Ҽ�����
	ML_MSG_LBUTTONDBLCLK,		//������˫��
	ML_MSG_HEADER_BT_A_CLICK ,  //�����������б�ͷA��ť
	ML_MSG_HEADER_BT_B_CLICK ,  //�����������б�ͷB��ť
	ML_MSG_HEADER_BT_C_CLICK ,  //�����������б�ͷC��ť
	ML_MSG_EDIT_INSERT_ITEM ,	//�û�ͨ����Ƕ�༭����д���item��Ϣ
	ML_MSG_EDIT_REMOVE_ITEM ,	//ɾ�� item �б� 
	ML_MSG_EDIT_MODIFY_ITEM ,	//�û�ͨ����Ƕ�༭����д�޸�item��Ϣ
	ML_MSG_ITEM_HOVER_CHNAGE,	//�û�ͨ������ƶ���itemѡ��ı��ʱ�򴥷�
	ML_MSG_DRAG_DROP_IN,		//�û�ͨ�������ⲿ��ק�ļ�������ͷ�ʱ����
	ML_MSG_DRAG_DROP_ITEMS,		//�û�ͨ�������ⲿ��ק�б��������ͷ�ʱ����
	ML_MSG_DRAG_DROP_REORDER,	//�û�ͨ�������קѡ���������������
	ML_MSG_DRAG_DROP_COPY,		//�û�ͨ�������קѡ�������\����ճ������
	ML_MSG_ITEM_RANGE_REPORT,	//���浱ǰҳ����ʾ��item��Χ
	ML_MSG_ITEM_IMG_CHANGE,		//����ĳItemѡ������ͼ����, ����nID/nItemSel �����洢��һ��/���һ����ʾitem
	ML_MSG_OVERDEL_BTN_CLICK,	//����������
	ML_MSG_ITEM_FOLD,			//��£��չ��ĳ��ʱ����
	ML_MSG_CLICK_TWICE,			//�û�������ε��ĳһ��
	ML_MSG_STRING_CHANGE,		//ĳ����������ݷ����仯
	ML_MSG_CLOUD_BTN_CLICK,		//˽���Ƶ�����Ϣ
	ML_MSG_KEY,
	ML_MSG_QVODTRANSCODE,	// �첥ת�뱻����
	ML_MSG_QVODTRANSCODETIPS,
	ML_MSG_REQUESTLOADSUBITEM		//���������Ŀ¼
};

//��ý���б�ؼ������Ϣ��
struct MULTI_LIST_MSG
{
	MULTI_LIST_MSG_TYPE uMSG;	//��Ϣ����

	int nItemSel;				//��ǰѡ�е�item
	int nChildItem;				//��ǰѡ�е�item���ӽڵ����0,1,2,3,4
	int nID;					//�ýڵ���ڲ�ID���
	int nChar;
	BOOL bMoserInNameRect;		//�����name����ʾ������
	MUL_ITEM_INFO m_SelInfo;    //��ǰѡ��item����ϸ��Ϣ
};
//��ý���б�ؼ������Ϣ��
struct MEDIA_TABLE_MSG
{
	MULTI_LIST_MSG_TYPE uMSG;	//��Ϣ����

	int nItemSel;				//��ǰѡ�е�item
	int nChildItem;				//��ǰѡ�е�item���ӽڵ����0,1,2,3,4
	int nID;					//�ýڵ���ڲ�ID���
	MEDIA_TABLE_ITEM m_SelInfo; //��ǰѡ��item����ϸ��Ϣ
};


//��ý���б�ؼ�ͼƬ���
enum
{
	M_LIST_BK,				//����
	M_LIST_ITEM,			//itemͼƬ
	M_LIST_ITEM_BT,			//item-����
	M_LIST_ITEM_PG,			//item-������
	M_LIST_ITEM_IMG,		//item-ͼƬ
	M_LIST_HEADER_BG,		//�б�ͷ-����
	M_LIST_HEADER_BT_A,		//�б�ͷ-��ťA
	M_LIST_HEADER_BT_B,		//�б�ͷ-��ťB
	M_LIST_ITEM_FOLD,		//Citem-չ������ͼƬ
	M_LIST_ITEM_SIMPLE,		//itemͼƬ-���ģʽ
	M_LIST_DRAG_IMG,		//�϶�ʱ��ʾ��ͼƬ
	M_LIST_ITEM_NEW,		//item-�Ƿ�Ϊnew
	M_LIST_ITEM_TRANSCODE,	// Ԥ����ť�ϵ�ת��
	M_LIST_ITEM_FOLD_CHILD,	//����Ŀ¼չ������
	M_LIST_ITEM_SMALLICON,	//item-�Ƿ�Ϊnew/hot/hd/bd

	M_LIST_BMP_MAX_NUM		//��ý��洢��ͼƬ��Ŀ
};


//item���Iconͼ����״̬
#define MUL_LIST_ITEM_BT_STATE_NUM      5                    //item���Iconͼ����״̬��

#define MUL_LIST_ITEM_BT_STATE_NODRAW   -1                   //item���Iconͼ����״̬-������
#define MUL_LIST_ITEM_BT_STATE_COMPUTER 0                    //item���Iconͼ����״̬-����
#define MUL_LIST_ITEM_BT_STATE_ARROW    1                    //item���Iconͼ����״̬-��ͷ
#define MUL_LIST_ITEM_BT_STATE_GREEN    2                    //item���Iconͼ����״̬-��
#define MUL_LIST_ITEM_BT_STATE_YELLOW   3                    //item���Iconͼ����״̬-��
#define MUL_LIST_ITEM_BT_STATE_RED      4                    //item���Iconͼ����״̬-��
enum CUS_LISTITEM_STATE
{
	CUS_LISTITEM_ST_UN     = -1,    //������б���   δ֪״̬
	CUS_LISTITEM_ST_NM,				//������б�������  NORMAL
	CUS_LISTITEM_ST_HV,				//������б�������  Hover
	CUS_LISTITEM_ST_SEL,			//�б���			select	
	CUS_LISTITEM_ST_DN, 			//������б�������  Down 
};
enum CUS_Alignment
{
	CUS_Align_left		  = 0,
	CUS_Align_center,
	CUS_Align_right,
};
enum CUSTOM_LIST_MSG_TYPE
{
	CUSTOM_MSG_LBUTTONCLICK ,		//����������
	CUSTOM_MSG_RBUTTONCLICK ,		//����Ҽ�����
	CUSTOM_MSG_LBUTTONDBLCLK,		//������˫��
	CUSTOM_MSG_ITEM_HOVER_CHNAGE,	//�û�ͨ������ƶ���itemѡ��ı��ʱ�򴥷�
	CUSTOM_MSG_DRAG_DROP_IN,		//�û�ͨ�������ⲿ��ק�ļ�������ͷ�ʱ����
	CUSTOM_MSG_DRAG_DROP_REORDER,	//�û�ͨ�������קѡ���������������
	CUSTOM_MSG_DRAG_DROP_COPY,		//�û�ͨ�������קѡ�������\����ճ������
	CUSTOM_MSG_SELECTEDITEM_CHANGE, //ѡ���б�����ı�ʱ����
	CUSTOM_MSG_KEY,					//������Ϣ  ����
	CUSTOM_MSG_COLUMN_DOWN,			//�������б�ͷ ��������
	CUSTOM_MSG_COLUMN_SIZE_CHANGE,  //�б��ȸı�ʱ ����
	CUSTOM_MSG_CLICK_TWICE,
	CUSTOM_MSG_STRING_CHANGE,
	CUSTOM_MSG_HEADER_CHECKBOX_CLICKED,	//�б�ͷ��checkbox������
	CUSTOM_MSG_ITEMCHECK_CHANGED,	//�б����ѡȡ״̬������
	CUSTOM_MSG_QVODTRANSCODE,	// �첥ת�뱻����
	CUSTOM_MSG_TRANSCODETIPS, //
};
struct CUSTOM_LIST_MSG
{
	CUSTOM_LIST_MSG_TYPE uMSG;	//��Ϣ����
	int nID;					//�ýڵ���ڲ�ID���
	int nColumn;				//��ǰ�������  �� -1 ��Ϊ��Ч
	int nItem;					//��ǰ�����item
	int nChildItem;				//��ǰѡ�е�item���ӽڵ����0,1,2,3,4
	WORD nChar    ;
};

//////////////////////////////////////////////////////////////////////////////////////
//
// ��ǰ�汾��1.0
// ��    �ߣ���ΰ��
// ������ڣ�
// ����˵������ý���б�ؼ�,ÿ��ѡ��item���Գ�������Ԫ��(�����,ͼƬ��)
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinMultiListCtrl : public CSkinCtrl, public IQvodCallBack
{
public:

	//
	//�ⲿ�ӿڹ�����
	//

	//ҵ���߼������   
	//IDΪ�ڵ�������Ϣ-�ļ�ID��
	int  GetItem_ID(int nIndex);											//��ȡ�б������ΪnIndex��ID���
	int  GetItembyID(int nID, BOOL bFindChild = TRUE);			//��ȡ�ڵ�����nID�����б�nIndexλ��, bFindChild�����ӽ�㣬����ֻ���ҽ��
	BOOL GetItemDatabyID(int nID, MUL_ITEM_INFO *pItemInfo, BOOL bFindChild = TRUE);				//��ȡָ���ڵ�����nID������, bFindChildΪFALSE��ֻ��������㣬����������н��
	int  GetSelectItem_ID(int nIndex = 0);									//��ȡ��ǰѡ��item��ԭ�Ƚڵ�Ĵ���ʱ���ID���(������0��ʼ)
	int  GetSelectItem_ID(std::vector<int> & vtSelIdList);					//��ȡ��ǰ����ѡ��item��ԭ�Ƚڵ�Ĵ���ʱ���ID���
	int  GetSelectItem_ID(std::vector<long> & vtSelIdList);					//��ȡ��ǰ����ѡ��item��ԭ�Ƚڵ�Ĵ���ʱ���ID���
	void GetSeriesItem_ID(int nID, std::vector<long> &vSeriesItem);	//��ȡĳ�缯���������ID
	BOOL RemoveItembyID(int nID, BOOL bChild = FALSE);											//ɾ��һ��item����,nIDΪ�ڵ��nID��Ϣ,bChildΪTRUE�������ӽ��
	BOOL ModifyItembyID(int nID, MUL_ITEM_INFO *pItemInfo);					//�޸�ָ��item����,nIDΪ�ڵ��nID��Ϣ
	void SetItemShowStatebyID(int nID, BOOL bShowTime);						//����item����ʾģʽ  ʱ��/����	--���ڼ��ģʽ��Ч,nIDΪ�ڵ��nID��Ϣ
	void SetItemIconStatebyID(int nID, int  nState);						//����item�����Iconͼ����ʾģʽ,nIDΪ�ڵ��nID��Ϣ
	void ClearAllIconState(int nExceptionID = 0);												//������в���״̬
	BOOL SetSelItembyID(int nID);											//����ָ��itemΪѡ��״̬,nIDΪ�ڵ��nID��Ϣ,-1Ϊ�������ѡ��
	const std::vector<CString>* GetDragDropPathList(void);					//��ȡ��ק�ļ�/�ļ���·�����б�ָ��

	//���ʲ���
	int  GetItemCount(void);												//��ȡitem����
	BOOL GetItemData(int nItem, MUL_ITEM_INFO *pItemInfo, BOOL bGetChildInfo = FALSE);//��ȡ�ڵ�ָ������ź�����(�����m_vtItemList�µĸ����)
	BOOL GetItemChildData(int nIndex, int nChild, MUL_ITEM_INFO *pItemInfo);//��ȡָ���ӽڵ����ź�����  ����:  nIndex --ָ��item����һ���ڵ�����,nChild - �ӽڵ�����λ��; itemInfo --����ȡitem�ڵ���Ϣ;
	int  GetItemState(int nIndex, BOOL bFindChild = TRUE);											//��ȡָ��item��״̬	
	int  GetSelItemCount(void);												//��ȡѡ��item��Ŀ	
	int  GetSelectItem(void);												//��ȡ��ǰѡ��item�����
	BOOL GetSelectItemData(int & nIndex, MUL_ITEM_INFO *pItemInfo);			//��ȡ��ǰѡ��item����ź�����
	int  GetHoverItem(void);												//��ȡ��ǰ���ͣ��(Hover)��item�����
	void GetPreviewImgSize(int & nWdith, int & nHeight);					//��ȡԤ��ͼ�ĳߴ�
	int  GetPageMaxShowItemCount(void);										//��ȡ��ǰ���ڴ�Сÿҳ��������item����
	BOOL IsFolderItem(int nIndex);											//�ж�ĳitem�������Ƿ�Ϊ�ļ��п�չ���ڵ�
	BOOL IsItemUnFold(int nIndex);											//�ж�ĳitem�������Ƿ�Ϊ�ļ���Ϊչ��״̬


	//�޸Ĳ���
	int InsertItem(MUL_ITEM_INFO *pItemInfo, int nIndex = -1);				//����һ��item���� -1Ϊ���뵽���
	BOOL InsertChildItem(MUL_ITEM_INFO *pItemInfo, int nIndex, int nChildIndex = -1); //����һ����item���ݣ�-1Ϊ���뵽���
	BOOL RemoveItem(int nIndex);											//ɾ��һ��item����
	void RemoveAllItem();													//ɾ��(���)����item����
	BOOL MoveItem(std::vector<UINT>& vMoveIndex, int nPos);						//�ƶ�n�ָ����λ����
	BOOL ModifyItem(int nIndex, MUL_ITEM_INFO *pItemInfo);					//�޸�ָ��item����
	BOOL ModifyItemChildData(int nIndex, int nChild, MUL_ITEM_INFO *pItemInfo);//�޸�ָ���ӽڵ����ź�����  ����:  nIndex --ָ��item����һ���ڵ�����,nChild - �ӽڵ�����λ��; itemInfo --����ȡitem�ڵ���Ϣ;
	BOOL SetSelItem(int nIndex);											//����ָ��itemΪѡ��״̬
	BOOL SetSelALLItem(int nState = 1);										//����ȫ��ѡ��״̬
	void SetItemHeight(int nHeight);										//����item�ĸ߶�
	void Sort(int nByKind);													//����-����ָ����ʽ
	typedef  bool (Fun_Sort)(MUL_ITEM_INFO* pItem1, MUL_ITEM_INFO* pItem2);	//	typedef  bool (*pFun_Sort)(MUL_ITEM_INFO* pItem1, MUL_ITEM_INFO* pItem2);
	void Sort(Fun_Sort *pSortFun);											//����-���ݴ���ĺ���ָ���ṩ���û��Զ���


	//��ʾģʽ����
	void UpdateSelCount(void);
	void SetShowMode(BOOL bSimpleMode);										//���ÿؼ�����ʾģʽ  ��ý��ģʽ/���ģʽ
	BOOL GetShowMode(void);													//��ȡ�ؼ�����ʾģʽ  ��ý��ģʽ/���ģʽ
	void SetItemShowState(int nItem, BOOL bShowTime);						//����item����ʾģʽ  ʱ��/����	--���ڼ��ģʽ��Ч
	BOOL GetItemShowState(int nItem);										//��ȡitem����ʾģʽ			--���ڼ��ģʽ��Ч
	void SetItemIconState(int nItem, int  nState);							//����item�����Iconͼ����ʾģʽ
	int  GetItemIconState(int nItem);										//��ȡitem�����Iconͼ����ʾģʽ
	void SetRightIconShowByID(int nID, BOOL bShow);							//����item���ұ�icon�Ƿ���ʾ
	void SetItemPgShowState(int nItem, BOOL bShowGrayPg);					//����item�Ľ������Ƿ���ʾ�һ�״̬
	BOOL GetItemPgShowState(int nItem);										//��ȡitem�Ľ������Ƿ���ʾ�һ�״̬ 
	void SetItemShortCutImg(int nItem, Image * pImgBmp);					//����ָ��item������ͼ	
	void SetReportItemFlag(BOOL bOpen);										//���ÿ���״̬ - ���浱ǰҳ����ʾitem����
	BOOL GetReportItemFlag(void);											//��ȡ����״̬ - ���浱ǰҳ����ʾitem����
	void SetRedraw(BOOL bDraw);												//�����ػ濪��
	BOOL GetRedraw(void);													//��ȡ�ػ濪��
	void SetFoldStyle(BOOL bFoldStyle);										//����item�Ƿ�Ϊ��չ����£��ʽ
	BOOL GetFoldStyle(void);												//��ȡitem�Ƿ�Ϊ��չ����£��ʽ
	void SetOnlyShowName(BOOL bOnlyShowName);										//����ֻ��ʾ���Ƶ�ģʽ
	void SetCanDrag(BOOL bCanDrag = TRUE);				//�����Ƿ�֧���϶�
	// ���� ��MUL_ITEM_INFO����
	// ���� ��������
	// ʱ�� ��2012-09-06
private:
	// ��  ����MUL_ITEM_INFO ��������
	//	       bCopyChild �Ƿ񿽱��ӽڵ� vtChildItemList
	// ����ֵ������pDes
	MUL_ITEM_INFO * Copy(MUL_ITEM_INFO *pDes ,MUL_ITEM_INFO *pSrc , BOOL bCopyChild = FALSE); 
	//
	//�ڲ�ʵ�ֹ�����
	//
public:
	CSkinMultiListCtrl();
	virtual ~CSkinMultiListCtrl();
	virtual BOOL OnKey(WORD nChar);											//���̰���

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();													//����ʼ��
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//��Ϣ�ص�--���ӿؼ�����ʱ�򴫸���obj,����ʵ�ֵ�ʱ����������
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//�����ӿ�--����COM��ѯ�ӿ�,
	virtual BOOL OnTimer();													//�Զ����ö�ʱ��--�鿴�Ƿ���Ҫ�ػ�

	virtual BOOL OnMouseMoveIn(short x, short y);							//��괦����Ϣ-�ƽ��ؼ�
	virtual BOOL OnMouseMoveOut();											//��괦����Ϣ-�Ƴ��ؼ�
	virtual BOOL OnMouseDown(short x, short y);								//��괦����Ϣ-����
	virtual BOOL OnMouseUp(short x, short y);								//��괦����Ϣ-�ͷ�
	virtual BOOL OnMouseDrag(short x, short y);								//��괦����Ϣ-�϶�
	virtual BOOL OnMouseMove(short x, short y);								//��괦����Ϣ-�ƶ�
	virtual BOOL OnRButtonUp(short x, short y);								//��괦����Ϣ-�Ҽ��ͷ�
	virtual BOOL OnRButtonDown(short x, short y);							//��괦����Ϣ-�Ҽ�����
	virtual BOOL OnDblClick(short x, short y);								//��괦����Ϣ-���˫��
	virtual BOOL OnWheel(short nStep);										//��괦����Ϣ-���ֹ���
	virtual BOOL Layout(short nWidth, short nHeight);						//����չ�ֿؼ�--�ú����ɸı䴰�ڴ�Сλ�õ�ʱ�����WM_SIZE

	BOOL OnMouseDown_FoldStyle(short x, short y);							//��괦����Ϣ-����
	BOOL OnMouseUp_FoldStyle(short x, short y);								//��괦����Ϣ-�ͷ�
	BOOL OnMouseDrag_FoldStyle(short x, short y);							//��괦����Ϣ-�϶�
	BOOL OnMouseMove_FoldStyle(short x, short y);							//��괦����Ϣ-�ƶ�
	BOOL OnRButtonUp_FoldStyle(short x, short y);							//��괦����Ϣ-�Ҽ��ͷ�
	BOOL OnDblClick_FoldStyle(short x, short y);							//��괦����Ϣ-���˫��

	void LButtonClickItem_Work_FoldStyle(int nClickItem);					//�����ĳһitem������Ҫ����ҵ����
	void ReOrderItemByDragDrop_FoldStyle(void);								//������קitem������item��λ��
	void Paint(Graphics *pGraph);											//���ƿؼ�
	void Paint_SimpleMode(Graphics *pGraph);								//���ƿؼ� ����ģʽ
	void Paint_MultiMode(Graphics *pGraph);									//���ƿؼ� ��ý��ģʽ

	void DrawItem(Graphics *pGraph, int nItem);								//����ĳһ��item
	void DrawItem_SimpleMode(Graphics *pGraph, int nVirtualOrder, int nItem);//����ĳһ��item
	void DrawItem_FlatStyle(Graphics *pGraph, const int nItem);					//����ĳһ��item
	void DrawItem_WaveStyle(Graphics *pGraph, int nItem);					//����ĳһ��item
	void DrawItem_FrameStyle(Graphics *pGraph, int nItem);					//����ĳһ��item
	void DrawDragBmp(HBITMAP & hBmp);										//�����϶�item
	void DrawItem_FlatStyle_byDrag(Graphics *pGraph, int nItem, int nOrder);//����ĳһ��item  _byDrag
	void DrawItem_SimpleMode_byDrag(Graphics *pGraph,  
		                            int nItem, int nOrder);					//����ĳһ��item _byDrag
	void Draw_Progress(Graphics *pGraph, int nItem, Rect & pgRect);			//����ĳһ��item������
	void DrawChildItem_SimpleMode(Graphics *pGraph, int nVirtualOrder, 
								   int nItem, int nChild);					//����ĳһ��item���ӽڵ�

	void GetItemRect_byDrag(int nOrder, Rect & rtItem);						//��ȡitem������_byDrag
	void OnCtrlScroll(long lPos);											//������������
	void RefreshScrollCtrlPos();											//���ù������Ļ���λ��
	void VisualizeScrollCtrl();												//���ù������Ŀ���״̬
	void RefreshAllItem_VitualPos(void);									//ˢ�µ�ǰ�ṹ�����нڵ�,�ӽڵ��Ӧ����ʾ����λ��
	BOOL CheckVerScroll(const short x, const short y);						//��������Ƿ���Scroll��
	BOOL CheckSpliter(const short x, const short y);						//��������Ƿ���Spliter��
	BOOL CheckCursor(short x, short y);										//��������Ƿ��ڿؼ���
	BOOL IsPointInRect(Rect rt, int x, int y);								//��������Ƿ��ھ�����
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//��XML�ļ�����Ƥ��Ԫ�ص�����
	void LButtonClickItem_Work(int nClickItem);								//�����ĳһitem������Ҫ����ҵ����
	void DoDragDrop(int xDragImgMove, int yDragImgMove);					//�϶����ƴ���--x,yΪ�϶�ͼ��������λ��
	void ReOrderItemByDragDrop(void);										//������קitem������item��λ��
	BOOL IsPointInTextArea(short x, short y);								//�ж������Ƿ�������������

	BOOL ISPointInFoldBtn(int x, int y, int nItem);							//�жϵ��Ƿ���չ����ť��
	BOOL ISPointInFolderItem(int x, int y);									//�жϵ��Ƿ��ڿ�չ��item��
	void FoldItemByIndex(BOOL bFold, UINT uIndex);							//�۵���չĳһ��item
	void FoldItemBynID(BOOL bFold, INT ItemnID);							//�۵���չĳһ��item
	BOOL IsFoldItem(int nItem);												//�ж�ĳ���Ƿ�Ϊ��չ����
	int  GetSelectChildItem(int & nItem, int & nChildItem);					//��ȡѡ��item�͵�һ��ѡ����item��
	int  GetShowItemCount(void);											//��ȡ��ǰ��ʾ��item��
	BOOL GetItemFromVirtualPos(UINT nVirtualPos, 
							   int &nItem, int &nChildItem);				//����������ʾλ�û�ȡ��Ӧ�Ľڵ�,�ӽڵ�ֵ
	int GetVirtualPosFromItem(UINT nItem, INT nChildItem);					//���ݽڵ�,�ӽڵ�ֵ��ȡ��Ӧ��������ʾλ��,����nChildItem-1Ϊ���ڵ�,����0Ϊ�ӽڵ�


	BOOL InsertSeriesChildItem(long nCtrlId, MUL_ITEM_INFO& mi, int nInsertIdx);
	BOOL  m_bFoldStyle;														//item�Ƿ�Ϊ��չ����£��ʽ
	int   m_nChildItemInWidth;												//�ӽڵ����ʱ���������
	void sort_by_ID(void);													//����--����ID
	void sort_by_Progress(void);											//����--���ݽ���
	void sort_by_NetQuality(void);											//����--��������
	void sort_by_Time(void);												//����--���ݲ���ʱ��

	void ReportPageItemShow(int nLower, int nHigher);						//���浱ǰҳ����ʾitem����

	// ѡ���ӽڵ�ʱ���¸��ڵ�״̬��nFatherIndex���ӽڵ��������ڵ�����
	void UpdateFatherState(UINT nFatherIndex);

	// ѡ�и��ڵ�ʱ�����ӽڵ�״̬��nFatherIndex���ӽڵ��������ڵ�����
	void UpdateChildState(UINT nFatherIndex);

	// ѡ�����ɸ��ڵ㡣nSFatherIndex����ʼ������nEFatherIndex����������
	void SelectFatherPeers(UINT nSFatherIndex, UINT nEFatherIndex);

	// ѡ�������ӽڵ㡣nSChildIndex����ʼ������nEChildIndex����������
	void SelectChildPeers(UINT nSChildIndex, UINT nEChildIndex, UINT nFatherIndex);

	// ��ȡ�ɼ���Ԫ�ص���ʼ����ֹ������nFirstIndex����ʼ������nLastIndex����������
	void GetShowItemIndex(int &nFirstIndex, int &nLastIndex);

	// ��ȡĳ��y���괦��Item����
	int GetItemIndexFromYpos( int nY );

	// ��ȡitem�ľ�����ʾ����
	void GetItemRect(int nItem, Rect & rtItem);

	// ������������λ�ã�ʹ���ڲ��ŵ��б���ɼ�
	void ScrollToPlayingItemPos(int nID);
	void ScrollToIndex(int index);
	void ScrollSelect(int nHover = -1, int y = -1);
	void VirtualRangeSelect(int start, int end);
	// ��ȡ ��ק���б���ʱ���Hover�ڵ�item
	int GetDragHoverItem() ;
	//	��ȡitem�߶�
	int GetItemHeight();

	// �����༭�ؼ�
	CSkinEdit* CreateEdit();

	BOOL EditItemVirtualIndex(int nIndex);

	BOOL EditItem(int nItem, int nChildItem = -1);

	BOOL OverEdit(BOOL bModify);

	long OnDrag(UINT uMsg, long wparam, long lparam);
	Rect GetContentRect(void);
	BOOL DragMoveList( short x, short y );
	static VOID CALLBACK DragScroll_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	//
	//��Ա����
	//

	// DragDrop���
	virtual BOOL DragMouseIn(short x, short y);
	virtual BOOL DragMouseOut();
	virtual BOOL DragMouseMove(short x, short y);
	virtual BOOL DragMouseUp(short x, short y);
private:
	void PaintCloudBtn(Graphics *pGh, int nItem, Rect & pgRect, int nChildItem = -1);

private:

	std::vector<MUL_ITEM_INFO*> m_vtItemList;						//����ʾ���������������

	MULTI_LIST_MSG  m_mouseMSG;										//����������෢������Ϣ,����������Ϣ

	CXMLElement		m_xmlItemStyle;									//Item ��ģ��
	CSkinScroll*	m_pScroll;										//�������ӿؼ�		
	CSkinButton*	m_pODBtn;										//����ɾ����ť
	CSkinEdit*		m_pEdit;										//�༭��ؼ�
	Bitmap*			m_pBmpODBtn;									//������ťͼƬ

	CRITICAL_SECTION m_rsLock;										//��Դ��-���ڷ�ֹ���߳���ɾ����������ƹ��̵ĳ�ͻ

	std::vector<CString> m_vtDragList;								//��ק�ļ�/�ļ������Ƶ�������
	ui::drop::CDragDropCtrl		 m_DragDropCtrl;							//�Ϸſؼ�
	HBITMAP				 m_hDragBitmap;								//���Ա����϶���λͼ
	BOOL				 m_bShouldDrag;								//�Ƿ�Ӧ������קʱ��
	BOOL				 m_bCanItemDrag;							//�Ƿ�֧���б����϶�
	int					 m_ndragHoverItem;							//��ק���б���ʱ���Hover�ڵ�item
	BOOL                 m_bDragAddNetTask ;
	int					 m_ndragHoverChildItem;						//��ק���б���ʱ���Hover�ڵ�item����itemλ��
	int					 m_nVirtualDragHoverItem;					//��ק���б���ʱ���Hover�ڵ�����itemλ��

	long m_nTotalHeight;											//Item���ܸ�
	BOOL m_bShowScroll;												//�Ƿ���Ҫ��ʾScroll
	BOOL m_bInscroll;												//����Ƿ���scroll��
	BOOL m_bInOverDelBtn;											//����Ƿ��ڸ���ɾ����ť��
	long m_lPageViewPos;											//��ǰ��ʾҳ������ܶ�����ƫ��(Scroll��ƫ����)
	long m_nScrollWidth;											//Scroll�Ŀ��
	BOOL m_bScrolldown;												//Scroll down
	BOOL m_bOverDelBtndown;											//����ɾ����ť�Ƿ���
	BOOL m_bDrag;													//������϶�ʱΪ TRUE
	BOOL m_bRButtonDown;											//����Ҽ��Ƿ�Ϊ����״̬
	BOOL m_bLBDbClickDown;											//���˫���Ƿ�Ϊ����״̬
	BOOL m_nItemRBUp;												//��¼�Ҽ�������б�Ԫ��
	BOOL m_bODBtnCmdInvalid;										//��¼�Ҽ�������б�Ԫ��
	BOOL m_bOnlyShowName;											//ֻ��ʾʱ���ģʽ

	int	 m_iNameFontSize;											//�����С -����
	int	 m_iTimeFontSize;											//�����С -ʱ��

	long	 m_nLastSeriesIconStatusId;							//���һ�ξ缯��״̬

	Rect m_btRect;													//��������
	Rect m_btSimpleRect;											//��������-���
	Rect m_textNameRect;											//�����ı�����
	Rect m_textNameSimpleRect;										//�����ı�����-���
	Rect m_textTimeRect;											//ʱ���ı�����
	Rect m_textTimeSimpleRect;										//ʱ���ı�����-���
	Rect m_pgRect;													//����������
	Rect m_pgSimpleRect;											//����������-���
	Rect m_imgRect;													//ͼƬ����
	Rect m_imgHVRect;												//ͼƬ����hover״̬

	Rect m_rtDragFrame;												//�����ѡʱ����ƵĿ�
	CPoint m_ptDragStart;										    //��¼�����ѡʱ����Ƶİ������
	CPoint m_ptDragEnd;										        //��¼�����ѡʱ����Ƶĵ����յ�
	CPoint m_ptDragStartOld;										//��¼�����ѡʱ����Ƶİ������


	Color m_clrItemBK_NL;											//������ɫNormal״̬
	Color m_clrItemBK_HV;											//������ɫHover״̬
	Color m_clrItemBK_DN;											//������ɫSelect(down)״̬

	Color m_clrNameText_NL;											//������ɫNormal״̬
	Color m_clrNameText_HV;											//������ɫHover״̬
	Color m_clrTimeText_NL;											//������ɫNormal״̬
	Color m_clrTimeText_HV;											//������ɫHover״̬

	Color m_clrLeftBorder;											//��߱߿���ɫ
	Color m_clrDragFrame;											//��ק����ɫ
	Color m_clrDragFrameBorder;										//��ק��ߵ���ɫ

	Bitmap * m_aImg[M_LIST_BMP_MAX_NUM];							//�洢����״̬�Ļ���λͼ
	Bitmap * m_pRightIcon[MAX_ICON_COUNT];							//�ұ�ͼ��				//item�ұߵ�icon
	CString m_textFontStyle;										//������ʽ
	CString m_textTimeFontStyle;									//ʱ��������ʽ

	int   m_nItemHeight;											//item�ĸ߶�-��ǰģʽ
	int	  m_nItemSelectHeight;										//ѡ��item�ĸ߶�(��ѡ)
	int   m_nItemSimpleHeight;										//item�ĸ߶�-���ģʽ
	int   m_nItemMultiHeight;										//item�ĸ߶�-��ý��ģʽ


	int   m_nItemCount;												//item����
	int   m_nVirtualItemCount;										//����item����
	int   m_nItemHover;												//��ǰ�����������һ��item
	int   m_nItemDown;												//��ǰ��갴������һ��item
	int   m_nItemSel;												//��ǰѡ����һ��item
	int   m_nItemSelCount;											//��ǰѡ��item����Ŀ
	int   m_nTopViewVirtualItem;									//����ҳ��,���������Item��(����Item)
	int   m_nTopViewItem;											//����ҳ��,�����Item
	int   m_nTopViewChildItem;										//����ҳ��,����������ʾ������item��,���¼�ڴ�,���m_nTopViewItem��λ
	int   m_nItemDragFrameDown;										//��ѡʱ���°�ť���ڵ�ѡ��
	int   m_nItemDragFrameDownTopVVI;								//��ѡʱ���°�ť���ڵ�ѡ��

	int	  m_nEditItem;												//�༭�����ڵ���
	int   m_nEditChildItem;											//�༭�����ڵ�����
	BOOL  m_bEditShow;												//�༭���Ƿ�����ʾ
	BOOL  m_bClickTwice;											//�Ƿ����ε����ͬһ����


	Rect  m_HeaderRect;												//�б�ͷ����
	Rect  m_ListRect;												//�б�����

	int   m_nItemDrawStyle;											//item������ʽ
	BOOL  m_bHaveHeader;											//�Ƿ����б�ͷ
	int   m_nHeaderHeight;											//�б�ͷ�ĸ߶�

	BOOL  m_bShowSimpleMode;										//�ؼ���ģʽ�Ƿ�Ϊ���ģʽ ����Ϊ��ý��ģʽ
	BOOL  m_bShowTimeState;											//item��ʾģʽ�Ƿ�Ϊʱ�� ����Ϊ ����--���ڼ��ģʽ��Ч

	BOOL  m_bShowDragFrame;											//�Ƿ��������קʱ��ʾ��
	BOOL m_bDragWay_byArea;											//�Ƿ��ѡ������������������������-����Ϊ��ǰ�Ƿ��Ѿ�ѡ��
	BOOL m_bOpenReportItemFlag;										//�Ƿ������浱ǰҳ����ʾitem�����
	int m_nOldShowLower;											//��ǰ��ʾ���-��������״̬
	int m_nOldShowHigher;											//��ǰ��ʾ�����-��������״̬
	int m_nOldShowLowerID;											//��ǰ��ʾ����ļ�ID-��������״̬
	int m_nOldShowHigherID;											//��ǰ��ʾ������ļ�ID-��������״̬
	BOOL m_bRedraw;													//ͼ�����ʱ�Ƿ���ƿؼ�
	BOOL m_bShowProgressByText;										//�Ƿ���������������
	BOOL m_bSetScrollRect;											//�Ƿ����ù�������RECT

	BOOL m_bAlwayHideODBtn;											// һֱ����ɾ����ť
	static CSkinMultiListCtrl * m_pMulListCtrl;
	int  m_nDragScrollDown;                                        // ��ֱ�������� 0 ���� 1 ����
	int m_preTimer;
	BOOL m_bSetTimer;
	DWORD m_dwScrlTimerID;
	BOOL		m_bCanDrag;								//�Ƿ�֧����

	Bitmap*			m_pBmpCloudBtn;									//˽����ͼƬ
	BOOL  m_bIsDragSrouce;

};

//////////////////////////////////////////////////////////////////////////
#define COPYCSTRING(NAME)\
SafeDeleteArray(NAME)\
	if ( info.##NAME )\
{\
	NAME = new TCHAR[lstrlen(info.##NAME)+1] ;\
	lstrcpy(NAME, info.##NAME) ;\
}\

struct ONLINEMEDIA_ITEM_INFO
{
	ONLINEMEDIA_ITEM_INFO()
	{
		memset(this, 0, sizeof(ONLINEMEDIA_ITEM_INFO) ) ;
		bSave = TRUE ;
	};

	~ONLINEMEDIA_ITEM_INFO()
	{
		SafeDeleteArray(classid)
		SafeDeleteArray(docid)
		SafeDeleteArray(title)
		SafeDeleteArray(playlink)
		SafeDeleteArray(source)
		SafeDeleteArray(upinfo)
		SafeDeleteArray(language)

		for (int i=0; i<director.size(); i++)
		{
			SafeDeleteArray(director[i]) ;
		}

		for (int i=0; i<actor.size(); i++)
		{
			SafeDeleteArray(actor[i]) ;
		}
	};

	ONLINEMEDIA_ITEM_INFO &operator =(const ONLINEMEDIA_ITEM_INFO& info)
	{
		COPYCSTRING(classid)
		COPYCSTRING(docid)
		COPYCSTRING(title)
		COPYCSTRING(playlink)
		COPYCSTRING(source)
		COPYCSTRING(upinfo)
		COPYCSTRING(cover)
		COPYCSTRING(more)
		COPYCSTRING(language)

		nLevel = info.nLevel ;
		nChild = info.nChild ;
		bFolder = info.bFolder ;
		bLight  = info.bLight ;
		bSave	= info.bSave ;
		sc		= info.sc ;

		num = info.num ;
		tips = info.tips ;
		pos = info.pos ;

		hot = info.hot ;
		newly = info.newly ;
		hd = info.hd ;
		bd = info.bd ;
		chase = info.chase ;

		color = info.color ;
		bold  = info.bold ;
		return *this ;
	}

	int  nLevel;		//����
	int  nChild;		//�ӽڵ����
	BOOL bFolder;		//�Ƿ�չ����TRUEΪչ��
	int  nUpInforWidth; //������Ϣ����
	ONLINEMEDIA_ITEM_INFO *pParent ;//���ڵ�ָ��
	BOOL bRequest;		//�Ƿ������ӽڵ�����
	BOOL bLoadding;		//��ʾ�����ڼ�����
	BOOL bLight;		//ǰ�������ʾ׷��
	BOOL bSave;			//�ڵ��Ƿ���Ҫ����

	int  num ;
	int  tips ;			
	BOOL chase ;		//׷������
	int	 pos ;

	LONGLONG sc;

	BOOL	hot;
	BOOL	newly;
	BOOL	hd;
	BOOL	bd;

	//classid 
	//0 ������ 
	//1 ��Ӱ
	//2 ���Ӿ�
	//3 ����
	//4 ����
	TCHAR	*classid;
	TCHAR	*docid;
	TCHAR	*title;
	TCHAR	*playlink;
	TCHAR	*source;
	TCHAR	*upinfo;
	TCHAR	*cover;
	TCHAR	*more;
	TCHAR	*language;

	//������ʽ
	DWORD	color;
	BOOL	bold;

	vector<TCHAR*> director;
	vector<TCHAR*> actor;

	Rect   rect;	//��ʾ������
	long   nIndex;	//���б��е�����
};


const TCHAR kMovieClassID[] = _T("1");
const TCHAR kTVClassID[] =    _T("2");
const TCHAR kVarietyClassID[] = _T("3");
const TCHAR kCartoonClassID[] = _T("4");

//��ý���б�ؼ������Ϣ��
struct ONLINEMEDIA_LIST_MSG
{
	ONLINEMEDIA_LIST_MSG()
	{
		memset(this, 0, sizeof(ONLINEMEDIA_LIST_MSG) ) ;
	}

	ONLINEMEDIA_LIST_MSG &operator =(const ONLINEMEDIA_LIST_MSG& info)
	{
		uMSG = info.uMSG ;
		nItemSel = info.nItemSel ;
		ptItem = info.ptItem ;
		pSelInfo = info.pSelInfo ;

		return *this ;
	}

	MULTI_LIST_MSG_TYPE uMSG;			//��Ϣ����

	int					nItemSel;		//��ǰѡ�е�item
	CPoint				ptItem ;		//item left top
	
	ONLINEMEDIA_ITEM_INFO *pSelInfo;    //��ǰѡ��item����ϸ��Ϣ
};

enum onlinemediaparamer
{
	bydocid,
	byplaylink
};

//////////////////////////////////////////////////////////////////////////////////////
//
// ��ǰ�汾��1.0
// ��    �ߣ���־��
// ������ڣ�
// ����˵��������ý���б�ؼ�
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinOnlineMediaListCtrl : public CSkinCtrl, public IQvodCallBack
{
public:

	//
	//�ⲿ�ӿڹ�����
	//

	//���ʲ���
	int  GetItemCount(void);												//��ȡitem����
	BOOL GetSelectItem(ONLINEMEDIA_ITEM_INFO &info);						//��ȡ��ǰѡ��item
	int  GetItemHeight(void);												//��ȡitem�ĸ߶�
	BOOL StopPlayMedia();
	BOOL GetSubItemList(vector<ONLINEMEDIA_ITEM_INFO *> &pItemlist, ONLINEMEDIA_ITEM_INFO *pParent, int nItemSel);
	void SetListDataIndex(int nIndex) ;
	void SetPlayItem(ONLINEMEDIA_ITEM_INFO *pItem, BOOL bScroll=FALSE) ;
	ONLINEMEDIA_ITEM_INFO *SetPlayItemBy_(TCHAR *docid, int byParamer) ;
	BOOL PlayNextItem() ;
	TCHAR *GetPlayItemDocID() ;
	void GetChildList(ONLINEMEDIA_ITEM_INFO *pParent, vector<ONLINEMEDIA_ITEM_INFO *> &vtChild) ;

	//�޸Ĳ���
	BOOL InsertItem(vector<ONLINEMEDIA_ITEM_INFO *>pItemlist, ONLINEMEDIA_ITEM_INFO *pParent=NULL, int nIndex = -1);
	BOOL InsertItem(ONLINEMEDIA_ITEM_INFO *pItemInfo, int nIndex = -1);		//����һ��item���� -1Ϊ���뵽���
	int  RemoveItem(ONLINEMEDIA_ITEM_INFO *pItemInfo, BOOL bDeleteFather = TRUE);											//ɾ��һ��item����
	void RemoveAllItem();													//ɾ��(���)����item����
	BOOL ModifyItem(int nIndex, ONLINEMEDIA_ITEM_INFO *pItemInfo);			//�޸�ָ��item����
	BOOL ModifyItembyReName(int nIndex, CString strNewName);				//�޸�item-������, -1Ϊѡ�е�item,���ַ�Ϊԭitem�ı�
	BOOL SetSelItem(int nIndex);											//����ָ��itemΪѡ��״̬
	void SetItemHeight(int nHeight);										//����item�ĸ߶�
	void Sort(int nByKind);													//����-����ָ����ʽ


	//
	//�ڲ�ʵ�ֹ�����
	//
public:
	CSkinOnlineMediaListCtrl();
	virtual ~CSkinOnlineMediaListCtrl();

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();													//����ʼ��
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//��Ϣ�ص�--���ӿؼ�����ʱ�򴫸���obj,����ʵ�ֵ�ʱ����������
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//�����ӿ�--����COM��ѯ�ӿ�,
	virtual BOOL OnTimer();													//�Զ����ö�ʱ��--�鿴�Ƿ���Ҫ�ػ�

	BOOL LoadItemsFromFile(CString strPath);
	BOOL SaveItemsToFile(CString strPath);
	BOOL SetVersion(int nVersion);
	LONGLONG  GetVersion(CString strPath);

	virtual BOOL OnKey(WORD nChar);											//���̰���
	virtual BOOL OnMouseMoveIn(short x, short y);							//��괦����Ϣ-�ƽ��ؼ�
	virtual BOOL OnMouseMoveOut();											//��괦����Ϣ-�Ƴ��ؼ�
	virtual BOOL OnMouseDown(short x, short y);								//��괦����Ϣ-����
	virtual BOOL OnMouseUp(short x, short y);								//��괦����Ϣ-�ͷ�
	virtual BOOL OnMouseDrag(short x, short y);								//��괦����Ϣ-�϶�
	virtual BOOL OnMouseMove(short x, short y);								//��괦����Ϣ-�ƶ�
	virtual BOOL OnRButtonUp(short x, short y);								//��괦����Ϣ-�Ҽ��ͷ�
	virtual BOOL OnRButtonDown(short x, short y);							//��괦����Ϣ-�Ҽ�����
	virtual BOOL OnDblClick(short x, short y);								//��괦����Ϣ-���˫��
	virtual BOOL OnWheel(short nStep);										//��괦����Ϣ-���ֹ���
	virtual BOOL Layout(short nWidth, short nHeight);						//����չ�ֿؼ�--�ú����ɸı䴰�ڴ�Сλ�õ�ʱ�����WM_SIZE
	virtual void SetState(skin_state state);								//���ÿؼ�״̬

	void Paint(Graphics *pGraph);											//���ƿؼ�
	BOOL DrawItem(Graphics *pGraph, ONLINEMEDIA_ITEM_INFO *pInfo, Rect &itemRect); //����ĳһ��item
	void GetItemRect(int nItem, Rect & rtItem);								//��ȡitem������
	void OnCtrlScroll(long lPos);											//������������
	void RefreshScrollCtrl();												//��������scroll�ؼ�����
	void RefreshCtrlLayOut(void);											//ˢ�¿ؼ��Ű�
	ISkin* GetSkinObject(const short x, const short y);						//�����������Ŀؼ�
	BOOL CheckVerScroll(const short x, const short y);						//��������Ƿ���Scroll��
	BOOL CheckSpliter(const short x, const short y);						//��������Ƿ���Spliter��
	BOOL CheckCursor(short x, short y);										//��������Ƿ��ڿؼ���
	BOOL IsPointInRect(Rect rt, int x, int y);								//��������Ƿ��ھ�����
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//��XML�ļ�����Ƥ��Ԫ�ص�����
	void ModifyItemWork(void);												//�����޸�item�Ĺ���
	BOOL GetItemFromYPos(int nY, int &nItem);
	BOOL IsItemValid(int nItem);
	int  GetShowItemCounts();
	void RelayoutMoreCtrl();

	void sort_by_name(void);												//����--��������

protected:
	BOOL LoadTaskNode(CXMLElement& xmlTask) ;
	void SaveTaskNode(CXMLDoc& xmlDoc, CXMLElement& xmlTasks, ONLINEMEDIA_ITEM_INFO *pTask);

	//
	//��Ա����
	//
private:
	ONLINEMEDIA_LIST_MSG  m_mouseMSG;								//����������෢������Ϣ,����������Ϣ

	CRITICAL_SECTION m_rsLock;										//��Դ��-���ڷ�ֹ���߳���ɾ����������ƹ��̵ĳ�ͻ
	CSkinScroll*	m_pScroll;										//�������ӿؼ�
	CSkinButton*	m_pODBtn;										//����ɾ����ť
	CSkinCtrl*		m_pMoreCtrl;									//���ఴť
	Bitmap*			m_pBmpODBtn;									//������ťͼƬ

	enum
	{
		object_hover,	//��ǰ�����������һ��item
		object_lbdown,//��꽻��״̬
		object_lbup,
		object_rbdown,
		object_rbup,

		object_active_count
	};
	ISkin	*m_pSkinObjectActive[object_active_count] ;				//����ؼ�
	
	long m_nTotalHeight;											//Item���ܸ�
	long m_nMaxShowItems;											//��ǰ�б���������ʾ��Item��
	BOOL m_bShowScroll;												//�Ƿ���Ҫ��ʾScroll
	long m_lPageViewPos;											//��ǰ��ʾҳ������ܶ�����ƫ��(Scroll��ƫ����)
	long m_nScrollWidth;											//Scroll�Ŀ��
	
	BOOL m_bDrag;													//������϶�ʱΪ TRUE

	CString m_strFontStyle;											//������ʽ
	int	 m_iNameFontSize;											//�����С -����
	int	 m_iDescFontSize;											//�����С -����

	Rect m_itemBtRect;												//item��������
	Rect m_itemTextRect;											//item�ı�����


	Color m_clrItemBK_NL;											//������ɫNormal״̬
	Color m_clrItemBK_HV;											//������ɫHover״̬
	Color m_clrItemBK_DN;											//������ɫSelect(down)״̬

	Color m_clrItemText_NL;											//������ɫNormal״̬
	Color m_clrItemText_HV;											//������ɫHover״̬	

	Bitmap* m_aImg[M_LIST_BMP_MAX_NUM];								//�洢����״̬�Ļ���λͼ

	int  m_nItemHeight;												//item�ĸ߶�
	int  m_nItemCount;												//item����
	int  m_nItemShowCount;											//��ʾ��Item����

	enum
	{
		item_hover,	//��ǰ�����������һ��item
		item_sel,	//��ǰѡ����һ��item
		item_play,	//��ǰ���ŵ����ĸ�Item

		item_lbdown,//��꽻��״̬
		item_lbup,
		item_rbdown,
		item_rbup,

		item_active_count
	};
	
	ONLINEMEDIA_ITEM_INFO* m_ItemsActive[item_active_count]	;		// ���Item

	vector<ONLINEMEDIA_ITEM_INFO*> m_vtItemsShow ;					//��ǰ��ʾ��Item����Pain��ʱ�����
	BOOL						   m_bDirtyItem ;					//ΪTrue����Ҫ�����б�
	
	int		m_nTopViewItem;											// ����ҳ��,�����Item   
	Rect	m_ListRect;												// �б�����
	int		m_nLevelDis;											// ����ƫ�ƾ���
	LONGLONG m_nOldVersion;										// ���ر�����б�汾�ţ������Ժ�����õ���
	int		m_elistindex ;
	int		m_nItemHover ;

};
//////////////////////////////////////////////////////////////////////////

//�ɱ༭�б�item�ṹ
struct EDIT_LIST_ITEM_INFO
{
	CBitmap * pBtBmp;	//��������ͼ��
	CString strName;	//��ʾ�ı�-����
	CString strDesc;	//��ʾ�ı�-����˵��
};
//////////////////////////////////////////////////////////////////////////////////////
//
// ��ǰ�汾��1.0
// ��    �ߣ���ΰ��
// ������ڣ�
// ����˵�������ڱ༭�����Ԫ�ص��б�ؼ�
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinEditListCtrl : public CSkinCtrl, public IQvodCallBack	
{
public:

	//
	//�ⲿ�ӿڹ�����
	//

	//���ʲ���
	int  GetItemCount(void);												//��ȡitem����
	BOOL GetItemData(int nIndex, EDIT_LIST_ITEM_INFO *pItemInfo);			//��ȡָ��item����ź�����
	int  GetSelectItem(void);												//��ȡ��ǰѡ��item�����
	BOOL GetSelectItemData(int & nIndex,EDIT_LIST_ITEM_INFO *pItemInfo);	//��ȡ��ǰѡ��item����ź�����
	int  GetItemHeight(void);												//��ȡitem�ĸ߶�
	BOOL CanShowFullText(int nIndex);										//item�������Ƿ���������ʾ

	//�޸Ĳ���
	BOOL InsertItem(EDIT_LIST_ITEM_INFO *pItemInfo, int nIndex = -1);		//����һ��item���� -1Ϊ���뵽���
	BOOL RemoveItem(int nIndex);											//ɾ��һ��item����
	void RemoveAllItem();													//ɾ��(���)����item����
	BOOL ModifyItem(int nIndex, EDIT_LIST_ITEM_INFO *pItemInfo);			//�޸�ָ��item����
	BOOL ModifyItembyReName(int nIndex, CString strNewName);				//�޸�item-������, -1Ϊѡ�е�item,���ַ�Ϊԭitem�ı�
	BOOL SetSelItem(int nIndex);											//����ָ��itemΪѡ��״̬
	void SetItemHeight(int nHeight);										//����item�ĸ߶�
	void Sort(int nByKind);													//����-����ָ����ʽ
	void HidetheInnerEdit(void);											//�������������ڲ�edit��



	//
	//�ڲ�ʵ�ֹ�����
	//
public:
	CSkinEditListCtrl();
	virtual ~CSkinEditListCtrl();


	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();													//����ʼ��
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//��Ϣ�ص�--���ӿؼ�����ʱ�򴫸���obj,����ʵ�ֵ�ʱ����������
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//�����ӿ�--����COM��ѯ�ӿ�,
	virtual BOOL OnTimer();													//�Զ����ö�ʱ��--�鿴�Ƿ���Ҫ�ػ�

	virtual BOOL OnKey(WORD nChar);											//���̰���
	virtual BOOL OnMouseMoveIn(short x, short y);							//��괦����Ϣ-�ƽ��ؼ�
	virtual BOOL OnMouseMoveOut();											//��괦����Ϣ-�Ƴ��ؼ�
	virtual BOOL OnMouseDown(short x, short y);								//��괦����Ϣ-����
	virtual BOOL OnMouseUp(short x, short y);								//��괦����Ϣ-�ͷ�
	virtual BOOL OnMouseDrag(short x, short y);								//��괦����Ϣ-�϶�
	virtual BOOL OnMouseMove(short x, short y);								//��괦����Ϣ-�ƶ�
	virtual BOOL OnRButtonUp(short x, short y);								//��괦����Ϣ-�Ҽ��ͷ�
	virtual BOOL OnRButtonDown(short x, short y);							//��괦����Ϣ-�Ҽ�����
	virtual BOOL OnDblClick(short x, short y);								//��괦����Ϣ-���˫��
	virtual BOOL OnWheel(short nStep);										//��괦����Ϣ-���ֹ���
	virtual BOOL Layout(short nWidth, short nHeight);						//����չ�ֿؼ�--�ú����ɸı䴰�ڴ�Сλ�õ�ʱ�����WM_SIZE
	virtual void SetState(skin_state state);								//���ÿؼ�״̬


	void Paint(Graphics *pGraph);											//���ƿؼ�
	void DrawItem(Graphics *pGraph, int nItem);								//����ĳһ��item
	void DrawListHeader(Graphics *pGraph);									//�����б�ͷ
	void GetItemRect(int nItem, Rect & rtItem);								//��ȡitem������
	void OnCtrlScroll(long lPos);											//������������
	void RefreshScrollCtrl();												//��������scroll�ؼ�����
	void RefreshCtrlLayOut(void);											//ˢ�¿ؼ��Ű�
	BOOL CheckVerScroll(const short x, const short y);						//��������Ƿ���Scroll��
	BOOL CheckSpliter(const short x, const short y);						//��������Ƿ���Spliter��
	BOOL CheckCursor(short x, short y);										//��������Ƿ��ڿؼ���
	BOOL IsPointInRect(Rect rt, int x, int y);								//��������Ƿ��ھ�����
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//��XML�ļ�����Ƥ��Ԫ�ص�����
	void ModifyItemWork(void);												//�����޸�item�Ĺ���


	void sort_by_name(void);												//����--��������


	//
	//��Ա����
	//
private:

	std::vector<EDIT_LIST_ITEM_INFO*> m_vtItemList;					//����ʾ���������������

	MULTI_LIST_MSG  m_mouseMSG;										//����������෢������Ϣ,����������Ϣ

	CRITICAL_SECTION m_rsLock;										//��Դ��-���ڷ�ֹ���߳���ɾ����������ƹ��̵ĳ�ͻ
	CXMLElement		m_xmlItemStyle;									//Item ��ģ��
	CSkinScroll*	m_pScroll;										//�������ӿؼ�
	CSkinEdit*		m_pEditCtrl;									//�༭���ӿؼ�	
	CSkinEdit*		m_pEditCtrlReName;								//�༭���ӿؼ�-������	

	long m_nTotalHeight;											//Item���ܸ�
	BOOL m_bShowScroll;												//�Ƿ���Ҫ��ʾScroll
	BOOL m_bInscroll;												//����Ƿ���scroll��
	long m_lPageViewPos;											//��ǰ��ʾҳ������ܶ�����ƫ��(Scroll��ƫ����)
	long m_nScrollWidth;											//Scroll�Ŀ��
	BOOL m_bScrolldown;												//Scroll down
	BOOL m_bDrag;													//������϶�ʱΪ TRUE

	CString m_strFontStyle;											//������ʽ
	int	 m_iNameFontSize;											//�����С -����
	int	 m_iDescFontSize;											//�����С -����
	int	 m_nHeaderTextFontSize;										//�б�ͷ�����С

	Rect m_itemBtRect;												//item��������
	Rect m_itemTextRect;											//item�ı�����


	Color m_clrItemBK_NL;											//������ɫNormal״̬
	Color m_clrItemBK_HV;											//������ɫHover״̬
	Color m_clrItemBK_DN;											//������ɫSelect(down)״̬

	Color m_clrItemText_NL;											//������ɫNormal״̬
	Color m_clrItemText_HV;											//������ɫHover״̬


	Color m_clrHeaderText_NL;										//�б�ͷ����Normal״̬��ɫ
	Color m_clrHeaderText_HV;										//�б�ͷ����Hover״̬��ɫ	

	Bitmap* m_aImg[M_LIST_BMP_MAX_NUM];								//�洢����״̬�Ļ���λͼ

	int  m_nItemHeight;												//item�ĸ߶�
	int  m_nItemCount;												//item����
	int  m_nItemHover;												//��ǰ�����������һ��item
	int  m_nItemSel;												//��ǰѡ����һ��item
	int  m_nTopViewItem;											//����ҳ��,�����Item   
	BOOL m_bShowEditCtrl;											//�Ƿ���ʾ�༭�ؼ�
	BOOL m_bShowEditCtrlRename;										//�Ƿ���ʾ�༭�ؼ�-������
	int  m_nItemEditing;											//��¼���ڱ༭����������
	int  m_nLBDownItem;												// ���������µ�item
	int  m_nLBUpItem;												// �����������item
	int  m_nRBDownItem;												// ����Ҽ����µ�item
	int  m_nRBUpItem;												// ����Ҽ������item

	Rect m_HeaderRect;												//�б�ͷ����
	Rect m_ListRect;												//�б�����

	BOOL m_bHaveHeader;												//�Ƿ����б�ͷ
	BOOL m_bHeaderShowTop;											//�б�ͷ�Ƿ���ʾ�ڶ���/�ײ�
	int  m_nHeaderHeight;											//�б�ͷ�ĸ߶�
	Rect m_HeaderBtRectA;											//�б�ͷ��ť����--A
	Rect m_HeaderBtRectB;											//�б�ͷ��ť����--Сͼ������
	Rect m_HeaderBtRectC;											//�б�ͷ��ť����--������ʾ����
	Rect m_HeaderEditRect;											//�б�ͷ�༭������
	BOOL m_bShowHeaderView;											//��¼��ǰ�Ƿ���ʾͷ��/������

	enum MUL_HEADER_STATE
	{
		HEADER_ST_OUT     = 0,    //������б�ͷ�ⲿ
		HEADER_ST_IN,			  //������б�ͷ�ڲ�
		HEADER_ST_DN,			  //������б�ͷ�ڲ�������갴ť
	};

	MUL_HEADER_STATE  m_nHeaderState;                               //�б�ͷԪ����״̬



};




//////////////////////////////////////////////////////////////////////////////////////
//
// ��ǰ�汾��1.0
// ��    �ߣ���ΰ��
// ������ڣ�
// ����˵������ý����ؼ�,�ؼ��������Ա����ʽչ��,ÿ��itemԪ�ر��m��n�е�����T(mRow,nCol)
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinMediaTableCtrl : public CSkinCtrl, public IQvodCallBack,public QTimer::ITimerEvent	
{
public:

	//
	//�ⲿ�ӿڹ�����
	//

	//ҵ���߼������   
	//IDΪ�ڵ�������Ϣ-�ļ�ID��
	int  GetItem_ID(int nIndex);											//��ȡ�б������ΪnIndex��ID���
	int  GetItembyID(int nID);												//��ȡ�ڵ�����nID�����б�nIndexλ�� 
	int  GetSelectItem_ID(int nIndex = 0);									//��ȡ��ǰѡ��item��ԭ�Ƚڵ�Ĵ���ʱ���ID���(������0��ʼ)
	int  GetSelectItem_ID(std::vector<int> & vtSelIdList);					//��ȡ��ǰ����ѡ��item��ԭ�Ƚڵ�Ĵ���ʱ���ID���!
	int  GetSelectItem_ID(std::vector<long> & vtSelIdList);					//��ȡ��ǰ����ѡ��item��ԭ�Ƚڵ�Ĵ���ʱ���ID���!
	BOOL RemoveItembyID(int nID);											//ɾ��һ��item����,nIDΪ�ڵ��nID��Ϣ!
	BOOL ModifyItembyID(int nID, MEDIA_TABLE_ITEM *pItemInfo);				//�޸�ָ��item����,nIDΪ�ڵ��nID��Ϣ!
	void SetItemShowStatebyID(int nID, BOOL bShowTime);						//����item����ʾģʽ  ʱ��/����	--���ڼ��ģʽ��Ч,nIDΪ�ڵ��nID��Ϣ!
	void SetItemIconStatebyID(int nID, int  nState);						//����item�����Iconͼ����ʾģʽ,nIDΪ�ڵ��nID��Ϣ!
	BOOL SetSelItembyID(int nID, bool bEmptyAll = false, bool bSelAll = false);//����ָ��itemΪѡ��״̬,nIDΪ�ڵ��nID��Ϣ,-1Ϊ�������ѡ��

	//�������ݲ���
	int  GetItemCount(void);												//��ȡitem����
	BOOL GetItemData(int nIndex, MEDIA_TABLE_ITEM *pItemInfo);				//��ȡָ��item����ź�����
	int  GetItemState(int nIndex);											//��ȡָ��item��״̬!	
	int  GetSelItemCount(void);												//��ȡѡ��item��Ŀ!
	void GetSelectItem(std::vector<UINT> & vSelIndex);						//��ȡ��ǰ����ѡ��item�����
	int  GetSelectItem(void);												//��ȡ��ǰѡ��item�����
	BOOL GetSelectItemData(int & nIndex,MEDIA_TABLE_ITEM *pItemInfo);		//��ȡ��ǰ���ѡ��item����ź�����
	BOOL GetIndexSelectItemData(int nSelected,MEDIA_TABLE_ITEM *pItemInfo);	//��ȡ��ǰ����ѡ��item�еĵ�n��ѡ��item����
	int  GetHoverItem(void);												//��ȡ��ǰ���ͣ��(Hover)��item�����!
	int  GetPageMaxShowItemCount(void);										//��ȡ��ǰ���ڴ�Сÿҳ��������item����

	//�޸����ݲ���
	BOOL InsertItem(MEDIA_TABLE_ITEM *pItemInfo, int nIndex = -1);			//����һ��item���� -1Ϊ���뵽���
	BOOL RemoveItem(int nIndex);											//ɾ��һ��item����
	void RemoveAllItem();													//ɾ��(���)����item����
	BOOL ModifyItem(int nIndex, MEDIA_TABLE_ITEM *pItemInfo);				//�޸�ָ��item����
	BOOL SetSelItem(int nIndex);											//����ָ��itemΪѡ��״̬
	void SetItemHeight(int nHeight);										//����item�ĸ߶�
	void Sort(int nByKind);													//����-����ָ����ʽ
	BOOL MoveItem(std::vector<UINT>& vMoveIndex, int nPos);					//�ƶ�n�ָ����λ����
	BOOL EditItem(int nItem);												//�༭ĳһ�������
	BOOL OverEdit(BOOL bModify);											//�����༭��bModify���Ƿ��޸�����
	CSkinEdit* CreateEdit();												//����edit�ؼ�

	//�����ʾ���ò���
	void SetShowMode(BOOL bSimpleMode);										//���ÿؼ�����ʾģʽ  ��ý��ģʽ/���ģʽ
	BOOL GetShowMode(void);													//��ȡ�ؼ�����ʾģʽ  ��ý��ģʽ/���ģʽ
	void SetShowProgress(BOOL bShowProgress);								//���ÿؼ��Ƿ���ʾ������
	BOOL GetShowProgress(void);												//��ȡ�ؼ��Ƿ���ʾ������
	void SetItemShowState(int nItem, BOOL bShowTime);						//����item����ʾģʽ  ʱ��/����	--���ڼ��ģʽ��Ч
	BOOL GetItemShowState(int nItem);										//��ȡitem����ʾģʽ			--���ڼ��ģʽ��Ч
	void SetItemPgShowState(int nItem, BOOL bShowGrayPg);					//����item�Ľ������Ƿ���ʾ�һ�״̬
	BOOL GetItemPgShowState(int nItem);										//��ȡitem�Ľ������Ƿ���ʾ�һ�״̬ 
	void SetItemHalfTrans(int nItem, BOOL HalfTrans);						//����item��͸
	void GetPreviewImgSize(int & nWdith, int & nHeight);					//��ȡԤ��ͼ�ĳߴ�
	void SetItemShortCutImg(int nItem, Image * pImgBmp);					//����ָ��item������ͼ	
	void SetNameAlignment(StringAlignment strAlignment);					//����item�����ֶ�������
	void SetChangeImgTime(int nElapse);										//���ø�������ͼ��ʱ�� ��λ΢��
	int  GetChangeImgTime(void);											//��ȡ��������ͼ��ʱ�� ��λ΢��
	void SetReportItemFlag(BOOL bOpen);										//���ÿ���״̬ - ���浱ǰҳ����ʾitem����
	BOOL GetReportItemFlag(void);											//��ȡ����״̬ - ���浱ǰҳ����ʾitem����
	void SetRedraw(BOOL bDraw);												//�����ػ濪��
	BOOL GetRedraw(void);													//��ȡ�ػ濪��
	void SetDragSort(BOOL bDragSort);
	void EnableMoveText(BOOL bEnable);
	void Seek(int index);
	//
	//�ڲ�ʵ�ֹ�����
	//
public:
	CSkinMediaTableCtrl();
	virtual ~CSkinMediaTableCtrl();

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();													//����ʼ��
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//��Ϣ�ص�--���ӿؼ�����ʱ�򴫸���obj,����ʵ�ֵ�ʱ����������
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//�����ӿ�--����COM��ѯ�ӿ�,
	virtual BOOL OnTimer();													//�Զ����ö�ʱ��--�鿴�Ƿ���Ҫ�ػ�

	virtual BOOL OnKey(WORD nChar);											// ������Ϣ
	virtual BOOL OnMouseMoveIn(short x, short y);							//��괦����Ϣ-�ƽ��ؼ�
	virtual BOOL OnMouseMoveOut();											//��괦����Ϣ-�Ƴ��ؼ�
	virtual BOOL OnMouseDown(short x, short y);								//��괦����Ϣ-����
	virtual BOOL OnMouseUp(short x, short y);								//��괦����Ϣ-�ͷ�
	virtual BOOL OnMouseDrag(short x, short y);								//��괦����Ϣ-�϶�
	virtual BOOL OnMouseMove(short x, short y);								//��괦����Ϣ-�ƶ�
	virtual BOOL OnRButtonUp(short x, short y);								//��괦����Ϣ-�Ҽ��ͷ�
	virtual BOOL OnRButtonDown(short x, short y);							//��괦����Ϣ-�Ҽ�����
	virtual BOOL OnDblClick(short x, short y);								//��괦����Ϣ-���˫��
	virtual BOOL OnWheel(short nStep);										//��괦����Ϣ-���ֹ���
	virtual	long OnDrag(UINT uMsg, long wparam, long lparam);				//�϶���Ϣ����
	virtual BOOL Layout(short nWidth, short nHeight);						//����չ�ֿؼ�--�ú����ɸı䴰�ڴ�Сλ�õ�ʱ�����WM_SIZE

	void Paint(Graphics *pGraph);											//���ƿؼ�
	void Paint_SimpleMode(Graphics *pGraph);								//���ƿؼ� ����ģʽ
	void Paint_MultiMode(Graphics *pGraph);									//���ƿؼ� ��ý��ģʽ

	void DrawItem(Graphics *pGraph, int nItem);								//����ĳһ��item
	void DrawItem_SimpleMode(Graphics *pGraph, int nItem);					//����ĳһ��item
	void DrawItem_FlatStyle(Graphics *pGraph, int nItem);					//����ĳһ��item
	void DrawItem_FlatStyle(Graphics *pGraph, int nItem, Rect& itemRect);
	void DrawItem_WaveStyle(Graphics *pGraph, int nItem);					//����ĳһ��item
	void DrawItem_FrameStyle(Graphics *pGraph, int nItem);					//����ĳһ��item
	void DrawListHeader(Graphics *pGraph);									//�����б�ͷ
	void Draw_Progress(Graphics *pGraph, int nItem, Rect & pgRect);			 //����ĳһ��item������
	void GetItemRect(int nItem, Rect & rtItem);								//��ȡitem������
	void OnCtrlScroll(long lPos);											//������������
	void RefreshScrollCtrl();												//��������scroll�ؼ�����
	void RefreshAllItem_VitualPos(void);									//ˢ�µ�ǰ�ṹ�����нڵ����ʾ����λ��
	void ReLayOutTheCtrlbyFix(void);										//��������layout
	BOOL CheckVerScroll(const short x, const short y);						//��������Ƿ���Scroll��
	BOOL CheckSpliter(const short x, const short y);						//��������Ƿ���Spliter��
	BOOL CheckCursor(short x, short y);										//��������Ƿ��ڿؼ���
	BOOL IsPointInRect(Rect rt, int x, int y);								//��������Ƿ��ھ�����
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//��XML�ļ�����Ƥ��Ԫ�ص�����
	BOOL IsPointInItemArea(short x, short y);								//�ж������Ƿ�������������
	BOOL IsRectInFrame(Rect rtRect, Rect rtFrame);							//�жϾ����Ƿ��ڿ����
	BOOL IsRectHeavyInFrame(Rect rtRect, Rect rtFrame);						//�жϾ��ε����������Ƿ��ڿ����
	BOOL IsRectMidPointInFrame(Rect rtRect, Rect rtFrame);					//�жϾ��ε������Ƿ������н������
	BOOL IsFrameInRect(Rect rtRect, Rect rtFrame);							//�жϿ���Ƿ��ھ�����
	void  CtrlPosCalculate(void);											//����Ԫ�����Ե���Ӧλ��
	void OnLBtnClickItem(int nClickItem);									//�����ĳһitem

	void sort_by_ID(void);													//����--����ID
	void sort_by_Progress(void);											//����--���ݽ���
	void sort_by_NetQuality(void);											//����--��������
	void sort_by_Time(void);												//����--���ݲ���ʱ��
	void ReportPageItemShow(int nLower, int nHigher);						//���浱ǰҳ����ʾitem����
	void RequestChangeMovieImg(void);										//�������ĳitem������ͼ

	CString m_strMovingText;  //��ǰ������������
	int m_nstrMovingTextX ;   // ��������
	BOOL  m_bFirstDrawTextBmp;

	BOOL m_bsActiveMovingText;										//�Ƿ񼤻��������
	BOOL m_bsTimerWorkDirty;										//�Ƿ�ʱ���Ѵ�����Ҫ����(�ػ�)


	//////////////////////////////////////////////////////////////////////////
	// ��ʱ������
	enum
	{
		kTimerMovingText = 1,
		kTimerChangeImg,
		kTimerRollImag,
		kTimerDropUp,
		kTimerDropDown,
	};
	virtual void OnTimer(UINT uTimerID);
	void OnTimerMovingText();
	void OnTimerChangeImg();
	void OnTimerRollImag();
	void OnTimerDropUp();
	void OnTimerDropDown();
	void DragMove(short x, short y);

	//static void CALLBACK MovingTextTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime); //�������ֵĶ�ʱ��
	//static void CALLBACK ChangeImgTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);  //������ͼ�Ķ�ʱ��
	void ChangeMovingText(CString strNewText);		//�ı������������
	//static void CALLBACK RollImgTimerCallBackFun(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);// ��������ͼ��ʱ�ص�����
	void SetRollImageTimer(UINT uRollElapse = 750);	// ���ù�����ʱ��
	void KillRollImageTimer();						// ȡ��������ʱ��

	const Rect &GetImageRect()const;				// ��������ͼλ��
	void GetSkinSnaps();

	//���ù������Ŀ���״̬
	void VisualizeScrollCtrl(void);

	//����item��Ŀ���¼���߶�
	int GetItemTotalHeight(void);

	//����Ԫ�ض�Ӧ��ý���ļ��Ƿ����
	void SetItemFileExist(int index, BOOL bExist);
	void SetItemFileCanPlay(int nID, BOOL bPlay );
	void ViewDataDone(void); // ���ؼ����ݸ�����Ϻ󣬻ص��ú���
	void CurrViewGone(void); // ��ǰҳ����ʧ�����
	void CheckItemSel(void); // �������Ԫ��ѡ��״̬
	BOOL OnDirectionKey(WORD key);

	//�϶����ƴ���
	void DoDragDrop(int xDragImgMove, int yDragImgMove);
	SIZE DrawDragBmp(HBITMAP& hBitmap);
	void GetDestItem(int x, int y, int& nInsertItem, int& nEnterItem);
	void MouseDragMoveItem(int nDestItem);
	BOOL GetDragList(std::vector<CString> & vDragList);
	void EnableDrawSelCaptain(BOOL bEnable){m_bDrawSelCaptain = bEnable;}

	// DragDrop���
	virtual BOOL DragMouseIn(short x, short y);
	virtual BOOL DragMouseOut();
	virtual BOOL DragMouseMove(short x, short y);
	virtual BOOL DragMouseUp(short x, short y);
	//
	//��Ա����
	//
private:

	std::vector<MEDIA_TABLE_ITEM*> m_vtItemList;					//����ʾ���������������

	MULTI_LIST_MSG  m_mouseMSG;										//����������෢������Ϣ,����������Ϣ

	CXMLElement		m_xmlItemStyle;									//Item ��ģ��
	CSkinScroll*	m_pScroll;										//�������ӿؼ�		

	CRITICAL_SECTION m_rsLock;										//��Դ��-���ڷ�ֹ���߳���ɾ����������ƹ��̵ĳ�ͻ

	long m_nTotalHeight;											//Item���ܸ�
	BOOL m_bShowScroll;												//�Ƿ���Ҫ��ʾScroll
	BOOL m_bInscroll;												//����Ƿ���scroll��
	long m_lPageViewPos;											//��ǰ��ʾҳ������ܶ�����ƫ��(Scroll��ƫ����)
	long m_nScrollWidth;											//Scroll�Ŀ��
	BOOL m_bScrolldown;												//Scroll down
	BOOL m_bDrag;													//������϶�ʱΪ TRUE
	BOOL m_bRButtonDown;											//����Ҽ��Ƿ�Ϊ����״̬!
	BOOL m_bLBDbClickDown;											//���˫���Ƿ�Ϊ����״̬!

	CString m_nameFace;
	int	 m_iNameFontSize;											//�����С -����
	int	 m_iTimeFontSize;											//�����С -ʱ��

	Rect m_TranscodeRC;												// ת������
	Rect m_btRect;													//��������
	Rect m_btSimpleRect;											//��������
	Rect m_textNameRect;											//�ı�����
	Rect m_textTimeRect;											//�ı�����
	Rect m_textTimeSimpleRect;										//�ı�����
	Rect m_pgRect;													//����������
	Rect m_pgSimpleRect;											//����������
	Rect m_imgRect;													//ͼƬ����
	Rect m_imgHVRect;												//ͼƬ����hover״̬

	Rect m_rtDragFrame;												//�����ѡʱ����ƵĿ�!
	CPoint m_ptDragStart;										    //��¼�����ѡʱ����Ƶİ������!
	CPoint m_ptDragEnd;										        //��¼�����ѡʱ����Ƶĵ����յ�!

	Color m_clrItemBK_NL;											//������ɫNormal״̬
	Color m_clrItemBK_HV;											//������ɫHover״̬
	Color m_clrItemBK_DN;											//������ɫSelect(down)״̬

	Color m_clrNameText_NL;											//������ɫNormal״̬
	Color m_clrNameText_HV;											//������ɫHover״̬
	Color m_clrNameText_DN;											//������ɫDown״̬
	Color m_clrTimeText_NL;											//������ɫNormal״̬
	Color m_clrTimeText_HV;											//������ɫHover״̬
	Color m_clrTimeText_DN;											//������ɫDown״̬

	Color m_clrDragFrame;											//��ק����ɫ!
	Color m_clrDragFrameBorder;										//��ק��ߵ���ɫ!
	Bitmap * m_aImg[M_LIST_BMP_MAX_NUM];							//�洢����״̬�Ļ���λͼ
	CString m_textFontStyle;										//������ʽ!
	CString m_textTimeFontStyle;									//ʱ��������ʽ!

	Image *m_pFrontImgOfRoll;										// ������ʾ����ͼ�е�ǰ��һ��ͼ
	SHORT  m_assWidthOfFrontImg;									// ������ʾ����ͼ�е�ǰ��һ��ͼ����ʾ���
	USHORT m_distancePerRoll;										// ������ʾ����ͼ��ÿ�ι��������ؾ���
	USHORT m_timeGapOfRoll;											// ������ʾ����ͼ��ÿ�ι�����ʱ����
	StringAlignment m_aNameAlignment;								// ���ֶ�������

	int   m_nItemHeight;											//item�ĸ߶�-��ǰģʽ
	int   m_nItemSimpleHeight;										//item�ĸ߶�-���ģʽ
	int   m_nItemMultiHeight;										//item�ĸ߶�-��ý��ģʽ
	int   m_nItemMultiWidth;										//item�Ŀ��-��ý��ģʽ

	int   m_nBorderTopSpace;										//item������Ե�ռ�
	int   m_nBorderLeftSpace;										//item��߱�Ե�ռ�
	int   m_nRowSpace;												//item֮���м���ռ�
	int   m_nMinColSpace;											//item֮������С�ļ���ռ�
	int   m_nColSpace;												//item֮���м���ռ�

	int   m_nItemCount;												//item����
	int   m_nItemHover;												//��ǰ�����������һ��item
	int   m_nItemSel;												//��ǰѡ����һ��item
	int   m_nItemDown;												//��ǰ��갴������һ��item
	int   m_nItemSelCount;											//��ǰѡ��item����Ŀ!
	int   m_nItemDragFrameDown;										//��ѡʱ���°�ť���ڵ�ѡ��!
	int   m_nIDSelCaptain;											// ��ǰѡ��׼�ǵ�ID

	Rect  m_HeaderRect;												//�б�ͷ����
	Rect  m_ListRect;												//�б�����

	int   m_nItemDrawStyle;											//item������ʽ
	BOOL  m_bHaveHeader;											//�Ƿ����б�ͷ
	int   m_nHeaderHeight;											//�б�ͷ�ĸ߶�
	int   m_nSimpleHeaderHeight;									//���ģʽ�б�ͷ�ĸ߶�


	BOOL  m_bShowSimpleMode;										//�ؼ���ģʽ�Ƿ�Ϊ���ģʽ ����Ϊ��ý��ģʽ
	BOOL  m_bShowTimeState;											//item��ʾģʽ�Ƿ�Ϊʱ�� ����Ϊ ����--���ڼ��ģʽ��Ч
	BOOL  m_bShowPgMode;											//item��ʾģʽ�Ƿ���ʾ������

	BOOL  m_bShowDragFrame;											//�Ƿ��������קʱ��ʾ��!
	BOOL  m_bDragWay_byArea;										//�Ƿ��ѡ������������������������-����Ϊ��ǰ�Ƿ��Ѿ�ѡ��!
	int m_nChangeImgSlice;											//���ĸ�������ͼ��ʱ��Ƭ��(��1��paint��OnTimer()���ü��Ϊ1����λ)
	int m_nPointInElement;											//�������ѡ�����Ԫ������
	int m_nMovingTextSlice;											//hover״̬�����ֹ��������ֵ�ʱ��Ƭ��(��1��paint��OnTimer()���ü��Ϊ1����λ)
	int m_nItemHoverTextMove;										//hover״̬�����ֹ����������ڵڼ���
	int m_nPerPageRowNum;											//ÿҳ�������ɵ�����
	int m_nPerPageColNum;											//ÿҳ�������ɵ�����
	int m_nTotalRowNum;												//�ܹ�ռ�ݵ�����
	int m_nTopViewRow;												//����ҳ��,���������   
	// ���� layout �����ˢ�� m_nTopViewRow
	BOOL m_blayoutRefreshTopViewRow;
	BOOL m_bOpenReportItemFlag;										//�Ƿ������浱ǰҳ����ʾitem�����
	int m_nOldShowLower;											//��ǰ��ʾ���-��������״̬
	int m_nOldShowHigher;											//��ǰ��ʾ�����-��������״̬
	int m_nOldShowLowerID;											//��ǰ��ʾ����ļ�ID-��������״̬
	int m_nOldShowHigherID;											//��ǰ��ʾ������ļ�ID-��������״̬
	BOOL m_bRedraw;													//ͼ�����ʱ�Ƿ���ƿؼ�

	
	int	  m_nEditItem;												//�༭�����ڵ���
	BOOL  m_bEditShow;												//�༭���Ƿ�����ʾ
	BOOL  m_bClickTwice;											//�Ƿ����ε����ͬһ����
	CSkinEdit*		m_pEdit;										//�༭��ؼ�
	//ui::drop::CDragDropCtrl	m_DragDropCtrl;									//��ק�ؼ�
	HBITMAP			m_hDragBitmap;									//��קλͼ
	BOOL			m_bDragSort;									//�Ƿ�������ק����
	int				m_nDragInsertPos;								//��ק����Ĳ���λ��
	int				m_nDragRow;										//��קʱ�����������		
	std::vector<CString> m_vtDragList;								//��ק�ļ�/�ļ������Ƶ�������
	BOOL            m_bDrawSelCaptain; // �Ƿ����SelCaptain	
	BOOL			m_bEnableMoveText;
	QTimer::CTimer m_timer;
	int m_lastMouseUpID;

	enum MUL_HEADER_STATE
	{
		HEADER_ST_OUT     = 0,    //������б�ͷ�ⲿ
		HEADER_ST_IN,			  //������б�ͷ�ڲ�������������ť����
		HEADER_ST_BTA_HV,		  //������б�ͷ��ťA����  Hover	
		HEADER_ST_BTA_DN, 		  //������б�ͷ��ťA����  Down 
		HEADER_ST_BTB_HV, 		  //������б�ͷ��ťB����  Hover 
		HEADER_ST_BTB_DN,  		  //������б�ͷ��ťA����  Down  	
		HEADER_ST_BTC_HV, 		  //������б�ͷ��ťC����  Hover
		HEADER_ST_BTC_DN,  		  //������б�ͷ��ťA����  Down  
	};

	MUL_HEADER_STATE  m_nHeaderState;                               //�б�ͷԪ����״̬
};


#define MFP_FIRST 1
#define MFP_LAST  2

class CSkinColorMenu//:public CMenu
{
public:
	CSkinColorMenu();
	~CSkinColorMenu();

	/*************��Ϣ��Ӧ����***************/

	//WM_INITMENUPOPUP����Ӧ������������һЩ��ʼ������
	LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

	//WM_MEASUREITEM����Ӧ����������ָ���˵����С
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

	//WM_DRAWITEM����Ӧ�����������Ի�
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnUnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	/***************************************/

public:
	//���ܣ����ò˵���ͼƬ
	
	BOOL SetMenuItemImage(HMENU hMenu, UINT item, Image* pImg, BOOL bByPos = FALSE);
	
	//���ܣ����ò˵����ı�
	BOOL SetMenuItemText(HMENU hMenu, UINT item, LPCTSTR sItemName, BOOL bByPos = FALSE);

	//���ܣ����ò˵����ݼ�
// 	BOOL SetMenuItemAcc(HMENU hMenu, UINT item, LPCTSTR sAccName, BOOL bByPos = FALSE);

	//���ܣ����ò˵���Ŀ�ݼ�
	BOOL SetAllMenuItemAcc(HMENU hMenu/*, std::map<UINT, ACCEL>& mAccel*/);

	//���ܣ����˵�����Ϊ����
	//hMenuΪ�˵������ڵ����˵����
	//uItemΪ�˵���������ID�ţ��ɵ�3����������
	//����ID�Ż��������ŷ��ʣ���Ҫ����һ�������˵��������������ŷ��ʣ�
	BOOL SetMenuItemHide(HMENU hMenu, UINT uItem, BOOL bByPos = FALSE);

	BOOL RemoveMenu(HMENU hMenu, UINT uItem, UINT uFlags);
	
	//���ܣ���ʾ�˵�  
	//hwnd:��ʾ���ĸ�����֮��     index:��ʾ�ڼ��������˵�
// 	BOOL DisplayMenu(HWND hwnd, short x, short y, UINT index = 0);

// 	//���ܣ���չ�˵�
// 	BOOL ExpandMenu();

	//���ܣ����ٲ˵�
	BOOL DestroyMenu(HMENU hMenu);

	//����: ���ز˵�����
	//LPCTSTR lpSkinName: Ƥ����
	//LPCTSTR lpStyleName: �˵������
	BOOL ResetSkin(LPCTSTR lpSkinName, LPCTSTR lpStyleName);

protected:
	//Ϊ��ȥ���˵��ı߿��Լ����Բ�ǲ˵������ù����滻���ڹ��̺���
	static LRESULT CALLBACK WindowsHook (int code, WPARAM wParam, LPARAM lParam);
	//���ڹ���
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT MenuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//��Բ�Ǿ���
	static void MakeRoundRect(GraphicsPath &gdiPath, Point topLeft, Point bottomRight, Point round);
	//����HSL
//	Color AdjustHSL(float kH, float kS, float kL, short bH, short bS, short bL, Color color);

	BOOL IsLongMenuInXP(int nHeight);

protected:
	typedef	struct 	            // �˵�����Ϣ
	{
		LPTSTR lpstrText;
		LPTSTR lpstrAcc;
		UINT fType;
		UINT fState;
		int nPos;
		BOOL bHide;
		Image* pImage;
	}MenuItemData, *LpMenuItemData;

	typedef	struct 	            // �˵�����Ϣ
	{
		Bitmap* pSelectBgImage;
	}MenuDrawData, *LpMenuDrawData;
	
	typedef struct _RGB_TYPE
	{
		BYTE Red;
		BYTE Green;
		BYTE Blue;
	}RGB_TYPE;

	typedef struct _HSL_TYPE
	{
		float Hub;
		float Saturation;
		float lightness;
	}HSL_TYPE;

protected:
	void DestroyMenu_(HMENU hMenu);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct);	
	LRESULT InitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	static void RGB2HSL(/*[in]*/const RGB_TYPE *rgb, /*[out]*/HSL_TYPE *hsl);
// 	static void HSL2RGB(const HSL_TYPE *hsl, RGB_TYPE *rgb);
	BOOL LoadFromXML(CXMLElement& xmlViews, LPCTSTR strSkinPath);
private:
	
	HHOOK m_hMenuHook;
//	Color g_Color;
	Bitmap* m_pMemBmp;					//�˵��ڴ�λͼ

	BYTE  m_nAlpha;						//�˵�͸����
//	float m_lBackAlpha;					//��߱�����͸����
//	float m_rBackAlpha;					//�ұ߱�����͸����
//	float m_textAlpha;					//�ֵ�͸���ȣ�ѡ��ʱ��

//	HWND m_currentHwnd;					//��ǰ���ڴ���
//	Point m_currentPoint;				//��ǰ����
//	UINT m_currentID;					//��ǰ���صĲ˵�ID
//	UINT m_currentIndex;				//��ǰ��ʾ�����˵�������
//	UINT m_nCurDrawItem;				//��ǰ���ڻ��Ĳ˵���λ��
	int m_nScreenHeight;				//��Ļ�߶�
//	int m_nDelayTime;					//�Ӳ˵��ӳٵ�����ʱ�䣨ms��
	int m_nTmpItemHeight;
	BOOL m_bSetAnimation;				//�û��Ƿ������ˡ����뵭����ʾ�˵���
	BOOL m_bDropShadow;					//�Ƿ������˲˵���Ӱ
	BOOL m_bDoubleBuffer;				//�Ƿ����˫����
//	BOOL m_bLongMenu;					//�Ƿ��ǳ��˵�
//	BOOL m_bPopParent;
//	BOOL m_bPaintByPull;
//	std::map<HWND, MenuDrawData> m_mDrawMap;

//	GraphicsPath* m_pWndPath;

	CString m_strSkinName;				//��ǰƤ����
	CString m_strStyleName;				//��ǰ�˵���ʽ(һ��Ƥ�������ж��ֲ˵���ʽ��

	//���²����������ļ���ȡ
	//color
	Color m_cLeftBgColor;				//�˵���߱�����ɫ
	Color m_cRightBgColor;				//�˵��ұ߱�����ɫ
	Color m_cEdgeColor;					//�˵��߿���ɫ
	Color m_cSelectFontColor;			//�˵�ѡ��ʱ������ɫ
	Color m_cGrayFontColor;				//�˵�����ʱ������ɫ
	Color m_cNormalFontColor;			//�˵���ͨ������ɫ
	//Color m_menuSelectColor1;			//�˵�ѡ��ʱ����ɫ
	//Color m_menuSelectColor2;			//�˵�ѡ��ʱ����ɫ
	//Color m_menuSeparatorColor1;		//�˵��ָ�����ɫ���
	//Color m_menuSeparatorColor2;		//�˵��ָ�����ɫ��ǳ��

	//bitmap
	Bitmap* m_pImgTick;					//��
	Bitmap* m_pImgPoint;				//��
	Bitmap* m_pImgDown;					//���µļ�ͷ
	Bitmap* m_pImgRight;				//���ҵļ�ͷ
	Bitmap* m_pSelectBar;				//ѡ����
	Bitmap*	m_pSeparator;				//�ָ���

	//size
	UINT m_cySpacing;					//�˵����ָߺͲ˵���ߵļ��  (�˵���߶� = �ָ߶� + m_cySpacing*2��
	UINT m_cxMidSpacing;				//�˵������Ϳ�ݼ�֮�����̼��
	UINT m_nLeftWidth;					//�˵���߿��   ���˵����� = �ֿ�� + m_nLeftWidth + m_cxMidSpacing��
	UINT m_nTopSpacing;					//�˵�������϶
	UINT m_nBottomSpacing;				//�˵��ײ���϶
	UINT m_nSelectBarX;					//ѡ���ĺ����꣨����ڲ˵��
	UINT m_nSelectBarY;					//ѡ���������꣨����ڲ˵��
	int m_nMenuRadian;					//�˵���Բ�ǻ���
	
	//font
	Font*  m_pMenuFont;					//�˵�����

public:

	/*********����CColorMenuBar�ı���***********/
//	BOOL m_bMenuClosed;					//�˵��Ƿ��ѹر�
	BOOL m_bHasChild;					//ѡ�еĲ˵����ǵ����˵�
//	HMENU m_currentMenu;				//ѡ�в˵�������ڲ˵����
	UINT m_lastItemID;					//�˵��˳�ǰ���һ����ѡ�еĲ˵���ID
//	HWND m_hWnd;

};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�
* ����˵���������Ƿָ���
********************/
class CSkinSplitter : public CSkinImage	// �ָ�����
{
public:
	CSkinSplitter();
	virtual ~CSkinSplitter();

	BOOL CheckCursor(short x, short y);

	virtual BOOL OnMouseMoveIn(short x, short y);
	virtual BOOL OnMouseMoveOut();
	virtual BOOL OnMouseDown(short x, short y);
	virtual BOOL OnMouseUp(short x, short y);
	virtual BOOL OnMouseDrag(short x, short y);
	virtual BOOL OnMouseMove(short x, short y);
	virtual BOOL OnTimer();

private:
	long m_oldPos;
	long m_dragPos;
	BOOL m_bDrag;
};

class CSkinDisplayBox : public CSkinCtrl, public IQvodCallBack	// ���й�������������ʾ��
{
public:
	CSkinDisplayBox();
	virtual ~CSkinDisplayBox();

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual BOOL Layout(short nWidth, short nHeight);
	virtual BOOL OnTimer();
	virtual BOOL OnMouseMove(short x, short y);
	virtual BOOL OnMouseMoveIn(short x, short y);
	virtual BOOL OnMouseMoveOut();
	virtual BOOL OnMouseDown(short x, short y);
	virtual BOOL OnMouseUp(short x, short y);
	virtual BOOL OnMouseDrag(short x, short y);
	virtual BOOL OnWheel(short nStep);
	virtual void Clear_();

	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);
	

	void ResetScroll();
	void Paint(Graphics *pGraph);
	ISkin * GetSkinObj(short x, short y);
	BOOL CheckCursor(short x, short y);

	void SetFileConnState(std::map<CString, BOOL> &fileconn, BOOL &bAll);

	std::map<CString, BOOL> m_fileConn;
	std::vector<CSkinButton_Check*> m_pvChangGui;	//��ų����ļ�
	std::vector<CSkinButton_Check*> m_pvMepg;		//���Mepg
	std::vector<CSkinButton_Check*> m_pvReal;		//���Real
	std::vector<CSkinButton_Check*> m_pvQuickTime;	//���QuickTime
	std::vector<CSkinButton_Check*> m_pvWindows;	//���Windows
	std::vector<CSkinButton_Check*> m_pvQTVideo;	//���������Ƶ

	std::vector<CSkinButton_Check*> m_pvCJAudio;	//��ų�����Ƶ
	std::vector<CSkinButton_Check*> m_pvQTAudio;	//���������Ƶ
	std::vector<CSkinButton_Check*> m_pvFlash;		//���Shockwa Flash
	std::vector<CSkinButton_Check*> m_pvPlayFileList;	//��Ų����б��ļ�
//	std::vector<CSkinButton_Check*> m_pvIMGFile;	//���ͼƬ�ļ�
	std::vector<CSkinButton_Check*> m_pvBtFile;		//���BT�����ļ�

public:
	CSkinScroll *m_pScroll;
	BOOL m_hasDown;
	int m_totalHeight; //����ʾ�����иߣ����û�У����ǺͿ����Ľ���һ����
	int m_offpos;
	BOOL m_showScroll;
	CSkinCtrl *m_pOldObj;
	CSkinCtrl *m_pDownObj;
	BOOL m_bSkinObjConstructOK;
};


/*************************************************
Class Name: CDropList
Author:Xiacc  Date: 2010-3-1
Description: �Ի�����CListBox����CSkinCombobox���ϣ��γ��Ի�����CComboBox
Others:      CDropList��ʾ��CWinDropList������
Function List:  ��Ҫ����ʹ��ǰ����SetComboboxParent, SetColor, SetTextFont
*************************************************/

class CSkinCombobox;
class CDropList : public CWindowImpl<CDropList, CListBox>
{
public:
	BEGIN_MSG_MAP(CDropList)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	END_MSG_MAP()

	CDropList();
	~CDropList();
	//
	//���ܽӿ�
	//	
	int SetItemHeight(int nIndex, UINT cyItemHeight = 16);
	void SetComboboxParent(void *hParent);
	void SetColor(COLORREF crBkColor, COLORREF crTextColor, COLORREF crBorderColor);
	void SetColor(COLORREF clrBK, COLORREF clrText, COLORREF clrHoverText,
		          COLORREF clrItemNormal, COLORREF clrItemHover, COLORREF clrNormalFrame);     //������ɫ����
	void SetTextFont(CString face, UINT size, int style);
	BOOL SetIndex(UINT nIndex);	//����ָ������±�
	BOOL			m_bFirstShow;		//�Ƿ�Ϊ��һ����ʾ
protected:

	LRESULT OnDrawItem(UINT    uMsg, WPARAM wp, LPARAM lp, BOOL &);						       //����item������
	LRESULT OnMouseMove(UINT   uMsg, WPARAM wp, LPARAM lp, BOOL &);						       //����ƶ�����
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);						       //���������������
	LRESULT OnKillFocus(UINT   uMsg, WPARAM wp, LPARAM lp, BOOL &);						       //KillFocus������
	void DrawOneItem(int nIndex, COLORREF crBkColor, COLORREF crTextColor);					   //ֻ��һ��item������

	UINT			m_nHight;			//ÿ��ĸ߶�
	UINT			m_nMouseIndex;		//��ǰ���λ��������
	UINT			m_nIndex;			//��ǰָ������±�
	COLORREF		m_clrBK;		    //������ɫ
	COLORREF		m_clrText;		    //������ɫ
	COLORREF		m_clrHoverText;		//������ɫ
	COLORREF		m_clrItemNormal;	//�߿���ɫ
	COLORREF		m_clrItemHover;		//�߿���ɫ
	COLORREF		m_clrNormalFrame;	//�߿���ɫ

	CSkinCombobox	*m_pComboBox;		//�������CSkinComboBox
	//
	CString			m_face;				//����
	float			m_size;				//�ִ�С
	int				m_style;			//�ַ��
};

//////////////////////////////////////////////////////////////////////////////////////
// ������  : CWinDropList
// ��ǰ�汾: 1.0
// ��    ��: Xiacc, �޸�by��ΰ��
// �������: 
// ����˵��: һ������ʽ�����࣬��CSkinCombobox���ϣ��γ��Ի�����CComboBox
//           ��CSkinCombobox�����Ƿ���ʾCWinDropList����
//////////////////////////////////////////////////////////////////////////////////////
class CWinDropList : public CWindowImpl<CWinDropList>
{
public:
	BEGIN_MSG_MAP(CDropList)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
	END_MSG_MAP()

	LRESULT OnDrawItem(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);	//����item������
};


//////////////////////////////////////////////////////////////////////////////////////
// ������  : CSkinCombobox
// ��ǰ�汾: 1.0
// ��    ��: Xiacc, �޸�by��ΰ��
// �������: 
// ����˵��: ����CWinDropList���CDropList���ϣ��γ��Ի�����CComboBox
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinCombobox : public CSkinCtrl
{
public:

	CSkinCombobox();
	virtual ~CSkinCombobox(){};

	//
	//���ܽӿ�
	//	
	void HideDropList_OutSide();									//����DropList(������ڿؼ��ⲿ)
	void HideDropList_Strong();										//����������DropList
	BOOL SetIndex(int nIndex);										//���±������ ר��CDropList�е��ú��ô˺���Ϊ�ӿڴ��ⲿ��������
	BOOL SetString(LPCTSTR lpszText);								//������ʾ���ı�
	int	 AddString(LPCTSTR lpszItem);								//CDropList�������
	void RemoveAllItem();											//ɾ������item
	BOOL SetItemHeight(int nIndex, UINT nHeight);					//����item�߶�
	int	 GetItemCount();											//��ȡ����item��
	int  GetSelectItemIndex();										//��ȡѡ�е���
	int  SetSelectItemIndex( int index ) ;							//����ѡ�е���
	CString GetItemText(int nIndex);								//��ȡitem���ı�
	void SetColor(COLORREF crBkColor, COLORREF crTextColor, 
		COLORREF crTriColor, COLORREF crBordColor);					//������ɫ����
	long Method_(LPCTSTR szAttrib, long para1, long para2);

	//
	//�ڲ�������
	//
protected:

	virtual BOOL Init_(CXMLElement &xmlNode,		
		               IQvodCallBack *pEventEntry);					//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();											//����ʼ��
	virtual void Paint(Graphics *pGraph);							//���ƺ���
	virtual BOOL OnTimer();											//�Զ����ö�ʱ��--�鿴�Ƿ���Ҫ�ػ�
	virtual BOOL OnMouseMoveOut();									//����Ƴ��ؼ�������
	virtual BOOL OnMouseMoveIn(short x, short y);					//����ƽ��ؼ�������
	virtual BOOL OnMouseDown(short x, short y);						//��갴�´�����
	virtual BOOL OnMouseUp(short x, short y);						//����ͷŴ�����
	virtual BOOL CheckCursor(short x, short y);						//̽�����
	virtual BOOL OnRButtonDown(short x, short y);
											
	//
	//����
	//
private:
	BOOL		m_bExtend;			//CWinDropList�Ƿ�����
	UINT		m_nDropItemNum;		//CDropList���������
	UINT		m_nHeight;			//CDropList��ĸ߶�


	COLORREF	m_clrBK;		    //������ɫ
	COLORREF	m_clrText;		    //������ɫ
	COLORREF	m_clrNormalFrame;	//�߿���ɫ
	COLORREF	m_clrHoverFrame;	//��������ϵ���ɫ


	CString		m_strText;			//��ǰ������
	CString		m_face;				//��������
	float		m_size;				//���ִ�С
	int			m_style;			//���ַ��

	Bitmap *		m_pBtImg;		//��ťͼ��

	CSkinEdit*  m_pEdit;
	CDropList	m_DropList;			//�����б�
	CWinDropList m_WinDropList;		//�����б�
};

#define SC_MSG_ITEM_CLICK  1
#define SC_MSG_ITEM_BTN_CLICK 2
#define SC_MSG_ITEM_HOVER_CHNAGE 3
class CSkinSuperCombox : public CSkinCtrl, public IQvodCallBack
{
public:
	CSkinSuperCombox();
	virtual ~CSkinSuperCombox();

	//����
	int	 GetSel();													//��ȡѡ���������
	int	 GetItemCount();											//��ȡ��������
	CString GetItemText(int nIndex);								//��ȡnIndex���ı�
	CString GetText();
	//����
	int	AddString(LPCTSTR lpszItem);								//����һ��
	int	RemoveItem(int nIndex);										//ɾ��һ��
	void RemoveAllItem();											//ɾ��������
	void ShowDropList(BOOL bShow);									//��ʾ/����������
	BOOL SetSel(int nSel);											//���õ�nSel������Ϊ��ʾ������
	BOOL SetText(LPCTSTR lpszItem);									//����edit������
	HWND SetFocus();												//���ý���
	void SetEditSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
	void GetEditSel(int& nStartChar, int& nEndChar);

	virtual void SetState(skin_state state);
	CSkinEdit* GetEdit(){return m_pEdit;};
	CSkinMultiListCtrl* GetListCtrl(){return m_pList;};
protected:

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);					//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();											//����ʼ��
	virtual void Paint(Graphics *pGraph);							//���ƺ���
	virtual BOOL OnTimer();											//�Զ����ö�ʱ��--�鿴�Ƿ���Ҫ�ػ�
	virtual BOOL OnMouseMove(short x, short y);						//����ƶ�������
	virtual BOOL OnMouseMoveOut();									//����Ƴ��ؼ�������
	virtual BOOL OnMouseMoveIn(short x, short y);					//����ƽ��ؼ�������
	virtual BOOL OnMouseDown(short x, short y);						//��갴�´�����
	virtual BOOL OnMouseUp(short x, short y);						//����ͷŴ�����
	virtual BOOL CheckCursor(short x, short y);						//̽�����
	virtual BOOL Layout(short nWidth, short nHeight);
	virtual BOOL InBtnRect(short x, short y);
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);
	virtual long Method_(LPCTSTR szMethod, long para1, long para2);
	CSkinEdit* CreateEdit(CXMLElement &xmlNode);

	Bitmap *		m_pBtImg;			//��ťͼ��
	Bitmap *		m_pIconImg;			//ͼ�갴Ŧ
	Bitmap *		m_pFrameImg_R;		//�߿�ͼ��
	Bitmap *		m_pFrameImg_M;		//�߿�ͼ��
	Bitmap *		m_pFrameImg_L;		//�߿�ͼ��
	BOOL			m_bBtnOver;			//����Ƿ��ڰ�ť��
	BOOL			m_bListShow;		//�����б��Ƿ�����ʾ״̬
	CString			m_strText;			//��ǰ��ʾ������
	CString			m_face;				//��������
	float			m_size;				//���ִ�С
	int				m_style;			//���ַ��
	int				m_nSel;				//ѡ���������
	int				m_nEditSpace_X;		//edit��x���϶
	int				m_nEditSpace_Y;		//edit��y���϶
	int				m_nBtnWidth;		//button���
	int				m_nBtnHeight;		//button�߶�
	int				m_nListMaxHeight;	//�б�����߿��
	int				m_nListOffset_x;
	int				m_nListOffset_y;
	int				m_nListWidth;
	COLORREF		m_clrBK;		    //������ɫ
	COLORREF		m_clrText;		    //������ɫ
	COLORREF		m_clrNormalFrame;	//�߿���ɫ
	COLORREF		m_clrHoverFrame;	//��������ϵ���ɫ
	CSkinEdit*		m_pEdit;			//edit�ؼ�
	CSkinMultiListCtrl* m_pList;		//list�ؼ�
	CSkinWindow*	m_pListWnd;			//list�ⴰ��
	BOOL			m_bLoginCtrl;		//�Ƿ��ǵ�¼�ؼ�
	BOOL			m_bChildWnd;
	BOOL			m_bEditFocus;
};

class CSkinSelectSuperCombox: public CSkinSuperCombox
{
public:
	CSkinSelectSuperCombox();

protected:
	virtual void Paint(Graphics *pGraph);							//���ƺ���
	void SetState(skin_state state);
private:
	BOOL			m_bShowOverImageEditFocus;	//����༭�����н��㣬Ҳ��ʾ��������ϵı���
};


/*************************************************
Class Name: CSkinTree
Author:Xiacc  Date: 2010-3-1
Description: �̳���CTreeViewCtrlEx,CSkinCtrl��������ƶ�������Ŀ¼������Ŀ¼��ʱ���ֱ���Ŀ¼�ұ���ʾ����ͼ�꣬ɾ��ͼ�ꡣ
			���ͼ��up�󣬷ֱ��������ɾ����ǰ�
*************************************************/

//class CSkinTree;
class CSkinScrollWnd : public CWindowImpl<CSkinScrollWnd>
{
public:
	BEGIN_MSG_MAP(CSkinScrollWnd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
	END_MSG_MAP()

	CSkinScrollWnd();
	~CSkinScrollWnd();
	LRESULT OnSize(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);
	LRESULT OnPaint(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);

	void SetPos(int pos);					//���ù�������ƫ��λ��
	void UpdateScroll();					//���¹�����,��������
	void SetScroll(CSkinScroll *pScroll);	
	void SetStyle(BOOL bVectical = TRUE);	//TURE--��������  FALSE--�������
//	void SetPage(long nPage);				//��ʾ�ĸ߻���û����Կ��õ��ģ�
//	void SetDuration(long dur);				//��Ҫ��ʾ�Ľ�����ܸ߻��
	UINT GetSize();							//������--��  �����--��
	BOOL IsDowning();						//�Ƿ�����
	void SetList(CListViewCtrl *pList);
//	void SetTree(CSkinTree *pTree);
private:
	CListViewCtrl	*m_pList;
//	CSkinTree		*m_pTree;
	BOOL			m_bVectical;
	BOOL			m_bDown;
	BOOL			m_bTracking;
	BOOL			m_bDragging;			//�Ƿ���������
	BOOL			m_bShowHoriztonal;		//�Ƿ���ʾ���͹���
	BOOL			m_bShowVectical;		//�Ƿ���ʾ���͹���
	SIZE			m_Size;					//��ʱ���ڴ�С
	SIZE			m_OldSize;				//��ǰ���ڴ�С
	CSkinScroll		*m_pScroll;				//�����ָ��--�����洫��
	CSkinScroll		*m_pScrollOver;			//������ڹ����ָ��
	Bitmap			*m_pBmp;				//�����ڵ�Bitmap

	SIZE			m_ClientSize;			//�ͻ�����С
	UINT			m_nHeaderHeight;		//list��header��

	UINT			m_nTotalWidth;
	UINT			m_nTotalHeight;
	BOOL IsBlankArea(int x, int y);			//����������͹��ᶼ����ʱ���Ƿ��ڿհ�����

	void Paint();
	void UpdateVScroll();
	void UpdateHScroll();
	BOOL ShowScroll();

	void ShowListScroll();			//��ʾlistScroll
	void UpdateListVScroll();		//����Scroll��ʾ�ĸ߶ȣ��ܸ߶ȣ��������ڵ�λ��
	void UpdateListHScroll();		//����Scroll��ʾ�Ŀ�ȣ��ܿ�ȣ��������ڵ�λ��

//	void ShowTreeScroll();			//��ʾTreeScroll
//	void UpdateTreeVScroll();		//����Scroll��ʾ�ĸ߶ȣ��ܸ߶ȣ��������ڵ�λ��
//	void UpdateTreeHScroll();		//����Scroll��ʾ�Ŀ�ȣ��ܿ�ȣ��������ڵ�λ��

};

/*
enum enItemStatus
{
	item_add = 1,
	item_delete
};

class CSkinTree : public CWindowImpl<CSkinTree, CTreeViewCtrlEx>,public CSkinCtrl, public IQvodCallBack	// ����
{		//��Ҫ����image �� ������ɫSetColor() �Լ� SetFont, SetHeight �߶���ͼ��һ�������
public:
	BEGIN_MSG_MAP(CSkinTree)	
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()
  
	CSkinTree();
	virtual ~CSkinTree();
	virtual void Clear_();
	LRESULT OnPaint(UINT umsg, WPARAM wp, LPARAM lp, BOOL&);
	LRESULT OnCreate(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);
	LRESULT OnEraseBkgnd(UINT umsg, WPARAM wp, LPARAM lp, BOOL&);
	LRESULT OnMouseMove(UINT umsg, WPARAM wp, LPARAM lp, BOOL&);
	LRESULT OnMouseLeave(UINT umsg, WPARAM wp, LPARAM lp, BOOL&);
	LRESULT OnLButtonDown(UINT umsg, WPARAM wp, LPARAM lp, BOOL&);
	LRESULT OnLButtonUp(UINT umsg, WPARAM wp, LPARAM lp, BOOL&);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wp, LPARAM lp, BOOL&);

	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	BOOL Layout(short nWidth, short nHeight);

	void SetColor(COLORREF crBkColor, COLORREF crTextColor);	//���ñ���ɫ����ɫ
	void SetID(UINT nID);
	void SetItemStatus(HTREEITEM hTreeItem, enItemStatus ItemStatus);
//	void SetBmp(CString strAddPath, CString strDelPath);		//�����½�ͼƬ��ɾ��ͼƬ
	
	UINT GetHScrollPos();
private:
	void Draw();	//��ͼƬ
	void SetPos(BOOL nPage, int pos);

	BOOL		m_bOverBmp;		//�Ƿ���ͼƬ��
	BOOL		m_bDownBmp;		//�Ƿ���ͼƬ�ϰ���
	BOOL		m_bTracking;	//�Ƿ��ڼ���״̬
	Bitmap		*m_hAddBmp;		//�½�ͼƬ  ͼƬ�ĸ߶Ȳ��ܴ�����߶�
	Bitmap		*m_hDeleteBmp;	//ɾ��ͼƬ	ͼƬ�ĸ߶Ȳ��ܴ�����߶�
	COLORREF	m_crBkColor;	
	COLORREF	m_crTextColor;

	HTREEITEM	m_hNowTreeItem;	//��ǰ��ͼƬ����
	HTREEITEM	m_hDownTreeItem;//��ǰ��ͼƬ�ұ����µ���
	UINT		m_nBmpWidth;	//������ͼƬ�Ŀ��
	UINT		m_nID;

	std::map<HTREEITEM , enItemStatus> m_MapItem;

	//hScroll
	CSkinScrollWnd	m_hScrollWnd;
	UINT			m_nhOffPos;
	int				m_nvStepDistance;	//�������� OnWheelʱ��ƫ����
	

	//vScroll
	CSkinScrollWnd	m_vScrollWnd;
	UINT			m_nvOffPos;
	int				m_nhStepDistance;	//������� OnWheelʱ��ƫ����
};


class CSkinMenu: public CMenu // �˵�	��û����������--SetFont
{
public:
	CSkinMenu();

	BOOL DrawItem(LPDRAWITEMSTRUCT lpDrawItem);
	BOOL MeasureItem(LPMEASUREITEMSTRUCT lpDrawItem);
	BOOL LoadMenu(UINT nIDResource, HMENU hMenu = NULL);	//�ı䵱ǰ��Դ�˵����������Ӳ˵��ķ��
private:
	int  GetPopItemID(TCHAR *szTitle);	//��szTitle�еõ����Ӧ��ID; ��� _menupopitemresid

	UINT m_nSepartor;
	UINT m_nHeight;
	UINT m_nWidth;
	UINT m_nDistance;

	COLORREF m_crBkSepartor;
	COLORREF m_crSepartor;

	COLORREF m_crNoSelBk;		//δѡ�еı�����ɫ	
	COLORREF m_crNoSelText;
	COLORREF m_crSelBk;			//ѡ�еı�����ɫ
	COLORREF m_crSelText;
	COLORREF m_crSelBorder;

};



class CSkinEdit : public CSkinCtrl	//�ı���
{
};





class CSkinTreeItem : public CWindowImpl<CSkinTreeItem, CTreeItem>, public CSkinCtrl	// ����
{
};

class CSkinTree : public CWindowImpl<CSkinTree, CTreeViewCtrlEx>, public CSkinCtrl	// ��
{
};

class CSkinMenuItem : public CSkinCtrl	// �˵���
{
};

class CSkinMenu: public CWindowImpl<CSkinMenu>, public CSkinCtrl, public IQvodCallBack	// �˵�
{
public:
long EventNotify_(UINT uMsg, long wparam, long lparam);
};

class CSkinSubWindow : public CWindowImpl<CSkinSubWindow>, public CSkinCtrl, public IQvodCallBack	// �Ӵ���
{
public:
long EventNotify_(UINT uMsg, long wparam, long lparam);
};
*/
enum remove_feet_type
{
	rf_none = 0,			// ����ȥ��
	rf_special,				// ���
	rf_img_all,				// ����ͼƬ���Ľ�
	rf_img_l,				// ....�����
	rf_img_lt,				// ....�����Ͻ�
	rf_img_lb,				// ....�����½�
	rf_img_t,				// ....������2����
	rf_img_r,				// ....���ұ�
	rf_img_rt,				// ....�����Ͻ�
	rf_img_rb,				// ....�����½�
	rf_img_b,				// ....���±�2��
	rf_triangle_all,		// �ĽǶ�Ϊ������
	rf_triangle_l,			// ��ߵĽŶ�Ϊ������
	rf_triangle_lt,			// ���Ͻ�
	rf_triangle_lb,			// ȥ�����½ǵ�������
	rf_triangle_r,			// �ұߵĽŶ�Ϊ������
	rf_triangle_rt,			// ���Ͻ�
	rf_triangle_rb,			// ���½�
	rf_round_all,			// �ĽǶ�Ϊ԰�ġ�
	rf_round_l,				// ��ߵĽ�λԲ��	
	rf_round_r,				// �ұߵĽ�λԲ��
	rf_round_lt,			// ȥ���Ͻǵ�Բ��
	rf_round_lb,			// ȥ���½ǵ�Բ��
	rf_round_rt,			// ȥ���Ͻǵ�Բ��
	rf_round_rb,			// ȥ���½ǵ�Բ��
	rf_irregular,			// ������
	rf_hollow				// �м��
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�����
* ������ڣ�
********************/
class CMenuHost;
class CSkinWindow : public CWindowImpl<CSkinWindow>, 
	                public CSkinCtrl, 
	                public IQvodCallBack,
					public ui::drop::IDropTargetEvent	// ����
{
public:
	DECLARE_WND_CLASS(_T("QVODPLAYERWND"));
	BEGIN_MSG_MAP(CSkinWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_SIZING, OnSizing)
        MESSAGE_HANDLER(WM_ENTERSIZEMOVE, OnEnterSizeMove)
		MESSAGE_HANDLER(WM_EXITSIZEMOVE, OnExitSizeMove)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_SYSKEYUP, OnSysKeyUp)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		//�˵���Ϣ
		MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenuPopup)
		MESSAGE_HANDLER(WM_MENUSELECT, OnMenuSelect)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		MESSAGE_HANDLER(WM_UNINITMENUPOPUP, OnUnInitMenuPopup)

		NOTIFY_RANGE_HANDLER(300, 400, OnNotifyRange)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)			//ljm
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)			//ljm
		MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColor_Edit)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor_Static)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		COMMAND_RANGE_HANDLER(300, 0x9000, OnCommandRange)
#ifdef VIDEOWALL
		MESSAGE_HANDLER(WM_TOUCH, OnTouch)
#endif
	END_MSG_MAP()

	CSkinWindow();
	virtual ~CSkinWindow();
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual BOOL InitRFType(CXMLElement &xmlNode);
	virtual void ResetSkinElement();
	virtual void SetState(skin_state state);
	virtual void Paint(Graphics *pGraph){};
	virtual void OnPaint();
	virtual long CheckBorder(short x, short y);
	virtual LRESULT OnCreate(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnClose(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnEraseBkgnd(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnMouseMove(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnLButtonDown(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnLButtonUp(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnMouseLeave(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnMouseWheel(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnLButtonDblClk(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnRButtonDown(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnRButtonUp(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnSize(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnSysKeyUp(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnKeyDown(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnKeyUp(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnSizing(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnEnterSizeMove(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnExitSizeMove(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnNotifyRange(UINT nID, LPNMHDR pNMHDR, BOOL&);
	virtual LRESULT OnCommandRange(USHORT uMsg, USHORT uID, HWND hWnd, BOOL&);
	virtual LRESULT OnCtlColor_Edit(UINT uMsg, WPARAM wp,LPARAM lp, BOOL& bHandle);
	virtual LRESULT OnCtlColor_Static(UINT uMsg, WPARAM wp,LPARAM lp, BOOL& bHandle);
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wp,LPARAM lp, BOOL& bHandle);
	virtual LRESULT OnCommand(UINT uMsg, WPARAM wp,LPARAM lp, BOOL& bHandle);
	virtual LRESULT OnTimer(UINT uMsg, WPARAM wp,LPARAM lp, BOOL& bHandle);

	//�˵���Ϣ
	virtual LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnUnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
#ifdef VIDEOWALL
	virtual LRESULT OnTouch(UINT, WPARAM wp,LPARAM lp, BOOL&);
#endif

	virtual BOOL OnWheel(short nStep);
	virtual BOOL OnTimer();
	virtual BOOL Layout(short nWidth, short nHeight);
	virtual void ExitThread();
	virtual void Clear_();
	virtual BOOL ResetSkin(LPCTSTR pSkinName);
	virtual BOOL ResetSkin2(LPCTSTR pSkinXmlPath); //ͨ��ָ��xml����Ƥ��
	virtual void GetSkinSnaps(void){};
	virtual void SetSkinSnaps(void){};
	virtual int FindRestoreDate(LPCTSTR id, LPCTSTR tag = 0);// ������Ҫ��ԭ������
	
	virtual void SetUseFlag();
	// ��ק���
	void InitDropTarget();   // ��ʼ��DropTarget,ʹ���ڿ�����ӦDropTarget�¼���������OnCreateʱ����
	void ClearDropTarget();	 // ����DropTarget��������OnCloseʱ����
	
	// ���ܣ���ʼ��ק
	// ������pDragSkin ��ק�Ŀؼ�,pData ��ק������
	virtual void DoDragDrop(ISkin* pDragSkin,ui::drop::DragDropDataPara* pData); 
	virtual long DropTargetDragEnter(IDataObject *pDataObject,POINT &pt);	// ��ק���ݽ��봰�� 
	virtual long DropTargetDragLeave();			// ��ק�����뿪����
	virtual long DropTargetDragOver(POINT &pt);	// ��ק�����ڴ������ƶ�
	virtual long DropTargetDrop(IDataObject *pDataObject,POINT &pt); // ֹͣ��ק

	virtual CString GetStrTime();
	BOOL CheckCursor(short x, short y);//{return FALSE;};
	static DWORD WINAPI Thread_Timer(LPVOID lp);
	LRESULT static CALLBACK MenuInputFilter(int code, WPARAM wp, LPARAM lp);
	void TrackMainMenu(CommonCmd uMsg, WPARAM wp);

	ISkin *GetSkinObj(short x, short y);
//  void SetChildState(LPCTSTR szID ,skin_state state);
//	BOOL GetThreadState(CSkinWindow *skinWnd);
	BOOL SetShadding(CString strFileName, Rect rect, BOOL bDraw=TRUE);
	BOOL SetBgColor(DWORD bgcolor);
	virtual void DrawImg();					// draw background image
	virtual void ResetRegion();		// ��
	void SetRegionByCorner();		// �ڹ���ͼ�εĽ�
	void SetRegionByTransColor();	// ��ȥͼ�����е�͸��ɫ����
	void SetRegionByImage();		// ����ͼƬȥ�ڴ��ڵı߽�
	void DeleteBmp();
//	void MySetFocus();
	void AskForPaint();
	virtual void ShutterDisplay(BOOL bUpward ,int nStep = 10);
	virtual void ShutterHidden(int nStep = 10);
	BOOL IsUpwardShutter(){return m_bUpwardShutter;}
	void SetMenuHost(CMenuHost *pMenuHost);

	BOOL CheckBorderForMouseMove(short x, short y);
	BOOL OnMouseDragForMouseMove(short x, short y);
	void TrackMouseLeaveEvent();
	void HideTips();

	void LockPain();
	void UnLockPain();

public:
	CSubWndHost		m_subWnd;

	// popupmenu properties
	BOOL			m_bPop;
	BOOL			m_bHookEvent;
	long			m_nHotMenu;
//	BOOL m_bThread;		// �߳��Ƿ����
	BOOL			m_bExit;		// �߳��˳���־
	HANDLE			m_hTread;
	CRITICAL_SECTION m_csPaint;
//	BOOL			m_bSizing;		
	TOOLINFO		m_toolInfo;
	BOOL			m_btipshow;
	BOOL			m_bNoShowADTip;
	CString			m_strSkinID;
	BOOL			m_bUsed;		// �����Ƿ����

//protected:
	ISkin		*m_pSkinOver;
	ISkin		*m_pSkinDown;
	ISkin		*m_pSkinFocuse;
	ISkin		*m_pSkinDragTarget;
	CSkinImage	*m_pShaddingImg;
	BOOL		m_bMouseDown;
	BOOL		m_bDrawShadding;
	BOOL		m_bTracking;
	POINT		m_ptLBtDown;		// �������λ��
	DWORD		m_dwEdgeColor;       // �߿����أ�ֻ�Թ��������Ч��
	remove_feet_type m_rf_type;		// ������ͼ������
	int			m_nRoundW;			// ��ͼ���(ֻ��ȥ���ǻ�ȥԲ������)
	BOOL		m_bRegionFinished;	// �����ͼ
	//ui::drop::CDragDropCtrl	m_DragDropCtrl;	// ��ק�ؼ�
	BOOL		m_bEffect;			// �Ƿ�����Ч��ʾ
	BOOL		m_bUpwardShutter;	// ���Ͼ���
	CMenuHost*	m_pMenuHost;		// �˵�
	typedef struct tRESTORE_DATA
	{
		CString id;
		CString tag;
		CString val;
	}RESTORE_DATA;
	std::vector<RESTORE_DATA> m_aRestoreData; // ������Ҫ��ԭ������
	ui::drop::CDropTarget *m_pDropTarget;
private:
	BOOL		m_bRMouseDown;		// �Ҽ�����
};

// ˵���������Ч����
// ���ߣ�������
// ʱ�䣺2012-09-10
class CSkinLayerWindow : public CSkinWindow
{
public:
	DECLARE_WND_CLASS(_T("QVOD_LAYE"));
	BEGIN_MSG_MAP(CSkinLayerWindow)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING , OnWindowPosChanging)
		CHAIN_MSG_MAP(CSkinWindow)
	END_MSG_MAP()
	CSkinLayerWindow();
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	virtual void Clear_();
	virtual void DrawImg();					// draw background image
	virtual void OnPaint();
	virtual BOOL ResetSkin(LPCTSTR pSkinName);
	virtual BOOL ResetSkin2(LPCTSTR pSkinXmlPath);
	virtual void ShutterDisplay(BOOL bUpward ,int nStep = 10);
	virtual void ShutterHidden(int nStep = 10);
	virtual void ShowEdit(BOOL bShow);			// ��ʾ\���ش����е�Edit����,��Edit�ؼ�״̬Ϊ���ɼ�������ʾ������ûҪ��
	virtual void SetState(skin_state state);
	virtual void ResetBackground(BOOL bUpward ,RECT rcLink);
	void CreateWnd(HWND hParent ,Rect rc , DWORD dwStyle ,DWORD dwExStyle = 0);	// �������ڡ��������������CSkinWindow���������⡣
	virtual void SetEventParent(IQvodCallBack *pEventEntry);

	BOOL SendMsgToEdit(UINT, WPARAM wp,LPARAM lp);
	virtual LRESULT OnMouseMove(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnLButtonDown(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnLButtonUp(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnRButtonUp(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnRButtonDown(UINT, WPARAM wp,LPARAM lp, BOOL&);
	virtual LRESULT OnWindowPosChanging(UINT, WPARAM wp,LPARAM lp, BOOL&);
	
protected:
	std::vector<CSkinEdit*> m_pvEdit;
	BOOL				m_nTransR;		// �ұ߽�͸�����ء�
};

/******************
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�
* ����˵���������������൱һ���Ӵ��ڣ��������д����ڲ��ؼ������Ӵ��ڲ�ͬ���ǣ�
			�������й�����������֮���Դ�CSkinWindow�̳У���Ҫ����Ϊ��Edit����
			�Ŀؼ���
********************/
class CSkinContainer : public CSkinWindow
{
public:
	CSkinContainer(void);
	virtual ~CSkinContainer(void);

	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	BOOL Layout(short nWidth, short nHeight);
	ISkin * GetSkinObj(short x, short y);
	BOOL CheckCursor(short x, short y);
	BOOL OnWheel(short nStep);

	long EventNotify_(UINT uMsg, long wparam, long lparam);
	void OnPaint();
	BOOL OnTimer();
	void SetState(skin_state state);
	void SetContentPos(int nPos);

private:
	DWORD m_dbordcolor;
	CSkinScroll * m_pScroll;
	int m_scrollWidth;
	ISkin *m_pHotSkin;
	int m_npos;
	int m_nTotalHeight;
	int m_nItem;

	BOOL m_hasDown;

	CSkinWindow *m_pSubWnd;
	CSkinCtrl *m_pOldObj;
	CSkinCtrl *m_pDownObj;

};

class CMyListCtrl : public CListViewCtrl ,public IQvodCallBack
{
public:

	CMyListCtrl();
	~CMyListCtrl();
	
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);
	void SetEventObj(IQvodCallBack* pEventObj);
	static LRESULT CALLBACK HookListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	HWND Create(HWND hWndParent, RECT &rect, LPCTSTR szWindowName = NULL,DWORD dwStyle = 0, DWORD dwExStyle = 0, UINT nID = 0);
	LRESULT ListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void SetParentHandle(HWND hWnd);

	WNDPROC m_oldWndProc;
	IQvodCallBack *m_pEventObj;
	HWND m_hParentHandle;	
	BOOL m_bTracking;
};

//�Ի沥���б�ģʽ�ؼ�
#define CUSTOM_LIST_BMP_MAX_NUM			10            //�Ի沥���б�ģʽ�ؼ�ͼƬ��Ŀ
#define CUSTOM_LIST_ICONIMG_MAX_NUM     64			  // ICON ͼƬ��
#define CUSTOM_LIST_BK					0             //����
#define CUSTOM_LIST_HeaderItemBk		1             //�б�ͷ����
#define CUSTOM_LIST_HeaderSeparator		2             //�б�ͷ��ָ���
#define CUSTOM_LIST_ItemBK				3             //�б����
#define CUSTOM_LIST_imageItemFold		4             //�б��� չ�� + - ͼ��

class  CCustomListCtrl:public CSkinCtrl,public IQvodCallBack
{
public:
	enum GetSpecifiedOption
	{
		FirstOne,		//��һ��
		FirstEnabled,		//��һ���ǽ��õ�
		LastOne,		//���һ��
		LastEnabled,		//���һ���ǽ��õ�
		NextOne,		//��һ��
		NextEnabled,	//��һ���ǽ��õ�
		PrevOne,		//��һ��
		PrevEnabled,		//��һ���ǽ��õ�
	};
	typedef struct ItemData			//�б���ṹ
	{
		int nID;				//�ڵ�Ԫ�صı��
		int nSeriesIdx;			//�缯�������
		CString str;
		Bitmap * m_Img;
		CUS_LISTITEM_STATE m_state; //�б����״̬
		item_property nProterty;
		//�Ƿ�Ϊ����״̬,����Ϊչ��״̬ --����ITEM_FOLDER��չ���ڵ���Ч    
		BOOL bFold; 
		BOOL bHalfTrans;			//�Ƿ��͸
		BOOL bChecked;				//�Ƿ���ѡȡ
		BOOL bDisable;
		Rect rtText;
		//���ڵ�ָ�� --����ITEM_CHILD��չ���ڵ���Ч           
		ItemData * pParent;
		//�ӽڵ��б�--����ITEM_FOLDER��չ���ڵ���Ч    
		std::vector<ItemData * > vtChildItemList;	

		ItemData()
		{
			nSeriesIdx = 0;
			m_Img = NULL;
			m_state=CUS_LISTITEM_ST_NM;
			nProterty = ITEM_NORMAL ;
			bFold = FALSE ;
			bHalfTrans = FALSE;
			bChecked = FALSE;
			pParent = NULL ;
			vtChildItemList.clear();
			bDisable = FALSE;
		}
		~ItemData()
		{
			for (int i = 0 ; i < vtChildItemList.size(); ++i)
			{
				ItemData * lItem = vtChildItemList[i] ;
				delete lItem ;
			}
			vtChildItemList.clear();
		}
	}ItemData;

	typedef struct CustomList_Column // �нṹ
	{
		long nWidth;
		int nMinWidth ;
		CString strName;
		BOOL bSort;
		BOOL bResize;
		CUS_Alignment m_Align;
		CUS_LISTITEM_STATE m_state;
		std::vector<ItemData *> vSubItem;

		CustomList_Column()
		{
			//		nWidth = 75;
			bResize = bSort = FALSE;
			m_state = CUS_LISTITEM_ST_NM;
			m_Align = CUS_Align_left;
			nMinWidth = 2;
			vSubItem.clear();
		}
		~CustomList_Column()
		{
			for ( int i = 0; i< vSubItem.size() ; ++i )
			{
				ItemData * lItem = vSubItem[i] ;
				if ( lItem != NULL)
				{
					delete lItem ;
				}			
			}
			vSubItem.clear();
		}
	}CustomList_Column;
	typedef struct CUS_List_Item_Child_N
	{
		int nItem ;
		int nChilditem ;
		CUS_List_Item_Child_N()
		{
			nItem = -1 ;
			nChilditem = -1 ;
		}
	} CUS_List_Item_Child_N ;
public:
	enum HeaderCheckVisualBoxState
	{
		HCS_Focus_Normal = 0,
		HCS_Focus_Hover = 1,
		HCS_Click_Down = 2,
	};
	enum HeaderCheckState
	{
		HCS_CheckState_Checked = 0,
		HCS_CheckState_Unchecked,
		HCS_CheckState_Box,
	};
	enum BackgroundType
	{
		GrayAndWhite,
		Separator,
	};
public:
	CCustomListCtrl();
	~CCustomListCtrl();

	//
	//Attributes	
	//
	BOOL Create( DWORD dwStyle, const RECT& rect, HWND hWndParent, UINT nID );	
	//You construct a CCustomListCtrl in two steps. First call the constructor, then call Create, which creates the list view control and attaches it to the CListCtrl object.
	COLORREF GetBkColor( ) const;														//Retrieves the background color of a list view control.
	BOOL SetBkColor( COLORREF cr );														//Sets the background color of the list view control.
	int GetItemCount( );
	// ��ȡ�������б��� ��Ŀ
	int GetAllItemCount( );
	//��ȡ �ض��� ���б��� ��Ŀ
	int GetChildItemCount( int nItem );
	// ��ȡ �б�����
	int GetColumnCount();
	//Retrieves the number of items in a list view control.
	//��ȡѡ���б�����Ŀ
	int GetSelItemCount();
	//��ȡѡ���б��� ����Ŀ �ӽڵ�ȫѡҲͳ�Ƹ��ڵ�
	int GetAllSelRowCount();
	//��ȡ���õ��б�����Ŀ
	int GetEnabledItemCount();
	//��ȡѡ���б�������
	BOOL GetSelItemVeter(std::vector<UINT > &vSelect);
	//��ȡѡ���б������� ��������
	BOOL GetAllSelItemVeter(std::vector< CUS_List_Item_Child_N > &vSelect);
	//��ȡѡ���б���ID���� ��������
	BOOL GetAllSel_ItemID_Veter(std::vector< long > &vSelectID) ;
	//��ȡ����ѡȡ���б���ID���� ��������
	BOOL GetAllCheckedItemVector(std::vector<CUS_List_Item_Child_N> &CheckedIDs);
	BOOL GetAllCheckedItemIDVector(std::vector<long> &CheckedIDs);
	// ��ȡ�� nIndex�� ѡ���б��� ID
	long GetSelectIndexItem_ID( int nIndex = 0 );
	// ����ѡ���б��� ID
	void UpdataSelectItemID();
	//��ȡ ��ק�ļ�/�ļ������Ƶ�������
	BOOL GetDragList(std::vector<CString> & vDragList);
	// �ƶ��������б���
	BOOL AutoMoveItem( short nitem , BOOL bvertical = TRUE );
	// ��ĳ���б����ڿɼ�����
	BOOL SetItemVisual(int nItem ,int nChildItem = -1 );
	//�����б���״̬
	BOOL SetItemState(int nItem,CUS_LISTITEM_STATE state , int nChildItem = -1 ,BOOL bAllChild = FALSE, BOOL bIgnoreDisabled = FALSE);
	// �����б���ͼ��
	BOOL SetItemIcon(  int nItem , CString & filename  ,int nChildItem = -1);
	//�����б��� ͼƬ
	BOOL SetItemBitmap(int nItem , Bitmap * bmp , int nChildItem = -1, int nColumn = 0  );
	//�����Ƿ� ��ʾͼ��
	void SetShowItemIcon( BOOL bshow );
	//�����б����͸
	BOOL SetItemHalfTrans(BOOL bHalfTrans, int nItem , int nChildItem = -1);
	//���������б��� Ϊ�ض�״̬
	BOOL SetAllItemState( CUS_LISTITEM_STATE state, BOOL bIgnoreDisabled = FALSE );
	//���������б���ѡ��״̬
	BOOL SetAllItemChecked(BOOL bChecked, BOOL bIgnoreDisabled = FALSE);
	//��ȡ�б����״̬
	int GetItemState( int nItem ,int nChildItem = -1);
	//��ȡ������ѡȡ״̬
	BOOL GetItemChecked( int nItem, int nChildItem = -1);
	BOOL SetItemChecked( int nItem, int nChildItem = -1, BOOL bChecked = TRUE);
	BOOL ToggleItemChecked( int nItem, int nChildItem = -1);
	void SetHeaderCheckState(HeaderCheckState state) { m_eHeaderCheckState = state; m_bDirty = TRUE; }
	//��ȡ�������Ƿ����
	BOOL GetItemEnable(int nItem, int nChildItem = -1);
	BOOL SetItemEnable(int nItem, int nChildItem, BOOL bEnable);
	//��ȡ�б��� �� �ڵ�����  0���� 1����չ�ڵ� 2�ӽڵ�
	int GetItemProterty( int nItem );
	//��ȡ�б��� �� �ڵ� �Ƿ���չ 
	BOOL GetItemFold( int nItem );
	//���� �б��� �� �ڵ� �Ƿ���չ 
	BOOL SetItemFold( int nItem , BOOL bFold);
	int GetFirstSelectedItemPosition( ) const;										//Gets the position of the first selected item in the list view control.
	int GetNextSelectedItem( int& pos ) const;										//Gets the index of the list item identified by pos, then sets pos to the POSITION value. You can use GetNextSelectedItem in a forward iteration loop if you establish the initial position with a call to GetFirstSelectedItemPosition.
	//You must ensure that your POSITION value is valid. If it is invalid, 
	BOOL GetItemRect( int nItem, LPRECT lpRect, UINT nCode ) const;
	// 				nItem	The index of the item whose position is to be retrieved.	
	// 				lpRect	Address of aRECT structure that receives the bounding rectangle. 	
	// 				nCode	Portion of the list view item for which to retrieve the bounding rectangle. It can be one of these values: 
	// 						LVIR_BOUNDS   Returns the bounding rectangle of the entire item, including the icon and label.
	// 						LVIR_ICON   Returns the bounding rectangle of the icon or small icon.
	// 						LVIR_LABEL   Returns the bounding rectangle of the item text.	
	// 				Retrieves the bounding rectangle for all or part of an item in the current view.

	int GetStringWidth( LPCTSTR lpsz ) const;											//Determines the minimum column width necessary to display all of a given string.
	int GetColumnWidth( int nCol ) const;												//Retrieves the width of a column in report view or list view.
	//��ȡ�б�ͷ ״̬
	int GetColumnState( int nCol ) const;
	//��ȡ���ھ��
	HWND GetWindow( );
	//���� �б�ͷ ״̬
	BOOL SetColumnState( int nCol ,int state ) ;
	//��ȡ�п���Ƿ��ܱ��ı�����
	BOOL GetColumnbReSize( int nCol ) const ;
	//�ж��Ƿ��ڸı��п�ȵ�λ���ϣ������ nCol ���صڼ���
	BOOL CheckInColumnSize(int px,int py, int &nCol); 
	//�ж��Ƿ��� �б�ͷλ���ϣ������ nCol ���صڼ���
	BOOL CheckInColumnHeader(int px,int py, int &nCol); 
	// �����б� �е�����  �� ˮƽ���뷽ʽ�� -1 Ϊ���䣩�� ��� ��-1 Ϊ���䣩
	BOOL SetColumnText( int nCol ,CString & str  ,int nWidth = -1, int hor_align = -1);
	// ������С�п�
	BOOL SetColumnMiniWidth( int nCol, int cx );
	BOOL SetColumnWidth( int nCol, int cx );											//Changes the width of a column in report view or list view.
	BOOL GetViewRect( LPRECT lpRect ) const;											//Retrieves the bounding rectangle of all items in the list view control. 
	COLORREF GetTextColor( ) const;														//Retrieves the text color of a list view control.
	BOOL SetTextColor( COLORREF cr );													//Sets the text color of a list view control.
	//��ȡ �б��� ��ͼƬָ��
	Bitmap * GetItem_pBitmap( int nItem, int nSubItem , int nChildItem ); 
	CString GetItemText( int nItem, int nSubItem , int nChildItem = -1) ;								//	if nSubItem is nonzero, it retrieves the text of the subitem. 
	BOOL SetItemText( int nItem, int nSubItem, CString & lpszText , int nChildItem = -1);						//Changes the text of a list view item or subitem. 
	//���� �б��� ID
	BOOL SetItemID  (int nItem , int ID ,int nChildItem = -1);
	//��� �б��� ID
	int GetItemID  (int nItem ,int nChildItem = -1 );
	//ͨ��ID���λ��
	int GetItemByID(int nID, int* pChildPos = NULL);
	//���ָ�����ID
	int GetSpecified(GetSpecifiedOption option, int nRefID/*������ID*/);

	//
	//Operations
	//
	int InsertItem( int nItem, LPCTSTR lpszItem ,item_property proterty = ITEM_NORMAL ,int nChildItem = -1 );										//Inserts an item into the list view control.
	// ��������
	BOOL SetItemProperty( int nItem , item_property proterty , int nChildItem = -1) ;
	BOOL DeleteItem( int nItem, int nChildItem = -1);														//Deletes an item from the control.
	BOOL DeleteAllItems( );																//Deletes all items from the control.
	BOOL Scroll( CSize size );															
	// 				size		A CSize object specifying the amount of horizontal and vertical scrolling, in pixels. The y member of size is divided by the height, in pixels, of the list view control��s line, and the control is scrolled by the resulting number of lines.
	// 				Remarks		Scrolls the content of a list view control.
	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nWidth = -1 ,BOOL b_Resize=TRUE,BOOL b_sort = TRUE, int nFormat = LVCFMT_LEFT,int nSubItem = -1);
	// 				nCol					The index of the new column.
	// 				lpszColumnHeading		Address of a string containing the column��s heading.
	// 				nFormat					Integer specifying the alignment of the column. It can be one of these values: LVCFMT_LEFT, LVCFMT_RIGHT, or LVCFMT_CENTER.
	// 				nWidth					Width of the column, in pixels. If this parameter is -1, the column width is not set.
	// 				nSubItem				Index of the subitem associated with the column. If this parameter is -1, no subitem is associatied with the column.
	// 				Remarks
	// 				Inserts a new column in a list view control.

	//�����б� �еĶ��뷽ʽ
	BOOL SetColumnAlign ( int nCol ,CUS_Alignment hor_align = CUS_Align_left );
	BOOL SetColumnAlign ( int nCol ,int hor_align = 0 );

	BOOL DeleteColumn( int nCol );														//Deletes a column from the list view control.
	// ɾ�������У�ֻ����һ�пհ���
	BOOL DeleteAllColumns() ;

	//�ƶ�n��Item��ָ��λ����
	BOOL MoveItem(std::vector<UINT>& vIndex, int nPos);

	BOOL EditItem(int nItem, int nCol, int nChildItem = -1);

	BOOL OverEdit(BOOL bModify);

	CSkinEdit* CreateEdit();

	ItemData* GetItemData(int nItem, int nCol, int nChildItem = -1);
	//
	//�ڲ�ʵ�ֹ�����
	//
	virtual BOOL OnKey(WORD nChar);											//���̰���

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//��ʼ��--һ��ʵ���ӿؼ�����,��Դ���ص�
	virtual void Clear_();													//����ʼ��
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//��Ϣ�ص�--���ӿؼ�����ʱ�򴫸���obj,����ʵ�ֵ�ʱ����������
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//�����ӿ�--����COM��ѯ�ӿ�,
	virtual BOOL OnTimer();													//�Զ����ö�ʱ��--�鿴�Ƿ���Ҫ�ػ�
	virtual BOOL OnMouseMoveIn(short x, short y);							//��괦����Ϣ-�ƽ��ؼ�
	virtual BOOL OnMouseMoveOut();											//��괦����Ϣ-�Ƴ��ؼ�
	virtual BOOL OnMouseDown(short x, short y);								//��괦����Ϣ-����
	virtual BOOL OnMouseUp(short x, short y);								//��괦����Ϣ-�ͷ�
	virtual BOOL OnMouseDrag(short x, short y);								//��괦����Ϣ-�϶�
	virtual BOOL OnMouseMove(short x, short y);								//��괦����Ϣ-�ƶ�
	virtual BOOL OnRButtonUp(short x, short y);								//��괦����Ϣ-�Ҽ��ͷ�
	virtual BOOL OnRButtonDown(short x, short y);							//��괦����Ϣ-�Ҽ�����
	virtual BOOL OnDblClick(short x, short y);								//��괦����Ϣ-���˫��
	virtual BOOL OnWheel(short nStep);										//��괦����Ϣ-���ֹ���
	virtual	long OnDrag(UINT uMsg, long wparam, long lparam);				//�϶���Ϣ����
	virtual BOOL Layout(short nWidth, short nHeight);						//����չ�ֿؼ�--�ú����ɸı䴰�ڴ�Сλ�õ�ʱ�����WM_SIZE

	BOOL CheckCursor(short x, short y);										//��������Ƿ��ڿؼ���

	//�ж��Ƿ��ڿؼ��������򣨲�������ֱ��ˮƽ��������
	BOOL CheckMouseInContent(short x, short y);
	//��ȡ�ؼ��������򣨲�������ֱ��ˮƽ���������������б�ͷ��
	Rect GetContentRect();
	//�ж��Ƿ���갴��
	BOOL IsMouseDown();
	//���������ĸ��б���������
	BOOL CheckMouseInItemHover(short x,short y);
	//���������ĸ��б����ϰ���
	int  CheckMouseInItemDown(short x,short y);
	//���ĳ�����ĸ��б�����
	int  CheckPointInItem(short x,short y);
	//���ĳ�����ĸ� ���б��� ��
	BOOL CheckPointInChildItem(short x, short y , int & nItem ,int & nChildItem);	
	//��� ��ѡ ���յ� ���ĸ� ���б��� ��
	BOOL CheckDragPtIn(short x, short y , int & nItem ,int & nChildItem);
	//����϶������ĸ��б����� ��������
	BOOL  CheckDragPointInItem(short x,short y ,int & nItem ,int & nChildItem);
	//�ж�ĳ���Ƿ���ĳһ��������ϣ�����/ͼ��..��
	BOOL IsMouseInItemContent(int x, int y);
	//��ѡ����
	BOOL OnMouseDragSelect(short x, short y);
	//�����ק�ƶ� �б���
	BOOL MouseDragMoveItem( short x, short y);
	//�϶����ƴ���
	void DoDragDrop(int xDragImgMove, int yDragImgMove);
	//���� Drag ����� �б����ݹ������ʵ�λ��
	BOOL DragMoveList( short x, short y );

	void Paint(Graphics *pGraph);											//���ƿؼ�

	void DrawCheckBox( Graphics * gra );		//����CheckBox���
	//���϶�ʱ��ͼƬ
	void DrawDragBmp(HBITMAP & hBmp);
	void DrawItem_byDrag(Graphics *pGraph, int nItem, int &nOrder);
	//����ĳһ���б�ͷ
	void DrawColumn(Graphics *pGraph, int index);
	//����һ�е��б���
	void DrawItembyColumn( Graphics *pGraph, int index );						


	//��������scroll�ؼ�����
	void RefreshScrollCtrl();

	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//��XML�ļ�����Ƥ��Ԫ�ص�����

	// 	BOOL ISPointInFoldBtn(int x, int y, int nItem);							//�жϵ��Ƿ���չ����ť��
	// 	BOOL ISPointInFolderItem(int x, int y);									//�жϵ��Ƿ��ڿ�չ��item��
	// 
	// 	void FoldItem(int nItem);												//�۵�ĳһ��item
	// 	void UnfoldItem(int nItem);												//��չĳһ��item
	// 	BOOL IsFoldItem(int nItem);												//�ж�ĳ���Ƿ�Ϊ��չ����
	// 
	// 	void Sort_by_Column(int index);											//����--���ݵ� index��

	Rect GetWindowRect() ;
	void SetRedraw(BOOL b);													//����ͼ�����ʱ�Ƿ���ƿؼ�
	void SetDragSort(BOOL bDragSort);
	void SetCheckBoxEnable(BOOL bEnable);				// ��/�� ��checkbox
	CCustomListCtrl::BackgroundType GetBackgroundType() const { return m_eBackgroundType; }
	void SetBackgroundType(CCustomListCtrl::BackgroundType val) { m_eBackgroundType = val; }
	BOOL EnsureItemShownOnTop(int nItem, int nChildItem = -1);

	void SetListHalfTrans(BOOL bListHalfTrans) { m_bListHalfTrans = bListHalfTrans ; } ;	//Add by chenzhibiao

	// DragDrop���
	virtual BOOL DragMouseIn(short x, short y);
	virtual BOOL DragMouseOut();
	virtual BOOL DragMouseMove(short x, short y);
	virtual BOOL DragMouseUp(short x, short y);
private:
	CRITICAL_SECTION m_rsLock;										//��Դ��-���ڷ�ֹ���߳���ɾ����������ƹ��̵ĳ�ͻ
	CUSTOM_LIST_MSG  m_MSG;											//���� ���෢������Ϣ,����������Ϣ
	std::vector<CustomList_Column *> m_Column;						// �нṹ����
	std::vector<short > m_vdragFrameSelect;							// ��ѡѡ���б�������
	std::vector<CUS_List_Item_Child_N > m_vSelectItem_Child_N;	
	std::vector<long > m_vSelectItemID;	
	Bitmap * m_aImg[CUSTOM_LIST_BMP_MAX_NUM];						//�洢����״̬�Ļ���λͼ
	Bitmap * m_IconImg[CUSTOM_LIST_ICONIMG_MAX_NUM];				//�洢���� ICON ����λͼ
	int  m_nColumnPicWidth ;										//�б�ͷ ÿ��״̬ͼ ���
	BOOL m_bDrag;													//������϶�ʱΪ TRUE
	//��ѡ�Ƿ���Ч
	BOOL m_bDragFrameValid;
	BOOL m_bDlclick;
	//��ק�ƶ� �� �б���
	int m_ndragMoveItem;
	//�����������б�ͷ ��
	int m_nHV_Column ;
	int m_ndragMoveChildI;
	//�Ƿ���ק�ƶ��б���
	BOOL m_bDragMoveItem;
	BOOL m_bShowDragSep;
	HBITMAP				 m_hDragBitmap;								//���Ա����϶���λͼ
	CSkinScroll * m_pScroll;											//��ֱ������
	int       m_nScrollWidth;											//��ֱ���������
	BOOL m_bScrolldown;													//��ֱ������Scroll down
	BOOL m_bHorScrolldown;												//ˮƽ������Scroll down
	BOOL m_bInscroll;													//����Ƿ��ڴ�ֱscroll��
	CSkinScroll * m_pHorScroll;											//ˮƽ������
	CSkinEdit* m_pEdit;													//�༭��
	//��ֱ�������߶�
	int       m_nHorScrollHeight;	
	BOOL m_bInHorScroll;													//����Ƿ���ˮƽscroll��

	BOOL m_bRedraw;														//ͼ�����ʱ�Ƿ���ƿؼ�
	BOOL m_bColomSizeDown;												//�б�ͷ�ı���λ���Ƿ���
	// �Ƿ��������� 
	BOOL m_bSorting ;
	// ����ǰ���б�����
	int m_nSortItemCount;
	BOOL m_bLButtonDown;
	//�Ƿ� ɾ��������ѡ�б���
	BOOL m_bDelOtherSel;
	//����Ƿ��� ѡ�е��б�����
	BOOL m_bMouseInSelectItem;
	BOOL m_bEditShow;												//�༭���Ƿ�����ʾ
	BOOL m_bClickTwice;
	int m_nEditCol;
	int m_nEditItem;
	int m_nEditChildItem;
	int pre_HV_Column ;
	//����� ѡ���б��������
	int m_nMouseInSelectItem;
	int m_nMouseInSelectChildItem ;
	//�Ƿ���� ��ק �б���
	//CDragDropCtrl		 m_DragDropCtrl;							//�Ϸſؼ�
	BOOL m_bEnableDragItem;
	// �������ļ����ȡ�Ƿ� ��Ҫ ��ק �ƶ� �б����
	BOOL m_bNeedDragMoveItem ;
	HCURSOR hCurColoumSize;                                             //�б�ͷ�ı���λ�������״
	int  m_nColSize;													//������б�ͷ�ı���λ�� ������
	CString m_headerFace;												//�б�ͷ����
	CString m_itemFace;													//�б�������
	int  m_HeaderHeight;												//�б�ͷ�߶�
	int  m_ItemHeight;													//�б���߶�
	int  m_headerSize;													//�б�ͷ�����С
	int  m_itemSize;													//�б��������С 
	//�б�ͷ �ı�����
	int  m_nheaderTextX;
	//�б����� ���
	short m_nHeadSeperatorWidth;
	int  m_nheaderTextY;
	int  m_nheaderTextWidth;
	int  m_nheaderTextHeight;
	BOOL  m_bheaderTextBold;
	//�б��б��� �ı�����
	int  m_nitemTextX;
	int  m_nitemTextY;
	int  m_nitemTextWidth;
	int  m_nitemTextHeight;
	BOOL  m_bitemTextBold;
	Color m_bgColor;													//������ɫ
	Color m_headerClr_NL;												//�б�ͷ������ɫ NL
	Color m_headerClr_HV;												//�б�ͷ������ɫ HV
	Color m_headerClr_DN;												//�б�ͷ������ɫ DN
	Color m_itemClr_NL;													//�б���������ɫ NL
	Color m_itemClr_HV;													//�б���������ɫ HV
	Color m_itemClr_DN;													//�б���������ɫ DN
	Color m_itemClr_DI;													//�б���������ɫ DI(SABLE)
	Color m_itemBkClr_HV;												//�б��� ������ɫ HV
	//�б��� ������ɫ DN �����ж����ɫֵ
	std::vector<Color> m_VtItemBkClr_NM;
	Color m_itemBkClr_DN ;
	std::vector<SolidBrush *> m_VtItem_NM_Brush;												
	// �б���֮���϶
	int m_nitemVerSapce;
	// ��ѡ�򱳾�ɫ
	Color m_DragFrameBkClr;
	// ��ѡ��ıߵ���ɫ
	Color m_DragBorderClr;
	SolidBrush * m_lpDragFrameBrush;
	Pen *  m_lpDragBorderPen;
	// ��ѡ��ıߵĿ��
	int m_DragBorderWidth;
	// ��ק �ƶ��б���ʱ���ֵ���ʾ����λ�õ� �ָ�����ɫ
	Color m_itemDragSepClr;
	// ��ק �ƶ��б���ʱ���ֵ���ʾ����λ�õ� �ָ����߶�
	int m_itemDragSepHeight;
	// �϶�ʱ��ͼƬ ������ɫ
	Color m_Item_byDragBkClr;
	// �϶�ʱ��ͼƬ ������ɫ
	Color m_Item_byDragTextClr;
	Rect m_rtDragFrame;												//�����ѡʱ����ƵĿ�!
	CPoint m_ptDragStart;										    //��¼�����ѡʱ����Ƶİ������!
	CPoint m_ptDragEnd;												 //��¼�����ѡʱ����Ƶĵ����յ�!

	SolidBrush *lpbushItemText_NL,*lpbushItemText_HV,*lpbushItemText_DN, *lpbushItemText_DI,
		*lpbushHeaderText_NL,*lpbushHeaderText_HV,*lpbushHeaderText_DN;
	Font * m_lpHeaderFont,* m_lpItemFont;


	long m_lPageViewPos;											//��ǰ��ʾҳ������ܶ�����ƫ��(Scroll��ƫ����)
	long m_nTotalHeight;											//Item���ܸ�
	long m_nTotalWidth;												//Item���ܿ�
	Rect  m_ListRect;												//�б�����
	int m_nPerPageRowNum;											//ÿҳ�������ɵ�����
	int m_nPerPageColNum;											//ÿҳ�������ɵ�����
	int m_nTotalRowNum;												//�ܹ�ռ�ݵ�����
	int m_nTopViewRow;												//����ҳ��,���������  

	Bitmap  *m_pMemBmp;											//�ڴ�λͼ
	Rect	 m_paintRect;										//����ͼ��ľ�������-�ؼ���С
	float    m_scroll_dx;										//ˮƽ����������λ��
	float    m_scroll_dy;										//��ֱ����������λ��
	BOOL   bSetTimer ;
	// չ��ͼ ���б��� �еľ���
	Rect foldRect;
	CSkinButton*	m_pTranscodeBtn;										//ת�밴ť
	Bitmap*			m_pTrascodeBmp;									//ת�밴ťͼƬ
	BOOL m_bOverTranscodeBtndown;									// ת�밴ť�Ƿ���
	BOOL m_bInOverTranscodeBtn;											//����Ƿ��ڸ���ת�밴ť��
	int nItemHV , nChildHV ;
	// �ļ�ͼ�� �� ��ʾ����
	Rect m_rectIcon;
	BOOL m_bShowIcon ;
	int m_nPreSelItem  ,m_nPreSelChild ;                        //���һ��ѡ�е� �б���
	int m_nClickItem , m_nClickChildItem ;						//���һ�ε����� �б���
	int nFirstItem , nFirstChild ;
	int nLastItem , nLastChild ;
	int m_nHoverItem;											//���������
	BOOL m_bDragSort;											//�϶��Ƿ񴥷�����
	std::vector<CString> m_vtDragList;							//��ק�ļ�/�ļ������Ƶ�������
	BOOL m_bEnableCheckBox;
	INT m_nXOffset;
	Gdiplus::Bitmap *m_pCheckImage;
	Rect m_rectHeadChkBox;
	HeaderCheckVisualBoxState m_eHeaderChkboxVisualState;
	HeaderCheckState m_eHeaderCheckState;
	BackgroundType m_eBackgroundType;

	BOOL m_bListHalfTrans ;										//Add by chenzhibiao �б��͸Ч����ÿ��ˢ��Ҫȫ��ˢ
};
/*
class CSkinDialog: public CDialogImpl<CSkinDialog>, public CSkinCtrl, public IQvodCallBack	// �Ի���
{
public:
long EventNotify_(UINT uMsg, long wparam, long lparam);
};
*/


#endif