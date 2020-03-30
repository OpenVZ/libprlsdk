/*
 * PrlHandlePortFwd.h
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

#ifndef __VIRTUOZZO_HANDLE_PORT_FWD_H__
#define __VIRTUOZZO_HANDLE_PORT_FWD_H__

#include "PrlHandleBase.h"
#include <prlxmlmodel/NetworkConfig/CPortForwardEntry.h>


/**
 * Handle object represents port forward entry
 */
class PrlHandlePortFwd : public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandlePortFwd();

	/**
	 * Create new port forward entry
	 */
	static PRL_RESULT Create(PRL_HANDLE_PTR phPortFwd);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	/**
	 * Get rule name
	 */
	PRL_RESULT GetRuleName(PRL_STR sRuleName, PRL_UINT32_PTR pnRuleNameBufLength);

	/**
	 * Set rule name
	 */
	PRL_RESULT SetRuleName(PRL_CONST_STR sRuleName);

	/**
	 * Get incoming port
	 */
	PRL_RESULT GetIncomingPort(PRL_UINT16_PTR pnIncomingPort);

	/**
	 * Set incoming port
	 */
	PRL_RESULT SetIncomingPort(PRL_UINT16 nIncomingPort);

	/**
	 * Get redirect IP address
	 */
	PRL_RESULT GetRedirectIPAddress(PRL_STR sRedirectIPAddress, PRL_UINT32_PTR pnRedirectIPAddressBufLength);

	/**
	 * Set redirect DHCP end IP address
	 */
	PRL_RESULT SetRedirectIPAddress(PRL_CONST_STR sRedirectIPAddress);

	/**
	 * Get redirect port
	 */
	PRL_RESULT GetRedirectPort(PRL_UINT16_PTR pnRedirectPort);

	/**
	 * Set redirect port
	 */
	PRL_RESULT SetRedirectPort(PRL_UINT16 nRedirectPort);

	/**
	 * Get redirect VM
	 */
	PRL_RESULT GetRedirectVm(PRL_STR sRedirectVm, PRL_UINT32_PTR pnRedirectVmBufLength);

	/**
	 * Set redirect VM
	 */
	PRL_RESULT SetRedirectVm(PRL_CONST_STR sRedirectVm);

private:

	CPortForwardEntry	m_PortForwardEntry;

	friend class PrlHandleVirtNet;
};


typedef PrlHandleSmartPtr<PrlHandlePortFwd> PrlHandlePortFwdPtr;


#endif	// __VIRTUOZZO_HANDLE_PORT_FWD_H__
