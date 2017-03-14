/*
 * PrlHandleSystemStatisticsUser.cpp
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


#include "PrlHandleSystemStatisticsUser.h"

PrlHandleSystemStatisticsUser::PrlHandleSystemStatisticsUser(PrlHandleSystemStatisticsPtr pSysStat, CUserStatistics *pUser)
: PrlHandleBase(PHT_SYSTEM_STATISTICS_USER_SESSION), m_pSysStat(pSysStat), m_pUser(pUser)
{}

PRL_RESULT PrlHandleSystemStatisticsUser::GetSessionTime(PRL_UINT64_PTR pnTime)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	*pnTime = m_pUser->getSessionTime();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSystemStatisticsUser::GetUserName(PRL_STR sUserName, PRL_UINT32_PTR pnUserNameBufLength)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	return CopyStringValue(m_pUser->getUserName(), sUserName, pnUserNameBufLength);
}

PRL_RESULT PrlHandleSystemStatisticsUser::GetServiceName(PRL_STR sServiceName, PRL_UINT32_PTR pnServiceNameBufLength)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	return CopyStringValue(m_pUser->getServiceName(), sServiceName, pnServiceNameBufLength);
}

PRL_RESULT PrlHandleSystemStatisticsUser::GetHostName(PRL_STR sHostName, PRL_UINT32_PTR pnHostNameBufLength)
{
	SYNCHRO_PARENT_SYSTEM_STATISTICS
	return CopyStringValue(m_pUser->getHostName(), sHostName, pnHostNameBufLength);
}
