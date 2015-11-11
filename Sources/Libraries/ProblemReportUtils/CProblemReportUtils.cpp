/*
 * CProblemReportUtils.cpp: Helper class for getting default
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


#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QStringList>
#include <QMap>
#include <QDir>
#include <QThread>
#include <QSet>
#include <QDateTime>

#include "Build/Current.ver"
#include "Build/Current-locale.ver"
#include "Interfaces/ParallelsQt.h"
#include "Interfaces/ParallelsNamespace.h"
#include "Libraries/Std/PrlAssert.h"
#include "CProblemReportUtils.h"

#include <QHttp>
#include <QtNetwork/QHttpRequestHeader>

#include "CInstalledSoftwareCollector.h"
#include "XmlModel/DispConfig/CDispatcherConfig.h"
#include "XmlModel/Updater/CUpdaterConfig.h"
#include "XmlModel/VmConfig/CVmConfiguration.h"
#include "XmlModel/HostHardwareInfo/CHostHardwareInfo.h"
#include "XmlModel/KeyboardMouse/ParallelsKeyboardMouse.h"
#include "Libraries/PrlCommonUtilsBase/StringUtils.h"
#include "CPackedProblemReport.h"
#include "Libraries/PrlCommonUtilsBase/CSimpleFileHelper.h"
#include "Libraries/HostUtils/HostUtils.h"
#include "Libraries/PrlCommonUtilsBase/ParallelsDirs.h"

#ifdef _WIN_
#include "Libraries/WmiWrap/WmiWrap.h"
#endif

using namespace Parallels;

namespace BinDumpPatterns {

const char* g_prl_vm_starter = "prl_vm_starter";
const char* g_prl_vm_app = "prl_vm_app";
const char* g_prl_disp_service = "prl_disp_service";
const char* g_prl_pm_service = "prl_pm_service";
const char* g_prl_client_app = "prl_client_app";
const char* g_prl_event_tap = "prl_event_tap";
const char* g_prl_naptd = "prl_naptd";
const char* g_prl_net_start = "prl_net_start";
const char* g_prlctl = "prlctl";
const char* g_prlsrvctl = "prlsrvctl";
const char* g_parallels_im = "ParallelsIM";
const char* g_prl_disk_tool = "prl_disk_tool";
const char* g_prl_mkiso = "prl_mkiso";
const char* g_prl_deactivation_id = "prl_deactivation_id";
const char* g_prl_updater_app = "prl_updater_app";
const char* g_prl_updater_ctl = "prl_updater_ctl";
const char* g_pax_updater_ctl = "pax_updater_ctl";
const char* g_prl_convert = "prl_convert";
const char* g_prl_perf_ctl = "prl_perf_ctl";
const char* g_prl_pcsc_proxy = "prl_pcsc_proxy";
const char* g_prlauth = "prlauth";
const char* g_prl_sharedapp_link = "prl_sharedapp_link";
const char* g_prl_applescript_runner = "prl_applescript_runner";
const char* g_prl_shappgroup_bridge = "prl_shappgroup_bridge";
const char* g_prl_deskctl_agent = "prl_deskctl_agent";
const char* g_prl_deskctl_wizard = "prl_deskctl_wizard";
const char* g_prl_SharedAppStub = "WinAppHelper";
const char* g_parallels_mounter = "Parallels Mounter";
const char* g_lsregappbundle = "lsregappbundle";
const char* g_pefs_util = "PEFSUtil";
const char* g_parallels_explorer = "Parallels Explorer";
const char* g_low_memory_file = "LowMemory";
const char* g_prl_procdump = "prl_procdump";
}

using namespace BinDumpPatterns;

namespace CProblemReportUtils
{

static void removeScreenshotByType( CPackedProblemReport & cReport,
										const QString & strNamePrefix );
static CRepScreenShot * getScreenshotByType( CPackedProblemReport & cReport,
										const QString & strNamePrefix );
QString getHostInfo();


QStringList GetCrashDumpsTemplates(const QString& qsPatternSuffix, bool bAddLowMemoryDumps)
{
	QStringList filters;

		filters <<
			g_prl_disp_service
			<< g_prl_naptd
			<< g_prl_net_start
			<< g_prl_vm_starter
			<< g_prl_vm_app
			<< g_prl_client_app
			<< g_prl_disk_tool
			<< g_prl_mkiso
			<< g_prl_updater_app
			<< g_prl_convert
			<< g_prl_perf_ctl
			<< g_prl_pcsc_proxy
			<< g_prlauth
			<< g_prl_SharedAppStub
			<< g_prlctl
			<< g_prlsrvctl
			<< g_prl_event_tap
			<< g_prl_deactivation_id
			<< g_prl_shappgroup_bridge
			<< g_prl_updater_ctl
			;

	for(int i = 0; i < filters.size(); i++)
	{
		filters[i] += qsPatternSuffix;
	}

	if (bAddLowMemoryDumps)
	{
		filters << QString("%1%2").arg(g_low_memory_file, "*.log");
	}

	return filters;
}

int postReport ( CProblemReport & pReport, QHttp & http )
{
	WRITE_TRACE( DBG_DEBUG, "user = %s", pReport.getContactInfo()->getName().toUtf8().data() );
	WRITE_TRACE( DBG_DEBUG, "user e-mail = %s", pReport.getContactInfo()->getEMail().toUtf8().data() );

	QString strReport = pReport.toString();
	CorrectNonPrintableSymbols( strReport );
	QByteArray param_msg_data = strReport.toUtf8();

	return postReport( param_msg_data, http, pReport.getProductName(), pReport.getReportType() );
}

QHttpRequestHeader createMultipartRequestHeader( QByteArray & content/*IN - OUT*/)
{
	fillContentWithRequestData( content );

	QHttpRequestHeader header( "POST", "/post" );
	header.setContentType( "multipart/form-data; boundary=" + PRL_PROBLEM_REPORT_REQUEST_BOUNDARY );
	header.setContentLength( content.length() );
	return header;
}

