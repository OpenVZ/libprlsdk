/*
 * PrlApiDeprecated.cpp
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


//#define FORCE_LOGGING_ON
//#define FORCE_LOGGING_LEVEL	DBG_DEBUG

#include "SDK/Include/Parallels.h"

#include "Libraries/PrlCommonUtilsBase/PrlStringifyConsts.h"
#include "Libraries/Std/PrlAssert.h"
#include "Libraries/Logging/Logging.h"

#include "PrlCommon.h"
#include "PrlHandleResult.h"
#include "PrlHandleLoginResponse.h"
#include "PrlHandleServerDeprecated.h"
#include "PrlHandleVmSrv.h"
#include "PrlHandleVmCfg.h"
#include "PrlHandleVmDevice.h"
#include "PrlHandleVmDeviceHardDrive.h"
#include "PrlHandleDispNet.h"
#include "PrlHandleLicense.h"
#include "PrlHandleUserProfile.h"
#include "HandleInterface/HandleAdapter.h"
// breakpad includes
#include "parallels_breakpad.h"

#include "Build/Current.ver"

#include "PrlContextSwitcher.h"
#include "PrlNew.h"

////////////////////////////////////////////////////////////////////////////////////////////////


PRL_METHOD (PrlResult_GetParamToken ) (
									   PRL_HANDLE hResult,
									   PRL_UINT32 index,
									   PRL_VOID_PTR_PTR data_ptr
									   )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hResult=%.8X index=%.8d data_ptr=%.8X)",
		__FUNCTION__,
		hResult,
		index,
		data_ptr
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hResult, PHT_RESULT) || PRL_WRONG_PTR(data_ptr))
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this job was registered one day
	PrlHandleResultPtr pResult = PRL_OBJECT_BY_HANDLE<PrlHandleResult>( hResult );

	return pResult->GetParamToken( index, data_ptr );
}

PRL_METHOD (PrlResult_GetError ) (
								  PRL_HANDLE hResult,
								  PRL_VOID_PTR_PTR data_ptr
								  )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hResult=%.8X data_ptr=%.8X)",
		__FUNCTION__,
		hResult,
		data_ptr
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hResult, PHT_RESULT) || PRL_WRONG_PTR(data_ptr))
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this job was registered one day
	PrlHandleResultPtr pResult = PRL_OBJECT_BY_HANDLE<PrlHandleResult>( hResult );

	return pResult->GetError( data_ptr );
}

PRL_METHOD ( PrlLoginResponse_ToString ) (
	PRL_HANDLE hLoginResp,
	PRL_VOID_PTR_PTR pLoginRespStr
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLoginResp=%.8X pLoginRespStr=%.8X)",
		__FUNCTION__,
		hLoginResp,
		pLoginRespStr
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLoginResp, PHT_LOGIN_RESPONSE) || PRL_WRONG_PTR(pLoginRespStr) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleLoginResponsePtr pLoginResp = PRL_OBJECT_BY_HANDLE<PrlHandleLoginResponse>( hLoginResp );
	*pLoginRespStr = strdup(pLoginResp->toString().toUtf8().data());
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlResult_FromString ) (
									PRL_HANDLE hResult,
									PRL_CONST_STR sResult
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hResult=%.8X sResult=%.8X)",
		__FUNCTION__,
		hResult,
		sResult
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hResult, PHT_RESULT) || PRL_WRONG_PTR(sResult) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleResultPtr pResult = PRL_OBJECT_BY_HANDLE<PrlHandleResult>( hResult );

	return pResult->FromString( sResult );
}

PRL_METHOD ( PrlUsrCfg_FromString ) (
									 PRL_HANDLE hUsrCfg,
									 PRL_CONST_STR sUsrCfg
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUsrCfg=%.8X, sUsrCfg=%.8x)",
		__FUNCTION__,
		hUsrCfg,
		sUsrCfg
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hUsrCfg, PHT_USER_PROFILE) || PRL_WRONG_PTR(sUsrCfg) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleUserProfilePtr pUsrCfg = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUsrCfg );

	return pUsrCfg->fromString( sUsrCfg );
}

PRL_METHOD ( PrlUsrCfg_ToString ) (
								   PRL_HANDLE hUsrCfg,
								   PRL_VOID_PTR_PTR pUsrCfg
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUsrCfg=%.8X pUsrCfg=%.8X)",
		__FUNCTION__,
		hUsrCfg,
		pUsrCfg
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hUsrCfg, PHT_USER_PROFILE) || PRL_WRONG_PTR(pUsrCfg) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUsrCfg );
	*pUsrCfg = strdup(pUserProfile->toString().toUtf8().data());
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlUsrCfg_AddProxy ) (
	PRL_HANDLE hUserProfile,
	PRL_CONST_STR sProxyHost,
	PRL_UINT32 nProxyPort,
	PRL_CONST_STR sProxyUserLogin,
	PRL_CONST_STR sProxyUserPasswd,
	PRL_INT32 nReserved
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserProfile=%.8X, )",
		__FUNCTION__,
		hUserProfile
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserProfile, PHT_USER_PROFILE))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUserProfile );

	return (pUserProfile->AddProxy(sProxyHost, nProxyPort, sProxyUserLogin, sProxyUserPasswd, nReserved));
}

PRL_METHOD ( PrlDispCfg_FromString ) (
									  PRL_HANDLE hDispCfg,
									  PRL_CONST_STR sDispCfg
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispCfg=%.8X, sDispCfg=%.8x)",
		__FUNCTION__,
		hDispCfg,
		sDispCfg
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDispCfg, PHT_DISP_CONFIG) || PRL_WRONG_PTR(sDispCfg) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispCfg = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispCfg );

	return pDispCfg->fromString( sDispCfg );
}

PRL_METHOD ( PrlDispCfg_ToString ) (
									PRL_HANDLE hDispCfg,
									PRL_VOID_PTR_PTR pDispCfg
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispCfg=%.8X pDispCfg=%.8X)",
		__FUNCTION__,
		hDispCfg,
		pDispCfg
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDispCfg, PHT_DISP_CONFIG) || PRL_WRONG_PTR(pDispCfg) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispCfg );
	*pDispCfg = strdup(pDispConfig->toString().toUtf8().data());
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD ( PrlVm_FromString ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR vm_config
		)
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%.8X)",
		__FUNCTION__,
		hVm
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(vm_config) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->fromString( vm_config );
}

PRL_METHOD ( PrlVm_ToString ) (
		PRL_HANDLE hVm,
		PRL_VOID_PTR_PTR vm_config
		)
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%.8X vm_config=%.8X)",
		__FUNCTION__,
		hVm,
		vm_config
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(vm_config) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );
	*vm_config = strdup(pVm->toString().toUtf8().data());
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlVmDev_FromString ) (
		PRL_HANDLE hVmDevice,
		PRL_CONST_STR sXml
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDevice=%.8X, sXml=%.8X)",
		__FUNCTION__,
		hVmDevice,
		sXml
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if (
			PRL_WRONG_VM_DEVICE_HANDLE(hVmDevice) ||
			PRL_WRONG_PTR( sXml )
	)
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDevice);
	return pDevice->fromStringCommon( sXml );
}

PRL_METHOD( PrlVmDev_ToString ) (
		PRL_HANDLE hVmDevice,
		PRL_VOID_PTR_PTR sXml
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDevice=%.8X, sXml=%.8X)",
		__FUNCTION__,
		hVmDevice,
		sXml
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if (
			PRL_WRONG_VM_DEVICE_HANDLE(hVmDevice) ||
			PRL_WRONG_PTR( sXml )
	)
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDevice);
	return pDevice->toStringCommon( sXml );
}

PRL_HANDLE PrlVmDev_UpdateInfo_Impl(PRL_HANDLE hServer,PRL_HANDLE hVmInputDev)
{
	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmInputDev))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_UNKNOWN))

		PrlHandleVmDevicePtr pVmDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmInputDev);
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_UNKNOWN))
		PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );
	PrlHandleJobPtr pJob = pServer->DspCmdUpdateDeviceInfo( pVmDevice->toString().toUtf8().data() );

	if (!pJob)
		RETURN_RES((PRL_INVALID_HANDLE))
		RETURN_RES((pJob->GetHandle()))
}

PRL_ASYNC_METHOD( PrlVmDev_UpdateInfo )(
					  PRL_HANDLE hServer,
					  PRL_HANDLE hVmInputDev
					  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmInputDev=%.8X)",
		__FUNCTION__,
		hVmInputDev
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_UNKNOWN)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmDev_UpdateInfo, (hServer,hVmInputDev) )
}

PRL_METHOD ( PrlLic_FromString ) (
								  PRL_HANDLE hLic,
								  PRL_CONST_STR sLic
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLic=%.8X, sLic=%.8x)",
		__FUNCTION__,
		hLic,
		sLic
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLic, PHT_LICENSE) || PRL_WRONG_PTR(sLic) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleLicensePtr pLic = PRL_OBJECT_BY_HANDLE<PrlHandleLicense>( hLic );

	return pLic->fromString( sLic );
}

PRL_METHOD ( PrlLic_ToString ) (
								PRL_HANDLE hLic,
								PRL_VOID_PTR_PTR pLicStr
								)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLic=%.8X pLicStr=%.8X)",
		__FUNCTION__,
		hLic,
		pLicStr
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLic, PHT_LICENSE) || PRL_WRONG_PTR(pLicStr) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleLicensePtr pLicense = PRL_OBJECT_BY_HANDLE<PrlHandleLicense>( hLic );
	*pLicStr = strdup(pLicense->toString().toUtf8().data());
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlLic_Create ) (
		PRL_HANDLE_PTR phLic
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (phLic=%.8X)",
		__FUNCTION__,
		phLic
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR(phLic) )
		return PRL_ERR_INVALID_ARG;

	return PrlHandleLicense::CreateLicense(phLic);
}

PRL_ASYNC_METHOD( PrlVm_GetVirtDevInfo ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nFlags
		)
{
	PRL_UNUSED_PARAM(hVm);
	PRL_UNUSED_PARAM(nFlags);
	return PRL_INVALID_HANDLE;
}

PRL_METHOD( PrlVmVirtDevInfo_GetUsbMouseState) (
		PRL_HANDLE handle,
		PRL_VOID_PTR pVmUsbMouseState
		)
{
	PRL_UNUSED_PARAM(handle);
	PRL_UNUSED_PARAM(pVmUsbMouseState);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_HANDLE PrlVm_UpdateToolsSection_Impl(PRL_HANDLE hVm, PRL_CONST_STR sToolsSection)
{
	ONE_HANDLE_AND_ONE_PTR_PARAM_VM_METH_IMPLEMENTATION( \
		UpdateToolsSection, sToolsSection, PJOC_VM_UPDATE_TOOLS_SECTION)
}

PRL_ASYNC_METHOD( PrlVm_UpdateToolsSection ) (
						PRL_HANDLE hVm,
						PRL_CONST_STR sToolsSection
							   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_UPDATE_TOOLS_SECTION);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_UpdateToolsSection, (hVm, sToolsSection));
}

PRL_METHOD( PrlVm_SetCpuVtxEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bVmCpuVtxEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%.8X, bVmCpuVtxEnabled=%.8X)",
		__FUNCTION__,
		hVm,
		bVmCpuVtxEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return (pVm->SetCpuVtxEnabled(bVmCpuVtxEnabled));
}

PRL_HANDLE PrlSrv_StartConvertHdd_Impl(
					  PRL_HANDLE hServer,
					  PRL_HANDLE hStringsList
					   )
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER)
		|| (hStringsList != PRL_INVALID_HANDLE && PRL_WRONG_HANDLE(hStringsList, PHT_STRINGS_LIST)) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_UNKNOWN))
		PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );
	PrlHandleJobPtr pJob = pServer->StartConvertHdd(hStringsList);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_StartConvertHdd) (
	PRL_HANDLE hServer,
	PRL_HANDLE hStringsList
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_UNKNOWN)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_StartConvertHdd, (hServer, hStringsList))
}

PRL_HANDLE PrlSrv_SmcGetRuntimeInfo_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DEPRECATED_METH_IMPLEMENTATION(SmcGetRuntimeInfo, PJOC_UNKNOWN)
}

PRL_ASYNC_METHOD( PrlSrv_SmcGetRuntimeInfo ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_UNKNOWN)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_SmcGetRuntimeInfo, (hServer))
}

PRL_METHOD( PrlVm_CreateAnswerEvent ) (
		PRL_HANDLE hVm,
		PRL_HANDLE_PTR phEvent,
		PRL_RESULT nAnswer
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%.8X, phEvent=%.8X, nAnswer=%.8X)",
		__FUNCTION__,
		hVm,
		phEvent,
		nAnswer
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(phEvent))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return (pVm->CreateAnswerEvent(phEvent, nAnswer));
}

PRL_METHOD ( PrlDispNet_PortForwardingFromString ) (
	PRL_HANDLE hDispNet,
	PRL_CONST_STR sPortForwarding
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sPortForwarding=%.8x)",
		__FUNCTION__,
		hDispNet,
		sPortForwarding
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) || PRL_WRONG_PTR(sPortForwarding) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pNetCfg = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return pNetCfg->SetPortForwardingFromString( sPortForwarding );
}

PRL_METHOD ( PrlDispNet_PortForwardingToString ) (
	PRL_HANDLE hDispNet,
	PRL_VOID_PTR_PTR pPortForwarding
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X pPortForwarding=%.8X)",
		__FUNCTION__,
		hDispNet,
		pPortForwarding
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) || PRL_WRONG_PTR(pPortForwarding) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pNetCfg = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return pNetCfg->GetPortForwardingToString( pPortForwarding );
}

PRL_METHOD ( PrlUsr_GetClientConfig ) (
									   PRL_HANDLE_PTR phConfig
									   )
{
	SYNC_CHECK_API_INITIALIZED
	UNUSED_PARAM(phConfig);
	LOG_MESSAGE( DBG_DEBUG, "%s (phConfig=%.8X)",
		__FUNCTION__,
		phConfig
	);

	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_HANDLE PrlSrv_PrepareForHibernate_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_DEPRECATED_METH_IMPLEMENTATION_WITH_FLAGS(PrepareForHibernate, nFlags, PJOC_SRV_PREPARE_FOR_HIBERNATE)
}

PRL_ASYNC_METHOD( PrlSrv_PrepareForHibernate ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_PREPARE_FOR_HIBERNATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_PrepareForHibernate, (hServer, nFlags))
}

PRL_HANDLE PrlSrv_AfterHostResume_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_DEPRECATED_METH_IMPLEMENTATION_WITH_FLAGS(AfterHostResume, nFlags, PJOC_SRV_AFTER_HOST_RESUME)
}

PRL_ASYNC_METHOD( PrlSrv_AfterHostResume ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_AFTER_HOST_RESUME)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_AfterHostResume, (hServer, nFlags))
}

PRL_METHOD( PrlDispNet_Remove ) (
								 PRL_HANDLE hDispNet
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X)",
		__FUNCTION__,
		hDispNet
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->Remove());
}

PRL_METHOD( PrlDispNet_IsEnabled ) (
									PRL_HANDLE hDispNet,
									PRL_BOOL_PTR pbEnabled
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hDispNet,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) || PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->IsEnabled(pbEnabled));
}

PRL_METHOD( PrlDispNet_SetEnabled ) (
									 PRL_HANDLE hDispNet,
									 PRL_BOOL bEnabled
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hDispNet,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->SetEnabled(bEnabled));
}

PRL_METHOD( PrlDispNet_IsHidden ) (
									PRL_HANDLE hDispNet,
									PRL_BOOL_PTR pbHidden
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, pbHidden=%.8X)",
		__FUNCTION__,
		hDispNet,
		pbHidden
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) || PRL_WRONG_PTR(pbHidden))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->IsHidden(pbHidden));
}

PRL_METHOD( PrlDispNet_SetHidden ) (
									 PRL_HANDLE hDispNet,
									 PRL_BOOL bHidden
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, bHidden=%.8X)",
		__FUNCTION__,
		hDispNet,
		bHidden
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->SetHidden(bHidden));
}

PRL_METHOD( PrlDispNet_IsDhcpEnabled ) (
										PRL_HANDLE hDispNet,
										PRL_BOOL_PTR pbDhcpEnabled
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, pbDhcpEnabled=%.8X)",
		__FUNCTION__,
		hDispNet,
		pbDhcpEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) || PRL_WRONG_PTR(pbDhcpEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->IsDhcpEnabled(pbDhcpEnabled));
}

PRL_METHOD( PrlDispNet_SetDhcpEnabled ) (
	PRL_HANDLE hDispNet,
	PRL_BOOL bDhcpEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, bDhcpEnabled=%.8X)",
		__FUNCTION__,
		hDispNet,
		bDhcpEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->SetDhcpEnabled(bDhcpEnabled));
}

PRL_METHOD( PrlDispNet_GetNetworkType ) (
	PRL_HANDLE hDispNet,
	PRL_NET_ADAPTER_EMULATED_TYPE_PTR pnNetworkType
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, pnNetworkType=%.8X)",
		__FUNCTION__,
		hDispNet,
		pnNetworkType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) || PRL_WRONG_PTR(pnNetworkType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->GetNetworkType(pnNetworkType));
}

PRL_METHOD( PrlDispNet_SetNetworkType ) (
	PRL_HANDLE hDispNet,
	PRL_NET_ADAPTER_EMULATED_TYPE nNetworkType
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, nNetworkType=%.8X)",
		__FUNCTION__,
		hDispNet,
		nNetworkType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->SetNetworkType(nNetworkType));
}

PRL_METHOD( PrlDispNet_GetName) (
								 PRL_HANDLE hDispNet,
								 PRL_STR sDispNetName,
								 PRL_UINT32_PTR pnDispNetNameBufLength
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sDispNetName=%.8X, pnDispNetNameBufLength=%.8X)",
		__FUNCTION__,
		hDispNet,
		sDispNetName,
		pnDispNetNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnDispNetNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->GetName(sDispNetName, pnDispNetNameBufLength));
}

PRL_METHOD( PrlDispNet_SetName) (
								 PRL_HANDLE hDispNet,
								 PRL_CONST_STR sNewDispNetName
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sNewDispNetName=%.8X)",
		__FUNCTION__,
		hDispNet,
		sNewDispNetName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(sNewDispNetName))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->SetName(sNewDispNetName));
}

PRL_METHOD( PrlDispNet_GetUuid) (
								 PRL_HANDLE hDispNet,
								 PRL_STR sDispNetUuid,
								 PRL_UINT32_PTR pnDispNetUuidBufLength
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sDispNetUuid=%.8X, pnDispNetUuidBufLength=%.8X)",
		__FUNCTION__,
		hDispNet,
		sDispNetUuid,
		pnDispNetUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnDispNetUuidBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->GetUuid(sDispNetUuid, pnDispNetUuidBufLength));
}

PRL_METHOD( PrlDispNet_GetSysName) (
									PRL_HANDLE hDispNet,
									PRL_STR sDispNetSysName,
									PRL_UINT32_PTR pnDispNetSysNameBufLength
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sDispNetSysName=%.8X, pnDispNetSysNameBufLength=%.8X)",
		__FUNCTION__,
		hDispNet,
		sDispNetSysName,
		pnDispNetSysNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnDispNetSysNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->GetSysName(sDispNetSysName, pnDispNetSysNameBufLength));
}

PRL_METHOD( PrlDispNet_SetSysName) (
									PRL_HANDLE hDispNet,
									PRL_CONST_STR sNewDispNetSysName
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sNewDispNetSysName=%.8X)",
		__FUNCTION__,
		hDispNet,
		sNewDispNetSysName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(sNewDispNetSysName))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->SetSysName(sNewDispNetSysName));
}

PRL_METHOD( PrlDispNet_GetIndex ) (
								   PRL_HANDLE hDispNet,
								   PRL_UINT32_PTR pnIndex
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, pnIndex=%.8X)",
		__FUNCTION__,
		hDispNet,
		pnIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) || PRL_WRONG_PTR( pnIndex ))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->GetIndex(pnIndex));
}

PRL_METHOD( PrlDispNet_SetIndex ) (
								   PRL_HANDLE hDispNet,
								   PRL_UINT32 nIndex
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, nIndex=%.8X)",
		__FUNCTION__,
		hDispNet,
		nIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->SetIndex(nIndex));
}

PRL_METHOD( PrlDispNet_GetDhcpScopeStartIp) (
	PRL_HANDLE hDispNet,
	PRL_STR sDispNetDhcpScopeStartIp,
	PRL_UINT32_PTR pnDispNetDhcpScopeStartIpBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sDispNetDhcpScopeStartIp=%.8X, pnDispNetDhcpScopeStartIpBufLength=%.8X)",
		__FUNCTION__,
		hDispNet,
		sDispNetDhcpScopeStartIp,
		pnDispNetDhcpScopeStartIpBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnDispNetDhcpScopeStartIpBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->GetDhcpScopeStartIp(sDispNetDhcpScopeStartIp, pnDispNetDhcpScopeStartIpBufLength));
}

PRL_METHOD( PrlDispNet_SetDhcpScopeStartIp) (
	PRL_HANDLE hDispNet,
	PRL_CONST_STR sNewDispNetDhcpScopeStartIp
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sNewDispNetDhcpScopeStartIp=%.8X)",
		__FUNCTION__,
		hDispNet,
		sNewDispNetDhcpScopeStartIp
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(sNewDispNetDhcpScopeStartIp))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->SetDhcpScopeStartIp(sNewDispNetDhcpScopeStartIp));
}

PRL_METHOD( PrlDispNet_GetDhcpScopeEndIp) (
	PRL_HANDLE hDispNet,
	PRL_STR sDispNetDhcpScopeEndIp,
	PRL_UINT32_PTR pnDispNetDhcpScopeEndIpBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sDispNetDhcpScopeEndIp=%.8X, pnDispNetDhcpScopeEndIpBufLength=%.8X)",
		__FUNCTION__,
		hDispNet,
		sDispNetDhcpScopeEndIp,
		pnDispNetDhcpScopeEndIpBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnDispNetDhcpScopeEndIpBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->GetDhcpScopeEndIp(sDispNetDhcpScopeEndIp, pnDispNetDhcpScopeEndIpBufLength));
}

PRL_METHOD( PrlDispNet_SetDhcpScopeEndIp) (
	PRL_HANDLE hDispNet,
	PRL_CONST_STR sNewDispNetDhcpScopeEndIp
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sNewDispNetDhcpScopeEndIp=%.8X)",
		__FUNCTION__,
		hDispNet,
		sNewDispNetDhcpScopeEndIp
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(sNewDispNetDhcpScopeEndIp))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->SetDhcpScopeEndIp(sNewDispNetDhcpScopeEndIp));
}

PRL_METHOD( PrlDispNet_GetDhcpScopeMask) (
	PRL_HANDLE hDispNet,
	PRL_STR sDispNetDhcpScopeMask,
	PRL_UINT32_PTR pnDispNetDhcpScopeMaskBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sDispNetDhcpScopeMask=%.8X, pnDispNetDhcpScopeMaskBufLength=%.8X)",
		__FUNCTION__,
		hDispNet,
		sDispNetDhcpScopeMask,
		pnDispNetDhcpScopeMaskBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnDispNetDhcpScopeMaskBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->GetDhcpScopeMask(sDispNetDhcpScopeMask, pnDispNetDhcpScopeMaskBufLength));
}

PRL_METHOD( PrlDispNet_SetDhcpScopeMask) (
	PRL_HANDLE hDispNet,
	PRL_CONST_STR sNewDispNetDhcpScopeMask
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sNewDispNetDhcpScopeMask=%.8X)",
		__FUNCTION__,
		hDispNet,
		sNewDispNetDhcpScopeMask
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
		PRL_WRONG_PTR(sNewDispNetDhcpScopeMask))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->SetDhcpScopeMask(sNewDispNetDhcpScopeMask));
}

/* ======= IPv6 ========= */

