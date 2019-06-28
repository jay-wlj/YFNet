#pragma once
#include <string>
#include <map>

using std::map;
using std::string;

namespace net
{


class CNetSign
{
public:
	CNetSign(string url, int timeout = 10000	);

	virtual ~CNetSign(void);

	void AddHead(const string& strHead, const string& strVal);
	void GetHeadAdd(std::map<std::string, std::string>& mHeadAdds);
	string GetHeadValue(const std::string& strHead);
	void Download();
private:
	void InitSign(const std::string& strUrl);
	std::string GenerateSign(const std::string& strUrl, const std::string& strPostData="");
	void AddCommHead();		//添加X-YF请求头字段
private:
	std::string GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams);										// 获得URL的host和请求子页面

	std::string SortParams(const std::string& strParams);																									// 参数排序
private:
	std::map<std::string, std::string>	m_mYFHeads ;// X-YF 字段集
	std::string m_strAppKey;		// appkey

	std::string m_strUrl;
};

}