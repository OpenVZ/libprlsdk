/*
 * PrlApiCore.cpp
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
#include <prlcommon/PrlCommonUtilsBase/ParallelsDirs.h>
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/Logging/Logging.h>

#include "PrlCommon.h"
#include "PrlHandleJob.h"
#include "PrlHandleResult.h"
#include "PrlHandleEvent.h"
#include "PrlHandleEventParam.h"
#include "PrlHandleStringsList.h"
#include "PrlHandleHandlesList.h"
#include "PrlHandleOpTypeList.h"
#include "PrlHandleProblemReport.h"
#include "PrlProblemReportSender.h"
#include "PrlHandleServerDisp.h"
#include "PrlHandleVmCfg.h"

#include "PrlErrStringsStorage.h"
#include "HandleInterface/GetHandleInterface.h"

#include "Build/Current.ver"

#include "PrlContextSwitcher.h"
#include "PrlNew.h"

////////////////////////////////////////////////////////////////////////////////////////////////


PRL_UINT32 g_SdkSequenceNum = 0;
static PRL_UINT32 s_SdkVersion = PARALLELS_API_VER;
static PRL_APPLICATION_MODE s_SdkAppMode = PAM_SERVER;
static QMutex apiRefCountLock;
static long apiRefCount = 0;
void (*g_fpInitRemoteDevices)() = 0;


PRL_UINT32 PrlGetApiVersion()
{
	return (s_SdkVersion);
}

PRL_APPLICATION_MODE PrlGetApiAppMode()
{
	return (s_SdkAppMode);
}

PRL_CONST_STR PrlGetApiProductName()
{
	// NOTE : TAG_TEXT_XXX is not translated string,
	//        to translate need to use QObject::tr()

    if (PrlGetApiAppMode() == PAM_SERVER)
        return TAG_TEXT_PRODUCT_NAME_SERVER;
    else
        return "unsupported";
}

QString PrlGetHostComputerName()
{
	switch(PrlGetApiAppMode())
	{
	case PAM_SERVER:
		return (QObject::tr("the physical server"));
	default:
		return (QObject::tr("the host computer"));
	}
}

QString PrlGetTrashName()
{
#ifdef _WIN_
	return (QObject::tr("the Recycle Bin"));
#else
	return (QObject::tr("Trash"));
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////

// Forward all Qt messages to Logging system
INSTALL_LOGGING_QT_MSG_HANDLER(PrlApiHandler)

////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * API initialization helper
 * @param version of API
 * @param application mode in which SDK should work
 * @return result of API initialization
 */
static PRL_RESULT InitApi(PRL_UINT32 version, PRL_APPLICATION_MODE nAppMode, PRL_UINT32 nFlags)
{
    QMutexLocker _lock(&apiRefCountLock);

    apiRefCount++;
    if (apiRefCount > 1)
        return PRL_ERR_SUCCESS;

	// API version check
	if ( (version & ~0xE0000000) > PARALLELS_API_VER ) {
		return PRL_ERR_API_INCOMPATIBLE;
	}

	// Store SDK application mode
	s_SdkAppMode = nAppMode;

	// Temporally disable console output, only log file
	int nConsoleLogging = SetConsoleLogging(0);
	ParallelsDirs::InitOptions	nSubMode = (nFlags & PAIF_INIT_AS_APPSTORE_CLIENT)
			? ParallelsDirs::smAppStoreMode
			: ParallelsDirs::smNormalMode;

	ParallelsDirs::Init(nAppMode, nSubMode, true );
	SetConsoleLogging(nConsoleLogging);

	// We're not allowed to create multiple instances of the application
	if ( QMainThread::IsInitialized() )
		return PRL_ERR_DOUBLE_INIT;

	// Creating SDK central object
	if ( !QMainThread::Instance() )
		return PRL_ERR_OUT_OF_MEMORY;

	// Actually this is a hack which is used only for SDK manager
	// to store SDK sequence number.
	g_SdkSequenceNum = version >> 29;

	// Save version with possible sequence number
	s_SdkVersion = version;

	if (g_fpInitRemoteDevices)
		(*g_fpInitRemoteDevices)();

	// Starting main application thread
	QMainThread::Instance()->start(nFlags);

	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlApi_Init ) (
						   PRL_UINT32 version
						   )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (version=%u)",
		__FUNCTION__,
		version
		);

	return (InitApi(version, PAM_SERVER, 0));
}

PRL_METHOD( PrlApi_InitEx ) (
		PRL_UINT32 nVersion,
    PRL_APPLICATION_MODE nAppMode,
		PRL_UINT32 nFlags,
    PRL_UINT32 nReserved
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (nVersion=%u, nAppMode=%u, nFlags=%u, nReserved2=%u)",
		__FUNCTION__,
		nVersion,
    nAppMode,
    nFlags,
    nReserved
		);

  PRL_UNUSED_PARAM(nReserved);
  return (InitApi(nVersion, nAppMode, nFlags));
}

PRL_METHOD( PrlApi_Deinit ) ()
{
    QMutexLocker _lock(&apiRefCountLock);

    apiRefCount--;
    if (apiRefCount > 0)
        return PRL_ERR_SUCCESS;

	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s ()",
		__FUNCTION__
		);

	// We're not allowed to create multiple instances of the application
	if ( !QMainThread::IsInitialized() )
		return PRL_ERR_API_WASNT_INITIALIZED;

	// We need to stop the main event loop and deinit the library
	QMainThread::Instance()->stop();
	QMainThread::Deinit();

	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlApi_GetVersion ) (
								 PRL_UINT32_PTR version
								 )
{
	SYNC_CHECK_API_INITIALIZED
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (version=%p)",
		__FUNCTION__,
		version
		);

	if ( PRL_WRONG_PTR(version) )
		return PRL_ERR_INVALID_ARG;

	// Return SDK version with possible sequence number
	*version = s_SdkVersion;

	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlApi_GetAppMode ) (
								 PRL_APPLICATION_MODE_PTR app_mode
								 )
{
	SYNC_CHECK_API_INITIALIZED
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (app_mode=%p)",
		__FUNCTION__,
		app_mode
		);

	if ( PRL_WRONG_PTR(app_mode) )
		return PRL_ERR_INVALID_ARG;

	// Return SDK application mode
	*app_mode = s_SdkAppMode;

	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlApi_GetResultDescription ) (
	PRL_RESULT nErrCode,
	PRL_BOOL bIsBriefMessage,
	PRL_BOOL bFormated,
	PRL_STR sErrString,
	PRL_UINT32_PTR pnErrStringBufLength
	)
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (result=%.8X, bIsBriefMessage=%.8X, "\
		"bFormated=%.8X, sErrString=%p, pnErrStringBufLength=%p",
		__FUNCTION__,
		nErrCode,
		bIsBriefMessage,
		bFormated,
		sErrString,
		pnErrStringBufLength
		);

	if ( PRL_WRONG_PTR(pnErrStringBufLength) )
		return PRL_ERR_INVALID_ARG;

	if ( PrlErrStringsStorage::Contains(nErrCode) )
		return CopyStringValue(PrlErrStringsStorage::GetErrString(nErrCode, bIsBriefMessage, bFormated),
		sErrString, pnErrStringBufLength);

	return CopyStringValue(UTF8_2QSTR(PRL_RESULT_TO_STRING(nErrCode)), sErrString, pnErrStringBufLength);
}

