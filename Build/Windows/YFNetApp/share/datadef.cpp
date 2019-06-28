//#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "datadef.h"

bool CChannel::load(char*& pbuf,char* pend)
{
	if(pbuf >= pend)
		return false;
	
	strname = pbuf;
	pbuf += strname.size() +1;
	strkey = pbuf;
	pbuf += strkey.size() +1;
	strchargetype = pbuf;
	pbuf += strchargetype.size() +1;
	strchargetype = pbuf;
	pbuf += strchargetype.size() +1;
	strcertifyurl = pbuf;
	pbuf += strcertifyurl.size() +1;
	strlogo = pbuf;
	pbuf += strlogo.size() +1;
	strinfo = pbuf;
	pbuf += strinfo.size() +1;
	if(!listnew.load(pbuf,pend))
		return false;
	if(!listrecommend.load(pbuf,pend))
		return false;
	if(!listhot.load(pbuf,pend))
		return false;
	if(!listclass.load(pbuf,pend))
		return false;
	
	return pbuf <= pend;
}
char* CChannel::flush(char* pbuf,UINT level)
{
	char* ptmp = pbuf;
	ptmp += sprintf(ptmp,"%s",strname.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strkey.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strchanneltype.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strchargetype.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strcertifyurl.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strlogo.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strinfo.c_str());
	*ptmp++ = 0;
	ptmp = listnew.flush(ptmp,--level);
	ptmp = listrecommend.flush(ptmp,level);
	ptmp = listhot.flush(ptmp,level);
	ptmp = listclass.flush(ptmp,level);
	return ptmp;
}
bool CChannel::flush(FILE* pf)
{
	fwrite(strname.c_str(),1,strname.size()+1,pf);
	fwrite(strkey.c_str(),1,strkey.size()+1,pf);
	fwrite(strchanneltype.c_str(),1,strchanneltype.size()+1,pf);
	fwrite(strchargetype.c_str(),1,strchargetype.size()+1,pf);
	fwrite(strcertifyurl.c_str(),1,strcertifyurl.size()+1,pf);
	fwrite(strlogo.c_str(),1,strlogo.size()+1,pf);
	fwrite(strinfo.c_str(),1,strinfo.size()+1,pf);
	listnew.flush(pf);
	listrecommend.flush(pf);
	listhot.flush(pf);
	listclass.flush(pf);
	return true;
}

bool CClass::load(char*& pbuf,char* pend)
{
	if(pbuf >= pend)
		return false;
	
	strname = pbuf;
	pbuf += strname.size() +1;
	strkey = pbuf;
	pbuf += strkey.size() +1;
	if(!listfilm.load(pbuf,pend))
		return false;
	
	return pbuf <= pend;
}
char* CClass::flush(char* pbuf,UINT level)
{
	char* ptmp = pbuf;
	ptmp += sprintf(ptmp,"%s",strname.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strkey.c_str());
	*ptmp++ = 0;
	ptmp = listfilm.flush(ptmp,--level);
	return ptmp;
}
bool CClass::flush(FILE* pf)
{
	fwrite(strname.c_str(),1,strname.size()+1,pf);
	fwrite(strkey.c_str(),1,strkey.size()+1,pf);
	listfilm.flush(pf);
	return true;
}


