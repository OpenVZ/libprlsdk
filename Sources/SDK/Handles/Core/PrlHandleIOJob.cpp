/*
 * PrlHandleIOJob.cpp
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


#include <QCoreApplication>
#include <QBuffer>
#include <QFile>

#include "PrlHandleIOJob.h"
#include "PrlHandleBase.h"
#include "PrlHandleVm.h"
#include "PrlHandleVmEvent.h"
#include "Libraries/IOService/src/IOCommunication/IOSendJob.h"

#include "SDK/Include/PrlIOStructs.h"

#include "Libraries/Logging/Logging.h"

/*****************************************************************************/

#ifdef _WIN_
 #include <windows.h>
 #define sleepMsecs Sleep
#else
 #include <unistd.h>
 #define sleepMsecs(usecs) usleep(usecs * 1000)
#endif

/*****************************************************************************/

namespace {
PRL_JOB_OPERATION_CODE ConvertIOJobTypeToJobType(PrlHandleIOJob::IOJobType nIOJobType)
{
	switch (nIOJobType)
	{
		case PrlHandleIOJob::IOConnectJob:				return (PJOC_VM_CONNECT_TO_VM);
		case PrlHandleIOJob::IOSendJob:					return (PJOC_UNKNOWN);
	}
	return (PJOC_UNKNOWN);
}

}

PrlHandleIOJob::PrlHandleIOJob ( IOJobType type,
		const SmartPtr<IOService::ExecChannel>& channel):
	PrlHandleJob(ConvertIOJobTypeToJobType(type), true),
	m_jobType( type ),
	m_responseReceived( false ),
	m_ioChannel( channel ),
	m_quality(-1),
	m_bIsVmShutdownAnswered( false ),
	m_uShutdownResult( 0 )
{
}

PrlHandleIOJob::~PrlHandleIOJob ()
{}

IOService::Channel* PrlHandleIOJob::GetIOChannel ()
{
	return m_ioChannel.getImpl();
}

PRL_RESULT PrlHandleIOJob::Wait(PRL_UINT32 timeout)
{
        const quint32 minSleep = 10;
        quint32 iterations = timeout/minSleep;

	if ( ! GetIOChannel() ) {
		LOG_MESSAGE(DBG_FATAL, "IO job object (desktop) in invalid!");
		return PRL_ERR_INVALID_ARG;
	}

	if ( m_jobType == IOConnectJob ) {
		while ( iterations-- &&
				GetIOChannel()->getState() == IOService::Channel::Disabled ) {
			QCoreApplication::processEvents();
			sleepMsecs( minSleep );
		}
		if ( GetIOChannel()->getState() == IOService::Channel::Disabled )
			return PRL_ERR_TIMEOUT;
		else if ( GetIOChannel()->getState() == IOService::Channel::Stopped )
			return PRL_ERR_CANT_CONNECT_TO_DISPATCHER;
		else
			return PRL_ERR_SUCCESS;
	}
	else if ( m_jobType == IOSendJob ) {
		IOService::IOSendJob::Result res =
			GetIOChannel()->waitForSend( m_ioJobHandle, timeout );

		if ( res == IOService::IOSendJob::Success )
			return PRL_ERR_SUCCESS;
		else if ( res == IOService::IOSendJob::Fail ||
				  res == IOService::IOSendJob::InvalidJob )
			return PRL_ERR_INVALID_ARG;
		else if ( res == IOService::IOSendJob::Timeout )
			return PRL_ERR_TIMEOUT;
	}
	else {
		LOG_MESSAGE(DBG_FATAL, "IO job type is wrong!");
		return PRL_ERR_INVALID_ARG;
	}

	return PRL_ERR_SUCCESS;
}

PRL_HANDLE PrlHandleIOJob::Cancel()
{
	return (GENERATE_ERROR_HANDLE(PRL_ERR_UNIMPLEMENTED, PJOC_JOB_CANCEL));
}

