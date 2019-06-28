#include "StdAfx.h"
#include "StatUpload.h"
#include "qvod_time.h"
#include "AutoPtr.h"
#include "qvod_sock.h"
#include "NetSign.h"
#include "HttpSSLSock.h"
#include "common-commonfun.h"
#include "json.h"

StatUpload::StatUpload(void):
m_handle(NULL),
m_status(STOP)
{
	m_comVal["pid"] = "lp";
	m_comVal["mi"] = "mi";
	m_comVal["did"] = "did";
	m_comVal["pv"] = "1.6.9";
#ifdef WIN32
	m_comVal["p"] = "pc";
#elif  MACOSX
	m_comVal["p"] = "ios";
#else
	m_comVal["p"] = "android";
#endif 
	//StatInfo info ;
	//info.strPostData = "{\"ca\":0,\"uid\":\"\",\"u\":\"\",\"ui\":\"\",\"ri\":\"\",\"pid\":\"lp\",\"mi\":\"xiaomi\",\"did\":\"SOSLMKjrtt\",\"p\":\"a\",\"pv\":\"1.0.1\",\"t\":1495507337,\"ev\":[{\"i\":\"start_page\",\"l\":null,\"pv\":\"1.0.0\",\"t\":1495507337,\"c\":1,\"d\":0,\"x\":null}]}";
	//m_vStatDatas.push(info);
}


StatUpload::~StatUpload(void)
{
}

bool StatUpload::Run()
{
	if (RUNNING != m_status)
	{
		m_status = RUNNING;
		 QvodCreateThread(&m_handle, (qvod_routine_pt)Routine, this);
	}
	CHttpSSLSock::InitSSLContext();		// SSL初始化
	return true;
}
bool StatUpload::Stop()
{
	m_status = STOP;
	QvodWaitForThread(m_handle, NULL); 
	CHttpSSLSock::UnInitSSLContext();		// SSL初始化
	return true;
}
bool StatUpload::AddStat(STAT_FIELD *stats, bool bAsy, QVOD_DWORD nTimeout)
{
	bool ret = false;
	if (NULL == stats)
	{
		return ret;
	}
	StatInfo info;
	const int nArrLen = sizeof(stats) / sizeof(stats[0]);
	Json::Value data;
	for (int i=0;stats[i].strKey != NULL; i++)
	{
		switch(stats[i].val.vType)
		{
		case net::VT_I4:
			data[stats[i].strKey] = stats[i].val.lVal;
			break;
		case net::VT_UI4:
			data[stats[i].strKey] = stats[i].val.ulVal;
			break;
		case net::VT_UI8:
			data[stats[i].strKey] = stats[i].val.GetString();
			break;
		case net::VT_I8:
			data[stats[i].strKey] = stats[i].val.GetString();
			break;
		case net::VT_PSTR:
			data[stats[i].strKey] = stats[i].val.pVal;
			break;
		default:
			break;
		}
	}

	CAutoLock lock(m_vStatLock);
	// 合并多个事件
	Json::Value root = m_comVal;
	root["t"] = (long)time(NULL);
	root["ev"].append(data);
	//while (m_vStatDatas.empty())
	//{
	//	StatInfo st = m_vStatDatas.front();
	//	m_vStatDatas.pop();

	//	Json::Reader reader;
	//	Json::Value stat;
	//	if (reader.parse(m_vStatDatas[i].strPostData, stat))
	//	{
	//		if (stat.isObject() && stat.isMember("ev"))
	//		{
	//			if (stat["ev"].isObject())
	//			{
	//				root["ev"].append(stat["ev"]);
	//			}
	//			else if (stat["ev"].isArray())
	//			{
	//				root["ev"].append(stat["ev"]);
	//			}
	//		}
	//	}
	//}
	
	Json::FastWriter writer;
	
	std::string strPost = writer.write(root);
	info.strPostData = strPost;
	m_vStatDatas.push(info);

	return true;
}

void StatUpload::SetCommonStat(Json::Value &data)
{
	m_comVal = data;
#ifdef WIN32
	m_comVal["p"] = "pc";
#elif  MACOSX
	m_comVal["p"] = "ios";
#else
	m_comVal["p"] = "android";
#endif 
#ifdef QVODDEBUG
	Json::FastWriter writer;
	std::string str = writer.write(m_comVal);
	_OUTPUT(INFO, "StatUpload::SetCommonStat %s", str.c_str());
#endif
}

void StatUpload::AddStat(const StatInfo& info)
{
	CAutoLock lock(m_vStatLock);
	m_vStatDatas.push(info);	
}

bool StatUpload::AddStat(Json::Value &data)
{
	CAutoLock lock(m_vStatLock);
	// 合并多个事件
	Json::Value root = m_comVal;
	root["t"] = (long)time(NULL);
	root["ev"].append(data);
	
	Json::FastWriter writer;
	StatInfo info;
	std::string strPost = writer.write(root);
	info.strPostData = strPost;
	m_vStatDatas.push(info);
	return true;
}

QVOD_THREAD_RETURN StatUpload::Routine(void* param)
{
	StatUpload *pThis = reinterpret_cast<StatUpload*>(param);
	while (pThis && RUNNING==pThis->m_status)
	{
		pThis->OnRoutine();
		QvodSleep(50);
	}
}

void StatUpload::OnRoutine()
{
	while (RUNNING == m_status)
	{
		StatInfo info;
		if (RecvStatInfo(info))
		{
			if (SendStat(info))
			{
				// 发送成功

			}
			else
			{
				// 发送失败，重新发送
				info.retrys ++;
				if (info.retrys < m_nMaxRetrys)
				{
					AddStat(info);
				}
				else
				{
#ifdef QVODDEBUG
				_OUTPUT(INFO, "StatUpload SendStat failed! url=%s", info.strUrl.c_str());
#endif		
				}
			}
		}
		QvodSleep(50);
	}
}

bool StatUpload::RecvStatInfo(StatInfo &info)
{
	bool ret = false;
	CAutoLock lock(m_vStatLock);
	if (! m_vStatDatas.empty())
	{
		info = m_vStatDatas.front();
		m_vStatDatas.pop();
		ret = true;
	}
	return ret;
}

bool StatUpload::SendStat(const StatInfo &info)
{
	bool ret = false;
	string url = info.strUrl;
	url = url.empty()? NF_STAT_URL: url;
	//string str = "{\"ca\":0,\"uid\":\"\",\"u\":\"\",\"ui\":\"\",\"ri\":\"\",\"pid\":\"lp\",\"mi\":\"xiaomi\",\"did\":\"SOSLMKjrtt\",\"p\":\"a\",\"pv\":\"1.0.1\",\"t\":1495507337,\"ev\":[{\"i\":\"start_page\",\"l\":null,\"pv\":\"1.0.0\",\"t\":1495507337,\"c\":1,\"d\":0,\"x\":null}]}";
	CNetSign netsign(url, info.strPostData);
	
	ret = netsign.SendStat(url, info.strPostData, 5000);
	
	return ret;
}