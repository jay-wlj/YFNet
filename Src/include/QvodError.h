#ifndef	_QVOD_ERROR_H__
#define _QVOD_ERROR_H__

typedef long				QVOD_ERROR;
#define E_QVOD_S_OK				0
#define E_QVOD_S_FALSE			1
#define E_QVOD_F_DSINITFAIL		-1
#define E_QVOD_F_BUFISNULL		-2
#define E_QVOD_F_NOFILE			-3
#define E_QVOD_F_NOREADFILTER	-4
#define E_QVOD_F_CANNOTSEEK		-5
#define E_QVOD_F_NOINTERFACE	-7
#define E_QVOD_N_NONETFACE		-8
#define E_QVOD_N_TIMEOUT		-9
#define E_QVOD_N_NODECODER      -10
#define E_QVOD_N_UNKONWMEDIATYPE -11
#define E_QVOD_F_INVALIDPARAM    -12
#define E_QVOD_F_MUSIC			-13
#define E_QVOD_E_CREATEAR_FAILED   -14
#define E_QVOD_F_STREAM_BUFFERING  -15
#define E_QVOD_E_FAIL            -20
#define E_QVOD_E_FULLDISK        -25
#define E_QVOD_E_NET_CREATE_FAILED -30 // 网络任务创建失败
#define E_QVOD_E_URLFAIL         -100


#define SafeDelete(pObject)		 if(pObject) {delete pObject; pObject = NULL;}
#define SafeDeleteArray(p)		 if(p){delete []p; p = NULL;}

#ifdef _WIN32
#define SafeRelease(pInterface)  if(pInterface) {pInterface->Release(); pInterface = NULL;}
#define SafeDelObj(pObject)		 if(pObject){::DeleteObject(pObject); pObject = 0;}	
#define SafeDelHandle(pHandle)	 if(pHandle){::CloseHandle(pHandle);pHandle = 0;}
#endif

#endif