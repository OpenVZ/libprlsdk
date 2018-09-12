/*
 * PrlCommandsFlags.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __PARALLELS_API_COMMANDS_FLAGS_H__
#define __PARALLELS_API_COMMANDS_FLAGS_H__

/* Security level enumeration. Defines levels of security that
   can be used in client/server communications.
   Type Parameters
   PSL_LOW_SECURITY :     Plain TCP/IP (no encryption).
   PSL_NORMAL_SECURITY :  Most important data is sent and
                          received using SSL over TCP/IP (user
                          credentials during login, guest OS
                          clipboard, etc.) Other data is sent and
                          receive using plain TCP/IP with no
                          encryption.
   PSL_HIGH_SECURITY :    All of the data is sent and received
                          using SSL.                              */
typedef enum _PRL_SECURITY_LEVEL
{
	PSL_LOW_SECURITY    = 0,
	PSL_NORMAL_SECURITY = 1<<0,
	PSL_HIGH_SECURITY   = 1<<1

} PRL_SECURITY_LEVEL;
typedef PRL_SECURITY_LEVEL* PRL_SECURITY_LEVEL_PTR;

/*
 Common operations flags set. Please do not override values
*/
typedef enum _PRL_API_COMMAND_FLAGS
{
	/* Client connection security mode */
	PACF_NORMAL_SECURITY               = PSL_NORMAL_SECURITY,
	PACF_HIGH_SECURITY                 = PSL_HIGH_SECURITY,
	/* Session interactive mode */
	PACF_NON_INTERACTIVE_MODE          = 1<<2,
	/* Cancel long-term operation by session disconnect */
	PACF_CANCEL_TASK_ON_END_SESSION = 1<<3,
	/**
	 * Interval from here to PACF_LAST reserved for further
	 * purposes. Please do not override this interval values
	 * with own flags
	 */
	PACF_MAX                           = 10,
	PACF_LAST                          = 1<<PACF_MAX
} PRL_API_COMMAND_FLAGS;
typedef PRL_API_COMMAND_FLAGS* PRL_API_COMMAND_FLAGS_PTR;

/* PrlSrv_RegisterVmEx command flags set */
typedef enum _PRL_REGISTER_VM_FLAGS
{
	PRVF_REGENERATE_VM_UUID			= 1<<(PACF_MAX+1),
	PRVF_KEEP_OTHERS_PERMISSIONS	= 1<<(PACF_MAX+2),
	PRCF_FORCE						= 1<<(PACF_MAX+3),
	PRVF_REGENERATE_SRC_VM_UUID		= 1<<(PACF_MAX+4),
	PRVF_IGNORE_HA_CLUSTER			= 1<<(PACF_MAX+5),
	PRCF_UNREG_PRESERVE			= 1<<(PACF_MAX+6),
} PRL_REGISTER_VM_FLAGS;
typedef PRL_REGISTER_VM_FLAGS* PRL_REGISTER_VM_FLAGS_PTR;

/* PrlVm_CloneEx command flags set */
/*
	PCVF_DETACH_EXTERNAL_VIRTUAL_HDD
		- to remove link to Virtual Hdd which located outside of VM.pvm directory from cloned vm configuration.
*/
typedef enum _PRL_CLONE_VM_FLAGS
{
	PCVF_CLONE_TO_TEMPLATE	 = 1<<(PACF_MAX+1),
	PCVF_CHANGE_SID		     = 1<<(PACF_MAX+2),
	PCVF_LINKED_CLONE        = 1<<(PACF_MAX+3),
	PCVF_IMPORT_BOOT_CAMP    = 1<<(PACF_MAX+4),
	PCVF_DETACH_EXTERNAL_VIRTUAL_HDD    = 1<<(PACF_MAX+5),
} PRL_CLONE_VM_FLAGS;
typedef PRL_CLONE_VM_FLAGS* PRL_CLONE_VM_FLAGS_PTR;

/* PrlSrv_Register3rdPartyVm command flags set */

typedef enum _PRL_REGISTER_3PARTY_VM_FLAGS
{
	PR3F_ALLOW_UNKNOWN_OS	= 1<<(PACF_MAX+1),
	PR3F_CREATE_SHADOW_VM	= 1<<(PACF_MAX+2),
} PRL_REGISTER_3PARTY_VM_FLAGS;
typedef PRL_REGISTER_3PARTY_VM_FLAGS* PRL_REGISTER_3PARTY_VM_FLAGS_PTR;

