#ifndef _QVOD_UI_DEFINE_H_
#define _QVOD_UI_DEFINE_H_

#include "CallbackDefine.h"
#include "ComCmd.h"

#define DYNAMIC_LOAD
#define MULIT_IMAGE
#define PREVIEW_W 170
#define PREVIEW_H 127
//	QCM_HOTKEY_MOVELEFT,
// 	QCM_SEARCHITEM_DELETE,
// 	QCM_SEARCHITEM_MOVEUP,
// 	QCM_SEARCHITEM_MOVEDOWN,

typedef struct _CALLBACK_UI_INFO
{
	CommonCmd cmdID;
	TCHAR szSkinID[60];
}CALLBACK_UI_INFO;

// 临时作为常量放在这里
const CALLBACK_UI_INFO g_CmdInfo[] = {
// 系统按钮
	{QCM_CLOSE, _T("close")},
	{QCM_MINIMIZE, _T("min")},
	{QCM_MAXIMIZE, _T("max")},
	{QCM_MAXIMIZE_RE, _T("max_re")},
	{QCM_SHOWMENU, _T("showmenu")},
	{QCM_OPTIONCREATE, _T("openmedia")},
	{QCM_MAINWIND, _T("playing")},

	{QCM_MIN_CLOSE, _T("min_close")},
	{QCM_MIN_MIN, _T("min_min")},

//消息框
	{QCM_MSGBOX_BTN1, _T("msgbox_btn1") },
	{QCM_MSGBOX_BTN2, _T("msgbox_btn2") },
	{QCM_MEDIA_PLAY, _T("play")},
	{QCM_MEDIA_PAUSE, _T("pause")},
	{QCM_MEDIA_STOP, _T("stop")},
	{QCM_MEDIA_PERVIOUS, _T("part_prev")},
	{QCM_MEDIA_NEXT, _T("part_next")},
	{QCM_MEDIA_POS, _T("mediaPos")},
	{QCM_VOLUME_DRAGING, _T("volume")},
	{QCM_MUTE, _T("mute")},
	{QCM_UNMUTE, _T("unmute")},
	{QCM_RECLIST, _T("revlist_btn")},
	{QCM_SHOWLIST, _T("showlist_btn")},
	{QCM_LISTNAME, _T("playlistname")},
	{QCM_LOCALLIST, _T("playlistLocal")},
	{QCM_NETLIST, _T("playlistNet")},
	{QCM_ONLINEMEDIALIST, _T("onlinemedialistname")},
	{QCM_TRIPICLISTNAME, _T("btntripic")},
	{QCM_LISTMODE,	_T("listmode")},					// 列表模式
	{QCM_SMALLLOGOMODE,	_T("logomode")},				// 缩略图模式
	{QCM_LIST_TRI, _T("tripic")},
	{QCM_LOOP, _T("loopchoose")},
	{QCM_LIST_DELETE, _T("list_delete")},					// 删除
	{QCM_LIST_ADD, _T("list_add")},						// 添加
	{QCM_LIST_SORT, _T("listsort")},
	{QCM_MLIST_OVERDEL_BTN, _T("ml_od")},
	{QCM_OPENMENU, _T("openmenu")},
	{QCM_PROTECT_EYES, _T("protect_eye")},
	{QCM_TEXT_AD, _T("textad")},
	{QCM_BTN_YES, _T("btn_yes")},
	{QCM_BTN_NO, _T("btn_no")},
	{QCM_ONLINEMEDIAAD, _T("onlinemediaad")},

	{QCM_MINI_PLAY, _T("min_play")},
	{QCM_MINI_PAUSE, _T("min_pause")},
	{QCM_MINI_STOP, _T("min_stop")},
	{QCM_MINI_MUTE, _T("min_mute")},
	{QCM_MINI_UNMUTE, _T("min_unmute")},
	{QCM_MINI_VOLUMEDRAG, _T("min_volume")},
	{QCM_MINI_OPENFILE, _T("min_openfile")},
	{QCM_MEDIA_MINI_POS, _T("min_mediaPos")},

	{QCM_FORWARD_PLAY, _T("forwardplay")},					// 变速快进
	{QCM_BACK_SPEED, _T("backplay")},						// 变速后退
	{QCM_SETPLAYPOS, _T("setplaypos")},

// 	//广告字链接
	{QCM_ADTEXT_BTN, _T("linktxt")},
	{QCM_PLAYNOTE_BTN, _T("playnote")},
	{QCM_PLAYNOTE_BTN_PLAYING, _T("playnote_play")},

//	{QCM_SEARCHONNET_BTN, _T("searchonnet")},
//	{QCM_MANAGESEARCH_BTN, _T("managestbtn")},

// 	{QCM_SEARCHITEM_ADD, _T("msw_add_btn")},
// 	{QCM_SEARCHITEM_EDIT, _T("msw_edit_btn")},
// 	{QCM_SEARCHITEM_DELETE, _T("msw_delete_btn")},
// 	{QCM_SEARCHITEM_MOVEUP, _T("msw_upmove_btn")},
// 	{QCM_SEARCHITEM_MOVEDOWN, _T("msw_downmove_btn")},
// 	{QCM_ENCODE_DEFAULT, _T("msw_code1")},
// 	{QCM_ENCODE_UTF8, _T("msw_code2")},
// 	{QCM_ENCODE_GB, _T("msw_code3")},

	//视频区域
	{QCM_WND_HALFX, _T("fb_halfx")},					// 0.5倍大小
	{QCM_WND_1X, _T("fb_min_1x")},
	{QCM_WND_1X, _T("fb_1x")},							// 1倍大小
	{QCM_WND_2X, _T("fb_2x")},						// 2倍大小
	{QCM_WND_FULL, _T("fb_full")},						// 全屏
	{QCM_WND_NOFULL, _T("fb_Nofull")},					//退出全屏
	{QCM_WND_MINI, _T("fb_mini")},						// 迷你模式
	{QCM_WND_RESTOR, _T("fb_restor")},					// 还原
	{QCM_WND_TOPMOST, _T("fb_fore")},					// 前端
	{QCM_WND_PLAYTOPMOST, _T("fb_fore_top")},
	{QCM_WND_NOTOPMOST, _T("fb_fore_playtop")},		// 播放置顶
//	{QCM_VIDEOFRAME, _T("fb_frame")},					// 视频框架
//	{QCM_BRIGHT_UP, _T("fb_lightA")},					//亮度变亮
//	{QCM_BRIGHT_DOWN, _T("fb_lightD")},					//亮度变暗
//	{QCM_FLIP, _T("fb_turn")},						//翻转
//	{QCM_SOUND, _T("fb_channel")},						//声道
	{QCM_EFFECT_WND, _T("fb_moreeffect")},				//更多调节
	{QCM_CATPURE, _T("fb_screenshot")},					//截屏
	{QCM_VIDEOENHANCE, _T("fb_enhance")},				// 视频增强
	{QCM_VIDEOENHANCE_CLOSE, _T("fb_enhance_close")},
//	{QCM_ADVANCED, _T("fb_ext_on")},					//打开高级
	{QCM_CLOSE_AD, _T("closeAd")},						// 关闭广告按钮
	{QCM_CLICKTOURL, _T("task_url_txt")},
	
#ifndef RDSP
	//视频截取窗口
	{QCM_CUT_SAVE,_T("cutsave")},						// 视频保存
	{QCM_CUT_PREVIEW,_T("cutpreview")},					// 视频预览
	{QCM_LEFTMOVE_L,_T("cut_left_l")},						// 微调左边左移
	{QCM_LEFTMOVE_R,_T("cut_left_r")},						// 微调左边右移
	{QCM_RIGHTMOVE_L,_T("cut_right_l")},					// 微调右边左移
	{QCM_RIGHTMOVE_R,_T("cut_right_r")},					// 微调右边右移
	{QCM_CUT_CLOSE,_T("cut_close")},						// 结束视频调节
	{QCM_CUT_SLIDER,_T("cutpos")},						// 截取进度

	//GIF截取窗口
	{QCM_GIF_CUT_SAVE,_T("gif_cutsave")},						// 视频保存
	{QCM_GIF_CUT_PREVIEW,_T("gif_cutpreview")},					// 视频预览
	{QCM_GIF_LEFTMOVE_L,_T("gif_cut_left_l")},						// 微调左边左移
	{QCM_GIF_LEFTMOVE_R,_T("gif_cut_left_r")},						// 微调左边右移
	{QCM_GIF_RIGHTMOVE_L,_T("gif_cut_right_l")},					// 微调右边左移
	{QCM_GIF_RIGHTMOVE_R,_T("gif_cut_right_r")},					// 微调右边右移
	{QCM_GIF_CUT_CLOSE,_T("gif_cut_close")},						// 结束视频调节
	{QCM_GIF_CUT_SLIDER_BEGIN,_T("gif_cut_slider_begin")},			// 截取起点进度
	{QCM_GIF_CUT_SLIDER_END,_T("gif_cut_slider_end")},			// 截取起点进度
	{QCM_GIF_SIZE_BTN, _T("gif_cut_size_btn")},
#endif

	// 播放列表 tab
	{QCM_SHOW_TASK, _T("tabTasklist")},
//	{QCM_TASK_MENU, _T("popmenuTask")},
	{QCM_SHOW_PL, _T("tabFilelist")},
	{QCM_PL_MENU, _T("popmenuFile")},
	{QCM_SUBWND_SIZE, _T("wndsplitter")},
#pragma region 选项
// 选项
	{QCM_OPTION_TAB, _T("option")},
	{QCM_OPTION_GENERAL, _T("general")},
	{QCM_OPTION_PLAY, _T("option_play")},
	{QCM_OPTION_NETWORK, _T("network")},
	{QCM_OPTION_SHORTCUT, _T("shortcut")},	
	{QCM_OPTION_MEDIA, _T("media")},	
	{QCM_OPTION_OTHER, _T("op_other")},	
	{QCM_OPTION_LYRIC, _T("lyric")},	
	{QCM_OPTION_UPDATE, _T("update")},
	{QCM_OPTION_USERSET, _T("op_userset")},
	{QCM_OPTION_OK, _T("option_ok")},
	{QCM_OPTION_CANCLE, _T("Option_cancle")},
	{QCM_OPTION_NEWDAILY, _T("newdaily")},
	{QCM_OPTION_NAVAGATIONWEB, _T("navigationWeb")},
	{QCM_OPTION_SETLOCALMODE, _T("startUpWithLocalMode")},
	{QCM_OPTION_RUNQVODBOX, _T("general_qvodbox")},
	{QCM_OPTION_VTAGON, _T("vtagon")},
	{QCM_OPTION_IMGWALLON, _T("imgwallon")},
	{QCM_OPTION_RUNBHO, _T("general_bho")},
	{QCM_OPTION_RUNTIPS,_T("general_autotips")},
	{QCM_OPTION_WEBSITEON,_T("websiteon")},

	{QCM_OPTION_CHANGGUI, _T("col5_changgui")}, 
	{QCM_OPTION_FILE_AVI, _T("col5_changgui_avi")},
	{QCM_OPTION_FILE_MKV, _T("col5_changgui_mkv")},
	{QCM_OPTION_FILE_OGM, _T("col5_changgui_ogm")},
	{QCM_OPTION_FILE_MP4, _T("col5_changgui_mp4")},
	{QCM_OPTION_FILE_M4P, _T("col5_changgui_m4p")},
	{QCM_OPTION_FILE_M4B, _T("col5_changgui_m4b")},

	{QCM_OPTION_MPEG, _T("col5_mpeg")},
	{QCM_OPTION_FILE_DAT, _T("col5_mpeg_dat")},
	{QCM_OPTION_FILE_MPG, _T("col5_mpeg_mpg")},
	{QCM_OPTION_FILE_MPEG, _T("col5_mpeg_mpeg")},
	{QCM_OPTION_FILE_TP, _T("col5_mpeg_tp")},
	{QCM_OPTION_FILE_TS, _T("col5_mpeg_ts")},
	{QCM_OPTION_FILE_TPR, _T("col5_mpeg_tpr")},
	{QCM_OPTION_FILE_PVA, _T("col5_mpeg_pva")},
	{QCM_OPTION_FILE_PSS, _T("col5_mpeg_pss")},
	{QCM_OPTION_FILE_VOB, _T("col5_mpeg_vob")},
	{QCM_OPTION_FILE_MPE, _T("col5_mpeg_mpe")},
	{QCM_OPTION_FILE_WV, _T("col5_mpeg_wv")},
	{QCM_OPTION_FILE_M2TS, _T("col5_mpeg_m2ts")},
	{QCM_OPTION_FILE_M2T, _T("col5_mpeg_m2t")},
	{QCM_OPTION_FILE_EVO, _T("col5_mpeg_evo")},
	{QCM_OPTION_FILE_DVD, _T("col5_mpeg_dvd")},

	{QCM_OPTION_REAL, _T("col5_real")},
	{QCM_OPTION_FILE_RAM, _T("col5_real_ram")},
	{QCM_OPTION_FILE_RM, _T("col5_real_rm")},
	{QCM_OPTION_FILE_RMVB, _T("col5_real_rmvb")},
	{QCM_OPTION_FILE_RP, _T("col5_real_rp")},
	{QCM_OPTION_FILE_RPM, _T("col5_real_rpm")},
	{QCM_OPTION_FILE_RSC, _T("col5_real_rsc")},
	{QCM_OPTION_FILE_RT, _T("col5_real_rt")},
	{QCM_OPTION_FILE_SMIL, _T("col5_real_smil")},

	{QCM_OPTION_QUICKTIM, _T("col5_qtime")},
	{QCM_OPTION_FILE_AIF, _T("col5_qtime_aif")},
	{QCM_OPTION_FILE_AIFF, _T("col5_qtime_aiff")},
	{QCM_OPTION_FILE_AIFC, _T("col5_qtime_aifc")},
	{QCM_OPTION_FILE_AMR, _T("col5_qtime_amr")},
	{QCM_OPTION_FILE_3GP, _T("col5_qtime_3gp")},
	{QCM_OPTION_FILE_3G2, _T("col5_qtime_3g2")},
	{QCM_OPTION_FILE_3GP2, _T("col5_qtime_3gp2")},
	{QCM_OPTION_FILE_3GPP, _T("col5_qtime_3gpp")},
	{QCM_OPTION_FILE_QT, _T("col5_qtime_qt")},
	{QCM_OPTION_FILE_MOV, _T("col5_qtime_mov")},

	{QCM_OPTION_WINDOWSMEDIA, _T("col5_windows")},
	{QCM_OPTION_FILE_WMV, _T("col5_windows_wmv")},
	{QCM_OPTION_FILE_WMP, _T("col5_windows_wmp")},
	{QCM_OPTION_FILE_WMA, _T("col5_windows_wma")},
	{QCM_OPTION_FILE_ASF, _T("col5_windows_asf")},

	{QCM_OPTION_QTVIDEO, _T("col5_qt")},
	{QCM_OPTION_FILE_AMV, _T("col5_qt_amv")},
	{QCM_OPTION_FILE_DSA, _T("col5_qt_dsa")},
	{QCM_OPTION_FILE_DSV, _T("col5_qt_dsv")},
	{QCM_OPTION_FILE_DSS, _T("col5_qt_dss")},
	{QCM_OPTION_FILE_M1V, _T("col5_qt_m1v")},
	{QCM_OPTION_FILE_M2V, _T("col5_qt_m2v")},
	{QCM_OPTION_FILE_M2P, _T("col5_qt_m2p")},
	{QCM_OPTION_FILE_MP2, _T("col5_qt_mp2")},
	{QCM_OPTION_FILE_PMP, _T("col5_qt_pmp")},
	{QCM_OPTION_FILE_SMK, _T("col5_qt_smk")},
	{QCM_OPTION_FILE_BIK, _T("col5_qt_bik")},
	{QCM_OPTION_FILE_RAT, _T("col5_qt_rat")},
	{QCM_OPTION_FILE_VG2, _T("col5_qt_vg2")},
	{QCM_OPTION_FILE_IVF, _T("col5_qt_ivf")},
	{QCM_OPTION_FILE_VP6, _T("col5_qt_vp6")},
	{QCM_OPTION_FILE_VP7, _T("col5_qt_vp7")},
	{QCM_OPTION_FILE_D2V, _T("col5_qt_d2v")},
	{QCM_OPTION_FILE_MOD, _T("col5_qt_mod")},
	{QCM_OPTION_FILE_VID, _T("col5_qt_vid")},
	{QCM_OPTION_FILE_PMP2, _T("col5_qt_pmp2")},
	{QCM_OPTION_FILE_MTS, _T("col5_qt_mts")},
	{QCM_OPTION_FILE_FLI, _T("col5_qt_fli")},
	{QCM_OPTION_FILE_FLC, _T("col5_qt_flc")},
	{QCM_OPTION_FILE_ROQ, _T("col5_qt_roq")},
	{QCM_OPTION_FILE_TIM, _T("col5_qt_tim")},
	{QCM_OPTION_FILE_CSF, _T("col5_qt_csf")},
	{QCM_OPTION_FILE_PM2, _T("col5_qt_pm2")},
	{QCM_OPTION_FILE_SMV, _T("col5_qt_smv")},

	{QCM_OPTION_CJAUDIO, _T("col5_cjyinpin")},
	{QCM_OPTION_FILE_MP3, _T("col5_cjyinpin_mp3")},
	{QCM_OPTION_FILE_OGG, _T("col5_cjyinpin_ogg")},
	{QCM_OPTION_FILE_CDA, _T("col5_cjyinpin_cda")},
	{QCM_OPTION_FILE_WAV, _T("col5_cjyinpin_wav")},
	{QCM_OPTION_FILE_APE, _T("col5_cjyinpin_ape")},

	{QCM_OPTION_QTAUDIO, _T("col5_qtyinpin")},
	{QCM_OPTION_FILE_AU, _T("col5_qtyinpin_au")},
	{QCM_OPTION_FILE_DTS, _T("col5_qtyinpin_dts")},
	{QCM_OPTION_FILE_MIDI, _T("col5_qtyinpin_midi")},
	{QCM_OPTION_FILE_MID, _T("col5_qtyinpin_mid")},
	{QCM_OPTION_FILE_RMI, _T("col5_qtyinpin_rmi")},
	{QCM_OPTION_FILE_MP5, _T("col5_qtyinpin_mp5")},
	{QCM_OPTION_FILE_MPA, _T("col5_qtyinpin_mpa")},
	{QCM_OPTION_FILE_MPGA, _T("col5_qtyinpin_mpga")},
	{QCM_OPTION_FILE_DSM, _T("col5_qtyinpin_dsm")},
	{QCM_OPTION_FILE_FLAC, _T("col5_qtyinpin_flac")},
	{QCM_OPTION_FILE_MAC, _T("col5_qtyinpin_mac")},
	{QCM_OPTION_FILE_TTA, _T("col5_qtyinpin_tta")},
	{QCM_OPTION_FILE_M4A, _T("col5_qtyinpin_m4a")},
	{QCM_OPTION_FILE_AAC, _T("col5_qtyinpin_aac")},
	{QCM_OPTION_FILE_AC3, _T("col5_qtyinpin_ac3")},

	{QCM_OPTION_FLASH, _T("col5_shockwave")},
	{QCM_OPTION_FILE_SWF, _T("col5_shockwave_swf")},
	{QCM_OPTION_FILE_FLV, _T("col5_shockwave_flv")},
	{QCM_OPTION_FILE_M4V, _T("col5_shockwave_m4v")},
	{QCM_OPTION_FILE_F4V, _T("col5_shockwave_f4v")},

	{QCM_OPTION_PLAYFILELIST, _T("col5_bflb")},
	{QCM_OPTION_FILE_M3U, _T("col5_bflb_m3u")},
	{QCM_OPTION_FILE_WPL, _T("col5_bflb_wpl")},
	{QCM_OPTION_FILE_WMX, _T("col5_bflb_wmx")},
	{QCM_OPTION_FILE_MVX, _T("col5_bflb_mvx")},
	{QCM_OPTION_FILE_TTPL, _T("col5_bflb_ttpl")},
	{QCM_OPTION_FILE_CUE, _T("col5_bflb_cue")},
	{QCM_OPTION_FILE_QPL, _T("col5_bflb_qpl")},
	{QCM_OPTION_FILE_ASX, _T("col5_bflb_asx")},	

	{QCM_OPTION_BTFILE, _T("col5_bt")},
	{QCM_OPTION_FILE_TORRENT, _T("col5_bt_torrent")},	
	{QCM_OPTION_FILE_QSED, _T("col5_bt_qsed")},

	{QCM_OPTION_ALLCHOOSE, _T("col5_allchoose")},	
	{QCM_OPTION_KEEPCONN, _T("col5_keepconn")},
	{QCM_OPTION_REGAIN, _T("col5_originalBtn")},

	//常规
	//{QCM_OPTION_STARTRUNNET,_T("general_yunxingNet")},			
	{QCM_OPTION_NOMINTRAY,_T("general_Ntuopan")},
	{QCM_OPTION_GETLAGUAGEID,_T("general_laguage_combox")},
//	{QCM_OPTION_CLOSEEXITALL,_T("general_colseterminal")},
	{QCM_OPTION_SMARTADD, _T("general_smartadd")},
	{QCM_OPTION_GENONLINE, _T("general_online")},

	{QCM_OPTION_RESARTDELSMALLTASK,_T("general_deletetask")},
	{QCM_OPTION_DELDAYBEFORFILE,_T("general_day_combox")},
	{QCM_OPTION_LIMITDOWNLOAD,_T("general_recvcount_combox")},

	{QCM_OPTION_IMGFORMAT,_T("general_imgformat_combox")},
	{QCM_OPTION_IMGPATHBTN,_T("general_imgpath_btn")},
	{QCM_OPTION_CAPTIONPATHBTN,_T("general_zimupath_btn")},
	{QCM_OPTION_SHOWLISTINNER, _T("listshowInner")},
	{QCM_OPTION_SHOWLISTOUT, _T("listshowOut")},
	{QCM_OPTION_CLEARDEFAULTLIST, _T("cleardefaultlist")},
	{QCM_OPTION_SHOWHOTSPOT, _T("showHotspotWhenIdle")},

	//播放
	{QCM_PLAY_NOSET, _T("videoset1")},
	{QCM_PLAY_SET_HARDACC, _T("videoset2")},
	{QCM_PLAY_SET_EFFECT, _T("videoset3")},
//	{ QCM_OPTION_HDHARDACC, _T("PS_speed")},	
//	{ QCM_OPTION_VADIOADJUST, _T("PS_adjust")},
	{ QCM_OPTION_CLICKSTOP, _T("PS_click")},
	{ QCM_OPTION_LISTPLAYMEDIAFILE, _T("PS_media")},
	{ QCM_OPTION_EXITSAVEWINDOWSIZE, _T("PS_exit_size")},
	{ QCM_OPTION_PLAYERADJUSTIMG, _T("PS_auto_size")},
	{ QCM_OPTION_RECORPLAYPOS, _T("PS_recordplaypos")},
	{ QCM_OPTION_AUTODOWNSUBTITLE, _T("PS_autodown_subtitle")},
	{ QCM_OPTION_AUDIOCARD, _T("PS_com_audio")},
//	{ QCM_OPTION_XUNRAN, _T("PS_com_render")},
	{ QCM_OPTION_MINIPAUSE, _T("ps_miniPause")},

	//网络
	{ QCM_OPTION_NET_FILEPATHBTN,_T("NS_path")},
	{ QCM_OPTION_NET_UDPTRAS,_T("NS_udp")},
	{ QCM_OPTION_NET_HTTPTRAS,_T("NS_http")},
	{ QCM_OPTION_NET_DOWNPLAY,_T("NS_continue")},
	{ QCM_OPTION_NET_KEEPTERMINAL,_T("NS_keep_terminal")},
	{ QCM_OPTION_NET_SPEEDMODE,_T("NS_com_speed_mode")},
	{ QCM_OPTION_NET_CHECHSYS, _T("checknet")},
	{ QCM_OPTION_NET_SHARE_RESLIST, _T("NS_share_reslist")},

	///////////////////////////////系统检测
	{QCM_OPTION_OTHER_IMGH, _T("general_hang_combox")},
	{QCM_OPTION_OTHER_IMGL, _T("general_lie_combox")},
	{QCM_OPTION_OTHER_IMGW, _T("general_imgwidth_combox")},

	//升级
	{ QCM_OPTION_UPGRAD_AUTO, _T("col6_autoUpgrad")},
	{ QCM_OPTION_UPGRAD_NOTO,_T("col6_noteUpgrad")},
	{ QCM_OPTION_UPGRAD_NOCHECK, _T("col6_nocheckUpgrad")},

	// 快捷键
	{QCM_OPTION_BOSS, _T("co3_boss_content")},
	{QCM_OPTION_BOSSCHECK, _T("co3_boss_choosbtn")},
	{QCM_OPTION_CATCH, _T("co3_catch_content")},
	{QCM_OPTION_CATCHCHECK, _T("co3_catch_choosbtn")},

	{QCM_OPTION_BACKORIGINAL, _T("co3_originalBtn")},
	{QCM_OPTION_HOTKEY_FILE, _T("hotkey_card0")},
	{QCM_OPTION_HOTKEY_PLAY, _T("hotkey_card1")},
	{QCM_OPTION_HOTKEY_IMG, _T("hotkey_card2")},
	{QCM_OPTION_HOTKEY_SOUND, _T("hotkey_card3")},
	{QCM_OPTION_HOTKEY_SUBTITLE, _T("hotkey_card4")},
	
	//文件
	{QCM_HOTKEY_HELP, _T("edit_helpinfo")},
	{QCM_HOTKEY_OPENMEDIA, _T("edit_openMedia")},
	{QCM_HOTKEY_OPENCD, _T("edit_opencd")},
	{QCM_HOTKEY_OPENLOCALFILE, _T("edit_openlocalfile")},
	{QCM_HOTKEY_OPTION, _T("edit_option")},
	{QCM_HOTKEY_OPENNET, _T("edit_opennet")},

	//播放
	{QCM_HOTKEY_FULL, _T("edit_full")},
	{QCM_HOTKEY_EXITFULL, _T("edit_exitfull")},
	{QCM_HOTKEY_MINI, _T("edit_mini")},
	{QCM_HOTKEY_PLAYING, _T("edit_playing")},
	{QCM_HOTKEY_PLAYLIST, _T("edit_playlist")},
	{QCM_HOTKEY_DESKPLAY, _T("edit_deskplay")},
	{QCM_HOTKEY_APOINT, _T("edit_apoint")},
	{QCM_HOTKEY_BPOINT, _T("edit_bpoint")},
//	{QCM_HOTKEY_BEGINLOOP, _T("edit_beginloop")},
	{QCM_HOTKEY_ENDLOOP, _T("edit_endloop")},
	{QCM_HOTKEY_TURNTO, _T("edit_turnto")},
	{QCM_HOTKEY_PLAYPAUSE, _T("edit_playpause")},
	{QCM_HOTKEY_SPACEPAUSE, _T("edit_spacepause")},
	{QCM_HOTKEY_STOP, _T("edit_stop")},
	{QCM_HOTKEY_HEADJUMP, _T("edit_headjump")},
	{QCM_HOTKEY_BACKJUMP, _T("edit_backjump")},
	{QCM_HOTKEY_QUICKPLAY, _T("edit_quickplay")},
	{QCM_HOTKEY_SLOWPLAY, _T("edit_slowplay")},
	{QCM_HOTKEY_ASPEEDPLAY, _T("edit_aspeedplay")},
	{QCM_HOTKEY_DSPEEDPLAY, _T("edit_dspeedplay")},
	{QCM_HOTKEY_RESTORSPEED, _T("edit_restorspeed")},
	{QCM_HOTKEY_UPONE, _T("edit_upone")},
	{QCM_HOTKEY_NEXTONE, _T("edit_nextone")},
	{QCM_HOTKEY_VIDEOADJUST, _T("edit_videoadjust")},
	{QCM_HOTKEY_CLOSEPLAYER, _T("edit_closeplayer")},
	{QCM_HOTKEY_TOPSHOW, _T("edit_topshow")},

	//图像
	{QCM_HOTKEY_UPFRAME, _T("edit_upframe")},
	{QCM_HOTKEY_NEXTFRAME, _T("edit_nextframe")},
	{QCM_HOTKEY_CUTSCREEN, _T("edit_cutscreen")},
	{QCM_HOTKEY_MODE_50, _T("edit_mode50")},
	{QCM_HOTKEY_MODE_100, _T("edit_mode100")},
	{QCM_HOTKEY_MODE_200, _T("edit_mode200")},
//	{QCM_HOTKEY_INSIZE, _T("edit_insize")},
//	{QCM_HOTKEY_DESIZE, _T("edit_desize")},
//	{QCM_HOTKEY_WIDER, _T("edit_wider")},
//	{QCM_HOTKEY_NARROWER, _T("edit_narrower")},
//	{QCM_HOTKEY_HIGHER, _T("edit_higher")},
//	{QCM_HOTKEY_LOWER, _T("edit_lower")},
//	{QCM_HOTKEY_MOVELEFT, _T("edit_moveleft")},
//	{QCM_HOTKEY_MOVERIGHT, _T("edit_moveright")},
//	{QCM_HOTKEY_MOVEUP, _T("edit_moveup")},
//	{QCM_HOTKEY_MOVEDOWN, _T("edit_movedown")},
//	{QCM_HOTKEY_RESTORPOS, _T("edit_restorpos")},

	//声音
	{QCM_HOTKEY_LOWVOL, _T("edit_lowvol")},
	{QCM_HOTKEY_HIVOL, _T("edit_hivol")},
	{QCM_HOTKEY_MUTE, _T("edit_mute")},
	{QCM_HOTKEY_SOUNDBAN, _T("edit_soundban")},
	{QCM_HOTKEY_SOUNDADJUST, _T("edit_soundadjust")},

	//字幕
	{QCM_HOTKEY_SUBTITLE, _T("edit_subtitlead")},


	//歌词
	{QCM_OPTION_ADDLYRIC, _T("LW_lyric")},
	{QCM_OPTION_LYRIC_FONT, _T("SL_tb_font")},
	{QCM_OPTION_LYRIC_BKCOLOR, _T("SL_btn_bk")},
	{QCM_OPTION_LYRIC_NORMALCOLOR, _T("SL_btn_normal")},
	{QCM_OPTION_LYRIC_LIGHTCOLOR, _T("SL_btn_light")},
	{QCM_OPTION_LYRIC_FOLD_ADD, _T("LC_btn_add")},
	{QCM_OPTION_LYRIC_FOLDER_DECREASE, _T("LC_btn_decrease")},
	{QCM_OPTION_LYRIC_IMAGE_ADD, _T("LI_btn_add")},
	{QCM_OPTION_LYRIC_IMAGE_DECREASE, _T("LI_btn_decrease")},
	{QCM_OPTION_LYRIC_SCROLLMODE, _T("SL_com_roll_mode")},//滚动方式
	{QCM_OPTION_LYRIC_ALIGNMENT, _T("SL_com_align_mode")},//对齐方式
	{QCM_OPTION_LYRIC_KTVMODE, _T("SL_ktv_lyric")},		//ktv模式
	{QCM_OPTION_LYRIC_IMG, _T("lyricimage")},
	{QCM_OPTION_LYRIC_FOLDER, _T("lyricfolder")},

	// 用户设置
	{ QCM_OPTION_CHANGEGESTURE, _T("cloudvertify_btn_change") },
	{ QCM_OPTION_CLOUDVERTIFY_GESTURE, _T("cloudvertify_gesture") },
	{ QCM_OPTION_CLOUDVERTIFY_PASSWORD, _T("cloudvertify_password") },
	{ QCM_OPTION_CLOUDVERTIFY_DISABLE, _T("cloudvertify_disable") },

	//本体文件属性
	{QCM_MEDIAINFO_OK, _T("info_ok")},
	{QCM_MEDIAINFO_MORE,_T("info_more")},

	//媒体库
	{QCM_MEDIACENTER_CREATE, _T("createMediaCenter")},
	{QCM_MEDIACENTER_ADDFOLDER, _T("addfolder")},
	{QCM_MEDIACENTER_AUTOSEARCH,_T("addFolder_autoSearch")},
	{QCM_MEDIACENTER_STOPSEARCH,_T("addFolder_stopSearch")},
	{QCM_MEDIACENTER_ALLCHOOSE,_T("addFolder_allchoose")},
	{QCM_MEDIACENTER_CANCLEALLCHOOSE,_T("addFolder_cancleallchoose")},
	{QCM_MEDIACENTER_MANUALLYADD,_T("addFolder_manuallyadd")},
	{QCM_MEDIACENTER_EXTENDBTN, _T("showlistbt")},
	{QCM_MEDIACENTER_VIRTUALBTN, _T("virtualbtn")},
	{QCM_MEDIACENTER_NORMALBTN, _T("normalbtn")},
	{QCM_MEDIACENTER_RESCAN, _T("rescan")},
	{QCM_MEDIACENTER_CHANNEL_RESCAN, _T("updatebtn")},
	{QCM_MEDIACENTER_CHANNEL_PLAY, _T("playbtn")},
	{QCM_MEDIACENTER_LOGIN,_T("logbtn")},

	//书签
	{QCM_MEDIACENTER_BOOKMARK_ADD,		_T("addbtn")},
	{QCM_MEDIACENTER_BOOKMARK_DELETE,	_T("delbtn")},
	{QCM_MEDIACENTER_BOOKMARK_MODIFY,	_T("modify")},
	{QCM_MEDIACENTER_WEBAUDIO_IDOK,		_T("btn_ok")},
	{QCM_MEDIACENTER_WEBAUDIO_CANCEL,	_T("btn_cancel")},
	{QCM_MEDIACENTER_EXITPLAY_CLEAR,	_T("clearbtn")},
	{QCM_MEDIACENTER_EXITPLAY_SAVE,		_T("savebtn")},

	{QCM_MEDIACENTER_NETTASK, _T("m_nettask")},
	{QCM_MEDIACENTER_MEDIA,_T("m_mymeida")}, 
	{QCM_MEDIACENTER_ONLINE, _T("m_onlinefilm")},
	{QCM_MEDIACENTER_GAMELOBBY, _T("m_gamelobby")},
	{QCM_MEDIACENTER_BEAUTIFULANCHOR, _T("m_beautifulanchor")},													 

	
	{QCM_MEDIACENTER_FILM,_T("m_film")}, 
	{QCM_MEDIACENTER_MUSIC,_T("m_music")}, 
	{QCM_MEDIACENTER_PLAYLISTBTN,_T("m_playlist")}, 
	{QCM_MEDIACENTER_CLOUDLISTBTN, _T("m_cloudlist")},
	{QCM_MEDIACENTER_PLAYLIST,_T("mediacenterList")},
	{QCM_MEDIACENTER_MEDIATABLE,_T("medialTable")},
	{QCM_MEDIACENTER_CUSTOMLIST,_T("mediaCustomList")},
	{QCM_MEDIACENTER_SHOWMODE,_T("media_listmode")},
	{QCM_MEDIACENTER_SHOWMODE,_T("media_imgmode")},
	{QCM_MEDIACENTER_ORDER,_T("media_sort")},
	{QCM_MEDIACENTER_QRCODE, _T("media_qrcode")},
	{QCM_MEDIACENTER_HOME,_T("media_home")},
	{QCM_MEDIACENTER_UPWARD,_T("media_upward")},
	{QCM_MEDIACENTER_CREATEDIR,_T("media_createdir")},
	{QCM_MEDIACENTER_REFRESH,_T("media_refresh")},
	{QCM_MEDIACENTER_WAIT_CANCLE,_T("wait_cancle")},
	{QCM_MEDIACENTER_WAIT_OK,_T("wait_ok")},
	
	{QCM_MEDIACENTER_LINKTV, _T("linktv")},
	{QCM_MEDIACENTER_TVSCAN, _T("media_linktvscan")},
	{QCM_MEDIACENTER_TVBUY,_T("linktv_ad_btn")},
	{QCM_MEDIACENTER_VIEDIO,_T("ad_btn_vidio")},
	{QCM_MEDIACENTER_DETAIL,_T("ad_btn_detail")},
	{QCM_MEDIACENTER_TVLIST,_T("linktvlist")},

	//网络任务
	{QCM_TASK_ITEMMESSAGE, _T("netitem")},
	{QCM_TASK_ITEM_LOAD, _T("stopnetbtn")},
//	{QCM_TASK_OPENTORFILE,  _T("openfile")},
	{QCM_LIST_MESSAGE, _T("main_multiList")},
	{QCM_PRIVATE_LIST_MESSAGE, _T("privatelist")},
	{QCM_COMBOX_LIST_MESSAGE, _T("combox_multiList")},
	{QCM_ONLINEMEDIA_LIST_MESSAGE, _T("onlinemediacenterList")},
	{QCM_ONLINEMEDIA_SEARCH_RESULT_LIST_MESSAGE, _T("SearchResultList")},
	{QCM_ONLINEMEDIA_MORE, _T("onlinelistmore")},

	//视频调节
	{QCM_SAVE_VIEDO, _T("savevideo")},						//	保存视频  ---视频截屏 
	{QCM_SAVE_AUDIO, _T("saveaudio")},

	//换肤
	{QCM_OPEN_SKINWND, _T("changeskinbtn")},
	{QCM_SKIN_COLORMIX, _T("colormix")},
	{QCM_SKIN_SHADDING, _T("shading")},
	{QCM_SKIN_SKIN, _T("skin")},
	{QCM_SKIN_COLORBTN0, _T("button0")},
	{QCM_SKIN_COLORBTN1, _T("button1")},
	{QCM_SKIN_COLORBTN2, _T("button2")},
	{QCM_SKIN_COLORBTN3, _T("button3")},
	{QCM_SKIN_COLORBTN4, _T("button4")},
	{QCM_SKIN_COLORBTN5, _T("button5")},
	{QCM_SKIN_COLORBTN6, _T("button6")},
	{QCM_SKIN_COLORBTN7, _T("button7")},

	{QCM_SKIN_SLIDERH, _T("sliderH")},
	{QCM_SKIN_SLIDERS, _T("sliderS")},
	{QCM_SKIN_SLIDERL, _T("sliderL")},

	{QCM_SKIN_CHOOSE0, _T("skin_choose0")}, //皮肤
	{QCM_SKIN_CHOOSE1, _T("skin_choose1")},
	{QCM_SKIN_CHOOSE2, _T("skin_choose2")},
	{QCM_SKIN_CHOOSE3, _T("skin_choose3")},

	{QCM_SKIN_SHADDING0,_T("shadding0")},//底纹
	{QCM_SKIN_SHADDING1,_T("shadding1")},
	{QCM_SKIN_SHADDING2,_T("shadding2")},
	{QCM_SKIN_SHADDING3,_T("shadding3")},
	{QCM_SKIN_SHADDING4,_T("shadding4")},
	{QCM_SKIN_SHADDING5,_T("shadding5")},
	{QCM_SKIN_SHADDING6,_T("shadding6")},
	{QCM_LISTHIDE, _T("listhidebtn")},//播放列表隐藏

	//歌词搜索
	{QCM_LYRICSEARCH_SEARCH, _T("search_btn")},
	{QCM_LYRICSEARCH_DOWN, _T("down_btn")},

	//转到
	{QCM_GOTO_GOTO, _T("Goto_goto")},
	{QCM_GOTO_CANCLE, _T("Goto_cancel")},

	//倒计时窗口
	{QCM_COUNT_NOW,	_T("count_now")},
	{QCM_COUNT_CANCLE, _T("count_cancle")},

#pragma endregion
// 效果调节
	{QCM_EFFECT_VIDEOSCREEN, _T("ev_screen")},//e_video
	{QCM_EFFECT_VIDEOCOLOR, _T("ev_color")},//视频色彩
	{QCM_EFFECT_SOUND, _T("ev_audio")},		//音频调节
	{QCM_EFFECT_SOUNDEFFECT, _T("ev_effect")}, //音效均衡
	{QCM_EFFECT_SUBTITLE, _T("ev_subtitle")}, //字幕调节
//wnd1
	{QCM_EFFECT1_SCALE_DEFAULT, _T("ew1_scale1")},
	{QCM_EFFECT1_SCALE_43, _T("ew1_scale2")},
//	{QCM_EFFECT1_SCALE_54, _T("ew1_scale3")},
	{QCM_EFFECT1_SCALE_169, _T("ew1_scale3")},
	{QCM_EFFECT1_SCALE_FULL, _T("ew1_scale4")},
	{QCM_EFFECT1_FLIPV,      _T("ew1_flipv")},
	{QCM_EFFECT1_FLIPH,      _T("ew1_fliph")},
	{QCM_EFFECT1_L90,        _T("ew1_rotatel")},
	{QCM_EFFECT1_R90,        _T("ew1_rotater")},
	{QCM_EFFECT1_ZOOMUP,     _T("ew1_zoomu")},
	{QCM_EFFECT1_ZOOMDOWN,   _T("ew1_zoomd")},
	{QCM_EFFECT1_LEFT,       _T("ew1_left")},
	{QCM_EFFECT1_RIGHT,      _T("ew1_right")},
	{QCM_EFFECT1_UP,         _T("ew1_up")},
	{QCM_EFFECT1_DOWN,       _T("ew1_down")},
	{QCM_EFFECT1_RESTORE,    _T("ew1_restore")},
//wnd2
	{QCM_EFFECT2_CMODE2,     _T("ew2_bright")},
	{QCM_EFFECT2_CMODE1,     _T("ew2_gentle")},
	{QCM_EFFECT2_BRIGHTNESS, _T("ew2_lider1")},
	{QCM_EFFECT2_SATURATION, _T("ew2_lider2")},
	{QCM_EFFECT2_CONTRAST,   _T("ew2_lider3")},
	{QCM_EFFECT2_HUE,        _T("ew2_lider4")},
	{QCM_EFFECT2_BACKB,		 _T("ew2_relider1")},			// 恢复默认亮度
	{QCM_EFFECT2_BACKS,		 _T("ew2_relider2")},			// 恢复默认饱和度
	{QCM_EFFECT2_BACKC,		 _T("ew2_relider3")},			// 恢复默认对比度
	{QCM_EFFECT2_BACKH,		 _T("ew2_relider4")},			// 恢复默认色调

//wnd3
	{QCM_EFFECT3_STEREO,_T("ew3_stereoSound")},				// 立体声
	//{QCM_EFFECT3_LCHANNEL,_T("ew3_lSound")},				// 左声道（被QCM_EFFECT3_L2ALL代替）
	//{QCM_EFFECT3_RCHANNEL,_T("ew3_rSound")},				// 右声道（被QCM_EFFECT3_R2ALL代替）
	{QCM_EFFECT3_MIXCHANNEL,_T("ew3_mixSound")},			// 混合
	{QCM_EFFECT3_L2ALL,	_T("ew3_l2aSound")},				// 
	{QCM_EFFECT3_R2ALL,_T("ew3_r2aSound")},					//
	{QCM_EFFECT3_51CHANNEL,  _T("ew3_51Sound")},			// 5.1环绕
	{QCM_EFFECT3_TRACKS,     _T("ew3_combo2")},
	{QCM_EFFECT3_SOUNDDELAY, _T("ew3_stime_combox")},		// 声音延迟
	{QCM_EFFECT3_ZOOM,       _T("ew3_lider1")},
	{QCM_EFFECT3_BALANCE,    _T("ew3_lider2")},

//wnd4
	{QCM_EFFECT4_CHECK,_T("ew4_enable")},
	{QCM_EFFECT4_SLIDER1,_T("ew4_lider1")},
	{QCM_EFFECT4_SLIDER2,_T("ew4_lider2")},
	{QCM_EFFECT4_SLIDER3,_T("ew4_lider3")},
	{QCM_EFFECT4_SLIDER4,_T("ew4_lider4")},
	{QCM_EFFECT4_SLIDER5,_T("ew4_lider5")},
	{QCM_EFFECT4_SLIDER6,_T("ew4_lider6")},
	{QCM_EFFECT4_SLIDER7,_T("ew4_lider7")},
	{QCM_EFFECT4_SLIDER8,_T("ew4_lider8")},
	{QCM_EFFECT4_SLIDER9,_T("ew4_lider9")},
	{QCM_EFFECT4_SLIDER10,_T("ew4_lider10")},
	{QCM_EFFECT4_AUDIOEFFECT,_T("ew4_combo")},

//wnd 5
	{QCM_EFFECT5_ADDSUBTITLE, _T("addcaption")},
	{QCM_EFFECT5_SETTIMETYPE, _T("ew5_time_com")},
	{QCM_EFFECT5_SETTITLEPOS, _T("ew5_cpos_com")},
	{QCM_EFFECT5_FONT,_T("ew5_font")},
	{QCM_EFFECT5_BIANCOLOR,_T("ew5_font_out")},	
	{QCM_EFFECT5_HIDEFONT, _T("ew5_chide")},
	{QCM_EFFECT5_CDELAY, _T("ew5_cdelay_combox")},	//字幕延迟
	{QCM_EFFECT5_HMOVE,	_T("ew5_h_com")},//字幕水平偏移
	{QCM_EFFECT5_VMOVE,	_T("ew5_v_com")},//....垂直....
	{QCM_EFFECT5_SHOWCAPTION, _T("ew5_cshow")}, 

	{QCM_EFFECT3_CAPTURE,    _T("ew3_capture")},


	//帮助菜单
	{QCM_ABOUT_LINK, _T("localweb")},
	{QCM_TIPWND_BBS, _T("localbbs")},
	{QCM_TIPWND_HELP,_T("localhelp")},
	{QCM_TIPWND_SHOWWND,_T("showWnd")},
	{QCM_TIPWND_NEXT,_T("nexttip")},

	//转到
	{QCM_GOTO_GOTO, _T("Goto_goto")},
	{QCM_GOTO_CANCLE, _T("Goto_cancle")},

	//用户登录
	{QCM_USERLOGIN,	_T("login")},
	//{QCM_USERMENU, _T("usermenu")},
	{QCM_LOSTPASSWORD_LINK,	 _T("lostpassword")},
	{QCM_USERREGISTER_LINK,	_T("registeruser")},
	{QCM_USERCOM,  _T("user_combox")},
	{QCM_PASSWORDCOMBOX, _T("password_combox")},
	{QCM_AUTOLOGIN,  _T("autologin")},
	{QCM_SAVEPASSWORD,  _T("savepassword")},
	{QCM_LOGIN_OK, _T("login_ok")},
	{QCM_LOGIN_CANCEL ,_T("login_cancel")},
	{QCM_OPTION_USERCENTER, _T("visitUserCenter")},
	{QCM_LOGIN_FOOTPRINT, _T("login_foot")},
	{QCM_SHOW_FOOTPRINT, _T("foot_link")},
	{QCM_LOGIN_SINA, _T("login_sina")},				//新浪微博登录
	{QCM_LOGIN_QQ, _T("login_qq")},					//QQ登录
	//{QCM_SPEEDOPEN,	_T("speed_open")},
	//{QCM_SPEEDSHUT,	_T("speed_shut")}, 

	{QCM_PRIVATETIP_CLOSE, _T("pt_close")},
	{QCM_PRIVATETIP_ADD, _T("pt_movepte")},
//	{QCM_DLINFOBTN, _T("dl_info_btn")},
	{QCM_FESTIVALLOGO_LINK, _T("login_logo")},
	{QCM_PRIVATEUSER_BTN, _T("privatename")},
	// N+
	{QCM_NPLUS, _T("NPlus")},
	{QCM_NPLUS_LIGHT, _T("NPlusLight")},
	{QCM_NPLUSTIP_OK, _T("ns_ok")},
	{QCM_NS_PEERLIST, _T("ns_peerList")},
	

	// 视频墙
	{QCM_VIDEOWALL_OPENFILE, _T("openfile")},
	{QCM_WIDI_SCAN, _T("widiScan")},
	{QCM_WIDI_CONNECT, _T("widi_connect")},
	{QCM_WIDI_LIST_MESSAGE, _T("widi_multiList")},

	// 用户信息界面
	{QCM_USERINFO_RENEWLINK,_T("renew")},
	{QCM_USERINFO_MAILVERIFYLINK,_T("mailverity")},
	{QCM_USERINFO_VIPTRY,_T("tryvip")},
	{QCM_USERINFO_USERS,_T("user_icon")},
	{QCM_USERINFO_LEVEL,_T("LvBg")},
	{QCM_USERINFO_VIP,_T("VIP")},
	{QCM_USERINFO_SPEEDPLUS,_T("speed_plus")},
	{QCM_USERINFO_CLOUDPLUS,_T("cloud_plus")},
	{QCM_USERINFO_FAVORABLE,_T("favorable_link")},
	{QCM_USERINFO_VIP_YEAR,_T("vip_year_icon")},

	{QCM_USERINFO_HONOR_ENTERHONOR,_T("honor_enterhonor")},
	{QCM_USERINFO_HONOR_SETDESTOP,_T("honor_setdestop")},
	{QCM_USERINFO_HONOR_USERINFO,_T("honor_userinfo")},
	{QCM_USERINFO_HONOR_VIEWMORE,_T("honor_more")},
	{QCM_USERINFO_HONOR_OPENGIFT,_T("open_gift")},
	{QCM_USERINFO_HONOR_WEAR,_T("honor_wear")},
	{QCM_USERINFO_HONOR_WEARNEW,_T("honor_wear_new")},
	{QCM_USERINFO_HONOR_SKETCHLEFT,_T("honor_sketchleft")},
	{QCM_USERINFO_HONOR_SKETCHRIGHT,_T("honor_sketchright")},

	{QCM_RELOAD_LINK,_T("reload_link")},
	{QCM_RETURN_LINK,_T("return_link")},

	//网址记录
	{QCM_TAGWND_LINK,_T("url_link")},

	// 文件传输
	{QCM_FT_PEERLIST, _T("ft_peerlist")},
	{QCM_FT_TASKLIST, _T("ft_tasklist")},
	{QCM_FT_FINISHEDTASKLIST, _T("ft_finishedlist")},
	{QCM_FT_CONNECT, _T("ft_connect")},
	{QCM_FT_CONNECT_CANCEL, _T("connect_cancel")},
	{QCM_FT_TRANSFER, _T("transfer")},
	{QCM_FT_COMPLETED, _T("completed")},
	{QCM_REFRESH, _T("refresh")},
	{QCM_FT_CLOSEPEER, _T("ft_closepeer")},
	{QCM_FT_OPENFILE, _T("ft_openfile")},
	{QCM_FT_PLAY, _T("ft_play")},
	{QCM_FT_DELETE, _T("ft_delete")},
	{QCM_FT_REFUSE, _T("refuse")},
	{QCM_FT_ACCEPT, _T("accept")},
	//{QCM_FT_CLEAR, _T("clear")},
	{QCM_FT_RETRY, _T("ft_retry")},
	{QCM_FT_CLOSE_TIP, _T("ft_tip_close")},
	{QCM_SEARCHMEDIA_SEARCHCOM, _T("search_combox")},
	{QCM_SEARCHMEDIA_ASSLIST ,_T("search_asslist")},
	{QCM_SEARCHMEDIA_SEARCH ,_T("search_search")},
	{QCM_SEARCH_CLOSE ,_T("search_close")},
	{QCM_LOGOUT, _T("logout")},
	{QCM_LIST, _T("list")},
	{QCM_PCLOUD_FILELIST, _T("filelist")},	 
	{QCM_RECYCLEBIN,_T("recycleBin")},
	{QCM_CLEAR,_T("clear")},
	{QCM_TAGWND_LINK, _T("Taglink")},
	{QCM_TAGWND_MORE, _T("morebtn")},
	{QCM_TAGWND_TAGIMG, _T("VTag1")},
	{QCM_RADAR_WEBLINK, _T("radarWebLink")},
	{QCM_RADAR_WEBLINK, _T("emptyWebLink")},
	{QCM_TUITUI_WEB, _T("tuituiWebLink")},
	{QCM_BTN_IKNOW, _T("btn_iknow")},
	{QCM_NO_WAKEUP, _T("nowakeup")},
	//2D to 3D
	{QCM_2T3D, _T("fb_2t3d")},				// 2D转3D
	{QCM_3T2D, _T("fb_3t2d")},				// 3D转2D
	{QCM_2T3D_NO3D_FORMAT, _T("2t3d_btn_no3d")},
	{QCM_2T3D_2T3D_FORMAT, _T("2t3d_btn_2t3d")},
	{QCM_2T3D_3D_FORMAT, _T("2t3d_btn_3d")},
	{QCM_2T3D_3T2D_FORMAT, _T("2t3d_btn_3t2d")},
	{QCM_2T3D_MODERB, _T("2t3d_rb")},
	{QCM_2T3D_MODERG, _T("2t3d_rg")},
	{QCM_2T3D_MODEBRB, _T("2t3d_browb")},
	{QCM_2T3D_VIEWANGLELR, _T("2t3d_chlr")},
	{QCM_2T3D_VIEWANGLERL, _T("2t3d_chrl")},
	{QCM_2T3D_VIEWANGLEUD, _T("2t3d_chud")},
	{QCM_2T3D_VIEWANGLEDU, _T("2t3d_chdu")},
	{QCM_2T3D_VIEWCHOOSEL, _T("2t3d_chl")},
	{QCM_2T3D_VIEWCHOOSER, _T("2t3d_chr")},
	{QCM_2T3D_VIEWCHOOSEU, _T("2t3d_chu")},
	{QCM_2T3D_VIEWCHOOSED, _T("2t3d_chd")},
	{QCM_2T3D_LIDER2T3D, _T("2t3d_lider")},
	{QCM_2T3D_3DPLAY, _T("2t3d_lider1")},
	{QCM_2T3D_PARALAXBACKB,		 _T("2t3d_relider")},
	{QCM_2T3D_BTN_OK,		 _T("2t3d_btn_ok")},
	{QCM_2T3D_BTN_CANCEL,		 _T("2t3d_btn_cancel")},
	
	{QCM_GENERATE_QRCODE,		_T("fb_qrcode")},

	//Add by chenzhibiao 
	{QCM_BRRAGE,				_T("fb_brrage")},
	{QCM_BRRAGE_CLOSE,			_T("fb_brrage_close")},

#ifdef RDSP360
   	{QCM_STDWEARHINT_CLOSE, _T("std_close")},
	{QCM_STDWEARHINT_UPGRADE, _T("upgradeSTD")},
#endif

#ifdef RDSP
	{QCM_REMIND_STDVER_NEXTVER, _T("nextver")},
	{QCM_REMIND_STDVER_NOWINST, _T("nowinst")},
#endif

	//相关剧集窗口
	{QCM_RELATEDSERIES_LIST, _T("RS_list") },
	{QCM_RELATEDSERIES_BTN_ADD, _T("RS_btn_add")},
	{QCM_RELATEDSERIES_BTN_CANCEL, _T("RS_btn_cancel")},
	//{QCM_RELATEDSERIES_CHKBOX_DOWNPLAY, _T("RS_enable_auto")},

	// 手势锁
	{QCM_GESTURELOCK_BTN0, _T("gl_btn_0")},
	{QCM_GESTURELOCK_BTN1, _T("gl_btn_1")},
	{QCM_GESTURELOCK_BTN2, _T("gl_btn_2")},
	{QCM_GESTURELOCK_BTN1, _T("gl_btn_1_")},
	{QCM_GESTURELOCK_BTN2, _T("gl_btn_2_")},
	{QCM_GESTURELOCK_LNK0, _T("gl_btn_lnk0")},
	{QCM_GESTURELOCK_BTNSETTING, _T("setting")},

	//截屏扫图(海报及二维码)
	{QCM_BTN_SUPERFOOT, _T("emptyWebLink")},
	{QCM_BTN_SUPERFOOT, _T("btn_super_foot")},
	{QCM_START_CATCH, _T("btn_saotu")},
	{QCM_HAIBAO_MOREINFO, _T("haibao_moreinfo")},
	{QCM_SAOTU_LIST, _T("saotu_list")},
	{QCM_RESULT_LIST, _T("result_list")},
	{QCM_BTN_CHECK,	_T("btn_check")},
	{QCM_BTN_CHECKALL,	_T("btn_check_all")},
	{QCM_ADD_PLAYLIST,	_T("btn_add_playlist")},
	{QCM_CATCH_OVER, _T("catch_ok")},
	{QCM_CATCH_CANCEL, _T("catch_cancel")},
	{QCM_CANCEL, _T("cancel")},
	{QCM_RE_CATCH, _T("btn_resaotu")},
	{QCM_RELATED_VIDEO, _T("related_video")},

	// 弹幕输入
	{QCM_BARRAGE_SENDCOM, _T("barrage_combox")},
	{QCM_BARRAGE_ENTER, _T("btn_enter")},
	{QCM_BARRAGE_CLOSE, _T("btn_close")},
	{QCM_BARRAGE_SETALL, _T("btn_setall")},
	{QCM_BARRAGE_OPTION, _T("btn_option")},
	
	{QCM_BARRAGE_OPTION_FONTCOM, _T("set_font_combox")},
	{QCM_BARRAGE_OPTION_TRANSPCOM, _T("transparent_combox")},
	{QCM_BARRAGE_OPTION_DENSITYCOM, _T("density_combox")},
	{QCM_BARRAGE_OPTION_TRANSPARENTSD, _T("transparent_slider")},
	{QCM_BARRAGE_OPTION_DENSITYSD, _T("density_slider")},
	{QCM_BARRAGE_OPTION_TIPCLOSE, _T("vip_tip_close")},
	{QCM_BARRAGE_OPTION_MORELINK, _T("vip_more_link")},

	// 弹幕列表
	{QCM_BARRAGE_COMMENTLIST, _T("comment_list")},
	{QCM_BARRAGE_MORE, _T("moreLink")},
	{QCM_BARRAGE_COMMENTCLOSE, _T("commentclose")},
	{QCM_BARRAGE_GOTOSQUARE, _T("gotobarage")},

	{QCM_BRRAGE_SWITCH, _T("btn_switch")},

	// 图片墙
	{QCM_SET_PROMPTWND_NOPROMPT, _T("closepromptnoprompt")},
	{QCM_SET_PROMPTWND_SHUTDOWN_FOREVER, _T("closepromptshutdownforever")},
	{QCM_SET_PROMPTWND_OK, _T("closepromptok")},
	{QCM_PCLOUD_WEB_ENTRY, _T("PCloudWebEntry")},

	{QCM_DELTASK_CLEAR, _T("btn_clear")},
	{QCM_OPEN_CLOUD_URL, _T("text_cloud")},

	// 在线视频
	{QCM_ONLINEMEDIA_HOME, _T("online_home")},
	{QCM_ONLINEMEDIA_BACK, _T("online_back")},
	{QCM_ONLINEMEIDA_FORWARD, _T("online_forward")},
	{QCM_ONLINEMEIDA_REFRESH, _T("online_refresh")}, 
	{QCM_ONLINEMEIDA_TAB, _T("online_tab")}, 
	{QCM_RETRY, _T("retry")},
	{QCM_WEB_CLOSE, _T("wb_close")},
	{QCM_WEB_OPEN,_T("wb_open")},
	{QCM_WEBTIPS_MORE,_T("wm_more"),}
};

