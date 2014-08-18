/*
 * CommandLine.h: Base Parallels types declarations
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


#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__

#include <QString>
#include <QStringList>

namespace CommandLine
{
	extern const char* g_strCommonKeyName_LogFileName;
	extern const char* g_strCommonKeyName_PidFileName;
	extern const char* g_strCommonKeyName_CrashReporter;
	extern const char* g_strCommonKeyName_CrashReporter_Dump;
	extern const char* g_strCommonKeyName_OpenFile;
	extern const char* g_strCommonKeyName_Uuid;
	extern const char* g_strCommonKeyName_DirUuid;
	extern const char* g_strCommonKeyName_SilentVmStart;
	extern const char* g_strCommonKeyName_UTW7TuneMode;
	extern const char* g_strCommonKeyName_StartOpenedVm;
	extern const char* g_strCommonKeyName_DetachOpenedVm;
	extern const char* g_strCommonKeyName_StartMinimized;
	extern const char* g_strCommonKeyName_AppStoreMode;

	extern const char* g_strCommonKeyName_ModeName;
	extern const char* g_strCommonValue_ModeName_PS;
	extern const char* g_strCommonValue_ModeName_PDfM;
	extern const char* g_strCommonValue_ModeName_PDfWL;
	extern const char* g_strCommonValue_ModeName_PWE;
	extern const char* g_strCommonValue_ModeName_PP;
	extern const char* g_strCommonValue_ModeName_PM;
	extern const char* g_strCommonValue_ModeName_CACHE;
	extern const char* g_strCommonValue_ModeName_CACHE_INSTALL;
	extern const char* g_strCommonKeyName_CRASH_IT;

	extern const char* g_strPlayerHostName;
	extern const char* g_strPlayerLocalhost;
	extern const char* g_strPlayerVmId;
	extern const char* g_strPlayerUserName;
	extern const char* g_strPlayerUserPassword;
	extern const char* g_strVerbose;
	extern const char* g_strSBAUpgrade;

	extern const char* g_strSkipVmVersionCheck;
	extern const char* g_strLaunchdMode;

	class Parser
	{
	public:
		// NOTE! On Windows, this constructor ignores argc/argv arguments and uses native
		// WinAPI calls GetCommandLineW/CommandLineToArgvW to get command line parameters of a caller's
		// process as Unicode stings.
		Parser ( int argc, char** argv );
		Parser ( const QString& cmd_line );
		Parser ( const QStringList& cmd_args );

		//return "" if not exist.
		QString	getValueByKey ( const QString& key ) const;

		//return true if exist
		bool hasKey ( const QString& key ) const;

		//get a copy of arguments list
		QStringList getArgsList() const { return m_lstArgs; }

	private:
		QStringList m_lstArgs;
	};

};

#endif // __COMMAND_LINE_H__
