#ifndef FS_H
#define FS_H

#include <stdio.h>

// the version of the FUSE API we're using
#define FUSE_USE_VERSION 31

#define CONTEXT ((struct fs_state *) fuse_get_context()->private_data)

// set log level
#ifdef DEBUG
#define LOG_LEVEL 3
#endif

typedef struct {
	FILE* log_file;
	char* root_dir;
} fs_state;


#endif /* FS_H */