PRL_RESULT PrlHandleIOJob::GetStatus( PRL_JOB_STATUS_PTR status )
{
	if ( ! GetIOChannel() ) {
		LOG_MESSAGE(DBG_FATAL, "IO job object (desktop) in invalid!");
		*status = PJS_UNKNOWN;
		return PRL_ERR_INVALID_ARG;
	}

	if ( m_jobType == IOConnectJob ) {
		IOService::Channel::State st = GetIOChannel()->getState();
		if ( st == IOService::Channel::Disabled )
			*status = PJS_RUNNING;
		else
			*status = PJS_FINISHED;

		return PRL_ERR_SUCCESS;
	}
	else if ( m_jobType == IOSendJob ) {
		IOService::IOSendJob::Result res =
			GetIOChannel()->getSendResult( m_ioJobHandle );

		if ( res == IOService::IOSendJob::SendPended )
			*status = PJS_RUNNING;
		else
			*status = PJS_FINISHED;

		return PRL_ERR_SUCCESS;
	}
	else {
		LOG_MESSAGE(DBG_FATAL, "IO job type is wrong!");
		*status = PJS_UNKNOWN;
		return PRL_ERR_INVALID_ARG;
	}
}

PRL_RESULT PrlHandleIOJob::GetProgress( PRL_UINT32_PTR percentage )
{
	if ( ! GetIOChannel() ) {
		LOG_MESSAGE(DBG_FATAL, "IO job object (desktop) in invalid!");
		*percentage = 0;
		return PRL_ERR_INVALID_ARG;
	}

	if ( m_jobType == IOConnectJob ) {
		IOService::Channel::State st = GetIOChannel()->getState();
		if ( st == IOService::Channel::Disabled )
			*percentage = 0;
		else
			*percentage = 100;

		return PRL_ERR_SUCCESS;
	}
	else if ( m_jobType == IOSendJob ) {
		IOService::IOSendJob::Result res =
			GetIOChannel()->getSendResult( m_ioJobHandle );

		if ( res == IOService::IOSendJob::SendPended )
			*percentage = 0;
		else
			*percentage = 100;

		return PRL_ERR_SUCCESS;
	}
	else {
		LOG_MESSAGE(DBG_FATAL, "IO job type is wrong!");
		*percentage = 0;
		return PRL_ERR_INVALID_ARG;
	}
}

PRL_RESULT PrlHandleIOJob::GetRetCode( PRL_RESULT_PTR retcode )
{
	if ( ! GetIOChannel() )
	{
		LOG_MESSAGE(DBG_FATAL, "IO job object (desktop) is invalid!");
		*retcode = PRL_ERR_UNEXPECTED;
		return PRL_ERR_INVALID_ARG;
	}

	if ( m_jobType == IOConnectJob )
	{
		IOService::Channel::State st = GetIOChannel()->getState();
		if ( st == IOService::Channel::Disabled )
			*retcode = PRL_ERR_OPERATION_PENDING;
		else if ( st == IOService::Channel::Started )
			*retcode = PRL_ERR_SUCCESS;
		else if ( st == IOService::Channel::Stopped )
			*retcode = PRL_ERR_IO_STOPPED;
		else if ( st == IOService::Channel::UnknownVMId )
		{
			*retcode = PRL_ERR_IO_UNKNOWN_VM_ID;
			if (!m_pError.getHandle())
			{
				PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>( GetVmHandle() );
				if ( pVm.getHandle() )
				{
					QMutexLocker _lock(pVm->GetSynchroObject());
					CVmConfiguration &vmConfig = pVm->GetVmConfig();
					CVmEvent vmEvent(
									PET_VM_INF_UNINITIALIZED_EVENT_CODE,
									vmConfig.getVmIdentification()->getVmUuid(),
									PIE_VIRTUAL_MACHINE,
									PRL_ERR_IO_UNKNOWN_VM_ID
								);
					vmEvent.addEventParameter(
						new CVmEventParameter(
								PVE::String,
								vmConfig.getVmIdentification()->getVmName(),
								EVT_PARAM_MESSAGE_PARAM_0)
							);
					m_pError = new PrlHandleVmEvent(pVm, &vmEvent);
				}
			}
		}
		else if ( st == IOService::Channel::ConnectionTimeout )
			*retcode = PRL_ERR_IO_CONNECTION_TIMEOUT;
		else if ( st == IOService::Channel::AuthenticationFailed )
			*retcode = PRL_ERR_IO_AUTHENTICATION_FAILED;
		else
			*retcode = PRL_ERR_UNEXPECTED;

		return PRL_ERR_SUCCESS;
	}
	else if ( m_jobType == IOSendJob )
	{
		IOService::IOSendJob::Result res =
			GetIOChannel()->getSendResult( m_ioJobHandle );

		if ( res == IOService::IOSendJob::SendPended )
			*retcode = PRL_ERR_OPERATION_PENDING;
		else if ( res == IOService::IOSendJob::Success )
			*retcode = PRL_ERR_SUCCESS;
		else if ( res == IOService::IOSendJob::Fail )
			*retcode = PRL_ERR_FAILURE;
		else if ( res == IOService::IOSendJob::InvalidJob )
			*retcode = PRL_ERR_INVALID_ARG;
		else if ( res == IOService::IOSendJob::InvalidPackage )
			*retcode = PRL_ERR_INVALID_ARG;
		else if ( res == IOService::IOSendJob::SendQueueIsFull )
			*retcode = PRL_ERR_IO_SEND_QUEUE_IS_FULL;
		else
			*retcode = PRL_ERR_UNEXPECTED;

		return PRL_ERR_SUCCESS;
	}
	else
	{
		LOG_MESSAGE(DBG_FATAL, "IO job type is wrong!");
		*retcode = PRL_ERR_INVALID_PARAM;
		return PRL_ERR_INVALID_ARG;
	}
}

