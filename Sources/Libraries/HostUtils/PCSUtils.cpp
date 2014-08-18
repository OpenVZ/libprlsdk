/*
 * PCSUtils.cpp: Wrapper for accessing PCS API imported from
 * libpcs_client dynamic libary.
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


#include <QMutex>
#include <QMutexLocker>

#include <dlfcn.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>

#include "Interfaces/ParallelsTypes.h"
#include "Libraries/Std/std_list.h"
#include "Libraries/HostUtils/PCSUtils.h"
#include "Libraries/Logging/Logging.h"

#define LIBPCS_CLIENT	"libpcs_client.so"

static pcs_api_t pcs_api;
static int api_valid = 0;
static QMutex mtx_lock;
static int api_refcnt = 0;

/*
 * psc cluster management routines
 * Here "cd" stands for cluster desctriptor
 * all functions has "_" prefix since it must be called with mtx_lock held
 */
CD_LIST_HEAD(pcs_cluster_lh);

struct psc_cd
{
	struct cd_list		list;
	unsigned int		ref_cnt;
	struct pcs_cluster	*pcs_cluster;
};

static struct psc_cd *_cd_alloc()
{
	struct psc_cd *cd = (struct psc_cd *)malloc(sizeof(*cd));
	if (!cd)
		return NULL;

	cd->ref_cnt = 0;
	cd_list_add_tail(&cd->list, &pcs_cluster_lh);
	return cd;
}

static void _cd_put(struct psc_cd *cd)
{
	cd_list_del(&cd->list);
	free(cd);
}

static struct psc_cd *_cd_find_name(const char *profile)
{
	struct psc_cd *cd;

	cd_list_for_each_entry(struct psc_cd, cd, &pcs_cluster_lh, list) {

		const char* p = pcs_api.cluster_name(cd->pcs_cluster);
		if (p == NULL)
			continue;

		if (strcmp(profile, p))
			continue;

		return cd;
	}
	return NULL;
}

static struct psc_cd *_cd_find(struct pcs_cluster	*cluster)
{
	struct psc_cd *cd;
	cd_list_for_each_entry(struct psc_cd, cd, &pcs_cluster_lh, list) {

		if (cd->pcs_cluster == cluster)
			return cd;
	}
	return NULL;
}

int pcs_cluster_name(	pcs_api_t* api, const char* file_path,
						char* uri, int uri_sz,
						char** canon_path, char** cluster_name)
{
	if (!api || !file_path || !uri || !uri_sz)
		return -EINVAL;

	int res = api->resolve(file_path, uri, uri_sz);

	if (res < (int)sizeof(PCS_URI) ||
		memcmp(uri, PCS_URI, sizeof(PCS_URI)-1)) {
		return -ENOENT;
	}

	char* path_start = strchr(uri + sizeof(PCS_URI)-1, '/');

	if (path_start == NULL)
		return -ENOENT;

	/*
	* Here we split uri to two strings
	* uri = 'pcs://pcs_csluster_name/canon_path\0'
	* path_start points to area '/canon_path\0'
	* by replacing "/" char after pcs_cluster_name we get two strings
	*/
	if (cluster_name) {
		*cluster_name=uri + sizeof(PCS_URI)-1;
		*path_start = '\0';
	}

	if (canon_path)
		*canon_path = ++path_start;

	return 0;
}

// forward declaration
static int __pcs_api_get(pcs_api_t** p);

static void __pcs_api_instance_lock()
{
	static bool locked = false;

	Q_ASSERT_X(api_valid,"PCS API","Locking of uninitialized API");

	if (locked)
		return;

	++api_refcnt;

	locked = true;
}

