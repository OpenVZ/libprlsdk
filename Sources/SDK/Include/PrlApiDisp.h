/*
 * PrlApiDisp.h
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


#ifndef __PARALLELS_API_DISPATCHER_H__
#define __PARALLELS_API_DISPATCHER_H__

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

///////////////////////////////////////////////////////////////////////////////
/// @section Generic API methods for the client
///
/// This section consists of methods that are used for general
/// manipulation of the parallels api library and are not specific
/// to objects in the object hierarchy.
///////////////////////////////////////////////////////////////////////////////

#include "PrlApiVersion.h"


/* Creates a new handle of type PHT_SERVER. In order to
   establish a connection with a Parallels Service, a handle of
   type PHT_SERVER must be created and the object that the
   handle references must be populated with the Parallels Service
   connection information. Once a connection is established, the
   handle can be used to access the Parallels Service. It is the
   responsibility of the caller to free the handle with the
   PrlHandle_Free call when it is no longer needed.
   Parameters
   handle :  [out] A pointer to a variable that receives the
             handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlSrv_LoginLocal

   PrlSrv_Login                                                  */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrv_Create, (
		PRL_HANDLE_PTR handle
		) );

/* Performs a remote login operation using the specified
   \parameters. A remote login operation logs the user to a
   Parallels Service running on a remote host. You can also use
   this function to log to a local host or you can use the
   PrlSrv_LoginLocal function, which is a simplified version
   created specifically to perform local host logins.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_LOGIN_RESPONSE.
   Parameters
   hServer :           A handle of type PHT_SERVER identifying
                       the Parallels Service.
   host :              The name or IP address of the host machine (a UTF\-8
                       encoded, null\-terminated string).
   user :              User name (a UTF\-8 encoded,
                       null\-terminated string).
   passwd :            Password (a UTF\-8 encoded,
                       null\-terminated string).
   sPrevSessionUuid :  Previous session ID. This is an optional
                       parameter that can be used in recovering
                       from a lost Parallels Service connection. The ID will
                       be used to restore references to the tasks
                       that were initiated in the previous
                       session and are still running inside the Parallels Service.
                       You can pass a null or an empty
                       string value if you are not restoring any
                       references. See PrlSrv_AttachToLostTask
                       for more information.
   port_cmd :          Port number. Pass 0 (zero) to use the
                       default port.
   timeout :           Timeout value in milliseconds. The
                       operation will be automatically
                       interrupted if a connection is not
                       established within this timeframe. Specify
                       0 (zero) for infinite timeout.
   security_level :    Communication security level to use for
                       this connection. The security level set
                       here will be used for all communications
                       with the Parallels Service for the duration of this
                       session. The minimum allowable security
                       level can be determined using the PrlDispCfg_GetMinSecurityLevel
                       function.
   flags :             A bitset of flags.
                       Allowed next values:

                       0 - no flags. It's behaviour by default ( as PrlSrv_Login() )
                       PACF_NON_INTERACTIVE_MODE - to use
					 non-interactive mode. In interactive mode,
					 a client may receive questions from the
					 Parallels Service, which it is expected
					 to answer in order for the operation to
					 continue. In non\-interactive mode, the
					 Parallels Service will make decisions
					 on its own.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation, including the return code and a
   handle of type PHT_LOGIN_RESPONSE, containing an additional
   Parallels Service information.

   PRL_INVALID_HANDLE if there's not enough memory to
   instantiate the job object.
   See Also
   PrlDispCfg_GetMinSecurityLevel                                                       */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_Login, (
		PRL_HANDLE hServer,
		PRL_CONST_STR host,
		PRL_CONST_STR user,
		PRL_CONST_STR passwd,
		PRL_CONST_STR sPrevSessionUuid,
		PRL_UINT32 port_cmd,
		PRL_UINT32 timeout,
		PRL_SECURITY_LEVEL security_level
		) );

PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
					PrlSrv_LoginEx, (
		PRL_HANDLE hServer,
		PRL_CONST_STR host,
		PRL_CONST_STR user,
		PRL_CONST_STR passwd,
		PRL_CONST_STR sPrevSessionUuid,
		PRL_UINT32 port_cmd,
		PRL_UINT32 timeout,
		PRL_SECURITY_LEVEL security_level,
		PRL_UINT32 flags
		) );

/* Allows to log in to the local Parallels Service (the Parallels
   Service running on this host) using the current user
   credentials. When performing a local login operation, no
   password/username is needed. The authentication and
   authorization will be performed using the current user
   account.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_LOGIN_RESPONSE.
   Note
   Parameters
   hServer :           A handle of type PHT_SERVER identifying
                       the Parallels Service.
   sPrevSessionUuid :  Previous session ID. This is an optional
                       parameter that can be used in recovering
                       from a lost Parallels Service connection. The ID will
                       be used to restore references to the tasks
                       that were initiated in the previous
                       session and are still running inside the
                       Parallels Service. You can pass a null or an empty
                       string value if you are not restoring any
                       references. See PrlSrv_AttachToLostTask
                       for more information.
   port :              Port number. Specify 0 (zero) to use the
                       default port.
   security_level :    Communication security level to use for
                       this connection. The security level set
                       here will be used for all communications
                       with the Parallels Service for the duration of this
                       session. The minimum allowable security
                       level can be determined using the PrlDispCfg_GetMinSecurityLevel
                       function.
   flags :             A bitset of flags.
                       Allowed next values:

                       0 - no flags. It's behaviour by default ( as PrlSrv_LoginLocal() )
                       PACF_NON_INTERACTIVE_MODE - to use
					 non-interactive mode. In interactive mode,
					 a client may receive questions from the
					 Parallels Service, which it is expected
					 to answer in order for the operation to
					 continue. In non\-interactive mode, the
					 Parallels Service will make decisions
					 on its own.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation, including the return code and a
   handle of type PHT_LOGIN_RESPONSE, containing an additional
   Parallels Service information.

   PRL_INVALID_HANDLE if there's not enough memory to
   instantiate the job object.
   See Also
   PrlDispCfg_GetMinSecurityLevel                                                       */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_LoginLocal, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sPrevSessionUuid,
		PRL_UINT32 port,
		PRL_SECURITY_LEVEL security_level
		) );

PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						   PrlSrv_LoginLocalEx, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sPrevSessionUuid,
		PRL_UINT32 port,
		PRL_SECURITY_LEVEL security_level,
		PRL_UINT32 flags
		) );

/* Logs off the Parallels Service. The function logs of the Parallels Service and ends
   a session that was started with the PrlSrv_Login or the
   PrlSrv_LoginLocal function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_Logoff, (
		PRL_HANDLE hServer
		) );

/* Registers an event handler. Asynchronous functions return
   data to the caller by means of a callback function (or event
   handler). The callback function could be called at any time,
   depending on how long the asynchronous function takes to
   complete. The callback function must have a specific
   signature. The callback function prototype type is
   PRL_EVENT_HANDLER_PTR. The PrlSrv_RegEventHandler function is
   used to register an event handler with the specified Parallels Service,
   PrlSrv_UnregEventHandler is used to unregister an event
   handler. When an event handler is registered, it will start
   receiving all events/jobs generated by the Parallels Service. It is the
   responsibility of the client application to identify and
   handle the relevant event(s). For more information on events
   and asynchronous functions, see the Parallels Virtualization SDK
   Programmer's Guide.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Parallels Service.
   handler :   A pointer to a user callback function (event
               handler).
   userData :  Optional. A pointer to a user data that will be
               passed to the callback function.                  */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrv_RegEventHandler, (
		PRL_HANDLE hServer,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		) );

/* Unregisters the specified event handler (callback function)
   that was previously registered with the
   PrlSrv_RegEventHandler function. When using callback
   functionality with asynchronous functions, a callback
   function is first registered with Parallels Service. When no
   longer needed (client doesn't want to receive events
   anymore), the function must by unregistered using the
   PrlSrv_UnregEventHandler function. The values of the handler
   and userData parameters constitute a unique pair which will
   be used to identify the event handler that will be
   unregistered. Make sure that you supply the same values as
   the ones you used when you registered the handler. See
   PrlSrv_SubscribeToHostStatistics for the complete example
   illustrating event handlers usage.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Parallels Service.
   handler :   A pointer to the callback function (event handler)
               to unregister. This must be the same pointer that
               was passed to the PrlSrv_RegEventHandler when the
               event handler was registered with the virtual
               machine.
   userData :  A pointer to the user data. This must also be the
               same pointer that was passed to the PrlSrv_RegEventHandler
               when the event handler was registered with the
               virtual machine.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                     */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrv_UnregEventHandler, (
		PRL_HANDLE hServer,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		) );

/**
The PrlSrv_GetQuestions function allows to synchronously receive questions from
a Parallels Service. It can be used as an alternative to asynchronous question
processing through events. You can use this function at any time to check
if a question is awaiting the client response on the Parallels Service.

Parameters
handle : A handle of type PHT_SERVER.
pQuestionsList :	[out] A pointer to a handle of type PHT_HANDLES_LIST containing a
					list of available questions.

Returns
PRL_RESULT. Possible values:
PRL_ERR_INVALID_ARG -	non valid PHT_SERVER handle or non valid (NULL) pointer
						to the buffer for storing result was passed.
PRL_ERR_OUT_OF_MEMORY - not enough memory to complete the operation.
PRL_ERR_SUCCESS - operation completed successfully.
*/
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrv_GetQuestions, (
		PRL_HANDLE hServer,
		PRL_HANDLE_PTR pQuestionsList
		) );

/**
   Sets the session in noninteractive or interactive mode.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   Parameters
   hServer :          A handle of type PHT_SERVER.
   bNonInteractive :  Noninteractive session mode.
   nFlags :           Reserved parameter

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_SetNonInteractiveSession, (
		PRL_HANDLE hServer,
		PRL_BOOL bNonInteractive,
		PRL_UINT32 nFlags
		) );

/**
   Determines if the session is noninteractive or interactive.

   Parameters
   hServer :           A handle of type PHT_SERVER.
   pbNonInteractive :  [out] A pointer to a variable that receives
                       the result. PRL_TRUE indicates that the
                       session is noninteractive. PRL_FALSE
                       indicates interactive session (default).

   Returns
   PRL_RESULT. Possible values:
   PRL_ERR_INVALID_ARG - non valid PHT_SERVER handle or non valid (NULL) pointer
						to the buffer for storing result was passed.
   PRL_ERR_SUCCESS - operation completed successfully.
*/
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrv_IsNonInteractiveSession, (
		PRL_HANDLE hServer,
		PRL_BOOL_PTR pbNonInteractive
		) );


/**
   Determines having restriction.

   Parameters
   hServer :           A handle of type PHT_SERVER.
   nRestrictionKey :   Restriction key value form PRL_LICENSE_RESTRICTION_KEY
                       enumeration.
   pbHasRestriction :  [out] A pointer to a variable that receives
                       the result. PRL_TRUE indicates that the
                       current license has the given restriction.

   Returns
   PRL_RESULT. Possible values:
   PRL_ERR_INVALID_ARG - non valid PHT_SERVER handle or non valid (NULL) pointer
                         to the buffer for storing result was passed.
   PRL_ERR_SUCCESS - operation completed successfully.
   See Also
      PrlSrv_GetRestrictionInfo
*/
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlSrv_HasRestriction, (
		PRL_HANDLE hServer,
		PRL_LICENSE_RESTRICTION_KEY nRestrictionKey,
		PRL_BOOL_PTR pbHasRestriction
		) );

/**
   Gets restriction info.

   Parameters
   hServer :           A handle of type PHT_SERVER.
   nRestrictionKey :   Restriction key value form PRL_LICENSE_RESTRICTION_KEY
                       enumeration.
   phEvtParam :        [out] A pointer to a variable that receives
                       the handle of PHT_EVENT_PARAMETER type.

   Returns
   PRL_RESULT. Possible values:
   PRL_ERR_INVALID_ARG - non valid PHT_SERVER handle or non valid (NULL) pointer
                         to the buffer for storing result was passed.
   PRL_ERR_NO_DATA - no info for the given restriction or restriction has boolean type.
   PRL_ERR_SUCCESS - operation completed successfully.
   See Also
      PrlSrv_HasRestriction
*/
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlSrv_GetRestrictionInfo, (
		PRL_HANDLE hServer,
		PRL_LICENSE_RESTRICTION_KEY nRestrictionKey,
		PRL_HANDLE_PTR phEvtParam
		) );


/**
   Disable administrator confirmation mode for the session.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   Parameters
   hServer :           A handle of type PHT_SERVER.
   sUser :             User name from administrator group (a UTF\-8 encoded, null\-terminated string).
   sPasswd :           Password of user (a UTF\-8 encoded, null\-terminated string).
   nFlags :            Reserved parameter

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
      PrlSrv_DisableConfirmationMode()
	  PrlSrv_EnableConfirmationMode()
      PrlSrv_IsConfirmationModeEnabled()
      PrlVmCfg_GetConfirmationsList()
      PrlVmCfg_SetConfirmationsList()
      PrlDispCfg_GetConfirmationsList()
      PrlDispCfg_SetConfirmationsList()
*/
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_DisableConfirmationMode, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sUser,
		PRL_CONST_STR sPasswd,
		PRL_UINT32 nFlags
		) );

/**
   Enable administrator confirmation mode for the session.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   Parameters
   hServer :           A handle of type PHT_SERVER.
   nFlags :           Reserved parameter

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
      PrlSrv_DisableConfirmationMode()
	  PrlSrv_EnableConfirmationMode()
      PrlSrv_IsConfirmationModeEnabled()
      PrlVmCfg_GetConfirmationsList()
      PrlVmCfg_SetConfirmationsList()
      PrlDispCfg_GetConfirmationsList()
      PrlDispCfg_SetConfirmationsList()
*/
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_EnableConfirmationMode, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );

/**
   Determines confirmation mode for the session

   Parameters
   hServer :           A handle of type PHT_SERVER.
   pbConfirmationModeEnabled :  [out] A pointer to a variable that receives
                       the result. PRL_TRUE indicates that confirmation mode
					   is enabled for the session. PRL_FALSE indicates that confirmation mode
					   is disabled for the session.
   Returns
   PRL_RESULT. Possible values:
   PRL_ERR_INVALID_ARG - non valid PHT_SERVER handle or non valid (NULL) pointer
						to the buffer for storing result was passed.
   PRL_ERR_SUCCESS - operation completed successfully.
   See Also
      PrlSrv_DisableConfirmationMode()
	  PrlSrv_EnableConfirmationMode()
      PrlSrv_IsConfirmationModeEnabled()
      PrlVmCfg_GetConfirmationsList()
      PrlVmCfg_SetConfirmationsList()
      PrlDispCfg_GetConfirmationsList()
      PrlDispCfg_SetConfirmationsList()
*/
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrv_IsConfirmationModeEnabled, (
		PRL_HANDLE hServer,
		PRL_BOOL_PTR pbConfirmationModeEnabled
		) );

/* Obtains a handle of type PHT_SERVER_CONFIG containing the
   host configuration information.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_SERVER_CONFIG containing the host
        configuration information.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_GetSrvConfig, (
		PRL_HANDLE hServer
		) );

/* \Returns a handle of type PHT_DISP_CONFIG containing the
   specified Parallels Service preferences information.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_DISP_CONFIG containing the Parallels Service preferences.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_GetCommonPrefs, (
		PRL_HANDLE hServer
		) );

/* The PrlSrv_CommonPrefsBeginEdit function is used in modifying
   Parallels Service preferences (see PHT_DISP_CONFIG). The
   PrlSrv_CommonPrefsBeginEdit and the PrlSrv_CommonPrefsCommit
   functions are used to detect collisions with other clients
   trying to modify preferences of the same Parallels Service.
   The PrlSrv_CommonPrefsBeginEdit call timestamps the beginning
   of the operation. It does not lock the Parallels Service, so other
   clients can modify the same Parallels Service settings at the same time. When you
   are done making the changes, you must call the
   PrlSrv_CommonPrefsCommit function to apply them to the
   Parallels Service. The function will verify that the Parallels Service preferences
   have not been modified by another client. If a collision is
   detected, your changes will be rejected.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_ACCESS_DENIED - current user does not have sufficient
   rights to perform this operation.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_CommonPrefsBeginEdit, (
		PRL_HANDLE hServer
		) );

/* The PrlSrv_CommonPrefsCommit function is used in modifying
   Parallels Service preferences (see PHT_DISP_CONFIG). The
   PrlSrv_CommonPrefsBeginEdit and the PrlSrv_CommonPrefsCommit
   functions are used to detect collisions with other clients
   trying to modify preferences of the same Parallels Service.
   The PrlSrv_CommonPrefsBeginEdit call timestamps the beginning
   of the operation. It does not lock the Parallels Service, so other
   clients can modify the same Parallels Service at the same time. When you
   are done making the changes, you must call the
   PrlSrv_CommonPrefsCommit function to apply them to the
   Parallels Service. The function will verify that the Parallels Service preferences
   have not been modified by another client. If a collision is
   detected, your changes will be rejected.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Parallels Service.
   hDispCfg :  A handle of type PHT_DISP_CONFIG containing the
               Parallels Service preferences.
   nFlags :    Reserved flags
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_CommonPrefsCommit, (
		PRL_HANDLE hServer,
		PRL_HANDLE hDispCfg
		) );
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_4,
						   PrlSrv_CommonPrefsCommitEx, (
		PRL_HANDLE hServer,
		PRL_HANDLE hDispCfg,
		PRL_UINT32 nFlags
		) );

/* Obtains a handle of type PHT_USER_PROFILE containing the
   profile data of the currently logged in user.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_USER_PROFILE.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_GetUserProfile, (
		PRL_HANDLE hServer
		) );


/* Obtains a list of handles of type PHT_USER_INFO containing
   information about all known users. To get the return code
   from the PHT_JOB object, use the PrlJob_GetRetCode function.
   Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParamByIndex function to obtain a handle
   of type PHT_USER_INFO containing an individual user
   information.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_GetUserInfoList, (
		PRL_HANDLE hServer
		) );

/* Obtains a handle of type PHT_USER_INFO containing information
   about the specified user. To get the return code from the
   PHT_JOB object, use the PrlJob_GetRetCode function. Possible
   values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParam function to obtain a handle of
   type PHT_USER_INFO containing the user information.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   sUserId :  UUID of the user to obtain the information for.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_GetUserInfoList                                        */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_GetUserInfo, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sUserId
		) );


