/*
 * PrlIOStructs.h: This file is the part of parallels public sdk
 * library.
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


#ifndef __PARALLELS_IO_STRUCTS_H__
#define __PARALLELS_IO_STRUCTS_H__

#include "PrlTypes.h"
#include "PrlEnums.h"

#ifdef _WIN_
#pragma pack(push, save_pack, 1)
#endif

#define PRL_IO_MAX_DISPLAYS 16
#define PRL_IO_MAX_ENCODINGS 32
#define PRL_PROXY_MAX_HOSTS 16

#define PRL_IO_GAMMARAMP_DEFAULT 1
#define PRL_IO_GAMMARAMP_RGB256x3x16 2

typedef PRL_UINT32 PRL_IO_PALETTE[256];

typedef struct _PRL_IO_SCREEN_SIZE
{
	PRL_UINT32 width;
	PRL_UINT32 height;
	PRL_UINT32 depth;
	PRL_INT32 lineBytes;

} PRL_STRUCT( PRL_IO_SCREEN_SIZE );
typedef PRL_IO_SCREEN_SIZE* PRL_IO_SCREEN_SIZE_PTR;

typedef struct _PRL_IO_DISPLAY_SCREEN_SIZE
{
	PRL_UINT32 width;
	PRL_UINT32 height;
	PRL_UINT32 depth;
	PRL_INT32 lineBytes;
	PRL_UINT32 display;
	PRL_INT32 displayX;
	PRL_INT32 displayY;

} PRL_STRUCT( PRL_IO_DISPLAY_SCREEN_SIZE );
typedef PRL_IO_DISPLAY_SCREEN_SIZE* PRL_IO_DISPLAY_SCREEN_SIZE_PTR;

typedef struct _PRL_IO_DPI_DISPLAY_SCREEN_SIZE
{
	PRL_IO_DISPLAY_SCREEN_SIZE display;
	PRL_INT32	dpi;
	///	@brief See PRL_DISPLAY_CONFIG_MODE enum for possible 'flags' values
	PRL_UINT32	flags;
} PRL_STRUCT( PRL_IO_DPI_DISPLAY_SCREEN_SIZE );
typedef PRL_IO_DPI_DISPLAY_SCREEN_SIZE* PRL_IO_DPI_DISPLAY_SCREEN_SIZE_PTR;

typedef struct _PRL_IO_DISPLAY_PALETTE
{
	PRL_IO_PALETTE palette;
	PRL_UINT32 display;

} PRL_STRUCT( PRL_IO_DISPLAY_PALETTE );
typedef PRL_IO_DISPLAY_PALETTE* PRL_IO_DISPLAY_PALETTE_PTR;

typedef struct _PRL_IO_DISPLAY_GAMMA
{
	PRL_UINT32 display;
	PRL_UINT32 type;
	PRL_UINT32 gammaSize;
	PRL_UINT32 reserved;

} PRL_STRUCT( PRL_IO_DISPLAY_GAMMA );
typedef PRL_IO_DISPLAY_GAMMA* PRL_IO_DISPLAY_GAMMA_PTR;

typedef struct _PRL_IO_SCREEN_REGION
{
	PRL_UINT32 x;
	PRL_UINT32 y;
	PRL_UINT32 width;
	PRL_UINT32 height;

} PRL_STRUCT( PRL_IO_SCREEN_REGION );
typedef PRL_IO_SCREEN_REGION* PRL_IO_SCREEN_REGION_PTR;

typedef struct _PRL_IO_DISPLAY_SCREEN_REGION
{
	PRL_UINT32 x;
	PRL_UINT32 y;
	PRL_UINT32 width;
	PRL_UINT32 height;
	PRL_UINT32 display;

} PRL_STRUCT( PRL_IO_DISPLAY_SCREEN_REGION );
typedef PRL_IO_DISPLAY_SCREEN_REGION* PRL_IO_DISPLAY_SCREEN_REGION_PTR;

typedef struct _PRL_IO_SCREEN_BUFFER_REGION
{
	PRL_UINT32 x;
	PRL_UINT32 y;
	PRL_UINT32 width;
	PRL_UINT32 height;
	PRL_UINT32 scaleWidth;
	PRL_UINT32 scaleHeight;
	PRL_UINT32 display;

} PRL_STRUCT( PRL_IO_SCREEN_BUFFER_REGION );
typedef PRL_IO_SCREEN_BUFFER_REGION* PRL_IO_SCREEN_BUFFER_REGION_PTR;

typedef struct _PRL_IO_DISPLAY_RESOLUTION_CONFIRMATION
{
	PRL_BOOL success;
	/// @brief Not used now
	PRL_UINT32	flags;
} PRL_STRUCT( PRL_IO_DISPLAY_RESOLUTION_CONFIRMATION );

typedef struct _PRL_IO_DISPLAY_RESOLUTION_CONFIRMATION_EX
{
	PRL_BOOL success;
	/// @brief Not used now
	PRL_UINT32 flags;
	PRL_UINT64 timestamp;
	PRL_UINT8 reserved[64];
} PRL_STRUCT( PRL_IO_DISPLAY_RESOLUTION_CONFIRMATION_EX );

typedef struct _PRL_IO_KEYBOARD_SCANCODE
{
	PRL_UINT16 key;
	PRL_UINT16 event;

} PRL_STRUCT( PRL_IO_KEYBOARD_SCANCODE );
typedef PRL_IO_KEYBOARD_SCANCODE* PRL_IO_KEYBOARD_SCANCODE_PTR;

typedef struct _PRL_IO_KEYBOARD_LEDS
{
	PRL_UINT32 ledsState;

} PRL_STRUCT( PRL_IO_KEYBOARD_LEDS );
typedef PRL_IO_KEYBOARD_LEDS* PRL_IO_KEYBOARD_LEDS_PTR;

typedef struct _PRL_IO_MOUSE_EVENT
{
	PRL_INT32 x;
	PRL_INT32 y;
	PRL_INT32 z;
	PRL_UINT32 buttons;

} PRL_STRUCT( PRL_IO_MOUSE_EVENT );
typedef PRL_IO_MOUSE_EVENT* PRL_IO_MOUSE_EVENT_PTR;

typedef struct _PRL_IO_MOUSE_EVENT_EX
{
	PRL_INT32 x;
	PRL_INT32 y;
	PRL_INT32 z;
	PRL_UINT32 buttons;

	PRL_INT32 w;
} PRL_STRUCT( PRL_IO_MOUSE_EVENT_EX );
typedef PRL_IO_MOUSE_EVENT_EX* PRL_IO_MOUSE_EVENT_EX_PTR;

typedef struct _PRL_IO_MOUSE_EVENT_EX2
{
	PRL_INT32 x;
	PRL_INT32 y;
	PRL_INT32 z;
	PRL_UINT32 buttons;

	PRL_INT32 w;

	PRL_INT32 z120;
	PRL_INT32 w120;
} PRL_STRUCT( PRL_IO_MOUSE_EVENT_EX2 );
typedef PRL_IO_MOUSE_EVENT_EX2* PRL_IO_MOUSE_EVENT_EX2_PTR;

typedef struct _PRL_IO_GRACEFUL_SHUTDOWN
{
	PRL_UINT32 kind;

} PRL_STRUCT( PRL_IO_GRACEFUL_SHUTDOWN );
typedef PRL_IO_GRACEFUL_SHUTDOWN* PRL_IO_GRACEFUL_SHUTDOWN_PTR;

typedef struct _PRL_IO_DESKTOP_UTILITIES_STATE
{
	PRL_UINT32	bDesktopShown;
	PRL_UINT32	bTaskbarShown;
} PRL_STRUCT( PRL_IO_DESKTOP_UTILITIES_STATE );
typedef PRL_IO_DESKTOP_UTILITIES_STATE* PRL_IO_DESKTOP_UTILITIES_STATE_PTR;

typedef struct _PRL_IO_LANGUAGE_HOTKEYS
{
	PRL_UINT32	LanguageHotkey;
	PRL_UINT32	LayoutHotkey;
} PRL_STRUCT( PRL_IO_LANGUAGE_HOTKEYS );
typedef PRL_IO_LANGUAGE_HOTKEYS* PRL_IO_LANGUAGE_HOTKEYS_PTR;

typedef struct _PRL_IO_SCREEN_SURFACE
{
	PRL_UINT32 windowId;
	PRL_UINT32 surfaceId;
	PRL_INT32  scrollX;
	PRL_INT32  scrollY;
	PRL_UINT32 width;
	PRL_UINT32 height;
	PRL_UINT32 display;

} PRL_STRUCT( PRL_IO_SCREEN_SURFACE );
typedef PRL_IO_SCREEN_SURFACE* PRL_IO_SCREEN_SURFACE_PTR;

typedef struct _PRL_IO_MOUSE_CURSOR
{
	PRL_UINT32 x;
	PRL_UINT32 y;
	PRL_UINT32 hotSpotX;
	PRL_UINT32 hotSpotY;
	PRL_UINT32 width;
	PRL_UINT32 height;

	// Cursor bitmap follows

} PRL_STRUCT( PRL_IO_MOUSE_CURSOR );
typedef PRL_IO_MOUSE_CURSOR* PRL_IO_MOUSE_CURSOR_PTR;

typedef struct _PRL_IO_SLIDING_MOUSE
{
	PRL_UINT32 state;

} PRL_STRUCT( PRL_IO_SLIDING_MOUSE );
typedef PRL_IO_SLIDING_MOUSE* PRL_IO_SLIDING_MOUSE_PTR;

typedef struct _PRL_IO_AVAILABLE_DISPLAYS
{
	PRL_UINT32 availableDisplays;
} PRL_STRUCT( PRL_IO_AVAILABLE_DISPLAYS );
typedef PRL_IO_AVAILABLE_DISPLAYS* PRL_IO_AVAILABLE_DISPLAYS_PTR;

typedef struct _PRL_IO_DEVICE_USED
{
	PRL_UINT32 devType;
	PRL_UINT32 devIndex;
	PRL_UINT32 devAction;

} PRL_STRUCT( PRL_IO_DEVICE_USED );
typedef PRL_IO_DEVICE_USED* PRL_IO_DEVICE_USED_PTR;

typedef struct _PRL_IO_GENERIC_BUFFER
{
	PRL_UINT32   size;
	PRL_VOID_PTR data;

} PRL_STRUCT( PRL_IO_GENERIC_BUFFER );
typedef PRL_IO_GENERIC_BUFFER* PRL_IO_GENERIC_BUFFER_PTR;


///	@brief Structure that is used in TIS to represent versions
/**
* TIS - Tools Information Service
*
* @sa PRL_TIS_INT_VERSION, PRL_TIS_RECORD_INFO
*/
typedef struct _PRL_TIS_VERSION
{
	///	@brief Version major number
	PRL_UINT32 maj;
	///	@brief Version minor number
	PRL_UINT32 min;
	///	@brief Build number
	PRL_UINT32 bld;
	///	@brief Revision number
	PRL_UINT32 rev;

} PRL_STRUCT( PRL_TIS_VERSION );
typedef PRL_TIS_VERSION* PRL_TIS_VERSION_PTR;

