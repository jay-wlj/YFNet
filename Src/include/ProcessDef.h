// 摘    要：		
//			
//
// 当前版本：
// 作    者：匡晓林
// 完成日期：
#pragma once


//////////////////////////////////////////////////////////////////////////
// 插件发出消息的相关定义
// 发送WM_COPYDATA消息，WPARAM参数为消息类型

enum Insert_msg
{
	//Add by chenzhibiao 2013-8-13 begin
	Insert_msg_updatelist	    = 0x10,  // 更新播放列表
	Insert_msg_addfile			= 0x20,  // 添加文件或文件夹到某个列表中
	Insert_msg_popuptipswnd	    = 0x30,  // 弹出搜索提示框
	Insert_msg_showfoladwnd	    = 0x40,  // 显示浮动窗口
	Insert_msg_notifytraytip	= 0x50,  // 通知托盘提示
	Insert_msg_addcloudfile		= 0x60,  // 添加云文件
	//Add by chenzhibiao 2013-8-13 end

	Insert_msg_check			= 0x200, // 检测播放器状态
	Insert_msg_Login			= 0x201, // 登录
	Insert_msg_ShowVipAD		= 0x202, // 非VIP用户开启云加速显示vipad窗口
	Insert_msg_isprivate		= 0x203, // 判断文件是否是在私人列表
	Insert_msg_addtoprivate		= 0x204, // 添加文件到私人列表
	Insert_msg_showpstronghint	= 0x205, // 显示私人强提示
	Insert_msg_capture			= 0x206, // 插件截图
	Insert_msg_speed			= 0x207, // 开启加速
	Insert_msg_getspeedswitch	= 0x208, // 获取加速开关
	Insert_msg_qmvdata			= 0x209, // 更新qmv数据
	Insert_msg_qmvurl_clicked	= 0x20a, // 插件中qmv标签被点击
	Insert_msg_isEnableFilterAd = 0x20b, // 是否屏蔽广告
	Insert_msg_playfile_sucess	= 0x20c, // 插件播放了网络文件
	Insert_msg_notify_exit		= 0x20d, // 通知播放器插件要退出了
	Insert_msg_notify_playtime  = 0x20e, // 通知播放器文件播放的位置
	Insert_msg_play				= 0x20f, // 播放	
	Insert_msg_TextAD			= 0x210, // 文字链双URL配置
	Insert_msg_kuaibo_openurl	= 0x211,
	ThirdControl				= 0x300,
#ifdef RDSP360
	Insert_msg_playlock,
#endif
#ifdef RDSP
	Insert_msg_updatetostdver,
#endif
};

enum I_CaptureType		// 插件截取类型
{
	I_CaptureType_img,
};

struct InsertCaptureData
{
	I_CaptureType	type;		// 截取类型
	bool			bFull;		// 插件是否全屏
	bool			bNet;
	bool			bBit;
	HWND			hInsert;
	wchar_t			wzSrcPath[260];
	wchar_t			wzPath[260];
};

struct InsertPlaytimeData
{
	char			szHash[41];	//当前播放文件hash
	long			lplaytime;		//当前播放位置，<-1，已播放完（单位ms)
};

//////////////////////////////////////////////////////////////////////////
// Insert_msg_play 相关
// Insert_msg_play 类型COPYDATASTRUCT中dwData的值 
enum 
{
	Insert_msg_play_caller_qvodweb = 1,
};

// 播放数据结构
struct QvodWebMediaUrlInfo
{
	TCHAR szURL[MAX_PATH];				// 播放链接
	TCHAR szRef[MAX_PATH];				// 引用页
	HWND hCorrelationWnd_;				// qvodplay中与qvodweb页面关联的窗口句柄
};

//////////////////////////////////////////////////////////////////////////
// 播放发出消息的相关定义
// 发送 WM_USER_PLAYER_TO_INSERT(WM_USER + 0x121) 消息，WPARAM参数为消息类型
enum Player_msg
{
	Player_msg_hasnetaddtoprivate,	  // 有网络文件添加到私人列表
	Player_msg_logoutprivate,		  // 退出私人列表
	Player_msg_speed,
	Player_msg_EnableFilterAd,		  // 
};