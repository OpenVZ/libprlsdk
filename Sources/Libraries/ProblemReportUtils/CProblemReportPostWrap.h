/*
 * CProblemReportPostWrap.h: wrapper on problem report post
 * operation
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


#ifndef CPROBLEMREPORT_POST_WRAP_H
#define CPROBLEMREPORT_POST_WRAP_H

#include "SDK/Include/PrlTypes.h"

#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QFutureWatcher>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QSslError>

#include "Libraries/ProblemReportUtils/CPackedProblemReport.h"


class QNetworkReply;
class QAuthenticator;
class QNetworkProxy;
class QSslError;
class QHostInfo;
class QString;
class QUrl;
class QTimer;
class CPackedProblemReport;

class CProblemReportPostWrap: public QObject
{
	Q_OBJECT
public:

	CProblemReportPostWrap();
	CProblemReportPostWrap(
			const QString& ,
			const QNetworkProxy& ,
			CPackedProblemReport::packedReportSide side = CPackedProblemReport::ClientSide
		);
	~CProblemReportPostWrap();

	void initProblemReport( CPackedProblemReport::packedReportSide side,
                            const QString& strTempDirPath = QString() );
    void initProblemReport(SmartPtr<CPackedProblemReport> pPR);
	void cleanupProblemReport( bool bCleanupTempDir = false );
	CPackedProblemReport* getProblemReport();

	void setProxy( const QNetworkProxy& proxy );
	void setData( const QByteArray& data );
	void setIgnoreCertifErrors( bool ignore );

	void start();

	QNetworkAccessManager* networkAccessManager();

	bool isFinished() const;

public slots:
	void cancel();

private slots:
	void onHostLookup( QHostInfo /*info*/ );
	void onRequestFinished( QNetworkReply * );
	void ignoreSslErrors( QNetworkReply *, const QList<QSslError> &);
	void doHostLookup();
	void onTimeout();
	void onUploadProgress( qint64, qint64 );

signals:
	void complete( PRL_RESULT, QStringList );
	void uploadProgress ( qint64, qint64 );
	void proxyAuthenticationRequired( const QNetworkProxy & , QAuthenticator * );

private:
	void init();
	void finishPost( PRL_RESULT );
	void doHttpPost( const QUrl & /*url*/ );
	void lookupNextHost();
	bool isNeedIgnoreErrorForReport( QSslError::SslError error );
	enum
	{
		initStep = -1,
		packReportStep = 0,
		lookupHostsStep,
		httpPostStep,
		finishedStep,
	}PostSteps;

private:
	QNetworkAccessManager			m_netManager;
    SmartPtr<CPackedProblemReport>	m_pReport;
	QPointer<QNetworkReply>			m_pReply;
	QByteArray						m_data;
	QTimer							m_timeoutTimer;
	QStringList						m_lstUrls;
	int								m_lookupId;
	QFutureWatcher<int>				m_packWatcher;
	QString							m_strFilePath;
	QStringList						m_lststrResponceParams;
	int								m_iCurrentStep;
	bool							m_ignore_cert_errors;
};

#endif //CPROBLEMREPORT_POST_WRAP_H