extern std::map<CString, CommonCmd> g_mCmd;
extern std::map<CommonCmd, CString> g_mStrId;


typedef struct _UI_TIPINFO
{	
	TCHAR szSkinID[60];
	UINT resourceID;
}UI_TIPINFO;

const _UI_TIPINFO g_TipInfo[] = 
{
	{_T("close"), QVOD_STR_CLOSE},
	{_T("cancel"), IDS_OPTION_CANCLE},
	{_T("min_close"), QVOD_STR_CLOSE},
	{_T("min"), QVOD_STR_MIN},
	{_T("min_min"), QVOD_STR_MIN},
	{_T("max"), QVOD_STR_MAX},
	{_T("max_re"), QVOD_STR_MAX_RESTOR},
	{_T("play"), QVOD_STR_PLAY},
	{_T("min_play"), QVOD_STR_PLAY},
	{_T("pause"), QVOD_STR_PAUSE},
	{_T("min_pause"), QVOD_STR_PAUSE},
	{_T("mediaPos"), QVOD_STR_PLAYPROGRESS},
	{_T("min_mediaPos"), QVOD_STR_PLAYPROGRESS},
	{_T("mediacenter"), QVOD_STR_MEDIACENTER},
	{_T("tabTasklist"), QVOD_STR_NETTASK},
	{_T("tabFilelist"), QVOD_STR_LOCALFILE},
//	{_T("searchonnet"), QVOD_STR_SEARCH},
//	{_T("managestbtn"), QVOD_STR_MSEARCH},
//	{_T("changeskinbtn"), QVOD_STR_SKIN},
	{_T("mute"), QVOD_STR_MUTE},
	{_T("min_mute"), QVOD_STR_MUTE},
	{_T("unmute"), QVOD_STR_MUTE},
	{_T("min_unmute"), QVOD_STR_MUTE},
	{_T("volume"), QVOD_STR_VOLUME},
	{_T("min_volume"), QVOD_STR_VOLUME},
	{_T("stop"), QVOD_STR_STOP},
	{_T("min_stop"), QVOD_STR_STOP},
	{_T("part_prev"), IDS_MMAIN_PREMEDIA},
	{_T("part_next"), IDS_MMAIN_NEXTMEDIA},
//	{_T("linktxt"), QVOD_STR_AD},
//	{_T("openMedia"),QVOD_STR_OPENFILE},
//	{_T("showlistbt"),QVOD_STR_M_SHOWFOLDER},
//	{_T("addfolder"),QVOD_STR_M_ADDFOLDERTOM},
//	{_T("rescan"),QVOD_STR_M_RESCANFOLDER},
//	{_T("showinfo"),QVOD_STR_M_SHOWMEDIAIFNO},
	{_T("search_btn"),QVOD_STR_M_SEARCH},
	{_T("main_multiList"), IDS_MMAIN_PLAYLIST},
	{_T("mediacenterList"), IDS_MMAIN_PLAYLIST},
	{_T("protect_eye"), QVOD_PROTECTEYES},

	{_T("fb_halfx"), QVOD_STR_HALFX},
	{_T("fb_min_1x"), QVOD_STR_V_1X},
	{_T("fb_1x"), QVOD_STR_V_1X},							// 1倍大小
	{_T("fb_2x"), QVOD_STR_V_2X},						// 2倍大小
	{_T("fb_full"), QVOD_STR_V_FULL},						// 全屏
	{_T("fb_Nofull"), QVOD_STR_NOFULL},						// 退出全屏
	{_T("fb_mini"), IDS_MMAIN_SIMPMODE},						// 迷你模式
	{_T("fb_restor"), QVOD_STR_V_RESTOR},				// 还原
	{_T("fb_fore"), QVOD_STR_V_FORE},					//前端
	{_T("fb_fore_top"), QVOD_STR_FORE_PLAYTOP},
	{_T("fb_fore_playtop"),QVOD_STR_FORE_CANCLE },
//	{_T("fb_frame"), QVOD_STR_V_FRAME},								//视频框架
//	{_T("fb_lightA"), QVOD_STR_V_LIGHTA},					//亮度变亮
//	{_T("fb_lightD"), QVOD_STR_V_LIGHTD},					//亮度变暗
//	{_T("fb_turn"), QVOD_STR_V_TURN},						//翻转
//	{_T("fb_channel"), QVOD_STR_V_CHANNEL},						//声道
	{_T("fb_moreeffect"), QVOD_STR_V_MOREEFFECT},				//更多调节
	{_T("fb_screenshot"), QVOD_STR_V_SCREENSHOT},					//截屏
	{_T("fb_enhance"), QVOD_STR_VIDEOENHANCE},
	{_T("fb_enhance_close"), QVOD_STR_VIDEOENHANCE_CLOSE},
//	{_T("fb_share"), QVOD_STR_V_SHARE},						//分享
//	{_T("fb_ext_on"), QVOD_STR_V_EXTON},		//打开高级
	{_T("fb_2t3d"), IDS_2T3D_3DTRANSFORM},
	{_T("fb_3t2d"), IDS_3T2D_3DTRANSFORM},
	{_T("fb_qrcode"), IDS_MENU_GENERAGEQR},

	//Add by chenzhibiao 
	{_T("fb_brrage"), IDS_BRRAGE_OPEN},
	{_T("fb_brrage_close"), IDS_BRRAGE_CLOSE},
	{_T("btn_enter"), IDS_BRRAGE_BTN_ENTER},
	{_T("btn_close"), IDS_BRRAGE_BTN_CLOSE},
	{_T("btn_setall"), IDS_BRRAGE_BTN_SETALL},
	{_T("btn_option"), IDS_BRRAGE_BTN_OPTION},
	{_T("barrage_hand"), IDS_BRRAGE_BTN_HAND},
	{_T("commentclose"), IDS_BRRAGE_COMMENT_BTN_CLOSE},
	{_T("gotobarage"), IDS_BRRAGE_GOTO},

	{_T("fb_nofore"), QVOD_STR_V_NOFORE},
	{_T("fb_stereo"), QVOD_STR_V_SND_STEREO},
	{_T("fb_leftall"), QVOD_STR_V_SND_LEFTALL},
	{_T("fb_rightall"), QVOD_STR_V_SND_RIGHTALL},
	{_T("listhidebtn"), IDS_MMAIN_PLAYLIST},
	{_T("showmenu"), IDS_MAINMENU},
	{_T("listmode"), IDS_LISTMODE},
	{_T("logomode"), IDS_IMAGEMODE},
	{_T("btntripic"),IDS_LISTCHANGE},
	{_T("media_listmode"), IDS_LISTMODE},
	{_T("media_imgmode"), IDS_IMAGEMODE},
	{_T("media_sort"), IDS_SORT},
	{_T("media_qrcode"), IDS_MENU_GENERAGEQR},
	{_T("media_home"), IDS_TURNTO_HOME},
	{_T("media_upward"), IDS_TURNTO_UPWARD},
	{_T("media_createdir"), IDS_CREATE_DIR},
	{_T("media_refresh"), IDS_REFRESH},
	{_T("loopchoose"), IDS_LOOPMODE},
	{_T("min_openfile"), QVOD_STR_OPENFILE},
	{_T("filename"), QVOD_STR_OPENFILE},
	{_T("cut_left_l"), IDS_STARTLEFTMOVE},
	{_T("cut_left_r"), IDS_STARTRIGHTMOVE},
	{_T("cut_right_l"), IDS_ENDLEFTMOVE},
	{_T("cut_right_r"), IDS_ENDRIGHTMOVE},
	{_T("cutpos"), IDS_ENDRIGHTMOVE},
	{_T("gif_cut_left_l"), IDS_STARTLEFTMOVE},
	{_T("gif_cut_left_r"), IDS_STARTRIGHTMOVE},
	{_T("gif_cut_right_l"), IDS_ENDLEFTMOVE},
	{_T("gif_cut_right_r"), IDS_ENDRIGHTMOVE},
	{_T("forwardplay"), QVOD_STR_QICKPLAY},
	{_T("backplay"), QVOD_STR_SLOWPLAY},
	{_T("list_add"), IDS_ADD},
	{_T("list_delete"), IDS_DELETE},
	{_T("listsort"), IDS_SORT},
	{_T("login"), IDS_QVODUSERLOGIN},
	{_T("VIP"), IDS_VIP_TIPS},
	{_T("usermenu"), IDS_QVODUSERINFO_TIPS},
	{_T("logmenu"), IDS_LOGINNING_TIPS},
	{_T("speed_open"), IDS_OPENHIGHSPEED},
	{_T("speed_shut"), IDS_SHUTHIGHSPEED},
	{_T("purse"), IDS_PRUSE_PAYVIP_TIPS},
	{_T("NPlus"), STR_QVODNPLUS}, 
	{_T("NPlusLight"), STR_QVODNPLUS},
	{_T("refresh"),  STR_FT_TIP_REFRESH},
	{_T("ft_closepeer"),  STR_FT_TIP_DISCONNECT},
	{_T("ft_openfile"),  STR_FT_TIP_ADDFILE},
	{_T("logout") ,IDS_USERMENU_LOGOUT},
	{_T("btn_super_foot"), FOOTPRINT_WEBLINKTXT},

	//相关剧集窗口
	{_T("RS_btn_add"), IDS_RELATEDSERIES_ADD},
	{_T("RS_btn_cancel"), IDS_RELATEDSERIES_CANCEL},
	{_T("setting"), IDS_PRIVATECLOUD_GESTURELOCK_SETTING},
	
	{_T("PCloudWebEntry"), IDS_PCLOUD_WEBENTRY_TIPS},
	
	{_T("online_home"), IDS_ONLINE_TIPS_HOME},
	{_T("online_back"), IDS_ONLINE_TIPS_BACK}, 
	{_T("online_forward"), IDS_ONLINE_TIPS_FORWARD}, 
	{_T("online_refresh"), IDS_ONLINE_TIPS_REFRESH}, 
};
extern std::map<CString, UINT> g_mTipID;

