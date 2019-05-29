/*
 * PrlApiCore.h
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


#ifndef __PARALLELS_API_CORE_H__
#define __PARALLELS_API_CORE_H__

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
/// manipulation of the api library and are not specific
/// to objects in the object hierarchy.
///////////////////////////////////////////////////////////////////////////////

#include "PrlApiVersion.h"


///////////////////////////////////////////////////////////////////////////////
/// @section Initialization SDK functionality.
///////////////////////////////////////////////////////////////////////////////

/* Initializes API library. This function must be
   called after the dynamic libraries are loaded and before
   making any of the API calls. Internally, this call creates
   the internal event-loop and initializes the library state.
   Before exiting the program, the PrlApi_Deinit function must
   be called to perform internal library de-initialization.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_DOUBLE_INIT - library has been already initialized.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to initialize the
   library.

   PRL_ERR_API_INCOMPATIBLE - SDK version is incompatible
   with version in param.

   PRL_ERR_SUCCESS - operation completed successfully.
   See Also
   PrlApi_Deinit
   Parameters
   version :  Library version number. This should be
               PARALLELS_API_VER.                              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlApi_Init, (
		PRL_UINT32 version
		) );

/* This is extended version of PrlApi_Init call that can be used
   to specify explicitly application mode in which the API
   library should function.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_DOUBLE_INIT - library has been already initialized.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to initialize the
   library.

   PRL_ERR_API_INCOMPATIBLE - SDK version is incompatible with
   \version in param.

   PRL_ERR_SUCCESS - operation completed successfully.
   See Also
   PrlApi_Deinit PrlApi_Init
   Parameters
   nVersion :    Library version number. This should be
                 PARALLELS_API_VER.
   nAppMode :    API library application mode.
   nFlags   :    Flags as mask ( from enums PRL_API_INIT_FLAGS and PRL_API_COMMAND_FLAGS )
   nReserved :  reserved parameter for future purposes.         */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlApi_InitEx, (
		PRL_UINT32 nVersion,
    PRL_APPLICATION_MODE nAppMode,
		PRL_UINT32 nFlags,
    PRL_UINT32 nReserved
		) );

/* De-initializes the library. Before exiting a client program,
   this function must be called to perform internal library
   de-initialization. Internally, this call will lead to the
   destruction of the internal event-loop and de-initialization
   of the library state.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_UNINITIALIZED - the library was not initialized.

   PRL_ERR_SUCCESS - operation completed successfully.
   See Also
   PrlApi_Init                                                  */

PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlApi_Deinit, (void) );

/* \Returns the API version number. Version number is
   incremented each time a public interfaces is changed. Our
   goal is to achieve full backward compatibility where the
   changes add new functionality without breaking the existing
   one.
   Parameters
   current_version :  [out] Current version of the API.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlApi_GetVersion, (
		PRL_UINT32_PTR current_version
		) );

/* \Returns the API application mode.
   Parameters
   app_mode :  [out] Current application mode of the API.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.       */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlApi_GetAppMode, (
		PRL_APPLICATION_MODE_PTR app_mode
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Result code description functionality.
///////////////////////////////////////////////////////////////////////////////

/* Evaluates a return code and returns a description of the
   problem. All synchronous functions in the API
   return PRL_RESULT - a numeric return code used to evaluate
   the success or failure of the operation. If the return code
   indicates a failure, the PrlApi_GetResultDescription function
   can be used to obtain the description of the problem.
   Parameters
   nErrCode :              A return code to get the description
                           for.
   bIsBriefMessage :       Specifies whether to obtain a long or
                           a brief description of the problem.
                           PRL_TRUE specifies to obtain a brief
                           message. PRL_FALSE specifies to obtain
                           a long message.
   bFormated:              Specifies whether HTML tags used for text formatting
                           should be included in the resulting string.
                           PRL_TRUE - include HTML tags; PRL_FALSE - do not
                           include the tags.
   sErrString :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnErrStringBufLength :  [in] The size of the output buffer (in
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
				 PrlApi_GetResultDescription, (
		PRL_RESULT nErrCode,
		PRL_BOOL bIsBriefMessage,
        PRL_BOOL bFormated,
		PRL_STR sErrString,
		PRL_UINT32_PTR pnErrStringBufLength
		) );

/* Evaluates the specified error code and returns its
   classification (warning, question, information, etc.).
   Parameters
   nErrCode :   The error code to evaluate.
   pnMsgType :  [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlApi_GetMessageType, (
		PRL_RESULT nErrCode,
		PRL_MESSAGE_TYPE_ENUM_PTR pnMsgType
		) );

/* The PrlApi_MsgCanBeIgnored function allows to evaluate an
   error code and determine if the error is critical (i.e.
   whether it can be ignored).
   Parameters
   nErrCode :        Error code to evaluate.
   pbCanBeIgnored :  [out] A pointer to a variable that receives
                     the result. PRL_TRUE indicates that the
                     error is not critical. PBL_FALSE indicates
                     that the error is critical and should be
                     properly handled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlApi_MsgCanBeIgnored, (
		PRL_RESULT nErrCode,
		PRL_BOOL_PTR pbCanBeIgnored
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Handles operating routines.
///
/// API is designed to allow clients to access most objects through handles.
/// Handles are represented by the PRL_HANDLE opaque data type.
/// For most object types, provided the special routine that creates or opens the object,
/// provides a handle to the caller.
/// The caller then uses that handle in subsequent operations on the object.
/// All handles are allocated in the 'callee creates - caller frees' model and should
/// be explecitely be freed by the special client with PrlHandle_Free call.
///////////////////////////////////////////////////////////////////////////////

/* Increases the specified handle reference count by one.
   Handles used within the API are thread safe.
   They can be used in multiple threads at the same time. To
   maintain the proper reference counting, the count should be
   increased each time a handle is passed to another thread by
   calling the PrlHandle_AddRef function. If this is not done,
   freeing a handle in one thread may destroy it while other
   threads are still using it.
   Parameters
   handle :  Handle to duplicate.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlHandle_Free                                                */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlHandle_AddRef, (
		PRL_HANDLE handle
		) );

/* Frees the specified handle. Handles used within the
   API are reference counted. Each handle contains a
   count of the number of references to it held by other
   objects. A corresponding object stays in memory for as long
   as the count is greater than zero. A client application is
   responsible for freeing any handles that are no longer
   required. A handle can be freed using the PrlHandle_Free
   function. The function decreases the reference count by one.
   When the count reaches zero, the object is destroyed. Failing
   to free a handle after it has been used will result in a
   memory leak.
   Parameters
   handle :  Handle to free.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlHandle_Free, (
		PRL_HANDLE handle
		) );

/* \Returns the type of the specified handle. Handle is an
   abstraction that is used to hide an object implementation.
   While there are many methods used to access functionality of
   different objects, this method is used to identify object
   type as one of the PRL_HANDLE_TYPE types.
   Parameters
   handle :  Handle to get the type for.
   type :    [out] A pointer to a variable that receives the
             \result. The type is returned as an enumerator of
             the PRL_HANDLE_TYPE enumeration.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlHandle_GetType, (
		PRL_HANDLE handle,
		PRL_HANDLE_TYPE_PTR type
		) );


/* Registers event handler.

   Parameters
   hObj     : [in] A handle of type PHT_SERVER, PHT_VIRTUAL_MACHINE,
			  PHT_DESKTOP_CONTROL or PHT_CVSRC.
   handler  : [in] User defined function that will be called for each event.
   userData : [in] User defined data that will be passed to handler.

   Returns
   PRL_RESULT. Possible values:
   PRL_ERR_UNINITIALIZED - SDK was not properly initialized.
   PRL_ERR_INVALID_ARG - specified arguments are invalid.
   PRL_ERR_OUT_OF_MEMORY - not enough memory.
   PRL_ERR_SUCCESS - function completed successfully.                         */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
			PrlHandle_RegEventHandler, (
		PRL_HANDLE hObj,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		) );