///	@brief Structure that is used in TIS to represent internal versions
/*
* TIS - Tools Information Service
*
* @sa PRL_TIS_VERSION, PRL_TIS_RECORD_INFO
*/
typedef struct _PRL_TIS_INT_VERSION
{
	///	@brief Internal version major number
	PRL_UINT32 maj;
	///	@brief Internal version minor number
	PRL_UINT32 min;

} PRL_STRUCT( PRL_TIS_INT_VERSION );
typedef PRL_TIS_INT_VERSION* PRL_TIS_INT_VERSION_PTR;

///	@brief Fixed size information about TIS record
/*
* TIS - Tools Information Service
*
* @sa PRL_TIS_VERSION, PRL_TIS_INT_VERSION
*/
typedef struct _PRL_TIS_RECORD_INFO
{
	///	@brief Version associated with record
	PRL_TIS_VERSION version;
	///	@brief Internal version associated with record
	PRL_TIS_INT_VERSION intVersion;

} PRL_STRUCT( PRL_TIS_RECORD_INFO );
typedef PRL_TIS_RECORD_INFO* PRL_TIS_RECORD_INFO_PTR;


///    @brief Structure that represents Parallels Tools Utility command
/** This structure is used to handle PET_IO_TOOLS_UTILITY_COMMAND.
 */
