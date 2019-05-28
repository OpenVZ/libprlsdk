/*
 * PrlHandleHandlesList.h
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


#ifndef __PARALLELS_HANDLE_HANDLES_LIST_H__
#define __PARALLELS_HANDLE_HANDLES_LIST_H__


#include "PrlHandleBase.h"
#include <QList>

/**
 * Handles list abstraction object.
 */
class PrlHandleHandlesList : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleHandlesList();

	/**
	 * Class constructor.
	 * @param list of handles for initialization
	 */
	PrlHandleHandlesList(const QList<PrlHandleBasePtr> &lstHandles);

	/** Returns wrapping QList object */
	QList<PrlHandleBasePtr> GetHandlesList();

	/** Sets wrapping QList object */
	void SetHandlesList(QList<PrlHandleBasePtr> list);

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
	 * @param adding handle item
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT AddItem(PrlHandleBasePtr pItemHandle);

	/**
	 * Returns item handles list value.
	 *
	 * @param null based item index
	 * @param buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - specified index is out of range
	 * PRL_ERR_SUCCESS			- operation completed successfully
	 */
	PRL_RESULT GetItem(PRL_UINT32 nItemIndex, PRL_HANDLE_PTR phItem);


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
	/** Wrapping handles list */
	QList<PrlHandleBasePtr> m_HandlesList;
};

typedef PrlHandleSmartPtr<PrlHandleHandlesList> PrlHandleHandlesListPtr;

#endif // __PARALLELS_HANDLE_HANDLES_LIST_H__
