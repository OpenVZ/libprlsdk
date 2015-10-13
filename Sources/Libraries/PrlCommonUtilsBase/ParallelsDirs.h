/*
 * ParallelsDirs.h: Helper class for getting default parallels
 * configs locations.
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


#ifndef __PARALLELS_DIRS_H__
#define __PARALLELS_DIRS_H__

#include <QString>
#include <QFile>
#include <QPair>
#include <QFlags>
#include <QMutex>
#include "Interfaces/ParallelsNamespace.h"
#include "SDK/Include/PrlEnums.h"
#include "Libraries/PrlCommonUtilsBase/ParallelsDirsDefs.h"

class QDir;

class ParallelsDirs
{
public:

#include "Libraries/PrlCommonUtilsBase/ParallelsDirsBase.h"

	static QString getAppGuiName( PRL_APPLICATION_MODE nAppMode );
	static QString getAppSwitcherAppName();
	static QString getLearnVideoAppName();

	static QString getDispatcherConfigDir();
    static QString getConfigScriptsDir();
	static QString getDispatcherConfigFilePath();
	static QString getDispatcherVmCatalogueFilePath();
	static QString getLicensesFilePath();

	// return  path to NetworkConfigFile for current app mode.
	static QString getNetworkConfigFilePath();

	// return  path to NetworkConfigFile for specified appMode.
	static QString getNetworkConfigFilePath( PRL_APPLICATION_MODE appMode );

	static QString getCallerUserPreferencesDir();

	static QString getCommonDefaultVmCatalogue();
	static QString getUserDefaultVmCatalogue(const ParallelsDirs::UserInfo* pUserInfo);

	static QString getUserHomePath(const ParallelsDirs::UserInfo* pUserInfo);

	// #132602
	static QString getPathToDispatcherTesterConfig();

	// #131807 (Customer experience program support from instaleler)
	static QString getEnableCEIProgramInstallerFilePath();
	static QString getDisableCEIProgramInstallerFilePath();

	static QString getParallelsDriversDir();
	/// returns path to the directory where Parallels applications is located
	static QString getParallelsApplicationDir();
	static QString getParallelsScriptsDir();

	/// returns true if build is runt under developers build environment.
	static bool isDevelopersBuild();

	//get system temp directory
	static QString getSystemTempDir();

	/**
	 * Get current user temp directory. Actually it invokes GetTempDirW for
	 * windows platform, and getSystemTempDir for others.
	 */
	static QString getCurrentUserTempDir();

	// get Windows applications directory -
	// in this directory associations with Guest windows application stored
	static QString getMappingApplicationsDir(const QString & strVmHomeDir);

	// get Windows Disks directory -
	// in this directory associations with Guest windows disks stored
	static QString getMappingDisksDir(const QString & strVmHomeDir);

	// get Snapshots directory -
	// in this directory Snapshot storage present
	static QString getSnapshotsDir(const QString & strVmHomeDir);

	// get Guest Crash Dumps directory -
	// in this directory guest crash dumps stored
	static QString getVmGuestCrashDumpsDir(const QString & strVmHomeDir);

	// get full path to VmInfo file for VM with [strVmHomeDir] home path
	static QString getVmInfoPath(const QString & strVmHomeDir);

	// get Parallels application directory
	static QString getParallelsDirName();

	// get base path to Parallels Tools .iso image
	static QString getToolsBaseImagePath(PRL_APPLICATION_MODE mode);
	// get full path to .iso with Parallels Tools for the given guest OS
	// Note: if function returns an empty string then OS is not supported
	static QString getToolsImage(PRL_APPLICATION_MODE mode, unsigned int nOsVersion);

	// get full path to .tar.gz with Parallels Tools for the given guest OS
	// Note: if function returns an empty string then OS is not supported
	static QString getToolsTarGz(PRL_APPLICATION_MODE mode, unsigned int nOsVersion);

	// get Parallels Tools installer name for the given guest OS
	// Note: if function returns an empty string then OS is not supported
	static QString getToolsInstallerName(unsigned int nOsVersion);

	// get full path to .fdd with Parallels Tools
	// return "" if fdd image doesn't required for this guest
	static QString getFddToolsImage( PRL_APPLICATION_MODE mode, unsigned int uGuestOsType );

	// get full path to .fdd with Parallels Tools
	static QString getFddToolsImageBaseName( unsigned int uGuestOsType );

	// get full path to .iso with Parallels Tools
	static QString getToolsFileName(unsigned int uGuestOsType);

	// get full path to .iso with unattended floppy drivers
	static QString getWindowsUnattendedFloppy(unsigned int osVersion_);
	// get reconfiguration image path
	static QString getLinReconfigImage(PRL_APPLICATION_MODE mode);

	// get fill path to unrar utility
	static QString getUnrarPath();

	static QStringList getCrashReportSystemLogPathsList();
	static QStringList getCrashReportUserLogPathsList( const QString & strUserHome );

	static QString getCrashDumpsPath();

	static QString getSystemLogPath();
	static QString getDefaultSystemLogPath(PRL_APPLICATION_MODE nAppMode);
	static QString getClientLogPath();

	static QString getPCSCBinPath();
	static QString getPCSCSocketPath();
	static QString getPCSCShmPath();

	static QString getNaptdPidPath();
	static QString getNaptdProxyPidPath();
	static QString getNaptdProxySocketPath( const QString& userName );

