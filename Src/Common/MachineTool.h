#ifndef _MACHINETOOL_H_H__
#define _MACHINETOOL_H_H__

#include <Windows.h>
#include <vector>
#include <Winioctl.h>
#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")
#include "..\share\sha1.h"
#include "stringtool.h"
#include "../include/function.h"
#include "../include/Registry.h"

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

#define  DFP_GET_VERSION          0x00074080
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088
#define  IDE_ATAPI_IDENTIFY		  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY         0xEC  //  Returns ID sector for ATA.

typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;      // Binary driver version.
	BYTE bRevision;     // Binary driver revision.
	BYTE bReserved;     // Not used.
	BYTE bIDEDeviceMap; // Bit map of IDE devices.
	DWORD fCapabilities; // Bit mask of driver capabilities.
	DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

class MachineTool
{
public:
	static CString GetMachineID()
	{
		CStringA ansiID = Convert::ToAString(CpuID() /*+ DiskID()*//* + GetMacAddress()*/);		//GetMacAddress会阻塞  DiskID只有管理员权限才能获取
		CString str;
		CRegistry reg(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\HunanTV"));
		reg.GetString(NULL, _T("statid"), str);
		ansiID += Fun::UnicodeToANSI(str).c_str();
		int nlen = ansiID.GetLength();
		BYTE *pBuffer = new BYTE[nlen];
		memcpy(pBuffer, (LPCSTR)ansiID, nlen);
		CSHA1 sha1;
		sha1.write(pBuffer, nlen);
		delete[] pBuffer;
		BYTE readBuffer[20];
		sha1.read(readBuffer);
		return Byte20To40Str(readBuffer);
	}

	static HRESULT GetHardwareID(VARIANT* pVal)
	{
		if(pVal == NULL) return E_INVALIDARG;
		if(pVal->vt == VT_BSTR)
		{
			if(pVal->bstrVal)
			{
				SysFreeString(pVal->bstrVal);
				pVal->bstrVal = NULL;
			}
		}

		CString machineID = GetMachineID();
		pVal->vt = VT_BSTR;
		pVal->bstrVal = SysAllocString(Convert::ToWString(machineID));
		if(pVal->bstrVal == NULL) return E_OUTOFMEMORY;
		return S_OK;
	}

	static CString Byte20To40Str(BYTE *pbyte)
	{
		CString strResult;
		CString strTemp;
		BYTE *p = pbyte;
		int i = 0, j =0;
		for(int k=0; k<20; k++)
		{
			i = pbyte[k]>>4;
			if(i >= 0 && i <= 9)
			{
				strTemp.Format(_T("%d"), i);
			}
			else if(i >= 10 && i <= 31)
			{
				strTemp.Format(_T("%c"), i + 55);
			}
			strResult += strTemp;

			i = pbyte[k] & 0x8;
			if(i >= 0 && i <= 9)
			{
				strTemp.Format(_T("%d"), i);
			}
			else if(i >= 10 && i <= 31)
			{
				strTemp.Format(_T("%c"), i + 55);
			}
			strResult += strTemp;
		}
		return strResult;
	}

	static CString DiskID()
	{
		CString strDiskID;
		int drive = 0;
		HANDLE hPhysicalDriveIOCTL = 0;
		TCHAR driveName [256];
		_stprintf (driveName, _T("\\\\.\\PhysicalDrive%d"), drive);
		hPhysicalDriveIOCTL = CreateFile (driveName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			GETVERSIONOUTPARAMS VersionParams;
			DWORD               cbBytesReturned = 0;
			// Get the version, etc of PhysicalDrive IOCTL
			memset ((void*) &VersionParams, 0, sizeof(VersionParams));

			if ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL,
				0,
				&VersionParams,
				sizeof(VersionParams),
				&cbBytesReturned, NULL) )
			{
				BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
				SENDCMDINPARAMS  scip;
				BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
				bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

				memset (&scip, 0, sizeof(scip));
				memset (IdOutCmd, 0, sizeof(IdOutCmd));

				if ( DoIDENTIFY (hPhysicalDriveIOCTL,
					&scip,
					(PSENDCMDOUTPARAMS)&IdOutCmd,
					(BYTE) bIDCmd,
					(BYTE) drive,
					&cbBytesReturned))
				{
					// 				USHORT *pIdSector = (USHORT *)
					// 					((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;
					// 				//pIdSector += 27;
					// 				char *p = (char *)pIdSector;

					char *p = (char *)((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer;
					p += 20;
					char p1[21] = {0};
					memcpy(p1, p, 20);
					strDiskID = p1;
				}
			}
			CloseHandle (hPhysicalDriveIOCTL);
		}
		return strDiskID;
	}

	static BOOL DoIDENTIFY(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
		PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
		PDWORD lpcbBytesReturned)
	{
		// Set up data structures for IDENTIFY command.
		pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
		pSCIP -> irDriveRegs.bFeaturesReg = 0;
		pSCIP -> irDriveRegs.bSectorCountReg = 1;
		pSCIP -> irDriveRegs.bSectorNumberReg = 1;
		pSCIP -> irDriveRegs.bCylLowReg = 0;
		pSCIP -> irDriveRegs.bCylHighReg = 0;

		// Compute the drive number.
		pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

		// The command can either be IDE identify or ATAPI identify.
		pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
		pSCIP -> bDriveNumber = bDriveNum;
		pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

		return( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
			(LPVOID) pSCIP,
			sizeof(SENDCMDINPARAMS) - 1,
			(LPVOID) pSCOP,
			sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
			lpcbBytesReturned, NULL) );
	}

