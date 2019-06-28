#include "NetSign.h"
#include "sha1.h"
#include <Windows.h>
#include <algorithm>
#include <vector>
using std::vector;


using namespace net;

CNetSign::CNetSign(string url, 	int timeout)
{
	m_strUrl = url;
	InitSign(m_strUrl);
}

void CNetSign::Download()
{
	GenerateSign(m_strUrl);
	AddCommHead();
}

void CNetSign::AddCommHead()
{
	for (std::map<std::string, std::string>::const_iterator itr=m_mYFHeads.begin(); itr!=m_mYFHeads.end(); itr++)
	{
		//SetRequestInfo(itr->first.c_str(), itr->second.c_str());
	}
}

void CNetSign::InitSign(const std::string& strUrl)
{
	DWORD dwTick = GetTickCount() + rand();
	char szBuf[20] ={0};
	sprintf(szBuf, "%d", dwTick);
	string strRid = szBuf;

	AddHead("X-YF-AppId", "wemedia");
	AddHead("X-YF-Rid", strRid);
	string strPlatform = "pc";
#ifdef WIN32
	strPlatform = "pc";
#elif MACOSX
	strPlatform = "ios";
#else 
	strPlatform = "android";
#endif

	AddHead("X-YF-Platform", strPlatform);
	AddHead("X-YF-Version", "1.2.0");

	m_strAppKey = "f03ff699cd1f6ba6e18a450dbb7a504d";

	std::string strMagic = "moO01lo0l";
	std::string strToken = "TX";
	CSHA1 sha(GetHeadValue("X-YF-AppId").c_str(), GetHeadValue("X-YF-AppId").length());
	strToken += sha.readHex();
	strMagic += strToken;
	CSHA1 sha1(strMagic.c_str(), strMagic.length());
	std::string strSum = sha1.readHex();
	strSum = strSum.substr(0, 3);
	strToken = strToken + strSum;

	AddHead("X-YF-Token", strToken);
}


CNetSign::~CNetSign(void)
{
}

void CNetSign::AddHead(const string& strHead, const string& strVal)
{
	if (strHead.empty())
	{
		return;
	}
	string strLowerHead = strHead;
	transform(strLowerHead.begin(), strLowerHead.end(), strLowerHead.begin(), ::tolower);
	m_mYFHeads[strLowerHead] = strVal;
}

string CNetSign::GetHeadValue(const std::string& strHead)
{
	string strLowerHead = strHead;
	transform(strLowerHead.begin(), strLowerHead.end(), strLowerHead.begin(), ::tolower);
	return m_mYFHeads[strLowerHead];
}

void CNetSign::GetHeadAdd(std::map<std::string, std::string>& mHeadAdds)
{
	mHeadAdds = m_mYFHeads;
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
	
	string str;
	for (MAP::iterator itr=mParams.begin(); itr!=mParams.end(); itr++)
	{
		vector<std::string> &vals = itr->second;
		std::sort(vals.begin(), vals.end(), std::less<std::string>());

		int nSize = vals.size();
		for (int i=0; i<nSize; i++)
		{
			if (! str.empty())
			{
				str += "&";
			}
			str += itr->first;
			str += "=";
			str += vals[i];
		}
	}

	return str;
}

std::string CNetSign::GenerateSign(const std::string& strUrl, const std::string& strPostData)
{
	std::string strHost;
	std::string strParams;
	std::string strSubUrl = GetURLHost(strUrl, strHost, strParams);

	strParams = SortParams(strParams);		// sort params


	std::string strSign;
	strSign += strSubUrl;		// <CanonicalURI>\n
	strSign += "\n";
	strSign += strParams;																		// <CanonicalArgs>\n
	strSign += "\n";
	
	string strXYF;
	string strYFHeads;
	for (std::map<std::string, std::string>::const_iterator itr=m_mYFHeads.begin(); itr!=m_mYFHeads.end(); itr++)
	{
		string strHead = itr->first.c_str();
		//strHead.MakeLower();
		strYFHeads += strHead;
		strYFHeads += ";";

		strXYF += strHead;
		strXYF +=":";
		strXYF += itr->second;
		strXYF += "\n";
		//strXYF.AppendFormat("%s: %s\n", strHead, itr->second.c_str());
	}
	strYFHeads.erase(strYFHeads.rfind(";"));

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