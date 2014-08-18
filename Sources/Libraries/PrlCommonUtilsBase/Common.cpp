/*
 * Common.cpp: file to use special edition of common functions
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


#include "Common.h"
#include <QString>
#include <QRegExp>
#include <QList>
#include "Interfaces/ParallelsTypes.h"
#include "Interfaces/ParallelsQt.h"

#ifdef _WIN_
#   include <windows.h>
#else
#   include <unistd.h>
#endif

// Description for patching known environment variables in string
struct EnvVarEntry
{
	EnvVarEntry(const QString& _val, INT _pos, UINT _len)
		: val(_val)
		, pos(_pos)
		, len(_len)
	{
	}
	// Value of environment variable
	QString val;
	// Position of this variable in string
	INT		pos;
	// Length of variable name
	UINT	len;
};

namespace Prl
{

void ProcessEnvVariables(QString& path)
{
	QString sep("%");

	if (path.section(sep, 1, 1).isEmpty())
		return;

	INT	idx;
	INT i = 1;
	QString	var;
	QRegExp regExp(QString("%.*%"));
	QList<EnvVarEntry> SystemVariables;

	idx = path.indexOf(regExp);
	while(idx >= 0)
	{
		var = path.section(sep, i, i);
		if ( !Prl::getenvU(var).isEmpty() )
			SystemVariables.append(EnvVarEntry(Prl::getenvU(var), idx, var.size() + 2));

		idx += var.size() + 2;
		i+=2;
		idx = path.indexOf(regExp, idx);
	}
	while(!SystemVariables.isEmpty())
	{
		path.replace(SystemVariables.last().pos, SystemVariables.last().len, SystemVariables.last().val);
		SystemVariables.pop_back();
	}
}

QString getenvU(const QString &parameter)
{
	QString value;
	if( parameter.isEmpty () )
		return value;
#ifdef _WIN_
	const wchar_t *const szEnvValue = _wgetenv(parameter.utf16());
	if( szEnvValue )
	{
		value = UTF16_2QSTR( szEnvValue );
	}
#else
	const QByteArray szEnvValue = qgetenv(parameter.toLocal8Bit().constData());
	if( !szEnvValue.isEmpty () )
	{
		value = QString::fromLocal8Bit( szEnvValue.constData() );
	}
#endif
	return value;
}



}	// end of namespace Prl
