/*
 * PrlHandleLoginLocalHelperJob.h
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


#ifndef __PARALLELS_HANDLE_LOGIN_LOCAL_HELPER_JOB_H__
#define __PARALLELS_HANDLE_LOGIN_LOCAL_HELPER_JOB_H__

#include "PrlHandleLocalJob.h"
#include "PrlHandleServer.h"

class CResult;

/**
 * Class that let emulates asynchronous job object for some local purposes.
 */
class PrlHandleLoginLocalHelperJob : public PrlHandleLocalJob
{
Q_OBJECT

public:
	/**
	 * Class constructor.
	 */
	PrlHandleLoginLocalHelperJob(
		PrlHandleServerPtr pServer,
		qlonglong nUserId,
		const char *strPrevSessionUuid,
		PRL_APPLICATION_MODE appMode,
		quint32 flags);

	/**
	 * Overloaded method of job cancel routine
	 */
	PRL_HANDLE Cancel();

	/**
	 * Job context
	 */
	void doJob();

	bool prepareDataForValidation(const QString& sFilePath,
								  const QString& sCheckData,
								  CResult* pResult);
	void switchToCompatibilityMode();
	bool isCompatibilityMode() const;

	bool isDelayedConnectionMode() const;

signals:

	void doSingleShot();

private slots:

	void onSingleShot();
	void onTryConnect();

private:

	PrlHandleServerPtr	m_pServer;
	qlonglong			m_nUserId;
	QString				m_qsPrevSessionUuid;
	PRL_APPLICATION_MODE m_nAppMode;
	quint32				m_nFlags;

	QMutex				m_mutex;
	enum _LoginStage
	{
		NoLogin = 0,
		LoginStage1,
		LoginStage2,
		LegacyLogin,
		LegacyLoginStage1,
	} m_iLoginStage;
};


#endif	// __PARALLELS_HANDLE_LOGIN_LOCAL_HELPER_JOB_H__
