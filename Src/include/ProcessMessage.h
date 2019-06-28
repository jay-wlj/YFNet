// ժ    Ҫ��		
//			
//
// ��ǰ�汾��
// ��    �ߣ�������
// ������ڣ�
#pragma once


//////////////////////////////////////////////////////////////////////////
// ���������Ϣ����ض���
// ����WM_COPYDATA��Ϣ��WPARAM����Ϊ��Ϣ����

enum Insert_msg
{
	Insert_msg_check		   = 0x200, // ��ⲥ����״̬
	Insert_msg_Login           = 0x201, // ��¼
	Insert_msg_ShowVipAD	   = 0x202, // ��VIP�û������Ƽ�����ʾvipad����
	Insert_msg_isprivate	   = 0x203, // �ж��ļ��Ƿ�����˽���б�
	Insert_msg_addtoprivate    = 0x204, // ����ļ���˽���б�
	Insert_msg_showpstronghint = 0x205, // ��ʾ˽��ǿ��ʾ
	Insert_msg_capture         = 0x206, // �����ͼ
	Insert_msg_speed		   = 0x207, // ��������
	Insert_msg_getspeedswitch  = 0x208, // ��ȡ���ٿ���
	ThirdControl			   = 0x300,
#ifdef RDSP360
	Insert_msg_playlock		   ,
#endif
#ifdef RDSP
	Insert_msg_updatetostdver,
#endif
};

enum I_CaptureType		// �����ȡ����
{
	I_CaptureType_img,
};

struct InsertCaptureData
{
	I_CaptureType type;		 // ��ȡ����
	bool	 bFull;			 // ����Ƿ�ȫ��
	bool	 bNet;
	bool     bBit;
	HWND	 hInsert;
	wchar_t  wzSrcPath[260];
	wchar_t  wzPath[260];
};

//////////////////////////////////////////////////////////////////////////
// ���ŷ�����Ϣ����ض���
// ���� WM_USER_PLAYER_TO_INSERT(WM_USER + 0x121) ��Ϣ��WPARAM����Ϊ��Ϣ����
enum Player_msg
{
	Player_msg_hasnetaddtoprivate,	  // �������ļ���ӵ�˽���б�
	Player_msg_logoutprivate,		  // �˳�˽���б�
	Player_msg_speed,
};