/*
 * PrlHandleServerStat.cpp
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


#include "PrlHandleServerJob.h"
#include "PrlHandleServerStat.h"


PrlHandleJobPtr PrlHandleServerStat::GetStatistics()
{
    QString job_uuid = m_pPveControl->DspCmdGetHostStatistics();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_STATISTICS)));
}

PrlHandleJobPtr PrlHandleServerStat::SubscribeToHostStatistics()
{
    QString job_uuid = m_pPveControl->DspCmdSubscribeToHostStatistics();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_SUBSCRIBE_TO_HOST_STATISTICS)));
}

PrlHandleJobPtr PrlHandleServerStat::UnsubscribeFromHostStatistics()
{
    QString job_uuid = m_pPveControl->DspCmdUnsubscribeFromHostStatistics();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_UNSUBSCRIBE_FROM_HOST_STATISTICS)));
}

/**
 * Perform send perfomance counters commands.
 */
PrlHandleJobPtr PrlHandleServerStat::DspCmdSubscribePerfStats(PRL_CONST_STR filter, PRL_CONST_STR sVmUuid)
{
    return create_job(m_pPveControl->DspCmdSubscribePerfStats(filter, sVmUuid),
                      sVmUuid ? PJOC_VM_SUBSCRIBE_PERFSTATS : PJOC_SRV_SUBSCRIBE_PERFSTATS) ;
}

PrlHandleJobPtr PrlHandleServerStat::DspCmdUnsubscribePerfStats(PRL_CONST_STR sVmUuid)
{
    return create_job(m_pPveControl->DspCmdUnsubscribePerfStats(sVmUuid),
                      sVmUuid ? PJOC_VM_UNSUBSCRIBE_PERFSTATS : PJOC_SRV_UNSUBSCRIBE_PERFSTATS) ;
}

PrlHandleJobPtr PrlHandleServerStat::DspCmdGetPerfStats(PRL_CONST_STR filter, PRL_CONST_STR sVmUuid)
{
    return create_job(m_pPveControl->DspCmdGetPerfStats(filter, sVmUuid),
                      sVmUuid ? PJOC_VM_GET_PERFSTATS : PJOC_SRV_GET_PERFSTATS) ;
}
