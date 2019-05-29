/*
 * PrlHandleDispNet.cpp
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


#include "PrlHandleDispNet.h"
#include <prlxmlmodel/DispConfig/CDispNetAdapter.h>
#include <prlxmlmodel/DispConfig/CDispDhcpPreferences.h>

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


#define CHECK_DISP_NET_OBJECT\
	if (!m_pDispNet)\
		TryToRestoreObjectElement();\
	if (!m_pDispNet)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleDispNet::PrlHandleDispNet(const PrlHandleDispConfigPtr &pDispCfg)
: PrlHandleBase(PHT_DISP_NET_ADAPTER), PrlControlValidity(GetHandle()), m_pDispCfg(pDispCfg)
{
	m_pDispNet = new CDispNetAdapter;
	SYNCHRO_PARENT_DISP_CONFIG
	m_pDispCfg->GetDispConfig().getNetworkPreferences()->addNetAdapter(m_pDispNet);
	PrlControlValidity::RegisterInstance(GetHandle(), m_pDispNet);
}

PrlHandleDispNet::PrlHandleDispNet(const PrlHandleDispConfigPtr &pDispCfg, CDispNetAdapter *pDispNet)
: PrlHandleBase(PHT_DISP_NET_ADAPTER), PrlControlValidity(GetHandle()), m_pDispNet(pDispNet), m_pDispCfg(pDispCfg)
{
	PrlControlValidity::RegisterInstance(GetHandle(), m_pDispNet);
}

PrlHandleDispNet::~PrlHandleDispNet()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pDispNet = NULL;
}

PRL_RESULT PrlHandleDispNet::Remove()
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	CDispNetAdapter *pDispNet = m_pDispNet;
	m_pDispCfg->GetDispConfig().getNetworkPreferences()->getNetAdapters()->removeAll(pDispNet);
	PrlControlValidity::MarkAsInvalid(pDispNet);
	delete pDispNet;
	m_pDispNet = NULL;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::IsEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	*pbEnabled = m_pDispNet->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::SetEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	m_pDispNet->setEnabled(bEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::IsHidden(PRL_BOOL_PTR pbHidden)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	*pbHidden = m_pDispNet->isHiddenAdapter();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::SetHidden(PRL_BOOL bHidden)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	m_pDispNet->setHiddenAdapter(bHidden);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetNetworkType(PRL_NET_ADAPTER_EMULATED_TYPE_PTR pnNetworkType)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	*pnNetworkType = m_pDispNet->getNetworkType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::SetNetworkType(PRL_NET_ADAPTER_EMULATED_TYPE nNetworkType)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	m_pDispNet->setNetworkType(nNetworkType);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetName(PRL_STR sDispNetNameBuf, PRL_UINT32_PTR pnDispNetNameBufLength)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	return CopyStringValue(m_pDispNet->getName(), sDispNetNameBuf, pnDispNetNameBufLength);
}

PRL_RESULT PrlHandleDispNet::SetName(PRL_CONST_STR sNewDispNetName)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	QString sNewDispNetNameUtf8 = UTF8_2QSTR(sNewDispNetName);
	m_pDispNet->setName(sNewDispNetNameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetUuid(PRL_STR sDispNetUuidBuf, PRL_UINT32_PTR pnDispNetUuidBufLength)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	return CopyStringValue(m_pDispNet->getUuid(), sDispNetUuidBuf, pnDispNetUuidBufLength);
}

PRL_RESULT PrlHandleDispNet::GetSysName(PRL_STR sDispNetSysNameBuf, PRL_UINT32_PTR pnDispNetSysNameBufLength)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	return CopyStringValue(m_pDispNet->getSysName(), sDispNetSysNameBuf, pnDispNetSysNameBufLength);
}

PRL_RESULT PrlHandleDispNet::SetSysName(PRL_CONST_STR sNewDispNetSysName)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	QString sNewDispNetSysNameUtf8 = UTF8_2QSTR(sNewDispNetSysName);
	m_pDispNet->setSysName(sNewDispNetSysNameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetIndex(PRL_UINT32_PTR pnIndex)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	*pnIndex = m_pDispNet->getIndex();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::SetIndex(PRL_UINT32 nIndex)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	m_pDispNet->setIndex(nIndex);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::IsDhcpEnabled(PRL_BOOL_PTR pbDhcpEnabled)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		*pbDhcpEnabled = m_pDispNet->getDhcpPreferences()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::SetDhcpEnabled(PRL_BOOL bDhcpEnabled)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		m_pDispNet->getDhcpPreferences()->setEnabled(bDhcpEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetDhcpScopeStartIp(PRL_STR sDispNetDhcpScopeStartIpBuf, PRL_UINT32_PTR pnDispNetDhcpScopeStartIpBufLength)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	return CopyStringValue(m_pDispNet->getDhcpPreferences()->getDhcpScopeStartIp().toString(), sDispNetDhcpScopeStartIpBuf,
													pnDispNetDhcpScopeStartIpBufLength);
}

PRL_RESULT PrlHandleDispNet::SetDhcpScopeStartIp(PRL_CONST_STR sNewDispNetDhcpScopeStartIp)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	QString sNewDispNetDhcpScopeStartIpUtf8 = UTF8_2QSTR(sNewDispNetDhcpScopeStartIp);
	m_pDispNet->getDhcpPreferences()->setDhcpScopeStartIp(QHostAddress(sNewDispNetDhcpScopeStartIpUtf8));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetDhcpScopeEndIp(PRL_STR sDispNetDhcpScopeEndIpBuf, PRL_UINT32_PTR pnDispNetDhcpScopeEndIpBufLength)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	return CopyStringValue(m_pDispNet->getDhcpPreferences()->getDhcpScopeEndIp().toString(), sDispNetDhcpScopeEndIpBuf,
													pnDispNetDhcpScopeEndIpBufLength);
}

PRL_RESULT PrlHandleDispNet::SetDhcpScopeEndIp(PRL_CONST_STR sNewDispNetDhcpScopeEndIp)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	QString sNewDispNetDhcpScopeEndIpUtf8 = UTF8_2QSTR(sNewDispNetDhcpScopeEndIp);
	m_pDispNet->getDhcpPreferences()->setDhcpScopeEndIp(QHostAddress(sNewDispNetDhcpScopeEndIpUtf8));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetDhcpScopeMask(PRL_STR sDispNetDhcpScopeMaskBuf, PRL_UINT32_PTR pnDispNetDhcpScopeMaskBufLength)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	return CopyStringValue(m_pDispNet->getDhcpPreferences()->getDhcpScopeMask().toString(), sDispNetDhcpScopeMaskBuf,
													pnDispNetDhcpScopeMaskBufLength);
}

PRL_RESULT PrlHandleDispNet::SetDhcpScopeMask(PRL_CONST_STR sNewDispNetDhcpScopeMask)
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT
	QString sNewDispNetDhcpScopeMaskUtf8 = UTF8_2QSTR(sNewDispNetDhcpScopeMask);
	m_pDispNet->getDhcpPreferences()->setDhcpScopeMask(QHostAddress(sNewDispNetDhcpScopeMaskUtf8));
	return (PRL_ERR_SUCCESS);
}

/* == IPV6 == */
PRL_RESULT PrlHandleDispNet::IsDhcp6Enabled(PRL_BOOL_PTR pbDhcp6Enabled)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		*pbDhcp6Enabled = m_pDispNet->getDhcpV6Preferences()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::SetDhcp6Enabled(PRL_BOOL bDhcp6Enabled)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		m_pDispNet->getDhcpV6Preferences()->setEnabled(bDhcp6Enabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetDhcp6ScopeStartIp(PRL_STR sDispNetDhcp6ScopeStartIpBuf, PRL_UINT32_PTR pnDispNetDhcp6ScopeStartIpBufLength)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		return CopyStringValue(m_pDispNet->getDhcpV6Preferences()->getDhcpScopeStartIp().toString(), sDispNetDhcp6ScopeStartIpBuf,
		pnDispNetDhcp6ScopeStartIpBufLength);
}

