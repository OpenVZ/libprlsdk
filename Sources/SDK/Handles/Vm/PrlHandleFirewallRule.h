/*
 * PrlHandleFirewallRule.h
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

#ifndef __PARALLELS_HANDLE_FIREWALL_RULE_H__
#define __PARALLELS_HANDLE_FIREWALL_RULE_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/VmConfig/CVmNetFirewallRule.h>


/**
 * Firewall rule of virtual network adapter handle object
 */
class PrlHandleFirewallRule: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleFirewallRule();

	/**
	 * Class initialize constructor.
	 */
	PrlHandleFirewallRule(QString sFirewallRule);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	/**
	 * Creates new rule handle
	 * @param [out] new rule handle
	 */
	static PRL_RESULT Create(PRL_HANDLE_PTR phRule);

	/**
	 * Gets local port
	 * @param [out] local port
	 */
	PRL_RESULT GetLocalPort(PRL_UINT32_PTR pnPort);

	/**
	 * Sets local port
	 * @param local port
	 */
	PRL_RESULT SetLocalPort(PRL_UINT32 nPort);

	/**
	 * Gets remote port
	 * @param [out] remote port
	 */
	PRL_RESULT GetRemotePort(PRL_UINT32_PTR pnPort);

	/**
	 * Sets remote port
	 * @param remote port
	 */
	PRL_RESULT SetRemotePort(PRL_UINT32 nPort);

	/**
	 * Gets protocol
	 * @param [out] protocol
	 */
	PRL_RESULT GetProtocol(PRL_STR sProtocol, PRL_UINT32_PTR pnBufLength);

	/**
	 * Sets protocol
	 * @param protocol
	 */
	PRL_RESULT SetProtocol(PRL_CONST_STR sProtocol);

	/**
	 * Gets local net address
	 * @param [out] local net address
	 */
	PRL_RESULT GetLocalNetAddress(PRL_STR sAddr, PRL_UINT32_PTR pnBufLength);

	/**
	 * Sets local net address
	 * @param local net address
	 */
	PRL_RESULT SetLocalNetAddress(PRL_CONST_STR sAddr);

	/**
	 * Gets remote net address
	 * @param [out] remote net address
	 */
	PRL_RESULT GetRemoteNetAddress(PRL_STR sAddr, PRL_UINT32_PTR pnBufLength);

	/**
	 * Sets remote net address
	 * @param remote net address
	 */
	PRL_RESULT SetRemoteNetAddress(PRL_CONST_STR sAddr);

private:

	CVmNetFirewallRule m_FirewallRule;

};


typedef PrlHandleSmartPtr<PrlHandleFirewallRule> PrlHandleFirewallRulePtr;


#endif	// __PARALLELS_HANDLE_FIREWALL_RULE_H__
