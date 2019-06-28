#pragma once

namespace qurl{

const char kTopLevelDomainArray[][20] =
{
	".com",
	".co",
	".info",
	".net",
	".org",
	".me",
	".mobi",
	".us",
	".biz",
	".xxx",
	".ca",
	".co.jp",
	".com.cn",
	".net.cn",
	".org.cn",
	".mx",
	".tv",
	".ws",
	".ag",
	".com.ag",
	".net.ag",
	".org.ag",
	".am",
	".asia",
	".at",
	".be",
	".com.br",
	".net.br",
	".bz",
	".com.bz",
	".net.bz",
	".cc",
	".com.co",
	".net.co",
	".nom.co",
	".de",
	".es",
	".com.es",
	".nom.es",
	".org.es",
	".eu",
	".fm",
	".fr",
	".gs",
	".in",
	".co.in",
	".firm.in",
	".gen.in",
	".ind.in",
	".net.in",
	".org.in",
	".it",
	".jobs",
	".jp",
	".ms",
	".com.mx",
	".nl",
	".nu",
	".co.nz",
	".net.nz",
	".org.nz",
	".se",
	".tc",
	".tk",
	".tw",
	".com.tw",
	".idv.tw",
	".org.tw",
	".hk",
	".co.uk",
	".me.uk",
	".org.uk",
	".vg",
};


class Tool
{
public:
	static std::string GetDomain(const std::string &url)
	{
		std::string domain = url;
		int pos = domain.find("www.");
		if(pos != std::string::npos)
		{
			domain = domain.substr(pos+4);
		}

		pos =  domain.find("http://");
		if(pos != std::string::npos)
		{
			domain = domain.substr(pos+7);
		}

		pos = domain.find('/');
		if(pos != std::string::npos)
		{
			domain = domain.substr(0,pos);
		}
		return domain;
	}
	static std::string  GetTopLevelDomain(const std::string &url)
	{
		std::string tld = GetDomain(url);
		if(tld.size() > 0)
		{	
			int headPos = 0;
			int count = sizeof(kTopLevelDomainArray)/20;
			for(int i = 0; i < count ; i++)
			{
				std::string tmp = kTopLevelDomainArray[i];
				int pos = tld.rfind(tmp);
				if(pos != std::string::npos && pos == tld.size() - tmp.size())
				{
					pos = tld.rfind('.',pos-1);
					if(pos != std::string::npos)
					{
						tld.erase(headPos, pos-headPos+1);
					}
					break;
				}
			}
		}
		return tld;
	}
	static std::string GetHost(const std::string &url)
	{
		std::string host = url;
		int pos = host.find("://");
		if(pos > 0)
		{
			host = url.substr(pos+3);
		}
		else
		{
			pos = 0;
		}

		pos = host.find('/', pos+3);
		if(pos != std::string::npos)
		{
			host = host.substr(0,pos);
		}
		return host;
	}

	static int GetAddrFromHost(LPCTSTR strHost , CString& strIp)
	{
		std::string str = Fun::UnicodeToANSI(strHost);
		str = GetHost(str);
		int nPos = str.find(":");
		if (nPos != std::string::npos)
		{
			str = str.substr(0, nPos);
		}
		hostent* phost = gethostbyname(str.c_str());
		if (!phost)
		{
			strIp = Fun::ANSIToUnicode(str);
			return -1;
		}
		strIp.Format(_T("%d.%d.%d.%d"), phost->h_addr[0]&0x00ff, phost->h_addr[1]&0x00ff, phost->h_addr[2]&0x00ff, phost->h_addr[3]&0x00ff);
		return 0;
	}
};

} // namespace url