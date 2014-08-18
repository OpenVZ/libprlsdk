/*
 * PrlHandleSystemStatisticsDisk.cpp
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


#include "PrlHandleSystemStatisticsDisk.h"
#include "PrlHandleSystemStatisticsDiskPart.h"

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


PrlHandleSystemStatisticsDisk::PrlHandleSystemStatisticsDisk(PrlHandleSystemStatisticsPtr pSysStat, CDiskStatistics *pDisk)
: PrlHandleBase(PHT_SYSTEM_STATISTICS_DISK), m_pSysStat(pSysStat), m_pDisk(pDisk)
{}

PRL_RESULT PrlHandleSystemStatisticsDisk::GetUsageDiskSpace(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnSize = m_pDisk->getUsageDiskSpace();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsDisk::GetFreeDiskSpace(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnSize = m_pDisk->getFreeDiskSpace();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsDisk::GetSystemName(PRL_STR sSystemName, PRL_UINT32_PTR pnSystemNameBufLength)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	return CopyStringValue(m_pDisk->getDeviceSystemName(), sSystemName, pnSystemNameBufLength);
}

PRL_RESULT PrlHandleSystemStatisticsDisk::GetPartsStatsCount(PRL_UINT32_PTR pnCount)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnCount = m_pDisk->m_lstPartitionsStatistics.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsDisk::GetPartStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phPartStat)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	if (nIndex >= PRL_UINT32(m_pDisk->m_lstPartitionsStatistics.size()))
		return (PRL_ERR_INVALID_ARG);
	CDiskPartStatistics *pPartStatXml = m_pDisk->m_lstPartitionsStatistics.value(nIndex);
	PrlHandleSystemStatisticsDiskPart *pPartStat = new PrlHandleSystemStatisticsDiskPart(m_pSysStat, pPartStatXml);
	*phPartStat = pPartStat->GetHandle();
	return (PRL_ERR_SUCCESS);
}
