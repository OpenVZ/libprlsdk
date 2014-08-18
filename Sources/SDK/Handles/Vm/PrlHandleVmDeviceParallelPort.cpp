/*
 * PrlHandleVmDeviceParallelPort.cpp
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


#include "PrlHandleVmDeviceParallelPort.h"
#include "XmlModel/VmConfig/CVmConfiguration.h"
#include "XmlModel/VmConfig/CVmParallelPort.h"

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


#define CHECK_PARALLEL_PORT_ELEMENT\
	if (!m_pVmParallelPort)\
		TryToRestoreObjectElement();\
	if (!m_pVmParallelPort)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceParallelPort::PrlHandleVmDeviceParallelPort()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_PARALLEL_PORT)
{
	m_pVmParallelPort = new CVmParallelPort;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmParallelPort);
}

PrlHandleVmDeviceParallelPort::PrlHandleVmDeviceParallelPort( const PrlHandleVmPtr &pVm, CVmParallelPort *pParallelPort )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_PARALLEL_PORT), m_pVmParallelPort(pParallelPort)
{
	if (!m_pVmParallelPort)
	{
		m_pVmParallelPort = new CVmParallelPort;
		if(m_pVm)
		{
			m_pVmParallelPort->setIndex(CalculateDefaultDevIndex(m_pVm->GetVmConfig().getVmHardwareList()->m_lstParallelPorts));
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstParallelPorts.append(m_pVmParallelPort);
		}
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmParallelPort);
}

PrlHandleVmDeviceParallelPort::~PrlHandleVmDeviceParallelPort()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmParallelPort;
		m_pVmParallelPort = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceParallelPort::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_PARALLEL_PORT_ELEMENT
	if (PRL_SUCCEEDED(m_pVmParallelPort->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceParallelPort::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmParallelPort)
		return QString();
	return m_pVmParallelPort->toString();
}

PRL_RESULT PrlHandleVmDeviceParallelPort::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstParallelPorts.removeAll(m_pVmParallelPort);
	m_pVmParallelPort = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceParallelPort::GetVmDeviceElem()
{
	if (!m_pVmParallelPort)
		TryToRestoreObjectElement();
	return (m_pVmParallelPort);
}

PRL_RESULT PrlHandleVmDeviceParallelPort::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_PARALLEL_PORT_ELEMENT
	m_pVmParallelPort->setEmulatedType((PVE::ParallelPortEmulatedType)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

CVmClusteredDevice *PrlHandleVmDeviceParallelPort::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmParallelPort)
		TryToRestoreObjectElement();
	if (!m_pVmParallelPort)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return m_pVmParallelPort;
}

CVmPort *PrlHandleVmDeviceParallelPort::GetVmPortDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmParallelPort)
		TryToRestoreObjectElement();
	if (!m_pVmParallelPort)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return m_pVmParallelPort;
}

void PrlHandleVmDeviceParallelPort::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmParallelPort = NULL;
}

void PrlHandleVmDeviceParallelPort::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmParallelPort *pParallelPort, m_pVm->GetVmConfig().getVmHardwareList()->m_lstParallelPorts)
		{
			if (GetHashCode() == pParallelPort->getIndex())
			{
				m_pVmParallelPort = pParallelPort;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmParallelPort);
				break;
			}
		}
	}
}