/* Configures the PCI device assignment. For more information
   about generic PCI devices and their usage, see the
   PHT_HW_GENERIC_PCI_DEVICE handle description.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Parallels Service.
   hDevList :  A handle of type PHT_HANDLES_LIST containing a
               list of handles of type PHT_HW_GENERIC_PCI_DEVICE.
               Each handle in the list identifies an individual
               PCI device installed on the host computer.
   nFlags :    Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PHT_HW_GENERIC_PCI_DEVICE                                      */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_ConfigureGenericPci, (
		PRL_HANDLE hServer,
		PRL_HANDLE hDevList,
		PRL_UINT32 nFlags
		) );


/* Timestamps the beginning of the user profile editing
   operation. Every modification to the user profile (see
   PHT_USER_PROFILE) must begin with this function call. The
   function, together with the PrlSrv_UserProfileCommit
   function, is used to detect collisions with other clients
   trying to modify the profile of the same user at the same
   time. The function does not lock the profile being edited. If
   a collision is detected on commit, the changes are rejected.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_ACCESS_DENIED - the user does not have sufficient
   rights to perform this task.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_UserProfileBeginEdit, (
		PRL_HANDLE hServer
		) );

/* Saves (commits) user profile changes to the Parallels Service. Every
   modification to the user profile (see PHT_USER_PROFILE) must
   begin with the PrlSrv_UserProfileBeginEdit function call. The
   function, together with the PrlSrv_UserProfileCommit
   function, is used to detect collisions with other client
   trying to modify the profile of the same user at the same
   time. The function does not lock the profile being edited. If
   a collision is detected on commit, the changes are rejected.
   Otherwise, the changes are saved to the Parallels Service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :       A handle of type PHT_SERVER identifying the
                   Parallels Service.
   hUserProfile :  A handle of type PHT_USER_PROFILE containing
                   the user information.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_UserProfileCommit, (
		PRL_HANDLE hServer,
		PRL_HANDLE hUserProfile
		) );

/* Determines if the connection to the specified Parallels Service is
   active.
   Parameters
   hServer :              A handle of type PHT_SERVER identifying
                          the Parallels Service.
   pbConnected :          [out] A pointer to a variable that
                          receives the result. PRL_TRUE indicates
                          that the connection is active.
                          PRL_FALSE indicates otherwise.
   pbIsConnectionLocal :  [out] Specifies whether this is a local
                          or remote connection. PRL_TRUE
                          indicates that this is a local
                          connection. PRL_FALSE indicates that
                          this is a remote connection.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrv_IsConnected, (
		PRL_HANDLE hServer,
		PRL_BOOL_PTR pbConnected,
		PRL_BOOL_PTR pbIsConnectionLocal
		) );

/* Determines if the connection to the specified Parallels Service is
   a proxy connection.
   Parameters
   hServer :              A handle of type PHT_SERVER identifying
                          the Parallels Service.
   pbProxyConnection :    [out] A pointer to a variable that
                          receives the result. PRL_TRUE indicates
                          that the connection is proxy.
                          PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlSrv_IsProxyConnection, (
		PRL_HANDLE hServer,
		PRL_BOOL_PTR pbProxyConnection
		) );

/* Obtains a handle of type PHT_SERVER_INFO containing the host
   computer information. Please note that in order to obtain the
   complete host info, the function must be called after
   the login operation is completed and the results of the
   login operation are received. In all other situations, some
   of the Parallels Service properties may not be populated.
   Parameters
   hServer :       A handle of type PHT_SERVER identifying the
                   Parallels Service.
   phServerInfo :  [out] A pointer to a variable that receives
                   a handle to the server info object.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrv_GetServerInfo, (
		PRL_HANDLE hServer,
		PRL_HANDLE_PTR phServerInfo
		) );


/* Shuts down the Parallels Service. The
   current user must have sufficient privileges to execute the
   Parallels Service shutdown.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_ACCESS_TOKEN_INVALID - the user doesn't have
   sufficient privileges.

   PRL_ERR_ANOTHER_USER_SESSIONS_PRESENT - another user is
   currently connected to the Parallels Service, the Parallels Service shutdown
   aborted.

   PRL_ERR_SOME_VMS_RUNNING - running virtual machines found,
   stop the running machines first.

   PRL_ERR_SOME_TASKS_PRESENT - running tasks detected, wait for
   the tasks to finish or cancel them.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   bForceShutdown : Specifies whether the shutdown operation should be forced. The value of
                    PRL_TRUE will force the shutdown; PRL_FALSE will perform a
                    normal shutdown. If one or more virtual machines are running, clients
                    are connected, or some tasks are currently in progress, then forcing the
                    shutdown will stop all processes automatically and shut down the machine.
   nFlags:          A flags to select shutdown mode( from enum PRL_SRV_SHUTDOWN_FLAGS )
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_Shutdown, (
		PRL_HANDLE hServer,
		PRL_BOOL bForceShutdown
		) );
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						  PrlSrv_ShutdownEx, (
						  PRL_HANDLE hServer,
						  PRL_UINT32 nFlags
						  ) );

///////////////////////////////////////////////////////////////////////////////
/// @section Parallels remote file system operations
///////////////////////////////////////////////////////////////////////////////

/* \Returns a list of root directories on the host computer.
   On Windows, the function returns a list of
   PHT_REMOTE_FILESYSTEM_INFO objects containing the disk volume
   information (e.g. C:\\, D:\\, etc.). On other operating
   systems, it returns a single object containing the root
   directory.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid server handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamsCount to get the number of
        results.
     3. Use the PrlResult_GetParamByIndex function to get
        individual results.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_FsGetDiskList, (
		PRL_HANDLE hServer
		) );


/* Retrieves information about the specified file system entry
   residing on the specified host computer. The information
   retrieved by the PrlSrv_FsGetDirEntries function includes the
   information about the specified entry and, if the entry is a
   directory, the information about its immediate child entries
   (files and subdirectories). On Windows, you can also specify
   a drive letter, in which case the information about the root
   directory entries will be retrieved.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid server handle was passed or
   invalid path was specified.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_REMOTE_FILESYSTEM_INFO containing the file system
        entry information.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   path :     An absolute directory name and path, or drive
              letter (e.g. C\:\\) on the host computer for which to
              get the information.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                       */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_FsGetDirEntries, (
		PRL_HANDLE hServer,
		PRL_CONST_STR path
		) );


/* Creates a directory in the specified location on the host.
   If you are creating a series of directories that
   relate to each other, you can use the
   PrlSrv_FsGenerateEntryName function to automatically generate
   their names.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid server handle was passed or
   invalid path was specified.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   path :     The new directory name and path.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_FsCreateDir, (
		PRL_HANDLE hServer,
		PRL_CONST_STR path
		) );


/* Removes a file system entry from the host computer.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   path :     An absolute path to the file system entry to be
              removed.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_FsRemoveEntry, (
		PRL_HANDLE hServer,
		PRL_CONST_STR path
		) );

/* Determines if the current user has sufficient rights to
   create a file or a directory in the specified location on the
   host.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid server handle was passed or
   invalid path was specified.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   path :     Full path to the intended file or directory.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_FsCanCreateFile, (
					  PRL_HANDLE hServer,
					  PRL_CONST_STR path
					  ) );



/* Renames a file system entry on the host.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   oldPath :  An absolute path to the entry to be renamed.
   newPath :  An absolute path including the new file system entry
              name.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                    */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_FsRenameEntry, (
		PRL_HANDLE hServer,
		PRL_CONST_STR oldPath,
		PRL_CONST_STR newPath
		) );

/* Automatically generates a unique name for a new directory.
   The function allows to generate directory names using a
   specified prefix and optionally a suffix. You can specify
   your own prefix or use the default one. The function will not
   create the directories on the host. To create the
   directories, use the PrlSrv_FsCreateDir function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid server handle was passed or
   invalid path was specified.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamAsString function to obtain
        the file system entry name from the PHT_RESULT object. The
        directory name is returned as a UTF-8 encoded,
        null-terminated string.
   Parameters
   hServer :          A handle of type PHT_SERVER identifying the
                      Parallels Service.
   sDirPath :         A full path to the target location
                      (directory).
   sFilenamePrefix :  Specifies a prefix to use in the directory
                      name. Pass null or empty string to use the
                      default prefix\: tmpfile. The name will be
                      generated by adding a number to the prefix
                      (e.g. tmpfile1, tmpfile2, tmpfile3, etc.)
   sFilenameSuffix :  Specifies a suffix to use in the directory
                      name. Pass null or an empty string value if
                      you don't want to use a suffix.
   sIndexDelimiter :  A character(s) that will be inserted
                      between the prefix and the index (e.g.
                      tmpfile_1, tmpfile_2, tmpfile_3, etc). You
                      can specify a null pointer or an empty
                      string if you don't want to insert any
                      extra characters there.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                    */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_FsGenerateEntryName, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sDirPath,
		PRL_CONST_STR sFilenamePrefix,
		PRL_CONST_STR sFilenameSuffix,
		PRL_CONST_STR sIndexDelimiter
		) );

/* Installs Parallels license on the specified Parallels Service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.
   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
   1. Use the PrlJob_GetResult function to obtain a handle to
   the PHT_RESULT object.
   2. Use the PrlResult_GetParam function to obtain a handle
   of type PHT_LICENSE containing the license information.

   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Parallels Service.
   sKey :      License key.
   sUser :     License user name.
   sCompany :  License company name.
   nFlags:     Flags ( from enums PRL_UPDATE_LICENSE_FLAGS + PRL_API_COMMAND_FLAGS)

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                */

PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_UpdateLicense, (
							 PRL_HANDLE hServer,
							 PRL_CONST_STR sKey,
							 PRL_CONST_STR sUser,
							 PRL_CONST_STR sCompany
							 ) );
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						  PrlSrv_UpdateLicenseEx, (
						  PRL_HANDLE hServer,
						  PRL_CONST_STR sKey,
						  PRL_CONST_STR sUser,
						  PRL_CONST_STR sCompany,
						  PRL_UINT32 nFlags
							 ) );

/* \Returns an object of type PHT_LICENSE containing the
   Parallels license information.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_LICENSE containing the license information.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_GetLicenseInfo, (
							 PRL_HANDLE hServer
							 ) );

/* Sends an answer to the Parallels Service in response to a question. Some
   operations have an option to be carried out in interactive
   mode. This means that the Parallels Service may interrupt an operation
   and ask client a question about how it would like to proceed.
   This function allows to send such an answer to the Parallels Service.
   The answer must be prepared using the
   PrlEvent_CreateAnswerEvent function before it can be sent to
   the Parallels Service using this function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   hAnswer :  A handle of type PHT_EVENT containing the answer.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_SendAnswer, (
		PRL_HANDLE hServer,
		PRL_HANDLE hAnswer
		) );


/* Refreshes states of installed parallels plugins.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :       A handle of type PHT_SERVER identifying the
                   Parallels Service.
   nFlags :        Reserved parameter
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_4,
						   PrlSrv_RefreshPlugins, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );


/* Gets plugin list with plugin info entries.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   sClassId : [optional] Class GUID in text format.
   nFlags :   Reserved parameter
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_4,
						   PrlSrv_GetPluginsList, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sClassId,
		PRL_UINT32 nFlags
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Parallels Problem Report operations
///////////////////////////////////////////////////////////////////////////////

/* Allows to receive a problem report from the Parallels Service
   in the event of a virtual machine operation failure.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get a report from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamAsString function to obtain a
        string value containing the report.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_GetProblemReport, (
					  PRL_HANDLE hServer
					  ) );

/* Allows to receive a packed problem report from the Parallels Service
in the event of a virtual machine operation failure.

To get the return code from the PHT_JOB object, use the
PrlJob_GetRetCode function. Possible values are:

PRL_ERR_INVALID_ARG - invalid handle was passed.

PRL_ERR_SUCCESS - function completed successfully.

To get a report from the PHT_JOB object:
1. Use the PrlJob_GetResult function to obtain a handle to
the PHT_RESULT object.
2. Use the PrlResult_GetParamAsString function to obtain a
string value containing the packed report dir path.
Parameters
hServer :  A handle of type PHT_SERVER identifying the Parallels
Service.
PRL_UINT32 nFlags - reserved flags parameter
Returns
A handle of type PHT_JOB containing the results of this
asynchronous operation or PRL_INVALID_HANDLE if there's not
enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						  PrlSrv_GetPackedProblemReport, (
						  PRL_HANDLE hServer,
						  PRL_UINT32 nFlags
						  ) );


/* Requests a server to send a problem report with possibility
   to append also the problem report assemblied at the client's side.

   The report should be created as follows:
    1) with PrlApi_CreateProblemReport call
    2) filled with PrlReport_Assembly

   Similar calls are PrlApi_SendProblemReport and PrlApi_SendPackedProblemReport,
   PrlReport_Send

   Async response should normally contain report id.

   Parameters:
   hServer - handle of type PHT_SERVER of the server
   hProblemReport - the report assemblied at the client's side
   nFlags - flags associated with the operation (see PRL_PROBLEM_REPORT_FLAGS)

    Returns:
    PRL_HANDLE - handle to the job generated					*/
PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_5,
                 PrlSrv_SendProblemReport, (
        PRL_HANDLE hServer,
        PRL_HANDLE hProblemReport,
        PRL_UINT32 nFlags
        ) );


/* The PrlSrv_AttachToLostTask function allows to obtain a
   handle to a running task after the connection to the Parallels Service
   was lost. If you've started a long asynchronous operation
   and then lost a connection with the Parallels Service,
   you may use this function to get a handle to the task after
   reconnecting to the Parallels Service. In order to do that, log in to
   the Parallels Service using the PrlSrv_Login or the PrlSrv_LoginLocal
   function passing the UUID of the session that you've lost.
   The returned PHT_LOGIN_RESPONSE object will contain the list
   of the tasks that belong to the lost session and are still
   running. Select a task that you want to get a handle to and
   pass its ID to the PrlSrv_AttachToLostTask function. The
   function will return a handle of type PHT_JOB which you may
   use to continue controlling and monitoring the task as usual.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   sTaskId :  The ID of the task to get a job handle for.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation, or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object. The returned
   PHT_JOB object will be attached to the specified task.        */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_AttachToLostTask, (
					  PRL_HANDLE hServer,
					  PRL_CONST_STR sTaskId
					  ) );


/* Checks whether specified feature supported by remote side.
   Please note that actual data presents just after login to
   server operation was completed.

   Parameters
   hServer :  A handle of type PHT_SERVER.
   nFeatureId : Requesting feature identifier
   pbIsSupported : pointer to the buffer to retrieve result

   Returns
   	PRL_ERR_INVALID_ARG - either invalid/wrong handle or null pointer was passed.
	PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlSrv_IsFeatureSupported, (
		PRL_HANDLE hServer,
		PRL_FEATURES_MATRIX nFeatureId,
		PRL_BOOL_PTR pbIsSupported
		) );


/* Get available disk space for path.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParamAsString function to obtain a result as string
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   sPath :    Path as pointer of type PRL_CONST_STR_PTR.
   nFlags :   Flags
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
			PrlSrv_GetDiskFreeSpace, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sPath,
		PRL_UINT32 nFlags
		) );


/* Set VNC encryption.
   To delete sertificate the empty sPubKey and sPrivKey should be specified.
   Parameters
   hHandle : A handle of type PHT_SERVER.
   sPubKey : SSL certificate to encrypt the VNC traffic
   sPrivKey: SSL private key to encrypt the VNC traffic
   Returns
	A handle of type PHT_JOB containing the results of operation.
*/
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_5,
				PrlSrv_SetVNCEncryption, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sPubKey,
		PRL_CONST_STR sPrivKey,
		PRL_UINT32 nFlags
		));


///////////////////////////////////////////////////////////////////////////////
/// @section Remote file system info object manipulations
///////////////////////////////////////////////////////////////////////////////

