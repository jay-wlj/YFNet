/*
* Copyright(c) 2010, 深圳市快播科技有限公司
* All rights reserved.
* 
* 文件名称：IQvodSkin.h
* 摘    要：各组件头文件的定义声明
*
* 当前版本：1.0
* 作    者：关玮 夏成聪 刘建敏
* 完成日期：
*/

#include "..\Common\timer.h"
#include "..\QvodPlayer\skin\Layout.h"

#ifndef	_IQVOD_SKIN_H__
#define _IQVOD_SKIN_H__

//控件的状态
#define  SKIN_STATE_NUM          5                 //控件状态数
#define  SKIN_STATE_NL           0                 //正常       normal
#define  SKIN_STATE_HV           1                 //悬空       hover
#define  SKIN_STATE_DN           2                 //按下       down
#define  SKIN_STATE_DS           3                 //不能用     disable
#define  SKIN_STATE_GY           4                 //灰化       gray
class CSkinWindow;
/******************
* 当前版本：1.0
* 作    者：关玮
* 完成日期：
********************/
interface ISkin
{
public:
	virtual ~ISkin(){};
	// 方法
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry)=0;				// 初始化
	virtual void Clear_()=0;															// 反初始化
	virtual void ExitThread()=0;														// 退出线程
	//	virtual ISkin* CreateSkinObj(CXMLElement &xmlNode, IQvodCallBack *pEventEntry)=0;	// 创建子元素
	//virtual void AppendChild(ISkin* pSkinObj)=0;										// 添加子元素
	virtual BOOL Layout(short nWidth, short nHeight)=0;						// 布局
	virtual BOOL ReLayout() = 0;
	virtual ISkin* FindChild(LPCTSTR szID)=0;								// 查找皮肤对象
	virtual void Paint(Graphics *pGraph)=0;									// 画皮肤对象

	// 设置
	virtual void SetState(skin_state state)=0;								// 设置状态
	virtual void SetChildState(LPCTSTR szID ,skin_state state)=0;
	virtual BOOL SetChildText(LPCTSTR szID, LPCTSTR szText)=0;
	virtual long Method_(LPCTSTR szMethod, long para1, long para2)=0;		// 调用方法

	// 属性
	virtual skin_type GetSkinType()=0;						// 得控件类型
	virtual CString GetID()=0;								// 得ID
	virtual BOOL CheckCursor(short x, short y)=0;			// 探测鼠标
	virtual BOOL GetCtrlRect(LPCTSTR szID, Rect &rect)=0;	// 得控件位置
	virtual BOOL IsVisible()=0;								// 是否可见
	virtual BOOL IsEnable() = 0;

	// 消息
	virtual BOOL OnTimer()=0;							// 计时器
	virtual BOOL OnMouseMoveIn(short x, short y)=0;		// 鼠标移进
	virtual BOOL OnMouseMoveOut()=0;					// 鼠标移出
	virtual BOOL OnMouseDown(short x, short y)=0;		// 鼠标左键下
	virtual BOOL OnMouseUp(short x, short y)=0;			// 鼠标左键上
	virtual BOOL OnMouseDrag(short x, short y)=0;		// 鼠标拖动
	virtual BOOL OnRButtonUp(short x, short y)=0;		// 鼠标右键上
	virtual BOOL OnRButtonDown(short x, short y)=0;		// 鼠标右键下
	virtual BOOL OnMouseMove(short x, short y)=0;		// 鼠标移动
	virtual BOOL OnDblClick(short x, short y)=0;		// 鼠标双击
	virtual BOOL OnWheel(short nStep)=0;				// 鼠标滚轮
	virtual BOOL OnKey(WORD nChar)=0;					// 键盘按键
	virtual long OnDrag(UINT uMsg, long wparam, long lparam)=0; //响应拖动有关的消息
	virtual BOOL DragCheckData(IDataObject *pObject)=0;
	virtual BOOL IsShowRectContain(short x, short y)=0;

	virtual CString GetTipsID() = 0;									//获取需要显示tips的控件ID

	// DragDrop相关
	virtual BOOL DragMouseIn(short x, short y)=0;
	virtual BOOL DragMouseOut()=0;
	virtual BOOL DragMouseMove(short x, short y)=0;
	virtual BOOL DragMouseUp(short x, short y)=0;
};

/******************
* 当前版本：1.0
* 作    者：关玮
* 完成日期：
********************/
class CSkinCtrl : public ISkin
{
public:

	CSkinCtrl();
	virtual ~CSkinCtrl(){};
	static BOOL BitmapFromFile(LPCTSTR path, Bitmap ** pRefBmp); // 读取图片文件
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

	//立即重绘。 若bRepaintWnd为True,则重绘控件所在的整个窗口（透明控件需填true),否则只重绘自己。
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
	virtual BOOL OnRButtonUp(short x, short y);		// 鼠标右键上
	virtual BOOL OnRButtonDown(short x, short y);	// 鼠标右键下
	virtual long OnDrag(UINT uMsg, long wparam, long lparam); //响应拖动有关的消息
	virtual BOOL DragCheckData(IDataObject *pObject);
	virtual BOOL IsShowRectContain(short x, short y);
	virtual CString GetTipsID();									//获取需要显示tips的控件ID
	
	// DragDrop相关
	virtual BOOL DragMouseIn(short x, short y);
	virtual BOOL DragMouseOut();
	virtual BOOL DragMouseMove(short x, short y);
	virtual BOOL DragMouseUp(short x, short y);

public:
	std::vector<ISkin*> m_pvSkinObj;
	BOOL m_bSkinObjConstructOK;		//皮肤控件构造完成吗
	SKIN_LAYOUT m_layout;
	BOOL m_bOver;					//鼠标在控件上
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
	DWORD m_bgColor;			// 背景色
	DWORD m_transColor;			// 透明色
	BOOL m_bShow;

	BOOL m_bDown;
	BOOL m_bDisable;
	BOOL m_bDirty;  //判断是否需要重画了。非动画模式绘画，只画一次
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
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：
********************/
class CSkinButton : public CSkinCtrl // 普通按钮, 包括图，文字，图和文字三种样式
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
	BOOL m_btChange; //按钮有动作时为True
	BOOL m_auto; // 按钮是否是动画效果。TRUE是动画效果。
	DWORD m_crBk;
	BOOL m_bChoose;
	Bitmap *m_pLeftImg;
	Bitmap *m_pRightImg;
	BOOL  m_bCanDrag;			// 是否需要在拖动时，发送消息。

	CString m_strImgPath;
	CString m_strSecondImgPath;
	BOOL	m_CanClickAlpha;		//透明度可以点击


	BOOL	 m_bUseClrFillBk;		//是否采用纯色填充背景色
	DWORD m_dwNormalClr;			//纯色填充，正常的填充色
	DWORD m_dwOverClr;				//纯色填充，划过的填充色
	DWORD m_dwDownClr;				//纯色填充，按下的填充色

protected:
	BOOL m_bMoveShow;
	BOOL m_bMultiImg;
	int  m_nDownPos;
	// 按下是否偏移
	BOOL m_bDownMove;

	ISkin *m_pSkinOver;
	ISkin *m_pSkinDown;
	DWORD m_dwTextOverColor;
	DWORD m_dwTextColor;
};


/******************
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：2010.03.03
* 功能说明：此类是网络任务本地信息等控件的切换按钮。
*           实现Tab中按钮中的选项的....不知道如何描述。
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
	BOOL m_showBt;		//Option按钮是否显示
	BOOL m_bAlwaysSendMSG ; //是否一直发送消息
	BOOL m_optionOver; //鼠标在option按钮
	CString m_OptionBtstrID;
	BOOL m_bChangeFont;
	BOOL m_bInnermsg;
	
	BOOL  m_bTabMsg ;			//是否在TabFrame里面
	DWORD m_dwFramOverColor   ; //外边框鼠标移动上去颜色
	DWORD m_dwFramChooseColor ;	//外边框选中颜色
	BOOL  m_bDrawFrame ;		//是否显示外边框
	BOOL  m_bVip ;				//是否Vip属性 
};

/******************
* 当前版本：1.0
* 作    者：夏成聪
* 完成日期：
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
	void SetLineSpace(int nSpace);				//设置行距
	void SetMultiLineByChar(BOOL bByChar);		//设置是否按字符换行
	BOOL ShowOmitPlayTime(Graphics *pGraph);	//显示省略的播放时间
	virtual BOOL IsShowRectContain(short x, short y);

	CString m_strText;
	CString m_face;
	float	m_size;
	DWORD	m_color;
	DWORD   m_offcolor;
	DWORD   m_colorDN;
	DWORD	m_colorHover;			// 悬停颜色
	DWORD	m_colorMouseDown;		// 鼠标按下颜色
	DWORD	m_colorDisable;
	BOOL    m_bShowDNclr;
	int		m_style;
	long	m_textWidth;
	float   m_tempsize;
	DWORD	m_alignment;
	BOOL	m_bAutoShow;		// 自动处理显示效果
	BOOL	m_bHotkeyPrefixShow;// 是否将“&”转义为下划线
	int		m_nOffsize_y;
	int     m_nOffsize_x;
	DWORD	m_effect;			// 字体效果
	BOOL	m_bMultiLineByChar;	//按字符换行，否则按词换行
	int		m_nLineSpace;		// 行距
	BOOL	m_bEllipsisChar;	// 显示不完全，则以省略号显示
	BOOL	m_bEllipsisAnimation; // 文字中的省略号动态显示
	BOOL	m_bAutoEllipsis;	// 自动省略号动态显示
	int		m_nShowDotCount;
	DWORD	m_dwTick;			// 计时器
	BOOL	m_bLinkTextStyle;	// 超链接样式
};

/******************
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：2010.04.20
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
* 当前版本：1.0
* 作    者：陈志标
* 完成日期：2014.07.02
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
	int					 m_nIndexRoll  ;  //当前滚动的Item索引
	DWORD				 m_dwRollTimes ;  //切换文字连接时间频率
	int					 m_nRollSpeed ;  //滚动速度, 滚动一次的时间
	int					 m_nRollStep;  ;  //滚动的步长
	int					 m_nRollTotal ;	  //滚动距离
	BOOL				 m_bNeedRoll;	  //是否需要滚动
	BOOL				 m_bExit;
	HANDLE				 m_hThread;
	CRITICAL_SECTION     m_rslock ;
};

/******************
* 当前版本：1.0
* 作    者：wlj
* 完成日期：2010.04.20
********************/
class CSkinHtmlText: public CSkinText
{
public :
	CSkinHtmlText();
	virtual ~CSkinHtmlText();
	void Paint(Graphics *pGraph);

protected:
	COLORREF StrToRGB(const wchar_t* strClr);			//解析字符串中的rgb值
	void RenderHtml(HDC hDC, LPCTSTR lpszContent, Rect rcText, int nLineHeight = 0);
};
/******************
* 当前版本：1.0
* 作    者：夏成聪
* 完成日期：
********************/
class CSkinImage : public CSkinCtrl	// 图片
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
	void PaintBloom(Graphics *pGraph);		//绘制绽放图片

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
	BOOL m_bShowOnce ;		//只显示一遍

	BOOL	m_bAnimation;	//是否是动画
	int		m_iDelayTime;	//延迟总时间
	int		m_iClockTime;	//延迟时间计时器
	BOOL	m_bStatic;		//是否为静态图片
	BOOL	m_bAuto;		//动画显示
	BOOL	m_bSingleStretch;//拉伸图片,非拉伸方向大小不变
	BOOL	m_bBloom;		//等比缩放，图片绽放以适应区域
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
* 当前版本：1.0
* 作    者：wlj
* 完成日期：
********************/
class CSkinTextureImg : public CSkinImage	// 图片
{
public:
	CSkinTextureImg();
	virtual ~CSkinTextureImg(){};
	void Paint(Graphics *pGraph);
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
public :
	int m_nHCount;		//水平纹理贴图个数
	int m_nVCount;		//垂直纹理贴图个数
};


class CSkinLinkImage : public CSkinImage	// 图片
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

	CString			m_strURL;		// 链接URL
	Rect			m_rcLink;		// 连接感应区
	BOOL			m_bEnableLink;	// 链接是否可用
	BOOL			m_bAutoLinkRc;	//  自动调节链接区域
	
};

/******************
* 当前版本：
* 作    者：匡晓林
* 说    明: 根据不同状态，显示不同图片
			状态对应图片的序号
* 完成日期：
********************/
class CSkinStateImage : public CSkinImage
{
public:
	CSkinStateImage();
	void Paint(Graphics *pGraph);
	virtual long Method_(LPCTSTR szMethod, long para1, long para2);
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
public:
	int		m_nStates;				// 当天的状态
	int		m_nStatesImgNum;		// 状态数
};

/******************
* 当前版本：1.0
* 作    者：
* 完成日期：
* 功能：
********************/
class CSkinCtrlForm : public CSkinCtrl
{
public:
	CSkinCtrlForm();
	virtual BOOL OnTimer();
	void Paint(Graphics *pGraph);
	virtual BOOL CheckCursor(short x, short y);
	virtual BOOL OnMouseDown(short x, short y);								    //鼠标处理消息-按下
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

	// 多控件
	ISkin *m_pSkinOver;
	ISkin *m_pSkinDown;
	ISkin *m_pSkinSel; // 当前选择的控件
private:
	
};

/******************
* 当前版本：1.0
* 作    者：夏成聪
* 完成日期：3.17
* 功能：类似static控件
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
* 当前版本：1.0
* 作    者：夏成聪
* 完成日期：
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
* 当前版本：1.0
* 作    者：张恺
* 完成日期：
* 说明：用于媒体库列表的按钮
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
* 当前版本：1.0
* 作    者：夏成聪
* 完成日期：
********************/
class CSkinButton_Step : public CSkinButton_Text // 渐进按钮
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
**     类名：CSkinMenuBar
** 功能说明：菜单栏，鼠标按钮的容器，根据自身宽度，完成按钮
**           的动态坐标计算，动态布局，记录菜单栏菜单是否弹
**           出，弹出菜单标号，标号记录为菜单按钮对应次序。
**           响应左右键消息；接收菜单消失消息，变化按钮状态；
**           响应ESC键；响应Alt键；响应菜单热键；响应回车键等。
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
	UINT					m_Alternation;	// 是否自动显示下一个菜单；0不自动；1鼠标移动；2左右键；3快捷键；
	INT						m_nowmenuId;	// 弹出菜单id
	INT						m_lastmenuId;	// 弹出菜单id
	std::map<int, HMENU>	m_btnId2Hmenu;	// 菜单Id和菜单句柄表
	ISkin*					m_pOver;		// 悬浮按钮
	ISkin*					m_pDown;		// 按下按钮
	INT						m_rightEdge;	// 到窗口右边小距离
	INT						m_interPixel;	// 文字到按钮单侧距离
	static CSkinMenuBar*	_pInstance;		// 保存当前实例，用于回调键盘处理
	HHOOK					m_hMsgHook;		// 菜单栏用到的菜单钩子
	CSkinColorMenu*			m_pColorMenu;	// colormenu实例
	CString					m_strStyle;		// 菜单栏菜单的风格
	HWND					m_hParent;		// sysBtns句柄
	HWND					m_hMainWnd;		// 主窗口句柄，用于处理菜单消息
	HMENU					m_curHmenu;		// 当前菜单句柄
	UINT					m_lastItemID;	// 菜单项ID
	UINT					m_nextbtnId;	// 下一步选中的菜单按钮id，用于左右键
	BOOL					m_bHiliteFirst;	// 是否高亮第一个项
	CPoint					m_cpMousePos;	// 记录菜单弹出是鼠标位置
};

/*
**     类名：CSkinMenuButton
** 功能说明：菜单按钮，鼠标按下后弹出菜单。总共有普通（normal）、
**           悬浮（hover）、按下（down）、不可用（disable）四种状态，
**           其中按下状态一直到菜单移走或消失，之后恢复普通状态。
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
**     类名：CSkinVolumeCtrls
** 功能说明：音量控件，包含三个按钮，分别是“静音”、
**           “取消静音”和一个音量滑块按钮，按钮间隙
**           可通过配置文件调整；完成三个按钮的布局，
**           消息传递等功能。
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
	int						m_numImg;		// 图片数
	ISkin*					m_pOver;		// 悬浮按钮
	ISkin*					m_pDown;		// 按下按钮
	ISkin*					m_pVolume;		// 音量拖杆
	ISkin*					m_pMute;		// 静音按钮
	ISkin*					m_pUnMute;		// 取消静音按钮
	CSkinWindow*			m_pWnd;			// 音量窗口
	BOOL					m_bVolumeShow;	// 音量窗口是否正在显示
	int						m_nWndOffsetX;
	int						m_nWndOffsetY;
};

/******************
* 当前版本：1.0
* 作    者：夏成聪
* 完成日期：
********************/
class CSkinButton_Check : public CSkinButton_Text //check按钮
{
public:
	CSkinButton_Check();
	virtual ~CSkinButton_Check(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Paint(Graphics *pGraph);
	BOOL CheckCursor(short x, short y);
	BOOL OnMouseUp(short x, short y);
	long Method_(LPCTSTR szAttrib, long para1, long para2); //可以由这个成员函数设置check状态。para1--0,1
	BOOL m_bCheck;				//check按钮的默认状态
	BOOL m_bHalfCheck;			//半选状态
	BOOL m_bAdaptWords;		//是否适应文字宽度
	UINT m_nTxtWidth;
	int m_childNum;
	int m_checkNum;
};

/******************
* 当前版本：1.0
* 作    者：陈志标
* 完成日期：2013.12.10
* 功能说明：单选按钮可以通过此类与TabFrame实现自动勾上。
********************/
class CSkinRadioBox : public CSkinButton_Check
{
public:
	CSkinRadioBox();
	virtual ~CSkinRadioBox(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	BOOL OnMouseUp(short x, short y);
	long Method_(LPCTSTR szAttrib, long para1, long para2); //可以由这个成员函数设置check状态。para1--0,1

private:
	BOOL m_bTabMsg ;
	BOOL m_bVip ;
};


/******************
* 当前版本：1.0
* 作    者：夏成聪
* 完成日期：
********************/
class CSkinTableImage: public CSkinCtrl
{
public:
	CSkinTableImage();
	virtual ~CSkinTableImage(){};
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Clear();
	void Paint(Graphics *pGraph);

	UINT m_nSrcx;			//图片左上的X坐标
	UINT m_nSrcy;			//图片左上的Y坐标
};

/******************
* 当前版本：1.0
* 作    者：夏成聪
* 完成日期：
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
	long Method_(LPCTSTR szAttrib, long para1, long para2);//para1--下标， para2: 0--up, 1--down
	virtual BOOL OnTimer();
	UINT	m_nDownIndex;		//按下标签的下标
	int		m_nIndex;			//当前标签的下标
	int		m_nPreIndex;		//被切换标签的下标
	UINT	m_nSonWidth;		//子标签的宽度
	UINT	m_nSonHeight;		//子标签的高度
	BOOL	m_bVerical;		//是否是垂直类型
	BOOL	m_bHorizontal;		//是否是水平类型
	BOOL	m_bBkMove;		//背景是否移动
	UINT	m_nCount;		//背景图移动的位移
	COLORREF m_crBord;		//边框颜色
};

/******************
* 当前版本：1.0
* 作    者：夏成聪
* 完成日期：
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
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：
* 功能说明：CSkinTabBtns是用来实现多个按钮进行切换时，动态效果
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
	virtual BOOL OnMouseDrag(short x, short y);								//鼠标处理消息-拖动
	int  CheckObj(short x, short y);
	BOOL CheckCursor(short x, short y);
	void Paint(Graphics *pGraph);

	void SetChangeColor(BOOL bChange);
	void SetFocusColor();
	void SetItem(int nitem);
	// 是否被拖拽
	BOOL GetbDrag() ;

private:
	BOOL m_bHorizontal;	//是否是水平移动
	int  m_nOffsetper;	//每次移动的偏移量
	int  m_nOffset;		// 总共的偏移量
	int	 m_ntotal;		//如果是水平移动，那么就是总宽，否则是总高。
	DWORD m_dfocusColor;
	DWORD m_dnormalColor;
	ISkin *m_pSkin;
	ISkin *m_pHotSkin;
	int m_nCurItem;	//当前按下的子控件。
	int   m_nOldItem;	//之前按下的子控件。
	BOOL  m_nOutSetItem;	//外部设置
	BOOL  m_bChangeColor;//需要修改颜色通道
	BOOL m_bDrag;
	BOOL m_bMouseDnChangeItem ;
};


/******************
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：
* 功能说明：Edit中的向上(增加)向下(减少)按钮
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
	static LRESULT UpDownProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);		//窗口子类化处理函数
	void OnPaint(HWND hWnd);

	BOOL			m_bFocus;			//是否为focus状态
private:
	WNDPROC			m_UpDownOldProc;

	Bitmap *		m_pBtImg;			//按钮图像
	COLORREF		m_clrNormalFrame;	//normal边框颜色						
	COLORREF		m_clrHoverFrame;	//hover边框颜色		
	COLORREF		m_clrBK;			//背景颜色

	int				m_nHeight;
	int				m_nWidth;
	BOOL			m_bUpOver;
	BOOL			m_bDownOver;
	BOOL			m_bUpDown;
	BOOL			m_bDownDown;
	HDC				m_memDC;										//绘图时用的内存DC
	HBITMAP			m_memBmp;										//绘图时用的内存位图
};

//////////////////////////////////////////////////////////////////////////////////////
//
// 当前版本：1.0
// 作    者：罗伟坚
// 完成日期：
// 功能说明：HotKey自绘控件
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
	//外部接口功能组
	//
	void SetModHotKey(BYTE fVirt, WORD key);											//设置Mod热键
	void GetModHotKey(BYTE & fVirt, WORD & key);										//获取Mod热键
	void GetAccelKey(BYTE & fVirt, WORD & key);											//参数为返回的fVirt虚拟组合键值, key按键值 --参考结构:ACCEL
	void SetAccelKey(BYTE fVirt, WORD key);												//参数为: fVirt虚拟组合键值(FALT|FCONTROL|FSHIFT), key按键值 --参考结构:ACCEL
	void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);								//HotKeyCtrl的标准热键设置接口
	void GetHotKey(WORD & wVirtualKeyCode, WORD & wModifiers);							//获取热键值
	void SetPos(int x, int y);															//设置窗口位置
	void SetState(skin_state state);													//设置控件状态

	void OnPaint(void);																	//控件绘制函数
	void EnterNofity(UINT uMsg, WPARAM wp);											    //对回车响应操作--通知父窗口
	//
	//内部功能组
	//
protected:
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);				//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();																//反初始化
	virtual BOOL Layout(short nWidth, short nHeight);									//陈列展现控件--该函数由改变窗口大小位置的时候出发WM_SIZE
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);

	static LRESULT SubClass_WndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);		//子类化窗口处理函数
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);									//从XML文件载入皮肤元素的配置
	void     OnNcPaint(void);															//控件NC绘制函数
	COLORREF Change2RGB(COLORREF clr);													//由传统颜色顺序ARGB转成GBR字节位顺序
	void	 SetHotKey_Fix_Work(void);													//设置热键后的处理工作
	CString  GetStringFromVk(UINT vk);													//由Vk虚拟值获取相应的字符表达

protected:
	CString			m_strHotKey;									//热键显示的内容

	WNDPROC			m_OldWndPrc;									//保存子类化前的窗口处理
	HDC				m_memDC;										//绘图时用的内存DC
	HBITMAP			m_memBmp;										//绘图时用的内存位图
	CRect           m_oldMemRect;									//用于表示窗口大小(内存位图不用重新分配)
	HFONT			m_hTextFont;									//文字样式句柄
	COLORREF		m_clrBK;										//背景颜色			
	COLORREF		m_clrNormalFrame;								//normal边框颜色						
	COLORREF		m_clrHoverFrame;								//hover边框颜色							
	COLORREF		m_clrNormalText;								//normal文字颜色						
	COLORREF		m_clrHoverText;									//hover文字颜色	
	COLORREF		m_clrHighLightBK;								//高亮选中文字背景颜色	
	HBRUSH			m_hBrBK;										//背景颜色
	HBRUSH			m_hBrNormalFrame;								//normal边框颜色
	HBRUSH			m_hBrHoverFrame;								//hover边框颜色	
	HBRUSH			m_hBrNormalText;								//normal文字颜色
	HBRUSH			m_hBrHoverText;									//hover文字颜色		
	HBRUSH			m_hBrHighLightBK;								//hover文字颜色		
	HPEN			m_hFramePen;									//边框的画笔
	HPEN			m_hFrameHoverPen;								//边框的画笔
	BOOL			m_bFocus;										//是否Focus状态
	BOOL			m_bMouseDown;									//是否Focus状态
	BOOL			m_bEnterNotify;									//是否对回车响应操作--通知父窗口
};

/******************
* 当前版本：1.0
* 作    者：夏成聪   修改 by 罗伟坚
* 完成日期：
********************/
//目前只修改了默认背景，默认字体颜色。。选中的字体颜色与选中背景颜色总是蓝底白字。未解决！
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
	//外部接口功能组
	//

