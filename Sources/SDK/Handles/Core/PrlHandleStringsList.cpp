/*
 * PrlHandleStringsList.cpp
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


#include "PrlHandleStringsList.h"

#include <prlcommon/Interfaces/ParallelsQt.h>

PrlHandleStringsList::PrlHandleStringsList()
: PrlHandleBase(PHT_STRINGS_LIST)
{}

PrlHandleStringsList::PrlHandleStringsList(const QStringList &src)
: PrlHandleBase(PHT_STRINGS_LIST), m_StringsList(src)
{
}

PRL_RESULT PrlHandleStringsList::GetItemsCount(PRL_UINT32_PTR pnItemsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnItemsCount = m_StringsList.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleStringsList::AddItem(PRL_CONST_STR sItem)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_StringsList.append(UTF8_2QSTR(sItem));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleStringsList::GetItem(PRL_UINT32 nItemIndex, PRL_STR sItemBuf, PRL_UINT32_PTR pnItemBufSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nItemIndex >= (PRL_UINT32)m_StringsList.size())
		return (PRL_ERR_INVALID_ARG);

	return (CopyStringValue(m_StringsList[nItemIndex], sItemBuf, pnItemBufSize));
}

PRL_RESULT PrlHandleStringsList::RemoveItem(PRL_UINT32 nItemIndex)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nItemIndex >= (PRL_UINT32)m_StringsList.size())
		return (PRL_ERR_INVALID_ARG);
	m_StringsList.removeAt(nItemIndex);
	return (PRL_ERR_SUCCESS);
}

QStringList PrlHandleStringsList::GetStringsList()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return (m_StringsList);
}