/* Unregisters event handler.

   Parameters
   hObj     : [in] A handle of type PHT_SERVER, PHT_VIRTUAL_MACHINE,
			  PHT_DESKTOP_CONTROL or PHT_CVSRC.
   handler  : [in] User defined function specified in previous call to
              PrlHandle_RegEventHandler().
   userData : [in] User defined data specified in previous call to
			  PrlHandle_RegEventHandler().

   Returns
   PRL_RESULT. Possible values:
   PRL_ERR_UNINITIALIZED - SDK was not properly initialized.
   PRL_ERR_INVALID_ARG - specified arguments are invalid.
   PRL_ERR_OUT_OF_MEMORY - not enough memory.
   PRL_ERR_SUCCESS - function completed successfully.                         */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
			PrlHandle_UnregEventHandler, (
		PRL_HANDLE hObj,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR userData
		) );

/* Retrieves IO channel statistics (throughput, delay) for correspond virtual
   machine or remote desktop control object.

   Parameters
   hObj        :   A handle of type PHT_SERVER, PHT_VIRTUAL_MACHINE or PHT_DESKTOP_CONTROL.
   channelStat :   A pointer to the statistics structure

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.
   PRL_ERR_NO_DATA     - mostly indicates there is no connection to remote
   desktop now available. I.e. PrlDevDisplay_ConnectToVm was not called.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_4,
				 PrlHandle_GetChannelStatistics, (
		PRL_HANDLE hObj,
		PRL_IO_CHANNEL_STAT_PTR channelStat
		) );


/* \Returns the numeric identifier of the package that caused the event.
   The numeric identifier is a monotonically increasing number. It is reset to
   one on dispatcher restart. If event or job was not originated in the
   dispatcher, then the numeric identifier is equal to zero.
   A triplet of values <dispatcher start time, monotonic dispatcher start time, numeric identifier>
   forms a unique monotonically increasing numeric identifier that could be used
   for event ordering. See PrlSrvInfo_GetStartTime and PrlSrvInfo_GetStartTimeMonotonic
   for details on how to obtain (monotonic) dispatcher start time value.

   Parameters
   hEvent :  A handle of type PHT_EVENT or PHT_JOB.
   id :    [out] A pointer to a variable that receives the result
             (event id).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlSrvInfo_GetStartTime
   PrlSrvInfo_GetStartTimeMonotonic                             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlHandle_GetPackageId, (
		PRL_HANDLE hObj,
		PRL_UINT64_PTR id
		) );


/**
 * Client can use this method to set XML representation to given SDK object
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid handle or non valid (NULL) pointer to XML
 *                                    representation string was passed
 * PRL_ERR_UNIMPLEMENTED            - method is not implemented or does not supported XML serialization
 * PRL_ERR_SUCCESS					- operation successfully completed
 */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlHandle_FromString, (
		PRL_HANDLE hHandle,
		PRL_CONST_STR sXml
		) );


