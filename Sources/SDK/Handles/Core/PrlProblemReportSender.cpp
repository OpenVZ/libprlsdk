/*
 * PrlProblemReportSender.cpp: Implementation of problem report
 * sender mech
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


#include "PrlProblemReportSender.h"

#include "XmlModel/ProblemReport/CProblemReport.h"
#include "XmlModel/Messaging/CVmEvent.h"
#include "XmlModel/Messaging/CVmEventParameter.h"
#include "XmlModel/Messaging/CVmBinaryEventParameter.h"

#include "PrlHandleVmEvent.h"
#include "PrlHandleVm.h"

#include <prlcommon/PrlCommonUtilsBase/CUrlParser.h>
#include "Libraries/ProblemReportUtils/CProblemReportUtils.h"
#include "Libraries/ProblemReportUtils/CProblemReportPostWrap.h"
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/Logging/Logging.h>

#ifdef _WIN_
#ifndef QT_NO_OPENSSL
#include <QSslSocket>
#endif
#endif

using namespace Parallels;

PrlProblemReportSender::PrlProblemReportSender(const PrlHandleLocalJobPtr &pSendReportJob,
		PRL_CONST_STR sProblemReport,
		PRL_BOOL bUseProxy,
		PRL_CONST_STR sProxyHost,
		PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin,
		PRL_CONST_STR sProxyUserPasswd,
		PRL_UINT32 nProblemSendTimeout,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR pUserData)
:
m_pSendReportJob(pSendReportJob),
m_sProblemReport(UTF8_2QSTR(sProblemReport)),
m_bUseProxy(bUseProxy),
m_sProxyHost(UTF8_2QSTR(sProxyHost ? sProxyHost : "")),
m_nProxyPort(nProxyPort),
m_sProxyUserLogin(UTF8_2QSTR(sProxyUserLogin ? sProxyUserLogin : "")),
m_sProxyUserPasswd(UTF8_2QSTR(sProxyUserPasswd ? sProxyUserPasswd : "")),
m_nProblemSendTimeout(nProblemSendTimeout),
m_handler(handler),
m_pUserData(pUserData),
m_netProxy(QNetworkProxy::HttpProxy),
m_pPRSender(new CProblemReportPostWrap()),
m_bWasCanceled(false),
m_nPrevPercents(0)
{
}

PrlProblemReportSender::PrlProblemReportSender(const PrlHandleLocalJobPtr &pSendReportJob,
		PrlHandleProblemReportPtr pProblemReport,
		PRL_BOOL bUseProxy,
		PRL_CONST_STR sProxyHost,
		PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin,
		PRL_CONST_STR sProxyUserPasswd,
		PRL_UINT32 nProblemSendTimeout,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR pUserData)
:
m_pSendReportJob(pSendReportJob),
m_pProblemReport(pProblemReport),
m_bUseProxy(bUseProxy),
m_sProxyHost(UTF8_2QSTR(sProxyHost ? sProxyHost : "")),
m_nProxyPort(nProxyPort),
m_sProxyUserLogin(UTF8_2QSTR(sProxyUserLogin ? sProxyUserLogin : "")),
m_sProxyUserPasswd(UTF8_2QSTR(sProxyUserPasswd ? sProxyUserPasswd : "")),
m_nProblemSendTimeout(nProblemSendTimeout),
m_handler(handler),
m_pUserData(pUserData),
m_netProxy(QNetworkProxy::HttpProxy),
m_pPRSender(new CProblemReportPostWrap()),
m_bWasCanceled(false),
m_nPrevPercents(0)
{
}

PrlProblemReportSender::~PrlProblemReportSender()
{
	m_pPRSender->cancel();
	m_pPRSender->deleteLater();
	m_pPRSender = NULL;
}

void PrlProblemReportSender::concreteRun()
{
	CHECK_CONNECT_DONE_PROPERLY(connect(m_pSendReportJob.getHandle(), SIGNAL(jobWasCanceled()), SLOT(onJobWasCancelled()), Qt::DirectConnection))

// Check whether proper problem report was passed

	if (m_pProblemReport.isValid())
	{
		if (PRS_OLD_XML_BASED == m_pProblemReport->GetScheme())
		{
			m_pSendReportJob->SetReturnCode(PRL_ERR_INVALID_ARG);
			notifyWithUserCallback(m_pSendReportJob->GetHandle());
			return;
		}
	}
	else
	{
		CProblemReportUtils::CorrectNonPrintableSymbols(m_sProblemReport);

		m_pProblemReport = new PrlHandleProblemReport(
						PrlHandleProblemReport::CreateReportData(PRS_NEW_PACKED));

		if (PRL_FAILED( m_pProblemReport->fromString(m_sProblemReport.toUtf8()) ))
		{
			m_pSendReportJob->SetReturnCode(PRL_ERR_INVALID_ARG);
			notifyWithUserCallback(m_pSendReportJob->GetHandle());
			return;
		}

		PRL_RESULT r = m_pProblemReport->Pack();
		if (PRL_FAILED(r))
		{
			m_pSendReportJob->SetReturnCode(r);
			notifyWithUserCallback(m_pSendReportJob->GetHandle());
			return;
		}
	}

// Configure proxy settings

	if (m_bUseProxy)
	{
		QString sHost, sUser, sPasswd;
		uint nPort = 0;
		DetectSystemProxySettings(sHost, nPort, sUser, sPasswd);
		sHost = (m_sProxyHost.isEmpty() ? sHost : m_sProxyHost);
		nPort = (0 == m_nProxyPort ? nPort : m_nProxyPort);
		sUser = (m_sProxyUserLogin.isEmpty() ? sUser : m_sProxyUserLogin);
		sPasswd = (m_sProxyUserPasswd.isEmpty() ? sPasswd : m_sProxyUserPasswd);

		if ( ! sHost.isEmpty() )
		{
			m_netProxy.setHostName(sHost);
			m_netProxy.setPort((quint16 )nPort);
			m_netProxy.setUser(sUser);
			m_netProxy.setPassword(sPasswd);

			QMutexLocker _lock(&m_mutex);
			m_pPRSender->setProxy(m_netProxy);
		}
	}

// Send report

	if (m_bWasCanceled)
		return;

	QMutexLocker _lock(&m_mutex);

	PrlProblemReportProxySender* pProxySender = new PrlProblemReportProxySender();

	CHECK_CONNECT_DONE_PROPERLY(connect( m_pPRSender,
										 SIGNAL(complete(PRL_RESULT , QStringList )),
										 pProxySender,
										 SIGNAL(complete(PRL_RESULT , QStringList )),
										 Qt::QueuedConnection ))
	CHECK_CONNECT_DONE_PROPERLY(connect( m_pPRSender,
										 SIGNAL(uploadProgress(qint64 , qint64 )),
										 pProxySender,
										 SIGNAL(uploadProgress(qint64 , qint64 )),
										 Qt::QueuedConnection ))

	CHECK_CONNECT_DONE_PROPERLY(connect( pProxySender,
										 SIGNAL(complete(PRL_RESULT , QStringList )),
										 this,
										 SLOT(onRequestFinished(PRL_RESULT , QStringList )),
										 Qt::DirectConnection ))
	CHECK_CONNECT_DONE_PROPERLY(connect( pProxySender,
										 SIGNAL(uploadProgress(qint64 , qint64 )),
										 this,
										 SLOT(onProgressChanged(qint64 , qint64 )),
										 Qt::DirectConnection ))
	_lock.unlock();

	QByteArray data = m_pProblemReport->GetBinaryData();

	_lock.relock();
	m_pPRSender->setData( data );
	m_pPRSender->start();
	_lock.unlock();

	exec();

	delete pProxySender;
}

void PrlProblemReportSender::DetectSystemProxySettings(QString &sHost, uint &nPort, QString &sUser, QString &sPasswd)
{
#ifndef _WIN_
	char *sProxyInfo = getenv("http_proxy");
	if (sProxyInfo)
	{
		CUrlParser _url(UTF8_2QSTR(sProxyInfo));
		sHost = _url.host();
		nPort = _url.port();
		sUser = _url.userName();
		sPasswd = _url.password();
        }
#else
	PRL_UNUSED_PARAM(sUser);
	PRL_UNUSED_PARAM(sPasswd);
#endif
}

void PrlProblemReportSender::onJobWasCancelled()
{
	QMutexLocker _lock(&m_mutex);
	m_bWasCanceled = true;
	m_pPRSender->cancel();
}

void PrlProblemReportSender::onRequestFinished(PRL_RESULT nRetCode, QStringList responseParams)
{
	QMutexLocker _lock(&m_mutex);

	if (PRL_SUCCEEDED(nRetCode))
	{
		QString serverStatus, reportId;
		if( responseParams.size() > 1 )
		{
			serverStatus = responseParams[0];

			reportId = responseParams[1];
			reportId.replace("id=", "");
		}

		if (serverStatus != "status=OK")
			m_pSendReportJob->SetReturnCode(PRL_ERR_HTTP_PROBLEM_REPORT_SEND_FAILURE);
		else
		{
			CResult _result(m_pSendReportJob->GetJobUuid(), PVE::DspCmdSendProblemReport, PRL_ERR_SUCCESS);
			_result.addParamToken(reportId);
			m_pSendReportJob->SetCResult(_result);

			WRITE_TRACE( DBG_FATAL, "Problem report has been sent. The report ID is %s", QSTR2UTF8(reportId) );

			m_pSendReportJob->SetReturnCode(PRL_ERR_SUCCESS);
		}
	}
	else
	{
		m_pSendReportJob->SetReturnCode(nRetCode);
	}

	notifyWithUserCallback(m_pSendReportJob->GetHandle());

	_lock.unlock();

	quit();
}

void PrlProblemReportSender::onProgressChanged(qint64 nDone, qint64 nTotal)
{
	if ( ! m_handler || ! nTotal )
		return;

	quint32 nCurrentPercent = quint32((nDone*100)/nTotal);
	if (!nCurrentPercent || m_nPrevPercents == nCurrentPercent)//Skip already notified or null percents value
		return;
	CVmEvent event(	PET_DSP_EVT_JOB_PROGRESS_CHANGED,
		Uuid().toString(),
		PIE_UNKNOWN);
	event.addEventParameter(new CVmEventParameter( PVE::UnsignedInt,
		QString::number(nCurrentPercent),
		EVT_PARAM_PROGRESS_CHANGED));
	PrlHandleVmEvent *pEvent = new PrlHandleVmEvent(PrlHandleVmPtr(0), &event);
	notifyWithUserCallback(pEvent->GetHandle());
	pEvent->Release();
	m_nPrevPercents = nCurrentPercent;
}

void PrlProblemReportSender::notifyWithUserCallback(PRL_HANDLE hEvent)
{
	if (m_handler)
	{
		PrlHandle_AddRef(hEvent);
		m_handler(hEvent, m_pUserData);
	}
}