/* PrlVmDev_ResizeImage command flags set */
typedef enum _PRL_RESIZE_IMAGE_FLAGS
{
	PRIF_RESIZE_LAST_PARTITION	= 1<<(PACF_MAX+1),
	PRIF_DISK_INFO		        = 1<<(PACF_MAX+2),
	PRIF_RESIZE_OFFLINE		= 1<<(PACF_MAX+3),
} PRL_RESIZE_IMAGE_FLAGS;
typedef PRL_RESIZE_IMAGE_FLAGS* PRL_RESIZE_IMAGE_FLAGS_PTR;

/* VM migration functionality flags set
   Type Parameters
   PVMSL_LOW_SECURITY
   PVMSL_NORMAL_SECURITY
   PVMSL_HIGH_SECURITY
   PVMT_COLD_MIGRATION		: VM migration with interruption of VM services (VM will be stopped
                                  on migration time).
   PVMT_WARM_MIGRATION		: VM services will have quite long downtime during migration but
                                  won't be interrupted.
   PVMT_HOT_MIGRATION		: VM services will have short downtime during migration without any
                                  interruption.
   PVMT_DONT_RESUME_VM      : do not start running VM after migration
   PVMT_CLONE_MODE          : preserves source VM
   PVMT_SWITCH_TEMPLATE     : applable just in clone mode - lets to clone remotely VM to template
   PVMT_CHANGE_SID          : for Windows VMs/containers lets to change SID after remote clone
   PVMT_IGNORE_EXISTING_BUNDLE : do not fail if Vm bundle exist. For clone Vm on mounted iSCSI storage
   PVMT_UNCOMPRESSED        : do not compress data during migration
   PVMT_DIRECT_DATA_CONNECTION : do not use tunnel for migration
   PVMT_DONT_CREATE_DISK       : don't create disk file on target host
*/
typedef enum _PRL_VM_MIGRATION_FLAGS
{
	PVMSL_LOW_SECURITY	     = PSL_LOW_SECURITY,
	PVMSL_NORMAL_SECURITY	 = PACF_NORMAL_SECURITY,
	PVMSL_HIGH_SECURITY	     = PACF_HIGH_SECURITY,
	PVMT_COLD_MIGRATION	     = 1<<(PACF_MAX+1),
	PVMT_WARM_MIGRATION	     = 1<<(PACF_MAX+2),
	PVMT_HOT_MIGRATION	     = 1<<(PACF_MAX+3),
	PVM_DONT_RESUME_VM	     = 1<<(PACF_MAX+4),
	PVMT_DONT_RESUME_VM      = PVM_DONT_RESUME_VM,
	PVMT_CLONE_MODE          = 1<<(PACF_MAX+5),
	PVMT_SWITCH_TEMPLATE     = 1<<(PACF_MAX+6),
	PVMT_CHANGE_SID          = 1<<(PACF_MAX+7),
	PVMT_IGNORE_EXISTING_BUNDLE  = 1<<(PACF_MAX+8),
	PVMT_REMOVE_SOURCE_BUNDLE  = 1<<(PACF_MAX+9),
	PVMT_SEND_DISK_MAP = 1<<(PACF_MAX+10),
	PVMT_UNCOMPRESSED	= 1<<(PACF_MAX+11),
	PVMT_DIRECT_DATA_CONNECTION  = 1<<(PACF_MAX+12),
	PVMT_DONT_CREATE_DISK        = 1<<(PACF_MAX+13),
	PVM_UPDATE_MODE		= 1<<(PACF_MAX+14),
	PVM_FORCE		= 1<<(PACF_MAX+15),
} PRL_VM_MIGRATION_FLAGS;