/* Determines the basic type of the specified file system entry.
   Parameters
   hFsInfo :   A handle of type PHT_REMOTE_FILESYSTEM_INFO
               identifying the remote file system entry.
   pnFsType :  [out] A pointer to a variable that receives the
               \file entry type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsInfo_GetType, (
		PRL_HANDLE hFsInfo,
		PRL_FILE_SYSTEM_TYPE_PTR pnFsType
		) );

/* Determines the file system type of the specified file system entry.
   Parameters
   hFsInfo :   A handle of type PHT_REMOTE_FILESYSTEM_INFO
               identifying the remote file system entry.
   pnFsType :  [out] A pointer to a variable that receives the
               \file entry type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlFsInfo_GetFsType, (
		PRL_HANDLE hFsInfo,
		PRL_FILE_SYSTEM_FS_TYPE_PTR pnFsType
		) );

/* Determines the number of child entries for the specified
   remote file system entry.
   Parameters
   hFsInfo :              A handle of type
                          PHT_REMOTE_FILESYSTEM_INFO identifying
                          the remote file system entry.
   pnChildEntriesCount :  [out] A pointer to a variable that
                          receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsInfo_GetChildEntriesCount, (
		PRL_HANDLE hFsInfo,
		PRL_UINT32_PTR pnChildEntriesCount
		) );


/* Obtains a handle of type PHT_REMOTE_FILESYSTEM_ENTRY
   containing a child entry information.
   Parameters
   hFsInfo :       A handle of type PHT_REMOTE_FILESYSTEM_INFO
                   identifying the parent file system entry.
   nIndex :        A child entry index. To get the total number
                   of child entries, use the
                   PrlFsInfo_GetChildEntriesCount function.
   phChildEntry :  [out] A pointer to a variable that receives
                   the child entry handle. The handle type is
                   PHT_REMOTE_FILESYSTEM_ENTRY.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsInfo_GetChildEntry, (
		PRL_HANDLE hFsInfo,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phChildEntry
		) );


/* Obtains a handle of type PHT_REMOTE_FILESYSTEM_ENTRY to the
   parent file system entry.
   Parameters
   hFsInfo :        A handle of type PHT_REMOTE_FILESYSTEM_INFO
                    identifying the remote file system entry.
   phParentEntry :  [out] A pointer to a variable that receives
                    the handle. The handle type is PHT_REMOTE_FILESYSTEM_ENTRY.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsInfo_GetParentEntry, (
		PRL_HANDLE hFsInfo,
		PRL_HANDLE_PTR phParentEntry
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Remote FS entry operations
///////////////////////////////////////////////////////////////////////////////

/* \Returns the specified file system entry absolute path.
   Parameters
   hFsEntry :                 A handle of type
                              PHT_REMOTE_FILESYSTEM_ENTRY
                              identifying the file system entry.
   sAbsolutePath :            [out] A pointer to a buffer that
                              receives the path (a UTF\-8
                              encoded, null\-terminated string).
   pnAbsolutePathBufLength :  [in] The size of the output buffer
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsEntry_GetAbsolutePath, (
		PRL_HANDLE hFsEntry,
		PRL_STR sAbsolutePath,
		PRL_UINT32_PTR pnAbsolutePathBufLength
		) );

/* \Returns the file system entry relative name.
   Parameters
   hFsEntry :                 A handle of type
                              PHT_REMOTE_FILESYSTEM_ENTRY
                              identifying the file system entry.
   sRelativeName :            [out] A pointer to a buffer that
                              receives the name (a UTF\-8
                              encoded, null\-terminated string).
   pnRelativeNameBufLength :  [in] The size of the output buffer
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsEntry_GetRelativeName, (
		PRL_HANDLE hFsEntry,
		PRL_STR sRelativeName,
		PRL_UINT32_PTR pnRelativeNameBufLength
		) );

/* \Returns the date on which the specified file system entry
   was last modified. Returned time is a local
   time (i.e. the time value is returned already converted to local time zone).
   Parameters
   hFsEntry :                            A handle of type
                                         PHT_REMOTE_FILESYSTEM_ENTRY
                                         identifying the file
                                         system entry.
   sFsEntryLastModifiedDate :            [out] A pointer to a
                                         buffer that receives the
                                         \result (a UTF\-8
                                         encoded, null\-terminated
                                         string). The date is
                                         specified as\:
                                         yyyy\-mm\-dd hh\:mi\:ss.
   pnFsEntryLastModifiedDateBufLength :  [in] The size of the
                                         \output buffer (in
                                         bytes). Set the buffer
                                         pointer to null and this
                                         parameter's value to
                                         zero to receive the
                                         required size. [out]
                                         The required output
                                         buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.                */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsEntry_GetLastModifiedDate, (
		PRL_HANDLE hFsEntry,
		PRL_STR sFsEntryLastModifiedDate,
		PRL_UINT32_PTR pnFsEntryLastModifiedDateBufLength
		) );

/* \Returns the file system entry size.
   Parameters
   hFsEntry :  A handle of type PHT_REMOTE_FILESYSTEM_ENTRY
               identifying the file system entry.
   pnSize :    [out] A pointer to a variable that receives the
               size (in bytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsEntry_GetSize, (
		PRL_HANDLE hFsEntry,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns the specified file system entry permissions (read,
   write, execute) for the current user.
   Parameters
   hFsEntry :       A handle of type PHT_REMOTE_FILESYSTEM_ENTRY
                    identifying the file system entry.
   pnPermissions :  [out] A pointer to a variable that receives
                    the permission information. The permissions
                    are specified as bitmasks. For bitmask values
                    see macros in the PrlFs.h file.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsEntry_GetPermissions, (
		PRL_HANDLE hFsEntry,
		PRL_UINT32_PTR pnPermissions
		) );

/* \Returns the file system entry type (file, directory, drive).
   Parameters
   hFsEntry :       A handle of type PHT_REMOTE_FILESYSTEM_ENTRY
                    identifying the file system entry.
   pnFsEntryType :  [out] A pointer to a variable that receives
                    the type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlFsEntry_GetType, (
		PRL_HANDLE hFsEntry,
		PRL_FILE_SYSTEM_ELEMENT_TYPE_PTR pnFsEntryType
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Parallels Service config operations
///////////////////////////////////////////////////////////////////////////////

/* Determines the amount of memory (RAM) available on the host.
   Parameters
   hSrvConfig :     A handle of type PHT_SERVER_CONFIG.
   pnHostRamSize :  [out] A pointer to a variable that receives
                    the result. The memory size is specified in
                    Megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetHostRamSize, (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnHostRamSize
		) );

/* Determines the model of CPU on the host machine.
   Parameters
   hSrvConfig :           A handle of type PHT_SERVER_CONFIG.
   sCpuModel :            [out] A pointer to a buffer that
                          receives the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnCpuModelBufLength :  [in] The size of the output buffer (in
                          bytes). Set the buffer pointer to null
                          and this parameter's value to zero to
                          receive the required size. [out]
                          The required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetCpuModel, (
		PRL_HANDLE hSrvConfig,
		PRL_STR sCpuModel,
		PRL_UINT32_PTR pnCpuModelBufLength
		) );

/* Determines the number of CPUs on the host machine.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   pnCpuCount :  [out] A pointer to a variable that receives the
                 \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetCpuCount, (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnCpuCount
		) );

/* Determines the host machine CPU speed.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   pnCpuSpeed :  [out] A pointer to a variable that receives the
                 \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetCpuSpeed, (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnCpuSpeed
		) );

/* Determines the CPU mode (32 bit or 64 bit) on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   pnCpuMode :   [out] A pointer to a variable that receives the
                 \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetCpuMode, (
		PRL_HANDLE hSrvConfig,
		PRL_CPU_MODE_PTR pnCpuMode
		) );

/* Determines the hardware virtualization type of the host CPU.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   pnCpuMode :   [out] A pointer to a variable that receives the
                 \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
		PrlSrvCfg_GetCpuHvt, (
			PRL_HANDLE hSrvConfig,
			PRL_CPU_HVT_PTR pnCpuHvt
		) );

/* \Returns the host operating system type.
   Parameters
   hSrvConfig :    A handle of type PHT_SERVER_CONFIG.
   pnHostOsType :  [out] A pointer to a variable that receives the
                   \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetHostOsType, (
		PRL_HANDLE hSrvConfig,
		PRL_HOST_OS_TYPE_PTR pnHostOsType
		) );

/* \Returns the major version number of the host
   operating system.
   Parameters
   hSrvConfig :     A handle of type PHT_SERVER_CONFIG.
   pnHostOsMajor :  [out] A pointer to a variable that receives the
                    \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.               */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetHostOsMajor, (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnHostOsMajor
		) );

/* \Returns the minor version number of the host
   operating system.
   Parameters
   hSrvConfig :     A handle of type PHT_SERVER_CONFIG.
   pnHostOsMinor :  [out] A pointer to a variable that receives the
                    \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.               */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetHostOsMinor, (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnHostOsMinor
		) );

/* \Returns the sub-minor version number of the host
   operating system.
   Parameters
   hSrvConfig :        A handle of type PHT_SERVER_CONFIG.
   pnHostOsSubMinor :  [out] A pointer to a variable that receives
                       the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetHostOsSubMinor, (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnHostOsSubMinor
		) );

/* \Returns the full host operating system information as
   a single string.
   Parameters
   hSrvConfig :                        A handle of type
                                       PHT_SERVER_CONFIG.
   sHostOsStrPresentation :            [out] A pointer to a
                                       buffer that receives the
                                       \result (a UTF\-8 encoded,
                                       null\-terminated string).
   pnHostOsStrPresentationBufLength :  [in] The size of the
                                       \output buffer (in bytes).
                                       Set the buffer pointer to
                                       null and this parameter's
                                       value to zero to receive
                                       the required size. [out]
                                       The required output buffer
                                       size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetHostOsStrPresentation, (
		PRL_HANDLE hSrvConfig,
		PRL_STR sHostOsStrPresentation,
		PRL_UINT32_PTR pnHostOsStrPresentationBufLength
		) );

/* Determines whether a sound device on the host is
   enabled or disabled.
   Parameters
   hSrvConfig :             A handle of type PHT_SERVER_CONFIG.
   pbSoundDefaultEnabled :  [out] A pointer to a variable that
                            receives the result. PRL_TRUE
                            indicates that the device is enabled.
                            PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_IsSoundDefaultEnabled, (
		PRL_HANDLE hSrvConfig,
		PRL_BOOL_PTR pbSoundDefaultEnabled
		) );

/* Determines if USB is supported on the host.
   Parameters
   hSrvConfig :      A handle of type PHT_SERVER_CONFIG.
   pbUsbSupported :  [out] A pointer to a variable that receives
                     the result. PRL_TRUE indicates that USB is
                     supported. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_IsUsbSupported, (
		PRL_HANDLE hSrvConfig,
		PRL_BOOL_PTR pbUsbSupported
		) );

/* Determines whether VT-d is supported on the host.
   Parameters
   hSrvConfig :      A handle of type PHT_SERVER_CONFIG.
   pbVtdSupported :  [out] A pointer to a variable that receives
                     the result. PRL_TRUE indicates that VT\-d is
                     supported. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_IsVtdSupported, (
		PRL_HANDLE hSrvConfig,
		PRL_BOOL_PTR pbVtdSupported
		) );

/* \Returns max possible Parallels network adapters on the host.
   Parameters
   hSrvConfig :      A handle of type PHT_SERVER_CONFIG.
   pnMaxNetAdaptersCount :  [out] A pointer to a variable that receives
                     the result. Null value (0) means that remote Parallels
					 server doesn't supported this interface.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlSrvCfg_GetMaxHostNetAdapters, (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnMaxNetAdaptersCount
		) );

/* \Returns max possible Parallels virtual network adapters which can be applied per VM.
   Parameters
   hSrvConfig :      A handle of type PHT_SERVER_CONFIG.
   pnMaxNetAdaptersCount :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlSrvCfg_GetMaxVmNetAdapters, (
		PRL_HANDLE hSrvConfig,
		PRL_UINT32_PTR pnMaxNetAdaptersCount
		) );

/* \Returns the hostname of the specified host or guest.
   Parameters
   hSrvConfig :           A handle of type PHT_SERVER_CONFIG. When
                          obtaining the host OS information, use
                          the PrlSrv_GetSrvConfig to obtain the
                          handle. When obtaining the guest OS
                          information, use the
                          PrlVmGuest_GetNetworkSettings function.
   sHostname :            [out] A pointer to a buffer that
                          receives the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnHostnameBufLength :  [in] The size of the output buffer (in
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

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_GetHostname, (
		PRL_HANDLE hSrvConfig,
		PRL_STR sHostname,
		PRL_UINT32_PTR pnHostnameBufLength
		) );

/* Obtains the global default gateway address of the specified
   host or guest.
   Parameters
   hSrvConfig :                 A handle of type
                                PHT_SERVER_CONFIG. When obtaining
                                the host OS gateway address, use
                                the PrlSrv_GetSrvConfig to obtain
                                the handle. When obtaining the
                                guest OS gateway address, use the
                                PrlVmGuest_GetNetworkSettings
                                function.
   sDefaultGateway :            [out] A pointer to a buffer that
                                receives the result (a UTF\-8
                                encoded, null\-terminated
                                string).
   pnDefaultGatewayBufLength :  [in] The size of the output
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_GetDefaultGateway, (
		PRL_HANDLE hSrvConfig,
		PRL_STR sDefaultGateway,
		PRL_UINT32_PTR pnDefaultGatewayBufLength
		) );

/* Obtains the global default IPv6 gateway address of the specified
   host or guest.
   Parameters
   hSrvConfig :                 A handle of type
                                PHT_SERVER_CONFIG. When obtaining
                                the host OS gateway address, use
                                the PrlSrv_GetSrvConfig to obtain
                                the handle. When obtaining the
                                guest OS gateway address, use the
                                PrlVmGuest_GetNetworkSettings
                                function.
   sDefaultGateway :            [out] A pointer to a buffer that
                                receives the result (a UTF\-8
                                encoded, null\-terminated
                                string).
   pnDefaultGatewayBufLength :  [in] The size of the output
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_GetDefaultGatewayIPv6, (
		PRL_HANDLE hSrvConfig,
		PRL_STR sDefaultGateway,
		PRL_UINT32_PTR pnDefaultGatewayBufLength
		) );


/* Obtains the list of IP addresses of DNS servers for the
   specified host or guest.
   Parameters
   hSrvConfig :        A handle of type PHT_SERVER_CONFIG. When
                       obtaining the host OS information, use the
                       PrlSrv_GetSrvConfig to obtain the handle.
                       When obtaining the guest OS information,
                       use the PrlVmGuest_GetNetworkSettings
                       function.
   phDnsServersList :  [out] A pointer to a variable that
                       receives the PHT_STRINGS_LIST handle
                       containing the IP addresses of DNS
                       servers.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate
   strings list object

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_GetDnsServers, (
				 PRL_HANDLE hSrvConfig,
				 PRL_HANDLE_PTR phDnsServersList
				 ) );

/* Obtains the list of search domains for the specified host or
   guest.
   Parameters
   hNetAdapter :          A handle of type PHT_SERVER_CONFIG. When
                          obtaining the host OS information, use
                          the PrlSrv_GetSrvConfig to obtain the
                          handle. When obtaining the guest OS
                          information, use the
                          PrlVmGuest_GetNetworkSettings function.
   phSearchDomainsList :  [out] A pointer to a variable that
                          receives the PHT_STRINGS_LIST handle
                          containing the list of search domains.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate
   strings list object

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_GetSearchDomains, (
				 PRL_HANDLE hSrvConfig,
				 PRL_HANDLE_PTR phSearchDomainsList
				 ) );

/* Determines the number of floppy disk drives on the host.
   Parameters
   hSrvConfig :          A handle of type PHT_SERVER_CONFIG.
   pnFloppyDisksCount :  [out] A pointer to a variable that
                         receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetFloppyDisksCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnFloppyDisksCount
				 ) );


/* Obtains an object of type PHT_HW_GENERIC_DEVICE containing
   information about a floppy disk drive on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the drive (beginning
                 with 0). The number of drives in the list can be
                 determined by making the
                 PrlSrvCfg_GetFloppyDisksCount function call.
   phDevice :    [out] A pointer to a variable that receives a
                 handle of type PHT_HW_GENERIC_DEVICE containing
                 the drive information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetFloppyDisk, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of optical disks on the host.
   Parameters
   hSrvConfig :           A handle of type PHT_SERVER_CONFIG.
   pnOpticalDisksCount :  [out] A pointer to a variable that
                          receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.         */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetOpticalDisksCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnOpticalDisksCount
				 ) );


/* Obtains an object of type PHT_HW_GENERIC_DEVICE containing
   information about an optical disk on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the optical disk
                 (beginning with 0). To get the number of disks
                 available, use the
                 PrlSrvCfg_GetOpticalDisksCount function.
   phDevice :    [out] A pointer to a variable that receives a
                 handle of type PHT_HW_GENERIC_DEVICE containing
                 the disk information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetOpticalDisk, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of serial ports available on the host.
   Parameters
   hSrvConfig :          A handle of type PHT_SERVER_CONFIG.
   pnSerialPortsCount :  [out] A pointer to a variable that
                         receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetSerialPortsCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnSerialPortsCount
				 ) );


/* Obtains a handle of type PHT_HW_GENERIC_DEVICE containing
   information about a serial port on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the port (beginning
                 with 0). To get the number of ports available,
                 use the PrlSrvCfg_GetSerialPortsCount function.
   phDevice :    [out] A pointer to a variable that receives a
                 handle of type PHT_HW_GENERIC_DEVICE containing
                 the port information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetSerialPort, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of printer ports on the host.
   Parameters
   hSrvConfig :            A handle of type PHT_SERVER_CONFIG.
   pnParallelPortsCount :  [out] A pointer to a variable that
                           receives the result
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetParallelPortsCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnParallelPortsCount
				 ) );


/* Obtains a handle of type PHT_HW_GENERIC_DEVICE containing
   information about a printer port on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the port (beginning
                 with 0). To get the number ports available, use
                 the PrlSrvCfg_GetParallelPortsCount function.
   phDevice :    [out] A pointer to a variable that receives a
                 handle of type PHT_HW_GENERIC_DEVICE containing
                 the port information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetParallelPort, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of sound devices available on the host.
   Parameters
   hSrvConfig :              A handle of type PHT_SERVER_CONFIG.
   pnSoundOutputDevsCount :  [out] A pointer to a variable that
                             receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetSoundOutputDevsCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnSoundOutputDevsCount
				 ) );


/* Obtains a handle of type PHT_HW_GENERIC_DEVICE containing
   information about a sound device on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the sound device
                 (beginning with 0). To get the number of sound
                 devices available, use the PrlSrvCfg_GetSoundOutputDevsCount
                 function.
   phDevice :    [out] A pointer to a variable that receives a
                 pointer of type PHT_HW_GENERIC_DEVICE containing
                 the device information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                         */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetSoundOutputDev, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of sound mixer devices available on the
   host.
   Parameters
   hSrvConfig :             A handle of type PHT_SERVER_CONFIG.
   pnSoundMixerDevsCount :  [out] A pointer to a variable that
                            receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetSoundMixerDevsCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnSoundMixerDevsCount
				 ) );


