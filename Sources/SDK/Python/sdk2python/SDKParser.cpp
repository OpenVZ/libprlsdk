/*
 * SDKParser.cpp: SDKParser class implementation
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


#include "SDKParser.h"
#include <QtXml>
#include "ModuleTemplates.h"
#include <prlcommon/Interfaces/ParallelsQt.h>


#define PRL_API_MACRO_H			"PrlApiMacro.h"
#define PRL_API_CORE_H			"PrlApiCore.h"
#define PRL_API_DISP_H			"PrlApiDisp.h"
#define PRL_API_VM_H			"PrlApiVm.h"
#define PRL_API_NET_H			"PrlApiNet.h"
#define PRL_API_STAT_H			"PrlApiStat.h"
#define PRL_DISK_H				"PrlDisk.h"
#define PRL_ENUMS_H				"PrlEnums.h"
#define PRL_COMMANDS_FLAGS_H	"PrlCommandsFlags.h"
#define PRL_OSES_H				"PrlOses.h"
#define PRL_KEYS_H				"PrlKeys.h"
#define PRL_ERRORS_VALUES_H		"PrlErrorsValues.h"


SDKParser::SDKParser(const QString& qsSdkIncludeDir, const QString& qsDocFile)
: m_qsSdkIncludeDir(qsSdkIncludeDir),
  m_lstSources( QList<QString* >()
				<< &m_qsPrlApiMacro
				<< &m_qsPrlApi
				<< &m_qsPrlApi
				<< &m_qsPrlApi
				<< &m_qsPrlApi
				<< &m_qsPrlApi
				<< &m_qsPrlApi
				<< &m_qsPrlEnums
				<< &m_qsPrlEnums
				<< &m_qsPrlOses
				<< &m_qsPrlKeys
				<< &m_qsPrlErrorsValues
				<< &m_qsPrlEnums),
  m_qsDocFile(qsDocFile)
{
}

bool SDKParser::SetParsingLogFile(const QString& qsFileName)
{
	m_fileLog.setFileName(qsFileName);

	if ( ! m_fileLog.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
	{
		m_qsErrorMessage = QString("Cannot open log file \"%1\"!")
							.arg(qsFileName);
		return false;
	}

	return true;
}

void SDKParser::WriteLineInLog(QString qsLine)
{
	if ( ! m_fileLog.isOpen() )
		return;

	qsLine += "\n";
	m_fileLog.write(qsLine.toUtf8());
}

QString SDKParser::GetErrorMessage() const
{
	return m_qsErrorMessage;
}

bool SDKParser::Init()
{
	QStringList lstFileNames = QStringList()
		<< QString(PRL_API_MACRO_H)
		<< QString(PRL_API_CORE_H)
		<< QString(PRL_API_DISP_H)
		<< QString(PRL_API_VM_H)
		<< QString(PRL_API_NET_H)
		<< QString(PRL_API_STAT_H)
		<< QString(PRL_DISK_H)
		<< QString(PRL_ENUMS_H)
		<< QString(PRL_COMMANDS_FLAGS_H)
		<< QString(PRL_OSES_H)
		<< QString(PRL_KEYS_H)
		<< QString(PRL_ERRORS_VALUES_H)
		<< QString(PRL_KEYS_H);

	for(int i = 0; i < lstFileNames.size() && i < m_lstSources.size(); i++)
	{
		QFile file(m_qsSdkIncludeDir + "/" + lstFileNames[i]);
		if ( ! file.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
			m_qsErrorMessage = QString("Cannot open file \"%1\" [%2] !")
									.arg(file.fileName(), file.errorString());
			return false;
		}

		*m_lstSources[i] += UTF8_2QSTR(file.readAll());
	}

	return true;
}

bool SDKParser::Parse()
{
	RemoveComments();

	if ( ! ParseAllFunctions() )
		return false;

	if ( ! ParseFunctionArgs() )
		return false;

	if ( ! ParseEnums() )
		return false;

	if ( ! ParseSessionConsts() )
		return false;

	if ( ! ParseOses() )
		return false;

	if ( ! ParseKeys() )
		return false;

	if ( ! ParseErrors() )
		return false;

	if ( ! ParseClasses() )
		return false;

	if ( ! ParseDocumentation() )
		return false;

	return true;
}

const QStringList& SDKParser::GetAllFunctions() const
{
	return m_lstAllFunctions;
}

bool SDKParser::IsFunctionImplement(const QString& qsFunc) const
{
	return ( GetAllFunctions().contains(qsFunc) && ! m_lstFuncUnimpl.contains(qsFunc) );
}

bool SDKParser::IsFunctionAsync(const QString& qsFunc) const
{
	return m_setFuncAsync.contains(qsFunc);
}

bool SDKParser::HasFunctionInArguments(const QString& qsFunc) const
{
	foreach(Arg arg, GetFunctionArguments(qsFunc))
	{
		if (arg.in)
			return true;
	}
	return false;
}

bool SDKParser::HasFunctionOutArguments(const QString& qsFunc) const
{
	foreach(Arg arg, GetFunctionArguments(qsFunc))
	{
		if (arg.out)
			return true;
	}
	return false;
}

QList<SDKParser::Arg > SDKParser::GetFunctionArguments(const QString& qsFunc) const
{
	return m_mapFuncArgs.value(qsFunc);
}

bool SDKParser::IsStringType(const QString& qsType) const
{
	if (qsType.endsWith("_STR"))
		return true;
	return false;
}

bool SDKParser::IsSignedType(const QString& qsType) const
{
	if ( IsEnumerationType(qsType) || qsType.startsWith("PRL_INT"))
		return true;
	return false;
}

bool SDKParser::Is8BitsType(const QString& qsType) const
{
	if (QRegExp("PRL_\\w+8\\b").indexIn(qsType) != -1)
		return true;
	return false;
}

bool SDKParser::Is16BitsType(const QString& qsType) const
{
	if (QRegExp("PRL_\\w+16\\b").indexIn(qsType) != -1)
		return true;
	return false;
}

bool SDKParser::Is64BitsType(const QString& qsType) const
{
	if (QRegExp("PRL_\\w+64\\b").indexIn(qsType) != -1)
		return true;
	return false;
}

bool SDKParser::IsHandleType(const QString& qsType) const
{
	if (QRegExp("\\bPRL_HANDLE\\b").indexIn(qsType) != -1)
		return true;
	return false;
}

bool SDKParser::IsBooleanType(const QString& qsType) const
{
	if (QRegExp("\\bPRL_BOOL\\b").indexIn(qsType) != -1)
		return true;
	return false;
}

bool SDKParser::IsEnumerationType(const QString& qsType) const
{
	return m_lstEnumTypes.contains(qsType);
}

bool SDKParser::IsDoubleType(const QString& qsType) const
{
	if (QRegExp("\\bPRL_DOUBLE\\b").indexIn(qsType) != -1)
		return true;
	return false;
}

QString SDKParser::GetTypeFormat(const QString& qsType) const
{
	if (IsStringType(qsType))
		return "s";
	if (IsHandleType(qsType))
		return "k";
	if (IsDoubleType(qsType))
		return "d";
	if (Is8BitsType(qsType))
		return (IsSignedType(qsType) ? "c" : "B");
	if (Is16BitsType(qsType))
		return (IsSignedType(qsType) ? "h" : "H");
	if (Is64BitsType(qsType))
		return (IsSignedType(qsType) ? "L" : "K");
	return (IsSignedType(qsType) ? "i" : "I");
}

const QStringList& SDKParser::GetEnums() const
{
	return m_lstEnums;
}

QStringList SDKParser::GetSessionNames() const
{
	return m_mapSessionUuids.keys();
}

QString SDKParser::GetSessionValue(const QString& qsSessionName) const
{
	return m_mapSessionUuids.value(qsSessionName);
}

const QStringList& SDKParser::GetOsVersions() const
{
	return m_lstOsVersions;
}

QStringList SDKParser::GetScanCodeNames() const
{
	return m_mapScanCodes.keys();
}

QString SDKParser::GetScanCodeValue(const QString& qsScanCodeName) const
{
	return m_mapScanCodes.value(qsScanCodeName);
}

QString SDKParser::GetScanCodeFormat(QString qsValue) const
{
	int size = qsValue.split(',').size();

	QStringList lstTmp;
	for(int i = 0; i < size; ++i)
	{
		lstTmp += "b";
	}
	return ( "(" + lstTmp.join(",") + ")" );
}

QString SDKParser::GetScanCodeArgs(QString qsValue) const
{
	return qsValue.remove("{").remove("}");
}

QString SDKParser::GetShortScanCodeName(const QString& qsScanCodeName) const
{
	return qsScanCodeName.mid(sizeof("PRL_SCAN_CODE_")-1);
}

const QStringList& SDKParser::GetErrors() const
{
	return m_lstErrors;
}

QString SDKParser::GetShortComment(const QString& qsEntity) const
{
	QStringList lstComments = m_mapDocs.value(qsEntity);
	return lstComments.isEmpty() ? qsEntity : lstComments[0];
}

QString SDKParser::GetLongComment(const QString& qsEntity) const
{
	QStringList lstComments = m_mapDocs.value(qsEntity);
	return lstComments.join("\n").trimmed();
}

QStringList SDKParser::GetAllClasses() const
{
	return m_mapClasses.keys();
}

QStringList SDKParser::GetClassFunctions(const QString& qsClass) const
{
	return m_mapClasses.value(qsClass);
}

#define CLS_DELIMITER		'_'

QString SDKParser::GetClassMethod(const QString& qsFunc) const
{
	int idx = qsFunc.indexOf(CLS_DELIMITER);
	QString qsMethod = qsFunc.mid(idx + 1);

	QRegExp re("[a-z][A-Z]");
	re.setMinimal(true);

	int pos = 0;
	while((pos = re.indexIn(qsMethod, pos)) != -1)
	{
		qsMethod.insert(pos + 1, "_");
		pos += re.matchedLength();
	}

	return qsMethod.toLower();
}

void SDKParser::RemoveComments()
{
	foreach(QString* pqsSource, m_lstSources)
	{
		RemoveComments( *pqsSource );
	}
}

void SDKParser::RemoveComments(QString& qsSource)
{
	QRegExp re;
	re.setMinimal(true);

	for(int i = 0; i < 2; i++)
	{
		if ( ! i )
// 1 Multiline comment
			re.setPattern("/\\*.*\\*/");
		else
