/*
 * PrlHandleDiskSDK.cpp
 *
 * Copyright (C) 1999-2016 Parallels IP Holdings GmbH
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

#include "SDK/Include/Parallels.h"
#include "SDK/Include/PrlErrors.h"
#include "PrlHandleBase.h"

/**
 * Method let to create disk and return handle
 * This function can be used locally, without dispatcher connection
 *
 * @param Receive disk handle
 * @param File name in UTF8
 * @param Parameters of the disk
 * @param Disk callback for long operations
 * @param Parameter for the callback
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD(PrlDisk_CreateDisk_Local) (
	// Handle to receive data
	PRL_HANDLE_PTR pHandle,
	// File name
	PRL_CONST_STR pDiskName,
	// Parameters for image
	PRL_CONST_DISK_PARAMETERS_PTR pParams,
	// Callback for long procedures of image creation
	const PRL_DISK_CALLBACK pCallback,
	// Additional parameter (any type) for image class
	PRL_CONST_VOID_PTR pAdditional)
{
	Q_UNUSED(pParams)
	Q_UNUSED(pCallback)
	Q_UNUSED(pAdditional)

	if (PRL_WRONG_PTR(pHandle) || PRL_WRONG_PTR(pDiskName))
		return PRL_ERR_INVALID_ARG;
	return PRL_ERR_UNIMPLEMENTED;
}

/**
 * Method let to open disk and return handle
 * This function can be used locally, without dispatcher connection
 *
 * @param Receive disk handle
 * @param File name in UTF8
 * @param Disk open parameters
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD(PrlDisk_OpenDisk_Local) (
	// Handle to receive data
	PRL_HANDLE_PTR pHandle,
	// File name
	PRL_CONST_STR pDiskName,
	// Open flags
	const PRL_DISK_OPEN_FLAGS OpenFlags,
	// Additional flags to image
	PRL_CONST_VOID_PTR pAdditionalFlags)
{
	Q_UNUSED(OpenFlags)
	Q_UNUSED(pAdditionalFlags)

	if (PRL_WRONG_PTR(pHandle) || PRL_WRONG_PTR(pDiskName))
		return PRL_ERR_INVALID_ARG;
	return PRL_ERR_UNIMPLEMENTED;
}

/**
 * Wait for completion of the work
 *
 * @param Disk handle
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD(PrlDisk_WaitForCompletion) (
		// Disk handle
		const PRL_HANDLE Handle)
{
	if (PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK))
		return PRL_ERR_INVALID_ARG;
	return PRL_ERR_UNIMPLEMENTED;
}

/**
 * Start switching to state procedure
 *
 * @param Disk handle
 * @param Directory name (ignored now)
 * @param UID of the state to switch to
 * @param Callback to signal progress/completion, if it is equal to NULL
 *        function will not return until completion
 * @param Callback parameter
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD(PrlDisk_SwitchToState) (
		// Disk handle
		const PRL_HANDLE Handle,
		// Pointer to parameter name in UTF8
		PRL_CONST_STR pDirectoryName,
		// UID of the state
		PRL_CONST_GUID_PTR pUid,
		// Callback
		const PRL_STATES_CALLBACK pCallback,
		// Parameter to callback
		PRL_VOID_PTR pParameter)
{
	Q_UNUSED(pCallback)
	Q_UNUSED(pParameter)

	if (PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK))
		return PRL_ERR_INVALID_ARG;
	if (PRL_WRONG_PTR(pDirectoryName) || PRL_WRONG_PTR(pUid))
		return PRL_ERR_INVALID_ARG;
	return PRL_ERR_UNIMPLEMENTED;
}

/**
 * Write data to disk
 *
 * @param Valid PRL_HANDLE
 * @param Pointer to sector/page aligned data to write (depends from host OS)
 * @param Size of data to write
 * @param Offset of the block on the disk in sectors
 *
 * @return PRL_RESULT.
 */
PRL_METHOD(PrlDisk_Write) (
		// Disk handle
		const PRL_HANDLE Handle,
		// Block to write
		PRL_CONST_VOID_PTR pBlock,
		// Size of block
		const PRL_UINT32 uiSize,
		// Offset of block (in sectors)
		const PRL_UINT64 uiBlockOffset)
{
	Q_UNUSED(uiSize)
	Q_UNUSED(uiBlockOffset)

	if (PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK))
		return PRL_ERR_INVALID_ARG;
	if (PRL_WRONG_PTR(pBlock))
		return PRL_ERR_INVALID_ARG;
	return PRL_ERR_UNIMPLEMENTED;
}

/**
 * Read data from disk
 *
 * @param Valid PRL_HANDLE
 * @param Pointer to sector/page aligned data to write (depends from host OS)
 * @param Size of data to write
 * @param Offset of the block on the disk in sectors
 *
 * @return PRL_RESULT.
 */
PRL_METHOD(PrlDisk_Read) (
		// Disk handle
		const PRL_HANDLE Handle,
		// Block to write
		PRL_VOID_PTR pBlock,
		// Size of block
		const PRL_UINT32 uiSize,
		// Offset of block (in sectors)
		const PRL_UINT64 uiBlockOffset)
{
	Q_UNUSED(uiSize)
	Q_UNUSED(uiBlockOffset)

	if (PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK))
		return PRL_ERR_INVALID_ARG;
	if (PRL_WRONG_PTR(pBlock))
		return PRL_ERR_INVALID_ARG;
	return PRL_ERR_UNIMPLEMENTED;
}

/**
 * Get disk information
 *
 * @param Valid PRL_HANDLE
 * @param Pointer to the result
 * @param Pointer to parameter value buffer size, if specified size is
 *        lower than needed, here will be returned corrected size
 *
 * @return PRL_RESULT.
 */
PRL_METHOD(PrlDisk_GetDiskInfo) (
		// Disk handle
		const PRL_HANDLE Handle,
		// Pointer to memory, that receive parameters
		PRL_DISK_PARAMETERS_PTR ppDDesc,
		// Parameter value buffer size
		PRL_UINT32_PTR BufferSize)
{
	if (PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK))
		return PRL_ERR_INVALID_ARG;
	if (PRL_WRONG_PTR(BufferSize))
		return PRL_ERR_INVALID_ARG;

	// If specified buffer size is not equal to 0 - pointer should be valid
	if (*BufferSize)
		if (PRL_WRONG_PTR(ppDDesc))
			return PRL_ERR_INVALID_ARG;
	return PRL_ERR_UNIMPLEMENTED;
}

