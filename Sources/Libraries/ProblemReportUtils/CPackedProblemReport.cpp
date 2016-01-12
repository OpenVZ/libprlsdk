/*
 * CPackedProblemReport.cpp: wrapper on problem report archived
 * model
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


#include "External/libtar/libtar/compat/config.h"
#include "External/libtar/libtar/lib/libtar.h"
#include <prlcommon/Interfaces/ParallelsTypes.h>
#include <prlcommon/Std/PrlAssert.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/param.h>

#ifdef STDC_HEADERS
# include <string.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_LIBZ
#include "External/zlib/zlib.h"
#endif

#include "External/libtar/libtar/compat/compat.h"

#ifdef _WIN_
	#include <io.h>
	#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
	#define S_ISREG(m) (((m) & (_S_IFMT)) == (_S_IFREG))
	#define fchmod(f, m) (0)
	#define snprintf(s, n, f, ...) _snprintf((s), (n), (f), __VA_ARGS__)
#ifndef strdup
	#define strdup					_strdup
#endif

#endif

#include <QByteArray>
#include <QFile>
#include <QString>
#include <QSysInfo>

#include "CPackedProblemReport.h"

#include "CProblemReportUtils.h"
#include <prlcommon/PrlCommonUtilsBase/ParallelsDirs.h>
#include <prlcommon/PrlCommonUtilsBase/CSimpleFileHelper.h>

#ifndef PRL_REPORT_READ_FILE_BUFFER_SIZE
#define PRL_REPORT_READ_FILE_BUFFER_SIZE 1024*1024
#endif

const int g_maxSizeToReadFromLog = 16*1024*1024;

static tarfd_t gzopen_frontend(const char *pathname, int oflags, int mode)
{
	(void)mode;
	const char *gzoflags = 0;
	gzFile gzf = 0;

#ifndef _WIN_
	int fd = -1;
#endif

	switch (oflags & O_ACCMODE)
	{
	case O_WRONLY:
		gzoflags = "wb";
		break;
	case O_RDONLY:
		gzoflags = "rb";
		break;
	default:
	case O_RDWR:
		errno = EINVAL;
		return TAR_INVALID_FD;
	}

#ifndef _WIN_
	fd = open(pathname, oflags, mode);
	if (fd == -1)
		return TAR_INVALID_FD;

	if ((oflags & O_CREAT) && fchmod(fd, mode))
		return TAR_INVALID_FD;

	gzf = gzdopen(fd, gzoflags);
#else
	gzf = gzopen(pathname, gzoflags);
#endif
	if (!gzf)
	{
		errno = ENOMEM;
		return TAR_INVALID_FD;
	}

	return (tarfd_t)gzf;
}

static int gzclose_frontend(tarfd_t fd)
{
	return gzclose((gzFile)fd);
}

static ssize_t gzread_frontend(tarfd_t fd, void * buf, size_t len)
{
	// NOTE : expected 'len' maximum value is T_BLOCKSIZE, so truncate unsigned is safe
	return gzread((gzFile)fd, buf, (unsigned)len);
}

static ssize_t gzwrite_frontend(tarfd_t fd, const void * buf, size_t len)
{
	// NOTE : expected 'len' maximum value is T_BLOCKSIZE, so truncate unsigned is safe
	return gzwrite((gzFile)fd, buf, (unsigned)len);
}

static tartype_t gztype = { gzopen_frontend, gzclose_frontend, gzread_frontend, gzwrite_frontend };

PRL_RESULT CPackedProblemReport::createInstance( CPackedProblemReport::packedReportSide side ,
								 CPackedProblemReport ** ppReport,
								 const QByteArray & data,
								 const QString & strTempDirPath )
{
	QString strBaseName = QFileInfo(strTempDirPath).fileName();
	QString strPathToSave = getPathToSave( side, strBaseName );
	CPackedProblemReport * pReport =
		new CPackedProblemReport( strPathToSave, data );

	if ( pReport->isValid() )
	{
		*ppReport = pReport;
		pReport->setPackedReportSide( side );
	}
	else
	{
		delete pReport;
		return PRL_ERR_FAILURE;
	}

	return PRL_ERR_SUCCESS;
}

PRL_RESULT CPackedProblemReport::createInstance( CPackedProblemReport::packedReportSide side ,
								 CPackedProblemReport ** ppReport,
								 const QString & strPathToTempDir )
{
	QString strPathToSave = getPathToSave( side );

	CPackedProblemReport * pReport =
		new CPackedProblemReport( strPathToSave, strPathToTempDir );

	if ( pReport->isValid() )
	{
		*ppReport = pReport;
		pReport->setPackedReportSide( side );
	}
	else
	{
		delete pReport;
		return PRL_ERR_FAILURE;
	}

	return PRL_ERR_SUCCESS;
}

CPackedProblemReport::~CPackedProblemReport()
{
	if ( m_bCleanupTempDir )
	{
		bool bRes = CSimpleFileHelper::ClearAndDeleteDir( m_strTempDirPath );
		if( !bRes )
			WRITE_TRACE(DBG_INFO, "temp report directory %s was not deleted",
								QSTR2UTF8(m_strTempDirPath));
	}

	switch( m_Side )
	{
		case ClientSide:
		case ReportToolSide:
		{
			bool bRes = QFile::remove( m_strArchPath );
			if( !bRes )
				WRITE_TRACE(DBG_FATAL, "archive path %s was not deleted",
								QSTR2UTF8(m_strArchPath));
		}
		default:
			break;
	}
}

CPackedProblemReport::CPackedProblemReport( const QString & strPathToSave ,
										   const QString & strPathToTempDir ):
m_bValid( true ),
m_bCleanupTempDir( false ),
m_strArchPath( strPathToSave ),
m_bQuitFromPack( false )
{
	m_strTempDirPath = strPathToTempDir;
	if ( !QFile::exists( strPathToTempDir ) )
	{
		WRITE_TRACE(DBG_FATAL, "temp dir for tar files not exists" );
		m_bValid = false;
	}
	if( loadMainXml() != PRL_ERR_SUCCESS )
	{
		WRITE_TRACE(DBG_FATAL, "failed load main xml descriptor!" );
		m_bValid = false;
	}
}

CPackedProblemReport::CPackedProblemReport(  const QString & strPathToSave,
											const QByteArray & data ):
m_bValid( true ),
m_bCleanupTempDir( false ),
m_strArchPath( strPathToSave ),
m_bQuitFromPack( false )
{
	try
	{
		if ( !data.isEmpty() )
		{
			QFile file( m_strArchPath );
			QFile::remove( m_strArchPath );

			// save new tar file from buffer
			if( !file.open( QIODevice::WriteOnly ) )
			{
				WRITE_TRACE(DBG_FATAL, "Cannot Open Or Create Tar Archive '%s'", QSTR2UTF8(strPathToSave) );
				throw -1;
			}
			if( file.write( data ) != data.size() )
			{
				WRITE_TRACE(DBG_FATAL, "Cannot write Tar Archive from data" );
				throw -1;
			}
		}
		else
		{
			WRITE_TRACE(DBG_INFO, "empty packed problem report creation" );
		}

		m_strTempDirPath = getTempStorageDirPath( m_strArchPath );
		if( QFile::exists( m_strTempDirPath ) )
		{
			WRITE_TRACE(DBG_INFO, "strange! temp directory %s already exists, clear it.",
						QSTR2UTF8(m_strTempDirPath) );
			if( !CSimpleFileHelper::ClearAndDeleteDir( m_strTempDirPath ) )
			{
				WRITE_TRACE(DBG_FATAL, "cannot remove temp directory");
				throw -1;
			}
		}
		// clear and create new temp dir
		if( !QFile::exists( m_strArchPath ) )
		{
			if( !QDir().mkdir( m_strTempDirPath ) )
			{
				WRITE_TRACE(DBG_FATAL, "Cannot create temp dir for tar files" );
				throw -1;
			}
		}
		else
		// check that path to save is existing file - unpack it contents to temp dir at any case
		{
			// extract archive data to temporal directory
			QFileInfo info( m_strTempDirPath );
			if( extractArchive( info.dir().absolutePath() ) != PRL_ERR_SUCCESS )
				throw -1;
		}

	}
	catch (...)
	{
		m_bValid = false;
	}

}

PRL_RESULT CPackedProblemReport::extractArchive( const QString & strPath )
{
	TAR *t;

	WRITE_TRACE(DBG_INFO, "Extract incoming archive %s to directoty %s",
					QSTR2UTF8( m_strArchPath ),
					QSTR2UTF8( strPath ) );
	if ( tar_open(&t, m_strArchPath.toUtf8().data(), &gztype,O_RDONLY, 0, 0 ) == -1 )
	{
		WRITE_TRACE(DBG_FATAL, "tar_open(): %s\n", strerror(errno));
		return PRL_ERR_FAILURE;
	}

	if ( tar_extract_all(t, strPath.toUtf8().data() ) != 0)
	{
		WRITE_TRACE(DBG_FATAL, "tar_extract_all(): %s\n", strerror(errno));
		tar_close(t);
		return PRL_ERR_FAILURE;
	}

	if (tar_close(t) != 0)
	{
		WRITE_TRACE(DBG_FATAL, "tar_close(): %s\n", strerror(errno));
		return PRL_ERR_FAILURE;
	}

	// search main xml Descriptor in unpacked archive and deserialize it to
	// CPackedProblemReport object
	if ( PRL_FAILED( loadMainXml() ) )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot find main xml deskriptor" );
		return PRL_ERR_FAILURE;
	}
	return PRL_ERR_SUCCESS;
}

QString CPackedProblemReport::getTempStorageDirPath( const QString & strPathToSave )
{
	QString strTemp = strPathToSave;
	return strTemp.remove(".tar.gz");
}

QString CPackedProblemReport::getPathToSave( packedReportSide side, const QString & strBaseName )
{
	QString strPath;
	switch( side )
	{
	case DispSide:
#ifdef _WIN_
		//#PWE-5049
		// we need first time create and setup permissions on win xp user based report storage directory
		// Because only admins and power users can write here by default
		if (QSysInfo::windowsVersion() == QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003)
		{
			QString strTemp =
				QFileInfo( ParallelsDirs::getCommonDocumentsDirectory() ).absoluteDir().absolutePath();
			strTemp = CProblemReportUtils::GetProblemReportPath( strTemp );
			if( ! strTemp.isEmpty() )
			{
				CAuth::AccessMode backupOwnerPerm = 0, backupOthersPerm = 0;
				CAuthHelper auth;
				auth.AuthUserBySelfProcessOwner();
				bool bMixed = false;
				PRL_RESULT res = CFileHelper::GetSimplePermissionsToFile( strTemp, backupOwnerPerm,
																			backupOthersPerm, bMixed, auth );
				if( PRL_SUCCEEDED( res ) &&
					backupOthersPerm != ( CAuth::fileMayRead | CAuth::fileMayWrite | CAuth::fileMayExecute ) )
				{
					bool ret = auth.AddAccessRightsForEveryone( strTemp );
					if( ! ret )
						WRITE_TRACE( DBG_FATAL, "cannot set simple permittions to %s!", QSTR2UTF8( strTemp ) );

				}
			}
		}
#endif //_WIN_
	case VmSide:
		{
			strPath = ParallelsDirs::getCommonDefaultVmCatalogue();
			// GetPathDorSelfSave returns empty string if path is not exist/not available
			strPath = CProblemReportUtils::GetPathForSelfSave( strPath );
		}

		// In case of Cloud Storage, VmCatalogue directory could be unaccesible
		// Fallback to system temporary directory
		if( strPath.isEmpty() )
			strPath = CProblemReportUtils::GetPathForSelfSave(ParallelsDirs::getSystemTempDir());

		break;
	case ClientSide:
	case ReportToolSide:
#ifdef _WIN_
		// FIX ME it temporal fix - the reason- on localized win this path contains unicode symbols
		// but libtar cannot works with unicode :(
		strPath = QFileInfo( ParallelsDirs::getCommonDocumentsDirectory() ).absoluteDir().absolutePath();
		strPath = CProblemReportUtils::GetPathForSelfSave( strPath );
#else
		strPath = CProblemReportUtils::GetPathForSelfSave( ParallelsDirs::getSystemTempDir() );
#endif
		break;

	default:
		break;
	}
	// remove .xml
	strPath.truncate( strPath.size() - 4 );
	if ( !strBaseName.isEmpty() )
		strPath.replace(QFileInfo(strPath).fileName(), strBaseName );

	strPath += ".tar.gz";
	return strPath;
}

void CPackedProblemReport::appendCrashDump(CRepCrashDump * pDump)
{
	QString strPath = pDump->getPath();
	if ( !QFile::exists( strPath ) )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot find file pass to append" );
		return;
	}
	pDump->setNameInArchive( QString("CrashDump%1").arg(m_lstCrashDumps.size()));
	QString strPathInTemp = m_strTempDirPath + QString("/") + pDump->getNameInArchive();
	if( !QFile::copy( strPath, strPathInTemp ) )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot copy file to temp dir" );
		return;
	}
	pDump->setDump();
	CProblemReport::appendCrashDump( pDump );
}

void CPackedProblemReport::appendMemoryDump(CRepMemoryDump * pDump)
{
	QString strPath = pDump->getPath();
	if ( !QFile::exists( strPath ) )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot find memory dump '%s' to append", QSTR2UTF8(pDump->getPath()) );
		return;
	}
	pDump->setNameInArchive( QString("MemoryDump%1").arg(m_lstMemoryDumps.size()));
	QString strPathInTemp = m_strTempDirPath + QString("/") + pDump->getNameInArchive();
	if( !QFile::copy( strPath, strPathInTemp ) )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot copy file '%s' to temp dir '%s'", QSTR2UTF8(strPath), QSTR2UTF8(strPathInTemp) );
		return;
	}
	pDump->setDump();
	CProblemReport::appendMemoryDump( pDump );
}

void CPackedProblemReport::appendSystemLog( const QString& strPathFrom,
										   const QString& strCustomName )
{
	if ( strPathFrom.isEmpty() )
	{
		WRITE_TRACE(DBG_FATAL, "cannot append from from empty path!" );
		return;
	}

	// No file - nothing to add
	if( !QFile::exists( strPathFrom) )
		return;

	QString strPathInTemp = m_strTempDirPath + QString("/") + strCustomName;

	// if file already exists overwrite it only, do not add data to log list
	bool bExist = QFile::exists( strPathInTemp );
	if( bExist )
		QFile::remove( strPathInTemp );

	QFile fileForRead( strPathFrom );
	if( !fileForRead.open( QIODevice::ReadOnly ) )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot open file to read log file" );
		return;
	}

	if( fileForRead.size() > g_maxSizeToReadFromLog )
		fileForRead.seek( fileForRead.size() - g_maxSizeToReadFromLog );

	QFile fileForWrite( strPathInTemp );
	if( !fileForWrite.open( QIODevice::WriteOnly ) )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot open or create file to temp dir" );
		fileForRead.close();
		return;
	}

	// get last read size
	int uiLastReadSize = g_maxSizeToReadFromLog % PRL_REPORT_READ_FILE_BUFFER_SIZE;
	int uiReadedSize = 0;
	QByteArray buffer;
	while ( 1 )
	{
		buffer = fileForRead.read( PRL_REPORT_READ_FILE_BUFFER_SIZE );
		if( buffer.size() == 0 )
			break;

		uiReadedSize += buffer.size();

		if( uiReadedSize > g_maxSizeToReadFromLog )
		{
			fileForWrite.write( buffer.data(), ( buffer.size() < uiLastReadSize ) ? buffer.size() : uiLastReadSize );
			break;
		}
		else
			fileForWrite.write( buffer );
	}

	fileForRead.close();
	fileForWrite.close();

	// set CRepSystemLog to xml model
	if( ! bExist )
	{
		CRepSystemLog * pLog = new CRepSystemLog();
		pLog->setName( strCustomName );
		pLog->setData();
		CProblemReport::appendSystemLog( pLog );
	}
}

void CPackedProblemReport::appendSystemLog( CRepSystemLog * plog )
{
	QString strPathInTemp = m_strTempDirPath + QString("/") + QFileInfo( plog->getName() ).fileName();
	if( !plog )
	{
		QFile::remove( strPathInTemp );
		return;
	}

	QString strValue = plog->getData();
	if ( strValue.isEmpty() )
	{
		WRITE_TRACE(DBG_FATAL, "cannot append element with value field!" );
		return;
	}

	QFile file( strPathInTemp );
	bool bExists = QFile::exists( strPathInTemp );
	if( !file.open( QIODevice::WriteOnly ) )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot open or create file to temp dir" );
		return;
	}

	if( file.write(QSTR2UTF8(strValue)) == 0 )
	{
		WRITE_TRACE(DBG_FATAL, "Cannot write from data to temp file" );
		file.close();
		return;
	}

	plog->setName( QFileInfo( plog->getName() ).fileName() );
	plog->setData();

	if( !bExists )
		CProblemReport::appendSystemLog( plog );
	else
	{
		delete plog;
		plog = NULL;
	}
}

void CPackedProblemReport::appendTemplateSystemLog( const QString& strPathFrom,
							const QString& strCustomName,
							int iLogsCount )
{
	if ( strPathFrom.isEmpty() )
	{
		WRITE_TRACE(DBG_FATAL, "cannot append from from empty path!" );
		return;
	}

	QFileInfo qLogFile(strPathFrom);
	QString strFileName = qLogFile.fileName();

	QDir qLogDir = qLogFile.absoluteDir();
	QStringList nameTemplate;
	nameTemplate << strFileName + "*";
	QStringList list = qLogDir.entryList(nameTemplate, QDir::Files, QDir::Time);

	if ( list.size() <= 1 )
	{
		this->appendSystemLog( strPathFrom, strCustomName );
		return;
	}

	for(int i = 0; i < qMin(iLogsCount, list.size()); i++)
	{
		//convert to full path
		QString fullFile = qLogDir.absolutePath() + "/" + list[i];

		//generate name of copied log file
		int postfixlen = list[i].length() - strFileName.length();
		PRL_ASSERT( postfixlen >= 0 );
		QString postfix = list[i].right( postfixlen );
		QString saveName = strCustomName + postfix;

		QRegExp rePostfix("\\.\\d+\\.gz");
		if (postfixlen > 0 && rePostfix.exactMatch(postfix))
		{
			// skip to add compressed log backup when it's size too big to
			// avoid compressed file truncation.
			if (QFileInfo(fullFile).size() >= g_maxSizeToReadFromLog)
				continue;
		}

		//report server don't show logs without ".log" extension
		if (postfixlen > 0 && saveName.length() > 4 && saveName.right(4) != QString(".log"))
		{	//we have name like parallels-system.log-XXX
			//convert to parallels-system-XXX.log
			if (strCustomName.length() > 4 && strCustomName.right(4) == QString(".log"))
				saveName = strCustomName.left(strCustomName.length()-4) + postfix + ".log";
			else
				saveName += ".log";
		}

		this->appendSystemLog( fullFile, saveName );
	}
}

void CPackedProblemReport::saveMainXml()
{
	QString strPathInTemp = m_strTempDirPath + QString("/") + PR_PACKED_REP_MAIN_XML;
	saveToFile( strPathInTemp )	;
}

int CPackedProblemReport::loadMainXml()
{
	QString strPathInTemp = m_strTempDirPath + QString("/") + PR_PACKED_REP_MAIN_XML;
	if( QFile::exists( strPathInTemp ) )
		return loadFromFile( strPathInTemp );
	else
		return PRL_ERR_FAILURE;
}

QString CPackedProblemReport::getTempDirPath() const
{
	return m_strTempDirPath;
}

QString CPackedProblemReport::getElementPathFromArchive( const QString & strValue )
{
	QString strPath;
	if( strValue.isEmpty() )
		return strPath;

	strPath = QFileInfo(m_strTempDirPath + QString("/") + strValue).absoluteFilePath();
	if ( QFile::exists( strPath ) )
		return strPath;
	else
		return QString();
}

QString CPackedProblemReport::getArchivePathFromTopLevelObject( CBaseNode * pObj,
													const QString& strNameInArchive )
{
	if( pObj && !strNameInArchive.isEmpty() )
	{
		QString strPath =
			QFileInfo(m_strTempDirPath + QString("/") + strNameInArchive).absoluteFilePath();

		if( QFile::exists( strPath ) )
			return strPath;
	}
	return QString();
}

QStringList CPackedProblemReport::createReportFilesList()
{
	QSet<QString> lstFiles;
	QString strPath;
	// get screenshot files
	if( getUserDefinedData() && getUserDefinedData()->getScreenShots() )
	foreach( CRepScreenShot * pScr, getUserDefinedData()->getScreenShots()->m_lstScreenShot )
		lstFiles += getArchivePathFromTopLevelObject( pScr, pScr->getName() );

	// get log files
	if( getSystemLogs() )
	foreach( CRepSystemLog * pLog, getSystemLogs()->m_lstSystemLog )
		lstFiles += getArchivePathFromTopLevelObject( pLog, pLog ? pLog->getName() : "" );

	// get crash dumps
	foreach( CRepCrashDump * pDump , m_lstCrashDumps )
		lstFiles += getArchivePathFromTopLevelObject( pDump, pDump ? pDump->getNameInArchive() : "" );

	//get memory dumps
	foreach( CRepMemoryDump * pDump , m_lstMemoryDumps )
		lstFiles += getArchivePathFromTopLevelObject( pDump, pDump ? pDump->getNameInArchive() : "" );

	// get vz report
	foreach ( CRepVzReport * pVzRep, m_lstVzReport)
		lstFiles += getArchivePathFromTopLevelObject( pVzRep, pVzRep ? pVzRep->getNameInArchive() : "" );

	lstFiles += getArchivePathFromTopLevelObject( getClientInfo(),
					getClientInfo() ? getClientInfo()->getNameInArchive() : "" );

	lstFiles += getArchivePathFromTopLevelObject( getAdvancedVmInfo(),
					getAdvancedVmInfo() ? getAdvancedVmInfo()->getNameInArchive() : "" );

	lstFiles += getElementPathFromArchive( getHostStatistic() );
	lstFiles += getElementPathFromArchive( getHostInfo() );
	lstFiles += getElementPathFromArchive( getMoreHostInfo() );
	lstFiles += getElementPathFromArchive( getAllProcesses() );
	lstFiles += getElementPathFromArchive( getAllProcessesSamples() );
	lstFiles += getElementPathFromArchive( getClientProxyInfo() );
	lstFiles += getElementPathFromArchive( getLoadedDrivers() );
	lstFiles += getElementPathFromArchive( getVmConfig() );
	lstFiles += getElementPathFromArchive(getVmDomain());
	lstFiles += getElementPathFromArchive( getPerformanceCounters() );
	lstFiles += getElementPathFromArchive( getMonitorData() );
	lstFiles += getElementPathFromArchive( getNetConfig() );
	lstFiles += getElementPathFromArchive( getAppConfig() );
	lstFiles += getElementPathFromArchive( getGuestOs() );
	lstFiles += getElementPathFromArchive( getVmDirectory() );
	lstFiles += getElementPathFromArchive( getInstalledSoftware() );
	lstFiles += getElementPathFromArchive( getAppSwitchPackages() );
	lstFiles += getElementPathFromArchive( getFilesMd5InProductBundle() );
	lstFiles += getElementPathFromArchive( getLaunchdInfo() );

	// get main xml
	lstFiles += QFileInfo(m_strTempDirPath + QString("/") + PR_PACKED_REP_MAIN_XML).absoluteFilePath();

	lstFiles.remove( QString() );
	return lstFiles.toList();
}

static int checkOnQuitCallback( void * pParam )
{
	CPackedProblemReport* pReport = dynamic_cast<CPackedProblemReport*>((CPackedProblemReport*)pParam);
	if ( pReport && pReport->isQuitFromPack() )
		return 1;

	return 0;
}

PRL_RESULT CPackedProblemReport::packReport()
{
	m_bQuitFromPack = false;
	saveMainXml();
	QStringList lstFiles = createReportFilesList();

	TAR *t = NULL;

	if ( tar_open( &t,
					m_strArchPath.toUtf8().data(),
					&gztype,
					O_WRONLY | O_CREAT,
					0644,
					0) == -1 )
	{
		WRITE_TRACE( DBG_FATAL, "tar_open(): %s\n", strerror(errno) );
		return PRL_ERR_FAILURE;
	}

	foreach( QString strPath, lstFiles )
	{
		QString strPathToSave = QFileInfo(m_strTempDirPath).fileName() + QString("/");
		strPathToSave += QFileInfo(strPath).fileName();
		if ( tar_append_tree( t,
								strPath.toUtf8().data(),
								strPathToSave.toUtf8().data(),
								checkOnQuitCallback, this ) != 0 )
		{
			WRITE_TRACE(DBG_FATAL, "tar_append_tree(\"%s\", \"%s\"): %s\n",
							QSTR2UTF8( m_strTempDirPath ),
							QSTR2UTF8( strPathToSave ),
							strerror(errno) );

			tar_close(t);
			if( m_bQuitFromPack )
			{
				m_bQuitFromPack = false;
				return PRL_ERR_OPERATION_WAS_CANCELED;
			}

			return PRL_ERR_FAILURE;
		}
	}

	if ( tar_append_eof( t ) != 0 )
	{
		WRITE_TRACE(DBG_FATAL, "tar_append_eof(): %s\n", strerror(errno) );
		tar_close(t);
		return PRL_ERR_FAILURE;
	}

	if (tar_close(t) != 0)
	{
		WRITE_TRACE(DBG_FATAL, "tar_close(): %s\n", strerror(errno) );
		return PRL_ERR_FAILURE;
	}

	m_bCleanupTempDir = true;
	m_bQuitFromPack = false;
	return PRL_ERR_SUCCESS;
}

QString CPackedProblemReport::getArchivePath() const
{
	return m_strArchPath;
}

SmartPtr<CProblemReport> CPackedProblemReport::convertToProblemReportOldFormat()
{
	SmartPtr<CProblemReport> pPR = SmartPtr<CProblemReport>(new CProblemReport);

	// Detach from current problem report object

	saveMainXml();
	QString strPathInTemp = m_strTempDirPath + QString("/") + PR_PACKED_REP_MAIN_XML;
	if( ! QFile::exists( strPathInTemp ) || PRL_FAILED(pPR->loadFromFile( strPathInTemp )) )
		pPR->fromString(toString());	// Bad way but it occurs rare

	CRepScreenShots * pScrs = pPR->getUserDefinedData()->getScreenShots();
	QByteArray data;
	if ( pScrs )
	{
		CRepScreenShot * pScr = NULL;
		foreach( pScr, pScrs->m_lstScreenShot )
		{
			QString strPathInTemp = m_strTempDirPath + QString("/") + pScr->getName();
			QFile cScreenShotFile(strPathInTemp);
			if(cScreenShotFile.open(QFile::ReadOnly))
				data = cScreenShotFile.readAll();
			data = data.toBase64();
			cScreenShotFile.close();

			pScr->setData( data );
			data.clear();
		}
	}

#define MAX_OLD_SIZE_TO_READ_FROM_LOGS 2*1024*1024

	CRepSystemLogs * pLogs = pPR->getSystemLogs();
	if ( pLogs )
	{
		CRepSystemLog * pLog = NULL;
		foreach( pLog, pLogs->m_lstSystemLog )
		{
			QString strPathInTemp = m_strTempDirPath + QString("/") + pLog->getName();
			CSimpleFileHelper::ReadfromFile( strPathInTemp,
										MAX_OLD_SIZE_TO_READ_FROM_LOGS,
										data );

			pLog->setData(UTF8_2QSTR(data));
			data.clear();
		}
	}

	foreach( CRepCrashDump * pDump , pPR->m_lstCrashDumps )
	{
		if ( pDump )
		{
			QString strDumpPath = m_strTempDirPath + QString("/") + pDump->getNameInArchive();
			QFile cDumpFile(strDumpPath);

			if( cDumpFile.open(QFile::ReadOnly) )
				data = cDumpFile.readAll();
			cDumpFile.close();

			pDump->setDump( data.toBase64() );
			data.clear();
		}
	}

	foreach( CRepMemoryDump * pDump , pPR->m_lstMemoryDumps )
	{
		if ( pDump )
		{
			QString strDumpPath = m_strTempDirPath + QString("/") + pDump->getNameInArchive();
			QFile cDumpFile(strDumpPath);

			if( cDumpFile.open(QFile::ReadOnly) )
				data = cDumpFile.readAll();
			cDumpFile.close();

			pDump->setDump( data.toBase64() );
			data.clear();
		}
	}

	if (pPR->getAdvancedVmInfo())
	{
		CRepAdvancedVmInfo* pAdvancedVmInfo = new CRepAdvancedVmInfo;
		pAdvancedVmInfo->fromString( getValue( pPR->getAdvancedVmInfo()->getNameInArchive() ) );
		pPR->setAdvancedVmInfo( pAdvancedVmInfo );
	}

	if (pPR->getClientInfo())
	{
		ClientInfo* pClientInfo = new ClientInfo;
		pClientInfo->fromString( getValue( pPR->getClientInfo()->getNameInArchive() ) );
		pPR->setClientInfo( pClientInfo );
	}

	pPR->setHostStatistic( getValue( pPR->getHostStatistic() ) );
	pPR->setHostInfo( getValue( pPR->getHostInfo() ) );
	pPR->setMoreHostInfo( getValue( pPR->getMoreHostInfo() ) );
	pPR->setAllProcesses( getValue( pPR->getAllProcesses() ) );
	pPR->setAllProcessesSamples( getValue( pPR->getAllProcessesSamples() ) );
	pPR->setClientProxyInfo( getValue( pPR->getClientProxyInfo() ) );
	pPR->setLoadedDrivers( getValue( pPR->getLoadedDrivers() ) );
	pPR->setAppConfig( getValue( pPR->getAppConfig() ) );
	pPR->setNetConfig( getValue( pPR->getNetConfig() ) );
	pPR->setMonitorData( getValue( pPR->getNetConfig() ) );
	pPR->setPerformanceCounters( getValue( pPR->getPerformanceCounters() ) );
	pPR->setVmConfig( getValue( pPR->getVmConfig() ) );
	pPR->setVmDomain(getValue(pPR->getVmDomain()));
	pPR->setGuestOs( getValue( pPR->getGuestOs() ) );
	pPR->setVmDirectory( getValue( pPR->getVmDirectory() ) );
	pPR->setFilesMd5InProductBundle( getValue( pPR->getFilesMd5InProductBundle() ) );
	pPR->setLaunchdInfo( getValue( pPR->getLaunchdInfo() ) );

	return pPR;
}

void CPackedProblemReport::clearSystemLogs()
{
	CRepSystemLog * pLog = NULL;
	foreach( pLog, getSystemLogs()->m_lstSystemLog )
	{
		QString strPathInTemp = m_strTempDirPath + QString("/") + pLog->getName();
		QFile::remove( strPathInTemp );
	}
	CProblemReport::clearSystemLogs();
}

void CPackedProblemReport::setValue( const QString & strValue,
									const QString & strNameInArchive ) const
{
	if( strValue.isEmpty() )
		return;

	if ( strValue == strNameInArchive )
		return;

	QString strArchPath = m_strTempDirPath + QString("/") + strNameInArchive;
	QFile file( strArchPath );
	if( file.open( QIODevice::WriteOnly) )
		file.write( strValue.toUtf8() );
	file.close();
}

QString CPackedProblemReport::getValue( const QString & strNameInArchive ) const
{
	QString strArchPath = m_strTempDirPath + QString("/") + strNameInArchive;
	QFile file( strArchPath );
	QByteArray data;
	if( file.open( QIODevice::ReadOnly ) )
		data = file.readAll();
	file.close();
	return UTF8_2QSTR( data );

}

QString CPackedProblemReport::getVmConfigFromArchive() const
{
	return getValue( PR_PACKED_REP_CURRENT_VM );
}

QString CPackedProblemReport::getVmDomainFromArchive() const
{
	return getValue(PR_PACKED_REP_CURRENT_VM_DOMAIN);
}

#define setValuePointerWithCaller( pointer, nameInArchive, functionName ) \
	if( !pointer )\
		CProblemReport::functionName( NULL );\
	else\
	{\
		QString strValue = pointer->toString();\
		setValue( strValue, nameInArchive );\
		pointer->ClearLists();\
		pointer->setDefaults();\
		pointer->setNameInArchive( nameInArchive );\
		CProblemReport::functionName( pointer );\
	}

CRepVzReport* CPackedProblemReport::getVzReport() const
{
	if( !m_lstVzReport.isEmpty() )
		return m_lstVzReport[0];
	else
		return NULL;
}

void CPackedProblemReport::setVzReport(CRepVzReport *pVzReport)
{
	if ( !pVzReport )
		CProblemReport::setVzReport( NULL );
	else
	{
		QString strValue = pVzReport->getReportString();
		setValue( strValue, PR_PACKED_REP_VZ_REPORT );
		pVzReport->ClearLists();
		pVzReport->setDefaults();
		pVzReport->setNameInArchive( PR_PACKED_REP_VZ_REPORT );
		CProblemReport::setVzReport( pVzReport );
	}
}

CRepAdvancedVmInfo* CPackedProblemReport::getAdvancedVmInfo() const
{
	if( !m_lstAdvancedVmInfo.isEmpty() )
		return m_lstAdvancedVmInfo[0];
	else
		return NULL;
}

void CPackedProblemReport::setAdvancedVmInfo(CRepAdvancedVmInfo* pAdvancedVmInfo)
{
	setValuePointerWithCaller( pAdvancedVmInfo, PR_PACKED_REP_ADVANCED_VM_INFO, setAdvancedVmInfo );
}

ClientInfo* CPackedProblemReport::getClientInfo() const
{
	if( !m_lstClientInfo.isEmpty() )
		return m_lstClientInfo[0];
	else
		return NULL;
}

void CPackedProblemReport::setClientInfo(ClientInfo* pClientInfo)
{
	setValuePointerWithCaller( pClientInfo, PR_PACKED_REP_CLIENT_INFO, setClientInfo );
}

#define setValueWithCaller( value, nameInArchive, functionName ) \
	setValue( value, nameInArchive );\
	if ( !value.isEmpty() )\
	CProblemReport::functionName(nameInArchive);\
	else\
		CProblemReport::functionName();\

void CPackedProblemReport::setHostStatistic(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_HOST_STAT, setHostStatistic );
}

void CPackedProblemReport::setHostInfo(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_HOST_INFO, setHostInfo );
}

void CPackedProblemReport::setMoreHostInfo(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_MORE_HOST_INFO, setMoreHostInfo );
}

void CPackedProblemReport::setAllProcesses(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_ALL_PROCESSES, setAllProcesses );
}

void CPackedProblemReport::setAllProcessesSamples(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_ALL_PROCESSES_SAMPLES, setAllProcessesSamples );
}

void CPackedProblemReport::setLoadedDrivers(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_ALL_LOADED_DRIVERS, setLoadedDrivers);
}

void CPackedProblemReport::setClientProxyInfo(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_CLIENT_PROXY_INFO, setClientProxyInfo );
}

void CPackedProblemReport::setAppConfig(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_APP_CONFIG, setAppConfig );
}

void CPackedProblemReport::setNetConfig(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_NET_CONFIG, setNetConfig );
}

void CPackedProblemReport::setMonitorData(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_MONITOR_DATA, setMonitorData );
}

void CPackedProblemReport::setPerformanceCounters(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_PERFORMANCE_COUNTERS, setPerformanceCounters );
}

void CPackedProblemReport::setVmConfig(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_CURRENT_VM, setVmConfig );
}

void CPackedProblemReport::setVmDomain(QString value)
{
	setValueWithCaller(value, PR_PACKED_REP_CURRENT_VM_DOMAIN, setVmDomain);
}

void CPackedProblemReport::setGuestOs(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_GUEST_OS, setGuestOs );
}

void CPackedProblemReport::setVmDirectory(QString value)
{
	setValueWithCaller( value, PR_PACKED_REP_VM_DIRECTORY, setVmDirectory );
}

void CPackedProblemReport::setInstalledSoftware( QString value )
{
	setValueWithCaller( value, PR_PACKED_REP_INSTALLED_SOFTWARE, setInstalledSoftware );
}

void CPackedProblemReport::setFilesMd5InProductBundle( QString value )
{
	setValueWithCaller( value, PR_PACKED_REP_FILES_MD5_IN_PROD_BUNDLE, setFilesMd5InProductBundle );
}

void CPackedProblemReport::setLaunchdInfo( QString value )
{
	setValueWithCaller( value, PR_PACKED_REP_LAUNCHD_INFO, setLaunchdInfo );
}

int CPackedProblemReport::fromBaseReport( const QString & strBaseReport )
{
	int iRes = CProblemReport::fromString( strBaseReport);
	if ( PRL_FAILED( iRes ) )
		return iRes;

	// set to temp directory all data from xml
	QString strValue = CProblemReport::getAdvancedVmInfo()->toString();
	setValue( strValue, PR_PACKED_REP_ADVANCED_VM_INFO );

	strValue = CProblemReport::getClientInfo()->toString();
	setValue( strValue, PR_PACKED_REP_CLIENT_INFO );

	setHostStatistic( CProblemReport::getHostStatistic() );

	setHostInfo( CProblemReport::getHostInfo() );

	setMoreHostInfo( CProblemReport::getMoreHostInfo() );

	setAllProcesses( CProblemReport::getAllProcesses() );

	setAllProcessesSamples( CProblemReport::getAllProcessesSamples() );

	setLoadedDrivers( CProblemReport::getLoadedDrivers() );

	setClientProxyInfo( CProblemReport::getClientProxyInfo() );

	setAppConfig( CProblemReport::getAppConfig() );

	setNetConfig( CProblemReport::getNetConfig() );

	setMonitorData( CProblemReport::getMonitorData() );

	setPerformanceCounters( CProblemReport::getPerformanceCounters() );

	setVmConfig( CProblemReport::getVmConfig() );

	setVmDomain( CProblemReport::getVmDomain() );

	setGuestOs( CProblemReport::getGuestOs() );

	setVmDirectory( CProblemReport::getVmDirectory() );

	setInstalledSoftware( CProblemReport::getInstalledSoftware() );

	setFilesMd5InProductBundle( CProblemReport::getFilesMd5InProductBundle() );

	setLaunchdInfo( CProblemReport::getLaunchdInfo() );

	// screenshot processing
	if( getUserDefinedData() && getUserDefinedData()->getScreenShots() )
		foreach( CRepScreenShot * pScr, getUserDefinedData()->getScreenShots()->m_lstScreenShot )
		{
			if( pScr )
			{
				if( !pScr->getName().isEmpty() &&
					pScr->getData() != QString(XML_DEFAULT_STRING_DATA) &&
					!pScr->getData().isEmpty() )
				{
					QStringList lstParts = pScr->getName().split("/");
					pScr->setName( lstParts.last() );

					QString strPathInTemp = m_strTempDirPath + QString("/") + lstParts.last();
					QFile cScreenShotFile(strPathInTemp);
					if( cScreenShotFile.open(QFile::WriteOnly) )
						cScreenShotFile.write( QByteArray::fromBase64( pScr->getData().toUtf8() ) );
					cScreenShotFile.close();

					pScr->setData();
				}
			}
		}

	// log files
	if( getSystemLogs() )
		foreach( CRepSystemLog * pLog, getSystemLogs()->m_lstSystemLog )
		{
			if( pLog )
			{
				QStringList lstParts = pLog->getName().split("/");
				QString strName = lstParts.last();
				if( strName.isEmpty() )
					continue;

				if( !strName.endsWith(".log" ) )
					strName += ".log";
				pLog->setName( strName );

				setValue( pLog->getData(), pLog->getName() );
				pLog->setData();
			}
		}

	// get crash dumps
	for( int i = 0 ; i < m_lstCrashDumps.size() ; i++ )
	{
		if ( m_lstCrashDumps[i] )
		{
			QString strPathInTemp = m_strTempDirPath + QString("/") + QString("CrashDump%1").arg(i);
			m_lstCrashDumps[i]->setNameInArchive( strPathInTemp );
			QFile cFile( strPathInTemp );
			if( cFile.open( QFile::WriteOnly ) )
				cFile.write( QByteArray::fromBase64( m_lstCrashDumps[i]->getDump() ) );
			cFile.close();

			m_lstCrashDumps[i]->setDump();
		}

	}

	saveMainXml();
	return PRL_ERR_SUCCESS;
}

bool CPackedProblemReport::isValid() const
{
	return (m_bValid);
}

void CPackedProblemReport::setCleanupTempDir( bool bValue )
{
	m_bCleanupTempDir = bValue;
}
