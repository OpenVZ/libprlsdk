/*
 * PrlDisk.h: SDK Disk image types
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


#ifndef ___PARALLELS_DISK_SDK_DEFINITIONS___
#define ___PARALLELS_DISK_SDK_DEFINITIONS___

#ifdef __cplusplus
extern "C" {
#endif

// Include main header file
#include "PrlTypes.h"
#include "PrlErrors.h"

/*
 * On windows platform we're taking advantage of the #pack directive
 * to ensure internal structures alignment by 1 - on unixes we achive
 * the same results by using the __attribute__((packed)) directive
 * -> restoring alignment
 */
#ifdef _WIN_
    #pragma pack(push, save_api_pack, 1)
#endif

/* States progress definition. */
#define PRL_DISK_PROGRESS_MIN			0
#define PRL_DISK_PROGRESS_MAX			1000
#define PRL_DISK_PROGRESS_COMPLETED		1005

// States progress definition
#define PRL_STATE_PROGRESS_MIN			0
#define PRL_STATE_PROGRESS_MAX			1000
#define PRL_STATE_PROGRESS_COMPLETED	1005

/* images types */

typedef PRL_UINT32 PRL_IMAGE_TYPE;
typedef PRL_IMAGE_TYPE* PRL_IMAGE_TYPE_PTR;

// Invalid type
#define PRL_IMAGE_INVALID			0
// Plain disk
#define PRL_IMAGE_PLAIN				1
// Compressed disk (version 1)
#define PRL_IMAGE_COMPRESSED		2
// Real disk (full)
#define PRL_IMAGE_PHYSICAL			4
// Partition from disk
#define PRL_IMAGE_PARTITION			5
// Bootcamp
#define PRL_IMAGE_BOOTCAMP			6
// Bootcamp set by UID
#define PRL_IMAGE_BOOTCAMP_UID			7
// FAKE Bootcamp, for test purposes
#define PRL_IMAGE_BOOTCAMP_FAKE			8
// Limited Microsoft VHD support
#define PRL_IMAGE_VHD_PLAIN			80
#define PRL_IMAGE_VHD_DYNAMIC		81
#define PRL_IMAGE_VHD_PLAIN_PART	82
#define PRL_IMAGE_VHD_DYNAMIC_PART	83
#define PRL_IMAGE_VHD_DIFF			84
// VMware VMDK support
#define PRL_IMAGE_VMDK_MONOLITHIC_FLAT       90
#define PRL_IMAGE_VMDK_MONOLITHIC_SPARSE     91
#define PRL_IMAGE_VMDK_EXTENT_2GB_FLAT       92
#define PRL_IMAGE_VMDK_EXTENT_2GB_SPARSE     93
// Software should guess what kind of disk it is
#define PRL_IMAGE_TRY_GUESS			100
// Last (invalid)
#define PRL_IMAGE_LAST_INVALID		0x80000000

/* Storage/image/disk open parameters */

typedef PRL_UINT32					PRL_DISK_OPEN_FLAGS;
typedef PRL_DISK_OPEN_FLAGS*		PRL_DISK_OPEN_FLAGS_PTR;
typedef const PRL_DISK_OPEN_FLAGS*	PRL_CONST_DISK_OPEN_FLAGS_PTR;

/* Nothing. */
#define PRL_DISK_NOTHING			0x00000000
// Read
#define PRL_DISK_READ				0x00000001
// Write
#define PRL_DISK_WRITE				0x00000002
// ReadLock
#define PRL_DISK_READ_LOCK			0x00000004
/* Fake open, just for parameters get (processed by real disks
   that need administrative privileges to work with).          */
#define PRL_DISK_FAKE_OPEN			0x00000008

/*
 * This parameter is used when user need to change XML data but not
 * access to the drive itself. It can prevent situations when VirtualDisk
 * library should unmount drive from host system to store small amount
 * of data to the XML descriptor.
 */
