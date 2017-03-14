/*
 * PrlHandleHandlesList.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#include "PrlHandleHandlesList.h"

PrlHandleHandlesList::PrlHandleHandlesList()
: PrlHandleBase(PHT_HANDLES_LIST)
{}

PrlHandleHandlesList::PrlHandleHandlesList(const QList<PrlHandleBasePtr> &lstHandles)
: PrlHandleBase(PHT_HANDLES_LIST), m_HandlesList(lstHandles)
{}

PRL_RESULT PrlHandleHandlesList::GetItemsCount(PRL_UINT32_PTR pnItemsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnItemsCount = m_HandlesList.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHandlesList::AddItem(PrlHandleBasePtr pItemHandle)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_HandlesList.append(pItemHandle);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHandlesList::GetItem(PRL_UINT32 nItemIndex, PRL_HANDLE_PTR phItem)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nItemIndex >= (PRL_UINT32)m_HandlesList.size())
		return (PRL_ERR_INVALID_ARG);
	PrlHandleBasePtr pItemHandle = m_HandlesList.at((int)nItemIndex);
	//Add reference before return handle outside SDK
	pItemHandle->AddRef();
	*phItem = pItemHandle->GetHandle();

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHandlesList::RemoveItem(PRL_UINT32 nItemIndex)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nItemIndex >= (PRL_UINT32)m_HandlesList.size())
		return (PRL_ERR_INVALID_ARG);

	m_HandlesList.removeAt((int)nItemIndex);

	return (PRL_ERR_SUCCESS);
}

QList<PrlHandleBasePtr> PrlHandleHandlesList::GetHandlesList()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return (m_HandlesList);
}

void PrlHandleHandlesList::SetHandlesList(QList<PrlHandleBasePtr> list)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_HandlesList = list;
}
