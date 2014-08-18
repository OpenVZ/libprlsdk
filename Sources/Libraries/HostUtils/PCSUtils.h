/*
 * PCSUtils.h: Declaration of PCS API imported from libpcs_client
 * dynamic libary
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

#ifndef __PCS_UTILS_H__
#define  __PCS_UTILS_H__ 1

#ifdef _LIN_

#include <pcs_api.h>

#define PCS_URI "pstorage://"

typedef struct s_pcs_api {

	void *dl_handle;

	struct pcs_api_wait_channel * (*alloc_wait_channel)(struct pcs_cluster *);
	void (*release_wait_channel)(struct pcs_api_wait_channel *);

	struct pcs_cluster* (*init)(char *profile, struct pcs_process_attr *process_attr);
	void (*fini)(struct pcs_cluster *);

	int (*create)(struct pcs_cluster *, const char * path, unsigned int mode, struct pcs_fh **);
	int (*open)(struct pcs_cluster *, const char * path, unsigned int flags, struct pcs_fh **);
	int (*close)(struct pcs_fh *);
	void (*io_submit)(struct pcs_fh *, pcs_api_iorequest_t *, struct pcs_api_wait_channel *);
	int (*io_wait)(struct pcs_api_wait_channel *, int timeout);
	int (*ftruncate)(struct pcs_fh * fh, off_t size);
	int (*fsync)(struct pcs_fh * fh, int isdatasync);
	int (*fstat)(struct pcs_fh *, struct stat *);
	int (*read)(struct pcs_fh *, char *buf, size_t size, off_t offset);
	int (*write)(struct pcs_fh *, const char *buf, size_t size, off_t offset);
	int (*sync_io)(struct pcs_fh *, pcs_api_iorequest_t *);

	int (*resolve)(const char * path, char * buf, int size);
	const char * (*cluster_name)(struct pcs_cluster *);
} pcs_api_t;

int pcs_api_get(pcs_api_t**);
void pcs_api_put(pcs_api_t*);
bool pcs_fs(const char * path);

struct pcs_cluster * pcs_cluster_get(char* profile);
void pcs_cluster_put(struct pcs_cluster *cluster);
int pcs_cluster_name(pcs_api_t* api, const char* file_path, char* uri, int uri_sz,
					 char** canon_path, char** cluster_name);

#else /* _LIN_ */
static __inline bool pcs_fs(const char *) {
	return false;
}
#endif

#endif
