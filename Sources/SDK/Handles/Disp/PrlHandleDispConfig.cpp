/*
 * PrlHandleDispConfig.cpp
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


#include "PrlHandleDispConfig.h"
#include "PrlHandleDispNet.h"
#include "PrlHandleOpTypeList.h"
#include "PrlHandleUsbIdentity.h"
#include "PrlHandleCpuFeatures.h"
#include <prlxmlmodel/DispConfig/CDispUser.h>
#include "PrlHandleCpuPool.h"

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


PrlHandleDispConfig::PrlHandleDispConfig(const QString& sDispConfig, const QString& sNetworkConfig)
: PrlHandleBase(PHT_DISP_CONFIG)
{
		m_DispConfig.fromString(sDispConfig);
		m_NetworkConfig.fromString(sNetworkConfig);
}

PRL_RESULT PrlHandleDispConfig::GetDefaultVmDir(PRL_STR sDefaultVmDirBuf, PRL_UINT32_PTR pnDefaultVmDirBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_DispConfig.getWorkspacePreferences()->getDefaultVmDirectory(), sDefaultVmDirBuf,
													pnDefaultVmDirBufLength);
}

PRL_RESULT PrlHandleDispConfig::GetDefaultCtDir(PRL_STR sDefaultVmDirBuf, PRL_UINT32_PTR pnDefaultVmDirBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_DispConfig.getDispVirtuozzoPreferences()->getDefaultCtDirectory(), sDefaultVmDirBuf,
													pnDefaultVmDirBufLength);
}

PRL_RESULT PrlHandleDispConfig::GetReservedMemLimit(PRL_UINT32_PTR pnMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMemSize = m_DispConfig.getMemoryPreferences()->getReservedMemoryLimit();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetReservedMemLimit(PRL_UINT32 nMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getMemoryPreferences()->setReservedMemoryLimit(nMemSize);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetMinVmMem(PRL_UINT32_PTR pnMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMemSize = m_DispConfig.getMemoryPreferences()->getMinVmMemory();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetMinVmMem(PRL_UINT32 nMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getMemoryPreferences()->setMinVmMemory(nMemSize);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetMaxVmMem(PRL_UINT32_PTR pnMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMemSize = m_DispConfig.getMemoryPreferences()->getMaxVmMemory();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetMaxVmMem(PRL_UINT32 nMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getMemoryPreferences()->setMaxVmMemory(nMemSize);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetRecommendMaxVmMem(PRL_UINT32_PTR pnMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMemSize = m_DispConfig.getMemoryPreferences()->getRecommendedMaxVmMemory();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetRecommendMaxVmMem(PRL_UINT32 nMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getMemoryPreferences()->setRecommendedMaxVmMemory(nMemSize);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetMaxReservMemLimit(PRL_UINT32_PTR pnMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMemSize = m_DispConfig.getMemoryPreferences()->getMaxReservedMemoryLimit();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetMaxReservMemLimit(PRL_UINT32 nMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getMemoryPreferences()->setMaxReservedMemoryLimit(nMemSize);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetMinReservMemLimit(PRL_UINT32_PTR pnMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMemSize = m_DispConfig.getMemoryPreferences()->getMinReservedMemoryLimit();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetMinReservMemLimit(PRL_UINT32 nMemSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getMemoryPreferences()->setMinReservedMemoryLimit(nMemSize);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::IsAdjustMemAuto(PRL_BOOL_PTR pbAdjustMemAuto)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbAdjustMemAuto = m_DispConfig.getMemoryPreferences()->isAdjustMemAuto();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetAdjustMemAuto(PRL_BOOL bAdjustMemAuto)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getMemoryPreferences()->setAdjustMemAuto(bAdjustMemAuto);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::IsSendStatisticReport(PRL_BOOL_PTR pbSendStatisticReport)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbSendStatisticReport = m_DispConfig.getWorkspacePreferences()->isEnableSendStatisticReport();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetSendStatisticReport(PRL_BOOL bSendStatisticReport)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getWorkspacePreferences()->setEnableSendStatisticReport(bSendStatisticReport);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetRemoteDisplayBasePort(PRL_UINT32_PTR pnPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnPort = m_DispConfig.getRemoteDisplayPreferences()->getBasePort();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetRemoteDisplayBasePort(PRL_UINT32 nPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getRemoteDisplayPreferences()->setBasePort(nPort);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetRemoteDisplayMaxPort(PRL_UINT32_PTR pnPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnPort = m_DispConfig.getRemoteDisplayPreferences()->getMaxPort();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetRemoteDisplayBaseHostName(PRL_STR sHostNameBuf, PRL_UINT32_PTR pnHostNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_DispConfig.getRemoteDisplayPreferences()->getBaseHostName(), sHostNameBuf,
		pnHostNameBufLength);
}

PRL_RESULT PrlHandleDispConfig::SetRemoteDisplayBaseHostName(PRL_CONST_STR sNewHostName)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewHostNameUtf8 = UTF8_2QSTR(sNewHostName);
	m_DispConfig.getRemoteDisplayPreferences()->setBaseHostName(sNewHostNameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::CreateDispNet(PRL_HANDLE_PTR phDispNet)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleDispNet *pDispNet = new PrlHandleDispNet(PrlHandleDispConfigPtr(this));
	if (!pDispNet)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phDispNet = pDispNet->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetDispNetCount(PRL_UINT32_PTR pnDispNetCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnDispNetCount = m_DispConfig.getNetworkPreferences()->getNetAdapters()->size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetDispNet(PRL_UINT32 nDispNetIndex, PRL_HANDLE_PTR phDispNet)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nDispNetIndex >= PRL_UINT32(m_DispConfig.getNetworkPreferences()->getNetAdapters()->size()))
		return (PRL_ERR_INVALID_ARG);
	CDispNetAdapter *pNetAdapter = m_DispConfig.getNetworkPreferences()->getNetAdapters()->value(nDispNetIndex);
	PrlHandleDispNet *pDispNet = new PrlHandleDispNet(PrlHandleDispConfigPtr(this), pNetAdapter);
	*phDispNet = pDispNet->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::CanChangeDefaultSettings(PRL_BOOL_PTR pbDefaultChangeSettings)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbDefaultChangeSettings = m_DispConfig.getWorkspacePreferences()->isDefaultChangeSettings();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetCanChangeDefaultSettings(PRL_BOOL bDefaultChangeSettings)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getWorkspacePreferences()->setDefaultChangeSettings(bDefaultChangeSettings);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetMinSecurityLevel(PRL_SECURITY_LEVEL_PTR pnMinSecurityLevel)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMinSecurityLevel = m_DispConfig.getWorkspacePreferences()->getMinimalSecurityLevel();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetMinSecurityLevel(PRL_SECURITY_LEVEL nMinSecurityLevel)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getWorkspacePreferences()->setMinimalSecurityLevel(nMinSecurityLevel);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::fromString(PRL_CONST_STR sDispConfig)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	foreach(CDispNetAdapter *pAdapter, *m_DispConfig.getNetworkPreferences()->getNetAdapters())
		PrlControlValidity::MarkAsInvalid(pAdapter);
	if (PRL_SUCCEEDED(m_DispConfig.fromString(UTF8_2QSTR(sDispConfig))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleDispConfig::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_DispConfig.toString();
}

PRL_RESULT PrlHandleDispConfig::GetConfirmationsList(PRL_HANDLE_PTR phConfirmationsList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;

	TOpaqueTypeList<PRL_ALLOWED_VM_COMMAND> *pOpaqueList = new TOpaqueTypeList<PRL_ALLOWED_VM_COMMAND>;
	if (!pOpaqueList)
		return (PRL_ERR_OUT_OF_MEMORY);

	QList<PRL_ALLOWED_VM_COMMAND> &lstRef = pOpaqueList->GetContainer();
	lstRef	= m_DispConfig.getLockedOperationsList()->getLockedOperations();

	PrlHandleOpTypeList *pOpaqueListHandle = new PrlHandleOpTypeList(pOpaqueList);
	if (!pOpaqueListHandle)
	{
		delete pOpaqueList;
		return (PRL_ERR_OUT_OF_MEMORY);
	}

	*phConfirmationsList = pOpaqueListHandle->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetConfirmationsList(PrlHandleOpTypeListPtr pOpaqueList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;

	PRL_RESULT nErr;

	PRL_SIZE nTypeSize;
	nErr = pOpaqueList->GetTypeSize(&nTypeSize);
	if( PRL_FAILED(nErr) )
		return nErr;

	PRL_UINT8* p = new PRL_UINT8[nTypeSize];
	SmartPtr< PRL_UINT8 > pBuff( p, SmartPtrPolicy::ArrayStorage );

	PRL_UINT32 nCount;
	nErr = pOpaqueList->GetItemsCount(&nCount);
	if( PRL_FAILED(nErr) )
		return nErr;

	QList<PRL_ALLOWED_VM_COMMAND> lst;
	for( PRL_UINT32 idx=0; idx < nCount; idx++ )
	{
		nErr = pOpaqueList->GetItem(idx, pBuff.getImpl() );
		if( PRL_FAILED(nErr) )
			return nErr;

		PRL_ALLOWED_VM_COMMAND cmd;

		switch (nTypeSize)
		{
#define CASE_BUFF_TO_CMD(type) \
		case sizeof(type): \
			{ \
				type* p = reinterpret_cast< type* >(pBuff.getImpl()); \
				cmd = (PRL_ALLOWED_VM_COMMAND)(*p); \
				break; \
			}
			CASE_BUFF_TO_CMD(PRL_UINT8)
			CASE_BUFF_TO_CMD(PRL_UINT16);
			CASE_BUFF_TO_CMD(PRL_UINT32);
			CASE_BUFF_TO_CMD(PRL_UINT64);
			default: return (PRL_ERR_INVALID_ARG);
#undef CASE_BUFF_TO_CMD
		}

		lst.append( (PRL_ALLOWED_VM_COMMAND)cmd );
	}
	m_DispConfig.getLockedOperationsList()->setLockedOperations( lst );

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetDefaultBackupServer(PRL_STR sBackupServer, PRL_UINT32_PTR pnBackupServerBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue( m_DispConfig.getBackupSourcePreferences()->getDefaultBackupServer(),
							sBackupServer,
							pnBackupServerBufLength);
}

PRL_RESULT PrlHandleDispConfig::SetDefaultBackupServer(PRL_CONST_STR sBackupServer)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getBackupSourcePreferences()->setDefaultBackupServer(UTF8_2QSTR(sBackupServer));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetBackupTmpDir(PRL_STR sBackupTmpDir, PRL_UINT32_PTR pnBackupTmpDirBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue( m_DispConfig.getBackupSourcePreferences()->getTmpDir(),
							sBackupTmpDir,
							pnBackupTmpDirBufLength);
}

PRL_RESULT PrlHandleDispConfig::SetBackupTmpDir(PRL_CONST_STR sBackupTmpDir)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getBackupSourcePreferences()->setTmpDir(UTF8_2QSTR(sBackupTmpDir));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetBackupUserLogin(PRL_STR sUserLogin, PRL_UINT32_PTR pnUserLoginBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue( m_DispConfig.getBackupSourcePreferences()->getLogin(),
							sUserLogin,
							pnUserLoginBufLength);
}

PRL_RESULT PrlHandleDispConfig::SetBackupUserLogin(PRL_CONST_STR sUserLogin)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getBackupSourcePreferences()->setLogin(UTF8_2QSTR(sUserLogin));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetBackupUserPassword(PRL_CONST_STR sUserPassword)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getBackupSourcePreferences()->setPassword(UTF8_2QSTR(sUserPassword));
	m_DispConfig.getBackupSourcePreferences()->setPasswordChanged(true);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::IsBackupUserPasswordEnabled(PRL_BOOL_PTR pbUserPasswordEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbUserPasswordEnabled = m_DispConfig.getBackupSourcePreferences()->isUsePassword();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetBackupUserPasswordEnabled(PRL_BOOL bUserPasswordEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getBackupSourcePreferences()->setUsePassword(bUserPasswordEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetDefaultBackupDirectory(PRL_STR sBackupDirectory, PRL_UINT32_PTR pnBackupDirectoryBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue( m_DispConfig.getBackupTargetPreferences()->getDefaultBackupDirectory(),
							sBackupDirectory,
							pnBackupDirectoryBufLength);
}

PRL_RESULT PrlHandleDispConfig::SetDefaultBackupDirectory(PRL_CONST_STR sBackupDirectory)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getBackupTargetPreferences()->setDefaultBackupDirectory(UTF8_2QSTR(sBackupDirectory));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetBackupTimeout(PRL_UINT32_PTR pnTimeout)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnTimeout = m_DispConfig.getBackupSourcePreferences()->getTimeout();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetBackupTimeout(PRL_UINT32 nTimeout)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getBackupSourcePreferences()->setTimeout(nTimeout);
	return (PRL_ERR_SUCCESS);
}


PRL_RESULT PrlHandleDispConfig::ArePluginsEnabled(PRL_BOOL_PTR pbIsPluginsAllowed)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbIsPluginsAllowed = (PRL_BOOL)m_DispConfig.getWorkspacePreferences()->isPluginsAllowed();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::EnablePlugins(PRL_BOOL bEnablePluginsSupport)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getWorkspacePreferences()->setPluginsAllowed( bEnablePluginsSupport );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetVmCpuLimitType(PRL_UINT32_PTR pnVmCpuLimitType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (pnVmCpuLimitType == NULL)
		return (PRL_ERR_INVALID_ARG);
	*pnVmCpuLimitType = m_DispConfig.getWorkspacePreferences()->getVmGuestCpuLimitType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetVmCpuLimitType(PRL_UINT32 nVmCpuLimitType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getWorkspacePreferences()->setVmGuestCpuLimitType(nVmCpuLimitType);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::IsVerboseLogEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_DispConfig.getDebug()->isVerboseLogEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetVerboseLogEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getDebug()->setVerboseLogEnabled(bEnabled);
	m_DispConfig.getDebug()->setVerboseLogWasChanged(true);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::IsAllowMultiplePMC(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_DispConfig.getWorkspacePreferences()->isAllowMultiplePMC();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetAllowMultiplePMC(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getWorkspacePreferences()->setAllowMultiplePMC(bEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::IsAllowDirectMobile(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_DispConfig.getWorkspacePreferences()->isAllowDirectMobile();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetAllowDirectMobile(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getWorkspacePreferences()->setAllowDirectMobile(bEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::IsAllowMobileClients(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_DispConfig.getWorkspacePreferences()->isAllowMobileClients();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetUsbIdentityCount(PRL_UINT32_PTR pnUsbIdentityCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnUsbIdentityCount = m_DispConfig.getUsbPreferences()->m_lstAuthenticUsbMap.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetUsbIdentity(PRL_UINT32 nUsbIdentIndex, PRL_HANDLE_PTR phUsbIdent)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if ( nUsbIdentIndex >= PRL_UINT32(m_DispConfig.getUsbPreferences()->m_lstAuthenticUsbMap.size()) )
			return (PRL_ERR_INVALID_ARG);
	QString sUsbIdentity = m_DispConfig.getUsbPreferences()->m_lstAuthenticUsbMap.value(nUsbIdentIndex)->toString();
	PrlHandleUsbIdentity * pUsbIdentity = new PrlHandleUsbIdentity( sUsbIdentity );
	if( !pUsbIdentity )
		return PRL_ERR_OUT_OF_MEMORY;
	*phUsbIdent = pUsbIdentity->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetUsbIdentAssociation(PRL_CONST_STR sSystemName,
													   PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
	Q_UNUSED(nFlags);
	SYNCHRO_INTERNAL_DATA_ACCESS
	foreach (CDispUsbIdentity * pUi, m_DispConfig.getUsbPreferences()->m_lstAuthenticUsbMap) {
		if( pUi->getSystemName() != sSystemName )
			continue;
		if( sVmUuid && *sVmUuid )
		{	// Set Associated Uuid
			CDispUsbAssociation * pUa = NULL;
			if( pUi->m_lstAssociations.isEmpty() )
			{
				pUa = new CDispUsbAssociation;
				if(!pUa)
					return PRL_ERR_OUT_OF_MEMORY;
				pUi->m_lstAssociations.append( pUa );
			} else
				pUa = pUi->m_lstAssociations[0];
			pUa->setAction( PUD_CONNECT_TO_GUEST_OS );
			pUa->setVmUuid( QString(sVmUuid) );
		} else
		{	// Clear Associated Uuid
			if( pUi->m_lstAssociations.isEmpty() )
				return (PRL_ERR_SUCCESS);

			CDispUsbAssociation * pUa = pUi->m_lstAssociations.takeLast();
			if( pUa )
				delete pUa;
		}
		return (PRL_ERR_SUCCESS);
	}
	return PRL_ERR_INVALID_ARG;
}

PRL_RESULT PrlHandleDispConfig::GetCpuFeaturesMask(PRL_CPU_FEATURES_MASK_PTR pMasks)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	pMasks->nFEATURES_MASK = m_DispConfig.getCpuPreferences()->getFEATURES_MASK();
	pMasks->nEXT_FEATURES_MASK = m_DispConfig.getCpuPreferences()->getEXT_FEATURES_MASK();
	pMasks->nEXT_80000001_ECX_MASK = m_DispConfig.getCpuPreferences()->getEXT_80000001_ECX_MASK();
	pMasks->nEXT_80000001_EDX_MASK = m_DispConfig.getCpuPreferences()->getEXT_80000001_EDX_MASK();
	pMasks->nEXT_80000007_EDX_MASK = m_DispConfig.getCpuPreferences()->getEXT_80000007_EDX_MASK();
	pMasks->nEXT_80000008_EAX = m_DispConfig.getCpuPreferences()->getEXT_80000008_EAX();
	pMasks->nEXT_00000007_EBX_MASK = m_DispConfig.getCpuPreferences()->getEXT_00000007_EBX_MASK();
	pMasks->nEXT_0000000D_EAX_MASK = m_DispConfig.getCpuPreferences()->getEXT_0000000D_EAX_MASK();
	pMasks->nEXT_00000007_EDX_MASK = m_DispConfig.getCpuPreferences()->getEXT_00000007_EDX_MASK();
	pMasks->bIsValid = m_DispConfig.getCpuPreferences()->isCpuFeaturesMaskValid();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleDispConfig::SetCpuFeaturesMaskExImpl(const PrlHandleCpuFeatures &features)
{
	PRL_UINT32 nMask;

	features.GetValue(PCFE_FEATURES, &nMask);
	m_DispConfig.getCpuPreferences()->setFEATURES_MASK(nMask);

	features.GetValue(PCFE_EXT_FEATURES, &nMask);
	m_DispConfig.getCpuPreferences()->setEXT_FEATURES_MASK(nMask);

	features.GetValue(PCFE_EXT_80000001_ECX, &nMask);
	m_DispConfig.getCpuPreferences()->setEXT_80000001_ECX_MASK(nMask);

	features.GetValue(PCFE_EXT_80000001_EDX, &nMask);
	m_DispConfig.getCpuPreferences()->setEXT_80000001_EDX_MASK(nMask);

	features.GetValue(PCFE_EXT_80000007_EDX, &nMask);
	m_DispConfig.getCpuPreferences()->setEXT_80000007_EDX_MASK(nMask);

	features.GetValue(PCFE_EXT_80000008_EAX, &nMask);
	m_DispConfig.getCpuPreferences()->setEXT_80000008_EAX(nMask);

	features.GetValue(PCFE_EXT_00000007_EBX, &nMask);
	m_DispConfig.getCpuPreferences()->setEXT_00000007_EBX_MASK(nMask);

	features.GetValue(PCFE_EXT_00000007_EDX, &nMask);
	m_DispConfig.getCpuPreferences()->setEXT_00000007_EDX_MASK(nMask);

	features.GetValue(PCFE_EXT_0000000D_EAX, &nMask);
	m_DispConfig.getCpuPreferences()->setEXT_0000000D_EAX_MASK(nMask);

	m_DispConfig.getCpuPreferences()->setCpuFeaturesMaskValid(PRL_TRUE);

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetCpuFeaturesMask(PRL_CONST_CPU_FEATURES_MASK_PTR pMasks)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	PRL_CPU_FEATURES_MASK masks = *pMasks;
	if (!masks.bIsValid) {
		masks.nFEATURES_MASK = 0xFFFFFFFF;
		masks.nEXT_FEATURES_MASK = 0xFFFFFFFF;
		masks.nEXT_80000001_ECX_MASK = 0xFFFFFFFF;
		masks.nEXT_80000001_EDX_MASK = 0xFFFFFFFF;
		masks.nEXT_80000007_EDX_MASK = 0xFFFFFFFF;
		masks.nEXT_80000008_EAX = 0xFFFFFFFF;
		masks.nEXT_00000007_EBX_MASK = 0xFFFFFFFF;
		masks.nEXT_00000007_EDX_MASK = 0xFFFFFFFF;
	}

	PrlHandleCpuFeatures *pFeatures = new PrlHandleCpuFeatures();
	if (!pFeatures)
		return (PRL_ERR_OUT_OF_MEMORY);

	pFeatures->SetValue(PCFE_FEATURES, masks.nFEATURES_MASK);
	pFeatures->SetValue(PCFE_EXT_FEATURES, masks.nEXT_FEATURES_MASK);
	pFeatures->SetValue(PCFE_EXT_80000001_ECX, masks.nEXT_80000001_ECX_MASK);
	pFeatures->SetValue(PCFE_EXT_80000001_EDX, masks.nEXT_80000001_EDX_MASK);
	pFeatures->SetValue(PCFE_EXT_80000007_EDX, masks.nEXT_80000007_EDX_MASK);
	pFeatures->SetValue(PCFE_EXT_80000008_EAX, masks.nEXT_80000008_EAX);
	pFeatures->SetValue(PCFE_EXT_00000007_EBX, masks.nEXT_00000007_EBX_MASK);
	pFeatures->SetValue(PCFE_EXT_00000007_EDX, masks.nEXT_00000007_EDX_MASK);

	PRL_RESULT r = SetCpuFeaturesMaskExImpl(*pFeatures);

	pFeatures->Release();
	return r;
}

PRL_RESULT PrlHandleDispConfig::GetCpuFeaturesMaskEx(PRL_HANDLE_PTR phCpuFeatures)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	PrlHandleCpuFeatures *pFeatures = new PrlHandleCpuFeatures();
	if (!pFeatures)
		return (PRL_ERR_OUT_OF_MEMORY);

	pFeatures->SetValue(PCFE_FEATURES, m_DispConfig.getCpuPreferences()->getFEATURES_MASK());
	pFeatures->SetValue(PCFE_EXT_FEATURES, m_DispConfig.getCpuPreferences()->getEXT_FEATURES_MASK());
	pFeatures->SetValue(PCFE_EXT_80000001_ECX, m_DispConfig.getCpuPreferences()->getEXT_80000001_ECX_MASK());
	pFeatures->SetValue(PCFE_EXT_80000001_EDX, m_DispConfig.getCpuPreferences()->getEXT_80000001_EDX_MASK());
	pFeatures->SetValue(PCFE_EXT_80000007_EDX, m_DispConfig.getCpuPreferences()->getEXT_80000007_EDX_MASK());
	pFeatures->SetValue(PCFE_EXT_80000008_EAX, m_DispConfig.getCpuPreferences()->getEXT_80000008_EAX());
	pFeatures->SetValue(PCFE_EXT_00000007_EBX, m_DispConfig.getCpuPreferences()->getEXT_00000007_EBX_MASK());
	pFeatures->SetValue(PCFE_EXT_00000007_EDX, m_DispConfig.getCpuPreferences()->getEXT_00000007_EDX_MASK());
	pFeatures->SetValue(PCFE_EXT_0000000D_EAX, m_DispConfig.getCpuPreferences()->getEXT_0000000D_EAX_MASK());

	*phCpuFeatures = pFeatures->GetHandle();

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetCpuFeaturesMaskEx(PRL_HANDLE hCpuFeatures)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	PrlHandleCpuFeaturesPtr pFeatures = PRL_OBJECT_BY_HANDLE<PrlHandleCpuFeatures>( hCpuFeatures );

	return SetCpuFeaturesMaskExImpl(*pFeatures);
}

PRL_RESULT PrlHandleDispConfig::IsLogRotationEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_DispConfig.getLogRotatePreferences()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetLogRotationEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getLogRotatePreferences()->setEnabled(bEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::IsAllowAttachScreenshotsEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_DispConfig.getProblemReportPreferences()->isAllowAttachScreenshots();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetAllowAttachScreenshotsEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getProblemReportPreferences()->setAllowAttachScreenshots(bEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetMobileAdvancedAuthMode(PRL_MOBILE_ADVANCED_AUTH_MODE_PTR pnMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMode = m_DispConfig.getWorkspacePreferences()->getMobileAdvancedAuthMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::SetMobileAdvancedAuthMode(PRL_MOBILE_ADVANCED_AUTH_MODE nMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getWorkspacePreferences()->setMobileAdvancedAuthMode(nMode);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleDispConfig::GetHostId(PRL_STR sHostId, PRL_UINT32_PTR pnHostIdBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_DispConfig.getWorkspacePreferences()->getHostId(),
						   sHostId, pnHostIdBufLength);
}

PRL_RESULT PrlHandleDispConfig::SetHostId(PRL_CONST_STR sHostId)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_DispConfig.getWorkspacePreferences()->setHostId(UTF8_2QSTR(sHostId));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleDispConfig::GetCpuPool(PRL_HANDLE_PTR phCpuPool)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;

	if (m_DispConfig.getCpuPoolInfo()->getName().isEmpty()) {
		*phCpuPool = PRL_INVALID_HANDLE;
		return (PRL_ERR_SUCCESS);
	}

	CCpuPool cpuPool;
	CCpuFeatures *cpuFeatures = new CCpuFeatures();
	if (!cpuFeatures)
		return (PRL_ERR_OUT_OF_MEMORY);
	cpuPool.setCpuMask(cpuFeatures);

	CDispCpuPreferences *prefs = m_DispConfig.getCpuPreferences();
	cpuFeatures->setFEATURES(prefs->getFEATURES_MASK());
	cpuFeatures->setEXT_FEATURES(prefs->getEXT_FEATURES_MASK());
	cpuFeatures->setEXT_80000001_ECX(prefs->getEXT_80000001_ECX_MASK());
	cpuFeatures->setEXT_80000001_EDX(prefs->getEXT_80000001_EDX_MASK());
	cpuFeatures->setEXT_80000001_EDX(prefs->getEXT_80000001_EDX_MASK());
	cpuFeatures->setEXT_80000007_EDX(prefs->getEXT_80000007_EDX_MASK());
	cpuFeatures->setEXT_80000008_EAX(prefs->getEXT_80000008_EAX());
	cpuFeatures->setEXT_00000007_EBX(prefs->getEXT_00000007_EBX_MASK());
	cpuFeatures->setEXT_00000007_EDX(prefs->getEXT_00000007_EDX_MASK());
	cpuFeatures->setEXT_0000000D_EAX(prefs->getEXT_0000000D_EAX_MASK());


	CCpuPoolInfo *poolInfo = new CCpuPoolInfo(m_DispConfig.getCpuPoolInfo());
	if (!poolInfo)
		return (PRL_ERR_OUT_OF_MEMORY);
	cpuPool.setCpuPoolInfo(poolInfo);

	PrlHandleCpuPool *pCpuPoolHandle = new PrlHandleCpuPool(cpuPool);
	if (!pCpuPoolHandle)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phCpuPool = pCpuPoolHandle->GetHandle();
	return (PRL_ERR_SUCCESS);
}
