// 摘    要：共享内存		
// 当前版本：
// 作    者：匡晓林
// 完成日期：

#pragma once
#include<vector>
using std::vector;
#include <iostream>
using namespace std;

// 插件共享内存
#define INSERT_MAX		100
#define INSERT_MAPPING _T("994FDA94-16E1-42c2-9588-7AC381F77249")
#define INSERT_MUTEX   _T("624C4E05-E3A7-4f94-97BF-6D95D737A27D")

enum ERROR_OPEN_MAPPING
{
	E_OM_OK,
	E_OM_ALREADY_EXISTS,	// 已经存在
	E_OM_FAILED,			// 失败
};

struct InsertData
{
	HWND  hWnd;
	int   playStatus;
	char  szPlayHash[40];
	InsertData(void)
	{
		memset(this, 0, sizeof(InsertData));
	}
	const InsertData & operator =(const InsertData & val)
	{
		memcpy(this, &val, sizeof(InsertData));
		return *this;
	}
};

struct VTagClick
{
	TCHAR szFileName[_MAX_FNAME];
	int   tagIndex;
	VTagClick(void)
	{
		memset(this, 0, sizeof(VTagClick));
	}
	const VTagClick & operator =(const VTagClick & val)
	{
		memcpy(this, &val, sizeof(VTagClick));
		return *this;
	}
};

class InsertMapping
{
public:
	InsertMapping()
	{
		m_hHandleMapping = NULL;
		m_hWnd = NULL;
		m_hMutex = NULL;
		CreateMutex();
		OpenMapping();
	}

	~InsertMapping()
	{
		Remove();
		DestroyMutex();
		CloseMapping();
	}

	void Add(HWND hWnd)
	{
		if(WAIT_TIMEOUT != WaitMutex())
		{
			MappingData* pMappingData = CreateMappingData();
			if(pMappingData)
			{
				int nIndex = pMappingData->nTop;
				if( nIndex > -1 && nIndex < INSERT_MAX)
				{
					ZeroMemory(pMappingData->nodes + nIndex , sizeof(InsertData));
					pMappingData->nodes[nIndex].hWnd = hWnd;
					pMappingData->nTop ++;
					m_hWnd = hWnd;
				}
			}
			DestroyMappingData(pMappingData);
		}
		ReleaseMutex();
	}

	void AddTagClick(LPCTSTR szFileName, int tagIndex)
	{
		if ( szFileName && szFileName[0] && 0 < tagIndex && WAIT_TIMEOUT != WaitMutex() )
		{
			if ( MappingData* pMappingData = CreateMappingData() )
			{
				pMappingData->tagClick.tagIndex = tagIndex;
				lstrcpyn(pMappingData->tagClick.szFileName, szFileName, _MAX_FNAME);
				DestroyMappingData(pMappingData);
			}
		}
		ReleaseMutex();
	}

	VTagClick GetTagClick(void)
	{
		VTagClick ret;
		MappingData* pMappingData = 0;
		if ( WAIT_TIMEOUT != WaitMutex() && ( pMappingData = CreateMappingData() ) )
		{
			ret = pMappingData->tagClick;
			DestroyMappingData(pMappingData);
		}
		ReleaseMutex();
		return ret;
	}

	void ClearTagClick(void)
	{
		if ( WAIT_TIMEOUT != WaitMutex() )
		{
			if ( MappingData* pMappingData = CreateMappingData() )
			{
				ZeroMemory( &(pMappingData->tagClick), sizeof(VTagClick) );
				DestroyMappingData(pMappingData);
			}
		}
		ReleaseMutex();
	}

	void Remove()
	{
		if(WAIT_TIMEOUT != WaitMutex())
		{
			MappingData* pMappingData = CreateMappingData();
			if(pMappingData)
			{
				int nPos = pMappingData->GetPos(m_hWnd);
				if(nPos > -1 && nPos < INSERT_MAX)
				{
					int nLastIndex = pMappingData->nTop -1;
					if(nPos < nLastIndex)
					{
						pMappingData->nodes[nPos] = pMappingData->nodes[nLastIndex];
					}
					pMappingData->nTop--;
				}
				m_hWnd = NULL;
			}
			DestroyMappingData(pMappingData);
		}
		ReleaseMutex();
	}

