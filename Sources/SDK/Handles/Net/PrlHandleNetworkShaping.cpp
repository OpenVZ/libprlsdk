/*
 * PrlHandleNetworkShaping.cpp
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

#include "PrlHandleNetworkShaping.h"

PrlHandleNetworkShaping::PrlHandleNetworkShaping()
: PrlHandleBase(PHT_NETWORK_SHAPING)
{
}

PrlHandleNetworkShaping::PrlHandleNetworkShaping(QString sNetworkShaping)
: PrlHandleBase(PHT_NETWORK_SHAPING)
{
	m_NetworkShaping.fromString(sNetworkShaping);
}

PRL_RESULT PrlHandleNetworkShaping::CreateNetworkShapingEntry(
		PRL_UINT32 nClassId,
		PRL_UINT32 nTotalRate,
		PRL_HANDLE_PTR phNetworkShaping
		)
{
	PrlHandleNetworkShaping* pNetworkShaping = new PrlHandleNetworkShaping();
	if ( !pNetworkShaping )
		return PRL_ERR_OUT_OF_MEMORY;

	pNetworkShaping->SetClassId(nClassId);
	pNetworkShaping->SetTotalRate(nTotalRate);

	*phNetworkShaping = pNetworkShaping->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShaping::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_NetworkShaping.fromString(QString::fromUtf8(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleNetworkShaping::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_NetworkShaping.toString();
}

PRL_RESULT PrlHandleNetworkShaping::GetDevice(PRL_STR sDev, PRL_UINT32_PTR pnDevBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue( m_NetworkShaping.getDevice(), sDev, pnDevBufLength);
}

PRL_RESULT PrlHandleNetworkShaping::SetDevice(PRL_CONST_STR sDev)
{
	if (sDev== NULL || sDev[0] == '\0')
		return PRL_ERR_INVALID_ARG;

	SYNCHRO_INTERNAL_DATA_ACCESS
	m_NetworkShaping.setDevice(QString::fromUtf8(sDev));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShaping::GetClassId(PRL_UINT32_PTR pnClassId)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnClassId = m_NetworkShaping.getClassId();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShaping::SetClassId(PRL_UINT32 nClassId)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_NetworkShaping.setClassId(nClassId);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShaping::GetTotalRate(PRL_UINT32_PTR pnTotalRate)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnTotalRate = m_NetworkShaping.getTotalRate();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShaping::SetTotalRate(PRL_UINT32 nTotalRate)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_NetworkShaping.setTotalRate(nTotalRate);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShaping::GetRate(PRL_UINT32_PTR pnRate)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnRate = m_NetworkShaping.getRate();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShaping::SetRate(PRL_UINT32 nRate)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_NetworkShaping.setRate(nRate);
	return PRL_ERR_SUCCESS;
}

PRL_INT32 PrlHandleNetworkShaping::GetRateMPU() const
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_NetworkShaping.getRateMPU();
}

void PrlHandleNetworkShaping::SetRateMPU(PRL_INT32 pnRateMPU)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_NetworkShaping.setRateMPU(pnRateMPU);
}
