
/*
 * QVOD file.
 */

#include "qvod_file.h"
#include "qvod_error.h"
#include "common-utility.h"
#include "common-commonfun.h"

#ifndef WIN32 /* WIN32 */
#include <sys/param.h>
#include <sys/mount.h>
#endif

#ifdef MACOSX
#import <Foundation/Foundation.h>
#endif

#define QVOD_POSIX_TEMP_PATH  "/tmp/"



int QvodRemoveFolder(const char *pathname)
{
#ifdef WIN32 /* WIN32 */
	RemoveDirectory( pathname ) ;
	return QVOD_OK;
#else /* posix */
	int res;
	DIR *pDir;
	struct dirent *ent;
	std::string strfolder = std::string(pathname);

	_OUTPUT(INFO,"opendir %s\n", pathname);
	pDir = opendir(pathname);

	while((ent = readdir(pDir)) != NULL)
	{
		if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}

		std::string	ent_name = std::string(ent->d_name);

		if(ent->d_type == DT_DIR)
		{
			/* directory, recursion */
			res = QvodRemoveFolder((strfolder + ent_name).c_str());
			if(res != QVOD_OK) {
				closedir(pDir);
				return QVOD_ERROR;
			}
		}
		else
		{
			/* file, delete */
			res = remove((strfolder + ent_name).c_str());
			if(res < 0)
			{
				/* error */
				closedir(pDir);
				return QVOD_ERROR;
			}
		}
	}

	closedir(pDir);

	/* delete empty folder */
	remove(pathname);

	return QVOD_OK;
#endif
}

/* file op */
int QvodRemoveFile(const char *pathname)
{
#ifdef WIN32 /* WIN32 */
	DeleteFile(pathname);
	return QVOD_OK;
#else /* posix */
	int res = remove(pathname);
	if(res != 0)
	{
		_OUTPUT(ERROR1,"%s(%d)-%s: remove %s failed: %d\n", __FILE__, __LINE__, __FUNCTION__, pathname, errno);

		if(errno == ENOTEMPTY)
		{
			/* delete folder that not empty */
			return QvodRemoveFolder(pathname);
		}

		if(errno == EROFS)
		{
			return QVOD_EROFS;
		}

		/* other errors */
		return QVOD_ERROR;
	}

	_OUTPUT(INFO,"%s(%d)-%s: remove %s ok\n", __FILE__, __LINE__, __FUNCTION__, pathname);
	return QVOD_OK;
#endif /* posix end */
}


int QVOD_STAT64(const char *pathname, qvod_stat64_t *st)
{
	int ret = -1;
    
    ret = QVOD_STAT64(pathname, st);
    
	return ret;
}

int QvodMkdir(const char *dirname)
{
	int res;
	char path[256] = {0};
	memcpy(path, dirname, strlen(dirname));
#ifdef WIN32 /* WIN32 */
	char* pEnd = strstr(path, "\\");
	if(NULL == pEnd)
	{
		res = _mkdir(path);
	}
	while(pEnd)
	{
		*pEnd = 0;
		res = _mkdir(path);
		*pEnd = '\\';
		if(NULL == strstr(pEnd + 1, "\\"))
		{
			if('\0' != *(pEnd+1))
			{
				_mkdir(path);
			}
			break;
		}
		else
		{
			pEnd = strstr(pEnd + 1, "\\");
		}
	}
#else /* posix */
	char* pEnd = strstr(path+1, "/");
	if(NULL == pEnd)
	{
		res = mkdir(path, 0777);
	}
	while(pEnd)
	{
		*pEnd = 0;
		res = mkdir(path, 0777);
		*pEnd = '/';
		if(NULL == strstr(pEnd + 1, "/"))
		{
			if('\0' != *(pEnd+1))
			{
				res = mkdir(path, 0777);
			}
			break;
		}
		else
		{
			pEnd = strstr(pEnd + 1, "/");
		}
	}
#endif /* posix end */
	if(-1 == res) {
		/* error */
        if (errno != EEXIST) {
            _OUTPUT(ERROR1,"QvodMkdir %s error = %d\n",dirname,errno);
        }
		return QVOD_ERROR;
	}

	return QVOD_OK;
}

