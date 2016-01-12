/*
 * CProblemReportUtils_common.h: Helper class for getting default
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


#ifndef __PARALLELS_URLS_COMMON_H__
#define __PARALLELS_URLS_COMMON_H__

#include <QString>
#include <QSet>
#include "SDK/Include/PrlEnums.h"
#include <prlxmlmodel/ProblemReport/CProblemReport.h>


namespace BinDumpPatterns {

extern const char* g_low_memory_file;
extern const char* g_prl_vm_app;
extern const char* g_prl_SharedAppStub;

}

using namespace BinDumpPatterns;

class QHttp;
class QStringList;
class QByteArray;
class CProblemReport;
class CRepScreenShot;
class CPackedProblemReport;
class QHttpRequestHeader;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QThread;
class QDateTime;
class QUrl;
class CRepAutoStatisticInfo;

#define PRL_DEFAULT_MAC_OS_CRASH_REPORTS_PATH "/Library/Logs/"
#define PRL_MAC_OS_10_5_PANICS_SUBFOLDER "PanicReporter"
#define PRL_MAC_OS_10_6_PANICS_SUBFOLDER "DiagnosticReports"

#define PRL_PROBLEM_REPORT_REQUEST_BOUNDARY QByteArray("MTruJQUcTEHprBtYzmJJrCySniIaLnl")


namespace CProblemReportUtils
{
	enum ReportVersion
	{
		xmlBaseVersion = 0,
		packVersion = 1,
		endVersion,
	};

	// get URL of crash report server
	// Note: when DNS can't resolve address return address with static IP
	// moved from GUI
	// romanp@
	QUrl getCrashReportServerUrlSync( bool bSupportSsl = false, bool bUseOldPath = false, int nReportType = -1 );

	// forms path of "Problem Reports" directory
	QString GetProblemReportPath(const QString & strBaseDir);

	// forms path to save guest screenshot
	QString GetPathForSelfSave(const QString & strBaseDir);

	void CorrectNonPrintableSymbols( QString & strBaseDir/*IN OUT */ );

	QStringList getReportUrlsList( bool bSupportSsl = false,
										  bool bUseOldPath = false,
										  int nReportType = -1 );

	void formNetworkRequestToPost( QNetworkRequest &, QByteArray & );

	void getDataFromFile( const QString& strFilePath,  QByteArray & data );

	void fillContentWithRequestData( QByteArray & content );

	/**
     * add to problem report all client size information.
     */
	void addClientInformation( CProblemReport & cReport, PRL_APPLICATION_MODE nAppMode );

	/**
     * add to problem report log files without component specific.
     */
	void addCommonLogFiles( CProblemReport & cReport );

	QString generateComputerModel();
}

#endif //__PARALLELS_URLS_COMMON_H__