/* Obtains a handle of type PHT_HW_GENERIC_DEVICE containing
   information about a sound mixer device on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the sound mixer
                 device (beginning with 0). To get the number of
                 devices available, use the PrlSrvCfg_GetSoundMixerDevsCount
                 function.
   phDevice :    [out] A pointer to a variable that receives a
                 pointer of type PHT_HW_GENERIC_DEVICE containing
                 the device information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                        */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetSoundMixerDev, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of printers installed on the host.
   Parameters
   hSrvConfig :       A handle of type PHT_SERVER_CONFIG.
   pnPrintersCount :  [out] A pointer to a variable that receives
                      the
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetPrintersCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnPrintersCount
				 ) );


/* Obtains a handle of type PHT_HW_GENERIC_DEVICE containing
   information about a printer installed on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the printer
                 (beginning with 0). To get the number of
                 installed printers, use the
                 PrlSrvCfg_GetPrintersCount function.
   phDevice :    [out] A pointer to a variable that receives a
                 handle of type PHT_HW_GENERIC_DEVICE containing
                 the printer information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetPrinter, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of USB devices on the host.
   Parameters
   hSrvConfig :      A handle of type PHT_SERVER_CONFIG.
   pnUsbDevsCount :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetUsbDevsCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnUsbDevsCount
				 ) );


/* Obtains a pointer of type PHT_HW_GENERIC_DEVICE containing
   information about a USB device on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the device
                 (beginning with 0). To get the number of USB
                 devices available, use the PrlSrvCfg_GetUsbDevsCount
                 function.
   phDevice :    [out] A pointer to a variable that receives a
                 pointer of type PHT_HW_GENERIC_DEVICE containing
                 the device information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.                 */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetUsbDev, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of hard disk drives on the host.
   Parameters
   hSrvConfig :        A handle of type PHT_SERVER_CONFIG.
   pnHardDisksCount :  [out] A pointer to a variable that receives
                       the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetHardDisksCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnHardDisksCount
				 ) );


/* Obtains an object of type PHT_HW_HARD_DISK containing
   information about a hard disks drive on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the drive (beginning
                 with 0). The number of drives in the list can be
                 determined by making the
                 PrlSrvCfg_GetHardDisksCount function call.
   phDevice :    [out] A pointer to a variable that receives a
                 handle of type PHT_HW_HARD_DISK containing the
                 drive information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetHardDisk, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of network adapters available on the
   server.
   Parameters
   hSrvConfig :          A handle of type PHT_SERVER_CONFIG.
   pnNetAdaptersCount :  [out] A pointer to a variable that
                         receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.         */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetNetAdaptersCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnNetAdaptersCount
				 ) );


/* Obtains a handle of type PHT_HW_NET_ADAPTER containing
   information about a network adapter installed in the host or
   guest.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG. When
                 obtaining the host OS information, use the
                 PrlSrv_GetSrvConfig to obtain the handle. When
                 obtaining the guest OS information, use the
                 PrlVmGuest_GetNetworkSettings function.
   nIndex :      A numeric index identifying the network adapter
                 (beginning with 0). To get the number of
                 adapters available, use the
                 PrlSrvCfg_GetNetAdaptersCount function.
   phDevice :    [out] A pointer to a variable that receives a
                 handle of type PHT_HW_NET_ADAPTER containing the
                 network adapter information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfg_GetNetAdapter, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of PCI devices installed on the host.
   Parameters
   hSrvConfig :                A handle of type
                               PHT_SERVER_CONFIG.
   pnGenericPciDevicesCount :  [out] A pointer to a variable that
                               receives the number of devices
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlSrvCfg_GetGenericPciDevice                                  */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_GetGenericPciDevicesCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnGenericPciDevicesCount
				 ) );


/* Obtains a handle of type PHT_HW_GENERIC_DEVICE containing
   information about a PCI device installed on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the GenericPciDevice
                 (beginning with 0). To get the number of
                 installed generic PCI devices, use the
                 PrlSrvCfg_GetGenericPciDevicesCount function.
   phDevice :    [out] A pointer to a variable that receives a
                 handle of type PHT_HW_GENERIC_DEVICE containing
                 the generic PCI device information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlSrvCfg_GetGenericPciDevicesCount                            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_GetGenericPciDevice, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );

/* Determines the number of generic SCSI devices installed on
   the host.
   Parameters
   hSrvConfig :                 A handle of type
                                PHT_SERVER_CONFIG.
   pnGenericScsiDevicesCount :  [out] A pointer to a variable
                                that receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.         */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_GetGenericScsiDevicesCount, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32_PTR pnGenericScsiDevicesCount
				 ) );


/* Obtains a handle of type PHT_HW_GENERIC_DEVICE containing
   information about a generic SCSI device installed on the host.
   Parameters
   hSrvConfig :  A handle of type PHT_SERVER_CONFIG.
   nIndex :      A numeric index identifying the GenericScsiDevice
                 (beginning with 0). To get the number of
                 installed generic SCSI devices, use the
                 PrlSrvCfg_GetGenericScsiDevicesCount function.
   phDevice :    [out] A pointer to a variable that receives a
                 handle of type PHT_HW_GENERIC_DEVICE containing
                 the generic SCSI device information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfg_GetGenericScsiDevice, (
				 PRL_HANDLE hSrvConfig,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phDevice
				 ) );


/* To get the CPU features from server
   hSrvConfig :			A handle of type PRL_HANDLE.
   pMasks:			[out] A pointer structure with processor features set
   Returns
   PRL_RESULT. Possible values:
   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.
   PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
			PrlSrvCfg_GetCpuFeatures, (
		PRL_HANDLE hSrvConfig,
		PRL_CPU_FEATURES_PTR pCpuFeatures
		) );

/* Allows to get the CPU features from server.
   Parameters
   hSrvConfig     :  A handle of type PHT_SERVER_CONFIG.
   phCpuFeatures  :  A pointer to a variable that receives a handle of type PHT_CPU_FEATURES.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate new object.

   PRL_ERR_SUCCESS - function completed successfully.*/
PRL_METHOD_DECL( PARALLELS_API_VER_5,
			PrlSrvCfg_GetCpuFeaturesEx, (
		PRL_HANDLE hSrvConfig,
		PRL_HANDLE_PTR phCpuFeatures
		) );

/* Allows to get the CPU features masking capabilities from server
   Parameters
   hSrvConfig     :  A handle of type PHT_SERVER_CONFIG.
   phCpuFeatures  :  A pointer to a variable that receives a handle of type PHT_CPU_FEATURES.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate new object.

   PRL_ERR_SUCCESS - function completed successfully.*/
PRL_METHOD_DECL( PARALLELS_API_VER_5,
			PrlSrvCfg_GetCpuFeaturesMaskingCapabilities, (
		PRL_HANDLE hSrvConfig,
		PRL_HANDLE_PTR phCpuFeatures
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Host hardware info generic device operations
///////////////////////////////////////////////////////////////////////////////

/* Obtains the device name.
   Parameters
   hGenericDevice :      A handle of type PHT_HW_GENERIC_DEVICE,
                         or one of the descendant types,
                         identifying the device.
   sDevName :            [out] A pointer to a buffer that
                         receives the device name (a UTF\-8
                         encoded, null\-terminated string).
   pnDevNameBufLength :  [in] The size of the output buffer (in
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

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgDev_GetName, (
		PRL_HANDLE hGenericDevice,
		PRL_STR	sDevName,
		PRL_UINT32_PTR pnDevNameBufLength
		) );


/* Obtains the device ID.
   Parameters
   hGenericDevice :    A handle of type PHT_HW_GENERIC_DEVICE, or
                       one of the descendant types, identifying
                       the device.
   sDevId :            [out] A pointer to a buffer that receives
                       the device ID (a UTF\-8 encoded,
                       null\-terminated string)
   pnDevIdBufLength :  [in] The size of the output buffer (in
                       bytes). Set the buffer pointer to null and
                       this value to zero to receive the required
                       size. [out] The required output buffer
                       size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgDev_GetId, (
		PRL_HANDLE hGenericDevice,
		PRL_STR	sDevId,
		PRL_UINT32_PTR pnDevIdBufLength
		) );


/* Obtains the device type.
   Parameters
   hGenericDevice :  A handle of type PHT_HW_GENERIC_DEVICE, or
                     one of the descendant types, identifying the
                     device.
   pnDevType :       [out] A pointer to a variable that receives
                     the device type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgDev_GetType, (
		PRL_HANDLE hGenericDevice,
		PRL_DEVICE_TYPE_PTR pnDevType
		) );

/* Determines whether the device is connected to a virtual
   machine.
   Parameters
   hGenericDevice :  A handle of type PHT_HW_GENERIC_DEVICE, or
                     one of the descendant types, identifying the
                     device.
   pbConnected :     [out] A pointer to a variable that receives
                     the device connection status.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgDev_IsConnectedToVm, (
		PRL_HANDLE hGenericDevice,
		PRL_BOOL_PTR pbConnected
		) );

/* Determines whether a virtual machine can directly use a
   device, such as a PCI device through IOMMU technology (e.g.
   Intel Vt-d) or a USB device.
   Parameters
   hGenericDevice :  A handle of type PHT_HW_GENERIC_DEVICE, or
                     one of the descendant types, identifying the
                     device.
   pnDeviceState :   [out] A pointer to a variable that receives
                     the currently set device access preference.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlSrvCfgDev_SetDeviceState                                    */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgDev_GetDeviceState, (
		PRL_HANDLE hGenericDevice,
		PRL_GENERIC_DEVICE_STATE_PTR pnDeviceState
		) );

/* Allows to specify whether a virtual machine can directly use
   a device, such as a PCI device through IOMMU technology (e.g.
   Intel Vt-d) or a USB device. When using this function to set
   a PCI device access, the PrlSrv_ConfigureGenericPci function
   needs to be called afterwards to apply the changes to the
   Parallels Service preferences.
   Parameters
   hGenericDevice :  A handle of type PHT_HW_GENERIC_DEVICE, or
                     one of the descendant types, identifying the
                     device.
   pnDeviceState :   The device access preference.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlSrvCfgDev_GetDeviceState                                    */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgDev_SetDeviceState, (
		PRL_HANDLE hGenericDevice,
		PRL_GENERIC_DEVICE_STATE nDeviceState
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Host hardware info hard disk operations
///////////////////////////////////////////////////////////////////////////////

/* \Returns the hard disk device name.
   Parameters
   hHardDisk :           A handle of type PHT_HW_HARD_DISK,
                         identifying the hard disk.
   sDevName :            [out] Pointer to a buffer that receives
                         the device name (a UTF\-8 encoded,
                         null\-terminated string).
   pnDevNameBufLength :  [in] The size of the output buffer (in
                         bytes). Set the buffer pointer to null
                         and this value to zero to receive the
                         required size. [out] The required
                         \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHdd_GetDevName, (
		PRL_HANDLE hHardDisk,
		PRL_STR	sDevName,
		PRL_UINT32_PTR pnDevNameBufLength
		) );


/* \Returns the hard disk device id.
   Parameters
   hHardDisk :         A handle of type PHT_HW_HARD_DISK,
                       identifying the hard disk.
   sDevId :            [out] Pointer to a buffer that receives
                       the device ID (a UTF\-8 encoded,
                       null\-terminated string).
   pnDevIdBufLength :  [in] The size of the output buffer (in
                       bytes). Set the buffer pointer to null and
                       this value to zero to receive the required
                       size. [out] The required output buffer
                       size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHdd_GetDevId, (
		PRL_HANDLE hHardDisk,
		PRL_STR	sDevId,
		PRL_UINT32_PTR pnDevIdBufLength
		) );


/* \Returns the size of the hard disk device.
   Parameters
   hHardDisk :  A handle of type PHT_HW_HARD_DISK, identifying the
                hard disk.
   pnSize :     [out] Pointer to a variable that receives the
                hard disk size. The size is specified in
                megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHdd_GetDevSize, (
				 PRL_HANDLE hHardDisk,
				 PRL_UINT64_PTR pnSize
				 ) );

/* \Returns the index of a hard disk device.
   Parameters
   hHardDisk :  A handle of type PHT_HW_HARD_DISK, identifying the
                hard disk.
   pnIndex :    [out] Pointer to a variable that receives the
                hard disk index.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHdd_GetDiskIndex, (
				 PRL_HANDLE hHardDisk,
				 PRL_UINT32_PTR pnIndex
				 ) );

/* \Returns the number of partitions available on a hard drive.
   Use this function together with the PrlSrvCfgHdd_GetPart
   function to obtain a handle to an object containing a hard
   disk partition information.
   Parameters
   hHardDisk :     A handle of type PHT_HW_HARD_DISK, identifying
                   the hard disk.
   pnPartsCount :  [out] A pointer to a variable receiving the
                   number of partitions.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHdd_GetPartsCount, (
				 PRL_HANDLE hHardDisk,
				 PRL_UINT32_PTR pnPartsCount
				 ) );


/* Obtains a handle of type PHT_HW_HARD_DISK_PARTITION
   identifying the specified hard disk partition.
   Parameters
   hHardDisk :  A handle of type PHT_HW_HARD_DISK, identifying the
                hard disk.
   nIndex :     An index of the hard disk partition for which you
                would like to obtain a handle. Partitions are
                index beginning with 0 (zero). The total number
                of partitions can be obtained by calling the
                PrlSrvCfgHdd_GetPartsCount function.
   phPart :     [out] A pointer to a handle of type
                PHT_HW_HARD_DISK_PARTITION identifying the
                partition.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHdd_GetPart, (
				 PRL_HANDLE hHardDisk,
				 PRL_UINT32 nIndex,
				 PRL_HANDLE_PTR phPart
				 ) );

///////////////////////////////////////////////////////////////////////////////
/// @section Host hardware info hard disk partition operations
///////////////////////////////////////////////////////////////////////////////

/* \Returns a hard disk partition name.
   Parameters
   hPart :            A valid handle of type
                      PHT_HW_HARD_DISK_PARTITION identifying the
                      hard disk partition.
   sName :            [out] Pointer to a buffer that receives the
                      partition name (a UTF\-8 encoded,
                      null\-terminated string).
   pnNameBufLength :  [in] The size of the output buffer (in
                      bytes). Set the buffer pointer to null and
                      this value to zero to receive the required
                      size. [out] The required output buffer
                      size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHddPart_GetName, (
		PRL_HANDLE hPart,
		PRL_STR	sName,
		PRL_UINT32_PTR pnNameBufLength
		) );

/* \Returns a hard disk partition system name.
   Parameters
   hPart :               A handle of type
                         PHT_HW_HARD_DISK_PARTITION identifying
                         the hard disk partition.
   sSysName :            [out] Pointer to a buffer that receives
                         the partition name (a UTF\-8 encoded,
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

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgHddPart_GetSysName, (
		PRL_HANDLE hPart,
		PRL_STR	sSysName,
		PRL_UINT32_PTR pnSysNameBufLength
		) );

/* \Returns the hard disk partition size.
   Parameters
   hPart :   A valid handle of type PHT_HW_HARD_DISK_PARTITION
             identifying the hard disk partition.
   pnSize :  [out] Pointer to a buffer that receives the
             partition size. The size is specified in megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHddPart_GetSize, (
				 PRL_HANDLE hPart,
				 PRL_UINT64_PTR pnSize
				 ) );

/* \Returns the index of the specified hard disk partition.
   Partition index uniquely identifies a partition on a hard
   disk.
   Parameters
   hPart :    A valid handle of type PHT_HW_HARD_DISK_PARTITION
              identifying the hard disk partition.
   pnIndex :  [out] Pointer to a variable that receives the
              partition index.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHddPart_GetIndex, (
				 PRL_HANDLE hPart,
				 PRL_UINT32_PTR pnIndex
				 ) );

/* \Returns a numerical code identifying the type of the
   specified partition.
   Parameters
   hPart :   A valid handle of type PHT_HW_HARD_DISK_PARTITION
             identifying the hard disk partition.
   pnType :  [out] Pointer to a variable that receives a
             numerical code identifying the partition type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHddPart_GetType, (
				 PRL_HANDLE hPart,
				 PRL_UINT32_PTR pnType
				 ) );

/* Determines whether a disk partition is in use, that is,
   whether it contains a valid file system, being used for swap
   or by a database, etc.
   Parameters
   hPart :    A valid handle of type PHT_HW_HARD_DISK_PARTITION
              identifying the hard disk partition.
   pbInUse :  [out] Pointer to a variable that receives the
              \result. PRL_TRUE indicates that the partition is
              in use. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHddPart_IsInUse, (
				 PRL_HANDLE hPart,
				 PRL_BOOL_PTR pbInUse
				 ) );

/* Determines whether the specified partition is a logical
   partition.
   Parameters
   hPart :      A valid handle of type PHT_HW_HARD_DISK_PARTITION
                identifying the hard disk partition.
   pbLogical :  [out] Pointer to a variable that receives the
                \result. PRL_TRUE indicates that this is a
                logical partition. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHddPart_IsLogical, (
				 PRL_HANDLE hPart,
				 PRL_BOOL_PTR pbLogical
				 ) );

/* Determines whether a disk partition is active or inactive.
   Parameters
   hPart :     A valid handle of type PHT_HW_HARD_DISK_PARTITION
               identifying the hard disk partition.
   pbActive :  [out] Pointer to a variable that receives the
               \return value. PRL_TRUE indicates that the
               partition is active. PRL_FALSE indicates that the
               partition is inactive.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgHddPart_IsActive, (
				 PRL_HANDLE hPart,
				 PRL_BOOL_PTR pbActive
				 ) );

///////////////////////////////////////////////////////////////////////////////
/// @section Host hardware info net adapters operations
///////////////////////////////////////////////////////////////////////////////

/* \Returns a network adapter type.
   Parameters
   hNetAdapter :       A valid handle of type PHT_HW_NET_ADAPTER
                       identifying the adapter.
   pnNetAdapterType :  [out] A pointer to a variable that
                       receives the network adapter type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgNet_GetNetAdapterType, (
		PRL_HANDLE hNetAdapter,
		PRL_HW_INFO_NET_ADAPTER_TYPE_PTR pnNetAdapterType
		) );

/* \Returns a network adapter system index. A system index
   uniquely identifies an adapter on a physical host.
   Parameters
   hNetAdapter :  A valid handle of type PHT_HW_NET_ADAPTER
                  identifying the adapter.
   pnSysIndex :   [out] A pointer to a variable that receives the
                  adapter index.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgNet_GetSysIndex, (
				 PRL_HANDLE hNetAdapter,
				 PRL_UINT32_PTR pnSysIndex
				 ) );

/* Determines whether the adapter is enabled or disabled.
   Parameters
   hNetAdapter :  A valid handle of type PHT_HW_NET_ADAPTER
                  identifying the adapter.
   pbEnabled :    [out] A pointer to a variable that receives the
                  \return value. PRL_TRUE indicates that the
                  adapter is enabled. PRL_FALSE indicates
                  otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvCfgNet_IsEnabled, (
				 PRL_HANDLE hNetAdapter,
				 PRL_BOOL_PTR pbEnabled
				 ) );

/* Determines whether the adapter network settings are
   configured through DHCP.
   Parameters
   hNetAdapter :  A valid handle of type PHT_HW_NET_ADAPTER
                  identifying the adapter.
   pbEnabled :    [out] A pointer to a variable that receives the
                  \return value. PRL_TRUE indicates that the
                  adapter settings are configured through DHCP.
                  PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgNet_IsConfigureWithDhcp, (
				 PRL_HANDLE hNetAdapter,
				 PRL_BOOL_PTR pbConfigureWithDhcp
				 ) );

/* Determines whether the adapter IPv6 network settings are
   configured through DHCP.
   Parameters
   hNetAdapter :  A valid handle of type PHT_HW_NET_ADAPTER
                  identifying the adapter.
   pbEnabled :    [out] A pointer to a variable that receives the
                  \return value. PRL_TRUE indicates that the
                  adapter settings are configured through DHCP.
                  PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlSrvCfgNet_IsConfigureWithDhcpIPv6, (
				 PRL_HANDLE hNetAdapter,
				 PRL_BOOL_PTR pbConfigureWithDhcp
				 ) );


/* \Returns the default gateway address for the specified
   network adapter.
   Parameters
   hNetAdapter :                A valid handle of type
                                PHT_HW_NET_ADAPTER identifying
                                the network adapter.
   sDefaultGateway :            [out] Pointer to a buffer that
                                receives the result (a UTF\-8
                                encoded, null\-terminated
                                string).
   pnDefaultGatewayBufLength :  [in] The size of the output
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

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgNet_GetDefaultGateway, (
		PRL_HANDLE hNetAdapter,
		PRL_STR	sDefaultGateway,
		PRL_UINT32_PTR pnDefaultGatewayBufLength
		) );

/* \Returns the default IPv6 gateway address for the specified
   network adapter.
   Parameters
   hNetAdapter :                A valid handle of type
                                PHT_HW_NET_ADAPTER identifying
                                the network adapter.
   sDefaultGateway :            [out] Pointer to a buffer that
                                receives the result (a UTF\-8
                                encoded, null\-terminated
                                string).
   pnDefaultGatewayBufLength :  [in] The size of the output
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

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlSrvCfgNet_GetDefaultGatewayIPv6, (
		PRL_HANDLE hNetAdapter,
		PRL_STR	sDefaultGateway,
		PRL_UINT32_PTR pnDefaultGatewayBufLength
		) );


/* \Returns the MAC address of the specified network adapter.
   Parameters
   hNetAdapter :            A handle of type PHT_HW_NET_ADAPTER
                            identifying the network adapter.
   sMacAddress :            [out] Pointer to a buffer that
                            receives the result (a UTF\-8
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

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgNet_GetMacAddress, (
		PRL_HANDLE hNetAdapter,
		PRL_STR	sMacAddress,
		PRL_UINT32_PTR pnMacAddressBufLength
		) );

/* Obtains a list of network addresses (IP address/Subnet mask
   pairs) assigned to the specified network adapter.
   Parameters
   hNetAdapter :         A valid handle of type
                         PHT_HW_NET_ADAPTER identifying the
                         adapter.
   phNetAddressesList :  [out] A pointer to a variable that
                         receives the PHT_STRINGS_LIST handle
                         containing the addresses.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate
   strings list object

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgNet_GetNetAddresses, (
				 PRL_HANDLE hNetAdapter,
				 PRL_HANDLE_PTR phNetAddressesList
				 ) );

/* Obtains a list of addresses of DNS servers assigned to the
   specified network adapter.
   Parameters
   hNetAdapter :       A valid handle of type PHT_HW_NET_ADAPTER
                       identifying the adapter.
   phDnsServersList :  [out] A pointer to a variable that
                       receives the PHT_STRINGS_LIST handle
                       containing the list of addresses.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate
   strings list object

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgNet_GetDnsServers, (
				 PRL_HANDLE hNetAdapter,
				 PRL_HANDLE_PTR phDnsServersList
				 ) );

/* Obtains a list of search domains assigned to the specified
   network adapter.
   Parameters
   hNetAdapter :          A valid handle of type
                          PHT_HW_NET_ADAPTER identifying the
                          adapter.
   phSearchDomainsList :  [out] A pointer to a variable that
                          receives the PHT_STRINGS_LIST handle
                          containing the list of search domains.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate
   strings list object

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgNet_GetSearchDomains, (
				 PRL_HANDLE hNetAdapter,
				 PRL_HANDLE_PTR phSearchDomainsList
				 ) );

/* \Returns the VLAN tag of the specified network adapter. Since
   the same physical adapter can be used at several virtual
   networks pair <VLAN tag, MAC address> can identify adapter.
   Parameters
   hNetAdapter :            A handle of type PHT_HW_NET_ADAPTER
                            identifying the network adapter.
   pnVlanTag   :            [out] The buffer for storing result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgNet_GetVlanTag, (
		PRL_HANDLE hNetAdapter,
		PRL_UINT16_PTR pnVlanTag
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Host hardware info generic PCI devices operations
///////////////////////////////////////////////////////////////////////////////

/* Obtains the class of the PCI device (network, display, sound,
   etc.).
   Parameters
   hGenericPci :        A valid handle of type
                        PHT_HW_GENERIC_PCI_DEVICE identifying the
                        adapter.
   pnGenericPciClass :  [out] A pointer to a variable that
                        receives the PCI device class.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlSrvCfgPci_GetDeviceClass, (
		PRL_HANDLE hGenericPci,
		PRL_GENERIC_PCI_DEVICE_CLASS_PTR pnGenericPciClass
		) );

/* Determines whether a specified PCI device (network adapter, display adapter, sound card, etc.)
   is the primary adapter in the system.
   Parameters
   hGenericPci :        A valid handle of type
                        PHT_HW_GENERIC_PCI_DEVICE identifying the
                        adapter.
   pbIsPrimaryDevice :  [out] A pointer to a variable that receives
                     the result. PRL_TRUE indicates that the
                     PCI device is primary.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlSrvCfgPci_IsPrimaryDevice, (
		PRL_HANDLE hGenericPci,
		PRL_BOOL_PTR pbIsPrimaryDevice
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section User profile parameters access methods
///////////////////////////////////////////////////////////////////////////////

/* \Returns name and path of the default virtual machine folder
   for this Parallels Service. If a default virtual machine folder has been
   set up for this user with the PrlUsrCfg_SetDefaultVmFolder
   function, use the PrlUsrCfg_GetDefaultVmFolder to get its
   name and path.
   Parameters
   hUserProfile :          A handle of type PHT_USER_PROFILE
                           identifying the user.
   sVmDirUuid :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnVmDirUuidBufLength :  [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrCfg_GetVmDirUuid, (
		PRL_HANDLE hUserProfile,
		PRL_STR sVmDirUuid,
		PRL_UINT32_PTR pnVmDirUuidBufLength
		) );

/* Sets user default VM dir.

   Specified string value must be UTF-8 encoding and ends with
   '0' terminating symbol.
   Parameters
   handle :         A handle of type PHT_USER_PROFILE identifying
                    the user.
   unnamed_param :  new VM dir
   Returns
   PRL_RESULT. Possible values: PRL_ERR_INVALID_ARG - non valid
   PHT_USER_PROFILE handle or non valid (NULL) pointer to the
   new VM dir string was specified PRL_ERR_SUCCESS - operation
   completed successfully                                         */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrCfg_SetVmDirUuid, (
		PRL_HANDLE hUserProfile,
		PRL_CONST_STR sNewVmDirUuid
		) );

/* \Returns name and path of the default virtual machine
   directory for this user. If the user doesn't have a default
   virtual machine folder (if it was not previously set up by
   the PrlUsrCfg_SetDefaultVmFolder function call), the function
   \returns an empty string. In such a case, use the
   PrlUsrCfg_GetVmDirUuid function to get the virtual machine
   folder for the Parallels Service, which is used by default for all
   users.
   Parameters
   hUserProfile :                A handle of type
                                 PHT_USER_PROFILE identifying the
                                 user.
   sDefaultVmFolder :            [out] A pointer to a buffer that
                                 receives the result (a UTF\-8
                                 encoded, null\-terminated
                                 string).
   pnDefaultVmFolderBufLength :  [in] The size of the output
                                 buffer (in bytes). Set the
                                 buffer pointer to null and this
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

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlUsrCfg_SetDefaultVmFolder                                   */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrCfg_GetDefaultVmFolder, (
					 PRL_HANDLE hUserProfile,
					 PRL_STR sDefaultVmFolder,
					 PRL_UINT32_PTR pnDefaultVmFolderBufLength
					 ) );