typedef struct _HOTKEYI_MS_NFO
{	
	UINT nMsgID;
	UINT nEditMsgID;
}HOTKEYI_MS_NFO;

const _HOTKEYI_MS_NFO g_HotKeyInfo[] = 
{
	//文件
	{ID_MENU3_FEEDBACK, QCM_HOTKEY_HELP},//帮助信息
	{ID_MEDIACENTER, QCM_HOTKEY_OPENMEDIA},//我的媒体库
	{ID_DVD, QCM_HOTKEY_OPENCD},//打开碟片/DVD
	{ID_MENU1_OPENLOCAL, QCM_HOTKEY_OPENLOCALFILE},//打开本地文件(O)
	{ID_MENU1_OPTIONS, QCM_HOTKEY_OPTION},//选项
	{ID_MENU1_OPENNET, QCM_HOTKEY_OPENNET},//打开网络文件(U)

	//播放
	{ID_FULLSCREEN, QCM_HOTKEY_FULL},//全屏
	{ID_NOFULLSCREEN, QCM_HOTKEY_EXITFULL},//退出全屏
	{ID_MENU2_FRAME_MINI, QCM_HOTKEY_MINI},//精简模式
	{ID_NOWPLAY, QCM_HOTKEY_PLAYING},//正在播放
	{ID_TRAY_PLAYLIST,  QCM_HOTKEY_PLAYLIST},//播放列表
	{ID_PLAYONDESK, QCM_HOTKEY_DESKPLAY}, // 桌面播放
	{ID_MENU2_PART_BEGIN, QCM_HOTKEY_APOINT},//起点位置
	{ID_MENU2_PART_END,QCM_HOTKEY_BPOINT},//终点位置
//	{ID_MENU2_PART_PLAY, QCM_HOTKEY_BEGINLOOP},//开始循环播放
	{ID_MENU2_PART_CANCEL, QCM_HOTKEY_ENDLOOP}, //取消循环播放
	{ID_MENU2_PC_TO, QCM_HOTKEY_TURNTO},//转到(&G)
	{ID_MENU2_PLAY_PAUSE, QCM_HOTKEY_PLAYPAUSE},//播放/暂停
	{ID_SPACEPLAY,  QCM_HOTKEY_SPACEPAUSE},//空格键播放/暂停
	{ID_MENU2_STOP, QCM_HOTKEY_STOP},//停止(&S)
	{ID_MENU2_PC_JUMPAHEAD, QCM_HOTKEY_HEADJUMP},//向前跳跃
	{ID_MENU2_PC_JUMPBACK, QCM_HOTKEY_BACKJUMP},//向后跳跃
	{ID_MENU2_PC_QUICKAHEAD, QCM_HOTKEY_QUICKPLAY},//快进播放
	{ID_MENU2_PC_QUICKBACK, QCM_HOTKEY_SLOWPLAY},//快退播放
	{ID_MENU2_PC_FAST, QCM_HOTKEY_ASPEEDPLAY},//加速播放(&A)
	{ID_MENU2_PC_SLOW, QCM_HOTKEY_DSPEEDPLAY},//减速播放(&C)
	{ID_MENU2_PC_RESTORE, QCM_HOTKEY_RESTORSPEED},//恢复播放(&R)
	{ID_MENU2_PREV, QCM_HOTKEY_UPONE},//上一个(&V)
	{ID_MENU2_NEXT, QCM_HOTKEY_NEXTONE},//下一个(&N)
	{ID_MENU2_VIDEO_CFG, QCM_HOTKEY_VIDEOADJUST},//视频调节
	{ID_MENU1_CLOSE, QCM_HOTKEY_CLOSEPLAYER},//关闭	
	{ID_MENU2_TOP_ALWAYS, QCM_HOTKEY_TOPSHOW},//始终前端显示

	//画面
	{ID_MENU2_PC_FRAMEBACK, QCM_HOTKEY_UPFRAME},//上一帧
	{ID_MENU2_PC_FRAMENEXT, QCM_HOTKEY_NEXTFRAME},//下一帧
	{ID_MENU2_CAPTURE_IMAGE, QCM_HOTKEY_CUTSCREEN},//截取画面
	{ID_MENU2_FRAME_50, QCM_HOTKEY_MODE_50}, 
	{ID_MENU2_FRAME_100, QCM_HOTKEY_MODE_100},//倍窗口
	{ID_MENU2_FRAME_200, QCM_HOTKEY_MODE_200},//2倍窗口

	//声音
	{ID_MENU2_VOL_DOWN, QCM_HOTKEY_LOWVOL},//减小音量
	{ID_MENU2_VOL_UP, QCM_HOTKEY_HIVOL},//       "升高音量"
	{ID_MENU2_VOL_MUTE, QCM_HOTKEY_MUTE},//静音
	{ID_MAINMENU_PLAY_AUDIOBALANCE, QCM_HOTKEY_SOUNDBAN},//音效均衡(&E)
	{ID_MENU2_SOUND_CFG, QCM_HOTKEY_SOUNDADJUST},//音频调节(&J)

	//字幕
	{ID_MENU2_SUB_CFG, QCM_HOTKEY_SUBTITLE}//字幕调节	
};

