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

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static void fs_fullpath (char fpath[PATH_MAX], const char *path) {
	strcpy(fpath, CONTEXT->root_dir);
  strncat(fpath, path, PATH_MAX);

  log_msg(
		"`fs_fullpath` rootdir = \"%s\", path = \"%s\", fpath = \"%s\"\n",
		CONTEXT->root_dir, path, fpath
	);
}


/******************************
 * FUSE API IMPLEMENTATIONS
 ******************************/

int fs_readdir (
	const char* path,
	void* buf,
	fuse_fill_dir_t filler,
	off_t offset,
	struct fuse_file_info* info,
	__attribute__ ((unused)) enum fuse_readdir_flags flags
) {
	DIR* dd;
	struct dirent* dir_entry;
	int ret = 0;

	log_msg(
		"`fs_readdir` path=\"%s\" | buf=0x%08x | filler=0x%08x | offset=%lld | info=0x%08x",
		path, buf, filler, offset, info
	);

	dd = (DIR*)(uintptr_t) info->fh;

	// ea directory contains at least two entries, . and ..
	// this, if the first call to `readdir` returns NULL, we've an error
	dir_entry = readdir(dd);

	log_msg("`readdir` returned 0x%p\n", dir_entry);

	if (dir_entry == 0) {
		log_msg("readdir");

		ret = -errno;
		return ret;
	}

	// copy the entire directory into the buffer
	// we'll exit the loop when either `readdir` returns NULL (we've read the entire directory),
	// or `filler` yields a non-zero return code (the buffer is full)
	do {
		log_msg("invoking `filler` with entry %s\n", dir_entry->d_name);

		if (filler(buf, dir_entry->d_name, NULL, 0, 0) != 0) {
			log_msg("`fs_readdir` `filler` -  buffer full");

			return -ENOMEM;
		}
	} while ((dir_entry = readdir(dd)) != NULL);

	return ret;
}

void *fs_init (
	__attribute__ ((unused)) struct fuse_conn_info* conn,
	struct fuse_config* conf
) {
	log_msg("`fs_init` invoked - setting kernel cache flag; file contents cache flushing on every open is now disabled");

	conf->kernel_cache = 1;

	return CONTEXT;
}

int fs_getattr (
	const char *path,
	struct stat* stbuf,
	__attribute__ ((unused)) struct fuse_file_info* info
) {
	log_msg("`fs_getattr` invoked");

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

int fs_open (const char* path, struct fuse_file_info* info) {
  int ret = 0;
  int fd;
  char f_path[PATH_MAX];

  log_msg(
		"`fs_open` - path\"%s\", info=0x%08x)\n",
	  path, info
	);

	fs_fullpath(f_path, path);

  fd = open(f_path, info->flags);

  if (fd < 0) {
		log_msg("`fs_open` - error opening file");
		ret = -errno;
	}

  info->fh = fd;

  return ret;
}

int fs_mknod (const char* path, mode_t mode, dev_t dev) {
  int ret;
  char f_path[PATH_MAX];

  log_msg(
		"`fs_mknod` path=\"%s\", mode=0%3o, dev=%lld",
	  path, mode, dev
	);

	fs_fullpath(f_path, path);

  if (S_ISREG(mode)) {
		if ((ret = open(f_path, O_CREAT | O_EXCL | O_WRONLY, mode) >= 0)) {
			ret = close(ret);
		}
  } else {
		if (S_ISFIFO(mode)) {
			ret = mkfifo(f_path, mode);
		} else {
			ret = mknod(f_path, mode, dev);
		}
	}

	return ret;
}

int fs_mkdir (const char* path, mode_t mode) {
  char f_path[PATH_MAX];

  log_msg(
		"`fs_mkdir` path=\"%s\", mode=0%3o",
	  path, mode
	);

  fs_fullpath(f_path, path);

  return mkdir(f_path, mode);
}

int fs_rmdir (const char* path) {
	char f_path[PATH_MAX];

	log_msg(
		"`fs_rmdir` path=\"%s\"",
		path
	);

	fs_fullpath(f_path, path);

	return rmdir(f_path);
}

struct fuse_operations fs_ops = {
	.init = fs_init,
	.getattr = fs_getattr,
	.open = fs_open,
	.mknod = fs_mknod,
	.mkdir = fs_mkdir,
};


/******************************
 * Initialization
 ******************************/

int main (int argc, char* argv[]) {
	if (is_root_user()) {
		die("fs cannot be mounted by the root user");
	}

	int fuse_stat;

	fs_state* fs_context = malloc(sizeof(fs_state));

  if (!fs_context) {
		panic("malloc");
	}

	// TODO validate
	fs_context->root_dir = realpath("/", NULL);
	// fs_context->root_dir = realpath(argv[argc-2], NULL);

	// argv[argc-2] = argv[argc-1];
	// argv[argc-1] = NULL;
	// argc--;

  // fs_context->log_file = log_init_transport();

	fuse_stat = fuse_main(argc, argv, &fs_ops, fs_context);

	log_msg("Fuse library version %d.%d\n", FUSE_MAJOR_VERSION, FUSE_MINOR_VERSION);
	log_msg("`fuse_main` invoked - fs context has been initialized");

	return fuse_stat;
}