/**
 * Client can use this method to get XML representation from given SDK object
 * This method must be eliminated at Release stage
 * To delete an XML pointer use PrlBuffer_Free method.
 *
 * @param valid PRL_HANDLE
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid handle or non valid (NULL) pointer to storing result was passed
 * PRL_ERR_UNIMPLEMENTED            - method is not implemented or does not supported XML serialization
 * PRL_ERR_SUCCESS                  - operation successfully completed
 */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlHandle_ToString, (
		PRL_HANDLE hHandle,
		PRL_VOID_PTR_PTR ppXml
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Strings list object manipulations
///////////////////////////////////////////////////////////////////////////////

/* Creates a new handle of type PHT_STRINGS_LIST.
   Parameters
   phStringsList :  [out] A pointer to a variable that receives
                    the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the
   handle.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlApi_CreateStringsList, (
		PRL_HANDLE_PTR phStringsList
		) );


/* Determines the number of strings in the container.
   Parameters
   hStringsList :  A handle of type PHT_STRINGS_LIST identifying
                   the string container.
   pnItemsCount :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStrList_GetItemsCount, (
		PRL_HANDLE hStringsList,
		PRL_UINT32_PTR pnItemsCount
		) );


/* Adds a new string to the container.
   Parameters
   hStringsList :  A handle of type PHT_STRINGS_LIST identifying
                   the string container.
   sItem :         A pointer to the string to add to the
                   container. The value must be a UTF\-8 encoded,
                   null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStrList_AddItem, (
		PRL_HANDLE hStringsList,
		PRL_CONST_STR sItem
		) );


/* Extracts the specified string from the container.
   Parameters
   hStringsList :   A handle of type PHT_STRINGS_LIST identifying
                    the string container.
   nItemIndex :     A numeric index identifying the string
                    (beginning with 0). To determine the number
                    of string in the container, use the PrlStrList_GetItemsCount
                    function.
   sItemBuf :       [out] A pointer to a buffer that receives the
                    \result (a UTF\-8 encoded, null\-terminated
                    string).
   pnItemBufSize :  [in] The size of the output buffer (in
                    bytes). Set the buffer pointer to null and
                    this parameter's value to zero to receive the
                    required size. [out] The required output
                    buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.                            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStrList_GetItem, (
		PRL_HANDLE hStringsList,
		PRL_UINT32 nItemIndex,
		PRL_STR sItemBuf,
		PRL_UINT32_PTR pnItemBufSize
		) );


/* Removes the specified string from the container.
   Parameters
   hStringsList :  A handle of type PHT_STRINGS_LIST identifying
                   the string container.
   nItemIndex :    A numeric index identifying the string. Index
                   is a sequential number assigned to every item
                   in the container. To determine the index of an
                   item, call the PrlStrList_GetItem in a loop
                   (from 0 to PrlStrList_GetItemsCount()). The
                   iteration number is the index of the
                   corresponding item.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStrList_RemoveItem, (
		PRL_HANDLE hStringsList,
		PRL_UINT32 nItemIndex
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Handles list object manipulations
///////////////////////////////////////////////////////////////////////////////

/* Creates a new handle of type PRL_HANDLES_LIST.
   Parameters
   phHandlesList :  A pointer to a variable that receives the new
                    handle.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - inavlid (NULL) pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the new
   handle.

   PRL_ERR_SUCCESS - operation completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlApi_CreateHandlesList, (
		PRL_HANDLE_PTR phHandlesList
		) );


/* Determines the total number of handles in the specified
   container. After determining the count, use the
   PrlHndlList_GetItem function to retrieve the handle from the
   container.
   Parameters
   hHandlesList :  A handle of type PHT_HANDLES_LIST identifying
                   the container.
   pnItemsCount :  [out] A pointer to a variable that receives
                   the result. Handle count begins with zero.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or out of range index
   value was passed.

   PRL_ERR_SUCCESS - operation completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlHndlList_GetItemsCount, (
		PRL_HANDLE hHandlesList,
		PRL_UINT32_PTR pnItemsCount
		) );


/* Adds the specified handle to the container.
   Parameters
   hHandlesList :  A handle of type PHT_HANDLES_LIST identifying
                   the container.
   hItem :         A handle to add to the container. No
                   validation of any kind is performed when
                   adding a handle to the list.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - operation completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlHndlList_AddItem, (
		PRL_HANDLE hHandlesList,
		PRL_HANDLE hItem
		) );


/* Retrieves a handle from the container.
   Parameters
   hHandlesList :  A handle of type PHT_HANDLES_LIST identifying
                   the container.
   nItemIndex :    A numeric index of the handle to retrieve
                   (beginning with zero). To get the total number
                   of handles in the container, use the
                   PrlHndlList_GetItemsCount function.
   phItem :        [out] A pointer to a variable that receives
                   the handle.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or out of range index
   value was passed.

   PRL_ERR_SUCCESS - operation completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlHndlList_GetItem, (
		PRL_HANDLE hHandlesList,
		PRL_UINT32 nItemIndex,
		PRL_HANDLE_PTR phItem
		) );


/* Removes a handle from the container.
   Parameters
   hHandlesList :  A handle of type PHT_HANDLES_LIST identifying
                   the container.
   nItemIndex :    A numeric index of the handle to remove
                   (beginning with zero). To get the total number
                   of handles in the container, use the
                   PrlHndlList_GetItemsCount function.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or out of range index
   value was passed.

   PRL_ERR_SUCCESS - operation completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlHndlList_RemoveItem, (
		PRL_HANDLE hHandlesList,
		PRL_UINT32 nItemIndex
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Opaque type list object manipulations
///////////////////////////////////////////////////////////////////////////////

/* Creates a new handle of type PHT_OPAQUE_TYPE_LIST. Simple
   sample of method usage follow:

   PrlApi_CreateOpTypeList(&hOpTypeList, sizeof(PRL_UINT32));

   Parameters
   phOpTypeList :  [out] A pointer to a variable that receives
                    the handle.
   nTypeSize    :  size of opaque type for which list should be
				   instantiated
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed or size of not supported
						 opaque type was passed. In order to determine
						 list of supported opaque types please refer with
						 PrlTypes.h.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the
						   list object.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlApi_CreateOpTypeList, (
		PRL_HANDLE_PTR phOpTypeList,
		PRL_SIZE nTypeSize
		) );


/* Determines the number of items in the container.
   Parameters
   hOpTypeList :  A handle of type PHT_OPAQUE_TYPE_LIST identifying
                   the opaque type container.
   pnItemsCount :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlOpTypeList_GetItemsCount, (
		PRL_HANDLE hOpTypeList,
		PRL_UINT32_PTR pnItemsCount
		) );


/* Adds a new item to the container.
   Parameters
   hOpTypeList :  A handle of type PHT_OPAQUE_TYPE_LIST identifying
                   the opaque type container.
   pnItem :       A pointer to the item to add to the container. Note
				  that value passing through pointer in order to
				  prevent loosing of precision (for 64 bit types for
				  example).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer to value variable was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlOpTypeList_AddItem, (
		PRL_HANDLE hOpTypeList,
		PRL_CONST_VOID_PTR pnItem
		) );


/* Extracts the specified item from the container.
   Parameters
   hOpTypeList :   A handle of type PHT_OPAQUE_TYPE_LIST identifying
                   the opaque type container.
   nItemIndex :    A numeric index identifying the item
                   (beginning with 0). To determine the number
                   of items in the container, use the PrlOpTypeList_GetItemsCount
                   function.
   pnItem      :  [in] The pointer to the buffer to extract item value.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer or
   out of range index was passed.

   PRL_ERR_SUCCESS - function completed successfully.                            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlOpTypeList_GetItem, (
		PRL_HANDLE hOpTypeList,
		PRL_UINT32 nItemIndex,
		PRL_VOID_PTR pnItem
		) );


/* Removes the specified item from the container.
   Parameters
   hOpTypeList :  A handle of type PHT_OPAQUE_TYPE_LIST identifying
                   the opaque type container.
   nItemIndex :    A numeric index identifying the item. Index
                   is a sequential number assigned to every item
                   in the container. To determine the index of an
                   item, call the PrlOpTypeList_GetItem in a loop
                   (from 0 to PrlOpTypeList_GetItemsCount()). The
                   iteration number is the index of the
                   corresponding item.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or out of range index was passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlOpTypeList_RemoveItem, (
		PRL_HANDLE hOpTypeList,
		PRL_UINT32 nItemIndex
		) );


/* Extracts the size of type of items storing in container.
   Parameters
   hOpTypeList :   A handle of type PHT_OPAQUE_TYPE_LIST identifying
                   the opaque type container.
   pnTypeSize :    [out] The pointer to the buffer to extract type size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.                            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlOpTypeList_GetTypeSize, (
		PRL_HANDLE hOpTypeList,
		PRL_SIZE_PTR pnTypeSize
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Jobs handling routines and data types.
///
/// Some methods in the SDK are known to take a lot of time to execute.
/// 'Job' abstraction is intended to allow users to perfrom asynchronous calls.
/// Caller can perform a call that returns a handle representing job object.
///////////////////////////////////////////////////////////////////////////////

/* The PrlJob_Wait function allows to use asynchronous functions
   synchronously. The function should be called immediately
   after the corresponding asynchronous function call. It
   suspends the main thread for the specified number of
   milliseconds and waits for the job to complete. If the job
   completes before the timeout value is reached, the function
   \returns and the execution of the main thread continues
   normally. If the function times out, it returns an error. On
   function return, the job object will not be destroyed and
   could be queried until its destruction with the call to
   PrlHandle_Free.
   Parameters
   hJob :   A handle of type PHT_JOB identifying the job.
   msecs :  Timeout in milliseconds. The timeout must be long
            enough for the job to complete. The optimal value is
            determined empirically. For an infinite timeout, use
            the UINT_MAX value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - an invalid PHT_JOB handle was passed.

   PRL_ERR_TIMEOUT - the specified timeout limit was reached.

   PRL_ERR_SUCCESS - operation completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlJob_Wait, (
		PRL_HANDLE hJob,
		PRL_UINT32 msecs
		) );


/* Cancel the specified job. If an asynchronous operation takes
   a long time to complete, you may cancel it by calling this
   function. The call does not destroy the job object.
   Parameters
   hJob :  A handle of type PHT_JOB identifying the job.
   Returns
   PRL_HANDLE to the job being canceled.

   PRL_INVALID_HANDLE - not enough memory to instantiate
   asynchronous operation.

   The results are retrieved using the PrlJob_GetRetCode
   function. Possible values:

   PRL_ERR_INVALID_ARG - invalid PHT_JOB handle was passed.

   PRL_ERR_UNINITIALIZED - job instance was not bound to Dispatcher Service
   connection.                                                  */
PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_1,
					   PrlJob_Cancel, (
		PRL_HANDLE hJob
		) );

/* Obtains the current status of the specified job.
   Parameters
   hJob :    A handle if type PHT_JOB identifying the job.
   status :  [out] A pointer to a variable that receives the
             \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlJob_GetStatus, (
		PRL_HANDLE hJob,
		PRL_JOB_STATUS_PTR status
		) );

/* Obtains a job progress report. The job progress is reported
   as percentage of completion.
   Parameters
   handle :      valid PRL_HANDLE
   percentage :  [out] A pointer to a variable that receives the
                 \result (an integer value from 0 to 100).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlJob_GetProgress, (
		PRL_HANDLE hJob,
		PRL_UINT32_PTR percentage
		) );

/* Obtains the return code from the specified job object. On
   asynchronous operation completion, the job object will
   contain a return code indicating whether the operation
   succeeded or failed. Different jobs may return different
   error codes. The most common codes are: PRL_ERR_INVALID_ARG
   (invalid input parameters were specified in the asynchronous
   function call) and PRL_ERR_SUCCESS (function successfully
   completed).
   Parameters
   hJob :     A handle of type PHT_JOB identifying the job.
   retcode :  [out] A pointer to a variable that receives the
              return code.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlJob_GetRetCode, (
		PRL_HANDLE hJob,
		PRL_RESULT_PTR retcode
		) );


/* The PrlJob_GetDataPtr function is used to obtain a pointer to
   a buffer containing the results of an asynchronous operation.
   Different operations may or may not provide a data buffer as
   a result of their execution. This function should be used
   only when an operation results in a buffer populated with the
   data specific to that operation.
   Parameters
   handle :     A handle of type PHT_JOB identifying the job.
   data_ptr :   [out] A pointer to a buffer that receives the
                data. Pass a null pointer to determine the
                required buffer size.
   data_size :  [in] The size of the output buffer (in bytes).
                Set the buffer pointer to null and this
                parameter's value to zero to receive the required
                size. [out] The required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter used to specify the size will
   contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlJob_GetDataPtr, (
		PRL_HANDLE hJob,
		PRL_VOID_PTR data_ptr,
		PRL_UINT32_PTR data_size
		) );

/* Obtains a handle of type PHT_RESULT containing the result
   returned by the job. Depending on the job, the PHT_RESULT
   object may contain a handle, a list of handles, a string, or
   a list of strings. The description of each asynchronous
   function in this documentation contains information whether
   the function returns a PHT_RESULT object or not, and provides
   instructions on how to obtain it.
   Parameters
   hJob :    A handle of type PHT_JOB identifying the job.
   handle :  [out] pointer to the job result handle
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OPERATION_PENDING - the asynchronous operation that
   this job refers to is still in progress.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlJob_GetResult, (
		PRL_HANDLE hJob,
		PRL_HANDLE_PTR handle
		) );


/* Provides additional job error information. If a job returned
   an error code (see PrlJob_GetRetCode), you may call this
   function to see if additional error information is available.
   If the information is available, the function returns with PRL_ERR_SUCCESS
   and obtains a handle of type PHT_EVENT containing the
   information. To extract the information from the object, use
   the PrlEvent_GetErrString function.
   Parameters
   hJob :     A handle of type PHT_JOB identifying the job.
   phError :  [out] A pointer to a variable that receives a
              handle of type PHT_EVENT containing the error
              information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - no additional error information is
   available.

   PRL_ERR_UNIMPLEMENTED - the functionality is not available
   for this job type.

   PRL_ERR_SUCCESS - function completed successfully.                         */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlJob_GetError, (
		PRL_HANDLE hJob,
		PRL_HANDLE_PTR phError
		) );

