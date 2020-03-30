/*
 * PrlHandleServerVm.h
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


#ifndef __VIRTUOZZO_HANDLE_SERVER_VM_H__
#define __VIRTUOZZO_HANDLE_SERVER_VM_H__

#include "PrlHandleServer.h"


/**
 * Base handle object - all objects representation on the client
 * side should follow it's interface by the means of inheritance.
 */
class PrlHandleServerVm : public PrlHandleServer
{
public:

	/**
	 * Registries pointer to VM object in common VM storage
	 *
	 * @param VM uuid
	 * @param pointer to VM object
	 */
	void RegisterVm(const QString &sVmUuid, PRL_HANDLE hVm);

	/**
	 * Unregistries pointer to VM object from common VM storage
	 *
	 * @param VM uuid
	 * @param pointer to VM object
	 */
	void UnregisterVm(const QString &sVmUuid, PRL_HANDLE hVm);

	/**
	 * Perform VM registration by vm path on server.
	 *
	 * @param path to VM directory of registering VM
	 * @param sign whether non interactive mode using
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr RegisterVm( PRL_CONST_STR  sVmDirPath, PRL_BOOL bNonInteractiveMode );

	/**
	* Performs register VM operation
	*
	* @param path to VM home dir
	* @param with bitmask of flags
	* @return id of performed asynchronous request
	*/
	PrlHandleJobPtr RegisterVmEx( PRL_CONST_STR sVmDirPath, PRL_CONST_STR strVmUuid, PRL_UINT32 nFlags );

	/**
	* Performs register 3rd party VM operation
	*
	* @param path to VM config
	* @param path to VM root dir
	* @param with bitmask of flags
	* @return id of performed asynchronous request
	*/
	PrlHandleJobPtr Register3rdPartyVm( PRL_CONST_STR sVmRootDirPath, PRL_CONST_STR  sVmConfigPath, PRL_UINT32 nFlags );

	/**
	* Returns supported OSes matrix operation
	*
	* @param pointer to the buffer for storing result
	* @return PRL_RESULT. Possible values:
	*			PRL_ERR_NO_DATA - login procedure wasn't done (completed) yet
	*			PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetSupportedOses( PRL_HANDLE_PTR phOsesMatrix );

	/**
	 * Perform get VM's list to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetVmList(PRL_UINT32 nFlags);

	/*
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdGetDefaultVmConfig(PRL_CONST_GET_VM_CONFIG_PARAM_DATA_PTR pParam,
								PRL_UINT32 nFlags);

	/**
	 * Perform start search VMs request to server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr StartSearchVms(PRL_HANDLE hStringsList);

	/**
	 * Performs backup VM request to the server
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param backup description
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	PrlHandleJobPtr DspCmdCreateVmBackup(
			PRL_CONST_STR sVmUuid,
			PRL_CONST_STR sServerHostname,
			PRL_UINT32 nServerPort,
			PRL_CONST_STR sServerSessionUuid,
			PRL_CONST_STR strDescription,
			PRL_UINT32 nBackupFlags,
			PRL_UINT32 nReservedFlags,
			PRL_BOOL bForceOperation
	);

	/**
	 * Performs restore VM  from backup request to the server
	 * @param VM UUID
	 * @param backup UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target VM home path
	 * @param flags
	 * @param reserved flags
	 */
	PrlHandleJobPtr DspCmdRestoreVmBackup(
			PRL_CONST_STR sVmUuid,
			PRL_CONST_STR sBackupUuid,
			PRL_CONST_STR sServerHostname,
			PRL_UINT32 nServerPort,
			PRL_CONST_STR sServerSessionUuid,
			PRL_CONST_STR sTargetVmHomePath,
			PRL_CONST_STR sTargetVmName,
			PRL_UINT32 nRestoreFlags,
			PRL_UINT32 nReservedFlags
	);

