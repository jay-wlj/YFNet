#include "NetSign.h"
#include "../share/sha1.h"
#include "../include/mytool.h"
#include <algorithm>

using std::vector;


using namespace net;

CNetSign::CNetSign(CString url, 	int timeout):
net::HttpDownloader(url, timeout)
{
	std::string strUrl = Fun::UnicodeToANSI(url);
	InitSign(strUrl);
	GenerateSign(strUrl);
	AddCommHead();
}
CNetSign::CNetSign(CString url, CString lastModify, int timeout):
net::HttpDownloader(url, lastModify, timeout)
{
	std::string strUrl = Fun::UnicodeToANSI(url);
	InitSign(strUrl);
	GenerateSign(strUrl);
	AddCommHead();
}
CNetSign::CNetSign(CString url, std::string postdata, int timeout, net::PostType pType):
net::HttpDownloader(url, postdata, pType)
{
	std::string strUrl = Fun::UnicodeToANSI(url);
	InitSign(strUrl);
	GenerateSign(strUrl, postdata);
	AddCommHead();
}

void CNetSign::AddCommHead()
{
	for (std::map<std::string, std::string>::const_iterator itr=m_mYFHeads.begin(); itr!=m_mYFHeads.end(); itr++)
	{
		SetRequestInfo(itr->first.c_str(), itr->second.c_str());
	}
}

void CNetSign::InitSign(const std::string& strUrl)
{
	DWORD dwTick = GetTickCount() + rand();
	CStringA strRid;
	strRid.Format("%d", dwTick);
	m_mYFHeads["X-YF-AppId"] = "api_v1";
	m_mYFHeads["X-YF-Rid"] = strRid;
	m_mYFHeads["X-YF-Platform"] = "pc";
	m_mYFHeads["X-YF-Version"] =  Fun::UnicodeToANSI(MyTool::GetPlayerVersion());
	m_strAppKey = "1c558dc045fc4fbf91053858be63a2cd";

	std::string strMagic = "moO01lo0l";
	std::string strToken = "TX";
	CSHA1 sha(m_mYFHeads["X-YF-AppId"].c_str(), m_mYFHeads["X-YF-AppId"].length());
	strToken += sha.readHex();
	strMagic += strToken;
	CSHA1 sha1(strMagic.c_str(), strMagic.length());
	std::string strSum = sha1.readHex();
	strSum = strSum.substr(0, 3);
	strToken = strToken + strSum;

	m_mYFHeads["X-YF-Token"] = strToken;
}


CNetSign::~CNetSign(void)
{
}

std::string CNetSign::GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams)										// 获得URL的host和请求子页面
{
	std::string strRet;
	std::string str = strURL;
	if (0 == strnicmp(strURL.c_str(), "http://", strlen("http://")))
	{
		str = strURL.substr(strlen("http://"));
	}
	else 	if (0 == strnicmp(strURL.c_str(), "https://", strlen("https://")))
	{
		str = strURL.substr(strlen("https://"));
	}

	int nSubURL = str.find("/");
	if(nSubURL < 0)
	{
		strHost = str;
		strRet = "/";
	}
	else
	{
		strHost = str.substr(0, nSubURL);
		strRet = str.substr(nSubURL);
	}
	int nParamsPos = strRet.find("?");
	if (nParamsPos != std::string::npos)
	{
		strParams = strRet.substr(nParamsPos+1);
		strRet = strRet.substr(0, nParamsPos);
	}
	return strRet;
}


std::string CNetSign::SortParams(const std::string& strParams)
{
	typedef std::map<std::string, vector<std::string>> MAP;
	MAP mParams;
	char *pStr = (char*)strParams.c_str();
	while (pStr)
	{
		char *pVal = strstr(pStr, "=");
		if (! pVal)
		{
			break;
		}

		std::string strKey(pStr, pVal-pStr);
		pVal += 1;
		pStr = strstr(pVal, "&");
		std::string strVal;
		if (pStr)
		{
			strVal.assign(pVal, pStr-pVal);
			pStr += 1;
		}
		else
		{
			strVal = pVal;
		}
		vector<std::string> &vals = mParams[strKey];
		vals.push_back(strVal);
	}
	
	CStringA str;
	for (MAP::iterator itr=mParams.begin(); itr!=mParams.end(); itr++)
	{
		vector<std::string> &vals = itr->second;
		std::sort(vals.begin(), vals.end(), std::less<std::string>());

		int nSize = vals.size();
		for (int i=0; i<nSize; i++)
		{
			str.AppendFormat("%s=%s&", itr->first.c_str(), vals[i].c_str());
		}
	}
	str.Trim("&");
	return str;
}

std::string CNetSign::GenerateSign(const std::string& strUrl, const std::string& strPostData)
{
	std::string strHost;
	std::string strParams;
	std::string strSubUrl = GetURLHost(strUrl, strHost, strParams);

	strParams = SortParams(strParams);		// sort params


	std::string strSign;
	strSign += Fun::URLEncode(strSubUrl.c_str(), strSubUrl.length(), false);		// <CanonicalURI>\n
	strSign += "\n";
	strSign += strParams;																		// <CanonicalArgs>\n
	strSign += "\n";
	
	CStringA strXYF;
	CStringA strYFHeads;
	for (std::map<std::string, std::string>::const_iterator itr=m_mYFHeads.begin(); itr!=m_mYFHeads.end(); itr++)
	{
		CStringA strHead = itr->first.c_str();
		strHead.MakeLower();
		strYFHeads += strHead;
		strYFHeads += ";";

		strXYF.AppendFormat("%s:%s\n", strHead, itr->second.c_str());
	}
	strYFHeads.Trim(";");
	strSign += strXYF;
	strSign += strYFHeads;
	strSign += "\n";

	CSHA1 sha(strPostData.c_str(), strPostData.length());
	std::string strHex = sha.readHex();

	strSign += strHex;																				// <Sha1Body>\n
	strSign += "\n";

	strSign += m_strAppKey;

	
	CSHA1 sha1(strSign.c_str(), strSign.length());
	strSign = sha1.readHex();

	m_mYFHeads["X-YF-Sign"] = strSign;
	return strSign;
}