/*
 * CProblemReportUtils_common.cpp: Helper class for getting default
 * parallels urls locations.
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


#include <QUrl>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QStringList>

#include "CProblemReportUtils_common.h"
#include "Interfaces/ParallelsNamespace.h"
#include "Libraries/Std/PrlAssert.h"
#include "Build/Current.ver"
#include "Build/Current-locale.ver"
#include "ProblemReportLocalCertificates.h"
#include "CPackedProblemReport.h"

#include "CInstalledSoftwareCollector.h"
#include "Libraries/HostUtils/HostUtils.h"

#ifdef _WIN_
#include "Libraries/WmiWrap/WmiWrap.h"
#endif


namespace CProblemReportUtils
{
/**
* Contact maximk@ if you want access this server directly.
* The server that runs on that machine is actually a script:
*   Build/CrashServer/CrashServer.py
*/
#define CRASH_REPORT_PATH "/post"
#define CRASH_REPORT_PATH_OLD "/parallels/crash_report"
#define CRASH_REPORT_PORT "8080"
#define CRASH_REPORT_SERVER	"http://" CRASH_REPORT_SERVER_BASE ":" CRASH_REPORT_PORT CRASH_REPORT_PATH

// links for ssl mode
#define CRASH_REPORT_SSL_PORT "443"
#define CRASH_REPORT_SSL_SERVER	"https://" CRASH_REPORT_SERVER_BASE ":" CRASH_REPORT_SSL_PORT CRASH_REPORT_PATH

#define CEP_REPORT_PATH	CRASH_REPORT_PATH
#define CEP_REPORT_PORT	CRASH_REPORT_PORT
#define CEP_REPORT_SERVER	"http://" CEP_REPORT_SERVER_BASE ":" CEP_REPORT_PORT CEP_REPORT_PATH
#define CEP_REPORT_SSL_PORT	CRASH_REPORT_SSL_PORT
#define CEP_REPORT_SSL_SERVER	"https://" CEP_REPORT_SERVER_BASE ":" CEP_REPORT_SSL_PORT CEP_REPORT_PATH

QStringList getReportUrlsList( bool bSupportSsl, bool bUseOldPath, int nReportType )
{
	LOG_MESSAGE( DBG_FATAL, "===getReportUrlsList: bSupportSsl=%d, bUseOldPath=%d, nReportType=%d"
		, bSupportSsl, bUseOldPath, nReportType );

	QStringList urls;

	switch(nReportType)
	{
	case PRT_AUTOMATIC_STATISTICS_REPORT:
	case PRT_AUTOMATIC_VZ_STATISTICS_REPORT:
		if ( bSupportSsl )
			urls << CEP_REPORT_SSL_SERVER;
		else
			urls << CEP_REPORT_SERVER;
		break;
	default:
		if ( bSupportSsl )
			urls <<
				CRASH_REPORT_SSL_SERVER;
		else
			urls <<
				CRASH_REPORT_SERVER;
		break;
	}

	LOG_MESSAGE( DBG_FATAL, "===getReportUrlsList: urls=%s", QSTR2UTF8(urls.join(",")) );
	return urls;

}

QUrl getCrashReportServerUrlSync( bool bSupportSsl, bool bUseOldPath, int nReportType )
{
	QStringList crashHosts = getReportUrlsList( bSupportSsl, bUseOldPath, nReportType );

	QUrl url;
	// Try to resolve
	foreach ( QString crashHost, crashHosts )
	{
		WRITE_TRACE( DBG_FATAL, "host = %s",QSTR2UTF8( crashHost ) );
		QString host = QUrl(crashHost).host();
		QHostAddress checkRemoteHost( host );
		// IP is entered
		if ( ! checkRemoteHost.isNull() )
		{
			url = QUrl( crashHost );
			break;
		}
		else // Do lookup, if it is not an IP
		{
			QHostInfo info = QHostInfo::fromName(host);
			if ( info.error() != QHostInfo::NoError
				|| info.addresses().size() == 0
				)
			{
					WRITE_TRACE(DBG_FATAL, "Error: can't do lookup for remote "
						"host name: %s, will use IP",
						qPrintable(host));
					continue;
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
				WRITE_TRACE(DBG_FATAL, "Error: can't do lookup for remote "
					"host name: %s, will use IP",
					qPrintable(host));
				continue;
			}
			WRITE_TRACE( DBG_FATAL, "set host = %s",QSTR2UTF8( addr.toString() ) );
			url = QUrl( crashHost );
			url.setHost( addr.toString() );
			break;
		}
	}
	PRL_ASSERT( url.isValid() );

	return url;
}