	/**
	 * Performs get backup tree request to the server
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	PrlHandleJobPtr DspCmdGetBackupTree(
			PRL_CONST_STR sVmUuid,
			PRL_CONST_STR sServerHostname,
			PRL_UINT32 nServerPort,
			PRL_CONST_STR sServerSessionUuid,
			PRL_UINT32 nBackupFlags,
			PRL_UINT32 nReservedFlags,
			PRL_BOOL bForceOperation
	);

	/**
	 * Performs remove backup request to the server
	 * @param VM UUID
	 * @param backup UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	PrlHandleJobPtr DspCmdRemoveVmBackup(
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sBackupUuid,
		PRL_CONST_STR sServerHostname,
		PRL_UINT32 nServerPort,
		PRL_CONST_STR sServerSessionUuid,
		PRL_UINT32 nRestoreFlags,
		PRL_UINT32 nReservedFlags,
		PRL_BOOL bForceOperation
	);

	/**
	 * Performs operation of creation bootable ISO-image for unattended guest OS installation
	 * @param guest OS version from the (PVS_GUEST_VER_...)
	 * @param user name
	 * @param password
	 * @param full user name
	 * @param a full path to the OS distribution image or folder
	 * @param a full path to output ISO-image file
	 */
	PrlHandleJobPtr DspCmdCreateUnattendedCd(
			PRL_UINT32 nGuestType,
			PRL_CONST_STR sUserName,
			PRL_CONST_STR sPasswd,
			PRL_CONST_STR sFullUserName,
			PRL_CONST_STR sOsDistroPath,
			PRL_CONST_STR sOutImagePath
	);

	/**
	 * Gets list of all Container templates.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetCtTemplateList(PRL_UINT32 nFlags);

	/**
	 * Removes Container template
	 * @param name of template
	 * @param name of os template
	 * @param command flags
	 */
	PrlHandleJobPtr RemoveCtTemplate(PRL_CONST_STR sName,
			PRL_CONST_STR sOsTmplName, PRL_UINT32 nFlags);

	/**
	 * Copy Container template
	 * @param name of template
	 * @param name of os template
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param command flags
	 * @param reserved command flags
	 */
	PrlHandleJobPtr CopyCtTemplate(
			PRL_CONST_STR sName,
			PRL_CONST_STR sOsTmplName,
			PRL_CONST_STR sTargetServerHostname,
			PRL_UINT32 nTargetServerPort,
			PRL_CONST_STR sTargetServerSessionUuid,
			PRL_UINT32 nFlags,
			PRL_UINT32 nReservedFlags);

	/**
	 * Get VM config by given UUID or name
	 * @param VM UUID or name
	 * @param flags, PFVF_BY_NAME, PFVF_BY_UUID or both
	 */
	PrlHandleJobPtr GetVmConfig(PRL_CONST_STR sSearchId, PRL_UINT32 nFlags);

