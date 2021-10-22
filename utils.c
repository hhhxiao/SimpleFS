#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "utils.h"

void log_info(const char *file_name, const char *function_name, size_t line, const char *fmt, ...) {
#ifdef  DEBUG
    va_list args;
    va_start(args, fmt);
    printf("<%s> [%s: %d]  ", file_name, function_name, line);
    vprintf(fmt, args);
    printf("\n");
    fflush(stdout);
#endif
}
