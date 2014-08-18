/*
 * PrlHandleServerDisp.cpp
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


#include "Libraries/Std/PrlAssert.h"

#include "PrlHandleServerJob.h"
#include "PrlHandleLoginLocalHelperJob.h"
#include "PrlHandleLoginHelperJob.h"
#include "PrlHandleServerInfo.h"
#include "PrlHandleEventParam.h"
#include "PrlHandleProblemReportBase.h"
#include "PrlHandleServerDisp.h"

#include "XmlModel/HostHardwareInfo/GenericPciDevices.h"

#include "PrlHandleDispConfig.h"
#include "PrlHandleUserProfile.h"

#ifdef _WIN_
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif


void PrlHandleServerDisp::SetNonInteractiveSession(PRL_BOOL bNonInteractive)
{
	QMutexLocker _lock(&m_MembersMutex);
	m_bNonInteractiveSession = bNonInteractive;
}

void PrlHandleServerDisp::SetConfirmationMode(PRL_BOOL bConfirmationModeEnabled)
{
	QMutexLocker _lock(&m_MembersMutex);
	m_bConfirmationModeEnabled = bConfirmationModeEnabled;
}

PrlQuestionsList& PrlHandleServerDisp::GetQuestionsList()
{
	return (m_QuestionsList);
}


PrlHandleJobPtr PrlHandleServerDisp::Login(
                                PRL_CONST_STR host,
                                PRL_CONST_STR user,
                                PRL_CONST_STR passwd,
				PRL_CONST_STR prevSessionUuid,
				PRL_UINT32 port_cmd,
				PRL_UINT32 timeout,
				PRL_SECURITY_LEVEL security_level,
				PRL_UINT32 flags
				)
{
	if ( host )
	{
		InitializeConnection();
		QMutexLocker _lock(&m_MembersMutex);
		InitConnectionSettings(UTF8_2QSTR(host), false, false, port_cmd, security_level);

		if (m_nManagePort)
			 m_sHostname += QString("#%1").arg(m_nManagePort);

		PrlHandleLoginHelperJobPtr pJob( new PrlHandleLoginHelperJob( PrlHandleServerPtr( this ),
								user, passwd, prevSessionUuid, flags ) );

		m_pPveControl->DspCmdUserLogin( pJob, m_sHostname.toUtf8().constData(), security_level,
										(timeout ? timeout : PVE_CONNECTION_TIMEOUT),
										m_nManagePort, m_sHostname );

		return PRL_OBJECT_BY_HANDLE<PrlHandleJob>( pJob->GetHandle() );
	}
	else
	{
		if ( !user || !passwd )
			return CreateErrorHandle( PRL_ERR_INVALID_ARG, "", PJOC_SRV_LOGIN );

		QString job_uuid = m_pPveControl->DspCmdUserRelogin( user, passwd,prevSessionUuid, flags );

		return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
								PJOC_SRV_LOGIN )));
	}
}

PrlHandleJobPtr PrlHandleServerDisp::Logoff()
{
    QString job_uuid = m_pPveControl->DspCmdUserLogoff();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_LOGOFF )));
}

PrlHandleJobPtr PrlHandleServerDisp::SendAnswer(PRL_HANDLE hAnswer)
{
	PrlHandleVmEventPtr pVmEvent = PRL_OBJECT_BY_HANDLE<PrlHandleVmEvent>( hAnswer );
	QString job_uuid = m_pPveControl->DspCmdVmAnswer(pVmEvent->GetEventObject().toString().toUtf8().data());

	//Unregister question object from questions list
	m_QuestionsList.UnregisterQuestionObject(pVmEvent->GetEventObject().getInitRequestId());

	// Fix to prevent infinity wait on Answer Job.
	PrlHandleServerJob* pJob = new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_SRV_SEND_ANSWER);
	pJob->SetReturnCode( PRL_ERR_SUCCESS );

	return PrlHandleJobPtr((PrlHandleJob *)( pJob ));
}

/**
* Send problem report to server
* @param hProblemReport - handle of type PHT_PROBLEM_REPORT intended to be send by the server
* @param nFlags - flags associated with the operation
* @return pointer to generated job instance
*/
PrlHandleJobPtr PrlHandleServerDisp::SrvSendProblemReport( PRL_HANDLE hProblemReport,
    PRL_UINT32 nFlags )
{
    PrlHandleProblemReportBasePtr pPR =
        PRL_OBJECT_BY_HANDLE<PrlHandleProblemReportBase>(hProblemReport);

    QString job_uuid = m_pPveControl->DspCmdSendProblemReport(pPR->toString(), QString(), nFlags );
    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob(
                           PrlHandleServerPtr(this), job_uuid,
                           PJOC_SRV_SEND_PROBLEM_REPORT)));
}

