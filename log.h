#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#define LOG_DEBUG(format, ...) fprintf(stdout, (format"\n"), ##__VA_ARGS__)
#define LOG_WARN(format, ...)  fprintf(stdout, (format"\n"), ##__VA_ARGS__)
#define LOG_ERROR(format, ...) fprintf(stdout, (format"\n"), ##__VA_ARGS__)

#endif