/* \Returns the job operation code. A job operation code can be
   used to identify the function that started the corresponding
   operation and to determine the type of the result the job
   object contains. Use this function in an event handler
   (callback function) when examining a received job object. For
   \example, the operation code PJOC_SRV_LOGIN means that this
   job belongs to login operation (PrlSrv_Login,
   PrlSrv_LoginLocal); the PJOC_SRV_GET_VM_LIST code means that
   the job was started by the PrlSrv_GetVmList function call and
   contains a list of virtual machines, etc.
   Parameters
   handle :   A handle of type PHT_JOB identifying the job.
   pnOpCode :  [out] A pointer to a variable that receives the
              \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlJob_GetOpCode, (
		PRL_HANDLE hJob,
		PRL_JOB_OPERATION_CODE_PTR pnOpCode
		) );


/* \Returns the sign whether request was sent to remote side or
   was aborted by some local reasons (wrong params passed or
   something else).
   Parameters
   handle :   A handle of type PHT_JOB identifying the job.
   pbIsOwner :  [out] A pointer to a variable that receives the
              \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlJob_IsRequestWasSent, (
		PRL_HANDLE hJob,
		PRL_BOOL_PTR pbIsRequestWasSent
		) );


/* Determines the number of items in the specified result
   object.
   Parameters
   hResult :  A handle of type PHT_RESULT identifying the result
              object.
   pCount :   [out] A pointer to a variable that receives the
              \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlResult_GetParamsCount, (
		PRL_HANDLE hResult,
		PRL_UINT32_PTR pCount
				 ) );


/* Obtains a handle from the result object identified by the
   index. Use this function when the result contains (or may
   contain) multiple objects. To obtain multiple handles from
   the result object, first, determine the number of handles
   contained in the object using the PrlResult_GetParamsCount
   function. Then, iterate through the list and use the
   iteration number as the value of the nIndex parameter.
   Parameters
   handle :   A handle of type PHT_RESULT identifying the result
              object.
   nIndex :   An index identifying the handle in the list.
   phParam :  [out] A pointer to a variable that receives the
              handle. The type of handle returned depends on the
              corresponding operation. The description of each
              asynchronous function in this documentation
              provides information about the type of handle that
              the function returns.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlResult_GetParamByIndex, (
		PRL_HANDLE hResult,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phParam
		) );


/* Obtains a handle to an object containing the results of the
   corresponding asynchronous operation. Use this function when
   you know that the corresponding function always produces a
   single object as a result. This function is the equivalent of
   the PrlResult_GetParamByIndex(hResult, 0, &amp;Handle) call
   (i.e. zero as the index value).
   Parameters
   hResult :  A handle of type PHT_RESULT identifying the result
              object.
   pHandle :  [out] A pointer to a variable that receives the
              handle. The type of handle returned depends on the
              corresponding operation. The description of each
              asynchronous function in this documentation
              provides information about the type of handle that
              the function returns.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_NO_DATA - no result or the result is not a single
   handle.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlResult_GetParam, (
				 PRL_HANDLE hResult,
				 PRL_HANDLE_PTR pHandle
				 ) );


/* Obtains a string result from the result object identified by
   the index. Use this function when the result contains (or may
   contain) multiple string items. To obtain multiple items from
   the result object, first, determine the number of items
   contained in the object using the PrlResult_GetParamsCount
   function. Then, iterate through the list and use the
   iteration number as the value of the nIndex parameter.
   Parameters
   hResult :         A handle of type PHT_RESULT identifying the
                     \result object.
   nIndex :          An index identifying the string item in the
                     list.
   sParamBuf :       [out] A pointer to a buffer that receives
                     the result (a UTF\-8 encoded,
                     null\-terminated string).
   pnParamBufSize :  [in] The size of the output buffer (in
                     bytes). Set the buffer pointer to null and
                     this parameter's value to zero to receive
                     the required size. [out] The required output
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
				 PrlResult_GetParamByIndexAsString, (
				 PRL_HANDLE hResult,
				 PRL_UINT32 nIndex,
				 PRL_STR sParamBuf,
				 PRL_UINT32_PTR pnParamBufSize
				 ) );


/* Obtains a string result from the result object. Use this
   function when you know that the corresponding function always
   produces a single string as a result. This function is the
   equivalent of the PrlResult_GetParamByIndexAsString(hResult,
   0, &amp;myString) call (i.e. zero as the index value).
   Parameters
   hResult :         A handle of type PHT_RESULT identifying the
                     \result object.
   sParamBuf :       [out] A pointer to a buffer that receives
                     the result (a UTF\-8 encoded,
                     null\-terminated string).
   pnParamBufSize :  [in] The size of the output buffer (in
                     bytes). Set the buffer pointer to null and
                     this parameter's value to zero to receive
                     the required size. [out] The required output
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
				 PrlResult_GetParamAsString, (
				 PRL_HANDLE hResult,
				 PRL_STR sParamBuf,
				 PRL_UINT32_PTR pnParamBufSize
				 ) );


///////////////////////////////////////////////////////////////////////////////
/// @section Event management routines and declarations.
///
/// Events are recieved by the client in the form of event 'handles'
/// There is a number of different event types and client can recieve
/// them by registering his 'callbacks' with the library.
///////////////////////////////////////////////////////////////////////////////

/* \Returns the type of the event.
   Parameters
   hEvent :  A handle of type PHT_EVENT identifying the event.
   type :    [out] A pointer to a variable that receives the result
             (event type).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.               */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetType, (
		PRL_HANDLE hEvent,
		PRL_EVENT_TYPE_PTR type
		) );

