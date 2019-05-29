/*
 * PrlHandleVmDeviceNetAdapter.h
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


#ifndef __PARALLELS_HANDLE_VM_DEVICE_NET_ADAPTER_H__
#define __PARALLELS_HANDLE_VM_DEVICE_NET_ADAPTER_H__


#include "PrlHandleVmDevice.h"

/**
 * Network adapter VM device handle object.
 */
class PrlHandleVmDeviceNetAdapter : public PrlHandleVmDevice
{

public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleVmDeviceNetAdapter();

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param handle to VM which instantiated device belongs
	 * @param [optional] pointer to the input XML configuration
	 */
	PrlHandleVmDeviceNetAdapter( const PrlHandleVmPtr &pVm, class CVmGenericNetworkAdapter *pSerial = 0 );

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 * Made overridable in view of correct child classes instnaces destruction
	 */
	~PrlHandleVmDeviceNetAdapter();

	/**
	 * Initializes VM device object from XML string.
	 *
	 * @param XML representation of VM device
	 * @return PRL_RESULT
	 */
	PRL_RESULT fromString( PRL_CONST_STR sXml );

	/**
	 * Returns VM device representation as XML string.
	 */
	QString toString();

	/**
	 * Overridden method that marks device object as invalid (sets to NULL storing device pointer)
	 */
	void MarkAsInvalid();

public://SDK API helpers set
	/**
	 * Overriden method.
	 * Removes VM device from parent VM devices list.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT ConcreteRemove();

	/**
	 * Overridden method.
	 * Sets VM device emulated type.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType);

	/**
	 * Gets net VM device bound physical adapter index.
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetBoundAdapterIndex(PRL_INT32_PTR pnIndex);

	/**
	 * Sets net VM device bound physical adapter index.
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetBoundAdapterIndex(PRL_INT32 nIndex);

	/**
	 * Returns net VM device bound physical adapter name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully adapter name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetBoundAdapterName(PRL_STR sBoundAdapterName, PRL_UINT32_PTR pnBoundAdapterNameBufLength);


	/**
	 * Sets new net VM device bound physical adapter name
	 * @param [in] new net VM device adapter name
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetBoundAdapterName(PRL_CONST_STR sNewBoundAdapterName);

	/**
	 * Returns VM custom host interface name (if customized)
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHostInterfaceName(PRL_STR sHostInterfaceName, PRL_UINT32_PTR pnHostInterfaceNameBufLength);
	/**
	 * Sets Vm custom host interface name
	 * @param [in] host interface name
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHostInterfaceName(PRL_CONST_STR sNewHostInterfaceName);

	/**
	 * Returns net VM device MAC address/
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * @param bCanonical mac address format canonical
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully MAC address)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetMacAddress(PRL_STR sMacAddress, PRL_UINT32_PTR pnMacAddressBufLength,
			bool bCanonical = false);


	/**
	 * Sets new net VM device MAC address
	 * @param [in] new net VM device MAC address
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetMacAddress(PRL_CONST_STR sNewMacAddress);

	/**
	 * Generates new net VM device MAC address
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - try to use already removed VM device object
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GenerateMacAddress();

	/**
	 * Returns net VM device IP address/
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetIPAddress(PRL_STR sIPAddress, PRL_UINT32_PTR pnIPAddressBufLength);

	/**
	 * Sets net VM device IP address
	 * @param [in] new net VM device IP address
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetIPAddress(PRL_CONST_STR sNewIPAddress);

	/**
	 * Returns net VM device default gateway address
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully default gateway address)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultGateway(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength);


	/**
	 * Sets new net VM device default gateway address
	 * @param [in] new net VM device default gateway address
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDefaultGateway(PRL_CONST_STR sNewDefaultGateway);

	/**
	 * Returns net VM device default gateway address (IPv6)
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully default gateway address)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultGatewayIPv6(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength);

	/**
	 * Sets new net VM device default gateway address (IPv6)
	 * @param [in] new net VM device default gateway address
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDefaultGatewayIPv6(PRL_CONST_STR sNewDefaultGateway);

	/**
	 * Returns net VM device virtual network ID
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully default gateway address)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetVirtualNetworkId(PRL_STR sVirtualNetworkId, PRL_UINT32_PTR pnVirtualNetworkIdBufLength);


	/**
	 * Sets new net VM device virtual nettwork ID
	 * @param [in] new net VM device virtual network ID
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetVirtualNetworkId(PRL_CONST_STR sNewVirtualNetworkId);

	/**
	 * Returns net VM device adapter type
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * non fully default gateway address)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetAdapterType(PRL_VM_NET_ADAPTER_TYPE_PTR pnAdapterType);

	/**
	 * Sets new net VM device adapter type
	 * @param [in] new net VM device AdapterType
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAdapterType(PRL_VM_NET_ADAPTER_TYPE nAdapterType);

	/**
	 * Returns net VM device network addresses list
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings list object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetNetAddresses(PRL_HANDLE_PTR phNetAddressesList);

	/**
	 * Sets net VM device network addresses list
	 * @param [in] new network addresses list
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetNetAddresses(const QStringList &lstNetAddresses);

	/**
	 * Returns net VM device DNS servers list
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings list object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDnsServers(PRL_HANDLE_PTR phDnsServersList);

	/**
	 * Sets net VM device DNS servers list
	 * @param [in] new DNS servers list
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDnsServers(const QStringList &lstDnsServers);

	/**
	 * Returns net VM device search domains list
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings list object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSearchDomains(PRL_HANDLE_PTR phSearchDomainsList);

	/**
	 * Sets net VM device search domains list
	 * @param [in] new search domains list
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSearchDomains(const QStringList &lstSearchDomains);

	/**
	 * Gets net VM device configure with DHCP sign.
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsConfigureWithDhcp(PRL_BOOL_PTR pbConfigureWithDhcp);

	/**
	 * Sets net VM device configure with DHCP sign.
	 *
	 * @param setting value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetConfigureWithDhcp(PRL_BOOL bConfigureWithDhcp);

	/**
	 * Gets net VM device configure with DHCP (IPv6) sign.
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsConfigureWithDhcpIPv6(PRL_BOOL_PTR pbConfigureWithDhcp);

	/**
	 * Sets net VM device configure with DHCP (IPv6) sign.
	 *
	 * @param setting value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetConfigureWithDhcpIPv6(PRL_BOOL bConfigureWithDhcp);

	/**
	 * Gets net VM device automatically applying configuration.
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsAutoApply(PRL_BOOL_PTR pbAutoApply);

	/**
	 * Sets net VM device automatically applying configuration.
	 *
	 * @param setting value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetAutoApply(PRL_BOOL bAutoApply);

	/**
	* Gets net VM device to send only from its own MAC address
	*
	* @param pointer to buffer for storing result
	* @return PRL_RESULT
	*/
	PRL_RESULT IsPktFilterPreventMacSpoof(PRL_BOOL_PTR pbPktFilterPreventMacSpoof);

