// ժ    Ҫ��		
//			
//
// ��ǰ�汾��
// ��    �ߣ���־��
// ������ڣ�
#pragma once


//////////////////////////////////////////////////////////////////////////
// ���������Ϣ����ض���
// ����WM_COPYDATA��Ϣ��WPARAM����Ϊ��Ϣ����

enum barrage_msg
{
	//��Ļ���۲����������Ϣ
	barrage_msg_setbarragewndsz  = 0x500, //������õ�Ļ���۴��ڴ�С
	barrage_msg_showloginwnd		= 0x501, //��ʾ��¼����
	barrage_msg_playfile			= 0x502, //�����ļ�
};

//��Ļ�������ݶ���
struct tagQvodInfo
{
	TCHAR szUserID[10]	;
	TCHAR szHash[41]	;
};
//�����ļ�
struct tagBarrageFile
{
	TCHAR		bstr_url[MAX_PATH];
	LONGLONG	film_pos	;
	TCHAR		bstr_content[MAX_PATH] ;
	BOOL		bIsThisFilm;
};