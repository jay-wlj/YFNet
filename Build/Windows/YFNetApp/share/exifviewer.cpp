#include "stdafx.h"
#include "exifviewer.h"

#define  numoftags 48
TAGTRANSLATE g_strExifTag[] = {
	{0x010f,_T("Exif.Image.Make"),E_ASC},						//存储文件大小
	{0x0110,_T("Exif.Image.Model"),E_ASC},						//存储hash
	{0x013b,_T("Exif.Image.Artist"),E_ASC},						//存储影片名字
	{0x9003,_T("Exif.Photo.DateTimeOriginal"),E_ASC},			//存储截图时间
	{0x9291,_T("Exif.Photo.SubSecTimeOriginal"),E_ASC},			//存储截图在电影中的时间（按秒计算）
	{0x010e,_T("Exif.Image.ImageDescription"),E_ASC},			//存储URL（浏览引用页）
	{0x8298,_T("Exif.Image.Copyright"),E_ASC},					//存储种子地址
	{0x9c9b,_T("Exif.Image.XPTitle"),E_BYTE},
	{0x9c9c,_T("Exif.Image.XPComment"),E_BYTE},
	{0x9c9d,_T("Exif.Image.XPAuthor"),E_BYTE},
	{0x9c9e,_T("Exif.Image.XPSubject"),E_BYTE},
	{0x9c9f,_T("Exif.Image.XPKeywords"),E_BYTE},
	{0x000b,_T("Exif.Image.ProcessingSoftware"),E_ASC},
	{0x0100,_T("Exif.Image.ImageWidth"),E_LONG},
	{0x0101,_T("Exif.Image.ImageLength"),E_LONG},
	{0x0102,_T("Exif.Image.BitsPerSample"),E_SHORT},
	{0x0103,_T("Exif.Image.Compression"),E_SHORT},
	{0x0106,_T("Exif.Image.PhotometricInterpretation"),E_SHORT},
	{0x0107,_T("Exif.Image.Threshholding"),E_SHORT},
	{0x0108,_T("Exif.Image.CellWidth"),E_SHORT},
	{0x0109,_T("Exif.Image.CellLength"),E_SHORT},
	{0x010a,_T("Exif.Image.FillOrder"),E_SHORT},
	{0x010d,_T("Exif.Image.DocumentName"),E_ASC},
	{0x010e,_T("Exif.Image.ImageDescription"),E_ASC},
	{0x0111,_T("Exif.Image.StripOffsets"),E_LONG},
	{0x0115,_T("Exif.Image.SamplesPerPixel"),E_SHORT},
	{0x0116,_T("Exif.Image.RowsPerStrip"),E_LONG},
	{0x0117,_T("Exif.Image.StripByteCounts"),E_LONG},
	{0x011a,_T("Exif.Image.XResolution"),E_RATIONAL},
	{0x011b,_T("Exif.Image.YResolution"),E_RATIONAL},
	{0x0124,_T("Exif.Image.T4Options"),E_LONG},
	{0x0125,_T("Exif.Image.T6Options"),E_LONG},
	{0x0128,_T("Exif.Image.ResolutionUnit"),E_SHORT},
	{0x012d,_T("Exif.Image.TransferFunction"),E_SHORT},
	{0x0131,_T("Exif.Image.Software"),E_ASC},
	{0x0132,_T("Exif.Image.DateTime"),E_ASC},
	{0x0142,_T("Exif.Image.TileWidth"),E_SHORT},
	{0x0143,_T("Exif.Image.TileLength"),E_SHORT},
	{0x0144,_T("Exif.Image.TileOffsets"),E_SHORT},
	{0x0145,_T("Exif.Image.TileByteCounts"),E_SHORT},
	{0x014a,_T("Exif.Image.SubIFDs"),E_LONG},
	{0x0158,_T("Exif.Image.XClipPathUnits"),E_SSHORT},
	{0x0159,_T("Exif.Image.YClipPathUnits"),E_SSHORT},
	{0x800d,_T("Exif.Image.ImageID"),E_ASC},
	{0x828f,_T("Exif.Image.BatteryLevel"),E_RATIONAL},
	{0x8769,_T("Exif.Image.ExifTag"),E_LONG},
	{0xc61e,_T("Exif.Image.DefaultScale"),E_RATIONAL},
	{0x927c,_T("Exif.Photo.MakerNote"),E_UNDEFINED},
	{0x9286,_T("Exif.Photo.UserComment"),E_BYTE}  //原为Comment
};

int g_types[exiftotaltypes] = {	1,1,2,4,8,
1,1,2,4,8,
4,8 };

CJpgExif::CJpgExif()
{
	m_fileStartPos = 0;
	//SetThreadLocale(LOCALE_SYSTEM_DEFAULT);
}

CJpgExif::~CJpgExif()
{
}

//BOOL GetExifInfo(LPTSTR szjpgpath,LPTSTR  pqueryedinfo,LPTSTR pexifinfo )
// parms:
//			LPTSTR szjpgpath :			 *.jpg文件的绝对路径
//			LPTSTR  pqueryedinfo :		需要查询的信息
//			LPTSTR pexifinfo :			查询到的结果
int CJpgExif::GetExifInfo(LPCTSTR szJpgPath, LPCTSTR  pQueryedType, LPSTR pExifInfo, int &maxDesSize)
{
	int bQueryed = unknow_error;
	if( NULL == szJpgPath || NULL == pQueryedType || NULL == pExifInfo )
	{
		return invalid_file_path;
	}
	if( _tcslen(szJpgPath) <= 0 || _tcslen(pQueryedType) <= 0 )
	{
		return invalid_file_path;
	}

	if( 0 != _tcscmp(m_initJpgPath.c_str(), szJpgPath) )
	{
		m_initJpgPath = _T("");
		bQueryed = InitJpegMap(szJpgPath) ;
		if( sucess == bQueryed )
		{
			m_initJpgPath = szJpgPath;
		}
		else
		{
			return bQueryed;
		}
	}

	bQueryed = GetExifInfoByType(pQueryedType, pExifInfo, maxDesSize);
	return bQueryed;
}

