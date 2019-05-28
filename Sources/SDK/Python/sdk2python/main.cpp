/*
 * main.cpp: Python composition utility.
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


#include "ModuleComposer.h"
#include "PythonComposer.h"


#define HELP \
	"Usage: sdk2python <Virtuozzo SDK include directory> <output directory>\n" \
	"                  [<XML document file>] [<Parsing log file>]\n" \
	"\n" \
	"-v, --version,      Utility version.\n" \
	"-h, help, --help    Show this help information.\n"

#define UTILITY_VERSION		"1.0"

#include <iostream>


/**
* Main function
*/
int main(int argc, const char** argv)
{
#define out std::cout
#define err std::cerr

#define print_err(msg) err << "ERROR: " << msg << "\n"

	if (argc == 2)
	{
		QString param = argv[1];
		if (param == "help" || param == "--help" || param == "-h")
		{
			out << HELP;
			return 0;
		}

		if (param == "-v" || param == "--version")
		{
			out << "Current version is ";
			out << UTILITY_VERSION;
			out << "\n";
			return 0;
		}
	}

	if (argc < 3)
	{
		out << "Type 'sdk2python help' for usage.\n";
		return 0;
	}

	QString qsSdkIncludeDir = QFileInfo(argv[1]).absoluteFilePath();
	if ( ! QDir().exists(qsSdkIncludeDir) )
	{
		print_err(("The Virtuozzo SDK include directoy \""
					+ qsSdkIncludeDir
					+ "\" doesn't exist!").toUtf8().data());
		return -1;
	}

	QString qsOutputDir = QFileInfo(argv[2]).absoluteFilePath();
	if ( ! QDir().exists(qsOutputDir) )
	{
		print_err(("The output directoy \""
					+ qsOutputDir
					+ "\" doesn't exist!").toUtf8().data());
		return -2;
	}

	QString qsDocFile;
	if (argc >= 4)
	{
		qsDocFile = QFileInfo(argv[3]).absoluteFilePath();
		if ( ! QFile().exists(qsDocFile) )
		{
			print_err(("The XML document file \""
						+ qsDocFile
						+ "\" doesn't exist!").toUtf8().data());
			return -8;
		}
	}

// Parse SDK

	SDKParser sdk_parser(qsSdkIncludeDir, qsDocFile);

	if (argc >= 5)
	{
		QString qsFileLog = QFileInfo(argv[4]).absoluteFilePath();
		if ( ! sdk_parser.SetParsingLogFile(qsFileLog) )
		{
			print_err(sdk_parser.GetErrorMessage().toUtf8().data());
			return -5;
		}
	}

	if ( ! sdk_parser.Init() )
	{
		print_err(sdk_parser.GetErrorMessage().toUtf8().data());
		return -3;
	}

	if ( ! sdk_parser.Parse() )
	{
		print_err(sdk_parser.GetErrorMessage().toUtf8().data());
		return -4;
	}

// Composite SDK functions registration module

	ModuleComposer module_composer(sdk_parser, qsOutputDir);

	if ( ! module_composer.CompositeModule() )
	{
		print_err(module_composer.GetErrorMessage().toUtf8().data());
		return -6;
	}

// Composite python classes

	PythonComposer python_composer(sdk_parser, qsOutputDir);

	if ( ! python_composer.CompositePython() )
	{
		print_err(python_composer.GetErrorMessage().toUtf8().data());
		return -7;
	}

	return 0;
}