// 2 Single line comment
			re.setPattern("//.*\\n");

		int pos = 0;
		while((pos = re.indexIn(qsSource, pos)) != -1)
		{
			qsSource.remove(pos, re.matchedLength());
		}
	}
}

#define WHOLE_WORD_PN	"\\b\\w+\\b"

bool SDKParser::ParseAllFunctions()
{
	WriteLineInLog("###################################");
	WriteLineInLog("###### MAIN LIST OF FUNCTIONS #####");
	WriteLineInLog("###################################\n");

	QRegExp re( "Prl\\w+_\\w+\\s*\\," );

	int pos = 0;
	while((pos = re.indexIn(m_qsPrlApi, pos)) != -1)
	{
		QString qsFunc = m_qsPrlApi.mid(pos, re.matchedLength());
		qsFunc = qsFunc.remove(",").trimmed();

		pos += re.matchedLength();

		WriteLineInLog(qsFunc);
	}

	WriteLineInLog("\n###################################");
	WriteLineInLog(  "###### DB LIST OF FUNCTIONS #######");
	WriteLineInLog(  "###################################\n");

	re.setPattern( "\\(\\s*"WHOLE_WORD_PN"\\s*\\)" );

	pos = 0;
	while((pos = re.indexIn(m_qsPrlApiMacro, pos)) != -1)
	{
		QString qsFunc = m_qsPrlApiMacro.mid(pos, re.matchedLength());
		qsFunc = qsFunc.remove("(").remove(")").trimmed();

		pos += re.matchedLength();

		m_lstAllFunctions += qsFunc;
		WriteLineInLog(qsFunc);
	}

	if ( m_lstAllFunctions.isEmpty() )
	{
		m_qsErrorMessage = "No one function was found!";
		return false;
	}

	// Non-standard functions check

	m_lstNonStdFuncs = QStringList()
		<< MDL_PRL_OP_TYPE_LIST_GET_ITEM
		<< MDL_PRL_REPORT_GET_DATA
		<< MDL_PRL_REPORT_SEND
		<< MDL_PRL_API_SEND_PROBLEM_REPORT
		<< MDL_PRL_API_SEND_PACKED_PROBLEM_REPORT
		<< MDL_PRL_SRV_GET_DEFAULT_VM_CONFIG
		<< MDL_PRL_GET_MEMGUARANTEE_SIZE
		<< MDL_PRL_SET_MEMGUARANTEE_SIZE
		<< MDL_PRL_DISK_MAP_READ
		<< MDL_PRL_DISK_GET_DISK_INFO;

	foreach(QString qsNonStdFunc, m_lstNonStdFuncs)
	{
		if ( ! m_lstAllFunctions.contains(qsNonStdFunc) )
		{
			m_qsErrorMessage = QString("Non-standard function %1 was not found!").arg(qsNonStdFunc);
			return false;
		}
	}

	WriteLineInLog(QString("\nTOTAL NUMBER OF FUNCTIONS: %1").arg(m_lstAllFunctions.size()));

	return true;
}

