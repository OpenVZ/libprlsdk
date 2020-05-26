/*
 * PrlApiVm.h
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


#ifndef __VIRTUOZZO_API_VM_H__
#define __VIRTUOZZO_API_VM_H__

#ifdef __cplusplus
extern "C" {
#endif

// On windows platform we're taking advantage of the #pack directive
// to ensure internal structures alignment by 1 - on unixes we achive
// the same results by using the __attribute__((packed)) directive
// -> changing alignment
#ifdef _WIN_
    #pragma pack(push, save_api_pack, 1)
#endif

#include "PrlIOStructs.h"
#include "PrlKeys.h"

///////////////////////////////////////////////////////////////////////////////
/// @section Generic API methods for the client
///
/// This section consists of methods that are used for general
/// manipulation of the api library and are not specific
/// to objects in the object hierarchy.
///////////////////////////////////////////////////////////////////////////////

#include "PrlApiVersion.h"


///////////////////////////////////////////////////////////////////////////////
/// @section Server object handling routines and data types.
///////////////////////////////////////////////////////////////////////////////

/* Registers an existing virtual machine with Dispatcher Service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_VIRTUAL_MACHINE.
   Parameters
   hServer :              A handle of type PHT_SERVER identifying
                          the Dispatcher Service.
   strVmDirPath :         An absolute path to the virtual machine
                          directory to register.
   bNonInteractiveMode :  Set to PRL_TRUE to use non\-interactive
                          mode. Set to PRL_FALSE to use
                          interactive mode. In interactive mode,
                          a client may receive questions from the
                          Dispatcher Service, which it is expected
                          to answer in order for the operation to
                          continue. In non\-interactive mode, the
                          Dispatcher Service will make decisions
                          on its own.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation, including the return code and a
   handle of type PHT_VIRTUAL_MACHINE containing information
   about the virtual machine that was registered.

   PRL_INVALID_HANDLE if there's not enough memory to
   instantiate the job object.                                    */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlSrv_RegisterVm, (
		PRL_HANDLE hServer,
		PRL_CONST_STR strVmDirPath,
		PRL_BOOL bNonInteractiveMode
		) );

/* Registers an existing virtual machine with Dispatcher Service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_VIRTUAL_MACHINE.
   Parameters
   hServer :      A handle of type PHT_SERVER identifying
                  the Dispatcher Service.
   strVmDirPath : An absolute path the virtual machine
                  directory to register.
   nFlags       : A bitset of flags.
                  Allowed next values:

                  0 - no flags. It's behaviour by default ( as PrlSrv_RegisterVm() )

                  PACF_NON_INTERACTIVE_MODE	  - to use
					 non-interactive mode. In interactive mode,
					 a client may receive questions from the
					 Dispatcher Service, which it is expected
					 to answer in order for the operation to
					 continue. In non\-interactive mode, the
					 Dispatcher Service will make decisions
					 on its own.

				  PRVF_REGENERATE_VM_UUID  - to generate new VM identity after registration.

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation, including the return code and a
   handle of type PHT_VIRTUAL_MACHINE containing information
   about the virtual machine that was registered.

   PRL_INVALID_HANDLE if there's not enough memory to
   instantiate the job object.                                    */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_RegisterVmEx, (
		PRL_HANDLE hServer,
		PRL_CONST_STR strVmDirPath,
		PRL_UINT32 nFlags
		) );


/* Registers an existing virtual machine with custom Vm UUID.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_VIRTUAL_MACHINE.
   Parameters
   hServer :      A handle of type PHT_SERVER identifying
                  the Dispatcher Service.
   strVmDirPath : An absolute path the virtual machine
                  directory to register.
   strVmUuid	: Assign custom Vm uuid
   nFlags       : A bitset of flags.
                  Allowed next values:

                  0 - no flags. It's behaviour by default ( as PrlSrv_RegisterVm() )

                  PACF_NON_INTERACTIVE_MODE	  - to use
					 non-interactive mode. In interactive mode,
					 a client may receive questions from the
					 Dispatcher Service, which it is expected
					 to answer in order for the operation to
					 continue. In non\-interactive mode, the
					 Dispatcher Service will make decisions
					 on its own.

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation, including the return code and a
   handle of type PHT_VIRTUAL_MACHINE containing information
   about the virtual machine that was registered.

   PRL_INVALID_HANDLE if there's not enough memory to
   instantiate the job object.                                    */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_5,
						   PrlSrv_RegisterVmWithUuid, (
		PRL_HANDLE hServer,
		PRL_CONST_STR strVmDirPath,
		PRL_CONST_STR strVmUuid,
		PRL_UINT32 nFlags
		) );


/* %VM_ONLY%
 
   Registers an existing 3rd party virtual machine with Dispatcher Service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_VIRTUAL_MACHINE.
   Parameters
   hServer :              A handle of type PHT_SERVER identifying
                          the Dispatcher Service.
   strVmConfigPath :      [required] An absolute path to the virtual
                          machine configuration file (*.vmx for
						  example)
   strVmRootDirPath :     [optional] An absolute path to the virtual machine
                          root directory where virtual machine home directory
						  will be created.
   nFlags :               Flags bitset:
			0			- Default behavior
			PR3F_ALLOW_UNKNOWN_OS
						- to allow undetectable OS
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation, including the return code and a
   handle of type PHT_VIRTUAL_MACHINE containing information
   about the virtual machine that was registered.

   PRL_INVALID_HANDLE if there's not enough memory to
   instantiate the job object.                                    */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_Register3rdPartyVm, (
		PRL_HANDLE hServer,
		PRL_CONST_STR strVmConfigPath,
		PRL_CONST_STR strVmRootDirPath,
		PRL_UINT32 nFlags
		) );


/* %VM_ONLY%
 
   Extracts supported guest OSes matrix from remote server (it's more proper
   way instead of using local synch API in case when client and server
   interoperate remotely).

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_GUEST_OSES_MATRIX which will contain complete list
		of supported guest OSes types, versions and defaults.
   Parameters
   hServer :              A handle of type PHT_SERVER identifying
                          the Dispatcher Service.
   phOsesMatrix :         pointer to the buffer for storing result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.
   PRL_ERR_NO_DATA - no supported OSes data (OSes matrix filling just after
                     login procedure completed). Also this field can be empty
					 in case when remote server is not supported this interface.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlSrv_GetSupportedOses, (
		PRL_HANDLE hServer,
		PRL_HANDLE_PTR phOsesMatrix
		) );


/* Obtains a new handle of type PHT_VIRTUAL_MACHINE. Obtaining a
   new PHT_VIRTUAL_MACHINE handle is the first step when
   creating a virtual machine. The second step is setting the
   virtual machine configuration parameters. The final step is
   calling the PrlVm_Reg function, which will create a new
   virtual machine.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   phVm :     [out] A pointer to a variable that receives the
              handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlSrv_CreateVm, (
					 PRL_HANDLE hServer,
					 PRL_HANDLE_PTR phVm
					 ) );

/* Retrieves a list of virtual machines registered with the
   specified Dispatcher Service. The information about each virtual machine
   is returned as a handle of type PHT_VIRTUAL_MACHINE.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamsCount function to get the
        number of virtual machine in the result set.
     3. Use the PrlResult_GetParamByIndex function in a loop, or
        by passing a specific index, to obtain a handle to each
        virtual machine.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlSrv_GetVmList, (
		PRL_HANDLE hServer
		) );

/* Retrieves a list of virtual machines registered with the
   specified Dispatcher Service. The information about each virtual machine
   is returned as a handle of type PHT_VIRTUAL_MACHINE.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamsCount function to get the
        number of virtual machine in the result set.
     3. Use the PrlResult_GetParamByIndex function in a loop, or
        by passing a specific index, to obtain a handle to each
        virtual machine.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
              Service.
   nFlags :   Flags bitset of PRL_GET_VM_LIST_FLAGS enumerator.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlSrv_GetVmListEx, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );

/* Obtains a new handle of type PHT_VIRTUAL_MACHINE. Obtaining a
   new PHT_VIRTUAL_MACHINE handle is the first step when
   creating a virtual machine. The second step is setting the
   virtual machine configuration parameters. The final step is
   calling the PrlVm_Reg function, which will create a new
   virtual machine.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_VIRTUAL_MACHINE.

   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   pParam :   Pointer to the PRL_GET_VM_CONFIG_PARAM_DATA structure.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
					PrlSrv_GetDefaultVmConfig, (
		PRL_HANDLE hServer,
		PRL_CONST_GET_VM_CONFIG_PARAM_DATA_PTR pParam,
		PRL_UINT32 nFlags
		) );


/* Searches for unregistered virtual machines at the specified
   location(s). The function can be used
   to search for virtual machines that are not currently
   registered with the specified Dispatcher Service but are otherwise valid
   machines to be registered with it. The virtual machine
   information is returned as a list of PHT_FOUND_VM_INFO
   objects. When the function is executed asynchronously using
   callback functionality, the callback function receives the
   information as an event of type
   PET_DSP_EVT_FOUND_LOST_VM_CONFIG.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamsCount function to get the
        number of virtual machines in the list.
     3. Use the PrlResult_GetParam function to obtain individual
        handles of type PHT_FOUND_VM_INFO.
   Parameters
   hServer :       A handle of type PHT_SERVER identifying the
                   Dispatcher Service.
   hStringsList :  A handle of type PHT_STRINGS_LIST containing
                   the list of pathnames to search. If the list
                   is empty, all available drives and directories
                   will be included in search scope.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlSrv_StartSearchVms, (
		PRL_HANDLE hServer,
		PRL_HANDLE hStringsList
		) );


/* Sends an appliance configuration to the Dispatcher Service in order to
   install the corresponding appliance from download service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :       A handle of type PHT_SERVER identifying the
                   Dispatcher Service.
   hAppCfg :       A handle of type PHT_APPLIANCE_CONFIG identifying the appliance.
   sVmParentPath : Optional. Name and path of the parent directory
                   where the virtual machine appliance should be installed.
                   If this parameter is omitted (null pointer or empty string
                   is passed), the new virtual machine appliance directory
                   will be created in the default directory for this
                   Dispatcher Service.
   nFlags :        Reserved parameter
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlSrv_InstallAppliance, (
		PRL_HANDLE hServer,
		PRL_HANDLE hAppCfg,
		PRL_CONST_STR sVmParentPath,
		PRL_UINT32 nFlags
		) );

/* %VM_ONLY%

   Cancels the appliance installation previously started with
   the PrlSrv_InstallAppliance function call.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :       A handle of type PHT_SERVER identifying the
                   Dispatcher Service.
   hAppCfg :       A handle of type PHT_APPLIANCE_CONFIG identifying the appliance.
                   This must be the same handle instance that was passed
                   to the PrlSrv_InstallAppliance function.
   nFlags :        Reserved parameter
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlSrv_CancelInstallAppliance, (
		PRL_HANDLE hServer,
		PRL_HANDLE hAppCfg,
		PRL_UINT32 nFlags
		) );


/* %VM_ONLY%

   Stops the appliance installation previously started with
   the PrlSrv_InstallAppliance function call.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :       A handle of type PHT_SERVER identifying the
                   Dispatcher Service.
   hAppCfg :       A handle of type PHT_APPLIANCE_CONFIG identifying the appliance.
                   This must be the same handle instance that was passed
                   to the PrlSrv_InstallAppliance function.
   nFlags :        Reserved parameter
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlSrv_StopInstallAppliance, (
		PRL_HANDLE hServer,
		PRL_HANDLE hAppCfg,
		PRL_UINT32 nFlags
		) );


/* Backup an existing virtual machine to backup server.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Note: errors are the same as for VmMigrate activity

   Parameters
   hSourceServer :     A handle of type PHT_SERVER
                       identifying the source Dispatcher Service.
   sVmUuid :           A virtual machine uuid
   sTargetHost:        The name of the target host machine.
   nTargetPort:        The port number on the target host.
   sTargetSessionId:   The target Dispatcher Service session ID.
   sDescription:       The backup description.
   backup_flags :      Flags that specify backup type and etc. Acceptable flags:
   PBT_FULL		: full backup
   PBT_INCREMENTAL	: incremental backup
   PBT_DIFFERENTIAL	: differental backup
   PBT_UNCOMPRESSED	: uncompressed backup (by default fast zip compression used)
                       PVMSL_LOW_SECURITY, PVMSL_NORMAL_SECURITY, PVMSL_HIGH_SECURITY
   PBT_DIRECT_DATA_CONNECTION  : Disables tunneling for backup (a laissez-fair
                       data transfer).
   reserved_flags :    Reserved flags.
   force_operation :   Specifies to process VM backup action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_CreateVmBackup, (
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_CONST_STR strDescription,
		PRL_UINT32 backup_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		) );

/* Restore backup of the virtual machine from backup server.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Note: errors are the same as for VmMigrate activity

   Parameters
   hSourceServer :     A handle of type PHT_SERVER
                       identifying the source Dispatcher Service.
   sVmUuid :           A virtual machine uuid
   sBackupUuid :       A backup uuid
   sTargetHost:        The name of the target host machine.
   nTargetPort:        The port number on the target host.
   sTargetSessionId:   The target Dispatcher Service session ID.
   sTargetVmHomePath : Home directory of the target virtual machine.
   restore_flags :     Flags that specify restore type and etc. Acceptable flags:
   PBT_RESTORE_TO_COPY	: change Vm name/uuid/MAC address on restore
                       PVMSL_LOW_SECURITY, PVMSL_NORMAL_SECURITY, PVMSL_HIGH_SECURITY
   reserved_flags :    Reserved flags.
   force_operation :   Specifies to process VM backup action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_RestoreVmBackup, (
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sBackupUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_CONST_STR sTargetVmHomePath,
		PRL_CONST_STR sTargetVmName,
		PRL_UINT32 restore_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		) );

/* Get backups tree from backup server.

   Some of the important XML elements are:

   \<VmItem\> - root element of a branch containing
   information about a virtual machine. This element, in turn, may contain
   a set of elements describing full backups of this virtual machine.

   \<BackupItem\> - root element of a branch containing
   information about a full backup. This element, in turn, may contain a set
   of elements describing incremental backups of this full backup.

   \<PartialBackupItem\> - root element of a branch containing
   information about an incremental backup.

   \<Id\> - the backup ID. The ID is required when performing other
   operations on a backup.

   \<DateTime\> - the backup creation date and time.

   \<Type> - backup type {'f' - full, 'i' - incremental}

   \<Size> Backup size in bytes

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_INVALID_PARAM - invalid combination of backup_flags was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get a backup tree from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamAsString function to obtain a
        string value containing the backup tree XML data.
   Please note that no parser is provided to traverse the
   returned XML. To use this functionality, you will have to
   parse the returned XML yourself.

   Parameters
   hSourceServer :     A handle of type PHT_SERVER
                       identifying the source Dispatcher Service.
   sUuid :             A virtual machine or a backup ID. If it is empty,
                       the tree will be built for backups of all virtual machines.
                       When PBT_BACKUP_ID or PBT_CHAIN flag is specified in
                       the backup_flags argument, then the value of this argument
                       is treated as a backup ID, otherwise - as a virtual machine UUID.
   sTargetHost:        The name of the target host machine.
   nTargetPort:        The port number on the target host.
   sTargetSessionId:   The target Dispatcher Service session ID.
   backup_flags :      Flags that specify backup type and etc. Acceptable flags:
   	   	   	   	   	   PBT_VM - include only virtual machine backups (used by default
   	   	   	   	   	   if PBT_VM and PBT_CT flags are not specified)
   	   	   	   	   	   PBT_CT - include only container backups
                       PBT_BACKUP_ID - get information about the backup, which identifier is
                       specified in sUuid parameter. Note that if an identifier of an
                       incremental backup is passed in the sUuid parameter, then
                       the output tree would also contain the corresponding full backup element.
                       PBT_CHAIN - get information about the whole backup chain, which
                       is dependent on the backup identifier, specified in sUuid parameter.
                       Note that if an identifier of an incremental backup is passed in
                       the sUuid parameter, then the output tree would also contain
                       the corresponding full backup element.

                       PVMSL_LOW_SECURITY, PVMSL_NORMAL_SECURITY, PVMSL_HIGH_SECURITY
   reserved_flags :    Reserved flags.
   force_operation :   Specifies to process VM backup action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_GetBackupTree, (
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_UINT32 backup_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		) );

/* Remove backup of the virtual machine from backup server.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Note: errors are the same as for VmMigrate activity

   Parameters
   hSourceServer :     A handle of type PHT_SERVER
                       identifying the source Dispatcher Service.
   sVmUuid :           A virtual machine uuid
   sBackupUuid :       A backup uuid
   sTargetHost:        The name of the target host machine.
   nTargetPort:        The port number on the target host.
   sTargetSessionId:   The target Dispatcher Service session ID.
   remove_flags :      Flags that specify remove type and etc. Acceptable flags:
                       PVMSL_LOW_SECURITY, PVMSL_NORMAL_SECURITY, PVMSL_HIGH_SECURITY
   reserved_flags :    Reserved flags.
   force_operation :   Specifies to process VM backup action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_RemoveVmBackup, (
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sBackupUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_UINT32 remove_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		) );


/* Backup an existing virtual machine to backup server.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Note: errors are the same as for VmMigrate activity

   Parameters
   hSourceServer :     A handle of type PHT_SERVER
                       identifying the source Dispatcher Service.
   sVmUuid :           A virtual machine uuid
   sTargetHost:        The name of the target host machine.
   nTargetPort:        The port number on the target host.
   sTargetSessionId:   The target Dispatcher Service session ID.
   sDescription:       The backup description.
   backup_flags :      Flags that specify backup type and etc. Acceptable flags:
   PBT_FULL		: full backup
   PBT_INCREMENTAL	: incremental backup
   PBT_DIFFERENTIAL	: differental backup
   PBT_UNCOMPRESSED	: uncompressed backup (by default fast zip compression used)
                       PVMSL_LOW_SECURITY, PVMSL_NORMAL_SECURITY, PVMSL_HIGH_SECURITY
   PBT_DIRECT_DATA_CONNECTION  : Disables tunneling for backup (a laissez-fair
                       data transfer).
   reserved_flags :    Reserved flags.
   force_operation :   Specifies to process VM backup action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).
   pExtra :            Extra params, e.g. backup_directory

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_7,
						   PrlSrv_CreateVmBackupEx, (
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_CONST_STR strDescription,
		PRL_UINT32 backup_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation,
		const PRL_BACKUP_PARAM *pExtra
		) );

/* Restore backup of the virtual machine from backup server.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Note: errors are the same as for VmMigrate activity

   Parameters
   hSourceServer :     A handle of type PHT_SERVER
                       identifying the source Dispatcher Service.
   sVmUuid :           A virtual machine uuid
   sBackupUuid :       A backup uuid
   sTargetHost:        The name of the target host machine.
   nTargetPort:        The port number on the target host.
   sTargetSessionId:   The target Dispatcher Service session ID.
   sTargetVmHomePath : Home directory of the target virtual machine.
   restore_flags :     Flags that specify restore type and etc. Acceptable flags:
   PBT_RESTORE_TO_COPY	: change Vm name/uuid/MAC address on restore
                       PVMSL_LOW_SECURITY, PVMSL_NORMAL_SECURITY, PVMSL_HIGH_SECURITY
   reserved_flags :    Reserved flags.
   force_operation :   Specifies to process VM backup action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_7,
						   PrlSrv_RestoreVmBackupEx, (
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sBackupUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_CONST_STR sTargetVmHomePath,
		PRL_CONST_STR sTargetVmName,
		PRL_UINT32 restore_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation,
		const PRL_BACKUP_PARAM *pExtra
		) );

/* Get backups tree from backup server.

   Some of the important XML elements are:

   \<VmItem\> - root element of a branch containing
   information about a virtual machine. This element, in turn, may contain
   a set of elements describing full backups of this virtual machine.

   \<BackupItem\> - root element of a branch containing
   information about a full backup. This element, in turn, may contain a set
   of elements describing incremental backups of this full backup.

   \<PartialBackupItem\> - root element of a branch containing
   information about an incremental backup.

   \<Id\> - the backup ID. The ID is required when performing other
   operations on a backup.

   \<DateTime\> - the backup creation date and time.

   \<Type> - backup type {'f' - full, 'i' - incremental}

   \<Size> Backup size in bytes

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_INVALID_PARAM - invalid combination of backup_flags was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get a backup tree from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamAsString function to obtain a
        string value containing the backup tree XML data.
   Please note that no parser is provided to traverse the
   returned XML. To use this functionality, you will have to
   parse the returned XML yourself.

   Parameters
   hSourceServer :     A handle of type PHT_SERVER
                       identifying the source Dispatcher Service.
   sUuid :             A virtual machine or a backup ID. If it is empty,
                       the tree will be built for backups of all virtual machines.
                       When PBT_BACKUP_ID or PBT_CHAIN flag is specified in
                       the backup_flags argument, then the value of this argument
                       is treated as a backup ID, otherwise - as a virtual machine UUID.
   sTargetHost:        The name of the target host machine.
   nTargetPort:        The port number on the target host.
   sTargetSessionId:   The target Dispatcher Service session ID.
   backup_flags :      Flags that specify backup type and etc. Acceptable flags:
                                           PBT_VM - include only virtual machine backups (used by default
                                           if PBT_VM and PBT_CT flags are not specified)
                                           PBT_CT - include only container backups
                       PBT_BACKUP_ID - get information about the backup, which identifier is
                       specified in sUuid parameter. Note that if an identifier of an
                       incremental backup is passed in the sUuid parameter, then
                       the output tree would also contain the corresponding full backup element.
                       PBT_CHAIN - get information about the whole backup chain, which
                       is dependent on the backup identifier, specified in sUuid parameter.
                       Note that if an identifier of an incremental backup is passed in
                       the sUuid parameter, then the output tree would also contain
                       the corresponding full backup element.

                       PVMSL_LOW_SECURITY, PVMSL_NORMAL_SECURITY, PVMSL_HIGH_SECURITY
   reserved_flags :    Reserved flags.
   force_operation :   Specifies to process VM backup action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_7,
						   PrlSrv_GetBackupTreeEx, (
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_UINT32 backup_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation,
		const PRL_BACKUP_PARAM *pExtra
		) );

/* Remove backup of the virtual machine from backup server.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Note: errors are the same as for VmMigrate activity

   Parameters
   hSourceServer :     A handle of type PHT_SERVER
                       identifying the source Dispatcher Service.
   sVmUuid :           A virtual machine uuid
   sBackupUuid :       A backup uuid
   sTargetHost:        The name of the target host machine.
   nTargetPort:        The port number on the target host.
   sTargetSessionId:   The target Dispatcher Service session ID.
   remove_flags :      Flags that specify remove type and etc. Acceptable flags:
                       PVMSL_LOW_SECURITY, PVMSL_NORMAL_SECURITY, PVMSL_HIGH_SECURITY
   reserved_flags :    Reserved flags.
   force_operation :   Specifies to process VM backup action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_7,
						   PrlSrv_RemoveVmBackupEx, (
		PRL_HANDLE hSourceServer,
		PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sBackupUuid,
		PRL_CONST_STR sTargetHost,
		PRL_UINT32 nTargetPort,
		PRL_CONST_STR sTargetSessionId,
		PRL_UINT32 remove_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation,
		const PRL_BACKUP_PARAM *pExtra
		) );


/* %VM_ONLY%

   Creates a bootable ISO-image for unattended Linux installation.
   The following steps describe how to prepare a virtual
   machine for unattended Linux installation:
     1. Create a virtual machine. The machine should have all
        the necessary devices, like hard drive and CD drive.
     2. Insert Linux installation CD-ROM into the drive (or
        mount a disk image).
     3. Execute the PrlVm_CreateUnattendedCd function to
        create a bootable ISO-image.
     4. Set created ISO-image as boot device.
     5. Start the virtual machine. The machine will boot from
        the created ISO-image.
     6. Perform unattended Windows installation as usual.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   		A handle of type PHT_SERVER identifying the
               		Dispatcher Service.
   nGuestDistroType :   Guest OS version from the (PVS_GUEST_VER_...).
   sPath :		Full path to the OS distribution image or folder.
   sUserName :          User name.
   sPasswd :	        Password.
   sFullUserName :      Full user name.
   sOsDistroPath :      A full path to the OS distribution image or folder.
   sOutImagePath :      A full path to output ISO-image file.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                 */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_CreateUnattendedCd, (
		PRL_HANDLE hServer,
		PRL_UINT32 nGuestType,
		PRL_CONST_STR sUserName,
		PRL_CONST_STR sPasswd,
		PRL_CONST_STR sFullUserName,
		PRL_CONST_STR sOsDistroPath,
		PRL_CONST_STR sOutImagePath
		) );


/* Retrieves a list of Containers templates registered with the
   specified Dispatcher Service. The information about each template
   is returned as a handle of type PHT_CT_TEMPLATE.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamsCount function to get the
        number of templates in the result set.
     3. Use the PrlResult_GetParamByIndex function in a loop, or
        by passing a specific index, to obtain a handle to each
        template.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
              Service.
   nFlags :   Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
			   PrlSrv_GetCtTemplateList, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );

/* Removes the specified Containers template.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   Parameters
   hServer    : A handle of type PHT_SERVER identifying the Dispatcher
                Service.
   sName      : Name of template to be removed.
   sOsTmplName: Name of OS template for which this application template is.
                Should be filled for application templates.
   nFlags     : Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlSrv_RemoveCtTemplate, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sName,
		PRL_CONST_STR sOsTmplName,
		PRL_UINT32 nFlags
		) );


/* %CT_ONLY%

   Copy the specified Containers template to the specified server.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   Parameters
   hSrv       : A handle of type PHT_SERVER identifying the Dispatcher
                Service.
   sName      : Name of template to be removed.
   sOsTmplName: Name of OS template for which this application template is.
                Should be filled for application templates.
   sTargetServerHostname : target server hostname
   nTargetServerPort     : target server port
   sTargetServerSessionUuid: target server session uuid
   nFlags     : Command flags.
   nReservedFlags : Reserved command flags.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlSrv_CopyCtTemplate, (
		PRL_HANDLE hSrv,
		PRL_CONST_STR sName,
		PRL_CONST_STR sOsTmplName,
		PRL_CONST_STR sTargetServerHostname,
		PRL_UINT32 nTargetServerPort,
		PRL_CONST_STR sTargetServerSessionUuid,
		PRL_UINT32 nFlags,
		PRL_UINT32 nReservedFlags
		) );


/* Get VM config by given UUID or name.
   Parameters
   hHandle :  A handle of type PHT_SERVER.
   sSearchId : VM UUID or name.
   nFlags  : Search by UUID if nFlags is 0 or PGVC_SEARCH_BY_UUID,
   	by name, if nFlags is PGVC_SEARCH_BY_NAME and search first by uuid,
	then by name if nFlags is PGVC_SEARCH_BY_UUID | PGVC_SEARCH_BY_NAME.
   PGVC_FILL_AUTOGENERATED indicates that config should include
   implicit autogenerated params.
   Returns
   	A handle of type PHT_JOB containing the results of operation. */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlSrv_GetVmConfig, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sSearchId,
		PRL_UINT32 nFlags
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Supported guest OSes extraction functions set
///////////////////////////////////////////////////////////////////////////////

/* %VM_ONLY%
 
   Returns handle of type PHT_OPAQUE_TYPE_LIST which contains supported guest OSes
   types list for current API mode (see PrlApi_InitEx for more info about API mode).

   Parameters
   nHostOsType	   :  type of host system (remote or local) for which supported OSes list
					  should be retrieved. In remote work mode please determine at first
					  remote side host type (please use PrlSrvCfg_GetHostOsType for this).
					  In local work mode PHO_UNKNOWN can be passed - host OS type will be
					  automatically detected by API call in this case.
   phOsesTypesList :  [out] A pointer to a variable that receives
					  the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the
						   list object.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlApi_GetSupportedOsesTypes, (
					PRL_HOST_OS_TYPE nHostOsType,
					PRL_HANDLE_PTR phOsesTypesList
		) );


/* %VM_ONLY%

   Returns handle of type PHT_OPAQUE_TYPE_LIST which contains supported guest OSes
   list for specified guest OS type at current API mode (see PrlApi_InitEx for more
   info about API mode).

   Parameters
   nHostOsType	   :  type of host system (remote or local) for which supported OSes list
					  should be retrieved. In remote work mode please determine at first
					  remote side host type (please use PrlSrvCfg_GetHostOsType for this).
					  In local work mode PHO_UNKNOWN can be passed - host OS type will be
					  automatically detected by API call in this case.
   nGuestOsType    :  requesting guest OS type. In order to determine list of supported
                      guests types please refer PrlOses.h
   phOsesTypesList :  [out] A pointer to a variable that receives
					  the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer or unknown OS type was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the
						   list object.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlApi_GetSupportedOsesVersions, (
					PRL_HOST_OS_TYPE nHostOsType,
					PRL_UINT8 nGuestOsType,
					PRL_HANDLE_PTR phGuestsList
		) );


/* %VM_ONLY%
 
   Returns default guest OS version for specified guest OS type at current API mode
   (see PrlApi_InitEx for more info about API mode).

   Parameters
   nGuestOsType    :  requesting guest OS type. In order to determine list of supported
                      guests types please refer PrlOses.h
   pnOsVersion     :  [out] A pointer to a variable that receives
					  the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer or unknown OS type was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlApi_GetDefaultOsVersion, (
					PRL_UINT8 nGuestOsType,
					PRL_UINT16_PTR pnOsVersion
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the guest OSes matrix object.
///////////////////////////////////////////////////////////////////////////////

/* Returns handle of type PHT_OPAQUE_TYPE_LIST which contains supported guest OSes
   types list retrieved from remote server (see PrlSrv_GetSupportedOses).

   Parameters
   hOsesMatrix	   :  [in] handle of type PHT_GUEST_OSES_MATRIX.
   phOsesTypesList :  [out] A pointer to a variable that receives
					  the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer or invalid handle was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the
						   list object.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlOsesMatrix_GetSupportedOsesTypes, (
					PRL_HANDLE hOsesMatrix,
					PRL_HANDLE_PTR phOsesTypesList
		) );

/* Returns handle of type PHT_OPAQUE_TYPE_LIST which contains supported guest OSes
   list for specified guest OS type retrieved from remote server (see PrlSrv_GetSupportedOses).

   Parameters
   hOsesMatrix	   :  [in] handle of type PHT_GUEST_OSES_MATRIX.
   nGuestOsType    :  [in] requesting guest OS type. In order to determine list of supported
                      guests types please refer PrlOses.h
   phOsesTypesList :  [out] A pointer to a variable that receives
					  the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle, null pointer or unknown OS type was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the
						   list object.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlOsesMatrix_GetSupportedOsesVersions, (
					PRL_HANDLE hOsesMatrix,
					PRL_UINT8 nGuestOsType,
					PRL_HANDLE_PTR phGuestsList
		) );

/* Returns default guest OS version for specified guest OS type retrieved from remote server
   (see PrlSrv_GetSupportedOses).

   Parameters
   hOsesMatrix	   :  [in] handle of type PHT_GUEST_OSES_MATRIX.
   nGuestOsType    :  [in] requesting guest OS type. In order to determine list of supported
                      guests types please refer PrlOses.h
   pnOsVersion     :  [out] A pointer to a variable that receives
					  the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid jandle, null pointer or unknown OS type was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlOsesMatrix_GetDefaultOsVersion, (
					PRL_HANDLE hOsesMatrix,
					PRL_UINT8 nGuestOsType,
					PRL_UINT16_PTR pnOsVersion
		) );

/* Returns the support type for the specified guest OS type retrieved from the
   remote server (see PrlSrv_GetSupportedOses)
   and guest OS version (see PrlOsesMatrix_GetSupportedOsesVersions)

   Parameters
   hOsesMatrix       :  [in] handle of type PHT_GUEST_OSES_MATRIX.
   nGuestOsType      :  [in] requesting guest OS type. In order to determine the list of supported
                        guest types refer to PrlOses.h
   nGuestOSVersion   :  [in] requesting guest OS version. In order to determine the list of supported
                        guest types refer to PrlOses.h
   pnGuestSupportType:  [out] A pointer to a variable that receives the result

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - an invalid handle, a null pointer or an unknown OS type/version was passed.

   PRL_ERR_SUCCESS - function completed successfully */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				PrlOsesMatrix_GetSupportType, (
					PRL_HANDLE hOsesMatrix,
					PRL_UINT8 nGuestOsType,
					PRL_UINT16 nGuestOSVersion,
					PRL_GUEST_OS_SUPPORT_TYPE_PTR pnGuestSupportType
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Virtual machine OS info
///////////////////////////////////////////////////////////////////////////////

/* %VM_ONLY%
 
   The PrlApi_GetRecommendMinVmMem returns recommend minimal
   memory size for guest OS defined in the OS version parameter.
   Parameters
   nOsVersion :      OS version
   pnMinMemSize :    [out] A pointer to a variable that receives
                     the result. Result is the recommend minimal
                     memory size for guest OS.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlApi_GetRecommendMinVmMem, (
		PRL_UINT32 nOsVersion,
		PRL_UINT32_PTR pnMinMemSize
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Virtual machine object handling routines and data types
///////////////////////////////////////////////////////////////////////////////

/* Start the specified virtual machine. This function can be
   used to start a stopped or paused virtual machine.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.s
   See Also
   PrlVm_Pause                                                 */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Start, (
		PRL_HANDLE hVm
		) );

/* Starts the specified virtual machine using the specified mode
   (see the PRL_VM_START_MODE enumeration). Use this function
   instead of the PrlVm_Start function to start the virtual
   machine in a normal or safe mode. Other modes may be added in
   the future.

   If VM is starting in PSM_VM_START_FOR_COMPACT mode then flags
   (third argument) have to be equal PRL_HARD_DISK_IFACE_MASK enum.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or start mode of virtual
   machine.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :         A handle of type PHT_VIRTUAL_MACHINE identifying
                 the virtual machine.
   nStartMode :  Virtual machine start mode.
   nReserved :   This is a reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_StartEx, (
		PRL_HANDLE hVm,
		PRL_UINT32 nStartMode,
		PRL_UINT32 nReserved
		) );

/* Restarts the operating system of the specified virtual
   machine. To get the return code from the PHT_JOB object, use
   the PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or start mode of virtual
   machine.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_Restart, (
		PRL_HANDLE hVm
		) );

/* Stop the specified virtual machine. To get the return code
   from the PHT_JOB object, use the PrlJob_GetRetCode function.
   Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :    A handle of type PHT_VIRTUAL_MACHINE identifying the
            virtual machine.
   bGraceful: If graceful shutdown is used, the machine will be properly
	    shutdown by means of Guest tools (if available), or by ACPI request,
	    If shutdown timeout expired the Vm will be forcebly stopped.
            If graceful shutdown is not used, the machine will be
            simply "unplugged"
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Stop, (
		PRL_HANDLE hVm,
		PRL_BOOL bGraceful
		) );

/* Stop the specified virtual machine. To get the return code
   from the PHT_JOB object, use the PrlJob_GetRetCode function.
   Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :	A handle of type PHT_VIRTUAL_MACHINE identifying the
		virtual machine.
   nStopMode:	Virtual machine stop mode, an enumerator from the
		PRL_VM_STOP_MODE enumeration.
   nFlags:	Virtual machine stop option, an enumerator from the
		PRL_VM_STOP_FLAGS enumeration.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */

PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlVm_StopEx, (
		PRL_HANDLE hVm,
                PRL_UINT32 nStopMode,
                PRL_UINT32 nFlags
		) );

/* Pauses the specified virtual machine. A paused virtual
   machine can be resumed using the PrlVm_Start function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :    A handle of type PHT_VIRTUAL_MACHINE identifying the
            virtual machine.
   bAcpi :  Specifies if ACPI should be used to pause the virtual
            machine. If set to PRL_FALSE, ACPI will not be used.
            In this case, all processes of the virtual machine
            will simply be stopped. If the parameter is set to
            PRL_TRUE, ACPI will be used to pause the machine.
            With ACPI, the machine will be paused using a method
            supported by the guest operating system (sleep,
            hibernate, etc.). The actual method used depends on
            the type and configuration of the guest OS.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_Start                                                    */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Pause, (
		PRL_HANDLE hVm,
		PRL_BOOL bAcpi
		) );

/* Resets the specified virtual machine. This function is an
   equivalent of PrlVm_Stop and PrlVm_Start functions executed
   in succession. The stop operation will NOT use ACPI, so the
   entire reset operation will resemble the "Reset" button
   pressed on a physical box.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Reset, (
		PRL_HANDLE hVm
		) );

/* Suspends the specified virtual machine. When a running
   virtual machine is suspended, the state of the virtual
   machine processes is saved into a file on the host and after
   that the machine is stopped. To resume the machine, use the
   PrlVm_Resume function. If, after suspending a machine, you
   don't want to resume it but would like to return to the
   previous state, you may call the PrlVm_DropSuspendedState
   function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_Resume

   PrlVm_DropSuspendedState                                       */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Suspend, (
		PRL_HANDLE hVm
		) );

/* %VM_ONLY%

   Retrieves the virtual machine screen state before it was
   suspending. To get the return code from the PHT_JOB object,
   use the PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_VM_IS_NOT_SUSPENDED - the virtual machine is not in
   the suspended state.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_GetSuspendedScreen, (
		PRL_HANDLE hVm
		) );

/* Resumes a virtual machine that was previously suspended with
   the PrlVm_Suspend call. To get the return code from the
   PHT_JOB object, use the PrlJob_GetRetCode function. Possible
   values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_Suspend                                                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Resume, (
		PRL_HANDLE hVm
		) );

/* Resets a suspended virtual machine. When a virtual machine is
   suspended with the PrlVm_Suspend call, the current state of
   the virtual machine processes is saved into a temporary file
   on the hard disk, and the virtual machine is stopped. When a
   suspended virtual machine is resumed with the PrlVm_Resume
   call, the data in the temp file is used to restore the state
   of the virtual machine processes. The PrlVm_DropSuspendedState
   function resets a suspended virtual machine by deleting the
   temp file from the drive Once the function successfully
   completes, the virtual machine can no longer be resumed with
   the PrlVm_Resume call. It still can be started normally,
   however.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_DropSuspendedState, (
		PRL_HANDLE hVm
		) );

/* Creates a virtual machine snapshot. To get the return code
   from the PHT_JOB object, use the PrlJob_GetRetCode function.
   Possible value are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :        A handle of type PHT_VIRTUAL_MACHINE
                   identifying the virtual machine.
   sName :         Snapshot name.
   sDescription :  Snapshot description.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_DeleteSnapshot

   PrlVm_GetSnapshotsTreeEx

   PrlVm_SwitchToSnapshot

   PrlVm_UpdateSnapshotData                                     */
PRL_ASYNC_METHOD_DECL( VIRTUOZZO_API_VER_2,
					   PrlVm_CreateSnapshot, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sName,
		PRL_CONST_STR sDescription
		) );

/* Creates a virtual machine snapshot. To get the return code
   from the PHT_JOB object, use the PrlJob_GetRetCode function.
   Possible value are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :        A handle of type PHT_VIRTUAL_MACHINE
                   identifying the virtual machine.
   sName :         Snapshot name.
   sDescription :  Snapshot description.
   nFlag :	   Flags from enum PRL_CREATE_SNAPSHOT_FLAGS
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_DeleteSnapshot

   PrlVm_GetSnapshotsTreeEx

   PrlVm_SwitchToSnapshot

   PrlVm_UpdateSnapshotData                                     */