PRL_METHOD( PrlApi_GetMessageType ) (
									 PRL_RESULT nErrCode,
									 PRL_MESSAGE_TYPE_ENUM_PTR pnMsgType
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (nErrCode=%.8X, pnMsgType=%p",
		__FUNCTION__,
		nErrCode,
		pnMsgType
		);

	if ( PRL_WRONG_PTR(pnMsgType) )
		return PRL_ERR_INVALID_ARG;

	*pnMsgType = PrlErrStringsStorage::GetMessageType(nErrCode);

	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlApi_MsgCanBeIgnored ) (
									  PRL_RESULT nErrCode,
									  PRL_BOOL_PTR pbCanBeIgnored
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (nErrCode=%.8X, pbCanBeIgnored=%p",
		__FUNCTION__,
		nErrCode,
		pbCanBeIgnored
		);

	if ( PRL_WRONG_PTR(pbCanBeIgnored) )
		return PRL_ERR_INVALID_ARG;

	*pbCanBeIgnored = (PRL_BOOL)PrlErrStringsStorage::CanMessageBeIgnored(nErrCode);

	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlHandle_AddRef ) (
								PRL_HANDLE handle
								)
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p)",
		__FUNCTION__,
		handle
		);

	SYNC_CHECK_API_INITIALIZED

	// The only thing we can do with them - is to decrease handle reference count
	PrlHandleBasePtr pHandle = PRL_OBJECT_BY_HANDLE<PrlHandleBase>( handle );
	if (pHandle)
		pHandle->AddRef();
	else
		return (PRL_ERR_INVALID_ARG);

	// We have done what we were asked for
	return PRL_ERR_SUCCESS;
}


PRL_METHOD( PrlHandle_Free) (
							 PRL_HANDLE handle
							 )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p)",
		__FUNCTION__,
		handle
		);

	SYNC_CHECK_API_INITIALIZED

	// The only thing we can do with them - is to decrease handle reference count
	PrlHandleBasePtr pHandle = PRL_OBJECT_BY_HANDLE<PrlHandleBase>( handle );
	if (pHandle)
	{
		LOG_MESSAGE( DBG_DEBUG, "%s (ref count=%d type=%.8X)",
			__FUNCTION__,
			pHandle->GetRefCount(),
			pHandle->GetType()
			);
		pHandle->Release();
	}
	else
		return (PRL_ERR_INVALID_ARG);

	// We have done what we were asked for
	return PRL_ERR_SUCCESS;
}


PRL_METHOD( PrlHandle_GetType ) (
								 PRL_HANDLE handle,
								 PRL_HANDLE_TYPE_PTR type
								 )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (handle=%p type=%p)",
		__FUNCTION__,
		handle,
		type
		);

	SYNC_CHECK_API_INITIALIZED

	// Handles should be valid pointers
	if ( PRL_WRONG_HANDLE(handle) || PRL_WRONG_PTR(type) )
		return PRL_ERR_INVALID_ARG;

	// Casting handle to the valid object and continuing
	PrlHandleBasePtr pHandle = PRL_OBJECT_BY_HANDLE<PrlHandleBase>( handle );
	*type = pHandle->GetType();

	// We have done what we were asked for
	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlHandle_RegEventHandler ) (
		PRL_HANDLE hObj,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		)
{
	SYNC_CHECK_API_INITIALIZED

	PRL_RESULT err = PRL_ERR_INVALID_ARG;
	IEventSourceHandle *pES;
	if (PRL_RIGHT_PTR(handler) &&
		getIEventSourceHandle(hObj, &pES))
	{
		err = pES->RegEventHandler(handler, userData);
	}
	return err;
}

PRL_METHOD( PrlHandle_UnregEventHandler ) (
		PRL_HANDLE hObj,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		)
{
	SYNC_CHECK_API_INITIALIZED

	PRL_RESULT err = PRL_ERR_INVALID_ARG;
	IEventSourceHandle *pES;
	if (PRL_RIGHT_PTR(handler) &&
		getIEventSourceHandle(hObj, &pES))
	{
		err = pES->UnregEventHandler(handler, userData);
	}
	return err;
}

PRL_METHOD( PrlHandle_FromString ) (
		PRL_HANDLE hHandle,
		PRL_CONST_STR sXml
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHandle=%p, sXml=%s)",
		__FUNCTION__,
		hHandle,
		sXml
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hHandle) || PRL_WRONG_PTR( sXml ) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleBasePtr pObj = PRL_OBJECT_BY_HANDLE<PrlHandleBase>(hHandle);
	return pObj->fromString(sXml);
}

PRL_METHOD( PrlHandle_ToString ) (
		PRL_HANDLE hHandle,
		PRL_VOID_PTR_PTR ppXml
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHandle=%p, ppXml=%p)",
		__FUNCTION__,
		hHandle,
		ppXml
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hHandle) ||	PRL_WRONG_PTR( ppXml ) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleBasePtr pObj = PRL_OBJECT_BY_HANDLE<PrlHandleBase>(hHandle);

	QString qsXml = pObj->toString();
	if (qsXml.isEmpty())
		return PRL_ERR_UNIMPLEMENTED;

	*ppXml = strdup(pObj->toString().toUtf8().data());
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlApi_CreateStringsList ) (
										PRL_HANDLE_PTR phStringsList
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (phStringsList=%p)",
		__FUNCTION__,
		phStringsList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_PTR(phStringsList))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleStringsList *pStringsList = new PrlHandleStringsList;
	if (!pStringsList)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phStringsList = pStringsList->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlStrList_GetItemsCount ) (
										PRL_HANDLE hStringsList,
										PRL_UINT32_PTR pnItemsCount
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStringsList=%p, pnItemsCount=%p)",
		__FUNCTION__,
		hStringsList,
		pnItemsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hStringsList, PHT_STRINGS_LIST) || PRL_WRONG_PTR(pnItemsCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleStringsListPtr pStringsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hStringsList);
	return (pStringsList->GetItemsCount(pnItemsCount));
}

PRL_METHOD( PrlStrList_AddItem ) (
								  PRL_HANDLE hStringsList,
								  PRL_CONST_STR sItem
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStringsList=%p, sItem=%s)",
		__FUNCTION__,
		hStringsList,
		sItem
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hStringsList, PHT_STRINGS_LIST) || PRL_WRONG_PTR(sItem))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleStringsListPtr pStringsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hStringsList);
	return (pStringsList->AddItem(sItem));
}

PRL_METHOD( PrlStrList_GetItem ) (
								  PRL_HANDLE hStringsList,
								  PRL_UINT32 nItemIndex,
								  PRL_STR sItemBuf,
								  PRL_UINT32_PTR pnItemBufSize
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStringsList=%p, nItemIndex=%.8X, sItemBuf=%p, pnItemBufSize=%p)",
		__FUNCTION__,
		hStringsList,
		nItemIndex,
		sItemBuf,
		pnItemBufSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hStringsList, PHT_STRINGS_LIST) || PRL_WRONG_PTR(pnItemBufSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleStringsListPtr pStringsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hStringsList);
	return (pStringsList->GetItem(nItemIndex, sItemBuf, pnItemBufSize));
}

PRL_METHOD( PrlStrList_RemoveItem ) (
									 PRL_HANDLE hStringsList,
									 PRL_UINT32 nItemIndex
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hStringsList=%p, nItemIndex=%.8X)",
		__FUNCTION__,
		hStringsList,
		nItemIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hStringsList, PHT_STRINGS_LIST))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleStringsListPtr pStringsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hStringsList);
	return (pStringsList->RemoveItem(nItemIndex));
}

