/*
 * CSimpleFileHelper.cpp: A couple of simple crossplatform helper
 * methods working with file system.
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


#ifdef _LIN_
#       include <sys/errno.h>
#endif

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QByteArray>

#include "CSimpleFileHelper.h"

#include "Libraries/Logging/Logging.h"
#include "SDK/Include/PrlErrorsValues.h"
#include "SDK/Include/PrlErrors.h"
#include "Interfaces/ParallelsQt.h"
#include "Libraries/PrlCommonUtilsBase/SysError.h"
#include "Libraries/Std/PrlAssert.h"

/**
* Read from end of file data with cur size
* @param file path
* @param uint uiSize - size to read
* @param QByteArray & data - data to store
* @return bool result
*/
bool CSimpleFileHelper::ReadfromFile(const QString & strFilePath,
										uint uiSize,
										QByteArray & data,
										bool bFromEnd,
										bool bReplaceNonPrintble )
{
	QFile cFile(strFilePath);

	if (cFile.open(QFile::ReadOnly))
	{
		if(bFromEnd)
		{
			if(cFile.size() > uiSize)
				cFile.seek(cFile.size()- uiSize);
		}

		data = cFile.read(uiSize);
		// replace non printable symbols
		if ( bReplaceNonPrintble )
		{
			QString strTemp = UTF8_2QSTR( data.constData() );
			for( int i = 0 ; i < strTemp.size(); i++ )
			{
				if ( !strTemp[i].isPrint() && strTemp[i] != '\n' && strTemp[i] != '\r')
					strTemp[i] = '?';
			}
			data = strTemp.toUtf8();
		}
		cFile.close();
		return true;
	}
	return false;

}

PRL_RESULT CSimpleFileHelper::GetDirSize( const QString& strDirPath, quint64 *pSize )
{
	if ( ! pSize )
		return PRL_ERR_INVALID_ARG;

	*pSize = 0;

	return GetDirSizePrivate( strDirPath, pSize );
}

PRL_RESULT CSimpleFileHelper::GetDirSizePrivate( const QString& strDirPath, quint64 *pSize )
{
	QFileInfo fi( strDirPath );
	if ( !fi.isDir() || !pSize )
		return PRL_ERR_INVALID_ARG;

	QFileInfoList fiList = QDir( strDirPath ).entryInfoList(
		QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden,
		QDir::DirsFirst
		);

	QListIterator< QFileInfo > it( fiList );
	while( it.hasNext() )
	{
		const QFileInfo& fi = it.next();
		PRL_RESULT ret = PRL_ERR_SUCCESS;

		if ( fi.isDir() )
		{
			// #125021 to prevent infinity recursion by QT bug in QDir::entryInfoList()
			if( QFileInfo(strDirPath) == fi )
				continue;

			ret = GetDirSizePrivate( fi.filePath(), pSize );
		}
		else
			*pSize += fi.size();

		if ( ! PRL_SUCCEEDED( ret ) )
		{
			WRITE_TRACE(DBG_FATAL, "GetDirSize() failed in dir [%s]", QSTR2UTF8( fi.filePath() ) );
			return ret;
		}
	}//while

	return PRL_ERR_SUCCESS;
}

#ifdef _WIN_
/**
 * Cleanups readonly attribute for specified file
 */
bool CSimpleFileHelper::CleanupReadOnlyAttr( const QString &sTargetPath )
{
	DWORD fileAttributes = GetFileAttributes(sTargetPath.utf16());
	if (fileAttributes & FILE_ATTRIBUTE_READONLY)
    {
		if ( !SetFileAttributes(sTargetPath.utf16(), fileAttributes ^ FILE_ATTRIBUTE_READONLY) )
		{
			DWORD dwError = GetLastError();
			WRITE_TRACE(DBG_FATAL, "Couldn't to unset readonly attribute for path '%s' with error: %d", QSTR2UTF8(sTargetPath), dwError);
			return false;
		}
	}
	return true;
}
#endif


