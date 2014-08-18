/*
 * PrlHandleLoginHelperJob.h
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


#ifndef __PARALLELS_HANDLE_LOGIN_HELPER_JOB_H__
#define __PARALLELS_HANDLE_LOGIN_HELPER_JOB_H__

#include "PrlHandleLocalJob.h"
#include "PrlHandleServer.h"

/**
 * Class that let emulates asynchronous job object for some local purposes.
 */
class PrlHandleLoginHelperJob : public PrlHandleLocalJob
{
public:
	/**
	 * Class constructor.
	 */
	PrlHandleLoginHelperJob(
		PrlHandleServerPtr pServer,
		PRL_CONST_STR user,
		PRL_CONST_STR passwd,
		PRL_CONST_STR sPrevSessionUuid,
		PRL_UINT32 flags
	);

	/**
	 * Overloaded method of job cancel routine
	 */
	PRL_HANDLE Cancel();

	/**
	 * Job context
	 */
	void doJob();

	void SetHost( const QString &host ) { m_sHost = host; }
	QString GetHost() const { return m_sHost; }
	PRL_UINT32 GetFlags() const { return m_flags; }

private:
	PrlHandleServerPtr m_pServer;
	QString m_sUser;
	QString m_sPassword;
	QString m_sPrevSessionUuid;
	QMutex m_mutex;
	bool m_bLoginRequestWasSent;
	QString m_sHost;
	PRL_UINT32 m_flags;
};

#endif // __PARALLELS_HANDLE_LOGIN_HELPER_JOB_H__

