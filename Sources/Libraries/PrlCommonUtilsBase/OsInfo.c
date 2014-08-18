/*
 * OsInfo.c: OS information routines
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



////////////////////////////////////////////////////////////////////////////////
// includes
#include <string.h>

#if defined(_WIN_)
	#include <windows.h>
#elif defined(_LIN_)
	#include <unistd.h>
	#include <sys/utsname.h>
#endif

#include "SDK/Include/PrlOses.h"
#include "OsInfo.h"



////////////////////////////////////////////////////////////////////////////////
// constants

// OS types --------------------------------------------------------------------

const char OSINFO_TYPE_WIN_STR[]		= "Microsoft Windows";
const char OSINFO_TYPE_LIN_STR[]		= "GNU Linux";
const char OSINFO_TYPE_UNKNOWN_STR[]	= "Unknown OS";



////////////////////////////////////////////////////////////////////////////////
// functions

// Getting OS information ------------------------------------------------------
/**
* @return One of the OSINFO_ARCHITECTURE enum members
*
* @sa OSINFO_ARCHITECTURE
*/
OSINFO_ARCHITECTURE osInfo_getArchitecture()
{
#if defined(_64BIT_)

	// If we are 64bit, OS also 64bit
	return OSINFO_ARCHITECTURE_64_BIT;

#elif defined(_32BIT_) && defined(_WIN_)

	// Get address of IsWow64Process() function
	typedef BOOL (WINAPI *IsWow64Process_t)(HANDLE, PBOOL);

	const IsWow64Process_t IsWow64Process_f =
			(IsWow64Process_t)GetProcAddress(GetModuleHandleW(L"kernel32"),
											 "IsWow64Process");

	// We can have no such function on old 32bit MS Windows
	if (0 == IsWow64Process_f) return OSINFO_ARCHITECTURE_32_BIT;

	{
		// Lets be 32bit by default
		BOOL isWow64 = FALSE;

		// Check, if we are World of Warcraft 64bit proccess =)
		if (0 == IsWow64Process_f(GetCurrentProcess(), &isWow64))
		{
			// We failed to obtain a result, so we don't know anything
			return OSINFO_ARCHITECTURE_UNKNOWN;
		}

		// Fine, we've got the result and now can return it
		return (isWow64)? OSINFO_ARCHITECTURE_64_BIT: OSINFO_ARCHITECTURE_32_BIT;
	}

#elif defined(_32BIT_) && (defined(_LIN_))

	const char       x64str[] = "x86_64";
	struct utsname   uts;

	if (uname(&uts) < 0)
	{
		return OSINFO_ARCHITECTURE_UNKNOWN;
	}
	else if (strcmp(uts.machine, x64str) == 0)
	{
		return OSINFO_ARCHITECTURE_64_BIT;
	}
	else
	{
		return OSINFO_ARCHITECTURE_32_BIT;
	}

#else

	// We will return 0 for unsupported platforms
	return OSINFO_ARCHITECTURE_UNKNOWN;

#endif
}


/**
* @return OS type as one of the PVS_GUEST_TYPE_XXX constants from
* SDK/Include/PrlOses.h
*/
UINT32 osInfo_getType()
{
#if defined(_WIN_)
	return PVS_GUEST_TYPE_WINDOWS;
#elif defined(_LIN_)
	return PVS_GUEST_TYPE_LINUX;
#else
	return PVS_GUEST_TYPE_OTHER;
#endif
}


/**
* @param[out] common Pointer on structure where common OS information will be
* placed. Can't be 0.
*/
void osInfo_getCommonInformation(OSINFO_COMMON_INFORMATION *const common)
{
#if defined(_WIN_)
	SYSTEM_INFO sysInfo;
#endif
	// Check function arguments ------------------------------------------------

	// Reset supplied structure
	memset(common, 0, sizeof(*common));

	// Set OS independent part -------------------------------------------------
	common->arch	= (UINT32)osInfo_getArchitecture();
	common->type	= osInfo_getType();

#if defined(_WIN_)
	GetSystemInfo(&sysInfo);
	common->pageSize	= (UINT32)sysInfo.dwPageSize;
	common->cpus		= (UINT32)sysInfo.dwNumberOfProcessors;
#elif defined(_LIN_)
	common->pageSize	= (UINT32)sysconf(_SC_PAGE_SIZE);
	common->cpus		= (UINT32)sysconf(_SC_NPROCESSORS_ONLN);
#endif
}