/* VM backup functionality flags set
   Type Parameters
   PBSL_LOW_SECURITY
   PBSL_NORMAL_SECURITY
   PBSL_HIGH_SECURITY
   PBT_FULL		: full backup
   PBT_INCREMENTAL	: incremental backup
   PBT_DIFFERENTAL	: differental backup
   PBT_RESTORE_TO_COPY	: change Vm name/uuid/MAC address on restore
   PBT_UNCOMPRESSED	: uncompressed backup
   PBT_VM		: for virtual mashines
   PBT_CT		: for containers
   PBT_BACKUP_ID: get information about a certain backup
   PBT_CHAIN	: get information about the whole backup chain for a certain backup
   PBT_KEEP_CHAIN	: preserve the rest of the backup chain on backup removal
   PBT_DIRECT_DATA_CONNECTION  : disables tunneling for backup (a laissez-fair
                          data transfer)
*/
typedef enum _PRL_VM_BACKUP_FLAGS
{
	PBSL_LOW_SECURITY	  = PSL_LOW_SECURITY,
	PBSL_NORMAL_SECURITY  = PACF_NORMAL_SECURITY,
	PBSL_HIGH_SECURITY	  = PACF_HIGH_SECURITY,
	PBT_FULL		      = 1<<(PACF_MAX+1),
	PBT_INCREMENTAL		  = 1<<(PACF_MAX+2),
	PBT_DIFFERENTIAL		  = 1<<(PACF_MAX+3),
	PBT_RESTORE_TO_COPY	= 1<<(PACF_MAX+4),
	PBT_IGNORE_NOT_EXISTS	= 1<<(PACF_MAX+5),
	PBT_UNCOMPRESSED	= 1<<(PACF_MAX+6),
	PBT_VM			= 1<<(PACF_MAX+7),
	PBT_CT			= 1<<(PACF_MAX+8),
	PBT_BACKUP_ID	= 1<<(PACF_MAX+9),
	PBT_CHAIN		= 1<<(PACF_MAX+10),
	PBT_KEEP_CHAIN		= 1<<(PACF_MAX+11),
	PBT_DIRECT_DATA_CONNECTION = 1<<(PACF_MAX+12),
} PRL_VM_BACKUP_FLAGS;

/* App execution result extraction moderators:
    PFD_STDOUT      : request stdout of executed in guest OS app
    PFD_STDERR      : request stderr of executed in guest OS app
*/
typedef enum _PRL_VM_EXEC_APP_FILE_DESCRIPTOR
{
	PFD_STDOUT				= 1<<(PACF_MAX+1),
	PFD_STDERR				= 1<<(PACF_MAX+2),
	PFD_STDIN				= 1<<(PACF_MAX+3),
	PFD_ALL					= (PFD_STDOUT | PFD_STDERR | PFD_STDIN)
} PRL_VM_EXEC_APP_FILE_DESCRIPTOR;
typedef PRL_VM_EXEC_APP_FILE_DESCRIPTOR* PRL_VM_EXEC_APP_FILE_DESCRIPTOR_PTR;

/* Run app in guest modes:
    PRPM_RUN_PROGRAM_AND_RETURN_IMMEDIATELY  : just run program in guest OS and do not wait result
    PRPM_RUN_PROGRAM_IN_SHELL  : run program via shell and in shell environment
    PRPM_RUN_PROGRAM_ENTER  : run shell in guest
*/
typedef enum _PRL_VM_EXEC_RUN_PROGRAM_MODES
{
	PRPM_RUN_PROGRAM_AND_RETURN_IMMEDIATELY		= 1<<(PACF_MAX+4),
	PRPM_RUN_PROGRAM_IN_SHELL			        = 1<<(PACF_MAX+5),
	PRPM_RUN_PROGRAM_ENTER				        = 1<<(PACF_MAX+6),
} PRL_VM_EXEC_RUN_PROGRAM_MODES;
typedef PRL_VM_EXEC_RUN_PROGRAM_MODES* PRL_VM_EXEC_RUN_PROGRAM_MODES_PTR;

/* VM start mode:
    PSM_VM_START             : start VM
	PSM_VM_SAFE_START        : start VM in the safe mode
	PSM_VM_START_FOR_COMPACT : start VM only for compacting disk operation
*/
typedef enum _PRL_VM_START_MODE
{
	PSM_VM_START				= 1<<(PACF_MAX+1),
	PSM_VM_SAFE_START			= 1<<(PACF_MAX+2),
	PSM_VM_START_FOR_COMPACT	= 1<<(PACF_MAX+3),
	// PSM_VM_START_LAST_ITEM has to be equal the last mode
	PSM_VM_START_LAST_ITEM	= PSM_VM_START_FOR_COMPACT,
} PRL_VM_START_MODE;
typedef PRL_VM_START_MODE* PRL_VM_START_MODE_PTR;

/* VM normal-mode start flags:
	PSM_VM_START_IGNORE_HA_CLUSTER : do not register VM on HA cluster in any case
					(for server mode only)
	PNSF_CT_SKIP_ARPDETECT : Ct specific parameter
*/
typedef enum _PRL_VM_NORMAL_START_FLAGS
{
//	PNSF_VM_START_IGNORE_HA_CLUSTER          = 1<<(PACF_MAX+1), This flag is unused
	PNSF_VM_START_WAIT			= 1<<(PACF_MAX+2),
	PNSF_CT_SKIP_ARPDETECT			= 1<<(PACF_MAX+3),
} PRL_VM_NORMAL_START_FLAGS;
typedef PRL_VM_NORMAL_START_FLAGS* PRL_VM_NORMAL_START_FLAGS_PTR;

