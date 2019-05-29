/*
 * PrlHandleNetworkShapingConfig.cpp
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

#include "PrlHandleNetworkShapingConfig.h"
#include "PrlHandleNetworkShaping.h"
#include "PrlHandleNetworkShapingBandwidth.h"
#include "PrlHandleHandlesList.h"

PrlHandleNetworkShapingConfig::PrlHandleNetworkShapingConfig()
: PrlHandleBase(PHT_NETWORK_SHAPING_CONFIG)
{
}

PrlHandleNetworkShapingConfig::PrlHandleNetworkShapingConfig(QString sNetworkShaping)
: PrlHandleBase(PHT_NETWORK_SHAPING_CONFIG)
{
	m_NetworkShapingConfig.fromString(sNetworkShaping);
}

PRL_RESULT PrlHandleNetworkShapingConfig::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_NetworkShapingConfig.fromString(QString::fromUtf8(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleNetworkShapingConfig::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_NetworkShapingConfig.toString();
}

PRL_RESULT PrlHandleNetworkShapingConfig::IsEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_NetworkShapingConfig.isEnabled();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShapingConfig::SetEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_NetworkShapingConfig.setEnabled(bEnabled);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShapingConfig::GetNetworkShapingList(PRL_HANDLE_PTR phList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	PrlHandleHandlesList *lst = new PrlHandleHandlesList;
	foreach(CNetworkShaping *pEntry, m_NetworkShapingConfig.m_lstNetworkShaping)
	{
		PrlHandleBasePtr phEntry(new PrlHandleNetworkShaping(pEntry->toString()));
		lst->AddItem(phEntry);
	}
	*phList = lst->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShapingConfig::SetNetworkShapingList(PRL_HANDLE hList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	m_NetworkShapingConfig.ClearList(m_NetworkShapingConfig.m_lstNetworkShaping);

	PrlHandleHandlesListPtr pList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hList);
	foreach (PrlHandleBasePtr ph, pList->GetHandlesList())
	{
		if (ph->GetType() != PHT_NETWORK_SHAPING)
			continue;

		PrlHandleNetworkShapingPtr phEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(ph->GetHandle());
		CNetworkShaping *pEntry = new CNetworkShaping;
		pEntry->fromString(phEntry->toString());
		m_NetworkShapingConfig.m_lstNetworkShaping += pEntry;
	}

	return PRL_ERR_SUCCESS;
}


PRL_RESULT PrlHandleNetworkShapingConfig::GetNetworkDeviceBandwidthList(PRL_HANDLE_PTR phList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	PrlHandleHandlesList *lst = new PrlHandleHandlesList;
	foreach(CDeviceBandwidth *pEntry, m_NetworkShapingConfig.m_lstDevicesBandwidth)
	{
		PrlHandleBasePtr phEntry(new PrlHandleNetworkShapingBandwidth(pEntry->toString()));
		lst->AddItem(phEntry);
	}
	*phList = lst->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkShapingConfig::SetNetworkDeviceBandwidthList(PRL_HANDLE hList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	m_NetworkShapingConfig.ClearList(m_NetworkShapingConfig.m_lstDevicesBandwidth);


	PrlHandleHandlesListPtr pList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hList);
	foreach (PrlHandleBasePtr ph, pList->GetHandlesList())
	{
		if (ph->GetType() != PHT_NETWORK_SHAPING_BANDWIDTH)
			continue;

		PrlHandleNetworkShapingBandwidthPtr phEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingBandwidth>(ph->GetHandle());
		CDeviceBandwidth *pEntry = new CDeviceBandwidth;
		pEntry->fromString(phEntry->toString());
		m_NetworkShapingConfig.m_lstDevicesBandwidth += pEntry;
	}

	return PRL_ERR_SUCCESS;
}
