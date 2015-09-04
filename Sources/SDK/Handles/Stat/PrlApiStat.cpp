/*
 * PrlApiStat.cpp
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


//#define FORCE_LOGGING_ON
//#define FORCE_LOGGING_LEVEL	DBG_DEBUG

#include "SDK/Include/Parallels.h"

#include "Libraries/PrlCommonUtilsBase/PrlStringifyConsts.h"
#include "Libraries/Std/PrlAssert.h"
#include "Libraries/Logging/Logging.h"

#include "PrlCommon.h"
#include "PrlHandleServerStat.h"
#include "PrlHandleSystemStatistics.h"
#include "PrlHandleSystemStatisticsCpu.h"
#include "PrlHandleSystemStatisticsIface.h"
#include "PrlHandleSystemStatisticsUser.h"
#include "PrlHandleSystemStatisticsDisk.h"
#include "PrlHandleSystemStatisticsDiskPart.h"
#include "PrlHandleSystemStatisticsProc.h"

#include "Build/Current.ver"

#include "PrlContextSwitcher.h"
#include "PrlNew.h"

////////////////////////////////////////////////////////////////////////////////////////////////

PRL_HANDLE PrlSrv_GetStatistics_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_STAT_METH_IMPLEMENTATION(GetStatistics, PJOC_SRV_GET_STATISTICS)
}

PRL_ASYNC_METHOD( PrlSrv_GetStatistics ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_STATISTICS)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetStatistics, (hServer))
}

PRL_HANDLE PrlSrv_SubscribeToHostStatistics_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_STAT_METH_IMPLEMENTATION(SubscribeToHostStatistics, PJOC_SRV_SUBSCRIBE_TO_HOST_STATISTICS)
}

PRL_ASYNC_METHOD( PrlSrv_SubscribeToHostStatistics ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_SUBSCRIBE_TO_HOST_STATISTICS)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_SubscribeToHostStatistics, (hServer))
}

PRL_HANDLE PrlSrv_UnsubscribeFromHostStatistics_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_STAT_METH_IMPLEMENTATION(UnsubscribeFromHostStatistics, PJOC_SRV_UNSUBSCRIBE_FROM_HOST_STATISTICS)
}

PRL_ASYNC_METHOD( PrlSrv_UnsubscribeFromHostStatistics ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UNSUBSCRIBE_FROM_HOST_STATISTICS)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UnsubscribeFromHostStatistics, (hServer))
}

// Perfomace statistics methods
PRL_HANDLE PrlSrv_SubscribeToPerfStats_Impl(PRL_HANDLE handle, PRL_CONST_STR sFilter)
{
	PRL_CntxSw_CHECK_HANDLE_ASYNC(handle, PHT_SERVER, PJOC_SRV_SUBSCRIBE_PERFSTATS) ;
	return store_result(PRL_OBJECT_BY_HANDLE<PrlHandleServerStat>(handle)->SubscribePerfStats(sFilter)) ;
}

/*
  Perfomace statistics methods
 */
PRL_ASYNC_METHOD( PrlSrv_SubscribeToPerfStats ) (PRL_HANDLE hServer, PRL_CONST_STR sFilter)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_SUBSCRIBE_PERFSTATS) ;
    CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_SubscribeToPerfStats, (hServer, sFilter)) ;
}

PRL_HANDLE PrlSrv_UnsubscribeFromPerfStats_Impl(PRL_HANDLE handle)
{
	PRL_CntxSw_CHECK_HANDLE_ASYNC(handle, PHT_SERVER, PJOC_SRV_UNSUBSCRIBE_PERFSTATS) ;
	return store_result(PRL_OBJECT_BY_HANDLE<PrlHandleServerStat>(handle)->UnsubscribePerfStats()) ;
}

PRL_ASYNC_METHOD( PrlSrv_UnsubscribeFromPerfStats ) (PRL_HANDLE hServer)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UNSUBSCRIBE_PERFSTATS) ;
    CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UnsubscribeFromPerfStats, (hServer)) ;
}

