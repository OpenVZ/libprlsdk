/*
 * PrlHandlePortFwd.cpp
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


#include "PrlHandlePortFwd.h"


PrlHandlePortFwd::PrlHandlePortFwd()
: PrlHandleBase(PHT_PORT_FORWARDING)
{
}

PRL_RESULT PrlHandlePortFwd::Create(PRL_HANDLE_PTR phPortFwd)
{
	PrlHandlePortFwd* pPortFwd = new PrlHandlePortFwd;
	if ( ! pPortFwd )
		return PRL_ERR_OUT_OF_MEMORY;

	*phPortFwd = pPortFwd->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandlePortFwd::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (PRL_SUCCEEDED(m_PortForwardEntry.fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandlePortFwd::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_PortForwardEntry.toString();
}

PRL_RESULT PrlHandlePortFwd::GetRuleName(PRL_STR sRuleName, PRL_UINT32_PTR pnRuleNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_PortForwardEntry.getRuleName(),
							sRuleName, pnRuleNameBufLength);
}

PRL_RESULT PrlHandlePortFwd::SetRuleName(PRL_CONST_STR sRuleName)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_PortForwardEntry.setRuleName(sRuleName);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandlePortFwd::GetIncomingPort(PRL_UINT16_PTR pnIncomingPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnIncomingPort = m_PortForwardEntry.getIncomingPort();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandlePortFwd::SetIncomingPort(PRL_UINT16 nIncomingPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_PortForwardEntry.setIncomingPort(nIncomingPort);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandlePortFwd::GetRedirectIPAddress(PRL_STR sRedirectIPAddress,
												  PRL_UINT32_PTR pnRedirectIPAddressBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_PortForwardEntry.getRedirectIp().toString(),
							sRedirectIPAddress, pnRedirectIPAddressBufLength);
}

PRL_RESULT PrlHandlePortFwd::SetRedirectIPAddress(PRL_CONST_STR sRedirectIPAddress)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QHostAddress addr;
	if (!addr.setAddress(UTF8_2QSTR(sRedirectIPAddress)))
		return PRL_ERR_INVALID_ARG;
	m_PortForwardEntry.setRedirectIp(addr);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandlePortFwd::GetRedirectPort(PRL_UINT16_PTR pnRedirectPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnRedirectPort = m_PortForwardEntry.getRedirectPort();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandlePortFwd::SetRedirectPort(PRL_UINT16 nRedirectPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_PortForwardEntry.setRedirectPort(nRedirectPort);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandlePortFwd::GetRedirectVm(PRL_STR sRedirectVm,
												  PRL_UINT32_PTR pnRedirectVmBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_PortForwardEntry.getRedirectVm(),
							sRedirectVm, pnRedirectVmBufLength);
}

PRL_RESULT PrlHandlePortFwd::SetRedirectVm(PRL_CONST_STR sRedirectVm)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_PortForwardEntry.setRedirectVm(sRedirectVm);
	return PRL_ERR_SUCCESS;
}
