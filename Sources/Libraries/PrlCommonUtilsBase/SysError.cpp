/*
 * SysError.cpp: error reference crossplatform functions
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


#include <stdio.h>

#ifdef _WIN_
#	include <windows.h>
#else
#	include <string.h>
#	include <sys/errno.h>
#endif


#include "SysError.h"
#include "Interfaces/ParallelsQt.h"

QString Prl::GetLastErrorAsString()
{
#	ifndef _WIN_

	return UTF8_2QSTR( strerror( errno ) );

#	else

	char tmpBuf[512];
	DWORD dwErr = ::GetLastError();
	if (!FormatMessageA(	FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		0, dwErr, 0, tmpBuf, sizeof(tmpBuf) , 0) )
	{
		sprintf(tmpBuf, "Unable to obtain system message text. Original error: %d", dwErr );
	}

	// remove windows '\n'= 0xD 0xA from end of string
	size_t lastCharPos = strlen( tmpBuf ) - 1;

	if( lastCharPos>=1 && tmpBuf[ lastCharPos ] == 0xA && tmpBuf[ lastCharPos -1 ] == 0xD )
		tmpBuf[ lastCharPos-1 ] = 0;

	return tmpBuf;

#	endif


}

long Prl::GetLastError()
{
#	ifndef _WIN_
	return errno;
#	else
    return ::GetLastError();
#	endif
}
