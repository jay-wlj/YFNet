#if (MONGOTV || WASU || AIPAI || ANDROID)
#include "YfNetJni.h"
#else
#ifdef YFNET_OPEN_SDK 
#include "YfNetJni_OpenSDK.h"
#include "OpenSDKAuth.h"
#endif
#endif

#include "IYfNetPlush.h"
#include "common-commonstruct.h"
#include "common-commonfun.h"
#include "qvod_file.h"
#include "qvod_lock.h"


static IYfNetPlush* pNet = 0;
static long lNet = 0;

static jmethodID g_callback = NULL;
static JavaVM* g_jvm = NULL;
static jobject g_object = NULL;

void JavaCallbackFunction(SCallBackPara& para)
{
	if (!g_jvm)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"JavaCallbackFunction jvm Null\n");
#endif
		return ;
	}
	static CLock l_lock;
	CAutoLock lock(l_lock);
	JNIEnv* env;
	jint ret = JNI_OK;
	static jint version = JNI_VERSION_1_6;
	ret = (g_jvm)->GetEnv((void **)&env,version);
	if (ret != JNI_OK && ret != JNI_EDETACHED)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"JavaCallbackFunction GetEnv ret = %d\n",ret);
#endif
		return ;
	}
	if(ret == JNI_EDETACHED) 
	{
		env = NULL;
		if (g_jvm->AttachCurrentThread(&env,0) < 0)
		{
#ifdef QVODDEBUG
			_OUTPUT(ERROR1,"JavaCallbackFunction AttachCurrentThread fail\n");
#endif
			return ;
		}		 
	}

	if (g_callback != NULL && env != NULL && g_object != NULL)
	{
		jstring strwhat = para.buf==NULL?NULL:env->NewStringUTF(para.buf);
		env->CallVoidMethod(g_object,g_callback,para.id,para.result,strwhat);		
		if(para.buf != NULL) 
		{
			env->DeleteLocalRef(strwhat);
		}
	}
	if (ret == JNI_EDETACHED)
	{
		(g_jvm)->DetachCurrentThread();
	}

}

#if (MONGOTV)
JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_Init( JNIEnv* env, jobject obj,jstring strConfigPath,jstring strCachePath,
													  jint iHttpPort,jint iMaxUseSpace )
#else
#ifdef YFNET_OPEN_SDK
JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_Init( JNIEnv* env, jobject obj,jstring strConfigPath,jstring strCachePath,
													  jstring strSdkKey,jobject cls,jstring strMethod )
#else
JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_Init( JNIEnv* env, jobject obj,jstring strConfigPath,jstring strCachePath,
													  jstring strLogPath,jobject cls,jstring strMethod )