PRL_METHOD( PrlApi_CreateHandlesList ) (
										PRL_HANDLE_PTR phHandlesList
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (phHandlesList=%p)",
		__FUNCTION__,
		phHandlesList
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_PTR(phHandlesList))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHandlesList *pHandlesList = new PrlHandleHandlesList;
	if (!pHandlesList)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phHandlesList = pHandlesList->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlHndlList_GetItemsCount ) (
	PRL_HANDLE hHandlesList,
	PRL_UINT32_PTR pnItemsCount
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHandlesList=%p, pnItemsCount=%p)",
		__FUNCTION__,
		hHandlesList,
		pnItemsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHandlesList, PHT_HANDLES_LIST) || PRL_WRONG_PTR(pnItemsCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHandlesListPtr pHandlesList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hHandlesList);
	return (pHandlesList->GetItemsCount(pnItemsCount));
}

PRL_METHOD( PrlHndlList_AddItem ) (
								   PRL_HANDLE hHandlesList,
								   PRL_HANDLE hItem
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHandlesList=%p, hItem=%p)",
		__FUNCTION__,
		hHandlesList,
		hItem
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHandlesList, PHT_HANDLES_LIST) || PRL_WRONG_HANDLE(hItem, PHT_ERROR))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHandlesListPtr pHandlesList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hHandlesList);
	PrlHandleBasePtr pItemHandle = PRL_OBJECT_BY_HANDLE<PrlHandleBase>(hItem);

	return (pHandlesList->AddItem(pItemHandle));
}

PRL_METHOD( PrlHndlList_GetItem ) (
								   PRL_HANDLE hHandlesList,
								   PRL_UINT32 nItemIndex,
								   PRL_HANDLE_PTR phItem
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHandlesList=%p, nItemIndex=%.8X, phItem=%p)",
		__FUNCTION__,
		hHandlesList,
		nItemIndex,
		phItem
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHandlesList, PHT_HANDLES_LIST) || PRL_WRONG_PTR(phItem))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHandlesListPtr pHandlesList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hHandlesList);
	return (pHandlesList->GetItem(nItemIndex, phItem));
}

PRL_METHOD( PrlHndlList_RemoveItem ) (
									  PRL_HANDLE hHandlesList,
									  PRL_UINT32 nItemIndex
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hHandlesList=%p, nItemIndex=%.8X)",
		__FUNCTION__,
		hHandlesList,
		nItemIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hHandlesList, PHT_HANDLES_LIST))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleHandlesListPtr pHandlesList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hHandlesList);
	return (pHandlesList->RemoveItem(nItemIndex));
}

PRL_METHOD( PrlApi_CreateOpTypeList ) (
										PRL_HANDLE_PTR phOpTypeList,
										PRL_SIZE nTypeSize
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (phOpTypeList=%p, nTypeSize=%.8X)",
		__FUNCTION__,
		phOpTypeList,
		nTypeSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_PTR(phOpTypeList))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleOpTypeList *pOpTypeList = NULL;
	try
	{
		pOpTypeList = PrlHandleOpTypeList::CreateInstance(nTypeSize);
	}
	catch (PRL_RESULT e)
	{
		return (e);
	}

	if (!pOpTypeList)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phOpTypeList = pOpTypeList->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlOpTypeList_GetItemsCount ) (
										PRL_HANDLE hOpTypeList,
										PRL_UINT32_PTR pnItemsCount
										)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hOpTypeList=%p, pnItemsCount=%p)",
		__FUNCTION__,
		hOpTypeList,
		pnItemsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOpTypeList, PHT_OPAQUE_TYPE_LIST) || PRL_WRONG_PTR(pnItemsCount))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleOpTypeListPtr pOpTypeList = PRL_OBJECT_BY_HANDLE<PrlHandleOpTypeList>(hOpTypeList);
	return (pOpTypeList->GetItemsCount(pnItemsCount));
}

PRL_METHOD( PrlOpTypeList_AddItem ) (
								  PRL_HANDLE hOpTypeList,
								  PRL_CONST_VOID_PTR pnItem
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hOpTypeList=%p, pnItem=%p)",
		__FUNCTION__,
		hOpTypeList,
		pnItem
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOpTypeList, PHT_OPAQUE_TYPE_LIST) || PRL_WRONG_PTR(pnItem))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleOpTypeListPtr pOpTypeList = PRL_OBJECT_BY_HANDLE<PrlHandleOpTypeList>(hOpTypeList);
	return (pOpTypeList->AddItem(pnItem));
}

PRL_METHOD( PrlOpTypeList_GetItem ) (
								  PRL_HANDLE hOpTypeList,
								  PRL_UINT32 nItemIndex,
								  PRL_VOID_PTR pnItem
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hOpTypeList=%p, nItemIndex=%.8X, pnItem=%p)",
		__FUNCTION__,
		hOpTypeList,
		nItemIndex,
		pnItem
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOpTypeList, PHT_OPAQUE_TYPE_LIST) || PRL_WRONG_PTR(pnItem))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleOpTypeListPtr pOpTypeList = PRL_OBJECT_BY_HANDLE<PrlHandleOpTypeList>(hOpTypeList);
	return (pOpTypeList->GetItem(nItemIndex, pnItem));
}

PRL_METHOD( PrlOpTypeList_RemoveItem ) (
									 PRL_HANDLE hOpTypeList,
									 PRL_UINT32 nItemIndex
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hOpTypeList=%p, nItemIndex=%.8X)",
		__FUNCTION__,
		hOpTypeList,
		nItemIndex
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOpTypeList, PHT_OPAQUE_TYPE_LIST))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleOpTypeListPtr pOpTypeList = PRL_OBJECT_BY_HANDLE<PrlHandleOpTypeList>(hOpTypeList);
	return (pOpTypeList->RemoveItem(nItemIndex));
}

PRL_METHOD( PrlOpTypeList_GetTypeSize ) (
									 PRL_HANDLE hOpTypeList,
									 PRL_SIZE_PTR pnTypeSize
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hOpTypeList=%p, pnTypeSize=%p)",
		__FUNCTION__,
		hOpTypeList,
		pnTypeSize
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hOpTypeList, PHT_OPAQUE_TYPE_LIST) || PRL_WRONG_PTR(pnTypeSize))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleOpTypeListPtr pOpTypeList = PRL_OBJECT_BY_HANDLE<PrlHandleOpTypeList>(hOpTypeList);
	return (pOpTypeList->GetTypeSize(pnTypeSize));
}

/*
PRL_RESULT PrlJob_Wait_Impl(PRL_HANDLE hJob, PRL_UINT32 msecs)
{
	// Handles should be valid pointers
	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) )
		RETURN_RES(PRL_ERR_INVALID_ARG)

		// We need to be sure that this job was registered one day
		PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	RETURN_RES(pJob->Wait(msecs))
}
*/

PRL_METHOD( PrlJob_Wait ) (
						   PRL_HANDLE hJob,
						   PRL_UINT32 msecs
						   )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p)",
	__FUNCTION__,
	hJob
	);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) )
		return (PRL_ERR_INVALID_ARG);

	// We need to be sure that this job was registered one day
	PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	return (pJob->Wait(msecs));
}

PRL_HANDLE PrlJob_Cancel_Impl(PRL_HANDLE hJob)
{
	// Handles should be valid pointers
	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_JOB_CANCEL))

		// We need to be sure that this job was registered one day
		PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	RETURN_RES(pJob->Cancel())
}

PRL_ASYNC_METHOD( PrlJob_Cancel ) (
								   PRL_HANDLE hJob
								   )
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_JOB_CANCEL)
		// Logging API calls for the possibility of debug trace
		LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p)",
		__FUNCTION__,
		hJob
		);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlJob_Cancel, (hJob))
}

