/*
 * PrlProblemReportSender.h: Implementation of problem report sender
 * mech
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


#ifndef PrlProblemReportSender_H
#define PrlProblemReportSender_H

#include "PrlHandleLocalJob.h"
#include "PrlCommon.h"
#include "PrlHandleProblemReport.h"
#include <QMutex>
#include <QWaitCondition>
#include <QNetworkProxy>

class PrlProblemReportProxySender : public QObject
{
Q_OBJECT
signals:
	void complete( PRL_RESULT, QStringList );
	void uploadProgress ( qint64, qint64 );
};

/**
 * Problem report sender implementation
 */
class PrlProblemReportSender : public PrlThread
{
Q_OBJECT

public:
	/** Class constructor */
	PrlProblemReportSender(const PrlHandleLocalJobPtr &pSendReportJob,
			PrlHandleProblemReportPtr pProblemReport,
			PRL_BOOL bUseProxy,
			PRL_CONST_STR sProxyHost,
			PRL_UINT32 nProxyPort,
			PRL_CONST_STR sProxyUserLogin,
			PRL_CONST_STR sProxyUserPasswd,
			PRL_UINT32 nProblemSendTimeout,
			PRL_EVENT_HANDLER_PTR handler,
			PRL_VOID_PTR pUserData);
	/** Class constructor */
	PrlProblemReportSender(const PrlHandleLocalJobPtr &pSendReportJob,
			PRL_CONST_STR sProblemReport,
			PRL_BOOL bUseProxy,
			PRL_CONST_STR sProxyHost,
			PRL_UINT32 nProxyPort,
			PRL_CONST_STR sProxyUserLogin,
			PRL_CONST_STR sProxyUserPasswd,
			PRL_UINT32 nProblemSendTimeout,
			PRL_EVENT_HANDLER_PTR handler,
			PRL_VOID_PTR pUserData);
	/** Class destructor */
	~PrlProblemReportSender();

private:
	/** Overridden thread function */
	void concreteRun();
	/**
	 * Notifies with event registered user callback
	 * @param handle to the event object
	 */
	void notifyWithUserCallback(PRL_HANDLE hEvent);

private slots:
	/** Process job cancelation action */
	void onJobWasCancelled();
	/**
	 * Process request to problem report server completed
	 * @param id of request which was completed
	 * @param sign whether request was completed with error
	 */
	void onRequestFinished(PRL_RESULT nRetCode, QStringList responseParams);
	/**
	 * Process progress of request completion
	 * @param complete steps num
	 * @param total steps num
	 */
	void onProgressChanged(qint64 nDone, qint64 nTotal);
#ifdef _WIN_
	/**
	 * Processes proxy settings detection event under Win platform
	 * @param sign whether settings were detected
	 * @param detected host value
	 * @param detected port value
	 */
	void onProxyDetected(bool bSuccess, QString sHost, uint nPort);
#endif

private:
	/** Pointer to the send problem report job object */
	PrlHandleLocalJobPtr m_pSendReportJob;
	/** Pointer to the packed problem report representation object (new scheme) */
	PrlHandleProblemReportPtr m_pProblemReport;
	/** Sending problem report string representation (old scheme) */
	QString m_sProblemReport;
	/** Sign specifies whether proxy should be used */
	PRL_BOOL m_bUseProxy;
	/** Proxy host param */
	QString m_sProxyHost;
	/** Proxy port param*/
	PRL_UINT32 m_nProxyPort;
	/** Proxy user login param */
	QString m_sProxyUserLogin;
	/** Proxy user password param */
	QString m_sProxyUserPasswd;
	/** Sending problem report timeout */
	PRL_UINT32 m_nProblemSendTimeout;
	/** Events handler */
	PRL_EVENT_HANDLER_PTR m_handler;
	/** Pointer to the registered user data */
	PRL_VOID_PTR m_pUserData;
	/** Pointer to the HTTP object using for sending problem reports */
	QNetworkProxy m_netProxy;
	class CProblemReportPostWrap* m_pPRSender;
	/** Mutex for synchronizing access to the class members */
	QMutex m_mutex;
	/** Condition for waiting problem report send action competion */
	QWaitCondition m_cond;
	/** Sign whether operation was cnaceled */
	bool m_bWasCanceled;
	/** Value of previously notified progress percents to prevent double notification */
	quint32 m_nPrevPercents;
#ifdef _WIN_
	/** Intermediate vars using for proxy settings detection mech under Win platform*/
	QString m_sDetectedHost;
	uint m_nDetectedPort;
#endif

private:
	/**
	 * Detects system proxy settings
	 * @param [out] var for storing detected proxy host value
	 * @param [out] var for storing detected proxy port value
	 * @param [out] var for storing detected proxy user value
	 * @param [out] var for storing detected proxy password value
	 */
	void DetectSystemProxySettings(QString &sHost, uint &nPort, QString &sUser, QString &sPasswd);
};

#endif