#endif
#endif
{
	if (pNet)
	{
		return E_NET_OK;
	}
#if !(MONGOTV)
	//非芒果tv，默认最大缓存800M
	int iMaxUseSpace = 800;
#endif
#ifdef YFNET_OPEN_SDK
	const char* pstrKey = env->GetStringUTFChars(strSdkKey, NULL);
	//检查key是否有效
	int iStatus = COpenSDKAuth::Instance()->CheckKeyStatus(string(pstrKey));
	env->ReleaseStringUTFChars(strSdkKey,pstrKey);
	if (-1 == iStatus)
	{
		//查询过程中发生网络错误
		return E_NET_CHECK_SDKKEY_NET_ERROR;
	}
	else	if (SDKKEY_STATUS_CLOSE == iStatus || SDKKEY_STATUS_SITENOTEXIST == iStatus || SDKKEY_STATUS_INVALID == iStatus)
	{
		//sdk key无效
		return E_NET_SDKKEY_INVALID;
	}		
#endif
	if (NULL==strConfigPath || NULL==strCachePath || NULL==strLogPath || NULL == strMethod || NULL == cls)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrConfigPath = env->GetStringUTFChars(strConfigPath, NULL);
	const char* pstrCachePath = env->GetStringUTFChars(strCachePath, NULL);
	const char *pstrLogPath = env->GetStringUTFChars(strLogPath, NULL);

	if (pstrLogPath && strlen(pstrLogPath)>0)
	{
		SetLogPath(pstrLogPath);
#ifndef ANDROID_DEBUG			
		QvodRemoveFolder(pstrLogPath);			// release版本只保存最近一份日志
		QvodMkdir(pstrLogPath);
#endif
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_Init into");
#endif

	InitPara para;
	para.pConfigPath = (char*)pstrConfigPath;
	para.pTempCachePath = (char*)pstrCachePath;
	para.pDownloadPath = (char*)pstrCachePath;				// 下载路径暂时同缓存路径一样
#ifdef MONGOTV
	para.iHttpPort = iHttpPort;
#else

	//非芒果tv版本，需要在初始化函数中设置回调函数
	const char* pstrMethod = env->GetStringUTFChars(strMethod, NULL);
	jclass cla = env->GetObjectClass(cls);

	g_callback = env->GetMethodID(cla, pstrMethod,"(IILjava/lang/String;)V");
	env->ReleaseStringUTFChars(strMethod,pstrMethod);
	if (NULL == g_callback)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"Java_com_yunfan_net_Yfnet_SetCallbackFunction GetMethodID fail\n");
#endif
		return E_NET_GET_JAVA_METHOD_FAIL;
	}
	if ((env)->GetJavaVM(&g_jvm) < 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"Java_com_yunfan_net_Yfnet_SetCallbackFunction GetJavaVM fail\n");
#endif
		return E_NET_GET_JVM_FAIL;
	}

	g_object = env->NewGlobalRef(cls);//创建一个全局变量

	env->DeleteLocalRef(cla);
#ifdef QVODDEBUG
	if (g_callback != NULL)
	{
		_OUTPUT(INFO,"SetCallbackFunction successful\n");
	}
#endif 	
	para.callback = JavaCallbackFunction;
#endif	
	para.maxspace = iMaxUseSpace;
	int ret = _InitYfNet(&pNet,para,lNet);
	env->ReleaseStringUTFChars(strConfigPath,pstrConfigPath);
	env->ReleaseStringUTFChars(strCachePath,pstrCachePath);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_Init out");