bool SDKParser::ParseFunctionArgs()
{
	WriteLineInLog("\n###################################");
	WriteLineInLog(  "######### FUNCTIONS INFO ##########");
	WriteLineInLog(  "###################################\n");

	QString qsUnusedFuncs;

	QSet<QString >::iterator it;
	foreach(QString qsFuncName, m_lstAllFunctions)
	{
		int idx = m_qsPrlApi.indexOf(QRegExp("\\b" + qsFuncName + "\\b"));
		if (idx == -1)
		{
			qsUnusedFuncs += (qsFuncName + " ");
			continue;
		}
		idx += qsFuncName.size();

		int idx2 = m_qsPrlApi.indexOf(";", idx);
		if (idx2 == -1)
			continue;	// unexpected
		QString qsArgs = m_qsPrlApi.mid(idx, idx2 - idx);

		idx = m_qsPrlApi.lastIndexOf(QRegExp("\\bPRL_(ASYNC_)?(SRV_)?METHOD_DECL\\b"), idx);
		if (idx == -1)
			continue;	// unexpected
		QString qsFunc = m_qsPrlApi.mid(idx, idx2 - idx);

		bool bAsync = qsFunc.startsWith("PRL_ASYNC_");

		if ( ! ParseFunctionArgs(qsFuncName, qsArgs, bAsync) )
			return false;
	}

	if ( m_mapFuncArgs.size() != m_lstAllFunctions.size() )
	{
		m_qsErrorMessage = QString("Wrong format of the header file: functions %1 not found!")
							.arg(qsUnusedFuncs.trimmed());
		return false;
	}

	WriteLineInLog(QString("\nTOTAL NUMBER OF UNIMPLEMENT FUNCTIONS: %1\nLIST OF UNIMPLEMENT FUNCTIONS:")
					.arg(m_lstFuncUnimpl.size()));
	WriteLineInLog("\t" + m_lstFuncUnimpl.join("\n\t") + "\n");

	return true;
}

