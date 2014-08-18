/*
 * ParallelsDirs.cpp: Helper class for getting default parallels
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

/*
 *     in _WIN_ version
 *
 *    if user hasn't userprofile directory in host (C:\Documents and Settings\<username>)
 *    we make user My Documtents directory  from default user (C:\Documents and Settings\Default\..)
 *     BUT WE DON'T SET SPECIAL PERMISSION to this Directory (required 'access user only')
 *     (we only return path string).
 *    IT IS POSSIBLE SECURITY HOLE for our users.
 */


#include "ParallelsDirs.h"
#include "ParallelsQt.h"

#include "Libraries/Logging/Logging.h"
#include "Libraries/HostUtils/HostUtils.h"
#include "Interfaces/ParallelsTypes.h"
#include "SDK/Include/PrlOses.h"
#include "Libraries/PrlCommonUtilsBase/Common.h"
#include "Libraries/Std/PrlAssert.h"

#include "CommandLine.h"
#include "OsInfo.h"
#include "ParallelsDirsDefs.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QProcess>
#include <QLocale>
#include <QMutexLocker>

#include "Build/Current.ver"
#include "Build/Current-locale.ver"

#ifdef _WIN_
#   include <windows.h>
#   include <shlobj.h>
#   include <shlwapi.h>
#else
#   include <stdlib.h>
#   include <unistd.h>
#   include <sys/types.h>
#   include <sys/un.h>
#   include <pwd.h>
#   include <errno.h>
#   include <string.h>
#   ifdef _LIN_
#       include <sys/utsname.h>
#       include <sys/stat.h>
#   else
#       error "platform not defined";
#   endif
#endif


PRL_APPLICATION_MODE ParallelsDirs::ms_nApplicationMode =  PAM_UNKNOWN;
ParallelsDirs::InitOptions ParallelsDirs::ms_nInitOptions = ParallelsDirs::smNormalMode;
bool ParallelsDirs::ms_bAppModeInited =  false;

namespace
{
	const char *const g_strParallelsDirName =
#ifdef _LIN_
				"parallels";
#else
				"Parallels";
#endif

#ifdef _WIN_
	const char *const g_strApplicationSwitcherDirName =
				"Application Switcher";
	const char *const g_strApplicationSwitcherFilesDirName =
				"Files";

	const char *const g_strDefaultInstallDrive =
				"C:";
	const char *const g_strUtw7DirName =
				"PD5UTW7";
	const char *const g_strUtw7AgentAppDirName =
				"Upgrade to Windows 7 Agent";
	const char *const g_strUtw7MediaDirName =
				"Movies";
	const char *const g_strAdobeAirAppName =
				"Adobe AIR Application Installer.exe";

	// uuids for report container name
	const char *const g_strPostMigrationUuid =
				"{FB9BA5E5-B6A1-4ef0-8DB5-68924298AE3E}.xml";

	const char *const g_strPreMigrationUuid =
				"{68B1217E-F507-4c48-BB9A-39F25FCAC938}.xml";

#endif // _WIN_
}

#ifdef _WIN_
static HRESULT GetMyDocumetsPathIfDirNotExist(HANDLE hToken, TCHAR* outPath, size_t outPath_sz);
#endif

ParallelsDirs::UserInfo::UserInfo( )
#ifdef _WIN_
: m_userAuthToken(0)
#endif
{

}

#ifdef _WIN_
ParallelsDirs::UserInfo::UserInfo(void* userAuthToken, const QString& homePath )
: m_userAuthToken(userAuthToken),
  m_homePath( homePath )
{}
bool ParallelsDirs::UserInfo::isValid() const
{
	return m_userAuthToken!=0;
}
#else
ParallelsDirs::UserInfo::UserInfo(const QString& userName, const QString& homePath)
: m_userName(userName),
  m_homePath( homePath )
{}
bool ParallelsDirs::UserInfo::isValid()  const
{
	return !m_userName.isEmpty();
}
#endif

ParallelsDirs::UserInfo::UserInfo( const ParallelsDirs::UserInfo& ui)
{
#	ifdef _WIN_
		m_userAuthToken = ui.m_userAuthToken;
#	else
		m_userName = ui.m_userName;
#	endif
		m_homePath = ui.m_homePath;
}

ParallelsDirs::UserInfo& ParallelsDirs::UserInfo::operator=( const ParallelsDirs::UserInfo& ui )
{
	if( &ui == this )
		return *this;

#	ifdef _WIN_
	m_userAuthToken = ui.m_userAuthToken;
#	else
	m_userName = ui.m_userName;
#	endif

	m_homePath = ui.m_homePath;

	return *this;
}

void ParallelsDirs::UserInfo::printUserInfo()
{
#ifndef _WIN_
	WRITE_TRACE( DBG_WARNING, "User with name %s",
		QSTR2UTF8( m_userName ) );
#endif //_WIN
	WRITE_TRACE( DBG_WARNING, "User with home path %s",
					QSTR2UTF8( m_homePath ) );
}

QString ParallelsDirs::getConfigScriptsDir()
{
    PRL_APPLICATION_MODE appMode = getAppExecuteMode();
    QString folder;
    switch( appMode )
    {
    case PAM_SERVER: folder = "server"; break;
    default:
        PRL_ASSERT(false);
        return "";
    }
    return QDir::toNativeSeparators(getDispatcherConfigDir() + '/' + folder);
}

QString ParallelsDirs::getDispatcherConfigDir()
{
	//Win: C:\Documents and Settings\All Users\Application Data\Parallels
	//Vista: C:\ProgramData\Parallels
	//Lin: /etc/parallels
	//Mac: /Library/Preferences/Parallels
	//Mac AppStore: ~/Library/Preferences/Parallels

	QString path;

	do
	{
		path = Prl::getenvU(PVS_DISPATCHER_CONFIG_DIR_ENV);
		if( !path.isEmpty () )
		{
			WRITE_TRACE(DBG_FATAL, "PVS_DISPATCHER_CONFIG_DIR_ENV: was set from enviroment: '%s'"
				, QSTR2UTF8( path ) );
			break;
		}

#ifdef _WIN_
	{
		TCHAR   szPath[MAX_PATH];
		HRESULT hres=SHGetFolderPath(NULL,
			CSIDL_COMMON_APPDATA,
			NULL, 0, szPath);
		if(!SUCCEEDED(hres))
		{
			WRITE_TRACE (DBG_FATAL, "Can't get profile by error %d", GetLastError());
			break;
		}
		path=UTF16_2QSTR(szPath);
	}
#   elif _LIN_
		path="/etc";
#   endif

		path+="/";
		path+=UTF8_2QSTR(g_strParallelsDirName);

		path=QDir::fromNativeSeparators(path);
	}while(0);
	return path;
}

