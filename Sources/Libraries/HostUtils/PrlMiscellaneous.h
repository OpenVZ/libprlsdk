/*
 * PrlMiscellaneous.h: This file is the part of parallels public sdk
 * library Miscelaneous functions.
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


#ifndef PrlMiscellaneous_H
#define PrlMiscellaneous_H

#include <stdlib.h>

#include <QMutex>

namespace PrlMiscellaneous
{

/**
 * Returns sign whether current process session is interactive
 */
#ifdef _WIN_
extern __declspec(dllexport) bool PrlIsInteractiveSession();
#else
extern bool PrlIsInteractiveSession() __attribute__ ((visibility("default")));
#endif

#ifndef _WIN_
/**
 * Simple helper which lets to redirect temporarily stderr to /dev/null and
 * restore last one on class instance destruction.
 * Major note: class thread safe and can be used from any contexts - do not use
 * at code which tries to have some other locks in order to prevent deadlocks
 */
class PrlStderrRedirector
{
public:
	/**
	 * Class constructor - tries to redirect stderr to /dev/null
	 */
	PrlStderrRedirector();
	/**
	 * Class destructor - tries to restore stderr
	 */
	~PrlStderrRedirector();

private:
	/** Locker to prevent simultaneous redirect from different threads */
	QMutexLocker m_MutexLocker;
	/** Stored stderr descriptor */
	int m_nStderr;
	/** Descriptor on opened /dev/null */
	int m_nDevNull;
	/** Variable for storing error code of operations */
	int m_nRetCode;
};
#endif//ifndef _WIN_

}//namespace PrlMiscellaneous

#endif //#ifdef PrlMiscellaneous_H