PRL_METHOD( PrlJob_GetStatus ) (
								PRL_HANDLE hJob,
								PRL_JOB_STATUS_PTR status
								)
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p status=%p)",
		__FUNCTION__,
		hJob,
		status
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) || PRL_WRONG_PTR(status) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this job was registered one day
	PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	return pJob->GetStatus( status );
}

PRL_METHOD( PrlJob_GetProgress ) (
								  PRL_HANDLE hJob,
								  PRL_UINT32_PTR percentage
								  )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p percentage=%p)",
		__FUNCTION__,
		hJob,
		percentage
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) || PRL_WRONG_PTR(percentage) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this job was registered one day
	PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	return pJob->GetProgress( percentage );
}

PRL_METHOD( PrlJob_GetRetCode ) (
								 PRL_HANDLE hJob,
								 PRL_RESULT_PTR retcode
								 )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p retcode=%p)",
		__FUNCTION__,
		hJob,
		retcode
		);

	SYNC_CHECK_API_INITIALIZED
	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) || PRL_WRONG_PTR(retcode) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this job was registered one day
	PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	return pJob->GetRetCode( retcode );
}

PRL_METHOD( PrlJob_GetDataPtr) (
								PRL_HANDLE hJob,
								PRL_VOID_PTR data_ptr,
								PRL_UINT32_PTR data_size
								)
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p data_ptr=%p)",
		__FUNCTION__,
		hJob,
		data_ptr
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this job was registered one day
	PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	return pJob->GetDataPtr( data_ptr, data_size );

}

PRL_METHOD ( PrlJob_GetResult ) (
								 PRL_HANDLE hJob,
								 PRL_HANDLE_PTR handle
								 )
{
	SYNC_CHECK_API_INITIALIZED

	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p handle=%p)",
		__FUNCTION__,
		hJob,
		handle
		);

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) || PRL_WRONG_PTR(handle) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this job was registered one day
	PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	return pJob->GetResult( handle );
}

PRL_METHOD( PrlJob_GetError ) (
							   PRL_HANDLE hJob,
							   PRL_HANDLE_PTR phError
							   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p phError=%p)",
		__FUNCTION__,
		hJob,
		phError
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) || PRL_WRONG_PTR(phError) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	return pJob->GetError( phError );
}

PRL_METHOD( PrlJob_GetOpCode ) (
								PRL_HANDLE hJob,
								PRL_JOB_OPERATION_CODE_PTR pnJobOpCode
								)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p pnJobType=%p)",
		__FUNCTION__,
		hJob,
		pnJobOpCode
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) || PRL_WRONG_PTR(pnJobOpCode) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	return pJob->GetOpCode( pnJobOpCode );
}

PRL_METHOD( PrlJob_IsRequestWasSent ) (
							   PRL_HANDLE hJob,
							   PRL_BOOL_PTR pbIsRequestWasSent
							   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hJob=%p pbIsRequestWasSent=%p)",
		__FUNCTION__,
		hJob,
		pbIsRequestWasSent
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hJob, PHT_JOB) || PRL_WRONG_PTR(pbIsRequestWasSent) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleJobPtr pJob = PRL_OBJECT_BY_HANDLE<PrlHandleJob>( hJob );

	return pJob->IsRequestWasSent( pbIsRequestWasSent );
}

PRL_METHOD( PrlResult_GetParamsCount ) (
										PRL_HANDLE hResult,
										PRL_UINT32_PTR pCount
										)
{
	SYNC_CHECK_API_INITIALIZED

	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hResult=%p pCount=%p)",
		__FUNCTION__,
		hResult,
		pCount
		);

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hResult, PHT_RESULT) || PRL_WRONG_PTR(pCount) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this job was registered one day
	PrlHandleResultPtr pResult = PRL_OBJECT_BY_HANDLE<PrlHandleResult>( hResult );

	return pResult->GetParamsCount( pCount );
}

PRL_METHOD( PrlResult_GetParamByIndex ) (
	PRL_HANDLE hResult,
	PRL_UINT32 nIndex,
	PRL_HANDLE_PTR phParam
	)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hResult=%p nIndex=%.8X, phParam=%p)",
		__FUNCTION__,
		hResult,
		nIndex,
		phParam
		);

	if ( PRL_WRONG_HANDLE(hResult, PHT_RESULT) || PRL_WRONG_PTR(phParam) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleResultPtr pResult = PRL_OBJECT_BY_HANDLE<PrlHandleResult>( hResult );

	return pResult->GetParamByIndex( nIndex, phParam );
}

PRL_METHOD( PrlResult_GetParam ) (
								  PRL_HANDLE hResult,
								  PRL_HANDLE_PTR pHandle
								  )
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hResult=%p pHandle=%p)",
		__FUNCTION__,
		hResult,
		pHandle
		);

	if ( PRL_WRONG_HANDLE(hResult, PHT_RESULT) || PRL_WRONG_PTR(pHandle))
		return PRL_ERR_INVALID_ARG;

	PrlHandleResultPtr pResult = PRL_OBJECT_BY_HANDLE<PrlHandleResult>( hResult );

	return pResult->GetResultAsHandle( pHandle );
}

PRL_METHOD(PrlResult_GetParamByIndexAsString) (
	PRL_HANDLE hResult,
	PRL_UINT32 nIndex,
	PRL_STR sParamBuf,
	PRL_UINT32_PTR pnParamBufSize
	)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hResult=%p, nIndex=%.8X, sParamBuf=%p, pnParamBufSize=%p)",
		__FUNCTION__,
		hResult,
		nIndex,
		sParamBuf,
		pnParamBufSize
		);

	if ( PRL_WRONG_HANDLE(hResult, PHT_RESULT) || PRL_WRONG_PTR(pnParamBufSize) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleResultPtr pResult = PRL_OBJECT_BY_HANDLE<PrlHandleResult>( hResult );

	return pResult->GetParamByIndexAsString( nIndex, sParamBuf, pnParamBufSize );
}

PRL_METHOD(PrlResult_GetParamAsString) (
										PRL_HANDLE hResult,
										PRL_STR sParamBuf,
										PRL_UINT32_PTR pnParamBufSize
										)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%s (hResult=%p, sParamBuf=%p, pnParamBufSize=%p)",
		__FUNCTION__,
		hResult,
		sParamBuf,
		pnParamBufSize
		);

	if ( PRL_WRONG_HANDLE(hResult, PHT_RESULT) || PRL_WRONG_PTR(pnParamBufSize) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleResultPtr pResult = PRL_OBJECT_BY_HANDLE<PrlHandleResult>( hResult );

	return pResult->GetParamByIndexAsString( 0, sParamBuf, pnParamBufSize );
}

PRL_METHOD( PrlEvent_GetType ) (
								PRL_HANDLE hEvent,
								PRL_EVENT_TYPE_PTR type
								)
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p type=%p)",
		__FUNCTION__,
		hEvent,
		type
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(type) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this event was registered one day
	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetEventType( type );
}

PRL_METHOD( PrlEvent_GetDataPtr ) (
								   PRL_HANDLE hEvent,
								   PRL_VOID_PTR_PTR data_ptr
								   )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p data_ptr=%p)",
		__FUNCTION__,
		hEvent,
		data_ptr
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(data_ptr) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this event was registered one day
	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetDataPtr( data_ptr );
}

PRL_METHOD( PrlEvent_GetServer) (
								 PRL_HANDLE hEvent,
								 PRL_HANDLE_PTR server
								 )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p server=%p)",
		__FUNCTION__,
		hEvent,
		server
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(server) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this event was registered one day
	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetServer( server );
}

