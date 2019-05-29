/*
 * PrlHandleVmDeviceHdPart.cpp
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


#include "PrlHandleVmDeviceHdPart.h"


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


#define CHECK_PARTITION_ELEM \
	if (!m_pVmHddPartition) \
		TryToRestoreObjectElement(); \
	if (!m_pVmHddPartition) \
		return PRL_ERR_OBJECT_WAS_REMOVED;


PrlHandleVmDeviceHdPart::PrlHandleVmDeviceHdPart()
: PrlHandleBase(PHT_VIRTUAL_DEV_HD_PARTITION), PrlControlValidity(GetHandle()), m_pVmHddPartition(0)
{
	m_pVmHddPartition = new CVmHddPartition;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmHddPartition);
}

PrlHandleVmDeviceHdPart::PrlHandleVmDeviceHdPart(const PrlHandleVmDeviceHardDrivePtr& pHardDrive,
												 class CVmHddPartition* pPartition)
: PrlHandleBase(PHT_VIRTUAL_DEV_HD_PARTITION), PrlControlValidity(GetHandle()),
  m_pHardDrive(pHardDrive),
  m_pVmHddPartition(pPartition)
{
	if (!m_pVmHddPartition)
	{
		m_pVmHddPartition = new CVmHddPartition;
		if (m_pHardDrive)
		{
			m_pHardDrive->GetHardDiskPtr()->m_lstPartition.append(m_pVmHddPartition);
		}
	}
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmHddPartition);
}

PrlHandleVmDeviceHdPart::~PrlHandleVmDeviceHdPart()
{
	if (!m_pHardDrive.getHandle())	// Seems we have an partition object that wasn't bound to hard drive
	{
		delete m_pVmHddPartition;
		m_pVmHddPartition = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceHdPart::Remove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_PARTITION_ELEM

	if (m_pHardDrive)
		m_pHardDrive->GetHardDiskPtr()->m_lstPartition.removeAll(m_pVmHddPartition);

	PrlControlValidity::MarkAsInvalid(m_pVmHddPartition);

	delete m_pVmHddPartition;
	m_pVmHddPartition = NULL;

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHdPart::GetSysName(PRL_STR sSysName, PRL_UINT32_PTR pnSysNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_PARTITION_ELEM
	return CopyStringValue(m_pVmHddPartition->getSystemName(), sSysName, pnSysNameBufLength);
}

PRL_RESULT PrlHandleVmDeviceHdPart::SetSysName(PRL_CONST_STR sSysName)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_PARTITION_ELEM
	m_pVmHddPartition->setSystemName(UTF8_2QSTR(sSysName));
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVmDeviceHdPart::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmHddPartition = NULL;
}

quint32 PrlHandleVmDeviceHdPart::ConcreteGenerateHashCode()
{
	return (quint32 )qHash(m_pVmHddPartition ? m_pVmHddPartition->getSystemName() : "");
}

void PrlHandleVmDeviceHdPart::TryToRestoreObjectElement()
{
	if (m_pHardDrive && m_pHardDrive->GetHardDiskPtr())
	{
		foreach(CVmHddPartition* pPartition, m_pHardDrive->GetHardDiskPtr()->m_lstPartition)
		{
			if (GetHashCode() == (quint32 )qHash(pPartition->getSystemName()))
			{
				m_pVmHddPartition = pPartition;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmHddPartition);
				break;
			}
		}
	}
}