PRL_ASYNC_METHOD_DECL( VIRTUOZZO_API_VER_7,
					   PrlVm_CreateSnapshotEx, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sName,
		PRL_CONST_STR sDescription,
		PRL_UINT32 nFlags
		) );

/* Retrieves snapshot information for the specified virtual
   machine. Snapshot information is returned as an XML file
   containing the snapshot tree. Some of the important XML
   elements are:

   <b>SavedStateItem</b> -- the root element of the branch
   containing information about an individual snapshot. The
   element has an attribute "guid" containing the snapshot ID.
   The ID is required when performing other operations on a
   snapshot.

   <b>Name</b> -- contains the snapshot name.

   <b>DateTime</b> -- the snapshot creation date and time.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get a snapshot tree from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamAsString function to obtain a
        string value containing the snapshots tree XML data.
   Please note that no parser is provided to traverse the
   returned XML. To use this functionality, you will have to
   parse the returned XML yourself. This function will be
   replaced in the future releases of the SDK.
   Parameters
   handle :  A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   flags : A flags from enum PRL_GET_SNAPSHOT_TREE_FLAGS and PRL_API_COMMAND_FLAGS
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_CreateSnapshot

   PrlVm_DeleteSnapshot

   PrlVm_SwitchToSnapshot

   PrlVm_UpdateSnapshotData                                       */
PRL_ASYNC_METHOD_DECL( VIRTUOZZO_API_VER_2,
					   PrlVm_GetSnapshotsTree, (
		PRL_HANDLE hVm
		) );
PRL_ASYNC_METHOD_DECL( VIRTUOZZO_API_VER_3,
					  PrlVm_GetSnapshotsTreeEx, (
					  PRL_HANDLE hVm,
					  PRL_UINT32 nFlags
					  ) );

/* %VM_ONLY%

   Lets to exclusively lock VM for current session. After this
   call just current sesiion be able to operate with locked VM
   until PrlVm_Unlock will be called or current session will be
   closed. If VM was already locked by some another session
   (or the same one) then lock operation will fail.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:
   PRL_ERR_INVALID_ARG - invalid handle was passed.
   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   nReserved : reserved param for further purposes
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_Lock, (
		PRL_HANDLE hVm,
		PRL_UINT32 nReserved
		) );

/* %VM_ONLY%

   Lets to unlock previously locked with PrlVm_Lock call VM.
   Note that just VM lock owner session can unlock VM.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   nReserved : reserved param for further purposes
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_Unlock, (
		PRL_HANDLE hVm,
		PRL_UINT32 nReserved
		) );


/* Reverts the specified virtual machine to the specified
   snapshot. To get the return code from the PHT_JOB object, use
   the PrlJob_GetRetCode function. Possible value are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :         A handle of type PHT_VIRTUAL_MACHINE
                    identifying the virtual machine.
   sSnapshotUuid :  The ID of a snapshot to revert to. To obtain
                    a list of the available snapshots and their
                    IDs, use the PrlVm_GetSnapshotsTreeEx function.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_CreateSnapshot

   PrlVm_DeleteSnapshot

   PrlVm_GetSnapshotsTreeEx

   PrlVm_UpdateSnapshotData                                       */
PRL_ASYNC_METHOD_DECL( VIRTUOZZO_API_VER_2,
					   PrlVm_SwitchToSnapshot, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sSnapshotUuid
		) );

/* Reverts the specified virtual machine to the specified
   snapshot. To get the return code from the PHT_JOB object, use
   the PrlJob_GetRetCode function. Possible value are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :         A handle of type PHT_VIRTUAL_MACHINE
                    identifying the virtual machine.
   sSnapshotUuid :  The ID of a snapshot to revert to. To obtain
                    a list of the available snapshots and their
                    IDs, use the PrlVm_GetSnapshotsTreeEx function.
   nFlags        :  Bitset of flags - from enum PRL_SWITCH_SNAPSHOT_FLAGS
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_CreateSnapshot

   PrlVm_DeleteSnapshot

   PrlVm_GetSnapshotsTreeEx

   PrlVm_UpdateSnapshotData                                       */
PRL_ASYNC_METHOD_DECL( VIRTUOZZO_API_VER_5,
                                           PrlVm_SwitchToSnapshotEx, (
                PRL_HANDLE hVm,
                PRL_CONST_STR sSnapshotUuid,
                PRL_UINT32 nFlags
                ) );


/* Deleted the specified virtual machine snapshot. To get the
   return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :            A handle of type PHT_VIRTUAL_MACHINE
                    identifying the virtual machine.
   sSnapshotUuid :  Snapshot ID. To obtain a snapshot ID use the
                    PrlVm_GetSnapshotsTreeEx function.
   bChild :         Specifies whether snapshots derived from the
                    specified snapshot should be deleted or not.
                    True \- delete child snapshots. False \-
                    don't delete child snapshots.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_CreateSnapshot

   PrlVm_GetSnapshotsTreeEx

   PrlVm_SwitchToSnapshot

   PrlVm_UpdateSnapshotData                                      */

PRL_ASYNC_METHOD_DECL( VIRTUOZZO_API_VER_2,
					   PrlVm_DeleteSnapshot, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sSnapshotUuid,
		PRL_BOOL bChild
		) );

/* Modifies the specified virtual machine snapshot name and
   \description. To get the return code from the PHT_JOB object,
   use the PrlJob_GetRetCode function. Possible value are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :           A handle of type PHT_VIRTUAL_MACHINE
                      identifying the virtual machine.
   sSnapshotUuid :    The ID of the snapshot to modify. To obtain
                      a list of the available snapshots and their
                      ID's, use the PrlVm_GetSnapshotsTreeEx
                      function.
   sNewName :         New snapshot name.
   sNewDescription :  New snapshot description.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_CreateSnapshot

   PrlVm_DeleteSnapshot

   PrlVm_GetSnapshotsTreeEx

   PrlVm_SwitchToSnapshot                                         */
PRL_ASYNC_METHOD_DECL( VIRTUOZZO_API_VER_2,
					  PrlVm_UpdateSnapshotData, (
					  PRL_HANDLE hVm,
					  PRL_CONST_STR sSnapshotUuid,
					  PRL_CONST_STR sNewName,
					  PRL_CONST_STR sNewDescription
					  ) );



/* Clones an existing virtual machine. The function creates an
   exact copy of the specified virtual machine on the same
   host using the specified new name and location. In order to
   perform a clone operation, the original machine must be
   registered with the Dispatcher Service. A clone can be created as a
   virtual machine or as a template. To create a template, set
   the bCreateTemplate parameter value to PRL_TRUE. The function
   can also be used to create new virtual machines from
   templates. In order to that, the hVm parameter must contain a
   handle to a template, the name and the path parameters must
   be set to desired values, and the bCreateTemplate parameter
   must be set to PRL_FALSE.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :               A handle of type PHT_VIRTUAL_MACHINE
                       identifying the virtual machine.
   new_vm_name :       The name to use for the new virtual
                       machine.
   new_vm_root_path :  Name and path of the directory where the
                       new virtual machine should be created. To
                       create the machine in a default directory,
                       pass an empty string.
   bCreateTemplate :   Specifies whether the clone should be
                       created as a virtual machine or as a
                       template. To create a template, set this
                       parameter value to PRL_TRUE. To create a
                       virtual machine, set it to PRL_FALSE. When
                       creating a virtual machine from a
                       template, set the parameter value to
                       PRL_FALSE.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Clone, (
		PRL_HANDLE hVm,
		PRL_CONST_STR new_vm_name,
		PRL_CONST_STR new_vm_root_path,
		PRL_BOOL bCreateTemplate
		) );


/* Clones an existing virtual machine. The function creates an
   exact copy of the specified virtual machine on the same
   host using the specified new name and location. In order to
   perform a clone operation, the original machine must be
   registered with the Dispatcher Service. A clone can be created as a
   virtual machine or as a template. To create a template, set
   the nFlags parameter to PACF_CLONEVM_TO_TEMPLATE value. The function
   can also be used to create new virtual machines from
   templates. In order to that, the hVm parameter must contain a
   handle to a template, the name and the path parameters must
   be set to desired values, and the bCreateTemplate parameter
   must be set to PRL_FALSE.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :               A handle of type PHT_VIRTUAL_MACHINE
                       identifying the virtual machine.
   new_vm_name :       The name to use for the new virtual
                       machine.
   new_vm_uuid :       The uuid to use for the new virtual
                       machine.
   new_vm_root_path :  Name and path of the directory where the
                       new virtual machine should be created. To
                       create the machine in a default directory,
                       pass an empty string.
   nFlags :           Clone flags (PCVF_CLONE_TO_TEMPLATE, PCVF_CHANGE_SID,
                   PCVF_LINKED_CLONE, PCVF_IMPORT_BOOT_CAMP).
                                  Also PACF_NON_INTERACTIVE_MODE can be used to specify non
                                  interactive session working mode (remote side won't ask
                                  questions during operation progress in this case).
                                  Please note that clone operation can take
                                  it's own exclusive lock of virtual machine (in change SID or
                                  linked clone modes) - so VM shouldn't be exclusively locked
                                  with PrlVm_Lock call before clone operation performed.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_7,
		PrlVm_CloneWithUuid, (
			PRL_HANDLE hVm,
			PRL_CONST_STR new_vm_name,
			PRL_CONST_STR new_vm_uuid,
			PRL_CONST_STR new_vm_root_path,
			PRL_UINT32 nFlags
			) );


/* Migrates an existing virtual machine to another host. To get
   the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :               A handle of type PHT_VIRTUAL_MACHINE
                       identifying the virtual machine.
   hTargetServer :     A handle of type PHT_SERVER identifying
                       the target Dispatcher Service.
   target_home_path :  Target VM home path (i.e. where VM will be
                       placed on target system). Empty string
                       accounts as default VM directory path
   migration_flags :   Flags that specify migration type and etc.
                       See acceptable flags at PrlCommandsFlags.h
   reserved_flags :    Reserved parameter for further purposes
   force_operation :   Specifies to process VM migration action
                       without asking any additional questions
                       from the Dispatcher Service side
                       (non\-interactive clients should use this
                       option).
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_Migrate, (
		PRL_HANDLE hVm,
		PRL_HANDLE hTargetServer,
		PRL_CONST_STR target_home_path,
		PRL_UINT32 migration_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		) );

/* Clones an existing virtual machine. The function creates an
   exact copy of the specified virtual machine on the same
   host using the specified new name and location. In order to
   perform a clone operation, the original machine must be
   registered with the Dispatcher Service. A clone can be created as a
   virtual machine or as a template. To create a template, set
   the nFlags parameter to PACF_CLONEVM_TO_TEMPLATE value. The function
   can also be used to create new virtual machines from
   templates. In order to that, the hVm parameter must contain a
   handle to a template, the name and the path parameters must
   be set to desired values, and the bCreateTemplate parameter
   must be set to PRL_FALSE.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :               A handle of type PHT_VIRTUAL_MACHINE
                       identifying the virtual machine.
   new_vm_name :       The name to use for the new virtual
                       machine.
   new_vm_root_path :  Name and path of the directory where the
                       new virtual machine should be created. To
                       create the machine in a default directory,
                       pass an empty string.
   nFlags :	       Clone flags (PCVF_CLONE_TO_TEMPLATE, PCVF_CHANGE_SID,
                   PCVF_LINKED_CLONE, PCVF_IMPORT_BOOT_CAMP).
				   Also PACF_NON_INTERACTIVE_MODE can be used to specify non
				   interactive session working mode (remote side won't ask
				   questions during operation progress in this case).
				   Please note that clone operation can take
				   it's own exclusive lock of virtual machine (in change SID or
				   linked clone modes) - so VM shouldn't be exclusively locked
				   with PrlVm_Lock call before clone operation performed.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_CloneEx, (
		PRL_HANDLE hVm,
		PRL_CONST_STR new_vm_name,
		PRL_CONST_STR new_vm_root_path,
		PRL_UINT32 nFlags
		) );

/* Migrates an existing virtual machine to another host.
   This is an extended version of PrlVm_Migrate function that allows to
   initiate VM migration process without active connection to target
   Dispatcher Service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :               A handle of type PHT_VIRTUAL_MACHINE
                       identifying the virtual machine.
   target_host :       The name of the target host machine.
   target_port :       The port number on the target host.
   target_session_id : The target Dispatcher Service session ID. This ID can be
                       retrieved with PrlLoginResponse_GetSessionUuid from the
					   response of the login request.
   target_home_path :  Target VM home path (i.e. where VM will be
                       placed on target system). Empty string accounts
					   as default VM directory path
   migration_flags :   Flags that specify migration type and etc.
                       See acceptable flags at PrlCommandsFlags.h
   reserved_flags :    Reserved parameter for further purposes
   force_operation :   Specifies to process VM migration action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_MigrateEx, (
		PRL_HANDLE hVm,
		PRL_CONST_STR target_host,
		PRL_UINT32 target_port,
		PRL_CONST_STR target_session_id,
		PRL_CONST_STR target_home_path,
		PRL_UINT32 migration_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		) );

/* Migrates an existing virtual machine to another host. To get
   the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :               A handle of type PHT_VIRTUAL_MACHINE
                       identifying the virtual machine.
   hTargetServer :     A handle of type PHT_SERVER identifying
                       the target Dispatcher Service.
   target_name :       Target VM name (i.e. what name will be
                       given to VM on target system). Specify empty
                       string to preserve VM name from source system.
   target_home_path :  Target VM home path (i.e. where VM will be
                       placed on target system). Empty string
                       accounts as default VM directory path
   migration_flags :   Flags that specify migration type and etc.
                       See acceptable flags at PrlCommandsFlags.h
   reserved_flags :    Reserved parameter for further purposes
   force_operation :   Specifies to process VM migration action
                       without asking any additional questions
                       from the Dispatcher Service side
                       (non\-interactive clients should use this
                       option).
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_MigrateWithRename, (
		PRL_HANDLE hVm,
		PRL_HANDLE hTargetServer,
		PRL_CONST_STR target_name,
		PRL_CONST_STR target_home_path,
		PRL_UINT32 migration_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		) );

/* Migrates an existing virtual machine to another host.
   This is an extended version of PrlVm_MigrateWithRename function that allows to
   initiate VM migration process without active connection to target
   Dispatcher Service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :               A handle of type PHT_VIRTUAL_MACHINE
                       identifying the virtual machine.
   target_host :       The name of the target host machine.
   target_port :       The port number on the target host.
   target_session_id : The target Dispatcher Service session ID. This ID can be
                       retrieved with PrlLoginResponse_GetSessionUuid from the
					   response of the login request.
   target_home_path :  Target VM home path (i.e. where VM will be
                       placed on target system). Empty string accounts
					   as default VM directory path
   migration_flags :   Flags that specify migration type and etc.
                       See acceptable flags at PrlCommandsFlags.h
   reserved_flags :    Reserved parameter for further purposes
   force_operation :   Specifies to process VM migration action without asking
                       any additional questions from the Dispatcher Service side
                       (non-interactive clients should use this option).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_MigrateWithRenameEx, (
		PRL_HANDLE hVm,
		PRL_CONST_STR target_host,
		PRL_UINT32 target_port,
		PRL_CONST_STR target_session_id,
		PRL_CONST_STR target_name,
		PRL_CONST_STR target_home_path,
		PRL_UINT32 migration_flags,
		PRL_UINT32 reserved_flags,
		PRL_BOOL force_operation
		) );

/* Cancels the migration operation that was started with
   PrlVm_Migrate or PrlVm_MigrateEx. To get the return code from
   the PHT_JOB object, use the PrlJob_GetRetCode function.
   Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_MigrateCancel, (
		PRL_HANDLE hVm
		) );

/* Deletes the specified virtual machine from the host. This
   function un-registers the machine from the Dispatcher Service
   and deletes all its files. This operation cannot be undone.
   The specified machine must be registered with the Dispatcher Service. To
   un-register a machine from the Dispatcher Service without deleting any of
   its file, use the PrlVm_Unreg function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :           A handle of type PHT_VIRTUAL_MACHINE
                   identifying the virtual machine.
   hDevicesList :  A handle of type PHT_STRINGS_LIST containing a
                   list of files (devices) to be deleted from the
                   virtual machine directory. If the list is
                   empty or the parameter contains a null
                   pointer, the entire virtual machine directory
                   will be deleted from the host. If the list
                   contains one or more items, only the specified
                   files will be delete (the rest of the virtual
                   machines files will remain on the host in
                   the original directory).
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Delete, (
		PRL_HANDLE hVm,
		PRL_HANDLE hDevicesList
		) );

/* The PrlVm_GetProblemReport function obtains a problem report
   on abnormal virtual machine termination. If a virtual machine
   stops running abnormally, you call this function to obtain a
   report from the Dispatcher Service describing the problem.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParamAsString function to obtain a
   string containing the report.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_GetProblemReport, (
					  PRL_HANDLE hVm
					  ) );

/* The PrlVm_GetPackedProblemReport function obtains a Packed problem report
on abnormal virtual machine termination. If a virtual machine
stops running abnormally, you call this function to obtain a
report from the Dispatcher Service describing the problem.

To get the return code from the PHT_JOB object, use the
PrlJob_GetRetCode function. Possible values are:

PRL_ERR_INVALID_ARG - invalid handle or null pointer was
passed.

PRL_ERR_SUCCESS - function completed successfully.

To get the results from the PHT_JOB object:

Use the PrlJob_GetResult function to obtain a handle to the
PHT_RESULT object.

Use the PrlResult_GetParamAsString function to obtain a
string containing path to the report directory.
Parameters
hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
virtual machine.
PRL_UINT32 nFlags - reserved flags parameter
Returns
A handle of type PHT_JOB containing the results of this
asynchronous operation or PRL_INVALID_HANDLE if there's not
enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						  PrlVm_GetPackedProblemReport, (
						  PRL_HANDLE hVm,
						  PRL_UINT32 nFlags
						  ) );


/* Obtains a handle of type PHT_VM_INFO containing the specified
   virtual machine state and access rights information. To get
   the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParam function to obtain the PHT_VM_INFO
   handle.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_GetState, (
		PRL_HANDLE hVm
		) );


/* %VM_ONLY%

   Automatically generates a name for a virtual device which
   will be unique within a home directory of the specified
   virtual machine. To get the return code from the PHT_JOB
   object, use the PrlJob_GetRetCode function. Possible value
   are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the home directory information is missing
   in the specified virtual machine object. If this is a new
   virtual machine, try updating the local object by calling the
   PrlVm_RefreshConfig or PrlSrv_GetVmList function.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVM :              A handle of type PHT_VIRTUAL_MACHINE
                      identifying the virtual machine.
   sFilenamePrefix :  A prefix to add to the name. Pass a null
                      pointer to use a default value\: "tmpfile".
                      The name will be generated by adding a
                      number to the prefix (e.g. tmpfile1, tmpfile2,
                      tmpfile3, etc.)
   sFilenameSuffix :  A suffix to add to the name. Pass a null
                      pointer for no suffix.
   sIndexDelimiter :  A character(s) that will be inserted
                      between the prefix and the index (e.g. tmpfile_1,
                      tmpfile_2, tmpfile_3, etc). You can specify
                      a null pointer or an empty string if you
                      don't want to insert any extra characters
                      there.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                         */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_GenerateVmDevFilename, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sFilenamePrefix,
		PRL_CONST_STR sFilenameSuffix,
		PRL_CONST_STR sIndexDelimiter
		) );


/* Obtains a handle of type PHT_VM_TOOLS_INFO containing
   information about the state of the Tools package in
   a virtual machine.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_GetToolsState, (
		PRL_HANDLE hVm
		) );

/* %VM_ONLY%

   The PrlVm_RefreshConfig function updates the configuration
   information contained in the specified virtual machine object
   (handle). When you obtain a virtual machine handle that
   refers to an existing virtual machine, it is populated with
   the virtual machine configuration data. When working with
   configuration data, you always work with this local copy.
   Naturally, the configuration can change on the Dispatcher Service side at
   any time. The PrlVm_RefreshConfig function allows to re-retrieve
   the configuration data from the Dispatcher Service. The function can be
   used when an event of type PET_DSP_EVT_VM_CONFIG_CHANGED is
   triggered (the event notifies the client that the virtual
   machine configuration has been modified), or on unsuccessful
   PrlVm_Commit operation, or at any time you want to
   synchronize your local virtual machine object with the
   Dispatcher Service. Please note that the function replaces the local
   data, so if you made any modification to it, the changes will
   be lost.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine. This is the object that will be
          updated.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_RefreshConfig, (
		PRL_HANDLE hVm
		) );

/* The PrlVm_RefreshConfigEx function updates the configuration
   information contained in the specified virtual machine object
   (handle). When you obtain a virtual machine handle that
   refers to an existing virtual machine, it is populated with
   the virtual machine configuration data. When working with
   configuration data, you always work with this local copy.
   Naturally, the configuration can change on the Dispatcher Service side at
   any time. The PrlVm_RefreshConfigEx function allows to re-retrieve
   the configuration data from the Dispatcher Service. The function can be
   used when an event of type PET_DSP_EVT_VM_CONFIG_CHANGED is
   triggered (the event notifies the client that the virtual
   machine configuration has been modified), or on unsuccessful
   PrlVm_Commit operation, or at any time you want to
   synchronize your local virtual machine object with the
   Dispatcher Service. Please note that the function replaces the local
   data, so if you made any modification to it, the changes will
   be lost.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :    A handle of type PHT_VIRTUAL_MACHINE identifying the
            virtual machine. This is the object that will be
            updated.
   nFlags : A bitset of flags.

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_RefreshConfigEx, (
		PRL_HANDLE hVm,
		PRL_UINT32 nFlags
		) );

/* Creates a new console session or binds to an existing GUI
   session in a virtual machine. <b>Note</b>: To use this
   function, Tools must be installed in the target
   virtual machine.

   This function is used when you want to run a program in a
   virtual machine or perform an administrative task that
   require native guest operating system login. Depending on the
   type of the program that you would like to run, the login
   must be performed using one of the following methods:
     * To run a GUI application, call this function passing the
       PRL_CURRENT_GUEST_OS_SESSION constant in the sUserName
       parameter. The reason is the function can only bind to an
       existing GUI session and cannot create a new one. Prior to
       calling this function, a GUI session must already be open in
       a virtual machine (the machine must be running and a user
       must be logged in to it). The user password parameter can be
       blank.
     * To run a console program, the sUserName parameter can
       contain the PRL_PRIVILEGED_GUEST_OS_SESSION constant or a
       valid user name and password. When using
       PRL_PRIVILEGED_GUEST_OS_SESSION, the function will bind to an
       existing session under which Dispatcher Service is running.
       This session belongs to a privileged user (e.g. root on Linux
       or LocalSystem on Windows), so it can be used to perform
       administrative tasks. When specifying a user name and
       password, a new session will be created in the virtual
       machine.
   On successful login, the function obtains a handle of type
   PHT_VM_GUEST_SESSION (see below for the instructions on how
   to get the handle from the PHT_JOB object). You can use the
   handle functions to run programs in a virtual machine and to
   perform other supported operations. See the
   PHT_VM_GUEST_SESSION handle description for a complete list
   of functions.

   When the session is no longer needed, it should be closed
   using the PrlVmGuest_Logout function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG &#45; invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS &#45; function completed successfully.

   To get the PHT_VM_GUEST_SESSION handle from the returned
   PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain the
        PHT_RESULT handle.
     2. Use the PrlResult_GetParam to obtain the
        PHT_VM_GUEST_SESSION handle.
   Parameters
   hVm :            A handle of type PHT_VIRTUAL_MACHINE
                    identifying the virtual machine.
   sUserName :      User name. This parameter must contain a
                    valid guest OS user name or one of the
                    following constants\:<p />PRL_CURRENT_GUEST_OS_SESSION
                    \-\- binds to an existing GUI session in a
                    virtual machine.<p />PRL_PRIVILEGED_GUEST_OS_SESSION
                    \-\- binds to an existing session under which
                    Dispatcher Service is running. This session
                    belongs to a privileged user (e.g. root on
                    Linux or LocalSystem on Windows), so it can
                    be used to perform administrative tasks.<p />Note
                    that in order to use the constants above, the
                    current user must be an administrator of the
                    host OS or be the owner of the target virtual
                    machine.
   sUserPassword :  User password. If the sUserName parameter
                    contains one of the constants (see above)
                    then this parameter can contain an empty
                    string.
   nFlags :         A bitset of flags. Must be 0 in the current
                    API implementation.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVmGuest_RunProgram

   PrlVmGuest_Logout                                                       */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_LoginInGuest, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sUserName,
		PRL_CONST_STR sUserPassword,
		PRL_UINT32 nFlags
		) );

/* Starts a VNC server for the specified virtual machine. To get
   the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer for user
   name was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :        A handle of type PHT_VIRTUAL_MACHINE identifying
                the virtual machine.
   nReserved :  Reserved parameter for future purposes. Must
                contain 0 in the current implementation.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_StopVncServer                                           */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_StartVncServer, (
		PRL_HANDLE hVm,
		PRL_UINT32 nReserved
		) );

/* Stops the VNC server that was previously started for a
   virtual machine. To get the return code from the PHT_JOB
   object, use the PrlJob_GetRetCode function. Possible values
   are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer for user
   name was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :        A handle of type PHT_VIRTUAL_MACHINE identifying
                the virtual machine.
   nReserved :  Reserved parameter for future purposes. Must be 0
                in the current implementation.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_StartVncServer                                           */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_StopVncServer, (
		PRL_HANDLE hVm,
		PRL_UINT32 nReserved
		) );


/* The PrlVm_SetConfig is reserved and does nothing.
   Parameters
   hVm :    A handle of type PHT_VIRTUAL_MACHINE identifying the
            virtual machine.
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_SetConfig, (
		PRL_HANDLE hVm,
		PRL_HANDLE hVmCfg
		) );


/* Obtains a handle of type PHT_VM_CONFIGURATION.
   Parameters
   hVm :      A handle of type PHT_VIRTUAL_MACHINE identifying
              the virtual machine.
   phVmCfg :  [out] A pointer to a variable that receives the
              handle of type PHT_VM_CONFIGURATION.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_GetConfig, (
		PRL_HANDLE hVm,
		PRL_HANDLE_PTR phVmCfg
		) );

/* Obtains a handle of type PHT_SYSTEM_STATISTICS containing the
   virtual machine resource usage information, including CPU(s),
   memory, disk drives, processes, user session, system uptime,
   network packets, etc. To get the return code from the PHT_JOB
   object, use the PrlJob_GetRetCode function. Possible value
   are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParam function to obtain a handle of
   type PHT_SYSTEM_STATISTICS containing the resource usage
   data.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_GetStatistics, (
		PRL_HANDLE hVm
		) );
/* The PrlVm_GetStatisticsEx function is the same as PrlVm_GetStatistics
   function but with the flags
   Parameters
   hVm :    A handle of type PHT_VIRTUAL_MACHINE identifying the
            virtual machine.
   nFlags : Bitset of flags - from enum PRL_API_COMMAND_FLAGS
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_4,
						  PrlVm_GetStatisticsEx, (
						  PRL_HANDLE hVm,
						  PRL_UINT32 nFlags
						  ) );

/* Subscribes to receive the specified virtual machine
   performance statistics on a periodic basis. To obtain
   statistics, an event handler (callback function) is required.
   Within the event handler, first check the type of event.
   Events of type PET_DSP_EVT_VM_STATISTICS_UPDATED indicate an
   event containing performance data. To access the statistics
   handle (a handle of type PHT_SYSTEM_STATISTICS), first
   extract the event parameter using PrlEvent_GetParam, then
   convert the result (which will be a handle to an object of
   type PHT_EVENT_PARAMETER) to a handle using
   PrlEvtPrm_ToHandle. The functions that operate on
   PHT_SYSTEM_STATISTICS references can then be used to obtain
   statistics data.

   For the event handler to be called, it is necessary to
   register it with PrlVm_RegEventHandler. When statistics data
   is no longer required, unsubscribe from statistics events
   using PrlVm_UnsubscribeFromGuestStatistics. When events are
   no longer required, unregister the event handler using PrlVm_UnregEventHandler.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_SubscribeToHostStatistics
                                                                                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_SubscribeToGuestStatistics, (
		PRL_HANDLE hVm
		) );

/* Cancels the performance statistics subscription that was
   previously initiated using the
   PrlVm_SubscribeToGuestStatistics function. To get the return
   code from the PHT_JOB object, use the PrlJob_GetRetCode
   function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                 */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_UnsubscribeFromGuestStatistics, (
		PRL_HANDLE hVm
		) );

/* Allows to subscribe to periodicaly receive virtual machine
   perfomance statistics. To obtain statistics, an event handler
   (callback function) is required. Within the event handler,
   first check the type of event. Events of type
   PET_DSP_EVT_VM_PERFSTATS indicate an event
   containing statistics data. The perfomace statistics repesented by
   parameters of Event object (a handle of type PHT_EVENT).

   For the event handler to be called, it is necessary to
   register it with PrlSrv_RegEventHandler. When statistics data
   is no longer required, unsubscribe from statistics events
   using PrlVm_UnsubscribeFromHostPerfStats. When events are
   no longer required, unregister the event handler using
   PrlVm_UnregEventHandler.

   If method is called mutliple times, the 'filter'
   value is updated each time on the Dispatcher Service side.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :      A handle of type PHT_VIRTUAL_MACHINE
              identifying the virtual machine.
   sFilter :  String representing statistics filter,
              use char '*' at the end to get subset of statistic.
              Could be NULL, to get all statistic.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_SubscribeToPerfStats, (
                           PRL_HANDLE hVm, PRL_CONST_STR sFilter
                           ) );

/* Cancels the Dispatcher Service performance statistics subscription
   that was started using the PrlVm_SubscribeToPerfStats function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :      A handle of type PHT_VIRTUAL_MACHINE
              identifying the virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                    */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_UnsubscribeFromPerfStats, (
                           PRL_HANDLE hVm
                           ) );

/* Obtains a handle of type PHT_EVENT containing the virtual
   machine performance statistics. To get the return code from
   the PHT_JOB object, use the PrlJob_GetRetCode function.
   Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_EVENT.
   Parameters
   hVm :      A handle of type PHT_VIRTUAL_MACHINE identifying the
              virtual machine.
   sFilter :  String representing statistics filter, use char '*'
              at the end to get subset of statistic. Could be
              NULL, to get all statistic.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                    */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_GetPerfStats, (
                           PRL_HANDLE hVm, PRL_CONST_STR sFilter
                           ) );


/* Creates a new virtual machine and registers it with the
   Dispatcher Service. The steps involved in creating a typical
   virtual machine are:
     1. Obtain a handle to a new virtual machine object using
        the PrlSrv_CreateVm function.
     2. Populate the object with the desired configuration data.
     3. Choose a name for the virtual machine and set it using
        the PrlVmCfg_SetName function.
     4. Create and register the new machine with a Dispatcher
        Service using the PrlVm_Reg function. This step will create
        the necessary virtual machine files on the host and
        will add the machine to the Dispatcher Service VM registry. The
        directory containing the virtual machine files will have the
        same name as the name you've chosen for your virtual machine.
        The directory will be created in the default VM root
        directory for this Dispatcher Service. If you would like to create the
        virtual machine directory in a different location, you may
        specify the desired parent directory name and path.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :                  A handle of type PHT_VIRTUAL_MACHINE
                          identifying the virtual machine.
   sVmParentPath :        Optional. Name and path of the parent
                          directory where the virtual machine
                          directory should be created. If this
                          parameter is omitted (null pointer or
                          empty string is passed), the new
                          virtual machine directory will be
                          created in the default directory for
                          this Dispatcher Service.
   nFlags :               Flags set which specifies way of
                          virtual machine creation. Use
						  PACF_NON_INTERACTIVE_MODE flag here
						  instead of deprecated bNonInteractiveMode
						  parameter.
						  Other bitset of flags - from enum PRL_CREATE_DISK_IMAGE_FLAGS
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                       */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_4,
						   PrlVm_RegEx, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sVmParentPath,
		PRL_UINT32 nFlags
		) );
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Reg, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sVmParentPath,
		PRL_BOOL bNonInteractiveMode
		) );

/* Unregisters the specified virtual machine from the Dispatcher
   Service. This call removes the virtual machine from the
   Dispatcher Service registry, but does not delete the virtual
   machine files from the hard drive. You can re-register a
   machine with the Dispatcher Service at any time using the
   PrlSrv_RegisterVm or PrlSrv_RegisterVmEx functions call.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Unreg, (
		PRL_HANDLE hVm
		) );

/* Restores the registered virtual machine. To get the return
   code from the PHT_JOB object, use the PrlJob_GetRetCode
   function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_VIRTUAL_MACHINE.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation, including the return code and a
   handle of type PHT_VIRTUAL_MACHINE containing information
   about the virtual machine that was registered.

   PRL_INVALID_HANDLE if there's not enough memory to
   instantiate the job object.                                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_Restore, (
		PRL_HANDLE hVm
		) );

/* The PrlVm_BeginEdit function is used in modifying a virtual
   machine configuration. The PrlVm_BeginEdit and the
   PrlVm_Commit functions are used to detect collisions with
   other clients trying to modify the same virtual machine at
   the same time. The PrlVm_BeginEdit call timestamps the
   beginning of the editing operation. It does not lock the
   machine, so other clients can modify the same machine at the
   same time. When you are done making the changes, you must
   call the PrlVm_Commit function to save them in the virtual
   machine configuration. The function will verify that the
   configuration has not been modified by another client. If a
   collision is detected, your changes will be rejected. To
   ensure that you always work with the most recent
   configuration data, the PrlVm_BeginEdit function reloads it
   when called. If you first modified the data in your program
   and then called the PrlVm_BeginEdit function, your changes
   will be lost.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                 */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_BeginEdit, (
		PRL_HANDLE hVm
		) );

/* The PrlVm_Commit function is used in modifying a virtual
   machine configuration. The PrlVm_BeginEdit and the
   PrlVm_Commit functions are used to detect collisions with
   other clients trying to modify the same virtual machine at
   the same time. The PrlVm_BeginEdit call timestamps the
   beginning of the editing operation. It does not lock the
   machine, so other clients can modify the same machine at the
   same time. When you are done making the changes, you must
   call the PrlVm_Commit function to save them in the virtual
   machine configuration. The function will verify that the
   configuration has not been modified by another client. If a
   collision is detected, your changes will be rejected.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                 */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_Commit, (
		PRL_HANDLE hVm
		) );

/*  The PrlVm_CommitEx function is the same as PrlVm_Commit function but with the flags
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
		  nFlags : Bitset of flags - from enum PRL_API_COMMAND_FLAGS
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						  PrlVm_CommitEx, (
						  PRL_HANDLE hVm,
						  PRL_UINT32 nFlags
						  ) );


/* Registers an event handler (callback function) with the
   virtual machine. To use the callback functionality, you first
   have to implement a callback function. The callback function
   must have a specific signature (see PRL_METHOD_PTR for the
   prototype). After you register an event handler with a
   virtual machine, the callback function will be called every
   time an event is generated in the machine.

   You can also register an event handler with a Dispatcher
   Service (instead of a particular virtual machine) using the
   PrlSrv_RegEventHandler function. If you register the event
   handler with the Dispatcher Service, you will be able to receive events
   from all of the virtual machines registered with that Dispatcher Service.
   For more information on events and asynchronous functions,
   see the Virtualization SDK Programmer's Guide.
   Parameters
   hObj :      A handle of type PHT_VIRTUAL_MACHINE or PHT_DESKTOP_CONTROL.
   handler :   A pointer to the callback function that will
               handle events.
   userData :  A pointer to a user data that will be passed to
               the callback function.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_RegEventHandler, (
		PRL_HANDLE hObj,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		) );

/* Unregisters an event handler (callback function) that was
   previously registered using the PrlVm_RegEventHandler
   function. Use this function when you no longer want to
   receive event notifications from the virtual machine. The
   values of the handler and userData parameter constitute a
   unique pair which will be used to identify the event handler
   that will be unregistered. Make sure that you supply the same
   values as the ones you used when you registered the handler.
   Parameters
   hObj :      A handle of type PHT_VIRTUAL_MACHINE or PHT_DESKTOP_CONTROL.
   handler :   A pointer to the callback function. This must be
               the same pointer that was passed to the PrlVm_RegEventHandler
               when the event handler was registered with the
               virtual machine.
   userData :  A pointer to the user data. This must also be the
               same pointer that was passed to the
               PrlVm_RegEventHandler when the event handler was
               registered with the virtual machine.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_UnregEventHandler, (
		PRL_HANDLE hObj,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		) );

/**
The PrlVm_GetQuestions function allows to synchronously receive questions from
a Dispatcher Service. It can be used as an alternative to asynchronous question
processing through events. You can use this function at any time to check
if a question is awaiting the client response on the Dispatcher Service side.

Parameters
handle : A handle of type PHT_VIRTUAL_MACHINE.
pQuestionsList :	[out] A handle of type PHT_HANDLES_LIST containing a list of
					available questions.

Returns
PRL_RESULT. Possible values:
PRL_ERR_INVALID_ARG -	non valid PHT_VIRTUAL_MACHINE handle or non valid (NULL) pointer
						to the buffer for storing result was passed.
PRL_ERR_SUCCESS - operation completed successfully.
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_GetQuestions, (
		PRL_HANDLE hVm,
		PRL_HANDLE_PTR pQuestionsList
		) );


/* Creates VM event that bint to current VM.
   Parameters
   handle :         A handle of type PHT_VIRTUAL_MACHINE
                    identifying the virtual machine.
   unnamed_param :  buffer for storing result
   Returns
   PRL_RESULT. Possible values: PRL_ERR_INVALID_ARG - non valid
   PHT_VIRTUAL_MACHINE handle or non valid (null) pointer to
   buffer was specified PRL_ERR_OUT_OF_MEMORY - not enough
   memory to instantiate VM event object PRL_ERR_SUCCESS -
   operation completed successfully                             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_CreateEvent, (
		PRL_HANDLE hVm,
		PRL_HANDLE_PTR phEvent
		) );

/* %VM_ONLY%

   Creates a floppy disk image in the virtual machine home
   directory for unattended Windows installation. The following
   steps describe how to prepare a virtual machine for
   unattended Windows installation:
     1. Create a virtual machine. The machine should have all
        the necessary devices, like hard drive and CD drive.
     2. Execute the PrlVm_CreateUnattendedFloppy function to
        create a floppy disk image.
     3. Insert Windows installation CD-ROM into the drive (or
        mount a disk image).
     4. Start the virtual machine. The machine will boot from
        the CD.
     5. Perform unattended Windows installation as usual.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :               A handle of type PHT_VIRTUAL_MACHINE
                       identifying the virtual machine.
   nGuestDistroType :  Windows version.
   sUsername :         User name.
   sCompanyName :      Company name.
   sSerialKey :        Serial number.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                 */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_CreateUnattendedFloppy, (
		PRL_HANDLE hVm,
		PRL_GUEST_DISTRO_VERSION_ENUM nGuestDistroType,
		PRL_CONST_STR sUsername,
		PRL_CONST_STR sCompanyName,
		PRL_CONST_STR sSerialKey
		) );