PRL_METHOD( PrlEvent_GetVm) (
							 PRL_HANDLE hEvent,
							 PRL_HANDLE_PTR pVm
							 )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p pVm=%p)",
		__FUNCTION__,
		hEvent,
		pVm
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(pVm) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this event was registered one day
	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetVm( pVm );
}

PRL_METHOD( PrlEvent_GetJob) (
							  PRL_HANDLE hEvent,
							  PRL_HANDLE_PTR pJob
							  )
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p pJob=%p)",
		__FUNCTION__,
		hEvent,
		pJob
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(pJob) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this event was registered one day
	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );
#ifdef _WIN_
#ifdef GetJob
#undef GetJob
#endif
#endif
	return pEvent->GetJob( pJob );
}

PRL_METHOD( PrlEvent_GetParamsCount ) (
									   PRL_HANDLE hEvent,
									   PRL_UINT32_PTR pnParamsCount
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p pnParamsCount=%p)",
		__FUNCTION__,
		hEvent,
		pnParamsCount
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(pnParamsCount) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetParamsCount( pnParamsCount );
}

PRL_METHOD( PrlEvent_GetParam ) (
								 PRL_HANDLE hEvent,
								 PRL_UINT32 nIndex,
								 PRL_HANDLE_PTR phEventParam
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p phEventParam=%p)",
		__FUNCTION__,
		hEvent,
		phEventParam
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(phEventParam) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetParam( nIndex, phEventParam );
}

PRL_METHOD( PrlEvent_GetParamByName ) (
									   PRL_HANDLE hEvent,
									   PRL_CONST_STR sParamName,
									   PRL_HANDLE_PTR phEventParam
									   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p sParamName=%s, phEventParam=%p)",
		__FUNCTION__,
		hEvent,
		sParamName,
		phEventParam
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(sParamName) || PRL_WRONG_PTR(phEventParam) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetParamByName( sParamName, phEventParam );
}

PRL_METHOD( PrlEvent_GetErrCode ) (
								   PRL_HANDLE hEvent,
								   PRL_RESULT_PTR pnErrCode
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p pnErrCode=%p)",
		__FUNCTION__,
		hEvent,
		pnErrCode
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(pnErrCode) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetErrCode( pnErrCode );
}

PRL_METHOD( PrlEvent_GetErrString ) (
									 PRL_HANDLE hEvent,
									 PRL_BOOL bIsBriefMessage,
									 PRL_BOOL bFormated,
									 PRL_STR	sErrString,
									 PRL_UINT32_PTR pnErrStringBufLength
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p, sErrString=%p, pnErrStringBufLength=%p)",
		__FUNCTION__,
		hEvent,
		sErrString,
		pnErrStringBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(pnErrStringBufLength) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetErrString( bIsBriefMessage, bFormated, sErrString, pnErrStringBufLength );
}

PRL_METHOD( PrlEvent_CanBeIgnored ) (
									 PRL_HANDLE hEvent,
									 PRL_BOOL_PTR pbCanBeIgnored
									 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p pbCanBeIgnored=%p)",
		__FUNCTION__,
		hEvent,
		pbCanBeIgnored
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(pbCanBeIgnored) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->CanBeIgnored( pbCanBeIgnored );
}

PRL_METHOD( PrlEvent_IsAnswerRequired ) (
	PRL_HANDLE hEvent,
	PRL_BOOL_PTR pbIsAnswerRequired
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p pbIsAnswerRequired=%p)",
		__FUNCTION__,
		hEvent,
		pbIsAnswerRequired
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(pbIsAnswerRequired) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->IsAnswerRequired( pbIsAnswerRequired );
}

PRL_METHOD( PrlEvent_GetIssuerType ) (
									  PRL_HANDLE hEvent,
									  PRL_EVENT_ISSUER_TYPE_PTR pnIssuerType
									  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p pnIssuerType=%p)",
		__FUNCTION__,
		hEvent,
		pnIssuerType
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(pnIssuerType) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetIssuerType( pnIssuerType );
}

PRL_METHOD( PrlEvent_GetIssuerId ) (
									PRL_HANDLE hEvent,
									PRL_STR	sIssuerId,
									PRL_UINT32_PTR pnIssuerIdBufLength
									)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p, sIssuerId=%p, pnIssuerIdBufLength=%p)",
		__FUNCTION__,
		hEvent,
		sIssuerId,
		pnIssuerIdBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) ||	PRL_WRONG_PTR(pnIssuerIdBufLength) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->GetIssuerId( sIssuerId, pnIssuerIdBufLength );
}

PRL_METHOD( PrlEvent_CreateAnswerEvent ) (
	PRL_HANDLE hEvent,
	PRL_HANDLE_PTR phEvent,
	PRL_RESULT nAnswer
	)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p, phEvent=%p, nAnswer=%.8X)",
		__FUNCTION__,
		hEvent,
		phEvent,
		nAnswer
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(phEvent))
		return (PRL_ERR_INVALID_ARG);

	PrlHandleVmEventPtr pVmEvent = PRL_OBJECT_BY_HANDLE<PrlHandleVmEvent>( hEvent );

	return (pVmEvent->CreateAnswerEvent(phEvent, nAnswer));
}

PRL_METHOD( PrlEvent_FromString ) (
								   PRL_HANDLE hEvent,
								   PRL_CONST_STR sEvent
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEvent=%p sEvent=%s)",
		__FUNCTION__,
		hEvent,
		sEvent
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEvent, PHT_EVENT) || PRL_WRONG_PTR(sEvent) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventPtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleEvent>( hEvent );

	return pEvent->FromString( sEvent );
}

PRL_METHOD( PrlEvtPrm_GetName ) (
								 PRL_HANDLE hEventParam,
								 PRL_STR	sParamName,
								 PRL_UINT32_PTR pnParamNameBufLength
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p sParamName=%p, pnParamNameBufLength=%p)",
		__FUNCTION__,
		hEventParam,
		sParamName,
		pnParamNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(pnParamNameBufLength) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->GetName( sParamName, pnParamNameBufLength );
}

PRL_METHOD( PrlEvtPrm_GetType ) (
								 PRL_HANDLE hEventParam,
								 PRL_PARAM_FIELD_DATA_TYPE_PTR pnParamType
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p pnParamType=%p)",
		__FUNCTION__,
		hEventParam,
		pnParamType
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(pnParamType) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->GetParamType( pnParamType );
}

PRL_METHOD( PrlEvtPrm_ToString ) (
								  PRL_HANDLE hEventParam,
								  PRL_STR	sParamValue,
								  PRL_UINT32_PTR pnParamValueBufLength
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p sParamValue=%p pnParamValueBufLength=%p)",
		__FUNCTION__,
		hEventParam,
		sParamValue,
		pnParamValueBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(pnParamValueBufLength) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->ToString( sParamValue, pnParamValueBufLength );
}

PRL_METHOD( PrlEvtPrm_ToCData ) (
								 PRL_HANDLE hEventParam,
								 PRL_STR	sParamValue,
								 PRL_UINT32_PTR pnParamValueBufLength
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p sParamValue=%p pnParamValueBufLength=%p)",
		__FUNCTION__,
		hEventParam,
		sParamValue,
		pnParamValueBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) ||
		PRL_WRONG_PTR(pnParamValueBufLength) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->ToCData( sParamValue, pnParamValueBufLength );
}

