/*
 * MemBarrier.h: Memory barriers for CPU synchronization.
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


#ifndef __MEM_BARRIER_H__
#define __MEM_BARRIER_H__

#include "Interfaces/ParallelsTypes.h"

/**
 * Default full memory barriers
 */

#if defined _MSC_VER

	#include <emmintrin.h>

	#if defined __INTEL_COMPILER
		#define CompilerBarrier()		__asm{ nop }
	#else // __INTEL_COMPILER
		#ifdef __cplusplus
			extern "C" void _ReadWriteBarrier();
		#else
			void _ReadWriteBarrier();
		#endif
		#pragma intrinsic(_ReadWriteBarrier)
		#define CompilerBarrier()		_ReadWriteBarrier()
	#endif // __INTEL_COMPILER

	#define ReadMemoryBarrier()		CompilerBarrier()
	#define WriteMemoryBarrier()	CompilerBarrier()
	#define RWMemoryBarrier()		_mm_mfence()

#elif defined __GNUC__

	#define CompilerBarrier()		__asm__ __volatile__ ("": : :"memory")

	#if defined(_X86_) || defined(_AMD64_)
		#define ReadMemoryBarrier()		CompilerBarrier()
		#define WriteMemoryBarrier()	CompilerBarrier()
		#define RWMemoryBarrier()		__asm__ __volatile__ ("mfence": : :"memory")
	#else
		#error "MemoryBarrier error: Unknown platform"
	#endif

#else
	#error "MemoryBarrier error: Unknown compiler"
#endif

#endif /* __MEM_BARRIER_H__ */