PrlHandleJobPtr PrlHandleServerDisp::GetAllHostUsers()
{
    QString job_uuid = m_pPveControl->DspCmdAllHostUsers();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_ALL_HOST_USERS)));
}


namespace {
/**
 * Returns currently logged in user id. Under Win platform it's access token of current process.
 * Under Unix like platforms - UID.
 */
qlonglong PrlGetCurrentUserId()
{
#ifdef _WIN_
	return ((qlonglong)GetCurrentProcessId());
#else
	return (getuid());
#endif
}

}

PrlHandleJobPtr PrlHandleServerDisp::LoginLocal( PRL_CONST_STR sPrevSessionUuid, PRL_UINT32 port,
					PRL_SECURITY_LEVEL security_level, PRL_UINT32 flags )
{
	InitializeConnection();
	QMutexLocker _lock(&m_MembersMutex);
	m_sHostname = "127.0.0.1";
	m_nManagePort = port;
	m_nSecurityLevel = security_level;
	m_isConnectionLocal = true;
	m_isProxyConnection = false;
	m_sServerUuid = "";
	m_sHostOsVersion = "";
	m_sProductVersion = "";
	m_sProxyPeerHandle = "";
	m_nStartTime = 0;
	m_nStartTimeMonotonic = 0;
	PRL_UINT32 outPortNumber = 0;

	PrlHandleLoginLocalHelperJobPtr pJob(
		new PrlHandleLoginLocalHelperJob(
			PrlHandleServerPtr( this ),
			PrlGetCurrentUserId(),
			sPrevSessionUuid,
			PrlGetApiAppMode(),
			flags ) );

	m_pPveControl->DspCmdUserLoginLocal(pJob, security_level, port, outPortNumber);

	m_nManagePort = outPortNumber;

	return PRL_OBJECT_BY_HANDLE<PrlHandleJob>( pJob->GetHandle() );
}

PrlHandleJobPtr PrlHandleServerDisp::SetNonInteractiveSession(PRL_BOOL bNonInteractive, PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdSetNonInteractiveSession(bNonInteractive, nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_SET_NON_INTERACTIVE_SESSION )));
}

PRL_RESULT PrlHandleServerDisp::IsNonInteractiveSession(PRL_BOOL_PTR pbNonInteractive)
{
	QMutexLocker _lock(&m_MembersMutex);
	*pbNonInteractive = m_bNonInteractiveSession;
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleServerDisp::HasRestriction(PRL_LICENSE_RESTRICTION_KEY nRestrictionKey, PRL_BOOL_PTR pbHasRestriction)
{
	QMutexLocker _lock(&m_MembersMutex);

	const char* keyAsStr = PRL_LICENSE_RESTR_KEY_TO_STRING(nRestrictionKey);
	QString qsParamName = MAKE_EVT_PARAM_SESSION_RESTRICT_KEY(keyAsStr);
	*pbHasRestriction =	m_evtRestriction.getEventParameter(qsParamName) ? PRL_TRUE : PRL_FALSE;

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleServerDisp::GetRestrictionInfo(PRL_LICENSE_RESTRICTION_KEY nRestrictionKey, PRL_HANDLE_PTR phEvtParam)
{
	QMutexLocker _lock(&m_MembersMutex);

	const char* keyAsStr = PRL_LICENSE_RESTR_KEY_TO_STRING(nRestrictionKey);
	QString qsParamName = MAKE_EVT_PARAM_SESSION_RESTRICT_KEY(keyAsStr);
	CVmEventParameter* param = m_evtRestriction.getEventParameter(qsParamName);
	if ( ! param )
		return PRL_ERR_NO_DATA;

	CVmEventParameter* new_param = new CVmEventParameter();
	if ( ! new_param )
		return PRL_ERR_OUT_OF_MEMORY;
	new_param->fromString(param->toString());

	PrlHandleEventParam* pEvtParam = new PrlHandleEventParam(new_param);
	if ( ! pEvtParam )
		return PRL_ERR_OUT_OF_MEMORY;

	*phEvtParam = pEvtParam->GetHandle();

	return PRL_ERR_SUCCESS;
}

PrlHandleJobPtr PrlHandleServerDisp::EnableConfirmationMode(PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdSetSessionConfirmationMode(true, "", "", nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_SRV_SET_SESSION_CONFIRMATION_MODE )));
}

