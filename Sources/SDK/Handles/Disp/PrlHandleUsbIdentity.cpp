/*
 * PrlHandleUsbIdentity.cpp
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


#include "PrlHandleUsbIdentity.h"

#ifdef ENABLE_MALLOC_DEBUG
	// By adding this interface we enable allocations tracing in the module
	#include "Interfaces/Debug.h"
#else
	// We're not allowed to throw exceptions from the library -
	// so we need to prevent operator ::new from doing this
	#include <new>
	using std::nothrow;
	#define new new(nothrow)
#endif


PrlHandleUsbIdentity::PrlHandleUsbIdentity(const QString &sUsbIdentity)
	: PrlHandleBase(PHT_USB_IDENTITY)
{
	m_UsbIdentity.fromString( sUsbIdentity );
}

PRL_RESULT PrlHandleUsbIdentity::GetSystemName(PRL_STR sSystemName, PRL_UINT32_PTR pnSystemNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue( m_UsbIdentity.getSystemName(), sSystemName, pnSystemNameBufLength );
}

PRL_RESULT PrlHandleUsbIdentity::GetFriendlyName(PRL_STR sFriendlyName, PRL_UINT32_PTR pnFriendlyNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString strFriendlyName = m_UsbIdentity.getFriendlyName();
	if( m_UsbIdentity.getIndex() > 1 )
		strFriendlyName += QString(" #%1").arg(m_UsbIdentity.getIndex());
	return CopyStringValue( strFriendlyName, sFriendlyName, pnFriendlyNameBufLength );
}

PRL_RESULT PrlHandleUsbIdentity::GetVmUuidAssociation(PRL_STR sVmUuid, PRL_UINT32_PTR pnVmUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sResult;
	if( !m_UsbIdentity.m_lstAssociations.isEmpty()
	   && m_UsbIdentity.m_lstAssociations[0]->getAction() == PUD_CONNECT_TO_GUEST_OS )
		sResult = m_UsbIdentity.m_lstAssociations[0]->getVmUuid();
	return CopyStringValue( sResult,
						   sVmUuid, pnVmUuidBufLength );
}
