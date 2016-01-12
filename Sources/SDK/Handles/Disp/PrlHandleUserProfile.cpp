/*
 * PrlHandleUserProfile.cpp
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


#include "PrlHandleUserProfile.h"
#include <prlxmlmodel/DispConfig/CDispUser.h>

PrlHandleUserProfile::PrlHandleUserProfile(const QString& sUserProfile)
: PrlHandleBase(PHT_USER_PROFILE), m_UserProfile(sUserProfile)
{}

PRL_RESULT PrlHandleUserProfile::GetVmDirUuid(PRL_STR sVmDirUuidBuf, PRL_UINT32_PTR pnVmDirUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_UserProfile.getUserWorkspace()->getVmDirectory(), sVmDirUuidBuf,
													pnVmDirUuidBufLength);
}

PRL_RESULT PrlHandleUserProfile::SetVmDirUuid(PRL_CONST_STR sNewVmDirUuid)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmDirUuidUtf8 = UTF8_2QSTR(sNewVmDirUuid);
	m_UserProfile.getUserWorkspace()->setVmDirectory(sNewVmDirUuidUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleUserProfile::GetDefaultVmFolder(PRL_STR sDefaultVmFolderBuf, PRL_UINT32_PTR pnDefaultVmFolderBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
		return CopyStringValue(m_UserProfile.getUserWorkspace()->getDefaultVmFolder(), sDefaultVmFolderBuf,
		pnDefaultVmFolderBufLength);
}

PRL_RESULT PrlHandleUserProfile::SetDefaultVmFolder(PRL_CONST_STR sNewDefaultVmFolder)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
		QString sNewDefaultVmFolderUtf8 = UTF8_2QSTR(sNewDefaultVmFolder);
	m_UserProfile.getUserWorkspace()->setDefaultVmFolder(sNewDefaultVmFolderUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleUserProfile::CanUseMngConsole(PRL_BOOL_PTR pbCanUseMngConsole)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbCanUseMngConsole = m_UserProfile.getUserAccess()->canUseServerManagementConsole();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleUserProfile::CanChangeSrvSets(PRL_BOOL_PTR pbCanChangeSrvSets)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbCanChangeSrvSets = m_UserProfile.getUserAccess()->canChangeServerSettings();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleUserProfile::fromString(PRL_CONST_STR sUserProfile)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (PRL_SUCCEEDED(m_UserProfile.fromString(UTF8_2QSTR(sUserProfile))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleUserProfile::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_UserProfile.toString();
}

PRL_RESULT PrlHandleUserProfile::IsLocalAdministrator(PRL_BOOL_PTR pbIsLocalAdministrator)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbIsLocalAdministrator = m_UserProfile.getUserAccess()->isLocalAdministrator();
	return (PRL_ERR_SUCCESS);
}


namespace {
	void UpdateCachedDetails(CDispUserCachedPasword* pass,
							PRL_CONST_STR sProxyHost, PRL_UINT32 nProxyPort,
							PRL_CONST_STR sProxyUserLogin, PRL_CONST_STR sProxyUserPasswd,
							PRL_INT32 nReserved)
	{
		Q_UNUSED(nReserved);

		pass->setProxyServerName( sProxyHost );
		pass->setProxyServerPort( nProxyPort );
		pass->setProxyServerUser( sProxyUserLogin );
		pass->setProxyServerPassword( sProxyUserPasswd );
	}
} // anonimous namespace

PRL_RESULT PrlHandleUserProfile::AddProxy(
		PRL_CONST_STR sProxyHost, PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin, PRL_CONST_STR sProxyUserPasswd,
		PRL_INT32 nReserved)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	CDispUserCachedData* data = m_UserProfile.getUserCachedData();

	foreach(CDispUserCachedPasword* pass, data->m_lstUserCachedPaswords)
	{
		// We check only host (port and scheme are ignored)
		if (pass->getProxyServerName() != sProxyHost)
			continue;

		UpdateCachedDetails(pass, sProxyHost, nProxyPort, sProxyUserLogin, sProxyUserPasswd, nReserved);

		return PRL_ERR_SUCCESS;
	}

	CDispUserCachedPasword * pass = new CDispUserCachedPasword();
	UpdateCachedDetails(pass, sProxyHost, nProxyPort, sProxyUserLogin, sProxyUserPasswd, nReserved);
	data->m_lstUserCachedPaswords.append( pass );

	return (PRL_ERR_SUCCESS);
}