/* %VM_ONLY%

   This method lets to initiate sending notifications about
   devices states by VM. Those notifications have to be same as
   notifications after changing a device state
   Parameters
   handle :  A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   Returns
   PRL_HANDLE to job that describing async operation result
   (PRL_INVALID_HANDLE means not enough memory to instantiate
   async job represents object handle). Operation return code
   can be extracted with @ref PrlJob_GetRetCode() call. Possible
   values: PRL_ERR_INVALID_ARG - non valid PHT_VIRTUAL_MACHINE
   handle was passed PRL_ERR_UNINITIALIZED - unexpected error:
   uninitialized with the Dispatcher Service connection or non identified VM
   object                                                         */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_InitiateDevStateNotifications, (
					  PRL_HANDLE hVm
					  ) );

/* Updates access level for the specified virtual machine. By
   default, only the owner of a virtual machine has full access
   to it. All other users are completely unaware of the machine
   (it will not even appear in the list of the available virtual
   machines for them). The owner of the virtual machine can
   change that by setting one of the predefined access levels
   for the machine. Depending on the level of access set by the
   owner, other users will be able to see the machine and to
   perform tasks on it.

   To update the access level, obtain a handle of type
   PHT_ACCESS_RIGHTS, set the desired level using the
   PrlAcl_SetAccessForOthers function, and then call this
   function to apply the changes to the virtual machine. The
   caller must be the owner of the machine to perform this
   operation. The PrlAcl_IsCurrentSessionOwner function can be
   used to determine if the current user is the owner of the
   virtual machine.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_ACCESS_DENIED - current user is not the owner of the
   target virtual machine.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :            A handle of type PHT_VIRTUAL_MACHINE
                    identifying the virtual machine.
   hAccessRights :  A handle of type PHT_ACCESS_RIGHTS containing
                    the virtual machine access level information.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_UpdateSecurity, (
					  PRL_HANDLE hVm,
					  PRL_HANDLE hAccessRights
					  ) );

/* Validates the specified section of a virtual machine
   configuration. When creating or modifying a virtual machine
   configuration, this function can be used to verify that a
   particular section of the configuration is valid. Use this
   function before sending the virtual machine object containing
   the configuration information to the Dispatcher Service. If the
   configuration is valid, you can commit the changes. If the
   configuration is not valid, make the necessary modifications
   and retry.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or the specified
   configuration section doesn't exist.

   PRL_ERR_PARSE_VM_CONFIG - error parsing configuration data.

   PRL_ERR_VMCONF_VALIDATION_FAILED - invalid configuration; the
   \description of the problem can be retrieved from from the
   PHT_JOB object (see below).

   PRL_ERR_SUCCESS - specified configuration is valid.

   In the event of validation failure, the PHT_JOB object will
   contain the description of the problem. To get the
   \description:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamsCount function to obtain the
        \result count.
     3. Use the PrlResult_GetParamByIndex to obtain a handle of
        type PHT_EVENT (if there are multiple results, call the
        function for each one).
     4. Use the PrlEvent_GetErrString function to get the
        validation error description.
   Parameters
   hVm :       A handle of type PHT_VIRTUAL_MACHINE identifying
               the virtual machine.
   nSection :  The section in the virtual machine configuration
               to verify.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_ValidateConfig, (
						PRL_HANDLE hVm,
						PRL_VM_CONFIG_SECTIONS nSection
						) );

/* %VM_ONLY%

   Begin the Tools installation process. The machine must be
   registered with the Dispatcher Service to perform this operation.
   Parameters
   handle :  A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   Returns
   PRL_HANDLE to job that describing async operation result
   (PRL_INVALID_HANDLE means not enough memory to instantiate
   async job represents object handle). Operation return code
   can be extracted with @ref PrlJob_GetRetCode() call. Possible
   values: PRL_ERR_INVALID_ARG - non valid PHT_VIRTUAL_MACHINE
   handle was passed PRL_ERR_UNINITIALIZED - unexpected error:
   uninitialized with Dispatcher Service connection or non identified VM
   object                                                         */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_InstallTools, (
					  PRL_HANDLE hVm
					  ) );


///////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the optimization of virtual hard disk
///////////////////////////////////////////////////////////////////////////////

/**
Start process of optimization of virtual hard disk.
The machine should be registered on the server to do that.

Parameters
hVm    :  valid PRL_HANDLE describing the virtual machine
uMask  :  bit mask of really hard disk devices (little endian order).
          See PRL_HARD_DISK_IFACE_MASK enumeration for all bits.
nFlags :  only bit PACF_NON_INTERACTIVE_MODE is used.
          Others bits are reserved

Returns
PRL_HANDLE to job that describing async operation result (PRL_INVALID_HANDLE means not enough memory to
instantiate async job represents object handle). Operation return code can be extracted with @ref PrlJob_GetRetCode()
call. Possible values:
PRL_ERR_INVALID_ARG - non valid PHT_VIRTUAL_MACHINE handle was passed
PRL_ERR_UNINITIALIZED - unexpected error: uninitialized with server connection or non identified VM object
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_Compact, (
               PRL_HANDLE hVm,
               PRL_UINT32 uMask,
			   PRL_UINT32 nFlags
             ) );

/**
Finishes process of optimization of virtual hard disk.
The machine should be registered on the server to do that.

Parameters
handle : valid PRL_HANDLE describing the virtual machine

Returns
PRL_HANDLE to job that describing async operation result (PRL_INVALID_HANDLE means not enough memory to
instantiate async job represents object handle). Operation return code can be extracted with @ref PrlJob_GetRetCode()
call. Possible values:
PRL_ERR_INVALID_ARG - non valid PHT_VIRTUAL_MACHINE handle was passed
PRL_ERR_UNINITIALIZED - unexpected error: uninitialized with server connection or non identified VM object
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVm_CancelCompact, (
               PRL_HANDLE hVm
               ) );


/* %VM_ONLY%

Start process conversion of the virtual hard disk(s).
The machine should be registered on the server to do that.

Parameters
hVm    :  valid PRL_HANDLE describing the virtual machine
uMask  :  bit mask of really hard disk devices (little endian order).
          See PRL_HARD_DISK_IFACE_MASK enumeration for all bits.
nFlags :  See PRL_CONVERT_VIRTUAL_DISK enumeration for using bits.

Returns
PRL_HANDLE to job that describing async operation result (PRL_INVALID_HANDLE means not enough memory to
instantiate async job represents object handle). Operation return code can be extracted with @ref PrlJob_GetRetCode()
call. Possible values:
PRL_ERR_INVALID_ARG - non valid PHT_VIRTUAL_MACHINE handle was passed
PRL_ERR_UNINITIALIZED - unexpected error: uninitialized with server connection or non identified VM object
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlVm_ConvertDisks, (
               PRL_HANDLE hVm,
               PRL_UINT32 uMask,
			   PRL_UINT32 nFlags
             ) );


/* %VM_ONLY%

Finishes process of conversion of the virtual hard disk(s).
The machine should be registered on the server to do that.

Parameters
hVm : valid PRL_HANDLE describing the virtual machine
nFlags :  Reserved.

Returns
PRL_HANDLE to job that describing async operation result (PRL_INVALID_HANDLE means not enough memory to
instantiate async job represents object handle). Operation return code can be extracted with @ref PrlJob_GetRetCode()
call. Possible values:
PRL_ERR_INVALID_ARG - non valid PHT_VIRTUAL_MACHINE handle was passed
PRL_ERR_UNINITIALIZED - unexpected error: uninitialized with server connection or non identified VM object
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlVm_CancelConvertDisks, (
               PRL_HANDLE hVm,
			   PRL_UINT32 nFlags
               ) );


/* %VM_ONLY%

   Generates and assigns a new Windows SID to the specified virtual machine running Windows guest OS.
   This function is useful when cloning or migrating a virtual machine to another host to avoid conflicts.
   Please note that this functionality is applicable to Windows guests only.
   Tools must be installed in the virtual machine.
   The machine must be stopped before calling this function.

   Parameters
      hVm : A handle of type PHT_VIRTUAL_MACHINE identifying the virtual machine.
      nFlags : Reserved for future use.

   Returns
   A handle of type PHT_JOB containing the results of this asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object. To get the return code from the PHT_JOB object, use the PrlJob_GetRetCode
   function. Possible values are:
      PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.
      PRL_ERR_SUCCESS - function completed successfully.
      PRL_ERR_UNINITIALIZED – unknown PHT_VIRTUAL_MACHINE handle or invalid binding to server connection.
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlVm_ChangeSid, (
               PRL_HANDLE hVm,
			   PRL_UINT32 nFlags
               ) );


/**
   Resets the virtual machine uptime counter (start date and counter itself).

   Parameters
      hVm : A handle of type PHT_VIRTUAL_MACHINE identifying the virtual machine.
      nFlags : Reserved for future use.

   Returns
   A handle of type PHT_JOB containing the results of this asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object. To get the return code from the PHT_JOB object, use the PrlJob_GetRetCode
   function. Possible values are:
      PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.
      PRL_ERR_SUCCESS - function completed successfully.
      PRL_ERR_UNINITIALIZED – unknown PHT_VIRTUAL_MACHINE handle or invalid binding to server connection.
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_ResetUptime, (
               PRL_HANDLE hVm,
			   PRL_UINT32 nFlags
               ) );


///////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the Offline management. This mode
/// lets to redirect packages going on VM assigned IP address to the some offline
/// service that will process these requests.
/// Note that offline management mode just working through Dispatcher Service
///////////////////////////////////////////////////////////////////////////////

/* Performs an attempt to authenticate the user through the
   guest OS security database. To get the return code from the
   PHT_JOB object, use the PrlJob_GetRetCode function. Possible
   values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer (empty
   string) for user name was passed.

   PRL_ERR_AUTHENTICATION_FAILED - user authentication failed.

   PRL_ERR_SUCCESS - user authenticated successfully.
   Parameters
   hVm :            A handle of type PHT_VIRTUAL_MACHINE
                    identifying the virtual machine.
   sUserName :      User login name.
   sUserPassword :  User password (can be an empty string or NULL
                    if the user doesn't have a password).
   nFlags :         A bitset of flags (should be 0 in the current
                    implementation).
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_AuthWithGuestSecurityDb, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sUserName,
		PRL_CONST_STR sUserPassword,
		PRL_UINT32 nFlags
		) );


/* Mount the specified virtual machine Virtual Disks.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   sMntPath: not used
   nFlags: Mount flags of type PRL_MOUNT_VIRTUAL_DISK
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVm_Mount, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sMntPath,
		PRL_UINT32 nFlags
		) );

/* Unmount the specified virtual machine Virtual Disks.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :  A handle of type PHT_VIRTUAL_MACHINE identifying the
          virtual machine.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
*/

PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVm_Umount, (
		PRL_HANDLE hVm,
		PRL_UINT32 nFlags
		) );


/* Move bundle of virtual machine or private area of container.
   The function move all content of bundle to specify location.
   This function will move only stopped or suspended Vm/Ct.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :               A handle of type PHT_VIRTUAL_MACHINE
                       identifying the virtual machine or container.
   sNewHomePath :      Target path for Vm bundle or CT private
   nFlags :            Flags that specify move type. Not used yet.

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVm_Move, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sNewHomePath,
		PRL_UINT32 nFlags
		) );


/* %VM_ONLY%

   Connect to remote desktop.
   Parameters
   hObj :  A handle of type PHT_VIRTUAL_MACHINE or PHT_DESKTOP_CONTROL.
   nFlags :  Flags, must be 0.

   Returns
   A handle of type PHT_JOB containing the results of operation.
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_5,
						   PrlVm_Connect, (
		PRL_HANDLE hObj,
		PRL_UINT32 nFlags
		) );


/* %VM_ONLY%

   Disconnect from remote desktop.
   Parameters
   hObj :  A handle of type PHT_VIRTUAL_MACHINE or PHT_DESKTOP_CONTROL.
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVm_Disconnect, (
		PRL_HANDLE hObj
		) );


/* Allows to change the password of a guest operating system
   user.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :          A handle of type PHT_VIRTUAL_MACHINE
                  identifying the virtual machine.
   sUserName :    Guest operating system user name. Can be null
                  or empty string when setting the administrator
                  password.
   sUserPasswd :  New password. Can be null when setting an empty
                  password.
   nFlags :       Bitset of flags - from enum PRL_SET_USERPASSWD_FLAGS

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.

*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_7,
						   PrlVm_SetUserPasswd, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sUserName,
		PRL_CONST_STR sUserPasswd,
		PRL_UINT32 nFlags
		) );


/* The PrlVm_CommitEncryption function is used in modifying a virtual
   machine disk encryption configuration. The PrlVm_BeginEdit and the
   PrlVm_CommitEncryption functions are used to detect collisions with
   other clients trying to modify the same virtual machine at
   the same time. The PrlVm_BeginEdit call timestamps the
   beginning of the editing operation. It does not lock the
   machine, so other clients can modify the same machine at the
   same time. When you are done making the changes, you must
   call the PrlVm_CommitEncryption function to save them in the virtual
   machine configuration. The function will verify that the
   configuration has not been modified by another client. If a
   collision is detected, your changes will be rejected.

   Note that changes in the virtual machine encryption settings can only be
   committed using this function. Committing encryption changes along
   with other changes in the virtual machine configuration using
   the PrlVm_Commit or PrlVm_CommitEx functions will result in error.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :          A handle of type PHT_VIRTUAL_MACHINE identifying the
                  virtual machine.
   nFlags :       A bitset of flags from the PRL_COMMIT_ENCRYPTION_FLAGS enumeration
   hPolicyList :  Reserved for future use, its value is ignored.
   hReserved :    Reserved for future use, its value is ignored.

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                 */
PRL_ASYNC_SRV_METHOD_DECL(VIRTUOZZO_API_VER_7,
	PrlVm_CommitEncryption, (
		PRL_HANDLE hVm,
		PRL_UINT32 nFlags,
		PRL_HANDLE hPolicyList,
		PRL_HANDLE hReserved
	));

///////////////////////////////////////////////////////////////////////////////
/// @section Virtual machine guest OS session operations set
///////////////////////////////////////////////////////////////////////////////

/* Closes a session (or unbinds from a pre-existing session) in
   a virtual machine that was previously opened with the
   PrlVm_LoginInGuest function. To get the return code from the
   PHT_JOB object, use the PrlJob_GetRetCode function. Possible
   values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVmGuest :   A handle of type PHT_VM_GUEST_SESSION identifying
                the session.
   nReserved :  Reserved parameter for future purposes. Must
                contain 0 in the current implementation.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlVm_LoginInGuest
                                                                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVmGuest_Logout, (
		PRL_HANDLE hVmGuest,
		PRL_UINT32 nReserved
		) );

/* Executes a program in a virtual machine. Prior to calling
   this function, a handle to a user session in a virtual
   machine must be obtained using the PrlVm_LoginInGuest
   function. Once you obtain a session handle, you can use this
   function to run programs in the virtual machine. Depending on
   the options that were used during login, you will be able to
   run GUI applications and/or console programs. See the
   PrlVm_LoginInGuest function for more info.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_VM_EXEC_PROGRAM_NOT_FOUND - the specified program at
   the specified path was not found.

   PRL_ERR_UNINITIALIZED - file descriptors interface tried to use but
   PrlDevDisplay_ConnectToVm wasn't call.

   PRL_ERR_SUCCESS - program executed successfully.
   Parameters
   hVmGuest :   A handle of type PHT_VM_GUEST_SESSION identifying
                the user session in a virtual machine. The handle
                is obtained using the PrlVm_LoginInGuest
                function.
   sAppName :   An absolute path to the program that you would
                like to execute.
   hArgsList :  A handle of type PHT_STRINGS_LIST specifying the
                list of arguments to pass to the program. Each
                token must be a separate list item.
   hEnvsList :  A handle of type PHT_STRINGS_LIST specifying the
                list of environment variables to add to the
                program execution environment. Each variable must
                be a separate list item in the var_name=var_value
                format.
   nFlags :     Modificators of the program execution. Acceptable
                values\:<p />PFD_STDOUT \- return stdout.<p />PFD_STDERR
                \- return stderr.<p />PFD_ALL \- return both
                stdout and stderr (a blank nFlags parameter
                yields the same result).<p />PRPM_RUN_PROGRAM_AND_RETURN_IMMEDIATELY
                \- run the program and return immediately without
                waiting for the program execution results.
   nStdin :     file descriptor on standard input (PRL_INVALID_FILE_DESCRIPTOR
                should be passed if you don't want to use file descriptors.
				Please note that IO channel between client and VM should be
				established before using file descriptors. Please call
				PrlDevDisplay_ConnectToVm in order to do it).
   nStdout :    file descriptor on standard output (PRL_INVALID_FILE_DESCRIPTOR
                should be passed if you don't want to use file descriptors.
				Please note that IO channel between client and VM should be
				established before using file descriptors. Please call
				PrlDevDisplay_ConnectToVm in order to do it).
   nStderr :    file descriptor on standard errors output (PRL_INVALID_FILE_DESCRIPTOR
                should be passed if you don't want to use file descriptors.
				Please note that IO channel between client and VM should be
				established before using file descriptors. Please call
				PrlDevDisplay_ConnectToVm in order to do it).
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                                      */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVmGuest_RunProgram, (
		PRL_HANDLE hVmGuest,
		PRL_CONST_STR sAppName,
		PRL_HANDLE hArgsList,
		PRL_HANDLE hEnvsList,
		PRL_UINT32 nFlags,
		PRL_FILE_DESC nStdin,
		PRL_FILE_DESC nStdout,
		PRL_FILE_DESC nStderr
		) );

/* Obtains network settings of a guest operating system running
   in a virtual machine. The network settings are obtained using
   a user session in the guest operating system. Prior to
   calling this function, a handle to a user session in the
   virtual machine must be obtained using the PrlVm_LoginInGuest
   function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.
   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the returned PHT_JOB object, use the
   following steps:
     1. Use the PrlJob_GetResult function to obtain a handle of
        type PHT_RESULT.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_SERVER_CONFIG. The handle will contain the guest
        OS network settings.
   Once you obtain the PHT_SERVER_CONFIG handle, you can use the
   following functions to obtain individual network settings of
   the guest OS:

   PrlSrvCfg_GetDnsServers

   PrlSrvCfg_GetHostname

   PrlSrvCfg_GetSearchDomains

   PrlSrvCfg_GetNetAdapter

   PrlSrvCfg_GetDefaultGateway
   Parameters
   hVmGuest :   A handle of type PHT_VM_GUEST_SESSION identifying
                the user session in a virtual machine. The handle
                is obtained using the PrlVm_LoginInGuest
                function.
   nReserved :  Reserved parameter for future purposes. Must be 0
                in the current implementation.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                      */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVmGuest_GetNetworkSettings, (
		PRL_HANDLE hVmGuest,
		PRL_UINT32 nReserved
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Virtual machine configuration operations set
///////////////////////////////////////////////////////////////////////////////

/* The PrlVmCfg_SetDefaultConfig function selects a default
   configuration for a new virtual machine based on the
   operating system that the virtual machine will be running.
   The default configuration information is obtained from the
   host machine configuration, which is passed to the function as
   a parameter. Using a default configuration for a new virtual
   machine is the simplest and safest way to configure a new
   virtual machine. For the example on how to create a new
   virtual machine using a default configuration, see the
   PrlVm_Reg function.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   hSrvConfig :         A handle of type PHT_SERVER_CONFIG.
   guestOsVersion :     The version of the operating system that
                        the virtual machine will be running. The
                        OS versions can be found in the PrlOses.h
                        \file. The names of the macros are
                        prefixed with PVS_GUEST_VER_ (e.g.
                        PVS_GUEST_VER_MACOS_LEOPARD).
   needCreateDevices :  A flag indicating whether default virtual
                        machine devices should be created or not.
                        Specify PRL_TRUE to create default
                        devices, or PRL_FALSE to skip the device
                        creation.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetDefaultConfig, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hSrvConfig,
		PRL_UINT32 guestOsVersion,
		PRL_BOOL needCreateDevices
		) );

/* The PrlVmCfg_GetConfigValidity function returns an error code.
   The error code indicates the status of the config: config doesn't
   exist, parsing error, config can be restored and others errors.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   pnErrCode : [out] A pointer to a variable that receives the
               error code as PRL_RESULT.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetConfigValidity, (
		PRL_HANDLE hVmCfg,
		PRL_RESULT_PTR pnErrCode
		) );

/* The PrlVmCfg_IsConfigInvalid function is used to check the
 * error code of PrlVmCfg_GetConfigValidity function that
 * indicates the invalid status of the config:
 * config doesn't exist, parsing error, invalid server uuid,
 * invalid VM uuid.
 * Parameters
 * nErrCode : The nErrCode value from PrlVmCfg_GetConfigValidity()
 * pbInvalid  : [out] validity sign.
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
                        PrlVmCfg_IsConfigInvalid, (
                PRL_RESULT nErrCode,
                PRL_BOOL_PTR pbInvalid
                ) );

/* The PrlVmCfg_AddDefaultDevice function automates the task of
   setting devices in a virtual machine. When creating or
   modifying a virtual machine configuration, one must choose if
   a device of a particular type will be a pure virtual device
   or if it will be mapped to a physical device on the host.
   To make a device pure virtual, set the hSrvConfig
   parameter value to PRL_INVALID_HANDLE. To map a virtual
   device to a physical device, obtain an object of type
   PHT_SERVER_CONFIG and pass it to the function using the
   hSrvConfig parameter. If multiple physical devices of the
   specified type exist on the host, the destination physical
   device will be chosen randomly by the function.

   The virtual devices that can be mapped to physical devices
   include floppy disk drives, CD drives, serial and parallel
   ports, and sound devices. The devices that can exist only as
   pure virtual include hard disk drives, network adapters, and
   USB devices. To determine if a device of particular type must
   be added to a virtual machine, use the
   PrlVmCfg_IsDefaultDeviceNeeded function.

   Before calling this function, you must execute the
   PrlVm_BeginEdit function. After the PrlVmCfg_AddDefaultDevice
   function returns, you must call the PrlVm_Commit to save the
   changes in the virtual machine configuration.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG containing
                 the host machine configuration information. If
                 this parameter is set to PRL_INVALID_HANDLE, the
                 device of the specified type will be configured
                 as pure virtual (i.e. it will not be mapped to a
                 physical device on the host).
   deviceType :  The type of the device to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OPERATION_FAILED - unable to set a device of the
   specified type.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_AddDefaultDevice, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hSrvConfig,
		PRL_DEVICE_TYPE deviceType
		) );


/* The PrlVmCfg_AddDefaultDeviceEx function automates the task of
   setting devices in a virtual machine. When creating or
   modifying a virtual machine configuration, one must choose if
   a device of a particular type will be a pure virtual device
   or if it will be mapped to a physical device on the host.
   To make a device pure virtual, set the hSrvConfig
   parameter value to PRL_INVALID_HANDLE. To map a virtual
   device to a physical device, obtain an object of type
   PHT_SERVER_CONFIG and pass it to the function using the
   hSrvConfig parameter. If multiple physical devices of the
   specified type exist on the host, the destination physical
   device will be chosen randomly by the function.

   The virtual devices that can be mapped to physical devices
   include floppy disk drives, CD drives, serial and parallel
   ports, and sound devices. The devices that can exist only as
   pure virtual include hard disk drives, network adapters, and
   USB devices. To determine if a device of particular type must
   be added to a virtual machine, use the
   PrlVmCfg_IsDefaultDeviceNeeded function.

   Before calling this function, you must execute the
   PrlVm_BeginEdit function. After the PrlVmCfg_AddDefaultDeviceEx
   function returns, you must call the PrlVm_Commit to save the
   changes in the virtual machine configuration.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG containing
                 the host machine configuration information. If
                 this parameter is set to PRL_INVALID_HANDLE, the
                 device of the specified type will be configured
                 as pure virtual (i.e. it will not be mapped to a
                 physical device on the host).
   deviceType :  The type of the device to set.
   phVmDevice :  [out] A pointer to a variable that receives the
                 new handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OPERATION_FAILED - unable to set a device of the
   specified type.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_AddDefaultDeviceEx, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hSrvConfig,
		PRL_DEVICE_TYPE deviceType,
		PRL_HANDLE_PTR phVmDevice
		) );


/* Determines whether the virtual machine requires a default
   device of the specified type if it's going to run an
   operating system of the specified type. The function accepts
   the target operating system type, version, and a device type
   identifier. Using these parameters, it determines if the
   specified device is required in order for the projected
   virtual machine to run properly. If the device is required,
   you must add it to the virtual machine using the
   PrlVmCfg_AddDefaultDevice function. If the device is not
   required, you don't have to add it (i.e. it will not affect
   the operation of a virtual machine).
   Parameters
   guestOsVersion :  Target OS version. The OS versions are
                     defined in the PrlOses.h file as macros.
                     Look for the macros that are prefixed with
                     PVS_GUEST_VER_ (e.g.
                     PVS_GUEST_VER_MACOS_LEOPARD).
   deviceType :      Device type.
   pbNeeded :        [out] A pointer to a variable that receives
                     the result. PRL_TRUE indicates that the
                     device of specified type is required.
                     PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsDefaultDeviceNeeded, (
		PRL_UINT32 guestOsVersion,
		PRL_DEVICE_TYPE deviceType,
		PRL_BOOL_PTR pbNeeded
		) );

/* The PrlVmCfg_GetDefaultMemSize function returns the default RAM
   size for to the specified OS type and version. When creating
   a virtual machine using a default configuration, the default
   memory size is defined for every supported operating system
   type and version. The default sizes can be retrieved using
   this function. The OS types are defined in the PrlOses.h file
   as macros. Look for the macros that are prefixed with
   PVS_GUEST_TYPE_ (e.g. PVS_GUEST_TYPE_MACOS). The OS versions
   can be found in the same file. The names of the macros are
   prefixed with PVS_GUEST_VER_ (e.g.
   PVS_GUEST_VER_MACOS_LEOPARD).
   Parameters
   guestOsVersion :  Operating system version. See PrlOses.h for
                     type definitions.
   hostRam :         Optional. The amount of memory (RAM)
                     available on the host system. For some
                     operating system, this parameter may be used
                     to calculate the default the RAM size more
                     precisely.
   pnMemSize :       [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetDefaultMemSize, (
		PRL_UINT32 guestOsVersion,
		PRL_UINT32 hostRam,
		PRL_UINT32_PTR pnMemSize
		) );

/* The PrlVmCfg_GetDefaultHddSize function returns the default hard
   disk size for to the specified OS type and version. When
   creating a virtual machine using a default configuration, the
   default hard disk is defined for every supported operating
   system type and version. The default HDD sizes can be
   retrieved using this function. The OS types are defined in
   the PrlOses.h file as macros. Look for the macros that are
   prefixed with PVS_GUEST_TYPE_ (e.g. PVS_GUEST_TYPE_MACOS).
   The OS versions can be found in the same file. The names of
   the macros are prefixed with PVS_GUEST_VER_ (e.g.
   PVS_GUEST_VER_MACOS_LEOPARD).
   Parameters
   guestOsVersion :  Operating system version. See PrlOses.h for
                     \version definitions.
   pnHddSize :       [out] A pointer to a variable that receives
                     the hard disk size, in megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetDefaultHddSize, (
		PRL_UINT32 guestOsVersion,
		PRL_UINT32_PTR pnHddSize
) );

/* The PrlVmCfg_GetDefaultVideoRamSize function returns the
   default video RAM size for the specified OS type and version.
   When creating a virtual machine using a default
   configuration, the default video memory size is defined for
   every supported operating system type and version. The
   default sizes can be retrieved using this function. The OS
   types are defined in the PrlOses.h file as macros. Look for
   the macros that are prefixed with PVS_GUEST_TYPE_ (e.g.
   PVS_GUEST_TYPE_MACOS). The OS versions can be found in the
   same file. The names of the macros are prefixed with
   PVS_GUEST_VER_ (e.g. PVS_GUEST_VER_MACOS_LEOPARD).
   Parameters
   guestOsVersion :       Operating system version. See PrlOses.h
                          for type definitions.
   hSrvConfig :           Optional. The host hardware info object
                          handle (PHT_SERVER_CONFIG type)
                          contains necessary information about
                          host hardware configuration (amount of
                          RAM, host type and etc.). For some
                          operating system, this parameter may be
                          used to calculate the default video RAM
                          size more precisely.
   bIs3DSupportEnabled :  sign which specifies whether 3D support
                          will be enabled at creating VM. 3D
                          support require at least 128 Mb of
                          video RAM in common case.
   pnVideoRamSize :       [out] A pointer to a variable that
                          receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetDefaultVideoRamSize, (
		PRL_UINT32 guestOsVersion,
		PRL_HANDLE hSrvConfig,
		PRL_BOOL bIs3DSupportEnabled,
		PRL_UINT32_PTR pnVideoRamSize
		) );


/* Creates a new virtual device handle of the specified type.
   The names of the virtual device handles are prefixed with
   PHT_VIRTUAL_DEV_ (you may search this documentation for their
   definitions or see the _PRL_HANDLE_TYPE enumeration for a
   complete list of handles). It is the responsibility of the
   user to free the handle when it is no longer needed.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   nDeviceType :  The type of the device to create a handle for.
   phVmDevice :   [out] A pointer to a variable that receives the
                  new handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_CreateVmDev, (
		PRL_HANDLE hVmCfg,
		PRL_DEVICE_TYPE nDeviceType,
		PRL_HANDLE_PTR phVmDevice
		) );

/* Determines the total number of devices of any type installed
   in the virtual machine.
   Parameters
   hVmCfg :               A handle of type PHT_VM_CONFIGURATION
                          identifying the virtual machine configuration.
   vmdev_handles_count :  [out] A pointer to a variable that
                          receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetDevsList                                           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetDevsCount, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR vmdev_handles_count
		) );

/* Obtains a handles list of all virtual devices in a virtual machine.

   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   phDevsList :    [out] A pointer to a variable that receives
                   the handle. The type of the handles list is PHT_HANDLES_LIST.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound

   PRL_ERR_SUCCESS - function completed successfully.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a handle. */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetAllDevices, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phDevsList
		) );

