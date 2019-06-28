#pragma once
//#include "stdafx.h"

#if UNICODE
#define _tstring std::wstring
#else
#define _tstring std::string
#endif

// ret:
//			>0 �ɹ�
//			-1 �ļ�������
//			-2 �ļ���ʧ��
//			-3 �ļ�����д
//			-4 ��ѯ���Բ�����
//			-5 �ļ�·�����Ϸ�
//			-6 δ֪����
//			-7 �ļ�seekʧ��
//			-8 �����ļ�·�����Ϸ�

enum E_EXIFERROR
{
	invalid_save_path = -8,
	seek_failed,
	unknow_error,
	invalid_file_path,
	invalid_type,
	file_cannot_be_write,
	open_file_failed,
//	fileinexistence,
	sucess = 0
};

enum E_EXIFSTATE
{
	unknow_state = 0,
	exif_flag,
	exif_len,
	use_exif,
	endian,
	usetiff,
	exifoffset
};

enum EXIFTYPES
{
	E_BYTE = 1,
	E_ASC,
	E_SHORT,
	E_LONG,
	E_RATIONAL,
	E_SBYTE,
	E_UNDEFINED,
	E_SSHORT,
	E_SLONG,
	E_SRATIONAL,
	E_FLOAT,
	E_DOUBLE
};
#define exiftotaltypes 12

enum EXIFENDIAN
{
	little_endian = 6,
	big_endian
};

enum EXIFOPERAT
{
	addtype = 0,
	altertype
};

typedef struct TAGTRANSLATE
{
	int iTag;
	_tstring strTag;
	int tagType;
}TAGTRANSLATE;

typedef struct EXIFHEAD
{
	EXIFHEAD()
	{
		reset();
	}
	void reset()
	{
		exifEndian = 0;
		bTiff = FALSE;
		offset = 8;
		headPos = -1;
	}
	int		exifEndian;
	BOOL	bTiff;
	int		offset;
	int		headPos;
}CExifHead;

typedef struct EXIFIFDENTRY
{
	EXIFIFDENTRY()
	{
		reset();
	}
	void reset()
	{
		memset(tag,0,2);
		nFieldType = -1;
		numOfVal = 0;
		offset = 0;
		alterOffset = 0;
		filePos = -1;
		operateType = -1;
		lpValue = NULL;
	}
	UCHAR	tag[2];
	int		nFieldType;
	int		numOfVal;
	int		offset;
	long	alterOffset;
	long	filePos;
	int		operateType;
	char	*lpValue;
}CExifIFDEntry;
typedef std::map<int,CExifIFDEntry> CExifIFDEntryMap;

typedef struct EXIFIFD
{
	EXIFIFD()
	{
		numOfEntry = 0;
		pNextIFD = NULL;
	}
	void reset()
	{
		numOfEntry = 0;
		mapIFDEntry.clear();
		filePos = -1;
	}
	int numOfEntry;
	long filePos;
	CExifIFDEntryMap mapIFDEntry;
	struct EXIFIFD * pNextIFD;
}CExifIFD;

typedef struct EXIFTAGNODE
{
	EXIFTAGNODE()
	{
		strTag = _T("");
		cbType = 0;
	}
	_tstring strTag;
	size_t cbType;
}EXIFTAGNODE;

typedef struct EXIFINFONODE
{
	EXIFINFONODE()
	{
		pInfo = NULL;
		num = 0;
	}
	char * pInfo;
	size_t num;
	~EXIFINFONODE()
	{
		if( pInfo )
		{
			delete pInfo;
			pInfo = NULL;
		}
		num = 0;
	}
}EXIFINFONODE,*LPEXIFINFONODE;

typedef struct EXIFFEX
{
	EXIFFEX()
	{
		reset();
	}
	void reset()
	{
		offset_FEX = 0;
		filePos = -1;
		offset_DES = 0;
	}
	long offset_FEX;
	long filePos;
	long offset_DES;
}EXIFFEX;

#define EXIF_MAX_PATH 512

typedef std::map<int,EXIFTAGNODE> CExifTagMap;
typedef std::map<_tstring,LPEXIFINFONODE> CExifMap;