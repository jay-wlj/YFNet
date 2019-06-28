#pragma once

//////////////////////////////////////////////////////////////////////////
//文件：	CSingleInstance.h
//说明：	单例模板类
//autor：	chezhibiao
//日期:		2016-7-1
//////////////////////////////////////////////////////////////////////////
template<class T>
interface CSingleInstance
{
	//	需要在主线程把窗口创建起来
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