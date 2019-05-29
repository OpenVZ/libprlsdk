/*
 * PrlApiNet.h
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


#ifndef __PARALLELS_API_NET_H__
#define __PARALLELS_API_NET_H__

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
/// @section Server object handling routines and data types.
///////////////////////////////////////////////////////////////////////////////

/* Obtains a list of handles of type PHT_VIRTUAL_NETWORK
   containing information about all existing virtual networks.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParamByIndex function to obtain a handle
   of type PHT_VIRTUAL_NETWORK containing the information about
   a virtual network.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   nFlags :   Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_GetVirtualNetworkList, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );


/* Adds a new virtual network to the Dispatcher Service
   configuration. To create a new virtual network, first create
   a handle of type PHT_VIRTUAL_NETWORK, configure the network
   using the handle's functions, and then call this function to
   add the network to the Dispatcher Service configuration.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Dispatcher Service.
   hVirtNet :  A handle of type PHT_VIRTUAL_NETWORK identifying
               the virtual network to add to the Dispatcher
               Service configuration.
   nFlags :    Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_GetVirtualNetworkList

   PrlSrv_DeleteVirtualNetwork

   PrlSrv_UpdateVirtualNetwork                                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_AddVirtualNetwork, (
		PRL_HANDLE hServer,
		PRL_HANDLE hVirtNet,
		PRL_UINT32 nFlags
		) );


/* Updates parameters of an existing virtual network. To get the
   return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Dispatcher Service.
   hVirtNet :  A handle of type PHT_VIRTUAL_NETWORK identifying a
               virtual network.
   nFlags :    Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_AddVirtualNetwork

   PrlSrv_GetVirtualNetworkList

   PrlSrv_DeleteVirtualNetwork                                    */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_UpdateVirtualNetwork, (
		PRL_HANDLE hServer,
		PRL_HANDLE hVirtNet,
		PRL_UINT32 nFlags
		) );


/* Removes an existing virtual network from the Dispatcher
   Service configuration. To get the return code from the
   PHT_JOB object, use the PrlJob_GetRetCode function. Possible
   values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Dispatcher Service.
   hVirtNet :  A handle of type PHT_VIRTUAL_NETWORK identifying a
               virtual network.
   nFlags :    Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_AddVirtualNetwork

   PrlSrv_GetVirtualNetworkList

   PrlSrv_UpdateVirtualNetwork                                    */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_DeleteVirtualNetwork, (
		PRL_HANDLE hServer,
		PRL_HANDLE hVirtNet,
		PRL_UINT32 nFlags
		) );


/* Starts the network service. To get the
   return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_NetServiceStart, (
		PRL_HANDLE hServer
		) );

/* Stops the network service. To get the return
   code from the PHT_JOB object, use the PrlJob_GetRetCode
   function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   unnamed_param :  A handle of type PHT_SERVER identifying the
                    Dispatcher Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_NetServiceStop, (
		PRL_HANDLE hServer
		) );

/* Restarts the network service. To get the
   return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_NetServiceRestart, (
		PRL_HANDLE hServer
		) );

/* Restores the default settings of the network
   service.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_NetServiceRestoreDefaults, (
		PRL_HANDLE hServer
		) );

/* \Returns an object of type PHT_NET_SERVICE_STATUS containing
   the network service status information.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_NET_SERVICE_STATUS containing the NET service
        status.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_GetNetServiceStatus, (
		PRL_HANDLE hServer
		) );


/* Add/Updates parameters of an offline management service.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Dispatcher Service.
   hOffmgmtService :  A handle of type PHT_OFFLINE_SERVICE identifying a
               offline management service
   nFlags :    Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_DeleteOfflineService

   PrlSrv_GetOfflineServicesList
*/
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						PrlSrv_UpdateOfflineService, (
		PRL_HANDLE hServer,
		PRL_HANDLE hOffmgmtService,
		PRL_UINT32 nFlags
		) );

/* Removes an existing offline service configuration.
   To get the return code from the
   PHT_JOB object, use the PrlJob_GetRetCode function. Possible
   values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Dispatcher Service.
   hOffmgmtService :  A handle of type PHT_OFFLINE_SERVICE identifying a
               offline management service

   nFlags :    Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_DeleteOfflineService

   PrlSrv_GetOfflineServicesList
*/

PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						PrlSrv_DeleteOfflineService, (
		PRL_HANDLE hServer,
		PRL_HANDLE hOffmgmtService,
		PRL_UINT32 nFlags
		) );

/* Obtains a list of handles of type PHT_OFFLINE_SERVICE
   containing information about all existing services.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParamByIndex function to obtain a handle
   of type PHT_OFFLINE_SERVICE containing the information about
   an individual offline service.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   nFlags :   Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						PrlSrv_GetOfflineServicesList, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );


/* Update network class classes configuration.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :		A handle of type PHT_SERVER identifying the
			Dispatcher Service.
   hNetworkClassList :	A handle of type PHT_HANDLES_LIST contains the
			list of handles of type PHT_NETWORK_CLASS
   nFlags :		Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_GetNetworkClassesList
*/
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						PrlSrv_UpdateNetworkClassesList, (
		PRL_HANDLE hServer,
		PRL_HANDLE hNetworkClassesList,
		PRL_UINT32 nFlags
		) );

/* Obtains a list of handles of type PHT_HANDLES_LIST
   containing the list of handles of type PHT_NETWORK_CLASS.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParam function to obtain a handle
   of type PHT_HANDLES_LIST
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   nFlags :   Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						PrlSrv_GetNetworkClassesList, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );


/* Update the network shaping configuration.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :		A handle of type PHT_SERVER identifying the
			Dispatcher Service.
   hNetworkShapingConfig: A handle of type PHT_NETWORK_SHAPING_CONFIG.
   nFlags :		Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_GetNetworkShapingConfig
*/
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						PrlSrv_UpdateNetworkShapingConfig, (
		PRL_HANDLE hServer,
		PRL_HANDLE hNetworkShapingConfig,
		PRL_UINT32 nFlags
		) );

