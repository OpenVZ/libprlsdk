/*
 * EFI_GPT_Types.h: GUID partitions types definition
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

#ifndef __GPT_EFI_TYPES_DEFINED__
#define __GPT_EFI_TYPES_DEFINED__

#include <QString>
#include <QList>

#include "Libraries/PrlUuid/Uuid.h"

////////////////////////////////////////////////////////////////////////
/**
 * OS types, defined by GUIDs
 *
 * @author antonz@
 */
////////////////////////////////////////////////////////////////////////
enum GPT_OS_TYPE
{
	// Generic type
	GPT_OS_GENERIC = 1,
	// Windows
	GPT_OS_WINDOWS,
	// HP-UX
	GPT_OS_HPUX,
	// Linux
	GPT_OS_LINUX,
	// Free BSD
	GPT_OS_FREEBSD,
	// Mac OS X
	GPT_OS_MACOSX,
	// Solaris
	GPT_OS_SOLARIS,
	// Common partitions (data)
	GPT_OS_COMMON,
	// Invalid type
	GPT_OS_INVALID = 0xFFFFFFFF
};

////////////////////////////////////////////////////////////////////////
/**
 * OS descriptor
 *
 * @author antonz@
 */
////////////////////////////////////////////////////////////////////////
typedef struct
{
	// Os type
	GPT_OS_TYPE	OsType;
	// MBR type
	PRL_UINT8 MbrType;
	// Description as string
	QString Description;
} GPT_DESCR;

/**
 * Static conversion routines
 *
 * @author antonz@
 */
class CGPTConvertor
{
public:
	// Get description of partition type by GUID
	static QString GetDescriptionByGUID(const Uuid& guid);
	// Get description of partition type by GUID
	static GPT_OS_TYPE GetTypeByGUID(const Uuid& guid);
	// Get description of partition type by GUID
	static PRL_UINT8 GetMbrTypeByGUID(const Uuid& guid);
	// Get list of supported partitions
	static QList<QString> GetDataPartitionsList();
};

#endif