PRL_HANDLE PrlSrv_GetPerfStats_Impl(PRL_HANDLE handle, PRL_CONST_STR sFilter)
{
	PRL_CntxSw_CHECK_HANDLE_ASYNC(handle, PHT_SERVER, PJOC_SRV_GET_PERFSTATS) ;
	return store_result(PRL_OBJECT_BY_HANDLE<PrlHandleServerStat>(handle)->GetPerfStats(sFilter)) ;
}

PRL_ASYNC_METHOD( PrlSrv_GetPerfStats ) (PRL_HANDLE hServer, PRL_CONST_STR sFilter)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_PERFSTATS) ;
    CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetPerfStats, (hServer, sFilter)) ;
}

PRL_METHOD( PrlStat_GetTotalRamSize) (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetTotalRamSize(pnSize));
}

PRL_METHOD( PrlStat_GetUsageRamSize) (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetUsageRamSize(pnSize));
}

PRL_METHOD( PrlStat_GetFreeRamSize) (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetFreeRamSize(pnSize));
}

PRL_METHOD( PrlStat_GetRealRamSize) (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetRealRamSize(pnSize));
}

PRL_METHOD( PrlStat_GetTotalSwapSize) (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetTotalSwapSize(pnSize));
}

PRL_METHOD( PrlStat_GetUsageSwapSize) (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetUsageSwapSize(pnSize));
}

PRL_METHOD( PrlStat_GetFreeSwapSize) (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetFreeSwapSize(pnSize));
}

PRL_METHOD( PrlStat_GetOsUptime) (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnUptime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnUptime=%.8X)",
		__FUNCTION__,
		hStat,
		pnUptime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnUptime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetOsUptime(pnUptime));
}

PRL_METHOD( PrlStat_GetDispUptime) (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnUptime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnUptime=%.8X)",
		__FUNCTION__,
		hStat,
		pnUptime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnUptime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetDispUptime(pnUptime));
}

PRL_METHOD( PrlStat_GetCpusStatsCount) (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnCount=%.8X)",
		__FUNCTION__,
		hStat,
		pnCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnCount) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetCpusStatsCount(pnCount));
}

PRL_METHOD( PrlStat_GetCpuStat) (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phCpuStat
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, nIndex=%.8X, phCpuStat=%.8X)",
		__FUNCTION__,
		hStat,
		nIndex,
		phCpuStat
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(phCpuStat) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetCpuStat(nIndex, phCpuStat));
}

PRL_METHOD( PrlStat_GetIfacesStatsCount) (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnCount=%.8X)",
		__FUNCTION__,
		hStat,
		pnCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnCount) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetIfacesStatsCount(pnCount));
}

PRL_METHOD( PrlStat_GetIfaceStat) (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phIfaceStat
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, nIndex=%.8X, phIfaceStat=%.8X)",
		__FUNCTION__,
		hStat,
		nIndex,
		phIfaceStat
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(phIfaceStat) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetIfaceStat(nIndex, phIfaceStat));
}

PRL_METHOD( PrlStat_GetUsersStatsCount) (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnCount=%.8X)",
		__FUNCTION__,
		hStat,
		pnCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnCount) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetUsersStatsCount(pnCount));
}

PRL_METHOD( PrlStat_GetUserStat) (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phUserStat
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, nIndex=%.8X, phUserStat=%.8X)",
		__FUNCTION__,
		hStat,
		nIndex,
		phUserStat
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(phUserStat) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetUserStat(nIndex, phUserStat));
}

PRL_METHOD( PrlStat_GetDisksStatsCount) (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnCount=%.8X)",
		__FUNCTION__,
		hStat,
		pnCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnCount) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetDisksStatsCount(pnCount));
}

PRL_METHOD( PrlStat_GetDiskStat) (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phDiskStat
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, nIndex=%.8X, phDiskStat=%.8X)",
		__FUNCTION__,
		hStat,
		nIndex,
		phDiskStat
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(phDiskStat) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetDiskStat(nIndex, phDiskStat));
}

