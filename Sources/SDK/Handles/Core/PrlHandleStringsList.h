/*
 * PrlHandleStringsList.h
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


#ifndef __PARALLELS_HANDLE_STRINGS_LIST_H__
#define __PARALLELS_HANDLE_STRINGS_LIST_H__


#include "PrlHandleBase.h"
#include <QStringList>

/**
 * String list abstraction object.
 */
class PrlHandleStringsList : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleStringsList();

	/**
	 * Class constructor.
	 * Copies content from QStringList.
	 */
	explicit PrlHandleStringsList(const QStringList &src);

	/** Returns wrapping QStringList object */
	QStringList GetStringsList();

public://API helpers
	/**
	 * Returns list items count.
	 * @param pointer to buffer for storing result
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetItemsCount(PRL_UINT32_PTR pnItemsCount);

	/**
	 * Adds new item to the list.
	 *
	 * @param adding string item
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT AddItem(PRL_CONST_STR sItem);

	/**
	 * Returns item strings list value.
	 *
	 * @param null based item index
	 * @param buffer for storing string value
	 * @param pointer to variable storing buffer length
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - specified index is out of range
	 * PRL_ERR_SUCCESS			- operation completed successfully
	 */
	PRL_RESULT GetItem(PRL_UINT32 nItemIndex, PRL_STR sItemBuf, PRL_UINT32_PTR pnItemBufSize);


	/**
	 * Removes specified item from the list.
	 *
	 * @param null based item index
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - specified index is out of range
	 * PRL_ERR_SUCCESS			- operation completed successfully
	 */
	PRL_RESULT RemoveItem(PRL_UINT32 nItemIndex);

private:
	/** Wrapping Qt strings list */
	QStringList m_StringsList;
};

typedef PrlHandleSmartPtr<PrlHandleStringsList> PrlHandleStringsListPtr;

#endif // __PARALLELS_HANDLE_STRINGS_LIST_H__
