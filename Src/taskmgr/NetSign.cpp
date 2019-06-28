#include "NetSign.h"
#include "sha1.h"
#include "qvod_time.h"
#include <algorithm>
#include <vector>
using std::vector;


using namespace net;

void CUrl::ParseParams(const string& strUrl, map<string, string>& mParams)
{
	const char *pStart = strstr(strUrl.c_str(), "?");
	if (NULL == pStart)
	{
		return;
	}
	pStart += 1;
	const char *pEnd = strstr(pStart, "=");
	while (pStart && pEnd)
	{
		string strKey(pStart, pEnd-pStart);
		pEnd ++;
		string strVal;
		pStart = strstr(pEnd, "&");
		if (NULL == pStart)
		{
			strVal = pEnd;
		}
		else
		{
			strVal.assign(pEnd, pStart-pEnd);
			pStart ++;
			pEnd = strstr(pStart, "=");
		}
		mParams[strKey] = strVal;
	}
}

CHeadHost::CHeadHost()
{

}
CHeadHost::~CHeadHost()
{
	m_mYFHeads.clear();
}

void CHeadHost::AddHead(const string& strHeadVals)
{
	const char *p = strHeadVals.c_str();
	const char *pEnd = strstr(p,":");
	while (p && pEnd)
	{
		string strHead(p, pEnd-p);
		p = pEnd+1;

		string strVal;
		pEnd = strstr(p, "\n");
		if (p && pEnd)
		{
			strVal.assign(p, pEnd-p);
			p = pEnd+1;
		}
		else
		{
			strVal.assign(p);
		}
		strVal = strVal.find_first_not_of(' ');	// 去掉前面的空格

		if (! strHead.empty())
		{
			AddHead(strHead, strVal);
		}
	}	
}
void CHeadHost::AddHead(const string& strHead, const string& strVal)
{
	if (strHead.empty())
	{
		return;
	}
	string strLowerHead = strHead;
	transform(strLowerHead.begin(), strLowerHead.end(), strLowerHead.begin(), ::tolower);		// head头统一转成小写
	m_mYFHeads[strLowerHead] = strVal;	
}
void CHeadHost::AddHead(const string& strHead, const int& iVal)
{
	char szBuf[20] = {0};
	sprintf(szBuf, "%d", iVal);
	AddHead(strHead, szBuf);	
}
void CHeadHost::GetHeadAdd(std::map<std::string, std::string>& mHeadAdds)
{
	mHeadAdds = m_mYFHeads;
}

string CHeadHost::GetHeadValue(const std::string& strHead)
{
	string strLowerHead = strHead;
	transform(strLowerHead.begin(), strLowerHead.end(), strLowerHead.begin(), ::tolower);		// head转成小写
	return m_mYFHeads[strLowerHead];
}

string CHeadHost::GetHeadAddString()
{
	string strRet;
	for (map<string, string>::iterator itr=m_mYFHeads.begin(); itr!= m_mYFHeads.end(); itr++)
	{
		strRet += itr->first;
		strRet += ": ";
		strRet += itr->second;
		strRet += "\r\n";
	}
	return strRet;
}
CNetSign::CNetSign()
{
	m_strAppKey = "f03ff699cd1f6ba6e18a450dbb7a504d";
	AddHead("X-YF-AppId", "wemedia");
	AddHead("X-YF-Platform", "pc");
	AddHead("X-YF-Version", "1.1.1.1");

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

void CNetSign::GenerateSign(const string& strUrl, const string& strPostData)
{
	m_strUrl = strUrl;
	InitSign(m_strUrl);
	GenerateSignString(m_strUrl, strPostData);
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
	QVOD_DWORD dwTick = QvodGetTime() + rand();
	char szBuf[20] ={0};
	sprintf(szBuf, "%d", dwTick);
	string strRid = szBuf;

	AddHead("X-YF-Rid", strRid);
}


CNetSign::~CNetSign(void)
{
}

std::string CNetSign::GetURLHost(const std::string &strURL, std::string &strHost, std::string &strParams)										// 获得URL的host和请求子页面
{
	std::string strRet;
	std::string str = strURL;
	if (strstr(strURL.c_str(), "http://"))
	{
		str = strURL.substr(strlen("http://"));
	}
	else if (strstr(strURL.c_str(), "https://"))
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
	typedef std::map<std::string, vector<std::string> > MAP;
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

std::string CNetSign::GenerateSignString(const std::string& strUrl, const std::string& strPostData)
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
	// 先剔除掉x-yf-sign
	std::map<std::string, std::string>::iterator itr = m_mYFHeads.find("x-yf-sign");
	if (itr != m_mYFHeads.end())
	{
		m_mYFHeads.erase(itr);
	}
	for (std::map<std::string, std::string>::const_iterator itr=m_mYFHeads.begin(); itr!=m_mYFHeads.end(); itr++)
	{
		string strHead = itr->first.c_str();
		if (0 != strHead.find("x-yf-"))		// 只对x-yf-字段签名
		{
			continue;	
		}
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

	AddHead("x-yf-sign", strSign);
	//m_mYFHeads["X-YF-Sign"] = strSign;
	return strSign;
}