/*
 * PrlHandleVmDeviceHardDrive.cpp
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


#include "PrlHandleVmCfg.h"
#include "PrlHandleVmDeviceHardDrive.h"
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include <prlxmlmodel/VmConfig/CVmHardDisk.h>
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

#define CHECK_HARD_DISK_ELEM\
	if (!m_pVmHardDisk)\
		TryToRestoreObjectElement();\
	if (!m_pVmHardDisk)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceHardDrive::PrlHandleVmDeviceHardDrive()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_HARD_DISK)
{
	m_pVmHardDisk = new CVmHardDisk;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmHardDisk);
}

PrlHandleVmDeviceHardDrive::PrlHandleVmDeviceHardDrive( const PrlHandleVmPtr &pVm, CVmHardDisk *pHardDisk )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_HARD_DISK), m_pVmHardDisk(pHardDisk)
{
	if (!m_pVmHardDisk)
	{
		m_pVmHardDisk = new CVmHardDisk;
		if(m_pVm)
		{
			m_pVmHardDisk->setIndex(CalculateDefaultDevIndex(m_pVm->GetVmConfig().getVmHardwareList()->m_lstHardDisks));
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstHardDisks.append(m_pVmHardDisk);
		}
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmHardDisk);
}

PrlHandleVmDeviceHardDrive::~PrlHandleVmDeviceHardDrive()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmHardDisk;
		m_pVmHardDisk = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceHardDrive::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	if (PRL_SUCCEEDED(m_pVmHardDisk->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceHardDrive::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmHardDisk)
		return QString();
	return m_pVmHardDisk->toString();
}

PRL_RESULT PrlHandleVmDeviceHardDrive::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if(m_pVm)
	{
		foreach(CVmHddPartition* pPartition, m_pVmHardDisk->m_lstPartition)
			PrlControlValidity::MarkAsInvalid(pPartition);
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstHardDisks.removeAll(m_pVmHardDisk);
	}
	m_pVmHardDisk = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceHardDrive::GetVmDeviceElem()
{
	if (!m_pVmHardDisk)
		TryToRestoreObjectElement();
	return (m_pVmHardDisk);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	m_pVmHardDisk->setEmulatedType((PVE::HardDiskEmulatedType)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

CVmClusteredDevice *PrlHandleVmDeviceHardDrive::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmHardDisk)
		TryToRestoreObjectElement();
	if (!m_pVmHardDisk)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return m_pVmHardDisk;
}

CVmStorageDevice *PrlHandleVmDeviceHardDrive::GetVmStorageDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmHardDisk)
		TryToRestoreObjectElement();
	if (!m_pVmHardDisk)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmHardDisk);
}

CVmMassStorageDevice *PrlHandleVmDeviceHardDrive::GetVmMassStorageDeviceElem(PRL_RESULT &nRetCode)
{
	if (!m_pVmHardDisk)
		TryToRestoreObjectElement();
	if (!m_pVmHardDisk)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return (m_pVmHardDisk);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::GetDiskType(PRL_HARD_DISK_INTERNAL_FORMAT_PTR pnDiskType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	*pnDiskType = m_pVmHardDisk->getDiskType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::SetDiskType(PRL_HARD_DISK_INTERNAL_FORMAT nDiskType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	m_pVmHardDisk->setDiskType(nDiskType);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::IsSplitted(PRL_BOOL_PTR pbSplitted)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	*pbSplitted = m_pVmHardDisk->isSplitted();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::SetSplitted(PRL_BOOL bSplitted)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	m_pVmHardDisk->setSplitted(bSplitted);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::GetDiskSize(PRL_UINT32_PTR pnDiskSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	*pnDiskSize = m_pVmHardDisk->getSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::SetDiskSize(PRL_UINT32 nDiskSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	m_pVmHardDisk->setSize(nDiskSize);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::GetBlockSize(PRL_UINT32_PTR pnBytes)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	*pnBytes = m_pVmHardDisk->getBlockSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::SetBlockSize(PRL_UINT32 nBytes)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	m_pVmHardDisk->setBlockSize(nBytes);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::GetSizeOnDisk(PRL_UINT32_PTR pnOnDiskSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	*pnOnDiskSize = m_pVmHardDisk->getSizeOnDisk();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::AddPartition(PRL_HANDLE_PTR phPartition)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM

	PrlHandleVmDeviceHdPart* pPartition = new PrlHandleVmDeviceHdPart(PrlHandleVmDeviceHardDrivePtr(this));

	if (!pPartition)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phPartition = pPartition->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::GetPartitionsCount(PRL_UINT32_PTR pnPartitionsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	*pnPartitionsCount = (PRL_UINT32 )m_pVmHardDisk->m_lstPartition.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::GetPartition(PRL_UINT32 nIndex, PRL_HANDLE_PTR phPartition)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM

	if (nIndex >= (PRL_UINT32 )m_pVmHardDisk->m_lstPartition.size())
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHdPart* pPartition = new PrlHandleVmDeviceHdPart(PrlHandleVmDeviceHardDrivePtr(this),
													m_pVmHardDisk->m_lstPartition.value(nIndex));
	if (!pPartition)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phPartition = pPartition->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::SetMountPoint(PRL_CONST_STR sMountPoint)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM

	m_pVmHardDisk->setMountPoint(sMountPoint);

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::GetMountPoint(PRL_STR sMountPoint,
		PRL_UINT32_PTR pnMountPointBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM

	return CopyStringValue(m_pVmHardDisk->getMountPoint(), sMountPoint, pnMountPointBufLength);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::SetAutoCompressEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM

	m_pVmHardDisk->setAutoCompressEnabled(bEnabled);

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmDeviceHardDrive::IsAutoCompressEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM

	*pbEnabled = m_pVmHardDisk->isAutoCompressEnabled();

	return PRL_ERR_SUCCESS;
}

void PrlHandleVmDeviceHardDrive::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmHardDisk = NULL;
}

void PrlHandleVmDeviceHardDrive::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmHardDisk *pHardDisk, m_pVm->GetVmConfig().getVmHardwareList()->m_lstHardDisks)
		{
			if (GetHashCode() == pHardDisk->getIndex())
			{
				m_pVmHardDisk = pHardDisk;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmHardDisk);
				break;
			}
		}
	}
}

PRL_RESULT PrlHandleVmDeviceHardDrive::GetStorageURL(PRL_STR sURL, PRL_UINT32_PTR pnURLBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	return CopyStringValue(m_pVmHardDisk->getStorageURL().toString(), sURL, pnURLBufLength);
}

PRL_RESULT PrlHandleVmDeviceHardDrive::SetStorageURL(PRL_CONST_STR sURL)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	QUrl url(UTF8_2QSTR(sURL));
	if (!url.isValid())
		return PRL_ERR_INVALID_ARG;
	m_pVmHardDisk->setStorageURL(url);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmDeviceHardDrive::GetSerialNumber(PRL_STR sSerialNumber,
	PRL_UINT32_PTR pnSerialNumberBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_HARD_DISK_ELEM
	return CopyStringValue(m_pVmHardDisk->getSerialNumber(),
		sSerialNumber, pnSerialNumberBufLength);
}
