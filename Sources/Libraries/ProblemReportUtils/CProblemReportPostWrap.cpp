/*
 * CProblemReportPostWrap.cpp: wrapper on problem report post
 * operation
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


#include <QtCore/QUrl>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtCore/QtConcurrentRun>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "CProblemReportPostWrap.h"

#include <prlcommon/Interfaces/ParallelsQt.h>
#include "SDK/Include/PrlErrors.h"
#include <prlcommon/Std/PrlAssert.h>
#include "Libraries/ProblemReportUtils/CProblemReportUtils.h"

#define PRL_LOOKUP_REPORT_HOST_TIMEOUT 30*1000 // 30 seconds
#define PRL_REPORT_SERVER_TIMEOUT 5*60*1000 // 5 minutes timeout to response from server
#define PRL_CONNECTING_TIMEOUT	2*60*1000 // 2 minutes timeout to connecting
#define PRL_PROGRESS_TIMEOUT	5*60*1000 // 5 minutes timeout to upload report
#define PROP_DELETE_ITSELF		"DeleteItself"

CProblemReportPostWrap::CProblemReportPostWrap()
: m_lookupId( -1 )
, m_iCurrentStep( initStep )
{
	init();
}

CProblemReportPostWrap::CProblemReportPostWrap( const QString& strTempDirPath,
												const QNetworkProxy& proxy,
												CPackedProblemReport::packedReportSide side )
: m_lookupId( -1 )
, m_iCurrentStep( initStep )
, m_ignore_cert_errors( false )
{
	init();
	initProblemReport(side, strTempDirPath);

	setProxy( proxy );

	setProperty( PROP_DELETE_ITSELF, QVariant(true) );
}

void CProblemReportPostWrap::init()
{
	setProperty( PROP_DELETE_ITSELF, QVariant(false) );

	m_timeoutTimer.setSingleShot( true );
	setIgnoreCertifErrors( false );

	bool bRes = connect( &m_timeoutTimer, SIGNAL( timeout() ), this, SLOT( onTimeout() ) );

	bRes = bRes && connect( &m_netManager, SIGNAL( finished( QNetworkReply* ) ), this, SLOT( onRequestFinished( QNetworkReply* ) ) );
	bRes = bRes && connect( &m_netManager, SIGNAL( sslErrors (QNetworkReply * , const QList<QSslError> & ) ),
		this, SLOT( ignoreSslErrors( QNetworkReply *, const QList<QSslError> &) ) );

	bRes = bRes && connect(  &m_netManager, SIGNAL(  proxyAuthenticationRequired(  const QNetworkProxy & , QAuthenticator * ) ),
				   SIGNAL(  proxyAuthenticationRequired(  const QNetworkProxy & , QAuthenticator * ) ) );
	PRL_ASSERT( bRes );
}

CProblemReportPostWrap::~CProblemReportPostWrap()
{
	// temporally directory will be clean up by caller
	cleanupProblemReport();
}

void CProblemReportPostWrap::cleanupProblemReport(bool bCleanupTempDir)
{
    if( m_pReport )
		m_pReport->setCleanupTempDir( bCleanupTempDir );
    m_pReport.reset(0);
}

void CProblemReportPostWrap::initProblemReport(SmartPtr<CPackedProblemReport> pPR)
{
    PRL_ASSERT(pPR);

    m_iCurrentStep = initStep;
    bool bRes = connect( &m_packWatcher, SIGNAL( finished() ), SLOT( doHostLookup() ) );
    PRL_ASSERT( bRes );

    if ( m_pReport )
        cleanupProblemReport();
    m_pReport = pPR;
}


void CProblemReportPostWrap::initProblemReport( CPackedProblemReport::packedReportSide side,
                                                const QString& strTempDirPath )
{
	m_iCurrentStep = initStep;

	bool bRes = connect( &m_packWatcher, SIGNAL( finished() ), SLOT( doHostLookup() ) );
	PRL_ASSERT( bRes );

	// create packed report wrapper
    CPackedProblemReport * pTmpReport = NULL;
    if (strTempDirPath.isEmpty())
        CPackedProblemReport::createInstance(side, &pTmpReport);
    else
        CPackedProblemReport::createInstance(side, &pTmpReport, strTempDirPath);

    if (pTmpReport == NULL)
        WRITE_TRACE(DBG_FATAL, "Failed to init Problem Report!");

    if ( m_pReport )
		cleanupProblemReport();
    m_pReport.reset(pTmpReport);
}

CPackedProblemReport* CProblemReportPostWrap::getProblemReport()
{
    return ( !m_pReport ) ? NULL : m_pReport.getImpl();
}

void CProblemReportPostWrap::setProxy( const QNetworkProxy& proxy )
{
	m_netManager.setProxy( proxy );
}

void CProblemReportPostWrap::setData( const QByteArray& data )
{
	m_data = data;
}

void CProblemReportPostWrap::setIgnoreCertifErrors( bool ignore )
{
	m_ignore_cert_errors = ignore;
}

PRL_RESULT packReport( CPackedProblemReport * pReport )
{
	if( !pReport )
		return PRL_ERR_FAILURE;

	return pReport->packReport();
}

void CProblemReportPostWrap::start()
{
	m_lstUrls = CProblemReportUtils::getReportUrlsList(
        true, false, (!m_pReport) ? -1 : m_pReport->getReportType() );

	if ( ! m_data.isEmpty() )
	{
		m_iCurrentStep = lookupHostsStep;
		lookupNextHost();
		return;
	}

	m_iCurrentStep = packReportStep;

	if( !m_pReport )
		finishPost( PRL_ERR_FAILURE );

    QFuture<int> future = QtConcurrent::run( ::packReport, m_pReport.getImpl() );
	m_packWatcher.setFuture( future );
}

QNetworkAccessManager* CProblemReportPostWrap::networkAccessManager()
{
	return &m_netManager;
}

bool CProblemReportPostWrap::isFinished() const
{
	return m_iCurrentStep == finishedStep;
}

void CProblemReportPostWrap::doHostLookup()
{
	int iRes = m_packWatcher.result();
	if( PRL_FAILED( iRes ) )
	{
		finishPost( iRes );
		return;
	}

    if( (!m_pReport) || !m_pReport->isValid() )
	{
		finishPost( PRL_ERR_FAILURE );
		return;
	}

	m_iCurrentStep = lookupHostsStep;

    if ( m_pReport )
		m_strFilePath = m_pReport->getArchivePath();
	lookupNextHost();
}

void CProblemReportPostWrap::lookupNextHost( )
{
	// all hosts was researched - no valid found..
	if( m_lstUrls.isEmpty() )
	{
		finishPost( PRL_ERR_FAILURE );
		return;
	}

	QString strHost = m_lstUrls.at( 0 );
	WRITE_TRACE( DBG_INFO, "lookup host = %s",QSTR2UTF8( strHost ) );

	QString host = QUrl( strHost ).host();
	QHostAddress checkRemoteHost( host );

	m_timeoutTimer.stop();
	// IP is entered
	if ( ! checkRemoteHost.isNull() )
		doHttpPost( QUrl( strHost ) );
	else
	{
		m_lookupId = QHostInfo::lookupHost( host, this, SLOT( onHostLookup( QHostInfo ) ) );
		m_timeoutTimer.start( PRL_LOOKUP_REPORT_HOST_TIMEOUT );
	}
}

void CProblemReportPostWrap::onHostLookup( QHostInfo info )
{
	if ( info.error() != QHostInfo::NoError
		|| info.addresses().size() == 0 )
	{
		if (m_lstUrls.isEmpty())
			return;

		QString strHost = m_lstUrls.takeFirst();
		WRITE_TRACE( DBG_FATAL, "Error: can't do lookup for remote "
			"host name: %s error:%d", QSTR2UTF8( strHost ), info.error() );

		lookupNextHost();
		return;
	}

	QHostAddress addr;
	for ( int i = 0; i < info.addresses().size(); ++i )
	{
		if ( info.addresses().at(i).protocol() == QAbstractSocket::IPv4Protocol )
		{
			addr = info.addresses().at(i);
			break;
		}
	}

	if ( addr.isNull() )
	{
		if (m_lstUrls.isEmpty())
			return;

		QString strHost = m_lstUrls.takeFirst();
		WRITE_TRACE(DBG_FATAL, "Error: can't do lookup for remote "
			"host name: %s, address is null.", QSTR2UTF8( strHost ) );
		lookupNextHost();
		return;
	}

	WRITE_TRACE( DBG_INFO, "set host = %s",QSTR2UTF8( addr.toString() ) );
	doHttpPost( QUrl( m_lstUrls.at( 0 ) ) );
}

void CProblemReportPostWrap::finishPost( PRL_RESULT res )
{
	if (m_iCurrentStep == finishedStep)
		return;

	WRITE_TRACE( DBG_FATAL, "Problem report finished post with result 0X%X", res );
	m_timeoutTimer.stop();

	disconnect(SIGNAL( uploadProgress ( qint64, qint64 ) ));
	m_pReply = 0;
	m_packWatcher.disconnect(SIGNAL( finished() ));

	emit complete( res, m_lststrResponceParams );

	if ( property( PROP_DELETE_ITSELF ).toBool() )
	{
		deleteLater();
		setProperty( PROP_DELETE_ITSELF, QVariant(false) );
	}

	m_iCurrentStep = finishedStep;
}

void CProblemReportPostWrap::doHttpPost( const QUrl & url )
{
	m_iCurrentStep = httpPostStep;
	QNetworkRequest request;
	request.setUrl( url );

	QByteArray data = m_data;
	if ( data.isEmpty() )
		CProblemReportUtils::getDataFromFile( m_strFilePath, data );

	CProblemReportUtils::formNetworkRequestToPost( request, data );

	m_pReply = m_netManager.post( request, data );

	if( ! m_pReply )
	{
		finishPost( PRL_ERR_FAILURE );
		return;
	}

	m_timeoutTimer.start( PRL_CONNECTING_TIMEOUT );

	bool bRes = connect( m_pReply, SIGNAL( uploadProgress ( qint64, qint64 ) ),
					this, SIGNAL( uploadProgress ( qint64, qint64 ) ) );

	bRes = bRes && connect( m_pReply, SIGNAL( uploadProgress ( qint64, qint64 ) ),
					this, SLOT( onUploadProgress ( qint64, qint64 ) ) );

	PRL_ASSERT( bRes );
}

void CProblemReportPostWrap::cancel()
{
	switch( m_iCurrentStep )
	{
		case finishedStep:
			break;

		case lookupHostsStep:
			QHostInfo::abortHostLookup( m_lookupId );
			finishPost( PRL_ERR_OPERATION_WAS_CANCELED );
			break;

		case packReportStep:
			// pack operation will be return PRL_ERR_OPERATION_WAS_CANCELED later
            if( m_pReport )
				m_pReport->setQuitFromPack( true );
			else
				finishPost( PRL_ERR_OPERATION_WAS_CANCELED );
			break;

		case initStep:
		case httpPostStep:
		default:
			finishPost( PRL_ERR_OPERATION_WAS_CANCELED );
			break;

	}
}

void CProblemReportPostWrap::onTimeout()
{
	switch( m_iCurrentStep )
	{
		case finishedStep:
			break;

		case initStep:
		case packReportStep:
			// no timeout for this step!
			PRL_ASSERT( false );
			break;

		case lookupHostsStep:
			QHostInfo::abortHostLookup( m_lookupId );
			onHostLookup( QHostInfo() );
			break;

		case httpPostStep:
			finishPost( PRL_ERR_TIMEOUT );
			break;
	}
}

void CProblemReportPostWrap::onUploadProgress( qint64 iDone, qint64 iTotal )
{
	if( iDone != 0 )
	{
		// timeout between posted portions
		m_timeoutTimer.start( PRL_PROGRESS_TIMEOUT );

		int iPercent = (int)(((double)iDone/(double)iTotal)*100);
		if ( iPercent == 100 )
			m_timeoutTimer.start( PRL_REPORT_SERVER_TIMEOUT );
	}
}

void CProblemReportPostWrap::onRequestFinished( QNetworkReply * pReply )
{
	PRL_RESULT res = PRL_ERR_SUCCESS;
	switch( pReply->error() )
	{
		case QNetworkReply::NoError:
			{
				QString strResponce = pReply->readAll();
				WRITE_TRACE( DBG_FATAL, "request finished with data = %s", QSTR2UTF8( strResponce ) );
				m_lststrResponceParams = strResponce.split(";", QString::SkipEmptyParts);
				res = PRL_ERR_SUCCESS;
			}
			break;

		case QNetworkReply::OperationCanceledError:
			res = PRL_ERR_OPERATION_WAS_CANCELED;
			break;

		case QNetworkReply::HostNotFoundError:
			res = PRL_ERR_HTTP_HOST_NOT_FOUND;
			break;

		case QNetworkReply::ConnectionRefusedError:
			res = PRL_ERR_HTTP_CONNECTION_REFUSED;
			break;

		case QNetworkReply::RemoteHostClosedError:
			res = PRL_ERR_HTTP_UNEXPECTED_CLOSE;
			break;

		case QNetworkReply::ProxyAuthenticationRequiredError:
			res = PRL_ERR_HTTP_PROXY_AUTH_REQUIRED;
			break;

		case QNetworkReply::AuthenticationRequiredError:
			res = PRL_ERR_HTTP_AUTH_REQUIRED;
			break;

		case QNetworkReply::ProtocolFailure:
			res = PRL_ERR_HTTP_INVALID_RESPONSE_HEADER;
			break;

		case QNetworkReply::ContentReSendError:
			res = PRL_ERR_HTTP_WRONG_CONTENT_LENGTH;
			break;

		default:
			WRITE_TRACE(DBG_FATAL, "QNetworkReply::error() == %d",pReply->error() );
			res = PRL_ERR_HTTP_PROBLEM_REPORT_SEND_FAILURE;
	}

	pReply->deleteLater();

	finishPost( res );
}

bool CProblemReportPostWrap::isNeedIgnoreErrorForReport( QSslError::SslError error )
{
	switch ( error )
	{
		case QSslError::HostNameMismatch:
		case QSslError::SubjectIssuerMismatch:
		// user can switch time to out valid certificate interval
		case QSslError::CertificateNotYetValid:
		case QSslError::CertificateExpired:
			return true;
		// used for internal tests only.
		case QSslError::SelfSignedCertificate:
		case QSslError::CertificateUntrusted:
			return m_ignore_cert_errors;
		default:
			return false;
	}
}

void CProblemReportPostWrap::ignoreSslErrors( QNetworkReply * reply, const QList<QSslError> & list)
{
#if defined(_WIN_) && defined(QT_NO_OPENSSL)
	(void) list;
#pragma message("ATTENTION: Compiling CParallelsProblemReportMailProgress without SSL support!")
#else
	bool bIgnore = true;
	for( int i = 0 ; i < list.size() ; i++ )
	{
		WRITE_TRACE(DBG_FATAL, "error == %s", QSTR2UTF8(list[i].errorString()) );
		if( !isNeedIgnoreErrorForReport( list[i].error() ) )
			bIgnore = false;
	}
	if( bIgnore )
		reply->ignoreSslErrors();
#endif
}
