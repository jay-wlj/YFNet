#pragma once
#include <string>
#include <map>

using std::map;
using std::string;

namespace net
{


class CNetSign:public net::HttpDownloader
{
public:
	CNetSign(CString url, int timeout = 10000	);
	CNetSign(CString url, CString lastModify, int timeout = 10000);
	CNetSign(CString url, std::string postdata, int timeout = 10000, net::PostType pType = net::kDefaultPost);

	virtual ~CNetSign(void);
private:
	void InitSign(const std::string& strUrl);
	std::string GenerateSign(const std::string& strUrl, const std::string& strPostData="");
	void AddCommHead();		//���X-YF����ͷ�ֶ�
private:
	std::string GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams);										// ���URL��host��������ҳ��

	std::string SortParams(const std::string& strParams);																									// ��������
private:
	std::map<std::string, std::string>	m_mYFHeads ;// X-YF �ֶμ�
	std::string m_strAppKey;		// appkey
};

}