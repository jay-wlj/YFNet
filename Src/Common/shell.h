#pragma once


namespace common{

	struct FilterFile
	{
		CString name_;		// 描述
		CString ext_;	// 后缀
	};
	struct OpenFileNamePara
	{
		OpenFileNamePara():
			hOwner_(NULL),
			nFilterIndex_(1),
			flags(OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_HIDEREADONLY)
			{}
		HWND hOwner_;
		CString defPath_; 
		std::vector<FilterFile> vFilterFiles_;
		int nFilterIndex_;
		DWORD flags;
	};

	class Shell
	{
	public:
		static void GetVersionExe(const wchar_t *pExePath,long *v1,long *v2,long *v3,long *v4)
		{
			*v1 = 0;
			*v2 = 0;
			*v3 = 0;
			*v4 = 0;
			if ( pExePath == NULL || *pExePath == 0 )
			{
				return ;
			}

			DWORD dwInfoSize = 0;
			DWORD dwHandle = 0;
			dwInfoSize = GetFileVersionInfoSizeW(pExePath,&dwHandle);
			if ( dwInfoSize == 0 )
			{
				return ;
			}
			char *pData =new char[dwInfoSize];
			memset(pData,0,dwInfoSize);
			if ( !GetFileVersionInfoW(pExePath,0,dwInfoSize,pData) )
			{
				delete[] pData;
				return ;
			}

			VS_FIXEDFILEINFO * pInfo;   
			unsigned int iInfoLen; 
			if ( VerQueryValueW(pData,L"\\",(void **)&pInfo,&iInfoLen) )
			{
				*v1 = HIWORD(pInfo->dwFileVersionMS);
				*v2 = LOWORD(pInfo->dwFileVersionMS);
				*v3 = HIWORD(pInfo->dwFileVersionLS);
				*v4 = LOWORD(pInfo->dwFileVersionLS);
			}
			delete[] pData;
		}
		static CString GetSaveFileName(
			HWND hOwner = NULL, 
			LPCTSTR lpTitle = NULL, 
			LPCTSTR lpDefFile = NULL, 
			LPCTSTR lpFilter = NULL,
			LPCTSTR lpDefExt = NULL,
			DWORD extraFlags = 0
			)
		{
			CString saveFileName;
			DWORD flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |OFN_OVERWRITEPROMPT;
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			TCHAR szFile[MAX_PATH] = {0};
			if(lpDefFile) _tcscpy(szFile, lpDefFile);

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hOwner;
			ofn.lpstrTitle = lpTitle;
			ofn.lpstrFilter = lpFilter;
			ofn.lpstrFile = szFile;
			ofn.Flags = flags|extraFlags;
			ofn.nMaxFile = MAX_PATH;
			ofn.nFilterIndex = 1;
			ofn.lpstrDefExt = lpDefExt;
			if(::GetSaveFileName(&ofn))
			{
				saveFileName = szFile;
			}
			return saveFileName;
		}
		static BOOL GetOpenFileName(std::vector<CString>& vFiles, const OpenFileNamePara &para,const TCHAR	* const lpstrFile = NULL)
		{
			TCHAR szFileBuffer[50*MAX_PATH] = {0};
			if(lpstrFile != NULL)
			{
				_tcscpy(szFileBuffer,lpstrFile);
			}

			TCHAR *pFileFilterBuffer = NULL;
			OPENFILENAME info;
			ZeroMemory(&info, sizeof(OPENFILENAME));
			info.lStructSize = sizeof(OPENFILENAME);
			info.hwndOwner = para.hOwner_;

			if(para.vFilterFiles_.size() > 0)
			{
				pFileFilterBuffer = new TCHAR[50*MAX_PATH];
				ZeroMemory(pFileFilterBuffer,50*MAX_PATH*sizeof(TCHAR));
				TCHAR *pFileFilter = pFileFilterBuffer;
				int nPos = 0;
				for(long i=0; i < para.vFilterFiles_.size(); i++)
				{
					nPos = _stprintf(pFileFilter, _T("%s"), para.vFilterFiles_[i].name_);
					pFileFilter += nPos + 1;
					nPos = _stprintf(pFileFilter, _T("%s"), para.vFilterFiles_[i].ext_);
					pFileFilter += nPos + 1;
				}
				info.lpstrFilter = pFileFilterBuffer;
				info.nFilterIndex = para.nFilterIndex_;
			}

			info.lpstrInitialDir = para.defPath_;
			info.nMaxFile = 50*MAX_PATH;
			info.Flags = para.flags;
			info.lpstrFile = szFileBuffer;
			if(::GetOpenFileName(&info))
			{
				TCHAR *pBuf = szFileBuffer;
				BOOL bMulti = *(pBuf + _tcslen(pBuf) + 1);
				if (bMulti)
				{
					CString strDir = pBuf;
					pBuf += _tcslen(pBuf) + 1;
					while(*pBuf)
					{
						CString strFile = strDir + _T("\\") + pBuf;
						pBuf += _tcslen(pBuf) + 1;
						vFiles.push_back(strFile);
					}
				}
				else
				{
					vFiles.push_back(pBuf);
				}
				if(pFileFilterBuffer)
				{
					delete[] pFileFilterBuffer;
				}
				return TRUE;
			}
			if(pFileFilterBuffer)
			{
				delete[] pFileFilterBuffer;
			}
			return FALSE;
		}
		static CString BrowseForFolder(
			HWND hOwner,
			LPCTSTR lpTitle = NULL,
			DWORD extraFlags = 0
			)
		{
			TCHAR szBuffer[MAX_PATH] = {0};
			BROWSEINFO BrowseInfo;
			ZeroMemory(&BrowseInfo, sizeof(BROWSEINFO));
			BrowseInfo.hwndOwner = hOwner;
			BrowseInfo.pszDisplayName = szBuffer;
			BrowseInfo.lpszTitle = lpTitle;
			BrowseInfo.ulFlags = BIF_USENEWUI| extraFlags;
			LPITEMIDLIST lpItemIDList = SHBrowseForFolder(&BrowseInfo);
			if(lpItemIDList)
			{
				TCHAR szDes[MAX_PATH] = {0};
				if(SHGetPathFromIDList(lpItemIDList, szDes))
				{
					return szDes;
				}
			}
			return _T("");
		}


