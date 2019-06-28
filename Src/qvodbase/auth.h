#ifdef _DEBUG
#pragma once

class CAuth
{
public:
	CAuth(void);
	virtual ~CAuth(void);
	static CAuth* Instance();
	static void FreeInstance();
	static CAuth* m_instance;
	bool IsAble();
private:
	int m_isAble;
};
#endif