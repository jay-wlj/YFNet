#ifndef __ISUB_H__
#define __ISUB_H__


#ifdef __cplusplus
extern "C" {
#endif
#ifndef QVODPLAYMEDIA_EXPORTS
	const GUID IID_COLORFILTER_SUB = {0x7e68b1a4, 0x306c
		, 0x4ed4, {0xb2, 0x62, 0x31, 0x95, 0xa, 0x83, 0x15, 0x29}};
#endif
	DECLARE_INTERFACE_(ISUBTITLE, IUnknown)
	{
		//路径最大长度: MAX_PATH * sizeof(WCHAR)
		//language   语言设置 简体 繁体 等
		STDMETHOD(put_path) (THIS_
			WCHAR* wszPath,
			int language  = 0      
			) PURE;
		STDMETHOD(get_path) (THIS_
			WCHAR* wszPathBuffer,
			int   nMaxBufferSize,
			int *language 
			) PURE;
		STDMETHOD(put_visable) (THIS_
			bool bVisable
			) PURE;
		STDMETHOD(get_visable) (THIS_
			bool *bVisable
			) PURE;
		STDMETHOD(put_clear) (THIS_
			) PURE;
		STDMETHOD(put_font) (THIS_
			LOGFONT lf
			) PURE;
		STDMETHOD(get_font) (THIS_
			LOGFONT *lf
			) PURE;
		//颜色的格式是 A B G R  去掉高位A正好是COLORREF
		STDMETHOD(put_fontColor) (THIS_
			DWORD cr
			) PURE;
		STDMETHOD(get_fontColor) (THIS_
			DWORD *cr
			) PURE;
		STDMETHOD(put_OutLineColor) (THIS_
			DWORD cr
			) PURE;
		STDMETHOD(get_OutLineColor) (THIS_
			DWORD *cr
			) PURE;

		STDMETHOD(put_place) (THIS_
			int place,
			int cx,
			int cy
			) PURE;
		STDMETHOD(get_place) (THIS_
			int *place,
			int *cx,
			int *cy
			) PURE;
		STDMETHOD(put_delayTime) (THIS_  //单位:ms
			int time
			) PURE;
		STDMETHOD(get_delayTime) (THIS_ 
			int *time
			) PURE;

			//	SUB_TIME_NOUSER = 0,	//不使用
			//	SUB_TIME_CUR,			//当前时间
			//	SUB_TIME_END,			//剩余时间
			//	SUB_TIME_SYSTEM			//系统时间
		STDMETHOD(put_timeSub) (THIS_ 
			int type
			) PURE;
		STDMETHOD(get_timeSub) (THIS_  
			int *type
			) PURE;
		//设置显示时间的字体  字体高最小值为10 最大值为72
		STDMETHOD(put_timefont) (THIS_
			LOGFONT lf
			) PURE;
		STDMETHOD(get_timefont) (THIS_
			LOGFONT *lf
			) PURE;		

		//设置播放总时长  单位：100 ns
		STDMETHOD(set_duration)(THIS_
			__int64 llTime
			)PURE;
		STDMETHOD(get_duration)(THIS_
			__int64 *llTime
			)PURE;
	};
#ifdef __cplusplus
}
#endif


#endif