bool SDKParser::ParseFunctionArgs(const QString& qsFuncName, const QString& qsArgs, bool bAsync)
{
#define TYPE_PN		"(const\\s+)?\\w+((\\s*\\*)+)?"
#define ARG_PN		TYPE_PN"(\\s+\\w+)?\\s*[\\,\\)]"

	WriteLineInLog(qsFuncName);
	WriteLineInLog(QString("\tAsync: ") + (bAsync ? "true" : "false"));

	QRegExp re(ARG_PN);

	int pos = 0;
	QList<Arg > lstArgs;
	while((pos = re.indexIn(qsArgs, pos)) != -1)
	{
		QString qsArg = qsArgs.mid(pos, re.matchedLength());

		QRegExp re2(TYPE_PN);
		int idx = re2.indexIn(qsArg);
		if (idx == -1)
			continue;	// unexpected

		Arg arg;
		arg.type = qsArg.mid(idx, re2.matchedLength());
		if ( arg.type == "PRL_VOID" || arg.type == "void" ) {
			pos += re.matchedLength();
			continue;	// means no arguments
		}

		arg.name = qsArg.mid(idx + re2.matchedLength())
						.remove(",").remove(")").trimmed();
		if (arg.name.isEmpty())
		{
			m_qsErrorMessage = QString("An argument has no name in '%1' function!").arg(qsFuncName);
			return false;
		}

		bool bWrongArg =
			(  QRegExp("\\*\\s*\\*").indexIn(arg.type) != -1
			|| QRegExp("_PTR\\s*\\*").indexIn(arg.type) != -1
			|| QRegExp("_PTR_PTR").indexIn(arg.type) != -1
			|| QRegExp("_VOID_").indexIn(arg.type) != -1
			|| QRegExp("_HANDLER").indexIn(arg.type) != -1
			|| QRegExp("_CALLBACK").indexIn(arg.type) != -1
			|| QRegExp("PRL_IO_").indexIn(arg.type) != -1
			|| QRegExp("PRL_TIS_RECORD_INFO").indexIn(arg.type) != -1
			|| QRegExp("PRL_DISK_PARAMETERS").indexIn(arg.type) != -1
			|| QRegExp("PRL_SNAPSHOT_ELEMENT").indexIn(arg.type) != -1
			|| QRegExp("PRL_CHS").indexIn(arg.type) != -1
			|| QRegExp("PRL_CPULIMIT_DATA").indexIn(arg.type) != -1
			|| QRegExp("PRL_IOLIMIT_DATA").indexIn(arg.type) != -1
			|| QRegExp("PRL_PROXY_HOST_INFO").indexIn(arg.type) != -1
			|| QRegExp("PRL_PROXY_CLIENT_INFO").indexIn(arg.type) != -1
			|| (arg.type != "PRL_CONST_STR_PTR" && QRegExp("_CONST_\\w+_PTR").indexIn(arg.type) != -1)
			);

		if (   bWrongArg
			&& ! m_lstNonStdFuncs.contains(qsFuncName)
			)
		{
			if ( ! m_lstFuncUnimpl.contains(qsFuncName) )
				m_lstFuncUnimpl += qsFuncName;
		}
		else
		{
			if ( ! bWrongArg && (  arg.type.endsWith("*")
								|| arg.type.endsWith("_PTR")
								|| arg.type == "PRL_STR"
								|| arg.type == "PRL_CHAR_PTR"
								)
				)
			{
				arg.out = true;

				if (arg.type == "PRL_CHAR_PTR")
					arg.type = "PRL_STR";

				int idx = arg.type.lastIndexOf("*");
				if ( idx == -1 )
					idx = arg.type.lastIndexOf("_PTR");
				arg.out_type = arg.type.mid(0, idx);

				arg.out_name = arg.name;
				if (idx != -1 && arg.name.at(0) == 'p')
					arg.out_name[0] = 'r';
			}
			else
			{
				arg.type = arg.type.remove("const").trimmed();
				arg.in = true;
			}
		}

		lstArgs += arg;
		WriteLineInLog("\t\t" + arg.type + "\t" + (arg.name + "\t")
			+ (arg.in ? "[in]" : (arg.out ? "[out]" : "[?]")));

		pos += re.matchedLength();
	}

	if ( m_lstFuncUnimpl.contains(qsFuncName) )
		WriteLineInLog("\t*** WILL IMPLEMENT LATER ***");

	if (bAsync)
		m_setFuncAsync.insert(qsFuncName);
	m_mapFuncArgs.insert(qsFuncName, lstArgs);

	return true;
}

