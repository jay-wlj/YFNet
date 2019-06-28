#ifndef _DATADEFINE_H
#define _DATADEFINE_H

template <class T>
struct List
{
	UINT num;
	std::vector<T> datalist;
	List(){num = 0;}
	T& operator[](int i){return datalist[i];}
};
struct object
{
	virtual bool load(char*& pbuf,char* pend) = 0;
	virtual char* flush(char* pbuf,UINT level = 0xFFFFFFFF) = 0;
	virtual bool flush(FILE* pf) = 0;
};

template <class T>
class CList : public List< T>,object
{
public:
	virtual bool load(char*& pbuf,char* pend)
	{
		if(pbuf +sizeof(num) > pend)
			return false;
		
		memcpy(&num,pbuf,sizeof(num));
		pbuf += sizeof(num);
		datalist.clear();
		int i;
		for(i =0;i<num;i++)
		{
			T t;
			datalist.push_back(t);
			if(!datalist[i].load(pbuf,pend))
				return false;
		}
		return true;
	}
	virtual char* flush(char* pbuf,UINT level = 0xFFFFFFFF)
	{
		char* ptmp = pbuf;
		if(level == 0)
		{
			memset(ptmp,0,sizeof(num));
			ptmp += sizeof(num);
			return ptmp;
		}
		memcpy(ptmp,&num,sizeof(num));
		ptmp += sizeof(num);
		std::vector<T>::iterator it;
		for(it = datalist.begin();it != datalist.end();it++)
		{
			T* pt = it;
			ptmp = pt->flush(ptmp,level);
		}	
		return ptmp;
	}
	virtual bool flush(FILE* pf)
	{
		fwrite(&num,1,sizeof(num),pf);
		std::vector<T>::iterator it;
		for(it = datalist.begin();it != datalist.end();it++)
		{
			T* pt = it;
			if(!pt->flush(pf))
				return false;
		}
		return true;
	}
	
	void push_back(T& t){datalist.push_back(t);num=datalist.size();}
	void insert(std::vector<T>::iterator it,T& t){datalist.insert(it,t);num = datalist.size();}
	void erase(std::vector<T>::iterator it){datalist.erase(it);num=datalist.size();}
};


struct Picture
{
	std::string strkey;
};
class CPicture : public Picture,object
{
public:
	virtual bool load(char*& pbuf,char* pend);
	virtual char* flush(char* pbuf,UINT level = 0xFFFFFFFF);
	virtual bool flush(FILE* pf);
};
struct Part 
{
	std::string strname;
	std::string strurl;
};
class CPart : public Part,object
{
public:
	virtual bool load(char*& pbuf,char* pend);
	virtual char* flush(char* pbuf,UINT level = 0xFFFFFFFF);
	virtual bool flush(FILE* pf);
};

struct Film
{
	std::string strname;
	std::string strkey;
	std::string strclass;
	std::string strcode;  // Öú¼ÇÂë
	std::string strcountry;


	std::string strdirector;
	std::string stractors;
	std::string strupdatedate;
	std::string strshowdate;
	UINT		score;
	UINT		men;
	UINT		playduring;
	UINT		hits;
	std::string strlanguage;
	std::string strpicture;
	std::string strcontent;	
	CList<CPicture> listpicture;
	CList<CPart> listPart;
	
};
class CFilm : public Film,object
{
public:
	virtual bool load(char*& pbuf,char* pend);
	virtual char* flush(char* pbuf,UINT level = 0xFFFFFFFF);
	virtual bool flush(FILE* pf);
};

struct Class
{
	std::string strname;
	std::string strkey;
	CList<CFilm> listfilm;
};
class CClass : public Class,object
{
public:
	virtual bool load(char*& pbuf,char* pend);
	virtual char* flush(char* pbuf,UINT level = 0xFFFFFFFF);
	virtual bool flush(FILE* pf);
};

struct Channel
{
	std::string strname;
	std::string strkey;	
	std::string strchanneltype;
	std::string strchargetype;
	std::string strcertifyurl;
	std::string strlogo;
	std::string strinfo;
	CList<CFilm> listnew;
	CList<CFilm> listrecommend;
	CList<CFilm> listhot;
	CList<CClass> listclass;
};


class CChannel : public Channel,object
{
public:
	virtual bool load(char*& pbuf,char* pend);
	virtual char* flush(char* pbuf,UINT level = 0xFFFFFFFF);
	virtual bool flush(FILE* pf);
};


#endif