extern std::map<UINT, UINT> g_mHotKeyID;

void InitSkinCmd();
void UinitSkinCmd();

enum skin_state
{
	state_disable = 0,
	state_enable,
	state_normal,
	state_hover,
	state_down,
	state_up,
	state_show,
	state_hide
};

enum BHOType
{
	bho_null = 0,
	bho_ie,
	bho_buffer,
	bho_pause
};

enum skin_type
{
	skin_base = 0,
	skin_text,
	skin_rolllinktext,
	skin_image,
	skin_stateimage,
	skin_button,
	skin_button_text,
	skin_button_Step,
	skin_button_Menu,
	skin_button_check,
	skin_slider,
	skin_playslider,
	skin_ahereSlider,
	skin_cutslider,
	skin_scroll,
	skin_edit,
	skin_hotKey,
//	skin_progress,
	skin_progressbar,
	skin_tasklist_item, 
	skin_tasklist,
	skin_table_image,
	skin_table,
	skin_list,
	skin_treeitem,
//	skin_tree,
	skin_combobox,
	skin_menu_item,
	skin_menu,
	skin_subwindow,
	skin_videowindow,
	skin_videoshowwindow,
	skin_videowallwindow,
//	skin_enlargewindow,
	skin_window,
//	skin_dialog,
	skin_displaybox,
	skin_static_text,
	skin_table_item,
	skin_volume,
	skin_updown,
	skin_tabbtns,
	skin_container,
	skin_qvodlist,
	skin_multiListCtrl,
	skin_editListCtrl,
	skin_mediaTableCtrl,
	skin_customListCtrl,
	skin_onlinemediaListCtrl,
	skin_splitter,
	skin_dragframe,
	skin_tabbutton,
	skin_radiobox,
	skin_ui_list,
	skin_ui_listframe,
	skin_ui_frame,
	skin_ui_tabframe,	//	
#ifdef VIDEOWALL
	skin_videowallFrameEx,
#endif
};

