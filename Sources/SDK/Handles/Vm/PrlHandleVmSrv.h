/*
 * PrlHandleVmSrv.h
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


#ifndef __PARALLELS_HANDLE_VM_SRV_H__
#define __PARALLELS_HANDLE_VM_SRV_H__

#include "PrlHandleVm.h"


/**
 * Asynch virtual machine handle interface.
 */
class PrlHandleVmSrv : public PrlHandleVm
{
public:
    /**
     * Perform send perfomance statistics subscription request to the remote server.
     *
     * @return pointer to the class that abstracts the job process
     */
    PrlHandleJobPtr SubscribePerfStats(PRL_CONST_STR filter) ;

    /**
     * Perform send unsubscription to host statistics request to the remote server.
     *
     * @return pointer to the class that abstracts the job process
     */
    PrlHandleJobPtr UnsubscribePerfStats() ;

    /**
     * Perform send perfomance statistics subscription request to the remote server.
     *
     * @return pointer to the class that abstracts the job process
     */
    PrlHandleJobPtr GetPerfStats(PRL_CONST_STR filter) ;

	/**
	 * Deletes VM from server
	 */
	PrlHandleJobPtr Delete(const QStringList & lstDevices);
	/**
	* Get report from VM
	*/
	PrlHandleJobPtr GetProblemReport();
	/**
	* Get packed report from VM
	*/
	PrlHandleJobPtr GetPackedProblemReport( PRL_UINT32 nFlags );
    /**
    * Send Problem Report by the means of a server
    */
    PrlHandleJobPtr SendProblemReport( PRL_HANDLE hProblemReport, PRL_UINT32 nFlags );
	/**
	 * Starts VM
	 */
	PrlHandleJobPtr Start();
	/**
	 * Starts VM VNC server
	 * @param flags bitset
	 */
	PrlHandleJobPtr StartVncServer(PRL_UINT32 nFlags);
	/**
	 * Stops VM VNC server
	 * @param flags bitset
	 */
	PrlHandleJobPtr StopVncServer(PRL_UINT32 nFlags);
	/**
	 * Tries to lock VM
	 * @param flags bitset
	 */
	PrlHandleJobPtr Lock(PRL_UINT32 nFlags);
	/**
	 * Tries to unlock VM
	 * @param flags bitset
	 */
	PrlHandleJobPtr Unlock(PRL_UINT32 nFlags);
	/**
	 * Starts VM in the specified mode
	 */
	PrlHandleJobPtr StartEx(PRL_UINT32 nStartMode, PRL_UINT32 nReserved);
	/**
	 * Mount VM hdd's
	 */
	PrlHandleJobPtr Mount(PRL_CONST_STR sMntPath, PRL_UINT32 nFlags);

	/**
	 * Unmount VM hdd's
	 */
	PrlHandleJobPtr Umount(PRL_UINT32 nFlags);

	/**
	 * Performs to VM process request on opening guest OS session
	 */
	PrlHandleJobPtr LoginInGuest(PRL_CONST_STR sUserName, PRL_CONST_STR sUserPassword, PRL_UINT32 nFlags);
	/**
	 * Performs to VM process request on authorizing user through guest OS security database
	 */
	PrlHandleJobPtr AuthWithGuestSecurityDb(PRL_CONST_STR sUserName, PRL_CONST_STR sUserPassword, PRL_UINT32 nFlags);
	/**
	 * Restarts VM guest OS
	 */
	PrlHandleJobPtr RestartGuest();
	/**
	 * Stops VM
	 * @param VM stop mode
	 */
	PrlHandleJobPtr Stop(PRL_UINT32 nStopMode, PRL_UINT32 nFlags);
	/**
	 * Pauses VM
	 * @param sign whether ACPI must be used
	 */
	PrlHandleJobPtr Pause(PRL_BOOL bAcpi);
	/**
	 * Suspends VM
	 */
	PrlHandleJobPtr Suspend();
	/**
	 * Retrieves suspended VM screen
	 */
	PrlHandleJobPtr GetSuspendedScreen();
	/**
	 * Drop suspendet VM state
	 */
	PrlHandleJobPtr DropSuspendedState();

