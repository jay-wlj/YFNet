// 摘    要：		
//			
//
// 当前版本：
// 作    者：陈志标
// 完成日期：
#pragma once


//////////////////////////////////////////////////////////////////////////
// 插件发出消息的相关定义
// 发送WM_COPYDATA消息，WPARAM参数为消息类型

enum barrage_msg
{
	//弹幕评论插件发来的消息
	barrage_msg_setbarragewndsz  = 0x500, //插件设置弹幕评论窗口大小
	barrage_msg_showloginwnd		= 0x501, //显示登录窗口
	barrage_msg_playfile			= 0x502, //播放文件
};

//弹幕评论数据定义
struct tagQvodInfo
{
	TCHAR szUserID[10]	;
	TCHAR szHash[41]	;
};
//播放文件
struct tagBarrageFile
{
	TCHAR		bstr_url[MAX_PATH];
	LONGLONG	film_pos	;
	TCHAR		bstr_content[MAX_PATH] ;
	BOOL		bIsThisFilm;
};