/* Allows to set a default virtual machine folder for the user.
   A Parallels Service has a default virtual machine directory
   (the directory where new virtual machines are created).
   Individual users can have their own default virtual machine
   directories if needed. If a user has its own virtual machine
   directory set up, it takes precedence over the Parallels Service
   default. To set a default directory for a user, first call
   the PrlSrv_UserProfileBeginEdit function to mark the
   beginning of the operation. Then call the
   PrlUsrCfg_SetDefaultVmFolder function to set the new default
   virtual machine folder. Finally, call the
   PrlSrv_UserProfileCommit function to save your changes.
   Parameters
   hUserProfile :         A handle of type PHT_USER_PROFILE
                          identifying the user.
   sNewDefaultVmFolder :  Name and path of the folder to set as
                          default virtual machine folder for the
                          user. The folder must already exist on
                          the host.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrCfg_SetDefaultVmFolder, (
					 PRL_HANDLE hUserProfile,
					 PRL_CONST_STR sNewDefaultVmFolder
					 ) );



/* Determines if the user is allowed to use the Parallels Service
   management console utility.
   Parameters
   hUserProfile :        A handle of type PHT_USER_PROFILE
                         identifying the user.
   pbCanUseMngConsole :  [out] A pointer to a variable that
                         receives the result. PRL_TRUE indicates
                         that the user is allows to use the
                         utility. PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrCfg_CanUseMngConsole, (
		PRL_HANDLE hUserProfile,
		PRL_BOOL_PTR pbCanUseMngConsole
		) );

/* Determines if the current user can modify Parallels Service
   preferences (PHT_DISP_CONFIG). By default, only
   administrators of the host machine can modify Parallels Service
   preferences. Depending on the current Parallels Service settings, other
   users may also be granted this privilege at the time a user
   profile is created. To verify the default settings, use the
   PrlDispCfg_CanChangeDefaultSettings function. Whether the
   user can or cannot modify Parallels Service preferences, this setting is
   final and cannot itself be changed through Parallels
   API.
   Parameters
   hUserProfile :        A handle of type PHT_USER_PROFILE
                         identifying the user.
   pbCanChangeSrvSets :  [out] A pointer to a variable that
                         receives the result. PRL_TRUE indicates
                         that the client can modify Parallels Service
                         preferences. PRL_FALSE indicates
                         otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_CanChangeDefaultSettings

   PrlDispCfg_SetCanChangeDefaultSettings                        */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrCfg_CanChangeSrvSets, (
		PRL_HANDLE hUserProfile,
		PRL_BOOL_PTR pbCanChangeSrvSets
		) );


/* Determines if the user is a local administrator account on the host where
   Parallels Service started.
   Parameters
   hUserProfile :        A handle of type PHT_USER_PROFILE
                         identifying the user.
   pbIsLocalAdmin :      [out] A pointer to a variable that
                         receives the result. PRL_TRUE indicates
                         that the user is local administrator.
						 PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlUsrCfg_IsLocalAdministrator, (
		PRL_HANDLE hUserProfile,
		PRL_BOOL_PTR pbIsLocalAdmin
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section User information parameters access methods
///////////////////////////////////////////////////////////////////////////////

/* \Returns the user name.
   Parameters
   hUserInfo :        A handle of type PHT_USER_INFO identifying
                      the user.
   sName :            [out] A pointer to a buffer that receives
                      the result (a UTF\-8 encoded,
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

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrInfo_GetName, (
					 PRL_HANDLE hUserInfo,
					 PRL_STR sName,
					 PRL_UINT32_PTR pnNameBufLength
					 ) );

/* \Returns the user Universally Unique Identifier (UUID).
   Parameters
   hUserInfo :        A handle of type PHT_USER_INFO identifying
                      the user.
   sUuid :            [out] A pointer to a buffer that receives
                      the result (a UTF\-8 encoded,
                      null\-terminated string).
   pnUuidBufLength :  [in] The size of the output buffer (in
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

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrInfo_GetUuid, (
					 PRL_HANDLE hUserInfo,
					 PRL_STR sUuid,
					 PRL_UINT32_PTR pnUuidBufLength
					 ) );

/* \Returns the user active session count.
   Parameters
   hUserInfo :       A handle of type PHT_USER_INFO identifying
                     the user.
   pnSessionCount :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrInfo_GetSessionCount, (
		PRL_HANDLE hUserInfo,
		PRL_UINT32_PTR pnSessionCount
		) );

/* \Returns name and path of the default virtual machine
   directory for the specified user. If the user doesn't have a
   default virtual machine directory, the function will return
   the name and path of the default directory for this Parallels
   Service. To set a default virtual machine directory for a
   user, use the PrlUsrCfg_SetDefaultVmFolder function.
   Parameters
   hUserInfo :                   A handle of type PHT_USER_INFO
                                 identifying the user.
   sDefaultVmFolder :            [out] A pointer to a buffer that
                                 receives the result (a UTF\-8
                                 encoded, null\-terminated
                                 string).
   pnDefaultVmFolderBufLength :  [in] The size of the output
                                 buffer (in bytes). Set the
                                 buffer pointer to null and this
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

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlUsrCfg_SetDefaultVmFolder                                   */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrInfo_GetDefaultVmFolder, (
					 PRL_HANDLE hUserInfo,
					 PRL_STR sDefaultVmFolder,
					 PRL_UINT32_PTR pnDefaultVmFolderBufLength
					 ) );

/* Determines whether the specified user is allowed to modify
   Parallels Service preferences.
   Parameters
   hUserInfo :           A handle of type PHT_USER_INFO
                         identifying the user.
   pbCanChangeSrvSets :  [out] A pointer to a variable that
                         receives the result. PRL_TRUE indicates
                         that the user is allowed to modify
                         Parallels Service preferences. PRL_FALSE indicates
                         otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlUsrInfo_CanChangeSrvSets, (
		PRL_HANDLE hUserInfo,
		PRL_BOOL_PTR pbCanChangeSrvSets
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Parallels Service config common preferences access methods
///////////////////////////////////////////////////////////////////////////////

/* Obtains name and path of the directory in which new virtual
   machines are created by default. The virtual machine
   directory returned by this function is the default directory
   for all users and cannot be modified. Individual users can
   have their own default virtual machine directories if needed.
   To set a default directory for a user, use the
   PrlUsrCfg_SetDefaultVmFolder function. If a user has its own
   virtual machine directory set up, it takes precedence over
   the directory returned by this function.
   Parameters
   hDispConfig :              A handle of type PHT_DISP_CONFIG.
   sDefaultVmDir :            [out] A pointer to a buffer that
                              receives the directory name (a
                              UTF\-8 encoded, null\-terminated
                              string).
   pnDefaultVmDirBufLength :  [in] The size of the output buffer
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

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetDefaultVmDir, (
		PRL_HANDLE hDispConfig,
		PRL_STR sDefaultVmDir,
		PRL_UINT32_PTR pnDefaultVmDirBufLength
		) );
/* Obtains path of the directory in which new Container
   are created by default.
   Parameters
   hDispConfig :              A handle of type PHT_DISP_CONFIG.
   sDefaultDir :              [out] A pointer to a buffer that
                              receives the directory name (a
                              UTF\-8 encoded, null\-terminated
                              string).
   pnDefaultDirBufLength :    [in] The size of the output buffer
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

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetDefaultCtDir, (
		PRL_HANDLE hDispConfig,
		PRL_STR sDefaultDir,
		PRL_UINT32_PTR pnDefaultDirBufLength
		) );

/* Determines the amount of physical memory reserved for
   Parallels Service operation. By default, this memory size is
   calculated automatically based on the RAM size available on
   the host machine. If needed, the default value can be
   modified manually using the PrlDispCfg_SetReservedMemLimit
   function.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   pnMemSize :    [out] A pointer to a variable that receives the
                  memory size (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxReservMemLimit

   PrlDispCfg_GetMinReservMemLimit

   PrlDispCfg_SetMaxReservMemLimit

   PrlDispCfg_SetMinReservMemLimit

   PrlDispCfg_SetReservedMemLimit

   PrlDispCfg_IsAdjustMemAuto

   PrlDispCfg_SetAdjustMemAuto                                    */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetReservedMemLimit, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32_PTR pnMemSize
		) );

