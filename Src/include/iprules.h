#ifndef _IPRULES_H
#define _IPRULES_H
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include <string>
#include <vector>
#include <set>
#include "qvod_lock.h"
enum RULE
{
	IPRULE_ACCEPT = 0,//只对IP范围内的接受
	IPRULE_EXCEPT//只对IP范围内的拒绝
};
struct range
{
	UINT lower;
	UINT higher;
};
class CIPRules
{
private:
	BOOL m_switch;
	RULE m_rule;
	std::vector<range> m_vctaccept;
	std::vector<range> m_vctexcept;
	std::set<UINT> m_setaccept;
	CLock m_lock;
public:
	CIPRules();
	~CIPRules();
	void SetSwitch(BOOL ruleswitch){m_switch = ruleswitch;}//设置条件开关
	BOOL CanAccept(UINT ip);//判断一个IP地址是否被允许
	BOOL ReloadRules();//载入IP规则文件
	void LoadIsp();
private:
	BOOL ProcessExcept(char*& pstr);
	BOOL ProcessAccept(char*& pstr);
};
#endif