int postReport( const QString& strFilePath /*IN*/, QHttp &http/*IN*/,
			PRL_PROBLEM_REPORT_TYPE reportType/*IN*/ )
{
	QByteArray data;
	getDataFromFile( strFilePath, data );

	return postReport( http, data, reportType );
}

int postReport( QHttp & http/*IN*/, const QByteArray & data/*IN*/,
			PRL_PROBLEM_REPORT_TYPE reportType/*IN*/ )
{
	bool bSupportSsl = false;

#ifndef QT_NO_OPENSSL
	if ( QSslSocket::supportsSsl() )
	{
		WRITE_TRACE(DBG_DEBUG, "this system supports ssl");
		bSupportSsl = true;
	}
#endif
	QUrl url = getCrashReportServerUrlSync( bSupportSsl, false, reportType );

	http.setHost( url.host(),
		bSupportSsl ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp,
		url.port( 0 )  );

	WRITE_TRACE( DBG_FATAL, "url = %s", QSTR2UTF8( url.toString() ) );
	// create http header
	QByteArray content = data;
	QHttpRequestHeader header = createMultipartRequestHeader( content );
	header.setValue( "host",url.host() );

	WRITE_TRACE( DBG_FATAL, "header = %s", QSTR2UTF8( header.toString() ) );
	// Sending http request to the server for analysis
	int iRequestId = http.request( header , content );
	return iRequestId;
}

int postReport( const QByteArray & param_msg_data, QHttp & http, const QString & productName,
				     PRL_PROBLEM_REPORT_TYPE reportType )
{
	bool bSupportSsl = false;

#ifndef QT_NO_OPENSSL
	if ( QSslSocket::supportsSsl() )
	{
		WRITE_TRACE(DBG_DEBUG, "this system supports ssl");
		bSupportSsl = true;
	}
#endif

	QByteArray param_total;
	formDataToHttpPost( param_msg_data, param_total, productName );

	QUrl url = getCrashReportServerUrlSync( bSupportSsl, true, reportType );

	quint16 port = url.port( 0 ); // local variable to easy debug sending through proxy
	http.setHost( url.host(),
		bSupportSsl ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp,
		port  );


	// Sending http request to the server for analysis
	int iRequestId = http.post( url.path(), param_total );
	return iRequestId;
}

