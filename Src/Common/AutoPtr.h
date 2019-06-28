#pragma once

#ifndef SafeDeleteArray
#define SafeDeleteArray(p)		 if(p){delete []p; p = NULL;}
#endif
#ifndef SafeDelete
#define SafeDelete(pObject)				if(pObject) {delete pObject; pObject = NULL;}
#endif

namespace QPtr{

	template<class Ty>
	class AutoPtr
	{
	public:
		AutoPtr(Ty *_ptr):m_pTy(_ptr)
		{
		}
		~AutoPtr()
		{
			SafeDelete(m_pTy);
		}

		const Ty* operator=(const Ty* _ptr)
		{
			if (_ptr == m_pTy)
			{
				return m_pTy;
			}
			SafeDelete(m_pTy);
			m_pTy = (Ty*)_ptr;
			return m_pTy;
		}

		operator Ty* ()	const
		{
			return m_pTy;
		}

		Ty* operator->() const 
		{
			return m_pTy;
		}

	private:
		Ty *m_pTy;
	};

	template<class Ty>
	class AutoArrayPtr
	{
	public:
		AutoArrayPtr(Ty *_ptr):m_pTy(_ptr)
		{
		}
		~AutoArrayPtr()
		{
			SafeDeleteArray(m_pTy);
		}

		const Ty* operator=(Ty* _ptr)
		{
			if (_ptr == m_pTy)
			{
				return m_pTy;
			}
			SafeDeleteArray(m_pTy);
			m_pTy = _ptr;
			return m_pTy;
		}

		operator Ty* ()	const
		{
			return m_pTy;
		}

		Ty* operator->() const 
		{
			return m_pTy;
		}

	private:
		Ty *m_pTy;
	};
};