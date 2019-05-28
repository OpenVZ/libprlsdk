/*
 * PrlHandleUserInfo.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#include "PrlHandleUserInfo.h"


PrlHandleUserInfo::PrlHandleUserInfo(const QString& sUserInfo)
: PrlHandleBase(PHT_USER_INFO)
{
	m_UserInfo.fromString(sUserInfo);
}

PRL_RESULT PrlHandleUserInfo::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (PRL_SUCCEEDED(m_UserInfo.fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleUserInfo::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_UserInfo.toString();
}

PRL_RESULT PrlHandleUserInfo::GetName(PRL_STR sName, PRL_UINT32_PTR pnNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue( m_UserInfo.getName(),
							sName,
							pnNameBufLength);
}

PRL_RESULT PrlHandleUserInfo::GetUuid(PRL_STR sUuid, PRL_UINT32_PTR pnUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue( m_UserInfo.getUuid(),
							sUuid,
							pnUuidBufLength);
}

PRL_RESULT PrlHandleUserInfo::GetSessionCount(PRL_UINT32_PTR pnSessionCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSessionCount = (PRL_UINT32 )m_UserInfo.getSessionCount();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleUserInfo::GetDefaultVmFolder(PRL_STR sDefaultVmFolder, PRL_UINT32_PTR pnDefaultVmFolderBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue( m_UserInfo.getDefaultVmFolder(),
							sDefaultVmFolder,
							pnDefaultVmFolderBufLength);
}

PRL_RESULT PrlHandleUserInfo::CanChangeSrvSets(PRL_BOOL_PTR pbCanChangeSrvSets)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbCanChangeSrvSets = (PRL_BOOL )m_UserInfo.isChangeServerSettings();
	return (PRL_ERR_SUCCESS);
}