	static CString CpuID()
	{
		unsigned   long   s1,s2;
		CString   VernderID;
		UCHAR	  vendor_id[]="------------";
		CString   MyCpuID,CPUID1,CPUID2;
		__asm{
			xor   eax,eax
				cpuid
				mov   dword   ptr   vendor_id,ebx
				mov   dword   ptr   vendor_id[+4],edx
				mov   dword   ptr   vendor_id[+8],ecx
		}
		VernderID.Format(_T("%s-"),vendor_id);
		__asm{
			mov   eax,01h
				xor   edx,edx
				cpuid
				mov   s1,edx
				mov   s2,eax
		}
		CPUID1.Format(_T("%08X%08X"),s1,s2);
		__asm{
			mov   eax,03h
				xor   ecx,ecx
				xor   edx,edx
				cpuid
				mov   s1,edx
				mov   s2,ecx
		}
		CPUID2.Format(_T("%08X%08X"),s1,s2);
		MyCpuID   =   CPUID1+CPUID2;
		return MyCpuID;
	}
	// 显卡型号
	static BOOL GetVideoCardInfo(CString &videoCardname,CString &videoCardVersion)
	{
		BOOL bRet = FALSE;
		TCHAR szReg[] = _T("SYSTEM\\CurrentControlSet\\Enum\\PCI");
		CString str;
		HKEY hKey = NULL;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, szReg, 0, KEY_READ, &hKey);
		if(hKey)
		{
			DWORD dwCount = 0;
			RegQueryInfoKey(hKey, NULL, NULL, NULL, &dwCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			for(long i=0; i<dwCount; i++)
			{
				TCHAR szVanName[MAX_PATH] = {0};
				DWORD dwVanNameLen = MAX_PATH;
				DWORD dwRet = RegEnumKeyEx(hKey, i, szVanName, &dwVanNameLen, NULL, NULL, NULL, NULL);
				if(dwRet == ERROR_SUCCESS)
				{
					str.Format(_T("%s\\%s"), szReg, szVanName);
					HKEY hKey1 = NULL;
					RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey1);
					if(hKey1)
					{
						TCHAR szHidName[MAX_PATH] = {0};
						DWORD dwHidNameLen = MAX_PATH;
						if(RegEnumKeyEx(hKey1, 0, szHidName, &dwHidNameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
						{
							str.Format(_T("%s\\%s\\%s"), szReg, szVanName, szHidName);
							HKEY hKey2 = NULL;
							RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey2);
							if(hKey2)
							{
								TCHAR szClassName[MAX_PATH] = {0};
								DWORD dwClassNameLen = MAX_PATH;
								if(RegQueryValueEx(hKey2, _T("Class"), NULL, NULL, (LPBYTE)szClassName, &dwClassNameLen) == ERROR_SUCCESS)
								{
									if(lstrcmp(szClassName, _T("Display")) == 0)
									{
										str += _T("\\Control");
										HKEY hKey3 = NULL;
										RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey3);
										if(hKey3)
										{
											RegCloseKey(hKey3);
											TCHAR szDriver[MAX_PATH] = {0};
											DWORD dwDriverLen = MAX_PATH;
											if(RegQueryValueEx(hKey2, _T("Driver"), NULL, NULL, (LPBYTE)szDriver, &dwDriverLen) == ERROR_SUCCESS)
											{
												str.Format(_T("SYSTEM\\CurrentControlSet\\Control\\Class\\%s"), szDriver);
												HKEY hKey4 = NULL;
												RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey4);
												if(hKey4)
												{
													TCHAR szDeviceName[MAX_PATH] = {0};
													DWORD dwDeviceNameLen = MAX_PATH;
													if(RegQueryValueEx(hKey4, _T("DriverDesc"), NULL, NULL, (LPBYTE)szDeviceName, &dwDeviceNameLen) == ERROR_SUCCESS)
													{
														if(videoCardname.GetLength()>0)
														{
															videoCardname += _T(".");
														}
														str = szDeviceName;
														str.Trim();
														videoCardname += str;
													}

													if(RegQueryValueEx(hKey4, _T("DriverVersion"), NULL, NULL, (LPBYTE)szDeviceName, &dwDeviceNameLen) == ERROR_SUCCESS)
													{
														if(videoCardname.GetLength()>0)
														{
															videoCardname += _T(".");
														}
														str = szDeviceName;
														str.Trim();
														videoCardVersion += str;
													}
													RegCloseKey(hKey4);

												}
											}
										}
									}
								}
								RegCloseKey(hKey2);
							}
						}
						RegCloseKey(hKey1);
					}
				}
			}
			RegCloseKey(hKey);
		}
		return bRet;
	}
	static DWORD GetProcessorType()
	{
		SYSTEM_INFO si;  
		memset(&si,0,sizeof(SYSTEM_INFO));  
		GetSystemInfo(&si);   
		return si.dwProcessorType;
	}
	static CString GetProcessorName()
	{
		CString	processorName;
		CRegKey regkey; 
		if (regkey.Open(HKEY_LOCAL_MACHINE,_T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),KEY_ALL_ACCESS) == ERROR_SUCCESS)  
		{  
			TCHAR szCPUName[50] = {0};  
			DWORD dwSize=50;    
			if (ERROR_SUCCESS == regkey.QueryStringValue(_T("ProcessorNameString"),szCPUName,&dwSize))  
			{  
				processorName = szCPUName;  
			}	 
		}  
		return processorName;
	}
	static float GetCpuUse(bool bUpdate = true)	//获取CPU占用率
	{
#define SystemBasicInformation 0
#define SystemPerformanceInformation 2
#define SystemTimeInformation 3

		typedef struct
		{
			ULONG Reserved;
			ULONG TimerResolution;
			ULONG PageSize;
			ULONG NumberOfPhysicalPages;
			ULONG LowestPhysicalPageNumber;
			ULONG HighestPhysicalPageNumber;
			ULONG AllocationGranularity;
			ULONG MinimumUserModeAddress;
			ULONG MaximumUserModeAddress;
			ULONG ActiveProcessorsAffinityMask;
			BYTE bKeNumberProcessors;
			BYTE bUn;
			WORD wUn;
		}SYSTEM_BASIC_INFORMATION;
		typedef struct
		{
			LARGE_INTEGER liIdleTime;
			DWORD dwSpace[76];
		}SYSTEM_PERFORMANCE_INFORMATION;
		typedef struct
		{
			LARGE_INTEGER liKeBootTime;
			LARGE_INTEGER liKeSystemTime;
			LARGE_INTEGER liExpTimeZoneBias;
			ULONG uCurrentTimeZoneId;
			DWORD dwReserved;
		}SYSTEM_TIME_INFORMATION;
		typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);

		PROCNTQSI NtQuerySystemInformation;

		struct SYSTEM_BASIC_INFORMATIONEX
		{
			SYSTEM_BASIC_INFORMATION SysBaseInfo;
			//	DWORD		wUn[4];
		};
		struct SYSTEM_PERFORMANCE_INFORMATIONEX
		{
			SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
			DWORD   wUn[4];
		};
		struct SYSTEM_TIME_INFORMATIONEX
		{
			SYSTEM_TIME_INFORMATION SysTimeInfo;
			DWORD wUn[4];
		};


		SYSTEM_PERFORMANCE_INFORMATIONEX SysPerfInfo; 
		SYSTEM_TIME_INFORMATIONEX SysTimeInfo; 
		SYSTEM_BASIC_INFORMATIONEX SysBaseInfo; 
		double dbIdleTime = 0.0; 
		double dbSystemTime; 
		LONG status; 
		static LARGE_INTEGER liOldIdleTime = {0,0}; 
		static LARGE_INTEGER liOldSystemTime = {0,0};
		static float fCpuUse = 0.0;

		if (!bUpdate) return fCpuUse;

		HMODULE hModule = GetModuleHandleW(L"ntdll");
		if (NULL == hModule) return fCpuUse;

		NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(hModule,"NtQuerySystemInformation");

		if (!NtQuerySystemInformation) 
			return 0.0;

		// get number of processors in the system 
		status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL); 
		if (status != NO_ERROR) 
			return 0.0;

		// get new system time 
		status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0); 
		if (status!=NO_ERROR) 
			return 0.0;

		// get new CPU's idle time 
		status =NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL); 
		if (status != NO_ERROR) 
			return 0.0;

		// if it's a first call - skip it 
		if (liOldIdleTime.QuadPart != 0) 
		{ 
			// CurrentValue = NewValue - OldValue 
			dbIdleTime = Li2Double(SysPerfInfo.SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime); 
			dbSystemTime = Li2Double(SysTimeInfo.SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

			// CurrentCpuIdle = IdleTime / SystemTime 
			dbIdleTime = dbIdleTime / dbSystemTime;

			// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors 
			dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.SysBaseInfo.bKeNumberProcessors + 0.5;

		}

		// store new CPU's idle and system time 
		liOldIdleTime = SysPerfInfo.SysPerfInfo.liIdleTime; 
		liOldSystemTime = SysTimeInfo.SysTimeInfo.liKeSystemTime;

		if (dbIdleTime < 0.0)
			dbIdleTime = 0.0;
		if (dbIdleTime>100.0)
			dbIdleTime = 100.0;
		fCpuUse = (float)dbIdleTime;
		return fCpuUse;
	}

	static void byte2Hex(unsigned char bData,unsigned char hex[])
	{
		int high=bData/16,low =bData %16;
		hex[0] = (high <10)?('0'+high):('A'+high-10);
		hex[1] = (low <10)?('0'+low):('A'+low-10);
	}

	static CString GetMacAddress()
	{
		CString strMacAddress;

		unsigned char address[1024] = {0};
		ULONG ulSize=0;
		PIP_ADAPTER_INFO pInfo=NULL;
		int temp=0;
		temp = GetAdaptersInfo(pInfo,&ulSize);//第一处调用，获取缓冲区大小
		pInfo=(PIP_ADAPTER_INFO)malloc(ulSize);
		temp = GetAdaptersInfo(pInfo,&ulSize);

		int iCount=0;
		while(pInfo)//遍历每一张网卡
		{
			//只取物理网上和无线网卡，忽略虚拟机等其它mac地址
			BOOL bPci = FALSE;
			if (NULL != strstr(pInfo->Description,"PCI") ||
				(NULL != strstr(pInfo->Description, "Ethernet") && NULL==strstr(pInfo->Description, "VMware")))
			{
				bPci = TRUE;
			}

			if(bPci//pAdapter->Description中包含"PCI"为：物理网卡
				||pInfo->Type==71//pAdapter->Type是71为：无线网卡
				)
			{
				//  pInfo->Address MAC址
				unsigned char address[50] = {0};
				iCount = 0;
				for(int i=0;i<(int)pInfo->AddressLength;i++)
				{
					byte2Hex(pInfo->Address[i],&address[iCount]);
					iCount+=2;
					if(i<(int)pInfo->AddressLength-1)
					{
						address[iCount++] = ':';
					}
					else
					{
						address[iCount++] = '#';

						CString strAddress = Fun::ANSIToUnicode((const char*)address);
						if (bPci)			//pci mac地址置前
						{
							strMacAddress.Insert(0, strAddress);
						}
						else
						{
							strMacAddress += strAddress;
						}
					}
				}
			}

			pInfo = pInfo->Next;
		}

		strMacAddress.Trim(_T("#"));
		return strMacAddress;
	}
};

#endif