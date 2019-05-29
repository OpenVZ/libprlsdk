/*
 * PrlHandleRunningTask.cpp
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


#include "PrlHandleRunningTask.h"
#include <prlcommon/Messaging/CVmEventParameter.h>

PrlHandleRunningTask::PrlHandleRunningTask( CVmEvent *e  )
: PrlHandleBase(PHT_RUNNING_TASK), m_evt( e )
{
}

PrlHandleRunningTask::~PrlHandleRunningTask()
{
}

#define GET_EVENT_PARAM(param_name)\
	CVmEventParameter *pParam = m_evt.getEventParameter(param_name);\
	if (!pParam)\
		return (PRL_ERR_NO_DATA);

PRL_RESULT PrlHandleRunningTask::GetTaskUuid(PRL_STR sTaskUuid, PRL_UINT32_PTR pnTaskUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	GET_EVENT_PARAM( EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_UUID );
	return CopyStringValue(pParam->getParamValue(), sTaskUuid, pnTaskUuidBufLength);
}

PRL_RESULT PrlHandleRunningTask::GetTaskType(PRL_RUNNING_TASK_TYPE_ENUM_PTR pType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	GET_EVENT_PARAM( EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_TYPE );

	bool bRes;
	unsigned int cmd = pParam->getParamValue().toUInt( &bRes );
	if( !bRes )
		return PRL_ERR_NO_DATA;

	switch( cmd )
	{
	default:
		*pType = RTT_RUNNING_TASK_UNKNOWN;
		break;

	case PVE::DspCmdDirVmCreate:
		*pType = RTT_RUNNING_TASK_CREATE_VM;
		break;
	case PVE::DspCmdDirRegVm:
		*pType = RTT_RUNNING_TASK_REGISTER_VM;
		break;
	case PVE::DspCmdDirRestoreVm:
		*pType = RTT_RUNNING_TASK_RESTORE_VM;
		break;
	case PVE::DspCmdDirVmClone:
		*pType = RTT_RUNNING_TASK_CLONE_VM;
		break;
	case PVE::DspCmdDirVmDelete:
		*pType = RTT_RUNNING_TASK_DELETE_VM;
		break;
	case PVE::DspCmdDirUnregVm:
		*pType = RTT_RUNNING_TASK_UNREG_VM;
		break;
	case PVE::DspCmdDirCreateImage:
		*pType = RTT_RUNNING_TASK_CREATE_IMAGE;
		break;
	case PVE::DspCmdDirCopyImage:
		*pType = RTT_RUNNING_TASK_COPY_IMAGE;
		break;
	case PVE::DspCmdDirVmMigrate:
		*pType = RTT_RUNNING_TASK_MIGRATE_VM;
		break;
	}//switch;

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleRunningTask::GetTaskParametersAsString(PRL_STR sParamsStr, PRL_UINT32_PTR pnsParamsStrBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	GET_EVENT_PARAM( EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_PARAMS );
	return CopyStringValue(pParam->getParamValue(), sParamsStr, pnsParamsStrBufLength);
}
