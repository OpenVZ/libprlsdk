/*
 * PrlHandleServerJob.cpp
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


#include "PrlHandleServerJob.h"
#include "PrlHandleServer.h"
#include "PrlHandleResult.h"
#include "PrlHandleVmEvent.h"
#include "PrlHandleVm.h"
#include <prlcommon/Messaging/CVmEvent.h>

#include <prlcommon/Logging/Logging.h>
#include <prlcommon/HostUtils/HostUtils.h>
#include <prlcommon/PrlCommonUtilsBase/CommandConvHelper.h>

#include <map>
#include <new>
#include <QHash>

#include <prlcommon/PrlUuid/Uuid.h>
#include <prlcommon/Std/PrlAssert.h>

#ifdef ENABLE_MALLOC_DEBUG
    // By adding this interface we enable allocations tracing in the module
    #include "Interfaces/Debug.h"
#else
    // We're not allowed to throw exceptions from the library -
    // so we need to prevent operator ::new from doing this
    #include <new>
    using std::nothrow;
    #define new new(nothrow)
#endif



/**
 * Statical map holding all jobs by their uuid's.
 */
typedef std::map< QString, PrlHandleServerJob* > JobUuidMap;
Q_GLOBAL_STATIC(JobUuidMap, getJobUuidMap)
/** Object synchronizing access to jobs list */
Q_GLOBAL_STATIC(QMutex, getJobUuidMapMutex)

PrlHandleJobPtr CreateErrorHandle(PRL_RESULT error_code,
                                  PRL_CONST_STR strErrorSource,
                                  PRL_JOB_OPERATION_CODE nJobOpCode)
{
	QString strUuid = Uuid::createUuid().toString();
	PrlHandleServerJobPtr pJob(new PrlHandleServerJob(PrlHandleServerPtr((PrlHandleServer *)0),
                                                      strUuid, nJobOpCode, false));
	pJob->InitializeError(strUuid, error_code, strErrorSource);
	return (PrlHandleJobPtr(pJob.getHandle()));
}

PrlHandleServerJob::PrlHandleServerJob(
						const PrlHandleServerPtr &server,
						const QString& job_uuid,
						PRL_JOB_OPERATION_CODE nJobOpCode,
						bool bIsRequestWasSent
						)
:
PrlHandleJob(nJobOpCode, bIsRequestWasSent),
m_pServer(server),
m_Result(job_uuid, DispatcherCmdsToJobTypeConverter::Convert(nJobOpCode), PRL_ERR_UNINITIALIZED),
m_JobStatus(PJS_RUNNING)
{
	m_Uuid = job_uuid;

	QMutexLocker _lock(::getJobUuidMapMutex());
	if (NULL == _lock.mutex())
		return;

	// Registering object in the map
	(*::getJobUuidMap())[ m_Uuid ] = this;
	//Register job at notification awaiting jobs list
	if (m_pServer)
		m_pServer->AddJobToResponseAwaitingList(PrlHandleServerJobPtr(this));
}


PrlHandleServerJob::~PrlHandleServerJob()
{
	QMutexLocker _lock(::getJobUuidMapMutex());
	if (NULL != _lock.mutex())
		::getJobUuidMap()->erase( m_Uuid );
}

QString PrlHandleServerJob::GetJobUuid() const
{
	return (m_Uuid);
}

PRL_RESULT PrlHandleServerJob::Wait(PRL_UINT32 msecs)
{
	if (QThread::currentThread() != QCoreApplication::instance()->thread())
	{
		QMutexLocker _lock(&m_JobStatusMutex);
		if (m_JobStatus != PJS_FINISHED)
			if ( !m_JobStatusCondition.wait(&m_JobStatusMutex, msecs) )
				return PRL_ERR_TIMEOUT;
	}
	else
	{
		//Special case to prevent block of main event loop owner thread
		PRL_UINT32 nRestTimeout = msecs;
		PRL_UINT32 nPartialTimeout = 50;
		while (PJS_FINISHED != GetInternalJobStatus() && nRestTimeout)
		{
			QCoreApplication::processEvents();
			HostUtils::Sleep(nPartialTimeout < nRestTimeout ? nPartialTimeout : nRestTimeout);
			if (nRestTimeout >= nPartialTimeout)
				nRestTimeout -= nPartialTimeout;
		}
		if (PJS_FINISHED != GetInternalJobStatus())
			return PRL_ERR_TIMEOUT;
	}

	return PRL_ERR_SUCCESS;
}

PRL_HANDLE PrlHandleServerJob::Cancel()
{
	if (!m_pServer)
		return GENERATE_ERROR_HANDLE(PRL_ERR_UNINITIALIZED, PJOC_JOB_CANCEL);
	PrlHandleJobPtr pJob = m_pServer->DspCmdUserCancelOperation(m_Uuid.toUtf8().data());
	if (!pJob)
		return (PRL_INVALID_HANDLE);
	return (pJob->GetHandle());
}

PRL_RESULT PrlHandleServerJob::GetStatus( PRL_JOB_STATUS_PTR status )
{
	QMutexLocker _lock(&m_JobStatusMutex);
	*status = m_JobStatus;

	return PRL_ERR_SUCCESS;
}


