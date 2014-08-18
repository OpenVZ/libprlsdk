/*
 * XSDParser.cpp: XSDParser class implementation.
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


#include "XSDParser.h"


/**
 * @brief This constructor initializes QDomDocument object
 * @param[in] xsd Text in XSD format
 */
XSDParser::XSDParser(const QString& Xsd)
: m_iLine(0), m_iColumn(0)
{
	m_bWasError = !m_domXsdDocument.setContent(Xsd, &m_qsErrorMessage, &m_iLine, &m_iColumn);
}


/**
 * @brief Virtual destructor
 */
XSDParser::~XSDParser()
{
}


/**
 * @brief Check the QDomDocument object validation
 * @return boolean value: true - document is valid, false - is not valid
 */
bool XSDParser::IsXSDValid() const
{
	return !m_bWasError;
}


/**
 * @brief Get top node of the XSD document
 * @return QDomNode top object
 */
QDomNode XSDParser::GetTopNode() const
{
	return m_domXsdDocument;
}


/**
 * @brief Get parsing error message
 * @return parsing error message
 */
const QString& XSDParser::GetErrorMessage() const
{
	return m_qsErrorMessage;
}


/**
 * @brief Get line where error was occurred
 * @return line
 */
int XSDParser::GetLine() const
{
	return m_iLine;
}


/**
 * @brief Get column where error was occurred
 * @return column
 */
int XSDParser::GetColumn() const
{
	return m_iColumn;
}
