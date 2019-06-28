#ifndef __QVOD_HASH_H__
#define __QVOD_HASH_H__

#define QVOD_ERROR_SUCCESS			 0			//成功
#define QVOD_ERROR_INVALID_PARA		-1			//函数参数错误
#define QVOD_ERROR_FILE_NOT_EXIST	-2			//文件不存在
#define QVOD_ERROR_FILE_OPEN_FAIL	-3			//文件打开失败
#define QVOD_ERROR_FILE_LEN_ZERO	-4			//文件长度为0
#define QVOD_ERROR_INTERNAL_FAIL	-5			//程序内部调用失败

//hash计算进度的回调函数
//wpFilePath:文件路径
//iProgress: 计算hash的进度,如果55%,则返回55;
//bError   : 是否发生错误
typedef void (*QVODCALCHASHPROC)(wchar_t *wpFilePath,int iProgress,bool bError);

//hash计算成功的回调函数
//wpFilePath: 文件路径
//szHash[41]: 文件的hash
typedef void (*QVODCALCHASHRESULT)(wchar_t *wpFilePath,char szHash[41]);

//开始计算文件的hash,返回任务的ID号
//wpFilePath: 文件路径
//pCalcProc : 计算进度的回调函数
//pGetResult: 计算结束后获取结果的回调函数
//bFast		: 默认为false;单个线程计算hash;若为true,则根据CPU的数量快速的进行hash的计算;
int StartCalcHash(wchar_t *wpFilePath,QVODCALCHASHPROC pCalcProc,QVODCALCHASHRESULT pGetResult,bool bFast = false);


//终止hash的计算;
//iTaskID: StartCalcHash返回的任务ID号
//该函数还有释放内存的功能
void StopCalcHash(int iTaskID);
#endif