QString ParallelsDirs::getDispatcherConfigFilePath()
{
	QString fName;

	PRL_APPLICATION_MODE appMode = getAppExecuteMode();
	switch( appMode )
	{
	case PAM_SERVER:
		fName = DISPATCHER_CONFIGURATION_SERVER_XML_FILE;
		break;
	default:
		fName = "fake.disp.xml";
		WRITE_TRACE(DBG_FATAL, "%s:  Not supported appMode = %d. config fname = %s"
			, __FUNCTION__
			, appMode
			, QSTR2UTF8( fName ) );
	}

	QString strDispConfigFile = QString( "%1/%2" ).
		arg( getDispatcherConfigDir() ).
		arg( fName );

	return strDispConfigFile;
}

QString ParallelsDirs::getDispatcherVmCatalogueFilePath()
{
	QString fName;

	PRL_APPLICATION_MODE appMode = getAppExecuteMode();
	switch( appMode )
	{
	case PAM_SERVER:
		fName = VMDIR_DEFAULT_CATALOGUE_SERVER_FILE;
		break;
	default:
		fName = "fake.vmdirlist.xml";
		WRITE_TRACE(DBG_FATAL, "%s:  Not supported appMode = %d. config fname = %s", __FUNCTION__, appMode, QSTR2UTF8( fName ) );
	}

	QString path = QString( "%1/%2" ).
		arg( getDispatcherConfigDir() ).
		arg( fName );
	return path;
}

QString ParallelsDirs::getLicensesFilePath()
{
	QString strLicensesFilePath = QString( "%1/%2" ).
		arg( getDispatcherConfigDir() ).
		arg( PARALLELS_LICENSES_XML_FILE );

	return strLicensesFilePath;
}

QString ParallelsDirs::getNetworkConfigFilePath( PRL_APPLICATION_MODE appMode )
{
	QString fName;
	switch( appMode )
	{
	case PAM_SERVER:
		fName = NETWORK_CONFIGURATION_SERVER_XML_FILE;
		break;
	default:
		WRITE_TRACE(DBG_FATAL, "Not supported appMode = %d", appMode );
		return "";
	}

	QString path = QString( "%1/%2" ).
		arg( getDispatcherConfigDir() ).
		arg( fName );

	return path;
}


QString ParallelsDirs::getNetworkConfigFilePath()
{
	PRL_APPLICATION_MODE appMode = getAppExecuteMode();
	QString path = getNetworkConfigFilePath( appMode );
	if( path.isEmpty() )
	{
		path = QString( "%1/%2" ).
			arg( getDispatcherConfigDir() ).
			arg( "fake.network.xml" );
		WRITE_TRACE(DBG_FATAL, "%s:  Not supported appMode = %d. config fname = %s"
			, __FUNCTION__
			, (int)appMode
			, QSTR2UTF8( QFileInfo(path).fileName() ) );
	}

	return path;
}

QString ParallelsDirs::getCallerUserPreferencesDir()
{
//Win: C:\Documents and Settings\<username>\Application Data\Parallels
//Vista: C:\Users\<username>\AppData\Roaming\Parallels
//Lin: $HOME/.parallels
//Mac: /Users/<username>/Library/Preferences/Parallels

	QString path;
	do
	{
#   ifdef _WIN_
		{
			TCHAR   szPath[MAX_PATH];
			HRESULT hres=SHGetFolderPath(NULL,
				CSIDL_APPDATA,
				NULL, 0, szPath);
			if(!SUCCEEDED(hres))
			{
				WRITE_TRACE (DBG_FATAL, "Can't get profile by error %d", GetLastError());
				break;
			}
			path=UTF16_2QSTR(szPath);
			path+="/";
			path+=UTF8_2QSTR(g_strParallelsDirName);
		}
#   else
		{
			uid_t euid=geteuid();
			struct passwd* pPswd=getpwuid(euid);
			if (!pPswd || !pPswd->pw_dir || !strlen(pPswd->pw_dir) )
			{
				WRITE_TRACE (DBG_FATAL, "Can't get profile by error %d, pswd=%p, pw_dir=%p"
					, errno, pPswd, pPswd?pPswd->pw_dir:"null");
				break;
			}
			//get home
			path=UTF8_2QSTR(pPswd->pw_dir);
			path+="/";
#			ifdef _LIN_
				path+= QString(".") + UTF8_2QSTR(g_strParallelsDirName);
#			else
#				error "Unsupported platform"
#			endif
}
#   endif
		path=QDir::fromNativeSeparators(path);

	}while(0);
	return path;
}

QString ParallelsDirs::getUserDefaultVmCatalogue(const ParallelsDirs::UserInfo*pUserInfo)
{
	return getDefaultVmCatalogue(pUserInfo);
}


QString ParallelsDirs::getCommonDefaultVmCatalogue()
{
	// Call PRIVATE METHOD. THAT SUPPORT ONLY SERVER or DESKTOP mode.
	return getDefaultVmCatalogue(0);

}

QString ParallelsDirs::getUserHomePath(const ParallelsDirs::UserInfo* pUserInfo)
{
	if ( ! pUserInfo || ! pUserInfo->isValid() )
		return "";

	return pUserInfo->m_homePath;
}

#ifdef _WIN_
QString ParallelsDirs::getCommonDocumentsDirectory()
{
	QString strResult;
	TCHAR   szPath[MAX_PATH];
	HRESULT hres;

	hres = SHGetFolderPath( NULL, CSIDL_COMMON_DOCUMENTS, NULL, 0, szPath );

	if( !SUCCEEDED( hres ) )
	{
		WRITE_TRACE(DBG_FATAL, "Can't SHGetFolderPath by error %d", GetLastError());
		return strResult;
	}

	ireturn UTF16_2QSTR(szPath);
}
#endif //_WIN_