typedef struct _PRL_IO_TOOLS_UTILITY_COMMAND
{
	///     @brief Command identifier
	PRL_UINT32 command;
	///     @brief Set of 32 bit-flags that can be used to control
	///     command behaviour
	PRL_UINT32 flags;
	///     @brief Size of data that comes with command and "pointed" by
	///     data member
	PRL_UINT32 dataSize;

	// Data follows
}
PRL_STRUCT( PRL_IO_TOOLS_UTILITY_COMMAND );
typedef PRL_IO_TOOLS_UTILITY_COMMAND* PRL_IO_TOOLS_UTILITY_COMMAND_PTR;


///	@brief Structure that represents Parallels Tools Shared Internet Applications data
/** This structure is used to handle message of PET_IO_TOOLS_SIA_DATA type.
*/
typedef struct _PRL_IO_TOOLS_SIA_DATA
{
	///	SIA protocol version
	PRL_UINT32	ver;
	///	SIA command
	PRL_UINT32	command;
	///	SIA command flags
	PRL_UINT32	flags;
	///	SIA data size
	PRL_UINT32	size;

	// data buffer follows
}
PRL_STRUCT( PRL_IO_TOOLS_SIA_DATA );
typedef PRL_IO_TOOLS_SIA_DATA* PRL_IO_TOOLS_SIA_DATA_PTR;