/* Clients can use this method to receive a data buffer specific
   to the event type. Different event types may provide
   different data buffers or they may provide none.
   Parameters
   hEvent :    A handle of type PHT_EVENT identifying the event.
   data_ptr :  [out] A pointer to the event\-specific data.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetDataPtr, (
		PRL_HANDLE hEvent,
		PRL_VOID_PTR_PTR data_ptr
		) );


/* Obtains a handle of type PHT_SERVER identifying the Dispatcher Service
   that generated the event. When processing an event
   inside a callback function (event handler) this function is
   the only way to get the server handle.
   Parameters
   hEvent :  A handle of type PHT_EVENT identifying the event.
   server :  [out] A pointer to a variable that receives the
             server handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetServer, (
		PRL_HANDLE hEvent,
		PRL_HANDLE_PTR server
		) );


/* Obtains a handle of type PHT_VIRTUAL_MACHINE referencing the
   virtual machine that generated the event. When processing an
   event inside a callback function (event handler) this
   function is the only way to get the virtual machine handle.
   Parameters
   hEvent :  A handle of type PHT_EVENT identifying the event.
   pVm :     [out] A pointer to a variable that receives the
             virtual machine handle.
   Returns
   \ \

   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetVm, (
		PRL_HANDLE hEvent,
		PRL_HANDLE_PTR pVm
		) );


/* Obtains the handle of the job that caused the event.
   Parameters
   hEvent :  A handle of type PHT_EVENT identifying the event.
   pJob :    [out] A pointer to a variable that receives a handle
             of type PHT_JOB.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetJob, (
		PRL_HANDLE hEvent,
		PRL_HANDLE_PTR pJob
		) );


/* Determines the number of the parameters in the event
   container.
   Parameters
   hEvent :         A handle of type PHT_EVENT identifying the
                    event.
   pnParamsCount :  [out] A pointer to a variable that receives
                    the result.
   Returns
   \ \

   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_UNIMPLEMENTED - the event object does not support
   this functionality.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetParamsCount, (
		PRL_HANDLE hEvent,
		PRL_UINT32_PTR pnParamsCount
		) );

/* Obtains a handle of type PHT_EVENT_PARAMETER containing an
   event parameter data. An event container may contain multiple
   \parameters. The meaning of the parameter may be different
   for different event types.
   Parameters
   hEvent :        A handle of type PHT_EVENT identifying the
                   event.
   nIndex :        An index identifying the event parameter
                   (beginning with 0). To determine the number of
                   the parameters in the event container, use the
                   PrlEvent_GetParamsCount function.
   phEventParam :  [out] A pointer to a variable that receives
                   the handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_UNIMPLEMENTED - the event object does not support
   this functionality.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetParam, (
		PRL_HANDLE hEvent,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phEventParam
		) );

/* Obtains a handle of type PHT_EVENT_PARAMETER containing an
   event parameter data by specified parameter name. An event
   container may contain multiple parameters. The meaning of the
   parameter may be different for different event types.
   Parameters
   hEvent :        A handle of type PHT_EVENT identifying the
                   event.
   sParamName :    Event parameter name.
   phEventParam :  [out] A pointer to a variable that receives the
                   handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_UNIMPLEMENTED - the event object does not support
   this functionality.

   PRL_ERR_NO_DATA - parameter with specfied name does not
   exist.

   PRL_ERR_SUCCESS - function completed successfully.              */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetParamByName, (
		PRL_HANDLE hEvent,
		PRL_CONST_STR sParamName,
		PRL_HANDLE_PTR phEventParam
		) );