QString ParallelsDirs::getDefaultVmCatalogue(
	const ParallelsDirs::UserInfo* pUserInfo
	)
{
//Win: (environmental variable $PARALLELS_DIR)
//Server: C:\Documents and Settings\All Users\Documents\Public Parallels
//Workstation: C:\Documents and Settings\<username>\Documents\My Parallels
//
//Vista:
//Server: C:\Users\Public\Documents\Public Parallels
//Workstation: C:\Users\<username>\Documents\My Parallels
//
//Lin:
//Server: /var/paralles
//Workstation: $HOME/parallels/
//
//Mac:
//  Server:  /Users/Shared/Parallels
//  Desktop: /Users/<username>/Documents/Parallels/
//

	QString path;
	do
	{
		// Try get strDefaultCommonVmCatalogueDir value from environment
		if(NULL == pUserInfo)
		{
			path = Prl::getenvU(PVS_DISPATCHER_CONFIG_DIR_ENV);
			if( !path.isEmpty () )
			{
				WRITE_TRACE(DBG_FATAL, "PVS_VMCATALOGUE_DIR_ENV: was set from enviroment: '%s'"
					, QSTR2UTF8( path ) );
				break;
			}
		}

#   ifdef _WIN_
		{
			int typeCSIDL=0;
			HANDLE hToken=NULL;

			TCHAR   szPath[MAX_PATH];
			HRESULT hres;

			if(PAM_SERVER==mode)
			{
				typeCSIDL=CSIDL_COMMON_DOCUMENTS;
				hToken=NULL;
				hres=SHGetFolderPath(NULL, typeCSIDL , hToken, 0, szPath);
			}

			if(!SUCCEEDED(hres))
			{
				WRITE_TRACE(DBG_FATAL, "Can't SHGetFolderPath by error %d", GetLastError());
				break;
			}

			path=UTF16_2QSTR(szPath);
			path+="/";
			if(PAM_SERVER==mode)
				path+=UTF8_2QSTR("Public Parallels");
		}
#   else
//#   elif _LIN_
		{
			if(NULL == pUserInfo)
			{
#			ifdef _LIN_
				path=QString("/var");
#			else
#				error "Unsupported platform"
#			endif
				path+=QString("/")+ UTF8_2QSTR(g_strParallelsDirName);
			}
			else
			{
				struct passwd* pwd=getpwnam(QSTR2UTF8(pUserInfo->m_userName));
				if (!pwd || !pwd->pw_dir || !strlen(pwd->pw_dir) )
				{
					WRITE_TRACE (DBG_FATAL, "Can't get profile by error %d, pswd=%p, pw_dir=%p"
						, errno, pwd, pwd?pwd->pw_dir:"null");
					break;
				}
				path=UTF8_2QSTR(pwd->pw_dir);//home dir
				path+=QString("/")+ UTF8_2QSTR(g_strParallelsDirName);
			}

		}
#   endif
		path=QDir::fromNativeSeparators(path);

	} while(0);

	return path;
}

#ifdef _WIN_
static HRESULT GetMyDocumetsPathIfDirNotExist(HANDLE hToken, TCHAR* outPath, size_t outPath_sz)
{
	TCHAR szBuff[MAX_PATH];
	int typeCSIDL=0;
	HRESULT hres=E_INVALIDARG;
	HANDLE  token=hToken;

	if (!hToken || outPath_sz!=MAX_PATH)
		return E_INVALIDARG;


	do
	{
		DWORD dw;
		dw=SHGFP_TYPE_CURRENT;

		dw=SHGFP_TYPE_DEFAULT;
		typeCSIDL=CSIDL_PERSONAL;
		token=(HANDLE)-1;//default user
		hres=SHGetFolderPath(NULL, typeCSIDL , token, dw, szBuff);
		if (SUCCEEDED(hres))
		{

			QString myDocs=UTF16_2QSTR(szBuff);
			// c:\DOcuments and Settings\Default User\My Documents"
			int lastSlash=-1, lastSecondSlash=-1;

			QChar chSLASH='\\';

			if(myDocs.endsWith(chSLASH))
				myDocs.chop(1);

			lastSlash=myDocs.lastIndexOf(chSLASH);
			if(-1==lastSlash || 0==lastSlash)
				break;
			lastSecondSlash=myDocs.lastIndexOf(chSLASH, lastSlash-1);
			if (lastSlash!=-1 && lastSecondSlash!=-1 && lastSlash!=lastSecondSlash)
			{
				PRL_ASSERT( ImpersonateLoggedOnUser(hToken) );

				DWORD sz=sizeof(szBuff)/sizeof(TCHAR);
				GetUserName(szBuff, &sz);
				myDocs.replace(lastSecondSlash+1, (lastSlash-lastSecondSlash)-1, UTF16_2QSTR(szBuff));

				PRL_ASSERT( RevertToSelf() );

				if (0==wcscpy_s(outPath, outPath_sz, myDocs.utf16()))
					hres=S_OK;
				else
					hres=E_FAIL;
			}
		}
	} while(0);

	return hres;
}

#endif


QString ParallelsDirs::getParallelsApplicationDir()
{
	QString sParallelsInstallDir = Prl::getenvU(PVS_VM_EXECUTABLE_ENV);
	if( sParallelsInstallDir.isEmpty() )
	{
		sParallelsInstallDir = QCoreApplication::applicationDirPath();
	}
	return sParallelsInstallDir;
}


QString ParallelsDirs::getParallelsScriptsDir()
{
	QString sParallelsScriptsDir = Prl::getenvU(PVS_VM_SCRIPTS_ENV);
	if( sParallelsScriptsDir.isEmpty() )
	{
		QDir dir(QCoreApplication::applicationDirPath());
		dir.cd("scripts");
		sParallelsScriptsDir = dir.absolutePath();
	}
	return sParallelsScriptsDir;
}


QString ParallelsDirs::getParallelsDriversDir()
{
	// Pathes defined in bug #1670

	//path = QDir::currentPath () + "/"; //??? always return c:\windows\system32 ???
	QString currDir = QCoreApplication::applicationDirPath();

	// 1. get parent directory of binary
	QString parentOfBinDir;

#   ifdef _WIN_
	parentOfBinDir = currDir + "/..";
#   elif _LIN_
	parentOfBinDir = currDir + "/..";
#   endif

	// 2. if it is 'z-Build' is development version
	//      otherwise - installed version

	parentOfBinDir = QDir( parentOfBinDir ).absolutePath();

	QString path;

	if ( QDir( parentOfBinDir ).dirName() == "z-Build" )
		path = parentOfBinDir + "/Drivers";
	else
	{
#       ifdef _WIN_
			path = parentOfBinDir + "/Drivers";
#       elif _LIN_
		{
			struct utsname un;
			if ( 0 != uname ( &un ))
				WRITE_TRACE(DBG_FATAL, "can't get kernel version err = %d, %s" , errno, strerror( errno ) );
			else
			{
				path = "/lib/modules/";
				path += un.release;
				path += "/extra/parallels";
			}//if uname
		}
#       endif
	}

	LOG_MESSAGE(DBG_DEBUG, "This path = (%s)", QSTR2UTF8( path ) );

	return path;
}


