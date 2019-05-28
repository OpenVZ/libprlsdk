/*
 * PrlHandleSystemStatisticsProc.h
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


#ifndef __PARALLELS_HANDLE_SYSTEM_STATISTICS_PROCESS_H__
#define __PARALLELS_HANDLE_SYSTEM_STATISTICS_PROCESS_H__


#include "PrlHandleSystemStatistics.h"

/**
 * Handle object represents process statistics element
 */
class PrlHandleSystemStatisticsProc : public PrlHandleBase
{

public:
	/**
	 * Class constructor.
	 */
	PrlHandleSystemStatisticsProc(PrlHandleSystemStatisticsPtr pSysStat, CProcInfoStatistics *pProc);

	/**
	 * Returns process command name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCommandName(PRL_STR sCommandName, PRL_UINT32_PTR pnCommandNameBufLength);

	/**
	 * Returns process system id
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetId(PRL_UINT32_PTR pnId);

	/**
	 * Returns process owner user name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOwnerUserName(PRL_STR sOwnerUserName, PRL_UINT32_PTR pnOwnerUserNameBufLength);

	/**
	 * Returns process total memory usage in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetTotalMemUsage(PRL_UINT64_PTR pnSize);

	/**
	 * Returns process real memory usage in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRealMemUsage(PRL_UINT64_PTR pnSize);

	/**
	 * Returns process virtual memory usage in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetVirtMemUsage(PRL_UINT64_PTR pnSize);

	/**
	 * Returns process start working time in secs (num of seconds since January 1, 1601 (UTC))
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetStartTime(PRL_UINT64_PTR pnTime);

	/**
	 * Returns process working total time in secs
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetTotalTime(PRL_UINT64_PTR pnTime);

	/**
	 * Returns process working user space time in secs
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUserTime(PRL_UINT64_PTR pnTime);

	/**
	 * Returns process working system space time in secs
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSystemTime(PRL_UINT64_PTR pnTime);

	/**
	 * Returns process CPU usage CPU usage in percents
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuUsage(PRL_UINT32_PTR pnPercents);

	/**
	 * Returns process state
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetState(PRL_PROCESS_STATE_TYPE_PTR pnState);

private:
	/// Pointer to parent system statistics object
	PrlHandleSystemStatisticsPtr m_pSysStat;
	/// SystemStatisticsProc object
	CProcInfoStatistics *m_pProc;
};

typedef PrlHandleSmartPtr<PrlHandleSystemStatisticsProc> PrlHandleSystemStatisticsProcPtr;

#endif // __PARALLELS_HANDLE_SYSTEM_STATISTICS_PROCESS_H__