	/**
	 * Resets VM
	 */
	PrlHandleJobPtr Reset();


	/**
	* Internal VM command
	*/
	PrlHandleJobPtr InternalCmd(PRL_CONST_STR sCmd, PRL_HANDLE hArgsList);

	/**
	 * Install utility
	 */
	PrlHandleJobPtr InstallUtility(PRL_CONST_STR strId);

	/**
	* Begin the Tools installation process
	*/
	PrlHandleJobPtr InstallTools();

	/**
	 * Start creation procedure of the new state
	 */
	PrlHandleJobPtr CreateSnapshot(PRL_CONST_STR sName,
			PRL_CONST_STR sDescription, PRL_UINT32 nFlags);

	/**
	 * Start switching to state procedure
	 *
	 * @param uuid of the state to switch to
	 */
	PrlHandleJobPtr SwitchToSnapshot(PRL_CONST_STR sSnapshotUuid, PRL_UINT32 nFlags);

	/**
	 * Start delete state procedure
	 *
	 * @param uuid of the state to delete
	 * @param delete childs of the snapshot
	 */
	PrlHandleJobPtr DeleteSnapshot(PRL_CONST_STR sSnapshotUuid, const PRL_BOOL bChild);

	/**
	* Get snapshots tree
	*/
	PrlHandleJobPtr GetSnapshotsTree(PRL_UINT32 nFlags);

	/**
	* Sets new snapshot name and description
	*
	* @param uuid of the state to switch to
	* @param new name
	* @param new description
	*/
	PrlHandleJobPtr UpdateSnapshotData(PRL_CONST_STR sSnapshotUuid,
		PRL_CONST_STR sNewName,
		PRL_CONST_STR sNewDescription);

	/**
	* Run the Compressor process
	*/
	PrlHandleJobPtr RunCompressor();

	/**
	* Cancel the Compressor process
	*/
	PrlHandleJobPtr CancelCompressor();

	/**
	* Performs operation of changing Windows guest OS machine SID
	* @param flags bitset
    * @return pointer to generated job instance
	*/
	PrlHandleJobPtr ChangeMachineSid(PRL_UINT32 nFlags);

	/**
	* Performs operation of resetting VM uptime
	* @param flags bitset
    * @return pointer to generated job instance
	*/
	PrlHandleJobPtr ResetUptime(PRL_UINT32 nFlags);

	/**
	 * Performs guest OS statistics subscription request to VM
	 */
	PrlHandleJobPtr SubscribeToGuestStatistics();
	/**
	 * Performs guest OS statistics unsubscription request to VM
	 */
	PrlHandleJobPtr UnsubscribeFromGuestStatistics();
	/**
	 * Starts transaction of editing VM configuration
	 */
	PrlHandleJobPtr BeginEdit();
	/**
	 * Commits changes of VM configuration
	 */
	PrlHandleJobPtr Commit( PRL_UINT32 nFlags );
	/**
	 * Creates image for specified VM device
	 * @param pointer to VM device object
	 * @param sign whether non interactive mode using
	 * return pointer to job object
	 */
	PrlHandleJobPtr CreateImage(const PrlHandleVmDevicePtr &pVmDevice, PRL_BOOL bRecreateIsAllowed, PRL_BOOL bNonInteractiveMode);
	/**
	 * Copies image of specified VM device to custom location
	 * @param pointer to VM device object
	 * @param new image short name
	 * @param new location
	 * @param reserved
	 * return pointer to job object
	 */
	PrlHandleJobPtr CopyImage(  const PrlHandleVmDevicePtr &pVmDevice,
								PRL_CONST_STR sNewImageName,
								PRL_CONST_STR sTargetPath,
								PRL_UINT32 nFlags );

