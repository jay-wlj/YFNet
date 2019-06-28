
/*
 * QVOD file.
 */

#ifndef _QVOD_FILE_H_
#define _QVOD_FILE_H_


#include "qvod_string.h"
#include <vector>
using std::vector;

#ifdef WIN32 /* WIN32 */

#include <windows.h>
#include <io.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>

#define QVOD_O_RDONLY 	_O_RDONLY
#define	QVOD_O_RDWR 	_O_RDWR
#define	QVOD_O_BINARY 	_O_BINARY
#define	QVOD_S_IREAD 	_S_IREAD
#define	QVOD_S_IWRITE 	_S_IWRITE
#define QVOD_S_IRUSR	_S_IRUSR
#define QVOD_S_IWUSR	_S_IWUSR
#define	QVOD_O_CREAT 	_O_CREAT

#define QVOD_ACCESS 	_access
#define QVOD_STAT		_stat
#define QVOD_STAT64		_stat64
#define QVOD_OPEN		_open
#define QVOD_READ		_read
#define QVOD_WRITE		_write
#define QVOD_CLOSE		_close
#define QVOD_SEEK		fseek
#define QVOD_LSEEK		_lseeki64
#define QVOD_FDFLUSH 	_commit
#define QVOD_SNPRINTF 	_snprintf
#define QVOD_CHDIR		_chdir
#define QVOD_REMOVE		remove
#define QVOD_RENAME		rename

typedef	struct _stat qvod_stat_t;
typedef struct _stat64 qvod_stat64_t;
typedef	HANDLE		 qvod_file_t;


#else /* posix */


#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
//#include <sys/vfs.h>
#include <sys/stat.h>

#ifdef MACOSX
#include <sys/stat.h>
#else
#include <sys/statfs.h>
#endif


#define QVOD_O_RDONLY 	O_RDONLY
#define	QVOD_O_RDWR 	O_RDWR
#define	QVOD_O_BINARY 	O_LARGEFILE
#define	QVOD_S_IREAD 	S_IRUSR 
#define	QVOD_S_IWRITE 	S_IWRITE
#define QVOD_S_IRUSR	S_IRUSR
#define QVOD_S_IWUSR	S_IWUSR
#define	QVOD_O_CREAT 	O_CREAT

#define QVOD_ACCESS 	access
#define QVOD_STAT		stat
#define QVOD_OPEN		open
#define QVOD_READ		read
#define QVOD_WRITE		write
#define QVOD_CLOSE		close
#define QVOD_SEEK		fseeko
#define QVOD_LSEEK		lseek
#define QVOD_FDFLUSH 	fsync
#define QVOD_SNPRINTF 	snprintf
#define QVOD_CHDIR		chdir
#define QVOD_REMOVE		remove
#define QVOD_RENAME		rename

typedef struct stat qvod_stat_t;
typedef struct stat64 qvod_stat64_t;
typedef	int			qvod_file_t;

#endif /* posix end */


/* file op */

int QvodMkdir(const char *dirname);
int QvodRemoveFolder(const char *pathname);
int QvodRemoveFile(const char *pathname);
int QvodGetFilesInFolder(const char *pathname, vector<string>& vFileNames);			// 获取目录下的文件名
QVOD_INT64 QvodFileLength(int fd);
QVOD_INT64 QvodGetFileLen(const char* filePath);
//int QvodGetHomeDir(char* name, int size);
//int QvodGetOldRootDir(char* name, int size);
//int QvodGetDocumentDir(char* name, int size);
int QvodGetModuleFileName(void *module/* ignore */, char* name, int size);
QVOD_BOOL QvodCopyFile(QVOD_LPCTSTR lpExistingFileName, QVOD_LPCTSTR lpNewFileName, QVOD_BOOL bFailIfExists);
int QvodMoveFile(QVOD_LPCTSTR lpExistingFileName, QVOD_LPCTSTR lpNewFileName);
QVOD_DWORD QvodGetTempPath(QVOD_DWORD nBufferLength, QVOD_LPTSTR lpBuffer);


/* file mapping op */
char* QvodCreateFileMapping(const char *file_loc, unsigned long file_size);
int QvodDestroyFileMapping(void *addr, size_t length);

/* DEC2 and ENC2 */
void Enc2(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key2);
void Dec2(const QVOD_BYTE* pSrc,QVOD_BYTE* pDes,int len,unsigned int key2);





#ifdef C2_EMBEDED /* C2_EMBEDED */

#include <linux/unistd.h>
//#ifdef __NR_fallocate
//#undef __NR_fallocate
//#endif
//#define __NR_fallocate      (__NR_Linux + 287)
#define __NR___fallocate  __NR_fallocate
#define __NR___time  __NR_time
#define __NR___gettimeofday __NR_gettimeofday

typedef int ssize_t;
typedef long int int32;
typedef unsigned long int u32;
typedef unsigned long long int uint64;

//static inline _syscall6(ssize_t, __fallocate, int, fd, int, mode, off_t, high_offset, off_t, low_offset, off_t, high_len, off_t, low_len);
static inline _syscall6(ssize_t, __fallocate, int, fd, int, mode, u32, high_offset, u32, low_offset, u32, high_len, u32, low_len);
//static inline _syscall1(ssize_t,__eventfd, unsigned int, count);
static inline _syscall1(long, __time, time_t __user *, tloc);
static inline _syscall2(long, __gettimeofday, struct timeval __user *, tv,struct timezone __user *, tz);

inline int32 c2_fat_fs_enlarge_file(u32 file_id, uint64 expect_filesize)
{
	//expect_filesize = 1024;
	fprintf(stderr, "c2_fat_fs_enlarge_file: %d %lld\n", file_id, expect_filesize);
	int ret = __fallocate(file_id, 0, 0, 0, (off_t)(expect_filesize>>32), (off_t)(expect_filesize & 0xffffffff));
	//int ret = __fallocate(file_id, 0, 0, 0, 0,(u32)(1024));
	printf("__fallocate return: ret = %d\n", ret);
	if(ret < 0)
	{
		printf("SYSCALL: received error %d, ret=%d\n", errno, ret);
		//fallback to normal way.
		ret = ftruncate64(file_id, expect_filesize);
		fprintf(stderr, "ftruncate64 return %d\n", ret);
	}

	return ret;
}

#endif /* C2_EMBEDED end */





#endif /* _QVOD_FILE_H_ */


