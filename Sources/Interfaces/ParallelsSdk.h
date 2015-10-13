/*
 * ParallelsSdk.h
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


#ifndef __PARALLELS_SDK_PRIVATE__
#define __PARALLELS_SDK_PRIVATE__

#include <SDK/Include/PrlOses.h>

// keys for SDK calls PrlSrv_StoreValueByKey / PrlVm_StoreValueByKey
#define PRL_KEY_TO_AUTH_USER				"{09A8CDDF-9C1D-4c2a-BEF0-02071BC72DFA}"
#define PRL_KEY_SET_VNC_ENCRYPTION_DATA		"{616a72c7-e7cf-4eb8-bb5e-b9c1b37f5fdb}"


static inline int PVS_GUEST_VIRTIO_SUPPORTED(unsigned int nVersion)
{
	return ((IS_WINDOWS(nVersion) && nVersion >= PVS_GUEST_VER_WIN_VISTA) ||
			(IS_LINUX(nVersion)
			 && nVersion != PVS_GUEST_VER_LIN_KRNL_24
			 && nVersion != PVS_GUEST_VER_LIN_RHLES3 ));
}

static inline int PVS_GUEST_E1000_SUPPORTED(unsigned int nVersion)
{
	return (IS_WINDOWS(nVersion) && nVersion >= PVS_GUEST_VER_WIN_VISTA) ||
		(IS_LINUX(nVersion) && nVersion != PVS_GUEST_VER_LIN_KRNL_24) ||
		(IS_MACOS(nVersion) && nVersion >= PVS_GUEST_VER_MACOS_SNOW_LEOPARD) ||
		(!IS_WINDOWS(nVersion) && !IS_LINUX(nVersion) && !IS_MACOS(nVersion) && !IS_OS2(nVersion));
}

static inline int PVS_GUEST_SCSI_LSI_SPI_SUPPORTED(unsigned int nVersion)
{
	return IS_LINUX(nVersion);
}

static inline int PVS_GUEST_SCSI_LSI_SAS_SUPPORTED(unsigned int nVersion)
{
	return nVersion >= PVS_GUEST_VER_WIN_VISTA && nVersion <= PVS_GUEST_VER_WIN_LAST;
}

#endif // __PARALLELS_SDK_PRIVATE__