typedef struct _PRL_IO_DYNRES_TOOLS_STATUS
{
	PRL_UINT32 status;
}
PRL_STRUCT( PRL_IO_DYNRES_TOOLS_STATUS );
typedef PRL_IO_DYNRES_TOOLS_STATUS* PRL_IO_DYNRES_TOOLS_STATUS_PTR;


typedef struct _PRL_PROXY_CLIENT_INFO
{
	PRL_UINT32 structSize;              /**< Should be filled by the caller with actual structure size*/
	PRL_CHAR clientOptions[64];         /**< Reserved */
	PRL_UINT8 customerSupportUUID[16];  /**< Reserved */
	PRL_CHAR managerURL[256];           /**< Reserved */
}
PRL_STRUCT( PRL_PROXY_CLIENT_INFO );
typedef PRL_PROXY_CLIENT_INFO* PRL_PROXY_CLIENT_INFO_PTR;


typedef struct _PRL_PROXY_REGISTRATION_INFO
{
	PRL_CHAR hostAddresses[PRL_PROXY_MAX_HOSTS][64]; /**< Host addresses which host listens to */
	PRL_INT32 hostAddressesSz;                       /**< Size of host addresses */
	PRL_CHAR hostName[64];                           /**< Short host name */
	PRL_UINT8 appUUID[16];                           /**< Application uuid */
	PRL_CHAR appVersion[32];                         /**< Application version */
	PRL_CHAR hostOptions[33];                        /**< Using for host icon hash transport */
	PRL_CHAR hostOsName[63];                         /**< Reserved */
	PRL_CHAR isoTimestamp[32];                       /**< Reserved */
}
PRL_STRUCT( PRL_PROXY_REGISTRATION_INFO );
typedef PRL_PROXY_REGISTRATION_INFO* PRL_PROXY_REGISTRATION_INFO_PTR;

