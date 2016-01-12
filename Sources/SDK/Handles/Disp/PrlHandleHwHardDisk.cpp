/*
 * PrlHandleHwHardDisk.cpp
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


#include "PrlHandleHwHardDisk.h"
#include <prlxmlmodel/HostHardwareInfo/CHwHardDisk.h>
#include "PrlHandleHwHddPart.h"

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


#define CHECK_HARD_DISK_OBJECT\
	if (!m_pHwHardDisk)\
		return (PRL_ERR_INVALID_ARG);

PrlHandleHwHardDisk::PrlHandleHwHardDisk(const PrlHandleSrvConfigPtr &pSrvConfig, CHwHardDisk *pDevice)
: PrlHandleBase(PHT_HW_HARD_DISK), m_pHwHardDisk(pDevice), m_pSrvConfig(pSrvConfig)
{}

PrlHandleHwHardDisk::~PrlHandleHwHardDisk()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pHwHardDisk = NULL;
}

PRL_RESULT PrlHandleHwHardDisk::GetDevName(PRL_STR sHwHardDiskNameBuf, PRL_UINT32_PTR pnHwHardDiskNameBufLength)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_HARD_DISK_OBJECT
	return CopyStringValue(m_pHwHardDisk->getDeviceName(), sHwHardDiskNameBuf, pnHwHardDiskNameBufLength);
}

PRL_RESULT PrlHandleHwHardDisk::GetDevId(PRL_STR sHwHardDiskIdBuf, PRL_UINT32_PTR pnHwHardDiskIdBufLength)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_HARD_DISK_OBJECT
	return CopyStringValue(m_pHwHardDisk->getDeviceId(), sHwHardDiskIdBuf, pnHwHardDiskIdBufLength);
}

PRL_RESULT PrlHandleHwHardDisk::GetDevSize(PRL_UINT64_PTR pnDevSize)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_HARD_DISK_OBJECT
	*pnDevSize = m_pHwHardDisk->getDeviceSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwHardDisk::GetDiskIndex(PRL_UINT32_PTR pnDiskIndex)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_HARD_DISK_OBJECT
	*pnDiskIndex = m_pHwHardDisk->getDiskIndex();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwHardDisk::GetPartsCount(PRL_UINT32_PTR pnPartsCount)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_HARD_DISK_OBJECT

	QList<CHwHddPartition* > lstFullPartList = m_pHwHardDisk->getAllPartitions(m_pHwHardDisk->m_lstPartitions);

	*pnPartsCount = lstFullPartList.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwHardDisk::GetPart(PRL_UINT32 nIndex, PRL_HANDLE_PTR phPart)
{
	SYNCHRO_PARENT_SRV_CONFIG
	CHECK_HARD_DISK_OBJECT

	QList<CHwHddPartition* > lstFullPartList = m_pHwHardDisk->getAllPartitions(m_pHwHardDisk->m_lstPartitions);

	if (nIndex >= PRL_UINT32(lstFullPartList.size()))
		return (PRL_ERR_INVALID_ARG);
	PrlHandleHwHddPart *pPartition = new PrlHandleHwHddPart(PrlHandleHwHardDiskPtr(this),
															lstFullPartList.value(nIndex));
	if (!pPartition)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phPart = pPartition->GetHandle();
	return (PRL_ERR_SUCCESS);
}
