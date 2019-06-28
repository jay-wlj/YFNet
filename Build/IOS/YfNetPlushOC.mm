//
//  YfNetPlush.m
//  YfNetPlush
//
//  Created by 陈志标 on 2016/10/13.
//  Copyright © 2016年 chenzhibiao. All rights reserved.
//

#import "YfNetPlushOC.h"
#import "IYfNetPlush.h"
#import "common-commonfun.h"
#import "common-utility.h"

YfNetPlushOC*       gYunFanNet  = nil;
IYfNetPlush*        g_pYfNet    = NULL;
long                g_lPointer  = 0;

@interface YfNetPlushOC ()

@property (nonatomic, assign) id <YunFanNetDelegate> delegate;

void YFcallback(SCallBackPara& para);

@end

@implementation YfNetPlushOC

-(id)init
{
    self = [super init];
    return self;
}

-(YFNET_ERROR) InitYunFanNet: (NSString*)pConfigPath pCachePath: (NSString*)pCachePath maxspace: (int)maxspace delegate: (id) delegate;
{
    _delegate = delegate;
    gYunFanNet = self;
    if (g_pYfNet != NULL)
    {
        return YF_NET_OK;
    }
    InitPara para;
    para.maxspace = maxspace;
    para.pTempCachePath = (char*)[pCachePath UTF8String];
    para.pConfigPath = (char*)[pConfigPath UTF8String];
    para.pDownloadPath = (char*)[pCachePath UTF8String];
    para.callback = YFcallback;
    YFNET_ERROR error =  (YFNET_ERROR)_InitYfNet(&g_pYfNet, para, g_lPointer);
#ifdef QVODDEBUG
    _OUTPUT(INFO,"%s point = %u\n",__FUNCTION__,g_pYfNet);
#endif
    return error;
}

-(void)ClearYfNet
{
#ifdef QVODDEBUG
    _OUTPUT(INFO,"%s point = %u\n",__FUNCTION__,g_pYfNet);
#endif
    _ClearYfNet(g_lPointer);
    g_pYfNet = NULL;
    g_lPointer = 0;
}


-(YFNET_ERROR) YFGetPrivateVersion: (NSString**)pVersion
{
    char szBuffer[50]={0};
    bool error = GetPrivateVersion(szBuffer);
    *pVersion = [NSString stringWithFormat: @"%s", szBuffer];
    return (YFNET_ERROR)(error? E_NET_OK: E_NET_FALSE);
}

-(YFNET_ERROR) YFSetConfig: (NSString*)pstrJson
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    YFNET_ERROR err = (YFNET_ERROR)g_pYfNet->SetConfig([pstrJson UTF8String]);
    return err;
}


-(YFNET_ERROR) YFCreateTask: (NSString*)pstrSrcUrl pstrKeyurl:(NSString*)pstrKeyurl pstrReferer:(NSString*) pstrReferer pstrHash:(NSString**)pstrHash playURL:(NSString**)playurl
{
#ifdef QVODDEBUG
    _OUTPUT(INFO,"%s point = %u\n",__FUNCTION__,g_pYfNet);
#endif
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    
    char szBuffer[500]={0};
    char szProxyUrl[500] = {0};
    
    YFNET_ERROR error =  (YFNET_ERROR)g_pYfNet->CreateTask([pstrSrcUrl UTF8String],[pstrKeyurl UTF8String], [pstrReferer UTF8String],szBuffer, szProxyUrl, false);
    
    *pstrHash = [NSString stringWithFormat:@"%s", szBuffer];
    
    
    *playurl = [[NSString alloc] initWithFormat:@"%s" ,szProxyUrl];
    //*playurl = [[NSString alloc] initWithFormat:@"http://127.0.0.1:%d/yfhttpagent/%@/playlist.m3u8", g_httpport, *pstrHash];
    return  error;
}


-(YFNET_ERROR) YFCreateUploadTask: (int)type pstrSrcUrl:(NSString*)pstrSrcUrl pstrFilePath:(NSString*)pstrFilePath pstrHash:(NSString**) pstrHash
{
#ifdef QVODDEBUG
    _OUTPUT(INFO,"%s point = %u\n",__FUNCTION__,g_pYfNet);
#endif
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    
    char szBuffer[500]={0};
    
    
    YFNET_ERROR error =  (YFNET_ERROR)g_pYfNet->CreateUploadTask(type, [pstrSrcUrl UTF8String],[pstrFilePath UTF8String],szBuffer);
    
    *pstrHash = [NSString stringWithFormat:@"%s", szBuffer];
    
    return  error;
}

