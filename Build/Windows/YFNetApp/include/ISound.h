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
		//put_channel������������
		//cchannel:0 �������1 ֻ����������������2 ֻ����������������3 Ĭ�ϣ���������4 ��������չ��ȫ��  5 ��������չ��ȫ��
		STDMETHOD(put_channel) (THIS_ char cchannel) PURE; //
		STDMETHOD(get_channel) (THIS_ char *cchannel)PURE;
		//����
		STDMETHOD(put_mute)	   (THIS_ bool bmute) PURE; //
		STDMETHOD(get_mute)	   (THIS_ bool *bmute)PURE; //
		//�����ӳ١�����λ��100 ns
		STDMETHOD(put_soundshift)(THIS_ LONGLONG lshift)PURE;
		STDMETHOD(get_soundshift)(THIS_ LONGLONG *lshift)PURE;
		//�����Ŵ� ����ֵΪ10��ladd / 10 Ϊ�Ŵ�ı�������С��10���������С
		STDMETHOD(put_addsound)	 (THIS_ LONG ladd)PURE;
		STDMETHOD(get_addsound)	 (THIS_ LONG *ladd)PURE;
		//�����������⡡ ȡֵ��Χ��0 -- 200	
		STDMETHOD(put_eq)		 (THIS_ int	 *neq)PURE;
		STDMETHOD(get_eq)		 (THIS_ int  *neq)PURE;
		//�� �ر���������
		STDMETHOD(put_seq)		 (THIS_ bool bseq)PURE;
		STDMETHOD(get_seq)		 (THIS_ bool *bseq)PURE;
		//����ƽ��  ͨ������IID_IBasicAudioʵ��  ȡֵ��Χ��-100 100
		STDMETHOD(put_balance)	 (THIS_ LONG lbal)PURE;
		STDMETHOD(get_balance)	 (THIS_ LONG *lbal)PURE;
		
		//���¹���δʵ�֣��ѻķ�
		STDMETHOD(put_astream)	 (THIS_ DWORD nastream)PURE;
		STDMETHOD(get_astream)	 (THIS_ DWORD *ncount, DWORD *ncur)PURE;
		STDMETHOD(put_wav)		 (THIS_ LONG  bsavetype, char *psavepath)PURE;
		STDMETHOD(get_wav)		 (THIS_ LONG *bsaveteyp, char *psavepath)PURE;
		STDMETHOD(init)			 ()PURE;
		//���ã���ȡ����ǰ������
		STDMETHOD(PutChannelNum)	(float channel)PURE;
		STDMETHOD(GetChannelNum)	(float* pchannel)PURE;
    };
	extern ISoundtrans *g_ISoundtrans; 

#ifdef __cplusplus
}
#endif

#endif // __ISOUNDTRANS__