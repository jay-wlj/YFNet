#ifndef _DNRULES_H
#define _DNRULES_H
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include <string>
#include <map>
#include <set>
#include "qvod_lock.h"
enum DNRULE
{
	DNRULE_ACCEPT = 0,//只对IP范围内的接受
	DNRULE_EXCEPT//只对IP范围内的拒绝
};

class CDNRules
{
private:
	BOOL m_switch;
	DNRULE m_rule;
	std::set<std::string> m_setaccept;
	std::set<std::string> m_setexcept;
	std::map<std::string,int> m_DNcount;
	CLock m_lock;
	time_t m_lasttime;//上次统计时间
	
	BOOL m_updateflag;
public:
	CDNRules();
	~CDNRules();
	void SetSwitch(BOOL ruleswitch){m_switch = ruleswitch;}//设置条件开关
	BOOL CanAccept(std::string& dn);//判断一个域名地址是否被允许
	BOOL ReloadRules();//载入IP规则文件
	BOOL bEnableDnRules(){return m_switch;}
	DNRULE GetRule(){return m_rule;}
private:
	BOOL ProcessExcept(char*& pstr);
	BOOL ProcessAccept(char*& pstr);
};

#endif