#define PRL_DISK_XML_CHANGE			0x00000020
/* Type of the file access to use (15 file accessors is enough). */
#define PRL_DISK_FOPS_MASK			0x000003C0
/* Default access mode that compiled itself. */
#define PRL_DISK_FOPS_DEFAULT		0x00000000
// DEPRECATED! Normal access mode (default os)
#define PRL_DISK_FOPS_NORMAL		0x00000040
/* DEPRECATED! Access mode with file caching turned off. */
#define PRL_DISK_FOPS_NONCACHED		0x00000080
/* DEPRECATED! Access mode with requests merging. */
#define PRL_DISK_FOPS_MERGING		0x000000C0
/* DEPRECATED! Access mode to files on PCS storage */
#define PRL_DISK_FOPS_PCS			0x000001C0
// Force open of disk for reading without error checking
#define PRL_DISK_NO_ERROR_CHECKING	0x00000400
// So not create image when creating disk, just put reference to file in XML
#define PRL_DISK_NO_IMAGE_CREATION	0x00001000
// Use raw disk for BootCamp partition (/dev/rdisk instead of /dev/disk)
#define PRL_DISK_USE_RAW_DISK		0x00002000
// Do not use handle pool for disk image handles
#define PRL_DISK_NOT_LIMIT_HANDLES	0x00004000
// Do not check host free space
#define PRL_DISK_NO_SPACE_CHECKS	0x00008000
// Allow inplace data modification to reduce memory copying and allocation
#define PRL_DISK_ALLOW_DATA_MODIFY	0x00010000
// Incoming requests to read/write async must be gathered and aligned
#define PRL_DISK_ALIGN_REQUESTS		0x00020000
// Ignore storages read only flag (useful at disk creation)
#define PRL_DISK_IGNORE_PROTECTION	0x00040000

/* Default (writelock always on at write on). */
#define PRL_DISK_DEFAULT_FLAG		(PRL_DISK_READ | PRL_DISK_WRITE | PRL_DISK_FOPS_DEFAULT)
/* Last (invalid). */
#define PRL_DISK_INVALID


////////////////////////////////////////////////////////////////////////
/**
 * Saved states callback types
 *
 * @author antonz@
 */
////////////////////////////////////////////////////////////////////////
typedef PRL_INT32 PRL_STATES_CALLBACK_TYPE;
typedef PRL_STATES_CALLBACK_TYPE* PRL_STATES_CALLBACK_TYPE_PTR;

// Ordinary callback that disk process going.
#define PRL_STATE_DISK_CALLBACK						0x1000
// Core callback completed
#define PRL_STATE_CORE_FIRST_COMPLETE_CALLBACK		0x1001
// Operation fully completed
#define PRL_STATE_CORE_FULL_COMPLETE_CALLBACK		0x1002
// Disk operation started
#define PRL_STATE_CORE_START_DISK_PROCESS_CALLBACK	0x1003
/*
 * At remove all states this callback indicates that one more stage complete
 * in states removal
 **/
#define PRL_STATE_DISK_CALLBACK_STAGE_CPL			0x1004
// Invalid callback
#define PRL_STATE_CALLBACK_INVALID					0x1005

/*
 * Disk partition types
 */
// Extended partitions
#define PRL_MSDOS_EXTENDED_PARTITION				0x05
#define PRL_WIN_EXTENDED_PARTITION					0x0F
#define PRL_LINUX_EXTENDED_PARTITION				0x85
// EFI system partition
#define PRL_EFI_PARTITION_GPT						0xEE
#define PRL_EFI_PARTITION							0xEF
// Mac OS
#define PRL_MACOS_PARTITION							0xAF
#define PRL_MACOS_BOOT_PARTITION					0xAB
/* Free BSD partition. */
#define PRL_BSD_PARTITION							0xA5
// Linux partitions
#define PRL_LINUX_PARTITION_SWAP					0x82
#define PRL_LINUX_PARTITION_DATA					0x83
// Windows partitions
#define PRL_WINDOWS_PARTITION_NTFS					0x07
#define PRL_WINDOWS_PARTITION_FAT					0x06
#define PRL_WINDOWS_PARTITION_FAT32					0x0B
#define PRL_WINDOWS_PARTITION_FAT32_LBA				0x0C
// Invalid partition
#define PRL_INVALID_PARTITION						0xFF
#define PRL_EMPTY_PARTITION							0x00
/* Special handle for data partition from GPT (can be linux,
   windows, any type).                                       */
#define PRL_DATA_PARTITION							0xd8

/*
 * Disk image cache modes (from host OS point of view)
 */

