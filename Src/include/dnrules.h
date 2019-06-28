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
	DNRULE_ACCEPT = 0,//ֻ��IP��Χ�ڵĽ���
	DNRULE_EXCEPT//ֻ��IP��Χ�ڵľܾ�
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
	time_t m_lasttime;//�ϴ�ͳ��ʱ��
	
	BOOL m_updateflag;
public:
	CDNRules();
	~CDNRules();
	void SetSwitch(BOOL ruleswitch){m_switch = ruleswitch;}//������������
	BOOL CanAccept(std::string& dn);//�ж�һ��������ַ�Ƿ�����
	BOOL ReloadRules();//����IP�����ļ�
	BOOL bEnableDnRules(){return m_switch;}
	DNRULE GetRule(){return m_rule;}
private:
	BOOL ProcessExcept(char*& pstr);
	BOOL ProcessAccept(char*& pstr);
};

#endif