	void SetPos(int x, int y);															//设置窗口位置
	BOOL IsUpDownStyle(void);															//判断是否upDown样式
	void SetUpDownStepLen(int nStepLen);												//设置UpDown控件的步长
	int  GetEditStyle(void);															//获取Edit的样式类型 0 Text, 1 num ,2 time, 3 updown
	void SetState(skin_state state);													//设置控件状态
	void SetTextFont(const LOGFONT & lf);												//设置字体样式
	HFONT GetTextFont(void);															//获取字体样式
	void SetUpDownNofityFlag(bool bAuto);												//设置在SetWindowText时是否回调通知
	bool GetUpDownNofityFlag(void);														//获取在SetWindowText时是否回调通知
	BOOL Create_SimpleMode(HWND hParentWnd, RECT & rtCtrl,DWORD dwStyle, 
						   IQvodCallBack *pEventEntry,DWORD dwExStyle=0);									//创建控件  -只修改简单的消息回调通知父窗口
	void SetText(LPCTSTR lpText);
	CUpDownCtrlEx	m_UpDownCtrl;
	
	void SetBkColor(DWORD dwARGB);

	//unDown控件
	//
	//内部功能组
	//	
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	void LegalNumber();
	virtual BOOL Layout(short nWidth, short nHeight);									//陈列展现控件--该函数由改变窗口大小位置的时候出发WM_SIZE
protected:
	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);				//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();



	static LRESULT SubClass_WndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);		   //子类化窗口处理函数
	static LRESULT SimPleMode_SubClass_WndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);//窗口子类化处理函数--简单模式
	void	 LoadSkinConfigFromXML(CXMLElement &xmlNode);								//从XML文件载入皮肤元素的配置
	LRESULT  OnChar(UINT nMsgType, WPARAM wParam, LPARAM lParam, BOOL& bHandled);		//键盘输入处理函数
	void     OnPaint(void);																//控件绘制函数
	void     OnNcPaint(void);															//控件NC绘制函数
	COLORREF Change2RGB(COLORREF clr);													//由传统颜色顺序ARGB转成GBR字节位顺序
	void	 ArrowLeft_Work(void);														//左箭头按键处理	--由于WTL封装的CEdit没有对箭头处理,需自身实现
	void	 ArrowRight_Work(void);														//右箭头按键处理	--由于WTL封装的CEdit没有对箭头处理,需自身实现
	BOOL	 EnterNofity(UINT uMsg, WPARAM wp,LPARAM lp);								//对回车响应操作--通知父窗口
	void	 DealWithDelete(WPARAM wParam, LPARAM lParam, BOOL& bHandled);				//处理时间状态时的back 和 delete消息
	void	 DealTimeCaret(WPARAM wParam, BOOL& bHandled);								//处理时间状态时方向键
	void	 LegalTime(WPARAM wParam, LPARAM lParam, BOOL& bHandled);					//时间是否合法
	void	 DealLButtonDown(LPARAM lParam);											//时间状态时鼠标点击

protected:
	WNDPROC			m_OldWndPrc;									//保存子类化前的窗口处理
	HDC				m_memDC;										//绘图时用的内存DC
	HBITMAP			m_memBmp;										//绘图时用的内存位图
	CRect           m_oldMemRect;									//用于表示窗口大小(内存位图不用重新分配)

	HFONT			m_hTextFont;									//文字样式句柄
	COLORREF		m_clrBK;										//背景颜色			
	COLORREF		m_clrNormalFrame;								//normal边框颜色						
	COLORREF		m_clrHoverFrame;								//hover边框颜色							
	COLORREF		m_clrNormalText;								//normal文字颜色						
	COLORREF		m_clrHoverText;									//hover文字颜色	
	COLORREF		m_clrHighLightBK;								//高亮选中文字背景颜色	
	HBRUSH			m_hBrBK;										//背景颜色
	HBRUSH			m_hBrNormalFrame;								//normal边框颜色
	HBRUSH			m_hBrHoverFrame;								//hover边框颜色	
	HBRUSH			m_hBrNormalText;								//normal文字颜色
	HBRUSH			m_hBrHoverText;									//hover文字颜色		
	HBRUSH			m_hBrHighLightBK;								//hover文字颜色		
	HPEN			m_hFramePen;									//边框的画笔
	HPEN			m_hFrameHoverPen;								//边框的画笔
	int             m_nStyle;										//edit的样式
	int				m_nMaxLen;										//字符串的最大允许长度
	int				m_nInterval;									//unDown控件的每步增减的步长(一次跳多少)
	int				m_nNumRangeMin;									//unDown控件的范围最小值
	int				m_nNumRangeMax;									//unDown控件的范围最大值
	int				m_nUpdownCtrlPos;								//unDown控件的值
	int				m_nAuto;										//控件的Auto属性
	BOOL			m_bBorder;										//是否有边框
	BOOL			m_bFocus;										//是否Focus状态
	// 是否可以输入负号
	BOOL            m_bExistNegativesign;
	BOOL			m_bMouseDown;									//是否Focus状态
	BOOL			m_bEnterNotify;									//是否对回车响应操作--通知父窗口
	bool			m_bUpDownNofity;								//是否启动updown设置edit内容回调
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
	BOOL				m_bFilterSpace;			// 过滤空格
	ISkin*				m_pOverText;
	HWND				m_hOwn;
	BOOL				m_bChildWnd;			//继承父窗体属性
	BOOL				m_bIsChild;		// 子窗口
};

/******************
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：
* 功能说明：此类是任务列表项中的一个控件，它显示任务项中的
*           文件类型，以及文件下载的进度。控制文件下载。
********************//*
class CSkinProgress : public CSkinCtrl	// 进度条
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
	int m_proX;	//进度条显示的X位置
	int m_proY;	//进度条显示的Y位置

	int m_proBmpW; //进度条的总宽度
	int m_imgsW; //左右两张图的总宽度
	int m_hasimgW; //已下载的图片的宽度
	Rect *m_pProRect;
	TCHAR m_fileBuff[255];
	HICON m_hico;
	int m_logoH; //系统文件ICON的高度 
	int m_logoW; //系统文件ICON的宽度
	BOOL m_isFinished;

	BOOL m_stopover; //鼠标在暂停/取消按钮上。
};*/

class CSkinProgressBar : public CSkinCtrl	// 进度栏
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

	BOOL  m_bWaterStyle;		//是否水涨类型进度
	BOOL  m_bHProgress;		    //是否水平水涨类型
};

/****
*
*此结构主要是为了播放文件的已下载段。
*
************/
struct _TIMEPOS
{	
	float beginPos;
	float endPos;
	struct _TIMEPOS *NextNode;
};

// CSkinSlider消息
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
* 控件名称: CSkinSlider,滑动条/拖杆
* 当前版本：1.01
* 作    者：刘建敏, 修改 罗伟坚
* 完成日期：
* 功能说明：此类是显示和控制视频文件播放程度的控件。
********************/
class CSkinSlider : public CSkinCtrl	// 拖杆
{
	//
	//外部接口功能组
	//
public:
	void  SetPos(float pos);													//设置播放位置：pos是百分比
	float GetPos();																//获取播放位置：pos是百分比
	void  ClearPos();															//重置播放位置
	void  SetCanDrag(BOOL bCanDrag);											//设置控件是否可以拖动
	float GetCurrPos();		
	void  SetAlpha(float alpha);					// 设置透明度

public :

	//
	//循环播放功能组
	//
	//说明:所有float类型就是标识位置进度,最大值为整数1.0

	//段播放函数的声明定义
	typedef void (*SecPlayPrc)(float);
	void SetSecPlayCallBackFun(SecPlayPrc  pCallBackFun);							//设置段播放播放越界时候的回调函数

#define SLIDER_SEC_PLAY_BORDER_NUM                            2                     //循环播放段的2端线
#define SLIDER_SEC_PLAY_POINT_S                               0						//循环播放段的起点
#define SLIDER_SEC_PLAY_POINT_E                               1						//循环播放段的终点
#define SLIDER_SEC_PLAY_INVALID                               -1.0					//循环播放段位置无效值



	//
	//内部实现功能组
	//
public:
	CSkinSlider();
	virtual ~CSkinSlider();

	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);				//初始化

	virtual BOOL OnTimer();														//定时器处理函数
	virtual BOOL Layout(short nWidth, short nHeight);							//陈列展现控件

	virtual BOOL OnMouseMoveIn(short x, short y);								//鼠标处理消息-移进控件
	virtual BOOL OnMouseMoveOut();												//鼠标处理消息-移出控件
	virtual BOOL OnMouseDown(short x, short y);								    //鼠标处理消息-按下
	virtual BOOL OnMouseUp(short x, short y);									//鼠标处理消息-释放
	virtual BOOL OnMouseDrag(short x, short y);									//鼠标处理消息-拖动
	virtual BOOL OnMouseMove(short x, short y);									//鼠标处理消息-移动

	virtual BOOL CheckCursor(short x, short y);									//检测(x,y)是否在控件上
	virtual BOOL CheckBlockCursor(short x, short y);							//检测(x,y)是否在Block上

	void Paint(Graphics *pGraph);												//绘制控件
	void Clear_();																//清理资源 释放控件资源
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);							//从XML文件载入皮肤元素的配置
	ISkin * GetSkinObj(short x, short y);

	SecPlayPrc  m_pCallBackFun;													//段播放回调函数指针
	int	  m_bSetPlayCheckTimer;													//是否启动段播放检测定时器

	//定义图片所属的区域位置
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

//滑动槽在鼠标按下时候移动的样式
#define SLIDER_MOVING_STYLE_DIRECT_MOVE                  0							//直接跳转到鼠标点击位置				
#define SLIDER_MOVING_STYLE_STEP_MOVE                    1							//逐步靠近到鼠标移动位置

	float m_fStepMoveLen;                                                       //逐步移动方式--每次移动的步长 MAX=100
	int m_nBlockMoveStyle;                                                      //滑动槽在鼠标按下时候移动的样式  

	Bitmap  *m_vBmp[11];														//存储各个状态的绘制位图
	float m_fPos;																//视频播放的百分比
	BOOL m_moveIn;																//鼠标进入Slider
	BOOL m_blockOver;															//鼠标在Block上
	BOOL m_downBlockOver;														//鼠标按下在Block上
	BOOL m_blockDown;															//鼠标点下Block滑块
	BOOL m_flagBlock;															//鼠标在Block上
	BOOL m_bDrag;																//拖拽状态
//	struct _TIMEPOS *m_structTime;												//时间结构指针
	int m_NodeNum;																//节点元素数
	int m_validWidth;															//基本的宽度
	int m_validHeight;															//基本的高度
	int m_invalidHeadLen;                                                       // 进度条首尾的无意义填充长度
	BOOL m_bVertical;															//控件摆放类型-垂直/水平
	DWORD m_filltype;															//中间像素是拉伸还是直接贴图
	int m_tileddistance;														//直接贴图，若鼠标拖动，其最小改变间隔
	int m_nImgNumber;															//图像数
	int m_nOldDistance;															//鼠标Down的位置和实际Pos位置的差值。

	int m_nDownMove;															//记忆鼠标点中滑块的位置与中间位置的偏移X/Y值
	int m_nOldBlockMidPos;														//记忆鼠标点中滑块的位置X/Y值
	BOOL m_bCanSeek;															//是否能拖动

	BOOL m_bDownInAdhereArea;													//是否按下时在吸附区
	float m_fCurrPos;															//鼠标当前所在的位置
	float m_fAlpha;			// 透明度值
	BOOL  m_nAutoHide;		// 0:不隐藏，1:自动隐藏，2:带动画效果的自动隐藏
	int	  m_nBgOffX;
	int	  m_nBgOffY;
	BOOL  m_bShowBlock;

	// 多控件
	ISkin *m_pSkinOver;
	ISkin *m_pSkinDown;
};


/******************
* 控件名称: CSkinCutSlider, 视频截取滑动条
* 当前版本：1.01
* 作    者：刘建敏
* 完成日期：
* 功能说明：此类是显示和控制视频截取播放程度的控件。
********************/
class CSkinCutSlider : public CSkinCtrl	// 拖杆
{
public:				//外部接口功能组
	void  SetBeginPos(float pos, BOOL bSetMsg);			// 设置起始位置
	float GetBeginPos();					// 获取起始位置
	void  SetEndPos(float pos, BOOL bSetMsg);				// 设置终止位置
	float GetEndPos();						// 获取终止位置
	void  SetPreViewPos(float pos);			// 设置预览位置
	void  StartPreView(BOOL bStart);			// 停止预览

public:
	CSkinCutSlider();
	virtual ~CSkinCutSlider();

	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);				//初始化
	void Clear_();																//清理资源 释放控件资源

	void Paint(Graphics *pGraph);												//绘制控件
	BOOL OnTimer();																//定时器处理函数
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);
	virtual BOOL CheckCursor(short x, short y);									//检测(x,y)是否在控件上
	void CheckBlock(short x, short y);											// 检测(x, y)是否在左右的块上
 
 	virtual BOOL OnMouseMoveIn(short x, short y);								//鼠标处理消息-移进控件
 	virtual BOOL OnMouseMoveOut();												//鼠标处理消息-移出控件
	virtual BOOL OnMouseMove(short x, short y);									//鼠标处理消息-移动
 	virtual BOOL OnMouseDown(short x, short y);								    //鼠标处理消息-按下
 	virtual BOOL OnMouseUp(short x, short y);									//鼠标处理消息-释放
 	virtual BOOL OnMouseDrag(short x, short y);									//鼠标处理消息-拖动




	//定义图片所属的区域位置
	enum Pos_Img
	{
		slider_bg      = 0,
		slider_leftblock,		
		slider_rightblock,
		slider_playblock
	};

	Bitmap  *m_vBmp[4];															//存储各个状态的绘制位图
	float	m_fBeginPos;														// 左边的位置
	float   m_fEndPos;															// 右边的位置
	int		m_nBMove;															// 左边开始的位置
	int		m_nEMove;															// 右边结束的位置
	BOOL    m_bLOver;															// 是否在左边块上
	BOOL	m_bLDown;															// 是否在左边的块的按下
	BOOL	m_bROver;															// 是否在右边块上
	BOOL	m_bRDown;															// 是否在右边块上按下
	BOOL	m_bThumbOver;														// 是否在进度条上
	int		m_nOldX;															// 点击点
	float	m_fProPos;															// 截取进度
	BOOL	m_bNeedProgress;													// 

	BOOL    m_bPreview;															// 开始播放
//	float   m_fPlayPos;															// 播放进度的相对位置
	float   m_fAbPlayPos;														// 播放进度的绝对位置
	BOOL    m_bBlockOver;														// 播放进度块
	BOOL    m_bBlockDown;														// 播放进度块按下


	float m_fStepMoveLen;                                                       //逐步移动方式--每次移动的步长 MAX=100
	int m_nBlockMoveStyle;                                                          //滑动槽在鼠标按下时候移动的样式  

	
	float m_fPos;																//视频播放的百分比
	BOOL m_moveIn;																//鼠标进入Slider
	BOOL m_blockOver;															//鼠标在Block上
	BOOL m_downBlockOver;														//鼠标按下在Block上
	BOOL m_blockDown;															//鼠标点下Block滑块
	BOOL m_flagBlock;															//鼠标在Block上
	BOOL m_bDrag;																//拖拽状态
	int m_NodeNum;																//节点元素数
	int m_validWidth;															//基本的宽度
	int m_validHeight;															//基本的高度
	BOOL m_bVertical;															//控件摆放类型-垂直/水平
	int m_nImgNumber;															//图像数
	int m_nOldDistance;															//鼠标Down的位置和实际Pos位置的差值。

	int m_nDownMove;															//记忆鼠标点中滑块的位置与中间位置的偏移X/Y值
	int m_nOldBlockMidPos;														//记忆鼠标点中滑块的位置X/Y值
	BOOL m_bCanSeek;															//是否能拖动

	BOOL m_bDownInAdhereArea;													//是否按下时在吸附区
	float m_fCurrPos;															//鼠标当前所在的位置
};

/******************
* 控件名称: 吸附滑动条
* 当前版本：1.01
* 作    者：罗伟坚
* 完成日期：
* 功能说明：拖动时接近中间点自动吸附到中点的滑动条
********************/
class CSkinAdhereSlider:public CSkinSlider
{

	//
	//外部功能接口组
	//
public:
	void SetAdhereRange(int nLowProcent, int nHightProcent);					//设置吸附的区域范围 -参数为百分比位置
	void GetAdhereRange(int & nLowProcent, int & nHightProcent);				//获取吸附的区域范围 -参数为百分比位置



	//
	//内部功能实现组
	//
public:
	CSkinAdhereSlider();
	virtual ~CSkinAdhereSlider();


	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);		//初始化
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);							//从XML文件载入皮肤元素的配置


	virtual BOOL OnMouseDrag(short x, short y);									//鼠标处理消息-拖动
	virtual BOOL OnMouseDown(short x, short y);								    //鼠标处理消息-按下
	virtual BOOL OnMouseUp(short x, short y);									//鼠标处理消息-释放


	float m_nLowProcent;														//吸附区域最小值
	float m_nHightProcent;														//吸附区域最大值
	float m_fOldSAdherePos;														//记录吸附前的位置
};

/******************
* 控件名称: CSkinSlider,滑动条/拖杆
* 当前版本：1.01
* 作    者：刘建敏
* 完成日期：
* 功能说明：此类是显示和控制视频文件播放程度的控件。
********************/
class CSkinPlaySlider:public CSkinSlider
{
	//
	//外部功能接口组
	//
public:
	BOOL OnMouseMove(short x, short y);									            //鼠标处理消息-移动
	BOOL OnMouseMoveOut(void);												        //鼠标处理消息-移出控件
	void  SetSectionPoint_S(float fPos);											//设置段落播放起点
	void  SetSectionPoint_E(float fPos);											//设置段落播放终点
	float GetSectionPoint_S();														//获取段落播放起点
	float GetSectionPoint_E();														//获取段落播放终点

	BOOL  SectionPlay_Start();														//开始循环播放段
	BOOL  SectionPlay_Stop();														//取消循环播放段
	BOOL  IsSectionPlayNow();														//判断当前施法循环播放状态

	long GetValideWidth();															//获取有效区域宽度
	void SetLoadInfo(char *pBitInfo, int npicNum, int nBuffer, int nIndexBegin, int nIdexLen);		// 设置下载信息 只完成数据的更新
	void ClearLoadInfo();															//清除下载信息
	void DrawBitBmp();																//绘制已下载的
	bool DrawVTBmp(Graphics & gra);													// 绘制视频标签的标记

	//
	//内部功能实现组
	//
public:
	CSkinPlaySlider();
	virtual ~CSkinPlaySlider();

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);			//初始化
	virtual void Clear_();															//反初始化-清理资源
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);					//类似Com类型接口
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);								//从XML文件载入皮肤元素的配置
	void Paint(Graphics *pGraph);													//绘制控件
	int  GetMiniH();
public:
	bool m_bSurportQmv;																// 当前进度条需不需要支持QMV

protected:
	BOOL CheckIndex(int index);														//查询该片是否已下载

	int m_nMiniH;
	float m_fStartPoint;															//循环播放段起点位置
	float m_fEndPoint;																//循环播放段终点位置
	BOOL  m_bSectionPlay;															//当前是否正在循环播放中
	BOOL  m_bSectionPlayStyle;														//是否拥有循环播放样式	
	BOOL  m_bHi;																	//当前是否为正常(高)状态

	char *m_pOldBitInfo;			// Bit 位上一次发送的信息
	char *m_pNewBitInfo;			// Bit 位最新的一次发送的信息
	int	  m_nPicNum;				// 片数
	int	  m_nBuffer;				// m_pNewBitInfo的大小
	Bitmap *m_pLoadBmp;				// 下载进度图
	BOOL  m_bBeginLoad;				// 第一次开始设置Bit位。
	int	  m_nBeginPic;				// 文件索引开始
	int   m_nIndexLen;				// 索引长度

	CRITICAL_SECTION m_csBitInfo;	// 因为绘制和更新Bit位信息是两个不同的线程操作m_pNewBitInfo.所以加锁。
};