PRL_METHOD( PrlDispNet_IsDhcp6Enabled ) (
										PRL_HANDLE hDispNet,
										PRL_BOOL_PTR pbDhcp6Enabled
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, pbDhcp6Enabled=%.8X)",
		__FUNCTION__,
		hDispNet,
		pbDhcp6Enabled
		);

	SYNC_CHECK_API_INITIALIZED

		if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) || PRL_WRONG_PTR(pbDhcp6Enabled))
			return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->IsDhcp6Enabled(pbDhcp6Enabled));
}

PRL_METHOD( PrlDispNet_SetDhcp6Enabled ) (
	PRL_HANDLE hDispNet,
	PRL_BOOL bDhcp6Enabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, bDhcp6Enabled=%.8X)",
		__FUNCTION__,
		hDispNet,
		bDhcp6Enabled
		);

	SYNC_CHECK_API_INITIALIZED

		if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER))
			return PRL_ERR_INVALID_ARG;

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>(hDispNet);
	return (pDispNet->SetDhcp6Enabled(bDhcp6Enabled));
}

PRL_METHOD( PrlDispNet_GetDhcp6ScopeStartIp) (
	PRL_HANDLE hDispNet,
	PRL_STR sDispNetDhcp6ScopeStartIp,
	PRL_UINT32_PTR pnDispNetDhcp6ScopeStartIpBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sDispNetDhcp6ScopeStartIp=%.8X, pnDispNetDhcp6ScopeStartIpBufLength=%.8X)",
		__FUNCTION__,
		hDispNet,
		sDispNetDhcp6ScopeStartIp,
		pnDispNetDhcp6ScopeStartIpBufLength
		);

	SYNC_CHECK_API_INITIALIZED

		if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnDispNetDhcp6ScopeStartIpBufLength))
			return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->GetDhcp6ScopeStartIp(sDispNetDhcp6ScopeStartIp, pnDispNetDhcp6ScopeStartIpBufLength));
}

