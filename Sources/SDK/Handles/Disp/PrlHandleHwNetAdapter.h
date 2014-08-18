/*
 * PrlHandleHwNetAdapter.h
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
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
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __PARALLELS_HANDLE_HW_NET_ADAPTER_H__
#define __PARALLELS_HANDLE_HW_NET_ADAPTER_H__


#include "PrlHandleHwGenericDevice.h"

/**
 * Host hardware info net adapter device handle object.
 */
class PrlHandleHwNetAdapter : public PrlHandleHwGenericDevice
{

public:
	/**
	 * Class constructor.
	 * Initializes net adapter device settings from specified object.
	 * @param parent server config object pointer
	 */
	PrlHandleHwNetAdapter(const PrlHandleSrvConfigPtr &pSrvConfig, class CHwNetAdapter *pNetAdapter);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleHwNetAdapter();

public://API helpers
	/**
	 * Returns net adapter type
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetNetAdapterType(PRL_HW_INFO_NET_ADAPTER_TYPE_PTR pnNetAdapterType);
	/**
	 * Returns system adapter index
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSysIndex(PRL_UINT32_PTR pnSysIndex);
	/**
	 * Returns system adapter active sign
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsEnabled(PRL_BOOL_PTR pbEnabled);
	/**
	 * Returns system adapter configure through DHCP sign
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsConfigureWithDhcp(PRL_BOOL_PTR pbConfigureWithDhcp);
	/**
	 * Returns system adapter configure IPv6 through DHCP sign
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsConfigureWithDhcpIPv6(PRL_BOOL_PTR pbConfigureWithDhcp);
	/**
	 * Returns network adapter MAC address
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetMacAddress(PRL_STR sMacAddress, PRL_UINT32_PTR pnMacAddressBufLength);
	/**
	 * Returns network adapter VLAN tag
	 * @param [out] buffer for storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetVlanTag(PRL_UINT16_PTR pnVlanTag);
	/**
	 * Returns network adapter default gateway address
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultGateway(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength);
	/**
	 * Returns network adapter IPv6 default gateway address
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultGatewayIPv6(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength);
	/**
	 * Returns system adapter network addresses list
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetNetAddresses(PRL_HANDLE_PTR phNetAddressesList);
	/**
	 * Returns system adapter DNS servers list
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDnsServers(PRL_HANDLE_PTR phDnsServersList);
	/**
	 * Returns system adapter search domains list
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSearchDomains(PRL_HANDLE_PTR phSearchDomainsList);

private:
	/// Storing net adapter device XML representation.
	class CHwNetAdapter *m_pHwNetAdapter;
};

typedef PrlHandleSmartPtr<PrlHandleHwNetAdapter> PrlHandleHwNetAdapterPtr;

#endif // __PARALLELS_HANDLE_HW_NET_ADAPTER_H__
