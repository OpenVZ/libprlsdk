/*
 * PrlTime.h: Time reference cross platform functions.
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


#ifndef STD_PrlTime_h__
#define STD_PrlTime_h__

#include "SDK/Include/PrlTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Return current tick counter value.
 */
PRL_UINT64 PrlTicks();

/*
 * Calculate delta between current tick counter value and
 * start value (for example, stored before by PrlTickStart).
 * This function properly handle tick counter overlap condition.
 * If new_start is given (not NULL) update it to current tick counter value.
 */
PRL_UINT64 PrlTicksDelta(PRL_UINT64 start, PRL_UINT64 *new_start);

/*
 * Convert value from ticks into microseconds
 */
PRL_UINT64 PrlTicksToMicro(PRL_UINT64 tick);

/*
 * Convert value from ticks into milliseconds
 */
PRL_UINT64 PrlTicksToMilli(PRL_UINT64 tick);

/*
 * Convert value from ticks into seconds
 */
PRL_UINT64 PrlTicksToSeconds(PRL_UINT64 tick);

/*
 * Return tick counter frequency (ticks per second)
 */
PRL_UINT64 PrlTicksFrequency();

/*
 *	Get value of ticks ( as 32 bit unsigned int )since some unknown moment for timing purposes.
 *  @return 32 bit unsigned int
 *  @note: WARNING: this value was overflow every 49,7 days
 */
unsigned int PrlGetTickCount();


/*
* Get value of ticks since some unknown moment for timing purposes.
* @return  64 bit unsigned int.
*
* @Note: PERFOMANCE WARNING: PrlGetTickCount64() under WINDOWS PLATFORM
* once time at 49,7 days way call lock ( QMutex(NonRecursive) ) to increment overflow_counter.
* PrlGetTickCount64() uses GetTickCount method and 	overflow_counter;
*/
PRL_UINT64 PrlGetTickCount64();

unsigned int PrlGetTicksPerSecond();

/*
 * Get monotonic time in microseconds
 */
PRL_UINT64 PrlGetTimeMonotonic();

#ifdef __cplusplus
}
#endif

#endif //STD_PrlTime_h__