enum cursor_type
{
	cursor_arrow = 0,
	cursor_hand,
	cursor_we,
	cursor_ns,
	cursor_nwse,
	cursor_nesw,
	cursor_sizeall,
	cursor_ibeam,
	cursor_move,
	cursor_zoom,
	cursor_zoomout,
	cursor_zoomin
};

enum large_type
{
	lt_init = 0,
	lt_down,
	lt_move,
	lt_large
};

#define	GIF_MAXTIME			10
#define GIF_FPS				10
#define GIF_CUTWND_H		90
#define GIF_BIG_H			240
#define GIF_BIG_W			320
#define GIF_MID_H			150
#define GIF_MID_W			200
#define GIF_SMALL_H			120
#define GIF_SMALL_W			160


#define GIF_OP_SAVE			1
#define GIF_OP_STOP			2
#define GIF_OP_PREVIEW		3

#define GIF_STATE_NONE		0
#define GIF_STATE_CUTTING	1
#define GIF_STATE_PREVEWING	2

#define VIDEOWALLMAXCOUNT		9

#define layout_left			0
#define layout_top			0
#define layout_right		0x1
#define layout_bottom		0x2
#define layout_hCenter		0x4		// 水平居中
#define layout_vCenter		0x8		// 垂直居中
#define layout_hStretch		0x10	// 水平拉伸
#define layout_vStretch		0x20	// 垂直拉伸
#define layout_hTiled		0x40	// 水平平铺
#define layout_vTiled		0x80	// 垂直平铺