/* Disk cache default. */
#define PRL_DISK_CACHE_DEFAULT						0

/* Cached access to disk image. Fast write() and read() in case
   of all data can be fit in OS cache. But in the other hand
   host cache will be partially duplicated in guest OS cache, so
   in this mode memory usage may be ineffective.                 */
#define PRL_DISK_CACHE_ENABLED						1

/* Uncached access to disk image. Slow read/write always, but
   host OS cache is not used, so it's good for host
   applications.                                              */
#define PRL_DISK_CACHE_DISABLED						2

/* Cached write/uncached read. Like DISK_CACHE_ENABLED, but read
   data does not duplicated in host OS RAM, so you have more RAM
   for host applications. On Mac you may also see performance
   gain when you read data that were just written before and
   held in OS cache - data will be read from cache even with
   F_NOCACHE read(). This may help guest OS with stuff like
   pagefile.sys for example.                                     */
#define PRL_DISK_CACHE_UNCACHED_READ				3

/* Like PRL_DISK_CACHE_UNCACHED_READ, but cache is enabled for
 * small and random/reordered requests. This let fix some
 * performance problems when guest read data sequentially, but
 * by small buffers.
 */
#define PRL_DISK_CACHE_UNCACHED_SEQ_READ			4

////////////////////////////////////////////////////////////////////////
/* storage parameters */
////////////////////////////////////////////////////////////////////////

typedef struct __PRL_IMAGE_PARAMETERS
{
	// Type of storage (base)
	PRL_IMAGE_TYPE Type;
	// Start of storage
	PRL_UINT64 uStart;
	// End of storage
	PRL_UINT64 uSize;
	// Disk name
	PRL_CHAR_PTR pFileName;
} PRL_STRUCT( PRL_IMAGE_PARAMETERS );
typedef PRL_IMAGE_PARAMETERS* PRL_IMAGE_PARAMETERS_PTR;

////////////////////////////////////////////////////////////////////////
/* disk parameters. Differs from internal due to lists classes */
////////////////////////////////////////////////////////////////////////

typedef struct __PRL_DISK_PARAMETERS
{
	// Number of heads of HDD
	PRL_UINT32 m_Heads;
	// Number of cylinders of HDD
	PRL_UINT32 m_Cylinders;
	// Number of sectors per track
	PRL_UINT32 m_Sectors;
	// Size of HDD in sectors (A size of the sector is 512 byte)
	PRL_UINT64 m_SizeInSectors;
	// Size of block for some kind of disks (in sectors, aligned to track,
	// track = sectors * sector_size)
	PRL_UINT32 m_BlockSize;
	// Count of the storages
	PRL_UINT32 m_Parts;
	// Pointer to array of file parts parameters
	PRL_IMAGE_PARAMETERS_PTR m_Storages;
	/*
	 * Following parameters are available only at SDK v3.
	 */
	// Disk UID
	PRL_GUID Uid;
	// Disk name pointer
	PRL_CHAR_PTR Name;
} PRL_STRUCT( PRL_DISK_PARAMETERS );
typedef PRL_DISK_PARAMETERS*		PRL_DISK_PARAMETERS_PTR;
typedef const PRL_DISK_PARAMETERS*	PRL_CONST_DISK_PARAMETERS_PTR;

////////////////////////////////////////////////////////////////////////
/**
 * snapshot tree element
 */
////////////////////////////////////////////////////////////////////////
typedef struct __PRL_SNAPSHOT_ELEMENT
{
	// Snapshot UID
	PRL_GUID m_GUID;
	// Link to the next snapshot element on this level
	struct __PRL_SNAPSHOT_ELEMENT* m_pNext;
	// Link to the next snapshot level
	struct __PRL_SNAPSHOT_ELEMENT* m_pChild;
} PRL_STRUCT( PRL_SNAPSHOT_ELEMENT );
typedef PRL_SNAPSHOT_ELEMENT*		PRL_SNAPSHOT_ELEMENT_PTR;
typedef const PRL_SNAPSHOT_ELEMENT* PRL_CONST_SNAPSHOT_ELEMENT_PTR;

