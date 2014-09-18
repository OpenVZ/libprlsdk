/*
 * PrlHandleUsbIdentity.h
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


#ifndef __PARALLELS_HANDLE_USB_IDENTITY_H__
#define __PARALLELS_HANDLE_USB_IDENTITY_H__


#include "PrlHandleBase.h"
#include "XmlModel/DispConfig/CDispUsbIdentity.h"


/**
 * Handle object represents dispatcher usb identity
 */
class PrlHandleUsbIdentity : public PrlHandleBase
{

public:
	PrlHandleUsbIdentity(const QString &sUsbIdentity);
	/**
	 * Get USB device system name (id)
	 * @param [out] buffer for storing result.
	 * @param [in] length of the buffer, [out] required buffer length
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - wrong string pointer, null length pointer
	 * * PRL_ERR_BUFFER_OVERRUN - insufficient buffer size
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSystemName(PRL_STR sSystemName, PRL_UINT32_PTR pnSystemNameBufLength);

	/**
	 * Get USB device friendly name
  * @param [out] buffer for storing result.
  * @param [in] length of the buffer, [out] required buffer length
  * @return PRL_RESULT. Possible values:
  * * PRL_ERR_INVALID_ARG - wrong string pointer, null length pointer
  * * PRL_ERR_BUFFER_OVERRUN - insufficient buffer size
  * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetFriendlyName(PRL_STR sFriendlyName, PRL_UINT32_PTR pnFriendlyNameBufLength);

	/**
	 * Get Uuid of VM USB device is associated with
  * @param [out] buffer for storing result.
  * @param [in] length of the buffer, [out] required buffer length
  * @return PRL_RESULT. Possible values:
  * * PRL_ERR_INVALID_ARG - wrong string pointer, null length pointer
  * * PRL_ERR_BUFFER_OVERRUN - insufficient buffer size
  * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetVmUuidAssociation(PRL_STR sVmUuid, PRL_UINT32_PTR pnVmUuidBufLength);

private:
	// UsbIdentity object
	CDispUsbIdentity m_UsbIdentity;
};

typedef PrlHandleSmartPtr<PrlHandleUsbIdentity> PrlHandleUsbIdentityPtr;

#endif // __PARALLELS_HANDLE_USB_IDENTITY_H__