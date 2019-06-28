#include <fstream>

class CGifFile
{
public:
	BOOL Init(LPCTSTR szPath)
	{
		m_strPath = szPath;
		if(GetFileAttributes(szPath) == INVALID_FILE_ATTRIBUTES)
		{
			return FALSE;
		}
		return TRUE;
	}

	void Uninit()
	{
		if(m_src.is_open())
		{
			m_src.close();
		}
		if(m_dest.is_open())
		{
			m_dest.close();
		}
	}

	CString GetComment()
	{
		m_bSet = FALSE;
		m_comment.Empty();
		OverAll();
		return m_comment;
	}

	BOOL SetComment(LPCTSTR szComment)
	{
		m_bSet = TRUE;
		BOOL bRet = FALSE;
		m_comment = szComment;
		TCHAR szTempPath[MAX_PATH] = {0};
		GetTempPath(MAX_PATH, szTempPath);
		TCHAR szTempFile[MAX_PATH] = {0};
		GetTempFileName(szTempPath, _T("qgt"), 0, szTempFile);
		m_dest.open(szTempFile, std::ios::binary);

		if(m_dest.is_open())
		{
			bRet = OverAll();
			m_dest.close();
			if(bRet)
			{
				bRet = FALSE;
				// 交换
				if(CopyFile(szTempFile, m_strPath, FALSE))
				{
					DeleteFile(szTempFile);
					bRet = TRUE;
				}
			}
		}
		return bRet;
	}

private:
	BOOL m_bSet;
	std::ifstream m_src;
	std::ofstream m_dest;
	CString m_comment;
	CString m_strPath;

	void OverColorTable(char flag)
	{
		long color_table_size = 0;
		if(flag&0x80)
		{
			long color_table_index = flag&0x07;
			color_table_index++;
			color_table_size = 1<<color_table_index;
			color_table_size *= 3;
		}
		if(color_table_size)
		{
			if(m_bSet)
			{
				char *pBuff = new char[color_table_size];
				m_src.read(pBuff, color_table_size);
				m_dest.write(pBuff, color_table_size);
				delete []pBuff;
			}
			else
			{
				m_src.seekg(color_table_size, std::ios::cur);
			}
		}
	}

	long OverBlock(BOOL bComment)
	{
		byte len;
		m_src.read((char*)&len, 1);
		if(m_bSet&& (!bComment))
		{
			m_dest.write((char*)&len, 1);
		}
		if(len)
		{
			if(bComment)
			{
				if(m_bSet)
				{
					// 跳过原注释
					m_src.seekg(len, std::ios::cur);
				}
				else
				{
					char buff[256] = {0};
					m_src.read(buff, len);
					m_comment += buff;
				}
			}
			else
			{
				if(m_bSet)
				{
					char *pBuff = new char[len];
					m_src.read(pBuff, len);
					m_dest.write(pBuff, len);
					delete []pBuff;
				}
				else
				{
					m_src.seekg(len, std::ios::cur);
				}
			}
		}
		return len;
	}

	void OverLZW()
	{
		char bit;
		m_src.read(&bit, 1);
		if(m_bSet)
		{
			m_dest.write(&bit, 1);
		}
		while(OverBlock(FALSE))
		{
		}
	}

	void OverImage()
	{
		char szBuff[8];
		m_src.read(szBuff, 8);
		if(m_bSet)
		{
			m_dest.write(szBuff, 8);
		}
		char flag;
		m_src.read(&flag, 1);
		if(m_bSet)
		{
			m_dest.write(&flag, 1);
		}
		OverColorTable(flag);
		OverLZW();
	}

	void OverExtend()
	{
		byte tag2;
		m_src.read((char*)&tag2, 1);
		if(m_bSet)
		{
			m_dest.write((char*)&tag2, 1);
		}
		BOOL bCommentTag = FALSE;
		if(tag2 == 0xfe)
		{
			bCommentTag = TRUE;
			if(m_bSet)
			{
				// 后退
				m_dest.seekp(-2, std::ios::cur);
			}
		}
		while(OverBlock(bCommentTag))
		{
		}
	}

	void WriteBlock(char *pBuff, long nLen)
	{
		byte len = 0;
		char *pEnd = pBuff + nLen;
		while(pBuff != pEnd)
		{
			long nWrite = pEnd - pBuff;
			if(nWrite > 254)
			{
				nWrite = 254;
			}
			len = nWrite;
			m_dest.write((char*)&len, 1);
			m_dest.write(pBuff, nWrite);
			pBuff += nWrite;
		}
		len = 0;
		m_dest.write((char*)&len, 1);
	}

	BOOL OverAll()
	{
		char szBuff[8] = {0};
		m_src.open(m_strPath, std::ios::binary);
		if(m_src.is_open())
		{
			m_src.seekg(0);

			// gif头
			m_src.read(szBuff, 6);
			if(memcmp(szBuff, "GIF89a", 6) != 0)
			{
				m_src.close();
				return FALSE;
			}

			if(m_bSet)
			{
				m_dest.write(szBuff, 6);
			}

			// 逻辑屏幕块
			m_src.read(szBuff, 7);
			if(m_bSet)
			{
				m_dest.write(szBuff, 7);
			}

			// 颜色表
			BYTE flag = szBuff[4];
			OverColorTable(flag);

			char tag1;
			// 写注释
			if(m_bSet)
			{
				long len = m_comment.GetLength();
				if(len)
				{
					tag1 = '!';
					m_dest.write(&tag1, 1);
					byte tag2 = 0xfe;
					m_dest.write((char*)&tag2, 1);
					char *pBuff = new char[len*2+2];
					USES_CONVERSION;
					lstrcpyA(pBuff, T2A(m_comment));
					len = lstrlenA(pBuff);
					WriteBlock(pBuff, len);
					delete pBuff;
				}
			}

			// 标识集合:  图像标识=0x2c，扩展=0x21
			m_src.read(&tag1, 1);
			if(m_bSet)
			{
				m_dest.write(&tag1, 1);
			}
			while(!m_src.eof())
			{
				BOOL bEnd = FALSE;
				switch(tag1)
				{
				case ',':
					OverImage();
					break;
				case '!':
					OverExtend();
					break;
				case ';':
				default:
					bEnd = TRUE;
					break;
				}
				if(bEnd)
				{
					break;
				}
				m_src.read(&tag1, 1);
				if(m_bSet)
				{
					m_dest.write(&tag1, 1);
				}
			}
			m_src.close();
		}
		return TRUE;
	}
};