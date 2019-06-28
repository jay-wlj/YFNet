#pragma once

//nType		弹出类型
//nPID			进程id，播放器类时传入
//nTime		nTime秒后若满足条件弹出
//pParam	传入参数,如： -mode=flash
typedef int (_cdecl  *StartQvodTips)(int nType,int nPID,TCHAR * pParam);