/* \Returns the code associated with the event.
   Parameters
   hEvent :     A handle of type PHT_EVENT identifying the event.
   pnErrCode :  [out] A pointer to a variable that receives the
                code.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetErrCode, (
		PRL_HANDLE hEvent,
		PRL_RESULT_PTR pnErrCode
		) );

/* \Returns string representation of the event message.
   Parameters
   hEvent :                A handle of type PHT_EVENT identifying
                           the event.
   bIsBriefMessage :       Specifies whether to return a brief or
                           a long version of the message. Specify
                           PBL_TRUE to return a brief message.
                           Specify PRL_FALSE to return a long
                           message.
   bFormated :             Specifies whether the HTML tags used for text
                           formatting should be included in the resulting
                           string. PRL_TRUE -\ include HTML tags.
                           PRL_FALSE -\ don't include.
   sErrString :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnErrStringBufLength :  [in] The size of the output buffer (in
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
				 PrlEvent_GetErrString, (
		PRL_HANDLE hEvent,
		PRL_BOOL bIsBriefMessage,
        PRL_BOOL bFormated,
		PRL_STR	sErrString,
		PRL_UINT32_PTR pnErrStringBufLength
		) );


/* Determines whether the event is critical or not.
   Parameters
   hEvent :          A handle of type PHT_EVENT identifying the
                     event.
   pbCanBeIgnored :  [out] A pointer to a variable that receives
                     the result. PRL_TRUE indicates that the
                     event message is critical and should be
                     investigated. PRL_FALSE indicates that the
                     event message is not critical and can be
                     ignored.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_CanBeIgnored, (
		PRL_HANDLE hEvent,
		PRL_BOOL_PTR pbCanBeIgnored
		) );

/* Determines whether the event requires a response from the
   client. See PrlEvent_CreateAnswerEvent for more information
   about Dispatcher Service questions.
   Parameters
   hEvent :              A handle of type PHT_EVENT identifying
                         the event.
   pbIsAnswerRequired :  [out] A pointer to a variable that
                         receives the result. PBL_TRUE indicates
                         that a response is required. PBL_TRUE
                         indicates otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_IsAnswerRequired, (
		PRL_HANDLE hEvent,
		PRL_BOOL_PTR pbIsAnswerRequired
		) );

/* \Returns the event issuer type (virtual machine, host,
   etc.).
   Parameters
   hEvent :        A handle of type PHT_EVENT identifying the
                   event.
   pnIssuerType :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_GetIssuerType, (
		PRL_HANDLE hEvent,
		PRL_EVENT_ISSUER_TYPE_PTR pnIssuerType
		) );

/* \Returns the event issuer ID (host UUID, virtual
   machine UUID, etc).
   Parameters
   hEvent :               A handle of type PHT_EVENT identifying
                          the event.
   sIssuerId :            [out] A pointer to a buffer that
                          receives the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnIssuerIdBufLength :  [in] The size of the output buffer (in
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
				 PrlEvent_GetIssuerId, (
		PRL_HANDLE hEvent,
		PRL_STR	sIssuerId,
		PRL_UINT32_PTR pnIssuerIdBufLength
		) );

/* Generates properly formatted and structured answer to a
   question included in the event. One of the event types that a
   callback function can receive is PET_DSP_EVT_VM_QUESTION. The
   event of this type is generated on the Dispatcher Service side when a user
   interaction is required. For example, let's say you've sent a
   request to the Dispatcher Service to create a virtual machine but
   specified the size of the virtual hard drive that exceeds the
   free space available on the physical hard drive. In such a
   case, the Dispatcher Service will generate an event containing a question
   (and possible answers) about how the user would like to
   proceed. In this particular case, the question will be
   similar to "The size of the specified disk is larger than the
   free space available. Would you like to create the disk
   anyway?" and the possible answers will be "Yes" or "No". The
   user must choose one of the available options, compose an
   answer, and send it to the Dispatcher Service. Based on the answer
   received, the Dispatcher Service will take the appropriate action.

   To obtain a string containing the question, use the
   PrlEvent_GetErrString function. Answer choices are acquired
   by obtaining a handle of type PHT_EVENT_PARAMETER using the
   PrlEvent_GetParam function (a single object will contain an
   individual answer choice). After the user selects an answer,
   the PrlEvent_CreateAnswerEvent function must be called to
   populate another object of type PHT_EVENT with the properly
   formatted answer data. The resulting object must then be
   passed to the PrlSrv_SendAnswer function which will send the
   answer to the Dispatcher Service.
   Parameters
   hEvent :   A handle of type PHT_EVENT identifying the event.
   phEvent :  [out] A pointer to a variable that receives the
              \result (a handle of type PHT_EVENT containing the
              answer data).
   nAnswer :  The code of the answer. The answer object data is
              assembled based on this value.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate
   event object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_CreateAnswerEvent, (
		PRL_HANDLE hEvent,
		PRL_HANDLE_PTR phEvent,
		PRL_RESULT nAnswer
		) );


/**
 * Client can use this method to set VM event XML representation to event handle
 * This method must be eliminated at Release stage
 *
 * @param valid PRL_HANDLE of PHT_EVENT type
 * @param XML configuration string
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG				- non valid PHT_EVENT handle or non valid (NULL) pointer to event XML
 * representation string was passed
 * PRL_ERR_UNIMPLEMENTED			- event object isn't supporting functionality
 * PRL_ERR_SUCCESS						- operation successfully completed
 */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvent_FromString, (
		PRL_HANDLE hEvent,
		PRL_CONST_STR sEvent
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Event parameters routines.
///////////////////////////////////////////////////////////////////////////////

/* \Returns the event parameter name.
   Parameters
   hEventParam :           A handle of type PHT_EVENT_PARAMETER
                           identifying the container.
   sParamName :            [out] A pointer to a buffer that
                           receives the result (a UTF\-8 encoded,
                           null\-terminated string).
   pnParamNameBufLength :  [in] The size of the output buffer (in
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
				 PrlEvtPrm_GetName, (
		PRL_HANDLE hEventParam,
		PRL_STR	sParamName,
		PRL_UINT32_PTR pnParamNameBufLength
		) );


/* \Returns the event parameter data type.
   Parameters
   hEventParam :  A handle of type PHT_EVENT_PARAMETER
                  identifying the container.
   pnParamType :  [out] A pointer to a variable that receives the
                  parameter type.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvtPrm_GetType, (
		PRL_HANDLE hEventParam,
		PRL_PARAM_FIELD_DATA_TYPE_PTR pnParamType
		) );


/* \Returns the event parameter value as string.
   Parameters
   hEventParam :            A handle of type PHT_EVENT_PARAMETER
                            identifying the container.
   sParamValue :            [out] A pointer to a buffer that
                            receives the result (a UTF\-8
                            encoded, null\-terminated string).
   pnParamValueBufLength :  [in] The size of the output buffer
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
				 PrlEvtPrm_ToString, (
		PRL_HANDLE hEventParam,
		PRL_STR	sParamValue,
		PRL_UINT32_PTR pnParamValueBufLength
		) );

/* \Returns the event parameter value as CDATA.
   Parameters
   hEventParam :            A handle of type PHT_EVENT_PARAMETER
                            identifying the container.
   sParamValue :            [out] A pointer to a buffer that
                            receives the result (a UTF\-8
                            encoded, null\-terminated string).
   pnParamValueBufLength :  [in] The size of the output buffer
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
				 PrlEvtPrm_ToCData, (
				PRL_HANDLE hEventParam,
				PRL_STR	sParamValue,
				PRL_UINT32_PTR pnParamValueBufLength
				) );

/* \Returns the event parameter value as PRL_UINT32.
   Parameters
   hEventParam :   A handle of type PHT_EVENT_PARAMETER
                   identifying the container.
   pnParamValue :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvtPrm_ToUint32, (
		PRL_HANDLE hEventParam,
		PRL_UINT32_PTR pnParamValue
		) );


/* \Returns the event parameter value as PRL_INT32.
   Parameters
   hEventParam :   A handle of type PHT_EVENT_PARAMETER
                   identifying the container.
   pnParamValue :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvtPrm_ToInt32, (
		PRL_HANDLE hEventParam,
		PRL_INT32_PTR pnParamValue
		) );


/* \Returns the event parameter value as PRL_UINT64.
   Parameters
   hEventParam :   A handle of type PHT_EVENT_PARAMETER
                   identifying the container.
   pnParamValue :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlEvtPrm_ToUint64, (
		PRL_HANDLE hEventParam,
		PRL_UINT64_PTR pnParamValue
		) );


/* \Returns the event parameter value as PRL_INT64.
   Parameters
   hEventParam :   A handle of type PHT_EVENT_PARAMETER
                   identifying the container.
   pnParamValue :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlEvtPrm_ToInt64, (
		PRL_HANDLE hEventParam,
		PRL_INT64_PTR pnParamValue
		) );


/* \Returns the event parameter value as boolean (PRL_BOOL).
   Parameters
   hEventParam :   A handle of type PHT_EVENT_PARAMETER
                   identifying the container.
   pbParamValue :  [out] A pointer to a variable that receives
                   the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvtPrm_ToBoolean, (
		PRL_HANDLE hEventParam,
		PRL_BOOL_PTR pbParamValue
		) );


/* \Returns the event parameter value as PRL_HANDLE.
   Parameters
   hEventParam :   A handle of type PHT_EVENT_PARAMETER
                   identifying the container.
   phParamValue :  [out] A pointer to a variable that receives
                   the handle. The handle type can be determined
                   using the PrlHandle_GetType function.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlEvtPrm_ToHandle, (
		PRL_HANDLE hEventParam,
		PRL_HANDLE_PTR phParamValue
		) );


/* \Returns the event parameter value as binary buffer.
   Parameters
   hEventParam :            A handle of type PHT_EVENT_PARAMETER
                            identifying the container.
   pParamValue :            [out] A pointer to a buffer that
                            receives the result.
   pnParamValueBufLength :  [in] The size of the output buffer
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlEvtPrm_GetBuffer, (
		PRL_HANDLE hEventParam,
		PRL_VOID_PTR pParamValue,
		PRL_UINT32_PTR pnParamValueBufLength
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Crash handler API support
///////////////////////////////////////////////////////////////////////////////

/* \Returns the name and path of the directory where
   crash dumps are stored.
   Parameters
   sPath :            [out] A pointer to a buffer that receives
                      the result (a UTF\-8 encoded,
                      null\-terminated string).
   pnPathBufLength :  [in] The size of the output buffer (in
                      bytes). Set the buffer pointer to null and
                      this parameter's value to zero to receive
                      the required size. [out] The required
                      \output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - null pointer to the buffer size was
   passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlApi_GetCrashDumpsPath, (
		PRL_STR sPath,
		PRL_UINT32_PTR pnPathBufLength
		) );

/* Initiates the standard crash dump handler. Call
   this function if you would like a dump file to be created on
   the application crash. The crush dump will be created in the
   common crush dump directory (use PrlApi_GetCrashDumpsPath
   to obtains its name and path). The crash dump file name will
   begin with the name of the crashed application and can also
   contain a user-define suffix that is passed a parameter (see
   the Parameters subsection).
   Parameters
   sCrashDumpFileSuffix :  A user\-define suffix to add to the
                           crash dump file name (UTF\-8 encoded,
                           null\-terminated string). The
                           parameter is optional and can be NULL.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_SUCCESS - function completed successfully.                  */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlApi_InitCrashHandler, (
		PRL_CONST_STR sCrashDumpFileSuffix
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the problem report object.
///////////////////////////////////////////////////////////////////////////////

/* Creates and returns a handle of type PHT_PROBLEM_REPORT,
   which is used to obtain and manage problem reports.

   Parameters
   nReportScheme : [in] the type of report that will be generated (XML based or new packed form).
   phProblemReport :  [out] A pointer to a variable that receives the handle.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer or unknown report scheme was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the problem report object.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlApi_CreateProblemReport, (
					PRL_PROBLEM_REPORT_SCHEME nReportScheme,
					PRL_HANDLE_PTR phProblemReport
		) );


/* The PrlApi_SendProblemReport lets to send previously extracted
   with PrlSrv_GetProblemReport or PrlVm_GetProblemReport calls
   problem report to the support server.
   Parameters
   sProblemReport :  problem report string representation (expected
					 null terminated string in UTF-8 encoding)
   bUseProxy	  :  sign specifies whether proxy settings should be used. Either
					 system proxy settings can be automatically by API call retrieved
					 and used or custom settings can be specified by follow params
					 (note that custom values override correspond system settings)
   sProxyHost      : [optional] if using proxy this param lets to specify
                     proxy server host address/name (can be null or empty)
   nProxyPort      : [optional] if using proxy this param lets to specify
                     proxy server port (should be 0 in order to specify to use system
					 settings)
   sProxyUserLogin : [optional] if using proxy this param lets to specify
					 user login if last one not specified at system proxy
					 settings or need to be overridden (can be null or empty)
   sProxyUserPasswd : [optional] if using proxy this param lets to specify
					  user password if last one not specified at system proxy
					  settings (can be null or empty)
   nProblemSendTimeout : timeout in milliseconds which specifies maximal awaiting
						 problem report send time (0 value not limiting problem
						 report send operation at all)
   nReserved		: [unused] reserved parameter for further purposes
   handler			: [optional] A pointer to a user callback function
			          (event handler) that will be called every time a
                      problem report sending progress changed or job completed.
   pUserData        : [optional] A pointer to a user data that will be
                      passed to the optional callback function.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */

PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlApi_SendProblemReport, (
		PRL_CONST_STR sProblemReport,
		PRL_BOOL bUseProxy,
		PRL_CONST_STR sProxyHost,
		PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin,
		PRL_CONST_STR sProxyUserPasswd,
		PRL_UINT32 nProblemSendTimeout,
		PRL_UINT32 nReserved,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR pUserData
		) );

/* The PrlApi_SendPackedProblemReport lets to send previously extracted
   with PrlSrv_GetPackedProblemReport or PrlVm_GetPackedProblemReport calls
   problem report to the support server.
   Parameters
   hProblemReport :  handle of PHT_EVENT type contains packed problem report data
   bUseProxy	  :  sign specifies whether proxy settings should be used. Either
					 system proxy settings can be automatically by API call retrieved
					 and used or custom settings can be specified by follow params
					 (note that custom values override correspond system settings)
   sProxyHost      : [optional] if using proxy this param lets to specify
                     proxy server host address/name (can be null or empty)
   nProxyPort      : [optional] if using proxy this param lets to specify
                     proxy server port (should be 0 in order to specify to use system
					 settings)
   sProxyUserLogin : [optional] if using proxy this param lets to specify
					 user login if last one not specified at system proxy
					 settings or need to be overridden (can be null or empty)
   sProxyUserPasswd : [optional] if using proxy this param lets to specify
					  user password if last one not specified at system proxy
					  settings (can be null or empty)
   nProblemSendTimeout : timeout in milliseconds which specifies maximal awaiting
						 problem report send time (0 value not limiting problem
						 report send operation at all)
   nReserved		: [unused] reserved parameter for further purposes
   handler			: [optional] A pointer to a user callback function
			          (event handler) that will be called every time a
                      problem report sending progress changed or job completed.
   pUserData        : [optional] A pointer to a user data that will be
                      passed to the optional callback function.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */

PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlApi_SendPackedProblemReport, (
		PRL_HANDLE hProblemReport,
		PRL_BOOL bUseProxy,
		PRL_CONST_STR sProxyHost,
		PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin,
		PRL_CONST_STR sProxyUserPasswd,
		PRL_UINT32 nProblemSendTimeout,
		PRL_UINT32 nReserved,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR pUserData
		) );


