/*
 * CommonTypes.h: Types abstractor (abstract from Qt if need).
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

#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#ifdef _WIN_
	#include <windows.h>
#else
	// Same for mac and linux
	#include <sys/uio.h>
	#include <sys/fcntl.h>
	#include <errno.h>
#endif

#ifdef _WIN_
	#define SYSERR_INTERNAL ERROR_INTERNAL_ERROR
	#define SYSERR_NOMEM ERROR_NOT_ENOUGH_MEMORY
#else
	#define SYSERR_INTERNAL EFAULT
	#define SYSERR_NOMEM ENOMEM
#endif

/**
 * Abstractions from OS specific types
 */
#ifdef _WIN_
	// File open flags
	#define FA_READ					GENERIC_READ
	#define FA_WRITE				GENERIC_WRITE

	// Flags definition
	#define FF_NO_BUFFERING			FILE_FLAG_NO_BUFFERING
	#define FF_DIRECT_WRITE			FILE_FLAG_WRITE_THROUGH

	// Seek pointer managing
	#define FS_BEGIN				FILE_BEGIN
	#define FS_END					FILE_END
	#define FS_CURRENT				FILE_CURRENT

	// File sharing
	#define FSHR_READ				FILE_SHARE_READ
	#define FSHR_WRITE				FILE_SHARE_WRITE

	// Creation disposition
	#define FO_NEW					CREATE_NEW
	#define FO_CREATE				CREATE_ALWAYS
	#define FO_ALWAYS				OPEN_ALWAYS
	#define FO_EXISTING				OPEN_EXISTING
	#define FO_TRUNC				TRUNCATE_EXISTING

	// Handles definition
	#define FILE_HANDLE				HANDLE
	#define INVALID_FILE_HANDLE		INVALID_HANDLE_VALUE

	// Thread definitions
	#define THREAD_RETVAL			DWORD
	#define THREAD_HANDLE			HANDLE
	#define INVALID_THREAD_HANDLE	INVALID_HANDLE_VALUE

	#define AFE_SHARING_VIOLATION	ERROR_SHARING_VIOLATION
	#define AFE_DISK_FULL			ERROR_DISK_FULL

	// Define structure for SG operations
	typedef struct __iovec_win
	{
		char* iov_base;
		unsigned int iov_len;
	} IOVEC;

	struct iovec {
		void	*iov_base;
		unsigned int iov_len;
	};

#elif defined (_LIN_)
	// File open flags
	// Problem that O_RDONLY is equal 0, so we can't use it without changes
	#define FA_READ					1
	//O_RDONLY
	#define FA_WRITE				2
	//O_WRONLY

	// Flags definition
	// Use O_DIRECT instead of O_SYNC..
	#define FF_NO_BUFFERING O_DIRECT
	#define FF_DIRECT_WRITE O_DIRECT

	// Seek pointer managing
	#define FS_BEGIN				SEEK_SET
	#define FS_END					SEEK_END
	#define FS_CURRENT				SEEK_CUR

	// File sharing
	#define FSHR_READ				1
	//F_RDLCK
	#define FSHR_WRITE				2
	//F_WRLCK

	// Creation disposition
	#define FO_NEW					O_EXCL | O_CREAT | O_LARGEFILE
	#define FO_CREATE				O_CREAT | O_LARGEFILE
	#define FO_EXISTING				O_LARGEFILE
	#define FO_ALWAYS				O_CREAT | O_LARGEFILE
	#define FO_TRUNC				O_TRUNC | O_LARGEFILE

	// Handles definition
	#define FILE_HANDLE				int
	#define INVALID_FILE_HANDLE		((int)(-1))
	// Thread definitions
	#define THREAD_RETVAL			void*
	#define THREAD_HANDLE			pthread_t
	#define INVALID_THREAD_HANDLE	0

	#define AFE_SHARING_VIOLATION	EWOULDBLOCK
	#define AFE_DISK_FULL			ENOSPC

	// Define same name as for windows code
	typedef struct iovec IOVEC;
#else
	#error "Unknown OS"
#endif

// Define PAGE_SIZE
#ifndef PAGE_SIZE
	#define PAGE_SIZE				0x1000
#endif

#endif // __COMMON_TYPES_H__