PRL_METHOD( PrlDispNet_SetDhcp6ScopeStartIp) (
	PRL_HANDLE hDispNet,
	PRL_CONST_STR sNewDispNetDhcp6ScopeStartIp
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sNewDispNetDhcp6ScopeStartIp=%.8X)",
		__FUNCTION__,
		hDispNet,
		sNewDispNetDhcp6ScopeStartIp
		);

	SYNC_CHECK_API_INITIALIZED

		if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
			PRL_WRONG_PTR(sNewDispNetDhcp6ScopeStartIp))
			return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->SetDhcp6ScopeStartIp(sNewDispNetDhcp6ScopeStartIp));
}

PRL_METHOD( PrlDispNet_GetDhcp6ScopeEndIp) (
	PRL_HANDLE hDispNet,
	PRL_STR sDispNetDhcp6ScopeEndIp,
	PRL_UINT32_PTR pnDispNetDhcp6ScopeEndIpBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sDispNetDhcp6ScopeEndIp=%.8X, pnDispNetDhcp6ScopeEndIpBufLength=%.8X)",
		__FUNCTION__,
		hDispNet,
		sDispNetDhcp6ScopeEndIp,
		pnDispNetDhcp6ScopeEndIpBufLength
		);

	SYNC_CHECK_API_INITIALIZED

		if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnDispNetDhcp6ScopeEndIpBufLength))
			return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->GetDhcp6ScopeEndIp(sDispNetDhcp6ScopeEndIp, pnDispNetDhcp6ScopeEndIpBufLength));
}