/* \Determines the scheme used by the problem report object.

   Parameters
   hProblemReport : [in] a handle of type PHT_PROBLEM_REPORT.
   pnReportScehme : [out] a pointer to a variable that receives the result.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer or wrong handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_GetScheme, (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_SCHEME_PTR pnReportScheme
		) );

/* \Returns problem report archive file name which can be used for
   further problem report data saving and processing. Please note that this
   data makes sense just for packed problem report scheme.
   Parameters
   hProblemReport       : [in] A handle of type PHT_PROBLEM_REPORT
                          identifying the problem report object.
   sArchiveFileName     : [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnBufLength          : [in] The size of the output buffer (in
                            bytes). Set the buffer pointer to null
                            and this parameter's value to zero to
                            receive the required size. [out] The
                            required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or old XML based scheme problem report object handle
                         or null pointer was passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_GetArchiveFileName, (
		PRL_HANDLE hProblemReport,
		PRL_STR sArchiveFileName,
		PRL_UINT32_PTR pnBufLength
		) );

/* Sets the problem report object type.

   Parameters
   hProblemReport : [in] A handle of type PHT_PROBLEM_REPORT.
   nReportType : [in] The problem report object type to set.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - wrong handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_SetType, (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_TYPE nReportType
		) );

/* Determines the type of the problem report object.

   Parameters
   hProblemReport : [in] A handle of type PHT_PROBLEM_REPORT.
   pnReportType : [out] A pointer to a variable that receives the result.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer or wrong handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_GetType, (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_TYPE_PTR pnReportType
		) );

/* Sets the value of the object property, identifying the reason for this problem report.

   Parameters
   hProblemReport : [in] A handle of type PHT_PROBLEM_REPORT.
   nReportReason  : [in] The problem report reason value.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - wrong handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_SetReason, (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_REASON nReportReason
		) );

/* Obtains the value of the property, identifying the reason for this project report.

   Parameters
   hProblemReport : [in] A handle of type PHT_PROBLEM_REPORT.
   pnReportReason : [out] A pointer to a variable that receives the result.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer or wrong handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.           */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_GetReason, (
					PRL_HANDLE hProblemReport,
					PRL_PROBLEM_REPORT_REASON_PTR pnReportReason
		) );

/* Obtains the name of the user on the problem report.
   Parameters
   hProblemReport       : [in] A handle of type PHT_PROBLEM_REPORT
                          identifying the problem report object.
   sUserName            : [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnUserNameBufLength  : [in] The size of the output buffer (in
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
				 PrlReport_GetUserName, (
		PRL_HANDLE hProblemReport,
		PRL_STR sUserName,
		PRL_UINT32_PTR pnUserNameBufLength
		) );

/* Sets the name of the user on the problem report.
   Parameters
   hProblemReport : [in] A handle of type PHT_PROBLEM_REPORT
                    identifying the problem report object.
   sNewUserName  : [in] The value to set - must be a a UTF\-8
                    encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_SetUserName, (
		PRL_HANDLE hProblemReport,
		PRL_CONST_STR sNewUserName
		) );

/* Obtains the e-mail address of the user on the problem report.
   Parameters
   hProblemReport       : [in] A handle of type PHT_PROBLEM_REPORT
                          identifying the problem report object.
   sUserEmail           : [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnUserEmailBufLength : [in] The size of the output buffer (in
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
				 PrlReport_GetUserEmail, (
		PRL_HANDLE hProblemReport,
		PRL_STR sUserEmail,
		PRL_UINT32_PTR pnUserEmailBufLength
		) );

/* Sets the e-mail address of the user on the problem report.
   Parameters
   hProblemReport : [in] A handle of type PHT_PROBLEM_REPORT
                    identifying the problem report object.
   sNewUserEmail  : [in] The value to set - must be a a UTF\-8
                    encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_SetUserEmail, (
		PRL_HANDLE hProblemReport,
		PRL_CONST_STR sNewUserEmail
		) );

/* Obtains the problem report description.
   Parameters
   hProblemReport       : [in] A handle of type PHT_PROBLEM_REPORT
                          identifying the problem report object.
   sDescription         : [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnDescriptionBufLength : [in] The size of the output buffer (in
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
				 PrlReport_GetDescription, (
		PRL_HANDLE hProblemReport,
		PRL_STR sDescription,
		PRL_UINT32_PTR pnDescriptionBufLength
		) );

/* Sets the specified problem report description.
   Parameters
   hProblemReport   : [in] A handle of type PHT_PROBLEM_REPORT
                      identifying the problem report object.
   sNewDescription  : [in] The value to set - must be a a UTF\-8
                      encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_SetDescription, (
		PRL_HANDLE hProblemReport,
		PRL_CONST_STR sNewDescription
		) );

/* \Returns the binary representation of problem report.
   Parameters
   hProblemReport :         [in] A handle of type PHT_PROBLEM_REPORT
                            identifying the container.
   pBuffer        :         [out] A pointer to a buffer that
                            receives the result.
   pnBufLength    :         [in] The size of the output buffer
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
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_GetData, (
		PRL_HANDLE hProblemReport,
		PRL_VOID_PTR pBuffer,
		PRL_UINT32_PTR pnBufLength
		) );

/* \Returns XML based string representation of problem report.
   Parameters
   hProblemReport       : [in] A handle of type PHT_PROBLEM_REPORT
                          identifying the problem report object.
   sBuffer              : [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnBufLength          : [in] The size of the output buffer (in
                          bytes). Set the buffer pointer to null
                          and this parameter's value to zero to
                          receive the required size. [out] The
                          required output buffer size.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_BUFFER_OVERRUN - the size of the output buffer is not
   large enough. The parameter that is used to specify the size
   will contain the required size.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_AsString, (
		PRL_HANDLE hProblemReport,
		PRL_STR sBuffer,
		PRL_UINT32_PTR pnBufLength
		) );

/* Assembles the problem report.
   Parameters
   hProblemReport :         [in] A handle of type PHT_PROBLEM_REPORT
                            identifying the container.
   nFlags         :         [in] flags bitset (accepts values just from
							PRL_PROBLEM_REPORT_FLAGS)

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_Assembly, (
		PRL_HANDLE hProblemReport,
		PRL_UINT32 nFlags
		) );

/* The PrlReport_Send function is provided for convenience. It allows submitting a previously
   generated problem report to the support server. The report could be generated using
   the PrlSrv_GetPackedProblemReport (PrlSrv_GetProblemReport) or
   PrlVm_GetProblemReport (PrlVm_GetPackedProblemReport) functions, or created and assembled using
   PrlApi_CreateProblemReport/PrlReport_Assembly functions.

   Parameters
   hProblemReport   : A handle of type PHT_PROBLEM_REPORT containing report data.

   bUseProxy        : Specifies whether proxy should be used. The call can obtain and
                      use the system proxy settings or you can specify your own settings.
                      Note that custom values override system settings.

   sProxyHost       : [optional] If using proxy, this parameter allows to specify the proxy server
                      host address/name (can be null or empty).

   nProxyPort       : [optional] If using proxy, this parameter allows to specify the proxy server
                      port (to use the system setting, set this parameter to 0).

   sProxyUserLogin  : [optional] If using proxy, this parameter allows to specify user login if it’s
                      not set in the system settings (can be null or empty).

   sProxyUserPasswd : [optional] If using proxy, this parameter allows to specify the user password
                      if it’s not specified in the system settings.

   nProblemSendTimeout : Timeout in milliseconds. Can be set to 0 for unlimited timeout.

   nReserved        : [unused] Reserved parameter for future use.

   handler          : [optional] A pointer to a user callback function (event handler) that will be called
                      every time a problem report sending progress is updated or when the job is completed.

   pUserData        : [optional] A pointer to a user data that will be passed to the optional callback function.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_SUCCESS - function completed successfully.            */

