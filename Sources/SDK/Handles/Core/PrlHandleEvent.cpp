/*
 * PrlHandleEvent.cpp
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


#include "PrlHandleEvent.h"
#include "PrlHandleServer.h"
#include "PrlHandleVm.h"
#include <prlcommon/Std/AtomicOps.h>

PrlHandleEvent::PrlHandleEvent( const PrlHandleServerPtr &pServer, PRL_EVENT_TYPE eType,
		PRL_UINT64 eId )
	:	PrlHandleBase( PHT_EVENT ), m_EventType( eType ), m_EventId(eId),
	 	m_pServer( pServer ), m_pVm( 0 )
{
}

PrlHandleEvent::~PrlHandleEvent()
{}

PRL_RESULT PrlHandleEvent::GetServer(PRL_HANDLE_PTR pServer) const
{
	if (m_pServer)
	{
		m_pServer->AddRef();
		*pServer = m_pServer->GetHandle();
	}
	else {
		*pServer = PRL_INVALID_HANDLE;
		return (PRL_ERR_INVALID_HANDLE);
	}
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleEvent::GetEventType( PRL_EVENT_TYPE_PTR type )
{
	*type = m_EventType;

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleEvent::GetEventId( PRL_UINT64_PTR id )
{
	*id = m_EventId;

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleEvent::GetPackageId( PRL_UINT64_PTR id )
{
	*id = m_EventId;

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleEvent::GetVm(PRL_HANDLE_PTR pVm) const
{
	if (m_pVm)
	{
		m_pVm->AddRef();
		*pVm = m_pVm->GetHandle();
	}
	else {
		*pVm = PRL_INVALID_HANDLE;
		return (PRL_ERR_INVALID_HANDLE);
	}
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleEvent::GetParamsCount(PRL_UINT32_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleEvent::GetParam(PRL_UINT32, PRL_HANDLE_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleEvent::GetParamByName(PRL_CONST_STR, PRL_HANDLE_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleEvent::FromString(PRL_CONST_STR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleEvent::GetErrCode(PRL_RESULT_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleEvent::GetErrString(PRL_BOOL, PRL_BOOL, PRL_STR, PRL_UINT32_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleEvent::CanBeIgnored(PRL_BOOL_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleEvent::IsAnswerRequired(PRL_BOOL_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleEvent::GetIssuerType( PRL_EVENT_ISSUER_TYPE_PTR )
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleEvent::GetIssuerId(PRL_STR,	PRL_UINT32_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PrlHandleServerPtr PrlHandleEvent::GetServer()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return (m_pServer);
}

PrlHandleVmPtr PrlHandleEvent::GetVm()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return (m_pVm);
}