/* Compact mode:
    PCM_COMPACT_WITH_HARD_DISKS_INFO : show question before compacting with size disks info
    PCM_HARD_DISKS_INFO              : just get disk info
	PCM_FULL_CLEAN_UP_VM             : compact all disks and delete all unneccessary files
*/
typedef enum _PRL_COMPACT_MODE
{
	PCM_COMPACT_WITH_HARD_DISKS_INFO	= 1 << (PACF_MAX+1),
	PCM_HARD_DISKS_INFO					= 1 << (PACF_MAX+2),
	PCM_FULL_CLEAN_UP_VM				= 1 << (PACF_MAX+3),
} PRL_COMPACT_MODE;
typedef PRL_COMPACT_MODE* PRL_COMPACT_MODE_PTR;

/**
* Guest Tools Utility command flags
*/
typedef enum _PRL_TOOLS_UTILITY_COMMAND_FLAG
{
       ///     @brief Flag is useless now, but placed here to show how and where it
       /// must be
       PTU_FLG_ZERO            = (1 << 0)
}
PRL_TOOLS_UTILITY_COMMAND_FLAG;
typedef PRL_TOOLS_UTILITY_COMMAND_FLAG* PRL_TOOLS_UTILITY_COMMAND_FLAG_PTR;

/** problem report flags functionality:
 *  PPRF_ADD_CLIENT_PART : specifies to add client information to problem report
 *  PPRF_ADD_SERVER_PART : specifies to add server information to problem report
 *  PPRF_DO_NOT_CREATE_HOST_SCREENSHOT : specifies do not create and add host screenshot to generating problem report
 */
typedef enum _PRL_PROBLEM_REPORT_FLAGS
{
	PPRF_ADD_CLIENT_PART				= 1 << (PACF_MAX+1),
	PPRF_ADD_SERVER_PART				= 1 << (PACF_MAX+2),
	PPRF_DO_NOT_CREATE_HOST_SCREENSHOT	= 1 << (PACF_MAX+3),
} PRL_PROBLEM_REPORT_FLAGS;
typedef PRL_PROBLEM_REPORT_FLAGS* PRL_PROBLEM_REPORT_FLAGS_PTR;

/** API initialization flags:
	* PrlApi_InitEx command flags set
	*  PAIF_USE_GRAPHIC_MODE : specifies create SDK session which has access to the graphic mode
 */
typedef enum _PRL_API_INIT_FLAGS
{
	PAIF_USE_GRAPHIC_MODE				=	1 << (PACF_MAX+1),
	PAIF_INIT_AS_APPSTORE_CLIENT= 1 << (PACF_MAX+2),
} PRL_API_INIT_FLAGS;
typedef PRL_API_INIT_FLAGS* PRL_API_INIT_FLAGS_PTR;

/* Virtual machine shutdown options.
 * PSF_FORCE : forcibly stop the virtual machine if shutdown timed out.
 * PSF_NOFORCE : do nothing if the virtual machine shutdown timed out.
 */
typedef enum _PRL_VM_STOP_FLAGS
{
        PSF_FORCE               = 1<<(PACF_MAX+1),
        PSF_NOFORCE             = 1<<(PACF_MAX+2),
} PRL_VM_STOP_FLAGS;

/* Install appliance options
 * PIAF_CANCEL : cancel appliance installation
 * PIAF_FORCE : do not cancel installation and begin a new one
 *              if the previous installation became corrupted outside.
 * PIAF_STOP : temporary stop appliance downloading
 */
typedef enum _PRL_INSTALL_APPLIANCE_FLAGS
{
	PIAF_CANCEL			= 1<<(PACF_MAX+1),
	PIAF_FORCE			= 1<<(PACF_MAX+2),
	PIAF_STOP			= 1<<(PACF_MAX+3),
} PRL_INSTALL_APPLIANCE_FLAGS;

/* Convert virtual disk
 * PCVD_CANCEL : cancel disk conversion
 * PCVD_TO_PLAIN_DISK : convert disk from expanding to plain disk
 * PCVD_TO_EXPANDING_DISK : convert disk from plain to expanding disk
 * PCVD_TO_SPLIT_DISK : convert disk from non-split to split disk
 * PCVD_TO_NON_SPLIT_DISK : convert disk from split to non-split disk
 * PCVD_MERGE_ALL_SNAPSHOTS : merge all disk snapshots
 */