/* Obtains the number of virtual devices of the specified type
   available in the specified virtual machine.
   Parameters
   hVmCfg :          A handle of type PHT_VM_CONFIGURATION
                     identifying the virtual machine configuration.
   vmDeviceType :    Device type.
   pnDevicesCount :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetDevByType                                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetDevsCountByType, (
		PRL_HANDLE hVmCfg,
		PRL_DEVICE_TYPE vmDeviceType,
		PRL_UINT32_PTR pnDevicesCount
		) );

/* Obtains a handle to a virtual device in a virtual machine
   according to the specified device type and index.
   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   vmDeviceType :  The device type.
   nIndex :        An index identifying the device instance
                   (beginning with 0). To determine the number of
                   the devices of the specified type available in
                   the virtual machine, use the
                   PrlVmCfg_GetDevsCountByType function.
   phDevice :      [out] A pointer to a variable that receives
                   the handle. The type of the handle can be
                   determined using the PrlHandle_GetType
                   function. You should also "know" the handle
                   type based on the device type for which you
                   are obtaining a handle. Search this
                   documentation for topics with PHT_VIRTUAL_DEV_
                   prefix in their names for the list of the
                   available virtual device handles.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetDevsCountByType                                       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetDevByType, (
		PRL_HANDLE hVmCfg,
		PRL_DEVICE_TYPE vmDeviceType,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phDevice
		) );

/* %VM_ONLY%

   Determines the number of floppy disk drives in a virtual
   machine.
   Parameters
   hVmCfg :              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine configuration.
   pnFloppyDisksCount :  [out] A pointer to a variable that
                         receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetFloppyDisksCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnFloppyDisksCount
				 ) );


/* %VM_ONLY%

   Obtains a handle of type PHT_VIRTUAL_DEV_FLOPPY containing
   information about the specified floppy disk drive in a
   virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   nIndex :    An index identifying the floppy disk drive
               instance (beginning with 0). To get the number of
               floppy disk drives in the virtual machine, use the
               PrlVmCfg_GetFloppyDisksCount function.
   phDevice :  [out] A pointer to a variable that receives the
               handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetFloppyDisk, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of virtual hard disks in a virtual
   machine.
   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   pnHardDisksCount :  [out] A pointer to a variable that
                       receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetHardDisksCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnHardDisksCount
				 ) );


/* Obtains a handle of type PHT_VIRTUAL_DEV_HARD_DISK containing
   the specified virtual hard disk information.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   nIndex :    An index identifying the hard disk instance
               (beginning with 0). To get the number of hard
               disks installed in the virtual machine, use the
               PrlVmCfg_GetHardDisksCount function.
   phDevice :  [out] A pointer to a variable that receives the
               handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetHardDisk, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* %VM_ONLY%

   Determines the number of optical disks in the specified
   virtual machine.
   Parameters
   hVmCfg :               A handle of type PHT_VM_CONFIGURATION
                          identifying the virtual machine configuration.
   pnOpticalDisksCount :  [out] A pointer to a variable that
                          receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetOpticalDisksCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnOpticalDisksCount
				 ) );


/* %VM_ONLY%

   Obtains a handle of type PHT_VIRTUAL_DEV_OPTICAL_DISK
   containing information a virtual optical disk in a virtual
   machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   nIndex :    An index identifying the optical disk instance
               (beginning with 0). To determine the number of
               optical disks in a virtual machine, use the
               PrlVmCfg_GetOpticalDisksCount function.
   phDevice :  [out] A pointer to a variable that receives the
               handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetOpticalDisk, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* %VM_ONLY%

   Determines the number of virtual printer ports in the
   specified virtual machine.
   Parameters
   hVmCfg :                A handle of type PHT_VM_CONFIGURATION
                           identifying the virtual machine configuration.
   pnParallelPortsCount :  [out] A pointer to a variable that
                           receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetParallelPortsCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnParallelPortsCount
				 ) );


/* %VM_ONLY%

   Obtains a handle of type PHT_VIRTUAL_DEV_PARALLEL_PORT
   containing information about a virtual printer port in the
   specified virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   nIndex :    An index identifying the printer port instance
               (beginning with 0). To determine the number of
               printer ports in a virtual machine, use the
               PrlVmCfg_GetParallelPortsCount function.
   phDevice :  [out] A pointer to a variable that receives the
               handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetParallelPort, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* %VM_ONLY%

   Determines the number of serial ports in a virtual machine.
   Parameters
   hVmCfg :              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine configuration.
   pnSerialPortsCount :  [out] A pointer to a variable that
                         receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetSerialPortsCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnSerialPortsCount
				 ) );


/* %VM_ONLY%

   Obtains a handle of type PHT_VIRTUAL_DEV_SERIAL_PORT
   containing information about a serial port in a virtual
   machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   nIndex :    An index identifying the serial port instance.
   phDevice :  [out] A pointer to a variable that receives the
               handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetSerialPort, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* %VM_ONLY%

   Determines the number of sound devices in a virtual machine.
   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   pnSoundDevsCount :  [out] A pointer to a variable that
                       receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetSoundDevsCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnSoundDevsCount
				 ) );


/* %VM_ONLY%

   Obtains a handle of type PHT_VIRTUAL_DEV_SOUND containing
   information about a sound device in a virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   nIndex :    An index identifying the sound device (beginning
               with 0). To obtain the number of sound devices in
               a virtual machine, use the PrlVmCfg_GetSoundDevsCount
               function.
   phDevice :  [out] A pointer to a variable that receives the
               \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetSoundDev, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* %VM_ONLY%

   Determines the number of USB devices in a virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pnUsbDevicesCount :  [out] A pointer to a variable that
                        receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetUsbDevicesCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnUsbDevicesCount
				 ) );


/* %VM_ONLY%

   Obtains a handle of type PHT_VIRTUAL_DEV_USB_DEVICE
   containing information about a USB device in a virtual
   machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   nIndex :    An index identifying the USB device (beginning
               with 0). To determine the number of USB devices in
               a virtual machine, use the
               PrlVmCfg_GetUsbDevicesCount function.
   phDevice :  [out] A pointer to a variable that receives the
               handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetUsbDevice, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of network adapters in a virtual
   machine.
   Parameters
   hVmCfg :              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine configuration.
   pnNetAdaptersCount :  specifies a pointer to the count of
                         devices
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetNetAdaptersCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnNetAdaptersCount
				 ) );


/* Obtains a handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
   containing information about a virtual network adapter in a
   virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   nIndex :    An index identifying the network adapter instance
               (beginning with 0). To determine the number of
               network adapters in the machine, use the
               PrlVmCfg_GetNetAdaptersCount function.
   phDevice :  [out] A pointer to a variable that receives the
               handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetNetAdapter, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );


/* %VM_ONLY%

   Determines the number of generic PCI devices in a virtual
   machine.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
                            configuration.
   pnGenericPciDevsCount :  [out] A pointer to a variable that
                            receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetGenericPciDev                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetGenericPciDevsCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnGenericPciDevsCount
				 ) );


/* %VM_ONLY%

   Obtains a handle of type PHT_VIRTUAL_DEV_GENERIC_PCI
   containing information about a generic PCI device in a
   virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   nIndex :    An index identifying the generic PCI device
               (beginning with 0). To obtain the number of
               generic PCI devices in a virtual machine, use the
               PrlVmCfg_GetGenericPciDevsCount function.
   phDevice :  [out] A pointer to a variable that receives the
               \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetGenericPciDevsCount                               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetGenericPciDev, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* %VM_ONLY%

   Determines the number of generic SCSI devices in a virtual
   machine.
   Parameters
   hVmCfg :                  A handle of type
                             PHT_VM_CONFIGURATION identifying the
                             virtual machine configuration.
   pnGenericScsiDevsCount :  [out] A pointer to a variable that
                             receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetGenericScsiDev                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetGenericScsiDevsCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnGenericScsiDevsCount
				 ) );


/* %VM_ONLY%

   Obtains a handle of type PHT_VIRTUAL_DEV_GENERIC_SCSI
   containing information about a generic SCSI device in a
   virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   nIndex :    An index identifying the generic SCSI device
               (beginning with 0). To obtain the number of
               generic SCSI devices in a virtual machine, use the
               PrlVmCfg_GetGenericScsiDevsCount function.
   phDevice :  [out] A pointer to a variable that receives the
               \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetGenericScsiDevsCount                               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetGenericScsiDev, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );


/* %VM_ONLY%

   Determines the number of display devices in a virtual
   machine.
   Parameters
   hVmCfg :                  A handle of type
                             PHT_VM_CONFIGURATION identifying the
                             virtual machine configuration.
   pnGenericScsiDevsCount :  [out] A pointer to a variable that
                             receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetDisplayDev                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetDisplayDevsCount, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32_PTR pnDisplayDevsCount
				 ) );


/* %VM_ONLY%

   Obtains a handle of type PHT_VIRTUAL_DEV_DISPLAY
   containing information about a display device in a
   virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   nIndex :    An index identifying the display device
               (beginning with 0). To obtain the number of
               display devices in a virtual machine, use the
               PrlVmCfg_GetDisplayDevsCount function.
   phDevice :  [out] A pointer to a variable that receives the
               \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetDisplayDevsCount                               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetDisplayDev, (
				 PRL_HANDLE hVmCfg,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );


/* %VM_ONLY%

   Determines whether the SmartGuard feature is enabled or not
   in a virtual machine. SmartGuard is feature that allows to
   make automatic snapshots of a virtual machine at predefined
   time intervals. The feature requires Tools
   installed in a virtual machine.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that SmartGuard is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetSmartGuardEnabled                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsSmartGuardEnabled, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables the SmartGuard feature in a virtual machine.
   SmartGuard is feature that allows to make automatic snapshots
   of a virtual machine at predefined time intervals. The
   feature requires Tools installed in a virtual
   machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Action type. Specify PRL_TRUE to enable
               SmartGuard. Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetSmartGuardInterval

   PrlVmCfg_SetSmartGuardMaxSnapshotsCount

   PrlVmCfg_SetSmartGuardNotifyBeforeCreation                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetSmartGuardEnabled, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* %VM_ONLY%

   Determines whether the user will be notified on automatic
   snapshot creation, a SmartGaurd setting. See
   PrlVmCfg_SetSmartGuardNotifyBeforeCreation for more
   information.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbNotifyBeforeCreation :  [out] A pointer to a variable that
                receives the \result. PRL_TRUE indicates that
				the user will be automatically notified.
				PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetSmartGuardNotifyBeforeCreation                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsSmartGuardNotifyBeforeCreation, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbNotifyBeforeCreation
		) );

/* <unfinished>
%VM_ONLY%

   Enables or disables notification of automatic snapshot
   creation, a SmartGuard feature. SmartGuard is a facility that
   allows to make automatic snapshots of a virtual machine at
   predefined time intervals. By default, the snapshots are
   creating transparently to the user. If needed, Dispatcher
   Service can notify the user that a snapshot is about to be
   created. The user can then choose to proceed with snapshot
   creation or cancel it. This function allows to enabled or
   disable this feature. The notifications are received by the
   client via the PHT_EVENT handle received by a callback
   function. See PHT_EVENT and PrlEvent_CreateAnswerEvent for
   more information and code samples.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bNotifyBeforeCreation :  Action type. Specify PRL_TRUE if you
               want the user to be notified. Specify PRL_FALSE
			   if you want to create snapshots silently.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsSmartGuardNotifyBeforeCreation                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetSmartGuardNotifyBeforeCreation, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bNotifyBeforeCreation
		) );


/* %VM_ONLY%

   Determines the interval at which snapshots are taken by
   SmartGuard. SmartGuard is a facility that allows to make
   automatic snapshots of a virtual machine at predefined time
   intervals. The currently set time interval can be obtained
   using this function.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pnInterval :  [out] A pointer to a variable the receives the
                snapshot creation interval, in seconds.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetSmartGuardInterval                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_GetSmartGuardInterval, (
                PRL_HANDLE hVmCfg,
                PRL_UINT32_PTR pnInterval
		) );

/* <unfinished>
%VM_ONLY%

   Sets the time interval at which snapshots are taken by
   SmartGuard. SmartGuard is feature that allows to make
   automatic snapshots of a virtual machine at predefined time
   intervals. This function allows to set the interval at which
   the automatic snapshots are taken.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   nInterval :  The time interval to set, in seconds.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetSmartGuardInterval                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetSmartGuardInterval, (
               PRL_HANDLE hVmCfg,
               PRL_UINT32 nInterval
		) );


/* %VM_ONLY%

   Determines the maximum snapshot count, a SmartGuard setting.
   SmartGuard is a facility that allows to make automatic
   snapshots of a virtual machine at predefined time intervals.
   The snapshot count setting specifies how many snapshots taken
   by SmartGuard will be kept on the hard drive (maximum is
   100). As soon as the maximum number is exceeded, the oldest
   snapshot is deleted.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_GetSmartGuardMaxSnapshotsCount, (
                PRL_HANDLE hVmCfg,
                PRL_UINT32_PTR pnMaxSnapshotsCount
		) );

/* <unfinished>
%VM_ONLY%

   Sets the maximum snapshot count, a SmartGuard feature.
   SmartGuard is a facility that allows to make automatic
   snapshots of a virtual machine at predefined time intervals.
   The snapshot count setting specifies how many snapshots taken
   by SmartGuard will be kept on the hard drive (maximum is
   100). As soon as the maximum number is exceeded, the oldest
   snapshot is deleted.
   Parameters
   hVmCfg :              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine
                         configuration.
   nMaxSnapshotsCount :  Maximum snapshot count to set. The
                         maximum allowable value is 100.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetSmartGuardMaxSnapshotsCount, (
               PRL_HANDLE hVmCfg,
               PRL_UINT32 nMaxSnapshotsCount
		) );


/* %VM_ONLY%

   Determines whether the user-defined shared folders are
   enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsUserDefinedSharedFoldersEnabled, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables or disables user-defined shared folders.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  The value to set. Specify PRL_TRUE to enable
               shared folder. Specify PRL_FALSE to disable them.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetUserDefinedSharedFoldersEnabled, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );

/* %VM_ONLY%

   Determines whether the SmartMount feature is
   enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
		PrlVmCfg_IsSmartMountEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables or disables the SmartMount feature.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  The value to set. Specify PRL_TRUE to enable
               shared folder. Specify PRL_FALSE to disable them.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
		PrlVmCfg_SetSmartMountEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		) );

/* %VM_ONLY%

   Determines whether the SmartMount removable drives is
   enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
		PrlVmCfg_IsSmartMountRemovableDrivesEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables or disables the SmartMount removable drives.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  The value to set. Specify PRL_TRUE to enable
               shared folder. Specify PRL_FALSE to disable them.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
		PrlVmCfg_SetSmartMountRemovableDrivesEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		) );

/* %VM_ONLY%

   Determines whether the SmartMount CD/DVD drives is
   enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
		PrlVmCfg_IsSmartMountDVDsEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables or disables the SmartMount CD/DVD drives.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  The value to set. Specify PRL_TRUE to enable
               shared folder. Specify PRL_FALSE to disable them.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
		PrlVmCfg_SetSmartMountDVDsEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		) );

/* %VM_ONLY%

   Determines whether the SmartMount network shares is
   enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
		PrlVmCfg_IsSmartMountNetworkSharesEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables or disables the SmartMount network shares.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  The value to set. Specify PRL_TRUE to enable
               shared folder. Specify PRL_FALSE to disable them.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
		PrlVmCfg_SetSmartMountNetworkSharesEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		) );

/* %VM_ONLY%

   Determines whether select boot device should be shown on
   virtual machine startup.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbAllowed :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that select boot device
                will be show. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetAllowSelectBootDevice                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsAllowSelectBootDevice, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbAllowed
		) );


/* %VM_ONLY%

   Switches on/off select boot device dialog on virtual machine startup.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bAllowed :  Action type. Specify PRL_TRUE to enable
               select boot device dialog. Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsAllowSelectBootDevice                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetAllowSelectBootDevice, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bAllowed
		) );


/* %VM_ONLY%

   Determines whether EFI boot is switched on.
   Parameters
   handle :       A handle of type PHT_VIRTUAL_MACHINE
                  identifying the virtual machine.
   pbEfiEnabled : A pointer to a variable that receives the
                  \result. PRL_TRUE indicates that the EFI boot is switched on.
                  PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_IsEfiEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEfiEnabled
		) );

/* %VM_ONLY%

   Sets EFI boot for the virtual machine.
   for the specified virtual machine.
   Parameters
   handle :      A handle of type PHT_VIRTUAL_MACHINE
                 identifying the virtual machine.
   bEfiEnabled : Setting value. Perform PRL_TRUE
                 to switch on EFI boot.
                 Perform PRL_FALSE to switch off last one.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_SetEfiEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEfiEnabled
		) );


/* \Returns an external boot device system name.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   sSysName :           [out] A pointer to a buffer that receives
                        the result (a UTF\-8 encoded,
                        null\-terminated string).
   pnSysNameBufLength : [in] The size of the output buffer (in
                        bytes). Set the buffer pointer to null
                        and this parameter's value to zero to
                        receive the required size. [out] The
                        required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_GetExternalBootDevice, (
		PRL_HANDLE hVmCfg,
		PRL_STR sSysName,
		PRL_UINT32_PTR pnSysNameBufLength
		) );

/* Sets an external boot device system name.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   sNewSysName : The name to set. The value must be a a UTF\-8
                 encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_SetExternalBootDevice, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewSysName
		) );


/* %VM_ONLY%

   Creates a new handle of type PHT_SHARE and adds it to the
   specified virtual machine list of shares. It is the
   responsibility of the caller to free the handle when it is no
   longer needed.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION
                identifying the virtual machine configuration.
   phVmShare :  [out] A pointer to a variable that receives the
                new handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_CreateShare, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phVmShare
		) );

/* %VM_ONLY%

   Determines the number of shared folders in a virtual machine.
   A shared folder is a folder in the host OS which is shared
   with a given virtual machine. To obtain its information,
   first determine the number of the available shared folders
   using the PrlVmCfg_GetSharesCount function. Then obtain
   information about an individual folder using the
   PrlVmCfg_GetShare function.

   Tools must be installed to use this feature. To
   obtain the Tools status, use the PrlVm_GetToolsState
   function.
   Parameters
   hVmCfg :         A handle of type PHT_VM_CONFIGURATION
                    identifying the virtual machine configuration.
   pnSharesCount :  [out] A pointer to a variable that receives
                    the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetSharesCount, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnSharesCount
		) );

/* %VM_ONLY%

   Obtains a handle of type PHT_SHARE containing information
   about a shared folder. A shared folder is a folder in the
   host OS which is shared with a given virtual machine. To
   obtain its information, first determine the number of the
   available shared folders using the PrlVmCfg_GetSharesCount
   function. Then iterate through the list using the iteration
   number as the nShareIndex parameter value.

   Tools must be installed to use this feature. To
   obtain the Tools status, use the PrlVm_GetToolsState
   function.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   nShareIndex :  An index identifying the shared folder
                  (beginning with 0). To determine the number of
                  shared folders in a virtual machine, use the
                  PrlVmCfg_GetSharesCount function.
   phVmShare :    [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetShare, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nShareIndex,
		PRL_HANDLE_PTR phVmShare
		) );


/* %VM_ONLY%

   Determines whether the Shared Profile feature is enabled in a
   virtual machine.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the shred
                profile is enabled. PRL_FALSE indicates
                otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsSharedProfileEnabled                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsSharedProfileEnabled, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables or disables the Shared Profile feature in a virtual
   machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  The value to set. Specify PRL_TRUE to enable the
               Shared Profile. Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsSharedProfileEnabled                               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetSharedProfileEnabled, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );

/* %VM_ONLY%

   Determines whether the "use desktop in shared profile" feature
   is enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsUseDesktop, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables or disables the "undo-desktop" feature in the shared
   profile.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Setting boolean value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetUseDesktop, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* %VM_ONLY%

   Determines whether "use documents in shared profile" feature
   is enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsUseDocuments, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Lets to set use documents in shared profile enabling sign.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION
                identifying the virtual machine configuration.
   bEnabled :   Setting boolean value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetUseDocuments, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );



/* %VM_ONLY%

   Determines whether the "used pictures in shared profile"
   feature is enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsUsePictures, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Lets to set use pictures in shared profile enabling sign.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION
                identifying the virtual machine configuration.
   bEnabled :   Setting boolean value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetUsePictures, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );



/* %VM_ONLY%

   Determines whether the "use music in shared profile" feature
   is enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsUseMusic, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Lets to set use music in shared profile enabling sign.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION
                identifying the virtual machine configuration.
   bEnabled :   Setting boolean value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetUseMusic, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* Determines whether the "used downloads in shared profile"
   feature is enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
                PrlVmCfg_IsUseDownloads, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Lets to set use downloads in shared profile enabling sign.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION
                identifying the virtual machine configuration.
   bEnabled :   Setting boolean value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
               PrlVmCfg_SetUseDownloads, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* %VM_ONLY%

   Determines whether the "used movies in shared profile"
   feature is enabled or not.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
                PrlVmCfg_IsUseMovies, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Lets to set use movies in shared profile enabling sign.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION
                identifying the virtual machine configuration.
   bEnabled :   Setting boolean value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
               PrlVmCfg_SetUseMovies, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* Gets optimize modifiers mode.
   Parameters
   hVm :    A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   pnMode : [out] A pointer to a variable that receives the
			\result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_GetOptimizeModifiersMode, (
		PRL_HANDLE hVm,
		PRL_OPTIMIZE_MODIFIERS_MODE_PTR pnMode
		) );

/* Sets optimize modifiers mode.
   Parameters
   hVm :   A handle of type PHT_VM_CONFIGURATION
           identifying the virtual machine configuration.
   nMode : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetOptimizeModifiersMode, (
		PRL_HANDLE hVm,
		PRL_OPTIMIZE_MODIFIERS_MODE nMode
		) );

/* %VM_ONLY%

   Determines whether the clipboard sharing feature is enabled
   or disabled in a virtual machine. When clipboard sharing is
   enabled, you can copy and paste text and files from the host
   to a virtual machine or vise versa, or from one virtual
   machine to another. This feature requires Tools
   installed in a virtual machine.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  A pointer to a variable that receives the result.
                PRL_TRUE indicates that the feature is enabled.
                PRL_FALSE indicates that it is disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsShareClipboard, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* Enables or disables the clipboard sharing feature. When
   clipboard sharing is enabled, you can copy and paste text and
   files from the host to a virtual machine or vise versa, or
   from one virtual machine to another. This feature requires
   Tools installed in a virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Action type. Set to PRL_TRUE to enabled clipboard
               sharing. Set to PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetShareClipboard, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* <unfinished>
%VM_ONLY%

   Determines whether the time synchronization feature is
   enabled in a virtual machine.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  A pointer to a variable that receives the result.
                PRL_TRUE indicates that the time synchronization
                is enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetTimeSynchronizationEnabled                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsTimeSynchronizationEnabled, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* <unfinished>
%VM_ONLY%

   Enables or disables the time synchronization feature in a
   virtual machine. You can synchronize the time in a virtual
   machine with the time in the host OS. To do so, the time
   synchronization feature must be enabled using the
   PrlVmCfg_SetTimeSynchronizationEnabled function. The time
   will be synchronized at predefined time intervals. To set the
   time interval, use the PrlVmCfg_SetSmartGuardInterval
   function. This feature requires Tools installed in
   a virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Setting boolean value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetTimeSyncSmartModeEnabled                          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetTimeSynchronizationEnabled, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* %VM_ONLY%

   Determines whether the smart time synchronization is enabled
   in a virtual machine. For more information, see PrlVmCfg_SetTimeSyncSmartModeEnabled.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  A pointer to a variable that receive the result.
                PRL_TRUE indicates that the feature is enabled.
                PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsTimeSyncSmartModeEnabled, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables or disables the smart time-synchronization mode in a
   virtual machine. You can automatically synchronize the time
   in a virtual machine with the time in the OS by using the PrlVmCfg_SetTimeSynchronizationEnabled
   function. That functionality will keep the time in the
   virtual machine in a perfect synch with the time in the host
   OS. You can also set different time zones in the host and the
   guest OS and still synchronize the time. To do so, use the PrlVmCfg_SetTimeSyncSmartModeEnabled
   function. This feature requires Tools installed in
   a virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Setting boolean value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetTimeSynchronizationEnabled

   PrlVmCfg_SetTimeSyncInterval                                                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetTimeSyncSmartModeEnabled, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* <unfinished>
%VM_ONLY%

   Obtains the time synchronization interval between the host
   and a guest OS. See PrlVmCfg_SetTimeSynchronizationEnabled
   for more information.
   Parameters
   hVmCfg :              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine
                         configuration.
   pnTimeSyncInterval :  [out] A pointer to a variable that
                         receive the time synchronization value,
                         in seconds.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetTimeSyncInterval                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_GetTimeSyncInterval, (
                PRL_HANDLE hVmCfg,
                PRL_UINT32_PTR pnTimeSyncInterval
		) );

/* <unfinished>
%VM_ONLY%

   Allows to set the time interval at which the time in the
   virtual machine will be synchronized with the time in the
   host OS. Before this function can be used, the time
   synchronization feature must be enabled in the virtual
   machine using the PrlVmCfg_SetTimeSynchronizationEnabled
   function.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine
                        configuration.
   nTimeSyncInterval :  Setting interval in seconds.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetTimeSyncInterval                               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetTimeSyncInterval, (
               PRL_HANDLE hVmCfg,
               PRL_UINT32 nTimeSyncInterval
		) );

/* %VM_ONLY%

   Creates a new handle of type PHT_BOOT_DEVICE and adds it to
   the specified virtual machine boot device list. It is the
   responsibility of the caller to free the handle when it is no
   longer needed.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   phVmBootDev :  [out] A pointer to a variable that receives the
                  new handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_CreateBootDev, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phVmBootDev
		) );

/* %VM_ONLY%

   Determines the number of devices in the virtual machine boot
   device priority list.
   Parameters
   hVmCfg :          A handle of type PHT_VM_CONFIGURATION
                     identifying the virtual machine configuration.
   pnBootDevCount :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetBootDevCount, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnBootDevCount
		) );

/* %VM_ONLY%

   Obtains a handle of type PHT_BOOT_DEVICE containing
   information about a specified device from the virtual machine
   boot device priority list. It is the responsibility of the
   called to free the handle when it is no longer needed.
   Parameters
   hVmCfg :         A handle of type PHT_VM_CONFIGURATION
                    identifying the virtual machine configuration.
   nBootDevIndex :  An index identifying the device (beginning
                    with 0). To determine the number of the
                    devices in the boot device list, use the
                    PrlVmCfg_GetBootDevCount function.
   phVmBootDev :    [out] A pointer to a variable that receives
                    the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetBootDev, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nBootDevIndex,
		PRL_HANDLE_PTR phVmBootDev
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Virtual machine configuration
///////////////////////////////////////////////////////////////////////////////

/* \Returns the name of the specified virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   sVmName :            [out] A pointer to a buffer that receives
                        the result (a UTF\-8 encoded,
                        null\-terminated string).
   pnVmNameBufLength :  [in] The size of the output buffer (in
                        bytes). Set the buffer pointer to null
                        and this parameter's value to zero to
                        receive the required size. [out] The
                        required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetName, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmName,
		PRL_UINT32_PTR pnVmNameBufLength
		) );

/* Sets the specified virtual machine name.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   sNewVmName :  The name to set. The value must be a a UTF\-8
                 encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetName, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmName
		) );

/* Obtains the hostname of the specified virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine
                        configuration.
   sVmName :            [out] A pointer to a buffer that receives
                        the result (a UTF\-8 encoded,
                        null\-terminated string).
   pnVmNameBufLength :  [in] The size of the output buffer (in
                        bytes). Set the buffer pointer to null
                        and this parameter's value to zero to
                        receive the required size. [out] The
                        required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The pnVmNameBufLength parameter will contain
   the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetHostname, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmHostname,
		PRL_UINT32_PTR pnVmHostnameBufLength
		) );

/* Sets the specified virtual machine hostname.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   sNewVmName :  The hostname to set. The value must be a a UTF\-8
                 encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetHostname, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmHostname
		) );

/* \Returns the UUID (universally unique ID) of the specified
   virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   sVmUuid :            [out] A pointer to a buffer that receives
                        the result (a UTF\-8 encoded,
                        null\-terminated string).
   pnVmUuidBufLength :  [in] The size of the output buffer (in
                        bytes). Set the buffer pointer to null
                        and this parameter's value to zero to
                        receive the required size. [out] The
                        required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetUuid, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmUuid,
		PRL_UINT32_PTR pnVmUuidBufLength
		) );

/* Sets the virtual machine UUID (universally unique ID). A UUID
   is assigned to a virtual machine automatically when it is
   created. Before using this function, make sure that you
   understand what UUID is, and that you have a good reason for
   changing it.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   sNewVmUuid :  The UUID to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetUuid, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmUuid
		) );

/* \Returns the UUID (universally unique ID) of the parent
   virtual machine which was used to create current linked cloned
   virtual machine (if last one is linked clone of some virtual machine -
   this value will be empty otherwise).
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   sVmUuid :            [out] A pointer to a buffer that receives
                        the result (a UTF\-8 encoded,
                        null\-terminated string).
   pnVmUuidBufLength :  [in] The size of the output buffer (in
                        bytes). Set the buffer pointer to null
                        and this parameter's value to zero to
                        receive the required size. [out] The
                        required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_GetLinkedVmUuid, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmUuid,
		PRL_UINT32_PTR pnVmUuidBufLength
		) );

/* \Returns the type of the operating system that the specified
   virtual machine is running. The OS types are defined in the
   PrlOses.h file as macros. Look for the macros that are
   prefixed with PVS_GUEST_TYPE_ (e.g. PVS_GUEST_TYPE_MACOS).
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   pnVmOsType :  [out] A pointer to a variable that receives the
                 OS type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - VM - the OS information is not present in
   the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetOsType, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmOsType
		) );

/* \Returns the version of the operating system that the
   specified virtual machine is running. The OS versions are
   defined in the PrlOses.h file as macros. Look for the macros
   that are prefixed with PVS_GUEST_VER_ (e.g.
   PVS_GUEST_VER_MACOS_LEOPARD).
   Parameters
   hVmCfg :         A handle of type PHT_VM_CONFIGURATION
                    identifying the virtual machine configuration.
   pnVmOsVersion :  [out] A pointer to a variable that receives
                    the OS version.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - VM - the OS information is not present in
   the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetOsVersion, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmOsVersion
		) );

/* Sets the virtual machine guest OS version.
   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   nVmOsVersion :  The guest OS version to set. The OS versions
                   are defined in the PrlOses.h file as macros.
                   Look for the macros that are prefixed with
                   PVS_GUEST_VER_ (e.g.
                   PVS_GUEST_VER_MACOS_LEOPARD).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetOsVersion, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmOsVersion
		) );

/* \Returns the virtual machine memory (RAM) size, in megabytes.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   pnVmRamSize :  [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the memory information is not available in
   the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetRamSize, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmRamSize
		) );

/* Sets the virtual machine memory (RAM) size.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   nVmRamSize :  The RAM size to set, in megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetRamSize, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmRamSize
		) );

/* %VM_ONLY%

   Determines whether the memory (RAM) hotplug is available in
   the virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pbVmRamHotplugEnabled :  [out] A pointer to a variable that
                        receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_IsRamHotplugEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmRamHotplugEnabled
		) );

/* %VM_ONLY%

   Sets the virtual machine memory (RAM) hotplug mode (enabled or
   disabled).
   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   bVmRamHotplugEnabled :  The RAM hotplug mode to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_SetRamHotplugEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmRamHotplugEnabled
		) );

/* %VM_ONLY%

   Returns the video memory size of the specified virtual
   machine.
   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   pnVmVideoRamSize :  [out] A pointer to a variable that
                       receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the video memory size information is
   missing in the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetVideoRamSize, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmVideoRamSize
		) );

/* %VM_ONLY%

   Sets the virtual machine video memory size.
   Parameters
   hVmCfg :           A handle of type PHT_VM_CONFIGURATION
                      identifying the virtual machine configuration.
   nVmVideoRamSize :  The new memory size, in megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetVideoRamSize, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmVideoRamSize
		) );

/* Returns the minimum memory limit which is guaranteed for
   the virtual machine, in percent of guest RAM.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pMemGuaranteeSize :  [out] A pointer to a structure that receives
                        the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the memory information is not available in
   the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			PrlVmCfg_GetMemGuaranteeSize, (
		PRL_HANDLE hVmCfg,
		PRL_MEMGUARANTEE_DATA_PTR pMemGuaranteeSize
		) );

/* Sets the minimum memory limit which is guaranteed for
   the virtual machine, in percent of guest RAM.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pMemGuaranteeSize :  The memory size structure to set, value and
                        type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			PrlVmCfg_SetMemGuaranteeSize, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_MEMGUARANTEE_DATA_PTR pMemGuaranteeSize
		) );

/* Determines the number of CPU cores in a single CPU socket of the
   virtual machine.
   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   pnVmCpuCount :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetCpuCount, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmCpuCount
		) );

/* Sets the number of CPU cores in a single CPU socket of the
   specified virtual machine.
   (the CPUs should be present in the machine).
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   nVmCpuCount :  The number to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetCpuCount, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmCpuCount
		) );

/* %VM_ONLY%

   Determines the specified virtual machine CPU mode (32 bit or
   64 bit).
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   pnVmCpuMode :  [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetCpuMode, (
		PRL_HANDLE hVmCfg,
		PRL_CPU_MODE_PTR pnVmCpuMode
		) );

/* %VM_ONLY%

   Sets CPU mode (32 bit or 64 bit) for the specified virtual
   machine.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   nVmCpuMode :  The CPU mode to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetCpuMode, (
		PRL_HANDLE hVmCfg,
		PRL_CPU_MODE nVmCpuMode
		) );

/* %VM_ONLY%

   Determines the virtual machine CPU acceleration level.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pnVmCpuAccelLevel :  [out] A pointer to a variable that
                        receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetCpuAccelLevel, (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACCELERATION_LEVEL_PTR pnVmCpuAccelLevel
		) );

/* %VM_ONLY%

   Sets CPU acceleration level for the specified virtual
   machine.
   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   nVmCpuAccelLevel :  The CPU acceleration level to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetCpuAccelLevel, (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACCELERATION_LEVEL nVmCpuAccelLevel
		) );

/* %VM_ONLY%

   Determines whether the x86 virtualization (such as Vt-x) is
   available in the virtual machine CPU.
   Parameters
   handle :             A handle of type PHT_VIRTUAL_MACHINE
                        identifying the virtual machine.
   pbVmCpuVtxEnabled :  A pointer to a variable that receives the
                        \result. PRL_TRUE indicates that the x86
                        virtualization is available. PRL_FALSE
                        indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_NO_DATA - virtual machine configuration does not have
   necessary data.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsCpuVtxEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmCpuVtxEnabled
		) );

/* %VM_ONLY%

   Determines whether the CPU hotplug is available in the virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pbVmCpuHotplugEnabled :  [out] A pointer to a variable that
                        receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_IsCpuHotplugEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmCpuHotplugEnabled
		) );

/* %VM_ONLY%

   Sets CPU hotplug mode (enabled or disabled) for the specified virtual
   machine.
   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   bVmCpuHotplugEnabled :  The CPU hotplug mode to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_SetCpuHotplugEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmCpuHotplugEnabled
		) );

/* Gets 3D graphical acceleration technology mode
   for the specified virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VIRTUAL_MACHINE
                        identifying the virtual machine.
   pn3DAccelerationMode : A pointer to a variable that receives the
                        \result. See PRL_VIDEO_3D_ACCELERATION enum.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_Get3DAccelerationMode, (
		PRL_HANDLE hVmCfg,
		PRL_VIDEO_3D_ACCELERATION_PTR pn3DAccelerationMode
		) );

/* Sets 3D graphical acceleration technology mode
   for the specified virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VIRTUAL_MACHINE
                        identifying the virtual machine.
   n3DAccelerationMode : Setting value.
                        See PRL_VIDEO_3D_ACCELERATION enum.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_Set3DAccelerationMode, (
		PRL_HANDLE hVmCfg,
		PRL_VIDEO_3D_ACCELERATION n3DAccelerationMode
		) );

/* Determines disable or enable vertical synchronization
   video mode for the specified virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VIRTUAL_MACHINE
               identifying the virtual machine.
   pbEnabled : A pointer to a variable that receives the
               result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_IsVerticalSynchronizationEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		) );

/* Disables or enables vertical synchronization video mode
   for the specified virtual machine.
   Parameters
   hVmCfg :   A handle of type PHT_VIRTUAL_MACHINE
              identifying the virtual machine.
   bEnabled : Setting value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_SetVerticalSynchronizationEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		) );

/* Determines disable or enable high resolution
   video mode for the specified virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VIRTUAL_MACHINE
               identifying the virtual machine.
   pbEnabled : A pointer to a variable that receives the
               result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_IsHighResolutionEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		) );

/* Disables or enables high resolution video mode
   for the specified virtual machine.
   Parameters
   hVmCfg :   A handle of type PHT_VIRTUAL_MACHINE
              identifying the virtual machine.
   bEnabled : Setting value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_SetHighResolutionEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		) );

/* Determines disable or enable adaptive hypervisor.
   Parameters
   hVm :	  A handle of type PHT_VM_CONFIGURATION
			  identifying the virtual machine
			  configuration.
   pbEnable : [out] A pointer to a variable that receives the
			  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_IsAdaptiveHypervisorEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		) );

/* Disables or enables adaptive hypervisor.
   Parameters
   hVm :	 A handle of type PHT_VM_CONFIGURATION
			 identifying the virtual machine
			 configuration.
   bEnable : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetAdaptiveHypervisorEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		) );

/* Determines disable or enable Windows logo.
   Parameters
   hVm :	  A handle of type PHT_VM_CONFIGURATION
			  identifying the virtual machine
			  configuration.
   pbEnable : [out] A pointer to a variable that receives the
			  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_IsSwitchOffWindowsLogoEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		) );

/* Disables or enables Windows logo.
   Parameters
   hVm :	 A handle of type PHT_VM_CONFIGURATION
			 identifying the virtual machine
			 configuration.
   bEnable : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetSwitchOffWindowsLogoEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		) );

/* Determines disable or enable longer battery life.
   Parameters
   hVm :	  A handle of type PHT_VM_CONFIGURATION
			  identifying the virtual machine
			  configuration.
   pbEnable : [out] A pointer to a variable that receives the
			  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_IsLongerBatteryLifeEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		) );

/* Disables or enables longer battery life.
   Parameters
   hVm :	 A handle of type PHT_VM_CONFIGURATION
			 identifying the virtual machine
			 configuration.
   bEnable : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetLongerBatteryLifeEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		) );

/* Determines disable or enable battery status.
   Parameters
   hVm :	  A handle of type PHT_VM_CONFIGURATION
			  identifying the virtual machine
			  configuration.
   pbEnable : [out] A pointer to a variable that receives the
			  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_IsBatteryStatusEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		) );

/* Disables or enables battery status.
   Parameters
   hVm :	 A handle of type PHT_VM_CONFIGURATION
			 identifying the virtual machine
			 configuration.
   bEnable : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetBatteryStatusEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		) );

/* Determines disable or enable nested virtualization.
   Parameters
   hVm :	  A handle of type PHT_VM_CONFIGURATION
			  identifying the virtual machine
			  configuration.
   pbEnable : [out] A pointer to a variable that receives the
			  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_IsNestedVirtualizationEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		) );

/* Disables or enables nested virtualization.
   Parameters
   hVm :	 A handle of type PHT_VM_CONFIGURATION
			 identifying the virtual machine
			 configuration.
   bEnable : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetNestedVirtualizationEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		) );

/* Determines disable or enable PMU virtualization.
   Parameters
   hVm :	  A handle of type PHT_VM_CONFIGURATION
			  identifying the virtual machine
			  configuration.
   pbEnable : [out] A pointer to a variable that receives the
			  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_IsPMUVirtualizationEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		) );

/* Disables or enables PMU virtualization.
   Parameters
   hVm :	 A handle of type PHT_VM_CONFIGURATION
			 identifying the virtual machine
			 configuration.
   bEnable : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetPMUVirtualizationEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		) );

/* Determines disable or enable lock guest on suspend.
   Parameters
   hVm :	  A handle of type PHT_VM_CONFIGURATION
			  identifying the virtual machine
			  configuration.
   pbEnable : [out] A pointer to a variable that receives the
			  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_IsLockGuestOnSuspendEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		) );

/* Disables or enables shared camera lock guest on suspend.
   Parameters
   hVm :	 A handle of type PHT_VM_CONFIGURATION
			 identifying the virtual machine
			 configuration.
   bEnable : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetLockGuestOnSuspendEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		) );

/* Determines disable or enable isolated VM.
   Parameters
   hVm :	  A handle of type PHT_VM_CONFIGURATION
			  identifying the virtual machine
			  configuration.
   pbEnable : [out] A pointer to a variable that receives the
			  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_IsIsolatedVmEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		) );

/* Disables or enables shared camera isolated VM.
   Parameters
   hVm :	 A handle of type PHT_VM_CONFIGURATION
			 identifying the virtual machine
			 configuration.
   bEnable : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetIsolatedVmEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		) );

/* Sets the number of CPU units that will be allocated to a
   virtual machine. Each virtual machine on the same host can be
   allocated a number of CPU units from the range of 8 to
   \500000. The larger the number is, the more CPU time the
   virtual machine will get.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   nVmCpuUnits :  The number of CPU units to allocate.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetCpuUnits                                          */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetCpuUnits, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmCpuUnits
		) );

/* Determines the number of CPU units allocated to a virtual
   machine.
   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   pnVmCpuUnits :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetCpuUnits                                           */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetCpuUnits, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR nVmCpuUnits
		) );

/* Sets the CPU usage limit (in percent) for a virtual machine.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   nVmCpuLimit :  CPU usage limit from the range of 0\-100%.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetCpuLimit                                          */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetCpuLimit, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmCpuLimit
		) );

/* Determines the CPU usage limit of a virtual machine, in
   percent.
   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   pnVmCpuLimit :  [out] A pointer to a variable that receives
                   CPU usage limit, in percent.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetCpuLimit                                           */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetCpuLimit, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR nVmCpuLimit
		) );

/* Sets limit of CPU usage for the virtual machine in Mhz.
   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   cpulimit	       A cpulimit data of PRL_CPULIMIT_DATA type
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_SetCpuLimitEx, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_CPULIMIT_DATA_PTR pVmCpuLimit
		) );

/* Determines the specified virtual machine CPU limit.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   pnVmCpuLimit : [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_GetCpuLimitEx, (
		PRL_HANDLE hVmCfg,
		PRL_CPULIMIT_DATA_PTR pVmCpuLimit
		) );

/* Determines the specified virtual machine CPU limit type.

   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   pnVmGuestCpuLimitType :
                  [out] A pointer to a variable that receives the
                  result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				 PrlVmCfg_GetGuestCpuLimitType, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmGuestCpuLimitType
		) );

/* Sets the specified virtual machine I/O priority.
   Priority range is [PRL_IOPRIO_MIN - PRL_IOPRIO_MAX]. The greater priority is,
   the more time for I/O  activity Vm has. By default each Vm has priority of
   IO_PRIO_DEFAULT.

   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   nVmIoPriority :         io priority 0-7.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetIoPriority, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmIoPriority
		) );

/* Determines the specified virtual machine I/O priority.

   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   pnVmIoPriority : [out] A pointer to a variable that receives the
                  result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetIoPriority, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmIoPriority
		) );

/* Sets limit of I/O usage for the virtual machine in Bytes/s.
   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   pVmIoLimit	       A I/O limit data of PRL_IOLIMIT_DATA type
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_SetIoLimit, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_IOLIMIT_DATA_PTR pVmIoLimit
		) );

/* Determines the specified virtual machine I/O limit.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   pnVmIoLimit :  [out] A pointer to a variable that receives the
                  result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_GetIoLimit, (
		PRL_HANDLE hVmCfg,
		PRL_IOLIMIT_DATA_PTR pVmIoLimit
		) );



/* Sets the specified virtual machine IOPS limit.

   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine configuration.
   nVmIopsLimit        IOPS limit.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
				 PrlVmCfg_SetIopsLimit, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmIopsLimit
		) );

/* Determines the specified virtual machine IOPS limit.

   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   pnVmIopsLimit : [out] A pointer to a variable that receives the
                  result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
				 PrlVmCfg_GetIopsLimit, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmIopsLimit
		) );

/* Sets the CPU affinity for a virtual machine.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   sMask:	CPU affinity mask is represented by an cpu number or
		cpu range separated by the comma. (0,2,3-10)
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetCpuLimit                                          */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_SetCpuMask, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sMask
		) );

/* \Returns the specified virtual machine CPU affinity mask
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   sMask:		[out] A pointer to a buffer that receives
                        the result (a UTF\-8 encoded,
                        null\-terminated string).
   pnVmUuidBufLength :  [in] The size of the output buffer (in
                        bytes). Set the buffer pointer to null
                        and this parameter's value to zero to
                        receive the required size. [out] The
                        required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_GetCpuMask, (
		PRL_HANDLE hVmCfg,
		PRL_STR sMask,
		PRL_UINT32_PTR pnMaskBufLength
		) );

/* Sets the NUMA node mask for a virtual machine.
   Parameters
   hVmCfg :		A handle of type PHT_VM_CONFIGURATION
				identifying the virtual machine configuration.
   sMask :		NUMA node mask represented by a CPU number or
				CPU range separated by comma, e.g., 0,2,3-10.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetCpuLimit                                          */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				 PrlVmCfg_SetNodeMask, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sMask
		) );