	/**
	 * Starts VM
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmStart(PRL_CONST_STR sVmUuid);

	/**
	 * Starts VM in the specified mode
	 * @param VM uuid
	 * @param start mode
	 * @param reserved parameter
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmStartEx(PRL_CONST_STR sVmUuid, PRL_UINT32 nStartMode, PRL_UINT32 nReserved);

	/**
	 * Restarts VM guest OS
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmRestartGuest(PRL_CONST_STR sVmUuid);

	/**
	 * Stops VM
	 * @param VM uuid
	 * @param VM stop mode
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmStop(PRL_CONST_STR sVmUuid, PRL_UINT32 nStopMode, PRL_UINT32 nFlags);

	/**
	 * Pauses VM
	 * @param VM uuid
	 * @param sign whether ACPI must be used
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmPause(PRL_CONST_STR sVmUuid, PRL_BOOL bAcpi);

	/**
	 * Resets VM
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmReset(PRL_CONST_STR sVmUuid);

	/**
	 * Suspends VM
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmSuspend(PRL_CONST_STR sVmUuid);

	/**
	 * Gets suspended VM screen
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmGetSuspendedScreen(PRL_CONST_STR sVmUuid);

	/**
	 * Resumes VM
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmResume(PRL_CONST_STR sVmUuid);

	/**
	* Drop suspended VM state
	* @param VM uuid
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmDropSuspendedState(PRL_CONST_STR sVmUuid);

	/**
	* Start creation procedure of the new snapshot
	* @param VM uuid
	* @param snapshot name
	* @param snapshot description
	* @param uuid of the state to create
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmCreateSnapshot(PRL_CONST_STR sVmUuid,
			PRL_CONST_STR sName, PRL_CONST_STR sDescription,
			PRL_CONST_STR sSnapshotUuid, PRL_UINT32 nFlags);

	/**
	* Get snapshots tree
	* @param VM uuid
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmGetSnapshotsTree(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	 * Tries to lock VM
	 * @param VM uuid
	 * @param flags bitset
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmLock(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	 * Tries to unlock VM
	 * @param VM uuid
	 * @param flags bitset
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmUnlock(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	* Start switching to state procedure
	* @param VM uuid
	* @param uuid of the state to switch to
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmSwitchToSnapshot(PRL_CONST_STR sVmUuid, PRL_CONST_STR sSnapshotUuid,
			PRL_UINT32 nFlags);

	/**
	* Start delete state procedure
	* @param VM uuid
	* @param uuid of the state to delete
	* @param Merge snapshot or pure delete
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmDeleteSnapshot(PRL_CONST_STR sVmUuid, PRL_CONST_STR sSnapshotUuid, const PRL_BOOL bMerge);

	/**
	* Sets new snapshot name and description
	* @param VM uuid
	* @param uuid of the state to switch to
	* @param new name
	* @param new description
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmUpdateSnapshotData(PRL_CONST_STR sVmUuid, PRL_CONST_STR sSnapshotUuid,
											   PRL_CONST_STR sNewName, PRL_CONST_STR sNewDescription);

	/**
	 * Performs clone VM request to the server
	 * @param VM UUID
	 * @param new VM name
	 * @param path to new VM configuration file
	 * @param flags (PACF_CLONEVM_TO_TEMPLATE, PACF_CLONEVM_CHANGE_SID)
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdDirVmClone(
			PRL_CONST_STR sVmUuid,
			PRL_CONST_STR sNewVmName,
			PRL_CONST_STR sNewVmUuid,
			PRL_CONST_STR sNewVmConfigPath,
			PRL_UINT32 nFlags
	);

	/**
	 * Performs migrate VM request to the server
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target VM name
	 * @param target VM home path
	 * @param VM migration flags
	 * @param reserved flags
	 * @param force operation flag
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdDirVmMigrate(
			PRL_CONST_STR sVmUuid,
			PRL_CONST_STR sTargetServerHostname,
			PRL_UINT32 nTargetServerPort,
			PRL_CONST_STR sTargetServerSessionUuid,
			PRL_CONST_STR sTargetVmName,
			PRL_CONST_STR sTargetVmHomePath,
			PRL_UINT32 nMigrationFlags,
			PRL_UINT32 nReservedFlags,
			PRL_BOOL bForceOperation
	);

	/**
	 * Performs VM migration procedure cancelation request to the server
	 * @param VM UUID
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmMigrateCancel(
			PRL_CONST_STR sVmUuid
	);

	/**
	 * Deletes VM from server
	 * @param valid VM configuration
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdDirVmDelete(PRL_CONST_STR sVmUuid,const QStringList & lstDevices);

	/**
	* Get problem Vm report from server
	* @param valid VM configuration
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmGetProblemReport(PRL_CONST_STR sVmUuid);

	/**
	* Get packed problem Vm report from server
	* @param valid VM configuration
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmGetPackedProblemReport(PRL_CONST_STR sVmUuid,
        PRL_UINT32 nFlags);

	/**
	 * Gets VM current state
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdGetVmInfo(PRL_CONST_STR sVmUuid);

	/**
	 * Extracts VM Tools current state
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdGetVmToolsInfo(PRL_CONST_STR sVmUuid);

	/**
	 * Updates VM configuration
	 * @param VM uuid
	 * @param nFlags
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmGetConfig(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	 * Perfroms request to VM process on opening guest OS user session
	 * @param VM uuid
	 * @param user name
	 * @param user password
	 * @param flags bitset
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmLoginInGuest(PRL_CONST_STR sVmUuid, PRL_CONST_STR sUserName, PRL_CONST_STR sUserPassword,
											PRL_UINT32 nFlags);

	/**
	 * Starts VM VNC server
	 * @param VM uuid
	 * @param flags bitset
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmStartVncServer(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	 * Stops VM VNC server
	 * @param VM uuid
	 * @param flags bitset
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmStopVncServer(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	 * Sends request of VM guest system resources usage statistics
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmGetStatistics(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	 * Performs guest OS statistics subscription request to VM
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmSubscribeToGuestStatistics(PRL_CONST_STR sVmUuid);

	/**
	 * Performs guest OS statistics unsubscription request to VM
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmUnsubscribeFromGuestStatistics(PRL_CONST_STR sVmUuid);

	/**
	 * Perform VM registration on server.
	 *
	 * @param pointer to registering VM
	 * @param path to VM parent dir path
	 * @param flags set
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr RegVm(const PrlHandleVmPtr &pVm, PRL_CONST_STR sVmParentPath, PRL_UINT32 nFlags);

	/**
	 * Unregisters VM from dispatcher
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdDirUnregVm(PRL_CONST_STR sVmUuid);

	/**
	 * Restorss VM config
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdDirRestoreVm(PRL_CONST_STR sVmUuid);

	/**
	 * Starts transaction of editing VM configuration procedure
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdDirVmEditBegin(PRL_CONST_STR sVmUuid);

	/**
	 * Commits VM configuration changes
	 * @param VM configuration
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdDirVmEditCommit(PRL_CONST_STR sVmConfiguration, PRL_UINT32 nFlags);

	/**
	 * Performs operation of creation floppy image for unattended guest OS installation
	 * @param VM UUID
	 * @param guest OS distro type
	 * @param predefined serial key
	 * @param predefined user name
	 * @param predefined company name
	 */
	PrlHandleJobPtr DspCmdVmCreateUnattendedFloppy(PRL_CONST_STR sVmUuid, PRL_GUEST_DISTRO_VERSION_ENUM nGuestDistroType, PRL_CONST_STR sKey, PRL_CONST_STR sUser, PRL_CONST_STR sCompany);

