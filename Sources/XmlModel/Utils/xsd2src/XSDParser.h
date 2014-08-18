/*
 * XSDParser.h: Definition of the class XSDParser. This class
 * implements XSD (XML Schema) parser.
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


#ifndef XSD_PARSER_H
#define XSD_PARSER_H


#include <QtCore>
#include <QtXml>


class XSDParser
{
public:

	// Initializing constructor
	explicit XSDParser(const QString& Xsd);
	// Virtual destructor
	virtual ~XSDParser();

	// Check parsing error of XSD document
	bool IsXSDValid() const;
	// Get top node
	QDomNode GetTopNode() const;

	// Get error message
	const QString& GetErrorMessage() const;
	// Get line
	int GetLine() const;
	// Get column
	int GetColumn() const;

private:

	// Default constructor (access denied)
	XSDParser() {}
	// Copy constructor (access denied)
	XSDParser(const XSDParser& ) {}
	// Assign operator (access denied)
	XSDParser& operator=(const XSDParser& ) { return *this; }

	// XSD document
	QDomDocument	m_domXsdDocument;

	// Error status
	bool	m_bWasError;
	// Error message
	QString	m_qsErrorMessage;
	// Line where error was occurred
	int		m_iLine;
	// Column where error was occurred
	int		m_iColumn;

};


#endif