PRL_RESULT PrlHandleServerJob::GetRetCode( PRL_RESULT_PTR retcode )
{
	QMutexLocker _lock(&m_JobStatusMutex);
	*retcode = m_Result.getReturnCode();

	return PRL_ERR_SUCCESS;
}


PRL_RESULT PrlHandleServerJob::GetErrCode( PRL_RESULT_PTR errcode )
{
	QMutexLocker _lock(&m_JobStatusMutex);
	if (!m_Result.GetError())
		return (PRL_ERR_UNINITIALIZED);
	*errcode = m_Result.GetError()->getEventCode();

	return PRL_ERR_SUCCESS;
}


PRL_RESULT PrlHandleServerJob::GetDataPtr( PRL_VOID_PTR data_ptr, PRL_UINT32_PTR size )
{
	PRL_UNUSED_PARAM(data_ptr);
	PRL_UNUSED_PARAM(size);
	return PRL_ERR_UNIMPLEMENTED;
}


PRL_RESULT PrlHandleServerJob::GetResult( PRL_HANDLE_PTR handle )
{
	m_JobStatusMutex.lock();
	CResult _result = m_Result;
	m_JobStatusMutex.unlock();
	// Check current job status
	PRL_JOB_STATUS status;
	GetStatus( &status );

	if (status == PJS_FINISHED)
	{
		PrlHandleResult *pHandleResult =
			new PrlHandleResult(m_pServer, PRL_OBJECT_BY_HANDLE<PrlHandleVm>(GetVmHandle()), _result);
		if (!pHandleResult)
			return (PRL_ERR_OUT_OF_MEMORY);
		*handle = pHandleResult->GetHandle();
		return PRL_ERR_SUCCESS;
	}

	return PRL_ERR_OPERATION_PENDING;
}


PRL_RESULT PrlHandleServerJob::GetProgress( PRL_UINT32_PTR percentage )
{
	QMutexLocker _lock(&m_HandleMutex);
	*percentage = 100;

	return PRL_ERR_SUCCESS;
}


void PrlHandleServerJob::SetCResult( const CResult& result )
{
	QMutexLocker _lock(&m_JobStatusMutex);
	m_Result = result;
	SetPackageId(result.getPackageId());
	m_JobStatus = PJS_FINISHED;
	m_JobStatusCondition.wakeAll();
}

void PrlHandleServerJob::InitializeError(const QString &strUuid, PRL_RESULT error_code, PRL_CONST_STR strErrorSource)
{
	QMutexLocker _lock(&m_JobStatusMutex);
	m_Result.setRequestId(strUuid);
	m_Result.setResultObjectType(PVE::GenericResult);
	m_Result.setReturnCode(error_code);

	CVmEvent *pEvent = new CVmEvent(PET_DSP_EVT_ERROR_MESSAGE,
    Uuid().toString(),
    PIE_WEB_SERVICE,
	error_code,
    PVE::EventRespNotRequired,
    strErrorSource);

	m_Result.setError(pEvent);
	m_JobStatus = PJS_FINISHED;
	m_JobStatusCondition.wakeAll();
}

PrlHandleServerJobPtr PrlHandleServerJob::GetJobByUuid( QString job_uuid )
{
	QMutexLocker _lock(::getJobUuidMapMutex());
	if (NULL == _lock.mutex())
		return PrlHandleServerJobPtr((PrlHandleServerJob *)0);

	JobUuidMap::iterator it = ::getJobUuidMap()->find( job_uuid );

	if ( it == ::getJobUuidMap()->end() )
		return PrlHandleServerJobPtr((PrlHandleServerJob *)0);

	return PrlHandleServerJobPtr(it->second);
}

PRL_RESULT PrlHandleServerJob::GetError( PRL_HANDLE_PTR phEvent )
{
	QMutexLocker _lock(&m_JobStatusMutex);
	CVmEvent *pError = m_Result.GetError();
	if (!pError)
		return (PRL_ERR_NO_DATA);
	if (PRL_SUCCEEDED(pError->getEventCode()))//Seems no error was specified
		return (PRL_ERR_NO_DATA);

	PrlHandleVmEvent *pEvent = new PrlHandleVmEvent(m_pServer, PET_VM_INF_UNINITIALIZED_EVENT_CODE, pError);
	if (!pEvent)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phEvent = pEvent->GetHandle();

	return (PRL_ERR_SUCCESS);
}

void PrlHandleServerJob::SetReturnCode(PRL_RESULT nRetCode)
{
	QMutexLocker _lock(&m_JobStatusMutex);
	m_Result.setReturnCode(nRetCode);
	m_JobStatus = PJS_FINISHED;
	m_JobStatusCondition.wakeAll();
}

CResult PrlHandleServerJob::GetResult()
{
	QMutexLocker _lock(&m_JobStatusMutex);
	return (m_Result);
}

PRL_JOB_STATUS PrlHandleServerJob::GetInternalJobStatus()
{
	QMutexLocker _lock(&m_JobStatusMutex);
	return (m_JobStatus);
}