// forms path to save guest screenshot
QString GetPathForScreenshotSave(const QString & strBaseDir)
{
	QString reportName = GetProblemReportPath(strBaseDir);

	if (reportName.isEmpty())
	    return "";

	QDateTime qDT = QDateTime::currentDateTime();

	// Generate main part on name
	reportName += QString("/%1").arg(VM_PROBLEM_REPORT_SCREEN_PREFIX);
	reportName += QString("-%1.%2")
		.arg(qDT.toString("yyyy.MM.dd-hh.mm.ss.zzz")).arg(VM_PROBLEM_REPORT_SCREEN_SUFFIX);

	return reportName;
}

// forms path to save host screenshot
QString GetPathForHostScreenshot(const QString & strBaseDir)
{
	QString reportName = GetProblemReportPath(strBaseDir);

	if (reportName.isEmpty())
		return "";

	QDateTime qDT = QDateTime::currentDateTime();

	reportName = reportName += QString("/%1").arg(VM_PROBLEM_REPORT_SCREEN_HOST_PREFIX);
	reportName += QString("-%1.%2")
		.arg(qDT.toString("yyyy.MM.dd-hh.mm.ss.zzz")).arg(VM_PROBLEM_REPORT_SCREEN_SUFFIX);

	return reportName;
}

// forms path to save host screenshot
QString GetUserSideBaseDir()
{
	QString strBaseDir;

	strBaseDir = ParallelsDirs::getCallerUserPreferencesDir();

	if (   ! QFileInfo( strBaseDir ).exists()
		&& ! QDir().mkpath( strBaseDir ) )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot create user base directory '%s' !",
					QSTR2UTF8(strBaseDir));
	}

	return strBaseDir;
}

//Input format: Product name x.x.x (y.y.y)
//Example: Parallels Cloud Server 6.0.6 (1992)
QString extractIsoVersion(const QString & sInput)
{
	QRegExp x("(\\d+(\\.\\d+)+).+\\((\\d+)\\)");
	x.indexIn(sInput);

	return x.cap(1) + "-" + x.cap(3);
}

void formDataToHttpPost(
	const QByteArray & param_msg_data, QByteArray & param_total, const QString & productName )
{
	// Adding product name parameter
	{
		QString data = productName;
		if (data.isEmpty())
			data = QString( VER_SHORTPRODUCTNAME_STR );
		QByteArray param_product_data = data.toUtf8();
		QByteArray param_product_header =
			QString( "problem_report_product(%1,txt)=" ).arg( param_product_data.size() ).toUtf8();
		QByteArray param_product = param_product_header + param_product_data;
		param_total += param_product;
	}

	// Adding software version parameter
	{
		QString data = QString( VER_FULL_BUILD_NUMBER_STR );
		QByteArray param_version_data = data.toUtf8();
		QByteArray param_version_header =
			QString( "problem_report_version(%1,txt)=" ).arg( param_version_data.size() ).toUtf8();
		QByteArray param_version = param_version_header + param_version_data;
		param_total += param_version;
	}

	// Adding user report text parameter
	{
		QByteArray param_msg_header =
			QString( "problem_report_xml(%1,txt)=" ).arg( param_msg_data.size() ).toUtf8();
		QByteArray param_msg = param_msg_header + param_msg_data;
		param_total += param_msg;
	}
}

bool executeShellCommandSync( const QString & strCommand, QString & strOut )
{
	QProcess proc;

	if (HostUtils::RunCmdLineUtility(strCommand, strOut, 3 * 60 * 1000, &proc, 0)) // 3 minutes
		return !strOut.isEmpty();

	if (proc.exitCode() != 0)
		return false;

	if (!proc.waitForFinished(1))
	{
		WRITE_TRACE(DBG_FATAL, "Command '%s' not finished in time, will"
			" be terminated", QSTR2UTF8(strCommand) );
		proc.terminate();
		if (!proc.waitForFinished(60 * 1000))
		{
			WRITE_TRACE(DBG_FATAL, "Command '%s' not finished in time, will"
				" be killed", QSTR2UTF8(strCommand) );
			proc.kill();
			proc.waitForFinished(-1); // infinite
		}
	}

	return false;
}

