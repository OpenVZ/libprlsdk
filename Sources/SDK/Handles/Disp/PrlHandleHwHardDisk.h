/*
 * PrlHandleHwHardDisk.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __PARALLELS_HANDLE_HW_HARD_DISK_H__
#define __PARALLELS_HANDLE_HW_HARD_DISK_H__


#include "PrlHandleSrvConfig.h"

/**
 * Host hardware info hard disk handle object.
 */
class PrlHandleHwHardDisk : public PrlHandleBase
{

public:
	/**
	 * Class constructor.
	 * Initializes hard disk settings from specified object.
	 * @param parent server config object pointer
	 */
	PrlHandleHwHardDisk(const PrlHandleSrvConfigPtr &pSrvConfig, class CHwHardDisk *pHardDisk);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleHwHardDisk();

public://API helpers
	/**
	 * Returns device name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDevName(PRL_STR sHwHardDiskName, PRL_UINT32_PTR pnHwHardDiskNameBufLength);


	/**
	 * Returns device id.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDevId(PRL_STR sHwHardDiskId, PRL_UINT32_PTR pnHwHardDiskIdBufLength);

	/**
	 * Returns device size (in megabytes)
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDevSize(PRL_UINT64_PTR pnDevSize);

	/**
	 * Returns disk index
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDiskIndex(PRL_UINT32_PTR pnIndex);

	/**
	 * Returns hard disk partitions count
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetPartsCount(PRL_UINT32_PTR pnPartsCount);

	/**
	 * Returns hard disk partition
	 * @param index of partition
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetPart(PRL_UINT32 nIndex, PRL_HANDLE_PTR phPart);

public:
	/** Returns parent server config object for external usage */
	inline PrlHandleSrvConfigPtr &GetSrvConfig() {return (m_pSrvConfig);}

private:

	/// Storing hard disk XML representation.
	class CHwHardDisk *m_pHwHardDisk;
	/// Pointer to the parent server config object
	PrlHandleSrvConfigPtr m_pSrvConfig;
};

typedef PrlHandleSmartPtr<PrlHandleHwHardDisk> PrlHandleHwHardDiskPtr;

#endif // __PARALLELS_HANDLE_HW_HARD_DISK_H__
