/*
 * OsInfo.h: OS information routines and types
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


#ifndef PRL_COMMON_UTILS_OS_INFO_INCLUDED
#define PRL_COMMON_UTILS_OS_INFO_INCLUDED

#include "Interfaces/ParallelsTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// constants

///	@name OS architecture
//@{

///	@brief OS architecture constants
/**
* @sa osInfo_getArchitecture()
*/
typedef enum _OSINFO_ARCHITECTURE
{
	///	@brief OS architecture is unknown (or just failed to detect)
	OSINFO_ARCHITECTURE_UNKNOWN	= 0,
	///	@brief It's 32bit OS
	OSINFO_ARCHITECTURE_32_BIT	= 32,
	///	@brief It's 64bit OS
	OSINFO_ARCHITECTURE_64_BIT	= 64
}
OSINFO_ARCHITECTURE;

//@}


///	@name OS types
//@{

extern const char OSINFO_TYPE_MAC_STR[];
extern const char OSINFO_TYPE_WIN_STR[];
extern const char OSINFO_TYPE_LIN_STR[];
extern const char OSINFO_TYPE_UNKNOWN_STR[];

//@}



////////////////////////////////////////////////////////////////////////////////
// types

///	@brief Structure holds common information for all OS's
/**
* @sa osInfo_getCommonInformation()
*/
typedef struct _OSINFO_COMMON_INFORMATION
{
	///	@brief Type of OS
	/**
	* This must be one of the PVS_GUEST_TYPE_XXX constants from
	* SDK/Include/PrlOses.h
	*/
	UINT32 type;

	///	@brief OS architecture
	/**
	* This can be on of the PRL_OS_ARCHITECTURE constants.
	*
	* @sa osInfo_getOSArchitecture()
	*/
	UINT32 arch;

	///	@brief Number of CPU's that OS is using
	/**
	* It can be 0, if failed to detect number of CPU used.
	*/
	UINT32 cpus;

	///	@brief Size of system page
	/**
	* It can be 0 if failed to detect system page size.
	*/
	UINT32 pageSize;

	///	@brief Reserved for future use (must be 0)
	UINT32 reserved0;

	///	@brief Reserved for future use (must be 0)
	UINT32 reserved1;

	///	@brief Reserved for future use (must be 0)
	UINT32 reserved2;

	//	Reserved for future use (must be 0)
	//	Last reserved were removed and added to OSINFO_FULL_INFORMATION structure
	//	for versioning support
} OSINFO_COMMON_INFORMATION;


///	@brief Microsoft Windows specific information
/**
* @sa osInfo_getWinInformation()
*/
typedef struct _OSINFO_WIN_INFORMATION
{
	///	@brief Analog of OSVERSIONINFO::dwMajorVersion
	UINT32 verMajor;

	///	@brief Analog of OSVERSIONINFO::dwMinorVersion
	UINT32 verMinor;

	///	@brief Analog of OSVERSIONINFO::dwBuildNumber
	UINT32 verBuild;

	///	@brief Analog of OSVERSIONINFO::szCSDVersion but in utf-8 encoding
	char verCSD[256];
}
OSINFO_WIN_INFORMATION;

typedef struct _OSINFO_WIN_INFORMATION_V2
{
	///	@brief Analog of OSVERSIONINFO::dwMajorVersion
	UINT32 verMajor;

	///	@brief Analog of OSVERSIONINFO::dwMinorVersion
	UINT32 verMinor;

	///	@brief Analog of OSVERSIONINFO::dwBuildNumber
	UINT32 verBuild;

	///	@brief Analog of OSVERSIONINFO::szCSDVersion but in utf-8 encoding
	char verCSD[256];

	/// @brief Analog of OSVERSIONINFOEX::wServicePackMajor
	UINT16 servicePackMajor;

	/// @brief Analog of OSVERSIONINFOEX::wServicePackMinor
	UINT16 servicePackMinor;

	/// @brief Analog of OSVERSIONINFOEX::wSuiteMask
	UINT16 suiteMask;

	/// @brief Analog of OSVERSIONINFOEX::wProductType
	UINT8  productType;

	/// @brief Analog of pdwReturnedProductType param return from GetProductInfo()
	UINT32 productEdition;
}
OSINFO_WIN_INFORMATION_V2;


