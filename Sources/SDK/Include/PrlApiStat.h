/*
 * PrlApiStat.h
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


#ifndef __PARALLELS_API_STATISTIC_H__
#define __PARALLELS_API_STATISTIC_H__

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


///////////////////////////////////////////////////////////////////////////////
/// @section Server object handling routines and data types.
///////////////////////////////////////////////////////////////////////////////

/* Obtains a handle of type PHT_SYSTEM_STATISTICS containing the
   host resource usage statistics.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_SYSTEM_STATISTICS.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_GetStatistics, (
		PRL_HANDLE hServer
		) );


/* Allows to subscribe to receive host OS statistics on a
   periodic basis. To obtain statistics, an event handler
   (callback function) is required. Within the event handler,
   first check the type of event. Events of type
   PET_DSP_EVT_HOST_STATISTICS_UPDATED indicate an event
   containing statistics data. To access the statistics handle
   (a handle of type PHT_SYSTEM_STATISTICS), first extract the
   event parameter using PrlEvent_GetParam, then convert the
   \result (which will be a handle to an object of type
   PHT_EVENT_PARAMETER) to a handle using PrlEvtPrm_ToHandle.
   The functions that operate on PHT_SYSTEM_STATISTICS
   references can then be used to obtain statistics data.

   For the event handler to be called, it is necessary to
   register it with PrlSrv_RegEventHandler. When statistics data
   is no longer required, unsubscribe from statistics events
   using PrlSrv_UnsubscribeFromHostStatistics. When events are
   no longer required, unregister the event handler using
   PrlSrv_UnregEventHandler.

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
						   PrlSrv_SubscribeToHostStatistics, (
		PRL_HANDLE hServer
		) );

/* Cancels the Parallels Service statistics subscription that was started
   using the PrlSrv_SubscribeToHostStatistics function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_SERVER identifying the Parallels
             Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                    */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_UnsubscribeFromHostStatistics, (
		PRL_HANDLE hServer
		) );

/* Allows to subscribe to periodically receive perfomance
   statistics. To obtain statistics, an event handler (callback
   function) is required. Within the event handler, first check
   the type of event. Events of type
   PET_DSP_EVT_PERFSTATS_UPDATED indicate an event containing
   statistics data. The perfomace statistics repesented by
   \parameters of Event object (a handle of type PHT_EVENT).

   For the event handler to be called, it is necessary to
   register it with PrlSrv_RegEventHandler. When statistics data
   is no longer required, unsubscribe from statistics events
   using PrlSrv_UnsubscribeFromHostPerfStats. When events are no
   longer required, unregister the event handler using
   PrlSrv_UnregEventHandler.

   If method is called more than once, the 'filter' value is
   updated each time on the Parallels Service side.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Parallels Service.
   sFilter :  String representing statistics filter, use char '*'
              at the end to get subset of statistic. Could be
              NULL, to get all statistic.
			  See PrlPerfCounters.h header file in order to determine
			  published performance counters patterns list.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                   */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_SubscribeToPerfStats, (
                           PRL_HANDLE hServer, PRL_CONST_STR sFilter
                           ) );

/* Cancels the Parallels Service perfomace statistics subscription
   that was started using the PrlSrv_SubscribeToPerfStats function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_SERVER identifying the Parallels
             Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                    */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_UnsubscribeFromPerfStats, (
		PRL_HANDLE hServer
		) );

/* Obtains a handle of type PHT_EVENT containing the
   perfomance statistics.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.

   To get the result from the PHT_JOB object:
     1. Use the PrlJob_GetResult function to obtain a handle to
        the PHT_RESULT object.
     2. Use the PrlResult_GetParam function to obtain a handle
        of type PHT_EVENT.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the Parallels
              Service.
   sFilter :  String representing statistics filter,
              use char '*' at the end to get subset of statistic.
              Could be NULL, to get all statistic.
			  See PrlPerfCounters.h header file in order to determine
			  published performance counters patterns list.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_2,
						   PrlSrv_GetPerfStats, (
                           PRL_HANDLE hServer, PRL_CONST_STR sFilter
                           ) );


///////////////////////////////////////////////////////////////////////////////
/// @section System statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

/* \Returns total RAM size in bytes.
   Parameters
   hStat :   A handle of type PHT_SYSTEM_STATISTICS.
   pnSize :  [out] A pointer to a variable that receives the
             \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetTotalRamSize, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns the size RAM currently in use (in bytes).
   Parameters
   hStat :   A handle of type PHT_SYSTEM_STATISTICS.
   pnSize :  [out] A pointer to a variable that receives the
             \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.        */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetUsageRamSize, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns free RAM size in bytes.
   Parameters
   hStat :   A handle of type PHT_SYSTEM_STATISTICS.
   pnSize :  [out] A pointer to a variable that receives the result
             (free RAM size in bytes).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.               */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetFreeRamSize, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns the amount of physical memory, in bytes, currently
   occupied by the virtual machine.
   Parameters
   hStat :   A handle of type PHT_SYSTEM_STATISTICS.
   pnSize :  [out] A pointer to a variable that receives the result.

   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.               */
PRL_METHOD_DECL( PARALLELS_API_VER_5,
				 PrlStat_GetRealRamSize, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetFreeSwapSize, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnSize
		) );

/* \Returns the total virtual machine usage time in seconds. The
   virtual machine usage counter is not reset when the virtual
   machine is stopped and restarted, so it's value reflects the
   total uptime since the virtual machine was first started.
   Parameters
   hStat :     A handle of type PHT_SYSTEM_STATISTICS.
   pnUptime :  [out] A pointer to a variable that receives the
               \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully. */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetOsUptime, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnUptime
		) );