// Define type of user's callback function
typedef PRL_BOOL (PRL_CALL *PRL_DISK_CALLBACK)(PRL_INT32 iComplete, PRL_INT32 iTotal, PRL_VOID_PTR pParam);
// Later put it somwhere else
typedef PRL_BOOL (PRL_CALL *PRL_STATES_CALLBACK)(PRL_STATES_CALLBACK_TYPE iCallbackType, PRL_INT32 iProgress, PRL_VOID_PTR pParameter);

////////////////////////////////////////////////////////////////////////
/**
 * CHS format of disk
 *
 */
////////////////////////////////////////////////////////////////////////
typedef struct _PRL_CHS
{
	PRL_UINT32 Sectors;
	PRL_UINT32 Heads;
	PRL_UINT32 Cylinders;
} PRL_CHS;
typedef PRL_CHS* PRL_CHS_PTR;

/*
 * On windows platform we're taking advantage of the #pack directive
 * to ensure internal structures alignment by 1 - on unixes we achive
 * the same results by using the __attribute__((packed)) directive
 * -> restoring alignment
 */
#ifdef _WIN_
    #pragma pack(pop, save_api_pack)
#endif

/* This function can be used to create a new virtual disk
   locally without being connected to the Dispatcher Service. The
   function creates a new virtual disk and obtains a handle of
   type PHT_VIRTUAL_DISK identifying the new disk. The handle
   can then be used to perform other operations on the disk if
   necessary (see other functions from this group). If you would
   like to create a new virtual disk on a remote host, use the
   PrlDisk_CreateDisk function.
   Parameters
   pHandle :      [out] A handle of type PHT_VIRTUAL_DISK
                  identifying the new virtual disk. You must
                  always free the handle using the PrlDisk_Free
                  function when the handle is no longer needed.
                  Failure to do so will result in memory leak.
   pDiskName :    Disk name in UTF\-8 format.
   pParams :      Disk configuration parameters.
   pCallback :    A pointer to a callback function or NULL
                  pointer. This parameter allows to call the
                  function asynchronously. If this parameter
                  contains a pointer to a user\-implemented
                  callback function, the PrlDisk_CreateDisk
                  function will return immediately and the
                  operation will continue in the background
                  thread. The background thread will be calling
                  the callback function passing the operation
                  progress information to it. See
                  PRL_DISK_CALLBACK for more information. Using a
                  callback might be helpful when the disk
                  creation is expected to take a long time. For
                  \example, you may want to use a callback when
                  you are creating a large disk and allocating
                  all space for it right away.<p />If this
                  parameter contains a NULL pointer, the function
                  will be called synchronously.
   pAdditional :  A pointer to a buffer containing user\-defined
                  data to pass to the callback function.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid argument values.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDisk_CreateDisk

   PrlDisk_Free                                                   */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				PrlDisk_CreateDisk_Local, (
	// Handle to receive data
	PRL_HANDLE_PTR pHandle,
	// File name
	PRL_CONST_STR pDiskName,
	// Parameters for image
	PRL_CONST_DISK_PARAMETERS_PTR pParams,
	// Callback for long procedures of image creation
	const PRL_DISK_CALLBACK pCallback,
	// Additional parameter (any type) for image class
	PRL_CONST_VOID_PTR pAdditional) );

/* Obtains a handle of type PHT_VIRTUAL_DISK identifying an
   existing virtual disk on a local machine. Use this function
   when you want to view or modify the configuration of an
   existing virtual disk. Once you obtain the disk handle, use
   its functions to perform the desired disk manipulations. This
   function can be used locally without a Dispatcher Service
   connection.
   Parameters
   pHandle :           [out] A handle of type PHT_VIRTUAL_DISK
                       identifying the virtual disk. You must
                       always free the handle using the
                       PrlDisk_Free function when the handle is
                       no longer needed. Failure to do so will
                       \result in memory leak.
   pDiskName :         The disk name for which to obtain the
                       handle, in UTF\-8 format. Disk name is the
                       same as the bundle name (Mac) or the
                       directory name (Windows, Linux) containing
                       the virtual disk files.
   OpenFlags :         Disk open parameters. See <link PRL_DISK Macros>
                       for available options.
   hPolicyList :       A handle of type PHT_HANDLE_LIST
                       containing handles of type
                       PHT_VIRTUAL_DISK_OPEN_POLICY
                       representing disk open policies.
					   May be NULL.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid argument values.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDisk_OpenDisk

   PrlDisk_Free                                                         */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				PrlDisk_OpenDisk_Local, (
	// Handle to receive data
	PRL_HANDLE_PTR pHandle,
	// File name
	PRL_CONST_STR pDiskName,
	// Open flags
	const PRL_DISK_OPEN_FLAGS OpenFlags,
	// Additional flags to image
	PRL_HANDLE hPolicyList) );

