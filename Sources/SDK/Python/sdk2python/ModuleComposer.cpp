/*
 * ModuleComposer.cpp: ModuleComposer class implementation
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


#include "ModuleComposer.h"
#include "CustomFunctions.h"
#include "ModuleTemplates.h"


ModuleComposer::ModuleComposer(const SDKParser& sdk_parser, const QString& qsOutputDir)
: Composer(sdk_parser, qsOutputDir)
{
}

bool ModuleComposer::CompositeModule()
{
	QString content;

// Caption
	content += MDL_CAPTION;
// Includes
	content += MDL_INCLUDES;

// Check SDK library macro
	content += MDL_PRL_SDK_CHECK;
// SDK library path
	content += MDL_SDK_LIBRARY_PATH;
	content += MDL_PRL_EVENT_HANDLER_ARGS;
	content += MDL_SET_SDK_LIBRARY_PATH;
	content += MDL_GET_SDK_LIBRARY_PATH;
// Initialization SDK
	content += MDL_IS_SDK_INITIALIZED;
	content += MDL_INITIALIZE_SDK;
	content += MDL_INITIALIZE_SDK_EX;
	content += MDL_DEINITIALIZE_SDK;
// SDK-Python functions
	foreach(QString qsFunc, m_sdk_parser.GetAllFunctions())
	{
		SdkPythonFunction(content, qsFunc);
	}
// Read CustomFunctions file:
        QFile customFunctionsFile(CUSTOM_FUNCS_CPP);
        if (!customFunctionsFile.open(QFile::ReadOnly | QFile::Text)){ 
                return false;
        }
        QTextStream in(&customFunctionsFile);
        content += in.readAll();
        customFunctionsFile.close();

// Sdk SDK-Python bundle
	content += MDL_SDK_PYTHON_BUNDLE;
// Extention
	content += QString(MDL_BIND_SDK_FUNCTION)
				.arg(MDL_SET_SDK_LIBRARY_PATH_NAME,
					 MDL_SET_SDK_LIBRARY_PATH_NAME,
					 MDL_METH_VARARGS,
					 m_sdk_parser.GetShortComment(MDL_SET_SDK_LIBRARY_PATH_NAME));
	content += QString(MDL_BIND_SDK_FUNCTION)
				.arg(MDL_GET_SDK_LIBRARY_PATH_NAME,
					 MDL_GET_SDK_LIBRARY_PATH_NAME,
					 MDL_METH_NOARGS,
					 m_sdk_parser.GetShortComment(MDL_GET_SDK_LIBRARY_PATH_NAME));
	content += QString(MDL_BIND_SDK_FUNCTION)
				.arg(MDL_IS_SDK_INITIALIZED_NAME,
					 MDL_IS_SDK_INITIALIZED_NAME,
					 MDL_METH_NOARGS,
					 m_sdk_parser.GetShortComment(MDL_IS_SDK_INITIALIZED_NAME));
	content += QString(MDL_BIND_SDK_FUNCTION)
				.arg(MDL_INITIALIZE_SDK_NAME,
					 MDL_INITIALIZE_SDK_NAME,
					 MDL_METH_VARARGS,
					 m_sdk_parser.GetShortComment(MDL_INITIALIZE_SDK_NAME));
	content += QString(MDL_BIND_SDK_FUNCTION)
				.arg(MDL_INITIALIZE_SDK_NAME_EX,
					 MDL_INITIALIZE_SDK_NAME_EX,
					 MDL_METH_VARARGS,
					 m_sdk_parser.GetShortComment(MDL_INITIALIZE_SDK_NAME_EX));
	content += QString(MDL_BIND_SDK_FUNCTION)
				.arg(MDL_DEINITIALIZE_SDK_NAME,
					 MDL_DEINITIALIZE_SDK_NAME,
					 MDL_METH_NOARGS,
					 m_sdk_parser.GetShortComment(MDL_DEINITIALIZE_SDK_NAME));
// Bind SDK all implemented functions
	foreach(QString qsFunc, m_sdk_parser.GetAllFunctions())
	{
// Bind SDK function
		if (m_sdk_parser.IsFunctionImplement(qsFunc))
		{
			content += QString(MDL_BIND_SDK_FUNCTION)
				.arg(qsFunc, qsFunc)
				.arg(m_sdk_parser.HasFunctionInArguments(qsFunc) ? MDL_METH_VARARGS : MDL_METH_NOARGS)
				.arg(m_sdk_parser.GetShortComment(qsFunc));
		}
	}
// End SDK-Python bundle
	content += MDL_END_SDK_PYTHON_BUNDLE;

// Entry function
	content += MDL_ENTRY_FUNCTION;
// Init module
	content += MDL_INIT_MODULE;
// Init GIL
    content += MDL_INIT_GIL;
// Register constants
	RegisterConstants(content);
// End entry function
        content += MDL_END_ENTRY_FUNCTION;
	if ( ! Save(MDL_PRL_PYTHON_SDK_CPP, content) )
		return false;

	return true;
}

void ModuleComposer::SdkPythonFunction(QString& content, const QString& qsFunc)
{
	if ( ! m_sdk_parser.IsFunctionImplement(qsFunc) )
		return;

	if ( getCustomFunctions().contains(qsFunc) )
		return;

	QString qsGenFunc;

	qsGenFunc = QString(MDL_SDK_PYTHON_FUNCTION)
				.arg(qsFunc)
				.arg(m_sdk_parser.HasFunctionInArguments(qsFunc) ? MDL_PY_ARGS : "");

	QString qsFuncArgs;
	QString qsInVars;
	QString qsInFormat;
	QString qsInValues;
	QString qsOutVars;
	QString qsOutStrVars;
	QString qsOutFormat;
	QString qsOutValues = MDL_NEW_RETURN_LIST;

	bool bAsync = m_sdk_parser.IsFunctionAsync(qsFunc);
	if (bAsync)
		qsOutValues += QString(MDL_PY_LIST_APPEND_PY_BUILD_VALUE)
							.arg("k", "hResultJob");
	else
		qsOutValues += QString(MDL_PY_LIST_APPEND_PY_BUILD_VALUE)
							.arg("k", "prlResult");

	QList<SDKParser::Arg > lstArgs = m_sdk_parser.GetFunctionArguments(qsFunc);
	bool bWasOutStrArg = false;
	for(int i = 0; i < lstArgs.size(); ++i)
	{
		SDKParser::Arg arg = lstArgs[i];

		if (arg.in)
		{
			bool bEnumType = m_sdk_parser.IsEnumerationType(arg.type);
			QString qsType = bEnumType ? "PRL_INT32" : arg.type;

			if (   (i + 1) < lstArgs.size()
				&& arg.type == "PRL_EVENT_HANDLER_PTR"
				&& lstArgs[i + 1].type == "PRL_VOID_PTR")
			{
				qsFuncArgs += ( qsFuncArgs.isEmpty() ? "0, 0" : ", 0, 0" );
				++i;
			}
			else
			{
				qsInVars += QString(MDL_DEF_VAR).arg(qsType, arg.name, qsType);
				qsInFormat += m_sdk_parser.GetTypeFormat(qsType);
				qsInValues += QString(MDL_IN_VALUE).arg(arg.name);

				qsFuncArgs += ( qsFuncArgs.isEmpty() ? "" : ", " )
					+ (bEnumType ? QString("(%1 )").arg(arg.type) : QString())
					+ arg.name;
			}
		}
		else if (arg.out)
		{
			qsOutFormat = m_sdk_parser.GetTypeFormat(arg.out_type);
			if ( ! bWasOutStrArg )
			{
				qsOutValues += QString(MDL_PY_LIST_APPEND_PY_BUILD_VALUE)
								.arg(qsOutFormat, arg.out_name);
			}

			qsFuncArgs += ( qsFuncArgs.isEmpty() ? "" : ", " )
						+ (arg.type == arg.out_type ? arg.out_name : "&" + arg.out_name);

			qsOutVars += QString(MDL_DEF_VAR).arg(arg.out_type, arg.out_name, arg.out_type);
			if ( ! bAsync
				&& m_sdk_parser.IsStringType(arg.out_type)
				&& (i + 1) < lstArgs.size()
				)
			{
				qsOutStrVars += QString(MDL_STR_DEF_VAR)
							.arg(arg.out_name, lstArgs[i + 1].out_name)
							.arg(arg.out_name, arg.out_name);

				bWasOutStrArg = true;
			}
		}
	}

	if ( ! qsInVars.isEmpty() )
	{
		qsInFormat += (":" + qsFunc);

		qsGenFunc += qsInVars + "\n";
		qsGenFunc += QString(MDL_PY_ARG_PARSE_TUPLE).arg(qsInFormat, qsInValues);
	}

	if ( ! qsOutVars.isEmpty() )
	{
		qsGenFunc += qsOutVars + "\n";
	}

	QString qsResultVar(bAsync ? "hResultJob" : "prlResult");
	qsGenFunc += QString(MDL_DECLARE_SDK_RESULT)
		.arg((bAsync ? "PRL_HANDLE " : "PRL_RESULT "), qsResultVar);
	qsGenFunc += QString(MDL_CALL_SDK_FUNCTION)
		.arg(qsResultVar, qsFunc, qsFuncArgs);

	if ( ! qsOutStrVars.isEmpty() )
	{
		qsGenFunc += qsOutStrVars + "\n";
		qsGenFunc += QString(MDL_CALL_SDK_FUNCTION)
			.arg("prlResult", qsFunc, qsFuncArgs);
	}

	qsGenFunc += qsOutValues;

	qsGenFunc += MDL_END_SDK_PYTHON_FUNCTION;

	content += qsGenFunc;
}

void ModuleComposer::RegisterConstants(QString& content)
{
// Constants module
	content += MDL_INIT_CONSTS_MODULE;
// Register enums
	foreach(QString qsConst, m_sdk_parser.GetEnums())
	{
		// workaround for
		// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1719.pdf
		// paragraph 2.2.2
		if (qsConst.startsWith("PJS_"))
		{
			content += QString(MDL_REGISTER_SIGNED_INT_CONSTANT).arg(qsConst, qsConst);
			continue;
		}

		content += QString(MDL_REGISTER_CONSTANT).arg(qsConst, qsConst);
		if (qsConst == "PKE_RELEASE")
			content += QString(MDL_REGISTER_CONSTANT).arg(MDL_PKE_CLICK, "0xFF");
	}
	content += "\n";

// Register Sessions
	foreach(QString qsSession, m_sdk_parser.GetSessionNames())
	{
		content += QString(MDL_REGISTER_STRING_CONSTANT).
				arg(qsSession, m_sdk_parser.GetSessionValue(qsSession));
	}
	content += "\n";

// Register OS versions
	foreach(QString qsConst, m_sdk_parser.GetOsVersions())
	{
		content += QString(MDL_REGISTER_CONSTANT).arg(qsConst, qsConst);
	}
	content += "\n";

// Register scan codes
	content += MDL_INIT_SCAN_CODES;
	foreach(QString qsName, m_sdk_parser.GetScanCodeNames())
	{
		QString qsValue = m_sdk_parser.GetScanCodeValue(qsName);
		QString qsShortName = m_sdk_parser.GetShortScanCodeName(qsName);

		content += QString(MDL_ADD_SCAN_CODE)
					.arg(qsShortName)
					.arg(m_sdk_parser.GetScanCodeFormat(qsValue))
					.arg(m_sdk_parser.GetScanCodeArgs(qsValue));
		if (qsShortName.at(0).isNumber())
		{
			content += QString(MDL_ADD_SCAN_CODE)
						.arg("NUM_" + qsShortName)
						.arg(m_sdk_parser.GetScanCodeFormat(qsValue))
						.arg(m_sdk_parser.GetScanCodeArgs(qsValue));
		}
	}
	content += "\n";

// Errors module
	content += MDL_INIT_ERRORS_MODULE;
// Register errors
	foreach(QString qsConst, m_sdk_parser.GetErrors())
	{
		content += QString(MDL_REGISTER_CONSTANT).arg(qsConst, qsConst);
	}
}