typedef enum _PRL_CONVERT_VIRTUAL_DISK
{
	PCVD_CANCEL					= 1<<(PACF_MAX+1),
	PCVD_TO_PLAIN_DISK			= 1<<(PACF_MAX+2),
	PCVD_TO_EXPANDING_DISK		= 1<<(PACF_MAX+3),
	PCVD_TO_SPLIT_DISK			= 1<<(PACF_MAX+4),
	PCVD_TO_NON_SPLIT_DISK		= 1<<(PACF_MAX+5),
	PCVD_MERGE_ALL_SNAPSHOTS	= 1<<(PACF_MAX+6),
} PRL_CONVERT_VIRTUAL_DISK;

/**
 * Flags which determines peer client while login to dispatcher
 * and while attach to vm.
 */
typedef enum _PRL_IO_CLIENT_FLAGS
{
	PCF_ORIGINAL_CLIENT    = 1<<(PACF_MAX+1), /** Client with all features */
	PCF_LIGHTWEIGHT_CLIENT = 1<<(PACF_MAX+2), /** Lightweight client (iPhone/iPad) which
												  does not subscribe on heavy packages */
	PCF_TRUSTED_CHANNEL    = 1<<(PACF_MAX+3), /** Login over trusted channel
 												  (no need in username/password)*/

} PRL_IO_CLIENT_FLAGS;

/* Store value by specified keys on server options.
 * PSV_CALC_BOOT_CAMP : calculate VM size with Boot Camp disks
 */
typedef enum _PRL_VM_STORE_VALUE_FLAGS
{
	PSV_CALC_BOOT_CAMP_SIZE		= 1<<(PACF_MAX+1),
} PRL_VM_STORE_VALUE_FLAGS;

/**
 * iSCSI storage management flags
 */
typedef enum _PRL_ISCSI_STORAGE_CMD_FLAGS
{
	PISCSI_STORAGE_CREATE		= 1<<(PACF_MAX+1), /** To create storage on register */
	PISCSI_STORAGE_REMOVE		= 1<<(PACF_MAX+2), /** To remove storage on unregister */
	PISCSI_STORAGE_MOUNT_RDONLY	= 1<<(PACF_MAX+3), /** To mount storage by read-only */
	PISCSI_STORAGE_REMOUNT		= 1<<(PACF_MAX+4), /** To remount storage */
	PISCSI_STORAGE_MOUNT		= 1<<(PACF_MAX+5), /** To mount storage on register */

} PRL_ISCSI_STORAGE_CMD_FLAGS;

/**
 * iSCSI storage filesystem type
 */
typedef enum _PRL_ISCSI_STORAGE_FSTYPE_FLAGS
{
	PISCSI_STORAGE_EXT3    = 1<<(PACF_MAX+1),
	PISCSI_STORAGE_EXT4    = 1<<(PACF_MAX+2),
	PISCSI_STORAGE_NTFS    = 1<<(PACF_MAX+3),

} PRL_ISCSI_STORAGE_FSTYPE_FLAGS;

typedef enum _PRL_VM_TYPE_FLAGS
{
	PVTF_VM          = 1<<(PACF_MAX+1),
	PVTF_CT          = 1<<(PACF_MAX+2),
} PRL_VM_TYPE_FLAGS;

/**
* PrlVm_GetSnapshotsTreeEx flags
*/
typedef enum _PRL_GET_SNAPSHOT_TREE_FLAGS
{
	PGST_FULL_SIZE_SCREENSHOTS   = 1<<(PACF_MAX+1), /* return screenshots in full size */
	PGST_WITHOUT_SCREENSHOTS   = 1<<(PACF_MAX+2), /* fast way to request snapshots tree */
} PRL_GET_SNAPSHOT_TREE_FLAGS;

/* VM snapshot creation flags set
   Type Parameters
   PCSF_DISK_ONLY - to snapshot hdd's only for running Vm
   PCSF_BACKUP - create snapshot for backup
*/
typedef enum _PRL_CREATE_SNAPSHOT_FLAGS
{
	PCSF_DISK_ONLY		= 1 << (PACF_MAX+1), // internal flag
	PCSF_BACKUP		= 1 << (PACF_MAX+2), // internal flag
} PRL_CREATE_SNAPSHOT_FLAGS;

/**
* PrlVm_SwitchSnapshotEx flags
*/
typedef enum _PRL_SWITCH_SNAPSHOT_FLAGS
{
	PSSF_SKIP_RESUME	= 1<<(PACF_MAX+1), /* skip resume stage for Vm in the
						    * running/paused state in the snapshot
						    */
} PRL_SWITCH_SNAPSHOT_FLAGS;

