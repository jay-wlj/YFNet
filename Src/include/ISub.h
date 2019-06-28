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
		//·����󳤶�: MAX_PATH * sizeof(WCHAR)
		//language   �������� ���� ���� ��
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
		//��ɫ�ĸ�ʽ�� A B G R  ȥ����λA������COLORREF
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
		STDMETHOD(put_delayTime) (THIS_  //��λ:ms
			int time
			) PURE;
		STDMETHOD(get_delayTime) (THIS_ 
			int *time
			) PURE;

			//	SUB_TIME_NOUSER = 0,	//��ʹ��
			//	SUB_TIME_CUR,			//��ǰʱ��
			//	SUB_TIME_END,			//ʣ��ʱ��
			//	SUB_TIME_SYSTEM			//ϵͳʱ��
		STDMETHOD(put_timeSub) (THIS_ 
			int type
			) PURE;
		STDMETHOD(get_timeSub) (THIS_  
			int *type
			) PURE;
		//������ʾʱ�������  �������СֵΪ10 ���ֵΪ72
		STDMETHOD(put_timefont) (THIS_
			LOGFONT lf
			) PURE;
		STDMETHOD(get_timefont) (THIS_
			LOGFONT *lf
			) PURE;		

		//���ò�����ʱ��  ��λ��100 ns
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