-(YFNET_ERROR) YFSetHeadValue: (NSString*)pstrHash pstrHead:(NSString*)pstrHead pstrValue:(NSString*)pstrValue
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->SetHeadValue( [pstrHash UTF8String], [pstrHead UTF8String], [pstrValue UTF8String]);
}

-(YFNET_ERROR) YFDeleteTask: (NSString*)pstrHash
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->DeleteTask( [pstrHash UTF8String]);
}

-(YFNET_ERROR) YFPauseTask: (NSString*) pstrHash
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->PauseTask( [pstrHash UTF8String]);
}

-(YFNET_ERROR) YFRunTask: (NSString*) pstrHash
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->RunTask( [pstrHash UTF8String]);
}

-(YFNET_ERROR) YFSetCachePath: (NSString*) pstrPath
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->SetTempCachePath( [pstrPath UTF8String] );
}

void YFcallback(SCallBackPara& para)
{
#ifdef QVODDEBUG
    printf("YFcallback gYunFanNet=%d delegate=%d\r\n", gYunFanNet, gYunFanNet.delegate);
#endif
    if(gYunFanNet && gYunFanNet.delegate != nil)
    {
        [gYunFanNet.delegate callback: (YFSCallBackPara*)&para];
    }
}

-(YFNET_ERROR) YFSetTaskDownload: (NSString*)pstrHash bDownload: (BOOL)bDownload
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->SetTaskDownload([pstrHash UTF8String], bDownload, NULL);
}

-(YFNET_ERROR) YFUpdateHttpOrgUrl: (NSString*)pstrHash pstrSrcUrl:(NSString*)pstrSrcUrl
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->UpdateHttpOrgUrl([pstrHash UTF8String], [pstrSrcUrl UTF8String]);
}

-(YFNET_ERROR) YFSetTaskPlaying: (NSString*)pstrHash bPlaying: (BOOL)bPlaying
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->SetTaskPlaying( [pstrHash UTF8String], bPlaying);
}

-(YFNET_ERROR) YFTaskIsPlaying: (NSString*) pstrHash bPlaying:(BOOL*)bPlaying
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    if (NULL == bPlaying)
    {
        return (YFNET_ERROR)E_NET_ERROR_PARA;
    }
    bool bRet = false;
    YFNET_ERROR ret = (YFNET_ERROR)g_pYfNet->TaskIsPlaying([pstrHash UTF8String], bRet);
    if (bPlaying)
    {
        *bPlaying = bRet;
    }
    return ret;
}

-(YFNET_ERROR) YFNotifyVideoBuffering: (NSString*)pstrHash bBuffering: (BOOL)bBuffering
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->NotifyVideoBuffering([pstrHash UTF8String], bBuffering);
    
}

-(YFNET_ERROR) YFSetSpaceCanUse: (int) spacecanuse
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->SetSpaceCanUse(spacecanuse);
}

-(YFNET_ERROR) YFQueryTaskByHash: (NSString*)pstrHash TaskInfo: (struct YFSTaskInfo*)pTaskInfo
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    STaskInfo info;
    YFNET_ERROR err = (YFNET_ERROR)g_pYfNet->QueryTaskInfo( [pstrHash UTF8String], &info);
    memcpy(pTaskInfo->hash, info.hash, 40);
    strcpy(pTaskInfo->filepath, info.szFilepath);
    pTaskInfo->filelen = info.iFileLen;
    pTaskInfo->downloadlen = info.iDownloadLen;
    pTaskInfo->downloadspeed = info.iDownloadSpeed;
    pTaskInfo->uploadlen = info.iUploadLen;
    pTaskInfo->uploadspeed = info.iUploadSpeed;
    pTaskInfo->status = info.status;
    
    printf("YFQueryTaskByHash uplen=%lld\n", pTaskInfo->uploadlen);
    return err;
}

-(YFNET_ERROR) YFSetVideoDuration: (NSString*)pstrHash duration: (int)duration
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->SetVideoDuration([pstrHash UTF8String], duration);
}

