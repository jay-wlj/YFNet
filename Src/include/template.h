#pragma once

//////////////////////////////////////////////////////////////////////////
//�ļ���	CSingleInstance.h
//˵����	����ģ����
//autor��	chezhibiao
//����:		2016-7-1
//////////////////////////////////////////////////////////////////////////
template<class T>
interface CSingleInstance
{
	//	��Ҫ�����̰߳Ѵ��ڴ�������
	static  T & Instance()
	{
		if (NULL == s_pT)
		{
			s_pT = new T;
		}
		return *s_pT;
	}

	static BOOL IsInstanceInvilate()
	{
		return (s_pT!=NULL) ;
	}

	virtual void Release()
	{
		s_pT = NULL;
	}

protected:
	static T *s_pT;
};

template<class T>
T* CSingleInstance<T>::s_pT = NULL;