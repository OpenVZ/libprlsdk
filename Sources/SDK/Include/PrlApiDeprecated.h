/*
 * PrlApiDeprecated.h: Methods declared in this file are deprecated.
 * If it possible do not use it in your code.
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

#if !defined(__VIRTUOZZO_API_DEPRECATED_H__)
#define __VIRTUOZZO_API_DEPRECATED_H__

#include "PrlIOStructs.h"

/**
 * Client can use this method to set user profile XML representation to user profile SDK object
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_USER_PROFILE handle or non valid (NULL) pointer to user profile XML
 * representation string was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlUsrCfg_FromString, (
		PRL_HANDLE hUsrCfg,
		PRL_CONST_STR sUsrCfg
		) );


/**
 * Client can use this method to get user profile XML representation string from user profile SDK object
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_DISP_CONFIG handle or non valid (NULL) pointer to storing result was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlUsrCfg_ToString, (
		PRL_HANDLE hUsrCfg,
		PRL_VOID_PTR_PTR pUsrCfg
		) );

/**
 * Add information about proxy and credentials to access to the specifid
 * proxy server.
 * Parameters
 *  hUserProfile :        A handle of type PHT_USER_PROFILE
 *                        identifying the user.
 *  sProxyHost :          proxy IP or host name (a UTF\-8 encoded,
 *                        null\-terminated string).
 *  nProxyPort :          proxy port
 *  sProxyUserLogin :     username to authenticate on proxy server
 *                        (a UTF\-8 encoded, null\-terminated string).
 *  sProxyUserPasswd :    password to authenticate on proxy server
 *                        (a UTF\-8 encoded, null\-terminated string).
 *  nReserved :           Reserved for future use. Should be 0.
 * Returns
 * PRL_RESULT. Possible values:
 *
 * PRL_ERR_INVALID_ARG - invalid handle or null pointer was
 * passed.
 *
 * PRL_ERR_SUCCESS - function completed successfully.
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				PrlUsrCfg_AddProxy, (
		PRL_HANDLE hUserProfile,
		PRL_CONST_STR sProxyHost,
		PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin,
		PRL_CONST_STR sProxyUserPasswd,
		PRL_INT32 nReserved
		) );

/**
 * Client can use this method to set dispatcher configuration to dispatcher config handle
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_DISP_CONFIG handle or non valid (NULL) pointer to disp config XML
 * representation string was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispCfg_FromString, (
		PRL_HANDLE hDispCfg,
		PRL_CONST_STR sDispCfg
		) );


/**
 * Client can use this method to get dispatcher configuration from disp config handle as XML string
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_DISP_CONFIG handle or non valid (NULL) pointer to storing result was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispCfg_ToString, (
		PRL_HANDLE hDispCfg,
		PRL_VOID_PTR_PTR pDispCfg
		) );

/**
 * Client can use this method to set VM configuration to VM handle
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_VIRTUAL_MACHINE handle or non valid (NULL) pointer to VM XML
 * representation string was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_FromString, (
		PRL_HANDLE hVm,
		PRL_CONST_STR vm_config
		) );


/**
 * Client can use this method to get VM configuration from VM handle as XML string
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_VIRTUAL_MACHINE handle or non valid (NULL) pointer to storing result was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_ToString, (
		PRL_HANDLE hVm,
		PRL_VOID_PTR_PTR vm_config
		) );

/**
 * This method initializing VM device from XML string.
 * This is temporary method that must be removed at Release stage.
 *
 * @param valid PRL_HANDLE describing the VM device
 * @param XML string representation of VM device
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid virtual device handle or non valid (NULL) pointer to specifying device string
 * representation or non valid virtual device XML representation
 * or handle of already removed VM device was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_FromString, (
		PRL_HANDLE hVmDev,
		PRL_CONST_STR sXml
		) );


/**
 * This method returns XML string representation of VM device.
 * This is temporary method that must be removed at Release stage.
 *
 * @param valid PRL_HANDLE describing the VM device
 * @param XML string representation of VM device
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid virtual device handle or non valid (NULL) pointer to storing result
 * or handle of already removed VM device was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmDev_ToString, (
		PRL_HANDLE hVmDev,
		PRL_VOID_PTR_PTR sXml
		) );


/**
* Gets VM device information from server .
*
* @param valid PRL_HANDLE describing the VM device
* @param pointer to storing result
* @return PRL_RESULT. Possible values:
* PRL_ERR_INVALID_ARG				- non valid virtual device handle
* PRL_ERR_UNINITIALIZED				- unexpected error: uninitialized with server connection or not bint to VM
* virtual device object
* PRL_ERR_SUCCESS						- operation successfully completed
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlVmDev_UpdateInfo, (
				PRL_HANDLE hServer,
				PRL_HANDLE hVmInputDev
				) );

/**
 * Get param token by index
 *
 * @param valid PRL_HANDLE
 * @param index of parameter
 * @param pointer to a command specific object that contains parameter value
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_RESULT handle or non valid (NULL) pointer for storing result or non valid
 * index was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlResult_GetParamToken, (
				 PRL_HANDLE hResult,
				 PRL_UINT32 index,
				 PRL_VOID_PTR_PTR data_ptr
				 ) );


/**
 * Get error description. Returns XML representation of error which must be parsed by CVmEvent instance.
 * String buffer allocating by strdup() so method caller must call free() for received result after it usage.
 * This method must be eliminated in Release stage.
 *
 * @param valid PRL_HANDLE describing result
 * @param pointer for getting result
 * @return PRL_ERR_SUCCESS if the method succeeds
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlResult_GetError, (
				 PRL_HANDLE hResult,
				 PRL_VOID_PTR_PTR data_ptr
				 ) );


/**
 * Client can use this method to set license XML representation to license SDK object
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE
 * @param XML license string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_LICENSE handle or non valid (NULL) pointer to license XML
 * representation string was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlLic_FromString, (
		PRL_HANDLE hLicense,
		PRL_CONST_STR sLic
		) );

/**
 * Client can use this method to get license XML representation string from License SDK object
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE
 * @param XML license representation  string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_LICENSE handle or non valid (NULL) pointer to storing result was passed
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlLic_ToString, (
					 PRL_HANDLE hLicense,
					 PRL_VOID_PTR_PTR pLicStr
					 ) );


/**
 * Creates a new handle of type PHT_PHT_LICENSE.
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
				 PrlLic_Create, (
					 PRL_HANDLE_PTR phLic
					 ) );

/**
* Client can use this method to get LoginResponse XML representation string from LoginResponse SDK object
* This method must be eliminated at Release stage
*
* @param valid PRL_HANDLE
* @param XML LoginResponse representation  string
* @return PRL_RESULT. Possible values:
* PRL_ERR_INVALID_ARG				- non valid PHT_LOGIN_RESPONSE handle or non valid (NULL) pointer to storing result was passed
* PRL_ERR_SUCCESS						- operation successfully completed
*/

PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlLoginResponse_ToString, (
	PRL_HANDLE hLoginResp,
	PRL_VOID_PTR_PTR pLoginRespStr
	) );



/**
* This is no longer supported. Use PrlDevDisplay_IsSlidingMouseEnabled()
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						   PrlVm_GetVirtDevInfo, (
		PRL_HANDLE hVm,
		PRL_UINT32 nFlags
		) );

/**
* This is no longer supported. Use PrlDevDisplay_IsSlidingMouseEnabled()
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmVirtDevInfo_GetUsbMouseState, (
		PRL_HANDLE handle,
		PRL_VOID_PTR pVmUsbMouseState
		) );


/**
Update any tools section in VM config

Parameters
handle : valid PRL_HANDLE describing the virtual machine

Returns
PRL_HANDLE to job that describing async operation result (PRL_INVALID_HANDLE means not enough memory to
instantiate async job represents object handle). Operation return code can be extracted with @ref PrlJob_GetRetCode()
call. Possible values:
* @param valid PRL_HANDLE
* @param XML subsection of Tools section in VmConfiguration
PRL_ERR_INVALID_ARG - non valid PHT_VIRTUAL_MACHINE handle was passed
PRL_ERR_UNINITIALIZED - unexpected error: uninitialized with server connection or non identified VM object
*/
PRL_ASYNC_SRV_METHOD_DECL(  VIRTUOZZO_API_VER_2,
							PrlVm_UpdateToolsSection, (
					  PRL_HANDLE hVm,
					  PRL_CONST_STR sToolsSection
					  ) );


/**
 * Client can use this method to set response XML representation (@see CProtoCommandDspWsResponse) to result handle
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE of PHT_RESULT type
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_RESULT handle or non valid (NULL) pointer to XML representation string (or XML representation
 *									  in wrong format - expecting correct CVmEvent representation contains CProtoCommandDspWsResponse proto command
 *									  class data) was passed
 * PRL_ERR_SUCCESS					- operation successfully completed
 */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlResult_FromString, (
		PRL_HANDLE hResult,
		PRL_CONST_STR sResult
		) );

