#pragma once
#include <string>
#include <map>

using std::map;
using std::string;

namespace net
{

class CUrl
{
public:
	static void ParseParams(const string& strUrl, map<string, string>& mParams);
};

class CHeadHost
{
public:
	CHeadHost();
	virtual ~CHeadHost();
public:
	void AddHead(const string& strHeadVals);
	void AddHead(const string& strHead, const string& strVal);
	void AddHead(const string& strHead, const int& iVal);
	void GetHeadAdd(std::map<std::string, std::string>& mHeadAdds);
	string GetHeadValue(const std::string& strHead);
	string GetHeadAddString();

protected:
	std::map<std::string, std::string>	m_mYFHeads ;// X-YF 字段集
};

class CNetSign:public CHeadHost
{
public:
	CNetSign();
	virtual ~CNetSign(void);

	void GenerateSign(const string& strUrl, const string& strPostData="");
private:
	void InitSign(const std::string& strUrl);
	std::string GenerateSignString(const std::string& strUrl, const std::string& strPostData="");
	void AddCommHead();		//添加X-YF请求头字段
private:
	std::string GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams);										// 获得URL的host和请求子页面

	std::string SortParams(const std::string& strParams);																									// 参数排序
private:
	std::string m_strAppKey;		// appkey

	std::string m_strUrl;
};

}