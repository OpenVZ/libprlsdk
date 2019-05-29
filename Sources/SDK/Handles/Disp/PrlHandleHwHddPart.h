/*
 * PrlHandleHwHddPart.h
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


#ifndef __PARALLELS_HANDLE_HW_HARD_DISK_PARTITION_H__
#define __PARALLELS_HANDLE_HW_HARD_DISK_PARTITION_H__


#include "PrlHandleHwHardDisk.h"

/**
 * Host hardware info hard disk partition handle object.
 */
class PrlHandleHwHddPart : public PrlHandleBase
{

public:
	/**
	 * Class constructor.
	 * Initializes hard disk partition settings from specified object.
	 * @param parent hard disk object pointer
	 */
	PrlHandleHwHddPart(const PrlHandleHwHardDiskPtr &pHardDisk, class CHwHddPartition *pHddPart);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleHwHddPart();

public://API helpers
	/**
	 * Returns partition name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetName(PRL_STR sHwHddPartName, PRL_UINT32_PTR pnHwHddPartNameBufLength);

	/**
	 * Returns partition system name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSysName(PRL_STR sHwHddPartSysName, PRL_UINT32_PTR pnHwHddPartSysNameBufLength);

	/**
	 * Returns partition size (in megabytes)
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSize(PRL_UINT64_PTR pnPartSize);

	/**
	 * Returns partition index
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetIndex(PRL_UINT32_PTR pnIndex);

	/**
	 * Returns partition type
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetPartType(PRL_UINT32_PTR pnType);

	/**
	 * Returns sign whether partition is in use
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsInUse(PRL_BOOL_PTR pbInUse);

	/**
	 * Returns sign whether partition is logical
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsLogical(PRL_BOOL_PTR pbLogical);

	/**
	 * Returns sign whether partition is active
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsActive(PRL_BOOL_PTR pbActive);

private:
	/// Storing hard disk XML representation.
	class CHwHddPartition *m_pHwHddPart;
	/// Pointer to the parent server config object
	PrlHandleHwHardDiskPtr m_pHardDisk;
};

typedef PrlHandleSmartPtr<PrlHandleHwHddPart> PrlHandleHwHddPartPtr;

#endif // __PARALLELS_HANDLE_HW_HARD_DISK_PARTITION_H__
