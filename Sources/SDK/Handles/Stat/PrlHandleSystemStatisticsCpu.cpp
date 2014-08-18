/*
 * PrlHandleSystemStatisticsCpu.cpp
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


#include "PrlHandleSystemStatisticsCpu.h"

PrlHandleSystemStatisticsCpu::PrlHandleSystemStatisticsCpu(PrlHandleSystemStatisticsPtr pSysStat, CCpuStatistics *pCpu)
: PrlHandleBase(PHT_SYSTEM_STATISTICS_CPU), m_pSysStat(pSysStat), m_pCpu(pCpu)
{}

PRL_RESULT PrlHandleSystemStatisticsCpu::GetCpuUsage(PRL_UINT32_PTR pnPercents)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnPercents = m_pCpu->getPercentsUsage();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsCpu::GetTotalTime(PRL_UINT64_PTR pnTime)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnTime = m_pCpu->getTotalTime();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsCpu::GetUserTime(PRL_UINT64_PTR pnTime)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnTime = m_pCpu->getUserTime();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsCpu::GetSystemTime(PRL_UINT64_PTR pnTime)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnTime = m_pCpu->getSystemTime();
	return (PRL_ERR_SUCCESS);
}