/**
* PrlVm_DeleteSnapshotEx flags
   PDSF_BACKUP - delete snapshot for backup
*/
typedef enum _PRL_DELETE_SNAPSHOT_FLAGS
{
	PDSF_SKIP_SPACE_CHECK   = 1<<(PACF_MAX+1), /* skip space check */
	PDSF_BACKUP		= 1 << (PACF_MAX+2), // internal flag
	PDSF_BACKUP_MAP		= 1 << (PACF_MAX+3), // internal flag
} PRL_DELETE_SNAPSHOT_FLAGS;

/* copy of CT templates functionality flags set
   Type Parameters
   PCTMP_LOW_SECURITY
   PCTMP_NORMAL_SECURITY
   PCTMP_HIGH_SECURITY
   PCTMPL_FORCE		: force template copy
*/
typedef enum _PRL_COPY_CT_TEMPLATE_FLAGS
{
	PCTMP_LOW_SECURITY		= PSL_LOW_SECURITY,
	PCTMP_NORMAL_SECURITY		= PACF_NORMAL_SECURITY,
	PCTMP_HIGH_SECURITY		= PACF_HIGH_SECURITY,
	PCTMPL_FORCE			= 1<<(PACF_MAX+1),
} PRL_COPY_CT_TEMPLATE_FLAGS;

/**
* PrlVm_CommitEx flags
*/
typedef enum _PRL_VM_COMMIT_FLAGS
{
	PVCF_DESTROY_HDD_BUNDLE		= 1<<(PACF_MAX+1), /* specifies to destroy HDD bundle
							physically on hard disk remove operation */
	PVCF_WAIT_FOR_APPLY		= 1<<(PACF_MAX+2), /* wait for apply */
	PEVF_WAIT_FOR_APPLY		= PVCF_WAIT_FOR_APPLY, /* to provide compatibility */
	PVCF_DESTROY_HDD_BUNDLE_FORCE	= 1<<(PACF_MAX+3), /* destroy HDD bundle physically even if
							used in snapshots */
	PVCF_DETACH_HDD_BUNDLE		= 1<<(PACF_MAX+4),
	PVCF_RENAME_EXT_DISKS		= 1<<(PACF_MAX+5),
} PRL_VM_COMMIT_FLAGS;

/* Mount virtual disk flags
 */
typedef enum _PRL_MOUNT_VIRTUAL_DISK
{
	PMVD_READ_ONLY			= 1<<(PACF_MAX+1),
	PMVD_READ_WRITE			= 1<<(PACF_MAX+2),
	PMVD_INFO			= 1<<(PACF_MAX+3),
} PRL_MOUNT_VIRTUAL_DISK;

/**
* Create image flags
*/
typedef enum _PRL_CREATE_DISK_IMAGE_FLAGS
{
	PCDIF_CREATE_FROM_LION_RECOVERY_PARTITION	= 1<<(PACF_MAX+1),	// create from lion recovery
} PRL_CREATE_DISK_IMAGE_FLAGS;
typedef PRL_CREATE_DISK_IMAGE_FLAGS* PRL_CREATE_DISK_IMAGE_FLAGS_PTR;

/**
* PrlVm_RegEx flags
*/
typedef enum _PRL_REGISTER_NEW_VM
{
	PRNVM_CREATE_FROM_LION_RECOVERY_PARTITION	= PCDIF_CREATE_FROM_LION_RECOVERY_PARTITION,
	PRNVM_ALLOW_TO_AUTO_DECREASE_HDD_SIZE		= 1<<(PACF_MAX+2),
	PRNVM_PRESERVE_DISK				= 1<<(PACF_MAX+3),
} PRL_REGISTER_NEW_VM;
typedef PRL_REGISTER_NEW_VM* PRL_REGISTER_NEW_VM_PTR;

/* List operations:
Uses for PrlSrv_GetVmListEx operation
*/
typedef enum _PRL_GET_VM_LIST_FLAGS
{
	PGVLF_GET_ONLY_VM = PVTF_VM,
	PGVLF_GET_ONLY_CT = PVTF_CT,
	PGVLF_GET_ONLY_IDENTITY_INFO	= 1 << (PACF_MAX+3),
	PGVLF_GET_STATE_INFO		= 1 << (PACF_MAX+4),
	PGVLF_FILL_AUTOGENERATED	= 1 << (PACF_MAX+5),
	PGVLF_GET_ONLY_VM_TEMPLATES	= 1 << (PACF_MAX+6),
} PRL_GET_VM_LIST_FLAGS;
typedef PRL_GET_VM_LIST_FLAGS* PRL_GET_VM_LIST_FLAGS_PTR;

