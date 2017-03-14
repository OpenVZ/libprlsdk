/*
 * PrlHandleAccessRights.cpp
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


#include "PrlHandleAccessRights.h"
#include <QHash>

PrlHandleAccessRights::PrlHandleAccessRights ( const CVmSecurity &_vm_security )
: PrlHandleBase(PHT_ACCESS_RIGHTS), m_VmSecurity(_vm_security)
{}

QString PrlHandleAccessRights::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return (m_VmSecurity.toString());
}

PRL_RESULT PrlHandleAccessRights::IsAllowed( PRL_ALLOWED_VM_COMMAND nPermission, PRL_BOOL_PTR pbAllowed )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbAllowed = (m_VmSecurity.getAccessControlList()->getAccessControl().contains(nPermission) ? PRL_TRUE : PRL_FALSE);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleAccessRights::GetAccessForOthers( PRL_VM_ACCESS_FOR_OTHERS_PTR pnAccessForOthers )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnAccessForOthers = m_VmSecurity.getAccessForOthers();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleAccessRights::SetAccessForOthers( PRL_VM_ACCESS_FOR_OTHERS nAccessForOthers )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmSecurity.setAccessForOthers(nAccessForOthers);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleAccessRights::GetOwnerName(PRL_STR sOwnerNameBuf, PRL_UINT32_PTR pnOwnerNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmSecurity.getOwner(), sOwnerNameBuf, pnOwnerNameBufLength);
}

PRL_RESULT PrlHandleAccessRights::IsCurrentSessionOwner( PRL_BOOL_PTR pbIsOwner )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbIsOwner = (PRL_BOOL)m_VmSecurity.isOwnerPresent();
	return (PRL_ERR_SUCCESS);
}
