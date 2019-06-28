#include "HttpUploadInterface.h"
#include "HttpUpload.h"

CHttpUploadInterface* CHttpUploadInterface::m_instance=NULL;
CHttpUploadInterface::CHttpUploadInterface(void)
{
}

CHttpUploadInterface::~CHttpUploadInterface(void)
{
}
CHttpUploadInterface* CHttpUploadInterface::Instance()
{
	if(m_instance==NULL)
	{
		m_instance= new CHttpUpload();
	}
	return m_instance;
}

void CHttpUploadInterface::FreeInstance()
{
	if (m_instance)
	{
		m_instance->Stop();
	}
	SafeDelete(m_instance);
}