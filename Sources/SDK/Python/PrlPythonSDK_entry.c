/*
 * PrlPythonSDK_entry.c: Python programming language extension
 * library that serves as a wrapper to the native virtuozzo SDK
 * library functions. Entry point to the module - packed as a "pure
 * C" library to define correct exported entry point to the clients.
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#include "SDK/Python/PrlPython.h"


#ifndef PYTHON_MODULE_NAME
	#error "Unknown python module name!"
#endif

#if (VZ_PY_VER < 3)
#define ENTRY_F2(t) init##t
#else
#define ENTRY_F2(t) PyInit_##t
#endif

#define ENTRY_F(t) ENTRY_F2(t)
#define ENTRY_FUNCTION(x) ENTRY_F(x)

#if !defined(_WIN_) && __GNUC__ >= 4 && VZ_PY_VER < 3
	#define ENTRY_FUNC_PUBLIC __attribute__ ((visibility("default")))
#else
	#define ENTRY_FUNC_PUBLIC
#endif

extern PyMODINIT_FUNC entry(void );

PyMODINIT_FUNC ENTRY_FUNC_PUBLIC ENTRY_FUNCTION(PYTHON_MODULE_NAME)(void )
{
#if (VZ_PY_VER < 3)
	entry();
#else
	return entry();
#endif
}
