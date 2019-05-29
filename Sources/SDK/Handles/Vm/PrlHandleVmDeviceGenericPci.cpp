/*
 * PrlHandleVmDeviceGenericPci.cpp
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


#include "PrlHandleVmDeviceGenericPci.h"
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include <prlxmlmodel/VmConfig/CVmGenericPciDevice.h>

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

#define CHECK_GENERIC_PCI_ELEMENT\
	if (!m_pVmGenericPci)\
		TryToRestoreObjectElement();\
	if (!m_pVmGenericPci)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceGenericPci::PrlHandleVmDeviceGenericPci()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_GENERIC_PCI)
{
	m_pVmGenericPci = new CVmGenericPciDevice;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmGenericPci);
}

PrlHandleVmDeviceGenericPci::PrlHandleVmDeviceGenericPci( const PrlHandleVmPtr &pVm, CVmGenericPciDevice *pGenericPci )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_GENERIC_PCI), m_pVmGenericPci(pGenericPci)
{
	if (!m_pVmGenericPci)
	{
		m_pVmGenericPci = new CVmGenericPciDevice;
		if(m_pVm)
		{
			m_pVmGenericPci->setIndex(CalculateDefaultDevIndex(m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericPciDevices));
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericPciDevices.append(m_pVmGenericPci);
		}
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmGenericPci);
}

PrlHandleVmDeviceGenericPci::~PrlHandleVmDeviceGenericPci()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmGenericPci;
		m_pVmGenericPci = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceGenericPci::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_GENERIC_PCI_ELEMENT
	if (PRL_SUCCEEDED(m_pVmGenericPci->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceGenericPci::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmGenericPci)
		return QString();
	return m_pVmGenericPci->toString();
}

PRL_RESULT PrlHandleVmDeviceGenericPci::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericPciDevices.removeAll(m_pVmGenericPci);
	m_pVmGenericPci = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceGenericPci::GetVmDeviceElem()
{
	if (!m_pVmGenericPci)
		TryToRestoreObjectElement();
	return (m_pVmGenericPci);
}

PRL_RESULT PrlHandleVmDeviceGenericPci::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_GENERIC_PCI_ELEMENT
	m_pVmGenericPci->setEmulatedType((PVE::GenericDeviceEmulatedType)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

CVmClusteredDevice *PrlHandleVmDeviceGenericPci::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmGenericPci)
		TryToRestoreObjectElement();
	if (!m_pVmGenericPci)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmGenericPci);
}

void PrlHandleVmDeviceGenericPci::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmGenericPci = NULL;
}

void PrlHandleVmDeviceGenericPci::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmGenericPciDevice *pGenericPci, m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericPciDevices)
		{
			if (GetHashCode() == pGenericPci->getIndex())
			{
				m_pVmGenericPci = pGenericPci;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmGenericPci);
				break;
			}
		}
	}
}