int CJpgExif::WriteExifInfo(LPCTSTR szJpgPath, LPCTSTR pQueryedType, LPSTR pExifInfo, int nLength, BOOL bSave, LPTSTR lpSavePath )
{
	int bResult = unknow_error;
	if( NULL == szJpgPath || NULL == pQueryedType && !bSave || NULL == pQueryedType && NULL != pExifInfo )
	{
		return bResult;
	}
	if( 0 != _tcscmp(m_initJpgPath.c_str(), szJpgPath) )
	{
		m_initJpgPath = _T("");
		bResult = InitJpegMap(szJpgPath) ;
		if( sucess == bResult )
		{
			m_initJpgPath = szJpgPath;
		}
		else
		{
			return bResult;
		}
	}
	bResult = ModifyIFD(pQueryedType, pExifInfo, nLength);
	//TODO : 修改g_exifinfo
	if( bSave )
	{
		//ffEX 不存在情况
		return SaveToFile(szJpgPath, lpSavePath);
	}
	//CExifIFDEntry addtnery;
	//int i,itag = 0;
	//for(i = 0;i < numoftags + 1;i ++)
	//{
	//	if( 0 == _tcscmp(g_strexiftag[i].strtag.c_str(),pqueryedtype) )
	//	{
	//		itag = g_strexiftag[i].itag;
	//		addtnery.tag[0] = g_strexiftag[i].itag / 256;
	//		addtnery.tag[1] = g_strexiftag[i].itag % 256;
	//		addtnery.fieldtype = g_strexiftag[i].tagtype;
	//		if( E_BYTE == addtnery.fieldtype|| E_ASC == addtnery.fieldtype )
	//		{
	//			addtnery.numofval = length;
	//		}
	//		else
	//		{
	//			addtnery.numofval = g_types[addtnery.fieldtype - 1];
	//		}
	//		break;
	//	}
	//}
	//if ( numoftags + 1 == i)
	//	return invalidtype;
	////TODO: tag已经存在 || tag不存在
	////计算addtnery offset量
	//int offsetfromvalue = 0;
	//CExifIFDEntryMap::iterator ifdentryit;
	//if( !g_ifds.ifdentrymap.empty() )
	//{
	//	ifdentryit = g_ifds.ifdentrymap.end();
	//	ifdentryit --;
	//	g_leftpos = g_exifhead.headpos + g_offsetfex - 9;
	//	g_posatferifd = ifdentryit->second.offset + g_exifhead.headpos;
	//}
	//else
	//{
	//	g_leftpos = -1;
	//	return unkownerror;
	//}
	////TODO: add1不存在

	//g_oldoffsetfex = g_offsetfex;
	//ifdentryit = g_ifds.ifdentrymap.find(itag);
	//if( ifdentryit != g_ifds.ifdentrymap.end() ) //tag存在
	//{
	//	g_addtypeflag[0] = 1;
	//	g_addtypeflag[1] = ifdentryit->second.offset;
	//	g_addtypeflag[2] = ifdentryit->second.numofval;
	//	g_addtypeflag[3] = ifdentryit->second.fieldtype;
	//	offsetfromvalue = addtnery.numofval - ifdentryit->second.numofval*g_types[ifdentryit->second.fieldtype - 1];
	//	//ifdentryit->second.offset = ifdentryit->second.offset + offsetfromvalue;
	//	ifdentryit->second.numofval = addtnery.numofval;
	//	ifdentryit ++;
	//	while( ifdentryit != g_ifds.ifdentrymap.end() )
	//	{
	//		ifdentryit->second.offset = ifdentryit->second.offset + offsetfromvalue;
	//		ifdentryit ++;
	//	}
	//	g_offsetfex = g_offsetfex + offsetfromvalue;
	//}
	//else  //tag不存在
	//{
	//	ifdentryit = g_ifds.ifdentrymap.end();
	//	ifdentryit --;
	//	g_addtypeflag[1] = ifdentryit->second.offset;
	//	g_addtypeflag[2] = ifdentryit->second.numofval;
	//	g_addtypeflag[3] = ifdentryit->second.fieldtype;
	//	addtnery.offset = ifdentryit->second.offset + ifdentryit->second.numofval*g_types[ifdentryit->second.fieldtype - 1];
	//	g_ifds.ifdentrymap[itag] = addtnery;
	//	ifdentryit = g_ifds.ifdentrymap.begin();
	//	BOOL bfindnewtag = FALSE;
	//	while( ifdentryit != g_ifds.ifdentrymap.end() )
	//	{
	//		if( ifdentryit->first == itag )
	//		{
	//			bfindnewtag = TRUE;
	//			ifdentryit ++;
	//			continue;
	//		}
	//		if( !bfindnewtag )
	//		{
	//			ifdentryit->second.offset += 12;
	//		}
	//		else
	//		{
	//			ifdentryit->second.offset = ifdentryit->second.offset + 12 + ifdentryit->second.numofval*g_types[ifdentryit->second.fieldtype - 1];
	//		}
	//		ifdentryit ++;
	//	}
	//	g_offsetfex = g_offsetfex + 12 + addtnery.numofval;  //ifd entry size(12) + value size
	//	g_ifds.numofentry ++;
	//}
	//WriteToFile(szjpgpath,pexifinfo,itag,length,bsave);
	return bResult;
}

int CJpgExif::InitJpegMap(LPCTSTR szJpgPath)
{
	int bQueryed = unknow_error;
	CExifMap::iterator infoMap_it = m_mapExifInfo.begin();
	while( infoMap_it != m_mapExifInfo.end() )
	{
		delete infoMap_it->second->pInfo;
		infoMap_it->second->pInfo = NULL;
		infoMap_it++;
	}
	m_mapExifInfo.clear();
	m_IFDs.reset();
	if( m_mapTag.empty() )
	{
		for( int i = 0;i < numoftags + 1;i++)
		{
			EXIFTAGNODE tagNode;
			tagNode.strTag = g_strExifTag[i].strTag;
			tagNode.cbType = g_types[g_strExifTag[i].tagType - 1];
			m_mapTag[g_strExifTag[i].iTag] = tagNode;
		}
	}
	if( PathFileExists(szJpgPath) )
	{
		//TODO:打开文件
		USES_CONVERSION;
		//TCHAR szerror[10];
		//szerror[0] = 0;
		//_itot(_acp,szerror,10);
		//OutputDebugString(szerror);
		//OutputDebugStringA(T2A(szJpgPath));
		//OutputDebugString(szJpgPath);
		FILE * ostreamFile = NULL;
		errno_t _error = _wfopen_s(&ostreamFile, CT2W(szJpgPath), T2W(_T("rb")));
		//errno_t _error = fopen_s(&ostreamFile, T2A(szJpgPath), "rb");
		//TCHAR szerror[10];
		//szerror[0] = 0;
		//_itot(_error,szerror,10);
		//OutputDebugString(szerror);
		//OutputDebugString(_T("\n\n"));
		if( 0 != _error )
		{
			return open_file_failed;
		}
		//struct _stat st;
		//if( 0 != _tstat(szjpgpath,&st))
		//{
		//	if( ostreamfile )
		//		fclose(ostreamfile);
		//	return openfilefailed;
		//}
		//TODO:
		//初始化m_exifinfo
		UCHAR currentC = fgetc(ostreamFile);
		E_EXIFSTATE exifState = unknow_state;
		while( !feof( ostreamFile )  )
		{
			switch( currentC )
			{
			case 0xFF:
				exifState = exif_flag;
				break;
			case 0x4D:
			case 0x49:
				if( exif_len == exifState )
				{
					exifState = endian;
				}
				break;
			default:
				break;
			}
			bQueryed = InitJpegMapByState(ostreamFile, exifState);
			if( bQueryed < 0 )
			{
				break;
			}
			if( exifoffset == exifState )
			{
				break;
			}
			currentC = fgetc(ostreamFile);
		}
		//TODO：关闭文件
		fclose(ostreamFile);
	}
	return bQueryed;
}

int CJpgExif::InitJpegMapByState(FILE * &ostreamFile, E_EXIFSTATE &exifState)
{
	E_EXIFERROR bResult = unknow_error;
	if( exif_flag == exifState )
	{
		BOOL bReadSuc = TRUE;
		bResult = sucess;
		UCHAR currentC = fgetc(ostreamFile);
		size_t len = 0;
		if( 0xE0 <= currentC && currentC < 0xEF )
		{
			unsigned char szLen [2] = {0};
			len = fread_s(szLen, 2, 1, 2, ostreamFile);
			if( 0xE1 != currentC && 2 == len)
			{
				if( 0 != fseek(ostreamFile,szLen[0]*256 + szLen[1] - 2,SEEK_CUR) )  //减去szLen长度
				{
					bResult = unknow_error;
				}
				exifState = unknow_state;
				return bResult;
			}
			m_fex.filePos = ftell(ostreamFile) - 1;
			exifState = exif_len;
			//g_offsetfex = szlen[0]*256 + szlen[1];
			m_fex.offset_FEX = szLen[0]*256 + szLen[1];
			m_fex.offset_DES = m_fex.offset_FEX;
		}
		else
		{
			if( 0xD8 == currentC )
			{
				m_fileStartPos = ftell(ostreamFile);
			}
			if( 0xDB == currentC )
			{
				exifState = exifoffset;
				return sucess;
			}
			bReadSuc = FALSE;
		}
		char szExif_flag[7] = {0};
		if( 2 == len && bReadSuc )
		{
			len = fread_s(szExif_flag, 7, 1, 6, ostreamFile);
		}
		else if( 2 != len && 0 != len )
		{
			bReadSuc = FALSE;
			bResult = seek_failed;
		}
		else
		{
			bReadSuc = FALSE;
		}
		if( 6 == len && bReadSuc )
		{
			if( 0 != _stricmp(szExif_flag, "Exif") )
			{
				bResult = unknow_error;
				bReadSuc = FALSE;
			}
			else
			{
				exifState = exif_len;
			}
		}
		else
		{
			bReadSuc = FALSE;
		}
	}
	else if( endian == exifState )
	{
		bResult = sucess;
		m_exifHead.headPos = ftell(ostreamFile);
		UCHAR currentC = fgetc(ostreamFile);
		if( 0x4D == currentC )	//MM
		{
			m_exifHead.exifEndian = big_endian;
		}
		else if( 0x49 == currentC )  //II
		{
			m_exifHead.exifEndian = little_endian;
		}
		else
		{
			bResult = unknow_error;
		}
		char usetiff[2] = {0};
		char szLen [2] = {0};
		size_t len = fread_s(szLen, 2, 1, 2, ostreamFile);
		int endianIndex = 0;
		if( big_endian == m_exifHead.exifEndian )
		{
			endianIndex = 1;
		}
		if( 2 == len && 0x2A != szLen[endianIndex] )
		{
			bResult = unknow_error;
		}
		else
		{
			m_exifHead.bTiff = TRUE;
		}
		if( bResult >= 0 )
		{
			char offset[5] = {0};
			len = fread_s(offset, 5, 1, 4, ostreamFile);
			if( little_endian == m_exifHead.exifEndian )
			{
				int temp = 0;
				for(int i = 0;i < 2;i++)
				{
					temp = offset[i];
					offset[i] = offset[3-i];
					offset[3-i] = temp;
				}
			}
			if( 4 == len )
			{
				//for(int i = 0;i < 4;i++)
				//{
				//	offset[i] += 48;
				//}
				int sum = 0;
				for(int i = 0;i < 4 ;i ++)
				{
					sum *= 256;
					sum += (int)(UCHAR)offset[i];
				}
				m_exifHead.offset = sum;
				bResult = InitIFD(ostreamFile);
			}
			else
			{
				bResult = unknow_error;
			}
			exifState = exifoffset;
		}
	}
	return bResult;
}