typedef struct _PRL_PROXY_HOST_INFO
{
	PRL_UINT8 hostUUID[16];                            /**< Proxy host uuid */
	PRL_CHAR brokerAddresses[PRL_PROXY_MAX_HOSTS][64]; /**< Broker addresses to which connection should be established */
	PRL_INT32 brokerAddressesSz;                       /**< Broker addresses size */
	PRL_PROXY_REGISTRATION_INFO info;                  /**< Host registration info */
}
PRL_STRUCT( PRL_PROXY_HOST_INFO );
typedef PRL_PROXY_HOST_INFO* PRL_PROXY_HOST_INFO_PTR;

typedef struct _PRL_PROXY_GEO_INFO
{
	PRL_CHAR countryName[64];
	PRL_CHAR countryCode[3];
	PRL_CHAR region[3];
	PRL_CHAR city[64];
	PRL_CHAR postalCode[10];
}
PRL_STRUCT( PRL_PROXY_GEO_INFO );
typedef PRL_PROXY_GEO_INFO* PRL_PROXY_GEO_INFO_PTR;

typedef struct _PRL_PROXY_HOST_INFO_EXT
{
	PRL_PROXY_HOST_INFO hostInfo;
	PRL_BOOL isOnline;                               /** Sign which specifies whether appropriate host online */
	PRL_PROXY_GEO_INFO_PTR geoInfo;
	struct _PRL_PROXY_HOST_INFO_EXT *next;
}
PRL_STRUCT( PRL_PROXY_HOST_INFO_EXT );
typedef PRL_PROXY_HOST_INFO_EXT* PRL_PROXY_HOST_INFO_EXT_PTR;

typedef struct _PRL_PROXY_USER_DATA
{
	PRL_STR dataKey;            /**< Data identifier */
	PRL_INT32 dataVersion;      /**< Data version */
	PRL_VOID_PTR data;          /**< Pointer to the data buffer */
	PRL_UINT32 dataSize;        /**< Data size */
	struct _PRL_PROXY_USER_DATA *next; /**< Pointer to the next element */
}
PRL_STRUCT( PRL_PROXY_USER_DATA );
typedef PRL_PROXY_USER_DATA* PRL_PROXY_USER_DATA_PTR;

typedef struct _PRL_PROXY_DATA_KEY
{
	PRL_STR dataKey;            /**< Data identifier */
	PRL_INT32 dataVersion;      /**< Data version */
	struct _PRL_PROXY_DATA_KEY *next; /**< Pointer to the next element */
}
PRL_STRUCT( PRL_PROXY_DATA_KEY );
typedef PRL_PROXY_DATA_KEY* PRL_PROXY_DATA_KEY_PTR;

typedef struct _PRL_IO_MEMORY_INFO
{
	PRL_UINT64 ramOffset;
	PRL_UINT64 ramSize;
	PRL_UINT64 vramOffset;
	PRL_UINT64 vramSize;
	PRL_UINT32 filenameSize;

	// filename follows

} PRL_STRUCT( PRL_IO_MEMORY_INFO );
typedef PRL_IO_MEMORY_INFO* PRL_IO_MEMORY_INFO_PTR;

typedef struct _PRL_IO_AUTH_REQUEST
{
	PRL_UINT8 sessionUuid[16];

} PRL_STRUCT( PRL_IO_AUTH_REQUEST );
typedef PRL_IO_AUTH_REQUEST* PRL_IO_AUTH_REQUEST_PTR;

typedef struct _PRL_IO_AUTH_EXEC_REQUEST
{
	PRL_UINT8 sessionUuid[16];

} PRL_STRUCT( PRL_IO_AUTH_EXEC_REQUEST );
typedef PRL_IO_AUTH_EXEC_REQUEST* PRL_IO_AUTH_EXEC_REQUEST_PTR;

typedef struct _PRL_IO_AUTH_RESPONSE
{
	PRL_UINT32 authResult;

} PRL_STRUCT( PRL_IO_AUTH_RESPONSE );
typedef PRL_IO_AUTH_RESPONSE* PRL_IO_AUTH_RESPONSE_PTR;

