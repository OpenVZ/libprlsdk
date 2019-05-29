/*
 * PrlFs.h: Remote file system specific definitions
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#ifndef __PARALLELS_API_FS_H__
#define __PARALLELS_API_FS_H__


#define FS_PERM_EXECUTE					0x1
#define FS_PERM_READ					0x2
#define FS_PERM_WRITE					0x4

#define IS_FS_EXECUTE_PERMITTED(x)		( (x) & FS_PERM_EXECUTE ?1:0 )
#define IS_FS_READ_PERMITTED(x)			( (x) & FS_PERM_READ ?1:0 )
#define IS_FS_WRITE_PERMITTED(x)		( (x) & FS_PERM_WRITE ?1:0 )

#define SET_FS_EXECUTE_ENABLED(x)		(x) | FS_PERM_EXECUTE;
#define SET_FS_READ_ENABLED(x)			(x) | FS_PERM_READ;
#define SET_FS_WRITE_ENABLED(x)			(x) | FS_PERM_WRITE;

#define SET_FS_EXECUTE_DISABLED(x)		(x) & ~FS_PERM_EXECUTE;
#define SET_FS_READ_DISABLED(x)			(x) & ~FS_PERM_READ;
#define SET_FS_WRITE_DISABLED(x)		(x) & ~FS_PERM_WRITE;

#endif//__PARALLELS_API_FS_H__
