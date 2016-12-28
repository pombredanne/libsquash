/*
 * Copyright (c) 2016-2017 Minqi Pan and Shengyuan Liu
 *
 * This file is part of libsquash, distributed under the MIT License
 * For full terms see the included LICENSE file
 */

#include "squash.h"
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

ssize_t squash_readlink(sqfs_err *error, sqfs *fs, const char *path, char *buf, size_t bufsize)
{
	if(!error)
		return -1;

	if(!buf || !path || !fs){
		*error = SQFS_ERR;
		return -1;
	}


	sqfs_inode node;
	bzero(&node, sizeof(sqfs_inode));
	bzero(buf, bufsize);

	bool found = false;

	*error = sqfs_inode_get(fs, &node, sqfs_inode_root(fs));
	if(SQFS_OK != *error)
		return -1;
	*error = sqfs_lookup_path(fs, &node, path, &found);
	if(SQFS_OK != *error)
		return -1;

	if(found){
		size_t want = 0;
		sqfs_md_cursor cur;

		if (!S_ISLNK(node.base.mode))
			return SQFS_ERR;

		want = node.xtra.symlink_size;

		if (want > bufsize - 1){
			*error = SQFS_ERR;
			return -1;//bufsize is too small
		}
		cur = node.next;
		*error = sqfs_md_read(fs, &cur, buf, want);
		if(SQFS_OK != *error)
			return -1;
		buf[want] = '\0';
		return want + 1;
	}
	else{
		*error = SQFS_NOENT;
		return -1;
	}
}
