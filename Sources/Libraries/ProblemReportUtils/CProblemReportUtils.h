/*
 * CProblemReportUtils.h: Helper class for getting default parallels
 * urls locations.
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


#ifndef __PARALLELS_URLS_H__
#define __PARALLELS_URLS_H__

#include "CProblemReportUtils_common.h"


namespace CProblemReportUtils
{
	//
	/**
	* post report to server
	* @param CProblemReport & pReport - this function is not change object content
	*/
	int postReport( CProblemReport & pReport/*IN*/, QHttp & http/*IN*/);
	int postReport( QHttp & http/*IN*/, const QByteArray & data/*IN*/,
			       PRL_PROBLEM_REPORT_TYPE reportType = PRT_REPORT_TYPE_UNDEFINED );
	int postReport( const QByteArray & param_msg_data/*IN*/, QHttp & http/*IN*/,
			       const QString & productName = QString(),
			       PRL_PROBLEM_REPORT_TYPE reportType = PRT_REPORT_TYPE_UNDEFINED );

	QNetworkReply * postReport( QNetworkAccessManager & /*IN*/, QByteArray & /*IN*/ );
	QNetworkReply * postReport( const QString& /*IN*/, QNetworkAccessManager &/*IN*/);

	QHttpRequestHeader createMultipartRequestHeader( QByteArray & param_msg_data/*IN-OUT*/);
	int postReport( const QString& strFilePath /*IN*/, QHttp & http/*IN*/,
			       PRL_PROBLEM_REPORT_TYPE reportType = PRT_REPORT_TYPE_UNDEFINED );

	// return list of crash-files templates to use in QDir::setNameFilters()
	QStringList GetCrashDumpsTemplates(const QString& qsPatternSuffix = "*.*",
											  bool bAddLowMemoryDumps = true);

	// forms path to save guest screenshot
	QString GetPathForScreenshotSave(const QString & strBaseDir);

	// forms path to save host screenshot
	QString GetPathForHostScreenshot(const QString & strBaseDir);

	QString GetUserSideBaseDir();

	QString extractIsoVersion(const QString & sInput);

	void formDataToHttpPost(
		const QByteArray & param_msg_data/*IN*/, QByteArray & param_total /*OUT*/,
		const QString & productName = QString() );

	// return string that contains host installed software information
	QString GetInstalledSoftwareList();

	// remove guest screen shot from report
	void removeGuestScreenshot( CPackedProblemReport & cReport );

	// remove host screen shot from report
	void removeHostScreenshot( CPackedProblemReport & cReport );

	// get guest screen shot pointer from report
	CRepScreenShot * getGuestScreenshot( CPackedProblemReport & cReport );

	// get host screen shot pointer from report
	CRepScreenShot * getHostScreenshot( CPackedProblemReport & cReport );

	/**
	* fill problem report object with data in desktop mode if dispatcher not connected.
	*/
	void fillDesktopProblemReportOnDisconnectServer(CProblemReport & cReport,
														bool add_crash_info = true );

	// fill OS and product installation history
	void fillInstallationsData( CRepAutoStatisticInfo* pAutoStatInfo );

	/**
	* fill list with paths to crush logs.
	* @param const QStringList& listOfCrashDirs list of dirs to search
	* @param int iDaysOld - days old from create if parameter == -1 - collect all logs
	*/
	QStringList getListOfParallelsCrashedLogs( const QStringList& listOfCrashDirs, int iDaysOld = 3 );

	/**
	 * Adds specified crash dump to problem report
	 * @param problem report object reference
	 * @param path to the crash dump file
	 */
	void fillCrashDump( CProblemReport & cReport, const QString & DumpPath );

	/**
	 * Adds client keyboard and mouse profiles to problem report
	 * @param problem report object reference
	 */
	void addKeyboardMouseProfiles( CProblemReport & cReport );

	// load all app switcher packages info
	QString loadAppSwitcherPackagesInfo();

	// load all learn video time data
	QString loadLearnVideoTimes();

	/**
	 * Returns path to the latest system panic log
	 * @param target path to analyse
	 */
	QString GetLastPanicLogPath( const QString &sTargetPath = PRL_DEFAULT_MAC_OS_CRASH_REPORTS_PATH );

	// execute Shell command using QProcess
	bool executeShellCommandSync( const QString & strCommand, QString & strOut );

	QString getGuiCepQSettingsOrganizationName();

	// get app version from line macosx crash report
	bool getAppVersionFromLineOfMacCrashDump( const QString& sLineOfCrashDump
		, QString& outBuildNum, QString& outRevision  );

	// get stub major version from line macosx crash report
	bool getStubVersionFromLineOfMacCrashDump( const QString& sLineOfCrashDump
		, QString& outMajorVer );

	// get app version from line macosx panic report
	bool getDrvVersionFromLineOfMacPanicDump( const QString& sLineOfPanicDump
		, QString& outBuildNum, QString& outRevision  );

	bool isJettisonedMobileApp_fromLineOfLowMemoryDump(
		const QString& mobileAppName, const QString& line );

	/**
	 * Collects all crash reports paths for analyzing with deep mobile device
	 * crash reports dirs structure. Please note call of this method makes sense
	 * just under Mac OS platform.
	 * @param list of users home directories to process
	 * @param sign whether common system paths should be added to the list
	 * @return list of directories to process for crash reports
	 */
	QSet<QString> getCompleteCrashReportsPaths( const QStringList& lstUsersHomeDirs, bool bAddSystemPaths );

	/**
	 * Eliminates extra slashes from directory path
	 */
	QString normalizePath( const QString& _sPath );

	/**
	 * Eliminates extra slashes from directories paths
	 */
	void normalizePaths( QSet<QString>& lstDirsPaths );

	/**
	 * Add system log to report with name suffix. if nBackupsCount > 0, then
	 * adds system log template.
	 */
	void addSystemLogWithSuffix( CProblemReport & cReport,
										const QString & strLogFilePath,
										const QString & strNameSuffix,
										quint32 nBackupsCount = 0 );
}

#endif //__PARALLELS_URLS_H__
