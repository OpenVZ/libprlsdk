/*
 * PrlHandleJob.h
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


#ifndef __VIRTUOZZO_HANDLE_JOB_H__
#define __VIRTUOZZO_HANDLE_JOB_H__


#include "PrlHandleBase.h"

#define GENERATE_ERROR_HANDLE(error_code, job_type)\
	craftError(error_code, __FUNCTION__, job_type)

/**
 * Generates job handle that contains error description
 * @param error code
 * @param error source
 * @param job async operation type
 * @return handle contains error description
 */
PrlHandleJobPtr CreateErrorHandle(PRL_RESULT error_code,
                                  PRL_CONST_STR strErrorSource,
                                  PRL_JOB_OPERATION_CODE nJobOpCode);

/**
 * Job handle object.
 */
class PrlHandleJob : public PrlHandleBase
{

protected:
	/**
	 * Object's constructor is protected to prevent direct
	 * object creation in non-inherited blocks of code.
	 * @param operation code which job belongs
	 * @param sign specifies whether request was sent to the remote side
	 */
	PrlHandleJob(PRL_JOB_OPERATION_CODE nJobOpCode, bool bIsRequestWasSent = false);

	/**
	 * Object's destructor is virtual to allow correct inheritance
	 */
	virtual ~PrlHandleJob();

public:
	/**
	 * Suspend calling thread and wait for the competion of the passed job.
	 * Job object would not be destroied and could be queried until it's destruction
	 * with the call to the PrlHandle_Free.
	 * @param timeout in milliseconds of job wait
	 */
	virtual PRL_RESULT Wait(PRL_UINT32 msecs) = 0;

	/**
	 * Cancelling job
	 */
	virtual PRL_HANDLE Cancel() = 0;

	/**
	 * Query job object status. There are a lot of potential models of usage for this
	 * method - one can query some job status in the thread to inform user about the
	 * status and prevent thread from blocking.
	 */
	virtual PRL_RESULT GetStatus( PRL_JOB_STATUS_PTR status ) = 0;

	/**
	 * Query job competion percentage. Clients may want to query the progress of the
	 * running job as a percentage value. Percentage granularity is not guaranteed but
	 * it would be '0' on the start and '100' on the finish.
	 */
	virtual PRL_RESULT GetProgress( PRL_UINT32_PTR percentage ) = 0;

	/**
	 * After the completion of the job (queried by PrlJob_GetStatus or others), clients
	 * can get the return code of the job with this method.
	 */
	virtual PRL_RESULT GetRetCode( PRL_RESULT_PTR retcode ) = 0;

	/**
	 * After the completion of the job (queried by PrlJob_GetStatus or others), clients
	 * can get the received data of the job with this method.
	 */
	virtual PRL_RESULT GetDataPtr( PRL_VOID_PTR data_ptr, PRL_UINT32_PTR size ) = 0;

	/**
	 * After the completion of the job (queried by PrlJob_GetStatus or others), clients
	 * can get the handle of job result.
	 */
	virtual PRL_RESULT GetResult( PRL_HANDLE_PTR handle ) = 0;

	/**
	 * Returns VM handle which current job belongs
	 */
	PRL_HANDLE GetVmHandle();
	/**
	 * Sets VM handle which current job belongs
	 */
	void SetVmHandle(PRL_HANDLE hVm);

	/**
	 * Overridable method that let to extract error object from result object
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA				- object doesn't have error data
	 * * PRL_ERR_OUT_OF_MEMORY	-	not enough memory to allocate error object
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	virtual PRL_RESULT GetError( PRL_HANDLE_PTR phEvent );

	/**
	 * Method that lets to extract job operation type
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetOpCode( PRL_JOB_OPERATION_CODE_PTR pnOpCode );

	/**
	 * Method that lets to extract whether request was sent sign
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT IsRequestWasSent( PRL_BOOL_PTR pbIsRequestWasSent );

	/**
	 * Get numeric identifier of the response package
	 * @param [out] where to store numeric identifier value
	 */
	virtual PRL_RESULT GetPackageId( PRL_UINT64_PTR id );

	/**
	 * Set numeric identifier of the response package
	 * @param numeric identifier of the response package
	 */
	void SetPackageId( PRL_UINT64 id );

public:
	/**
	 * Returns operation code which job belongs
	 */
	PRL_JOB_OPERATION_CODE GetOpCode() const;

private:
	/** VM handle which current job belongs */
	PRL_HANDLE m_VmHandle;
	/** Object for synchronizing VM handle access */
	QMutex m_VmHandleMutex;
	/** Operation code which job belongs */
	PRL_JOB_OPERATION_CODE m_nJobOpCode;
	/** Sign whether request was sent to the remote side */
	bool m_bIsRequestWasSent;
	/** Numeric identifier of the response package */
	PRL_UINT64 m_nRespPackageId;
};

typedef PrlHandleSmartPtr<PrlHandleJob> PrlHandleJobPtr;

inline PRL_HANDLE craftError(PRL_RESULT code_, PRL_CONST_STR text_, PRL_JOB_OPERATION_CODE type_)
{
	PrlHandleJobPtr h(CreateErrorHandle(code_, text_, type_));
	return h.isValid() ? h->GetHandle() : PRL_INVALID_HANDLE;
}

#endif // __VIRTUOZZO_HANDLE_JOB_H__