#define LIN_RELEASE_LENGTH	65

///	@brief Linux specific information
/**
* @sa osInfo_getLinInformation()
*/
typedef struct _OSINFO_LIN_INFORMATION
{
	///	@brief Release level information
	char release[LIN_RELEASE_LENGTH];
}
OSINFO_LIN_INFORMATION;

///	@brief MAC OS specific information
/**
* @sa osInfo_getMacInformation()
*/

/**
*  maximum size of the field is 14 (its platform version), so even 16 is
*  enough.
*/
#define MAC_RELEASE_LENGTH 64

typedef struct _OSINFO_MAC_INFORMATION
{
	char build[MAC_RELEASE_LENGTH];
	char platform[MAC_RELEASE_LENGTH];
	unsigned int verMaj, verMin, verBugfix;
}
OSINFO_MAC_INFORMATION;

///	@brief OS information specific for each OS
/**
* @sa OSINFO_FULL_INFORMATION
*/
typedef union _OSINFO_SPEC_INFORMATION
{
	///	@brief Information specific for Microsoft Windows OS
	OSINFO_WIN_INFORMATION win;

	/// @brief Information specific for Microsoft Windows OS extended
	//	with OS edition and SP version.
	OSINFO_WIN_INFORMATION_V2 win2;

	/// @brief Information specific for Linux OS
	OSINFO_LIN_INFORMATION lin;

	/// @brief Information specific for Mac OS
	OSINFO_MAC_INFORMATION mac;
}
OSINFO_SPEC_INFORMATION;


///	@brief Structure holds full information about OS
/**
* @sa osInfo_getFullInformation()
*/
typedef struct _OSINFO_FULL_INFORMATION
{
	///	@brief Size of structure
	/**
	* Must be set to sizeof(OSINFO_FULL_INFORMATION).
	*/
	UINT32 size;

	///	@brief Common information
	OSINFO_COMMON_INFORMATION common;

	/// @brief Size of spec part
	//	Added for version support. This uint32 were zeroed part
	//	of OSINFO_COMMON_INFORMATION structure.
	//	If specSize is 0 then spec is of old version, if not - new one.
	UINT32 specSize;

	///	@brief OS information specific for each OS
	/**
	* Proper  union member must be selected based on <i>common.type</i>
	* member.
	*/
	OSINFO_SPEC_INFORMATION spec;
}
OSINFO_FULL_INFORMATION;


////////////////////////////////////////////////////////////////////////////////
// functions

///	@name Getting OS information
//@{

///	@brief Returns "number of bits in" OS architecture (see
//	PRL_OS_ARCHITECTURE enum)
OSINFO_ARCHITECTURE osInfo_getArchitecture();

///	@brief Returns OS type as one of the PVS_GUEST_TYPE_XXX constants from
///	SDK/Include/PrlOses.h
UINT32 osInfo_getType();

///	@brief Gathers information common for all OS's
void osInfo_getCommonInformation(OSINFO_COMMON_INFORMATION *const common);

#if defined(_WIN_)
///	@brief Gathers Microsoft Windows specific information
void osInfo_getWinInformation(OSINFO_WIN_INFORMATION_V2 *const win);
#endif

///	@brief Gathers OS specific information
void osInfo_getSpecInformation(OSINFO_SPEC_INFORMATION *const spec, UINT32 *const specSize);

///	@brief Gathers all available information about OS
BOOL osInfo_getFullInformation(OSINFO_FULL_INFORMATION *const full);

//@}


///	@name Representing OS information
//@{

///	@brief Represents common OS information as string
const char* osInfo_commonOSString(const OSINFO_COMMON_INFORMATION *const common);

//@}

unsigned int osInfo_getMinRecommendedOsMemorySize(unsigned int nOsVersion);

// @return result in Mb
unsigned int osInfo_getRequiredOsDiskSpace(unsigned int nOsVersion, int arch);

#ifdef __cplusplus
}
#endif


#endif	// PRL_COMMON_UTILS_OS_INFO_INCLUDED