/* Obtains the network shaping configuration.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParam function to obtain a handle
   of type PHT_NETWORK_SHAPING_CONFIG
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   nFlags :   Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
						PrlSrv_GetNetworkShapingConfig, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );


/* Adds a new private network to the Dispatcher Service
   configuration. To create a new private network, first create
   a handle of type PHT_IPPRIV_NET, configure the network
   using the handle's functions, and then call this function to
   add the network to the Dispatcher Service configuration.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Dispatcher Service.
   hPrivNet :  A handle of type PHT_IPPRIV_NET identifying
               the private network to add to the Dispatcher
               Service configuration.
   nFlags :    Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_GetIPPrivateNetworkList

   PrlSrv_DeleteIPPrivateNetwork

   PrlSrv_UpdateIPPrivateNetwork                                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
			PrlSrv_AddIPPrivateNetwork, (
		PRL_HANDLE hServer,
		PRL_HANDLE hPrivNet,
		PRL_UINT32 nFlags
		) );

/* Removes an existing private network from the Dispatcher
   Service configuration. To get the return code from the
   PHT_JOB object, use the PrlJob_GetRetCode function. Possible
   values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Dispatcher Service.
   hPrivNet :  A handle of type PHT_IPPRIV_NET identifying a
               private network.
   nFlags :    Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_AddIPPrivateNetwork

   PrlSrv_GetIPPrivateNetworkList

   PrlSrv_UpdateIPPrivateNetwork                                    */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
			PrlSrv_RemoveIPPrivateNetwork, (
		PRL_HANDLE hServer,
		PRL_HANDLE hPrivNet,
		PRL_UINT32 nFlags
		) );

/* Updates parameters of an existing private network. To get the
   return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :   A handle of type PHT_SERVER identifying the
               Dispatcher Service.
   hPrivNet :  A handle of type PHT_IPPRIV_NET identifying a
               private network.
   nFlags :    Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.
   See Also
   PrlSrv_AddIPPrivateNetwork

   PrlSrv_GetIPPrivateNetworksList

   PrlSrv_DeleteIPPrivateNetwork                                    */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
			PrlSrv_UpdateIPPrivateNetwork, (
		PRL_HANDLE hServer,
		PRL_HANDLE hPrivNet,
		PRL_UINT32 nFlags
		) );