	/**
	* Sets net VM device to send only from its own MAC address
	*
	* @param setting value
	* @return PRL_RESULT
	*/
	PRL_RESULT SetPktFilterPreventMacSpoof(PRL_BOOL bPktFilterPreventMacSpoof);

	/**
	 * Gets net VM device to prevent receive of not directed to VM packets.
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsPktFilterPreventPromisc(PRL_BOOL_PTR pbPktFilterPreventPromisc);

	/**
	 * Sets net VM device to prevent receive of not directed to VM packets.
	 *
	 * @param setting value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetPktFilterPreventPromisc(PRL_BOOL bPktFilterPreventPromisc);

	/**
	 * Gets net VM device to send only from addresses in the adapter NetAdrresses list
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsPktFilterPreventIpSpoof(PRL_BOOL_PTR pbPktFilterPreventIpSpoof);

	/**
	 * Sets net VM device to send only from addresses in the adapter NetAdrresses list
	 *
	 * @param setting value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetPktFilterPreventIpSpoof(PRL_BOOL bPktFilterPreventIpSpoof);

	/**
	 * Gets state switched on or off of the adapter firewall
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsFirewallEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets state switched on or off of the adapter firewall
	 *
	 * @param setting value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetFirewallEnabled(PRL_BOOL bEnabled);

	/**
	 * Gets an adapter firewall default policy
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetFirewallDefaultPolicy(PRL_FIREWALL_DIRECTION nDirection,
			PRL_FIREWALL_POLICY_PTR pnPolicy);

	/**
	 * Sets state switched on or off of the adapter firewall
	 *
	 * @param setting value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetFirewallDefaultPolicy(PRL_FIREWALL_DIRECTION nDirection,
			PRL_FIREWALL_POLICY nPolicy);

	/**
	 * Gets firewall rule entries list
	 */
	PRL_RESULT GetFirewallRuleList(PRL_FIREWALL_DIRECTION nDirection,
			PRL_HANDLE_PTR phRuleList);

	/**
	 * Sets firewall rule entries list
	 */
	PRL_RESULT SetFirewallRuleList(PRL_FIREWALL_DIRECTION nDirection,
			PRL_HANDLE hRuleList);

private:

	/// Storing VM device XML representation.
	class CVmGenericNetworkAdapter *m_pVmNetAdapter;

private:
	/**
	 * Overridden method that returns pointer to VM device XML model object
	 */
	class CVmDevice *GetVmDeviceElem();
	/**
	 * Overrridden method that returns pointer to VM clustered device XML model object
	 * @param return code that should be correctly filled
	 */
	class CVmClusteredDevice *GetVmClusteredDeviceElem(PRL_RESULT &nRetCode);

private://Internal class helpers
	/**
	 * Tries to restore internal wrapping XML model element
	 */
	void TryToRestoreObjectElement();
};

typedef PrlHandleSmartPtr<PrlHandleVmDeviceNetAdapter> PrlHandleVmDeviceNetAdapterPtr;

#endif // __PARALLELS_HANDLE_VM_DEVICE_NET_ADAPTER_H__
