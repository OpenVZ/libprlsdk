/*
 * PrlHandleVirtNet.h
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

#ifndef __PARALLELS_HANDLE_VIRT_NET_H__
#define __PARALLELS_HANDLE_VIRT_NET_H__


#include "PrlHandleBase.h"
#include "XmlModel/NetworkConfig/CVirtualNetwork.h"


/**
 * Handle object represents virtual network
 */
class PrlHandleVirtNet : public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleVirtNet();

	/**
	 * Class initialize constructor.
	 */
	PrlHandleVirtNet(QString sVirtNet);

	/**
	 * Create new virual network handle
	 */
	static PRL_RESULT CreateVirtualNetwork(PRL_HANDLE_PTR phVirtNet);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	/**
	 * Get virtual network ID
	 */
	PRL_RESULT GetNetworkId(PRL_STR sNetworkId, PRL_UINT32_PTR pnNetworkIdBufLength);

	/**
	 * Set virtual network ID
	 */
	PRL_RESULT SetNetworkId(PRL_CONST_STR sNetworkId);

	/**
	 * Get virtual network description
	 */
	PRL_RESULT GetDescription(PRL_STR sDescription, PRL_UINT32_PTR pnDescriptionBufLength);

	/**
	 * Set virtual network description
	 */
	PRL_RESULT SetDescription(PRL_CONST_STR sDescription);

	/**
	 * Get virtual network type
	 */
	PRL_RESULT GetNetworkType(PRL_NET_VIRTUAL_NETWORK_TYPE_PTR pnNetworkType);

	/**
	 * Set virtual network type
	 */
	PRL_RESULT SetNetworkType(PRL_NET_VIRTUAL_NETWORK_TYPE nNetworkType);

	/**
	 * Get virtual network bound card MAC address
	 */
	PRL_RESULT GetBoundCardMac(PRL_STR sBoundCardMac, PRL_UINT32_PTR pnBoundCardMacBufLength);

	/**
	 * Set virtual network bound card MAC address
	 */
	PRL_RESULT SetBoundCardMac(PRL_CONST_STR sBoundCardMac);

	/**
	 * Get virtual network adapter name
	 */
	PRL_RESULT GetAdapterName(PRL_STR sAdapterName, PRL_UINT32_PTR pnAdapterNameBufLength);

	/**
	 * Set virtual network adapter name
	 */
	PRL_RESULT SetAdapterName(PRL_CONST_STR sAdapterName);

	/**
	 * Get adapter index
	 */
	PRL_RESULT GetAdapterIndex(PRL_UINT32_PTR pnAdapterIndex);

	/**
	 * Set adapter index
	 */
	PRL_RESULT SetAdapterIndex(PRL_UINT32 nAdapterIndex);

	/**
	 * Get virtual network host IP address
	 */
	PRL_RESULT GetHostIPAddress(PRL_STR sHostIPAddress, PRL_UINT32_PTR pnHostIPAddressBufLength);

	/**
	 * Set virtual network host IP address
	 */
	PRL_RESULT SetHostIPAddress(PRL_CONST_STR sHostIPAddress);

	/**
	 * Get virtual network DHCP IP address
	 */
	PRL_RESULT GetDhcpIPAddress(PRL_STR sDhcpIPAddress, PRL_UINT32_PTR pnDhcpIPAddressBufLength);

	/**
	 * Set virtual network DHCP IP address
	 */
	PRL_RESULT SetDhcpIPAddress(PRL_CONST_STR sDhcpIPAddress);

	/**
	 * Get virtual network IP net mask
	 */
	PRL_RESULT GetIPNetMask(PRL_STR sIPNetMask, PRL_UINT32_PTR pnIPNetMaskBufLength);

	/**
	 * Set virtual network IP net mask
	 */
	PRL_RESULT SetIPNetMask(PRL_CONST_STR sIPNetMask);

	/**
	 * Get VLAN tag
	 */
	PRL_RESULT GetVlanTag(PRL_UINT16_PTR pnVlanTag);

	/**
	 * Set VLAN tag
	 */
	PRL_RESULT SetVlanTag(PRL_UINT16 nVlanTag);

	/**
	 * Get virtual network DHCP start IP address
	 */
	PRL_RESULT GetIPScopeStart(PRL_STR sIPScopeStart, PRL_UINT32_PTR pnIPScopeStartBufLength);

	/**
	 * Set virtual network DHCP start IP address
	 */
	PRL_RESULT SetIPScopeStart(PRL_CONST_STR sIPScopeStart);

	/**
	 * Get virtual network DHCP end IP address
	 */
	PRL_RESULT GetIPScopeEnd(PRL_STR sIPScopeEnd, PRL_UINT32_PTR pnIPScopeEndBufLength);

	/**
	 * Set virtual network DHCP end IP address
	 */
	PRL_RESULT SetIPScopeEnd(PRL_CONST_STR sIPScopeEnd);

	/**
	 * Get virtual network enabled or disabled
	 */
	PRL_RESULT IsEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Set virtual network enabled or disabled
	 */
	PRL_RESULT SetEnabled(PRL_BOOL bEnabled);

	/**
	 * Get virtual network adapter enabled or disabled
	 */
	PRL_RESULT IsAdapterEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Set virtual network adapter enabled or disabled
	 */
	PRL_RESULT SetAdapterEnabled(PRL_BOOL bEnabled);

	/**
	 * Get virtual network DHCP server enabled or disabled
	 */
	PRL_RESULT IsDHCPServerEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Set virtual network DHCP server enabled or disabled
	 */
	PRL_RESULT SetDHCPServerEnabled(PRL_BOOL bEnabled);

	/**
	 * Get virtual network NAT server enabled or disabled
	 */
	PRL_RESULT IsNATServerEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Set virtual network NAT server enabled or disabled
	 */
	PRL_RESULT SetNATServerEnabled(PRL_BOOL bEnabled);

	/**
	 * Get port forward entries list
	 */
	PRL_RESULT GetPortForwardList(PRL_PORT_FORWARDING_TYPE nPortFwdType, PRL_HANDLE_PTR phPortFwdList);

	/**
	 * Set port forward entries list
	 */
	PRL_RESULT SetPortForwardList(PRL_PORT_FORWARDING_TYPE nPortFwdType, PRL_HANDLE hPortFwdList);

	/**
	 * Obtains information of network adapter bound to the virtual network
	 * @param pointer to the host hardware info object
	 * @param pointer to the buffer for storing result
	 * @return result of operation
	 */
	PRL_RESULT GetBoundAdapterInfo(const PrlHandleSrvConfigPtr &pSrvConfig, PRL_HANDLE_PTR phNetAdapter);

private:

	CVirtualNetwork		m_VirtualNetwork;

};


typedef PrlHandleSmartPtr<PrlHandleVirtNet> PrlHandleVirtNetPtr;


#endif	// __PARALLELS_HANDLE_VIRT_NET_H__