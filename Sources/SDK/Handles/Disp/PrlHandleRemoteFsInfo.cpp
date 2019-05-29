/*
 * PrlHandleRemoteFsInfo.cpp
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


#include "PrlHandleRemoteFsInfo.h"
#include <prlcommon/Logging/Logging.h>
#include "PrlHandleRemoteFsEntry.h"

#ifdef ENABLE_MALLOC_DEBUG
    // By adding this interface we enable allocations tracing in the module
    #include "Interfaces/Debug.h"
#else
    // We're not allowed to throw exceptions from the library -
    // so we need to prevent operator ::new from doing this
    #include <new>
    using std::nothrow;
    #define new new(nothrow)
#endif


PrlHandleRemoteFsInfo::PrlHandleRemoteFsInfo(const QString& sRemoteFsInfo)
: PrlHandleBase(PHT_REMOTE_FILESYSTEM_INFO)
{
	m_RemoteFsInfo.fromString(sRemoteFsInfo);
	if (PRL_FAILED(m_RemoteFsInfo.m_uiRcInit))
		LOG_MESSAGE(DBG_FATAL, "Couldn't to parse file system info: [%s]", sRemoteFsInfo.toUtf8().constData());
}

PRL_RESULT PrlHandleRemoteFsInfo::GetFSType(PRL_FILE_SYSTEM_TYPE_PTR pnFsType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnFsType = PRL_FILE_SYSTEM_TYPE(m_RemoteFsInfo.getFileSystemGenre());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleRemoteFsInfo::GetFileSystemType(PRL_FILE_SYSTEM_FS_TYPE_PTR pnFsType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
		*pnFsType = m_RemoteFsInfo.getFileSystemType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleRemoteFsInfo::GetChildEntriesCount(PRL_UINT32_PTR pnChildEntriesCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnChildEntriesCount = PRL_UINT32(m_RemoteFsInfo.m_lstFileSystemItems.size());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleRemoteFsInfo::GetChildEntry(PRL_UINT32 nIndex, PRL_HANDLE_PTR phChildEntry)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_RemoteFsInfo.m_lstFileSystemItems.size()))
		return (PRL_ERR_INVALID_ARG);
	PrlHandleRemoteFsEntry *pFsEntry = new PrlHandleRemoteFsEntry(PrlHandleRemoteFsInfoPtr(this), m_RemoteFsInfo.m_lstFileSystemItems.value(nIndex));
	if (!pFsEntry)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phChildEntry = pFsEntry->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleRemoteFsInfo::GetParentEntry(PRL_HANDLE_PTR phParentEntry)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHwFsItem *pParentItem = m_RemoteFsInfo.getFsParentItem();
	if (!pParentItem)
		return (PRL_ERR_NO_DATA);
	PrlHandleRemoteFsEntry *pParentFsEntry = new PrlHandleRemoteFsEntry(PrlHandleRemoteFsInfoPtr(this), pParentItem);
	if (!pParentFsEntry)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phParentEntry = pParentFsEntry->GetHandle();
	return (PRL_ERR_SUCCESS);
}
