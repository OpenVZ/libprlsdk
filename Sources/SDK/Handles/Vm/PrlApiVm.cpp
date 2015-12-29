/*
 * PrlApiVm.cpp
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

#include <prlcommon/PrlCommonUtilsBase/PrlStringifyConsts.h>
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/Logging/Logging.h>

#include "PrlCommon.h"
#include "PrlHandleServer.h"
#include "PrlHandleVm.h"
#include "PrlHandleStringsList.h"
#include "HandleInterface/GetHandleInterface.h"

#include <prlcommon/PrlCommonUtilsBase/OsInfo.h>
#include "PrlHandleOpTypeList.h"
#include "PrlHandleServerVm.h"
#include "PrlHandleVmSrv.h"
#include "PrlHandleVmCfg.h"
#include "PrlHandleFoundVmInfo.h"
#include "PrlHandleVmToolsInfo.h"
#include "PrlHandleVmDevice.h"
#include "PrlHandleVmDeviceHardDrive.h"
#include "PrlHandleVmDeviceHdPart.h"
#include "PrlHandleVmDeviceOpticalDisk.h"
#include "PrlHandleVmDeviceNetAdapter.h"
#include "PrlHandleVmDeviceUsbPort.h"
#include "PrlHandleVmDeviceSound.h"
#include "PrlHandleVmDeviceSerialPort.h"
#include "PrlHandleVmShare.h"
#include "PrlHandleVmBootDev.h"
#include "PrlHandleSystemStatistics.h"
#include "PrlHandleSystemStatisticsVmData.h"
#include "PrlHandleVmDefaultConfig.h"
#include "PrlHandleVmGuest.h"
#include "PrlHandleApplianceConfig.h"
#include "PrlHandleCtTemplate.h"
#include "PrlHandleFirewallRule.h"

#include "PrlHandleSrvConfig.h"
#include "PrlHandleVmInfo.h"

#include "Build/Current.ver"

#include "PrlContextSwitcher.h"
#include "PrlNew.h"

////////////////////////////////////////////////////////////////////////////////////////////////

struct assert_PHT_VM_CONFIGURATION_not_equal_PHT_VIRTUAL_MACHINE
{
	// Note: If compilation is broken here, check all references
	//       on PHT_VM_CONFIGURATION handle!
	char assert_compilation[PHT_VM_CONFIGURATION == PHT_VIRTUAL_MACHINE ? 1 : -1];
};

PRL_RESULT PrlSrv_CreateVm_Impl(PRL_HANDLE hServer, PRL_HANDLE_PTR phVm)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(phVm) )
		RETURN_RES(PRL_ERR_INVALID_ARG)

	PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>(hServer);
	PrlHandleVm *pVm = new PrlHandleVm(pServer);

	if (!pVm)
		RETURN_RES(PRL_ERR_OUT_OF_MEMORY)
	*phVm = pVm->GetHandle();
	RETURN_RES(PRL_ERR_SUCCESS)
}

PRL_METHOD( PrlSrv_CreateVm) (
							  PRL_HANDLE hServer,
							  PRL_HANDLE_PTR phVm
							  )
{
	SYNC_CHECK_API_INITIALIZED
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_CreateVm, (hServer, phVm))
}

PRL_METHOD( PrlVm_RegEventHandler ) (
		PRL_HANDLE hObj,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		)
{
	return PrlHandle_RegEventHandler(hObj, handler, userData);
}


PRL_METHOD( PrlVm_UnregEventHandler ) (
		PRL_HANDLE hObj,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		)
{
	return PrlHandle_UnregEventHandler(hObj, handler, userData);
}

PRL_ASYNC_METHOD( PrlVm_Connect ) (
		PRL_HANDLE hObj,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_CONNECT_TO_VM)
	(void)nFlags;

	PRL_RESULT err = PRL_ERR_UNIMPLEMENTED;
	IHandleVm *pVm;
	if (getIHandleVm(hObj, &pVm))
	{
		PrlHandleJob *pJob = 0;
		err = pVm->VmConnect(&pJob);
		if (0 != pJob)
		{
			return pJob->GetHandle();
		}
	}

	return GENERATE_ERROR_HANDLE(err, PJOC_VM_CONNECT_TO_VM);
}

PRL_METHOD( PrlVm_Disconnect ) (
		PRL_HANDLE hObj
	)
{
	SYNC_CHECK_API_INITIALIZED

	PRL_RESULT err = PRL_ERR_UNIMPLEMENTED;
	IHandleVm *pVm;
	if (getIHandleVm(hObj, &pVm))
	{
		err = pVm->VmDisconnect();
	}
	return err;
}

PRL_METHOD( PrlApi_GetSupportedOsesTypes ) (
										PRL_HOST_OS_TYPE nHostOsType,
										PRL_HANDLE_PTR phOsesTypesList
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (nHostOsType=%.8X, phOsesTypesList=%p)",
		__FUNCTION__,
		nHostOsType,
		phOsesTypesList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_PTR(phOsesTypesList))
		return (PRL_ERR_INVALID_ARG);

	return (PrlGetSupportedOsesTypesList(nHostOsType, phOsesTypesList));
}

PRL_METHOD( PrlApi_GetSupportedOsesVersions ) (
										PRL_HOST_OS_TYPE nHostOsType,
										PRL_UINT8 nGuestOsType,
										PRL_HANDLE_PTR phGuestsList
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (nHostOsType=%.8X, nGuestOsType=%.8X, phGuestsList=%p)",
		__FUNCTION__,
		nHostOsType,
		nGuestOsType,
		phGuestsList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_PTR(phGuestsList))
		return (PRL_ERR_INVALID_ARG);

	return (PrlGetSupportedOsesVersionsList(nHostOsType, nGuestOsType, phGuestsList));
}

PRL_METHOD( PrlApi_GetDefaultOsVersion ) (
										PRL_UINT8 nGuestOsType,
										PRL_UINT16_PTR pnOsVersion
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (nGuestOsType=%.8X, pnOsVersion=%p)",
		__FUNCTION__,
		nGuestOsType,
		pnOsVersion
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_PTR(pnOsVersion))
		return (PRL_ERR_INVALID_ARG);

	return (PrlGetDefaultOsVersion(nGuestOsType, pnOsVersion));
}

PRL_HANDLE PrlSrv_RegisterVm_Impl(PRL_HANDLE hServer, PRL_CONST_STR strVmDirPath, PRL_BOOL bNonInteractiveMode)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(strVmDirPath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_REGISTER_VM))
		PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	PrlHandleJobPtr pJob = pServer->RegisterVm(strVmDirPath, bNonInteractiveMode);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_RegisterVm ) (
									   PRL_HANDLE hServer,
									   PRL_CONST_STR strVmDirPath,
									   PRL_BOOL bNonInteractiveMode
									   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_REGISTER_VM)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_RegisterVm, (hServer, strVmDirPath, bNonInteractiveMode ))
}

PRL_HANDLE PrlSrv_RegisterVmEx_Impl(PRL_HANDLE hServer, PRL_CONST_STR strVmDirPath, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(strVmDirPath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_REGISTER_VM_EX))
	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	PrlHandleJobPtr pJob = pServer->RegisterVmEx(strVmDirPath, NULL, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_RegisterVmEx ) (
									   PRL_HANDLE hServer,
									   PRL_CONST_STR strVmDirPath,
									   PRL_UINT32 nFlags
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%p, strVmDirPath=%s, nFlags=%.8X)",
		__FUNCTION__,
		hServer,
		strVmDirPath,
		nFlags
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_REGISTER_VM_EX)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_RegisterVmEx, (hServer, strVmDirPath, nFlags ))
}

PRL_HANDLE PrlSrv_RegisterVmWithUuid_Impl(PRL_HANDLE hServer, PRL_CONST_STR strVmDirPath,
			PRL_CONST_STR strVmUuid, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(strVmDirPath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_REGISTER_VM))
	if (strVmUuid == NULL || !Uuid::isUuid(QString(strVmUuid)))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_REGISTER_VM))

	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	PrlHandleJobPtr pJob = pServer->RegisterVmEx(strVmDirPath, strVmUuid, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_RegisterVmWithUuid ) (
									   PRL_HANDLE hServer,
									   PRL_CONST_STR strVmDirPath,
									   PRL_CONST_STR strVmUuid,
									   PRL_UINT32 nFlags
									   )
{

	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_REGISTER_VM)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_RegisterVmWithUuid, (	hServer, strVmDirPath, strVmUuid, nFlags ))
}

PRL_HANDLE PrlSrv_Register3rdPartyVm_Impl(PRL_HANDLE hServer, PRL_CONST_STR strVmConfigPath, PRL_CONST_STR strVmRootDirPath, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(strVmConfigPath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_REGISTER_3RD_PARTY_VM))

	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	PrlHandleJobPtr pJob = pServer->Register3rdPartyVm(strVmConfigPath, strVmRootDirPath, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_Register3rdPartyVm ) (
									   PRL_HANDLE hServer,
									   PRL_CONST_STR strVmConfigPath,
									   PRL_CONST_STR strVmRootDirPath,
									   PRL_UINT32 nFlags
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%p, strVmConfigPath=%s, strVmRootDirPath=%s, nFlags=%.8X)",
		__FUNCTION__,
		hServer,
		strVmConfigPath,
		strVmRootDirPath,
		nFlags
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_REGISTER_3RD_PARTY_VM)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_Register3rdPartyVm, (hServer, strVmConfigPath, strVmRootDirPath, nFlags ))
}

PRL_HANDLE PrlSrv_GetVmList_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_VM_METH_IMPLEMENTATION_WITH_FLAGS(GetVmList, nFlags, PJOC_SRV_GET_VM_LIST)
}

PRL_ASYNC_METHOD( PrlSrv_GetVmList ) (
									  PRL_HANDLE hServer
									  )
{
	return PrlSrv_GetVmListEx(hServer, 0);
}

PRL_ASYNC_METHOD( PrlSrv_GetVmListEx ) (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_VM_LIST)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetVmList, (hServer, nFlags))
}

PRL_HANDLE PrlVm_Clone_Impl(PRL_HANDLE hVm,
		PRL_CONST_STR new_vm_name,
		PRL_CONST_STR new_vm_uuid,
		PRL_CONST_STR new_vm_config_path,
		PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) ||
			PRL_WRONG_PTR(new_vm_name) ||
			PRL_WRONG_PTR(new_vm_uuid) ||
			PRL_WRONG_PTR(new_vm_config_path) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_CLONE))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->Clone(new_vm_name, new_vm_uuid, new_vm_config_path, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_CloneEx ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR new_vm_name,
		PRL_CONST_STR new_vm_config_path,
		PRL_UINT32 nFlags
		)
{
	return PrlVm_Clone_Impl(hVm, new_vm_name, "", new_vm_config_path, nFlags);
}

PRL_HANDLE PrlVm_Delete_Impl(PRL_HANDLE hVm,PRL_HANDLE hDevicesList)
{
	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE)
		|| (hDevicesList != PRL_INVALID_HANDLE && PRL_WRONG_HANDLE(hDevicesList, PHT_STRINGS_LIST)) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_DELETE))

		PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	if (!pVm->GetServer())
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_DELETE))

		PrlHandleStringsListPtr pStringsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hDevicesList);

	PrlHandleJobPtr pJob = pVm->Delete(
		hDevicesList != PRL_INVALID_HANDLE ? pStringsList->GetStringsList() : QStringList());
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_Delete ) (
		PRL_HANDLE hVm,
		PRL_HANDLE hDevicesList
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_DELETE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Delete, (hVm,hDevicesList))
}

PRL_HANDLE PrlVm_GetProblemReport_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(GetProblemReport, PJOC_VM_GET_PROBLEM_REPORT);
}

PRL_ASYNC_METHOD( PrlVm_GetProblemReport ) (
								  PRL_HANDLE hVm
								  )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GET_PROBLEM_REPORT)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_GetProblemReport, (hVm))
}

PRL_HANDLE PrlVm_GetPackedProblemReport_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(GetPackedProblemReport, nFlags, PJOC_VM_GET_PACKED_PROBLEM_REPORT)
}

PRL_ASYNC_METHOD( PrlVm_GetPackedProblemReport ) (
	PRL_HANDLE hVm,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GET_PACKED_PROBLEM_REPORT)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_GetPackedProblemReport, (hVm,nFlags))
}

PRL_ASYNC_METHOD( PrlVm_Clone ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR new_vm_name,
		PRL_CONST_STR new_vm_config_path,
		PRL_BOOL bCreateTemplate
		)
{
	return PrlVm_Clone_Impl(hVm, new_vm_name, "", new_vm_config_path,
			bCreateTemplate ? PCVF_CLONE_TO_TEMPLATE : 0);
}

PRL_ASYNC_METHOD( PrlVm_CloneWithUuid ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR new_vm_name,
		PRL_CONST_STR new_vm_uuid,
		PRL_CONST_STR new_vm_config_path,
		PRL_UINT32 nFlags
		)
{
	return PrlVm_Clone_Impl(hVm, new_vm_name, new_vm_uuid,
			new_vm_config_path, nFlags);
}

PRL_HANDLE PrlVm_GenerateVmDevFilename_Impl(
	PRL_HANDLE hVm,
	PRL_CONST_STR sFilenamePrefix,
	PRL_CONST_STR sFilenameSuffix,
	PRL_CONST_STR sIndexDelimiter
	)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_GENERATE_VM_DEV_FILENAME))
		PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->GenerateVmDevFilename(sFilenamePrefix, sFilenameSuffix, sIndexDelimiter);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_GenerateVmDevFilename ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR sFilenamePrefix,
		PRL_CONST_STR sFilenameSuffix,
		PRL_CONST_STR sIndexDelimiter
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GENERATE_VM_DEV_FILENAME)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_GenerateVmDevFilename,\
								(hVm, sFilenamePrefix, sFilenameSuffix, sIndexDelimiter))
}

PRL_HANDLE PrlVm_ChangeSid_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(ChangeMachineSid, nFlags, PJOC_VM_CHANGE_SID)
}

PRL_ASYNC_METHOD( PrlVm_ChangeSid ) (
					   PRL_HANDLE hVm,
					   PRL_UINT32 nFlags
							   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, nFlags=%.8X)",
		__FUNCTION__,
		hVm,
		nFlags
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_CHANGE_SID)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_ChangeSid, (hVm, nFlags))
}

PRL_HANDLE PrlVm_ResetUptime_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(ResetUptime, nFlags, PJOC_VM_RESET_UPTIME)
}

PRL_ASYNC_METHOD( PrlVm_ResetUptime) (
					   PRL_HANDLE hVm,
					   PRL_UINT32 nFlags
							   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, nFlags=%.8X)",
		__FUNCTION__,
		hVm,
		nFlags
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_RESET_UPTIME)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_ResetUptime, (hVm, nFlags))
}

PRL_HANDLE PrlVm_Start_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(Start, PJOC_VM_START)
}

PRL_ASYNC_METHOD( PrlVm_Start ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_START)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Start, (hVm))
}

PRL_ASYNC_METHOD( PrlVm_Resume ) (
		PRL_HANDLE hVm
		)
{
	return (PrlVm_Start(hVm));
}

PRL_HANDLE PrlVm_StartEx_Impl(PRL_HANDLE hVm, PRL_UINT32 nStartMode, PRL_UINT32 nReserved)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || nStartMode > PSM_VM_START_LAST_ITEM )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_START_EX))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->StartEx(nStartMode, nReserved);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_StartEx ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nStartMode,
		PRL_UINT32 nReserved
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_START_EX)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_StartEx, (hVm, nStartMode, nReserved))
}

PRL_HANDLE PrlVm_StartVncServer_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(StartVncServer, nFlags, PJOC_VM_START_VNC_SERVER)
}

PRL_ASYNC_METHOD( PrlVm_StartVncServer ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nReserved
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, nReserved=%.8X)",
		__FUNCTION__,
		hVm,
		nReserved
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_START_VNC_SERVER)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_StartVncServer, (hVm, nReserved))
}

PRL_HANDLE PrlVm_StopVncServer_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(StopVncServer, nFlags, PJOC_VM_STOP_VNC_SERVER)
}

PRL_ASYNC_METHOD( PrlVm_StopVncServer ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nReserved
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, nReserved=%.8X)",
		__FUNCTION__,
		hVm,
		nReserved
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_STOP_VNC_SERVER)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_StopVncServer, (hVm, nReserved))
}

PRL_HANDLE PrlVm_Lock_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(Lock, nFlags, PJOC_VM_LOCK)
}

PRL_ASYNC_METHOD( PrlVm_Lock ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nReserved
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, nReserved=%.8X)",
		__FUNCTION__,
		hVm,
		nReserved
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_LOCK)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Lock, (hVm, nReserved))
}

PRL_HANDLE PrlVm_Unlock_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(Unlock, nFlags, PJOC_VM_UNLOCK)
}

PRL_ASYNC_METHOD( PrlVm_Unlock ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nReserved
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, nReserved=%.8X)",
		__FUNCTION__,
		hVm,
		nReserved
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_UNLOCK)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Unlock, (hVm, nReserved))
}

PRL_HANDLE PrlVm_Restart_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(RestartGuest, PJOC_VM_RESTART)
}

PRL_ASYNC_METHOD( PrlVm_Restart ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_RESTART)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Restart, (hVm))
}

PRL_HANDLE PrlVm_Unreg_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(Unreg, PJOC_VM_UNREG)
}

PRL_ASYNC_METHOD( PrlVm_Unreg ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_UNREG)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Unreg, (hVm))
}

PRL_HANDLE PrlVm_Restore_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(Restore, PJOC_VM_RESTORE)
}

PRL_ASYNC_METHOD( PrlVm_Restore ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_RESTORE)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Restore, (hVm))
}

PRL_HANDLE PrlVm_SubscribeToGuestStatistics_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(SubscribeToGuestStatistics, PJOC_VM_SUBSCRIBE_TO_GUEST_STATISTICS)
}

PRL_ASYNC_METHOD( PrlVm_SubscribeToGuestStatistics ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_SUBSCRIBE_TO_GUEST_STATISTICS)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_SubscribeToGuestStatistics, (hVm))
}

PRL_HANDLE PrlVm_UnsubscribeFromGuestStatistics_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(UnsubscribeFromGuestStatistics, PJOC_VM_UNSUBSCRIBE_FROM_GUEST_STATISTICS)
}

PRL_ASYNC_METHOD( PrlVm_UnsubscribeFromGuestStatistics ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_UNSUBSCRIBE_FROM_GUEST_STATISTICS)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_UnsubscribeFromGuestStatistics, (hVm))
}

PRL_HANDLE PrlVm_Stop_Impl(PRL_HANDLE hVm, PRL_BOOL bAcpi)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_STOP))
		PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->Stop(bAcpi ? PSM_SHUTDOWN : PSM_KILL, 0);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_Stop ) (
		PRL_HANDLE hVm,
		PRL_BOOL bAcpi
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_STOP)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Stop, (hVm, bAcpi))
}

PRL_HANDLE PrlVm_StopEx_Impl(PRL_HANDLE hVm,
		PRL_UINT32 nStopMode,
		PRL_UINT32 nFlags
		)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) ||
	     nStopMode > PSM_LAST)
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_STOP))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->Stop(nStopMode, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_StopEx ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nStopMode,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_STOP)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_StopEx, (hVm, nStopMode, nFlags))
}

PRL_HANDLE PrlVm_Pause_Impl(PRL_HANDLE hVm, PRL_BOOL bAcpi)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_PAUSE))
		PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->Pause(bAcpi);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_Pause ) (
		PRL_HANDLE hVm,
		PRL_BOOL bAcpi
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_PAUSE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Pause, (hVm, bAcpi))
}

PRL_HANDLE PrlVm_Suspend_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(Suspend, PJOC_VM_SUSPEND)
}

PRL_ASYNC_METHOD( PrlVm_Suspend ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_SUSPEND)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Suspend, (hVm))
}

PRL_HANDLE PrlVm_GetSuspendedScreen_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(GetSuspendedScreen, PJOC_VM_GET_SUSPENDED_SCREEN)
}

PRL_ASYNC_METHOD( PrlVm_GetSuspendedScreen ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GET_SUSPENDED_SCREEN)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_GetSuspendedScreen, (hVm))
}

PRL_HANDLE PrlVm_CreateSnapshot_Impl(PRL_HANDLE hVm, PRL_CONST_STR sName, PRL_CONST_STR sDescription)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sName) || PRL_WRONG_PTR(sDescription) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_CREATE_SNAPSHOT))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->CreateSnapshot(sName, sDescription);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_CreateSnapshot ) (
								  PRL_HANDLE hVm,
								  PRL_CONST_STR sName,
								  PRL_CONST_STR sDescription
								  )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_CREATE_SNAPSHOT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_CreateSnapshot, (hVm, sName, sDescription))
}

PRL_HANDLE PrlVm_SwitchToSnapshot_Impl(PRL_HANDLE hVm, PRL_CONST_STR sSnapshotUuid,
		PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sSnapshotUuid) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_SWITCH_TO_SNAPSHOT))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->SwitchToSnapshot(sSnapshotUuid, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_SwitchToSnapshotEx ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR sSnapshotUuid,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_SWITCH_TO_SNAPSHOT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_SwitchToSnapshot, (hVm,
				sSnapshotUuid, nFlags))
}

PRL_ASYNC_METHOD( PrlVm_SwitchToSnapshot ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR sSnapshotUuid
		)
{
	return PrlVm_SwitchToSnapshotEx(hVm, sSnapshotUuid, 0);
}

PRL_HANDLE PrlVm_DeleteSnapshot_Impl(PRL_HANDLE hVm, PRL_CONST_STR sSnapshotUuid, const PRL_BOOL bChild)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sSnapshotUuid) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_DELETE_SNAPSHOT))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->DeleteSnapshot(sSnapshotUuid, bChild);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_DeleteSnapshot ) (
								  PRL_HANDLE hVm,
								  PRL_CONST_STR sSnapshotUuid,
								  PRL_BOOL bChild
								  )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_DELETE_SNAPSHOT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_DeleteSnapshot, (hVm, sSnapshotUuid, bChild))
}

PRL_ASYNC_METHOD( PrlVm_GetSnapshotsTree ) (
	PRL_HANDLE hVm
	)
{
	return PrlVm_GetSnapshotsTreeEx( hVm, 0 );
}

PRL_HANDLE PrlVm_GetSnapshotsTreeEx_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(GetSnapshotsTree, nFlags, PJOC_VM_GET_SNAPSHOTS_TREE)
}

PRL_ASYNC_METHOD( PrlVm_GetSnapshotsTreeEx ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GET_SNAPSHOTS_TREE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_GetSnapshotsTreeEx, (hVm, nFlags))
}

PRL_HANDLE PrlVm_UpdateSnapshotData_Impl(PRL_HANDLE hVm,
												  PRL_CONST_STR sSnapshotUuid,
												  PRL_CONST_STR sNewName,
												  PRL_CONST_STR sNewDescription)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sSnapshotUuid) ||
		 PRL_WRONG_PTR(sNewName) || PRL_WRONG_PTR(sNewDescription) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_UPDATE_SNAPSHOT_DATA))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->UpdateSnapshotData(sSnapshotUuid, sNewName, sNewDescription);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_UpdateSnapshotData ) (
	PRL_HANDLE hVm,
	PRL_CONST_STR sSnapshotUuid,
	PRL_CONST_STR sNewName,
	PRL_CONST_STR sNewDescription
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_UPDATE_SNAPSHOT_DATA)

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(),
		PrlVm_UpdateSnapshotData,
		(hVm, sSnapshotUuid, sNewName, sNewDescription)
		)
}

PRL_HANDLE PrlVm_Reset_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(Reset, PJOC_VM_RESET)
}

PRL_ASYNC_METHOD( PrlVm_Reset ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_RESET)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Reset, (hVm))
}

PRL_HANDLE PrlVm_InstallTools_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(InstallTools, PJOC_VM_INSTALL_TOOLS)
}

PRL_ASYNC_METHOD( PrlVm_InstallTools ) (
					   PRL_HANDLE hVm
							   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_INSTALL_TOOLS)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_InstallTools, (hVm))
}

PRL_HANDLE PrlVm_BeginEdit_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(BeginEdit, PJOC_VM_BEGIN_EDIT)
}

PRL_ASYNC_METHOD( PrlVm_BeginEdit ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_BEGIN_EDIT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_BeginEdit, (hVm))
}

PRL_HANDLE PrlVm_Commit_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(Commit, 0, PJOC_VM_COMMIT)
}

PRL_ASYNC_METHOD( PrlVm_Commit ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_COMMIT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Commit, (hVm))
}

PRL_HANDLE PrlVm_CommitEx_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(Commit, nFlags, PJOC_VM_COMMIT)
}

PRL_ASYNC_METHOD( PrlVm_CommitEx ) (
								  PRL_HANDLE hVm,
								  PRL_UINT32 nFlags
								  )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_COMMIT)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_CommitEx, (hVm, nFlags))
}

PRL_HANDLE PrlVm_GetState_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(GetState, PJOC_VM_GET_STATE)
}

PRL_ASYNC_METHOD( PrlVm_GetState ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GET_STATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_GetState, (hVm))
}

PRL_HANDLE PrlVm_GetToolsState_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(GetToolsState, PJOC_VM_GET_TOOLS_STATE)
}

PRL_ASYNC_METHOD( PrlVm_GetToolsState ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GET_TOOLS_STATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_GetToolsState, (hVm))
}

PRL_HANDLE PrlVm_RefreshConfig_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(RefreshConfig, 0, PJOC_VM_REFRESH_CONFIG)
}

PRL_ASYNC_METHOD( PrlVm_RefreshConfig ) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_REFRESH_CONFIG)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_RefreshConfig, (hVm))
}

PRL_HANDLE PrlVm_RefreshConfigEx_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(RefreshConfig, nFlags, PJOC_VM_REFRESH_CONFIG)
}

PRL_ASYNC_METHOD( PrlVm_RefreshConfigEx ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_REFRESH_CONFIG)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_RefreshConfigEx, (hVm, nFlags))
}

PRL_ASYNC_METHOD( PrlVm_GetStatistics ) (
		PRL_HANDLE hVm
		)
{
	return PrlVm_GetStatisticsEx(hVm, 0);
}

PRL_HANDLE PrlVm_GetStatisticsEx_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(GetStatistics, nFlags, PJOC_VM_GET_STATISTICS)
}

PRL_ASYNC_METHOD( PrlVm_GetStatisticsEx ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GET_STATISTICS)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_GetStatisticsEx, (hVm, nFlags))
}

PRL_HANDLE PrlVm_InitiateDevStateNotifications_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(InitiateDevStateNotifications, PJOC_VM_INITIATE_DEV_STATE_NOTIFICATIONS)
}

PRL_ASYNC_METHOD( PrlVm_InitiateDevStateNotifications ) (
								PRL_HANDLE hVm
								)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_INITIATE_DEV_STATE_NOTIFICATIONS)
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p)",
		__FUNCTION__,
		hVm
		);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_InitiateDevStateNotifications, (hVm))
}

PRL_HANDLE PrlVm_UpdateSecurity_Impl(PRL_HANDLE hVm, PRL_HANDLE hAccessRights)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_HANDLE(hAccessRights, PHT_ACCESS_RIGHTS) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_UPDATE_SECURITY))
		PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleAccessRightsPtr pAccessRights = PRL_OBJECT_BY_HANDLE<PrlHandleAccessRights>(hAccessRights);
	PrlHandleJobPtr pJob = pVm->UpdateSecurity(pAccessRights);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_UpdateSecurity ) (
								PRL_HANDLE hVm,
								PRL_HANDLE hAccessRights
								)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_UPDATE_SECURITY)
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p hAccessRights=%p)",
		__FUNCTION__,
		hVm,
		hAccessRights
		);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_UpdateSecurity, (hVm, hAccessRights))
}

PRL_HANDLE PrlVm_ValidateConfig_Impl(PRL_HANDLE hVm, PRL_VM_CONFIG_SECTIONS nSection)
{
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->ValidateConfig(nSection);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_ValidateConfig ) (
	PRL_HANDLE hVm,
	PRL_VM_CONFIG_SECTIONS nSection
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_VALIDATE_CONFIG)

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, nSection=%d)",
	__FUNCTION__,
	hVm,
	nSection
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE)
		|| !(nSection >= PVC_VALIDATE_CHANGES_ONLY && nSection <= PVC_LAST_SECTION))
		return GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_VALIDATE_CONFIG);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_ValidateConfig, (hVm, nSection))
}

PRL_HANDLE PrlVm_Authorise_Impl(
	PRL_HANDLE hVm,
	PRL_CONST_STR sPassword,
	PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_AUTHORISE));

	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>( hVm );
	PrlHandleJobPtr pJob = pVm->Authorise( sPassword, nFlags );
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_Authorise ) (
					  PRL_HANDLE hVm,
					  PRL_CONST_STR sPassword,
					  PRL_UINT32 nFlags
					  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, sPassword=%p, nFlags=%.8x)",
		__FUNCTION__,
		hVm,
		sPassword,
		nFlags
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_AUTHORISE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Authorise, (hVm, sPassword, nFlags))
}

PRL_HANDLE PrlVm_ChangePassword_Impl(
	PRL_HANDLE hVm,
	PRL_CONST_STR sOldPassword,
	PRL_CONST_STR sNewPassword,
	PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_CHANGE_PASSWORD));

	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>( hVm );
	PrlHandleJobPtr pJob = pVm->ChangePassword( sOldPassword, sNewPassword, nFlags );
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_ChangePassword ) (
					  PRL_HANDLE hVm,
					  PRL_CONST_STR sOldPassword,
					  PRL_CONST_STR sNewPassword,
					  PRL_UINT32 nFlags
					  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, sOldPassword=%p, sNewPassword=%p nFlags=%.8x)",
		__FUNCTION__,
		hVm,
		sOldPassword,
		sNewPassword,
		nFlags
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_CHANGE_PASSWORD)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_ChangePassword, (hVm, sOldPassword, sNewPassword, nFlags))
}

PRL_HANDLE PrlVm_Encrypt_Impl(
	PRL_HANDLE hVm,
	PRL_CONST_STR sPassword,
	PRL_CONST_STR sCipherPluginUuid,
	PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_ENCRYPT));

	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>( hVm );
	PrlHandleJobPtr pJob = pVm->Encrypt( sPassword, sCipherPluginUuid, nFlags );
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_Encrypt ) (
					  PRL_HANDLE hVm,
					  PRL_CONST_STR sPassword,
					  PRL_CONST_STR sCipherPluginUuid,
					  PRL_UINT32 nFlags
					  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, sPassword=%p, sCipherPluginUuid=%s, nFlags=%.8x)",
		__FUNCTION__,
		hVm,
		sPassword,
		sCipherPluginUuid,
		nFlags
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_ENCRYPT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Encrypt, (hVm, sPassword, sCipherPluginUuid, nFlags))
}

PRL_HANDLE PrlVm_Decrypt_Impl(
	PRL_HANDLE hVm,
	PRL_CONST_STR sPassword,
	PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_DECRYPT));

	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>( hVm );
	PrlHandleJobPtr pJob = pVm->Decrypt( sPassword, nFlags );
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_Decrypt ) (
					  PRL_HANDLE hVm,
					  PRL_CONST_STR sPassword,
					  PRL_UINT32 nFlags
					  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, sPassword=%p, nFlags=%.8x)",
		__FUNCTION__,
		hVm,
		sPassword,
		nFlags
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_DECRYPT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Decrypt, (hVm, sPassword, nFlags))
}

PRL_METHOD( PrlVm_GetQuestions ) (
		PRL_HANDLE hVm,
		PRL_HANDLE_PTR pQuestionsList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pQuestionsList=%p)",
		__FUNCTION__,
		hVm,
		pQuestionsList
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(pQuestionsList) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );
	PrlHandleServerDispPtr pServer = pVm->GetServerDisp();
	if (!pServer)
	{
		//Abnormal situation
		WRITE_TRACE(DBG_FATAL, "VM object with id '%s' not bound to server detected",\
					pVm->GetUuid().toUtf8().constData());
		return (PRL_ERR_UNEXPECTED);
	}
	try
	{
		PrlHandleHandlesList  *pHandlesList =
			new PrlHandleHandlesList(pServer->GetQuestionsList().GetQuestionsByIssuerId(pVm->GetUuid()));
		*pQuestionsList = pHandlesList->GetHandle();
	}
	catch(...)
	{
		*pQuestionsList = PRL_INVALID_HANDLE;
		return (PRL_ERR_OUT_OF_MEMORY);
	}

	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlVm_CreateEvent ) (
		PRL_HANDLE hVm,
		PRL_HANDLE_PTR phEvent
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, phEvent=%p)",
		__FUNCTION__,
		hVm,
		phEvent
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(phEvent))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );
#ifdef _WIN_
#ifdef CreateEvent
#undef CreateEvent
#endif
#endif
	return (pVm->CreateEvent(phEvent));
}

PRL_METHOD( PrlVmInfo_GetState) (
		PRL_HANDLE handle,
		VIRTUAL_MACHINE_STATE_PTR pVmState
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pVmState=%p)",
		__FUNCTION__,
		handle,
		pVmState
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_VM_INFO) || PRL_WRONG_PTR(pVmState) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleVmInfo>( handle );
	return (pVmInfo->GetState(pVmState));
}

PRL_METHOD( PrlVmInfo_GetAdditionState) (
								 PRL_HANDLE handle,
								 VIRTUAL_MACHINE_ADDITION_STATE_PTR pVmState
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pVmState=%p)",
		__FUNCTION__,
		handle,
		pVmState
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_VM_INFO) || PRL_WRONG_PTR(pVmState) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleVmInfo>( handle );
	return (pVmInfo->GetAdditionState(pVmState));
}

PRL_METHOD( PrlVmInfo_GetAccessRights ) (
		PRL_HANDLE hVmInfo,
		PRL_HANDLE_PTR phVmAcl
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmInfo=%p, phVmAcl=%p)",
		__FUNCTION__,
		hVmInfo,
		phVmAcl
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmInfo, PHT_VM_INFO) || PRL_WRONG_PTR(phVmAcl) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleVmInfo>( hVmInfo );
	return (pVmInfo->GetAccessRights(phVmAcl));
}

PRL_METHOD( PrlVmInfo_IsInvalid) (
								 PRL_HANDLE handle,
								 PRL_BOOL_PTR pbIsInvalid
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pbVmIsInvalid=%p)",
		__FUNCTION__,
		handle,
		pbIsInvalid
		);

	if ( PRL_WRONG_HANDLE(handle, PHT_VM_INFO) || PRL_WRONG_PTR(pbIsInvalid) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleVmInfo>( handle );

	PRL_RESULT nRcValid=0;
	return (pVmInfo->IsInvalid(pbIsInvalid, &nRcValid));
}

PRL_METHOD( PrlVmInfo_IsInvalidEx) (
								 PRL_HANDLE handle,
								 PRL_BOOL_PTR pbIsInvalid,
								 PRL_RESULT_PTR pnInvalidReason
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pbVmIsInvalid=%p, pnInvalidReason=%p)",
		__FUNCTION__,
		handle,
		pbIsInvalid,
		pnInvalidReason
		);

	if ( PRL_WRONG_HANDLE(handle, PHT_VM_INFO)
		|| PRL_WRONG_PTR(pbIsInvalid) || PRL_WRONG_PTR(pnInvalidReason) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleVmInfo>( handle );
	return (pVmInfo->IsInvalid(pbIsInvalid, pnInvalidReason));
}

PRL_METHOD( PrlVmInfo_IsVmWaitingForAnswer) (
								 PRL_HANDLE handle,
								 PRL_BOOL_PTR pbIsVmWaitingForAnswer
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pbIsVmWaitingForAnswer=%p)",
		__FUNCTION__,
		handle,
		pbIsVmWaitingForAnswer
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_VM_INFO) || PRL_WRONG_PTR(pbIsVmWaitingForAnswer) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleVmInfo>( handle );
	return (pVmInfo->IsVmWaitingForAnswer(pbIsVmWaitingForAnswer));
}

PRL_METHOD( PrlVmInfo_IsVncServerStarted ) (
		PRL_HANDLE handle,
		PRL_BOOL_PTR pbIsVncServerStarted
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pbIsVncServerStarted=%p)",
		__FUNCTION__,
		handle,
		pbIsVncServerStarted
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_VM_INFO) || PRL_WRONG_PTR(pbIsVncServerStarted) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleVmInfo>( handle );
	return (pVmInfo->IsVncServerSrtated(pbIsVncServerStarted));
}

PRL_HANDLE PrlSrv_StartSearchVms_Impl(PRL_HANDLE hServer,	PRL_HANDLE hStringsList)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER)
		|| (hStringsList != PRL_INVALID_HANDLE && PRL_WRONG_HANDLE(hStringsList, PHT_STRINGS_LIST)) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_START_SEARCH_VMS))
	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	PrlHandleJobPtr pJob = pServer->StartSearchVms(hStringsList);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlSrv_GetVmConfig_Impl(PRL_HANDLE hServer, PRL_CONST_STR sSearchId, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_GET_VM_CONFIG));
	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	PrlHandleJobPtr pJob = pServer->GetVmConfig(sSearchId, nFlags);
	if ( !pJob )
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_GetVmConfig ) (
		PRL_HANDLE hServer,
		PRL_CONST_STR sSearchId,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_VM_CONFIG)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetVmConfig, (hServer, sSearchId, nFlags))
}

PRL_METHOD( PrlVm_SetConfig ) (
		PRL_HANDLE hVm,
		PRL_HANDLE hVmCfg
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE)
		 || PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVm);
	return pVm->SetConfig(hVmCfg);
}

PRL_METHOD( PrlVm_GetConfig ) (
		PRL_HANDLE hVm,
		PRL_HANDLE_PTR phVmCfg
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, phVmCfg=%p)",
		__FUNCTION__,
		hVm,
		phVmCfg
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(phVmCfg) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVm);
	return pVm->GetConfig(phVmCfg);
}

PRL_METHOD( PrlVmCfg_IsOfflineManagementEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsOfflineManagementEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetOfflineManagementEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetOfflineManagementEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_SetToolsAutoUpdateEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetToolsAutoUpdateEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsToolsAutoUpdateEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsToolsAutoUpdateEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_GetConfirmationsList ) (
	PRL_HANDLE hVmCfg,
	PRL_HANDLE_PTR phConfirmList
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phConfirmList=%p)",
		__FUNCTION__,
		hVmCfg,
		phConfirmList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phConfirmList))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetConfirmationsList(phConfirmList));
}


PRL_METHOD( PrlVmCfg_SetConfirmationsList ) (
	PRL_HANDLE hVmCfg,
	PRL_HANDLE hConfirmList
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, hConfirmList=%p)",
		__FUNCTION__,
		hVmCfg,
		hConfirmList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_HANDLE(hConfirmList, PHT_OPAQUE_TYPE_LIST))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	PrlHandleOpTypeListPtr pList = PRL_OBJECT_BY_HANDLE<PrlHandleOpTypeList>(hConfirmList);
	return (pVm->SetConfirmationsList(pList));
}


PRL_METHOD( PrlVmCfg_IsAutoCompressEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsAutoCompressEnabled(pbEnabled));
}


PRL_METHOD( PrlVmCfg_SetAutoCompressEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetAutoCompressEnabled(bEnabled));
}


PRL_METHOD( PrlVmCfg_GetAutoCompressInterval ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnInterval
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnInterval=%p)",
		__FUNCTION__,
		hVmCfg,
		pnInterval
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnInterval) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetAutoCompressInterval(pnInterval));
}


PRL_METHOD( PrlVmCfg_SetAutoCompressInterval ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nInterval
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nInterval=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nInterval
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetAutoCompressInterval(nInterval));
}


PRL_METHOD( PrlVmCfg_GetFreeDiskSpaceRatio ) (
		PRL_HANDLE hVmCfg,
		PRL_DOUBLE_PTR pdFreeDiskSpaceRatio
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pdFreeDiskSpaceRatio=%p)",
		__FUNCTION__,
		hVmCfg,
		pdFreeDiskSpaceRatio
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pdFreeDiskSpaceRatio) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetFreeDiskSpaceRatio(pdFreeDiskSpaceRatio));
}


PRL_METHOD( PrlVmCfg_SetFreeDiskSpaceRatio ) (
		PRL_HANDLE hVmCfg,
		PRL_DOUBLE dFreeDiskSpaceRatio
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, dFreeDiskSpaceRatio=%f)",
		__FUNCTION__,
		hVmCfg,
		dFreeDiskSpaceRatio
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetFreeDiskSpaceRatio(dFreeDiskSpaceRatio));
}

PRL_METHOD( PrlVmCfg_IsConfigInvalid ) (
		PRL_RESULT nErrCode,
		PRL_BOOL_PTR pbInvalid
		)
{
	switch (nErrCode)
	{
	case PRL_ERR_VM_CONFIG_DOESNT_EXIST:
	case PRL_ERR_PARSE_VM_CONFIG:
	case PRL_ERR_VM_CONFIG_INVALID_VM_UUID:
	case PRL_ERR_VM_CONFIG_INVALID_SERVER_UUID:
		*pbInvalid = PRL_TRUE;
		break;
	default:
		*pbInvalid = PRL_FALSE;
		break;
	}
	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlVmCfg_GetConfigValidity ) (
								 PRL_HANDLE hVmCfg,
								 PRL_RESULT_PTR pnErrCode
								 )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p pnErrCode=%p)",
		__FUNCTION__,
		hVmCfg,
		pnErrCode
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(pnErrCode) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);

	return pVm->GetConfigValidity( pnErrCode );
}

PRL_METHOD( PrlVmCfg_GetName) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmName,
		PRL_UINT32_PTR pnVmNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmName=%s, pnVmNameBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmName,
		pnVmNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetName(sVmName, pnVmNameBufLength));
}


PRL_METHOD( PrlVmCfg_SetName) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewVmName=%s)",
		__FUNCTION__,
		hVmCfg,
		sNewVmName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmName))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetName(sNewVmName));
}


PRL_METHOD( PrlVmCfg_GetHostname) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmHostname,
		PRL_UINT32_PTR pnVmHostnameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmHostname=%s, pnVmHostnameBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmHostname,
		pnVmHostnameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmHostnameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetHostname(sVmHostname, pnVmHostnameBufLength));
}


PRL_METHOD( PrlVmCfg_SetHostname) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmHostname
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewVmHostname=%s)",
		__FUNCTION__,
		hVmCfg,
		sNewVmHostname
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmHostname))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetHostname(sNewVmHostname));
}

PRL_METHOD( PrlVmCfg_GetUuid) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmUuid,
		PRL_UINT32_PTR pnVmUuidBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmUuid=%s, pnVmUuidBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmUuid,
		pnVmUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmUuidBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetUuid(sVmUuid, pnVmUuidBufLength));
}

PRL_METHOD( PrlVmCfg_GetEnvId) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnEnvId
		)
{
	PRL_UNUSED_PARAM(hVmCfg);

	SYNC_CHECK_API_INITIALIZED
	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnEnvId))
		return (PRL_ERR_INVALID_ARG);

	*pnEnvId = 0;

	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlVmCfg_GetCtId) (
		PRL_HANDLE hVmCfg,
		PRL_STR sCtId, 
		PRL_UINT32_PTR pnCtIdBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnCtIdBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetCtId(sCtId, pnCtIdBufLength);
}


PRL_METHOD( PrlVmCfg_SetUuid) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmUuid
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewVmUuid=%s)",
		__FUNCTION__,
		hVmCfg,
		sNewVmUuid
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmUuid))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetUuid(sNewVmUuid));
}


PRL_METHOD( PrlVmCfg_GetLinkedVmUuid) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmUuid,
		PRL_UINT32_PTR pnVmUuidBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmUuid=%s, pnVmUuidBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmUuid,
		pnVmUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmUuidBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetLinkedVmUuid(sVmUuid, pnVmUuidBufLength));
}

PRL_METHOD( PrlVmCfg_GetOsType ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmOsType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmOsType=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmOsType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmOsType))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetOsType(pnVmOsType));
}

PRL_METHOD( PrlVmCfg_GetOsVersion ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmOsVersion
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmOsVersion=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmOsVersion
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmOsVersion))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetOsVersion(pnVmOsVersion));
}

PRL_METHOD( PrlVmCfg_SetOsVersion ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmOsVersion
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmOsVersion=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmOsVersion
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetOsVersion(nVmOsVersion));
}

PRL_METHOD( PrlVmCfg_GetRamSize ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmRamSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmRamSize=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmRamSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmRamSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetRamSize(pnVmRamSize));
}

PRL_METHOD( PrlVmCfg_SetRamSize ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmRamSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmRamSize=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmRamSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(nVmRamSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetRamSize(nVmRamSize));
}

PRL_METHOD( PrlVmCfg_GetVideoRamSize ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmVideoRamSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmVideoRamSize=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmVideoRamSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmVideoRamSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetVideoRamSize(pnVmVideoRamSize));
}

PRL_METHOD( PrlVmCfg_SetVideoRamSize ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmVideoRamSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmVideoRamSize=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmVideoRamSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(nVmVideoRamSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetVideoRamSize(nVmVideoRamSize));
}

PRL_METHOD( PrlVmCfg_GetHostMemQuotaMin ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnHostMemQuotaMin
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnHostMemQuotaMin=%p)",
		__FUNCTION__,
		hVmCfg,
		pnHostMemQuotaMin
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnHostMemQuotaMin))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetHostMemQuotaMin(pnHostMemQuotaMin);
}

PRL_METHOD( PrlVmCfg_SetHostMemQuotaMin ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nHostMemQuotaMin
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nHostMemQuotaMin=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nHostMemQuotaMin
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetHostMemQuotaMin(nHostMemQuotaMin);
}

PRL_METHOD( PrlVmCfg_GetHostMemQuotaMax ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnHostMemQuotaMax
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnHostMemQuotaMax=%p)",
		__FUNCTION__,
		hVmCfg,
		pnHostMemQuotaMax
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnHostMemQuotaMax))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetHostMemQuotaMax(pnHostMemQuotaMax);
}

PRL_METHOD( PrlVmCfg_SetHostMemQuotaMax ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nHostMemQuotaMax
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nHostMemQuotaMax=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nHostMemQuotaMax
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetHostMemQuotaMax(nHostMemQuotaMax);
}

PRL_METHOD( PrlVmCfg_GetHostMemQuotaPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnHostMemQuotaPriority
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnHostMemQuotaPriority=%p)",
		__FUNCTION__,
		hVmCfg,
		pnHostMemQuotaPriority
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnHostMemQuotaPriority))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetHostMemQuotaPriority(pnHostMemQuotaPriority);
}

PRL_METHOD( PrlVmCfg_SetHostMemQuotaPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nHostMemQuotaPriority
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nHostMemQuotaPriority=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nHostMemQuotaPriority
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetHostMemQuotaPriority(nHostMemQuotaPriority);
}

PRL_METHOD( PrlVmCfg_IsHostMemAutoQuota ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbHostMemAutoQuota
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbHostMemAutoQuota=%p)",
		__FUNCTION__,
		hVmCfg,
		pbHostMemAutoQuota
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbHostMemAutoQuota))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->IsHostMemAutoQuota(pbHostMemAutoQuota);
}

PRL_METHOD( PrlVmCfg_SetHostMemAutoQuota ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bHostMemAutoQuota
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bHostMemAutoQuota=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bHostMemAutoQuota
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetHostMemAutoQuota(bHostMemAutoQuota);
}

PRL_METHOD( PrlVmCfg_GetMaxBalloonSize ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnMaxBalloonSize
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnMaxBalloonSize=%p)",
		__FUNCTION__,
		hVmCfg,
		pnMaxBalloonSize
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnMaxBalloonSize))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetMaxBalloonSize(pnMaxBalloonSize);
}

PRL_METHOD( PrlVmCfg_SetMaxBalloonSize ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nMaxBalloonSize
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nHostMemQuotaMax=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nMaxBalloonSize
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetMaxBalloonSize(nMaxBalloonSize);
}

PRL_METHOD( PrlVmCfg_GetCpuCount ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmCpuCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmCpuCount=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmCpuCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmCpuCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetCpuCount(pnVmCpuCount));
}

PRL_METHOD( PrlVmCfg_SetCpuCount ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmCpuCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmCpuCount=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmCpuCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCpuCount(nVmCpuCount));
}

PRL_METHOD( PrlVmCfg_GetCpuMode ) (
		PRL_HANDLE hVmCfg,
		PRL_CPU_MODE_PTR pnVmCpuMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmCpuMode=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmCpuMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmCpuMode))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetCpuMode(pnVmCpuMode));
}

PRL_METHOD( PrlVmCfg_SetCpuMode ) (
		PRL_HANDLE hVmCfg,
		PRL_CPU_MODE nVmCpuMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmCpuMode=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmCpuMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCpuMode(nVmCpuMode));
}

PRL_METHOD( PrlVmCfg_GetCpuAccelLevel ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACCELERATION_LEVEL_PTR pnVmCpuAccelLevel
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmCpuAccelLevel=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmCpuAccelLevel
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmCpuAccelLevel))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetCpuAccelLevel(pnVmCpuAccelLevel));
}

PRL_METHOD( PrlVmCfg_SetCpuAccelLevel ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACCELERATION_LEVEL nVmCpuAccelLevel
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmCpuAccelLevel=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmCpuAccelLevel
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCpuAccelLevel(nVmCpuAccelLevel));
}

PRL_METHOD( PrlVmCfg_IsCpuVtxEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmCpuVtxEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmCpuVtxEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmCpuVtxEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmCpuVtxEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsCpuVtxEnabled(pbVmCpuVtxEnabled));
}

PRL_METHOD( PrlVmCfg_IsCpuHotplugEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmCpuHotplugEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmCpuHotplugEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmCpuHotplugEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmCpuHotplugEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsCpuHotplugEnabled(pbVmCpuHotplugEnabled));
}

PRL_METHOD( PrlVmCfg_SetCpuHotplugEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmCpuHotplugEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmCpuHotplugEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmCpuHotplugEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCpuHotplugEnabled(bVmCpuHotplugEnabled));
}

PRL_METHOD( PrlVmCfg_Get3DAccelerationMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VIDEO_3D_ACCELERATION_PTR pn3DAccelerationMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pn3DAccelerationMode=%p)",
		__FUNCTION__,
		hVmCfg,
		pn3DAccelerationMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pn3DAccelerationMode))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->Get3DAccelerationMode(pn3DAccelerationMode));
}

PRL_METHOD( PrlVmCfg_Set3DAccelerationMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VIDEO_3D_ACCELERATION n3DAccelerationMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, n3DAccelerationMode=%.8X)",
		__FUNCTION__,
		hVmCfg,
		n3DAccelerationMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->Set3DAccelerationMode(n3DAccelerationMode));
}

PRL_METHOD( PrlVmCfg_IsVerticalSynchronizationEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsVerticalSynchronizationEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetVerticalSynchronizationEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetVerticalSynchronizationEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsHighResolutionEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsHighResolutionEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetHighResolutionEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetHighResolutionEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsAdaptiveHypervisorEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pbEnable=%p)",
		__FUNCTION__,
		hVm,
		pbEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(pbEnable))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->IsAdaptiveHypervisorEnabled(pbEnable);
}

PRL_METHOD( PrlVmCfg_SetAdaptiveHypervisorEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, bEnable=%.8X)",
		__FUNCTION__,
		hVm,
		bEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->SetAdaptiveHypervisorEnabled(bEnable);
}

PRL_METHOD( PrlVmCfg_IsSwitchOffWindowsLogoEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pbEnable=%p)",
		__FUNCTION__,
		hVm,
		pbEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(pbEnable))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->IsSwitchOffWindowsLogoEnabled(pbEnable);
}

PRL_METHOD( PrlVmCfg_SetSwitchOffWindowsLogoEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, bEnable=%.8X)",
		__FUNCTION__,
		hVm,
		bEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->SetSwitchOffWindowsLogoEnabled(bEnable);
}

PRL_METHOD( PrlVmCfg_IsLongerBatteryLifeEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pbEnable=%p)",
		__FUNCTION__,
		hVm,
		pbEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(pbEnable))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->IsLongerBatteryLifeEnabled(pbEnable);
}

PRL_METHOD( PrlVmCfg_SetLongerBatteryLifeEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, bEnable=%.8X)",
		__FUNCTION__,
		hVm,
		bEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->SetLongerBatteryLifeEnabled(bEnable);
}

PRL_METHOD( PrlVmCfg_IsBatteryStatusEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pbEnable=%p)",
		__FUNCTION__,
		hVm,
		pbEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(pbEnable))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->IsBatteryStatusEnabled(pbEnable);
}

PRL_METHOD( PrlVmCfg_SetBatteryStatusEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, bEnable=%.8X)",
		__FUNCTION__,
		hVm,
		bEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->SetBatteryStatusEnabled(bEnable);
}

PRL_METHOD( PrlVmCfg_IsNestedVirtualizationEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pbEnable=%p)",
		__FUNCTION__,
		hVm,
		pbEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(pbEnable))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->IsNestedVirtualizationEnabled(pbEnable);
}

PRL_METHOD( PrlVmCfg_SetNestedVirtualizationEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, bEnable=%.8X)",
		__FUNCTION__,
		hVm,
		bEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->SetNestedVirtualizationEnabled(bEnable);
}

PRL_METHOD( PrlVmCfg_IsPMUVirtualizationEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pbEnable=%p)",
		__FUNCTION__,
		hVm,
		pbEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(pbEnable))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->IsPMUVirtualizationEnabled(pbEnable);
}

PRL_METHOD( PrlVmCfg_SetPMUVirtualizationEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, bEnable=%.8X)",
		__FUNCTION__,
		hVm,
		bEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->SetPMUVirtualizationEnabled(bEnable);
}

PRL_METHOD( PrlVmCfg_IsLockGuestOnSuspendEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pbEnable=%p)",
		__FUNCTION__,
		hVm,
		pbEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(pbEnable))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->IsLockGuestOnSuspendEnabled(pbEnable);
}

PRL_METHOD( PrlVmCfg_SetLockGuestOnSuspendEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, bEnable=%.8X)",
		__FUNCTION__,
		hVm,
		bEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->SetLockGuestOnSuspendEnabled(bEnable);
}

PRL_METHOD( PrlVmCfg_IsIsolatedVmEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pbEnable=%p)",
		__FUNCTION__,
		hVm,
		pbEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(pbEnable))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->IsIsolatedVmEnabled(pbEnable);
}

PRL_METHOD( PrlVmCfg_SetIsolatedVmEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, bEnable=%.8X)",
		__FUNCTION__,
		hVm,
		bEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->SetIsolatedVmEnabled(bEnable);
}

PRL_METHOD( PrlVmCfg_GetServerUuid) (
		PRL_HANDLE hVmCfg,
		PRL_STR sServerUuid,
		PRL_UINT32_PTR pnServerUuidBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sServerUuid=%s, pnServerUuidBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sServerUuid,
		pnServerUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnServerUuidBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetServerUuid(sServerUuid, pnServerUuidBufLength));
}


PRL_METHOD( PrlVmCfg_GetServerHost) (
		PRL_HANDLE hVmCfg,
		PRL_STR sServerHost,
		PRL_UINT32_PTR pnServerHostBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sServerHost=%p, pnServerHostBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sServerHost,
		pnServerHostBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnServerHostBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetServerHost(sServerHost, pnServerHostBufLength));
}


PRL_METHOD( PrlVmCfg_GetHomePath) (
		PRL_HANDLE hVmCfg,
		PRL_STR sHomePath,
		PRL_UINT32_PTR pnHomePathBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sHomePath=%p, pnHomePathBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sHomePath,
		pnHomePathBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnHomePathBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetHomePath(sHomePath, pnHomePathBufLength));
}


PRL_METHOD( PrlVmCfg_GetLocation) (
		PRL_HANDLE hVmCfg,
		PRL_VM_LOCATION_PTR pnVmLocation
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmLocation=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmLocation
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmLocation))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetLocation(pnVmLocation));
}


PRL_METHOD( PrlVmCfg_GetIcon) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmIcon,
		PRL_UINT32_PTR pnVmIconBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmIcon=%p, pnVmIconBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmIcon,
		pnVmIconBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmIconBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetIcon(sVmIcon, pnVmIconBufLength));
}


PRL_METHOD( PrlVmCfg_SetIcon) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmIcon
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewVmIcon=%s)",
		__FUNCTION__,
		hVmCfg,
		sNewVmIcon
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmIcon))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetIcon(sNewVmIcon));
}


PRL_METHOD( PrlVmCfg_GetDescription) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmDescription,
		PRL_UINT32_PTR pnVmDescriptionBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmDescription=%p, pnVmDescriptionBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmDescription,
		pnVmDescriptionBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmDescriptionBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetDescription(sVmDescription, pnVmDescriptionBufLength));
}


PRL_METHOD( PrlVmCfg_SetDescription) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmDescription
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewVmDescription=%s)",
		__FUNCTION__,
		hVmCfg,
		sNewVmDescription
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmDescription))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetDescription(sNewVmDescription));
}

PRL_METHOD( PrlVmCfg_IsTemplate ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmIsTemplate
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmIsTemplate=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmIsTemplate
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmIsTemplate))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsTemplate(pbVmIsTemplate));
}

PRL_METHOD( PrlVmCfg_SetTemplateSign ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmIsTemplate
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmIsTemplate=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmIsTemplate
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetTemplateSign(bVmIsTemplate));
}

PRL_METHOD( PrlVmCfg_GetCustomProperty) (
								   PRL_HANDLE hVmCfg,
								   PRL_STR sVmCustomProperty,
								   PRL_UINT32_PTR pnVmCustomPropertyBufLength
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmCustomProperty=%p, pnVmCustomPropertyBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmCustomProperty,
		pnVmCustomPropertyBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		PRL_WRONG_PTR(pnVmCustomPropertyBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetCustomProperty(sVmCustomProperty, pnVmCustomPropertyBufLength));
}

PRL_METHOD( PrlVmCfg_SetCustomProperty) (
								   PRL_HANDLE hVmCfg,
								   PRL_CONST_STR sNewVmCustomProperty
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewVmCustomProperty=%s)",
		__FUNCTION__,
		hVmCfg,
		sNewVmCustomProperty
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		PRL_WRONG_PTR(sNewVmCustomProperty))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCustomProperty(sNewVmCustomProperty));
}

PRL_METHOD( PrlVmCfg_GetAutoStart ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_AUTOSTART_OPTION_PTR pnVmAutoStart
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmAutoStart=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmAutoStart
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmAutoStart))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetAutoStart(pnVmAutoStart));
}

PRL_METHOD( PrlVmCfg_SetAutoStart ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_AUTOSTART_OPTION nVmAutoStart
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmAutoStart=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmAutoStart
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetAutoStart(nVmAutoStart));
}

PRL_METHOD( PrlVmCfg_GetAutoStartDelay ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmAutoStartDelay
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmAutoStartDelay=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmAutoStartDelay
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmAutoStartDelay))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetAutoStartDelay(pnVmAutoStartDelay));
}

PRL_METHOD( PrlVmCfg_SetAutoStartDelay ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmAutoStartDelay
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmAutoStartDelay=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmAutoStartDelay
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetAutoStartDelay(nVmAutoStartDelay));
}

PRL_METHOD( PrlVmCfg_GetStartLoginMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_START_LOGIN_MODE_PTR pnVmStartLoginMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmStartLoginMode=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmStartLoginMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmStartLoginMode))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetStartLoginMode(pnVmStartLoginMode));
}

PRL_METHOD( PrlVmCfg_SetStartLoginMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_START_LOGIN_MODE nVmStartLoginMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmStartLoginMode=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmStartLoginMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetStartLoginMode(nVmStartLoginMode));
}

PRL_METHOD( PrlVmCfg_GetStartUserLogin) (
		PRL_HANDLE hVmCfg,
		PRL_STR sStartUserLogin,
		PRL_UINT32_PTR pnStartUserLoginBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sStartUserLogin=%p, pnStartUserLoginBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sStartUserLogin,
		pnStartUserLoginBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnStartUserLoginBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetStartUserLogin(sStartUserLogin, pnStartUserLoginBufLength));
}


PRL_METHOD( PrlVmCfg_SetStartUserCreds) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sStartUserLogin,
		PRL_CONST_STR sPassword
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sStartUserLogin=%s, sPassword=%p)",
		__FUNCTION__,
		hVmCfg,
		sStartUserLogin,
		sPassword
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sStartUserLogin) || PRL_WRONG_PTR(sPassword))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetStartUserCreds(sStartUserLogin, sPassword));
}

PRL_METHOD( PrlVmCfg_GetAutoStop ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_AUTOSTOP_OPTION_PTR pnVmAutoStop
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmAutoStop=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmAutoStop
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmAutoStop))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetAutoStop(pnVmAutoStop));
}

PRL_METHOD( PrlVmCfg_SetAutoStop ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_AUTOSTOP_OPTION nVmAutoStop
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmAutoStop=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmAutoStop
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetAutoStop(nVmAutoStop));
}

PRL_METHOD( PrlVmCfg_GetActionOnWindowClose ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACTION_ON_WINDOW_CLOSE_PTR pnActionOnWindowClose
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnActionOnWindowClose=%p)",
		__FUNCTION__,
		hVmCfg,
		pnActionOnWindowClose
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnActionOnWindowClose))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetActionOnWindowClose(pnActionOnWindowClose));
}

PRL_METHOD( PrlVmCfg_SetActionOnWindowClose ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACTION_ON_WINDOW_CLOSE nActionOnWindowClose
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nActionOnWindowClose=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nActionOnWindowClose
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetActionOnWindowClose(nActionOnWindowClose));
}

PRL_METHOD( PrlVmCfg_GetActionOnStopMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACTION_ON_STOP_PTR pnMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnMode=%p)",
		__FUNCTION__,
		hVmCfg,
		pnMode
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnMode) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetActionOnStopMode(pnMode));
}

PRL_METHOD( PrlVmCfg_SetActionOnStopMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACTION_ON_STOP nMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nMode=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nMode
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetActionOnStopMode(nMode));
}

PRL_METHOD( PrlVmCfg_GetWindowMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_WINDOW_MODE_PTR pnVmWindowMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmWindowMode=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmWindowMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmWindowMode))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetWindowMode(pnVmWindowMode));
}

PRL_METHOD( PrlVmCfg_SetWindowMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_WINDOW_MODE nVmWindowMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmWindowMode=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmWindowMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetWindowMode(nVmWindowMode));
}

PRL_METHOD( PrlVmCfg_IsStartInDetachedWindowEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, pbEnable=%p)",
		__FUNCTION__,
		hVm,
		pbEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(pbEnable))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->IsStartInDetachedWindowEnabled(pbEnable);
}

PRL_METHOD( PrlVmCfg_SetStartInDetachedWindowEnabled ) (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, bEnable=%.8X)",
		__FUNCTION__,
		hVm,
		bEnable
	);

	if (PRL_WRONG_HANDLE(hVm, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVm );

	return pVm->SetStartInDetachedWindowEnabled(bEnable);
}

PRL_METHOD( PrlVmCfg_IsLockInFullScreenMode ) (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbValue
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbValue=%p)",
		__FUNCTION__,
		hVmCfg,
		pbValue
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(pbValue))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsLockInFullScreenMode(pbValue));
}

PRL_METHOD( PrlVmCfg_SetLockInFullScreenMode ) (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bValue
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bValue=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bValue
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetLockInFullScreenMode(bValue));
}

PRL_METHOD( PrlVmCfg_GetLastModifiedDate) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmLastModifiedDate,
		PRL_UINT32_PTR pnVmLastModifiedDateBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmLastModifiedDate=%p, pnVmLastModifiedDateBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmLastModifiedDate,
		pnVmLastModifiedDateBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmLastModifiedDateBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetLastModifiedDate(sVmLastModifiedDate, pnVmLastModifiedDateBufLength));
}


PRL_METHOD( PrlVmCfg_GetLastModifierName) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmLastModifierName,
		PRL_UINT32_PTR pnVmLastModifierNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmLastModifierName=%p, pnVmLastModifierNameBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmLastModifierName,
		pnVmLastModifierNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmLastModifierNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetLastModifierName(sVmLastModifierName, pnVmLastModifierNameBufLength));
}

PRL_METHOD( PrlVmCfg_GetUptimeStartDate) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmUptimeStartDate,
		PRL_UINT32_PTR pnVmUptimeStartDateBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmUptimeStartDate=%p, pnVmUptimeStartDateBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmUptimeStartDate,
		pnVmUptimeStartDateBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetUptimeStartDate(sVmUptimeStartDate, pnVmUptimeStartDateBufLength));
}


PRL_METHOD( PrlVmCfg_GetUptime) (
		PRL_HANDLE hVmCfg,
		PRL_UINT64_PTR pnVmUptime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmUptime=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmUptime
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		PRL_WRONG_PTR(pnVmUptime))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetUptime(pnVmUptime));
}


PRL_METHOD( PrlVmCfg_GetAccessRights ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phVmAcl
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phVmAcl=%p)",
		__FUNCTION__,
		hVmCfg,
		phVmAcl
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(phVmAcl))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetAccessRights(phVmAcl));
}


PRL_METHOD( PrlVmCfg_IsGuestSharingEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmGuestSharingEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmGuestSharingEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmGuestSharingEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmGuestSharingEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsGuestSharingEnabled(pbVmGuestSharingEnabled));
}

PRL_METHOD( PrlVmCfg_SetGuestSharingEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmGuestSharingEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmGuestSharingEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmGuestSharingEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetGuestSharingEnabled(bVmGuestSharingEnabled));
}

PRL_METHOD( PrlVmCfg_IsGuestSharingAutoMount ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmGuestSharingAutoMount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmGuestSharingAutoMount=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmGuestSharingAutoMount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmGuestSharingAutoMount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsGuestSharingAutoMount(pbVmGuestSharingAutoMount));
}

PRL_METHOD( PrlVmCfg_SetGuestSharingAutoMount ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmGuestSharingAutoMount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmGuestSharingAutoMount=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmGuestSharingAutoMount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetGuestSharingAutoMount(bVmGuestSharingAutoMount));
}

PRL_METHOD( PrlVmCfg_IsGuestSharingEnableSpotlight ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmGuestSharingEnableSpotlight
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmGuestSharingEnableSpotlight=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmGuestSharingEnableSpotlight
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmGuestSharingEnableSpotlight))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsGuestSharingEnableSpotlight(pbVmGuestSharingEnableSpotlight));
}

PRL_METHOD( PrlVmCfg_SetGuestSharingEnableSpotlight ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmGuestSharingEnableSpotlight
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmGuestSharingEnableSpotlight=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmGuestSharingEnableSpotlight
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetGuestSharingEnableSpotlight(bVmGuestSharingEnableSpotlight));
}

PRL_METHOD( PrlVmCfg_IsHostSharingEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmHostSharingEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmHostSharingEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmHostSharingEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmHostSharingEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsHostSharingEnabled(pbVmHostSharingEnabled));
}

PRL_METHOD( PrlVmCfg_SetHostSharingEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmHostSharingEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmHostSharingEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmHostSharingEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetHostSharingEnabled(bVmHostSharingEnabled));
}

PRL_METHOD( PrlVmCfg_IsShareAllHostDisks ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbShareAllHostDisks
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbShareAllHostDisks=%p)",
		__FUNCTION__,
		hVmCfg,
		pbShareAllHostDisks
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbShareAllHostDisks))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsShareAllHostDisks(pbShareAllHostDisks));
}

PRL_METHOD( PrlVmCfg_SetShareAllHostDisks ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bShareAllHostDisks
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bShareAllHostDisks=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bShareAllHostDisks
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetShareAllHostDisks(bShareAllHostDisks));
}

PRL_METHOD( PrlVmCfg_IsShareUserHomeDir ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbShareUserHomeDir
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbShareUserHomeDir=%p)",
		__FUNCTION__,
		hVmCfg,
		pbShareUserHomeDir
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbShareUserHomeDir))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsShareUserHomeDir(pbShareUserHomeDir));
}

PRL_METHOD( PrlVmCfg_SetShareUserHomeDir ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bShareUserHomeDir
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bShareUserHomeDir=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bShareUserHomeDir
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetShareUserHomeDir(bShareUserHomeDir));
}

PRL_METHOD( PrlVmCfg_IsMapSharedFoldersOnLetters ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbMapSharedFoldersOnLetters
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbMapSharedFoldersOnLetters=%p)",
		__FUNCTION__,
		hVmCfg,
		pbMapSharedFoldersOnLetters
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbMapSharedFoldersOnLetters))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsMapSharedFoldersOnLetters(pbMapSharedFoldersOnLetters));
}

PRL_METHOD( PrlVmCfg_SetMapSharedFoldersOnLetters ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bMapSharedFoldersOnLetters
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bMapSharedFoldersOnLetters=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bMapSharedFoldersOnLetters
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetMapSharedFoldersOnLetters(bMapSharedFoldersOnLetters));
}

PRL_METHOD( PrlVmCfg_GetVNCMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_REMOTE_DISPLAY_MODE_PTR pnVmRemoteDisplayMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmRemoteDisplayMode=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmRemoteDisplayMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmRemoteDisplayMode))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetRemoteDisplayMode(pnVmRemoteDisplayMode));
}

PRL_METHOD( PrlVmCfg_SetVNCMode ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_REMOTE_DISPLAY_MODE nVmRemoteDisplayMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmRemoteDisplayMode=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmRemoteDisplayMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetRemoteDisplayMode(nVmRemoteDisplayMode));
}

PRL_METHOD( PrlVmCfg_GetVNCPassword) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmRemoteDisplayPassword,
		PRL_UINT32_PTR pnVmRemoteDisplayPasswordBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmRemoteDisplayPassword=%p, pnVmRemoteDisplayPasswordBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmRemoteDisplayPassword,
		pnVmRemoteDisplayPasswordBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmRemoteDisplayPasswordBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetRemoteDisplayPassword(sVmRemoteDisplayPassword, pnVmRemoteDisplayPasswordBufLength));
}

PRL_METHOD( PrlVmCfg_SetVNCPassword) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmRemoteDisplayPassword
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewVmRemoteDisplayPassword=%p)",
		__FUNCTION__,
		hVmCfg,
		sNewVmRemoteDisplayPassword
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmRemoteDisplayPassword))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetRemoteDisplayPassword(sNewVmRemoteDisplayPassword));
}

PRL_METHOD( PrlVmCfg_GetVNCHostName) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmRemoteDisplayHostName,
		PRL_UINT32_PTR pnVmRemoteDisplayHostNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmRemoteDisplayHostName=%p, pnVmRemoteDisplayHostNameBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmRemoteDisplayHostName,
		pnVmRemoteDisplayHostNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmRemoteDisplayHostNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetRemoteDisplayHostName(sVmRemoteDisplayHostName, pnVmRemoteDisplayHostNameBufLength));
}

PRL_METHOD( PrlVmCfg_SetVNCHostName) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmRemoteDisplayHostName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewVmRemoteDisplayHostName=%s)",
		__FUNCTION__,
		hVmCfg,
		sNewVmRemoteDisplayHostName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmRemoteDisplayHostName))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetRemoteDisplayHostName(sNewVmRemoteDisplayHostName));
}

PRL_METHOD( PrlVmCfg_GetVNCPort ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmRemoteDisplayPortNumber
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmRemoteDisplayPortNumber=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmRemoteDisplayPortNumber
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmRemoteDisplayPortNumber))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetRemoteDisplayPortNumber(pnVmRemoteDisplayPortNumber));
}

PRL_METHOD( PrlVmCfg_SetVNCPort ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmRemoteDisplayPortNumber
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmRemoteDisplayPortNumber=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmRemoteDisplayPortNumber
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetRemoteDisplayPortNumber(nVmRemoteDisplayPortNumber));
}

PRL_METHOD( PrlVmCfg_IsScrResEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmScrResEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmScrResEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmScrResEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmScrResEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsScrResEnabled(pbVmScrResEnabled));
}

PRL_METHOD( PrlVmCfg_SetScrResEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmScrResEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmScrResEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmScrResEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetScrResEnabled(bVmScrResEnabled));
}

PRL_METHOD( PrlVmCfg_IsDiskCacheWriteBack ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmDiskCacheWriteBack
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmDiskCacheWriteBack=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmDiskCacheWriteBack
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmDiskCacheWriteBack))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsDiskCacheWriteBack(pbVmDiskCacheWriteBack));
}

PRL_METHOD( PrlVmCfg_SetDiskCacheWriteBack ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmDiskCacheWriteBack
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmDiskCacheWriteBack=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmDiskCacheWriteBack
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetDiskCacheWriteBack(bVmDiskCacheWriteBack));
}

PRL_METHOD( PrlVmCfg_IsOsResInFullScrMode ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmOsResInFullScrMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmOsResInFullScrMode=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmOsResInFullScrMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmOsResInFullScrMode))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsOsResInFullScrMode(pbVmOsResInFullScrMode));
}

PRL_METHOD( PrlVmCfg_SetOsResInFullScrMode ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmOsResInFullScrMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmOsResInFullScrMode=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmOsResInFullScrMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetOsResInFullScrMode(bVmOsResInFullScrMode));
}

PRL_METHOD( PrlVmCfg_IsCloseAppOnShutdown ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmCloseAppOnShutdown
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmCloseAppOnShutdown=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmCloseAppOnShutdown
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbVmCloseAppOnShutdown))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsCloseAppOnShutdown(pbVmCloseAppOnShutdown));
}

PRL_METHOD( PrlVmCfg_SetCloseAppOnShutdown ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmCloseAppOnShutdown
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmCloseAppOnShutdown=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmCloseAppOnShutdown
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCloseAppOnShutdown(bVmCloseAppOnShutdown));
}

PRL_METHOD( PrlVmCfg_GetSystemFlags) (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmSystemFlags,
		PRL_UINT32_PTR pnVmSystemFlagsBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sVmSystemFlags=%p, pnVmSystemFlagsBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sVmSystemFlags,
		pnVmSystemFlagsBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmSystemFlagsBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetSystemFlags(sVmSystemFlags, pnVmSystemFlagsBufLength));
}


PRL_METHOD( PrlVmCfg_SetSystemFlags) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmSystemFlags
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewVmSystemFlags=%s)",
		__FUNCTION__,
		hVmCfg,
		sNewVmSystemFlags
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewVmSystemFlags))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetSystemFlags(sNewVmSystemFlags));
}

PRL_METHOD( PrlVmCfg_IsDisableAPIC ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbDisableAPIC
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbDisableAPIC=%p)",
		__FUNCTION__,
		hVmCfg,
		pbDisableAPIC
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbDisableAPIC))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsDisableAPIC(pbDisableAPIC));
}

PRL_METHOD( PrlVmCfg_SetDisableAPICSign ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bDisableAPIC
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bDisableAPIC=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bDisableAPIC
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetDisableAPICSign(bDisableAPIC));
}

PRL_METHOD( PrlVmCfg_IsDisableSpeaker ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbDisableSpeaker
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbDisableSpeaker=%p)",
		__FUNCTION__,
		hVmCfg,
		pbDisableSpeaker
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbDisableSpeaker))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsDisableSpeaker(pbDisableSpeaker));
}

PRL_METHOD( PrlVmCfg_SetDisableSpeakerSign ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bDisableSpeaker
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bDisableSpeaker=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bDisableSpeaker
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetDisableSpeakerSign(bDisableSpeaker));
}

PRL_METHOD( PrlVmCfg_GetUndoDisksMode ) (
		PRL_HANDLE hVmCfg,
		PRL_UNDO_DISKS_MODE_PTR pnUndoDisksMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnUndoDisksMode=%p)",
		__FUNCTION__,
		hVmCfg,
		pnUndoDisksMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnUndoDisksMode))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetUndoDisksMode(pnUndoDisksMode));
}

PRL_METHOD( PrlVmCfg_SetUndoDisksMode ) (
		PRL_HANDLE hVmCfg,
		PRL_UNDO_DISKS_MODE nUndoDisksMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nUndoDisksMode=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nUndoDisksMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetUndoDisksMode(nUndoDisksMode));
}

PRL_METHOD( PrlVmCfg_GetForegroundPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_PRIORITY_PTR pnVmForegroundPriority
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmForegroundPriority=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmForegroundPriority
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmForegroundPriority))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetForegroundPriority(pnVmForegroundPriority));
}

PRL_METHOD( PrlVmCfg_SetForegroundPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_PRIORITY nVmForegroundPriority
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmForegroundPriority=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmForegroundPriority
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetForegroundPriority(nVmForegroundPriority));
}

PRL_METHOD( PrlVmCfg_GetBackgroundPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_PRIORITY_PTR pnVmBackgroundPriority
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmBackgroundPriority=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmBackgroundPriority
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnVmBackgroundPriority))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetBackgroundPriority(pnVmBackgroundPriority));
}

PRL_METHOD( PrlVmCfg_SetBackgroundPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_PRIORITY nVmBackgroundPriority
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmBackgroundPriority=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nVmBackgroundPriority
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetBackgroundPriority(nVmBackgroundPriority));
}

PRL_METHOD( PrlVmCfg_IsUseDefaultAnswers ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbUseDefaultAnswers
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbUseDefaultAnswers=%p)",
		__FUNCTION__,
		hVmCfg,
		pbUseDefaultAnswers
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbUseDefaultAnswers) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsUseDefaultAnswers(pbUseDefaultAnswers));
}

PRL_METHOD( PrlVmCfg_SetUseDefaultAnswers ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bUseDefaultAnswers
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bUseDefaultAnswers=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bUseDefaultAnswers
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetUseDefaultAnswers(bUseDefaultAnswers));
}

PRL_METHOD( PrlVmCfg_GetSearchDomains ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phSearchDomainsList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phSearchDomainsList=%p)",
		__FUNCTION__,
		hVmCfg,
		phSearchDomainsList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phSearchDomainsList))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetSearchDomains(phSearchDomainsList));
}

PRL_METHOD( PrlVmCfg_SetSearchDomains ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hSearchDomainsList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, hSearchDomainsList=%p)",
		__FUNCTION__,
		hVmCfg,
		hSearchDomainsList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		(PRL_INVALID_HANDLE != hSearchDomainsList && PRL_WRONG_HANDLE(hSearchDomainsList, PHT_STRINGS_LIST)))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	PrlHandleStringsListPtr pSearchDomainsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hSearchDomainsList);
	return (pVm->SetSearchDomains(pSearchDomainsList.getHandle() ? pSearchDomainsList->GetStringsList() : QStringList()));
}

PRL_METHOD( PrlVmCfg_GetDnsServers ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phDnsServersList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phDnsServersList=%p)",
		__FUNCTION__,
		hVmCfg,
		phDnsServersList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phDnsServersList))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetDnsServers(phDnsServersList));
}

PRL_METHOD( PrlVmCfg_SetDnsServers ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hDnsServersList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, hDnsServersList=%p)",
		__FUNCTION__,
		hVmCfg,
		hDnsServersList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		(PRL_INVALID_HANDLE != hDnsServersList && PRL_WRONG_HANDLE(hDnsServersList, PHT_STRINGS_LIST)))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	PrlHandleStringsListPtr pDnsServersList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hDnsServersList);
	return (pVm->SetDnsServers(pDnsServersList.getHandle() ? pDnsServersList->GetStringsList() : QStringList()));
}

PRL_METHOD( PrlVmCfg_IsAutoApplyIpOnly ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbAutoApplyIpOnly
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbAutoApplyIpOnly))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsAutoApplyIpOnly(pbAutoApplyIpOnly));
}

PRL_METHOD( PrlVmCfg_SetAutoApplyIpOnly ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bAutoApplyIpOnly
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetAutoApplyIpOnly(bAutoApplyIpOnly));
}


PRL_METHOD( PrlVmCfg_GetVmInfo ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phVmInfo
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phVmInfo=%p)",
		__FUNCTION__,
		hVmCfg,
		phVmInfo
		);

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phVmInfo) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);

	return (pVm->GetVmInfo(phVmInfo));
}

PRL_METHOD( PrlVmCfg_IsEncrypted ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEncrypted
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEncrypted=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEncrypted
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEncrypted) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsEncrypted(pbEncrypted));
}

PRL_METHOD( PrlVmCfg_GetCapabilitiesMask ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnCapMask
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnCapMask=%p)",
		__FUNCTION__,
		hVmCfg,
		pnCapMask
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnCapMask))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetCapabilitiesMask(pnCapMask);
}

PRL_METHOD( PrlVmCfg_SetCapabilitiesMask ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nCapMask
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nCapMask=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nCapMask
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetCapabilitiesMask(nCapMask);
}

PRL_METHOD( PrlVmCfg_GetNetfilterMode ) (
		PRL_HANDLE hVmCfg,
		PRL_NETFILTER_MODE_PTR pnMode
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnMode=%p)", __FUNCTION__, hVmCfg, pnMode);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(pnMode))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetNetfilterMode(pnMode);
}

PRL_METHOD( PrlVmCfg_SetNetfilterMode ) (
		PRL_HANDLE hVmCfg,
		PRL_NETFILTER_MODE nMode
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nMode=%.8X)", __FUNCTION__, hVmCfg, nMode);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetNetfilterMode(nMode);
}

PRL_METHOD( PrlVmCfg_GetFeaturesMask ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnOn,
		PRL_UINT32_PTR pnOff
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnOn) || PRL_WRONG_PTR(pnOff))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetFeaturesMask(pnOn, pnOff);
}

PRL_METHOD( PrlVmCfg_SetFeaturesMask ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nOn,
		PRL_UINT32 nOff
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetFeaturesMask(nOn, nOff);
}


PRL_METHOD( PrlVmCfg_GetProfile ) (
		PRL_HANDLE hVmCfg,
		PRL_VIRTUAL_MACHINE_PROFILE_PTR pnVmProfile
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmProfile=%p)",
		__FUNCTION__,
		hVmCfg,
		pnVmProfile
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		 PRL_WRONG_PTR(pnVmProfile) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetProfile(pnVmProfile));
}


PRL_METHOD( PrlVmCfg_IsCustomProfile ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbCustom
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbCustom=%p)",
		__FUNCTION__,
		hVmCfg,
		pbCustom
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		 PRL_WRONG_PTR(pbCustom) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsCustomProfile(pbCustom));
}


PRL_METHOD( PrlVmCfg_SetProfile ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hSrvConfig,
		PRL_VIRTUAL_MACHINE_PROFILE nVmProfile
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmProfile=%d)",
		__FUNCTION__,
		hVmCfg,
		nVmProfile
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION)
		 || (hSrvConfig != PRL_INVALID_HANDLE && PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG)))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>(hSrvConfig);

	return (pVm->SetProfile(pSrvConfig, nVmProfile));
}


PRL_METHOD( PrlVmCfg_IsHighAvailabilityEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsHighAvailabilityEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetHighAvailabilityEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetHighAvailabilityEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_GetHighAvailabilityPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnPriority
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnPriority=%p)",
		__FUNCTION__,
		hVmCfg,
		pnPriority
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnPriority) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetHighAvailabilityPriority(pnPriority));
}

PRL_METHOD( PrlVmCfg_SetHighAvailabilityPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nPriority
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nPriority=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nPriority
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetHighAvailabilityPriority(nPriority));
}

PRL_METHOD( PrlVmCfg_SetNetworkRateList ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hNetworkRateList
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);

	return (pVm->SetNetworkRates(hNetworkRateList));
}

PRL_METHOD( PrlVmCfg_GetNetworkRateList ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phNetworkRateList
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);

	return (pVm->GetNetworkRates(phNetworkRateList));
}

PRL_METHOD( PrlVmCfg_IsRateBound ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsRateBound(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetRateBound ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetRateBound(bEnabled));
}

PRL_HANDLE PrlSrv_InstallAppliance_Impl(PRL_HANDLE hServer,
												 PRL_HANDLE hAppCfg,
												 PRL_CONST_STR sVmParentPath,
												 PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hAppCfg, PHT_APPLIANCE_CONFIG))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_INSTALL_APPLIANCE));

	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	PrlHandleJobPtr pJob = pServer->InstallAppliance(hAppCfg, sVmParentPath, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVm_LoginInGuest_Impl(PRL_HANDLE hVm, PRL_CONST_STR sUserName,
										PRL_CONST_STR sUserPassword, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sUserName) ||
		 UTF8_2QSTR(sUserName).isEmpty() )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_LOGIN_IN_GUEST))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->LoginInGuest(sUserName, sUserPassword, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVm_AuthWithGuestSecurityDb_Impl(PRL_HANDLE hVm, PRL_CONST_STR sUserName,
										PRL_CONST_STR sUserPassword, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sUserName) ||
		 UTF8_2QSTR(sUserName).isEmpty() )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_AUTH_WITH_GUEST_SECURITY_DB))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->AuthWithGuestSecurityDb(sUserName, sUserPassword, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVmGuest_Logout_Impl(PRL_HANDLE hVmGuest, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVmGuest, PHT_VM_GUEST_SESSION) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_GUEST_LOGOUT))
	PrlHandleVmGuestPtr pVmGuest = PRL_OBJECT_BY_HANDLE<PrlHandleVmGuest>(hVmGuest);
	PrlHandleJobPtr pJob = pVmGuest->Logout(nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(pVmGuest->GetVmHandle());
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVmGuest_GetNetworkSettings_Impl(PRL_HANDLE hVmGuest, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVmGuest, PHT_VM_GUEST_SESSION) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_GUEST_GET_NETWORK_SETTINGS))
	PrlHandleVmGuestPtr pVmGuest = PRL_OBJECT_BY_HANDLE<PrlHandleVmGuest>(hVmGuest);
	PrlHandleJobPtr pJob = pVmGuest->GetNetworkSettings(nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(pVmGuest->GetVmHandle());
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVmGuest_RunProgram_Impl(PRL_HANDLE hVmGuest, PRL_CONST_STR sProgramName,
							PRL_HANDLE hArgsList, PRL_HANDLE hEnvsList, PRL_UINT32 nFlags,
							PRL_FILE_DESC nStdin, PRL_FILE_DESC nStdout, PRL_FILE_DESC nStderr)
{
	if ( PRL_WRONG_HANDLE(hVmGuest, PHT_VM_GUEST_SESSION) || PRL_WRONG_PTR(sProgramName) ||
			UTF8_2QSTR(sProgramName).isEmpty() ||
			(PRL_INVALID_HANDLE != hArgsList && PRL_WRONG_HANDLE(hArgsList, PHT_STRINGS_LIST)) ||
			(PRL_INVALID_HANDLE != hEnvsList && PRL_WRONG_HANDLE(hEnvsList, PHT_STRINGS_LIST)) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_GUEST_RUN_PROGRAM))
	PrlHandleVmGuestPtr pVmGuest = PRL_OBJECT_BY_HANDLE<PrlHandleVmGuest>(hVmGuest);
	PrlHandleJobPtr pJob = pVmGuest->RunProgram(sProgramName, hArgsList, hEnvsList, nFlags, nStdin, nStdout, nStderr);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(pVmGuest->GetVmHandle());
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVmGuest_SetUserPasswd_Impl(PRL_HANDLE hVmGuest, PRL_CONST_STR sUserName,
							PRL_CONST_STR sUserPasswd, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVmGuest, PHT_VM_GUEST_SESSION) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_GUEST_SET_USER_PASSWD))
	PrlHandleVmGuestPtr pVmGuest = PRL_OBJECT_BY_HANDLE<PrlHandleVmGuest>(hVmGuest);
	PrlHandleJobPtr pJob = pVmGuest->SetUserPasswd(sUserName, sUserPasswd, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(pVmGuest->GetVmHandle());
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVm_MigrateCancel_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(MigrateCancel, PJOC_VM_MIGRATE_CANCEL)
}

PRL_HANDLE PrlVm_DropSuspendedState_Impl(PRL_HANDLE hVm)
{
	ONE_HANDLE_VM_METH_IMPLEMENTATION(DropSuspendedState, PJOC_VM_DROP_SUSPENDED_STATE)
}

PRL_HANDLE PrlVm_Compact_Impl(PRL_HANDLE hVm, PRL_UINT32 uMask, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_COMPACT))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->Compact(uMask, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVm_CancelCompact_Impl(PRL_HANDLE hVm)
{

	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_CANCEL_COMPACT))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->CancelCompact();
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVm_ConvertDisks_Impl(PRL_HANDLE hVm, PRL_UINT32 uMask, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_CONVERT_DISKS))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->ConvertDisks(uMask, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVmDev_Connect_Impl(PRL_HANDLE hVmDevice)
{
	ONE_HANDLE_VM_DEV_METH_IMPLEMENTATION(Connect, PJOC_VM_DEV_CONNECT)
}

PRL_HANDLE PrlVmDev_Disconnect_Impl(PRL_HANDLE hVmDevice)
{
	ONE_HANDLE_VM_DEV_METH_IMPLEMENTATION(Disconnect, PJOC_VM_DEV_DISCONNECT)
}

PRL_HANDLE PrlVmDevHd_CheckPassword_Impl(PRL_HANDLE hVmDev, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_DEV_HD_CHECK_PASSWORD))
	PrlHandleVmDeviceHardDrivePtr pHdd = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	PrlHandleJobPtr pJob = pHdd->CheckPassword( nFlags );
	if ( !pJob )
		RETURN_RES((PRL_INVALID_HANDLE))
	RETURN_RES(pJob->GetHandle())
}

PRL_HANDLE PrlVmDev_CreateImage_Impl(PRL_HANDLE hVmDevice, PRL_BOOL bRecreateIsAllowed, PRL_BOOL bNonInteractiveMode)
{
	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDevice))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_DEV_CREATE_IMAGE))

		PrlHandleVmDevicePtr pVmDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDevice);
	if (!pVmDevice->GetVmPtr())
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_UNINITIALIZED, PJOC_VM_DEV_CREATE_IMAGE))
		PrlHandleJobPtr pJob = pVmDevice->GetVmSrvPtr()->CreateImage(pVmDevice, bRecreateIsAllowed, bNonInteractiveMode );
	if (!pJob)
		RETURN_RES((PRL_INVALID_HANDLE))

	PRL_HANDLE hVm;
	if (PRL_SUCCEEDED(pVmDevice->GetVm(&hVm)))
		pJob->SetVmHandle(hVm);
	RETURN_RES((pJob->GetHandle()))
}

PRL_HANDLE PrlVmDev_CopyImage_Impl(PRL_HANDLE hVmDevice,
											PRL_CONST_STR sNewImageName,
											PRL_CONST_STR sTargetPath,
											PRL_UINT32 nFlags)
{
	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDevice) || PRL_WRONG_PTR(sTargetPath))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_DEV_COPY_IMAGE));

	PrlHandleVmDevicePtr pVmDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDevice);
	if (!pVmDevice->GetVmPtr())
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_UNINITIALIZED, PJOC_VM_DEV_COPY_IMAGE));

	PrlHandleJobPtr pJob = pVmDevice->GetVmSrvPtr()->CopyImage(
							pVmDevice, sNewImageName, sTargetPath, nFlags );
	if (!pJob)
		RETURN_RES((PRL_INVALID_HANDLE));

	PRL_HANDLE hVm;
	if (PRL_SUCCEEDED(pVmDevice->GetVm(&hVm)))
		pJob->SetVmHandle(hVm);

	RETURN_RES((pJob->GetHandle()));
}

PRL_HANDLE PrlVmDev_ResizeImage_Impl(PRL_HANDLE hVmDevice, PRL_UINT32 nNewSize, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDevice))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_RESIZE_DISK_IMAGE))

	PrlHandleVmDevicePtr pVmDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDevice);
	if (!pVmDevice->GetVmPtr())
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_UNINITIALIZED, PJOC_VM_RESIZE_DISK_IMAGE))

	CVmDevice *pVmDev = pVmDevice->GetVmDeviceElem();
	if (!pVmDev)
		 RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_UNINITIALIZED, PJOC_VM_RESIZE_DISK_IMAGE))

	PrlHandleJobPtr pJob = pVmDevice->GetVmSrvPtr()->ResizeDiskImage(pVmDev->getUserFriendlyName(), nNewSize, nFlags );
	if (!pJob)
		RETURN_RES((PRL_INVALID_HANDLE))

	PRL_HANDLE hVm;
	if (PRL_SUCCEEDED(pVmDevice->GetVm(&hVm)))
		pJob->SetVmHandle(hVm);

	RETURN_RES((pJob->GetHandle()))
}

PRL_HANDLE PrlVm_SubscribeToPerfStats_Impl(PRL_HANDLE handle, PRL_CONST_STR sFilter)
{
	PRL_CntxSw_CHECK_HANDLE_ASYNC(handle, PHT_VIRTUAL_MACHINE, PJOC_VM_SUBSCRIBE_PERFSTATS) ;
	return store_result(PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(handle)->SubscribePerfStats(sFilter)) ;
}

PRL_HANDLE PrlVm_UnsubscribeFromPerfStats_Impl(PRL_HANDLE handle)
{
	PRL_CntxSw_CHECK_HANDLE_ASYNC(handle, PHT_VIRTUAL_MACHINE, PJOC_VM_UNSUBSCRIBE_PERFSTATS) ;
	return store_result(PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(handle)->UnsubscribePerfStats()) ;
}

PRL_HANDLE PrlVm_GetPerfStats_Impl(PRL_HANDLE handle, PRL_CONST_STR sFilter)
{
	PRL_CntxSw_CHECK_HANDLE_ASYNC(handle, PHT_VIRTUAL_MACHINE, PJOC_VM_GET_PERFSTATS) ;
	return store_result(PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(handle)->GetPerfStats(sFilter), handle) ;
}

PRL_METHOD( PrlApi_GetRecommendMinVmMem ) (
		PRL_UINT32 nOsVersion,
		PRL_UINT32_PTR pnMinMemSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (nOsVersion=%.8X, pnMinMemSize=%p",
		__FUNCTION__,
		nOsVersion,
		pnMinMemSize
		);

	if ( PRL_WRONG_PTR(pnMinMemSize) )
		return PRL_ERR_INVALID_ARG;

	*pnMinMemSize = osInfo_getMinRecommendedOsMemorySize(nOsVersion);

	return PRL_ERR_SUCCESS;
}

PRL_ASYNC_METHOD( PrlSrv_StartSearchVms ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hStringsList
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_START_SEARCH_VMS)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_StartSearchVms, (hServer, hStringsList))
}

PRL_METHOD( PrlSrv_GetSupportedOses ) (
		PRL_HANDLE hServer,
		PRL_HANDLE_PTR phOsesMatrix
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%p, phOsesMatrix=%p)",
		__FUNCTION__,
		hServer,
		phOsesMatrix
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(phOsesMatrix) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	return (pServer->GetSupportedOses(phOsesMatrix));
}

PRL_ASYNC_METHOD( PrlSrv_InstallAppliance ) (
								   PRL_HANDLE hServer,
								   PRL_HANDLE hAppCfg,
								   PRL_CONST_STR sVmParentPath,
								   PRL_UINT32 nFlags
								   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_INSTALL_APPLIANCE)

	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%p, hAppCfg=%p, sVmParentPath=%s, nFlags=%.8X)",
	__FUNCTION__,
	hServer,
	hAppCfg,
	sVmParentPath,
	nFlags
	);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), \
								PrlSrv_InstallAppliance, (hServer, hAppCfg, sVmParentPath, nFlags))
}

PRL_ASYNC_METHOD( PrlSrv_CancelInstallAppliance ) (
								   PRL_HANDLE hServer,
								   PRL_HANDLE hAppCfg,
								   PRL_UINT32 nFlags
								   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_INSTALL_APPLIANCE)

	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%p, hAppCfg=%p, nFlags=%.8X)",
	__FUNCTION__,
	hServer,
	hAppCfg,
	nFlags
	);

	PRL_CONST_STR buf = "";
	nFlags = (nFlags | PIAF_CANCEL);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), \
								PrlSrv_InstallAppliance, (hServer, hAppCfg, buf, nFlags))
}

PRL_ASYNC_METHOD( PrlSrv_StopInstallAppliance ) (
								   PRL_HANDLE hServer,
								   PRL_HANDLE hAppCfg,
								   PRL_UINT32 nFlags
								   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_INSTALL_APPLIANCE)

	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%p, hAppCfg=%p, nFlags=%.8X)",
	__FUNCTION__,
	hServer,
	hAppCfg,
	nFlags
	);

	PRL_CONST_STR buf = "";
	nFlags = (nFlags | PIAF_STOP);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), \
								PrlSrv_InstallAppliance, (hServer, hAppCfg, buf, nFlags))
}

PRL_ASYNC_METHOD( PrlVm_DropSuspendedState ) (
								  PRL_HANDLE hVm
								  )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_DROP_SUSPENDED_STATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_DropSuspendedState, (hVm))
}

PRL_HANDLE PrlVm_Migrate_Impl(PRL_HANDLE hVm, PRL_HANDLE hTargetServer,
									   PRL_CONST_STR target_name, PRL_CONST_STR target_home_path,
									   PRL_UINT32 migration_flags, PRL_UINT32 reserved_flags,
									   PRL_BOOL force_operation)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_HANDLE(hTargetServer, PHT_SERVER)
		|| PRL_WRONG_PTR(target_name) || PRL_WRONG_PTR(target_home_path)	)
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_MIGRATE))
		PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleServerPtr pTargetServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>(hTargetServer);
	PrlHandleJobPtr pJob = pVm->Migrate(pTargetServer, target_name, target_home_path, migration_flags, reserved_flags,
		force_operation);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_Migrate) (
		PRL_HANDLE hVm,
		PRL_HANDLE hTargetServer,
		PRL_CONST_STR target_home_path,
		PRL_UINT32 migration_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		)
{
	PRL_CONST_STR target_name = "";
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_MIGRATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Migrate, (hVm, hTargetServer, target_name, target_home_path, migration_flags, \
							reserved_flags, force_operation))
}

PRL_HANDLE PrlVm_MigrateEx_Impl(PRL_HANDLE hVm,
										 PRL_CONST_STR target_host,
										 PRL_UINT32 target_port,
										 PRL_CONST_STR target_session_id,
										 PRL_CONST_STR target_name,
										 PRL_CONST_STR target_home_path,
										 PRL_UINT32 migration_flags,
										 PRL_UINT32 reserved_flags,
										 PRL_BOOL force_operation)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(target_host) ||
		PRL_WRONG_PTR(target_session_id) || PRL_WRONG_PTR(target_name) || PRL_WRONG_PTR(target_home_path))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_MIGRATE))
		PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->Migrate(target_host, target_port, target_session_id,
		target_name, target_home_path,
		migration_flags, reserved_flags, force_operation);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_MigrateEx ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR target_host,
		PRL_UINT32 target_port,
		PRL_CONST_STR target_session_id,
		PRL_CONST_STR target_home_path,
		PRL_UINT32 migration_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		)
{
	PRL_CONST_STR target_name = "";
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_MIGRATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_MigrateEx, (hVm, target_host, target_port, target_session_id,
							target_name, target_home_path, migration_flags, reserved_flags, force_operation))
}

PRL_ASYNC_METHOD(PrlVm_MigrateWithRename) (
		PRL_HANDLE hVm,
		PRL_HANDLE hTargetServer,
		PRL_CONST_STR target_name,
		PRL_CONST_STR target_home_path,
		PRL_UINT32 migration_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_MIGRATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Migrate, (hVm, hTargetServer, target_name, target_home_path, migration_flags, \
							reserved_flags, force_operation))
}

PRL_ASYNC_METHOD( PrlVm_MigrateWithRenameEx ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR target_host,
		PRL_UINT32 target_port,
		PRL_CONST_STR target_session_id,
		PRL_CONST_STR target_name,
		PRL_CONST_STR target_home_path,
		PRL_UINT32 migration_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_MIGRATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_MigrateEx, (hVm, target_host, target_port, target_session_id,
							target_name, target_home_path, migration_flags, reserved_flags, force_operation))
}

PRL_ASYNC_METHOD( PrlVm_MigrateCancel) (
		PRL_HANDLE hVm
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_MIGRATE_CANCEL)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_MigrateCancel, (hVm))
}

PRL_HANDLE PrlSrv_CreateVmBackup_Impl(
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_CONST_STR strDescription,
		PRL_UINT32 backup_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation)
{
	if (PRL_WRONG_HANDLE(hSourceServer, PHT_SERVER) || PRL_WRONG_PTR(sVmUuid) ||
			PRL_WRONG_PTR(sTargetHost) || PRL_WRONG_PTR(sTargetSessionId))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_CREATE_VM_BACKUP))
	PrlHandleServerVmPtr pSourceServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>(hSourceServer);
	PrlHandleJobPtr pJob = pSourceServer->DspCmdCreateVmBackup(
			sVmUuid,
			sTargetHost,
			nTargetPort,
			sTargetSessionId,
			strDescription,
			backup_flags,
			reserved_flags,
			force_operation);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_CreateVmBackup ) (
	PRL_HANDLE hSourceServer,
	PRL_CONST_STR sVmUuid,
	PRL_CONST_STR sTargetHost,
	PRL_UINT32 nTargetPort,
	PRL_CONST_STR sTargetSessionId,
	PRL_CONST_STR sDescription,
	PRL_UINT32 backup_flags,
	PRL_UINT32 reserved_flags,
	PRL_BOOL force_operation
)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_CREATE_VM_BACKUP)
	CALL_THROUGH_CTXT_SWITCHER(
		PrlContextSwitcher::Instance(),
		PrlSrv_CreateVmBackup,
		(hSourceServer, sVmUuid, sTargetHost, nTargetPort, sTargetSessionId, sDescription,
			backup_flags, reserved_flags, force_operation))
}

PRL_HANDLE PrlSrv_RestoreVmBackup_Impl(
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sBackupUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_CONST_STR sTargetVmHomePath,
		PRL_CONST_STR sTargetVmName,
		PRL_UINT32 restore_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation)
{
	PRL_UNUSED_PARAM(force_operation);
	if (PRL_WRONG_HANDLE(hSourceServer, PHT_SERVER) || PRL_WRONG_PTR(sVmUuid) || PRL_WRONG_PTR(sBackupUuid) ||
			PRL_WRONG_PTR(sTargetHost) || PRL_WRONG_PTR(sTargetSessionId))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_RESTORE_VM_BACKUP))
	PrlHandleServerVmPtr pSourceServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>(hSourceServer);
	PrlHandleJobPtr pJob = pSourceServer->DspCmdRestoreVmBackup(
			sVmUuid,
			sBackupUuid,
			sTargetHost,
			nTargetPort,
			sTargetSessionId,
			sTargetVmHomePath,
			sTargetVmName,
			restore_flags,
			reserved_flags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_RestoreVmBackup ) (
	PRL_HANDLE hSourceServer,
	PRL_CONST_STR sVmUuid,
	PRL_CONST_STR sBackupUuid,
	PRL_CONST_STR sTargetHost,
	PRL_UINT32 nTargetPort,
	PRL_CONST_STR sTargetSessionId,
	PRL_CONST_STR sTargetVmHomePath,
	PRL_CONST_STR sTargetVmName,
	PRL_UINT32 restore_flags,
	PRL_UINT32 reserved_flags,
	PRL_BOOL force_operation
)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_RESTORE_VM_BACKUP)
	CALL_THROUGH_CTXT_SWITCHER(
		PrlContextSwitcher::Instance(),
		PrlSrv_RestoreVmBackup,
		(hSourceServer, sVmUuid, sBackupUuid, sTargetHost, nTargetPort, sTargetSessionId,
			sTargetVmHomePath, sTargetVmName, restore_flags, reserved_flags, force_operation))
}

PRL_HANDLE PrlSrv_GetBackupTree_Impl(
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_UINT32 backup_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation)
{
	if (PRL_WRONG_HANDLE(hSourceServer, PHT_SERVER) || PRL_WRONG_PTR(sUuid) ||
			PRL_WRONG_PTR(sTargetHost) || PRL_WRONG_PTR(sTargetSessionId))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_GET_BACKUP_TREE))
	PrlHandleServerVmPtr pSourceServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>(hSourceServer);
	PrlHandleJobPtr pJob = pSourceServer->DspCmdGetBackupTree(
			sUuid,
			sTargetHost,
			nTargetPort,
			sTargetSessionId,
			backup_flags,
			reserved_flags,
			force_operation);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_GetBackupTree ) (
	PRL_HANDLE hSourceServer,
	PRL_CONST_STR sUuid,
	PRL_CONST_STR sTargetHost,
	PRL_UINT32 nTargetPort,
	PRL_CONST_STR sTargetSessionId,
	PRL_UINT32 backup_flags,
	PRL_UINT32 reserved_flags,
	PRL_BOOL force_operation
)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_BACKUP_TREE)
	CALL_THROUGH_CTXT_SWITCHER(
		PrlContextSwitcher::Instance(),
		PrlSrv_GetBackupTree,
		(hSourceServer, sUuid, sTargetHost, nTargetPort, sTargetSessionId,
			backup_flags, reserved_flags, force_operation))
}

PRL_HANDLE PrlSrv_RemoveVmBackup_Impl(
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sBackupUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_UINT32 remove_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation)
{
	if (PRL_WRONG_HANDLE(hSourceServer, PHT_SERVER) || PRL_WRONG_PTR(sVmUuid) || PRL_WRONG_PTR(sBackupUuid) ||
			PRL_WRONG_PTR(sTargetHost) || PRL_WRONG_PTR(sTargetSessionId))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_REMOVE_VM_BACKUP))
	PrlHandleServerVmPtr pSourceServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>(hSourceServer);
	PrlHandleJobPtr pJob = pSourceServer->DspCmdRemoveVmBackup(
			sVmUuid,
			sBackupUuid,
			sTargetHost,
			nTargetPort,
			sTargetSessionId,
			remove_flags,
			reserved_flags,
			force_operation);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_RemoveVmBackup ) (
	PRL_HANDLE hSourceServer,
	PRL_CONST_STR sVmUuid,
	PRL_CONST_STR sBackupUuid,
	PRL_CONST_STR sTargetHost,
	PRL_UINT32 nTargetPort,
	PRL_CONST_STR sTargetSessionId,
	PRL_UINT32 restore_flags,
	PRL_UINT32 reserved_flags,
	PRL_BOOL force_operation
)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_REMOVE_VM_BACKUP)
	CALL_THROUGH_CTXT_SWITCHER(
		PrlContextSwitcher::Instance(),
		PrlSrv_RemoveVmBackup,
		(hSourceServer, sVmUuid, sBackupUuid, sTargetHost, nTargetPort, sTargetSessionId,
			restore_flags, reserved_flags, force_operation))
}

PRL_ASYNC_METHOD( PrlVm_LoginInGuest ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR sUserName,
		PRL_CONST_STR sUserPassword,
		PRL_UINT32 nFlags
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, sUserName=%s, sUserPassword=%p, nFlags=%.8X)",
		__FUNCTION__,
		hVm,
		sUserName,
		sUserPassword,
		nFlags
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_LOGIN_IN_GUEST)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_LoginInGuest, (hVm, sUserName, sUserPassword, nFlags))
}

PRL_ASYNC_METHOD( PrlVm_AuthWithGuestSecurityDb ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR sUserName,
		PRL_CONST_STR sUserPassword,
		PRL_UINT32 nFlags
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, sUserName=%s, sUserPassword=%p, nFlags=%.8X)",
		__FUNCTION__,
		hVm,
		sUserName,
		sUserPassword,
		nFlags
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_AUTH_WITH_GUEST_SECURITY_DB)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_AuthWithGuestSecurityDb, (hVm, sUserName, sUserPassword, nFlags))
}

PRL_METHOD( PrlVmCfg_GetOfflineServices ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phOfflineServicesList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phOfflineServicesList=%p)",
		__FUNCTION__,
		hVmCfg,
		phOfflineServicesList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phOfflineServicesList))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetOfflineServices(phOfflineServicesList));
}

PRL_METHOD( PrlVmCfg_SetOfflineServices ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hOfflineServicesList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, hOfflineServicesList=%p)",
		__FUNCTION__,
		hVmCfg,
		hOfflineServicesList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		(PRL_INVALID_HANDLE != hOfflineServicesList && PRL_WRONG_HANDLE(hOfflineServicesList, PHT_STRINGS_LIST)))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	PrlHandleStringsListPtr pOfflineServicesList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hOfflineServicesList);
	return (pVm->SetOfflineServices(pOfflineServicesList.getHandle() ? pOfflineServicesList->GetStringsList() : QStringList()));
}

PRL_ASYNC_METHOD( PrlVmGuest_Logout ) (
		PRL_HANDLE hVmGuest,
		PRL_UINT32 nReserved
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmGuest=%p, nReserved=%.8X)",
		__FUNCTION__,
		hVmGuest,
		nReserved
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GUEST_LOGOUT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmGuest_Logout, (hVmGuest, nReserved))
}

PRL_ASYNC_METHOD( PrlVmGuest_RunProgram ) (
		PRL_HANDLE hVmGuest,
		PRL_CONST_STR sAppName,
		PRL_HANDLE hArgsList,
		PRL_HANDLE hEnvsList,
		PRL_UINT32 nFlags,
		PRL_FILE_DESC nStdin,
		PRL_FILE_DESC nStdout,
		PRL_FILE_DESC nStderr
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmGuest=%p, sAppName=%s, hArgsList=%p, hEnvsList=%p, nFlags=%.8X, nStdin=%.8X, nStdout=%.8X, nStderr=%.8X)",
		__FUNCTION__,
		hVmGuest,
		sAppName,
		hArgsList,
		hEnvsList,
		nFlags,
		nStdin,
		nStdout,
		nStderr
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GUEST_RUN_PROGRAM)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmGuest_RunProgram, (hVmGuest, sAppName, hArgsList, hEnvsList, nFlags, nStdin, nStdout, nStderr))
}

PRL_ASYNC_METHOD( PrlVmGuest_GetNetworkSettings ) (
		PRL_HANDLE hVmGuest,
		PRL_UINT32 nReserved
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmGuest=%p, nReserved=%.8X)",
		__FUNCTION__,
		hVmGuest,
		nReserved
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GUEST_GET_NETWORK_SETTINGS)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmGuest_GetNetworkSettings, (hVmGuest, nReserved))
}

PRL_ASYNC_METHOD( PrlVmGuest_SetUserPasswd ) (
		PRL_HANDLE hVmGuest,
		PRL_CONST_STR sUserName,
		PRL_CONST_STR sUserPasswd,
		PRL_UINT32 nFlags
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmGuest=%p, sUserName=%s, sUserPasswd=%p, nFlags=%.8X)",
		__FUNCTION__,
		hVmGuest,
		sUserName,
		sUserPasswd,
		nFlags
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GUEST_SET_USER_PASSWD)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmGuest_SetUserPasswd, (hVmGuest, sUserName, sUserPasswd, nFlags))
}

PRL_METHOD( PrlVmCfg_SetDefaultConfig ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hSrvConfig,
		PRL_UINT32 guestOsVersion,
		PRL_BOOL needCreateDevices
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>(hSrvConfig);
	return (pVm->SetDefaultConfig(pSrvConfig, guestOsVersion, needCreateDevices));
}

PRL_METHOD( PrlVmCfg_AddDefaultDevice ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hSrvConfig,
		PRL_DEVICE_TYPE deviceType
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION)
		|| (hSrvConfig != PRL_INVALID_HANDLE && PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG)))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>(hSrvConfig);
	return (pVm->AddDefaultDevice(pSrvConfig, deviceType));
}


PRL_METHOD( PrlVmCfg_AddDefaultDeviceEx ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hSrvConfig,
		PRL_DEVICE_TYPE deviceType,
		PRL_HANDLE_PTR phVmDevice
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION)
		|| PRL_WRONG_PTR(phVmDevice)
		|| (hSrvConfig != PRL_INVALID_HANDLE && PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG)))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>(hSrvConfig);
	return (pVm->AddDefaultDeviceEx(pSrvConfig, deviceType, phVmDevice));
}

PRL_METHOD( PrlVmCfg_IsDefaultDeviceNeeded ) (
		PRL_UINT32 guestOsVersion,
		PRL_DEVICE_TYPE deviceType,
		PRL_BOOL_PTR pbNeeded
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR( pbNeeded ) )
		return PRL_ERR_INVALID_ARG;

	*pbNeeded = PrlHandleVmDefaultConfig::IsDefaultDeviceNeeded( guestOsVersion, deviceType );
	return PRL_ERR_SUCCESS;
}


PRL_METHOD( PrlVmCfg_GetDefaultMemSize ) (
		PRL_UINT32 guestOsVersion,
		PRL_UINT32 hostRam,
		PRL_UINT32_PTR pnMemSize
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR( pnMemSize ) )
		return PRL_ERR_INVALID_ARG;

	*pnMemSize = PrlHandleVmDefaultConfig::GetDefaultRamSize( guestOsVersion, hostRam );
	return PRL_ERR_SUCCESS;
}


PRL_METHOD( PrlVmCfg_GetDefaultHddSize ) (
		PRL_UINT32 guestOsVersion,
		PRL_UINT32_PTR pnHddSize
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR( pnHddSize ) )
		return PRL_ERR_INVALID_ARG;

	*pnHddSize = PrlHandleVmDefaultConfig::GetDefaultHddSize( guestOsVersion );
	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlVmCfg_GetDefaultVideoRamSize ) (
		PRL_UINT32 guestOsVersion,
		PRL_HANDLE hSrvConfig,
		PRL_BOOL bIs3DSupportEnabled,
		PRL_UINT32_PTR pnVideoRamSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (guestOsVersion=%.8X, hSrvConfig=%p, bIs3DSupportEnabled=%.8X, pnVideoRamSize=%p)",
		__FUNCTION__,
		guestOsVersion,
		hSrvConfig,
		bIs3DSupportEnabled,
		pnVideoRamSize
		);

	SYNC_CHECK_API_INITIALIZED
	if ( PRL_WRONG_PTR( pnVideoRamSize ) ||
			( PRL_INVALID_HANDLE != hSrvConfig && PRL_WRONG_HANDLE( hSrvConfig, PHT_SERVER_CONFIG ) ) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>(hSrvConfig);

	*pnVideoRamSize = PrlHandleVmDefaultConfig::GetDefaultVideoRamSize( guestOsVersion, pSrvConfig, bIs3DSupportEnabled );
	return PRL_ERR_SUCCESS;
}

PRL_HANDLE PrlVm_RegEx_Impl(PRL_HANDLE hVm, PRL_CONST_STR sVmParentPath, PRL_UINT32 nFlags)
{
	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sVmParentPath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_REG))

		PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>(hVm);
	if (!pVm->GetServer())
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_REG))

		PrlHandleJobPtr pJob = pVm->GetServerVm()->RegVm(pVm, sVmParentPath, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_RegEx) (
		PRL_HANDLE hVm,
		PRL_CONST_STR sVmParentPath,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_REG)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_RegEx, (hVm, sVmParentPath, nFlags))
}


PRL_ASYNC_METHOD( PrlVm_Reg) (
		PRL_HANDLE hVm,
		PRL_CONST_STR sVmParentPath,
		PRL_BOOL bNonInteractiveMode
		)
{
	return PrlVm_RegEx( hVm, sVmParentPath, bNonInteractiveMode ? PACF_NON_INTERACTIVE_MODE : 0);
}

PRL_METHOD( PrlVmCfg_CreateVmDev) (
		PRL_HANDLE hVmCfg,
		PRL_DEVICE_TYPE device_type,
		PRL_HANDLE_PTR phVmDevice
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, device_type=%.8X, phVmDevice=%p)",
		__FUNCTION__,
		hVmCfg,
		device_type,
		phVmDevice
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_VM_DEVICE_TYPE(device_type) || PRL_WRONG_PTR(phVmDevice) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>(hVmCfg);
	return (PrlHandleVmDevice::CreateVmDevice(pVm, device_type, phVmDevice));
}

PRL_METHOD( PrlVmCfg_GetDevsCount ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR vmdev_handles_count
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, vmdev_handles_count=%p)",
		__FUNCTION__,
		hVmCfg,
		vmdev_handles_count
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(vmdev_handles_count) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);

	return (pVm->GetDevicesCount(vmdev_handles_count));
}


PRL_METHOD( PrlVmCfg_GetAllDevices ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phDevsList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phDevsList=%p)",
		__FUNCTION__,
		hVmCfg,
		phDevsList
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phDevsList) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);

	return (pVm->GetAllDevices(phDevsList));
}


PRL_METHOD( PrlVmCfg_GetDevsCountByType ) (
		PRL_HANDLE hVmCfg,
		PRL_DEVICE_TYPE vmDeviceType,
		PRL_UINT32_PTR pnDevicesCount
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (vmDeviceType >= PDE_MAX)
	   return PRL_ERR_INVALID_ARG ;

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(pnDevicesCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetDevsCountByType(vmDeviceType, pnDevicesCount) ;
}


PRL_METHOD( PrlVmCfg_GetDevByType ) (
		PRL_HANDLE hVmCfg,
		PRL_DEVICE_TYPE vmDeviceType,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phDevice
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phDevice))
		return (PRL_ERR_INVALID_ARG);

	if (vmDeviceType >= PDE_MAX)
	   return PRL_ERR_INVALID_ARG ;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );
	return pVm->GetDevByType(vmDeviceType, nIndex, phDevice) ;
}


PRL_METHOD(PrlVmCfg_GetFloppyDisksCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_FLOPPY_DISK, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetFloppyDisk) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_FLOPPY_DISK, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetHardDisksCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_HARD_DISK, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetHardDisk) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_HARD_DISK, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetOpticalDisksCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_OPTICAL_DISK, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetOpticalDisk) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_OPTICAL_DISK, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetParallelPortsCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_PARALLEL_PORT, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetParallelPort) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_PARALLEL_PORT, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetSerialPortsCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_SERIAL_PORT, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetSerialPort) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_SERIAL_PORT, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetSoundDevsCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_SOUND_DEVICE, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetSoundDev) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_SOUND_DEVICE, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetGenericPciDevsCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_GENERIC_PCI_DEVICE, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetGenericPciDev) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_GENERIC_PCI_DEVICE, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetGenericScsiDevsCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_GENERIC_SCSI_DEVICE, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetGenericScsiDev) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_GENERIC_SCSI_DEVICE, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetDisplayDevsCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_PCI_VIDEO_ADAPTER, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetDisplayDev) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_PCI_VIDEO_ADAPTER, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetUsbDevicesCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_USB_DEVICE, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetUsbDevice) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_USB_DEVICE, nIndex, phDevice) ;
}

PRL_METHOD(PrlVmCfg_GetNetAdaptersCount) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnCount
				 )
{
   return PrlVmCfg_GetDevsCountByType(hVmCfg, PDE_GENERIC_NETWORK_ADAPTER, pnCount) ;
}

PRL_METHOD(PrlVmCfg_GetNetAdapter) (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 )
{
   return PrlVmCfg_GetDevByType(hVmCfg, PDE_GENERIC_NETWORK_ADAPTER, nIndex, phDevice) ;
}

PRL_METHOD( PrlVmCfg_CreateShare) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phVmShare
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phVmShare=%p)",
		__FUNCTION__,
		hVmCfg,
		phVmShare
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phVmShare) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->CreateShare(phVmShare));
}

PRL_METHOD( PrlVmCfg_GetSharesCount) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnSharesCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnSharesCount=%p)",
		__FUNCTION__,
		hVmCfg,
		pnSharesCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(pnSharesCount) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetSharesCount(pnSharesCount));
}


PRL_METHOD( PrlVmCfg_GetShare) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nShareIndex,
		PRL_HANDLE_PTR phVmShare
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nShareIndex=%.8X, phVmShare=%p)",
		__FUNCTION__,
		hVmCfg,
		nShareIndex,
		phVmShare
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phVmShare) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetShare(nShareIndex, phVmShare));
}

PRL_METHOD( PrlVmCfg_IsUserDefinedSharedFoldersEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsUserDefinedSharedFoldersEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetUserDefinedSharedFoldersEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetUserDefinedSharedFoldersEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsSmartMountEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsSmartMountEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetSmartMountEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetSmartMountEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsSmartMountRemovableDrivesEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsSmartMountRemovableDrivesEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetSmartMountRemovableDrivesEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetSmartMountRemovableDrivesEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsSmartMountDVDsEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsSmartMountDVDsEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetSmartMountDVDsEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetSmartMountDVDsEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsSmartMountNetworkSharesEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsSmartMountNetworkSharesEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetSmartMountNetworkSharesEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetSmartMountNetworkSharesEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsSmartGuardEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsSmartGuardEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetSmartGuardEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetSmartGuardEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsSmartGuardNotifyBeforeCreation ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbNotifyBeforeCreation
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbNotifyBeforeCreation=%p)",
		__FUNCTION__,
		hVmCfg,
		pbNotifyBeforeCreation
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbNotifyBeforeCreation) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsSmartGuardNotifyBeforeCreation(pbNotifyBeforeCreation));
}

PRL_METHOD( PrlVmCfg_SetSmartGuardNotifyBeforeCreation ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bNotifyBeforeCreation
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bNotifyBeforeCreation=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bNotifyBeforeCreation
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetSmartGuardNotifyBeforeCreation(bNotifyBeforeCreation));
}

PRL_METHOD( PrlVmCfg_GetSmartGuardInterval ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnInterval
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnInterval=%p)",
		__FUNCTION__,
		hVmCfg,
		pnInterval
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnInterval) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetSmartGuardInterval(pnInterval));
}

PRL_METHOD( PrlVmCfg_SetSmartGuardInterval ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nInterval
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nInterval=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nInterval
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetSmartGuardInterval(nInterval));
}

PRL_METHOD( PrlVmCfg_GetSmartGuardMaxSnapshotsCount ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnMaxSnapshotsCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnMaxSnapshotsCount=%p)",
		__FUNCTION__,
		hVmCfg,
		pnMaxSnapshotsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnMaxSnapshotsCount) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetSmartGuardMaxSnapshotsCount(pnMaxSnapshotsCount));
}

PRL_METHOD( PrlVmCfg_SetSmartGuardMaxSnapshotsCount ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nMaxSnapshotsCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nMaxSnapshotsCount=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nMaxSnapshotsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetSmartGuardMaxSnapshotsCount(nMaxSnapshotsCount));
}

PRL_METHOD( PrlVmCfg_IsAllowSelectBootDevice ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbAllowed
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbAllowed=%p)",
		__FUNCTION__,
		hVmCfg,
		pbAllowed
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbAllowed) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsAllowSelectBootDevice(pbAllowed));
}

PRL_METHOD( PrlVmCfg_SetAllowSelectBootDevice ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bAllowed
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bAllowed=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bAllowed
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetAllowSelectBootDevice(bAllowed));
}

PRL_METHOD( PrlVmCfg_IsEfiEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEfiEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEfiEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEfiEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEfiEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->IsEfiEnabled(pbEfiEnabled));
}

PRL_METHOD( PrlVmCfg_SetEfiEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEfiEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEfiEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEfiEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetEfiEnabled(bEfiEnabled));
}

PRL_METHOD( PrlVmCfg_GetExternalBootDevice) (
		PRL_HANDLE hVmCfg,
		PRL_STR sSysName,
		PRL_UINT32_PTR pnSysNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sSysName=%p, pnSysNameBufLength=%p)",
		__FUNCTION__,
		hVmCfg,
		sSysName,
		pnSysNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnSysNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetExternalBootDevice(sSysName, pnSysNameBufLength));
}

PRL_METHOD( PrlVmCfg_SetExternalBootDevice) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewSysName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, sNewSysName=%p)",
		__FUNCTION__,
		hVmCfg,
		sNewSysName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(sNewSysName))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetExternalBootDevice(sNewSysName));
}

PRL_METHOD( PrlVmCfg_IsSharedProfileEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsSharedProfileEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetSharedProfileEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetSharedProfileEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsUseDesktop ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsUseDesktop(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetUseDesktop ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetUseDesktop(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsUseDocuments ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsUseDocuments(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetUseDocuments ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetUseDocuments(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsUsePictures ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsUsePictures(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetUsePictures ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetUsePictures(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsUseMusic ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsUseMusic(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetUseMusic ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetUseMusic(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsUseDownloads ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsUseDownloads(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetUseDownloads ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetUseDownloads(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsUseMovies ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsUseMovies(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetUseMovies ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetUseMovies(bEnabled));
}

PRL_METHOD( PrlVmCfg_GetOptimizeModifiersMode ) (
		PRL_HANDLE hVmCfg,
		PRL_OPTIMIZE_MODIFIERS_MODE_PTR pnMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnMode=%p)",
		__FUNCTION__,
		hVmCfg,
		pnMode
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnMode) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetOptimizeModifiersMode(pnMode));
}

PRL_METHOD( PrlVmCfg_SetOptimizeModifiersMode ) (
		PRL_HANDLE hVmCfg,
		PRL_OPTIMIZE_MODIFIERS_MODE nMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nMode=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nMode
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetOptimizeModifiersMode(nMode));
}

PRL_METHOD( PrlVmCfg_IsShareClipboard ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsShareClipboard(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetShareClipboard ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetShareClipboard(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsTimeSynchronizationEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsTimeSynchronizationEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetTimeSynchronizationEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetTimeSynchronizationEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_IsTimeSyncSmartModeEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pbEnabled) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->IsTimeSyncSmartModeEnabled(pbEnabled));
}

PRL_METHOD( PrlVmCfg_SetTimeSyncSmartModeEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetTimeSyncSmartModeEnabled(bEnabled));
}

PRL_METHOD( PrlVmCfg_GetTimeSyncInterval ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnTimeSyncInterval
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnTimeSyncInterval=%p)",
		__FUNCTION__,
		hVmCfg,
		pnTimeSyncInterval
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
			PRL_WRONG_PTR(pnTimeSyncInterval) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetTimeSyncInterval(pnTimeSyncInterval));
}

PRL_METHOD( PrlVmCfg_SetTimeSyncInterval ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nTimeSyncInterval
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nTimeSyncInterval=%.8X)",
		__FUNCTION__,
		hVmCfg,
		nTimeSyncInterval
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetTimeSyncInterval(nTimeSyncInterval));
}

PRL_METHOD( PrlVmCfg_CreateBootDev) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phVmBootDev
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phVmBootDev=%p)",
		__FUNCTION__,
		hVmCfg,
		phVmBootDev
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phVmBootDev) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->CreateBootDev(phVmBootDev));
}

PRL_METHOD( PrlVmCfg_GetBootDevCount) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnBootDevCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnBootDevCount=%p)",
		__FUNCTION__,
		hVmCfg,
		pnBootDevCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(pnBootDevCount) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetBootDevCount(pnBootDevCount));
}


PRL_METHOD( PrlVmCfg_GetBootDev) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nBootDevIndex,
		PRL_HANDLE_PTR phVmBootDev
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nBootDevIndex=%.8X, phVmBootDev=%p)",
		__FUNCTION__,
		hVmCfg,
		nBootDevIndex,
		phVmBootDev
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(phVmBootDev) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetBootDev(nBootDevIndex, phVmBootDev));
}

PRL_METHOD( PrlVmCfg_SetCpuUnits ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmCpuUnits
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmCpuUnits=%.8X)",
			__FUNCTION__,
			hVmCfg,
			nVmCpuUnits
		   );

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCpuUnits(nVmCpuUnits));
}

PRL_METHOD( PrlVmCfg_GetCpuUnits ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmCpuUnits
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmCpuUnits=%p)",
			__FUNCTION__,
			hVmCfg,
			pnVmCpuUnits
		   );

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    PRL_WRONG_PTR(pnVmCpuUnits))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetCpuUnits(pnVmCpuUnits));
}

PRL_METHOD( PrlVmCfg_SetCpuLimit ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmCpuLimit
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmCpuLimit=%.8X)",
			__FUNCTION__,
			hVmCfg,
			nVmCpuLimit
		   );

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCpuLimit(nVmCpuLimit));
}

PRL_METHOD( PrlVmCfg_GetCpuLimit ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmCpuLimit
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pnVmCpuLimit=%p)",
			__FUNCTION__,
			hVmCfg,
			pnVmCpuLimit
		   );

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    PRL_WRONG_PTR(pnVmCpuLimit))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetCpuLimit(pnVmCpuLimit));
}

PRL_METHOD( PrlVmCfg_SetCpuLimitEx ) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_CPULIMIT_DATA_PTR pVmCpuLimit
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pVmCpuLimit=%p)",
			__FUNCTION__,
			hVmCfg,
			pVmCpuLimit
		   );

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    PRL_WRONG_PTR(pVmCpuLimit))
		return (PRL_ERR_INVALID_ARG);

	if (pVmCpuLimit->type != PRL_CPULIMIT_PERCENTS &&
	    pVmCpuLimit->type != PRL_CPULIMIT_PERCENTS100 &&
	    pVmCpuLimit->type != PRL_CPULIMIT_MHZ &&
	    pVmCpuLimit->type != PRL_CPULIMIT_PERCENTS_TO_MHZ)
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCpuLimitEx(pVmCpuLimit));
}

PRL_METHOD( PrlVmCfg_GetCpuLimitEx ) (
		PRL_HANDLE hVmCfg,
		PRL_CPULIMIT_DATA_PTR pVmCpuLimit
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmCpuLimit=%p)",
			__FUNCTION__,
			hVmCfg,
			pVmCpuLimit
		   );

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    PRL_WRONG_PTR(pVmCpuLimit))
		return (PRL_ERR_INVALID_ARG);

	if (pVmCpuLimit->type != PRL_CPULIMIT_PERCENTS &&
	    pVmCpuLimit->type != PRL_CPULIMIT_PERCENTS100 &&
	    pVmCpuLimit->type != PRL_CPULIMIT_MHZ)
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetCpuLimitEx(pVmCpuLimit));
}

PRL_METHOD( PrlVmCfg_SetCpuMask) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sMask
		)
{
	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetCpuMask(sMask));
}

PRL_RESULT PrlVmCfg_GetCpuMask(
		PRL_HANDLE hVmCfg,
		PRL_STR sMask,
		PRL_UINT32_PTR pnMaskBufLength
		)
{
	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    PRL_WRONG_PTR(pnMaskBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetCpuMask(sMask, pnMaskBufLength));
}

PRL_METHOD( PrlVmCfg_SetIoPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmIoPriority
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    nVmIoPriority > PRL_IOPRIO_MAX)
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetIoPriority(nVmIoPriority));
}

PRL_METHOD( PrlVmCfg_GetIoPriority ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmIoPriority
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    PRL_WRONG_PTR(pnVmIoPriority))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetIoPriority(pnVmIoPriority));
}

PRL_METHOD( PrlVmCfg_SetIopsLimit ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmIopsLimit
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, nVmIoLimit=%u)",
			__FUNCTION__,
			hVmCfg,
			nVmIopsLimit
		   );

#ifndef _LIN_
	return PRL_ERR_UNIMPLEMENTED;
#endif

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetIopsLimit(nVmIopsLimit));
}

PRL_METHOD( PrlVmCfg_GetIopsLimit ) (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmIopsLimit
		)
{
	SYNC_CHECK_API_INITIALIZED

#ifndef _LIN_
	return PRL_ERR_UNIMPLEMENTED;
#endif

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    PRL_WRONG_PTR(pnVmIopsLimit))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetIopsLimit(pnVmIopsLimit));
}

PRL_METHOD( PrlVmCfg_SetIoLimit ) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_IOLIMIT_DATA_PTR pVmIoLimit
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pVmIoLimit=%p)",
			__FUNCTION__,
			hVmCfg,
			pVmIoLimit
		   );

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    PRL_WRONG_PTR(pVmIoLimit))
		return (PRL_ERR_INVALID_ARG);

	if (pVmIoLimit->type != PRL_IOLIMIT_BS)
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->SetIoLimit(pVmIoLimit));
}

PRL_METHOD( PrlVmCfg_GetIoLimit ) (
		PRL_HANDLE hVmCfg,
		PRL_IOLIMIT_DATA_PTR pVmIoLimit
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pVmIoLimit=%p)",
			__FUNCTION__,
			hVmCfg,
			pVmIoLimit
		   );

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	    PRL_WRONG_PTR(pVmIoLimit))
		return (PRL_ERR_INVALID_ARG);

	if (pVmIoLimit->type != PRL_IOLIMIT_BS)
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return (pVm->GetIoLimit(pVmIoLimit));
}

PRL_HANDLE PrlVm_CreateUnattendedFloppy_Impl(PRL_HANDLE hVm,
													  PRL_GUEST_DISTRO_VERSION_ENUM nGuestDistroType,
													  PRL_CONST_STR sUser,
													  PRL_CONST_STR sCompany,
													  PRL_CONST_STR sKey
													  )
{
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>( hVm );
	PrlHandleJobPtr pJob = pVm->CreateUnattendedFloppy( nGuestDistroType, sKey, sUser, sCompany );
	if ( !pJob )
		RETURN_RES(PRL_INVALID_HANDLE)
		pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_CreateUnattendedFloppy ) (
		PRL_HANDLE hVm,
		PRL_GUEST_DISTRO_VERSION_ENUM nGuestDistroType,
		PRL_CONST_STR sUsername,
		PRL_CONST_STR sCompanyName,
		PRL_CONST_STR sSerialKey
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_CREATE_UNATTENDED_FLOPPY)

	LOG_MESSAGE( DBG_DEBUG, "%s (hVm=%p, nGuestDistroType=%.8X, sUsername=%s, sCompanyName=%s, sSerialKey=%s)",
		__FUNCTION__,
		hVm,
		nGuestDistroType,
		sUsername,
		sCompanyName,
		sSerialKey
		);
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sUsername) || PRL_WRONG_PTR(sCompanyName) ||
				PRL_WRONG_PTR(sSerialKey) )
		return GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_CREATE_UNATTENDED_FLOPPY);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_CreateUnattendedFloppy, (hVm, nGuestDistroType, sUsername, sCompanyName, sSerialKey))
}

PRL_HANDLE PrlSrv_CreateUnattendedCd_Impl(
		PRL_HANDLE hServer,
		PRL_UINT32 nGuestType,
		PRL_CONST_STR sUserName,
		PRL_CONST_STR sPasswd,
		PRL_CONST_STR sFullUserName,
		PRL_CONST_STR sOsDistroPath,
		PRL_CONST_STR sOutImagePath
		)

{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(sUserName) || PRL_WRONG_PTR(sPasswd) ||
		PRL_WRONG_PTR(sFullUserName) || PRL_WRONG_PTR(sOsDistroPath) || PRL_WRONG_PTR(sOutImagePath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_CREATE_UNATTENDED_CD))

	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	PrlHandleJobPtr pJob = pServer->DspCmdCreateUnattendedCd(nGuestType, sUserName,
		sPasswd, sFullUserName, sOsDistroPath, sOutImagePath);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_CreateUnattendedCd ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nGuestType,
	PRL_CONST_STR sUserName,
	PRL_CONST_STR sPasswd,
	PRL_CONST_STR sFullUserName,
	PRL_CONST_STR sOsDistroPath,
	PRL_CONST_STR sOutImagePath
)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_CREATE_UNATTENDED_CD)
	CALL_THROUGH_CTXT_SWITCHER(
		PrlContextSwitcher::Instance(),
		PrlSrv_CreateUnattendedCd,
		(hServer, nGuestType, sUserName, sPasswd, sFullUserName, sOsDistroPath, sOutImagePath))
}

PRL_HANDLE PrlSrv_GetDefaultVmConfig_Impl(
		PRL_HANDLE hServer,
		PRL_CONST_GET_VM_CONFIG_PARAM_DATA_PTR pParam,
		PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(pParam) ||
	     (pParam->nVmType != PVT_VM && pParam->nVmType != PVT_CT))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_GET_DEFAULT_VM_CONFIG))

	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );
	PrlHandleJobPtr pJob = pServer->DspCmdGetDefaultVmConfig(pParam, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_GetDefaultVmConfig ) (
		PRL_HANDLE hServer,
		PRL_CONST_GET_VM_CONFIG_PARAM_DATA_PTR pParam,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_DEFAULT_VM_CONFIG)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetDefaultVmConfig, (hServer, pParam, nFlags))
}

PRL_METHOD( PrlVmDev_Create) (
		PRL_DEVICE_TYPE device_type,
		PRL_HANDLE_PTR phVmDevice
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (device_type=%.8X, phVmDevice=%p)",
		__FUNCTION__,
		device_type,
		phVmDevice
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_VM_DEVICE_TYPE(device_type) || PRL_WRONG_PTR(phVmDevice) )
		return (PRL_ERR_INVALID_ARG);

	return (PrlHandleVmDevice::CreateVmDevice(PrlHandleVmPtr(0), device_type, phVmDevice));
}

PRL_ASYNC_METHOD( PrlVmDev_Connect ) (
		PRL_HANDLE hVmDevice
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDevice=%p)",
		__FUNCTION__,
		hVmDevice
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_DEV_CONNECT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmDev_Connect, (hVmDevice))
}


PRL_ASYNC_METHOD( PrlVmDev_Disconnect ) (
		PRL_HANDLE hVmDevice
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDevice=%p)",
		__FUNCTION__,
		hVmDevice
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_DEV_DISCONNECT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmDev_Disconnect, (hVmDevice))
}

PRL_ASYNC_METHOD( PrlVmDev_CreateImage ) (
		PRL_HANDLE hVmDevice,
		PRL_BOOL bRecreateIsAllowed,
		PRL_BOOL bNonInteractiveMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDevice=%p, bRecreateIsAllowed=%.8X)",
		__FUNCTION__,
		hVmDevice,
		bRecreateIsAllowed
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_DEV_CREATE_IMAGE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmDev_CreateImage, (hVmDevice, bRecreateIsAllowed, bNonInteractiveMode ) )
}


PRL_ASYNC_METHOD( PrlVmDev_CopyImage ) (
		PRL_HANDLE hVmDevice,
		PRL_CONST_STR sNewImageName,
		PRL_CONST_STR sTargetPath,
		PRL_UINT32 nFlags
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDevice=%p, sNewImageName=%s, sTargetPath=%s, nFlags=%.8X)",
		__FUNCTION__,
		hVmDevice,
		sNewImageName,
		sTargetPath,
		nFlags
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_DEV_COPY_IMAGE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(),\
		PrlVmDev_CopyImage, (hVmDevice, sNewImageName, sTargetPath, nFlags) )
}


PRL_ASYNC_METHOD( PrlVmDev_ResizeImage ) (
                PRL_HANDLE hVmDevice,
                PRL_UINT32 nNewSize,
                PRL_UINT32 nFlags
                )
{
	WRITE_TRACE(DBG_FATAL, "PrlVmDev_ResizeImage");
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_RESIZE_DISK_IMAGE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmDev_ResizeImage, (hVmDevice, nNewSize, nFlags ) )
}

PRL_METHOD( PrlVmDev_GetIndex ) (
		PRL_HANDLE hVmDevice,
		PRL_UINT32_PTR nIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDevice=%p, nIndex=%p)",
		__FUNCTION__,
		hVmDevice,
		nIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDevice) || PRL_WRONG_PTR( nIndex ))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDevice);
	return (pDevice->GetIndex(nIndex));
}


PRL_METHOD( PrlVmDev_SetIndex ) (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nIndex=%.8X)",
		__FUNCTION__,
		hVmDev,
		nIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetIndex(nIndex));
}


PRL_METHOD( PrlVmDev_Remove ) (
		PRL_HANDLE hVmDev
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p)",
		__FUNCTION__,
		hVmDev
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->Remove());
}

PRL_METHOD( PrlVmDev_GetType ) (
		PRL_HANDLE hVmDev,
		PRL_DEVICE_TYPE_PTR pnDeviceType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnDeviceType=%p)",
		__FUNCTION__,
		hVmDev,
		pnDeviceType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(pnDeviceType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->GetDeviceType(pnDeviceType));
}

PRL_METHOD( PrlVmDev_IsConnected ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbConnected
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbConnected=%p)",
		__FUNCTION__,
		hVmDev,
		pbConnected
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(pbConnected))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->IsConnected(pbConnected));
}


PRL_METHOD( PrlVmDev_SetConnected ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bConnected
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bConnected=%.8X)",
		__FUNCTION__,
		hVmDev,
		bConnected
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetConnected(bConnected));
}


PRL_METHOD( PrlVmDev_IsEnabled ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmDev,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->IsEnabled(pbEnabled));
}


PRL_METHOD( PrlVmDev_SetEnabled ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmDev,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetEnabled(bEnabled));
}

PRL_METHOD( PrlVmDev_IsRemote ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbRemote
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbRemote=%p)",
		__FUNCTION__,
		hVmDev,
		pbRemote
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(pbRemote))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->IsRemote(pbRemote));
}


PRL_METHOD( PrlVmDev_SetRemote ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bRemote
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bRemote=%.8X)",
		__FUNCTION__,
		hVmDev,
		bRemote
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetRemote(bRemote));
}

PRL_METHOD( PrlVmDev_GetEmulatedType ) (
		PRL_HANDLE hVmDev,
		PRL_VM_DEV_EMULATION_TYPE_PTR pnEmulatedType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnEmulatedType=%p)",
		__FUNCTION__,
		hVmDev,
		pnEmulatedType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(pnEmulatedType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->GetEmulatedType(pnEmulatedType));
}

PRL_METHOD( PrlVmDev_SetEmulatedType ) (
		PRL_HANDLE hVmDev,
		PRL_VM_DEV_EMULATION_TYPE nEmulatedType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nEmulatedType=%.8X)",
		__FUNCTION__,
		hVmDev,
		nEmulatedType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetEmulatedType(nEmulatedType));
}

PRL_METHOD( PrlVmDev_GetImagePath ) (
		PRL_HANDLE hVmDev,
		PRL_STR sImagePath,
		PRL_UINT32_PTR pnImagePathBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sImagePath=%p, pnImagePathBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sImagePath,
		pnImagePathBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) ||
			PRL_WRONG_PTR(pnImagePathBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->GetFriendlyName(sImagePath, pnImagePathBufLength));
}

PRL_METHOD( PrlVmDev_SetImagePath ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewImagePath
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewImagePath=%s)",
		__FUNCTION__,
		hVmDev,
		sNewImagePath
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(sNewImagePath))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetFriendlyName(sNewImagePath));
}


PRL_METHOD( PrlVmDev_GetSysName ) (
		PRL_HANDLE hVmDev,
		PRL_STR sSysName,
		PRL_UINT32_PTR pnSysNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sSysName=%p, pnSysNameBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sSysName,
		pnSysNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) ||
			PRL_WRONG_PTR(pnSysNameBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->GetSysName(sSysName, pnSysNameBufLength));
}

PRL_METHOD( PrlVmDev_SetSysName ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewSysName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewSysName=%s)",
		__FUNCTION__,
		hVmDev,
		sNewSysName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(sNewSysName))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetSysName(sNewSysName));
}


PRL_METHOD( PrlVmDev_GetFriendlyName ) (
		PRL_HANDLE hVmDev,
		PRL_STR sFriendlyName,
		PRL_UINT32_PTR pnFriendlyNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sFriendlyName=%p, pnFriendlyNameBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sFriendlyName,
		pnFriendlyNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) ||
			PRL_WRONG_PTR(pnFriendlyNameBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->GetFriendlyName(sFriendlyName, pnFriendlyNameBufLength));
}

PRL_METHOD( PrlVmDev_SetFriendlyName ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewFriendlyName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewFriendlyName=%s)",
		__FUNCTION__,
		hVmDev,
		sNewFriendlyName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(sNewFriendlyName))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetFriendlyName(sNewFriendlyName));
}


PRL_METHOD( PrlVmDev_GetDescription ) (
		PRL_HANDLE hVmDev,
		PRL_STR sDescription,
		PRL_UINT32_PTR pnDescriptionBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sDescription=%p, pnDescriptionBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sDescription,
		pnDescriptionBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) ||
			PRL_WRONG_PTR(pnDescriptionBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->GetDescription(sDescription, pnDescriptionBufLength));
}

PRL_METHOD( PrlVmDev_SetDescription ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewDescription
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewDescription=%s)",
		__FUNCTION__,
		hVmDev,
		sNewDescription
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(sNewDescription))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetDescription(sNewDescription));
}


PRL_METHOD( PrlVmDev_GetIfaceType ) (
		PRL_HANDLE hVmDev,
		PRL_MASS_STORAGE_INTERFACE_TYPE_PTR pnIfaceType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnIfaceType=%p)",
		__FUNCTION__,
		hVmDev,
		pnIfaceType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(pnIfaceType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->GetIfaceType(pnIfaceType));
}

PRL_METHOD( PrlVmDev_SetIfaceType ) (
		PRL_HANDLE hVmDev,
		PRL_MASS_STORAGE_INTERFACE_TYPE nIfaceType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nIfaceType=%.8X)",
		__FUNCTION__,
		hVmDev,
		nIfaceType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetIfaceType(nIfaceType));
}

PRL_METHOD( PrlVmDev_GetSubType ) (
		PRL_HANDLE hVmDev,
		PRL_CLUSTERED_DEVICE_SUBTYPE_PTR pnSubType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnSubType=%p)",
		__FUNCTION__,
		hVmDev,
		pnSubType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(pnSubType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->GetSubType(pnSubType));
}

PRL_METHOD( PrlVmDev_SetSubType ) (
		PRL_HANDLE hVmDev,
		PRL_CLUSTERED_DEVICE_SUBTYPE nSubType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nSubType=%.8X)",
		__FUNCTION__,
		hVmDev,
		nSubType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetSubType(nSubType));
}

PRL_METHOD( PrlVmDev_GetStackIndex ) (
		PRL_HANDLE hVmDevice,
		PRL_UINT32_PTR nStackIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDevice=%p, nStackIndex=%p)",
		__FUNCTION__,
		hVmDevice,
		nStackIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDevice) || PRL_WRONG_PTR( nStackIndex ))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDevice);
	return (pDevice->GetStackIndex(nStackIndex));
}


PRL_METHOD( PrlVmDev_SetStackIndex ) (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nStackIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nStackIndex=%.8X)",
		__FUNCTION__,
		hVmDev,
		nStackIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetStackIndex(nStackIndex));
}

PRL_METHOD( PrlVmDev_SetDefaultStackIndex ) (
		PRL_HANDLE hVmDev
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p)",
		__FUNCTION__,
		hVmDev
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetDefaultStackIndex());
}


PRL_METHOD( PrlVmDev_GetOutputFile ) (
		PRL_HANDLE hVmDev,
		PRL_STR sOutputFile,
		PRL_UINT32_PTR pnOutputFileBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sOutputFile=%p, pnOutputFileBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sOutputFile,
		pnOutputFileBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) ||
			PRL_WRONG_PTR(pnOutputFileBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->GetFriendlyName(sOutputFile, pnOutputFileBufLength));
}

PRL_METHOD( PrlVmDev_SetOutputFile ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewOutputFile
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewOutputFile=%s)",
		__FUNCTION__,
		hVmDev,
		sNewOutputFile
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) || PRL_WRONG_PTR(sNewOutputFile))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetFriendlyName(sNewOutputFile));
}

PRL_METHOD( PrlVmDevHd_GetDiskType ) (
		PRL_HANDLE hVmDev,
		PRL_HARD_DISK_INTERNAL_FORMAT_PTR pnDiskType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnDiskType=%p)",
		__FUNCTION__,
		hVmDev,
		pnDiskType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pnDiskType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->GetDiskType(pnDiskType));
}

PRL_METHOD( PrlVmDevHd_SetDiskType ) (
		PRL_HANDLE hVmDev,
		PRL_HARD_DISK_INTERNAL_FORMAT nDiskType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nDiskType=%.8X)",
		__FUNCTION__,
		hVmDev,
		nDiskType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->SetDiskType(nDiskType));
}

PRL_METHOD( PrlVmDevHd_IsSplitted ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbSplitted
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbSplitted=%p)",
		__FUNCTION__,
		hVmDev,
		pbSplitted
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pbSplitted))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->IsSplitted(pbSplitted));
}

PRL_METHOD( PrlVmDevHd_SetSplitted ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bSplitted
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bSplitted=%.8X)",
		__FUNCTION__,
		hVmDev,
		bSplitted
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->SetSplitted(bSplitted));
}

PRL_METHOD( PrlVmDevHd_GetDiskSize ) (
		PRL_HANDLE hVmDev,
		PRL_UINT32_PTR pnDiskSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnDiskSize=%p)",
		__FUNCTION__,
		hVmDev,
		pnDiskSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pnDiskSize))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->GetDiskSize(pnDiskSize));
}

PRL_METHOD( PrlVmDevHd_SetDiskSize ) (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nDiskSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nDiskSize=%.8X)",
		__FUNCTION__,
		hVmDev,
		nDiskSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->SetDiskSize(nDiskSize));
}

PRL_METHOD( PrlVmDevHd_GetSizeOnDisk ) (
		PRL_HANDLE hVmDev,
		PRL_UINT32_PTR pnOnDiskSize
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnOnDiskSize=%p)",
		__FUNCTION__,
		hVmDev,
		pnOnDiskSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pnOnDiskSize))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->GetSizeOnDisk(pnOnDiskSize));
}

PRL_METHOD( PrlVmDevHd_SetPassword ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sPassword
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sPassword=%p)",
		__FUNCTION__,
		hVmDev,
		sPassword
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->SetPassword(sPassword));
}

PRL_METHOD( PrlVmDevHd_IsEncrypted ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbIsEncrypted
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbIsEncrypted=%p)",
		__FUNCTION__,
		hVmDev,
		pbIsEncrypted
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) || PRL_WRONG_PTR(pbIsEncrypted))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->IsEncrypted(pbIsEncrypted));
}

PRL_ASYNC_METHOD( PrlVmDevHd_CheckPassword ) (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nFlags
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nFlags=%.8x)",
		__FUNCTION__,
		hVmDev,
		nFlags
		);
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_DEV_HD_CHECK_PASSWORD)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVmDevHd_CheckPassword, (hVmDev, nFlags))
}

PRL_METHOD( PrlVmDevHd_AddPartition ) (
		PRL_HANDLE hVmDev,
		PRL_HANDLE_PTR phPartition
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, phPartition=%p)",
		__FUNCTION__,
		hVmDev,
		phPartition
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(phPartition))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return pDevice->AddPartition(phPartition);
}

PRL_METHOD( PrlVmDevHd_GetPartitionsCount ) (
		 PRL_HANDLE hVmDev,
		 PRL_UINT32_PTR pnPartitionsCount
		 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnPartitionsCount=%p)",
		__FUNCTION__,
		hVmDev,
		pnPartitionsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pnPartitionsCount))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return pDevice->GetPartitionsCount(pnPartitionsCount);
}

PRL_METHOD( PrlVmDevHd_GetPartition ) (
		 PRL_HANDLE hVmDev,
		 PRL_UINT32 nIndex,
		 PRL_HANDLE_PTR phPartition
		 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nIndex=%.8X, phPartition=%p)",
		__FUNCTION__,
		hVmDev,
		nIndex,
		phPartition
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(phPartition))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return pDevice->GetPartition(nIndex, phPartition);
}

PRL_METHOD( PrlVmDevHd_SetMountPoint ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sMountPoint
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(sMountPoint))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return pDevice->SetMountPoint(sMountPoint);
}

PRL_METHOD( PrlVmDevHd_GetMountPoint ) (
		PRL_HANDLE hVmDev,
		PRL_STR sMountPoint,
		PRL_UINT32_PTR pnMountPointBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pnMountPointBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return pDevice->GetMountPoint(sMountPoint, pnMountPointBufLength);
}

PRL_METHOD( PrlVmDevHd_SetStorageURL ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sURL
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sURL=%s)",
		__FUNCTION__,
		hVmDev,
		sURL
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->SetStorageURL(sURL));
}

PRL_METHOD( PrlVmDevHd_GetStorageURL ) (
		PRL_HANDLE hVmDev,
		PRL_STR sURL,
		PRL_UINT32_PTR pnURLBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sURL=%p, pnURLBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sURL,
		pnURLBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pnURLBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->GetStorageURL(sURL, pnURLBufLength));
}

PRL_METHOD( PrlVmDevHd_SetAutoCompressEnabled ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return pDevice->SetAutoCompressEnabled(bEnabled);
}

PRL_METHOD( PrlVmDevHd_IsAutoCompressEnabled ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return pDevice->IsAutoCompressEnabled(pbEnabled);
}

PRL_METHOD( PrlVmDevHd_GetSerialNumber ) (
		PRL_HANDLE hVmDev,
		PRL_STR sSerialNumber,
		PRL_UINT32_PTR pnSerialNumberBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sSerialNumber=%p, pnSerialNumberBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sSerialNumber,
		pnSerialNumberBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_HARD_DISK) ||
			PRL_WRONG_PTR(pnSerialNumberBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHardDrivePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHardDrive>(hVmDev);
	return (pDevice->GetSerialNumber(sSerialNumber, pnSerialNumberBufLength));
}

PRL_METHOD( PrlVmDevHdPart_Remove ) (
		PRL_HANDLE hPartition
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPartition=%p)",
		__FUNCTION__,
		hPartition
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPartition, PHT_VIRTUAL_DEV_HD_PARTITION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHdPartPtr pPartition = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHdPart>(hPartition);
	return pPartition->Remove();
}

PRL_METHOD( PrlVmDevHdPart_GetSysName ) (
		PRL_HANDLE hPartition,
		PRL_STR	sSysName,
		PRL_UINT32_PTR pnSysNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPartition=%p, sSysName=%p, pnSysNameBufLength=%p)",
		__FUNCTION__,
		hPartition,
		sSysName,
		pnSysNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPartition, PHT_VIRTUAL_DEV_HD_PARTITION) ||
			PRL_WRONG_PTR(pnSysNameBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHdPartPtr pPartition = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHdPart>(hPartition);
	return pPartition->GetSysName(sSysName, pnSysNameBufLength);
}

PRL_METHOD( PrlVmDevHdPart_SetSysName ) (
		PRL_HANDLE hPartition,
		PRL_CONST_STR sSysName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPartition=%p, sSysName=%s)",
		__FUNCTION__,
		hPartition,
		sSysName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPartition, PHT_VIRTUAL_DEV_HD_PARTITION) ||
			PRL_WRONG_PTR(sSysName))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceHdPartPtr pPartition = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceHdPart>(hPartition);
	return pPartition->SetSysName(sSysName);
}

PRL_METHOD( PrlVmDev_IsPassthrough ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbPassthrough
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbPassthrough=%p)",
		__FUNCTION__,
		hVmDev,
		pbPassthrough
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev) ||	PRL_WRONG_PTR(pbPassthrough))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->IsPassthrough(pbPassthrough));
}

PRL_METHOD( PrlVmDev_SetPassthrough ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bPassthrough
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bPassthrough=%.8X)",
		__FUNCTION__,
		hVmDev,
		bPassthrough
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_VM_DEVICE_HANDLE(hVmDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDev);
	return (pDevice->SetPassthrough(bPassthrough));
}

PRL_METHOD( PrlVmDevNet_GetBoundAdapterIndex ) (
		PRL_HANDLE hVmDev,
		PRL_INT32_PTR pnIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnIndex=%p)",
		__FUNCTION__,
		hVmDev,
		pnIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnIndex))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetBoundAdapterIndex(pnIndex));
}

PRL_METHOD( PrlVmDevNet_SetBoundAdapterIndex ) (
		PRL_HANDLE hVmDev,
		PRL_INT32 nIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nIndex=%.8X)",
		__FUNCTION__,
		hVmDev,
		nIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->SetBoundAdapterIndex(nIndex));
}

PRL_METHOD( PrlVmDevNet_GetBoundAdapterName ) (
		PRL_HANDLE hVmDev,
		PRL_STR sBoundAdapterName,
		PRL_UINT32_PTR pnBoundAdapterNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sBoundAdapterName=%p, pnBoundAdapterNameBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sBoundAdapterName,
		pnBoundAdapterNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnBoundAdapterNameBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetBoundAdapterName(sBoundAdapterName, pnBoundAdapterNameBufLength));
}

PRL_METHOD( PrlVmDevNet_SetBoundAdapterName ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewBoundAdapterName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewBoundAdapterName=%s)",
		__FUNCTION__,
		hVmDev,
		sNewBoundAdapterName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(sNewBoundAdapterName))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->SetBoundAdapterName(sNewBoundAdapterName));
}

PRL_METHOD( PrlVmDevNet_GetHostInterfaceName ) (
		PRL_HANDLE hVmDev,
		PRL_STR sHostInterfaceName,
		PRL_UINT32_PTR pnHostInterfaceNameBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnHostInterfaceNameBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetHostInterfaceName(sHostInterfaceName, pnHostInterfaceNameBufLength));
}

PRL_METHOD( PrlVmDevNet_SetHostInterfaceName ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewHostInterfaceName
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(sNewHostInterfaceName))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->SetHostInterfaceName(sNewHostInterfaceName));
}

PRL_METHOD( PrlVmDevNet_GetMacAddress ) (
		PRL_HANDLE hVmDev,
		PRL_STR sMacAddress,
		PRL_UINT32_PTR pnMacAddressBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sMacAddress=%p, pnMacAddressBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sMacAddress,
		pnMacAddressBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnMacAddressBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetMacAddress(sMacAddress, pnMacAddressBufLength));
}

PRL_METHOD( PrlVmDevNet_GetMacAddressCanonical ) (
		PRL_HANDLE hVmDev,
		PRL_STR sMacAddress,
		PRL_UINT32_PTR pnMacAddressBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnMacAddressBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetMacAddress(sMacAddress, pnMacAddressBufLength, true));
}

PRL_METHOD( PrlVmDevNet_SetMacAddress ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewMacAddress
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewMacAddress=%s)",
		__FUNCTION__,
		hVmDev,
		sNewMacAddress
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(sNewMacAddress))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->SetMacAddress(sNewMacAddress));
}

PRL_METHOD( PrlVmDevNet_GenerateMacAddr ) (
		PRL_HANDLE hVmDev
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p)",
		__FUNCTION__,
		hVmDev
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GenerateMacAddress());
}

PRL_METHOD( PrlVmDevNet_IsAutoApply ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbAutoApply
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbAutoApply=%p)",
		__FUNCTION__,
		hVmDev,
		pbAutoApply
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(pbAutoApply))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->IsAutoApply(pbAutoApply));
}


PRL_METHOD( PrlVmDevNet_SetAutoApply ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bAutoApply
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bAutoApply=%.8X)",
		__FUNCTION__,
		hVmDev,
		bAutoApply
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->SetAutoApply(bAutoApply));
}

PRL_METHOD( PrlVmDevNet_GetDefaultGateway ) (
		PRL_HANDLE hVmDev,
		PRL_STR sDefaultGateway,
		PRL_UINT32_PTR pnDefaultGatewayBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sDefaultGateway=%p, pnDefaultGatewayBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sDefaultGateway,
		pnDefaultGatewayBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnDefaultGatewayBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetDefaultGateway(sDefaultGateway, pnDefaultGatewayBufLength));
}

PRL_METHOD( PrlVmDevNet_SetDefaultGateway ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewDefaultGateway
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewDefaultGateway=%s)",
		__FUNCTION__,
		hVmDev,
		sNewDefaultGateway
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(sNewDefaultGateway))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->SetDefaultGateway(sNewDefaultGateway));
}

PRL_METHOD( PrlVmDevNet_GetDefaultGatewayIPv6 ) (
		PRL_HANDLE hVmDev,
		PRL_STR sDefaultGateway,
		PRL_UINT32_PTR pnDefaultGatewayBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sDefaultGateway=%p, pnDefaultGatewayBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sDefaultGateway,
		pnDefaultGatewayBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnDefaultGatewayBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetDefaultGatewayIPv6(sDefaultGateway, pnDefaultGatewayBufLength));
}

PRL_METHOD( PrlVmDevNet_SetDefaultGatewayIPv6 ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewDefaultGateway
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewDefaultGateway=%s)",
		__FUNCTION__,
		hVmDev,
		sNewDefaultGateway
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(sNewDefaultGateway))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->SetDefaultGatewayIPv6(sNewDefaultGateway));
}

PRL_METHOD( PrlVmDevNet_GetVirtualNetworkId ) (
		PRL_HANDLE hVmDev,
		PRL_STR sVirtualNetworkId,
		PRL_UINT32_PTR pnVirtualNetworkIdBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sVirtualNetworkId=%p, pnVirtualNetworkIdBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sVirtualNetworkId,
		pnVirtualNetworkIdBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnVirtualNetworkIdBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetVirtualNetworkId(sVirtualNetworkId, pnVirtualNetworkIdBufLength));
}

PRL_METHOD( PrlVmDevNet_SetVirtualNetworkId ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewVirtualNetworkId
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewVirtualNetworkId=%s)",
		__FUNCTION__,
		hVmDev,
		sNewVirtualNetworkId
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->SetVirtualNetworkId(sNewVirtualNetworkId));
}

PRL_METHOD( PrlVmDevNet_GetNetAddresses ) (
		PRL_HANDLE hVmDev,
		PRL_HANDLE_PTR phNetAddressesList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, phNetAddressesList=%p)",
		__FUNCTION__,
		hVmDev,
		phNetAddressesList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(phNetAddressesList))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetNetAddresses(phNetAddressesList));
}

PRL_METHOD( PrlVmDevNet_SetNetAddresses ) (
		PRL_HANDLE hVmDev,
		PRL_HANDLE hNetAddressesList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, hNetAddressesList=%p)",
		__FUNCTION__,
		hVmDev,
		hNetAddressesList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	PrlHandleStringsListPtr pNetAddressesList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hNetAddressesList);
	return (pDevice->SetNetAddresses(pNetAddressesList.getHandle() ? pNetAddressesList->GetStringsList() : QStringList()));
}

PRL_METHOD( PrlVmDevNet_GetDnsServers ) (
		PRL_HANDLE hVmDev,
		PRL_HANDLE_PTR phDnsServersList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, phDnsServersList=%p)",
		__FUNCTION__,
		hVmDev,
		phDnsServersList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(phDnsServersList))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetDnsServers(phDnsServersList));
}

PRL_METHOD( PrlVmDevNet_SetDnsServers ) (
		PRL_HANDLE hVmDev,
		PRL_HANDLE hDnsServersList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, hDnsServersList=%p)",
		__FUNCTION__,
		hVmDev,
		hDnsServersList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	PrlHandleStringsListPtr pDnsServersList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hDnsServersList);
	return (pDevice->SetDnsServers(pDnsServersList.getHandle() ? pDnsServersList->GetStringsList() : QStringList()));
}

PRL_METHOD( PrlVmDevNet_GetSearchDomains ) (
		PRL_HANDLE hVmDev,
		PRL_HANDLE_PTR phSearchDomainsList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, phSearchDomainsList=%p)",
		__FUNCTION__,
		hVmDev,
		phSearchDomainsList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(phSearchDomainsList))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetSearchDomains(phSearchDomainsList));
}

PRL_METHOD( PrlVmDevNet_SetSearchDomains ) (
		PRL_HANDLE hVmDev,
		PRL_HANDLE hSearchDomainsList
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, hSearchDomainsList=%p)",
		__FUNCTION__,
		hVmDev,
		hSearchDomainsList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
		(PRL_INVALID_HANDLE != hSearchDomainsList && PRL_WRONG_HANDLE(hSearchDomainsList, PHT_STRINGS_LIST)))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	PrlHandleStringsListPtr pSearchDomainsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hSearchDomainsList);
	return (pDevice->SetSearchDomains(pSearchDomainsList.getHandle() ? pSearchDomainsList->GetStringsList() : QStringList()));
}

PRL_METHOD( PrlVmDevNet_IsConfigureWithDhcp ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbConfigureWithDhcp
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbConfigureWithDhcp=%p)",
		__FUNCTION__,
		hVmDev,
		pbConfigureWithDhcp
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(pbConfigureWithDhcp))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->IsConfigureWithDhcp(pbConfigureWithDhcp));
}


PRL_METHOD( PrlVmDevNet_SetConfigureWithDhcp ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bConfigureWithDhcp
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bConfigureWithDhcp=%.8X)",
		__FUNCTION__,
		hVmDev,
		bConfigureWithDhcp
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->SetConfigureWithDhcp(bConfigureWithDhcp));
}


PRL_METHOD( PrlVmDevNet_IsConfigureWithDhcpIPv6 ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbConfigureWithDhcp
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbConfigureWithDhcp=%p)",
		__FUNCTION__,
		hVmDev,
		pbConfigureWithDhcp
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(pbConfigureWithDhcp))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->IsConfigureWithDhcpIPv6(pbConfigureWithDhcp));
}


PRL_METHOD( PrlVmDevNet_SetConfigureWithDhcpIPv6 ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bConfigureWithDhcp
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bConfigureWithDhcp=%.8X)",
		__FUNCTION__,
		hVmDev,
		bConfigureWithDhcp
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->SetConfigureWithDhcpIPv6(bConfigureWithDhcp));
}


PRL_METHOD( PrlVmDevNet_IsPktFilterPreventMacSpoof ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbPktFilterPreventMacSpoof
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbPktFilterPreventMacSpoof=%p)",
		__FUNCTION__,
		hVmDev,
		pbPktFilterPreventMacSpoof
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(pbPktFilterPreventMacSpoof))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->IsPktFilterPreventMacSpoof(pbPktFilterPreventMacSpoof));
}


PRL_METHOD( PrlVmDevNet_SetPktFilterPreventMacSpoof ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bPktFilterPreventMacSpoof
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bPktFilterPreventMacSpoof =%.8X)",
		__FUNCTION__,
		hVmDev,
		bPktFilterPreventMacSpoof
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->SetPktFilterPreventMacSpoof(bPktFilterPreventMacSpoof));
}

PRL_METHOD( PrlVmDevNet_IsPktFilterPreventPromisc ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbPktFilterPreventPromisc
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbPktFilterPreventPromisc=%p)",
		__FUNCTION__,
		hVmDev,
		pbPktFilterPreventPromisc
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(pbPktFilterPreventPromisc))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->IsPktFilterPreventPromisc(pbPktFilterPreventPromisc));
}


PRL_METHOD( PrlVmDevNet_SetPktFilterPreventPromisc ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bPktFilterPreventPromisc
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bPktFilterPreventPromisc =%.8X)",
		__FUNCTION__,
		hVmDev,
		bPktFilterPreventPromisc
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->SetPktFilterPreventPromisc(bPktFilterPreventPromisc));
}

PRL_METHOD( PrlVmDevNet_IsPktFilterPreventIpSpoof ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbPktFilterPreventIpSpoof
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbPktFilterPreventIpSpoof=%p)",
		__FUNCTION__,
		hVmDev,
		pbPktFilterPreventIpSpoof
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) || PRL_WRONG_PTR(pbPktFilterPreventIpSpoof))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->IsPktFilterPreventIpSpoof(pbPktFilterPreventIpSpoof));
}


PRL_METHOD( PrlVmDevNet_SetPktFilterPreventIpSpoof ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bPktFilterPreventIpSpoof
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bPktFilterPreventIpSpoof =%.8X)",
		__FUNCTION__,
		hVmDev,
		bPktFilterPreventIpSpoof
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pNetAdapter->SetPktFilterPreventIpSpoof(bPktFilterPreventIpSpoof));
}

PRL_METHOD( PrlVmDevNet_GetAdapterType) (
		PRL_HANDLE hVmDev,
		PRL_VM_NET_ADAPTER_TYPE_PTR pnAdapterType
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER) ||
			PRL_WRONG_PTR(pnAdapterType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->GetAdapterType(pnAdapterType));
}

PRL_METHOD( PrlVmDevNet_SetAdapterType ) (
		PRL_HANDLE hVmDev,
		PRL_VM_NET_ADAPTER_TYPE nAdapterType
		)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pDevice->SetAdapterType(nAdapterType));
}

PRL_METHOD( PrlVmDevNet_IsFirewallEnabled ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmDev,
		pbEnabled
	);

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER)
		|| PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pVmDevNet = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>( hVmDev );

	return pVmDevNet->IsFirewallEnabled(pbEnabled);
}

PRL_METHOD( PrlVmDevNet_SetFirewallEnabled ) (
		PRL_HANDLE hVmDev,
		PRL_BOOL bEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmDev,
		bEnabled
	);

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pVmDevNet = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>( hVmDev );

	return pVmDevNet->SetFirewallEnabled(bEnabled);
}

PRL_METHOD( PrlVmDevNet_GetFirewallDefaultPolicy ) (
		PRL_HANDLE hVmDev,
		PRL_FIREWALL_DIRECTION nDirection,
		PRL_FIREWALL_POLICY_PTR pnPolicy
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnPolicy=%p)",
		__FUNCTION__,
		hVmDev,
		pnPolicy
	);

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER)
		|| PRL_WRONG_PTR(pnPolicy))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pVmDevNet = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>( hVmDev );

	return pVmDevNet->GetFirewallDefaultPolicy(nDirection, pnPolicy);
}

PRL_METHOD( PrlVmDevNet_SetFirewallDefaultPolicy ) (
		PRL_HANDLE hVmDev,
		PRL_FIREWALL_DIRECTION nDirection,
		PRL_FIREWALL_POLICY nPolicy
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nPolicy=%.8X)",
		__FUNCTION__,
		hVmDev,
		nPolicy
	);

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pVmDevNet = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>( hVmDev );

	return pVmDevNet->SetFirewallDefaultPolicy(nDirection, nPolicy);
}

PRL_METHOD( PrlVmDevNet_GetFirewallRuleList ) (
									PRL_HANDLE hVmDev,
									PRL_FIREWALL_DIRECTION nDirection,
									PRL_HANDLE_PTR phRuleList
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, phRuleList=%p)",
		__FUNCTION__,
		hVmDev,
		phRuleList
		);

	SYNC_CHECK_API_INITIALIZED

	if (   PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER)
		|| PRL_WRONG_PTR(phRuleList)
		)
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pVmDevNet = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pVmDevNet->GetFirewallRuleList(nDirection, phRuleList));
}


PRL_METHOD( PrlVmDevNet_SetFirewallRuleList ) (
									PRL_HANDLE hVmDev,
									PRL_FIREWALL_DIRECTION nDirection,
									PRL_HANDLE hRuleList
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, hRuleList=%p)",
		__FUNCTION__,
		hVmDev,
		hRuleList
		);

	SYNC_CHECK_API_INITIALIZED

	if (   PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_NET_ADAPTER)
		|| ( hRuleList != PRL_INVALID_HANDLE && PRL_WRONG_HANDLE(hRuleList, PHT_HANDLES_LIST) )
		|| PRL_WRONG_HANDLES_LIST(hRuleList, PHT_FIREWALL_RULE)
		)
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceNetAdapterPtr pVmDevNet = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceNetAdapter>(hVmDev);
	return (pVmDevNet->SetFirewallRuleList(nDirection, hRuleList));
}


PRL_METHOD( PrlFirewallRule_Create ) (
		PRL_HANDLE_PTR phRule
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (phRule=%p)",
		__FUNCTION__,
		phRule
	);

	if (PRL_WRONG_PTR(phRule))
		return PRL_ERR_INVALID_ARG;

	return PrlHandleFirewallRule::Create(phRule);
}

PRL_METHOD( PrlFirewallRule_GetLocalPort ) (
		PRL_HANDLE hRule,
		PRL_UINT32_PTR pnPort
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, pnPort=%p)",
		__FUNCTION__,
		hRule,
		pnPort
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE)
		|| PRL_WRONG_PTR(pnPort))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->GetLocalPort(pnPort);
}

PRL_METHOD( PrlFirewallRule_SetLocalPort ) (
		PRL_HANDLE hRule,
		PRL_UINT32 nPort
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, nPort=%.8X)",
		__FUNCTION__,
		hRule,
		nPort
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->SetLocalPort(nPort);
}

PRL_METHOD( PrlFirewallRule_GetRemotePort ) (
		PRL_HANDLE hRule,
		PRL_UINT32_PTR pnPort
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, pnPort=%p)",
		__FUNCTION__,
		hRule,
		pnPort
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE)
		|| PRL_WRONG_PTR(pnPort))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->GetRemotePort(pnPort);
}

PRL_METHOD( PrlFirewallRule_SetRemotePort ) (
		PRL_HANDLE hRule,
		PRL_UINT32 nPort
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, nPort=%.8X)",
		__FUNCTION__,
		hRule,
		nPort
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->SetRemotePort(nPort);
}

PRL_METHOD( PrlFirewallRule_GetProtocol ) (
		PRL_HANDLE hRule,
		PRL_STR sProtocol,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, sProtocol=%p, pnBufLength=%p)",
		__FUNCTION__,
		hRule,
		sProtocol,
		pnBufLength
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE)
		|| PRL_WRONG_PTR(pnBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->GetProtocol(sProtocol, pnBufLength);
}

PRL_METHOD( PrlFirewallRule_SetProtocol ) (
		PRL_HANDLE hRule,
		PRL_CONST_STR sProtocol
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, sProtocol=%s)",
		__FUNCTION__,
		hRule,
		sProtocol
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE)
		|| PRL_WRONG_PTR(sProtocol))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->SetProtocol(sProtocol);
}

PRL_METHOD( PrlFirewallRule_GetLocalNetAddress ) (
		PRL_HANDLE hRule,
		PRL_STR sAddr,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, sAddr=%p, pnBufLength=%p)",
		__FUNCTION__,
		hRule,
		sAddr,
		pnBufLength
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE)
		|| PRL_WRONG_PTR(pnBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->GetLocalNetAddress(sAddr, pnBufLength);
}

PRL_METHOD( PrlFirewallRule_SetLocalNetAddress ) (
		PRL_HANDLE hRule,
		PRL_CONST_STR sAddr
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, sAddr=%s)",
		__FUNCTION__,
		hRule,
		sAddr
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE)
		|| PRL_WRONG_PTR(sAddr))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->SetLocalNetAddress(sAddr);
}

PRL_METHOD( PrlFirewallRule_GetRemoteNetAddress ) (
		PRL_HANDLE hRule,
		PRL_STR sAddr,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, sAddr=%p, pnBufLength=%p)",
		__FUNCTION__,
		hRule,
		sAddr,
		pnBufLength
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE)
		|| PRL_WRONG_PTR(pnBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->GetRemoteNetAddress(sAddr, pnBufLength);
}

PRL_METHOD( PrlFirewallRule_SetRemoteNetAddress ) (
		PRL_HANDLE hRule,
		PRL_CONST_STR sAddr
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hRule=%p, sAddr=%s)",
		__FUNCTION__,
		hRule,
		sAddr
	);

	if (PRL_WRONG_HANDLE(hRule, PHT_FIREWALL_RULE)
		|| PRL_WRONG_PTR(sAddr))
		return PRL_ERR_INVALID_ARG;

	PrlHandleFirewallRulePtr pFirewallRule = PRL_OBJECT_BY_HANDLE<PrlHandleFirewallRule>( hRule );

	return pFirewallRule->SetRemoteNetAddress(sAddr);
}


PRL_METHOD( PrlVmDevUsb_GetAutoconnectOption ) (
		PRL_HANDLE hVmDev,
		PRL_USB_DEVICE_AUTO_CONNECT_OPTION_PTR pnAutoconnectOption
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnAutoconnectOption=%p)",
		__FUNCTION__,
		hVmDev,
		pnAutoconnectOption
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_USB_DEVICE) ||
			PRL_WRONG_PTR(pnAutoconnectOption))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceUsbPortPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceUsbPort>(hVmDev);
	return (pDevice->GetAutoconnectOption(pnAutoconnectOption));
}

PRL_METHOD( PrlVmDevUsb_SetAutoconnectOption ) (
		PRL_HANDLE hVmDev,
		PRL_USB_DEVICE_AUTO_CONNECT_OPTION nAutoconnectOption
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nAutoconnectOption=%.8X)",
		__FUNCTION__,
		hVmDev,
		nAutoconnectOption
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_USB_DEVICE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceUsbPortPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceUsbPort>(hVmDev);
	return (pDevice->SetAutoconnectOption(nAutoconnectOption));
}

PRL_METHOD( PrlVmDevSound_GetOutputDev ) (
		PRL_HANDLE hVmDev,
		PRL_STR sOutputDev,
		PRL_UINT32_PTR pnOutputDevBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sOutputDev=%p, pnOutputDevBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sOutputDev,
		pnOutputDevBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_SOUND) ||
			PRL_WRONG_PTR(pnOutputDevBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceSoundPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceSound>(hVmDev);
	return (pDevice->GetOutputDev(sOutputDev, pnOutputDevBufLength));
}

PRL_METHOD( PrlVmDevSound_SetOutputDev ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewOutputDev
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewOutputDev=%s)",
		__FUNCTION__,
		hVmDev,
		sNewOutputDev
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_SOUND) || PRL_WRONG_PTR(sNewOutputDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceSoundPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceSound>(hVmDev);
	return (pDevice->SetOutputDev(sNewOutputDev));
}


PRL_METHOD( PrlVmDevSound_GetMixerDev ) (
		PRL_HANDLE hVmDev,
		PRL_STR sMixerDev,
		PRL_UINT32_PTR pnMixerDevBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sMixerDev=%p, pnMixerDevBufLength=%p)",
		__FUNCTION__,
		hVmDev,
		sMixerDev,
		pnMixerDevBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_SOUND) ||
			PRL_WRONG_PTR(pnMixerDevBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceSoundPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceSound>(hVmDev);
	return (pDevice->GetMixerDev(sMixerDev, pnMixerDevBufLength));
}

PRL_METHOD( PrlVmDevSound_SetMixerDev ) (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewMixerDev
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, sNewMixerDev=%s)",
		__FUNCTION__,
		hVmDev,
		sNewMixerDev
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_SOUND) || PRL_WRONG_PTR(sNewMixerDev))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceSoundPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceSound>(hVmDev);
	return (pDevice->SetMixerDev(sNewMixerDev));
}


PRL_METHOD( PrlVmDevSerial_GetSocketMode ) (
		PRL_HANDLE hVmDev,
		PRL_SERIAL_PORT_SOCKET_OPERATION_MODE_PTR pnSocketMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, pnSocketMode=%p)",
		__FUNCTION__,
		hVmDev,
		pnSocketMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_SERIAL_PORT) || PRL_WRONG_PTR(pnSocketMode))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceSerialPortPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceSerialPort>(hVmDev);
	return (pDevice->GetSocketMode(pnSocketMode));
}

PRL_METHOD( PrlVmDevSerial_SetSocketMode ) (
		PRL_HANDLE hVmDev,
		PRL_SERIAL_PORT_SOCKET_OPERATION_MODE nSocketMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDev=%p, nSocketMode=%.8X)",
		__FUNCTION__,
		hVmDev,
		nSocketMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmDev, PHT_VIRTUAL_DEV_SERIAL_PORT))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmDeviceSerialPortPtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDeviceSerialPort>(hVmDev);
	return (pDevice->SetSocketMode(nSocketMode));
}

PRL_METHOD( PrlShare_Remove ) (
		PRL_HANDLE hVmShare
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p)",
		__FUNCTION__,
		hVmShare
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmSharePtr pShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>(hVmShare);
	return (pShare->Remove());
}


PRL_METHOD( PrlShare_GetName) (
		PRL_HANDLE hVmShare,
		PRL_STR sShareName,
		PRL_UINT32_PTR pnShareNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, sShareName=%p, pnShareNameBufLength=%p)",
		__FUNCTION__,
		hVmShare,
		sShareName,
		pnShareNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE) ||
			PRL_WRONG_PTR(pnShareNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmSharePtr pVmShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>( hVmShare );

	return (pVmShare->GetName(sShareName, pnShareNameBufLength));
}


PRL_METHOD( PrlShare_SetName) (
		PRL_HANDLE hVmShare,
		PRL_CONST_STR sNewShareName
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, sNewShareName=%s)",
		__FUNCTION__,
		hVmShare,
		sNewShareName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE) ||
			PRL_WRONG_PTR(sNewShareName))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmSharePtr pVmShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>( hVmShare );

	return (pVmShare->SetName(sNewShareName));
}


PRL_METHOD( PrlShare_GetPath) (
		PRL_HANDLE hVmShare,
		PRL_STR sSharePath,
		PRL_UINT32_PTR pnSharePathBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, sSharePath=%p, pnSharePathBufLength=%p)",
		__FUNCTION__,
		hVmShare,
		sSharePath,
		pnSharePathBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE) ||
			PRL_WRONG_PTR(pnSharePathBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmSharePtr pVmShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>( hVmShare );

	return (pVmShare->GetPath(sSharePath, pnSharePathBufLength));
}


PRL_METHOD( PrlShare_SetPath) (
		PRL_HANDLE hVmShare,
		PRL_CONST_STR sNewSharePath
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, sNewSharePath=%s)",
		__FUNCTION__,
		hVmShare,
		sNewSharePath
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE) ||
			PRL_WRONG_PTR(sNewSharePath))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmSharePtr pVmShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>( hVmShare );

	return (pVmShare->SetPath(sNewSharePath));
}


PRL_METHOD( PrlShare_GetDescription) (
		PRL_HANDLE hVmShare,
		PRL_STR sShareDescription,
		PRL_UINT32_PTR pnShareDescriptionBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, sShareDescription=%p, pnShareDescriptionBufLength=%p)",
		__FUNCTION__,
		hVmShare,
		sShareDescription,
		pnShareDescriptionBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE) ||
			PRL_WRONG_PTR(pnShareDescriptionBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmSharePtr pVmShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>( hVmShare );

	return (pVmShare->GetDescription(sShareDescription, pnShareDescriptionBufLength));
}


PRL_METHOD( PrlShare_SetDescription) (
		PRL_HANDLE hVmShare,
		PRL_CONST_STR sNewShareDescription
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, sNewShareDescription=%s)",
		__FUNCTION__,
		hVmShare,
		sNewShareDescription
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE) ||
			PRL_WRONG_PTR(sNewShareDescription))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmSharePtr pVmShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>( hVmShare );

	return (pVmShare->SetDescription(sNewShareDescription));
}


PRL_METHOD( PrlShare_IsEnabled ) (
		PRL_HANDLE hVmShare,
		PRL_BOOL_PTR pbEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, pbEnabled=%p)",
		__FUNCTION__,
		hVmShare,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE) || PRL_WRONG_PTR(pbEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmSharePtr pShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>(hVmShare);
	return (pShare->IsEnabled(pbEnabled));
}


PRL_METHOD( PrlShare_SetEnabled ) (
		PRL_HANDLE hVmShare,
		PRL_BOOL bEnabled
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, bEnabled=%.8X)",
		__FUNCTION__,
		hVmShare,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmSharePtr pShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>(hVmShare);
	return (pShare->SetEnabled(bEnabled));
}

PRL_METHOD( PrlShare_IsReadOnly ) (
		PRL_HANDLE hVmShare,
		PRL_BOOL_PTR pbReadOnly
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, pbReadOnly=%p)",
		__FUNCTION__,
		hVmShare,
		pbReadOnly
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE) || PRL_WRONG_PTR(pbReadOnly))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmSharePtr pShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>(hVmShare);
	return (pShare->IsReadOnly(pbReadOnly));
}


PRL_METHOD( PrlShare_SetReadOnly ) (
		PRL_HANDLE hVmShare,
		PRL_BOOL bReadOnly
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmShare=%p, bReadOnly=%.8X)",
		__FUNCTION__,
		hVmShare,
		bReadOnly
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmShare, PHT_SHARE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmSharePtr pShare = PRL_OBJECT_BY_HANDLE<PrlHandleVmShare>(hVmShare);
	return (pShare->SetReadOnly(bReadOnly));
}

PRL_METHOD( PrlBootDev_Remove ) (
		PRL_HANDLE hVmBootDev
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmBootDev=%p)",
		__FUNCTION__,
		hVmBootDev
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmBootDev, PHT_BOOT_DEVICE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmBootDevPtr pBootDev = PRL_OBJECT_BY_HANDLE<PrlHandleVmBootDev>(hVmBootDev);
	return (pBootDev->Remove());
}

PRL_METHOD( PrlBootDev_GetType ) (
		PRL_HANDLE hVmBootDev,
		PRL_DEVICE_TYPE_PTR pnDevType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmBootDev=%p, pnDevType=%p)",
		__FUNCTION__,
		hVmBootDev,
		pnDevType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmBootDev, PHT_BOOT_DEVICE) || PRL_WRONG_PTR(pnDevType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmBootDevPtr pBootDev = PRL_OBJECT_BY_HANDLE<PrlHandleVmBootDev>(hVmBootDev);
	return (pBootDev->GetBootDevType(pnDevType));
}

PRL_METHOD( PrlBootDev_SetType ) (
		PRL_HANDLE hVmBootDev,
		PRL_DEVICE_TYPE nDevType
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmBootDev=%p, nDevType=%.8X)",
		__FUNCTION__,
		hVmBootDev,
		nDevType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmBootDev, PHT_BOOT_DEVICE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmBootDevPtr pBootDev = PRL_OBJECT_BY_HANDLE<PrlHandleVmBootDev>(hVmBootDev);
	return (pBootDev->SetType(nDevType));
}

PRL_METHOD( PrlBootDev_GetIndex ) (
		PRL_HANDLE hVmBootDev,
		PRL_UINT32_PTR pnDevIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmBootDev=%p, pnDevIndex=%p)",
		__FUNCTION__,
		hVmBootDev,
		pnDevIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmBootDev, PHT_BOOT_DEVICE) || PRL_WRONG_PTR(pnDevIndex))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmBootDevPtr pBootDev = PRL_OBJECT_BY_HANDLE<PrlHandleVmBootDev>(hVmBootDev);
	return (pBootDev->GetIndex(pnDevIndex));
}

PRL_METHOD( PrlBootDev_SetIndex ) (
		PRL_HANDLE hVmBootDev,
		PRL_UINT32 nDevIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmBootDev=%p, nDevIndex=%.8X)",
		__FUNCTION__,
		hVmBootDev,
		nDevIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmBootDev, PHT_BOOT_DEVICE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmBootDevPtr pBootDev = PRL_OBJECT_BY_HANDLE<PrlHandleVmBootDev>(hVmBootDev);
	return (pBootDev->SetIndex(nDevIndex));
}

PRL_METHOD( PrlBootDev_GetSequenceIndex ) (
		PRL_HANDLE hVmBootDev,
		PRL_UINT32_PTR pnSequenceIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmBootDev=%p, pnSequenceIndex=%p)",
		__FUNCTION__,
		hVmBootDev,
		pnSequenceIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmBootDev, PHT_BOOT_DEVICE) || PRL_WRONG_PTR(pnSequenceIndex))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmBootDevPtr pBootDev = PRL_OBJECT_BY_HANDLE<PrlHandleVmBootDev>(hVmBootDev);
	return (pBootDev->GetSequenceIndex(pnSequenceIndex));
}

PRL_METHOD( PrlBootDev_SetSequenceIndex ) (
		PRL_HANDLE hVmBootDev,
		PRL_UINT32 nSequenceIndex
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmBootDev=%p, nSequenceIndex=%.8X)",
		__FUNCTION__,
		hVmBootDev,
		nSequenceIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmBootDev, PHT_BOOT_DEVICE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmBootDevPtr pBootDev = PRL_OBJECT_BY_HANDLE<PrlHandleVmBootDev>(hVmBootDev);
	return (pBootDev->SetSequenceIndex(nSequenceIndex));
}

PRL_METHOD( PrlBootDev_IsInUse ) (
		PRL_HANDLE hVmBootDev,
		PRL_BOOL_PTR pbInUse
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmBootDev=%p, pbInUse=%p)",
		__FUNCTION__,
		hVmBootDev,
		pbInUse
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmBootDev, PHT_BOOT_DEVICE) || PRL_WRONG_PTR(pbInUse))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmBootDevPtr pBootDev = PRL_OBJECT_BY_HANDLE<PrlHandleVmBootDev>(hVmBootDev);
	return (pBootDev->IsInUse(pbInUse));
}

PRL_METHOD( PrlBootDev_SetInUse ) (
		PRL_HANDLE hVmBootDev,
		PRL_BOOL bInUse
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmBootDev=%p, bInUse=%.8X)",
		__FUNCTION__,
		hVmBootDev,
		bInUse
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hVmBootDev, PHT_BOOT_DEVICE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmBootDevPtr pBootDev = PRL_OBJECT_BY_HANDLE<PrlHandleVmBootDev>(hVmBootDev);
	return (pBootDev->SetInUse(bInUse));
}

PRL_METHOD( PrlFoundVmInfo_GetName ) (
		 PRL_HANDLE handle,
		 PRL_STR sName,
		 PRL_UINT32_PTR pnNameBufLength
		 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, sName=%p, pnNameBufLength=%p",
		__FUNCTION__,
		handle,
		sName,
		pnNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_FOUND_VM_INFO) || PRL_WRONG_PTR(pnNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleFoundVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleFoundVmInfo>( handle );
	return (pVmInfo->GetName(sName, pnNameBufLength));
}

PRL_METHOD( PrlFoundVmInfo_IsOldConfig ) (
		 PRL_HANDLE handle,
		 PRL_BOOL_PTR pbOldVersion
		 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pbOldVersion=%p)",
		__FUNCTION__,
		handle,
		pbOldVersion
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_FOUND_VM_INFO) || PRL_WRONG_PTR(pbOldVersion) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleFoundVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleFoundVmInfo>( handle );
	return (pVmInfo->IsOldConfig(pbOldVersion));
}

PRL_METHOD( PrlFoundVmInfo_GetOSVersion ) (
		PRL_HANDLE handle,
		PRL_UINT32_PTR pnOSVersion
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pnOSVersion=%p)",
		__FUNCTION__,
		handle,
		pnOSVersion
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_FOUND_VM_INFO) || PRL_WRONG_PTR(pnOSVersion) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleFoundVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleFoundVmInfo>( handle );
	return (pVmInfo->GetOSVersion(pnOSVersion));
}

PRL_METHOD( PrlFoundVmInfo_GetConfigPath ) (
		   PRL_HANDLE handle,
		   PRL_STR sPath,
		   PRL_UINT32_PTR pnPathBufLength
		 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, sPath=%p, pnPathBufLength=%p",
		__FUNCTION__,
		handle,
		sPath,
		pnPathBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_FOUND_VM_INFO) || PRL_WRONG_PTR(pnPathBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleFoundVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleFoundVmInfo>( handle );
	return (pVmInfo->GetConfigPath(sPath, pnPathBufLength));
}

PRL_METHOD( PrlFoundVmInfo_IsTemplate ) (
	PRL_HANDLE handle,
	PRL_BOOL_PTR pbIsTemplate
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pbIsTemplate=%p)",
		__FUNCTION__,
		handle,
		pbIsTemplate
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_FOUND_VM_INFO) || PRL_WRONG_PTR(pbIsTemplate) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleFoundVmInfoPtr pVmInfo = PRL_OBJECT_BY_HANDLE<PrlHandleFoundVmInfo>( handle );
	return (pVmInfo->IsTemplate(pbIsTemplate));
}

PRL_METHOD( PrlVmToolsInfo_GetState) (
		PRL_HANDLE handle,
		PRL_VM_TOOLS_STATE_PTR pVmToolsState
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, pVmToolsState=%p)",
		__FUNCTION__,
		handle,
		pVmToolsState
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_VM_TOOLS_INFO) || PRL_WRONG_PTR(pVmToolsState) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleVmToolsInfoPtr pVmToolsInfo = PRL_OBJECT_BY_HANDLE<PrlHandleVmToolsInfo>( handle );
	return (pVmToolsInfo->GetState(pVmToolsState));
}

PRL_METHOD( PrlVmToolsInfo_GetVersion ) (
		 PRL_HANDLE handle,
		 PRL_STR sName,
		 PRL_UINT32_PTR pnNameBufLength
		 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p, sName=%p, pnNameBufLength=%p",
		__FUNCTION__,
		handle,
		sName,
		pnNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(handle, PHT_VM_TOOLS_INFO) || PRL_WRONG_PTR(pnNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleVmToolsInfoPtr pVmToolsInfo = PRL_OBJECT_BY_HANDLE<PrlHandleVmToolsInfo>( handle );
	return (pVmToolsInfo->GetVersion(sName, pnNameBufLength));
}

PRL_METHOD( PrlAcl_IsAllowed ) (
		PRL_HANDLE hAcl,
		PRL_ALLOWED_VM_COMMAND nPermission,
		PRL_BOOL_PTR pbAllowed
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hAcl=%p, nPermission=%.8X, pbAllowed=%p",
		__FUNCTION__,
		hAcl,
		nPermission,
		pbAllowed
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hAcl, PHT_ACCESS_RIGHTS) || PRL_WRONG_PTR(pbAllowed) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleAccessRightsPtr pAccessRights = PRL_OBJECT_BY_HANDLE<PrlHandleAccessRights>( hAcl );
	return (pAccessRights->IsAllowed(nPermission, pbAllowed));
}

PRL_METHOD( PrlAcl_GetAccessForOthers ) (
		PRL_HANDLE hAcl,
		PRL_VM_ACCESS_FOR_OTHERS_PTR pnAccessForOthers
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hAcl=%p, pnAccessForOthers=%p",
		__FUNCTION__,
		hAcl,
		pnAccessForOthers
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hAcl, PHT_ACCESS_RIGHTS) || PRL_WRONG_PTR(pnAccessForOthers) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleAccessRightsPtr pAccessRights = PRL_OBJECT_BY_HANDLE<PrlHandleAccessRights>( hAcl );
	return (pAccessRights->GetAccessForOthers(pnAccessForOthers));
}

PRL_METHOD( PrlAcl_SetAccessForOthers ) (
		PRL_HANDLE hAcl,
		PRL_VM_ACCESS_FOR_OTHERS nAccessForOthers
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hAcl=%p, nAccessForOthers=%.8X",
		__FUNCTION__,
		hAcl,
		nAccessForOthers
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hAcl, PHT_ACCESS_RIGHTS) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleAccessRightsPtr pAccessRights = PRL_OBJECT_BY_HANDLE<PrlHandleAccessRights>( hAcl );
	return (pAccessRights->SetAccessForOthers(nAccessForOthers));
}

PRL_METHOD( PrlAcl_GetOwnerName ) (
				PRL_HANDLE hAcl,
				PRL_STR sOwnerNameBuf,
				PRL_UINT32_PTR pnOwnerNameBufLength
				)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hAcl=%p, sOwnerNameBuf=%p, pnOwnerNameBufLength=%p",
		__FUNCTION__,
		hAcl,
		sOwnerNameBuf,
		pnOwnerNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hAcl, PHT_ACCESS_RIGHTS) || PRL_WRONG_PTR(pnOwnerNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleAccessRightsPtr pAccessRights = PRL_OBJECT_BY_HANDLE<PrlHandleAccessRights>( hAcl );
	return (pAccessRights->GetOwnerName(sOwnerNameBuf, pnOwnerNameBufLength));
}

PRL_METHOD( PrlAcl_IsCurrentSessionOwner ) (
		PRL_HANDLE hAcl,
		PRL_BOOL_PTR pbIsOwner
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hAcl=%p, pbIsOwner=%p",
		__FUNCTION__,
		hAcl,
		pbIsOwner
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hAcl, PHT_ACCESS_RIGHTS) || PRL_WRONG_PTR(pbIsOwner) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleAccessRightsPtr pAccessRights = PRL_OBJECT_BY_HANDLE<PrlHandleAccessRights>( hAcl );
	return (pAccessRights->IsCurrentSessionOwner(pbIsOwner));
}

PRL_METHOD( PrlStat_GetVmDataStat) (
		PRL_HANDLE hStat,
		PRL_HANDLE_PTR phVmDataStat
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStat=%p, phVmDataStat=%p)",
		__FUNCTION__,
		hStat,
		phVmDataStat
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hStat, PHT_SYSTEM_STATISTICS) || PRL_WRONG_PTR(phVmDataStat) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleSystemStatisticsPtr pSystemStatistics = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatistics>( hStat );
	return (pSystemStatistics->GetVmDataStat(phVmDataStat));
}

PRL_METHOD( PrlStatVmData_GetSegmentCapacity) (
		PRL_HANDLE hVmDataStat,
		PRL_DATA_STATISTIC_SEGMENTS nSegment,
		PRL_UINT64_PTR pnCapacity
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hVmDataStat=%p, nSegment=%.8X, pnCapacity=%p)",
		__FUNCTION__,
		hVmDataStat,
		nSegment,
		pnCapacity
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmDataStat, PHT_SYSTEM_STATISTICS_VM_DATA)
		|| (nSegment <= PDSS_UNKNOWN || nSegment > PDSS_VM_FULL_SPACE)
		|| PRL_WRONG_PTR(pnCapacity) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSystemStatisticsVmDataPtr pSystemStatisticsVmData = PRL_OBJECT_BY_HANDLE<PrlHandleSystemStatisticsVmData>( hVmDataStat );
	return (pSystemStatisticsVmData->GetSegmentCapacity(nSegment, pnCapacity));
}

PRL_ASYNC_METHOD( PrlVm_Compact ) (
	PRL_HANDLE hVm,
	PRL_UINT32 uMask,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_COMPACT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Compact, (hVm, uMask, nFlags))
}

PRL_ASYNC_METHOD( PrlVm_CancelCompact ) (
        PRL_HANDLE hVm
       )
{
    ASYNC_CHECK_API_INITIALIZED(PJOC_UNKNOWN)
    CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_CancelCompact, (hVm))
}


PRL_ASYNC_METHOD( PrlVm_ConvertDisks ) (
	PRL_HANDLE hVm,
	PRL_UINT32 uMask,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_CONVERT_DISKS)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_ConvertDisks, (hVm, uMask, nFlags))
}

PRL_ASYNC_METHOD( PrlVm_CancelConvertDisks ) (
	PRL_HANDLE hVm,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_CONVERT_DISKS)
	PRL_UINT32 nMask = 0;
	nFlags = (nFlags & PCVD_CANCEL);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_ConvertDisks, (hVm, nMask, nFlags))
}

PRL_ASYNC_METHOD( PrlVm_SubscribeToPerfStats ) (PRL_HANDLE hVm, PRL_CONST_STR sFilter)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_SUBSCRIBE_PERFSTATS) ;
    CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_SubscribeToPerfStats, (hVm, sFilter)) ;
}

PRL_ASYNC_METHOD( PrlVm_UnsubscribeFromPerfStats ) (PRL_HANDLE hVm)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_UNSUBSCRIBE_PERFSTATS) ;
    CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_UnsubscribeFromPerfStats, (hVm)) ;
}

PRL_ASYNC_METHOD( PrlVm_GetPerfStats ) (PRL_HANDLE hVm, PRL_CONST_STR sFilter)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_GET_PERFSTATS) ;
    CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_GetPerfStats, (hVm, sFilter)) ;
}

PRL_METHOD( PrlOsesMatrix_GetSupportedOsesTypes ) (
					PRL_HANDLE hOsesMatrix,
					PRL_HANDLE_PTR phOsesTypesList
		)
{
	LOG_MESSAGE( DBG_INFO, "%s (hOsesMatrix=%p, phOsesTypesList=%p)",
		__FUNCTION__,
		hOsesMatrix,
		phOsesTypesList
		);

	SYNC_CHECK_API_INITIALIZED

	// check input parameters validity
	if ( PRL_WRONG_HANDLE(hOsesMatrix, PHT_GUEST_OSES_MATRIX) || PRL_WRONG_PTR(phOsesTypesList) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleGuestOsesMatrixPtr pOsesMatrix = PRL_OBJECT_BY_HANDLE<PrlHandleGuestOsesMatrix>( hOsesMatrix );

	return (pOsesMatrix->GetSupportedOsesTypes(phOsesTypesList));
}

PRL_METHOD( PrlOsesMatrix_GetSupportedOsesVersions ) (
					PRL_HANDLE hOsesMatrix,
					PRL_UINT8 nGuestOsType,
					PRL_HANDLE_PTR phGuestsList
		)
{
	LOG_MESSAGE( DBG_INFO, "%s (hOsesMatrix=%p, nGuestOsType=%.2X, phGuestsList=%p)",
		__FUNCTION__,
		hOsesMatrix,
		nGuestOsType,
		phGuestsList
		);

	SYNC_CHECK_API_INITIALIZED

	// check input parameters validity
	if ( PRL_WRONG_HANDLE(hOsesMatrix, PHT_GUEST_OSES_MATRIX) || PRL_WRONG_PTR(phGuestsList) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleGuestOsesMatrixPtr pOsesMatrix = PRL_OBJECT_BY_HANDLE<PrlHandleGuestOsesMatrix>( hOsesMatrix );

	return (pOsesMatrix->GetSupportedOsesVersions(nGuestOsType, phGuestsList));
}

PRL_METHOD( PrlOsesMatrix_GetDefaultOsVersion ) (
					PRL_HANDLE hOsesMatrix,
					PRL_UINT8 nGuestOsType,
					PRL_UINT16_PTR pnOsVersion
		)
{
	LOG_MESSAGE( DBG_INFO, "%s (hOsesMatrix=%p, nGuestOsType=%.2X, pnOsVersion=%p)",
		__FUNCTION__,
		hOsesMatrix,
		nGuestOsType,
		pnOsVersion
		);

	SYNC_CHECK_API_INITIALIZED

	// check input parameters validity
	if ( PRL_WRONG_HANDLE(hOsesMatrix, PHT_GUEST_OSES_MATRIX) || PRL_WRONG_PTR(pnOsVersion) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleGuestOsesMatrixPtr pOsesMatrix = PRL_OBJECT_BY_HANDLE<PrlHandleGuestOsesMatrix>( hOsesMatrix );

	return (pOsesMatrix->GetDefaultOsVersion(nGuestOsType, pnOsVersion));
}

PRL_METHOD( PrlAppliance_Create) (
		PRL_HANDLE_PTR phAppCfg
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (phAppCfg=%p)",
		__FUNCTION__,
		phAppCfg
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_PTR(phAppCfg) )
		return PRL_ERR_INVALID_ARG;

	return PrlHandleApplianceConfig::Create(phAppCfg);
}

PRL_METHOD( PrlVmCfg_SetVmType ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_TYPE nType
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetVmType(nType));
}

PRL_METHOD( PrlVmCfg_GetVmType ) (
		PRL_HANDLE hVmCfg,
		PRL_VM_TYPE_PTR pnType
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetVmType(pnType));
}

PRL_METHOD( PrlVmCfg_SetOsTemplate ) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sOsTemplate
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetOstemplate(sOsTemplate));
}

PRL_METHOD( PrlVmCfg_GetOsTemplate ) (
		PRL_HANDLE hVmCfg,
		PRL_STR sOsTemplate,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED
	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) || PRL_WRONG_PTR(pnBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);

	return (pVm->GetOstemplate(sOsTemplate, pnBufLength));
}

PRL_METHOD( PrlVmCfg_ApplyConfigSample ) (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sConfigSample
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
	     PRL_WRONG_PTR(sConfigSample))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetApplyConfigSample(sConfigSample));
}

PRL_METHOD( PrlVmCfg_SetResource ) (
		PRL_HANDLE hVmCfg,
		PRL_CT_RESOURCE nResourceId,
		PRL_UINT64 nBarrirer,
		PRL_UINT64 nLimit
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->SetResourceById(nResourceId, nBarrirer, nLimit));
}

PRL_METHOD( PrlVmCfg_GetResource ) (
		PRL_HANDLE hVmCfg,
		PRL_CT_RESOURCE nResourceId,
		PRL_UINT64_PTR pnBarrirer,
		PRL_UINT64_PTR pnLimit
		)
{
	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) )
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>(hVmCfg);
	return (pVm->GetResourceById(nResourceId, pnBarrirer, pnLimit));
}

PRL_METHOD( PrlCtTemplate_GetName ) (
		PRL_HANDLE hTemplate,
		PRL_STR sName,
		PRL_UINT32_PTR pnNameLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hTemplate=%p, sName=%p, pnNameLength=%p)",
		__FUNCTION__,
		hTemplate,
		sName,
		pnNameLength
		);

	if (PRL_WRONG_HANDLE(hTemplate, PHT_CT_TEMPLATE) ||
			PRL_WRONG_PTR(pnNameLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCtTemplatePtr pCtTemplate = PRL_OBJECT_BY_HANDLE<PrlHandleCtTemplate>( hTemplate );

	return pCtTemplate->GetName(sName, pnNameLength);
}

PRL_METHOD( PrlCtTemplate_GetType ) (
		PRL_HANDLE hTemplate,
		PRL_CT_TEMPLATE_TYPE_PTR pType
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hTemplate=%p, pType=%p)",
		__FUNCTION__,
		hTemplate,
		pType
		);

	if (PRL_WRONG_HANDLE(hTemplate, PHT_CT_TEMPLATE) ||
			PRL_WRONG_PTR(pType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCtTemplatePtr pCtTemplate = PRL_OBJECT_BY_HANDLE<PrlHandleCtTemplate>( hTemplate );

	return pCtTemplate->GetTemplType(pType);
}

PRL_METHOD( PrlCtTemplate_GetDescription ) (
		PRL_HANDLE hTemplate,
		PRL_STR sDescription,
		PRL_UINT32_PTR pnDescriptionLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hTemplate=%p, sDescription=%p, pnDescriptionLength=%p)",
		__FUNCTION__,
		hTemplate,
		sDescription,
		pnDescriptionLength
		);

	if (PRL_WRONG_HANDLE(hTemplate, PHT_CT_TEMPLATE) ||
			PRL_WRONG_PTR(pnDescriptionLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCtTemplatePtr pCtTemplate = PRL_OBJECT_BY_HANDLE<PrlHandleCtTemplate>( hTemplate );

	return pCtTemplate->GetDescription(sDescription, pnDescriptionLength);
}

PRL_METHOD( PrlCtTemplate_GetVersion ) (
		PRL_HANDLE hTemplate,
		PRL_STR sVersion,
		PRL_UINT32_PTR pnVersionLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hTemplate=%p, sVersion=%p, pnVersionLength=%p)",
		__FUNCTION__,
		hTemplate,
		sVersion,
		pnVersionLength
		);

	if (PRL_WRONG_HANDLE(hTemplate, PHT_CT_TEMPLATE) ||
			PRL_WRONG_PTR(pnVersionLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCtTemplatePtr pCtTemplate = PRL_OBJECT_BY_HANDLE<PrlHandleCtTemplate>( hTemplate );

	return pCtTemplate->GetVersion(sVersion, pnVersionLength);
}

PRL_METHOD( PrlCtTemplate_GetOsType ) (
		PRL_HANDLE hTemplate,
		PRL_UINT32_PTR pnOsType
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hTemplate=%p, pnOsType=%p)",
		__FUNCTION__,
		hTemplate,
		pnOsType
		);

	if (PRL_WRONG_HANDLE(hTemplate, PHT_CT_TEMPLATE) ||
			PRL_WRONG_PTR(pnOsType))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCtTemplatePtr pCtTemplate = PRL_OBJECT_BY_HANDLE<PrlHandleCtTemplate>( hTemplate );

	return pCtTemplate->GetOsType(pnOsType);
}

PRL_METHOD( PrlCtTemplate_GetOsVersion ) (
		PRL_HANDLE hTemplate,
		PRL_UINT32_PTR pnOsVersion
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hTemplate=%p, pnOsVersion=%p)",
		__FUNCTION__,
		hTemplate,
		pnOsVersion
		);

	if (PRL_WRONG_HANDLE(hTemplate, PHT_CT_TEMPLATE) ||
			PRL_WRONG_PTR(pnOsVersion))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCtTemplatePtr pCtTemplate = PRL_OBJECT_BY_HANDLE<PrlHandleCtTemplate>( hTemplate );

	return pCtTemplate->GetOsVersion(pnOsVersion);
}

PRL_METHOD( PrlCtTemplate_GetCpuMode ) (
		PRL_HANDLE hTemplate,
		PRL_CPU_MODE_PTR pnCpuMode
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hTemplate=%p, pnCpuMode=%p)",
		__FUNCTION__,
		hTemplate,
		pnCpuMode
		);

	if (PRL_WRONG_HANDLE(hTemplate, PHT_CT_TEMPLATE) ||
			PRL_WRONG_PTR(pnCpuMode))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCtTemplatePtr pCtTemplate = PRL_OBJECT_BY_HANDLE<PrlHandleCtTemplate>( hTemplate );

	return pCtTemplate->GetCpuMode(pnCpuMode);
}

PRL_METHOD( PrlCtTemplate_IsCached ) (
		PRL_HANDLE hTemplate,
		PRL_BOOL_PTR pbCached
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hTemplate=%p, pbCached=%p)",
		__FUNCTION__,
		hTemplate,
		pbCached
		);

	if (PRL_WRONG_HANDLE(hTemplate, PHT_CT_TEMPLATE) ||
			PRL_WRONG_PTR(pbCached))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCtTemplatePtr pCtTemplate = PRL_OBJECT_BY_HANDLE<PrlHandleCtTemplate>( hTemplate );

	return pCtTemplate->IsCached(pbCached);
}

PRL_HANDLE PrlSrv_GetCtTemplateList_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_VM_METH_IMPLEMENTATION_WITH_FLAGS(GetCtTemplateList, nFlags,
		PJOC_SRV_GET_CT_TEMPLATE_LIST)
}

PRL_ASYNC_METHOD( PrlSrv_GetCtTemplateList ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_CT_TEMPLATE_LIST)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetCtTemplateList, (hServer, nFlags))
}

PRL_METHOD( PrlVmCfg_GetAppTemplateList ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phAppList
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, phAppList=%p)",
		__FUNCTION__,
		hVmCfg,
		phAppList
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		PRL_WRONG_PTR(phAppList))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->GetAppTemplateList(phAppList);
}

PRL_METHOD( PrlVmCfg_SetAppTemplateList ) (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hAppList
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, hAppList=%p)",
		__FUNCTION__,
		hVmCfg,
		hAppList
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		PRL_WRONG_HANDLE(hAppList, PHT_STRINGS_LIST))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetAppTemplateList(hAppList);
}

PRL_HANDLE PrlSrv_RemoveCtTemplate_Impl(PRL_HANDLE hServer,
	PRL_CONST_STR sName, PRL_CONST_STR sOsTmplName, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(sName))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_REMOVE_CT_TEMPLATE))

	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>(hServer);
	PrlHandleJobPtr pJob = pServer->RemoveCtTemplate(sName, sOsTmplName, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_RemoveCtTemplate ) (
		PRL_HANDLE hServer,
		PRL_CONST_STR sName,
		PRL_CONST_STR sOsTmplName,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_REMOVE_CT_TEMPLATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(),
		PrlSrv_RemoveCtTemplate, (hServer, sName, sOsTmplName, nFlags))
}

PRL_METHOD( PrlCtTemplate_GetOsTemplate ) (
		PRL_HANDLE hTemplate,
		PRL_STR sOsTemplate,
		PRL_UINT32_PTR pnOsTemplateLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hTemplate=%p, sOsTemplate=%p, pnOsTemplateLength=%p)",
		__FUNCTION__,
		hTemplate,
		sOsTemplate,
		pnOsTemplateLength
	);

	if (PRL_WRONG_HANDLE(hTemplate, PHT_CT_TEMPLATE))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCtTemplatePtr pCtTemplate = PRL_OBJECT_BY_HANDLE<PrlHandleCtTemplate>( hTemplate );

	return pCtTemplate->GetOsTemplate(sOsTemplate, pnOsTemplateLength);
}

PRL_HANDLE PrlSrv_CopyCtTemplate_Impl(
			PRL_HANDLE hSrv,
			PRL_CONST_STR sName,
			PRL_CONST_STR sOsTmplName,
			PRL_CONST_STR sTargetServerHostname,
			PRL_UINT32 nTargetServerPort,
			PRL_CONST_STR sTargetServerSessionUuid,
			PRL_UINT32 nFlags,
			PRL_UINT32 nReservedFlags)
{
	if (	PRL_WRONG_HANDLE(hSrv, PHT_SERVER) ||
		PRL_WRONG_PTR(sName) ||
		PRL_WRONG_PTR(sTargetServerHostname) ||
		PRL_WRONG_PTR(sTargetServerSessionUuid))
			RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_COPY_CT_TEMPLATE))

	PrlHandleServerVmPtr pSrv = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>(hSrv);
	PrlHandleJobPtr pJob = pSrv->CopyCtTemplate(
			sName,
			sOsTmplName,
			sTargetServerHostname,
			nTargetServerPort,
			sTargetServerSessionUuid,
			nFlags,
			nReservedFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_CopyCtTemplate ) (
			PRL_HANDLE hSrv,
			PRL_CONST_STR sName,
			PRL_CONST_STR sOsTmplName,
			PRL_CONST_STR sTargetServerHostname,
			PRL_UINT32 nTargetServerPort,
			PRL_CONST_STR sTargetServerSessionUuid,
			PRL_UINT32 nFlags,
			PRL_UINT32 nReservedFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_COPY_CT_TEMPLATE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_CopyCtTemplate, (
			hSrv,
			sName,
			sOsTmplName,
			sTargetServerHostname,
			nTargetServerPort,
			sTargetServerSessionUuid,
			nFlags,
			nReservedFlags))
}

PRL_METHOD( PrlVmCfg_IsRamHotplugEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmRamHotplugEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, pbVmRamHotplugEnabled=%p)",
		__FUNCTION__,
		hVmCfg,
		pbVmRamHotplugEnabled
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION) ||
		PRL_WRONG_PTR(pbVmRamHotplugEnabled))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->IsRamHotplugEnabled(pbVmRamHotplugEnabled);
}

PRL_METHOD( PrlVmCfg_SetRamHotplugEnabled ) (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmRamHotplugEnabled
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hVmCfg=%p, bVmRamHotplugEnabled=%.8X)",
		__FUNCTION__,
		hVmCfg,
		bVmRamHotplugEnabled
	);

	if (PRL_WRONG_HANDLE(hVmCfg, PHT_VM_CONFIGURATION))
		return PRL_ERR_INVALID_ARG;

	PrlHandleVmCfgPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmCfg>( hVmCfg );

	return pVm->SetRamHotplugEnabled(bVmRamHotplugEnabled);
}

PRL_HANDLE PrlVm_Mount_Impl(PRL_HANDLE hVm, PRL_CONST_STR sMntPath, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_MOUNT))

	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->Mount(sMntPath, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_Mount ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR sMntPath,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_MOUNT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Mount, (hVm, sMntPath, nFlags))
}

PRL_HANDLE PrlVm_Umount_Impl(PRL_HANDLE hVm, PRL_UINT32 nFlags)
{
	ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(Umount, nFlags, PJOC_VM_UMOUNT)
}

PRL_ASYNC_METHOD( PrlVm_Umount ) (
		PRL_HANDLE hVm,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_UMOUNT)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Umount, (hVm, nFlags))
}

PRL_HANDLE PrlVm_Move_Impl(PRL_HANDLE hVm, PRL_CONST_STR sNewHomePath, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(sNewHomePath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_VM_MOVE))
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);
	PrlHandleJobPtr pJob = pVm->Move(sNewHomePath, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	pJob->SetVmHandle(hVm);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlVm_Move ) (
		PRL_HANDLE hVm,
		PRL_CONST_STR sNewHomePath,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_VM_MOVE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlVm_Move, (hVm, sNewHomePath, nFlags))
}