/* \Returns the specified virtual machine node mask
   Parameters
   hVmCfg :				A handle of type PHT_VM_CONFIGURATION
						identifying the virtual machine configuration.
   sMaski :				[out] A pointer to a buffer that receives
                        the result (a UTF\-8 encoded,
						null\-terminated string).
   pnVmUuidBufLength :  [in] The size of the output buffer (in
                        bytes). Set the buffer pointer to null
                        and this parameter's value to zero to
                        receive the required size. [out] The
                        required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				 PrlVmCfg_GetNodeMask, (
		PRL_HANDLE hVmCfg,
		PRL_STR sMask,
		PRL_UINT32_PTR pnMaskBufLength
		) );

/* \Returns the UUID of the machine hosting the specified virtual
   machine.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine configuration.
   sServerUuid :            [out] A pointer to a buffer that
                            receives the result (a UTF\-8
                            encoded, null\-terminated string).
   pnServerUuidBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this parameter's value to
                            zero to receive the required size.
                            [out] The required output buffer
                            size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetServerUuid, (
		PRL_HANDLE hVmCfg,
		PRL_STR sServerUuid,
		PRL_UINT32_PTR pnServerUuidBufLength
		) );

/* \Returns the hostname of the machine hosting the specified
   virtual machine.
   Parameters
   hVmCfg :                    A handle of type PHT_VM_CONFIGURATION
                               identifying the virtual machine
							   configuration.
   unnamsServerHosted_param :  [out] A pointer to a buffer that
                               receives the result (a UTF\-8
                               encoded, null\-terminated string).
   pnServerHostBufLength :     [in] The size of the output buffer
                               (in bytes). Set the buffer pointer
                               to null and this parameter's value
                               to zero to receive the required
                               size. [out] The required output
                               buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetServerHost, (
		PRL_HANDLE hVmCfg,
		PRL_STR sServerHost,
		PRL_UINT32_PTR pnServerHostBufLength
		) );

/* \Returns the virtual machine home directory name and path.
   Parameters
   hVmCfg :               A handle of type PHT_VM_CONFIGURATION
                          identifying the virtual machine configuration.
   sHomePath :            [out] A pointer to a buffer that
                          receives the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnHomePathBufLength :  [in] The size of the output buffer (in
                          bytes). Set the buffer pointer to null
                          and this parameter's value to zero to
                          receive the required size. [out] The
                          required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetHomePath, (
		PRL_HANDLE hVmCfg,
		PRL_STR sHomePath,
		PRL_UINT32_PTR pnHomePathBufLength
		) );

/* \Returns the virtual machine files location type.
   Please note that this parameter is runtime and read only.
   Parameters
   hVmCfg :               A handle of type PHT_VM_CONFIGURATION
                          identifying the virtual machine configuration.
   pnVmLocation :         [out] A pointer to a buffer that
                          receives the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetLocation, (
		PRL_HANDLE hVmCfg,
		PRL_VM_LOCATION_PTR pnVmLocation
		) );

/* %VM_ONLY%

   \Returns the name of the icon file used by the specified
   virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   sVmIcon :            [out] A pointer to a buffer that receives
                        the result (a UTF\-8 encoded,
                        null\-terminated string).
   pnVmIconBufLength :  [in] The size of the output buffer (in
                        bytes). Set the buffer pointer to null
                        and this parameter's value to zero to
                        receive the required size. [out] The
                        required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetIcon, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmIcon,
		PRL_UINT32_PTR pnVmIconBufLength
		) );

/* %VM_ONLY%

   Sets the virtual machine icon.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   sNewVmIcon :  The new icon file name.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetIcon, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmIcon
		) );

/* \Returns the specified virtual machine description.
   Parameters
   hVmCfg :                    A handle of type PHT_VM_CONFIGURATION
                               identifying the virtual machine
							   configuration.
   sVmDescription :            [out] A pointer to a buffer that
                               receives the result (a UTF\-8
                               encoded, null\-terminated string).
   pnVmDescriptionBufLength :  [in] The size of the output buffer
                               (in bytes). Set the buffer pointer
                               to null and this parameter's value
                               to zero to receive the required
                               size. [out] The required output
                               buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetDescription, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmDescription,
		PRL_UINT32_PTR pnVmDescriptionBufLength
		) );

/* Sets the virtual machine description.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine
						configuration.
   sNewVmDescription :  The virtual machine description to set.
                        The value must be a UTF\-8 encoded,
                        null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetDescription, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmDescription
		) );

/* Determines if the specified virtual machine object contains a
   virtual machine or a template configuration information. If
   the object contains a template configuration, it can be used
   to create new virtual machines from it. See PrlVm_Clone for
   the information on how to create virtual machines from
   templates.
   Parameters
   hVmCfg :          A handle of type PHT_VM_CONFIGURATION
                     identifying the virtual machine configuration.
   pbVmIsTemplate :  [out] A pointer to a variable that receives
                     the result. PRL_TRUE indicates that the
                     machine is a template. PBL_FALE indicates
                     otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsTemplate, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmIsTemplate
		) );

/* Allows to modify a regular virtual machine to become a
   template, and vise versa. A template is very similar to a
   regular virtual machine with the major exception being it
   cannot be started. Templates are used to create new virtual
   machines as exact copies of them. To create a virtual machine
   from a template, use the PrlVm_Clone function specifying the
   template as the source.
   Parameters
   hVmCfg :         A handle of type PHT_VM_CONFIGURATION
                    identifying the virtual machine configuration.
   bVmIsTemplate :  The value to set. Specify PRL_TRUE to make
                    the virtual machine a template. Specify
                    PRL_FALSE to switch a template to a regular
                    virtual machine.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetTemplateSign, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmIsTemplate
		) );

/* %VM_ONLY%

   Returns the virtual machine custom property information. If
   you have some custom data which relates to a virtual machine
   in some way, you can save it in the virtual machine
   configuration for future retrieval. This can be any data that
   your client application requires. The data is stored in CDATA
   format.
   Parameters
   hVmCfg :                       A handle of type PHT_VM_CONFIGURATION
                                  identifying the virtual machine
								  configuration.
   sVmCustomProperty :            [out] A pointer to a buffer
                                  that receives the result (a
                                  UTF\-8 encoded, null\-terminated
                                  string). The custom property
                                  information is stored in CDATA
                                  format.
   pnVmCustomPropertyBufLength :  [in] The size of the output
                                  buffer (in bytes). Set the
                                  buffer pointer to null and this
                                  parameter's value to zero to
                                  receive the required size.
                                  [out] The required output
                                  buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is too
   small. The parameter that is used to specify the size will
   contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetCustomProperty                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetCustomProperty, (
				PRL_HANDLE hVmCfg,
				PRL_STR sVmCustomProperty,
				PRL_UINT32_PTR pnVmCustomPropertyBufLength
				) );

/* %VM_ONLY%

   Sets the virtual machine custom property information. If you
   have some custom data, which relates to a virtual machine in
   some way, you can save it in the virtual machine
   configuration for future retrieval. This can be any data that
   your client application requires. The data is stored in CDATA
   format.
   Parameters
   hVmCfg :                A handle of type PHT_VM_CONFIGURATION
                           identifying the virtual machine
						   configuration.
   sNewVmCustomProperty :  A pointer to a buffer containing the
                           custom information. The data must be
                           in CDATA format.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetCustomProperty, (
				PRL_HANDLE hVmCfg,
				PRL_CONST_STR sNewVmCustomProperty
				) );

/* Determines if the specified virtual machine is set to start
   automatically on Dispatcher Service startup.
   Parameters
   hVmCfg :         A handle of type PHT_VM_CONFIGURATION
                    identifying the virtual machine configuration.
   pnVmAutoStart :  [out] A pointer to a variable that receives
                    the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetAutoStart, (
		PRL_HANDLE hVmCfg,
		PRL_VM_AUTOSTART_OPTION_PTR pnVmAutoStart
		) );

/* Allows to sets the automatic startup option for the specified
   virtual machine.
   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   nVmAutoStart :  The automatic startup option to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetAutoStart, (
		PRL_HANDLE hVmCfg,
		PRL_VM_AUTOSTART_OPTION nVmAutoStart
		) );

/* \Returns the time delay used during the virtual machine
   automatic startup.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION identifying
            the virtual machine configuration.
   pnVmAutoStartDelay :  [out] A pointer to a variable that
                         receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetAutoStartDelay, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmAutoStartDelay
		) );

/* Sets the time delay that will be used during the virtual
   machine automatic startup.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   nVmAutoStartDelay :  Time delay to set, in seconds.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetAutoStartDelay, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmAutoStartDelay
		) );

/* \Returns the automatic startup login mode for the specified
   virtual machine.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   pnVmStartLoginMode :  [out] A pointer to a variable that
                         receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetStartLoginMode, (
		PRL_HANDLE hVmCfg,
		PRL_VM_START_LOGIN_MODE_PTR pnVmStartLoginMode
		) );

/* Allows to set the automatic startup login mode for the
   specified virtual machine.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
         identifying the virtual machine configuration.
   nVmStartLoginMode :  Automatic startup login mode to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetStartLoginMode, (
		PRL_HANDLE hVmCfg,
		PRL_VM_START_LOGIN_MODE nVmStartLoginMode
		) );

/* \Returns the user name used during the specified virtual
   machine automatic startup.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
                                the virtual machine.
   sStartUserLogin :            [out] A pointer to a buffer that
                                receives the result (a UTF\-8
                                encoded, null\-terminated
                                string).
   pnStartUserLoginBufLength :  [in] The size of the output
                                buffer (in bytes). Set the buffer
                                pointer to null and this
                                parameter's value to zero to
                                receive the required size. [out]
                                The required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetStartUserLogin, (
		PRL_HANDLE hVmCfg,
		PRL_STR sStartUserLogin,
		PRL_UINT32_PTR pnStartUserLoginBufLength
		) );

/* Allows to set the automatic startup user login and password
   for the specified virtual machine.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   sStartUserLogin :  User login. Must be a a UTF\-8 encoded,
                      null\-terminated string.
   sPassword :        User password. Must be a a UTF\-8 encoded,
                      null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetStartUserCreds, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sStartUserLogin,
		PRL_CONST_STR sPassword
		) );

/* Determines the mode of the automatic shutdown for the
   specified virtual machine.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   pnVmAutoStop :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetAutoStop, (
		PRL_HANDLE hVmCfg,
		PRL_VM_AUTOSTOP_OPTION_PTR pnVmAutoStop
		) );

/* Allows to set the automatic shutdown mode for the specified
   virtual machine.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   nVmAutoStop :  Automatic shutdown mode to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetAutoStop, (
		PRL_HANDLE hVmCfg,
		PRL_VM_AUTOSTOP_OPTION nVmAutoStop
		) );

/* %VM_ONLY%

   Determines the action on Application window close for the
   specified virtual machine.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   pnActionOnWindowCloase :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetActionOnWindowClose, (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACTION_ON_WINDOW_CLOSE_PTR pnActionOnWindowClose
		) );

/* %VM_ONLY%

   Allows to set the action on Application window close for the specified
   virtual machine.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   nActionOnWindowClose :  Action on window close to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetActionOnWindowClose, (
		PRL_HANDLE hVmCfg,
		PRL_VM_ACTION_ON_WINDOW_CLOSE nActionOnWindowClose
		) );

/* Gets action on stop mode.
   Parameters
   hVm :    A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   pnMode : [out] A pointer to a variable that receives the
			\result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_GetActionOnStopMode, (
		PRL_HANDLE hVm,
		PRL_VM_ACTION_ON_STOP_PTR pnMode
		) );

/* Sets action on stop mode.
   Parameters
   hVm :   A handle of type PHT_VM_CONFIGURATION
           identifying the virtual machine configuration.
   nMode : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetActionOnStopMode, (
		PRL_HANDLE hVm,
		PRL_VM_ACTION_ON_STOP nMode
		) );

/* %VM_ONLY%

   Returns the current window mode the specified virtual
   machine is in.
   Parameters
   hVm :             A handle of type PHT_VIRTUAL_MACHINE
                     identifying the virtual machine.
   pnVmWindowMode :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetWindowMode, (
		PRL_HANDLE hVmCfg,
		PRL_VM_WINDOW_MODE_PTR pnVmWindowMode
		) );

/* %VM_ONLY%

   Sets the virtual machine window mode.
   Parameters
   hVmCfg :         A handle of type PHT_VM_CONFIGURATION
                    identifying the virtual machine configuration.
   nVmWindowMode :  The window mode to set. See the PRL_VM_WINDOW_MODE
                    enumeration for the list of modes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetWindowMode, (
		PRL_HANDLE hVmCfg,
		PRL_VM_WINDOW_MODE nVmWindowMode
		) );

/* Determines disable or enable start in detached window.
   Parameters
   hVm :	  A handle of type PHT_VM_CONFIGURATION
			  identifying the virtual machine
			  configuration.
   pbEnable : [out] A pointer to a variable that receives the
			  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_IsStartInDetachedWindowEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL_PTR pbEnable
		) );

/* Disables or enables start in detached window.
   Parameters
   hVm :	 A handle of type PHT_VM_CONFIGURATION
			 identifying the virtual machine
			 configuration.
   bEnable : [in] Setting value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetStartInDetachedWindowEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bEnable
		) );

/* %VM_ONLY%

   Determines whether the lock in screen mode sign is set or not
   in a virtual machine configuration. Lock in screen mode is feature
   of Player product that allows to mandatory apply full
   screen mode for GUI until guest OS started and go to
   the windowed mode just after guest OS will be stopped.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbValue :    [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that feature is
                enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetLockInFullScreenMode                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsLockInFullScreenMode, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbValue
		) );

/* <unfinished>
%VM_ONLY%

   Enables or disables the lock in screen mode feature
   in a virtual machine configuration. Lock in screen mode is feature
   of Player product that allows to mandatory apply full
   screen mode for GUI until guest OS started and go to
   the windowed mode just after guest OS will be stopped.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bValue :    Action type. Specify PRL_TRUE to enable
               feature. Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsLockInFullScreenMode                                   */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetLockInFullScreenMode, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bValue
		) );


/* \Returns the date and time when the specified virtual machine
   was last modified. Returned time is a local time (i.e. the time value is already converted to local time zone).
   Parameters
   hVmCfg :                         A handle of type PHT_VM_CONFIGURATION
                                    identifying the virtual machine
									configuration.
   sVmLastModifiedDate :            [out] A pointer to a buffer
                                    that receives the result (a
                                    UTF\-8 encoded,
                                    null\-terminated string). The
                                    date is returned using the "yyyy\-mm\-dd
                                    hh\:mi\:ss" format.
   pnVmLastModifiedDateBufLength :  [in] The size of the output
                                    buffer (in bytes). Set the
                                    buffer pointer to null and
                                    this parameter's value to
                                    zero to receive the required
                                    size. [out] The required
                                    \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.                        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetLastModifiedDate, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmLastModifiedDate,
		PRL_UINT32_PTR pnVmLastModifiedDateBufLength
		) );

/* %VM_ONLY%

   Returns the name of the user who last modified the specified
   virtual machine.
   Parameters
   hVmCfg :                         A handle of type PHT_VM_CONFIGURATION
                                    identifying the virtual machine
									configuration.
   sVmLastModifierName :            [out] A pointer to a buffer
                                    that receives the result (a
                                    UTF\-8 encoded,
                                    null\-terminated string).
   pnVmLastModifierNameBufLength :  [in] The size of the output
                                    buffer (in bytes). Set the
                                    buffer pointer to null and
                                    this parameter's value to
                                    zero to receive the required
                                    size. [out] The required
                                    \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetLastModifierName, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmLastModifierName,
		PRL_UINT32_PTR pnVmLastModifierNameBufLength
		) );

/* \Returns the date and time when uptime counter calculation was started for the specified virtual machine.
   Returned time is a local time (i.e. the time value is already converted to local time zone).
   Parameters
   hVmCfg :                         A handle of type PHT_VM_CONFIGURATION
                                    identifying the virtual machine
									configuration.
   sVmUptimeStartDate :             [out] A pointer to a buffer
                                    that receives the result (a
                                    UTF\-8 encoded,
                                    null\-terminated string). The
                                    date is returned using the "yyyy\-mm\-dd
                                    hh\:mi\:ss" format.
   pnVmUptimeStartDateBufLength :   [in] The size of the output
                                    buffer (in bytes). Set the
                                    buffer pointer to null and
                                    this parameter's value to
                                    zero to receive the required
                                    size. [out] The required
                                    \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See also PrlVmCfg_GetUptime                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetUptimeStartDate, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmUptimeStartDate,
		PRL_UINT32_PTR pnVmUptimeStartDateBufLength
		) );

/* \Returns the virtual machine uptime in seconds since some datetime (see PrlVmCfg_GetUptimeStartDate).
   Parameters
   hVmCfg :                         A handle of type PHT_VM_CONFIGURATION
                                    identifying the virtual machine
									configuration.
   pnVmUptime :                     [out] The buffer for storing result.

   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See also PrlVmCfg_GetUptimeStartDate PrlStat_GetOsUptime     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetUptime, (
		PRL_HANDLE hVmCfg,
		PRL_UINT64_PTR pnVmUptime
		) );

/* %VM_ONLY%

   Obtains a handle of type PHT_ACCESS_RIGHTS.
   Parameters
   hVmCfg :   A handle of type PHT_VM_CONFIGURATION
              identifying the virtual machine configuration.
   phVmAcl :  [out] A pointer to a variable that receives the
              handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a handle.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetAccessRights, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phVmAcl
		) );

/* %VM_ONLY%

   Determines if guest sharing is enabled (the guest OS disk
   drives are visible in the host OS).

   Tools must be installed to use this feature. To
   obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                   A handle of type PHT_VM_CONFIGURATION
                              identifying the virtual machine
							  configuration.
   pbVmGuestSharingEnabled :  [out] A pointer to a variable that
                              receives the result. PRL_TRUE
                              indicates that guest sharing is
                              enabled. PRL_FALSE indicates
                              otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsGuestSharingEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmGuestSharingEnabled
		) );

/* %VM_ONLY%

   Enables the guest sharing feature. If guest sharing is
   enabled, the guest OS disk drives will be accessible in the
   host OS. If, in addition, the auto-mount feature is turned
   on, the guest OS disk drives will appear as icons on the host
   desktop. If sharing is enabled but auto-mount is not, you
   will not see the drives in the host OS but Smart Select
   feature will still work.

   Tools must be installed to use this feature. To
   obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                  A handle of type PHT_VM_CONFIGURATION
                             identifying the virtual machine
                             configuration.
   bVmGuestSharingEnabled :  The value to set. Specify PRL_TRUE
                             to enable guest sharing. Specify
                             PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetGuestSharingEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmGuestSharingEnabled
		) );

/* %VM_ONLY%

   Determines if host shared folders are mounted automatically
   in a given virtual machine. If shares are mounted
   automatically, then shared folders will be mapped and appear
   in the guest OS on startup.

   Tools must be installed to use this feature. To
   obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                     A handle of type PHT_VM_CONFIGURATION
                                identifying the virtual machine
								configuration.
   pbVmGuestSharingAutoMount :  [out] A pointer to a variable
                                that receives the result.
                                PRL_TRUE indicates that shares
                                are mounted automatically.
                                PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsGuestSharingAutoMount, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmGuestSharingAutoMount
		) );

/* %VM_ONLY%

   Sets the guest OS sharing auto-mount option. If guest sharing
   is enabled (see PrlVmCfg_SetGuestSharingEnabled) and auto-mount
   is turned on, the guest OS disk drives will appear as icons
   on the host desktop. If sharing is enabled but auto-mount is
   not, you will not see the drives in the host OS but Smart
   Select feature will still work.

   Tools must be installed to use this feature. To
   obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                    A handle of type PHT_VM_CONFIGURATION
                               identifying the virtual machine
							   configuration.
   bVmGuestSharingAutoMount :  Auto\-mount option to set. Specify
                               PRL_TRUE to enable sharing
                               auto\-mount. Specify PRL_FALSE to
                               disable auto\-mount.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetGuestSharingAutoMount, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmGuestSharingAutoMount
		) );

/* %VM_ONLY%

   Determines if the virtual disks in a virtual machine will be
   added to Spotlight search subsystem (Mac OS X feature).
   Tools must be installed to use this feature. To
   obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                           A handle of type
                                      PHT_VM_CONFIGURATION
                                      identifying the virtual
                                      machine configuration.
   pbVmGuestSharingEnableSpotlight :  [out] A pointer to a
                                      variable that receives the
                                      \result. PRL_TRUE indicates
                                      that virtual disks are
                                      added to Spotlight search
                                      subsystem. PRL_FALSE
                                      indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_IsGuestSharingEnableSpotlight, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmGuestSharingEnableSpotlight
		) );

/* %VM_ONLY%

   Sets the VM virtual disks added to Spotlight search subsystem sign.
   If guest sharing is enabled (see PrlVmCfg_SetGuestSharingEnabled) and
   Spotlight enable sign is turned on, the search through Spotlight subsystem
   will be using VM virtual hardisks too.

   Tools must be installed to use this feature. To
   obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                    A handle of type PHT_VM_CONFIGURATION
                               identifying the virtual machine
							   configuration.
   bVmGuestSharingEnableSpotlight :  Enable Spotlight option to set. Specify
                               PRL_TRUE to enable using virtual harddisks
                               at Spotlight search subsystem. Specify PRL_FALSE to
                               disable virtual hard disks from Spotlight.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetGuestSharingEnableSpotlight, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmGuestSharingEnableSpotlight
		) );

/* %VM_ONLY%

   Determines if host sharing is enabled (host shared folders
   are visible in the guest OS). Tools must be
   installed to use this feature. To obtain the Tools
   status, use the PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                  A handle of type
                             PHT_VM_CONFIGURATION identifying the
                             virtual machine configuration.
   pbVmHostSharingEnabled :  [out] A pointer to a variable that
                             receives the result. PRL_TRUE
                             indicates that sharing is enabled.
                             PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsHostSharingEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmHostSharingEnabled
		) );

/* %VM_ONLY%

   Enables host sharing for the specified virtual machine. When
   host sharing is enabled, the host OS shared folders will be
   visible in the guest OS. To use the host sharing feature, the
   shared folders must be set up first using the functionality
   provided by the PHT_SHARE object.

   Tools must be installed to use this feature. To
   obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
							configuration.
   bVmHostSharingEnabled :  The value to set. Specify PRL_TRUE to
                            enable sharing. Specify PRL_FALSE to
                            disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetHostSharingEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmHostSharingEnabled
		) );

/* %VM_ONLY%

   Determines whether all host disks will be present at guest OS
   as shares. Tools must be installed to use this
   feature. To obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :               A handle of type PHT_VM_CONFIGURATION
                          identifying the virtual machine
                          configuration.
   pbShareAllHostDisks :  [out] A pointer to a variable that
                          receives the result. PRL_TRUE indicates
                          that all host disks will be. shared
                          into guest OS. PRL_FALSE indicates
                          otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_IsShareAllHostDisks, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbShareAllHostDisks
		) );

/* %VM_ONLY%

   Enables sharing of all host disks for the specified virtual machine.
   When this sign switched on all host disks will be shared into guest
   OS. Note that host sharing should be switched on to enable this
   functionality.

   Tools must be installed to use this feature. To
   obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
							configuration.
   bShareAllHostDisks    :  The value to set. Specify PRL_TRUE to
                            share all host disks to guest OS.
							Specify PRL_FALSE to not share them.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetShareAllHostDisks, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bShareAllHostDisks
		) );


/* %VM_ONLY%

   Determines whether the host user home directory will be
   available in the guest OS as a share. Tools must be
   installed to use this feature. To obtain the Tools
   status, use the PrlVm_GetToolsState function.
   Parameters
   hVmCfg :              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine
                         configuration.
   pbShareUserHomeDir :  [out] A pointer to a variable that
                         receives the result. PRL_TRUE indicates
                         that host user home directory will be
                         shared with the guest OS. PRL_FALSE
                         indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_IsShareUserHomeDir, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbShareUserHomeDir
		) );

/* %VM_ONLY%

   Enables or disables sharing of the host user home directory
   in the specified virtual machine. When this feature is
   enabled, the host user home directory will be shared in the
   virtual machine. Note that host sharing should be enabled to
   use this functionality. Tools must be installed to
   use this feature. To obtain the Tools status, use
   the PrlVm_GetToolsState function.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine
                        configuration.
   bShareUserHomeDir :  The value to set. Specify PRL_TRUE to
                        share the host user home directory.
                        Specify PRL_FALSE not to share it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetShareUserHomeDir, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bShareUserHomeDir
		) );


/* %VM_ONLY%

   Determines whether host disks shared with the guest Windows
   OS will be mapped to drive letters. Tools must be
   installed to use this feature. To obtain the Tools
   status, use the PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                       A handle of type
                                  PHT_VM_CONFIGURATION
                                  identifying the virtual machine
                                  configuration.
   pbMapSharedFoldersOnLetters :  [out] A pointer to a variable
                                  that receives the result.
                                  PRL_TRUE indicates that host
                                  disks will be mapped to drive
                                  letters. PRL_FALSE indicates
                                  otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_IsMapSharedFoldersOnLetters, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbMapSharedFoldersOnLetters
		) );

/* %VM_ONLY%

   Enables mapping of shared host disks on drive letters for the specified virtual machine.
   When this sign switched on host disks shared into guest Windows OS will be maped on
   correspond drive letters. Note that host sharing should be switched on to enable this
   functionality.

   Tools must be installed to use this feature. To
   obtain the Tools status, use the
   PrlVm_GetToolsState function.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
							configuration.
   bMapSharedFoldersOnLetters    :  The value to set. Specify PRL_TRUE to
                            map shared host disks on drive letters.
							Specify PRL_FALSE to not share it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetMapSharedFoldersOnLetters, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bMapSharedFoldersOnLetters
		) );


/* \Returns the VNC mode of the specified virtual machine.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
							configuration.
   pnVmRemoteDisplayMode :  [out] A pointer to a variable that
                            receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetVNCMode, (
		PRL_HANDLE hVmCfg,
		PRL_VM_REMOTE_DISPLAY_MODE_PTR pnVmRemoteDisplayMode
		) );

/* Sets the virtual machine VNC mode.
   Parameters
   hVmCfg :                A handle of type PHT_VM_CONFIGURATION
                           identifying the virtual machine
						   configuration.
   nVmRemoteDisplayMode :  The VNC mode to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetVNCMode, (
		PRL_HANDLE hVmCfg,
		PRL_VM_REMOTE_DISPLAY_MODE nVmRemoteDisplayMode
		) );

/* \Returns the VNC password for the specified virtual machine.
   Parameters
   hVmCfg :                              A handle of type PHT_VM_CONFIGURATION
                                         identifying the virtual machine
										 configuration.
   sVmRemoteDisplayPassword :            [out] A pointer to a
                                         buffer that receives the
                                         \result (a UTF\-8
                                         encoded, null\-terminated
                                         string).
   pnVmRemoteDisplayPasswordBufLength :  [in] The size of the
                                         \output buffer (in
                                         bytes). Set the buffer
                                         pointer to null and this
                                         parameter's value to
                                         zero to receive the
                                         required size. [out] The
                                         required output buffer
                                         size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetVNCPassword, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmRemoteDisplayPassword,
		PRL_UINT32_PTR pnVmRemoteDisplayPasswordBufLength
		) );

/* Sets the virtual machine VNC password.
   Parameters
   hVmCfg :                       A handle of type PHT_VM_CONFIGURATION
                                  identifying the virtual machine
								  configuration.
   sNewVmRemoteDisplayPassword :  The VNC password to set. The
                                  value must be a UTF\-8 encoded,
                                  null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetVNCPassword, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmRemoteDisplayPassword
		) );

/* \Returns the VNC hostname of the specified virtual machine.
   Parameters
   hVmCfg :                              A handle of type PHT_VM_CONFIGURATION
                                         identifying the virtual machine
										 configuration.
   sVmRemoteDisplayHostName :            [out] A pointer to a
                                         buffer that receives the
                                         \result (a UTF\-8
                                         encoded, null\-terminated
                                         string).
   pnVmRemoteDisplayHostNameBufLength :  [in] The size of the
                                         \output buffer (in
                                         bytes). Set the buffer
                                         pointer to null and this
                                         parameter's value to
                                         zero to receive the
                                         required size. [out] The
                                         required output buffer
                                         size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetVNCHostName, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmRemoteDisplayHostName,
		PRL_UINT32_PTR pnVmRemoteDisplayHostNameBufLength
		) );

/* Sets the virtual machine VNC host name.
   Parameters
   hVmCfg :                       A handle of type PHT_VM_CONFIGURATION
                                  identifying the virtual machine
								  configuration.
   sNewVmRemoteDisplayHostName :  The new VNC host name. The
                                  value must be a a UTF\-8
                                  encoded, null\-terminated
                                  string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetVNCHostName, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmRemoteDisplayHostName
		) );

/* \Returns the VNC port number for the specified virtual
   machine.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
							configuration.
   pnVmRemoteDisplayPort :  [out] A pointer to a variable that
                            receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetVNCPort, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmRemoteDisplayPort
		) );

/* Sets the virtual machine VNC port number.
   Parameters
   hVmCfg :                A handle of type PHT_VM_CONFIGURATION
                           identifying the virtual machine
						   configuration.
   nVmRemoteDisplayPort :  The port number to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetVNCPort, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmRemoteDisplayPort
		) );

/* \Returns the VNC WebSocket port number for the specified virtual
   machine.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
							configuration.
   pnVmRemoteDisplayPort :  [out] A pointer to a variable that
                            receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				 PrlVmCfg_GetVNCWebSocketPort, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmRemoteDisplayWebSocketPort
		) );

/* Determines if VNC is encrypted.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pbVmRemoteDisplayEncryptionEnabled  :  [out] A pointer to a variable that
                        receives the result. PRL_TRUE indicates
                        that the feature is enabled. PRL_FALSE
                        indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				 PrlVmCfg_IsVNCEncryptionEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmRemoteDisplayEncryptionEnabled
		) );

/* %VM_ONLY%

   Determines if additional screen resolution support is enabled
   in a virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pbVmScrResEnabled :  [out] A pointer to a variable that
                        receives the result. PRL_TRUE indicates
                        that the feature is enabled. PRL_FALSE
                        indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsScrResEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmScrResEnabled
		) );

/* %VM_ONLY%

   Enables or disables the additional screen resolution support
   in a virtual machine.
   Parameters
   hVmCfg :            A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine
                       configuration.
   bVmScrResEnabled :  Action type. Specify PRL_TRUE to enable
                       the additional screen resolution support.
                       Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsScrResEnabled                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetScrResEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmScrResEnabled
		) );

/* %VM_ONLY%

   Determines if disk cache write-back is enabled in the
   specified virtual machine.
   Parameters
   hVmCfg :                  A handle of type
                             PHT_VM_CONFIGURATION identifying the
                             virtual machine configuration.
   pbVmDiskCacheWriteBack :  [out] A pointer to a variable that
                             receives the result. PRL_TRUE
                             indicates that cache write\-back is
                             enabled. PRL_FALSE indicates
                             otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetDiskCacheWriteBack                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsDiskCacheWriteBack, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmDiskCacheWriteBack
		) );

/* %VM_ONLY%

   Sets the virtual machine disk cache write-back option.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
							configuration.
   bVmDiskCacheWriteBack :  The disk cache write\-back option to
                            set. Specify PRL_TRUE to enable the
                            disk cache write\-back. Specify
                            PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetDiskCacheWriteBack, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmDiskCacheWriteBack
		) );

/* %VM_ONLY%

   Determines wether the virtual machine OS resolution is in
   full screen mode.
   Parameters
   hVmCfg :                  A handle of type
                             PHT_VM_CONFIGURATION identifying the
                             virtual machine configuration.
   pbVmOsResInFullScrMode :  [out] A pointer to a variable that
                             receives the result. PRL_TRUE
                             indicates full screen mode.
                             PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsOsResInFullScrMode, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmOsResInFullScrMode
		) );

/* %VM_ONLY%

   Sets the virtual machine OS resolution in full screen mode
   option.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
							configuration.
   bVmOsResInFullScrMode :  The value to set. Specify PRL_TRUE to
                            enable the option. Specify PRL_FALSE
                            to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetOsResInFullScrMode, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmOsResInFullScrMode
		) );

/* <unfinished>
%VM_ONLY%

   Determines whether the application (such as
   Desktop) will be automatically closed on the
   virtual machine shutdown.
   Parameters
   handle :                  A handle of type PHT_VIRTUAL_MACHINE
                             identifying the virtual machine.
   pbVmCloseAppOnShutdown :  [out] A pointer to a variable that
                             receives the result. PLR_TRUE
                             indicates that applications will be
                             closed. PRL_FALSE indicates
                             otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - operation successfully completed.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_IsCloseAppOnShutdown, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVmCloseAppOnShutdown
		) );

/* %VM_ONLY%

   Allows to specify whether the application (such
   Desktop) will be closed on the virtual machine
   shutdown.
   Parameters
   hVmCfg :                 A handle of type PHT_VIRTUAL_MACHINE
                            identifying the virtual machine.
   bVmCloseAppOnShutdown :  Action type. Specify PRL_TRUE to
                            close the application. Specify
                            PRL_FALSE to keep it running.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsCloseAppOnShutdown                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetCloseAppOnShutdown, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVmCloseAppOnShutdown
		) );

/* <unfinished>
%VM_ONLY%

   Returns the virtual machine system flags.
   Parameters
   hVmCfg :                    A handle of type
                               PHT_VM_CONFIGURATION identifying
                               the virtual machine configuration.
   sVmSystemFlags :            [out] A pointer to a buffer that
                               receives the result (a UTF\-8
                               encoded, null\-terminated string).
   pnVmSystemFlagsBufLength :  [in] The size of the output buffer
                               (in bytes). Set the buffer pointer
                               to null and this parameter's value
                               to zero to receive the required
                               size. [out] The required output
                               buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetSystemFlags, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmSystemFlags,
		PRL_UINT32_PTR pnVmSystemFlagsBufLength
		) );

/* %VM_ONLY%

   Sets the virtual machine system flags.
   Parameters
   handle :         A handle of type PHT_VIRTUAL_MACHINE
                    identifying the virtual machine.
   unnamed_param :  New virtual machine system flags, a UTF\-8
                    encoded null terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetSystemFlags, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmSystemFlags
		) );

/* %VM_ONLY%

   Determines whether the APIC is enabled or disabled during the
   virtual machine runtime.
   Parameters
   hVmCfg :         A handle of type PHT_VM_CONFIGURATION
                    identifying the virtual machine
                    configuration.
   pbDisableAPIC :  [out] A pointer to a variable that receives
                    the result. PRL_TRUE indicates that the APIC
                    is disabled. PBL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_IsDisableAPIC, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbDisableAPIC
		) );

/* %VM_ONLY%

   Allows to specify whether the virtual machine should be using
   APIC during runtime.
   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   bDisableAPIC :  The value to set. Specify PRL_TRUE to enable
                   the APIC. Specify PRL_FALSE disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsDisableAPIC                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetDisableAPICSign, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bDisableAPIC
		) );

/* %VM_ONLY%

   Determines the current undo-disks mode for the specified
   virtual machine.
   Parameters
   hVmCfg :           A handle of type PHT_VM_CONFIGURATION
                      identifying the virtual machine
                      configuration.
   pnUndoDisksMode :  [out] A pointer to a variable that receives
                      the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetUndoDisksMode, (
		PRL_HANDLE hVmCfg,
		PRL_UNDO_DISKS_MODE_PTR pnUndoDisksMode
		) );


/* %VM_ONLY%

   Sets the undo-disks mode for the virtual machine.
   Parameters
   hVmCfg :          A handle of type PHT_VM_CONFIGURATION
                     identifying the virtual machine
                     configuration.
   nUndoDisksMode :  The undo disks mode option to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetUndoDisksMode, (
		PRL_HANDLE hVmCfg,
		PRL_UNDO_DISKS_MODE nUndoDisksMode
		) );


/* %VM_ONLY%

   Determines whether the speaker is enabled or disabled during the
   virtual machine runtime.
   Parameters
   hVmCfg :           A handle of type PHT_VM_CONFIGURATION
                      identifying the virtual machine
                      configuration.
   pbDisableSpeaker : [out] A pointer to a variable that receives
                      the result. PRL_TRUE indicates that the speaker
                      is disabled. PBL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_IsDisableSpeaker, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbDisableSpeaker
		) );

/* %VM_ONLY%

   Allows to specify whether the virtual machine should be using
   speaker during runtime.
   Parameters
   hVmCfg :          A handle of type PHT_VM_CONFIGURATION
                     identifying the virtual machine configuration.
   bDisableSpeaker : The value to set. Specify PRL_TRUE to enable
                     the speaker. Specify PRL_FALSE disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsDisableSpeaker                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_SetDisableSpeakerSign, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bDisableSpeaker
		) );

/* %VM_ONLY%

   Returns foreground processes priority for the specified
   virtual machine.
   Parameters
   hVmCfg :                  A handle of type PHT_VM_CONFIGURATION
                             identifying the virtual machine configuration.
   pnVmForegroundPriority :  [out] A pointer to a variable that
                             receives the result
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetForegroundPriority, (
		PRL_HANDLE hVmCfg,
		PRL_VM_PRIORITY_PTR pnVmForegroundPriority
		) );

/* %VM_ONLY%

   Sets the virtual machine foreground processes priority.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
							configuration.
   nVmForegroundPriority :  The priority level to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetForegroundPriority, (
		PRL_HANDLE hVmCfg,
		PRL_VM_PRIORITY nVmForegroundPriority
		) );

/* %VM_ONLY%

   Determines the specified virtual machine background process
   priority type.
   Parameters
   hVmCfg :                  A handle of type PHT_VM_CONFIGURATION
                             identifying the virtual machine
							 configuration.
   pnVmBackgroundPriority :  [out] A pointer to a variable that
                             receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetBackgroundPriority, (
		PRL_HANDLE hVmCfg,
		PRL_VM_PRIORITY_PTR pnVmBackgroundPriority
		) );

/* %VM_ONLY%

   Sets the virtual machine background processes priority.
   Parameters
   hVmCfg :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
                            configuration.
   nVmBackgroundPriority :  The priority level to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_SetBackgroundPriority, (
		PRL_HANDLE hVmCfg,
		PRL_VM_PRIORITY nVmBackgroundPriority
		) );

/* %VM_ONLY%

   Determines whether the use default answers mechanism active for
   the virtual machine.
   Parameters
   hVmCfg :               A handle of type PHT_VM_CONFIGURATION identifying
                          the virtual machine configuration.
   pbUseDefaultAnswers :  [out] A pointer to a variable that receives the
                          \result. PRL_TRUE indicates that mehanism is
                          active. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetUseDefaultAnswers                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsUseDefaultAnswers, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbUseDefaultAnswers
		) );

/* %VM_ONLY%

   Enables the use default answers mechanism in a virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Action type. Specify PRL_TRUE to enable
               mechanism. In this case system won't passing questions
			   from virtual machine to client side and answering on
			   them automatically. Specify PRL_FALSE to disable
			   mechanism and processing virtual machine questions
			   explicitly.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsUseDefaultAnswers                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetUseDefaultAnswers, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bUseDefaultAnswers
		) );

/* Obtains the list of globally defined search domains that will
   be assigned to a virtual machine on the guest operating
   system side.
   Parameters
   hVmCfg:               A handle of type PHT_VM_CONFIGURATION
                          identifying the virtual machine
                          configuration.
   phSearchDomainsList :  [out] A pointer to the PHT_STRINGS_LIST
                          handle containing the list of search
                          domains.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate the
   string list object.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetSearchDomains				*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetSearchDomains, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phSearchDomainsList
		) );

/* Allows to manually set the global search domain list that
   will be assigned to the Container.
   \Note: For these changes to take effect, Guest Tools must
   be installed in a target virtual machine.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmCfg:              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine
                         configuration.
   hSearchDomainsList :  A handle of type PHT_STRINGS_LIST
                         containing the list of search domains to
                         be applied to the global network
                         settings of the virtual machine
                         operating system. Can be
                         PRL_INVALID_HANDLE if it's necessary to
                         specify an empty list.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of virtual network
   adapter specified.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetSearchDomains				*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetSearchDomains, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hSearchDomainsList
		) );

/* Obtains the list of of globally defined DNS servers which is assigned to a
   Container.
   Parameters
   hVmCfg:              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine
                         configuration.
   phDnsServersList:  [out] A pointer to the PHT_STRINGS_LIST
                       handle that receives the list of DNS
                       servers.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings
   list object.

   PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_GetDnsServers, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phDnsServersList
		) );

/* Allows to manually assign DNS servers
   a virtual machine. <b>Note</b>: For these changes to take
   effect, Tools must be installed in the target virtual
   machine, and its network adapter settings should be configured
   to change automatically.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmCfg:              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine
                         configuration.
   hDnsServersList:  A handle of type PHT_STRINGS_LIST
                      containing the list of DNS servers to be
                      assigned to the network adapter. Can be
                      PRL_INVALID_HANDLE if it's necessary to
                      specify an empty list.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of virtual network
   adapter specified.

   PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_SetDnsServers, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hDnsServersList
		) );

/* Allows to specify whether a virtual machine should be automatically
   configured for IP addrs.
   In comparison with PrlVmDevNet_SetAutoApply(), dns servers
   and searchdomains settings will be skipped.
   Note: for changes to take effect, Tools must be
   installed in a target virtual machine.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmCfg :              A handle of type
                         PHT_VM_CONFIGURATION identifying
                         the virtual network adapter.
   bAutoApplyIpOnly :    Specify PRL_TRUE to configure the
                         adapter automatically. Specified
                         PRL_FALSE to configure the network
                         settings manually in a virtual machine.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
                             PrlVmCfg_SetAutoApplyIpOnly, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bAutoApplyIpOnly
               ) );

/* Determines if the Guest configured to automatically apply IP addrs network
   settings inside guest.
   Parameters
   hVmCfg :               A handle of type PHT_VM_CONFIGURATION
                          identifying the virtual network
                          adapter.
   pbAutoApplyIpOnly : [out] A pointer to a variable that
                          receives the result. PRL_TRUE indicates
                          that the adapter is configured to automatically
                          apply network settings. PRL_FALSE indicates
                          otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
                                PrlVmCfg_IsAutoApplyIpOnly, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL_PTR pbAutoApplyIpOnly
               ) );

/* <unfinished>

   Determines whether the offline management feature is enabled
   for a virtual machine.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that offline
                management is available. PRL_FALSE indicates
                otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetOfflineManagementEnabled                           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
			PrlVmCfg_IsOfflineManagementEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		) );

/* <unfinished>

   Enables or disables the offline management feature for a
   virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Action type. Specify PRL_TRUE to enable the
               offline management feature. Specify PRL_FALSE to
               disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetOfflineManagementEnabled, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* <unfinished>

   Obtains the list of services available in the virtual machine
   offline management.
   Parameters
   hVmDev :                 A handle of type PHT_VM_CONFIGURATION
                            identifying the virtual machine
                            configuration.
   phOfflineServicesList :  [out] A pointer to the
                            PHT_STRINGS_LIST handle containing
                            the list of offline services.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate the
   string list object.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetOfflineManagementEnabled

   PrlVmCfg_SetOfflineServices                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetOfflineServices, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phOfflineServicesList
		) );

/* <unfinished>

   Allows to manually set offline services that will be
   available in the virtual machine offline management.
   Parameters
   hVmDev :                A handle of type PHT_VM_CONFIGURATION
                           identifying the virtual machine
                           configuration.
   hOfflineServicesList :  A handle of type PHT_STRINGS_LIST
                           containing the list of offline
                           services. Specify PRL_INVALID_HANDLE
                           to assign the default list of
                           services.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of virtual machine
   specified.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetOfflineServices                                   */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetOfflineServices, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hOfflineServicesList
		) );