// remove screen shot from report by type of prefix
static void removeScreenshotByType( CPackedProblemReport & cReport,
											const QString & strNamePrefix )
{
	CRepUserDefinedData * pData = cReport.getUserDefinedData();
	if ( !pData )
		return;

	CRepScreenShots * pScrs = pData->getScreenShots();
	if( !pScrs )
		return;

	CRepScreenShot * pScr = NULL;
	foreach( pScr, pScrs->m_lstScreenShot )
	{
		if ( pScr && pScr->getName().contains( strNamePrefix ) )
		{
			pScrs->m_lstScreenShot.removeAll( pScr );
			delete pScr;
			break;
		}
	}
}

// remove guest screen shot from report
void removeGuestScreenshot( CPackedProblemReport & cReport )
{
	removeScreenshotByType( cReport, VM_PROBLEM_REPORT_SCREEN_PREFIX );
}

// remove host screen shot from report
void removeHostScreenshot( CPackedProblemReport & cReport )
{
	removeScreenshotByType( cReport, VM_PROBLEM_REPORT_SCREEN_HOST_PREFIX );
}


// get screen shot pointer from report by type of prefix
static CRepScreenShot * getScreenshotByType( CPackedProblemReport & cReport,
										 const QString & strNamePrefix )
{
	CRepUserDefinedData * pData = cReport.getUserDefinedData();
	if ( !pData )
		return NULL;

	CRepScreenShots * pScrs = pData->getScreenShots();
	if( !pScrs )
		return NULL;

	CRepScreenShot * pScr = NULL;
	foreach( pScr, pScrs->m_lstScreenShot )
	{
		if ( pScr && pScr->getName().contains( strNamePrefix ) )
			return pScr;
	}
	return NULL;
}

// get guest screen shot pointer from report
CRepScreenShot * getGuestScreenshot( CPackedProblemReport & cReport )
{
	return getScreenshotByType( cReport, VM_PROBLEM_REPORT_SCREEN_PREFIX );
}

// get host screen shot pointer from report
CRepScreenShot * getHostScreenshot( CPackedProblemReport & cReport )
{
	return getScreenshotByType( cReport, VM_PROBLEM_REPORT_SCREEN_HOST_PREFIX );
}

void fillDesktopProblemReportOnDisconnectServer( CProblemReport & cReport,
															bool add_crash_info )
{
	//Fill host info information
	cReport.setHostInfo( getHostInfo() );
	cReport.setMoreHostInfo( HostUtils::GetMoreHostInfo() );
	cReport.setAllProcesses( HostUtils::GetAllProcesses(true) );
	cReport.setLoadedDrivers( HostUtils::GetLoadedDrivers() );
	cReport.setMountInfo( HostUtils::GetMountInfo() );

	// try load DISPATCHER_CONFIGURATION_DESKTOP_XML_FILE to get information
	CDispatcherConfig cDispConfig;
	QString strPrefPath = ParallelsDirs::getDispatcherConfigFilePath();
	if( cDispConfig.loadFromFile( strPrefPath ) == PRL_ERR_SUCCESS )
	{
		// set dispatcher config data
		cReport.setAppConfig( cDispConfig.getDispatcherSettings()->getCommonPreferences()->toString() );
	}



	// try to load updater config file to xml

	QString strDispConfigDir = ParallelsDirs::getDispatcherConfigDir();
	QFile _vm_config_file(strDispConfigDir+"/.parallels_updater.xml");
	CUpdaterConfig UpdaterConfig(&_vm_config_file);
	if (UpdaterConfig.m_uiRcInit == PRL_ERR_SUCCESS)
	{
		cReport.setVmUpdaterInfo( UpdaterConfig.toString() );
	}

	/**
	* add common log files
	*/
	addCommonLogFiles( cReport );

	if ( add_crash_info )
	{
		QStringList crashDirList;

		crashDirList << ParallelsDirs::getCrashDumpsPath();
		QStringList strLogs = getListOfParallelsCrashedLogs( crashDirList );

		for (int i = 0 ; i < strLogs.size() ; i++)
		{
			fillCrashDump( cReport, strLogs[i] );
		}
	}

	// add computer model
	cReport.setComputerModel( generateComputerModel() );

	// add OS and product installation history
	CRepAutoStatisticInfo* pAutoStatInfo = new CRepAutoStatisticInfo();
	fillInstallationsData( pAutoStatInfo );
	cReport.setAutoStatisticInfo( pAutoStatInfo );
}

