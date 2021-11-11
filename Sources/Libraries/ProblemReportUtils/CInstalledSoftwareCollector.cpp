/*
 * CInstalledSoftwareCollector.cpp: Installed software collector
 * declaration.
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
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


#include "CInstalledSoftwareCollector.h"

#include <QProcess>
#include <QThread>
#include <QWeakPointer>
#include <QTimer>
#include <QString>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QFuture>
#include "Build/Current.ver"
#include "CProblemReportUtils.h"
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/Interfaces/VirtuozzoQt.h>
#include <prlcommon/PrlCommonUtilsBase/StringUtils.h>

class CInstalledSoftwareCollectorPrivate : public QObject
{
	Q_OBJECT

public:
	CInstalledSoftwareCollectorPrivate( CInstalledSoftwareCollector* );
	~CInstalledSoftwareCollectorPrivate();

	bool startCollecting();
	bool isRunning() const { return m_isRunning; }

public slots:
	void onInstalledSoftwareCollected();
	void cancel();

#ifdef _LIN_
	void onShellCommandProcessFinished(int,QProcess::ExitStatus);
#endif//_LIN_
#ifdef _WIN_
	void onConcurrentThreadFinished();
#endif//_WIN_

signals:
	void finished( const QString& );

public:
	CInstalledSoftwareCollector*	m_p;
	QString							m_result;
	volatile bool					m_cancel;
	bool							m_deleteOnComplete;
	bool							m_isRunning;
#ifdef _LIN_
	QSharedPointer<QProcess>			m_shelProcess;
#endif//_LIN_
#ifdef _WIN_
	QFutureWatcher<QString>			m_appListSearchWatcher;
#endif//_WIN_
};

///////////////////////////////////////////////////////////////////////////////
/// Win specific
//////////////////////////////////////////////////////////////////////////
#ifdef _WIN_
#include "windows.h"

#ifndef PRL_WIN_UNINSTALL_REG_PROGRAMS_PATH
#define PRL_WIN_UNINSTALL_REG_PROGRAMS_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#endif

#ifndef PRL_WIN_UNINSTALL_REG_PROGRAMS_PATH_64
#define PRL_WIN_UNINSTALL_REG_PROGRAMS_PATH_64 "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#endif

#ifndef PRL_WIN_UNINSTALL_REG_PROGRAMS_NAME_KEY
#define PRL_WIN_UNINSTALL_REG_PROGRAMS_NAME_KEY "DisplayName"
#endif

#ifndef PRL_WIN_UNINSTALL_REG_PROGRAMS_VERSION_KEY
#define PRL_WIN_UNINSTALL_REG_PROGRAMS_VERSION_KEY "DisplayVersion"
#endif

static QString getInstalledSoftwareListWin( const QString & strKey, volatile bool* cancel )
{
	// search in win registry
	QString strResult;
	LONG res;
	HKEY hKeyUninstall;

	res = RegOpenKeyExW (HKEY_LOCAL_MACHINE,
		strKey.utf16(), 0, KEY_READ,  &hKeyUninstall);

	if ( res != ERROR_SUCCESS )
	{
		WRITE_TRACE(DBG_FATAL, "Failed to open registry key '%s'",
			QSTR2UTF8(strKey) );
		return strResult;
	}

	WCHAR    szKey[1024];   // buffer for subkey name
	DWORD    dwName;                   // size of name string
	WCHAR    szClass[MAX_PATH] = TEXT("");  // buffer for class name
	DWORD    dwClassName = MAX_PATH;  // size of class string
	DWORD    dwSubKeys=0;               // number of subkeys
	DWORD    dwMaxSubKey;              // longest subkey size
	DWORD    dwMaxClass;              // longest class string
	DWORD    dwValues;              // number of values for key
	DWORD    dwMaxValue;          // longest value name
	DWORD    dwMaxValueData;       // longest value data
	DWORD    dwSecurityDescriptor; // size of security descriptor
	FILETIME ftLastWriteTime;      // last write time

	DWORD i;

	WCHAR  szValue[8192];

	// Get the class name and the value count.
	res = RegQueryInfoKey(
		hKeyUninstall,                    // key handle
		szClass,                // buffer for class name
		&dwClassName,           // size of class string
		NULL,                    // reserved
		&dwSubKeys,               // number of subkeys
		&dwMaxSubKey,            // longest subkey size
		&dwMaxClass,            // longest class string
		&dwValues,                // number of values for this key
		&dwMaxValue,            // longest value name
		&dwMaxValueData,         // longest value data
		&dwSecurityDescriptor,   // security descriptor
		&ftLastWriteTime);       // last write time

	// Enumerate the subkeys, until RegEnumKeyEx fails.

	if (dwSubKeys)
	{
		for ( i = 0 ; i < dwSubKeys ; i++ )
		{
			if ( *cancel )
			{
				WRITE_TRACE( DBG_INFO, "App list collection was canceled" );
				return strResult;
			}

			dwName = 8192;
			res = RegEnumKeyExW(hKeyUninstall, i,
				szKey,
				&dwName,
				NULL,
				NULL,
				NULL,
				&ftLastWriteTime);

			if ( res != ERROR_SUCCESS )
				continue;

			// open subkey
			HKEY hSubKey;
			res = RegOpenKeyExW( hKeyUninstall,
				szKey,
				0,
				KEY_READ,
				&hSubKey);

			if ( res != ERROR_SUCCESS )
				continue;
			DWORD dwKeyType;
			QString strkey = PRL_WIN_UNINSTALL_REG_PROGRAMS_NAME_KEY;
			dwName = 8192;
			res = RegQueryValueExW (hSubKey,
				strkey.utf16(),
				NULL,
				&dwKeyType,
				(LPBYTE)szValue,
				&dwName);

			QString strValue = UTF16_2QSTR(szValue);

			if ( strValue.isEmpty() || res != ERROR_SUCCESS )
			{
				RegCloseKey ( hSubKey );
				continue;
			}
			else
				strResult += strValue;

			strkey = PRL_WIN_UNINSTALL_REG_PROGRAMS_VERSION_KEY;
			dwName = 8192;
			res = RegQueryValueExW (hSubKey,
				strkey.utf16(),
				NULL,
				&dwKeyType,
				(LPBYTE)szValue,
				&dwName);

			strValue = UTF16_2QSTR(szValue);
			if ( strValue.isEmpty() || res != ERROR_SUCCESS )
			{
				strResult += QString("\n");
			}
			else
				strResult += QString(" ") + strValue + QString("\n");

			RegCloseKey ( hSubKey );
		}
	}

	RegCloseKey (hKeyUninstall);
	return strResult;

}

static QString collectWinPrograms( volatile bool* cancel )
{
	QString programs = getInstalledSoftwareListWin( PRL_WIN_UNINSTALL_REG_PROGRAMS_PATH, cancel );
	programs += getInstalledSoftwareListWin( PRL_WIN_UNINSTALL_REG_PROGRAMS_PATH_64, cancel );
	return programs;
}
#endif //_WIN_

///////////////////////////////////////////////////////////////////////////////
/// Lin specific
//////////////////////////////////////////////////////////////////////////

#ifdef _LIN_
//dpkg --get-selections - for debian linux
//RPM based distributions (RHEL, Fedora Core, Cent OS, Suse Linux etc) rpm -qa
// FIX ME Add here shell commands for other linux distributives
const QStringList g_LIST_SHELL_COMMANDS_GET_INSTALLED_LIST = ( QStringList() << "rpm -qa" << "dpkg --get-selections" );
static QString getInstalledSoftwareListLin()
{
	QString strOut;
	foreach( QString strCommand, g_LIST_SHELL_COMMANDS_GET_INSTALLED_LIST )
	{
		if( CProblemReportUtils::executeShellCommandSync( strCommand, strOut ) )
			return strOut;
	}
	return QString();
}

static QProcess* executeShellCommandASync( const QString& command, QObject* parent )
{
	QProcess* process = new QProcess( parent );
	process->start( command, QStringList{} );
	if ( ! process->waitForStarted() )
		return NULL;

	return process;
}

#endif//_LIN_


CInstalledSoftwareCollectorPrivate::CInstalledSoftwareCollectorPrivate( CInstalledSoftwareCollector* owner )
: m_p( owner )
, m_cancel( false )
, m_deleteOnComplete( false )
, m_isRunning( false )
{
	// timeout 3 minute
	QTimer::singleShot( 3 * 60 * 1000, this, SLOT(cancel()) );
	bool connected = connect( this, SIGNAL(finished(const QString&)),
		m_p, SIGNAL(collected(const QString&)), Qt::QueuedConnection );
	PRL_ASSERT( connected );
	Q_UNUSED( connected );
}

CInstalledSoftwareCollectorPrivate::~CInstalledSoftwareCollectorPrivate()
{}

void CInstalledSoftwareCollectorPrivate::cancel()
{
	m_cancel = true;
#ifdef _LIN_
	if ( m_shelProcess.data() && m_shelProcess.data()->state() != QProcess::NotRunning )
	{
		m_shelProcess.data()->kill();
		m_shelProcess.data()->waitForFinished();
	}
#endif//_LIN_
#ifdef _WIN_
	m_appListSearchWatcher.waitForFinished();
#endif//WIN_
}


bool CInstalledSoftwareCollectorPrivate::startCollecting()
{
#ifdef _WIN_
	// sync variant - used volatile m_cancel as parameter
	m_result = collectWinPrograms( &m_cancel );
	QFuture<QString> future = QtConcurrent::run( collectWinPrograms, &m_cancel );
	bool connected = connect( &m_appListSearchWatcher, SIGNAL(finished()),
							this, SLOT(onConcurrentThreadFinished()), Qt::UniqueConnection );
	PRL_ASSERT( connected );
	m_appListSearchWatcher.setFuture( future );
	return connected;
#endif
#ifdef _LIN_
	// for first valid command from list
	for( const QString& command : g_LIST_SHELL_COMMANDS_GET_INSTALLED_LIST )
	{
		m_shelProcess = QSharedPointer<QProcess>{executeShellCommandASync( command, this )};
		if ( m_shelProcess.data() )
		{
			bool connected = connect( m_shelProcess.data(), SIGNAL(finished(int,QProcess::ExitStatus)),
					this, SLOT(onShellCommandProcessFinished(int,QProcess::ExitStatus)) );
			PRL_ASSERT( connected );
			return connected;
		}
	}
	return false;
#endif//_LIN_
}

#ifdef _LIN_
void CInstalledSoftwareCollectorPrivate::onShellCommandProcessFinished( int, QProcess::ExitStatus status )
{
	PRL_ASSERT( m_shelProcess.data() );
	if ( status == QProcess::NormalExit && m_shelProcess.data() )
		m_result = UTF8_2QSTR( m_shelProcess.data()->readAllStandardOutput() );

	onInstalledSoftwareCollected();
}
#endif//_LIN_

#ifdef _WIN_
void CInstalledSoftwareCollectorPrivate::onConcurrentThreadFinished()
{
	m_result = m_appListSearchWatcher.result();
	onInstalledSoftwareCollected();
}
#endif//_WIN_

void CInstalledSoftwareCollectorPrivate::onInstalledSoftwareCollected()
{
	m_isRunning = false;
	emit finished( m_result );
}

CInstalledSoftwareCollector::CInstalledSoftwareCollector( QObject* parent )
: QObject( parent )
, m_d( new CInstalledSoftwareCollectorPrivate( this ) )
{

}
CInstalledSoftwareCollector::~CInstalledSoftwareCollector()
{

}

QString CInstalledSoftwareCollector::getInstalledSoftwareListSync()
{
#ifdef _WIN_
	volatile bool cansel = false;
	return collectWinPrograms( &cansel );
#endif
#ifdef _LIN_
	return getInstalledSoftwareListLin();
#endif//_LIN_
}

bool CInstalledSoftwareCollector::start()
{
	if ( m_d->isRunning() )
		return false;

	if ( m_d->m_deleteOnComplete )
	{
		bool connected = connect( m_d.data(), SIGNAL(finished(const QString&)), this, SLOT(deleteLater()) );
		PRL_ASSERT( connected );
		Q_UNUSED( connected );
	}

	m_d->m_isRunning = m_d->startCollecting();
	return m_d->m_isRunning;
}

void CInstalledSoftwareCollector::cancel()
{
	m_d->cancel();
}

bool CInstalledSoftwareCollector::isRunning() const
{
	return m_d->isRunning();
}

void CInstalledSoftwareCollector::setDeletionOnComplete( bool value )
{
	m_d->m_deleteOnComplete = value;
}

#include "CInstalledSoftwareCollector.moc"