int CJpgExif::GetExifInfoByType(LPCTSTR pQueryedType, LPSTR pExifInfo, int &maxDesSize)
{
	int bQueryed = invalid_type;
	CExifMap::iterator it = m_mapExifInfo.find(pQueryedType);
	if( it != m_mapExifInfo.end() )
	{
		if( -1 == maxDesSize )
		{
			maxDesSize = (int)it->second->num;
			return sucess;
		}
		int tmpmax = maxDesSize;
		maxDesSize = maxDesSize > (int)it->second->num?(int)it->second->num:maxDesSize;
		if( tmpmax != -1 )
		{
			memcpy_s(pExifInfo, maxDesSize, (const void*)it->second->pInfo, maxDesSize);
		}
		bQueryed = sucess;
	}
	return bQueryed;
}

E_EXIFERROR CJpgExif::InitIFD(FILE * &ostreamFile)
{
	E_EXIFERROR bResult = unknow_error;
	if( -1 != m_exifHead.headPos )
	{
		if( 0 == fseek(ostreamFile, m_exifHead.headPos, SEEK_SET))
		{
			int offset = m_exifHead.offset;
			if( offset < 0 ||  0 != fseek(ostreamFile, m_exifHead.offset - 1, SEEK_CUR ))
			{
				bResult = unknow_error;
			}
			else
			{
				char len[3] = {0};
				size_t readLen = fread_s(len, 3, 1, 2, ostreamFile);
				if( 2 != readLen )
				{
					return seek_failed;
				}
				/*for( int i = 0;i < 2;i++)
				{
					len[i] += 48;
				}*/
				if( little_endian == m_exifHead.exifEndian )
				{
					int temp = len[0];
					len[0] = len[1];
					len[1] = temp;
				}
				m_IFDs.filePos = ftell(ostreamFile);
				m_IFDs.numOfEntry = len[0] * 256 + len[1];
				char ifd[15] = {0};
				CExifIFD * pCurrentIFDs = &m_IFDs;
				do
				{
					for(int i = 0;i < pCurrentIFDs->numOfEntry; i++)
					{
						readLen = fread_s(ifd, 12, 1, 12, ostreamFile);
						if( 12 == readLen )
						{
							ParseIFDEnry(pCurrentIFDs, i, ifd, ostreamFile);
						}
					}
					CExifIFDEntryMap::iterator it_IDFEntry = pCurrentIFDs->mapIFDEntry.begin();
					BOOL bexiftagexist = FALSE;
					while( it_IDFEntry != pCurrentIFDs->mapIFDEntry.end() )
					{
						if( 0x87 == it_IDFEntry->second.tag[0] && 0x69 == it_IDFEntry->second.tag[1] )
						{
							bexiftagexist = TRUE;
							break;
						}
						it_IDFEntry++;
					}
					if( bexiftagexist )
					{
						long filePos = m_exifHead.headPos + it_IDFEntry->second.offset - 1;
						if( 0 != fseek(ostreamFile, filePos, SEEK_SET) )
						{
							break;
						}
						size_t readLen = fread_s(len, 3, 1, 2, ostreamFile);
						pCurrentIFDs->pNextIFD = new CExifIFD;
						pCurrentIFDs = pCurrentIFDs->pNextIFD;
						pCurrentIFDs->filePos = ftell(ostreamFile);
						if( little_endian == m_exifHead.exifEndian )
						{
							int temp = len[0];
							len[0] = len[1];
							len[1] = temp;
						}
						pCurrentIFDs->numOfEntry = len[0]*256 + len[1];
					}
					else
					{
						break;
					}
					/*CExifIFDEntryMap::iterator ifdentyit = pcurrentifs->ifdentrymap.end();
					ifdentyit--;
					long filepos = g_exifhead.headpos + ifdentyit->second.offset + ifdentyit->second.numofval*g_types[ ifdentyit->second.fieldtype - 1];
					if( 0 != fseek(ostreamfile,filepos,SEEK_SET) )
						break;
					if( 2 != fread_s(ifd,2,1,2,ostreamfile) )
						break;
					if( ifd[0] == 0xFF && (ifd[1] >= 0xE0 && ifd[1] <= 0xEF || ifd[1] == 0xDB ) )
						break;
					pcurrentifs->pnextifd = new CExifIFD;
					pcurrentifs = pcurrentifs->pnextifd;
					pcurrentifs->filepos = ftell(ostreamfile);
					pcurrentifs->numofentry = ifd[0]*256 + ifd[1];*/
				} while (1);

				ParseExifInfo(ostreamFile);
				bResult = sucess;
			}
		}
	}
	return bResult;
}

int CJpgExif::ParseIFDEnry(EXIFIFD * pCurrentIFDs, int nIFDIndex, char szIFD[], FILE *&ostreamFile)
{
	CExifIFDEntry it_IFDEntry;
	char tempvalue[5] = {0};
	for(int i = 2;i < 4;i ++)
	{
		szIFD[i] += 48;
	}
	if( little_endian == m_exifHead.exifEndian )
	{
		int temp = szIFD[0];
		szIFD[0] = szIFD[1];
		szIFD[1] = temp;
		temp = szIFD[2];
		szIFD[2] = szIFD[3];
		szIFD[3] = temp;
		for(int j = 1;j < 3;j++)
		{
			int rightpos = 4*(1 + j);
			for(int i = 4*j;i < 4*j + 2;i++)
			{
				temp = szIFD[i];
				szIFD[i] = szIFD[rightpos-i+4*j-1];
				szIFD[rightpos-i+4*j -1] = temp;
			}
		}
	}
	memcpy_s(it_IFDEntry.tag, 2, szIFD, 2);
	memcpy_s(tempvalue, 2, szIFD + 2, 2);
	it_IFDEntry.nFieldType = atoi(tempvalue);
	memcpy_s(tempvalue, 4, szIFD + 4, 4);
	int sum = 0;
	for(int i = 0;i < 4 ;i ++)
	{
		sum *= 256;
		sum += (int)(UCHAR)tempvalue[i];
	}
	it_IFDEntry.numOfVal = sum;
	memcpy_s(tempvalue, 4, szIFD + 8, 4);
	sum = 0;
	for(int i = 0;i < 4 ;i ++)
	{
		sum *= 256;
		sum += (int)(UCHAR)tempvalue[i];
	}
	it_IFDEntry.offset = sum;
	it_IFDEntry.alterOffset = sum;
	int ifdmapkey = it_IFDEntry.tag[0] *256 + it_IFDEntry.tag[1];
	it_IFDEntry.filePos = ftell(ostreamFile);
	pCurrentIFDs->mapIFDEntry[ifdmapkey] = it_IFDEntry;
	//g_ifds.ifdentrymap[ifdmapkey] = ifdentry;
	return sucess;
}

