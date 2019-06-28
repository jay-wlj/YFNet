//------------------------------------------------------------------------------
// File: ISound.h
//
// Desc: DirectShow  adjust the sound level.
//
// Copyright (c) QvodPlayer.  All rights reserved.
//------------------------------------------------------------------------------
#ifndef __ISOUNDTRANS__
#define __ISOUNDTRANS__

#ifdef __cplusplus
extern "C" {
#endif

	// {7DA9CECE-A81A-471d-8CD3-98736CBA05DC}
	//DEFINE_GUID(IID_ISoundtrans, 0x7da9cece, 0xa81a, 0x471d, 0x8c, 0xd3, 0x98, 0x73, 0x6c, 0xba, 0x5, 0xdc);#ifndef QVODPLAYMEDIA_EXPORTS	const IID IID_ISoundtrans = {0x7da9cece, 0xa81a, 0x471d, {0x8c, 0xd3, 0x98, 0x73, 0x6c, 0xba, 0x5, 0xdc}};#endif
#ifndef QVODPLAYMEDIA_EXPORTS
EXTERN_GUID(IID_ISoundtrans, 0x7da9cece, 0xa81a, 0x471d, 0x8c, 0xd3, 0x98, 0x73, 0x6c, 0xba, 0x5, 0xdc);
#endif

    DECLARE_INTERFACE_(ISoundtrans, IUnknown)
    {
		//put_channel设置左右声道
		//cchannel:0 混合音，1 只有左声道有声音，2 只有右声道有声音，3 默认（立体声）4 左声道扩展到全部  5 右声道扩展到全部
		STDMETHOD(put_channel) (THIS_ char cchannel) PURE; //
		STDMETHOD(get_channel) (THIS_ char *cchannel)PURE;
		//静音
		STDMETHOD(put_mute)	   (THIS_ bool bmute) PURE; //
		STDMETHOD(get_mute)	   (THIS_ bool *bmute)PURE; //
		//声音延迟　　单位　100 ns
		STDMETHOD(put_soundshift)(THIS_ LONGLONG lshift)PURE;
		STDMETHOD(get_soundshift)(THIS_ LONGLONG *lshift)PURE;
		//声音放大 正常值为10，ladd / 10 为放大的倍数，但小于10声音不会变小
		STDMETHOD(put_addsound)	 (THIS_ LONG ladd)PURE;
		STDMETHOD(get_addsound)	 (THIS_ LONG *ladd)PURE;
		//设置声音均衡　 取值范围：0 -- 200	
		STDMETHOD(put_eq)		 (THIS_ int	 *neq)PURE;
		STDMETHOD(get_eq)		 (THIS_ int  *neq)PURE;
		//打开 关闭声音均衡
		STDMETHOD(put_seq)		 (THIS_ bool bseq)PURE;
		STDMETHOD(get_seq)		 (THIS_ bool *bseq)PURE;
		//声音平衡  通过调用IID_IBasicAudio实现  取值范围：-100 100
		STDMETHOD(put_balance)	 (THIS_ LONG lbal)PURE;
		STDMETHOD(get_balance)	 (THIS_ LONG *lbal)PURE;
		
		//以下功能未实现，已荒废
		STDMETHOD(put_astream)	 (THIS_ DWORD nastream)PURE;
		STDMETHOD(get_astream)	 (THIS_ DWORD *ncount, DWORD *ncur)PURE;
		STDMETHOD(put_wav)		 (THIS_ LONG  bsavetype, char *psavepath)PURE;
		STDMETHOD(get_wav)		 (THIS_ LONG *bsaveteyp, char *psavepath)PURE;
		STDMETHOD(init)			 ()PURE;
		//设置（获取）当前声道数
		STDMETHOD(PutChannelNum)	(float channel)PURE;
		STDMETHOD(GetChannelNum)	(float* pchannel)PURE;
    };
	extern ISoundtrans *g_ISoundtrans; 

#ifdef __cplusplus
}
#endif

#endif // __ISOUNDTRANS__