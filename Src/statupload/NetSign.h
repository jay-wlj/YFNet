#pragma once
#include "HttpSock.h"
#include <string>
#include <map>
using std::map;
using std::string;

#define NF_STAT_URL "https://stats.lapianapp.com/v1/stats/report"
//#define NF_STAT_URL "https://www.baidu.com"

class CNetSign
{
public:
	CNetSign(std::string url, const std::string& strPostData);

	virtual ~CNetSign(void);
public :
	bool SendStat(std::string url, const std::string &strPost, const int nTimeout);
private:
	void InitSign(const std::string& strUrl);
	std::string GenerateSign(const std::string& strUrl, const std::string& strPostData="");
private:
	std::string GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams);										// 获得URL的host和请求子页面

	std::string SortParams(const std::string& strParams);																									// 参数排序
	std::string GetYFHeadStr();
private:
	std::map<std::string, std::string>	m_mYFHeads ;// X-YF 字段集
	std::string m_strAppKey;		// appkey
};
