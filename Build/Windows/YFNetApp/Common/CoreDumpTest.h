#ifndef COMMON_COREDUMPTEST_H
#define COMMON_COREDUMPTEST_H

#ifdef  COREDUMP_TEST_DEBUG
#define COREDUMP_TEST do{int *p = NULL; *p = NULL;}while(false);
#else
#define COREDUMP_TEST
#endif

#endif