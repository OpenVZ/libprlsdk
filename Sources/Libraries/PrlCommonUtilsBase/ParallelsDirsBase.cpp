/*
 * ParallelsDirsBase.cpp: Base functionality of ParallelsDirs class.
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


#include "ParallelsDirs.h"

#include "Libraries/Logging/Logging.h"

// #include "Build/Current.ver" for VER_FULL_BUILD_NUMBER_STR
#include "Build/Current.ver"

bool ParallelsDirs::Init( PRL_APPLICATION_MODE mode, ParallelsDirs::InitOptions options, bool bForce )
{
	bool ret = false;
	if( ms_bAppModeInited && ! bForce )
		WRITE_TRACE(DBG_FATAL, "Error: ParallelsDirs::Init( %d ) is already called!  This call will be ignored.", mode );
	else
	{
		ms_bAppModeInited = true;
		ms_nApplicationMode = mode;
		ms_nInitOptions = options;

		ret = true;

		WRITE_TRACE(DBG_FATAL, "ParallelsDirs::Init( ) was called. Current app mode = %d ( %s )"
			" initOpts = %d %s "
			" build version: %s %s"
			, mode
			, getAppExecuteModeAsCString()
			, (int)options, (options & smAppStoreMode)?"(AppStore mode enabled)":""
			, VER_PRODUCTVERSION_STR
			, VER_SPECIAL_BUILD_STR
			);
	}

	return ret;
}

const char* ParallelsDirs::getAppExecuteModeAsCString()
{
	return getAppExecuteModeAsCString( getAppExecuteMode() );
}
const char* ParallelsDirs::getAppExecuteModeAsCString( PRL_APPLICATION_MODE mode )
{
	switch( mode )
	{
	case PAM_SERVER:
		return "SERVER";
	default:
		return "UNKNOWN";
	}
}

PRL_APPLICATION_MODE ParallelsDirs::getAppExecuteMode()
{
	if( ! ms_bAppModeInited  )
		WRITE_TRACE(DBG_FATAL, "Error: ParallelsDirs::getAppExecuteMode() called without initialize!!! "
			"You should use ParallelsDirs::Init() before. Return %d", ms_nApplicationMode );
	return ms_nApplicationMode;
}

void ParallelsDirs::setLogPath(const char* path)
{
	SetLogFileName(path, GetProdDefaultLogFileName());
}
