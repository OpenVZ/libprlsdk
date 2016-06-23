/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  All rights reserved.
**
**  handle.c - libtar code for initializing a TAR handle
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#include <internal.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#ifndef _WIN_
	#ifdef HAVE_UNISTD_H
	#include <unistd.h>
	#endif
#endif

#ifdef STDC_HEADERS
# include <string.h>
# include <stdlib.h>
#endif

const char libtar_version[] = PACKAGE_VERSION;

static tarfd_t
default_openfunc(const char * name, int flags, int mode)
{
	int fd = open(name, flags, mode);
	if (-1 == fd)
		return TAR_INVALID_FD;

	return (tarfd_t)(intptr_t)fd;
}

static int
default_closefunc(tarfd_t fd)
{
	return close((int)(intptr_t)fd);
}

static ssize_t
default_readfunc(tarfd_t fd, void * buf, size_t len)
{
	return read((int)(intptr_t)fd, buf, (unsigned)len);
}

static ssize_t
default_writefunc(tarfd_t fd, const void * buf, size_t len)
{
	return write((int)(intptr_t)fd, buf, (unsigned)len);
}


static tartype_t default_type = { default_openfunc,
	default_closefunc, default_readfunc, default_writefunc };


static int
tar_init(TAR **t, char *pathname, tartype_t *type,
	 int oflags, int mode, int options)
{
	(void)mode;
	if ((oflags & O_ACCMODE) == O_RDWR)
	{
		errno = EINVAL;
		return -1;
	}

	*t = (TAR *)calloc(1, sizeof(TAR));
	if (*t == NULL)
		return -1;

	(*t)->pathname = strdup(pathname);
	(*t)->options = options;
	(*t)->type = (type ? type : &default_type);
	(*t)->oflags = oflags;

	if ((oflags & O_ACCMODE) == O_RDONLY)
		(*t)->h = libtar_hash_new(256,
					  (libtar_hashfunc_t)path_hashfunc);
	else
		(*t)->h = libtar_hash_new(16, (libtar_hashfunc_t)dev_hash);
	if ((*t)->h == NULL)
	{
		free((*t)->pathname);
		free(*t);
		return -1;
	}

	return 0;
}


/* open a new tarfile handle */
int
tar_open(TAR **t, char *pathname, tartype_t *type,
	 int oflags, int mode, int options)
{
	if (tar_init(t, pathname, type, oflags, mode, options) == -1)
		return -1;

	if ((options & TAR_NOOVERWRITE) && (oflags & O_CREAT))
		oflags |= O_EXCL;

#ifdef O_BINARY
	oflags |= O_BINARY;
#endif

	(*t)->fd = (*((*t)->type->openfunc))(pathname, oflags, mode);
	if ((*t)->fd == TAR_INVALID_FD)
	{
		free(*t);
		return -1;
	}

	return 0;
}


int
tar_fdopen(TAR **t, tarfd_t fd, char *pathname, tartype_t *type,
	   int oflags, int mode, int options)
{
	if (tar_init(t, pathname, type, oflags, mode, options) == -1)
		return -1;

	(*t)->fd = fd;
	return 0;
}


tarfd_t
tar_fd(TAR *t)
{
	return t->fd;
}


/* close tarfile handle */
int
tar_close(TAR *t)
{
	int i;

	i = (*(t->type->closefunc))(t->fd);

	if (t->h != NULL)
		libtar_hash_free(t->h, ((t->oflags & O_ACCMODE) == O_RDONLY
					? free
					: (libtar_freefunc_t)tar_dev_free));

	free(t->pathname);
	free(t);

	return i;
}