PRL_METHOD( PrlDispNet_SetDhcp6ScopeEndIp) (
	PRL_HANDLE hDispNet,
	PRL_CONST_STR sNewDispNetDhcp6ScopeEndIp
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sNewDispNetDhcp6ScopeEndIp=%.8X)",
		__FUNCTION__,
		hDispNet,
		sNewDispNetDhcp6ScopeEndIp
		);

	SYNC_CHECK_API_INITIALIZED

		if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
			PRL_WRONG_PTR(sNewDispNetDhcp6ScopeEndIp))
			return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->SetDhcp6ScopeEndIp(sNewDispNetDhcp6ScopeEndIp));
}

PRL_METHOD( PrlDispNet_GetDhcp6ScopeMask) (
	PRL_HANDLE hDispNet,
	PRL_STR sDispNetDhcp6ScopeMask,
	PRL_UINT32_PTR pnDispNetDhcp6ScopeMaskBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sDispNetDhcp6ScopeMask=%.8X, pnDispNetDhcp6ScopeMaskBufLength=%.8X)",
		__FUNCTION__,
		hDispNet,
		sDispNetDhcp6ScopeMask,
		pnDispNetDhcp6ScopeMaskBufLength
		);

	SYNC_CHECK_API_INITIALIZED

		if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnDispNetDhcp6ScopeMaskBufLength))
			return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->GetDhcp6ScopeMask(sDispNetDhcp6ScopeMask, pnDispNetDhcp6ScopeMaskBufLength));
}

