/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  All rights reserved.
**
**  append.c - libtar code to append files to a tar archive
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#include <internal.h>

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>

#ifdef STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef _WIN_
# pragma warning (disable:4189)
#endif

struct tar_dev
{
	dev_t td_dev;
	libtar_hash_t *td_h;
};
typedef struct tar_dev tar_dev_t;

struct tar_ino
{
	ino_t ti_ino;
	char ti_name[MAXPATHLEN];
};
typedef struct tar_ino tar_ino_t;


/* free memory associated with a tar_dev_t */
void
tar_dev_free(tar_dev_t *tdp)
{
	libtar_hash_free(tdp->td_h, free);
	free(tdp);
}


/* appends a file to the tar archive */
int
tar_append_file(TAR *t, char *realname, char *savename, PACK_OPERATION_CHECKQUIT_CALLBACK pFunc, void* pParam )
{
	struct stat s;
#ifndef _WIN32
	int i;
	char path[MAXPATHLEN];
#endif // _WIN32
	libtar_hashptr_t hp;
	tar_dev_t *td = NULL;
	tar_ino_t *ti = NULL;

#ifdef DEBUG
	printf("==> tar_append_file(TAR=0x%lx (\"%s\"), realname=\"%s\", "
	       "savename=\"%s\")\n", t, t->pathname, realname,
	       (savename ? savename : "[NULL]"));
#endif

	if (lstat(realname, &s) != 0)
	{
#ifdef DEBUG
		perror("lstat()");
#endif
		return -1;
	}

	/* set header block */
#ifdef DEBUG
	puts("    tar_append_file(): setting header block...");
#endif
	memset(&(t->th_buf), 0, sizeof(struct tar_header));
	th_set_from_stat(t, &s);

	/* set the header path */
#ifdef DEBUG
	puts("    tar_append_file(): setting header path...");
#endif
	th_set_path(t, (savename ? savename : realname));

	/* check if it's a hardlink */
#ifdef DEBUG
	puts("    tar_append_file(): checking inode cache for hardlink...");
#endif
	libtar_hashptr_reset(&hp);
	if (libtar_hash_getkey(t->h, &hp, &(s.st_dev),
			       (libtar_matchfunc_t)dev_match) != 0)
		td = (tar_dev_t *)libtar_hashptr_data(&hp);
	else
	{
#ifdef DEBUG
		printf("+++ adding hash for device (0x%lx, 0x%lx)...\n",
		       major(s.st_dev), minor(s.st_dev));
#endif
		td = (tar_dev_t *)calloc(1, sizeof(tar_dev_t));
		td->td_dev = s.st_dev;
		td->td_h = libtar_hash_new(256, (libtar_hashfunc_t)ino_hash);
		if (td->td_h == NULL)
			return -1;
		if (libtar_hash_add(t->h, td) == -1)
			return -1;
	}
#ifndef _WIN32 // modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
	libtar_hashptr_reset(&hp);
	if (libtar_hash_getkey(td->td_h, &hp, &(s.st_ino),
			       (libtar_matchfunc_t)ino_match) != 0)
	{
		ti = (tar_ino_t *)libtar_hashptr_data(&hp);
#ifdef DEBUG
		printf("    tar_append_file(): encoding hard link \"%s\" "
		       "to \"%s\"...\n", realname, ti->ti_name);
#endif
		t->th_buf.typeflag = LNKTYPE;
		th_set_link(t, ti->ti_name);
	}
	else
	{
#ifdef DEBUG
		printf("+++ adding entry: device (0x%lx,0x%lx), inode %ld "
		       "(\"%s\")...\n", major(s.st_dev), minor(s.st_dev),
		       s.st_ino, realname);
#endif
		ti = (tar_ino_t *)calloc(1, sizeof(tar_ino_t));
		if (ti == NULL)
			return -1;
		ti->ti_ino = s.st_ino;
		snprintf(ti->ti_name, sizeof(ti->ti_name), "%s",
			 savename ? savename : realname);
		libtar_hash_add(td->td_h, ti);
	}
#endif /* #ifndef _WIN32 */

#ifndef _WIN32 // modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
	/* check if it's a symlink */
	if (TH_ISSYM(t))
	{
		i = readlink(realname, path, sizeof(path));
		if (i == -1)
			return -1;
		if (i >= MAXPATHLEN)
			i = MAXPATHLEN - 1;
		path[i] = '\0';
#ifdef DEBUG
		printf("    tar_append_file(): encoding symlink \"%s\" -> "
		       "\"%s\"...\n", realname, path);
#endif
		th_set_link(t, path);
	}
#endif /* #ifndef _WIN32 */

	/* print file info */
	if (t->options & TAR_VERBOSE)
		th_print_long_ls(t);

#ifdef DEBUG
	puts("    tar_append_file(): writing header");
#endif
	/* write header */
	if (th_write(t) != 0)
	{
#ifdef DEBUG
		printf("t->fd = %p\n", t->fd);
#endif
		return -1;
	}
#ifdef DEBUG
	puts("    tar_append_file(): back from th_write()");
#endif

	/* if it's a regular file, write the contents as well */
	if (TH_ISREG(t) && tar_append_regfile(t, realname, pFunc, pParam ) != 0)
		return -1;

	return 0;
}


/* write EOF indicator */
int
tar_append_eof(TAR *t)
{
	int i, j;
	char block[T_BLOCKSIZE];

	memset(&block, 0, T_BLOCKSIZE);
	for (j = 0; j < 2; j++)
	{
		i = tar_block_write(t, &block);
		if (i != T_BLOCKSIZE)
		{
			if (i != -1)
				errno = EINVAL;
			return -1;
		}
	}

	return 0;
}

/* add file contents to a tarchive */
int
tar_append_regfile(TAR *t, char *realname, PACK_OPERATION_CHECKQUIT_CALLBACK pFunc, void* pParam )
{
	char block[T_BLOCKSIZE];
	int filefd;
	int i, j;
	int size;

	// modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
	//filefd = open(realname, O_RDONLY);
	filefd = open(realname, O_RDONLY
#ifdef O_BINARY
			| O_BINARY
#endif
			);

	if (filefd == -1)
	{
#ifdef DEBUG
		perror("open()");
#endif
		return -1;
	}

	size = th_get_size(t);
	for (i = size; i > T_BLOCKSIZE; i -= T_BLOCKSIZE)
	{
		if( pFunc && ( pFunc( pParam ) > 0 ) )
			goto ERROR_EXIT;

		j = read(filefd, &block, T_BLOCKSIZE);
		if (j != T_BLOCKSIZE)
		{
			if (j != -1)
				errno = EINVAL;
			goto ERROR_EXIT;
		}
		if (tar_block_write(t, &block) == -1)
			goto ERROR_EXIT;
	}

	if (i > 0)
	{
		j = read(filefd, &block, i);
		if (j == -1)
			goto ERROR_EXIT;

		memset(&(block[i]), 0, T_BLOCKSIZE - i);

		if (tar_block_write(t, &block) == -1)
			goto ERROR_EXIT;
	}

	close(filefd);
	return 0;

ERROR_EXIT:
	close( filefd );
	return -1;
}


