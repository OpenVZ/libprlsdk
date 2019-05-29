/*
 * PrlHandleSystemStatisticsVmData.cpp: This file is the part of
 * parallels public sdk library
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


#include "PrlHandleSystemStatisticsVmData.h"

PrlHandleSystemStatisticsVmData::PrlHandleSystemStatisticsVmData(PrlHandleSystemStatisticsPtr pSysStat,
																 const CVmDataStatistic& vmDataStatistic)
: PrlHandleBase(PHT_SYSTEM_STATISTICS_VM_DATA),
  m_pSysStat(pSysStat),
  m_VmDataStatistic(vmDataStatistic)
{
}

PRL_RESULT PrlHandleSystemStatisticsVmData::fromString(PRL_CONST_STR sXml)
{
	return m_VmDataStatistic.fromString(UTF8_2QSTR(sXml));
}

QString PrlHandleSystemStatisticsVmData::toString()
{
	return m_VmDataStatistic.toString();
}

PRL_RESULT PrlHandleSystemStatisticsVmData::GetSegmentCapacity(PRL_DATA_STATISTIC_SEGMENTS nSegment, PRL_UINT64_PTR pnCapacity)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS

	foreach(CVmDataSegment* pSegment, m_VmDataStatistic.getVmDataSegments()->m_lstVmDataSegments)
	{
		if (pSegment->getType() != nSegment)
			continue;

		*pnCapacity = pSegment->getCapacity();
		return PRL_ERR_SUCCESS;
	}

	return PRL_ERR_NO_DATA;
}
