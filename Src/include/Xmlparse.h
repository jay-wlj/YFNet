#ifndef __XMLPARSE_H__
#define __XMLPARSE_H__
const GUID CLSID_XmlDoc3 = {0xF5078F32,0xC551,0x11D3,0x89,0xB9,0x0,0x00,0xF8,0x1F,0xE2,0x21};
const GUID CLSID_XmlDoc4 = {0x88D969C0,0xF192,0x11D4,0xA6,0x5F,0x0,0x40,0x96,0x32,0x51,0xE5};
const GUID CLSID_XmlDoc5 = {0x88D969E5,0xF192,0x11D4,0xA6,0x5F,0x0,0x40,0x96,0x32,0x51,0xE5};
const GUID CLSID_XmlDoc6 = {0x88D96A05,0xF192,0x11D4,0xA6,0x5F,0x0,0x40,0x96,0x32,0x51,0xE5};

//#include <map>
//class CXMLElement;
class ZDispatch : public CComDispatchDriver
{
public:

	ZDispatch()
	{
		p = NULL;
	}
	ZDispatch *operator =(IDispatch* pDisp)
	{
		if(p) p->Release();
		p = pDisp;
		if(pDisp)
		{
			pDisp->AddRef();
		}
		return this;
	}
	void Release()
	{
		if(p) p->Release();
		p = NULL;
	}

	HRESULT GetPropertyByName_Disp(LPCOLESTR lpsz, IDispatch** ppDisp)
	{
		CComVariant v1;
		HRESULT hr = GetPropertyByName(lpsz, &v1);
		if(SUCCEEDED(hr))
		{
			if(v1.vt == VT_DISPATCH && v1.pdispVal)
			{
				*ppDisp = v1.pdispVal;
				(*ppDisp)->AddRef();
			}
			else
			{
				hr = E_FAIL;
			}
		}
		return hr;
	}

	HRESULT Invoke1_Disp(LPCOLESTR lpszName, VARIANT* pvarParam1, IDispatch** ppDisp)
	{
		HRESULT hr;
		CComVariant v1;
		hr = Invoke1(lpszName, pvarParam1, &v1);
		if(SUCCEEDED(hr))
		{
			if(v1.vt == VT_DISPATCH && v1.pdispVal)
			{
				*ppDisp = v1.pdispVal;
				(*ppDisp)->AddRef();
			}
			else
			{
				hr = E_FAIL;
			}
		}
		return hr;
	}

	HRESULT Invoke2_Disp(LPCOLESTR lpszName, VARIANT* pvarParam1, VARIANT* pvarParam2, IDispatch** ppDisp)
	{
		HRESULT hr;
		CComVariant v1;
		hr = Invoke2(lpszName, pvarParam1, pvarParam2, &v1);
		if(SUCCEEDED(hr))
		{
			if(v1.vt == VT_DISPATCH && v1.pdispVal)
			{
				*ppDisp = v1.pdispVal;
				(*ppDisp)->AddRef();
			}
			else
			{
				hr = E_FAIL;
			}
		}
		return hr;
	}
};


class CXMLNodeList
{
public:
	HRESULT hr;
	ZDispatch m_Node;

	CXMLNodeList()
	{
		hr = 0;
	}

	long GetItemCount_()
	{
		long count = 0;
		CComVariant vLen;
		hr = m_Node.GetPropertyByName(L"length", &vLen);
		if(SUCCEEDED(hr) && vLen.vt == VT_I4)
		{
			count = vLen.lVal;
		}
		return count;
	}

	BOOL GetItemDisp_(long index, ZDispatch& dd)
	{
		BOOL isOK = FALSE;
		if(index>=0)
		{
			ZDispatch node;
			hr = m_Node.Invoke1_Disp(L"item", &CComVariant(index, VT_I4), &node);
			if(SUCCEEDED(hr) && node.p)
			{
				dd = node.p;
				isOK = TRUE;
			}
		}
		return isOK;
	}

	CString GetAttribute_(ZDispatch& node, const TCHAR* pName)
	{
		CString value;
		CComVariant v;
		hr = node.Invoke1(L"getAttribute", &CComVariant(pName), &v);
		if(hr>=0 && v.vt == VT_BSTR && v.bstrVal)
		{
			value = CString(v.bstrVal);
		}
		return value;
	}