/******************
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：
* 修改日期: 2010.3.31
* 功能说明：此类是显示和控制声音的控件。
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
	virtual BOOL OnMouseDown(short x, short y);	// 鼠标按下
	virtual BOOL OnMouseUp(short x, short y);		// 鼠标按上
	virtual BOOL OnMouseDrag(short x, short y);	
	virtual BOOL OnMouseMove(short x, short y);
	virtual BOOL CheckCursor(short x, short y);
	virtual BOOL Layout(short nWidth, short nHeight);

	BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);
	void Paint(Graphics *pGraph);
	void Clear();
	BOOL CheckBlockCursor(short x, short y);	//检测鼠标是否在托块上
	BOOL CheckThrumpet(short x, short y);		// 检测小喇叭
	BOOL CheckDragBar(short x, short y);		// 检测移动块
	void SetPos(int pos);						// 设置位置
	int GetPos();								// 获取位置
	void SetMute(BOOL isMute);					// 设置静音
	Bitmap * GetImg(int pos);					// 获取音量所对应的音量
	void SetAlpha(float alpha);					// 设置透明度

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

	int m_vPos;				//音量的大小范围是(0-100)
	Bitmap  *m_vBmp[10];
	int m_dowPos;			// 鼠标按下时，在托块所在x轴的位置。
	int m_numImg;			// 图片数
	BOOL m_hasBtn;			// 是否有Btn
	BOOL m_mute;			// 静音
	int m_validLen;			// 可拖动的有效长度。
	int m_bX;				//拖动条的开始位置。
	BOOL m_onBtn;			//鼠标在小喇叭上
	float m_fAlpha;			// 透明度值
	BOOL  m_bAutoHide;		// 自动隐藏
	BOOL  m_bVertical;      // 垂直/水平


};

/******************
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：
* 功能说明：此类是滚动条控件。
********************/
class CSkinScroll : public CSkinCtrl	// 滚动条
{
public:
	CSkinScroll();
	virtual ~CSkinScroll();

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();													//反初始化
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//函数接口--类似COM查询接口
	virtual BOOL OnTimer();													//自动调用定时器--查看是否需要重绘

	virtual BOOL OnMouseMoveIn(short x, short y);			//鼠标移进
	virtual BOOL OnMouseMoveOut();							//鼠标移出
	virtual BOOL OnMouseMove(short x, short y);				//鼠标移动
	virtual BOOL OnMouseDown(short x, short y);				//鼠标按下
	virtual BOOL OnMouseUp(short x, short y);				//鼠标释放
	virtual BOOL OnMouseDrag(short x, short y);				//鼠标拖动
	virtual BOOL CheckCursor(short x, short y);				//测试光标


	void SetVertical(BOOL bVertical);						//设置垂直/水平样式-默认情况下是垂直的滚动条，如果设置bVertical为FALSE，则滚动条为水平
	void SetPageHeight(long nPageHeight);					//设置显示的界面高度(用户可以看得到的)
	void SetDuration(long dur);								//需要显示的界面的总高
	void SetPos(float pos);									//设置滚动条移动的位置
	void SetPosDirect(float pos);
	void SetBtnDownScrollPixel(UINT pixel);					//设置按钮按下一次需要滚动的像素值
	//获取滚动条移动的位置百分比
	float GetPos();
	float GetControlMove();
	void Paint(Graphics *pGraph);							//绘制控件

	//水平滚动条
	void SetPageWidth(long nPageWidth);						//设置显示界面的宽度
	void SetEventEntry(IQvodCallBack *pEventEntry);			//设置回调入口
	int  GetWitdh(void);									//获取滚动条的宽度(图像)

protected:
	BOOL CheckThumb(const short x, const short y);			//测试鼠标是否在滑块区域
	BOOL CheckUpBtn(const short x, const short y);			//测试鼠标是否在Up/Left按钮区域
	BOOL CheckDownBtn(const short x, const short y);		//测试鼠标是否在Down/Right按钮区域
	void UpdateScrollState(int x, int y);					//更新控件状态
	int  UpdateBlockSideMove(short x, short y);				//获取指定坐标离滑块左上角区域移动方向偏移

	// 定义滑块的最小高度
#define MinHeight_Slider		((m_bVertical ? m_nScrollBtnHeight : m_nScrollBtnWidth)*2)

	enum scroll_img
	{
		scroll_up = 0,			// 向上按钮
		scroll_down,			// 向下按钮
		scroll_bg,				// 底图
		scroll_roundtop,		// 圆角的顶部
		scroll_roundmid,		// 移动块的底图
		scroll_midblock,		// 移动块中间的三横的图
		SCROLL_IMG_NUM          // 控件图像数-放最后
	};

	Bitmap  *m_vBmp[SCROLL_IMG_NUM];							//几个状态下的位图
	Bitmap  *m_pMemBmp;											//内存位图
	BOOL	 m_bVertical;										//是否为垂直类型
	long	 m_pageHeight;										//每页显示的高度
	UINT	 m_btnDownScrollPixel;								//按钮按下一次时的滚动像素值
	long	 m_nAllHeight;										//scroll所显示的总共的高度/宽度
	float	 m_fPos;											//滚动条的位置偏移比例 
	float	 m_fDownPos;										//记录鼠标按下时的位置
	int		 m_nWidth;											//滚动条的宽度
 	int		 m_nDownMoveUp;										//鼠标按下时与滑块顶部/左侧边缘的偏移
 	int		 m_nDownMoveDown;									//鼠标按下时与滑块底部/右侧边缘的偏移
	int      m_nOldDragPos;                                     //上一次拖动前的位置

	Rect	 m_paintRect;										//绘制图像的矩形区域-控件大小

	int      m_ScrollState;										//滚动条当前状态

	int      m_nScrollBtnWidth;									//滚动条按钮宽度
	int      m_nScrollBtnHeight;								//滚动条按钮高度
	int      m_nScrollBgWidth;									//滚动条背景宽度
	int      m_nScrollBgHeight;									//滚动条背景高度

	int      m_nScrollRoundTopWidth;							//滚动条按钮宽度
	int      m_nScrollRoundTopHeight;							//滚动条按钮高度
	int      m_nScrollRoundMidWidth;							//滚动条背景宽度
	int      m_nScrollRoundMidHeight;							//滚动条背景高度
	int      m_nScrollMidLockWidth;								//滚动条背景宽度
	int      m_nScrollMidLockHeight;							//滚动条背景高度

	int		 m_nCallPressBtCounter;								//定时器处理按下按钮后连续滚动
};

//网络任务项的数据结构
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

//UI任务项需要的信息
// typedef struct _LISTITEMINFO
// {
// 	long	nType;				// 文件类型 0: 本地 1: 网络
// 	long	nTaskID;			// 任务ID
// 	long	nFileID;			// 文件ID
// 	TCHAR   szName[MAX_PATH];	// 显示名
// 	netstate netState;			// 网络状态 创建1 暂停2 接收3 完成4
// 	float	fProgress;			// 下载进度 0 到 100
// 	net_quality quality;		// 网络质量
// 	BOOL	bPlaying;			// 是否正在播放
// 	// Bitmap* 截图指针
// 	_LISTITEMINFO()
// 	{
// 		memset(this, 0, sizeof(_LISTITEMINFO));
// 	}
// }LISTITEMINFO;

// enum _NETBTN_TYPE
// {
// 	NBT_Finished = 1, //已下载完毕
// 	NBT_StopLoad,	//停止下载
// 	NBT_StartLoad,	//继续下载
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
// 	_CLICK_TYPE clickType; //消息类型
// 	int			item;		//第几个任务项
// 	int			itemID;		//任务项的ID号
// 	netstate     itemnetsate; //网络状态
// 	_NETBTN_TYPE	netbtnState;		//网络下载按钮的状态
// 	_TLVCMESSAGE()
// 	{
// 		clickType = ct_LClick;
// 		item  = -1;
// 		itemID = 0;
// 	}
// 
// }TLVCMESSAGE;

//节点属性
enum item_property
{
	ITEM_NORMAL = 0,   //正常
	ITEM_FOLDER,	   //可收展节点
	ITEM_CHILD,		   //子节点
	ITEM_DIRECTORY	   //目录
};

#define MAX_ICON_COUNT  5
//多媒体列表item结构
struct MUL_ITEM_INFO
{
	int       nID;				//节点元素的编号
	int		  nListIndex;		//列表序号
	int		nSeriesIdx;		//剧集的集数序号
	CString   strName;			//显示文本-名称
	CString   strTime;			//显示文本-时间
	Image *   pImgBmp;			//图片区域图像
	int       nPgPecent;		//进度条百分率 MAX=100
	BOOL      bSimpleShowTime;	//简洁模式下是否显示时间/否则为显示进度 
	int       nIconShowState;   //显示左边小方块图的状态
	int       nState;			//当前节点的状态
	BOOL	  bShowGrayPg;      //是否绘制灰化进度条
	netstate  netState;			// 网络状态
	Rect	  rtText;			// 文字的矩形区域
	item_property nProterty;    //节点属性  0正常 1可收展节点 2子节点
	BOOL      bFold;            //是否为收缩状态,否则为展开状态 --仅对ITEM_FOLDER可展开节点有效
	MUL_ITEM_INFO * pParent;    //父节点指针 --仅对ITEM_CHILD可展开节点有效
	std::vector<MUL_ITEM_INFO*> vtChildItemList;	//子节点列表--仅对ITEM_FOLDER可展开节点有效
	int		nType;			//当前结点类型
	BOOL  bOver;			//剧集是否更新完成，仅对剧集有效
	int      nCount;			//剧集总集数，仅对剧集有效
	int       nVisualPos;       //记录节点的显示位置
	BOOL	  bShowRightIcon[MAX_ICON_COUNT];   //是否显示右边的图标
	BOOL	bCloud;			//是否为私有云播放
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
		for (int i=0; i<vtChildItemList.size(); i++)			//释放分配的子项
		{
			delete vtChildItemList[i];
			vtChildItemList[i] = NULL;
		}
		vtChildItemList.clear();
	}
};

typedef   MUL_ITEM_INFO TASKITEMINFO;

//媒体库表-item项 结构
struct MEDIA_TABLE_ITEM
{
	int       nID;				//节点元素的编号
	int		nSeriesIdx;		//剧集序号
	CString   strName;			//显示文本-名称
	CString   strTime;			//显示文本-时间
	Image *   pImgBmp;			//图片区域图像
	int       nPgPecent;		//进度条百分率 MAX=100
	BOOL      bSimpleShowTime;	//简洁模式下是否显示时间/否则为显示进度 
	int       nIconShowState;   //显示左边小方块图的状态
	int       nState;			//当前节点的状态
	BOOL	  bShowGrayPg;      //是否绘制灰化进度条
	netstate  netState;			// 网络状态
	BOOL      bShowPlayBtn;     //是否显示播放按钮
	BOOL	  bFileExist;		//文件是否存在
	BOOL	  bHalfTrans;		//是否半透
	Rect	  rtText;
	BOOL      bIsEnableSel;	 // 能否选中
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

//多媒体列表控件鼠标消息交互类型
enum MULTI_LIST_MSG_TYPE
{
	ML_MSG_LBUTTONCLICK ,		//鼠标左键单击
	ML_MSG_RBUTTONCLICK ,		//鼠标右键单击
	ML_MSG_LBUTTONDBLCLK,		//鼠标左键双击
	ML_MSG_HEADER_BT_A_CLICK ,  //鼠标左键单击列表头A按钮
	ML_MSG_HEADER_BT_B_CLICK ,  //鼠标左键单击列表头B按钮
	ML_MSG_HEADER_BT_C_CLICK ,  //鼠标左键单击列表头C按钮
	ML_MSG_EDIT_INSERT_ITEM ,	//用户通过内嵌编辑框填写添加item信息
	ML_MSG_EDIT_REMOVE_ITEM ,	//删除 item 列表 
	ML_MSG_EDIT_MODIFY_ITEM ,	//用户通过内嵌编辑框填写修改item信息
	ML_MSG_ITEM_HOVER_CHNAGE,	//用户通过鼠标移动当item选项改变的时候触发
	ML_MSG_DRAG_DROP_IN,		//用户通过鼠标从外部拖拽文件进入后释放时触发
	ML_MSG_DRAG_DROP_ITEMS,		//用户通过鼠标从外部拖拽列表项进入后释放时触发
	ML_MSG_DRAG_DROP_REORDER,	//用户通过鼠标拖拽选项触发自身重排数据
	ML_MSG_DRAG_DROP_COPY,		//用户通过鼠标拖拽选项触发复制\剪切粘贴操作
	ML_MSG_ITEM_RANGE_REPORT,	//报告当前页面显示的item范围
	ML_MSG_ITEM_IMG_CHANGE,		//请求某Item选项缩略图更换, 其中nID/nItemSel 用作存储第一个/最后一个显示item
	ML_MSG_OVERDEL_BTN_CLICK,	//鼠标左键单击
	ML_MSG_ITEM_FOLD,			//收拢或展开某项时触发
	ML_MSG_CLICK_TWICE,			//用户左键两次点击某一项
	ML_MSG_STRING_CHANGE,		//某项的文字内容发生变化
	ML_MSG_CLOUD_BTN_CLICK,		//私有云单击消息
	ML_MSG_KEY,
	ML_MSG_QVODTRANSCODE,	// 快播转码被单击
	ML_MSG_QVODTRANSCODETIPS,
	ML_MSG_REQUESTLOADSUBITEM		//请求加载子目录
};

//多媒体列表控件鼠标消息体
struct MULTI_LIST_MSG
{
	MULTI_LIST_MSG_TYPE uMSG;	//消息类型

	int nItemSel;				//当前选中的item
	int nChildItem;				//当前选中的item的子节点序号0,1,2,3,4
	int nID;					//该节点的内部ID编号
	int nChar;
	BOOL bMoserInNameRect;		//鼠标在name的显示区域中
	MUL_ITEM_INFO m_SelInfo;    //当前选中item的详细信息
};
//多媒体列表控件鼠标消息体
struct MEDIA_TABLE_MSG
{
	MULTI_LIST_MSG_TYPE uMSG;	//消息类型

	int nItemSel;				//当前选中的item
	int nChildItem;				//当前选中的item的子节点序号0,1,2,3,4
	int nID;					//该节点的内部ID编号
	MEDIA_TABLE_ITEM m_SelInfo; //当前选中item的详细信息
};


//多媒体列表控件图片相关
enum
{
	M_LIST_BK,				//背景
	M_LIST_ITEM,			//item图片
	M_LIST_ITEM_BT,			//item-方块
	M_LIST_ITEM_PG,			//item-进度条
	M_LIST_ITEM_IMG,		//item-图片
	M_LIST_HEADER_BG,		//列表头-背景
	M_LIST_HEADER_BT_A,		//列表头-按钮A
	M_LIST_HEADER_BT_B,		//列表头-按钮B
	M_LIST_ITEM_FOLD,		//Citem-展开收缩图片
	M_LIST_ITEM_SIMPLE,		//item图片-简洁模式
	M_LIST_DRAG_IMG,		//拖动时显示的图片
	M_LIST_ITEM_NEW,		//item-是否为new
	M_LIST_ITEM_TRANSCODE,	// 预览按钮上的转码
	M_LIST_ITEM_FOLD_CHILD,	//二级目录展开收缩
	M_LIST_ITEM_SMALLICON,	//item-是否为new/hot/hd/bd

