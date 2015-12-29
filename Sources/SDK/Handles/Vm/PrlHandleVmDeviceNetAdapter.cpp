/*
 * PrlHandleVmDeviceNetAdapter.cpp
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


#include "PrlHandleVmDeviceNetAdapter.h"
#include "XmlModel/VmConfig/CVmConfiguration.h"
#include "XmlModel/VmConfig/CVmGenericNetworkAdapter.h"
#include <time.h>
#include <prlcommon/HostUtils/HostUtils.h>
#include <prlcommon/PrlCommonUtilsBase/netutils.h>
#include "PrlHandleStringsList.h"
#include <prlcommon/PrlCommonUtilsBase/NetworkUtils.h>
#include "PrlHandleFirewallRule.h"
#include <QHostAddress>

#ifdef ENABLE_MALLOC_DEBUG
    // By adding this interface we enable allocations tracing in the module
    #include "Interfaces/Debug.h"
#else
    // We're not allowed to throw exceptions from the library -
    // so we need to prevent operator ::new from doing this
    #include <new>
    using std::nothrow;
    #define new new(nothrow)
#endif


#define CHECK_NET_ADAPTER\
	if (!m_pVmNetAdapter)\
		TryToRestoreObjectElement();\
	if (!m_pVmNetAdapter)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDeviceNetAdapter::PrlHandleVmDeviceNetAdapter()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_NET_ADAPTER)
{
	m_pVmNetAdapter = new CVmGenericNetworkAdapter;
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmNetAdapter);
	GenerateMacAddress();
}

PrlHandleVmDeviceNetAdapter::PrlHandleVmDeviceNetAdapter( const PrlHandleVmPtr &pVm, CVmGenericNetworkAdapter *pNetAdapter )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_NET_ADAPTER), m_pVmNetAdapter(pNetAdapter)
{
	if (!m_pVmNetAdapter)
	{
		m_pVmNetAdapter = new CVmGenericNetworkAdapter;
		if(m_pVm)
		{
			m_pVmNetAdapter->setIndex(CalculateDefaultDevIndex(m_pVm->GetVmConfig().getVmHardwareList()->m_lstNetworkAdapters));
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstNetworkAdapters.append(m_pVmNetAdapter);
		}
		GenerateMacAddress();
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmNetAdapter);
}

PrlHandleVmDeviceNetAdapter::~PrlHandleVmDeviceNetAdapter()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmNetAdapter;
		m_pVmNetAdapter = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	if (PRL_SUCCEEDED(m_pVmNetAdapter->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceNetAdapter::toString()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	if (!m_pVmNetAdapter)
		return QString();
	return m_pVmNetAdapter->toString();
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::ConcreteRemove()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstNetworkAdapters.removeAll(m_pVmNetAdapter);
	m_pVmNetAdapter = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceNetAdapter::GetVmDeviceElem()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	if (!m_pVmNetAdapter)
		TryToRestoreObjectElement();
	return (m_pVmNetAdapter);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setEmulatedType((PRL_NET_ADAPTER_EMULATED_TYPE)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

CVmClusteredDevice *PrlHandleVmDeviceNetAdapter::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	if (!m_pVmNetAdapter)
		TryToRestoreObjectElement();
	if (!m_pVmNetAdapter)
		nRetCode = PRL_ERR_OBJECT_WAS_REMOVED;
	return m_pVmNetAdapter;
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetBoundAdapterIndex(PRL_INT32_PTR pnIndex)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	*pnIndex = m_pVmNetAdapter->getBoundAdapterIndex();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetBoundAdapterIndex(PRL_INT32 nIndex)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setBoundAdapterIndex(nIndex);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetBoundAdapterName(PRL_STR sBoundAdapterName, PRL_UINT32_PTR pnBoundAdapterNameBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	return CopyStringValue(m_pVmNetAdapter->getBoundAdapterName(), sBoundAdapterName, pnBoundAdapterNameBufLength);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetBoundAdapterName(PRL_CONST_STR sNewBoundAdapterName)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	QString sNewBoundAdapterNameUtf8 = UTF8_2QSTR(sNewBoundAdapterName);
	m_pVmNetAdapter->setBoundAdapterName(sNewBoundAdapterNameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetHostInterfaceName(PRL_STR sHostInterfaceName,
		PRL_UINT32_PTR pnHostInterfaceNameBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	return CopyStringValue(m_pVmNetAdapter->getHostInterfaceName(), sHostInterfaceName, pnHostInterfaceNameBufLength);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetHostInterfaceName(PRL_CONST_STR sNewHostInterfaceName)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setHostInterfaceName(UTF8_2QSTR(sNewHostInterfaceName));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetMacAddress(PRL_STR sMacAddress,
		PRL_UINT32_PTR pnMacAddressBufLength, bool bCanonical)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	QString sMac = m_pVmNetAdapter->getMacAddress();

	if (bCanonical)
		PrlNet::convertMacAddress(sMac);

	return CopyStringValue(sMac, sMacAddress, pnMacAddressBufLength);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetMacAddress(PRL_CONST_STR sNewMacAddress)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	QString sNewMacAddressUtf8 = UTF8_2QSTR(sNewMacAddress);

	if (!PrlNet::convertToPrlMacAddress(sNewMacAddressUtf8))
		return (PRL_ERR_INVALID_ARG);

	m_pVmNetAdapter->setMacAddress(sNewMacAddressUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GenerateMacAddress()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setMacAddress(HostUtils::generateMacAddress());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetDefaultGateway(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	return CopyStringValue(m_pVmNetAdapter->getDefaultGateway(), sDefaultGateway, pnDefaultGatewayBufLength);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetDefaultGateway(PRL_CONST_STR sNewDefaultGateway)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	QString sNewDefaultGatewayUtf8 = UTF8_2QSTR(sNewDefaultGateway);

	if (!sNewDefaultGatewayUtf8.isEmpty())
	{
		QHostAddress ip_addr;
		if (!ip_addr.setAddress(sNewDefaultGateway))
			return PRL_ERR_INVALID_ARG;
		if (ip_addr.protocol() != QAbstractSocket::IPv4Protocol)
			return PRL_ERR_INVALID_ARG;
	}
	m_pVmNetAdapter->setDefaultGateway(sNewDefaultGatewayUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetDefaultGatewayIPv6(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	return CopyStringValue(m_pVmNetAdapter->getDefaultGatewayIPv6(), sDefaultGateway, pnDefaultGatewayBufLength);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetDefaultGatewayIPv6(PRL_CONST_STR sNewDefaultGateway)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	QString sNewDefaultGatewayUtf8 = UTF8_2QSTR(sNewDefaultGateway);

	if (!sNewDefaultGatewayUtf8.isEmpty())
	{
		QHostAddress ip_addr;
		if (!ip_addr.setAddress(sNewDefaultGateway))
			return PRL_ERR_INVALID_ARG;
		if (ip_addr.protocol() != QAbstractSocket::IPv6Protocol)
			return PRL_ERR_INVALID_ARG;
	}

	m_pVmNetAdapter->setDefaultGatewayIPv6(sNewDefaultGatewayUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetVirtualNetworkId(PRL_STR sVirtualNetworkId, PRL_UINT32_PTR pnVirtualNetworkIdBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	return CopyStringValue(m_pVmNetAdapter->getVirtualNetworkID(), sVirtualNetworkId, pnVirtualNetworkIdBufLength);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetVirtualNetworkId(PRL_CONST_STR sNewVirtualNetworkId)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	QString sNewVirtualNetworkIdUtf8 = UTF8_2QSTR(sNewVirtualNetworkId);
	m_pVmNetAdapter->setVirtualNetworkID(sNewVirtualNetworkIdUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetAdapterType(PRL_VM_NET_ADAPTER_TYPE_PTR pnAdapterType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER

	*pnAdapterType = m_pVmNetAdapter->getAdapterType();

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetAdapterType(PRL_VM_NET_ADAPTER_TYPE nAdapterType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setAdapterType(nAdapterType);
	return (PRL_ERR_SUCCESS);
}


PRL_RESULT PrlHandleVmDeviceNetAdapter::GetNetAddresses(PRL_HANDLE_PTR phNetAddressesList)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER

	PrlHandleStringsList *pNetAddressesList =
		new PrlHandleStringsList(m_pVmNetAdapter->getNetAddresses());
	if (pNetAddressesList)
	{
		*phNetAddressesList = pNetAddressesList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}

	return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetNetAddresses(const QStringList &lstNetAddresses)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	QStringList lstValidNetAddresses;
	for (int i = 0; i < lstNetAddresses.size(); i++)
	{
		QString ip_mask = lstNetAddresses[i];
		if (!NetworkUtils::ValidateAndConvertIpMask(ip_mask))
			return (PRL_ERR_INVALID_ARG);
		lstValidNetAddresses.append(ip_mask);
	}
	m_pVmNetAdapter->setNetAddresses(lstValidNetAddresses);
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVmDeviceNetAdapter::MarkAsInvalid()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	m_pVmNetAdapter = NULL;
}

void PrlHandleVmDeviceNetAdapter::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmGenericNetworkAdapter *pNetAdapter, m_pVm->GetVmConfig().getVmHardwareList()->m_lstNetworkAdapters)
		{
			if (GetHashCode() == pNetAdapter->getIndex())
			{
				m_pVmNetAdapter = pNetAdapter;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmNetAdapter);
				break;
			}
		}
	}
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetDnsServers(PRL_HANDLE_PTR phDnsServersList)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	PrlHandleStringsList *pDnsServersList = new PrlHandleStringsList(m_pVmNetAdapter->getDnsIPAddresses());
	if (pDnsServersList)
	{
		*phDnsServersList = pDnsServersList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}

	return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetDnsServers(const QStringList &lstDnsServers)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setDnsIPAddresses(lstDnsServers);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetSearchDomains(PRL_HANDLE_PTR phSearchDomainsList)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	PrlHandleStringsList *pSearchDomainsList = new PrlHandleStringsList(m_pVmNetAdapter->getSearchDomains());
	if (pSearchDomainsList)
	{
		*phSearchDomainsList = pSearchDomainsList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}

	return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetSearchDomains(const QStringList &lstSearchDomains)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setSearchDomains(lstSearchDomains);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::IsConfigureWithDhcp(PRL_BOOL_PTR pbConfigureWithDhcp)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	*pbConfigureWithDhcp = m_pVmNetAdapter->isConfigureWithDhcp();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetConfigureWithDhcp(PRL_BOOL bConfigureWithDhcp)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setConfigureWithDhcpEx(bConfigureWithDhcp);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::IsConfigureWithDhcpIPv6(PRL_BOOL_PTR pbConfigureWithDhcp)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	*pbConfigureWithDhcp = m_pVmNetAdapter->isConfigureWithDhcpIPv6();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetConfigureWithDhcpIPv6(PRL_BOOL bConfigureWithDhcp)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setConfigureWithDhcpIPv6Ex(bConfigureWithDhcp);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::IsAutoApply(PRL_BOOL_PTR pbAutoApply)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	*pbAutoApply = m_pVmNetAdapter->isAutoApply();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetAutoApply(PRL_BOOL bAutoApply)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->setAutoApply(bAutoApply);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::IsPktFilterPreventMacSpoof(PRL_BOOL_PTR pbPktFilterPreventMacSpoof)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	*pbPktFilterPreventMacSpoof = m_pVmNetAdapter->getPktFilter()->isPreventMacSpoof();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetPktFilterPreventMacSpoof(PRL_BOOL bPktFilterPreventMacSpoof)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->getPktFilter()->setPreventMacSpoof(bPktFilterPreventMacSpoof);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::IsPktFilterPreventPromisc(PRL_BOOL_PTR pbPktFilterPreventPromisc)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	*pbPktFilterPreventPromisc = m_pVmNetAdapter->getPktFilter()->isPreventPromisc();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetPktFilterPreventPromisc(PRL_BOOL bPktFilterPreventPromisc)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->getPktFilter()->setPreventPromisc(bPktFilterPreventPromisc);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::IsPktFilterPreventIpSpoof(PRL_BOOL_PTR pbPktFilterPreventIpSpoof)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	*pbPktFilterPreventIpSpoof = m_pVmNetAdapter->getPktFilter()->isPreventIpSpoof();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetPktFilterPreventIpSpoof(PRL_BOOL bPktFilterPreventIpSpoof)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->getPktFilter()->setPreventIpSpoof(bPktFilterPreventIpSpoof);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::IsFirewallEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	*pbEnabled = m_pVmNetAdapter->getFirewall()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetFirewallEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	m_pVmNetAdapter->getFirewall()->setEnabled(bEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetFirewallDefaultPolicy(PRL_FIREWALL_DIRECTION nDirection,
		PRL_FIREWALL_POLICY_PTR pnPolicy)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	if (nDirection == PFD_INCOMING)
		*pnPolicy = m_pVmNetAdapter->getFirewall()->getIncoming()->getDirection()->getDefaultPolicy();
	else
		*pnPolicy = m_pVmNetAdapter->getFirewall()->getOutgoing()->getDirection()->getDefaultPolicy();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetFirewallDefaultPolicy(PRL_FIREWALL_DIRECTION nDirection,
		PRL_FIREWALL_POLICY nPolicy)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER
	if (nDirection == PFD_INCOMING)
		m_pVmNetAdapter->getFirewall()->getIncoming()->getDirection()->setDefaultPolicy(nPolicy);
	else
		m_pVmNetAdapter->getFirewall()->getOutgoing()->getDirection()->setDefaultPolicy(nPolicy);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::GetFirewallRuleList(PRL_FIREWALL_DIRECTION nDirection,
		PRL_HANDLE_PTR phRuleList)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER

	CVmNetFirewallDirection *pDirection;
	if (nDirection == PFD_INCOMING)
		pDirection = m_pVmNetAdapter->getFirewall()->getIncoming()->getDirection();
	else
		pDirection = m_pVmNetAdapter->getFirewall()->getOutgoing()->getDirection();

	QList<CVmNetFirewallRule* > lstEntries = pDirection->getFirewallRules()->m_lstFirewallRules;
	QList<PrlHandleBasePtr> list;
	foreach( CVmNetFirewallRule* pEntry, lstEntries )
	{
		PrlHandleFirewallRule* pRule = new PrlHandleFirewallRule;
		if ( ! pRule )
			return PRL_ERR_OUT_OF_MEMORY;

		pRule->fromString( QSTR2UTF8(pEntry->toString()) );

		list += PrlHandleBasePtr(pRule);
		pRule->Release();
	}

	PrlHandleHandlesList* pList = new PrlHandleHandlesList;
	if ( ! pList )
		return PRL_ERR_OUT_OF_MEMORY;

	pList->SetHandlesList(list);

	*phRuleList = pList->GetHandle();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmDeviceNetAdapter::SetFirewallRuleList(PRL_FIREWALL_DIRECTION nDirection,
		PRL_HANDLE hRuleList)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_NET_ADAPTER

	CVmNetFirewallDirection *pDirection;
	if (nDirection == PFD_INCOMING)
		pDirection = m_pVmNetAdapter->getFirewall()->getIncoming()->getDirection();
	else
		pDirection = m_pVmNetAdapter->getFirewall()->getOutgoing()->getDirection();

	pDirection->getFirewallRules()->ClearLists();

	if (hRuleList == PRL_INVALID_HANDLE)
		return PRL_ERR_SUCCESS;

	QList<CVmNetFirewallRule* >& lstEntries = pDirection->getFirewallRules()->m_lstFirewallRules;

	PrlHandleHandlesListPtr pList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hRuleList);
	QList<PrlHandleBasePtr> list = pList->GetHandlesList();

	for( int i = 0; i < list.size(); i++ )
	{
		CVmNetFirewallRule* pEntry = new CVmNetFirewallRule;
		if ( ! pEntry )
			return PRL_ERR_OUT_OF_MEMORY;

		PrlHandleFirewallRule* pRule = static_cast<PrlHandleFirewallRule* >(list[i].getHandle());
		pEntry->fromString(pRule->toString());

		lstEntries += pEntry;
	}
	return PRL_ERR_SUCCESS;
}
