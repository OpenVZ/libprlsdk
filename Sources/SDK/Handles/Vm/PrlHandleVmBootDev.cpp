/*
 * PrlHandleVmBootDev.cpp
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


#include "PrlHandleVmBootDev.h"
#include <prlxmlmodel/VmConfig/CVmStartupOptions.h>

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

#define BOOT_DEV_HASH_STRING_FORMAT "devtype: %1 index: %2"

#define CHECK_BOOT_DEV_OBJECT\
	if (!m_pVmBootDev)\
		TryToRestoreObjectElement();\
	if (!m_pVmBootDev)\
			return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmBootDev::PrlHandleVmBootDev(const PrlHandleVmPtr &pVm)
: PrlHandleBase(PHT_BOOT_DEVICE), PrlControlValidity(GetHandle()), m_pVm(pVm)
{
	m_pVmBootDev = new CVmStartupOptions::CVmBootDevice;
	SYNCHRO_PARENT_VM_CONFIG
	m_pVm->GetVmConfig().getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList.append(m_pVmBootDev);
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmBootDev);
}

PrlHandleVmBootDev::PrlHandleVmBootDev(const PrlHandleVmPtr &pVm, CVmStartupOptions::CVmBootDevice *pBootDev)
: PrlHandleBase(PHT_BOOT_DEVICE), PrlControlValidity(GetHandle()), m_pVmBootDev(pBootDev), m_pVm(pVm)
{
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmBootDev);
}

PrlHandleVmBootDev::~PrlHandleVmBootDev()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmBootDev = NULL;
}

PRL_RESULT PrlHandleVmBootDev::Remove()
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_BOOT_DEV_OBJECT
	CVmStartupOptions::CVmBootDevice *pVmBootDev = m_pVmBootDev;
	m_pVm->GetVmConfig().getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList.removeAll(pVmBootDev);
	PrlControlValidity::MarkAsInvalid(pVmBootDev);
	delete pVmBootDev;
	m_pVmBootDev = NULL;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmBootDev::IsInUse(PRL_BOOL_PTR pbInUse)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_BOOT_DEV_OBJECT
	*pbInUse = (PRL_BOOL)m_pVmBootDev->inUseStatus;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmBootDev::SetInUse(PRL_BOOL bInUse)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_BOOT_DEV_OBJECT
	m_pVmBootDev->inUseStatus = (bool)bInUse;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmBootDev::GetIndex(PRL_UINT32_PTR pnDevIndex)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_BOOT_DEV_OBJECT
	*pnDevIndex = m_pVmBootDev->deviceIndex;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmBootDev::SetIndex(PRL_UINT32 nDevIndex)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_BOOT_DEV_OBJECT
	m_pVmBootDev->deviceIndex = nDevIndex;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmBootDev::GetSequenceIndex(PRL_UINT32_PTR pnSequenceIndex)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_BOOT_DEV_OBJECT
	*pnSequenceIndex = m_pVmBootDev->sequenceNumber;
	return (PRL_ERR_SUCCESS);
}

static bool bootingNumberLessThan(const CVmBootDeviceBase* pBD1, const CVmBootDeviceBase* pBD2)
{
	return pBD1->getBootingNumber() < pBD2->getBootingNumber();
}

PRL_RESULT PrlHandleVmBootDev::SetSequenceIndex(PRL_UINT32 nSequenceIndex)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_BOOT_DEV_OBJECT
	m_pVmBootDev->sequenceNumber = nSequenceIndex;

	BootingOrder* pBO = m_pVm->GetVmConfig()
							.getVmSettings()->getVmStartupOptions()->getBootingOrder();
	qSort(pBO->m_lstBootDevice.begin(),
		  pBO->m_lstBootDevice.end(),
		  bootingNumberLessThan);

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmBootDev::GetBootDevType(PRL_DEVICE_TYPE_PTR pnType)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_BOOT_DEV_OBJECT
	*pnType = m_pVmBootDev->deviceType;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmBootDev::SetType(PRL_DEVICE_TYPE nType)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_BOOT_DEV_OBJECT
	m_pVmBootDev->deviceType = nType;
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVmBootDev::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmBootDev = NULL;
}

quint32 PrlHandleVmBootDev::ConcreteGenerateHashCode()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return (qHash(m_pVmBootDev ?\
		QString(BOOT_DEV_HASH_STRING_FORMAT).arg(m_pVmBootDev->deviceType).arg(m_pVmBootDev->deviceIndex)\
		: QString("")));
}

void PrlHandleVmBootDev::TryToRestoreObjectElement()
{
	Q_ASSERT(!m_pVmBootDev);
	Q_ASSERT(m_pVm);
	foreach(CVmStartupOptions::CVmBootDevice *pBootDev, m_pVm->GetVmConfig().getVmSettings()->
									getVmStartupOptions()->m_lstBootDeviceList)
	{
		if (GetHashCode() == qHash(QString(BOOT_DEV_HASH_STRING_FORMAT).arg(pBootDev->deviceType).
									arg(pBootDev->deviceIndex)))
		{
			m_pVmBootDev = pBootDev;
			/* Reregister instance with new pointer */
			PrlControlValidity::RegisterInstance(GetHandle(), m_pVmBootDev);
			break;
		}
	}
}