	M_LIST_BMP_MAX_NUM		//多媒体存储类图片数目
};


//item左边Icon图案的状态
#define MUL_LIST_ITEM_BT_STATE_NUM      5                    //item左边Icon图案的状态数

#define MUL_LIST_ITEM_BT_STATE_NODRAW   -1                   //item左边Icon图案的状态-不绘制
#define MUL_LIST_ITEM_BT_STATE_COMPUTER 0                    //item左边Icon图案的状态-电脑
#define MUL_LIST_ITEM_BT_STATE_ARROW    1                    //item左边Icon图案的状态-箭头
#define MUL_LIST_ITEM_BT_STATE_GREEN    2                    //item左边Icon图案的状态-绿
#define MUL_LIST_ITEM_BT_STATE_YELLOW   3                    //item左边Icon图案的状态-黄
#define MUL_LIST_ITEM_BT_STATE_RED      4                    //item左边Icon图案的状态-红
enum CUS_LISTITEM_STATE
{
	CUS_LISTITEM_ST_UN     = -1,    //鼠标在列表项   未知状态
	CUS_LISTITEM_ST_NM,				//鼠标在列表项区域  NORMAL
	CUS_LISTITEM_ST_HV,				//鼠标在列表项区域  Hover
	CUS_LISTITEM_ST_SEL,			//列表项			select	
	CUS_LISTITEM_ST_DN, 			//鼠标在列表项区域  Down 
};
enum CUS_Alignment
{
	CUS_Align_left		  = 0,
	CUS_Align_center,
	CUS_Align_right,
};
enum CUSTOM_LIST_MSG_TYPE
{
	CUSTOM_MSG_LBUTTONCLICK ,		//鼠标左键单击
	CUSTOM_MSG_RBUTTONCLICK ,		//鼠标右键单击
	CUSTOM_MSG_LBUTTONDBLCLK,		//鼠标左键双击
	CUSTOM_MSG_ITEM_HOVER_CHNAGE,	//用户通过鼠标移动当item选项改变的时候触发
	CUSTOM_MSG_DRAG_DROP_IN,		//用户通过鼠标从外部拖拽文件进入后释放时触发
	CUSTOM_MSG_DRAG_DROP_REORDER,	//用户通过鼠标拖拽选项触发自身重排数据
	CUSTOM_MSG_DRAG_DROP_COPY,		//用户通过鼠标拖拽选项触发复制\剪切粘贴操作
	CUSTOM_MSG_SELECTEDITEM_CHANGE, //选中列表项组改变时触发
	CUSTOM_MSG_KEY,					//键盘消息  触发
	CUSTOM_MSG_COLUMN_DOWN,			//左键点击列表头 触发排序
	CUSTOM_MSG_COLUMN_SIZE_CHANGE,  //列表宽度改变时 触发
	CUSTOM_MSG_CLICK_TWICE,
	CUSTOM_MSG_STRING_CHANGE,
	CUSTOM_MSG_HEADER_CHECKBOX_CLICKED,	//列表头的checkbox被单击
	CUSTOM_MSG_ITEMCHECK_CHANGED,	//列表项的选取状态被更改
	CUSTOM_MSG_QVODTRANSCODE,	// 快播转码被单击
	CUSTOM_MSG_TRANSCODETIPS, //
};
struct CUSTOM_LIST_MSG
{
	CUSTOM_LIST_MSG_TYPE uMSG;	//消息类型
	int nID;					//该节点的内部ID编号
	int nColumn;				//当前处理的列  非 -1 即为有效
	int nItem;					//当前处理的item
	int nChildItem;				//当前选中的item的子节点序号0,1,2,3,4
	WORD nChar    ;
};

//////////////////////////////////////////////////////////////////////////////////////
//
// 当前版本：1.0
// 作    者：罗伟坚
// 完成日期：
// 功能说明：多媒体列表控件,每个选项item可以承载若干元素(如进度,图片等)
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinMultiListCtrl : public CSkinCtrl, public IQvodCallBack
{
public:

	//
	//外部接口功能组
	//

	//业务逻辑层操作   
	//ID为节点属性信息-文件ID号
	int  GetItem_ID(int nIndex);											//获取列表内序号为nIndex的ID编号
	int  GetItembyID(int nID, BOOL bFindChild = TRUE);			//获取节点属性nID所在列表nIndex位置, bFindChild查找子结点，否则只查找结点
	BOOL GetItemDatabyID(int nID, MUL_ITEM_INFO *pItemInfo, BOOL bFindChild = TRUE);				//获取指定节点属性nID和数据, bFindChild为FALSE，只查找主结点，否则查找所有结点
	int  GetSelectItem_ID(int nIndex = 0);									//获取当前选中item的原先节点的传入时候的ID编号(参数从0开始)
	int  GetSelectItem_ID(std::vector<int> & vtSelIdList);					//获取当前所有选中item的原先节点的传入时候的ID编号
	int  GetSelectItem_ID(std::vector<long> & vtSelIdList);					//获取当前所有选中item的原先节点的传入时候的ID编号
	void GetSeriesItem_ID(int nID, std::vector<long> &vSeriesItem);	//获取某剧集下所有项的ID
	BOOL RemoveItembyID(int nID, BOOL bChild = FALSE);											//删除一个item数据,nID为节点的nID信息,bChild为TRUE，查找子结点
	BOOL ModifyItembyID(int nID, MUL_ITEM_INFO *pItemInfo);					//修改指定item数据,nID为节点的nID信息
	void SetItemShowStatebyID(int nID, BOOL bShowTime);						//设置item的显示模式  时间/进度	--仅在简洁模式有效,nID为节点的nID信息
	void SetItemIconStatebyID(int nID, int  nState);						//设置item的左边Icon图像显示模式,nID为节点的nID信息
	void ClearAllIconState(int nExceptionID = 0);												//清空所有播放状态
	BOOL SetSelItembyID(int nID);											//设置指定item为选中状态,nID为节点的nID信息,-1为清空所有选中
	const std::vector<CString>* GetDragDropPathList(void);					//获取拖拽文件/文件夹路径的列表指针

	//访问操作
	int  GetItemCount(void);												//获取item总数
	BOOL GetItemData(int nItem, MUL_ITEM_INFO *pItemInfo, BOOL bGetChildInfo = FALSE);//获取节点指定的序号和数据(相对于m_vtItemList下的根结点)
	BOOL GetItemChildData(int nIndex, int nChild, MUL_ITEM_INFO *pItemInfo);//获取指定子节点的序号和数据  输入:  nIndex --指定item的在一级节点的序号,nChild - 子节点的序号位置; itemInfo --待回取item节点信息;
	int  GetItemState(int nIndex, BOOL bFindChild = TRUE);											//获取指定item的状态	
	int  GetSelItemCount(void);												//获取选中item数目	
	int  GetSelectItem(void);												//获取当前选中item的序号
	BOOL GetSelectItemData(int & nIndex, MUL_ITEM_INFO *pItemInfo);			//获取当前选中item的序号和数据
	int  GetHoverItem(void);												//获取当前鼠标停留(Hover)在item的序号
	void GetPreviewImgSize(int & nWdith, int & nHeight);					//获取预览图的尺寸
	int  GetPageMaxShowItemCount(void);										//获取当前窗口大小每页最多可容纳item个数
	BOOL IsFolderItem(int nIndex);											//判断某item的属性是否为文件夹可展开节点
	BOOL IsItemUnFold(int nIndex);											//判断某item的属性是否为文件夹为展开状态


	//修改操作
	int InsertItem(MUL_ITEM_INFO *pItemInfo, int nIndex = -1);				//增加一个item数据 -1为插入到最后
	BOOL InsertChildItem(MUL_ITEM_INFO *pItemInfo, int nIndex, int nChildIndex = -1); //增加一个子item数据，-1为插入到最后
	BOOL RemoveItem(int nIndex);											//删除一个item数据
	void RemoveAllItem();													//删除(清空)所有item数据
	BOOL MoveItem(std::vector<UINT>& vMoveIndex, int nPos);						//移动n项到指定的位置下
	BOOL ModifyItem(int nIndex, MUL_ITEM_INFO *pItemInfo);					//修改指定item数据
	BOOL ModifyItemChildData(int nIndex, int nChild, MUL_ITEM_INFO *pItemInfo);//修改指定子节点的序号和数据  输入:  nIndex --指定item的在一级节点的序号,nChild - 子节点的序号位置; itemInfo --待回取item节点信息;
	BOOL SetSelItem(int nIndex);											//设置指定item为选中状态
	BOOL SetSelALLItem(int nState = 1);										//设置全部选中状态
	void SetItemHeight(int nHeight);										//设置item的高度
	void Sort(int nByKind);													//排序-根据指定方式
	typedef  bool (Fun_Sort)(MUL_ITEM_INFO* pItem1, MUL_ITEM_INFO* pItem2);	//	typedef  bool (*pFun_Sort)(MUL_ITEM_INFO* pItem1, MUL_ITEM_INFO* pItem2);
	void Sort(Fun_Sort *pSortFun);											//排序-根据传入的函数指针提供给用户自定义


	//显示模式操作
	void UpdateSelCount(void);
	void SetShowMode(BOOL bSimpleMode);										//设置控件的显示模式  多媒体模式/简洁模式
	BOOL GetShowMode(void);													//获取控件的显示模式  多媒体模式/简洁模式
	void SetItemShowState(int nItem, BOOL bShowTime);						//设置item的显示模式  时间/进度	--仅在简洁模式有效
	BOOL GetItemShowState(int nItem);										//获取item的显示模式			--仅在简洁模式有效
	void SetItemIconState(int nItem, int  nState);							//设置item的左边Icon图像显示模式
	int  GetItemIconState(int nItem);										//获取item的左边Icon图像显示模式
	void SetRightIconShowByID(int nID, BOOL bShow);							//设置item的右边icon是否显示
	void SetItemPgShowState(int nItem, BOOL bShowGrayPg);					//设置item的进度条是否显示灰化状态
	BOOL GetItemPgShowState(int nItem);										//获取item的进度条是否显示灰化状态 
	void SetItemShortCutImg(int nItem, Image * pImgBmp);					//设置指定item的缩略图	
	void SetReportItemFlag(BOOL bOpen);										//设置开关状态 - 报告当前页面显示item的项
	BOOL GetReportItemFlag(void);											//获取开关状态 - 报告当前页面显示item的项
	void SetRedraw(BOOL bDraw);												//设置重绘开关
	BOOL GetRedraw(void);													//获取重绘开关
	void SetFoldStyle(BOOL bFoldStyle);										//设置item是否为可展开收拢样式
	BOOL GetFoldStyle(void);												//获取item是否为可展开收拢样式
	void SetOnlyShowName(BOOL bOnlyShowName);										//设置只显示名称的模式
	void SetCanDrag(BOOL bCanDrag = TRUE);				//设置是否支持拖动
	// 描述 ：MUL_ITEM_INFO操作
	// 作者 ：匡晓林
	// 时间 ：2012-09-06
private:
	// 描  述：MUL_ITEM_INFO 拷贝函数
	//	       bCopyChild 是否拷贝子节点 vtChildItemList
	// 返回值：返回pDes
	MUL_ITEM_INFO * Copy(MUL_ITEM_INFO *pDes ,MUL_ITEM_INFO *pSrc , BOOL bCopyChild = FALSE); 
	//
	//内部实现功能组
	//
public:
	CSkinMultiListCtrl();
	virtual ~CSkinMultiListCtrl();
	virtual BOOL OnKey(WORD nChar);											//键盘按下

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();													//反初始化
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//消息回调--对子控件创建时候传父类obj,让它实现的时候能逐层回溯
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//函数接口--类似COM查询接口,
	virtual BOOL OnTimer();													//自动调用定时器--查看是否需要重绘

	virtual BOOL OnMouseMoveIn(short x, short y);							//鼠标处理消息-移进控件
	virtual BOOL OnMouseMoveOut();											//鼠标处理消息-移出控件
	virtual BOOL OnMouseDown(short x, short y);								//鼠标处理消息-按下
	virtual BOOL OnMouseUp(short x, short y);								//鼠标处理消息-释放
	virtual BOOL OnMouseDrag(short x, short y);								//鼠标处理消息-拖动
	virtual BOOL OnMouseMove(short x, short y);								//鼠标处理消息-移动
	virtual BOOL OnRButtonUp(short x, short y);								//鼠标处理消息-右键释放
	virtual BOOL OnRButtonDown(short x, short y);							//鼠标处理消息-右键按下
	virtual BOOL OnDblClick(short x, short y);								//鼠标处理消息-左键双击
	virtual BOOL OnWheel(short nStep);										//鼠标处理消息-滚轮滚动
	virtual BOOL Layout(short nWidth, short nHeight);						//陈列展现控件--该函数由改变窗口大小位置的时候出发WM_SIZE

	BOOL OnMouseDown_FoldStyle(short x, short y);							//鼠标处理消息-按下
	BOOL OnMouseUp_FoldStyle(short x, short y);								//鼠标处理消息-释放
	BOOL OnMouseDrag_FoldStyle(short x, short y);							//鼠标处理消息-拖动
	BOOL OnMouseMove_FoldStyle(short x, short y);							//鼠标处理消息-移动
	BOOL OnRButtonUp_FoldStyle(short x, short y);							//鼠标处理消息-右键释放
	BOOL OnDblClick_FoldStyle(short x, short y);							//鼠标处理消息-左键双击

	void LButtonClickItem_Work_FoldStyle(int nClickItem);					//鼠标点击某一item触发需要做的业务工作
	void ReOrderItemByDragDrop_FoldStyle(void);								//自身拖拽item后重排item的位置
	void Paint(Graphics *pGraph);											//绘制控件
	void Paint_SimpleMode(Graphics *pGraph);								//绘制控件 精简模式
	void Paint_MultiMode(Graphics *pGraph);									//绘制控件 多媒体模式

	void DrawItem(Graphics *pGraph, int nItem);								//绘制某一个item
	void DrawItem_SimpleMode(Graphics *pGraph, int nVirtualOrder, int nItem);//绘制某一个item
	void DrawItem_FlatStyle(Graphics *pGraph, const int nItem);					//绘制某一个item
	void DrawItem_WaveStyle(Graphics *pGraph, int nItem);					//绘制某一个item
	void DrawItem_FrameStyle(Graphics *pGraph, int nItem);					//绘制某一个item
	void DrawDragBmp(HBITMAP & hBmp);										//绘制拖动item
	void DrawItem_FlatStyle_byDrag(Graphics *pGraph, int nItem, int nOrder);//绘制某一个item  _byDrag
	void DrawItem_SimpleMode_byDrag(Graphics *pGraph,  
		                            int nItem, int nOrder);					//绘制某一个item _byDrag
	void Draw_Progress(Graphics *pGraph, int nItem, Rect & pgRect);			//绘制某一个item进度条
	void DrawChildItem_SimpleMode(Graphics *pGraph, int nVirtualOrder, 
								   int nItem, int nChild);					//绘制某一个item的子节点

	void GetItemRect_byDrag(int nOrder, Rect & rtItem);						//获取item的区域_byDrag
	void OnCtrlScroll(long lPos);											//滚动条处理函数
	void RefreshScrollCtrlPos();											//设置滚动条的滑块位置
	void VisualizeScrollCtrl();												//设置滚动条的可视状态
	void RefreshAllItem_VitualPos(void);									//刷新当前结构下所有节点,子节点对应的显示虚拟位置
	BOOL CheckVerScroll(const short x, const short y);						//检测坐标是否在Scroll内
	BOOL CheckSpliter(const short x, const short y);						//检测坐标是否在Spliter内
	BOOL CheckCursor(short x, short y);										//检测坐标是否在控件内
	BOOL IsPointInRect(Rect rt, int x, int y);								//检测坐标是否在矩形内
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//从XML文件载入皮肤元素的配置
	void LButtonClickItem_Work(int nClickItem);								//鼠标点击某一item触发需要做的业务工作
	void DoDragDrop(int xDragImgMove, int yDragImgMove);					//拖动绘制处理--x,y为拖动图像在鼠标的位移
	void ReOrderItemByDragDrop(void);										//自身拖拽item后重排item的位置
	BOOL IsPointInTextArea(short x, short y);								//判断坐标是否在文字区域内

	BOOL ISPointInFoldBtn(int x, int y, int nItem);							//判断点是否在展开按钮上
	BOOL ISPointInFolderItem(int x, int y);									//判断点是否在可展开item上
	void FoldItemByIndex(BOOL bFold, UINT uIndex);							//折叠拓展某一项item
	void FoldItemBynID(BOOL bFold, INT ItemnID);							//折叠拓展某一项item
	BOOL IsFoldItem(int nItem);												//判断某项是否为可展开项
	int  GetSelectChildItem(int & nItem, int & nChildItem);					//获取选中item和第一个选中子item项
	int  GetShowItemCount(void);											//获取当前显示的item数
	BOOL GetItemFromVirtualPos(UINT nVirtualPos, 
							   int &nItem, int &nChildItem);				//根据虚拟显示位置获取相应的节点,子节点值
	int GetVirtualPosFromItem(UINT nItem, INT nChildItem);					//根据节点,子节点值获取相应的虚拟显示位置,其中nChildItem-1为父节点,大于0为子节点


	BOOL InsertSeriesChildItem(long nCtrlId, MUL_ITEM_INFO& mi, int nInsertIdx);
	BOOL  m_bFoldStyle;														//item是否为可展开收拢样式
	int   m_nChildItemInWidth;												//子节点绘制时候缩进宽度
	void sort_by_ID(void);													//排序--根据ID
	void sort_by_Progress(void);											//排序--根据进度
	void sort_by_NetQuality(void);											//排序--网络质量
	void sort_by_Time(void);												//排序--根据播放时长

	void ReportPageItemShow(int nLower, int nHigher);						//报告当前页面显示item的项

	// 选中子节点时更新父节点状态。nFatherIndex，子节点所属父节点索引
	void UpdateFatherState(UINT nFatherIndex);

	// 选中父节点时更新子节点状态。nFatherIndex，子节点所属父节点索引
	void UpdateChildState(UINT nFatherIndex);

	// 选中若干父节点。nSFatherIndex，起始索引，nEFatherIndex，结束索引
	void SelectFatherPeers(UINT nSFatherIndex, UINT nEFatherIndex);

	// 选中若干子节点。nSChildIndex，起始索引，nEChildIndex，结束索引
	void SelectChildPeers(UINT nSChildIndex, UINT nEChildIndex, UINT nFatherIndex);

	// 获取可见的元素的起始和终止索引。nFirstIndex，起始索引，nLastIndex，结束索引
	void GetShowItemIndex(int &nFirstIndex, int &nLastIndex);

	// 获取某个y坐标处的Item索引
	int GetItemIndexFromYpos( int nY );

	// 获取item的矩形显示区域
	void GetItemRect(int nItem, Rect & rtItem);

	// 调整滚动条的位置，使正在播放的列表项可见
	void ScrollToPlayingItemPos(int nID);
	void ScrollToIndex(int index);
	void ScrollSelect(int nHover = -1, int y = -1);
	void VirtualRangeSelect(int start, int end);
	// 获取 拖拽到列表中时鼠标Hover在的item
	int GetDragHoverItem() ;
	//	获取item高度
	int GetItemHeight();

	// 创建编辑控件
	CSkinEdit* CreateEdit();

	BOOL EditItemVirtualIndex(int nIndex);

	BOOL EditItem(int nItem, int nChildItem = -1);

	BOOL OverEdit(BOOL bModify);

	long OnDrag(UINT uMsg, long wparam, long lparam);
	Rect GetContentRect(void);
	BOOL DragMoveList( short x, short y );
	static VOID CALLBACK DragScroll_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	//
	//成员属性
	//

	// DragDrop相关
	virtual BOOL DragMouseIn(short x, short y);
	virtual BOOL DragMouseOut();
	virtual BOOL DragMouseMove(short x, short y);
	virtual BOOL DragMouseUp(short x, short y);
private:
	void PaintCloudBtn(Graphics *pGh, int nItem, Rect & pgRect, int nChildItem = -1);

private:

	std::vector<MUL_ITEM_INFO*> m_vtItemList;						//跟显示对象关联的数据链

	MULTI_LIST_MSG  m_mouseMSG;										//用于鼠标向父类发触发消息,包含多种信息

	CXMLElement		m_xmlItemStyle;									//Item 的模板
	CSkinScroll*	m_pScroll;										//滚动条子控件		
	CSkinButton*	m_pODBtn;										//浮动删除按钮
	CSkinEdit*		m_pEdit;										//编辑框控件
	Bitmap*			m_pBmpODBtn;									//浮动按钮图片

	CRITICAL_SECTION m_rsLock;										//资源锁-用于防止多线程中删除操作与绘制过程的冲突

	std::vector<CString> m_vtDragList;								//拖拽文件/文件夹名称的数据链
	ui::drop::CDragDropCtrl		 m_DragDropCtrl;							//拖放控件
	HBITMAP				 m_hDragBitmap;								//用以保存拖动的位图
	BOOL				 m_bShouldDrag;								//是否应该是拖拽时机
	BOOL				 m_bCanItemDrag;							//是否支持列表项拖动
	int					 m_ndragHoverItem;							//拖拽到列表中时鼠标Hover在的item
	BOOL                 m_bDragAddNetTask ;
	int					 m_ndragHoverChildItem;						//拖拽到列表中时鼠标Hover在的item的子item位置
	int					 m_nVirtualDragHoverItem;					//拖拽到列表中时鼠标Hover在的虚拟item位置

	long m_nTotalHeight;											//Item的总高
	BOOL m_bShowScroll;												//是否需要显示Scroll
	BOOL m_bInscroll;												//鼠标是否在scroll上
	BOOL m_bInOverDelBtn;											//鼠标是否在浮动删除按钮上
	long m_lPageViewPos;											//当前显示页面相对总顶部的偏移(Scroll的偏移量)
	long m_nScrollWidth;											//Scroll的宽度
	BOOL m_bScrolldown;												//Scroll down
	BOOL m_bOverDelBtndown;											//浮动删除按钮是否按下
	BOOL m_bDrag;													//鼠标在拖动时为 TRUE
	BOOL m_bRButtonDown;											//鼠标右键是否为按下状态
	BOOL m_bLBDbClickDown;											//鼠标双击是否为按下状态
	BOOL m_nItemRBUp;												//记录右键弹起的列表元素
	BOOL m_bODBtnCmdInvalid;										//记录右键弹起的列表元素
	BOOL m_bOnlyShowName;											//只显示时间的模式

	int	 m_iNameFontSize;											//字体大小 -名称
	int	 m_iTimeFontSize;											//字体大小 -时间

	long	 m_nLastSeriesIconStatusId;							//最近一次剧集的状态

	Rect m_btRect;													//方块区域
	Rect m_btSimpleRect;											//方块区域-简洁
	Rect m_textNameRect;											//名称文本区域
	Rect m_textNameSimpleRect;										//名称文本区域-简洁
	Rect m_textTimeRect;											//时间文本区域
	Rect m_textTimeSimpleRect;										//时间文本区域-简洁
	Rect m_pgRect;													//进度条区域
	Rect m_pgSimpleRect;											//进度条区域-简洁
	Rect m_imgRect;													//图片区域
	Rect m_imgHVRect;												//图片区域hover状态

	Rect m_rtDragFrame;												//鼠标拖选时候绘制的框
	CPoint m_ptDragStart;										    //记录鼠标拖选时候绘制的按下起点
	CPoint m_ptDragEnd;										        //记录鼠标拖选时候绘制的到达终点
	CPoint m_ptDragStartOld;										//记录鼠标拖选时候绘制的按下起点


	Color m_clrItemBK_NL;											//背景颜色Normal状态
	Color m_clrItemBK_HV;											//背景颜色Hover状态
	Color m_clrItemBK_DN;											//背景颜色Select(down)状态

	Color m_clrNameText_NL;											//背景颜色Normal状态
	Color m_clrNameText_HV;											//背景颜色Hover状态
	Color m_clrTimeText_NL;											//背景颜色Normal状态
	Color m_clrTimeText_HV;											//背景颜色Hover状态

	Color m_clrLeftBorder;											//左边边框颜色
	Color m_clrDragFrame;											//拖拽框颜色
	Color m_clrDragFrameBorder;										//拖拽框边的颜色

	Bitmap * m_aImg[M_LIST_BMP_MAX_NUM];							//存储各个状态的绘制位图
	Bitmap * m_pRightIcon[MAX_ICON_COUNT];							//右边图标				//item右边的icon
	CString m_textFontStyle;										//字体样式
	CString m_textTimeFontStyle;									//时间字体样式

	int   m_nItemHeight;											//item的高度-当前模式
	int	  m_nItemSelectHeight;										//选中item的高度(单选)
	int   m_nItemSimpleHeight;										//item的高度-简洁模式
	int   m_nItemMultiHeight;										//item的高度-多媒体模式


	int   m_nItemCount;												//item总数
	int   m_nVirtualItemCount;										//虚拟item总数
	int   m_nItemHover;												//当前鼠标悬挂在哪一个item
	int   m_nItemDown;												//当前鼠标按下在哪一个item
	int   m_nItemSel;												//当前选中哪一个item
	int   m_nItemSelCount;											//当前选中item的数目
	int   m_nTopViewVirtualItem;									//滚动页中,最顶部的虚拟Item项(含子Item)
	int   m_nTopViewItem;											//滚动页中,最顶部的Item
	int   m_nTopViewChildItem;										//滚动页中,如果最顶部的显示的是子item项,则记录在此,结合m_nTopViewItem定位
	int   m_nItemDragFrameDown;										//框选时按下按钮所在的选项
	int   m_nItemDragFrameDownTopVVI;								//框选时按下按钮所在的选项

	int	  m_nEditItem;												//编辑框所在的项
	int   m_nEditChildItem;											//编辑框所在的子项
	BOOL  m_bEditShow;												//编辑框是否在显示
	BOOL  m_bClickTwice;											//是否两次点击了同一个项


	Rect  m_HeaderRect;												//列表头区域
	Rect  m_ListRect;												//列表区域

	int   m_nItemDrawStyle;											//item绘制样式
	BOOL  m_bHaveHeader;											//是否有列表头
	int   m_nHeaderHeight;											//列表头的高度

	BOOL  m_bShowSimpleMode;										//控件的模式是否为简洁模式 否则为多媒体模式
	BOOL  m_bShowTimeState;											//item显示模式是否为时间 否则为 进度--仅在简洁模式有效

	BOOL  m_bShowDragFrame;											//是否在鼠标拖拽时显示框
	BOOL m_bDragWay_byArea;											//是否框选触发由文字区域外点击而触发-否则为当前是否已经选中
	BOOL m_bOpenReportItemFlag;										//是否开启报告当前页面显示item的项功能
	int m_nOldShowLower;											//当前显示最顶项-用作报告状态
	int m_nOldShowHigher;											//当前显示最后项-用作报告状态
	int m_nOldShowLowerID;											//当前显示最顶项文件ID-用作报告状态
	int m_nOldShowHigherID;											//当前显示最后项文件ID-用作报告状态
	BOOL m_bRedraw;													//图像更新时是否绘制控件
	BOOL m_bShowProgressByText;										//是否用文字来表达进度
	BOOL m_bSetScrollRect;											//是否设置滚动条的RECT

	BOOL m_bAlwayHideODBtn;											// 一直隐藏删除按钮
	static CSkinMultiListCtrl * m_pMulListCtrl;
	int  m_nDragScrollDown;                                        // 垂直滚动方向 0 向上 1 向下
	int m_preTimer;
	BOOL m_bSetTimer;
	DWORD m_dwScrlTimerID;
	BOOL		m_bCanDrag;								//是否支持拖

	Bitmap*			m_pBmpCloudBtn;									//私有云图片
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

	int  nLevel;		//级别
	int  nChild;		//子节点个数
	BOOL bFolder;		//是否展开，TRUE为展开
	int  nUpInforWidth; //附加信息长度
	ONLINEMEDIA_ITEM_INFO *pParent ;//父节点指针
	BOOL bRequest;		//是否发起了子节点请求
	BOOL bLoadding;		//显示了正在加载中
	BOOL bLight;		//前面高亮显示追剧
	BOOL bSave;			//节点是否需要保存

	int  num ;
	int  tips ;			
	BOOL chase ;		//追剧属性
	int	 pos ;

	LONGLONG sc;

	BOOL	hot;
	BOOL	newly;
	BOOL	hd;
	BOOL	bd;

	//classid 
	//0 无意义 
	//1 电影
	//2 电视剧
	//3 综艺
	//4 动漫
	TCHAR	*classid;
	TCHAR	*docid;
	TCHAR	*title;
	TCHAR	*playlink;
	TCHAR	*source;
	TCHAR	*upinfo;
	TCHAR	*cover;
	TCHAR	*more;
	TCHAR	*language;

	//文字样式
	DWORD	color;
	BOOL	bold;

	vector<TCHAR*> director;
	vector<TCHAR*> actor;

	Rect   rect;	//显示的区域
	long   nIndex;	//在列表中的索引
};


const TCHAR kMovieClassID[] = _T("1");
const TCHAR kTVClassID[] =    _T("2");
const TCHAR kVarietyClassID[] = _T("3");
const TCHAR kCartoonClassID[] = _T("4");

//多媒体列表控件鼠标消息体
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

	MULTI_LIST_MSG_TYPE uMSG;			//消息类型

	int					nItemSel;		//当前选中的item
	CPoint				ptItem ;		//item left top
	
	ONLINEMEDIA_ITEM_INFO *pSelInfo;    //当前选中item的详细信息
};

enum onlinemediaparamer
{
	bydocid,
	byplaylink
};

