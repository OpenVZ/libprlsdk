/*
 * PrlHandleNetworkRate.cpp
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

#include "PrlHandleNetworkRate.h"

PrlHandleNetworkRate::PrlHandleNetworkRate()
: PrlHandleBase(PHT_NETWORK_RATE)
{
}

PrlHandleNetworkRate::PrlHandleNetworkRate(QString sNetworkRate)
: PrlHandleBase(PHT_NETWORK_RATE)
{
	m_NetworkRate.fromString(sNetworkRate);
}

PRL_RESULT PrlHandleNetworkRate::CreateNetworkRate(
		PRL_UINT32 nClass,
		PRL_UINT32 nRate,
		PRL_HANDLE_PTR phNetworkRate
		)
{
	PrlHandleNetworkRate* pNetworkRate = new PrlHandleNetworkRate();
	if ( !pNetworkRate )
		return PRL_ERR_OUT_OF_MEMORY;

	pNetworkRate->SetClass(nClass);
	pNetworkRate->SetRate(nRate);

	*phNetworkRate = pNetworkRate->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkRate::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_NetworkRate.fromString(QString::fromUtf8(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleNetworkRate::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_NetworkRate.toString();
}

PRL_RESULT PrlHandleNetworkRate::GetClass(PRL_UINT32_PTR pnClass)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnClass = m_NetworkRate.getClassId();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkRate::SetClass(PRL_UINT32 nClass)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_NetworkRate.setClassId(nClass);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkRate::GetRate(PRL_UINT32_PTR pnRate)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnRate = m_NetworkRate.getRate();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkRate::SetRate(PRL_UINT32 nRate)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_NetworkRate.setRate(nRate);
	return PRL_ERR_SUCCESS;
}
