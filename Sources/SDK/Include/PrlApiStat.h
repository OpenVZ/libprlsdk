/*
 * PrlApiStat.h
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
/// manipulation of the api library and are not specific
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
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
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
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
              Service.
   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.                     */
PRL_ASYNC_SRV_METHOD_DECL( PARALLELS_API_VER_1,
						   PrlSrv_SubscribeToHostStatistics, (
		PRL_HANDLE hServer
		) );

/* Cancels the Dispatcher Service statistics subscription that was started
   using the PrlSrv_SubscribeToHostStatistics function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_SERVER identifying the Dispatcher
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
   updated each time on the Dispatcher Service side.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hServer :  A handle of type PHT_SERVER identifying the
              Dispatcher Service.
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

/* Cancels the Dispatcher Service perfomace statistics subscription
   that was started using the PrlSrv_SubscribeToPerfStats function.

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:

   PRL_ERR_INVALID_ARG - invalid handle was passed.

   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   handle :  A handle of type PHT_SERVER identifying the Dispatcher
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
   hServer :  A handle of type PHT_SERVER identifying the Dispatcher
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

#ifdef _WIN_
    #pragma pack(pop, save_api_pack)
#endif

#ifdef __cplusplus
}
#endif

#endif // __PARALLELS_API_STATISTIC_H__