PRL_ASYNC_METHOD_DECL( PARALLELS_API_VER_3,
				 PrlReport_Send, (
		PRL_HANDLE hProblemReport,
		PRL_BOOL bUseProxy,
		PRL_CONST_STR sProxyHost,
		PRL_UINT32 nProxyPort,
		PRL_CONST_STR sProxyUserLogin,
		PRL_CONST_STR sProxyUserPasswd,
		PRL_UINT32 nProblemSendTimeout,
		PRL_UINT32 nReserved,
		PRL_EVENT_HANDLER_PTR handler,
		PRL_VOID_PTR pUserData
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section miscellaneous functions.
///////////////////////////////////////////////////////////////////////////////

/* Frees a buffer allocated by the API library and
   returned to the client. Every time a pointer to a buffer
   containing the data is returned to the client, it must be
   freed using this function. Failure to do so will result in
   memory leaks.
   Parameters
   buffer :  A pointer to a buffer to free.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid pointer to the buffer was
   specified.

   PRL_ERR_SUCCESS - operation completed successfully.        */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlBuffer_Free, (
		PRL_VOID_PTR buffer
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Debuging functionality.
///
/// API is designed to allow clients to access most objects through handles.
/// Handles are represented by the PRL_HANDLE opaque data type.
/// All handles are allocated in the 'callee creates - caller frees' model and should
/// be explecitely be freed by the special client with PrlHandle_Free call.
/// The next methods suite is useful in case of determining of memory leak issues.
/// They let to determine how many handles of all types instantiates in SDK lib now.
/// Or how many instantiates of concrete specified handle type. Also to determine
/// how many references count has some concrete handle instance.
///////////////////////////////////////////////////////////////////////////////


/* The PrlDbg_GetHandlesNum<b> </b>function determines how many
   handles were instantiated in the API library.
   Parameters
   handles_num :  [out] A pointer to a variable that receives the
                  \result.
   type :         specifies type of handles which instances num
                  necessary to return. If this param has value
                  PHT_ERROR then handle type is ignored and method
                  \returns common handles num. Note\: using this
                  method in case to get num of handles instances
                  of concrete type may suspends SDK lib
                  functionality on a quite long time.
   Returns
   PRL_RESULT. Possible values: PRL_ERR_INVALID_ARG - non valid
   (NULL) pointer for storing result was passed PRL_ERR_SUCCESS
   \- operation completed successfully                             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDbg_GetHandlesNum, (
		PRL_UINT32_PTR handles_num,
		PRL_HANDLE_TYPE type
		) );

/* \Returns a readable string representation of the PRL_RESULT
   value. Helpful for logging.
   Parameters
   value :    A variable of type PRL_RESULT.
   result\ :  [out] The PRL_RESULT value as a string.
   Returns
   PRL_ERR_SUCCESS                                             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDbg_PrlResultToString, (
		PRL_RESULT value,
		PRL_CONST_STR_PTR result
		) );

/* \Returns a readable string representation of the specified
   handle type. Helpful for logging.
   Parameters
   handle_type :  Handle type.
   result\ :      [out] Handle type as a string.
   Returns
   PRL_ERR_SUCCESS                                            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDbg_HandleTypeToString, (
		PRL_HANDLE_TYPE handle_type,
		PRL_CONST_STR_PTR result
		) );
/* \Returns a readable string representation of the specified
   event type. Helpful for logging.
   Parameters
   event_type :  Event type.
   result\ :     [out] Event type as a string.
   Returns
   PRL_ERR_SUCCESS                                            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlDbg_EventTypeToString, (
		PRL_EVENT_TYPE event_type,
		PRL_CONST_STR_PTR result
		) );

/* Turn on/off print logging messages on console
 *    PRL_ERR_SUCCESS - function completed successfully
 */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
					 PrlApi_SwitchConsoleLogging, (
		PRL_BOOL bSwitchOn
		) );

/* \Returns a readable string representation given guest OS ID.
	Parameters
	nGuestId :			[in] Guest OS ID
	sGuestString :		[out] Guest OS name as string.
	pnPathStringSize :	[in] The size of string containing guest
						 OS name
	Returns
	PRL_RESULT. Possible values are:

	PRL_ERR_INVALID_ARG - null pointer to the string size was
	passed.

	PRL_ERR_SUCCESS - function completed successfully.			*/
PRL_METHOD_DECL( PARALLELS_API_VER_7,
				PrlApi_GuestToString, (
		PRL_UINT32 nGuestId,
		PRL_STR sGuestString,
		PRL_UINT32_PTR pnGuestStringSize
		) );

#ifndef DYN_API_WRAP

/* This inline function returns a readable string representation
   of the PRL_RESULT value.
   Parameters
   value :  A variable of type PRL_RESULT.                       */
static inline const char * prl_result_to_string(PRL_RESULT value)
{
    const char * result ;
    PrlDbg_PrlResultToString(value, &result) ;
    return result ;
}

/* This inline function returns a readable string representation
   of the specified handle type.
   Parameters
   value :  The handle type.                                     */
static inline const char * handle_type_to_string(PRL_HANDLE_TYPE value)
{
    const char * result ;
    PrlDbg_HandleTypeToString(value, &result) ;
    return result ;
}

/* This inline function returns a readable string representation
   of the specified event type.
   Parameters
   value :  The event type.                                      */
static inline const char * event_type_to_string(PRL_EVENT_TYPE value)
{
    const char * result ;
    PrlDbg_EventTypeToString(value, &result) ;
    return result ;
}
#endif // not DYN_API_WRAP


#ifdef _WIN_
    #pragma pack(pop, save_api_pack)
#endif

#ifdef __cplusplus
}
#endif

#endif // __PARALLELS_API_CORE_H__