//////////////////////////////////////////////////////////////////////////////////////
//
// 当前版本：1.0
// 作    者：陈志标
// 完成日期：
// 功能说明：在线媒体列表控件
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinOnlineMediaListCtrl : public CSkinCtrl, public IQvodCallBack
{
public:

	//
	//外部接口功能组
	//

	//访问操作
	int  GetItemCount(void);												//获取item总数
	BOOL GetSelectItem(ONLINEMEDIA_ITEM_INFO &info);						//获取当前选中item
	int  GetItemHeight(void);												//获取item的高度
	BOOL StopPlayMedia();
	BOOL GetSubItemList(vector<ONLINEMEDIA_ITEM_INFO *> &pItemlist, ONLINEMEDIA_ITEM_INFO *pParent, int nItemSel);
	void SetListDataIndex(int nIndex) ;
	void SetPlayItem(ONLINEMEDIA_ITEM_INFO *pItem, BOOL bScroll=FALSE) ;
	ONLINEMEDIA_ITEM_INFO *SetPlayItemBy_(TCHAR *docid, int byParamer) ;
	BOOL PlayNextItem() ;
	TCHAR *GetPlayItemDocID() ;
	void GetChildList(ONLINEMEDIA_ITEM_INFO *pParent, vector<ONLINEMEDIA_ITEM_INFO *> &vtChild) ;

	//修改操作
	BOOL InsertItem(vector<ONLINEMEDIA_ITEM_INFO *>pItemlist, ONLINEMEDIA_ITEM_INFO *pParent=NULL, int nIndex = -1);
	BOOL InsertItem(ONLINEMEDIA_ITEM_INFO *pItemInfo, int nIndex = -1);		//增加一个item数据 -1为插入到最后
	int  RemoveItem(ONLINEMEDIA_ITEM_INFO *pItemInfo, BOOL bDeleteFather = TRUE);											//删除一个item数据
	void RemoveAllItem();													//删除(清空)所有item数据
	BOOL ModifyItem(int nIndex, ONLINEMEDIA_ITEM_INFO *pItemInfo);			//修改指定item数据
	BOOL ModifyItembyReName(int nIndex, CString strNewName);				//修改item-重命名, -1为选中的item,空字符为原item文本
	BOOL SetSelItem(int nIndex);											//设置指定item为选中状态
	void SetItemHeight(int nHeight);										//设置item的高度
	void Sort(int nByKind);													//排序-根据指定方式


	//
	//内部实现功能组
	//
public:
	CSkinOnlineMediaListCtrl();
	virtual ~CSkinOnlineMediaListCtrl();

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();													//反初始化
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//消息回调--对子控件创建时候传父类obj,让它实现的时候能逐层回溯
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//函数接口--类似COM查询接口,
	virtual BOOL OnTimer();													//自动调用定时器--查看是否需要重绘

	BOOL LoadItemsFromFile(CString strPath);
	BOOL SaveItemsToFile(CString strPath);
	BOOL SetVersion(int nVersion);
	LONGLONG  GetVersion(CString strPath);

	virtual BOOL OnKey(WORD nChar);											//键盘按下
	virtual BOOL OnMouseMoveIn(short x, short y);							//鼠标处理消息-移进控件
	virtual BOOL OnMouseMoveOut();											//鼠标处理消息-移出控件
	virtual BOOL OnMouseDown(short x, short y);								//鼠标处理消息-按下
	virtual BOOL OnMouseUp(short x, short y);								//鼠标处理消息-释放
	virtual BOOL OnMouseDrag(short x, short y);								//鼠标处理消息-拖动
	virtual BOOL OnMouseMove(short x, short y);								//鼠标处理消息-移动
	virtual BOOL OnRButtonUp(short x, short y);								//鼠标处理消息-右键释放
	virtual BOOL OnRButtonDown(short x, short y);							//鼠标处理消息-右键按下
	virtual BOOL OnDblClick(short x, short y);								//鼠标处理消息-左键双击
	virtual BOOL OnWheel(short nStep);										//鼠标处理消息-滚轮滚动
	virtual BOOL Layout(short nWidth, short nHeight);						//陈列展现控件--该函数由改变窗口大小位置的时候出发WM_SIZE
	virtual void SetState(skin_state state);								//设置控件状态

	void Paint(Graphics *pGraph);											//绘制控件
	BOOL DrawItem(Graphics *pGraph, ONLINEMEDIA_ITEM_INFO *pInfo, Rect &itemRect); //绘制某一个item
	void GetItemRect(int nItem, Rect & rtItem);								//获取item的区域
	void OnCtrlScroll(long lPos);											//滚动条处理函数
	void RefreshScrollCtrl();												//重新设置scroll控件参数
	void RefreshCtrlLayOut(void);											//刷新控件排版
	ISkin* GetSkinObject(const short x, const short y);						//检测坐标里面的控件
	BOOL CheckVerScroll(const short x, const short y);						//检测坐标是否在Scroll内
	BOOL CheckSpliter(const short x, const short y);						//检测坐标是否在Spliter内
	BOOL CheckCursor(short x, short y);										//检测坐标是否在控件内
	BOOL IsPointInRect(Rect rt, int x, int y);								//检测坐标是否在矩形内
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//从XML文件载入皮肤元素的配置
	void ModifyItemWork(void);												//检测和修改item的工作
	BOOL GetItemFromYPos(int nY, int &nItem);
	BOOL IsItemValid(int nItem);
	int  GetShowItemCounts();
	void RelayoutMoreCtrl();

	void sort_by_name(void);												//排序--根据名字

protected:
	BOOL LoadTaskNode(CXMLElement& xmlTask) ;
	void SaveTaskNode(CXMLDoc& xmlDoc, CXMLElement& xmlTasks, ONLINEMEDIA_ITEM_INFO *pTask);

	//
	//成员属性
	//
private:
	ONLINEMEDIA_LIST_MSG  m_mouseMSG;								//用于鼠标向父类发触发消息,包含多种信息

	CRITICAL_SECTION m_rsLock;										//资源锁-用于防止多线程中删除操作与绘制过程的冲突
	CSkinScroll*	m_pScroll;										//滚动条子控件
	CSkinButton*	m_pODBtn;										//浮动删除按钮
	CSkinCtrl*		m_pMoreCtrl;									//更多按钮
	Bitmap*			m_pBmpODBtn;									//浮动按钮图片

	enum
	{
		object_hover,	//当前鼠标悬挂在哪一个item
		object_lbdown,//鼠标交互状态
		object_lbup,
		object_rbdown,
		object_rbup,

		object_active_count
	};
	ISkin	*m_pSkinObjectActive[object_active_count] ;				//鼠标活动控件
	
	long m_nTotalHeight;											//Item的总高
	long m_nMaxShowItems;											//当前列表最大可以显示的Item数
	BOOL m_bShowScroll;												//是否需要显示Scroll
	long m_lPageViewPos;											//当前显示页面相对总顶部的偏移(Scroll的偏移量)
	long m_nScrollWidth;											//Scroll的宽度
	
	BOOL m_bDrag;													//鼠标在拖动时为 TRUE

	CString m_strFontStyle;											//字体样式
	int	 m_iNameFontSize;											//字体大小 -名称
	int	 m_iDescFontSize;											//字体大小 -描述

	Rect m_itemBtRect;												//item方块区域
	Rect m_itemTextRect;											//item文本区域


	Color m_clrItemBK_NL;											//背景颜色Normal状态
	Color m_clrItemBK_HV;											//背景颜色Hover状态
	Color m_clrItemBK_DN;											//背景颜色Select(down)状态

	Color m_clrItemText_NL;											//背景颜色Normal状态
	Color m_clrItemText_HV;											//背景颜色Hover状态	

	Bitmap* m_aImg[M_LIST_BMP_MAX_NUM];								//存储各个状态的绘制位图

	int  m_nItemHeight;												//item的高度
	int  m_nItemCount;												//item总数
	int  m_nItemShowCount;											//显示的Item总数

	enum
	{
		item_hover,	//当前鼠标悬挂在哪一个item
		item_sel,	//当前选中哪一个item
		item_play,	//当前播放的是哪个Item

		item_lbdown,//鼠标交互状态
		item_lbup,
		item_rbdown,
		item_rbup,

		item_active_count
	};
	
	ONLINEMEDIA_ITEM_INFO* m_ItemsActive[item_active_count]	;		// 活动的Item

	vector<ONLINEMEDIA_ITEM_INFO*> m_vtItemsShow ;					//当前显示的Item，在Pain的时候加入
	BOOL						   m_bDirtyItem ;					//为True，需要更新列表
	
	int		m_nTopViewItem;											// 滚动页中,最顶部的Item   
	Rect	m_ListRect;												// 列表区域
	int		m_nLevelDis;											// 级别偏移距离
	LONGLONG m_nOldVersion;										// 本地保存的列表版本号，可能以后可能用得着
	int		m_elistindex ;
	int		m_nItemHover ;

};
//////////////////////////////////////////////////////////////////////////

//可编辑列表item结构
struct EDIT_LIST_ITEM_INFO
{
	CBitmap * pBtBmp;	//方块区域图像
	CString strName;	//显示文本-名称
	CString strDesc;	//显示文本-补充说明
};
//////////////////////////////////////////////////////////////////////////////////////
//
// 当前版本：1.0
// 作    者：罗伟坚
// 完成日期：
// 功能说明：可在编辑框添加元素的列表控件
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinEditListCtrl : public CSkinCtrl, public IQvodCallBack	
{
public:

	//
	//外部接口功能组
	//

	//访问操作
	int  GetItemCount(void);												//获取item总数
	BOOL GetItemData(int nIndex, EDIT_LIST_ITEM_INFO *pItemInfo);			//获取指定item的序号和数据
	int  GetSelectItem(void);												//获取当前选中item的序号
	BOOL GetSelectItemData(int & nIndex,EDIT_LIST_ITEM_INFO *pItemInfo);	//获取当前选中item的序号和数据
	int  GetItemHeight(void);												//获取item的高度
	BOOL CanShowFullText(int nIndex);										//item的文字是否能完整显示

	//修改操作
	BOOL InsertItem(EDIT_LIST_ITEM_INFO *pItemInfo, int nIndex = -1);		//增加一个item数据 -1为插入到最后
	BOOL RemoveItem(int nIndex);											//删除一个item数据
	void RemoveAllItem();													//删除(清空)所有item数据
	BOOL ModifyItem(int nIndex, EDIT_LIST_ITEM_INFO *pItemInfo);			//修改指定item数据
	BOOL ModifyItembyReName(int nIndex, CString strNewName);				//修改item-重命名, -1为选中的item,空字符为原item文本
	BOOL SetSelItem(int nIndex);											//设置指定item为选中状态
	void SetItemHeight(int nHeight);										//设置item的高度
	void Sort(int nByKind);													//排序-根据指定方式
	void HidetheInnerEdit(void);											//主动调用隐藏内部edit框



	//
	//内部实现功能组
	//
public:
	CSkinEditListCtrl();
	virtual ~CSkinEditListCtrl();


	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();													//反初始化
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//消息回调--对子控件创建时候传父类obj,让它实现的时候能逐层回溯
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//函数接口--类似COM查询接口,
	virtual BOOL OnTimer();													//自动调用定时器--查看是否需要重绘

	virtual BOOL OnKey(WORD nChar);											//键盘按下
	virtual BOOL OnMouseMoveIn(short x, short y);							//鼠标处理消息-移进控件
	virtual BOOL OnMouseMoveOut();											//鼠标处理消息-移出控件
	virtual BOOL OnMouseDown(short x, short y);								//鼠标处理消息-按下
	virtual BOOL OnMouseUp(short x, short y);								//鼠标处理消息-释放
	virtual BOOL OnMouseDrag(short x, short y);								//鼠标处理消息-拖动
	virtual BOOL OnMouseMove(short x, short y);								//鼠标处理消息-移动
	virtual BOOL OnRButtonUp(short x, short y);								//鼠标处理消息-右键释放
	virtual BOOL OnRButtonDown(short x, short y);							//鼠标处理消息-右键按下
	virtual BOOL OnDblClick(short x, short y);								//鼠标处理消息-左键双击
	virtual BOOL OnWheel(short nStep);										//鼠标处理消息-滚轮滚动
	virtual BOOL Layout(short nWidth, short nHeight);						//陈列展现控件--该函数由改变窗口大小位置的时候出发WM_SIZE
	virtual void SetState(skin_state state);								//设置控件状态


	void Paint(Graphics *pGraph);											//绘制控件
	void DrawItem(Graphics *pGraph, int nItem);								//绘制某一个item
	void DrawListHeader(Graphics *pGraph);									//绘制列表头
	void GetItemRect(int nItem, Rect & rtItem);								//获取item的区域
	void OnCtrlScroll(long lPos);											//滚动条处理函数
	void RefreshScrollCtrl();												//重新设置scroll控件参数
	void RefreshCtrlLayOut(void);											//刷新控件排版
	BOOL CheckVerScroll(const short x, const short y);						//检测坐标是否在Scroll内
	BOOL CheckSpliter(const short x, const short y);						//检测坐标是否在Spliter内
	BOOL CheckCursor(short x, short y);										//检测坐标是否在控件内
	BOOL IsPointInRect(Rect rt, int x, int y);								//检测坐标是否在矩形内
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//从XML文件载入皮肤元素的配置
	void ModifyItemWork(void);												//检测和修改item的工作


	void sort_by_name(void);												//排序--根据名字


	//
	//成员属性
	//
private:

	std::vector<EDIT_LIST_ITEM_INFO*> m_vtItemList;					//跟显示对象关联的数据链

	MULTI_LIST_MSG  m_mouseMSG;										//用于鼠标向父类发触发消息,包含多种信息

	CRITICAL_SECTION m_rsLock;										//资源锁-用于防止多线程中删除操作与绘制过程的冲突
	CXMLElement		m_xmlItemStyle;									//Item 的模板
	CSkinScroll*	m_pScroll;										//滚动条子控件
	CSkinEdit*		m_pEditCtrl;									//编辑框子控件	
	CSkinEdit*		m_pEditCtrlReName;								//编辑框子控件-重命名	

	long m_nTotalHeight;											//Item的总高
	BOOL m_bShowScroll;												//是否需要显示Scroll
	BOOL m_bInscroll;												//鼠标是否在scroll上
	long m_lPageViewPos;											//当前显示页面相对总顶部的偏移(Scroll的偏移量)
	long m_nScrollWidth;											//Scroll的宽度
	BOOL m_bScrolldown;												//Scroll down
	BOOL m_bDrag;													//鼠标在拖动时为 TRUE

	CString m_strFontStyle;											//字体样式
	int	 m_iNameFontSize;											//字体大小 -名称
	int	 m_iDescFontSize;											//字体大小 -描述
	int	 m_nHeaderTextFontSize;										//列表头字体大小

	Rect m_itemBtRect;												//item方块区域
	Rect m_itemTextRect;											//item文本区域


	Color m_clrItemBK_NL;											//背景颜色Normal状态
	Color m_clrItemBK_HV;											//背景颜色Hover状态
	Color m_clrItemBK_DN;											//背景颜色Select(down)状态

	Color m_clrItemText_NL;											//背景颜色Normal状态
	Color m_clrItemText_HV;											//背景颜色Hover状态


	Color m_clrHeaderText_NL;										//列表头文字Normal状态颜色
	Color m_clrHeaderText_HV;										//列表头文字Hover状态颜色	

	Bitmap* m_aImg[M_LIST_BMP_MAX_NUM];								//存储各个状态的绘制位图

	int  m_nItemHeight;												//item的高度
	int  m_nItemCount;												//item总数
	int  m_nItemHover;												//当前鼠标悬挂在哪一个item
	int  m_nItemSel;												//当前选中哪一个item
	int  m_nTopViewItem;											//滚动页中,最顶部的Item   
	BOOL m_bShowEditCtrl;											//是否显示编辑控件
	BOOL m_bShowEditCtrlRename;										//是否显示编辑控件-重命名
	int  m_nItemEditing;											//记录正在编辑重命名的项
	int  m_nLBDownItem;												// 鼠标左键按下的item
	int  m_nLBUpItem;												// 鼠标左键弹起的item
	int  m_nRBDownItem;												// 鼠标右键按下的item
	int  m_nRBUpItem;												// 鼠标右键弹起的item

	Rect m_HeaderRect;												//列表头区域
	Rect m_ListRect;												//列表区域

	BOOL m_bHaveHeader;												//是否有列表头
	BOOL m_bHeaderShowTop;											//列表头是否显示在顶部/底部
	int  m_nHeaderHeight;											//列表头的高度
	Rect m_HeaderBtRectA;											//列表头按钮区域--A
	Rect m_HeaderBtRectB;											//列表头按钮区域--小图标区域
	Rect m_HeaderBtRectC;											//列表头按钮区域--文字显示区域
	Rect m_HeaderEditRect;											//列表头编辑框区域
	BOOL m_bShowHeaderView;											//记录当前是否显示头部/虚拟项

	enum MUL_HEADER_STATE
	{
		HEADER_ST_OUT     = 0,    //鼠标在列表头外部
		HEADER_ST_IN,			  //鼠标在列表头内部
		HEADER_ST_DN,			  //鼠标在列表头内部并且鼠标按钮
	};

	MUL_HEADER_STATE  m_nHeaderState;                               //列表头元件的状态



};




