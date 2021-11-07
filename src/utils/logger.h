#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>
#include <stdio.h>
#include <time.h>

#define ERROR_LEVEL 0x01
#define INFO_LEVEL 0x02
#define DEBUG_LEVEL 0x03

#ifndef LOG_LEVEL
#define LOG_LEVEL DEBUG_LEVEL
#endif

#define TIME_FORMAT "%Y-%m-%d %H:%M:%S"
#define LOG_FORMAT "%s | %-7s | %-15s | %s:%d | "

#define ERROR_TAG "ERROR"
#define INFO_TAG "INFO"
#define DEBUG_TAG "DEBUG"

#define LOG_ARGS(LOG_TAG) now(), LOG_TAG, __FILE__, __func__, __LINE__
#define PRINT(format, ...) fprintf(stderr, format, __VA_ARGS__)

#if LOG_LEVEL >= DEBUG_LEVEL
#define LOG_DEBUG(message) PRINT(LOG_FORMAT message "\n", LOG_ARGS(DEBUG_TAG))
#else
#define LOG_DEBUG(...)
#endif

#if LOG_LEVEL >= INFO_LEVEL
#define LOG_INFO(message) PRINT(LOG_FORMAT message "\n", LOG_ARGS(INFO_TAG))
#else
#define LOG_INFO(...)
#endif

#if LOG_LEVEL >= ERROR_LEVEL
#define LOG_ERROR(message) PRINT(LOG_FORMAT message "\n", LOG_ARGS(ERROR_TAG))
#else
#define LOG_ERROR(...)
#endif

// #define LOG_ERROR(message, ...) PRINT(LOG_FORMAT message "\n", LOG_ARGS(ERROR_TAG), ## args)

static inline char* now () {
	static char buffer[64];
	time_t raw_time;
	struct tm* time_info;

	time(&raw_time);
	time_info = localtime(&raw_time);

	strftime(buffer, 64, TIME_FORMAT, time_info);

	return buffer;
}

#endif /* LOGGER_H */