#if defined(_WIN_)
/**
* @param[out] win Pointer on structure where Microsoft Windows specific
* information will be placed. Can't be 0.
*/

typedef BOOL (WINAPI *GetProductInfo_t)(DWORD, DWORD, DWORD, DWORD, PDWORD);

void osInfo_getWinInformation(OSINFO_WIN_INFORMATION_V2 *const win)
{
	OSVERSIONINFOEXW osVerInfo;

	const GetProductInfo_t GetProductInfo_f = (GetProductInfo_t)GetProcAddress(
		GetModuleHandleW(L"kernel32"), "GetProductInfo");

	// Reset supplied structure
	memset(win, 0, sizeof(*win));

	// Information supplied by GetVersionExW() function ------------------------
	memset(&osVerInfo, 0, sizeof(osVerInfo));
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);

	if (0 != GetVersionExW((LPOSVERSIONINFOW)&osVerInfo))
	{
		DWORD edition = 0;
		// Version numbers
		win->verMajor	= (UINT32)osVerInfo.dwMajorVersion;
		win->verMinor	= (UINT32)osVerInfo.dwMinorVersion;
		win->verBuild	= (UINT32)osVerInfo.dwBuildNumber;

		// Version additional string
		// XXX handle [unlikely] failures
		WideCharToMultiByte(
			CP_UTF8, 0, osVerInfo.szCSDVersion, -1/*strlen*/,
			win->verCSD, sizeof(win->verCSD), NULL, NULL);

		win->servicePackMajor	= (UINT16)osVerInfo.wServicePackMajor;
		win->servicePackMinor	= (UINT16)osVerInfo.wServicePackMinor;
		win->suiteMask			= (UINT16)osVerInfo.wSuiteMask;
		win->productType		= (UINT8)osVerInfo.wProductType;

		if (0 != GetProductInfo_f &&
			0 != GetProductInfo_f(osVerInfo.dwMajorVersion, osVerInfo.dwMinorVersion,
			osVerInfo.wServicePackMajor, osVerInfo.wServicePackMinor, &edition))
		{
			win->productEdition = (UINT32)edition;
		}
	}
}
#endif


#if defined(_LIN_)
/**
* @param[out] lin Pointer on structure where Linux specific
* information will be placed. Can't be 0.
*/
void osInfo_getLinInformation(OSINFO_LIN_INFORMATION * const lin)
{
	struct utsname   uts;

	if (uname(&uts) >= 0)
	{
		// Copy string, but leave the place for '\0' terminating character
		strncpy(lin->release, uts.release, LIN_RELEASE_LENGTH - 1);

		// ... and this character
		lin->release[LIN_RELEASE_LENGTH - 1] = '\0';
	}
}
#endif // _LIN_

/**
* @param[out] spec Pointer on structure where OS specific information will be
* placed. Can't be 0.
* @param[out] specSize Size of structure filled with OS specific information
*/
void osInfo_getSpecInformation(OSINFO_SPEC_INFORMATION *const spec, UINT32 *const specSize)
{
	// Reset supplied structure
	memset(spec, 0, sizeof(*spec));
	*specSize = 0;

#if defined(_WIN_)
	*specSize = sizeof(spec->win2);
	osInfo_getWinInformation(&(spec->win2));
#elif defined(_LIN_)
	osInfo_getLinInformation(&(spec->lin));
#endif
}


