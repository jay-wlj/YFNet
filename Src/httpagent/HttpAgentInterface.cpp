#include "HttpAgentInterface.h"
#include "HttpAgent.h"

CHttpAgentInterface* CHttpAgentInterface::m_instance=NULL;
CHttpAgentInterface::CHttpAgentInterface(void)
{
}

CHttpAgentInterface::~CHttpAgentInterface(void)
{
}
CHttpAgentInterface* CHttpAgentInterface::Instance()
{
	if(m_instance==NULL)
		m_instance= new CHttpAgent();
	return m_instance;
}
void CHttpAgentInterface::FreeInstance()
{
	SafeDelete(m_instance);
}