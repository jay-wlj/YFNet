
#ifndef MEDIACONN_H
#define MEDIACONN_H
#include <map>
#include <string>

using namespace std;

#define EXTEND_MAX_PATH 520
#ifdef _UNICODE
#define ZeroTChar(pStr, cbSize) memset(pStr, 0, cbSize*2)
#else
#define ZeroTChar(pStr, cbSize) memset(pStr, 0, cbSize)
#endif
struct CONNFILEICON
{
	LPCTSTR pszfilekey;
	LPCTSTR pszbackitem;
	LPCTSTR pszconnkey;
	LPCTSTR pszparam;
	UINT	uicon;
	BOOL	bSet;
};

const TCHAR kBackKeyValue[] = _T("mangobak");

//关联文件LPCTSTR pszparam;
static CONNFILEICON _connfileicon[]=
{//1-asf;2-avi;3-dvd;4-mp3;5-mpeg;6-qt;7-real;8-wm;9-wmp;10-wmv;11-flv,flash;12-cd;13-可播放格式 //15-scm
	// pszfilekey;       pszbackitem;         pszconnkey;         pszparam;	  uicon;
	//常见视频/DvdRip/Mpeg4
	{	_T(".avi")  , kBackKeyValue, _T("mango.avi"), _T("localfile"), 3,  TRUE},//6
	{	_T(".mkv")	, kBackKeyValue, _T("mango.mkv"), _T("localfile"), 19, TRUE},
	{	_T(".ogm")	, kBackKeyValue, _T("mango.ogm"), _T("localfile"), 1,  TRUE},
	{	_T(".mp4")	, kBackKeyValue, _T("mango.mp4"), _T("localfile"), 4,  TRUE},
	{	_T(".m4p")	, kBackKeyValue, _T("mango.m4p"), _T("localfile"), 1,  TRUE},
	{	_T(".m4b")	, kBackKeyValue, _T("mango.m4b"), _T("localfile"), 1,  TRUE},
	//Mpeg/Vcd/Dvd 视频文件
	{	_T(".dat")	, kBackKeyValue, _T("mango.dat"), _T("localfile"), 5,  FALSE},//14
	{	_T(".mpg")	, kBackKeyValue, _T("mango.mpg"), _T("localfile"), 26,  TRUE},
	{	_T(".mpeg")	, kBackKeyValue, _T("mango.mpeg"), _T("localfile"), 25,  TRUE},
	{	_T(".tp")	, kBackKeyValue, _T("mango.tp"), _T("localfile"), 1,		TRUE},
	{	_T(".ts")	, kBackKeyValue, _T("mango.ts"), _T("localfile"), 27,  TRUE },
	{	_T(".tpr")	, kBackKeyValue, _T("mango.tpr"), _T("localfile"), 1,  TRUE },
	{	_T(".pva")	, kBackKeyValue, _T("mango.pva"), _T("localfile"), 1,  TRUE },
	{	_T(".pss")	, kBackKeyValue, _T("mango.pss"), _T("localfile"), 1,  TRUE },
	{	_T(".vob")	, kBackKeyValue, _T("mango.vob"), _T("localfile"), 28,  TRUE },
	{	_T(".mpe")	, kBackKeyValue, _T("mango.mpe"), _T("localfile"), 6,  TRUE },
	{	_T(".wv")	, kBackKeyValue, _T("mango.wv"), _T("localfile"), 1,  TRUE },
	{	_T(".m2ts")	, kBackKeyValue, _T("mango.m2ts"), _T("localfile"), 1,  TRUE },
	{	_T(".m2t")	, kBackKeyValue, _T("mango.m2t"), _T("localfile"), 1,  TRUE },
	{	_T(".evo")	, kBackKeyValue, _T("mango.evo"), _T("localfile"), 1,  TRUE },
	{	_T(".dvd")	, kBackKeyValue, _T("mango.dvd"), _T("localfile"), 7,  TRUE },

	//Real 音频/视频文件
	//{	_T(".ra")	, kBackKeyValue, _T("mango.ra"), _T("localfile"), 8,  TRUE },//9
	{	_T(".ram")	, kBackKeyValue, _T("mango.ram"), _T("localfile"), 8,  FALSE },
	{	_T(".rm")	, kBackKeyValue, _T("mango.rm"), _T("localfile"), 8,  TRUE },
	{	_T(".rmvb")	, kBackKeyValue, _T("mango.rmvb"), _T("localfile"), 9,  TRUE },
	{	_T(".rp")	, kBackKeyValue, _T("mango.rp"), _T("localfile"), 8,  TRUE },
	{	_T(".rpm")	, kBackKeyValue, _T("mango.rpm"), _T("localfile"), 8,  TRUE },
	{	_T(".rsc")	, kBackKeyValue, _T("mango.rsc"), _T("localfile"), 8,  TRUE },
	{	_T(".rt")	, kBackKeyValue, _T("mango.rt"), _T("localfile"), 8,  TRUE },
	{	_T(".smil")	, kBackKeyValue, _T("mango.smil"), _T("localfile"), 17,  TRUE },

	//QuickTime 音频/视频文件
	{	_T(".aif")	, kBackKeyValue, _T("mango.aif"), _T("localfile"), 10,  TRUE },//10
	{	_T(".aiff")	, kBackKeyValue, _T("mango.aiff"), _T("localfile"), 10,  TRUE },
	{	_T(".aifc")	, kBackKeyValue, _T("mango.aifc"), _T("localfile"), 10,  TRUE },
	{	_T(".amr")	, kBackKeyValue, _T("mango.amr"), _T("localfile"), 29 ,  TRUE},
	{	_T(".3gp")	, kBackKeyValue, _T("mango.3gp"), _T("localfile"), 11,  TRUE },
	{	_T(".3g2")	, kBackKeyValue, _T("mango.3g2"), _T("localfile"), 11,  TRUE },
	{	_T(".3gp2")	, kBackKeyValue, _T("mango.3gp2"), _T("localfile"), 11,  TRUE },
	{	_T(".3gpp")	, kBackKeyValue, _T("mango.3gpp"), _T("localfile"), 11,  TRUE },
	{	_T(".qt")	, kBackKeyValue, _T("mango.qt"), _T("localfile"), 10,  TRUE },
	{	_T(".mov")	, kBackKeyValue, _T("mango.mov"), _T("localfile"), 20,  TRUE },

	//Windows Media 音频/视频文件
	{	_T(".wmv")	, kBackKeyValue, _T("mango.wmv"), _T("localfile"), 12,  TRUE },//5
	{	_T(".wmp")	, kBackKeyValue, _T("mango.wmp"), _T("localfile"), 12,  TRUE },
	{	_T(".wm")	, kBackKeyValue, _T("mango.wm"), _T("localfile"), 12,  TRUE },
	{	_T(".wma")	, kBackKeyValue, _T("mango.wma"), _T("localfile"), 21,  FALSE },
	{	_T(".asf")	, kBackKeyValue, _T("mango.asf"), _T("localfile"), 13,  TRUE},

	//其他视频文件
	{	_T(".amv")	, kBackKeyValue, _T("mango.amv"), _T("localfile"), 1,  TRUE },//28
	{	_T(".dsa")	, kBackKeyValue, _T("mango.dsa"), _T("localfile"), 1,  TRUE },
	{	_T(".dsv")	, kBackKeyValue, _T("mango.dsv"), _T("localfile"), 1,  TRUE },
	{	_T(".dss")	, kBackKeyValue, _T("mango.dss"), _T("localfile"), 1,  TRUE },
	{	_T(".m1v")	, kBackKeyValue, _T("mango.m1v"), _T("localfile"), 1,  TRUE },
	{	_T(".m2v")	, kBackKeyValue, _T("mango.m2v"), _T("localfile"), 1,  TRUE },
	{	_T(".m2p")	, kBackKeyValue, _T("mango.m2p"), _T("localfile"), 1,  TRUE },
	{	_T(".mp2")	, kBackKeyValue, _T("mango.mp2"), _T("localfile"), 1,  FALSE},
	{	_T(".pmp")	, kBackKeyValue, _T("mango.pmp"), _T("localfile"), 1,  TRUE },
	{	_T(".smk")	, kBackKeyValue, _T("mango.smk"), _T("localfile"), 1,  TRUE },
	{	_T(".bik")	, kBackKeyValue, _T("mango.bik"), _T("localfile"), 1,  TRUE },
	{	_T(".rat")	, kBackKeyValue, _T("mango.rat"), _T("localfile"), 1,  TRUE },
	{	_T(".vg2")	, kBackKeyValue, _T("mango.vg2"), _T("localfile"), 1,  TRUE },
	{	_T(".ivf")	, kBackKeyValue, _T("mango.ivf"), _T("localfile"), 1,  TRUE },
	{	_T(".vp6")	, kBackKeyValue, _T("mango.vp6"), _T("localfile"), 1,  TRUE },
	{	_T(".vp7")	, kBackKeyValue, _T("mango.vp7"), _T("localfile"), 1,  TRUE },
	{	_T(".d2v")	, kBackKeyValue, _T("mango.d2v"), _T("localfile"), 1,  TRUE },
	{	_T(".mod")	, kBackKeyValue, _T("mango.mod"), _T("localfile"), 1,  TRUE },
	{	_T(".vid")	, kBackKeyValue, _T("mango.vid"), _T("localfile"), 1,  TRUE },
	{	_T(".pmp2")	, kBackKeyValue, _T("mango.pmp2"), _T("localfile"), 1, TRUE },
	{	_T(".mts")	, kBackKeyValue, _T("mango.mts"), _T("localfile"), 1,  TRUE },
	{	_T(".fli")	, kBackKeyValue, _T("mango.fli"), _T("localfile"), 1,  TRUE },
	{	_T(".flc")	, kBackKeyValue, _T("mango.flc"), _T("localfile"), 1,  TRUE },
	{	_T(".roq")	, kBackKeyValue, _T("mango.roq"), _T("localfile"), 1,  TRUE },
	{	_T(".tim")	, kBackKeyValue, _T("mango.tim"), _T("localfile"), 1,  TRUE },
	{	_T(".csf")	, kBackKeyValue, _T("mango.csf"), _T("localfile"), 1,  TRUE },
	{	_T(".pm2")	, kBackKeyValue, _T("mango.pm2"), _T("localfile"), 1,  TRUE },
	{	_T(".smv")	, kBackKeyValue, _T("mango.smv"), _T("localfile"), 2,  FALSE },
	//{	_T(".qmv")	, kBackKeyValue, _T("mango.qmv"), _T("localfile"), 18,  TRUE },
	//{	_T(".qmvb")	, kBackKeyValue, _T("mango.qmvb"), _T("localfile"), 33,  TRUE },

	//常见音频文件
	{	_T(".mp3")	, kBackKeyValue, _T("mango.mp3"), _T("localfile"), 14,  FALSE },
	{	_T(".ogg")	, kBackKeyValue, _T("mango.ogg"), _T("localfile"), 2,  FALSE },
	{	_T(".cda")	, kBackKeyValue, _T("mango.cda"), _T("localfile"), 2,  FALSE },
	{	_T(".wav")	, kBackKeyValue, _T("mango.wav"), _T("localfile"), 2,  FALSE },
	{	_T(".ape")	, kBackKeyValue, _T("mango.ape"), _T("localfile"), 2,  FALSE },

	//其他音频文件
	{	_T(".au")	, kBackKeyValue, _T("mango.au"), _T("localfile"), 2,  FALSE },
	{	_T(".dts")	, kBackKeyValue, _T("mango.dts"), _T("localfile"), 2,  FALSE },
	{	_T(".midi")	, kBackKeyValue, _T("mango.midi"), _T("localfile"), 2,  FALSE },
	{	_T(".rmi")	, kBackKeyValue, _T("mango.rmi"), _T("localfile"), 2,  FALSE },
	{	_T(".mp5")	, kBackKeyValue, _T("mango.mp5"), _T("localfile"), 2,  FALSE },
	{	_T(".mpa")	, kBackKeyValue, _T("mango.mpa"), _T("localfile"), 2,  FALSE },
	{	_T(".mpga")	, kBackKeyValue, _T("mango.mpga"), _T("localfile"), 2,  FALSE },
	{	_T(".dsm")	, kBackKeyValue, _T("mango.dsm"), _T("localfile"), 2,  FALSE },
	{	_T(".flac")	, kBackKeyValue, _T("mango.flac"), _T("localfile"), 2,  FALSE },
	{	_T(".mac")	, kBackKeyValue, _T("mango.mac"), _T("localfile"), 2,  FALSE },
	{	_T(".tta")	, kBackKeyValue, _T("mango.tta"), _T("localfile"), 2,  FALSE },
	{	_T(".m4a")	, kBackKeyValue, _T("mango.m4a"), _T("localfile"), 2,  FALSE },
	{	_T(".aac")	, kBackKeyValue, _T("mango.aac"), _T("localfile"), 2,  FALSE },
	{	_T(".ac3")	, kBackKeyValue, _T("mango.ac3"), _T("localfile"), 30,  FALSE },
	{	_T(".mid")	, kBackKeyValue, _T("mango.mid"), _T("localfile"), 31,  FALSE },

	//Shockwave Flash
	{	_T(".swf")	, kBackKeyValue, _T("mango.swf"), _T("localfile"), 15,  TRUE },//4
	{	_T(".flv")	, kBackKeyValue, _T("mango.flv"), _T("localfile"), 16,  TRUE },
	{	_T(".m4v")	, kBackKeyValue, _T("mango.m4v"), _T("localfile"), 23 ,  TRUE},
	{	_T(".f4v")	, kBackKeyValue, _T("mango.f4v"), _T("localfile"), 24,  TRUE },

	//播放列表文件
	{	_T(".m3u")	, kBackKeyValue, _T("mango.m3u"), _T("qlistfile"), 17,  TRUE },//8
	{	_T(".wpl")	, kBackKeyValue, _T("mango.wpl"), _T("qlistfile"), 17,  TRUE },
	{	_T(".wmx")	, kBackKeyValue, _T("mango.wmx"), _T("qlistfile"), 17,  FALSE },
	{	_T(".mvx")	, kBackKeyValue, _T("mango.mvx"), _T("qlistfile"), 17,  FALSE },
	{	_T(".ttpl")	, kBackKeyValue, _T("mango.ttpl"), _T("qlistfile"), 17,  TRUE },
	{	_T(".cue")	, kBackKeyValue, _T("mango.cue"), _T("qlistfile"), 17,  TRUE },
	{	_T(".qpl")	, kBackKeyValue, _T("mango.qpl"), _T("qlistfile"), 17,  TRUE },
	{	_T(".asx")	, kBackKeyValue, _T("mango.asx"), _T("qlistfile"), 17,  TRUE },

	//BT 种子文件
	//{	_T(".torrent")	, kBackKeyValue, _T("Qvodbt"), _T("mango.torrent"), 32,  TRUE },//2
	//{	_T(".qsed")	, kBackKeyValue, _T("mango.qsed"), _T("mango.torrent"), 22,  TRUE }
};


extern "C" __declspec(dllexport) BOOL SetMediaConn();

//获取注册表中的关联值
extern "C" __declspec(dllexport) BOOL GetMediaConn(LPCTSTR lpFormat, BOOL bDefault = FALSE);

//设置HKEY_CLASSES_ROOT关联
extern "C" __declspec(dllexport) BOOL SetMediaConn_HClass();

//设置HKEY_CURRENT_USER关联
extern "C" __declspec(dllexport) BOOL SetMediaConn_HCur();

//判断关联键是否被修改
extern "C" __declspec(dllexport) BOOL IsMediaConnChange();
#endif