#define STRCtrl		_T("Ctrl")
#define STRAtl		_T("Alt")
#define STRShift	_T("Shift")
#define STRJia		_T("+")

#define	QVODCFG_INI								_T("QvodCfg.ini")
#define QVODADDIN_INI							_T("VisLrc.ini")

#define QVODCFG_DMS								_T("DMS")
#define QVODCFG_DMS_Definition					_T("Definition")
#define QVODCFG_DMS_Table						_T("Type")
#define QVODCFG_DMS_Dlna						_T("Dlna")
#define QVODCFG_DMS_LocalMode					_T("LocalMode")

#define QVODCFG_GLOBALINFO						_T("GLOBALINFO")
#define QVODCFG_TERMINAL						_T("QVODTERMINAL")
#define QVODCFG_TERMINAL_RADAR					_T("RADAR")
#define QVODCFG_TERMINAL_ENABLE					_T("enable")
#define QVODCFG_TERMINAL_PORT					_T("listenport")
#define QVODCFG_TERMINAL_SETPORT				_T("setlistenport")
#define QVODCFG_TERMINAL_EnableDelTask			_T("EnableDelTask")
#define	QVODCFG_TERMINAL_MinValue				_T("MinValue")
#define QVODCFG_TERMINAL_UDPONLY				_T("UDPONLY")
#define QVODCFG_TERMINAL_AdminPort				_T("AdminPort")
#define QVODCFG_TERMINAL_protocol				_T("protocol")
#define QVODCFG_TERMINAL_Update					_T("update")
#define QVODCFG_TERMINAL_SPEEDLIMIT				_T("speedlimit")
#define QVODCFG_TERMINAL_FILEPATH				_T("mediapath")

