/*
 * PrlMiscellaneous.cpp: This file is the part of parallels public
 * sdk library Miscelaneous functions.
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


#include "PrlMiscellaneous.h"
#include "Libraries/Logging/Logging.h"

#include <QObject>

#if defined(_WIN_)
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifndef _WIN_
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#endif

/** Object synchronizing access to stderr redirector */
Q_GLOBAL_STATIC(QMutex, getStderrRedirectorMutex)

namespace PrlMiscellaneous
{

bool PrlIsInteractiveSession()
{
	return (false);
}

#ifndef _WIN_

PrlStderrRedirector::PrlStderrRedirector()
: m_MutexLocker(getStderrRedirectorMutex()), m_nRetCode(-1)
{
	m_nStderr = dup(fileno(stderr));
	if (-1 == m_nStderr)
	{
		m_nRetCode = errno;
		WRITE_TRACE(DBG_FATAL, "Failed to dup() stderr with error: %d '%s'", m_nRetCode, strerror(m_nRetCode));
	}
	m_nDevNull = open("/dev/null", O_APPEND);
	if (-1 == m_nDevNull)
	{
		m_nRetCode = errno;
		WRITE_TRACE(DBG_FATAL, "Failed to open /dev/null with error: %d '%s'", m_nRetCode, strerror(m_nRetCode));
	}
	if (-1 != m_nStderr && -1 != m_nDevNull)
		m_nRetCode = dup2(m_nDevNull, fileno(stderr));
}

PrlStderrRedirector::~PrlStderrRedirector()
{
	if (-1 != m_nStderr && -1 != m_nDevNull && -1 != m_nRetCode)
	{
		m_nRetCode = dup2(m_nStderr, fileno(stderr));
		close(m_nStderr);
		close(m_nDevNull);
		if (-1 == m_nRetCode)
		{
			m_nRetCode = errno;
			WRITE_TRACE(DBG_FATAL, "Failed to restore stderr with error: %d '%s'", m_nRetCode, strerror(m_nRetCode));
		}
	}
}

#endif

}//namespace PrlMiscellaneous