	PrlHandleJobPtr ResizeDiskImage(const QString &diskImage, PRL_UINT32 nNewSize, PRL_UINT32 nFlags);
	/**
	 * Sends update VM configuration request
	 */
	PrlHandleJobPtr RefreshConfig(PRL_UINT32 nFlags);
	/**
	 * Sends request of VM guest system resources usage statistics
	 */
	PrlHandleJobPtr GetStatistics(PRL_UINT32 nFlags);
	/**
	 * Unregisters VM from dispatcher
	 */
	PrlHandleJobPtr Unreg();
	/**
	 * Perform VM restoration
	 */
	PrlHandleJobPtr Restore();
	/**
	 * Clones VM
	 *
	 * @param new VM name
	 * @param new VM uuid
	 * @param path to new VM configuration
	 * @param clone flags (PACF_CLONEVM_TO_TEMPLATE, PACF_CLONEVM_CHANGE_SID)
	 * return pointer to job object
	 */
	PrlHandleJobPtr Clone(
		PRL_CONST_STR new_vm_name,
		PRL_CONST_STR new_vm_uuid,
		PRL_CONST_STR new_vm_config_path,
		PRL_UINT32 nFlags);
	/**
	 * Migrates VM
	 *
	 * @param pointer to the target server object
	 * @param target server VM name
	 * @param target server VM home path
	 * @param VM migration flags
	 * @param reserved flags
	 * @param force operation flag
	 * return pointer to job object
	 */
	PrlHandleJobPtr Migrate(
		PrlHandleServerPtr pTargetServer,
		PRL_CONST_STR sTargetVmName,
		PRL_CONST_STR sTargetVmHomePath,
		PRL_UINT32 nMigrationFlags,
		PRL_UINT32 nReservedFlags,
		PRL_BOOL bForceOperation
	);
	/**
	 * Migrates VM
	 *
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target server VM name
	 * @param target server VM home path
	 * @param VM migration flags
	 * @param reserved flags
	 * @param force operation flag
	 * return pointer to job object
	 */
	PrlHandleJobPtr Migrate(
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
	 * Performs VM migration procedure cancelation
	 * return pointer to job object
	 */
	PrlHandleJobPtr MigrateCancel();

	/**
	 * Generates VM device image filename
	 *
	 * @param filename prefix
	 * @param filename suffix
	 * @param delimiter between prefix and generated index
	 */
	PrlHandleJobPtr GenerateVmDevFilename(
						PRL_CONST_STR sFilenamePrefix,
						PRL_CONST_STR sFilenameSuffix,
						PRL_CONST_STR sIndexDelimiter
						);

	/**
	 * Sends request of current VM state
	 */
	PrlHandleJobPtr GetState();
	/**
	 * Sends request of current VM Tools state
	 */
	PrlHandleJobPtr GetToolsState();

	/**
	 * Sends request on creation for VM unattended installation floppy image
	 * @param guest OS distro type
	 * @param predefined serial key
	 * @param predefined user name
	 * @param predefined company name
	 */
	PrlHandleJobPtr CreateUnattendedFloppy( PRL_GUEST_DISTRO_VERSION_ENUM nGuestDistroType, PRL_CONST_STR sKey, PRL_CONST_STR sUser, PRL_CONST_STR sCompany );
	/**
	* Initiates sending notifications about devices states by VM
	*/
	PrlHandleJobPtr InitiateDevStateNotifications();
	/**
	* Performs change VM permissions operation
	*/
	PrlHandleJobPtr UpdateSecurity(PrlHandleAccessRightsPtr pAccessRights);

	/**
	 * Validates VM configuration
	 */
	PrlHandleJobPtr ValidateConfig(PRL_VM_CONFIG_SECTIONS nSection);

 	/**
	* Start the process of virtual hard disk optimization
	*/
	PrlHandleJobPtr Compact(PRL_UINT32 uMask, PRL_UINT32 nFlags);

	/**
	 * Cancels the process of virtual hard disk optimization
	 */
	PrlHandleJobPtr CancelCompact();

 	/**
	* Start the process of virtual hard disk conversion
	*/
	PrlHandleJobPtr ConvertDisks(PRL_UINT32 uMask, PRL_UINT32 nFlags);

	/**
	* Update Vm Tools Section
	*/
	PrlHandleJobPtr UpdateToolsSection(PRL_CONST_STR sToolsSection);

	/**
	* Performs request on logout from VM guest session
	* @param VM guest session UUID
	* @param operation flags bitset
	* @return pointer to the generated job object
	*/
	PrlHandleJobPtr VmGuestLogout(const QString &sVmGuestUuid, PRL_UINT32 nFlags);

	/**
	* Performs request on execute a program at VM guest session
	* @param VM guest session UUID
	* @param program name
	* @param list of program arguments
	* @param list of environment variables
	* @param operation flags bitset
	* @param file descriptor pointing to the standard input
	* @param file descriptor pointing to the standard output
	* @param file descriptor pointing to the standard errors output
	* @return pointer to the generated job object
	*/
	PrlHandleJobPtr VmGuestRunProgram(const QString &sVmGuestUuid, PRL_CONST_STR sProgramName,
						const QStringList &lstArgs, const QStringList &lstEnvs, PRL_UINT32 nFlags,
						PRL_FILE_DESC nStdin, PRL_FILE_DESC nStdout, PRL_FILE_DESC nStderr);

	/**
	* Performs request on getting network settings from guest OS
	* @param VM guest session UUID
	* @param operation flags bitset
	* @return pointer to the generated job object
	*/
	PrlHandleJobPtr VmGuestGetNetworkSettings(const QString &sVmGuestUuid, PRL_UINT32 nFlags);

	/**
	* Performs request on reset guest user account password
	* @param VM guest session UUID
	* @param user login name
	* @param user password
	* @param operation flags bitset
	* @return pointer to the generated job object
	*/
	PrlHandleJobPtr VmGuestSetUserPasswd(const QString &sVmGuestUuid, PRL_CONST_STR sUserLoginName,
						PRL_CONST_STR sUserPasswd, PRL_UINT32 nFlags);

	PrlHandleJobPtr StoreValueByKey( PRL_CONST_STR sKey, PRL_CONST_STR sValue, PRL_UINT32 nFlags);

	/**
	* Performs request on set VM protection
	* @param password for VM protection
	* @param reserved
	* @return pointer to the generated job object
	*/
	PrlHandleJobPtr SetProtection( PRL_CONST_STR sProtectionPassword, PRL_UINT32 nReserved );

	/**
	* Performs request on remove VM protection
	* @param password for VM protection
	* @param reserved
	* @return pointer to the generated job object
	*/
	PrlHandleJobPtr RemoveProtection( PRL_CONST_STR sProtectionPassword, PRL_UINT32 nReserved );

	/**
	 * Move VM
	 *
	 * @param new path to home
	 * @param flags
	 * return pointer to job object
	 */
	PrlHandleJobPtr Move(PRL_CONST_STR sNewHomePath, PRL_UINT32 nFlags);

	/**
	 * Capture VM screen
	 *
	 * @param image's width in pixels
	 * @param image's height in pixels
	 * return pointer to job object
	 */
	PrlHandleJobPtr CaptureScreen(PRL_UINT32 nWidth, PRL_UINT32 nHeight, PRL_UINT32 nFlags);

	/**
	 * Commit changes in VM's encryption settings
	 *
	 * @param nFlags - encryption flags
	 * @return pointer to job object
	 */
	PrlHandleJobPtr CommitEncryption(PRL_UINT32 nFlags);

	/**
	 * Reinstall
	 *
	 * @param
	 * @param nFlags - encryption flags
	 * @return pointer to job object
	 */
	PrlHandleJobPtr Reinstall(PRL_CONST_STR sOs, PRL_UINT32 nFlags);
};

#endif // __PARALLELS_HANDLE_VM_SRV_H__