bool SDKParser::ParseEnums()
{
	WriteLineInLog("\n###################################");
	WriteLineInLog(  "########## ENUMERATIONS ###########");
	WriteLineInLog(  "###################################\n");

	m_lstEnums << "PARALLELS_API_VER";
	WriteLineInLog(m_lstEnums[0]);
	m_lstEnums << "PRL_INVALID_HANDLE";
	WriteLineInLog(m_lstEnums[1]);
	m_lstEnums << "PRL_INVALID_FILE_DESCRIPTOR";
	WriteLineInLog(m_lstEnums[2]);

	QRegExp re( WHOLE_WORD_PN"\\s*(\\=.*)?[\\,\\}]" );
	re.setMinimal(true);

	int pos = 0;
	while((pos = re.indexIn(m_qsPrlEnums, pos)) != -1)
	{
		QString qsConst = m_qsPrlEnums.mid(pos, re.matchedLength());

		QRegExp re2(WHOLE_WORD_PN);
		int idx = re2.indexIn(qsConst);
		if (idx == -1)
			continue;	// unexpected

		pos += re.matchedLength();

		qsConst = qsConst.mid(idx, re2.matchedLength());
		if ( ! qsConst.isEmpty() && qsConst.at(0).isDigit())
			continue;

		m_lstEnums += qsConst;
		WriteLineInLog(qsConst);
	}

	if ( m_lstEnums.isEmpty() )
	{
		m_qsErrorMessage = "Wrong format of the \""PRL_ENUMS_H"\" file: no one enum was found!";
		return false;
	}

	WriteLineInLog("\n###################################");
	WriteLineInLog(  "######## ENUMERATION TYPES ########");
	WriteLineInLog(  "###################################\n");

	re.setPattern("\\}\\s*"WHOLE_WORD_PN"\\s*;");
	pos = 0;
	while((pos = re.indexIn(m_qsPrlEnums, pos)) != -1)
	{
		QString qsEnumType = m_qsPrlEnums.mid(pos, re.matchedLength());

		QRegExp re2(WHOLE_WORD_PN);
		int idx = re2.indexIn(qsEnumType);
		if (idx == -1)
			continue;	// unexpected
		qsEnumType = qsEnumType.mid(idx, re2.matchedLength());

		m_lstEnumTypes += qsEnumType;
		WriteLineInLog(qsEnumType);

		pos += re.matchedLength();
	}

	return true;
}

