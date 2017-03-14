/*
 * Debug.h: Sometimes we have some issues that have to be debugged
 * This header has to be included to the modules that want their
 * memory allocations to be carefully tracked on the sumbolic level,
 * also to have ability to perform compile-time assertions, etc.
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */



#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef ENABLE_MALLOC_DEBUG
    #include "../External/Stones/debug_new.h"
#endif

#include "../External/Stones/static_assert.h"

#endif // __DEBUG_H__
