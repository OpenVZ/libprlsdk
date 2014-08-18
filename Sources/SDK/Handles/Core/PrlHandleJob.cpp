/*
 * PrlHandleJob.cpp
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


#include "PrlHandleJob.h"

PrlHandleJob::PrlHandleJob(PRL_JOB_OPERATION_CODE nJobOpCode, bool bIsRequestWasSent) :
	PrlHandleBase( PHT_JOB ), m_VmHandle(PRL_INVALID_HANDLE), m_nJobOpCode(nJobOpCode),
	m_bIsRequestWasSent(bIsRequestWasSent), m_nRespPackageId(0)
{}

PrlHandleJob::~PrlHandleJob()
{}

PRL_HANDLE PrlHandleJob::GetVmHandle()
{
	QMutexLocker _lock(&m_VmHandleMutex);
	return (m_VmHandle);
}

void PrlHandleJob::SetVmHandle(PRL_HANDLE hVm)
{
	QMutexLocker _lock(&m_VmHandleMutex);
	m_VmHandle = hVm;
}

PRL_RESULT PrlHandleJob::GetError( PRL_HANDLE_PTR )
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleJob::GetOpCode( PRL_JOB_OPERATION_CODE_PTR pnOpCode )
{
	*pnOpCode = m_nJobOpCode;
	return (PRL_ERR_SUCCESS);
}

PRL_JOB_OPERATION_CODE PrlHandleJob::GetOpCode() const
{
	return (m_nJobOpCode);
}

PRL_RESULT PrlHandleJob::IsRequestWasSent( PRL_BOOL_PTR pbIsRequestWasSent )
{
	*pbIsRequestWasSent = (PRL_BOOL)m_bIsRequestWasSent;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleJob::GetPackageId( PRL_UINT64_PTR id )
{
	*id = m_nRespPackageId;
	return (PRL_ERR_SUCCESS);
}

void PrlHandleJob::SetPackageId( PRL_UINT64 id )
{
	m_nRespPackageId = id;
}