/**
Sets Virtual Machine CPU VTx support enabling sign.

Parameters
handle : valid PRL_HANDLE describing the virtual machine
unnamed_param : setting CPU VTx support enabling sign

Returns
PRL_RESULT. Possible values:
PRL_ERR_INVALID_ARG - non valid PHT_VIRTUAL_MACHINE handle was specified
PRL_ERR_SUCCESS - operation successfully completed
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_SetCpuVtxEnabled, (
		PRL_HANDLE hVm,
		PRL_BOOL bVmCpuVtxEnabled
		) );


/**
@brief	(PrlApi deprecated) command that intended for
		sending any tools information from client to host.

		All information should be stored in the buffer.
		This procedure just transferring the buffer no matter
		what kind of information is in it.

Parameters
@param [in] hVm		- valid PRL_HANDLE of PHT_VIRTUAL_MACHINE type - handle of VM where the command must be sent
@param [in] command	- number that identifies protocol (sender-receiver pair)
@param [in] data	- pointer to the transferring data buffer
@param [in] dataSize- size of the transferring data

Returns
PRL_RESULT.

Possible values:
PRL_ERR_INVALID_ARG - non valid VM device
PRL_INVALID_HANDLE - invalid handle
PRL_UNINITIALIZED
PRL_ERR_FAILURE
or PRL_ERR_SUCCESS if success.
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVm_SendToolsGeneralCommand, (
	PRL_HANDLE			hVm,
	PRL_UINT32			command,
	PRL_CONST_VOID_PTR	data,
	PRL_UINT32			dataSize
	)) ;


/**
Performs begin start hard disks conversions to the server side. it can be catched with async result
processing callback (@see PrlSrv_RegEventHandler).

Parameters
handle : valid PRL_HANDLE of PHT_SERVER type
handle : valid PRL_HANDLE of PHT_STRINGS_LIST type with list of hard disks paths

Returns
PRL_HANDLE to job that describing async operation result (PRL_INVALID_HANDLE means not enough memory to
instantiate async job represents object handle). Operation return code can be extracted with @ref PrlJob_GetRetCode()
call. Possible values:
PRL_ERR_INVALID_ARG - non valid PHT_SERVER handle	or non valid PHT_STRINGS_LIST handle was passed
PRL_ERR_SUCCESS - task was started successfully
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlSrv_StartConvertHdd, (
					  PRL_HANDLE hServer,
					  PRL_HANDLE hStringsList
					  ) );

/**
Performs retrieving dispatcher runtime information command to server.

Parameters
handle : valid PRL_HANDLE of PHT_SERVER type

Returns
PRL_HANDLE to job that describing async operation result (PRL_INVALID_HANDLE means not enough memory to
instantiate async job represents object handle). Operation return code can be extracted with @ref PrlJob_GetRetCode()
call. Possible values:
PRL_ERR_INVALID_ARG - non valid PHT_SERVER handle was passed
PRL_ERR_ACCESS_TOKEN_INVALID - current user session seems to be not valid to perform SMC command
PRL_ERR_SUCCESS - operation completed successfully
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlSrv_SmcGetRuntimeInfo, (
		PRL_HANDLE hServer
		) );


/* Generates VM answer event on some VM question.
   Parameters
   handle :         A handle of type PHT_VIRTUAL_MACHINE
                    identifying the virtual machine.
   unnamed_param :  buffer for storing result answer code
   Returns
   PRL_RESULT. Possible values: PRL_ERR_INVALID_ARG - non valid
   PHT_VIRTUAL_MACHINE handle or non valid (null) pointer to
   buffer was specified PRL_ERR_OUT_OF_MEMORY - not enough
   memory to instantiate VM event object PRL_ERR_SUCCESS -
   operation successfully completed                             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVm_CreateAnswerEvent, (
		PRL_HANDLE hVm,
		PRL_HANDLE_PTR phEvent,
		PRL_RESULT nAnswer
		) );

/**
* Client can use this method to set port forwarding configuration
* This method must be eliminated at Release stage
*
* @param valid PRL_HANDLE
* @param XML configuration string
* @return PRL_RESULT. Possible values:
* PRL_ERR_INVALID_ARG				- non valid PHT_DISP_NET_ADAPTER handle or non valid (NULL) pointer to config XML
* representation string was passed
* PRL_ERR_SUCCESS						- operation successfully completed
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlDispNet_PortForwardingFromString, (
				PRL_HANDLE hDispNet,
				PRL_CONST_STR sPortForwarding
				) );

/**
* Client can use this method to get port forwarding configuration configuration as XML string
* This method must be eliminated at Release stage
*
* @param valid PRL_HANDLE
* @param XML configuration string
* @return PRL_RESULT. Possible values:
* PRL_ERR_INVALID_ARG				- non valid PHT_DISP_NET_ADAPTER handle or non valid (NULL) pointer to storing result was passed
* PRL_ERR_SUCCESS						- operation successfully completed
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlDispNet_PortForwardingToString, (
				PRL_HANDLE hDispNet,
				PRL_VOID_PTR_PTR pPortForwarding
				) );


/* Parameters
   phConfig :  [out] pointer to storing new client hardware
               configuration handle
   Returns
   PRL_RESULT. Possible values: PRL_ERR_INVALID_ARG - non valid
   (NULL) pointer for storing result was passed
   PRL_ERR_OUT_OF_MEMORY - not enough memory to create server
   object PRL_ERR_SUCCESS - operation successfully completed    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlUsr_GetClientConfig, (
		PRL_HANDLE_PTR phConfig
		) );


/* Stands by all guests with generic PCI devices.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   nFlags :   Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_PrepareForHibernate, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );


/* Performs to wake up generic PCI devices after host resume.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   nFlags :   Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_AfterHostResume, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Dispatcher Service net adapters operations
///////////////////////////////////////////////////////////////////////////////

/* Removes the specified virtual network adapter object from
   memory. This function does not delete the adapter from the
   Dispatcher Service configuration. Use this function if you
   would like to cancel the changes that you've made to the
   object properties so far. The function must be called before
   calling the PrlSrv_AddNetAdapter or the
   PrlSrv_UpdateNetAdapter function (the calls that update the
   Dispatcher Service configuration and make the changes permanent). To
   delete an existing adapter from the configuration, use the
   PrlSrv_DeleteNetAdapter function.
   Parameters
   hDispNet :  A handle of type PHT_DISP_NET_ADAPTER identifying
               the network adapter.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_Remove, (
		PRL_HANDLE hDispNet
		) );

/* Determines whether the specified virtual network adapter is
   enabled or disabled.
   Parameters
   hDispNet :   A handle of type PHT_DISP_NET_ADAPTER identifying
                the network adapter.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the adapter is
                enabled. PRL_FALSE indicates that the adapter is
                disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_SetEnabled                                          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_IsEnabled, (
		PRL_HANDLE hDispNet,
		PRL_BOOL_PTR pbEnabled
		) );

/* Allows to enable or disable a specified virtual network
   adapter.
   Parameters
   hDispNet :  A handle of type PHT_DISP_NET_ADAPTER identifying
               the network adapter.
   bEnabled :  A boolean value indicating the type of action to
               perform. PRL_TRUE will enable the adapter.
               PRL_FALSE will disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_IsEnabled                                          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_SetEnabled, (
		PRL_HANDLE hDispNet,
		PRL_BOOL bEnabled
		) );

/* Determines whether the specified virtual network adapter is
   hidden.
   Parameters
   hDispNet :   A handle of type PHT_DISP_NET_ADAPTER identifying
                the network adapter.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the adapter is
                hidden.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_SetEnabled                                          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlDispNet_IsHidden, (
		PRL_HANDLE hDispNet,
		PRL_BOOL_PTR pbHidden
		) );

/* Allows to hide or show a specified virtual network adapter at system
   preferences.
   Parameters
   hDispNet :  A handle of type PHT_DISP_NET_ADAPTER identifying
               the network adapter.
   bHidden :  A boolean value indicating the type of action to
               perform. PRL_TRUE will hide the adapter.
               PRL_FALSE will show it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_IsHidden                                          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlDispNet_SetHidden, (
		PRL_HANDLE hDispNet,
		PRL_BOOL bHidden
		) );

/* Determines if DHCP scope is enabled for the specified virtual
   network adapter.
   Parameters
   hDispNet :       A handle of type PHT_DISP_NET_ADAPTER
                    identifying the network adapter.
   pbDhcpEnabled :  [out] A pointer to a variable that receives
                    the result. PRL_TRUE indicates that the DHCP
                    scope is enabled, PRL_FALSE indicates that it
                    is disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_SetDhcpEnabled                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_IsDhcpEnabled, (
		PRL_HANDLE hDispNet,
		PRL_BOOL_PTR pbDhcpEnabled
		) );

/* Allows to enable or disable the DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :      A handle of type PHT_DISP_NET_ADAPTER
                   identifying the network adapter.
   bDhcpEnabled :  Specifies the action to perform. PRL_TRUE
                   enables the DHCP scope. PRL_FALSE disables it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_IsDhcpEnabled                                       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_SetDhcpEnabled, (
		PRL_HANDLE hDispNet,
		PRL_BOOL bDhcpEnabled
		) );

/* \Returns the virtual network adapter network type (host-only
   or shared).
   Parameters
   hDispNet :       A handle of type PHT_DISP_NET_ADAPTER
                    identifying the network adapter.
   pnNetworkType :  [out] A pointer to a variable that receives
                    the network type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_SetNetworkType                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_GetNetworkType, (
		PRL_HANDLE hDispNet,
		PRL_NET_ADAPTER_EMULATED_TYPE_PTR pnNetworkType
		) );

/* Allows to set a network type for the specified virtual
   network adapter.
   Parameters
   hDispNet :      A handle of type PHT_DISP_NET_ADAPTER
                   identifying the network adapter.
   nNetworkType :  A network type to set. NOTE\: You can only use
                   PNA_HOST_ONLY and PNA_SHARED types here. You
                   CANNOT use the PNA_BRIDGED_ETHERNET type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetNetworkType                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_SetNetworkType, (
		PRL_HANDLE hDispNet,
		PRL_NET_ADAPTER_EMULATED_TYPE nNetworkType
		) );

/* \Returns the name of the specified virtual network adapter.
   Parameters
   hDispNet :                A handle of type
                             PHT_DISP_NET_ADAPTER identifying the
                             network adapter.
   sDispNetName :            [out] A pointer to a buffer that
                             receives the name (a UTF\-8 encoded,
                             null\-terminated string).
   pnDispNetNameBufLength :  [in] The size of the output buffer
                             (in bytes). Set the buffer pointer
                             to null and this value to zero to
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
   PrlDispNet_SetName                                             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_GetName, (
		PRL_HANDLE hDispNet,
		PRL_STR sDispNetName,
		PRL_UINT32_PTR pnDispNetNameBufLength
		) );

/* Sets the specified virtual network adapter name.
   Parameters
   hDispNet :         A handle of type PHT_DISP_NET_ADAPTER
                      identifying the network adapter.
   sNewDispNetName :  A name to set. The value must be a UTF\-8
                      encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetName                                           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_SetName, (
		PRL_HANDLE hDispNet,
		PRL_CONST_STR sNewDispNetName
		) );

/* \Returns the specified virtual network adapter UUID.
   Parameters
   hDispNet :                A handle of type
                             PHT_DISP_NET_ADAPTER identifying the
                             network adapter.
   sDispNetUuid :            [out] A pointer to a buffer that
                             receives the UUID (a UTF\-8 encoded,
                             null\-terminated string).
   pnDispNetUuidBufLength :  [in] The size of the output buffer
                             (in bytes). Set the buffer pointer
                             to null and this value to zero to
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
				 PrlDispNet_GetUuid, (
		PRL_HANDLE hDispNet,
		PRL_STR sDispNetUuid,
		PRL_UINT32_PTR pnDispNetUuidBufLength
		) );

/* \Returns the name of the physical network adapter to which
   the specified virtual network adapter is bound.
   Parameters
   hDispNet :                   A handle of type
                                PHT_DISP_NET_ADAPTER identifying
                                the network adapter.
   sDispNetSysName :            [out] A pointer to a buffer that
                                receives the name (a UTF\-8
                                encoded, null\-terminated
                                string).
   pnDispNetSysNameBufLength :  [in] The size of the output
                                buffer (in bytes). Set the buffer
                                pointer to null and this value to
                                zero to receive the required
                                size. [out] The required output
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
   PrlDispNet_SetSysName                                          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_GetSysName, (
		PRL_HANDLE hDispNet,
		PRL_STR sDispNetSysName,
		PRL_UINT32_PTR pnDispNetSysNameBufLength
		) );

/* Sets the specified virtual network adapter system name.
   Parameters
   hDispNet :            A handle of type PHT_DISP_NET_ADAPTER
                         identifying the network adapter.
   sNewDispNetSysName :  A system name to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetSysName                                       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_SetSysName, (
		PRL_HANDLE hDispNet,
		PRL_CONST_STR sNewDispNetSysName
		) );

/* \Returns the index identifying the specified adapter on the
   Dispatcher Service.
   Parameters
   hDispNet :  A handle of type PHT_DISP_NET_ADAPTER identifying
               the network adapter.
   pnIndex :   [out] A pointer to a variable that receives the
               index.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_SetIndex                                           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_GetIndex, (
		PRL_HANDLE hDispNet,
		PRL_UINT32_PTR pnIndex
		) );

/* Sets the specified virtual network adapter index (from 0 to
   5). Use this function to set a unique number for a virtual
   network adapter for easy identification in your client
   applications.
   Parameters
   hDispNet :  A handle of type PHT_DISP_NET_ADAPTER identifying
               the network adapter.
   nIndex :    An index to set. The value cannot be less then 0
               and greater than 5.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetIndex                                           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_SetIndex, (
		PRL_HANDLE hDispNet,
		PRL_UINT32 nIndex
		) );

/* \Returns the start IP address of DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                            A handle of type
                                         PHT_DISP_NET_ADAPTER
                                         identifying the network
                                         adapter.
   sDispNetDhcpScopeStartIp :            [out] A pointer to a
                                         buffer that receives the
                                         IP address (a UTF\-8
                                         encoded, null\-terminated
                                         string).
   pnDispNetDhcpScopeStartIpBufLength :  [in] The size of the
                                         \output buffer (in
                                         bytes). Set the buffer
                                         pointer to null and this
                                         value to zero to receive
                                         the required size. [out]
                                         The required output
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
   PrlDispNet_SetDhcpScopeStartIp                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_GetDhcpScopeStartIp, (
		PRL_HANDLE hDispNet,
		PRL_STR sDispNetDhcpScopeStartIp,
		PRL_UINT32_PTR pnDispNetDhcpScopeStartIpBufLength
		) );

/* Sets the start IP address of the DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                     A handle of type
                                  PHT_DISP_NET_ADAPTER
                                  identifying the network
                                  adapter.
   sNewDispNetDhcpScopeStartIp :  The IP address. The value must
                                  be a UTF\-8 encoded,
                                  null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetDhcpScopeStartIp                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_SetDhcpScopeStartIp, (
		PRL_HANDLE hDispNet,
		PRL_CONST_STR sNewDispNetDhcpScopeStartIp
		) );

/* \Returns the end IP address of DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                          A handle of type
                                       PHT_DISP_NET_ADAPTER
                                       identifying the network
                                       adapter.
   sDispNetDhcpScopeEndIp :            [out] A pointer to a buffer
                                       that receives the IP
                                       address (a UTF\-8 encoded,
                                       null\-terminated string).
   pnDispNetDhcpScopeEndIpBufLength :  [in] The size of the output
                                       buffer (in bytes). Set the
                                       buffer pointer to null and
                                       this value to zero to
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
   PrlDispNet_SetDhcpScopeEndIp                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_GetDhcpScopeEndIp, (
		PRL_HANDLE hDispNet,
		PRL_STR sDispNetDhcpScopeEndIp,
		PRL_UINT32_PTR pnDispNetDhcpScopeEndIpBufLength
		) );

/* Sets the end IP address of the DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                   A handle of type
                                PHT_DISP_NET_ADAPTER identifying
                                the network adapter.
   sNewDispNetDhcpScopeEndIp :  The IP address. The value must be
                                a UTF\-8 encoded, null\-terminated
                                string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetDhcpScopeEndIp                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_SetDhcpScopeEndIp, (
		PRL_HANDLE hDispNet,
		PRL_CONST_STR sNewDispNetDhcpScopeEndIp
		) );

/* \Returns the subnet mask of DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                         A handle of type
                                      PHT_DISP_NET_ADAPTER
                                      identifying the network
                                      adapter.
   sDispNetDhcpScopeMask :            [out] A pointer to a buffer
                                      that receives the subnet
                                      mask (a UTF\-8 encoded,
                                      null\-terminated string).
   pnDispNetDhcpScopeMaskBufLength :  [in] The size of the output
                                      buffer (in bytes). Set the
                                      buffer pointer to null and
                                      this value to zero to
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
   PrlDispNet_SetDhcpScopeMask                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_GetDhcpScopeMask, (
		PRL_HANDLE hDispNet,
		PRL_STR sDispNetDhcpScopeMask,
		PRL_UINT32_PTR pnDispNetDhcpScopeMaskBufLength
		) );

/* Sets the subnet mask of the DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                  A handle of type
                               PHT_DISP_NET_ADAPTER identifying
                               the network adapter.
   sNewDispNetDhcpScopeMask :  The subnet mask. The value must be
                               a UTF\-8 encoded, null\-terminated
                               string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetDhcpScopeMask                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispNet_SetDhcpScopeMask, (
		PRL_HANDLE hDispNet,
		PRL_CONST_STR sNewDispNetDhcpScopeMask
		) );


/* ============== IPV6 ==============  */