/* Find VM operations:
*/
typedef enum _PRL_GET_VM_CONFIG_FLAGS
{
	PGVC_SEARCH_BY_UUID = 1 << (PACF_MAX+1),
	PGVC_SEARCH_BY_NAME = 1 << (PACF_MAX+2),
	PGVC_FILL_AUTOGENERATED = 1 << (PACF_MAX+3),
} PRL_GET_VM_CONFIG_FLAGS;
typedef PRL_GET_VM_CONFIG_FLAGS* PRL_GET_VM_CONFIG_FLAGS_PTR;

/* Update/Install license operations:
Uses for PrlSrv_UpdateLicenseEx operation
*/
typedef enum _PRL_UPDATE_LICENSE_FLAGS
{
	PUPLF_KICK_TO_UDPATE_CURR_FILE_LICENSE = 1 << (PACF_MAX+1),
	PUPLF_ACTIVATE_PERMANENT_LICENSE_ONLINE = 1 << (PACF_MAX+2),
	PUPLF_ACTIVATE_PERMANENT_LICENSE_OFFLINE = 1 << (PACF_MAX+3),
	PUPLF_CHECK_KEY_ONLY										 = 1 << (PACF_MAX+4),
	PUPLF_DEACTIVATE_CURRENT_LICENSE         = 1 << (PACF_MAX+5),
	PUPLF_ACTIVATE_PRECACHED_KEY             = 1 << (PACF_MAX+6),
	PUPLF_STORE_PRECACHED_KEY                = 1 << (PACF_MAX+7),
	PUPLF_REMOVE_PRECACHED_KEY               = 1 << (PACF_MAX+8),
	PUPLF_DEACTIVATE_CURRENT_LICENSE_OFFLINE_ALLOWED = 1 << (PACF_MAX+9),
	PUPLF_CHECK_KEY_ONLY_VOLUME							 = 1 << (PACF_MAX+10),
	PUPLF_REGISTER_CURRENT_LICENSE					 = 1 << (PACF_MAX+11),

} PRL_UPDATE_LICENSE_FLAGS;
typedef PRL_UPDATE_LICENSE_FLAGS* PRL_UPDATE_LICENSE_FLAGS_PTR;

/**
* PrlSrv_ShutdownEx flags
*/
typedef enum _PRL_SRV_SHUTDOWN_FLAGS
{
	PSHF_FORCE_SHUTDOWN	= 1<<(PACF_MAX+1),	/* Allow shutdown with active users sessions and running vms */
	PSHF_DONT_WAIT_TO_COMPLETE = 1<<(PACF_MAX+2),	/* Send response immediately when shutdown started */
	PSHF_SUSPEND_VM_TO_PRAM	= 1<<(PACF_MAX+3),	/* Use PRAM to suspend/resume VM if available */
} PRL_SRV_SHUTDOWN_FLAGS;
typedef PRL_SRV_SHUTDOWN_FLAGS* PRL_SRV_SHUTDOWN_FLAGS_PTR;

/**
* PrlVm_SetUserPasswd flags
*/
typedef enum _PRL_SET_USERPASSWD_FLAGS
{
	PSPF_PASSWD_CRYPTED	= 1 <<(PACF_MAX+1)		/* passwords are encrypted */
} PRL_SET_USERPASSWD_FLAGS;
typedef PRL_SET_USERPASSWD_FLAGS* PRL_SET_USERPASSWD_FLAGS_PTR;

/*
 PrlVm_GetStatisticsEx flags
*/
typedef enum _PRL_VM_STATISTICS_FLAGS
{
	PVMSF_HOST_DISK_SPACE_USAGE_ONLY		= 1 << (PACF_MAX+1),
	PVMSF_GUEST_OS_INFORMATION_ONLY			= 1 << (PACF_MAX+2),
} PRL_VM_STATISTICS_FLAGS;
typedef PRL_VM_STATISTICS_FLAGS* PRL_VM_STATISTICS_FLAGS_PTR;

/*
 * PrlSrv_LoginLocal flags
 */
typedef enum _PRL_LOGIN_LOCAL_FLAGS
{
	PLLF_LOGIN_WITH_DELAYED_CONNECTION		= 1 << (PACF_MAX+1),
} PRL_LOGIN_LOCAL_FLAGS;
typedef PRL_LOGIN_LOCAL_FLAGS* PRL_LOGIN_LOCAL_FLAGS_PTR;