/**
* @param[out] full Pointer on structure where full OS information will be
* placed. Can't be 0.
*
* @return <i>TRUE</i> if information was obtained, <i>FALSE</i> otherwise. The
* only reason to return <i>FALSE</i> is unsupported value in
* OSINFO_FULL_INFORMATION::size member.
*/
BOOL osInfo_getFullInformation(OSINFO_FULL_INFORMATION *const full)
{
	// Check function arguments ------------------------------------------------

	if (full->size != sizeof(*full)) return FALSE;

	// Obtain common information -----------------------------------------------
	osInfo_getCommonInformation(&(full->common));

	// Obtain information specific for OS --------------------------------------
	osInfo_getSpecInformation(&(full->spec), &(full->specSize));

	return TRUE;
}



// Representing OS information -------------------------------------------------

/**
* @param[in] common Pointer on structure with common OS information
*
* @return String representation of supplied structure
*
* @note This function is not finished yet, it is for testing purposes here.
*/
const char* osInfo_commonOSString(const OSINFO_COMMON_INFORMATION *const common)
{
	switch (common->type)
	{
	case PVS_GUEST_TYPE_WINDOWS:
		return OSINFO_TYPE_WIN_STR;
	case PVS_GUEST_TYPE_LINUX:
		return OSINFO_TYPE_LIN_STR;
	default:
		return OSINFO_TYPE_UNKNOWN_STR;
	}
}


unsigned int osInfo_getMinRecommendedOsMemorySize(unsigned int nOsVersion)
{
	switch ( nOsVersion )
	{
	case PVS_GUEST_VER_WIN_311:
	case PVS_GUEST_VER_WIN_OTHER:
	case PVS_GUEST_VER_DOS_MS622:
	case PVS_GUEST_VER_DOS_OTHER:
	case PVS_GUEST_VER_OS2_WARP3:
	case PVS_GUEST_VER_OS2_OTHER:
		return 64;

	case PVS_GUEST_VER_WIN_95:
	case PVS_GUEST_VER_WIN_98:
	case PVS_GUEST_VER_WIN_ME:
	case PVS_GUEST_VER_WIN_NT:
	case PVS_GUEST_VER_WIN_2K:
	case PVS_GUEST_VER_LIN_REDHAT:
	case PVS_GUEST_VER_LIN_SUSE:
	case PVS_GUEST_VER_LIN_MANDRAKE:
	case PVS_GUEST_VER_LIN_KRNL_24:
	case PVS_GUEST_VER_LIN_KRNL_26:
	case PVS_GUEST_VER_LIN_DEBIAN:
	case PVS_GUEST_VER_LIN_FEDORA:
	case PVS_GUEST_VER_LIN_FEDORA_5:
	case PVS_GUEST_VER_LIN_XANDROS:
	case PVS_GUEST_VER_LIN_UBUNTU:
	case PVS_GUEST_VER_LIN_SLES9:
	case PVS_GUEST_VER_LIN_RHLES3:
	case PVS_GUEST_VER_LIN_CENTOS:
	case PVS_GUEST_VER_LIN_PSBM:
	case PVS_GUEST_VER_LIN_RH_LEGACY:
	case PVS_GUEST_VER_LIN_OPENSUSE:
	case PVS_GUEST_VER_LIN_MAGEIA:
	case PVS_GUEST_VER_LIN_MINT:
	case PVS_GUEST_VER_LIN_OTHER:
	case PVS_GUEST_VER_OS2_WARP4:
	case PVS_GUEST_VER_OS2_WARP45:
	case PVS_GUEST_VER_OS2_ECS11:
	case PVS_GUEST_VER_OS2_ECS12:
	case PVS_GUEST_VER_CHROMEOS_1x:
	case PVS_GUEST_VER_ANDROID_4_0:
	case PVS_GUEST_VER_OTH_OPENSTEP:
	case PVS_GUEST_VER_OTH_OTHER:
		return 128;

	case PVS_GUEST_VER_NET_4X:
	case PVS_GUEST_VER_WIN_2008:
	case PVS_GUEST_VER_BSD_6X:
	case PVS_GUEST_VER_BSD_7X:
	case PVS_GUEST_VER_NET_OTHER:
		return 256;

	case PVS_GUEST_VER_NET_5X:
	case PVS_GUEST_VER_NET_6X:
	case PVS_GUEST_VER_WIN_VISTA:
	case PVS_GUEST_VER_WIN_WINDOWS7:
	case PVS_GUEST_VER_WIN_WINDOWS8:
	case PVS_GUEST_VER_WIN_WINDOWS8_1:
	case PVS_GUEST_VER_WIN_2012:
	case PVS_GUEST_VER_MACOS_TIGER:
	case PVS_GUEST_VER_MACOS_LEOPARD:
	case PVS_GUEST_VER_MACOS_SNOW_LEOPARD:
	case PVS_GUEST_VER_SOL_10:
	case PVS_GUEST_VER_SOL_9:
	case PVS_GUEST_VER_SOL_OTHER:
	case PVS_GUEST_VER_SOL_11:
		return 512;

	case PVS_GUEST_VER_WIN_XP:
	case PVS_GUEST_VER_WIN_2003:
	case PVS_GUEST_VER_BSD_4X:
	case PVS_GUEST_VER_BSD_5X:
	case PVS_GUEST_VER_BSD_OTHER:
	case PVS_GUEST_VER_OTH_QNX:
	default:
		;
	}

	return 256;
}