	/**
	* Initiates sending notifications about devices states by VM
	* @param VM uuid
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmInitiateDevStateNotifications(PRL_CONST_STR sVmUuid);

	/**
	* Performs VM security update operation
	* @param VM uuid
	* @param VM security info
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmUpdateSecurity(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmSecurity);

	/**
	 * Validates VM configuration
	 * @param VM configuration
	 * @param Section identification in VM configuration
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmSectionValidateConfig(PRL_CONST_STR sVmConfiguration, PRL_VM_CONFIG_SECTIONS nSection);

	/**
	* Send request to remote side on set VM protection
	* @param VM UUID
	* @param password for VM protection
	* @param reserved
	*/
	PrlHandleJobPtr DspCmdVmSetProtection( PRL_CONST_STR sVmUuid,
										 PRL_CONST_STR sProtectionPassword, PRL_UINT32 nReserved );

	/**
	* Send request to remote side on remove VM protection
	* @param VM UUID
	* @param password for VM protection
	* @param reserved
	*/
	PrlHandleJobPtr DspCmdVmRemoveProtection( PRL_CONST_STR sVmUuid,
											PRL_CONST_STR sProtectionPassword, PRL_UINT32 nReserved );

	/**
	 * Begin some utility installation process
	 * @param VM uuid
	 * @param utility string id
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmInstallUtility(PRL_CONST_STR sVmUuid, PRL_CONST_STR strId);

	/**
	 * Begin the Tools installation process
	 * @param VM uuid
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmInstallTools(PRL_CONST_STR sVmUuid);

	/**
	 * Starts optimizing VM disk
	 * @param VM uuid
	 * @param Bit mask of really hard disk devices (little endian order)
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmCompact(PRL_CONST_STR sVmUuid, PRL_UINT32 uMask, PRL_UINT32 nFlags);

    /**
     * Cancels optimizing VM disk
     * @param VM uuid
     * @return pointer to generated job instance
     */
    PrlHandleJobPtr DspCmdVmCancelCompact(PRL_CONST_STR sVmUuid);

