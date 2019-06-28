#ifndef __LOG_H__
#define __LOG_H__

#include <windows.h>
#include <stdio.h>

// #define __LOG     //ON/OFF LOG
// #define __DBGVIEW //for DbgView
// #define __LOGFILE //for Log file

#define LOGFILEPATH "C:\\QvodPlayer_Log.txt"

#ifdef __LOG

	#if defined(__DBGVIEW) && !defined(__LOGFILE)
		#define OpenFile()
		#define CloseFile()
		#define FlushData()
		#define DefineVar()
		#define LogText(Str) OutputDebugString(Str)
		#define LogData(Format, Data)\
		{\
			CHAR tszDump[1024];\
			wsprintf(tszDump, (Format), (Data));\
			OutputDebugString(tszDump);\
		}
	#elif defined(__LOGFILE) && !defined(__DBGVIEW)
		extern	FILE* pFile;
		extern	char  LogBuffer[128];

		#define DefineVar()\
			FILE* pFile;\
		    char LogBuffer[128];

		#define OpenFile()\
		{\
			SYSTEMTIME st;\
			GetLocalTime(&st);\
			sprintf(LogBuffer, g_logFilePath);\
			pFile = fopen(LogBuffer,"w");\
			sprintf(LogBuffer, "-- Open at %02u:%02u:%02u --\n\n", st.wHour, st.wMinute, st.wSecond); \
			fwrite(LogBuffer, strlen(LogBuffer),1,pFile);\
			fflush(pFile);\
		}

		#define CloseFile()\
		{\
			SYSTEMTIME st;\
			GetLocalTime(&st);\
			sprintf(LogBuffer, "\n\n-- Close at %02u:%02u:%02u --", st.wHour, st.wMinute, st.wSecond); \
			fwrite(LogBuffer, strlen(LogBuffer), 1, pFile);	\
			fflush(pFile);\
			fclose(pFile);\
		}

		#define LogData(Format, Data)\
		{\
			sprintf(LogBuffer, Format, Data);\
			fwrite(LogBuffer, strlen(LogBuffer), 1, pFile); \
			fflush(pFile);\
		}

		#define LogText(Str)\
		{\
			sprintf(LogBuffer, Str);\
			fwrite(LogBuffer, strlen(LogBuffer), 1, pFile); \
			fflush(pFile);\
		}

		#define FlushData()\
		{\
			fflush(pFile);\
		}

	#elif defined(__LOGFILE) && defined(__DBGVIEW)
		extern	FILE* pFile;
		extern	char  LogBuffer[128];
		
		#define DefineVar()\
			FILE* pFile;\
			char LogBuffer[128];
		
		#define OpenFile()\
		{\
			SYSTEMTIME st;\
			GetLocalTime(&st);\
			sprintf(LogBuffer, g_logFilePath);\
			pFile = fopen(LogBuffer,"w");\
			sprintf(LogBuffer, "-- Open at %02u:%02u:%02u --\n\n", st.wHour, st.wMinute, st.wSecond); \
			fwrite(LogBuffer, strlen(LogBuffer),1,pFile);\
			fflush(pFile);\
		}
		
		#define CloseFile()\
		{\
			SYSTEMTIME st;\
			GetLocalTime(&st);\
			sprintf(LogBuffer, "\n\n-- Close at %02u:%02u:%02u --", st.wHour, st.wMinute, st.wSecond); \
			fwrite(LogBuffer, strlen(LogBuffer), 1, pFile);	\
			fflush(pFile);\
			fclose(pFile);\
		}
		
		#define LogData(Format, Data)\
		{\
			sprintf(LogBuffer, Format, Data);\
			fwrite(LogBuffer, strlen(LogBuffer), 1, pFile); \
			fflush(pFile);\
			char tszDump[1024];\
			sprintf(tszDump, (Format), (Data));\
			OutputDebugStringA(tszDump);\
		}
		
		#define LogText(Str)\
		{\
			sprintf(LogBuffer, Str);\
			fwrite(LogBuffer, strlen(LogBuffer), 1, pFile); \
			fflush(pFile);\
			OutputDebugStringA(Str);\
		}
		
		#define FlushData()\
		{\
			fflush(pFile);\
		}
	#else
		#define OpenFile()
		#define CloseFile()
		#define LogData(Format, Data)
		#define LogText(Str)
		#define FlushData()
		#define DefineVar()	
	#endif
#else
	#define OpenFile()
	#define CloseFile()
	#define LogData(Format, Data)
	#define LogText(Str)
	#define FlushData()
	#define DefineVar()	
#endif //__LOG

#endif//__LOG_H__