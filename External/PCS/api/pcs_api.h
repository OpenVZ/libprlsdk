/*
 * pcs_api.h
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef _PCS_API_H_
#define _PCS_API_H_ 1

#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct pcs_cluster;
struct pcs_fh;
struct stat;
struct pcs_process_attr;

#define PCS_MAX_SYMLINK_SIZE 4095

/* Special magic suffix. readlink() on a name which such suffix from fuse-mouted pcs
 * gives URI of file, which can be accessible via pcs api. If the file is PCS symlink,
 * it returns its contents to run it though VFS layer again: we cannot do this internally.
 */
#define PCS_API_URI_SUFFIX "#._PCS_URI_"

enum
{
	PCS_REQ_T_READ = 0,
	PCS_REQ_T_WRITE = 1,
};

#define PCS_REQ_F_ERROR 2

struct _pcs_api_iorequest_t
{
	off_t		pos;
	unsigned int	size;
	unsigned short	type;
	unsigned short	flags;

	void		*datasource;

	/* XXX For now get_chunk() returns contiguous area of memory at offset.
	 * iovec does not make of sense,but it could save some cpu. Later.
	 */
	void *		(*get_chunk)(void * datasource, unsigned int offset, int *lenp);

	void		(*complete)(struct _pcs_api_iorequest_t *);
};
typedef struct _pcs_api_iorequest_t pcs_api_iorequest_t;

struct pcs_api_wait_channel;

struct pcs_api_wait_channel * pcs_api_alloc_wait_channel(struct pcs_cluster *);
void pcs_api_release_wait_channel(struct pcs_api_wait_channel *);

/* Submit IO request */
void pcs_api_io_submit(struct pcs_fh *, pcs_api_iorequest_t *, struct pcs_api_wait_channel *);
int  pcs_api_io_wait(struct pcs_api_wait_channel *, int timeout);

int pcs_api_open(struct pcs_cluster *, const char * path, unsigned int flags, struct pcs_fh **);
int pcs_api_close(struct pcs_fh *);

int pcs_api_mknod(struct pcs_cluster *, const char * path, unsigned int mode, const char * data);
int pcs_api_mkdir(struct pcs_cluster *, const char * path, unsigned int mode);
int pcs_api_create(struct pcs_cluster *, const char * path, unsigned int mode, struct pcs_fh **);
int pcs_api_delete(struct pcs_cluster *, const char * path);
int pcs_api_rmdir(struct pcs_cluster *, const char * path);
int pcs_api_rename(struct pcs_cluster *, const char * from, const char * to);
int pcs_api_readlink(struct pcs_cluster *, const char * path, char * buf, int buflen);

int pcs_api_truncate(struct pcs_cluster *, const char * path, off_t size);
int pcs_api_ftruncate(struct pcs_fh * fh, off_t size);

int pcs_api_fsync(struct pcs_fh * fh, int isdatasync);
int pcs_api_read(struct pcs_fh *, char *buf, size_t size, off_t offset);
int pcs_api_write(struct pcs_fh *, const char *buf, size_t size, off_t offset);
int pcs_api_sync_io(struct pcs_fh *, pcs_api_iorequest_t *);

/* stat. File is not locked, so that the information can be invalid immediately upon return */
int pcs_api_stat(struct pcs_cluster *, const char * path, struct stat *);

int pcs_api_fstat(struct pcs_fh *, struct stat *);

int pcs_api_readdir(struct pcs_cluster * cluster, const char * path,
		    int (*filler)(const char * name, unsigned int dtype, void * priv),
		    const char * last, off_t skip, void * priv);

/* These functions fetch locally cached result, so that they are eligible only
 * with leased files.
 */

/* Called before retrying failed operations. */
void pcs_api_delay(struct pcs_cluster * cluster);

/* Initialize connection to cluster */

struct pcs_cluster * pcs_api_init(char * profile, struct pcs_process_attr *process_attr);
void pcs_api_fini(struct pcs_cluster *);

/* Service functions */
void pcs_api_set_loglevel(int loglevel);
void pcs_api_set_timeout(struct pcs_cluster *, int seconds);

const char * pcs_api_cluster_name(struct pcs_cluster *);
int pcs_api_resolve(const char * path, char * buf, int size);

#if defined(__cplusplus)
}
#endif

#endif /* _PCS_API_H_ */
