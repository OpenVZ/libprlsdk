/*
 * PrlHandleLoginResponse.h
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


#ifndef __PARALLELS_HANDLE_LOGIN_RESPONSE_H__
#define __PARALLELS_HANDLE_LOGIN_RESPONSE_H__


#include "PrlHandleBase.h"
#include <prlcommon/Messaging/CVmEvent.h>


/**
 * Handle object represents LoginResponse info
 */
class PrlHandleLoginResponse : public PrlHandleBase
{

public:
	/**
	 * Class default constructor.
	 * @param VM event data for internal initialization
	 */
	PrlHandleLoginResponse ( CVmEvent *e );

	/**
	 * Class destructor.
	 */
	~PrlHandleLoginResponse();

	/**
	 * Returns XML as string
	 */
	QString toString();

public://SDK helpers set
	/**
	* Returns session uuid string ( uses to restore session )
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully string value)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetSessionUuid(PRL_STR sSessionUuid, PRL_UINT32_PTR pnSessionUuidBufLength);

	/**
	* Returns server UUID
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully string value)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetServerUuid(PRL_STR sServerUuid, PRL_UINT32_PTR pnServerUuidBufLength);

	/**
	* Returns server host OS version
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully string value)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetHostOsVersion(PRL_STR sHostOsVersion, PRL_UINT32_PTR pnHostOsVersionBufLength);

	/**
	* Returns server product version
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully string value)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetProductVersion(PRL_STR sProductVersion, PRL_UINT32_PTR pnProductVersionBufLength);

	/**
	* Get count of running task items
	* @param specifies a pointer to the count of parameters
	* @return PRL_ERR_SUCCESS if the method succeeds
	*/
	PRL_RESULT GetRunningTaskCount(PRL_UINT32_PTR pCount);

	/**
	* Get running task item by index
	*
	* @param index of parameter
	* @param pointer to buffer for storing handle to object that represents parameter
	* @return PRL_ERR_SUCCESS if the method succeeds
	*/
	PRL_RESULT GetRunningTaskByIndex(PRL_UINT32 nIndex, PRL_HANDLE_PTR phParam);


private:
	/** LoginResponse internal object */
	CVmEvent m_evtLoginResponse;
};

#endif // __PARALLELS_HANDLE_LOGIN_RESPONSE_H__