void fillContentWithRequestData( QByteArray & content )
{
	QByteArray data = "--" + PRL_PROBLEM_REPORT_REQUEST_BOUNDARY + "\r\n" +
		"Content-Disposition: form-data; name=\"report\"; filename=\"report.tar.gz\"\r\n" +
		"Content-Type: application/octet-stream\r\n\r\n";

	content.prepend( data );
	data = "\r\n--" + PRL_PROBLEM_REPORT_REQUEST_BOUNDARY + "--";
	content.append( data );
}

void formNetworkRequestToPost( QNetworkRequest & request/*IN*/, QByteArray & data/*IN*/ )
{
	fillContentWithRequestData( data );

	request.setHeader( QNetworkRequest::ContentTypeHeader,
		"multipart/form-data; boundary=" + PRL_PROBLEM_REPORT_REQUEST_BOUNDARY );
	request.setHeader( QNetworkRequest::ContentLengthHeader, data.length() );
	request.setRawHeader( "host", request.url().host().toUtf8() );

	// setup ssl configuration
	QSslConfiguration config = QSslConfiguration::defaultConfiguration();
	config.setPeerVerifyMode( QSslSocket::VerifyPeer );
	config.setProtocol( QSsl::AnyProtocol );

	QSslCertificate certificate( CaReportCertificate );
	PRL_ASSERT( ! certificate.isNull() );
	if ( ! certificate.isNull() )
	{
		QList<QSslCertificate> list;
		list << certificate;
		config.setCaCertificates( list );
	}
	else
		config.setPeerVerifyMode( QSslSocket::VerifyNone );

	QSslKey key( CaReportPrivateKey,
					QSsl::Rsa,
					QSsl::Pem,
					QSsl::PrivateKey,
					KeyChecker );
	// TODO : Need research do we really should try to set key.
	if ( ! key.isNull() )
		config.setPrivateKey( key );
	else
		WRITE_TRACE( DBG_DEBUG, "Error : unable to set private key to ssl config obj, key is null" );

	request.setSslConfiguration( config);
}

void getDataFromFile( const QString& strFilePath,  QByteArray & data )
{
	QFile packedFile( strFilePath );
	if ( packedFile.open( QIODevice::ReadOnly ) )
		data = packedFile.readAll();
}

QString GetProblemReportPath(const QString & strBaseDir)
{
	if (!QFileInfo(strBaseDir).exists())
		return "";

	// try to create Problem reports dir!
	QString reportName = strBaseDir + QString("/") + VM_PROBLEM_REPORT_DIR_NAME;

	if ( !QFileInfo( reportName ).exists() )
	{
		if( !QDir().mkpath( reportName ) )
			return "";
	}

	//0x222
	QFile::Permissions writePerm = QFile::Permissions(
				QFile::WriteOther | QFile::WriteUser | QFile::WriteGroup
				| QFile::ReadOther | QFile::ReadUser | QFile::ReadGroup
				| QFile::ExeOther | QFile::ExeUser | QFile::ExeGroup
			);
	QFile::Permissions dirPerm =  QFile::permissions( reportName );
	if ( (dirPerm & writePerm) != writePerm )
	{
	    if ( !QFile::setPermissions( reportName, dirPerm | writePerm ) )
		    WRITE_TRACE( DBG_FATAL, "Failed  to set write permissions to '%s'", QSTR2UTF8( reportName ));
	}

	return reportName;
 }

// forms path to save guest screenshot
QString GetPathForSelfSave(const QString & strBaseDir)
{
	QString reportName = GetProblemReportPath(strBaseDir);

	if (reportName.isEmpty())
	    return "";

	QDateTime qDT = QDateTime::currentDateTime();

	reportName = reportName += QString("/%1").arg(VM_PROBLEM_REPORT_PREFIX);
	reportName += QString("-%1.%2")
		.arg(qDT.toString("yyyy.MM.dd-hh.mm.ss.zzz")).arg(VM_PROBLEM_REPORT_SUFFIX);

	return reportName;
}

