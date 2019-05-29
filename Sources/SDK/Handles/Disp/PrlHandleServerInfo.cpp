/*
 * PrlHandleServerInfo.cpp
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


#include "PrlHandleServerInfo.h"
#include <prlcommon/Logging/Logging.h>
#include <prlcommon/Messaging/CVmEventParameter.h>

PrlHandleServerInfo::PrlHandleServerInfo( const QString &sVmEvent, const QString &sServerHost)
: PrlHandleBase(PHT_SERVER_INFO), m_evtServerInfo( sVmEvent ), m_sServerHost(sServerHost)
{}

PrlHandleServerInfo::PrlHandleServerInfo( const QString &sVmEvent)
: PrlHandleBase(PHT_SERVER_INFO), m_evtServerInfo( sVmEvent )
{
	CVmEventParameter *pParam = m_evtServerInfo.getEventParameter(EVT_PARAM_PRL_SERVER_INFO_HOSTNAME);
	if (pParam)
		m_sServerHost = pParam->getParamValue();
	else
		WRITE_TRACE(DBG_FATAL, "Wrong initialization data was passed - server hostname is absent. Event data: [%s]", sVmEvent.toUtf8().constData());
}

#define GET_EVENT_PARAM(param_name)\
	CVmEventParameter *pParam = m_evtServerInfo.getEventParameter(param_name);\
	if (!pParam)\
		return (PRL_ERR_NO_DATA);

PRL_RESULT PrlHandleServerInfo::GetHostName(PRL_STR sHostName, PRL_UINT32_PTR pnHostNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_sServerHost, sHostName, pnHostNameBufLength);
}

PRL_RESULT PrlHandleServerInfo::GetOsVersion(PRL_STR sOsVersion, PRL_UINT32_PTR pnOsVersionBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_OS_VERSION)
	return CopyStringValue(pParam->getParamValue(), sOsVersion, pnOsVersionBufLength);
}

PRL_RESULT PrlHandleServerInfo::GetCmdPort(PRL_UINT32_PTR pnPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_COMMANDS_PORT)
	*pnPort = (PRL_UINT32)pParam->getParamValue().toUInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleServerInfo::GetServerUuid(PRL_STR sServerUuid, PRL_UINT32_PTR pnServerUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_SERVER_UUID)
	return CopyStringValue(pParam->getParamValue(), sServerUuid, pnServerUuidBufLength);
}

PRL_RESULT PrlHandleServerInfo::GetProductVersion(PRL_STR sProductVersion, PRL_UINT32_PTR pnProductVersionBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_PRODUCT_VERSION)
	return CopyStringValue(pParam->getParamValue(), sProductVersion, pnProductVersionBufLength);
}

PRL_RESULT PrlHandleServerInfo::GetApplicationMode (PRL_APPLICATION_MODE_PTR pnApplicationMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_APP_EXECUTE_MODE)
	*pnApplicationMode = (PRL_APPLICATION_MODE)pParam->getParamValue().toUInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleServerInfo::GetStartTime(PRL_UINT64_PTR pnStartTime)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_START_TIME)
	*pnStartTime = (PRL_UINT64)pParam->getParamValue().toULongLong();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleServerInfo::GetStartTimeMonotonic(PRL_UINT64_PTR pnStartTimeMonotonic)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_START_TIME_MONOTONIC)
	*pnStartTimeMonotonic = (PRL_UINT64)pParam->getParamValue().toULongLong();
	return (PRL_ERR_SUCCESS);
}
