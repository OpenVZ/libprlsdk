/*
 * PrlHandleVmGuest.h
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


#ifndef __VIRTUOZZO_HANDLE_VM_GUEST_H__
#define __VIRTUOZZO_HANDLE_VM_GUEST_H__

#include "PrlHandleVmSrv.h"
#include <QString>

/**
 * Virtual machine guest OS session handle.
 */
class PrlHandleVmGuest : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * @param pointer to the parent VM handle object
	 * @param VM guest OS session
	 */
	PrlHandleVmGuest ( const PrlHandleVmPtr &pVm, const QString &sVmGuestUuid );
	/**
	 * Performs request on logout from VM guest session
	 * @param operation flags bitset
	 * @return pointer to the generated job object
	 */
	PrlHandleJobPtr Logout(PRL_UINT32 nFlags);
	/**
	 * Performs request on execute a program at VM guest session
	 * @param program name
	 * @param handle to the object of list with program arguments
	 * @param handle to the object of list with environment variables
	 * @param operation flags bitset
	 * @param file descriptor to standard input
	 * @param file descriptor to standard output
	 * @param file descriptor to standard errors output
	 * @return pointer to the generated job object
	 */
	PrlHandleJobPtr RunProgram(PRL_CONST_STR sProgramName, PRL_HANDLE hArgsList, PRL_HANDLE hEnvsList,\
		PRL_UINT32 nFlags, PRL_FILE_DESC nStdin, PRL_FILE_DESC nStdout, PRL_FILE_DESC nStderr);
	/**
	 * Performs request on getting network settings from guest OS
	 * @param operation flags bitset
	 * @return pointer to the generated job object
	 */
	PrlHandleJobPtr GetNetworkSettings(PRL_UINT32 nFlags);
	/**
	 * Performs request on reset guest user account password
	 * @param user login name
	 * @param user password
	 * @param operation flags bitset
	 * @return pointer to the generated job object
	 */
	PrlHandleJobPtr SetUserPasswd(PRL_CONST_STR sUserName, PRL_CONST_STR sUserPasswd, PRL_UINT32 nFlags);

	PRL_HANDLE GetVmHandle() { return m_pVm->GetHandle(); }

private:
	/** Pointer to the parent VM handle object */
	PrlHandleVmSrvPtr m_pVm;
	/** VM guest OS session UUID */
	QString m_sVmGuestUuid;
};

#endif // __VIRTUOZZO_HANDLE_VM_GUEST_H__

