// qmvplus.h : 定义 qmvplus 应用程序的依赖项
#ifndef _QMVPLUS_H_A092433C_E73A_493f_982D_7F34F24CB923
#define _QMVPLUS_H_A092433C_E73A_493f_982D_7F34F24CB923

namespace qmvplus
{
#define QMVPLUSAPI __stdcall

#ifdef __cplusplus
    extern "C" {
#endif
        /*@1*/
        void QMVPLUSAPI GetInterface(
            __in __out void ** ppv
            );
        typedef void (QMVPLUSAPI *pfGetInterface)(
            __in __out void **
            );
#ifdef __cplusplus
    }
#endif

    interface IVideoInsideQmvPlus
    {
        /// for video render particle effect//////////////////////////////////////
        virtual void RenderVideoInsideTag(
            __in void * pISurface,
            __in __int64 nCurrPlayPos
            ) = 0;
    };

    interface IQmvPlus
    {
        virtual void InitParams(
            __in void * pIQuery,
            __in void * pITagRender,
            __in long nTagOn,
            __in LPCTSTR szCurSkinName
            ) = 0;

        virtual void StartQueryVideoTag(
            void
            ) = 0;

        virtual void * MemToBitmap(
            __in void * vData,
            __in ULONG uSize
            ) = 0;

        virtual void TagState(
            __in short nState,
            __in bool bDrip,
            __in bool bDetail,
            __in bool bThumb
            ) = 0;

        virtual void AnalyzeVideoTag(
            __in bool bRestart
            ) = 0;

        virtual void MouseMoveOnPlaySlider(
            __in short x,
            __in short y
            ) = 0;

        virtual bool DrawTagOnPlaySlider(
            __in __out Graphics & gra
            ) = 0;

        virtual long MouseDownSeekFilter(
            __in long nType,
            __in __out float & fPos
            ) = 0;

        virtual void UpdateDetailOpenStatus(
            __in bool bSwitchingSkin,
            __in bool bSwitchingMC,
            __in bool bBeforeSwitch
            ) = 0;

        virtual void InitThumbYH(
            void
            ) = 0;

        virtual long GetCurrentTotalTagNum(
            void
            ) = 0;

        virtual void ClearTag(
            void
            ) = 0;

        virtual void QueryVideoTagFromServer(
            __in char * szHash,
            __in int len,
            __in WORD tagID
            ) = 0;

        virtual __int64 GetPositioningPlayPos(
            __in __int64 pos
            ) = 0;

        virtual void OpenTagUrl(
            __in LPCTSTR szMovieName,
            __in LPCTSTR szTagUrl,
            __in LPCTSTR szSeedUrl,
            __in __int64 startPos
            ) = 0;

        virtual bool IsThereATagAtCurrentPos(
            void
            ) = 0;

        /*
        ** call this function to delete a function instance, and free resource.
        ** the interface is invalid after Release return, no more usage again.
        */
        virtual void Release(
            void
            ) = 0;
    };
}
#endif // _QMVPLUS_H_A092433C_E73A_493f_982D_7F34F24CB923