// returns true if we are runt under developers build environment
bool ParallelsDirs::isDevelopersBuild()
{
	static bool s_bBuildModeInitialized = false;
	static bool s_bDevelopersBuild = false;

	if (!s_bBuildModeInitialized)
	{
		s_bBuildModeInitialized = true;

		QString currDir = QCoreApplication::applicationDirPath();
		currDir = QDir::toNativeSeparators(QDir( currDir ).absolutePath());
		if ( currDir.contains(QDir::toNativeSeparators("z-Build/Release")) ||
				currDir.contains(QDir::toNativeSeparators("z-Build/Debug")) )
			s_bDevelopersBuild = true;
		else
			s_bDevelopersBuild = false;
	}
	return s_bDevelopersBuild;
}

bool ParallelsDirs::isServerModePSBM()
{
#ifdef _LIN_
	if (getAppExecuteMode() == PAM_SERVER)
	{
		struct stat st;
		if (!stat("/proc/fairsched2", &st))
			return true;
	}
#endif
	return false;
}

bool ParallelsDirs::isServerModeVZWIN()
{
#ifdef _WIN_
	if (getAppExecuteMode() == PAM_SERVER)
	{
		HKEY hkey = NULL;
		RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\SWSoft\\Virtuozzo", &hkey);
		if (hkey) {
			RegCloseKey(hkey);
			return true;
		}
	}
#endif
	return false;
}

QString ParallelsDirs::getSystemTempDir()
{
#   ifndef _WIN_
	return "/tmp";
#   else


	QString path;
	do
	{
		TCHAR   szPath[MAX_PATH];
		// In Terminal Services environment SHGetFolderPath with CSIDL_WINDOWS will
		// return a "virtualized" path like "Documents and Settings\%USERNAME%\WINDOWS
		// To get a real system directory a GetSystemWindowsDirectory should be used
		if ( 0 == GetSystemWindowsDirectory( szPath, MAX_PATH ) )
		{
			WRITE_TRACE (DBG_FATAL, "Can't get windows by error %d", GetLastError());
			break;
		}
		path=UTF16_2QSTR(szPath);
		// Use native slashes
		path+="\\Temp";
	}while( 0 );

	return path;

#   endif

}

QString ParallelsDirs::getCurrentUserTempDir()
{
#ifdef _WIN_
	// Here is the similar logic to GetTempPathW function. Avoided to use it,
	// because it has known bugs when working with long names (>MAX_PATH), so
	// I suggest, that it would be more safe.
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa364992%28v=vs.85%29.aspx
	static const char * tmpLocations[] = {"TMP", "TEMP", "USERPROFILE"};
	QString tempDir;

	for (quint32 i = 0; i < sizeof(tmpLocations) / sizeof(tmpLocations[0]); ++i)
	{
		tempDir = Prl::getenvU(tmpLocations[i]);
		if (!tempDir.isEmpty())
			break;
	}
	return tempDir;
#else
	return ParallelsDirs::getSystemTempDir();
#endif
}

// get Mapping applications directory -
// in this directory associations with Guest windows application stored
QString ParallelsDirs::getMappingApplicationsDir(const QString & strVmHomeDir)
{
	return strVmHomeDir + "/" + QObject::tr(VM_GENERATED_WINDOWS_APPLICATION_DIR);
}

// get Mapping Disks directory -
// in this directory associations with Guest disks stored
QString ParallelsDirs::getMappingDisksDir(const QString & strVmHomeDir)
{
	return strVmHomeDir + "/" + QObject::tr(VM_GENERATED_WINDOWS_WIN_DISKS_DIR);
}

// get Snapshots directory -
// in this directory Snapshot storage present
QString ParallelsDirs::getSnapshotsDir(const QString & strVmHomeDir)
{
	return strVmHomeDir + "/" + QObject::tr(VM_GENERATED_WINDOWS_SNAPSHOTS_DIR);
}

// get Guest Crash Dumps directory -
// in this directory guest crash dumps stored
QString ParallelsDirs::getVmGuestCrashDumpsDir(const QString & strVmHomeDir)
{
	return strVmHomeDir + "/" + VM_GENERATED_GUEST_CRASH_DUMPS_DIR;
}

// get full path to VmInfo file for VM with [strVmHomeDir] home path
QString ParallelsDirs::getVmInfoPath(const QString & strVmHomeDir)
{
	return strVmHomeDir + "/" + VM_INFO_FILE_NAME;
}

// get Parallels application directory
QString ParallelsDirs::getParallelsDirName()
{
	return QString(g_strParallelsDirName);
}

// get base path to Parallels Tools .iso image
QString ParallelsDirs::getToolsBaseImagePath(PRL_APPLICATION_MODE mode)
{
	QString path;

# ifdef _WIN_

	Q_UNUSED(mode);
	path = getProductInstallDir( VER_REG_TREE_ROOT_STR ) + "/Tools/";
	path[0] = path.toUpper().at(0);

# elif _LIN_

	if (mode == PAM_SERVER)
		path = "/usr/share/parallels-server/tools/";
	return path;
#endif
}

QString ParallelsDirs::getToolsImage(PRL_APPLICATION_MODE mode, unsigned int nOsVersion)
{
	QString qsFileName;
	if (IS_WINDOWS(nOsVersion) && (nOsVersion >= PVS_GUEST_VER_WIN_2K))
	{
		qsFileName = "prl-tools-win.iso";
	}
	else if IS_MACOS(nOsVersion)
	{
		qsFileName = "prl-tools-mac.iso";
	}
	else if IS_LINUX(nOsVersion)
	{
		qsFileName = "prl-tools-lin.iso";
	}
	else
	{
		qsFileName = "prl-tools-other.iso";
	}

	QString qsToolsImage;
	if (!qsFileName.isEmpty())
	{
		qsToolsImage = getToolsBaseImagePath(mode) + qsFileName;
	}
	return qsToolsImage;
}

QString ParallelsDirs::getToolsTarGz(PRL_APPLICATION_MODE mode, unsigned int nOsVersion)
{
	QString qsFileName;
	if (IS_WINDOWS(nOsVersion) && (nOsVersion >= PVS_GUEST_VER_WIN_2K))
	{
		qsFileName = "prl-tools-win.tar.gz";
	}
	else if IS_MACOS(nOsVersion)
	{
		qsFileName = "prl-tools-mac.tar.gz";
	}
	else if IS_LINUX(nOsVersion)
	{
		qsFileName = "prl-tools-lin.tar.gz";
	}
	else
	{
		qsFileName = "prl-tools-other.tar.gz";
	}

	QString qsToolsImage;
	if (!qsFileName.isEmpty())
	{
		qsToolsImage = getToolsBaseImagePath(mode) + qsFileName;
	}
	return qsToolsImage;
}

QString ParallelsDirs::getToolsInstallerName(unsigned int nOsVersion)
{
	QString qsFileName;
	if (IS_WINDOWS(nOsVersion) && (nOsVersion >= PVS_GUEST_VER_WIN_2K))
	{
		qsFileName = "prl-tools-win.tar.gz";
	}
	else if IS_MACOS(nOsVersion)
	{
		qsFileName = "prl-tools-mac.iso";
	}
	else if IS_LINUX(nOsVersion)
	{
		qsFileName = "prl-tools-lin.iso";
	}

	return qsFileName;
}

