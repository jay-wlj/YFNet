#pragma once
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif
#define WIN32_LEAN_AND_MEAN   
#ifndef WINVER               
#define WINVER 0x0400           
#endif

#ifndef _WIN32_WINNT            
#define _WIN32_WINNT 0x0500    
#endif

#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
using namespace std;



