/*
 * ParallelsCompiler.h: There are some "generic" compiler-specific
 * options that are useful to keep in one place to re-use and
 * disable when time comes.
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



#ifndef __PARALLELS_COMPILER_H__
#define __PARALLELS_COMPILER_H__

#ifdef __INTEL_COMPILER

	/**
	 * warning #1684: conversion from pointer to same-sized integral type (potential portability problem)
	 * unfortunately, we're getting way too many of these warnings because of void* -> ULONG_PTR conversion
	 * - so it's safer to disable it, cause because of it we don't see other warnings
	 */
	#pragma warning(disable:1684)

	/**
	 * warning #1195: conversion from integer to smaller pointer
	 * unfortunately, we're getting way too many of these warnings because of ULONG_PTR -> void* conversion
	 * - so it's safer to disable it, cause because of it we don't see other warnings
	 */
	#pragma warning(disable:1195)

	/**
	 * warning #177: function was declared but never referenced
	 * We have set of library include files in System\Kernel\modules\std defined UNUSED
	 * functions. GCC ignores warnings by using __attribute__((unused)) attribute. ICC
	 * for Windows supports MSVC syntax unsupported unused atribute. Thus ignore unused
	 * warning in this case.
	 */
	#pragma warning(disable:177)

#ifndef SUPPORT_ASM_MS
#  define SUPPORT_ASM_MS
#endif

#endif // __INTEL_COMPILER

#ifdef __GNUC__
#define SUPPORT_ASM_GCC
#endif

#endif // __PARALLELS_COMPILER_H__