void fillInstallationsData( CRepAutoStatisticInfo* pAutoStatInfo )
{
	if( !pAutoStatInfo )
		return;

	CRepInstallations * pInstallations = new CRepInstallations();
	bool bHasInstallations = false;

	// try to get PD installations dates
	if( QFile::exists( PRL_DIRS_PD_INSTALLATION_STATS_FOLDER ) )
	{
		QDir	cDir(PRL_DIRS_PD_INSTALLATION_STATS_FOLDER);
		cDir.setFilter(QDir::Files| QDir::NoDotAndDotDot | QDir::Hidden);
		cDir.setSorting(QDir::Time);

		QFileInfoList cFileList = cDir.entryInfoList();

		if( !cFileList.isEmpty() )
		{
			bHasInstallations = true;
			pInstallations->setPDInstallationHistoryes( new CRepPDInstallations() );
		}

		for( int i = 0 ; i < cFileList.size() ; i++ )
		{
			QFile file( cFileList[i].filePath() );
			if( !file.open( QIODevice::ReadOnly ) )
				continue;

			CRepPDInstallationData * pData = new CRepPDInstallationData();
			pData->setInstalledVersionDate( UTF8_2QSTR( file.readAll() ) );
			pData->setInstalledVersionName( cFileList[i].fileName() );

			pInstallations->getPDInstallationHistoryes()->m_lstPDInstallations += pData;

		}//for
	}

	if( !bHasInstallations )
		delete pInstallations;
	else
		pAutoStatInfo->setInstallationsData( pInstallations );
}

QStringList getListOfParallelsCrashedLogs( const QStringList&, int )
{
	QStringList lststrFiles;
	return lststrFiles;
}

void fillCrashDump( CProblemReport & cReport, const QString & DumpPath )
{
	if ( ! DumpPath.isEmpty() )
	{
		QFile file( DumpPath );
		QFileInfo fileInfo( file );
		bool res = file.open( QFile::ReadOnly );
		if ( ! res )
		{
			WRITE_TRACE(DBG_FATAL, "Error: can't open crash dump '%s'",
				DumpPath.toLocal8Bit().constData());
		}
		else if ( file.size() )//Account just non empty files
		{
			// Get dump pid
			int pid = 0;
			QRegExp rx("\\.(\\d{1,})\\.(lin|win|mac)\\.(dmp|lcore)");
			if ( rx.indexIn(DumpPath) != -1 )
			{
				QString pidStr = rx.cap(1);
				bool ok = false;
				pid = pidStr.toInt(&ok);
			}

			CRepCrashDump* repCrashDump = new CRepCrashDump;

			repCrashDump->setPath( DumpPath );
			repCrashDump->setApplicationPid( QString("%1").arg(pid) );
			repCrashDump->setCreationDateTime( fileInfo.lastModified().toString(XML_DATETIME_FORMAT_LONG) );

			cReport.appendCrashDump( repCrashDump );
		}
	}
}

QString getHostInfo()
{
	return ( QString() );
}

void addKeyboardMouseProfiles( CProblemReport & cReport )
{
	QString path = ParallelsDirs::getCallerUserPreferencesDir() + "/";
	QDir dir( path );
	QStringList filters;
	filters << QString("*.") + PROFILE_EXTENSION;
	QStringList profiles = dir.entryList(filters, QDir::Files);

	QString qsProfilesData;
	KeyboardMouseProfiles * pProfiles = new KeyboardMouseProfiles;

	foreach( QString kbdFile, profiles )
	{
		QFile file(path + kbdFile);
		ParallelsKeyboardMouse * profile = new ParallelsKeyboardMouse;
		if ( PRL_FAILED( profile->loadFromFile(path + kbdFile) ) )
			continue;

		pProfiles->m_lstKeyboardMouseProfile += profile;
	}

	cReport.setKeyboardMouseProfiles( pProfiles );
}

QString loadAppSwitcherPackagesInfo()
{
	QString productName = QString( PRL_PRODUCT_NAME_DESKTOP_WL ) + PRL_STATISTICS_KEY_POSTFIX;
	QString vendorName = PRL_VENDOR;
	QString appName = ParallelsDirs::getAppSwitcherAppName();

	QSettings settings( vendorName, productName );
	settings.beginGroup( appName + QString("/") + PRL_KEY_CEP_STATISTICS_APP_SWITCHER_PACKAGES );

	QStringList lstKeys = settings.allKeys();
	QString strResult;
	QString strValue;
	foreach( QString strKey, lstKeys )
	{
		strValue = settings.value( strKey ).toString();
		if( strKey.isEmpty() || strValue.isEmpty() )
			continue;
		strResult += strKey + ":";
		strResult += strValue;
		strResult += "\n";
	}

	settings.endGroup();
	return strResult;
}