PRL_RESULT PrlHandleDispNet::SetDhcp6ScopeStartIp(PRL_CONST_STR sNewDispNetDhcp6ScopeStartIp)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		QString sNewDispNetDhcp6ScopeStartIpUtf8 = UTF8_2QSTR(sNewDispNetDhcp6ScopeStartIp);
	m_pDispNet->getDhcpV6Preferences()->setDhcpScopeStartIp(QHostAddress(sNewDispNetDhcp6ScopeStartIpUtf8));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetDhcp6ScopeEndIp(PRL_STR sDispNetDhcp6ScopeEndIpBuf, PRL_UINT32_PTR pnDispNetDhcp6ScopeEndIpBufLength)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		return CopyStringValue(m_pDispNet->getDhcpV6Preferences()->getDhcpScopeEndIp().toString(), sDispNetDhcp6ScopeEndIpBuf,
		pnDispNetDhcp6ScopeEndIpBufLength);
}

PRL_RESULT PrlHandleDispNet::SetDhcp6ScopeEndIp(PRL_CONST_STR sNewDispNetDhcp6ScopeEndIp)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		QString sNewDispNetDhcp6ScopeEndIpUtf8 = UTF8_2QSTR(sNewDispNetDhcp6ScopeEndIp);
	m_pDispNet->getDhcpV6Preferences()->setDhcpScopeEndIp(QHostAddress(sNewDispNetDhcp6ScopeEndIpUtf8));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::GetDhcp6ScopeMask(PRL_STR sDispNetDhcp6ScopeMaskBuf, PRL_UINT32_PTR pnDispNetDhcp6ScopeMaskBufLength)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		return CopyStringValue(m_pDispNet->getDhcpV6Preferences()->getDhcpScopeMask().toString(), sDispNetDhcp6ScopeMaskBuf,
		pnDispNetDhcp6ScopeMaskBufLength);
}

