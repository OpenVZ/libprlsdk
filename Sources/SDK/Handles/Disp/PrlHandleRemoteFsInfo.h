/*
 * PrlHandleRemoteFsInfo.h
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


#ifndef __PARALLELS_HANDLE_REMOTE_FS_INFO_H__
#define __PARALLELS_HANDLE_REMOTE_FS_INFO_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/HostHardwareInfo/CHwFileSystemInfo.h>

#define SYNCHRO_PARENT_FS_INFO QMutexLocker _lock(m_pFsInfo->GetSynchroObject());

/**
 * Handle object represents remote FS info
 */
class PrlHandleRemoteFsInfo : public PrlHandleBase
{

public:
	/**
	 * Class default constructor.
	 */
	PrlHandleRemoteFsInfo(const QString& sRemoteFsInfo);

	/**
	 * Returns remote FS basic type: Windows or Unix like FS
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetFSType(PRL_FILE_SYSTEM_TYPE_PTR pnFsType);

	/**
	* Returns remote FS basic type: Windows or Unix like FS
	* @param [out] buffer for storing result
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetFileSystemType(PRL_FILE_SYSTEM_FS_TYPE_PTR pnFsType);


	/**
	 * Returns child FS entries count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetChildEntriesCount(PRL_UINT32_PTR pnChildEntriesCount);


	/**
	 * Returns child FS entry object
	 * @param [in] index of object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - object index is out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetChildEntry(PRL_UINT32 nIndex, PRL_HANDLE_PTR phChildEntry);

	/**
	 * Returns parent FS entry (above FS level) object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - object doesn't have parent FS entry
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetParentEntry(PRL_HANDLE_PTR phParentEntry);

public:
	/**
	 * Returns pointer to internal VM event data synchronization object
	 */
	inline QMutex *GetSynchroObject() {return (&m_HandleMutex);}

private:
	/// RemoteFsInfo object
	CHwFileSystemInfo m_RemoteFsInfo;
};

typedef PrlHandleSmartPtr<PrlHandleRemoteFsInfo> PrlHandleRemoteFsInfoPtr;

#endif // __PARALLELS_HANDLE_REMOTE_FS_INFO_H__
