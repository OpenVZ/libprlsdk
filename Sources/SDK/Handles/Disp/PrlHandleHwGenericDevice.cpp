/*
 * PrlHandleHwGenericDevice.cpp
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


#include "PrlHandleHwGenericDevice.h"
#include <prlxmlmodel/HostHardwareInfo/CHwGenericDevice.h>

#define CHECK_DEVICE_OBJECT\
	if (!m_pHwGenericDevice)\
		return (PRL_ERR_INVALID_ARG);

PrlHandleHwGenericDevice::PrlHandleHwGenericDevice(const PrlHandleSrvConfigPtr &pSrvConfig, CHwGenericDevice *pDevice)
: PrlHandleBase(PHT_HW_GENERIC_DEVICE), m_pHwGenericDevice(pDevice), m_pSrvConfig(pSrvConfig)
{}

PrlHandleHwGenericDevice::PrlHandleHwGenericDevice(PRL_HANDLE_TYPE _handle_type, const PrlHandleSrvConfigPtr &pSrvConfig,
														CHwGenericDevice *pDevice)
: PrlHandleBase(_handle_type), m_pHwGenericDevice(pDevice), m_pSrvConfig(pSrvConfig)
{}

PrlHandleHwGenericDevice::~PrlHandleHwGenericDevice()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pHwGenericDevice = NULL;
}

PRL_RESULT PrlHandleHwGenericDevice::GetName(PRL_STR sHwGenericDeviceNameBuf, PRL_UINT32_PTR pnHwGenericDeviceNameBufLength)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_DEVICE_OBJECT
	return CopyStringValue(m_pHwGenericDevice->getDeviceName(), sHwGenericDeviceNameBuf, pnHwGenericDeviceNameBufLength);
}

PRL_RESULT PrlHandleHwGenericDevice::GetId(PRL_STR sHwGenericDeviceIdBuf, PRL_UINT32_PTR pnHwGenericDeviceIdBufLength)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_DEVICE_OBJECT
	return CopyStringValue(m_pHwGenericDevice->getDeviceId(), sHwGenericDeviceIdBuf, pnHwGenericDeviceIdBufLength);
}

PRL_RESULT PrlHandleHwGenericDevice::GetDevType(PRL_DEVICE_TYPE_PTR pnDevType)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_DEVICE_OBJECT
	*pnDevType = m_pHwGenericDevice->getDeviceType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwGenericDevice::IsConnectedToVm(PRL_BOOL_PTR pbConnected)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_DEVICE_OBJECT
	*pbConnected = PRL_BOOL(  !m_pHwGenericDevice->getVmUuids().isEmpty() &&
							(m_pHwGenericDevice->getDeviceState() == PGS_CONNECTED_TO_VM ||
							 m_pHwGenericDevice->getDeviceState() == PGS_CONNECTING_TO_VM));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwGenericDevice::GetDeviceState(PRL_GENERIC_DEVICE_STATE_PTR pnDeviceState)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_DEVICE_OBJECT
	*pnDeviceState = m_pHwGenericDevice->getDeviceState();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwGenericDevice::SetDeviceState(PRL_GENERIC_DEVICE_STATE nDeviceState)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_DEVICE_OBJECT
	m_pHwGenericDevice->setDeviceState(nDeviceState);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwGenericDevice::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_DEVICE_OBJECT
	m_pHwGenericDevice->fromString(sXml);
	return PRL_ERR_SUCCESS;
}

QString PrlHandleHwGenericDevice::toString()
{
	SYNCHRO_PARENT_SRV_CONFIG
	if ( ! m_pHwGenericDevice )
	{
		return QString();
	}
	return m_pHwGenericDevice->toString();
}
