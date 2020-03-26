/*
 * PrlHandleOpTypeList.h
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


#ifndef __VIRTUOZZO_HANDLE_OP_TYPE_LIST_H__
#define __VIRTUOZZO_HANDLE_OP_TYPE_LIST_H__


#include "PrlHandleBase.h"
#include <prlcommon/PrlCommonUtilsBase/CGuestOsesHelper.h>

/**
 * Opaque type list abstraction object.
 */
class PrlHandleOpTypeList : public PrlHandleBase
{
public:
	/**
	 * Instantiates opaque type list object.
	 * @param size of opaque type which should be wrapped. Note
	 *		  that PRL_RESULT exception will be thrown by function
	 *		  in case if not supported type size will be passed.
	 *		  Proceed with PrlTypes.h in order to determine list
	 *		  of supported opaque types.
	 * @return pointer to the instantiated opaque list object
	 */
	static PrlHandleOpTypeList *CreateInstance(PRL_SIZE nTypeSize);

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 * @param pointer to the opaque type list implementation instance.
	 */
	PrlHandleOpTypeList(COpaqueTypeListImpl *pOpTypeListImpl);

	/** Class destructor */
	~PrlHandleOpTypeList();

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
	 * @param pointer to the adding item
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT AddItem(PRL_CONST_VOID_PTR pnItem);

	/**
	 * Returns item list value.
	 *
	 * @param null based item index
	 * @param buffer for storing value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - specified index is out of range
	 * PRL_ERR_SUCCESS			- operation completed successfully
	 */
	PRL_RESULT GetItem(PRL_UINT32 nItemIndex, PRL_VOID_PTR pnItem);

	/**
	 * Removes specified item from the list.
	 *
	 * @param null based item index
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - specified index is out of range
	 * PRL_ERR_SUCCESS			- operation completed successfully
	 */
	PRL_RESULT RemoveItem(PRL_UINT32 nItemIndex);

	/**
	 * Returns size of wrapping opaque type
	 * @param pointer to the buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS			- operation completed successfully
	 */
	PRL_RESULT GetTypeSize(PRL_SIZE_PTR pnTypeSize);

private:
	/** Implementation of container */
	COpaqueTypeListImpl *m_pOpTypeListImpl;
};


#endif // __VIRTUOZZO_HANDLE_OP_TYPE_LIST_H__

