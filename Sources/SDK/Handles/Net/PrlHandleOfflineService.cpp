/*
 * PrlHandleOfflineService.cpp
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

#include "PrlHandleOfflineService.h"

PrlHandleOfflineService::PrlHandleOfflineService()
: PrlHandleBase(PHT_OFFLINE_SERVICE)
{
}

PrlHandleOfflineService::PrlHandleOfflineService(QString sOffSrv)
: PrlHandleBase(PHT_OFFLINE_SERVICE)
{
        m_OffmgmtService.fromString(sOffSrv);
}

PRL_RESULT PrlHandleOfflineService::CreateOffmgmtService(
		PRL_HANDLE_PTR phOffmgmtService
		)
{
	PrlHandleOfflineService* pOffSrv = new PrlHandleOfflineService();
	if ( !pOffSrv )
		return PRL_ERR_OUT_OF_MEMORY;

	*phOffmgmtService = pOffSrv->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleOfflineService::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_OffmgmtService.fromString(UTF8_2QSTR(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleOfflineService::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_OffmgmtService.toString();
}

PRL_RESULT PrlHandleOfflineService::GetName(PRL_STR sName, PRL_UINT32_PTR pnNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_OffmgmtService.getName(), sName, pnNameBufLength);
}

PRL_RESULT PrlHandleOfflineService::SetName(PRL_CONST_STR sName)
{
	if (sName == NULL || sName[0] == '\0')
		return PRL_ERR_INVALID_ARG;

	SYNCHRO_INTERNAL_DATA_ACCESS
	m_OffmgmtService.setName(UTF8_2QSTR(sName));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleOfflineService::GetOfflineServicePort(PRL_UINT32_PTR pnPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnPort = m_OffmgmtService.getPort();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleOfflineService::SetOfflineServicePort(PRL_UINT32 nPort)
{
	if (nPort == 0)
		return PRL_ERR_INVALID_ARG;

	SYNCHRO_INTERNAL_DATA_ACCESS
	m_OffmgmtService.setPort(nPort);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleOfflineService::IsUsedByDefault(PRL_BOOL_PTR pbDefault)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbDefault = m_OffmgmtService.isUsedByDefault();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleOfflineService::SetUsedByDefault(PRL_BOOL bDefault)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_OffmgmtService.setUsedByDefault(bDefault);
	return PRL_ERR_SUCCESS;
}
