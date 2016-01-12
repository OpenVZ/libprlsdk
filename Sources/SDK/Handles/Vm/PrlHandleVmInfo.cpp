/*
 * PrlHandleVmInfo.cpp
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


#include "PrlHandleVmInfo.h"
#include <prlcommon/Logging/Logging.h>
#include <prlxmlmodel/Messaging/CVmEventParameter.h>
#include <prlxmlmodel/Messaging/CVmEventParameterList.h>
#include "PrlHandleAccessRights.h"

PrlHandleVmInfo::PrlHandleVmInfo( CVmEvent *e )
: PrlHandleBase(PHT_VM_INFO), m_VmEvent(e)
{}

PRL_RESULT PrlHandleVmInfo::GetState ( VIRTUAL_MACHINE_STATE_PTR pVmState )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VMINFO_VM_STATE );
	if (!pParam)
	{
		*pVmState = VMS_UNKNOWN;
		return (PRL_ERR_NO_DATA);
	}
	*pVmState = (VIRTUAL_MACHINE_STATE)pParam->getParamValue().toInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmInfo::GetAccessRights ( PRL_HANDLE_PTR phVmAcl )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VMINFO_VM_SECURITY );
	if (!pParam)
	{
		*phVmAcl = PRL_INVALID_HANDLE;
		return (PRL_ERR_NO_DATA);
	}
	CVmSecurity _vm_security;
	PRL_RESULT nParseResult = _vm_security.fromString(pParam->getParamValue());
	if (PRL_FAILED(nParseResult))
	{
		LOG_MESSAGE(DBG_FATAL, "An error occured on parsing VM info: %s", PRL_RESULT_TO_STRING(nParseResult));
		*phVmAcl = PRL_INVALID_HANDLE;
		return (PRL_ERR_NO_DATA);
	}
	PrlHandleAccessRights *pVmAcl = new PrlHandleAccessRights(_vm_security);
	*phVmAcl = pVmAcl->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmInfo::IsInvalid ( PRL_BOOL_PTR pbIsInvalid, PRL_RESULT_PTR pnInvalidReason )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VMINFO_VM_IS_INVALID );
	if (!pParam)
	{
		*pbIsInvalid = PRL_TRUE;
		return (PRL_ERR_NO_DATA);
	}
	*pbIsInvalid = (PRL_BOOL)pParam->getParamValue().toInt();

	pParam = m_VmEvent.getEventParameter( EVT_PARAM_VMINFO_VM_VALID_RC );
	if (!pParam)
		*pnInvalidReason = PRL_ERR_SUCCESS; // old version of server
	else
		*pnInvalidReason = (PRL_RESULT)pParam->getParamValue().toInt();

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmInfo::IsVmWaitingForAnswer ( PRL_BOOL_PTR pbIsVmWaitingForAnswer )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
		CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VMINFO_IS_VM_WAITING_FOR_ANSWER );
	if (!pParam)
	{
		*pbIsVmWaitingForAnswer = PRL_FALSE;
		return (PRL_ERR_NO_DATA);
	}
	*pbIsVmWaitingForAnswer = (PRL_BOOL)pParam->getParamValue().toInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmInfo::IsVncServerSrtated ( PRL_BOOL_PTR pbIsVncServerStarted )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
		CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VMINFO_IS_VNC_SERVER_STARTED );
	if (!pParam)
	{
		*pbIsVncServerStarted = PRL_FALSE;
		return (PRL_ERR_NO_DATA);
	}
	*pbIsVncServerStarted = (PRL_BOOL)pParam->getParamValue().toInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmInfo::GetAdditionState ( VIRTUAL_MACHINE_ADDITION_STATE_PTR pVmState )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
		CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VMINFO_VM_ADDITION_STATE );
	if (!pParam)
	{
		*pVmState = VMAS_NOSTATE;
		return (PRL_ERR_NO_DATA);
	}
	*pVmState = ( VIRTUAL_MACHINE_ADDITION_STATE )pParam->getParamValue().toInt();
	return (PRL_ERR_SUCCESS);
}


PRL_RESULT PrlHandleVmInfo::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	if (IS_OPERATION_SUCCEEDED(m_VmEvent.fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleVmInfo::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_VmEvent.toString();
}