void CorrectNonPrintableSymbols( QString & strBaseDir/*IN OUT */ )
{
	for( int i = 0 ; i < strBaseDir.size(); i++ )
	{
		if ( !strBaseDir[i].isPrint() && strBaseDir[i] != '\n' && strBaseDir[i] != '\r')
			strBaseDir[i] = '?';
	}
}

QString getHostInfo();
void addSystemLogWithSuffix(CProblemReport & cReport,
                            const QString & strLogFilePath,
                            const QString & strNameSuffix,
                            quint32 nBackupsCount = 0);
QString generateHostInfo();
void addCrashDumps(CProblemReport& cReport);

/**
 * add to problem report all client size information.
 */
void addClientInformation( CProblemReport & cReport, PRL_APPLICATION_MODE nAppMode )
{
    Q_UNUSED(nAppMode);

	/**
     * fill client info
     */
	ClientInfo * pInfo = new ClientInfo();
	QSettings appSettings(QSettings().organizationName(), ParallelsDirs::getAppGuiName(nAppMode));
	QStringList lstKeys = appSettings.allKeys();
	QString strResult;
	QString strValue;
	foreach( QString strKey, lstKeys )
	{
		strValue = appSettings.value( strKey ).toString();
		if( strKey.isEmpty() || strValue.isEmpty() )
			continue;
		strResult += strKey + " -- ";
		strResult += strValue;
		strResult += "\n";
	}
	CorrectNonPrintableSymbols( strResult );
	pInfo->setClientSettings( strResult );
    
	addSystemLogWithSuffix( cReport, ParallelsDirs::getClientLogPath(), "-client" );
    
	if ( PAM_SERVER == nAppMode )
	{
		pInfo->setHostInfo( getHostInfo() );
		pInfo->setEnvironment( QProcess::systemEnvironment().join("\n") );
        
		pInfo->setInstalledSoftware( CInstalledSoftwareCollector::getInstalledSoftwareListSync() );
	}
    
	cReport.setClientInfo( pInfo );
}

QString GetLastPanicLogPath( const QString &sTargetPath = PRL_DEFAULT_MAC_OS_CRASH_REPORTS_PATH );

void addCommonLogFiles( CProblemReport & cReport )
{
	QStringList lstLogPaths;
	/**
     * First we try to read the overall system log file
     */
    
	QString strParallelsLogPath = ParallelsDirs::getSystemLogPath();
    
	addSystemLogWithSuffix( cReport, strParallelsLogPath, "-system", 2 );
    
#ifndef _WIN_
    
#	ifdef _LIN_
	strParallelsLogPath = "/var/log/messages";
#	else
	strParallelsLogPath = "/var/log/system.log";
#	endif

	int iSystemLogFilesCount = 2; // by default
	cReport.appendTemplateSystemLog( strParallelsLogPath,
                                    QFileInfo(strParallelsLogPath).fileName(), iSystemLogFilesCount );
    
#endif//ifndef _WIN_
    
	lstLogPaths += ParallelsDirs::getInstallationLogFilePaths();


	foreach( QString strlogPath, lstLogPaths )
	{
		if ( !strlogPath.isEmpty() )
			cReport.appendSystemLog( strlogPath, QFileInfo(strlogPath).fileName() );
	}
}

QString generateComputerModel()
{
#if defined(_WIN_)
	static WmiWrap ww; /* caching values we get from WMI */
    
	QString vendor = QString::fromUtf16(ww.GetComputerManufacturer().c_str());
	QString model = QString::fromUtf16(ww.GetComputerModel().c_str());
    
	if (vendor == "" && model == "")
		return "Unknown"; /* if no info was detected */
    
	return QString("%1 - %2").arg(vendor).arg(model);
#endif
    
	return "";
}

QString generateHostInfo()
{
    QString qsHostInfo =
    "<ParallelsHostInfo>"
    "\t<OsVersion>"
    "\t\t<StringPresentation>%1</StringPresentation>"
    "\t</OsVersion>"
    "</ParallelsHostInfo>";

    QString qsOsVersion;

    qsHostInfo = qsHostInfo.arg(qsOsVersion);
    return qsHostInfo;
}

void addCrashDumps(CProblemReport& cReport)
{
    Q_UNUSED(cReport);
}

}