#endif
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_Clear( JNIEnv* env, jobject obj )
{
#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_Clear into");
#endif
	if (pNet)
	{
		_ClearYfNet(lNet);
		pNet = NULL;
		lNet = 0;
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_Clear out");
#endif
	return E_NET_OK;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_GetPrivateVersion(JNIEnv* env, jobject obj, jbyteArray version)
{
	if (NULL==version)
	{
		return E_NET_ERROR_PARA;
	}

	char szVersion[50] = {0};
	GetPrivateVersion(szVersion);
	env->SetByteArrayRegion(version, 0, 40, (const jbyte*)szVersion);

	return E_NET_OK;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetConfig(JNIEnv* env, jobject obj, jstring strJson)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strJson)
	{
		return E_NET_ERROR_PARA;
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_SetConfig into");
#endif
	if (pNet)
	{
		const char* pstrJson = env->GetStringUTFChars(strJson, NULL);
		pNet->SetConfig(pstrJson);
		env->ReleaseStringUTFChars(strJson,pstrJson);
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_SetConfig out");
#endif
	return E_NET_OK;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetNetworkStatus( JNIEnv* env, jobject obj,jboolean status )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	//return pNet->SetNetworkStatus(status);
	return -1;
}

#ifdef YFNET_OPEN_SDK
JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_CreateTask( JNIEnv* env, jobject obj,jstring strOrgUrl,jstring strKeyUrl,
															jbyteArray hashArray, jbyteArray httpProxyUrl )
#else
JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_CreateTask( JNIEnv* env, jobject obj,jstring strOrgUrl,jstring strKeyUrl,
															jstring strReferer,jbyteArray hashArray, jbyteArray httpProxyUrl )
#endif
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strOrgUrl || NULL==strKeyUrl || NULL==hashArray)
	{
		return E_NET_ERROR_PARA;
	}
#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_CreateTask into");
#endif

	const char* pstrOrgUrl = env->GetStringUTFChars(strOrgUrl, NULL);
	const char* pstrKeyUrl = env->GetStringUTFChars(strKeyUrl, NULL);
	const char* pstrReferer = NULL;
#ifndef YFNET_OPEN_SDK
	pstrReferer = env->GetStringUTFChars(strReferer, NULL);
#endif
	char szHash[41] = {0};
	char szHttpProxyUrl[1024] = {0};
	int ret = pNet->CreateTask(pstrOrgUrl,pstrKeyUrl,pstrReferer,szHash,szHttpProxyUrl,false);
	env->ReleaseStringUTFChars(strOrgUrl,pstrOrgUrl);
	env->ReleaseStringUTFChars(strKeyUrl,pstrKeyUrl);
#ifndef YFNET_OPEN_SDK
	env->ReleaseStringUTFChars(strReferer,pstrReferer);
#endif
	if (ret >= E_NET_OK)
	{
		env->SetByteArrayRegion(hashArray, 0, 40, (const jbyte*)szHash);
		if (NULL != httpProxyUrl)
		{
			env->SetByteArrayRegion(httpProxyUrl, 0, strlen(szHttpProxyUrl), (const jbyte*)szHttpProxyUrl);
		}
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_CreateTask out");
#endif
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_CreateHlsTask( JNIEnv* env, jobject obj,jstring strOrgUrl,jstring strKeyUrl,
														jboolean bIsOtherCdn, jbyteArray hashArray, jbyteArray httpProxyUrl)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strOrgUrl || NULL==strKeyUrl || NULL==hashArray || NULL==httpProxyUrl)
	{
		return E_NET_ERROR_PARA;
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_CreateHlsTask into");
#endif

	const char* pstrOrgUrl = env->GetStringUTFChars(strOrgUrl, NULL);
	const char* pstrKeyUrl = env->GetStringUTFChars(strKeyUrl, NULL);
	const char* pstrReferer = NULL;

	char szHash[41] = {0};
	char szHttpProxyUrl[1024] = {0};
	int ret = pNet->CreateHlsTask(pstrOrgUrl,pstrKeyUrl,bIsOtherCdn,szHash, szHttpProxyUrl);
	env->ReleaseStringUTFChars(strOrgUrl,pstrOrgUrl);
	env->ReleaseStringUTFChars(strKeyUrl,pstrKeyUrl);

	if (ret >= E_NET_OK)
	{
		env->SetByteArrayRegion(hashArray, 0, 40, (const jbyte*)szHash);
		env->SetByteArrayRegion(httpProxyUrl, 0, strlen(szHttpProxyUrl), (const jbyte*)szHttpProxyUrl);
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_CreateHlsTask out");
#endif
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_CreateUploadTask(JNIEnv* env, jobject obj, jint type, jstring strOrgUrl, jstring strFilePath, jbyteArray hashArray)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strOrgUrl || NULL==strFilePath || NULL==hashArray)
	{
		return E_NET_ERROR_PARA;
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_CreateUploadTask into");
#endif

	const char* pstrOrgUrl = env->GetStringUTFChars(strOrgUrl, NULL);
	const char* pstrFilePath= env->GetStringUTFChars(strFilePath, NULL);

	char szHash[41] = {0};
	int ret = pNet->CreateUploadTask(type, pstrOrgUrl, pstrFilePath, szHash);
	env->ReleaseStringUTFChars(strOrgUrl,pstrOrgUrl);
	env->ReleaseStringUTFChars(strFilePath,pstrFilePath);

	if (ret >= E_NET_OK)
	{
		env->SetByteArrayRegion(hashArray, 0, 40, (const jbyte*)szHash);
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_CreateUploadTask out");
#endif
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetHeadValue(JNIEnv* env, jobject obj,jstring strHash, jstring strHead, jstring strValue)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash || NULL==strHead || NULL==strValue)
	{
		return E_NET_ERROR_PARA;
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_SetHeadValue into");
#endif
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	const char* pstrHead = env->GetStringUTFChars(strHead, NULL);
	const char* pstrValue= env->GetStringUTFChars(strValue, NULL);
	int ret = pNet->SetHeadValue(pstrHash, pstrHead, pstrValue);
	env->ReleaseStringUTFChars(strHash,pstrHash);
	env->ReleaseStringUTFChars(strHead,pstrHead);
	env->ReleaseStringUTFChars(strValue,pstrValue);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_SetHeadValue out");
#endif
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_DeleteTask( JNIEnv* env, jobject obj,jstring strHash )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash)
	{
		return E_NET_ERROR_PARA;
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_DeleteTask into");
#endif
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	int ret = pNet->DeleteTask(pstrHash);
	env->ReleaseStringUTFChars(strHash,pstrHash);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_DeleteTask out");
#endif
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_RunTask( JNIEnv* env, jobject obj,jstring strHash )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash)
	{
		return E_NET_ERROR_PARA;
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_RunTask into");
#endif
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	int ret = pNet->RunTask(pstrHash);
	env->ReleaseStringUTFChars(strHash,pstrHash);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_RunTask out");
#endif
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_PauseTask( JNIEnv* env, jobject obj,jstring strHash )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash)
	{
		return E_NET_ERROR_PARA;
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_PauseTask into");
#endif
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	int ret = pNet->PauseTask(pstrHash);
	env->ReleaseStringUTFChars(strHash,pstrHash);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_PauseTask out");
#endif
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetTaskPlaying(JNIEnv* env, jobject obj,jstring strHash,jboolean bPlaying)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash)
	{
		return E_NET_ERROR_PARA;
	}

	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_SetTaskPlaying into hash=[%s] bPlaying=%d", pstrHash, bPlaying);
#endif
	int ret = pNet->SetTaskPlaying(pstrHash, bPlaying);
	env->ReleaseStringUTFChars(strHash,pstrHash);

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_SetTaskPlaying out");
#endif	
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_ReadFile( JNIEnv* env, jobject obj,jstring strHash,
														  jbyteArray dataArray,jlong start,jint len, jintArray readlen )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash || NULL==dataArray || NULL==readlen)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	char* pData = new char[len];
	long iRealReadLen[1];
	iRealReadLen[0] = 0;
	int ret = pNet->ReadFile(NULL,pstrHash,pData,start,len,iRealReadLen);
	env->ReleaseStringUTFChars(strHash,pstrHash);

	env->SetByteArrayRegion(dataArray,0,len,(const jbyte*)pData);
	env->SetIntArrayRegion(readlen, 0, 1, (const jint*)iRealReadLen);
	
	delete[] pData;
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetCachePath( JNIEnv* env, jobject obj,jstring strCachePath )
{
	int ret = E_NET_NO_INIT;
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strCachePath)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrCachePath = env->GetStringUTFChars(strCachePath, NULL);
	ret = pNet->SetTempCachePath(pstrCachePath);
	env->ReleaseStringUTFChars(strCachePath,pstrCachePath);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetAdPara( JNIEnv* env, jobject obj,jstring strAdCachePath,jint iAdMaxUseSpace )
{
	int ret = E_NET_NO_INIT;
	if (!pNet)
	{
		return ret;
	}
	if (NULL==strAdCachePath)
	{
		return E_NET_ERROR_PARA;
	}

	//const char* pstrCachePath = env->GetStringUTFChars(strAdCachePath, NULL);
	//pNet->SetAdSpaceCanUse(iAdMaxUseSpace);
	//int ret = pNet->SetAdCachePath(pstrCachePath);
	//env->ReleaseStringUTFChars(strAdCachePath,pstrCachePath);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetPlayingPos( JNIEnv* env, jobject obj,jstring strHash,jlong pos )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	int ret = pNet->SetPlayingPos(pstrHash,pos);
	env->ReleaseStringUTFChars(strHash,pstrHash);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetPlayingTimepoint(JNIEnv* env, jobject obj,jstring strHash,jint timepoint, jboolean bSeek, jintArray cachetime)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL == strHash || NULL == cachetime)
	{
		return E_NET_ERROR_PARA;
	}
	
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);

	int nCacheTime[1];
	nCacheTime[0] = 0;
	int ret = pNet->SetPlayingTimepoint(pstrHash,timepoint,bSeek, nCacheTime[0]);
	env->ReleaseStringUTFChars(strHash,pstrHash);

	if (ret >= E_NET_OK)
	{
		env->SetIntArrayRegion(cachetime, 0, 1, (const jint*)nCacheTime);
	}
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetSpaceCanUse( JNIEnv* env, jobject obj,jint space )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	pNet->SetSpaceCanUse(space);
	return E_NET_OK;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetCallbackFunction(JNIEnv* env, jobject obj,jobject cls,jstring strMethod,
																	 jstring strMethodPara)
{
	if (!pNet)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"Java_com_yunfan_net_Yfnet_SetCallbackFunction Net not init\n");
#endif
		return E_NET_NO_INIT;
	}
	if (NULL==cls || NULL==strMethod || NULL==strMethodPara)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrMethod = env->GetStringUTFChars(strMethod, NULL);
	const char* pstrMethodPara = env->GetStringUTFChars(strMethodPara, NULL);
	jclass cla = env->GetObjectClass(cls);
	g_callback = env->GetMethodID(cla, pstrMethod,pstrMethodPara);
	env->ReleaseStringUTFChars(strMethod,pstrMethod);
	env->ReleaseStringUTFChars(strMethodPara,pstrMethodPara);
	if (NULL == g_callback)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"Java_com_yunfan_net_Yfnet_SetCallbackFunction GetMethodID fail\n");
