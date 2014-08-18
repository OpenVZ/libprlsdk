/*
 * CSimpleFileHelper.h: A couple of simple crossplatform helper
 * methods working with file system
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


#ifndef SIMPLE_FILEHELPER_H
#define SIMPLE_FILEHELPER_H

#include "SDK/Include/PrlTypes.h"
/**
* @brief
*		  CSimpleFileHelper - object for simple files operation.
*/

class QFile;
class CSimpleFileHelper
{
private:
	CSimpleFileHelper ();

public:
	/**
	* Read from end of file data with cur size
	* @param File Path to read
	* @param uint uiSize - reading size
	* @param QByteArray & data - out data
	* @return well formed path part string value
	*/
	static bool ReadfromFile(const QString & strFilePath,
								uint uiSize,
								QByteArray & data,
								bool bFromEnd = true,
								bool bReplaceNonPrintble = true );

	// calculate total size of content of directory (recursive with subdirs)
	static PRL_RESULT GetDirSize( const QString& strDirPath, quint64 *pSize );

	// remove directory with all contained objects
	static bool ClearAndDeleteDir(const QString & strDir, int attempts = 3);

	static bool AtomicMoveFile( const QString& sFrom, const QString& sTo );

private:
	static PRL_RESULT GetDirSizePrivate( const QString& strDirPath, quint64 *pSize );
#ifdef _WIN_
	static bool CleanupReadOnlyAttr( const QString &sTargetPath );
#endif
};

#endif // SIMPLE_FILEHELPER_H