#define WHOLE_UUID_PN	"\"\\b\\w+-\\w+-\\w+-\\w+-\\w+\\b\""

bool SDKParser::ParseSessionConsts()
{
	WriteLineInLog("\n###################################");
	WriteLineInLog(  "########## PARSE SESSIONS ###########");
	WriteLineInLog(  "###################################\n");

	QRegExp re("\\#\\s*define\\s+("WHOLE_WORD_PN")\\s+("WHOLE_UUID_PN")");

	int pos = 0;
	while((pos = re.indexIn(m_qsPrlEnums, pos)) != -1)
	{
		WriteLineInLog(QString("%1 %2").arg(re.cap(1), re.cap(2)));

		m_mapSessionUuids.insert(re.cap(1), re.cap(2));

		pos += re.matchedLength();
	}

	if ( m_mapSessionUuids.isEmpty() )
	{
		m_qsErrorMessage = "Session constants not found!";
		return false;
	}

	return true;
}

bool SDKParser::ParseOses()
{
	WriteLineInLog("\n###################################");
	WriteLineInLog(  "########### OS VERSIONS ###########");
	WriteLineInLog(  "###################################\n");

	QRegExp re("\\#\\s*define\\s+"WHOLE_WORD_PN"\\s+"WHOLE_WORD_PN);

	int pos = 0;
	while((pos = re.indexIn(m_qsPrlOses, pos)) != -1)
	{
		QString qsDefine = m_qsPrlOses.mid(pos, re.matchedLength());

		int idx = qsDefine.lastIndexOf(QRegExp(WHOLE_WORD_PN));
		if (idx == -1)
			continue; // unexpected
		qsDefine = qsDefine.mid(0, idx);

		idx = qsDefine.lastIndexOf(QRegExp(WHOLE_WORD_PN));
		if (idx == -1)
			continue; // unexpected

		QString qsOsVer = qsDefine.mid(idx).trimmed();
		m_lstOsVersions += qsOsVer;
		WriteLineInLog(qsOsVer);

		pos += re.matchedLength();
	}

	if ( m_lstOsVersions.isEmpty() )
	{
		m_qsErrorMessage = "Wrong format of the \""PRL_OSES_H"\" file: no one #define was found!";
		return false;
	}

	return true;
}

bool SDKParser::ParseKeys()
{
	WriteLineInLog("\n###################################");
	WriteLineInLog(  "########### SCAN CODES ############");
	WriteLineInLog(  "###################################\n");

	QRegExp re("\\#\\s*define\\s+"WHOLE_WORD_PN"\\s*\\{.+\\}");
	re.setMinimal(true);

	int pos = 0;
	while((pos = re.indexIn(m_qsPrlKeys, pos)) != -1)
	{
		QString qsDefine = m_qsPrlKeys.mid(pos, re.matchedLength());

		int idx = qsDefine.lastIndexOf("{");
		if (idx == -1)
			continue; // unexpected
		QString qsValues = qsDefine.mid(idx);

		qsDefine = qsDefine.mid(0, idx);
		idx = qsDefine.lastIndexOf(QRegExp(WHOLE_WORD_PN));
		if (idx == -1)
			continue; // unexpected
		QString qsScanCode = qsDefine.mid(idx).trimmed();

		m_mapScanCodes.insert(qsScanCode, qsValues);
		WriteLineInLog(qsScanCode + "\t\t" + qsValues);

		pos += re.matchedLength();
	}

	if ( m_mapScanCodes.isEmpty() )
	{
		m_qsErrorMessage = "Wrong format of the \""PRL_KEYS_H"\" file: no one #define was found!";
		return false;
	}

	return true;
}