PRL_METHOD( PrlStat_GetProcsStatsCount) (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, pnCount=%.8X)",
		__FUNCTION__,
		hStat,
		pnCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(pnCount) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetProcsStatsCount(pnCount));
}

PRL_METHOD( PrlStat_GetProcStat) (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phProcStat
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%.8X, nIndex=%.8X, phProcStat=%.8X)",
		__FUNCTION__,
		hStat,
		nIndex,
		phProcStat
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(phProcStat) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetProcStat(nIndex, phProcStat));
}

PRL_METHOD( PrlStatCpu_GetCpuUsage) (
		PRL_HANDLE hCpuStat,
		PRL_UINT32_PTR pnPercents
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hCpuStat=%.8X, pnPercents=%.8X)",
		__FUNCTION__,
		hCpuStat,
		pnPercents
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hCpuStat, PHT_SYSTEM_STATISTICS_CPU) || PRL_WRONG_PTR(pnPercents) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsCpuPtr pSystemStatisticsCpu = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsCpu>( hCpuStat );
	return (pSystemStatisticsCpu->GetCpuUsage(pnPercents));
}

PRL_METHOD( PrlStatCpu_GetTotalTime) (
		PRL_HANDLE hCpuStat,
		PRL_UINT64_PTR pnTime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hCpuStat=%.8X, pnTime=%.8X)",
		__FUNCTION__,
		hCpuStat,
		pnTime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hCpuStat, PHT_SYSTEM_STATISTICS_CPU) || PRL_WRONG_PTR(pnTime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsCpuPtr pSystemStatisticsCpu = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsCpu>( hCpuStat );
	return (pSystemStatisticsCpu->GetTotalTime(pnTime));
}

PRL_METHOD( PrlStatCpu_GetUserTime) (
		PRL_HANDLE hCpuStat,
		PRL_UINT64_PTR pnTime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hCpuStat=%.8X, pnTime=%.8X)",
		__FUNCTION__,
		hCpuStat,
		pnTime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hCpuStat, PHT_SYSTEM_STATISTICS_CPU) || PRL_WRONG_PTR(pnTime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsCpuPtr pSystemStatisticsCpu = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsCpu>( hCpuStat );
	return (pSystemStatisticsCpu->GetUserTime(pnTime));
}

PRL_METHOD( PrlStatCpu_GetSystemTime) (
		PRL_HANDLE hCpuStat,
		PRL_UINT64_PTR pnTime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hCpuStat=%.8X, pnTime=%.8X)",
		__FUNCTION__,
		hCpuStat,
		pnTime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hCpuStat, PHT_SYSTEM_STATISTICS_CPU) || PRL_WRONG_PTR(pnTime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsCpuPtr pSystemStatisticsCpu = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsCpu>( hCpuStat );
	return (pSystemStatisticsCpu->GetSystemTime(pnTime));
}

PRL_METHOD( PrlStatIface_GetSystemName) (
		PRL_HANDLE hIfaceStat,
		PRL_STR	sSystemName,
		PRL_UINT32_PTR pnSystemNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hIfaceStat=%.8X, sSystemName=%.8X, pnSystemNameBufLength=%.8X)",
		__FUNCTION__,
		hIfaceStat,
		sSystemName,
		pnSystemNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hIfaceStat, PHT_SYSTEM_STATISTICS_IFACE)
				|| PRL_WRONG_PTR(pnSystemNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsIfacePtr pSystemStatisticsIface = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsIface>( hIfaceStat );
	return (pSystemStatisticsIface->GetSystemName(sSystemName, pnSystemNameBufLength));
}

PRL_METHOD( PrlStatIface_GetInDataSize) (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnDataSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hIfaceStat=%.8X, pnDataSize=%.8X)",
		__FUNCTION__,
		hIfaceStat,
		pnDataSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hIfaceStat, PHT_SYSTEM_STATISTICS_IFACE) || PRL_WRONG_PTR(pnDataSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsIfacePtr pSystemStatisticsIface = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsIface>( hIfaceStat );
	return (pSystemStatisticsIface->GetInDataSize(pnDataSize));
}

