/*
 * ParallelsAlloc.h: Aligned memory allocation code for cross
 * platform compilation.
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


#ifndef __PARALLELS_ALLOC_HEADER_H__
#define __PARALLELS_ALLOC_HEADER_H__

/**
 * This is cross-platform code for the virtual memory alloc/free calls.
 *
 * Mainly this required becase we are use valloc/free too often and its better
 * to have one call instead of three.
 * Also we found problem with Linux version of the valloc. And we can fix call
 * without rewriting a lot of code.
 *
 * Read links below  before asking about valloc()
 * https://bugzilla.redhat.com/show_bug.cgi?id=461481
 */
#if defined(_LIN_) || defined(_WIN_)
# include <malloc.h>
#endif

/* Virtual allocation */
#if defined(_WIN_)
# define prl_valloc(size) _aligned_malloc(size, PAGE_SIZE)
#elif defined(_LIN_)
#include <Interfaces/ParallelsTypes.h>

static inline void *prl_valloc(size_t size)
{
	void *chunk = malloc(size + PAGE_SIZE + sizeof(void *));
	if (chunk == NULL)
		return chunk;
	unsigned long addr = (unsigned long)chunk;
	addr = (addr + sizeof(void *) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	*(void **)(addr - sizeof(void *)) = chunk;
	return (void *)addr;
}
#endif

/* Virtual free */
#if defined(_WIN_)
# define prl_vfree(ptr) _aligned_free(ptr)
#elif defined(_LIN_)
static inline void prl_vfree(void *ptr)
{
	if (ptr == NULL)
		return;
	free(*((void **)ptr - 1));
}
#endif

#endif // ALLOC
