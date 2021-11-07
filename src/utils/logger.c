#include "logger.h"
#include "error.h"

#include <stdarg.h>

static const char* log_file = "fs.log";

char* now (void) {
	static char buffer[64];

	time_t raw_time;
	struct tm* time_info;

	time(&raw_time);
	time_info = localtime(&raw_time);

	strftime(buffer, 64, TIME_FORMAT, time_info);

	return buffer;
}

/**
 * @brief Initialize a file transport for the logger
 *
 * @return FILE*
 */
FILE* log_init_transport() {
	FILE* fd;

	if (!(fd = fopen(log_file, "a"))) {
		panic("`log_init_transport` log file initialization");
	}

	// set the stream to line buffering
	setvbuf(fd, NULL, _IOLBF, 0);

	return fd;
}

void log_msg(const char* fmt, ...) {
	char buffer[2048];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buffer, 2048, fmt, ap);
	va_end(ap);

	// fprintf(CONTEXT->log_file, "[%s] %s\n", now(), buffer);
	FILE* fd = log_init_transport();
	fprintf(fd, "[%s] %s", now(), buffer);

	fclose(fd);
}
