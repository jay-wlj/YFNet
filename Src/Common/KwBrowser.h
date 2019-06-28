#pragma once
#include "..\Common\File.h"
#include "..\share\StringCode.h"
#include <string>
#include <ShellAPI.h>
#include "..\include\json\json.h"
//#ifdef _DEBUG
//#pragma comment(lib,"../../lib/json_mtd_d.lib")
//#else
//#pragma comment(lib,"../../lib/json_mtd.lib")
//#endif

namespace kuaiwan{

class KWBrowser
{
public:
	KWBrowser(std::string &sn, std::string &ds, std::string &ver)
		:m_ver(ver),m_ds(ds),m_sn(sn)
	{

	}
	BOOL OpenUrl(const std::string &url)
	{
		std::string processPath = ReadProcessPathFromReg();
		if(QFile::Tool::IsFileExist(processPath))
		{
			std::string strPara = MakeFromKuaiboPara(url);	
			ShellExecuteA( 0, "open", processPath.c_str(), strPara.c_str(), 0, SW_SHOW);
			return TRUE;
		}
		return FALSE;
	}
public:
	// 从注册表读取快玩浏览器进程路径
	static std::string ReadProcessPathFromReg(void)
	{
		const char kYFBrowserRegKey[] =  "SOFTWARE\\yfe";
		char szPath[MAX_PATH] = {0};
		HKEY hKey = NULL;
		RegOpenKeyExA(HKEY_CURRENT_USER, kYFBrowserRegKey, 0, KEY_READ, &hKey);
		if ( hKey )
		{
			DWORD dwSize = sizeof(char)*MAX_PATH;
			DWORD dwType = REG_SZ;
			RegQueryValueExA(hKey, "exe_path", 0, &dwType, (BYTE*)szPath, &dwSize);
			RegCloseKey(hKey);
			return szPath;
		}

		const char kKWBrowserRegKey[] =  "Software\\KWExplorer";
		RegOpenKeyExA(HKEY_CURRENT_USER, kKWBrowserRegKey, 0, KEY_READ, &hKey);
		if ( hKey )
		{
			DWORD dwSize = sizeof(char)*MAX_PATH;
			DWORD dwType = REG_SZ;
			RegQueryValueExA(hKey, "exe_path", 0, &dwType, (BYTE*)szPath, &dwSize);
			RegCloseKey(hKey);
		}
		return szPath;
	}
private:
	// 快玩浏览器的启动参数
	// 格式：--kwe-run-from=jsonstring -- url
	// jsonstring格式如下为 "{"sn":"kb", "ds":"快播媒体库","ver":"1.0.0.0"}"
	// sn: 软件名称 播放器 kb_player 资讯快播界面kb_zx
	// ds: 跳转来源 
	// ver:软件版本
	// 跳转说明：
	//    快播媒体库：    meida
	//    插件弱提示:     weakhint	
	//    咨询快播的链接：link
	std::string MakeFromKuaiboPara(const std::string &url)
	{
		Json::FastWriter fastWriter;
		Json::Value val;
		val["sn"] = m_sn;
		val["ds"] = m_ds;
		val["ver"] = m_ver;
		std::string json = fastWriter.write(val);
		std::string json_base = Base64::base64_encode(json.c_str(),json.size());
		std::string para = "--kwe-run-from=" + json_base;
		para += " -- " + url;
		return  para;
	}
private:
	std::string m_ver;
	std::string m_sn;
	std::string m_ds;
};

}