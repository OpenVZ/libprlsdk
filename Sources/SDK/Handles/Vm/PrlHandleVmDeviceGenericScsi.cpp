/*
 * PrlHandleVmDeviceGenericScsi.cpp
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


#include "PrlHandleVmDeviceGenericScsi.h"
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include <prlxmlmodel/VmConfig/CVmGenericScsiDevice.h>

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

#define CHECK_GENERIC_SCSI_ELEMENT\
	if (!m_pVmGenericScsi)\
		TryToRestoreObjectElement();\
	if (!m_pVmGenericScsi)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceGenericScsi::PrlHandleVmDeviceGenericScsi()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_GENERIC_SCSI)
{
	m_pVmGenericScsi = new CVmGenericScsiDevice;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmGenericScsi);
}

PrlHandleVmDeviceGenericScsi::PrlHandleVmDeviceGenericScsi( const PrlHandleVmPtr &pVm, CVmGenericScsiDevice *pGenericScsi )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_GENERIC_SCSI), m_pVmGenericScsi(pGenericScsi)
{
	if (!m_pVmGenericScsi)
	{
		m_pVmGenericScsi = new CVmGenericScsiDevice;
		if(m_pVm)
		{
			m_pVmGenericScsi->setIndex(CalculateDefaultDevIndex(m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericScsiDevices));
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericScsiDevices.append(m_pVmGenericScsi);
		}
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmGenericScsi);
}

PrlHandleVmDeviceGenericScsi::~PrlHandleVmDeviceGenericScsi()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmGenericScsi;
		m_pVmGenericScsi = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceGenericScsi::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_GENERIC_SCSI_ELEMENT
	if (PRL_SUCCEEDED(m_pVmGenericScsi->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceGenericScsi::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmGenericScsi)
		return QString();
	return m_pVmGenericScsi->toString();
}

PRL_RESULT PrlHandleVmDeviceGenericScsi::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericScsiDevices.removeAll(m_pVmGenericScsi);
	m_pVmGenericScsi = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceGenericScsi::GetVmDeviceElem()
{
	if (!m_pVmGenericScsi)
		TryToRestoreObjectElement();
	return (m_pVmGenericScsi);
}

PRL_RESULT PrlHandleVmDeviceGenericScsi::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_GENERIC_SCSI_ELEMENT
	m_pVmGenericScsi->setEmulatedType((PVE::GenericDeviceEmulatedType)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

CVmClusteredDevice *PrlHandleVmDeviceGenericScsi::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmGenericScsi)
		TryToRestoreObjectElement();
	if (!m_pVmGenericScsi)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmGenericScsi);
}

CVmMassStorageDevice *PrlHandleVmDeviceGenericScsi::GetVmMassStorageDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmGenericScsi)
		TryToRestoreObjectElement();
	if (!m_pVmGenericScsi)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmGenericScsi);
}

void PrlHandleVmDeviceGenericScsi::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmGenericScsi = NULL;
}

void PrlHandleVmDeviceGenericScsi::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmGenericScsiDevice *pGenericScsi, m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericScsiDevices)
		{
			if (GetHashCode() == pGenericScsi->getIndex())
			{
				m_pVmGenericScsi = pGenericScsi;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmGenericScsi);
				break;
			}
		}
	}
}
