/*
 * PrlApiNet.cpp
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


//#define FORCE_LOGGING_ON
//#define FORCE_LOGGING_LEVEL	DBG_DEBUG

#include "SDK/Include/Parallels.h"

#include <prlcommon/PrlCommonUtilsBase/PrlStringifyConsts.h>
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/Logging/Logging.h>

#include "PrlCommon.h"
#include "PrlHandleStringsList.h"
#include "PrlHandleServerNet.h"
#include "PrlHandleSrvConfig.h"
#include "PrlHandleVirtNet.h"
#include "PrlHandlePortFwd.h"
#include "PrlHandleNetServiceStatus.h"
#include "PrlHandleOfflineService.h"
#include "PrlHandleNetworkRate.h"
#include "PrlHandleNetworkClass.h"
#include "PrlHandleNetworkShaping.h"
#include "PrlHandleNetworkShapingBandwidth.h"
#include "PrlHandleNetworkShapingConfig.h"
#include "PrlHandleIPPrivNet.h"

#include "Build/Current.ver"

#include "PrlContextSwitcher.h"
#include "PrlNew.h"

////////////////////////////////////////////////////////////////////////////////////////////////


PRL_HANDLE PrlSrv_GetVirtualNetworkList_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION_WITH_FLAGS(GetVirtualNetworkList, nFlags, PJOC_SRV_GET_VIRTUAL_NETWORK_LIST)
}

PRL_ASYNC_METHOD( PrlSrv_GetVirtualNetworkList ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_VIRTUAL_NETWORK_LIST)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetVirtualNetworkList, (hServer, nFlags))
}

PRL_HANDLE PrlSrv_AddVirtualNetwork_Impl(PRL_HANDLE hServer, PRL_HANDLE hVirtNet, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_ADD_VIRTUAL_NETWORK));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleJobPtr pJob = pServer->AddVirtualNetwork(hVirtNet, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_AddVirtualNetwork ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hVirtNet,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_ADD_VIRTUAL_NETWORK)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_AddVirtualNetwork, (hServer, hVirtNet, nFlags))
}

PRL_HANDLE PrlSrv_UpdateVirtualNetwork_Impl(PRL_HANDLE hServer, PRL_HANDLE hVirtNet, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_UPDATE_VIRTUAL_NETWORK));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleJobPtr pJob = pServer->UpdateVirtualNetwork(hVirtNet, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_UpdateVirtualNetwork ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hVirtNet,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UPDATE_VIRTUAL_NETWORK)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UpdateVirtualNetwork, (hServer, hVirtNet, nFlags))
}

PRL_HANDLE PrlSrv_DeleteVirtualNetwork_Impl(PRL_HANDLE hServer, PRL_HANDLE hVirtNet, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_DELETE_VIRTUAL_NETWORK));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleJobPtr pJob = pServer->DeleteVirtualNetwork(hVirtNet, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_DeleteVirtualNetwork ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hVirtNet,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_DELETE_VIRTUAL_NETWORK)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_DeleteVirtualNetwork, (hServer, hVirtNet, nFlags))
}

PRL_HANDLE PrlSrv_NetServiceStart_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION(NetServiceStart, PJOC_SRV_NET_SERVICE_START)
}

PRL_ASYNC_METHOD( PrlSrv_NetServiceStart ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_NET_SERVICE_START)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_NetServiceStart, (hServer))
}

PRL_HANDLE PrlSrv_NetServiceStop_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION(NetServiceStop, PJOC_SRV_NET_SERVICE_STOP)
}

PRL_ASYNC_METHOD( PrlSrv_NetServiceStop ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_NET_SERVICE_STOP)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_NetServiceStop, (hServer))
}

PRL_HANDLE PrlSrv_NetServiceRestart_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION(NetServiceRestart, PJOC_SRV_NET_SERVICE_RESTART)
}

PRL_ASYNC_METHOD( PrlSrv_NetServiceRestart ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_NET_SERVICE_RESTART)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_NetServiceRestart, (hServer))
}

PRL_HANDLE PrlSrv_NetServiceRestoreDefaults_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION(NetServiceRestoreDefaults, PJOC_SRV_NET_SERVICE_RESTORE_DEFAULTS)
}

PRL_ASYNC_METHOD( PrlSrv_NetServiceRestoreDefaults ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_NET_SERVICE_RESTORE_DEFAULTS)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_NetServiceRestoreDefaults, (hServer))
}

PRL_HANDLE PrlSrv_GetNetServiceStatus_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION(GetNetServiceStatus, PJOC_SRV_GET_NET_SERVICE_STATUS)
}

PRL_ASYNC_METHOD( PrlSrv_GetNetServiceStatus ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_NET_SERVICE_STATUS)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetNetServiceStatus, (hServer))
}

PRL_HANDLE PrlSrv_UpdateOfflineService_Impl(
	PRL_HANDLE hServer,
	PRL_HANDLE hOffmgmtService,
	PRL_UINT32 nFlags
	)
{

	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) ||
			PRL_WRONG_HANDLE(hOffmgmtService, PHT_OFFLINE_SERVICE))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_UPDATE_OFFLINE_SERVICE));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleOfflineServicePtr pOffmgmtService = PRL_OBJECT_BY_HANDLE<PrlHandleOfflineService>(hOffmgmtService);
	PrlHandleJobPtr pJob = pServer->UpdateOfflineService( pOffmgmtService, nFlags );
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_UpdateOfflineService ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hOffmgmtService,
	PRL_UINT32 nFlags
	)
{

	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UPDATE_OFFLINE_SERVICE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UpdateOfflineService, (hServer, hOffmgmtService, nFlags))
}

PRL_HANDLE PrlSrv_DeleteOfflineService_Impl(
	PRL_HANDLE hServer,
	PRL_HANDLE hOffmgmtService,
	PRL_UINT32 nFlags
	)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) ||
			PRL_WRONG_HANDLE(hOffmgmtService, PHT_OFFLINE_SERVICE))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_DELETE_OFFLINE_SERVICE));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleOfflineServicePtr pOffmgmtService = PRL_OBJECT_BY_HANDLE<PrlHandleOfflineService>(hOffmgmtService);
	PrlHandleJobPtr pJob = pServer->DeleteOfflineService( pOffmgmtService, nFlags );
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_DeleteOfflineService ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hOffmgmtService,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_DELETE_OFFLINE_SERVICE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_DeleteOfflineService, (hServer, hOffmgmtService, nFlags))
}

PRL_HANDLE PrlSrv_GetOfflineServicesList_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION_WITH_FLAGS(GetOfflineServicesList, nFlags, PJOC_SRV_GET_OFFLINE_SERVICES_LIST)
}

PRL_ASYNC_METHOD( PrlSrv_GetOfflineServicesList ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_OFFLINE_SERVICES_LIST)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetOfflineServicesList, (hServer, nFlags))
}

PRL_HANDLE PrlSrv_UpdateNetworkClassesList_Impl(
	PRL_HANDLE hServer,
	PRL_HANDLE hNetworkClassesList,
	PRL_UINT32 nFlags
	)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) ||
	    PRL_WRONG_HANDLE(hNetworkClassesList, PHT_HANDLES_LIST))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_UPDATE_NETWORK_CLASSES_CONFIG));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleHandlesListPtr phList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hNetworkClassesList);
	PrlHandleJobPtr pJob = pServer->UpdateNetworkClassesList( phList, nFlags );
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_UpdateNetworkClassesList) (
	PRL_HANDLE hServer,
	PRL_HANDLE hNetworkClassesList,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UPDATE_NETWORK_CLASSES_CONFIG)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UpdateNetworkClassesList, (hServer, hNetworkClassesList, nFlags))
}

PRL_HANDLE PrlSrv_GetNetworkClassesList_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION_WITH_FLAGS(GetNetworkClassesList, nFlags, PJOC_SRV_GET_NETWORK_CLASSES_LIST)
}

PRL_ASYNC_METHOD( PrlSrv_GetNetworkClassesList ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_NETWORK_CLASSES_LIST)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetNetworkClassesList, (hServer, nFlags))
}

PRL_HANDLE PrlSrv_UpdateNetworkShapingConfig_Impl(
	PRL_HANDLE hServer,
	PRL_HANDLE hNetworkShapingConfig,
	PRL_UINT32 nFlags
	)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) ||
	    PRL_WRONG_HANDLE(hNetworkShapingConfig, PHT_NETWORK_SHAPING_CONFIG))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_UPDATE_NETWORK_SHAPING_CONFIG));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleJobPtr pJob = pServer->UpdateNetworkShapingConfig(hNetworkShapingConfig, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_UpdateNetworkShapingConfig) (
	PRL_HANDLE hServer,
	PRL_HANDLE hNetworkShapingConfig,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UPDATE_NETWORK_SHAPING_CONFIG)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UpdateNetworkShapingConfig,
			(hServer, hNetworkShapingConfig, nFlags))
}

PRL_HANDLE PrlSrv_GetNetworkShapingConfig_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION_WITH_FLAGS(GetNetworkShapingConfig, nFlags, PJOC_SRV_GET_NETWORK_SHAPING_CONFIG)

}

PRL_ASYNC_METHOD( PrlSrv_GetNetworkShapingConfig ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_NETWORK_SHAPING_CONFIG)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetNetworkShapingConfig, (hServer, nFlags))
}

PRL_HANDLE PrlSrv_AddIPPrivateNetwork_Impl(PRL_HANDLE hServer, PRL_HANDLE hPrivNet, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hPrivNet, PHT_IPPRIV_NET))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_ADD_IPPRIVATE_NETWORK));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleJobPtr pJob = pServer->AddIPPrivateNetwork(hPrivNet, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_AddIPPrivateNetwork ) (
		PRL_HANDLE hServer,
		PRL_HANDLE hPrivNet,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_ADD_IPPRIVATE_NETWORK)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_AddIPPrivateNetwork, (hServer, hPrivNet, nFlags))
}

PRL_HANDLE PrlSrv_RemoveIPPrivateNetwork_Impl(PRL_HANDLE hServer, PRL_HANDLE hPrivNet, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hPrivNet, PHT_IPPRIV_NET))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_REMOVE_IPPRIVATE_NETWORK));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleJobPtr pJob = pServer->RemoveIPPrivateNetwork(hPrivNet, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_RemoveIPPrivateNetwork ) (
		PRL_HANDLE hServer,
		PRL_HANDLE hPrivNet,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_REMOVE_IPPRIVATE_NETWORK)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_RemoveIPPrivateNetwork, \
			(hServer, hPrivNet, nFlags))
}

PRL_HANDLE PrlSrv_UpdateIPPrivateNetwork_Impl(PRL_HANDLE hServer, PRL_HANDLE hPrivNet, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hPrivNet, PHT_IPPRIV_NET))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_UPDATE_IPPRIVATE_NETWORK));

	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );
	PrlHandleJobPtr pJob = pServer->UpdateIPPrivateNetwork(hPrivNet, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_UpdateIPPrivateNetwork ) (
		PRL_HANDLE hServer,
		PRL_HANDLE hPrivNet,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UPDATE_IPPRIVATE_NETWORK)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UpdateIPPrivateNetwork, \
			(hServer, hPrivNet, nFlags))
}

PRL_HANDLE PrlSrv_GetIPPrivateNetworksList_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION_WITH_FLAGS(GetIPPrivateNetworksList, nFlags,
		PJOC_SRV_GET_IPPRIVATE_NETWORKS_LIST)
}

PRL_ASYNC_METHOD( PrlSrv_GetIPPrivateNetworksList ) (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_IPPRIVATE_NETWORKS_LIST)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetIPPrivateNetworksList, (hServer, nFlags))
}

PRL_METHOD( PrlVirtNet_Create) (
		PRL_HANDLE_PTR phVirtNet
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (phVirtNet=%.8X)",
		__FUNCTION__,
		phVirtNet
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR(phVirtNet) )
		return PRL_ERR_INVALID_ARG;

	return PrlHandleVirtNet::CreateVirtualNetwork(phVirtNet);
}

PRL_METHOD( PrlVirtNet_GetNetworkId ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sNetworkId,
		PRL_UINT32_PTR pnNetworkIdBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sNetworkId=%.8X, pnNetworkIdBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sNetworkId,
		pnNetworkIdBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnNetworkIdBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetNetworkId(sNetworkId, pnNetworkIdBufLength);
}

PRL_METHOD( PrlVirtNet_SetNetworkId ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sNetworkId
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sNetworkId=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sNetworkId
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sNetworkId))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetNetworkId(sNetworkId);
}

PRL_METHOD( PrlVirtNet_GetDescription ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sDescription,
		PRL_UINT32_PTR pnDescriptionBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sDescription=%.8X, pnDescriptionBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sDescription,
		pnDescriptionBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnDescriptionBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetDescription(sDescription, pnDescriptionBufLength);
}

PRL_METHOD( PrlVirtNet_SetDescription ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sDescription
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sDescription=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sDescription
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sDescription))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetDescription(sDescription);
}

PRL_METHOD( PrlVirtNet_GetNetworkType ) (
									PRL_HANDLE hVirtNet,
									PRL_NET_VIRTUAL_NETWORK_TYPE_PTR pnNetworkType
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, pnNetworkType=%.8X)",
		__FUNCTION__,
		hVirtNet,
		pnNetworkType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) || PRL_WRONG_PTR(pnNetworkType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->GetNetworkType(pnNetworkType));
}

PRL_METHOD( PrlVirtNet_SetNetworkType ) (
									 PRL_HANDLE hVirtNet,
									 PRL_NET_VIRTUAL_NETWORK_TYPE nNetworkType
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, nNetworkType=%.8X)",
		__FUNCTION__,
		hVirtNet,
		nNetworkType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->SetNetworkType(nNetworkType));
}

PRL_METHOD( PrlVirtNet_GetBoundCardMac ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sBoundCardMac,
		PRL_UINT32_PTR pnBoundCardMacBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sBoundCardMac=%.8X, pnBoundCardMacBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sBoundCardMac,
		pnBoundCardMacBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnBoundCardMacBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetBoundCardMac(sBoundCardMac, pnBoundCardMacBufLength);
}

PRL_METHOD( PrlVirtNet_SetBoundCardMac ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sBoundCardMac
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sBoundCardMac=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sBoundCardMac
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sBoundCardMac))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetBoundCardMac(sBoundCardMac);
}

PRL_METHOD( PrlVirtNet_GetAdapterName ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sAdapterName,
		PRL_UINT32_PTR pnAdapterNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sAdapterName=%.8X, pnAdapterNameBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sAdapterName,
		pnAdapterNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnAdapterNameBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetAdapterName(sAdapterName, pnAdapterNameBufLength);
}

PRL_METHOD( PrlVirtNet_SetAdapterName ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sAdapterName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sAdapterName=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sAdapterName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sAdapterName))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetAdapterName(sAdapterName);
}

PRL_METHOD( PrlVirtNet_GetAdapterIndex ) (
									PRL_HANDLE hVirtNet,
									PRL_UINT32_PTR pnAdapterIndex
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, pnAdapterIndex=%.8X)",
		__FUNCTION__,
		hVirtNet,
		pnAdapterIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) || PRL_WRONG_PTR(pnAdapterIndex))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->GetAdapterIndex(pnAdapterIndex));
}

PRL_METHOD( PrlVirtNet_SetAdapterIndex ) (
									 PRL_HANDLE hVirtNet,
									 PRL_UINT32 nAdapterIndex
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, nAdapterIndex=%.8X)",
		__FUNCTION__,
		hVirtNet,
		nAdapterIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->SetAdapterIndex(nAdapterIndex));
}

PRL_METHOD( PrlVirtNet_GetHostIPAddress ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sHostIPAddress,
		PRL_UINT32_PTR pnHostIPAddressBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sHostIPAddress=%.8X, pnHostIPAddressBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sHostIPAddress,
		pnHostIPAddressBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnHostIPAddressBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetHostIPAddress(sHostIPAddress, pnHostIPAddressBufLength);
}

PRL_METHOD( PrlVirtNet_SetHostIPAddress ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sHostIPAddress
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sHostIPAddress=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sHostIPAddress
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sHostIPAddress))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetHostIPAddress(sHostIPAddress);
}

PRL_METHOD( PrlVirtNet_GetHostIP6Address ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sHostIPAddress,
		PRL_UINT32_PTR pnHostIPAddressBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sHostIPAddress=%.8X, pnHostIPAddressBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sHostIPAddress,
		pnHostIPAddressBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnHostIPAddressBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetHostIP6Address(sHostIPAddress, pnHostIPAddressBufLength);
}

PRL_METHOD( PrlVirtNet_SetHostIP6Address ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sHostIPAddress
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sHostIPAddress=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sHostIPAddress
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sHostIPAddress))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetHostIP6Address(sHostIPAddress);
}

PRL_METHOD( PrlVirtNet_GetDhcpIPAddress ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sDhcpIPAddress,
		PRL_UINT32_PTR pnDhcpIPAddressBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sDhcpIPAddress=%.8X, pnDhcpIPAddressBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sDhcpIPAddress,
		pnDhcpIPAddressBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnDhcpIPAddressBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetDhcpIPAddress(sDhcpIPAddress, pnDhcpIPAddressBufLength);
}

PRL_METHOD( PrlVirtNet_SetDhcpIPAddress ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sDhcpIPAddress
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sDhcpIPAddress=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sDhcpIPAddress
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sDhcpIPAddress))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetDhcpIPAddress(sDhcpIPAddress);
}

PRL_METHOD( PrlVirtNet_GetDhcpIP6Address ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sDhcpIPAddress,
		PRL_UINT32_PTR pnDhcpIPAddressBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sDhcpIPAddress=%.8X, pnDhcpIPAddressBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sDhcpIPAddress,
		pnDhcpIPAddressBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnDhcpIPAddressBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetDhcpIP6Address(sDhcpIPAddress, pnDhcpIPAddressBufLength);
}

PRL_METHOD( PrlVirtNet_SetDhcpIP6Address ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sDhcpIPAddress
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sDhcpIPAddress=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sDhcpIPAddress
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sDhcpIPAddress))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetDhcpIP6Address(sDhcpIPAddress);
}

PRL_METHOD( PrlVirtNet_GetIPNetMask ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPNetMask,
		PRL_UINT32_PTR pnIPNetMaskBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPNetMask=%.8X, pnIPNetMaskBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPNetMask,
		pnIPNetMaskBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnIPNetMaskBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetIPNetMask(sIPNetMask, pnIPNetMaskBufLength);
}

PRL_METHOD( PrlVirtNet_SetIPNetMask ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPNetMask
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPNetMask=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPNetMask
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sIPNetMask))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetIPNetMask(sIPNetMask);
}

PRL_METHOD( PrlVirtNet_GetIP6NetMask ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPNetMask,
		PRL_UINT32_PTR pnIPNetMaskBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPNetMask=%.8X, pnIPNetMaskBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPNetMask,
		pnIPNetMaskBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnIPNetMaskBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetIP6NetMask(sIPNetMask, pnIPNetMaskBufLength);
}

PRL_METHOD( PrlVirtNet_SetIP6NetMask ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPNetMask
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPNetMask=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPNetMask
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sIPNetMask))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetIP6NetMask(sIPNetMask);
}

PRL_METHOD( PrlVirtNet_GetVlanTag ) (
		PRL_HANDLE hVirtNet,
		PRL_UINT16_PTR pnVlanTag
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, pnVlanTag=%.8X)",
		__FUNCTION__,
		hVirtNet,
		pnVlanTag
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnVlanTag))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetVlanTag(pnVlanTag);
}

PRL_METHOD( PrlVirtNet_SetVlanTag ) (
		PRL_HANDLE hVirtNet,
		PRL_UINT16 nVlanTag
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, nVlanTag=%.8X)",
		__FUNCTION__,
		hVirtNet,
		nVlanTag
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetVlanTag(nVlanTag);
}

PRL_METHOD( PrlVirtNet_GetIPScopeStart ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPScopeStart,
		PRL_UINT32_PTR pnIPScopeStartBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPScopeStart=%.8X, pnIPScopeStartBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPScopeStart,
		pnIPScopeStartBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnIPScopeStartBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetIPScopeStart(sIPScopeStart, pnIPScopeStartBufLength);
}

PRL_METHOD( PrlVirtNet_SetIPScopeStart ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPScopeStart
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPScopeStart=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPScopeStart
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sIPScopeStart))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetIPScopeStart(sIPScopeStart);
}

PRL_METHOD( PrlVirtNet_GetIPScopeEnd ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPScopeEnd,
		PRL_UINT32_PTR pnIPScopeEndBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPScopeEnd=%.8X, pnIPScopeEndBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPScopeEnd,
		pnIPScopeEndBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnIPScopeEndBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetIPScopeEnd(sIPScopeEnd, pnIPScopeEndBufLength);
}

PRL_METHOD( PrlVirtNet_SetIPScopeEnd ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPScopeEnd
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPScopeEnd=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPScopeEnd
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sIPScopeEnd))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetIPScopeEnd(sIPScopeEnd);
}

PRL_METHOD( PrlVirtNet_GetIP6ScopeStart ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPScopeStart,
		PRL_UINT32_PTR pnIPScopeStartBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPScopeStart=%.8X, pnIPScopeStartBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPScopeStart,
		pnIPScopeStartBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnIPScopeStartBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetIP6ScopeStart(sIPScopeStart, pnIPScopeStartBufLength);
}

PRL_METHOD( PrlVirtNet_SetIP6ScopeStart ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPScopeStart
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPScopeStart=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPScopeStart
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sIPScopeStart))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetIP6ScopeStart(sIPScopeStart);
}

PRL_METHOD( PrlVirtNet_GetIP6ScopeEnd ) (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPScopeEnd,
		PRL_UINT32_PTR pnIPScopeEndBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPScopeEnd=%.8X, pnIPScopeEndBufLength=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPScopeEnd,
		pnIPScopeEndBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(pnIPScopeEndBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->GetIP6ScopeEnd(sIPScopeEnd, pnIPScopeEndBufLength);
}

PRL_METHOD( PrlVirtNet_SetIP6ScopeEnd ) (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPScopeEnd
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, sIPScopeEnd=%.8X)",
		__FUNCTION__,
		hVirtNet,
		sIPScopeEnd
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) ||
			PRL_WRONG_PTR(sIPScopeEnd))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>( hVirtNet );

	return pVirtNet->SetIP6ScopeEnd(sIPScopeEnd);
}

PRL_METHOD( PrlVirtNet_IsEnabled ) (
									PRL_HANDLE hVirtNet,
									PRL_BOOL_PTR pbEnabled
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) || PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->IsEnabled(pbEnabled));
}

PRL_METHOD( PrlVirtNet_SetEnabled ) (
									 PRL_HANDLE hVirtNet,
									 PRL_BOOL bEnabled
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->SetEnabled(bEnabled));
}

PRL_METHOD( PrlVirtNet_IsAdapterEnabled ) (
									PRL_HANDLE hVirtNet,
									PRL_BOOL_PTR pbEnabled
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) || PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->IsAdapterEnabled(pbEnabled));
}

PRL_METHOD( PrlVirtNet_SetAdapterEnabled ) (
									 PRL_HANDLE hVirtNet,
									 PRL_BOOL bEnabled
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->SetAdapterEnabled(bEnabled));
}

PRL_METHOD( PrlVirtNet_IsDHCPServerEnabled ) (
									PRL_HANDLE hVirtNet,
									PRL_BOOL_PTR pbEnabled
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) || PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->IsDHCPServerEnabled(pbEnabled));
}

PRL_METHOD( PrlVirtNet_SetDHCPServerEnabled ) (
									 PRL_HANDLE hVirtNet,
									 PRL_BOOL bEnabled
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->SetDHCPServerEnabled(bEnabled));
}

PRL_METHOD( PrlVirtNet_IsDHCP6ServerEnabled ) (
									PRL_HANDLE hVirtNet,
									PRL_BOOL_PTR pbEnabled
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) || PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->IsDHCP6ServerEnabled(pbEnabled));
}

PRL_METHOD( PrlVirtNet_SetDHCP6ServerEnabled ) (
									 PRL_HANDLE hVirtNet,
									 PRL_BOOL bEnabled
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->SetDHCP6ServerEnabled(bEnabled));
}

PRL_METHOD( PrlVirtNet_IsNATServerEnabled ) (
									PRL_HANDLE hVirtNet,
									PRL_BOOL_PTR pbEnabled
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK) || PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->IsNATServerEnabled(pbEnabled));
}

PRL_METHOD( PrlVirtNet_SetNATServerEnabled ) (
									 PRL_HANDLE hVirtNet,
									 PRL_BOOL bEnabled
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hVirtNet,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->SetNATServerEnabled(bEnabled));
}

#define PRL_WRONG_PORT_FWD_TYPE(type) \
	(type != PPF_TCP && type != PPF_UDP)

PRL_METHOD( PrlVirtNet_GetPortForwardList ) (
									PRL_HANDLE hVirtNet,
									PRL_PORT_FORWARDING_TYPE nPortFwdType,
									PRL_HANDLE_PTR phPortFwdList
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, nPortFwdType=%.8X, phPortFwdList=%.8X)",
		__FUNCTION__,
		hVirtNet,
		nPortFwdType,
		phPortFwdList
		);

	SYNC_CHECK_API_INITIALIZED

	if (   PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK)
		|| PRL_WRONG_PORT_FWD_TYPE(nPortFwdType)
		|| PRL_WRONG_PTR(phPortFwdList)
		)
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->GetPortForwardList(nPortFwdType, phPortFwdList));
}

PRL_METHOD( PrlVirtNet_SetPortForwardList ) (
									PRL_HANDLE hVirtNet,
									PRL_PORT_FORWARDING_TYPE nPortFwdType,
									PRL_HANDLE hPortFwdList
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, nPortFwdType=%.8X, hPortFwdList=%.8X)",
		__FUNCTION__,
		hVirtNet,
		nPortFwdType,
		hPortFwdList
		);

	SYNC_CHECK_API_INITIALIZED

	if (   PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK)
		|| PRL_WRONG_PORT_FWD_TYPE(nPortFwdType)
		|| ( hPortFwdList != PRL_INVALID_HANDLE && PRL_WRONG_HANDLE(hPortFwdList, PHT_HANDLES_LIST) )
		|| PRL_WRONG_HANDLES_LIST(hPortFwdList, PHT_PORT_FORWARDING)
		)
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	return (pVirtNet->SetPortForwardList(nPortFwdType, hPortFwdList));
}

PRL_METHOD( PrlVirtNet_GetBoundAdapterInfo ) (
		PRL_HANDLE hVirtNet,
		PRL_HANDLE hSrvConfig,
		PRL_HANDLE_PTR phNetAdapter
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVirtNet=%.8X, hSrvConfig=%.8X, phNetAdapter=%.8X)",
		__FUNCTION__,
		hVirtNet,
		hSrvConfig,
		phNetAdapter
		);

	SYNC_CHECK_API_INITIALIZED

	if (   PRL_WRONG_HANDLE(hVirtNet, PHT_VIRTUAL_NETWORK)
		|| PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG)
		|| PRL_WRONG_PTR(phNetAdapter)
		)
		return PRL_ERR_INVALID_ARG;

	PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(hVirtNet);
	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>(hSrvConfig);
	return (pVirtNet->GetBoundAdapterInfo(pSrvConfig, phNetAdapter));
}

PRL_METHOD( PrlPortFwd_Create) (
		PRL_HANDLE_PTR phPortFwd
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (phPortFwd=%.8X)",
		__FUNCTION__,
		phPortFwd
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR(phPortFwd) )
		return PRL_ERR_INVALID_ARG;

	return PrlHandlePortFwd::Create(phPortFwd);
}

PRL_METHOD( PrlPortFwd_GetRuleName ) (
		PRL_HANDLE hPortFwd,
		PRL_STR sRuleName,
		PRL_UINT32_PTR pnRuleNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, sRuleName=%.8X, pnRuleNameBufLength=%.8X)",
		__FUNCTION__,
		hPortFwd,
		sRuleName,
		pnRuleNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING) ||
			PRL_WRONG_PTR(pnRuleNameBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>( hPortFwd );

	return pPortFwd->GetRuleName(sRuleName, pnRuleNameBufLength);
}

PRL_METHOD( PrlPortFwd_SetRuleName ) (
		PRL_HANDLE hPortFwd,
		PRL_CONST_STR sRuleName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, sRuleName=%.8X)",
		__FUNCTION__,
		hPortFwd,
		sRuleName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING) ||
			PRL_WRONG_PTR(sRuleName))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>( hPortFwd );

	return pPortFwd->SetRuleName(sRuleName);
}

PRL_METHOD( PrlPortFwd_GetIncomingPort ) (
									PRL_HANDLE hPortFwd,
									PRL_UINT16_PTR pnIncomingPort
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, pnIncomingPort=%.8X)",
		__FUNCTION__,
		hPortFwd,
		pnIncomingPort
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING) || PRL_WRONG_PTR(pnIncomingPort))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>(hPortFwd);
	return (pPortFwd->GetIncomingPort(pnIncomingPort));
}

PRL_METHOD( PrlPortFwd_SetIncomingPort ) (
									 PRL_HANDLE hPortFwd,
									 PRL_UINT16 nIncomingPort
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, nIncomingPort=%.8X)",
		__FUNCTION__,
		hPortFwd,
		nIncomingPort
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>(hPortFwd);
	return (pPortFwd->SetIncomingPort(nIncomingPort));
}

PRL_METHOD( PrlPortFwd_GetRedirectIPAddress ) (
		PRL_HANDLE hPortFwd,
		PRL_STR sRedirectIPAddress,
		PRL_UINT32_PTR pnRedirectIPAddressBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, sRedirectIPAddress=%.8X, pnRedirectIPAddressBufLength=%.8X)",
		__FUNCTION__,
		hPortFwd,
		sRedirectIPAddress,
		pnRedirectIPAddressBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING) ||
			PRL_WRONG_PTR(pnRedirectIPAddressBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>( hPortFwd );

	return pPortFwd->GetRedirectIPAddress(sRedirectIPAddress, pnRedirectIPAddressBufLength);
}

PRL_METHOD( PrlPortFwd_SetRedirectIPAddress ) (
		PRL_HANDLE hPortFwd,
		PRL_CONST_STR sRedirectIPAddress
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, sRedirectIPAddress=%.8X)",
		__FUNCTION__,
		hPortFwd,
		sRedirectIPAddress
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING) ||
			PRL_WRONG_PTR(sRedirectIPAddress))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>( hPortFwd );

	return pPortFwd->SetRedirectIPAddress(sRedirectIPAddress);
}

PRL_METHOD( PrlPortFwd_GetRedirectPort ) (
									PRL_HANDLE hPortFwd,
									PRL_UINT16_PTR pnRedirectPort
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, pnRedirectPort=%.8X)",
		__FUNCTION__,
		hPortFwd,
		pnRedirectPort
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING) || PRL_WRONG_PTR(pnRedirectPort))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>(hPortFwd);
	return (pPortFwd->GetRedirectPort(pnRedirectPort));
}

PRL_METHOD( PrlPortFwd_SetRedirectPort ) (
									 PRL_HANDLE hPortFwd,
									 PRL_UINT16 nRedirectPort
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, nRedirectPort=%.8X)",
		__FUNCTION__,
		hPortFwd,
		nRedirectPort
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>(hPortFwd);
	return (pPortFwd->SetRedirectPort(nRedirectPort));
}

PRL_METHOD( PrlPortFwd_GetRedirectVm ) (
		PRL_HANDLE hPortFwd,
		PRL_STR sRedirectVm,
		PRL_UINT32_PTR pnRedirectVmBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, sRedirectVm=%.8X, pnRedirectVmBufLength=%.8X)",
		__FUNCTION__,
		hPortFwd,
		sRedirectVm,
		pnRedirectVmBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING) ||
			PRL_WRONG_PTR(pnRedirectVmBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>( hPortFwd );

	return pPortFwd->GetRedirectVm(sRedirectVm, pnRedirectVmBufLength);
}

PRL_METHOD( PrlPortFwd_SetRedirectVm ) (
		PRL_HANDLE hPortFwd,
		PRL_CONST_STR sRedirectVm
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPortFwd=%.8X, sRedirectVm=%.8X)",
		__FUNCTION__,
		hPortFwd,
		sRedirectVm
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPortFwd, PHT_PORT_FORWARDING) ||
			PRL_WRONG_PTR(sRedirectVm))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePortFwdPtr pPortFwd = PRL_OBJECT_BY_HANDLE<PrlHandlePortFwd>( hPortFwd );

	return pPortFwd->SetRedirectVm(sRedirectVm);
}

PRL_METHOD( PrlNetSvc_GetStatus) (
		PRL_HANDLE handle,
		PRL_SERVICE_STATUS_ENUM_PTR pnNetStatus
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%.8X, pNetStatus=%.8X)",
		__FUNCTION__,
		handle,
		pnNetStatus
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_NET_SERVICE_STATUS) || PRL_WRONG_PTR(pnNetStatus) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleNetServiceStatusPtr pNetStatus = PRL_OBJECT_BY_HANDLE<PrlHandleNetServiceStatus>( handle );
	return (pNetStatus->GetStatus(pnNetStatus));
}

PRL_METHOD( PrlOffmgmtService_Create ) (
		PRL_HANDLE_PTR phOffmgmtService
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR(phOffmgmtService) )
		return PRL_ERR_INVALID_ARG;

	return PrlHandleOfflineService::CreateOffmgmtService(phOffmgmtService);
}

PRL_METHOD( PrlOffmgmtService_GetName ) (
		PRL_HANDLE hOffmgmtService,
		PRL_STR sName,
		PRL_UINT32_PTR pnNameBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOffmgmtService, PHT_OFFLINE_SERVICE ) ||
			PRL_WRONG_PTR(pnNameBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleOfflineServicePtr pOffmgmtService = PRL_OBJECT_BY_HANDLE<PrlHandleOfflineService>( hOffmgmtService );

	return pOffmgmtService->GetName(sName, pnNameBufLength);
}

PRL_METHOD( PrlOffmgmtService_SetName) (
		PRL_HANDLE hOffmgmtService,
		PRL_CONST_STR sName
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOffmgmtService, PHT_OFFLINE_SERVICE) ||
			PRL_WRONG_PTR(sName))
		return PRL_ERR_INVALID_ARG;

	PrlHandleOfflineServicePtr pOffmgmtService = PRL_OBJECT_BY_HANDLE<PrlHandleOfflineService>( hOffmgmtService );

	return pOffmgmtService->SetName(sName);
}

PRL_METHOD( PrlOffmgmtService_GetPort ) (
		PRL_HANDLE hOffmgmtService,
		PRL_UINT32_PTR pnPort
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOffmgmtService, PHT_OFFLINE_SERVICE ) ||
			PRL_WRONG_PTR(pnPort))
		return PRL_ERR_INVALID_ARG;

	PrlHandleOfflineServicePtr pOffmgmtService = PRL_OBJECT_BY_HANDLE<PrlHandleOfflineService>( hOffmgmtService );

	return pOffmgmtService->GetOfflineServicePort(pnPort);
}

PRL_METHOD( PrlOffmgmtService_SetPort) (
		PRL_HANDLE hOffmgmtService,
		PRL_UINT32 nPort
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOffmgmtService, PHT_OFFLINE_SERVICE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleOfflineServicePtr pOffmgmtService = PRL_OBJECT_BY_HANDLE<PrlHandleOfflineService>( hOffmgmtService );

	return pOffmgmtService->SetOfflineServicePort(nPort);
}

PRL_METHOD( PrlOffmgmtService_IsUsedByDefault ) (
		PRL_HANDLE hOffmgmtService,
		PRL_BOOL_PTR pbDefault
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOffmgmtService, PHT_OFFLINE_SERVICE ) ||
			PRL_WRONG_PTR(pbDefault))
		return PRL_ERR_INVALID_ARG;

	PrlHandleOfflineServicePtr pOffmgmtService = PRL_OBJECT_BY_HANDLE<PrlHandleOfflineService>( hOffmgmtService );

	return pOffmgmtService->IsUsedByDefault(pbDefault);
}

PRL_METHOD( PrlOffmgmtService_SetUsedByDefault) (
		PRL_HANDLE hOffmgmtService,
		PRL_UINT32 bDefault
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOffmgmtService, PHT_OFFLINE_SERVICE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleOfflineServicePtr pOffmgmtService = PRL_OBJECT_BY_HANDLE<PrlHandleOfflineService>( hOffmgmtService );

	return pOffmgmtService->SetUsedByDefault(bDefault);
}

PRL_METHOD( PrlNetworkClass_Create ) (
			PRL_UINT32 nClassId,
                        PRL_HANDLE_PTR phNetworkClass
                        )

{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR(phNetworkClass))
		return PRL_ERR_INVALID_ARG;

	return PrlHandleNetworkClass::CreateNetworkClass(nClassId, phNetworkClass);
}

PRL_METHOD( PrlNetworkClass_GetClassId ) (
			PRL_HANDLE hNetworkClass,
			PRL_UINT32_PTR pnClassId
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkClass, PHT_NETWORK_CLASS))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkClassPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkClass>(hNetworkClass);

	return pEntry->GetClassId(pnClassId);
}

PRL_METHOD( PrlNetworkClass_SetClassId ) (
			PRL_HANDLE hNetworkClass,
			PRL_UINT32 nClassId
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkClass, PHT_NETWORK_CLASS))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkClassPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkClass>(hNetworkClass);

	return pEntry->SetClassId(nClassId);
}

PRL_METHOD( PrlNetworkClass_GetNetworkList ) (
			PRL_HANDLE hNetworkClass,
			PRL_HANDLE_PTR phNetworkList
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkClass, PHT_NETWORK_CLASS))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkClassPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkClass>(hNetworkClass);

	return pEntry->GetNetworkList(phNetworkList);
}

PRL_METHOD( PrlNetworkClass_SetNetworkList ) (
			PRL_HANDLE hNetworkClass,
			PRL_HANDLE hNetworkList
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkClass, PHT_NETWORK_CLASS) ||
	     PRL_WRONG_HANDLE(hNetworkList, PHT_STRINGS_LIST))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkClassPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkClass>(hNetworkClass);

	return pEntry->SetNetworkList(hNetworkList);
}

PRL_METHOD( PrlNetworkShapingEntry_Create ) (
                        PRL_UINT32 nClass,
                        PRL_UINT32 nTotalRate,
                        PRL_HANDLE_PTR phNetworkShapingEntry
                        )

{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR(phNetworkShapingEntry))
		return PRL_ERR_INVALID_ARG;

	return PrlHandleNetworkShaping::CreateNetworkShapingEntry(nClass, nTotalRate, phNetworkShapingEntry);
}

PRL_METHOD( PrlNetworkShapingEntry_GetClassId ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_UINT32_PTR pnClassId
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	return pEntry->GetClassId(pnClassId);
}

PRL_METHOD( PrlNetworkShapingEntry_SetClassId ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_UINT32 nClassId
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	return pEntry->SetClassId(nClassId);
}

PRL_METHOD( PrlNetworkShapingEntry_GetTotalRate ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_UINT32_PTR pnTotalRate
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	return pEntry->GetTotalRate(pnTotalRate);
}

PRL_METHOD( PrlNetworkShapingEntry_SetTotalRate ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_UINT32 nTotalRate
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	return pEntry->SetTotalRate(nTotalRate);
}

PRL_METHOD( PrlNetworkShapingEntry_SetDevice ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_CONST_STR sDev
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING) ||
		PRL_WRONG_PTR(sDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	return pEntry->SetDevice(sDev);
}

PRL_METHOD( PrlNetworkShapingEntry_GetDevice ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_STR sDev,
			PRL_UINT32_PTR pnBufLength
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	return pEntry->GetDevice(sDev, pnBufLength);
}

PRL_METHOD( PrlNetworkShapingEntry_SetRate ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_UINT32 nRate
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	return pEntry->SetRate(nRate);
}

PRL_METHOD( PrlNetworkShapingEntry_GetRate ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_UINT32_PTR pnRate
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	return pEntry->GetRate(pnRate);
}

PRL_METHOD( PrlNetworkShapingEntry_SetRateMPU ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_INT32 nRateMPU
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	pEntry->SetRateMPU(nRateMPU);
	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlNetworkShapingEntry_GetRateMPU ) (
			PRL_HANDLE hNetworkShapingEntry,
			PRL_INT32_PTR pnRateMPU
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hNetworkShapingEntry, PHT_NETWORK_SHAPING) ||
		PRL_WRONG_PTR(pnRateMPU) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShaping>(hNetworkShapingEntry);

	*pnRateMPU = pEntry->GetRateMPU();
	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlNetworkShapingConfig_IsEnabled ) (
		PRL_HANDLE hConfig,
		PRL_BOOL_PTR pbEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hConfig=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hConfig,
		pbEnabled
	);

	if (PRL_WRONG_HANDLE(hConfig, PHT_NETWORK_SHAPING_CONFIG))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingConfigPtr pNetworkShapingConfig =
		PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingConfig>( hConfig );

	return pNetworkShapingConfig->IsEnabled(pbEnabled);
}

PRL_METHOD( PrlNetworkShapingConfig_SetEnabled ) (
		PRL_HANDLE hConfig,
		PRL_BOOL bEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hConfig=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hConfig,
		bEnabled
	);

	if (PRL_WRONG_HANDLE(hConfig, PHT_NETWORK_SHAPING_CONFIG))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingConfigPtr pNetworkShapingConfig =
		PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingConfig>( hConfig );

	return pNetworkShapingConfig->SetEnabled(bEnabled);
}

PRL_METHOD( PrlNetworkShapingConfig_GetNetworkShapingList ) (
		PRL_HANDLE hConfig,
		PRL_HANDLE_PTR phList
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hConfig=%.8X, phList=%.8X)",
		__FUNCTION__,
		hConfig,
		phList
	);

	if (PRL_WRONG_HANDLE(hConfig, PHT_NETWORK_SHAPING_CONFIG))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingConfigPtr pNetworkShapingConfig =
		PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingConfig>( hConfig );

	return pNetworkShapingConfig->GetNetworkShapingList(phList);
}

PRL_METHOD( PrlNetworkShapingConfig_SetNetworkShapingList ) (
		PRL_HANDLE hConfig,
		PRL_HANDLE hList
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hConfig=%.8X, hList=%.8X)",
		__FUNCTION__,
		hConfig,
		hList
	);

	if (PRL_WRONG_HANDLE(hConfig, PHT_NETWORK_SHAPING_CONFIG))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingConfigPtr pNetworkShapingConfig =
		PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingConfig>( hConfig );

	return pNetworkShapingConfig->SetNetworkShapingList(hList);
}

PRL_METHOD( PrlNetworkShapingConfig_GetNetworkDeviceBandwidthList ) (
		PRL_HANDLE hConfig,
		PRL_HANDLE_PTR phList
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hConfig=%.8X, phList=%.8X)",
		__FUNCTION__,
		hConfig,
		phList
	);

	if (PRL_WRONG_HANDLE(hConfig, PHT_NETWORK_SHAPING_CONFIG))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingConfigPtr pNetworkShapingConfig =
		PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingConfig>( hConfig );

	return pNetworkShapingConfig->GetNetworkDeviceBandwidthList(phList);
}

PRL_METHOD( PrlNetworkShapingConfig_SetNetworkDeviceBandwidthList ) (
		PRL_HANDLE hConfig,
		PRL_HANDLE hList
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hConfig=%.8X, hList=%.8X)",
		__FUNCTION__,
		hConfig,
		hList
	);

	if (PRL_WRONG_HANDLE(hConfig, PHT_NETWORK_SHAPING_CONFIG))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingConfigPtr pNetworkShapingConfig =
		PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingConfig>( hConfig );

	return pNetworkShapingConfig->SetNetworkDeviceBandwidthList(hList);
}

PRL_METHOD( PrlNetworkShapingBandwidthEntry_Create ) (
                        PRL_CONST_STR sDev,
                        PRL_UINT32 nBandwidth,
                        PRL_HANDLE_PTR phDeviceEntry
                        )

{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR(phDeviceEntry))
		return PRL_ERR_INVALID_ARG;

	return PrlHandleNetworkShapingBandwidth::CreateDeviceBandwidthEntry(sDev, nBandwidth, phDeviceEntry);
}

PRL_METHOD( PrlNetworkShapingBandwidthEntry_SetDevice ) (
			PRL_HANDLE hDeviceEntry,
			PRL_CONST_STR sDev
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDeviceEntry, PHT_NETWORK_SHAPING_BANDWIDTH) ||
		PRL_WRONG_PTR(sDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingBandwidthPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingBandwidth>(hDeviceEntry);

	return pEntry->SetDevice(sDev);
}

PRL_METHOD( PrlNetworkShapingBandwidthEntry_GetDevice ) (
			PRL_HANDLE hDeviceEntry,
			PRL_STR sDev,
			PRL_UINT32_PTR pnBufLength
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDeviceEntry, PHT_NETWORK_SHAPING_BANDWIDTH))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingBandwidthPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingBandwidth>(hDeviceEntry);

	return pEntry->GetDevice(sDev, pnBufLength);
}

PRL_METHOD( PrlNetworkShapingBandwidthEntry_SetBandwidth ) (
			PRL_HANDLE hDeviceEntry,
			PRL_UINT32 nBandwidth
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDeviceEntry, PHT_NETWORK_SHAPING_BANDWIDTH))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingBandwidthPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingBandwidth>(hDeviceEntry);

	return pEntry->SetBandwidth(nBandwidth);
}

PRL_METHOD( PrlNetworkShapingBandwidthEntry_GetBandwidth ) (
			PRL_HANDLE hDeviceEntry,
			PRL_UINT32_PTR pnBandwidth
			)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDeviceEntry, PHT_NETWORK_SHAPING_BANDWIDTH))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkShapingBandwidthPtr pEntry = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkShapingBandwidth>(hDeviceEntry);

	return pEntry->GetBandwidth(pnBandwidth);
}

PRL_METHOD( PrlNetworkRate_Create ) (
                        PRL_UINT32 nClass,
			PRL_UINT32 nRate,
                        PRL_HANDLE_PTR phNetworkRate
                        )

{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR(phNetworkRate) ||
	     nClass == 0 || nRate == 0)
		return PRL_ERR_INVALID_ARG;

	return PrlHandleNetworkRate::CreateNetworkRate(nClass, nRate, phNetworkRate);
}

PRL_METHOD( PrlNetworkRate_GetClassId ) (
                        PRL_HANDLE hNetworkRate,
                        PRL_UINT32_PTR pnClass
                        )
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetworkRate, PHT_NETWORK_RATE) ||
	    PRL_WRONG_PTR(pnClass))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkRatePtr pRate = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkRate>(hNetworkRate);

	return pRate->GetClass(pnClass);
}

PRL_METHOD( PrlNetworkRate_GetRate ) (
                        PRL_HANDLE hNetworkRate,
                        PRL_UINT32_PTR pnRate
                        )
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetworkRate, PHT_NETWORK_RATE) ||
	    PRL_WRONG_PTR(pnRate))
		return PRL_ERR_INVALID_ARG;

	PrlHandleNetworkRatePtr pRate = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkRate>(hNetworkRate);

	return pRate->GetRate(pnRate);
}

PRL_METHOD( PrlIPPrivNet_Create ) (
		PRL_HANDLE_PTR phPrivNet
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (phPrivNet=%.8X)",
		__FUNCTION__,
		phPrivNet
	);

	if (PRL_WRONG_PTR(phPrivNet))
		return PRL_ERR_INVALID_ARG;

	return PrlHandleIPPrivNet::Create(phPrivNet);
}

PRL_METHOD( PrlIPPrivNet_GetName ) (
		PRL_HANDLE hPrivNet,
		PRL_STR sName,
		PRL_UINT32_PTR pnNameBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPrivNet=%.8X, sName=%.8X, pnNameBufLength=%.8X)",
		__FUNCTION__,
		hPrivNet,
		sName,
		pnNameBufLength
	);

	if (PRL_WRONG_HANDLE(hPrivNet, PHT_IPPRIV_NET) ||
		PRL_WRONG_PTR(pnNameBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleIPPrivNetPtr pIPPrivNet = PRL_OBJECT_BY_HANDLE<PrlHandleIPPrivNet>( hPrivNet );

	return pIPPrivNet->GetName(sName, pnNameBufLength);
}

PRL_METHOD( PrlIPPrivNet_SetName ) (
		PRL_HANDLE hPrivNet,
		PRL_CONST_STR sName
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPrivNet=%.8X, sName=%.8X)",
		__FUNCTION__,
		hPrivNet,
		sName
	);

	if (PRL_WRONG_HANDLE(hPrivNet, PHT_IPPRIV_NET) ||
		PRL_WRONG_PTR(sName))
		return PRL_ERR_INVALID_ARG;

	PrlHandleIPPrivNetPtr pIPPrivNet = PRL_OBJECT_BY_HANDLE<PrlHandleIPPrivNet>( hPrivNet );

	return pIPPrivNet->SetName(sName);
}

PRL_METHOD( PrlIPPrivNet_SetNetAddresses ) (
		PRL_HANDLE hPrivNet,
		PRL_HANDLE hNetAddresses
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPrivNet=%.8X, hNetAddresses=%.8X)",
		__FUNCTION__,
		hPrivNet,
		hNetAddresses
	);

	if (PRL_WRONG_HANDLE(hPrivNet, PHT_IPPRIV_NET) ||
		(PRL_INVALID_HANDLE != hNetAddresses && PRL_WRONG_HANDLE(hNetAddresses, PHT_STRINGS_LIST)))
		return PRL_ERR_INVALID_ARG;

	PrlHandleIPPrivNetPtr pIPPrivNet = PRL_OBJECT_BY_HANDLE<PrlHandleIPPrivNet>( hPrivNet );
	PrlHandleStringsListPtr pNetAddressesList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hNetAddresses);
	return (pIPPrivNet->SetNetAddresses(pNetAddressesList.getHandle() ?
				pNetAddressesList->GetStringsList() : QStringList()));
}

PRL_METHOD( PrlIPPrivNet_GetNetAddresses ) (
		PRL_HANDLE hPrivNet,
		PRL_HANDLE_PTR phNetAddresses
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPrivNet=%.8X, phNetAddresses=%.8X)",
		__FUNCTION__,
		hPrivNet,
		phNetAddresses
	);

	if (PRL_WRONG_HANDLE(hPrivNet, PHT_IPPRIV_NET) ||
		PRL_WRONG_PTR(phNetAddresses))
		return PRL_ERR_INVALID_ARG;

	PrlHandleIPPrivNetPtr pIPPrivNet = PRL_OBJECT_BY_HANDLE<PrlHandleIPPrivNet>( hPrivNet );

	return pIPPrivNet->GetNetAddresses(phNetAddresses);
}

PRL_METHOD( PrlIPPrivNet_IsGlobal ) (
		PRL_HANDLE hPrivNet,
		PRL_BOOL_PTR pbGlobal
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPrivNet=%.8X, pbGlobal=%.8X)",
		__FUNCTION__,
		hPrivNet,
		pbGlobal
	);

	if (PRL_WRONG_HANDLE(hPrivNet, PHT_IPPRIV_NET) ||
		PRL_WRONG_PTR(pbGlobal))
		return PRL_ERR_INVALID_ARG;

	PrlHandleIPPrivNetPtr pIPPrivNet = PRL_OBJECT_BY_HANDLE<PrlHandleIPPrivNet>( hPrivNet );

	return pIPPrivNet->IsGlobal(pbGlobal);
}

PRL_METHOD( PrlIPPrivNet_SetGlobal ) (
		PRL_HANDLE hPrivNet,
		PRL_BOOL bGlobal
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPrivNet=%.8X, bGlobal=%.8X)",
		__FUNCTION__,
		hPrivNet,
		bGlobal
	);

	if (PRL_WRONG_HANDLE(hPrivNet, PHT_IPPRIV_NET))
		return PRL_ERR_INVALID_ARG;

	PrlHandleIPPrivNetPtr pIPPrivNet = PRL_OBJECT_BY_HANDLE<PrlHandleIPPrivNet>( hPrivNet );

	return pIPPrivNet->SetGlobal(bGlobal);
}
