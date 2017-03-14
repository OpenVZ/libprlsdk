/*
 * PrlHandleNetworkShapingBandwidth.cpp
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

#include "PrlHandleNetworkShapingBandwidth.h"

PrlHandleNetworkShapingBandwidth::PrlHandleNetworkShapingBandwidth()
: PrlHandleBase(PHT_NETWORK_SHAPING_BANDWIDTH)
{
}

PrlHandleNetworkShapingBandwidth::PrlHandleNetworkShapingBandwidth(QString sDevice)
: PrlHandleBase(PHT_NETWORK_SHAPING_BANDWIDTH)
{
	m_DeviceBandwidth.fromString(sDevice);
}

PRL_RESULT PrlHandleNetworkShapingBandwidth::CreateDeviceBandwidthEntry(
		PRL_CONST_STR sDev,
		PRL_UINT32 nBandwidth,
		PRL_HANDLE_PTR phDeviceBandwidth
		)
{
	PrlHandleNetworkShapingBandwidth* pDeviceBandwidth = new PrlHandleNetworkShapingBandwidth();
	if ( !pDeviceBandwidth)
		return PRL_ERR_OUT_OF_MEMORY;

	pDeviceBandwidth->SetDevice(sDev);
	pDeviceBandwidth->SetBandwidth(nBandwidth);

	*phDeviceBandwidth = pDeviceBandwidth->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShapingBandwidth::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_DeviceBandwidth.fromString(UTF8_2QSTR(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleNetworkShapingBandwidth::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_DeviceBandwidth.toString();
}

PRL_RESULT PrlHandleNetworkShapingBandwidth::GetDevice(PRL_STR sDev, PRL_UINT32_PTR pnDevBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_DeviceBandwidth.getDevice(), sDev, pnDevBufLength);
}

PRL_RESULT PrlHandleNetworkShapingBandwidth::SetDevice(PRL_CONST_STR sDev)
{
	if (sDev== NULL || sDev[0] == '\0')
		return PRL_ERR_INVALID_ARG;

	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DeviceBandwidth.setDevice(UTF8_2QSTR(sDev));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShapingBandwidth::GetBandwidth(PRL_UINT32_PTR pnBandwidth)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnBandwidth = m_DeviceBandwidth.getBandwidth();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShapingBandwidth::SetBandwidth(PRL_UINT32 nBandwidth)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DeviceBandwidth.setBandwidth(nBandwidth);
	return PRL_ERR_SUCCESS;
}
