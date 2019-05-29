/*
 * PrlHandleVmDeviceDisplay.cpp
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


#include "PrlHandleVmDeviceDisplay.h"
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>


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

#define CHECK_DISPLAY_ELEMENT\
	if (!m_pVmVideoAdapter)\
		TryToRestoreObjectElement();\
	if (!m_pVmVideoAdapter)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceDisplay::PrlHandleVmDeviceDisplay()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_DISPLAY)
{
	m_pVmVideoAdapter = new CVmPciVideoAdapter;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmVideoAdapter);
}

PrlHandleVmDeviceDisplay::PrlHandleVmDeviceDisplay( const PrlHandleVmPtr& pVm,
													class CVmPciVideoAdapter* pVideoAdapter)
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_DISPLAY), m_pVmVideoAdapter(pVideoAdapter)
{
	if (!m_pVmVideoAdapter)
	{
		m_pVmVideoAdapter = new CVmPciVideoAdapter;
		if(m_pVm)
		{
			m_pVmVideoAdapter->setIndex(
				CalculateDefaultDevIndex(m_pVm->GetVmConfig().getVmHardwareList()->m_lstPciVideoAdapters));
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstPciVideoAdapters.append(m_pVmVideoAdapter);
		}
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmVideoAdapter);
}

PrlHandleVmDeviceDisplay::~PrlHandleVmDeviceDisplay()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmVideoAdapter;
		m_pVmVideoAdapter = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceDisplay::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_DISPLAY_ELEMENT
	if (PRL_SUCCEEDED(m_pVmVideoAdapter->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceDisplay::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmVideoAdapter)
		return QString();
	return m_pVmVideoAdapter->toString();
}

void PrlHandleVmDeviceDisplay::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmVideoAdapter = NULL;
}

PRL_RESULT PrlHandleVmDeviceDisplay::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstPciVideoAdapters.removeAll(m_pVmVideoAdapter);
	m_pVmVideoAdapter = NULL;
	return PRL_ERR_SUCCESS;
}

CVmDevice* PrlHandleVmDeviceDisplay::GetVmDeviceElem()
{
	if (!m_pVmVideoAdapter)
		TryToRestoreObjectElement();
	return (m_pVmVideoAdapter);
}

CVmClusteredDevice* PrlHandleVmDeviceDisplay::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmVideoAdapter)
		TryToRestoreObjectElement();
	if (!m_pVmVideoAdapter)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmVideoAdapter);
}

void PrlHandleVmDeviceDisplay::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmPciVideoAdapter* pVideoAdapter, m_pVm->GetVmConfig().getVmHardwareList()->m_lstPciVideoAdapters)
		{
			if (GetHashCode() == pVideoAdapter->getIndex())
			{
				m_pVmVideoAdapter = pVideoAdapter;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmVideoAdapter);
				break;
			}
		}
	}
}