bool SDKParser::ParseErrors()
{
	WriteLineInLog("\n###################################");
	WriteLineInLog(  "########## ERROR VALUES ###########");
	WriteLineInLog(  "###################################\n");

	QRegExp re("\\#\\s*define\\s+"WHOLE_WORD_PN);
	re.setMinimal(true);

	int pos = 0;
	while((pos = re.indexIn(m_qsPrlErrorsValues, pos)) != -1)
	{
		QString qsDefine = m_qsPrlErrorsValues.mid(pos, re.matchedLength());

		int idx = qsDefine.lastIndexOf(QRegExp(WHOLE_WORD_PN));
		if (idx == -1)
			continue; // unexpected

		QString qsError = qsDefine.mid(idx);
		if (qsError.at(0) != '_')
		{
			m_lstErrors += qsError;
			WriteLineInLog(qsError);
		}

		pos += re.matchedLength();
	}

	if ( m_lstErrors.isEmpty() )
	{
		m_qsErrorMessage = "Wrong format of the \""PRL_ERRORS_VALUES_H"\" file: no one #define was found!";
		return false;
	}

	return true;
}

bool SDKParser::ParseClasses()
{
	WriteLineInLog("\n###################################");
	WriteLineInLog(  "############# CLASSES #############");
	WriteLineInLog(  "###################################\n");

	foreach(QString qsFunc, GetAllFunctions())
	{
		if ( ! IsFunctionImplement(qsFunc) )
			continue;

		if ( ! AddClass(qsFunc) )
			return false;
	}

	return true;
}

bool SDKParser::ParseDocumentation()
{
	if (m_qsDocFile.isEmpty())
		return true;

	QDomDocument doc;
	QFile file(m_qsDocFile);
	QString qsMsg;
	if ( ! doc.setContent(&file, &qsMsg) )
	{
		m_qsErrorMessage = QString("XML documentation file \"%1\" parsing failed: %2!")
							.arg(m_qsDocFile, qsMsg);
		return false;
	}

	QDomElement root = doc.firstChildElement("parallels_python_api");
	if (root.isNull())
	{
		m_qsErrorMessage = QString("XML documentation file \"%1\" has wrong format!").arg(m_qsDocFile);
		return false;
	}

	QDomElement docList = root.firstChildElement("symbol");
	while( ! docList.isNull() )
	{
		QDomElement name = docList.firstChildElement("name");
		QDomElement short_comment = docList.firstChildElement("shortcomment");
		QDomElement long_comment = docList.firstChildElement("longcomment");

		if (name.isNull() || short_comment.isNull() || long_comment.isNull())
		{
			m_qsErrorMessage = QString("XML documentation file \"%1\" has wrong format!").arg(m_qsDocFile);
			return false;
		}

		QStringList lstComments;

		QString qsShortComment = short_comment.firstChild().toText().data();
		lstComments += qsShortComment;

		QDomElement line = long_comment.firstChildElement();
		while( ! line.isNull() )
		{
			QString qsLine = line.firstChild().toText().data();
			lstComments += qsLine;

			line = line.nextSiblingElement();
		}

		QString qsName = name.firstChild().toText().data();
		m_mapDocs.insert(qsName, lstComments);

		docList = docList.nextSiblingElement("symbol");
	}

	return true;
}

bool SDKParser::AddClass(const QString& qsFunc)
{
	QString qsClass = GetFunctionClass(qsFunc);
	if ( qsClass.isEmpty() )
	{
		m_qsErrorMessage = QString("Function %1 is not Parallels SDK API function!")
							.arg(qsFunc);
		return false;
	}

	if ( ! m_mapClasses.contains(qsClass) )
	{
		m_mapClasses.insert( qsClass, QStringList() );
		WriteLineInLog(qsClass);
	}

	m_mapClasses[qsClass] += qsFunc;

	return true;
}

QString SDKParser::GetFunctionClass(const QString& qsFunc) const
{
#define CLS_PRL_PREFIX		"Prl"

	if ( ! qsFunc.startsWith(CLS_PRL_PREFIX) && ! qsFunc.contains(CLS_DELIMITER))
		return QString();

	int idx = sizeof(CLS_PRL_PREFIX) - 1;
	return qsFunc.mid(idx, qsFunc.indexOf(CLS_DELIMITER) - idx);
}