QString loadLearnVideoTimes()
{
// we are collecting this information only for stw7
#ifndef _WIN_
	return QString();
#endif //_WIN_

	QSettings settings( PRL_VENDOR, ParallelsDirs::getLearnVideoAppName() );
	QStringList lstKeys = settings.allKeys();
	QString strResult;
	QString strValue;
	foreach( QString strKey, lstKeys )
	{
		strValue = settings.value( strKey ).toString();
		if( strKey.isEmpty() || strValue.isEmpty() )
			continue;
		strResult += strKey + " ";
		strResult += strValue;
		strResult += "\n";
	}

	return strResult;
}

/**
 * Simple helper - searches for newest panic log at specified folder
 * @param target folder path
 */
QFileInfo GetLastPanicLogFromSubfolder( const QString &sTargetFolder )
{
	QDir _target_folder( sTargetFolder );
	QFileInfoList _entries =
		_target_folder.entryInfoList( QStringList() << "*.panic", QDir::Files | QDir::NoSymLinks | QDir::Hidden, QDir::Time );
	if ( _entries.isEmpty() )
		return QFileInfo();
	else
		return _entries.front();
}

QString GetLastPanicLogPath( const QString &sTargetPath )
{
	QMap<QDateTime, QString> _panic_logs;
	//Process first case: Mac OS 10.6.* 32-bit kernel: all panics store to the single
	//file /Library/Logs/panic.log
	QFileInfo _panic_log( sTargetPath + "/panic.log" );
	if ( _panic_log.exists() )
		_panic_logs[ _panic_log.lastModified() ] = _panic_log.absoluteFilePath();
	//Process second case: Mac OS 10.6.* 64-bit kernel: all panics store to special
	//subfolder DiagnosticReports and have extension *.panic
	_panic_log = GetLastPanicLogFromSubfolder( sTargetPath + PRL_MAC_OS_10_6_PANICS_SUBFOLDER );
	if ( _panic_log.exists() )
		_panic_logs[ _panic_log.lastModified() ] = _panic_log.absoluteFilePath();
	//Process third case: Mac OS 10.5.*: all panics store to special subfolder PanicReporter
	//and have extension *.panic
	_panic_log = GetLastPanicLogFromSubfolder( sTargetPath + PRL_MAC_OS_10_5_PANICS_SUBFOLDER );
	if ( _panic_log.exists() )
		_panic_logs[ _panic_log.lastModified() ] = _panic_log.absoluteFilePath();

	if ( _panic_logs.isEmpty() )
		return QString();

	//Return last entry - it should be newest panic log actually
	return _panic_logs.values().back();
}

QString getGuiCepQSettingsOrganizationName()
{
	PRL_ASSERT( !qApp->organizationName().compare( PRL_VENDOR ) );
	return QSettings().organizationName();
}

bool getAppVersionFromLineOfMacCrashDump(
	const QString& line, QString& outBuildNum, QString& outRevision )
{
	QRegExp pd9RegExp("^Version:\\s+.*\\d+\\.\\d+\\s+.*\\((\\d+)\\.(\\d+)\\)"); //Version: 9.0 (22444.903761)
	pd9RegExp.setMinimal(true);

	QRegExp pd10RegExp("^Version:\\s+.*\\d+\\.\\d+\\.\\d+\\s+.*\\((\\d+)\\)"); //Version: 10.0.0 (23456)
	pd10RegExp.setMinimal(true);

	if( -1 != pd9RegExp.indexIn(line) )
	{
		outBuildNum = pd9RegExp.cap(1);
		outRevision = pd9RegExp.cap(2);
		return true;
	}

	if ( -1 != pd10RegExp.indexIn(line) )
	{
		outBuildNum = pd10RegExp.cap(1);
		outRevision = QString();
		return true;
	}

	return false;
}

