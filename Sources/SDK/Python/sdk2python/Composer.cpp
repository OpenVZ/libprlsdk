/*
 * Composer.cpp: Composer class implementation
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
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


#include "Composer.h"


Composer::Composer(const SDKParser& sdk_parser, const QString& qsOutputDir)
: m_sdk_parser(sdk_parser),
  m_qsOutputDir(qsOutputDir)
{
}

QString Composer::GetErrorMessage() const
{
	return m_qsErrorMessage;
}

bool Composer::Save(QString qsFileName, const QString& content)
{
	QFile file(m_qsOutputDir + "/" + qsFileName);

	if (file.exists())
	{
		if ( ! file.open(QIODevice::ReadOnly) )
		{
			m_qsErrorMessage = QString("Cannot open \"%1\" file!").arg(file.fileName());
			return false;
		}

		QString old_content = file.readAll();
		old_content.replace("\r\n", "\n");

		if (old_content == content)
			return true;

		file.close();
	}

	if ( ! file.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		m_qsErrorMessage = QString("Cannot open \"%1\" file to save it!").arg(file.fileName());
		return false;
	}

	file.write(content.toUtf8());

	return true;
}
