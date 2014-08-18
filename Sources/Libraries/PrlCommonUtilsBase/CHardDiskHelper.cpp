/*
 * CHardDiskHelper.cpp: Implementation class helper for the hard
 * disk additional verification
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


#include "PrlDisk.h"
#include "CHardDiskHelper.h"
#include "SDK/Include/PrlOses.h"

bool CHardDiskHelper::isEfiPartition(unsigned int nPartitionType)
{
	switch ( nPartitionType )
	{
		case PRL_EFI_PARTITION:
		case PRL_EFI_PARTITION_GPT:
			return true;
		default:
			;
	}

	return false;
}

bool CHardDiskHelper::isBootCampPartition(unsigned int nPartitionType)
{
	if ( isEfiPartition( nPartitionType ) )
		return true;

	switch ( nPartitionType )
	{
		case PRL_WIN_EXTENDED_PARTITION:
		case PRL_WINDOWS_PARTITION_NTFS:
		case PRL_WINDOWS_PARTITION_FAT:
		case PRL_WINDOWS_PARTITION_FAT32:
		case PRL_WINDOWS_PARTITION_FAT32_LBA:
		case PRL_DATA_PARTITION:
		case PRL_LINUX_PARTITION_SWAP:
		case PRL_LINUX_PARTITION_DATA:
		case PRL_BSD_PARTITION:
			return true;
		default:
			;
	}

	return false;
}

bool CHardDiskHelper::isSataSupportedForOs( unsigned int ioOsType, unsigned int uiOsVer )
{
	if ( ioOsType == PVS_GUEST_TYPE_MACOS
		|| ( ioOsType == PVS_GUEST_TYPE_LINUX &&
			uiOsVer != PVS_GUEST_VER_LIN_KRNL_24 &&
			uiOsVer != PVS_GUEST_VER_LIN_OTHER )
		|| ( uiOsVer >= PVS_GUEST_VER_WIN_VISTA &&
			uiOsVer <= PVS_GUEST_VER_WIN_LAST ) )
		return true;

	if( uiOsVer >= PVS_GUEST_VER_BSD_7X && uiOsVer <= PVS_GUEST_VER_BSD_LAST )
		return true;

	return false;
}

bool CHardDiskHelper::isSataSupportedForChipset( unsigned int chipsetType )
{
	if( 0 != chipsetType )
		return true;

	return false;
}

bool CHardDiskHelper::isSataSupportedForVmConfig( unsigned int osType, unsigned int oType, unsigned int chipsetType )
{
	if( !isSataSupportedForOs( osType, oType ) )
		return false;

	if( !isSataSupportedForChipset( chipsetType ) )
		return false;

	return true;
}


