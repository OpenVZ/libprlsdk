/*
 * PrlHandleAccessRights.h
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


#ifndef __VIRTUOZZO_HANDLE_ACCESS_RIGHTS_H__
#define __VIRTUOZZO_HANDLE_ACCESS_RIGHTS_H__

#include "PrlHandleBase.h"
#include <prlxmlmodel/VmConfig/CVmSecurity.h>

/**
 * Wrapper for access rights set.
 */
class PrlHandleAccessRights : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * @param reference to VM security configuration part
	 */
	PrlHandleAccessRights ( const CVmSecurity &_vm_security );
	/**
	 * Returns VM security object representation as string
	 */
	virtual QString toString();

public://API helpers
	/**
	 * Checks whether specified permission is allowed.
	 * @param checking permission
	 * @param pointer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT IsAllowed( PRL_ALLOWED_VM_COMMAND nPermission, PRL_BOOL_PTR pbAllowed );
	/**
	 * Returns access to VM for others.
	 * @param pointer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetAccessForOthers( PRL_VM_ACCESS_FOR_OTHERS_PTR pnAccessForOthers );
	/**
	 * Sets access to VM for others.
	 * @param assigning value
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT SetAccessForOthers( PRL_VM_ACCESS_FOR_OTHERS nAccessForOthers );
	/**
	 * Returns VM owner name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOwnerName(PRL_STR sOwnerName, PRL_UINT32_PTR pnOwnerNameBufLength);
	/**
	 * Returns sign whether current session belongs to VM owner user.
	 * @param pointer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT IsCurrentSessionOwner( PRL_BOOL_PTR pbIsOwner );

private:
	/** VM security object */
	CVmSecurity m_VmSecurity;
};
typedef PrlHandleSmartPtr<class PrlHandleAccessRights> PrlHandleAccessRightsPtr;

#endif // __VIRTUOZZO_HANDLE_VM_H__