unsigned int osInfo_getRequiredOsDiskSpace(unsigned int nOsVersion, int arch)
{
	int x64 = (arch == PVS_GUEST_ARCH_X86_64);

	switch(nOsVersion)
	{
	case PVS_GUEST_VER_WIN_311:
		return 40;
	case PVS_GUEST_VER_WIN_95:
		return 100;
	case PVS_GUEST_VER_WIN_98:
		return 120;
	case PVS_GUEST_VER_WIN_ME:
		return 500;
	case PVS_GUEST_VER_WIN_NT:
		return 140;
	case PVS_GUEST_VER_WIN_2K:
		return 1024;
	case PVS_GUEST_VER_WIN_XP:
		return 1536;
	case PVS_GUEST_VER_WIN_2003:
		return 2 * 1024;
	case PVS_GUEST_VER_WIN_VISTA:
		return x64 ? (20 * 1024) : (15 * 1024);
	case PVS_GUEST_VER_WIN_2008:
		return 40 * 1024;
	case PVS_GUEST_VER_WIN_WINDOWS7:
		return x64 ? (20 * 1024) : (16 * 1024);
	case PVS_GUEST_VER_WIN_WINDOWS8:
		return x64 ? (20 * 1024) : (16 * 1024);
	case PVS_GUEST_VER_WIN_2012:
		return 60 * 1024;
	case PVS_GUEST_VER_WIN_WINDOWS8_1:
		return x64 ? (20 * 1024) : (16 * 1024);

	case PVS_GUEST_VER_MACOS_TIGER:
		return 3 * 1024;
	case PVS_GUEST_VER_MACOS_LEOPARD:
		return 9 * 1024;
	case PVS_GUEST_VER_MACOS_UNIVERSAL:
		return 8 * 1024;

	// Note: 64 bits Linux OS's installations were tested
	//       in express installation mode.
	case PVS_GUEST_VER_LIN_UBUNTU:	// 13.10
		return (unsigned int )(4.9 * 1024.0);
	case PVS_GUEST_VER_LIN_FEDORA:	// Fedora 20
		return (unsigned int )(3.8 * 1024.0);
	case PVS_GUEST_VER_LIN_REDHAT:	// RedHat 6.5
		return (unsigned int )(3.1 * 1024.0);
	case PVS_GUEST_VER_LIN_CENTOS:	// CentOS 6.4
		return (unsigned int )(4.2 * 1024.0);
	case PVS_GUEST_VER_LIN_PSBM:	// PCS 6.0.5
		return (unsigned int )(3.3 * 1024.0);

	default:
		;
	}

	return 0;
}
