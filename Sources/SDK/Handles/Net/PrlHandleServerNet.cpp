/*
 * PrlHandleServerNet.cpp
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


#include "XmlModel/NetworkConfig/CNetworkClassesConfig.h"

#include "PrlHandleServerJob.h"
#include "PrlHandleVirtNet.h"
#include "PrlHandleNetworkClass.h"
#include "PrlHandleNetworkShapingConfig.h"
#include "PrlHandleIPPrivNet.h"
#include "PrlHandleServerNet.h"


PrlHandleJobPtr PrlHandleServerNet::GetVirtualNetworkList(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdGetVirtualNetworkList(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_VIRTUAL_NETWORK_LIST)));
}

PrlHandleJobPtr PrlHandleServerNet::AddVirtualNetwork(PRL_HANDLE hVirtNet, PRL_UINT32 nFlags)
{
	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	QString job_uuid = m_pPveControl->DspCmdAddVirtualNetwork(QSTR2UTF8(pVirtNet->toString()), nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_ADD_VIRTUAL_NETWORK)));
}

PrlHandleJobPtr PrlHandleServerNet::UpdateVirtualNetwork(PRL_HANDLE hVirtNet, PRL_UINT32 nFlags)
{
	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	QString job_uuid = m_pPveControl->DspCmdUpdateVirtualNetwork(QSTR2UTF8(pVirtNet->toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_UPDATE_VIRTUAL_NETWORK)));
}

PrlHandleJobPtr PrlHandleServerNet::DeleteVirtualNetwork(PRL_HANDLE hVirtNet, PRL_UINT32 nFlags)
{
	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	QString job_uuid = m_pPveControl->DspCmdDeleteVirtualNetwork(QSTR2UTF8(pVirtNet->toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_DELETE_VIRTUAL_NETWORK)));
}

PrlHandleJobPtr PrlHandleServerNet::NetServiceStart()
{
	QString job_uuid = m_pPveControl->DspCmdNetPrlNetworkServiceStart();

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_NET_SERVICE_START)));
}

PrlHandleJobPtr PrlHandleServerNet::NetServiceStop()
{
	QString job_uuid = m_pPveControl->DspCmdNetPrlNetworkServiceStop();

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_NET_SERVICE_STOP)));
}

PrlHandleJobPtr PrlHandleServerNet::NetServiceRestart()
{
	QString job_uuid = m_pPveControl->DspCmdNetPrlNetworkServiceRestart();

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_NET_SERVICE_RESTART)));
}

PrlHandleJobPtr PrlHandleServerNet::NetServiceRestoreDefaults()
{
	QString job_uuid = m_pPveControl->DspCmdNetPrlNetworkServiceRestoreDefaults();

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_NET_SERVICE_RESTORE_DEFAULTS)));
}

PrlHandleJobPtr PrlHandleServerNet::GetNetServiceStatus()
{
	QString job_uuid = m_pPveControl->DspCmdGetNetServiceStatus();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_NET_SERVICE_STATUS)));
}

PrlHandleJobPtr PrlHandleServerNet::UpdateOfflineService(PrlHandleOfflineServicePtr pOffmgmtService, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdUpdateOfflineService(QSTR2UTF8(pOffmgmtService->toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_UPDATE_OFFLINE_SERVICE)));
}

PrlHandleJobPtr PrlHandleServerNet::DeleteOfflineService(PrlHandleOfflineServicePtr pOffmgmtService, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdDeleteOfflineService(QSTR2UTF8(pOffmgmtService->toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_DELETE_OFFLINE_SERVICE)));
}

PrlHandleJobPtr PrlHandleServerNet::GetOfflineServicesList(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdGetOfflineServicesList(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_OFFLINE_SERVICES_LIST)));
}

PrlHandleJobPtr PrlHandleServerNet::UpdateNetworkClassesList(PrlHandleHandlesListPtr pList, PRL_UINT32 nFlags)
{
	CNetworkClassesConfig conf;

	foreach (PrlHandleBasePtr ph, pList->GetHandlesList())
	{
		if (ph->GetType() != PHT_NETWORK_CLASS)
			continue;

		PrlHandleNetworkClassPtr phClass = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkClass>(ph->GetHandle());

		CNetworkClass *pClass = new CNetworkClass;
		pClass->fromString(phClass->toString());
		conf.m_lstNetworkClasses += pClass;
	}
	QString job_uuid = m_pPveControl->DspCmdUpdateNetworkClassesConfig(QSTR2UTF8(conf.toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_UPDATE_NETWORK_CLASSES_CONFIG)));
}

PrlHandleJobPtr PrlHandleServerNet::GetNetworkClassesList(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdGetNetworkClassesConfig(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_NETWORK_CLASSES_LIST)));
}

PrlHandleJobPtr PrlHandleServerNet::UpdateNetworkShapingConfig(PRL_HANDLE hConfig, PRL_UINT32 nFlags)
{
	PrlHandleNetworkShapingConfigPtr pConfig = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingConfig>(hConfig);
	QString job_uuid = m_pPveControl->DspCmdUpdateNetworkShapingConfig(QSTR2UTF8(pConfig->toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_UPDATE_NETWORK_SHAPING_CONFIG)));
}

PrlHandleJobPtr PrlHandleServerNet::GetNetworkShapingConfig(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdGetNetworkShapingConfig(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_NETWORK_SHAPING_CONFIG)));
}

PrlHandleJobPtr PrlHandleServerNet::AddIPPrivateNetwork(PRL_HANDLE hPrivNet, PRL_UINT32 nFlags)
{
	PrlHandleIPPrivNetPtr pPrivNet = PRL_OBJECT_BY_HANDLE<PrlHandleIPPrivNet>(hPrivNet);
	QString job_uuid = m_pPveControl->DspCmdAddIPPrivateNetwork(QSTR2UTF8(pPrivNet->toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
					PJOC_SRV_ADD_IPPRIVATE_NETWORK)));
}

PrlHandleJobPtr PrlHandleServerNet::RemoveIPPrivateNetwork(PRL_HANDLE hPrivNet, PRL_UINT32 nFlags)
{
	PrlHandleIPPrivNetPtr pPrivNet = PRL_OBJECT_BY_HANDLE<PrlHandleIPPrivNet>(hPrivNet);
	QString job_uuid = m_pPveControl->DspCmdRemoveIPPrivateNetwork(QSTR2UTF8(pPrivNet->toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
					PJOC_SRV_REMOVE_IPPRIVATE_NETWORK)));
}

PrlHandleJobPtr PrlHandleServerNet::UpdateIPPrivateNetwork(PRL_HANDLE hPrivNet, PRL_UINT32 nFlags)
{
	PrlHandleIPPrivNetPtr pPrivNet = PRL_OBJECT_BY_HANDLE<PrlHandleIPPrivNet>(hPrivNet);
	QString job_uuid = m_pPveControl->DspCmdUpdateIPPrivateNetwork(QSTR2UTF8(pPrivNet->toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
					PJOC_SRV_UPDATE_IPPRIVATE_NETWORK)));
}

PrlHandleJobPtr PrlHandleServerNet::GetIPPrivateNetworksList(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdGetIPPrivateNetworksList(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_IPPRIVATE_NETWORKS_LIST)));
}
