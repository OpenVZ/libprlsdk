/*
 * PythonComposer.h: Definition of the class PythonComposer. This
 * class implements composer for python Parallels SDK.
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


#ifndef PYTHON_COMPOSER_H
#define PYTHON_COMPOSER_H


#include "Composer.h"


class PythonComposer : public Composer
{
public:

	PythonComposer(const SDKParser& sdk_parser, const QString& qsOutputDir);

	bool CompositePython();

private:

	bool AddClass(QString& content, const QString& qsClass, QString& qsHandleToObject);
	bool AddMethod(QString& content, const QString& qsFunc, bool bHasOwnHandle);
	bool AddArgDefaultValue(const QString& qsFunc, int nArgIndex, QString& qsMethArg);

	QStringList					m_lstClasses;
	QMap<QString , QString >	m_mapClassNames;
	QMap<QString , QString >	m_mapBaseClasses;
	QMap<QString , QString >	m_mapClassOption;
	QMap<QString , QString >	m_mapClassTypes;
	QMap<QString , QString >	m_mapClassFactories;

	struct CustomClassInfo
	{
		const char*		class_id;
		const char*		class_name;
		const char*		base_class;
		const char*		class_option;
		const char*		class_type;
		const char*		class_factory;

		const char*		reserved_1;
		const char*		reserved_2;
		const char*		reserved_3;
		const char*		reserved_4;
	};

	static CustomClassInfo s_CustomClassInfo[];
};


#endif	// PYTHON_COMPOSER_H
