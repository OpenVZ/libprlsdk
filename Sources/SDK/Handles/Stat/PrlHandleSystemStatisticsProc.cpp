/*
 * PrlHandleSystemStatisticsProc.cpp
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


#include "PrlHandleSystemStatisticsProc.h"

PrlHandleSystemStatisticsProc::PrlHandleSystemStatisticsProc(PrlHandleSystemStatisticsPtr pSysStat,
																															CProcInfoStatistics *pProc)
: PrlHandleBase(PHT_SYSTEM_STATISTICS_PROCESS), m_pSysStat(pSysStat), m_pProc(pProc)
{}

PRL_RESULT PrlHandleSystemStatisticsProc::GetCommandName(PRL_STR sCommandName, PRL_UINT32_PTR pnCommandNameBufLength)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	return CopyStringValue(m_pProc->getCommandName(), sCommandName, pnCommandNameBufLength);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetId(PRL_UINT32_PTR pnId)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnId = m_pProc->getProcId();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetOwnerUserName(PRL_STR sOwnerUserName, PRL_UINT32_PTR pnOwnerUserNameBufLength)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	return CopyStringValue(m_pProc->getOwnerUser(), sOwnerUserName, pnOwnerUserNameBufLength);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetTotalMemUsage(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnSize = m_pProc->getTotalMemUsage();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetRealMemUsage(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnSize = m_pProc->getRealMemUsage();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetVirtMemUsage(PRL_UINT64_PTR pnSize)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnSize = m_pProc->getVirtualMemUsage();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetStartTime(PRL_UINT64_PTR pnTime)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnTime = m_pProc->getStartTime();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetTotalTime(PRL_UINT64_PTR pnTime)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnTime = m_pProc->getTotalTime();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetUserTime(PRL_UINT64_PTR pnTime)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnTime = m_pProc->getUserTime();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetSystemTime(PRL_UINT64_PTR pnTime)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnTime = m_pProc->getSystemTime();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetCpuUsage(PRL_UINT32_PTR pnPercents)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnPercents = m_pProc->getPercentsUsage();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsProc::GetState(PRL_PROCESS_STATE_TYPE_PTR pnState)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnState = m_pProc->getState();
	return (PRL_ERR_SUCCESS);
}
