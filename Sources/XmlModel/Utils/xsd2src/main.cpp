/*
 * main.cpp: XML source composition utility.
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


#include "CodeComposer.h"

#define HELP \
	"Usage: xsd2src <input XSD file> <output directory> [<flag>] [<flag>].\n" \
	"-v, --version,             Utility version.\n" \
	"-h, help, --help           Show this help information.\n" \
	"flags:\n" \
	" -s, --signals             Add Qt signals mech to the generated classes.\n" \
	" -m, --merge               Add merge mech to the generated classes.\n" \
	" -p, --properties          Add change properties mech to the generated classes.\n"

#define UTILITY_VERSION			"1.0"
#define TRANSPORTER_XSD_SCHEMA	"Parallels Transporter XSD-schema"
#define SERVER_XSD_SCHEMA       "Parallels Server XSD-schema"

#include <iostream>

/**
* Main function
*/
int main(int argc, const char** argv)
{
#define out std::cout
#define err std::cerr

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
		out << "Type 'xsd2src help' for usage.\n";
		return 0;
	}

// Open XSD file

	QString file_name = argv[1];
	QFile xsd_file(file_name);
	if (!xsd_file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString msg = QString("Cannot open '%1' file.\n").arg(file_name);
		err << msg.toLocal8Bit().data();
		return 1;
	}
	QString xsd_data = xsd_file.readAll();
	xsd_file.close();

// Parse XSD file

	XSDParser xsd_parser(xsd_data);
	if (!xsd_parser.IsXSDValid())
	{
		QString msg = QString("Broken XSD document '%1' content.\n").arg(file_name);
		err << msg.toLocal8Bit().data();
		msg = QString("Error: %1, line: %2, column: %3.\n")
				.arg(xsd_parser.GetErrorMessage()).arg(xsd_parser.GetLine()).arg(xsd_parser.GetColumn());
		err << msg.toLocal8Bit().data();
		return 1;
	}

// Composite source code

	QString qsXSDName = QFileInfo(xsd_file).fileName();
	qsXSDName.remove(QRegExp("(\\.xsd)$"));

	CodeComposer code_composer(xsd_parser, qsXSDName);
	QString Flags;

	// Check if there is any flag in
	for ( int i = 3; i < argc ; i++ )
	{
	  QString qsParam = argv[i];

	  if (   qsParam == "-s" || qsParam == "--signals"
		  || qsParam == "-m" || qsParam == "--merge"
		  || qsParam == "-p" || qsParam == "--properties")
	  {
		  // Set optional flags
		  code_composer.SetFlag(qsParam);
	  }
	}

	if (!code_composer.CompositeSourceCode( argv[2] ))
	{
		err << code_composer.GetErrorMessage().toLocal8Bit().data();
		return 1;
	}

// Warnings

	out << code_composer.GetErrorMessage().toLocal8Bit().data();

	return 0;
}