-(YFNET_ERROR) YFSetPlayingTimepoint: (NSString*) pstrHash timepoint:(int)timepoint bSeek:(BOOL)bSeek iCacheTime:(int*)iCacheTime
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    int iTime = 0;
    YFNET_ERROR ret = (YFNET_ERROR)g_pYfNet->SetPlayingTimepoint([pstrHash UTF8String], timepoint, bSeek, iTime);
    if (iCacheTime)
    {
        *iCacheTime = iTime;
    }
    return ret;
}

-(YFNET_ERROR) YFTaskIsFinished: (NSString*) pstrHash bFinish:(BOOL*)bFinish
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    if (NULL == bFinish)
    {
        return (YFNET_ERROR)E_NET_ERROR_PARA;
    }
    bool finished = false;
    YFNET_ERROR ret = (YFNET_ERROR)g_pYfNet->TaskIsFinished([pstrHash UTF8String], finished);
    if (bFinish)
    {
        *bFinish = finished;
    }
    return ret;
}


-(YFNET_ERROR) YFSetTaskDownload: (NSString*)pstrHash bOption: (BOOL)bOption pSavePath:(NSString*) pSavePath;
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR) g_pYfNet->SetTaskDownload( [pstrHash UTF8String], bOption, [pSavePath UTF8String]);
}

-(YFNET_ERROR) YFSetUploadStatus: (BOOL)enableUpload
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return YF_NET_OK;
    //return (YFNET_ERROR)g_pYfNet->SetUploadStatus( enableUpload);
}

-(YFNET_ERROR)YFCleanCache
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->CleanCache();
}

-(YFNET_ERROR)YFSetCurrentScene:(YFNET_SCENE)scene
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->SetCurrentScene((ECurrentScene)scene);
}
-(YFNET_ERROR) YFSetLogLevel: (int)nLevel
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    return (YFNET_ERROR)g_pYfNet->SetLogLevel(nLevel);
}

-(YFNET_ERROR) YFGetTaskInfo:(NSString*) pstrHash pstrBuf:(NSString**)pstrBuf
{
#ifdef QVODDEBUG
    _OUTPUT(INFO,"%s point = %u\n",__FUNCTION__,g_pYfNet);
#endif
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    
    int len = 500;
    char *pBuf = new char[len+1]();
    
    YFNET_ERROR error = (YFNET_ERROR)g_pYfNet->GetTaskInfo([pstrHash UTF8String],pBuf, len);
    if (YF_NET_BUFFER_NOT_ENOUGH == error)
    {
        if (pBuf)
        {
            delete []pBuf;
            pBuf = NULL;
        }
        pBuf = new char[len+1]();
        error = (YFNET_ERROR)g_pYfNet->GetTaskInfo([pstrHash UTF8String],pBuf, len);
    }
    //*pstrBuf = [NSString stringWithFormat:@"%s", pBuf];
    *pstrBuf = [[NSString alloc] initWithFormat:@"%s" ,pBuf];
    if (pBuf)
    {
        delete []pBuf;
        pBuf = NULL;
    }
    return  error;
}

-(YFNET_ERROR) YFGetDownloadingTaskLists:(NSString**) pstrBuf
{
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
    
    int len = 500;
    char *pBuf = new char[len+1]();
    
    YFNET_ERROR error = (YFNET_ERROR)g_pYfNet->GetDownloadingTaskLists(pBuf, len);
    if (YF_NET_BUFFER_NOT_ENOUGH == error)
    {
        if (pBuf)
        {
            delete []pBuf;
            pBuf = NULL;
        }
        pBuf = new char[len+1]();
        error = (YFNET_ERROR)g_pYfNet->GetDownloadingTaskLists(pBuf, len);
    }
    //*pstrBuf = [NSString stringWithFormat:@"%s", pBuf];
    *pstrBuf = [[NSString alloc] initWithFormat:@"%s" ,pBuf];
    if (pBuf)
    {
        delete []pBuf;
        pBuf = NULL;
    }
    return  error;
}

-(YFNET_ERROR) YFUpdateNetType:(YFNET_NETYPE) nettype
{
#ifdef QVODDEBUG
    _OUTPUT(INFO,"%s nettype = %u\n",__FUNCTION__,nettype);
#endif
    if (!g_pYfNet)
    {
        return (YFNET_ERROR)E_NET_NO_INIT;
    }
	YFNET_ERROR error = (YFNET_ERROR)g_pYfNet->UpdateNetType((ENetType)nettype);
	return error;
}

@end
