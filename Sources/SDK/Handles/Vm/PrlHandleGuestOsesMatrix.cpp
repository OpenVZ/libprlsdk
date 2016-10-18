/*
 * PrlHandleGuestOsesMatrix.cpp
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


#include "PrlHandleGuestOsesMatrix.h"
#include "PrlHandleOpTypeList.h"

PrlHandleGuestOsesMatrix::PrlHandleGuestOsesMatrix(SmartPtr<COsesMatrix> pOsesMatrix)
: PrlHandleBase(PHT_GUEST_OSES_MATRIX), m_OsesMatrix(*pOsesMatrix.getImpl())
{
}

PRL_RESULT PrlHandleGuestOsesMatrix::GetSupportedOsesTypes(PRL_HANDLE_PTR phOsesTypesList)
{
	TOpaqueTypeList<PRL_UINT8> *pOpaqueTypeList = new TOpaqueTypeList<PRL_UINT8>(m_OsesMatrix.GetSupportedOsesTypes());
	if (!pOpaqueTypeList)
		return (PRL_ERR_OUT_OF_MEMORY);

	PrlHandleOpTypeList *pOpaqueTypeHandle = new PrlHandleOpTypeList(pOpaqueTypeList);
	if (!pOpaqueTypeHandle)
	{
		delete pOpaqueTypeList;
		return (PRL_ERR_OUT_OF_MEMORY);
	}

	*phOsesTypesList = pOpaqueTypeHandle->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleGuestOsesMatrix::GetSupportedOsesVersions(PRL_UINT8 nGuestOsType, PRL_HANDLE_PTR phOsesGuestsList)
{
	if (!CGuestOsesHelper::CheckGuestOsType(nGuestOsType))
		return (PRL_ERR_INVALID_ARG);

	TOpaqueTypeList<PRL_UINT16> *pOpaqueTypeList = new TOpaqueTypeList<PRL_UINT16>(m_OsesMatrix.GetSupportedOsesVersions(nGuestOsType));
	if (!pOpaqueTypeList)
		return (PRL_ERR_OUT_OF_MEMORY);

	PrlHandleOpTypeList *pOpaqueTypeHandle = new PrlHandleOpTypeList(pOpaqueTypeList);
	if (!pOpaqueTypeHandle)
	{
		delete pOpaqueTypeList;
		return (PRL_ERR_OUT_OF_MEMORY);
	}

	*phOsesGuestsList = pOpaqueTypeHandle->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleGuestOsesMatrix::GetDefaultOsVersion(PRL_UINT8 nGuestOsType, PRL_UINT16_PTR pnOsVersion)
{
	if (!CGuestOsesHelper::CheckGuestOsType(nGuestOsType))
		return (PRL_ERR_INVALID_ARG);

	*pnOsVersion = m_OsesMatrix.GetDefaultOsVersion(nGuestOsType);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleGuestOsesMatrix::GetSupportType(PRL_UINT8 nGuestOsType, PRL_UINT16 nOsVersion,
							PRL_GUEST_OS_SUPPORT_TYPE_PTR pnSupportType)
{
	if (!CGuestOsesHelper::CheckGuestOsVersion(nGuestOsType, nOsVersion))
		return (PRL_ERR_INVALID_ARG);

	*pnSupportType = CGuestOsesHelper::GetSupportType(nOsVersion);
	return (PRL_ERR_SUCCESS);
}