/* Determines if DHCP6 scope is enabled for the specified virtual
   network adapter.
   Parameters
   hDispNet :       A handle of type PHT_DISP_NET_ADAPTER
                    identifying the network adapter.
   pbDhcpEnabled :  [out] A pointer to a variable that receives
                    the result. PRL_TRUE indicates that the DHCP
                    scope is enabled, PRL_FALSE indicates that it
                    is disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_SetDhcpEnabled                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlDispNet_IsDhcp6Enabled, (
		PRL_HANDLE hDispNet,
		PRL_BOOL_PTR pbDhcpEnabled
		) );

/* Allows to enable or disable the DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :      A handle of type PHT_DISP_NET_ADAPTER
                   identifying the network adapter.
   bDhcpEnabled :  Specifies the action to perform. PRL_TRUE
                   enables the DHCP scope. PRL_FALSE disables it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_IsDhcpEnabled                                       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlDispNet_SetDhcp6Enabled, (
		PRL_HANDLE hDispNet,
		PRL_BOOL bDhcpEnabled
		) );


/* \Returns the start IP address of DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                            A handle of type
                                         PHT_DISP_NET_ADAPTER
                                         identifying the network
                                         adapter.
   sDispNetDhcpScopeStartIp :            [out] A pointer to a
                                         buffer that receives the
                                         IP6 address (a UTF\-8
                                         encoded, null\-terminated
                                         string).
   pnDispNetDhcpScopeStartIpBufLength :  [in] The size of the
                                         \output buffer (in
                                         bytes). Set the buffer
                                         pointer to null and this
                                         value to zero to receive
                                         the required size. [out]
                                         The required output
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
   PrlDispNet_SetDhcp6ScopeStartIp                                  */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlDispNet_GetDhcp6ScopeStartIp, (
		PRL_HANDLE hDispNet,
		PRL_STR sDispNetDhcpScopeStartIp,
		PRL_UINT32_PTR pnDispNetDhcpScopeStartIpBufLength
		) );

