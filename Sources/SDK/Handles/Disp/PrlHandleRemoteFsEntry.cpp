/*
 * PrlHandleRemoteFsEntry.cpp
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


#include "PrlHandleRemoteFsEntry.h"

PrlHandleRemoteFsEntry::PrlHandleRemoteFsEntry(const PrlHandleRemoteFsInfoPtr &pParent, CHwFsItem *pFsEntry)
: PrlHandleBase(PHT_REMOTE_FILESYSTEM_ENTRY), m_pFsInfo(pParent), m_pFsEntry(pFsEntry)
{
}

PRL_RESULT PrlHandleRemoteFsEntry::GetAbsolutePath(PRL_STR sAbsolutePath, PRL_UINT32_PTR pnAbsolutePathBufLength)
{
	SYNCHRO_PARENT_FS_INFO
	return CopyStringValue(m_pFsEntry->getName(), sAbsolutePath, pnAbsolutePathBufLength);
}

PRL_RESULT PrlHandleRemoteFsEntry::GetRelativeName(PRL_STR sRelativeName, PRL_UINT32_PTR pnRelativeNameBufLength)
{
	SYNCHRO_PARENT_FS_INFO
	return CopyStringValue(m_pFsEntry->getRelativeName(), sRelativeName, pnRelativeNameBufLength);
}

PRL_RESULT PrlHandleRemoteFsEntry::GetLastModifiedDate(PRL_STR sFsEntryLastModifiedDate,
																												PRL_UINT32_PTR pnFsEntryLastModifiedDateBufLength)
{
	SYNCHRO_PARENT_FS_INFO
	return CopyStringValue(m_pFsEntry->getModified().toString(XML_DATETIME_FORMAT), sFsEntryLastModifiedDate,
													pnFsEntryLastModifiedDateBufLength);
}

PRL_RESULT PrlHandleRemoteFsEntry::GetSize(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_PARENT_FS_INFO
	*pnSize = m_pFsEntry->getSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleRemoteFsEntry::GetPermissions(PRL_UINT32_PTR pnPermissions)
{
	SYNCHRO_PARENT_FS_INFO
	*pnPermissions = m_pFsEntry->getPermissions();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleRemoteFsEntry::GetEntryType(PRL_FILE_SYSTEM_ELEMENT_TYPE_PTR pnFsEntryType)
{
	SYNCHRO_PARENT_FS_INFO
	*pnFsEntryType = PRL_FILE_SYSTEM_ELEMENT_TYPE(m_pFsEntry->getType());
	return (PRL_ERR_SUCCESS);
}
