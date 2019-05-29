/*
 * PrlHandleServerJob.h
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


#ifndef __PARALLELS_HANDLE_SERVER_JOB_H__
#define __PARALLELS_HANDLE_SERVER_JOB_H__


#include "PrlHandleJob.h"
#include "PrlHandleServer.h"
#include <prlcommon/Messaging/CResult.h>
#include <QWaitCondition>

class PrlHandleResult;
class PrlHandleServerJob;

/**
 * Job handle object.
 */
class PrlHandleServerJob : public PrlHandleJob
{

public:

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param pointer to the server object where action takes place
	 * @param job unique identifier
	 * @param dispatcher protocol command ID which belongs this job
	 * @param sign specifies whether request was sent to the remote side
	 */
	PrlHandleServerJob(
		const PrlHandleServerPtr &server,
		const QString& job_uuid,
		PRL_JOB_OPERATION_CODE nJobOpCode,
		bool bIsRequestWasSent = true
		);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	virtual ~PrlHandleServerJob();

	/**
	 * Suspend calling thread and wait for the competion of the passed job.
	 * Job object would not be destroied and could be queried until it's destruction
	 * with the call to the PrlHandle_Free.
	 * @param timeout in milliseconds of job wait
	 */
	PRL_RESULT Wait(PRL_UINT32 msecs);

	/**
	 * Cancelling job
	 */
	virtual PRL_HANDLE Cancel();

	/**
	 * Query job object status. There are a lot of potential models of usage for this
	 * method - one can query some job status in the thread to inform user about the
	 * status and prevent thread from blocking.
	 */
	PRL_RESULT GetStatus( PRL_JOB_STATUS_PTR status );

	/**
	 * Query job competion percentage. Clients may want to query the progress of the
	 * running job as a percentage value. Percentage granularity is not guaranteed but
	 * it would be '0' on the start and '100' on the finish.
	 */
	PRL_RESULT GetProgress( PRL_UINT32_PTR percentage );

	/**
	 * After the completion of the job (queried by PrlJob_GetStatus or others), clients
	 * can get the return code of the job with this method.
	 */
	PRL_RESULT GetRetCode( PRL_RESULT_PTR retcode );

	/**
	 * If return code of job completion determined by the @ref PrlJob_GetRetCode() is not successfull
	 * this method can be used to concretize error code.
	 */
	PRL_RESULT GetErrCode( PRL_RESULT_PTR retcode );

	/**
	 * After the completion of the job (queried by PrlJob_GetStatus or others), clients
	 * can get the received data of the job with this method.
	 */
	PRL_RESULT GetDataPtr( PRL_VOID_PTR data_ptr, PRL_UINT32_PTR size );

	/**
	 * After the completion of the job (queried by PrlJob_GetStatus or others), clients
	 * can get the handle of job result.
	 */
	PRL_RESULT GetResult( PRL_HANDLE_PTR handle );

	/**
	 * Sets to job object with specified uuid command execution result
	 * @param result   - applying result
	 */
	virtual void SetCResult( const CResult& result );
	/**
	 * Initializes job result with specified error description
	 * @param job uuid
	 * @param error code
	 * @param error source
	 */
	void InitializeError(const QString &strUuid, PRL_RESULT error_code, PRL_CONST_STR strErrorSource);

	/**
	 * Return pointer to the job object by uuid identifier.
	 */
	static PrlHandleServerJobPtr GetJobByUuid( QString job_uuid );

	/** Returns reference on storing job result for internal SDK lib usage */
	CResult GetResult();

	/**
	 * Overridden method that let to extract error object from result object
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA				- object doesn't have error data
	 * * PRL_ERR_OUT_OF_MEMORY	-	not enough memory to allocate error object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetError( PRL_HANDLE_PTR phEvent );

	/**
	 * Lets to set job return code
	 * @param setting value
	 */
	virtual void SetReturnCode(PRL_RESULT nRetCode);

	/**
	 * Returns job UUID
	 */
	QString GetJobUuid() const;

private:
	/**
	 * Returns current job status (provides synchronized access to m_JobStatus member)
	 */
	PRL_JOB_STATUS GetInternalJobStatus();

private:

	/** Job owner */
	PrlHandleServerPtr m_pServer;

	/** Job uuid */
	QString m_Uuid;

	/** Job result object */
	CResult m_Result;

	/** Storing job status */
	PRL_JOB_STATUS m_JobStatus;

	/** Synchronizing job status access object */
	QMutex m_JobStatusMutex;

	/** Waiting job completion condition object */
	QWaitCondition m_JobStatusCondition;
};

#endif // __PARALLELS_HANDLE_SERVER_JOB_H__