//////////////////////////////////////////////////////////////////////////////////////
//
// 当前版本：1.0
// 作    者：罗伟坚
// 完成日期：
// 功能说明：多媒体表格控件,控件将数据以表格形式展现,每个item元素表格m行n列的数据T(mRow,nCol)
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinMediaTableCtrl : public CSkinCtrl, public IQvodCallBack,public QTimer::ITimerEvent	
{
public:

	//
	//外部接口功能组
	//

	//业务逻辑层操作   
	//ID为节点属性信息-文件ID号
	int  GetItem_ID(int nIndex);											//获取列表内序号为nIndex的ID编号
	int  GetItembyID(int nID);												//获取节点属性nID所在列表nIndex位置 
	int  GetSelectItem_ID(int nIndex = 0);									//获取当前选中item的原先节点的传入时候的ID编号(参数从0开始)
	int  GetSelectItem_ID(std::vector<int> & vtSelIdList);					//获取当前所有选中item的原先节点的传入时候的ID编号!
	int  GetSelectItem_ID(std::vector<long> & vtSelIdList);					//获取当前所有选中item的原先节点的传入时候的ID编号!
	BOOL RemoveItembyID(int nID);											//删除一个item数据,nID为节点的nID信息!
	BOOL ModifyItembyID(int nID, MEDIA_TABLE_ITEM *pItemInfo);				//修改指定item数据,nID为节点的nID信息!
	void SetItemShowStatebyID(int nID, BOOL bShowTime);						//设置item的显示模式  时间/进度	--仅在简洁模式有效,nID为节点的nID信息!
	void SetItemIconStatebyID(int nID, int  nState);						//设置item的左边Icon图像显示模式,nID为节点的nID信息!
	BOOL SetSelItembyID(int nID, bool bEmptyAll = false, bool bSelAll = false);//设置指定item为选中状态,nID为节点的nID信息,-1为清空所有选中

	//访问数据操作
	int  GetItemCount(void);												//获取item总数
	BOOL GetItemData(int nIndex, MEDIA_TABLE_ITEM *pItemInfo);				//获取指定item的序号和数据
	int  GetItemState(int nIndex);											//获取指定item的状态!	
	int  GetSelItemCount(void);												//获取选中item数目!
	void GetSelectItem(std::vector<UINT> & vSelIndex);						//获取当前所有选中item的序号
	int  GetSelectItem(void);												//获取当前选中item的序号
	BOOL GetSelectItemData(int & nIndex,MEDIA_TABLE_ITEM *pItemInfo);		//获取当前最后选中item的序号和数据
	BOOL GetIndexSelectItemData(int nSelected,MEDIA_TABLE_ITEM *pItemInfo);	//获取当前所有选中item中的第n个选中item数据
	int  GetHoverItem(void);												//获取当前鼠标停留(Hover)在item的序号!
	int  GetPageMaxShowItemCount(void);										//获取当前窗口大小每页最多可容纳item个数

	//修改数据操作
	BOOL InsertItem(MEDIA_TABLE_ITEM *pItemInfo, int nIndex = -1);			//增加一个item数据 -1为插入到最后
	BOOL RemoveItem(int nIndex);											//删除一个item数据
	void RemoveAllItem();													//删除(清空)所有item数据
	BOOL ModifyItem(int nIndex, MEDIA_TABLE_ITEM *pItemInfo);				//修改指定item数据
	BOOL SetSelItem(int nIndex);											//设置指定item为选中状态
	void SetItemHeight(int nHeight);										//设置item的高度
	void Sort(int nByKind);													//排序-根据指定方式
	BOOL MoveItem(std::vector<UINT>& vMoveIndex, int nPos);					//移动n项到指定的位置下
	BOOL EditItem(int nItem);												//编辑某一项的内容
	BOOL OverEdit(BOOL bModify);											//结束编辑，bModify：是否修改内容
	CSkinEdit* CreateEdit();												//创建edit控件

	//外观显示设置操作
	void SetShowMode(BOOL bSimpleMode);										//设置控件的显示模式  多媒体模式/简洁模式
	BOOL GetShowMode(void);													//获取控件的显示模式  多媒体模式/简洁模式
	void SetShowProgress(BOOL bShowProgress);								//设置控件是否显示进度条
	BOOL GetShowProgress(void);												//获取控件是否显示进度条
	void SetItemShowState(int nItem, BOOL bShowTime);						//设置item的显示模式  时间/进度	--仅在简洁模式有效
	BOOL GetItemShowState(int nItem);										//获取item的显示模式			--仅在简洁模式有效
	void SetItemPgShowState(int nItem, BOOL bShowGrayPg);					//设置item的进度条是否显示灰化状态
	BOOL GetItemPgShowState(int nItem);										//获取item的进度条是否显示灰化状态 
	void SetItemHalfTrans(int nItem, BOOL HalfTrans);						//设置item半透
	void GetPreviewImgSize(int & nWdith, int & nHeight);					//获取预览图的尺寸
	void SetItemShortCutImg(int nItem, Image * pImgBmp);					//设置指定item的缩略图	
	void SetNameAlignment(StringAlignment strAlignment);					//设置item的名字对齐属性
	void SetChangeImgTime(int nElapse);										//设置更换缩略图的时间 单位微秒
	int  GetChangeImgTime(void);											//获取更换缩略图的时间 单位微秒
	void SetReportItemFlag(BOOL bOpen);										//设置开关状态 - 报告当前页面显示item的项
	BOOL GetReportItemFlag(void);											//获取开关状态 - 报告当前页面显示item的项
	void SetRedraw(BOOL bDraw);												//设置重绘开关
	BOOL GetRedraw(void);													//获取重绘开关
	void SetDragSort(BOOL bDragSort);
	void EnableMoveText(BOOL bEnable);
	void Seek(int index);
	//
	//内部实现功能组
	//
public:
	CSkinMediaTableCtrl();
	virtual ~CSkinMediaTableCtrl();

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();													//反初始化
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//消息回调--对子控件创建时候传父类obj,让它实现的时候能逐层回溯
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//函数接口--类似COM查询接口,
	virtual BOOL OnTimer();													//自动调用定时器--查看是否需要重绘

	virtual BOOL OnKey(WORD nChar);											// 键盘消息
	virtual BOOL OnMouseMoveIn(short x, short y);							//鼠标处理消息-移进控件
	virtual BOOL OnMouseMoveOut();											//鼠标处理消息-移出控件
	virtual BOOL OnMouseDown(short x, short y);								//鼠标处理消息-按下
	virtual BOOL OnMouseUp(short x, short y);								//鼠标处理消息-释放
	virtual BOOL OnMouseDrag(short x, short y);								//鼠标处理消息-拖动
	virtual BOOL OnMouseMove(short x, short y);								//鼠标处理消息-移动
	virtual BOOL OnRButtonUp(short x, short y);								//鼠标处理消息-右键释放
	virtual BOOL OnRButtonDown(short x, short y);							//鼠标处理消息-右键按下
	virtual BOOL OnDblClick(short x, short y);								//鼠标处理消息-左键双击
	virtual BOOL OnWheel(short nStep);										//鼠标处理消息-滚轮滚动
	virtual	long OnDrag(UINT uMsg, long wparam, long lparam);				//拖动消息处理
	virtual BOOL Layout(short nWidth, short nHeight);						//陈列展现控件--该函数由改变窗口大小位置的时候出发WM_SIZE

	void Paint(Graphics *pGraph);											//绘制控件
	void Paint_SimpleMode(Graphics *pGraph);								//绘制控件 精简模式
	void Paint_MultiMode(Graphics *pGraph);									//绘制控件 多媒体模式

	void DrawItem(Graphics *pGraph, int nItem);								//绘制某一个item
	void DrawItem_SimpleMode(Graphics *pGraph, int nItem);					//绘制某一个item
	void DrawItem_FlatStyle(Graphics *pGraph, int nItem);					//绘制某一个item
	void DrawItem_FlatStyle(Graphics *pGraph, int nItem, Rect& itemRect);
	void DrawItem_WaveStyle(Graphics *pGraph, int nItem);					//绘制某一个item
	void DrawItem_FrameStyle(Graphics *pGraph, int nItem);					//绘制某一个item
	void DrawListHeader(Graphics *pGraph);									//绘制列表头
	void Draw_Progress(Graphics *pGraph, int nItem, Rect & pgRect);			 //绘制某一个item进度条
	void GetItemRect(int nItem, Rect & rtItem);								//获取item的区域
	void OnCtrlScroll(long lPos);											//滚动条处理函数
	void RefreshScrollCtrl();												//重新设置scroll控件参数
	void RefreshAllItem_VitualPos(void);									//刷新当前结构下所有节点的显示虚拟位置
	void ReLayOutTheCtrlbyFix(void);										//重新修正layout
	BOOL CheckVerScroll(const short x, const short y);						//检测坐标是否在Scroll内
	BOOL CheckSpliter(const short x, const short y);						//检测坐标是否在Spliter内
	BOOL CheckCursor(short x, short y);										//检测坐标是否在控件内
	BOOL IsPointInRect(Rect rt, int x, int y);								//检测坐标是否在矩形内
	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//从XML文件载入皮肤元素的配置
	BOOL IsPointInItemArea(short x, short y);								//判断坐标是否在文字区域内
	BOOL IsRectInFrame(Rect rtRect, Rect rtFrame);							//判断矩形是否在框架内
	BOOL IsRectHeavyInFrame(Rect rtRect, Rect rtFrame);						//判断矩形的重心区域是否在框架内
	BOOL IsRectMidPointInFrame(Rect rtRect, Rect rtFrame);					//判断矩形的重心是否跟框架有交集框架
	BOOL IsFrameInRect(Rect rtRect, Rect rtFrame);							//判断框架是否在矩形内
	void  CtrlPosCalculate(void);											//计算元件属性的相应位置
	void OnLBtnClickItem(int nClickItem);									//鼠标点击某一item

	void sort_by_ID(void);													//排序--根据ID
	void sort_by_Progress(void);											//排序--根据进度
	void sort_by_NetQuality(void);											//排序--网络质量
	void sort_by_Time(void);												//排序--根据播放时长
	void ReportPageItemShow(int nLower, int nHigher);						//报告当前页面显示item的项
	void RequestChangeMovieImg(void);										//请求更换某item的缩略图

	CString m_strMovingText;  //当前滚动文字内容
	int m_nstrMovingTextX ;   // 滚动距离
	BOOL  m_bFirstDrawTextBmp;

	BOOL m_bsActiveMovingText;										//是否激活滚动文字
	BOOL m_bsTimerWorkDirty;										//是否定时器已触发并要求处理(重绘)


	//////////////////////////////////////////////////////////////////////////
	// 定时器处理
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

	//static void CALLBACK MovingTextTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime); //滚动文字的定时器
	//static void CALLBACK ChangeImgTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);  //换缩略图的定时器
	void ChangeMovingText(CString strNewText);		//改变滚动文字内容
	//static void CALLBACK RollImgTimerCallBackFun(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);// 滚动缩略图定时回调函数
	void SetRollImageTimer(UINT uRollElapse = 750);	// 设置滚动计时器
	void KillRollImageTimer();						// 取消滚动计时器

	const Rect &GetImageRect()const;				// 返回缩略图位置
	void GetSkinSnaps();

	//设置滚动条的可视状态
	void VisualizeScrollCtrl(void);

	//根据item数目重新计算高度
	int GetItemTotalHeight(void);

	//设置元素对应的媒体文件是否存在
	void SetItemFileExist(int index, BOOL bExist);
	void SetItemFileCanPlay(int nID, BOOL bPlay );
	void ViewDataDone(void); // 本控件数据更新完毕后，回调该函数
	void CurrViewGone(void); // 当前页面消失后调用
	void CheckItemSel(void); // 检查数据元的选中状态
	BOOL OnDirectionKey(WORD key);

	//拖动绘制处理
	void DoDragDrop(int xDragImgMove, int yDragImgMove);
	SIZE DrawDragBmp(HBITMAP& hBitmap);
	void GetDestItem(int x, int y, int& nInsertItem, int& nEnterItem);
	void MouseDragMoveItem(int nDestItem);
	BOOL GetDragList(std::vector<CString> & vDragList);
	void EnableDrawSelCaptain(BOOL bEnable){m_bDrawSelCaptain = bEnable;}

	// DragDrop相关
	virtual BOOL DragMouseIn(short x, short y);
	virtual BOOL DragMouseOut();
	virtual BOOL DragMouseMove(short x, short y);
	virtual BOOL DragMouseUp(short x, short y);
	//
	//成员属性
	//
private:

	std::vector<MEDIA_TABLE_ITEM*> m_vtItemList;					//跟显示对象关联的数据链

	MULTI_LIST_MSG  m_mouseMSG;										//用于鼠标向父类发触发消息,包含多种信息

	CXMLElement		m_xmlItemStyle;									//Item 的模板
	CSkinScroll*	m_pScroll;										//滚动条子控件		

	CRITICAL_SECTION m_rsLock;										//资源锁-用于防止多线程中删除操作与绘制过程的冲突

	long m_nTotalHeight;											//Item的总高
	BOOL m_bShowScroll;												//是否需要显示Scroll
	BOOL m_bInscroll;												//鼠标是否在scroll上
	long m_lPageViewPos;											//当前显示页面相对总顶部的偏移(Scroll的偏移量)
	long m_nScrollWidth;											//Scroll的宽度
	BOOL m_bScrolldown;												//Scroll down
	BOOL m_bDrag;													//鼠标在拖动时为 TRUE
	BOOL m_bRButtonDown;											//鼠标右键是否为按下状态!
	BOOL m_bLBDbClickDown;											//鼠标双击是否为按下状态!

	CString m_nameFace;
	int	 m_iNameFontSize;											//字体大小 -名称
	int	 m_iTimeFontSize;											//字体大小 -时间

	Rect m_TranscodeRC;												// 转码区域
	Rect m_btRect;													//方块区域
	Rect m_btSimpleRect;											//方块区域
	Rect m_textNameRect;											//文本区域
	Rect m_textTimeRect;											//文本区域
	Rect m_textTimeSimpleRect;										//文本区域
	Rect m_pgRect;													//进度条区域
	Rect m_pgSimpleRect;											//进度条区域
	Rect m_imgRect;													//图片区域
	Rect m_imgHVRect;												//图片区域hover状态

	Rect m_rtDragFrame;												//鼠标拖选时候绘制的框!
	CPoint m_ptDragStart;										    //记录鼠标拖选时候绘制的按下起点!
	CPoint m_ptDragEnd;										        //记录鼠标拖选时候绘制的到达终点!

	Color m_clrItemBK_NL;											//背景颜色Normal状态
	Color m_clrItemBK_HV;											//背景颜色Hover状态
	Color m_clrItemBK_DN;											//背景颜色Select(down)状态

	Color m_clrNameText_NL;											//背景颜色Normal状态
	Color m_clrNameText_HV;											//背景颜色Hover状态
	Color m_clrNameText_DN;											//背景颜色Down状态
	Color m_clrTimeText_NL;											//背景颜色Normal状态
	Color m_clrTimeText_HV;											//背景颜色Hover状态
	Color m_clrTimeText_DN;											//背景颜色Down状态

	Color m_clrDragFrame;											//拖拽框颜色!
	Color m_clrDragFrameBorder;										//拖拽框边的颜色!
	Bitmap * m_aImg[M_LIST_BMP_MAX_NUM];							//存储各个状态的绘制位图
	CString m_textFontStyle;										//字体样式!
	CString m_textTimeFontStyle;									//时间字体样式!

	Image *m_pFrontImgOfRoll;										// 滚动显示缩略图中的前面一幅图
	SHORT  m_assWidthOfFrontImg;									// 滚动显示缩略图中的前面一幅图的显示宽度
	USHORT m_distancePerRoll;										// 滚动显示缩略图中每次滚动的像素距离
	USHORT m_timeGapOfRoll;											// 滚动显示缩略图中每次滚动的时间间隔
	StringAlignment m_aNameAlignment;								// 名字对齐属性

	int   m_nItemHeight;											//item的高度-当前模式
	int   m_nItemSimpleHeight;										//item的高度-简洁模式
	int   m_nItemMultiHeight;										//item的高度-多媒体模式
	int   m_nItemMultiWidth;										//item的宽度-多媒体模式

	int   m_nBorderTopSpace;										//item顶部边缘空间
	int   m_nBorderLeftSpace;										//item左边边缘空间
	int   m_nRowSpace;												//item之间行间隔空间
	int   m_nMinColSpace;											//item之间列最小的间隔空间
	int   m_nColSpace;												//item之间列间隔空间

	int   m_nItemCount;												//item总数
	int   m_nItemHover;												//当前鼠标悬挂在哪一个item
	int   m_nItemSel;												//当前选中哪一个item
	int   m_nItemDown;												//当前鼠标按下在哪一个item
	int   m_nItemSelCount;											//当前选中item的数目!
	int   m_nItemDragFrameDown;										//框选时按下按钮所在的选项!
	int   m_nIDSelCaptain;											// 当前选择准星的ID

	Rect  m_HeaderRect;												//列表头区域
	Rect  m_ListRect;												//列表区域

	int   m_nItemDrawStyle;											//item绘制样式
	BOOL  m_bHaveHeader;											//是否有列表头
	int   m_nHeaderHeight;											//列表头的高度
	int   m_nSimpleHeaderHeight;									//简洁模式列表头的高度


	BOOL  m_bShowSimpleMode;										//控件的模式是否为简洁模式 否则为多媒体模式
	BOOL  m_bShowTimeState;											//item显示模式是否为时间 否则为 进度--仅在简洁模式有效
	BOOL  m_bShowPgMode;											//item显示模式是否显示进度条

	BOOL  m_bShowDragFrame;											//是否在鼠标拖拽时显示框!
	BOOL  m_bDragWay_byArea;										//是否框选触发由文字区域外点击而触发-否则为当前是否已经选中!
	int m_nChangeImgSlice;											//更改更换缩略图的时间片数(以1次paint的OnTimer()调用间隔为1个单位)
	int m_nPointInElement;											//鼠标所在选项的子元素区域
	int m_nMovingTextSlice;											//hover状态的文字滚动首文字的时间片数(以1次paint的OnTimer()调用间隔为1个单位)
	int m_nItemHoverTextMove;										//hover状态的文字滚动首文字在第几个
	int m_nPerPageRowNum;											//每页最多可容纳的行数
	int m_nPerPageColNum;											//每页最多可容纳的列数
	int m_nTotalRowNum;												//总共占据的行数
	int m_nTopViewRow;												//滚动页中,最顶部的行数   
	// 由于 layout 引起的刷新 m_nTopViewRow
	BOOL m_blayoutRefreshTopViewRow;
	BOOL m_bOpenReportItemFlag;										//是否开启报告当前页面显示item的项功能
	int m_nOldShowLower;											//当前显示最顶项-用作报告状态
	int m_nOldShowHigher;											//当前显示最后项-用作报告状态
	int m_nOldShowLowerID;											//当前显示最顶项文件ID-用作报告状态
	int m_nOldShowHigherID;											//当前显示最后项文件ID-用作报告状态
	BOOL m_bRedraw;													//图像更新时是否绘制控件

	
	int	  m_nEditItem;												//编辑框所在的项
	BOOL  m_bEditShow;												//编辑框是否在显示
	BOOL  m_bClickTwice;											//是否两次点击了同一个项
	CSkinEdit*		m_pEdit;										//编辑框控件
	//ui::drop::CDragDropCtrl	m_DragDropCtrl;									//拖拽控件
	HBITMAP			m_hDragBitmap;									//拖拽位图
	BOOL			m_bDragSort;									//是否允许拖拽排序
	int				m_nDragInsertPos;								//拖拽排序的插入位置
	int				m_nDragRow;										//拖拽时鼠标所在行数		
	std::vector<CString> m_vtDragList;								//拖拽文件/文件夹名称的数据链
	BOOL            m_bDrawSelCaptain; // 是否绘制SelCaptain	
	BOOL			m_bEnableMoveText;
	QTimer::CTimer m_timer;
	int m_lastMouseUpID;

	enum MUL_HEADER_STATE
	{
		HEADER_ST_OUT     = 0,    //鼠标在列表头外部
		HEADER_ST_IN,			  //鼠标在列表头内部但不在其他按钮区域
		HEADER_ST_BTA_HV,		  //鼠标在列表头按钮A区域  Hover	
		HEADER_ST_BTA_DN, 		  //鼠标在列表头按钮A区域  Down 
		HEADER_ST_BTB_HV, 		  //鼠标在列表头按钮B区域  Hover 
		HEADER_ST_BTB_DN,  		  //鼠标在列表头按钮A区域  Down  	
		HEADER_ST_BTC_HV, 		  //鼠标在列表头按钮C区域  Hover
		HEADER_ST_BTC_DN,  		  //鼠标在列表头按钮A区域  Down  
	};

	MUL_HEADER_STATE  m_nHeaderState;                               //列表头元件的状态
};


#define MFP_FIRST 1
#define MFP_LAST  2

class CSkinColorMenu//:public CMenu
{
public:
	CSkinColorMenu();
	~CSkinColorMenu();

	/*************消息响应函数***************/

	//WM_INITMENUPOPUP的响应函数，用来做一些初始化工作
	LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

	//WM_MEASUREITEM的响应函数，用来指定菜单项大小
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

	//WM_DRAWITEM的响应函数，用来自绘
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnUnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	/***************************************/

public:
	//功能：设置菜单项图片
	
	BOOL SetMenuItemImage(HMENU hMenu, UINT item, Image* pImg, BOOL bByPos = FALSE);
	
	//功能：设置菜单项文本
	BOOL SetMenuItemText(HMENU hMenu, UINT item, LPCTSTR sItemName, BOOL bByPos = FALSE);

	//功能：设置菜单项快捷键
// 	BOOL SetMenuItemAcc(HMENU hMenu, UINT item, LPCTSTR sAccName, BOOL bByPos = FALSE);

	//功能：设置菜单项的快捷键
	BOOL SetAllMenuItemAcc(HMENU hMenu/*, std::map<UINT, ACCEL>& mAccel*/);

	//功能：将菜单项设为隐藏
	//hMenu为菜单项所在弹出菜单句柄
	//uItem为菜单的索引或ID号，由第3个参数决定
	//是由ID号还是索引号访问（若要设置一个弹出菜单项，则必须由索引号访问）
	BOOL SetMenuItemHide(HMENU hMenu, UINT uItem, BOOL bByPos = FALSE);

	BOOL RemoveMenu(HMENU hMenu, UINT uItem, UINT uFlags);
	
	//功能：显示菜单  
	//hwnd:显示在哪个窗口之上     index:显示第几个弹出菜单
// 	BOOL DisplayMenu(HWND hwnd, short x, short y, UINT index = 0);

// 	//功能：扩展菜单
// 	BOOL ExpandMenu();

	//功能：销毁菜单
	BOOL DestroyMenu(HMENU hMenu);

	//功能: 加载菜单配置
	//LPCTSTR lpSkinName: 皮肤名
	//LPCTSTR lpStyleName: 菜单风格名
	BOOL ResetSkin(LPCTSTR lpSkinName, LPCTSTR lpStyleName);

protected:
	//为了去掉菜单的边框以及变成圆角菜单，利用钩子替换窗口过程函数
	static LRESULT CALLBACK WindowsHook (int code, WPARAM wParam, LPARAM lParam);
	//窗口过程
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT MenuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//画圆角矩形
	static void MakeRoundRect(GraphicsPath &gdiPath, Point topLeft, Point bottomRight, Point round);
	//调整HSL
//	Color AdjustHSL(float kH, float kS, float kL, short bH, short bS, short bL, Color color);

	BOOL IsLongMenuInXP(int nHeight);

protected:
	typedef	struct 	            // 菜单项信息
	{
		LPTSTR lpstrText;
		LPTSTR lpstrAcc;
		UINT fType;
		UINT fState;
		int nPos;
		BOOL bHide;
		Image* pImage;
	}MenuItemData, *LpMenuItemData;

	typedef	struct 	            // 菜单项信息
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
	Bitmap* m_pMemBmp;					//菜单内存位图

	BYTE  m_nAlpha;						//菜单透明度
//	float m_lBackAlpha;					//左边背景的透明度
//	float m_rBackAlpha;					//右边背景的透明度
//	float m_textAlpha;					//字的透明度（选中时）

//	HWND m_currentHwnd;					//当前所在窗口
//	Point m_currentPoint;				//当前坐标
//	UINT m_currentID;					//当前加载的菜单ID
//	UINT m_currentIndex;				//当前显示弹出菜单的索引
//	UINT m_nCurDrawItem;				//当前正在画的菜单项位置
	int m_nScreenHeight;				//屏幕高度
//	int m_nDelayTime;					//子菜单延迟弹出的时间（ms）
	int m_nTmpItemHeight;
	BOOL m_bSetAnimation;				//用户是否设置了“淡入淡出显示菜单”
	BOOL m_bDropShadow;					//是否启用了菜单阴影
	BOOL m_bDoubleBuffer;				//是否采用双缓冲
//	BOOL m_bLongMenu;					//是否是长菜单
//	BOOL m_bPopParent;
//	BOOL m_bPaintByPull;
//	std::map<HWND, MenuDrawData> m_mDrawMap;

//	GraphicsPath* m_pWndPath;

	CString m_strSkinName;				//当前皮肤名
	CString m_strStyleName;				//当前菜单样式(一套皮肤可能有多种菜单样式）

	//以下参数从配置文件读取
	//color
	Color m_cLeftBgColor;				//菜单左边背景颜色
	Color m_cRightBgColor;				//菜单右边背景颜色
	Color m_cEdgeColor;					//菜单边框颜色
	Color m_cSelectFontColor;			//菜单选中时字体颜色
	Color m_cGrayFontColor;				//菜单禁用时字体颜色
	Color m_cNormalFontColor;			//菜单普通字体颜色
	//Color m_menuSelectColor1;			//菜单选中时的颜色
	//Color m_menuSelectColor2;			//菜单选中时的颜色
	//Color m_menuSeparatorColor1;		//菜单分隔条颜色（深）
	//Color m_menuSeparatorColor2;		//菜单分隔条颜色（浅）

	//bitmap
	Bitmap* m_pImgTick;					//勾
	Bitmap* m_pImgPoint;				//点
	Bitmap* m_pImgDown;					//向下的箭头
	Bitmap* m_pImgRight;				//向右的箭头
	Bitmap* m_pSelectBar;				//选中条
	Bitmap*	m_pSeparator;				//分割线

	//size
	UINT m_cySpacing;					//菜单项字高和菜单项高的间距  (菜单项高度 = 字高度 + m_cySpacing*2）
	UINT m_cxMidSpacing;				//菜单项名和快捷键之间的最短间距
	UINT m_nLeftWidth;					//菜单左边宽度   （菜单项宽度 = 字宽度 + m_nLeftWidth + m_cxMidSpacing）
	UINT m_nTopSpacing;					//菜单顶部间隙
	UINT m_nBottomSpacing;				//菜单底部间隙
	UINT m_nSelectBarX;					//选条的横坐标（相对于菜单项）
	UINT m_nSelectBarY;					//选条的纵坐标（相对于菜单项）
	int m_nMenuRadian;					//菜单的圆角弧度
	
	//font
	Font*  m_pMenuFont;					//菜单字体

public:

	/*********用于CColorMenuBar的变量***********/
//	BOOL m_bMenuClosed;					//菜单是否已关闭
	BOOL m_bHasChild;					//选中的菜单项是弹出菜单
//	HMENU m_currentMenu;				//选中菜单项的所在菜单句柄
	UINT m_lastItemID;					//菜单退出前最后一个被选中的菜单项ID
//	HWND m_hWnd;

};

/******************
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：
* 功能说明：此类是分隔条
********************/
class CSkinSplitter : public CSkinImage	// 分隔条类
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

class CSkinDisplayBox : public CSkinCtrl, public IQvodCallBack	// 带有滚动条的内容显示框
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
	std::vector<CSkinButton_Check*> m_pvChangGui;	//存放常规文件
	std::vector<CSkinButton_Check*> m_pvMepg;		//存放Mepg
	std::vector<CSkinButton_Check*> m_pvReal;		//存放Real
	std::vector<CSkinButton_Check*> m_pvQuickTime;	//存放QuickTime
	std::vector<CSkinButton_Check*> m_pvWindows;	//存放Windows
	std::vector<CSkinButton_Check*> m_pvQTVideo;	//存放其他视频

	std::vector<CSkinButton_Check*> m_pvCJAudio;	//存放常见音频
	std::vector<CSkinButton_Check*> m_pvQTAudio;	//存放其他音频
	std::vector<CSkinButton_Check*> m_pvFlash;		//存放Shockwa Flash
	std::vector<CSkinButton_Check*> m_pvPlayFileList;	//存放播放列表文件
//	std::vector<CSkinButton_Check*> m_pvIMGFile;	//存放图片文件
	std::vector<CSkinButton_Check*> m_pvBtFile;		//存放BT种子文件

public:
	CSkinScroll *m_pScroll;
	BOOL m_hasDown;
	int m_totalHeight; //将显示的所有高，如果没有，就是和看到的界面一样高
	int m_offpos;
	BOOL m_showScroll;
	CSkinCtrl *m_pOldObj;
	CSkinCtrl *m_pDownObj;
	BOOL m_bSkinObjConstructOK;
};


