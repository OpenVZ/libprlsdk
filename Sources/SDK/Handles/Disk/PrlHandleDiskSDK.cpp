/*
 * PrlHandleDiskSDK.cpp
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

#include "SDK/Include/Virtuozzo.h"
#include "SDK/Include/PrlErrors.h"
#include "PrlHandleBase.h"
#include "PrlHandleDisk.h"
#include "PrlHandleDiskOpenPolicy.h"


/**
 * Free allocated disk handle
 *
 * @param handle value
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD( PrlDisk_Free ) (
		// Handle
		PRL_HANDLE Handle)
{
	if ( PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK) )
		return PRL_ERR_INVALID_ARG;

	// Just free it
	return PrlHandle_Free(Handle);
};

/**
 * Create disk and return handle
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
	PRL_RESULT retVal = PRL_ERR_INVALID_ARG;
	// Pointer to disk
	PrlHandleDisk* pDisk = NULL;

	// Check parameters
	if (PRL_WRONG_PTR(pHandle) ||
		PRL_WRONG_PTR(pDiskName) ||
		PRL_WRONG_PTR(pParams))
		return retVal;

	// Create default disk class
	pDisk = new (std::nothrow) PrlHandleDisk(true);

	if (!pDisk)
		return PRL_ERR_OUT_OF_MEMORY;

	retVal = pDisk->CreateDisk(pDiskName, pParams, pCallback, pAdditional);

	if (PRL_FAILED(retVal))
	{
		pDisk->Release();
		return retVal;
	}

	*pHandle = pDisk->GetHandle();
	return retVal;
}

/**
 * Open disk and return handle
 * This function can be used locally, without dispatcher connection
 *
 * @param Receive disk handle
 * @param File name in UTF8
 * @param Disk open parameters
 * @param Disk open policies
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
	const PRL_HANDLE hPolicyList)
{
	// Return value
	PRL_RESULT retVal = PRL_ERR_INVALID_ARG;
	// Pointer to disk
	PrlHandleDisk* pDisk = NULL;

	// Check parameters
	if (PRL_WRONG_PTR(pHandle) ||
		PRL_WRONG_PTR(pDiskName))
		return retVal;

	PrlHandleHandlesListPtr phList;
	if (PRL_RIGHT_HANDLE(hPolicyList))
	{
		if (PRL_WRONG_HANDLE(hPolicyList, PHT_HANDLES_LIST))
			return retVal;

		phList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hPolicyList);
	}

	// Create default disk class
	pDisk = new (std::nothrow) PrlHandleDisk(true);

	if (!pDisk)
		return PRL_ERR_OUT_OF_MEMORY;

	retVal = pDisk->OpenDisk(pDiskName, OpenFlags, phList);

	if (PRL_SUCCEEDED(retVal))
	{
		*pHandle = pDisk->GetHandle();
		return PRL_ERR_SUCCESS;
	}

	// The handle must be invalid on error
	pDisk->Release();
	pDisk = NULL;
	return retVal;
}

/**
 * Create 'offset' disk open policy
 * It automatically increases the offset of r/w operations by the given value.
 *
 * @param Receive disk handle
 * @param Offset value in bytes
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD(PrlDiskOpenPolicy_CreateOffset) (
	// Handle to receive data
	PRL_HANDLE_PTR pHandle,
	// Offset value in bytes
	PRL_UINT64 uiOffset)
{
	if (PRL_WRONG_PTR(pHandle))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDiskOpenPolicy *pPolicy = PrlHandleDiskOpenPolicy::createOffset(uiOffset);
	if (!pPolicy)
		return PRL_ERR_OUT_OF_MEMORY;

	*pHandle = pPolicy->GetHandle();
	return PRL_ERR_SUCCESS;
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

	PrlHandleDiskPtr pDisk = PRL_OBJECT_BY_HANDLE<PrlHandleDisk>(Handle);

	return pDisk->WaitForCompletion();
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

	PrlHandleDiskPtr pDisk = PRL_OBJECT_BY_HANDLE<PrlHandleDisk>(Handle);

	return pDisk->SwitchToState(pUid, pCallback, pParameter);
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
	if ( PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK) )
		return PRL_ERR_INVALID_ARG;

	if ( PRL_WRONG_PTR(pBlock) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleDiskPtr pDisk = PRL_OBJECT_BY_HANDLE<PrlHandleDisk>(Handle);

	return pDisk->Write(pBlock, uiSize, uiBlockOffset);
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
	if ( PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK) )
		return PRL_ERR_INVALID_ARG;

	if ( PRL_WRONG_PTR(pBlock) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleDiskPtr pDisk = PRL_OBJECT_BY_HANDLE<PrlHandleDisk>(Handle);

	return pDisk->Read(pBlock, uiSize, uiBlockOffset);
}

PRL_METHOD( PrlDisk_FlushCache ) (
		const PRL_HANDLE Handle)
{
	if (PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK))
		return PRL_ERR_INVALID_ARG;

	return PRL_ERR_SUCCESS;
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
	if ( PRL_WRONG_HANDLE(Handle, PHT_VIRTUAL_DISK) )
	{
		WRITE_TRACE(DBG_FATAL, "wrong handle");
		return PRL_ERR_INVALID_ARG;
	}

	if ( PRL_WRONG_PTR(BufferSize) )
	{
		WRITE_TRACE(DBG_FATAL, "wrong buffer size ptr");
		return PRL_ERR_INVALID_ARG;
	}

	// If specified buffer size is not equal to 0 - pointer should be valid
	if ( *BufferSize )
		if ( PRL_WRONG_PTR(ppDDesc) )
		{
			WRITE_TRACE(DBG_FATAL, "wrong buffer ptr");
			return PRL_ERR_INVALID_ARG;
		}

	PrlHandleDiskPtr pDisk = PRL_OBJECT_BY_HANDLE<PrlHandleDisk>(Handle);

	return pDisk->GetDiskInfo(ppDDesc, BufferSize);
}

/**
 * Builds a map of the disk contents changes between 2 PITs.
 *
 * @param Disk handle.
 * @param Uuid of the oldest PIT.
 * @param Uuid of the later PIT.
 * @param Pointer to a variable which receives the map handle.
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD( PrlDisk_GetChangesMap_Local ) (
	// Disk handle
	PRL_HANDLE hDisk,
	// Uuid of the oldest PIT
	PRL_CONST_STR sPit1Uuid,
	// Uuid of the later PIT
	PRL_CONST_STR sPit2Uuid,
	// Pointer to a variable which receives the map handle
	PRL_HANDLE_PTR phMap)
{
	if (PRL_WRONG_HANDLE(hDisk, PHT_VIRTUAL_DISK))
		return PRL_ERR_INVALID_ARG;

	if (PRL_WRONG_PTR(phMap))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDiskPtr d = PRL_OBJECT_BY_HANDLE<PrlHandleDisk>(hDisk);
	if (!d.isValid())
		return PRL_INVALID_HANDLE;

	PrlHandleDiskMap* m;
	PRL_RESULT e = d->GetChanges(sPit1Uuid, sPit2Uuid, m);
	if (PRL_FAILED(e))
		return e;

	*phMap = m->GetHandle();
	return PRL_ERR_SUCCESS;
}

/**
 * Reports the number of significant bits in the map.
 *
 * @param Map handle.
 * @param Pointer to a variable which receives the map size.
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD(PrlDiskMap_GetSize) (
		// Map handle
		PRL_HANDLE hMap,
		// Pointer to a variable which receives the map size
		PRL_UINT32_PTR pnSize)
{
	if (PRL_WRONG_HANDLE(hMap, PHT_VIRTUAL_DISK_MAP))
		return PRL_ERR_INVALID_ARG;

	if (PRL_WRONG_PTR(pnSize))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDiskMapPtr m = PRL_OBJECT_BY_HANDLE<PrlHandleDiskMap>(hMap);
	if (!m.isValid())
		return PRL_INVALID_HANDLE;

	*pnSize = m->getSize();
	return PRL_ERR_SUCCESS;
}

/**
 * Reports the size (in bytes) of a block mapped by a single bit
 * in the map.
 *
 * @param Map handle.
 * @param Pointer to a variable which receives the map block size.
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD(PrlDiskMap_GetGranularity) (
		// Map handle
		PRL_HANDLE hMap,
		// Pointer to a variable which receives the map block size
		PRL_UINT32_PTR pnSize)
{
	if (PRL_WRONG_HANDLE(hMap, PHT_VIRTUAL_DISK_MAP))
		return PRL_ERR_INVALID_ARG;

	if (PRL_WRONG_PTR(pnSize))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDiskMapPtr m = PRL_OBJECT_BY_HANDLE<PrlHandleDiskMap>(hMap);
	if (!m.isValid())
		return PRL_INVALID_HANDLE;

	*pnSize = m->getGranularity();
	return PRL_ERR_SUCCESS;
}

/**
 * Returns bits from the changes map.
 *
 * @param Map handle.
 * @param Pointer to a store.
 * @param Pointer to a variable holding the size of the store.
 *
 * @return Error code in PRL_RESULT format
 */
PRL_METHOD(PrlDiskMap_Read) (
		// Map handle
		PRL_HANDLE hMap,
		// Pointer to a store
		PRL_VOID_PTR pBuffer,
		// Pointer to a variable holding the size of the store
		PRL_UINT32_PTR pnCapacity)
{
	if (PRL_WRONG_HANDLE(hMap, PHT_VIRTUAL_DISK_MAP))
		return PRL_ERR_INVALID_ARG;

	if (PRL_WRONG_PTR(pnCapacity))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDiskMapPtr m = PRL_OBJECT_BY_HANDLE<PrlHandleDiskMap>(hMap);
	if (!m.isValid())
		return PRL_INVALID_HANDLE;

	return m->getBits(pBuffer, *pnCapacity);
}
