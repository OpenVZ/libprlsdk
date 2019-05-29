/*
 * PrlHandleFirewallRule.cpp
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


#include "PrlHandleFirewallRule.h"


PrlHandleFirewallRule::PrlHandleFirewallRule()
: PrlHandleBase(PHT_FIREWALL_RULE)
{
}

PrlHandleFirewallRule::PrlHandleFirewallRule(QString sFirewallRule)
: PrlHandleBase(PHT_FIREWALL_RULE)
{
	m_FirewallRule.fromString(sFirewallRule);
}

PRL_RESULT PrlHandleFirewallRule::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_FirewallRule.fromString(QString::fromUtf8(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleFirewallRule::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_FirewallRule.toString();
}

PRL_RESULT PrlHandleFirewallRule::Create(PRL_HANDLE_PTR phRule)
{
	PrlHandleFirewallRule* pRule = new PrlHandleFirewallRule;
	if ( ! pRule )
		return PRL_ERR_OUT_OF_MEMORY;

	*phRule = pRule->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleFirewallRule::GetLocalPort(PRL_UINT32_PTR pnPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnPort = m_FirewallRule.getLocalPort();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleFirewallRule::SetLocalPort(PRL_UINT32 nPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_FirewallRule.setLocalPort(nPort);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleFirewallRule::GetRemotePort(PRL_UINT32_PTR pnPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnPort = m_FirewallRule.getRemotePort();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleFirewallRule::SetRemotePort(PRL_UINT32 nPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_FirewallRule.setRemotePort(nPort);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleFirewallRule::GetProtocol(PRL_STR sProtocol, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_FirewallRule.getProtocol(), sProtocol, pnBufLength);
}

PRL_RESULT PrlHandleFirewallRule::SetProtocol(PRL_CONST_STR sProtocol)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_FirewallRule.setProtocol(UTF8_2QSTR(sProtocol));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleFirewallRule::GetLocalNetAddress(PRL_STR sAddr, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_FirewallRule.getLocalNetAddress(), sAddr, pnBufLength);
}

PRL_RESULT PrlHandleFirewallRule::SetLocalNetAddress(PRL_CONST_STR sAddr)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_FirewallRule.setLocalNetAddress(UTF8_2QSTR(sAddr));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleFirewallRule::GetRemoteNetAddress(PRL_STR sAddr, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_FirewallRule.getRemoteNetAddress(), sAddr, pnBufLength);
}

PRL_RESULT PrlHandleFirewallRule::SetRemoteNetAddress(PRL_CONST_STR sAddr)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_FirewallRule.setRemoteNetAddress(UTF8_2QSTR(sAddr));
	return PRL_ERR_SUCCESS;
}
