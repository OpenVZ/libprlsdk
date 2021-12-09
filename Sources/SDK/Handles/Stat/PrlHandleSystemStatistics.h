/*
 * PrlHandleSystemStatistics.h
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


#ifndef __VIRTUOZZO_HANDLE_SYSTEM_STATISTICS_H__
#define __VIRTUOZZO_HANDLE_SYSTEM_STATISTICS_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/HostHardwareInfo/CSystemStatistics.h>
#include <QDataStream>

#define SYNCHRO_PARENT_SYSTEM_STATISTICS QMutexLocker _lock(m_pSysStat->GetSynchroObject());

/**
 * Handle object represents system statistics
 */
class PrlHandleSystemStatistics : public PrlHandleBase
{

public:
	/**
	 * Class constructor. Performs initialization from system statistics XML representation
	 */
	PrlHandleSystemStatistics(const QString& sSystemStatistics);

	/**
	 * Class constructor. Performs initialization from raw data (alternative serialization mech)
	 */
	PrlHandleSystemStatistics(QDataStream &_data_stream);

	/**
	 * Returns total RAM size in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetTotalRamSize(PRL_UINT64_PTR pnSize);


	/**
	 * Returns usage RAM size in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUsageRamSize(PRL_UINT64_PTR pnSize);


	/**
	 * Returns free RAM size in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetFreeRamSize(PRL_UINT64_PTR pnSize);


	/**
	 * Returns real RAM size in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRealRamSize(PRL_UINT64_PTR pnSize);


	/**
	 * Returns total swap size in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetTotalSwapSize(PRL_UINT64_PTR pnSize);


	/**
	 * Returns usage swap size in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUsageSwapSize(PRL_UINT64_PTR pnSize);


	/**
	 * Returns free swap size in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetFreeSwapSize(PRL_UINT64_PTR pnSize);


	/**
	 * Returns OS uptime in secs
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOsUptime(PRL_UINT64_PTR pnUptime);


	/**
	 * Returns dispatcher uptime in secs
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDispUptime(PRL_UINT64_PTR pnUptime);


	/**
	 * Returns pointer to internal dispatcher config data synchronization object
	 */
	inline QRecursiveMutex *GetSynchroObject() {return (&m_HandleMutex);}

	/**
	 * Returns CPUs statistics elements count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpusStatsCount(PRL_UINT32_PTR pnCount);


	/**
	 * Returns CPU statistics object
	 * @param [in] index of object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - index is out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phCpuStat);


	/**
	 * Returns net ifaces statistics elements count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetIfacesStatsCount(PRL_UINT32_PTR pnCount);


	/**
	 * Returns net iface statistics object
	 * @param [in] index of object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - index is out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetIfaceStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phIfaceStat);


	/**
	 * Returns users sessions statistics elements count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUsersStatsCount(PRL_UINT32_PTR pnCount);


	/**
	 * Returns user session statistics object
	 * @param [in] index of object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - index is out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUserStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phUserStat);


	/**
	 * Returns disks statistics elements count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDisksStatsCount(PRL_UINT32_PTR pnCount);


	/**
	 * Returns disk statistics object
	 * @param [in] index of object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - index is out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDiskStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phDiskStat);


	/**
	 * Returns processes statistics elements count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetProcsStatsCount(PRL_UINT32_PTR pnCount);


	/**
	 * Returns process statistics object
	 * @param [in] index of object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - index is out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetProcStat(PRL_UINT32 nIndex, PRL_HANDLE_PTR phProcStat);

	/**
	 * Returns VM data statistic object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - index is out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetVmDataStat(PRL_HANDLE_PTR phVmDataStat);

private:
	/// SystemStatistics object
	CSystemStatistics m_SystemStatistics;
};

typedef PrlHandleSmartPtr<PrlHandleSystemStatistics> PrlHandleSystemStatisticsPtr;

#endif // __VIRTUOZZO_HANDLE_SYSTEM_STATISTICS_H__