/* Allows to set the amount of memory that will be allocated for
   Parallels Service operation. The memory size must fall within
   the memory limits that can be determined using the
   PrlDispCfg_GetMaxReservMemLimit and the
   PrlDispCfg_GetMinReservMemLimit functions. By default, the
   memory size is calculated automatically based on the total
   RAM available on the host. If you are changing the default
   value, you must also switch the Parallels Service memory
   allocation to manual mode using the
   PrlDispCfg_SetAdjustMemAuto function.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   nMemSize :     The new memory size in megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxReservMemLimit

   PrlDispCfg_GetMinReservMemLimit

   PrlDispCfg_GetReservedMemLimit

   PrlDispCfg_SetMaxReservMemLimit

   PrlDispCfg_SetMinReservMemLimit

   PrlDispCfg_IsAdjustMemAuto

   PrlDispCfg_SetAdjustMemAuto                                   */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetReservedMemLimit, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32 nMemSize
		) );

/* Determines the minimum required memory size that must be
   allocated to an individual virtual machine. The value
   returned by this function indicates the minimum amount of
   memory that is absolutely required for a virtual machine to
   run. The actual amount of memory for a particular virtual
   machine is set in the virtual machine configuration and
   cannot be less than the value returned by this function.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   pnMemSize :    [out] A pointer to a variable that receives the
                  memory size (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxVmMem

   PrlDispCfg_GetRecommendMaxVmMem

   PrlDispCfg_SetMaxVmMem

   PrlDispCfg_SetMinVmMem

   PrlDispCfg_SetRecommendMaxVmMem                                */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetMinVmMem, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32_PTR pnMemSize
		) );

/* Allows to set the minimum required memory size that must be
   allocated to an individual virtual machine. By default,
   virtual machine memory limits are calculated automatically.
   If needed, you can set your own minimum memory limit using
   this function. Please note that the new value must not fall
   below the size of the minimum memory limit set for Parallels
   Service, which can be obtained using the
   PrlDispCfg_GetMinReservMemLimit function. The actual memory
   size for a virtual machine is set when configuring it and
   will be validated against the value set here.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   nMemSize :     The new minimum size (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxVmMem

   PrlDispCfg_GetRecommendMaxVmMem

   PrlDispCfg_SetMaxVmMem

   PrlDispCfg_SetMinVmMem

   PrlDispCfg_SetRecommendMaxVmMem                              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetMinVmMem, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32 nMemSize
		) );

/* Determines the maximum memory size that can be allocated to
   an individual virtual machine. The actual amount of memory
   for a particular virtual machine is set in the virtual
   machine configuration and cannot exceed the value returned by
   this function.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   pnMemSize :    [out] A pointer to a variable that receives the
                  memory size (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMinVmMem

   PrlDispCfg_GetRecommendMaxVmMem

   PrlDispCfg_SetMaxVmMem

   PrlDispCfg_SetMinVmMem

   PrlDispCfg_SetRecommendMaxVmMem                                */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetMaxVmMem, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32_PTR pnMemSize
		) );

/* Allows to set the maximum memory size that can be allocated
   to an individual virtual machine. By default, virtual machine
   memory limits are calculated automatically. If needed, you
   can set your own maximum memory limit using this function.
   Please note that the new value must not exceed the size of
   the memory reserved for Parallels Service, which can be
   obtained using the PrlDispCfg_GetReservedMemLimit function.
   The actual memory size for a virtual machine is set when
   configuring it and will be validated against the value set
   here.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   nMemSize :     The new memory size (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxVmMem

   PrlDispCfg_GetMinVmMem

   PrlDispCfg_GetRecommendMaxVmMem

   PrlDispCfg_SetMinVmMem

   PrlDispCfg_SetRecommendMaxVmMem                               */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetMaxVmMem, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32 nMemSize
		) );

/* Determines the recommended memory size for an individual
   virtual machine. Setting the virtual machine memory size to
   the value returned by this function is the safest way to do
   it but is not mandatory. You can use your own value but you
   have to make sure that it falls within the limits returned by
   the PrlDispCfg_GetMaxVmMem and PrlDispCfg_GetMinVmMem
   functions.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   pnMemSize :    [out] A pointer to a variable that receives the
                  memory size (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxVmMem

   PrlDispCfg_GetMinVmMem

   PrlDispCfg_SetMaxVmMem

   PrlDispCfg_SetMinVmMem

   PrlDispCfg_SetRecommendMaxVmMem                                */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetRecommendMaxVmMem, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32_PTR pnMemSize
		) );

/* Allows to manually set recommended memory size for an
   individual virtual machine. By default, the recommended
   memory size for a virtual machine is calculated
   automatically. If needed, you can set your own recommended
   memory size using this function. The value must be selected
   from the memory limit range that can be determined using the
   PrlDispCfg_GetMinVmMem and PrlDispCfg_GetMaxVmMem functions.
   The value by itself is not used as a constraint of any kind
   and is just a suggestion that the end user may act on or not.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   nMemSize :     The new recommended memory size in megabytes.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxVmMem

   PrlDispCfg_GetMinVmMem

   PrlDispCfg_GetRecommendMaxVmMem

   PrlDispCfg_SetMaxVmMem

   PrlDispCfg_SetMinVmMem                                        */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetRecommendMaxVmMem, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32 nMemSize
		) );

/* Determines the maximum amount of physical memory that can be
   reserved for Parallels Service operation. By default, memory
   limits and the actual size of the reserved memory are
   calculated automatically based on the total RAM available on
   the host machine. If needed, you can modify the default
   values. When setting the size of the memory reserved for
   Parallels Service, use this function to find out the maximum
   allowable size.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   pnMemSize :    [out] A pointer to a variable that receives the
                  memory size (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMinReservMemLimit

   PrlDispCfg_GetReservedMemLimit

   PrlDispCfg_SetMaxReservMemLimit

   PrlDispCfg_SetMinReservMemLimit

   PrlDispCfg_SetReservedMemLimit

   PrlDispCfg_IsAdjustMemAuto

   PrlDispCfg_SetAdjustMemAuto                                    */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetMaxReservMemLimit, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32_PTR pnMemSize
		) );

/* Allows to manually set the upper limit of the memory size
   that can be reserved for Parallels Service operation. By
   default, memory limits are calculated automatically based on
   the host machine RAM size. If needed, you can use this
   function to modify the default value of the maximum reserved
   memory size. Please note that modifying default memory limits
   must be done with extreme caution. Unjustified memory limits
   may result in too much or too little memory reserved for
   Parallels Service, which in turn may lead to an unstable or
   inefficient system. While this and other functions allow you
   to experiment with Parallels Service memory limits, the
   default values are the ones that should be used in most
   situations.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   nMemSize :     The new maximum memory limit (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxReservMemLimit

   PrlDispCfg_GetMinReservMemLimit

   PrlDispCfg_GetReservedMemLimit

   PrlDispCfg_SetMinReservMemLimit

   PrlDispCfg_SetReservedMemLimit

   PrlDispCfg_IsAdjustMemAuto

   PrlDispCfg_SetAdjustMemAuto                                   */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetMaxReservMemLimit, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32 nMemSize
		) );

/* Determines the minimum amount of physical memory that must be
   reserved for Parallels Service operation. By default, memory
   limits and the actual size of the reserved memory are
   calculated automatically based on the total RAM available on
   the host machine. If needed, you can modify the default
   values. When setting the size of the memory reserved on the
   host machine for Parallels Service, use this function to find
   out the minimum required size.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   pnMemSize :    [out] A pointer to a variable that receives the
                  memory size (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxReservMemLimit

   PrlDispCfg_GetReservedMemLimit

   PrlDispCfg_SetMaxReservMemLimit

   PrlDispCfg_SetMinReservMemLimit

   PrlDispCfg_SetReservedMemLimit

   PrlDispCfg_IsAdjustMemAuto

   PrlDispCfg_SetAdjustMemAuto                                    */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetMinReservMemLimit, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32_PTR pnMemSize
		) );

/* Allows to manually set the lower limit of the memory size
   that must be reserved for Parallels Service operation. By
   default, memory limits are calculated automatically based on
   the host machine RAM size and other parameters. If needed,
   you can use this function to modify the default value of the
   minimum required reserved memory size. Please note that
   modifying default memory limits must be done with extreme
   caution. Unjustified memory limits may result in too much or
   too little memory reserved for Parallels Service, which in
   turn may lead to an unstable or inefficient system. While
   this and other functions allow you to experiment with
   Parallels Service memory limits, the default values are
   should be used in most situations.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   nMemSize :     The new minimum size (in megabytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxReservMemLimit

   PrlDispCfg_GetMinReservMemLimit

   PrlDispCfg_GetReservedMemLimit

   PrlDispCfg_SetMaxReservMemLimit

   PrlDispCfg_SetReservedMemLimit

   PrlDispCfg_IsAdjustMemAuto

   PrlDispCfg_SetAdjustMemAuto                                  */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetMinReservMemLimit, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32 nMemSize
		) );

/* Determines whether memory allocation for Parallels Service is
   performed automatically or manually. By default, the amount
   of memory allocated for Parallels Service is calculated
   automatically based on the total RAM available on the host.
   You can allocate a different amount of memory if needed using
   the PrlDispCfg_SetReservedMemLimit function. Before you do
   that, you must switch memory allocation to manual mode using
   the PrlDispCfg_SetAdjustMemAuto function. The amount of
   currently allocated memory can be retrieved using the
   PrlDispCfg_GetReservedMemLimit function.
   Parameters
   hDispConfig :      A handle of type PHT_DISP_CONFIG.
   pbAdjustMemAuto :  [out] A pointer to a variable that receives
                      the result. PRL_TRUE indicates that the
                      memory is set automatically. PRL_FALSE
                      indicates that the size is set manually.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxReservMemLimit

   PrlDispCfg_GetMinReservMemLimit

   PrlDispCfg_GetReservedMemLimit

   PrlDispCfg_SetMaxReservMemLimit

   PrlDispCfg_SetMinReservMemLimit

   PrlDispCfg_SetReservedMemLimit

   PrlDispCfg_SetAdjustMemAuto                                    */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_IsAdjustMemAuto, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL_PTR pbAdjustMemAuto
		) );

/* Allows to choose the Parallels Service memory allocation mode
   from automatic or manual. By default, the amount of memory
   allocated for Parallels Service operations is calculated
   automatically based on the total RAM available on the host.
   You can allocate a different amount of memory if needed using
   the PrlDispCfg_SetReservedMemLimit function. Before you do
   that, you must switch memory allocation to manual using this
   function. The current memory allocation method can be
   determined using the PrlDispCfg_IsAdjustMemAuto function. The
   amount of currently allocated memory can be retrieved using
   the PrlDispCfg_GetReservedMemLimit function.
   Parameters
   hDispConfig :     A handle of type PHT_DISP_CONFIG.
   bAdjustMemAuto :  To set the size automatically, set this
                     parameter to PRL_TRUE. To set the size
                     manually, set this parameter to PRL_FALSE
                     and then use the
                     PrlDispCfg_SetReservedMemLimit function to
                     set the size.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMaxReservMemLimit

   PrlDispCfg_GetMinReservMemLimit

   PrlDispCfg_GetReservedMemLimit

   PrlDispCfg_SetMaxReservMemLimit

   PrlDispCfg_SetMinReservMemLimit

   PrlDispCfg_SetReservedMemLimit

   PrlDispCfg_IsAdjustMemAuto                                    */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetAdjustMemAuto, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL bAdjustMemAuto
		) );

/* Determines whether send statistics reports (CEP) mechanism
   activated. When this mechanism activated periodically report
   about Parallels product configuration genrating and sending to
   the Parallels problem reports server automatically.
   Parameters
   hDispConfig :      A handle of type PHT_DISP_CONFIG.
   pbSendStatisticReport :  [out] A pointer to a variable that receives
                      the result. PRL_TRUE indicates that the
                      mechanism is activated. PRL_FALSE
                      indicates that mechanism is switched off
					  (reports do not sending).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_SetSendStatisticReport                                    */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_IsSendStatisticReport, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL_PTR pbSendStatisticReport
		) );


/* Allows to switch on/off mechanism of sending statistics reports (CEP).
   Parameters
   hDispConfig :     A handle of type PHT_DISP_CONFIG.
   bSendStatisticReport :  To switch mechanism on, set this
                     parameter to PRL_TRUE. To switch off
                     mechanism set this parameter to PRL_FALSE.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_IsSendStatisticReport                                    */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_SetSendStatisticReport, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL bSendStatisticReport
		) );


/* \Returns the default VNC host name for the Parallels Service.
   Parameters
   hDispConfig :          A handle of type PHT_DISP_CONFIG.
   sHostName :            [out] Pointer to a buffer that receives
                          the VNC host name (a UTF\-8 encoded,
                          null\-terminated string).
   pnHostNameBufLength :  [in] The size of the output buffer (in
                          bytes). Set the buffer pointer to null
                          and this value to zero to receive the
                          required size. [out] The required
                          \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetDefaultVNCHostName, (
		PRL_HANDLE hDispConfig,
		PRL_STR sHostName,
		PRL_UINT32_PTR pnHostNameBufLength
		) );

/* Sets the base VNC host name.
   Parameters
   hDispConfig :   A handle of type PHT_DISP_CONFIG.
   sNewHostName :  The new VNC host name. The value must be UTF\-8
                   encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetDefaultVNCHostName, (
		PRL_HANDLE hDispConfig,
		PRL_CONST_STR sNewHostName
		) );

/* Obtains the currently set base VNC port number.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   pnPort :       [out] A pointer to a variable that receives the
                  port number.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetVNCBasePort, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32_PTR pnPort
		) );

/* Sets the base VNC port number.
   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   nPort :        Port number to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetVNCBasePort, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32 nPort
		) );

/* Determines if new users have the right to modify Parallels
   Service preferences. By default, only administrators of the
   host machine can modify Parallels Service preferences. When a
   new Parallels Service user profile is created (this happens
   automatically when a user logs in to the Parallels Service
   for the first time), he/she will be granted or denied this
   privilege based on the flag set in the Parallels Service
   configuration. This function determines the status of this
   flag.
   Parameters
   hDispConfig :              A handle of type PHT_DISP_CONFIG.
   pbDefaultChangeSettings :  [out] A pointer to a variable that
                              receives the result. PRL_TRUE
                              indicates that new users are
                              allowed to change the Parallels
                              Service preferences. PRL_FALSE
                              indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_SetCanChangeDefaultSettings                        */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_CanChangeDefaultSettings, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL_PTR pbDefaultChangeSettings
		) );

/* Allows to grant or deny a permission to new users to modify
   Parallels Service preferences. By default, only administrators
   of the host OS can modify Parallels Service preferences.
   When a new Parallels Service user profile is created (this
   happens when a user logs on to the Parallels Service for the first
   time), he/she will be granted or denied this privilege based
   on the default setting. This function allows to set that
   default setting. To determine the current behavior, use
   PrlDispCfg_CanChangeDefaultSettings. Please note that this
   function only affects the new users (the users that will be
   created in the future). The profiles of the existing users
   will not (and cannot) be modified.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   bDefaultChangeSettings :  If set to PRL_TRUE, new users will
                             be allowed to modify Parallels Service
                             preferences. If set to PRL_FALSE,
                             new users will not be allowed to
                             make such modifications.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_CanChangeDefaultSettings                           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetCanChangeDefaultSettings, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL bDefaultChangeSettings
		) );

/* Determines the lowest allowable security level that can be
   used to connect to the Parallels Service. Parallels Service
   has a configuration parameter that specifies the connection
   security level that must be used by clients to communicate
   with it. If a client tries to login using a lower level than
   the one returned by this function, the connection to the
   Parallels Service will be refused. Higher security levels are
   allowed. If the minimum security level is not known in
   advance, use this function to determine it before
   establishing a connection with the Service.
   Parameters
   hDispConfig :         A handle of type PHT_DISP_CONFIG.
   pnMinSecurityLevel :  [out] A pointer to a variable that
                         receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_SetMinSecurityLevel

   PrlSrv_Login

   PrlSrv_LoginLocal                                             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_GetMinSecurityLevel, (
		PRL_HANDLE hDispConfig,
		PRL_SECURITY_LEVEL_PTR pnMinSecurityLevel
		) );

/* Allows to set the lowest allowable security level that can be
   used to connect to the specified Parallels Service. The lowest
   allowable security level is a setting in the Parallels Service preferences.
   If a client tries to login using a lower level that the
   one set by this function, the connection to the Parallels Service will
   be refused.
   Parameters
   hDispConfig :        A handle of type PHT_DISP_CONFIG.
   nMinSecurityLevel :  Security level to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetMinSecurityLevel
                                                                 */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDispCfg_SetMinSecurityLevel, (
		PRL_HANDLE hDispConfig,
		PRL_SECURITY_LEVEL nMinSecurityLevel
		) );