/* \Returns the Parallels Service uptime in seconds.
   Parameters
   hStat :     A handle of type PHT_SYSTEM_STATISTICS.
   pnUptime :  [out] A pointer to a variable that receives the
               \result (Parallels Service uptime in seconds).
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetDispUptime, (
		PRL_HANDLE hStat,
		PRL_UINT64_PTR pnUptime
		) );

/* Determines the number of handles of type
   PHT_SYSTEM_STATISTICS_CPU contained in the specified
   PHT_SYSTEM_STATISTICS object. Each available CPU in the
   system will have its own PHT_SYSTEM_STATISTICS_CPU object
   instance.
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
   PrlStat_GetCpuStat                                         */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetCpusStatsCount, (
		PRL_HANDLE hStat,
		PRL_UINT32_PTR pnCount
		) );

/* Obtains a handle of type PHT_SYSTEM_STATISTICS_CPU containing
   the specified CPU statistics.
   Parameters
   hStat :      A handle of type PHT_SYSTEM_STATISTICS.
   nIndex :     A numeric index identifying the CPU statistics
                object instance (beginning with 0). To get the
                number of instances, use the
                PrlStat_GetCpusStatsCount function.
   phCpuStat :  [out] A pointer to a variable that receives the
                handle.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetCpuStat, (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phCpuStat
		) );

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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetIfaceStat, (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phIfaceStat
		) );

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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetUserStat, (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phUserStat
		) );

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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetDiskStat, (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phDiskStat
		) );

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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStat_GetProcStat, (
		PRL_HANDLE hStat,
		PRL_UINT32 nIndex,
		PRL_HANDLE_PTR phProcStat
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section CPU statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

/* \Returns the CPU usage, in percents.
   Parameters
   hCpuStat :    A handle of type PHT_SYSTEM_STATISTICS_CPU
                 identifying the CPU.
   pnPercents :  [out] A pointer to a variable that receives the
                 \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.            */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatCpu_GetCpuUsage, (
		PRL_HANDLE hCpuStat,
		PRL_UINT32_PTR pnPercents
		) );

/* \Returns the CPU total time in seconds.
   Parameters
   hCpuStat :  A handle of type PHT_SYSTEM_STATISTICS_CPU
               identifying the CPU.
   pnTime :    [out] A pointer to a variable that receives the
               \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatCpu_GetTotalTime, (
		PRL_HANDLE hCpuStat,
		PRL_UINT64_PTR pnTime
		) );

/* \Returns the CPU user time in seconds.
   Parameters
   hCpuStat :  A handle of type PHT_SYSTEM_STATISTICS_CPU
               identifying the CPU.
   pnTime :    [out] A pointer to a variable that receives the
               \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatCpu_GetUserTime, (
		PRL_HANDLE hCpuStat,
		PRL_UINT64_PTR pnTime
		) );

/* \Returns the CPU system time, in seconds.
   Parameters
   hCpuStat :  A handle of type PHT_SYSTEM_STATISTICS_CPU
               identifying the CPU.
   pnTime :    [out] A pointer to a variable that receives the
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.          */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatCpu_GetSystemTime, (
		PRL_HANDLE hCpuStat,
		PRL_UINT64_PTR pnTime
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Net iface statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatIface_GetSystemName, (
		PRL_HANDLE hIfaceStat,
		PRL_STR	sSystemName,
		PRL_UINT32_PTR pnSystemNameBufLength
		) );

/* \Returns the total number of bytes the network interface has
   received since the Parallels Service was last started.
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatIface_GetInDataSize, (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnDataSize
		) );

/* \Returns the total number of bytes the network interface has
   sent since the Parallels Service was last started.
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatIface_GetOutDataSize, (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnDataSize
		) );

/* \Returns the total number of packets the network interface
   has received since the Parallels Service was last started.
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatIface_GetInPkgsCount, (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnPkgsCount
		) );

/* \Returns the total number of packets the network interface
   has sent since the Parallels Service was last started.
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatIface_GetOutPkgsCount, (
		PRL_HANDLE hIfaceStat,
		PRL_UINT64_PTR pnPkgsCount
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section User session statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatUser_GetSessionTime, (
		PRL_HANDLE hUserStat,
		PRL_UINT64_PTR pnTime
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Disk statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
   hDiskStat :  A handle of type PHT_SYSTEM_STATISTICS_DISK
                identifying the disk.
   pnCount :    [out] A pointer to a variable that receives the
                \result.
   Returns
   PRL_RESULT. Possible values:

   PRL_ERR_INVALID_ARG - invalid handle or null pointer was
   passed.

   PRL_ERR_SUCCESS - function completed successfully.
   See Also
   PrlStatDisk_GetPartStat                                      */
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatDiskPart_GetFreeDiskSpace, (
		PRL_HANDLE hPartStat,
		PRL_UINT64_PTR pnSize
		) );


///////////////////////////////////////////////////////////////////////////////
/// @section Process statistics processing functionality.
///////////////////////////////////////////////////////////////////////////////

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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
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
PRL_METHOD_DECL( PARALLELS_API_VER_1,
				 PrlStatProc_GetCpuUsage, (
				PRL_HANDLE hProcStat,
				PRL_UINT32_PTR pnPercents
				) );


#ifdef _WIN_
    #pragma pack(pop, save_api_pack)
#endif

#ifdef __cplusplus
}
#endif

#endif // __PARALLELS_API_STATISTIC_H__
