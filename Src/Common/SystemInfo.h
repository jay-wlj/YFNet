#pragma once

#include <Windows.h>

class CSysteminfo
{
public:
	static CSysteminfo& Instance()
	{
		static CSysteminfo _instance;
		return _instance;
	}
	CSysteminfo()
	{
		ZeroMemory(&m_osVersionInfo, sizeof(m_osVersionInfo));
		m_osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&m_osVersionInfo);

		ZeroMemory(&m_sysInfo, sizeof(m_sysInfo));
		GetSystemInfo(&m_sysInfo);
	}
	BOOL IsWin7()
	{
		return m_osVersionInfo.dwMajorVersion >= 6;
	}
	BOOL IsWin8()
	{
		return (m_osVersionInfo.dwMajorVersion >=6 && m_osVersionInfo.dwMinorVersion >= 2);
	}
	DWORD GetNumberOfProcessors()
	{
		return m_sysInfo.dwNumberOfProcessors;
	}
	static BOOL Is64Bit_OS()
	{
		BOOL bRetVal = FALSE;

		SYSTEM_INFO si = { 0 };

		typedef		void (WINAPI *LPFN_PGNSI)(LPSYSTEM_INFO);
		LPFN_PGNSI pGNSI = (LPFN_PGNSI) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
		if (pGNSI == NULL)
		{
			return FALSE;
		}
		pGNSI(&si);

		//�Ƿ�64λ
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
			si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
		{
			bRetVal = TRUE;
		}

		return bRetVal;
	}
private:
	OSVERSIONINFO m_osVersionInfo;
	SYSTEM_INFO m_sysInfo;
};


/*��Դ��ص�*/
typedef struct _PWRREASON_CONTEXT
{
	ULONG Version;
	DWORD Flags;

	union
	{
		struct
		{
			HMODULE LocalizedReasonModule;
			ULONG   LocalizedReasonId;
			ULONG   ReasonStringCount;
			LPWSTR  *ReasonStrings;
		}Detailed;
		LPWSTR SimpleReasonString;

	}Reason;

}PWRREASON_CONTEXT, *PPWRREASON_CONTEXT;

typedef enum _PWRREQUEST_TYPE
{
	PwrRequestDisplayRequired,
	PwrRequestSystemRequired,
	PwrRequestAwayModeRequired,
	PwrRequestExecutionRequired

}PWRREQUEST_TYPE, *PPWRREQUEST_TYPE;

#define PWRREQUEST_CONTEXT_VERSION          0
#define PWRREQUEST_CONTEXT_SIMPLE_STRING    0x00000001
#define PWRREQUEST_CONTEXT_DETAILED_STRING  0x00000002

typedef HANDLE	(WINAPI* fpPowerCreateRequest)(PPWRREASON_CONTEXT);
typedef BOOL	(WINAPI* fpPowerSetRequest)(HANDLE, PWRREQUEST_TYPE);
typedef BOOL	(WINAPI* fpPowerClearRequest)(HANDLE, PWRREQUEST_TYPE);

class SystemPowerTool
{
	SystemPowerTool()
	{
		m_hPwrReq = 0;
		m_fpPwrCreateReq = NULL;
		m_fpPwrSetReq	 = NULL;
		m_fpPwrClearReq	 = NULL;
		m_bLastEnableSaver = TRUE;
		m_bSysActiveSaver  = TRUE ;
	}

public:
	static SystemPowerTool & Instance()
	{
		static SystemPowerTool oSysTool ;
		return oSysTool ;
	}

	BOOL Initialize()
	{
		if ( CSysteminfo::Instance().IsWin7() )
		{
			m_fpPwrCreateReq = (fpPowerCreateRequest)GetProcAddress( GetModuleHandle(_T("Kernel32.dll")), "PowerCreateRequest" );
			m_fpPwrSetReq = (fpPowerSetRequest)GetProcAddress( GetModuleHandle(_T("Kernel32.dll")), "PowerSetRequest" );
			m_fpPwrClearReq = (fpPowerClearRequest)GetProcAddress( GetModuleHandle(_T("Kernel32.dll")), "PowerClearRequest" );
		}

		// ����
		SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &m_bSysActiveSaver, 0);

		return TRUE ;
	}

	void Release()
	{
	}

	void EnableSaver(BOOL bEnable)
	{
		if ( m_bSysActiveSaver )
		{
			if ( bEnable != m_bLastEnableSaver )
			{
				::SystemParametersInfo( SPI_SETSCREENSAVEACTIVE, bEnable, 0, 0 );
			}
		}

		if ( bEnable )
		{
			if ( !m_bLastEnableSaver ) // �ָ����ܷ���
			{
				if ( CSysteminfo::Instance().IsWin7() )
				{
					if ( m_fpPwrCreateReq && m_fpPwrSetReq && m_fpPwrClearReq && m_hPwrReq )
					{
						m_fpPwrClearReq( m_hPwrReq, PwrRequestDisplayRequired );
						m_fpPwrClearReq( m_hPwrReq, PwrRequestExecutionRequired );
						CloseHandle(m_hPwrReq);
						m_hPwrReq = 0;
					}
				}
				else
				{
					SetThreadExecutionState(ES_CONTINUOUS);
				}
			}
		}
		else // ��ͣ���ܷ���
		{
			if ( CSysteminfo::Instance().IsWin7() )
			{
				if ( m_fpPwrCreateReq && m_fpPwrSetReq && m_fpPwrClearReq && !m_hPwrReq )
				{
					PWRREASON_CONTEXT prc = {0};
					prc.Version = PWRREQUEST_CONTEXT_VERSION;
					prc.Flags = PWRREQUEST_CONTEXT_SIMPLE_STRING;
					prc.Reason.SimpleReasonString = L"Video Player";
					if ( m_hPwrReq = m_fpPwrCreateReq(&prc) )
					{
						m_fpPwrSetReq( m_hPwrReq, PwrRequestDisplayRequired );
						m_fpPwrSetReq( m_hPwrReq, PwrRequestExecutionRequired );
					}
				}
			}
			else
			{
				SetThreadExecutionState(ES_DISPLAY_REQUIRED|ES_SYSTEM_REQUIRED);
			}
		}
		
		m_bLastEnableSaver = bEnable;
	}

private:
	/*��Դ��ص�*/
	HANDLE	m_hPwrReq;						// ����Դ���󡱾������CloseHandle�ر�

	BOOL	m_bSysActiveSaver;				// ϵͳԭ���Ƿ�����������
	BOOL	m_bLastEnableSaver;				// �������ϴ��Ƿ�����������

	fpPowerCreateRequest m_fpPwrCreateReq;  // ��������Դ����
	fpPowerSetRequest    m_fpPwrSetReq;     // �����Դ����
	fpPowerClearRequest  m_fpPwrClearReq;   // �������Դ����
};