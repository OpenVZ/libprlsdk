/*
 * PrlHandleSystemStatisticsDiskPart.cpp
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


#include "PrlHandleSystemStatisticsDiskPart.h"

PrlHandleSystemStatisticsDiskPart::PrlHandleSystemStatisticsDiskPart(PrlHandleSystemStatisticsPtr pSysStat, CDiskPartStatistics *pPart)
: PrlHandleBase(PHT_SYSTEM_STATISTICS_DISK_PARTITION), m_pSysStat(pSysStat), m_pPart(pPart)
{}

PRL_RESULT PrlHandleSystemStatisticsDiskPart::GetUsageDiskSpace(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnSize = m_pPart->getUsageDiskSpace();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsDiskPart::GetFreeDiskSpace(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnSize = m_pPart->getFreeDiskSpace();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsDiskPart::GetSystemName(PRL_STR sSystemName, PRL_UINT32_PTR pnSystemNameBufLength)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	return CopyStringValue(m_pPart->getDeviceSystemName(), sSystemName, pnSystemNameBufLength);
}
