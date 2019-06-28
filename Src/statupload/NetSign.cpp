#include "NetSign.h"
#include "../include/openssl/sha.h"
#include "../Common/common-utility.h"
#include "../Common/common-commonfun.h"
#include "HttpSock.h"
#include "HttpSSLSock.h"
#include "../Common/AutoPtr.h"
#include "../Common/qvod_error.h"
#include "NetUrl.h"

using std::vector;


CNetSign::CNetSign(std::string url, const std::string& strPostData)
{
	std::string strUrl = url;
	InitSign(strUrl);
	GenerateSign(strUrl, strPostData);
}

void CNetSign::InitSign(const std::string& strUrl)
{
	DWORD dwTick = GetTickCount() + rand();
	std::string strRid;
	char szRid[20] = {0};
	sprintf(szRid, "%d", dwTick);
	strRid = szRid;
	m_mYFHeads["X-Yf-Appid"] = "stats";
	m_mYFHeads["X-Yf-User"] = "KvJMSGtDBrJYeZYkdiVnZlDKuQbxNmXT";
	m_mYFHeads["X-Yf-Uid"] = "1";
	m_mYFHeads["X-Yf-Req-Id"] = "RatomaQAHyU.f54";
	//m_mYFHeads["X-YF-Rid"] = strRid;
	//m_mYFHeads["X-YF-Platform"] = "pc";
	//m_mYFHeads["X-YF-Version"] =  Fun::UnicodeToANSI(MyTool::GetPlayerVersion());
	m_strAppKey = "eaca247989777582ae37e23d7dbb402b";

	//std::string strMagic = "moO01lo0l";
	//std::string strToken = "TX";
	//unsigned char szSha1[50] = {0};
	//string strAppid = m_mYFHeads["X-YF-AppId"];
	//SHA1((const unsigned char*)strAppid.c_str(), strAppid.length(), szSha1);
	//strToken += Hash2Char(szSha1);
	//strMagic += strToken;
	//SHA1((const unsigned char*)strMagic.c_str(), strMagic.length(), szSha1);
	//std::string strSum = Hash2Char(szSha1);
	//strSum = strSum.substr(0, 3);
	//strToken = strToken + strSum;

	//m_mYFHeads["X-YF-Token"] = strToken;
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
	
	std::string str;
	for (MAP::iterator itr=mParams.begin(); itr!=mParams.end(); itr++)
	{
		vector<std::string> &vals = itr->second;
		std::sort(vals.begin(), vals.end(), std::less<std::string>());

		int nSize = vals.size();
		for (int i=0; i<nSize; i++)
		{
			str += itr->first;
			str += "=";
			str += vals[i];
			str += "&";
		}
	}
	int nPos = str.rfind('&');
	if (nPos != string::npos && nPos ==str.length()-1)
	{
		str.erase(str.begin()+nPos);
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
	
	std::string strXYF;
	std::string strYFHeads;
	for (std::map<std::string, std::string>::const_iterator itr=m_mYFHeads.begin(); itr!=m_mYFHeads.end(); itr++)
	{
		std::string strHead = itr->first.c_str();
		transform(strHead.begin(), strHead.end(), strHead.begin(), ::tolower);
		strYFHeads += strHead;
		strYFHeads += ";";

		strXYF += strHead;
		strXYF += ":";
		strXYF += itr->second;
		strXYF += "\n";
		//strXYF.AppendFormat("%s:%s\n", strHead, itr->second.c_str());
	}
	string_trim(strYFHeads, ";");
	strSign += strXYF;
	strSign += strYFHeads;
	strSign += "\n";

	unsigned char szSha1[50] = {0};
	SHA1((const unsigned char*)strPostData.c_str(), strPostData.length(), szSha1);
	std::string strHex = Hash2Char(szSha1);
	transform(strHex.begin(), strHex.end(), strHex.begin(), ::tolower);

	strSign += strHex;																				// <Sha1Body>\n
	strSign += "\n";

	strSign += m_strAppKey;

	SHA1((const unsigned char*)strSign.c_str(), strSign.length(), szSha1);
	strSign = Hash2Char(szSha1);
	transform(strSign.begin(), strSign.end(), strSign.begin(), ::tolower);
	m_mYFHeads["X-YF-Sign"] = strSign;
	return strSign;
}

bool CNetSign::SendStat(std::string url, const std::string &strPost, const int nTimeout)
{
	url = url.empty()? NF_STAT_URL: url;
	bool ret = false;
	
	string strRet;
	NetUrl net(url);
	net.SetHeaderString(GetYFHeadStr());
	try {
		ret = net.PostRequest(strPost, strRet);
		if (! ret)
		{
	#ifdef QVODDEBUG
			_OUTPUT(INFO, "CNetSign err!");
	#endif
		}
	}	catch (...) {
	#ifdef QVODDEBUG
			_OUTPUT(INFO, "CNetSign catch!");
	#endif		
	}

	return ret;
}


std::string CNetSign::GetYFHeadStr()
{
	std::string strBuff;
	for (std::map<std::string, std::string>::const_iterator itr=m_mYFHeads.begin(); itr!=m_mYFHeads.end(); itr++)
	{
		strBuff += itr->first.c_str();
		strBuff += ": ";
		strBuff += itr->second.c_str();
		strBuff += "\r\n";
	}
	return strBuff;
}