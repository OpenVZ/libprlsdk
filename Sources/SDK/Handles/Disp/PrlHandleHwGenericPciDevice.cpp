/*
 * PrlHandleHwGenericPciDevice.cpp
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


#include "PrlHandleHwGenericPciDevice.h"
#include <prlxmlmodel/HostHardwareInfo/CHwGenericPciDevice.h>

#define CHECK_GENERIC_PCI_OBJECT\
	if (!m_pHwGenericPciDevice)\
		return (PRL_ERR_INVALID_ARG);

PrlHandleHwGenericPciDevice::PrlHandleHwGenericPciDevice(const PrlHandleSrvConfigPtr &pSrvConfig, CHwGenericPciDevice *pDevice)
: PrlHandleHwGenericDevice(PHT_HW_GENERIC_PCI_DEVICE, pSrvConfig, pDevice), m_pHwGenericPciDevice(pDevice)
{}

PrlHandleHwGenericPciDevice::~PrlHandleHwGenericPciDevice()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pHwGenericPciDevice = NULL;
}

PRL_RESULT PrlHandleHwGenericPciDevice::GetDeviceClass(PRL_GENERIC_PCI_DEVICE_CLASS_PTR pnGenericPciClass)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_GENERIC_PCI_OBJECT
	*pnGenericPciClass = m_pHwGenericPciDevice->getType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwGenericPciDevice::IsPrimary(PRL_BOOL_PTR pbIsPrimaryDevice)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_GENERIC_PCI_OBJECT
	*pbIsPrimaryDevice = (PRL_BOOL)m_pHwGenericPciDevice->isPrimary();
	return (PRL_ERR_SUCCESS);
}