#endif
		return E_NET_GET_JAVA_METHOD_FAIL;
	}
	if ((env)->GetJavaVM(&g_jvm) < 0)
	{
#ifdef QVODDEBUG
		_OUTPUT(ERROR1,"Java_com_yunfan_net_Yfnet_SetCallbackFunction GetJavaVM fail\n");
#endif
		return E_NET_GET_JVM_FAIL;
	}
	
	g_object = env->NewGlobalRef(cls);//创建一个全局变量
	env->DeleteLocalRef(cla);
#ifdef QVODDEBUG
	if (g_callback != NULL)
	{
		_OUTPUT(INFO,"SetCallbackFunction successful\n");
	}
#endif
	//pNet->SetEventCallBackFunc(JavaCallbackFunction);
	return E_NET_OK;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_QueryTaskInfo( JNIEnv* env, jobject obj,jstring strHash,jobject info )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash || NULL==info)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	STaskInfo tmpInfo;
	NET_ERROR ret = pNet->QueryTaskInfo(pstrHash,&tmpInfo);	
	env->ReleaseStringUTFChars(strHash,pstrHash);
	if (ret < E_NET_OK)
	{
		return ret;
	}
	//把数据格式化到info中
	jclass clazz = env->GetObjectClass(info);
	jfieldID fid;
	//hash
 	jbyteArray jHash = env->NewByteArray(40);
 	env->SetByteArrayRegion(jHash, 0, 40, (jbyte*)tmpInfo.hash);
 	fid = env->GetFieldID(clazz, "hash", "[B");
 	env->SetObjectField(info, fid, jHash);
 	env->DeleteLocalRef(jHash);
	//filelen
	fid = env->GetFieldID(clazz, "filelen", "J");
	env->SetLongField(info, fid,tmpInfo.iFileLen);
	//downloadlen
	fid = env->GetFieldID(clazz, "downloadlen", "J");
	env->SetLongField(info, fid,tmpInfo.iDownloadLen);
	//uploadlen
	fid = env->GetFieldID(clazz, "uploadlen", "J");
	env->SetLongField(info, fid, tmpInfo.iUploadLen);
	//cdndownlen

	//downloadspeed
	fid = env->GetFieldID(clazz, "downloadspeed", "I");
	env->SetIntField(info, fid, tmpInfo.iDownloadSpeed);
	//uploadspeed	
	fid = env->GetFieldID(clazz, "uploadspeed", "I");
	env->SetIntField(info, fid, tmpInfo.iUploadSpeed);
	////cdndownspeed
	//fid = env->GetFieldID(clazz, "cdndownspeed", "I");
	//env->SetIntField(info, fid,tmpInfo.cdndownspeed);
	////p2pdownspeed
	//fid = env->GetFieldID(clazz, "p2pdownspeed", "I");
	//env->SetIntField(info, fid,tmpInfo.p2pdownspeed);
	//uploadspeed


	//status
	fid = env->GetFieldID(clazz, "status", "B");
	env->SetByteField(info, fid, tmpInfo.status);
	//filepath
	fid = env->GetFieldID(clazz, "filepath", "Ljava/lang/String;");
	jstring jstrFilepath = env->NewStringUTF(tmpInfo.szFilepath);
	env->SetObjectField(info, fid, jstrFilepath);
	env->DeleteLocalRef(jstrFilepath);

	env->DeleteLocalRef(clazz);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_TaskIsFinished(JNIEnv* env, jobject obj,jstring strHash,jbooleanArray bfinish)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash || NULL==bfinish)
	{
		return E_NET_ERROR_PARA;
	}	

	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);

	bool nFinish[1];
	nFinish[0] = 0;
	int ret = pNet->TaskIsFinished(pstrHash,nFinish[0]);
	env->ReleaseStringUTFChars(strHash,pstrHash);

	env->SetBooleanArrayRegion(bfinish, 0, 1, (const jboolean*)nFinish);

	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_EnableUpload( JNIEnv* env, jobject obj,jboolean status )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	//pNet->SetUploadStatus(status);
	return E_NET_OK;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetTaskDownload( JNIEnv* env, jobject obj,jstring strHash,jboolean bOption)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	NET_ERROR ret = pNet->SetTaskDownload(pstrHash,bOption,NULL);
	env->ReleaseStringUTFChars(strHash,pstrHash);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetPlayingStatus( JNIEnv* env, jobject obj,jstring strHash,jboolean status )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	int ret = pNet->SetTaskPlaying(pstrHash,status);
	env->ReleaseStringUTFChars(strHash,pstrHash);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetVideoDuration( JNIEnv* env, jobject obj,jstring strHash,jint duration )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	int ret = pNet->SetVideoDuration(pstrHash,duration);
	env->ReleaseStringUTFChars(strHash,pstrHash);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_CleanCache( JNIEnv* env, jobject obj )
{
	int ret = E_NET_NO_INIT;
	if (!pNet)
	{
		return ret;
	}

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_CleanCache into");
#endif
	ret = pNet->CleanCache();

#ifdef QVODDEBUG
	_OUTPUT(INFO, "Java_com_yunfan_net_Yfnet_CleanCache out");
#endif
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_UpdateHttpUrl( JNIEnv* env, jobject obj,jstring strHash,jstring strUrl )
{
	int ret = E_NET_NO_INIT;
	if (!pNet)
	{
		return ret;
	}
	if (NULL==strHash || NULL==strUrl)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	const char* pstrUrl = env->GetStringUTFChars(strUrl, NULL);
	
	ret = pNet->UpdateHttpOrgUrl(pstrHash,pstrUrl);

	env->ReleaseStringUTFChars(strHash,pstrHash);
	env->ReleaseStringUTFChars(strUrl,pstrUrl);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_NotifyVideoBuffering( JNIEnv* env, jobject obj,jstring strHash,jboolean bBuffering )
{
	int ret = E_NET_NO_INIT;
	if (!pNet)
	{
		return ret;
	}
	if (NULL==strHash)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);

	ret = pNet->NotifyVideoBuffering(pstrHash, bBuffering);
	env->ReleaseStringUTFChars(strHash,pstrHash);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_EnableP2pDownload( JNIEnv* env, jobject obj,jboolean status )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	//return pNet->EnableP2pDownload(status);
	return E_NET_NO_INIT;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetDeviceType( JNIEnv* env, jobject obj, jint type )
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	//return pNet->SetDeviceType(type);
	return E_NET_NO_INIT;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_SetLimitUploadSpeedMode(JNIEnv* env, jobject obj, jint mode)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	return pNet->SetLimitUploadSpeedMode((ELimitUploadSpeedMode)mode);
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_GetTaskInfo
		(JNIEnv* env, jobject obj,jstring strHash,jbyteArray dataArray,jint len,jintArray readlen)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==strHash || NULL==dataArray || NULL==readlen)
	{
		return E_NET_ERROR_PARA;
	}
	if (len <= 0)
	{
		return E_NET_BUFFER_NOT_ENOUGH;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	char* pData = new char[len]();
	int nLen = len;
	int ret = pNet->GetTaskInfo(pstrHash,pData, nLen);
	env->ReleaseStringUTFChars(strHash,pstrHash);
	long iRealReadLen[1];
	iRealReadLen[0] = nLen;
	env->SetIntArrayRegion(readlen, 0, 1, (const jint*)iRealReadLen);
	
	if (ret >= E_NET_OK)
	{
		env->SetByteArrayRegion(dataArray,0,len,(const jbyte*)pData);
	}
	
	delete[] pData;
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_GetDownloadingTaskLists
	(JNIEnv* env, jobject obj,jbyteArray dataArray, jintArray iBufLen)
{
	if (!pNet)
	{
		return E_NET_NO_INIT;
	}
	if (NULL==dataArray || NULL==iBufLen)
	{
		return E_NET_ERROR_PARA;
	}
	jint bufLen[1]; 
    jint i, sum = 0; 
    env->GetIntArrayRegion(iBufLen, 0, 1, bufLen); 
	if (bufLen[0] <= 0)
	{
		return E_NET_BUFFER_NOT_ENOUGH;
	}
	int nLen = bufLen[0];
	char* pData = new char[bufLen[0]];
	
	int ret = pNet->GetDownloadingTaskLists(pData, nLen);
	
	long iRealReadLen[1];
	iRealReadLen[0] = nLen;
	env->SetIntArrayRegion(iBufLen, 0, 1, (const jint*)iRealReadLen);
	
	if (E_NET_BUFFER_NOT_ENOUGH == ret)
	{
		return ret;
	}
	
	env->SetByteArrayRegion(dataArray,0,nLen,(const jbyte*)pData);	
	delete[] pData;
	
	return ret;	
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_UpdateClarityUrl( JNIEnv* env, jobject obj,jstring strHash,jstring strUrl , jstring strUrlKey, jint nClarity )
{
	int ret = E_NET_NO_INIT;
	if (!pNet)
	{
		return ret;
	}
	if (NULL==strHash || NULL==strUrl)
	{
		return E_NET_ERROR_PARA;
	}
	const char* pstrHash = env->GetStringUTFChars(strHash, NULL);
	const char* pstrUrl = env->GetStringUTFChars(strUrl, NULL);
	const char* pstrUrlKey = env->GetStringUTFChars(strUrlKey, NULL);
	ret = pNet->UpdateClarityUrl(pstrHash,pstrUrl, pstrUrlKey, nClarity);

	env->ReleaseStringUTFChars(strHash,pstrHash);
	env->ReleaseStringUTFChars(strUrl,pstrUrl);
	env->ReleaseStringUTFChars(strUrlKey, pstrUrlKey);
	return ret;
}

JNIEXPORT jint JNICALL Java_com_yunfan_net_Yfnet_UpdateNetType(JNIEnv* env, jobject obj, jint mode)
{
	int ret = E_NET_NO_INIT;
	if (!pNet)
	{
		return ret;
	}

	ret = pNet->UpdateNetType((ENetType)mode);

	return ret;	
}