/*************************************************
Class Name: CDropList
Author:Xiacc  Date: 2010-3-1
Description: 自绘类型CListBox，与CSkinCombobox类结合，形成自绘类型CComboBox
Others:      CDropList显示在CWinDropList窗口中
Function List:  主要是在使用前调用SetComboboxParent, SetColor, SetTextFont
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
	//功能接口
	//	
	int SetItemHeight(int nIndex, UINT cyItemHeight = 16);
	void SetComboboxParent(void *hParent);
	void SetColor(COLORREF crBkColor, COLORREF crTextColor, COLORREF crBorderColor);
	void SetColor(COLORREF clrBK, COLORREF clrText, COLORREF clrHoverText,
		          COLORREF clrItemNormal, COLORREF clrItemHover, COLORREF clrNormalFrame);     //设置颜色属性
	void SetTextFont(CString face, UINT size, int style);
	BOOL SetIndex(UINT nIndex);	//设置指定项的下标
	BOOL			m_bFirstShow;		//是否为第一次显示
protected:

	LRESULT OnDrawItem(UINT    uMsg, WPARAM wp, LPARAM lp, BOOL &);						       //绘制item处理函数
	LRESULT OnMouseMove(UINT   uMsg, WPARAM wp, LPARAM lp, BOOL &);						       //鼠标移动理函数
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);						       //鼠标左键点击处理函数
	LRESULT OnKillFocus(UINT   uMsg, WPARAM wp, LPARAM lp, BOOL &);						       //KillFocus处理函数
	void DrawOneItem(int nIndex, COLORREF crBkColor, COLORREF crTextColor);					   //只画一个item的区域

	UINT			m_nHight;			//每项的高度
	UINT			m_nMouseIndex;		//当前光标位置所在项
	UINT			m_nIndex;			//当前指定项的下标
	COLORREF		m_clrBK;		    //背景颜色
	COLORREF		m_clrText;		    //文字颜色
	COLORREF		m_clrHoverText;		//文字颜色
	COLORREF		m_clrItemNormal;	//边框颜色
	COLORREF		m_clrItemHover;		//边框颜色
	COLORREF		m_clrNormalFrame;	//边框颜色

	CSkinCombobox	*m_pComboBox;		//相关联的CSkinComboBox
	//
	CString			m_face;				//字体
	float			m_size;				//字大小
	int				m_style;			//字风格
};

//////////////////////////////////////////////////////////////////////////////////////
// 类名称  : CWinDropList
// 当前版本: 1.0
// 作    者: Xiacc, 修改by罗伟坚
// 完成日期: 
// 功能说明: 一个弹出式窗口类，与CSkinCombobox类结合，形成自绘类型CComboBox
//           由CSkinCombobox决定是否显示CWinDropList窗口
//////////////////////////////////////////////////////////////////////////////////////
class CWinDropList : public CWindowImpl<CWinDropList>
{
public:
	BEGIN_MSG_MAP(CDropList)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
	END_MSG_MAP()

	LRESULT OnDrawItem(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &);	//绘制item处理函数
};


//////////////////////////////////////////////////////////////////////////////////////
// 类名称  : CSkinCombobox
// 当前版本: 1.0
// 作    者: Xiacc, 修改by罗伟坚
// 完成日期: 
// 功能说明: 可与CWinDropList类和CDropList类结合，形成自绘类型CComboBox
//
//////////////////////////////////////////////////////////////////////////////////////
class CSkinCombobox : public CSkinCtrl
{
public:

	CSkinCombobox();
	virtual ~CSkinCombobox(){};

	//
	//功能接口
	//	
	void HideDropList_OutSide();									//隐藏DropList(当鼠标在控件外部)
	void HideDropList_Strong();										//无条件隐藏DropList
	BOOL SetIndex(int nIndex);										//由下标得文字 专在CDropList中调用和用此函数为接口从外部设置文字
	BOOL SetString(LPCTSTR lpszText);								//设置显示的文本
	int	 AddString(LPCTSTR lpszItem);								//CDropList添加子项
	void RemoveAllItem();											//删除所有item
	BOOL SetItemHeight(int nIndex, UINT nHeight);					//设置item高度
	int	 GetItemCount();											//获取所有item数
	int  GetSelectItemIndex();										//获取选中的项
	int  SetSelectItemIndex( int index ) ;							//设置选中的项
	CString GetItemText(int nIndex);								//获取item的文本
	void SetColor(COLORREF crBkColor, COLORREF crTextColor, 
		COLORREF crTriColor, COLORREF crBordColor);					//设置颜色属性
	long Method_(LPCTSTR szAttrib, long para1, long para2);

	//
	//内部处理函数
	//
protected:

	virtual BOOL Init_(CXMLElement &xmlNode,		
		               IQvodCallBack *pEventEntry);					//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();											//反初始化
	virtual void Paint(Graphics *pGraph);							//绘制函数
	virtual BOOL OnTimer();											//自动调用定时器--查看是否需要重绘
	virtual BOOL OnMouseMoveOut();									//鼠标移出控件处理函数
	virtual BOOL OnMouseMoveIn(short x, short y);					//鼠标移进控件处理函数
	virtual BOOL OnMouseDown(short x, short y);						//鼠标按下处理函数
	virtual BOOL OnMouseUp(short x, short y);						//鼠标释放处理函数
	virtual BOOL CheckCursor(short x, short y);						//探测鼠标
	virtual BOOL OnRButtonDown(short x, short y);
											
	//
	//属性
	//
private:
	BOOL		m_bExtend;			//CWinDropList是否隐藏
	UINT		m_nDropItemNum;		//CDropList子项的总数
	UINT		m_nHeight;			//CDropList项的高度


	COLORREF	m_clrBK;		    //背景颜色
	COLORREF	m_clrText;		    //文字颜色
	COLORREF	m_clrNormalFrame;	//边框颜色
	COLORREF	m_clrHoverFrame;	//鼠标在其上的颜色


	CString		m_strText;			//当前的文字
	CString		m_face;				//文字字体
	float		m_size;				//文字大小
	int			m_style;			//文字风格

	Bitmap *		m_pBtImg;		//按钮图像

	CSkinEdit*  m_pEdit;
	CDropList	m_DropList;			//弹出列表
	CWinDropList m_WinDropList;		//弹出列表
};

#define SC_MSG_ITEM_CLICK  1
#define SC_MSG_ITEM_BTN_CLICK 2
#define SC_MSG_ITEM_HOVER_CHNAGE 3
class CSkinSuperCombox : public CSkinCtrl, public IQvodCallBack
{
public:
	CSkinSuperCombox();
	virtual ~CSkinSuperCombox();

	//属性
	int	 GetSel();													//获取选中项的索引
	int	 GetItemCount();											//获取所有项数
	CString GetItemText(int nIndex);								//获取nIndex的文本
	CString GetText();
	//操作
	int	AddString(LPCTSTR lpszItem);								//增加一项
	int	RemoveItem(int nIndex);										//删除一项
	void RemoveAllItem();											//删除所有项
	void ShowDropList(BOOL bShow);									//显示/隐藏下拉框
	BOOL SetSel(int nSel);											//设置第nSel项内容为显示的内容
	BOOL SetText(LPCTSTR lpszItem);									//设置edit的文字
	HWND SetFocus();												//设置焦点
	void SetEditSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
	void GetEditSel(int& nStartChar, int& nEndChar);

	virtual void SetState(skin_state state);
	CSkinEdit* GetEdit(){return m_pEdit;};
	CSkinMultiListCtrl* GetListCtrl(){return m_pList;};
protected:

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);					//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();											//反初始化
	virtual void Paint(Graphics *pGraph);							//绘制函数
	virtual BOOL OnTimer();											//自动调用定时器--查看是否需要重绘
	virtual BOOL OnMouseMove(short x, short y);						//鼠标移动处理函数
	virtual BOOL OnMouseMoveOut();									//鼠标移出控件处理函数
	virtual BOOL OnMouseMoveIn(short x, short y);					//鼠标移进控件处理函数
	virtual BOOL OnMouseDown(short x, short y);						//鼠标按下处理函数
	virtual BOOL OnMouseUp(short x, short y);						//鼠标释放处理函数
	virtual BOOL CheckCursor(short x, short y);						//探测鼠标
	virtual BOOL Layout(short nWidth, short nHeight);
	virtual BOOL InBtnRect(short x, short y);
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);
	virtual long Method_(LPCTSTR szMethod, long para1, long para2);
	CSkinEdit* CreateEdit(CXMLElement &xmlNode);

	Bitmap *		m_pBtImg;			//按钮图像
	Bitmap *		m_pIconImg;			//图标按纽
	Bitmap *		m_pFrameImg_R;		//边框图像
	Bitmap *		m_pFrameImg_M;		//边框图像
	Bitmap *		m_pFrameImg_L;		//边框图像
	BOOL			m_bBtnOver;			//鼠标是否在按钮上
	BOOL			m_bListShow;		//下拉列表是否是显示状态
	CString			m_strText;			//当前显示的文字
	CString			m_face;				//文字字体
	float			m_size;				//文字大小
	int				m_style;			//文字风格
	int				m_nSel;				//选择的项索引
	int				m_nEditSpace_X;		//edit的x轴空隙
	int				m_nEditSpace_Y;		//edit的y轴空隙
	int				m_nBtnWidth;		//button宽度
	int				m_nBtnHeight;		//button高度
	int				m_nListMaxHeight;	//列表框的最高宽度
	int				m_nListOffset_x;
	int				m_nListOffset_y;
	int				m_nListWidth;
	COLORREF		m_clrBK;		    //背景颜色
	COLORREF		m_clrText;		    //文字颜色
	COLORREF		m_clrNormalFrame;	//边框颜色
	COLORREF		m_clrHoverFrame;	//鼠标在其上的颜色
	CSkinEdit*		m_pEdit;			//edit控件
	CSkinMultiListCtrl* m_pList;		//list控件
	CSkinWindow*	m_pListWnd;			//list外窗口
	BOOL			m_bLoginCtrl;		//是否是登录控件
	BOOL			m_bChildWnd;
	BOOL			m_bEditFocus;
};

class CSkinSelectSuperCombox: public CSkinSuperCombox
{
public:
	CSkinSelectSuperCombox();

protected:
	virtual void Paint(Graphics *pGraph);							//绘制函数
	void SetState(skin_state state);
private:
	BOOL			m_bShowOverImageEditFocus;	//如果编辑框内有焦点，也显示鼠标在其上的背景
};


/*************************************************
Class Name: CSkinTree
Author:Xiacc  Date: 2010-3-1
Description: 继承于CTreeViewCtrlEx,CSkinCtrl。当光标移动到二级目录，三级目录上时，分别在目录右边显示新增图标，删除图标。
			点击图标up后，分别增加子项，删除当前项。
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

	void SetPos(int pos);					//设置滚动条的偏移位置
	void UpdateScroll();					//更新滚动条,由外界调用
	void SetScroll(CSkinScroll *pScroll);	
	void SetStyle(BOOL bVectical = TRUE);	//TURE--竖滚动条  FALSE--横滚动条
//	void SetPage(long nPage);				//显示的高或宽（用户可以看得到的）
//	void SetDuration(long dur);				//需要显示的界面的总高或宽
	UINT GetSize();							//竖滚轴--宽  横滚轴--高
	BOOL IsDowning();						//是否按下中
	void SetList(CListViewCtrl *pList);
//	void SetTree(CSkinTree *pTree);
private:
	CListViewCtrl	*m_pList;
//	CSkinTree		*m_pTree;
	BOOL			m_bVectical;
	BOOL			m_bDown;
	BOOL			m_bTracking;
	BOOL			m_bDragging;			//是否处于拖拉中
	BOOL			m_bShowHoriztonal;		//是否显示竖型滚轴
	BOOL			m_bShowVectical;		//是否显示横型滚轴
	SIZE			m_Size;					//此时窗口大小
	SIZE			m_OldSize;				//以前窗口大小
	CSkinScroll		*m_pScroll;				//滚轴的指针--由外面传入
	CSkinScroll		*m_pScrollOver;			//光标所在滚轴的指针
	Bitmap			*m_pBmp;				//自身窗口的Bitmap

	SIZE			m_ClientSize;			//客户区大小
	UINT			m_nHeaderHeight;		//list的header高

	UINT			m_nTotalWidth;
	UINT			m_nTotalHeight;
	BOOL IsBlankArea(int x, int y);			//当竖型与横型滚轴都存在时，是否在空白区域

	void Paint();
	void UpdateVScroll();
	void UpdateHScroll();
	BOOL ShowScroll();

	void ShowListScroll();			//显示listScroll
	void UpdateListVScroll();		//设置Scroll显示的高度，总高度，和自身窗口的位置
	void UpdateListHScroll();		//设置Scroll显示的宽度，总宽度，和自身窗口的位置

//	void ShowTreeScroll();			//显示TreeScroll
//	void UpdateTreeVScroll();		//设置Scroll显示的高度，总高度，和自身窗口的位置
//	void UpdateTreeHScroll();		//设置Scroll显示的宽度，总宽度，和自身窗口的位置

};

/*
enum enItemStatus
{
	item_add = 1,
	item_delete
};

class CSkinTree : public CWindowImpl<CSkinTree, CTreeViewCtrlEx>,public CSkinCtrl, public IQvodCallBack	// 树项
{		//先要设置image 和 设置颜色SetColor() 以及 SetFont, SetHeight 高度与图标一样高最好
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

	void SetColor(COLORREF crBkColor, COLORREF crTextColor);	//设置背景色与字色
	void SetID(UINT nID);
	void SetItemStatus(HTREEITEM hTreeItem, enItemStatus ItemStatus);
//	void SetBmp(CString strAddPath, CString strDelPath);		//设置新建图片与删除图片
	
	UINT GetHScrollPos();
private:
	void Draw();	//画图片
	void SetPos(BOOL nPage, int pos);

	BOOL		m_bOverBmp;		//是否在图片上
	BOOL		m_bDownBmp;		//是否在图片上按下
	BOOL		m_bTracking;	//是否处于激活状态
	Bitmap		*m_hAddBmp;		//新建图片  图片的高度不能大于项高度
	Bitmap		*m_hDeleteBmp;	//删除图片	图片的高度不能大于项高度
	COLORREF	m_crBkColor;	
	COLORREF	m_crTextColor;

	HTREEITEM	m_hNowTreeItem;	//当前有图片的项
	HTREEITEM	m_hDownTreeItem;//当前有图片且被按下的项
	UINT		m_nBmpWidth;	//画多少图片的宽度
	UINT		m_nID;

	std::map<HTREEITEM , enItemStatus> m_MapItem;

	//hScroll
	CSkinScrollWnd	m_hScrollWnd;
	UINT			m_nhOffPos;
	int				m_nvStepDistance;	//竖滚动轴 OnWheel时的偏移量
	

	//vScroll
	CSkinScrollWnd	m_vScrollWnd;
	UINT			m_nvOffPos;
	int				m_nhStepDistance;	//横滚动轴 OnWheel时的偏移量
};


class CSkinMenu: public CMenu // 菜单	还没有设置字体--SetFont
{
public:
	CSkinMenu();

	BOOL DrawItem(LPDRAWITEMSTRUCT lpDrawItem);
	BOOL MeasureItem(LPMEASUREITEMSTRUCT lpDrawItem);
	BOOL LoadMenu(UINT nIDResource, HMENU hMenu = NULL);	//改变当前资源菜单自身与其子菜单的风格
private:
	int  GetPopItemID(TCHAR *szTitle);	//从szTitle中得到相对应得ID; 详见 _menupopitemresid

	UINT m_nSepartor;
	UINT m_nHeight;
	UINT m_nWidth;
	UINT m_nDistance;

	COLORREF m_crBkSepartor;
	COLORREF m_crSepartor;

	COLORREF m_crNoSelBk;		//未选中的背景颜色	
	COLORREF m_crNoSelText;
	COLORREF m_crSelBk;			//选中的背景颜色
	COLORREF m_crSelText;
	COLORREF m_crSelBorder;

};



class CSkinEdit : public CSkinCtrl	//文本框
{
};





class CSkinTreeItem : public CWindowImpl<CSkinTreeItem, CTreeItem>, public CSkinCtrl	// 树项
{
};

class CSkinTree : public CWindowImpl<CSkinTree, CTreeViewCtrlEx>, public CSkinCtrl	// 树
{
};

class CSkinMenuItem : public CSkinCtrl	// 菜单项
{
};

class CSkinMenu: public CWindowImpl<CSkinMenu>, public CSkinCtrl, public IQvodCallBack	// 菜单
{
public:
long EventNotify_(UINT uMsg, long wparam, long lparam);
};

class CSkinSubWindow : public CWindowImpl<CSkinSubWindow>, public CSkinCtrl, public IQvodCallBack	// 子窗口
{
public:
long EventNotify_(UINT uMsg, long wparam, long lparam);
};
*/
enum remove_feet_type
{
	rf_none = 0,			// 不用去脚
	rf_special,				// 奇怪
	rf_img_all,				// 根据图片切四角
	rf_img_l,				// ....切左边
	rf_img_lt,				// ....切左上角
	rf_img_lb,				// ....切左下角
	rf_img_t,				// ....切上面2个角
	rf_img_r,				// ....切右边
	rf_img_rt,				// ....切右上角
	rf_img_rb,				// ....切右下角
	rf_img_b,				// ....切下边2角
	rf_triangle_all,		// 四角都为三角形
	rf_triangle_l,			// 左边的脚都为三角形
	rf_triangle_lt,			// 左上角
	rf_triangle_lb,			// 去掉左下角的三角形
	rf_triangle_r,			// 右边的脚都为三角形
	rf_triangle_rt,			// 右上角
	rf_triangle_rb,			// 右下角
	rf_round_all,			// 四角都为园的。
	rf_round_l,				// 左边的脚位圆形	
	rf_round_r,				// 右边的脚位圆形
	rf_round_lt,			// 去左上角的圆角
	rf_round_lb,			// 去左下角的圆角
	rf_round_rt,			// 去右上角的圆角
	rf_round_rb,			// 去右下角的圆角
	rf_irregular,			// 不规则
	rf_hollow				// 中间空
};

/******************
* 当前版本：1.0
* 作    者：关玮
* 完成日期：
********************/
class CMenuHost;
class CSkinWindow : public CWindowImpl<CSkinWindow>, 
	                public CSkinCtrl, 
	                public IQvodCallBack,
					public ui::drop::IDropTargetEvent	// 窗口
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
		//菜单消息
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

	//菜单消息
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
	virtual BOOL ResetSkin2(LPCTSTR pSkinXmlPath); //通过指定xml重设皮肤
	virtual void GetSkinSnaps(void){};
	virtual void SetSkinSnaps(void){};
	virtual int FindRestoreDate(LPCTSTR id, LPCTSTR tag = 0);// 查找需要还原的数据
	
	virtual void SetUseFlag();
	// 拖拽相关
	void InitDropTarget();   // 初始化DropTarget,使窗口可以响应DropTarget事件，必须在OnCreate时调用
	void ClearDropTarget();	 // 清理DropTarget，必须在OnClose时调用
	
	// 功能：开始拖拽
	// 参数：pDragSkin 拖拽的控件,pData 拖拽的数据
	virtual void DoDragDrop(ISkin* pDragSkin,ui::drop::DragDropDataPara* pData); 
	virtual long DropTargetDragEnter(IDataObject *pDataObject,POINT &pt);	// 拖拽数据进入窗口 
	virtual long DropTargetDragLeave();			// 拖拽数据离开窗口
	virtual long DropTargetDragOver(POINT &pt);	// 拖拽数据在窗口中移动
	virtual long DropTargetDrop(IDataObject *pDataObject,POINT &pt); // 停止拖拽

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
	virtual void ResetRegion();		// 挖
	void SetRegionByCorner();		// 挖规则图形的角
	void SetRegionByTransColor();	// 挖去图中所有的透明色处。
	void SetRegionByImage();		// 根据图片去挖窗口的边角
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
//	BOOL m_bThread;		// 线程是否存在
	BOOL			m_bExit;		// 线程退出标志
	HANDLE			m_hTread;
	CRITICAL_SECTION m_csPaint;
//	BOOL			m_bSizing;		
	TOOLINFO		m_toolInfo;
	BOOL			m_btipshow;
	BOOL			m_bNoShowADTip;
	CString			m_strSkinID;
	BOOL			m_bUsed;		// 窗口是否可用

//protected:
	ISkin		*m_pSkinOver;
	ISkin		*m_pSkinDown;
	ISkin		*m_pSkinFocuse;
	ISkin		*m_pSkinDragTarget;
	CSkinImage	*m_pShaddingImg;
	BOOL		m_bMouseDown;
	BOOL		m_bDrawShadding;
	BOOL		m_bTracking;
	POINT		m_ptLBtDown;		// 左键按下位置
	DWORD		m_dwEdgeColor;       // 边框像素（只对规则矩形有效）
	remove_feet_type m_rf_type;		// 窗口挖图的类型
	int			m_nRoundW;			// 挖图宽度(只对去三角或去圆角有用)
	BOOL		m_bRegionFinished;	// 完成挖图
	//ui::drop::CDragDropCtrl	m_DragDropCtrl;	// 拖拽控件
	BOOL		m_bEffect;			// 是否在特效显示
	BOOL		m_bUpwardShutter;	// 向上卷帘
	CMenuHost*	m_pMenuHost;		// 菜单
	typedef struct tRESTORE_DATA
	{
		CString id;
		CString tag;
		CString val;
	}RESTORE_DATA;
	std::vector<RESTORE_DATA> m_aRestoreData; // 换肤后要复原的数据
	ui::drop::CDropTarget *m_pDropTarget;
private:
	BOOL		m_bRMouseDown;		// 右键按下
};

// 说明：混合特效窗口
// 作者：匡晓林
// 时间：2012-09-10
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
	virtual void ShowEdit(BOOL bShow);			// 显示\隐藏窗口中的Edit窗口,弱Edit控件状态为不可见，则不显示，隐藏没要求。
	virtual void SetState(skin_state state);
	virtual void ResetBackground(BOOL bUpward ,RECT rcLink);
	void CreateWnd(HWND hParent ,Rect rc , DWORD dwStyle ,DWORD dwExStyle = 0);	// 创建窗口。解决窗口类名与CSkinWindow重名的问题。
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
	BOOL				m_nTransR;		// 右边界透明像素。
};