// get base fil name of .fdd Parallels Tools
QString ParallelsDirs::getFddToolsImageBaseName( unsigned int uGuestOsType )
{
	if (uGuestOsType == PVS_GUEST_TYPE_OS2)
		return "prl-tools-os2.fdd";
	else
		return "";
}
// get full path to .fdd with Parallels Tools
QString ParallelsDirs::getFddToolsImage( PRL_APPLICATION_MODE mode, unsigned int uGuestOsType )
{
	if (uGuestOsType == PVS_GUEST_TYPE_OS2 )
		return getToolsBaseImagePath(mode) + ParallelsDirs::getFddToolsImageBaseName( uGuestOsType );
	else
		return "";
}

QString ParallelsDirs::getLinReconfigImage(PRL_APPLICATION_MODE mode)
{
	Q_UNUSED(mode);

#if _LIN_
	// PVS case
	return "/usr/share/parallels-reconfiguration/reconfiguration.iso";
#else
	return "";
#endif 
}

namespace{
	QStringList getSubDirsPaths( const QString& sPath )
	{
		QStringList lst;

		QFileInfo fi(sPath);
		PRL_ASSERT(fi.isAbsolute());
		if( !fi.exists() )
			return lst;

		QDir::Filters
			dirFilter  = QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks;

		QFileInfoList fiList = QDir(sPath).entryInfoList( dirFilter );

		foreach( QFileInfo fi, fiList )
		{
			lst << fi.absoluteFilePath();
			if(fi.isSymLink())
				continue;
			// #125021 to prevent infinity recursion by QT bug in QDir::entryInfoList()
			if( QFileInfo(sPath) != fi )
				lst << getSubDirsPaths( fi.absoluteFilePath() );
		}

		return lst;
	}

} // namespace

QString ParallelsDirs::getCrashDumpsPath()
{
		// Use native slashes
#if defined(_WIN_)
	return getSystemTempDir() + "\\ParallelsCrashDumps";
#else
	return getSystemTempDir() + "/parallels_crash_dumps";
#endif
}

QString ParallelsDirs::getSystemLogPath()
{
	return QString("%1/%2").arg(UTF8_2QSTR(GetDefaultLogFilePath()))
						   .arg(GetProdDefaultLogFileName());
}

QString ParallelsDirs::getDefaultSystemLogPath(PRL_APPLICATION_MODE)
{
	QString fileName = PRL_LOG_FILE_NAME_DEFAULT;
	return QString("%1/%2").arg(UTF8_2QSTR(GetDefaultLogFilePath()))
						   .arg(fileName);
}

QString ParallelsDirs::getClientLogPath()
{
	return QString("%1/%2").arg(UTF8_2QSTR(GetUserHomeDir()))
						   .arg(GetProdDefaultLogFileName());
}

QString ParallelsDirs::getPCSCBinPath()
{
#ifdef _WIN_
	PRL_ASSERT("Attempt to call getPCSCBinPath() under Win" == 0);
	return QString();
#else
	QString dir_path = QCoreApplication::instance()->applicationDirPath();
#ifdef _LIN_
	return dir_path + "/prl_pcsc_proxy";
#endif
#endif
}

QString ParallelsDirs::getPCSCSocketPath()
{
	return getIPCPath("prl_pcsc_proxy.socket", "PrlPCSC-socket");
}

QString ParallelsDirs::getPCSCShmPath()
{
	return getIPCPath("prl_pcsc_proxy.shm", "PrlPCSC-shm");
}

QString ParallelsDirs::getNaptdPidPath()
{
	QString pidFile="prl_naptd.pid";
	return getIPCPath("prl_naptd.pid", "Naptd-pid");
}

QString ParallelsDirs::getNaptdProxyPidPath()
{
	QString pidFile="prl_naptd.pcproxy.pid";

	return QString( "%1/Library/Parallels/%2" )
			.arg( ParallelsDirs::getCurrentUserHomeDir() )
			.arg( pidFile );
}


QString ParallelsDirs::getNaptdProxySocketPath( const QString& userName )
{
	QString sockName= QString( "com.parallels.desktop.parental_control_socket2.%1").arg( userName );

	return QString("/var/tmp/%1").arg(sockName);
}

void ParallelsDirs::setClientLogPathByAppMode(PRL_APPLICATION_MODE nAppMode)
{
	if (nAppMode == PAM_SERVER)
		ParallelsDirs::setLogPath( QSTR2UTF8(ParallelsDirs::getClientLogPath()) );
}

// get for currently logged user home path
QString ParallelsDirs::getCurrentUserHomeDir()
{
	//Win: C:\Documents and Settings\<username>
	//Vista: C:\Users\<username>
	//Lin: $HOME
	//Mac: /Users/<username>

	QString path;
	do
	{
#   ifdef _WIN_
		{
			TCHAR   szPath[MAX_PATH];
			HRESULT hres=SHGetFolderPath(NULL,
				CSIDL_PROFILE,
				NULL, 0, szPath);
			if(!SUCCEEDED(hres))
			{
				WRITE_TRACE (DBG_FATAL, "Can't get profile by error %d", GetLastError());
				break;
			}
			path=UTF16_2QSTR(szPath);
		}
#   else
		{
			uid_t euid=geteuid();
			struct passwd* pPswd=getpwuid(euid);
			if (!pPswd || !pPswd->pw_dir || !strlen(pPswd->pw_dir) )
			{
				WRITE_TRACE (DBG_FATAL, "Can't get profile by error %d, pswd=%p, pw_dir=%p"
					, errno, pPswd, pPswd?pPswd->pw_dir:"null");
				break;
			}

			//get home
			path=UTF8_2QSTR(pPswd->pw_dir);
		}
#   endif
		path=QDir::fromNativeSeparators(path);

	}while(0);
	return path;
}

// get full path to .iso with Parallels Tools
QString ParallelsDirs::getToolsFileName(unsigned int uGuestOsType)
{
	QString strToolsPath;

	if (IS_WINDOWS(uGuestOsType))
	{
		strToolsPath = "prl-tools-win.iso";
	}
	else if IS_MACOS(uGuestOsType)
	{
		strToolsPath = "prl-tools-mac.iso";
	}
	else if IS_LINUX(uGuestOsType)
	{
		strToolsPath = "prl-tools-lin.iso";
	}

	return strToolsPath;
}

QString ParallelsDirs::getUnrarPath()
{
#if _LIN_
	return "/usr/bin/unrar";
#else
	return "unrar";
#endif
}

