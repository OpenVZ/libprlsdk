/*
 * CProtoSerializer.cpp: Implementation of project protocol commands
 * serializer.
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


#include "CProtoSerializer.h"

namespace Parallels
{

CProtoCommandPtr CProtoSerializer::CreateDspCmdUserLoginCommand(
		const QString &sUsername,
		const QString &sPassword,
		const QString &sPrevSessionUuid,
                quint32 nFlags
	){
	return CProtoCommandPtr(new CProtoCommandDspCmdUserLogin(sUsername, sPassword, sPrevSessionUuid, nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateDspCmdUserLoginLocalCommand(
		quint32 nUserId,
		PRL_APPLICATION_MODE appMode,
		quint64 nProcessId,
		quint32 flags
)
{
	return CProtoCommandPtr(new CProtoCommandDspCmdUserLoginLocal(nUserId, appMode, nProcessId, flags));
}

CProtoCommandPtr CProtoSerializer::CreateProtoCommandWithoutParams(
		PVE::IDispatcherCommands nCmdIdentifier,
		bool bForceQuestionsSign,
		quint32 mFlags
	)
{
	return CProtoCommandPtr(new CProtoCommandWithoutParams(nCmdIdentifier, bForceQuestionsSign, mFlags));
}

CProtoCommandPtr CProtoSerializer::CreateProtoCommandWithOneStrParam(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sFirstParam,
		bool bForceQuestionsSign,
		quint32 nFlags
)
{
	return (CProtoCommandPtr(new CProtoCommandWithOneStrParam(nCmdIdentifier,
															  sFirstParam,
															  bForceQuestionsSign,
															  nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateProtoCommandWithTwoStrParams(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sFirstParam,
		const QString &sSecondParam,
		bool bForceQuestionsSign,
		quint32 nFlags
)
{
	return (CProtoCommandPtr(new CProtoCommandWithTwoStrParams(nCmdIdentifier,
															  sFirstParam,
															  sSecondParam,
															  bForceQuestionsSign,
															  nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateVmCreateProtoCommand(
		const QString &sVmConfig,
		const QString &sVmHomePath,
		quint32 nFlags
	)
{
	return (CProtoCommandPtr(new CProtoVmCreateCommand(sVmConfig, sVmHomePath, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateVmDeleteProtoCommand(
	const QString &sVmUuid,
	const QStringList& lstDevices
	)
{
	return (CProtoCommandPtr(new CProtoVmDeleteCommand(sVmUuid, lstDevices)));
}

CProtoCommandPtr CProtoSerializer::CreateFsRenameEntryProtoCommand(
	const QString &sOldEntryName,
	const QString &sNewEntryName
)
{
	return (CProtoCommandPtr(new CProtoFsRenameEntryCommand(sOldEntryName, sNewEntryName)));
}

CProtoCommandPtr CProtoSerializer::CreateSerialNumProtoCommand(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sUserName,
	const QString &sCompanyName,
	const QString &sSerialNumber,
	quint32 nFlags
)
{
	return (CProtoCommandPtr(new CProtoSerialNumCommand(nCmdIdentifier, sUserName, sCompanyName, sSerialNumber, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateVmCreateUnattendedProtoCommand(
	const QString &sVmUuid,
	const QString &sUserName,
	const QString &sCompanyName,
	const QString &sSerialNumber,
	PRL_GUEST_DISTRO_VERSION_ENUM nGuestType
)
{
	return (CProtoCommandPtr(new CProtoCreateUnattendedFloppyCommand(sVmUuid, sUserName, sCompanyName,
                                                                         sSerialNumber, nGuestType)));
}

CProtoCommandPtr CProtoSerializer::CreateSrvCreateUnattendedCdProtoCommand(
	PRL_UINT32 nGuestType,
	const QString &sUserName,
	const QString &sPasswd,
	const QString &sFullUserName,
	const QString &sOsDistroPath,
	const QString &sOutImagePath
)
{
	return (CProtoCommandPtr(new CProtoCreateUnattendedCdCommand(nGuestType, sUserName, sPasswd,
		sFullUserName, sOsDistroPath, sOutImagePath)));
}

CProtoCommandPtr CProtoSerializer::CreateVmCloneProtoCommand(
	const QString &sVmUuid,
	const QString &sVmName,
	const QString &sNewVmUuid,
	const QString &sVmHomePath,
	PRL_UINT32 nFlags
)
{
	return (CProtoCommandPtr(new CProtoVmCloneCommand(sVmUuid, sVmName,
					sNewVmUuid, sVmHomePath, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateImageCreateProtoCommand(
	const QString &sVmUuid,
	const QString &sImageConfig,
	bool bRecreateIsAllowed,
	bool bForceQuestionsSign
)
{
	return (CProtoCommandPtr(new CProtoCreateImageCommand(sVmUuid, sImageConfig,
                                                              bRecreateIsAllowed, bForceQuestionsSign)));
}

CProtoCommandPtr CProtoSerializer::CopyImageCreateProtoCommand(
	const QString &sVmUuid,
	const QString &sImageConfig,
	const QString &sNewImageName,
	const QString &sTargetPath,
	quint32 nFlags
)
{
	return (CProtoCommandPtr(
		new CProtoCopyImageCommand(sVmUuid, sImageConfig, sNewImageName, sTargetPath, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateProtoBasicVmCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		quint32 nFlags
	)
{
	return (CProtoCommandPtr(new CProtoBasicVmCommand(nCmdIdentifier, sVmUuid, false, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateProtoVmCommandWithAcpiSign(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		bool bUseAcpi
	)
{
	return (CProtoCommandPtr(new CProtoVmCommandWithAcpiSign(nCmdIdentifier, sVmUuid, bUseAcpi)));
}

CProtoCommandPtr CProtoSerializer::CreateProtoVmCommandStop(
		const QString &sVmUuid,
		PRL_UINT32 nStopMode,
		PRL_UINT32 nFlags
	)
{
	return (CProtoCommandPtr(new CProtoVmCommandStop(sVmUuid, nStopMode, nFlags)));
}


CProtoCommandPtr CProtoSerializer::CreateProtoVmCommandWithOneStrParam(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	const QString &sParam,
	quint32 nFlags
	)
{
	return (CProtoCommandPtr(new CProtoVmCommandWithOneStrParam(nCmdIdentifier, sVmUuid, sParam, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateVmDeviceProtoCommand(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	PRL_DEVICE_TYPE nDevType,
	quint32 nDevIndex,
	const QString &sDevConfig,
	PRL_UINT32 nFlags
)
{
	return (CProtoCommandPtr(new CProtoVmDeviceCommand(nCmdIdentifier, sVmUuid, nDevType, nDevIndex, sDevConfig, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateStartSearchConfigProtoCommand(
	const QStringList &lstSearchDirs
)
{
	return (CProtoCommandPtr(new CProtoStartSearchConfigCommand(lstSearchDirs)));
}

CProtoCommandPtr CProtoSerializer::CreateConvertHddProtoCommand(
	const QStringList &lstHddPathsToConvert
	)
{
	return (CProtoCommandPtr(new CProtoStartConvertHardDisksCommand(lstHddPathsToConvert)));
}

CProtoCommandPtr CProtoSerializer::CreateUpdateUsbAssocListProtoCommand(
	const QStringList &lstUsbList,
	quint32 nListVersion,
	quint32 nFlags
	)
{
	return CProtoCommandPtr(new CProtoUpdateUsbAssocListCommand(lstUsbList,
																nListVersion,
																nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateDspWsResponseCommand(
		PVE::IDispatcherCommands nOpCode,
		PRL_RESULT nRetCode
	)
{
	return CProtoCommandPtr(new CProtoCommandDspWsResponse(nOpCode, nRetCode));
}

CProtoCommandPtr CProtoSerializer::CreateDspWsResponseCommand(
    const SmartPtr<IOService::IOPackage>& p,
    PRL_RESULT nRetCode
    )
{
    return CreateDspWsResponseCommand( (PVE::IDispatcherCommands)p->header.type, nRetCode );
}

CProtoCommandPtr CProtoSerializer::CreateVmUpdateSecurityCommand(
		const QString &sVmUuid,
		const QString &sVmSecurity
	)
{
	return CProtoCommandPtr(CProtoCommandPtr(new CProtoVmUpdateSecurityCommand(sVmUuid, sVmSecurity)));
}

CProtoCommandPtr CProtoSerializer::CreateCreateSnapshotProtoCommand(
	const QString &sVmUuid,
	const QString &sName,
	const QString &sDescription,
	const QString &sSnapshotUuid,
	const QString &sSnapshotCreator,
	const QString &sSnapshotPath,
	quint32 nFlags
	)
{
	return (CProtoCommandPtr(new CProtoCreateSnapshotCommand(
				sVmUuid, sName, sDescription, sSnapshotUuid, sSnapshotCreator, sSnapshotPath, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateSwitchToSnapshotProtoCommand(
	const QString &sVmUuid,
	const QString &sSnapshotUuid,
	quint32 nFlags
	)
{
	return (CProtoCommandPtr(new CProtoSwitchToSnapshotCommand(sVmUuid, sSnapshotUuid, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateDeleteSnapshotProtoCommand(
	const QString &sVmUuid,
	const QString &sSnapshotUuid,
	bool bChild,
	quint32 nFlags
	)
{
	return (CProtoCommandPtr(new CProtoDeleteSnapshotCommand(sVmUuid, sSnapshotUuid, bChild, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateUpdateSnapshotDataProtoCommand(
	const QString &sVmUuid,
	const QString &sSnapshotUuid,
	const QString &sNewName,
	const QString &sNewDescription
	)

{
	return (CProtoCommandPtr(new CProtoUpdateSnapshotDataCommand(sVmUuid, sSnapshotUuid, sNewName, sNewDescription)));
}

CProtoCommandPtr CProtoSerializer::ParseCommand(PVE::IDispatcherCommands nCmdIdentifier, const QString &sPackage)
{
	CProtoCommand* pCommand;
	switch (nCmdIdentifier)
	{
		case PVE::DspCmdUserLogin: pCommand = static_cast<CProtoCommand *>(new CProtoCommandDspCmdUserLogin); break;
		case PVE::DspCmdUserLoginLocal: pCommand = static_cast<CProtoCommand *>(new CProtoCommandDspCmdUserLoginLocal); break;
		case PVE::DspWsResponse: pCommand = static_cast<CProtoCommand *>(new CProtoCommandDspWsResponse); break;
		case PVE::DspCmdDirVmCreate: pCommand = static_cast<CProtoCommand *>(new CProtoVmCreateCommand); break;
		case PVE::DspCmdDirVmClone: pCommand = static_cast<CProtoCommand *>(new CProtoVmCloneCommand); break;
		case PVE::DspCmdDirVmMove: pCommand = static_cast<CProtoCommand *>(new CProtoVmMoveCommand); break;
		case PVE::DspCmdDirVmMigrate: pCommand = static_cast<CProtoCommand *>(new CProtoVmMigrateCommand); break;
		case PVE::DspCmdCreateVmBackup:
			pCommand = static_cast<CProtoCommand *>(new CProtoCreateVmBackupCommand);
			break;
		case PVE::DspCmdRestoreVmBackup:
			pCommand = static_cast<CProtoCommand *>(new CProtoRestoreVmBackupCommand);
			break;
		case PVE::DspCmdRemoveVmBackup:
			pCommand = static_cast<CProtoCommand *>(new CProtoRemoveVmBackupCommand);
			break;
		case PVE::DspCmdGetBackupTree:
			pCommand = static_cast<CProtoCommand *>(new CProtoGetBackupTreeCommand);
			break;
		case PVE::DspCmdCopyCtTemplate:
			pCommand = static_cast<CProtoCommand *>(new CProtoCopyCtTemplateCommand);
			break;
		case PVE::DspCmdDirCreateImage: pCommand = static_cast<CProtoCommand *>(new CProtoCreateImageCommand); break;
		case PVE::DspCmdDirCopyImage: pCommand = static_cast<CProtoCommand *>(new CProtoCopyImageCommand); break;
		case PVE::DspCmdStartSearchConfig: pCommand = static_cast<CProtoCommand *>(new CProtoStartSearchConfigCommand); break;
		case PVE::DspCmdConvertOldHdd: pCommand = static_cast<CProtoCommand *>(new CProtoStartConvertHardDisksCommand); break;
		case PVE::DspCmdFsRenameEntry: pCommand = static_cast<CProtoCommand *>(new CProtoFsRenameEntryCommand); break;
		case PVE::DspCmdVmCreateUnattendedFloppy: pCommand = static_cast<CProtoCommand *>(new CProtoCreateUnattendedFloppyCommand); break;
		case PVE::DspCmdCreateUnattendedCd:
			 pCommand = static_cast<CProtoCommand *>(new CProtoCreateUnattendedCdCommand);
			 break;
		case PVE::DspCmdDirVmDelete: pCommand = static_cast<CProtoCommand *>(new CProtoVmDeleteCommand); break;
		case PVE::DspCmdFsGenerateEntryName: pCommand = static_cast<CProtoCommand *>(new CProtoCommandDspCmdFsGenerateEntryName); break;
		case PVE::DspCmdVmUpdateSecurity: pCommand = static_cast<CProtoCommand *>(new CProtoVmUpdateSecurityCommand); break;
		case PVE::DspCmdVmCreateSnapshot: pCommand = static_cast<CProtoCommand *>(new CProtoCreateSnapshotCommand); break;
		case PVE::DspCmdVmSwitchToSnapshot: pCommand = static_cast<CProtoCommand *>(new CProtoSwitchToSnapshotCommand); break;
		case PVE::DspCmdVmInternal: pCommand = static_cast<CProtoCommand *>(new CProtoVmInternalCommand); break;
		case PVE::DspCmdCtlVmCommitDiskUnfinished: pCommand = static_cast<CProtoCommand *>(new CProtoVmCommitDiskUnfinished); break;
		case PVE::DspCmdVmDeleteSnapshot: pCommand = static_cast<CProtoCommand *>(new CProtoDeleteSnapshotCommand); break;
		case PVE::DspCmdVmUpdateSnapshotData: pCommand = \
				static_cast<CProtoCommand *>(new CProtoUpdateSnapshotDataCommand); break;
		case PVE::DspCmdVmStartEx: pCommand = static_cast<CProtoCommand *>(new CProtoVmStartExCommand); break;
		case PVE::DspCmdVmLoginInGuest:
		case PVE::DspCmdVmAuthWithGuestSecurityDb:
			pCommand = static_cast<CProtoCommand *>(new CProtoVmLoginInGuestCommand(nCmdIdentifier));
		break;
		case PVE::DspCmdVmMount:
			pCommand = static_cast<CProtoCommand *>(new CProtoVmCommandWithOneStrParam(nCmdIdentifier));
			break;
		case PVE::DspCmdVmUmount:
			 pCommand = static_cast<CProtoCommand *>(new CProtoBasicVmCommand(nCmdIdentifier)); break;
		case PVE::DspCmdUserLogoff:
		case PVE::DspCmdDirGetVmList:
		case PVE::DspCmdUserGetEvent:
		case PVE::DspCmdUserGetProfile:
		case PVE::DspCmdUserInfoList:
		case PVE::DspCmdGetVirtualNetworkList:
		case PVE::DspCmdAllHostUsers:
		case PVE::DspCmdUserProfileBeginEdit:
		case PVE::DspCmdGetHostCommonInfo:
		case PVE::DspCmdHostCommonInfoBeginEdit:
		case PVE::DspCmdUserGetHostHwInfo:
		case PVE::DspCmdUserPing:
		case PVE::DspCmdFsGetDiskList:
		case PVE::DspCmdFsGetCurrentDirectory:
		case PVE::DspCmdSMCGetDispatcherRTInfo:
		case PVE::DspCmdSMCRestartDispatcher:
		case PVE::DspCmdSMCDisconnectAllUsers:
		case PVE::DspCmdNetPrlNetworkServiceStart:
		case PVE::DspCmdNetPrlNetworkServiceStop:
		case PVE::DspCmdNetPrlNetworkServiceRestart:
		case PVE::DspCmdNetPrlNetworkServiceRestoreDefaults:
		case PVE::DspCmdUserGetLicenseInfo:
		case PVE::DspCmdGetHostStatistics:
		case PVE::DspCmdSubscribeToHostStatistics:
		case PVE::DspCmdUnsubscribeFromHostStatistics:
		case PVE::DspCmdGetNetServiceStatus:
		case PVE::DspCmdPrepareForHibernate:
		case PVE::DspCmdAfterHostResume:
		case PVE::DspCmdGetOfflineServicesList:
		case PVE::DspCmdGetNetworkClassesConfig:
		case PVE::DspCmdGetNetworkShapingConfig:
		case PVE::DspCmdRestartNetworkShaping:
		case PVE::DspCmdGetCtTemplateList:
		case PVE::DspCmdGetIPPrivateNetworksList:
		case PVE::DspCmdRefreshPlugins:
		case PVE::DspCmdGetVmConfigById:
		case PVE::DspCmdGetCPUPoolsList:
		case PVE::DspCmdJoinCPUPool:
		case PVE::DspCmdLeaveCPUPool:
			pCommand = static_cast<CProtoCommand *>(new CProtoCommandWithoutParams(nCmdIdentifier));
		break;

		case PVE::DspCmdVmAnswer:
		case PVE::DspCmdDirVmEditCommit:
		case PVE::DspCmdUserProfileCommit:
		case PVE::DspCmdHostCommonInfoCommit:
		case PVE::DspCmdUserCancelOperation:
		case PVE::DspCmdFsGetDirectoryEntries:
		case PVE::DspCmdFsCreateDirectory:
		case PVE::DspCmdFsRemoveEntry:
		case PVE::DspCmdFsCanCreateFile:
		case PVE::DspCmdSMCGetCommandHistoryByUser:
		case PVE::DspCmdSMCDisconnectUser:
		case PVE::DspCmdSMCCancelUserCommand:
		case PVE::DspCmdDirRegVm:
		case PVE::DspCmdCtlApplyVmConfig:
		case PVE::DspEvtNotifyVm:
		case PVE::DspCmdUpdateDeviceInfo:
		case PVE::DspCmdUserLoginLocalStage2:
		case PVE::DspCmdAttachToLostTask:
		case PVE::DspCmdAddNetAdapter:
		case PVE::DspCmdDeleteNetAdapter:
		case PVE::DspCmdUpdateNetAdapter:
		case PVE::DspCmdUserInfo:
		case PVE::DspCmdSMCShutdownDispatcher:
		case PVE::DspCmdAddVirtualNetwork:
		case PVE::DspCmdUpdateVirtualNetwork:
		case PVE::DspCmdDeleteVirtualNetwork:
		case PVE::DspCmdConfigureGenericPci:
		case PVE::DspCmdSetNonInteractiveSession:
		case PVE::DspCmdSendClientStatistics:
		case PVE::DspCmdVmChangeLogLevel:
		case PVE::DspCmdInstallAppliance:
		case PVE::DspCmdUpdateOfflineService:
		case PVE::DspCmdDeleteOfflineService:
		case PVE::DspCmdStartClusterService:
		case PVE::DspCmdStopClusterService:
		case PVE::DspCmdUpdateNetworkClassesConfig:
		case PVE::DspCmdUpdateNetworkShapingConfig:
		case PVE::DspCmdGetDefaultVmConfig:
		case PVE::DspCmdAddIPPrivateNetwork:
		case PVE::DspCmdRemoveIPPrivateNetwork:
		case PVE::DspCmdUpdateIPPrivateNetwork:
		case PVE::DspCmdGetPluginsList:
		case PVE::DspCmdGetDiskFreeSpace:
		case PVE::DspCmdCtlLicenseChange:
		case PVE::DspCmdMoveToCPUPool:
		case PVE::DspCmdRecalculateCPUPool:
			pCommand = static_cast<CProtoCommand *>(new CProtoCommandWithOneStrParam(nCmdIdentifier));
		break;

		case PVE::DspCmdRemoveCtTemplate:
			pCommand = static_cast<CProtoCommand *>(new CProtoCommandWithTwoStrParams(nCmdIdentifier));
		break;

		case PVE::DspCmdVmStart:
		case PVE::DspCmdVmRestartGuest:
		case PVE::DspCmdVmGetConfig:
		case PVE::DspCmdVmGetProblemReport:
		case PVE::DspCmdVmGetPackedProblemReport:
		case PVE::DspCmdVmSuspend:
		case PVE::DspCmdVmGetSuspendedScreen:
		case PVE::DspCmdVmResume:
		case PVE::DspCmdVmDropSuspendedState:
		case PVE::DspCmdVmGetSnapshotsTree:
		case PVE::DspCmdVmReset:
		case PVE::DspCmdVmInstallTools:
		case PVE::DspCmdDirVmEditBegin:
		case PVE::DspCmdDirUnregVm:
		case PVE::DspCmdDirRestoreVm:
		case PVE::DspCmdGetVmInfo:
		case PVE::DspCmdGetVmToolsInfo:
		case PVE::DspCmdSMCShutdownVm:
		case PVE::DspCmdSMCRestartVm:
		case PVE::DspCmdVmGetStatistics:
		case PVE::DspCmdVmSubscribeToGuestStatistics:
		case PVE::DspCmdVmUnsubscribeFromGuestStatistics:
		case PVE::DspCmdSMCGetCommandHistoryByVm:
		case PVE::DspCmdVmInitiateDevStateNotifications:
		case PVE::DspCmdVmCancelCompact:
		case PVE::DspCmdVmMigrateCancel:
		case PVE::DspCmdVmRunCompressor:
		case PVE::DspCmdVmCancelCompressor:
		case PVE::DspCmdVmStartVNCServer:
		case PVE::DspCmdVmStopVNCServer:
		case PVE::DspCmdVmLock:
		case PVE::DspCmdVmUnlock:
		case PVE::DspCmdVmGuestSuspendHardDisk:
		case PVE::DspCmdVmGuestResumeHardDisk:
		case PVE::DspCmdVmChangeSid:
		case PVE::DspCmdVmResetUptime:
			pCommand = static_cast<CProtoCommand *>(new CProtoBasicVmCommand(nCmdIdentifier));
		break;
        case PVE::DspCmdSendProblemReport:
            pCommand = new CProtoSendProblemReport;
        break;
		case PVE::DspCmdVmStop:
			pCommand = static_cast<CProtoCommand *>(new CProtoVmCommandStop(nCmdIdentifier));
		break;
		case PVE::DspCmdVmPause:
			pCommand = static_cast<CProtoCommand *>(new CProtoVmCommandWithAcpiSign(nCmdIdentifier));
		break;
		case PVE::DspCmdVmDevConnect:
		case PVE::DspCmdVmDevDisconnect:
			pCommand = static_cast<CProtoCommand *>(new CProtoVmDeviceCommand(nCmdIdentifier));
		break;

		case PVE::DspCmdDirInstallGuestOS:
		case PVE::DspCmdUserUpdateLicense:
			pCommand = static_cast<CProtoCommand *>(new CProtoSerialNumCommand(nCmdIdentifier));
		break;

		case PVE::DspCmdVmSectionValidateConfig:
			pCommand = static_cast<CProtoCommand *>(new CProtoCreateVmValidateConfigCommand);
		break;

		case PVE::DspCmdPerfomanceStatistics:
			pCommand = new CProtoPerfStatsCommand ;
		break;

		case PVE::DspCmdVmUpdateToolsSection:
		case PVE::DspCmdVmInstallUtility:
		case PVE::DspCmdVmCompact:
		case PVE::DspCmdVmConvertDisks:
		case PVE::DspCmdVmSetProtection:
		case PVE::DspCmdVmRemoveProtection:
			pCommand = static_cast<CProtoCommand *>(new CProtoVmCommandWithOneStrParam(nCmdIdentifier) );
			break;

		case PVE::DspCmdVmGuestLogout:
		case PVE::DspCmdVmGuestGetNetworkSettings:
		case PVE::DspCmdVmGuestChangeSID:
			pCommand = static_cast<CProtoCommand *>(new CProtoBasicVmGuestCommand(nCmdIdentifier));
			break;
		case PVE::DspCmdVmGuestRunProgram:
			pCommand = static_cast<CProtoCommand *>(new CProtoVmGuestRunProgramCommand);
			break;

		case PVE::DspCmdVmGuestSetUserPasswd:
			pCommand = static_cast<CProtoCommand *>(new CProtoVmGuestSetUserPasswdCommand);
			break;
		case PVE::DspCmdVmResizeDisk:
			pCommand = static_cast<CProtoCommand *>(new CProtoVmResizeDiskImageCommand);
			break;
		case PVE::DspCmdSetSessionConfirmationMode:
			pCommand = static_cast<CProtoCommand *>(new CProtoCommandDspCmdSetSessionConfirmationMode);
			break;
		case PVE::DspCmdStorageSetValue:
			pCommand = static_cast<CProtoCommand *>(new CProtoDspCmdStorageSetValueCommand);
			break;
		case PVE::DspCmdVmStorageSetValue:
			pCommand = static_cast<CProtoCommand *>(new CProtoDspCmdVmStorageSetValueCommand);
			break;
		case PVE::DspCmdUpdateUsbAssocList:
			pCommand = static_cast<CProtoCommand *>(new CProtoUpdateUsbAssocListCommand);
			break;
		case PVE::DspCmdDirReg3rdPartyVm:
			pCommand = static_cast<CProtoCommand *>(new CProtoDspCmdDirReg3rdPartyVmCommand);
			break;
		case PVE::DspEvtNetworkPrefChanged:
			pCommand = static_cast<CProtoCommand *>(new CProtoDspCmdNetworkPrefsChangedCommand);
			break;
		default: pCommand = static_cast<CProtoCommand *>(new CProtoCommandIllegalCommand); break;
	}
	pCommand->ParsePackage(sPackage);
	return CProtoCommandPtr(pCommand);
}

CProtoCommandPtr CProtoSerializer::ParseCommand(
    const SmartPtr<IOService::IOPackage>& p )
{
    if ( ! p.isValid() )
        return CProtoCommandPtr(new CProtoCommandIllegalCommand());

    return ParseCommand( (PVE::IDispatcherCommands)p->header.type,
						 UTF8_2QSTR( p->buffers[0].getImpl() ) );
}

CProtoCommandPtr CProtoSerializer::CreateDspCmdFsGenerateEntryNameCommand(
		const QString &sDirPath,
		const QString &sFilenamePrefix,
		const QString &sFilenameSuffix,
		const QString &sIndexDelimiter
	)
{
	return CProtoCommandPtr(new CProtoCommandDspCmdFsGenerateEntryName(
									sDirPath,
									sFilenamePrefix,
									sFilenameSuffix,
									sIndexDelimiter
									));
}

CProtoCommandPtr CProtoSerializer::CreateVmValidateConfigProtoCommand(const QString &sVmConfig,	PRL_VM_CONFIG_SECTIONS nSection)
{
	return CProtoCommandPtr(new CProtoCreateVmValidateConfigCommand(sVmConfig, nSection));
}

CProtoCommandPtr CProtoSerializer::CreateVmMigrateProtoCommand(
		const QString &sVmUuid,
		const QString &sTargetServerHostname,
		quint32 nTargetServerPort,
		const QString &sTargetServerSessionUuid,
		const QString &sTargetVmName,
		const QString &sTargetVmHomePath,
		quint32 nMigrationFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	)
{
	return CProtoCommandPtr(new CProtoVmMigrateCommand(sVmUuid, sTargetServerHostname, nTargetServerPort,
														sTargetServerSessionUuid, sTargetVmName, sTargetVmHomePath,
														nMigrationFlags, nReservedFlags, bForceOperation));
}

CProtoCommandPtr CProtoSerializer::CreateCreateVmBackupCommand(
	const QString &sVmUuid,
	const QString &sServerHostname,
	quint32 nServerPort,
	const QString &sServerSessionUuid,
	const QString &sDescription,
	quint32 nBackupFlags,
	quint32 nReservedFlags,
	bool bForceOperation
)
{
	return CProtoCommandPtr( new CProtoCreateVmBackupCommand(
		sVmUuid,
		sServerHostname,
		nServerPort,
		sServerSessionUuid,
		sDescription,
		nBackupFlags,
		nReservedFlags,
		bForceOperation));
}

CProtoCommandPtr CProtoSerializer::CreateRestoreVmBackupCommand(
	const QString &sVmUuid,
	const QString &sBackupUuid,
	const QString &sServerHostname,
	quint32 nServerPort,
	const QString &sServerSessionUuid,
	const QString &sTargetVmHomePath,
	const QString &sTargetVmName,
	quint32 nRestoreFlags,
	quint32 nReservedFlags
)
{
	return CProtoCommandPtr( new CProtoRestoreVmBackupCommand(
		sVmUuid,
		sBackupUuid,
		sServerHostname,
		nServerPort,
		sServerSessionUuid,
		sTargetVmHomePath,
		sTargetVmName,
		nRestoreFlags,
		nReservedFlags));
}

CProtoCommandPtr CProtoSerializer::CreateGetBackupTreeCommand(
	const QString &sVmUuid,
	const QString &sServerHostname,
	quint32 nServerPort,
	const QString &sServerSessionUuid,
	quint32 nBackupFlags,
	quint32 nReservedFlags,
	bool bForceOperation
)
{
	return CProtoCommandPtr( new CProtoGetBackupTreeCommand(
		sVmUuid,
		sServerHostname,
		nServerPort,
		sServerSessionUuid,
		nBackupFlags,
		nReservedFlags,
		bForceOperation));
}

CProtoCommandPtr CProtoSerializer::CreateRemoveVmBackupCommand(
	const QString &sVmUuid,
	const QString &sBackupUuid,
	const QString &sServerHostname,
	quint32 nServerPort,
	const QString &sServerSessionUuid,
	quint32 nRestoreFlags,
	quint32 nReservedFlags,
	bool bForceOperation
)
{
	return CProtoCommandPtr( new CProtoRemoveVmBackupCommand(
		sVmUuid,
		sBackupUuid,
		sServerHostname,
		nServerPort,
		sServerSessionUuid,
		nRestoreFlags,
		nReservedFlags,
		bForceOperation));
}

CProtoCommandPtr CProtoSerializer::CreateVmStartExProtoCommand(
		const QString &sVmUuid,
		quint32 nStartMode,
		quint32 nReserved
	)
{
	return CProtoCommandPtr(new CProtoVmStartExCommand(sVmUuid, nStartMode, nReserved));
}

CProtoCommandPtr CProtoSerializer::CreateVmLoginInGuestProtoCommand(
	const QString &sVmUuid,
	const QString &sUserName,
	const QString &sUserPassword,
	quint32 nFlags
)
{
	return CProtoCommandPtr(new CProtoVmLoginInGuestCommand(PVE::DspCmdVmLoginInGuest, sVmUuid, sUserName, sUserPassword, nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateVmAuthWithGuestSecurityDbProtoCommand(
	const QString &sVmUuid,
	const QString &sUserName,
	const QString &sUserPassword,
	quint32 nFlags
)
{
	return CProtoCommandPtr(new CProtoVmLoginInGuestCommand(PVE::DspCmdVmAuthWithGuestSecurityDb, sVmUuid, sUserName, sUserPassword, nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateBasicVmGuestProtoCommand(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	const QString &sVmGuestUuid,
	quint32 nFlags
)
{
	return CProtoCommandPtr(new CProtoBasicVmGuestCommand(nCmdIdentifier, sVmUuid, sVmGuestUuid, nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateVmGuestRunProgramProtoCommand(
	const QString &sVmUuid,
	const QString &sVmGuestUuid,
	const QString &sProgramName,
	const QStringList &lstArgs,
	const QStringList &lstVars,
	quint32 nFlags
)
{
	return CProtoCommandPtr(new CProtoVmGuestRunProgramCommand(sVmUuid, sVmGuestUuid, sProgramName,
																lstArgs, lstVars, nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateVmGuestSetUserPasswdProtoCommand(
	const QString &sVmUuid,
	const QString &sVmGuestUuid,
	const QString &sUserLoginName,
	const QString &sUserPassword,
	quint32 nFlags
)
{
	return CProtoCommandPtr(new CProtoVmGuestSetUserPasswdCommand(sVmUuid, sVmGuestUuid, sUserLoginName,
																sUserPassword, nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateVmResizeDiskImageProtoCommand(
	const QString &sVmUuid,
	const QString &sDiskImage,
	quint32 nNewSize,
	quint32 nFlags
)
{
	return CProtoCommandPtr(new CProtoVmResizeDiskImageCommand(sVmUuid, sDiskImage, nNewSize, nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateSetSessionConfirmationModeProtoCommand(
	bool bEnable,
	const QString &sUsername,
	const QString &sPassword,
	quint32 nFlags
	)
{
	return CProtoCommandPtr(new CProtoCommandDspCmdSetSessionConfirmationMode(
		bEnable, sUsername, sPassword, nFlags));
}


CProtoCommandPtr CProtoSerializer::CreateDspCmdStorageSetValueCommand(
	const QString &sKey,
	const QString &sValue,
	quint32 nReservedFlags
	)
{
	return CProtoCommandPtr(new CProtoDspCmdStorageSetValueCommand(
		sKey, sValue, nReservedFlags));
}

CProtoCommandPtr CProtoSerializer::CreateDspCmdVmStorageSetValueCommand(
	const QString &sVmUuid,
	const QString &sKey,
	const QString &sValue,
	quint32 nReservedFlags
	)
{
	return CProtoCommandPtr(new CProtoDspCmdVmStorageSetValueCommand(
		sVmUuid, sKey, sValue, nReservedFlags));
}

CProtoCommandPtr CProtoSerializer::CreateDspCmdDirReg3rdPartyVmProtoCommand(
	const QString &sVmConfigPath,
	const QString &sVmRootDirPath,
	quint32 nFlags
)
{
	return CProtoCommandPtr(new CProtoDspCmdDirReg3rdPartyVmCommand(sVmConfigPath, sVmRootDirPath, nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateDspCmdNetworkPrefsChanged(
		const QString &sNetworkConfig,
		const QString &sOldNetworkID,
		const QString &sNewNetworkID,
		quint32 nFlags
)
{
	return CProtoCommandPtr(new CProtoDspCmdNetworkPrefsChangedCommand(
					sNetworkConfig, sOldNetworkID, sNewNetworkID, nFlags));
}

CProtoCommandPtr CProtoSerializer::CreateVmInternalProtoCommand(
	const QString &sVmUuid,
	const QString &sCmd,
	const QStringList &lstArgs
	)
{
	return (CProtoCommandPtr(new CProtoVmInternalCommand(sVmUuid, sCmd, lstArgs)));
}

CProtoCommandPtr CProtoSerializer::CreateVmCommitDiskUnfinished(
	const QString &sVmUuid,
	const QString &sDiskName,
	const QString &sSnapshotUuid
	)
{
	return (CProtoCommandPtr(new CProtoVmCommitDiskUnfinished(sVmUuid, sDiskName, sSnapshotUuid )));
}

CProtoCommandPtr CProtoSerializer::CreateProtoVmCommandWithTwoStrParams(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	const QString &sParam1,
	const QString &sParam2,
	quint32 nFlags
	)
{
	return (CProtoCommandPtr(new CProtoVmCommandWithTwoStrParams(nCmdIdentifier, sVmUuid, sParam1, sParam2, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateCopyCtTemplateProtoCommand(
		const QString &sName,
		const QString &sOsTmplName,
		const QString &sTargetServerHostname,
		quint32 nTargetServerPort,
		const QString &sTargetServerSessionUuid,
		quint32 nFlags,
		quint32 nReservedFlags
	)
{
	return CProtoCommandPtr(new CProtoCopyCtTemplateCommand(
			sName, sOsTmplName, sTargetServerHostname, nTargetServerPort, sTargetServerSessionUuid,
			nFlags, nReservedFlags));
}

CProtoCommandPtr CProtoSerializer::CreateVmMoveProtoCommand(
	const QString &sVmUuid,
	const QString &sNewHomePath,
	PRL_UINT32 nFlags
)
{
	return (CProtoCommandPtr(new CProtoVmMoveCommand(sVmUuid, sNewHomePath, nFlags)));
}

CProtoCommandPtr CProtoSerializer::CreateSendProblemReportProtoCommand(
    const QString &report,
    const QString &vmUuid,
    PRL_UINT32 nFlags
)
{
    return (CProtoCommandPtr(
        new CProtoSendProblemReport(report, vmUuid, nFlags)));
}

}//namespace Parallels