/* Obtains a handles list of operations with virtual machine which required administrator confirmation

Parameters

   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   phConfirmList : [out] A pointer to a variable that receives
                   the handle. The type of the handle is PHT_OPAQUE_TYPE_LIST.
				   ( values in list consist in enum PRL_ALLOWED_VM_COMMAND )
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound

   PRL_ERR_SUCCESS - function completed successfully.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a handle.
   See Also
      PrlSrv_DisableConfirmationMode()
	  PrlSrv_EnableConfirmationMode()
      PrlSrv_IsConfirmationModeEnabled()
      PrlVmCfg_GetConfirmationsList()
      PrlVmCfg_SetConfirmationsList()
      PrlDispCfg_GetConfirmationsList()
      PrlDispCfg_SetConfirmationsList()
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				PrlVmCfg_GetConfirmationsList, (
				PRL_HANDLE hVmCfg,
				PRL_HANDLE_PTR phConfirmList
				) );

/* Determines a list of operations with virtual machine which required administrator confirmation

Parameters

   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   hConfirmList :  A handle of type is PHT_OPAQUE_TYPE_LIST
                   contains new list of operations( values in enum PRL_ALLOWED_VM_COMMAND )
				   that required administrator's confirmation.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound

   PRL_ERR_SUCCESS - function completed successfully.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a handle.
   See Also
      PrlSrv_DisableConfirmationMode()
	  PrlSrv_EnableConfirmationMode()
      PrlSrv_IsConfirmationModeEnabled()
      PrlVmCfg_GetConfirmationsList()
      PrlVmCfg_SetConfirmationsList()
      PrlDispCfg_GetConfirmationsList()
      PrlDispCfg_SetConfirmationsList()
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				PrlVmCfg_SetConfirmationsList, (
				PRL_HANDLE hVmCfg,
				PRL_HANDLE hConfirmList
				) );


/* Determines whether the Automatic HDD compress feature is enabled
   or not in a virtual machine. Automatic HDD compress is feature
   that allows to make automatic compacting of a virtual machine disks
   at predefined time intervals. The feature recommends Tools
   installed in a virtual machine.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that Automatic HDD compress
				is enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetAutoCompressEnabled                               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_IsAutoCompressEnabled, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
		) );


/* Enables the Automatic HDD compress feature in a virtual machine.
   Automatic HDD compress is feature that allows to make automatic
   compress of a virtual machine disks at predefined time intervals.
   The feature recommends Tools installed in a virtual
   machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Action type. Specify PRL_TRUE to enable
               Automatic HDD compress. Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetAutoCompressdInterval
   PrlVmCfg_SetFreeDiskSpaceRatio                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetAutoCompressEnabled, (
               PRL_HANDLE hVmCfg,
               PRL_BOOL bEnabled
		) );


/* Determines the interval at which compacting virtual disks
   are doing by Automatic HDD compress. Automatic HDD compress
   is a facility that allows to make automatic compress of a virtual
   machine disks at predefined time intervals. The currently set
   time interval can be obtained using this function.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION identifying
                 the virtual machine configuration.
   pnInterval :  [out] A pointer to a variable the receives the
                 automatic compress interval, in seconds.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetAutoCompressInterval                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_GetAutoCompressInterval, (
                PRL_HANDLE hVmCfg,
                PRL_UINT32_PTR pnInterval
		) );


/* Sets the time interval at which compacting virtual disks
   are doing by Automatic HDD compress. Automatic HDD compress
   is feature that allows to make automatic compress of a virtual
   machine disks at predefined time intervals. This function
   allows to set the interval at which the automatic compress
   are taken.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   nInterval :  The time interval to set, in seconds.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetAutoCompressInterval                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetAutoCompressInterval, (
               PRL_HANDLE hVmCfg,
               PRL_UINT32 nInterval
		) );


/* Determines the free disk space ratio at which compacting virtual
   disks are doing by Automatic HDD compress. Automatic HDD compress
   is a facility that allows to make automatic compress of a virtual
   machine disks at predefined space ratio. The currently set
   space ratio can be obtained using this function.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION identifying
                 the virtual machine configuration.
   pdFreeDiskSpaceRatio :  [out] A pointer to a variable the receives
                 the automatic compress space ratio, in percents.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetFreeDiskSpaceRatio                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
                PrlVmCfg_GetFreeDiskSpaceRatio, (
                PRL_HANDLE hVmCfg,
                PRL_DOUBLE_PTR pdFreeDiskSpaceRatio
		) );


/* Sets the time free disk space ratio at which compacting virtual
   disks are doing by Automatic HDD compress. Automatic HDD compress
   is feature that allows to make automatic compress of a virtual
   machine disks at predefined space ratio. This function allows to
   set the space ratio at which the automatic compress are taken.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   dFreeDiskSpaceRatio :  The space ratio to set, in percents.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetFreeDiskSpaceRatio                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
               PrlVmCfg_SetFreeDiskSpaceRatio, (
               PRL_HANDLE hVmCfg,
               PRL_DOUBLE dFreeDiskSpaceRatio
		) );

/* %VM_ONLY%

   Enables or disables the Tools AutoUpdate feature for a
   virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Action type. Specify PRL_TRUE to enable the
               AutoUpdate feature. Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_IsToolsAutoUpdateEnabled, (
		 PRL_HANDLE hVmCfg,
		 PRL_BOOL_PTR pbEnabled
		) );

/*
   Enables or disables the Tools AutoUpdate feature for a
   virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Action type. Specify PRL_TRUE to enable the
               AutoUpdate feature. Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetToolsAutoUpdateEnabled, (
		 PRL_HANDLE hVmCfg,
		 PRL_BOOL bEnabled
		) );

/* Obtains a handle of type PHT_VM_INFO containing the specified
   virtual machine state and access rights information.
   Possible values are:

   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION
               identifying the virtual machine configuration.
   phVmInfo :  [out] A pointer to a variable that receives the
               handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate object

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_GetVmInfo, (
				 PRL_HANDLE hVmCfg,
				 PRL_HANDLE_PTR phVmInfo
				 ) );

/* Determines whether the High Availability feature is enabled
   or not for a virtual machine.
   Parameters
   hVmCfg :     A handle of type PHT_VM_CONFIGURATION identifying
                the virtual machine configuration.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that High Availability
				is enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetHighAvailabilityEnabled                           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_IsHighAvailabilityEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbEnabled
		) );

/*
   Enables or disables the High Availability feature for a virtual machine.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   bEnabled :  Action type. Specify PRL_TRUE to enable the
               High Availability feature. Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_IsHighAvailabilityEnabled                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_SetHighAvailabilityEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bEnabled
		) );

/* Determines the priority of the virtual machine in the High Availability
   Cluster.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION identifying
                 the virtual machine configuration.
   pnPriority :  [out] A pointer to a variable the receives
                 the priority value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to
   buffer was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetHighAvailabilityPriority                          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_GetHighAvailabilityPriority, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnPriority
		) );

/* Sets the priority of the virtual machine in the High Availability
   Cluster.
   Parameters
   hVmCfg :    A handle of type PHT_VM_CONFIGURATION identifying
               the virtual machine configuration.
   nPriority :  The priority value to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetHighAvailabilityPriority                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmCfg_SetHighAvailabilityPriority, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nPriority
		) );

/* Add the specified rates list to the Vm configuration.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVmCfg :		A handle of type PHT_VM_CONFIGURATION identifying
			the virtual machine configuration.
   phNetworkRateList:	A handle of type PHT_HANDLES_LIST contains the
			list of handle sof type PHT_NETWORK_RATE.

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_GetNetworkShapingConfig
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
						PrlVmCfg_SetNetworkRateList, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hNetworkRateList
		) );

/* Obtains the list of shaping rated
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   phNetworkRateList:	[out] A pointer to the PHT_HANDLES_LIST
			handle containing the list of handle sof type
			PHT_NETWORK_RATE.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate the object.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetSearchDomains                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
						PrlVmCfg_GetNetworkRateList, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phNetworkRateList
		) );

/* Sets the ratebound flag.
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   bEnabled:		If this parameter is set to PRL_TRUE then Vm output bandwidth
			specified by rate will be bandwidth limit and guarantee.
			Otherwise rate sets Vm output bandwidth guarantee for traffic and
			limit is set by TOTALRATE in the network shaping Configuratio,.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
						 PrlVmCfg_SetRateBound, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL bEnabled
                ) );

/* Determines whether the ratebound is enabled or not.
   Parameters
   hVmCfg :		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   pbEnabled :		[out] A pointer to a variable that receives the
			\result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
						 PrlVmCfg_IsRateBound, (
                PRL_HANDLE hVmCfg,
                PRL_BOOL_PTR pbEnabled
                ) );


/* Specify the Virtual Machine type on create action.
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   nType		Virtuam Machine type.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
						 PrlVmCfg_SetVmType, (
                PRL_HANDLE hVmCfg,
		PRL_VM_TYPE nType
                ) );

/* Get the Virtual Machine type.
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   pnType :		[out] A pointer to a variable that receives the
			\result.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
						 PrlVmCfg_GetVmType, (
                PRL_HANDLE hVmCfg,
		PRL_VM_TYPE_PTR pnType
                ) );

/* \Returns Operation System template name the Container were created on.
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   sOsTemplate:		[out] A pointer to a buffer that receives
			the result.
   pnBufLength:		[in] The size of the output buffer (in
			bytes). Set the buffer pointer to null
			and this parameter's value to zero to
			receive the required size. [out] The
			required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
						PrlVmCfg_GetOsTemplate, (
		PRL_HANDLE hVmCfg,
		PRL_STR sOsTemplate,
		PRL_UINT32_PTR pnBufLength
		) );

/* Specify the Virtual Machine Operation System template name
   which will be used to create new Container.
   This option acceptable for Virtual Machine of PVT_CT only.
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   sOsTemplate		ostemplate name.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
						PrlVmCfg_SetOsTemplate, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sOsTemplate
		) );

/* Specify the Configuration sample file
   Parameters
   hVmCfg:              A handle of type PHT_VM_CONFIGURATION
                       identifying the virtual machine
                       configuration.
   ConfigSample         Configurationsample name name.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.
   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
                                                PrlVmCfg_ApplyConfigSample, (
                PRL_HANDLE hVmCfg,
                PRL_CONST_STR sConfigSample
                ) );

/* %CT_ONLY%

   Returns the list of application templates installed for the specified
   Container. This option acceptable for Virtual Machine of PVT_CT only.
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   phAppList:		[out] A pointer to a variable that receives the
			PHT_STRINGS_LIST handle containing the list of
			application templates.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a string object.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_GetAppTemplateList, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR phAppList
		) );

/* %CT_ONLY%

   Specify application templates list which will be used in the specified
   Container.
   This option acceptable for Virtual Machine of PVT_CT only.
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   hAppList:		A handle of type PHT_STRINGS_LIST
			that specifies the list of application templates
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_SetAppTemplateList, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE hAppList
		) );

/////////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the Containers.
/////////////////////////////////////////////////////////////////////////////////

/* %CT_ONLY%

   Sets barrier and limit for various user beancounters.
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   nResourceIdThe	resource id (Only PCR_SWAPPAGES supported for now).
   nBarrirerThe		resource barrier.
   nLimit		The resource limit

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.
   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetResource, (
		PRL_HANDLE hVmCfg,
		PRL_CT_RESOURCE nResourceId,
		PRL_UINT64 nBarrirer,
		PRL_UINT64 nLimit
		) );

/* %CT_ONLY%

   Returns beancounters barrier and limit for specified resource id.
   were created on.
   Parameters
   hVmCfg:		A handle of type PHT_VM_CONFIGURATION
			identifying the virtual machine
			configuration.
   nResourceId		The resource id (Only PCR_SWAPPAGES supported for now).
   pnBarrirer		[out]The resource barrier.
   pnLimit		[out]The resource limit
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.
   PRL_ERR_PARAM_NOT_FOUND - resource not found.
   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_GetResource, (
		PRL_HANDLE hVmCfg,
		PRL_CT_RESOURCE nResourceId,
		PRL_UINT64_PTR pnBarrirer,
		PRL_UINT64_PTR pnLimit
		) );


/* \Returns the Container id of the specified
   virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   sCtId      :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnCtIdBufLength :       [in] The size of the output buffer (in
                           bytes). Set the buffer pointer to null
                           and this parameter's value to zero to
                           receive the required size. [out] The
                           required output buffer size.
 
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
                                 PrlVmCfg_GetCtId, (
                PRL_HANDLE hVmCfg,
		PRL_STR sCtId,
		PRL_UINT32_PTR pnCtIdBufLength
                ) );


/* %CT_ONLY%

   Returns a capabilities mask specified inside the CT.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   pnCapMask :    [out] A pointer to a bitmask containing the supported
                  capabilities.  For available options see
                  the PRL_CT_CAPMASK enumeration and
                  CAPDEFAULTMASK defined in PrlEnums.h as macros.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetCapabilitiesMask                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_GetCapabilitiesMask, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnCapMask
		) );

/* %CT_ONLY%

   Sets a capabilities inside the CT.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   nCapMask :     [in] A bitmask containing the supported
                  capabilities.  For available options see
                  the PRL_CT_CAPMASK enumeration and
                  CAPDEFAULTMASK defined in PrlEnums.h as macros.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetCapabilitiesMask                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetCapabilitiesMask, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nCapMask
		) );

/* %CT_ONLY%

   Returns a features mask specified inside the CT.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   pnOn :         [out] A pointer to a bitmask containing the enabled
                  features. For available options see
                  the PRL_CT_FEATURES enumeration.
   pnOff :        [out] A pointer to a bitmask containing the disabled
                  features. For available options see
                  the PRL_CT_FEATURES enumeration.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetCapabilitiesMask                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_GetFeaturesMask, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnOn,
		PRL_UINT32_PTR pnOff
		) );

/* %CT_ONLY%

   Returns netfilter mode specified inside the CT.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   pnMode  : [out] A pointer to a variable that receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_SetNetfilterMode                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_GetNetfilterMode, (
		PRL_HANDLE hVmCfg,
		PRL_NETFILTER_MODE_PTR pnMode
		) );


/* %CT_ONLY%

   Allows to set netfilter mode inside the CT.
   Parameters
   hVmCfg : A handle of type PHT_VM_CONFIGURATION
            identifying the virtual machine configuration.
   nMode  : [in] The netfilter mode to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetNetfilterMode                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetNetfilterMode, (
		PRL_HANDLE hVmCfg,
		PRL_NETFILTER_MODE nMode
		) );

/* %CT_ONLY%

   Sets a features inside the CT.
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   nOn:           [in] A bitmask of enabled features.
                  For available options see
                  the PRL_CT_FEATURES enumeration
   nOff:          [in] A bitmask of disabled features.
                  For available options see
                  the PRL_CT_FEATURES enumeration

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmCfg_GetCapabilitiesMask                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmCfg_SetFeaturesMask, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nOn,
		PRL_UINT32 nOff
		) );


/* Get current VM profile.  Use PrlVmCfg_IsCustomProfile to
   detect if settings were modified after profile application.

   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   pnVmProfile : [out] A predefined settings profile.
                 See the PRL_VIRTUAL_MACHINE_PROFILE enumeration
                 for available options.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.			*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				PrlVmCfg_GetProfile, (
					  PRL_HANDLE hVm,
					  PRL_VIRTUAL_MACHINE_PROFILE_PTR pnVmProfile
					  ) );


/* Determines were settings modified after profile was applied
   or not.

   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   pbCustom :    [out] A pointer to variable that indicates that
                 settings were modified after profile was applied.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.			*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				PrlVmCfg_IsCustomProfile, (
					  PRL_HANDLE hVm,
					  PRL_BOOL_PTR pbCustom
					  ) );


/* Set one of predefined VM profiles.

   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   hSrvConfig :   A handle of type PHT_SERVER_CONFIG.
   nVmProfile :   [in] A predefined settings profile.
                  See the PRL_VIRTUAL_MACHINE_PROFILE enumeration
                  for available options.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.			*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				PrlVmCfg_SetProfile, (
					  PRL_HANDLE hVmCfg,
					  PRL_HANDLE hSrvConfig,
					  PRL_VIRTUAL_MACHINE_PROFILE nVmProfile
					  ) );


///////////////////////////////////////////////////////////////////////////////
/// @section Virtual Machine Device object handling routines and data types
///////////////////////////////////////////////////////////////////////////////

/* Creates a new virtual device handle not bound to any virtual
   machine. It is necessary to call PrlHandle_Free after the
   handle usage.
   Parameters
   nDeviceType :  The type of handle to create.
   phVmDevice :   [out] A pointer to a variable that receives the
                  handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_Create, (
		PRL_DEVICE_TYPE nDeviceType,
		PRL_HANDLE_PTR phVmDevice
		) );

/* The PrlVmDev_Connect function allows to connect virtual
   devices while the virtual machine is running. To get the
   return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVmDev :  A handle identifying the virtual device.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVmDev_Connect, (
		PRL_HANDLE hVmDev
		) );

/* Allows to disconnect a device while the virtual machine is
   running. To get the return code from the PHT_JOB object, use
   the PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVmDev :  A handle identifying the virtual device.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                 */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVmDev_Disconnect, (
		PRL_HANDLE hVmDev
		) );

/* Physically creates a virtual device image on the host. To get
   the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVmDev :               A handle identifying the virtual
                          device.
   bRecreateIsAllowed :   Specifies wether the existing image
                          should be replaced. If set to PRL_TRUE,
                          the existing image will be replaced. If
                          set to PRL_FALSE, the operation will be
                          aborted if image already exists.
   bNonInteractiveMode :  Specifies wether the operation should
                          be carried out in interactive mode
                          (where the Dispatcher Service may ask
                          questions and the client will have to
                          send answers). If set to PRL_TRUE,
                          non\-interactive mode will be used (the
                          Dispatcher Service will make decisions
                          on its own). If set to PRL_FALSE,
                          interactive mode will be used.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVmDev_CreateImage, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bRecreateIsAllowed,
		PRL_BOOL bNonInteractiveMode
		) );

/* Copies a virtual device image to another custom location.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVmDev :        A handle identifying the virtual device.
   sNewImageName : New image short name.
   sTargetPath :   New custom location.
   nFlags :        Reserved
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlVmDev_CopyImage, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewImageName,
		PRL_CONST_STR sTargetPath,
		PRL_UINT32 nFlags
		) );

/* Resize a virtual device image. To get
   the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVmDev :		A handle identifying the virtual device.

   nNewSize :		Specifies a new image size in Mbytes.

   nFlags :		bitset of (PRIF_DISK_INFO, PRIF_RESIZE_LAST_PARTITION).
                Also PACF_NON_INTERACTIVE_MODE can be used in order to
				specify non interactive session working mode (remote
				side won't ask questions during operation progress in
				this case).

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVmDev_ResizeImage, (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nNewSize,
		PRL_UINT32 nFlags
		) );

/* \Returns an index identifying the virtual device.
   Parameters
   hVmDev :   A handle identifying the virtual device. This can
              be one of the following\: PHT_VIRTUAL_DEV_FLOPPY,
              PHT_VIRTUAL_DEV_HARD_DISK,
              PHT_VIRTUAL_DEV_OPTICAL_DISK,
              PHT_VIRTUAL_DEV_PARALLEL_PORT,
              PHT_VIRTUAL_DEV_SERIAL_PORT,
              PHT_VIRTUAL_DEV_NET_ADAPTER.
   pnIndex :  [out] A pointer to a variable that receives the
              \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type,
   or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_GetIndex, (
		PRL_HANDLE hVmDev,
		PRL_UINT32_PTR pnIndex
		) );

/* Sets an index identifying the virtual device.
   Parameters
   hVmDev :  A handle identifying the virtual device. This can be
             one of the following\: PHT_VIRTUAL_DEV_FLOPPY,
             PHT_VIRTUAL_DEV_HARD_DISK,
             PHT_VIRTUAL_DEV_OPTICAL_DISK,
             PHT_VIRTUAL_DEV_PARALLEL_PORT,
             PHT_VIRTUAL_DEV_SERIAL_PORT,
             PHT_VIRTUAL_DEV_NET_ADAPTER.
   nIndex :  The new index.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetIndex, (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nIndex
		) );

/* Removes the specified virtual device handle from
   the parent virtual machine list.
   Parameters
   hVmDev :  A handle identifying the virtual device.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_Remove, (
		PRL_HANDLE hVmDev
		) );

/* \Returns the type of the specified device.
   Parameters
   hVmDev :        A handle identifying the virtual device.
   pnDeviceType :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_GetType, (
		PRL_HANDLE hVmDev,
		PRL_DEVICE_TYPE_PTR pnDeviceType
		) );

/**
Checks if the specified virtual device is connected.

Parameters
hVmDev : A handle identifying the virtual device.
pbConnected : [out] A pointer to a variable that receives the
                \result.

Returns
PRL_RESULT. Possible values:
PRL_ERR_INVALID_ARG - invalid device handle or non valid (NULL) pointer to storing result,
or handle of already removed VM device was passed.
PRL_ERR_SUCCESS - operation completed successfully
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_IsConnected, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbConnected
		) );

/* Connects the specified virtual device.
   Parameters
   hVmDev :      A handle identifying the virtual device.
   bConnected :  Value to set. PRL_TRUE to connect the device.
                 PRL_FALSE to disconnect it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetConnected, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bConnected
		) );

/* Determines if the specified device is enabled.
   Parameters
   hVmDev :     A handle identifying the virtual device.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_IsEnabled, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbEnabled
		) );

/* Enables the specified virtual device..
   Parameters
   hVmDev :    A handle identifying the virtual device.
   bEnabled :  Specify PRL_TRUE to enable the device. Specified
               PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetEnabled, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bEnabled
		) );

/* Determines if the specified virtual device is a remote
   device.
   Parameters
   hVmDev :    A handle identifying the virtual device.
   pbRemote :  [out] A pointer to a variable that receives the
               \result. PRL_TRUE indicates that this is a remote
               device. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_IsRemote, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbRemote
		) );

/* Changes the "remote" flag for the specified device.
   Parameters
   hVmDev :   A handle identifying the virtual device.
   bRemote :  Specify PRL_TRUE to make the device appear as
              remote. Specify PRL_FALSE to make the device appear
              as local.
   Returns
   PRL_RESULT. Possible values: PRL_ERR_INVALID_ARG - non valid
   virtual device handle or handle of already removed VM device
   was passed PRL_ERR_SUCCESS - operation completed successfully  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetRemote, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bRemote
		) );

/* \Returns the virtual device emulation type. Emulation types
   are defined in the PRL_VM_DEV_EMULATION_TYPE enumeration. The
   following describes emulation types used for different types
   of virtual devices:
     * PHT_VIRTUAL_DEV_FLOPPY: PDT_USE_REAL_DEVICE,
       PDT_USE_IMAGE_FILE.
     * PHT_VIRTUAL_DEV_HARD_DISK: PDT_USE_REAL_DEVICE,
       PDT_USE_IMAGE_FILE.
     * PHT_VIRTUAL_DEV_OPTICAL_DISK: PDT_USE_REAL_DEVICE,
       PDT_USE_IMAGE_FILE.
     * PHT_VIRTUAL_DEV_SERIAL_PORT: PDT_USE_REAL_DEVICE,
       PDT_USE_OUTPUT_FILE, PDT_USE_OTHER (for socket mode).
     * PHT_VIRTUAL_DEV_PARALLEL_PORT: PDT_USE_REAL_DEVICE,
       PDT_USE_OUTPUT_FILE, PDT_USE_OTHER (for printer mode).
     * PHT_VIRTUAL_DEV_NET_ADAPTER: PDT_USE_REAL_DEVICE
       (host-only network), PDT_USE_IMAGE_FILE (shared network),
       PDT_USE_OUTPUT_FILE (bridget ethernet)
     * PHT_VIRTUAL_DEV_SOUND: PDT_USE_REAL_DEVICE (AC'97),
       PDT_USE_IMAGE_FILE (Creative).
     * PHT_VIRTUAL_DEV_USB_DEVICE: PDT_USE_REAL_DEVICE
   Parameters
   hVmDev :          A handle identifying the virtual device.
   pnEmulatedType :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_GetEmulatedType, (
		PRL_HANDLE hVmDev,
		PRL_VM_DEV_EMULATION_TYPE_PTR pnEmulatedType
		) );

/* Sets the specified virtual device emulation type. Emulation
   types are defined in the PRL_VM_DEV_EMULATION_TYPE
   enumeration. The following describes emulation types used for
   different types of virtual devices:
     * PHT_VIRTUAL_DEV_FLOPPY: PDT_USE_REAL_DEVICE,
       PDT_USE_IMAGE_FILE.
     * PHT_VIRTUAL_DEV_HARD_DISK: PDT_USE_REAL_DEVICE,
       PDT_USE_IMAGE_FILE.
     * PHT_VIRTUAL_DEV_OPTICAL_DISK: PDT_USE_REAL_DEVICE,
       PDT_USE_IMAGE_FILE.
     * PHT_VIRTUAL_DEV_SERIAL_PORT: PDT_USE_REAL_DEVICE,
       PDT_USE_OUTPUT_FILE, PDT_USE_OTHER (for socket mode).
     * PHT_VIRTUAL_DEV_PARALLEL_PORT: PDT_USE_REAL_DEVICE,
       PDT_USE_OUTPUT_FILE, PDT_USE_OTHER (for printer mode).
     * PHT_VIRTUAL_DEV_NET_ADAPTER: PDT_USE_REAL_DEVICE (host
       only network), PDT_USE_IMAGE_FILE (shared network).
       PDT_USE_OUTPUT_FILE (bridget ethernet)
     * PHT_VIRTUAL_DEV_SOUND: PDT_USE_REAL_DEVICE (AC'97),
       PDT_USE_IMAGE_FILE (Creative).
     * PHT_VIRTUAL_DEV_USB_DEVICE: PDT_USE_REAL_DEVICE
   Parameters
   hVmDev :         A handle identifying the virtual device.
   nEmulatedType :  The emulation type to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetEmulatedType, (
		PRL_HANDLE hVmDev,
		PRL_VM_DEV_EMULATION_TYPE nEmulatedType
		) );

/* \Returns virtual device image path.
   Parameters
   hVmDev :                A handle identifying the virtual
                           device. This can be one of the
                           following\: PHT_VIRTUAL_DEV_FLOPPY,
                           PHT_VIRTUAL_DEV_HARD_DISK,
                           PHT_VIRTUAL_DEV_OPTICAL_DISK.
   sImagePath :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnImagePathBufLength :  [in] The size of the output buffer (in
                           bytes). Set the buffer pointer to null
                           and this parameter's value to zero to
                           receive the required size. [out] The
                           required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type,
   or null pointer was passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_GetImagePath, (
		PRL_HANDLE hVmDev,
		PRL_STR sImagePath,
		PRL_UINT32_PTR pnImagePathBufLength
		) );

/* Sets the virtual device image path.
   Parameters
   hVmDev :         A handle identifying the virtual device. This
                    can be one of the following\:
                    PHT_VIRTUAL_DEV_FLOPPY,
                    PHT_VIRTUAL_DEV_HARD_DISK,
                    PHT_VIRTUAL_DEV_OPTICAL_DISK.
   sNewImagePath :  The image path to set. The value must be a
                    UTF\-8 encoded, null\-terminated string
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetImagePath, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewImagePath
		) );

/* \Returns the virtual device system name.
   Parameters
   hVmDev :              A handle identifying the virtual device.
   sSysName :            [out] A pointer to a buffer that
                         receives the result (a UTF\-8 encoded,
                         null\-terminated string).
   pnSysNameBufLength :  [in] The size of the output buffer (in
                         bytes). Set the buffer pointer to null
                         and this parameter's value to zero to
                         receive the required size. [out] The
                         required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_GetSysName, (
		PRL_HANDLE hVmDev,
		PRL_STR sSysName,
		PRL_UINT32_PTR pnSysNameBufLength
		) );

/* Sets virtual device system name.
   Parameters
   hVmDev :       A handle identifying the virtual device.
   sNewSysName :  The new system name to set. The value must be a
                  UTF\-8 encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetSysName, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewSysName
		) );

/* \Returns the virtual device user friendly name.
   Parameters
   hVmDev :                   A handle identifying the virtual
                              device.
   sFriendlyName :            [out] A pointer to a buffer that
                              receives the result (a UTF\-8
                              encoded, null\-terminated string).
   pnFriendlyNameBufLength :  [in] The size of the output buffer
                              (in bytes). Set the buffer pointer
                              to null and this parameter's value
                              to zero to receive the required
                              size. [out] The required output
                              buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_GetFriendlyName, (
		PRL_HANDLE hVmDev,
		PRL_STR sFriendlyName,
		PRL_UINT32_PTR pnFriendlyNameBufLength
		) );

/* Sets virtual device user-friendly name.
   Parameters
   hVmDev :            A handle identifying the virtual device.
   sNewFriendlyName :  The user\-friendly name to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetFriendlyName, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewFriendlyName
		) );

/* \Returns the description of a specified virtual device.
   Parameters
   hVmDev :                  A handle identifying the virtual
                             device.
   sDescription :            [out] A pointer to a buffer that
                             receives the result (a UTF\-8
                             encoded, null\-terminated string).
   pnDescriptionBufLength :  [in] The size of the output buffer
                             (in bytes). Set the buffer pointer
                             to null and this parameter's value
                             to zero to receive the required
                             size. [out] The required output
                             buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The pnDescriptionBufLength parameter will
   contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDev_SetDescription                                       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDev_GetDescription, (
		PRL_HANDLE hVmDev,
		PRL_STR sDescription,
		PRL_UINT32_PTR pnDescriptionBufLength
		) );

/* Sets the description for a specified virtual device.
   Parameters
   hVmDev :           A handle identifying the virtual device.
   sNewDescription :  The new description to set. The value must
                      be a UTF\-8 encoded, null\-terminated
                      string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDev_GetDescription                                       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDev_SetDescription, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewDescription
		) );

/* \Returns the virtual device interface type (IDE or SCSI).
   Parameters
   hVmDev :       A handle identifying the virtual device. This
                  can be one of the following\:
                  PHT_VIRTUAL_DEV_HARD_DISK,
                  PHT_VIRTUAL_DEV_OPTICAL_DISK.
   pnIfaceType :  [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type,
   or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_GetIfaceType, (
		PRL_HANDLE hVmDev,
		PRL_MASS_STORAGE_INTERFACE_TYPE_PTR pnIfaceType
		) );

/* Sets the virtual device interface type (IDE or SCSI).
   Parameters
   hVmDev :      A handle identifying the virtual device. This
                 can be one of the following\:
                 PHT_VIRTUAL_DEV_HARD_DISK,
                 PHT_VIRTUAL_DEV_OPTICAL_DISK,
				 PHT_VIRTUAL_DEV_GENERIC_SCSI.
   nIfaceType :  The interface type to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type,
   or PMS_IDE_DEVICE for PHT_VIRTUAL_DEV_GENERIC_SCSI handle type was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetIfaceType, (
		PRL_HANDLE hVmDev,
		PRL_MASS_STORAGE_INTERFACE_TYPE nIfaceType
		) );

/* \Returns the virtual device subtype.
   Parameters
   hVmDev :       A handle identifying the virtual device. This
                  can be one of the following\:
                  PHT_VIRTUAL_DEV_HARD_DISK,
                  PHT_VIRTUAL_DEV_OPTICAL_DISK.
   pnSubType :  [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmDev_GetSubType, (
		PRL_HANDLE hVmDev,
		PRL_CLUSTERED_DEVICE_SUBTYPE_PTR pnSubType
		) );

/* Sets the virtual device subtype.
   Parameters
   hVmDev :      A handle identifying the virtual device. This
                 can be one of the following\:
                 PHT_VIRTUAL_DEV_HARD_DISK,
                 PHT_VIRTUAL_DEV_OPTICAL_DISK,
				 PHT_VIRTUAL_DEV_GENERIC_SCSI.
   nSubType :  The subtype to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmDev_SetSubType, (
		PRL_HANDLE hVmDev,
		PRL_CLUSTERED_DEVICE_SUBTYPE nSubType
		) );

/* \Returns the virtual device stack index (position at the
   controller (IDE or SCSI) bus).
   Parameters
   hVmDev :        A handle identifying the virtual device. This
                   can be one of the following\:
                   PHT_VIRTUAL_DEV_HARD_DISK,
                   PHT_VIRTUAL_DEV_OPTICAL_DISK,
				   PHT_VIRTUAL_DEV_GENERIC_SCSI,
				   PHT_VIRTUAL_DEV_GENERIC_PCI.
   pnStackIndex :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type,
   or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_GetStackIndex, (
		PRL_HANDLE hVmDev,
		PRL_UINT32_PTR pnStackIndex
		) );

/* Sets the virtual device stack index (position at the IDE or
   SCSI controller bus).
   Parameters
   hVmDev :       A handle identifying the virtual device. This
                  can be one of the following\:
                  PHT_VIRTUAL_DEV_HARD_DISK,
                  PHT_VIRTUAL_DEV_OPTICAL_DISK,
				  PHT_VIRTUAL_DEV_GENERIC_SCSI,
				  PHT_VIRTUAL_DEV_GENERIC_PCI.
   nStackIndex :  A new index to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type
   was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetStackIndex, (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nStackIndex
		) );

/* Automatically generates and assigns a stack index to a
   virtual device (a corresponding device interface, IDE or
   SCSI, has to be set in advance).
   Parameters
   hVmDev :  A handle identifying the virtual device. This can be
             one of the following\: PHT_VIRTUAL_DEV_HARD_DISK,
             PHT_VIRTUAL_DEV_OPTICAL_DISK, PHT_VIRTUAL_DEV_GENERIC_SCSI
			 or PHT_VIRTUAL_DEV_GENERIC_PCI.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type
   was passed.

   PRL_ERR_NO_MORE_FREE_INTERFACE_SLOTS - there are no free
   slots left.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetDefaultStackIndex, (
		PRL_HANDLE hVmDev
		) );

/* \Returns the virtual device output file.
   Parameters
   hVmDev :                 A handle identifying the virtual
                            device. This can be one of the
                            following\:
                            PHT_VIRTUAL_DEV_PARALLEL_PORT,
                            PHT_VIRTUAL_DEV_SERIAL_PORT.
   sOutputFile :            [out] A pointer to a buffer that
                            receives the result (a UTF\-8
                            encoded, null\-terminated string).
   pnOutputFileBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this parameter's value to
                            zero to receive the required size.
                            [out] The required output buffer
                            size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type,
   or null pointer was passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_GetOutputFile, (
		PRL_HANDLE hVmDev,
		PRL_STR sOutputFile,
		PRL_UINT32_PTR pnOutputFileBufLength
		) );

/* Sets the virtual device output file.
   Parameters
   hVmDev :          A handle identifying the virtual device.
                     This can be one of the following\:
                     PHT_VIRTUAL_DEV_PARALLEL_PORT,
                     PHT_VIRTUAL_DEV_SERIAL_PORT.
   sNewOutputFile :  Name and path of the new output file.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type,
   or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetOutputFile, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewOutputFile
		) );

/* Determines if the passthrough mode is enabled for the
   specified mass storage device.
   Parameters
   hVmDev :         A handle identifying the device. This can be
                    one of the following\:
                    PHT_VIRTUAL_DEV_OPTICAL_DISK,
                    PHT_VIRTUAL_DEV_HARD_DISK.
   pbPassthrough :  [out] A pointer to a variable that receives
                    the result. PRL_TRUE indicates that the
                    passthrough mode is enabled. PRL_FALSE
                    indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_IsPassthrough, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbPassthrough
		) );

/* Enables the passthrough mode for the specified mass storage
   device (optical ot hard disk).
   Parameters
   hVmDev :        A handle identifying the device. This can be
                   one of the following\:
                   PHT_VIRTUAL_DEV_OPTICAL_DISK,
                   PHT_VIRTUAL_DEV_HARD_DISK.
   bPassthrough :  Specify PRL_TRUE to enable the passthrough
                   mode. Specify PRL_FALSE to disab;e it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid handle type
   was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_SetPassthrough, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bPassthrough
		) );


/* \Returns the hard disk type. A virtual hard disk can be one
   of two types: expanding or fixed. The size of an expanding
   hard disk is expanded dynamically up to a predefined limit.
   The size of a fixed hard disk is set to the specified value
   at the time of creation.
   Parameters
   hVmDev :      A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                 identifying the virtual hard disk.
   pnDiskType :  [out] Pointer to a variable that receives the
                 disk type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_SetDiskType                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevHd_GetDiskType, (
		PRL_HANDLE hVmDev,
		PRL_HARD_DISK_INTERNAL_FORMAT_PTR pnDiskType
		) );

/* Sets the type of the specified virtual hard disk. A virtual
   hard disk can be one of the two types: expanding or
   fixed-size. The size of an expanding hard disk is expanded
   dynamically up to a predefined limit. The size of a
   fixed-size hard disk is set to the specified value at the
   time of creation. The function can be used with both new and
   existing virtual hard disks.
   Parameters
   hVmDev :     A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                identifying the virtual hard disk.
   nDiskType :  The hard disk type to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_GetDiskType                                        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevHd_SetDiskType, (
		PRL_HANDLE hVmDev,
		PRL_HARD_DISK_INTERNAL_FORMAT nDiskType
		) );

/* Determines if the virtual hard disk is split into multiple
   files. When adding a virtual hard disk to a virtual machine,
   you have an option of splitting the disk into multiple files.
   By using this function, you can determine whether this
   options was used or not.
   Parameters
   hVmDev :      A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                 identifying the virtual hard disk.
   pbSplitted :  [out] Pointer to a variable that receives the
                 \result. PRL_TRUE indicates that the disk is
                 split into multiple files. PRL_FALSE indicates
                 that a single file is used.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_SetSplitted                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevHd_IsSplitted, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbSplitted
		) );

/* Allows to specify whether the hard disk should be split into
   multiple files or if a single file should be used.
   Parameters
   hVmDev :     A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                identifying the virtual hard disk.
   bSplitted :  Hard disk file option. Specify PRL_TRUE to split
                the disk into multiple files. Specify PRL_FALSE
                to use a single file.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_IsSplitted                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevHd_SetSplitted, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bSplitted
		) );

/* \Returns the hard disk size.
   Parameters
   hVmDev :      A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                 identifying the virtual hard disk.
   pnDiskSize :  [out] Pointer to a variable that receives the
                 hard disk size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_SetDiskSize                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevHd_GetDiskSize, (
		PRL_HANDLE hVmDev,
		PRL_UINT32_PTR pnDiskSize
		) );

/* Sets the size of the specified virtual hard disk. Use this
   function when adding a new virtual hard disk or when
   modifying the size of an existing disk.
   Parameters
   hVmDev :     A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                identifying the virtual hard disk.
   nDiskSize :  The new size in megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_GetDiskSize                                        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevHd_SetDiskSize, (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nDiskSize
		) );

/* \Returns the size of the occupied space on the hard disk.
   Parameters
   hVmDev :        A valid handle of type
                   PHT_VIRTUAL_DEV_HARD_DISK identifying the
                   virtual hard disk.
   pnOnDiskSize :  [out] Pointer to a variable that receives the
                   size. The size is specified in bytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevHd_GetSizeOnDisk, (
		PRL_HANDLE hVmDev,
		PRL_UINT32_PTR pnOnDiskSize
		) );

/* Assigns a boot camp partition to the specified virtual hard
   disk. When creating a virtual machine based on a boot camp
   partition, this function must be used to obtain a handle of
   type PHT_VIRTUAL_DEV_HD_PARTITION. The handle can then be
   used to set the system name of the boot camp partition that
   you would like to use.
   Parameters
   hVmDev :       A valid handle of type
                  PHT_VIRTUAL_DEV_HARD_DISK identifying the
                  virtual hard disk.
   phPartition :  [out] A pointer to a variable that receives a
                  handle of type PHT_VIRTUAL_DEV_HD_PARTITION
                  containing the new partition information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevHd_AddPartition, (
		PRL_HANDLE hVmDev,
		PRL_HANDLE_PTR phPartition
		) );

/* Determines the number of partitions on the virtual hard disk.
   Parameters
   hVmDev :             A valid handle of type
                        PHT_VIRTUAL_DEV_HARD_DISK identifying the
                        virtual hard disk.
   pnPartitionsCount :  [out] A pointer to a variable that
                        receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_GetPartition                                        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevHd_GetPartitionsCount, (
		 PRL_HANDLE hVmDev,
		 PRL_UINT32_PTR pnPartitionsCount
		 ) );

/* Obtains a handle of type PHT_VIRTUAL_DEV_HD_PARTITION
   containing information about the specified partition of the
   virtual hard disk.
   Parameters
   hVmDev :       A valid handle of type
                  PHT_VIRTUAL_DEV_HARD_DISK identifying the
                  virtual hard disk.
   nIndex :       An index identifying the partition instance
                  (beginning with 0). To get the number of
                  partitions of the virtual hard disk, use the
                  PrlVmDevHd_GetPartitionsCount function.
   phPartition :  [out] A pointer to a variable that receives the
                  handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed, or device index out of bound.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_GetPartitionsCount                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevHd_GetPartition, (
		 PRL_HANDLE hVmDev,
		 PRL_UINT32 nIndex,
		 PRL_HANDLE_PTR phPartition
		 ) );

/* Sets the mount point of the specified virtual hard disk.
   (for Container only)
   Parameters
   hVmDev :      A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                 identifying the virtual hard disk.
   sMountPoint : Mount point path

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_GetMountPoint                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
                       PrlVmDevHd_SetMountPoint, (
               PRL_HANDLE hVmDev,
               PRL_CONST_STR sMountPoint
               ) );

/* Returns the mount point of the specified virtual hard disk.
   Parameters
   hVmDev :      A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                 identifying the virtual hard disk.
   sMountPoint :            [out] A pointer to a buffer that
                            receives the result (a UTF\-8
                            encoded, null\-terminated string).
   pnMountPointBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this parameter's value to
                            zero to receive the required size.
                            [out] The required output buffer
                            size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_SetMountPoint,           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
                       PrlVmDevHd_GetMountPoint, (
               PRL_HANDLE hVmDev,
               PRL_STR sMountPoint,
               PRL_UINT32_PTR pnMountPointBufLength
               ) );

/*
   Return custom storage URL.

   Parameters
   hVmDev :         A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                    identifying the virtual hard disk.
   sURL :           [out] A pointer to a buffer that receives the result (a UTF\-8 encoded,
                    null\-terminated string).
   pnURLBufLength : [in] The size of the output buffer (in bytes). Set the buffer pointer to null
                    and this parameter's value to zero to receive the required size.
                    [out] The required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.

   See Also
   PrlVmDevHd_SetStorageURL                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmDevHd_GetStorageURL, (
		PRL_HANDLE hVmDev,
		PRL_STR sURL,
		PRL_UINT32_PTR pnURLBufLength
		));

/*
   Set custom storage URL.

   A storage URL is required when actual data of a virtual hard disk is
   accessed using some external tool or protocol. For example, a VM disk
   backup, which resides on a remote server, could be attached as a data
   source using the following storage URL:
   "backup://backup_server/backup_UUID/disk_number".
   Here, instead of reading data from the locally stored disk image, a VM will
   connect to the "backup_server" and read data from the "backup_UUID/disk_number"
   backup.

   Parameters
   hVmDev :      A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                 identifying the virtual hard disk.
   sURL   :     [in] Storage URL - an UTF-8 encoded string with null '\0' symbol at the end.
                The following URL types are supported:
                "backup://backup_server/backup_UUID/disk_number" - use disk backup,
                identified by "backup_UUID/disk_number" from server "backup_server".
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   See Also
   PrlVmDevHd_GetStorageURL                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmDevHd_SetStorageURL, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sURL
		));

/* Returns the auto compress flag of the specified virtual hard disk.
   (for Container only)
   Parameters
   hVmDev :      A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                 identifying the virtual hard disk.
   pbEnabled :   [out] A pointer to a variable that
                 receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_IsAutoCompressEnabled,		*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmDevHd_IsAutoCompressEnabled, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbEnabled
		) );

/* Sets the auto compress sign flag of the specified virtual hard disk.
   (for Container only)
   Parameters
   hVmDev :      A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                 identifying the virtual hard disk.
   bEnabled :    Specify PRL_FALSE to disable auto compress

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_GetMountPoint				*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVmDevHd_SetAutoCompressEnabled, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bEnabled
		) );

/*
   Return disk serial number.

   Parameters
   hVmDev :         A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                    identifying the virtual hard disk.
   sSerialNumber :  [out] A pointer to a buffer that receives the result (a UTF\-8 encoded,
                    null\-terminated string).
   pnSerialNumberBufLength : [in] The size of the output buffer (in bytes). Set the buffer pointer to null
                    and this parameter's value to zero to receive the required size.
                    [out] The required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				 PrlVmDevHd_GetSerialNumber, (
		PRL_HANDLE hVmDev,
		PRL_STR sSerialNumber,
		PRL_UINT32_PTR pnSerialNumberBufLength
		));

/*
   Set disk serial number.

   Parameters
   hVmDev :	A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                identifying the virtual hard disk.
   sSerialNumber : [in] Serial number - a UTF-8 encoded string with null '\0' symbol at the end.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_GetSerialNumber
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				PrlVmDevHd_SetSerialNumber, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sSerialNumber
		));

/* \Returns the disk encryption parameters
   Parameters
   hVmDev :         A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                    identifying the virtual hard disk.
   phEncryption:   [out] A pointer to a variable that receives the
			PHT_DISK_ENCRYPTION handle containing the disk
			encryption parameters.
   Returns
   PRL_RESULT. */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			PrlVmDevHd_GetEncryption, (
		PRL_HANDLE hVmDev,
		PRL_HANDLE_PTR phEncryption
		) );

