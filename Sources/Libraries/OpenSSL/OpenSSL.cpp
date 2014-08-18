/*
 * OpenSSL.cpp: Initialization/Deinitialization code of SSL library
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

#include <assert.h>

#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif
#include <openssl/err.h>
#include <openssl/rand.h>

#ifndef _WIN_
#include <pthread.h>
#endif

#include <QMutex>
#include <Libraries/Std/AtomicOps.h>
#include <Libraries/Std/MemBarrier.h>
#include "OpenSSL.h"

namespace OpenSSL
{
	static QMutex *sslMuteces;
	/*
	 * The static variable of nonPOD format may call OpenSSL::Initialize(), thus
	 * QMutex here must be created and initialized as a pointer. In other case
	 * code may work unpredictable.
	 */
	static QMutex *initMutex;
	static int initialized;

	static unsigned long ThreadIdCallback()
	{
#ifdef _WIN_
		return (unsigned long)::GetCurrentThreadId();
#else
		return (unsigned long)::pthread_self();
#endif
	}

	static void LockingCallback(int mode, int idx, const char*, int)
	{
		assert(sslMuteces != 0);

		if (mode & CRYPTO_LOCK)
			sslMuteces[idx].lock();
		else
			sslMuteces[idx].unlock();
	}

	// Called under init lock held
	static inline void __freeSSL()
	{
		// Must be zeroed before engine/crypto cleanup
		CRYPTO_set_id_callback(0);
		CRYPTO_set_locking_callback(0);

		CONF_modules_free();
		EVP_cleanup();
		ENGINE_cleanup();
		CRYPTO_mem_leaks_fp(stderr);
		CRYPTO_cleanup_all_ex_data();
		ERR_remove_state(0);
		ERR_free_strings();

		delete[] sslMuteces;
		sslMuteces = NULL;
	}

	bool Initialize()
	{
		QMutex *tmp;
		bool ret = true;

		while (1) {
			if ((ULONG_PTR)initMutex > 1)
				break;

			if (initMutex)
				goto wbarrier;

			// Simulate locking
			if (AtomicCompareSwapPv((void**)&initMutex, (void *)NULL, (void *)1))
				goto wbarrier;

			// At first instanciating, the io must be active
			tmp = new (std::nothrow) QMutex();
			if (tmp == NULL) {
				/*
				 * When you set initMutex to 0 - some other waiter will try to
				 * allocate this mutex. Set to 0 is safe operation in case the only
				 * one instance (currently executing allocation) will set it.
				 */
				initMutex = 0;
				return false;
			}

			WriteMemoryBarrier();

			initMutex = tmp;
			break;

		wbarrier:
			WriteMemoryBarrier();
		}

		initMutex->lock();

		initialized++;

		if (initialized > 1)
			goto exit;

		assert(CRYPTO_num_locks());
		sslMuteces = new (std::nothrow) QMutex[CRYPTO_num_locks()];

		if (!sslMuteces) {
			__freeSSL();
			ret = false;
			goto exit;
		}

		CRYPTO_set_id_callback(&ThreadIdCallback);
		CRYPTO_set_locking_callback(&LockingCallback);

		SSL_library_init();
		SSL_load_error_strings();

	exit:
		initMutex->unlock();
		return ret;
	}

	void Finalize()
	{
		initMutex->lock();

		if (!initialized) {
			assert(0);
			goto exit;
		}

		if (initialized == 1)
			__freeSSL();

		--initialized;
	exit:
		initMutex->unlock();
	}
} // OpenSSL