static int init()
{
	int i;
	void *dlhandle;
	static int pcs_present = 1;

	struct _api {
		const char *name;
		void **pptr;
	} funcs[] = {
		{ "pcs_api_alloc_wait_channel",		(void**)&pcs_api.alloc_wait_channel },
		{ "pcs_api_release_wait_channel",	(void**)&pcs_api.release_wait_channel },
		{ "pcs_api_create",		(void**)&pcs_api.create },
		{ "pcs_api_init",		(void**)&pcs_api.init },
		{ "pcs_api_fini",		(void**)&pcs_api.fini },
		{ "pcs_api_open",		(void**)&pcs_api.open },
		{ "pcs_api_close",		(void**)&pcs_api.close },
		{ "pcs_api_io_submit",	(void**)&pcs_api.io_submit },
		{ "pcs_api_io_wait",	(void**)&pcs_api.io_wait },
		{ "pcs_api_ftruncate",	(void**)&pcs_api.ftruncate },
		{ "pcs_api_fsync",		(void**)&pcs_api.fsync },
		{ "pcs_api_fstat",		(void**)&pcs_api.fstat },
		{ "pcs_api_read",		(void**)&pcs_api.read },
		{ "pcs_api_write",		(void**)&pcs_api.write },
		{ "pcs_api_sync_io",	(void**)&pcs_api.sync_io },
		{ "pcs_api_resolve",	(void**)&pcs_api.resolve },
		{ "pcs_api_cluster_name",(void**)&pcs_api.cluster_name },
		{ NULL, NULL }
	};

	if (!pcs_present)
		return -ENOENT;

	if (api_valid)
		return 0;

	dlhandle = dlopen(LIBPCS_CLIENT, RTLD_LAZY);
	if (dlhandle == NULL) {

		if (errno == ENOENT)
			pcs_present = 0; // Suppress flooding

		WRITE_TRACE(DBG_FATAL, "Failed to load %s: %s", LIBPCS_CLIENT, dlerror());
		return -EAGAIN;
	}

	dlerror();

	for (i=0; funcs[i].name; ++i) {
		*(funcs[i].pptr) = dlsym(dlhandle, funcs[i].name);
		if (*(funcs[i].pptr) == NULL) {
			WRITE_TRACE(DBG_FATAL,"PCS API: function %s isn't available - %s",
					funcs[i].name, dlerror());
			dlclose(dlhandle);
			return -ENOENT;
		}
	}

	pcs_api.dl_handle = dlhandle;
	api_valid = 1;
	return 0;
}

static void fini()
{
	Q_ASSERT_X(cd_list_empty(&pcs_cluster_lh),
				"PCS API","There are active references to cluster");
	dlclose(pcs_api.dl_handle);
	api_valid = 0;
}

int __pcs_api_get(pcs_api_t** p)
{
	int r;

	if (p == NULL)
		return -EINVAL;

	if ((r = init()))
		return r;

	*p = &pcs_api;
	++api_refcnt;

	__pcs_api_instance_lock();

	return 0;
}

int pcs_api_get(pcs_api_t** p)
{
	QMutexLocker lock(&mtx_lock);
	return __pcs_api_get(p);
}

static void __pcs_api_put(pcs_api_t*)
{
	if (!api_valid)
		return;

	if (--api_refcnt != 0)
		return;

	fini();
}

void pcs_api_put(pcs_api_t* p)
{
	QMutexLocker lock(&mtx_lock);
	return __pcs_api_put(p);
}

struct pcs_cluster * pcs_cluster_get(char* profile)
{
	QMutexLocker lock(&mtx_lock);

	struct psc_cd *cd;
	pcs_api_t* p = NULL;
	if (!api_valid && __pcs_api_get(&p))
		return NULL;

	// just keep reference to obtained api
	Q_UNUSED(p);

	cd = _cd_find_name(profile);
	if (!cd) {

		cd = _cd_alloc();
		if (!cd)
			goto fini;

		cd->pcs_cluster = pcs_api.init(profile, NULL);

		if (!cd->pcs_cluster) {
			_cd_put(cd);
			goto fini;
		}

	}

	cd->ref_cnt++;
	return cd->pcs_cluster;

fini:
	__pcs_api_put(p);
	return NULL;
}

void pcs_cluster_put(struct pcs_cluster *cluster)
{
	QMutexLocker lock(&mtx_lock);
	struct psc_cd *cd;

	if (!cluster)
		return;

	cd = _cd_find(cluster);
	if (!cd) {
		Q_ASSERT_X(!cd,"PCS API","put: cluster does not exist");
		return;
	}

	Q_ASSERT_X(cd->ref_cnt>0,"PCS API","put: invalid reference count");
	if (--cd->ref_cnt != 0)
		return;

	pcs_api.fini(cd->pcs_cluster);

	_cd_put(cd);

	if (!cd_list_empty(&pcs_cluster_lh))
		return;

	__pcs_api_put(NULL);
}

bool pcs_fs(const char * path)
{
#ifndef _LIN_
	(void)path;
	return false;
#else
	pcs_api_t* pcs_api = NULL;

	int r = pcs_api_get(&pcs_api);
	if (r || pcs_api == NULL)
		return false;

	char uri[PCS_MAX_SYMLINK_SIZE+1];
	int size = sizeof(uri);


	r = pcs_api->resolve(path, uri, size);
	pcs_api_put(pcs_api);

	return (r>=(int)sizeof(PCS_URI) && memcmp(uri, PCS_URI, sizeof(PCS_URI)-1)==0);
#endif
}
