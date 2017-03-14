/*
 * PrlHandleLocalJob.cpp
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


#include "PrlHandleLocalJob.h"

PrlHandleLocalJob::PrlHandleLocalJob( const QString& job_uuid, PRL_JOB_OPERATION_CODE nJobOpCode )
: PrlHandleServerJob(PrlHandleServerPtr((PrlHandleServer *)0), job_uuid, nJobOpCode)
{}

PrlHandleLocalJob::~PrlHandleLocalJob()
{}

PRL_HANDLE PrlHandleLocalJob::Cancel()
{
	SetReturnCode(PRL_ERR_OPERATION_WAS_CANCELED);
	emit jobWasCanceled();
	return (GENERATE_ERROR_HANDLE(PRL_ERR_SUCCESS, PJOC_JOB_CANCEL));
}

bool PrlHandleLocalJob::wasCanceled()
{
	return ( PRL_ERR_OPERATION_WAS_CANCELED == GetResult().getReturnCode() );
}

