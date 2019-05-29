/*
 * PrlHandleIPPrivNet.cpp
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


#include "PrlHandleIPPrivNet.h"
#include "PrlHandleStringsList.h"

PrlHandleIPPrivNet::PrlHandleIPPrivNet()
: PrlHandleBase(PHT_IPPRIV_NET)
{
}

PrlHandleIPPrivNet::PrlHandleIPPrivNet(QString sPrivateNetwork)
: PrlHandleBase(PHT_IPPRIV_NET)
{
	m_PrivateNetwork.fromString(sPrivateNetwork);
}

PRL_RESULT PrlHandleIPPrivNet::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_PrivateNetwork.fromString(QString::fromUtf8(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleIPPrivNet::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_PrivateNetwork.toString();
}

PRL_RESULT PrlHandleIPPrivNet::Create(PRL_HANDLE_PTR phPrivNet)
{
	PrlHandleIPPrivNet* pIPPrivNet = new PrlHandleIPPrivNet;
	if ( ! pIPPrivNet )
		return PRL_ERR_OUT_OF_MEMORY;

	*phPrivNet = pIPPrivNet->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleIPPrivNet::GetName(PRL_STR sName, PRL_UINT32_PTR pnNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_PrivateNetwork.getName(), sName, pnNameBufLength);
}

PRL_RESULT PrlHandleIPPrivNet::SetName(PRL_CONST_STR sName)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_PrivateNetwork.setName(sName);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleIPPrivNet::SetNetAddresses(const QStringList &lstNetAddresses)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_PrivateNetwork.setNetAddresses(lstNetAddresses);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleIPPrivNet::GetNetAddresses(PRL_HANDLE_PTR phNetAddresses)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleStringsList *pNetAddressesList = new PrlHandleStringsList(m_PrivateNetwork.getNetAddresses());
	if (pNetAddressesList)
	{
		*phNetAddresses = pNetAddressesList->GetHandle();
		return PRL_ERR_SUCCESS;
	}

	return PRL_ERR_OUT_OF_MEMORY;
}

PRL_RESULT PrlHandleIPPrivNet::IsGlobal(PRL_BOOL_PTR pbGlobal)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbGlobal = PRL_BOOL(m_PrivateNetwork.isGlobal());
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleIPPrivNet::SetGlobal(PRL_BOOL bGlobal)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_PrivateNetwork.setGlobal((bool)bGlobal);
	return PRL_ERR_SUCCESS;
}
