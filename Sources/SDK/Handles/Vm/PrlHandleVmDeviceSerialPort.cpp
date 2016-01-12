/*
 * PrlHandleVmDeviceSerialPort.cpp
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


#include "PrlHandleVmDeviceSerialPort.h"
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include <prlxmlmodel/VmConfig/CVmSerialPort.h>

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


#define CHECK_SERIAL_PORT_ELEM\
	if (!m_pVmSerialPort)\
		TryToRestoreObjectElement();\
	if (!m_pVmSerialPort)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceSerialPort::PrlHandleVmDeviceSerialPort()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_SERIAL_PORT)
{
	m_pVmSerialPort = new CVmSerialPort;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmSerialPort);
}

PrlHandleVmDeviceSerialPort::PrlHandleVmDeviceSerialPort( const PrlHandleVmPtr &pVm, CVmSerialPort *pSerialPort )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_SERIAL_PORT), m_pVmSerialPort(pSerialPort)
{
	if (!m_pVmSerialPort)
	{
		m_pVmSerialPort = new CVmSerialPort;
		if(m_pVm)
		{
			m_pVmSerialPort->setIndex(CalculateDefaultDevIndex(m_pVm->GetVmConfig().getVmHardwareList()->m_lstSerialPorts));
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstSerialPorts.append(m_pVmSerialPort);
		}
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmSerialPort);
}

PrlHandleVmDeviceSerialPort::~PrlHandleVmDeviceSerialPort()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmSerialPort;
		m_pVmSerialPort = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceSerialPort::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERIAL_PORT_ELEM
	if (PRL_SUCCEEDED(m_pVmSerialPort->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceSerialPort::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmSerialPort)
		return QString();
	return m_pVmSerialPort->toString();
}

PRL_RESULT PrlHandleVmDeviceSerialPort::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstSerialPorts.removeAll(m_pVmSerialPort);
	m_pVmSerialPort = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceSerialPort::GetVmDeviceElem()
{
	if (!m_pVmSerialPort)
		TryToRestoreObjectElement();
	return (m_pVmSerialPort);
}

PRL_RESULT PrlHandleVmDeviceSerialPort::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERIAL_PORT_ELEM
	m_pVmSerialPort->setEmulatedType((PVE::SerialPortEmulatedType)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

CVmClusteredDevice *PrlHandleVmDeviceSerialPort::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmSerialPort)
		TryToRestoreObjectElement();
	if (!m_pVmSerialPort)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return m_pVmSerialPort;
}

CVmPort *PrlHandleVmDeviceSerialPort::GetVmPortDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmSerialPort)
		TryToRestoreObjectElement();
	if (!m_pVmSerialPort)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return m_pVmSerialPort;
}

PRL_RESULT PrlHandleVmDeviceSerialPort::GetSocketMode(PRL_SERIAL_PORT_SOCKET_OPERATION_MODE_PTR pnSocketMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERIAL_PORT_ELEM
	*pnSocketMode = m_pVmSerialPort->getSocketMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceSerialPort::SetSocketMode(PRL_SERIAL_PORT_SOCKET_OPERATION_MODE nSocketMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERIAL_PORT_ELEM
	m_pVmSerialPort->setSocketMode(nSocketMode);
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVmDeviceSerialPort::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmSerialPort = NULL;
}

void PrlHandleVmDeviceSerialPort::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmSerialPort *pSerialPort, m_pVm->GetVmConfig().getVmHardwareList()->m_lstSerialPorts)
		{
			if (GetHashCode() == pSerialPort->getIndex())
			{
				m_pVmSerialPort = pSerialPort;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmSerialPort);
				break;
			}
		}
	}
}
