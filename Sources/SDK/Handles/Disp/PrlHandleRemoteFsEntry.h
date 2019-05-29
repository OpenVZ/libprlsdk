/*
 * PrlHandleRemoteFsEntry.h
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


#ifndef __PARALLELS_HANDLE_REMOTE_FS_ENTRY_H__
#define __PARALLELS_HANDLE_REMOTE_FS_ENTRY_H__


#include "PrlHandleRemoteFsInfo.h"

/**
 * Handle object represents remote FS entry
 */
class PrlHandleRemoteFsEntry : public PrlHandleBase
{

public:
	/**
	 * Class constructor.
	 * @param pointer to the parent FS info object
	 * @param pointer to the FS entry data
	 */
	PrlHandleRemoteFsEntry(const PrlHandleRemoteFsInfoPtr &pParent, CHwFsItem *pFsEntry);

	/**
	 * Returns FS entry absolute path
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetAbsolutePath(PRL_STR sAbsolutePath, PRL_UINT32_PTR pnAbsolutePathBufLength);

	/**
	 * Returns FS entry relative name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRelativeName(PRL_STR sRelativeName, PRL_UINT32_PTR pnRelativeNameBufLength);

	/**
	 * Returns FS entry last modified date in "yyyy-mm-dd hh:mi:ss" format.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetLastModifiedDate(PRL_STR sFsEntryLastModifiedDate, PRL_UINT32_PTR pnFsEntryLastModifiedDateBufLength);

	/**
	 * Returns FS entry size (in bytes)
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSize(PRL_UINT64_PTR pnSize);

	/**
	 * Returns FS entry permissions (read, write, execute) for currently logged in user
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetPermissions(PRL_UINT32_PTR pnPermissions);

	/**
	 * Returns FS entry type (file, directory or drive)
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetEntryType(PRL_FILE_SYSTEM_ELEMENT_TYPE_PTR pnFsEntryType);

private:
	// Pointer parent FS info object
	PrlHandleRemoteFsInfoPtr m_pFsInfo;
	// RemoteFsEntry object
	CHwFsItem *m_pFsEntry;
};

typedef PrlHandleSmartPtr<PrlHandleRemoteFsEntry> PrlHandleRemoteFsEntryPtr;

#endif // __PARALLELS_HANDLE_REMOTE_FS_ENTRY_H__
