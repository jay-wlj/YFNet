#ifndef _QVOD_MINFO_H__
#define _QVOD_MINFO_H__

#include "..\..\include\IQvodMInfo.h"

class CQvodMInfo : public IQvodMInfo
{
public:
	CQvodMInfo();
	~CQvodMInfo();
//IQvodMInfo
public:
	BOOL LoadFile(wchar_t *pstrfilename);
	BOOL GetMediaInfo(MediaInfo_s *pMediaInfo);
	void Clear();
private:
	wchar_t *m_pstrfilename;
	MediaInfo_s	*m_mediainfo;
};


#endif