/* Sets the start IP address of the DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                     A handle of type
                                  PHT_DISP_NET_ADAPTER
                                  identifying the network
                                  adapter.
   sNewDispNetDhcpScopeStartIp :  The IP6 address. The value must
                                  be a UTF\-8 encoded,
                                  null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetDhcpScopeStartIp                                */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlDispNet_SetDhcp6ScopeStartIp, (
		PRL_HANDLE hDispNet,
		PRL_CONST_STR sNewDispNetDhcpScopeStartIp
		) );

/* \Returns the end IP address of DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                          A handle of type
                                       PHT_DISP_NET_ADAPTER
                                       identifying the network
                                       adapter.
   sDispNetDhcpScopeEndIp :            [out] A pointer to a buffer
                                       that receives the IP6
                                       address (a UTF\-8 encoded,
                                       null\-terminated string).
   pnDispNetDhcpScopeEndIpBufLength :  [in] The size of the output
                                       buffer (in bytes). Set the
                                       buffer pointer to null and
                                       this value to zero to
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
   PrlDispNet_SetDhcp6ScopeEndIp                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlDispNet_GetDhcp6ScopeEndIp, (
		PRL_HANDLE hDispNet,
		PRL_STR sDispNetDhcpScopeEndIp,
		PRL_UINT32_PTR pnDispNetDhcpScopeEndIpBufLength
		) );

/* Sets the end IP address of the DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                   A handle of type
                                PHT_DISP_NET_ADAPTER identifying
                                the network adapter.
   sNewDispNetDhcpScopeEndIp :  The IP6 address. The value must be
                                a UTF\-8 encoded, null\-terminated
                                string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetDhcp6ScopeEndIp                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlDispNet_SetDhcp6ScopeEndIp, (
		PRL_HANDLE hDispNet,
		PRL_CONST_STR sNewDispNetDhcpScopeEndIp
		) );

/* \Returns the subnet mask of DHCP scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                         A handle of type
                                      PHT_DISP_NET_ADAPTER
                                      identifying the network
                                      adapter.
   sDispNetDhcpScopeMask :            [out] A pointer to a buffer
                                      that receives the subnet
                                      mask (a UTF\-8 encoded,
                                      null\-terminated string).
   pnDispNetDhcpScopeMaskBufLength :  [in] The size of the output
                                      buffer (in bytes). Set the
                                      buffer pointer to null and
                                      this value to zero to
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
   PrlDispNet_SetDhcp6ScopeMask                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlDispNet_GetDhcp6ScopeMask, (
		PRL_HANDLE hDispNet,
		PRL_STR sDispNetDhcpScopeMask,
		PRL_UINT32_PTR pnDispNetDhcpScopeMaskBufLength
		) );

/* Sets the subnet mask of the DHCP6 scope for the specified
   virtual network adapter.
   Parameters
   hDispNet :                  A handle of type
                               PHT_DISP_NET_ADAPTER identifying
                               the network adapter.
   sNewDispNetDhcpScopeMask :  The subnet mask. The value must be
                               a UTF\-8 encoded, null\-terminated
                               string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispNet_GetDhcp6ScopeMask                                    */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlDispNet_SetDhcp6ScopeMask, (
		PRL_HANDLE hDispNet,
		PRL_CONST_STR sNewDispNetDhcpScopeMask
		) );

/* Adds a new virtual network adapter to the specified Dispatcher
   Service. To get the return code from the PHT_JOB object, use
   the PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :      A handle of type PHT_SERVER identifying the
                  Dispatcher Service.
   hNetAdapter :  A handle of type PHT_DISP_NET_ADAPTER containing
                  the new adapter information. For the details on
                  how to create the handle and how to set the
                  adapter properties, see PHT_DISP_CONFIG.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                    */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlSrv_AddNetAdapter, (
		PRL_HANDLE hServer,
		PRL_HANDLE hNetAdapter
		) );

/* Removes a virtual network adapter from the Dispatcher Service
   configuration. See PHT_DISP_CONFIG and PHT_DISP_NET_ADAPTER
   for more information about Dispatcher Service preferences and
   virtual network adapters.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid server handle was passed or
   adapter index out of bound.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
              Service.
   nIndex :   An index of the virtual network adapter to remove.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlSrv_DeleteNetAdapter, (
		PRL_HANDLE hServer,
		PRL_UINT32 nIndex
		) );

/* Updates an existing virtual network adapter in the specified
   Dispatcher Service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :      A handle of type PHT_SERVER identifying the
                  Dispatcher Service.
   hNetAdapter :  A handle of PHT_DISP_NET_ADAPTER containing the
                  new adapter information.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_1,
						   PrlSrv_UpdateNetAdapter, (
		PRL_HANDLE hServer,
		PRL_HANDLE hNetAdapter
		) );


/* Private function to store value by specified keys on server

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :[in] A handle of type PHT_SERVER identifying the
             Dispatcher Service.
   sKey	   :[in] Key to access to value
   sValue  :[in] Value to store
   nFlags  :[in] Reserved parameter.

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlSrv_StoreValueByKey, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sKey,
		PRL_CONST_STR sValue,
		PRL_UINT32 nFlags
		) );

/* Private function to store value by specified keys on server

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :[in] A handle of type PHT_VIRTUAL_MACHINE identifying the
             virtual machine.
   sKey	   :[in] Key to access to value
   sValue  :[in] Value to store
   nFlags  :[in] Reserved parameter.

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_StoreValueByKey, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sKey,
		PRL_CONST_STR sValue,
		PRL_UINT32 nFlags
		) );

/* Creates a new virtual network adapter object and adds it to
   the network adapters list. This function merely creates a new
   virtual network adapter object in the local memory. To add a
   network adapter to the Dispatcher Service, populate the
   necessary network adapter properties using functions from the
   PHT_DISP_NET_ADAPTER group and then call PrlSrv_AddNetAdapter
   to add the adapter to the Dispatcher Service configuration.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   phDispNet :    [out] A pointer to a new handle of type
                  PHT_DISP_NET_ADAPTER.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispCfg_CreateDispNet, (
		PRL_HANDLE hDispConfig,
		PRL_HANDLE_PTR phDispNet
		) );

/* Determines the total number of the virtual network adapters
   in the Dispatcher Service configuration.
   Parameters
   hDispCfg :        A handle of type PHT_DISP_CONFIG.
   pnDispNetCount :  [out] A pointer to a variable that receives
                     the total adapter count.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispCfg_GetDispNetCount, (
		PRL_HANDLE hDispCfg,
		PRL_UINT32_PTR pnDispNetCount
		) );

/* Obtains information about a virtual network adapter from the
   Dispatcher Service configuration. Dispatcher Service can have
   multiple network adapters, each identified by a numeric index
   starting with 0 and up. Use PrlDispCfg_GetDispNetCount to
   determine the total adapter count and then obtain each
   adapter information by calling this function in a loop,
   passing an index counter in each iteration.
   Parameters
   hDispCfg :       A handle of type PHT_DISP_CONFIG.
   nDispNetIndex :  An index identifying the adapter in the
                    adapter list starting with 0 and up. To
                    obtain the total number of the available
                    adapters, use the PrlDispCfg_GetDispNetCount
                    function.
   phDispNet :      [out] A pointer to a handle of type
                    PHT_DISP_NET_ADAPTER. On function return, the
                    object will contain the specified net adapter
                    information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlDispCfg_GetDispNet, (
		PRL_HANDLE hDispCfg,
		PRL_UINT32 nDispNetIndex,
		PRL_HANDLE_PTR phDispNet
		) );

/* Set path to log location for SDK library module.
   If sLogPath is NULL or empty string - default
   client log path will be set.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
					 PrlApi_SetLogPath, (
		PRL_CONST_STR sLogPath
		) );

/* \Returns a pointer to a buffer containing handles to the
   virtual devices installed in a virtual machine. To determine
   the number of virtual devices installed in a virtual machine
   (so that you can allocate the output buffer memory
   dynamically), use the PrlVmCfg_GetDevsCount function. You can
   also allocate the memory statically if you wish. When
   iterating through the returned array, the handle type
   (essentially the device type) in each iteration can be
   determined using the PrlHandle_GetType function. You can
   search this documentation for topics with PHT_VIRTUAL_DEV_
   prefix in their names for the list of the available virtual
   device handles. It is the responsibility of the caller to
   free the memory when the handles are no longer needed.
   Parameters
   hVmCfg :               A handle of type PHT_VM_CONFIGURATION
                          identifying the virtual machine configuration.
   vmdev_handles :        [out] A pointer to a variable that
                          receives the data.
   vmdev_handles_count :  [in] The number of handles that can be
                          placed into the output buffer. [out]
                          The actual number of handles returned.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle, null pointer, or
   invalid number of handles.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough to fit all of the available devices.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a device
   object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlVmCfg_GetDevsList, (
		PRL_HANDLE hVmCfg,
		PRL_HANDLE_PTR vmdev_handles,
		PRL_UINT32_PTR vmdev_handles_count
		) );

/* \Sends Client statistic data to server.
Parameters
hServer		:			[in] Server handle.
sStatistics :           [in] Pointer to a buffer which contains statistic data
(a UTF\-8 encoded, null\-terminated string).
PRL_UINT32 nFlags - reserved parameter
Returns
PRL_RESULT. Possible values are:

PRL_ERR_INVALID_ARG - null pointer was
passed.

PRL_ERR_SUCCESS - function completed successfully.             */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
				PrlSrv_SendClientStatistics, (
				PRL_HANDLE hServer,
				PRL_CONST_STR sStatistics,
				PRL_UINT32 nFlags
				) );