QString ParallelsDirs::getScreenshotSoundFile()
{
	return "";
}

QString ParallelsDirs::getAppResourcesPath()
{
	QString resPath;

#ifdef _WIN_
	resPath = getProductInstallDir( VER_REG_TREE_ROOT_STR ) + "/Resources";
#endif

#ifdef _LIN_
	resPath = "/usr/share/parallels-desktop/resources";
#endif

	return resPath;
}


QString ParallelsDirs::getProductPath()
{
	QString resPath;

#ifdef _WIN_
	resPath = getProductInstallDir( VER_REG_TREE_ROOT_STR ) + "/";
#endif

#ifdef _LIN_
	resPath = "/usr/share/parallels-desktop/";
#endif

	return resPath;
}

QString ParallelsDirs::getPathToDispatcherTesterConfig()
{
	return QString( "%1/%2" )
		.arg( getDispatcherConfigDir())
		.arg( "dispatcher.tester.conf" );
}

QString ParallelsDirs::getEnableCEIProgramInstallerFilePath()
{
	return QString("%1/%2")
		.arg( getSystemTempDir() )
		.arg( INSTALLER_CEIPROGRAM_FLAG_FILE_ENABLE );
}

QString ParallelsDirs::getDisableCEIProgramInstallerFilePath()
{
	return QString("%1/%2")
		.arg( getSystemTempDir() )
		.arg( INSTALLER_CEIPROGRAM_FLAG_FILE_DISABLE );
}

QPair<PRL_APPLICATION_MODE, ParallelsDirs::InitOptions>
	ParallelsDirs::loadAppExecuteMode( const QString& appPath )
{
	QFile fAppMode( appPath + ".params" );
	if( ! fAppMode.open( QIODevice::ReadOnly ) )
	{
		WRITE_TRACE( DBG_INFO, "Can't open file '%s' ", QSTR2UTF8( fAppMode.fileName() ) );
		return qMakePair(PAM_UNKNOWN, InitOptions(smNormalMode) );
	}

	QString  argvCmdLine =  QString("%1 %2")
								.arg(appPath)
								.arg( QString( fAppMode.readLine() ) );
	CommandLine::Parser parser(argvCmdLine);

	//////////////////////////////////////////////////////////////////////////
	// set execute mode
	PRL_APPLICATION_MODE mode = PAM_UNKNOWN;

	const QString value=parser.getValueByKey(CommandLine::g_strCommonKeyName_ModeName);
	if (value==CommandLine::g_strCommonValue_ModeName_PS)
		mode = PAM_SERVER;
	else
		WRITE_TRACE( DBG_INFO, "Wrong value of execute mode %s.", QSTR2UTF8(value) );

	InitOptions subMode = smNormalMode;
	if( parser.hasKey(CommandLine::g_strCommonKeyName_AppStoreMode) )
		subMode = smAppStoreMode;

	return qMakePair(mode, subMode);
}

PRL_APPLICATION_MODE ParallelsDirs::getBuildExecutionMode()
{
	PRL_APPLICATION_MODE mode = PAM_SERVER;
	return mode;
}

QString ParallelsDirs::getDefaultBackupDir()
{
	QString sParallelsBackupDir = getCommonDefaultVmCatalogue();
#ifdef _WIN_
	sParallelsBackupDir += "\\Backups";
#else
	sParallelsBackupDir += "/backups";
#endif
	return sParallelsBackupDir;
}

QString ParallelsDirs::getDefaultPramPath()
{
	QString sDir = "";
#ifdef _LIN_
	sDir += "/mnt/pram_vms";
#endif
	return sDir;
}

#ifdef _WIN_
QString ParallelsDirs::getProductInstallDir(const char *const productRegistry)
{
	static const wchar_t *const c_instLoc = L"InstallLocation";
	LONG rerr;
	HKEY hk;
	DWORD type;
	wchar_t sb[MAX_PATH];
	wchar_t *val;
	DWORD valSz;
	wchar_t *tmp;
	QString ret;

	// Now installer uses only 32-bit registry view
#ifdef _64BIT_
	rerr = RegOpenKeyExA(HKEY_LOCAL_MACHINE, productRegistry,
						 0, KEY_QUERY_VALUE|KEY_WOW64_32KEY, &hk);
#else
	rerr = RegOpenKeyExA(HKEY_LOCAL_MACHINE, productRegistry,
						 0, KEY_QUERY_VALUE, &hk);
#endif
	switch (rerr)
	{
	case ERROR_SUCCESS:
		break;
	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
		WRITE_TRACE(DBG_INFO, "Product not installed, "
					"RegOpenKeyExA() err %i, key=\"%s\"",
					(int)rerr, productRegistry);
		goto deinit_fin;
	default:
		WRITE_TRACE(DBG_FATAL, "Failed to get product install dir, "
					"RegOpenKeyExA() err %i, key=\"%s\"",
					(int)rerr, productRegistry);
		goto deinit_fin;
	}

	val = sb;
	valSz = sizeof(sb);
	for (;;)
	{
		rerr = RegQueryValueExW(hk, c_instLoc, 0,
								&type, (LPBYTE)val, &valSz);
		switch (rerr)
		{
		case ERROR_SUCCESS:
		case ERROR_MORE_DATA:
			break;
		case ERROR_FILE_NOT_FOUND:
			WRITE_TRACE(DBG_INFO, "Product not installed, "
						"RegQueryValueExW() err %i, key=\"%s\"",
						(int)rerr, productRegistry);
			goto deinit_val;
		default:
			WRITE_TRACE(DBG_FATAL, "Failed to get product install dir, "
					"RegQueryValueExW() err %i, key=\"%s\"",
					(int)rerr, productRegistry);
			goto deinit_val;
		}
		if (REG_SZ != type)
		{
			WRITE_TRACE(DBG_FATAL, "Bad type of \"%s\" value, "
						"type=%u, valSz=%u",
						QSTR2UTF8(QString::fromWCharArray(c_instLoc)),
						(unsigned)type, (unsigned)valSz);
			goto deinit_val;
		}
		if (ERROR_SUCCESS == rerr) break;
		if (sb == val) val = 0;
		tmp = (wchar_t *)realloc(val, valSz);
		if (0 == tmp)
		{
			WRITE_TRACE(DBG_FATAL, "Failed to allocate %u bytes",
						(unsigned)valSz);
			goto deinit_val;
		}
		val = tmp;
	}
	ret = QString::fromWCharArray(val, (int)wcsnlen(val, valSz / sizeof(wchar_t)));
deinit_val:
	if (sb != val) free(val);
	RegCloseKey(hk);
deinit_fin:
	return ret;
}
#endif