	void SetPlayHash(std::string hash)
	{
		if(WAIT_TIMEOUT != WaitMutex())
		{
			MappingData* pMappingData = CreateMappingData();
			if(pMappingData)
			{
				int nPos = pMappingData->GetPos(m_hWnd);
				if( nPos > -1 && nPos < INSERT_MAX)
				{
					memcpy(pMappingData->nodes[nPos].szPlayHash ,hash.c_str() ,hash.size() > 40 ? 40 : hash.size());
				}
			}
			DestroyMappingData(pMappingData);
		}
		ReleaseMutex();
	}

	void SetPlayStatus(int playStatus)
	{
		if(WAIT_TIMEOUT != WaitMutex())
		{
			MappingData* pMappingData = CreateMappingData();
			if(pMappingData)
			{
				int nPos = pMappingData->GetPos(m_hWnd);
				if( nPos > -1 && nPos < INSERT_MAX)
				{
					pMappingData->nodes[nPos].playStatus = playStatus;
				}
			}
			DestroyMappingData(pMappingData);
		}
		ReleaseMutex();
	}

	void Get(vector<InsertData>&vData)
	{
		if(WAIT_TIMEOUT != WaitMutex())
		{
			MappingData* pMappingData = CreateMappingData();
			if(pMappingData)
			{
				for (int i = 0; i < pMappingData->nTop; i++)
				{
					vData.push_back(pMappingData->nodes[i]);
				}
			}
			DestroyMappingData(pMappingData);
		}
		ReleaseMutex();
	}

private:
	struct MappingData
	{
		int			nTop;					// 指向第一个未使用的node
		InsertData	nodes[INSERT_MAX];		// node数组
		VTagClick	tagClick;				// 播放器点击标签，传给插件
		int GetPos(HWND hWnd)
		{
			int nPos = -1;
			if(hWnd)
			{
				for(int i = 0; i < nTop ;i++)
				{
					if(nodes[i].hWnd == hWnd)
					{
						nPos = i;
						break;
					}
				}
			}
			return nPos;
		}
	};

	int OpenMapping()			// 多次调用不会重复打开
	{
		ERROR_OPEN_MAPPING error;
		if(NULL == m_hHandleMapping)
		{
			m_hHandleMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE ,NULL ,PAGE_READWRITE ,0 ,sizeof(MappingData) ,INSERT_MAPPING);
			if(NULL == m_hHandleMapping)
			{
				error = E_OM_FAILED;
			}
			else
			{
				error = (GetLastError() == ERROR_ALREADY_EXISTS) ? E_OM_ALREADY_EXISTS : E_OM_OK;
			}
		}
		else
		{
			error = E_OM_ALREADY_EXISTS;
		}
		return error;
	}

	void CloseMapping()
	{
		if(NULL !=m_hHandleMapping)
		{
			CloseHandle(m_hHandleMapping);
		}
	}

	MappingData *CreateMappingData()
	{
		BOOL bFirstCreate = FALSE;
		MappingData *pMappingData = NULL;
		if(NULL != m_hHandleMapping)
		{
			LPVOID p = MapViewOfFile (m_hHandleMapping ,FILE_MAP_ALL_ACCESS ,0 ,0 ,sizeof(MappingData));
			if(NULL != p)
			{
				pMappingData = (MappingData*)p;
			}
		}
		return pMappingData;
	}

	void DestroyMappingData(MappingData* p)
	{
		if(p)
		{
			UnmapViewOfFile(p);
		}
	}

	void CreateMutex()
	{
		m_hMutex = ::CreateMutex( NULL, FALSE, INSERT_MUTEX);
	}

	void DestroyMutex()
	{
		if(NULL != m_hMutex)
		{
			::CloseHandle(m_hMutex);
		}
	}

	int WaitMutex()
	{
		return ::WaitForSingleObject(m_hMutex, 5000);
	}

	void ReleaseMutex()
	{
		::ReleaseMutex(m_hMutex);
	}
private:
	HANDLE				m_hHandleMapping;
	HANDLE				m_hMutex;
	HWND				m_hWnd;
};