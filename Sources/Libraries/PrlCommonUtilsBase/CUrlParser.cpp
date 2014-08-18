/*
 * CUrlParser.cpp: Helper class for parsing URLs strings like
 * [protocol://][user:password@]machine[:port].
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


#include "CUrlParser.h"
#include <QRegExp>

CUrlParser::CUrlParser(const QString &sUrl)
: m_sUrl(sUrl)
{}

QString CUrlParser::proto() const
{
	QRegExp _exp("^([a-z]*)://([^:]*:[^@]*@)?[^:]*(:[0-9]*)?$");
	int pos = _exp.indexIn(m_sUrl);
	if (pos > -1)
		return (_exp.cap(1));
	return QString();
}

QString CUrlParser::userName() const
{
	QRegExp _exp("^([a-z]*://)?([^:]*):[^@]*@.*$");
	int pos = _exp.indexIn(m_sUrl);
	if (pos > -1)
		return (_exp.cap(2));
	return QString();
}

QString CUrlParser::password() const
{
	QRegExp _exp("^([a-z]*://)?[^:]*:([^@]*)@.*$");
	int pos = _exp.indexIn(m_sUrl);
	if (pos > -1)
		return (_exp.cap(2));
	return QString();
}

QString CUrlParser::host() const
{
	QRegExp _exp("^([a-z]*://)?([^:]*:[^@]*@)?([^:]*)(:[0-9]*)?$");
	int pos = _exp.indexIn(m_sUrl);
	if (pos > -1)
		return (_exp.cap(3));
	return QString();
}

quint32 CUrlParser::port() const
{
	QRegExp _exp("^([a-z]*://)?([^:]*:[^@]*@)?[^:]*:([0-9]*)$");
	int pos = _exp.indexIn(m_sUrl);
	if (pos > -1)
		return (_exp.cap(3).toUInt());
	return (0);
}

