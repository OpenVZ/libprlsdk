/*
 * PrlHandleGuestOsesMatrix.h
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


#ifndef __PARALLELS_HANDLE_GUEST_OSES_MATRIX_H__
#define __PARALLELS_HANDLE_GUEST_OSES_MATRIX_H__

#include "PrlHandleBase.h"
#include <prlcommon/PrlCommonUtilsBase/CGuestOsesHelper.h>
#include <prlcommon/Messaging/CVmEvent.h>

/**
 * Handle which wrapping guest OSes matrix object received from remote side.
 */
class PrlHandleGuestOsesMatrix : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 * @param pointer to the object for initialization.
	 */
	PrlHandleGuestOsesMatrix(SmartPtr<COsesMatrix> pOsesMatrix);

public://API helpers
	/**
	 * Returns supported guest OSes types list.
	 * @param pointer to buffer for storing result
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate list object
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSupportedOsesTypes(PRL_HANDLE_PTR phOsesTypesList);
	/**
	 * Returns supported guest OSes versions list for specified OS type.
	 * @param requesting guest OS type
	 * @param pointer to buffer for storing result
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate list object
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSupportedOsesVersions(PRL_UINT8 nGuestOsType, PRL_HANDLE_PTR phOsesGuestsList);
	/**
	 * Returns default OS version for specified OS type.
	 * @param requesting guest OS type
	 * @param pointer to buffer for storing result
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultOsVersion(PRL_UINT8 nGuestOsType, PRL_UINT16_PTR pnOsVersion);

	/**
	 * Returns the type of support (PRL_GUEST_OS_SUPPORT_TYPE) for the specified OS type
	 * and OS version.
	 * @param requesting guest OS type
	 * @param requesting guest OS version
	 * @param pointer to buffer for storing result
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - unknown OS type or version specified
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSupportType(PRL_UINT8 nGuestOsType, PRL_UINT16 nOsVersion,
					PRL_GUEST_OS_SUPPORT_TYPE_PTR pnSupportType);

private:
	/** Guest OSes matrix object */
	COsesMatrix m_OsesMatrix;
};

typedef PrlHandleSmartPtr<PrlHandleGuestOsesMatrix> PrlHandleGuestOsesMatrixPtr;

#endif // __PARALLELS_HANDLE_GUEST_OSES_MATRIX_H__