/******************
* 当前版本：1.0
* 作    者：刘建敏
* 完成日期：
* 功能说明：此类是用来相当一个子窗口，可以自行处理内部控件。与子窗口不同的是，
			此类中有滚动条。另外之所以从CSkinWindow继承，主要是因为有Edit这样
			的控件。
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

//自绘播放列表模式控件
#define CUSTOM_LIST_BMP_MAX_NUM			10            //自绘播放列表模式控件图片数目
#define CUSTOM_LIST_ICONIMG_MAX_NUM     64			  // ICON 图片数
#define CUSTOM_LIST_BK					0             //背景
#define CUSTOM_LIST_HeaderItemBk		1             //列表头背景
#define CUSTOM_LIST_HeaderSeparator		2             //列表头项分隔条
#define CUSTOM_LIST_ItemBK				3             //列表项背景
#define CUSTOM_LIST_imageItemFold		4             //列表项 展开 + - 图形

class  CCustomListCtrl:public CSkinCtrl,public IQvodCallBack
{
public:
	enum GetSpecifiedOption
	{
		FirstOne,		//第一个
		FirstEnabled,		//第一个非禁用的
		LastOne,		//最后一个
		LastEnabled,		//最后一个非禁用的
		NextOne,		//下一个
		NextEnabled,	//下一个非禁用的
		PrevOne,		//上一个
		PrevEnabled,		//上一个非禁用的
	};
	typedef struct ItemData			//列表项结构
	{
		int nID;				//节点元素的编号
		int nSeriesIdx;			//剧集集数序号
		CString str;
		Bitmap * m_Img;
		CUS_LISTITEM_STATE m_state; //列表项的状态
		item_property nProterty;
		//是否为收缩状态,否则为展开状态 --仅对ITEM_FOLDER可展开节点有效    
		BOOL bFold; 
		BOOL bHalfTrans;			//是否半透
		BOOL bChecked;				//是否已选取
		BOOL bDisable;
		Rect rtText;
		//父节点指针 --仅对ITEM_CHILD可展开节点有效           
		ItemData * pParent;
		//子节点列表--仅对ITEM_FOLDER可展开节点有效    
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

	typedef struct CustomList_Column // 列结构
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
	// 获取包括子列表项 数目
	int GetAllItemCount( );
	//获取 特定项 子列表项 数目
	int GetChildItemCount( int nItem );
	// 获取 列表列数
	int GetColumnCount();
	//Retrieves the number of items in a list view control.
	//获取选中列表项数目
	int GetSelItemCount();
	//获取选中列表项 行数目 子节点全选也统计父节点
	int GetAllSelRowCount();
	//获取启用的列表项数目
	int GetEnabledItemCount();
	//获取选中列表项数组
	BOOL GetSelItemVeter(std::vector<UINT > &vSelect);
	//获取选中列表项数组 包括子项
	BOOL GetAllSelItemVeter(std::vector< CUS_List_Item_Child_N > &vSelect);
	//获取选中列表项ID数组 包括子项
	BOOL GetAllSel_ItemID_Veter(std::vector< long > &vSelectID) ;
	//获取所有选取的列表项ID数组 包括子项
	BOOL GetAllCheckedItemVector(std::vector<CUS_List_Item_Child_N> &CheckedIDs);
	BOOL GetAllCheckedItemIDVector(std::vector<long> &CheckedIDs);
	// 获取第 nIndex个 选中列表项 ID
	long GetSelectIndexItem_ID( int nIndex = 0 );
	// 更新选中列表项 ID
	void UpdataSelectItemID();
	//获取 拖拽文件/文件夹名称的数据链
	BOOL GetDragList(std::vector<CString> & vDragList);
	// 移动整数个列表项
	BOOL AutoMoveItem( short nitem , BOOL bvertical = TRUE );
	// 让某个列表项在可见区域
	BOOL SetItemVisual(int nItem ,int nChildItem = -1 );
	//设置列表项状态
	BOOL SetItemState(int nItem,CUS_LISTITEM_STATE state , int nChildItem = -1 ,BOOL bAllChild = FALSE, BOOL bIgnoreDisabled = FALSE);
	// 设置列表项图标
	BOOL SetItemIcon(  int nItem , CString & filename  ,int nChildItem = -1);
	//设置列表项 图片
	BOOL SetItemBitmap(int nItem , Bitmap * bmp , int nChildItem = -1, int nColumn = 0  );
	//设置是否 显示图标
	void SetShowItemIcon( BOOL bshow );
	//设置列表项半透
	BOOL SetItemHalfTrans(BOOL bHalfTrans, int nItem , int nChildItem = -1);
	//设置所有列表项 为特定状态
	BOOL SetAllItemState( CUS_LISTITEM_STATE state, BOOL bIgnoreDisabled = FALSE );
	//设置所有列表项选定状态
	BOOL SetAllItemChecked(BOOL bChecked, BOOL bIgnoreDisabled = FALSE);
	//获取列表项的状态
	int GetItemState( int nItem ,int nChildItem = -1);
	//获取与设置选取状态
	BOOL GetItemChecked( int nItem, int nChildItem = -1);
	BOOL SetItemChecked( int nItem, int nChildItem = -1, BOOL bChecked = TRUE);
	BOOL ToggleItemChecked( int nItem, int nChildItem = -1);
	void SetHeaderCheckState(HeaderCheckState state) { m_eHeaderCheckState = state; m_bDirty = TRUE; }
	//获取与设置是否禁用
	BOOL GetItemEnable(int nItem, int nChildItem = -1);
	BOOL SetItemEnable(int nItem, int nChildItem, BOOL bEnable);
	//获取列表项 的 节点属性  0正常 1可收展节点 2子节点
	int GetItemProterty( int nItem );
	//获取列表项 的 节点 是否收展 
	BOOL GetItemFold( int nItem );
	//设置 列表项 的 节点 是否收展 
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
	//获取列表头 状态
	int GetColumnState( int nCol ) const;
	//获取窗口句柄
	HWND GetWindow( );
	//设置 列表头 状态
	BOOL SetColumnState( int nCol ,int state ) ;
	//获取列宽度是否能被改变属性
	BOOL GetColumnbReSize( int nCol ) const ;
	//判断是否在改变列宽度的位置上，如果是 nCol 返回第几列
	BOOL CheckInColumnSize(int px,int py, int &nCol); 
	//判断是否在 列表头位置上，如果是 nCol 返回第几列
	BOOL CheckInColumnHeader(int px,int py, int &nCol); 
	// 设置列表 列的文字  、 水平对齐方式（ -1 为不变）、 宽度 （-1 为不变）
	BOOL SetColumnText( int nCol ,CString & str  ,int nWidth = -1, int hor_align = -1);
	// 设置最小列宽
	BOOL SetColumnMiniWidth( int nCol, int cx );
	BOOL SetColumnWidth( int nCol, int cx );											//Changes the width of a column in report view or list view.
	BOOL GetViewRect( LPRECT lpRect ) const;											//Retrieves the bounding rectangle of all items in the list view control. 
	COLORREF GetTextColor( ) const;														//Retrieves the text color of a list view control.
	BOOL SetTextColor( COLORREF cr );													//Sets the text color of a list view control.
	//获取 列表项 的图片指针
	Bitmap * GetItem_pBitmap( int nItem, int nSubItem , int nChildItem ); 
	CString GetItemText( int nItem, int nSubItem , int nChildItem = -1) ;								//	if nSubItem is nonzero, it retrieves the text of the subitem. 
	BOOL SetItemText( int nItem, int nSubItem, CString & lpszText , int nChildItem = -1);						//Changes the text of a list view item or subitem. 
	//设置 列表项 ID
	BOOL SetItemID  (int nItem , int ID ,int nChildItem = -1);
	//获得 列表项 ID
	int GetItemID  (int nItem ,int nChildItem = -1 );
	//通过ID获得位置
	int GetItemByID(int nID, int* pChildPos = NULL);
	//获得指定项的ID
	int GetSpecified(GetSpecifiedOption option, int nRefID/*参照项ID*/);

	//
	//Operations
	//
	int InsertItem( int nItem, LPCTSTR lpszItem ,item_property proterty = ITEM_NORMAL ,int nChildItem = -1 );										//Inserts an item into the list view control.
	// 设置属性
	BOOL SetItemProperty( int nItem , item_property proterty , int nChildItem = -1) ;
	BOOL DeleteItem( int nItem, int nChildItem = -1);														//Deletes an item from the control.
	BOOL DeleteAllItems( );																//Deletes all items from the control.
	BOOL Scroll( CSize size );															
	// 				size		A CSize object specifying the amount of horizontal and vertical scrolling, in pixels. The y member of size is divided by the height, in pixels, of the list view control’s line, and the control is scrolled by the resulting number of lines.
	// 				Remarks		Scrolls the content of a list view control.
	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nWidth = -1 ,BOOL b_Resize=TRUE,BOOL b_sort = TRUE, int nFormat = LVCFMT_LEFT,int nSubItem = -1);
	// 				nCol					The index of the new column.
	// 				lpszColumnHeading		Address of a string containing the column’s heading.
	// 				nFormat					Integer specifying the alignment of the column. It can be one of these values: LVCFMT_LEFT, LVCFMT_RIGHT, or LVCFMT_CENTER.
	// 				nWidth					Width of the column, in pixels. If this parameter is -1, the column width is not set.
	// 				nSubItem				Index of the subitem associated with the column. If this parameter is -1, no subitem is associatied with the column.
	// 				Remarks
	// 				Inserts a new column in a list view control.

	//设置列表 列的对齐方式
	BOOL SetColumnAlign ( int nCol ,CUS_Alignment hor_align = CUS_Align_left );
	BOOL SetColumnAlign ( int nCol ,int hor_align = 0 );

	BOOL DeleteColumn( int nCol );														//Deletes a column from the list view control.
	// 删除所有列，只留下一列空白列
	BOOL DeleteAllColumns() ;

	//移动n项Item到指定位置下
	BOOL MoveItem(std::vector<UINT>& vIndex, int nPos);

	BOOL EditItem(int nItem, int nCol, int nChildItem = -1);

	BOOL OverEdit(BOOL bModify);

	CSkinEdit* CreateEdit();

	ItemData* GetItemData(int nItem, int nCol, int nChildItem = -1);
	//
	//内部实现功能组
	//
	virtual BOOL OnKey(WORD nChar);											//键盘按下

	virtual BOOL Init_(CXMLElement &xmlNode, IQvodCallBack *pEventEntry);	//初始化--一般实现子控件创建,资源加载等
	virtual void Clear_();													//反初始化
	virtual long EventNotify_(UINT uMsg, long wparam, long lparam);			//消息回调--对子控件创建时候传父类obj,让它实现的时候能逐层回溯
	virtual long Method_(LPCTSTR szAttrib, long para1, long para2);			//函数接口--类似COM查询接口,
	virtual BOOL OnTimer();													//自动调用定时器--查看是否需要重绘
	virtual BOOL OnMouseMoveIn(short x, short y);							//鼠标处理消息-移进控件
	virtual BOOL OnMouseMoveOut();											//鼠标处理消息-移出控件
	virtual BOOL OnMouseDown(short x, short y);								//鼠标处理消息-按下
	virtual BOOL OnMouseUp(short x, short y);								//鼠标处理消息-释放
	virtual BOOL OnMouseDrag(short x, short y);								//鼠标处理消息-拖动
	virtual BOOL OnMouseMove(short x, short y);								//鼠标处理消息-移动
	virtual BOOL OnRButtonUp(short x, short y);								//鼠标处理消息-右键释放
	virtual BOOL OnRButtonDown(short x, short y);							//鼠标处理消息-右键按下
	virtual BOOL OnDblClick(short x, short y);								//鼠标处理消息-左键双击
	virtual BOOL OnWheel(short nStep);										//鼠标处理消息-滚轮滚动
	virtual	long OnDrag(UINT uMsg, long wparam, long lparam);				//拖动消息处理
	virtual BOOL Layout(short nWidth, short nHeight);						//陈列展现控件--该函数由改变窗口大小位置的时候出发WM_SIZE

	BOOL CheckCursor(short x, short y);										//检测坐标是否在控件内

	//判断是否在控件内容区域（不包括垂直、水平滚动条）
	BOOL CheckMouseInContent(short x, short y);
	//获取控件内容区域（不包括垂直、水平滚动条、不包括列表头）
	Rect GetContentRect();
	//判断是否鼠标按下
	BOOL IsMouseDown();
	//检查鼠标在哪个列表项上悬浮
	BOOL CheckMouseInItemHover(short x,short y);
	//检查鼠标在哪个列表项上按下
	int  CheckMouseInItemDown(short x,short y);
	//检查某点在哪个列表项上
	int  CheckPointInItem(short x,short y);
	//检查某点在哪个 子列表项 上
	BOOL CheckPointInChildItem(short x, short y , int & nItem ,int & nChildItem);	
	//检查 框选 起终点 在哪个 子列表项 上
	BOOL CheckDragPtIn(short x, short y , int & nItem ,int & nChildItem);
	//检查拖动点在哪个列表项上 四舍五入
	BOOL  CheckDragPointInItem(short x,short y ,int & nItem ,int & nChildItem);
	//判断某点是否在某一项的内容上（文字/图标..）
	BOOL IsMouseInItemContent(int x, int y);
	//框选处理
	BOOL OnMouseDragSelect(short x, short y);
	//鼠标拖拽移动 列表项
	BOOL MouseDragMoveItem( short x, short y);
	//拖动绘制处理
	void DoDragDrop(int xDragImgMove, int yDragImgMove);
	//处理 Drag 引起的 列表内容滚动到适当位置
	BOOL DragMoveList( short x, short y );

	void Paint(Graphics *pGraph);											//绘制控件

	void DrawCheckBox( Graphics * gra );		//绘制CheckBox相关
	//画拖动时的图片
	void DrawDragBmp(HBITMAP & hBmp);
	void DrawItem_byDrag(Graphics *pGraph, int nItem, int &nOrder);
	//绘制某一个列表头
	void DrawColumn(Graphics *pGraph, int index);
	//绘制一列的列表项
	void DrawItembyColumn( Graphics *pGraph, int index );						


	//重新设置scroll控件参数
	void RefreshScrollCtrl();

	void LoadSkinConfigFromXML(CXMLElement &xmlNode);						//从XML文件载入皮肤元素的配置

	// 	BOOL ISPointInFoldBtn(int x, int y, int nItem);							//判断点是否在展开按钮上
	// 	BOOL ISPointInFolderItem(int x, int y);									//判断点是否在可展开item上
	// 
	// 	void FoldItem(int nItem);												//折叠某一项item
	// 	void UnfoldItem(int nItem);												//拓展某一项item
	// 	BOOL IsFoldItem(int nItem);												//判断某项是否为可展开项
	// 
	// 	void Sort_by_Column(int index);											//排序--根据第 index列

	Rect GetWindowRect() ;
	void SetRedraw(BOOL b);													//设置图像更新时是否绘制控件
	void SetDragSort(BOOL bDragSort);
	void SetCheckBoxEnable(BOOL bEnable);				// 启/禁 用checkbox
	CCustomListCtrl::BackgroundType GetBackgroundType() const { return m_eBackgroundType; }
	void SetBackgroundType(CCustomListCtrl::BackgroundType val) { m_eBackgroundType = val; }
	BOOL EnsureItemShownOnTop(int nItem, int nChildItem = -1);

	void SetListHalfTrans(BOOL bListHalfTrans) { m_bListHalfTrans = bListHalfTrans ; } ;	//Add by chenzhibiao

	// DragDrop相关
	virtual BOOL DragMouseIn(short x, short y);
	virtual BOOL DragMouseOut();
	virtual BOOL DragMouseMove(short x, short y);
	virtual BOOL DragMouseUp(short x, short y);
private:
	CRITICAL_SECTION m_rsLock;										//资源锁-用于防止多线程中删除操作与绘制过程的冲突
	CUSTOM_LIST_MSG  m_MSG;											//用于 向父类发触发消息,包含多种信息
	std::vector<CustomList_Column *> m_Column;						// 列结构数组
	std::vector<short > m_vdragFrameSelect;							// 框选选中列表项数组
	std::vector<CUS_List_Item_Child_N > m_vSelectItem_Child_N;	
	std::vector<long > m_vSelectItemID;	
	Bitmap * m_aImg[CUSTOM_LIST_BMP_MAX_NUM];						//存储各个状态的绘制位图
	Bitmap * m_IconImg[CUSTOM_LIST_ICONIMG_MAX_NUM];				//存储各个 ICON 绘制位图
	int  m_nColumnPicWidth ;										//列表头 每个状态图 宽度
	BOOL m_bDrag;													//鼠标在拖动时为 TRUE
	//框选是否有效
	BOOL m_bDragFrameValid;
	BOOL m_bDlclick;
	//拖拽移动 的 列表项
	int m_ndragMoveItem;
	//现在悬浮在列表头 列
	int m_nHV_Column ;
	int m_ndragMoveChildI;
	//是否拖拽移动列表项
	BOOL m_bDragMoveItem;
	BOOL m_bShowDragSep;
	HBITMAP				 m_hDragBitmap;								//用以保存拖动的位图
	CSkinScroll * m_pScroll;											//垂直滚动条
	int       m_nScrollWidth;											//垂直滚动条宽度
	BOOL m_bScrolldown;													//垂直滚动条Scroll down
	BOOL m_bHorScrolldown;												//水平滚动条Scroll down
	BOOL m_bInscroll;													//鼠标是否在垂直scroll上
	CSkinScroll * m_pHorScroll;											//水平滚动条
	CSkinEdit* m_pEdit;													//编辑框
	//垂直滚动条高度
	int       m_nHorScrollHeight;	
	BOOL m_bInHorScroll;													//鼠标是否在水平scroll上

	BOOL m_bRedraw;														//图像更新时是否绘制控件
	BOOL m_bColomSizeDown;												//列表头改变宽度位置是否按下
	// 是否正在排序 
	BOOL m_bSorting ;
	// 排序前的列表项数
	int m_nSortItemCount;
	BOOL m_bLButtonDown;
	//是否 删除其它已选列表项
	BOOL m_bDelOtherSel;
	//鼠标是否在 选中的列表项上
	BOOL m_bMouseInSelectItem;
	BOOL m_bEditShow;												//编辑框是否在显示
	BOOL m_bClickTwice;
	int m_nEditCol;
	int m_nEditItem;
	int m_nEditChildItem;
	int pre_HV_Column ;
	//鼠标在 选中列表项的项数
	int m_nMouseInSelectItem;
	int m_nMouseInSelectChildItem ;
	//是否可以 拖拽 列表项
	//CDragDropCtrl		 m_DragDropCtrl;							//拖放控件
	BOOL m_bEnableDragItem;
	// 从配置文件里读取是否 需要 拖拽 移动 列表项功能
	BOOL m_bNeedDragMoveItem ;
	HCURSOR hCurColoumSize;                                             //列表头改变宽度位置鼠标形状
	int  m_nColSize;													//鼠标在列表头改变宽度位置 的列数
	CString m_headerFace;												//列表头字体
	CString m_itemFace;													//列表项字体
	int  m_HeaderHeight;												//列表头高度
	int  m_ItemHeight;													//列表项高度
	int  m_headerSize;													//列表头字体大小
	int  m_itemSize;													//列表项字体大小 
	//列表头 文本属性
	int  m_nheaderTextX;
	//列表竖线 宽度
	short m_nHeadSeperatorWidth;
	int  m_nheaderTextY;
	int  m_nheaderTextWidth;
	int  m_nheaderTextHeight;
	BOOL  m_bheaderTextBold;
	//列表列表项 文本属性
	int  m_nitemTextX;
	int  m_nitemTextY;
	int  m_nitemTextWidth;
	int  m_nitemTextHeight;
	BOOL  m_bitemTextBold;
	Color m_bgColor;													//背景颜色
	Color m_headerClr_NL;												//列表头字体颜色 NL
	Color m_headerClr_HV;												//列表头字体颜色 HV
	Color m_headerClr_DN;												//列表头字体颜色 DN
	Color m_itemClr_NL;													//列表项字体颜色 NL
	Color m_itemClr_HV;													//列表项字体颜色 HV
	Color m_itemClr_DN;													//列表项字体颜色 DN
	Color m_itemClr_DI;													//列表项字体颜色 DI(SABLE)
	Color m_itemBkClr_HV;												//列表项 背景颜色 HV
	//列表项 背景颜色 DN 可能有多个颜色值
	std::vector<Color> m_VtItemBkClr_NM;
	Color m_itemBkClr_DN ;
	std::vector<SolidBrush *> m_VtItem_NM_Brush;												
	// 列表项之间间隙
	int m_nitemVerSapce;
	// 框选框背景色
	Color m_DragFrameBkClr;
	// 框选框的边的颜色
	Color m_DragBorderClr;
	SolidBrush * m_lpDragFrameBrush;
	Pen *  m_lpDragBorderPen;
	// 框选框的边的宽度
	int m_DragBorderWidth;
	// 拖拽 移动列表项时出现的提示插入位置的 分隔条颜色
	Color m_itemDragSepClr;
	// 拖拽 移动列表项时出现的提示插入位置的 分隔条高度
	int m_itemDragSepHeight;
	// 拖动时的图片 背景颜色
	Color m_Item_byDragBkClr;
	// 拖动时的图片 字体颜色
	Color m_Item_byDragTextClr;
	Rect m_rtDragFrame;												//鼠标拖选时候绘制的框!
	CPoint m_ptDragStart;										    //记录鼠标拖选时候绘制的按下起点!
	CPoint m_ptDragEnd;												 //记录鼠标拖选时候绘制的到达终点!

	SolidBrush *lpbushItemText_NL,*lpbushItemText_HV,*lpbushItemText_DN, *lpbushItemText_DI,
		*lpbushHeaderText_NL,*lpbushHeaderText_HV,*lpbushHeaderText_DN;
	Font * m_lpHeaderFont,* m_lpItemFont;


	long m_lPageViewPos;											//当前显示页面相对总顶部的偏移(Scroll的偏移量)
	long m_nTotalHeight;											//Item的总高
	long m_nTotalWidth;												//Item的总宽
	Rect  m_ListRect;												//列表区域
	int m_nPerPageRowNum;											//每页最多可容纳的行数
	int m_nPerPageColNum;											//每页最多可容纳的列数
	int m_nTotalRowNum;												//总共占据的行数
	int m_nTopViewRow;												//滚动页中,最顶部的行数  

	Bitmap  *m_pMemBmp;											//内存位图
	Rect	 m_paintRect;										//绘制图像的矩形区域-控件大小
	float    m_scroll_dx;										//水平滚动条滚动位移
	float    m_scroll_dy;										//垂直滚动条滚动位移
	BOOL   bSetTimer ;
	// 展开图 在列表项 中的矩形
	Rect foldRect;
	CSkinButton*	m_pTranscodeBtn;										//转码按钮
	Bitmap*			m_pTrascodeBmp;									//转码按钮图片
	BOOL m_bOverTranscodeBtndown;									// 转码按钮是否按下
	BOOL m_bInOverTranscodeBtn;											//鼠标是否在浮动转码按钮上
	int nItemHV , nChildHV ;
	// 文件图标 的 显示矩形
	Rect m_rectIcon;
	BOOL m_bShowIcon ;
	int m_nPreSelItem  ,m_nPreSelChild ;                        //最近一次选中的 列表项
	int m_nClickItem , m_nClickChildItem ;						//最近一次单击的 列表项
	int nFirstItem , nFirstChild ;
	int nLastItem , nLastChild ;
	int m_nHoverItem;											//鼠标所在项
	BOOL m_bDragSort;											//拖动是否触发排序
	std::vector<CString> m_vtDragList;							//拖拽文件/文件夹名称的数据链
	BOOL m_bEnableCheckBox;
	INT m_nXOffset;
	Gdiplus::Bitmap *m_pCheckImage;
	Rect m_rectHeadChkBox;
	HeaderCheckVisualBoxState m_eHeaderChkboxVisualState;
	HeaderCheckState m_eHeaderCheckState;
	BackgroundType m_eBackgroundType;

	BOOL m_bListHalfTrans ;										//Add by chenzhibiao 列表半透效果，每次刷新要全部刷
};
/*
class CSkinDialog: public CDialogImpl<CSkinDialog>, public CSkinCtrl, public IQvodCallBack	// 对话框
{
public:
long EventNotify_(UINT uMsg, long wparam, long lparam);
};
*/


#endif