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
#include <prlcommon/IOService/IOCommunication/IOSendJob.h>

#include "SDK/Include/PrlIOStructs.h"

#include <prlcommon/Logging/Logging.h>

/*****************************************************************************/

#ifdef _WIN_
 #include <windows.h>
 #define sleepMsecs Sleep
#else
 #include <unistd.h>
 #define sleepMsecs(usecs) usleep(usecs * 1000)
#endif

/*****************************************************************************/

namespace
{
const quint32 g_minSleepMsecs = 10;
const int g_ConnectJobTimeoutMsecs = 60000;

struct Convert
{

static PRL_JOB_OPERATION_CODE toOpCode(PrlHandleIOJob::IOJobType nType)
{
	switch(nType) {
		case PrlHandleIOJob::IOConnectJob:
			return (PJOC_VM_CONNECT_TO_VM);
		case PrlHandleIOJob::IOSendJob:
			return (PJOC_UNKNOWN);
		default:
			return (PJOC_UNKNOWN);
	}
}

static PRL_RESULT toResult(IOService::Channel::State state)
{
	switch(state) {
		case IOService::Channel::Disabled:
			return PRL_ERR_TIMEOUT;
		case IOService::Channel::Stopped:
			return PRL_ERR_IO_STOPPED;
		case IOService::Channel::Started:
			return PRL_ERR_SUCCESS;
		case IOService::Channel::ConnectionTimeout:
			return PRL_ERR_IO_CONNECTION_TIMEOUT;
		case IOService::Channel::AuthenticationFailed:
			return PRL_ERR_IO_AUTHENTICATION_FAILED;
		case IOService::Channel::UnknownVMId:
			return PRL_ERR_IO_UNKNOWN_VM_ID;
		default:
			return PRL_ERR_UNEXPECTED;
	}
}

static PRL_RESULT toResult(IOService::IOSendJob::Result result)
{
	switch(result) {
		case IOService::IOSendJob::Success:
			return PRL_ERR_SUCCESS;
		case IOService::IOSendJob::Fail:
			return PRL_ERR_FAILURE;
		case IOService::IOSendJob::InvalidJob:
		case IOService::IOSendJob::InvalidPackage:
			return PRL_ERR_INVALID_ARG;
		case IOService::IOSendJob::Timeout:
			return PRL_ERR_TIMEOUT;
		case IOService::IOSendJob::SendPended:
			return PRL_ERR_OPERATION_PENDING;
		case IOService::IOSendJob::SendQueueIsFull:
			return PRL_ERR_IO_SEND_QUEUE_IS_FULL;
		default:
			return PRL_ERR_UNEXPECTED;
	}
}

};
}// namespace

PrlHandleIOJob::PrlHandleIOJob ( IOJobType type,
		const SmartPtr<IOService::ExecChannel>& channel):
	PrlHandleJob(Convert::toOpCode(type), true),
	m_Result(PRL_ERR_OPERATION_PENDING),
	m_JobStatus(PJS_RUNNING),
	m_jobType(type),
	m_responseReceived(false),
	m_ioChannel(channel)
{
	if ( type == IOConnectJob ) {
		m_Timer.setSingleShot(true);
		m_Timer.start(g_ConnectJobTimeoutMsecs);

		QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(Finalize()));
	}
}

PrlHandleIOJob::~PrlHandleIOJob ()
{}

IOService::Channel* PrlHandleIOJob::GetIOChannel ()
{
	return m_ioChannel.getImpl();
}

PRL_RESULT PrlHandleIOJob::Wait(PRL_UINT32 timeout)
{
	if ( ! GetIOChannel() ) {
		LOG_MESSAGE(DBG_FATAL, "IO job object (desktop) in invalid!");
		return PRL_ERR_INVALID_ARG;
	}

	quint32 iterations = timeout/g_minSleepMsecs;

	if ( m_jobType == IOConnectJob ) {
		m_Timer.stop(); //use custom timeout instead of builtin one.

		while ( iterations-- &&
				GetIOChannel()->getState() == IOService::Channel::Disabled ) {
			QCoreApplication::processEvents();
			sleepMsecs( g_minSleepMsecs );
		}

		return ProcessResult();
	}
	else if ( m_jobType == IOSendJob ) {
		GetIOChannel()->waitForSend( m_ioJobHandle, timeout );
		return ProcessResult();
	}

	return m_Result;
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

	QMutexLocker _lock(&m_JobStatusMutex);
	*status = m_JobStatus;

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleIOJob::GetProgress( PRL_UINT32_PTR percentage )
{
	if ( ! GetIOChannel() ) {
		LOG_MESSAGE(DBG_FATAL, "IO job object (desktop) in invalid!");
		*percentage = 0;
		return PRL_ERR_INVALID_ARG;
	}

	QMutexLocker _lock(&m_JobStatusMutex);
	if ( m_JobStatus == PJS_RUNNING )
		*percentage = 0;
	else
		*percentage = 100;

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleIOJob::GetRetCode( PRL_RESULT_PTR retcode )
{
	if ( ! GetIOChannel() )
	{
		LOG_MESSAGE(DBG_FATAL, "IO job object (desktop) is invalid!");
		*retcode = PRL_ERR_UNEXPECTED;
		return PRL_ERR_INVALID_ARG;
	}

	QMutexLocker _lock(&m_JobStatusMutex);
	*retcode = m_Result;

	return PRL_ERR_SUCCESS;
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
	QMutexLocker _lock(&m_JobStatusMutex);
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
	QMutexLocker locker( &m_JobStatusMutex );

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

void PrlHandleIOJob::StateChanged(IOService::Channel::State state)
{
	if ( m_jobType != PrlHandleIOJob::IOConnectJob )
		return;

	if ( state == IOService::Channel::Disabled )
		return;

	m_Timer.stop();
	Finalize();
}

void PrlHandleIOJob::Finalize()
{
	ProcessResult();

	PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>( GetVmHandle() );
	if ( ! pVm )
		return;

	pVm->eventSource()->NotifyListeners(this);

	PrlHandleServerPtr pSrv = pVm->GetServer();
	if ( pSrv )
		pSrv->eventSource()->NotifyListeners(this);
}

void PrlHandleIOJob::ResponseReceived (
    const IOService::IOSendJob::Handle& h,
	const SmartPtr<IOService::IOPackage>& p )
{
	if ( p->header.type != PET_IO_SCREEN_CAPTURED_BUFFER &&
		 p->header.type != PET_IO_TOOLS_CLIPBOARD_DATA &&
		 p->header.type != PET_IO_TOOLS_VM_SHUTDOWN )
		return;

	QMutexLocker locker( &m_JobStatusMutex );

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

PRL_RESULT PrlHandleIOJob::ProcessResult()
{
	QMutexLocker _lock(&m_JobStatusMutex);
	m_JobStatus = PJS_FINISHED;

	if ( m_jobType == IOConnectJob ) {
		m_Result = Convert::toResult( GetIOChannel()->getState() );

		if ( m_Result != PRL_ERR_IO_UNKNOWN_VM_ID )
			return m_Result;

		if (! m_pError.getHandle() ) {
			PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>( GetVmHandle() );
			if ( pVm.getHandle() ) {
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
	else if ( m_jobType == IOSendJob ) {
		m_Result = Convert::toResult( GetIOChannel()->getSendResult( m_ioJobHandle ) );
	}

	return m_Result;
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
