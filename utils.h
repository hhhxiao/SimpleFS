#ifndef SIMFS_UTILS_H
#define SIMFS_UTILS_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#define DEBUG 1
#ifdef WIN32
#define __FILENAME__ (__builtin_strrchr(__FILE__, '\\') ? __builtin_strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#define LOG(...) log_info(__FILENAME__,__FUNCTION__,__LINE__,__VA_ARGS__)
#define  ERR(...) fprintf(stderr,__VA_ARGS__)
#define  INFO(...) fprintf(stdout,__VA_ARGS__)
#define true  1
#define false 0
#define bool int
#define byte_t uint8_t
#define B_SIZE sizeof(byte)


void log_info(const char *file_name, const char *function_name, size_t line, const char *fmt, ...);


#endif