/* Obtains a handles list of operations which required administrator confirmation
   ( for all users and for all virtual machines( if operation is applicable to vm) )

Parameters

   Parameters
   hDispConfig :   A handle of type PHT_DISP_CONFIG.
   phConfirmList : [out] A pointer to a variable that receives
                   the handle. The type of the handle is PHT_OPAQUE_TYPE_LIST
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				PrlDispCfg_GetConfirmationsList, (
				PRL_HANDLE hDispConfig,
				PRL_HANDLE_PTR phConfirmList
				) );

/* Obtains a handles list of operations which required administrator confirmation
	( for all users and for all virtual machines( if operation is applicable to vm) )
Parameters

   Parameters
   hDispConfig :  A handle of type PHT_DISP_CONFIG.
   hConfirmList : variable that receives the handle. The type of the handle is PHT_OPAQUE_TYPE_LIST.
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				PrlDispCfg_SetConfirmationsList, (
				PRL_HANDLE hDispConfig,
				PRL_HANDLE hConfirmList
				) );


/* \Returns the default backup server host name or IP address.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   sBackupServer :           [out] Pointer to a buffer that receives
                             the backup server host name or IP address
							 (a UTF\-8 encoded, null\-terminated string).
   pnBackupServerBufLength : [in] The size of the output buffer (in
                             bytes). Set the buffer pointer to null
                             and this value to zero to receive the
                             required size. [out] The required
                             \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_GetDefaultBackupServer, (
		PRL_HANDLE hDispConfig,
		PRL_STR sBackupServer,
		PRL_UINT32_PTR pnBackupServerBufLength
		) );


/* \Sets the default backup server host name or IP address.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   sBackupServer :           [in] The backup server host name or IP address
							 (a UTF\-8 encoded, null\-terminated string).
							 Can be null or empty in case when an empty
							 value should be assigned.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_SetDefaultBackupServer, (
		PRL_HANDLE hDispConfig,
		PRL_CONST_STR sBackupServer
		) );

/* \Returns the backup temporary directory.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   sBackupTmpDir :           [out] Pointer to a buffer that receives
                             the backup temporary directory
							 (a UTF\-8 encoded, null\-terminated string).
   pnBackupTmpDirBufLength : [in] The size of the output buffer (in
                             bytes). Set the buffer pointer to null
                             and this value to zero to receive the
                             required size. [out] The required
                             \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				PrlDispCfg_GetBackupTmpDir, (
		PRL_HANDLE hDispConfig,
		PRL_STR sBackupTmpDir,
		PRL_UINT32_PTR pnBackupTmpDirBufLength
		) );

/* \Sets the backup temporary directory.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   sBackupTmpDir :           [in] The backup temporary directory
							 (a UTF\-8 encoded, null\-terminated string).
							 Can be null or empty in case when an empty
							 value should be assigned.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				PrlDispCfg_SetBackupTmpDir, (
		PRL_HANDLE hDispConfig,
		PRL_CONST_STR sBackupTmpDir
		) );

/* \Returns the backup user login.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   sUserLogin :			     [out] Pointer to a buffer that receives
                             the backup user login
							 (a UTF\-8 encoded, null\-terminated string).
   pnUserLoginBufLength :    [in] The size of the output buffer (in
                             bytes). Set the buffer pointer to null
                             and this value to zero to receive the
                             required size. [out] The required
                             \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_GetBackupUserLogin, (
		PRL_HANDLE hDispConfig,
		PRL_STR sUserLogin,
		PRL_UINT32_PTR pnUserLoginBufLength
		) );


/* \Sets the backup user login.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   sUserLogin :              [in] The backup user login
							 (a UTF\-8 encoded, null\-terminated string).
							 Can be null or empty in case when an empty
							 value should be assigned.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_SetBackupUserLogin, (
		PRL_HANDLE hDispConfig,
		PRL_CONST_STR sUserLogin
		) );


/* \Sets the backup user password.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   sUserPassword :           [in] The backup user password
							 (a UTF\-8 encoded, null\-terminated string).
							 Can be null or empty in case when an empty
							 value should be assigned.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_SetBackupUserPassword, (
		PRL_HANDLE hDispConfig,
		PRL_CONST_STR sUserPassword
		) );


/* \Returns the backup user password enabling sign.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   pbUserPasswordEnabled :	 [out] Pointer to a buffer that receives
                             the backup user password enabling sign
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_IsBackupUserPasswordEnabled, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL_PTR pbUserPasswordEnabled
		) );


/* \Sets the backup user password.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   bUserPasswordEnabled :    [in] The backup user password enabling sign
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_SetBackupUserPasswordEnabled, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL bUserPasswordEnabled
		) );


/* \Returns PATH to the default backup directory.
   Parameters
   hDispConfig :                A handle of type PHT_DISP_CONFIG.
   sBackupDirectory :           [out] Pointer to a buffer that receives
                                PATH to the backup directory
                                (a UTF\-8 encoded, null\-terminated string).
   pnBackupDirectoryBufLength : [in] The size of the output buffer (in
                                bytes). Set the buffer pointer to null
                                and this value to zero to receive the
                                required size. [out] The required
                                \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_GetDefaultBackupDirectory, (
		PRL_HANDLE hDispConfig,
		PRL_STR sBackupDirectory,
		PRL_UINT32_PTR pnBackupDirectoryBufLength
		) );

/* \Sets PATH to the default backup directory.
   Parameters
   hDispConfig :                A handle of type PHT_DISP_CONFIG.
   sBackupDirectory :           [in] PATH to the backup directory
                                (a UTF\-8 encoded, null\-terminated string).
								Can be null or empty in case when
								an empty value should be assigned.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_SetDefaultBackupDirectory, (
		PRL_HANDLE hDispConfig,
		PRL_CONST_STR sBackupDirectory
		) );

/* Get backup timeout in seconds.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   pnTimeout  :              [out] A pointer to a buffer that receives
                             the result
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
				PrlDispCfg_GetBackupTimeout, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32_PTR pnBackupTimeout
		) );

/* Set backup timeout in seconds.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   nTimeout   :              [in] timeout in seconds
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
				PrlDispCfg_SetBackupTimeout, (
		PRL_HANDLE hDispConfig,
		PRL_UINT32 nTimeout
		) );


/* \Determines whether the plugins enabled or not
   Parameters
   hDispConfig :                A handle of type PHT_DISP_CONFIG.
   pbIsPluginsAllowed :        [out] Pointer to a variable that receives the result.
                PRL_TRUE indicates that the plugins support is allowed
                PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
				 PrlDispCfg_ArePluginsEnabled, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL_PTR pbIsPluginsAllowed
		) );

/* \Enable or disable the plugins support
   Parameters
   hDispConfig :                A handle of type PHT_DISP_CONFIG.
   bEnablePluginsSupport :     [in] A value to set.
                               Use PRL_TRUE to enable, PRL_FALSE to disable plugins support.

   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
				 PrlDispCfg_EnablePlugins, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL bEnablePluginsSupport
		) );

/* %VM_ONLY%

   \Returns VM CPU limit type.
   Parameters
   hDispConfig :                A handle of type PHT_DISP_CONFIG.
   pnVmCpuLimitType :           [out] Pointer to a buffer that receives
                                Vm CPU limit type
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
				 PrlDispCfg_GetVmCpuLimitType,  (
		PRL_HANDLE hDispConfig,
		PRL_UINT32_PTR pnVmCpuLimitType
		) );

/* %VM_ONLY%

   Parameters
   hDispConfig :                A handle of type PHT_DISP_CONFIG.
   nVmCpuLimitType:             [in] VM CPU limit type value
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( PARALLELS_API_VER_4,
				 PrlDispCfg_SetVmCpuLimitType,  (
		PRL_HANDLE hDispConfig,
		PRL_UINT32 nVmCpuLimitType
		) );

/* \Returns default encryption plugin Id.
   Parameters
   hDispConfig :                A handle of type PHT_DISP_CONFIG.
   sDefaultPluginId :           [out] Pointer to a buffer that receives
                                default encryption plugin Id
                                (a UTF\-8 encoded, null\-terminated string).
								It is empty string for default Parallels encryption plugin

   pnDefaultPluginIdBufLength : [in] The size of the output buffer (in
                                bytes). Set the buffer pointer to null
                                and this value to zero to receive the
                                required size. [out] The required
                                \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlDispCfg_GetDefaultEncryptionPluginId, (
		PRL_HANDLE hDispConfig,
		PRL_STR sDefaultPluginId,
		PRL_UINT32_PTR pnDefaultPluginIdBufLength
		) );

/* \Sets default encryption plugin Id.
   Parameters
   hDispConfig :                A handle of type PHT_DISP_CONFIG.
   sDefaultPluginId :           [in] default encryption plugin Id
                                (a UTF\-8 encoded, null\-terminated string).
								Can be null or empty in case when
								an empty value should be assigned.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlDispCfg_SetDefaultEncryptionPluginId, (
		PRL_HANDLE hDispConfig,
		PRL_CONST_STR sDefaultPluginId
		) );

/* Determines whether the verbose log level is configured for
   dispatcher and virtual machines processes.
   Parameters
   hDispConfig: A handle of type PHT_DISP_CONFIG.
   pbEnabled: [out] A pointer to a variable that receives the result.
   PRL_TRUE indicates that the verbose log level is configured.
   PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed. */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_IsVerboseLogEnabled, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL_PTR pbEnabled
		) );

/* \Enables or disables the verbose log level for dispatcher and virtual machines processes.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   bEnabled   :              [in] Action type. Specify PRL_TRUE to enable the verbose debug log level.
                             Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlDispCfg_SetVerboseLogEnabled, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL bEnabled
		) );


/* Determines whether the Allow Multiple PMC option is enabled.
   Parameters
   hDispConfig: A handle of type PHT_DISP_CONFIG.
   pbEnabled:   [out] A pointer to a variable that receives the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.
   PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
				 PrlDispCfg_IsAllowMultiplePMC, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL_PTR pbEnabled
		) );

/* Enables or disables the Allow Multiple PMC option.
   Parameters
   hDispConfig : A handle of type PHT_DISP_CONFIG.
   bEnabled   :  [in] Action type. Specify PRL_TRUE to enable
                 the Allow Multiple PMC option.
                 Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
				 PrlDispCfg_SetAllowMultiplePMC, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL bEnabled
		) );

/* Determines whether the log rotation is enabled for
   dispatcher and virtual machine logs.
   Parameters
   hDispConfig: A handle of type PHT_DISP_CONFIG.
   pbEnabled: [out] A pointer to a variable that receives the result.
   PRL_TRUE indicates that the log rotation is enabled.
   PRL_FALSE indicates otherwise.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed. */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlDispCfg_IsLogRotationEnabled, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL_PTR pbEnabled
		) );

/* Enables or disables the log rotation for dispatcher and virtual machine logs.
   Parameters
   hDispConfig :             A handle of type PHT_DISP_CONFIG.
   bEnabled   :              [in] Action type. Specify PRL_TRUE to enable the log rotation.
                             Specify PRL_FALSE to disable it.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlDispCfg_SetLogRotationEnabled, (
		PRL_HANDLE hDispConfig,
		PRL_BOOL bEnabled
		) );

/* Determines number of USB devices known to the Parallels Service.
   Parameters
   hDispCfg :  A handle of type PHT_DISP_CONFIG.
   pnUsbIdentityCount: [out] A pointer to a variable that receives result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - either hDispCfg is not a valid PHT_DISP_CONFIG

   PRL_SUCCESS - function completed successfully */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlDispCfg_GetUsbIdentityCount, (
		PRL_HANDLE hDispCfg,
		PRL_UINT32_PTR pnUsbIdentityCount
		) );

/* \Returns handle of type PHT_USB_IDENTITY storing information of
   USB device with specified index.
   Parameters
   hDispCfg :  A handle of type PHT_DISP_CONFIG.
   nUsbIdentIndex: A numeric index identifying object with information
		about USB device. To determine total number of such objects
		use PrlDispCfg_GetUsbIdentityCount
	phUsbIdent : [out] a pointer to a buffer receiving PHT_USB_IDENTITY handle
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle, wrong index or null pointer
		was passed

   PRL_ERR_OUT_OF_MEMORY - insufficient memory to complete operation

   PRL_ERR_SUCCESS - function completed successively */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlDispCfg_GetUsbIdentity, (
		PRL_HANDLE hDispCfg,
		PRL_UINT32 nUsbIdentIndex,
		PRL_HANDLE_PTR phUsbIdent
		) );


/* (Un)Associates USB device with given Vm.
   Parameters
   hDispCfg :  A handle of type PHT_DISP_CONFIG.
   sSystemName: System name / id of USB device
   sVmUuid: Uuid of Vm to associate USB device with.
	It could be NULL or empty to clear association.
   nFlags  :  Reserved parameter.
   Returns
   PRL_RESULT. Posible values:

   PRL_ERR_INVALID_ARG - invalid handle or
	invalid system name was passed.

   PRL_ERR_OUT_OF_MEMORY - insufficient memory to complete operation

	PRL_ERR_SUCCESS - function completed successfully */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlDispCfg_SetUsbIdentAssociation, (
		PRL_HANDLE hDispCfg,
		PRL_CONST_STR sSystemName,
		PRL_CONST_STR sVmUuid,
		PRL_UINT32 nFlags
		) );


/* Allows to get the CPU features masks from dispatcher config.
   hDispConfig   :  A handle of type PHT_DISP_CONFIG.
   phCpuFeatures :  A pointer to a variable that receives a handle of type PHT_CPU_FEATURES.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate new object.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_SetCpuFeaturesMaskEx */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
			PrlDispCfg_GetCpuFeaturesMaskEx, (
		PRL_HANDLE hDispConfig,
		PRL_HANDLE_PTR phCpuFeatures
		) );

/* Allows to set the CPU features masks. This masks will be saved at dispatcher config
   and will be used on VM start.
   Parameters
   hDispConfig  :  A handle of type PHT_DISP_CONFIG.
   hCpuFeatures :  A handle of type PHT_CPU_FEATURES.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlDispCfg_GetCpuFeaturesMaskEx */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
			PrlDispCfg_SetCpuFeaturesMaskEx, (
		PRL_HANDLE hDispConfig,
		PRL_HANDLE hCpuFeatures
		) );

/* Creates a new handle of type PHT_CPU_FEATURES.
   Parameters
   phCpuFeatures :  [out] A pointer to a variable that receives
                    the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the
   handle.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
			PrlCpuFeatures_Create, (
		PRL_HANDLE_PTR phCpuFeatures
		) );

/* Returns value of the specific cpu feature.
   Parameters
   hCpuFeatures :  A handle of type PHT_CPU_FEATURES.
   nId          :  Id of specific feature.
   pnValue      :  Pointer for storing result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer or invalid feature id was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlCpuFeatures_SetValue */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
			PrlCpuFeatures_GetValue, (
		PRL_HANDLE hCpuFeatures,
		PRL_CPU_FEATURES_EX nId,
		PRL_UINT32_PTR pnValue
		) );

/* Sets value of the specific feature.
   Parameters
   hCpuFeatures :  A handle of type PHT_CPU_FEATURES.
   nId          :  Id of specific feature.
   nValue       :  Setting value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or invalid feature id was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlCpuFeatures_GetValue */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
			PrlCpuFeatures_SetValue, (
		PRL_HANDLE hCpuFeatures,
		PRL_CPU_FEATURES_EX nId,
		PRL_UINT32 nValue
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Parallels license extracting info operations set
///////////////////////////////////////////////////////////////////////////////

/* Determines if the license is valid.
   Parameters
   hLicense :  A handle of type PHT_LICENSE identifying the
               license.
   pbValue :   [out] A pointer to a variable that receives the
               \result. PRL_TRUE indicates that the license is
               valid. PRL_FALSE indicates that the license is
               invalid.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the specified license object does not
   contain license data.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLic_IsValid, (
		PRL_HANDLE hLicense,
		PRL_BOOL_PTR pbValue
		) );


/* \Returns the license status.
   Parameters
   hLicense :  A handle of type PHT_LICENSE identifying the
               license.
   pnValue :   [out] A pointer to a variable that receives the
               license status.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the specified license object does not
   contain license data.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLic_GetStatus, (
		PRL_HANDLE hLicense,
		PRL_RESULT_PTR pnValue
		) );


/* \Returns the license key string.
   Parameters
   hLicense :               A handle of type PHT_LICENSE
                            identifying the license.
   sLicenseKey :            [out] A pointer to a buffer that
                            receives the result (a UTF\-8
                            encoded, null\-terminated string).
   pnLicenseKeyBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this parameter's value to
                            zero to receive the required size. [out]
                            The required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_NO_DATA - the specified license object does not
   contain license data.

   PRL_ERR_SUCCESS - function completed successfully.                    */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLic_GetLicenseKey, (
		PRL_HANDLE hLicense,
		PRL_STR sLicenseKey,
		PRL_UINT32_PTR pnLicenseKeyBufLength
		) );

