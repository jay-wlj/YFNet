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
	Insert_msg_check		   = 0x200, // 检测播放器状态
	Insert_msg_Login           = 0x201, // 登录
	Insert_msg_ShowVipAD	   = 0x202, // 非VIP用户开启云加速显示vipad窗口
	Insert_msg_isprivate	   = 0x203, // 判断文件是否是在私人列表
	Insert_msg_addtoprivate    = 0x204, // 添加文件到私人列表
	Insert_msg_showpstronghint = 0x205, // 显示私人强提示
	Insert_msg_capture         = 0x206, // 插件截图
	Insert_msg_speed		   = 0x207, // 开启加速
	Insert_msg_getspeedswitch  = 0x208, // 获取加速开关
	ThirdControl			   = 0x300,
#ifdef RDSP360
	Insert_msg_playlock		   ,
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
	I_CaptureType type;		 // 截取类型
	bool	 bFull;			 // 插件是否全屏
	bool	 bNet;
	bool     bBit;
	HWND	 hInsert;
	wchar_t  wzSrcPath[260];
	wchar_t  wzPath[260];
};

//////////////////////////////////////////////////////////////////////////
// 播放发出消息的相关定义
// 发送 WM_USER_PLAYER_TO_INSERT(WM_USER + 0x121) 消息，WPARAM参数为消息类型
enum Player_msg
{
	Player_msg_hasnetaddtoprivate,	  // 有网络文件添加到私人列表
	Player_msg_logoutprivate,		  // 退出私人列表
	Player_msg_speed,
};