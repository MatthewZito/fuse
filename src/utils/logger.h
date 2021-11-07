#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>
#include <stdio.h>
#include <time.h>

#define TIME_FORMAT "%Y-%m-%d %H:%M:%S"

char* now (void);

FILE* log_init_transport();

char* fmt_str(char* fmt, char* arg);

void log_msg(const char* fmt, ...);

#endif /* LOGGER_H */
