/*
 * PrlApiDisp.cpp
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

#include "Interfaces/ParallelsSdk.h"
#include "SDK/Include/Parallels.h"

#include "Libraries/PrlCommonUtilsBase/PrlStringifyConsts.h"
#include "Libraries/Std/PrlAssert.h"
#include "Libraries/Logging/Logging.h"

#include "PrlCommon.h"
#include "PrlHandleServer.h"
#include "PrlHandleServerDisp.h"
#include "PrlHandleLocalJob.h"
#include "PrlCheckServerHelper.h"
#include "PrlHandleLoginResponse.h"

#include "PrlHandleServerDeprecated.h"
#include "PrlHandleServerInfo.h"
#include "PrlHandleHwGenericDevice.h"
#include "PrlHandleHwHardDisk.h"
#include "PrlHandleHwHddPart.h"
#include "PrlHandleHwNetAdapter.h"
#include "PrlHandleHwGenericPciDevice.h"
#include "PrlHandleRemoteFsInfo.h"
#include "PrlHandleRemoteFsEntry.h"
#include "PrlHandleLicense.h"
#include "PrlHandleRunningTask.h"
#include "PrlHandleUsbIdentity.h"
#include "PrlHandlePluginInfo.h"
#include "PrlHandleBackupResult.h"
#include "PrlHandleDispConfig.h"
#include "PrlHandleSrvConfig.h"
#include "PrlHandleUserProfile.h"
#include "PrlHandleUserInfo.h"
#include "PrlHandleOpTypeList.h"
#include "PrlHandleCpuFeatures.h"
#include "PrlHandleCpuPool.h"

// breakpad includes
#include "parallels_breakpad.h"

#include "Build/Current.ver"

#include "PrlContextSwitcher.h"
#include "PrlNew.h"

////////////////////////////////////////////////////////////////////////////////////////////////


PRL_ASYNC_METHOD( PrlSrv_CheckParallelsServerAliveEx ) (
		PRL_CONST_STR sServerHostname,
		PRL_UINT16 nServerPort,
		PRL_UINT32 timeout,
		PRL_EVENT_HANDLER_PTR pUserCallback,
		PRL_VOID_PTR pUserData,
		PRL_UINT32 nReserved
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_CHECK_ALIVE)
	WRITE_TRACE( DBG_DEBUG, "%s (sServerHostname=%p, nServerPort=%.8X, timeout=%.8X, pUserCallback=%p, pUserData=%p, nReserved=%.8x)",
	__FUNCTION__,
	sServerHostname,
	nServerPort,
	timeout,
	pUserCallback,
	pUserData,
	nReserved
	);
	PrlHandleLocalJobPtr pCheckServerAliveJob( new PrlHandleLocalJob(Uuid::createUuid().toString(),
																PJOC_SRV_CHECK_ALIVE) );
	if ( ! pCheckServerAliveJob.isValid() )
		return PRL_INVALID_HANDLE;

	PrlCheckServerHelper *pHelper = new PrlCheckServerHelper( UTF8_2QSTR( sServerHostname ), nServerPort, pUserCallback, pUserData, timeout, pCheckServerAliveJob );
	if ( ! pHelper )
		pCheckServerAliveJob->SetReturnCode( PRL_ERR_OUT_OF_MEMORY );
	return pCheckServerAliveJob->GetHandle();
}

PRL_RESULT PrlSrv_Create_Impl(PRL_HANDLE_PTR handle)
{
	if ( PRL_WRONG_PTR( handle ) )
		RETURN_RES(PRL_ERR_INVALID_ARG)

	PrlHandleServer* pServer = new PrlHandleServer;

	if ( !pServer )
		RETURN_RES(PRL_ERR_OUT_OF_MEMORY)

	if ( !pServer->isStarted())
		RETURN_RES(PRL_ERR_UNEXPECTED)

	*handle = pServer->GetHandle();

	RETURN_RES(PRL_ERR_SUCCESS)
}

PRL_METHOD( PrlSrv_Create ) (
							 PRL_HANDLE_PTR handle
							 )
{
	SYNC_CHECK_API_INITIALIZED
		LOG_MESSAGE( DBG_DEBUG, "%s (handle=%.8X)",
		__FUNCTION__,
		handle
		);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_Create, (handle))
}

PRL_HANDLE PrlSrv_Login_Impl(PRL_HANDLE hServer, PRL_CONST_STR host, PRL_CONST_STR user, PRL_CONST_STR passwd,
				PRL_CONST_STR sPrevSessionUuid , PRL_UINT32 cmd_port, PRL_UINT32 timeout,
				PRL_SECURITY_LEVEL security_level, PRL_UINT32 flags)
{
	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_LOGIN))

	// Performing login operation to the server
	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->Login(host, user, passwd, sPrevSessionUuid, cmd_port,
						timeout, security_level, flags );
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_Login ) (
								  PRL_HANDLE hServer,
								  PRL_CONST_STR host,
								  PRL_CONST_STR user,
								  PRL_CONST_STR passwd,
								  PRL_CONST_STR sPrevSessionUuid,
								  PRL_UINT32 port_cmd,
								  PRL_UINT32 timeout,
								  PRL_SECURITY_LEVEL security_level
								  )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_LOGIN)
		// Logging API calls for the possibility of debug trace
		LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X host=%s user=%s passwd=%s port_cmd=%u sPrevSessionUuid='%s' timeout=%.8X, security_level=%.8X)",
		__FUNCTION__,
		hServer,
		host,
		user,
		"********", // passwd
		port_cmd,
		sPrevSessionUuid ? sPrevSessionUuid : "",
		timeout,
		security_level
		);

	PRL_UINT32 nFlags = 0;
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_Login, (hServer, host, user,
				passwd, sPrevSessionUuid, port_cmd, timeout, security_level, nFlags));
}

PRL_ASYNC_METHOD( PrlSrv_LoginEx ) (
								  PRL_HANDLE hServer,
								  PRL_CONST_STR host,
								  PRL_CONST_STR user,
								  PRL_CONST_STR passwd,
								  PRL_CONST_STR sPrevSessionUuid,
								  PRL_UINT32 port_cmd,
								  PRL_UINT32 timeout,
								  PRL_SECURITY_LEVEL security_level,
								  PRL_UINT32 flags)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_LOGIN)
		// Logging API calls for the possibility of debug trace
		LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X host=%s user=%s passwd=%s port_cmd=%u sPrevSessionUuid='%s' "
					"timeout=%.8X, security_level=%.8X, flags=%.8X)",
		__FUNCTION__,
		hServer,
		host,
		user,
		"********", // passwd
		port_cmd,
		sPrevSessionUuid ? sPrevSessionUuid : "",
		timeout,
		security_level
		);

	flags = PRL_ELIMINATE_DEPRECATED_FLAGS(flags);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_Login, (hServer, host, user,
			passwd, sPrevSessionUuid, port_cmd, timeout, security_level, flags));
}

PRL_HANDLE PrlSrv_Logoff_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(Logoff, PJOC_SRV_LOGOFF)
}

PRL_ASYNC_METHOD( PrlSrv_Logoff ) (
								   PRL_HANDLE hServer
								   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_LOGOFF)
		// Logging API calls for the possibility of debug trace
		LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X)",
		__FUNCTION__,
		hServer
		);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_Logoff, (hServer))
}

PRL_HANDLE PrlSrv_SendAnswer_Impl(PRL_HANDLE hServer,	PRL_HANDLE hAnswer)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hAnswer, PHT_EVENT) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_SEND_ANSWER))
	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->SendAnswer(hAnswer);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_SendAnswer ) (
									   PRL_HANDLE hServer,
									   PRL_HANDLE hAnswer
									   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_SEND_ANSWER)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_SendAnswer, (hServer, hAnswer))
}

PRL_HANDLE PrlSrv_SendProblemReport_Impl(
	PRL_HANDLE hServer,
	PRL_HANDLE hProblemReport,
	PRL_UINT32 nFlags
)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
        RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_SEND_PROBLEM_REPORT))

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
    PrlHandleJobPtr pJob = pServer->SrvSendProblemReport( hProblemReport,
                                                               nFlags );
	if ( !pJob )
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle());
}

PRL_ASYNC_METHOD( PrlSrv_SendProblemReport ) (
		PRL_HANDLE hServer,
		PRL_HANDLE hProblemReport,
		PRL_UINT32 nFlags
		)
{
    ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_SEND_PROBLEM_REPORT)
    CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_SendProblemReport, (hServer, hProblemReport, nFlags))
}

PRL_HANDLE PrlSrv_LoginLocal_Impl(PRL_HANDLE hServer, PRL_CONST_STR sPrevSessionUuid, PRL_UINT32 port,
					PRL_SECURITY_LEVEL security_level, PRL_UINT32 flags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_LOGIN_LOCAL))

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->LoginLocal( sPrevSessionUuid, port, security_level, flags );
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_LoginLocal ) (
									   PRL_HANDLE hServer,
									   PRL_CONST_STR sPrevSessionUuid,
									   PRL_UINT32 port,
									   PRL_SECURITY_LEVEL security_level
									   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_LOGIN_LOCAL)
		// Logging API calls for the possibility of debug trace
		LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, sPrevSessionUuid=%.8X, port=%.8X, security_level=%.8X)",
		__FUNCTION__,
		hServer,
		sPrevSessionUuid,
		port,
		security_level
		);

	PRL_UINT32 nFlags = 0;
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_LoginLocal,
				(hServer, sPrevSessionUuid, port, security_level, nFlags))
}

PRL_ASYNC_METHOD( PrlSrv_LoginLocalEx ) (
									   PRL_HANDLE hServer,
									   PRL_CONST_STR sPrevSessionUuid,
									   PRL_UINT32 port,
									   PRL_SECURITY_LEVEL security_level,
									   PRL_UINT32 flags
									   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_LOGIN_LOCAL)
		// Logging API calls for the possibility of debug trace
		LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, sPrevSessionUuid=%.8X, port=%.8X, security_level=%.8X, flags=%.8X)",
		__FUNCTION__,
		hServer,
		sPrevSessionUuid,
		port,
		security_level
		);

	flags = PRL_ELIMINATE_DEPRECATED_FLAGS(flags);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_LoginLocal, (hServer,
				sPrevSessionUuid, port, security_level, flags));
}

PRL_METHOD( PrlSrv_RegEventHandler ) (
									  PRL_HANDLE hServer,
									  PRL_EVENT_HANDLER_PTR handler,
									  PRL_VOID_PTR userData
									  )
{
	return PrlHandle_RegEventHandler(hServer, handler, userData);
}

PRL_METHOD( PrlSrv_UnregEventHandler ) (
										PRL_HANDLE hServer,
										PRL_EVENT_HANDLER_PTR handler,
										PRL_VOID_PTR userData
										)
{
	return PrlHandle_UnregEventHandler(hServer, handler, userData);
}

PRL_METHOD( PrlSrv_GetQuestions ) (
								   PRL_HANDLE hServer,
								   PRL_HANDLE_PTR pQuestionsList
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, pQuestionsList=%.8X)",
		__FUNCTION__,
		hServer,
		pQuestionsList
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(pQuestionsList) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	try
	{
		PrlHandleHandlesList  *pHandlesList =
			new PrlHandleHandlesList(pServer->GetQuestionsList().GetQuestionsList());
		*pQuestionsList = pHandlesList->GetHandle();
	}
	catch(...)
	{
		*pQuestionsList = PRL_INVALID_HANDLE;
		return (PRL_ERR_OUT_OF_MEMORY);
	}

	return (PRL_ERR_SUCCESS);
}

PRL_HANDLE PrlSrv_SetNonInteractiveSession_Impl(PRL_HANDLE hServer,
														 PRL_BOOL bNonInteractive,
														 PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_SET_NON_INTERACTIVE_SESSION));

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->SetNonInteractiveSession(bNonInteractive, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_SetNonInteractiveSession ) (
								   PRL_HANDLE hServer,
								   PRL_BOOL bNonInteractive,
								   PRL_UINT32 nFlags
								   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_SET_NON_INTERACTIVE_SESSION)

	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, bNonInteractive=%.8X, nFlags=%.8X)",
	__FUNCTION__,
	hServer,
	bNonInteractive,
	nFlags
	);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_SetNonInteractiveSession, (hServer, bNonInteractive, nFlags))
}

PRL_METHOD( PrlSrv_IsNonInteractiveSession ) (
	PRL_HANDLE hServer,
	PRL_BOOL_PTR pbNonInteractive
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, pbNonInteractive=%.8X)",
		__FUNCTION__,
		hServer,
		pbNonInteractive
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(pbNonInteractive))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );

	return pServer->IsNonInteractiveSession(pbNonInteractive);
}

PRL_METHOD( PrlSrv_HasRestriction ) (
	PRL_HANDLE hServer,
	PRL_LICENSE_RESTRICTION_KEY nRestrictionKey,
	PRL_BOOL_PTR pbHasRestriction
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, nRestrictionKey=%.8x, pbHasRestriction=%.8X)",
		__FUNCTION__,
		hServer,
		nRestrictionKey,
		pbHasRestriction
		);

	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(pbHasRestriction))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );

	return pServer->HasRestriction(nRestrictionKey, pbHasRestriction);
}

PRL_METHOD( PrlSrv_GetRestrictionInfo ) (
	PRL_HANDLE hServer,
	PRL_LICENSE_RESTRICTION_KEY nRestrictionKey,
	PRL_HANDLE_PTR phEvtParam
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, nRestrictionKey=%.8x, phEvtParam=%.8X)",
		__FUNCTION__,
		hServer,
		nRestrictionKey,
		phEvtParam
		);

	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(phEvtParam))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );

	return pServer->GetRestrictionInfo(nRestrictionKey, phEvtParam);
}

PRL_HANDLE PrlSrv_DisableConfirmationMode_Impl(PRL_HANDLE hServer,
														PRL_CONST_STR sUser,
														PRL_CONST_STR sPasswd,
														PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER)
		|| PRL_WRONG_PTR(sUser)
		|| PRL_WRONG_PTR(sPasswd)
		)
	{
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_SET_SESSION_CONFIRMATION_MODE));
	}

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->DisableConfirmationMode(sUser, sPasswd, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_DisableConfirmationMode ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR sUser,
	PRL_CONST_STR sPasswd,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_SET_NON_INTERACTIVE_SESSION)

		LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, sUser=%.8X, sPasswd=%.8X, nFlags=%.8X)",
		__FUNCTION__,
		hServer,
		sUser,
		sPasswd,
		nFlags
		);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_DisableConfirmationMode,\
		(hServer, sUser, sPasswd, nFlags))
}

PRL_HANDLE PrlSrv_EnableConfirmationMode_Impl(PRL_HANDLE hServer,
													PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_SET_SESSION_CONFIRMATION_MODE));

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->EnableConfirmationMode(nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_EnableConfirmationMode ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_SET_NON_INTERACTIVE_SESSION)

		LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, nFlags=%.8X)",
		__FUNCTION__,
		hServer,
		nFlags
		);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_EnableConfirmationMode,\
		(hServer, nFlags))
}

PRL_METHOD( PrlSrv_IsConfirmationModeEnabled ) (
	PRL_HANDLE hServer,
	PRL_BOOL_PTR pbConfirmationModeEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, pbConfirmationModeEnabled=%.8X)",
		__FUNCTION__,
		hServer,
		pbConfirmationModeEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(pbConfirmationModeEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );

	return pServer->IsConfirmationModeEnabled(pbConfirmationModeEnabled);
}

PRL_HANDLE PrlSrv_GetSrvConfig_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(GetSrvConfig, PJOC_SRV_GET_SRV_CONFIG)
}

PRL_ASYNC_METHOD( PrlSrv_GetSrvConfig ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_SRV_CONFIG)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetSrvConfig, (hServer))
}

PRL_HANDLE PrlSrv_GetCommonPrefs_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(GetCommonPrefs, PJOC_SRV_GET_COMMON_PREFS)
}

PRL_ASYNC_METHOD( PrlSrv_GetCommonPrefs ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_COMMON_PREFS)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetCommonPrefs, (hServer))
}

PRL_HANDLE PrlSrv_CommonPrefsBeginEdit_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(CommonPrefsBeginEdit, PJOC_SRV_COMMON_PREFS_BEGIN_EDIT)
}

PRL_ASYNC_METHOD( PrlSrv_CommonPrefsBeginEdit ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_COMMON_PREFS_BEGIN_EDIT)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_CommonPrefsBeginEdit, (hServer))
}

PRL_HANDLE PrlSrv_CommonPrefsCommitEx_Impl(PRL_HANDLE hServer,	PRL_HANDLE hDispCfg, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hDispCfg, PHT_DISP_CONFIG) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_COMMON_PREFS_COMMIT))

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->CommonPrefsCommit(hDispCfg, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_CommonPrefsCommit ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hDispCfg
	)
{
	return PrlSrv_CommonPrefsCommitEx( hServer, hDispCfg, 0 );
}

PRL_ASYNC_METHOD( PrlSrv_CommonPrefsCommitEx ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hDispCfg,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_COMMON_PREFS_COMMIT)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_CommonPrefsCommitEx, (hServer, hDispCfg, nFlags))
}

PRL_HANDLE PrlSrv_GetUserProfile_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(GetUserProfile, PJOC_SRV_GET_USER_PROFILE)
}

PRL_ASYNC_METHOD( PrlSrv_GetUserProfile ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_USER_PROFILE)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetUserProfile, (hServer))
}

PRL_HANDLE PrlSrv_GetUserInfoList_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(GetUserInfoList, PJOC_SRV_GET_USER_INFO_LIST)
}

PRL_ASYNC_METHOD( PrlSrv_GetUserInfoList ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_USER_INFO_LIST)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetUserInfoList, (hServer))
}

PRL_HANDLE PrlSrv_GetUserInfo_Impl(PRL_HANDLE hServer, PRL_CONST_STR sUserId)
{
	DISP_ONE_HANDLE_AND_ONE_PTR_PARAM_METH_IMPLEMENTATION(GetUserInfo, sUserId, PJOC_SRV_GET_USER_INFO);
}

PRL_ASYNC_METHOD( PrlSrv_GetUserInfo ) (
										PRL_HANDLE hServer,
										PRL_CONST_STR sUserId
										)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_USER_INFO)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetUserInfo, (hServer, sUserId))
}

PRL_HANDLE PrlSrv_ConfigureGenericPci_Impl(PRL_HANDLE hServer, PRL_HANDLE hDevList, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hDevList, PHT_HANDLES_LIST)
		|| PRL_WRONG_HANDLES_LIST(hDevList, PHT_HW_GENERIC_PCI_DEVICE))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_CONFIGURE_GENERIC_PCI));

	PrlHandleHandlesListPtr pDevList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hDevList);
	QList<PrlHandleBasePtr> dev_list = pDevList->GetHandlesList();
	if (dev_list.isEmpty())
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_CONFIGURE_GENERIC_PCI));

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->ConfigureGenericPci(dev_list, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_ConfigureGenericPci ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hDevList,
	PRL_UINT32 nFlags
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, hDevList=%.8X, nFlags=%.8X)",
		__FUNCTION__,
		hServer,
		hDevList,
		nFlags
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_CONFIGURE_GENERIC_PCI)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_ConfigureGenericPci, (hServer, hDevList, nFlags))
}

PRL_HANDLE PrlSrv_UserProfileBeginEdit_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(UserProfileBeginEdit, PJOC_SRV_USER_PROFILE_BEGIN_EDIT)
}

PRL_ASYNC_METHOD( PrlSrv_UserProfileBeginEdit ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_USER_PROFILE_BEGIN_EDIT)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UserProfileBeginEdit, (hServer))
}

PRL_HANDLE PrlSrv_UserProfileCommit_Impl(PRL_HANDLE hServer,	PRL_HANDLE hUserProfile)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hUserProfile, PHT_USER_PROFILE) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_USER_PROFILE_COMMIT))

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->UserProfileCommit(hUserProfile);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_UserProfileCommit ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hUserProfile
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_USER_PROFILE_COMMIT)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UserProfileCommit, (hServer, hUserProfile))
}

PRL_METHOD( PrlSrv_IsConnected ) (
								  PRL_HANDLE hServer,
								  PRL_BOOL_PTR pbConnected,
								  PRL_BOOL_PTR pbIsConnectionLocal
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, pbConnected=%.8X)",
		__FUNCTION__,
		hServer,
		pbConnected
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(pbConnected) || PRL_WRONG_PTR(pbIsConnectionLocal) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );

	PRL_RESULT ret = pServer->IsConnected(pbConnected);

	if ( PRL_SUCCEEDED(ret) && *pbConnected )
		*pbIsConnectionLocal = pServer->IsConnectionLocal();

	return ret;
}

PRL_METHOD( PrlSrv_IsProxyConnection ) (
								  PRL_HANDLE hServer,
								  PRL_BOOL_PTR pbProxyConnection
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, pbProxyConnection=%.8X)",
		__FUNCTION__,
		hServer,
		pbProxyConnection
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(pbProxyConnection) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );

	PRL_RESULT ret = PRL_ERR_SUCCESS;
	*pbProxyConnection = pServer->IsProxyConnection();
	return ret;
}

PRL_METHOD( PrlSrv_GetServerInfo ) (
									PRL_HANDLE hServer,
									PRL_HANDLE_PTR phServerInfo
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, phServerInfo=%.8X)",
		__FUNCTION__,
		hServer,
		phServerInfo
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(phServerInfo) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );

	return (pServer->GetServerInfo(phServerInfo));
}

PRL_HANDLE PrlSrv_Shutdown_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_SHUTDOWN))

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->Shutdown(nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_Shutdown) (
									PRL_HANDLE hServer,
									PRL_BOOL bForceShutdown
									)
{
	return PrlSrv_ShutdownEx( hServer, bForceShutdown?PSHF_FORCE_SHUTDOWN:0 );
}

PRL_ASYNC_METHOD( PrlSrv_ShutdownEx) (
									PRL_HANDLE hServer,
									PRL_UINT32 nFlags
									)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_SHUTDOWN)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_Shutdown, (hServer, nFlags))
}

PRL_HANDLE PrlSrv_FsGetDiskList_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(FsGetDiskList, PJOC_SRV_FS_GET_DISK_LIST)
}

PRL_ASYNC_METHOD( PrlSrv_FsGetDiskList ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_FS_GET_DISK_LIST)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_FsGetDiskList, (hServer))
}

PRL_HANDLE PrlSrv_FsGetDirEntries_Impl(PRL_HANDLE hServer,	PRL_CONST_STR path)
{
	DISP_ONE_HANDLE_AND_ONE_PTR_PARAM_METH_IMPLEMENTATION(FsGetDirEntries, path, PJOC_SRV_FS_GET_DIR_ENTRIES)
}

PRL_ASYNC_METHOD( PrlSrv_FsGetDirEntries ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR path
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_FS_GET_DIR_ENTRIES)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_FsGetDirEntries, (hServer, path))
}

PRL_HANDLE PrlSrv_FsCreateDir_Impl(PRL_HANDLE hServer,	PRL_CONST_STR path)
{
	DISP_ONE_HANDLE_AND_ONE_PTR_PARAM_METH_IMPLEMENTATION(FsCreateDir, path, PJOC_SRV_FS_CREATE_DIR)
}

PRL_ASYNC_METHOD( PrlSrv_FsCreateDir ) (
										PRL_HANDLE hServer,
										PRL_CONST_STR path
										)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_FS_CREATE_DIR)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_FsCreateDir, (hServer, path))
}

PRL_HANDLE PrlSrv_FsRemoveEntry_Impl(PRL_HANDLE hServer,	PRL_CONST_STR path)
{
	DISP_ONE_HANDLE_AND_ONE_PTR_PARAM_METH_IMPLEMENTATION(FsRemoveEntry, path, PJOC_SRV_FS_REMOVE_ENTRY)
}

PRL_ASYNC_METHOD( PrlSrv_FsRemoveEntry ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR path
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_FS_REMOVE_ENTRY)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_FsRemoveEntry, (hServer, path))
}

PRL_HANDLE PrlSrv_FsCanCreateFile_Impl(PRL_HANDLE hServer,	PRL_CONST_STR path)
{
	DISP_ONE_HANDLE_AND_ONE_PTR_PARAM_METH_IMPLEMENTATION(FsCanCreateFile, path, PJOC_SRV_FS_CAN_CREATE_FILE)
}

PRL_ASYNC_METHOD( PrlSrv_FsCanCreateFile ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR path
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_FS_CAN_CREATE_FILE)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_FsCanCreateFile, (hServer, path))
}

PRL_HANDLE PrlSrv_FsRenameEntry_Impl(PRL_HANDLE hServer,	PRL_CONST_STR oldPath, PRL_CONST_STR newPath)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(oldPath) || PRL_WRONG_PTR(newPath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_FS_RENAME_ENTRY))

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->FsRenameEntry(oldPath, newPath);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_FsRenameEntry ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR oldPath,
	PRL_CONST_STR newPath
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_FS_RENAME_ENTRY)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_FsRenameEntry, (hServer, oldPath, newPath))
}

PRL_HANDLE PrlSrv_FsGenerateEntryName_Impl(
							PRL_HANDLE hServer,
							PRL_CONST_STR sDirPath,
							PRL_CONST_STR sFilenamePrefix,
							PRL_CONST_STR sFilenameSuffix,
							PRL_CONST_STR sIndexDelimiter
							)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(sDirPath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_FS_GENERATE_ENTRY_NAME))

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->FsGenerateEntryName(sDirPath, sFilenamePrefix, sFilenameSuffix, sIndexDelimiter);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_FsGenerateEntryName ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR sDirPath,
	PRL_CONST_STR sFilenamePrefix,
	PRL_CONST_STR sFilenameSuffix,
	PRL_CONST_STR sIndexDelimiter
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_FS_GENERATE_ENTRY_NAME)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_FsGenerateEntryName,\
		(hServer, sDirPath, sFilenamePrefix, sFilenameSuffix, sIndexDelimiter))
}

PRL_HANDLE PrlSrv_UpdateLicense_Impl(PRL_HANDLE hServer,
											  PRL_CONST_STR sKey,
											  PRL_CONST_STR sUser,
											  PRL_CONST_STR sCompany,
											  PRL_UINT32 nFlags
											  )
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER)  || PRL_WRONG_PTR( sKey ) || PRL_WRONG_PTR( sUser ) || PRL_WRONG_PTR( sCompany ) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_UPDATE_LICENSE))

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->DspCmdUserUpdateLicense( sKey, sUser, sCompany, nFlags );
	if ( !pJob )
		RETURN_RES(PRL_INVALID_HANDLE)
		RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_UpdateLicense ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR sKey,
	PRL_CONST_STR sUser,
	PRL_CONST_STR sCompany
	)
{
	return PrlSrv_UpdateLicenseEx( hServer, sKey, sUser, sCompany, 0 );
}

PRL_ASYNC_METHOD( PrlSrv_UpdateLicenseEx ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR sKey,
	PRL_CONST_STR sUser,
	PRL_CONST_STR sCompany,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_UPDATE_LICENSE)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_UpdateLicense, (hServer, sKey, sUser, sCompany, nFlags))
}

PRL_HANDLE PrlSrv_GetLicenseInfo_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(DspCmdUserGetLicenseInfo, PJOC_SRV_GET_LICENSE_INFO)
}

PRL_ASYNC_METHOD( PrlSrv_GetLicenseInfo ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_LICENSE_INFO)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetLicenseInfo, (hServer))
}

PRL_HANDLE PrlSrv_RefreshPlugins_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION_WITH_FLAGS(RefreshPlugins, nFlags,
		PJOC_SRV_REFRESH_PLUGINS)
}

PRL_ASYNC_METHOD( PrlSrv_RefreshPlugins ) (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_REFRESH_PLUGINS)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_RefreshPlugins, (hServer, nFlags))
}

PRL_HANDLE PrlSrv_GetPluginsList_Impl(PRL_HANDLE hServer, PRL_CONST_STR sClassId, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_GET_PLUGINS_LIST));
	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->GetPluginsList(sClassId, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle());
}

PRL_ASYNC_METHOD( PrlSrv_GetPluginsList ) (
		PRL_HANDLE hServer,
		PRL_CONST_STR sClassId,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_PLUGINS_LIST)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetPluginsList, (hServer, sClassId, nFlags))
}

PRL_HANDLE PrlSrv_GetProblemReport_Impl(PRL_HANDLE hServer)
{
	ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(SrvGetProblemReport, PJOC_SRV_GET_PROBLEM_REPORT);
}

PRL_ASYNC_METHOD( PrlSrv_GetProblemReport ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_PROBLEM_REPORT)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetProblemReport, (hServer))
}

PRL_HANDLE PrlSrv_GetPackedProblemReport_Impl(PRL_HANDLE hServer, PRL_UINT32 nFlags)
{
    ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION_WITH_FLAGS(SrvGetPackedProblemReport, nFlags, PJOC_SRV_GET_PACKED_PROBLEM_REPORT);
}

PRL_ASYNC_METHOD( PrlSrv_GetPackedProblemReport ) (
	PRL_HANDLE hServer,
	PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_PROBLEM_REPORT)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetPackedProblemReport, (hServer,nFlags))
}

PRL_HANDLE PrlSrv_AttachToLostTask_Impl(PRL_HANDLE hServer, PRL_CONST_STR sTaskId)
{
	DISP_ONE_HANDLE_AND_ONE_PTR_PARAM_METH_IMPLEMENTATION(SrvAttachToLostTask, sTaskId, PJOC_SRV_ATTACH_TO_LOST_TASK);
}

PRL_ASYNC_METHOD( PrlSrv_AttachToLostTask ) (
	PRL_HANDLE hServer,
	PRL_CONST_STR sTaskId
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_ATTACH_TO_LOST_TASK)
		CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_AttachToLostTask, (hServer, sTaskId))
}

PRL_METHOD( PrlSrv_IsFeatureSupported ) (
		PRL_HANDLE hServer,
		PRL_FEATURES_MATRIX nFeatureId,
		PRL_BOOL_PTR pbIsSupported
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hServer=%.8X, nFeatureId=%.8X, pbIsSupported=%.8X)",
		__FUNCTION__,
		hServer,
		nFeatureId,
		pbIsSupported
	);

	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(pbIsSupported))
		return PRL_ERR_INVALID_ARG;

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );

	return pServer->IsFeatureSupported(nFeatureId, pbIsSupported);
}

PRL_HANDLE PrlSrv_GetDiskFreeSpace_Impl(PRL_HANDLE hServer, PRL_CONST_STR sPath, PRL_UINT32 nFlags)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(sPath) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_GET_DISK_FREE_SPACE))

	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );
	PrlHandleJobPtr pJob = pServer->GetFreeDiskSpace(sPath, nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_GetDiskFreeSpace ) (
		PRL_HANDLE hServer,
		PRL_CONST_STR sPath,
		PRL_UINT32 nFlags
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_GET_DISK_FREE_SPACE)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetDiskFreeSpace, (hServer, sPath, nFlags))
}

PRL_HANDLE PrlSrv_SetVNCEncryption_Impl(
		PRL_HANDLE hServer,
		PRL_CONST_STR sPubKey,
		PRL_CONST_STR sPrivKey,
		PRL_UINT32 nFlags
		)
{
	if (PRL_WRONG_HANDLE(hServer, PHT_SERVER) ||
			PRL_WRONG_PTR(sPubKey) || PRL_WRONG_PTR(sPrivKey))
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_STORE_VALUE_BY_KEY));

	CVmEventParameter *pEventParam;
	SmartPtr<CVmEvent> pEvent(new CVmEvent());
	pEventParam = new CVmEventParameter(PVE::String, sPubKey,
			EVT_PARAM_VNC_SSL_CERTIFICATE);
	pEvent->addEventParameter(pEventParam);
	pEventParam = new CVmEventParameter(PVE::String, sPrivKey,
			EVT_PARAM_VNC_SSL_PRIVATE_KEY);
	pEvent->addEventParameter(pEventParam);

	PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );
	PrlHandleJobPtr pJob = pServer->DspCmdStorageSetValue(PRL_KEY_SET_VNC_ENCRYPTION_DATA,
				QSTR2UTF8(pEvent->toString()), nFlags);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE);
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_SetVNCEncryption ) (
		PRL_HANDLE hServer,
		PRL_CONST_STR sPubKey,
		PRL_CONST_STR sPrivKey,
		PRL_UINT32 nFlags
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_STORE_VALUE_BY_KEY)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_SetVNCEncryption,
				(hServer, sPubKey, sPrivKey, nFlags))
}

PRL_METHOD( PrlFsInfo_GetType ) (
								 PRL_HANDLE hFsInfo,
								 PRL_FILE_SYSTEM_TYPE_PTR pnFsType
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsInfo=%.8X, pnFsType=%.8X)",
		__FUNCTION__,
		hFsInfo,
		pnFsType
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsInfo, PHT_REMOTE_FILESYSTEM_INFO) || PRL_WRONG_PTR(pnFsType) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsInfoPtr pFsInfo = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsInfo>( hFsInfo );

	return pFsInfo->GetFSType(pnFsType);
}

PRL_METHOD( PrlFsInfo_GetFsType ) (
								 PRL_HANDLE hFsInfo,
								 PRL_FILE_SYSTEM_FS_TYPE_PTR pnFsType
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsInfo=%.8X, pnFsType=%.8X)",
		__FUNCTION__,
		hFsInfo,
		pnFsType
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsInfo, PHT_REMOTE_FILESYSTEM_INFO) || PRL_WRONG_PTR(pnFsType) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsInfoPtr pFsInfo = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsInfo>( hFsInfo );

	return pFsInfo->GetFileSystemType(pnFsType);
}

PRL_METHOD( PrlFsInfo_GetChildEntriesCount ) (
	PRL_HANDLE hFsInfo,
	PRL_UINT32_PTR pnChildEntriesCount
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsInfo=%.8X, pnChildEntriesCount=%.8X)",
		__FUNCTION__,
		hFsInfo,
		pnChildEntriesCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsInfo, PHT_REMOTE_FILESYSTEM_INFO) || PRL_WRONG_PTR(pnChildEntriesCount) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsInfoPtr pFsInfo = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsInfo>( hFsInfo );

	return pFsInfo->GetChildEntriesCount(pnChildEntriesCount);
}

PRL_METHOD( PrlFsInfo_GetChildEntry ) (
									   PRL_HANDLE hFsInfo,
									   PRL_UINT32 nIndex,
									   PRL_HANDLE_PTR phChildEntry
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsInfo=%.8X, nIndex=%.8X, phChildEntry=%.8X)",
		__FUNCTION__,
		hFsInfo,
		nIndex,
		phChildEntry
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsInfo, PHT_REMOTE_FILESYSTEM_INFO) || PRL_WRONG_PTR(phChildEntry) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsInfoPtr pFsInfo = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsInfo>( hFsInfo );

	return pFsInfo->GetChildEntry(nIndex, phChildEntry);
}

PRL_METHOD( PrlFsInfo_GetParentEntry ) (
										PRL_HANDLE hFsInfo,
										PRL_HANDLE_PTR phParentEntry
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsInfo=%.8X, phParentEntry=%.8X)",
		__FUNCTION__,
		hFsInfo,
		phParentEntry
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsInfo, PHT_REMOTE_FILESYSTEM_INFO) || PRL_WRONG_PTR(phParentEntry) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsInfoPtr pFsInfo = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsInfo>( hFsInfo );

	return pFsInfo->GetParentEntry(phParentEntry);
}

PRL_METHOD( PrlFsEntry_GetAbsolutePath ) (
	PRL_HANDLE hFsEntry,
	PRL_STR sAbsolutePath,
	PRL_UINT32_PTR pnAbsolutePathBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsEntry=%.8X, sAbsolutePath=%.8X, pnAbsolutePathBufLength=%.8X)",
		__FUNCTION__,
		hFsEntry,
		sAbsolutePath,
		pnAbsolutePathBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsEntry, PHT_REMOTE_FILESYSTEM_ENTRY)
		|| PRL_WRONG_PTR(pnAbsolutePathBufLength) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsEntryPtr pFsEntry = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsEntry>( hFsEntry );

	return pFsEntry->GetAbsolutePath(sAbsolutePath, pnAbsolutePathBufLength);
}

PRL_METHOD( PrlFsEntry_GetRelativeName ) (
	PRL_HANDLE hFsEntry,
	PRL_STR sRelativeName,
	PRL_UINT32_PTR pnRelativeNameBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsEntry=%.8X, sRelativeName=%.8X, pnRelativeNameBufLength=%.8X)",
		__FUNCTION__,
		hFsEntry,
		sRelativeName,
		pnRelativeNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsEntry, PHT_REMOTE_FILESYSTEM_ENTRY)
		|| PRL_WRONG_PTR(pnRelativeNameBufLength) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsEntryPtr pFsEntry = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsEntry>( hFsEntry );

	return pFsEntry->GetRelativeName(sRelativeName, pnRelativeNameBufLength);
}

PRL_METHOD( PrlFsEntry_GetLastModifiedDate ) (
	PRL_HANDLE hFsEntry,
	PRL_STR sFsEntryLastModifiedDate,
	PRL_UINT32_PTR pnFsEntryLastModifiedDateBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsEntry=%.8X, sFsEntryLastModifiedDate=%.8X, pnFsEntryLastModifiedDateBufLength=%.8X)",
		__FUNCTION__,
		hFsEntry,
		sFsEntryLastModifiedDate,
		pnFsEntryLastModifiedDateBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsEntry, PHT_REMOTE_FILESYSTEM_ENTRY)
		|| PRL_WRONG_PTR(pnFsEntryLastModifiedDateBufLength) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsEntryPtr pFsEntry = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsEntry>( hFsEntry );

	return pFsEntry->GetLastModifiedDate(sFsEntryLastModifiedDate, pnFsEntryLastModifiedDateBufLength);
}

PRL_METHOD( PrlFsEntry_GetSize ) (
								  PRL_HANDLE hFsEntry,
								  PRL_UINT64_PTR pnSize
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsEntry=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hFsEntry,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsEntry, PHT_REMOTE_FILESYSTEM_ENTRY) || PRL_WRONG_PTR(pnSize) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsEntryPtr pFsEntry = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsEntry>( hFsEntry );

	return pFsEntry->GetSize(pnSize);
}

PRL_METHOD( PrlFsEntry_GetPermissions ) (
	PRL_HANDLE hFsEntry,
	PRL_UINT32_PTR pnPermissions
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsEntry=%.8X, pnPermissions=%.8X)",
		__FUNCTION__,
		hFsEntry,
		pnPermissions
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsEntry, PHT_REMOTE_FILESYSTEM_ENTRY) || PRL_WRONG_PTR(pnPermissions) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsEntryPtr pFsEntry = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsEntry>( hFsEntry );

	return pFsEntry->GetPermissions(pnPermissions);
}

PRL_METHOD( PrlFsEntry_GetType ) (
								  PRL_HANDLE hFsEntry,
								  PRL_FILE_SYSTEM_ELEMENT_TYPE_PTR pnFsEntryType
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hFsEntry=%.8X, pnFsEntryType=%.8X)",
		__FUNCTION__,
		hFsEntry,
		pnFsEntryType
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hFsEntry, PHT_REMOTE_FILESYSTEM_ENTRY) || PRL_WRONG_PTR(pnFsEntryType) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleRemoteFsEntryPtr pFsEntry = PRL_OBJECT_BY_HANDLE<PrlHandleRemoteFsEntry>( hFsEntry );

	return pFsEntry->GetEntryType(pnFsEntryType);
}

PRL_METHOD( PrlSrvCfg_GetHostRamSize ) (
										PRL_HANDLE hSrvConfig,
										PRL_UINT32_PTR pnHostRamSize
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnHostRamSize=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnHostRamSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnHostRamSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetHostRamSize(pnHostRamSize);
}

PRL_METHOD( PrlSrvCfg_GetCpuModel ) (
									 PRL_HANDLE hSrvConfig,
									 PRL_STR sCpuModel,
									 PRL_UINT32_PTR pnCpuModelBufLength
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, sCpuModel=%.8X, pnCpuModelBufLength=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		sCpuModel,
		pnCpuModelBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) ||
		PRL_WRONG_PTR(pnCpuModelBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return (pSrvConfig->GetCpuModel(sCpuModel, pnCpuModelBufLength));
}

PRL_METHOD( PrlSrvCfg_GetCpuCount ) (
									 PRL_HANDLE hSrvConfig,
									 PRL_UINT32_PTR pnCpuCount
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnCpuCount=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnCpuCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnCpuCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetCpuCount(pnCpuCount);
}

PRL_METHOD( PrlSrvCfg_GetCpuSpeed ) (
									 PRL_HANDLE hSrvConfig,
									 PRL_UINT32_PTR pnCpuSpeed
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnCpuSpeed=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnCpuSpeed
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnCpuSpeed))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetCpuSpeed(pnCpuSpeed);
}

PRL_METHOD( PrlSrvCfg_GetCpuMode ) (
									PRL_HANDLE hSrvConfig,
									PRL_CPU_MODE_PTR pnCpuMode
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnCpuMode=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnCpuMode
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnCpuMode))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetCpuMode(pnCpuMode);
}

PRL_METHOD( PrlSrvCfg_GetCpuHvt ) (
	PRL_HANDLE hSrvConfig,
	PRL_CPU_HVT_PTR pnCpuHvt
)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnCpuHvt=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnCpuHvt
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnCpuHvt))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetCpuVtxMode(pnCpuHvt);
}

PRL_METHOD( PrlSrvCfg_GetHostOsType ) (
									   PRL_HANDLE hSrvConfig,
									   PRL_HOST_OS_TYPE_PTR pnHostOsType
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnHostOsType=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnHostOsType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnHostOsType))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetHostOsType(pnHostOsType);
}

PRL_METHOD( PrlSrvCfg_GetHostOsMajor ) (
										PRL_HANDLE hSrvConfig,
										PRL_UINT32_PTR pnHostOsMajor
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnHostOsMajor=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnHostOsMajor
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnHostOsMajor))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetHostOsMajor(pnHostOsMajor);
}

PRL_METHOD( PrlSrvCfg_GetHostOsMinor ) (
										PRL_HANDLE hSrvConfig,
										PRL_UINT32_PTR pnHostOsMinor
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnHostOsMinor=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnHostOsMinor
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnHostOsMinor))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetHostOsMinor(pnHostOsMinor);
}

PRL_METHOD( PrlSrvCfg_GetHostOsSubMinor ) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32_PTR pnHostOsSubMinor
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnHostOsSubMinor=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnHostOsSubMinor
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnHostOsSubMinor))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetHostOsSubMinor(pnHostOsSubMinor);
}

PRL_METHOD( PrlSrvCfg_GetHostOsStrPresentation ) (
	PRL_HANDLE hSrvConfig,
	PRL_STR sHostOsStrPresentation,
	PRL_UINT32_PTR pnHostOsStrPresentationBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, sHostOsStrPresentation=%.8X, pnHostOsStrPresentationBufLength=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		sHostOsStrPresentation,
		pnHostOsStrPresentationBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) ||
		PRL_WRONG_PTR(pnHostOsStrPresentationBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return (pSrvConfig->GetHostOsStrPresentation(sHostOsStrPresentation, pnHostOsStrPresentationBufLength));
}

PRL_METHOD( PrlSrvCfg_IsSoundDefaultEnabled ) (
	PRL_HANDLE hSrvConfig,
	PRL_BOOL_PTR pbSoundDefaultEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pbSoundDefaultEnabled=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pbSoundDefaultEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pbSoundDefaultEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->IsSoundDefaultEnabled(pbSoundDefaultEnabled);
}

PRL_METHOD( PrlSrvCfg_IsUsbSupported ) (
										PRL_HANDLE hSrvConfig,
										PRL_BOOL_PTR pbUsbSupported
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pbUsbSupported=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pbUsbSupported
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pbUsbSupported))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->IsUsbSupported(pbUsbSupported);
}

PRL_METHOD( PrlSrvCfg_IsVtdSupported ) (
										PRL_HANDLE hSrvConfig,
										PRL_BOOL_PTR pbVtdSupported
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pbVtdSupported=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pbVtdSupported
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pbVtdSupported))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->IsVtdSupported(pbVtdSupported);
}

PRL_METHOD( PrlSrvCfg_GetMaxHostNetAdapters ) (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnMaxNetAdaptersCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnMaxNetAdaptersCount=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnMaxNetAdaptersCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnMaxNetAdaptersCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetMaxHostNetAdapters(pnMaxNetAdaptersCount);
}

PRL_METHOD( PrlSrvCfg_GetMaxVmNetAdapters ) (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnMaxNetAdaptersCount
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pnMaxNetAdaptersCount=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		pnMaxNetAdaptersCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pnMaxNetAdaptersCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetMaxVmNetAdapters(pnMaxNetAdaptersCount);
}

PRL_METHOD( PrlSrvCfg_GetHostname ) (
	PRL_HANDLE hSrvConfig,
	PRL_STR sHostname,
	PRL_UINT32_PTR pnHostnameBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, sHostname=%.8X, pnHostnameBufLength=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		sHostname,
		pnHostnameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) ||
		PRL_WRONG_PTR(pnHostnameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return (pSrvConfig->GetHostname(sHostname, pnHostnameBufLength));
}

PRL_METHOD( PrlSrvCfg_GetDefaultGateway ) (
	PRL_HANDLE hSrvConfig,
	PRL_STR sDefaultGateway,
	PRL_UINT32_PTR pnDefaultGatewayBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, sDefaultGateway=%.8X, pnDefaultGatewayBufLength=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		sDefaultGateway,
		pnDefaultGatewayBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) ||
		PRL_WRONG_PTR(pnDefaultGatewayBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return (pSrvConfig->GetDefaultGateway(sDefaultGateway, pnDefaultGatewayBufLength));
}

PRL_METHOD( PrlSrvCfg_GetDefaultGatewayIPv6 ) (
	PRL_HANDLE hSrvConfig,
	PRL_STR sDefaultGateway,
	PRL_UINT32_PTR pnDefaultGatewayBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, sDefaultGateway=%.8X, pnDefaultGatewayBufLength=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		sDefaultGateway,
		pnDefaultGatewayBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) ||
		PRL_WRONG_PTR(pnDefaultGatewayBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return (pSrvConfig->GetDefaultGatewayIPv6(sDefaultGateway, pnDefaultGatewayBufLength));
}

PRL_METHOD( PrlSrvCfg_GetDnsServers ) (
				 PRL_HANDLE hSrvConfig,
				 PRL_HANDLE_PTR phDnsServersList
				 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, phDnsServersList=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		phDnsServersList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) ||
		PRL_WRONG_PTR(phDnsServersList))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return (pSrvConfig->GetDnsServers(phDnsServersList));
}

PRL_METHOD( PrlSrvCfg_GetSearchDomains ) (
				 PRL_HANDLE hSrvConfig,
				 PRL_HANDLE_PTR phSearchDomainsList
				 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, phSearchDomainsList=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		phSearchDomainsList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) ||
		PRL_WRONG_PTR(phSearchDomainsList))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return (pSrvConfig->GetSearchDomains(phSearchDomainsList));
}

#define GET_HW_DEVS_COUNT(device_name)\
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pn##device_name##sCount=%.8X)",\
	__FUNCTION__,\
	hSrvConfig,\
	pn##device_name##sCount\
	);\
	SYNC_CHECK_API_INITIALIZED\
	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pn##device_name##sCount))\
	return (PRL_ERR_INVALID_ARG);\
	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );\
	return pSrvConfig->Get##device_name##sCount(pn##device_name##sCount);

#define GET_HW_DEVICE(device_name)\
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, phDevice=%.8X)",\
	__FUNCTION__,\
	hSrvConfig,\
	phDevice\
	);\
	SYNC_CHECK_API_INITIALIZED\
	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(phDevice))\
	return (PRL_ERR_INVALID_ARG);\
	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );\
	return pSrvConfig->Get##device_name(nIndex, phDevice);

PRL_METHOD(PrlSrvCfg_GetFloppyDisksCount) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32_PTR pnFloppyDisksCount
	)
{
	GET_HW_DEVS_COUNT(FloppyDisk)
}

PRL_METHOD(PrlSrvCfg_GetFloppyDisk) (
									 PRL_HANDLE hSrvConfig,
									 PRL_UINT32 nIndex,
									 PRL_HANDLE_PTR phDevice
									 )
{
	GET_HW_DEVICE(FloppyDisk)
}

PRL_METHOD(PrlSrvCfg_GetOpticalDisksCount) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32_PTR pnOpticalDisksCount
	)
{
	GET_HW_DEVS_COUNT(OpticalDisk)
}

PRL_METHOD(PrlSrvCfg_GetOpticalDisk) (
									  PRL_HANDLE hSrvConfig,
									  PRL_UINT32 nIndex,
									  PRL_HANDLE_PTR phDevice
									  )
{
	GET_HW_DEVICE(OpticalDisk)
}

PRL_METHOD(PrlSrvCfg_GetSerialPortsCount) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32_PTR pnSerialPortsCount
	)
{
	GET_HW_DEVS_COUNT(SerialPort)
}

PRL_METHOD(PrlSrvCfg_GetSerialPort) (
									 PRL_HANDLE hSrvConfig,
									 PRL_UINT32 nIndex,
									 PRL_HANDLE_PTR phDevice
									 )
{
	GET_HW_DEVICE(SerialPort)
}

PRL_METHOD(PrlSrvCfg_GetParallelPortsCount) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32_PTR pnParallelPortsCount
	)
{
	GET_HW_DEVS_COUNT(ParallelPort)
}

PRL_METHOD(PrlSrvCfg_GetParallelPort) (
									   PRL_HANDLE hSrvConfig,
									   PRL_UINT32 nIndex,
									   PRL_HANDLE_PTR phDevice
									   )
{
	GET_HW_DEVICE(ParallelPort)
}

PRL_METHOD(PrlSrvCfg_GetSoundOutputDevsCount) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32_PTR pnSoundOutputDevsCount
	)
{
	GET_HW_DEVS_COUNT(SoundOutputDev)
}

PRL_METHOD(PrlSrvCfg_GetSoundOutputDev) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32 nIndex,
	PRL_HANDLE_PTR phDevice
	)
{
	GET_HW_DEVICE(SoundOutputDev)
}

PRL_METHOD(PrlSrvCfg_GetSoundMixerDevsCount) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32_PTR pnSoundMixerDevsCount
	)
{
	GET_HW_DEVS_COUNT(SoundMixerDev)
}

PRL_METHOD(PrlSrvCfg_GetSoundMixerDev) (
										PRL_HANDLE hSrvConfig,
										PRL_UINT32 nIndex,
										PRL_HANDLE_PTR phDevice
										)
{
	GET_HW_DEVICE(SoundMixerDev)
}

PRL_METHOD(PrlSrvCfg_GetPrintersCount) (
										PRL_HANDLE hSrvConfig,
										PRL_UINT32_PTR pnPrintersCount
										)
{
	GET_HW_DEVS_COUNT(Printer)
}

PRL_METHOD(PrlSrvCfg_GetPrinter) (
								  PRL_HANDLE hSrvConfig,
								  PRL_UINT32 nIndex,
								  PRL_HANDLE_PTR phDevice
								  )
{
	GET_HW_DEVICE(PrinterDev)
}

PRL_METHOD(PrlSrvCfg_GetUsbDevsCount) (
									   PRL_HANDLE hSrvConfig,
									   PRL_UINT32_PTR pnUsbDevsCount
									   )
{
	GET_HW_DEVS_COUNT(UsbDev)
}

PRL_METHOD(PrlSrvCfg_GetUsbDev) (
								 PRL_HANDLE hSrvConfig,
								 PRL_UINT32 nIndex,
								 PRL_HANDLE_PTR phDevice
								 )
{
	GET_HW_DEVICE(UsbDev)
}

PRL_METHOD(PrlSrvCfg_GetHardDisksCount) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32_PTR pnHardDisksCount
	)
{
	GET_HW_DEVS_COUNT(HardDisk)
}

PRL_METHOD(PrlSrvCfg_GetHardDisk) (
								   PRL_HANDLE hSrvConfig,
								   PRL_UINT32 nIndex,
								   PRL_HANDLE_PTR phDevice
								   )
{
	GET_HW_DEVICE(HardDisk)
}

PRL_METHOD(PrlSrvCfg_GetNetAdaptersCount) (
	PRL_HANDLE hSrvConfig,
	PRL_UINT32_PTR pnNetAdaptersCount
	)
{
	GET_HW_DEVS_COUNT(NetAdapter)
}

PRL_METHOD(PrlSrvCfg_GetNetAdapter) (
									 PRL_HANDLE hSrvConfig,
									 PRL_UINT32 nIndex,
									 PRL_HANDLE_PTR phDevice
									 )
{
	GET_HW_DEVICE(NetAdapter)
}

PRL_METHOD(PrlSrvCfg_GetGenericPciDevicesCount) (
										PRL_HANDLE hSrvConfig,
										PRL_UINT32_PTR pnGenericPciDevicesCount
										)
{
	GET_HW_DEVS_COUNT(GenericPciDevice)
}

PRL_METHOD(PrlSrvCfg_GetGenericPciDevice) (
								  PRL_HANDLE hSrvConfig,
								  PRL_UINT32 nIndex,
								  PRL_HANDLE_PTR phDevice
								  )
{
	GET_HW_DEVICE(GenericPciDeviceDev)
}

PRL_METHOD(PrlSrvCfg_GetGenericScsiDevicesCount) (
										PRL_HANDLE hSrvConfig,
										PRL_UINT32_PTR pnGenericScsiDevicesCount
										)
{
	GET_HW_DEVS_COUNT(GenericScsiDevice)
}

PRL_METHOD(PrlSrvCfg_GetGenericScsiDevice) (
								  PRL_HANDLE hSrvConfig,
								  PRL_UINT32 nIndex,
								  PRL_HANDLE_PTR phDevice
								  )
{
	GET_HW_DEVICE(GenericScsiDeviceDev)
}

PRL_METHOD( PrlSrvCfg_GetCpuFeatures) (PRL_HANDLE hSrvConfig, PRL_CPU_FEATURES_PTR pCpuFeatures)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, pCpuFeatures)", __FUNCTION__, hSrvConfig, pCpuFeatures);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(pCpuFeatures))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return (pSrvConfig->GetCpuFeatures(pCpuFeatures));
}

PRL_METHOD( PrlSrvCfg_GetCpuFeaturesEx ) (
		PRL_HANDLE hSrvConfig,
		PRL_HANDLE_PTR phCpuFeatures
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, phCpuFeatures=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		phCpuFeatures
	);

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(phCpuFeatures))
		return PRL_ERR_INVALID_ARG;

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetCpuFeaturesEx(phCpuFeatures);
}

PRL_METHOD( PrlSrvCfg_GetCpuFeaturesMaskingCapabilities ) (
		PRL_HANDLE hSrvConfig,
		PRL_HANDLE_PTR phCpuFeatures
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hSrvConfig=%.8X, phCpuFeatures=%.8X)",
		__FUNCTION__,
		hSrvConfig,
		phCpuFeatures
	);

	if (PRL_WRONG_HANDLE(hSrvConfig, PHT_SERVER_CONFIG) || PRL_WRONG_PTR(phCpuFeatures))
		return PRL_ERR_INVALID_ARG;

	PrlHandleSrvConfigPtr pSrvConfig = PRL_OBJECT_BY_HANDLE<PrlHandleSrvConfig>( hSrvConfig );

	return pSrvConfig->GetCpuFeaturesMaskingCapabilities(phCpuFeatures);
}

static bool PRL_WRONG_GENERIC_DEVICE_HANDLE(PRL_HANDLE _handle)
{
	if (PRL_WRONG_HANDLE(_handle, PHT_HW_GENERIC_DEVICE) && PRL_WRONG_HANDLE(_handle, PHT_HW_NET_ADAPTER)
		&& PRL_WRONG_HANDLE(_handle, PHT_HW_GENERIC_PCI_DEVICE))
		return (true);
	return (false);
}

PRL_METHOD(PrlSrvCfgDev_GetName) (
								  PRL_HANDLE hGenericDevice,
								  PRL_STR	sDevName,
								  PRL_UINT32_PTR pnDevNameBufLength
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hGenericDevice=%.8X, sDevName=%.8X, pnDevNameBufLength=%.8X)",
		__FUNCTION__,
		hGenericDevice,
		sDevName,
		pnDevNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_GENERIC_DEVICE_HANDLE(hGenericDevice) ||
		PRL_WRONG_PTR(pnDevNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwGenericDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericDevice>( hGenericDevice );

	return (pDevice->GetName(sDevName, pnDevNameBufLength));
}

PRL_METHOD(PrlSrvCfgDev_GetId) (
								PRL_HANDLE hGenericDevice,
								PRL_STR	sDevId,
								PRL_UINT32_PTR pnDevIdBufLength
								)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hGenericDevice=%.8X, sDevId=%.8X, pnDevIdBufLength=%.8X)",
		__FUNCTION__,
		hGenericDevice,
		sDevId,
		pnDevIdBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_GENERIC_DEVICE_HANDLE(hGenericDevice) ||
		PRL_WRONG_PTR(pnDevIdBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwGenericDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericDevice>( hGenericDevice );

	return (pDevice->GetId(sDevId, pnDevIdBufLength));
}

PRL_METHOD(PrlSrvCfgDev_GetType) (
								  PRL_HANDLE hGenericDevice,
								  PRL_DEVICE_TYPE_PTR pnDevType
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hGenericDevice=%.8X, pnDevType=%.8X)",
		__FUNCTION__,
		hGenericDevice,
		pnDevType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_GENERIC_DEVICE_HANDLE(hGenericDevice) ||
		PRL_WRONG_PTR(pnDevType))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwGenericDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericDevice>( hGenericDevice );

	return (pDevice->GetDevType(pnDevType));
}

PRL_METHOD( PrlSrvCfgDev_IsConnectedToVm ) (
		PRL_HANDLE hGenericDevice,
		PRL_BOOL_PTR pbConnected
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hGenericDevice=%.8X, pbConnected=%.8X)",
		__FUNCTION__,
		hGenericDevice,
		pbConnected
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_GENERIC_DEVICE_HANDLE(hGenericDevice) ||
		PRL_WRONG_PTR(pbConnected))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwGenericDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericDevice>( hGenericDevice );

	return (pDevice->IsConnectedToVm(pbConnected));
}

PRL_METHOD( PrlSrvCfgDev_GetDeviceState ) (
		PRL_HANDLE hGenericDevice,
		PRL_GENERIC_DEVICE_STATE_PTR pnDeviceState
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hGenericDevice=%.8X, pnDeviceState=%.8X)",
		__FUNCTION__,
		hGenericDevice,
		pnDeviceState
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_GENERIC_DEVICE_HANDLE(hGenericDevice) ||
		PRL_WRONG_PTR(pnDeviceState))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwGenericDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericDevice>( hGenericDevice );

	return (pDevice->GetDeviceState(pnDeviceState));
}

PRL_METHOD( PrlSrvCfgDev_SetDeviceState ) (
		PRL_HANDLE hGenericDevice,
		PRL_GENERIC_DEVICE_STATE nDeviceState
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hGenericDevice=%.8X, nDeviceState=%.8X)",
		__FUNCTION__,
		hGenericDevice,
		nDeviceState
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_GENERIC_DEVICE_HANDLE(hGenericDevice))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwGenericDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericDevice>( hGenericDevice );

	return (pDevice->SetDeviceState(nDeviceState));
}

PRL_METHOD(PrlSrvCfgHdd_GetDevName) (
									 PRL_HANDLE hHardDisk,
									 PRL_STR	sDevName,
									 PRL_UINT32_PTR pnDevNameBufLength
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHardDisk=%.8X, sDevName=%.8X, pnDevNameBufLength=%.8X)",
		__FUNCTION__,
		hHardDisk,
		sDevName,
		pnDevNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHardDisk, PHT_HW_HARD_DISK) ||
		PRL_WRONG_PTR(pnDevNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHardDiskPtr pHardDisk = PRL_OBJECT_BY_HANDLE<PrlHandleHwHardDisk>( hHardDisk );

	return (pHardDisk->GetDevName(sDevName, pnDevNameBufLength));
}

PRL_METHOD(PrlSrvCfgHdd_GetDevId) (
								   PRL_HANDLE hHardDisk,
								   PRL_STR	sDevId,
								   PRL_UINT32_PTR pnDevIdBufLength
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHardDisk=%.8X, sDevId=%.8X, pnDevIdBufLength=%.8X)",
		__FUNCTION__,
		hHardDisk,
		sDevId,
		pnDevIdBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHardDisk, PHT_HW_HARD_DISK) ||
		PRL_WRONG_PTR(pnDevIdBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHardDiskPtr pHardDisk = PRL_OBJECT_BY_HANDLE<PrlHandleHwHardDisk>( hHardDisk );

	return (pHardDisk->GetDevId(sDevId, pnDevIdBufLength));
}

PRL_METHOD(PrlSrvCfgHdd_GetDevSize) (
									 PRL_HANDLE hHardDisk,
									 PRL_UINT64_PTR pnSize
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHardDisk=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hHardDisk,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHardDisk, PHT_HW_HARD_DISK) ||
		PRL_WRONG_PTR(pnSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHardDiskPtr pHardDisk = PRL_OBJECT_BY_HANDLE<PrlHandleHwHardDisk>( hHardDisk );

	return (pHardDisk->GetDevSize(pnSize));
}

PRL_METHOD(PrlSrvCfgHdd_GetDiskIndex) (
									   PRL_HANDLE hHardDisk,
									   PRL_UINT32_PTR pnIndex
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHardDisk=%.8X, pnIndex=%.8X)",
		__FUNCTION__,
		hHardDisk,
		pnIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHardDisk, PHT_HW_HARD_DISK) ||
		PRL_WRONG_PTR(pnIndex))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHardDiskPtr pHardDisk = PRL_OBJECT_BY_HANDLE<PrlHandleHwHardDisk>( hHardDisk );

	return (pHardDisk->GetDiskIndex(pnIndex));
}

PRL_METHOD(PrlSrvCfgHdd_GetPartsCount) (
										PRL_HANDLE hHardDisk,
										PRL_UINT32_PTR pnPartsCount
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHardDisk=%.8X, pnPartsCount=%.8X)",
		__FUNCTION__,
		hHardDisk,
		pnPartsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHardDisk, PHT_HW_HARD_DISK) ||
		PRL_WRONG_PTR(pnPartsCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHardDiskPtr pHardDisk = PRL_OBJECT_BY_HANDLE<PrlHandleHwHardDisk>( hHardDisk );

	return (pHardDisk->GetPartsCount(pnPartsCount));
}

PRL_METHOD(PrlSrvCfgHdd_GetPart) (
								  PRL_HANDLE hHardDisk,
								  PRL_UINT32 nIndex,
								  PRL_HANDLE_PTR phPart
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHardDisk=%.8X, phPart=%.8X)",
		__FUNCTION__,
		hHardDisk,
		phPart
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHardDisk, PHT_HW_HARD_DISK) ||
		PRL_WRONG_PTR(phPart))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHardDiskPtr pHardDisk = PRL_OBJECT_BY_HANDLE<PrlHandleHwHardDisk>( hHardDisk );

	return (pHardDisk->GetPart(nIndex, phPart));
}

PRL_METHOD(PrlSrvCfgHddPart_GetName) (
									  PRL_HANDLE hPart,
									  PRL_STR	sName,
									  PRL_UINT32_PTR pnNameBufLength
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPart=%.8X, sName=%.8X, pnNameBufLength=%.8X)",
		__FUNCTION__,
		hPart,
		sName,
		pnNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPart, PHT_HW_HARD_DISK_PARTITION) ||
		PRL_WRONG_PTR(pnNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHddPartPtr pHddPart = PRL_OBJECT_BY_HANDLE<PrlHandleHwHddPart>( hPart );

	return (pHddPart->GetName(sName, pnNameBufLength));
}

PRL_METHOD(PrlSrvCfgHddPart_GetSysName) (
									  PRL_HANDLE hPart,
									  PRL_STR	sSysName,
									  PRL_UINT32_PTR pnSysNameBufLength
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPart=%.8X, sSysName=%.8X, pnSysNameBufLength=%.8X)",
		__FUNCTION__,
		hPart,
		sSysName,
		pnSysNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPart, PHT_HW_HARD_DISK_PARTITION) ||
		PRL_WRONG_PTR(pnSysNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHddPartPtr pHddPart = PRL_OBJECT_BY_HANDLE<PrlHandleHwHddPart>( hPart );

	return (pHddPart->GetSysName(sSysName, pnSysNameBufLength));
}

PRL_METHOD(PrlSrvCfgHddPart_GetSize) (
									  PRL_HANDLE hPart,
									  PRL_UINT64_PTR pnSize
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPart=%.8X, pnSize=%.8X)",
		__FUNCTION__,
		hPart,
		pnSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPart, PHT_HW_HARD_DISK_PARTITION) ||
		PRL_WRONG_PTR(pnSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHddPartPtr pHddPart = PRL_OBJECT_BY_HANDLE<PrlHandleHwHddPart>( hPart );

	return (pHddPart->GetSize(pnSize));
}

PRL_METHOD(PrlSrvCfgHddPart_GetIndex) (
									   PRL_HANDLE hPart,
									   PRL_UINT32_PTR pnIndex
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPart=%.8X, pnIndex=%.8X)",
		__FUNCTION__,
		hPart,
		pnIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPart, PHT_HW_HARD_DISK_PARTITION) ||
		PRL_WRONG_PTR(pnIndex))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHddPartPtr pHddPart = PRL_OBJECT_BY_HANDLE<PrlHandleHwHddPart>( hPart );

	return (pHddPart->GetIndex(pnIndex));
}

PRL_METHOD(PrlSrvCfgHddPart_GetType) (
									  PRL_HANDLE hPart,
									  PRL_UINT32_PTR pnType
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPart=%.8X, pnType=%.8X)",
		__FUNCTION__,
		hPart,
		pnType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPart, PHT_HW_HARD_DISK_PARTITION) ||
		PRL_WRONG_PTR(pnType))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHddPartPtr pHddPart = PRL_OBJECT_BY_HANDLE<PrlHandleHwHddPart>( hPart );

	return (pHddPart->GetPartType(pnType));
}

PRL_METHOD(PrlSrvCfgHddPart_IsInUse) (
									  PRL_HANDLE hPart,
									  PRL_BOOL_PTR pbInUse
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPart=%.8X, pbInUse=%.8X)",
		__FUNCTION__,
		hPart,
		pbInUse
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPart, PHT_HW_HARD_DISK_PARTITION) ||
		PRL_WRONG_PTR(pbInUse))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHddPartPtr pHddPart = PRL_OBJECT_BY_HANDLE<PrlHandleHwHddPart>( hPart );

	return (pHddPart->IsInUse(pbInUse));
}

PRL_METHOD(PrlSrvCfgHddPart_IsLogical) (
										PRL_HANDLE hPart,
										PRL_BOOL_PTR pbLogical
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPart=%.8X, pbLogical=%.8X)",
		__FUNCTION__,
		hPart,
		pbLogical
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPart, PHT_HW_HARD_DISK_PARTITION) ||
		PRL_WRONG_PTR(pbLogical))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHddPartPtr pHddPart = PRL_OBJECT_BY_HANDLE<PrlHandleHwHddPart>( hPart );

	return (pHddPart->IsLogical(pbLogical));
}

PRL_METHOD(PrlSrvCfgHddPart_IsActive) (
									   PRL_HANDLE hPart,
									   PRL_BOOL_PTR pbActive
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hPart=%.8X, pbActive=%.8X)",
		__FUNCTION__,
		hPart,
		pbActive
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hPart, PHT_HW_HARD_DISK_PARTITION) ||
		PRL_WRONG_PTR(pbActive))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwHddPartPtr pHddPart = PRL_OBJECT_BY_HANDLE<PrlHandleHwHddPart>( hPart );

	return (pHddPart->IsActive(pbActive));
}

PRL_METHOD(PrlSrvCfgNet_GetNetAdapterType) (
	PRL_HANDLE hNetAdapter,
	PRL_HW_INFO_NET_ADAPTER_TYPE_PTR pnNetAdapterType
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, pnNetAdapterType=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		pnNetAdapterType
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnNetAdapterType))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->GetNetAdapterType(pnNetAdapterType));
}

PRL_METHOD(PrlSrvCfgNet_GetSysIndex) (
									  PRL_HANDLE hNetAdapter,
									  PRL_UINT32_PTR pnSysIndex
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, pnSysIndex=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		pnSysIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnSysIndex))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->GetSysIndex(pnSysIndex));
}

PRL_METHOD(PrlSrvCfgNet_IsEnabled) (
									PRL_HANDLE hNetAdapter,
									PRL_BOOL_PTR pbEnabled
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(pbEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->IsEnabled(pbEnabled));
}

PRL_METHOD(PrlSrvCfgNet_IsConfigureWithDhcp) (
									PRL_HANDLE hNetAdapter,
									PRL_BOOL_PTR pbConfigureWithDhcp
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, pbConfigureWithDhcp=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		pbConfigureWithDhcp
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(pbConfigureWithDhcp))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->IsConfigureWithDhcp(pbConfigureWithDhcp));
}

PRL_METHOD(PrlSrvCfgNet_IsConfigureWithDhcpIPv6) (
									PRL_HANDLE hNetAdapter,
									PRL_BOOL_PTR pbConfigureWithDhcp
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, pbConfigureWithDhcp=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		pbConfigureWithDhcp
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(pbConfigureWithDhcp))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->IsConfigureWithDhcpIPv6(pbConfigureWithDhcp));
}

PRL_METHOD(PrlSrvCfgNet_GetDefaultGateway) (
									  PRL_HANDLE hNetAdapter,
									  PRL_STR	sDefaultGateway,
									  PRL_UINT32_PTR pnDefaultGatewayBufLength
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, sDefaultGateway=%.8X, pnDefaultGatewayBufLength=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		sDefaultGateway,
		pnDefaultGatewayBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnDefaultGatewayBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->GetDefaultGateway(sDefaultGateway, pnDefaultGatewayBufLength));
}

PRL_METHOD(PrlSrvCfgNet_GetDefaultGatewayIPv6) (
									  PRL_HANDLE hNetAdapter,
									  PRL_STR	sDefaultGateway,
									  PRL_UINT32_PTR pnDefaultGatewayBufLength
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, sDefaultGateway=%.8X, pnDefaultGatewayBufLength=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		sDefaultGateway,
		pnDefaultGatewayBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnDefaultGatewayBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->GetDefaultGatewayIPv6(sDefaultGateway, pnDefaultGatewayBufLength));
}

PRL_METHOD(PrlSrvCfgNet_GetMacAddress) (
									  PRL_HANDLE hNetAdapter,
									  PRL_STR	sMacAddress,
									  PRL_UINT32_PTR pnMacAddressBufLength
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, sMacAddress=%.8X, pnMacAddressBufLength=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		sMacAddress,
		pnMacAddressBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnMacAddressBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->GetMacAddress(sMacAddress, pnMacAddressBufLength));
}

PRL_METHOD( PrlSrvCfgNet_GetNetAddresses ) (
				 PRL_HANDLE hNetAdapter,
				 PRL_HANDLE_PTR phNetAddressesList
				 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, phNetAddressesList=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		phNetAddressesList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(phNetAddressesList))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->GetNetAddresses(phNetAddressesList));
}

PRL_METHOD( PrlSrvCfgNet_GetDnsServers ) (
				 PRL_HANDLE hNetAdapter,
				 PRL_HANDLE_PTR phDnsServersList
				 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, phDnsServersList=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		phDnsServersList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(phDnsServersList))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->GetDnsServers(phDnsServersList));
}

PRL_METHOD( PrlSrvCfgNet_GetSearchDomains ) (
				 PRL_HANDLE hNetAdapter,
				 PRL_HANDLE_PTR phSearchDomainsList
				 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, phSearchDomainsList=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		phSearchDomainsList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(phSearchDomainsList))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->GetSearchDomains(phSearchDomainsList));
}

PRL_METHOD(PrlSrvCfgNet_GetVlanTag) (
									  PRL_HANDLE hNetAdapter,
									  PRL_UINT16_PTR pnVlanTag
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hNetAdapter=%.8X, pnVlanTag=%.8X)",
		__FUNCTION__,
		hNetAdapter,
		pnVlanTag
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hNetAdapter, PHT_HW_NET_ADAPTER) ||
		PRL_WRONG_PTR(pnVlanTag))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwNetAdapterPtr pNetAdapter = PRL_OBJECT_BY_HANDLE<PrlHandleHwNetAdapter>( hNetAdapter );

	return (pNetAdapter->GetVlanTag(pnVlanTag));
}

PRL_METHOD( PrlSrvCfgPci_GetDeviceClass ) (
		PRL_HANDLE hGenericPci,
		PRL_GENERIC_PCI_DEVICE_CLASS_PTR pnGenericPciClass
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hGenericPci=%.8X, pnGenericPciClass=%.8X)",
		__FUNCTION__,
		hGenericPci,
		pnGenericPciClass
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hGenericPci, PHT_HW_GENERIC_PCI_DEVICE) ||
		PRL_WRONG_PTR(pnGenericPciClass))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHwGenericPciDevicePtr pGenericPci = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericPciDevice>( hGenericPci );

	return (pGenericPci->GetDeviceClass(pnGenericPciClass));
}

PRL_METHOD( PrlSrvCfgPci_IsPrimaryDevice ) (
	PRL_HANDLE hGenericPci,
	PRL_BOOL_PTR pbIsPrimaryDevice
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hGenericPci=%.8X, pbIsPrimaryDevice=%.8X)",
		__FUNCTION__,
		hGenericPci,
		pbIsPrimaryDevice
		);

	SYNC_CHECK_API_INITIALIZED

		if (PRL_WRONG_HANDLE(hGenericPci, PHT_HW_GENERIC_PCI_DEVICE) ||
			PRL_WRONG_PTR(pbIsPrimaryDevice))
			return (PRL_ERR_INVALID_ARG);

	PrlHandleHwGenericPciDevicePtr pGenericPci = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericPciDevice>( hGenericPci );

	return (pGenericPci->IsPrimary(pbIsPrimaryDevice));
}

PRL_METHOD( PrlUsrCfg_GetVmDirUuid ) (
									  PRL_HANDLE hUserProfile,
									  PRL_STR sVmDirUuid,
									  PRL_UINT32_PTR pnVmDirUuidBufLength
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserProfile=%.8X, sVmDirUuid=%.8X, pnVmDirUuidBufLength=%.8X)",
		__FUNCTION__,
		hUserProfile,
		sVmDirUuid,
		pnVmDirUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserProfile, PHT_USER_PROFILE) ||
		PRL_WRONG_PTR(pnVmDirUuidBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUserProfile );

	return (pUserProfile->GetVmDirUuid(sVmDirUuid, pnVmDirUuidBufLength));
}

PRL_METHOD( PrlUsrCfg_SetVmDirUuid ) (
									  PRL_HANDLE hUserProfile,
									  PRL_CONST_STR sNewVmDirUuid
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserProfile=%.8X, sNewVmDirUuid=%.8X)",
		__FUNCTION__,
		hUserProfile,
		sNewVmDirUuid
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserProfile, PHT_USER_PROFILE) ||
		PRL_WRONG_PTR(sNewVmDirUuid))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUserProfile );

	return (pUserProfile->SetVmDirUuid(sNewVmDirUuid));
}

PRL_METHOD( PrlUsrCfg_GetDefaultVmFolder ) (
	PRL_HANDLE hUserProfile,
	PRL_STR sDefaultVmFolder,
	PRL_UINT32_PTR pnDefaultVmFolderBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserProfile=%.8X, sDefaultVmFolder=%.8X, pnDefaultVmFolderBufLength=%.8X)",
		__FUNCTION__,
		hUserProfile,
		sDefaultVmFolder,
		pnDefaultVmFolderBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserProfile, PHT_USER_PROFILE) ||
		PRL_WRONG_PTR(pnDefaultVmFolderBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUserProfile );

	return (pUserProfile->GetDefaultVmFolder(sDefaultVmFolder, pnDefaultVmFolderBufLength));
}

PRL_METHOD( PrlUsrCfg_SetDefaultVmFolder ) (
	PRL_HANDLE hUserProfile,
	PRL_CONST_STR sNewDefaultVmFolder
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserProfile=%.8X, sNewDefaultVmFolder=%.8X)",
		__FUNCTION__,
		hUserProfile,
		sNewDefaultVmFolder
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserProfile, PHT_USER_PROFILE) ||
		PRL_WRONG_PTR(sNewDefaultVmFolder))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUserProfile );

	return (pUserProfile->SetDefaultVmFolder(sNewDefaultVmFolder));
}

PRL_METHOD( PrlUsrCfg_CanUseMngConsole ) (
	PRL_HANDLE hUserProfile,
	PRL_BOOL_PTR pbCanUseMngConsole
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserProfile=%.8X, pbCanUseMngConsole=%.8X)",
		__FUNCTION__,
		hUserProfile,
		pbCanUseMngConsole
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserProfile, PHT_USER_PROFILE) ||
		PRL_WRONG_PTR(pbCanUseMngConsole))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUserProfile );

	return (pUserProfile->CanUseMngConsole(pbCanUseMngConsole));
}

PRL_METHOD( PrlUsrCfg_CanChangeSrvSets ) (
	PRL_HANDLE hUserProfile,
	PRL_BOOL_PTR pbCanChangeSrvSets
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserProfile=%.8X, pbCanChangeSrvSets=%.8X)",
		__FUNCTION__,
		hUserProfile,
		pbCanChangeSrvSets
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserProfile, PHT_USER_PROFILE) ||
		PRL_WRONG_PTR(pbCanChangeSrvSets))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUserProfile );

	return (pUserProfile->CanChangeSrvSets(pbCanChangeSrvSets));
}

PRL_METHOD( PrlUsrCfg_IsLocalAdministrator ) (
	PRL_HANDLE hUserProfile,
	PRL_BOOL_PTR pbIsLocalAdministrator
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserProfile=%.8X, pbIsLocalAdministrator=%.8X)",
		__FUNCTION__,
		hUserProfile,
		pbIsLocalAdministrator
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserProfile, PHT_USER_PROFILE) ||
		PRL_WRONG_PTR(pbIsLocalAdministrator))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>( hUserProfile );

	return (pUserProfile->IsLocalAdministrator(pbIsLocalAdministrator));
}

PRL_METHOD( PrlUsrInfo_GetName ) (
								  PRL_HANDLE hUserInfo,
								  PRL_STR sName,
								  PRL_UINT32_PTR pnNameBufLength
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserInfo=%.8X, sName=%.8X, pnNameBufLength=%.8X)",
		__FUNCTION__,
		hUserInfo,
		sName,
		pnNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserInfo, PHT_USER_INFO) ||
		PRL_WRONG_PTR(pnNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserInfoPtr pUserInfo = PRL_OBJECT_BY_HANDLE<PrlHandleUserInfo>( hUserInfo );

	return (pUserInfo->GetName(sName, pnNameBufLength));
}

PRL_METHOD( PrlUsrInfo_GetUuid ) (
								  PRL_HANDLE hUserInfo,
								  PRL_STR sUuid,
								  PRL_UINT32_PTR pnUuidBufLength
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserInfo=%.8X, sUuid=%.8X, pnUuidBufLength=%.8X)",
		__FUNCTION__,
		hUserInfo,
		sUuid,
		pnUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserInfo, PHT_USER_INFO) ||
		PRL_WRONG_PTR(pnUuidBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserInfoPtr pUserInfo = PRL_OBJECT_BY_HANDLE<PrlHandleUserInfo>( hUserInfo );

	return (pUserInfo->GetUuid(sUuid, pnUuidBufLength));
}

PRL_METHOD( PrlUsrInfo_GetSessionCount ) (
	PRL_HANDLE hUserInfo,
	PRL_UINT32_PTR pnSessionCount
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserInfo=%.8X, pnSessionCount=%.8X)",
		__FUNCTION__,
		hUserInfo,
		pnSessionCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserInfo, PHT_USER_INFO) ||
		PRL_WRONG_PTR(pnSessionCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserInfoPtr pUserInfo = PRL_OBJECT_BY_HANDLE<PrlHandleUserInfo>( hUserInfo );

	return (pUserInfo->GetSessionCount(pnSessionCount));
}

PRL_METHOD( PrlUsrInfo_GetDefaultVmFolder ) (
	PRL_HANDLE hUserInfo,
	PRL_STR sDefaultVmFolder,
	PRL_UINT32_PTR pnDefaultVmFolderBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserInfo=%.8X, sDefaultVmFolder=%.8X, pnDefaultVmFolderBufLength=%.8X)",
		__FUNCTION__,
		hUserInfo,
		sDefaultVmFolder,
		pnDefaultVmFolderBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserInfo, PHT_USER_INFO) ||
		PRL_WRONG_PTR(pnDefaultVmFolderBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserInfoPtr pUserInfo = PRL_OBJECT_BY_HANDLE<PrlHandleUserInfo>( hUserInfo );

	return (pUserInfo->GetDefaultVmFolder(sDefaultVmFolder, pnDefaultVmFolderBufLength));
}

PRL_METHOD( PrlUsrInfo_CanChangeSrvSets ) (
	PRL_HANDLE hUserInfo,
	PRL_BOOL_PTR pbCanChangeSrvSets
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hUserInfo=%.8X, pbCanChangeSrvSets=%.8X)",
		__FUNCTION__,
		hUserInfo,
		pbCanChangeSrvSets
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hUserInfo, PHT_USER_INFO) ||
		PRL_WRONG_PTR(pbCanChangeSrvSets))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleUserInfoPtr pUserInfo = PRL_OBJECT_BY_HANDLE<PrlHandleUserInfo>( hUserInfo );

	return (pUserInfo->CanChangeSrvSets(pbCanChangeSrvSets));
}

PRL_METHOD( PrlDispCfg_GetDefaultVmDir ) (
	PRL_HANDLE hDispConfig,
	PRL_STR sDefaultVmDir,
	PRL_UINT32_PTR pnDefaultVmDirBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sDefaultVmDir=%.8X, pnDefaultVmDirBufLength=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sDefaultVmDir,
		pnDefaultVmDirBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnDefaultVmDirBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetDefaultVmDir(sDefaultVmDir, pnDefaultVmDirBufLength));
}

PRL_METHOD( PrlDispCfg_GetDefaultCtDir ) (
	PRL_HANDLE hDispConfig,
	PRL_STR sDefaultVmDir,
	PRL_UINT32_PTR pnDefaultVmDirBufLength
	)
{
	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnDefaultVmDirBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetDefaultCtDir(sDefaultVmDir, pnDefaultVmDirBufLength));
}

PRL_METHOD( PrlDispCfg_GetReservedMemLimit ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32_PTR pnMemSize
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pnMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pnMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnMemSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetReservedMemLimit(pnMemSize));
}

PRL_METHOD( PrlDispCfg_SetReservedMemLimit ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32 nMemSize
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		nMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetReservedMemLimit(nMemSize));
}

PRL_METHOD( PrlDispCfg_GetMinVmMem ) (
									  PRL_HANDLE hDispConfig,
									  PRL_UINT32_PTR pnMemSize
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pnMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pnMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnMemSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetMinVmMem(pnMemSize));
}

PRL_METHOD( PrlDispCfg_SetMinVmMem ) (
									  PRL_HANDLE hDispConfig,
									  PRL_UINT32 nMemSize
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		nMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetMinVmMem(nMemSize));
}

PRL_METHOD( PrlDispCfg_GetMaxVmMem ) (
									  PRL_HANDLE hDispConfig,
									  PRL_UINT32_PTR pnMemSize
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pnMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pnMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnMemSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetMaxVmMem(pnMemSize));
}

PRL_METHOD( PrlDispCfg_SetMaxVmMem ) (
									  PRL_HANDLE hDispConfig,
									  PRL_UINT32 nMemSize
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		nMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetMaxVmMem(nMemSize));
}

PRL_METHOD( PrlDispCfg_GetRecommendMaxVmMem ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32_PTR pnMemSize
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pnMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pnMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnMemSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetRecommendMaxVmMem(pnMemSize));
}

PRL_METHOD( PrlDispCfg_SetRecommendMaxVmMem ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32 nMemSize
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		nMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetRecommendMaxVmMem(nMemSize));
}

PRL_METHOD( PrlDispCfg_GetMaxReservMemLimit ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32_PTR pnMemSize
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pnMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pnMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnMemSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetMaxReservMemLimit(pnMemSize));
}

PRL_METHOD( PrlDispCfg_SetMaxReservMemLimit ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32 nMemSize
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		nMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetMaxReservMemLimit(nMemSize));
}

PRL_METHOD( PrlDispCfg_GetMinReservMemLimit ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32_PTR pnMemSize
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pnMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pnMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnMemSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetMinReservMemLimit(pnMemSize));
}

PRL_METHOD( PrlDispCfg_SetMinReservMemLimit ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32 nMemSize
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nMemSize=%.8X)",
		__FUNCTION__,
		hDispConfig,
		nMemSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetMinReservMemLimit(nMemSize));
}

PRL_METHOD( PrlDispCfg_IsAdjustMemAuto ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL_PTR pbAdjustMemAuto
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pbAdjustMemAuto=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pbAdjustMemAuto
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pbAdjustMemAuto))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->IsAdjustMemAuto(pbAdjustMemAuto));
}

PRL_METHOD( PrlDispCfg_SetAdjustMemAuto ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL bAdjustMemAuto
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, bAdjustMemAuto=%.8X)",
		__FUNCTION__,
		hDispConfig,
		bAdjustMemAuto
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetAdjustMemAuto(bAdjustMemAuto));
}

PRL_METHOD( PrlDispCfg_IsSendStatisticReport ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL_PTR pbSendStatisticReport
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pbSendStatisticReport=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pbSendStatisticReport
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pbSendStatisticReport))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->IsSendStatisticReport(pbSendStatisticReport));
}

PRL_METHOD( PrlDispCfg_SetSendStatisticReport ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL bSendStatisticReport
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, bSendStatisticReport=%.8X)",
		__FUNCTION__,
		hDispConfig,
		bSendStatisticReport
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetSendStatisticReport(bSendStatisticReport));
}

PRL_METHOD( PrlDispCfg_GetDefaultVNCHostName) (
	PRL_HANDLE hDispConfig,
	PRL_STR sBaseHostName,
	PRL_UINT32_PTR pnBaseHostNameBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sBaseHostName=%.8X, pnBaseHostNameBufLength=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sBaseHostName,
		pnBaseHostNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnBaseHostNameBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetRemoteDisplayBaseHostName(sBaseHostName, pnBaseHostNameBufLength));
}

PRL_METHOD( PrlDispCfg_SetDefaultVNCHostName) (
	PRL_HANDLE hDispConfig,
	PRL_CONST_STR sNewBaseHostName
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sNewBaseHostName=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sNewBaseHostName
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(sNewBaseHostName))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetRemoteDisplayBaseHostName(sNewBaseHostName));
}

PRL_METHOD( PrlDispCfg_GetVNCBasePort ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32_PTR pnPort
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pnPort=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pnPort
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnPort))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetRemoteDisplayBasePort(pnPort));
}

PRL_METHOD( PrlDispCfg_SetVNCBasePort ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32 nPort
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nPort=%.8X)",
		__FUNCTION__,
		hDispConfig,
		nPort
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetRemoteDisplayBasePort(nPort));
}

PRL_METHOD( PrlDispCfg_CanChangeDefaultSettings ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL_PTR pbDefaultChangeSettings
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pbDefaultChangeSettings=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pbDefaultChangeSettings
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pbDefaultChangeSettings))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->CanChangeDefaultSettings(pbDefaultChangeSettings));
}

PRL_METHOD( PrlDispCfg_SetCanChangeDefaultSettings ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL bDefaultChangeSettings
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, bDefaultChangeSettings=%.8X)",
		__FUNCTION__,
		hDispConfig,
		bDefaultChangeSettings
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetCanChangeDefaultSettings(bDefaultChangeSettings));
}

PRL_METHOD( PrlDispCfg_GetMinSecurityLevel ) (
	PRL_HANDLE hDispConfig,
	PRL_SECURITY_LEVEL_PTR pnMinSecurityLevel
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pnMinSecurityLevel=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pnMinSecurityLevel
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnMinSecurityLevel))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetMinSecurityLevel(pnMinSecurityLevel));
}

PRL_METHOD( PrlDispCfg_SetMinSecurityLevel ) (
	PRL_HANDLE hDispConfig,
	PRL_SECURITY_LEVEL nMinSecurityLevel
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nMinSecurityLevel=%.8X)",
		__FUNCTION__,
		hDispConfig,
		nMinSecurityLevel
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetMinSecurityLevel(nMinSecurityLevel));
}

PRL_METHOD( PrlDispCfg_GetConfirmationsList ) (
	PRL_HANDLE hDispConfig,
	PRL_HANDLE_PTR phConfirmList
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, phConfirmList=%.8X)",
		__FUNCTION__,
		hDispConfig,
		phConfirmList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(phConfirmList))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetConfirmationsList(phConfirmList));
}

PRL_METHOD( PrlDispCfg_GetCpuPool ) (
	PRL_HANDLE hDispConfig,
	PRL_HANDLE_PTR phCpuPool
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, phCpuPool=%.8X)",
		__FUNCTION__,
		hDispConfig,
		phCpuPool
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(phCpuPool))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetCpuPool(phCpuPool));
}

PRL_METHOD( PrlDispCfg_SetConfirmationsList ) (
	PRL_HANDLE hDispConfig,
	PRL_HANDLE hConfirmList
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, hConfirmList=%.8X)",
		__FUNCTION__,
		hDispConfig,
		hConfirmList
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG)
		 || PRL_WRONG_HANDLE(hConfirmList, PHT_OPAQUE_TYPE_LIST))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );
	PrlHandleOpTypeListPtr pList = PRL_OBJECT_BY_HANDLE<PrlHandleOpTypeList>(hConfirmList);

	return (pDispConfig->SetConfirmationsList(pList));
}

PRL_METHOD( PrlDispCfg_GetDefaultBackupServer) (
	PRL_HANDLE hDispConfig,
	PRL_STR sBackupServer,
	PRL_UINT32_PTR pnBackupServerBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sBackupServer=%.8X, pnBackupServerBufLength=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sBackupServer,
		pnBackupServerBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnBackupServerBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetDefaultBackupServer(sBackupServer, pnBackupServerBufLength));
}

PRL_METHOD( PrlDispCfg_SetDefaultBackupServer) (
	PRL_HANDLE hDispConfig,
	PRL_CONST_STR sBackupServer
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sBackupServer=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sBackupServer
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetDefaultBackupServer(sBackupServer));
}

PRL_METHOD( PrlDispCfg_GetBackupTmpDir) (
	PRL_HANDLE hDispConfig,
	PRL_STR sBackupTmpDir,
	PRL_UINT32_PTR pnBackupTmpDirBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sBackupTmpDir=%.8X, pnBackupTmpDirBufLength=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sBackupTmpDir,
		pnBackupTmpDirBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnBackupTmpDirBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetBackupTmpDir(sBackupTmpDir, pnBackupTmpDirBufLength));
}

PRL_METHOD( PrlDispCfg_SetBackupTmpDir) (
	PRL_HANDLE hDispConfig,
	PRL_CONST_STR sBackupTmpDir
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sBackupTmpDir=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sBackupTmpDir
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetBackupTmpDir(sBackupTmpDir));
}

PRL_METHOD( PrlDispCfg_GetBackupUserLogin) (
	PRL_HANDLE hDispConfig,
	PRL_STR sUserLogin,
	PRL_UINT32_PTR pnUserLoginBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sUserLogin=%.8X, pnUserLoginBufLength=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sUserLogin,
		pnUserLoginBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnUserLoginBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetBackupUserLogin(sUserLogin, pnUserLoginBufLength));
}

PRL_METHOD( PrlDispCfg_SetBackupUserLogin) (
	PRL_HANDLE hDispConfig,
	PRL_CONST_STR sUserLogin
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sUserLogin=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sUserLogin
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetBackupUserLogin(sUserLogin));
}

PRL_METHOD( PrlDispCfg_SetBackupUserPassword) (
	PRL_HANDLE hDispConfig,
	PRL_CONST_STR sUserPassword
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sUserPassword=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sUserPassword
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetBackupUserPassword(sUserPassword));
}

PRL_METHOD( PrlDispCfg_IsBackupUserPasswordEnabled) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL_PTR pbUserPasswordEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pbUserPasswordEnabled=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pbUserPasswordEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pbUserPasswordEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->IsBackupUserPasswordEnabled(pbUserPasswordEnabled));
}

PRL_METHOD( PrlDispCfg_SetBackupUserPasswordEnabled ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL bUserPasswordEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, bUserPasswordEnabled=%.8X)",
		__FUNCTION__,
		hDispConfig,
		bUserPasswordEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetBackupUserPasswordEnabled(bUserPasswordEnabled));
}

PRL_METHOD( PrlDispCfg_GetDefaultBackupDirectory ) (
	PRL_HANDLE hDispConfig,
	PRL_STR sBackupDirectory,
	PRL_UINT32_PTR pnBackupDirectoryBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sBackupDirectory=%.8X, pnBackupDirectoryBufLength=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sBackupDirectory,
		pnBackupDirectoryBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnBackupDirectoryBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetDefaultBackupDirectory(sBackupDirectory, pnBackupDirectoryBufLength));
}

PRL_METHOD( PrlDispCfg_SetDefaultBackupDirectory ) (
	PRL_HANDLE hDispConfig,
	PRL_CONST_STR sBackupDirectory
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sBackupDirectory=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sBackupDirectory
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetDefaultBackupDirectory(sBackupDirectory));
}

PRL_METHOD( PrlDispCfg_GetBackupTimeout ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32_PTR pnBackupTimeout
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pnBackupTimeout=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pnBackupTimeout
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) || PRL_WRONG_PTR(pnBackupTimeout))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetBackupTimeout(pnBackupTimeout));
}

PRL_METHOD( PrlDispCfg_SetBackupTimeout ) (
	PRL_HANDLE hDispConfig,
	PRL_UINT32 nTimeout
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nTimeout=%u)",
		__FUNCTION__,
		hDispConfig,
		nTimeout
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetBackupTimeout(nTimeout));
}

PRL_METHOD( PrlDispCfg_ArePluginsEnabled ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL_PTR pbIsPluginsAllowed
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pbIsPluginsAllowed=%.8X",
		__FUNCTION__,
		hDispConfig,
		pbIsPluginsAllowed
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pbIsPluginsAllowed))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->ArePluginsEnabled(pbIsPluginsAllowed));
}

PRL_METHOD( PrlDispCfg_EnablePlugins ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL bEnablePluginsSupport
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, bEnablePluginsSupport=%d)",
		__FUNCTION__,
		hDispConfig,
		bEnablePluginsSupport
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->EnablePlugins(bEnablePluginsSupport));
}

PRL_METHOD( PrlDispCfg_GetDefaultEncryptionPluginId ) (
	PRL_HANDLE hDispConfig,
	PRL_STR sDefaultPluginId,
	PRL_UINT32_PTR pnDefaultPluginIdBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sDefaultPluginId=%.8X, pnDefaultPluginIdBufLength=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sDefaultPluginId,
		pnDefaultPluginIdBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pnDefaultPluginIdBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->GetDefaultEncryptionPluginId(sDefaultPluginId, pnDefaultPluginIdBufLength));
}

PRL_METHOD( PrlDispCfg_SetDefaultEncryptionPluginId ) (
	PRL_HANDLE hDispConfig,
	PRL_CONST_STR sDefaultPluginId
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, sDefaultPluginId=%.8X)",
		__FUNCTION__,
		hDispConfig,
		sDefaultPluginId
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->SetDefaultEncryptionPluginId(sDefaultPluginId));
}

PRL_METHOD( PrlDispCfg_IsVerboseLogEnabled ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL_PTR pbEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pbEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->IsVerboseLogEnabled(pbEnabled));
}

PRL_METHOD( PrlDispCfg_SetVerboseLogEnabled ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL bEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hDispConfig,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return ( pDispConfig->SetVerboseLogEnabled( bEnabled ) );
}

PRL_METHOD( PrlDispCfg_IsAllowMultiplePMC ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL_PTR pbEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pbEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->IsAllowMultiplePMC(pbEnabled));
}

PRL_METHOD( PrlDispCfg_SetAllowMultiplePMC ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL bEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hDispConfig,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return ( pDispConfig->SetAllowMultiplePMC( bEnabled ) );
}

PRL_METHOD( PrlDispCfg_IsLogRotationEnabled ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL_PTR pbEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pbEnabled=%.8X)",
		__FUNCTION__,
		hDispConfig,
		pbEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) ||
		PRL_WRONG_PTR(pbEnabled))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return (pDispConfig->IsLogRotationEnabled(pbEnabled));
}

PRL_METHOD( PrlDispCfg_SetLogRotationEnabled ) (
	PRL_HANDLE hDispConfig,
	PRL_BOOL bEnabled
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, bEnabled=%.8X)",
		__FUNCTION__,
		hDispConfig,
		bEnabled
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return ( pDispConfig->SetLogRotationEnabled( bEnabled ) );
}

PRL_METHOD( PrlDispCfg_GetUsbIdentityCount ) (
		PRL_HANDLE hDispCfg,
		PRL_UINT32_PTR pnUsbIdentityCount
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hDispCfg=%.8X, pnUsbIdentityCount=%.8X)",
		__FUNCTION__,
		hDispCfg,
		pnUsbIdentityCount
	);

	if (PRL_WRONG_HANDLE(hDispCfg, PHT_DISP_CONFIG))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispCfg );

	return pDispConfig->GetUsbIdentityCount(pnUsbIdentityCount);
}

PRL_METHOD( PrlDispCfg_GetUsbIdentity ) (
		PRL_HANDLE hDispCfg,
		PRL_UINT32 nUsbIdentIndex,
		PRL_HANDLE_PTR phUsbIdent
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hDispCfg=%.8X, nUsbIdentIndex=%.8X, phUsbIdent=%.8X)",
		__FUNCTION__,
		hDispCfg,
		nUsbIdentIndex,
		phUsbIdent
	);

	if (PRL_WRONG_HANDLE(hDispCfg, PHT_DISP_CONFIG)
			|| PRL_WRONG_PTR( phUsbIdent ) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispCfg );

	return pDispConfig->GetUsbIdentity(nUsbIdentIndex, phUsbIdent);
}

PRL_METHOD( PrlDispCfg_SetUsbIdentAssociation ) (
		PRL_HANDLE hDispCfg,
		PRL_CONST_STR sSystemName,
		PRL_CONST_STR sVmUuid,
		PRL_UINT32 nFlags
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hDispCfg=%.8X, sSystemName=%.8X, sVmUuid=%.8X)",
		__FUNCTION__,
		hDispCfg,
		sSystemName,
		sVmUuid
	);

	if (PRL_WRONG_HANDLE(hDispCfg, PHT_DISP_CONFIG))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispCfg );

	return pDispConfig->SetUsbIdentAssociation(sSystemName, sVmUuid, nFlags);
}

PRL_METHOD( PrlDispCfg_GetCpuFeaturesMask ) (PRL_HANDLE hDispConfig, PRL_CPU_FEATURES_MASK_PTR pMasks)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, pMasks=%.8X)", __FUNCTION__, hDispConfig, pMasks);

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) || PRL_WRONG_PTR(pMasks))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return pDispConfig->GetCpuFeaturesMask(pMasks);
}

PRL_METHOD( PrlDispCfg_SetCpuFeaturesMask ) (PRL_HANDLE hDispConfig, PRL_CONST_CPU_FEATURES_MASK_PTR pMasks)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, nMasks=%.8X)", __FUNCTION__, hDispConfig, pMasks);

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) || PRL_WRONG_PTR(pMasks))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return pDispConfig->SetCpuFeaturesMask(pMasks);
}

PRL_METHOD( PrlDispCfg_GetCpuFeaturesMaskEx ) (
		PRL_HANDLE hDispConfig,
		PRL_HANDLE_PTR phCpuFeatures
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, phCpuFeatures=%.8X)",
		__FUNCTION__,
		hDispConfig,
		phCpuFeatures
	);

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) || PRL_WRONG_PTR(phCpuFeatures))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return pDispConfig->GetCpuFeaturesMaskEx(phCpuFeatures);
}

PRL_METHOD( PrlDispCfg_SetCpuFeaturesMaskEx ) (
		PRL_HANDLE hDispConfig,
		PRL_HANDLE hCpuFeatures
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hDispConfig=%.8X, hCpuFeatures=%.8X)",
		__FUNCTION__,
		hDispConfig,
		hCpuFeatures
	);

	if (PRL_WRONG_HANDLE(hDispConfig, PHT_DISP_CONFIG) || PRL_WRONG_HANDLE(hCpuFeatures, PHT_CPU_FEATURES))
		return PRL_ERR_INVALID_ARG;

	PrlHandleDispConfigPtr pDispConfig = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>( hDispConfig );

	return pDispConfig->SetCpuFeaturesMaskEx(hCpuFeatures);
}

PRL_METHOD( PrlCpuFeatures_Create ) (
		PRL_HANDLE_PTR phCpuFeatures
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (phCpuFeatures=%.8X)",
		__FUNCTION__,
		phCpuFeatures
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_PTR(phCpuFeatures))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleCpuFeatures *pCpuFeatures = new PrlHandleCpuFeatures;
	if (!pCpuFeatures)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phCpuFeatures = pCpuFeatures->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlCpuFeatures_GetValue ) (
		PRL_HANDLE hCpuFeatures,
		PRL_CPU_FEATURES_EX nId,
		PRL_UINT32_PTR pnValue
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hCpuFeatures=%.8X, nId=%.8X, pnValue=%.8X)",
		__FUNCTION__,
		hCpuFeatures,
		nId,
		pnValue
	);

	if (PRL_WRONG_HANDLE(hCpuFeatures, PHT_CPU_FEATURES) || PRL_WRONG_PTR(pnValue))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCpuFeaturesPtr pCpuFeatures = PRL_OBJECT_BY_HANDLE<PrlHandleCpuFeatures>( hCpuFeatures );

	return pCpuFeatures->GetValue(nId, pnValue);
}

PRL_METHOD( PrlCpuFeatures_SetValue ) (
		PRL_HANDLE hCpuFeatures,
		PRL_CPU_FEATURES_EX nId,
		PRL_UINT32 nValue
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hCpuFeatures=%.8X, nId=%.8X, nValue=%.8X)",
		__FUNCTION__,
		hCpuFeatures,
		nId,
		nValue
	);

	if (PRL_WRONG_HANDLE(hCpuFeatures, PHT_CPU_FEATURES))
		return PRL_ERR_INVALID_ARG;

	PrlHandleCpuFeaturesPtr pCpuFeatures = PRL_OBJECT_BY_HANDLE<PrlHandleCpuFeatures>( hCpuFeatures );

	return pCpuFeatures->SetValue(nId, nValue);
}

PRL_METHOD( PrlLic_IsValid) (
		PRL_HANDLE hLicense,
		PRL_BOOL_PTR pbValue
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLicense=%.8X, pbValue=%.8X)",
		__FUNCTION__,
		hLicense,
		pbValue
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLicense, PHT_LICENSE) || PRL_WRONG_PTR(pbValue) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLicensePtr pLicense = PRL_OBJECT_BY_HANDLE<PrlHandleLicense>( hLicense );
	return (pLicense->IsValid(pbValue));
}

PRL_METHOD( PrlLic_GetStatus) (
		PRL_HANDLE hLicense,
		PRL_RESULT_PTR pnValue
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLicense=%.8X, pnValue=%.8X)",
		__FUNCTION__,
		hLicense,
		pnValue
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLicense, PHT_LICENSE) || PRL_WRONG_PTR(pnValue) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLicensePtr pLicense = PRL_OBJECT_BY_HANDLE<PrlHandleLicense>( hLicense );
	return (pLicense->GetStatus(pnValue));
}

PRL_METHOD( PrlLic_GetLicenseKey) (
		PRL_HANDLE hLicense,
		PRL_STR sLicenseKey,
		PRL_UINT32_PTR pnLicenseKeyBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLicense=%.8X, sLicenseKey=%.8X, pnLicenseKeyBufLength=%.8X)",
		__FUNCTION__,
		hLicense,
		sLicenseKey,
		pnLicenseKeyBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLicense, PHT_LICENSE) || PRL_WRONG_PTR(pnLicenseKeyBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLicensePtr pLicense = PRL_OBJECT_BY_HANDLE<PrlHandleLicense>( hLicense );
	return (pLicense->GetLicenseKey(sLicenseKey, pnLicenseKeyBufLength));
}

PRL_METHOD( PrlLic_GetUserName) (
		PRL_HANDLE hLicense,
		PRL_STR sUserName,
		PRL_UINT32_PTR pnUserNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLicense=%.8X, sUserName=%.8X, pnUserNameBufLength=%.8X)",
		__FUNCTION__,
		hLicense,
		sUserName,
		pnUserNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLicense, PHT_LICENSE) || PRL_WRONG_PTR(pnUserNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLicensePtr pLicense = PRL_OBJECT_BY_HANDLE<PrlHandleLicense>( hLicense );
#ifdef _WIN_
#ifdef GetUserName
#undef GetUserName
#endif
#endif
	return (pLicense->GetUserName(sUserName, pnUserNameBufLength));
}

PRL_METHOD( PrlLic_GetCompanyName) (
		PRL_HANDLE hLicense,
		PRL_STR sCompanyName,
		PRL_UINT32_PTR pnCompanyNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLicense=%.8X, sCompanyName=%.8X, pnCompanyNameBufLength=%.8X)",
		__FUNCTION__,
		hLicense,
		sCompanyName,
		pnCompanyNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLicense, PHT_LICENSE) || PRL_WRONG_PTR(pnCompanyNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLicensePtr pLicense = PRL_OBJECT_BY_HANDLE<PrlHandleLicense>( hLicense );
	return (pLicense->GetCompanyName(sCompanyName, pnCompanyNameBufLength));
}

PRL_METHOD( PrlSrvInfo_GetHostName) (
		PRL_HANDLE hServerInfo,
		PRL_STR sHostName,
		PRL_UINT32_PTR pnHostNameBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServerInfo=%.8X, sHostName=%.8X, pnHostNameBufLength=%.8X)",
		__FUNCTION__,
		hServerInfo,
		sHostName,
		pnHostNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServerInfo, PHT_SERVER_INFO) || PRL_WRONG_PTR(pnHostNameBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleServerInfoPtr pServerInfo = PRL_OBJECT_BY_HANDLE<PrlHandleServerInfo>( hServerInfo );
	return (pServerInfo->GetHostName(sHostName, pnHostNameBufLength));
}

PRL_METHOD( PrlSrvInfo_GetOsVersion) (
		PRL_HANDLE hServerInfo,
		PRL_STR sOsVersion,
		PRL_UINT32_PTR pnOsVersionBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServerInfo=%.8X, sOsVersion=%.8X, pnOsVersionBufLength=%.8X)",
		__FUNCTION__,
		hServerInfo,
		sOsVersion,
		pnOsVersionBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServerInfo, PHT_SERVER_INFO) || PRL_WRONG_PTR(pnOsVersionBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleServerInfoPtr pServerInfo = PRL_OBJECT_BY_HANDLE<PrlHandleServerInfo>( hServerInfo );
	return (pServerInfo->GetOsVersion(sOsVersion, pnOsVersionBufLength));
}

PRL_METHOD( PrlSrvInfo_GetCmdPort) (
		PRL_HANDLE hServerInfo,
		PRL_UINT32_PTR pnPort
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServerInfo=%.8X, pnPort=%.8X)",
		__FUNCTION__,
		hServerInfo,
		pnPort
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServerInfo, PHT_SERVER_INFO) || PRL_WRONG_PTR(pnPort) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleServerInfoPtr pServerInfo = PRL_OBJECT_BY_HANDLE<PrlHandleServerInfo>( hServerInfo );
	return (pServerInfo->GetCmdPort(pnPort));
}

PRL_METHOD( PrlSrvInfo_GetServerUuid ) (
		PRL_HANDLE hServerInfo,
		PRL_STR sServerUuid,
		PRL_UINT32_PTR pnServerUuidBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServerInfo=%.8X, sServerUuid=%.8X, pnServerUuidBufLength=%.8X)",
		__FUNCTION__,
		hServerInfo,
		sServerUuid,
		pnServerUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServerInfo, PHT_SERVER_INFO) || PRL_WRONG_PTR(pnServerUuidBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleServerInfoPtr pServerInfo = PRL_OBJECT_BY_HANDLE<PrlHandleServerInfo>( hServerInfo );
	return (pServerInfo->GetServerUuid(sServerUuid, pnServerUuidBufLength));
}

PRL_METHOD( PrlSrvInfo_GetProductVersion) (
		PRL_HANDLE hServerInfo,
		PRL_STR sProductVersion,
		PRL_UINT32_PTR pnProductVersionBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServerInfo=%.8X, sProductVersion=%.8X, pnProductVersionBufLength=%.8X)",
		__FUNCTION__,
		hServerInfo,
		sProductVersion,
		pnProductVersionBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServerInfo, PHT_SERVER_INFO) || PRL_WRONG_PTR(pnProductVersionBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleServerInfoPtr pServerInfo = PRL_OBJECT_BY_HANDLE<PrlHandleServerInfo>( hServerInfo );
	return (pServerInfo->GetProductVersion(sProductVersion, pnProductVersionBufLength));
}

PRL_METHOD( PrlSrvInfo_GetApplicationMode) (
		PRL_HANDLE hServerInfo,
		PRL_APPLICATION_MODE_PTR pnApplicationMode
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServerInfo=%.8X, pnApplicationMode=%.8X)",
		__FUNCTION__,
		hServerInfo,
		pnApplicationMode
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServerInfo, PHT_SERVER_INFO) || PRL_WRONG_PTR(pnApplicationMode) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleServerInfoPtr pServerInfo = PRL_OBJECT_BY_HANDLE<PrlHandleServerInfo>( hServerInfo );
	return (pServerInfo->GetApplicationMode(pnApplicationMode));
}

PRL_METHOD( PrlSrvInfo_GetStartTime) (
		PRL_HANDLE hServerInfo,
		PRL_UINT64_PTR pnStartTime
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServerInfo=%.8X, pnStartTime=%.8X)",
		__FUNCTION__,
		hServerInfo,
		pnStartTime
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServerInfo, PHT_SERVER_INFO) || PRL_WRONG_PTR(pnStartTime) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleServerInfoPtr pServerInfo = PRL_OBJECT_BY_HANDLE<PrlHandleServerInfo>( hServerInfo );
	return (pServerInfo->GetStartTime(pnStartTime));
}

PRL_METHOD( PrlSrvInfo_GetStartTimeMonotonic) (
		PRL_HANDLE hServerInfo,
		PRL_UINT64_PTR pnStartTimeMonotonic
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hServerInfo=%.8X, pnStartTimeMonotonic=%.8X)",
		__FUNCTION__,
		hServerInfo,
		pnStartTimeMonotonic
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hServerInfo, PHT_SERVER_INFO) || PRL_WRONG_PTR(pnStartTimeMonotonic) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleServerInfoPtr pServerInfo = PRL_OBJECT_BY_HANDLE<PrlHandleServerInfo>( hServerInfo );
	return (pServerInfo->GetStartTimeMonotonic(pnStartTimeMonotonic));
}

PRL_METHOD ( PrlLoginResponse_GetSessionUuid ) (
	PRL_HANDLE hLoginResp,
	PRL_STR sSessionUuid,
	PRL_UINT32_PTR pnSessionUuidBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLoginResp=%.8X, sSessionUuid=%.8X, pnSessionUuidBufLength=%.8X)",
		__FUNCTION__,
		hLoginResp,
		sSessionUuid,
		pnSessionUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLoginResp, PHT_LOGIN_RESPONSE) || PRL_WRONG_PTR(pnSessionUuidBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLoginResponsePtr pLoginResp = PRL_OBJECT_BY_HANDLE<PrlHandleLoginResponse>( hLoginResp );
	return (pLoginResp->GetSessionUuid(sSessionUuid, pnSessionUuidBufLength));
}

PRL_METHOD ( PrlLoginResponse_GetServerUuid ) (
	PRL_HANDLE hLoginResp,
	PRL_STR sServerUuid,
	PRL_UINT32_PTR pnServerUuidBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLoginResp=%.8X, sServerUuid=%.8X, pnServerUuidBufLength=%.8X)",
		__FUNCTION__,
		hLoginResp,
		sServerUuid,
		pnServerUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLoginResp, PHT_LOGIN_RESPONSE) || PRL_WRONG_PTR(pnServerUuidBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLoginResponsePtr pLoginResp = PRL_OBJECT_BY_HANDLE<PrlHandleLoginResponse>( hLoginResp );
	return (pLoginResp->GetServerUuid(sServerUuid, pnServerUuidBufLength));
}

PRL_METHOD ( PrlLoginResponse_GetHostOsVersion ) (
	PRL_HANDLE hLoginResp,
	PRL_STR sHostOsVersion,
	PRL_UINT32_PTR pnHostOsVersionBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLoginResp=%.8X, sHostOsVersion=%.8X, pnHostOsVersionBufLength=%.8X)",
		__FUNCTION__,
		hLoginResp,
		sHostOsVersion,
		pnHostOsVersionBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLoginResp, PHT_LOGIN_RESPONSE) || PRL_WRONG_PTR(pnHostOsVersionBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLoginResponsePtr pLoginResp = PRL_OBJECT_BY_HANDLE<PrlHandleLoginResponse>( hLoginResp );
	return (pLoginResp->GetHostOsVersion(sHostOsVersion, pnHostOsVersionBufLength));
}

PRL_METHOD ( PrlLoginResponse_GetProductVersion ) (
	PRL_HANDLE hLoginResp,
	PRL_STR sProductVersion,
	PRL_UINT32_PTR pnProductVersionBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLoginResp=%.8X, sProductVersion=%.8X, pnProductVersionBufLength=%.8X)",
		__FUNCTION__,
		hLoginResp,
		sProductVersion,
		pnProductVersionBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLoginResp, PHT_LOGIN_RESPONSE) || PRL_WRONG_PTR(pnProductVersionBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLoginResponsePtr pLoginResp = PRL_OBJECT_BY_HANDLE<PrlHandleLoginResponse>( hLoginResp );
	return (pLoginResp->GetProductVersion(sProductVersion, pnProductVersionBufLength));
}

PRL_METHOD ( PrlLoginResponse_GetRunningTaskCount ) (
	PRL_HANDLE hLoginResp,
	PRL_UINT32_PTR pnCount
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLoginResp=%.8X, pnCount=%.8X)",
		__FUNCTION__,
		hLoginResp,
		pnCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLoginResp, PHT_LOGIN_RESPONSE) || PRL_WRONG_PTR(pnCount) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLoginResponsePtr pLoginResp = PRL_OBJECT_BY_HANDLE<PrlHandleLoginResponse>( hLoginResp );
	return (pLoginResp->GetRunningTaskCount( pnCount ));
}

PRL_METHOD ( PrlLoginResponse_GetRunningTaskByIndex ) (
	PRL_HANDLE hLoginResp,
	PRL_UINT32 nIndex,
	PRL_HANDLE_PTR phRunningTask
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hLoginResp=%.8X, nIndex=%.8d, phRunningTask=%.8X)",
		__FUNCTION__,
		hLoginResp,
		nIndex,
		phRunningTask
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hLoginResp, PHT_LOGIN_RESPONSE) || PRL_WRONG_PTR(phRunningTask) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleLoginResponsePtr pLoginResp = PRL_OBJECT_BY_HANDLE<PrlHandleLoginResponse>( hLoginResp );
	return ( pLoginResp->GetRunningTaskByIndex( nIndex, phRunningTask ) );
}

PRL_METHOD ( PrlRunningTask_GetTaskUuid ) (
	PRL_HANDLE hRunningTask,
	PRL_STR sTaskUuid,
	PRL_UINT32_PTR pnTaskUuidBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hRunningTask=%.8X, sTaskUuid=%.8X, pnTaskUuidBufLength=%.8X)",
		__FUNCTION__,
		hRunningTask,
		sTaskUuid,
		pnTaskUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hRunningTask, PHT_RUNNING_TASK) || PRL_WRONG_PTR(pnTaskUuidBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleRunningTaskPtr pTask = PRL_OBJECT_BY_HANDLE<PrlHandleRunningTask>( hRunningTask );
	return ( pTask->GetTaskUuid( sTaskUuid, pnTaskUuidBufLength ) );
}

PRL_METHOD ( PrlRunningTask_GetTaskType ) (
	PRL_HANDLE hRunningTask,
	PRL_RUNNING_TASK_TYPE_ENUM_PTR pnType
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hRunningTask=%.8X, pnType=%.8X",
		__FUNCTION__,
		hRunningTask,
		pnType
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hRunningTask, PHT_RUNNING_TASK) || PRL_WRONG_PTR(pnType) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleRunningTaskPtr pTask = PRL_OBJECT_BY_HANDLE<PrlHandleRunningTask>( hRunningTask );
	return ( pTask->GetTaskType( pnType ) );
}

PRL_METHOD ( PrlRunningTask_GetTaskParametersAsString ) (
	PRL_HANDLE hRunningTask,
	PRL_STR sParamsStr,
	PRL_UINT32_PTR pnsParamsStrBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hRunningTask=%.8X, sParamsStr=%.8X, pnsParamsStrBufLength=%.8X)",
		__FUNCTION__,
		hRunningTask,
		sParamsStr,
		pnsParamsStrBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hRunningTask, PHT_RUNNING_TASK) || PRL_WRONG_PTR(pnsParamsStrBufLength) )
		return (PRL_ERR_INVALID_ARG);
	PrlHandleRunningTaskPtr pTask = PRL_OBJECT_BY_HANDLE<PrlHandleRunningTask>( hRunningTask );
	return ( pTask->GetTaskParametersAsString( sParamsStr, pnsParamsStrBufLength ) );
}

PRL_METHOD( PrlUsbIdent_GetSystemName ) (
		PRL_HANDLE hUsbIdent,
		PRL_STR sSystemName,
		PRL_UINT32_PTR pnSystemNameBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hUsbIdent=%.8X, sSystemName=%.8X, pnSystemNameBufLength=%.8X)",
		__FUNCTION__,
		hUsbIdent,
		sSystemName,
		pnSystemNameBufLength
	);

	if (PRL_WRONG_HANDLE(hUsbIdent, PHT_USB_IDENTITY))
		return PRL_ERR_INVALID_ARG;

	PrlHandleUsbIdentityPtr pUsbIdentity = PRL_OBJECT_BY_HANDLE<PrlHandleUsbIdentity>( hUsbIdent );

	return pUsbIdentity->GetSystemName(sSystemName, pnSystemNameBufLength);
}

PRL_METHOD( PrlUsbIdent_GetFriendlyName ) (
		PRL_HANDLE hUsbIdent,
		PRL_STR sFriendlyName,
		PRL_UINT32_PTR pnFriendlyNameBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hUsbIdent=%.8X, sFriendlyName=%.8X, pnFriendlyNameBufLength=%.8X)",
		__FUNCTION__,
		hUsbIdent,
		sFriendlyName,
		pnFriendlyNameBufLength
	);

	if (PRL_WRONG_HANDLE(hUsbIdent, PHT_USB_IDENTITY))
		return PRL_ERR_INVALID_ARG;

	PrlHandleUsbIdentityPtr pUsbIdentity = PRL_OBJECT_BY_HANDLE<PrlHandleUsbIdentity>( hUsbIdent );

	return pUsbIdentity->GetFriendlyName(sFriendlyName, pnFriendlyNameBufLength);
}

PRL_METHOD( PrlUsbIdent_GetVmUuidAssociation ) (
		PRL_HANDLE hUsbIdent,
		PRL_STR sVmUuid,
		PRL_UINT32_PTR pnVmUuidBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hUsbIdent=%.8X, sVmUuid=%.8X, pnVmUuidBufLength=%.8X)",
		__FUNCTION__,
		hUsbIdent,
		sVmUuid,
		pnVmUuidBufLength
	);

	if (PRL_WRONG_HANDLE(hUsbIdent, PHT_USB_IDENTITY))
		return PRL_ERR_INVALID_ARG;

	PrlHandleUsbIdentityPtr pUsbIdentity = PRL_OBJECT_BY_HANDLE<PrlHandleUsbIdentity>( hUsbIdent );

	return pUsbIdentity->GetVmUuidAssociation(sVmUuid, pnVmUuidBufLength);
}

PRL_METHOD( PrlPluginInfo_GetVendor ) (
		PRL_HANDLE hPluginInfo,
		PRL_STR sVendor,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPluginInfo=%.8X, sVendor=%.8X, pnBufLength=%.8X)",
		__FUNCTION__,
		hPluginInfo,
		sVendor,
		pnBufLength
	);

	if (PRL_WRONG_HANDLE(hPluginInfo, PHT_PLUGIN_INFO)
		|| PRL_WRONG_PTR(pnBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePluginInfoPtr pPluginInfo = PRL_OBJECT_BY_HANDLE<PrlHandlePluginInfo>( hPluginInfo );

	return pPluginInfo->GetVendor(sVendor, pnBufLength);
}

PRL_METHOD( PrlPluginInfo_GetCopyright ) (
		PRL_HANDLE hPluginInfo,
		PRL_STR sCopyright,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPluginInfo=%.8X, sCopyright=%.8X, pnBufLength=%.8X)",
		__FUNCTION__,
		hPluginInfo,
		sCopyright,
		pnBufLength
	);

	if (PRL_WRONG_HANDLE(hPluginInfo, PHT_PLUGIN_INFO)
		|| PRL_WRONG_PTR(pnBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePluginInfoPtr pPluginInfo = PRL_OBJECT_BY_HANDLE<PrlHandlePluginInfo>( hPluginInfo );

	return pPluginInfo->GetCopyright(sCopyright, pnBufLength);
}

PRL_METHOD( PrlPluginInfo_GetShortDescription ) (
		PRL_HANDLE hPluginInfo,
		PRL_STR sDescr,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPluginInfo=%.8X, sDescr=%.8X, pnBufLength=%.8X)",
		__FUNCTION__,
		hPluginInfo,
		sDescr,
		pnBufLength
	);

	if (PRL_WRONG_HANDLE(hPluginInfo, PHT_PLUGIN_INFO)
		|| PRL_WRONG_PTR(pnBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePluginInfoPtr pPluginInfo = PRL_OBJECT_BY_HANDLE<PrlHandlePluginInfo>( hPluginInfo );

	return pPluginInfo->GetShortDescription(sDescr, pnBufLength);
}

PRL_METHOD( PrlPluginInfo_GetLongDescription ) (
		PRL_HANDLE hPluginInfo,
		PRL_STR sDescr,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPluginInfo=%.8X, sDescr=%.8X, pnBufLength=%.8X)",
		__FUNCTION__,
		hPluginInfo,
		sDescr,
		pnBufLength
	);

	if (PRL_WRONG_HANDLE(hPluginInfo, PHT_PLUGIN_INFO)
		|| PRL_WRONG_PTR(pnBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePluginInfoPtr pPluginInfo = PRL_OBJECT_BY_HANDLE<PrlHandlePluginInfo>( hPluginInfo );

	return pPluginInfo->GetLongDescription(sDescr, pnBufLength);
}

PRL_METHOD( PrlPluginInfo_GetVersion ) (
		PRL_HANDLE hPluginInfo,
		PRL_STR sVersion,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPluginInfo=%.8X, sVersion=%.8X, pnBufLength=%.8X)",
		__FUNCTION__,
		hPluginInfo,
		sVersion,
		pnBufLength
	);

	if (PRL_WRONG_HANDLE(hPluginInfo, PHT_PLUGIN_INFO)
		|| PRL_WRONG_PTR(pnBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePluginInfoPtr pPluginInfo = PRL_OBJECT_BY_HANDLE<PrlHandlePluginInfo>( hPluginInfo );

	return pPluginInfo->GetVersion(sVersion, pnBufLength);
}

PRL_METHOD( PrlPluginInfo_GetId ) (
		PRL_HANDLE hPluginInfo,
		PRL_STR sId,
		PRL_UINT32_PTR pnBufLength
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hPluginInfo=%.8X, sId=%.8X, pnBufLength=%.8X)",
		__FUNCTION__,
		hPluginInfo,
		sId,
		pnBufLength
	);

	if (PRL_WRONG_HANDLE(hPluginInfo, PHT_PLUGIN_INFO)
		|| PRL_WRONG_PTR(pnBufLength))
		return PRL_ERR_INVALID_ARG;

	PrlHandlePluginInfoPtr pPluginInfo = PRL_OBJECT_BY_HANDLE<PrlHandlePluginInfo>( hPluginInfo );

	return pPluginInfo->GetId(sId, pnBufLength);
}

PRL_METHOD( PrlBackupResult_GetBackupUuid ) (
		PRL_HANDLE hBackupResult,
		PRL_STR sBackupUuid,
		PRL_UINT32_PTR pnBackupUuidBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hBackupResult=%.8X, sBackupUuid=%.8X, pnBackupUuidBufLength=%.8X)",
		__FUNCTION__,
		hBackupResult,
		sBackupUuid,
		pnBackupUuidBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hBackupResult, PHT_BACKUP_RESULT) ||
			PRL_WRONG_PTR(pnBackupUuidBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleBackupResultPtr pBackupResult =
			PRL_OBJECT_BY_HANDLE<PrlHandleBackupResult>( hBackupResult );

	return (pBackupResult->GetBackupUuid(sBackupUuid, pnBackupUuidBufLength));
}

PRL_METHOD( PrlCPUPool_GetName) (
	PRL_HANDLE hCpuPool,
	PRL_STR sBuf,
	PRL_UINT32_PTR pnBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hCpuPool=%.8X, sBuf=%.8X, pnBufLength=%.8X)",
		__FUNCTION__,
		hCpuPool,
		sBuf,
		pnBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hCpuPool, PHT_CPU_POOL) ||
		PRL_WRONG_PTR(pnBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleCpuPoolPtr pCpuPool = PRL_OBJECT_BY_HANDLE<PrlHandleCpuPool>( hCpuPool );

	return (pCpuPool->GetName(sBuf, pnBufLength));
}

PRL_METHOD( PrlCPUPool_GetVendor) (
	PRL_HANDLE hCpuPool,
	PRL_STR sBuf,
	PRL_UINT32_PTR pnBufLength
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hCpuPool=%.8X, sBuf=%.8X, pnBufLength=%.8X)",
		__FUNCTION__,
		hCpuPool,
		sBuf,
		pnBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hCpuPool, PHT_CPU_POOL) ||
		PRL_WRONG_PTR(pnBufLength))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleCpuPoolPtr pCpuPool = PRL_OBJECT_BY_HANDLE<PrlHandleCpuPool>( hCpuPool );

	return (pCpuPool->GetVendor(sBuf, pnBufLength));
}

PRL_METHOD( PrlCPUPool_GetCpuFeaturesMask ) (
	PRL_HANDLE hCpuPool,
	PRL_HANDLE_PTR phCpuFeatures
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hCpuPool=%.8X, phCpuFeatures=%.8X)",
		__FUNCTION__,
		hCpuPool,
		phCpuFeatures
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hCpuPool, PHT_CPU_POOL) ||
		PRL_WRONG_PTR(phCpuFeatures))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleCpuPoolPtr pCpuPool = PRL_OBJECT_BY_HANDLE<PrlHandleCpuPool>( hCpuPool );

	return (pCpuPool->GetCpuFeaturesMask(phCpuFeatures));
}

PRL_HANDLE PrlSrv_GetCPUPoolsList_Impl(PRL_HANDLE hServer)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_CPU_POOLS_LIST_POOLS))
	PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>( hServer );
	PrlHandleJobPtr pJob = pServer->GetCPUPoolsList();
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_GetCPUPoolsList ) (
	PRL_HANDLE hServer
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_CPU_POOLS_LIST_POOLS);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_GetCPUPoolsList, (hServer))
}

PRL_HANDLE PrlSrv_MoveToCPUPool_Impl(PRL_HANDLE hServer, PRL_HANDLE hCpuPool)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hCpuPool, PHT_CPU_POOL) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_CPU_POOLS_MOVE))
	PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>( hServer );
	PrlHandleJobPtr pJob = pServer->MoveToCPUPool(hCpuPool);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_MoveToCPUPool ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hCpuPool
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_CPU_POOLS_MOVE);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_MoveToCPUPool , (hServer, hCpuPool))
}

PRL_HANDLE PrlSrv_RecalculateCPUPool_Impl(PRL_HANDLE hServer, PRL_HANDLE hCpuPool)
{
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_HANDLE(hCpuPool, PHT_CPU_POOL) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_SRV_CPU_POOLS_RECALCULATE))
	PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>( hServer );
	PrlHandleJobPtr pJob = pServer->RecalculateCPUPool(hCpuPool);
	if (!pJob)
		RETURN_RES(PRL_INVALID_HANDLE)
	RETURN_RES(pJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlSrv_RecalculateCPUPool ) (
	PRL_HANDLE hServer,
	PRL_HANDLE hCpuPool
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_SRV_CPU_POOLS_RECALCULATE);
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlSrv_RecalculateCPUPool, (hServer, hCpuPool))
}