/* Obtains a handle of type PHT_VIRTUAL_DISK_OPEN_POLICY
   identifying an offset policy. This policy allows to
   automatically increasing the offsets in PrlDisk_Read/Write
   by specified value.
   Parameters
   pHandle :           [out] A handle of type
                       PHT_VIRTUAL_DISK_OPEN_POLICY
                       identifying the offset policy. You must
                       always free the handle using the
                       PrlHandle_Free function when the handle is
                       no longer needed. Failure to do so will
                       \result in memory leak.
   uiOffset :          Offset value in bytes. This value is
                       added to each read/write operation offset.       */
PRL_METHOD_DECL( PARALLELS_API_VER_7,
				PrlDiskOpenPolicy_CreateOffset, (
	// Handle to receive data
	PRL_HANDLE_PTR pHandle,
	// Offset value in bytes
	PRL_UINT64 uiOffset) );

/* Waits for the background thread to finish executing before
   returning control to the client. Use this function with any
   of the PHT_VIRTUAL_DISK disk functions that can be executed
   asynchronously. When a function is called asynchronously, it
   \returns immediately. The actual task that was initiated by
   the call is performed in the background thread. By calling
   the PrlDisk_WaitForCompletion function, you will suspend the
   main thread execution until the background thread is finished
   processing the task.
   Parameters
   Handle :  A handle of type PHT_VIRTUAL_DISK identifying the
             virtual disk.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid argument values.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				PrlDisk_WaitForCompletion, (
		// Disk handle
		const PRL_HANDLE Handle) );

/* Reverts a virtual disk to the specified snapshot.
   Parameters
   Handle :          A handle of type PHT_VIRTUAL_DISK
                     identifying the virtual disk.
   pDirectoryName :  Directory name. This parameter is not
                     currently used.
   pUid :            The universally unique ID (UUID) assigned to
                     the disk. Use the PrlDisk_GetSnapshotsTree
                     function to obtain the snapshot tree with
                     UUIDs of individual snapshots.
   pCallback :       A pointer to a callback function or NULL
                     pointer. If the parameter contains a valid
                     pointer to the callback, the function will
                     be called asynchronously. If it contains a
                     NULL pointer, the function will be called
                     synchronously.
   pParameter :      A pointer to a buffer containing
                     user\-defined data to pass to the callback
                     function.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid argument values.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDisk_CreateState

   PrlDisk_DeleteState

   PrlDisk_GetSnapshotsTree                                       */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				PrlDisk_SwitchToState, (
		// Disk handle
		const PRL_HANDLE Handle,
		// Pointer to parameter name in UTF8
		PRL_CONST_STR pDirectoryName,
		// UID of the state
		PRL_CONST_GUID_PTR pUid,
		// Callback
		const PRL_STATES_CALLBACK pCallback,
		// Parameter to callback
		PRL_VOID_PTR pParameter) );

/* Frees the PHT_VIRTUAL_DISK handle. This function must be
   called to free the PHT_VIRTUAL_DISK handle when it is no
   loner needed. Failure to do so will result in a memory leak.
   Parameters
   Handle :  A handle of type PHT_VIRTUAL_DISK identifying the
             virtual disk.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - operation completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
                                PrlDisk_Free, (
                // Disk handle
                const PRL_HANDLE Handle) );

/* Writes data to the specified virtual disk.
   Parameters
   Handle :         A handle of type PHT_VIRTUAL_DISK identifying
                    the virtual disk.
   pBlock :         A pointer to the sector/page\-aligned data to
                    write (depends on the host operating system).
   uiSize :         Size of the data to write.
   uiBlockOffset :  Offset of the block on the disk, in sectors.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid argument values.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDisk_Read                                                   */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				PrlDisk_Write, (
		// Disk handle
		const PRL_HANDLE Handle,
		// Block to write
		PRL_CONST_VOID_PTR pBlock,
		// Size of block
		const PRL_UINT32 uiSize,
		// Offset of block (in sectors)
		const PRL_UINT64 uiBlockOffset) );

