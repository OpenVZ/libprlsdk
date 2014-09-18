/*
 * PrlHandleIOJob.h
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


#ifndef __PARALLELS_HANDLE_IO_JOB_H__
#define __PARALLELS_HANDLE_IO_JOB_H__

#include <QObject>
#include <QString>
#include <QRect>
#include <QByteArray>
#include <QSize>
#include <QEvent>
#include <QVector>

#include "PrlHandleJob.h"
#include "Libraries/IOService/src/Common/ExecChannel.h"

/**
 * IO job handle object.
 */
class PrlHandleIOJob : public PrlHandleJob
{
public:
	/** IO job types for inner use */
	enum IOJobType {
		IOConnectJob       = 0,
		IOSendJob          = 1,
	};

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param type of IO job
	 * @param desktop
	 */
	PrlHandleIOJob ( IOJobType type,
					const SmartPtr<IOService::ExecChannel>& channel = SmartPtr<IOService::ExecChannel>() );

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleIOJob ();

	/**
	 * Suspend calling thread and wait for the competion of the passed job.
	 * Job object would not be destroied and could be queried until it's destruction
	 * with the call to the PrlHandle_Free.
	 */
	PRL_RESULT Wait(PRL_UINT32);

	/**
	 * Cancelling job
	 */
	PRL_HANDLE Cancel();

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
	 * After the completion of the job (queried by PrlJob_GetStatus or others), clients
	 * can get the received data of the job with this method.
	 */
	PRL_RESULT GetDataPtr( PRL_VOID_PTR data_ptr, PRL_UINT32_PTR size);

	/**
	 * After the completion of the job (queried by PrlJob_GetStatus or others), clients
	 * can get the handle of job result.
	 */
	PRL_RESULT GetResult( PRL_HANDLE_PTR handle );

	PRL_RESULT GetError( PRL_HANDLE_PTR phEvent ) ;

	void RegisterRequest ( const IOService::IOSendJob::Handle& );

	/******************* Callbacks *******************************************/

	/** Screen captured callback */
	void ResponseReceived ( const IOService::IOSendJob::Handle& h,
							const SmartPtr<IOService::IOPackage>& p );

private:
	void HandleResponse ( const IOService::IOSendJob::Handle& h,
						  const SmartPtr<IOService::IOPackage>& p );
	IOService::Channel *GetIOChannel();


private:
	QMutex m_mutex;

	// Job type
	IOJobType m_jobType;
	// IO job handle
	IOService::IOSendJob::Handle m_ioJobHandle;
	// Responses
	QHash< IOService::IOSendJob::Handle,
		   SmartPtr<IOService::IOPackage> > m_waitingJobs;

	bool m_responseReceived;
	SmartPtr<IOService::ExecChannel> m_ioChannel;

	// Incoming vlipboard response package
	SmartPtr<IOService::IOPackage> m_pClipboardResponsePkg;
	SmartPtr<IOService::IOPackage> m_pShutdownResponsePkg;

	// Flag to handle capturing
	volatile bool m_isScreenCaptured;

	QByteArray m_captureBuffer;

	// Capture screen to file
	QString m_fileName;
	QString m_format;
	int m_quality;
	QSize m_thumbnail;
	volatile bool m_capturingToFile;
	volatile bool m_captureResult;

	bool	m_bIsCopyPasteAnswered;
	bool	m_bIsVmShutdownAnswered;

	/**
	 * Pointer to the object that contains additional error info
	 */
	PrlHandleVmEventPtr m_pError;

	PRL_UINT32	m_uShutdownResult;
};


#endif // __PARALLELS_HANDLE_IO_JOB_H__