/*
 * PrlHandleSystemStatisticsCpu.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
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


#ifndef __VIRTUOZZO_HANDLE_SYSTEM_STATISTICS_CPU_H__
#define __VIRTUOZZO_HANDLE_SYSTEM_STATISTICS_CPU_H__


#include "PrlHandleSystemStatistics.h"

/**
 * Handle object represents CPU statistics element
 */
class PrlHandleSystemStatisticsCpu : public PrlHandleBase
{

public:
	/**
	 * Class constructor.
	 */
	PrlHandleSystemStatisticsCpu(PrlHandleSystemStatisticsPtr pSysStat, CCpuStatistics *pCpu);

	/**
	 * Returns CPU usage in percents
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuUsage(PRL_UINT32_PTR pnPercents);


	/**
	 * Returns total time in secs
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetTotalTime(PRL_UINT64_PTR pnTime);


	/**
	 * Returns user time in secs
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUserTime(PRL_UINT64_PTR pnTime);


	/**
	 * Returns system time in secs
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSystemTime(PRL_UINT64_PTR pnTime);

private:
	/// Pointer to parent system statistics object
	PrlHandleSystemStatisticsPtr m_pSysStat;
	/// SystemStatisticsCpu object
	CCpuStatistics *m_pCpu;
};

typedef PrlHandleSmartPtr<PrlHandleSystemStatisticsCpu> PrlHandleSystemStatisticsCpuPtr;

#endif // __VIRTUOZZO_HANDLE_SYSTEM_STATISTICS_CPU_H__
