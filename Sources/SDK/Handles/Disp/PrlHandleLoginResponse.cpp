/*
 * PrlHandleLoginResponse.cpp
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


#include "PrlHandleLoginResponse.h"
#include "PrlHandleRunningTask.h"
#include <prlcommon/Messaging/CVmEventParameter.h>

PrlHandleLoginResponse::PrlHandleLoginResponse( CVmEvent *e  )
: PrlHandleBase(PHT_LOGIN_RESPONSE), m_evtLoginResponse( e )
{
}

PrlHandleLoginResponse::~PrlHandleLoginResponse()
{
}

QString PrlHandleLoginResponse::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_evtLoginResponse.toString();
}

#define GET_EVENT_PARAM(param_name)\
	CVmEventParameter *pParam = m_evtLoginResponse.getEventParameter(param_name);\
	if (!pParam)\
		return (PRL_ERR_NO_DATA);

PRL_RESULT PrlHandleLoginResponse::GetSessionUuid(PRL_STR sSessionUuid, PRL_UINT32_PTR pnSessionUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	GET_EVENT_PARAM( EVT_PARAM_RESPONSE_LOGIN_CMD_SESSION );
	return CopyStringValue(pParam->getParamValue(), sSessionUuid, pnSessionUuidBufLength);
}

PRL_RESULT PrlHandleLoginResponse::GetServerUuid(PRL_STR sServerUuid, PRL_UINT32_PTR pnServerUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_SERVER_UUID)
	return CopyStringValue(pParam->getParamValue(), sServerUuid, pnServerUuidBufLength);
}

PRL_RESULT PrlHandleLoginResponse::GetHostOsVersion(PRL_STR sHostOsVersion, PRL_UINT32_PTR pnHostOsVersionBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_OS_VERSION)
	return CopyStringValue(pParam->getParamValue(), sHostOsVersion, pnHostOsVersionBufLength);
}

PRL_RESULT PrlHandleLoginResponse::GetProductVersion(PRL_STR sProductVersion, PRL_UINT32_PTR pnProductVersionBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	GET_EVENT_PARAM(EVT_PARAM_PRL_SERVER_INFO_PRODUCT_VERSION)
	return CopyStringValue(pParam->getParamValue(), sProductVersion, pnProductVersionBufLength);
}

PRL_RESULT PrlHandleLoginResponse::GetRunningTaskCount(PRL_UINT32_PTR pCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;


	*pCount = m_evtLoginResponse.GetParamsCount( EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_INFO );

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleLoginResponse::GetRunningTaskByIndex(PRL_UINT32 nIndex, PRL_HANDLE_PTR phParam)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	if ( nIndex >= (PRL_UINT32)m_evtLoginResponse.GetParamsCount( EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_INFO ) )
		return (PRL_ERR_INVALID_ARG);

	CVmEventParameter* pEvtParam = m_evtLoginResponse.getEventParameter(EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_INFO, nIndex);
	if( !pEvtParam )
		return PRL_ERR_UNEXPECTED; //Internal error of implementation of CVmEvent

	// internal events of task info is cdata parameters
	CVmEvent taskInfo( pEvtParam->getCdata() );
	if( PRL_FAILED( taskInfo.getInternalRcInit() ) )
		return taskInfo.getInternalRcInit();

	PrlHandleRunningTask* pTaskInfo = new PrlHandleRunningTask( &taskInfo );
	if( !pTaskInfo )
		return PRL_ERR_OUT_OF_MEMORY;

	*phParam = pTaskInfo->GetHandle();

	return (PRL_ERR_SUCCESS);
}