PRL_RESULT PrlHandleIOJob::GetDataPtr( PRL_VOID_PTR pOuterPtr,
									   PRL_UINT32_PTR size )
{
	if ( !size || (*size && !pOuterPtr) )
		return PRL_ERR_INVALID_ARG;
        if ( ! GetIOChannel() ) {
                LOG_MESSAGE(DBG_FATAL, "IO job object (desktop) in invalid!");
                return PRL_ERR_INVALID_ARG;
        }

	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleIOJob::GetResult( PRL_HANDLE_PTR )
{
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleIOJob::GetError( PRL_HANDLE_PTR phError )
{
	QMutexLocker _lock(&m_mutex);
	if (m_pError.getHandle())
	{
		m_pError->AddRef();
		*phError = m_pError->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	return PRL_ERR_NO_DATA;
}

void PrlHandleIOJob::RegisterRequest ( const IOService::IOSendJob::Handle& h )
{
	QMutexLocker locker( &m_mutex );

	m_ioJobHandle = h;

	if ( ! m_waitingJobs.contains(h) ) {
		m_waitingJobs.clear();
		return;
	}

	m_responseReceived = true;

	SmartPtr<IOService::IOPackage> p = m_waitingJobs[h];
	m_waitingJobs.clear();
	HandleResponse( h, p );
}

void PrlHandleIOJob::ResponseReceived (
    const IOService::IOSendJob::Handle& h,
	const SmartPtr<IOService::IOPackage>& p )
{
	if ( p->header.type != PET_IO_SCREEN_CAPTURED_BUFFER &&
		 p->header.type != PET_IO_TOOLS_CLIPBOARD_DATA &&
		 p->header.type != PET_IO_TOOLS_VM_SHUTDOWN )
		return;

	QMutexLocker locker( &m_mutex );

	if ( m_responseReceived )
		return;

	// Not yet registered
	if ( ! m_ioJobHandle.isValid() ) {
		m_waitingJobs[h] = p;
		return;
	}

	// Registered, but wrong response
	if ( h != m_ioJobHandle ) {
		return;
	}

	m_responseReceived = true;

	HandleResponse( h, p );
}

void PrlHandleIOJob::HandleResponse (
    const IOService::IOSendJob::Handle&,
	const SmartPtr<IOService::IOPackage>& p )
{
	SetPackageId( p->header.numericId );
	PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>( GetVmHandle() );
	if (pVm)
		pVm->eventSource()->NotifyListeners(this);
}

/*****************************************************************************/