/* Obtains a list of handles of type PHT_IPPRIV_NET
   containing information about all existing private networks.
   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the results from the PHT_JOB object:

   Use the PrlJob_GetResult function to obtain a handle to the
   PHT_RESULT object.

   Use the PrlResult_GetParamByIndex function to obtain a handle
   of type PHT_IPPRIV_NET containing the information about
   a private network.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
   nFlags :   Reserved parameter.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                  */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_3,
			PrlSrv_GetIPPrivateNetworksList, (
		PRL_HANDLE hServer,
		PRL_UINT32 nFlags
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Virtual Network operations
///////////////////////////////////////////////////////////////////////////////

/* Creates a new handle of type PHT_VIRTUAL_NETWORK. The handle
   is necessary to create and configure a virtual network. If
   you would like to create a new virtual network, use this
   function to create the handle first and then use the handle
   functions to configure the network. If you would like to
   modify the configuration of an existing virtual network, use
   the PrlSrv_GetVirtualNetworkList function to obtain a handle
   to the virtual network of interest.

   It is the responsibility of the caller to free the handle
   with the PrlHandle_Free call when it is no longer needed.
   Parameters
   phVirtNet :  [out] A pointer to a variable that receives the
                handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a virtual
   network object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_Create, (
		PRL_HANDLE_PTR phVirtNet
		) );

/* \Returns the ID of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sNetworkId :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnNetworkIdBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetNetworkId, (
		PRL_HANDLE hVirtNet,
		PRL_STR sNetworkId,
		PRL_UINT32_PTR pnNetworkIdBufLength
		) );

/* Sets the specified virtual network ID.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sNetworkId :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetNetworkId, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sNetworkId
		) );

/* \Returns the description of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sDescription :           [out] A pointer to a buffer that receives
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetDescription, (
		PRL_HANDLE hVirtNet,
		PRL_STR sDescription,
		PRL_UINT32_PTR pnDescriptionBufLength
		) );

/* Sets the specified virtual network description.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sDescription :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetDescription, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sDescription
		) );

/* \Returns the virtual network type. Network types
   are defined in the PRL_NET_VIRTUAL_NETWORK_TYPE enumeration.
   The following describes network types are used:
     * PVN_BRIDGED_ETHERNET
     * PVN_HOST_ONLY
   Parameters
   hVirtNet :        A handle of type PHT_VIRTUAL_NETWORK
                     identifying the virtual network.
   pnNetworkType :   [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetNetworkType, (
		PRL_HANDLE hVirtNet,
		PRL_NET_VIRTUAL_NETWORK_TYPE_PTR pnNetworkType
		) );

/* Sets the specified virtual network type. Network types
   are defined in the PRL_NET_VIRTUAL_NETWORK_TYPE enumeration.
   The following describes network types are used:
     * PVN_BRIDGED_ETHERNET
     * PVN_HOST_ONLY
   Parameters
   hVirtNet :      A handle of type PHT_VIRTUAL_NETWORK
                   identifying the virtual network.
   nNetworkType :  The value to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetNetworkType, (
		PRL_HANDLE hVirtNet,
		PRL_NET_VIRTUAL_NETWORK_TYPE nNetworkType
		) );

/* \Returns the bound card MAC address of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sBoundCardMac :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnBoundCardMacBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetBoundCardMac, (
		PRL_HANDLE hVirtNet,
		PRL_STR sBoundCardMac,
		PRL_UINT32_PTR pnBoundCardMacBufLength
		) );

/* Sets the specified virtual network bound card MAC address.
   MAC address gets by PrlSrvCfgNet_GetMacAddress functions
   for given network adapter.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sBoundCardMac :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetBoundCardMac, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sBoundCardMac
		) );

/* \Returns the name of the network adapter in the specified
   virtual network.
   Parameters
   hVirtNet :                A handle of type PHT_VIRTUAL_NETWORK
                             identifying the virtual network.
   sAdapterName :            [out] A pointer to a buffer that
                             receives the result (a UTF\-8
                             encoded, null\-terminated string).
   pnAdapterNameBufLength :  [in] The size of the output buffer
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetAdapterName, (
		PRL_HANDLE hVirtNet,
		PRL_STR sAdapterName,
		PRL_UINT32_PTR pnAdapterNameBufLength
		) );

/* Sets the specified virtual network adapter name.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sAdapterName : The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetAdapterName, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sAdapterName
		) );

/* \Returns a numeric index assigned to the network adapter in
   the specified virtual network. Special values for this parameter
   in PRL_ADAPTER_INDEX enum.
   Parameters
   hVirtNet :        A handle of type PHT_VIRTUAL_NETWORK
                     identifying the virtual network.
   pnAdapterIndex :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetAdapterIndex, (
		PRL_HANDLE hVirtNet,
		PRL_UINT32_PTR pnAdapterIndex
		) );

/* Sets the specified adapter index. Special values for this parameter
   in PRL_ADAPTER_INDEX enum.
   Parameters
   hVirtNet :       A handle of type PHT_VIRTUAL_NETWORK
                    identifying the virtual network.
   nAdapterIndex :  The value to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetAdapterIndex, (
		PRL_HANDLE hVirtNet,
		PRL_UINT32 nAdapterIndex
		) );

/* \Returns the host IP address of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sHostIPAddress :       [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnHostIPAddressBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetHostIPAddress, (
		PRL_HANDLE hVirtNet,
		PRL_STR sHostIPAddress,
		PRL_UINT32_PTR pnHostIPAddressBufLength
		) );

/* Sets the specified virtual network host IP address.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sHostIPAddress :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetHostIPAddress, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sHostIPAddress
		) );

/* \Returns the host IPv6 address of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sHostIPAddress :       [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnHostIPAddressBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_GetHostIP6Address, (
		PRL_HANDLE hVirtNet,
		PRL_STR sHostIPAddress,
		PRL_UINT32_PTR pnHostIPAddressBufLength
		) );

/* Sets the specified virtual network host IPv6 address.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sHostIPAddress :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_SetHostIP6Address, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sHostIPAddress
		) );

/* \Returns the DCHP IP address of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sDhcpIPAddress :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnDhcpIPAddressBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetDhcpIPAddress, (
		PRL_HANDLE hVirtNet,
		PRL_STR sDhcpIPAddress,
		PRL_UINT32_PTR pnDhcpIPAddressBufLength
		) );

/* Sets the specified virtual network DHCP IP address.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sDhcpIPAddress :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetDhcpIPAddress, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sDhcpIPAddress
		) );

/* \Returns the DCHP IPv6 address of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sDhcpIPAddress :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnDhcpIPAddressBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_GetDhcpIP6Address, (
		PRL_HANDLE hVirtNet,
		PRL_STR sDhcpIPAddress,
		PRL_UINT32_PTR pnDhcpIPAddressBufLength
		) );

/* Sets the specified virtual network DHCP IPv6 address.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sDhcpIPAddress :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_SetDhcpIP6Address, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sDhcpIPAddress
		) );

/* \Returns the IP net mask of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sIPNetMask :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnIPNetMaskBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetIPNetMask, (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPNetMask,
		PRL_UINT32_PTR pnIPNetMaskBufLength
		) );

/* Sets the specified virtual network IP net mask.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sIPNetMask :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetIPNetMask, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPNetMask
		) );

/* \Returns the IPv6 net mask of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sIPNetMask :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnIPNetMaskBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_GetIP6NetMask, (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPNetMask,
		PRL_UINT32_PTR pnIPNetMaskBufLength
		) );

/* Sets the specified virtual network IPv6 net mask.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sIPNetMask :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_SetIP6NetMask, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPNetMask
		) );

/* \Returns the VLAN tag of the specified virtual network. Since
   the same physical adapter can be used at several virtual
   networks pair <VLAN tag, MAC address> can identify adapter.
   Parameters
   hVirtNet :            A handle of type PHT_VIRTUAL_NETWORK
                         identifying the virtual network.
   pnVlanTag   :         [out] The buffer for storing result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetVlanTag, (
		PRL_HANDLE hVirtNet,
		PRL_UINT16_PTR pnVlanTag
		) );

/* \Sets the VLAN tag for the specified virtual network. Since
   the same physical adapter can be used at several virtual
   networks pair <VLAN tag, MAC address> can identify adapter.
   Parameters
   hVirtNet :            A handle of type PHT_VIRTUAL_NETWORK
                         identifying the virtual network.
   nVlanTag   :          [in] Setting value.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetVlanTag, (
		PRL_HANDLE hVirtNet,
		PRL_UINT16 nVlanTag
		) );

/* \Returns the DHCP start IP address of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sIPScopeStart :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnIPScopeStartBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetIPScopeStart, (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPScopeStart,
		PRL_UINT32_PTR pnIPScopeStartBufLength
		) );

/* Sets the specified virtual network DHCP start IP address.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sIPScopeStart :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetIPScopeStart, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPScopeStart
		) );

/* \Returns the DHCP end IP address of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sIPScopeEnd :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnIPScopeEndBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetIPScopeEnd, (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPScopeEnd,
		PRL_UINT32_PTR pnIPScopeEndBufLength
		) );

/* Sets the specified virtual network DHCP end IP address.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sIPScopeEnd :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetIPScopeEnd, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPScopeEnd
		) );

/* \Returns the DHCP start IPv6 address of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sIPScopeStart :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnIPScopeStartBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_GetIP6ScopeStart, (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPScopeStart,
		PRL_UINT32_PTR pnIPScopeStartBufLength
		) );

/* Sets the specified virtual network DHCP start IPv6 address.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sIPScopeStart :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_SetIP6ScopeStart, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPScopeStart
		) );

/* \Returns the DHCP end IPv6 address of the specified virtual network.
   Parameters
   hVirtNet :             A handle of type PHT_VIRTUAL_NETWORK
                          identifying the virtual network.
   sIPScopeEnd :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnIPScopeEndBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_GetIP6ScopeEnd, (
		PRL_HANDLE hVirtNet,
		PRL_STR sIPScopeEnd,
		PRL_UINT32_PTR pnIPScopeEndBufLength
		) );

/* Sets the specified virtual network DHCP end IPv6 address.
   Parameters
   hVirtNet :     A handle of type PHT_VIRTUAL_NETWORK
                  identifying the virtual network.
   sIPScopeEnd :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_SetIP6ScopeEnd, (
		PRL_HANDLE hVirtNet,
		PRL_CONST_STR sIPScopeEnd
		) );

/* Determines whether the specified virtual network is
   enabled or disabled.
   Parameters
   hVirtNet :   A handle of type PHT_VIRTUAL_NETWORK identifying
                the virtual network.
   pbEnabled :  [out] A pointer to a variable that receives the \result.
                PRL_TRUE indicates that the virtual network is enabled.
                PRL_FALSE indicates that the virtual network is disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_IsEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL_PTR pbEnabled
		) );

/* Allows to enable or disable a specified virtual network.
   Parameters
   hVirtNet :  A handle of type PHT_VIRTUAL_NETWORK identifying
               the virtual network.
   bEnabled :  A boolean value indicating the type of action to
               perform. PRL_TRUE will enable the virtual network.
               PRL_FALSE will disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL bEnabled
		) );

/* Determines whether the specified virtual network adapter is
   enabled or disabled.
   Parameters
   hVirtNet :   A handle of type PHT_VIRTUAL_NETWORK identifying
                the virtual network.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the adapter is
                enabled. PRL_FALSE indicates that the adapter is
                disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_IsAdapterEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL_PTR pbEnabled
		) );

/* Allows to enable or disable a specified virtual network
   adapter.
   Parameters
   hVirtNet :  A handle of type PHT_VIRTUAL_NETWORK identifying
               the virtual network.
   bEnabled :  A boolean value indicating the type of action to
               perform. PRL_TRUE will enable the adapter.
               PRL_FALSE will disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetAdapterEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL bEnabled
		) );

/* Determines whether the specified virtual network DHCP server is
   enabled or disabled.
   Parameters
   hVirtNet :   A handle of type PHT_VIRTUAL_NETWORK identifying
                the virtual network.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the DHCP server is
                enabled. PRL_FALSE indicates that the DHCP server is
                disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_IsDHCPServerEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL_PTR pbEnabled
		) );

/* Allows to enable or disable a specified virtual network
   DHCP server.
   Parameters
   hVirtNet :  A handle of type PHT_VIRTUAL_NETWORK identifying
               the virual network.
   bEnabled :  A boolean value indicating the type of action to
               perform. PRL_TRUE will enable the DHCP server.
               PRL_FALSE will disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetDHCPServerEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL bEnabled
		) );

/* Determines whether the specified virtual network DHCPv6 server is
   enabled or disabled.
   Parameters
   hVirtNet :   A handle of type PHT_VIRTUAL_NETWORK identifying
                the virtual network.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the DHCP server is
                enabled. PRL_FALSE indicates that the DHCP server is
                disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_IsDHCP6ServerEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL_PTR pbEnabled
		) );

/* Allows to enable or disable a specified virtual network
   DHCPv6 server.
   Parameters
   hVirtNet :  A handle of type PHT_VIRTUAL_NETWORK identifying
               the virual network.
   bEnabled :  A boolean value indicating the type of action to
               perform. PRL_TRUE will enable the DHCP server.
               PRL_FALSE will disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlVirtNet_SetDHCP6ServerEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL bEnabled
		) );

/* Determines whether the specified virtual network NAT server is
   enabled or disabled.
   Parameters
   hVirtNet :   A handle of type PHT_VIRTUAL_NETWORK identifying
                the virtual network.
   pbEnabled :  [out] A pointer to a variable that receives the
                \result. PRL_TRUE indicates that the NAT server is
                enabled. PRL_FALSE indicates that the NAT server is
                disabled.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_IsNATServerEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL_PTR pbEnabled
		) );

/* Allows to enable or disable a specified virtual network
   NAT server.
   Parameters
   hVirtNet :  A handle of type PHT_VIRTUAL_NETWORK identifying
               the virual network.
   bEnabled :  A boolean value indicating the type of action to
               perform. PRL_TRUE will enable the NAT server.
               PRL_FALSE will disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetNATServerEnabled, (
		PRL_HANDLE hVirtNet,
		PRL_BOOL bEnabled
		) );

/* \Returns the port forward entries list.
   Parameters
   hVirtNet :        A handle of type PHT_VIRTUAL_NETWORK
                     identifying the virtual network.
   nPortFwdType :    A port forward type ( PRL_PORT_FORWARDING_TYPE
                     enumeration ).
   phPortFwdList :   [out] A pointer to a variable that receives the
                     new handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the list
   or a port forward handle object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetPortForwardList, (
		PRL_HANDLE hVirtNet,
		PRL_PORT_FORWARDING_TYPE nPortFwdType,
		PRL_HANDLE_PTR phPortFwdList
		) );

/* \Sets the port forward entries list.
   Parameters
   hVirtNet :        A handle of type PHT_VIRTUAL_NETWORK
                     identifying the virtual network.
   nPortFwdType :    A port forward type ( PRL_PORT_FORWARDING_TYPE
                     enumeration ).
   hPortFwdList :    The list of PHT_PORT_FORWARDING handles.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a port
   forward entry object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_SetPortForwardList, (
		PRL_HANDLE hVirtNet,
		PRL_PORT_FORWARDING_TYPE nPortFwdType,
		PRL_HANDLE hPortFwdList
		) );

/* \Obtains physical adapter info which bound to the virtual network object.
   This method provided for convenience in order do not create own algorithm
   of bound to the virtual network physical adapter information.
   Parameters
   hVirtNet :        A handle of type PHT_VIRTUAL_NETWORK
                     identifying the virtual network.
   hSrvConfig :	     A handle of type PHT_SERVER_CONFIG which provides
                     host hardware information.
   phNetAdapter :   [out] A pointer to a variable that receives the
                     new handle of PHT_HW_NET_ADAPTER object.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of virtual network object or invalid
                         handle of host hardware info or null pointer was passed.

   PRL_ERR_NETWORK_ADAPTER_NOT_FOUND - correspond network adapter is absent at
                                       provided host

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the object of network
                           adapter.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlVirtNet_GetBoundAdapterInfo, (
		PRL_HANDLE hVirtNet,
		PRL_HANDLE hSrvConfig,
		PRL_HANDLE_PTR phNetAdapter
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Port Forwarding operations
///////////////////////////////////////////////////////////////////////////////

/* %VM_ONLY%

   Creates a new handle of type PHT_PORT_FORWARDING. It is the
   responsibility of the user to free the handle when it is no
   longer needed.
   Parameters
   phPortFwd :  [out] A pointer to a variable that receives the new
                handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a port
   forward handle object.

   PRL_ERR_SUCCESS - function completed successfully.               */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlPortFwd_Create, (
		PRL_HANDLE_PTR phPortFwd
		) );