PRL_METHOD( PrlStatIface_GetOutDataSize) (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnDataSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hIfaceStat=%.8X, pnDataSize=%.8X)",
		__FUNCTION__,
		hIfaceStat,
		pnDataSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hIfaceStat, PHT_SYSTEM_STATISTICS_IFACE) || PRL_WRONG_PTR(pnDataSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsIfacePtr pSystemStatisticsIface = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsIface>( hIfaceStat );
	return (pSystemStatisticsIface->GetOutDataSize(pnDataSize));
}

PRL_METHOD( PrlStatIface_GetInPkgsCount) (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnPkgsCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hIfaceStat=%.8X, pnPkgsCount=%.8X)",
		__FUNCTION__,
		hIfaceStat,
		pnPkgsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hIfaceStat, PHT_SYSTEM_STATISTICS_IFACE) || PRL_WRONG_PTR(pnPkgsCount) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsIfacePtr pSystemStatisticsIface = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsIface>( hIfaceStat );
	return (pSystemStatisticsIface->GetInPkgsCount(pnPkgsCount));
}

PRL_METHOD( PrlStatIface_GetOutPkgsCount) (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnPkgsCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hIfaceStat=%.8X, pnPkgsCount=%.8X)",
		__FUNCTION__,
		hIfaceStat,
		pnPkgsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hIfaceStat, PHT_SYSTEM_STATISTICS_IFACE) || PRL_WRONG_PTR(pnPkgsCount) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsIfacePtr pSystemStatisticsIface = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsIface>( hIfaceStat );
	return (pSystemStatisticsIface->GetOutPkgsCount(pnPkgsCount));
}

PRL_METHOD( PrlStatUser_GetUserName) (
		PRL_HANDLE hUserStat,
		PRL_STR	sUserName,
		PRL_UINT32_PTR pnUserNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserStat=%.8X, sUserName=%.8X, pnUserNameBufLength=%.8X)",
		__FUNCTION__,
		hUserStat,
		sUserName,
		pnUserNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hUserStat, PHT_SYSTEM_STATISTICS_USER_SESSION)
				|| PRL_WRONG_PTR(pnUserNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsUserPtr pSystemStatisticsUser = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsUser>( hUserStat );
#ifdef _WIN_
#ifdef GetUserName
#undef GetUserName
#endif
#endif
	return (pSystemStatisticsUser->GetUserName(sUserName, pnUserNameBufLength));
}

PRL_METHOD( PrlStatUser_GetServiceName) (
		PRL_HANDLE hUserStat,
		PRL_STR	sServiceName,
		PRL_UINT32_PTR pnServiceNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserStat=%.8X, sServiceName=%.8X, pnServiceNameBufLength=%.8X)",
		__FUNCTION__,
		hUserStat,
		sServiceName,
		pnServiceNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hUserStat, PHT_SYSTEM_STATISTICS_USER_SESSION)
				|| PRL_WRONG_PTR(pnServiceNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsUserPtr pSystemStatisticsUser = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsUser>( hUserStat );
	return (pSystemStatisticsUser->GetServiceName(sServiceName, pnServiceNameBufLength));
}

PRL_METHOD( PrlStatUser_GetHostName) (
		PRL_HANDLE hUserStat,
		PRL_STR	sHostName,
		PRL_UINT32_PTR pnHostNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserStat=%.8X, sHostName=%.8X, pnHostNameBufLength=%.8X)",
		__FUNCTION__,
		hUserStat,
		sHostName,
		pnHostNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hUserStat, PHT_SYSTEM_STATISTICS_USER_SESSION)
				|| PRL_WRONG_PTR(pnHostNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsUserPtr pSystemStatisticsUser = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsUser>( hUserStat );
	return (pSystemStatisticsUser->GetHostName(sHostName, pnHostNameBufLength));
}