#define XML_Q_HotKey_Item_virt			_T("Keyvirt_%d")
#define XML_Q_HotKey_Item_key			_T("Keykey_%d")
#define XML_Q_HotKey_Item_cmd			_T("Kyecmd_%d")

#define QVODTERMINALEXE					_T("QvodTerminal.exe")

#define QVOD_HOME_PAGE					_T("http://hao.kuaibo.com/")
#define QVODBOX_WND						_T("QvodBoxWnd")

typedef struct _SKIN_LAYOUT
{
	DWORD alignment;
	float x;
	float y;
	float w;
	float h;
	_SKIN_LAYOUT()
	{
		memset(this, 0, sizeof(_SKIN_LAYOUT));
		w = 1.0f;
		h = 1.0f;
	}
}SKIN_LAYOUT;

// typedef struct _CTRL_INFO
// {
// 	CString name;
// 	skin_type ctrlType;
// 	CTRL_LAYOUT layout;
// 	CString tip;
// }CTRL_INFO;

typedef struct _SKIN_INFO
{
	CString strSkinName;
	CString strShadingName;
	CString strSkinPath;
	DWORD g_dwColor_narmal;
	DWORD g_dwColor_hilight;
	DWORD g_dwColor_focus;

}SKIN_INFO;

// 资源ID号
enum ID_RESOURCE
{
// 	ID_LIST_CATEGORY=301,
// 	ID_LIST_FILES,
	IDC_LIST_HOTKEY = 303,
	IDC_COMBO1,
	IDC_COMBO2
};