typedef enum _PRL_USB_LIST_VERSION
{
	ULV_VER0 = 0,
	ULV_VER1 = 1,
	ULV_VER_END = ULV_VER1,
} PRL_USB_LIST_VERSION;

/* Updates internal client USB associations list.
list to update has format UsbFriendlyName:USB_LIST_DIVIDER:UsbId

To get the return code from the PHT_JOB object, use the
PrlJob_GetRetCode function. Possible values are:

PRL_ERR_INVALID_ARG - invalid handle was passed.

PRL_ERR_SUCCESS - function completed successfully.

Parameters
hServer :       A handle of type PHT_SERVER identifying the
Dispatcher Service.
hStringsList :  A handle of type PHT_STRINGS_LIST containing
the USB associations list.
PRL_UINT32 nListVersion - version of list on client
PRL_UINT32 nFlags - reserved parameter
Returns
A handle of type PHT_JOB containing the results of this
asynchronous operation or PRL_INVALID_HANDLE if there's not
enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						  PrlSrv_UpdateUsbDevicesAssociationsList, (
						  PRL_HANDLE hServer,
						  PRL_HANDLE hStringsList,
						  PRL_UINT32 nListVersion,
						  PRL_UINT32 nFlags
						  ) );


///////////////////////////////////////////////////////////////////////////////
/// @section Compressor Utility functions
///////////////////////////////////////////////////////////////////////////////

/* Initiates the Compressor process. The virtual machine must be
   registered with Dispatcher Service.

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
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_RunCompressor, (
		PRL_HANDLE hVm
		) );

/* Cancels the running Compressor operation that was started
   with PrlVm_RunCompressor. To get the return code from the
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
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVm_CancelCompressor, (
		PRL_HANDLE hVm
		) );


PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
					   PrlVm_InternalCommand, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sCmdName,
		PRL_HANDLE hArgsList
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Traffic Accounting/Shaping operations
///////////////////////////////////////////////////////////////////////////////
/* Shaping configuration changed, apply new configuration to all running VMs.
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :           A handle of type PHT_SERVER identifying the
                       Dispatcher Service.
   nFlags :            Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_GetNetworkClassesList

*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
					PrlSrv_RestartNetworkShaping, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );

/* Update the list of network shaping entries represents the
   network shaping  configuration.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :		A handle of type PHT_SERVER identifying the
			Dispatcher Service.
   hNetworkShapingList: A handle of type PHT_HANDLE_LIST contains the
                        list of handles of type PHT_NETWORK_SHAPING

   nFlags :		Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_GetNetworkShapingList
*/
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						PrlSrv_UpdateNetworkShapingList, (
		PRL_HANDLE hServer,
		PRL_HANDLE hNetworkShapingList,
		PRL_UINT32 nFlags
		) );

/* Obtains a list of network shaping entries
   represents the network shaping configuration.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParam function to obtain a handle
   of type PHT_HANDLES_LIST  containing the list of the handles of type
   PHT_NETWORK_SHAPING
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   nFlags :   Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
						PrlSrv_GetNetworkShapingList, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );

/* Sets the specified virtual machine default gateway.
   Parameters
   hVmCfg :                A handle of type PHT_VM_CONFIGURATION
                           identifying the virtual machine
                           configuration.
   sNewVmDefaultGateway :  The default gateway to set. The value
                           must be a a UTF\-8 encoded,
                           null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_SetDefaultGateway, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmDefaultGateway
		) );


/* Sets the specified virtual machine default gateway (IPv6).
   Parameters
   hVmCfg :                A handle of type PHT_VM_CONFIGURATION
                           identifying the virtual machine
                           configuration.
   sNewVmDefaultGateway :  The default gateway to set. The value
                           must be a a UTF\-8 encoded,
                           null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
				 PrlVmCfg_SetDefaultGatewayIPv6, (
		PRL_HANDLE hVmCfg,
		PRL_CONST_STR sNewVmDefaultGateway
		) );


/* \Returns the default gateway of the specified virtual
   machine.
   Parameters
   hVmCfg :						 A handle of type PHT_VM_CONFIGURATION
								 identifying the virtual machine
								 configuration.
   sVmDefaultGateway :           [out] A pointer to a buffer that receives
								 the result (a UTF\-8 encoded,
								 null\-terminated string).
   pnVmDefaultGatewayBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_GetDefaultGateway, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmDefaultGateway,
		PRL_UINT32_PTR pnVmDefaultGatewayBufLength
		) );

/* \Returns the default gateway (IPv6) of the specified virtual
   machine.
   Parameters
   hVmCfg :						 A handle of type PHT_VM_CONFIGURATION
								 identifying the virtual machine
								 configuration.
   sVmDefaultGateway :           [out] A pointer to a buffer that receives
								 the result (a UTF\-8 encoded,
								 null\-terminated string).
   pnVmDefaultGatewayBufLength : [in] The size of the output buffer (in
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
				 PrlVmCfg_GetDefaultGatewayIPv6, (
		PRL_HANDLE hVmCfg,
		PRL_STR sVmDefaultGateway,
		PRL_UINT32_PTR pnVmDefaultGatewayBufLength
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Single Sign On operations
///////////////////////////////////////////////////////////////////////////////
/*
   Single Sign On operations allows to realize automatic logon to guest OS and
   to control guest lock/unlock state.

   When VM needs credentials to logon or unlock it sends
   PET_IO_SSO_GET_CREDENTIALS event. You should call function
   PrlVm_SSOSendEventCredentials with credentials in responce to this event.

   You should call PrlVm_SSOSendEventSettingsChanged when:
   - credentials changed
   - when you want to enable/disable Single Sign On functionality
   - when you want to enable/disable lock/unock synchronization

   You should call PrlVm_SSOSendEventLockState to control guest
   lock/unlock state.
*/

