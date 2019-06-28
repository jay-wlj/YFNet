// pausebarr.h : 定义 paubar 应用程序的依赖项
#ifndef _PAUSEBARR_H_6E60D1F2_3F6F_4396_80F6_2892AD09DE94
#define _PAUSEBARR_H_6E60D1F2_3F6F_4396_80F6_2892AD09DE94

namespace pauseBarr
{
#define PAUSEBARR __stdcall

#ifdef __cplusplus
    extern "C" {
#endif
        /*@1*//*to get a function instance*/
        void PAUSEBARR GetInterface(
            __in __out void ** ppv
            );
        typedef void ( PAUSEBARR * pfGetInterface )(
            __in __out void **
            );
#ifdef __cplusplus
    }
#endif

    interface IPauseBarr
    {
        /*
        ** call this function when play status changed,
        ** to start up the pause barrage and show it,
        ** or to end up the pause barrage and free all used resource.
        */
        virtual void UpdateBarr(
            void
            ) = 0;

        /*
        ** call this function to delete a function instance, and free resource.
        ** the interface is invalid after Release return, no more usage again.
        */
        virtual void Release(
            void
            ) = 0;

        /*
        ** call this function to wonder if pause barrage exist.
        */
        virtual bool Alive(
            void
            ) = 0;
    };
}

#endif // _PAUSEBARR_H_6E60D1F2_3F6F_4396_80F6_2892AD09DE94
