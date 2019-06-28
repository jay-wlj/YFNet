#pragma once
#include "qvod_basetypes.h"
#include "json.h"

#define STAT_TIMEOUT				5000

#pragma pack(push)
#pragma pack(1)


namespace net {
enum VARTYPE {
		VT_I4,
		VT_UI4,
		VT_I8,
		VT_UI8,
		VT_PSTR,
	};
struct Value
{
	VARTYPE			vType;	//数据类型
	union
	{
		QVOD_UINT64				ullVal; //VT_UI8
		QVOD_INT64					llVal;	//VT_I8
		QVOD_UINT					ulVal;	//VT_UI4
		QVOD_INT						lVal;		//VT_I4
		const char*					pVal;	//VT_PTSTR
	};

	Value()
	{
		memset(this, 0, sizeof(Value));
	}
	Value(QVOD_UINT64 val)
	{
		vType = VT_UI8;
		ullVal = val;
	}
	Value(QVOD_INT64 val)
	{
		vType = VT_I8;
		llVal = val;
	}
	Value(QVOD_UINT val)
	{
		vType = VT_UI4;
		ulVal = val;
	}
	Value(QVOD_INT val)
	{
		vType = VT_I4;
		lVal = val;
	}
	Value(const char* val)
	{
		vType = VT_PSTR;
		pVal = val;
	}

	Value(const Value& val)
	{
		memcpy(this, &val, sizeof(val));
	}

	Value& operator=(const Value& val)
	{
		memcpy(this, &val, sizeof(val));
		return *this;
	}
	Value& operator=(QVOD_UINT64 val)
	{
		vType = VT_UI8;
		ullVal = val;
		return *this;
	}
	Value& operator=(QVOD_INT64 val)
	{
		vType = VT_I8;
		llVal = val;
		return *this;
	}

	Value& operator=(QVOD_UINT val)
	{
		vType = VT_UI4;
		ulVal = val;
		return *this;
	}

	Value& operator=(QVOD_INT val)
	{
		vType = VT_I4;
		lVal = val;
		return *this;
	}
	Value& operator=(char* val)
	{
		vType = VT_PSTR;
		pVal = val;
		return *this;
	}
	Value& operator=(QVOD_LONG val)
	{
		vType = VT_I4;
		lVal = val;
		return *this;
	}
	Value& operator=(QVOD_ULONG val)
	{
		vType = VT_UI4;
		lVal = val;
		return *this;
	}
	std::string GetString()
	{
		std::string str;
		char szBuf[50] = {0};
		switch(vType)
		{
		case VT_I4:
			sprintf(szBuf, "%d", lVal);
			str = szBuf;
			break;
		case VT_UI4:
			sprintf(szBuf, "%u", ulVal);
			str = szBuf;
			break;
		case VT_UI8:
			sprintf(szBuf, "%llu", ullVal);
			str = szBuf;
			break;
		case VT_I8:
			sprintf(szBuf, "%lld", llVal);
			str = szBuf;
			break;
		case VT_PSTR:
			str = pVal;
			break;
		default:
			sprintf(szBuf, "%d", lVal);
			str = szBuf;
			break;
		}
		return str;
	}
};
}


//字段信息
struct STAT_FIELD
{
	const char*  strKey;				//字段ID
	net::Value    val;
	//std::string  strVal;					//值
};

class CStatMgr
{
public:
	CStatMgr(void);
	virtual ~CStatMgr(void);
	static CStatMgr& Instance();

	virtual bool Run() = 0;
	virtual bool Stop() = 0;
	virtual bool AddStat(STAT_FIELD *stats, bool bAsy=true, QVOD_DWORD nTimeout=STAT_TIMEOUT) = 0;	// 添加一条统计
	virtual bool AddStat(Json::Value &data) = 0;							// 添加一条统计
	virtual void SetCommonStat(Json::Value &data) = 0;			// 设置公共统计字段
};