PRL_METHOD( PrlEvtPrm_ToUint32 ) (
								  PRL_HANDLE hEventParam,
								  PRL_UINT32_PTR pnParamValue
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p pnParamValue=%p)",
		__FUNCTION__,
		hEventParam,
		pnParamValue
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(pnParamValue) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->ToUint32( pnParamValue );
}

PRL_METHOD( PrlEvtPrm_ToInt32 ) (
								 PRL_HANDLE hEventParam,
								 PRL_INT32_PTR pnParamValue
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p pnParamValue=%p)",
		__FUNCTION__,
		hEventParam,
		pnParamValue
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(pnParamValue) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->ToInt32( pnParamValue );
}

PRL_METHOD( PrlEvtPrm_ToUint64 ) (
								  PRL_HANDLE hEventParam,
								  PRL_UINT64_PTR pnParamValue
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p pnParamValue=%p)",
		__FUNCTION__,
		hEventParam,
		pnParamValue
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(pnParamValue) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->ToUint64( pnParamValue );
}

PRL_METHOD( PrlEvtPrm_ToInt64 ) (
								 PRL_HANDLE hEventParam,
								 PRL_INT64_PTR pnParamValue
								 )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p pnParamValue=%p)",
		__FUNCTION__,
		hEventParam,
		pnParamValue
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(pnParamValue) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->ToInt64( pnParamValue );
}

PRL_METHOD( PrlEvtPrm_ToBoolean ) (
								   PRL_HANDLE hEventParam,
								   PRL_BOOL_PTR pbParamValue
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p phParamValue=%p)",
		__FUNCTION__,
		hEventParam,
		pbParamValue
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(pbParamValue) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->ToBoolean( pbParamValue );
}

PRL_METHOD( PrlEvtPrm_ToHandle ) (
								  PRL_HANDLE hEventParam,
								  PRL_HANDLE_PTR phParamValue
								  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p phParamValue=%p)",
		__FUNCTION__,
		hEventParam,
		phParamValue
		);

	SYNC_CHECK_API_INITIALIZED

	if (	PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(phParamValue) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->ToHandle( phParamValue );
}

PRL_METHOD( PrlEvtPrm_GetBuffer ) (
		PRL_HANDLE hEventParam,
		PRL_VOID_PTR pParamValue,
		PRL_UINT32_PTR pnParamValueBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (hEventParam=%p pParamValue=%p, pnParamValueBufLength=%p)",
		__FUNCTION__,
		hEventParam,
		pParamValue,
		pnParamValueBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hEventParam, PHT_EVENT_PARAMETER) || PRL_WRONG_PTR(pnParamValueBufLength) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleEventParamPtr pEventParam = PRL_OBJECT_BY_HANDLE<PrlHandleEventParam>( hEventParam );

	return pEventParam->GetBuffer( pParamValue, pnParamValueBufLength );
}

PRL_METHOD( PrlApi_CreateProblemReport ) (
                    PRL_PROBLEM_REPORT_SCHEME nReportScheme,
                    PRL_HANDLE_PTR phProblemReport
        )
{
    WRITE_TRACE( DBG_DEBUG, "%s (nReportScheme=%.8X, phProblemReport=%p)",
        __FUNCTION__,
        nReportScheme,
        phProblemReport
        );

    SYNC_CHECK_API_INITIALIZED

    if ( PRL_WRONG_PTR(phProblemReport) )
        return (PRL_ERR_INVALID_ARG);

    try
    {
        PrlHandleProblemReportBase *pProblemReport = new PrlHandleProblemReport(PrlHandleProblemReport::CreateReportData(nReportScheme));

        if ( !pProblemReport )
            return (PRL_ERR_OUT_OF_MEMORY);

        *phProblemReport = pProblemReport->GetHandle();
    }
    catch (PRL_RESULT nResult)
    {
        return (nResult);
    }

    return (PRL_ERR_SUCCESS);
}

PRL_HANDLE PrlApi_SendPackedProblemReport_Impl(
			PRL_HANDLE hProblemReport,
			PRL_BOOL bUseProxy,
			PRL_CONST_STR sProxyHost,
			PRL_UINT32 nProxyPort,
			PRL_CONST_STR sProxyUserLogin,
			PRL_CONST_STR sProxyUserPasswd,
			PRL_UINT32 nProblemSendTimeout,
			PRL_UINT32 nReserved,
			PRL_EVENT_HANDLER_PTR handler,
			PRL_VOID_PTR pUserData
)
{
	PRL_UNUSED_PARAM(nReserved);
	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_API_SEND_PACKED_PROBLEM_REPORT))

	PrlHandleLocalJobPtr pSendReportJob(new PrlHandleLocalJob(Uuid::createUuid().toString(),
													PJOC_API_SEND_PACKED_PROBLEM_REPORT));
	if (!pSendReportJob.getHandle())
		RETURN_RES(PRL_INVALID_HANDLE);

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>(hProblemReport);

	PrlProblemReportSender *pReportSender = new PrlProblemReportSender(pSendReportJob, pProblemReport, bUseProxy, sProxyHost,
			nProxyPort, sProxyUserLogin, sProxyUserPasswd, nProblemSendTimeout, handler, pUserData);
	if (!pReportSender)
		RETURN_RES(PRL_INVALID_HANDLE);
	pReportSender->start();
	RETURN_RES(pSendReportJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlApi_SendPackedProblemReport ) (
		PRL_HANDLE hProblemReport,
		PRL_BOOL bUseProxy,
		PRL_CONST_STR sProxyHost,
		PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin,
		PRL_CONST_STR sProxyUserPasswd,
		PRL_UINT32 nProblemSendTimeout,
		PRL_UINT32 nReserved,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR pUserData
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_API_SEND_PACKED_PROBLEM_REPORT)
		LOG_MESSAGE( DBG_DEBUG, "%s (hProblemReport=%p, bUseProxy=%.8X, sProxyHost=%s, nProxyPort=%.8X, "\
			"sProxyUserLogin=%s, sProxyUserPasswd=%p, nProblemSendTimeout=%.8X, nReserved=%.8X), "\
			"handler=%p, pUserData=%p",
		__FUNCTION__,
		hProblemReport,
		bUseProxy,
		sProxyHost,
		nProxyPort,
		sProxyUserLogin,
		sProxyUserPasswd,
		nProblemSendTimeout,
		nReserved,
		handler,
		pUserData
		);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlApi_SendPackedProblemReport,\
		(hProblemReport,
		bUseProxy,
		sProxyHost,
		nProxyPort,
		sProxyUserLogin,
		sProxyUserPasswd,
		nProblemSendTimeout,
		nReserved,
		handler,
		pUserData))
}

PRL_METHOD( PrlReport_SetType ) (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_TYPE nReportType
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, nReportType=%.8X)",
		__FUNCTION__,
		hProblemReport,
		nReportType
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->SetType(nReportType));
}

PRL_METHOD( PrlReport_GetType ) (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_TYPE_PTR pnReportType
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, pnReportType=%p)",
		__FUNCTION__,
		hProblemReport,
		pnReportType
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) || PRL_WRONG_PTR(pnReportType) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->GetPRType(pnReportType));
}

PRL_METHOD( PrlReport_SetReason ) (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_REASON nReportReason
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, nReportReason=%.8X)",
		__FUNCTION__,
		hProblemReport,
		nReportReason
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->SetReason(nReportReason));
}

PRL_METHOD( PrlReport_GetReason ) (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_REASON_PTR pnReportReason
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, pnReportReason=%p)",
		__FUNCTION__,
		hProblemReport,
		pnReportReason
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) || PRL_WRONG_PTR(pnReportReason) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->GetReason(pnReportReason));
}

