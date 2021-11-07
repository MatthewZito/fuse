#ifndef FS_H
#define FS_H

// this flag will ensure gcc includes the `stat` file modes e.g. `S_IFDIR`
#define _XOPEN_SOURCE 700

// the version of the FUSE API we're using
#define FUSE_USE_VERSION 31

#include <stdio.h>

// set log level
#ifdef DEBUG
#define LOG_LEVEL 0x03
#endif

typedef struct fs_state {
	FILE* log_file;
	char* root_dir;
} fs_state;

#define CONTEXT ((struct fs_state*) fuse_get_context()->private_data)

void log_msg(const char* fmt, ...);

#endif /* FS_H */