QString ParallelsDirs::getAppGuiName( PRL_APPLICATION_MODE nAppMode )
{
	switch (nAppMode)
	{
		case PAM_SERVER: return PRL_PRODUCT_NAME_CONSOLE;
		default: return PRL_PRODUCT_NAME_UNKNOWN;
	}
}

QString	ParallelsDirs::getAppSwitcherAppName()
{
	return PRL_APP_SWITCHER_NAME;
}

QString ParallelsDirs::getLearnVideoAppName()
{
	return PRL_APP_LEARN_VIDEO_NAME;
}

#ifdef _WIN_
QString ParallelsDirs::getApplicationSwitcherDir()
{
	QString ret;
	HRESULT hr;
	wchar_t appData[MAX_PATH];

	hr = SHGetFolderPathW(0, CSIDL_COMMON_APPDATA, 0, SHGFP_TYPE_CURRENT,
						  appData);
	if (!SUCCEEDED(hr))
	{
		WRITE_TRACE(DBG_FATAL, "Failed to get CSIDL_COMMON_APPDATA, "
					"SHGetFolderPathW() err 0x%x", (unsigned)hr);
		goto end;
	}
	ret = QString::fromWCharArray(appData);
	if (ret.isEmpty())
	{
		WRITE_TRACE(DBG_FATAL, "Path for CSIDL_COMMON_APPDATA is empty");
		goto end;
	}
	switch (ret[ret.size() - 1].toAscii())
	{
	case '\\':
	case '/':
		break;
	default:
		ret.append('\\');
		break;
	}
	ret.append(g_strParallelsDirName);
	ret.append('\\');
	ret.append(g_strApplicationSwitcherDirName);
end:
	return ret;
}
#endif

#define VM_SWAP_SUBDIR	"swap"

QString ParallelsDirs::getDefaultSwapPathForVMOnNetworkShares()
{
#ifdef _WIN_
	QString sqSwapPath = QString( "%1/%2" ).
		arg( getDispatcherConfigDir() ).
		arg( QString(VM_SWAP_SUBDIR) );
#else
	QString sqSwapPath = QString("/var/.") + UTF8_2QSTR(g_strParallelsDirName)  +
						 QString("_") + QString(VM_SWAP_SUBDIR);
#endif

	return sqSwapPath;
}


QString ParallelsDirs::getMigrationReportFilePath()
{
#if defined _LIN_
	return "/var/log/parallels_migration.log";
#elif defined _WIN_
	// TODO correct path
	return "parallels_migration.log";
#endif
}

QString ParallelsDirs::getMounterReportFilePath()
{
#if defined _LIN_
	return "/var/log/parallels_mounter.log";
#elif defined _WIN_
	return "parallels_mounter.log";
#endif
}

#ifdef _WIN_
QString getWindowsDirectory()
{
	TCHAR   szPath[MAX_PATH];
	if ( 0 == GetSystemWindowsDirectory( szPath, MAX_PATH ) )
	{
		WRITE_TRACE (DBG_FATAL, "Can't get windows by error %d", GetLastError());
		return "";
	}
	return UTF16_2QSTR(szPath);
}
#endif //_WIN_

// Get Installation log file path
QStringList ParallelsDirs::getInstallationLogFilePaths()
{
	QStringList lstPathes;
#ifdef _LIN_
	PRL_APPLICATION_MODE appMode = getAppExecuteMode();
	switch( appMode )
	{
		case PAM_SERVER:
			lstPathes << "/var/log/parallels-server-install.log" << "/var/log/parallels-mc-install.log";
			break;
		default:
			WRITE_TRACE(DBG_FATAL, "%s:  Not supported appMode = %d"
				, __FUNCTION__
				, appMode );
	}
#endif //_LIN_

	WRITE_TRACE( DBG_DEBUG, "installation log pathes == %s",
		QSTR2UTF8( lstPathes.join("\n") ) );
	return lstPathes;
}

static QString getLocalUnixSocketFileName()
{
	return "prl_disp_service.socket";
}

QString ParallelsDirs::getDispatcherLocalSocketPath()
{
	return getIPCPath(getLocalUnixSocketFileName(), "UnixSockPath");
}

QString ParallelsDirs::getIPCPath( const QString& fileName, const QString& humanName)
{
#ifdef _WIN_
	PRL_ASSERT("Attempt to call getIPCPath() under Win" == 0);
	Q_UNUSED(fileName);
	Q_UNUSED(humanName);
	return "";
#else
	QString path;
	path = QString( "/var/run/%1" ).arg(fileName);

	LOG_MESSAGE(DBG_FATAL, "========== %s: %s='%s' ", __FUNCTION__
		, QSTR2UTF8(humanName), QSTR2UTF8(path));

	return path;
#endif
}

/*
parameters are:
- Vm uuid
- Vm home directory (not config path)
- per-Vm swap dir
- dispatcher's swap path for shared Vm
- use dispatcher's swap path for shared Vm by default
*/
QString ParallelsDirs::getVmMemoryFileLocation(
		const QString &sVmUuid,
		const QString &sVmHomeDir,
		const QString &sSwapDir,
		const QString &sSwapPathForSharedVm,
		bool bUseSwapPathForSharedVm,
		UINT64 uMemSize)
{
	QString sMemFilePath;

	if (sSwapDir.isEmpty()) {
		PRL_UINT64 uMaxFileSize = HostUtils::GetMaxFileSize(sVmHomeDir);

		if (bUseSwapPathForSharedVm || (uMaxFileSize < uMemSize)) {

			sMemFilePath = QString( "%1/%2" ).arg( sSwapPathForSharedVm ).arg( sVmUuid );
			QDir dir(sMemFilePath);
			if (!dir.exists()) {
				//
				// Create subdirectory only
				//
				dir.mkdir(sMemFilePath);
				//
				// And we are not going to check if we failed or succeeded. Check has to be done from above
				//
			}
		} else {
			sMemFilePath = sVmHomeDir;
		}
	} else {
		sMemFilePath = sSwapDir;
		Prl::ProcessEnvVariables(sMemFilePath);
	}
	return sMemFilePath;
}


QString ParallelsDirs::getVmAppPath(bool bX64)
{
	QString strVmExecutableDir = UTF8_2QSTR(getenv(PVS_VM_EXECUTABLE_ENV));

	// If environment variable PVS_VM_EXECUTABLE_ENV is not set,
	// we set full VM Controller path as: [current executable dir] + [VM_EXECUTABLE]
	if (strVmExecutableDir.isEmpty())
	{
		strVmExecutableDir = (QCoreApplication::instance())->applicationDirPath();
	}

	return strVmExecutableDir + (bX64 ? VM_EXECUTABLE64 : VM_EXECUTABLE);
}

