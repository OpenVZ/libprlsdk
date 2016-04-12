/*
 * PrlHandleDisk.h
 *
 * @modifier mperevedentsev
 *
 * Copyright (C) 2005-2016 Parallels IP Holdings GmbH
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

#ifndef __PARALLELS_HANDLE_DISK__
#define __PARALLELS_HANDLE_DISK__

#include "PrlHandleBase.h"
#include "PrlHandleVm.h"
#include "PrlControlValidity.h"
#include "PrlHandleDiskMap.h"
#include <prlcommon/VirtualDisk/VirtualDisk.h>

/**
 * VM disk management object
 */
class PrlHandleDisk : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleDisk();

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleDisk(bool bLocalObject);

public:
	/**
	 * Create disk and return handle
	 *
	 * @param Receive disk handle
	 * @param File name in UTF8
	 * @param Parameters of the disk
	 * @param Disk callback for long operations
	 * @param Parameter for the callback
	 *
	 * @return Error code in PRL_RESULT format
	 */
	PRL_RESULT CreateDisk(
		// File name
		PRL_CONST_STR pDiskName,
		// Parameters for image
		PRL_CONST_DISK_PARAMETERS_PTR pParams,
		// Callback for long procedures of image creation
		const PRL_DISK_CALLBACK pCallback,
		// Additional parameter (any type) for image class
		PRL_CONST_VOID_PTR pAdditional);

	/**
	 * Open disk and return handle
	 *
	 * @param File name in UTF8
	 * @param Disk open flags
	 * @param Disk open policies
	 *
	 * @return Error code in PRL_RESULT format
	 */
	PRL_RESULT OpenDisk(
		// File name
		PRL_CONST_STR pDiskName,
		// Open flags
		const PRL_DISK_OPEN_FLAGS OpenFlags,
		// Open policies
		const PrlHandleHandlesListPtr pPolicyList = PrlHandleHandlesListPtr());

	/**
	 * Wait for completion of the work
	 *
	 * @return Error code in PRL_RESULT format
	 */
	PRL_RESULT WaitForCompletion();

	/**
	 * Start switching to state procedure
	 *
	 * @param UID of the state to switch to
	 * @param Callback to signal progress/completion, if it is equal to NULL
	 *        function will not return until completion
	 * @param Callback parameter
	 *
	 * @return Error code in PRL_RESULT format
	 */
	PRL_RESULT SwitchToState(
		// UID of the state
		PRL_CONST_GUID_PTR pUid,
		// Callback
		const PRL_STATES_CALLBACK pCallback,
		// Parameter to callback
		PRL_VOID_PTR pParameter);

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
	PRL_RESULT Write(
		// Block to write
		PRL_CONST_VOID_PTR pBlock,
		// Size of block
		const PRL_UINT32 uiSize,
		// Offset of block (in sectors)
		const PRL_UINT64 uiBlockOffset);

	/**
	 * Write data to disk
	 *
	 * @param Pointer to sector/page aligned data to write (depends from host OS)
	 * @param Size of data to write
	 * @param Offset of the block on the disk in sectors
	 *
	 * @return PRL_RESULT.
	 */
	PRL_RESULT Read(
		// Block to write
		PRL_VOID_PTR pBlock,
		// Size of block
		const PRL_UINT32 uiSize,
		// Offset of block (in sectors)
		const PRL_UINT64 uiBlockOffset);

	/**
	 * Get disk information
	 *
	 * @param Pointer to the result
	 * @param Pointer to parameter value buffer size, if specified size is
	 *        lower than needed, here will be returned corrected size
	 *
	 * @return PRL_RESULT.
	 */
	PRL_RESULT GetDiskInfo(
		// Pointer to memory, that receive parameters
		PRL_DISK_PARAMETERS_PTR ppDDesc,
		// Parameter value buffer size
		PRL_UINT32_PTR BufferSize);

	/**
	 * Builds a map of the disk contents changes between 2 PITs.
	 * @param Uuid of the oldest PIT.
	 * @param Uuid of the later PIT.
	 * @param Variable which receives the result.
	 *
	 * @return Error code in PRL_RESULT format
	 */
	PRL_RESULT GetChanges(
		// Uuid of the oldest PIT
		PRL_CONST_STR sPit1Uuid,
		// Uuid of the later PIT
		PRL_CONST_STR sPit2Uuid,
		// Result
		PrlHandleDiskMap*& dst_
		);


private:
	static PRL_RESULT getPolicyList(const PrlHandleHandlesListPtr pPolicyList,
	                                VirtualDisk::policyList_type &policies);

	// Mark object as local or remote
	bool m_bLocalObject;
	// Disk interface
	QSharedPointer<VirtualDisk::Format> m_pDisk;
};

typedef PrlHandleSmartPtr<class PrlHandleDisk> PrlHandleDiskPtr;

#endif // __PARALLELS_HANDLE_DISK__
