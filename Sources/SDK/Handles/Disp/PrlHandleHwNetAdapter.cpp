/*
 * PrlHandleHwNetAdapter.cpp
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


#include "PrlHandleHwNetAdapter.h"
#include <prlxmlmodel/HostHardwareInfo/CHwNetAdapter.h>
#include "PrlHandleStringsList.h"

#define CHECK_NET_ADAPTER_OBJECT\
	if (!m_pHwNetAdapter)\
		return (PRL_ERR_INVALID_ARG);

PrlHandleHwNetAdapter::PrlHandleHwNetAdapter(const PrlHandleSrvConfigPtr &pSrvConfig, CHwNetAdapter *pDevice)
: PrlHandleHwGenericDevice(PHT_HW_NET_ADAPTER, pSrvConfig, pDevice), m_pHwNetAdapter(pDevice)
{}

PrlHandleHwNetAdapter::~PrlHandleHwNetAdapter()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pHwNetAdapter = NULL;
}

PRL_RESULT PrlHandleHwNetAdapter::GetNetAdapterType(PRL_HW_INFO_NET_ADAPTER_TYPE_PTR pnNetAdapterType)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	*pnNetAdapterType = m_pHwNetAdapter->getNetAdapterType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwNetAdapter::GetSysIndex(PRL_UINT32_PTR pnSysIndex)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	*pnSysIndex = m_pHwNetAdapter->getSysIndex();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwNetAdapter::IsEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	*pbEnabled = (PRL_BOOL)m_pHwNetAdapter->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwNetAdapter::GetNetAddresses(PRL_HANDLE_PTR phNetAddressesList)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	PrlHandleStringsList *pNetAddressesList = new PrlHandleStringsList(m_pHwNetAdapter->getNetAddresses());
	if (pNetAddressesList)
	{
		*phNetAddressesList = pNetAddressesList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	else
		return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleHwNetAdapter::IsConfigureWithDhcp(PRL_BOOL_PTR pbConfigureWithDhcp)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	*pbConfigureWithDhcp = (PRL_BOOL)m_pHwNetAdapter->isConfigureWithDhcp();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwNetAdapter::IsConfigureWithDhcpIPv6(PRL_BOOL_PTR pbConfigureWithDhcp)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	*pbConfigureWithDhcp = (PRL_BOOL)m_pHwNetAdapter->isConfigureWithDhcpIPv6();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwNetAdapter::GetMacAddress(PRL_STR sMacAddress, PRL_UINT32_PTR pnMacAddressBufLength)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	return CopyStringValue(m_pHwNetAdapter->getMacAddress(), sMacAddress,
													pnMacAddressBufLength);
}

PRL_RESULT PrlHandleHwNetAdapter::GetVlanTag(PRL_UINT16_PTR pnVlanTag)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	*pnVlanTag = m_pHwNetAdapter->getVLANTag();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwNetAdapter::GetDefaultGateway(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	return CopyStringValue(m_pHwNetAdapter->getDefaultGateway(), sDefaultGateway,
													pnDefaultGatewayBufLength);
}

PRL_RESULT PrlHandleHwNetAdapter::GetDefaultGatewayIPv6(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	return CopyStringValue(m_pHwNetAdapter->getDefaultGatewayIPv6(), sDefaultGateway,
													pnDefaultGatewayBufLength);
}

PRL_RESULT PrlHandleHwNetAdapter::GetDnsServers(PRL_HANDLE_PTR phDnsServersList)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	PrlHandleStringsList *pDnsServersList = new PrlHandleStringsList(m_pHwNetAdapter->getDnsIPAddresses());
	if (pDnsServersList)
	{
		*phDnsServersList = pDnsServersList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	else
		return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleHwNetAdapter::GetSearchDomains(PRL_HANDLE_PTR phSearchDomainsList)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_NET_ADAPTER_OBJECT
	PrlHandleStringsList *pSearchDomainsList = new PrlHandleStringsList(m_pHwNetAdapter->getSearchDomains());
	if (pSearchDomainsList)
	{
		*phSearchDomainsList = pSearchDomainsList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	else
		return (PRL_ERR_OUT_OF_MEMORY);
}

