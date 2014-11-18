/*
 * PrlHandleVmSrv.cpp
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


#include <QHostInfo>

#include "PrlHandleVmSrv.h"
#include "PrlHandleStringsList.h"
#include "PrlHandleVmDevice.h"


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


/*****************************************************************************/
#define CHECK_SERVER if (!m_pServer) return (PrlHandleJobPtr((PrlHandleJob *)PRL_INVALID_HANDLE));
#define CHECK_IDENTIFICATION if (!m_VmConfig.getVmIdentification()) return (PrlHandleJobPtr((PrlHandleJob *)PRL_INVALID_HANDLE));
#define GET_VM_UUID	m_VmConfig.getVmIdentification()->getVmUuid().toUtf8().data()


PrlHandleJobPtr PrlHandleVmSrv::SendProblemReport( PRL_HANDLE hProblemReport, PRL_UINT32 nFlags )
{
    SYNCHRO_INTERNAL_DATA_ACCESS
    CHECK_SERVER
    return m_pServer->DspCmdSendProblemReport(GET_VM_UUID, hProblemReport, nFlags);
}

PrlHandleJobPtr PrlHandleVmSrv::InternalCmd(PRL_CONST_STR sCmd, PRL_HANDLE hArgsList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	QStringList lstArgs;
	PrlHandleStringsListPtr pArgsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hArgsList);
	if (pArgsList.getHandle())
		lstArgs = pArgsList->GetStringsList();

	return (m_pServer->DspCmdVmInternal(GET_VM_UUID, sCmd, lstArgs));
}

PrlHandleJobPtr PrlHandleVmSrv::RunCompressor()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServer->DspCmdVmRunCompressor(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::CancelCompressor()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServer->DspCmdVmCancelCompressor(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::GenerateVmDevFilename(
					PRL_CONST_STR sFilenamePrefix,
					PRL_CONST_STR sFilenameSuffix,
					PRL_CONST_STR sIndexDelimiter
					)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	if (m_VmConfig.getVmIdentification()->getHomePath().isEmpty())
		return (CreateErrorHandle(PRL_ERR_NO_DATA, (PRL_CONST_STR)__FUNCTION__, PJOC_VM_GENERATE_VM_DEV_FILENAME));
	return (m_pServerDisp->FsGenerateEntryName(
				QFileInfo(m_VmConfig.getVmIdentification()->getHomePath()).path().toUtf8().constData(),
				sFilenamePrefix,
				sFilenameSuffix,
				sIndexDelimiter));
}

PrlHandleJobPtr PrlHandleVmSrv::UpdateToolsSection(PRL_CONST_STR sToolsSection)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServer->DspCmdVmUpdateToolsSection(GET_VM_UUID, sToolsSection ));
}

/**
 * Perfomance statictics methods
 *
 */
PrlHandleJobPtr PrlHandleVmSrv::SubscribePerfStats(PRL_CONST_STR filter)
{
    CHECK_SERVER ;
    return m_pServerStat->DspCmdSubscribePerfStats(filter, GET_VM_UUID) ;
}

PrlHandleJobPtr PrlHandleVmSrv::UnsubscribePerfStats()
{
    CHECK_SERVER ;
    return m_pServerStat->DspCmdUnsubscribePerfStats(GET_VM_UUID) ;
}

PrlHandleJobPtr PrlHandleVmSrv::GetPerfStats(PRL_CONST_STR filter)
{
    CHECK_SERVER ;
    return m_pServerStat->DspCmdGetPerfStats(filter, GET_VM_UUID) ;
}

PrlHandleJobPtr PrlHandleVmSrv::Delete(const QStringList & lstDevices)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	return (m_pServerVm->DspCmdDirVmDelete(GET_VM_UUID,lstDevices));
}