void CJpgExif::ParseExifInfo(FILE * &ostreamFile)
{
//	int idfsize = m_IFDs.numOfEntry;
	size_t readLen = 0;
	CExifIFD * pCurrentIFD = &m_IFDs;
	CExifIFDEntryMap::iterator it_IFDEntry;
	while ( pCurrentIFD )
	{
		it_IFDEntry = pCurrentIFD->mapIFDEntry.begin();
		while( it_IFDEntry != pCurrentIFD->mapIFDEntry.end() )
			//for(int i = 0;i < idfsize;i ++ )
		{
			if( 0 == fseek(ostreamFile, m_exifHead.headPos, SEEK_SET))
			{
				int offset = it_IFDEntry->second.offset/*g_ifds.mapIFDEntry[i].offset*/;
				if( offset < 0 || 0 != fseek(ostreamFile, it_IFDEntry->second.offset/*g_ifds.mapIFDEntry[i].offset*/ - 1, SEEK_CUR ))
				{
					it_IFDEntry++;
					continue;
				}
				else
				{
					char info[EXIF_MAX_PATH] = {0};
					int typeofval = it_IFDEntry->second.nFieldType;
					if( it_IFDEntry->second.numOfVal > EXIF_MAX_PATH || it_IFDEntry->second.numOfVal*g_types[typeofval - 1] > EXIF_MAX_PATH || it_IFDEntry->second.numOfVal < 0)
					{
						it_IFDEntry++;
						continue;
					}
					if( typeofval >= exiftotaltypes || typeofval <= 0 )
					{
						it_IFDEntry++;
						continue;
					}
					readLen = fread_s(info, EXIF_MAX_PATH, 1, it_IFDEntry->second.numOfVal*g_types[typeofval - 1], ostreamFile);
					if( it_IFDEntry->second.numOfVal*g_types[typeofval - 1] == readLen)
					{
						if( it_IFDEntry->second.lpValue )
						{
							delete it_IFDEntry->second.lpValue;
							it_IFDEntry->second.lpValue = NULL;
						}
						it_IFDEntry->second.lpValue = new char[readLen];
						memcpy_s(it_IFDEntry->second.lpValue, readLen, info, readLen);
						int sum = 0;
						for(int j = 0;j < 2;j ++)
						{
							sum *= 256;
							sum += it_IFDEntry->second.tag[j];
						}
						LPEXIFINFONODE exinfo = new EXIFINFONODE;
						exinfo->pInfo = new char[readLen + 1];
						exinfo->pInfo[readLen] = 0;
						memcpy_s(exinfo->pInfo, readLen + 1, info, readLen);
						exinfo->num = readLen;
						CExifTagMap::iterator it = m_mapTag.find(sum);
						if( it == m_mapTag.end() )
						{
							USES_CONVERSION;
							m_mapExifInfo[A2T((char*)it_IFDEntry->second.tag)] = exinfo;
						}
						else
						{
							m_mapExifInfo[it->second.strTag] = exinfo;
						}
					}
				}
			}
			it_IFDEntry++;
		}
		pCurrentIFD = pCurrentIFD->pNextIFD;
	}
}

