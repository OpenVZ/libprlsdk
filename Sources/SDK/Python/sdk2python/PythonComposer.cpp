/*
 * PythonComposer.cpp: PythonComposer class implementation
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


#include "ModuleTemplates.h"
#include "PythonComposer.h"
#include "PythonTemplates.h"

#define DEV_TYPE_OTHERS \
	"PHT_VIRTUAL_DEV_FLOPPY," \
	"PHT_VIRTUAL_DEV_PARALLEL_PORT," \
	"PHT_VIRTUAL_DEV_OPTICAL_DISK," \
	"PHT_VIRTUAL_DEV_GENERIC_PCI," \
	"PHT_VIRTUAL_DEV_GENERIC_SCSI"

// Registration class table

PythonComposer::CustomClassInfo PythonComposer::s_CustomClassInfo[] =
{
//1              2               3             4         5          6
//class id       class name      base class    option    class type class factory
{"Api",          "ApiHelper",    "",           "helper", "", "",0,0,0,0},
{"Dbg",          "Debug",        "",           "helper", "", "",0,0,0,0},
{"Handle",       "_Handle",      "object",     "top",    "", "",0,0,0,0},
{"StrList",      "StringList",   "_Handle",    "iter: get_items_count, get_item","PHT_STRINGS_LIST", \
												"call_sdk_function('PrlApi_CreateStringsList')",0,0,0,0},
{"HndlList",     "HandleList",   "_Handle",    "iter: get_items_count, get_item","PHT_HANDLES_LIST", \
												"call_sdk_function('PrlApi_CreateHandlesList')",0,0,0,0},
{"OpTypeList",   "",	         "_Handle",    "iter: get_items_count, get_item","PHT_OPAQUE_TYPE_LIST", \
												"call_sdk_function('PrlApi_CreateOpTypeList')",0,0,0,0},
{"Result",       "",             "_Handle",    "iter: get_params_count, get_param_by_index","PHT_RESULT", "",0,0,0,0},
{"Event",        "",             "_Handle",    "iter: get_params_count, get_param","PHT_EVENT", "",0,0,0,0},
{"EvtPrm",       "EventParam",   "_Handle",    "",       "PHT_EVENT_PARAMETER", "",0,0,0,0},
{"Job",          "",             "_Handle",    "",       "PHT_JOB", "",0,0,0,0},
{"Srv",          "Server",       "_Handle",    "",       "PHT_SERVER", \
												"call_sdk_function('PrlSrv_Create')",0,0,0,0},
{"FsInfo",       "",             "_Handle",    "",       "PHT_REMOTE_FILESYSTEM_INFO", "",0,0,0,0},
{"FsEntry",      "",             "_Handle",    "",       "PHT_REMOTE_FILESYSTEM_ENTRY", "",0,0,0,0},
{"SrvCfg",       "ServerConfig", "_Handle",    "",       "PHT_SERVER_CONFIG", "",0,0,0,0},
{"SrvCfgDev",    "HostDevice",   "_Handle",    "",       "PHT_HW_GENERIC_DEVICE", "",0,0,0,0},
{"SrvCfgHdd",    "HostHardDisk", "HostDevice", "",       "PHT_HW_HARD_DISK", "",0,0,0,0},
{"SrvCfgHddPart","HdPartition",  "_Handle",    "",       "PHT_HW_HARD_DISK_PARTITION", "",0,0,0,0},
{"SrvCfgNet",    "HostNet",      "HostDevice", "",       "PHT_HW_NET_ADAPTER", "",0,0,0,0},
{"SrvCfgPci",    "HostPciDevice","HostDevice", "",       "PHT_HW_GENERIC_PCI_DEVICE", "",0,0,0,0},
{"UsrCfg",       "UserConfig",   "_Handle",    "",       "PHT_USER_PROFILE", "",0,0,0,0},
{"UsrInfo",      "UserInfo",     "_Handle",    "",       "PHT_USER_INFO", "",0,0,0,0},
{"DispCfg",      "DispConfig",   "_Handle",    "",       "PHT_DISP_CONFIG", "",0,0,0,0},
{"VirtNet",      "VirtualNet",   "_Handle",    "",       "PHT_VIRTUAL_NETWORK", \
												"call_sdk_function('PrlVirtNet_Create')",0,0,0,0},
{"PortFwd",      "PortForward",  "_Handle",    "",       "PHT_PORT_FORWARDING", \
												"call_sdk_function('PrlPortFwd_Create')",0,0,0,0},
{"VmDev",        "VmDevice",     "_Handle",    "",       DEV_TYPE_OTHERS, "",0,0,0,0},
{"VmDevHd",      "VmHardDisk",   "VmDevice",   "",       "PHT_VIRTUAL_DEV_HARD_DISK", "",0,0,0,0},
{"VmDevHdPart",  "VmHdPartition","_Handle",    "",       "PHT_VIRTUAL_DEV_HD_PARTITION", "",0,0,0,0},
{"VmDevHdEncryption","VmHdEncryption","_Handle","",      "PHT_VIRTUAL_DISK_ENCRYPTION", "",0,0,0,0},
{"VmDevNet",     "VmNet",        "VmDevice",   "",       "PHT_VIRTUAL_DEV_NET_ADAPTER", "",0,0,0,0},
{"VmDevUsb",     "VmUsb",        "VmDevice",   "",       "PHT_VIRTUAL_DEV_USB_DEVICE", "",0,0,0,0},
{"VmDevSound",   "VmSound",      "VmDevice",   "",       "PHT_VIRTUAL_DEV_SOUND", "",0,0,0,0},
{"VmDevSerial",  "VmSerial",     "VmDevice",   "",       "PHT_VIRTUAL_DEV_SERIAL_PORT", "",0,0,0,0},
{"VmCfg",        "VmConfig",     "_Handle",    "",       "", "",0,0,0,0},
{"Vm",           "",             "VmConfig",   "",       "PHT_VIRTUAL_MACHINE", "",0,0,0,0},
{"VmGuest",      "",             "_Handle",    "",       "PHT_VM_GUEST_SESSION", "",0,0,0,0},
{"Share",        "",             "_Handle",    "",       "PHT_SHARE", "",0,0,0,0},
{"BootDev",      "BootDevice",   "_Handle",    "",       "PHT_BOOT_DEVICE", "",0,0,0,0},
{"VmInfo",       "",             "_Handle",    "",       "PHT_VM_INFO", "",0,0,0,0},
{"FoundVmInfo",  "",             "_Handle",    "",       "PHT_FOUND_VM_INFO", "",0,0,0,0},
{"Acl",          "AccessRights", "_Handle",    "",       "PHT_ACCESS_RIGHTS", "",0,0,0,0},
{"VmToolsInfo",  "",             "_Handle",    "",       "PHT_VM_TOOLS_INFO", "",0,0,0,0},
{"Stat",         "Statistics",   "_Handle",    "",       "PHT_SYSTEM_STATISTICS", "",0,0,0,0},
{"StatCpu",      "",             "_Handle",    "",       "PHT_SYSTEM_STATISTICS_CPU", "",0,0,0,0},
{"Lic",          "License",      "_Handle",    "",       "PHT_LICENSE", "",0,0,0,0},
{"SrvInfo",      "ServerInfo",   "_Handle",    "",       "PHT_SERVER_INFO", "",0,0,0,0},
{"NetSvc",       "NetService",   "_Handle",    "",       "PHT_NET_SERVICE_STATUS", "",0,0,0,0},
{"LoginResponse","",             "_Handle",    "",       "PHT_LOGIN_RESPONSE", "",0,0,0,0},
{"RunningTask",  "",             "_Handle",    "",       "PHT_RUNNING_TASK", "",0,0,0,0},
{"OsesMatrix",   "",             "_Handle",    "",       "PHT_GUEST_OSES_MATRIX", "",0,0,0,0},
{"Report",		 "ProblemReport","_Handle",    "",       "PHT_PROBLEM_REPORT",    "",0,0,0,0},
{"Appliance", "ApplianceConfig", "_Handle",    "",       "PHT_APPLIANCE_CONFIG", \
												"call_sdk_function('PrlAppliance_Create')",0,0,0,0},
{"OffmgmtService","OfflineManageService","_Handle",  "", "PHT_OFFLINE_SERVICE", "",0,0,0,0},
{"NetworkShapingEntry", "",      "_Handle",    "",       "PHT_NETWORK_SHAPING", "",0,0,0,0},
{"NetworkShapingConfig", "",     "_Handle",    "",       "PHT_NETWORK_SHAPING_CONFIG", "",0,0,0,0},
{"NetworkClass", "",             "_Handle",    "",       "PHT_NETWORK_CLASS", "",0,0,0,0},
{"NetworkRate",  "",             "_Handle",    "",       "PHT_NETWORK_RATE", "",0,0,0,0},
{"CtTemplate",   "",             "_Handle",    "",       "PHT_CT_TEMPLATE",  "",0,0,0,0},
{"UsbIdent", "UsbIdentity",      "_Handle",    "",       "PHT_USB_IDENTITY", "", 0,0,0,0},
{"FirewallRule", "",             "_Handle",    "",       "PHT_FIREWALL_RULE", \
												"call_sdk_function('PrlFirewallRule_Create')",0,0,0,0},
{"IPPrivNet",    "",             "_Handle",    "",       "PHT_IPPRIV_NET", "",0,0,0,0},
{"PluginInfo",   "",             "_Handle",    "",       "PHT_PLUGIN_INFO","",0,0,0,0},
{"BackupResult", "",             "_Handle",    "",       "PHT_BACKUP_RESULT", "",0,0,0,0},
{"NetworkShapingBandwidthEntry","","_Handle",  "",       "PHT_NETWORK_SHAPING_BANDWIDTH", "",0,0,0,0},
{"CpuFeatures",  "",             "_Handle",    "",       "PHT_CPU_FEATURES",  \
			"call_sdk_function('PrlCpuFeatures_Create')",0,0,0,0},
{"CPUPool",      "",             "_Handle",    "",       "PHT_CPU_POOL",   "",0,0,0,0},
{"VmBackup",       "",             "_Handle",    "",       "PHT_BACKUP",  "",0,0,0,0},
{"DiskMap",      "",             "_Handle",    "",       "PHT_VIRTUAL_DISK_MAP",  "",0,0,0,0},
{"Disk",	     "",             "_Handle",    "",       "PHT_VIRTUAL_DISK",   "",0,0,0,0},
{"DiskMap",      "",             "_Handle",    "",       "PHT_VIRTUAL_DISK_MAP",   "",0,0,0,0},
{"VcmmdConfig",      "",             "_Handle",    "",       "PHT_VCMMD_CONFIG",   "",0,0,0,0},
};


PythonComposer::PythonComposer(const SDKParser& sdk_parser, const QString& qsOutputDir)
: Composer(sdk_parser, qsOutputDir)
{
	for(int i = 0; i < (int )( sizeof(s_CustomClassInfo)/sizeof(s_CustomClassInfo[0]) ); ++i)
	{
		QString qsClass = s_CustomClassInfo[i].class_id;
		m_lstClasses += qsClass;

		QString qsClassName = s_CustomClassInfo[i].class_name;
		if (qsClassName.isEmpty())
			qsClassName = qsClass;
		m_mapClassNames.insert(qsClass, qsClassName);

		m_mapBaseClasses.insert(qsClass, s_CustomClassInfo[i].base_class);
		m_mapClassOption.insert(qsClass, s_CustomClassInfo[i].class_option);
		m_mapClassTypes.insert(qsClass, s_CustomClassInfo[i].class_type);
		m_mapClassFactories.insert(qsClass, s_CustomClassInfo[i].class_factory);
	}
}

bool PythonComposer::CompositePython()
{
	QSet<QString > setUnusedClasses = m_sdk_parser.GetAllClasses().toSet()
										.subtract( m_lstClasses.toSet() );
	if ( ! setUnusedClasses.isEmpty() )
	{
		QStringList lstUnusedClasses = setUnusedClasses.toList();
		m_qsErrorMessage = QString("The class(es) %1 are unused!").arg(lstUnusedClasses.join(", "));
		return false;
	}

	QString content;

// Caption
	content += PTN_CAPTION;
// Import modules
	content += PTN_IMPORT_MODULES;
// SDK module
	content += PTN_SDK_MODULE;
// SDK error exception
	content += PTN_PRL_SDK_ERROR;
// Check SDK result
	content += PTN_SDK_CHECK_RESULT;
// Deinit SDK
	content += PTN_DEINIT_SDK;
// Call sync. sdk function
	content += PTN_CALL_SDK_FUNCTION;
// SDK initialization functions
	content += PTN_SET_SDK_LIBRARY_PATH;
	content += PTN_GET_SDK_LIBRARY_PATH;
	content += PTN_IS_SDK_INITIALIZED;
	content += PTN_INIT_DESKTOP_SDK;
	content += PTN_INIT_DESKTOP_WL_SDK;
	content += PTN_INIT_WORKSTATION_SDK;
	content += PTN_INIT_PLAYER_SDK;
	content += PTN_INIT_SERVER_SDK;
// Convert handle argument to handle
	content += PTN_CONV_HANDLE_ARG;
// Pre-define handle to object
	content += PTN_HANDLE_TO_OBJECT PTN_PASS;

	QString qsHandleToObject = PTN_HANDLE_TO_OBJECT PTN_HANDLE_TO_OBJECT_INIT;

// Python classes
	foreach(QString qsClass, m_lstClasses)
	{
// Add class
		if ( ! AddClass(content, qsClass, qsHandleToObject) )
			return false;
	}
	qsHandleToObject += PTN_END_HANDLE_TO_OBJECT;

// VmIO class
	content += PTN_PRL_IO_DISPLAY_SCREEN_SIZE;
	content += PTN_VM_IO_SEND_KEY_EVENT
			   PTN_VM_IO_SEND_KEY_EVENT_EX
			   PTN_VM_IO_DISPLAY_SET_CONFIGURATION;

// Handle to object
	content += qsHandleToObject;

	if ( ! Save(PTN_PRL_PYTHON_SDK_PY, content) )
		return false;

	return true;
}

bool PythonComposer::AddClass(QString& content, const QString& qsClass, QString& qsHandleToObject)
{
	if ( m_mapClassOption.value(qsClass) == "dropped" )
		return true;

	QString qsClassName = qsClass;
	if ( ! m_mapClassNames.value(qsClass).isEmpty() )
		qsClassName = m_mapClassNames.value(qsClass);

	QString qsBaseClasses;
	if ( ! m_mapBaseClasses.value(qsClass).isEmpty() )
		qsBaseClasses = "(" + m_mapBaseClasses.value(qsClass) + ")";

// Class definition
	content += QString(PTN_CLASS).arg(qsClassName, qsBaseClasses);

	QString qsComment = m_sdk_parser.GetLongComment(qsClassName);
	if ( ! qsComment.isEmpty() )
	{
		qsComment = "\t\"\"\"\n\t" + qsComment.replace("\n", "\n\t") + "\n\t\"\"\"\n";
		content += qsComment;
	}
	content += "\n";

// Constructor
	if ( m_mapClassOption.value(qsClass) == "top" )
	{
		content += QString(PTN_TOP_CLASS_INIT).arg(qsClassName, qsClassName);
	}
	else if (qsClassName.startsWith("_"))
	{
		content += QString(PTN_PRIVATE_CLASS_INIT).arg(qsClassName, qsClassName);
	}
	else
	{
		QString qsFactory = m_mapClassFactories.value(qsClass);
		if ( ! qsFactory.isEmpty() )
		{
			content += QString(PTN_CLASS_INIT).arg(qsFactory);
		}
	}

	if ( m_sdk_parser.GetClassFunctions(qsClass).isEmpty() )
	{
		m_qsErrorMessage = QString("The class %1 has no one function in SDK !").arg(qsClass);
		return false;
	}

	QString qsCheckContent = content;
	foreach(QString qsFunc, m_sdk_parser.GetClassFunctions(qsClass))
	{
		if ( ! AddMethod(content, qsFunc, m_mapClassOption.value(qsClass) != "helper") )
			return false;
	}

	if ( qsCheckContent == content )
	{
		m_qsErrorMessage = QString("No one method was added to the class %1 !"
									" It has to be dropped.")
							.arg(qsClass);
		return false;
	}

	if ( m_mapClassOption.value(qsClass) == "top" )
	{
// Get handle type
		content += PTN_TOP_GET_HANDLE_TYPE;
// Destructor
		content += PTN_TOP_CLASS_DEL;
	}

	if ( qsClass == "OpTypeList" )
	{
		content += PTN_OP_TYPE_LIST_SIGNED;
		content += PTN_OP_TYPE_LIST_UNSIGNED;
		content += PTN_OP_TYPE_LIST_DOUBLE;
	}

	QStringList lstOption = m_mapClassOption.value(qsClass).split(":");
	if (lstOption.size() == 2 && lstOption[0] == "iter")
	{
		QStringList lstIterFuncs = lstOption[1].split(",");
		if (lstIterFuncs.size() >= 2)
		{
			content += QString(PTN_ITER_METHODS)
						.arg(lstIterFuncs[0].trimmed(),
							 lstIterFuncs[1].trimmed());
		}
	}

// Handle to object by type

	QString qsType = m_mapClassTypes.value(qsClass);
	if ( ! qsType.isEmpty() )
	{
		QString qsCondition = qsType
								.split(",")
								.join(" or ")
								.replace("PHT_", PTN_HANDLE_TO_OBJECT_CHECK_TYPE);
		qsHandleToObject += QString(PTN_HANDLE_TO_OBJECT_BY_TYPE)
							.arg(qsCondition, qsClassName);
	}

	return true;
}

#define IF_FUNC_IS(func_name) \
	if ( ! m_sdk_parser.IsFunctionImplement(QString(func_name)) ) \
	{ \
		m_qsErrorMessage = QString("The %1 function is not for implementation or not found!") \
							.arg(func_name); \
		return false; \
	} \
	if ( qsFunc == func_name )

bool PythonComposer::AddMethod(QString& content, const QString& qsFunc, bool bHasOwnHandle)
{
	if (qsFunc == MDL_PRL_DEV_DISPLAY_SET_CONFIGURATION)
		return true;

	QString qsMethArgs;
	QString qsFuncArgs;

	bool bOutArgHandle = false;
	bool bOutArgBoolean = false;
	int nOutArgCount = 0;
	QList<SDKParser::Arg > lstArgs = m_sdk_parser.GetFunctionArguments(qsFunc);
	for(int i = 0; i < lstArgs.size(); ++i)
	{
		SDKParser::Arg arg = lstArgs[i];
		if (arg.in)
		{
			// Skip special types
			if (  (arg.type == "PRL_EVENT_HANDLER_PTR"
				|| arg.type == "PRL_VOID_PTR")
				&& !qsFunc.contains("eventhandler", Qt::CaseInsensitive))
			{
				continue;
			}

			QString qsFuncArg = arg.name;
			QString qsMethArg = arg.name;
			if ( ! AddArgDefaultValue(qsFunc, i, qsMethArg) )
				return false;

			if (m_sdk_parser.IsHandleType(arg.type))
			{
				if ( ! i && bHasOwnHandle )
				{
					qsFuncArgs += PTN_SELF_HANDLE;
					continue;
				}

				qsFuncArg = QString(PTN_ARG_TO_HANDLE).arg(qsFuncArg);
			}
			else if (arg.type == "PRL_CONST_GET_VM_CONFIG_PARAM_DATA_PTR")
			{
				qsFuncArg = PTN_GET_VM_CONFIG_PARAMS;
				qsMethArg = PTN_GET_VM_CONFIG_PARAMS;
			}
			else if (arg.type == "PRL_CONST_MEMGUARANTEE_DATA_PTR")
			{
				qsFuncArg = PTN_MEMGUARANTEE_SIZE_PARAMS;
				qsMethArg = PTN_MEMGUARANTEE_SIZE_PARAMS;
			}
			else if (arg.type == "PRL_CONST_CPULIMIT_DATA_PTR")
			{
				qsFuncArg = PTN_CPULIMIT_EX_PARAMS;
				qsMethArg = PTN_CPULIMIT_EX_PARAMS;
			}
			else if (arg.type == "PRL_EVENT_HANDLER_PTR")
            {   
                qsFuncArg = PTN_EVENT_HANDLER_PARAMS;
                qsMethArg = PTN_EVENT_HANDLER_PARAMS;
            }
            else if (arg.type == "PRL_IOLIMIT_DATA_PTR" || arg.type == "PRL_CONST_IOLIMIT_DATA_PTR")
            {
                qsFuncArg = PTN_IOLIMIT_DATA_PARAMS;
                qsMethArg = PTN_IOLIMIT_DATA_PARAMS;
            }

			qsMethArgs += ", " + qsMethArg;
			qsFuncArgs += (qsFuncArgs.isEmpty() ? "" : ", ") + qsFuncArg;
		}

		if (arg.out)
			nOutArgCount++;

		if ( ! bOutArgHandle && arg.out && m_sdk_parser.IsHandleType(arg.out_type) )
		{
			bOutArgHandle = true;
		}

		if ( ! bOutArgBoolean && arg.out && m_sdk_parser.IsBooleanType(arg.out_type) )
		{
			bOutArgBoolean = true;
		}
	}

	bOutArgHandle = nOutArgCount == 1 ? bOutArgHandle : false;
	bOutArgBoolean = nOutArgCount == 1 ? bOutArgBoolean : false;

	QString qsMethodPattern;
	if (m_sdk_parser.IsFunctionAsync(qsFunc))
		qsMethodPattern = PTN_ASYNC_CLASS_METHOD;
	else
	{
		qsFuncArgs = (qsFuncArgs.isEmpty() ? "" : ", ") + qsFuncArgs;

		IF_FUNC_IS( "PrlJob_Wait" )
			qsMethodPattern = PTN_JOB_WAIT_METHOD;
		else if (m_sdk_parser.HasFunctionOutArguments(qsFunc))
		{
			if (bOutArgHandle)
				qsMethodPattern = PTN_GET_OBJECT_CLASS_METHOD;
			else if (bOutArgBoolean)
				qsMethodPattern = PTN_GET_BOOLEAN_CLASS_METHOD;
			else
				qsMethodPattern = PTN_GET_CLASS_METHOD;
		}
		else
			qsMethodPattern = PTN_SET_CLASS_METHOD;
	}

	QString qsComment = m_sdk_parser.GetLongComment(qsFunc);
	if ( ! qsComment.isEmpty() )
	{
		qsComment = "\t\t\"\"\"\n\t\t" + qsComment.replace("\n", "\n\t\t") + "\n\t\t\"\"\"\n";
	}

	content += qsMethodPattern
				.arg(m_sdk_parser.GetClassMethod(qsFunc),
					 qsMethArgs,
					 qsComment,
					 qsFunc,
					 qsFuncArgs);

	return true;
}

bool PythonComposer::AddArgDefaultValue(const QString& qsFunc, int nArgIndex, QString& qsMethArg)
{
	IF_FUNC_IS( "PrlApi_InitEx" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = 0";	// reserved param 1
		if (nArgIndex == 3)
			qsMethArg += " = 0";	// reserved param 2
	}

	IF_FUNC_IS( "PrlApi_GetResultDescription" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = True";	// brief message
		if (nArgIndex == 2)
			qsMethArg += " = False";	// formated message
	}

	IF_FUNC_IS( "PrlSrv_LoginLocal" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = ''";	// previous session ID
		if (nArgIndex == 2)
			qsMethArg += " = 0";	// port number
		if (nArgIndex == 3)
			qsMethArg += " = consts.PSL_HIGH_SECURITY";	// security level
	}

	IF_FUNC_IS( "PrlSrv_Login" )
	{
		if (nArgIndex == 4)
			qsMethArg += " = ''";	// previous session ID
		if (nArgIndex == 5)
			qsMethArg += " = 0";	// port number
		if (nArgIndex == 6)
			qsMethArg += " = 0";	// timeout
		if (nArgIndex == 7)
			qsMethArg += " = consts.PSL_HIGH_SECURITY";	// security level
	}

	IF_FUNC_IS( "PrlSrv_SetNonInteractiveSession" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlSrv_EnableConfirmationMode" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlSrv_DisableConfirmationMode" )
	{
		if (nArgIndex == 3)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlJob_Wait" )
	{
		if (nArgIndex == 1)
#if (VZ_PY_VER < 3)
			qsMethArg += " = sys.maxint";	// timeout
#else
			qsMethArg += " = sys.maxsize";	// timeout
#endif
	}

	IF_FUNC_IS( "PrlSrv_RegisterVm" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = False";	// non interactive mode
	}

	IF_FUNC_IS( "PrlVm_Reg"  )
	{
		if (nArgIndex == 2)
			qsMethArg += " = False";	// non interactive mode
	}

	IF_FUNC_IS( "PrlVm_StartEx" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = consts.PSM_VM_START";	// start mode
		if (nArgIndex == 2)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlVm_Stop" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = False";	// ACPI
	}

	IF_FUNC_IS( "PrlVm_Pause" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = False";	// ACPI
	}

	IF_FUNC_IS( "PrlVm_Clone" )
	{
		if (nArgIndex == 3)
			qsMethArg += " = False";	// template
	}

	IF_FUNC_IS( "PrlVm_Delete" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = consts.PRL_INVALID_HANDLE";	// devices list
	}

	IF_FUNC_IS( "PrlSrv_FsGenerateEntryName" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = ''";	// filename prefix
		if (nArgIndex == 3)
			qsMethArg += " = ''";	// filename suffix
		if (nArgIndex == 4)
			qsMethArg += " = ''";	// index delimiter
	}

	IF_FUNC_IS( "PrlVm_GenerateVmDevFilename" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = ''";	// filename prefix
		if (nArgIndex == 2)
			qsMethArg += " = ''";	// filename suffix
		if (nArgIndex == 3)
			qsMethArg += " = ''";	// index delimiter
	}

	IF_FUNC_IS( "PrlSrv_StartSearchVms" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = consts.PRL_INVALID_HANDLE";	// list of pathnames to search
	}

	IF_FUNC_IS( "PrlSrv_CreateVmBackup" )
	{
		if (nArgIndex == 5)
			qsMethArg += " = ''";	// description
		if (nArgIndex == 6)
			qsMethArg += " = consts.PBT_FULL";	// backup flags
		if (nArgIndex == 7)
			qsMethArg += " = 0";	// reserved flags
		if (nArgIndex == 8)
			qsMethArg += " = True";	// non interactive mode
	}

	IF_FUNC_IS( "PrlSrv_RestoreVmBackup" )
	{
		if (nArgIndex == 6)
			qsMethArg += " = ''";	// target home path
		if (nArgIndex == 7)
			qsMethArg += " = ''";	// target VM name
		if (nArgIndex == 8)
			qsMethArg += " = consts.PVMSL_LOW_SECURITY";	// restore flags
		if (nArgIndex == 9)
			qsMethArg += " = 0";	// reserved flags
		if (nArgIndex == 10)
			qsMethArg += " = True";	// non interactive mode
	}

	IF_FUNC_IS( "PrlSrv_GetBackupTree" )
	{
		if (nArgIndex == 5)
			qsMethArg += " = consts.PBT_FULL";	// backup flags
		if (nArgIndex == 6)
			qsMethArg += " = 0";	// reserved flags
		if (nArgIndex == 7)
			qsMethArg += " = True";	// non interactive mode
	}

	IF_FUNC_IS( "PrlVm_Migrate" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = ''";	// target home path
		if (nArgIndex == 3)
			qsMethArg += " = consts.PVMSL_LOW_SECURITY";	// migration flags
		if (nArgIndex == 4)
			qsMethArg += " = 0";	// reserved flags
		if (nArgIndex == 5)
			qsMethArg += " = True";	// non interactive mode
	}

	IF_FUNC_IS( "PrlVm_MigrateEx" )
	{
		if (nArgIndex == 4)
			qsMethArg += " = ''";	// target home path
		if (nArgIndex == 5)
			qsMethArg += " = consts.PVMSL_LOW_SECURITY";	// migration flags
		if (nArgIndex == 6)
			qsMethArg += " = 0";	// reserved flags
		if (nArgIndex == 7)
			qsMethArg += " = True";	// non interactive mode
	}

	IF_FUNC_IS( "PrlSrv_GetVirtualNetworkList" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlSrv_AddVirtualNetwork" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlSrv_UpdateVirtualNetwork" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlSrv_DeleteVirtualNetwork" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlSrv_ConfigureGenericPci" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlSrv_Shutdown" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = False";	// force shutdown
	}

	IF_FUNC_IS( "PrlVmDev_CreateImage" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = False";	// recreate is allowed
		if (nArgIndex == 2)
			qsMethArg += " = True";		// non interactive mode
	}

	IF_FUNC_IS( "PrlVm_CreateSnapshot" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = ''";		// description
	}

	IF_FUNC_IS( "PrlVm_DeleteSnapshot" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = False";		// delete child snapshots
	}

	IF_FUNC_IS( "PrlVm_UpdateSnapshotData" )
	{
		if (nArgIndex == 3)
			qsMethArg += " = ''";		// description
	}

	IF_FUNC_IS( "PrlVm_LoginInGuest" )
	{
		if (nArgIndex == 3)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlVm_StartVncServer" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlVm_StopVncServer" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlVm_ValidateConfig" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = consts.PVC_ALL";	// section
	}

	IF_FUNC_IS( "PrlVm_AuthWithGuestSecurityDb" )
	{
		if (nArgIndex == 3)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlVmGuest_Logout" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlVmGuest_RunProgram" )
	{
		if (nArgIndex == 4)
			qsMethArg += " = consts.PRPM_RUN_PROGRAM_AND_RETURN_IMMEDIATELY";	// program execution flags
		if (nArgIndex == 5)
			qsMethArg += " = consts.PRL_INVALID_FILE_DESCRIPTOR";	// stdin
		if (nArgIndex == 6)
			qsMethArg += " = consts.PRL_INVALID_FILE_DESCRIPTOR";	// stdout
		if (nArgIndex == 7)
			qsMethArg += " = consts.PRL_INVALID_FILE_DESCRIPTOR";	// stderr
	}

	IF_FUNC_IS( "PrlVmGuest_GetNetworkSettings" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlVm_SetUserPasswd" )
	{
		if (nArgIndex == 3)
			qsMethArg += " = 0";	// reserved flags
	}

	IF_FUNC_IS( "PrlVm_Compact" )
	{
		if (nArgIndex == 2)
			qsMethArg += " = 0";	// flags
	}

	IF_FUNC_IS( "PrlVm_ResetUptime" )
	{
		if (nArgIndex == 1)
			qsMethArg += " = 0";	// flags
	}

	return true;
}