/* \Returns the license user name.
   Parameters
   hLicense :             A handle of type PHT_LICENSE
                          identifying the license.
   sUserName :            [out] A pointer to a buffer that
                          receives the user name (a UTF\-8
                          encoded, null\-terminated string).
   pnUserNameBufLength :  [in] The size of the output buffer (in
                          bytes). Set the buffer pointer to null
                          and this parameter's value to zero to
                          receive the required size. [out]
                          The required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_NO_DATA - the specified license object does not
   contain license data.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLic_GetUserName, (
		PRL_HANDLE hLicense,
		PRL_STR sUserName,
		PRL_UINT32_PTR pnUserNameBufLength
		) );

/* \Returns the name of the company to which this license was
   issued.
   Parameters
   hLicense :                A handle of type PHT_LICENSE
                             identifying the license.
   sCompanyName :            [out] A pointer to a buffer that
                             receives the name (a UTF\-8 encoded,
                             null\-terminated string).
   pnCompanyNameBufLength :  [in] The size of the output buffer
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

   PRL_ERR_NO_DATA - the specified license object does not
   contain license data.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLic_GetCompanyName, (
		PRL_HANDLE hLicense,
		PRL_STR sCompanyName,
		PRL_UINT32_PTR pnCompanyNameBufLength
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Parallels Service info extracting operations set
///////////////////////////////////////////////////////////////////////////////

/* \Returns the name of the machine hosting the specified Parallels Service.
   Parameters
   hServerInfo :          A handle of type PHT_SERVER_INFO
                          containing the Parallels Service information.
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

   PRL_ERR_NO_DATA - the PHT_SERVER_INFO object does not contain
   any data.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvInfo_GetHostName, (
		PRL_HANDLE hServerInfo,
		PRL_STR sHostName,
		PRL_UINT32_PTR pnHostNameBufLength
		) );

/* \Returns the version of the host operating system.
   Parameters
   hServerInfo :           A handle of type PHT_SERVER_INFO
                           containing the Parallels Service information.
   sOsVersion :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnOsVersionBufLength :  [in] The size of the output buffer (in
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

   PRL_ERR_NO_DATA - the PHT_SERVER_INFO object does not contain
   any data.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvInfo_GetOsVersion, (
		PRL_HANDLE hServerInfo,
		PRL_STR sOsVersion,
		PRL_UINT32_PTR pnOsVersionBufLength
		) );

/* \Returns the port number at which the Parallels Service is listening for requests.
   Parameters
   hServerInfo :  A handle of type PHT_SERVER_INFO containing the
                  Parallels Service information.
   pnPort :       [out] A pointer to a variable that receives the
                  port number.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the PHT_SERVER_INFO object does not contain
   any data.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvInfo_GetCmdPort, (
		PRL_HANDLE hServerInfo,
		PRL_UINT32_PTR pnPort
		) );


/* \Returns the host machine UUID (universally unique ID).
   Parameters
   hServerInfo :            A handle of type PHT_SERVER_INFO
                            containing the Parallels Service information.
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

   PRL_ERR_NO_DATA - the PHT_SERVER_INFO object does not contain
   any data.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvInfo_GetServerUuid, (
		PRL_HANDLE hServerInfo,
		PRL_STR sServerUuid,
		PRL_UINT32_PTR pnServerUuidBufLength
		) );

/* \Returns the Parallels product version number.
   Parameters
   hServerInfo :                A handle of type PHT_SERVER_INFO
                                containing the Parallels Service
                                information.
   sProductVersion :            [out] A pointer to a buffer that
                                receives the result (a UTF\-8
                                encoded, null\-terminated
                                string).
   pnProductVersionBufLength :  [in] The size of the output
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

   PRL_ERR_NO_DATA - the PHT_SERVER_INFO object does not contain
   any data.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlSrvInfo_GetProductVersion, (
		PRL_HANDLE hServerInfo,
		PRL_STR sProductVersion,
		PRL_UINT32_PTR pnProductVersionBufLength
		) );

/* \Returns the application mode of server.
   Parameters
   hServerInfo :  A handle of type PHT_SERVER_INFO containing the
                  Parallels Service information.
   pnAppMode :    [out] A pointer to a variable that receives the
                  server application mode.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the PHT_SERVER_INFO object does not contain
   any data.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlSrvInfo_GetApplicationMode, (
		PRL_HANDLE hServerInfo,
		PRL_APPLICATION_MODE_PTR pnApplicationMode
		) );

/* \Returns the service start time in milliseconds since Epoch (01-01-1970).
   Parameters
   hServerInfo :  A handle of type PHT_SERVER_INFO containing the
                  Parallels Service information.
   pnStartTime :  [out] A pointer to a variable that receives the
                  service start time in milliseconds.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the PHT_SERVER_INFO object does not contain
   any data.

   PRL_ERR_SUCCESS - function completed successfully.

   See Also
   PrlSrvInfo_GetStartTimeMonotonic
   PrlHandle_GetPackageId                                         */

PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlSrvInfo_GetStartTime, (
		PRL_HANDLE hServerInfo,
		PRL_UINT64_PTR pnStartTime
		) );

/* \Returns the service start time in milliseconds (monotonic time clock).
   Parameters
   hServerInfo          : A handle of type PHT_SERVER_INFO containing the
                          Parallels Service information.
   pnStartTimeMonotonic : [out] A pointer to a variable that receives the
                          service start time in milliseconds from monotonic time clock.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - the PHT_SERVER_INFO object does not contain
   any data.

   PRL_ERR_SUCCESS - function completed successfully.

   See Also
   PrlSrvInfo_GetStartTime
   PrlHandle_GetPackageId                                         */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlSrvInfo_GetStartTimeMonotonic, (
		PRL_HANDLE hServerInfo,
		PRL_UINT64_PTR pnStartTimeMonotonic
		) );


/* \Returns session UUID string (used to restore a session).
   Parameters
   hLoginResp :              A handle of type PHT_LOGIN_RESPONSE
                             containing the login response info.
   sSessionUuid :            [out] A pointer to a buffer that
                             receives the session UUID (a UTF\-8
                             encoded, null\-terminated string).
   pnSessionUuidBufLength :  [in] The size of the output buffer
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLoginResponse_GetSessionUuid, (
	PRL_HANDLE hLoginResp,
	PRL_STR sSessionUuid,
	PRL_UINT32_PTR pnSessionUuidBufLength
	) );

/* \Returns the host machine UUID.
   Parameters
   hLoginResp :             A handle of type PHT_LOGIN_RESPONSE
                            containing the login response info.
   sServerUuid :            [out] A pointer to a buffer that
                            receives the Parallels Service UUID (a UTF\-8
                            encoded, null\-terminated string).
   pnServerUuidBufLength :  [in] The size of the output buffer
                            (in bytes). Set the buffer pointer to
                            null and this parameter's value to
                            zero to receive the required size. [out]
                            The required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.                    */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLoginResponse_GetServerUuid, (
	PRL_HANDLE hLoginResp,
	PRL_STR sServerUuid,
	PRL_UINT32_PTR pnServerUuidBufLength
	) );

/* \Returns the host OS version.
   Parameters
   hLoginResp :                A handle of type
                               PHT_LOGIN_RESPONSE containing the
                               login response info.
   sHostOsVersion :            [out] A pointer to a buffer that
                               receives the OS version (a UTF\-8
                               encoded, null\-terminated string).
   pnHostOsVersionBufLength :  [in] The size of the output buffer
                               (in bytes). Set the buffer pointer
                               to null and this parameter's value
                               to zero to receive the required
                               size. [out] The required
                               \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLoginResponse_GetHostOsVersion, (
	PRL_HANDLE hLoginResp,
	PRL_STR sHostOsVersion,
	PRL_UINT32_PTR pnHostOsVersionBufLength
	) );

/* \Returns the Parallels product version number.
   Parameters
   hLoginResp :                 A handle of type
                                PHT_LOGIN_RESPONSE containing the
                                login response info.
   sProductVersion :            [out] A pointer to a buffer that
                                receives the product version (a
                                UTF\-8 encoded, null\-terminated
                                string).
   pnProductVersionBufLength :  [in] The size of the output
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLoginResponse_GetProductVersion, (
	PRL_HANDLE hLoginResp,
	PRL_STR sProductVersion,
	PRL_UINT32_PTR pnProductVersionBufLength
	) );

/* \Returns the total number of PHT_RUNNING_TASK handles
   contained in the PHT_LOGIN_RESPONSE object. See the
   PrlLoginResponse_GetRunningTaskByIndex function for more
   information.
   Parameters
   hLoginResp :  A handle of type PHT_LOGIN_RESPONSE containing
                 the login response info.
   pnCount :     [out] A pointer to a variable that receives the
                 size of the array.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLoginResponse_GetRunningTaskCount, (
	PRL_HANDLE hLoginResp,
	PRL_UINT32_PTR pnCount
	) );


/* Obtains a handle of type PHT_RUNNING_TASK. If a previous
   session ID was passed to the corresponding login operation,
   the PHT_LOGIN_RESPONSE object will contain a list of the
   tasks that were initiated during the previous session and are
   still running (if any). A task ID (that can be retrieved
   using this function) can be passed to the
   PrlSrv_AttachToLostTask function to get a handle to the task,
   thus gaining control to it.
   Parameters
   hLoginResp :     A handle of type PHT_LOGIN_RESPONSE
                    containing the login response info.
   nIndex :         An index of the task to obtain the handle for
                    (starting at 0). To determine the total
                    number of the available tasks, use the
                    PrlLoginResponse_GetRunningTaskCount
                    function.
   phRunningTask :  [out] A pointer to a variable that receives
                    the handle of type PHT_RUNNING_TASK
                    containing the task information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlLoginResponse_GetRunningTaskByIndex, (
	PRL_HANDLE hLoginResp,
	PRL_UINT32 nIndex,
	PRL_HANDLE_PTR phRunningTask
	) );

/* \Returns the task UUID (universally unique ID).
   Parameters
   hRunningTask :         A handle of type PHT_RUNNING_TASK
                          identifying the task.
   sTaskUuid :            [out] A pointer to a buffer that
                          receives the task UUID (a UTF\-8
                          encoded, null\-terminated string).
   pnTaskUuidBufLength :  [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlRunningTask_GetTaskUuid, (
	PRL_HANDLE hRunningTask,
	PRL_STR sTaskUuid,
	PRL_UINT32_PTR pnTaskUuidBufLength
	) );

/* Determines the task type.
   Parameters
   hRunningTask :  A handle of type PHT_RUNNING_TASK identifying
                   the task.
   pnType :        [out] A pointer to a variable that receives
                   the task type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlRunningTask_GetTaskType, (
	PRL_HANDLE hRunningTask,
	PRL_RUNNING_TASK_TYPE_ENUM_PTR pnType
	) );

/* \Returns task parameters as a string. Value returns in UTF-8
   encoding. Null char ('0') adding to the end of string.
   Parameters
   hRunningTask :           A handle of type PHT_RUNNING_TASK
                            identifying the task.
   sParamsStr :             [out] A pointer to a buffer that
                            receives the result (a UTF\-8
                            encoded, null\-terminated string).
   pnsParamsStrBufLength :  [in] The size of the output buffer
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlRunningTask_GetTaskParametersAsString, (
	PRL_HANDLE hRunningTask,
	PRL_STR sParamsStr,
	PRL_UINT32_PTR pnsParamsStrBufLength
	) );


/* \Returns system name for USB device.
   Parameters
   hUsbIdent :  A handle of type PHT_USB_IDENTITY.
   sSystemName:  [out] A pointer to a buffer that receives
						   the result (a UTF\-8 encoded,
						   null\-terminated string).
   pnSystemNameBufLength:     [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlUsbIdent_GetSystemName, (
		PRL_HANDLE hUsbIdent,
		PRL_STR sSystemName,
		PRL_UINT32_PTR pnSystemNameBufLength
		) );

/* \Returns user friendly name of USB device.
   Parameters
   hUsbIdent :  A handle of type PHT_USB_IDENTITY.
   sFriendlyName:  [out] A pointer to a buffer that receives
						   the result (a UTF\-8 encoded,
						   null\-terminated string).
   pnFriedlyNameBufLength:     [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlUsbIdent_GetFriendlyName, (
		PRL_HANDLE hUsbIdent,
		PRL_STR sFriendlyName,
		PRL_UINT32_PTR pnFriendlyNameBufLength
		) );

/* \Returns Uuid of Vm associated with USB device or
   empty string if no association is specified.
   Parameters
   hUsbIdent :  A handle of type PHT_USB_IDENTITY.
   sVmUuid:     [out] A pointer to a buffer that receives
						   the result (null\-terminated string).
   pnVmUuidBufLength:     [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlUsbIdent_GetVmUuidAssociation, (
		PRL_HANDLE hUsbIdent,
		PRL_STR sVmUuid,
		PRL_UINT32_PTR pnVmUuidBufLength
		) );


/* \Returns the vendor of the plugin.
   Parameters
   hPluginInfo : A handle of type PHT_PLUGIN_INFO
                 identifying the plugin info.
   sVendor     : [out] A pointer to a buffer that receives
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

   PRL_ERR_NO_DATA - no information about this field.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
			PrlPluginInfo_GetVendor, (
		PRL_HANDLE hPluginInfo,
		PRL_STR sVendor,
		PRL_UINT32_PTR pnBufLength
		) );

/* \Returns copyright of the plugin.
   Parameters
   hPluginInfo : A handle of type PHT_PLUGIN_INFO
                 identifying the plugin info.
   sCopyright  : [out] A pointer to a buffer that receives
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

   PRL_ERR_NO_DATA - no information about this field.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
			PrlPluginInfo_GetCopyright, (
		PRL_HANDLE hPluginInfo,
		PRL_STR sCopyright,
		PRL_UINT32_PTR pnBufLength
		) );

/* \Returns short description about the plugin.
   Parameters
   hPluginInfo : A handle of type PHT_PLUGIN_INFO
                 identifying the plugin info.
   sDescr      : [out] A pointer to a buffer that receives
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

   PRL_ERR_NO_DATA - no information about this field.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
			PrlPluginInfo_GetShortDescription, (
		PRL_HANDLE hPluginInfo,
		PRL_STR sDescr,
		PRL_UINT32_PTR pnBufLength
		) );

/* \Returns long description about the plugin.
   Parameters
   hPluginInfo : A handle of type PHT_PLUGIN_INFO
                 identifying the plugin info.
   sDescr      : [out] A pointer to a buffer that receives
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

   PRL_ERR_NO_DATA - no information about this field.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
			PrlPluginInfo_GetLongDescription, (
		PRL_HANDLE hPluginInfo,
		PRL_STR sDescr,
		PRL_UINT32_PTR pnBufLength
		) );

/* \Returns the version of the plugin.
   Parameters
   hPluginInfo : A handle of type PHT_PLUGIN_INFO
                 identifying the plugin info.
   sVersion     : [out] A pointer to a buffer that receives
                 the result (a UTF\-8 encoded,
                 null\-terminated string).
				 Format version is "Major.Minor.Build".
   pnBufLength : [in] The size of the output buffer (in
                 bytes). Set the buffer pointer to null
                 and this parameter's value to zero to
                 receive the required size. [out] The
                 required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - no information about this field.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
			PrlPluginInfo_GetVersion, (
		PRL_HANDLE hPluginInfo,
		PRL_STR sVersion,
		PRL_UINT32_PTR pnBufLength
		) );

/* \Returns identificator of the plugin in GUID format.
   Parameters
   hPluginInfo : A handle of type PHT_PLUGIN_INFO
                 identifying the plugin info.
   sId       : [out] A pointer to a buffer that receives
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

   PRL_ERR_NO_DATA - no information about this field.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
			PrlPluginInfo_GetId, (
		PRL_HANDLE hPluginInfo,
		PRL_STR sId,
		PRL_UINT32_PTR pnBufLength
		) );


/* \Returns the UUID (universally unique ID) of the created backup
   Parameters
   hBackupResult :         A handle of type PHT_BACKUP_RESULT
                           identifying the information about created backup.
   sBackupUuid :           [out] A pointer to a buffer that receives
                     	   the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnBackupUuidBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlBackupResult_GetBackupUuid, (
		PRL_HANDLE hBackupResult,
		PRL_STR sBackupUuid,
		PRL_UINT32_PTR pnBackupUuidBufLength
		) );

/* Obtains a handle to cpu pool information

Parameters

   Parameters
   hDispConfig :   A handle of type PHT_DISP_CONFIG.
   phCpuPool :     [out] A pointer to a variable that receives the handle.
		   The type of the handle is PHT_CPU_POOL. If node is
		   not in cpu pool handle will be PRL_INVALID_HANDLE

   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( PARALLELS_API_VER_5,
				PrlDispCfg_GetCpuPool, (
		PRL_HANDLE hDispConfig,
		PRL_HANDLE_PTR phCpuPool
		) );


/* \Returns the name of the pool
   Parameters
   hCpuPool :          A handle of type PHT_CPU_POOL
   sBuf :            [out] Pointer to a buffer that receives
                          the pool vendor (a UTF\-8 encoded,
                          null\-terminated string).
   pnBufLength :  [in] The size of the output buffer (in
                          bytes). Set the buffer pointer to null
                          and this value to zero to receive the
                          required size. [out] The required
                          \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlCPUPool_GetVendor, (
		PRL_HANDLE hCpuPool,
		PRL_STR sBuf,
		PRL_UINT32_PTR pnBufLength
		) );

/* \Returns the vendor of the pool
   Parameters
   hCpuPool :          A handle of type PHT_CPU_POOL
   sBuf :            [out] Pointer to a buffer that receives
                          the pool name (a UTF\-8 encoded,
                          null\-terminated string).
   pnBufLength :  [in] The size of the output buffer (in
                          bytes). Set the buffer pointer to null
                          and this value to zero to receive the
                          required size. [out] The required
                          \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlCPUPool_GetName, (
		PRL_HANDLE hCpuPool,
		PRL_STR sBuf,
		PRL_UINT32_PTR pnBufLength
		) );

/* Obtains a handle to cpu pool information

Parameters

   Parameters
   hCpuPool :          A handle of type PHT_CPU_POOL
   ph :                [out] A pointer to a variable that receives the handle.
                     The type of the handle is PHT_CPU_FEATURES
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate new object.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				PrlCPUPool_GetCpuFeaturesMask, (
		PRL_HANDLE hCpuPool,
		PRL_HANDLE_PTR phCpuFeatures
		) );

/* Obtains a list of handles of type PHT_CPU_POOL
   containing information about all existing cpu pools.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParamByIndex function to obtain a handle
   of type PHT_CPU_POOL containing the information about
   a cpu pool.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */

PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_5,
				PrlSrv_GetCPUPoolsList, (
		PRL_HANDLE hServer
		) );

/* Join server to the default cpu pool

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   hCpuPool : A handle of type PHT_CPU_POOL, defines
              the pool where server should be moved.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */

PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_5,
				PrlSrv_JoinCPUPool, (
		PRL_HANDLE hServer
		) );

/* Moves server to a different cpu pool

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   sCpuPool : name of the pool where server should be moved.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */

PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_5,
				PrlSrv_MoveToCPUPool, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sCpuPool
		) );

/* Leaves server from any cpu pool

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */

PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_5,
				PrlSrv_LeaveCPUPool, (
		PRL_HANDLE hServer
		) );

/* Recalculates features in pool

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   sCpuPool : name of the pool where features should
              be recalculated.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */

PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_5,
				PrlSrv_RecalculateCPUPool, (
		PRL_HANDLE hServer,
		PRL_CONST_STR sCpuPool
		) );

#ifdef _WIN_
    #pragma pack(pop, save_api_pack)
#endif

#ifdef __cplusplus
}
#endif

#endif // __PARALLELS_API_DISPATCHER_H__