PrlHandleJobPtr PrlHandleServerDisp::DisableConfirmationMode(
	PRL_CONST_STR sUser,
	PRL_CONST_STR sPasswd,
	PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdSetSessionConfirmationMode(false, sUser, sPasswd, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_SRV_SET_SESSION_CONFIRMATION_MODE )));
}

PRL_RESULT PrlHandleServerDisp::IsConfirmationModeEnabled(PRL_BOOL_PTR pbConfirmationModeEnabled)
{
	QMutexLocker _lock(&m_MembersMutex);
	*pbConfirmationModeEnabled = m_bConfirmationModeEnabled;
	return PRL_ERR_SUCCESS;
}

PrlHandleJobPtr PrlHandleServerDisp::GetSrvConfig()
{
    QString job_uuid = m_pPveControl->DspCmdUserGetHostHwInfo();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_SRV_CONFIG)));
}

PrlHandleJobPtr PrlHandleServerDisp::GetCommonPrefs()
{
    QString job_uuid = m_pPveControl->DspCmdGetHostCommonInfo();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_COMMON_PREFS)));
}

PrlHandleJobPtr PrlHandleServerDisp::CommonPrefsBeginEdit()
{
    QString job_uuid = m_pPveControl->DspCmdHostCommonInfoBeginEdit();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_COMMON_PREFS_BEGIN_EDIT)));
}

PrlHandleJobPtr PrlHandleServerDisp::CommonPrefsCommit(PRL_HANDLE hDispCfg, PRL_UINT32 nFlags)
{
	PrlHandleDispConfigPtr pDispCfg = PRL_OBJECT_BY_HANDLE<PrlHandleDispConfig>(hDispCfg);
	QString strCommonPrefs = pDispCfg->GetDispConfig().toString();

  QString job_uuid = m_pPveControl->DspCmdHostCommonInfoCommit(strCommonPrefs.toUtf8().data(), nFlags);

  return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_COMMON_PREFS_COMMIT)));
}

PrlHandleJobPtr PrlHandleServerDisp::GetUserProfile()
{
    QString job_uuid = m_pPveControl->DspCmdUserGetProfile();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_USER_PROFILE)));
}

PrlHandleJobPtr PrlHandleServerDisp::GetUserInfoList()
{
    QString job_uuid = m_pPveControl->DspCmdUserInfoList();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_USER_INFO_LIST)));
}

PrlHandleJobPtr PrlHandleServerDisp::GetUserInfo(PRL_CONST_STR sUserId)
{
    QString job_uuid = m_pPveControl->DspCmdUserInfo(sUserId);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_USER_INFO)));
}

