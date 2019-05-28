/*
 * PrlHandleVmDeviceFloppy.cpp
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


#include "PrlHandleVmDeviceFloppy.h"
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include <prlxmlmodel/VmConfig/CVmFloppyDisk.h>

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

#define CHECK_FLOPPY_ELEMENT\
	if (!m_pVmFloppy)\
		TryToRestoreObjectElement();\
	if (!m_pVmFloppy)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceFloppy::PrlHandleVmDeviceFloppy()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_FLOPPY)
{
	m_pVmFloppy = new CVmFloppyDisk;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmFloppy);
}

PrlHandleVmDeviceFloppy::PrlHandleVmDeviceFloppy( const PrlHandleVmPtr &pVm, CVmFloppyDisk *pFloppy )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_FLOPPY), m_pVmFloppy(pFloppy)
{
	if (!m_pVmFloppy)
	{
		m_pVmFloppy = new CVmFloppyDisk;
		if(m_pVm)
		{
			m_pVmFloppy->setIndex(CalculateDefaultDevIndex(m_pVm->GetVmConfig().getVmHardwareList()->m_lstFloppyDisks));
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstFloppyDisks.append(m_pVmFloppy);
		}
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmFloppy);
}

PrlHandleVmDeviceFloppy::~PrlHandleVmDeviceFloppy()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmFloppy;
		m_pVmFloppy = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceFloppy::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_FLOPPY_ELEMENT
	if (PRL_SUCCEEDED(m_pVmFloppy->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceFloppy::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmFloppy)
		return QString();
	return m_pVmFloppy->toString();
}

PRL_RESULT PrlHandleVmDeviceFloppy::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstFloppyDisks.removeAll(m_pVmFloppy);
	m_pVmFloppy = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceFloppy::GetVmDeviceElem()
{
	if (!m_pVmFloppy)
		TryToRestoreObjectElement();
	return (m_pVmFloppy);
}

PRL_RESULT PrlHandleVmDeviceFloppy::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_FLOPPY_ELEMENT
	m_pVmFloppy->setEmulatedType((PVE::FloppyEmulatedType)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

CVmClusteredDevice *PrlHandleVmDeviceFloppy::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmFloppy)
		TryToRestoreObjectElement();
	if (!m_pVmFloppy)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmFloppy);
}

CVmStorageDevice *PrlHandleVmDeviceFloppy::GetVmStorageDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmFloppy)
		TryToRestoreObjectElement();
	if (!m_pVmFloppy)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmFloppy);
}

void PrlHandleVmDeviceFloppy::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmFloppy = NULL;
}

void PrlHandleVmDeviceFloppy::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmFloppyDisk *pFloppy, m_pVm->GetVmConfig().getVmHardwareList()->m_lstFloppyDisks)
		{
			if (GetHashCode() == pFloppy->getIndex())
			{
				m_pVmFloppy = pFloppy;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmFloppy);
				break;
			}
		}
	}
}