/**
* @brief
*		 remove directory with all contained objects
*		 It is only windows variant tested,check it on linux
*
* @params
*		  QString & strDir - directory for delete
* @return
*		 bool - result
*
*/


bool CSimpleFileHelper::ClearAndDeleteDir(const QString & strDir, int attempts)
{
	PRL_ASSERT( ! strDir.isEmpty() );
	if( strDir.isEmpty() || QFileInfo( strDir).isRoot() )
		return false;

	QDir	cDir(strDir);
	if (!cDir.exists())
		return false;

	cDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);

	cDir.setSorting(QDir::Name | QDir::DirsLast);
	QFileInfoList cFileList;
	bool bRes = false;

	for (int cnt = 0; cnt < attempts; cnt++)
	{
		cDir.refresh();
		cFileList = cDir.entryInfoList();
		if (cFileList.isEmpty())
			break;

		// recursive clearing dirs
		for(int i = 0; i < cFileList.size();i++ )
		{
			if(cFileList.at(i).isDir() )
			{
				// #125021 to prevent infinity recursion by QT bug in QDir::entryInfoList()
				if( QFileInfo(strDir) == cFileList.at(i) )
					continue;

				QString strTemp = cFileList.at(i).filePath();
#ifdef _WIN_
				CleanupReadOnlyAttr( strTemp );
#endif
				if(cFileList.at(i).isSymLink())
				{
					// if it alias delete alias file,on macos symlink open as link target
					QFile::remove( strTemp);
				}
				else
					ClearAndDeleteDir(strTemp);
			}
			else
			{
				// delete file
#ifdef _WIN_
				CleanupReadOnlyAttr( cFileList.at(i).filePath() );
#endif
				bRes = QFile::remove( cFileList.at(i).filePath() );
				if ( ! bRes )
					WRITE_TRACE(DBG_FATAL,
						"ClearAndDeleteDir: "
						"cannot delete file '%s' ! System error: %ld [%s]",
					QSTR2UTF8(cFileList.at(i).filePath()),
					Prl::GetLastError(),
					QSTR2UTF8(Prl::GetLastErrorAsString()) );
			}
		}
	}
	// remove empty directory
#ifdef _WIN_
	CleanupReadOnlyAttr( strDir );
#endif
	bRes = QDir().rmdir( strDir );
	if ( ! bRes )
	{
		WRITE_TRACE(DBG_FATAL,
			"ClearAndDeleteDir: cannot delete directory '%s' ! System error: %ld [%s]",
			QSTR2UTF8(strDir),
			Prl::GetLastError(),
			QSTR2UTF8(Prl::GetLastErrorAsString()) );
		cDir.refresh();
		cFileList = cDir.entryInfoList();
		foreach(QFileInfo info, cFileList)
		{
			WRITE_TRACE(DBG_FATAL, "Entry stayed after cleanup: [%s]", QSTR2UTF8(info.filePath()));
		}
	}

	return bRes;
}

// atomic rename file. It uses in crash-safe save mechs
// NOTE: sFrom and sTo should be on one hdd partiotion!
bool CSimpleFileHelper::AtomicMoveFile( const QString& sFrom, const QString& sTo )
{
	// copy past from CBaseNode::saveToFile()
	bool bError = false;

#ifdef	_WIN_
	DWORD errCode = 0;
	if( ! MoveFileEx(
		(LPWSTR)sFrom.utf16()
		, (LPWSTR)sTo.utf16()
		, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH  )
		)
	{
		bError = true;
		errCode = GetLastError();
	}
#else
	int errCode = 0;
	if( rename( QSTR2UTF8( sFrom ), QSTR2UTF8( sTo ) ) )
	{
		bError = true;
		errCode = errno;
	}
#endif

	if( bError )
	{
		WRITE_TRACE( DBG_FATAL, "Unable to move file by error %#x. sFrom==>sTo:'%s' ==>'%s'"
			, errCode
			, QSTR2UTF8( sFrom )
			, QSTR2UTF8( sTo ) );
		return false;
	}
	return true;
}