/* Specify disk encryption parameters
   Parameters
   hVmDev :         A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                    identifying the virtual hard disk.
   phEncryption:    PHT_DISK_ENCRYPTION handle containing the disk
			encryption parameters.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			PrlVmDevHd_SetEncryption, (
		PRL_HANDLE hVmDev,
		PRL_HANDLE hEncryption
		) );

/* Removes the specified partition handle from the virtual hard
   disk list.
   Parameters
   hPartition :  A handle identifying the partition.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevHdPart_Remove, (
		PRL_HANDLE hPartition
		) );

/* \Returns the system name of the specified disk partition.
   Parameters
   hPartition :          A handle of type
                         PHT_VIRTUAL_DEV_HD_PARTITION identifying
                         the disk partition.
   sSysName :            [out] A pointer to a buffer that
                         receives the name (a UTF\-8 encoded,
                         null\-terminated string).
   pnSysNameBufLength :  [in] The size of the output buffer (in
                         bytes). Set the buffer pointer to null
                         and this value to zero to receive the
                         required size. [out] The required output
                         buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHdPart_SetSysName                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevHdPart_GetSysName, (
		PRL_HANDLE hPartition,
		PRL_STR	sSysName,
		PRL_UINT32_PTR pnSysNameBufLength
		) );

/* Sets system name for the specified disk partition.
   Parameters
   hPartition :  A handle of type PHT_VIRTUAL_DEV_HD_PARTITION
                 identifying the disk partition.
   sSysName :    A system name to set for the partition.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHdPart_GetSysName                                   */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevHdPart_SetSysName, (
		PRL_HANDLE hPartition,
		PRL_CONST_STR sSysName
		) );

/*
   Return disk encryption key identifier.

   Parameters
   hEncryption :    A valid handle of type PHT_VIRTUAL_DISK_ENCRYPTION
                    identifying the virtual hard disk encryption parameters.
   sKeyID           [out] A pointer to a buffer that receives the result (a UTF\-8 encoded,
                    null\-terminated string).
   pnKeyIdBufLength : [in] The size of the output buffer (in bytes). Set the buffer pointer to null
                    and this parameter's value to zero to receive the required size.
                    [out] The required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.

   See Also
   PrlVmDevHdEncryption_SetKeyId
   PrlVmDevHd_GetEncryption                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			PrlVmDevHdEncryption_GetKeyId, (
		PRL_HANDLE hEncryption,
		PRL_STR sKeyId,
		PRL_UINT32_PTR pnKeyIdBufLength
		));

/*
   Set disk encryption key identifier.

   Parameters
   hEncryption:  A valid handle of type PHT_VIRTUAL_DISK_ENCRYPTION
                 identifying the virtual hard disk.
   sKeyId   :     [in] Key identifier - an UTF-8 encoded string with null '\0' symbol at the end.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   See Also
   PrlVmDevHdEncryption_GetKeyId
   PrlVmDevHd_GetEncryption                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			 PrlVmDevHdEncryption_SetKeyId, (
		PRL_HANDLE hEncryption,
		PRL_CONST_STR sKeyId
		));

/* \Returns the index of the adapter to which the specified
   virtual adapter is bound. In a bridged networking mode, a
   network adapter inside a virtual machine is bound to an
   adapter on the host. This function allows to retrieve the
   index of the adapter from the host.
   Parameters
   hVmDev :   A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
              identifying the virtual network adapter.
   pnIndex :  [out] A pointer to a variable that receives the
              index. A returned value of \-1 (negative one)
              indicates that the virtual network adapter is bound
              to the default adapter on the host.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetBoundAdapterIndex                               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevNet_GetBoundAdapterIndex, (
		PRL_HANDLE hVmDev,
		PRL_INT32_PTR pnIndex
		) );

/* Sets the index of the adapter to which the specified virtual
   adapter should be bound. In a bridged networking mode, a
   network adapter inside a virtual machine is bound to an
   adapter on the host. This function allows to set the index of
   the adapter from the host.
   Parameters
   hVmDev :  A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
             identifying the virtual network adapter.
   nIndex :  The index of the adapter to which the specified
             virtual adapter should be bound. If you would like
             to bind the virtual adapter to the default adapter,
             specify \-1 (negative one).
   Returns
   PRL_RESULT. Possible values: PRL_ERR_INVALID_ARG - non valid
   net VM device handle or handle of already removed net VM
   device was passed PRL_ERR_SUCCESS - operation successfully
   completed
   See Also
   PrlVmDevNet_GetBoundAdapterIndex                              */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevNet_SetBoundAdapterIndex, (
		PRL_HANDLE hVmDev,
		PRL_INT32 nIndex
		) );

/* \Returns the name of the adapter to which the specified
   virtual adapter is bound. In a bridged networking mode, a
   network adapter inside a virtual machine is bound to an
   adapter on the host. This function allows to retrieve the
   name of the adapter from the host.
   Parameters
   hVmDev :                       A handle of type
                                  PHT_VIRTUAL_DEV_NET_ADAPTER
                                  identifying the virtual network
                                  adapter.
   sBoundAdapterName :            [out] Pointer to a buffer that
                                  receives the physical adapter
                                  name (a UTF\-8 encoded,
                                  null\-terminated string).
   pnBoundAdapterNameBufLength :  [in] The size of the output
                                  buffer (in bytes). Set the
                                  buffer pointer to null and this
                                  value to zero to receive the
                                  required size. [out] The
                                  required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetBoundAdapterName                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevNet_GetBoundAdapterName, (
		PRL_HANDLE hVmDev,
		PRL_STR sBoundAdapterName,
		PRL_UINT32_PTR pnBoundAdapterNameBufLength
		) );

/* Sets the name of the network adapter to which a virtual
   network adapter inside a virtual machine will bind. In a
   bridged networking mode, a network adapter inside a virtual
   machine is bound to an adapter on the host. This function
   allows to set the name of the adapter to bind to.
   Parameters
   hVmDev :                A handle of type
                           PHT_VIRTUAL_DEV_NET_ADAPTER
                           identifying the virtual network
                           adapter.
   sNewBoundAdapterName :  The name of the adapter to bind to.
                           The adapter must exist on the host. To
                           get the names of the available
                           adapters from the host, use the
                           PrlSrvCfg_GetNetAdapter function.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_GetBoundAdapterName                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevNet_SetBoundAdapterName, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewBoundAdapterName
		) );

/* \Returns the MAC address of a virtual network adapter inside
   a virtual machine (in format XXXXXXXXXXXX).
   Parameters
   hVmDev :                 A handle of type
                            PHT_VIRTUAL_DEV_NET_ADAPTER
                            identifying the virtual network
                            adapter.
   sMacAddress :            [out] Pointer to a buffer that
                            receives the MAC address (a UTF\-8
                            encoded, null\-terminated string).
   pnMacAddressBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this value to zero to
                            receive the required size. [out] The
                            required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_GetMacAddressCanonical,
   PrlVmDevNet_SetMacAddress                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevNet_GetMacAddress, (
		PRL_HANDLE hVmDev,
		PRL_STR sMacAddress,
		PRL_UINT32_PTR pnMacAddressBufLength
		) );

/* \Returns the MAC address of a virtual network adapter inside
   a virtual machine (in human-friendly format xx:xx:xx:xx:xx:xx).
   Parameters
   hVmDev :                 A handle of type
                            PHT_VIRTUAL_DEV_NET_ADAPTER
                            identifying the virtual network
                            adapter.
   sMacAddress :            [out] Pointer to a buffer that
                            receives the MAC address (a UTF\-8
                            encoded, null\-terminated string).
   pnMacAddressBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this value to zero to
                            receive the required size. [out] The
                            required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetMacAddress                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlVmDevNet_GetMacAddressCanonical, (
		PRL_HANDLE hVmDev,
		PRL_STR sMacAddress,
		PRL_UINT32_PTR pnMacAddressBufLength
		) );


/* Allows to manually set MAC address to a network adapter
   inside a virtual machine. Use this function if you would like
   to set a specific MAC address to an adapter. To generate an
   address and to set it to an adapter automatically, use the
   PrlVmDevNet_GenerateMacAddr function.
   Parameters
   hVmDev :          A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
                     identifying the virtual network adapter.
   sNewMacAddress :  The new MAC address. The specified string
                     must be encoded using UTF\-8 and must be
                     terminated with a binary zero character. The
                     MAC address value cannot contain any
                     delimiters. The following is an example of a
                     valid MAC address\: "001731EE6FBC".
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_GetMacAddress                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevNet_SetMacAddress, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewMacAddress
		) );

/* Generates a unique MAC address for a virtual network adapter
   on a virtual machine. The function will generate a new MAC
   address and will automatically assign it to the adapter. The
   function does not return the address to the caller. To
   retrieve the address, use the PrlVmDevNet_GetMacAddress
   function.
   Parameters
   hVmDev :  A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
             identifying the network adapter.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevNet_GenerateMacAddr, (
		PRL_HANDLE hVmDev
		) );

/* Determines if the specified network adapter is configured
   to automatically apply network settings inside guest.
   Parameters
   hVmDev :               A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
                          identifying the virtual network
                          adapter.
   pbAutoApply :  [out] A pointer to a variable that
                          receives the result. PRL_TRUE indicates
                          that the adapter is configured to automatically
                          apply network settings. PRL_FALSE indicates
                          otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_IsAutoApply, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbAutoApply
		) );

/* Allows to specify whether a network adapter in a virtual
   machine should be automatically configured. Note:
   For these changes to take effect, Tools must be
   installed in a target virtual machine.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                         the virtual network adapter.
   bAutoApply :  Specify PRL_TRUE to configure the
                         adapter automatically. Specified
                         PRL_FALSE to configure the network
                         settings manually in a virtual machine.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_SetAutoApply, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bAutoApply
		) );

/* Obtains a list of IP addresses in the address/subnet_mask
   format which is assigned to a virtual network adapter on
   virtual machine startup.

   \Returns the list of pairs "IP address/Subnet mask" which
   will be applied to the network adapter on the guest OS side
   (on guest start or immediately on config commit if guest
   already working and Tools with necessary
   functionality are installed). The result will be returned as
   list of strings (PHT_STRINGS_LIST type) of pairs "IP
   address/Subnet mask".
   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                         the virtual network adapter.
   phNetAddressesList :  [out] A pointer to the PHT_STRINGS_LIST
                         handle containing the IP addresses.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings
   list object.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetNetAddresses                                   */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_GetNetAddresses, (
		PRL_HANDLE hVmDev,
		PRL_HANDLE_PTR phNetAddressesList
		) );

/* Allows to manually set IP addresses/Subnet masks to a network
   adapter in a virtual machine. <b>Note</b>: For these changes
   to take effect, Tools must be installed in the target
   virtual machine, and its network adapter settings should be
   configured to change automatically.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmDev :             A handle of type
                        PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                        the virtual network adapter.
   hNetAddressesList :  A handle of type PHT_STRINGS_LIST
                        containing the list of "IP address/Subnet
                        mask" pairs to be applied to the network
                        adapter. Can be PRL_INVALID_HANDLE if
                        it's necessary to specify an empty list.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of virtual network
   adapter specified.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetAutoApply					*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_SetNetAddresses, (
		PRL_HANDLE hVmDev,
		PRL_HANDLE hNetAddressesList
		) );


/* Obtains the list of DNS servers which is assigned to a
   virtual network adapter on virtual machine startup.
   Parameters
   hVmDev :            A handle of type
                       PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                       the virtual network adapter.
   phDnsServersList :  [out] A pointer to the PHT_STRINGS_LIST
                       handle that receives the list of DNS
                       servers.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings
   list object.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetDnsServers                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_GetDnsServers, (
		PRL_HANDLE hVmDev,
		PRL_HANDLE_PTR phDnsServersList
		) );

/* Allows to manually assign DNS servers to a network adapter in
   a virtual machine. <b>Note</b>: For these changes to take
   effect, Tools must be installed in the target virtual
   machine, and its network adapter settings should be configured
   to change automatically.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmDev :           A handle of type
                      PHT_VIRTUAL_DEV_NET_ADAPTER identifying the
                      virtual network adapter.
   hDnsServersList :  A handle of type PHT_STRINGS_LIST
                      containing the list of DNS servers to be
                      assigned to the network adapter. Can be
                      PRL_INVALID_HANDLE if it's necessary to
                      specify an empty list.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of virtual network
   adapter specified.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetAutoApply					*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_SetDnsServers, (
		PRL_HANDLE hVmDev,
		PRL_HANDLE hDnsServersList
		) );


/* Obtains a lists of search domains which are assigned to a
   virtual network adapter on virtual machine startup.
   Parameters
   hVmDev :               A handle of type
                          PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                          the virtual network adapter.
   phSearchDomainsList :  [out] A pointer to the PHT_STRINGS_LIST
                          handle containing the results.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings
   list object.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetSearchDomains                                   */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_GetSearchDomains, (
		PRL_HANDLE hVmDev,
		PRL_HANDLE_PTR phSearchDomainsList
		) );

/* Allows to manually assign search domains to a network adapter
   in a virtual machine. <b>Note</b>: For these changes to take
   effect, Tools must be installed in the target virtual
   machine, and its network adapter settings should be configured
   to change automatically.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                         the virtual network adapter.
   hSearchDomainsList :  A handle of type PHT_STRINGS_LIST
                         containing the list of search domains to
                         be assigned to the network adapter. Can
                         be PRL_INVALID_HANDLE iif it's necessary
                         to specify an empty list.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of virtual network
   adapter specified.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetAutoApply                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_SetSearchDomains, (
		PRL_HANDLE hVmDev,
		PRL_HANDLE hSearchDomainsList
		) );


/* Determines if the specified network adapter is configured
   through DHCP on the guest OS side.
   Parameters
   hVmDev :               A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
                          identifying the virtual network
                          adapter.
   pbConfigureWithDhcp :  [out] A pointer to a variable that
                          receives the result. PRL_TRUE indicates
                          that the adapter is configured through
                          DHCP. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_IsConfigureWithDhcp, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbConfigureWithDhcp
		) );

/* Allows to specify whether a network adapter in a virtual
   machine should be configured through DHCP or manually. Note:
   For these changes to take effect, Tools must be
   installed in the target virtual machine, and its network adapter
   settings should be configured to change automatically.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                         the virtual network adapter.
   bConfigureWithDhcp :  Specify PRL_TRUE to configure the
                         adapter through DHCP. Specified
                         PRL_FALSE to configure the network
                         settings manually.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetAutoApply				            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_SetConfigureWithDhcp, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bConfigureWithDhcp
		) );


/* Determines if the specified network adapter is configured
   through DHCP (IPv6) on the guest OS side.
   Parameters
   hVmDev :               A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
                          identifying the virtual network
                          adapter.
   pbConfigureWithDhcp :  [out] A pointer to a variable that
                          receives the result. PRL_TRUE indicates
                          that the adapter is configured through
                          DHCP. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmDevNet_IsConfigureWithDhcpIPv6, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbConfigureWithDhcp
		) );

/* Allows to specify whether a network adapter in a virtual
   machine should be configured through DHCP (IPv6) or manually. Note:
   For these changes to take effect, Tools must be
   installed in the target virtual machine, and its network adapter
   settings should be configured to change automatically.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                         the virtual network adapter.
   bConfigureWithDhcp :  Specify PRL_TRUE to configure the
                         adapter through DHCP. Specified
                         PRL_FALSE to configure the network
                         settings manually.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetAutoApply				            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmDevNet_SetConfigureWithDhcpIPv6, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bConfigureWithDhcp
		) );


/* Obtains the default gateway assigned to a virtual network
   adapter in a virtual machine.
   Parameters
   hVmDev :                 A handle of type
                            PHT_VIRTUAL_DEV_NET_ADAPTER
                            identifying the virtual network
                            adapter.
   sDefaultGateway :        [out] A pointer to a buffer that
                            receives the default gateway address
                            (a UTF\-8 encoded, null\-terminated
                            string).
   pnDefaultGatewayBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this value to zero to
                            receive the required size. [out] The
                            required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetDefaultGateway                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_GetDefaultGateway, (
		PRL_HANDLE hVmDev,
		PRL_STR sDefaultGateway,
		PRL_UINT32_PTR pnDefaultGatewayBufLength
		) );

/* Allows to manually set the default gateway address for a
   network adapter in a virtual machine. <b>Note</b>: For these
   changes to take effect, Tools must be installed in
   the target virtual machine, and its network adapter settings
   should be configured to change automatically.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                         the virtual network adapter.
   sNewDefaultGateway :  The new default gateway address. The
                         specified string must be encoded using
                         UTF\-8 and must be terminated with the
                         binary zero character.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_GetDefaultGateway
   PrlVmDevNet_SetAutoApply                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_SetDefaultGateway, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewDefaultGateway
		) );


/* Obtains the default gateway (IPv6) assigned to a virtual network
   adapter in a virtual machine.
   Parameters
   hVmDev :                 A handle of type
                            PHT_VIRTUAL_DEV_NET_ADAPTER
                            identifying the virtual network
                            adapter.
   sDefaultGateway :        [out] A pointer to a buffer that
                            receives the default gateway address
                            (a UTF\-8 encoded, null\-terminated
                            string).
   pnDefaultGatewayBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this value to zero to
                            receive the required size. [out] The
                            required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetDefaultGateway                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmDevNet_GetDefaultGatewayIPv6, (
		PRL_HANDLE hVmDev,
		PRL_STR sDefaultGateway,
		PRL_UINT32_PTR pnDefaultGatewayBufLength
		) );

/* Allows to manually set the default gateway address (IPv6) for a
   network adapter in a virtual machine. <b>Note</b>: For these
   changes to take effect, Tools must be installed in
   the target virtual machine, and its network adapter settings
   should be configured to change automatically.

   If the changes are applied to a stopped virtual machine, they
   will take effect the next time the machine is started. If the
   changes are applied to a running virtual machine, they will
   take effect on configuration changes commit.
   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                         the virtual network adapter.
   sNewDefaultGateway :  The new default gateway address. The
                         specified string must be encoded using
                         UTF\-8 and must be terminated with the
                         binary zero character.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_GetDefaultGateway
   PrlVmDevNet_SetAutoApply                                 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmDevNet_SetDefaultGatewayIPv6, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewDefaultGateway
		) );


/* Obtains the virtual network ID assigned to a virtual network
   adapter in a virtual machine.
   Parameters
   hVmDev :                 A handle of type
                            PHT_VIRTUAL_DEV_NET_ADAPTER
                            identifying the virtual network
                            adapter.
   sVirtualNetworkId :      [out] A pointer to a buffer that
                            receives the virtual network ID
                            (a UTF\-8 encoded, null\-terminated
                            string).
   pnVirtNetIdBufLength :   [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this value to zero to
                            receive the required size. [out] The
                            required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetVirtualNetworkId                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_GetVirtualNetworkId, (
		PRL_HANDLE hVmDev,
		PRL_STR sVirtualNetworkId,
		PRL_UINT32_PTR pnVirtualNetworkIdBufLength
		) );

/* Allows to set the virtual network ID for a network adapter in
   a virtual machine.
   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                         the virtual network adapter.
   sNewVirtualNetworkId :  The new virtual network ID. The
                         specified string must be encoded using
                         UTF\-8 and must be terminated with the
                         binary zero character. Can be null or empty
						 in order to drop virtual network ID device
						 field.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_GetVirtualNetworkId                                   */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmDevNet_SetVirtualNetworkId, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewVirtualNetworkId
		) );



	/* Determines if the specified network adapter is configured
	   to filter net packages and send packages only from its own MAC address.

	   Parameters
	   hVmDev :               A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
				  identifying the virtual network
				  adapter.
	   pbPktFilterPreventMacSpoof :  [out] A pointer to a variable that
				  receives the result. PRL_TRUE indicates that adapter
				  is allowed to send packages only from its own MAC address.
				  PRL_FALSE indicates  otherwise.
	   Returns
	   PRL_RESULT. Possible values:

	   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
	   passed.

	   PRL_ERR_SUCCESS - function completed successfully.                  */
	PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
					 PrlVmDevNet_IsPktFilterPreventMacSpoof, (
			PRL_HANDLE hVmDev,
			PRL_BOOL_PTR pbPktFilterPreventMacSpoof
			) );

	/* Allows to specify whether a network adapter in a virtual
	   machine should filter packets through MAC address spoofing.

	   Parameters
	   hVmDev :              A handle of type
				 PHT_VIRTUAL_DEV_NET_ADAPTER identifying
				 the virtual network adapter.
	   bPktFilterPreventMacSpoof :  Specify PRL_TRUE to send packages
				 only from its own MAC address. Specify
				 PRL_FALSE to turn off filtration.

	   Returns
	   PRL_RESULT. Possible values:

	   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
	   passed.

	   PRL_ERR_SUCCESS - function completed successfully.            */
	PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
					 PrlVmDevNet_SetPktFilterPreventMacSpoof, (
			PRL_HANDLE hVmDev,
			PRL_BOOL bPktFilterPreventMacSpoof
			) );

	/* Determines if the specified network adapter is configured
	   to receive packets sent not to VM.

	   Parameters
	   hVmDev :               A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
				  identifying the virtual network
				  adapter.
	   pbPktFilterPreventPromisc :  [out] A pointer to a variable that
				  receives the result. PRL_TRUE indicates
				  that adapter is confured to prevent to
				  to receive packets sent not to VM.
				  PRL_FALSE indicates  otherwise.
	   Returns
	   PRL_RESULT. Possible values:

	   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
	   passed.

	   PRL_ERR_SUCCESS - function completed successfully.                  */
	PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
					  PrlVmDevNet_IsPktFilterPreventPromisc , (
			PRL_HANDLE hVmDev,
			PRL_BOOL_PTR pbPktFilterPreventPromisc
			) ) ;

	/* Allows to specify whether a network adapter in a virtual
	   machine should receive packets sent not to VM.

	   Parameters
	   hVmDev :              A handle of type
				 PHT_VIRTUAL_DEV_NET_ADAPTER identifying
				 the virtual network adapter.
	   bPktFilterPreventPromisc :  Specify PRL_TRUE to prevent VM to receive
				 packets that were sent not to VM. Specify
				 PRL_FALSE to switch off filtration.

	   Returns
	   PRL_RESULT. Possible values:

	   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
	   passed.

	   PRL_ERR_SUCCESS - function completed successfully.            */
	PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
					  PrlVmDevNet_SetPktFilterPreventPromisc, (
			PRL_HANDLE hVmDev,
			PRL_BOOL bPktFilterPreventPromisc
			) );

	/* Determines if the specified network adapter is configured
	   to filter net packages and send packages only from IP addresses in the
	   adapter NetAdrresses list.

	   Parameters
	   hVmDev :               A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
				  identifying the virtual network
				  adapter.
	   pbPktFilterPreventMacSpoof :  [out] A pointer to a variable that
				  receives the result. PRL_TRUE indicates that adapter
				  is allowed to send packages only from IP addresses in the
				  adapter NetAdrresses list.
				  PRL_FALSE indicates  otherwise.
	   Returns
	   PRL_RESULT. Possible values:

	   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
	   passed.

	   PRL_ERR_SUCCESS - function completed successfully.                  */
	PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
					  PrlVmDevNet_IsPktFilterPreventIpSpoof, (
			PRL_HANDLE hVmDev,
			PRL_BOOL_PTR pbPktFilterPreventIpSpoof
			) );

	/* Allows to specify whether a network adapter in a virtual
	   machine should perform the filtration by IP addresses.

	   Parameters
	   hVmDev :              A handle of type
				 PHT_VIRTUAL_DEV_NET_ADAPTER identifying
				 the virtual network adapter.
	   bPktFilterPreventMacSpoof :  Specify PRL_TRUE to send packages
				 only from addresses in the adapter NetAdrresses
				 list. Specify PRL_FALSE to switch off filtration.

	   Returns
	   PRL_RESULT. Possible values:

	   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
	   passed.

	   PRL_ERR_SUCCESS - function completed successfully.            */
	PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
					  PrlVmDevNet_SetPktFilterPreventIpSpoof, (
			PRL_HANDLE hVmDev,
			PRL_BOOL bPktFilterPreventIpSpoof
			) );

/* Obtain  the adapter type assigned to a virtual network adapter.

   Parameters
   hVmDev :                 A handle of type
                            PHT_VIRTUAL_DEV_NET_ADAPTER
                            identifying the virtual network
                            adapter.
   pnAdapterType :	    [out] A pointer to a buffer that
                            receives the adapter type
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_SetAdapterType                                  */

	PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
					  PrlVmDevNet_GetAdapterType, (
			PRL_HANDLE hVmDev,
			PRL_VM_NET_ADAPTER_TYPE_PTR pnAdapterType
			) );


/* Allows to manually assign adapter type to a network adapter.

   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_NET_ADAPTER identifying
                         the virtual network adapter.
   nAdapterType :	 The new network adapter type.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevNet_GetAdapterType                                   */

	PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
					  PrlVmDevNet_SetAdapterType, (
			PRL_HANDLE hVmDev,
			PRL_VM_NET_ADAPTER_TYPE nAdapterType
			) );


/* Gets state switched on or off of the adapter firewall.
   Parameters
   hVmDev    : A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER identifying
               the virtual network adapter.
   pbEnabled : [out] A pointer to a buffer that
               receives the adapter firewall state.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmDevNet_IsFirewallEnabled, (
		PRL_HANDLE hVmDev,
		PRL_BOOL_PTR pbEnabled
		) );


/* Sets state switched on or off of the adapter firewall.
   Parameters
   hVmDev    : A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER identifying
               the virtual network adapter.
   bEnabled  : The adapter firewall state.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmDevNet_SetFirewallEnabled, (
		PRL_HANDLE hVmDev,
		PRL_BOOL bEnabled
		) );


/* Gets an adapter firewall default policy. Policy values are defined
   in the PRL_FIREWALL_POLICY enumeration.
   Parameters
   hVmDev    : A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER identifying
               the virtual network adapter.
   nDirection: Direction of a firewall - to/from the virtual network adapter
   pnPolicy : [out] A pointer to a buffer that receives
              the adapter firewall default policy.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmDevNet_GetFirewallDefaultPolicy, (
		PRL_HANDLE hVmDev,
		PRL_FIREWALL_DIRECTION nDirection,
		PRL_FIREWALL_POLICY_PTR pnPolicy
		) );


/* Sets an adapter firewall default policy. Policy values are defined
   in the PRL_FIREWALL_POLICY enumeration.
   Parameters
   hVmDev    : A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER identifying
               the virtual network adapter.
   nDirection: Direction of a firewall - to/from the virtual network adapter
   nPolicy   : An adapter firewall default policy.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmDevNet_SetFirewallDefaultPolicy, (
		PRL_HANDLE hVmDev,
		PRL_FIREWALL_DIRECTION nDirection,
		PRL_FIREWALL_POLICY nPolicy
		) );


/* \Returns the firewall rule entries list.
   Parameters
   hVmDev :     A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
                identifying the virtual network adapter.
   nDirection : Direction of a firewall - to/from the virtual network adapter
   phRuleList : [out] A pointer to a variable that receives the
                new handle of PHT_HANDLES_LIST type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the list
   or a port forward handle object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmDevNet_GetFirewallRuleList, (
		PRL_HANDLE hVmDev,
		PRL_FIREWALL_DIRECTION nDirection,
		PRL_HANDLE_PTR phRuleList
		) );


/* \Sets the firewall rule entries list.
   Parameters
   hVmDev :    A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
               identifying the virtual network adapter.
   nDirection: Direction of a firewall - to/from the virtual network adapter
   hRuleList : The list of PHT_FIREWALL_RULE handles.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a port
   forward entry object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmDevNet_SetFirewallRuleList, (
		PRL_HANDLE hVmDev,
		PRL_FIREWALL_DIRECTION nDirection,
		PRL_HANDLE hRuleList
		) );

/* Obtains the custom host adapter name.
   Parameters
   hVmDev :                 A handle of type
                            PHT_VIRTUAL_DEV_NET_ADAPTER
                            identifying the virtual network
                            adapter.
   sHostInterfaceName :     [out] A pointer to a buffer that
                            receives the name
                            (a UTF\-8 encoded, null\-terminated
                            string).
   pnDefaultGatewayBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this value to zero to
                            receive the required size. [out] The
                            required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
		PrlVmDevNet_GetHostInterfaceName, (
			PRL_HANDLE hVmDev,
			PRL_STR sHostInterfaceName,
			PRL_UINT32_PTR pnHostInterfaceNameBufLength
			) );

/* Sets the custom host adapter name in a bridged networking mode,
   (autogenerated if not set)
   hVmDev :  A handle of type PHT_VIRTUAL_DEV_NET_ADAPTER
             identifying the virtual network adapter.
   sNewHostInterfaceName : Host interface name.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
		PrlVmDevNet_SetHostInterfaceName, (
			PRL_HANDLE hVmDev,
			PRL_CONST_STR sNewHostInterfaceName
			) );


///////////////////////////////////////////////////////////////////////////////
/// @section firewall rule operations
///////////////////////////////////////////////////////////////////////////////

/* Creates a new handle of type PHT_FIREWALL_RULE. It is the
   responsibility of the user to free the handle when it is no
   longer needed.
   Parameters
   phRule : [out] A pointer to a variable that receives the new handle.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a port
   forward handle object.

   PRL_ERR_SUCCESS - function completed successfully.               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_Create, (
		PRL_HANDLE_PTR phRule
		) );

/* \Returns the local port.
   Parameters
   hRule :   A handle of type PHT_FIREWALL_RULE
             identifying the firewall rule.
   pnPort :  [out] A pointer to a variable that receives
             the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_GetLocalPort, (
		PRL_HANDLE hRule,
		PRL_UINT32_PTR pnPort
		) );

/* Sets the specified local port.
   Parameters
   hRule :  A handle of type PHT_FIREWALL_RULE
            identifying the firewall rule.
   nPort :  The value to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_SetLocalPort, (
		PRL_HANDLE hRule,
		PRL_UINT32 nPort
		) );

/* \Returns the remote port.
   Parameters
   hRule :   A handle of type PHT_FIREWALL_RULE
             identifying the firewall rule.
   pnPort :  [out] A pointer to a variable that receives
             the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_GetRemotePort, (
		PRL_HANDLE hRule,
		PRL_UINT32_PTR pnPort
		) );

/* Sets the specified remote port.
   Parameters
   hRule :  A handle of type PHT_FIREWALL_RULE
            identifying the firewall rule.
   nPort :  The value to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_SetRemotePort, (
		PRL_HANDLE hRule,
		PRL_UINT32 nPort
		) );

/* \Returns the protocol of the specified firewall rule entry.
   Parameters
   hRule :       A handle of type PHT_FIREWALL_RULE
                 identifying the firewall rule.
   sProtocol :   [out] A pointer to a buffer that receives
                 the result (a UTF\-8 encoded,
                 null\-terminated string).
   pnBufLength : [in] The size of the output buffer (in
                 bytes). Set the buffer pointer to null
                 and this parameter's value to zero to
                 receive the required size. [out] The
                 required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_GetProtocol, (
		PRL_HANDLE hRule,
		PRL_STR sProtocol,
		PRL_UINT32_PTR pnBufLength
		) );

/* Sets the specified firewall rule entry protocol.
   Parameters
   hRule :     A handle of type PHT_FIREWALL_RULE
               identifying the firewall rule.
   sProtocol : The value to set. The value must be a a UTF\-8
               encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_SetProtocol, (
		PRL_HANDLE hRule,
		PRL_CONST_STR sProtocol
		) );

/* \Returns the local address of the specified firewall rule entry.
   Parameters
   hRule :       A handle of type PHT_FIREWALL_RULE
                 identifying the firewall rule.
   sAddr :       [out] A pointer to a buffer that receives
                 the result (a UTF\-8 encoded,
                 null\-terminated string).
   pnBufLength : [in] The size of the output buffer (in
                 bytes). Set the buffer pointer to null
                 and this parameter's value to zero to
                 receive the required size. [out] The
                 required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_GetLocalNetAddress, (
		PRL_HANDLE hRule,
		PRL_STR sAddr,
		PRL_UINT32_PTR pnBufLength
		) );

/* Sets the specified firewall rule entry local address.
   Parameters
   hRule : A handle of type PHT_FIREWALL_RULE
           identifying the firewall rule.
   sAddr : The value to set. The value must be a a UTF\-8
           encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_SetLocalNetAddress, (
		PRL_HANDLE hRule,
		PRL_CONST_STR sAddr
		) );

/* \Returns the remote address of the specified firewall rule entry.
   Parameters
   hRule :       A handle of type PHT_FIREWALL_RULE
                 identifying the firewall rule.
   sAddr :       [out] A pointer to a buffer that receives
                 the result (a UTF\-8 encoded,
                 null\-terminated string).
   pnBufLength : [in] The size of the output buffer (in
                 bytes). Set the buffer pointer to null
                 and this parameter's value to zero to
                 receive the required size. [out] The
                 required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_GetRemoteNetAddress, (
		PRL_HANDLE hRule,
		PRL_STR sAddr,
		PRL_UINT32_PTR pnBufLength
		) );

/* Sets the specified firewall rule entry remote address.
   Parameters
   hRule : A handle of type PHT_FIREWALL_RULE
           identifying the firewall rule.
   sAddr : The value to set. The value must be a a UTF\-8
           encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlFirewallRule_SetRemoteNetAddress, (
		PRL_HANDLE hRule,
		PRL_CONST_STR sAddr
		) );


/* %VM_ONLY%

   Obtains USB controller autoconnect device option.
   Parameters
   hVmDev :               A handle of type
                          PHT_VIRTUAL_DEV_USB_DEVICE identifying
                          the USB device.
   pnAutoconnectOption :  [out] A pointer to a variable that
                          receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevUsb_SetAutoconnectOption                              */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevUsb_GetAutoconnectOption, (
		PRL_HANDLE hVmDev,
		PRL_USB_DEVICE_AUTO_CONNECT_OPTION_PTR pnAutoconnectOption
		) );