PRL_METHOD( PrlReport_GetUserEmail ) (
		PRL_HANDLE hProblemReport,
		PRL_STR sUserEmail,
		PRL_UINT32_PTR pnUserEmailBufLength
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, sUserEmail=%p, pnUserEmailBufLength=%p)",
		__FUNCTION__,
		hProblemReport,
		sUserEmail,
		pnUserEmailBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->GetUserEmail(sUserEmail, pnUserEmailBufLength));
}

PRL_METHOD( PrlReport_SetUserEmail ) (
		PRL_HANDLE hProblemReport,
		PRL_CONST_STR sNewUserEmail
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, sNewUserEmail=%p)",
		__FUNCTION__,
		hProblemReport,
		sNewUserEmail
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->SetUserEmail(sNewUserEmail));
}

PRL_METHOD( PrlReport_GetDescription ) (
		PRL_HANDLE hProblemReport,
		PRL_STR sDescription,
		PRL_UINT32_PTR pnDescriptionBufLength
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, sDescription=%p, pnDescriptionBufLength=%p)",
		__FUNCTION__,
		hProblemReport,
		sDescription,
		pnDescriptionBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->GetDescription(sDescription, pnDescriptionBufLength));
}

PRL_METHOD( PrlReport_SetDescription ) (
		PRL_HANDLE hProblemReport,
		PRL_CONST_STR sNewDescription
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, sNewDescription=%p)",
		__FUNCTION__,
		hProblemReport,
		sNewDescription
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->SetDescription(sNewDescription));
}

PRL_HANDLE PrlReport_Assembly_Impl(
                                   PRL_HANDLE hProblemReport,
                                   PRL_UINT32 nFlags
                                   )
{
	if ( PRL_WRONG_HANDLE( hProblemReport, PHT_PROBLEM_REPORT ) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_REPORT_ASSEMBLY))

        PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );
	RETURN_RES(pProblemReport->Assembly(nFlags))
}

PRL_ASYNC_METHOD( PrlReport_Assembly ) (
                                        PRL_HANDLE hProblemReport,
                                        PRL_UINT32 nFlags
                                        )
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, nFlags=%.8X)",
                __FUNCTION__,
                hProblemReport,
                nFlags
                );

	ASYNC_CHECK_API_INITIALIZED(PJOC_REPORT_ASSEMBLY)

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlReport_Assembly, (hProblemReport, nFlags))
}

PRL_METHOD( PrlBuffer_Free ) (
							  PRL_VOID_PTR buffer
							  )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (buffer=%p)",
		__FUNCTION__,
		buffer
		);

	if ( PRL_WRONG_PTR(buffer) )
		return PRL_ERR_INVALID_ARG;

	::free(buffer);
	return (PRL_ERR_SUCCESS);
}

PRL_METHOD( PrlDbg_GetHandlesNum) (
								   PRL_UINT32_PTR handles_num,
								   PRL_HANDLE_TYPE type
								   )
{
	LOG_MESSAGE( DBG_DEBUG, "%s (handles_num=%p)",
		__FUNCTION__,
		handles_num
		);

	SYNC_CHECK_API_INITIALIZED

	if (PRL_WRONG_PTR(handles_num))
		return PRL_ERR_INVALID_ARG;

	QMutexLocker _lock(PrlHandleBase::s_pHandlesMapMutex);
	if (PHT_ERROR == type)
		*handles_num = PrlHandleBase::s_pHandlesMap->size();
	else
	{
		PRL_UINT32 nHandlesNum = 0;
		HandlesMap::iterator _handle = PrlHandleBase::s_pHandlesMap->begin();
		for (; _handle != PrlHandleBase::s_pHandlesMap->end(); ++_handle)
			if (_handle.value()->GetType() == type)
				nHandlesNum++;
		*handles_num = nHandlesNum;
	}

	return PRL_ERR_SUCCESS;
}

PRL_METHOD( PrlDbg_PrlResultToString) (PRL_RESULT value, const char **result)
{
	if ( PRL_WRONG_PTR(result) )
		return PRL_ERR_INVALID_ARG ;

	*result = PrlResultToString(value) ;
	return PRL_ERR_SUCCESS ;
}

PRL_METHOD( PrlDbg_HandleTypeToString) (PRL_HANDLE_TYPE value, const char **result)
{
	if ( PRL_WRONG_PTR(result) )
		return PRL_ERR_INVALID_ARG ;

	*result = HandleTypeToString(value) ;
	return PRL_ERR_SUCCESS ;
}

PRL_METHOD( PrlDbg_EventTypeToString) (PRL_EVENT_TYPE value, const char **result)
{
	if ( PRL_WRONG_PTR(result) )
		return PRL_ERR_INVALID_ARG ;

	*result = EventTypeToString(value) ;
	return PRL_ERR_SUCCESS ;
}


PRL_METHOD( PrlHandle_GetPackageId ) (
								PRL_HANDLE hObj,
								PRL_UINT64_PTR id
								)
{
	// Logging API calls for the possibility of debug trace
	LOG_MESSAGE( DBG_DEBUG, "%s (hObj=%p id=%p)",
		__FUNCTION__,
		hObj,
		id
		);

	SYNC_CHECK_API_INITIALIZED

	// We always have to ensure input parameters validity
	if ( ( PRL_WRONG_HANDLE(hObj, PHT_EVENT) && PRL_WRONG_HANDLE(hObj, PHT_JOB) )
		|| PRL_WRONG_PTR(id) )
		return PRL_ERR_INVALID_ARG;

	// We need to be sure that this event was registered one day
	PrlHandleBasePtr pObj = PRL_OBJECT_BY_HANDLE<PrlHandleBase>( hObj );

	return pObj->GetPackageId( id );
}

PRL_METHOD( PrlApi_GetCrashDumpsPath ) (
		PRL_STR sPath,
		PRL_UINT32_PTR pnPathBufLength
		)
{
	LOG_MESSAGE( DBG_DEBUG, "%s (sPath=%p, pnPathBufLength=%p)",
		__FUNCTION__,
		sPath,
		pnPathBufLength
		);

	if ( PRL_WRONG_PTR(pnPathBufLength) )
		return (PRL_ERR_INVALID_ARG);

	return (CopyStringValue(ParallelsDirs::getCrashDumpsPath(), sPath, pnPathBufLength));
}

PRL_METHOD( PrlApi_InitCrashHandler ) (
		PRL_CONST_STR sCrashDumpFileSuffix
		)
{
	LOG_MESSAGE(DBG_WARNING, "%s is depricated, crash dump generation was removed", __FUNCTION__);
	Q_UNUSED(sCrashDumpFileSuffix);
	return PRL_ERR_SUCCESS;
}

PRL_HANDLE PrlApi_SendProblemReport_Impl(
			PRL_CONST_STR sProblemReport,
			PRL_BOOL bUseProxy,
			PRL_CONST_STR sProxyHost,
			PRL_UINT32 nProxyPort,
			PRL_CONST_STR sProxyUserLogin,
			PRL_CONST_STR sProxyUserPasswd,
			PRL_UINT32 nProblemSendTimeout,
			PRL_UINT32 nReserved,
			PRL_EVENT_HANDLER_PTR handler,
			PRL_VOID_PTR pUserData
)
{
	PRL_UNUSED_PARAM(nReserved);
	if ( PRL_WRONG_PTR( sProblemReport) )
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_API_SEND_PROBLEM_REPORT))

	PrlHandleLocalJobPtr pSendReportJob(new PrlHandleLocalJob(Uuid::createUuid().toString(),
													PJOC_API_SEND_PROBLEM_REPORT));
	if (!pSendReportJob.getHandle())
		RETURN_RES(PRL_INVALID_HANDLE);
	PrlProblemReportSender *pReportSender = new PrlProblemReportSender(pSendReportJob, sProblemReport, bUseProxy, sProxyHost,
			nProxyPort, sProxyUserLogin, sProxyUserPasswd, nProblemSendTimeout, handler, pUserData);
	if (!pReportSender)
		RETURN_RES(PRL_INVALID_HANDLE);
	pReportSender->start();
	RETURN_RES(pSendReportJob->GetHandle())
}