QVOD_INT64 QvodFileLength(int fd)
{
#ifdef WIN32 /* WIN32 */

	return _filelengthi64(fd);

#else /* posix */

	qvod_stat_t info;
	if(fstat(fd, &info) != 0) {

        _OUTPUT(ERROR1,"%s(%d)-%s: fstat error %d\n", __FILE__, __LINE__, __FUNCTION__,errno);
        return 0;
    }

    return info.st_size;

#endif /* posix end */
}

QVOD_INT64 QvodGetFileLen(const char* filePath)
{
#ifdef WIN32 /* WIN32 */
	return 0;
#else /* posix */
	struct stat thisStat;
	_OUTPUT(INFO,"%s(%d)-%s: start stat %s\n", __FILE__, __LINE__, __FUNCTION__, filePath);
	int res = stat(filePath, &thisStat);
	if(res != 0)
	{
		/* ENOENT, file not found */
		_OUTPUT(ERROR1,"%s(%d)-%s: stat errno = %d.\n", __FILE__, __LINE__, __FUNCTION__, errno);
	}
	else
	{
	
		struct statfs stat;
		res = statfs(g_media_dir.c_str(), &stat);
		if(res != 0)
		{
			_OUTPUT(ERROR1,"%s(%d)-%s: failed to get directory info error =%d\n", __FILE__, __LINE__, __FUNCTION__,errno);
		}
		else
		{
			_OUTPUT(INFO,"%s(%d)-%s: file system type stat.f_type = 0x%08x\n", __FILE__, __LINE__, __FUNCTION__, stat.f_type);
			if(0xEF53 == stat.f_type || 0x65735546 == stat.f_type)//ext2,ext3 and fuseblk 
			{
				//QVOD_DEBUG("%s(%d)-%s: file system type is ext2 or ext3\n", __FILE__, __LINE__, __FUNCTION__);
				return (QVOD_INT64)thisStat.st_blocks*512;
			}
			return (QVOD_INT64)thisStat.st_size;
		}
	}
	return 0;
#endif
}

#ifdef MACOSX
int QvodGetHomeDir(char* name, int size)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    NSString* str = NSHomeDirectory(); 
	strcpy(name, [str cStringUsingEncoding:NSUTF8StringEncoding]);
	int len = strlen(name);
	if (name[len-1] != '/')
	{
		strcat(name, "/");
	}
    [pool release];
    return strlen(name);
}

int QvodGetOldRootDir(char* name, int size)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    NSString* str = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) lastObject]; 
	strcpy(name, [str cStringUsingEncoding:NSUTF8StringEncoding]);
	int len = strlen(name);
	if (name[len-1] != '/')
	{
		strcat(name, "/");
	}
	strcat(name, "QvodPlayer/");
    [pool release];
    return strlen(name);
}

int QvodGetDocumentDir(char* name, int size)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    NSString* str = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject]; 
	strcpy(name, [str cStringUsingEncoding:NSUTF8StringEncoding]);
	int len = strlen(name);
	if (name[len-1] != '/')
	{
		strcat(name, "/");
	}
    [pool release];
    return strlen(name); 
}