/* Reads data from the specified virtual disk.
   Parameters
   Handle :         A handle of type PHT_VIRTUAL_DISK identifying
                    the virtual disk.
   pBlock :         A pointer to sector/page\-aligned data to
                    read (depends on the host operating system).
   uiSize :         The size of the data to read.
   uiBlockOffset :  Offset of the block on the disk, in sectors.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid argument values.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDisk_Write                                                  */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				PrlDisk_Read, (
		// Disk handle
		const PRL_HANDLE Handle,
		// Block to write
		PRL_VOID_PTR pBlock,
		// Size of block
		const PRL_UINT32 uiSize,
		// Offset of block (in sectors)
		const PRL_UINT64 uiBlockOffset) );


/* data from a virtual disk cache to the real storage.
   Parameters
   Handle :  A handle of type PHT_VIRTUAL_DISK identifying the
             virtual disk.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid argument values.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_7,
				PrlDisk_FlushCache, (
		const PRL_HANDLE Handle) );

/* Obtains the information about the specified virtual disk.
   Parameters
   Handle :           A handle of type PHT_VIRTUAL_DISK
                      identifying the virtual disk.
   pDiskParameters :  [out] A pointer to a buffer that receives
                      the disk information.
   BufferSize :       [in] The size of the output buffer (in
                      bytes). Set the buffer pointer to null and
                      this parameter's value to zero to receive
                      the required size. [out] The required
                      \output buffer size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid argument values.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				PrlDisk_GetDiskInfo, (
		// Disk handle
		const PRL_HANDLE Handle,
		// Pointer to memory, that receive parameters
		PRL_DISK_PARAMETERS_PTR pDiskParameters,
		// Parameter value buffer size
		PRL_UINT32_PTR BufferSize) );
/*
   Builds a map of the disk contents changes between 2 PITs.
   Parameters
   hDisk :       A handle of type PHT_VIRTUAL_DISK identifying
                 the virtual disk.
   sPit1Uuid :   Uuid of the older PIT.
   sPit2Uuid :   Uuid of the later PIT.
   phMap :       A pointer to a variable which receives the
                 result (a handle of type PHT_VIRTUAL_DISK_MAP).
   Returns
   PRL_RESULT.
*/
PRL_METHOD_DECL( PARALLELS_API_VER_5,
			PrlDisk_GetChangesMap_Local, (
		PRL_HANDLE hDisk,
		PRL_CONST_STR sPit1Uuid,
		PRL_CONST_STR sPit2Uuid,
		PRL_HANDLE_PTR phMap) );

/*	Reports the number of significant bits in the map.
	Parameters
	hMap :			A handle of type PHT_VIRTUAL_DISK_MAP identifying
					the changes map.
	phSize :		A pointer to a variable which receives the
					result.
	Returns
	PRL_RESULT. */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				PrlDiskMap_GetSize, (
		PRL_HANDLE hMap,
		PRL_UINT32_PTR pnSize) );

/*	Reports the size (in bytes) of a block mapped by a single bit
	in the map.
	Parameters
	hMap :			A handle of type PHT_VIRTUAL_DISK_MAP identifying
					the changes map.
	phSize :		A pointer to a variable which receives the
					result.
	Returns
	PRL_RESULT. */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				PrlDiskMap_GetGranularity, (
		PRL_HANDLE hMap,
		PRL_UINT32_PTR pnSize) );

/*	Returns bits from the blocks map.
	Parameters
	hMap :			A handle of type PHT_VIRTUAL_DISK_MAP identifying
					the changes map.
	pBuffer :		A pointer to a store.
	pnCapacity :	A pointer to a variable holding the size
					of the buffer and receiving the number of
					bytes actually written.
	Returns
	PRL_RESULT. */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				PrlDiskMap_Read, (
		PRL_HANDLE hMap,
		PRL_VOID_PTR pBuffer,
		PRL_UINT32_PTR pnCapacity) );



#ifdef __cplusplus
}
#endif

#endif
