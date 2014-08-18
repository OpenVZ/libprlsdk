/*
 * PrlHandleVirtNet.cpp
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


#include "PrlHandleVirtNet.h"
#include "PrlHandlePortFwd.h"
#include "PrlHandleHandlesList.h"
#include "PrlHandleSrvConfig.h"
#include "PrlHandleHwNetAdapter.h"

#include "Libraries/PrlCommonUtilsBase/NetworkUtils.h"

PrlHandleVirtNet::PrlHandleVirtNet()
: PrlHandleBase(PHT_VIRTUAL_NETWORK)
{
}

PrlHandleVirtNet::PrlHandleVirtNet(QString sVirtNet)
: PrlHandleBase(PHT_VIRTUAL_NETWORK)
{
	m_VirtualNetwork.fromString(sVirtNet);
}

PRL_RESULT PrlHandleVirtNet::CreateVirtualNetwork(PRL_HANDLE_PTR phVirtNet)
{
	PrlHandleVirtNet* pVirtNet = new PrlHandleVirtNet;
	if ( ! pVirtNet )
		return PRL_ERR_OUT_OF_MEMORY;

	*phVirtNet = pVirtNet->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (PRL_SUCCEEDED(m_VirtualNetwork.fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleVirtNet::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_VirtualNetwork.toString();
}

PRL_RESULT PrlHandleVirtNet::GetNetworkId(PRL_STR sNetworkId, PRL_UINT32_PTR pnNetworkIdBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VirtualNetwork.getNetworkID(), sNetworkId, pnNetworkIdBufLength);
}

PRL_RESULT PrlHandleVirtNet::SetNetworkId(PRL_CONST_STR sNetworkId)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.setNetworkID(UTF8_2QSTR(sNetworkId));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetDescription(PRL_STR sDescription, PRL_UINT32_PTR pnDescriptionBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VirtualNetwork.getDescription(), sDescription, pnDescriptionBufLength);
}

PRL_RESULT PrlHandleVirtNet::SetDescription(PRL_CONST_STR sDescription)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.setDescription(UTF8_2QSTR(sDescription));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetNetworkType(PRL_NET_VIRTUAL_NETWORK_TYPE_PTR pnNetworkType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnNetworkType = m_VirtualNetwork.getNetworkType();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::SetNetworkType(PRL_NET_VIRTUAL_NETWORK_TYPE nNetworkType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.setNetworkType(nNetworkType);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetBoundCardMac(PRL_STR sBoundCardMac, PRL_UINT32_PTR pnBoundCardMacBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VirtualNetwork.getBoundCardMac(), sBoundCardMac, pnBoundCardMacBufLength);
}

PRL_RESULT PrlHandleVirtNet::SetBoundCardMac(PRL_CONST_STR sBoundCardMac)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.setBoundCardMac(UTF8_2QSTR(sBoundCardMac));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetAdapterName(PRL_STR sAdapterName, PRL_UINT32_PTR pnAdapterNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VirtualNetwork.getHostOnlyNetwork()->getParallelsAdapter()->getName(),
							sAdapterName, pnAdapterNameBufLength);
}

PRL_RESULT PrlHandleVirtNet::SetAdapterName(PRL_CONST_STR sAdapterName)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.getHostOnlyNetwork()->
						getParallelsAdapter()->setName(UTF8_2QSTR(sAdapterName));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetAdapterIndex(PRL_UINT32_PTR pnAdapterIndex)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnAdapterIndex = m_VirtualNetwork.getHostOnlyNetwork()->
						getParallelsAdapter()->getPrlAdapterIndex();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::SetAdapterIndex(PRL_UINT32 nAdapterIndex)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.getHostOnlyNetwork()->
						getParallelsAdapter()->setPrlAdapterIndex(nAdapterIndex);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetHostIPAddress(PRL_STR sHostIPAddress, PRL_UINT32_PTR pnHostIPAddressBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VirtualNetwork.getHostOnlyNetwork()->getHostIPAddress().toString(),
							sHostIPAddress, pnHostIPAddressBufLength);
}

PRL_RESULT PrlHandleVirtNet::SetHostIPAddress(PRL_CONST_STR sHostIPAddress)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QHostAddress addr;
	if (!addr.setAddress(UTF8_2QSTR(sHostIPAddress)))
		return PRL_ERR_INVALID_ARG;
	m_VirtualNetwork.getHostOnlyNetwork()->setHostIPAddress(addr);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetDhcpIPAddress(PRL_STR sDhcpIPAddress, PRL_UINT32_PTR pnDhcpIPAddressBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VirtualNetwork.getHostOnlyNetwork()->getDhcpIPAddress().toString(),
							sDhcpIPAddress, pnDhcpIPAddressBufLength);
}

PRL_RESULT PrlHandleVirtNet::SetDhcpIPAddress(PRL_CONST_STR sDhcpIPAddress)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QHostAddress addr;
	if (!addr.setAddress(UTF8_2QSTR(sDhcpIPAddress)))
		return PRL_ERR_INVALID_ARG;
	m_VirtualNetwork.getHostOnlyNetwork()->setDhcpIPAddress(addr);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetIPNetMask(PRL_STR sIPNetMask, PRL_UINT32_PTR pnIPNetMaskBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VirtualNetwork.getHostOnlyNetwork()->getIPNetMask().toString(),
							sIPNetMask, pnIPNetMaskBufLength);
}

PRL_RESULT PrlHandleVirtNet::SetIPNetMask(PRL_CONST_STR sIPNetMask)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString ip_mask = QString("%1/%2")
		.arg(m_VirtualNetwork.getHostOnlyNetwork()->getHostIPAddress().toString())
		.arg(UTF8_2QSTR(sIPNetMask));
	QString ip, mask;
	if (!NetworkUtils::ParseIpMask(ip_mask, ip, mask))
		return (PRL_ERR_INVALID_ARG);
	QHostAddress addr;
	if (!addr.setAddress(mask))
		return PRL_ERR_INVALID_ARG;
	m_VirtualNetwork.getHostOnlyNetwork()->setIPNetMask(addr);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetVlanTag(PRL_UINT16_PTR pnVlanTag)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVlanTag = m_VirtualNetwork.getVLANTag();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVirtNet::SetVlanTag(PRL_UINT16 nVlanTag)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.setVLANTag(nVlanTag);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVirtNet::GetIPScopeStart(PRL_STR sIPScopeStart, PRL_UINT32_PTR pnIPScopeStartBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VirtualNetwork.getHostOnlyNetwork()->getDHCPServer()->getIPScopeStart().toString(),
							sIPScopeStart, pnIPScopeStartBufLength);
}

PRL_RESULT PrlHandleVirtNet::SetIPScopeStart(PRL_CONST_STR sIPScopeStart)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QHostAddress addr;
	if (!addr.setAddress(UTF8_2QSTR(sIPScopeStart)))
		return PRL_ERR_INVALID_ARG;
	m_VirtualNetwork.getHostOnlyNetwork()->getDHCPServer()->setIPScopeStart(addr);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetIPScopeEnd(PRL_STR sIPScopeEnd, PRL_UINT32_PTR pnIPScopeEndBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VirtualNetwork.getHostOnlyNetwork()->getDHCPServer()->getIPScopeEnd().toString(),
							sIPScopeEnd, pnIPScopeEndBufLength);
}

PRL_RESULT PrlHandleVirtNet::SetIPScopeEnd(PRL_CONST_STR sIPScopeEnd)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QHostAddress addr;
	if (!addr.setAddress(UTF8_2QSTR(sIPScopeEnd)))
		return PRL_ERR_INVALID_ARG;
	m_VirtualNetwork.getHostOnlyNetwork()->getDHCPServer()->setIPScopeEnd(addr);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::IsEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = PRL_BOOL(m_VirtualNetwork.isEnabled());
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::SetEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.setEnabled((bool )bEnabled);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::IsAdapterEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = PRL_BOOL(m_VirtualNetwork.getHostOnlyNetwork()->getParallelsAdapter()->isEnabled());
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::SetAdapterEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.getHostOnlyNetwork()->getParallelsAdapter()->setEnabled((bool )bEnabled);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::IsDHCPServerEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = PRL_BOOL(m_VirtualNetwork.getHostOnlyNetwork()->getDHCPServer()->isEnabled());
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::SetDHCPServerEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.getHostOnlyNetwork()->getDHCPServer()->setEnabled((bool )bEnabled);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::IsNATServerEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = PRL_BOOL(m_VirtualNetwork.getHostOnlyNetwork()->getNATServer()->isEnabled());
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::SetNATServerEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VirtualNetwork.getHostOnlyNetwork()->getNATServer()->setEnabled((bool )bEnabled);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetPortForwardList(PRL_PORT_FORWARDING_TYPE nPortFwdType, PRL_HANDLE_PTR phPortFwdList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	CPortForwarding* pPortFwd = m_VirtualNetwork.getHostOnlyNetwork()->getNATServer()->getPortForwarding();
	QList<CPortForwardEntry* > lstEntries = ( nPortFwdType == PPF_TCP ?
												  pPortFwd->getTCP()->m_lstForwardEntry
												: pPortFwd->getUDP()->m_lstForwardEntry );

	QList<PrlHandleBasePtr> list;
	foreach( CPortForwardEntry* pEntry, lstEntries )
	{
		PrlHandlePortFwd* pPortFwd = new PrlHandlePortFwd;
		if ( ! pPortFwd )
			return PRL_ERR_OUT_OF_MEMORY;

		pPortFwd->m_PortForwardEntry.fromString(pEntry->toString());

		list += PrlHandleBasePtr(pPortFwd);
		pPortFwd->Release();
	}

	PrlHandleHandlesList* pList = new PrlHandleHandlesList;
	if ( ! pList )
		return PRL_ERR_OUT_OF_MEMORY;

	pList->SetHandlesList(list);

	*phPortFwdList = pList->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::SetPortForwardList(PRL_PORT_FORWARDING_TYPE nPortFwdType, PRL_HANDLE hPortFwdList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	CPortForwarding* pPortFwd = m_VirtualNetwork.getHostOnlyNetwork()->getNATServer()->getPortForwarding();
	if (nPortFwdType == PPF_TCP)
		pPortFwd->getTCP()->ClearLists();
	else
		pPortFwd->getUDP()->ClearLists();

	if (hPortFwdList == PRL_INVALID_HANDLE)
		return PRL_ERR_SUCCESS;

	QList<CPortForwardEntry* >& lstEntries = ( nPortFwdType == PPF_TCP ?
												  pPortFwd->getTCP()->m_lstForwardEntry
												: pPortFwd->getUDP()->m_lstForwardEntry );

	PrlHandleHandlesListPtr pList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hPortFwdList);
	QList<PrlHandleBasePtr> list = pList->GetHandlesList();

	for( int i = 0; i < list.size(); i++ )
	{
		CPortForwardEntry* pEntry = new CPortForwardEntry;
		if ( ! pEntry )
			return PRL_ERR_OUT_OF_MEMORY;

		PrlHandlePortFwd* pPortFwd = static_cast<PrlHandlePortFwd* >(list[i].getHandle());
		pEntry->fromString(pPortFwd->m_PortForwardEntry.toString());

		lstEntries += pEntry;
	}
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVirtNet::GetBoundAdapterInfo(const PrlHandleSrvConfigPtr &pSrvConfig, PRL_HANDLE_PTR phNetAdapter)
{
	QString sMacAddress;
	PRL_UINT16 nVlanTag = 0;
	{
		//To prevent potential deadlocks
		SYNCHRO_INTERNAL_DATA_ACCESS
		sMacAddress = m_VirtualNetwork.getBoundCardMac();
		nVlanTag = m_VirtualNetwork.getVLANTag();
	}
	QMutexLocker _lock(pSrvConfig->GetSynchroObject());
	CHwNetAdapter *pTargetNetAdapter = NULL;
	foreach(CHwNetAdapter *pNetAdapter, pSrvConfig->GetSrvConfig().m_lstNetworkAdapters)
	{
		if (!QString::compare(pNetAdapter->getMacAddress(),
			sMacAddress, Qt::CaseInsensitive) &&
			pNetAdapter->getVLANTag() == nVlanTag)
		{
			pTargetNetAdapter = pNetAdapter;
			break;
		}
	}
	if (!pTargetNetAdapter)
		return (PRL_ERR_NETWORK_ADAPTER_NOT_FOUND);

	PrlHandleHwNetAdapter *pNetAdapter = new PrlHandleHwNetAdapter(pSrvConfig, pTargetNetAdapter);
	if (!pNetAdapter)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phNetAdapter = pNetAdapter->GetHandle();
	return (PRL_ERR_SUCCESS);
}