PrlHandleJobPtr PrlHandleServerDisp::ConfigureGenericPci(const QList<PrlHandleBasePtr>& dev_list, PRL_UINT32 nFlags)
{
	GenericPciDevices devices;
	for(int i = 0; i < dev_list.size(); i++)
	{
		CHwGenericPciDevice* pDev = new CHwGenericPciDevice();
		pDev->fromString(dev_list[i]->toString());
		devices.m_lstGenericPciDevice += pDev;
	}
	QString job_uuid = m_pPveControl->DspCmdConfigureGenericPci(QSTR2UTF8(devices.toString()), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_CONFIGURE_GENERIC_PCI)));
}

PrlHandleJobPtr PrlHandleServerDisp::UserProfileBeginEdit()
{
    QString job_uuid = m_pPveControl->DspCmdUserProfileBeginEdit();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_USER_PROFILE_BEGIN_EDIT)));
}

PrlHandleJobPtr PrlHandleServerDisp::UserProfileCommit(PRL_HANDLE hUserProfile)
{
	PrlHandleUserProfilePtr pUserProfile = PRL_OBJECT_BY_HANDLE<PrlHandleUserProfile>(hUserProfile);
	QString strUserProfile = pUserProfile->GetUserProfile().toString();
	QString job_uuid = m_pPveControl->DspCmdUserProfileCommit(strUserProfile.toUtf8().data());

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_USER_PROFILE_COMMIT)));
}

PRL_RESULT PrlHandleServerDisp::IsConnected(PRL_BOOL_PTR pbConnected)
{
	*pbConnected = (m_pPveControl->GetState() == IOSender::Connected);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleServerDisp::GetServerInfo( PRL_HANDLE_PTR phServerInfo )
{
	QMutexLocker _lock(&m_MembersMutex);
	CVmEvent _event;
	_event.addEventParameter(new CVmEventParameter(PVE::String, m_sServerUuid, EVT_PARAM_PRL_SERVER_INFO_SERVER_UUID));
	_event.addEventParameter(new CVmEventParameter(PVE::String, m_sHostOsVersion, EVT_PARAM_PRL_SERVER_INFO_OS_VERSION));
	_event.addEventParameter(new CVmEventParameter(PVE::String, m_sProductVersion, EVT_PARAM_PRL_SERVER_INFO_PRODUCT_VERSION));
	_event.addEventParameter(new CVmEventParameter(PVE::String, QString::number(quint32(m_nManagePort)), EVT_PARAM_PRL_SERVER_INFO_COMMANDS_PORT));
	_event.addEventParameter(new CVmEventParameter(PVE::String,
			QString::number(quint32(m_nServerAppExecuteMode)), EVT_PARAM_PRL_SERVER_INFO_APP_EXECUTE_MODE));
	_event.addEventParameter(new CVmEventParameter(PVE::String,
			QString::number(m_nStartTime), EVT_PARAM_PRL_SERVER_INFO_START_TIME));
	_event.addEventParameter(new CVmEventParameter(PVE::String,
			QString::number(m_nStartTimeMonotonic), EVT_PARAM_PRL_SERVER_INFO_START_TIME_MONOTONIC));

	PrlHandleServerInfo *pServerInfo = new PrlHandleServerInfo(_event.toString(), m_sHostname);
	*phServerInfo = pServerInfo->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PrlHandleJobPtr PrlHandleServerDisp::Shutdown(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdSMCShutdownDispatcher(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_SHUTDOWN)));
}

PrlHandleJobPtr PrlHandleServerDisp::FsGetDiskList()
{
    QString job_uuid = m_pPveControl->DspCmdFsGetDiskList();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_FS_GET_DISK_LIST)));
}

PrlHandleJobPtr PrlHandleServerDisp::FsGetDirEntries(PRL_CONST_STR path)
{
    QString job_uuid = m_pPveControl->DspCmdFsGetDirectoryEntries(path);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_FS_GET_DIR_ENTRIES)));
}


PrlHandleJobPtr PrlHandleServerDisp::FsCreateDir(PRL_CONST_STR path)
{
    QString job_uuid = m_pPveControl->DspCmdFsCreateDirectory(path);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_FS_CREATE_DIR)));
}

PrlHandleJobPtr PrlHandleServerDisp::FsRemoveEntry(PRL_CONST_STR path)
{
    QString job_uuid = m_pPveControl->DspCmdFsRemoveEntry(path);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_FS_REMOVE_ENTRY)));
}

