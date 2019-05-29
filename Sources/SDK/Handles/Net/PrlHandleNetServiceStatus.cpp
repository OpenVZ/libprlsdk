/*
 * PrlHandleNetServiceStatus.cpp
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


#include "PrlHandleNetServiceStatus.h"

PrlHandleNetServiceStatus::PrlHandleNetServiceStatus(const QString& sNetStatus)
: PrlHandleBase(PHT_NET_SERVICE_STATUS), m_qsStatus(sNetStatus)
{
}

PrlHandleNetServiceStatus::~PrlHandleNetServiceStatus()
{
}

PRL_RESULT PrlHandleNetServiceStatus::GetStatus(PRL_SERVICE_STATUS_ENUM_PTR pnStatus)
{
	if (m_qsStatus.isEmpty())
	{
		*pnStatus = PSS_UNKNOWN;
		return PRL_ERR_GET_NET_SERVICE_STATUS_FAILED;
	}

	*pnStatus = (PRL_SERVICE_STATUS_ENUM)m_qsStatus.toUInt();
	return PRL_ERR_SUCCESS;
}
