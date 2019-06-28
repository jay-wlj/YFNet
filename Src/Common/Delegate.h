#ifndef _DELEGATE_H_
#define _DELEGATE_H_

template<class _T_LISTENER, class _PARAM_TYPE, class _T_SENDER = _T_LISTENER>
class EventHandler//Delegate
{
public:
	typedef unsigned long (_T_LISTENER::*SelectorType)(_T_SENDER *sender, _PARAM_TYPE * pParam);

private:
	struct callbackdata
	{
		_T_LISTENER* pListener;
		SelectorType pSelector;
		callbackdata() : pListener(0), pSelector(0) {}
		callbackdata(_T_LISTENER* pL, SelectorType pS) : pListener(pL), pSelector(pS) {}
		bool operator=(callbackdata const& o)
		{
			return o.pListener == pListener && o.pSelector == pSelector;
		}
	};
	std::vector<callbackdata> setCallbacks;
	QLock::CriticalSection		m_lock;

public:
	unsigned long Invoke(_T_SENDER *sender, _PARAM_TYPE * pParam)
	{
		QLock::AutoCriticalSection lock(m_lock);

		unsigned long ret = 0;
		for (std::vector<callbackdata>::iterator it = setCallbacks.begin();
			it != setCallbacks.end();
			it++)
		{
			ret |= (it->pListener->*(it->pSelector))(sender, pParam);
		}
		return ret;
	}

	void Add(_T_LISTENER *pListener, SelectorType pSelector)
	{
		QLock::AutoCriticalSection lock(m_lock);

		setCallbacks.push_back(std::set<callbackdata>::value_type(pListener, pSelector));
	}

	void Remove(_T_LISTENER *pListener, SelectorType pSelector)
	{
		QLock::AutoCriticalSection lock(m_lock);

		for (std::vector<callbackdata>::iterator it = setCallbacks.begin();
			it != setCallbacks.end();
			it++)
		{
			if (pListener == it->pListener && pSelector == it->pSelector)
			{
				it = setCallbacks.erase(it);
				break;
			}
		}
	}
};
#endif