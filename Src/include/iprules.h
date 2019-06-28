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
	IPRULE_ACCEPT = 0,//ֻ��IP��Χ�ڵĽ���
	IPRULE_EXCEPT//ֻ��IP��Χ�ڵľܾ�
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
	void SetSwitch(BOOL ruleswitch){m_switch = ruleswitch;}//������������
	BOOL CanAccept(UINT ip);//�ж�һ��IP��ַ�Ƿ�����
	BOOL ReloadRules();//����IP�����ļ�
	void LoadIsp();
private:
	BOOL ProcessExcept(char*& pstr);
	BOOL ProcessAccept(char*& pstr);
};
#endif