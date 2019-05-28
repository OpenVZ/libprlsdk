/*
 * PrlHandleSystemStatistics.cpp
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


#include "PrlHandleSystemStatistics.h"
#include "PrlHandleSystemStatisticsCpu.h"
#include "PrlHandleSystemStatisticsIface.h"
#include "PrlHandleSystemStatisticsUser.h"
#include "PrlHandleSystemStatisticsDisk.h"
#include "PrlHandleSystemStatisticsProc.h"
#include "PrlHandleSystemStatisticsVmData.h"

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


PrlHandleSystemStatistics::PrlHandleSystemStatistics(const QString& sSystemStatistics)
: PrlHandleBase(PHT_SYSTEM_STATISTICS), m_SystemStatistics(sSystemStatistics)
{}

PrlHandleSystemStatistics::PrlHandleSystemStatistics(QDataStream &_data_stream)
: PrlHandleBase(PHT_SYSTEM_STATISTICS)
{
	m_SystemStatistics.Deserialize(_data_stream);
}

PRL_RESULT PrlHandleSystemStatistics::GetTotalRamSize(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSize = m_SystemStatistics.getMemoryStatistics()->getTotalSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetUsageRamSize(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSize = m_SystemStatistics.getMemoryStatistics()->getUsageSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetFreeRamSize(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSize = m_SystemStatistics.getMemoryStatistics()->getFreeSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetRealRamSize(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSize = m_SystemStatistics.getMemoryStatistics()->getRealSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetTotalSwapSize(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSize = m_SystemStatistics.getSwapStatistics()->getTotalSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetUsageSwapSize(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSize = m_SystemStatistics.getSwapStatistics()->getUsageSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetFreeSwapSize(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSize = m_SystemStatistics.getSwapStatistics()->getFreeSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetOsUptime(PRL_UINT64_PTR pnUptime)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnUptime = m_SystemStatistics.getUptimeStatistics()->getOsUptime();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetDispUptime(PRL_UINT64_PTR pnUptime)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnUptime = m_SystemStatistics.getUptimeStatistics()->getDispatcherUptime();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetCpusStatsCount(PRL_UINT32_PTR pnCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCount = m_SystemStatistics.m_lstCpusStatistics.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetCpuStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phCpuStat)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_SystemStatistics.m_lstCpusStatistics.size()))
		return (PRL_ERR_INVALID_ARG);
	CCpuStatistics *pCpuStatXml = m_SystemStatistics.m_lstCpusStatistics.value(nIndex);
	PrlHandleSystemStatisticsCpu *pCpuStat = new PrlHandleSystemStatisticsCpu(PrlHandleSystemStatisticsPtr(this), pCpuStatXml);
	*phCpuStat = pCpuStat->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetIfacesStatsCount(PRL_UINT32_PTR pnCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCount = m_SystemStatistics.m_lstNetIfacesStatistics.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetIfaceStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phIfaceStat)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_SystemStatistics.m_lstNetIfacesStatistics.size()))
		return (PRL_ERR_INVALID_ARG);
	CNetIfaceStatistics *pIfaceStatXml = m_SystemStatistics.m_lstNetIfacesStatistics.value(nIndex);
	PrlHandleSystemStatisticsIface *pIfaceStat = new PrlHandleSystemStatisticsIface(PrlHandleSystemStatisticsPtr(this), pIfaceStatXml);
	*phIfaceStat = pIfaceStat->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetUsersStatsCount(PRL_UINT32_PTR pnCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCount = m_SystemStatistics.m_lstUsersStatistics.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetUserStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phUserStat)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_SystemStatistics.m_lstUsersStatistics.size()))
		return (PRL_ERR_INVALID_ARG);
	CUserStatistics *pUserStatXml = m_SystemStatistics.m_lstUsersStatistics.value(nIndex);
	PrlHandleSystemStatisticsUser *pUserStat = new PrlHandleSystemStatisticsUser(PrlHandleSystemStatisticsPtr(this), pUserStatXml);
	*phUserStat = pUserStat->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetDisksStatsCount(PRL_UINT32_PTR pnCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCount = m_SystemStatistics.m_lstDisksStatistics.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetDiskStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phDiskStat)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_SystemStatistics.m_lstDisksStatistics.size()))
		return (PRL_ERR_INVALID_ARG);
	CDiskStatistics *pDiskStatXml = m_SystemStatistics.m_lstDisksStatistics.value(nIndex);
	PrlHandleSystemStatisticsDisk *pDiskStat = new PrlHandleSystemStatisticsDisk(PrlHandleSystemStatisticsPtr(this), pDiskStatXml);
	*phDiskStat = pDiskStat->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetProcsStatsCount(PRL_UINT32_PTR pnCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCount = m_SystemStatistics.m_lstProcessesStatistics.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetProcStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phProcStat)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_SystemStatistics.m_lstProcessesStatistics.size()))
		return (PRL_ERR_INVALID_ARG);
	CProcInfoStatistics *pProcStatXml = m_SystemStatistics.m_lstProcessesStatistics.value(nIndex);
	PrlHandleSystemStatisticsProc *pProcStat = new PrlHandleSystemStatisticsProc(PrlHandleSystemStatisticsPtr(this), pProcStatXml);
	*phProcStat = pProcStat->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatistics::GetVmDataStat(PRL_HANDLE_PTR phVmDataStat)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleSystemStatisticsVmData *pVmDataStat
		= new PrlHandleSystemStatisticsVmData(PrlHandleSystemStatisticsPtr(this),
											  m_SystemStatistics.m_VmDataStatistic);
	*phVmDataStat = pVmDataStat->GetHandle();
	return (PRL_ERR_SUCCESS);
}