PrlHandleJobPtr PrlHandleServerDisp::FsCanCreateFile(PRL_CONST_STR path)
{
	QString job_uuid = m_pPveControl->DspCmdFsCanCreateFile(path);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_FS_CAN_CREATE_FILE)));
}

PrlHandleJobPtr PrlHandleServerDisp::FsRenameEntry(PRL_CONST_STR oldPath, PRL_CONST_STR newPath)
{
    QString job_uuid = m_pPveControl->DspCmdFsRenameEntry(oldPath, newPath);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_FS_RENAME_ENTRY)));
}

PrlHandleJobPtr PrlHandleServerDisp::FsGenerateEntryName(
					PRL_CONST_STR sDirPath,
					PRL_CONST_STR sFilenamePrefix,
					PRL_CONST_STR sFilenameSuffix,
					PRL_CONST_STR sIndexDelimiter
					)
{
    QString job_uuid = m_pPveControl->DspCmdFsGenerateEntryName(
										sDirPath,
										sFilenamePrefix,
										sFilenameSuffix,
										sIndexDelimiter
										);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_FS_GENERATE_ENTRY_NAME)));
}

PrlHandleJobPtr PrlHandleServerDisp::DspCmdUserUpdateLicense( PRL_CONST_STR sKey,  PRL_CONST_STR sUser,  PRL_CONST_STR sCompany, PRL_UINT32 nFlags )
{
	QString job_uuid = m_pPveControl->DspCmdUserUpdateLicense(sKey, sUser, sCompany, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_UPDATE_LICENSE)));
}

PrlHandleJobPtr PrlHandleServerDisp::DspCmdUserGetLicenseInfo()
{
	QString job_uuid = m_pPveControl->DspCmdUserGetLicenseInfo();

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_LICENSE_INFO)));
}

PrlHandleJobPtr PrlHandleServerDisp::RefreshPlugins(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdRefreshPlugins(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_REFRESH_PLUGINS)));
}

PrlHandleJobPtr PrlHandleServerDisp::GetPluginsList(PRL_CONST_STR sClassId, PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdGetPluginsList(sClassId, nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_PLUGINS_LIST)));
}

/**
* Get problem Vm report from server
* @param none
* @return pointer to generated job instance
*/
PrlHandleJobPtr PrlHandleServerDisp::SrvGetProblemReport()
{
	QString job_uuid = m_pPveControl->DspCmdVmGetProblemReport(NULL);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_PROBLEM_REPORT)));
}

/**
* Get problem Vm report from server
* @param nFlags - flags associated with the problem report
* @return pointer to generated job instance
*/
PrlHandleJobPtr PrlHandleServerDisp::SrvGetPackedProblemReport( PRL_UINT32 nFlags )
{
    QString job_uuid = m_pPveControl->DspCmdVmGetPackedProblemReport(NULL, nFlags);
    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this),
                           job_uuid, PJOC_SRV_GET_PACKED_PROBLEM_REPORT)));
}

/**
* Attach to executed task on the server
* @param valid task_id
* @return pointer to generated job instance
*/
PrlHandleJobPtr PrlHandleServerDisp::SrvAttachToLostTask(PRL_CONST_STR sTaskId)
{
	QString job_uuid = m_pPveControl->DspCmdAttachToLostTask(sTaskId);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_ATTACH_TO_LOST_TASK)));
}

PRL_RESULT PrlHandleServerDisp::IsFeatureSupported(PRL_FEATURES_MATRIX nFeatureId, PRL_BOOL_PTR pbIsSupported)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbIsSupported = m_FeaturesMatrix.IsFeatureSupported( nFeatureId );
	return PRL_ERR_SUCCESS;
}

PrlHandleJobPtr PrlHandleServerDisp::GetFreeDiskSpace(PRL_CONST_STR sPath, PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdGetDiskFreeSpace(sPath, nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_DISK_FREE_SPACE)));
}

