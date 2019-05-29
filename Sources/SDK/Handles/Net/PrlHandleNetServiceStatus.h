/*
 * PrlHandleNetServiceStatus.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#ifndef __PARALLELS_HANDLE_NET_STATUS_H__
#define __PARALLELS_HANDLE_NET_STATUS_H__

#include "PrlHandleBase.h"

/**
 * Handle object represents server configuration
 */
class PrlHandleNetServiceStatus : public PrlHandleBase
{

public:
	/**
	 * Class default constructor.
	 */
	PrlHandleNetServiceStatus(const QString& sNetStatus);

	/**
	 * Class destructor.
	 */
	~PrlHandleNetServiceStatus();

	/**
	 * Returns a status of Net service
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetStatus(PRL_SERVICE_STATUS_ENUM_PTR pnStatus);

private:
	QString m_qsStatus;
};

#endif // __PARALLELS_HANDLE_NET_STATUS_H__
