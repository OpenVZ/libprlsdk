/*
 * PrlHandleHwHddPart.cpp
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


#include "PrlHandleHwHddPart.h"
#include <prlxmlmodel/HostHardwareInfo/CHwHardDisk.h>

#define CHECK_HARD_DISK_PARTITION_OBJECT\
	if (!m_pHwHddPart)\
		return (PRL_ERR_INVALID_ARG);

#define SYNCHRO_PARENT_OBJECT QMutexLocker _lock(m_pHardDisk->GetSrvConfig()->GetSynchroObject());

PrlHandleHwHddPart::PrlHandleHwHddPart(const PrlHandleHwHardDiskPtr &pHardDisk, CHwHddPartition *pPartition)
: PrlHandleBase(PHT_HW_HARD_DISK_PARTITION), m_pHwHddPart(pPartition), m_pHardDisk(pHardDisk)
{}

PrlHandleHwHddPart::~PrlHandleHwHddPart()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pHwHddPart = NULL;
}

PRL_RESULT PrlHandleHwHddPart::GetName(PRL_STR sHwHddPartNameBuf, PRL_UINT32_PTR pnHwHddPartNameBufLength)
{
	SYNCHRO_PARENT_OBJECT
	CHECK_HARD_DISK_PARTITION_OBJECT
	return CopyStringValue(m_pHwHddPart->getName(), sHwHddPartNameBuf, pnHwHddPartNameBufLength);
}

PRL_RESULT PrlHandleHwHddPart::GetSysName(PRL_STR sHwHddPartSysNameBuf, PRL_UINT32_PTR pnHwHddPartSysNameBufLength)
{
	SYNCHRO_PARENT_OBJECT
	CHECK_HARD_DISK_PARTITION_OBJECT
	return CopyStringValue(m_pHwHddPart->getSystemName(), sHwHddPartSysNameBuf, pnHwHddPartSysNameBufLength);
}

PRL_RESULT PrlHandleHwHddPart::GetSize(PRL_UINT64_PTR pnPartSize)
{
	SYNCHRO_PARENT_OBJECT
	CHECK_HARD_DISK_PARTITION_OBJECT
	*pnPartSize = m_pHwHddPart->getSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwHddPart::GetIndex(PRL_UINT32_PTR pnIndex)
{
	SYNCHRO_PARENT_OBJECT
	CHECK_HARD_DISK_PARTITION_OBJECT
	*pnIndex = m_pHwHddPart->getIndex();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwHddPart::GetPartType(PRL_UINT32_PTR pnType)
{
	SYNCHRO_PARENT_OBJECT
	CHECK_HARD_DISK_PARTITION_OBJECT
	*pnType = m_pHwHddPart->getType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwHddPart::IsInUse(PRL_BOOL_PTR pbInUse)
{
	SYNCHRO_PARENT_OBJECT
	CHECK_HARD_DISK_PARTITION_OBJECT
	*pbInUse = m_pHwHddPart->getInUse();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwHddPart::IsLogical(PRL_BOOL_PTR pbLogical)
{
	SYNCHRO_PARENT_OBJECT
	CHECK_HARD_DISK_PARTITION_OBJECT
	*pbLogical = m_pHwHddPart->getIsLogical();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleHwHddPart::IsActive(PRL_BOOL_PTR pbActive)
{
	SYNCHRO_PARENT_OBJECT
	CHECK_HARD_DISK_PARTITION_OBJECT
	*pbActive = m_pHwHddPart->getIsActive();
	return (PRL_ERR_SUCCESS);
}