PRL_RESULT PrlHandleDispNet::SetDhcp6ScopeMask(PRL_CONST_STR sNewDispNetDhcp6ScopeMask)
{
	SYNCHRO_PARENT_DISP_CONFIG
		CHECK_DISP_NET_OBJECT
		QString sNewDispNetDhcp6ScopeMaskUtf8 = UTF8_2QSTR(sNewDispNetDhcp6ScopeMask);
	m_pDispNet->getDhcpV6Preferences()->setDhcpScopeMask(QHostAddress(sNewDispNetDhcp6ScopeMaskUtf8));
	return (PRL_ERR_SUCCESS);
}

void PrlHandleDispNet::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pDispNet = NULL;
}

quint32 PrlHandleDispNet::ConcreteGenerateHashCode()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return (qHash(m_pDispNet ? m_pDispNet->getUuid() : QString("")));
}

void PrlHandleDispNet::TryToRestoreObjectElement()
{
	Q_ASSERT(!m_pDispNet);
	Q_ASSERT(m_pDispCfg);
	foreach(CDispNetAdapter *pDispNet, *m_pDispCfg->GetDispConfig().
						getNetworkPreferences()->getNetAdapters())
	{
		if (GetHashCode() == qHash(pDispNet->getUuid()))
		{
			m_pDispNet = pDispNet;
			/* Reregister instance with new pointer */
			PrlControlValidity::RegisterInstance(GetHandle(), m_pDispNet);
			break;
		}
	}
}

PRL_RESULT PrlHandleDispNet::GetPortForwardingToString( PRL_VOID_PTR_PTR psPortForwarding )
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT

	*psPortForwarding = strdup( m_pDispNet->getPortForwarding()->toString().toUtf8().data() );
	if( !*psPortForwarding )
		return PRL_ERR_OUT_OF_MEMORY;

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispNet::SetPortForwardingFromString( PRL_CONST_STR sPortForwarding )
{
	SYNCHRO_PARENT_DISP_CONFIG
	CHECK_DISP_NET_OBJECT

	if( ! sPortForwarding )
		return PRL_ERR_INVALID_ARG;

	CDispPortForwarding* pPortForw =  new CDispPortForwarding;
	if( !pPortForw )
		return PRL_ERR_OUT_OF_MEMORY;

	if( 0 != pPortForw->fromString( sPortForwarding ) )
	{
		delete pPortForw;
		return PRL_ERR_INVALID_ARG;
	}

	m_pDispNet->setPortForwarding( pPortForw );
	return PRL_ERR_SUCCESS;
}
