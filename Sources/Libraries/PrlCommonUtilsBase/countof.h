/*
 * countof.h: Defines _countof() macro if it is not defined yet.
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


#pragma once

#if defined(_countof)
// Do nothing, _countof already defined.
#elif defined(_MSC_VER)
// MS C/C++ compilers have _countof() defined in stdlib.h.
#include <stdlib.h>
#elif defined(__GNUC__)
// GNU C compiler
#if 0 /*defined(__cplusplus)*/
// More safe implementation for C++
// This implementation was copied from MS VC sources and it
// doesn't works in gcc for arrays of anonymous structures.
// Therefor, less safe C implementation will be used for C++ too.
#include <stddef.h> // required for size_t
extern "C++"
{
	// Note, that there is UNALIGNED attribute in MS VC implementation.
	// As I understood, in GCC both ((packed)) and ((aligned(x))) attributes
	// will have no effect here. I also failed to compose sample that will
	// produce incorrect results.
	template <typename _CountofType, size_t _SizeOfArray>
	char (*__countof_helper( /*UNALIGNED*/ _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
	#define _countof(_Array) sizeof(*__countof_helper(_Array))
}
#else
// Simple implementation for C
#define _countof(_Array) (sizeof(_Array) / sizeof((_Array)[0]))
#endif
#endif