typedef struct _PRL_IO_ATTACH_REQUEST
{
	PRL_UINT32 isClientLocal;
	PRL_UINT32 encodingType;
	PRL_UINT8  vmUuid[16];
	PRL_UINT32 attachFlags; /** Since 6.4 IO protocol version */

} PRL_STRUCT( PRL_IO_ATTACH_REQUEST );
typedef PRL_IO_ATTACH_REQUEST* PRL_IO_ATTACH_REQUEST_PTR;

typedef struct _PRL_IO_ATTACH_RESPONSE
{
	PRL_UINT32 attachResult;
	PRL_UINT32 encodingResult;

} PRL_STRUCT( PRL_IO_ATTACH_RESPONSE );
typedef PRL_IO_ATTACH_RESPONSE* PRL_IO_ATTACH_RESPONSE_PTR;

typedef struct _PRL_IO_SCREEN_UPDATE
{
	PRL_UINT32 offset;
	PRL_UINT32 x;
	PRL_UINT32 y;
	PRL_UINT32 width;
	PRL_UINT32 height;
	PRL_UINT32 display;

	// for remote clients or screenshots buffers bitmap follows

} PRL_STRUCT( PRL_IO_SCREEN_UPDATE );
typedef PRL_IO_SCREEN_UPDATE* PRL_IO_SCREEN_UPDATE_PTR;

#define PRL_TC_CLASS_MAX   16

typedef struct _PRL_STAT_NET_TRAFFIC {
	PRL_UINT64 incoming[PRL_TC_CLASS_MAX];
	PRL_UINT64 outgoing[PRL_TC_CLASS_MAX];
	PRL_UINT32 incoming_pkt[PRL_TC_CLASS_MAX];
	PRL_UINT32 outgoing_pkt[PRL_TC_CLASS_MAX];
} PRL_STRUCT( PRL_STAT_NET_TRAFFIC );
typedef PRL_STAT_NET_TRAFFIC* PRL_STAT_NET_TRAFFIC_PTR;

typedef struct _PRL_CPULIMIT_DATA
{
	PRL_UINT32 value;
	PRL_CPULIMIT_TYPE type;
} PRL_STRUCT( PRL_CPULIMIT_DATA );
typedef PRL_CPULIMIT_DATA* PRL_CPULIMIT_DATA_PTR;
typedef const PRL_CPULIMIT_DATA* PRL_CONST_CPULIMIT_DATA_PTR;

typedef struct _PRL_IOLIMIT_DATA
{
	PRL_UINT32 value;
	PRL_IOLIMIT_TYPE type;
} PRL_STRUCT( PRL_IOLIMIT_DATA );
typedef PRL_IOLIMIT_DATA* PRL_IOLIMIT_DATA_PTR;
typedef const PRL_IOLIMIT_DATA* PRL_CONST_IOLIMIT_DATA_PTR;

typedef struct _PRL_GET_VM_CONFIG_PARAM_DATA
{
	PRL_VM_TYPE nVmType;
	PRL_CONST_STR sConfigSample;
	PRL_UINT32 nOsVersion;
} PRL_STRUCT( PRL_GET_VM_CONFIG_PARAM_DATA );
typedef PRL_GET_VM_CONFIG_PARAM_DATA* PRL_GET_VM_CONFIG_PARAM_DATA_PTR;
typedef const PRL_GET_VM_CONFIG_PARAM_DATA* PRL_CONST_GET_VM_CONFIG_PARAM_DATA_PTR;

typedef struct _PRL_CVSRC_OPEN_MODE
{
	PRL_UINT32 sessionId;
	PRL_UINT32 frameFormat;
	PRL_UINT32 frameWidth;
	PRL_UINT32 frameHeight;
	PRL_UINT32 frameBytesPerRow;
} PRL_STRUCT( PRL_CVSRC_OPEN_MODE );
typedef PRL_CVSRC_OPEN_MODE* PRL_CVSRC_OPEN_MODE_PTR;
typedef const PRL_CVSRC_OPEN_MODE* PRL_CONST_CVSRC_OPEN_MODE;

