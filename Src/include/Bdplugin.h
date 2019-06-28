

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0499 */
/* at Mon Aug 12 15:34:28 2013
 */
/* Compiler settings for .\Bdplugin.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Bdplugin_h__
#define __Bdplugin_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IBdCtrl_FWD_DEFINED__
#define __IBdCtrl_FWD_DEFINED__
typedef interface IBdCtrl IBdCtrl;
#endif 	/* __IBdCtrl_FWD_DEFINED__ */


#ifndef ___IBdCtrlEvents_FWD_DEFINED__
#define ___IBdCtrlEvents_FWD_DEFINED__
typedef interface _IBdCtrlEvents _IBdCtrlEvents;
#endif 	/* ___IBdCtrlEvents_FWD_DEFINED__ */


#ifndef __BdCtrl_FWD_DEFINED__
#define __BdCtrl_FWD_DEFINED__

#ifdef __cplusplus
typedef class BdCtrl BdCtrl;
#else
typedef struct BdCtrl BdCtrl;
#endif /* __cplusplus */

#endif 	/* __BdCtrl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IBdCtrl_INTERFACE_DEFINED__
#define __IBdCtrl_INTERFACE_DEFINED__

/* interface IBdCtrl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBdCtrl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1fc7ba2c-1d61-4a7e-898a-d12c387761d5")
    IBdCtrl : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_URL( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_URL( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Autoplay( 
            /* [retval][out] */ INT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Autoplay( 
            /* [in] */ INT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FullScreen( 
            /* [retval][out] */ INT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FullScreen( 
            /* [in] */ INT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LastWebPage( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LastWebPage( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NextWebPage( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_NextWebPage( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NextCacheURL( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_NextCacheURL( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OnPause( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OnPause( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OnPlay( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OnPlay( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OnFirstBufferingStart( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OnFirstBufferingStart( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OnFirstBufferingEnd( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OnFirstBufferingEnd( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OnPlayBufferingStart( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OnPlayBufferingStart( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OnPlayBufferingEnd( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OnPlayBufferingEnd( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OnComplete( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OnComplete( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowStartClient( 
            /* [retval][out] */ INT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowStartClient( 
            /* [in] */ INT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OnError( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OnError( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Play( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVersion( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsPlaying( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsPause( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsStop( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsBuffing( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetEvent( 
            /* [in] */ BSTR bstrType,
            /* [in] */ BSTR bstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEvent( 
            /* [in] */ BSTR bstrType,
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDownPercent( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBdCtrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBdCtrl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBdCtrl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBdCtrl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBdCtrl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBdCtrl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBdCtrl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBdCtrl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_URL )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_URL )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Autoplay )( 
            IBdCtrl * This,
            /* [retval][out] */ INT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Autoplay )( 
            IBdCtrl * This,
            /* [in] */ INT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FullScreen )( 
            IBdCtrl * This,
            /* [retval][out] */ INT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FullScreen )( 
            IBdCtrl * This,
            /* [in] */ INT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LastWebPage )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LastWebPage )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NextWebPage )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_NextWebPage )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NextCacheURL )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_NextCacheURL )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OnPause )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OnPause )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OnPlay )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OnPlay )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OnFirstBufferingStart )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OnFirstBufferingStart )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OnFirstBufferingEnd )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OnFirstBufferingEnd )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OnPlayBufferingStart )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OnPlayBufferingStart )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OnPlayBufferingEnd )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OnPlayBufferingEnd )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OnComplete )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OnComplete )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowStartClient )( 
            IBdCtrl * This,
            /* [retval][out] */ INT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShowStartClient )( 
            IBdCtrl * This,
            /* [in] */ INT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OnError )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OnError )( 
            IBdCtrl * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Play )( 
            IBdCtrl * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IBdCtrl * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsPlaying )( 
            IBdCtrl * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsPause )( 
            IBdCtrl * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsStop )( 
            IBdCtrl * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsBuffing )( 
            IBdCtrl * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetEvent )( 
            IBdCtrl * This,
            /* [in] */ BSTR bstrType,
            /* [in] */ BSTR bstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEvent )( 
            IBdCtrl * This,
            /* [in] */ BSTR bstrType,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDownPercent )( 
            IBdCtrl * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IBdCtrlVtbl;

    interface IBdCtrl
    {
        CONST_VTBL struct IBdCtrlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBdCtrl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBdCtrl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBdCtrl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBdCtrl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBdCtrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBdCtrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBdCtrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBdCtrl_get_URL(This,pVal)	\
    ( (This)->lpVtbl -> get_URL(This,pVal) ) 

#define IBdCtrl_put_URL(This,newVal)	\
    ( (This)->lpVtbl -> put_URL(This,newVal) ) 

#define IBdCtrl_get_Autoplay(This,pVal)	\
    ( (This)->lpVtbl -> get_Autoplay(This,pVal) ) 

#define IBdCtrl_put_Autoplay(This,newVal)	\
    ( (This)->lpVtbl -> put_Autoplay(This,newVal) ) 

#define IBdCtrl_get_FullScreen(This,pVal)	\
    ( (This)->lpVtbl -> get_FullScreen(This,pVal) ) 

#define IBdCtrl_put_FullScreen(This,newVal)	\
    ( (This)->lpVtbl -> put_FullScreen(This,newVal) ) 

#define IBdCtrl_get_LastWebPage(This,pVal)	\
    ( (This)->lpVtbl -> get_LastWebPage(This,pVal) ) 

#define IBdCtrl_put_LastWebPage(This,newVal)	\
    ( (This)->lpVtbl -> put_LastWebPage(This,newVal) ) 

#define IBdCtrl_get_NextWebPage(This,pVal)	\
    ( (This)->lpVtbl -> get_NextWebPage(This,pVal) ) 

#define IBdCtrl_put_NextWebPage(This,newVal)	\
    ( (This)->lpVtbl -> put_NextWebPage(This,newVal) ) 

#define IBdCtrl_get_NextCacheURL(This,pVal)	\
    ( (This)->lpVtbl -> get_NextCacheURL(This,pVal) ) 

#define IBdCtrl_put_NextCacheURL(This,newVal)	\
    ( (This)->lpVtbl -> put_NextCacheURL(This,newVal) ) 

#define IBdCtrl_get_OnPause(This,pVal)	\
    ( (This)->lpVtbl -> get_OnPause(This,pVal) ) 

#define IBdCtrl_put_OnPause(This,newVal)	\
    ( (This)->lpVtbl -> put_OnPause(This,newVal) ) 

#define IBdCtrl_get_OnPlay(This,pVal)	\
    ( (This)->lpVtbl -> get_OnPlay(This,pVal) ) 

#define IBdCtrl_put_OnPlay(This,newVal)	\
    ( (This)->lpVtbl -> put_OnPlay(This,newVal) ) 

#define IBdCtrl_get_OnFirstBufferingStart(This,pVal)	\
    ( (This)->lpVtbl -> get_OnFirstBufferingStart(This,pVal) ) 

#define IBdCtrl_put_OnFirstBufferingStart(This,newVal)	\
    ( (This)->lpVtbl -> put_OnFirstBufferingStart(This,newVal) ) 

#define IBdCtrl_get_OnFirstBufferingEnd(This,pVal)	\
    ( (This)->lpVtbl -> get_OnFirstBufferingEnd(This,pVal) ) 

#define IBdCtrl_put_OnFirstBufferingEnd(This,newVal)	\
    ( (This)->lpVtbl -> put_OnFirstBufferingEnd(This,newVal) ) 

#define IBdCtrl_get_OnPlayBufferingStart(This,pVal)	\
    ( (This)->lpVtbl -> get_OnPlayBufferingStart(This,pVal) ) 

#define IBdCtrl_put_OnPlayBufferingStart(This,newVal)	\
    ( (This)->lpVtbl -> put_OnPlayBufferingStart(This,newVal) ) 

#define IBdCtrl_get_OnPlayBufferingEnd(This,pVal)	\
    ( (This)->lpVtbl -> get_OnPlayBufferingEnd(This,pVal) ) 

#define IBdCtrl_put_OnPlayBufferingEnd(This,newVal)	\
    ( (This)->lpVtbl -> put_OnPlayBufferingEnd(This,newVal) ) 

#define IBdCtrl_get_OnComplete(This,pVal)	\
    ( (This)->lpVtbl -> get_OnComplete(This,pVal) ) 

#define IBdCtrl_put_OnComplete(This,newVal)	\
    ( (This)->lpVtbl -> put_OnComplete(This,newVal) ) 

#define IBdCtrl_get_ShowStartClient(This,pVal)	\
    ( (This)->lpVtbl -> get_ShowStartClient(This,pVal) ) 

#define IBdCtrl_put_ShowStartClient(This,newVal)	\
    ( (This)->lpVtbl -> put_ShowStartClient(This,newVal) ) 

#define IBdCtrl_get_OnError(This,pVal)	\
    ( (This)->lpVtbl -> get_OnError(This,pVal) ) 

#define IBdCtrl_put_OnError(This,newVal)	\
    ( (This)->lpVtbl -> put_OnError(This,newVal) ) 

#define IBdCtrl_Play(This)	\
    ( (This)->lpVtbl -> Play(This) ) 

#define IBdCtrl_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#define IBdCtrl_GetVersion(This,pVal)	\
    ( (This)->lpVtbl -> GetVersion(This,pVal) ) 

#define IBdCtrl_IsPlaying(This,pVal)	\
    ( (This)->lpVtbl -> IsPlaying(This,pVal) ) 

#define IBdCtrl_IsPause(This,pVal)	\
    ( (This)->lpVtbl -> IsPause(This,pVal) ) 

#define IBdCtrl_IsStop(This,pVal)	\
    ( (This)->lpVtbl -> IsStop(This,pVal) ) 

#define IBdCtrl_IsBuffing(This,pVal)	\
    ( (This)->lpVtbl -> IsBuffing(This,pVal) ) 

#define IBdCtrl_SetEvent(This,bstrType,bstrValue)	\
    ( (This)->lpVtbl -> SetEvent(This,bstrType,bstrValue) ) 

#define IBdCtrl_GetEvent(This,bstrType,pVal)	\
    ( (This)->lpVtbl -> GetEvent(This,bstrType,pVal) ) 

#define IBdCtrl_GetDownPercent(This,pVal)	\
    ( (This)->lpVtbl -> GetDownPercent(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBdCtrl_INTERFACE_DEFINED__ */



#ifndef __BdpluginLib_LIBRARY_DEFINED__
#define __BdpluginLib_LIBRARY_DEFINED__

/* library BdpluginLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_BdpluginLib;

#ifndef ___IBdCtrlEvents_DISPINTERFACE_DEFINED__
#define ___IBdCtrlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IBdCtrlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IBdCtrlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5851f89b-b186-4025-b877-b56bcf5de4fd")
    _IBdCtrlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IBdCtrlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IBdCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IBdCtrlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IBdCtrlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IBdCtrlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IBdCtrlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IBdCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IBdCtrlEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IBdCtrlEventsVtbl;

    interface _IBdCtrlEvents
    {
        CONST_VTBL struct _IBdCtrlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IBdCtrlEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IBdCtrlEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IBdCtrlEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IBdCtrlEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IBdCtrlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IBdCtrlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IBdCtrlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IBdCtrlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_BdCtrl;

#ifdef __cplusplus

class DECLSPEC_UUID("02e2d748-67f8-48b4-8ab4-0a085374bb99")
BdCtrl;
#endif
#endif /* __BdpluginLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


