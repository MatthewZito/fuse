/**
 * @file fuse.c
 * @author Matthew Zito  (exbotanical@protonmail.com)
 * @brief A high-level FUSE file system
 * @version 0.1
 * @date 2021-11-06
 *
 * @copyright Copyright (c) Matthew Zito 2021
 *
 */

#include "fs.h"
#include "utils/logger.h"
#include "utils/guards.h"
#include "utils/error.h"

#include <fuse3/fuse.h>

#include <stdlib.h>

int fs_getattr (const char *path, struct stat* stbuf, struct fuse_file_info *fi) {
	LOG_DEBUG("`fs_getattr` invoked");

	int ret = 0;

	// here, we fill the `stat` struct with various attributes about the file at the given path

	// user id of the file's owner
	// by default, the owner of the file is the user who mounted the filesystem
	stbuf->st_uid = getuid();
	// group id of the file
	// by default, the group of the file is the same as that of the user who mounted the filesystem
	stbuf->st_gid = getgid();
	// time file was last accessed
	stbuf->st_atime = time(NULL);
	// time file was last modified
	stbuf->st_mtime = time(NULL);

	if (strcmp(path, "/") == 0) {
		// file type, permissions
		stbuf->st_mode = S_IFDIR | 0755;

		// set hard links - this tracks how many dirs have entries for this file, sans symlinks
		// if this attribute reaches 0, the file is discarded as soon as no process is using it
		// https://unix.stackexchange.com/questions/101515/why-does-a-new-directory-have-a-hard-link-count-of-2-before-anything-is-added-to/101536#101536
		stbuf->st_nlink = 2;
	} else {
		stbuf->st_mode = S_IFREG | 0644;
		stbuf->st_nlink = 1;

		// file size, in bytes
		// for symlinks, this specifies the length of the filename to which the link refers
		stbuf->st_size = 1024;
	}

	return ret;
}

struct fuse_operations fs_ops = {
	.getattr = fs_getattr,
};

int main (int argc, char* argv[]) {
	if (is_root_user()) {
		panic("fs cannot be mounted by the root user");
	}

	int fuse_stat;

	fs_state* fs_context = malloc(sizeof(fs_state));

  if (fs_context == NULL) {
		LOG_ERROR("failed to allocate memory for FUSE state");
		panic("memory allocation failure - fs_context");
	}

  // fs_context->logfile = log_open();

	fprintf(stderr, "Fuse library version %d.%d\n", FUSE_MAJOR_VERSION, FUSE_MINOR_VERSION);

	fuse_stat = fuse_main(argc, argv, &fs_ops, fs_context);

	LOG_DEBUG("fuse_main invoked");

	return fuse_stat;
}