#ifndef _WIN_
	static QString getClientLogDirPath( const QString & strUserHome );
#endif

	static void setClientLogPathByAppMode(PRL_APPLICATION_MODE nAppMode);

	static QString getCurrentUserHomeDir();

	static QString getPVAInstallLogPath();

	static QString getScreenshotSoundFile();

	// return correctly path if conversion from mac pd3 present
	// else return empty string
	static QString getPd3NetworkSettingsPath();

	static QString getAppResourcesPath();

	static QString getProductPath();

	static QString getDispatcherPluginsPath();


	// developers issue to load app mode from file "appPath" + ".params"
	//		need to start applications from Run.py
	// if error - return PAM_UNKNOWN
	static QPair<PRL_APPLICATION_MODE,InitOptions> loadAppExecuteMode( const QString& appPath );

	//return execution mode specified during
	//Gen.py [forceserver|...].
	//Gen.py save according macros,
	//then getBuildExecutionMode() select mode by macros.
	//return PAM_UNKNOWN if macros are not specified
	static PRL_APPLICATION_MODE getBuildExecutionMode();

	static QString getDefaultPramPath();
	static QString getDefaultBackupDir();

#ifdef _WIN_
	// get product install directory
	// Note: VER_REG_TREE_ROOT_STR can be used as argument to get install
	// Note: directory for current build product.
	// Note: Include "Build/Current.ver" to get

	// Note: Installer uses only 32-bit registry view
	static QString getProductInstallDir(const char *const productRegistry);
#endif

	static QString getDefaultSwapPathForVMOnNetworkShares();

	// Get migration statistics/reports file path
	static QString getMigrationReportFilePath();

	// Get mounter statistics/reports file path
	static QString getMounterReportFilePath();

	// Get Installation log file path
	static QStringList getInstallationLogFilePaths();

	// Get path to dispatcher local UNIX socket
	static QString getDispatcherLocalSocketPath();

	// Returns true if build is run in PSBM environment.
	static bool isServerModePSBM();

	// Returns true if build is run in Virtuozzo for Windows environment.
	static bool isServerModeVZWIN();

	// returns Vm memory file location (directory)
	static QString getVmMemoryFileLocation(
		const QString &sVmUuid,
		const QString &sVmHomeDir,
		const QString &sSwapDir,
		const QString &sSwapPathForSharedVm,
		bool bUseSwapPathForSharedVm,
		UINT64 uMemSize);

	static QString getVmAppPath(bool bX64);
	static QString getVmStarterPath();
	static QString getConvertToolPath( const QDir& baseDir );
	static QString getDiskToolPath( const QDir& baseDir );
	static QString getVmScriptsDir(const QString &sBaseDir);
	static QString getVmActionScriptPath(const QString &sBaseDir, PRL_VM_ACTION nAction);
	static QString getVmConfigurationSamplePath(const QString &sName);
	static QString get7zipUtilityPath( const QDir& baseDir );
	static QString getMkisoUtilityPath( const QDir& baseDir );
	static QString getDesktopControlAgentPath( const QString& baseDir );
	static QString getUpdaterUtilityPath(const QString &sBaseDir,
										 const QString& buildMode,
										 PRL_APPLICATION_MODE mode = PAM_UNKNOWN);
	static QString getUpdaterUrl(PRL_APPLICATION_MODE appMode);

#ifdef _WIN_
	static QString getProcDumpToolPath(bool bX64);
#endif

	static bool isMultilanguagesBuild();

	static QString getServiceAppName();

private:
	static QString getDefaultVmCatalogue(const ParallelsDirs::UserInfo* pUserInfo);
	static QString getIPCPath(const QString& fileName, const QString& humanName);

public:
	struct UserInfo
	{
#ifdef _WIN_
		UserInfo( void* userAuthToken, const QString& homePath);
#else
		UserInfo( const QString& userName, const QString& homePath);
#endif

	UserInfo( const UserInfo& );
	explicit UserInfo();

	UserInfo& operator=( const UserInfo& );
	void printUserInfo();
	QString getHomePath() const {return m_homePath; }

	private:
		friend class ParallelsDirs;

		bool isValid()  const;

#ifdef _WIN_
		void* m_userAuthToken;
#else
		QString m_userName;
#endif
		QString m_homePath;
	}; //UserInfo

private:
	static PRL_APPLICATION_MODE ms_nApplicationMode;
	static bool ms_bAppModeInited;
	static InitOptions ms_nInitOptions;

};
Q_DECLARE_OPERATORS_FOR_FLAGS( ParallelsDirs::InitOptions );

#endif // __PARALLELS_DIRS_H__

