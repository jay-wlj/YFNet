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
		pDisp->AddRef();
		return this;
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
};


class CXMLNodeList
{
public:
	HRESULT hr;
	ZDispatch m_Node;

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

	BOOL SelectSingleNode_(const TCHAR* pQuery, CXMLElement& elem)
	{
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

	BOOL RemoveChild_(CXMLElement& elem)
	{
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
		BOOL isOK = FALSE;
		if(m_Node.GetPropertyByName_Disp(L"ownerDocument", &dd) >= 0 &&dd.p)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	CString GetTagName_()
	{
		BOOL isOK = FALSE;
		CComVariant vRet;
		CString tagName;
		if(m_Node.GetPropertyByName(L"tagName", &vRet) >= 0)
		{
			if(vRet.vt == VT_BSTR && vRet.bstrVal)
			{
				tagName = CString(vRet.bstrVal);
				if(tagName.GetLength() > 0)
				{
					isOK = TRUE;
				}
			}
		}
		return tagName;
	}

	BOOL GetText_(CString& sText)
	{
		BOOL isOK = FALSE;
		CComVariant v;
		if(m_Node.GetPropertyByName(L"text", &v) >= 0)
		{
			if(v.vt == VT_BSTR && v.bstrVal)
			{
				sText = CString(v.bstrVal);
				isOK = TRUE;
			}
		}
		return isOK;
	}

	BOOL SetText_(const TCHAR* pstr)
	{
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
		BOOL isOK = FALSE;
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &CComVariant(lValue, VT_I4));
		if(hr >=0)
		{
			isOK = TRUE;
		}
		return isOK;
	}
	BOOL SetAttribute_(const TCHAR* pName, double dValue)
	{
		BOOL isOK = FALSE;
		hr = m_Node.Invoke2(L"setAttribute", &CComVariant(pName), &CComVariant(dValue));
		if(hr >=0)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	CString GetAttribute_(const TCHAR* pName)
	{
		CString value;
		CComVariant v;
		hr = m_Node.Invoke1(L"getAttribute", &CComVariant(pName), &v);
		if(hr>=0 && v.vt == VT_BSTR && v.bstrVal)
		{
			value = v.bstrVal;
		}
		return value;
	}
	int		GetAttribute_int(const TCHAR* pName)
	{
		int nreturn = 0;
		CString value;
		CComVariant v;
		hr = m_Node.Invoke1(L"getAttribute", &CComVariant(pName), &v);
		if(hr>=0 && v.vt == VT_BSTR && v.bstrVal)
		{
			value = CString(v.bstrVal);
			value.TrimLeft();
			value.TrimRight();
			nreturn = _ttoi(value);
		}
		else
		{
			nreturn = -1;
		}
		return nreturn;
	}

	BOOL		GetAttribute_bool(const TCHAR* pName, BOOL bDefault)
	{
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

	CString GetXml_()
	{
		CString xml;
		CComVariant v;
		hr = m_Node.GetPropertyByName(L"xml", &v);
		if(hr >= 0 && v.vt == VT_BSTR && v.bstrVal)
		{
			xml = CString(v.bstrVal);
		}
		return xml;
	}

};

class CXMLDoc
{
public:
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
		if(SUCCEEDED(hr))
		{
			isOK = TRUE;
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
		CComVariant vRet;
		if(m_doc.Invoke1(L"load", &CComVariant(pPath), &vRet) >= 0 && vRet.boolVal == VARIANT_TRUE)
		{
			m_LoadSrcPath = pPath;
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL LoadXml_(const TCHAR* pXml)
	{
		BOOL isOK = FALSE;
		CComVariant vRet;		
		if(m_doc.Invoke1(L"loadXml", &CComVariant(pXml), &vRet) >= 0 && vRet.boolVal == VARIANT_TRUE)
		{
			isOK = TRUE;
		}
		return isOK;
	}

	BOOL LoadXmlShell_()
	{
		return LoadXml_(_T("<?xml version='1.0'?><root></root>"));
		//return LoadXml_("<?xml version='1.0'?>");
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
			HRESULT  hr = m_doc.Invoke1(L"Save", &CComVariant(path));
			if(hr == S_OK)
			{
				isOK = TRUE;
			}
		}
		return isOK;
	}
	CString GetXmlpath() {return m_LoadSrcPath;}
private:
	CString m_LoadSrcPath;
	ZDispatch m_doc;
	HRESULT hr;

	BOOL CheckXmlVersion_(GUID* pGid = NULL)
	{
		HKEY key;
		CString str1 = "CLSID\\{F5078F32-C551-11D3-89B9-0000F81FE221}";
		CString str2 = "CLSID\\{88D969C0-F192-11D4-A65F-0040963251E5}";
		CString str3 = "CLSID\\{88D969E5-F192-11D4-A65F-0040963251E5}";
		CString str4 = "CLSID\\{88D96A05-F192-11D4-A65F-0040963251E5}";
		if(::RegOpenKey(HKEY_CLASSES_ROOT, str1, &key) == ERROR_SUCCESS)
		{
			if(pGid)*pGid = CLSID_XmlDoc3;
			::RegCloseKey(key);
			return TRUE;
		}
		if(::RegOpenKey(HKEY_CLASSES_ROOT, str2, &key) == ERROR_SUCCESS)
		{
			if(pGid)*pGid = CLSID_XmlDoc4;
			::RegCloseKey(key);
			return TRUE;
		}
		if(::RegOpenKey(HKEY_CLASSES_ROOT, str3, &key) == ERROR_SUCCESS)
		{
			if(pGid)*pGid = CLSID_XmlDoc5;
			::RegCloseKey(key);
			return TRUE;
		}
		if(::RegOpenKey(HKEY_CLASSES_ROOT, str4, &key) == ERROR_SUCCESS)
		{
			if(pGid)*pGid = CLSID_XmlDoc6;
			::RegCloseKey(key);
			return TRUE;
		}
		return FALSE;				
	}
};

#endif