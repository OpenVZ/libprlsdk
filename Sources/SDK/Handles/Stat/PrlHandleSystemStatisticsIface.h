/*
 * PrlHandleSystemStatisticsIface.h
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


#ifndef __PARALLELS_HANDLE_SYSTEM_STATISTICS_IFACE_H__
#define __PARALLELS_HANDLE_SYSTEM_STATISTICS_IFACE_H__


#include "PrlHandleSystemStatistics.h"

/**
 * Handle object represents net iface statistics element
 */
class PrlHandleSystemStatisticsIface : public PrlHandleBase
{

public:
	/**
	 * Class constructor.
	 */
	PrlHandleSystemStatisticsIface(PrlHandleSystemStatisticsPtr pSysStat, CNetIfaceStatistics *pIface);

	/**
	 * Returns net iface system name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSystemName(PRL_STR sSystemName, PRL_UINT32_PTR pnSystemNameBufLength);

	/**
	 * Returns input net iface transmitted data size in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetInDataSize(PRL_UINT64_PTR pnDataSize);

	/**
	 * Returns output net iface transmitted data size in bytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOutDataSize(PRL_UINT64_PTR pnDataSize);

	/**
	 * Returns in packages count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetInPkgsCount(PRL_UINT64_PTR pnPkgsCount);

	/**
	 * Returns out packages count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOutPkgsCount(PRL_UINT64_PTR pnPkgsCount);

private:
	/// Pointer to parent system statistics object
	PrlHandleSystemStatisticsPtr m_pSysStat;
	/// SystemStatisticsIface object
	CNetIfaceStatistics *m_pIface;
};

typedef PrlHandleSmartPtr<PrlHandleSystemStatisticsIface> PrlHandleSystemStatisticsIfacePtr;

#endif // __PARALLELS_HANDLE_SYSTEM_STATISTICS_IFACE_H__