/* Send credentials to guest
   Parameters
   hVm :					A handle of type PHT_VM_CONFIGURATION identifying
                        	the VM.
   hEvent :					Handle of PET_IO_SSO_GET_CREDENTIALS event,
							in response to which this function is invoked.
   bEnabled :				Is Single Sign On functionality enabled
   sDomain :				Domain name (The value must be a a UTF\-8 encoded,
							null\-terminated string, can't be null)
   sUsername :				User name (The value must be a a UTF\-8 encoded,
							null\-terminated string, can't be null)
   sPassword :				Password (The value must be a a UTF\-8 encoded,
							null\-terminated string, can't be null)

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of VM

   PRL_ERR_UNINITIALIZED - when Desktop object is not accessible

   PRL_ERR_OUT_OF_MEMORY - when failed to allocate memory

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
				PrlVm_SSOSendEventCredentials, (
				PRL_HANDLE hVm,
				PRL_HANDLE hEvent,
				PRL_BOOL bEnabled,
				PRL_CONST_STR sDomain,
				PRL_CONST_STR sUsername,
				PRL_CONST_STR sPassword
				) );

/* Tell VM that Single Sign On settings were changed
   Parameters
   hVm :				A handle of type PHT_VM_CONFIGURATION identifying
                        the VM.
   bEnabled :			Is Single Sign On functionality enabled
   bLockSyncEnabled :	Is sync lock/unlock enabled

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of VM

   PRL_ERR_UNINITIALIZED - when Desktop object is not accessible

   PRL_ERR_OUT_OF_MEMORY - when failed to allocate memory

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
			PrlVm_SSOSendEventSettingsChanged, (
			PRL_HANDLE hVm,
			PRL_BOOL bEnabled,
			PRL_BOOL bLockSyncEnabled
			) );

/* Send command to change guest screen lock state
   Parameters
   hVm :				A handle of type PHT_VM_CONFIGURATION identifying
                        the VM.
   bLocked :			Is guest should be locked

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of VM

   PRL_ERR_UNINITIALIZED - when Desktop object is not accessible

   PRL_ERR_OUT_OF_MEMORY - when failed to allocate memory

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
				PrlVm_SSOSendEventLockState, (
				PRL_HANDLE hVm,
				PRL_BOOL bLocked
				) );

/* \Returns the hard disk block size.
   Parameters
   hVmDev :      A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                 identifying the virtual hard disk.
   pnnBytes :    [out] Pointer to a variable that receives the
                 hard disk size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_SetDiskSize                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
				 PrlVmDevHd_GetBlockSize, (
		PRL_HANDLE hVmDev,
		PRL_UINT32_PTR pnBytes
		) );

/* Sets the block size of the specified virtual hard disk. Use this
   function when adding a new virtual hard disk or when
   modifying the size of an existing disk.
   Parameters
   hVmDev :     A valid handle of type PHT_VIRTUAL_DEV_HARD_DISK
                identifying the virtual hard disk.
   nBytes:      The new size in bytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlVmDevHd_GetDiskSize                                        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
				 PrlVmDevHd_SetBlockSize, (
		PRL_HANDLE hVmDev,
		PRL_UINT32 nBytes
		) );

/* Determines whether the 3D graphical acceleration technology is
   available in the virtual machine video.
   Parameters
   handle :             A handle of type PHT_VIRTUAL_MACHINE
                        identifying the virtual machine.
   pbVm3DAccelerationEnabled :  A pointer to a variable that receives the
                        \result. PRL_TRUE indicates that the 3D
                        acceleration is available. PRL_FALSE
                        indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_Is3DAccelerationEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbVm3DAccelerationEnabled
		) );

/* Sets 3D graphical acceleration technology mode (enabled or disabled)
   for the specified virtual machine.
   Parameters
   handle :             A handle of type PHT_VIRTUAL_MACHINE
                        identifying the virtual machine.
   bVm3DAccelerationEnabled :  Setting value. Perform PRL_TRUE
                               to enable the 3D acceleration.
                               Perform PRL_FALSE to disable last one.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_VIRTUAL_MACHINE handle or
   NULL pointer to the output buffer was specified.

   PRL_ERR_SUCCESS - operation completed successfully             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_2,
				 PrlVmCfg_Set3DAccelerationEnabled, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bVm3DAccelerationEnabled
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section HA processing.
///////////////////////////////////////////////////////////////////////////////

/* \Start HA service by service name
   Parameters
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
                                 PrlSrv_StartClusterService, (
                PRL_HANDLE hServer,
                PRL_CONST_STR sServiceName,
                PRL_UINT32 nFlags
                ) );

/* \Stop HA service by service name
   Parameters
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_3,
                                 PrlSrv_StopClusterService, (
                PRL_HANDLE hServer,
                PRL_CONST_STR sServiceName,
                PRL_UINT32 nFlags
                ) );


/* Allows to activate/deactivate cluster mode.
   Parameters
   hDispConfig :     A handle of type PHT_DISP_CONFIG.
   bClusterMode :    To activate cluster mode, set this
                     parameter to PRL_TRUE.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
                        PrlDispCfg_SetClusterMode, (
                PRL_HANDLE hDispConfig,
                PRL_BOOL bClusterMode
                ) );

/* Determines whether product operates in cluster mode.
   Parameters
   hDispConfig :      A handle of type PHT_DISP_CONFIG.
   pbClusterMode :    [out] A pointer to a variable that receives
                      the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
                        PrlDispCfg_IsClusterMode, (
                PRL_HANDLE hDispConfig,
                PRL_BOOL_PTR pbClusterMode
                ) );

/* Get mobile host id.
   Parameters
   hHandle :  A handle of type PHT_DISP_CONFIG.
   sHostId :  [out] A pointer to a buffer that receives the result.
   pnHostIdBufLength: [in] The size of the output buffer (in
                   bytes). Set the buffer pointer to null
                   and this parameter's value to zero to
                   receive the required size. [out] The
                   required output buffer size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or string was passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlDispCfg_GetHostId, (
		PRL_HANDLE hDispConfig,
		PRL_STR sHostId,
		PRL_UINT32_PTR pnHostIdBufLength
		) );

/* Set user-defined mobile host id.
   Parameters
   hHandle :  A handle of type PHT_DISP_CONFIG.
   sHostId :  host id value. Pass an empty string to reset
			  host id to hardware id.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or string was passed.

   PRL_ERR_SUCCESS - function completed successfully.
*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlDispCfg_SetHostId, (
		PRL_HANDLE hDispConfig,
		PRL_CONST_STR sHostId
		) );


/* Requests a server to send a problem report on behalf of the given VM (its log
   is supposed to be attached to the report too) with possibility
   to append also the problem report assemblied at the client's side.

   The report should be created as follows:
    1) with PrlApi_CreateProblemReport call
    2) filled with PrlReport_Assembly

   Similar calls are PrlApi_SendProblemReport and PrlApi_SendPackedProblemReport,
   PrlReport_Send, PrlSrv_SendProblemReport

   Async response should normally contain report id.

   Parameters:
   hVm - handle of type PHT_VM
   hProblemReport - the report assemblied at the client's side
   nFlags - flags associated with the operation (see PRL_PROBLEM_REPORT_FLAGS)

    Returns:
    PRL_HANDLE - handle to the job generated					*/
PRL_ASYNC_METHOD_DECL( VIRTUOZZO_API_VER_5,
                 PrlVm_SendProblemReport, (
        PRL_HANDLE hVm,
        PRL_HANDLE hProblemReport,
        PRL_UINT32 nFlags
        ) );

/* To get the CPU features masks from dispatcher config
   hDispConfig :		A handle of type PHT_DISP_CONFIG.
   pMasks:			[out] A pointer to masks of processor features
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_SetCpuFeaturesMask	*/
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlDispCfg_GetCpuFeaturesMask, (
		PRL_HANDLE hDispConfig,
		PRL_CPU_FEATURES_MASK_PTR pMasks
		) );

/* Allows to set the CPU features masks. This masks will save at dispatcher config
   and will use on VM start.
   Parameters
   hDispConfig :		A handle of type PHT_DISP_CONFIG.
   pMasks:			[in] A pointer to masks of processor features
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetCpuFeaturesMask */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_5,
			PrlDispCfg_SetCpuFeaturesMask, (
		PRL_HANDLE hDispConfig,
		PRL_CONST_CPU_FEATURES_MASK_PTR pMasks
		) );
/* \Returns the Container id of the specified
   virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pnEnvId:             [out] Container id
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_GetEnvId, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnEnvId
		) );	

/* \Returns the minimal size of the host's memory (RAM) consumption
   by the virtual machine, in megabytes.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pnHostMemQuotaMin :  [out] A pointer to a variable that receives
                        the \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the memory information is not available in
   the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_GetHostMemQuotaMin, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnHostMemQuotaMin
		) );

/* Sets the minimal size of the host's memory (RAM) consumption
   by the virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   nHostMemQuotaMin :   The minimal RAM size to set, in megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_SetHostMemQuotaMin, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nHostMemQuotaMin
		) );

/* \Returns the maximal size of the host's memory (RAM) consumption
   by the virtual machine, in megabytes.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pnHostMemQuotaMax :  [out] A pointer to a variable that receives
                        the \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the memory information is not available in
   the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_GetHostMemQuotaMax, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnHostMemQuotaMax
		) );

/* Sets the maximal size of the host's memory (RAM) consumption
   by the virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   nHostMemQuotaMax :   The maximal RAM size to set, in megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_SetHostMemQuotaMax, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nHostMemQuotaMax
		) );

/* \Returns the relative priority of the virtual machine in host's
   memory (RAM) distribution mechanism.
   Parameters
   hVmCfg :                  A handle of type PHT_VM_CONFIGURATION
                             identifying the virtual machine
                             configuration.
   pnHostMemQuotaPriority :  [out] A pointer to a variable that
                             receives the \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the memory information is not available in
   the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_GetHostMemQuotaPriority, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnHostMemQuotaPriority
		) );

/* Sets the relative priority of the virtual machine in host's
   memory (RAM) distribution mechanism.
   Parameters
   hVmCfg :                A handle of type PHT_VM_CONFIGURATION
                           identifying the virtual machine
                           configuration.
   nHostMemQuotaPriority : The priority to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_SetHostMemQuotaPriority, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nHostMemQuotaPriority
		) );

/* Determines whether the automatic calculation of the host's memory
   (RAM) consumption parameters enabled for the virtual machine.
   Parameters
   hVmCfg :              A handle of type PHT_VM_CONFIGURATION
                         identifying the virtual machine configuration.
   pbHostMemAutoQuota :  [out] A pointer to a variable that receives
                         the \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the memory information is not available in
   the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_IsHostMemAutoQuota, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL_PTR pbHostMemAutoQuota
		) );

/* Enables or disables the automatic calculation of the host's memory
   (RAM) consumption parameters for the virtual machine.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   bHostMemAutoQuota :  The value to set. Specify PRL_TRUE to enable
                        the automatic calculation or PRL_FALSE to
                        disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_SetHostMemAutoQuota, (
		PRL_HANDLE hVmCfg,
		PRL_BOOL bHostMemAutoQuota
		) );


/* Returns the maximum memory limit which can be locked by balloon
   in the virtual machine, in percent of guest RAM.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   pnMaxBalloonSize :  [out] A pointer to a variable that receives
                        the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the memory information is not available in
   the virtual machine configuration.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_GetMaxBalloonSize, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32_PTR pnMaxBalloonSize
		) );

/* Sets the maximum memory limit which can be locked by balloon
   in the virtual machine, in percent of guest RAM.
   Parameters
   hVmCfg :             A handle of type PHT_VM_CONFIGURATION
                        identifying the virtual machine configuration.
   nMaxBalloonSize :   The maximum balloon size to set, in percent.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_3,
			PrlVmCfg_SetMaxBalloonSize, (
		PRL_HANDLE hVmCfg,
		PRL_UINT32 nMaxBalloonSize
		) );

/* Allows to change the password of a guest operating system
   user. Prior to calling this function, a handle to a user
   session in a virtual machine must be obtained using the
   PrlVm_LoginInGuest function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVmGuest :     A handle of type PHT_VM_GUEST_SESSION
                  identifying the user session in a virtual
                  machine. The handle is obtained using the
                  PrlVm_LoginInGuest function.
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


   See Also
   PrlVm_LoginInGuest                                             */
