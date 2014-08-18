/*
 * CommandLine.cpp: Base Parallels types declarations.
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


#include "CommandLine.h"
#include <QRegExp>

#include "Interfaces/ParallelsQt.h"
#include "Libraries/Logging/Logging.h"

#ifdef _WIN_
#include <windows.h>
#endif

namespace CommandLine
{
	const char* g_strCommonKeyName_LogFileName		= "--logfile";
	const char* g_strCommonKeyName_PidFileName		= "--pidfile";
	const char* g_strCommonKeyName_CrashReporter		= "--crashreporter";
	const char* g_strCommonKeyName_CrashReporter_Dump	= "--dump";
	const char* g_strCommonKeyName_OpenFile			= "--openvm";
	const char* g_strCommonKeyName_Uuid			= "--uuid";
	const char* g_strCommonKeyName_DirUuid			= "--dir-uuid";
	const char* g_strCommonKeyName_SilentVmStart	= "--silent-vm-start";
	const char* g_strCommonKeyName_UTW7TuneMode		= "--utw7tune";
	const char* g_strCommonKeyName_StartOpenedVm	= "--start-opened-vm";
	const char* g_strCommonKeyName_DetachOpenedVm	= "--detach-opened-vm";
	const char* g_strCommonKeyName_StartMinimized	= "--start-minimized";
	const char* g_strCommonKeyName_AppStoreMode	= "--appstore";

	const char* g_strCommonKeyName_ModeName			= "--mode";
	const char* g_strCommonValue_ModeName_PS		= "ps";
	const char* g_strCommonValue_ModeName_PDfM	= "pdfm";
	const char* g_strCommonValue_ModeName_PDfWL	= "pdfwl";
	const char* g_strCommonValue_ModeName_PWE = "pwe";
	const char* g_strCommonValue_ModeName_PP		= "pp";
	const char* g_strCommonValue_ModeName_PM		= "pmobile";
	const char* g_strCommonValue_ModeName_CACHE		= "cache";
	const char* g_strCommonValue_ModeName_CACHE_INSTALL	= "cache_install";
	const char* g_strCommonKeyName_CRASH_IT		= "--crash_it";

	const char* g_strPlayerUseCase					= "--player";
	const char* g_strPlayerLocalhost				= "--localhost";
	const char* g_strPlayerHostName					= "--host";
	const char* g_strPlayerVmId						= "--vmuuid";
	const char* g_strPlayerUserName					= "--user";
	const char* g_strPlayerUserPassword				= "--passwd";
	const char* g_strVerbose				= "--verbose";
	const char* g_strSBAUpgrade				= "--sba-upgrade";

	const char* g_strSkipVmVersionCheck			= "--skip-vm-version-check";

	const char* g_strLaunchdMode						= "--launchd-mode";

}; //namespace CommandLine

using namespace CommandLine;

/*****************************************************************************/

Parser::Parser(
    int argc, char** argv)
{
#ifdef _WIN_
	#ifdef UNICODE
		// On Windows argv is not Unicode, so we have
		// to use Windows API to get Unicode command line.
		Q_UNUSED( argc )
		Q_UNUSED( argv )

		LPWSTR *szArglist;
		int nArgs;

		szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
		if(NULL != szArglist)
		{
			for(int i=0; i<nArgs; i++)
				m_lstArgs.push_back(UTF16_2QSTR((ushort*)szArglist[i]));

			LocalFree(szArglist);
		}
	#else // ! UNICODE
		for (int i=0; i<argc; i++)
			m_lstArgs.push_back(QString::fromLocal8Bit(argv[i]));
	#endif
#else // ! _WIN_
	for (int i=0; i<argc; i++)
		m_lstArgs.push_back(UTF8_2QSTR(argv[i]));
#endif
}

Parser::Parser ( const QString& cmd_line )
{
	QRegExp rx("\\s+");
	m_lstArgs=cmd_line.split(rx, QString::SkipEmptyParts);
	LOG_MESSAGE(DBG_FATAL, "m_lstArgs.size()=%d", m_lstArgs.size());
}

Parser::Parser ( const QStringList& cmd_args )
:m_lstArgs(cmd_args)
{
}

QString Parser::getValueByKey( const QString& key ) const
{
	QString value;
	int keyPos=m_lstArgs.lastIndexOf(key);
	if (	keyPos!=-1 //not found
			&& keyPos!=0 //app name
			&& keyPos!=m_lstArgs.size()-1 //has not value
		)
	{
		value=m_lstArgs[keyPos+1];
		if (value.startsWith("-") || value.startsWith("--"))
			value.clear();
	}
	return value;
}

bool Parser::hasKey ( const QString& key ) const
{
	int keyPos=m_lstArgs.lastIndexOf(key);
	return keyPos!=0 && keyPos!=-1;
}

/*****************************************************************************/
