/*
 * PrlHandleServerDeprecated.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#include <prlxmlmodel/VmConfig/CVmConfiguration.h>

#include "PrlHandleStringsList.h"
#include "PrlHandleDispNet.h"
#include "PrlHandleServerJob.h"
#include "PrlHandleServerDeprecated.h"


PrlHandleJobPtr PrlHandleServerDeprecated::StartConvertHdd(PRL_HANDLE hStringsList)
{
	PrlHandleStringsListPtr pStringsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hStringsList);
	QString job_uuid = m_pPveControl->DspCmdConvertOldHdd(
		hStringsList != PRL_INVALID_HANDLE ? pStringsList->GetStringsList() : QStringList());

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_UNKNOWN)));
}

PrlHandleJobPtr PrlHandleServerDeprecated::SmcGetRuntimeInfo()
{
    QString job_uuid = m_pPveControl->DspCmdSMCGetDispatcherRTInfo();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_UNKNOWN)));
}

PrlHandleJobPtr PrlHandleServerDeprecated::PrepareForHibernate(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdPrepareForHibernate(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_PREPARE_FOR_HIBERNATE)));
}

PrlHandleJobPtr PrlHandleServerDeprecated::AfterHostResume(PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdAfterHostResume(nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_SRV_AFTER_HOST_RESUME)));
}

PrlHandleJobPtr PrlHandleServerDeprecated::AddNetAdapter(PRL_HANDLE hNetAdapter)
{
	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hNetAdapter);
	QString strDispNet = ElementToString<CDispNetAdapter*>(pDispNet->GetNetAdapter(), "tmpTag");

	QString job_uuid = m_pPveControl->DspCmdAddNetAdapter(strDispNet.toUtf8().data());

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_ADD_NET_ADAPTER)));
}

PrlHandleJobPtr PrlHandleServerDeprecated::DeleteNetAdapter(PRL_UINT32 nIndex)
{
	QString job_uuid = m_pPveControl->DspCmdDeleteNetAdapter(nIndex);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_DELETE_NET_ADAPTER)));
}

PrlHandleJobPtr PrlHandleServerDeprecated::UpdateNetAdapter(PRL_HANDLE hNetAdapter)
{
	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hNetAdapter);
	QString strDispNet = ElementToString<CDispNetAdapter*>(pDispNet->GetNetAdapter(), "tmpTag");

	QString job_uuid = m_pPveControl->DspCmdUpdateNetAdapter(strDispNet.toUtf8().data());

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_UPDATE_NET_ADAPTER)));
}

PrlHandleJobPtr PrlHandleServerDeprecated::RestartNetworkShaping(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdRestartNetworkShaping(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_RESTART_NETWORK_SHAPING)));
}

PrlHandleJobPtr PrlHandleServerDeprecated::DspCmdStorageSetValue(PRL_CONST_STR sKey, PRL_CONST_STR sValue, PRL_UINT32 nFlags)
{
	return create_job(m_pPveControl->DspCmdStorageSetValue(sKey, sValue, nFlags),
		PJOC_SRV_STORE_VALUE_BY_KEY);
}

PrlHandleJobPtr PrlHandleServerDeprecated::DspCmdSendClientStatistics( PRL_CONST_STR sStatistics, PRL_UINT32 nFlags )
{
	return create_job(m_pPveControl->DspCmdSendClientStatistics(sStatistics, nFlags),
					PJOC_SRV_SEND_CLIENT_STATISTICS);
}

PrlHandleJobPtr PrlHandleServerDeprecated::DspCmdUpdateUsbAssocList( PRL_HANDLE hList,
														  PRL_UINT32 nListVersion,
														  PRL_UINT32 nFlags )
{
	PrlHandleStringsListPtr pStringsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hList);
	QString job_uuid = m_pPveControl->DspCmdUpdateUsbAssocList(
		hList != PRL_INVALID_HANDLE ? pStringsList->GetStringsList() : QStringList(),
		nListVersion,
		nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_SRV_UPDATE_USB_ASSOC_LIST)));
}

PrlHandleJobPtr PrlHandleServerDeprecated::DspCmdUpdateDeviceInfo(PRL_CONST_STR sVmDeviceConfiguration)
{
	QString job_uuid = m_pPveControl->DspCmdUpdateDeviceInfo(sVmDeviceConfiguration);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_UNKNOWN)));
}