PRL_ASYNC_SRV_METHOD_DECL( VIRTUOZZO_API_VER_2,
						   PrlVmGuest_SetUserPasswd, (
		PRL_HANDLE hVmGuest,
		PRL_CONST_STR sUserName,
		PRL_CONST_STR sUserPasswd,
		PRL_UINT32 nFlags
		) );

/* \Returns the Dispatcher Service uptime in seconds.
   Parameters
   hStat :     A handle of type PHT_SYSTEM_STATISTICS.
   pnUptime :  [out] A pointer to a variable that receives the
               \result (Dispatcher Service uptime in seconds).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetDispUptime, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnUptime
		) );

/* \Returns total swap size in bytes.
   Parameters
   hStat :   A handle of type PHT_SYSTEM_STATISTICS.
   pnSize :  [out] A pointer to a variable that receives the
             \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetTotalSwapSize, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns the size of swap space currently in use (in bytes).
   Parameters
   hStat :   A handle of type PHT_SYSTEM_STATISTICS.
   pnSize :  [out] A pointer to a variable that receives the
             \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetUsageSwapSize, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns free swap size in bytes.
   Parameters
   hStat :   A handle of type PHT_SYSTEM_STATISTICS.
   pnSize :  [out] A pointer to a variable that receives the result
             (free swap size in bytes.)
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.               */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetFreeSwapSize, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Net iface statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

/* Determines the number of objects of type
   PHT_SYSTEM_STATISTICS_IFACE contained in the specified
   PHT_SYSTEM_STATISTICS object. Each network interface will
   have its own PHT_SYSTEM_STATISTICS_IFACE object instance.
   Parameters
   hStat :    A handle of type PHT_SYSTEM_STATISTICS.
   pnCount :  [out] A pointer to a variable that receives the
              \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlStat_GetIfaceStat                                       */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetIfacesStatsCount, (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		) );

/* Obtains a handle of type PHT_SYSTEM_STATISTICS_IFACE
   containing the specified network interface statistics.
   Parameters
   hStat :        A handle of type PHT_SYSTEM_STATISTICS.
   nIndex :       A numeric index identifying the network
                  statistics object instance (beginning with 0).
                  To get the number of instances, use the
                  PrlStat_GetIfacesStatsCount function.
   phIfaceStat :  [out] A pointer to a variable that receives the
                  handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.d            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetIfaceStat, (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phIfaceStat
		) );

/* \Returns the network interface system name.
   Parameters
   hIfaceStat :             A handle of type of
                            PHT_SYSTEM_STATISTICS_IFACE
                            identifying the network interface.
   sSystemName :            [out] A pointer to a buffer that
                            receives the result (a UTF\-8
                            encoded, null\-terminated string).
   pnSystemNameBufLength :  [in] The size of the output buffer
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
				 PrlStatIface_GetSystemName, (
		PRL_HANDLE hIfaceStat,
		PRL_STR	sSystemName,
		PRL_UINT32_PTR pnSystemNameBufLength
		) );

/* \Returns the total number of bytes the network interface has
   received since the Dispatcher Service was last started.
   Parameters
   hIfaceStat :  A handle of type of PHT_SYSTEM_STATISTICS_IFACE
                 identifying the network interface.
   pnDataSize :  [out] A pointer to a variable that receives the
                 \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatIface_GetInDataSize, (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnDataSize
		) );

/* \Returns the total number of bytes the network interface has
   sent since the Dispatcher Service was last started.
   Parameters
   hIfaceStat :  A handle of type of PHT_SYSTEM_STATISTICS_IFACE
                 identifying the network interface.
   pnDataSize :  [out] A pointer to a variable that receives the
                 \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatIface_GetOutDataSize, (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnDataSize
		) );

/* \Returns the total number of packets the network interface
   has received since the Dispatcher Service was last started.
   Parameters
   hIfaceStat :   A handle of type of PHT_SYSTEM_STATISTICS_IFACE
                  identifying the network interface.
   pnPkgsCount :  [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatIface_GetInPkgsCount, (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnPkgsCount
		) );

/* \Returns the total number of packets the network interface
   has sent since the Dispatcher Service was last started.
   Parameters
   hIfaceStat :   A handle of type of PHT_SYSTEM_STATISTICS_IFACE
                  identifying the network interface.
   pnPkgsCount :  [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatIface_GetOutPkgsCount, (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnPkgsCount
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section User session statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

/* Determines the number of handles of type
   PHT_SYSTEM_STATISTICS_USER_SESSION contained in the specified
   PHT_SYSTEM_STATISTICS object. Each user session will have its
   own PHT_SYSTEM_STATISTICS_USER_SESSION object instance.
   Parameters
   hStat :    A handle of type PHT_SYSTEM_STATISTICS.
   pnCount :  [out] A pointer to a variable that receives the
              \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlStat_GetUserStat                                           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetUsersStatsCount, (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		) );

/* Obtains a handle of type PHT_SYSTEM_STATISTICS_USER_SESSION
   containing the specified user session statistics.
   Parameters
   hStat :       A handle of type PHT_SYSTEM_STATISTICS.
   nIndex :      A numeric index identifying the user session
                 statistics object instance (beginning with 0).
                 To determine the number of instances, use the
                 PrlStat_GetUsersStatsCount function.
   phUserStat :  [out] A pointer to a variable that receives the
                 handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.d           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetUserStat, (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phUserStat
		) );

/* \Returns session user name.
   Parameters
   hUserStat :            A handle of type
                          PHT_SYSTEM_STATISTICS_USER_SESSION
                          identifying the user session.
   sUserName :            [out] A pointer to a buffer that
                          receives the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnUserNameBufLength :  [in] The size of the output buffer (in
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
				 PrlStatUser_GetUserName, (
		PRL_HANDLE hUserStat,
		PRL_STR	sUserName,
		PRL_UINT32_PTR pnUserNameBufLength
		) );

/* \Returns the name of the host system service that created the
   session.
   Parameters
   hUserStat :               A handle of type
                             PHT_SYSTEM_STATISTICS_USER_SESSION
                             identifying the user session.
   sServiceName :            [out] A pointer to a buffer that
                             receives the result (a UTF\-8
                             encoded, null\-terminated string).
   pnServiceNameBufLength :  [in] The size of the output buffer
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
				 PrlStatUser_GetServiceName, (
		PRL_HANDLE hUserStat,
		PRL_STR	sServiceName,
		PRL_UINT32_PTR pnServiceNameBufLength
		) );

/* \Returns the host name of the client machine from which the
   session was initiated.
   Parameters
   hUserStat :            A handle of type
                          PHT_SYSTEM_STATISTICS_USER_SESSION
                          identifying the user session.
   sHostName :            [out] A pointer to a buffer that
                          receives the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnHostNameBufLength :  [in] The size of the output buffer (in
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
				 PrlStatUser_GetHostName, (
		PRL_HANDLE hUserStat,
		PRL_STR	sHostName,
		PRL_UINT32_PTR pnHostNameBufLength
		) );

/* \Returns session duration, in seconds.
   Parameters
   hUserStat :  A handle of type
                PHT_SYSTEM_STATISTICS_USER_SESSION identifying
                the user session.
   pnTime :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatUser_GetSessionTime, (
		PRL_HANDLE hUserStat,
		PRL_UINT64_PTR pnTime
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Disk statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

/* Determines the number of handles of type
   PHT_SYSTEM_STATISTICS_DISK contained in the specified
   PHT_SYSTEM_STATISTICS object. Each available disk drive in
   the system will have its own PHT_SYSTEM_STATISTICS_DISK
   object instance.
   Parameters
   hStat :    A handle of type PHT_SYSTEM_STATISTICS.
   pnCount :  [out] A pointer to a variable that receives the
              \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlStat_GetDiskStat                                        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetDisksStatsCount, (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		) );

/* Obtains a handle of type PHT_SYSTEM_STATISTICS_DISK
   containing the specified disk statistics.
   Parameters
   hStat :       A handle of type PHT_SYSTEM_STATISTICS.
   nIndex :      A numeric index identifying the disk statistics
                 object instance (beginning with 0). To get the
                 number of instances, use the
                 PrlStat_GetDisksStatsCount function.
   phDiskStat :  [out] A pointer to a variable that receives the
                 handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.d           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetDiskStat, (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phDiskStat
		) );

/* \Returns the disk device name.
   Parameters
   hDiskStat :              A handle of type
                            PHT_SYSTEM_STATISTICS_DISK
                            identifying the disk.
   sSystemName :            [out] A pointer to a buffer that
                            receives the disk name (a UTF\-8
                            encoded, null\-terminated string).
   pnSystemNameBufLength :  [in] The size of the output buffer
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
				 PrlStatDisk_GetSystemName, (
		PRL_HANDLE hDiskStat,
		PRL_STR	sSystemName,
		PRL_UINT32_PTR pnSystemNameBufLength
		) );

/* \Returns the size of the used space on the specified disk, in
   bytes.
   Parameters
   hDiskStat :  A handle of type PHT_SYSTEM_STATISTICS_DISK
                identifying the disk.
   pnSize :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatDisk_GetUsageDiskSpace, (
		PRL_HANDLE hDiskStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns free disk space, in bytes.
   Parameters
   hDiskStat :  A handle of type PHT_SYSTEM_STATISTICS_DISK
                identifying the disk.
   pnSize :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatDisk_GetFreeDiskSpace, (
		PRL_HANDLE hDiskStat,
		PRL_UINT64_PTR pnSize
		) );

/* Determines the number of the handles of type
   PHT_SYSTEM_STATISTICS_DISK_PARTITION contained in the
   specified PHT_SYSTEM_STATISTICS_DISK object. Each available
   partition will have its own
   PHT_SYSTEM_STATISTICS_DISK_PARTITION object instance.
   Parameters
   hDiskStat :  A handle of type PHT_SYSTEM_STATISTICS_DISK identifying the disk.  pnCount :    [out] A pointer to a variable that receives the \result.  Returns PRL_RESULT. Possible values: 
   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlStatDisk_GetPartStat                                      */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatDisk_GetPartsStatsCount, (
		PRL_HANDLE hDiskStat,
		PRL_UINT32_PTR pnCount
		) );