	BOOL RemoveAll_()
	{
		BOOL isOK = FALSE;
		hr = m_Node.Invoke0(L"removeAll");
		if(hr >= 0)
		{
			isOK = TRUE;
		}
		return isOK;
	}
};

class CXMLElement
{
public:
	HRESULT hr;
	ZDispatch m_Node;

	CXMLElement()
	{
		hr = 0;
	}

	BOOL SelectSingleNode_(const TCHAR* pQuery, CXMLElement& elem)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		ZDispatch node;
		hr = m_Node.Invoke1_Disp(L"selectSingleNode", &CComVariant(pQuery), &node);
		if(hr >= 0 && node.p)
		{
			elem.m_Node = node.p;
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL SelectNodes_(const TCHAR* pQuery, CXMLNodeList& list)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		ZDispatch node;
		hr = m_Node.Invoke1_Disp(L"selectNodes", &CComVariant(pQuery), &node);
		if(hr>=0 && node.p)
		{
			list.m_Node = node.p;
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL GetChildNodes_(CXMLNodeList& list)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.GetPropertyByName_Disp(L"childNodes", &list.m_Node);
		if(hr>=0 && list.m_Node.p)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL AppendChild_(CXMLElement& elem)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		CComVariant vRet;
		hr = m_Node.Invoke1(L"appendChild", &CComVariant(elem.m_Node.p), &vRet);
		if(hr>=0 && vRet.vt == VT_DISPATCH && vRet.pdispVal)
		{
			elem.m_Node.p->Release();
			vRet.pdispVal->AddRef();
			elem.m_Node.p = vRet.pdispVal;
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL InsertBefore_(CXMLElement& elemNew, CXMLElement& elemRef)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		CComVariant vRet;
		hr = m_Node.Invoke2(L"insertBefore", &CComVariant(elemNew.m_Node.p), &CComVariant(elemRef.m_Node.p), &vRet);
		if(hr>=0 && vRet.vt == VT_DISPATCH && vRet.pdispVal)
		{
			elemNew.m_Node.p->Release();
			vRet.pdispVal->AddRef();
			elemNew.m_Node.p = vRet.pdispVal;
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL RemoveChild_(CXMLElement& elem)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.Invoke1(L"removeChild", &CComVariant(elem.m_Node.p));
		if(hr >= 0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL GetParentNode_(CXMLElement& elem)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.GetPropertyByName_Disp(L"parentNode", &elem.m_Node);
		if(hr == S_OK && elem.m_Node.p)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL GetDocumentDisp_(ZDispatch& dd)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		if(m_Node.GetPropertyByName_Disp(L"ownerDocument", &dd) >= 0 &&dd.p)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	CString GetTagName_()
	{
		if(m_Node == NULL) 
		{
			return _T("");
		}
		CString tagName = _T("");
		CComVariant vRet;
		if(m_Node.GetPropertyByName(L"tagName", &vRet) >= 0)
		{
			if(vRet.vt == VT_BSTR && vRet.bstrVal)
			{
				tagName = CString(vRet.bstrVal);
			}
		}
		return tagName;
	}

	BOOL GetText_(CString& sText)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		CComVariant v;
		if(m_Node.GetPropertyByName(L"text", &v) >= 0)
		{
			if(v.vt == VT_BSTR && v.bstrVal)
			{
// #ifndef UNICODE
// 				long len = wcslen(v.bstrVal);
// 				if(len>0)
// 				{
// 					char *buff = new char[len*2+1];
// 					memset(buff, 0, len*2+1);
// 					WideCharToMultiByte(CP_THREAD_ACP, 0, v.bstrVal, len, buff, len*2+1, 0, 0);
// 					sText = buff;
// 					delete []buff;
// 				}
// #else
// 				sText = CString(v.bstrVal);
// #endif
				USES_CONVERSION;
				sText = CString(W2T(v.bstrVal));
				isOK = TRUE;
			}
		}
		return isOK;
	}

	BOOL SetText_(const TCHAR* pstr)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		CComVariant v(pstr);
		if(m_Node.PutPropertyByName(L"text", &v) >= 0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL SetAttribute_(const TCHAR* pName, const TCHAR* pValue)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &CComVariant(pValue));
		if(hr >= 0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL SetAttribute_(const TCHAR* pName, ULONG uValue)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &CComVariant((UINT)uValue, VT_UI4));
		if(hr >= 0)
		{
		  isOK = TRUE;
		}
		return isOK;
	}

	BOOL SetAttribute_(const TCHAR* pName, long lValue)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &CComVariant(lValue, VT_I4));
		if(hr >=0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL SetAttribute_(const TCHAR* pName, int lValue)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &CComVariant(lValue, VT_I4));
		if(hr >=0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL SetAttribute_(const TCHAR* pName, __int64 lValue)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &CComVariant(lValue));
		if(hr >=0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL SetAttribute_(const TCHAR* pName, double dValue)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &CComVariant(dValue));
		if(hr >=0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL SetAttributeDWORD_(const TCHAR* pName, DWORD dwValue)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		CString value;
		value.Format(_T("0x%08x"), dwValue);
		CComVariant v(value);
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &v);
		if(hr>=0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL SetAttributeInt64_(const TCHAR* pName, __int64 llValue)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		CString value;
		value.Format(_T("%I64d"), llValue);
		CComVariant v(value);
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &v);
		if(hr>=0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL RemoveAttribute_(const TCHAR* pName)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.Invoke1(L"removeAttribute", &CComVariant(pName));
		if(hr >=0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	CString GetAttribute_(const TCHAR* pName)
	{
		if(m_Node == NULL) 
		{
			return _T("");
		}
		CString value;
		CComVariant v;
		hr = m_Node.Invoke1(L"getAttribute", &CComVariant(pName), &v);
		if(hr>=0 && v.vt == VT_BSTR && v.bstrVal)
		{
// #ifndef UNICODE
// 			long len = wcslen(v.bstrVal);
// 			if(len>0)
// 			{
// 				char *buff = new char[len*2+1];
// 				memset(buff, 0, len*2+1);
// 				WideCharToMultiByte(CP_THREAD_ACP, 0, v.bstrVal, len, buff, len*2+1, 0, 0);
// 				value = buff;
// 				delete []buff;
// 			}
// #else
// 			value = CString(v.bstrVal);
// #endif
			USES_CONVERSION;
			value = CString(W2T(v.bstrVal));
		}
		return value;
	}
	long GetAttributeInt_(const TCHAR* pName, long nDefault = 0)
	{
		if(m_Node == NULL) 
		{
			return nDefault;
		}
		CString value;
		CComVariant v;
		hr = m_Node.Invoke1(L"getAttribute", &CComVariant(pName), &v);
		if(hr>=0 && v.vt == VT_BSTR && v.bstrVal)
		{
			value = CString(v.bstrVal);
			return _ttol(value);
		}
		return nDefault;
	}
	__int64 GetAttributeInt64_(const TCHAR* pName, __int64 nDefault = 0)
	{
		if(m_Node == NULL) 
		{
			return nDefault;
		}
		CString value;
		CComVariant v;
		hr = m_Node.Invoke1(L"getAttribute", &CComVariant(pName), &v);
		if(hr>=0 && v.vt == VT_BSTR && v.bstrVal)
		{
			value = CString(v.bstrVal);
			return _ttoi64(value);
		}
		return nDefault;
	}

	DWORD GetAttributeDWORD_(const TCHAR* pName, DWORD nDefault = 0)
	{
		if(m_Node == NULL) 
		{
			return nDefault;
		}
		CString value;
		CComVariant v;
		hr = m_Node.Invoke1(L"getAttribute", &CComVariant(pName), &v);
		if(hr>=0 && v.vt == VT_BSTR && v.bstrVal)
		{
			value = CString(v.bstrVal);
			TCHAR *pStr;
			return _tcstoul(value, &pStr, 16);
		}
		return nDefault;
	}

	BOOL GetAttribute_bool(const TCHAR* pName, BOOL bDefault)
	{
		if(m_Node == NULL) 
		{
			return bDefault;
		}
		BOOL bIsOk = FALSE;
		int nv = 0;
		CString value;
		CComVariant v;
		hr = m_Node.Invoke1(L"getAttribute", &CComVariant(pName), &v);
		if(hr>=0 && v.vt == VT_BSTR && v.bstrVal)
		{
			value = CString(v.bstrVal);
			value.TrimLeft();
			value.TrimRight();
			nv = _ttol(value);
			if(nv > 0)
				bIsOk = TRUE;
		}
		else
		{
			bIsOk = bDefault;
		}
		return bIsOk;
	}

	float GetAttributeFloat_(const TCHAR* pName, float nDefault = 0.0f)
	{
		if(m_Node == NULL) 
		{
			return nDefault;
		}
		CString value;
		CComVariant v;
		hr = m_Node.Invoke1(L"getAttribute", &CComVariant(pName), &v);
		if(hr>=0 && v.vt == VT_BSTR && v.bstrVal)
		{
			value = CString(v.bstrVal);
#ifdef UNICODE
			return (float)_wtof(value);
#else
			return (float)atof(value);
#endif
		}
		return nDefault;
	}

	CString GetXml_()
	{
		if(m_Node == NULL) 
		{
			return _T("");
		}
		CString xml;
		CComVariant v;
		hr = m_Node.GetPropertyByName(L"xml", &v);
		if(hr >= 0 && v.vt == VT_BSTR && v.bstrVal)
		{
			xml = CString(v.bstrVal);
		}
		return xml;
	}

	BOOL ReplaceChild_(CXMLElement& newChild, CXMLElement& oldChild)
	{
		if(m_Node == NULL) 
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		hr = m_Node.Invoke2(L"replaceChild", &CComVariant(newChild.m_Node.p), &CComVariant(oldChild.m_Node.p));
		if(hr >= 0)
		{
			isOK = TRUE;
		}
		return isOK;
	}
};

class CXMLDoc
{
public:
	CXMLDoc()
	{
		hr = 0;
	}

	BOOL Init_()
	{
		BOOL isOK = FALSE;
		GUID gid;
		if(!CheckXmlVersion_(&gid))
		{
			gid = CLSID_XmlDoc3;
			//return FALSE;
		}
		hr = ::CoCreateInstance(gid, NULL, CLSCTX_INPROC, IID_IDispatch, (void**)&m_doc);
		if(hr == CO_E_NOTINITIALIZED) // 临时的做法
		{
			CoInitialize(0);
			hr = ::CoCreateInstance(gid, NULL, CLSCTX_INPROC, IID_IDispatch, (void**)&m_doc);
		}
		if(SUCCEEDED(hr))
		{
			isOK = TRUE;
		}
		else
		{
			//避免出现xml加载初始化错误
			gid = CLSID_XmlDoc3;
			hr = ::CoCreateInstance(gid, NULL, CLSCTX_INPROC, IID_IDispatch, (void**)&m_doc);
			if(hr >= 0) return TRUE;
			gid = CLSID_XmlDoc4;
			hr = ::CoCreateInstance(gid, NULL, CLSCTX_INPROC, IID_IDispatch, (void**)&m_doc);
			if(hr >= 0) return TRUE;
			gid = CLSID_XmlDoc5;
			hr = ::CoCreateInstance(gid, NULL, CLSCTX_INPROC, IID_IDispatch, (void**)&m_doc);
			if(hr >= 0) return TRUE;
			gid = CLSID_XmlDoc6;
			hr = ::CoCreateInstance(gid, NULL, CLSCTX_INPROC, IID_IDispatch, (void**)&m_doc);
			if(hr >= 0) return TRUE;
		}
		return isOK;
	}

	void Clear_()
	{
		SafeRelease(m_doc.p);
	}

	BOOL Load_(const TCHAR* pPath)
	{
		BOOL isOK = FALSE;
		if(GetFileAttributes(pPath) == -1) return isOK;
		CComVariant vRet;
		if(m_doc.Invoke1(L"load", &CComVariant(pPath), &vRet) >= 0 && vRet.boolVal == VARIANT_TRUE)
		{
			m_LoadSrcPath = pPath;
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL LoadXml_(const char* pXml)
	{
		if((*(DWORD*)pXml) == 0x3cbfbbef)
		{
			pXml += 3;
		}
		if(lstrlenA(pXml) == 0)
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		CComVariant vRet;
		if(m_doc.Invoke1(L"loadXml", &CComVariant(pXml), &vRet) >= 0 && vRet.boolVal == VARIANT_TRUE)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL LoadXml_(const WCHAR* pXml)
	{
		if(lstrlen(pXml) == 0)
		{
			return FALSE;
		}
		BOOL isOK = FALSE;
		CComVariant vRet;
		if(m_doc.Invoke1(L"loadXml", &CComVariant(pXml), &vRet) >= 0 && vRet.boolVal == VARIANT_TRUE)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL LoadXmlShell_(const TCHAR *pRootName)
	{
		CString strRootName = pRootName;
		if(strRootName.GetLength() == 0) strRootName = _T("root");
		CString str;
		str.Format(_T("<?xml version='1.0' encoding='utf-8'?><%s></%s>"), strRootName, strRootName);
		return LoadXml_(str);
	}

	BOOL CreateElement_(const TCHAR* pTag, CXMLElement& elem)
	{
		BOOL isOK = FALSE;

		ZDispatch newNode;
		hr = m_doc.Invoke1_Disp(L"createElement", &CComVariant(pTag), &newNode);
		if(hr>=0 && newNode.p)
		{
			elem.m_Node = newNode.p;
			isOK = TRUE;
		}

		return isOK;
	}

	BOOL GetDocumentElement_(CXMLElement& elem)
	{
		BOOL isOK = FALSE;

		ZDispatch root;
		hr = m_doc.GetPropertyByName_Disp(L"documentElement", &root);
		if(hr >= 0 && root.p)
		{
			elem.m_Node = root.p;
			isOK = TRUE;
		}

		return isOK;
	}

	BOOL SelectSingleNode_(const TCHAR* pQuery, CXMLElement& elem)
	{
		BOOL isOK = FALSE;
		ZDispatch node;
		hr = m_doc.Invoke1_Disp(L"selectSingleNode", &CComVariant(pQuery), &node);
		if(hr >= 0 && node.p)
		{
			elem.m_Node = node.p;
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL SelectNodes_(const TCHAR* pQuery, CXMLNodeList& list)
	{
		BOOL isOK = FALSE;
		ZDispatch node;
		hr = m_doc.Invoke1_Disp(L"selectNodes", &CComVariant(pQuery), &node);
		if(hr>=0 && node.p)
		{
			list.m_Node = node.p;
			isOK = TRUE;
		}
		return isOK;
	}

	CString GetXml_()
	{
		CString xml;
		CComVariant v;
		hr = m_doc.GetPropertyByName(L"xml", &v);
		if(hr >= 0 && v.vt == VT_BSTR && v.bstrVal)
		{
			xml = CString(v.bstrVal);
		}
		return xml;
	}

	BOOL Save_(const TCHAR* pPath = NULL)
	{
		BOOL isOK = FALSE;
		CString path;
		if(pPath)
		{
			path = pPath;
		}
		else if(m_LoadSrcPath.GetLength() > 0)
		{
			path = m_LoadSrcPath;
		}

		if(path.GetLength() > 0 && m_doc.p)
		{
#ifdef _DEBUG
			FormatXML();
#endif
			if(m_doc.Invoke1(L"Save", &CComVariant(path)) == S_OK)
			{
				isOK = TRUE;
			}
		}
		return isOK;
	}
	CString GetXmlpath() {return m_LoadSrcPath;}
//private:
	CString m_LoadSrcPath;
	ZDispatch m_doc;
	HRESULT hr;

	BOOL CheckXmlVersion_(GUID* pGid = NULL)
	{
		HKEY key;
		CString str1 = _T("CLSID\\{F5078F32-C551-11D3-89B9-0000F81FE221}");
		CString str2 = _T("CLSID\\{88D969C0-F192-11D4-A65F-0040963251E5}");
		CString str3 = _T("CLSID\\{88D969E5-F192-11D4-A65F-0040963251E5}");
		CString str4 = _T("CLSID\\{88D96A05-F192-11D4-A65F-0040963251E5}");
		if(::RegOpenKey(HKEY_CLASSES_ROOT, str4, &key) == ERROR_SUCCESS)
		{
			if(pGid)*pGid = CLSID_XmlDoc6;
			::RegCloseKey(key);
			return TRUE;
		}
		if(::RegOpenKey(HKEY_CLASSES_ROOT, str3, &key) == ERROR_SUCCESS)
		{
			if(pGid)*pGid = CLSID_XmlDoc5;
			::RegCloseKey(key);
			return TRUE;
		}
		if(::RegOpenKey(HKEY_CLASSES_ROOT, str2, &key) == ERROR_SUCCESS)
		{
			if(pGid)*pGid = CLSID_XmlDoc4;
			::RegCloseKey(key);
			return TRUE;
		}
		if(::RegOpenKey(HKEY_CLASSES_ROOT, str1, &key) == ERROR_SUCCESS)
		{
			if(pGid)*pGid = CLSID_XmlDoc3;
			::RegCloseKey(key);
			return TRUE;
		}
		return FALSE;
	}

	BOOL FormatXML(BOOL bFormat = TRUE) // TRUE:格式化排版, FALSE:紧凑排版
	{
		CXMLElement xmlRoot;
		GetDocumentElement_(xmlRoot);
		if(bFormat)
		{
			RegularIndentRecur(xmlRoot);
		}
		else
		{
			IrregularIndentRecur(xmlRoot);
		}
		return TRUE;
	}
private:
	BOOL CreateTextNode_(const TCHAR *pstr, CXMLElement &elem)
	{
		BOOL isOK = FALSE;

		ZDispatch node;
		hr = m_doc.Invoke1_Disp(L"createTextNode", &CComVariant(pstr), &node);
		if(hr>=0 && node.p)
		{
			elem.m_Node = node.p;
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL RegularIndentRecur(CXMLElement &parentElem, long tCount=0)
	{
		long nIndex;
		long nCount;
		long nFlag;
		BOOL b;
		CString szTagName;
		CString szText = _T("\r\n");
		for(int t=0; t<tCount; t++)
		{
			szText += _T("\t");
		}
		tCount++;

		CXMLNodeList nodeList;
		CXMLElement childElem;
		parentElem.GetChildNodes_(nodeList);
		nCount = nodeList.GetItemCount_();

		if(1 == nCount)
		{
			nodeList.GetItemDisp_(0, childElem.m_Node);  //只有文本节点的元素
			if(!childElem.GetTagName_().GetLength())
			{
				CString str;
				parentElem.GetText_(str);
				parentElem.SetText_(_T(""));
				parentElem.SetText_(str);
				return TRUE;
			}
		}

		for(nFlag=0; nFlag<nCount; nFlag++)
		{
			nIndex = nFlag*2;
			b = nodeList.GetItemDisp_(nIndex, childElem.m_Node);
			{
				CXMLNodeList childList;
				b = childElem.GetChildNodes_(childList);
				if(childList.GetItemCount_()>0)
				{
					RegularIndentRecur(childElem,tCount);
				}
			}

			CString szPreText;
			szPreText = szText + _T("\t");
			CXMLElement childTextElem;
			CreateTextNode_(szPreText, childTextElem);
			parentElem.InsertBefore_(childTextElem, childElem);
		}

		CXMLElement parentTextElem;
		CreateTextNode_(szText, parentTextElem);
		parentElem.AppendChild_(parentTextElem);
		return TRUE;
	}

	BOOL IrregularIndentRecur(CXMLElement &parentElem)
	{
		long nIndex;
		long nCount;
		long nFlag;
		BOOL b;

		CXMLNodeList nodeList;
		CXMLElement childElem;
		parentElem.GetChildNodes_(nodeList);
		nCount = nodeList.GetItemCount_();

		if(1 == nCount)
		{
			nodeList.GetItemDisp_(0, childElem.m_Node); //只有文本节点的元素
			if(!childElem.GetTagName_().GetLength())
			{
				CString str;
				parentElem.GetText_(str);
				parentElem.SetText_(_T(""));
				parentElem.SetText_(str);
				return TRUE;
			}
		}

		for(nFlag=0; nFlag<nCount; nFlag++)
		{
			nIndex = nFlag*2;
			b = nodeList.GetItemDisp_(nIndex, childElem.m_Node);
			{
				CXMLNodeList childList;
				b = childElem.GetChildNodes_(childList);
				if(childList.GetItemCount_()>0)
				{
					IrregularIndentRecur(childElem);
				}
			}
			CXMLElement childTextElem;
			CreateTextNode_(_T(""), childTextElem);
			parentElem.InsertBefore_(childTextElem, childElem);
		}

		CXMLElement parentTextElem;
		CreateTextNode_(_T(""), parentTextElem);
		parentElem.AppendChild_(parentTextElem);
		return TRUE;
	}
};

#endif