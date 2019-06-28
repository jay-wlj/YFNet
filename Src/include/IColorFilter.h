//------------------------------------------------------------------------------
// File: IContrst.h
//
// Desc: DirectShow adjust the contrast level.
//
// Copyright (c) QvodPlayer.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __ICONTRAST__
#define __ICONTRAST__

#ifdef __cplusplus
extern "C" {
#endif

	// {17F648FA-E9DC-4818-80B2-817F4EC51155}
// 	DEFINE_GUID(IID_IColortrans, 
// 		0x17f648fa, 0xe9dc, 0x4818, 0x80, 0xb2, 0x81, 0x7f, 0x4e, 0xc5, 0x11, 0x55);
	const IID IID_IColortrans = {0x17f648fa, 0xe9dc, 0x4818, {0x80, 0xb2, 0x81, 0x7f, 0x4e, 0xc5, 0x11, 0x55}};

    DECLARE_INTERFACE_(IColortrans, IUnknown)
    {
        STDMETHOD(get_bflip) (THIS_ bool *bflip) PURE;//
		STDMETHOD(put_bflip) (THIS_ bool bflip)  PURE;//

        STDMETHOD(get_blevel) (THIS_ bool *blevel) PURE;// 
	    STDMETHOD(put_blevel) (THIS_ bool blevel)  PURE;//
		STDMETHOD(get_90flipvalue) (THIS_ char    *c90flip) PURE; //0  左 1    右2
		STDMETHOD(put_90flipvalue) (THIS_ char    c90flip) PURE;	

		STDMETHOD(get_lightvalue) (THIS_ char *lightvalue) PURE;//亮度
		STDMETHOD(put_lightvalue) (THIS_ char  lightvalue) PURE;
		STDMETHOD(get_contrasvalue) (THIS_ char *contrasvalue) PURE;//对比
		STDMETHOD(put_contrasvalue) (THIS_ char  contrasvalue) PURE;
		STDMETHOD(get_saturavalue) (THIS_ char *saturavalue) PURE;//饱和
		STDMETHOD(put_saturasvalue) (THIS_ char  saturavalue) PURE;
		STDMETHOD(get_chromavalue) (THIS_ USHORT *chromavalue) PURE;//色调
		STDMETHOD(put_chromavalue) (THIS_ USHORT  chromavalue) PURE;

    };
	extern IColortrans *g_IColortrans; 

#ifdef __cplusplus
}
#endif

#endif // __ICONTRAST__