PrlHandleJobPtr PrlHandleVmSrv::GetProblemReport()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	return (m_pServerVm->DspCmdVmGetProblemReport(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::GetPackedProblemReport( PRL_UINT32 nFlags )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	return (m_pServerVm->DspCmdVmGetPackedProblemReport(GET_VM_UUID,nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Start()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	m_bStartInProgress = true;

	return (m_pServerVm->DspCmdVmStart(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::StartVncServer(PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmStartVncServer(GET_VM_UUID, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::StopVncServer(PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmStopVncServer(GET_VM_UUID, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Lock(PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmLock(GET_VM_UUID, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Unlock(PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmUnlock(GET_VM_UUID, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::StartEx(PRL_UINT32 nStartMode, PRL_UINT32 nReserved)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	m_bStartInProgress = true;

	return (m_pServerVm->DspCmdVmStartEx(GET_VM_UUID, nStartMode, nReserved));
}

PrlHandleJobPtr PrlHandleVmSrv::LoginInGuest(PRL_CONST_STR sUserName, PRL_CONST_STR sUserPassword, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmLoginInGuest(GET_VM_UUID, sUserName, sUserPassword, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::AuthWithGuestSecurityDb(PRL_CONST_STR sUserName, PRL_CONST_STR sUserPassword, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmAuthWithGuestSecurityDb(GET_VM_UUID, sUserName, sUserPassword, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::RestartGuest()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmRestartGuest(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::SubscribeToGuestStatistics()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmSubscribeToGuestStatistics(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::UnsubscribeFromGuestStatistics()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmUnsubscribeFromGuestStatistics(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::Stop(PRL_UINT32 nStopMode, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmStop(GET_VM_UUID, nStopMode, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Pause(PRL_BOOL bAcpi)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmPause(GET_VM_UUID, bAcpi));
}

PrlHandleJobPtr PrlHandleVmSrv::Compact(PRL_UINT32 uMask, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmCompact(GET_VM_UUID, uMask, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::CancelCompact()
{
       CHECK_SERVER
       CHECK_IDENTIFICATION
       return (m_pServerVm->DspCmdVmCancelCompact(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::ConvertDisks(PRL_UINT32 uMask, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmConvertDisks(GET_VM_UUID, uMask, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::ChangeMachineSid(PRL_UINT32 nFlags)
{
       CHECK_SERVER
       CHECK_IDENTIFICATION
       return (m_pServerVm->DspCmdVmChangeSid(GET_VM_UUID, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::ResetUptime(PRL_UINT32 nFlags)
{
       CHECK_SERVER
       CHECK_IDENTIFICATION
       return (m_pServerVm->DspCmdVmResetUptime(GET_VM_UUID, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Suspend()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmSuspend(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::GetSuspendedScreen()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmGetSuspendedScreen(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::DropSuspendedState()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmDropSuspendedState(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::Reset()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmReset(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::InstallUtility(PRL_CONST_STR strId)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmInstallUtility(GET_VM_UUID, strId));
}

PrlHandleJobPtr PrlHandleVmSrv::InstallTools()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmInstallTools(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::CreateSnapshot(PRL_CONST_STR sName, PRL_CONST_STR sDescription)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	QString strSnapshotUuid = Uuid::createUuid().toString();
	return (m_pServerVm->DspCmdVmCreateSnapshot(GET_VM_UUID, sName, sDescription, strSnapshotUuid.toUtf8().data()));
}

PrlHandleJobPtr PrlHandleVmSrv::SwitchToSnapshot(PRL_CONST_STR sSnapshotUuid, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmSwitchToSnapshot(GET_VM_UUID, sSnapshotUuid, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::DeleteSnapshot(PRL_CONST_STR sSnapshotUuid, const PRL_BOOL bChild)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmDeleteSnapshot(GET_VM_UUID, sSnapshotUuid, bChild));
}

PrlHandleJobPtr PrlHandleVmSrv::GetSnapshotsTree(PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmGetSnapshotsTree(GET_VM_UUID, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::UpdateSnapshotData(PRL_CONST_STR sSnapshotUuid,
												PRL_CONST_STR sNewName,
												PRL_CONST_STR sNewDescription)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmUpdateSnapshotData(GET_VM_UUID, sSnapshotUuid, sNewName, sNewDescription));
}

PrlHandleJobPtr PrlHandleVmSrv::BeginEdit()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdDirVmEditBegin(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::Commit( PRL_UINT32 nFlags )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	return (m_pServerVm->DspCmdDirVmEditCommit( m_VmConfig.toString().toUtf8().data(), nFlags ));
}

PrlHandleJobPtr PrlHandleVmSrv::CreateImage(const PrlHandleVmDevicePtr &pVmDevice, PRL_BOOL bRecreateIsAllowed, PRL_BOOL bNonInteractiveMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return ( m_pServerVm->DspCmdDirCreateImage(GET_VM_UUID, pVmDevice->toString().toUtf8().data(), bRecreateIsAllowed, bNonInteractiveMode) );
}

PrlHandleJobPtr PrlHandleVmSrv::CopyImage( const PrlHandleVmDevicePtr &pVmDevice,
										PRL_CONST_STR sNewImageName,
										PRL_CONST_STR sTargetPath,
										PRL_UINT32 nFlags )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return ( m_pServerVm->DspCmdDirCopyImage(
		GET_VM_UUID, QSTR2UTF8(pVmDevice->toString()), sNewImageName, sTargetPath, nFlags) );
}

PrlHandleJobPtr PrlHandleVmSrv::ResizeDiskImage(const QString &diskImage, PRL_UINT32 nNewSize, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return ( m_pServerVm->DspCmdVmResizeDiskImage(GET_VM_UUID, diskImage, nNewSize, nFlags) );
}

PrlHandleJobPtr PrlHandleVmSrv::RefreshConfig()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmGetConfig(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::GetStatistics(PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmGetStatistics(GET_VM_UUID, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Unreg()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdDirUnregVm(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::Restore()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdDirRestoreVm(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::Clone(PRL_CONST_STR new_vm_name, PRL_CONST_STR new_vm_config_path, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdDirVmClone(m_VmConfig.getVmIdentification()->getVmUuid().toUtf8().data(),
				new_vm_name, new_vm_config_path, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Migrate(
	PrlHandleServerPtr pTargetServer,
	PRL_CONST_STR sTargetVmName,
	PRL_CONST_STR sTargetVmHomePath,
	PRL_UINT32 nMigrationFlags,
	PRL_UINT32 nReservedFlags,
	PRL_BOOL bForceOperation)
{
	return (Migrate(pTargetServer->GetRemoteHost().toUtf8().constData(),
					pTargetServer->GetManagePort(),
					pTargetServer->GetSessionUuid().toUtf8().constData(),
					sTargetVmName, sTargetVmHomePath,
					nMigrationFlags, nReservedFlags, bForceOperation));
}

PrlHandleJobPtr PrlHandleVmSrv::Migrate(
	PRL_CONST_STR sTargetServerHostname,
	PRL_UINT32 nTargetServerPort,
	PRL_CONST_STR sTargetServerSessionUuid,
	PRL_CONST_STR sTargetVmName,
	PRL_CONST_STR sTargetVmHomePath,
	PRL_UINT32 nMigrationFlags,
	PRL_UINT32 nReservedFlags,
	PRL_BOOL bForceOperation
)
{
	QString sTargetHostname;
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	sTargetHostname = sTargetServerHostname;
	if ((sTargetHostname == QString("localhost")) || (sTargetHostname == QString("127.0.0.1"))) {
		/* migration from remote host to localhost */
		sTargetHostname = QHostInfo::localHostName();
	}
	return (m_pServerVm->DspCmdDirVmMigrate(GET_VM_UUID, sTargetHostname.toUtf8().constData(), nTargetServerPort,
											sTargetServerSessionUuid, sTargetVmName, sTargetVmHomePath,
											nMigrationFlags, nReservedFlags, bForceOperation));
}

PrlHandleJobPtr PrlHandleVmSrv::MigrateCancel()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmMigrateCancel(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::GetState()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdGetVmInfo(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::GetToolsState()
{
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdGetVmToolsInfo(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::CreateUnattendedFloppy( PRL_GUEST_DISTRO_VERSION_ENUM nGuestDistroType, PRL_CONST_STR sKey, PRL_CONST_STR sUser, PRL_CONST_STR sCompany )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmCreateUnattendedFloppy(GET_VM_UUID, nGuestDistroType, sKey, sUser, sCompany));
}

PrlHandleJobPtr PrlHandleVmSrv::InitiateDevStateNotifications()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmInitiateDevStateNotifications(GET_VM_UUID));
}

PrlHandleJobPtr PrlHandleVmSrv::UpdateSecurity(PrlHandleAccessRightsPtr pAccessRights)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmUpdateSecurity(GET_VM_UUID, pAccessRights->toString().toUtf8().constData()));
}

PrlHandleJobPtr PrlHandleVmSrv::ValidateConfig(PRL_VM_CONFIG_SECTIONS nSection)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdVmSectionValidateConfig(m_VmConfig.toString().toUtf8().constData(), nSection));
}

PrlHandleJobPtr PrlHandleVmSrv::VmGuestLogout(const QString &sVmGuestUuid, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServerVm->DspCmdVmGuestLogout(GET_VM_UUID, sVmGuestUuid.toUtf8().constData(), nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::VmGuestGetNetworkSettings(const QString &sVmGuestUuid, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServerVm->DspCmdVmGuestGetNetworkSettings(GET_VM_UUID, sVmGuestUuid.toUtf8().constData(), nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::VmGuestRunProgram(const QString &sVmGuestUuid, PRL_CONST_STR sProgramName,
					const QStringList &lstArgs, const QStringList &lstEnvs, PRL_UINT32 nFlags,
					PRL_FILE_DESC nStdin, PRL_FILE_DESC nStdout, PRL_FILE_DESC nStderr)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	if (PRL_INVALID_FILE_DESCRIPTOR != nStdin || PRL_INVALID_FILE_DESCRIPTOR != nStdout ||
		PRL_INVALID_FILE_DESCRIPTOR != nStderr)
	{
		if (!GetIOChannel() || !GetIOChannel()->isValid())
			return (CreateErrorHandle(PRL_ERR_UNINITIALIZED, (PRL_CONST_STR)__FUNCTION__, PJOC_VM_GUEST_RUN_PROGRAM));
	}

	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	QString sSessionUuid = sVmGuestUuid;
	if (m_VmConfig.getVmType() == PVT_CT && m_ioExecChannel.isValid())
		sSessionUuid = m_ioExecChannel->getHandle();
	return (m_pServerVm->DspCmdVmGuestRunProgram(GET_VM_UUID, sSessionUuid.toUtf8().constData(), sProgramName,
												lstArgs, lstEnvs, nFlags, nStdin, nStdout, nStderr));
}

PrlHandleJobPtr PrlHandleVmSrv::VmGuestSetUserPasswd(const QString &sVmGuestUuid, PRL_CONST_STR sUserLoginName,
					PRL_CONST_STR sUserPasswd, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServerVm->DspCmdVmGuestSetUserPasswd(GET_VM_UUID, sVmGuestUuid.toUtf8().constData(), sUserLoginName,
												sUserPasswd, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::StoreValueByKey( PRL_CONST_STR sKey, PRL_CONST_STR sValue, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServer->DspCmdVmStorageSetValue(GET_VM_UUID, sKey, sValue, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Authorise( PRL_CONST_STR sPassword, PRL_UINT32 nFlags )
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServerVm->DspCmdVmAuthorise(GET_VM_UUID, sPassword, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::ChangePassword( PRL_CONST_STR sOldPassword, PRL_CONST_STR sNewPassword, PRL_UINT32 nFlags )
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServerVm->DspCmdVmChangePassword(GET_VM_UUID, sOldPassword, sNewPassword, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Encrypt( PRL_CONST_STR sPassword, PRL_CONST_STR sCipherPluginUuid, PRL_UINT32 nFlags )
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServerVm->DspCmdVmEncrypt(GET_VM_UUID, sPassword, sCipherPluginUuid, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Decrypt( PRL_CONST_STR sPassword, PRL_UINT32 nFlags )
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServerVm->DspCmdVmDecrypt(GET_VM_UUID, sPassword, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::SetProtection( PRL_CONST_STR sProtectionPassword, PRL_UINT32 nReserved )
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServerVm->DspCmdVmSetProtection(GET_VM_UUID, sProtectionPassword, nReserved));
}

PrlHandleJobPtr PrlHandleVmSrv::RemoveProtection( PRL_CONST_STR sProtectionPassword, PRL_UINT32 nReserved )
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CHECK_SERVER;
	CHECK_IDENTIFICATION;
	return (m_pServerVm->DspCmdVmRemoveProtection(GET_VM_UUID, sProtectionPassword, nReserved));
}

PrlHandleJobPtr PrlHandleVmSrv::Mount(PRL_CONST_STR sMntPath, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmMount(GET_VM_UUID, sMntPath, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Umount(PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION

	return (m_pServerVm->DspCmdVmUmount(GET_VM_UUID, nFlags));
}

PrlHandleJobPtr PrlHandleVmSrv::Move(PRL_CONST_STR sNewHomePath, PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_SERVER
	CHECK_IDENTIFICATION
	return (m_pServerVm->DspCmdDirVmMove(m_VmConfig.getVmIdentification()->getVmUuid().toUtf8().data(),
				sNewHomePath, nFlags));
}
