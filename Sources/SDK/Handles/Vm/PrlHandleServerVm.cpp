/*
 * PrlHandleServerVm.cpp
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


#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include "PrlHandleServerJob.h"
#include "PrlHandleStringsList.h"
#include "PrlHandleVm.h"
#include "PrlRunProgramInGuestJob.h"
#include "PrlHandleServerVm.h"


void PrlHandleServerVm::RegisterVm(const QString &sVmUuid, PRL_HANDLE hVm)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_HandleVmUuidMap[sVmUuid] = hVm;
}

void PrlHandleServerVm::UnregisterVm(const QString &sVmUuid, PRL_HANDLE hVm)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	HandleVmUuidMap::iterator it = m_HandleVmUuidMap.find(sVmUuid);

	if ( it != m_HandleVmUuidMap.end() && it->second == hVm)
		m_HandleVmUuidMap.erase(it);
}

PrlHandleJobPtr PrlHandleServerVm::RegisterVm( PRL_CONST_STR  sVmDirPath, PRL_BOOL bNonInteractiveMode )
{
	return RegisterVmEx(sVmDirPath, NULL, bNonInteractiveMode ? PACF_NON_INTERACTIVE_MODE : 0);
}

PrlHandleJobPtr PrlHandleServerVm::RegisterVmEx( PRL_CONST_STR sVmDirPath, PRL_CONST_STR strVmUuid,
		PRL_UINT32 nFlags )
{
	QString job_uuid = m_pPveControl->DspCmdDirRegVm(sVmDirPath, strVmUuid, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_SRV_REGISTER_VM_EX)));
}

PrlHandleJobPtr PrlHandleServerVm::Register3rdPartyVm( PRL_CONST_STR sVmConfigPath, PRL_CONST_STR  sVmRootDirPath, PRL_UINT32 nFlags )
{
	if (UTF8_2QSTR(sVmConfigPath).isEmpty())//Do not accept empty paths
		return (CreateErrorHandle(PRL_ERR_INVALID_ARG,
                                  (PRL_CONST_STR)__FUNCTION__,
                                  PJOC_SRV_REGISTER_3RD_PARTY_VM));

	QString job_uuid = m_pPveControl->DspCmdDirReg3rdPartyVm(sVmConfigPath, sVmRootDirPath, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_SRV_REGISTER_3RD_PARTY_VM)));
}

PRL_RESULT PrlHandleServerVm::GetSupportedOses( PRL_HANDLE_PTR phOsesMatrix )
{
	QMutexLocker _lock(&m_MembersMutex);

	if (!m_pSupportedOsesMatrix.isValid())
		return (PRL_ERR_NO_DATA);

	PrlHandleGuestOsesMatrix *pOsesMatrix = new PrlHandleGuestOsesMatrix(m_pSupportedOsesMatrix);
	if (!pOsesMatrix)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phOsesMatrix = pOsesMatrix->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PrlHandleJobPtr PrlHandleServerVm::GetVmList(PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdDirGetVmList(nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_VM_LIST)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdGetDefaultVmConfig(PRL_CONST_GET_VM_CONFIG_PARAM_DATA_PTR pParam,
		PRL_UINT32 nFlags)
{
	CVmConfiguration conf;

	conf.setVmType(pParam->nVmType);
	if (pParam->sConfigSample)
		conf.getCtSettings()->setConfigSample(UTF8_2QSTR(pParam->sConfigSample));

	QString job_uuid = m_pPveControl->DspCmdGetDefaultVmConfig(conf.toString(), nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_DEFAULT_VM_CONFIG)));
}

PrlHandleJobPtr PrlHandleServerVm::StartSearchVms(PRL_HANDLE hStringsList)
{
	PrlHandleStringsListPtr pStringsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hStringsList);
	QString job_uuid = m_pPveControl->DspCmdStartSearchConfig(
		hStringsList != PRL_INVALID_HANDLE ? pStringsList->GetStringsList() : QStringList());

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_START_SEARCH_VMS)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdCreateVmBackup(
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sServerHostname,
		PRL_UINT32 nServerPort,
		PRL_CONST_STR sServerSessionUuid,
		PRL_CONST_STR strDescription,
		PRL_UINT32 nBackupFlags,
		PRL_UINT32 nReservedFlags,
		PRL_BOOL bForceOperation
)
{
	QString job_uuid = m_pPveControl->DspCmdCreateVmBackup(
			sVmUuid,
			sServerHostname,
			nServerPort,
			sServerSessionUuid,
			strDescription,
			nBackupFlags,
			nReservedFlags,
			bForceOperation);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob(
			PrlHandleServerPtr(this),
			job_uuid,
			PJOC_SRV_CREATE_VM_BACKUP)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdRestoreVmBackup(
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sBackupUuid,
		PRL_CONST_STR sServerHostname,
		PRL_UINT32 nServerPort,
		PRL_CONST_STR sServerSessionUuid,
		PRL_CONST_STR sTargetVmHomePath,
		PRL_CONST_STR sTargetVmName,
		PRL_UINT32 nRestoreFlags,
		PRL_UINT32 nReservedFlags
)
{
	QString job_uuid = m_pPveControl->DspCmdRestoreVmBackup(
			sVmUuid,
			sBackupUuid,
			sServerHostname,
			nServerPort,
			sServerSessionUuid,
			sTargetVmHomePath,
			sTargetVmName,
			nRestoreFlags,
			nReservedFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob(
			PrlHandleServerPtr(this),
			job_uuid,
			PJOC_SRV_RESTORE_VM_BACKUP)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdGetBackupTree(
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sServerHostname,
		PRL_UINT32 nServerPort,
		PRL_CONST_STR sServerSessionUuid,
		PRL_UINT32 nBackupFlags,
		PRL_UINT32 nReservedFlags,
		PRL_BOOL bForceOperation
)
{
	QString job_uuid = m_pPveControl->DspCmdGetBackupTree(
			sVmUuid,
			sServerHostname,
			nServerPort,
			sServerSessionUuid,
			nBackupFlags,
			nReservedFlags,
			bForceOperation);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob(
			PrlHandleServerPtr(this),
			job_uuid,
			PJOC_SRV_GET_BACKUP_TREE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdRemoveVmBackup(
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sBackupUuid,
		PRL_CONST_STR sServerHostname,
		PRL_UINT32 nServerPort,
		PRL_CONST_STR sServerSessionUuid,
		PRL_UINT32 nRestoreFlags,
		PRL_UINT32 nReservedFlags,
		PRL_BOOL bForceOperation
)
{
	QString job_uuid = m_pPveControl->DspCmdRemoveVmBackup(
			sVmUuid,
			sBackupUuid,
			sServerHostname,
			nServerPort,
			sServerSessionUuid,
			nRestoreFlags,
			nReservedFlags,
			bForceOperation);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob(
			PrlHandleServerPtr(this),
			job_uuid,
			PJOC_SRV_REMOVE_VM_BACKUP)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdCreateUnattendedCd(
	PRL_UINT32 nGuestType,
	PRL_CONST_STR sUserName,
	PRL_CONST_STR sPasswd,
	PRL_CONST_STR sFullUserName,
	PRL_CONST_STR sOsDistroPath,
	PRL_CONST_STR sOutImagePath
	)
{
	QString job_uuid = m_pPveControl->DspCmdCreateUnattendedCd(nGuestType, sUserName,
		 sPasswd, sFullUserName, sOsDistroPath, sOutImagePath);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_CREATE_UNATTENDED_CD)));
}

PrlHandleJobPtr PrlHandleServerVm::GetCtTemplateList(PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdGetCtTemplateList(nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_CT_TEMPLATE_LIST)));
}

PrlHandleJobPtr PrlHandleServerVm::RemoveCtTemplate(PRL_CONST_STR sName,
		PRL_CONST_STR sOsTmplName, PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdRemoveCtTemplate(sName, sOsTmplName, nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_REMOVE_CT_TEMPLATE)));
}

PrlHandleJobPtr PrlHandleServerVm::CopyCtTemplate(
			PRL_CONST_STR sName,
			PRL_CONST_STR sOsTmplName,
			PRL_CONST_STR sTargetServerHostname,
			PRL_UINT32 nTargetServerPort,
			PRL_CONST_STR sTargetServerSessionUuid,
			PRL_UINT32 nFlags,
			PRL_UINT32 nReservedFlags)
{
    QString job_uuid = m_pPveControl->DspCmdCopyCtTemplate(
			sName,
			sOsTmplName,
			sTargetServerHostname,
			nTargetServerPort,
			sTargetServerSessionUuid,
			nFlags,
			nReservedFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_COPY_CT_TEMPLATE)));
}

PrlHandleJobPtr PrlHandleServerVm::GetVmConfig(PRL_CONST_STR sSearchId, PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdGetVmConfigById(sSearchId, nFlags);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_GET_VM_CONFIG)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmStart(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmStart(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_START)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmStartEx(PRL_CONST_STR sVmUuid, PRL_UINT32 nStartMode, PRL_UINT32 nReserved)
{
	QString job_uuid = m_pPveControl->DspCmdVmStartEx(sVmUuid, nStartMode, nReserved);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_START_EX)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmRestartGuest(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmRestartGuest(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_RESTART)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmStop(PRL_CONST_STR sVmUuid, PRL_UINT32 nStopMode, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmStop(sVmUuid, nStopMode, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_STOP)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmPause(PRL_CONST_STR sVmUuid, PRL_BOOL bAcpi)
{
	QString job_uuid = m_pPveControl->DspCmdVmPause(sVmUuid, bAcpi);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_PAUSE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmReset(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmReset(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_RESET)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmSuspend(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmSuspend(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_SUSPEND)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGetSuspendedScreen(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmGetSuspendedScreen(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_GET_SUSPENDED_SCREEN)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmResume(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmResume(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_RESUME)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmDropSuspendedState(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmDropSuspendedState(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_DROP_SUSPENDED_STATE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmCreateSnapshot(PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sName,
		PRL_CONST_STR sDescription,
		PRL_CONST_STR sSnapshotUuid,
		PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmCreateSnapshot(sVmUuid,
			sName, sDescription, sSnapshotUuid, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
					PJOC_VM_CREATE_SNAPSHOT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGetSnapshotsTree(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmGetSnapshotsTree(sVmUuid, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_VM_GET_SNAPSHOTS_TREE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmLock(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmLock(sVmUuid, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_LOCK)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmUnlock(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmUnlock(sVmUuid, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_UNLOCK)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmSwitchToSnapshot(PRL_CONST_STR sVmUuid, PRL_CONST_STR sSnapshotUuid,
		PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmSwitchToSnapshot(sVmUuid, sSnapshotUuid, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_SWITCH_TO_SNAPSHOT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmDeleteSnapshot(PRL_CONST_STR sVmUuid, PRL_CONST_STR sSnapshotUuid, const PRL_BOOL bMerge)
{
	QString job_uuid = m_pPveControl->DspCmdVmDeleteSnapshot(sVmUuid, sSnapshotUuid, bMerge);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_DELETE_SNAPSHOT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmUpdateSnapshotData(PRL_CONST_STR sVmUuid, PRL_CONST_STR sSnapshotUuid,
															PRL_CONST_STR sNewName, PRL_CONST_STR sNewDescription)
{
	QString job_uuid = m_pPveControl->DspCmdVmUpdateSnapshotData(sVmUuid, sSnapshotUuid, sNewName, sNewDescription);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_VM_UPDATE_SNAPSHOT_DATA)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirVmClone(
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sNewVmName,
		PRL_CONST_STR sNewVmUuid,
		PRL_CONST_STR sNewVmConfigPath,
		PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdDirVmClone(sVmUuid, sNewVmName, sNewVmUuid, sNewVmConfigPath, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_CLONE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirVmMigrate(
		PRL_CONST_STR sVmUuid,
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
	QString job_uuid = m_pPveControl->DspCmdDirVmMigrate(sVmUuid, sTargetServerHostname,
														nTargetServerPort, sTargetServerSessionUuid,
														sTargetVmName, sTargetVmHomePath, nMigrationFlags,
														nReservedFlags, bForceOperation);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_MIGRATE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmMigrateCancel(
		PRL_CONST_STR sVmUuid
)
{
	QString job_uuid = m_pPveControl->DspCmdVmMigrateCancel(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_MIGRATE_CANCEL)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirVmDelete(PRL_CONST_STR sVmUuid,const QStringList & lstDevices)
{
	QString job_uuid = m_pPveControl->DspCmdDirVmDelete(sVmUuid,lstDevices);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_DELETE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGetProblemReport(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmGetProblemReport(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_GET_PROBLEM_REPORT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGetPackedProblemReport(PRL_CONST_STR sVmUuid,
                                                                PRL_UINT32 nFlags)
{
    QString job_uuid = m_pPveControl->DspCmdVmGetPackedProblemReport(sVmUuid, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_VM_GET_PACKED_PROBLEM_REPORT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdGetVmInfo(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdGetVmInfo(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_GET_STATE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdGetVmToolsInfo(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdGetVmToolsInfo(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_GET_TOOLS_STATE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGetConfig(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmGetConfig(sVmUuid, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_REFRESH_CONFIG)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmLoginInGuest(PRL_CONST_STR sVmUuid, PRL_CONST_STR sUserName,
														PRL_CONST_STR sUserPassword, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmLoginInGuest(sVmUuid, sUserName, sUserPassword, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_LOGIN_IN_GUEST)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmStartVncServer(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmStartVncServer(sVmUuid, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_START_VNC_SERVER)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmStopVncServer(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmStopVncServer(sVmUuid, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_STOP_VNC_SERVER)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGetStatistics(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmGetStatistics(sVmUuid, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_GET_STATISTICS)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmSubscribeToGuestStatistics(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmSubscribeToGuestStatistics(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_SUBSCRIBE_TO_GUEST_STATISTICS)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmUnsubscribeFromGuestStatistics(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmUnsubscribeFromGuestStatistics(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_UNSUBSCRIBE_FROM_GUEST_STATISTICS)));
}

PrlHandleJobPtr PrlHandleServerVm::RegVm(const PrlHandleVmPtr &pVm, PRL_CONST_STR sVmParentPath, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdDirVmCreate(pVm->GetVmConfig().toString().toUtf8().data(), sVmParentPath, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_REG)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirUnregVm(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdDirUnregVm(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_UNREG)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirRestoreVm(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdDirRestoreVm(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_RESTORE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirVmEditBegin(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdDirVmEditBegin(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_BEGIN_EDIT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirVmEditCommit(PRL_CONST_STR sVmConfiguration, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdDirVmEditCommit(sVmConfiguration, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_COMMIT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmCreateUnattendedFloppy(PRL_CONST_STR sVmUuid, PRL_GUEST_DISTRO_VERSION_ENUM nGuestDistroType, PRL_CONST_STR sKey, PRL_CONST_STR sUser, PRL_CONST_STR sCompany)
{
	QString job_uuid = m_pPveControl->DspCmdVmCreateUnattendedFloppy(sVmUuid, nGuestDistroType, sKey, sUser, sCompany);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_CREATE_UNATTENDED_FLOPPY)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmInitiateDevStateNotifications(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmInitiateDevStateNotifications(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_INITIATE_DEV_STATE_NOTIFICATIONS)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmUpdateSecurity(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmSecurity)
{
	QString job_uuid = m_pPveControl->DspCmdVmUpdateSecurity(sVmUuid, sVmSecurity);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_UPDATE_SECURITY)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmSectionValidateConfig(PRL_CONST_STR sVmConfiguration, PRL_VM_CONFIG_SECTIONS nSection)
{
	QString job_uuid = m_pPveControl->DspCmdVmSectionValidateConfig(sVmConfiguration, nSection);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_VALIDATE_CONFIG)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmSetProtection( PRL_CONST_STR sVmUuid,
														PRL_CONST_STR sProtectionPassword,
														PRL_UINT32 nReserved )
{
	return create_job(m_pPveControl->DspCmdVmSetProtection(sVmUuid, sProtectionPassword, nReserved),
					PJOC_VM_SET_PROTECTION);
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmRemoveProtection( PRL_CONST_STR sVmUuid,
														   PRL_CONST_STR sProtectionPassword,
														   PRL_UINT32 nReserved )
{
	return create_job(m_pPveControl->DspCmdVmRemoveProtection(sVmUuid, sProtectionPassword, nReserved),
					PJOC_VM_REMOVE_PROTECTION);
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmInstallUtility(PRL_CONST_STR sVmUuid, PRL_CONST_STR strId)
{
	QString job_uuid = m_pPveControl->DspCmdVmInstallUtility(sVmUuid, strId);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_INSTALL_UTILITY)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmInstallTools(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmInstallTools(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_INSTALL_TOOLS)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmCompact(PRL_CONST_STR sVmUuid, PRL_UINT32 uMask, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmCompact(sVmUuid, uMask, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_COMPACT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmCancelCompact(PRL_CONST_STR sVmUuid)
{

       QString job_uuid = m_pPveControl->DspCmdVmCancelCompact(sVmUuid);

       return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_CANCEL_COMPACT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmConvertDisks(PRL_CONST_STR sVmUuid, PRL_UINT32 uMask, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmConvertDisks(sVmUuid, uMask, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_CONVERT_DISKS)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmChangeSid(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
       QString job_uuid = m_pPveControl->DspCmdVmChangeSid(sVmUuid, nFlags);

       return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_CHANGE_SID)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmResetUptime(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
       QString job_uuid = m_pPveControl->DspCmdVmResetUptime(sVmUuid, nFlags);

       return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_RESET_UPTIME)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmAuthWithGuestSecurityDb(PRL_CONST_STR sVmUuid, PRL_CONST_STR sUserName,
														PRL_CONST_STR sUserPassword, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmAuthWithGuestSecurityDb(sVmUuid, sUserName, sUserPassword, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_AUTH_WITH_GUEST_SECURITY_DB)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmMount(PRL_CONST_STR sVmUuid, PRL_CONST_STR sMntPath, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmMount(sVmUuid, sMntPath, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
					PJOC_VM_MOUNT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmUmount(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmUmount(sVmUuid, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
					PJOC_VM_UMOUNT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirVmMove(PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sNewHomePath, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdDirVmMove(sVmUuid, sNewHomePath, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_MOVE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGuestLogout(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmGuestUuid, PRL_UINT32 nFlags)
{
    return create_job(m_pPveControl->DspCmdVmGuestLogout(sVmUuid, sVmGuestUuid, nFlags), PJOC_VM_GUEST_LOGOUT);
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGuestRunProgram(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmGuestUuid, PRL_CONST_STR sProgramName,
										const QStringList &lstArgs, const QStringList &lstEnvs, PRL_UINT32 nFlags,
										PRL_FILE_DESC nStdin, PRL_FILE_DESC nStdout, PRL_FILE_DESC nStderr)
{
    return PrlHandleJobPtr(new PrlRunProgramInGuestJob(PrlHandleServerPtr(this),
									m_pPveControl->DspCmdVmGuestRunProgram(sVmUuid, sVmGuestUuid, sProgramName, lstArgs, lstEnvs, nFlags),
									nStdin, nStdout, nStderr
								));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGuestGetNetworkSettings(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmGuestUuid, PRL_UINT32 nFlags)
{
    return create_job(m_pPveControl->DspCmdVmGuestGetNetworkSettings(sVmUuid, sVmGuestUuid, nFlags), PJOC_VM_GUEST_GET_NETWORK_SETTINGS);
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmGuestSetUserPasswd(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmGuestUuid, PRL_CONST_STR sUserLoginName,
								const char *sUserPasswd, PRL_UINT32 nFlags)
{
    return create_job(m_pPveControl->DspCmdVmGuestSetUserPasswd(sVmUuid, sVmGuestUuid, sUserLoginName, sUserPasswd, nFlags),\
					PJOC_VM_GUEST_SET_USER_PASSWD);
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmDevConnect(PRL_CONST_STR sVmUuid, PRL_DEVICE_TYPE nDevType, PRL_INT32 nDevIndex,
																		PRL_CONST_STR sDeviceConfig)
{
	QString job_uuid = m_pPveControl->DspCmdVmDevConnect(sVmUuid, nDevType, nDevIndex, sDeviceConfig);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_DEV_CONNECT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmDevDisconnect(PRL_CONST_STR sVmUuid, PRL_DEVICE_TYPE nDevType, PRL_INT32 nDevIndex,
																		 PRL_CONST_STR sDeviceConfig)
{
	QString job_uuid = m_pPveControl->DspCmdVmDevDisconnect(sVmUuid, nDevType, nDevIndex, sDeviceConfig);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_DEV_DISCONNECT)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirCreateImage(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmDeviceConfiguration, PRL_BOOL bRecreateIsAllowed, PRL_BOOL bNonInteractiveMode)
{
	QString job_uuid = m_pPveControl->DspCmdDirCreateImage(sVmUuid, sVmDeviceConfiguration, bool(bRecreateIsAllowed), bool(bNonInteractiveMode));

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_DEV_CREATE_IMAGE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdDirCopyImage(PRL_CONST_STR sVmUuid,
													PRL_CONST_STR sVmDeviceConfiguration,
													PRL_CONST_STR sNewImageName,
													PRL_CONST_STR sTargetPath,
													PRL_UINT32 nFlags )
{
	QString job_uuid = m_pPveControl->DspCmdDirCopyImage(
		sVmUuid, sVmDeviceConfiguration, sNewImageName, sTargetPath, nFlags);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_DEV_COPY_IMAGE)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmResizeDiskImage(PRL_CONST_STR sVmUuid, const QString &sDiskImage,
								PRL_UINT32 nNewSize, PRL_UINT32 nFlags)
{
    return create_job(m_pPveControl->DspCmdVmResizeDiskImage(sVmUuid, sDiskImage, nNewSize, nFlags),
					PJOC_VM_RESIZE_DISK_IMAGE);
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmCaptureScreen(PRL_CONST_STR sVmUuid, PRL_UINT32 nWidth,
								PRL_UINT32 nHeight, PRL_UINT32 nFlags)
{
    return create_job(m_pPveControl->DspCmdVmCaptureScreen(sVmUuid, nWidth, nHeight, nFlags),
					PJOC_VM_DEV_DISPLAY_CAPTURE_SCREEN);
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdVmCommitEncryption(PRL_CONST_STR sVmConfiguration,
	PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdVmCommitEncryption(sVmConfiguration, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob(PrlHandleServerPtr(this),
		job_uuid, PJOC_VM_COMMIT_ENCRYPTION)));
}

PrlHandleJobPtr PrlHandleServerVm::DspCmdCtReinstall(PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sOs, PRL_UINT32 nFlags)
{
	QString job_uuid = m_pPveControl->DspCmdCtReinstall(sVmUuid, sOs, nFlags);
	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob(PrlHandleServerPtr(this),
		job_uuid, PJOC_CT_REINSTALL)));
}