/* \Returns the rule name of the specified port forward entry.
   Parameters
   hPortFwd :            A handle of type PHT_PORT_FORWARDING
                         identifying the port forwarding.
   sRuleName :  [out] A pointer to a buffer that receives
                         the result (a UTF\-8 encoded,
                         null\-terminated string).
   pnRuleNameBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlPortFwd_GetRuleName, (
		PRL_HANDLE hPortFwd,
		PRL_STR sRuleName,
		PRL_UINT32_PTR pnRuleNameBufLength
		) );

/* Sets the specified port forward entry rule name.
   Parameters
   hPortFwd :    A handle of type PHT_PORT_FORWARDING
                 identifying the port forwarding.
   sRuleName : The value to set. The value must be a a UTF\-8
                 encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlPortFwd_SetRuleName, (
		PRL_HANDLE hPortFwd,
		PRL_CONST_STR sRuleName
		) );
  
/* %VM_ONLY%

   Returns the incoming port.
   Parameters
   hPortFwd :        A handle of type PHT_PORT_FORWARDING
                     identifying the port forwarding.
   pnIncomingPort :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlPortFwd_GetIncomingPort, (
		PRL_HANDLE hPortFwd,
		PRL_UINT16_PTR pnIncomingPort
		) );

/* %VM_ONLY%

   Sets the specified incoming port.
   Parameters
   hPortFwd :      A handle of type PHT_PORT_FORWARDING
                   identifying the port forwarding.
   nIncomingPort :  The value to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlPortFwd_SetIncomingPort, (
		PRL_HANDLE hPortFwd,
		PRL_UINT16 nIncomingPort
		) );

/* %VM_ONLY%

   Returns the redirect IP address of the specified port forward entry.
   Parameters
   hPortFwd :            A handle of type PHT_PORT_FORWARDING
                         identifying the port forwarding.
   sRedirectIPAddress :  [out] A pointer to a buffer that receives
                         the result (a UTF\-8 encoded,
                         null\-terminated string).
   pnRedirectIPAddressBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlPortFwd_GetRedirectIPAddress, (
		PRL_HANDLE hPortFwd,
		PRL_STR sRedirectIPAddress,
		PRL_UINT32_PTR pnRedirectIPAddressBufLength
		) );

/* %VM_ONLY%

   Sets the specified port forwardi entry redirect IP address.
   Parameters
   hPortFwd :           A handle of type PHT_PORT_FORWARDING
                        identifying the port forwarding.
   sRedirectIPAddress : The value to set. The value must be a a UTF\-8
                        encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlPortFwd_SetRedirectIPAddress, (
		PRL_HANDLE hPortFwd,
		PRL_CONST_STR sRedirectIPAddress
		) );

/* %VM_ONLY%

   Returns the redirect port.
   Parameters
   hPortFwd :        A handle of type PHT_PORT_FORWARDING
                     identifying the port forwarding.
   pnRedirectPort :  [out] A pointer to a variable that receives
                     the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlPortFwd_GetRedirectPort, (
		PRL_HANDLE hPortFwd,
		PRL_UINT16_PTR pnRedirectPort
		) );

/* %VM_ONLY%

   Sets the specified redirect port.
   Parameters
   hPortFwd :      A handle of type PHT_PORT_FORWARDING
                   identifying the port forwarding.
   nRedirectPort :  The value to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_2,
				 PrlPortFwd_SetRedirectPort, (
		PRL_HANDLE hPortFwd,
		PRL_UINT16 nRedirectPort
		) );

/* \Returns the redirect virtual machine identifier
   of the specified port forward entry.
   Parameters
   hPortFwd :            A handle of type PHT_PORT_FORWARDING
                         identifying the port forwarding.
   sRedirectVm :  [out] A pointer to a buffer that receives
                         the result (a UTF\-8 encoded,
                         null\-terminated string).
   pnRedirectVmBufLength : [in] The size of the output buffer (in
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
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlPortFwd_GetRedirectVm, (
		PRL_HANDLE hPortFwd,
		PRL_STR sRedirectVm,
		PRL_UINT32_PTR pnRedirectVmBufLength
		) );

/* Sets the specified port forward entry virtual machine identifier.
   Parameters
   hPortFwd :    A handle of type PHT_PORT_FORWARDING
                 identifying the port forwarding.
   sRedirectVm : The value to set. The value must be a a UTF\-8
                 encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlPortFwd_SetRedirectVm, (
		PRL_HANDLE hPortFwd,
		PRL_CONST_STR sRedirectVm
		) );

///////////////////////////////////////////////////////////////////////////////
/// @section Functions specific for the Net service processing
///////////////////////////////////////////////////////////////////////////////

/* \Returns the Dispatcher Service network status information.
   Parameters
   handle :       A handle of type PHT_NET_SERVICE_STATUS.
   pnNetStatus :  [out] A pointer to a variable that receives the
                  status information.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlNetSvc_GetStatus, (
		PRL_HANDLE handle,
		PRL_SERVICE_STATUS_ENUM_PTR pnNetStatus
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Offline Services operations
///////////////////////////////////////////////////////////////////////////////

/* Creates a new handle of type PHT_OFFLINE_SRVICE. The handle
   is necessary to configure offline services. If
   you would like to manage offline service configuration, use this
   function to create the handle first and then use the handle
   functions to configure the offline services.

   It is the responsibility of the caller to free the handle
   with the PrlHandle_Free call when it is no longer needed.
   Parameters
   phOffmgmtService :  [out] A pointer to a variable that receives the
                       handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a virtual
   network object.

   PRL_ERR_SUCCESS - function completed successfully.            */

PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlOffmgmtService_Create, (
		PRL_HANDLE_PTR phOffmgmtService
		) );

/* \Returns the name of the specified offline service.
   Parameters
   hOffmgmtService:       A handle of type PHT_OFFLINE_SERVICE
                          identifying the offline service.
   sname      :           [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnNameBufLength:       [in] The size of the output buffer (in
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
						PrlOffmgmtService_GetName, (
		PRL_HANDLE hOffmgmtService,
		PRL_STR sName,
		PRL_UINT32_PTR pnNameBufLength
		) );

/* Sets the specified offline service name.
   Parameters
   hOffmgmtService :   A handle of type PHT_OFFLINE_SERVICE
                  identifying the offline service.
   sName:         The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlOffmgmtService_SetName, (
		PRL_HANDLE hOffmgmtService,
		PRL_CONST_STR sName
		) );

/* \Returns the port of the specified offline service.
   Parameters
   hOffmgmtService:       A handle of type PHT_OFFLINE_SERVICE
                          identifying the offline service.
   sname      :           [out] A pointer to a buffer that receives
                          the result
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlOffmgmtService_GetPort, (
		PRL_HANDLE hOffmgmtService,
		PRL_UINT32_PTR pnPort
		) );
/* Sets the specified offline service port.
   Parameters
   hOffmgmtService :   A handle of type PHT_OFFLINE_SERVICE
                  identifying the offline service.
   nPort:         The value to set.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlOffmgmtService_SetPort, (
		PRL_HANDLE hOffmgmtService,
		PRL_UINT32 nPort
		) );

/* \Returns the type of the flag of the specified offline service used by default.
   Parameters
   hOffmgmtService:       A handle of type PHT_OFFLINE_SERVICE
                          identifying the offline service.
   sname      :           [out] A pointer to a buffer that receives
                          the result
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlOffmgmtService_IsUsedByDefault, (
		PRL_HANDLE hOffmgmtService,
		PRL_BOOL_PTR pbDefault
		) );

/* Sets the default flag type for the specified offline service.
   Parameters
   hOffmgmtService :   A handle of type PHT_OFFLINE_SERVICE
                      identifying the offline service.
   bDefault:          The value to set.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */


PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlOffmgmtService_SetUsedByDefault, (
		PRL_HANDLE hOffmgmtService,
		PRL_BOOL bDefault
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Traffic Accounting/Shaping operations
///////////////////////////////////////////////////////////////////////////////

/* Creates a new handle of type PHT_NETWORK_CLASS. The handle
   is necessary to configure network accounting configuration
   If you would like to manage network classes configuration,
   use this function to create the handle first and then use the
   handle functions to configure the Network class.

   It is the responsibility of the caller to free the handle
   with the PrlHandle_Free call when it is no longer needed.
   Parameters
   nClassId:		Network class id.
   phNetworkClass :	[out] A pointer to a variable that receives the
                        handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a virtual
   network object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkClass_Create, (
		PRL_UINT32 nClassId,
		PRL_HANDLE_PTR phNetworkClass
		) );

/* \Returns the network class id.
   Parameters
   hNetworkClass:	A handle of type PHT_NETWORK_CLASS
			identifying the network class entry.
   pnClassId:		[out] A pointer to a buffer that receives
                        the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkClass_GetClassId, (
		PRL_HANDLE hNetworkClass,
		PRL_UINT32_PTR pnClassId
		) );

/* \Sets the network class id.
   Parameters
   hNetworkClass:	A handle of type PHT_NETWORK_CLASS
			identifying the network class entry.
   nClassId:		Class id
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkClass_SetClassId, (
		PRL_HANDLE hNetworkClass,
		PRL_UINT32 nClassId
		) );


/* \Returns the list of networks for specified network class entry.
   Parameters
   hNetworkClass:	A handle of type PHT_NETWORK_CLASS
			identifying the network class entry.
   phNetworkList:	[out] A pointer to a handle of type PHT_STRINGS_LIST
			that receives the list of networks
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a virtual
   network object.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkClass_GetNetworkList, (
		PRL_HANDLE hNetworkClass,
		PRL_HANDLE_PTR phNetworkList
		) );

/* \Sets the list of networks for specified network class entry.
   Parameters
   hNetworkClass:	A handle of type PHT_NETWORK_CLASS
			identifying the network class entry.
   hNetworkList:	A handle of type PHT_STRINGS_LIST
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a virtual
   network object.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkClass_SetNetworkList, (
		PRL_HANDLE hNetworkClass,
		PRL_HANDLE hNetworkList
		) );


/* Creates a new handle of type PHT_NETWORK_SHAPING. The handle
   is necessary to configure network shaping configuration.

   It is the responsibility of the caller to free the handle
   with the PrlHandle_Free call when it is no longer needed.
   Parameters
   nClassId:		Network class id
   nTotalRate:		Specifies bound rate for network class
   phNetworkShapingEntry:[out] A pointer to a variable that receives the
                        handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a virtual
   network object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkShapingEntry_Create, (
		PRL_UINT32 nClassId,
		PRL_UINT32 nTotalRate,
		PRL_HANDLE_PTR phNetworkShapingEntry
		) );

/* \Returns the network class id.
   Parameters
   pNetworkShapingEntry:A handle of type PHT_NETWORK_SHAPING
			identifying the network shaping entry.
   pnClassId:		[out] A pointer to a buffer that receives
			the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkShapingEntry_GetClassId, (

		PRL_HANDLE pNetworkShapingEntry,
		PRL_UINT32_PTR pnClassId
		) );

/* \Sets the network class id for specified shaping entry.
   Parameters
   pNetworkShapingEntry:A handle of type PHT_NETWORK_SHAPING
			identifying the network shaping entry.
   nClassId:		[out] A pointer to a buffer that receives
			the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkShapingEntry_SetClassId, (

		PRL_HANDLE pNetworkShapingEntry,
		PRL_UINT32 nClassId
		) );


/* \Returns the total rate for specified shaping entry.
   Parameters
   pNetworkShapingEntry:A handle of type PHT_NETWORK_SHAPING
			identifying the network shaping entry.
   pnTotalRate:		[out] A pointer to a buffer that receives
			the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkShapingEntry_GetTotalRate, (

		PRL_HANDLE pNetworkShapingEntry,
		PRL_UINT32_PTR pnTotalRate
		) );

/* Sets the total rate for specified shaping entry.
   Parameters
   hNetworkShapingEntry: A handle of type PHT_NETWORK_SHAPING
			identifying the shaping entry.
   nTotalRate:		The network total rate in Kbits.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkShapingEntry_SetTotalRate, (

		PRL_HANDLE pNetworkShapingEntry,
		PRL_UINT32 nTotalRate
		) );


/* Sets the specified network device.
   Parameters
   hNetworkShapingEntry: A handle of type PHT_NETWORK_SHAPING
			identifying the shaping entry.
   sDev:		The network device.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkShapingEntry_SetDevice, (

		PRL_HANDLE hNetworkShapingEntry,
		PRL_CONST_STR sDev
		) );

/* \Returns the network device for specified shaping entry.
   Parameters
   pNetworkShapingEntry:A handle of type PHT_NETWORK_SHAPING
			identifying the network shaping entry.
   sDev:		[out] A pointer to a buffer that receives
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
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkShapingEntry_GetDevice, (

		PRL_HANDLE hNetworkShapingEntry,
		PRL_STR sDev,
		PRL_UINT32_PTR pnBufLength
		) );

/* Sets the default network guaranteed rate.
   Parameters
   hNetworkShapingEntry: A handle of type PHT_NERWORK_SHAPING
			identifying the shaping entry.
   sRate:		The network rate in Kbits.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkShapingEntry_SetRate, (

		PRL_HANDLE pNetworkShapingEntry,
		PRL_UINT32 nRate
		) );

/* \Returns the default rate for specified shaping entry.
   Parameters
   pNetworkShapingEntry:A handle of type PHT_NETWORK_SHAPING
			identifying the network shaping entry.
   pnRate:		[out] A pointer to a buffer that receives
			the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkShapingEntry_GetRate, (

		PRL_HANDLE pNetworkShapingEntry,
		PRL_UINT32_PTR pnRate
		) );

/* Allows to enable and disable packet rate limiting by setting MPU.
   Parameters
   hNetworkShapingEntry: A handle of type PHT_NERWORK_SHAPING
			identifying the shaping entry.
   nRateMPU:	An integer value indicating packet MPU. Packets with size
			lower than MPU will be threated as those of size MPU.
			May be a number lower than MTU or one of the special values:
			NRM_DISABLED or NRM_ENABLED (for default value).

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_7,
						PrlNetworkShapingEntry_SetRateMPU, (

		PRL_HANDLE pNetworkShapingEntry,
		PRL_INT32 nRateMPU
		) );

/* Returns MPU size for packet rate limitation.
   Parameters
   pNetworkShapingEntry: A handle of type PHT_NETWORK_SHAPING
			identifying the network shaping entry.
   pnRateMPU:	[out] A pointer to a variable that receives the
			\return value. May be a number lower than MPU or
			one of the special values:
			NRM_DISABLED or NRM_ENABLED (for default value).

   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_7,
						PrlNetworkShapingEntry_GetRateMPU, (

		PRL_HANDLE pNetworkShapingEntry,
		PRL_INT32_PTR pnRateMPU
		) );

/* Determines whether the network shaping is enabled or disabled.
   Parameters
   hConfig :      A valid handle of type PHT_NETWORK_SHAPING_CONFIG
                  identifying the adapter.
   pbEnabled :    [out] A pointer to a variable that receives the
                  \return value. PRL_TRUE indicates that the
                  network shaping is enabled. PRL_FALSE indicates
                  otherwise.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlNetworkShapingConfig_IsEnabled, (
		PRL_HANDLE hConfig,
		PRL_BOOL_PTR pbEnabled
		) );

/* Allows to enable or disable the network shaping.
   Parameters
   hConfig  :  A handle of type PHT_NETWORK_SHAPING_CONFIG.
   bEnabled :  A boolean value indicating the type of action to
               perform. PRL_TRUE will enable the network shaping.
               PRL_FALSE will disable it.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlNetworkShapingConfig_SetEnabled, (
		PRL_HANDLE hConfig,
		PRL_BOOL bEnabled
		) );

/* Obtains a list of network shaping entries
   represents the network shaping configuration.
   Parameters
   hConfig :  A valid handle of type PHT_NETWORK_SHAPING_CONFIG
              identifying the adapter.
   hList:     A pointer to a handle of type PHT_HANDLE_LIST which
              contains the list of handles of type PHT_NETWORK_SHAPING
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlNetworkShapingConfig_GetNetworkShapingList, (
		PRL_HANDLE hConfig,
		PRL_HANDLE_PTR phList
		) );

/* Update the list of network shaping entries represents the
   network shaping  configuration.
   Parameters
   hConfig :  A valid handle of type PHT_NETWORK_SHAPING_CONFIG
              identifying the adapter.
   hList:     A handle of type PHT_HANDLE_LIST contains the
              list of handles of type PHT_NETWORK_SHAPING
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlNetworkShapingConfig_SetNetworkShapingList, (
		PRL_HANDLE hConfig,
		PRL_HANDLE hList
		) );


/* Obtains a list of network shaping bandwidth entries
   represents the network shaping configuration.
   Parameters
   hConfig :  A valid handle of type PHT_NETWORK_SHAPING_CONFIG
              identifying the adapter.
   hList:     A pointer to a handle of type PHT_HANDLE_LIST which
              contains the list of handles of type PHT_NETWORK_SHAPING
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlNetworkShapingConfig_GetNetworkDeviceBandwidthList, (
		PRL_HANDLE hConfig,
		PRL_HANDLE_PTR phList
		) );

/* Update the list of network device bandwidth represents the
   network shaping configuration.
   Parameters
   hConfig :  A valid handle of type PHT_NETWORK_SHAPING_CONFIG
              identifying the adapter.
   hList:     A handle of type PHT_HANDLE_LIST contains the
              list of handles of type PHT_NETWORK_SHAPING
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlNetworkShapingConfig_SetNetworkDeviceBandwidthList, (
		PRL_HANDLE hConfig,
		PRL_HANDLE hList
		) );


/* Creates a new handle of type PHT_NETWORK_SHAPING_BANDWIDTH. The handle
   is necessary to configure network shaping configuration.

   It is the responsibility of the caller to free the handle
   with the PrlHandle_Free call when it is no longer needed.
   Parameters
   sDev:		network device name.
   nBandwidth:		Network device bandwidth
   phDeviceEntryN:[out] A pointer to a variable that receives the
                        handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a virtual
   network object.

   PRL_ERR_SUCCESS - function completed successfully.            */

PRL_METHOD_DECL(PARALLELS_API_VER_3,
			PrlNetworkShapingBandwidthEntry_Create, (
		PRL_CONST_STR sDev,
		PRL_UINT32 nBandwidth,
		PRL_HANDLE_PTR phDeviceEntry
		) );


/* \Sets the network device name for specified shaping entry.
   Parameters
   hDeviceEntry:	A handle of type PHT_NETWORK_SHAPING_BANDWIDTH
			identifying the network shaping entry.
   sDev:		network device name
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL(PARALLELS_API_VER_3,
			PrlNetworkShapingBandwidthEntry_SetDevice, (
		PRL_HANDLE hDeviceEntry,
		PRL_CONST_STR sDev
		) );


/* \Returns the network device name for specified shaping entry.
   Parameters
   hDeviceEntry:	A handle of type PHT_NETWORK_SHAPING_BANDWIDTH
			identifying the network shaping entry.
   sDev:		[out] A pointer to a buffer that receives
			the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL(PARALLELS_API_VER_3,
			PrlNetworkShapingBandwidthEntry_GetDevice, (
		PRL_HANDLE hDeviceEntry,
		PRL_STR sDev,
		PRL_UINT32_PTR pnBufLength
		) );


/* \Sets the network device bandwidth for specified shaping entry.
   Parameters
   hDeviceEntry:	A handle of type PHT_NETWORK_SHAPING_BANDWIDTH
			identifying the network shaping entry.
   nBandwidth:		Network device bandwidth
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */

PRL_METHOD_DECL(PARALLELS_API_VER_3,
			PrlNetworkShapingBandwidthEntry_SetBandwidth, (
		PRL_HANDLE hDeviceEntry,
		PRL_UINT32 nBandwidth
		) );


/* \Returns the network device bandwidth for specified shaping entry.
   Parameters
   hDeviceEntry:	A handle of type PHT_NETWORK_SHAPING_BANDWIDTH
			identifying the network shaping entry.
   pnBandwidth:		[out] A pointer to a buffer that receives
			the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL(PARALLELS_API_VER_3,
			PrlNetworkShapingBandwidthEntry_GetBandwidth, (
		PRL_HANDLE hDeviceEntry,
		PRL_UINT32_PTR pnBandwidth
		) );


/* Creates a new handle of type PHT_NETWORK_RATE. The handle
   is necessary to configure Vm output bandwidth.

   It is the responsibility of the caller to free the handle
   with the PrlHandle_Free call when it is no longer needed.
   Parameters
   nClassId:		Network class id
   nRate:		Specifies bound rate for network class
   phNetwork:		[out] A pointer to a variable that receives the
                        handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create the object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkRate_Create, (
		PRL_UINT32 nClassId,
		PRL_UINT32 nRate,
		PRL_HANDLE_PTR phNetworkRate
		) );


/* \Returns the network rate entry class id.
   Parameters
   hNetworkRate:	A handle of type PHT_NETWORK_RATE
			identifying the network rate entry.
   pnClassId:		[out] A pointer to a buffer that receives
                        the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkRate_GetClassId, (
		PRL_HANDLE hNetworkRate,
		PRL_UINT32_PTR pnClassId
		) );


/* \Returns the network rate entry Rate value.
   Parameters
   hNetworkRate:	A handle of type PHT_NETWORK_RATE
			identifying the network rate entry.
   pnRate:		[out] A pointer to a buffer that receives
                        the result.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
						PrlNetworkRate_GetRate, (
		PRL_HANDLE hNetworkRate,
		PRL_UINT32_PTR pnRate
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section IP private network operations
///////////////////////////////////////////////////////////////////////////////

/* Creates a new handle of type PHT_IPPRIV_NET. The handle
   is necessary to create and configure an IP private network. If
   you would like to create a new IP private network, use this
   function to create the handle first and then use the handle
   functions to configure the network. If you would like to
   modify the configuration of an existing network, use
   the PrlSrv_GetIPPrivateNetworksList function to obtain a handle
   to the private network of interest.

   It is the responsibility of the caller to free the handle
   with the PrlHandle_Free call when it is no longer needed.
   Parameters
   phPrivNet :  [out] A pointer to a variable that receives the
                handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - null pointer was passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to create a virtual
   network object.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlIPPrivNet_Create, (
		PRL_HANDLE_PTR phPrivNet
		) );

/* \Returns the name of the specified private network.
   Parameters
   hPrivNet :             A handle of type PHT_IPPRIV_NET
                          identifying the private network.
   sName    :             [out] A pointer to a buffer that receives
                          the result (a UTF\-8 encoded,
                          null\-terminated string).
   pnNameBufLength :      [in] The size of the output buffer (in
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
			PrlIPPrivNet_GetName, (
		PRL_HANDLE hPrivNet,
		PRL_STR sName,
		PRL_UINT32_PTR pnNameBufLength
		) );

/* Sets the specified private network ID.
   Parameters
   hPrivNet :     A handle of type PHT_IPPRIV_NET
                  identifying the private network.
   sNetworkId :   The value to set. The value must be a a UTF\-8
                  encoded, null\-terminated string.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlIPPrivNet_SetName, (
		PRL_HANDLE hPrivNet,
		PRL_CONST_STR sName
		) );

/* Allows to set IP addresses/Subnet masks to a private network.
   Parameters
   hPrivNet :           A handle of type PHT_IPPRIV_NET identifying
                        the private network.
   hNetAddressesList :  A handle of type PHT_STRINGS_LIST
                        containing the list of "IP address/Subnet
                        mask" pairs to be applied to the network
                        adapter. Can be PRL_INVALID_HANDLE if
                        it's necessary to specify an empty list.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle of virtual network
   adapter specified.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlIPPrivNet_SetNetAddresses, (
		PRL_HANDLE hPrivNet,
		PRL_HANDLE NetAddresses
		) );

/* Obtains a list of IP addresses in the address/subnet_mask
   format which is assigned to a private network.

   \Returns the list of pairs "IP address/Subnet mask" which
   is applied to the private network. The result will be returned
   as list of strings (PHT_STRINGS_LIST type) of pairs "IP
   address/Subnet mask".
   Parameters
   hPrivNet :            A handle of type PHT_IPPRIV_NET identifying
                         the private network.
   phNetAddressesList :  [out] A pointer to the PHT_STRINGS_LIST
                         handle containing the IP addresses.
   Returns
   PRL_RESULT. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings
   list object.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlIPPrivNet_GetNetAddresses, (
		PRL_HANDLE hPrivNet,
		PRL_HANDLE_PTR pNetAddresses
		) );

/* Determines the mode (global or not) of the specified private network.
   Parameters
   hPrivNet :     A handle of type PHT_IPPRIV_NET
                  identifying the private network.
   pbGlobal :     [out] A pointer to a variable that receives the result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlIPPrivNet_IsGlobal, (
		PRL_HANDLE hPrivNet,
		PRL_BOOL_PTR pbGlobal
		) );

/* Sets the mode (global or not) of the specified private network.
   Parameters
   hPrivNet :     A handle of type PHT_IPPRIV_NET
                  identifying the private network.
   bGlobal :      The value to set.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.             */
PRL_METHOD_DECL( PARALLELS_API_VER_3,
			PrlIPPrivNet_SetGlobal, (
		PRL_HANDLE hPrivNet,
		PRL_BOOL bGlobal
		) );


#ifdef _WIN_
    #pragma pack(pop, save_api_pack)
#endif

#ifdef __cplusplus
}
#endif

#endif // __PARALLELS_API_NET_H__