//E_EXIFERROR WriteToFile(LPTSTR szjpgpath,LPSTR pexifinfo,int itag,int length,BOOL bsave)
//{
//	int ifdindex = 0;
//	CExifIFDEntryMap::iterator ifdmapit = g_ifds.ifdentrymap.begin();
//	while( ifdmapit != g_ifds.ifdentrymap.end() )
//	{
//		if( ifdmapit->first == itag )
//		{
//			break;
//		}
//		ifdmapit++;
//		ifdindex ++;
//	}
//	if( ifdmapit == g_ifds.ifdentrymap.end() )
//		return unkownerror;
//	if( g_tempfilename == _T("") )
//	{
//		TCHAR temppath[MAX_PATH] = {0};
//		DWORD pathlen = GetTempPath(MAX_PATH,temppath);
//		if(pathlen <= 0 || pathlen > MAX_PATH )
//			return unkownerror;
//		if( _T('\\') != temppath[pathlen - 1] )
//		{
//			temppath[pathlen] = _T('\\');
//			temppath[pathlen + 1] = 0;
//		}
//		srand((unsigned int)time(NULL));
//		int randname = rand()%100000 + 1000;
//		TCHAR tempname[MAX_PATH] = {0};
//		SYSTEMTIME stm;
//		GetLocalTime(&stm);
//		_stprintf_s(tempname,MAX_PATH,_T("%s%d%d%d%dqpg.dat"),temppath,stm.wYear,stm.wMonth,stm.wDay,randname);
//		g_tempfilename = tempname;
//		int randnamebk = rand()%100000 + 1000;
//		while( randnamebk == randname )
//			randnamebk = rand()%100000 + 1000;
//		_stprintf_s(tempname,MAX_PATH,_T("%s%d%d%d%dqpg.dat"),temppath,stm.wYear,stm.wMonth,stm.wDay,randnamebk);
//		g_tempfilenamebk = tempname;
//		USES_CONVERSION;
//		errno_t _error = fopen_s(&g_ptempostreamfile,T2A(szjpgpath),"rb");
//		if( _error != 0)
//			return openfilefailed;
//		_error = fopen_s(&g_ptempistreamfile,T2A(g_tempfilenamebk.c_str()),"w+b");
//		if( _error != 0 )
//			return openfilefailed;
//		CExifIFDEntryMap::iterator ifdentryit = g_ifds.ifdentrymap.end();
//		ifdentryit --;
//		long buffersize = g_exifhead.headpos + g_oldoffsetfex - 9;
//		char * pbuffer = new char [buffersize];
//		size_t readlen = fread_s(pbuffer,buffersize,1, buffersize,g_ptempostreamfile);
//		fwrite(pbuffer,1,readlen,g_ptempistreamfile);
//		delete pbuffer;
//		//TODO: don't close
//		fclose(g_ptempostreamfile);
//		_error = fopen_s(&g_ptempostreamfile,T2A(g_tempfilename.c_str()),"w+b");
//		if( _error != 0 )
//		{
//			ResetWrite();
//			return openfilefailed;
//		}
//	}
//	//else
//	{
//		if( NULL != g_ptempistreamfile && NULL != g_ptempostreamfile )
//		{
//			fseek(g_ptempistreamfile,0,SEEK_SET);
//			long buffersize = g_exifhead.headpos + g_exifhead.offset;
//			char * pbuffer = new char [buffersize];
//			size_t readlen = fread_s(pbuffer,buffersize,1,buffersize ,g_ptempistreamfile);
//			fwrite(pbuffer,1,readlen,g_ptempostreamfile);
//			delete pbuffer;
//			if( 0 == fseek(g_ptempostreamfile,g_exifhead.headpos - 9 ,SEEK_SET))
//			{
//				UCHAR fexoffset[2] = { g_offsetfex/256,g_offsetfex%256 };
//				if( little_endian == g_exifhead.exifendian )
//				{
//					UCHAR temp = fexoffset[0];
//					fexoffset[0] = fexoffset[1];
//					fexoffset[1] = temp;
//				}
//				fwrite(fexoffset,1,2,g_ptempostreamfile);
//			}
//			else
//			{
//				ResetWrite();
//				return seekfailed;
//			}
//			if( 0 == fseek(g_ptempostreamfile,-1,SEEK_END ) )
//			{
//				char numofifd [2] = { g_ifds.numofentry/256,g_ifds.numofentry%256 };
//				if( little_endian == g_exifhead.exifendian )
//				{
//					UCHAR temp = numofifd[0];
//					numofifd[0] = numofifd[1];
//					numofifd[1] = temp;
//				}
//				fwrite(numofifd,1,2,g_ptempostreamfile);
//			}
//			else
//			{
//				ResetWrite();
//				return seekfailed;
//			}
//			pbuffer = NULL;
//			buffersize = ifdindex;
//			if( 0 == fseek(g_ptempistreamfile,g_exifhead.headpos + g_exifhead.offset + 1,SEEK_SET) && 0 == fseek(g_ptempostreamfile,g_exifhead.headpos + g_exifhead.offset + 1,SEEK_SET) )
//			{
//				int buffersize = ifdindex*12;
//				if( buffersize > 0 )
//				{
//					pbuffer = new char[buffersize];
//					fread_s(pbuffer,buffersize,1,buffersize,g_ptempistreamfile);
//					fwrite(pbuffer,1,buffersize,g_ptempostreamfile);
//				}
//			}
//			else
//			{
//				ResetWrite();
//				return seekfailed;
//			}
//			pbuffer[0] = ifdmapit->second.tag[0];
//			pbuffer[1] = ifdmapit->second.tag[1];
//			char exiftype[2];
//			{
//				exiftype[0] = ifdmapit->second.fieldtype / 256;
//				exiftype[1] = ifdmapit->second.fieldtype % 256;
//			}
//			//_itoa_s(/*g_ifds.ifdentrymap[ifdindex]*/ifdmapit->second.fieldtype,exiftype,2,16);
//			if( little_endian == g_exifhead.exifendian)
//			{
//				UCHAR temp = exiftype[0];
//				exiftype[0] = exiftype[1];
//				exiftype[1] = temp;
//			}
//			memcpy_s(pbuffer + 2,2,exiftype,2);
//			char numofval[4];
//			{
//				numofval[0] = ifdmapit->second.numofval / 16777216;
//				numofval[1] = (ifdmapit->second.numofval % 16777216) / 65536;
//				numofval[2] = ((ifdmapit->second.numofval % 16777216) % 65536)/256;
//				numofval[3] = ifdmapit->second.numofval % 256;
//			}
//			//_itoa_s(/*g_ifds.ifdentrymap[ifdindex]*/ifdmapit->second.numofval,numofval,4,16);
//			if( little_endian == g_exifhead.exifendian)
//			{
//				for( int i = 0;i < 2 ;i++)
//				{
//					UCHAR temp = numofval[i];
//					numofval[i] = numofval[3 - i];
//					numofval[3 - i] = temp;
//				}
//			}
//			memcpy_s(pbuffer + 4,4,numofval,4);
//			{
//				numofval[0] = ifdmapit->second.offset / 16777216;
//				numofval[1] = (ifdmapit->second.offset % 16777216) / 65536;
//				numofval[2] = ((ifdmapit->second.offset % 16777216) % 65536)/256;
//				numofval[3] = ifdmapit->second.offset % 256;
//			}
//			//_itoa_s(/*g_ifds.ifdentrymap[ifdindex]*/ifdmapit->second.offset,numofval,4,16);
//			if( little_endian == g_exifhead.exifendian)
//			{
//				for( int i = 0;i < 2 ;i++)
//				{
//					UCHAR temp = numofval[i];
//					numofval[i] = numofval[3 - i];
//					numofval[3 - i] = temp;
//				}
//			}
//			memcpy_s(pbuffer + 8,4,numofval,4);
//			fwrite(pbuffer,1,12,g_ptempostreamfile);
//
//			//write value
//			if( g_addtypeflag[0] )
//			{
//				fseek(g_ptempistreamfile,g_exifhead.headpos + g_exifhead.offset + 1 + (ifdindex + 1)*12,SEEK_SET);
//				long len = (g_addtypeflag[1] + g_exifhead.headpos) - (g_exifhead.headpos + g_exifhead.offset + 2 + ifdindex*12 + 12);
//				if( len > 0)
//				{
//					char* pbuffer = new char[len];
//					fread_s(pbuffer,len,1,len,g_ptempistreamfile);
//					fwrite(pbuffer,1,len,g_ptempostreamfile);
//					delete pbuffer;
//				}
//				fwrite(pexifinfo,1,length,g_ptempostreamfile);
//				fseek(g_ptempistreamfile,g_addtypeflag[2]*g_types[g_addtypeflag[3] - 1],SEEK_CUR);
//			}
//			else
//			{
//				fseek(g_ptempistreamfile,g_exifhead.headpos + g_exifhead.offset + 1 + ifdindex*12,SEEK_SET);
//				long len = (g_addtypeflag[1] + g_exifhead.headpos + g_addtypeflag[2]*g_types[g_addtypeflag[3] - 1] - 1) - (g_exifhead.headpos + g_exifhead.offset + 2 + ifdindex*12);
//				if( len > 0)
//				{
//					char* pbuffer = new char[len];
//					fread_s(pbuffer,len,1,len,g_ptempistreamfile);
//					fwrite(pbuffer,1,len,g_ptempostreamfile);
//					delete pbuffer;
//				}
//				fwrite(pexifinfo,1,length,g_ptempostreamfile);
//			}
//
//			long currentpos = ftell(g_ptempistreamfile);
//			fseek(g_ptempistreamfile,0,SEEK_END);
//			long filesizeofi = ftell(g_ptempistreamfile);
//			fseek(g_ptempistreamfile,currentpos,SEEK_SET);
//			if( pbuffer )
//				delete pbuffer;
//			pbuffer = new char[filesizeofi - currentpos];
//			fread_s(pbuffer,filesizeofi - currentpos,1,filesizeofi - currentpos,g_ptempistreamfile);
//			fwrite(pbuffer,1,filesizeofi - currentpos,g_ptempostreamfile);
//			CExifIFDEntryMap::iterator ifdentryit = g_ifds.ifdentrymap.begin();
//			int currentindexifd = 0;
//			while( ifdentryit != g_ifds.ifdentrymap.end() )
//			{
//				if( 0 == fseek(g_ptempostreamfile,g_exifhead.headpos + g_exifhead.offset + 2 + currentindexifd*12,SEEK_SET) )
//				{
//					{
//						numofval[0] = ifdentryit->second.offset / 16777216;
//						numofval[1] = (ifdentryit->second.offset % 16777216) / 65536;
//						numofval[2] = ((ifdentryit->second.offset % 16777216) % 65536)/256;
//						numofval[3] = ifdentryit->second.offset % 256;
//					}
//					if( little_endian == g_exifhead.exifendian)
//					{
//						for( int i = 0;i < 2 ;i++)
//						{
//							UCHAR temp = numofval[i];
//							numofval[i] = numofval[3 - i];
//							numofval[3 - i] = temp;
//						}
//					}
//					if( 0 == fseek(g_ptempostreamfile,g_exifhead.headpos + g_exifhead.offset + 2 + currentindexifd*12 + 7,SEEK_SET) )
//					{
//						fwrite(numofval,1,4,g_ptempostreamfile);
//					}
//					else
//					{
//						ResetWrite();
//						return unkownerror;
//					}
//				}
//				ifdentryit++;
//				currentindexifd++;
//			}
//			//fseek(g_ptempostreamfile,0,SEEK_END);
//			filesizeofi = ftell(g_ptempostreamfile);
//			delete pbuffer;
//			pbuffer = new char[filesizeofi];
//			fseek(g_ptempostreamfile,0,SEEK_SET);
//			fread_s(pbuffer,filesizeofi,1,filesizeofi,g_ptempostreamfile);
//			fseek(g_ptempistreamfile,0,SEEK_SET);
//			fwrite(pbuffer,1,filesizeofi,g_ptempistreamfile);
//			delete pbuffer;
//		}
//	}
//	if( bsave )
//	{
//		fclose(g_ptempistreamfile);
//		g_ptempistreamfile = NULL;
//		USES_CONVERSION;
//		errno_t _error = fopen_s(&g_ptempistreamfile,T2A(szjpgpath),"rb");
//		if( _error != 0 )
//		{
//			ResetWrite();
//			return unkownerror;
//		}
//		else
//		{
//			fseek(g_ptempistreamfile,g_leftpos,SEEK_END);
//			long leftlen = ftell(g_ptempistreamfile) - g_leftpos;
//			char* pbuffer = new char[leftlen];
//			fseek(g_ptempistreamfile,g_leftpos,SEEK_SET);
//			fread_s(pbuffer,leftlen,1,leftlen,g_ptempistreamfile);
//			CExifIFDEntryMap::iterator ifdentryit = g_ifds.ifdentrymap.end();
//			ifdentryit --;
//			fseek(g_ptempostreamfile,g_exifhead.headpos + g_offsetfex - 9,SEEK_SET);
//			fwrite(pbuffer,1,leftlen,g_ptempostreamfile);
//			fclose(g_ptempostreamfile);
//			g_ptempostreamfile = NULL;
//			_tstring newfilename = _T("C:\\Documents and Settings\\Administrator\\桌面\\复件 watermark34511.JPG");
//			DeleteFile(newfilename.c_str());
//			MoveFile(g_tempfilename.c_str(),newfilename.c_str());
//			ResetWrite();
//		}
//	}
//	return sucess;
//}

