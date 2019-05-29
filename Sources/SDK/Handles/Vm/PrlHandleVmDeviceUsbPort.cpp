/*
 * PrlHandleVmDeviceUsbPort.cpp
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


#include "PrlHandleVmDeviceUsbPort.h"
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include <prlxmlmodel/VmConfig/CVmUsbDevice.h>

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


#define CHECK_USB_CONTROLLER_ELEM\
	if (!m_pVmUsbPort)\
		TryToRestoreObjectElement();\
	if (!m_pVmUsbPort)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceUsbPort::PrlHandleVmDeviceUsbPort()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_USB_DEVICE)
{
	m_pVmUsbPort = new CVmUsbDevice;
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstUsbDevices.append(m_pVmUsbPort);
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmUsbPort);
}

PrlHandleVmDeviceUsbPort::PrlHandleVmDeviceUsbPort( const PrlHandleVmPtr &pVm, CVmUsbDevice *pUsbPort )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_USB_DEVICE), m_pVmUsbPort(pUsbPort)
{
	if (!m_pVmUsbPort)
	{
		m_pVmUsbPort = new CVmUsbDevice;
		if(m_pVm)
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstUsbDevices.append(m_pVmUsbPort);
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmUsbPort);
}

PrlHandleVmDeviceUsbPort::~PrlHandleVmDeviceUsbPort()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmUsbPort;
		m_pVmUsbPort = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceUsbPort::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_USB_CONTROLLER_ELEM
	if (PRL_SUCCEEDED(m_pVmUsbPort->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceUsbPort::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmUsbPort)
		return QString();
	return m_pVmUsbPort->toString();
}

PRL_RESULT PrlHandleVmDeviceUsbPort::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstUsbDevices.removeAll(m_pVmUsbPort);
	m_pVmUsbPort = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceUsbPort::GetVmDeviceElem()
{
	if (!m_pVmUsbPort)
		TryToRestoreObjectElement();
	return (m_pVmUsbPort);
}

PRL_RESULT PrlHandleVmDeviceUsbPort::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_USB_CONTROLLER_ELEM
	m_pVmUsbPort->setEmulatedType((PVE::UsbDeviceEmulatedType)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceUsbPort::GetAutoconnectOption(PRL_USB_DEVICE_AUTO_CONNECT_OPTION_PTR pnAutoconnectOption)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_USB_CONTROLLER_ELEM
	*pnAutoconnectOption = m_pVmUsbPort->getAutoconnectDevices();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceUsbPort::SetAutoconnectOption(PRL_USB_DEVICE_AUTO_CONNECT_OPTION nAutoconnectOption)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_USB_CONTROLLER_ELEM
	m_pVmUsbPort->setAutoconnectDevices(nAutoconnectOption);
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVmDeviceUsbPort::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmUsbPort = NULL;
}

void PrlHandleVmDeviceUsbPort::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		//We appologize here that just single USB controller presents at VM
		//configuration
		if (m_pVm->GetVmConfig().getVmHardwareList()->m_lstUsbDevices.size())
		{
			m_pVmUsbPort = m_pVm->GetVmConfig().getVmHardwareList()->m_lstUsbDevices.at(0);
			/* Reregister instance with new pointer */
			PrlControlValidity::RegisterInstance(GetHandle(), m_pVmUsbPort);
		}
	}
}