PRL_METHOD( PrlStatUser_GetSessionTime) (
		PRL_HANDLE hUserStat,
		PRL_UINT64_PTR pnTime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserStat=%.8X, pnTime=%.8X)",
		__FUNCTION__,
		hUserStat,
		pnTime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hUserStat, PHT_SYSTEM_STATISTICS_USER_SESSION) || PRL_WRONG_PTR(pnTime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsUserPtr pSystemStatisticsUser = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsUser>( hUserStat );
	return (pSystemStatisticsUser->GetSessionTime(pnTime));
}

PRL_METHOD( PrlStatDisk_GetSystemName) (
		PRL_HANDLE hDiskStat,
		PRL_STR	sSystemName,
		PRL_UINT32_PTR pnSystemNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDiskStat=%.8X, sSystemName=%.8X, pnSystemNameBufLength=%.8X)",
		__FUNCTION__,
		hDiskStat,
		sSystemName,
		pnSystemNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDiskStat, PHT_SYSTEM_STATISTICS_DISK)
				|| PRL_WRONG_PTR(pnSystemNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsDiskPtr pSystemStatisticsDisk = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsDisk>( hDiskStat );
	return (pSystemStatisticsDisk->GetSystemName(sSystemName, pnSystemNameBufLength));
}

PRL_METHOD( PrlStatDisk_GetUsageDiskSpace) (
		PRL_HANDLE hDiskStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDiskStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hDiskStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDiskStat, PHT_SYSTEM_STATISTICS_DISK) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsDiskPtr pSystemStatisticsDisk = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsDisk>( hDiskStat );
	return (pSystemStatisticsDisk->GetUsageDiskSpace(pnSize));
}

PRL_METHOD( PrlStatDisk_GetFreeDiskSpace) (
		PRL_HANDLE hDiskStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDiskStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hDiskStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDiskStat, PHT_SYSTEM_STATISTICS_DISK) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsDiskPtr pSystemStatisticsDisk = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsDisk>( hDiskStat );
	return (pSystemStatisticsDisk->GetFreeDiskSpace(pnSize));
}

PRL_METHOD( PrlStatDisk_GetPartsStatsCount) (
		PRL_HANDLE hDiskStat,
		PRL_UINT32_PTR pnCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDiskStat=%.8X, pnCount=%.8X)",
		__FUNCTION__,
		hDiskStat,
		pnCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDiskStat, PHT_SYSTEM_STATISTICS_DISK) || PRL_WRONG_PTR(pnCount) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsDiskPtr pSystemStatisticsDisk = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsDisk>( hDiskStat );
	return (pSystemStatisticsDisk->GetPartsStatsCount(pnCount));
}

PRL_METHOD( PrlStatDisk_GetPartStat) (
		PRL_HANDLE hDiskStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phPartStat
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDiskStat=%.8X, nIndex=%.8X, phPartStat=%.8X)",
		__FUNCTION__,
		hDiskStat,
		nIndex,
		phPartStat
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDiskStat, PHT_SYSTEM_STATISTICS_DISK) || PRL_WRONG_PTR(phPartStat) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsDiskPtr pSystemStatisticsDisk = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsDisk>( hDiskStat );
	return (pSystemStatisticsDisk->GetPartStat(nIndex, phPartStat));
}

PRL_METHOD( PrlStatDiskPart_GetSystemName) (
		PRL_HANDLE hPartStat,
		PRL_STR	sSystemName,
		PRL_UINT32_PTR pnSystemNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPartStat=%.8X, sSystemName=%.8X, pnSystemNameBufLength=%.8X)",
		__FUNCTION__,
		hPartStat,
		sSystemName,
		pnSystemNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hPartStat, PHT_SYSTEM_STATISTICS_DISK_PARTITION)
				|| PRL_WRONG_PTR(pnSystemNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsDiskPartPtr pSystemStatisticsDiskPart = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsDiskPart>( hPartStat );
	return (pSystemStatisticsDiskPart->GetSystemName(sSystemName, pnSystemNameBufLength));
}