/*
 * PrlSrv_AddVirtualNetwork flangs
 */
typedef enum _PRL_VIRTUAL_NETWORK_FLAGS
{
	PRL_USE_VNET_NAME_FOR_BRIDGE_NAME		= 1 << (PACF_MAX+1),
} PRL_VIRTUAL_NETWORK_FLAGS;
typedef PRL_VIRTUAL_NETWORK_FLAGS* PRL_VIRTUAL_NETWORK_FLAGS_PTR;

/*
 * PrlVm_BeginBackup flags.
 */
typedef enum _PRL_BEGIN_VM_BACKUP_FLAGS
{
	PBMBF_CREATE_MAP	= 1 << (PACF_MAX+1), /* Create a disk map */
} PRL_BEGIN_VM_BACKUP_FLAGS;

/*
 * PrlVmBackup_Commit / PrlVmBackup_Rollback flags.
 */
typedef enum _PRL_END_VM_BACKUP_FLAGS
{
	PEMBF_FAILURE_CLEANUP	= 1 << (PACF_MAX+1), /* Perform failure specific clean-up */
} PRL_END_VM_BACKUP_FLAGS;

/* PrlVm_CommitEncryption command flags */
typedef enum _PRL_COMMIT_ENCRYPTION_FLAGS
{
	/**
	 * Normally, only the master key (MK is used to actually encrypt the data
	 * on a virtual disk) is being re-encrypted when the disk encryption key
	 * identifier is changed for an already encrypted disk. But sometimes
	 * it is desirable to change the master key as well. In this case, new
	 * master key is generated and the virtual disk content is re-encrypted
	 * with this new master key. After that, the master key is encrypted
	 * with the key, obtained by key identifier, and this encrypted value
	 * of master key is stored in the LUSK header on the encrypted virtual disk.
	 *
	 * Set this flag to re-generate master key during the change of
	 * encryption key identifier.
	 */
	PCEF_CHANGE_MASTER_KEY		= 1 << (PACF_MAX+1),
	/**
	 * When a plain disk is being encrypted, the encrypted copy of the original
	 * disk is created. After the data had been successfully copied from
	 * the original disk to the encrypted one, the original disk is removed.
	 * Simply removing the original virtual disk file from the host HDD
	 * would leave the unencrypted data on the HDD, albeit it is not accessible
	 * via its name in the filesystem anymore. This data could later be
	 * (partly) restored from the host HDD using special tools. The more secure
	 * way of destroying the original virtual disk would be to overwrite the
	 * original data with zeros ("shred" it) and only after that remove the
	 * virtual disk from the host filesystem. This is the default behavior
	 * when the original virtual disk is removed after its data had been
	 * copied to the encrypted disk.
	 *
	 * Set this flag to disable wiping unencrypted data with zeros.
	 */
	PCEF_DONT_SHRED_PLAIN_DATA	= 1 << (PACF_MAX+2),
} PRL_COMMIT_ENCRYPTION_FLAGS;
typedef PRL_COMMIT_ENCRYPTION_FLAGS* PRL_COMMIT_ENCRYPTION_FLAGS_PTR;

/**
 * Specifies an on-crash options
 */
typedef enum _PRL_VM_ON_CRASH_OPTION
{
	POCO_DUMP	= 1<<(PACF_MAX+1),
	POCO_REPORT	= 1<<(PACF_MAX+2),
	POCO_FULL	= POCO_DUMP|POCO_REPORT,
} PRL_VM_ON_CRASH_OPTION;
typedef PRL_VM_ON_CRASH_OPTION* PRL_VM_ON_CRASH_OPTION_PTR;

typedef enum _PRL_VCMMD_COMMAND_FLAGS
{
	VCMMD_COMMAND_RUNTIME = 1<<(PACF_MAX+1),
} PRL_VCMMD_COMMAND_FLAGS;
typedef PRL_VCMMD_COMMAND_FLAGS* PRL_VCMMD_COMMAND_FLAGS_PTR;

typedef enum _PRL_CT_REINSTALL_OPTIONS
{
	REINSTALL_SKIP_BACKUP	= 1<<(PACF_MAX+1),
	REINSTALL_SKIP_SCRIPTS	= 2<<(PACF_MAX+1),
	REINSTALL_RESET_PWDB	= 3<<(PACF_MAX+1),
} PRL_CT_REINSTALL_OPTIONS;
typedef PRL_CT_REINSTALL_OPTIONS* PRL_CT_REINSTALL_OPTIONS_PTR;
#endif // __PARALLELS_API_COMMANDS_FLAGS_H__