void CJpgExif::ResetWrite()
{
	//if( g_ptempistreamfile )
	//	fclose(g_ptempistreamfile);
	//if( g_ptempostreamfile )
	//	fclose(g_ptempostreamfile);
	//g_ptempistreamfile = NULL;
	//g_ptempostreamfile = NULL;
	//DeleteFile(g_tempfilename.c_str());
	//DeleteFile(g_tempfilenamebk.c_str());
	m_initJpgPath = _T("");
	//g_tempfilename = _T("");
	//g_tempfilenamebk = _T("");
	//g_leftpos = -1;
	//g_offsetfex = 0;
	//g_oldoffsetfex = 0;
	//g_addtypeflag[0]=g_addtypeflag[1]=g_addtypeflag[2] = 0;
	//g_posatferifd = 0;
}

E_EXIFERROR CJpgExif::ModifyIFD(LPCTSTR pQueryedType, LPSTR pExifInfo, int nLength)
{
	E_EXIFERROR bResult = unknow_error;
	CExifIFDEntry addtnery;
	int i, iTag = 0;
	for(i = 0;i < numoftags + 1;i ++)
	{
		if( 0 == _tcscmp(g_strExifTag[i].strTag.c_str(), pQueryedType) )
		{
			iTag = g_strExifTag[i].iTag;
			addtnery.tag[0] = g_strExifTag[i].iTag / 256;
			addtnery.tag[1] = g_strExifTag[i].iTag % 256;
			addtnery.nFieldType = g_strExifTag[i].tagType;
			if( E_BYTE == addtnery.nFieldType|| E_ASC == addtnery.nFieldType )
			{
				addtnery.numOfVal = nLength;
			}
			else
			{
				addtnery.numOfVal = nLength/g_types[addtnery.nFieldType - 1];
			}
			break;
		}
	}
	if ( numoftags + 1 == i)
	{
		return invalid_type;
	}

	//计算addtnery offset量
	CExifIFD * pCurrentIFDs = &m_IFDs;
	CExifIFDEntryMap::iterator it_IDFEntry = pCurrentIFDs->mapIFDEntry.find(iTag);
	BOOL bTagExist = FALSE;
	if( it_IDFEntry != pCurrentIFDs->mapIFDEntry.end() )
		bTagExist = TRUE;
	while( it_IDFEntry == pCurrentIFDs->mapIFDEntry.end() && pCurrentIFDs->pNextIFD )
	{
		pCurrentIFDs = pCurrentIFDs->pNextIFD;
		it_IDFEntry = pCurrentIFDs->mapIFDEntry.find(iTag);
		if( it_IDFEntry != pCurrentIFDs->mapIFDEntry.end() )
		{
			bTagExist = TRUE;
			break;
		}
	}

	if( bTagExist ) //tag存在
	{
		CExifTagMap::iterator it = m_mapTag.find(iTag);
		CExifMap::iterator mapInfoit;
		char sztag[2] = {iTag/256, iTag%256};
		if( it == m_mapTag.end() )
		{
			USES_CONVERSION;
			mapInfoit = m_mapExifInfo.find(A2T(sztag));
		}
		else
		{
			mapInfoit = m_mapExifInfo.find(it->second.strTag);
		}
		if( mapInfoit != m_mapExifInfo.end() )
		{
			mapInfoit->second->num = nLength;
			if( mapInfoit->second->pInfo )
			{
				delete mapInfoit->second->pInfo;
				mapInfoit->second->pInfo = NULL;
			}
			mapInfoit->second->pInfo = new char[nLength];
			memcpy_s(mapInfoit->second->pInfo, nLength, pExifInfo, nLength);
		}
		long offsetfromvalue = addtnery.numOfVal - it_IDFEntry->second.numOfVal;
		it_IDFEntry->second.numOfVal = addtnery.numOfVal;
		if( addtype != it_IDFEntry->second.operateType )
		{
			it_IDFEntry->second.operateType = altertype;
		}
		if( it_IDFEntry->second.lpValue )
		{
			delete it_IDFEntry->second.lpValue;
			it_IDFEntry->second.lpValue = NULL;
		}
		it_IDFEntry->second.lpValue = new char[nLength];
		memcpy_s(it_IDFEntry->second.lpValue, nLength, pExifInfo, nLength);
		it_IDFEntry++;
		long offsetfilePos = offsetfromvalue*g_types[addtnery.nFieldType - 1];
		m_fex.offset_DES += offsetfilePos;
		do
		{
			while( it_IDFEntry != pCurrentIFDs->mapIFDEntry.end() )
			{
				it_IDFEntry->second.alterOffset = it_IDFEntry->second.alterOffset + offsetfilePos;
				it_IDFEntry++;
			}
			pCurrentIFDs = pCurrentIFDs->pNextIFD;
			if( pCurrentIFDs )
			{
				it_IDFEntry = pCurrentIFDs->mapIFDEntry.begin();
			}
		}while(pCurrentIFDs);
	}
	else  //tag不存在
	{
		LPEXIFINFONODE pInfonode = new EXIFINFONODE;
		pInfonode->num = nLength;
		pInfonode->pInfo = new char[nLength];
		memcpy_s(pInfonode->pInfo, nLength, pExifInfo, nLength);
		CExifTagMap::iterator it = m_mapTag.find(iTag);
		if( it == m_mapTag.end() )
		{
			char sztag[2] = {iTag/256, iTag%256};
			USES_CONVERSION;
			m_mapExifInfo[A2T(sztag)] = pInfonode;
		}
		else
		{
			m_mapExifInfo[it->second.strTag] = pInfonode;
		}
		if( !pCurrentIFDs->mapIFDEntry.empty() )
		{
			do
			{
				it_IDFEntry = pCurrentIFDs->mapIFDEntry.end();
				it_IDFEntry--;
				addtnery.lpValue = new char[nLength];
				memcpy_s(addtnery.lpValue, nLength, pExifInfo, nLength);
				addtnery.operateType = addtype;
				addtnery.filePos = it_IDFEntry->second.filePos + 12;
				pCurrentIFDs->mapIFDEntry[iTag] = addtnery;
				pCurrentIFDs->numOfEntry++;
				m_fex.offset_DES = m_fex.offset_DES + 12 + nLength;
				it_IDFEntry = pCurrentIFDs->mapIFDEntry.begin();
				BOOL bfindnewtag = FALSE;
				int currentindex = 0;
				int ifdmapsize = (int)pCurrentIFDs->mapIFDEntry.size();
				while( it_IDFEntry != pCurrentIFDs->mapIFDEntry.end() )
				{
					currentindex ++;
					if( it_IDFEntry->first == iTag )
					{
						bfindnewtag = TRUE;
						if( ifdmapsize >= currentindex && currentindex > 1 )
						{
							it_IDFEntry--;
							addtnery.alterOffset = it_IDFEntry->second.alterOffset + it_IDFEntry->second.numOfVal*g_types[it_IDFEntry->second.nFieldType - 1] ;
							it_IDFEntry++;
						}
						else if( 1 == currentindex )
						{
							it_IDFEntry++;
							addtnery.alterOffset = it_IDFEntry->second.alterOffset + 12;
							it_IDFEntry--;
						}
						addtnery.offset = addtnery.alterOffset;
						it_IDFEntry->second.alterOffset = addtnery.alterOffset;
						it_IDFEntry++;
						continue;
					}
					if( !bfindnewtag )
					{
						it_IDFEntry->second.alterOffset = it_IDFEntry->second.alterOffset + 12;
					}
					else
					{
						it_IDFEntry->second.alterOffset = it_IDFEntry->second.alterOffset + 12 + nLength;
					}
					it_IDFEntry++;
				}
				pCurrentIFDs = pCurrentIFDs->pNextIFD;
			}while(pCurrentIFDs);
		}
		else
		{
			m_exifHead.exifEndian = big_endian;
			addtnery.lpValue = new char[nLength];
			memcpy_s(addtnery.lpValue, nLength, pExifInfo, nLength);
			addtnery.operateType = addtype;
			addtnery.filePos = m_fileStartPos + 21;
			addtnery.alterOffset = 26;  //10 + 12 + 4
			addtnery.offset = addtnery.alterOffset;
			pCurrentIFDs->mapIFDEntry[iTag] = addtnery;
			pCurrentIFDs->numOfEntry++;
			m_fex.offset_DES = 34 + nLength; //18  + 12 + 4
		}
		//pcurrentifds->numofentry = (int)m_ifds.ifdentrymap.size();
	}
	return bResult;
}