PRL_METHOD( PrlStatDiskPart_GetUsageDiskSpace) (
		PRL_HANDLE hPartStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPartStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hPartStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hPartStat, PHT_SYSTEM_STATISTICS_DISK_PARTITION) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsDiskPartPtr pSystemStatisticsDiskPart = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsDiskPart>( hPartStat );
	return (pSystemStatisticsDiskPart->GetUsageDiskSpace(pnSize));
}

PRL_METHOD( PrlStatDiskPart_GetFreeDiskSpace) (
		PRL_HANDLE hPartStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPartStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hPartStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hPartStat, PHT_SYSTEM_STATISTICS_DISK_PARTITION) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsDiskPartPtr pSystemStatisticsDiskPart = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsDiskPart>( hPartStat );
	return (pSystemStatisticsDiskPart->GetFreeDiskSpace(pnSize));
}

PRL_METHOD( PrlStatProc_GetCommandName) (
		PRL_HANDLE hProcStat,
		PRL_STR	sCommandName,
		PRL_UINT32_PTR pnCommandNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, sCommandName=%.8X, pnCommandNameBufLength=%.8X)",
		__FUNCTION__,
		hProcStat,
		sCommandName,
		pnCommandNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS)
				|| PRL_WRONG_PTR(pnCommandNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetCommandName(sCommandName, pnCommandNameBufLength));
}

PRL_METHOD( PrlStatProc_GetId) (
		PRL_HANDLE hProcStat,
		PRL_UINT32_PTR pnId
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnId=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnId
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnId) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetId(pnId));
}

PRL_METHOD( PrlStatProc_GetOwnerUserName) (
		PRL_HANDLE hProcStat,
		PRL_STR	sOwnerUserName,
		PRL_UINT32_PTR pnOwnerUserNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, sOwnerUserName=%.8X, pnOwnerUserNameBufLength=%.8X)",
		__FUNCTION__,
		hProcStat,
		sOwnerUserName,
		pnOwnerUserNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS)
				|| PRL_WRONG_PTR(pnOwnerUserNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetOwnerUserName(sOwnerUserName, pnOwnerUserNameBufLength));
}

PRL_METHOD( PrlStatProc_GetTotalMemUsage) (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetTotalMemUsage(pnSize));
}

PRL_METHOD( PrlStatProc_GetRealMemUsage) (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetRealMemUsage(pnSize));
}

PRL_METHOD( PrlStatProc_GetVirtMemUsage) (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnSize) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetVirtMemUsage(pnSize));
}

PRL_METHOD( PrlStatProc_GetStartTime) (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnTime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnTime=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnTime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnTime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetStartTime(pnTime));
}

PRL_METHOD( PrlStatProc_GetTotalTime) (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnTime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnTime=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnTime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnTime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetTotalTime(pnTime));
}

PRL_METHOD( PrlStatProc_GetUserTime) (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnTime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnTime=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnTime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnTime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetUserTime(pnTime));
}

PRL_METHOD( PrlStatProc_GetSystemTime) (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnTime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnTime=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnTime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnTime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetSystemTime(pnTime));
}

PRL_METHOD( PrlStatProc_GetState) (
		PRL_HANDLE hProcStat,
		PRL_PROCESS_STATE_TYPE_PTR pnState
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnState=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnState
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnState) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetState(pnState));
}

PRL_METHOD( PrlStatProc_GetCpuUsage) (
										PRL_HANDLE hProcStat,
										PRL_UINT32_PTR pnPercents
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hProcStat=%.8X, pnPercents=%.8X)",
		__FUNCTION__,
		hProcStat,
		pnPercents
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProcStat, PHT_SYSTEM_STATISTICS_PROCESS) || PRL_WRONG_PTR(pnPercents) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsProcPtr pSystemStatisticsProc = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsProc>( hProcStat );
	return (pSystemStatisticsProc->GetCpuUsage(pnPercents));
}