PRL_METHOD( PrlDispNet_SetDhcp6ScopeMask) (
	PRL_HANDLE hDispNet,
	PRL_CONST_STR sNewDispNetDhcp6ScopeMask
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispNet=%.8X, sNewDispNetDhcp6ScopeMask=%.8X)",
		__FUNCTION__,
		hDispNet,
		sNewDispNetDhcp6ScopeMask
		);

	SYNC_CHECK_API_INITIALIZED

		if (PRL_WRONG_HANDLE(hDispNet, PHT_DISP_NET_ADAPTER) ||
			PRL_WRONG_PTR(sNewDispNetDhcp6ScopeMask))
			return (PRL_ERR_INVALID_ARG);

	PrlHandleDispNetPtr pDispNet = PRL_OBJECT_BY_HANDLE<PrlHandleDispNet>( hDispNet );

	return (pDispNet->SetDhcp6ScopeMask(sNewDispNetDhcp6ScopeMask));
}

PRL_HANDLE PrlSrv_AddNetAdapter_Impl(PRL_HANDLE hServer,	PRL_HANDLE hNetAdapter)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hNetAdapter, PHT_DISP_NET_ADAPTER) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_ADD_NET_ADAPTER))
		PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );
	PrlHandleJobPtr pJob = pServer->AddNetAdapter(hNetAdapter);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_AddNetAdapter ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hNetAdapter
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_ADD_NET_ADAPTER)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_AddNetAdapter, (hServer, hNetAdapter))
}

