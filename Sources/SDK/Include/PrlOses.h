/*
 * PrlOses.h: Declarations of OSes constants
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


#ifndef __PARALLELS_API_OSES_H__
#define __PARALLELS_API_OSES_H__

#include "PrlTypes.h"

// Guest OS types enumeration
#define PVS_GUEST_TYPE_MACOS			0x07
#define PVS_GUEST_TYPE_WINDOWS			0x08
#define PVS_GUEST_TYPE_LINUX			0x09
#define PVS_GUEST_TYPE_FREEBSD			0x0A
#define PVS_GUEST_TYPE_OS2				0x0B
#define PVS_GUEST_TYPE_MSDOS			0x0C
#define PVS_GUEST_TYPE_NETWARE			0x0D
#define PVS_GUEST_TYPE_SOLARIS			0x0E
#define PVS_GUEST_TYPE_CHROMEOS			0x0F
#define PVS_GUEST_TYPE_ANDROID			0x10
#define PVS_GUEST_TYPE_OTHER			0xFF

#define PVS_GUEST_VER_MACOS_TIGER		 0x0701
#define PVS_GUEST_VER_MACOS_LEOPARD		 0x0702
#define PVS_GUEST_VER_MACOS_SNOW_LEOPARD 0x0703
#define PVS_GUEST_VER_MACOS_UNIVERSAL	 PVS_GUEST_VER_MACOS_SNOW_LEOPARD
// Add here a new OS version and assign it to PVS_GUEST_VER_MACOS_LAST
#define PVS_GUEST_VER_MACOS_LAST		PVS_GUEST_VER_MACOS_UNIVERSAL
#define IS_VALID_MACOS_VERSION(ver)	\
	((ver) >= PVS_GUEST_VER_MACOS_TIGER && (ver) <= PVS_GUEST_VER_MACOS_LAST)

#define PVS_GUEST_VER_WIN_311			0x0801
#define PVS_GUEST_VER_WIN_95			0x0802
#define PVS_GUEST_VER_WIN_98			0x0803
#define PVS_GUEST_VER_WIN_ME			0x0804
#define PVS_GUEST_VER_WIN_NT			0x0805
#define PVS_GUEST_VER_WIN_2K			0x0806
#define PVS_GUEST_VER_WIN_XP			0x0807
#define PVS_GUEST_VER_WIN_2003			0x0808
#define PVS_GUEST_VER_WIN_VISTA			0x0809
#define PVS_GUEST_VER_WIN_2008			0x080A
#define PVS_GUEST_VER_WIN_WINDOWS7		0x080B
#define PVS_GUEST_VER_WIN_WINDOWS8		0x080C
#define PVS_GUEST_VER_WIN_2012			0x080D
#define PVS_GUEST_VER_WIN_WINDOWS8_1	0x080E
#define PVS_GUEST_VER_WIN_WINDOWS10		0x080F
#define PVS_GUEST_VER_WIN_WINDOWS_10		0x080F
#define PVS_GUEST_VER_WIN_2016			0x0810
#define PVS_GUEST_VER_WIN_2019			0x0811
// Add here a new OS version and assign it to PVS_GUEST_VER_WIN_LAST
#define PVS_GUEST_VER_WIN_LAST			PVS_GUEST_VER_WIN_2019
#define PVS_GUEST_VER_WIN_OTHER			0x08FF

// PVS_GUEST_VER_WIN_OTHER is used for legacy guest OS (9x and below), but has maximal number 0xFF.
#define IS_WIN_VER_BELOW(ver1, ver2) ((ver1 < ver2) || (ver1 == PVS_GUEST_VER_WIN_OTHER))

#define IS_VALID_WINDOWS_VERSION(ver)										\
	(((ver) >= PVS_GUEST_VER_WIN_311 && (ver) <= PVS_GUEST_VER_WIN_LAST) ||	\
	 (ver) == PVS_GUEST_VER_WIN_OTHER)

#define PVS_GUEST_VER_CHROMEOS_1x		0x0F01
// Add here a new OS version and assign it to PVS_GUEST_VER_MACOS_LAST
#define PVS_GUEST_VER_CHROMEOS_LAST		PVS_GUEST_VER_CHROMEOS_1x
#define PVS_GUEST_VER_CHROMEOS_OTHER	0x0FFF
#define IS_VALID_CHROMEOS_VERSION(ver)								\
	(((ver) >= PVS_GUEST_VER_CHROMEOS_1x &&							\
	  (ver) <= PVS_GUEST_VER_CHROMEOS_LAST) ||						\
	  (ver) == PVS_GUEST_VER_CHROMEOS_OTHER)

#define PVS_GUEST_VER_ANDROID_2_x		0x1000
#define PVS_GUEST_VER_ANDROID_3_x		0x1001
#define PVS_GUEST_VER_ANDROID_4_0		0x1002
#define PVS_GUEST_VER_ANDROID_LAST		PVS_GUEST_VER_ANDROID_4_0
#define PVS_GUEST_VER_ANDROID_OTHER		0x10FF
#define IS_VALID_ANDROID_VERSION(ver)								\
	(((ver) >= PVS_GUEST_VER_ANDROID_2_x &&							\
	  (ver) <= PVS_GUEST_VER_ANDROID_LAST) ||						\
	  (ver) == PVS_GUEST_VER_ANDROID_OTHER)

#define PVS_GUEST_VER_LIN_REDHAT		0x0901
#define PVS_GUEST_VER_LIN_SUSE			0x0902
#define PVS_GUEST_VER_LIN_MANDRAKE		0x0903
#define PVS_GUEST_VER_LIN_KRNL_24		0x0904
#define PVS_GUEST_VER_LIN_KRNL_26		0x0905
#define PVS_GUEST_VER_LIN_DEBIAN		0x0906
#define PVS_GUEST_VER_LIN_FEDORA		0x0907
#define PVS_GUEST_VER_LIN_FEDORA_5		0x0908
#define PVS_GUEST_VER_LIN_XANDROS		0x0909
#define PVS_GUEST_VER_LIN_UBUNTU		0x090A
#define PVS_GUEST_VER_LIN_SLES9			0x090B
#define PVS_GUEST_VER_LIN_RHLES3		0x090C
#define PVS_GUEST_VER_LIN_CENTOS		0x090D
#define PVS_GUEST_VER_LIN_RH_LEGACY		0x090E
#define PVS_GUEST_VER_LIN_OPENSUSE		0x090F
#define PVS_GUEST_VER_LIN_PSBM			0x0910
#define PVS_GUEST_VER_LIN_MAGEIA		0x0911
#define PVS_GUEST_VER_LIN_MINT			0x0912
#define PVS_GUEST_VER_LIN_REDHAT_7		0x0913
#define PVS_GUEST_VER_LIN_CENTOS_7		0x0914
#define PVS_GUEST_VER_LIN_VZLINUX		0x0915
#define PVS_GUEST_VER_LIN_VZLINUX_7		0x0916
#define PVS_GUEST_VER_LIN_SLES11		0x0917
#define PVS_GUEST_VER_LIN_SLES12		0x0918
#define PVS_GUEST_VER_LIN_CLOUDLINUX		0x0919
#define PVS_GUEST_VER_LIN_CLOUDLINUX_7		0x0920
// Add here a new OS version and assign it to PVS_GUEST_VER_LIN_LAST
#define PVS_GUEST_VER_LIN_LAST			PVS_GUEST_VER_LIN_CLOUDLINUX_7
#define PVS_GUEST_VER_LIN_OTHER			0x09FF
#define IS_VALID_LINUX_VERSION(ver)											  \
	(((ver) >= PVS_GUEST_VER_LIN_REDHAT && (ver) <= PVS_GUEST_VER_LIN_LAST) ||\
     (ver) == PVS_GUEST_VER_LIN_OTHER || IS_VALID_CHROMEOS_VERSION(ver) ||\
	 IS_VALID_ANDROID_VERSION(ver))

#define PVS_GUEST_VER_BSD_4X			0x0A01
#define PVS_GUEST_VER_BSD_5X			0x0A02
#define PVS_GUEST_VER_BSD_6X			0x0A03
#define PVS_GUEST_VER_BSD_7X			0x0A04
#define PVS_GUEST_VER_BSD_8X			0x0A05
// Add here a new OS version and assign it to PVS_GUEST_VER_BSD_LAST
#define PVS_GUEST_VER_BSD_LAST			PVS_GUEST_VER_BSD_8X
#define PVS_GUEST_VER_BSD_OTHER			0x0AFF
#define IS_VALID_FREEBSD_VERSION(ver)										\
	(((ver) >= PVS_GUEST_VER_BSD_4X && (ver) <= PVS_GUEST_VER_BSD_LAST) ||	\
	 (ver) == PVS_GUEST_VER_BSD_OTHER)

#define PVS_GUEST_VER_OS2_WARP3			0x0B01
#define PVS_GUEST_VER_OS2_WARP4			0x0B02
#define PVS_GUEST_VER_OS2_WARP45		0x0B03
#define PVS_GUEST_VER_OS2_ECS11			0x0B04
#define PVS_GUEST_VER_OS2_ECS12			0x0B05
// Add here a new OS version and assign it to PVS_GUEST_VER_OS2_LAST
#define PVS_GUEST_VER_OS2_LAST			PVS_GUEST_VER_OS2_ECS12
#define PVS_GUEST_VER_OS2_OTHER			0x0BFF
#define IS_VALID_OS2_VERSION(ver)											\
	(((ver) >= PVS_GUEST_VER_OS2_WARP3 && (ver) <= PVS_GUEST_VER_OS2_LAST)||\
	 (ver) == PVS_GUEST_VER_OS2_OTHER)

#define PVS_GUEST_VER_DOS_MS622			0x0C01
// Add here a new OS version and assign it to PVS_GUEST_VER_DOS_LAST
#define PVS_GUEST_VER_DOS_LAST			PVS_GUEST_VER_DOS_MS622
#define PVS_GUEST_VER_DOS_OTHER			0x0CFF
#define IS_VALID_MSDOS_VERSION(ver)											\
	(((ver) >= PVS_GUEST_VER_DOS_MS622 && (ver) <= PVS_GUEST_VER_DOS_LAST)||\
	 (ver) == PVS_GUEST_VER_DOS_OTHER)

#define PVS_GUEST_VER_NET_4X			0x0D01
#define PVS_GUEST_VER_NET_5X			0x0D02
#define PVS_GUEST_VER_NET_6X			0x0D03
// Add here a new OS version and assign it to PVS_GUEST_VER_NET_LAST
#define PVS_GUEST_VER_NET_LAST			PVS_GUEST_VER_NET_6X
#define PVS_GUEST_VER_NET_OTHER			0x0DFF
#define IS_VALID_NETWARE_VERSION(ver)										\
	(((ver) >= PVS_GUEST_VER_NET_4X && (ver) <= PVS_GUEST_VER_NET_LAST) ||	\
	 (ver) == PVS_GUEST_VER_NET_OTHER)

#define PVS_GUEST_VER_SOL_9				0x0E01
#define PVS_GUEST_VER_SOL_10			0x0E02
#define PVS_GUEST_VER_SOL_11			0x0E03
#define PVS_GUEST_VER_SOL_OPEN			0x0E04 // OpenSolaris

	// Add here a new OS version and assign it to PVS_GUEST_VER_SOL_LAST
#define PVS_GUEST_VER_SOL_LAST			PVS_GUEST_VER_SOL_OPEN
#define PVS_GUEST_VER_SOL_OTHER			0x0EFF
#define IS_VALID_SOLARIS_VERSION(ver)										\
	(((ver) >= PVS_GUEST_VER_SOL_9 && (ver) <= PVS_GUEST_VER_SOL_LAST) ||	\
	 (ver) == PVS_GUEST_VER_SOL_OTHER)

#define PVS_GUEST_VER_OTH_QNX			0xFF01
#define PVS_GUEST_VER_OTH_OPENSTEP		0xFF02
// Add here a new OS version and assign it to PVS_GUEST_VER_OTH_LAST
#define PVS_GUEST_VER_OTH_LAST			PVS_GUEST_VER_OTH_OPENSTEP
#define PVS_GUEST_VER_OTH_OTHER			0xFFFF
#define IS_VALID_OTHER_OS_VERSION(ver)										\
	(((ver) >= PVS_GUEST_VER_OTH_QNX && (ver) <= PVS_GUEST_VER_OTH_LAST) ||	\
	 (ver) == PVS_GUEST_VER_OTH_OTHER)

#define PVS_GET_GUEST_TYPE(v)			(((v) >> 8) & 0xff)
#define PVS_GET_GUEST_NUMBER(v)			((v) & 0xff)
#define PVS_MAKE_GUEST_VER(t,n)			((((t) << 8) & 0xff00) | ((n) & 0xff))

#define IS_MACOS(x)				(!(((x) >> 8) ^ PVS_GUEST_TYPE_MACOS))
#define IS_WINDOWS(x)			(!(((x) >> 8) ^ PVS_GUEST_TYPE_WINDOWS))
#define IS_CHROMEOS(x)			(!(((x) >> 8) ^ PVS_GUEST_TYPE_CHROMEOS))
#define IS_ANDROID(x)			(!(((x) >> 8) ^ PVS_GUEST_TYPE_ANDROID))
#define IS_LINUX(x)				(!(((x) >> 8) ^ PVS_GUEST_TYPE_LINUX) ||	\
								 IS_CHROMEOS(x) || IS_ANDROID(x))
#define IS_FREEBSD(x)			(!(((x) >> 8) ^ PVS_GUEST_TYPE_FREEBSD))
#define IS_OS2(x)				(!(((x) >> 8) ^ PVS_GUEST_TYPE_OS2))
#define IS_MSDOS(x)				(!(((x) >> 8) ^ PVS_GUEST_TYPE_MSDOS))
#define IS_NETWARE(x)			(!(((x) >> 8) ^ PVS_GUEST_TYPE_NETWARE))
#define IS_SOLARIS(x)			(!(((x) >> 8) ^ PVS_GUEST_TYPE_SOLARIS))
#define IS_OTHER(x)				(!(((x) >> 8) ^ PVS_GUEST_TYPE_OTHER))

//
// Autodetected OS Architecture
//
#define PVS_GUEST_ARCH_OTHER	(0)
#define PVS_GUEST_ARCH_I386		(1)
#define PVS_GUEST_ARCH_X86_64	(2)

// Guest OS type names
#define PVS_GUEST_TYPE_NAME_MACOS			"Mac OS X"
#define PVS_GUEST_TYPE_NAME_WINDOWS			"Windows"
#define PVS_GUEST_TYPE_NAME_LINUX			"Linux"
#define PVS_GUEST_TYPE_NAME_FREEBSD			"FreeBSD"
#define PVS_GUEST_TYPE_NAME_CHROMEOS		"Chrome OS"
#define PVS_GUEST_TYPE_NAME_ANDROID			"Android"
#define PVS_GUEST_TYPE_NAME_OS2				"OS/2"
#define PVS_GUEST_TYPE_NAME_MSDOS			"MS-DOS"
#define PVS_GUEST_TYPE_NAME_NETWARE			"NetWare"
#define PVS_GUEST_TYPE_NAME_SOLARIS			"Solaris"
#define PVS_GUEST_TYPE_NAME_OTHER			"Other"

// Guest OS version names
#define PVS_GUEST_VER_NAME_MACOS_TIGER			"Mac OS X"
#define PVS_GUEST_VER_NAME_MACOS_LEOPARD		"Mac OS X"
#define PVS_GUEST_VER_NAME_MACOS_SNOW_LEOPARD	"Mac OS X"

#define PVS_GUEST_VER_NAME_WIN_311			"Windows 3.11"
#define PVS_GUEST_VER_NAME_WIN_95			"Windows 95"
#define PVS_GUEST_VER_NAME_WIN_98			"Windows 98"
#define PVS_GUEST_VER_NAME_WIN_ME			"Windows ME"
#define PVS_GUEST_VER_NAME_WIN_NT			"Windows NT"
#define PVS_GUEST_VER_NAME_WIN_2K			"Windows 2000"
#define PVS_GUEST_VER_NAME_WIN_XP			"Windows XP"
#define PVS_GUEST_VER_NAME_WIN_2003			"Windows Server 2003"
#define PVS_GUEST_VER_NAME_WIN_VISTA		"Windows Vista"
#define PVS_GUEST_VER_NAME_WIN_2008			"Windows Server 2008"
#define PVS_GUEST_VER_NAME_WIN_WINDOWS7		"Windows 7"
#define PVS_GUEST_VER_NAME_WIN_WINDOWS8		"Windows 8"
#define PVS_GUEST_VER_NAME_WIN_2012			"Windows Server 2012"
#define PVS_GUEST_VER_NAME_WIN_WINDOWS8_1	"Windows 8.1"
#define PVS_GUEST_VER_NAME_WIN_2016			"Windows Server 2016"
#define PVS_GUEST_VER_NAME_WIN_2019			"Windows Server 2019"
#define PVS_GUEST_VER_NAME_WIN_OTHER		"Other Windows"

#define PVS_GUEST_VER_NAME_LIN_REDHAT		"Red Hat Enterprise Linux"
#define PVS_GUEST_VER_NAME_LIN_SUSE			"SUSE Linux Enterprise"
#define PVS_GUEST_VER_NAME_LIN_MANDRAKE		"Mandriva Linux"
#define PVS_GUEST_VER_NAME_LIN_KRNL_24		"Other Linux kernel 2.4"
#define PVS_GUEST_VER_NAME_LIN_KRNL_26		"Other Linux kernel 2.6"
#define PVS_GUEST_VER_NAME_LIN_DEBIAN		"Debian GNU/Linux"
#define PVS_GUEST_VER_NAME_LIN_FEDORA		"Fedora Linux"
#define PVS_GUEST_VER_NAME_LIN_FEDORA_5		"Fedora Core 5 Linux"
#define PVS_GUEST_VER_NAME_LIN_XANDROS		"Xandros Linux"
#define PVS_GUEST_VER_NAME_LIN_UBUNTU		"Ubuntu Linux"
#define PVS_GUEST_VER_NAME_LIN_SLES9		"SUSE Linux Enterprise Server 9"
#define PVS_GUEST_VER_NAME_LIN_RHLES3		"Red Hat Enterprise Server 3"
#define PVS_GUEST_VER_NAME_LIN_CENTOS		"CentOS Linux"
#define PVS_GUEST_VER_NAME_LIN_VZLINUX		"Virtuozzo Linux"
#define PVS_GUEST_VER_NAME_LIN_CLOUDLINUX	"CloudLinux"
#define PVS_GUEST_VER_NAME_LIN_RH_LEGACY	"Red Hat Linux"
#define PVS_GUEST_VER_NAME_LIN_OPENSUSE		"OpenSUSE Linux"
#define PVS_GUEST_VER_NAME_LIN_PSBM			"Parallels Cloud Server"
#define PVS_GUEST_VER_NAME_LIN_MAGEIA		"Mageia Linux"
#define PVS_GUEST_VER_NAME_LIN_MINT			"Mint Linux"
#define PVS_GUEST_VER_NAME_LIN_OTHER		"Other Linux"

#define PVS_GUEST_VER_NAME_BSD_4X			"FreeBSD 4.x"
#define PVS_GUEST_VER_NAME_BSD_5X			"FreeBSD 5.x"
#define PVS_GUEST_VER_NAME_BSD_6X			"FreeBSD 6.x"
#define PVS_GUEST_VER_NAME_BSD_7X			"FreeBSD 7.x"
#define PVS_GUEST_VER_NAME_BSD_8X			"FreeBSD 8.x"
#define PVS_GUEST_VER_NAME_BSD_OTHER		"Other FreeBSD"


#define PVS_GUEST_VER_NAME_OS2_WARP3		"OS/2 Warp 3"
#define PVS_GUEST_VER_NAME_OS2_WARP4		"OS/2 Warp 4"
#define PVS_GUEST_VER_NAME_OS2_WARP45		"OS/2 Warp 4.5"
#define PVS_GUEST_VER_NAME_OS2_ECS11		"eComStation 1.1"
#define PVS_GUEST_VER_NAME_OS2_ECS12		"eComStation 1.2"
#define PVS_GUEST_VER_NAME_OS2_OTHER		"Other OS/2"

#define PVS_GUEST_VER_NAME_DOS_MS622		"MS-DOS 6.22"
#define PVS_GUEST_VER_NAME_DOS_OTHER		"Other DOS"

#define PVS_GUEST_VER_NAME_NET_4X			"NetWare 4.x"
#define PVS_GUEST_VER_NAME_NET_5X			"NetWare 5.x"
#define PVS_GUEST_VER_NAME_NET_6X			"NetWare 6.x"
#define PVS_GUEST_VER_NAME_NET_OTHER		"Other NetWare"

#define PVS_GUEST_VER_NAME_SOL_9			"Solaris 9"
#define PVS_GUEST_VER_NAME_SOL_10			"Solaris 10"
#define PVS_GUEST_VER_NAME_SOL_11			"Solaris 11"
#define PVS_GUEST_VER_NAME_SOL_OPEN			"OpenSolaris"
#define PVS_GUEST_VER_NAME_SOL_OTHER		"Other Solaris"

#define PVS_GUEST_VER_NAME_CHROMEOS_1x		"Chrome OS"
#define PVS_GUEST_VER_NAME_CHROMEOS_OTHER	"Other Chrome OS"

#define PVS_GUEST_VER_NAME_ANDROID_4_0		"Android"
#define PVS_GUEST_VER_NAME_ANDROID_OTHER	"Other Android"

#define PVS_GUEST_VER_NAME_OTH_QNX			"QNX"
#define PVS_GUEST_VER_NAME_OTH_OPENSTEP		"OpenStep"
#define PVS_GUEST_VER_NAME_OTH_OTHER		"Other"

/**
 * Utility function for identifier -> string conversion.
 */