	/**
	 * Starts conversion VM disk
	 * @param VM uuid
	 * @param Bit mask of really hard disk devices (little endian order)
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmConvertDisks(PRL_CONST_STR sVmUuid, PRL_UINT32 uMask, PRL_UINT32 nFlags);

    /**
     * Performs change machine SID operation to VM
     * @param VM uuid
	 * @params flags bitset
     * @return pointer to generated job instance
     */
    PrlHandleJobPtr DspCmdVmChangeSid(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

    /**
     * Performs reset uptime counter operation to VM
     * @param VM uuid
	 * @params flags bitset
     * @return pointer to generated job instance
     */
    PrlHandleJobPtr DspCmdVmResetUptime(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	 * Perfroms request to VM process on authorizing user through guest OS security database
	 * @param VM uuid
	 * @param user name
	 * @param user password
	 * @param flags bitset
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmAuthWithGuestSecurityDb(PRL_CONST_STR sVmUuid, PRL_CONST_STR sUserName, PRL_CONST_STR sUserPassword,
											PRL_UINT32 nFlags);

	/**
	 * Mount VM hdd's
	 * @param VM uuid
	 * @param
	 * @param reserved parameter
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmMount(PRL_CONST_STR sVmUuid, PRL_CONST_STR sMntPath, PRL_UINT32 nFlags);

	/**
	 * Unount VM hdd's
	 * @param VM uuid
	 * @param reserved parameter
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmUmount(PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	 * Move bundle of virtual machine or private area of container.
	 * @param VM UUID
	 * @param new home path
	 * @param flags
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdDirVmMove(
			PRL_CONST_STR sVmUuid,
			PRL_CONST_STR sNewHomePath,
			PRL_UINT32 nFlags
	);

	/**
	* Perfroms request on logout from VM guest session
	* @param VM UUID
	* @param VM guest session UUID
	* @param operation flags bitset
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmGuestLogout(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmGuestUuid, PRL_UINT32 nFlags);

	/**
	* Perfroms request on execute a program at VM guest session
	* @param VM UUID
	* @param VM guest session UUID
	* @param program name
	* @param list of program arguments
	* @param list of environment variables
	* @param operation flags bitset
	* @param file descriptor to the standard input
	* @param file descriptor to the standard output
	* @param file descriptor to the standard errors output
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmGuestRunProgram(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmGuestUuid, PRL_CONST_STR sProgramName,
										const QStringList &lstArgs, const QStringList &lstEnvs, PRL_UINT32 nFlags,
										PRL_FILE_DESC nStdin, PRL_FILE_DESC nStdout, PRL_FILE_DESC nStderr);

	/**
	* Perfroms request on getting network settings from guest OS
	* @param VM UUID
	* @param VM guest session UUID
	* @param operation flags bitset
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmGuestGetNetworkSettings(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmGuestUuid, PRL_UINT32 nFlags);

	/**
	* Perfroms request on reset guest user account password
	* @param VM UUID
	* @param VM guest session UUID
	* @param user login name
	* @param user password
	* @param operation flags bitset
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdVmGuestSetUserPasswd(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmGuestUuid, PRL_CONST_STR sUserLoginName,
										const char *sUserPasswd, PRL_UINT32 nFlags);

	/**
	 * Connects VM device
	 * @param VM uuid
	 * @param virtual device type
	 * @param virtual device index
	 * @param virtual device xml config
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmDevConnect(PRL_CONST_STR sVmUuid, PRL_DEVICE_TYPE nDevType, PRL_INT32 nDevIndex,
																			PRL_CONST_STR sDeviceConfig);
	/**
	 * Disconnects VM device
	 * @param VM uuid
	 * @param virtual device type
	 * @param virtual device index
	 * @param virtual device xml config
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmDevDisconnect(PRL_CONST_STR sVmUuid, PRL_DEVICE_TYPE nDevType, PRL_INT32 nDevIndex,
																			 PRL_CONST_STR sDeviceConfig);

	/**
	 * Creates VM device image on server side
	 * @param VM UUID
	 * @param VM device configuration
	 * @param recreate image sign
	 * @param sign whether non interactive mode using
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdDirCreateImage(PRL_CONST_STR sVmUuid, PRL_CONST_STR sVmDeviceConfiguration, PRL_BOOL bRecreateIsAllowed, PRL_BOOL bNonInteractiveMode);

	/**
	 * Copies image of specified VM device to custom location
	 * @param pointer to VM device object
	 * @param new image short name
	 * @param new location
	 * @param reserved
	 * return pointer to job object
	 */
	PrlHandleJobPtr DspCmdDirCopyImage( PRL_CONST_STR sVmUuid,
										PRL_CONST_STR sVmDeviceConfiguration,
										PRL_CONST_STR sNewImageName,
										PRL_CONST_STR sTargetPath,
										PRL_UINT32 nFlags );

	PrlHandleJobPtr DspCmdVmResizeDiskImage(PRL_CONST_STR sVmUuid, const QString &sDiskImage,
								PRL_UINT32 nNewSize, PRL_UINT32 nFlags);

	PrlHandleJobPtr DspCmdVmCaptureScreen(PRL_CONST_STR sVmUuid, PRL_UINT32 nWidth,
								PRL_UINT32 nHeight, PRL_UINT32 nFlags);

	/**
	 * Commits changes in the VM encryption settings
	 *
	 * @param VM configuration with changed encryption settings
	 * @return pointer to generated job instance
	 */
	PrlHandleJobPtr DspCmdVmCommitEncryption(PRL_CONST_STR sVmConfiguration, PRL_UINT32 nFlags);

	PrlHandleJobPtr DspCmdCtReinstall(PRL_CONST_STR sVmUuid,
			PRL_CONST_STR sOs, PRL_UINT32 nFlags);
};

#endif // __VIRTUOZZO_HANDLE_SERVER_VM_H__
