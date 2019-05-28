/*
 * PrlApiStat.cpp
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


//#define FORCE_LOGGING_ON
//#define FORCE_LOGGING_LEVEL	DBG_DEBUG

#include "SDK/Include/Parallels.h"

#include <prlcommon/PrlCommonUtilsBase/PrlStringifyConsts.h>
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/Logging/Logging.h>

#include "PrlCommon.h"
#include "PrlHandleServerStat.h"
#include "PrlHandleSystemStatistics.h"
#include "PrlHandleSystemStatisticsCpu.h"

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