QString ParallelsDirs::getVmStarterPath()
{
	QString strVmExecutableDir;

	strVmExecutableDir = (QCoreApplication::instance())->applicationDirPath();

	return strVmExecutableDir + VM_STARTER_EXECUTABLE;
}


QString ParallelsDirs::getConvertToolPath( const QDir& baseDir )
{
#if defined _WIN_
	QString sConvertToolPath =
		ParallelsDirs::getProductInstallDir( VER_REG_TREE_ROOT_STR )
			+ QString("Application\\%1").arg(CONVERT_TOOL_EXECUTABLE);
	if ( !QFile::exists( sConvertToolPath ) )//Might we have a deal with developer's build?
		sConvertToolPath = baseDir.absoluteFilePath( CONVERT_TOOL_EXECUTABLE );
	return sConvertToolPath;
#elif defined _LIN_
	QString sConvertToolPath = CONVERT_TOOL_EXECUTABLE;
	if ( !QFile::exists( sConvertToolPath ) )//Might we have a deal with developer's build?
		sConvertToolPath = QFileInfo(sConvertToolPath).fileName();
	return baseDir.absoluteFilePath( sConvertToolPath );
#endif
}

QString ParallelsDirs::getDiskToolPath( const QDir& baseDir )
{
#if _LIN_
	QString sDiskToolPath = DISK_TOOL_EXECUTABLE;
	if ( !QFile::exists( sDiskToolPath ) )//Might we have a deal with developer's build?
		sDiskToolPath = QFileInfo(sDiskToolPath).fileName();
	return baseDir.absoluteFilePath( sDiskToolPath );
#else//_WIN_
	QString sDiskToolPath = ParallelsDirs::getProductInstallDir( VER_REG_TREE_ROOT_STR ) + QString("Application\\%1").arg( DISK_TOOL_EXECUTABLE );
	if ( !QFile::exists( sDiskToolPath ) )//Might we have a deal with developer's build?
		sDiskToolPath = baseDir.absoluteFilePath( DISK_TOOL_EXECUTABLE );
	return sDiskToolPath;
#endif//_WIN_
}

QString ParallelsDirs::get7zipUtilityPath( const QDir& baseDir )
{
#ifdef _LIN_
	return baseDir.absoluteFilePath( ARCH_7_ZIP_EXECUTABLE );
#else//_WIN_
	QString s7zipUtilityPath = ParallelsDirs::getProductInstallDir( VER_REG_TREE_ROOT_STR ) + QString("Application\\%1").arg( ARCH_7_ZIP_EXECUTABLE );
	if ( !QFile::exists( s7zipUtilityPath ) )//Might we have a deal with developer's build?
		s7zipUtilityPath = baseDir.absoluteFilePath( ARCH_7_ZIP_EXECUTABLE );
	return s7zipUtilityPath;
#endif//_WIN_
}

QString ParallelsDirs::getMkisoUtilityPath( const QDir& baseDir )
{
#ifdef _LIN_
	return baseDir.absoluteFilePath( MKISO_EXECUTABLE );
#else//_WIN_
	QString sMkisoUtilityPath = ParallelsDirs::getProductInstallDir( VER_REG_TREE_ROOT_STR ) + QString("Application\\%1").arg( MKISO_EXECUTABLE );
	if ( !QFile::exists( sMkisoUtilityPath ) )//Might we have a deal with developer's build?
		sMkisoUtilityPath = baseDir.absoluteFilePath( MKISO_EXECUTABLE );
	return sMkisoUtilityPath;
#endif//_WIN_
}


QString ParallelsDirs::getDesktopControlAgentPath( const QString& baseDir )
{
#if defined(_WIN_)

#ifdef _32BIT_
	if (osInfo_getArchitecture() == OSINFO_ARCHITECTURE_64_BIT)
	{
		QString agentPath(baseDir + QDir::separator() + DESKCTL_AGENT_EXECUTABLE64);
		if (QFile::exists(agentPath))
			return agentPath;
	}
#endif

	return baseDir + QDir::separator() + DESKCTL_AGENT_EXECUTABLE;

#else
	Q_UNUSED(baseDir);
	return QString();
#endif
}


QString ParallelsDirs::getDispatcherPluginsPath()
{
	static const QString sDefaultDir("");
	return sDefaultDir;
}

QString ParallelsDirs::getVmScriptsDir(const QString &sBaseDir)
{
	PRL_ASSERT( !sBaseDir.isEmpty() );
	if( sBaseDir.isEmpty() )
		return "";
	return sBaseDir + "/scripts";
}

QString ParallelsDirs::getVmActionScriptPath(const QString &sBaseDir, PRL_VM_ACTION nAction)
{
	PRL_ASSERT( !sBaseDir.isEmpty() );
	if( sBaseDir.isEmpty() )
		return "";

	switch (nAction)
	{
	case PVA_PRESTART:
		return getVmScriptsDir(sBaseDir) + "/prestart";
	case PVA_POSTSTART:
		return getVmScriptsDir(sBaseDir) + "/poststart";
	case PVA_PRESTOP:
		return getVmScriptsDir(sBaseDir) + "/prestop";
	case PVA_POSTSTOP:
		return getVmScriptsDir(sBaseDir) + "/poststop";
	}
	return QString();
}

QString ParallelsDirs::getVmConfigurationSamplePath(const QString &sName)
{
	QString sPath = QString( "%1/samples/%2.pvs" ).
		arg( getDispatcherConfigDir() ).
		arg( sName );

	return sPath;
}

bool ParallelsDirs::isMultilanguagesBuild()
{
	return false;
}

#ifdef _WIN_
QString ParallelsDirs::getProcDumpToolPath(bool bX64)
{
	QDir baseDir(QCoreApplication::applicationDirPath());

#ifdef _64BIT_
	bool bSameBitness = bX64;
#else
	bool bSameBitness = !bX64;
#endif

	if (bSameBitness)
		return baseDir.absoluteFilePath(PRLPROCDUMP_EXECUTABLE);

	if (isDevelopersBuild())
		return QString();

#ifdef _64BIT_
	baseDir.cdUp(); // up from "amd64" to main app dir
#endif

	if (!bX64)
		return baseDir.absoluteFilePath(PRLPROCDUMP_EXECUTABLE);
	else
		return baseDir.absoluteFilePath(PRLPROCDUMP_EXECUTABLE64);
}
#endif

QString ParallelsDirs::getServiceAppName()
{
	return DISPATCHER_SERVICE_COMMON_NAME;
}

#ifndef _WIN_
QString ParallelsDirs::getClientLogDirPath( const QString & strUserHome )
{
	return QString("%1/.parallels").arg(strUserHome);
}
#endif

