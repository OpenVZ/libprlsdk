/*
 * PrlHandleNetworkClass.cpp
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

#include <QHostAddress>

#include "SDK/Include/PrlIOStructs.h"
#include "PrlHandleNetworkClass.h"
#include "PrlHandleStringsList.h"

PrlHandleNetworkClass::PrlHandleNetworkClass()
: PrlHandleBase(PHT_NETWORK_CLASS)
{
}

PrlHandleNetworkClass::PrlHandleNetworkClass(QString sNetworkClass)
: PrlHandleBase(PHT_NETWORK_CLASS)
{
	m_NetworkClass.fromString(sNetworkClass);
}

PRL_RESULT PrlHandleNetworkClass::CreateNetworkClass(
		PRL_UINT32 nClassId,
		PRL_HANDLE_PTR phNetworkClass
		)
{
	if (nClassId > PRL_TC_CLASS_MAX)
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkClass* pNetworkClass = new PrlHandleNetworkClass();
	if ( !pNetworkClass )
		return PRL_ERR_OUT_OF_MEMORY;

	pNetworkClass->SetClassId(nClassId);
	*phNetworkClass = pNetworkClass->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkClass::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_NetworkClass.fromString(QString::fromUtf8(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleNetworkClass::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_NetworkClass.toString();
}

PRL_RESULT PrlHandleNetworkClass::GetClassId(PRL_UINT32_PTR pnClassId)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnClassId = m_NetworkClass.getClassId();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkClass::SetClassId(PRL_UINT32 nClassId)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nClassId > PRL_TC_CLASS_MAX)
		return PRL_ERR_INVALID_ARG;
	m_NetworkClass.setClassId(nClassId);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleNetworkClass::GetNetworkList(PRL_HANDLE_PTR phNetworkList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleStringsList *pList = new PrlHandleStringsList(m_NetworkClass.getNetworkList());
	if (pList)
	{
		*phNetworkList = pList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}

	return (PRL_ERR_OUT_OF_MEMORY);
}

static bool isNetworkParamValid(const QString &sNet)
{
	// ipAddress/Mask
	QStringList pair = sNet.split("/");
	if (pair.count() != 2)
		return false;
	bool bOk;
	PRL_UINT32 nMask = pair[1].toUInt(&bOk);
	if (!bOk || nMask > 128)
		return false;

	QString sPAddress(pair[0]);
	QHostAddress addr;
	if (!addr.setAddress(sPAddress))
		return false;

	return true;
}

PRL_RESULT PrlHandleNetworkClass::SetNetworkList(PRL_HANDLE hNetworkList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleStringsListPtr pList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hNetworkList);

	QStringList lst = pList->GetStringsList();
	foreach(QString sNet, lst)
	{
		if (!isNetworkParamValid(sNet))
			return PRL_ERR_INVALID_ARG;
	}

	m_NetworkClass.setNetworkList(lst);

	return (PRL_ERR_SUCCESS);
}