PRL_UNUSED static const char* PVS_GUEST_TO_STRING( unsigned int id )
{
	switch ( id )
	{
	case PVS_GUEST_TYPE_MACOS:			return PVS_GUEST_TYPE_NAME_MACOS;
	case PVS_GUEST_TYPE_WINDOWS:		return PVS_GUEST_TYPE_NAME_WINDOWS;
	case PVS_GUEST_TYPE_LINUX:			return PVS_GUEST_TYPE_NAME_LINUX;
	case PVS_GUEST_TYPE_FREEBSD:		return PVS_GUEST_TYPE_NAME_FREEBSD;
	case PVS_GUEST_TYPE_CHROMEOS:		return PVS_GUEST_TYPE_NAME_CHROMEOS;
	case PVS_GUEST_TYPE_ANDROID:		return PVS_GUEST_TYPE_NAME_ANDROID;
	case PVS_GUEST_TYPE_OS2:			return PVS_GUEST_TYPE_NAME_OS2;
	case PVS_GUEST_TYPE_MSDOS:			return PVS_GUEST_TYPE_NAME_MSDOS;
	case PVS_GUEST_TYPE_NETWARE:		return PVS_GUEST_TYPE_NAME_NETWARE;
	case PVS_GUEST_TYPE_SOLARIS:		return PVS_GUEST_TYPE_NAME_SOLARIS;
	case PVS_GUEST_TYPE_OTHER:			return PVS_GUEST_TYPE_NAME_OTHER;

	case PVS_GUEST_VER_MACOS_TIGER:		return PVS_GUEST_VER_NAME_MACOS_TIGER;
	case PVS_GUEST_VER_MACOS_LEOPARD:	return PVS_GUEST_VER_NAME_MACOS_LEOPARD;
	case PVS_GUEST_VER_MACOS_SNOW_LEOPARD:	return PVS_GUEST_VER_NAME_MACOS_SNOW_LEOPARD;

	case PVS_GUEST_VER_WIN_311:			return PVS_GUEST_VER_NAME_WIN_311;
	case PVS_GUEST_VER_WIN_95:			return PVS_GUEST_VER_NAME_WIN_95;
	case PVS_GUEST_VER_WIN_98:			return PVS_GUEST_VER_NAME_WIN_98;
	case PVS_GUEST_VER_WIN_ME:			return PVS_GUEST_VER_NAME_WIN_ME;
	case PVS_GUEST_VER_WIN_NT:			return PVS_GUEST_VER_NAME_WIN_NT;
	case PVS_GUEST_VER_WIN_2K:			return PVS_GUEST_VER_NAME_WIN_2K;
	case PVS_GUEST_VER_WIN_XP:			return PVS_GUEST_VER_NAME_WIN_XP;
	case PVS_GUEST_VER_WIN_2003:		return PVS_GUEST_VER_NAME_WIN_2003;
	case PVS_GUEST_VER_WIN_VISTA:		return PVS_GUEST_VER_NAME_WIN_VISTA;
	case PVS_GUEST_VER_WIN_2008:		return PVS_GUEST_VER_NAME_WIN_2008;
	case PVS_GUEST_VER_WIN_WINDOWS7:	return PVS_GUEST_VER_NAME_WIN_WINDOWS7;
	case PVS_GUEST_VER_WIN_WINDOWS8:	return PVS_GUEST_VER_NAME_WIN_WINDOWS8;
	case PVS_GUEST_VER_WIN_2012:		return PVS_GUEST_VER_NAME_WIN_2012;
	case PVS_GUEST_VER_WIN_WINDOWS8_1:	return PVS_GUEST_VER_NAME_WIN_WINDOWS8_1;
	case PVS_GUEST_VER_WIN_2016:		return PVS_GUEST_VER_NAME_WIN_2016;
	case PVS_GUEST_VER_WIN_2019:		return PVS_GUEST_VER_NAME_WIN_2019;
	case PVS_GUEST_VER_WIN_OTHER:		return PVS_GUEST_VER_NAME_WIN_OTHER;

	case PVS_GUEST_VER_LIN_REDHAT:		return PVS_GUEST_VER_NAME_LIN_REDHAT;
	case PVS_GUEST_VER_LIN_REDHAT_7:	return PVS_GUEST_VER_NAME_LIN_REDHAT;
	case PVS_GUEST_VER_LIN_SUSE:		return PVS_GUEST_VER_NAME_LIN_SUSE;
	case PVS_GUEST_VER_LIN_MANDRAKE:	return PVS_GUEST_VER_NAME_LIN_MANDRAKE;
	case PVS_GUEST_VER_LIN_KRNL_24:		return PVS_GUEST_VER_NAME_LIN_KRNL_24;
	case PVS_GUEST_VER_LIN_KRNL_26:		return PVS_GUEST_VER_NAME_LIN_KRNL_26;
	case PVS_GUEST_VER_LIN_DEBIAN:		return PVS_GUEST_VER_NAME_LIN_DEBIAN;
	case PVS_GUEST_VER_LIN_FEDORA:		return PVS_GUEST_VER_NAME_LIN_FEDORA;
	case PVS_GUEST_VER_LIN_FEDORA_5:	return PVS_GUEST_VER_NAME_LIN_FEDORA_5;
	case PVS_GUEST_VER_LIN_XANDROS:		return PVS_GUEST_VER_NAME_LIN_XANDROS;
	case PVS_GUEST_VER_LIN_UBUNTU:		return PVS_GUEST_VER_NAME_LIN_UBUNTU;
	case PVS_GUEST_VER_LIN_SLES9:		return PVS_GUEST_VER_NAME_LIN_SLES9;
	case PVS_GUEST_VER_LIN_RHLES3:		return PVS_GUEST_VER_NAME_LIN_RHLES3;
	case PVS_GUEST_VER_LIN_CENTOS:		return PVS_GUEST_VER_NAME_LIN_CENTOS;
	case PVS_GUEST_VER_LIN_CENTOS_7:	return PVS_GUEST_VER_NAME_LIN_CENTOS;
	case PVS_GUEST_VER_LIN_VZLINUX:		return PVS_GUEST_VER_NAME_LIN_VZLINUX;
	case PVS_GUEST_VER_LIN_VZLINUX_7:	return PVS_GUEST_VER_NAME_LIN_VZLINUX;
	case PVS_GUEST_VER_LIN_CLOUDLINUX:	return PVS_GUEST_VER_NAME_LIN_CLOUDLINUX;
	case PVS_GUEST_VER_LIN_CLOUDLINUX_7:	return PVS_GUEST_VER_NAME_LIN_CLOUDLINUX;
	case PVS_GUEST_VER_LIN_RH_LEGACY:	return PVS_GUEST_VER_NAME_LIN_RH_LEGACY;
	case PVS_GUEST_VER_LIN_OPENSUSE:	return PVS_GUEST_VER_NAME_LIN_OPENSUSE;
	case PVS_GUEST_VER_LIN_PSBM:		return PVS_GUEST_VER_NAME_LIN_PSBM;
	case PVS_GUEST_VER_LIN_MAGEIA:		return PVS_GUEST_VER_NAME_LIN_MAGEIA;
	case PVS_GUEST_VER_LIN_MINT:		return PVS_GUEST_VER_NAME_LIN_MINT;
	case PVS_GUEST_VER_LIN_OTHER:		return PVS_GUEST_VER_NAME_LIN_OTHER;

	case PVS_GUEST_VER_BSD_4X:			return PVS_GUEST_VER_NAME_BSD_4X;
	case PVS_GUEST_VER_BSD_5X:			return PVS_GUEST_VER_NAME_BSD_5X;
	case PVS_GUEST_VER_BSD_6X:			return PVS_GUEST_VER_NAME_BSD_6X;
	case PVS_GUEST_VER_BSD_7X:			return PVS_GUEST_VER_NAME_BSD_7X;
	case PVS_GUEST_VER_BSD_8X:			return PVS_GUEST_VER_NAME_BSD_8X;
	case PVS_GUEST_VER_BSD_OTHER:		return PVS_GUEST_VER_NAME_BSD_OTHER;

	case PVS_GUEST_VER_OS2_WARP3:		return PVS_GUEST_VER_NAME_OS2_WARP3;
	case PVS_GUEST_VER_OS2_WARP4:		return PVS_GUEST_VER_NAME_OS2_WARP4;
	case PVS_GUEST_VER_OS2_WARP45:		return PVS_GUEST_VER_NAME_OS2_WARP45;
	case PVS_GUEST_VER_OS2_ECS11:		return PVS_GUEST_VER_NAME_OS2_ECS11;
	case PVS_GUEST_VER_OS2_ECS12:		return PVS_GUEST_VER_NAME_OS2_ECS12;
	case PVS_GUEST_VER_OS2_OTHER:		return PVS_GUEST_VER_NAME_OS2_OTHER;

	case PVS_GUEST_VER_DOS_MS622:		return PVS_GUEST_VER_NAME_DOS_MS622;
	case PVS_GUEST_VER_DOS_OTHER:		return PVS_GUEST_VER_NAME_DOS_OTHER;

	case PVS_GUEST_VER_NET_4X:			return PVS_GUEST_VER_NAME_NET_4X;
	case PVS_GUEST_VER_NET_5X:			return PVS_GUEST_VER_NAME_NET_5X;
	case PVS_GUEST_VER_NET_6X:			return PVS_GUEST_VER_NAME_NET_6X;
	case PVS_GUEST_VER_NET_OTHER:		return PVS_GUEST_VER_NAME_NET_OTHER;

	case PVS_GUEST_VER_SOL_9:			return PVS_GUEST_VER_NAME_SOL_9;
	case PVS_GUEST_VER_SOL_10:			return PVS_GUEST_VER_NAME_SOL_10;
	case PVS_GUEST_VER_SOL_11:			return PVS_GUEST_VER_NAME_SOL_11;
	case PVS_GUEST_VER_SOL_OTHER:		return PVS_GUEST_VER_NAME_SOL_OTHER;

	case PVS_GUEST_VER_CHROMEOS_1x:		return PVS_GUEST_VER_NAME_CHROMEOS_1x;
	case PVS_GUEST_VER_CHROMEOS_OTHER:	return PVS_GUEST_VER_NAME_CHROMEOS_OTHER;

	case PVS_GUEST_VER_OTH_QNX:			return PVS_GUEST_VER_NAME_OTH_QNX;
	case PVS_GUEST_VER_OTH_OPENSTEP:	return PVS_GUEST_VER_NAME_OTH_OPENSTEP;
	case PVS_GUEST_VER_OTH_OTHER:		return PVS_GUEST_VER_NAME_OTH_OTHER;
	}

	return "unknown";
}

#endif // __PARALLELS_API_OSES_H__
