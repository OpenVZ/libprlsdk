/*
 * PrlHandleOpTypeList.cpp
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


#include "PrlHandleOpTypeList.h"

PrlHandleOpTypeList *PrlHandleOpTypeList::CreateInstance(PRL_SIZE nTypeSize)
{
	return (new PrlHandleOpTypeList(COpaqueTypeListImpl::CreateInstance(nTypeSize)));
}

PrlHandleOpTypeList::PrlHandleOpTypeList(COpaqueTypeListImpl *pOpTypeListImpl)
: PrlHandleBase(PHT_OPAQUE_TYPE_LIST), m_pOpTypeListImpl(pOpTypeListImpl)
{
}

PrlHandleOpTypeList::~PrlHandleOpTypeList()
{
	delete m_pOpTypeListImpl;
}

PRL_RESULT PrlHandleOpTypeList::GetItemsCount(PRL_UINT32_PTR pnItemsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnItemsCount = m_pOpTypeListImpl->GetItemsCount();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleOpTypeList::AddItem(PRL_CONST_VOID_PTR pnItem)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pOpTypeListImpl->AddItem(pnItem);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleOpTypeList::GetItem(PRL_UINT32 nItemIndex, PRL_VOID_PTR pnItem)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nItemIndex >= m_pOpTypeListImpl->GetItemsCount())
		return (PRL_ERR_INVALID_ARG);

	return (m_pOpTypeListImpl->GetItem(nItemIndex, pnItem));
}

PRL_RESULT PrlHandleOpTypeList::RemoveItem(PRL_UINT32 nItemIndex)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nItemIndex >= (PRL_UINT32)m_pOpTypeListImpl->GetItemsCount())
		return (PRL_ERR_INVALID_ARG);
	m_pOpTypeListImpl->RemoveItem(nItemIndex);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleOpTypeList::GetTypeSize(PRL_SIZE_PTR pnTypeSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnTypeSize = m_pOpTypeListImpl->GetTypeSize();
	return (PRL_ERR_SUCCESS);
}