E_EXIFERROR CJpgExif::SaveToFile(LPCTSTR szJpgPath, LPTSTR lpSavePath )
{
	TCHAR temppath[EXIF_MAX_PATH] = {0};
	DWORD pathlen = GetTempPath(EXIF_MAX_PATH,temppath);
	if(pathlen <= 0 || pathlen > EXIF_MAX_PATH )
	{
		return unknow_error;
	}
	if( _T('\\') != temppath[pathlen - 1] )
	{
		temppath[pathlen] = _T('\\');
		temppath[pathlen + 1] = 0;
	}
	srand((unsigned int)time(NULL));
	int randname = rand()%100000 + 1000;
	TCHAR tempname[EXIF_MAX_PATH] = {0};
	SYSTEMTIME stm;
	GetLocalTime(&stm);
	_stprintf_s(tempname,EXIF_MAX_PATH,_T("%s%d%d%d%dqpg.dat"),temppath,stm.wYear,stm.wMonth,stm.wDay,randname);
	_tstring _tempfilename = tempname;
	FILE * _pTempIstreamFile = NULL, *_pTempOstreamFile = NULL;

	USES_CONVERSION;
	errno_t _error = _wfopen_s(&_pTempOstreamFile, CT2W(szJpgPath), T2W(_T("rb")));
	if( _error != 0)
	{
		return open_file_failed;
	}
	_error = _wfopen_s(&_pTempIstreamFile, CT2W(_tempfilename.c_str()), T2W(_T("w+b")));
	if( _error != 0 )
	{
		return open_file_failed;
	}
	long filePos = 0;
    long readLen = 0;
	char * pBuffer = NULL;
	if( -1 != m_fex.filePos )
	{
		filePos = m_fex.filePos + 1;
		pBuffer = new char[filePos];
		if( filePos == fread_s(pBuffer, filePos, 1, filePos, _pTempOstreamFile) )
		{
			fwrite(pBuffer, 1, filePos, _pTempIstreamFile);
		}
		else
		{
			delete pBuffer;
			ResetWrite();
			fclose(_pTempIstreamFile);
			fclose(_pTempOstreamFile);
			return seek_failed;
		}
		delete pBuffer;

		UCHAR fexOffset[2] = {0};
		fread_s(fexOffset, 2, 1, 2, _pTempOstreamFile);
		fexOffset[0] = UCHAR(m_fex.offset_DES/256);
		fexOffset[1] = UCHAR(m_fex.offset_DES%256);
		if( little_endian == m_exifHead.exifEndian )
		{
			UCHAR temp = fexOffset[0];
			fexOffset[0] = fexOffset[1];
			fexOffset[1] = temp;
		}
		fwrite(fexOffset, 1, 2, _pTempIstreamFile);

		//exif head
		readLen = m_exifHead.offset + 6;
		pBuffer = new char[readLen];
		if( readLen == fread_s(pBuffer, readLen, 1, readLen, _pTempOstreamFile) )
		{
			fwrite(pBuffer, 1, readLen, _pTempIstreamFile);
		}
		else
		{
			delete pBuffer;
			ResetWrite();
			fclose(_pTempIstreamFile);
			fclose(_pTempOstreamFile);
			return seek_failed;
		}
		delete pBuffer;
	}
	else
	{
		filePos = m_fileStartPos;
		pBuffer = new char[filePos];
		if( filePos == fread_s(pBuffer, filePos, 1, filePos, _pTempOstreamFile) )
		{
			fwrite(pBuffer, 1, filePos, _pTempIstreamFile);
		}
		else
		{
			delete pBuffer;
			ResetWrite();
			fclose(_pTempIstreamFile);
			fclose(_pTempOstreamFile);
			return seek_failed;
		}
		delete pBuffer;
		UCHAR tag[2] = {0xFF,0xE1};
		fwrite(tag, 1, 2, _pTempIstreamFile);

		UCHAR fexOffset[2] = {0};
		fexOffset[0] = UCHAR(m_fex.offset_DES/256);
		fexOffset[1] = UCHAR(m_fex.offset_DES%256);
		if( little_endian == m_exifHead.exifEndian )
		{
			UCHAR temp = fexOffset[0];
			fexOffset[0] = fexOffset[1];
			fexOffset[1] = temp;
		}
		fwrite(fexOffset, 1, 2, _pTempIstreamFile);

		char exifHead[14] = {69, 120, 105, 102, 0, 0, 77, 77, 0, 42, 0, 0, 0, 8};
		fwrite(exifHead, 1, 14, _pTempIstreamFile);
	}

	//ifd num
	CExifIFD * pCurrentIFDs = &m_IFDs;
	int maxValueOffset = 0;
	while( pCurrentIFDs )
	{
		char numOfIFD [2] = {0};
		if( -1 != m_fex.filePos )
			fread_s(numOfIFD, 2, 1, 2, _pTempOstreamFile);
		numOfIFD[0] =  pCurrentIFDs->numOfEntry/256;
		numOfIFD[1] =  pCurrentIFDs->numOfEntry%256;
		if( little_endian == m_exifHead.exifEndian )
		{
			UCHAR temp = numOfIFD[0];
			numOfIFD[0] = numOfIFD[1];
			numOfIFD[1] = temp;
		}
		fwrite(numOfIFD, 1, 2, _pTempIstreamFile);

		//write ifd
		pBuffer = new char[12];
		CExifIFDEntryMap::iterator it_IFDEntry = pCurrentIFDs->mapIFDEntry.begin();
		for( int i = 0;i < pCurrentIFDs->numOfEntry; i++, it_IFDEntry++ )
		{
			if( addtype == it_IFDEntry->second.operateType )
			{
				pBuffer[0] = it_IFDEntry->second.tag[0];
				pBuffer[1] = it_IFDEntry->second.tag[1];
				char exifType[2];
				{
					exifType[0] = it_IFDEntry->second.nFieldType / 256;
					exifType[1] = it_IFDEntry->second.nFieldType % 256;
				}
				if( little_endian == m_exifHead.exifEndian)
				{
					UCHAR temp = exifType[0];
					exifType[0] = exifType[1];
					exifType[1] = temp;
				}
				memcpy_s(pBuffer + 2, 2, exifType, 2);
				char numOfVal[4];
				{
					numOfVal[0] = it_IFDEntry->second.numOfVal / 16777216;
					numOfVal[1] = (it_IFDEntry->second.numOfVal % 16777216) / 65536;
					numOfVal[2] = ((it_IFDEntry->second.numOfVal % 16777216) % 65536)/256;
					numOfVal[3] = it_IFDEntry->second.numOfVal % 256;
				}
				if( little_endian == m_exifHead.exifEndian)
				{
					for( int i = 0;i < 2 ;i++)
					{
						UCHAR temp = numOfVal[i];
						numOfVal[i] = numOfVal[3 - i];
						numOfVal[3 - i] = temp;
					}
				}
				memcpy_s(pBuffer + 4, 4, numOfVal, 4);
				{
					numOfVal[0] = (char)(it_IFDEntry->second.alterOffset / 16777216);
					numOfVal[1] = (char)((it_IFDEntry->second.alterOffset % 16777216) / 65536);
					numOfVal[2] = (char)(((it_IFDEntry->second.alterOffset % 16777216) % 65536)/256);
					numOfVal[3] = (char)(it_IFDEntry->second.alterOffset % 256);
				}
				if( little_endian == m_exifHead.exifEndian)
				{
					for( int i = 0;i < 2 ;i++)
					{
						UCHAR temp = numOfVal[i];
						numOfVal[i] = numOfVal[3 - i];
						numOfVal[3 - i] = temp;
					}
				}
				memcpy_s(pBuffer + 8, 4, numOfVal, 4);
			}
			else if( altertype == it_IFDEntry->second.operateType )
			{
				if( 12 == fread_s(pBuffer, 12, 1, 12, _pTempOstreamFile) )
				{
					if( altertype == it_IFDEntry->second.operateType )
					{
						char numOfVal[4];
						{
							numOfVal[0] = it_IFDEntry->second.numOfVal / 16777216;
							numOfVal[1] = (it_IFDEntry->second.numOfVal % 16777216) / 65536;
							numOfVal[2] = ((it_IFDEntry->second.numOfVal % 16777216) % 65536)/256;
							numOfVal[3] = it_IFDEntry->second.numOfVal % 256;
						}
						if( little_endian == m_exifHead.exifEndian)
						{
							for( int i = 0;i < 2 ;i++)
							{
								UCHAR temp = numOfVal[i];
								numOfVal[i] = numOfVal[3 - i];
								numOfVal[3 - i] = temp;
							}
						}
						memcpy_s(pBuffer + 4, 4, numOfVal, 4);
						{
							numOfVal[0] = (char)(it_IFDEntry->second.alterOffset / 16777216);
							numOfVal[1] = (char)((it_IFDEntry->second.alterOffset % 16777216) / 65536);
							numOfVal[2] = (char)(((it_IFDEntry->second.alterOffset % 16777216) % 65536)/256);
							numOfVal[3] = (char)(it_IFDEntry->second.alterOffset % 256);
						}
						if( little_endian == m_exifHead.exifEndian)
						{
							for( int i = 0;i < 2 ;i++)
							{
								UCHAR temp = numOfVal[i];
								numOfVal[i] = numOfVal[3 - i];
								numOfVal[3 - i] = temp;
							}
						}
						memcpy_s(pBuffer + 8, 4, numOfVal, 4);
					}
				}
				else
				{
					continue;
				}
			}
			else if( -1 == it_IFDEntry->second.operateType )
			{
				if( 12 != fread_s(pBuffer, 12, 1, 12, _pTempOstreamFile) )
				{
					continue;
				}
				char numOfVal[4];
				{
					numOfVal[0] = (char)(it_IFDEntry->second.alterOffset / 16777216);
					numOfVal[1] = (char)((it_IFDEntry->second.alterOffset % 16777216) / 65536);
					numOfVal[2] = (char)(((it_IFDEntry->second.alterOffset % 16777216) % 65536)/256);
					numOfVal[3] = (char)(it_IFDEntry->second.alterOffset % 256);
				}
				if( little_endian == m_exifHead.exifEndian)
				{
					for( int i = 0;i < 2 ;i++)
					{
						UCHAR temp = numOfVal[i];
						numOfVal[i] = numOfVal[3 - i];
						numOfVal[3 - i] = temp;
					}
				}
				memcpy_s(pBuffer + 8, 4, numOfVal, 4);
			}
			fwrite(pBuffer, 1, 12, _pTempIstreamFile);
		}
		delete pBuffer;

		if( -1 == m_fex.filePos )
		{
			char ifdend[4] = {0};
			fwrite(ifdend, 1, 4, _pTempIstreamFile);
		}

		//ifd value
		it_IFDEntry = pCurrentIFDs->mapIFDEntry.begin();
		while( it_IFDEntry != pCurrentIFDs->mapIFDEntry.end() && addtype == it_IFDEntry->second.operateType )
		{
			it_IFDEntry++;
		}
		if( it_IFDEntry != pCurrentIFDs->mapIFDEntry.end() )
		{
			readLen = ftell(_pTempOstreamFile);
			readLen = it_IFDEntry->second.offset + m_exifHead.headPos - readLen - 1;
			if( readLen > 0 )
			{
				pBuffer = new char[readLen];
				fread_s(pBuffer, readLen, 1, readLen, _pTempOstreamFile);
				fwrite(pBuffer, 1, readLen, _pTempIstreamFile);
				delete pBuffer;
			}
		}

		it_IFDEntry = pCurrentIFDs->mapIFDEntry.begin();
		while( it_IFDEntry != pCurrentIFDs->mapIFDEntry.end())
		{
			if( addtype != it_IFDEntry->second.operateType )
			{
				if( m_exifHead.headPos + it_IFDEntry->second.offset >= maxValueOffset)
				{
					maxValueOffset = m_exifHead.headPos + it_IFDEntry->second.offset + it_IFDEntry->second.numOfVal*g_types[it_IFDEntry->second.nFieldType - 1] - 1;
				}
			}
			if( it_IFDEntry->second.lpValue )
			{
				fwrite(it_IFDEntry->second.lpValue, 1, it_IFDEntry->second.numOfVal*g_types[it_IFDEntry->second.nFieldType - 1], _pTempIstreamFile);
			}
			it_IFDEntry++;
		}
		it_IFDEntry = pCurrentIFDs->mapIFDEntry.find(0x8769);
		if( it_IFDEntry != pCurrentIFDs->mapIFDEntry.end() )
		{
			fseek(_pTempOstreamFile, m_exifHead.headPos + it_IFDEntry->second.offset - 1, SEEK_SET);
			fseek(_pTempIstreamFile, -4, SEEK_CUR);
		}
		pCurrentIFDs = pCurrentIFDs->pNextIFD;
	}

	if( 0 != maxValueOffset && 0 != fseek(_pTempOstreamFile, maxValueOffset, SEEK_SET) )
	{
		ResetWrite();
		fclose(_pTempIstreamFile);
		fclose(_pTempOstreamFile);
		return seek_failed;
	}
	filePos = ftell(_pTempOstreamFile);
	fseek(_pTempOstreamFile, 0, SEEK_END);
	readLen = ftell(_pTempOstreamFile) - filePos;
	fseek(_pTempOstreamFile, filePos, SEEK_SET);
    if(readLen > 0 && readLen < 0x03200000)
    {
        pBuffer = new char[readLen];
        fread_s(pBuffer, readLen, 1, readLen, _pTempOstreamFile);
        fwrite(pBuffer, 1, readLen, _pTempIstreamFile);
        delete pBuffer;
        pBuffer = NULL;
    }
    else
    {
        fclose(_pTempIstreamFile);
        fclose(_pTempOstreamFile);
        //DeleteFile(szJpgPath);
        return unknow_error;
    }

	fclose(_pTempIstreamFile);
	fclose(_pTempOstreamFile);
	//TCHAR newfilename[] = _T("C:\\Documents and Settings\\Administrator\\桌面\\复件 watermark34511.JPG");
	if( _tcslen(lpSavePath ) <= 0 )
	{
		DeleteFile(szJpgPath);
		MoveFile(_tempfilename.c_str(), szJpgPath);
	}
	else
	{
		DeleteFile(lpSavePath);
		MoveFile(_tempfilename.c_str(), lpSavePath);
	}

	ResetWrite();
	_tempfilename = _T("");
	return sucess;
}

