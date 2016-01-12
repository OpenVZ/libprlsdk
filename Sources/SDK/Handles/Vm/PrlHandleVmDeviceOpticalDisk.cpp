/*
 * PrlHandleVmDeviceOpticalDisk.cpp: This file is the part of
 * parallels public sdk library.
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


#include "PrlHandleVmDeviceOpticalDisk.h"
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include <prlxmlmodel/VmConfig/CVmOpticalDisk.h>

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


#define CHECK_OPTICAL_DISK_ELEMENT\
	if (!m_pVmOpticalDisk)\
		TryToRestoreObjectElement();\
	if (!m_pVmOpticalDisk)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceOpticalDisk::PrlHandleVmDeviceOpticalDisk()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_OPTICAL_DISK)
{
	m_pVmOpticalDisk = new CVmOpticalDisk;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmOpticalDisk);
}

PrlHandleVmDeviceOpticalDisk::PrlHandleVmDeviceOpticalDisk( const PrlHandleVmPtr &pVm, CVmOpticalDisk *pOpticalDisk )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_OPTICAL_DISK), m_pVmOpticalDisk(pOpticalDisk)
{
	if (!m_pVmOpticalDisk)
	{
		m_pVmOpticalDisk = new CVmOpticalDisk;
		if(m_pVm)
		{
			m_pVmOpticalDisk->setIndex(CalculateDefaultDevIndex(m_pVm->GetVmConfig().getVmHardwareList()->m_lstOpticalDisks));
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstOpticalDisks.append(m_pVmOpticalDisk);
		}
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmOpticalDisk);
}

PrlHandleVmDeviceOpticalDisk::~PrlHandleVmDeviceOpticalDisk()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmOpticalDisk;
		m_pVmOpticalDisk = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceOpticalDisk::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_OPTICAL_DISK_ELEMENT
	if (PRL_SUCCEEDED(m_pVmOpticalDisk->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceOpticalDisk::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmOpticalDisk)
		return QString();
	return m_pVmOpticalDisk->toString();
}

PRL_RESULT PrlHandleVmDeviceOpticalDisk::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstOpticalDisks.removeAll(m_pVmOpticalDisk);
	m_pVmOpticalDisk = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceOpticalDisk::GetVmDeviceElem()
{
	if (!m_pVmOpticalDisk)
		TryToRestoreObjectElement();
	return (m_pVmOpticalDisk);
}

PRL_RESULT PrlHandleVmDeviceOpticalDisk::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_OPTICAL_DISK_ELEMENT
	m_pVmOpticalDisk->setEmulatedType((PVE::CdromEmulatedType)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

CVmClusteredDevice *PrlHandleVmDeviceOpticalDisk::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmOpticalDisk)
		TryToRestoreObjectElement();
	if (!m_pVmOpticalDisk)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return m_pVmOpticalDisk;
}

CVmStorageDevice *PrlHandleVmDeviceOpticalDisk::GetVmStorageDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmOpticalDisk)
		TryToRestoreObjectElement();
	if (!m_pVmOpticalDisk)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmOpticalDisk);
}

CVmMassStorageDevice *PrlHandleVmDeviceOpticalDisk::GetVmMassStorageDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmOpticalDisk)
		TryToRestoreObjectElement();
	if (!m_pVmOpticalDisk)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmOpticalDisk);
}

void PrlHandleVmDeviceOpticalDisk::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmOpticalDisk = NULL;
}

void PrlHandleVmDeviceOpticalDisk::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmOpticalDisk *pOpticalDisk, m_pVm->GetVmConfig().getVmHardwareList()->m_lstOpticalDisks)
		{
			if (GetHashCode() == pOpticalDisk->getIndex())
			{
				m_pVmOpticalDisk = pOpticalDisk;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmOpticalDisk);
				break;
			}
		}
	}
}