PRL_HANDLE PrlSrv_DeleteNetAdapter_Impl(PRL_HANDLE hServer,	PRL_UINT32 nIndex)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_DELETE_NET_ADAPTER))
		PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );
	PrlHandleJobPtr pJob = pServer->DeleteNetAdapter(nIndex);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_DeleteNetAdapter ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nIndex
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_DELETE_NET_ADAPTER)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_DeleteNetAdapter, (hServer, nIndex))
}

PRL_HANDLE PrlSrv_UpdateNetAdapter_Impl(PRL_HANDLE hServer,	PRL_HANDLE hNetAdapter)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hNetAdapter, PHT_DISP_NET_ADAPTER) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_UPDATE_NET_ADAPTER))
		PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );
	PrlHandleJobPtr pJob = pServer->UpdateNetAdapter(hNetAdapter);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_UpdateNetAdapter ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hNetAdapter
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UPDATE_NET_ADAPTER)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UpdateNetAdapter, (hServer, hNetAdapter))
}

PRL_HANDLE PrlSrv_StoreValueByKey_Impl(
	PRL_HANDLE hServer,
	PRL_CONST_STR sKey,
	PRL_CONST_STR sValue,
	PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER)
		|| PRL_WRONG_PTR(sKey)
		|| PRL_WRONG_PTR(sValue)
		)
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_STORE_VALUE_BY_KEY));

	PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );
	PrlHandleJobPtr pJob = pServer->DspCmdStorageSetValue(sKey, sValue, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_StoreValueByKey ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR sKey,
	PRL_CONST_STR sValue,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_STORE_VALUE_BY_KEY)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_StoreValueByKey, (hServer, sKey, sValue, nFlags))
}

PRL_HANDLE PrlVm_StoreValueByKey_Impl(
	PRL_HANDLE hVm,
	PRL_CONST_STR sKey,
	PRL_CONST_STR sValue,
	PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE)
		|| PRL_WRONG_PTR(sKey)
		|| PRL_WRONG_PTR(sValue)
		)
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_STORE_VALUE_BY_KEY));

	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>( hVm );
	PrlHandleJobPtr pJob = pVm->StoreValueByKey(sKey, sValue, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_StoreValueByKey ) (
	PRL_HANDLE hVm,
	PRL_CONST_STR sKey,
	PRL_CONST_STR sValue,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_STORE_VALUE_BY_KEY)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_StoreValueByKey, (hVm, sKey, sValue, nFlags))
}

PRL_METHOD( PrlDispCfg_CreateDispNet) (
									   PRL_HANDLE hDispCfg,
									   PRL_HANDLE_PTR phDispNet
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispCfg=%.8X, phDispNet=%.8X)",
		__FUNCTION__,
		hDispCfg,
		phDispNet
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDispCfg, PHT_DISP_CONFIG) || PRL_WRONG_PTR(phDispNet) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispCfg = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>(hDispCfg);
	return (pDispCfg->CreateDispNet(phDispNet));
}

PRL_METHOD( PrlDispCfg_GetDispNetCount) (
	PRL_HANDLE hDispCfg,
	PRL_UINT32_PTR pnDispNetCount
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispCfg=%.8X, pnDispNetCount=%.8X)",
		__FUNCTION__,
		hDispCfg,
		pnDispNetCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDispCfg, PHT_DISP_CONFIG) || PRL_WRONG_PTR(pnDispNetCount) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispCfg = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>(hDispCfg);
	return (pDispCfg->GetDispNetCount(pnDispNetCount));
}

PRL_METHOD( PrlDispCfg_GetDispNet) (
									PRL_HANDLE hDispCfg,
									PRL_UINT32 nDispNetIndex,
									PRL_HANDLE_PTR phDispNet
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispCfg=%.8X, nDispNetIndex=%.8X, phDispNet=%.8X)",
		__FUNCTION__,
		hDispCfg,
		nDispNetIndex,
		phDispNet
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDispCfg, PHT_DISP_CONFIG) || PRL_WRONG_PTR(phDispNet) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispCfg = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>(hDispCfg);
	return (pDispCfg->GetDispNet(nDispNetIndex, phDispNet));
}

PRL_METHOD( PrlApi_SetLogPath ) ( PRL_CONST_STR sLogPath )
{
	if ( sLogPath && sLogPath[0] )
	{
		ParallelsDirs::setLogPath(sLogPath);
		return PRL_ERR_SUCCESS;
	}

	PRL_APPLICATION_MODE nAppMode = ParallelsDirs::getAppExecuteMode();
	if (nAppMode != PAM_UNKNOWN)
	{
		ParallelsDirs::setClientLogPathByAppMode(nAppMode);
		return PRL_ERR_SUCCESS;
	}

	return PRL_ERR_UNINITIALIZED;
}

