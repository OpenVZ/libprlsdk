/*
 * PrlHandleSystemStatisticsIface.cpp
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


#include "PrlHandleSystemStatisticsIface.h"

PrlHandleSystemStatisticsIface::PrlHandleSystemStatisticsIface(PrlHandleSystemStatisticsPtr pSysStat, CNetIfaceStatistics *pIface)
: PrlHandleBase(PHT_SYSTEM_STATISTICS_IFACE), m_pSysStat(pSysStat), m_pIface(pIface)
{}

PRL_RESULT PrlHandleSystemStatisticsIface::GetInDataSize(PRL_UINT64_PTR pnDataSize)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnDataSize = m_pIface->getInDataSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsIface::GetOutDataSize(PRL_UINT64_PTR pnDataSize)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnDataSize = m_pIface->getOutDataSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsIface::GetSystemName(PRL_STR sSystemName, PRL_UINT32_PTR pnSystemNameBufLength)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	return CopyStringValue(m_pIface->getIfaceSystemName(), sSystemName, pnSystemNameBufLength);
}

PRL_RESULT PrlHandleSystemStatisticsIface::GetInPkgsCount(PRL_UINT64_PTR pnPkgsCount)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnPkgsCount = m_pIface->getInPkgsCount();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsIface::GetOutPkgsCount(PRL_UINT64_PTR pnPkgsCount)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnPkgsCount = m_pIface->getOutPkgsCount();
	return (PRL_ERR_SUCCESS);
}