/* Obtains a handle of type PHT_SYSTEM_STATISTICS_DISK_PARTITION
   containing the specified disk partition statistics.
   Parameters
   hDiskStat :   A handle of type PHT_SYSTEM_STATISTICS_DISK
                 identifying the disk.
   nIndex :      A numeric index identifying the
                 PHT_SYSTEM_STATISTICS_DISK_PARTITION object
                 instance (beginning with 0). To obtain the
                 number of instances, use the PrlStatDisk_GetPartsStatsCount
                 function.
   phPartStat :  [out] A pointer to a variable that receives the
                 handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                        */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatDisk_GetPartStat, (
		PRL_HANDLE hDiskStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phPartStat
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Disk partition statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

/* \Returns disk partition device name.
   Parameters
   hPartStat :              A handle of type
                            PHT_SYSTEM_STATISTICS_DISK_PARTITION
                            identifying the disk partition.
   sSystemName :            [out] A pointer to a buffer that
                            receives the partition name (a UTF\-8
                            encoded, null\-terminated string).
   pnSystemNameBufLength :  [in] The size of the output buffer
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
				 PrlStatDiskPart_GetSystemName, (
		PRL_HANDLE hPartStat,
		PRL_STR	sSystemName,
		PRL_UINT32_PTR pnSystemNameBufLength
		) );

/* \Returns the size of the used space on the specified disk
   partition, in bytes.
   Parameters
   hPartStat :  A handle of type
                PHT_SYSTEM_STATISTICS_DISK_PARTITION identifying
                the disk partition.
   pnSize :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatDiskPart_GetUsageDiskSpace, (
		PRL_HANDLE hPartStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns the size of the free space on the specified disk
   partition, in bytes.
   Parameters
   hPartStat :  A handle of type
                PHT_SYSTEM_STATISTICS_DISK_PARTITION identifying
                the disk partition.
   pnSize :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatDiskPart_GetFreeDiskSpace, (
		PRL_HANDLE hPartStat,
		PRL_UINT64_PTR pnSize
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Process statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

/* Determines the number of handles of type
   PHT_SYSTEM_STATISTICS_PROCESS contained in the specified
   PHT_SYSTEM_STATISTICS object. Each system process will have
   its own PHT_SYSTEM_STATISTICS_PROCESS object instance.
   Parameters
   hStat :    A handle of type PHT_SYSTEM_STATISTICS.
   pnCount :  [out] A pointer to a variable that receives the
              \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlStat_GetProcStat                                         */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetProcsStatsCount, (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		) );

/* Obtains a handle of type PHT_SYSTEM_STATISTICS_PROCESS
   containing the specified system process statistics.
   Parameters
   hStat :       A handle of type PHT_SYSTEM_STATISTICS.
   nIndex :      A numeric index identifying the system process
                 statistics object instance (beginning with 0).
                 To determine the number of instances, use the
                 PrlStat_GetProcsStatsCount function.
   phProcStat :  [out] A pointer to a variable that receives the
                 handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStat_GetProcStat, (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phProcStat
		) );

/* \Returns the process command name.
   Parameters
   hProcStat :               A handle of type
                             PHT_SYSTEM_STATISTICS_PROCESS
                             identifying the system process.
   sCommandName :            [out] A pointer to a buffer that
                             receives the result (a UTF\-8
                             encoded, null\-terminated string).
   pnCommandNameBufLength :  [in] The size of the output buffer
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
				 PrlStatProc_GetCommandName, (
		PRL_HANDLE hProcStat,
		PRL_STR	sCommandName,
		PRL_UINT32_PTR pnCommandNameBufLength
		) );

/* \Returns process system id.
   Parameters
   hProcStat :  A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                identifying the system process.
   pnId :       [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetId, (
		PRL_HANDLE hProcStat,
		PRL_UINT32_PTR pnId
		) );

/* \Returns process owner user name.
   Parameters
   hProcStat :             A handle of type
                           PHT_SYSTEM_STATISTICS_PROCESS
                           identifying the system process.
   sOwnerName :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
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

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetOwnerUserName, (
		PRL_HANDLE hProcStat,
		PRL_STR	sOwnerName,
		PRL_UINT32_PTR pnOwnerNameBufLength
		) );

/* \Returns total memory usage size in bytes.
   Parameters
   hProcStat :  A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                identifying the system process.
   pnSize :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetTotalMemUsage, (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns real memory usage size in bytes.
   Parameters
   hProcStat :  A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                identifying the system process.
   pnSize :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetRealMemUsage, (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns virtual memory usage size in bytes.
   Parameters
   hProcStat :  A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                identifying the system process.
   pnSize :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetVirtMemUsage, (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns start time of the process, in seconds (number of
   seconds since January 1, 1601 (UTC)).
   Parameters
   hProcStat :  A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                identifying the system process.
   pnTime :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetStartTime, (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnTime
		) );

/* \Returns total running time of the process, in seconds.
   Parameters
   hProcStat :  A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                identifying the system process.
   pnTime :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetTotalTime, (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnTime
		) );

/* \Returns user running time of the process, in seconds.
   Parameters
   hProcStat :  A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                identifying the system process.
   pnTime :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetUserTime, (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnTime
		) );

/* \Returns system running time of the process, in seconds.
   Parameters
   hProcStat :  A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                identifying the system process.
   pnTime :     [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetSystemTime, (
		PRL_HANDLE hProcStat,
		PRL_UINT64_PTR pnTime
		) );

/* \Returns process state.
   Parameters
   hProcStat :  A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                identifying the system process.
   pnState :    [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetState, (
		PRL_HANDLE hProcStat,
		PRL_PROCESS_STATE_TYPE_PTR pnState
		) );

/* \Returns process CPU usage in percents.
   Parameters
   hProcStat :   A handle of type PHT_SYSTEM_STATISTICS_PROCESS
                 identifying the system process.
   pnPercents :  [out] A pointer to a variable that receives the
                 \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_1,
				 PrlStatProc_GetCpuUsage, (
				PRL_HANDLE hProcStat,
				PRL_UINT32_PTR pnPercents
				) );

///////////////////////////////////////////////////////////////////////////////
/// @section VM data statistic functionality.
///////////////////////////////////////////////////////////////////////////////

/* Obtains a handle of type PHT_SYSTEM_STATISTICS_VM_DATA
   containing the specified system VM data statistic.
   Parameters
   hStat :         A handle of type PHT_SYSTEM_STATISTICS.
   phVmDataStat :  [out] A pointer to a variable that receives
                   the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
				 PrlStat_GetVmDataStat, (
		PRL_HANDLE hStat,
		PRL_HANDLE_PTR phVmDataStat
		) );

/* \Returns VM data segment capacity in bytes.
   Parameters
   hVmDataStat :  A handle of type PHT_SYSTEM_STATISTICS_VM_DATA
                  identifying the system process.
   nSegment :     Segment id defined in PRL_DATA_STATISTIC_SEGMENTS enum.
   pnCapacity :   [out] A pointer to a variable that receives the
                  \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - no data for given segment.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( VIRTUOZZO_API_VER_4,
				 PrlStatVmData_GetSegmentCapacity, (
				PRL_HANDLE hVmDataStat,
				PRL_DATA_STATISTIC_SEGMENTS nSegment,
				PRL_UINT64_PTR pnCapacity
				) );

#endif //__VIRTUOZZO_API_DEPRECATED_H__