E_EXIFERROR CJpgExif::Init(LPTSTR szJpgPath)
{
	E_EXIFERROR bResult = unknow_error;
	if( 0 != _tcscmp(m_initJpgPath.c_str(), szJpgPath) )
	{
		m_initJpgPath = _T("");
		bResult = (E_EXIFERROR)InitJpegMap(szJpgPath) ;
		if( sucess == bResult )
		{
			m_initJpgPath = szJpgPath;
		}
		else
		{
			return bResult;
		}
	}
	return bResult;
}

void CJpgExif::ReleaseIFDs(CExifIFD * lpIFDs)
{
	if( lpIFDs->pNextIFD )
	{
		ReleaseIFDs(lpIFDs->pNextIFD);
		delete lpIFDs->pNextIFD;
		lpIFDs->pNextIFD = NULL;
	}
	else
	{
		lpIFDs->mapIFDEntry.clear();
	}
}

void CJpgExif::Clear()
{
	m_mapTag.clear();
	CExifMap::iterator it_exif = m_mapExifInfo.begin();
	while( it_exif != m_mapExifInfo.end() )
	{
		if( it_exif->second )
		{
			if( it_exif->second->pInfo )
			{
				delete it_exif->second->pInfo;
				it_exif->second->pInfo = NULL;
			}
		}
		it_exif++;
	}
	m_mapExifInfo.clear();
	m_exifHead.reset();
	ReleaseIFDs(&m_IFDs);
	m_fex.reset();
	m_initJpgPath = _T("");
	m_fileStartPos = 0;
}