bool CFilm::load(char*& pbuf,char* pend)
{
	if(pbuf >= pend)
		return false;
	
	strname = pbuf;
	pbuf += strname.size() +1;
	strkey = pbuf;
	pbuf += strkey.size() +1;
	strclass = pbuf;
	pbuf += strclass.size() +1;
	strcode = pbuf;
	pbuf += strcode.size() +1;
	strcountry = pbuf;
	pbuf += strcountry.size() +1;
	strdirector = pbuf;
	pbuf += strdirector.size() +1;
	stractors = pbuf;
	pbuf += stractors.size() +1;
	strupdatedate = pbuf;
	pbuf += strupdatedate.size() +1;
	strshowdate = pbuf;
	pbuf += strshowdate.size() +1;
	memcpy(&score,pbuf,sizeof(score));
	pbuf += sizeof(score);
	memcpy(&men,pbuf,sizeof(men));
	pbuf += sizeof(men);
	memcpy(&playduring,pbuf,sizeof(playduring));
	pbuf += sizeof(playduring);
	memcpy(&hits,pbuf,sizeof(hits));
	pbuf += sizeof(hits);
	strlanguage = pbuf;
	pbuf += strlanguage.size() +1;
	strpicture = pbuf;
	pbuf += strpicture.size() +1;
	strcontent = pbuf;
	pbuf += strcontent.size() +1;
	
	if(!listpicture.load(pbuf,pend))
		return false;
	if(!listPart.load(pbuf,pend))
		return false;

	return pbuf <= pend;
}
char* CFilm::flush(char* pbuf,UINT level)
{
	char* ptmp = pbuf;
	ptmp += sprintf(ptmp,"%s",strname.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strkey.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strclass.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strcode.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strcountry.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strdirector.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",stractors.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strupdatedate.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strshowdate.c_str());
	*ptmp++ = 0;

	memcpy(ptmp,&score,sizeof(score));
	ptmp += sizeof(score);
	memcpy(ptmp,&men,sizeof(men));
	ptmp += sizeof(men);
	memcpy(ptmp,&playduring,sizeof(playduring));
	ptmp += sizeof(playduring);
	memcpy(ptmp,&hits,sizeof(hits));
	ptmp += sizeof(hits);

	ptmp += sprintf(ptmp,"%s",strlanguage.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strpicture.c_str());
	*ptmp++ = 0;
	ptmp += sprintf(ptmp,"%s",strcontent.c_str());
	*ptmp++ = 0;

	--level;
	ptmp = listpicture.flush(ptmp,level);
	ptmp = listPart.flush(ptmp,level);

	return ptmp;
}
bool CFilm::flush(FILE* pf)
{
	fwrite(strname.c_str(),1,strname.size()+1,pf);
	fwrite(strkey.c_str(),1,strkey.size()+1,pf);
	fwrite(strclass.c_str(),1,strclass.size()+1,pf);
	fwrite(strcode.c_str(),1,strcode.size()+1,pf);
	fwrite(strcountry.c_str(),1,strcountry.size()+1,pf);
	fwrite(strdirector.c_str(),1,strdirector.size()+1,pf);
	fwrite(stractors.c_str(),1,stractors.size()+1,pf);
	fwrite(strupdatedate.c_str(),1,strupdatedate.size()+1,pf);
	fwrite(strshowdate.c_str(),1,strshowdate.size()+1,pf);

	fwrite(&score,1,sizeof(score),pf);
	fwrite(&men,1,sizeof(men),pf);
	fwrite(&playduring,1,sizeof(playduring),pf);
	fwrite(&hits,1,sizeof(hits),pf);
	
	fwrite(strlanguage.c_str(),1,strlanguage.size()+1,pf);
	fwrite(strpicture.c_str(),1,strpicture.size()+1,pf);
	fwrite(strcontent.c_str(),1,strcontent.size()+1,pf);

	listpicture.flush(pf);
	listPart.flush(pf);
	return true;
}

bool CPart::load(char*& pbuf,char* pend)
{
	if(pbuf >= pend)
		return false;

	strurl = pbuf;
	pbuf += strurl.size() +1;

	return pbuf <= pend;
}
char* CPart::flush(char* pbuf,UINT level)
{
	char* ptmp = pbuf;
	ptmp += sprintf(ptmp,"%s",strurl.c_str());
	*ptmp++ = 0;
	return ptmp;
}
bool CPart::flush(FILE* pf)
{
	fwrite(strurl.c_str(),1,strurl.size()+1,pf);

	return true;
}


bool CPicture::load(char*& pbuf,char* pend)
{
	if(pbuf >= pend)
		return false;

	strkey = pbuf;
	pbuf += strkey.size() +1;
	return pbuf <= pend;
}
char* CPicture::flush(char* pbuf,UINT level)
{
	char* ptmp = pbuf;
	ptmp += sprintf(ptmp,"%s",strkey.c_str());
	*ptmp++ = 0;
	return ptmp;
}
bool CPicture::flush(FILE* pf)
{
	fwrite(strkey.c_str(),1,strkey.size()+1,pf);
	return true;
}




