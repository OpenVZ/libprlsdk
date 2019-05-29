/*
 * PrlHandleLicense.h
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


#ifndef __PARALLELS_HANDLE_LICENSE_H__
#define __PARALLELS_HANDLE_LICENSE_H__


#include "PrlHandleBase.h"
#include <prlcommon/Messaging/CVmEvent.h>


/**
 * Handle object represents License info
 */
class PrlHandleLicense : public PrlHandleBase
{

public:
	/**
	 * Class default constructor.
	 * @param VM event data for internal initialization
	 */
	PrlHandleLicense ( CVmEvent *e = 0 );


	/**
	 * Class destructor.
	 */
	~PrlHandleLicense();

	/**
	 * Read object representation from string
	 */
	PRL_RESULT fromString(PRL_CONST_STR license);

	/**
	 * Returns XML as string
	 */
	QString toString();

	/**
	 * Create new license handle
	 */
	static PRL_RESULT CreateLicense(PRL_HANDLE_PTR phLic );

public://SDK helpers set
	/**
	 * Returns sign whether license is valid
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_NO_DATA - license object do not has necessary data
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsValid(PRL_BOOL_PTR pbValue);
	/**
	 * Returns license status info.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_NO_DATA - license object do not has necessary data
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetStatus(PRL_RESULT_PTR pnValue);
	/**
	 * Returns license key string.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_NO_DATA - license object do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetLicenseKey(PRL_STR sLicenseKey, PRL_UINT32_PTR pnLicenseKeyBufLength);
	/**
	 * Returns license user name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_NO_DATA - license object do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUserName(PRL_STR sUserName, PRL_UINT32_PTR pnUserNameBufLength);
	/**
	 * Returns license company name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_NO_DATA - license object do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCompanyName(PRL_STR sCompanyName, PRL_UINT32_PTR pnCompanyNameBufLength);

private:
	/** License info internal object */
	CVmEvent m_evtLicense;
};

#endif // __PARALLELS_HANDLE_LICENSE_H__