/* %VM_ONLY%

   Sets USB controller autoconnect device option.
   Parameters
   hVmDev :              A handle of type
                         PHT_VIRTUAL_DEV_USB_DEVICE identifying
                         the USB device.
   nAutoconnectOption :  Autoconnect option to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevUsb_GetAutoconnectOption                             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevUsb_SetAutoconnectOption, (
		PRL_HANDLE hVmDev,
		PRL_USB_DEVICE_AUTO_CONNECT_OPTION nAutoconnectOption
		) );

/* %VM_ONLY%

   \Returns output device string for the specified sound device.
   Parameters
   hVmDev :                A handle of type PHT_VIRTUAL_DEV_SOUND
                           identifying the sound device.
   sOutputDev :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnOutputDevBufLength :  [in] The size of the output buffer (in
                           bytes). Set the buffer pointer to null
                           and this parameter's value to zero to
                           receive the required size. [out] The
                           required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevSound_SetOutputDev                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevSound_GetOutputDev, (
		PRL_HANDLE hVmDev,
		PRL_STR sOutputDev,
		PRL_UINT32_PTR pnOutputDevBufLength
		) );

/* Sets output device string for the specified sound device.
   Parameters
   hVmDev :         A handle of type PHT_VIRTUAL_DEV_SOUND
                    identifying the sound device.
   sNewOutputDev :  \Output device string to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevSound_GetOutputDev                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevSound_SetOutputDev, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewOutputDev
		) );

/* %VM_ONLY%

   Returns mixer device string for the specified sound device.
   Parameters
   hVmDev :               A handle of type PHT_VIRTUAL_DEV_SOUND
                          identifying the sound device.
   sMixerDev :            [out] A pointer to a buffer that
                          receives the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnMixerDevBufLength :  [in] The size of the output buffer (in
                          bytes). Set the buffer pointer to null
                          and this parameter's value to zero to
                          receive the required size. [out] The
                          required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevSound_SetMixerDev                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevSound_GetMixerDev, (
		PRL_HANDLE hVmDev,
		PRL_STR sMixerDev,
		PRL_UINT32_PTR pnMixerDevBufLength
		) );


/* %VM_ONLY%

   Sets mixer device string for the specified sound device.
   Parameters
   hVmDev :        A handle of type PHT_VIRTUAL_DEV_SOUND
                   identifying the sound device.
   sNewMixerDev :  Mixer device string to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevSound_GetMixerDev                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevSound_SetMixerDev, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sNewMixerDev
		) );

/* %VM_ONLY%

   \Returns the socket mode of a virtual serial port. Use this
   function to get the socket mode when the emulation type of
   the port is "socket". To get the port's emulation type, use
   the PrlVmDev_GetEmulatedType function.
   Parameters
   hVmDev :        A handle of type PHT_VIRTUAL_DEV_SERIAL_PORT
                   identifying the serial port.
   pnSocketMode :  [out] Pointer to a variable that receives the
                   socket mode.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevSerial_SetSocketMode                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevSerial_GetSocketMode, (
		PRL_HANDLE hVmDev,
		PRL_SERIAL_PORT_SOCKET_OPERATION_MODE_PTR pnSocketMode
		) );

/* Sets the socket mode for a virtual serial port. Use this
   function to set the socket mode when the emulation type of
   the port is "socket". To get the port's emulation type, use
   the PrlVmDev_GetEmulatedType function.
   Parameters
   hVmDev :       A handle of type PHT_VIRTUAL_DEV_SERIAL_PORT
                  identifying the serial port.
   nSocketMode :  The socket mode emulation type to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevSerial_GetSocketMode                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDevSerial_SetSocketMode, (
		PRL_HANDLE hVmDev,
		PRL_SERIAL_PORT_SOCKET_OPERATION_MODE nSocketMode
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section VM shared folders operations
///////////////////////////////////////////////////////////////////////////////

/* %VM_ONLY%

   Removes the share from the virtual machine configuration.
   Once a share is removed from the virtual machine
   configuration, the folder cannot be accessed from the guest
   OS. Just like any other virtual machine configuration
   modification, this operation must be performed within the
   PrlVm_BeginEdit / PrlVm_Commit block.
   Parameters
   hVmShare :  A handle of type PHT_SHARE identifying the shared
               folder.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_Remove, (
		PRL_HANDLE hVmShare
		) );

/* %VM_ONLY%

   \Returns the shared folder name (as it appears in the guest
   OS).
   Parameters
   hVmShare :              A handle of type PHT_SHARE identifying
                           the shared folder.
   sShareName :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnShareNameBufLength :  [in] The size of the output buffer (in
                           bytes). Set the buffer pointer to null
                           and this parameter's value to zero to
                           receive the required size. [out] The
                           required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_SetName                                               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_GetName, (
		PRL_HANDLE hVmShare,
		PRL_STR sShareName,
		PRL_UINT32_PTR pnShareNameBufLength
		) );

/* %VM_ONLY%

   Sets the share name (as it will appear in the guest OS).
   Parameters
   hVmShare :       A handle of type PHT_SHARE identifying the
                    shared folder.
   sNewShareName :  The user\-defined share name (must be a a
                    UTF\-8 encoded, null\-terminated string).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_GetName                                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_SetName, (
		PRL_HANDLE hVmShare,
		PRL_CONST_STR sNewShareName
		) );

/* %VM_ONLY%
 
   \Returns the shared folder path.
   Parameters
   hVmShare :              A handle of type PHT_SHARE identifying
                           the shared folder.
   sSharePath :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnSharePathBufLength :  [in] The size of the output buffer (in
                           bytes). Set the buffer pointer to null
                           and this parameter's value to zero to
                           receive the required size. [out] The
                           required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_SetPath                                               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_GetPath, (
		PRL_HANDLE hVmShare,
		PRL_STR sSharePath,
		PRL_UINT32_PTR pnSharePathBufLength
		) );

/* %VM_ONLY%

   Sets the shared folder path.
   Parameters
   hVmShare :       A handle of type PHT_SHARE identifying the
                    shared folder.
   sNewSharePath :  A string containing an absolute path to the
                    folder on the host that you would like to
                    share with the guest OS ( must be a UTF\-8
                    encoded, null\-terminated string).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_GetPath                                             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_SetPath, (
		PRL_HANDLE hVmShare,
		PRL_CONST_STR sNewSharePath
		) );

/* %VM_ONLY%

   Returns the shared folder description.
   Parameters
   hVmShare :                     A handle of type PHT_SHARE
                                  identifying the shared folder.
   sShareDescription :            [out] A pointer to a buffer
                                  that receives the result (a
                                  UTF\-8 encoded, null\-terminated
                                  string).
   pnShareDescriptionBufLength :  [in] The size of the output
                                  buffer (in bytes). Set the
                                  buffer pointer to null and this
                                  parameter's value to zero to
                                  receive the required size.
                                  [out] The required output
                                  buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_SetDescription                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_GetDescription, (
		PRL_HANDLE hVmShare,
		PRL_STR sShareDescription,
		PRL_UINT32_PTR pnShareDescriptionBufLength
		) );

/* %VM_ONLY%

   Sets the shared folder description.
   Parameters
   hVmShare :              A handle of type PHT_SHARE identifying
                           the shared folder.
   sNewShareDescription :  A user\-defined share description
                           (must be a UTF\-8 encoded,
                           null\-terminated string).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_GetDescription                                        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_SetDescription, (
		PRL_HANDLE hVmShare,
		PRL_CONST_STR sNewShareDescription
		) );

/* %VM_ONLY%

   Determines whether the share is enabled or not.
   Parameters
   hVmShare :   A handle of type PHT_SHARE identifying the shared
                folder.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the share is
                enabled. PRL_FALSE indicates that it is disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_SetEnabled                                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_IsEnabled, (
		PRL_HANDLE hVmShare,
		PRL_BOOL_PTR pbEnabled
		) );

/* %VM_ONLY%

   Enables the specified share.
   Parameters
   hVmShare :  A handle of type PHT_SHARE identifying the shared
               folder.
   bEnabled :  Specify PRL_TRUE to enable the share. Specified
               PRL_FALSE to disable it. If a share is disabled,
               it cannot be accessed from the guest OS.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_IsEnabled                                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_SetEnabled, (
		PRL_HANDLE hVmShare,
		PRL_BOOL bEnabled
		) );

/* %VM_ONLY%

   Determines if the share is read-only.
   Parameters
   hVmShare :    A handle of type PHT_SHARE identifying the
                 shared folder.
   pbReadOnly :  [out] A pointer to a variable that receives the
                 \result. PRL_TRUE indicates that the share is
                 read\-only (files can be read and copied but not
                 modified). PRL_FALSE indicates that the folder
                 contents can be modified.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_SetReadOnly                                           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_IsReadOnly, (
		PRL_HANDLE hVmShare,
		PRL_BOOL_PTR pbReadOnly
		) );

/* %VM_ONLY%

   Makes the specified shared folder read-only.
   Parameters
   hVmShare :   A handle of type PHT_SHARE identifying the shared
                folder.
   bReadOnly :  Specify PRL_TRUE to make the share read\-only.
                Specify PRL_FALSE to make the share editable.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlShare_IsReadOnly                                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlShare_SetReadOnly, (
		PRL_HANDLE hVmShare,
		PRL_BOOL bReadOnly
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section VM boot devices operations
///////////////////////////////////////////////////////////////////////////////

/* %VM_ONLY%

   Removes the boot device from the boot priority list. This
   function permanently removes a devices from the list. If you
   would like to temporarily disable a device, use the
   PrlBootDev_SetInUse function. To add a device back to the
   list, use PrlVmCfg_CreateBootDev and then call the necessary
   functions of the PHT_BOOT_DEVICE handle to set the necessary
   properties.
   Parameters
   hVmBootDev :  A handle of type PHT_BOOT_DEVICE identifying the
                 boot device.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or the device has been
   removed from the boot list.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlBootDev_Remove, (
		PRL_HANDLE hVmBootDev
		) );

/* %VM_ONLY%

   Returns the boot device type. Device type is a property
   that, together with device index, is used to uniquely
   identify a device in the virtual machine boot list. The
   supported device types are defined in the PRL_DEVICE_TYPE
   enumeration.
   Parameters
   hVmBootDev :  A handle of type PHT_BOOT_DEVICE identifying the
                 boot device.
   pnDevType :   [out] A pointer to a variable that receives the
                 device type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlBootDev_SetType                                             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlBootDev_GetType, (
		PRL_HANDLE hVmBootDev,
		PRL_DEVICE_TYPE_PTR pnDevType
		) );

/* %VM_ONLY%

   Sets the boot device type. Use this function when adding a
   device to the boot device priority list. Device type is a
   property that, together with device index, is used to
   uniquely identify a device in a virtual machine boot list.
   When adding a device to the boot priority list, these
   properties must be set correctly or the device will not be
   recognized. The supported device types are defined in the
   PRL_DEVICE_TYPE enumeration.
   Parameters
   hVmBootDev :  A handle of type PHT_BOOT_DEVICE identifying the
                 boot device.
   nDevType :    The device type to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or the device has been
   removed from the boot list.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlBootDev_GetType                                             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlBootDev_SetType, (
		PRL_HANDLE hVmBootDev,
		PRL_DEVICE_TYPE nDevType
		) );

/* %VM_ONLY%

   Obtains the boot device index. Device index is a property
   that, together with device type, is used to uniquely identify
   a device in the boot priority list of a virtual machine. The
   index should be the same index the device has in the main
   virtual machine configuration, which is determined using the
   PrlVmDev_GetIndex function.
   Parameters
   hVmBootDev :  A handle of type PHT_BOOT_DEVICE identifying the
                 boot device.
   pnDevIndex :  [out] A pointer to a variable that receives the
                 device index.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlBootDev_SetIndex                                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlBootDev_GetIndex, (
		PRL_HANDLE hVmBootDev,
		PRL_UINT32_PTR pnDevIndex
		) );

/* %VM_ONLY%

   Sets the boot device index. Device index is a property that,
   together with device type, is used to uniquely identify a
   device in the virtual machine boot priority list. The index
   must be the same index the device has in the main virtual
   machine configuration or it will not be recognized during the
   boot procedure. To obtain a virtual device index from the
   virtual machine configuration, use the PrlVmDev_GetIndex
   function.
   Parameters
   hVmBootDev :  A handle of type PHT_BOOT_DEVICE identifying the
                 boot device.
   nDevIndex :   The index of the device of interest as it exists
                 in the virtual machine configuration.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or the device has been
   removed from the boot list.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlBootDev_GetIndex                                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlBootDev_SetIndex, (
		PRL_HANDLE hVmBootDev,
		PRL_UINT32 nDevIndex
		) );

/* %VM_ONLY%

   Obtains the boot device sequence index. The boot device
   sequence index starts with 0. When a virtual machine is
   powered on, it will first try to boot from the boot device
   that has a sequence index of 0. If it cannot boot from the
   device, it will try to boot from the device with sequence
   index 1, and so forth.
   Parameters
   hVmBootDev :       A handle of type PHT_BOOT_DEVICE
                      identifying the boot device.
   pnSequenceIndex :  [out] A pointer to a variable that receives
                      the device sequence index.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlBootDev_SetSequenceIndex                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlBootDev_GetSequenceIndex, (
		PRL_HANDLE hVmBootDev,
		PRL_UINT32_PTR pnSequenceIndex
		) );

/* %VM_ONLY%

   Assigns a sequence index to a boot device in the boot
   priority list. The boot device sequence index starts with 0.
   When a virtual machine is powered on, it will first try to
   boot from the boot device that has a sequence index of 0. If
   it cannot boot from the device, it will try to boot from the
   device with sequence index 1, and so forth.

   If you are changing a sequence index for an existing device,
   this is the only function that you have to call. If you are
   adding a new boot device to the list, you will have to set
   all the other properties, including the device index, type,
   and enabled/disabled flag. Please note that each device in
   the boot priority list must have a unique sequence index, so
   if you are modifying a sequence index for an existing device
   or assigning an index to a new device, you will have to make
   sure that no two devices have the same index.
   Parameters
   hVmBootDev :      A handle of type PHT_BOOT_DEVICE identifying
                     the boot device.
   nSequenceIndex :  The boot sequence index to set for the
                     device. The value of 0 (zero) is the highest
                     boot priority and indicates the first device
                     to boot from. The value of 1 (one) indicates
                     the second device to boot from, and so
                     forth.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or the device has been
   removed from the boot list.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlBootDev_GetSequenceIndex                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlBootDev_SetSequenceIndex, (
		PRL_HANDLE hVmBootDev,
		PRL_UINT32 nSequenceIndex
		) );

/* %VM_ONLY%

   Determines whether the boot device is enabled or disabled. A
   boot device can be either enabled or disabled. If a device is
   disabled, it is ignored during the boot operation. To change
   the status of a device, use the PrlBootDev_SetInUse function.
   Parameters
   hVmBootDev :  A handle of type PHT_BOOT_DEVICE identifying the
                 boot device.
   pbInUse :     [out] Pointer to a variable that receives the
                 \result PRL_TRUE indicates that the device is
                 enabled. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlBootDev_SetInUse                                            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlBootDev_IsInUse, (
		PRL_HANDLE hVmBootDev,
		PRL_BOOL_PTR pbInUse
		) );

/* %VM_ONLY%

   Enables or disables the boot device. A boot device can be
   enabled or disabled without being removed from the boot
   device priority list. If the device is disabled, it is
   ignored during the boot operation. To obtain the current
   status of a device, use the PrlBootDev_IsInUse function. To
   permanently remove a device from the list, use the
   PrlBootDev_Remove function.
   Parameters
   hVmBootDev :  A handle of type PHT_BOOT_DEVICE identifying the
                 boot device.
   bInUse :      A boolean value specifying the action type.
                 PRL_TRUE enables a device, PRL_FALSE disables
                 it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or the device has been
   removed from the boot list.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlBootDev_IsInUse

   PrlBootDev_Remove                                              */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlBootDev_SetInUse, (
		PRL_HANDLE hVmBootDev,
		PRL_BOOL bInUse
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Tools Utility functions
///////////////////////////////////////////////////////////////////////////////

/* %VM_ONLY%

   Returns the Tools package state information.
   Parameters
   handle :         A handle of type PHT_VM_TOOLS_INFO.
   pVmToolsState :  [out] A pointer to a variable that receives
                    the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the Tools package state
   information is not available.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmToolsInfo_GetState, (
		PRL_HANDLE handle,
		PRL_VM_TOOLS_STATE_PTR pVmToolsState
		) );

/* %VM_ONLY%

   \Returns version information of the Tools package
   installed in a given virtual machine. Before calling this
   function, make sure that the Tools package is
   installed in the virtual machine by calling the
   PrlVmToolsInfo_GetState function.
   Parameters
   handle :           A handle of type PHT_VM_TOOLS_INFO.
   sName :            [out] A pointer to a buffer that receives
                      the version information (a UTF\-8 encoded,
                      null\-terminated string).
   pnNameBufLength :  [in] The size of the output buffer (in
                      bytes). Set the buffer pointer to null and
                      this parameter's value to zero to receive
                      the required size. [out] The required
                      \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_NO_DATA - the Tools package version
   information is not available.

   PRL_ERR_SUCCESS - function completed successfully.            */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmToolsInfo_GetVersion, (
				PRL_HANDLE handle,
				PRL_STR sName,
				PRL_UINT32_PTR pnNameBufLength
				) );


///////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the virtual machine info processing
///////////////////////////////////////////////////////////////////////////////

/* \Returns the virtual machine state information.
   Parameters
   handle :    A handle of type PHT_VM_INFO.
   pVmState :  [out] A pointer to a variable that receives the
               virtual machine state information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine state information is
   not available.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmInfo_GetState, (
		PRL_HANDLE handle,
		VIRTUAL_MACHINE_STATE_PTR pVmState
		) );

/* \Returns the virtual machine addition state information.
Parameters
handle :    A handle of type PHT_VM_INFO.
pVmState :  [out] A pointer to a variable that receives the
virtual machine addition state information.
Returns
PRL_RESULT. Possible values:

PRL_ERR_INVALID_ARG - invalid handle or null pointer was
passed.

PRL_ERR_NO_DATA - the virtual machine state information is
not available.

PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				PrlVmInfo_GetAdditionState, (
				PRL_HANDLE handle,
				VIRTUAL_MACHINE_ADDITION_STATE_PTR pVmState
				) );

/* %VM_ONLY%

   Obtains a handle of type PHT_ACCESS_RIGHTS.
   Parameters
   hVmInfo :  A handle of type PHT_VM_INFO.
   phVmAcl :  [out] A pointer to a variable that receives the
              handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create PHT_ACCESS_RIGHTS
   object.

   PRL_ERR_NO_DATA - the virtual machine access rights
   information is not available.

   PRL_ERR_SUCCESS - function completed successfully.                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmInfo_GetAccessRights, (
		PRL_HANDLE hVmInfo,
		PRL_HANDLE_PTR phVmAcl
		) );


/* Determines if the specified virtual machine is invalid. A
   virtual machine can be reported as invalid by the function if
   its configuration has been corrupted, it's been removed from
   the virtual machine, and for other similar reasons.
   Parameters
   handle :        A handle of type PHT_VM_INFO.
   pVmIsInvalid :  [out] A pointer to a variable that receives
                   \result. PRL_TRUE \- the virtual machine is
                   invalid. PRL_FALSE \- the virtual machine is
                   fully operational.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the status of the virtual machine cannot be
   determined.

   RL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmInfo_IsInvalid, (
				PRL_HANDLE handle,
				PRL_BOOL_PTR pbIsInvalid
				) );


/* Determines if the specified virtual machine is invalid. A
   virtual machine can be reported as invalid by the function if
   its configuration has been corrupted, it's been removed from
   the virtual machine, and for other similar reasons.
   Parameters
   handle :        A handle of type PHT_VM_INFO.
   pVmIsInvalid :  [out] A pointer to a variable that receives
                   \result. PRL_TRUE \- the virtual machine is
                   invalid. PRL_FALSE \- the virtual machine is
                   fully operational.

   pVmInvalidReason:  [out] A pointer to a variable that receives
					reason of invalidity as error code.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the status of the virtual machine cannot be
   determined.

   RL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmInfo_IsInvalidEx, (
				PRL_HANDLE handle,
				PRL_BOOL_PTR pbIsInvalid,
				PRL_RESULT_PTR pnInvalidReason
				) );

/* Determines if the specified virtual machine is waiting for answer
   on self question.
   Parameters
   handle :            A handle of type PHT_VM_INFO.
   pbIsVmWaitingForAnswer :  [out] A pointer to a variable that receives
                       \result. PRL_TRUE \- the virtual machine is
                       waiting for answer. PRL_FALSE \- the virtual
					   machine is working normal.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the status of the virtual machine cannot be
   determined.

   RL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmInfo_IsVmWaitingForAnswer, (
				 PRL_HANDLE handle,
				 PRL_BOOL_PTR pbIsVmWaitingForAnswer
				) );


/* Determines whether a VNC server is running for the specified
   virtual machine.
   Parameters
   handle :                A handle of type PHT_VM_INFO.
   pbIsVncServerStarted :  [out] A pointer to a variable that
                           receives the result. PRL_TRUE
                           indicates that a VNC server is
                           running. PRL_FALSE indicates
                           otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the VNC server state information is not
   available.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVm_StartVncServer

   PrlVm_StopVncServer                                          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmInfo_IsVncServerStarted, (
		PRL_HANDLE handle,
		PRL_BOOL_PTR pbIsVncServerStarted
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for search virtual machines info processing
///////////////////////////////////////////////////////////////////////////////

/* Extracts the virtual machine name form the container.
   Parameters
   handle :           A handle of type PHT_FOUND_VM_INFO
                      identifying the container.
   sName :            [out] A pointer to a buffer that receives
                      the name. Pass a null pointer to determine
                      the required buffer size.
   pnNameBufLength :  [in] The size of the buffer used to receive
                      the output data (in bytes). [out] The
                      required buffer size if the buffer
                      parameter contains a null pointer or if the
                      specified buffer size is not large enough.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter used to specify the size will
   contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlFoundVmInfo_GetName, (
				PRL_HANDLE handle,
				PRL_STR sName,
				PRL_UINT32_PTR pnNameBufLength
				) );

/* Determines if the virtual machine has an old configuration.
   Parameters
   handle :        A handle of type PHT_FOUND_VM_INFO identifying
                   the container.
   pbOldVersion :  [out] A pointer to a variable that receives
                   the result. A value of "true" indicates that
                   the machine has an old configuration.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlFoundVmInfo_IsOldConfig, (
				PRL_HANDLE handle,
				PRL_BOOL_PTR pbOldVersion
				) );

/* Extracts the virtual machine's OS version from the container.
   Parameters
   handle :       A handle of type PHT_FOUND_VM_INFO identifying
                  the container.
   pnOSVersion :  [out] A pointer to a variable that receives the
                  OS version.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlFoundVmInfo_GetOSVersion, (
				PRL_HANDLE handle,
				PRL_UINT32_PTR pnOSVersion
				) );

/* Extracts the virtual machine configuration path from the
   container.
   Parameters
   handle :           A handle of type PHT_FOUND_VM_INFO
                      identifying the container.
   sPath :            [out] A pointer to a buffer that receives
                      the path. Pass a null pointer to determine
                      the required buffer size.
   pnPathBufLength :  [in] The size of the buffer used to receive
                      the output data (in bytes). [out] The
                      required buffer size if the buffer
                      parameter contains a null pointer or if the
                      specified buffer size is not large enough.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter used to specify the size will
   contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlFoundVmInfo_GetConfigPath, (
				PRL_HANDLE handle,
				PRL_STR sPath,
				PRL_UINT32_PTR pnPathBufLength
				) );

/* Determines of the specified virtual machine is a template.
   Parameters
   handle :        A handle of type PHT_FOUND_VM_INFO identifying
                   the container.
   pbIsTemplate :  [out] A pointer to a variable that receives
                   the result. PRL_TRUE indicates that the
                   virtual machine is a template. PRL_TRUE
                   indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlFoundVmInfo_IsTemplate, (
				PRL_HANDLE handle,
				PRL_BOOL_PTR pbIsTemplate
				) );


///////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the access rights list info processing
///////////////////////////////////////////////////////////////////////////////

/* %VM_ONLY%

   Determines if the current user is authorized to perform a
   specified task on a virtual machine.
   Parameters
   hAcl :         A handle of type PHT_ACCESS_RIGHTS containing
                  the access rights information for a virtual
                  machine.
   nPermission :  Task type.
   pbAllowed :    [out] A pointer to a variable that receives the
                  \result. PRL_TRUE indicates that the user is
                  allowed to perform the task. PRL_FALSE
                  indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlAcl_IsAllowed, (
		PRL_HANDLE hAcl,
		PRL_ALLOWED_VM_COMMAND nPermission,
		PRL_BOOL_PTR pbAllowed
		) );


/* %VM_ONLY%

   Obtains the virtual machine access rights information. The
   function determines whether other users besides the owner can
   view, run, or have full access to a virtual machine.
   Parameters
   hAcl :               A handle of type PHT_ACCESS_RIGHTS
                        containing the access rights information
                        for a virtual machine.
   pnAccessForOthers :  [out] A pointer to a variable that
                        receives the virtual machine access level
                        information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlAcl_SetAccessForOthers                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlAcl_GetAccessForOthers, (
		PRL_HANDLE hAcl,
		PRL_VM_ACCESS_FOR_OTHERS_PTR pnAccessForOthers
		) );


/* %VM_ONLY%

   Sets access rights on a virtual machine. The owner of a
   virtual machine can use this function to grant or deny access
   rights for the virtual machine to other users. The access
   rights will be applied to all users. The call will only
   update the local copy of the PHT_ACCESS_RIGHTS object passed
   to the function (you will have to properly obtain the object
   prior to calling this function). To apply the changes to the
   virtual machine, call the PrlVm_UpdateSecurity function after
   making this calls and pass the same PHT_ACCESS_RIGHTS handle
   to it.
   Parameters
   hAcl :              A handle of type PHT_ACCESS_RIGHTS
                       containing the access rights information
                       for a virtual machine.
   nAccessForOthers :  The access rights to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlAcl_GetAccessForOthers                                     */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlAcl_SetAccessForOthers, (
		PRL_HANDLE hAcl,
		PRL_VM_ACCESS_FOR_OTHERS nAccessForOthers
		) );


/* %VM_ONLY%

   Determines the virtual machine owner name.
   Parameters
   hAcl :                  A handle of type PHT_ACCESS_RIGHTS
                           containing the access rights
                           information for a virtual machine.
   sOwnerNameBuf :         [out] A pointer to a buffer that
                           receives the virtual machine owner
                           name (a UTF\-8 encoded,
                           null\-terminated string).
   pnOwnerNameBufLength :  [in] The size of the output buffer (in
                           bytes). Set the buffer pointer to null
                           and this parameter's value to zero to
                           receive the required size. [out] The
                           required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlAcl_IsCurrentSessionOwner                                   */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlAcl_GetOwnerName, (
				PRL_HANDLE hAcl,
				PRL_STR sOwnerNameBuf,
				PRL_UINT32_PTR pnOwnerNameBufLength
				) );


/* %VM_ONLY%

   Determines if the current user is the owner of the specified
   virtual machine.
   Parameters
   hAcl :       A handle of type PHT_ACCESS_RIGHTS containing the
                access rights information for a virtual machine.
   pbIsOwner :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the user is the
                owner of the virtual machine. PRL_FALSE indicates
                otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlAcl_IsCurrentSessionOwner, (
		PRL_HANDLE hAcl,
		PRL_BOOL_PTR pbIsOwner
		) );


/////////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the appliance configuration object.
/////////////////////////////////////////////////////////////////////////////////


/* %VM_ONLY%

   Creates a new handle of type PHT_APPLIANCE_CONFIG. The handle
   is necessary to create and configure an appliance download configuration.
   If you would like to create a new appliance download configuration,
   use this function to create the handle first and then use the handle
   functions to configure the appliance info.

   It is the responsibility of the caller to free the handle
   with the PrlHandle_Free call when it is no longer needed.
   Parameters
   phAppCfg :  [out] A pointer to a variable that receives the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a virtual
   network object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlAppliance_Create, (
		PRL_HANDLE_PTR phAppCfg
		) );


/////////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the Containers Templates.
/////////////////////////////////////////////////////////////////////////////////


/* %CT_ONLY%

   Returns name of the Container's template.
   Parameters
   hTemplate :            A handle of type PHT_CT_TEMPLATE.
   sName      :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnNameBufLength:       [in] The size of the output buffer (in
                          bytes). Set the buffer pointer to null
                          and this parameter's value to zero to
                          receive the required size. [out] The
                          required output buffer size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlCtTemplate_GetName, (
		PRL_HANDLE hTemplate,
		PRL_STR sName,
		PRL_UINT32_PTR pnNameLength
		) );

/* %CT_ONLY%

   Returns the type of the specified template.
   Parameters
   hTemplate :   A handle of type PHT_CT_TEMPLATE.
   pnType :      [out] A pointer to a variable that receives the
                 type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlCtTemplate_GetType, (
		PRL_HANDLE hTemplate,
		PRL_CT_TEMPLATE_TYPE_PTR pType
		) );

/* %CT_ONLY%

   Returns description of the Container's template.
   Parameters
   hTemplate :             A handle of type PHT_CT_TEMPLATE.
   sDescription:           [out] A pointer to a buffer that receives
                           the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnDescriptionBufLength: [in] The size of the output buffer (in
                           bytes). Set the buffer pointer to null
                           and this parameter's value to zero to
                           receive the required size. [out] The
                           required output buffer size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlCtTemplate_GetDescription, (
		PRL_HANDLE hTemplate,
		PRL_STR sDescription,
		PRL_UINT32_PTR pnDescriptionLength
		) );

/* %CT_ONLY%

   Returns version of the Container's template.
   Parameters
   hTemplate :             A handle of type PHT_CT_TEMPLATE.
   sVersion:               [out] A pointer to a buffer that receives
                           the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnVersionBufLength:     [in] The size of the output buffer (in
                           bytes). Set the buffer pointer to null
                           and this parameter's value to zero to
                           receive the required size. [out] The
                           required output buffer size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlCtTemplate_GetVersion, (
		PRL_HANDLE hTemplate,
		PRL_STR sVersion,
		PRL_UINT32_PTR pnVersionLength
		) );

/* %CT_ONLY%

   Returns the type of the operating system for which the specified
   template is. The OS types are defined in the PrlOses.h file as
   macros. Look for the macros that are prefixed with
   PVS_GUEST_TYPE_ (e.g. PVS_GUEST_TYPE_LINUX).
   Parameters
   hTemplate :   A handle of type PHT_CT_TEMPLATE.
   pnOsType :    [out] A pointer to a variable that receives the
                 OS type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the OS information is not present in
   the template configuration.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlCtTemplate_GetOsType, (
		PRL_HANDLE hTemplate,
		PRL_UINT32_PTR pnOsType
		) );

/* %CT_ONLY%

   Returns the version of the operating system for which the
   specified template is running. The OS versions are
   defined in the PrlOses.h file as macros. Look for the macros
   that are prefixed with PVS_GUEST_VER_ (e.g.
   PVS_GUEST_VER_LIN_REDHAT).
   Parameters
   hTemplate :      A handle of type PHT_CT_TEMPLATE.
   pnOsVersion :    [out] A pointer to a variable that receives
                    the OS version.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the OS information is not present in
   the template configuration.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlCtTemplate_GetOsVersion, (
		PRL_HANDLE hTemplate,
		PRL_UINT32_PTR pnOsVersion
		) );

/* %CT_ONLY%

   Returns the version of the operating system template for which the
   specified application template is.
   Parameters
   hTemplate :             A handle of type PHT_CT_TEMPLATE.
   sOsTemplate:            [out] A pointer to a buffer that receives
                           the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnOsTemplateLength:     [in] The size of the output buffer (in
                           bytes). Set the buffer pointer to null
                           and this parameter's value to zero to
                           receive the required size. [out] The
                           required output buffer size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlCtTemplate_GetOsTemplate, (
		PRL_HANDLE hTemplate,
		PRL_STR sOsTemplate,
		PRL_UINT32_PTR pnOsTemplateLength
		) );

/* %CT_ONLY%

   Determines the specified template CPU mode (32 bit or
   64 bit).
   Parameters
   hTemplate:     A handle of type PHT_CT_TEMPLATE.
   pnCpuMode :    [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the template object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlCtTemplate_GetCpuMode, (
		PRL_HANDLE hTemplate,
		PRL_CPU_MODE_PTR pnCpuMode
		) );

/* %CT_ONLY%

   Determines whether the template cached.
   Makes sense only for operating system templates.
   Parameters
   hTemplate:     A handle of type PHT_CT_TEMPLATE.
   pbCached :     [out] A pointer to a variable that
                        receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the template object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlCtTemplate_IsCached, (
		PRL_HANDLE hTemplate,
		PRL_BOOL_PTR pbCached
		) );


/* %VM_ONLY%

   Captures a screen area of a remote virtual machine desktop in
   PNG format, scales is down to the specified size, and puts it
   into a buffer which is then returned to the client. For local
   connections, the capture works almost instantly. For remote
   connections, the data must be transferred over network to the
   client machine, so it may take some time.

   Parameters
   hVm :          A handle of type PHT_VIRTUAL_MACHINE identifying
                  the virtual machine.
   nWidth :       The width of the scaled down image, in pixels.
   nHeight :      The height of the scaled down image, in pixels.
   nFlags  :      Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlVm_CaptureScreen, (
		PRL_HANDLE hVm,
		PRL_UINT32 nWidth,
		PRL_UINT32 nHeight,
		PRL_UINT32 nFlags
		) );

/* %VM_ONLY%

   Allows to set the on-crash action for the specified
   virtual machine
   Parameters
   hVmCfg :  A handle of type PHT_VM_CONFIGURATION
		identifying the virtual machine configuration.
   nAction:  The on-crash action to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully. 		*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			PrlVmCfg_SetActionOnGuestCrash, (
		PRL_HANDLE hVmCfg,
		PRL_VM_ON_CRASH_ACTION nAction
		) );

/* %VM_ONLY%

   Determines the on-crash action for the specified
   virtual machine
   Parameters
   hHandle :  A handle of type PHT_VM_CONFIGURATION
              identifying the virtual machine configuration.
   pnAction:  [out] A pointer to a variable that receives
              the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
								*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			PrlVmCfg_GetActionOnGuestCrash, (
		PRL_HANDLE hVmCfg,
		PRL_VM_ON_CRASH_ACTION_PTR pnAction
		) );

/* %VM_ONLY%

   Allows to set the on-crash options for the specified
   virtual machine
   Parameters
   hVmCfg :  A handle of type PHT_VM_CONFIGURATION
             identifying the virtual machine configuration.
   nOptions: Additional options to set (PRL_VM_ON_CRASH_OPTION)
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
								*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			PrlVmCfg_SetOptionsOnGuestCrash, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nOptions
		) );

/* %VM_ONLY%

   Determines the on-crash options for the specified
   virtual machine
   Parameters
   hHandle :  A handle of type PHT_VM_CONFIGURATION
              identifying the virtual machine configuration.
   pnOptions: [out] A pointer to a variable that recieves
              additional options.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
								*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
			PrlVmCfg_GetOptionsOnGuestCrash, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnOptions
		) );

/* Determines the number of CPU sockets in the virtual machine.
   Parameters
   hVmCfg :        A handle of type PHT_VM_CONFIGURATION
                   identifying the virtual machine configuration.
   pnVmCpuSocketsCount :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the virtual machine object does not contain
   the necessary data.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				 PrlVmCfg_GetCpuSocketsCount, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnVmCpuSocketsCount
		) );

/* Sets the number of CPU sockets in the specified virtual machine.
   (the CPUs should be present in the machine).
   Parameters
   hVmCfg :       A handle of type PHT_VM_CONFIGURATION
                  identifying the virtual machine configuration.
   nVmCpuSocketsCount :  The number to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_7,
				 PrlVmCfg_SetCpuSocketsCount, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nVmCpuSocketsCount
		) );

/* Obtains the backup directory path of the specified virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine
                        configuration.
   sVmBackupDirectory:  [out] A pointer to a buffer that receives
                        the result (a UTF\-8 encoded,
                        null\-terminated string).
   pnVmBackupDirectoryBufLength:  [in] The size of the output buffer
                        (in bytes). Set the buffer pointer to null
                        and this parameter's value to zero to
                        receive the required size. [out] The
                        required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The pnVmNameBufLength parameter will contain
   the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetDefaultBackupDirectory, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmBackupDirectory,
		PRL_UINT32_PTR pnVmBackupDirectoryBufLength
		) );

/* Sets the specified virtual machine backup directory path.
   Parameters
   hVmCfg :      A handle of type PHT_VM_CONFIGURATION
                 identifying the virtual machine configuration.
   sVmBackupDirectory:  The backup directory path to set.
                 The value must be a a UTF\-8
                 encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetDefaultBackupDirectory, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmBackupDirectory
		) );

#ifdef _WIN_
    #pragma pack(pop, save_api_pack)
#endif

#ifdef __cplusplus
}
#endif

#endif // __VIRTUOZZO_API_VM_H__