typedef struct _PRL_IO_SSO_CLIENT_VM_PACKET
{
	PRL_UINT8 credentialsRequestUUID[16];
	PRL_UINT32 reserved[12];
} PRL_STRUCT( PRL_IO_SSO_CLIENT_VM_PACKET );
typedef PRL_IO_SSO_CLIENT_VM_PACKET* PRL_IO_SSO_CLIENT_VM_PACKET_PTR;

typedef struct _PRL_IO_CHANNEL_STAT
{
	// @brief Round trip time in milliseconds
	PRL_UINT32	delay;
	// @brief Approximate upload speed in bytes
	PRL_UINT32	upload;
	// @brief Approximate download speed in bytes
	PRL_UINT32	download;
	// Reserved fields for further purposes
	PRL_UINT32 reserved1;
	PRL_UINT32 reserved2;
	PRL_UINT32 reserved3;
} PRL_STRUCT( PRL_IO_CHANNEL_STAT );
typedef PRL_IO_CHANNEL_STAT* PRL_IO_CHANNEL_STAT_PTR;

typedef struct _PRL_CPU_FEATURES_MASK
{
	// @brief mask of processor features
	PRL_UINT32 nFEATURES_MASK;
	// @brief mask of extended processor features
	PRL_UINT32 nEXT_FEATURES_MASK;
	// @brief 00000007 ebx mask
	PRL_UINT32 nEXT_00000007_EBX_MASK;
	// @brief 80000001 ecx mask
	PRL_UINT32 nEXT_80000001_ECX_MASK;
	// @brief 80000001 edx mask
	PRL_UINT32 nEXT_80000001_EDX_MASK;
	// @brief 80000007 edx mask
	PRL_UINT32 nEXT_80000007_EDX_MASK;
	// @brief 80000008 eax
	PRL_UINT32 nEXT_80000008_EAX;
	// @brief 0000000D eax
	PRL_UINT32 nEXT_0000000D_EAX_MASK;
	// @brief masks is valid
	PRL_UINT32 bIsValid;
} PRL_STRUCT( PRL_CPU_FEATURES_MASK );
typedef PRL_CPU_FEATURES_MASK* PRL_CPU_FEATURES_MASK_PTR;
typedef const PRL_CPU_FEATURES_MASK* PRL_CONST_CPU_FEATURES_MASK_PTR;

typedef struct _PRL_CPU_FEATURES
{
	// @brief processor features
	PRL_UINT32 nFEATURES;
	// @brief extended processor features
	PRL_UINT32 nEXT_FEATURES;
	// @brief 00000007 ebx
	PRL_UINT32 nEXT_00000007_EBX;
	// @brief 80000001 ecx
	PRL_UINT32 nEXT_80000001_ECX;
	// @brief 80000001 edx
	PRL_UINT32 nEXT_80000001_EDX;
	// @brief 80000007 edx
	PRL_UINT32 nEXT_80000007_EDX;
	// @brief 80000008 eax
	PRL_UINT32 nEXT_80000008_EAX;
	// @brief 0000000D eax
	PRL_UINT32 nEXT_0000000D_EAX;
} PRL_STRUCT( PRL_CPU_FEATURES );
typedef PRL_CPU_FEATURES* PRL_CPU_FEATURES_PTR;

typedef struct _PRL_HOST_OS_INFO
{
	// @brief Host OS type (one of PVS_GUEST_TYPE_...)
	PRL_UINT32 hostOSType;
} PRL_STRUCT( PRL_HOST_OS_INFO );
typedef PRL_HOST_OS_INFO* PRL_HOST_OS_INFO_PTR;

typedef struct _PRL_MEMGUARANTEE_DATA
{
	PRL_UINT32 value;
	PRL_MEMGUARANTEE_TYPE type;
} PRL_STRUCT( PRL_MEMGUARANTEE_DATA );
typedef PRL_MEMGUARANTEE_DATA* PRL_MEMGUARANTEE_DATA_PTR;
typedef const PRL_MEMGUARANTEE_DATA* PRL_CONST_MEMGUARANTEE_DATA_PTR;

#ifdef _WIN_
#pragma pack(pop, save_pack)
#endif

#endif//__PARALLELS_IO_STRUCTS_H__
