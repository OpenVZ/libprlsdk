/*
 * PrlHandleLocalJob.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#ifndef __PARALLELS_HANDLE_LOCAL_JOB_H__
#define __PARALLELS_HANDLE_LOCAL_JOB_H__

#include <QObject>
#include "PrlHandleServerJob.h"

/**
 * Class that let emulates asynchronous job object for some local purposes.
 */
class PrlHandleLocalJob : public QObject, public PrlHandleServerJob
{
Q_OBJECT

public:

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param job unique identifier
	 * @param job async operation type
	 */
	PrlHandleLocalJob( const QString& job_uuid, PRL_JOB_OPERATION_CODE nJobOpCode );

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	virtual ~PrlHandleLocalJob();

	/**
	 * Cancelling job
	 */
	virtual PRL_HANDLE Cancel();

	/**
	 * Returns sign whether job was canceled
	 */
	bool wasCanceled();

signals:
	/** Signal that notifies that job was canceled */
	void jobWasCanceled();
};

typedef PrlHandleSmartPtr<PrlHandleLocalJob> PrlHandleLocalJobPtr;

#endif // __PARALLELS_HANDLE_LOCAL_JOB_H__
