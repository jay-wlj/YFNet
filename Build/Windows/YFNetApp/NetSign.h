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
	void AddCommHead();		//���X-YF����ͷ�ֶ�
private:
	std::string GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams);										// ���URL��host��������ҳ��

	std::string SortParams(const std::string& strParams);																									// ��������
private:
	std::map<std::string, std::string>	m_mYFHeads ;// X-YF �ֶμ�
	std::string m_strAppKey;		// appkey

	std::string m_strUrl;
};

}