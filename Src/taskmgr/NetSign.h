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
	std::map<std::string, std::string>	m_mYFHeads ;// X-YF �ֶμ�
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
	void AddCommHead();		//���X-YF����ͷ�ֶ�
private:
	std::string GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams);										// ���URL��host��������ҳ��

	std::string SortParams(const std::string& strParams);																									// ��������
private:
	std::string m_strAppKey;		// appkey

	std::string m_strUrl;
};

}