bool getStubVersionFromLineOfMacCrashDump(
	const QString& line, QString& outMajorVer )
{
	//Version: Windows 7 x64 Ee (9.0.22495.913787)
	QRegExp pd9VersionRegExp("^Version:\\s+.*\\((\\d+)\\.\\d+\\.\\d+\\.\\d+\\)");
	pd9VersionRegExp.setMinimal(true);

	//Version: Windows 7 x64 Ee (10.25857.1000746)
	QRegExp pd10VersionRegExp("^Version:\\s+.*\\((\\d+)\\.\\d+\\.\\d+\\)");
	pd10VersionRegExp.setMinimal(true);

	if( -1 != pd9VersionRegExp.indexIn(line) )
	{
		outMajorVer = pd9VersionRegExp.cap(1);
		return true;
	}

	if( -1 != pd10VersionRegExp.indexIn(line) )
	{
		outMajorVer = pd10VersionRegExp.cap(1);
		return true;
	}

	return false;
}

bool getDrvVersionFromLineOfMacPanicDump(
	const QString& line, QString& outBuildNum, QString& outRevision  )
{
	// Example:
	// "com.parallels.kext.hypervisor	6.0 11828.615184"
	QRegExp pd9("^com\\.parallels\\.kext\\..+\\s+\\d+\\.\\d+\\s+(\\d+\\.\\d+)\\s*$");
	// Do '.*' rule no greedy
	pd9.setMinimal(true);

	// "com.parallels.kext.hypervisor	10.0.0 23456"
	QRegExp pd10("^com\\.parallels\\.kext\\..+\\s+\\d+\\.\\d+\\.\\d+\\s+(\\d+)\\s*$");
	// Do '.*' rule no greedy
	pd10.setMinimal(true);

	if ( -1 != pd9.indexIn(line) )
	{
		QString reportDrvVer  = pd9.cap(1);
		LOG_MESSAGE(DBG_INFO, "Panic appversion '%s.%s' ", QSTR2UTF8(reportDrvVer));

		// reportDrvVer is "13770.660914"
		QStringList lstVers = reportDrvVer.split(".");
		if( lstVers.size() < 2 )
			return false;

		outRevision = lstVers[ (lstVers.size()-1) ]; // get last value.
		outBuildNum = lstVers[ (lstVers.size()-1)-1 ]; // get second value from the end.
		return true;
	}

	if ( -1 != pd10.indexIn(line) )
	{
		QString reportDrvVer  = pd10.cap(1);
		LOG_MESSAGE(DBG_INFO, "Panic appversion '%s.%s' ", QSTR2UTF8(reportDrvVer));

		// reportDrvVer is "23456"
		outRevision = QString();
		outBuildNum = reportDrvVer;
		return true;
	}

	return false;
}

bool isJettisonedMobileApp_fromLineOfLowMemoryDump(
	const QString& mobileAppName,
	const QString& line )
{
	// For example: RemoteClient <da377209df383c908f2466b856748f12> 11858 (jettisoned) (active)
	QRegExp ParallelsMobileExp( QString( "^.*%1\\s+.*jettisoned.*" ).arg(mobileAppName) );
	if( ParallelsMobileExp.indexIn(line)!= -1 )
		return true;

	return false;
}

QString normalizePath( const QString& _sPath )
{
	QString sPath = QFileInfo(_sPath).canonicalFilePath();
	// dropLastSlashes
	while( sPath.endsWith("/") && sPath.size()>1 )
		sPath.chop(1);
	return sPath;
}

void normalizePaths( QSet<QString>& lst )
{
	QSet<QString> out;

	QSet<QString>::iterator it, itEnd;
	for( it = lst.begin(), itEnd = lst.end(); it!=itEnd; it++ )
		out << normalizePath(*it);
	lst = out;
}

void addSystemLogWithSuffix( CProblemReport & cReport,
												  const QString & strLogFilePath,
												  const QString & strNameSuffix,
												  quint32 nBackupsCount )
{
	QFileInfo fi(strLogFilePath);
	QString name(fi.completeBaseName() + strNameSuffix + '.' + fi.suffix());

	if (nBackupsCount)
		cReport.appendTemplateSystemLog( strLogFilePath, name, nBackupsCount );
	else
		cReport.appendSystemLog( strLogFilePath, name );
}

}
