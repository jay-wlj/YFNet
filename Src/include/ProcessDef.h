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
	//Add by chenzhibiao 2013-8-13 begin
	Insert_msg_updatelist	    = 0x10,  // ���²����б�
	Insert_msg_addfile			= 0x20,  // ����ļ����ļ��е�ĳ���б���
	Insert_msg_popuptipswnd	    = 0x30,  // ����������ʾ��
	Insert_msg_showfoladwnd	    = 0x40,  // ��ʾ��������
	Insert_msg_notifytraytip	= 0x50,  // ֪ͨ������ʾ
	Insert_msg_addcloudfile		= 0x60,  // ������ļ�
	//Add by chenzhibiao 2013-8-13 end

	Insert_msg_check			= 0x200, // ��ⲥ����״̬
	Insert_msg_Login			= 0x201, // ��¼
	Insert_msg_ShowVipAD		= 0x202, // ��VIP�û������Ƽ�����ʾvipad����
	Insert_msg_isprivate		= 0x203, // �ж��ļ��Ƿ�����˽���б�
	Insert_msg_addtoprivate		= 0x204, // ����ļ���˽���б�
	Insert_msg_showpstronghint	= 0x205, // ��ʾ˽��ǿ��ʾ
	Insert_msg_capture			= 0x206, // �����ͼ
	Insert_msg_speed			= 0x207, // ��������
	Insert_msg_getspeedswitch	= 0x208, // ��ȡ���ٿ���
	Insert_msg_qmvdata			= 0x209, // ����qmv����
	Insert_msg_qmvurl_clicked	= 0x20a, // �����qmv��ǩ�����
	Insert_msg_isEnableFilterAd = 0x20b, // �Ƿ����ι��
	Insert_msg_playfile_sucess	= 0x20c, // ��������������ļ�
	Insert_msg_notify_exit		= 0x20d, // ֪ͨ���������Ҫ�˳���
	Insert_msg_notify_playtime  = 0x20e, // ֪ͨ�������ļ����ŵ�λ��
	Insert_msg_play				= 0x20f, // ����	
	Insert_msg_TextAD			= 0x210, // ������˫URL����
	Insert_msg_kuaibo_openurl	= 0x211,
	ThirdControl				= 0x300,
#ifdef RDSP360
	Insert_msg_playlock,
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
	I_CaptureType	type;		// ��ȡ����
	bool			bFull;		// ����Ƿ�ȫ��
	bool			bNet;
	bool			bBit;
	HWND			hInsert;
	wchar_t			wzSrcPath[260];
	wchar_t			wzPath[260];
};

struct InsertPlaytimeData
{
	char			szHash[41];	//��ǰ�����ļ�hash
	long			lplaytime;		//��ǰ����λ�ã�<-1���Ѳ����꣨��λms)
};

//////////////////////////////////////////////////////////////////////////
// Insert_msg_play ���
// Insert_msg_play ����COPYDATASTRUCT��dwData��ֵ 
enum 
{
	Insert_msg_play_caller_qvodweb = 1,
};

// �������ݽṹ
struct QvodWebMediaUrlInfo
{
	TCHAR szURL[MAX_PATH];				// ��������
	TCHAR szRef[MAX_PATH];				// ����ҳ
	HWND hCorrelationWnd_;				// qvodplay����qvodwebҳ������Ĵ��ھ��
};

//////////////////////////////////////////////////////////////////////////
// ���ŷ�����Ϣ����ض���
// ���� WM_USER_PLAYER_TO_INSERT(WM_USER + 0x121) ��Ϣ��WPARAM����Ϊ��Ϣ����
enum Player_msg
{
	Player_msg_hasnetaddtoprivate,	  // �������ļ���ӵ�˽���б�
	Player_msg_logoutprivate,		  // �˳�˽���б�
	Player_msg_speed,
	Player_msg_EnableFilterAd,		  // 
};