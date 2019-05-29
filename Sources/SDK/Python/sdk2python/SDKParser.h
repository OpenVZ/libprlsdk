/*
 * SDKParser.h: Definition of the class SDKParser. This class
 * implements Virtuozzo SDK parser.
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


#ifndef SDK_PARSER_H
#define SDK_PARSER_H

#include <QtCore>


class SDKParser
{
public:

	SDKParser(const QString& qsSdkIncludeDir, const QString& qsDocFile);

// Common interface

	bool SetParsingLogFile(const QString& qsFileName);

	QString GetErrorMessage() const;

// Parsing interface

	bool Init();
	bool Parse();

// Data base interface

	struct Arg
	{
		QString type;
		QString name;
		bool	in;
		bool	out;
		QString out_type;
		QString out_name;

		Arg() : in(false), out(false) {}
	};

	const QStringList& GetAllFunctions() const;
	bool IsFunctionImplement(const QString& qsFunc) const;
	bool IsFunctionAsync(const QString& qsFunc) const;
	bool HasFunctionInArguments(const QString& qsFunc) const;
	bool HasFunctionOutArguments(const QString& qsFunc) const;

	QList<SDKParser::Arg > GetFunctionArguments(const QString& qsFunc) const;
	bool IsStringType(const QString& qsType) const;
	bool IsSignedType(const QString& qsType) const;
	bool Is8BitsType(const QString& qsType) const;
	bool Is16BitsType(const QString& qsType) const;
	bool Is64BitsType(const QString& qsType) const;
	bool IsHandleType(const QString& qsType) const;
	bool IsBooleanType(const QString& qsType) const;
	bool IsEnumerationType(const QString& qsType) const;
	bool IsDoubleType(const QString& qsType) const;
	QString GetTypeFormat(const QString& qsType) const;

	const QStringList& GetEnums() const;
	const QStringList& GetOsVersions() const;

	QStringList GetSessionNames() const;
	QString GetSessionValue(const QString& qsSessionName) const;

	QStringList GetScanCodeNames() const;
	QString GetScanCodeValue(const QString& qsScanCodeName) const;
	QString GetShortScanCodeName(const QString& qsScanCodeName) const;
	QString GetScanCodeFormat(QString qsValue) const;
	QString GetScanCodeArgs(QString qsValue) const;

	const QStringList& GetErrors() const;

	QString GetShortComment(const QString& qsEntity) const;
	QString GetLongComment(const QString& qsEntity) const;

	QStringList GetAllClasses() const;
	QStringList GetClassFunctions(const QString& qsClass) const;
	QString GetClassMethod(const QString& qsFunc) const;

private:

	void WriteLineInLog(QString qsLine);

	void RemoveComments();
	void RemoveComments(QString& qsSource);
	bool ParseAllFunctions();
	bool ParseFunctionArgs();
	bool ParseFunctionArgs(const QString& qsFuncName, const QString& qsArgs, bool bAsync);
	bool ParseEnums();
	bool ParseSessionConsts();
	bool ParseOses();
	bool ParseKeys();
	bool ParseErrors();
	bool ParseClasses();
	bool ParseDocumentation();

	bool AddClass(const QString& qsFunc);
	QString GetFunctionClass(const QString& qsFunc) const;

// Data base

	QString			m_qsPrlApiMacro;
	QStringList		m_lstAllFunctions;
	QStringList		m_lstNonStdFuncs;

	QString						m_qsPrlApi;
	QStringList					m_lstFuncUnimpl;
	QSet<QString >				m_setFuncAsync;
	QMap<QString, QList<Arg > >	m_mapFuncArgs;

	QString			m_qsPrlEnums;
	QStringList		m_lstEnums;
	QStringList		m_lstEnumTypes;

	QMap<QString, QString>		m_mapSessionUuids;

	QString			m_qsPrlOses;
	QStringList		m_lstOsVersions;

	QString						m_qsPrlKeys;
	QMap<QString , QString >	m_mapScanCodes;

	QString			m_qsPrlErrorsValues;
	QStringList		m_lstErrors;

	QMap<QString , QStringList >	m_mapClasses;

	QMap<QString, QStringList >		m_mapDocs;

// Other helpful data

	QString				m_qsSdkIncludeDir;
	QList<QString* >	m_lstSources;
	QString				m_qsDocFile;

	QString m_qsErrorMessage;

	QFile	m_fileLog;
};


#endif	// SDK_PARSER_H