		static BOOL OpenFolderAndSelectFile( CString strFilePath)
		{
			if(GetFileAttributes(strFilePath) == INVALID_FILE_ATTRIBUTES)
			{
				return FALSE;
			}

			//
			// Get a pointer to the Desktop's IShellFolder interface.
			// 
			LPSHELLFOLDER pDesktopFolder;
			if ( SUCCEEDED(SHGetDesktopFolder( &pDesktopFolder)))
			{
				// 
				// IShellFolder::ParseDisplayName requires the file name be in
				// Unicode.
				// 
				OLECHAR oleStr[MAX_PATH] = {0};
				CString strDir =  QFile::Tool::GetDirPath(strFilePath);
				lstrcpy(oleStr , strDir);
				// 
				// Convert the path to an ITEMIDLIST.
				// 
				LPITEMIDLIST     pidl;
				ULONG             chEaten;
				ULONG             dwAttributes;
				HRESULT             hr;

				hr = pDesktopFolder->ParseDisplayName(NULL, NULL, oleStr, &chEaten, &pidl, &dwAttributes);
				if (FAILED(hr))
				{
					pDesktopFolder->Release();
					return FALSE;
				}
				LPCITEMIDLIST pidlFolder = pidl;

				ZeroMemory(oleStr ,_countof(oleStr));
				lstrcpy(oleStr , strFilePath );
				hr = pDesktopFolder->ParseDisplayName(NULL, NULL, oleStr, &chEaten, &pidl, &dwAttributes);
				if (FAILED(hr))
				{
					pDesktopFolder->Release();
					return FALSE;
				}
				//CoInitialize(0);
				LPCITEMIDLIST pidlFile = pidl;
				hr = SHOpenFolderAndSelectItems( pidlFolder, 1, &pidlFile, 0);

				pDesktopFolder->Release();
				//CoUninitialize();
				if ( hr == S_OK)
				{
					return TRUE;
				}
			}
			return FALSE;
		}

		//创建桌面快捷方式
		static BOOL CreateDesktopShotCut(CString strName, CString strSourcePath, CString strIconPath, CString strcmm)
		{
			CoInitialize(NULL);

			BOOL bRe = FALSE ;
			int i;	
			WCHAR Path[MAX_PATH+1];
			CString strDestDir;
			i=CSIDL_COMMON_DESKTOPDIRECTORY;
			LPITEMIDLIST pidl;
			LPMALLOC pShell;
			if(SUCCEEDED(SHGetMalloc(&pShell)))	
			{	
				if(SUCCEEDED(SHGetSpecialFolderLocation(NULL,i,&pidl)))	
				{
					if(!SHGetPathFromIDList(pidl,Path))		
					{		
						pShell->Free(pidl);		
						::CoUninitialize();	

						return FALSE;			
					}	
					pShell->Release();		
					strDestDir.Format( _T("%s"),Path);
					strDestDir += _T("\\") ;		
					strDestDir += strName;		
					strDestDir += _T(".lnk") ;		
					IShellLink* psl;	

					if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(LPVOID*)&psl)))			
					{		
						HRESULT hr = psl->SetPath(strSourcePath) ;

						if(  SUCCEEDED(hr) )	
						{			
							hr = psl->SetIconLocation( strIconPath, 0) ;

							if(  SUCCEEDED(hr) )	
							{
								if ( strcmm.GetLength() > 0 )
								{
									psl->SetArguments( strcmm ) ;
								}

								IPersistFile* ppf;		
								if(SUCCEEDED(psl->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf)))				
								{								
									if(SUCCEEDED(ppf->Save(strDestDir,TRUE)))					
									{	
										bRe = TRUE ;
									}	

									ppf->Release();
								}
							}

							psl->Release();
						}
					}
				}
			}

			::CoUninitialize();	

			return bRe ;
		}

	};

}