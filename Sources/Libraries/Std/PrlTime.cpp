/*
 * PrlTime.cpp: Time reference crossplatform functions.
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


#include "PrlTime.h"

#if defined(_LIN_)
	#include <time.h>
#elif defined(_WIN_)
	#include <Windows.h>
#endif

#include "Libraries/Std/AtomicOps.h"


#ifdef _WIN_

typedef ULONGLONG (WINAPI *PGET_TICK_COUNT64)(void);
static PGET_TICK_COUNT64 pGetTickCount64;
static ULARGE_INTEGER Ticks;

static PRL_UINT64 PrlGetTimeWin()
{
	ULARGE_INTEGER Prev, Next;

	if (pGetTickCount64)
		return (*pGetTickCount64)() * 1000;

	// Protect from overflow on old Windows platforms (before Vista and 2008).
	// GetTickCount64() was introduced in Windows Vista and Windows Server 2008.
	Prev.QuadPart = Ticks.QuadPart;
	do
	{
		Next.LowPart = GetTickCount();
		Next.HighPart = Prev.HighPart;
		if (Next.LowPart < Prev.LowPart)
			Next.HighPart++;
		Prev.QuadPart = AtomicCompareSwap64U(&Ticks.QuadPart, Prev.QuadPart, Next.QuadPart);
	} while (Prev.QuadPart != Next.QuadPart);

	return Next.QuadPart * 1000;
}

#endif

#if defined(_WIN_)

typedef PRL_UINT64 (PRL_GET_TIME)(void);
static PRL_GET_TIME *pPrlGetTime;

static PRL_GET_TIME* PrlGetTimeSource()
{
#ifdef _WIN_
	HMODULE m = GetModuleHandleA("kernel32.dll");
	pGetTickCount64 = (PGET_TICK_COUNT64)GetProcAddress(m, "GetTickCount64");
	return PrlGetTimeWin;
#else
	#error "PrlGetTimeSource() is not implemented for the current platform"
#endif
}

#endif

/*
 * Get monotonic time in microseconds
 */
PRL_UINT64 PrlGetTimeMonotonic()
{
#ifdef _LIN_
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts))
		return 0;
	return ((PRL_UINT64)ts.tv_sec * 1000000) + (ts.tv_nsec / 1000);
#else
	if (pPrlGetTime == NULL)
		pPrlGetTime = PrlGetTimeSource();

	return (*pPrlGetTime)();
#endif
}

unsigned int PrlGetTicksPerSecond()
{
	return 1000;
}

unsigned int PrlGetTickCount()
{
	// FIXME: PrlGetTickCount() is deprecated, use PrlGetTimeMonotonic() instead.
	return (unsigned int)(PrlGetTimeMonotonic() / 1000);
}

PRL_UINT64 PrlGetTickCount64()
{
	// FIXME: PrlGetTickCount64() is deprecated, use PrlGetTimeMonotonic() instead.
	return (PrlGetTimeMonotonic() / 1000);
}