QVOD_DWORD QvodGetTempPath(QVOD_DWORD nBufferLength, QVOD_LPTSTR lpBuffer)
{
    int len = 0;
    if(nBufferLength < sizeof(QVOD_POSIX_TEMP_PATH))
    {
        /* error */
        return 0;
    }
    
#ifdef _UNICODE  /* _UNICODE  */
    wmemcpy(lpBuffer, QVOD_POSIX_TEMP_PATH, sizeof(QVOD_POSIX_TEMP_PATH);
            len = wcslen(QVOD_POSIX_TEMP_PATH);
#else /* non _UNICODE  */
            
            //len = QvodGetModuleFileName(NULL, lpBuffer, nBufferLength);
            //strcat(lpBuffer, "Media/");
            //len = strlen(lpBuffer);
            NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
            NSString* tmpDir = NSTemporaryDirectory();
            strcpy(lpBuffer, [tmpDir cStringUsingEncoding:NSUTF8StringEncoding]);
            len = strlen(lpBuffer);
            if('/' != lpBuffer[len-1])
            {
                lpBuffer[len] = '/';
                len++;
            }
            [pool release];
            //memcpy(lpBuffer, QVOD_POSIX_TEMP_PATH, sizeof(QVOD_POSIX_TEMP_PATH));
            //len = strlen(QVOD_POSIX_TEMP_PATH);

#endif /* non  _UNICODE  end */
            
 return len;
 }
#else
QVOD_DWORD QvodGetTempPath(QVOD_DWORD nBufferLength, QVOD_LPTSTR lpBuffer)
{
#ifdef WIN32 /* WIN32 */
    return GetTempPath(nBufferLength, lpBuffer);
    
#else /* posix */
    
    int len = 0;
    if(nBufferLength < sizeof(QVOD_POSIX_TEMP_PATH))
    {
        /* error */
        return 0;
    }
    
#ifdef _UNICODE  /* _UNICODE  */
    wmemcpy(lpBuffer, QVOD_POSIX_TEMP_PATH, sizeof(QVOD_POSIX_TEMP_PATH);
            len = wcslen(QVOD_POSIX_TEMP_PATH);
#else /* non _UNICODE  */
    memcpy(lpBuffer, QVOD_POSIX_TEMP_PATH, sizeof(QVOD_POSIX_TEMP_PATH));
    len = strlen(QVOD_POSIX_TEMP_PATH);
#endif /* non  _UNICODE  end */
            
return len;
            
#endif /* posix end */
}
#endif

int QvodGetModuleFileName(void *module/* ignore */, char *name, int size)
{
	//(void*)module;

#ifdef WIN32 /* WIN32 */

	GetModuleFileName(NULL, name, size);

	return QVOD_OK;
#else /* posix */

	char *res;

	//(void*)module;
	//FILE* stream = fopen("/proc/self/cmdline", "r");
	//fgets(name, size, stream);
	//fclose(stream);
/*	
    NSString* str = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) lastObject]; 
	strcpy(name, [str cStringUsingEncoding:NSUTF8StringEncoding]);
	int len = strlen(name);
	if (name[len-1] != '/')
	{
		strcat(name, "/");
	}
	strcat(name, "QvodPlayer/");
*/
	if (size > g_strCurrentRoot.size()) 
	{
		strcpy(name, g_strCurrentRoot.c_str());
	}
	return strlen(name);
	
	
//	char buf[256] = {0};
//	memset(name, 0, size);
//	sprintf(buf, "/proc/%d/exe", getpid());
//	int len = readlink(buf, name, size);
//	char* pos = strrchr(name, '/');
//	if(pos)
//	{
//		pos++;
//		*pos = '\0';
//		return pos-name;
//	}
//	else
//	{
//		res = getcwd(name, size);
//		if(res == NULL) {
//			/* fail */
//			return QVOD_ERROR;
//		}
//		return strlen(name);
//	}

#endif /* posix end */
}


int QvodMoveFile(QVOD_LPCTSTR lpExistingFileName, QVOD_LPCTSTR lpNewFileName)
{
#ifdef WIN32 /* WIN32 */

	MoveFile(lpExistingFileName, lpNewFileName);

	return QVOD_OK;
#else /* posix */

	int res, res_1;
	FILE *file, *file_new;
	char buf[4096];

	file = fopen(lpExistingFileName, "rb");
	if(file == NULL) {
		/* error */
		return QVOD_ERROR;
	}

	res = access(lpExistingFileName, R_OK);
	if(res == -1) {
		/* error */
		fclose(file);
		return QVOD_ERROR;
	}

	/* copy */
	file_new = fopen(lpNewFileName, "wb");
	if(file_new == NULL) {
		/* error */
		fclose(file);
		return QVOD_ERROR;
	}

	while(true) {

		res = fread(buf, 1, 4096, file);
		if(ferror(file) || 0 == res) {
			/* abnormal */
			if(ferror(file)) {
				/* error */
				fclose(file);
				fclose(file_new);
				remove(lpNewFileName);
				return QVOD_ERROR;
			}

			if(feof(file)) {
				/* EOF */
				res_1 = fwrite(buf, 1, res, file_new);
				if(ferror(file_new)) {
					/* write error */
					fclose(file);
					fclose(file_new);
					return QVOD_ERROR;
				}

				break;
			}
		}

		/* read success */
		res_1 = fwrite(buf, 1, res, file_new);
		if(ferror(file_new)) {
			/* write error */
			fclose(file);
			fclose(file_new);
			return QVOD_ERROR;
		}

	} /* while */

	fclose(file);
	fclose(file_new);

	/* remove old file */
	remove(lpExistingFileName);

	return QVOD_OK;

#endif /* posix end */
}

QVOD_BOOL QvodCopyFile(QVOD_LPCTSTR lpExistingFileName, QVOD_LPCTSTR lpNewFileName, QVOD_BOOL bFailIfExists)
{
#ifdef WIN32 /* WIN32 */

	CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists);

	return QVOD_OK;
#else /* posix */

	int res, res_1;
	FILE *file, *file_new;
	char buf[4096];

	/* first judge if lpNewFileName exist */
	res = access(lpNewFileName, R_OK);
	if(res == 0) {
		/* exist */
		if(bFailIfExists != QVOD_FALSE) {
			/* fail */
			return QVOD_FOUND;
		}
	}

	file = fopen(lpExistingFileName, "rb");
	if(file == NULL) {
		/* error */
		return QVOD_ERROR;
	}

	res = access(lpExistingFileName, R_OK);
	if(res == -1) {
		/* error */
		fclose(file);
		return QVOD_ERROR;
	}

	/* copy */
	file_new = fopen(lpNewFileName, "wb");
	if(file_new == NULL) {
		/* error */
		fclose(file);
		return QVOD_ERROR;
	}

	while(true) {

		res = fread(buf, 4096, 1, file);
		if(res != 1) {
			/* abnormal */
			if(ferror(file)) {
				/* error */
				fclose(file);
				fclose(file_new);
				remove(lpNewFileName);
				return QVOD_ERROR;
			}

			if(feof(file)) {
				/* EOF */

				res_1 = fwrite(buf, res, 1, file_new);
				if(ferror(file_new)) {
					/* write error */
					fclose(file);
					fclose(file_new);
					return QVOD_ERROR;
				}

				break;
			}
		}

		/* read success */
		res_1 = fwrite(buf, res, 1, file_new);
		if(ferror(file_new)) {
			/* write error */
			fclose(file);
			fclose(file_new);
			return QVOD_ERROR;
		}

	} /* while */

	fclose(file);
	fclose(file_new);

	return QVOD_OK;

#endif /* posix end */
}


/* file mapping op */
char* QvodCreateFileMapping(const char *file_loc, unsigned long file_size)
{
	char *pFileData;

#ifdef WIN32 /* WIN32 */

	/* create file */
	HANDLE hFile;
	hFile = CreateFile(file_loc,
					   GENERIC_WRITE|GENERIC_READ,
					   FILE_SHARE_READ|FILE_SHARE_WRITE,
					   NULL,
					   OPEN_ALWAYS,
					   FILE_FLAG_SEQUENTIAL_SCAN,
					   NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
		return (char*)QVOD_ERROR;
	}

	/* create mapping */
	HANDLE hMapping;
	hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, file_size, NULL);
	if(hMapping == NULL) {
		return (char*)QVOD_ERROR;
	}

	/* file mapping */
	pFileData = (char*)MapViewOfFile(hMapping,
									 FILE_MAP_READ|FILE_MAP_WRITE,
									 0,
									 0,
									 0);

	/* close */
	CloseHandle(hMapping);
	CloseHandle(hFile);

#else /* posix */

	/* open file in fd */
	int fd;
	fd = open(file_loc, O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
	if(fd == -1) {
		/* error */
		_OUTPUT(ERROR1,"open error: %d\n", errno);
		return (char*)QVOD_ERROR;
	}

	/*
	 * we ftruncate it for avoid mmapping fd with a empty file,
	 * otherwise "bus error" will ocurr.
	 */
	int res = ftruncate(fd, file_size);
	if(res < 0) {
		_OUTPUT(ERROR1,"ftruncate error = %d\n",errno);
		close(fd);
		return (char*)QVOD_ERROR;
	}

	/* mmap */
	pFileData = (char*)mmap(NULL, file_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(pFileData == (char*)-1) {
		/* error */
		_OUTPUT(ERROR1,"mmap error!!!!!\n");
		close(fd);
		return (char*)QVOD_ERROR;
	}
	//QVOD_DEBUG("pFileData after mmap: %x\n", pFileData);

	/*
	 * Closing the file descriptor does not unmap the region.
	 */
	close(fd);

#endif /* posix end */

	return pFileData;
}

int QvodDestroyFileMapping(void *addr, size_t length)
{
#ifdef WIN32

	/* ignore length */
	UnmapViewOfFile(addr);

#else /* posix */

	/* length must be equal to the length when mmap! */
	munmap(addr, length);

#endif

	return QVOD_OK;
}



#if 0

/* DEC2 and ENC2 */

//0< x <32
const char key[32] = {12,29,3,7,15,17,24,9,2,
					  2,11,8,31,5,20,16,1,28,
					  27,30,21,18,13,10,23,22,
					  14,4,6,19,25,26};

void Enc2(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key2)
{
	int i,j,n = 0;
	unsigned int k;

	memcpy(pDes,pSrc,len);
	QVOD_BYTE* pchr = pDes;
	QVOD_UINT* puint = (QVOD_UINT*)pchr;
	k = key2;
	for(i = 0;i<len/4;i++)
	{
		puint[i] = (puint[i]>>key[n]) + (puint[i]<<(32-key[n]));
		puint[i] ^= k;
		k = puint[i];
		n = pchr[3+i*4]%32;
	}

	QVOD_BYTE* pkey2 = (QVOD_BYTE*)&key2;
	n%=8;
	for(j = i*4;j<len;j++)
	{
		pchr[j] = (pchr[j] >> n) + (pchr[j] << (8-n));
		pchr[j] ^= *pkey2++;
		n = pchr[j]%8;
	}
}

void Dec2(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key2)
{

	int i,j,nn,n = 0;
	QVOD_BYTE* pchr = pDes;
	memcpy(pchr,pSrc,len);
	QVOD_UINT k,nk;
	k = key2;

	QVOD_UINT* puint = (QVOD_UINT*)pchr;
	for(i = 0;i<len/4;i++)
	{
		nn = pchr[3+i*4]%32;
		nk = puint[i];
		puint[i] ^= k;
		puint[i] = (puint[i]<<key[n]) + (puint[i]>>(32-key[n]));
		n = nn;
		k = nk;
	}
	QVOD_BYTE* pkey2 = (QVOD_BYTE*)&key2;
	n%=8;
	for(j = i*4;j<len;j++)
	{
		nn = pchr[j]%8;
		pchr[j] ^= *pkey2++;
		pchr[j] = (pchr[j] << n) + (pchr[j] >> (8-n));
		n = nn;
	}
}

#endif /* #if 0 */

