#ifndef COMMON_TRAYICONTOOL_H
#define COMMON_TRAYICONTOOL_H

namespace TrayIcon{

	class Tool
	{
	public:

		//��ȡ���̵�����
		static RECT GetTrayRect(LPCTSTR lpStrWindowName)
		{
			RECT rect = {0};
			HWND hWnd = NULL;

			hWnd = FindTrayWnd();

			BOOL bRe = FALSE ;
			if (hWnd != NULL)
			{
				bRe = EnumNotifyWindow(rect,hWnd, lpStrWindowName) ;
				if ( !bRe && CSysteminfo::Instance().IsWin7() )//���û����ͨ������
				{		
					hWnd = FindNotifyIconOverflowWindow(); //���������win7��
					RECT rect2 = {0};

					if (hWnd != NULL && EnumNotifyWindow(rect2,hWnd, lpStrWindowName) )
					{
						if ( ::IsWindowVisible(hWnd) )
						{
							rect = rect2 ;
						}
						else
						{
							//����������أ���ȡ����ͼ���һ��ͼ��λ��
							if ( (hWnd = FindWindow( _T("Shell_TrayWnd"), NULL)) != NULL )
							{
								if ( (hWnd = FindWindowEx(hWnd, 0, _T("TrayNotifyWnd"), NULL)) != NULL )
								{
									::GetWindowRect( hWnd, &rect ) ;
									rect.right = rect.left + 16 ;
								}
							}
						}
					}
				}
				else
				{
					// 			CString str ;
					// 			str.Format( _T("%d %d %d %d"), rect.left, rect.right, rect.top, rect.bottom ) ;
					// 			MessageBox( str ) ;

					if ( !bRe )
					{
						//����������أ���ȡ����ͼ���һ��ͼ��λ��
						if ( (hWnd = FindWindow( _T("Shell_TrayWnd"), NULL)) != NULL )
						{
							if ( (hWnd = FindWindowEx(hWnd, 0, _T("TrayNotifyWnd"), NULL)) != NULL )
							{
								::GetWindowRect( hWnd, &rect ) ;
								rect.right = rect.left + 16 ;
							}
						}
					}
				}
			}

			return rect;
		}

	private:

		//ö�ٻ�ȡ��������λ��
		static bool EnumNotifyWindow(RECT &rect,HWND hWnd, LPCTSTR lpStrWindowName)
		{
			//RECT rect = {0};
			bool bSuc = false;
			unsigned long lngPID = 0;
			long ret = 0,lngButtons = 0;
			long lngHwndAdr = 0,lngHwnd = 0;//,lngTextAdr,lngButtonID;
			HANDLE hProcess = NULL;
			LPVOID lngAddress = NULL,lngRect = NULL;
			SIZE_T nNumberOfBytesRead;

			if (hWnd != NULL)
			{
				DWORD dwPID = 0;
				GetWindowThreadProcessId( hWnd, &dwPID );
				HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS|PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, 0, dwPID );
				if ( !hProcess )
				{
					return false;
				}
				// ��Ŀ�����������һ���ڴ棬��TBBUTTON��Ϣ
				LPVOID pvAddress = VirtualAllocEx( hProcess, 0, _MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
				// ��SendMessage����TB_BUTTONCOUNT��Ϣ��ToolbarWindow32������ͼ����
				int nBtnCount = ::SendMessage( hWnd, TB_BUTTONCOUNT, 0, 0 );

				for ( int i = 0; i < nBtnCount; ++i )
				{
					// ��ReadProcessMemory�ӽ����ڴ�ռ��ȡ���ݣ�ͼ����⡢ͼ������
					::SendMessage( hWnd, TB_GETBUTTON, i, (long)pvAddress );
					
					TCHAR szBtnText[_MAX_PATH] = {0};
					TBBUTTON bi;
					ReadProcessMemory( hProcess, (LPVOID)pvAddress,  &bi, sizeof(TBBUTTON), &nNumberOfBytesRead );

					SendMessage(hWnd, TB_GETBUTTONTEXT , bi.idCommand, (LPARAM)(pvAddress));
					ReadProcessMemory(hProcess, (void *)(pvAddress), szBtnText, _MAX_PATH*sizeof(TCHAR), &nNumberOfBytesRead);

					if ( !lstrcmp( szBtnText, lpStrWindowName ) )
					{
						RECT rtCLient ;
						int nd = ::SendMessage( hWnd, TB_GETRECT, bi.idCommand, (LPARAM)pvAddress );

						if ( nd == 0 )
						{
							break;
						}

						ReadProcessMemory( hProcess, pvAddress, &rtCLient, sizeof(RECT), 0 );
						RECT rt = {0};
						::GetWindowRect( hWnd, &rt );
						::OffsetRect( &rtCLient, rt.left, rt.top );
						rect = rtCLient ;

						bSuc = TRUE ;

						break;
					}
				}
				VirtualFreeEx( hProcess, pvAddress, 0, MEM_RELEASE );
				CloseHandle( hProcess );
			}

			return bSuc;
		}
		//��ȡ��ͨ���������ھ��
		static HWND FindTrayWnd()
		{
			HWND hWnd = NULL;
			HWND hWndPaper = NULL;

			if ( (hWnd = FindWindow( _T("Shell_TrayWnd"), NULL)) != NULL )
			{
				if ( (hWnd = FindWindowEx(hWnd, 0, _T("TrayNotifyWnd"), NULL)) != NULL )
				{
					hWndPaper = FindWindowEx(hWnd, 0, _T("SysPager"), NULL);
					if(!hWndPaper)
					{
						hWnd = FindWindowEx(hWnd, 0, _T("ToolbarWindow32"), NULL);
					}
					else
					{
						hWnd = FindWindowEx(hWndPaper, 0, _T("ToolbarWindow32"), NULL);
					}
				}
			}

			return hWnd;
		}

		//��ȡ������������ھ��
		static HWND FindNotifyIconOverflowWindow()
		{
			HWND hWnd = NULL;

			hWnd = FindWindow(_T("NotifyIconOverflowWindow"), NULL);
			if (hWnd != NULL)
			{
				hWnd = FindWindowEx(hWnd, NULL, _T("ToolbarWindow32"), NULL);
			}

			return hWnd;
		}
	};
}
#endif