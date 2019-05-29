/*
 * PrlHandleHwGenericPciDevice.h
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


#ifndef __PARALLELS_HANDLE_HW_GENERIC_PCI_DEVICE_H__
#define __PARALLELS_HANDLE_HW_GENERIC_PCI_DEVICE_H__


#include "PrlHandleHwGenericDevice.h"

/**
 * Host hardware info generic PCI device handle object.
 */
class PrlHandleHwGenericPciDevice : public PrlHandleHwGenericDevice
{

public:
	/**
	 * Class constructor.
	 * Initializes net adapter device settings from specified object.
	 * @param parent server config object pointer
	 */
	PrlHandleHwGenericPciDevice(const PrlHandleSrvConfigPtr &pSrvConfig, class CHwGenericPciDevice *pGenericPciDevice);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleHwGenericPciDevice();

public://API helpers
	/**
	 * Returns net adapter type
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDeviceClass(PRL_GENERIC_PCI_DEVICE_CLASS_PTR pnGenericPciClass);

	/**
	 * Returns is primary attribute of device
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsPrimary(PRL_BOOL_PTR pbIsPrimaryDevice);

private:
	/// Storing generic PCI device XML representation.
	class CHwGenericPciDevice *m_pHwGenericPciDevice;
};

typedef PrlHandleSmartPtr<PrlHandleHwGenericPciDevice> PrlHandleHwGenericPciDevicePtr;

#endif // __PARALLELS_HANDLE_HW_GENERIC_PCI_DEVICE_H__

