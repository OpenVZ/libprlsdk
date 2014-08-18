/*
 * backtrace.c
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

#include "backtrace.h"
#include "Libraries/Logging/Logging.h"

#ifndef _WIN_
	#include <execinfo.h>
	#include <stdlib.h>
	#define MAX_BT_FRAMES 128
#endif

void log_backtrace(unsigned level, const char* where)
{
#ifndef _WIN_
	void*  callstack[MAX_BT_FRAMES];
	int frames = backtrace(callstack, MAX_BT_FRAMES);
	if (frames > 1)
	{
		int i;
		char** strs = backtrace_symbols(callstack, frames);

		WRITE_TRACE(level, "%s %d stack frames follows", where, frames-1);
		for (i = 1; i < frames; ++i)
			WRITE_TRACE(level, "%s\t%s", where, strs[i]);

		free(strs);
	}
#else
	(void)level;
	(void)where;
#endif
}
