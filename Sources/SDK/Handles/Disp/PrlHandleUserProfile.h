/*
 * PrlHandleUserProfile.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __PARALLELS_HANDLE_USER_PROFILE_H__
#define __PARALLELS_HANDLE_USER_PROFILE_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/DispConfig/CDispUser.h>

/**
 * Handle object represents user profile
 */
class PrlHandleUserProfile : public PrlHandleBase
{

public:
	/**
	 * Class default constructor.
	 */
	PrlHandleUserProfile(const QString& sUserProfile);

	/**
	 * Read object representation from string
	 */
	PRL_RESULT fromString(PRL_CONST_STR user_profile);

	/**
	 * Returns XML as string
	 */
	QString toString();

	/**
	 * Returns default VM dir
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetVmDirUuid(PRL_STR sVmDirUuid, PRL_UINT32_PTR pnVmDirUuidBufLength);

	/**
	 * Sets default VM dir
	 * @param [in] new VM dir string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetVmDirUuid(PRL_CONST_STR sNewVmDirUuid);


	/**
	 * Returns path of default folder for users VM
	 * May be empty
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultVmFolder(PRL_STR sDefaultVmFolder, PRL_UINT32_PTR pnDefaultVmFolderBufLength);

	/**
	 * Sets default folder to storing users VM
	 * May be empty
	 * @param [in] new VM dir string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDefaultVmFolder(PRL_CONST_STR sNewDefaultVmFolder);


	/**
	 * Returns sign whether user can using server management console utility
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT CanUseMngConsole(PRL_BOOL_PTR pbCanUseMngConsole);

	/**
	 * Returns sign whether user can changing common server settings
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT CanChangeSrvSets(PRL_BOOL_PTR pbCanChangeSrvSets);

	/**
	 * Returns sign whether user is local administrator at the host where Parallels Service started
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsLocalAdministrator(PRL_BOOL_PTR pbIsLocalAdministrator);

	/**
	 * Add information about proxy and credentials to access to the specifid
     * proxy server.
     * @param proxy IP or host name (a UTF\-8 encoded, null\-terminated
	 *        string).
     * @param proxy port
     * @param username to authenticate on proxy server (a UTF\-8 encoded,
	 *        null\-terminated string).
     * @param password to authenticate on proxy server (a UTF\-8 encoded,
	 *        null\-terminated string).
	 * @param reserved for future use. Should be 0.
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT AddProxy(PRL_CONST_STR sProxyHost, PRL_UINT32 nProxyPort,
			PRL_CONST_STR sProxyUserLogin, PRL_CONST_STR sProxyUserPasswd,
			PRL_INT32 nReserved);

public:
	/** Returns user profile internal object for external usage */
	inline CDispUser &GetUserProfile() {return (m_UserProfile);}

private:
	/// UserProfile object
	CDispUser m_UserProfile;
};

typedef PrlHandleSmartPtr<PrlHandleUserProfile> PrlHandleUserProfilePtr;

#endif // __PARALLELS_HANDLE_USER_PROFILE_H__
