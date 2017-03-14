/*
 * PrlHandleSystemStatisticsVmData.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __PARALLELS_HANDLE_SYSTEM_STATISTICS_VM_DATA_H__
#define __PARALLELS_HANDLE_SYSTEM_STATISTICS_VM_DATA_H__

#include <prlxmlmodel/VmDataStatistic/CVmDataStatistic.h>
#include "PrlHandleSystemStatistics.h"

/**
 * Handle object represents VM data statistic element
 */
class PrlHandleSystemStatisticsVmData : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 */
	PrlHandleSystemStatisticsVmData(PrlHandleSystemStatisticsPtr pSysStat,
									const CVmDataStatistic& vmDataStatistic);
	/**
	 * Initialize object from XML string
	 */
	virtual PRL_RESULT fromString(PRL_CONST_STR sXml);
	/**
	 * Returns object as XML string
	 */
	virtual QString toString();

	/**
	 * Gets VM data segment capacity
	 */
	PRL_RESULT GetSegmentCapacity(PRL_DATA_STATISTIC_SEGMENTS nSegment, PRL_UINT64_PTR pnCapacity);

private:

	// Pointer to parent system statistics object
	PrlHandleSystemStatisticsPtr m_pSysStat;

	CVmDataStatistic	m_VmDataStatistic;

};

typedef PrlHandleSmartPtr<PrlHandleSystemStatisticsVmData> PrlHandleSystemStatisticsVmDataPtr;

#endif	// __PARALLELS_HANDLE_SYSTEM_STATISTICS_VM_DATA_H__
