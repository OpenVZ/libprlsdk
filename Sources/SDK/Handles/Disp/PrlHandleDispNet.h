/*
 * PrlHandleDispNet.h
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


#ifndef __VIRTUOZZO_HANDLE_DISP_NET_H__
#define __VIRTUOZZO_HANDLE_DISP_NET_H__

#include <prlxmlmodel/DispConfig/CDispNetAdapter.h>

#include "PrlHandleDispConfig.h"
#include "PrlControlValidity.h"

/**
 * Dispatcher net adapter handle object.
 */
class PrlHandleDispNet : public PrlHandleBase, public PrlControlValidity
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 * @param parent dispatcher config object pointer
	 */
	PrlHandleDispNet(const PrlHandleDispConfigPtr &pDispCfg);

	/**
	 * Class constructor.
	 * Initializes net adapter settings from specified object.
	 * @param parent dispatcher config object pointer
	 */
	PrlHandleDispNet(const PrlHandleDispConfigPtr &pDispCfg, class CDispNetAdapter *pDispNet);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleDispNet();

public://API helpers
	/**
	 * Removes net adapter from parent net adapters list.
	 *
	 * @return PRL_RESULT
	 */
	PRL_RESULT Remove();

	/**
	 * Checks whether net adapter enabled.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets net adapter enabled sign.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetEnabled(PRL_BOOL bEnabled);


	/**
	 * Checks whether net adapter hidden.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsHidden(PRL_BOOL_PTR pbHidden);

	/**
	 * Sets net adapter hidden sign.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetHidden(PRL_BOOL bHidden);

	/**
	 * Returns net adapter network type.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetNetworkType(PRL_NET_ADAPTER_EMULATED_TYPE_PTR pnNetworkType);

	/**
	 * Sets net adapter network type.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetNetworkType(PRL_NET_ADAPTER_EMULATED_TYPE nNetworkType);


	/**
	 * Returns network adapter user friendly name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully network adapter name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetName(PRL_STR sDispNetName, PRL_UINT32_PTR pnDispNetNameBufLength);


	/**
	 * Sets new network adapter user friendly name
	 * @param [in] new network adapter user friendly name string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetName(PRL_CONST_STR sNewDispNetName);


	/**
	 * Returns network adapter UUID
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully network adapter UUID)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUuid(PRL_STR sDispNetUuid, PRL_UINT32_PTR pnDispNetUuidBufLength);


	/**
	 * Returns network adapter bint physical adapter name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully network adapter name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSysName(PRL_STR sDispNetSysName, PRL_UINT32_PTR pnDispNetSysNameBufLength);


	/**
	 * Sets new network adapter bint physical adapter name
	 * @param [in] new network adapter user friendly name string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSysName(PRL_CONST_STR sNewDispNetSysName);


	/**
	 * Returns network adapter index.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetIndex(PRL_UINT32_PTR pnIndex);

	/**
	 * Sets network adapter index.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetIndex(PRL_UINT32 nIndex);

	/**
	* Checks whether net adapter DHCP enabled.
	*
	* @param buffer for storing result
	* @return PRL_RESULT
	*/
	PRL_RESULT IsDhcpEnabled(PRL_BOOL_PTR pbDhcpEnabled);

	/**
	* Sets net adapter DHCP enabled sign.
	*
	* @param assigning value
	* @return PRL_RESULT
	*/
	PRL_RESULT SetDhcpEnabled(PRL_BOOL bDhcpEnabled);

	/**
	 * Returns network adapter DHCP scope start IP
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully network adapter name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDhcpScopeStartIp(PRL_STR sDispNetDhcpScopeStartIp, PRL_UINT32_PTR pnDispNetDhcpScopeStartIpBufLength);


	/**
	 * Sets new network adapter DHCP scope start IP
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDhcpScopeStartIp(PRL_CONST_STR sNewDispNetDhcpScopeStartIp);


	/**
	 * Returns network adapter DHCP scope end IP
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully network adapter name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDhcpScopeEndIp(PRL_STR sDispNetDhcpScopeEndIp, PRL_UINT32_PTR pnDispNetDhcpScopeEndIpBufLength);


	/**
	 * Sets new network adapter DHCP scope end IP
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDhcpScopeEndIp(PRL_CONST_STR sNewDispNetDhcpScopeEndIp);


	/**
	 * Returns network adapter DHCP scope mask
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully network adapter name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDhcpScopeMask(PRL_STR sDispNetDhcpScopeMask, PRL_UINT32_PTR pnDispNetDhcpScopeMaskBufLength);


	/**
	 * Sets new network adapter DHCP scope mask
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDhcpScopeMask(PRL_CONST_STR sNewDispNetDhcpScopeMask);

	/**
	* Checks whether net adapter DHCP enabled.
	*
	* @param buffer for storing result
	* @return PRL_RESULT
	*/
	PRL_RESULT IsDhcp6Enabled(PRL_BOOL_PTR pbDhcp6Enabled);

	/**
	* Sets net adapter DHCP enabled sign.
	*
	* @param assigning value
	* @return PRL_RESULT
	*/
	PRL_RESULT SetDhcp6Enabled(PRL_BOOL bDhcp6Enabled);

	/**
	* Returns network adapter DHCP scope start IP
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully network adapter name)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetDhcp6ScopeStartIp(PRL_STR sDispNetDhcp6ScopeStartIp, PRL_UINT32_PTR pnDispNetDhcp6ScopeStartIpBufLength);


	/**
	* Sets new network adapter DHCP scope start IP
	* @param [in] setting value
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_INVALID_ARG - incorrect (empty) string value
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT SetDhcp6ScopeStartIp(PRL_CONST_STR sNewDispNetDhcp6ScopeStartIp);


	/**
	* Returns network adapter DHCP scope end IP
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully network adapter name)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetDhcp6ScopeEndIp(PRL_STR sDispNetDhcp6ScopeEndIp, PRL_UINT32_PTR pnDispNetDhcp6ScopeEndIpBufLength);


	/**
	* Sets new network adapter DHCP scope end IP
	* @param [in] setting value
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_INVALID_ARG - incorrect (empty) string value
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT SetDhcp6ScopeEndIp(PRL_CONST_STR sNewDispNetDhcp6ScopeEndIp);


	/**
	* Returns network adapter DHCP scope mask
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully network adapter name)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetDhcp6ScopeMask(PRL_STR sDispNetDhcp6ScopeMask, PRL_UINT32_PTR pnDispNetDhcp6ScopeMaskBufLength);


	/**
	* Sets new network adapter DHCP scope mask
	* @param [in] setting value
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_INVALID_ARG - incorrect (empty) string value
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT SetDhcp6ScopeMask(PRL_CONST_STR sNewDispNetDhcp6ScopeMask);

	CDispNetAdapter* GetNetAdapter() const { return m_pDispNet; }

	/**
	 * FIXME: #116344
	 * Temporally solution for port forwarding support in Desktop
	 * @return PRL_RESULT. Possible values:
	 */
	PRL_RESULT GetPortForwardingToString( PRL_VOID_PTR_PTR psPortForwarding );

	/**
	 * FIXME: #116344
	 * Temporally solution for port forwarding support in Desktop
	 * @return PRL_RESULT. Possible values:
	 */
	PRL_RESULT SetPortForwardingFromString( PRL_CONST_STR sPortForwarding );

private:
	/// Storing net adapter XML object pointer.
	CDispNetAdapter *m_pDispNet;
	/// Pointer to the parent dispatcher config object
	PrlHandleDispConfigPtr m_pDispCfg;

private:
	/**
	 * Overridden method that marks device object as invalid (sets to NULL storing screen resolution pointer)
	 */
	void MarkAsInvalid();

	/**
	 * Overridden method that should to generate unique hash code for an object
	 * @return generated hash code
	 */
	quint32 ConcreteGenerateHashCode();

private://Internal class helpers
	/**
	 * Tries to restore internal wrapping XML model element
	 */
	void TryToRestoreObjectElement();
};

typedef PrlHandleSmartPtr<PrlHandleDispNet> PrlHandleDispNetPtr;

#endif // __VIRTUOZZO_HANDLE_DISP_NET_H__
