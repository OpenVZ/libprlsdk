/*
 * CommandConvHelper.cpp: Command converter from SDK to dispatcher
 * constants and vice versa
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


#include <QMutex>
#include <QHash>
#include "CommandConvHelper.h"

//*********************************Dispatcher commands converter implementation******************************
Q_GLOBAL_STATIC(QMutex, DispatcherCommandsAndJobTypesMapsMutex)
typedef QHash<PVE::IDispatcherCommands, PRL_JOB_OPERATION_CODE> DispatcherCmdsToJobTypesMapType;
Q_GLOBAL_STATIC(DispatcherCmdsToJobTypesMapType, DispatcherCommandsToJobTypesMap)
typedef QHash<PRL_JOB_OPERATION_CODE, PVE::IDispatcherCommands> JobTypesToDispatcherCmdsMapType;
Q_GLOBAL_STATIC(JobTypesToDispatcherCmdsMapType, JobTypesToDispatcherCommandsMap)

namespace {
void InitializeDispatcherCommandsMaps()
{
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserCancelOperation] = PJOC_JOB_CANCEL;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdLookupParallelsServers] = PJOC_SRV_LOOKUP_PARALLELS_SERVERS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserLogin] = PJOC_SRV_LOGIN;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserLoginLocal] = PJOC_SRV_LOGIN_LOCAL;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserLoginLocalStage2] = PJOC_SRV_LOGIN_LOCAL;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserLogoff] = PJOC_SRV_LOGOFF;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdSetNonInteractiveSession] = PJOC_SRV_SET_NON_INTERACTIVE_SESSION;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdSetSessionConfirmationMode] = PJOC_SRV_SET_SESSION_CONFIRMATION_MODE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserGetHostHwInfo] = PJOC_SRV_GET_SRV_CONFIG;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetHostCommonInfo] = PJOC_SRV_GET_COMMON_PREFS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdHostCommonInfoBeginEdit] = PJOC_SRV_COMMON_PREFS_BEGIN_EDIT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdHostCommonInfoCommit] = PJOC_SRV_COMMON_PREFS_COMMIT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserGetProfile] = PJOC_SRV_GET_USER_PROFILE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetHostStatistics] = PJOC_SRV_GET_STATISTICS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserProfileBeginEdit] = PJOC_SRV_USER_PROFILE_BEGIN_EDIT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserProfileCommit] = PJOC_SRV_USER_PROFILE_COMMIT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirRegVm] = PJOC_SRV_REGISTER_VM;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirRestoreVm] = PJOC_VM_RESTORE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirGetVmList] = PJOC_SRV_GET_VM_LIST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdSubscribeToHostStatistics] = PJOC_SRV_SUBSCRIBE_TO_HOST_STATISTICS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUnsubscribeFromHostStatistics] =\
											PJOC_SRV_UNSUBSCRIBE_FROM_HOST_STATISTICS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdSMCShutdownDispatcher] = PJOC_SRV_SHUTDOWN;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdFsGetDiskList] = PJOC_SRV_FS_GET_DISK_LIST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdFsGetDirectoryEntries] = PJOC_SRV_FS_GET_DIR_ENTRIES;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdFsCreateDirectory] = PJOC_SRV_FS_CREATE_DIR;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdFsRemoveEntry] = PJOC_SRV_FS_REMOVE_ENTRY;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdFsCanCreateFile] = PJOC_SRV_FS_CAN_CREATE_FILE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdFsRenameEntry] = PJOC_SRV_FS_RENAME_ENTRY;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdFsGenerateEntryName] = PJOC_SRV_FS_GENERATE_ENTRY_NAME;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserUpdateLicense] = PJOC_SRV_UPDATE_LICENSE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserGetLicenseInfo] = PJOC_SRV_GET_LICENSE_INFO;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmAnswer] = PJOC_SRV_SEND_ANSWER;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdStartSearchConfig] = PJOC_SRV_START_SEARCH_VMS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdNetPrlNetworkServiceStart] = PJOC_SRV_NET_SERVICE_START;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdNetPrlNetworkServiceStop] = PJOC_SRV_NET_SERVICE_STOP;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdNetPrlNetworkServiceRestart] = PJOC_SRV_NET_SERVICE_RESTART;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdNetPrlNetworkServiceRestoreDefaults] =\
											PJOC_SRV_NET_SERVICE_RESTORE_DEFAULTS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetNetServiceStatus] = PJOC_SRV_GET_NET_SERVICE_STATUS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdAddNetAdapter] = PJOC_SRV_ADD_NET_ADAPTER;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDeleteNetAdapter] = PJOC_SRV_DELETE_NET_ADAPTER;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUpdateNetAdapter] = PJOC_SRV_UPDATE_NET_ADAPTER;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGetProblemReport] = PJOC_SRV_GET_PROBLEM_REPORT;
    (*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGetPackedProblemReport] = PJOC_SRV_GET_PACKED_PROBLEM_REPORT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdAttachToLostTask] = PJOC_SRV_ATTACH_TO_LOST_TASK;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserInfoList] = PJOC_SRV_GET_USER_INFO_LIST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUserInfo] = PJOC_SRV_GET_USER_INFO;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdPrepareForHibernate] = PJOC_SRV_PREPARE_FOR_HIBERNATE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdAfterHostResume] = PJOC_SRV_AFTER_HOST_RESUME;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetVirtualNetworkList] = PJOC_SRV_GET_VIRTUAL_NETWORK_LIST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdAddVirtualNetwork] = PJOC_SRV_ADD_VIRTUAL_NETWORK;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUpdateVirtualNetwork] = PJOC_SRV_UPDATE_VIRTUAL_NETWORK;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDeleteVirtualNetwork] = PJOC_SRV_DELETE_VIRTUAL_NETWORK;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdConfigureGenericPci] = PJOC_SRV_CONFIGURE_GENERIC_PCI;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdAllHostUsers] = PJOC_SRV_GET_ALL_HOST_USERS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmStart] = PJOC_VM_START;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmStartEx] = PJOC_VM_START_EX;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmRestartGuest] = PJOC_VM_RESTART;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmStop] = PJOC_VM_STOP;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmPause] = PJOC_VM_PAUSE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmReset] = PJOC_VM_RESET;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmInternal] = PJOC_VM_CMD_INTERNAL;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmSuspend] = PJOC_VM_SUSPEND;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGetSuspendedScreen] = PJOC_VM_GET_SUSPENDED_SCREEN;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmResume] = PJOC_VM_RESUME;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmDropSuspendedState] = PJOC_VM_DROP_SUSPENDED_STATE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirVmClone] = PJOC_VM_CLONE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirVmDelete] = PJOC_VM_DELETE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGetState] = PJOC_VM_GET_STATE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetVmToolsInfo] = PJOC_VM_GET_TOOLS_STATE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGetConfig] = PJOC_VM_REFRESH_CONFIG;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGetStatistics] = PJOC_VM_GET_STATISTICS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmSubscribeToGuestStatistics] =\
											PJOC_VM_SUBSCRIBE_TO_GUEST_STATISTICS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmUnsubscribeFromGuestStatistics] =\
											PJOC_VM_UNSUBSCRIBE_FROM_GUEST_STATISTICS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirVmCreate] = PJOC_VM_REG;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirUnregVm] = PJOC_VM_UNREG;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirVmEditBegin] = PJOC_VM_BEGIN_EDIT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirVmEditCommit] = PJOC_VM_COMMIT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmCreateUnattendedFloppy] = PJOC_VM_CREATE_UNATTENDED_FLOPPY;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmInitiateDevStateNotifications] =\
											PJOC_VM_INITIATE_DEV_STATE_NOTIFICATIONS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmUpdateSecurity] = PJOC_VM_UPDATE_SECURITY;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmSectionValidateConfig] = PJOC_VM_VALIDATE_CONFIG;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmDevConnect] = PJOC_VM_DEV_CONNECT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmDevDisconnect] = PJOC_VM_DEV_DISCONNECT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmDevHdCheckPassword] = PJOC_VM_DEV_HD_CHECK_PASSWORD;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirCreateImage] = PJOC_VM_DEV_CREATE_IMAGE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirCopyImage] = PJOC_VM_DEV_COPY_IMAGE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmResizeDisk] = PJOC_VM_RESIZE_DISK_IMAGE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmInstallUtility] = PJOC_VM_INSTALL_UTILITY;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmInstallTools] = PJOC_VM_INSTALL_TOOLS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmRunCompressor] = PJOC_VM_RUN_COMPRESSOR;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmCancelCompressor] = PJOC_VM_CANCEL_COMPRESSOR;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmStartVNCServer] = PJOC_VM_START_VNC_SERVER;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmStopVNCServer] = PJOC_VM_STOP_VNC_SERVER;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGuestGetNetworkSettings] = PJOC_VM_GUEST_GET_NETWORK_SETTINGS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmLoginInGuest] = PJOC_VM_LOGIN_IN_GUEST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGuestRunProgram] = PJOC_VM_GUEST_RUN_PROGRAM;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGuestLogout] = PJOC_VM_GUEST_LOGOUT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmAuthWithGuestSecurityDb] = PJOC_VM_AUTH_WITH_GUEST_SECURITY_DB;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmGuestSetUserPasswd] = PJOC_VM_GUEST_SET_USER_PASSWD;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdCreateVmBackup] = PJOC_SRV_CREATE_VM_BACKUP;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdRestoreVmBackup] = PJOC_SRV_RESTORE_VM_BACKUP;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetBackupTree] = PJOC_SRV_GET_BACKUP_TREE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdRemoveVmBackup] = PJOC_SRV_REMOVE_VM_BACKUP;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmLock] = PJOC_VM_LOCK;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmUnlock] = PJOC_VM_UNLOCK;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdStorageSetValue] = PJOC_SRV_STORE_VALUE_BY_KEY;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmStorageSetValue] = PJOC_VM_STORE_VALUE_BY_KEY;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmCompact] = PJOC_VM_COMPACT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmConvertDisks] = PJOC_VM_CONVERT_DISKS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmCancelCompact] = PJOC_VM_CANCEL_COMPACT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirReg3rdPartyVm] = PJOC_SRV_REGISTER_3RD_PARTY_VM;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmChangeSid] = PJOC_VM_CHANGE_SID;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmResetUptime] = PJOC_VM_RESET_UPTIME;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdInstallAppliance] = PJOC_SRV_INSTALL_APPLIANCE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUpdateOfflineService] = PJOC_SRV_UPDATE_OFFLINE_SERVICE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDeleteOfflineService] = PJOC_SRV_DELETE_OFFLINE_SERVICE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetOfflineServicesList] = PJOC_SRV_GET_OFFLINE_SERVICES_LIST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmAuthorise] = PJOC_VM_AUTHORISE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmChangePassword] = PJOC_VM_CHANGE_PASSWORD;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmEncrypt] = PJOC_VM_ENCRYPT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmDecrypt] = PJOC_VM_DECRYPT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUpdateNetworkClassesConfig] = PJOC_SRV_UPDATE_NETWORK_CLASSES_CONFIG;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetNetworkClassesConfig] = PJOC_SRV_GET_NETWORK_CLASSES_LIST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUpdateNetworkShapingConfig] = PJOC_SRV_UPDATE_NETWORK_SHAPING_CONFIG;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetNetworkShapingConfig] = PJOC_SRV_GET_NETWORK_SHAPING_CONFIG;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdRestartNetworkShaping] = PJOC_SRV_RESTART_NETWORK_SHAPING;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdRegisterIscsiStorage] = PJOC_SRV_REGISTER_ISCSI_STORAGE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUnregisterIscsiStorage] = PJOC_SRV_UNREGISTER_ISCSI_STORAGE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdExtendIscsiStorage] = PJOC_SRV_EXTEND_ISCSI_STORAGE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetCtTemplateList] = PJOC_SRV_GET_CT_TEMPLATE_LIST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdRemoveCtTemplate] = PJOC_SRV_REMOVE_CT_TEMPLATE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdCopyCtTemplate] = PJOC_SRV_COPY_CT_TEMPLATE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdAddIPPrivateNetwork] = PJOC_SRV_ADD_IPPRIVATE_NETWORK;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdRemoveIPPrivateNetwork] = PJOC_SRV_REMOVE_IPPRIVATE_NETWORK;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdUpdateIPPrivateNetwork] = PJOC_SRV_UPDATE_IPPRIVATE_NETWORK;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetIPPrivateNetworksList] = PJOC_SRV_GET_IPPRIVATE_NETWORKS_LIST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirVmMigrate] = PJOC_VM_MIGRATE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmMigrateCancel] = PJOC_VM_MIGRATE_CANCEL;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdRefreshPlugins] = PJOC_SRV_REFRESH_PLUGINS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmMount] = PJOC_VM_MOUNT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmUmount] = PJOC_VM_UMOUNT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetPluginsList] = PJOC_SRV_GET_PLUGINS_LIST;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetDiskFreeSpace] = PJOC_SRV_GET_DISK_FREE_SPACE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdDirVmMove] = PJOC_VM_MOVE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetVmConfigById] = PJOC_SRV_GET_VM_CONFIG;
    (*DispatcherCommandsToJobTypesMap())[PVE::DspCmdSendProblemReport] = PJOC_SRV_SEND_PROBLEM_REPORT;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmSetProtection] = PJOC_VM_SET_PROTECTION;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdVmRemoveProtection] = PJOC_VM_REMOVE_PROTECTION;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdGetCPUPoolsList] = PJOC_SRV_CPU_POOLS_LIST_POOLS;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdMoveToCPUPool] = PJOC_SRV_CPU_POOLS_MOVE;
	(*DispatcherCommandsToJobTypesMap())[PVE::DspCmdRecalculateCPUPool] = PJOC_SRV_CPU_POOLS_RECALCULATE;

	DispatcherCmdsToJobTypesMapType::const_iterator _it = DispatcherCommandsToJobTypesMap()->begin();
	for(; _it != DispatcherCommandsToJobTypesMap()->end(); ++_it)
		(*JobTypesToDispatcherCommandsMap())[_it.value()] = _it.key();
	//Account some exceptions
	(*JobTypesToDispatcherCommandsMap())[PJOC_VM_GET_PROBLEM_REPORT] = PVE::DspCmdVmGetProblemReport;
	(*JobTypesToDispatcherCommandsMap())[PJOC_VM_GET_PACKED_PROBLEM_REPORT] = PVE::DspCmdVmGetPackedProblemReport;
    (*JobTypesToDispatcherCommandsMap())[PJOC_SRV_SEND_PROBLEM_REPORT] = PVE::DspCmdSendProblemReport;
	(*JobTypesToDispatcherCommandsMap())[PJOC_VM_GENERATE_VM_DEV_FILENAME] = PVE::DspCmdFsGenerateEntryName;

    for(unsigned i=PJOC_SRV_SUBSCRIBE_PERFSTATS; i<=PJOC_VM_GET_PERFSTATS; ++i)
        (*JobTypesToDispatcherCommandsMap())[(PRL_JOB_OPERATION_CODE)i] = PVE::DspCmdPerfomanceStatistics ;
}

}

PRL_JOB_OPERATION_CODE DispatcherCmdsToJobTypeConverter::Convert(PVE::IDispatcherCommands nCmdId)
{
	QMutexLocker _lock(DispatcherCommandsAndJobTypesMapsMutex());
	if (!DispatcherCommandsToJobTypesMap()->size())
		InitializeDispatcherCommandsMaps();
	DispatcherCmdsToJobTypesMapType::const_iterator _it =
		DispatcherCommandsToJobTypesMap()->find(nCmdId);
	if (_it != DispatcherCommandsToJobTypesMap()->end())
		return (_it.value());
	return (PJOC_UNKNOWN);
}

PVE::IDispatcherCommands DispatcherCmdsToJobTypeConverter::Convert(PRL_JOB_OPERATION_CODE nJobOpCode)
{
	QMutexLocker _lock(DispatcherCommandsAndJobTypesMapsMutex());
	if (!JobTypesToDispatcherCommandsMap()->size())
		InitializeDispatcherCommandsMaps();
	JobTypesToDispatcherCmdsMapType::const_iterator _it =
		JobTypesToDispatcherCommandsMap()->find(nJobOpCode);
	if (_it != JobTypesToDispatcherCommandsMap()->end())
		return (_it.value());
	return (PVE::DspIllegalCommand);
}