PRL_METHOD( PrlVmCfg_GetDevsList ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR vmdev_handles,
		PRL_UINT32_PTR vmdev_handles_count
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%.8X, vmdev_handles=%.8X, vmdev_handles_count=%.8X)",
		__FUNCTION__,
		hVmCfg,
		vmdev_handles,
		vmdev_handles_count
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(vmdev_handles) || PRL_WRONG_PTR(vmdev_handles_count) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);

	return (pVm->GetDevicesList(vmdev_handles, vmdev_handles_count));
}

PRL_HANDLE PrlSrv_SendClientStatistics_Impl(PRL_HANDLE hServer, PRL_CONST_STR sStatistics ,PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_PTR(sStatistics) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_SEND_CLIENT_STATISTICS));

	PRL_CntxSw_CHECK_HANDLE_ASYNC(hServer, PHT_SERVER, PJOC_SRV_SEND_CLIENT_STATISTICS) ;

	PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );
	PrlHandleJobPtr pJob = pServer->DspCmdSendClientStatistics( sStatistics, nFlags );

	if (!pJob)
		RETURN_RES((PRL_INVALID_HANDLE))
		RETURN_RES((pJob->GetHandle()))
}

PRL_ASYNC_METHOD( PrlSrv_SendClientStatistics ) (PRL_HANDLE hServer, PRL_CONST_STR sStatistics, PRL_UINT32 nFlags)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_SEND_CLIENT_STATISTICS) ;
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_SendClientStatistics, (hServer, sStatistics, nFlags)) ;
}

PRL_HANDLE PrlSrv_UpdateUsbDevicesAssociationsList_Impl(PRL_HANDLE hServer,
												PRL_HANDLE hStringsList,
												PRL_UINT32 nListVersion,
												PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER)
		|| (hStringsList != PRL_INVALID_HANDLE && PRL_WRONG_HANDLE(hStringsList, PHT_STRINGS_LIST)) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_UPDATE_USB_ASSOC_LIST))
		PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );
	PrlHandleJobPtr pJob = pServer->DspCmdUpdateUsbAssocList(hStringsList,nListVersion,nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_UpdateUsbDevicesAssociationsList ) (
				 PRL_HANDLE hServer,
				 PRL_HANDLE hStringsList,
				 PRL_UINT32 nListVersion,
				 PRL_UINT32 nFlags
				)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UPDATE_USB_ASSOC_LIST)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UpdateUsbDevicesAssociationsList, (hServer, hStringsList,nListVersion,nFlags))
}

PRL_HANDLE PrlVm_RunCompressor_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(RunCompressor, PJOC_VM_RUN_COMPRESSOR)
}

PRL_ASYNC_METHOD( PrlVm_RunCompressor ) (
					   PRL_HANDLE hVm
							   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_RUN_COMPRESSOR)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_RunCompressor, (hVm))
}

PRL_HANDLE PrlVm_CancelCompressor_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(CancelCompressor, PJOC_VM_CANCEL_COMPRESSOR)
}

PRL_ASYNC_METHOD( PrlVm_CancelCompressor ) (
					   PRL_HANDLE hVm
							   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_CANCEL_COMPRESSOR)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_CancelCompressor, (hVm))
}

PRL_HANDLE PrlVm_InternalCommand_Impl(PRL_HANDLE hVm, PRL_CONST_STR sCmdName, PRL_HANDLE hArgsList)
{
	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sCmdName) ||
		(PRL_INVALID_HANDLE != hArgsList && PRL_WRONG_HANDLE(hArgsList, PHT_STRINGS_LIST)))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_CMD_INTERNAL))

	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->InternalCmd(sCmdName, hArgsList);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_InternalCommand ) (PRL_HANDLE hVm, PRL_CONST_STR sCmd, PRL_HANDLE hArgsList)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_CMD_INTERNAL)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_InternalCommand, (hVm, sCmd, hArgsList))
}

PRL_HANDLE PrlSrv_RestartNetworkShaping_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_DEPRECATED_METH_IMPLEMENTATION_WITH_FLAGS(RestartNetworkShaping, nFlags, PJOC_SRV_RESTART_NETWORK_SHAPING)
}

PRL_ASYNC_METHOD( PrlSrv_RestartNetworkShaping) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_RESTART_NETWORK_SHAPING)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_RestartNetworkShaping, (hServer, nFlags))
}

PRL_ASYNC_METHOD( PrlSrv_UpdateNetworkShapingList) (
	PRL_HANDLE hServer,
	PRL_HANDLE hNetworkShapingList,
	PRL_UINT32 nFlags
	)
{
	PRL_UNUSED_PARAM(hServer);
	PRL_UNUSED_PARAM(hNetworkShapingList);
	PRL_UNUSED_PARAM(nFlags);
	return PRL_INVALID_HANDLE;
}

PRL_ASYNC_METHOD( PrlSrv_GetNetworkShapingList ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	PRL_UNUSED_PARAM(hServer);
	PRL_UNUSED_PARAM(nFlags);
	return PRL_INVALID_HANDLE;
}

PRL_METHOD( PrlVmCfg_SetDefaultGateway) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmDefaultGateway
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%.8X, sNewVmDefaultGateway=%.8X)",
		__FUNCTION__,
		hVmCfg,
		sNewVmDefaultGateway
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmDefaultGateway))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetDefaultGateway(sNewVmDefaultGateway));
}

PRL_METHOD( PrlVmCfg_SetDefaultGatewayIPv6) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmDefaultGateway
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%.8X, sNewVmDefaultGateway=%.8X)",
		__FUNCTION__,
		hVmCfg,
		sNewVmDefaultGateway
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmDefaultGateway))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetDefaultGatewayIPv6(sNewVmDefaultGateway));
}