enum noteMode
{
	note_null = 0,
// 	note_null_save,		// 显示无，并且保存 之前的信息
// 	note_show_old,		// 显示之前的信息
	note_broadcast,		// 广播
 	note_status			// 一直显示
};

enum wndMove
{
	wmove_null = 0,		// 还原
	wmove_left,			// 左移
	wmove_right,		// 右移
	wmove_top,			// 上移
	wmove_bottom,		// 下移
	wmove_enlarge,		// 放大
	wmove_small			// 缩小
};

enum noteType
{
	note_hide = 0,		// 隐藏
	note_ready,			// 就绪
	note_createds,		// 连接
	note_pause,			// 暂停
	note_abplay,		// AB循环播放
	note_restor_video,	// 画面缩放，平移后提示信息
	note_speedplay,		// 加速减速播放
	note_quickplay,		// 快进
	note_slowplay,		// 快退
	note_buffering,		// 缓冲
#if CHECKNET
	note_buffering_netstop, //缓冲时网络中断
#endif
	note_downspeed,		// 下载速度
	note_recordPer,		// 截取进度
	note_play,			// 播放，简单提示然后消失
	note_realtime_play,	// 实时播放信息，一直提示，区分本地和网络任务
	note_netspeed,		// 网络限速模式
	note_changevol,		// 音量
	note_changemute,	// 静音切换
	note_topshow,		// 前端显示
	note_bufferend,		// 缓冲结束
	note_playfromlast,	// 上次播放位置	
	note_loopmode,		// 循环模式	
	note_speedplay_faild,// 加减速失败	
	note_recordPer_end,  // 截取结束
	note_net_createfaild,// 网络创建失败
	note_save_minimage,	// 截取缩略图
	note_soundchannel,	// 声道 
	note_addsubtitle,	// 加载字幕
	note_videoframe,	// 视频框架
	note_enhance,		// 视频增强
	note_resizewnd,		// 改变窗口大小
	note_skipstart,		// 跳过片头
	note_skipstart_overflow,
	note_skipstart_noseek,
	note_skipend_overflow,
	note_skipend_noseek,
	note_protecteyes,	// 护眼提示
	note_skip,			// 向前向后跳跃
	note_nonskip,
	note_livestop,		// 直播因新建点播而中断
	note_speed,			// 关闭\开启加速
	note_search_source,	// 正在查找图片来源
	note_search_sourceok,//已找到图片来源
	note_search_sourcefaild,//找不到图片来源
	note_nplus_connect,
	note_nplus_connect_failed,
	note_nplus_connect_int,
	note_create_private_task,
	note_nplus_cannotplay,
	note_play_nonseek,
	note_play_dormant,
	note_play_shutdown,
	note_play_prevframe,
	note_play_nextframe,
	note_play_nonprevframe,
	note_play_nonnextframe,
	note_play_full_com,
	note_play_full_cinema,
	note_recv_shutdown,
	note_AB_setA,
	note_AB_cancel,
	note_goto_invalid,
	note_nonseek,
	note_hotspot_net_bad,
	note_drag_lyrics,
	note_barrage_open,		//开启弹幕
	note_barrage_close		//关闭弹幕
};

typedef 
enum tagDVD_MENU_ID
{
	DVD_NULL = 0,
	DVD_MENU_NULL = 1,
	DVD_MENU_Title	= 2,
	DVD_MENU_Root	= 3,
	DVD_MENU_Subpicture	= 4,
	DVD_MENU_Audio	= 5,
	DVD_MENU_Angle	= 6,
	DVD_MENU_Chapter	= 7
} DVD_MENU_ID;

// 文件类型 以后写入资源
const TCHAR g_szFileExt_Common[] = _T("常见音视频文件(*.rmvb;*.avi;*.wma;*.mp3;...)");
const TCHAR g_szFileExt_Video[] = _T("视频文件(*.rmvb;*.mov;*.avi;*.mpg;*.mkv;*.wmv;...)");
const TCHAR g_szFileExt_Audio[] = _T("音频文件(*.mp3;*.midi;*.ac3;*.mpa;*.rmi;*.flac;...)");
const TCHAR g_szFileExt_Image[] = _T("图像文件(*.bmp;*.gif;*.jpg;*.tiff;...)");
const TCHAR g_szFileExt_PlayList[] = _T("播放列表(*.qpl;*.m3u;*.wpl)");

const TCHAR g_szFileExt_All[] = _T("所有文件(*.*)");
const TCHAR g_szFiles_Common[] =
		_T("*.qmv;*.qmvb;*.ape;*.rmvb;*.mov;*.amv;*.avi;*.mpg;*.mpeg;*.mpe;*.3gp;*.mp4;*.flv;*.wm;*.swf;*.csf;")\
		_T("*.ts;*.tp;*.mkv;*.dat;*.vob;*.qt;*.ram;*.wav;*.wmv;*.asf;*.rm;*.3gp2;*.3gpp;*.mkv;*.au;*.flac;*.ra;") \
		_T("*.m4a;*.mpga;*.wma;*.mp3;*.midi;*.ac3;*.aac;*.ogg;*.mid");
const TCHAR g_szFiles_Video[] =
		_T("*.qmv;*.qmvb;*.m4v;*.f4v;*.tim;*.swf;*.csf;*.scm;*.ts;*.tp;*.mkv;*.dat;*.vob;*.qt;*.wmv;*.asf;*.rm;") \
		_T("*.rmvb;*.mov;*.avi;*.mpg;*.mpeg;*.mpe;*.3gp;*.mp4;*.flv;*.wm;*.rp;*.rpm;*.rt;*.rsc;") \
		_T("*.3gp2;*.3gpp;*.acc;*.aif;*.aifc;*.aiff;*.asx;*.bik;*.flc;*.roq;*.d2v;*.dsa;*.dss;*.dsv;*.m1v;") \
		_T("*.m2p;*.m2v;*.m4b;*.mkv;*.mod;*.ogm;*.pmp;*.pm2;*.pss;*.m4p;*.mv;*.m2ts;*.m2t;*.evo;*.3g2;*.wmp;*.wm;*.ivf;") \
		_T("*.rmm;*.mjp;*.pva;*.rat;*.dvd;*.smk;*.tpr;*.vg2;*.vid;*.vp6;*.vp7;*.amv;*.fli;*.pmp2;*.mts;*.drc;");//*.fsp;
const TCHAR g_szFiles_Audio[] =
		_T("*.smv;*.spx;*.au;*.flac;*.ra;*.ram;*.wav;*.amr;*.m4a;*.mpga;*.mp5;*.dsm;*.mpa;") \
		_T("*.mac;*.dts;*.mpga;*.wma;*.mp2;*.mp3;*.midi;*.ac3;*.aac;*.ogg;*.mid;*.ape;*.cda;*.rmi;*.tta;*.tta");
const TCHAR g_szFiles_Image[] =
		_T("*.bmp;*.gif;*.jpg;*.tif;*.jpeg;*.jpe;*.pcd;*.bnm;*.qti*.qtf;*.qtif;*.tif;*.tiff;*.tiff");
const TCHAR g_szFiles_QvodImage[] =
		_T("*.bmp;*.gif;*.jpg;*.jpeg");
const TCHAR g_szFiles_List[] =
_T("*.wpl;*.m3u;*.ttpl;*.qpl;*.cue;*.wmx;*.wvx;*.smil;*.asx");
const TCHAR g_szFiles_Seed[] = _T("*.qsed;*.torrent");
 const TCHAR g_szImage_Filter[] = _T("*.swf;");
#ifdef VIDEOWALL
const TCHAR g_szFiles_SubTitle[] = _T("*.ssa;*.srt;*.ass;*.smi;*.idx;*.sub;*.psb;*.txt;*.usf;*.xss");// add by Wangyungang
#else
const TCHAR g_szFiles_SubTitle[] = _T("*.ssa;*.srt;*.ass;*.smi;*.idx;*.sub");
#endif

//音效类型数据
const int g_nAudioEffect[][10] = {{107  ,93  ,86  ,79  ,107  ,107  ,100  ,100  ,71  ,71},	//摇滚
								{107  ,100  ,86  ,86  ,107  ,107  ,100  ,100  ,71  ,57},	//打击
								{129  ,100  ,100  ,114  ,100  ,100  ,86  ,79  ,100  ,121},	//垃圾
								{129  ,100  ,100  ,100  ,100  ,100  ,86  ,100  ,86  ,100},	//金属
								{142  ,93  ,71  ,107  ,114  ,114  ,100  ,100  ,136  ,136},	//电子
								{107  ,100  ,100  ,86  ,86  ,100  ,100  ,100  ,79  ,79},	//乡村
								{100  ,100  ,100  ,79  ,79  ,79  ,100  ,86  ,129  ,129},	//爵士
								{100  ,93  ,86  ,100  ,100  ,100  ,100  ,100  ,86  ,86},	//非电
								{107  ,100  ,93  ,93  ,86  ,100  ,100  ,100  ,86  ,100},	//民乐
								{100  ,79  ,79  ,100  ,100  ,100  ,100  ,100  ,93  ,93},	//新纪
								{100  ,57  ,57  ,79  ,100  ,100  ,100  ,100  ,86  ,86},		//古典
								{107  ,100  ,86  ,93  ,100  ,100  ,100  ,100  ,107  ,121},	//布鲁斯
								{114  ,100  ,86  ,93  ,100  ,100  ,100  ,100  ,114  ,136},	//怀旧
								{107  ,100  ,100  ,121  ,100  ,79  ,71  ,100  ,79  ,71},	//瑞格
								{100  ,100  ,100  ,79  ,71  ,86  ,64  ,86  ,100  ,100},		//歌剧
								{107  ,100  ,100  ,100  ,79  ,79  ,100  ,86  ,71  ,71},		//摇摆
								{114  ,100  ,86  ,93  ,100  ,100  ,100  ,100  ,114  ,136},	//语音
								{100  ,100  ,100  ,100  ,100  ,100  ,100  ,100  ,114  ,100},//音乐56K
								{100  ,100  ,100  ,100  ,100  ,100  ,100  ,114  ,121  ,100},//音乐28K
								{100  ,100  ,100  ,100  ,100  ,100  ,100  ,100  ,100  ,100}};//默认
#endif