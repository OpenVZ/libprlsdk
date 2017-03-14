/*
 * PrlHandleServerInfo.h
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


#ifndef __PARALLELS_HANDLE_SERVER_INFO_H__
#define __PARALLELS_HANDLE_SERVER_INFO_H__


#include "PrlHandleBase.h"
#include <prlcommon/Messaging/CVmEvent.h>


/**
 * Handle object represents ServerInfo info
 */
class PrlHandleServerInfo : public PrlHandleBase
{

public:
	/**
	 * Class constructor.
	 * @param VM event string data for internal initialization
	 * @param server hostname
	 */
	PrlHandleServerInfo ( const QString &sVmEvent, const QString &sServerHost );
	/**
	 * Class constructor.
	 * @param VM event string data for internal initialization (additionally should contain server hostname for class members initialization)
	 */
	explicit PrlHandleServerInfo ( const QString &sVmEvent );

public://SDK helpers set
	/**
	 * Returns server hostname.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_NO_DATA - server info object do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHostName(PRL_STR sHostName, PRL_UINT32_PTR pnHostNameBufLength);
	/**
	 * Returns server OS version.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_NO_DATA - server info object do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOsVersion(PRL_STR sOsVersion, PRL_UINT32_PTR pnOsVersionBufLength);
	/**
	 * Returns server info commands processing port.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_NO_DATA - server info object do not has necessary data
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCmdPort(PRL_UINT32_PTR pnPort);
	/**
	 * Returns server UUID.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_NO_DATA - server info object do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetServerUuid(PRL_STR sServerUuid, PRL_UINT32_PTR pnServerUuidBufLength);
	/**
	 * Returns server product version.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_NO_DATA - server info object do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetProductVersion(PRL_STR sProductVersion, PRL_UINT32_PTR pnProductVersionBufLength);

	/**
	* Returns server info commands app execution mode.
	* @param [out] buffer for storing result
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_NO_DATA - server info object do not has necessary data
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetApplicationMode (PRL_APPLICATION_MODE_PTR pnApplicationMode);
	/**
	 * Returns service start time in milliseconds since Epoch 01-01-1970 (wall clock time).
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_NO_DATA - server info object does not have necessary data
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetStartTime(PRL_UINT64_PTR pnStartTime);
	/**
	 * Returns service start time in milliseconds (monotonic clock time).
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_NO_DATA - server info object does not have necessary data
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetStartTimeMonotonic(PRL_UINT64_PTR pnStartTimeMonotonic);

private:
	/** ServerInfo info internal object */
	CVmEvent m_evtServerInfo;
	/** Server hostname */
	QString m_sServerHost;
};

typedef PrlHandleSmartPtr<PrlHandleServerInfo> PrlHandleServerInfoPtr;

#endif // __PARALLELS_HANDLE_SERVER_INFO_H__