PRL_METHOD( PrlVmCfg_GetDefaultGateway) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmDefaultGateway,
		PRL_UINT32_PTR pnVmDefaultGatewayBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%.8X, sVmDefaultGateway=%.8X, pnVmDefaultGatewayBufLength=%.8X)",
		__FUNCTION__,
		hVmCfg,
		sVmDefaultGateway,
		pnVmDefaultGatewayBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmDefaultGatewayBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetDefaultGateway(sVmDefaultGateway, pnVmDefaultGatewayBufLength));
}

PRL_METHOD( PrlVmCfg_GetDefaultGatewayIPv6) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmDefaultGateway,
		PRL_UINT32_PTR pnVmDefaultGatewayBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%.8X, sVmDefaultGateway=%.8X, pnVmDefaultGatewayBufLength=%.8X)",
		__FUNCTION__,
		hVmCfg,
		sVmDefaultGateway,
		pnVmDefaultGatewayBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmDefaultGatewayBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetDefaultGatewayIPv6(sVmDefaultGateway, pnVmDefaultGatewayBufLength));
}

PRL_METHOD( PrlVmDevHd_GetBlockSize ) (
		PRL_HANDLE hVmDev,
		PRL_UINT32_PTR pnBytes
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pnBytes))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->GetBlockSize(pnBytes));
}

PRL_METHOD( PrlVmDevHd_SetBlockSize ) (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nBytes
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->SetBlockSize(nBytes));
}

PRL_METHOD( PrlVmCfg_Is3DAccelerationEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVm3DAccelerationEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%.8X, pbVm3DAccelerationEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		pbVm3DAccelerationEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVm3DAccelerationEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->Is3DAccelerationEnabled(pbVm3DAccelerationEnabled));
}

PRL_METHOD( PrlVmCfg_Set3DAccelerationEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVm3DAccelerationEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%.8X, bVm3DAccelerationEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVm3DAccelerationEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->Set3DAccelerationEnabled(bVm3DAccelerationEnabled));
}

PRL_HANDLE PrlSrv_StartClusterService_Impl(PRL_HANDLE hServer, PRL_CONST_STR sServiceName, PRL_UINT32 nFlags)
{
	Q_UNUSED(hServer);
	Q_UNUSED(sServiceName);
	Q_UNUSED(nFlags);
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_UNIMPLEMENTED, PJOC_SRV_START_CLUSTER_SERVICE))
}

PRL_ASYNC_METHOD( PrlSrv_StartClusterService ) (PRL_HANDLE hServer, PRL_CONST_STR sServiceName, PRL_UINT32 nFlags)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_START_CLUSTER_SERVICE) ;
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_StartClusterService, (hServer, sServiceName, nFlags)) ;
}

PRL_HANDLE PrlSrv_StopClusterService_Impl(PRL_HANDLE hServer, PRL_CONST_STR sServiceName, PRL_UINT32 nFlags)
{
	Q_UNUSED(hServer);
	Q_UNUSED(sServiceName);
	Q_UNUSED(nFlags);

	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_UNIMPLEMENTED, PJOC_SRV_STOP_CLUSTER_SERVICE))
}

PRL_ASYNC_METHOD( PrlSrv_StopClusterService ) (PRL_HANDLE hServer, PRL_CONST_STR sServiceName, PRL_UINT32 nFlags)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_STOP_CLUSTER_SERVICE) ;
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_StopClusterService, (hServer, sServiceName, nFlags)) ;
}

PRL_METHOD( PrlDispCfg_SetClusterMode ) (
		PRL_HANDLE hDispConfig,
		PRL_BOOL bClusterMode
		)
{
	Q_UNUSED(hDispConfig);
	Q_UNUSED(bClusterMode);
	SYNC_CHECK_API_INITIALIZED

	return PRL_ERR_UNIMPLEMENTED;
}

PRL_METHOD( PrlDispCfg_IsClusterMode ) (
		PRL_HANDLE hDispConfig,
		PRL_BOOL_PTR pbClusterMode
		)
{
	Q_UNUSED(hDispConfig);
	Q_UNUSED(pbClusterMode);
	SYNC_CHECK_API_INITIALIZED
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_METHOD( PrlDispCfg_GetHostId ) (
		PRL_HANDLE hDispConfig,
		PRL_STR sHostId,
		PRL_UINT32_PTR pnHostIdBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sHostId=%.8X, pnHostIdBufLength=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sHostId,
		pnHostIdBufLength
	);

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG)
		|| PRL_WRONG_PTR(pnHostIdBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return pDispConfig->GetHostId(sHostId, pnHostIdBufLength);
}

PRL_METHOD( PrlDispCfg_SetHostId ) (
		PRL_HANDLE hDispConfig,
		PRL_CONST_STR sHostId
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sHostId=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sHostId
	);

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(sHostId))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return pDispConfig->SetHostId(sHostId);
}

PRL_HANDLE PrlVm_SendProblemReport_Impl(
    PRL_HANDLE hVm,
    PRL_HANDLE hProblemReport,
    PRL_UINT32 nFlags
)
{
    if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
        RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_SEND_PROBLEM_REPORT))

    PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>( hVm );
    PrlHandleJobPtr pJob = pVm->SendProblemReport( hProblemReport, nFlags );
    if ( !pJob )
        RETURN_RES(PRL_INVALID_HANDLE);
    RETURN_RES(pJob->GetHandle());
}

PRL_ASYNC_METHOD( PrlVm_SendProblemReport ) (
        PRL_HANDLE hVm,
        PRL_HANDLE hProblemReport,
        PRL_UINT32 nFlags
        )
{
    ASYNC_CHECK_API_INITIALIZED(PJOC_VM_SEND_PROBLEM_REPORT)
    CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_SendProblemReport, (hVm, hProblemReport, nFlags))
}