PRL_ASYNC_METHOD( PrlApi_SendProblemReport ) (
		PRL_CONST_STR sProblemReport,
		PRL_BOOL bUseProxy,
		PRL_CONST_STR sProxyHost,
		PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin,
		PRL_CONST_STR sProxyUserPasswd,
		PRL_UINT32 nProblemSendTimeout,
		PRL_UINT32 nReserved,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR pUserData
		)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_API_SEND_PROBLEM_REPORT)
		LOG_MESSAGE( DBG_DEBUG, "%s (sProblemReport=%s, bUseProxy=%.8X, sProxyHost=%s, nProxyPort=%.8X, "\
			"sProxyUserLogin=%s, sProxyUserPasswd=%p, nProblemSendTimeout=%.8X, nReserved=%.8X), "\
			"handler=%p, pUserData=%p",
		__FUNCTION__,
		sProblemReport,
		bUseProxy,
		sProxyHost,
		nProxyPort,
		sProxyUserLogin,
		sProxyUserPasswd,
		nProblemSendTimeout,
		nReserved,
		handler,
		pUserData
		);

	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), PrlApi_SendProblemReport,\
		(sProblemReport,
		bUseProxy,
		sProxyHost,
		nProxyPort,
		sProxyUserLogin,
		sProxyUserPasswd,
		nProblemSendTimeout,
		nReserved,
		handler,
		pUserData))
}

PRL_METHOD( PrlReport_GetScheme ) (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_SCHEME_PTR pnReportScheme
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, pnReportScheme=%p)",
		__FUNCTION__,
		hProblemReport,
		pnReportScheme
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) || PRL_WRONG_PTR(pnReportScheme) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->GetScheme(pnReportScheme));
}

PRL_METHOD( PrlReport_GetArchiveFileName ) (
		PRL_HANDLE hProblemReport,
		PRL_STR sArchiveFileName,
		PRL_UINT32_PTR pnBufLength
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, sArchiveFileName=%p, pnBufLength=%p)",
		__FUNCTION__,
		hProblemReport,
		sArchiveFileName,
		pnBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->GetArchiveFileName(sArchiveFileName, pnBufLength));
}

PRL_METHOD( PrlReport_GetUserName ) (
		PRL_HANDLE hProblemReport,
		PRL_STR sUserName,
		PRL_UINT32_PTR pnUserNameBufLength
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, sUserName=%p, pnUserNameBufLength=%p)",
		__FUNCTION__,
		hProblemReport,
		sUserName,
		pnUserNameBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );
#ifdef _WIN_
#ifdef GetUserName
#undef GetUserName
#endif
#endif
	return (pProblemReport->GetUserName(sUserName, pnUserNameBufLength));
}

PRL_METHOD( PrlReport_SetUserName ) (
		PRL_HANDLE hProblemReport,
		PRL_CONST_STR sNewUserName
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, sNewUserName=%p)",
		__FUNCTION__,
		hProblemReport,
		sNewUserName
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->SetUserName(sNewUserName));
}

PRL_METHOD( PrlReport_GetData ) (
		PRL_HANDLE hProblemReport,
		PRL_VOID_PTR pBuffer,
		PRL_UINT32_PTR pnBufLength
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, pBuffer=%p, pnBufLength=%p)",
		__FUNCTION__,
		hProblemReport,
		pBuffer,
		pnBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->GetData(pBuffer, pnBufLength));
}

PRL_METHOD( PrlReport_AsString ) (
		PRL_HANDLE hProblemReport,
		PRL_STR sBuffer,
		PRL_UINT32_PTR pnBufLength
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, sBuffer=%p, pnBufLength=%p)",
		__FUNCTION__,
		hProblemReport,
		sBuffer,
		pnBufLength
		);

	SYNC_CHECK_API_INITIALIZED

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return PRL_ERR_INVALID_ARG;

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	return (pProblemReport->AsString(sBuffer, pnBufLength));
}

PRL_ASYNC_METHOD( PrlReport_Send ) (
		PRL_HANDLE hProblemReport,
		PRL_BOOL bUseProxy,
		PRL_CONST_STR sProxyHost,
		PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin,
		PRL_CONST_STR sProxyUserPasswd,
		PRL_UINT32 nProblemSendTimeout,
		PRL_UINT32 nReserved,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR pUserData
		)
{
	WRITE_TRACE( DBG_DEBUG, "%s (hProblemReport=%p, bUseProxy=%.8X, sProxyHost=%p, nProxyPort=%.8X, "\
			"sProxyUserLogin=%p, sProxyUserPasswd=%p, nProblemSendTimeout=%.8X, nReserved=%.8X), "\
			"handler=%p, pUserData=%p",
		__FUNCTION__,
		hProblemReport,
		bUseProxy,
		sProxyHost,
		nProxyPort,
		sProxyUserLogin,
		sProxyUserPasswd,
		nProblemSendTimeout,
		nReserved,
		handler,
		pUserData
		);

	ASYNC_CHECK_API_INITIALIZED(PJOC_API_SEND_PACKED_PROBLEM_REPORT)

	if ( PRL_WRONG_HANDLE(hProblemReport, PHT_PROBLEM_REPORT) )
		return (GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, PJOC_API_SEND_PROBLEM_REPORT));

	PrlHandleProblemReportPtr pProblemReport = PRL_OBJECT_BY_HANDLE<PrlHandleProblemReport>( hProblemReport );

	if (PRS_OLD_XML_BASED == pProblemReport->GetScheme())
	{
		return (PrlApi_SendProblemReport(
					QSTR2UTF8(pProblemReport->toString()),
					bUseProxy,
					sProxyHost,
					nProxyPort,
					sProxyUserLogin,
					sProxyUserPasswd,
					nProblemSendTimeout,
					nReserved,
					handler,
					pUserData)
				);
	}
	else
	{
		return (PrlApi_SendPackedProblemReport(
					hProblemReport,
					bUseProxy,
					sProxyHost,
					nProxyPort,
					sProxyUserLogin,
					sProxyUserPasswd,
					nProblemSendTimeout,
					nReserved,
					handler,
					pUserData)
				);
	}
}

PRL_METHOD( PrlApi_SwitchConsoleLogging ) ( PRL_BOOL bSwitchOn )
{
	SetConsoleLogging(bSwitchOn == PRL_TRUE);

        return 0;
}

PRL_METHOD( PrlApi_GuestToString ) (
									PRL_UINT32 nGuestId,
									PRL_STR sGuestString,
									PRL_UINT32_PTR pnGuestStringSize
									)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE ( DBG_DEBUG, "%s (nGuestId=%d, sGuestString=%p)",
		__FUNCTION__,
		nGuestId,
		sGuestString
		);

	if (PRL_WRONG_PTR(pnGuestStringSize))
		return PRL_ERR_INVALID_ARG;

	return (CopyStringValue(PVS_GUEST_TO_STRING(nGuestId), sGuestString, pnGuestStringSize));
}
