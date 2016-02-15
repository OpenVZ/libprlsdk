/*
 * PrlErrStringsStorage.cpp
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


#include "PrlErrStringsStorage.h"
#include <prlxmlmodel/Messaging/CVmEvent.h>
#include "PrlHandleBase.h"
#include <QObject>
#include <QRegExp>

#ifdef ENABLE_MALLOC_DEBUG
	// By adding this interface we enable allocations tracing in the module
	#include "Interfaces/Debug.h"
#else
	// We're not allowed to throw exceptions from the library -
	// so we need to prevent operator ::new from doing this
	#include <new>
	using std::nothrow;
	#define new new(nothrow)
#endif


PrlErrStringsStorage *PrlErrStringsStorage::g_pErrStringsStorage = NULL;

QMutex *PrlErrStringsStorage::g_pErrorsStringsStorageMutex = new QMutex(QMutex::Recursive);

PrlErrStringsStorage::PrlErrStringsStorage()
{
	g_pErrStringsStorage = this;
	Initialize();
}

PrlErrStringsStorage *PrlErrStringsStorage::GetStorageInstance()
{
	QMutexLocker _lock(g_pErrorsStringsStorageMutex);
	if (!g_pErrStringsStorage)
		g_pErrStringsStorage = new PrlErrStringsStorage;
	return (g_pErrStringsStorage);
}

PrlErrStringData PrlErrStringsStorage::GetErrorStringData(PRL_RESULT nErrCode, bool &bMessageWasFound)
{
	bMessageWasFound = false;
	PrlErrStringsStorage *pStorage = GetStorageInstance();
	PrlErrStringsStorageMap::const_iterator _it;
	PRL_APPLICATION_MODE nAppMode = PrlGetApiAppMode();
	if (nAppMode == PAM_SERVER)
		_it = pStorage->m_PSErrorsStringsMap.find(nErrCode);
	else
		return PrlErrStringData();

	if (_it == pStorage->m_PSErrorsStringsMap.end()
	)//Product specific message wasn't found - try to get it from common map
		_it = pStorage->m_CommonErrorsStringsMap.find(nErrCode);

	if (_it != pStorage->m_CommonErrorsStringsMap.end() &&
		_it != pStorage->m_PSErrorsStringsMap.end())
	{
		bMessageWasFound = true;
		return (*_it);
	}
	return PrlErrStringData();
}

bool PrlErrStringsStorage::Contains(PRL_RESULT nErrCode)
{
	bool bResult = false;
	GetErrorStringData(nErrCode, bResult);
	return (bResult);
}

static const QRegExp br_regexp("< *br *>", Qt::CaseInsensitive) ;
static const QRegExp tag_regexp("< *([ba][^>]*>|/[ba] *>)", Qt::CaseInsensitive) ;

static bool useSingularMessage( int paramIndex, const CVmEvent* pParams)
{
	if( !pParams || paramIndex <=0 )
		return false;
	QString name = QString(EVT_PARAM_MESSAGE_PARAM_n).arg(paramIndex -1); // %1 ==> EVT_PARAM_MESSAGE_PARAM_0
	CVmEventParameter* pParam = pParams->getEventParameter(name);
	if( pParam && pParam->getParamValue().toInt() == 1 )
			return true;
	return false;
}

QString PrlErrStringsStorage::GetErrString(
	PRL_RESULT nErrCode, bool bIsBriefMessage, bool bFormated, const CVmEvent* pParams)
{
	bool bWasFound = false;
	PrlErrStringData data = GetErrorStringData(nErrCode, bWasFound);
	if (!bWasFound)
		return QString() ;

	QString sResult = bIsBriefMessage ? data.m_sBriefMessage : data.m_sLongMessage;
	if( pParams )
	{
		if( bIsBriefMessage
			&& data.m_nBriefParamNumberToCheckForSingular
			&& !data.m_sBriefSingularMessage.isEmpty()
			&& useSingularMessage( data.m_nBriefParamNumberToCheckForSingular, pParams ) )
		{
			sResult = data.m_sBriefSingularMessage;
		}
		else if( !bIsBriefMessage
			&& data.m_nLongParamNumberToCheckForSingular
			&& !data.m_sLongSingularMessage.isEmpty()
			&& useSingularMessage( data.m_nLongParamNumberToCheckForSingular, pParams ) )
		{
			sResult = data.m_sLongSingularMessage;
		}
	}

	sResult = sResult.replace("@PRL_PRODUCT_NAME", PrlGetApiProductName());
	sResult = sResult.replace("@PRL_TRASH_NAME", PrlGetTrashName());
	if (bFormated)
		return sResult;
	return  sResult.replace(br_regexp, "/n").replace(tag_regexp, "") ;
}

bool PrlErrStringsStorage::CanMessageBeIgnored(PRL_RESULT nErrCode)
{
	bool bWasFound = false;
	bool bResult = false;
	PrlErrStringData _err_string_data = GetErrorStringData(nErrCode, bWasFound);
	if (bWasFound)
		bResult = _err_string_data.m_bCanBeIgnored;
	return (bResult);
}

PRL_MESSAGE_TYPE_ENUM PrlErrStringsStorage::GetMessageType(PRL_RESULT nErrCode)
{
	bool bWasFound = false;
	PRL_MESSAGE_TYPE_ENUM nMessageType = PMT_WARNING;
	PrlErrStringData _err_string_data = GetErrorStringData(nErrCode, bWasFound);
	if (bWasFound)
		nMessageType = _err_string_data.m_nMessageType;
	return (nMessageType);
}

static void REGISTER_ERROR_STRING(PRL_RESULT error_code, const QString &brief_msg, const QString &long_msg, bool can_be_ignored_sign, PRL_MESSAGE_TYPE_ENUM message_type)
{
	PrlErrStringsStorage::GetStorageInstance()->m_CommonErrorsStringsMap.insert(error_code, PrlErrStringData(brief_msg, long_msg, can_be_ignored_sign, message_type));
}

static void REGISTER_ERROR_STRING2(PRL_RESULT error_code
	, const QString &brief_msg
	, int brief_check_singular_param, const QString &brief_singular_msg
	, const QString &long_msg
	, int long_check_singular_param, const QString &long_singular_msg
	, bool can_be_ignored_sign, PRL_MESSAGE_TYPE_ENUM message_type)
{
	PrlErrStringsStorage::GetStorageInstance()->m_CommonErrorsStringsMap.insert(error_code
		, PrlErrStringData(brief_msg
			, brief_check_singular_param, brief_singular_msg
			, long_msg
			, long_check_singular_param, long_singular_msg
			, can_be_ignored_sign, message_type));
}

void PrlErrStringsStorage::Initialize()
{
	InitializePSErrorMessages();

	REGISTER_ERROR_STRING(
		PET_ANSWER_OK,
		tr("OK"),
		tr("OK..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_CANCEL,
		tr("Cancel"),
		tr("Cancel..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_YES,
		tr("Yes"),
		tr("Yes..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_NO,
		tr("No"),
		tr("No..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_BREAK,
		tr("Break"),
		tr("Break..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_OVERIDE,
		tr("Override"),
		tr("Override..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_SHUTDOWN,
		tr("Shutdown"),
		tr("Shutdown..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_STOP,
		tr("Stop"),
		tr("Stop..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_CONTINUE,
		tr("Continue"),
		tr("Continue..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_RETRY,
		tr("Retry"),
		tr("Retry..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_CREATE_NEW,
		tr("Create new"),
		tr("Create new"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_USE_CURRENT,
		tr("Use current"),
		tr("Use current"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_APPEND,
		tr("Append"),
		tr("Append"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_REPLACE,
		tr("Replace"),
		tr("Replace"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_STARTVM,
		tr("Start the virtual machine"),
		tr("Start the virtual machine"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_CREATE,
		tr("Create"),
		tr("Create"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_SKIP,
		tr("Skip"),
		tr("Skip"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_SUSPEND_ANYWAY,
		tr("Suspend anyway"),
		tr("Suspend anyway..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_SUSPEND,
		tr("Suspend"),
		tr("Suspend..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_SUSPEND,
		tr("Suspend"),
		tr("Suspend..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_RESTART_NOW,
		tr("Restart Now"),
		tr("Restart Now..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_LATER,
		tr("Later"),
		tr("Later..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_COMPACT,
		tr("Compact"),
		tr("Compact..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_BROWSE,
		tr("Browse"),
		tr("Browse..."),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_CHANGE,
		tr("Change"),
		tr("Change"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_DONT_CHANGE,
		tr("Don't change"),
		tr("Don't change"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUCCESS,
		tr("Success."),
		tr("The operation was successfully completed."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNEXPECTED,
		tr("Unexpected error."),
		tr("An unexpected error occurred."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_OUT_OF_MEMORY,
		tr("Out of memory."),
		tr("Not enough memory to complete the operation execution."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_ARG,
		tr("Invalid argument."),
		tr("An invalid argument was passed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_OBJECT_WAS_REMOVED,
		tr("The object has been removed."),
		tr("The object to be processed has been already removed from the parent composite object."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_HANDLE,
		tr("Invalid handle."),
		tr("A handle of a wrong type was specified."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACCESS_DENIED,
		tr("The operation cannot be performed."),
		tr("You are not authorized to perform this operation."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BUFFER_OVERRUN,
		tr("Buffer overrun."),
		tr("The buffer size is not enough to store the action result."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNINITIALIZED,
		tr("Uninitialized."),
		tr("Uninitialized data requested."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNIMPLEMENTED,
		tr("Unimplemented."),
		tr("This feature is not implemented yet."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FAILURE,
		tr("Operation failed."),
		tr("Failed to execute the operation."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_OPERATION_FAILED,
		tr("Operation failed."),
		tr("Failed to execute the operation."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FAILURE_ON_VM_DESTROYED,
		tr("The operation failed because the virtual machine is being shut down."),
		tr("Failed to execute the operation."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILE_NOT_FOUND,
		tr("File not found."),
		tr("The specified file was not found."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DOUBLE_INIT,
		tr("Double initialization attempt."),
		tr("The SDK library has been already initialized."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SYMBOL_NOT_FOUND,
		tr("Symbol not found."),
		tr("Unable to find the requested library symbol."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PARSE_STATISTICS,
		tr("Error in parsing the system statistics."),
		tr("Failed to parse the system statistics information data."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_SAMPLE_1,
		tr("Just a simple question."),
		tr("This simple question for you to know the answer... What is your name? \nparam0:%1 \nparam1:%2 \nparam2:%3 \nparam0:%1"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_SAMPLE_2,
		tr("This is another question."),
		tr("This question has only one parameter '%1' in the message... Agree?"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_TXT_ERROR,
		tr("Parallels Warning Message."),
		tr("%1"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_RESTORE_VM_CONFIG_FROM_BACKUP,
		tr("Do you want to restore the configuration file of the '%1' virtual machine from backup?"),
		tr("The configuration file of this virtual machine was damaged due to some reason. You can restore it from a backup copy made earlier."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_UNABLE_OPEN_DEVICE,
		tr("Unable to connect %1 %2."),
		tr("A file or device required for the operation of %1 %2 does not exist or is used by another process, or you have no permission to access it. The virtual machine will continue running, but the device will be disconnected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_SHARED_BLOCK,
		tr("The virtual disk %1 %2 is corrupt. Please check it for errors."),
		tr("To check the disk, use the prl_disk_tool command line utility. For more information, see <a href=\"http://kb.parallels.com/en/113241\">http://kb.parallels.com/en/113241</a>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_HDD_ERROR,
		tr("Unable to access Hard Disk %1"),
		tr("An error occurred when accessing the hard disk. Error ID: %2."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_COMMON_HDD_ERROR,
		tr("An error occurred while accessing the Hard Disk %1 used by \"%2\"."),
		tr("Click <b>Retry</b> to try again or click <b>Stop</b> to forcibly stop the virtual machine. If this error persists, please contact the Parallels support team for assistance.<br><br><b>Note</b>: If you stop the virtual machine \"%2\" now, you will lose all unsaved data.<br><br><b>Error ID:</b> %3."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_FAT32_FILE_OVERRUN,
		tr("FAT32 Error."),
		tr("FAT32 File Overrun (%1)."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_UNKNOWN_HDD_REQUEST_TYPE,
		tr("Unable to perform the operation."),
		tr("The requested operation is not supported for this hard disk drive."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_TARGET_HOST_DISK_IS_FULL,
		tr("PRL_WARN_TARGET_HOST_DISK_IS_FULL"),
		tr("PRL_WARN_TARGET_HOST_DISK_IS_FULL"),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_FLOPPY_CONNECT_FAILED,
		tr("PRL_ERR_DEV_FLOPPY_CONNECT_FAILED"),
		tr("PRL_ERR_DEV_FLOPPY_CONNECT_FAILED %1"),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_ALREADY_REGISTERED_VM_PATH,
		tr("The virtual machine \"%1\" already exists in \"%2\"."),
		tr("Specify another name or location for the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_IMAGE_IS_STOLEN,
		tr("Unable to access the image file."),
		tr("Make sure that the path to the image file %1 is correct and the target location is accessible."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_UNABLE_OPEN_DEVICE_NAME,
		tr("Unable to connect the device."),
		tr("A file or device required for the %1 %2 operation does not exist or is used by another process, or you have insufficient access permissions. The virtual machine will continue running with the device disconnected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_ALREADY_CONNECTED,
		tr("A device connecting error occurred."),
		tr("The device %1 %2 is already connected."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_SERIAL_PORT_PIPE_CONNECT_FAILED,
		tr("Unable to connect the serial port %1. The connection status for this device is set to Disconnected."),
		tr("The socket required by the serial port %1 is used by another process or you do not have the permission to access it. The virtual machine will continue running, but the serial port will be disconnected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_SERIAL_PORT_PHYSICAL_CONNECT_FAILED,
		tr("Unable to connect the serial port %1. The connection status for this device is set to Disconnected."),
		tr("The socket required by the serial port %1 is used by another process or you do not have the permission to access it. The virtual machine will continue running, but the serial port will be disconnected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_DO_YOU_WANT_TO_OVERWRITE_FILE,
		tr("The output file already exists. Do you want to append data to this file or replace the file?"),
		tr("Click Append to continue using the existing output file for %1 %2. Click Replace to overwrite it."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_SERIAL_PORT_FILE_CONNECT_FAILED,
		tr("Unable to connect the serial port %1. The connection status for this device is set to Disconnected."),
		tr("The file required by the serial port %1 is used by another process or you do not have the permission to access it. The virtual machine will continue running, but the serial port will be disconnected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_PARALLEL_PORT_PRINTER_CONNECT_FAILED,
		tr("Unable to connect the printer port %1. The connection status for this device is set to Disconnected."),
		tr("The printer used by the printer port %1 is not available or you have insufficient privileges to access it. "\
		   "The virtual machine will continue running, but the printer port will be disconnected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_PARALLEL_PORT_PHYSICAL_CONNECT_FAILED,
		tr("Unable to connect the printer port %1. The connection status for this device is set to Disconnected."),
		tr("The physical resource used by the printer port %1 is used by another process or you have insufficient privileges to access it. "\
		   "The virtual machine will continue running, but the printer port will be disconnected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_PARALLEL_PORT_FILE_CONNECT_FAILED,
		tr("Unable to connect the printer port %1. The connection status for this device is set to Disconnected."),
		tr("The file used by the printer port %1 is used by another process or you have insufficient privileges to access it. "\
		   "The virtual machine will continue running, but the printer port will be disconnected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_NET_CABLE_DISCONNECTED,
		tr("The real network adapter <b>%2</b> used by the virtual adapter %1 is not connected to the network."),
		tr("Make sure the real adapter is connected to a wireless or wired network."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_NET_IPADDRESS_MODIFY_FAILED_WARNING,
		tr("The IP address of the Parallels network adapter has not been changed."),
		tr("There was a failure changing the IP address of the Parallels network adapter. The other network parameters have been modified successfully."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VMDEVICE_VIRTUAL_NETWORK_NOT_EXIST,
		tr("The virtual network does not exist."),
		tr("The \"%2\" virtual network does not exist. Network adapter %1 will be disconnected."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_ADAPTER_ALREADY_USED,
		tr("This network adapter is already in use."),
		tr("Please select another network adapter and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VMDEVICE_VIRTUAL_NETWORK_CONFIG_ERROR,
		tr("The virtual network configuration is invalid."),
		tr("The \"%2\" virtual network configuration is invalid. Network adapter %1 will be disconnected."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VMDEVICE_VIRTUAL_NETWORK_DISABLED,
		tr("The virtual network is disabled."),
		tr("The \"%2\" virtual network is disabled. Network adapter %1 will be disconnected."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_PKTFILTER_ERROR,
		tr("Unable to configure the packet filter."),
		tr("The packet filter for network adapter %1 could not be configured. The network adapter will be disconnected."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_DUPLICATE_VIRTUAL_NETWORK_ID,
		tr("A virtual network named %1 already exists."),
		tr("Specify another name for this virtual network."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VIRTUAL_NETWORK_ID_NOT_EXISTS,
		tr("The %1 virtual network does not exist."),
		tr("Specify another network."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VIRTUAL_NETWORK_NOT_FOUND,
		tr("The %1 virtual network cannot be found."),
		tr("Try to choose another network."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VIRTUAL_NETWORK_SHARED_EXISTS,
		tr("Shared network already exists on this server."),
		tr("You can have only one shared network on a single physical server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_OFFMGMT_ERROR,
		tr("Unable to configure the offline management."),
		tr("Offline management is not available for this virtual machine and cannot be configured."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_DUPLICATE_IPPRIVATE_NETWORK_NAME,
		tr("An IP private network named %1 already exists."),
		tr("Specify another name for this IP private network."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_IPPRIVATE_NETWORK_DOES_NOT_EXIST,
		tr("The %1 IP private network does not exist."),
		tr("Specify another network."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_IPPRIVATE_NETWORK_INVALID_IP,
		tr("The \"IP address/subnet mask\" \"%1\" assigned to IP private network %2 is invalid."),
		tr("Specify another network address."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_TOO_LOW_HDD_SIZE,
		tr("You are running out of free hard disk space."),
		tr("Free some disk space on the hard disk and click <b>Retry</b>.<br><br>Note: If you stop the virtual machine \"%1\" now, you will lose all unsaved data."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_CANT_CONNECT_VOLUME,
		tr("Failed to connect to the \"%1\" volume. Click Continue to try again."),
		tr("This virtual machine is stored on the \"%1\" volume. You need to connect to this volume to be able to start this virtual machine."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_APPLY_CONFIG_NEEDS_REBOOT,
		tr("The changes will take effect after you restart the virtual machine."),
		tr("To apply the changes, shut down your guest operating system and then start the virtual machine again." ),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CANT_DELETE_RUNNING,
		tr("Cannot delete the virtual machine because it is running."),
		tr("Stop the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CANT_CLONE_RUNNING,
		tr("Cannot clone the virtual machine because it is running."),
		tr("Stop the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_NAME_IS_EMPTY,
		tr("The virtual machine name is not specified."),
		tr("Specify a name for the virtual machine to proceed."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CANT_UNREG_RUNNING,
		tr("Cannot unregister the virtual machine because it is running."),
		tr("Stop the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PARSE_VM_CONFIG,
		tr("The configuration file you specified is invalid."),
		tr("Make sure that the specified file is a valid Parallels virtual machine configuration file and it is not corrupt, or select another file."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_XML_WRITE_FILE,
		tr("Failed to save the configuration file."),
		tr("The configuration file could not be saved on the hard disk. Make sure you are authorized to write to the disk."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CONFIG_WAS_CHANGED,
		tr("The virtual machine configuration cannot be saved due to a user activity conflict. Your changes will be discarded."),
		tr("Another user saved the configuration while you were editing it. Try to edit the configuration again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_PROFILE_WAS_CHANGED,
		tr("The user preferences cannot be saved due to a user activity conflict. Your changes will be discarded."),
		tr("Another user saved the preferences while you were editing them. Try to edit the preferences again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CONFIG_ALREADY_EXISTS,
		tr("A virtual machine with this name already exists."),
		tr("Specify another name for your virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_DIR_CONFIG_ALREADY_EXISTS,
		tr("A virtual machine folder with this name already exists."),
		tr("Specify another name for your virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOAD_BINARY_FAILED,
		tr("The operation cannot be completed because one or more binary files are missing."),
		tr("Reinstall the application and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMDIR_PATH_IS_NOT_ABSOLUTE,
		tr("The path you specified is not valid."),
		tr("Specify a valid virtual machine path and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_MAX_NUMBER_EXCEEDED,
		tr("The number of devices connected to this virtual machine has reached its maximum."),
		tr("You cannot add more devices to this virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_MAX_CD_HDD_EXCEEDED,
		tr("The number of CD/DVD devices and virtual hard disks connected to this type of interface has reached its maximum."),
		tr("You cannot add more devices to this type of interface."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SPECIFY_GUEST_INSTALL_FILES,
		tr("You need to specify the path to the installation source."),
		tr("Specify the path to the guest operating system installation disc or another source of installation and try again."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_DROP_SUSPENDED_STATE,
		tr("Unable to change the virtual machine state."),
		tr("Some files have not been deleted either because they were in use or because you do not have the permission to delete them. You may try to delete these files manually."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MUST_BE_STOPPED_BEFORE_RENAMING,
		tr("You cannot rename the virtual machine when it is running."),
		tr("Stop the virtual machine \"%1\" and try again."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CDD_IMAGE_NOT_SPECIFIED,
		tr("The virtual machine configuration is invalid."),
		tr("Specify a valid CD/DVD disc image and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_QUEST_HDD_IMAGE_NOT_EXIST,
		tr("The virtual machine configuration is invalid."),
		tr("The virtual hard disk image file \"%1\" is not found. Check the virtual machine configuration and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HDD_IMAGE_NOT_SPECIFIED,
		tr("The virtual machine configuration is invalid."),
		tr("Specify a valid hard disk image file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_QUEST_CDD_IMAGE_NOT_EXIST,
		tr("The virtual machine configuration is invalid."),
		tr("The virtual CD/DVD disc image file \"%1\" is not found. Check the virtual machine configuration and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FLOPPY_IMAGE_NOT_EXIST,
		tr("The virtual machine configuration is invalid."),
		tr("The virtual floppy disk image file \"%1\" is not found. Check the virtual machine configuration and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FDD_IMAGE_NOT_SPECIFIED,
		tr("The virtual machine configuration is invalid."),
		tr("Specify a valid floppy disk image file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FLOPPY_IMAGE_ALREADY_EXIST,
		tr("A floppy disk image file \"%1\" already exists."),
		tr("Do you want to replace the existing floppy disk image file \"%1\" with a new one?"),
		false,
		PMT_QUESTION
		);


	REGISTER_ERROR_STRING(
		PRL_QUEST_POWER_OFF,
		tr("Are you sure you want to stop the virtual machine?"),
		tr("The virtual machine is running. If you stop it now, you may lose all unsaved data."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_QUEST_RESET,
		tr("Are you sure you want to reset the virtual machine?"),
		tr("The virtual machine is running. If you reset it now, you may lose all unsaved data."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_QUEST_RESTART,
		tr("Are you sure you want to restart the virtual machine?"),
		tr("If you click Yes now, the virtual machine will be shut down and then started again."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONCURRENT_COMMAND_EXECUTING,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is currently processing operations initialized by another user. Try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_CLONE,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being cloned. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_DELETE,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being deleted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_UNREGISTER,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being removed from the list."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_EDIT_COMMIT,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The configuration of this virtual machine is being edited. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_EXECUTE,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is currently running. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_EXECUTE_EX,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is currently running. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_INTERNAL_REASON,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is locked due to some internal reason. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_EDIT_COMMIT_WITH_RENAME,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being renamed. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_UPDATE_SECURITY,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The security settings are being updated. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_MIGRATE,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being migrated."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_CREATE_SNAPSHOT,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("A snapshot of this virtual machine is being created. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_SWITCH_TO_SNAPSHOT,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is reverting to a snapshot. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_DELETE_TO_SNAPSHOT,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("A snapshot of this virtual machine is being deleted. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_BACKUP,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being backed up. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_RESTORE_FROM_BACKUP,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being restored from backup. Please try again later."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_DISK_RESIZE,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being resized. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_DISK_COMPACT,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being compacted. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_DISK_CONVERT,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual hard disk is being converted. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_ENCRYPT,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being encrypted. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_DECRYPT,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being decrypted. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_CHANGE_PASSWORD,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The password for this virtual machine is being modified. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_CHANGE_FIREWALL,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The firewall settings are being modified. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_COPY_IMAGE,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The image file is being copied. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ADD_HW_FLOPPY_IMAGE_NOT_SPECIFYED,
		tr("The path to the floppy disk image file is not specified."),
		tr("Specify the path and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HDD_IMAGE_NOT_EXIST,
		tr("Unable to find the specified virtual hard disk file."),
		tr("Make sure that the file exists or specify another one."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_AUTHENTICATION_FAILED,
		tr("Incorrect user name or password."),
		tr("Type a valid user name and password and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LOCAL_AUTHENTICATION_FAILED,
		tr("Incorrect user name or password."),
		tr("Type a valid user name and password and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_MOBILE_ADVANCED_AUTH_REQUIRED,
		tr("Authentication Required."),
		tr("Enter your user authentication."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_DELETE_FILES_OUT_OF_VM_DIR,
		tr("Are you sure you want to delete the selected files?"),
		tr("Some of the listed files are located outside the virtual machine folder. These files may be used by other virtual machines or applications. Deleting these files may affect the work of these virtual machines and applications. Click <b>No</b> to cancel the deleting."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CANT_CLONE_HDD_MISSING,
		tr("Cannot clone the virtual machine because the virtual hard disk file %1 is missing."),
		tr("Close the wizard and check the virtual machine configuration file."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CONF_CHANGED_INVALID_ARG,
		tr("Cannot change the virtual machine configuration because the \"%1\" with image path \"%2\" is incorrect."),
		tr("Make sure that the file name is correct or specify another one."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WINDOWS_EXPRESS_INSTALL_USER_NAME_EMPTY,
		tr("The user name is not specified."),
		tr("Type a user name to proceed."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILE_NOT_EXIST,
		tr("The file %1 does not exist."),
		tr("Make sure the file name is correct or specify another %2 file."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CANCEL_CLONE_OPERATION,
		tr("The virtual machine is being cloned. Are you sure you want to stop the process?"),
		tr("The cloning has not been completed. If you stop the process now, you won't be able to use the resulting virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CANCEL_IMPORT_BOOTCAMP_OPERATION,
		tr("The import is in progress. Are you sure you want to stop it?"),
		tr("If you cancel the import now, no Parallels virtual machine will be created."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CANCEL_CONVERT_3RD_VM_OPERATION,
		tr("The conversion is in progress. Are you sure you want to cancel it?"),
		tr("If you cancel the conversion now, no Parallels virtual machine will be created."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CANCEL_CLONE_TO_TEMPLATE_OPERATION,
		tr("The template is being created. Are you sure you want to stop the process?"),
		tr("The process is not complete. If you stop it now, you won't be able to use the resulting template."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CANCEL_DEPLOY_OPERATION,
		tr("The virtual machine is being deployed. Are you sure you want to stop the process?"),
		tr("The deployment has not been completed. If you stop it now, you won't be able to use the resulting virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_FREE_SIZE_FOR_COMPRESSED_DISK,
		tr("The virtual hard disk size exceeds the free space available on the physical hard disk. Do you want to create it anyway?"),
		tr("If you use up the free space on the physical hard disk, you will not be able to add data to this virtual disk."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_COMPRESSED_DISK_IN_EXPRESS_MODE,
		tr("The default capacity of this virtual machine's hard disk is greater than the free space available on the physical hard disk. Do you want to create the virtual machine anyway?"),
		tr("If you use up the free space on the physical hard disk, you will not be able to add new data to this virtual machine's hard disk. To set a different capacity for the virtual hard disk, go back and select the Custom mode of creating a virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_ENOUGH_DISK_FREE_SPACE,
		tr("The size specified for this virtual hard disk exceeds the free space available on the physical hard disk."),
		tr("Choose a different physical volume on which this hard disk image file is to be stored or specify a smaller disk size."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_INSUFFICIENT_SPACE,
		tr("The size specified for this virtual hard disk exceeds the free space available on the physical hard disk."),
		tr("Choose a different physical volume on which this hard disk image file is to be stored or specify a smaller disk size."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_MAC_ADDRESS_INCORRECT,
		tr("Unable to save the configuration because it is invalid."),
		tr("The MAC address of the network adapter %1 is invalid. Correct the MAC address and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SWAP_SLOTS,
		tr("The NODENAME_X position is already used by DEVICENAME_X. Do you want to assign this INTERFACE position to DEVICENAME_Y?"),
		tr("If you assign the NODENAME_X position to DEVICENAME_Y, DEVICENAME_X will use the NODENAME_Y position."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CONVERT_VM_CONFIG_DUE_UNDO_SNAPSHOTS_PRESENT,
		tr("Unable to add the virtual machine in %1."),
		tr("This virtual machine was created with an earlier version of Parallels virtualization software and has a number of snapshots in it. Before adding this virtual machine, open it with the application you used to create it and delete all snapshots."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_RECREATE_HDD,
		tr("Unable to recreate the virtual hard disk."),
		tr("The virtual hard disk cannot be recreated because the virtual machine that uses this disk is not stopped. Shut down the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_RESIZE_HDD,
		tr("Unable to resize the virtual hard disk."),
		tr("The virtual hard disk cannot be resized because the virtual machine that uses this disk is not stopped. Shut down the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CONVERT_HDD_IMAGE_FILE,
		tr("Unable to convert the virtual hard disk to the new format."),
		tr("The virtual hard disk image file is either damaged or locked."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_PARALLELS_DISK,
		tr("%1 does not exist or the specified hard disk image file is not valid."),
		tr("Specify the correct path to a valid Parallels virtual hard disk image file."),
		false,
		PMT_WARNING
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_VMDIR_INVALID_PATH,
		tr("The path you specified is incorrect."),
		tr("Type a full path or use the <b>Choose</b> button to locate the folder."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_RESTORE_DEFAULT_SETTINGS,
		tr("Do you want to restore the default settings?"),
		tr("Your current settings will be discarded and replaced with the default ones."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CREATE_FLOPPY_IMAGE,
		tr("The floppy disk image file \"%1\" cannot be created."),
		tr("Make sure that you have the permission to create it."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_CREATE_FLOPPY_IMAGE_PARAMETERS,
		tr("The floppy disk image file \"%1\" cannot be created."),
		tr("Make sure that you have specified the correct parameters 'User Name', 'Company Name', and 'License Number' for Express installation and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_REPLACE_FLOPPY_IMAGE,
		tr("The %1 floppy disk image file cannot be replaced."),
		tr("Make sure that the \"%1\" file is not read-only and you have the permission to overwrite it."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_FDD_IMAGE_ALREADY_EXIST,
		tr("An older floppy disk image file %1 already exists in this location. Do you want to replace it with a blank floppy image?"),
		tr("If you click Replace, all the data on the image file will be lost."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NO_BOOTING_DEVICE_SELECTED,
		tr("No boot device was selected. Select the device you want the virtual machine to boot from."),
		tr("At least one boot device must be selected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_REMOTE_DISPLAY_HOST_NOT_SPECIFIED,
		tr("Invalid IP Address."),
		tr("An IP address must consist of four numbers, each ranging from 0 to 255, separated by dots."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_REMOTE_DISPLAY_WRONG_PORT_NUMBER,
		tr("The port number is invalid."),
		tr("The port number must be greater than 5899. Enter a valid number and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_REMOTE_DISPLAY_PASSWORD_TOO_LONG,
		tr("The specified VNC password is too long."),
		tr("The VNC password must not exceed %1 characters."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INCORRECT_FDD_PATH,
		tr("The file you specified cannot be used as a floppy disk image file."),
		tr("Specify another floppy disk image file."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DIRECTORY_DOES_NOT_EXIST,
		tr("The folder %1 does not exist."),
		tr("Specify another folder and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_TO_REGISTER_IS_NOT_SPECIFIED,
		tr("You have not selected any virtual machines."),
		tr("Please select one or more virtual machines to add and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_ALREADY_REGISTERED_UNIQUE_PARAMS,
		tr("The name you specified for this virtual machine is already taken."),
		tr("Specify another name and location for the new virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_WARN_ALLOCATE_MEM,
		tr("Are you sure you want to allocate %1 of memory to the virtual machine?"),
		tr("Allocating more than %1 of memory to a virtual machine may significantly decrease its performance."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_QUEST_EXCEED_RECOMMENDED_MEMORY,
		tr("The amount of memory allocated to this virtual machine exceeds the recommended maximum of <b>%2 MB</b>."),
		tr("Allocating more than <b>%2 MB</b> of memory to a virtual machine may affect the overall system performance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CONFIGURE_PHYSICAL_HDD,
		tr("Unable to configure the hard disk settings."),
		tr("You do not have the permission to read the disk or an error occurred when reading the disk."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SHARED_FOLDER_NAME_EXISTS,
		tr("The \"%1\" name is already taken by the shared folder \"%2\"."),
		tr("Type a different name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SHARED_FOLDER_PATH_EXISTS,
		tr("The folder \"%1\" is already shared under the \"%2\" name."),
		tr("Choose a different folder and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SHARED_FOLDER_NAME_IS_NOT_SPECIFIED,
		tr("The folder name is not specified."),
		tr("Specify an arbitrary name for the folder and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SHARED_FOLDER_PATH_IS_NOT_SPECIFIED,
		tr("The folder path is not specified."),
		tr("Specify the path to the folder and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_INCORRECT_SHARED_FOLDER_NAME,
		tr("The shared folder name \"%1\" is invalid."),
		tr("A shared folder name cannot contain any of the following characters: \\/:*?\"&lt;&gt;|."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_INCORRECT_SHARED_FOLDER_PATH,
		tr("The shared folder path is invalid."),
		tr("The path cannot contain references to the current (.) or parent (..) folder. Specify a valid path and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SHARED_FOLDER_NAME_IS_RESERVED_BY_SYSTEM,
		tr("The shared folder cannot use the \"%1\" name because this name is reserved by the software internally."),
		tr("Specify a different folder name. Keep in mind that a shared folder name cannot contain any of the following characters: \\/:*?\"&lt;&gt;|."),
		false,
		PMT_WARNING
		);


	REGISTER_ERROR_STRING(
		GUI_ERR_INVALID_SHARED_FOLDER_PATH,
		tr("Unable to add the folder \"%1\" because it does not exist or is not accessible."),
		tr("Make sure that the folder path is correct and the target location is accessible."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANT_CHANGE_SHARED_FOLDER,
		tr("Unable to select the folder \"%1\" because it does not exist or is not accessible."),
		tr("Make sure that the folder path is correct and the target location is accessible."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_ZERO_FIRST_OCTET_IN_START_IP,
		tr("The DHCP scope start address is incorrect."),
		tr("The first number of a network address should be greater than zero. Specify a valid number."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_ZERO_FIRST_OCTET_IN_END_IP,
		tr("The DHCP scope end address is incorrect."),
		tr("The first number of a network address should be greater than zero. Specify a valid number."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_ZERO_FIRST_OCTET_IN_MASK_IP,
		tr("The DHCP scope mask is incorrect."),
		tr("The DHCP scope mask must be 255.0.0.0, 255.255.0.0, or 255.255.255.0. Specify a valid value."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_INCORRECT_START_IP,
		tr("The DHCP scope start address is incorrect."),
		tr("The specified address cannot be used as the start address of the DHCP scope. Specify a valid address or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_INCORRECT_END_IP,
		tr("The DHCP scope end address is incorrect."),
		tr("The specified address cannot be used as the end address of the DHCP scope. Specify a valid address or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_INCORRECT_MASK_IP,
		tr("The DHCP scope mask is incorrect."),
		tr("The DHCP scope mask must be 255.0.0.0, 255.255.0.0, or 255.255.255.0. Specify a valid value."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_ZERO_LAST_OCTET_IN_START_IP,
		tr("The DHCP scope start address is incorrect."),
		tr("The last number of the DHCP scope start address must be in the range from 1 to 255. Specify a valid address."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_ZERO_LAST_OCTET_IN_END_IP,
		tr("The DHCP scope end address is incorrect."),
		tr("The last number of the DHCP scope end address must be in the range from 1 to 255. Specify a valid address."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_SCOPE_END_OUT_OF_SUBNET,
		tr("The DHCP scope end address is out of the subnet range."),
		tr("The DHCP scope end address must be within the subnet range based on the start address. Specify a valid address."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_START_IP_IS_LESS_THAN_END_IP,
		tr("The DHCP scope is incorrect."),
		tr("The DHCP scope must comprise a continuous range of at least 16 IP addresses from the start address to the end address. Adjust the start and end address values and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_INVALID_IPV6_ADDRESS,
		tr("Invalid IP address"),
		tr("The IPv6 address must consist of 8 hexadecimal numbers from 0x0 to 0xffff, separated by colons."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_INVALID_VM_NAME_SPECIFIED,
		tr("The virtual machine name \"%1\" is invalid."),
		tr("A virtual machine name cannot contain any of the following characters: \\/:*?\"&lt;&gt;|. Specify a valid name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_INVALID_MAC_ADDRESS,
		tr("The MAC address of the network adapter is invalid."),
		tr("Specify a valid MAC address for this network adapter."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_INSTALL_PRL_TOOLS_BETA,
		tr("Automatic installation of Parallels Tools is not supported in this Beta. Please install Parallels Tools manually."),
		tr("Parallels Tools are available from the image file \"%1\". Connect this image to the virtual machine and install Parallels Tools."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_CANT_INSTALL_TOOLS_WITHOUT_CD,
		tr("You cannot install Parallels Tools because the virtual machine has no CD/DVD device."),
		tr("The virtual machine must have at least one CD/DVD device to install Parallels Tools. Please shut down the virtual machine, add a CD/DVD device, run the virtual machine, and start the installation again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_TOOLS_UNSUPPORTED_GUEST,
		tr("Parallels Tools cannot be installed in the virtual machine \"%1\"."),
		tr("The operating system of your virtual machine belongs to the \"%2\" type. Parallels Tools cannot be installed in operating systems of this type."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_INSTALL_TOOLS_WIN,
		tr("Click Continue to connect the Parallels Tools installation CD to this virtual machine and start the installation."),
		tr("If the installation does not start automatically, open your CD/DVD drive in Windows and double-click \"Setup.exe\"."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_INSTALL_TOOLS_LIN,
		tr("Click Continue to connect the Parallels Tools installation CD to this virtual machine."),
		tr("To install Parallels Tools, open a terminal, go to the CD/DVD drive directory and run the following command as root: \"sudo ./install\".<br><br>Note: In some Linux systems, you may need to unmount the installation CD and then mount it again using the following command: \"mount -o exec\", and only after that install Parallels Tools."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_INSTALL_TOOLS_MAC,
		tr("Click Continue to connect the Parallels Tools installation CD to this virtual machine."),
		tr("To install Parallels Tools, open the Parallels Tools CD in the virtual machine and double-click \"Install\"."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_INSTALL_GHOSTSCRIPT_ON_HOST,
		tr("The document has been sent to a printer that is shared with the host computer."),
		tr("To ensure proper printing from the virtual machine, it is recommended that you install a PostScript interpreter as described on the Parallels <a href=\"%1\">website</a>" ),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_QUEST_HDD_SAME_IMAGE,
		tr("The virtual hard disk image file %1 is already used by virtual hard disk %2."),
		tr("Specify another virtual hard disk image file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_REGISTER_VM_TEMPLATE,
		tr("You have selected the virtual machine template %1. Do you want to add it?"),
		tr("You can use the template only for deploying new virtual machines based on it. You cannot run the template as a virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CREATE_NEW_MAC_ADDRESS,
		tr("Duplicate MAC addresses detected. Do you want to recreate the MAC address for the virtual machine \"%1\" or use the current MAC address anyway?"),
		tr("The MAC address of the network adapter of the virtual machine \"%1\" to be registered is already in use."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_TO_START_VM_TEMPLATE,
		tr("You cannot start virtual machine templates."),
		tr("You can either deploy or convert this template to a virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_IDE_NODE_ALREADY_TAKEN,
		tr("The %1 position is already in use."),
		tr("Specify another IDE position for the device."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_SCSI_NODE_ALREADY_TAKEN,
		tr("The %1 position is already in use."),
		tr("Specify another SCSI position for the device."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_CANT_DISPLAY_HELP_FILE,
		tr("Unable to open the help file."),
		tr("Make sure the file exists and you have the means to open it. If the file doesn't exist, reinstall the product and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_INVALID_IMAGE_PATH,
		tr("The specified path to the image file is invalid."),
		tr("Check the path to the file and try again, making sure to type the path correctly."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_INVALID_OUTPUT_PATH,
		tr("The specified path to the output file is invalid."),
		tr("Check the path to the file and try again, making sure to type the path correctly."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SHORTCUT_EXISTS,
		tr("The %1 shortcut is already used for another action."),
		tr("The %1 shortcut cannot simultaneously be assigned to two actions - %2. Correct the assignments and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SHORTCUT_HAS_EMPTY_MODIFIERS,
		tr("The following keyboard shortcuts are invalid: %1."),
		tr("Shortcuts must contain two or more keys. Add one or more modifier keys to each of the listed shortcuts and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VM_NAME_NOT_SPECIFIED,
		tr("The virtual machine name is not specified."),
		tr("Specify an arbitrary name for your virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VM_DIR_NOT_SPECIFIED,
		tr("The virtual machine folder is not specified."),
		tr("Specify a folder for storing the virtual machine files and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_X64GUEST_ON_X32HOST,
		tr("You cannot install a 64-bit operating system in this virtual machine."),
		tr("The virtual machine is configured to run 32-bit operating systems. Create a new virtual machine with a proper configuration and try again."),
		false,
		PMT_CRITICAL
		);


		REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_CONFIGURATION,
		tr("Unable to start the virtual machine."),
		tr("The virtual machine configuration is invalid. Please correct the settings and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CPU_RESTART,
		tr("The virtual machine %1 was restarted."),
		tr("The virtual machine processor was reset by the operating system."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CPU_SHUTDOWN,
		tr("The virtual machine %1 was stopped due to an error."),
		tr("The virtual machine processor was disabled by the operating system."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DELETE_NET_ADAPTER,
		tr("Are you sure you want to delete the virtual network adapter %1?"),
		tr("This operation is irreversible."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_PROBLEM_REPORT_POST_SUCCESS,
		tr("A problem report with the ID number %1 has been sent to Parallels to help us improve the product. Thank you for your feedback."),
		tr("If you'd like to contact our support team to request help with your problem, please copy the problem report ID number, %1, and submit a support ticket to Parallels Support."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_IMAGE_BUSY,
		tr("Unable to recreate the image file %1."),
		tr("The specified image file %1 is currently in use by another application. Specify a different file name and try again." ),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_XML_INVALID,
		tr("The hard disk image file used by the hard disk %1 (%2) is corrupt and its data cannot be recovered."),
		tr("To recreate the image file, open the hard disk properties pane, enable the \"Show advanced settings\" option, and click Recreate. After recreating the disk image, all data on this disk will be lost."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_XML_MISSING,
		tr("The specified file cannot be used as a virtual hard disk."),
		tr("Please specify another file."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_READONLY_FILESYSTEM,
		tr("An error occurred while adding the virtual machine."),
		tr("Unable to add the virtual machine because some of its files are read-only or the specified path does not exist. Make sure that the path is valid and you have the write permission for the virtual machine files and folder and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WRONG_CDD_IMAGE_PATH_SPECIFIED,
		tr("The image file is not specified."),
		tr("Specify the path to the appropriate CD/DVD image file and try again."),
		false,
		PMT_WARNING
		);

	// NOTE: this error is currently unused and replaced by the error
	// PRL_ERR_VMCONF_CPU_COUNT_MORE_MAX_CPU_COUNT
	REGISTER_ERROR_STRING(
		GUI_ERR_WRONG_NUMBER_OF_CPU,
		tr("The specified number of CPUs is incorrect."),
		tr("The maximum allowed number of CPUs cannot exceed %1. Specify a correct value and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SUSPEND_FAILED,
		tr("The virtual machine %1 will continue running."),
		tr("The virtual machine state cannot be saved due to an unrecoverable error. You can only shut down the virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_REJECTED_BY_GUEST,
		tr("Unable to suspend the virtual machine."),
		tr("The virtual machine cannot be suspended because some operations have not been completed in its operating system."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PLAYER_CANT_SUSPEND_IN_PAUSE,
		tr("This virtual machine is paused and cannot be suspended."),
		tr("Resume the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SUSPEND_CHANGED_VM_CONFIG,
		tr("The virtual machine %1 cannot be suspended."),
		tr("Unable to save the state of the virtual machine because its configuration has changed since the last start. Restart the virtual machine and try to suspend it again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_SUSPEND_VM_WITH_BOOTCAMP,
		tr("The virtual machine %1 is a Boot Camp virtual machine and cannot be suspended."),
		tr("Virtual machines that use Boot Camp partitions as hard disks cannot be suspended because the operation may damage the Boot Camp partition."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_ALLOW_TO_SUSPEND_VM_WITH_BOOTCAMP_DISK,
		tr("%1 is using Boot Camp as one of its hard disks. Are you sure you want to suspend this virtual machine?"),
		tr("Do not work with Boot Camp natively while the virtual machine is suspended, or you will lose all unsaved data when resuming the virtual machine."),
		true,
		PMT_QUESTION
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_CORE_STATE_VM_WOULD_STOP,
		tr("Do you want to restore the virtual machine as running or stop it while reverting to this snapshot?"),
		tr("This snapshot was created when the virtual machine was running. If you restore the virtual machine as running, it may work not properly until you restart it."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_SUSPEND_STATE_INCOMPATIBLE_CPU,
		tr("Do you want to resume the virtual machine as running or restart it?"),
		tr("The virtual machine was suspended on another computer. If you resume it as running, it may work incorrectly until you restart it."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_SNAPSHOT_STATE_INCOMPATIBLE_CPU,
		tr("Do you want to restore the virtual machine as running or stop it while reverting to this snapshot?"),
		tr("This snapshot was created on another computer. If you restore the virtual machine, it may work incorrectly until you restart it."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_SUSPEND_STATE_INCOMPATIBLE,
		tr("Unable to resume this virtual machine because it was suspended in a newer version of your Parallels software."),
		tr("Do you want to restart the virtual machine to be able to use it in this version? Note: If you restart the virtual machine now, all unsaved data will be lost."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_SNAPSHOT_STATE_INCOMPATIBLE,
		tr("Unable to restore this virtual machine as running because the snapshot was created in a newer version of your Parallels software."),
		tr("Do you want to stop the virtual machine while reverting? Note: If you stop the virtual machine now, all the data that was not saved at the time the snapshot was taken will be lost."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SNAPSHOT_CHANGED_VM_CONFIG,
		tr("A snapshot of the virtual machine %1 cannot be created."),
		tr("Unable to take a snapshot of the virtual machine because its configuration has changed since the last start. Restart the virtual machine and try to make a snapshot again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_DELETE_STATE_FAILED,
		tr("Unable to delete the snapshot."),
		tr("An error occurred when deleting the snapshot. Try to remove it again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DELETE_UNFINISHED_STATE_FAILED,
		tr("Unable to perform this operation because one of your virtual machine snapshots is still being removed."),
		tr("Free some disk space on the hard disk where the virtual machine is stored and try again. If the problem persists, contact the Parallels support team." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CREATE_SNAPSHOT_FAILED,
		tr("Unable to create the snapshot."),
		tr("An error occurred when creating the snapshot. Try to create it again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STATE_ERROR_CREATE_IMAGE,
		tr("Unable to create the snapshot."),
		tr("An error occurred when creating the snapshot. Try to create it again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STATE_ERROR_RENAMING_IMAGE,
		tr("Unable to create the snapshot."),
		tr("An error occurred when creating the snapshot. Try to create it again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STATE_MERGE_FAILED,
		tr("Unable to merge the snapshots."),
		tr("An error occurred when merging the snapshots. Try to merge the snapshots again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STATE_MERGE_NO_SPACE,
		tr("Not enough free space to merge the snapshots."),
		tr("Free some disk space on the physical hard disk and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STATE_CANT_OPEN_IMAGE,
		tr("Unable to complete the operation."),
		tr("Make sure the snapshot is not in use and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STATE_CANT_OPEN_FOR_WRITE,
		tr("Unable to complete the operation."),
		tr("Make sure the snapshot is not in use and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STATE_CANT_OPEN_LOCKED,
		tr("Unable to complete the operation."),
		tr("Make sure the snapshot is not in use and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STATE_CANT_SAVE_LOCKED,
		tr("Unable to complete the operation."),
		tr("Make sure the snapshot is not in use and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STATE_NO_STATE,
		tr("Unable to complete the operation."),
		tr("The files required for the operation could not be found." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_UPDATE_SNAPSHOT_DATA_FAILED,
		tr("Unable to configure the snapshot parameters."),
		tr("An error occurred when configuring the snapshot parameters. Try to configure them again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_RESUME_FAILED,
		tr("The virtual machine %1 cannot be resumed."),
		tr("Unable to restore the virtual machine state due to an internal error. The virtual machine will be restarted." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CORE_STATE_CORRUPT_SAV_FILE,
		tr("The virtual machine %1 cannot be resumed."),
		tr("Unable to restore the virtual machine state because the file containing this state is damaged. The virtual machine will be restarted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CORE_STATE_CORRUPT_MEM_FILE,
		tr("The virtual machine %1 cannot be resumed."),
		tr("Unable to restore the virtual machine state because the file containing this state is damaged. The virtual machine will be restarted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CORE_STATE_CHANGE_VM_CONFIG,
		tr("The virtual machine %1 cannot be resumed."),
		tr("Unable to restore the state of the virtual machine because its configuration has changed since the last state saving. The virtual machine will be restarted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CORE_STATE_ERROR_COMMON,
		tr("The virtual machine %1 cannot be resumed."),
		tr("Unable to restore the virtual machine state due to an internal error. The virtual machine will be restarted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CORE_STATE_NO_FILE,
		tr("The virtual machine %1 cannot be restored as running."),
		tr("Unable to restore the virtual machine state because the file containing this state is absent."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CORE_STATE_CANCELLED,
		tr("The virtual machine %1 won't be restored."),
		tr("The operation was canceled by the user"),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CORE_STATE_VM_WOULD_RESTART,
		tr("The virtual machine %1 cannot be restored as running."),
		tr("The virtual machine will be restarted"),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_BOOTCAMP_NOT_NTFS_ONLY_DISK,
		tr("%1 cannot be suspended, because the Boot Camp partition it uses is not formatted as NTFS."),
		tr("Only NTFS-formatted Boot Camp partitions can be suspended."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_BOOTCAMP_NTFS_RW_MOUNTERS_DETECTED,
		tr("%1 uses an NTFS-formatted Boot Camp partition and cannot be suspended."),
		tr("Software allowing reading and writing to NTFS partitions has been found on your Mac. To be able to suspend %1, remove any such software from your Mac."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_RESUME_BOOTCAMP_CORRUPT_DISK_STATE_PARAM,
		tr("%1 cannot be resumed and will be restarted."),
		tr("The Boot Camp partition used by this virtual machine has a wrong checksum."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_RESUME_BOOTCAMP_CHANGED_DISK_CONTENTS,
		tr("%1 will be restarted, as the Boot Camp partition it uses has changed since the moment the virtual machine was suspended."),
		tr("Somebody might have logged in to the Boot Camp partition natively since the virtual machine was suspended."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILE_OR_DIR_ALREADY_EXISTS,
		tr("A virtual machine or folder with the name of \"%1\" already exists in %2."),
		tr("Specify another name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LPT_OUTPUT_FILE_IS_NOT_SPECIFIED,
		tr("The path to the printer port output file %1 is not specified."),
		tr("Specify a path to the output file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_COM_OUTPUT_FILE_IS_NOT_SPECIFIED,
		tr("The path to the serial port output file %1 is not specified."),
		tr("Specify a path to the output file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_EDIT_SUSPENDED_VM,
		tr("This virtual machine is suspended."),
		tr("To be able to edit the configuration of this virtual machine, you need to stop it."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_EDIT_SUSPENDED_VM,
		tr("This virtual machine is suspended."),
		tr("You can view the configuration of a suspended virtual machine but cannot edit it. To stop the virtual machine and edit its configuration, click <b>Stop and Edit</b>. To keep the suspended state and view the configuration in read-only mode, click <b>View</b>."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_STOP_SUSPENDED_VM,
		tr("This virtual machine is suspended and may contain unsaved data. Are you sure you want to stop it?"),
		tr("If you stop the virtual machine now, all unsaved data will be lost."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CHANGE_VM_MIGRATION_TYPE,
		tr("Unable to move the virtual machine %1 using the specified migration type. Do you want to migrate it anyway?"),
		tr("The CPUs on the source and destination servers are incompatible. If you continue, the virtual machine will be stopped during the migration."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_APPLY_CHANGES_PENDING,
		tr("Unable to change the device state."),
		tr("The device state cannot be modified because there are some pending changes that require the virtual machine to be restarted. Restart the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_GUEST_MAC_INVALID_VERSION,
		tr("An invalid Mac OS version is installed."),
		tr("Mac OS X virtual machines can have only the Mac OS X Server Leopard or Mac OS X Server Snow Leopard operating system installed. Other versions of Mac OS are not supported."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SOUND_DEVICE_WRITE_FAILED,
		tr("An error occurred while writing to the sound device %1."),
		tr("The sound device will be disconnected from the virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACCES_DENIED_FILE_TO_PARENT_PARENT_DIR,
		tr("The file %1 cannot be created in the specified folder."),
		tr("You do not have the permission to create files in this folder. Choose another folder and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CREATE_PARALLEL_PORT_IMAGE,
		tr("Unable to create the printer port output file %1."),
		tr("Make sure that the specified path is correct and you have the permission to write to the specified folder."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_OVERWRITE_HDD_IMAGE,
		tr("The virtual hard disk image file %1 already exists. Do you want to replace it with a new one?"),
		tr("If you replace the image file, all the data on the original virtual hard disk will be lost."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HDD_IMAGE_IS_ALREADY_EXIST,
		tr("The hard disk image file cannot be created."),
		tr("The virtual hard disk image file named %1 already exists."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_VM_SHUTDOWN_CONDITIONS,
		tr("The virtual machine can be shut down if its operating system is loaded and supports Advanced Configuration and Power Interface (ACPI)."),
		tr("If the virtual machine does not shut down after you click OK, perform a shutdown from within the virtual machine."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CONFIG_DOESNT_EXIST,
		tr("Unable to complete the action for the virtual machine %1."),
		tr("No virtual machine configuration file could be found in the location \"%2\"."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_ALREADY_REGISTERED_VM_NAME,
		tr("The name %1 is already used by another virtual machine or template."),
		tr("Specify another name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_ALREADY_REGISTERED,
		tr("The virtual machine \"%1\" is already registered."),
		tr("You cannot register a virtual machine more than once."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_VM_DVD_DISCONNECTED_BY_GUEST,
		tr("The CD/DVD disc was ejected from the guest operating system."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMDIR_PATH_IS_NOT_ABSOLUTE,
		tr("The specified path is incorrect."),
		tr("Type the absolute path or use the <b>Choose</b> button to locate the folder."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_PAUSE_FAILED,
		tr("Unable to pause the virtual machine %1."),
		tr("An error occurred while trying to pause the virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_UNPAUSE_FAILED,
		tr("Unable to unpause the virtual machine %1."),
		tr("An error occurred while trying to unpause the virtual machine."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_SERVICE_BUSY,
		tr("Action failed."),
		tr("Cluster service is busy."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_IMAGE_PATH_NOT_SPECIFIED,
		tr("The image file path is invalid."),
		tr("Please specify a valid image file path and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SOCKET_NOT_SPECIFIED,
		tr("The socket location is invalid."),
		tr("Specify a valid location for the socket and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OUTPUT_PATH_NOT_SPECIFIED,
		tr("The output file path is not specified or invalid."),
		tr("Specify a valid path for the output file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_GUEST_MAC_NOT_ENOUGH_MEMORY,
		tr("Not enough memory to start the virtual machine."),
		tr("Increase the amount of memory allocated to the virtual machine. To this effect, select <b>Virtual Machine</b> > <b>Edit Configuration...</b> and change the settings on the <b>Memory</b> pane."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILE_WRITE_ERROR,
		tr("Failed to write data to the hard disk."),
		tr("An input/output error occurred when accessing the hard disk. The error code is %1."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANT_CHANGE_CDROM_CONNECT_OPTION,
		tr("The \"Connected\" option is unavailable because no hard disk image file is specified."),
		tr("Specify a virtual hard disk image file to enable this option."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILE_DISK_SPACE_ERROR,
		tr("Not enough free space on the physical disk drive %1."),
		tr("Free some space on this disk drive or select another location and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANT_CLONE_SUSPENDED_VM,
		tr("You cannot %1 the virtual machine \"%2\"."),
		tr("The virtual machine \"%1\" is currently suspended. Resume the virtual machine, shut it down, and try to %2 it again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INCORRECT_CDROM_PATH,
		tr("The specified file cannot be used as a CD/DVD image file."),
		tr("Specify another CD/DVD image file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP_VM_IS_NOT_STARTED,
		tr("Unable to perform the operation because the virtual machine \"%1\" is not started."),
		tr("This operation can be performed for running virtual machines only."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP_VM_IS_NOT_STOPPED,
		tr("Unable to perform the operation because the virtual machine \"%1\" is not stopped."),
		tr("This operation can be performed for stopped virtual machines only."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_GET_DISK_FREE_SPACE_FAILED,
		tr("Unable to get disk information."),
		tr("An error occurred while retrieving the disk information. Please try again. If the problem persists, contact the Parallels support team."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_QUESTION_CAN_NOT_GET_DISK_FREE_SPACE,
		tr("Not enough free space on the physical hard disk. Do you want to create the virtual hard disk anyway?"),
		tr("The operation may fail if the amount of free space on the physical hard disk proves insufficient."),
		true,
		PMT_QUESTION
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CONFIG_INVALID_VM_UUID,
		tr("Unable to complete the operation for the virtual machine \"%1\"."),
		tr("The virtual machine ID is invalid. To resolve the problem, remove the virtual machine from the list and add it again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CONFIG_INVALID_SERVER_UUID,
		tr("Unable to complete the operation for the virtual machine \"%1\"."),
		tr("The virtual machine is registered on another server. You can only unregister the virtual machine or stop it."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_GOING_TO_TAKE_SMART_GUARD_SNAPSHOT,
		tr("SmartGuard is ready to create a scheduled snapshot of the virtual machine \"%1\". Do you want to create it now?"),
		tr("If no action is taken, the snapshot will be created automatically in @PRL_GUI_STD_SECONDS_TIMER seconds."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_GOING_TO_TAKE_SMART_GUARD_SNAPSHOT,
		tr("An automatic snapshot is going to be taken."),
		tr("SmartGuard is about to start creating a scheduled snapshot of the virtual machine \"%1\". Click <b>Create</b> to create a snapshot now, or click <b>Skip</b> to do without.<br><br>Note: To disable this notification, edit the SmartGuard settings in the virtual machine configuration."),
		false,
		PMT_QUESTION
		);

	///// Config validation ///////////////////////////////////////////////////////

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_VM_NAME_IS_EMPTY,
		tr("The virtual machine name is not specified."),
		tr("Enter a unique name for the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_VM_NAME_HAS_INVALID_SYMBOL,
		tr("The virtual machine name \"%1\" is invalid."),
		tr("A virtual machine name cannot contain any of the following characters: \\/:*?\"&lt;&gt;|. Specify a different name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_INVALID_DEVICE_MAIN_INDEX,
		tr("The virtual machine configuration is invalid."),
		tr("One or more virtual devices have a wrong device index. Please make sure that each device has a unique index."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_REAL_HARD_UNDO_DISKS_NOT_ALLOW,
		tr("Rollback mode cannot be used with real hard disks."),
		tr("One or more real hard disks are enabled in the virtual machine configuration. Rollback mode cannot be used for such virtual machines."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_BOOTCAMP_HARD_UNDO_DISKS_NOT_ALLOW,
		tr("Rollback mode cannot be used with Boot Camp hard disks."),
		tr("One or more Boot Camp hard disks are enabled in the virtual machine configuration. Rollback mode cannot be used in such virtual machines."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_INCOMPAT_HARD_UNDO_DISKS_NOT_ALLOW,
		tr("Unable to turn on Rollback mode because the virtual machine upgrade is not yet completed."),
		tr("Start the virtual machine and wait until the upgrade is complete."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_BOOTCAMP_HARD_SNAPSHOTS_NOT_ALLOW,
		tr("Snapshots cannot be used with Boot Camp hard disks."),
		tr("One or more Boot Camp hard disks are enabled in the virtual machine configuration. Snapshots cannot be used in such virtual machines."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FREE_DISK_SPACE_FOR_CREATE_SNAPSHOT,
		tr("Unable to create the snapshot."),
		tr("There is not enough free space on the physical disk. Free some space and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FREE_DISK_SPACE_FOR_REVERT_TO_SNAPSHOT,
		tr("Unable to revert to the snapshot."),
		tr("There is not enough free space on the physical disk. Free some space and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FREE_DISC_SPACE_FOR_CLONE,
		tr("The virtual machine %1 cannot be cloned."),
		tr("There is not enough disk space. Free some disk space or select another location for the virtual machine clone and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_REAL_HARD_SAFE_MODE_NOT_ALLOW,
		tr("Rollback Mode cannot be used with real hard disks."),
		tr("One or more real hard disks are enabled in the virtual machine configuration. Such virtual machines cannot be started in Rollback Mode."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_BOOTCAMP_SAFE_MODE_NOT_ALLOW,
		tr("Rollback Mode cannot be used with Boot Camp hard disks."),
		tr("One or more Boot Camp hard disks are enabled in the virtual machine configuration. Such virtual machines cannot be started in Rollback Mode."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_INCOMPAT_SAFE_MODE_NOT_ALLOW,
		tr("Unable to start the virtual machine in Rollback Mode because the virtual machine upgrade is not yet completed."),
		tr("Start the virtual machine in the regular way and wait until the upgrade is complete."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NO_HD_IMAGES_IN_UNDO_DISKS_MODE,
		tr("Unable to turn on Rollback mode."),
		tr("Rollback mode cannot be enabled because there is no virtual hard disk in the virtual machine configuration."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NO_HD_IMAGES_IN_SAFE_MODE,
		tr("Unable to start the virtual machine in Rollback Mode."),
		tr("The virtual machine does not have any virtual hard disk in its configuration."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_BOOTCAMP_HARD_DISK_SMART_GUARD_NOT_ALLOW,
		tr("Unable to turn on the SmartGuard option."),
		tr("The SmartGuard option cannot be enabled because the virtual machine uses the Boot Camp partition as its virtual hard disk."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_INCOMPAT_HARD_DISK_SMART_GUARD_NOT_ALLOW,
		tr("Unable to turn on SmartGuard option because the virtual machine upgrade is not yet completed."),
		tr("Start the virtual machine and wait until the upgrade is complete."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_VIDEO_NOT_ENABLED,
		tr("This virtual machine cannot be started because the video device is disabled."),
		tr("Open the virtual machine configuration and enable the video device."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NO_AUTO_COMPRESS_WITH_UNDO_DISKS,
		tr("AutoCompress cannot be used with Rollback mode. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, the AutoCompress feature will not work."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NO_AUTO_COMPRESS_WITH_SMART_GUARD,
		tr("AutoCompress cannot be used with SmartGuard. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, the AutoCompress feature will only work while there are no snapshots. If at least one snapshot is created, AutoCompress will stop functioning."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NO_SMART_GUARD_WITH_UNDO_DISKS,
		tr("Both Rollback mode and SmartGuard are enabled. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, SmartGuard will not work because snapshots cannot be created when Rollback mode is enabled."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_BOOT_OPTION_INVALID_DEVICE_TYPE,
		tr("The virtual machine has one or more invalid boot devices."),
		tr("Only hard disks, floppy disks, CD/DVD drives, and network adapters can be used as boot devices. Remove the inappropriate devices from the Boot Order list and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_BOOT_OPTION_DUPLICATE_DEVICE,
		tr("One or more duplicate devices are present in the boot sequence."),
		tr("Each device can be included in the boot sequence only once. Remove the duplicate devices from the boot sequence and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_BOOT_OPTION_DEVICE_NOT_EXISTS,
		tr("The boot sequence contains a device that does not exist."),
		tr("Make sure that the specified boot devices exist in the virtual machine configuration. Remove the missing device from the boot sequence and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_REMOTE_DISPLAY_PORT_NUMBER_IS_ZERO,
		tr("An incorrect port number is specified for Remote Display."),
		tr("The port number must be greater than 0. Correct the value and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_REMOTE_DISPLAY_EMPTY_PASSWORD,
		tr("The password for the Remote Display access is not specified. Are you sure you want to save the settings?"),
		tr("If you do not specify the password, other users will be able to access this virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SHARED_FOLDERS_EMPTY_FOLDER_NAME,
		tr("The shared folder name is not specified."),
		tr("Enter a unique name for the shared folder and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SHARED_FOLDERS_DUPLICATE_FOLDER_NAME,
		tr("One or more duplicate shared folder names have been detected."),
		tr("Make sure that each shared folder has a unique name. Remove the duplicate folders and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SHARED_FOLDERS_INVALID_FOLDER_PATH,
		tr("The \"%1\" shared folder is not accessible."),
		tr("Make sure that its path is correct: %2."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SHARED_FOLDERS_DUPLICATE_FOLDER_PATH,
		tr("One or more duplicate paths for your shared folders have been detected."),
		tr("Make sure that each shared folder has a unique path. Remove the duplicate folders and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CPU_ZERO_COUNT,
		tr("The specified CPUs number is invalid."),
		tr("The number of CPUs must be greater than 0. Correct the value and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CPU_COUNT_MORE_MAX_CPU_COUNT,
		tr("The specified CPUs number is invalid."),
		tr("The number of CPUs cannot exceed %1. Correct the value and try again."),
		false,
		PMT_WARNING
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CPU_MASK_INVALID,
		tr("The specified CPU mask is invalid."),
		tr("Correct the value and try again."),
		false,
		PMT_WARNING
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CPU_MASK_INVALID_CPU_NUM,
		tr("The specified CPU mask is invalid."),
		tr("The CPU number cannot exceed mask size (%1). Correct the value and try again."),
		true,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_ZERO_SIZE,
		tr("The specified main memory size is invalid."),
		tr("The memory size must be greater than 0. Correct the value and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_OUT_OF_RANGE,
		tr("The specified main memory size is invalid."),
		tr("The memory size must be a number between %1 and %2 MB. Correct the value and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_NOT_4_RATIO_SIZE,
		tr("The specified main memory size is invalid."),
		tr("The memory size must be a multiple of 4. Correct the value and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_MAX_BALLOON_SIZE_MORE_100_PERCENT,
		tr("The specified memory baloon size is invalid."),
		tr("The memory baloon size must be specified in the range from 0 to 100 percent. Correct the maximum size and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_MQ_PRIOR_ZERO,
		tr("The specified host memory quota priority is zero."),
		tr("The host memory quota priority must be specified in the range from 1 to 100. Correct the value and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_MQ_PRIOR_OUT_OF_RANGE,
		tr("The specified host memory priority is greater than the maximum value."),
		tr("The host memory priority must be specified in the range from 1 to 100. Correct the value and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_MQ_INVALID_RANGE,
		tr("The specified host memory quota range is invalid."),
		tr("Correct the minimum and maximum values of the host memory quota and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_MQ_MIN_LESS_VMM_OVERHEAD_VALUE,
		tr("The specified host memory quota minimum value is not enough for the virtual machine to work normally."),
		tr("The host memory quota minimum value must exceed the virtual machine overhead. It can be calculated as 100 + (guest OS memory + video memory)/64. Correct the value and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_MQ_MIN_OUT_OF_RANGE,
		tr("The specified host memory quota minimum value is too high. The virtual machine will use it inefficiently."),
		tr("Set the lower value (guest OS memory + video memory + expected overhead) or specify the unlimited value and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_VIDEO_MEMORY_OUT_OF_RANGE,
		tr("The specified video memory size is invalid."),
		tr("The memory size must be a number between %1 and %2 MB. Correct the value and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_FLOPPY_DISK_SYS_NAME_IS_EMPTY,
		tr("No image file is specified for the floppy disk drive. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, the floppy disk drive will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_FLOPPY_DISK_SYS_NAME_HAS_INVALID_SYMBOL,
		tr("The floppy disk drive name \"%1\" is invalid."),
		tr("A floppy disk drive name cannot contain any of the following characters: :*?\"&lt;&gt;|. Specify a different name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_FLOPPY_DISK_IMAGE_IS_NOT_EXIST,
		tr("The floppy disk image file \"%1\" does not exist. Do you want to save the virtual machine configuration anyway?"),
		tr("Make sure that the path to the floppy disk image file is correct and try again."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_FLOPPY_DISK_IS_NOT_ACCESSIBLE,
		tr("The physical floppy disk drive used to simulate your virtual floppy disk drive is not accessible. Do you want to save the virtual machine configuration anyway?"),
		tr("Make sure that the physical floppy disk drive is available or disable the floppy disk drive in the virtual machine configuration."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_FLOPPY_DISK_IMAGE_IS_NOT_VALID,
		tr("The floppy disk image file \"%1\" is not valid. Do you want to save the configuration of the virtual machine anyway?"),
		tr("Make sure that the file is valid or select another image file for Floppy Disk."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_FLOPPY_DISK_URL_FORMAT_SYS_NAME,
		tr("The image file \"%1\" used by the Floppy Disk device is stored on an Internet location."),
		tr("Virtual devices cannot use files stored on Internet locations. Please specify another file for the Floppy Disk device."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_WRONG_TYPE_BOOTCAMP_PARTITION,
		tr("The Boot Camp partition \"%1\" used by Hard Disk %2 is not supported."),
		tr("Please open the virtual machine configuration and select a supported Boot Camp partition in the Hard Disk %2 settings."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_MISS_BOOTCAMP_PARTITION,
		tr("The Boot Camp partition \"%1\" used by Hard Disk %2 is missing."),
		tr("Please open the virtual machine configuration and select an existing Boot Camp partition in the Hard Disk %2 settings."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_WRONG_TYPE_FOR_ENCRYPTRED_VM,
		tr("Hard Disk %1 is using a physical disk and cannot be encrypted."),
		tr("Encrypted virtual machines can use virtual hard disks only."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_NOT_ENOUGH_SPACE_FOR_ENCRYPT_DISK,
		tr("Not enough free space on the physical hard disk \"%1\"."),
		tr("Only %2 GB is available on the physical hard disk, while %3 GB is needed for the encryption. Please free some space on the physical hard disk and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CD_DVD_ROM_SYS_NAME_IS_EMPTY,
		tr("No image file is specified for CD/DVD drive %1. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, CD/DVD drive %1 will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CD_DVD_ROM_DUPLICATE_SYS_NAME,
		tr("The specified CD/DVD drive is already used by another virtual CD/DVD drive. Do you want to connect it to CD/DVD drive %1 anyway?"),
		tr("If you click Yes now, the physical CD/DVD drive will be disconnected from the virtual CD/DVD drive that is currently using it."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CD_DVD_ROM_IMAGE_IS_NOT_EXIST,
		tr("The image file \"%1\" used by CD/DVD drive %2 does not exist. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, CD/DVD drive %2 will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CD_DVD_ROM_URL_FORMAT_SYS_NAME,
		tr("The image file \"%1\" used by CD/DVD %2 is stored on an Internet location."),
		tr("Virtual devices cannot use files stored on Internet locations. Please specify another file for CD/DVD %2."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_SYS_NAME_IS_EMPTY,
		tr("No image file is specified for hard disk %1. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, hard disk %1 will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_SYS_NAME_HAS_INVALID_SYMBOL,
		tr("The hard disk drive name \"%1\" is invalid."),
		tr("A hard disk drive name cannot contain any of the following characters: :*?\"&lt;&gt;|. Specify a different name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_IMAGE_IS_NOT_EXIST,
		tr("The image file \"%1\" used by hard disk %2 does not exist. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, hard disk %2 will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_IMAGE_IS_NOT_VALID,
		tr("The specified hard disk image file is invalid. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, the virtual hard disk will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_DUPLICATE_SYS_NAME,
		tr("The specified source is already used by hard disk %1."),
		tr("Specify any other available source for this device and try again."),
		true,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_URL_FORMAT_SYS_NAME,
		tr("The image file \"%1\" used by Hard Disk %2 is stored on an Internet location."),
		tr("Virtual devices cannot use files stored on Internet locations. Please specify another file for Hard Disk %2."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_INVALID_BOUND_INDEX,
		tr("The physical network adapter connected to virtual network adapter %1 does not exist. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, this virtual network adapter will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_INVALID_MAC_ADDRESS,
		tr("The MAC address assigned to Network Adapter %1 is invalid."),
		tr("Specify a valid MAC address for this network adapter. "\
		   "A valid MAC address should be six groups of two hexadecimal digits, without any colons or hyphens, beginning with 001C42."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_DUPLICATE_MAC_ADDRESS,
		tr("One or more duplicate addresses of your network adapters have been detected. Do you want to save the virtual machine configuration anyway?"),
		tr("If you save the configuration, the network adapters with conflicting MAC addresses may work incorrectly."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_DUPLICATE_IP_ADDRESS,
		tr("One or more duplicate IP addresses of your network adapters have been detected. Do you want to save the virtual machine configuration anyway?"),
		tr("If you save the configuration, the network adapters with conflicting IP addresses may work incorrectly."),
		true,
		PMT_QUESTION
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_DUPLICATE_IP_ADDRESS,
		tr("The following duplicated IP addresses have been detected: '%1'"),
		tr("Saving the configuration may cause the network adapters with conflicting"
			" IP addresses to work incorrectly."),
		true,
		PMT_QUESTION
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_GUEST_TOOLS_NOT_AVAILABLE,
				tr("The virtual machine does not have Parallels Tools installed."
				   " Do you want to save the configuration anyway?"),
				tr("If you save the configuration now, the network adapter may work incorrectly."
				   " Install Parallels Tools and configure the network adapter settings again."),
		true,
		PMT_QUESTION
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_ETHLIST_CREATE_ERROR,
		tr("Failed to obtain a list of physical network adapters. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, the virtual machine networking will be disabled."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_INVALID_IP_ADDRESS,
		tr("The \"IP address/subnet MASK\" \"%1\" assigned to Network Adapter %2 is invalid. Do you want to save the virtual machine configuration anyway?"),
		tr("If you save the configuration, the network adapter may work incorrectly."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_MULTICAST_IP_ADDRESS,
		tr("The IP address \"%1\" assigned to Network Adapter %2 is a multicast IP address. Do you want to save the virtual machine configuration anyway?"),
		tr("IP addresses in the range from 224.0.0.0 to 239.255.255.255 are reserved as multicast addresses. If you save the configuration, the network adapter may work incorrectly."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_BROADCAST_IP_ADDRESS,
		tr("The IP address \"%1\" assigned to Network Adapter %2 is a broadcast IP address. Do you want to save the virtual machine configuration anyway?"),
		tr("If you save the configuration, the network adapter may work incorrectly."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_INVALID_GATEWAY_IP_ADDRESS,
		tr("The IP address \"%1\" assigned to Network Adapter %2 is an invalid gateway IP address. Do you want to save the virtual machine configuration anyway?"),
		tr("If you save the configuration, the network adapter may work incorrectly."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_GATEWAY_NOT_IN_SUBNET,
		tr("The IP address \"%1\" assigned to Network Adapter %2 does not belong to any subnet. Do you want to save the virtual machine configuration anyway?"),
		tr("If you save the configuration, the network adapter may work incorrectly."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_INVALID_DNS_IP_ADDRESS,
		tr("The IP address \"%1\" assigned to Network Adapter %2 is an invalid DNS IP address. Do you want to save the virtual machine configuration anyway?"),
		tr("If you save the configuration, the network adapter may work incorrectly."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_INVALID_SEARCH_DOMAIN_NAME,
		tr("The domain name \"%1\" assigned to Network Adapter %2 contains invalid symbols. Do you want to save the virtual machine configuration anyway?"),
		tr("If you save the configuration, the network adapter may work incorrectly."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_ROUTED_NO_STATIC_ADDRESS,
		tr("Network Adapter %1 operating in the routed mode must have at least one static IP address assigned."),
		tr("If you save the configuration, the network adapter may work incorrectly."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SOUND_MIXER_IS_EMPTY,
		tr("The sound input device name is not specified. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, the sound input device will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SOUND_OUTPUT_IS_EMPTY,
		tr("The sound output device name is not specified. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, the sound output device will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SERIAL_PORT_SYS_NAME_IS_EMPTY,
		tr("No output file is specified for Serial Port %1. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, Serial Port %1 will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SERIAL_PORT_SYS_NAME_HAS_INVALID_SYMBOL,
		tr("The serial port name \"%1\" is invalid."),
		tr("A serial port name cannot contain any of the following characters: :*?\"&lt;&gt;|. Specify a different name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SERIAL_PORT_IMAGE_IS_NOT_EXIST,
		tr("The output file of the Serial Port does not exist. Do you want to save the configuration of the virtual machine anyway?"),
		tr("If you continue the Serial Port will be disconnected at virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SERIAL_PORT_URL_FORMAT_SYS_NAME,
		tr("The output file \"%1\" used by Serial Port %2 is stored on an Internet location."),
		tr("Virtual devices cannot use files stored on Internet locations. Please specify another file for Serial Port %2."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_PARALLEL_PORT_SYS_NAME_IS_EMPTY,
		tr("No output file is specified for Printer Port %1. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, Printer Port %1 will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_PARALLEL_PORT_SYS_NAME_HAS_INVALID_SYMBOL,
		tr("The printer port name \"%1\" is invalid."),
		tr("A printer port name cannot contain any of the following characters: :*?\"&lt;&gt;|. Specify a different name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_PARALLEL_PORT_IMAGE_IS_NOT_EXIST,
		tr("The output file used by the printer port does not exist. Do you want to save the virtual machine configuration anyway?"),
		tr("If you click Yes now, the printer port will not be connected at the virtual machine startup."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_PARALLEL_PORT_URL_FORMAT_SYS_NAME,
		tr("The output file \"%1\" used by Printer Port %2 is stored on an Internet location."),
		tr("Virtual devices cannot use files stored on Internet locations. Please specify another file for Printer Port %2."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_IDE_DEVICES_COUNT_OUT_OF_RANGE,
		tr("The allowed number of IDE devices for this virtual machine has been exceeded."),
		tr("The total number of IDE devices in the virtual machine configuration cannot be greater than %1. Remove some of the devices and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_IDE_DEVICES_DUPLICATE_STACK_INDEX,
		tr("This virtual machine has two or more devices with the same IDE position."),
		tr("Make sure that each device has a unique position on the IDE bus and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SATA_DEVICES_COUNT_OUT_OF_RANGE,
		tr("The allowed number of SATA devices for this virtual machine has been exceeded."),
		tr("The total number of SATA devices in the virtual machine configuration cannot be greater than %1. Remove some of the devices and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SATA_DEVICES_DUPLICATE_STACK_INDEX,
		tr("This virtual machine has two or more devices with the same SATA position."),
		tr("Make sure that each device has a unique position on the SATA bus and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SCSI_DEVICES_COUNT_OUT_OF_RANGE,
		tr("The allowed number of SCSI devices for this virtual machine has been exceeded."),
				tr("The total number of SCSI devices in the virtual machine configuration cannot be greater than %1. Remove some of the devices and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SCSI_DEVICES_DUPLICATE_STACK_INDEX,
		tr("This virtual machine has two or more devices with the same SCSI position."),
		tr("Make sure that each device has a unique position on the SCSI bus and try again."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_SCSI_BUSLOGIC_WITH_EFI_NOT_SUPPORTED,
		tr("Virtual machines cannot use both the BusLogic SCSI controller and EFI firmware at the same time."),
		tr("Specify another SCSI controller or disable the EFI firmware."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_GENERIC_PCI_DEVICE_CANNOT_BE_ADDED,
		tr("The PCIe device you are trying to add cannot be used by this virtual machine. Do you want to add it anyway?"),
		tr("Virtual machines can use PCIe devices only when they are running on a host computer whose processor supports the Intel VT-d technology."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_GENERIC_PCI_DEVICE_NOT_FOUND,
		tr("The device %1 cannot be found."),
		tr("Make sure it is plugged in."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_GENERIC_PCI_DEVICE_NOT_CONNECTED,
		tr("This device is reserved for use by the host computer only and cannot be added."),
		tr("You can change this setting in <b>File</b> > <b>Preferences</b> > <b>Intel VT-d</b>."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_GENERIC_PCI_DEVICE_DUPLICATE_IN_ANOTHER_VM,
		tr("The device %1 is already assigned to another virtual machine. Do you want to save the virtual machine configuration anyway?" ),
		tr("This device can be used only by a single running virtual machine at a time. If you start this virtual machine, the device will become unavailable to other virtual machines."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_GENERIC_PCI_WRONG_DEVICE,
		tr("The device you are trying to add belongs to another class of devices."),
		tr("Specify another device and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_GENERIC_PCI_DUPLICATE_SYS_NAME,
		tr("This device is already present in the virtual machine configuration."),
		tr("Specify another device and try again."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_GENERIC_PCI_VIDEO_WRONG_COUNT,
		tr("You cannot assign more than two PCIe video adapters to a single virtual machine."),
		tr("Make sure that only two PCIe video adapters are added to this virtual machine configuration and try again."),
		true,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NO_IP_ADDRESSES_SPECIFIED_FOR_OFFLINE_MANAGEMENT,
		tr("This virtual machine configuration contains no host-only network adapter which is required for offline management."),
		tr("To be able to manage this virtual machine offline, you need to add at least one host-only network adapter with at least one IP address to its configuration."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_NO_SHARED_NETWORK_FOR_OFFLINE_MANAGEMENT,
		tr("Shared virtual network required for offline management is missing from this server network configuration."),
		tr("To be able to manage virtual machines on this server while they are offline, you need to add a shared virtual network by editing the server's network settings."),
		false,
		PMT_WARNING
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_DESKTOP_MODE_REMOTE_DEVICES,
		tr("@PRL_PRODUCT_NAME does not support using remote devices."),
		tr("Check the virtual machine configuration and try again."),
		false,
		PMT_WARNING
		);
////
////////////////////////////////////////////////////////////////////////////////

	REGISTER_ERROR_STRING(
		GUI_ERR_CANT_CHANGE_CLIENT_CDROM_CONNECT_OPTION,
		tr("The Connected option cannot be enabled for CD/DVD drives on the client computer."),
		tr("This option can be enabled for CD/DVD drives on the host computer only. You can manually connect a CD/DVD drive on the client computer to a running virtual machine using the Devices menu."),
		true,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CONVERT_VM_CONFIG_DUE_UNDO_DISKS_PRESENT,
		tr("Unable to add the virtual machine located in %1."),
		tr("This virtual machine was created with an earlier version of the Parallels software and has the Rollback mode option enabled. Before adding the virtual machine, open it with the same application version that was used for its creation and disable the Rollback mode option using Configuration Editor > Options > Advanced."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CONVERT_OLD_VM_WITH_INSTALLED_TOOLS,
		tr("Unable to add the virtual machine located in %1."),
		tr("This virtual machine was created with an earlier version of the Parallels software and has Parallels Tools installed. Before adding the virtual machine, run it with the same application version that was used for its creation and uninstall Parallels Tools using Add or Remove Programs in Windows Control Panel. Then shut down the virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MUST_BE_STOPPED_FOR_CHANGE_DEVICES,
		tr("The device cannot be added or removed while the virtual machine is running."),
		tr("Stop the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HDD_IMAGE_COPY,
		tr("Unable to copy the hard disk image file '%1' to '%2'."),
		tr("Make sure that you have the necessary permissions for the source file and the target folder. Verify that the source file is not in use and the target folder is available."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_USER_AUTHENTICATION_FAILED,
		tr("Unable to authenticate the user."),
		tr("Make sure that the specified credentials are correct and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_COULDNT_BE_STARTED_UNDER_SPECIFIED_USER,
		tr("The user \"%1\" does not have the permission to start this virtual machine."),
		tr("Open the Virtual Machine Configuration dialog and enable the View and run option on the Permissions tab. Make sure that the user has the read and execute permissions for the virtual machine folder. Contact your Parallels Server administrator for further assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_USB_INSTALL_DRIVER_FAILED,
		tr("Do you want to disconnect the %1 device from your physical computer?"),
		tr("This device is in use by your physical computer. For the device to be connected to the virtual machine, it should be forcefully disconnected from your physical computer, which might cause data loss or corruption."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_USB_NO_FREE_PORTS,
		tr("The USB device cannot be connected."),
		tr("There are no virtual USB host controller ports available in the virtual "
		"machine for the \"%1\" device to be connected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_DEV_USB_OPEN_FAILED,
		tr("Unable to find any drivers for this USB device."),
		tr("Please try to update the USB drivers installed on the host computer using Device Manager."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_DEV_USB_CONNECT_FAILED,
		tr("Unable to connect the USB device to your virtual machine."),
		tr("The device cannot be connected now. Please try again later."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_USB_NOT_CONFIGURED,
		tr("Unable to connect the USB device to your virtual machine."),
		tr("Virtual USB controller is not present. Please check USB settings in the virtual machine configuration."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_AUTOSTART_FROM_CURRENT_USER_FORBIDDEN,
		tr("You cannot edit the virtual machine startup options with your current user account."),
		tr("To be able to edit these options, specify the credentials of another user account on the Startup and Shutdown pane."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INCORRECT_PATH,
		tr("The folder \"%1\" does not exist."),
		tr("Specify an existing folder and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_VM_PD3_COMPAT_MODE,
		tr("The virtual machine is running in compatibility mode. A newer version of Parallels Tools needs to be installed. The installation will start automatically."),
			tr("Please wait for the guest operating system to boot and start the Parallels Tools installation, it might take a while. If the installation of Parallels Tools does not start automatically, select Install Parallels Tools on the Virtual Machine menu to start the installation manually."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_VM_PD3_COMPAT_NO_MOUSE,
		tr("The mouse in your virtual machine may work incorrectly. The problem will be solved automatically after the restart."),
		tr("Windows requires some time to initialize the newly found devices after the virtual machine upgrade. During this time, your mouse may work incorrectly. If you need to log in to your virtual machine, use the keyboard."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_VM_BOOTCAMP_MODE,
		tr("You are booting from your Boot Camp partition inside a virtual machine."),
		tr("You will need to reactivate Windows and Microsoft Office. Do not change the virtual machine configuration without necessity as it may require another reactivation."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PET_DSP_EVT_DISP_SHUTDOWN,
		tr("Parallels Service will be stopped."),
		tr("After Parallels Service is stopped, you will not be able to manage your virtual machines."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_ALREADY_RUNNING,
		tr("The virtual machine \"%1\" is running."),
		tr("You or another user have already started this virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DROP_SUSPEND_FOR_HDD_QUEST_CFG_EDITOR,
		tr("The image file %1 emulating hard disk %2 is used by a suspended virtual machine. Are you sure you want to use this image file?"),
		tr("If you click Yes now, all unsaved data of the suspended virtual machine may get lost and the data of the image file may become damaged."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DROP_SUSPEND_FOR_HDD_QUEST_WIZARDS,
		tr("The hard disk image file %1 is used by a suspended virtual machine. Are you sure you want to use this image file?"),
		tr("If you click Yes now, all unsaved data of the suspended virtual machine may get lost and the data of the image file may become damaged."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_SMALL_IMAGE_SIZE,
		tr("Not enough space on virtual hard disk %1."),
		tr("Increase the hard disk size and try again."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENTRY_DOES_NOT_EXIST,
		tr("Unable to complete the operation."),
		tr("The file \"%1\" could not be found. Make sure that this file is available and you have the permission to access it."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENTRY_DIR_DOES_NOT_EXIST,
		tr("Unable to complete the operation for the folder \"%1\"."),
		tr("This folder is no longer available."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENTRY_ALREADY_EXISTS,
		tr("The file \"%1\" already exists."),
		tr("Specify another name for the file and try again."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENTRY_DIR_ALREADY_EXISTS,
		tr("The folder \"%1\" already exists."),
		tr("Specify another name for the folder and try again."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_RENAME_ENTRY,
		tr("Unable to rename the file \"%1\"."),
		tr("Make sure that this file is not used by another application and you have the permission to rename it."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_RENAME_DIR_ENTRY,
		tr("Unable to rename the folder \"%1\"."),
		tr("Make sure that this folder is not used by another application and you have the permission to rename it."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_REMOVE_ENTRY,
		tr("Unable to delete the file \"%1\"."),
		tr("Make sure that this file is not used by another application and you have the permission to delete it."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_REMOVE_DIR_ENTRY,
		tr("Unable to delete the folder \"%1\"."),
		tr("Make sure that this folder is not used by another application and you have the permission to delete it."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_NO_TARGET_PATH_SPECIFIED,
		tr("The file name is not specified."),
		tr("Specify an arbitrary name for the file and try again."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_NO_TARGET_DIR_PATH_SPECIFIED,
		tr("The folder name is not specified."),
		tr("Specify an arbitrary name for the folder and try again."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CREATE_HDD_IMAGE_NO_SPACE,
		tr("Not enough free space on \"%1\"."),
		tr("Free some space on this physical disk or select another location and try again."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP_VM_COMMAND_CANT_BE_EXECUTED,
		tr("Unable to complete the operation."),
		tr("This operation cannot be completed because the virtual machine \"%1\" is in the \"%2\" state."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_IN_WAKING_UP_STATE,
		tr("The virtual machine \"%1\" is waking from sleep."),
		tr("The operation cannot be completed because the virtual machine is waking from sleep. Please wait a moment and try again."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_IN_FROZEN_STATE,
		tr("The operation could not be completed because there was a problem reading the virtual machine location."),
		tr("The hard disk storing the virtual machine \"%1\" is slow. The virtual machine will be paused until disk reading speed is increased."),
		false,
		PMT_CRITICAL);


	REGISTER_ERROR_STRING(
		GUI_QUESTION_CANT_USE_DEVICE_CLIENT_OPTION_ON_HOST,
		tr("Unable to browse the image files on the client computer. Do you want to browse the files on the host computer?" ),
		tr("This device is set to use image files stored on the client computer."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP2DISP_SESSION_ALREADY_AUTHORIZED,
		tr("Unable to authorize the dispatcher session."),
		tr("The dispatcher session has already been authorized. No additional action is required."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP2DISP_WRONG_USER_SESSION_UUID,
		tr("The specified user session ID is invalid."),
		tr("The specified user session ID is invalid or the session was already closed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_CHECKING_PRECONDITIONS_FAILED,
		tr("Unable to migrate the virtual machine."),
		tr("The virtual machine or the source and destination servers do not comply with the migration requirements."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_NOT_ENOUGH_DISK_SPACE_ON_SOURCE,
		tr("Not enough disk space on the source server."),
		tr("The source server has %2 MB of free disk space. Migrating this virtual machine requires %1 MB of free disk space to be available on the source server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_NOT_ENOUGH_CPUS_ON_TARGET,
		tr("Not enough CPUs on the destination server."),
		tr("The destination server has %2 CPUs installed. Migrating this virtual machine requires %1 CPUs to be installed on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_OUT_OF_MEMORY_ON_TARGET,
		tr("The virtual machine cannot be migrated due to an insufficient amount of RAM on the destination server."),
		tr("The destination server has only %2 of RAM installed. Migrating this virtual machine requires %1 MB of RAM to be installed on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_TARGET_VM_HOME_PATH_NOT_EXISTS,
		tr("Unable to find the path to the virtual machine folder on the destination server."),
		tr("The path '%1' does not exist on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_NETWORK_SHARE_IS_ABSENT_ON_TARGET,
		tr("Unable to find the necessary network share on the destination server."),
		tr("The network share '%1' required for the virtual machine migration is not available on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_FLOPPY_DISK_IS_ABSENT_ON_TARGET,
		tr("Unable to find the necessary floppy disk on the destination server."),
		tr("The floppy disk '%1' required for the virtual machine migration is not available on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_OPTICAL_DISK_IS_ABSENT_ON_TARGET,
		tr("Unable to find the necessary CD/DVD drive on the destination server."),
		tr("The CD/DVD drive '%1' required for the virtual machine migration is not available on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_SERIAL_PORT_IS_ABSENT_ON_TARGET,
		tr("Unable to find the necessary serial port on the destination server."),
		tr("The serial port '%1' required for the virtual machine migration is not available on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_PARALLEL_PORT_IS_ABSENT_ON_TARGET,
		tr("Unable to find the necessary printer port on the destination server."),
		tr("The printer port '%1' required for the virtual machine migration is not available on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_NETWORK_ADAPTER_IS_ABSENT_ON_TARGET,
		tr("Unable to find the necessary network adapter on the destination server."),
		tr("The network adapter '%1' required for the virtual machine migration is not available on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_USB_CONTROLLER_IS_ABSENT_ON_TARGET,
		tr("Unable to find the necessary USB controller on the destination server."),
		tr("The USB controller required for the virtual machine migration is not available on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_SOUND_DEVICE_IS_ABSENT_ON_TARGET,
		tr("Unable to find the necessary sound device on the destination server."),
		tr("The sound device '%1' required for the virtual machine migration is not available on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_VM_UUID_ALREADY_EXISTS_ON_TARGET,
		tr("The virtual machine with the UUID %1 already exists on the destination server."),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_NOT_ENOUGH_DISK_SPACE_ON_TARGET,
		tr("Not enough disk space on the destination server."),
		tr("Migrating this virtual machine requires %1 MB of free disk space, while only %2 MB are available on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_COULDNT_DETACH_TARGET_CONNECTION,
		tr("Unable to perform the migration."),
		tr("The virtual machine migration could not be initialized on the destination server. Contact your Parallels Server administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_UNSUITABLE_VM_STATE,
		tr("Unable to perform the migration."),
		tr("Only running virtual machines can be migrated in the hot and warm modes."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_REMOTE_DEVICE_IS_ATTACHED,
		tr("This virtual machine uses a remote device and cannot be migrated."),
		tr("Remove the %1 device from the virtual machine configuration and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_CANNOT_REMOTE_CLONE_SHARED_VM,
		tr("Unable to clone the virtual machine."),
		tr("The virtual machine is stored on a shared storage device and cannot be cloned. To clone this virtual machine, copy it first to your local disk drive."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_ALREADY_DISCONNECTED,
		tr("Unable to disconnect the virtual device."),
		tr("The virtual device that uses the file %1 is already disconnected."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_PRINTER_OVERFLOW,
		tr("Printing to PDF failed, possibly because this file is too big."),
		tr("Try printing a smaller range of pages from the file."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NO_CD_DRIVE_AVAILABLE,
		tr("Unable to install Parallels Tools."),
		tr("Parallels Tools can be installed if at least one CD/DVD drive is connected to the virtual machine. Add a CD/DVD drive to the virtual machine configuration and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_GUEST_TOOLS_NOT_INSTALLED,
		tr("Parallels Tools are not installed in this virtual machine."),
		tr("Start the installation now by choosing Install Parallels Tools from the Virtual Machine menu."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_IPHONE_PROXY_CANNOT_STOP,
		tr("The iPhone service cannot be stopped."),
		tr("An error occurred when stopping the iPhone service."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_IPHONE_PROXY_ALREADY_STARTED,
		tr("The iPhone service has already been started."),
		tr("The iPhone service has already been started. No additional actions are required to start it."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CONFIGURE_PARTITION_HDD,
		tr("One of the physical hard disk partitions is in use and cannot be connected."),
		tr("One of the disk partitions is currently used by the host computer. You need to unmount this partition in the primary OS before connecting it to the virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_IS_NOT_SUSPENDED,
		tr("The virtual machine is not suspended."),
		tr("The requested operation cannot be performed as it is applicable to virtual machines in the suspended state only."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ENABLE_GLOBAL_HOST_SHARING,
		tr("Do you want to enable access to Mac OS X files and folders from Windows?"),
		tr("Click Yes to allow Windows to access files and folders on your Mac.<br>Click No to allow only copying files and folders from your Mac to Windows.<br>You can change these settings later on the Sharing pane of Virtual Machine Configuration."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ENABLE_LOCAL_HOST_SHARING,
		tr("Do you want to enable access to Mac OS X files and folders from Windows?"),
		tr("Click Yes to allow Windows to access the files in your Mac OS X home folder.<br>Click No to allow only copying files and folders from your Mac to Windows.<br>You can change these settings later on the Sharing pane of Virtual Machine Configuration."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_SWITCH_TO_FULLSCREEN,
		tr("The virtual machine is about to enter the full screen mode."),
		tr("Press <b>%1</b> to exit the full screen mode or press <b>%2</b> to release the mouse and keyboard from the virtual machine."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_PRODUCT_ACTIVATED_SUCCESSFULLY,
		tr("The current copy of @PRL_PRODUCT_NAME%1 was successfully activated."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_ANOTHER_TOOLS_IN_USE,
		tr("Parallels Tools cannot be mounted to the virtual machine."),
		tr("Another ISO image with Parallels Tools is mounted to the CD/DVD device of this virtual machine. Unmount this image, then try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_RUN_COMPRESSOR_WIN,
		tr("Parallels Compressor helps you keep your virtual machine compact and efficient."),
		tr("Parallels Compressor is an easy-to-use tool allowing you to clean up unused disk space in the virtual machine. Using this tool, you can significantly reduce the size of your virtual machine hard disks files and use the resources of your physical hard disks more efficiently."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_COPIED,
		tr("Copied"),
		tr("Copied"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_MOVED,
		tr("Moved"),
		tr("Moved"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_COMPRESSOR_UNSUPPORTED_GUEST,
		tr("Parallels Compressor cannot be started."),
		tr("Your virtual machine's operating system is \"%1\". This operating system is not supported by Parallels Compressor."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_REACH_OVERCOMMIT_STATE_ON_SETMEM,
		tr("Setting this memory amount may slow down the performance of your running virtual machines."),
		tr("Your running virtual machines are consuming more memory together than the total amount you are allocating. Increase this amount or shut down or suspend some of your running virtual machines."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_TOTAL_LIMIT,
		tr("The specified total memory limit falls outside the allowed range."),
		tr("The memory limit must be greater than 96 MB and must not exceed the host memory size."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_APPLY_TOTAL_LIMIT,
		tr("The total memory limit is less than the sum of VM guarantees and overhead."),
		tr("The amount of memory used by virtual machines exceeds the specified value. Suspend or shut down some of the running virtual machines and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_ERR_WARN_REACH_OVERCOMMIT_STATE_ON_SETMEM,
		tr("Setting this memory amount may slow down the performance of your running virtual machines."),
		tr("Your running virtual machines are consuming more memory together than the total amount you are allocating. Increase this amount or shut down or suspend some of your running virtual machines."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_APPLY_WHEN_HYP_CANNOT_ALLOC_VMS_MEM,
		tr("Are you sure you want to set this memory amount?"),
		tr("Unable to apply the changes right now because the specified amount of memory is less than the sum of all virtual machine guarantees and overhead.If you click Yes, the changes will take effect only after server restart. Or you can also suspend or stop some of the virtual machines and try again."),
		false,
		PMT_QUESTION
		);


	REGISTER_ERROR_STRING(
		GUI_WNG_STOP_VM_WITH_BOOTCAMP,
		tr("Are you sure you want to stop the virtual machine?"),
		tr("The virtual machine uses the Boot Camp partition as its virtual hard disk. If you stop the virtual machine now, you may lose unsaved data or corrupt the data on the Boot Camp partition. Perform a shutdown from within the virtual machine instead."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SNAPSHOT_VM_WITH_BOOTCAMP,
		tr("A virtual machine snapshot cannot be created."),
		tr("You cannot create snapshots of the virtual machines that use the Boot Camp partition as their virtual hard disk."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_BOOTCAMP_HELPER_INIT_FAILURE,
		tr("An unexpected error occurred when initializing Parallels Tools."),
		tr("The virtual machine is booting from a Boot Camp partition. If you continue, you may encounter problems with Microsoft Windows or Office activation."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_BOOTCAMP_HELPER_OS_UNSUPPORTED,
		tr("The Windows version is not supported by Boot Camp."),
		tr("The virtual machine is booting from a Boot Camp partition. The version of Windows installed in the virtual machine is not officially supported by Boot Camp. If you continue, you may encounter problems with Microsoft Windows or Office activation."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_VIDEO_MEMORY_IS_TOO_LOW,
		tr("Not enough video memory for 3D acceleration."),
		tr("%1 MB of video memory is recommended for 3D acceleration. Do you want to increase the video memory up to %1 MB?"),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_MOUSE_SYNC_OFF_FOR_LINUX_WITH3D,
		tr("SmartMouse is disabled in this virtual machine configuration, which may cause graphical artifacts to appear while working in it."),
		tr("To enable SmartMouse, shut down the virtual machine, open its configuration and edit the Services settings on the Options tab."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_NOT_VALID,
		tr("This product could not be activated because your activation key is invalid."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_EXPIRED,
		tr("This product was activated for a limited period of time that has now expired."),
		tr("Make sure you typed the key correctly or purchase another key."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_NOT_STARTED,
		tr("Your activation key covers a period in the future, so it cannot be validated at the moment."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_WRONG_VERSION,
		tr("This product could not be activated because your activation key is not valid for this version of @PRL_PRODUCT_NAME."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_WRONG_PRODUCT,
		tr("This product could not be activated because your activation key is designed for use with another Parallels product."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_WRONG_PLATFORM,
		tr("This product could not be activated because your activation key is designed for using @PRL_PRODUCT_NAME on another platform."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_WRONG_LANGUAGE,
		tr("This product could not be activated because your activation key is designed for use with another language version."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_WRONG_DISTRIBUTOR,
		tr("This product could not be activated because your activation key distributor does not match that of the product itself."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_IS_NOT_VOLUME,
		tr("This product could not be activated because your activation key is not supported special flags."),
		tr("In this screen, you can enter an activation key only for Parallels Desktop Enterprise Edition."),
		false,
		PMT_WARNING
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_BETA_KEY_RELEASE_PRODUCT,
		tr("This product could not be activated because your activation key is designed for use with the Beta version of @PRL_PRODUCT_NAME."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_RELEASE_KEY_BETA_PRODUCT,
		tr("This product could not be activated because your activation key is not designed for use with the Beta version of @PRL_PRODUCT_NAME."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_BLACKLISTED,
		tr("Your activation key has been disabled due to security reasons: someone might have attempted to use the same activation key."),
		tr("Please contact the Parallels support team for further assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_BLACKLISTED_TO_VM_OPERATION,
		tr("Your activation key has been disabled due to security reasons: someone might have attempted to use the same activation key."),
		tr("Please contact the Parallels support team for further assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VOLUME_LICENSE_EXCEEDED_LIMIT,
		tr("The maximum number of installations available for your activation key has been reached."),
		tr("This key cannot be used with any additional installations. For more information, please contact your system administrator."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_TOO_MANY_VCPUS,
		tr("This virtual machine has more than %1 CPUs and cannot be started."),
		tr("The activation key you used to activate the product allows a maximum of %1 CPUs for each virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_TOO_MANY_MEMORY,
		tr("This virtual machine has more than %1 MB of memory and cannot be started."),
		tr("The activation key you used to activate the product allows a maximum of	%1 MB of memory for each virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_VM_HAS_VTD_DEVICES,
		tr("This virtual machine cannot be started because its configuration includes a directly assigned PCIe device."),
		tr("The activation key you used to activate the product does not allow direct assignment of PCIe devices to virtual machines."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_OFFLINE_PERIOD_EXPIRED,
		tr("The deadline for confirming your activation has expired."),
		tr("To continue using Parallels Desktop, please complete the activation now."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OLD_VERSION_LICENSE_WRONG_LANGUAGE,
		tr("This product could not be activated because the language version of your activation key does not match the language version of the product you are trying to upgrade from."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OLD_VERSION_LICENSE_WRONG_DISTRIBUTOR,
		tr("This product could not be activated because the distributor of your activation key does not match the distributor of the product you are trying to upgrade to."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OLD_VERSION_LICENSE_NOT_VALID,
		tr("The specified key cannot be used to activate the product."),
		tr("Please provide your key for the previous version of the product. The key is usually printed on the CD sleeve or sent to you by e-mail."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_TIMEOUT,
		tr("Could not complete the operation."),
		tr("The operation was canceled due to timeout."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_CANCEL,
		tr("Could not complete the operation."),
		tr("The operation was canceled by the user."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_PROXY_AUTH,
		tr("HTTP proxy authentication required."),
		tr("The operation failed, because the HTTP proxy requires user authentication."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_PROXY,
		tr("HTTP proxy error."),
		tr("The operation failed due to an HTTP proxy error."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_NETWORK,
		tr("Network error."),
		tr("The operation failed due to a network error."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_NOTSUP,
		tr("Wrong object."),
		tr("The operation is not supported for this object."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_WRONG,
		tr("Wrong license."),
		tr("The operation failed because of an attempt to use a wrong license."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_NODATA,
		tr("No data available."),
		tr("The operation failed, because the license contains no data."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_ERR_KA,
		tr("License server error."),
		tr("The operation failed due to an error on the license server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_LOCK,
		tr("Resource locked."),
		tr("Unable to complete the operation, because the resource is locked by another process."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_EACCES,
		tr("Access denied."),
		tr("The operation failed, because access was denied."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_NOENT,
		tr("License does not exist."),
		tr("The operation failed, because there is no license installed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_NOINIT,
		tr("Library not initialized."),
		tr("The operation failed, because the library was not initialized."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_EXIST,
		tr("The entry already exists."),
		tr("The operation failed, because the license already exists."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_IO,
		tr("I/O error."),
		tr("The operation failed due to an I/O error."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_INVAL,
		tr("Invalid parameter."),
		tr("The operation failed, because an invalid parameter was entered."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_NOMEM,
		tr("Not enough memory."),
		tr("The operation failed, because there is not enough memory to perform it."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_FATAL,
		tr("The operation failed due to an unknown error."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_INVALID_HWID,
		tr("Invalid hardware ID."),
		tr("The operation failed, because the specified hardware ID is invalid."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_ACTIVATION_KEY,
		tr("An activation key was entered instead of a license key."),
		tr("The operation failed, because the activation key is already in use."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_VMS_LIMIT_EXCEEDED,
		tr("The number of running virtual machines has reached its maximum."),
		tr("You cannot start any more virtual machines."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_GRACED,
		tr("You've reached the maximum limit of simultaneously running virtual machines."),
		tr("The virtual machine will be stopped in %1 hours."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_UNSUPPORTED_APP_MODE,
		tr("Failed to perform this operation."),
		tr("Use the appropriate Parallels product to manage the license."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_UNSUPPORTED_UPDATE_OP,
		tr("Failed to update the license."),
		tr("Your type of license does not support this operation."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_TASK_ALREADY_RUN,
		tr("Failed to perform this operation because it is already running."),
		tr("Please wait for the operation to finish."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_KA_LICENSE_IS_UP_TO_DATE,
		tr("Unable to update the product license."),
		tr("Your license is already up to date. For more detailed information, please contact the Parallels support team."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_KA_LICENSE_IS_NOT_PROLONGATED_YET,
		tr("Unable to update the product license."),
		tr("Your license has not been renewed. Please renew it and try again. For more detailed information, contact the Parallels support team."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_KA_ACTIVATION_LIMIT_REACHED,
		tr("Unable to activate the product."),
		tr("Your license has reached the activation limit. Please contact the Parallels support team for assistance. "),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_KA_HWID_DOES_NOT_MATCH,
		tr("Unable to update the product license."),
		tr("Your license hardware ID has changed and does not coincide with that on the license server. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_KA_LICENSE_IS_TERMINATED,
		tr("Unable to update or activate the product license."),
		tr("Your license has been terminated on the license server. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_UNSUPPORTED_LICENSE_TYPE_TO_DEACTIVATION,
		tr("Unable to deactivate the product."),
		tr("Your type of license cannot be deactivated."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_WRONG_CONFIRMATION_FORMAT,
		tr("Unable to confirm the product license."),
		tr("Make sure that you typed the confirmation ID correctly. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_WRONG_CONFIRMATION_MESSAGE,
		tr("Unable to confirm the product license."),
		tr("Make sure that you typed the confirmation ID correctly. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_WRONG_CONFIRMATION_SIGNATURE,
		tr("Unable to confirm the product license."),
		tr("Make sure that you typed the confirmation ID correctly. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATE_WRONG_TYPE_OF_ACTIVE_LICENSE,
		tr("Unable to confirm the product license."),
		tr("The current license type does not support this operation. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATE_WRONG_TYPE_OF_ACTIVE_LICENSE,
		tr("Unable to deactivate the product."),
		tr("The current license type does not support this operation. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATE_NO_INSTALLED_LICENSE,
		tr("Unable to confirm the product license."),
		tr("Please activate the product first."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATE_NO_INSTALLED_LICENSE,
		tr("Unable to deactivate the product."),
		tr("The product is not activated."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATE_TRIAL_LICENSE,
		tr("Unable to confirm the product license."),
		tr("Trial activations cannot be confirmed. Please activate the product with a full key."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATE_TRIAL_LICENSE,
		tr("Unable to deactivate the product."),
		tr("The product activated with a trial license cannot be deactivated."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_SERVER_ERROR,
		tr("Unable to confirm the product license."),
		tr("Please try again later or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATION_SERVER_ERROR,
		tr("Unable to deactivate the product."),
		tr("Please try again later or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_SERVER_ACTIVATION_ID_IS_INVALID,
		tr("Unable to confirm the product license."),
		tr("Make sure that you typed the activation ID correctly. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATION_SERVER_ACTIVATION_ID_IS_INVALID,
		tr("Unable to deactivate the product."),
		tr("Please contact the Parallels support team for assistance or deactivate the product license manually. For details, see <a href=\"http://kb.parallels.com/en/115133\">http://kb.parallels.com/en/115133</a>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_SERVER_KEY_IS_INVALID,
		tr("Unable to confirm the product license."),
		tr("There's a problem with your product activation key. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATION_SERVER_KEY_IS_INVALID,
		tr("Unable to deactivate the product."),
		tr("There's a problem with your product activation key. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_SERVER_KEY_IS_BLACKLISTED,
		tr("Unable to confirm the product license."),
		tr("Your product activation key was blacklisted. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATION_SERVER_KEY_IS_BLACKLISTED,
		tr("Unable to deactivate the product."),
		tr("Your product activation key was blacklisted. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_SERVER_HWIDS_AMOUNT_REACHED,
		tr("Unable to confirm the product license."),
		tr("You have reached the maximum number of activations available for your activation key (<a href=\"http://www.parallels.com/eula\">http://www.parallels.com/eula</a>). Please purchase a new product license."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_WRONG_SERVER_RESPONSE,
		tr("Unable to confirm the product license."),
		tr("The operation failed due to a connection problem. Please try again later or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATION_WRONG_SERVER_RESPONSE,
		tr("Unable to deactivate the product."),
		tr("The operation failed due to a connection problem. Please try again later. If the problem persists, contact the Parallels support team for assistance or deactivate the product license manually. For details, see <a href=\"http://kb.parallels.com/en/115133\">http://kb.parallels.com/en/115133</a>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_UNABLE_TO_SEND_REQUEST,
		tr("Unable to confirm the product license."),
		tr("Unable to send the confirmation request. Please try again later or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATION_UNABLE_TO_SEND_REQUEST,
		tr("Unable to deactivate the product."),
		tr("Unable to send the deactivation request. Please try again later. If the problem persists, contact the Parallels support team for assistance or deactivate the product license manually. For details, see <a href=\"http://kb.parallels.com/en/115133\">http://kb.parallels.com/en/115133</a>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_HTTP_REQUEST_FAILED,
		tr("Unable to confirm the product license."),
		tr("The operation failed due to a connection problem. Please try again later or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATION_HTTP_REQUEST_FAILED,
		tr("Unable to deactivate the product."),
		tr("The operation failed due to a connection problem. Please try again later. If the problem persists, contact the Parallels support team for assistance or deactivate the product license manually. For details, see <a href=\"http://kb.parallels.com/en/115133\">http://kb.parallels.com/en/115133</a>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATION_OLD_HWID_NOT_FOUND,
		tr("Unable to deactivate the product."),
		tr("Your product activation key is not registered on the activation server. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_COMMON_SERVER_ERROR,
		tr("Unable to confirm the product license."),
		tr("Please try again later or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEACTIVATION_COMMON_SERVER_ERROR,
		tr("Unable to deactivate the product."),
		tr("Please try again later. If the problem persists, contact the Parallels support team for assistance or deactivate the product license manually. For details, see <a href=\"http://kb.parallels.com/en/115133\">http://kb.parallels.com/en/115133</a>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_SEND_REQUEST,
		tr("Unable to send HTTP request to server."),
		tr("Please try again or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HTTP_REQUEST_FAILED,
		tr("Server returns failed HTTP response."),
		tr("Please try again or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTIVATION_UPDATE_FAILED,
		tr("Unable to confirm the product license."),
		tr("The confirmation could not be registered. Please try again later or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_LICENSE_RESTRICTED_RKU,
		tr("Failed to suspend virtual machines to PRAM."),
		tr("Your license does not support suspending virtual machines to PRAM. The virtual machines will be suspended to the hard disk drive."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_DEFERRED_LICENSE_NOT_FOUND,
		tr("License operation failed."),
		tr("Unable to find the pre-cached license. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_INVALID_FORMAT,
		tr("The virtual disk is invalid."),
		tr("'%1' is not a valid Parallels virtual disk. Specify another disk."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_UNDO_DISKS_MODE,
		tr("Apply the changes to the virtual machine's hard disks?"),
		tr("Click Apply to apply the changes or Discard to revert to the original state of the virtual machine."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_MAC_PHYSICAL_MEMORY_MAP_BUG,
		tr("Starting the virtual machine may cause your Mac to malfunction. Are you sure you want to start it?"),
		tr("If you start the virtual machine now, your Mac may work unstable. It is recommended to update your system to Mac OS X 10.5.5 or later before starting your virtual machines."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CREATE_OS2_GUEST_WITHOUT_FDD_IMAGE,
		tr("The required floppy image file was not found."),
		tr("If you continue, the resulting OS/2 virtual machine may work incorrectly. Please reinstall @PRL_PRODUCT_NAME, then try to create the virtual machine again."),
		true,
		PMT_QUESTION
		);


	REGISTER_ERROR_STRING(
		PET_ANSWER_COMMIT,
		tr("Apply"),
		tr("Apply"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_REVERT,
		tr("Discard"),
		tr("Discard"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_STOP_AND_RESTORE,
		tr("Stop and Restore"),
		tr("Stop and Restore"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_RESTART,
		tr("Restart"),
		tr("Restart"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_RESTORE,
		tr("Restore"),
		tr("Restore"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_RESUME,
		tr("Resume"),
		tr("Resume"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SNAPSHOT_IN_UNDO_DISKS_MODE,
		tr("You cannot work with snapshots in Rollback mode."),
		tr("Disable Rollback mode in the virtual machine configuration to manage snapshots."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SNAPSHOT_IN_SAFE_MODE,
		tr("You cannot work with snapshots in Rollback Mode."),
		tr("Start the virtual machine in the regular way to manage snapshots."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_OPERATION_WAS_CANCELED,
		tr("Operation canceled."),
		tr("The operation has been canceled."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CLONE_OPERATION_CANCELED,
		tr("Operation canceled."),
		tr("The cloning operation was canceled by the user."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HYP_ALLOC_VMS_MEMORY,
		tr("The operation could not be completed."),
		tr("The memory amount you are allocating is not enough for starting another virtual machine. Increase this amount."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_POSSIBLE_OVERRUN,
		tr("Split the virtual hard disk before starting the virtual machine."),
		tr("The capacity of your virtual hard disk is larger than the maximum file size allowed by the filesystem. This may lead to data loss as the virtual disk grows in size. Split the disk by editing its settings in the Virtual Machine Configuration > Hardware > Hard Disk."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_ANSWER_DISCONNECT_ANYWAY,
		tr("Disconnect Anyway"),
		tr("Disconnect Anyway"),
		false,
		PMT_ANSWER
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_VM_DVD_DISCONNECT_LOCKED_DEV,
		tr("The CD/DVD disc is mounted inside the guest operating system."),
		tr("If you disconnect this disc now, the guest operating system will not recognize its absence, which may lead to system errors. It is recommended to eject this disc by means of the guest operating system."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_VM_DISCONNECT_SATA_HDD,
		tr("The SATA hard disk is mounted inside the guest operating system."),
		tr("If you disconnect this disk now, the guest operating system will not recognize its absence, which may lead to system errors. It is recommended to eject this disk by means of the guest operating system."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EDIT_UNABLE_CONVERT_TO_TEMPLATE,
		tr("Shut down the virtual machine before proceeding."),
		tr("You can convert your virtual machine to a template only when the virtual machine is shut down."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EDIT_UNABLE_SWITCH_ON_UNDO_DISKS_MODE,
		tr("Shut down the virtual machine before proceeding."),
		tr("You can enable the Rollback mode only when the virtual machine is shut down."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EDIT_UNABLE_SWITCH_OFF_UNDO_DISKS_MODE,
		tr("Shut down the virtual machine before proceeding."),
		tr("You can disable the Rollback mode only when the virtual machine is shut down."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EDIT_UNABLE_CHANGE_IP_ADDRESS,
		tr("Shut down the virtual machine before proceeding."),
		tr("You can change the virtual machine IP address only when the virtual machine is shut down."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_TOOLS_INSTALL_OTHER,
		tr("Parallels Tools are a set of useful add-in features, like automatic mouse capture and dynamic screen resolution adjustment."),
		tr("To install Parallels Tools, click Continue, make sure your CD/DVD drive is mounted, go to the CD/DVD root directory, and install the available drivers."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_COMPACT_THROUGH_COPY,
		tr("Parallels Compressor will temporarily use the free space on the physical hard disk."),
		tr("Compressing the disk will be performed with the help of the free space on your physical hard disk. Make sure that you do not occupy this space with extra files during the compression."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_COMPACT_DESTRUCTIVE,
		tr("It is highly recommended to make a backup of the virtual machine before the operation."),
		tr("Compressing the disk is sensitive to power failures and similar events. If you have a backup copy of your virtual machine, click OK. Otherwise, click Cancel, shut down the virtual machine, create a backup copy, then run Parallels Compressor again."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_RESTORE_STATE_FAILED,
		tr("Unable to revert to this snapshot."),
		tr("An error occurred while restoring the virtual machine state."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MEMORY_SWAPPING_IN_PROGRESS,
		tr("The operation cannot be performed at the moment."),
		tr("Data synchronization is currently in progress. Try to perform the operation later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NVRAM_FILE_COPY,
		tr("Unable to copy the \"%1\" file to \"%2\"."),
		tr("Make sure that you have the necessary permissions for the source file and the target folder. Verify that the source file is not in use and the target folder is available."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_COPY_VM_INFO_FILE,
		tr("Unable to copy the \"%1\" file to \"%2\"."),
		tr("Make sure that you have the necessary permissions for the source file and the target folder. Verify that the source file is not in use and the target folder is available."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SNAPSHOTS_COPY,
		tr("Unable to copy the snapshot \"%1\" to \"%2\"."),
		tr("Make sure that you have the necessary permissions for the snapshot and the target folder. Verify that the snapshot is not in use and the target folder is available."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FDD_IMAGE_COPY,
		tr("Unable to copy the floppy disk image file \"%1\" to \"%2\"."),
		tr("Make sure that you have the necessary permissions for the source file and the target folder. Verify that the source file is not in use and the target folder is available."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_RESTART_GUEST_FAILED,
		tr("Unable to restart the virtual machine operating system."),
		tr("Check that Parallels Tools are installed in the virtual machine. As an alternative, you can restart the operating system from within the virtual machine."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_USB_HARD_DEVICE_INSERTED,
		tr("A Palm device has been connected to this virtual machine."),
		tr("If you try to synchronize the device, it will be synchronized with the virtual machine. If the Palm device synchronization has already started, you need to stop it by tapping Cancel on the device's screen. Stopping the synchronization may take a while."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_USB_HARD_DEVICE_INSERTED2,
		tr("You need to reconnect the USB device to the host computer."),
		tr("Unplug the USB device from the host computer and plug it back to the same port. The device will be automatically connected to your virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_USB_BUSY,
		tr("The USB device %1 cannot be disconnected from the host computer."),
		tr("To be able to connect this device to the virtual machine, you need to disconnect it from the host computer operating system first,​ which may require administrator rights."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_USB_REUSE,
		tr("\"%1\" USB device is in use. Are you sure you want to connect it to the virtual machine?"),
		tr("If you connect this USB device to this virtual machine, it will be disconnected from the %2 host computer or another virtual machine using it."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_USB_CHANGEPID,
		tr("Do you want to retain USB device \"%1\" for this VM?"),
		tr("If you did not replace this device you might want to leave it connected to VM."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANNOT_PROCESSING_UNDO_DISKS_FOR_INVALID_VM,
		tr("Failed to apply the Rollback mode option to the virtual machine."),
		tr("The virtual machine's hard disk could not be found. All your unsaved data might be lost."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANNOT_PROCESSING_SAFE_MODE_FOR_INVALID_VM,
		tr("Failed to apply the Rollback Mode option to the virtual machine."),
		tr("The virtual machine's hard disk could not be found. All your unsaved data might be lost."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_COHERENCE_NOTIFICATION,
		tr("Your virtual machine is running in Coherence mode."),
		tr("In this mode Windows applications are fully integrated into Mac OS X. Click the @PRL_PRODUCT_NAME icon in the Dock to open the Windows Start menu. For more information on Coherence and other view modes, <a href=\"%1\">read the online help</a> or <a href=\"%2\">watch the demo</a>." ),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SAVE_VM_CATALOG,
		tr("Unable to update the virtual machines registry."),
		tr("The virtual machines registry file is not found or corrupt. Reinstall @PRL_PRODUCT_NAME and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP_CONFIG_WRITE_ERR,
		tr("Unable to update the Parallels Service configuration."),
		tr("The Parallels Service configuration file is not found or corrupt. Reinstall @PRL_PRODUCT_NAME and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_FILE_WRITE_FAILED,
		tr("Unable to update the license registry."),
		tr("The license registry file is not found or corrupt. Reinstall @PRL_PRODUCT_NAME and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_TIMEOUT,
		tr("Operation timeout."),
		tr("The operation could not be completed due to a timeout."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_READ_FAILED,
		tr("Operation failed."),
		tr("An error occurred while reading the disk."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_ENOUGH_DISK_SPACE_TO_XML_SAVE,
		tr("There is not enough free space to save the settings."),
		tr("Free up some space on the physical hard disk and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_SNAPSHOT_NAME_IS_EMPTY,
		tr("The snapshot name cannot be empty."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_CANT_CLOSE_CONVERSION_IN_PROGRESS,
		tr("Cannot close the \"%1\" virtual machine window."),
		tr("The virtual machine is being converted to the new format. Wait until the operation is complete and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_CANT_CLOSE_SNAPSHOTING_IN_PROGRESS,
		tr("Cannot close the \"%1\" virtual machine window."),
		tr("The operation of creating a virtual machine snapshot is currently in progress. Wait until it is complete and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_CANT_CLOSE_REVERTING_IN_PROGRESS,
		tr("Cannot close the \"%1\" virtual machine window."),
		tr("The operation of restoring the virtual machine from its snapshot is currently in progress. Wait until it is complete and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_CANT_CLOSE_DELETING_STATE_IN_PROGRESS,
		tr("Cannot close the \"%1\" virtual machine window."),
		tr("The operation of deleting a virtual machine snapshot is currently in progress. Wait until it is complete and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_BACKUP_FAILED_TOO_LARGE_FOR_FAT,
		tr("Unable to back up the virtual machine."),
		tr("One of the virtual machine's files exceeds 4 GB, which is the maximum allowed size for the FAT32 file system on your destination drive. Select another destination for the backup."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_BACKUP_FAILED_NOT_ENOUGH_SPACE,
		tr("Not enough free space for creating the backup."),
		tr("Free some space on the destination drive and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_BACKUP_FAILED_GENERAL_MSG,
		tr("Failed to back up the virtual machine."),
		tr("There is no more free space for a virtual machine backup. Free some disk space or select another destination for the backup."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_REVERT_TO_SPECIFIED_SNAPSHOT,
		tr("Do you want to proceed?"),
		tr("If you revert to the specified snapshot, you will lose all the changes in the current state of the virtual machine."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_REVERT_TO_LAST_SNAPSHOT,
		tr("Are you sure you want to revert to the previous virtual machine snapshot?"),
		tr("If you revert to this snapshot, you will lose all the changes made to the virtual machine state since the snapshot creation."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_CONFIG_IS_ALREADY_VALID,
		tr("This virtual machine does not need to be restored."),
		tr("The virtual machine configuration is valid and does not need to be restored from backup."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ONLY_ADMIN_OR_VM_OWNER_CAN_OPEN_THIS_SESSION,
		tr("Unable to open new session in this virtual machine."),
		tr("Make sure that you are either logged in as an administrator to the physical server or have the rights to read and modify the virtual machine's folder and its files."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EXEC_GUEST_TOOL_NOT_AVAILABLE,
		tr("Unable to open new session in this virtual machine."),
		tr("Make sure the latest version of Parallels Tools is installed in this virtual machine and it has finished booting."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_GUEST_SESSION_EXPIRED,
		tr("The session has expired."),
		tr("Please open a new session and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EXEC_PROGRAM_NOT_FOUND,
		tr("The program you requested to execute was not found."),
		tr("The program '%1' was not found in the virtual machine. Make sure that you specified the path to it correctly and it is installed in the virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_RESIZER_NOT_FOUND,
		tr("Unable to resize the disk because the resizing tool could not be found."),
		tr("Please reinstall the application or contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_RESIZE_WITH_SNAPSHOTS_NOT_ALLOWED,
		tr("Unable to edit the hard disk properties because it has one or more snapshots."),
		tr("This disk belongs to a virtual machine that has one or more snapshots. Please delete these snapshots and try again."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_ENOUGH_DISK_SPACE_TO_START_VM,
		tr("There's not enough disk space available to start the \"%1\" virtual machine."),
		tr("Free at least %2 MB on the hard disk that stores the \"%1\" virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VNC_SERVER_ALREADY_STARTED,
		tr("The VNC server is already running in this virtual machine."),
		tr("Stop the VNC server running in this virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VNC_SERVER_DISABLED,
		tr("The VNC server cannot be started in this virtual machine."),
		tr("Edit the Remote Display settings in the virtual machine configuration and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VNC_SERVER_AUTOSET_PORT_FAILED,
		tr("Unable to configure the port to be used by the VNC server."),
		tr("All VNC protocol ports might be busy. Stop some of the VNC servers running in the virtual machines and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FAILED_TO_START_VNC_SERVER,
		tr("Unable to start the VNC server in this virtual machine."),
		tr("Change the VNC server port number in the virtual machine configuration and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FAILED_TO_STOP_VNC_SERVER,
		tr("Unable to stop the VNC server running in this virtual machine."),
		tr("Please contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VNC_SERVER_NOT_STARTED,
		tr("The VNC server is not running in this virtual machine."),
		tr("You cannot stop the VNC server, because it is currently not running."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_REGISTERED_VM_PATH_NOT_AVAILABLE,
		tr("The virtual machine \"%1\" is not available anymore."),
		tr("Click Back and search again for the virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_FAILED_TO_START_VNC_SERVER,
		tr("Unable to start the VNC server in this virtual machine."),
		tr("Change the VNC server port number in the virtual machine configuration and try again. If the problem persists, contact the Parallels support team for assistance."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_GUEST_PROGRAM_EXECUTION_FAILED,
		tr("The operation was completed with an error."),
		tr("The operation you performed in the virtual machine was completed with this error: %1: '%2'."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SET_NETWORK_SETTINGS_FAILED,
		tr("The network settings could not be applied."),
		tr("An error occurred while applying the network settings to this virtual machine. Error: %1."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SUSPEND_VM_WITH_VTD,
		tr("Suspend is not available for this virtual machine."),
		tr("You cannot suspend virtual machines that use directly assigned PCIe devices."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONFIGURE_GENERIC_PCI_TASK_ALREADY_RUN,
		tr("A pass-through driver for the directly assigned PCIe device is being installed."),
		tr("Please wait for the pass-through driver installation to finish."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_DEVICE_DOES_NOT_EXIST,
		tr("The %1 PCIe device is not found."),
		tr("Make sure this device is plugged into the host computer or specify another PCIe device."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_VM_VTD_PAUSED,
		tr("Unable to suspend this virtual machine."),
		tr("This virtual machine is paused and has a directly assigned PCIe device in its configuration. Resume the virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CREATE_SNAPSHOT_VM_VTD_PAUSED,
		tr("Unable to create a snapshot of this virtual machine."),
		tr("This virtual machine is paused and has a directly assigned PCIe device in its configuration. Resume the virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_REVERT_SNAPSHOT_VM_VTD_PAUSED,
		tr("Unable to revert to the snapshot."),
		tr("This virtual machine is paused and has a directly assigned PCIe device in its configuration. Resume the virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_VM_VTD_WITH_UNSUPPORTED_SHUTDOWN_TOOL,
		tr("This virtual machine cannot be suspended."),
		tr("This virtual machine uses a directly assigned PCIe device and can be"\
			" suspended only after you install a native driver for this device or update Parallels"\
			" Tools. Please install the native driver or reinstall Parallels Tools and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CREATE_SNAPSHOT_VM_VTD_WITH_UNSUPPORTED_SHUTDOWN_TOOL,
		tr("Unable to create a snapshot of this virtual machine."),
		tr("This virtual machine uses a directly assigned PCIe device. You need"\
			" to install a native driver for this device or update Parallels Tools before you can"\
			" create a snapshot of this virtual machine. Please install the native driver or"\
			" reinstall Parallels Tools and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_REVERT_SNAPSHOT_VM_VTD_WITH_UNSUPPORTED_SHUTDOWN_TOOL,
		tr("Unable to revert to the snapshot."),
		tr("This virtual machine uses a directly assigned PCIe device. You need"\
			" to install a native driver for this device or update Parallels Tools before you can"\
			" revert to this snapshot. Please install the native driver or reinstall Parallels Tools"\
			" and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_VM_VTD_WITH_UNLOADED_SHUTDOWN_TOOL,
		tr("Unable to suspend the virtual machine."),
		tr("This virtual machine can be suspended only after Parallels Tools are loaded because it uses a directly assigned PCIe device. Wait until the guest operating system is fully loaded and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CREATE_SNAPSHOT_VM_VTD_WITH_UNLOADED_SHUTDOWN_TOOL,
		tr("Unable to create a snapshot of this virtual machine."),
		tr("This virtual machine uses a directly assigned PCIe device. You need to wait for the guest operating system and Parallels Tools to be loaded before you can create a snapshot of it."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_REVERT_SNAPSHOT_VM_VTD_WITH_UNLOADED_SHUTDOWN_TOOL,
		tr("Unable to revert to the snapshot."),
		tr("This virtual machine uses a directly assigned PCIe device. You need to wait for the guest operating system and Parallels Tools to be loaded before you can revert to this snapshot."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_VM_VTD_WITH_OUTDATED_SHUTDOWN_TOOL,
		tr("This virtual machine cannot be suspended."),
		tr("This virtual machine uses a directly assigned PCIe device and can be suspended only after Parallels Tools are updated. Please reinstall Parallels Tools and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CREATE_SNAPSHOT_VM_VTD_WITH_OUTDATED_SHUTDOWN_TOOL,
		tr("Unable to create a snapshot of this virtual machine."),
		tr("This virtual machine uses a directly assigned PCIe device. You need to update Parallels Tools before you can create a snapshot of it. Please reinstall Parallels Tools and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_REVERT_SNAPSHOT_VM_VTD_WITH_OUTDATED_SHUTDOWN_TOOL,
		tr("Unable to revert to the snapshot."),
		tr("This virtual machine uses a directly assigned PCIe device. You need to update Parallels Tools before you can revert to this snapshot. Please reinstall Parallels Tools and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_VM_VTD_BY_GUEST_SLEEP_TIMEOUT,
		tr("Unable to suspend this virtual machine."),
		tr("All attempts to put the guest operating system into sleep mode failed. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CREATE_SNAPSHOT_VM_VTD_BY_GUEST_SLEEP_TIMEOUT,
		tr("Unable to create a snapshot of this virtual machine."),
		tr("All attempts to put the guest operating system into sleep mode failed. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_REVERT_SNAPSHOT_VM_VTD_BY_GUEST_SLEEP_TIMEOUT,
		tr("Unable to revert to the snapshot."),
		tr("All attempts to put the guest operating system into sleep mode failed. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SOUND_IN_DEVICE_OPEN_FAILED,
		tr("The sound input device specified in the virtual machine configuration is not found."),
		tr("You can continue working in the virtual machine, but the sound will be disabled."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_HOOK_AFTER_INSTALL_NEED_REBOOT,
		tr("You need to restart your computer to complete the pass-through driver installation."),
		tr("You will be able to use the respective PCIe device with your virtual machines only after you restart the host computer."),
		false,
		PMT_WARNING	);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_HOOK_AFTER_REVERT_NEED_REBOOT,
		tr("You need to restart your computer to complete the removal of the pass-through driver."),
		tr("You will be able to use the respective PCIe device with the host computer only after you restart it."),
		false,
		PMT_WARNING	);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_HOOK_INSTALLATION_FAILED,
		tr("Unable to install the pass-through drivers for the directly assigned PCIe devices."),
		tr("The installation of the pass-through drivers failed. Restart the computer and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_HOOK_REVERT_FAILED,
		tr("Unable to remove the pass-through drivers for the directly assigned PCIe devices."),
		tr("The pass-through drivers could not be removed. Restart the computer and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_HOOK_DEVICE_CURRENTLY_IN_USE,
		tr("Unable to change the settings of the following PCIe device: %1."),
		tr( "This device is currently used by a running virtual machine and cannot be configured. Stop or suspend this virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VTD_INSTALLATION_FAILED_FOR_DEVICES,
		tr("Unable to install the pass-through drivers."),
		tr("Pass-through drivers for the following PCIe devices have not been installed: <br>%1<br>Restart the computer and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VTD_HOOK_DEVICE_CURRENTLY_IN_USE,
		tr("Pass-through drivers for some of the PCIe devices could not be installed or removed because these devices are currently in use."),
		tr("The following PCIe devices are currently in use by your virtual machines: <br>%1<br>"
		   "Stop or suspend the virtual machines that are using these devices and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VTD_REVERT_FAILED_FOR_DEVICES,
		tr("Unable to remove the pass-through drivers."),
		tr("The pass-through drivers for the following PCIe devices could not be removed: <br>%1<br>Restart the computer and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_HOOK_INVALID_CONFIG,
		tr("Unable to remove the pass-through driver."),
		tr("The configuration of this device is damaged. Please restore the original driver by updating it in the Device Manager." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_HOOK_UPDATE_SCRIPT_EXECUTE,
		tr("Unable to configure VT-d devices to work with the virtual machine."),
		tr("Some VT-d devices may work incorrectly. To avoid this problem in future, please reinstall the software."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_START_VM_SINCE_NO_VTD_DRIVER,
		tr("This virtual machine cannot be started because the pass-through driver for the \"%1\" PCIe device it uses is not installed on the host computer."),
		tr("To install a pass-through driver, assign the device to virtual machines in Preferences > Intel VT-d."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_START_VM_SINCE_VTD_DEVICE_ALREADY_USED,
		tr("This virtual machine cannot be started because it shares the \"%1\" PCIe device with another virtual machine that is currently running."),
		tr("To be able to start this virtual machine, you need to shut down the virtual machine that is currently using this PCIe device."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(

		PRL_ERR_MEMORY_ALLOC_ERROR,
		tr("There is not enough memory to run this virtual machine."),
		tr("Please quit some of the applications and processes running on the host computer or edit the memory settings in the virtual machine configuration."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(

		PRL_ERR_CANT_REVERT_VM_SINCE_VTD_DEVICE_ALREADY_USED,
		tr("You cannot revert to this snapshot because this virtual machine shares the \"%1\" PCIe device with another virtual machine that is currently running."),
		tr("To be able to revert to this snapshot, you need to shut down the virtual machine that is currently using this PCIe device."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_PARALLELS_IMAGE_INTERNET_LOCATION,
		tr("The image file \"%1\" cannot be used because it is stored on an Internet location."),
		tr("Please specify another file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_PARALLELS_OUTPUT_FILE_INTERNET_LOCATION,
		tr("The output file \"%1\" cannot be used because it is stored on an Internet location."),
		tr("Please specify another file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_VM_HW_UPGRADING_IN_PROGRESS,
		tr("Cannot close the \"%1\" virtual machine window."),
		tr("The virtual machine is currently being upgraded. Wait until the operation is complete and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_VM_HW_UPGRADING_VMXPHY_IN_PROGRESS,
		tr("Cannot close the \"%1\" virtual machine window."),
		tr("The virtual machine is currently being configured. Wait until the operation is complete and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VM_HW_UPGRADE_UNKNOWN_ERROR,
		tr("Manual interaction is required to complete the upgrading of the \"%1\" virtual machine."),
		tr("For information on how to complete the upgrade manually, follow <a href=\"%1\">this link</a>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VM_HW_UPGRADE_VMXPHY_UNKNOWN_ERROR,
		tr("Manual interaction is required to complete the configuring of the \"%1\" virtual machine."),
		tr("For information on how to complete the configuration manually, follow <a href=\"%1\">this link</a>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VM_HW_UPGRADE_INIT_TIMEOUT,
		tr("You need to log in to Windows to complete upgrading your virtual machine."),
		tr("If autorun is disabled, you will also need to open My Computer and run the Setup.exe file located on the CD, ignoring any Windows notifications. For more information, visit Parallels <a href=\"%1\">website</a>."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VM_HW_UPGRADE_VMXPHY_INIT_TIMEOUT,
		tr("You need to log in to Windows to complete configuring your virtual machine."),
		tr("If autorun is disabled, you will also need to open My Computer and run the Setup.exe file located on the CD, ignoring any Windows notifications. For more information, visit Parallels <a href=\"%1\">website</a>."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_VM_HW_UPGRADE_COMPLETED,
		tr("The virtual machine \"%1\" has been successfully upgraded."),
		tr("You can now use this virtual machine."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_VM_HW_UPGRADE_VMXPHY_COMPLETED,
		tr("The virtual machine \"%1\" has been successfully configured."),
		tr("You can now use this virtual machine."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NO_TEMPLATE_SUPPORT_IN_PLAYER_MODE,
		tr("You are trying to open a virtual machine template."),
		tr("Virtual machine templates cannot be used in Parallels Desktop Express."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NO_PVS_FILE_SPECIFIED_IN_PLAYER_MODE,
		tr("You did not specify the virtual machine file you want to open."),
		tr("Please type the path to the virtual machine file after the --openvm option."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_LEAVE_AUTO_HW_UPGRADING,
		tr("Are you sure you want to switch to the manual upgrade mode for the \"%1\" virtual machine?"),
		tr("It is highly recommended to continue upgrading the virtual machine in automatic mode. For more information on upgrading your virtual machines, <a href=\"%1\">visit the upgrade page</a> on the Parallels website." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_LEAVE_AUTO_HW_UPGRADING_VMXPHY,
		tr("Are you sure you want to switch to the manual configuration mode for the \"%1\" virtual machine?"),
		tr("It is highly recommended to continue configuring the virtual machine in automatic mode. For more information on configuring your virtual machines, <a href=\"%1\">visit the configuration page</a> on the Parallels website." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_RUNNING_UNDER_UNSUPPORTED_WM,
		tr("This application supports only GNOME and KDE desktop environments."),
		tr("Using this application in other desktop environments may result in unexpected behavior."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_REBOOT_HOST_ON_PCI_DRIVER_INSTALL_OR_REVERT,
		tr("You need to restart your computer to apply the changes you made to the Intel VT-d preferences."),
		tr(""),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_DSP_EVT_REBOOT_HOST_MANUAL_TO_INIT_VTD_DEVICES,
		tr("You need to restart the physical server to apply the changes you made to the Intel VT-d preferences."),
		tr("If you need assistance in restarting the server, please contact your system administrator."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_UNCOMMITED_OPERATION,
		tr("The previous operation needs to be completed before you can perform any actions on your snapshots. This operation will be completed automatically."),
		tr("The required action will be performed after the previous operation is completed."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_VM_ROOT_DIRECTORY_NOT_EXISTS,
		tr("The folder \"%1\" does not exist."),
		tr("Do you want to create it?"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_STOP_VM_TO_COMPACT,
		tr("You need to shut down the virtual machine to complete the compression process. Do you want to shut it down now?"),
		tr("If you want to cancel the operation, click Cancel."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_COMPACT_VM_DISKS,
		tr("Unused space on the \"%1\" virtual machine hard disk has significantly increased. Do you want to compress the disk?"),
		tr("When the compression is over, the space initially occupied by the virtual hard disk(s) will be reduced from %2 GB to %3 GB."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_FORCE_COMPACT_VM_DISKS,
		tr("Would you like to compress the disk of the \"%1\" virtual machine?"),
		tr("When the compression is over, the space initially occupied by the virtual machine will be reduced from %2 GB to %3 GB."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_SWITCH_OFF_AUTO_COMPRESS,
		tr("Do you want to disable the AutoCompress feature for the \"%1\" virtual machine?"),
		tr("If you click Yes now, AutoCompress will be disabled. To enable it again, go to Virtual Machine Configuration > Options > Optimization and select the AutoCompress option."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_DELAYED_COMPACT_VM_DISKS,
		tr("You have scheduled the virtual disk compacting. Do you want to start this procedure now?"),
		tr("After compacting the virtual disk, the virtual machine will occupy less space on the physical hard disk drive."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_TOOL_PROCESS_ERROR,
		tr("Unable to process the disk."),
		tr("The disk is corrupt or dynamic or has snapshots. Delete the snapshots and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_TOOL_NOT_FOUND,
		tr("The prl_disk_tool utility is not found."),
		tr("Please reinstall @PRL_PRODUCT_NAME and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP_CANNOT_COMPACT_VM_HARD_DISK,
		tr("The disk cannot be compressed because it is used by the \"%1\" virtual machine that is in Rollback mode, or has snapshots."),
		tr("Please make sure that the virtual machine is not running in Rollback mode is disabled in its configuration, and it has no snapshots."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_COMPACT_HARD_DISK_FAILED,
		tr("Could not compress the Hard Disk %1 used by the \"%2\" virtual machine."),
		tr("The disk is corrupt or has snapshots. Delete the snapshots and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_HARD_DISK_NOT_COMPACTABLE,
		tr("Hard disk %1 used by the virtual machine \"%2\" cannot be compressed."),
		tr("The disk is either plain, or dynamic, or contains no data. Such disks cannot be compressed."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HARD_DISK_NOT_VIRTUAL_OR_DISABLED,
		tr("Hard disk %1 used by the virtual machine \"%2\" cannot be compressed."),
		tr("This disk is either a real hard disk or it is not connected in the virtual machine configuration. Real hard disks and Boot Camp partitions cannot be compressed."),
		false,
		PMT_WARNING
		);

	// Copied error for fast resolving problem (bug #446007)
	REGISTER_ERROR_STRING(
		PRL_ERR_VM_COMPACT_NOT_SUPPORTED_GUEST_OS,
		tr("Parallels Compressor cannot be started."),
		tr("Your virtual machine's operating system is \"%1\". This operating system is not supported by Parallels Compressor."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_VM_STATE,
		tr("The disk used by the \"%1\" virtual machine can be compressed only when it is stopped or running."),
		tr("Please resume the virtual machine or shut it down, and then try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_AUTO_COMPRESS_TASK_ALREADY_RUN,
		tr("The virtual hard disk used by the \"%1\" virtual machine is already being compressed."),
		tr("Please wait until the operation is complete and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_NO_AUTH_TO_CREATE_VM_IN_DIR,
		tr("You do not have the permission to create a virtual machine in the \"%1\" folder."),
		tr("Select another location and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_NO_AUTH_TO_CREATE_ROOT_VM_DIR,
		tr("Unable to create the folder \"%1\"."),
		tr("You might not have permission to create a folder in this location. Select another location and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SHUTDOWN_FAILED,
		tr("Unable to shut down this virtual machine."),
		tr("Try to shut down this virtual machine through the operating system installed in it."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SHUTDOWN_HIBERNATE_NOT_SUPPORTED,
		tr("This virtual machine cannot be suspended."),
		tr("The operating system installed in this virtual machine does not support hibernation."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SHUTDOWN_SUSPEND_NOT_SUPPORTED,
		tr("This virtual machine cannot be suspended."),
		tr("The operating system installed in this virtual machine does not support suspend state."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SHUTDOWN_HIBERNATE_FAILED,
		tr("Unable to put this virtual machine into hibernation."),
		tr("Try to put this virtual machine into hibernation through the operating system installed in it."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SHUTDOWN_MACHINE_LOCKED,
		tr("This virtual machine cannot be shut down because it is locked."),
		tr("Unlock the operating system installed in this virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PREPARE_FOR_HIBERNATE_TASK_ALREADY_RUN,
		tr("The host computer is going to hibernate."),
		tr("Please wait while the running virtual machines are being put into the Stand By mode."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PREPARE_FOR_HIBERNATE_VM_WRONG_STATE,
		tr("Some of the virtual machines cannot be put into the Stand By mode."),
		tr("Only virtual machines that are running or paused can be put into the Stand By mode."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PREPARE_FOR_HIBERNATE_VM_WITHOUT_TOOLS,
		tr("Some of the running virtual machines cannot be put into the Stand By mode because they have no Parallels Tools installed."),
		tr("Install Parallels Tools in the running virtual machines or shut them down."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PREPARE_FOR_HIBERNATE_VM_CANNOT_STAND_BY,
		tr("Some of the running virtual machines cannot be put into the Stand By mode."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SAME_FUNCTION_NET_ASSIGN,
		tr("This PCIe network card has a number of PCIe functions. Changing the assignment for this network card will affect all these functions. Do you want to apply this assignment to all the PCIe functions?"),
		tr("If you click No, the current assignments for these PCIe functions will not change."),
		false,
		PMT_QUESTION
		);
	REGISTER_ERROR_STRING(
		GUI_WNG_NO_FREE_NETWORK_ADAPTERS_FOR_HOST,
		tr("This network adapter is used by the physical server."),
		tr("If you assign this adapter to virtual machines, the physical server network connections will be lost."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_NOT_SYMMETRIC_CPU,
		tr("Your configuration of the CPUs is not symmetric. @PRL_PRODUCT_NAME may work not properly on computers with such configuration."),
		tr("To avoid this problem in future, try to disable the Limit CPUID Value option in BIOS."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILECOPY_PROTOCOL,
		tr("Transport protocol internal error."),
		tr("Package type %1, field %2, value \"%3\"."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILECOPY_DIR_EXIST,
		tr("Transport protocol error."),
		tr("The directory %1 already exists on the destination node."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILECOPY_FILE_EXIST,
		tr("Transport protocol error."),
		tr("The file %1 already exists on the destination node."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILECOPY_CANT_CREATE_DIR,
		tr("Transport protocol error."),
		tr("Unable to create the directory %1 on the destination node."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILECOPY_CANT_OPEN_FILE,
		tr("Transport protocol error."),
		tr("Unable to open the file %1 on the destination node."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILECOPY_CANT_WRITE,
		tr("Transport protocol error."),
		tr("An error occurred while writing on the destination node."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FILECOPY_INTERNAL,
		tr("Transport protocol internal error."),
		tr("%1"),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HTTP_HOST_NOT_FOUND,
		tr("Could not connect to the report server."),
		tr("There might be a problem with your network connection, or the server might be too busy. Wait a while and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HTTP_CONNECTION_REFUSED,
		tr("Could not connect to the report server."),
		tr("The server might be too busy. Wait a while and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HTTP_UNEXPECTED_CLOSE,
		tr("The report server closed the connection."),
		tr("The server might be too busy. Wait a while and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HTTP_INVALID_RESPONSE_HEADER,
		tr("An invalid response was received from the report server."),
		tr("The server might be too busy. Wait a while and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HTTP_WRONG_CONTENT_LENGTH,
		tr("An invalid response was received from the report server."),
		tr("The server might be too busy. Wait a while and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HTTP_PROXY_AUTH_REQUIRED,
		tr("Your problem report could not be sent because of proxy authentication problems."),
		tr("Please check your proxy authentication settings and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HTTP_AUTH_REQUIRED,
		tr("Your report could not be sent because the server you are trying to connect requires authentication."),
		tr("Wait a while and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HTTP_PROBLEM_REPORT_SEND_FAILURE,
		tr("Your problem report could not be sent."),
		tr("Wait a while and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_VM_HOME_ALREADY_EXISTS_ON_TARGET,
		tr("Unable to migrate the virtual machine."),
		tr("The virtual machine home directory '%1' already exists on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SNAPSHOT_NOT_FOUND,
		tr("Unable to find the snapshot ID."),
		tr("The snapshot may have been deleted. If this problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_SNAPSHOTS_CONFIG_NOT_FOUND,
		tr("Unable to open the snapshot configuration file."),
		tr("If this problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_IS_EXCLUSIVELY_LOCKED,
		tr("The virtual machine is currently locked by another session."),
		tr("This virtual machine will become available when it is unlocked or the session is ended."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_IS_NOT_LOCKED,
		tr("This virtual machine is not locked."),
		tr("You do not need to unlock it."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_LOCK_OWNER_SESSION_TRIES_TO_UNLOCK,
		tr("The virtual machine cannot be unlocked in this session."),
		tr("This virtual machine was locked by another user. It can be unlocked only by the user who locked it."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_COMMAND_SUPPORTED_ONLY_AT_SERVER_MODE,
		tr("The operation is not supported."),
		tr("The requested operation is exclusive for the Parallels Server product."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LOGIN_BY_MISMATCH_CLIENT_MODE,
		tr("Unable to log you in."),
		tr("The mode of your application does not match the mode of Parallels Virtualization Service."),
		false,
		PMT_CRITICAL
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_SENTILLION_CLIENT,
		tr("Unathorized access to Parallels Virtualization Service."),
		tr("To perform this action, please use the GUI application designed for this service."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANNOT_SAVE_REMOTE_DEVICE_STATE,
		tr("This virtual machine uses a remote device whose state cannot be changed now."),
		tr("Remote devices can be connected or disconnected only when the virtual machine is running."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_INTERNAL_ERROR,
		tr("Failed to back up the virtual machine due to an internal error."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_INTERNAL_PROTO_ERROR,
		tr("Failed to back up the virtual machine due to an internal protocol error."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_ACCESS_TO_VM_DENIED,
		tr("The virtual machine \"%1\" is not available (%2)."),
		tr("Make sure that you specified the virtual machine's name or UUID correctly and you have enough rights to access it and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_BACKUP_UUID_NOT_FOUND,
		tr("The backup %1 could not be found."),
		tr("Make sure that you specified the backup correctly and you have enough rights to access it and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_CREATE_SNAPSHOT_FAILED,
		tr("Unable to create a backup copy of the virtual machine \"%1\" (%2)."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_SWITCH_TO_SNAPSHOT_FAILED,
		tr("The virtual machine \"%1\" could not revert to the snapshot (%2)."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_REGISTER_VM_FAILED,
		tr("Unable to register the virtual machine \"%1\" (%2)."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_DIRECTORY_ALREADY_EXIST,
		tr("The backup directory \"%1\" already exists."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_CANNOT_CREATE_DIRECTORY,
		tr("Unable to create the directory \"%1\" on the backup server."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_CANNOT_REMOVE_DIRECTORY,
		tr("Unable to remove the directory \"%1\" on the backup server."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_RESTORE_VM_RUNNING,
		tr("Failed to restore the virtual machine \"%1\"."),
		tr("This virtual machine cannot be restored because it is currently running, suspended, paused, or mounted. Shut down the virtual machine and try to restore it again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_BACKUP_NOT_FOUND,
		tr("Could not find any backup of the virtual machine \"%1\"."),
		tr("Make sure that you specified the virtual machine's name or UUID correctly and you have enough rights to access it and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_REQUIRE_LOGIN_PASSWORD,
		tr("Authentication is required to access the backup server."),
		tr("You must type your name and password to access the default backup server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_TOOL_CANNOT_START,
		tr("Unable to start the backup/restore tool."),
		tr("An error occurred while starting the backup/restore tool. Try to restart the physical server. If the problem persists, reinstall the application or contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FAILED_TO_CONNECT_TO_BACKUP_SERVER,
		tr("Unable to connect to the backup server %1."),
		tr("You may experience a connection problem, the backup server may be down, or its name is incorrect."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_FAILED_TO_AUTH_ON_BACKUP_SERVER,
		tr("Unable to connect to the backup server %1 because the user name or password is incorrect."),
		tr("Type a valid user name and password and try again."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_IPC_CREATE_MAP_FAILED,
		tr("File mapping could not be set up."),
		tr("Failed to set up file mapping when creating a shared memory object."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_IPC_ATTACH_FAILED,
		tr("Could not locate the shared memory object."),
		tr("All attempts to locate the shared memory object failed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_IPC_FTOK_FAILED,
		tr("The file name could not be converted to a shared memory key."),
		tr("All attempts to create a shared memory key failed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_MAX_CD_EXCEEDED_WITH_DISABLED_CLIENT_SCSII,
		tr("Unable to change the device placement because SCSI interfaces are not available for client computers."),
		tr("You may set the interface type to IDE and then change the device placement, but all IDE locations are currently taken. You need to free at least one IDE location before you can change the interface type."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_PREPARE_FOR_HIBERNATE_UNABLE_SUSPEND_DO_STOP,
		tr("The virtual machine \"%1\" has been stopped."),
		tr("The virtual machine was forcibly stopped due to an error in its operating system. The error occurred while pausing the virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_CTL_FOR_BACKUP,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("The virtual machine is being backed up. Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_LOCKED_FOR_READING,
		tr("Unable to perform the action because the backup is busy for reading."),
		tr("Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_LOCKED_FOR_WRITING,
		tr("Unable to perform the action because the backup is busy for writing."),
		tr("Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_GET_HDD_IMG_NOT_OPEN,
		tr("The disk you specified cannot be used with Parallels virtual machines."),
		tr("Only virtual hard disks created in @PRL_PRODUCT_NAME or Parallels Transporter are supported."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONFIRMATION_MODE_UNABLE_CHANGE_BY_NOT_ADMIN,
		tr("You must type an administrator's password to be able to perform this action."),
		tr("Please try again or contact your administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONFIRMATION_MODE_ALREADY_ENABLED,
		tr("The confirmation mode is already enabled."),
		tr("You do not need to enable it again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONFIRMATION_MODE_ALREADY_DISABLED,
		tr("The confirmation mode is already disabled."),
		tr("You do not need to disable it again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ADMIN_CONFIRMATION_IS_REQUIRED_FOR_OPERATION,
		tr("The operation failed."),
		tr("To perform this operation, you must enter the host OS administrator's credentials."),
		false,
		PMT_CRITICAL
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_ADMIN_CONFIRMATION_IS_REQUIRED_FOR_VM_OPERATION,
		tr("The operation failed."),
		tr("To perform this operation, you must enter the host OS administrator's credentials."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_RECONNECT_USB_TO_ANOTHER_VM,
		tr("You are trying to connect a USB device that is already connected to another virtual machine (%1)."),
		tr("If you continue, the device will be disconnected from that virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_TRY_DISCONNECT_CONNECTING_USB,
		tr("This USB device is being connected to the virtual machine. Do you want to connect it to the host computer?"),
		tr("If you connect this device to the host computer, you will not be able to use it in the virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANNOT_RUN_SNAPSHOT_OPERATION_ON_VM_BACKUP,
		tr("Cannot perform any operations on snapshots because the virtual machine is being backed up."),
		tr("Please wait until the backup process is complete and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANNOT_RUN_SNAPSHOT_OPERATION_ON_VM_RESTORE,
		tr("Cannot perform any operations on snapshots because the virtual machine is being restored from backup."),
		tr(" Please wait until the restore process is complete and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CHANGESID_FAILED,
		tr("Unable to change the Windows security identifier."),
		tr("The Windows security identifier could not be changed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CHANGESID_GUEST_TOOLS_NOT_AVAILABLE,
		tr("Could not change the Windows security identifier."),
		tr("Parallels Tools might be not installed in the source virtual machine."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_CHANGESID_NOT_AVAILABLE,
		tr("Could not change the Windows security identifier."),
		tr("Make sure the latest version of Parallels Tools is installed in this virtual machine."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_CHANGESID_VM_START_FAILED,
		tr("Unable to change the Windows security identifier."),
		tr("The virtual machine clone could not be started."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_CHANGESID_NOT_SUPPORTED,
		tr("Unable to change the Windows security identifier."),
		tr("The operation is supported only in Windows XP and later versions of Windows."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NETWORK_ADAPTER_NOT_FOUND,
		tr("Could not find the network adapter."),
		tr("The MAC address and VLAN tag you specified do not match any of the host computer's network adapters."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_BRIDGE_NOT_FOUND_FOR_NETWORK_ADAPTER,
		tr("Could not find a bridge for network adapter \"%1\"."),
		tr("Please make sure the bridge is created."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		GUI_QUESTION_REMOVE_BACKUP,
		tr("Are you sure you want to delete the selected backup?"),
		tr("This operation is irreversible."),
		true,
		PMT_QUESTION
		);
	REGISTER_ERROR_STRING(
		GUI_QUESTION_RESTORE_FROM_BACKUP,
		tr("Are you sure you want to restore the virtual machine from backup?"),
		tr("This operation is irreversible. The current state of the virtual machine will be lost."),
		true,
		PMT_QUESTION
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_RESIZE_SIZE_TOO_LOW,
		tr("Unable to resize the disk."),
		tr("The minimum disk size cannot be less than 100 MB. Specify another value and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP_START_VM_FOR_COMPACT_FAILED,
		tr("Unable to compress the virtual hard disk."),
		tr("Please make sure the virtual machine is not suspended, has no snapshots, and the Enable Rollback mode option is disabled in the virtual machine configuration."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_REMOTE_DISPLAY_EMPTY_PASSWORD,
		tr("The password for the Remote Display settings is not specified. Are you sure you want to save the settings?"),
		tr("If you do not specify the password, other users will be able to access this virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CONTINUE_IF_HVT_DISABLED,
		tr("Hardware virtualization support is disabled in BIOS settings. Are you sure you want to continue?"),
		tr("If you do not enable hardware virtualization support, your virtual machines performance may be degraded."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SMP_NOT_SUPPORTED_HVT_NOT_PRESENT,
		tr("Your virtual machine will use only 1 CPU when running."),
		tr("This virtual machine cannot run with 2 or more CPUs, because hardware virtualization is not supported on the host computer."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SMP_NOT_SUPPORTED_HVT_DISABLED,
		tr("Your virtual machine will use only 1 CPU when running."),
		tr("This virtual machine cannot run with 2 or more CPUs, because hardware virtualization is turned off on the host computer."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_RESIZE_FAILED,
		tr("Failed to resize the virtual hard disk."),
		tr("An internal error occurred while resizing the disk."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_ACRONIS_ERR,
		tr("Failed to back up the virtual machine hard disk due to an internal error."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_BACKUP_HAS_NOT_FULL_BACKUP,
		tr("A full backup is being created for this virtual machine."),
		tr("Though an incremental backup is created by default, it cannot be made because the virtual machine has no full backups."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SAFE_MODE_START_DURING_SUSPENDING_SYNC,
		tr("Unable to start the virtual machine in Rollback Mode because the virtual machine suspending is not completed yet."),
		tr("Wait until the virtual machine suspending is completed and try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_VM_VTD_WITH_UNSUPPORTED_CAPS,
		tr("Unable to suspend this virtual machine because it uses a directly assigned PCIe device."),
		tr("This guest operating system does not support suspending virtual machines with PCIe devices assigned."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CREATE_SNAPSHOT_VM_VTD_WITH_UNSUPPORTED_CAPS,
		tr("Unable to create a snapshot because this virtual machine uses a directly assigned PCIe device."),
		tr("This guest operating system does not support creating snapshots of virtual machines with PCIe"\
		" devices assigned."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_REVERT_SNAPSHOT_VM_VTD_WITH_UNSUPPORTED_CAPS,
		tr("Unable to revert to the snapshot because this virtual machine uses a directly assigned PCIe device."),
		tr("This guest operating system does not support reverting to snapshots of virtual machines with PCIe"\
		" devices assigned."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_BACKUP_CMD_FAILED,
		tr("The backup command for the \"%1\" virtual machine failed with code %2 (\"%3\")."),
		tr("Try to back up the virtual machine again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_RESTORE_CMD_FAILED,
		tr("The restore command for the \"%1\" virtual machine failed with code %2 (\"%3\")."),
		tr("Try to restore the virtual machine again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_BACKUP_GUEST_SYNCHRONIZATION_FAILED,
		tr("Unable to synchronize the virtual machine's file system."),
		tr("The backup process will continue without synchronization. To avoid this problem in the future, make sure that Parallels Tools are properly installed in the virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_SNAPSHOT_OF_PAUSED_VM,
		tr("The virtual machine \"%1\" could not be backed up because it is currently paused."),
		tr("Shut down the virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_CANNOT_SET_PERMISSIONS,
		tr("Unable to set the required permissions for the directory \"%1\" on the backup server."),
		tr("Please wait for a while and try again. If the problem persists, contact the server administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNRECOGNIZED_REQUEST,
		tr("Unrecognized request."),
		tr("The remote server does not support the requested operation."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DELETE_VM_IMMEDIATELY,
		tr("The item \"%1\" will be deleted immediately."),
		tr("Are you sure you want to continue?"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_BACKUP_DEVICE_IMAGE_NOT_FOUND,
		tr("The image file \"%1\" used by %2 could not be found."),
		tr("This device will appear as disabled in the virtual machine configuration. You will need to enable it manually."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_GET_FILE_PERMISSIONS,
		tr("You do not have sufficient privileges to perform this operation."),
		tr("Make sure that you have the right to read the virtual machine's files."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANTS_CREATE_DISK_IMAGE_ON_FAT32,
		tr("Virtual hard disks larger than 4 GB cannot be created on FAT32 volumes."),
		tr("Please change the virtual hard disk size or specify another location."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANTS_CREATE_DISK_IMAGE_ON_FAT,
		tr("Virtual hard disks larger than 2 GB cannot be created on FAT volumes."),
		tr("Please change the virtual hard disk size or specify another location."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_PROCESS_IS_NOT_STARTED_BY_WRONG_HOST_SW,
		tr("Unable to start the virtual machine due to software incompatibility."),
		tr("The problem may be caused by an antivirus, firewall, network traffic analyzer, or similar software installed on your computer. For additional information, please see the following knowledgebase article: <a href=\"http://kb.parallels.com/en/6748\">http://kb.parallels.com/en/6748</a>."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CPUUNITS_NOT_SUPPORTED,
		tr("Unable to configure the cpuunits parameter."),
		tr("It is a Virtuozzo-specific parameter and can be configured on Virtuozzo hardware nodes only."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CPULIMIT_NOT_SUPPORTED,
		tr("Unable to configure the cpulimit parameter."),
		tr("It is a Virtuozzo-specific parameter and can be configured on Virtuozzo hardware nodes only."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_IOPRIO_NOT_SUPPORTED,
		tr("Unable to configure the I/O priority."),
		tr("This parameter can be configured only on servers running the Parallels Virtuozzo Containers software."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_SAMPLE_CONFIG_NOT_FOUND,
		tr("Unable to apply sample configuration file."),
		tr("Sample configuration file is not found."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NEED_HEADLESS_MODE_FOR_AUTOSTART,
		tr("For this change to take effect, you must set the Parallels Desktop service to automatically start on Mac startup. Select anyway?"),
		tr("To do so, choose <b>Start Parallels Desktop as a service</b> in General Preferences."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NEED_HEADLESS_MODE_FOR_AUTOSTOP,
		tr("For this change to take effect, you must set the Parallels Desktop service to automatically start on Mac startup. Select anyway?"),
		tr("To do so, choose <b>Start Parallels Desktop as a service</b> in General Preferences."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NEED_HEADLESS_MODE_FOR_KEEP_VM_ALIVE_ON_GUI_EXIT,
		tr("For this change to take effect, you must set the Parallels Desktop service to automatically start on Mac startup. Select anyway?"),
		tr("To do so, choose <b>Start Parallels Desktop as a service</b> in General Preferences."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_VM_NAME_IS_TOO_LONG,
		tr("The specified virtual machine name is too long."),
		tr("The virtual machine name must not exceed %1 characters."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NEED_MORE_MEMORY_TO_ENABLE_HOTPLUG,
		tr("Cannot change memory parameter."),
		tr("The VM must have at least 1GB of RAM when hotplug is on."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CT_IS_RUNNING,
		tr("Unable to complete the operation."),
		tr("This operation cannot be completed because the Container is running."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_IOPSLIMIT_NOT_SUPPORTED,
		tr("Unable to configure the IOPS limit parameter."),
		tr("It is a Virtuozzo-specific parameter and can be configured on Virtuozzo hardware nodes only."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_IOLIMIT_NOT_SUPPORTED,
		tr("Unable to configure the I/O limit."),
		tr("This parameter can be configured only on servers running the Parallels Virtuozzo Containers software."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACTION_NOT_SUPPORTED_FOR_CT,
		tr("Unable to perform the operation."),
		tr("The requested operation is not supported for Containers."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZ_API_NOT_INITIALIZED,
		tr("Unable to perform the operation."),
		tr("The Virtuozzo Containers functionality is not loaded."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_RESTRICTED_OS_VERSION,
		tr("A wrong version of guest operating system is specified."),
		tr("Virtual machines with this type of guest OS cannot be used in this edition of @PRL_PRODUCT_NAME."),
		false,
		PMT_WARNING
		);

	// PMT_QUESTION to allow to skip
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_RESTRICTED_CPU_COUNT,
		tr("The virtual machine will not be started. Are you sure you want to continue?"),
		tr("This virtual machine has %2 CPUs, but your @PRL_PRODUCT_NAME license allows any virtual machine to have no more than %1 CPUs."),
		true,
		PMT_QUESTION
		);

	// PMT_QUESTION to allow to skip
	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_RESTRICTED_MEMORY_SIZE,
		tr("The virtual machine will not be started. Are you sure you want to continue?"),
		tr("This virtual machine has %2 MB of memory, but your @PRL_PRODUCT_NAME license allows any virtual machine to have no more than %1 MB of memory."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_RESTRICTED_SMART_GUARD,
		tr("Could not enable SmartGuard."),
		tr("SmartGuard is not available in this edition of @PRL_PRODUCT_NAME."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_RESTRICTED_UNDO_DISKS,
		tr("Could not enable Rollback mode."),
		tr("Rollback mode is not available in this edition of @PRL_PRODUCT_NAME."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_SET_SATA_FOR_UNSUPPORTED_OS,
		tr("SATA locations are supported for Windows versions starting from Windows Vista. Do you want to change the location for Hard Disk %1?" ),
		tr("If you use an earlier version of Windows, you will have to install SATA drivers manually to be able to use this disk." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CD_DVD_ROM_SET_SATA_FOR_UNSUPPORTED_OS,
		tr("SATA locations are supported for Windows versions starting from Windows Vista. Do you want to change the location for CD/DVD %1?" ),
		tr("If you use an earlier version of Windows, you will have to install SATA drivers manually to be able to use this CD/DVD drive." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_SET_SATA_FOR_OLD_CHIPSET,
		tr("Unable to use the SATA interface for Hard Disk %1." ),
		tr("The virtual machine is obsolete and doesn't support the SATA interface. Choose any other supported interface type for this hard disk." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CD_DVD_ROM_SET_SATA_FOR_OLD_CHIPSET,
		tr("Unable to use the SATA interface for CD/DVD %1." ),
		tr("The virtual machine is obsolete and doesn't support the SATA interface. Choose any other supported interface type for this CD/DVD drive." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_RESTORE_STATE_FAILED_VM_STOPPED,
		tr("Unable to revert to this snapshot."),
		tr("An error occurred while restoring the virtual machine state."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP_TIME_MACHINE_IS_RUNNING,
		tr("Time Machine backup is in progress."),
		tr("Your virtual machines may work slower during the backup."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_TRIAL_EXPIRES_TEXT1,
		tr("Your trial activation key will expire in %1 day."),
		tr("<a href=\"%1\">Purchase a permanent activation key</a> in order to ensure continuous use of the product after the trial period expires."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_TRIAL_EXPIRES_TEXT2,
		tr("Your trial activation key will expire in %1 days."),
		tr("<a href=\"%1\">Purchase a permanent activation key</a> in order to ensure continuous use of the product after the trial period expires."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_TRIAL_EXPIRES_TEXT3,
		tr("Your trial activation key expires today."),
		tr("<a href=\"%1\">Purchase a permanent activation key</a> in order to ensure continuous use of the product after the trial period expires."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_TRIAL_EXPIRES_TEXT4,
		tr("Your trial activation key has expired."),
		tr("<a href=\"%1\">Purchase a permanent activation key</a> to continue using the product."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PCMOVER_NOT_INSTALLED,
		tr("Some components required for migration are missing."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PCMOVER_VAN_FILE_PREPARE_FAILED,
		tr("Could not prepare the migration file."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PCMOVER_MIGRATE_FAILED,
		tr("Migration failed."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMHELPER_CREATEVM_FAILED,
		tr("Could not create the virtual machine."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANNOT_MOVE_DISK_TO_TRASH,
		tr("Unable to move the virtual hard disk file to @PRL_TRASH_NAME."),
		tr("A system error occurred or @PRL_TRASH_NAME folder was deleted or moved."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANNOT_DELETE_DISK_FILES,
		tr("Unable to delete the virtual hard disk file."),
		tr("This file is in use or you do not have enough permissions to delete it."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_MOVE_TO_TRASH_DISK,
		tr("Do you want to keep the virtual hard disk file or move it to @PRL_TRASH_NAME?"),
		tr("Keep the file to be able to use this disk in future. If you move the file to @PRL_TRASH_NAME, the virtual hard disk will be deleted."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_MOVE_TO_TRASH_VM,
		tr("Do you want to keep the \"%1\" files or move them to @PRL_TRASH_NAME?"),
		tr("Keep the files to be able to use this virtual machine in future. If you move the files to @PRL_TRASH_NAME, the virtual machine will be deleted."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANNOT_MOVE_VM_TO_TRASH,
		tr("Unable to move the \"%1\" files to @PRL_TRASH_NAME."),
		tr("A system error occurred or @PRL_TRASH_NAME folder was deleted or moved."),
		true,
		PMT_WARNING
		);

#define LICENSE_RESTRICTED_ERR_STR( err ) \
	REGISTER_ERROR_STRING( err, \
		tr("Unable to complete the operation."), \
		tr("This operation cannot be completed because your license is restricted."), \
		false, \
		PMT_CRITICAL \
		);
	// NOTE:	It is used only by SDK users and prlctl.
	//			So in the future we need to rewrite these messages for each and every case.

	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_GUEST_OS );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_CREATE_VM );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_REGISTER_VM );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_REGISTER_3RD_PARTY_VM );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_CLONE_VM );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_CONVERT_TO_TEMPLATE );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_CONVERT_FROM_TEMPLATE );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_SUSPEND_VM );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_PAUSE_VM );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_SNAPSHOT_CREATE );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_SNAPSHOT_SWITCH );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_SNAPSHOT_DELETE );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_SNAPSHOT_SHOW_TREE );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_UNDODISK_FEATURE );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_SAFEMODE_FEATURE );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_TO_SMARTGUARD_FEATURE );
	LICENSE_RESTRICTED_ERR_STR( PRL_ERR_LICENSE_RESTRICTED_CPU_COUNT );
#undef LICENSE_RESTRICTED_ERR_STR

	REGISTER_ERROR_STRING2(
		PRL_ERR_LICENSE_RESTRICTED_TO_RUNNING_VMS_LIMIT,
		/* BRIEF */
		tr("The number of virtual machines running on this computer has reached its maximum."),
		/* brief singular */
		0, tr(""),
		/* LONG */
		tr("This edition of @PRL_PRODUCT_NAME does not allow you to work with more than %1 virtual machines simultaneously."),
		/* LONG singular */
		1, tr("In this edition of @PRL_PRODUCT_NAME you can work in only one virtual machine at a time."),
		false,
		PMT_CRITICAL );

	REGISTER_ERROR_STRING2(
		PRL_ERR_LICENSE_RESTRICTED_TO_RUNNING_VMS_LIMIT_PER_USER,
		/* BRIEF */
		tr("The number of running virtual machines has reached its maximum."),
		/* brief singular */
		0, tr(""),
		/* LONG */
		tr("This edition of @PRL_PRODUCT_NAME does not allow you to work with more than %1 virtual machines simultaneously."),
		/* LONG singular */
		1, tr("In this edition of @PRL_PRODUCT_NAME you can work in only one virtual machine at a time."),
		false,
		PMT_CRITICAL );

	REGISTER_ERROR_STRING(
		GUI_INFO_FEATURE_RESTRICTED,
		tr("This feature is not available in this edition of Parallels Desktop."),
		tr("This edition contains only features that are necessary for working with Windows XP applications and files on Windows 7."),
		false,
		PMT_INFORMATION );

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_RESUME_FAILED,
		tr("The virtual machine was not migrated, because it could not be resumed on the destination server."),
		tr("Shut down the virtual machine and start the migration again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_BACKUP_SP_GUEST_SYNCHRONIZATION_FAILED,
		tr("Unable to synchronize the file system of the virtual machine, because it is suspended or paused."),
		tr("The backup process will continue without synchronization. To avoid this problem in future, shut down or start the virtual machine before backing it up."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACCESS_TO_VM_HDD_DENIED,
		tr("Unable to access the hard disk used by the \"%1\" virtual machine."),
		tr("You do not have enough rights to use following hard disk in this virtual machine: \"%2\"."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACCESS_TO_CLONE_VM_DEVICE_DENIED,
		tr("Unable to clone a virtual device used by the \"%1\" virtual machine."),
		tr("You do not have enough rights to copy the following device in this virtual machine: \"%2\"."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		GUI_ERR_POST_REPORT,
		tr("Unable to send the problem report."),
		tr("Please check your network settings and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_CONVERT_3RD_PARTY_VM_NO_SPACE,
		tr("Not enough free space on the physical hard disk drive."),
		tr("Free some disk space or select another location for the converted copy of the virtual machine, and try again."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_IMPORT_BOOTCAMP_VM_NO_SPACE,
		tr("Not enough free space on the physical hard disk drive."),
		tr("Free some disk space or select another location for the imported copy of the virtual machine, and try again."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_INVALID_CONFIG,
		tr("The appliance description file contains errors and cannot be used."),
		tr("Repair the file or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_USER_NOT_FOUND,
		tr("You are logged in as %1 and cannot download appliances."),
		tr("Please log in as another user and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_DOWNLOAD_PARENT_PATH_NOT_DIR,
		tr("The download folder \"%1\" is invalid."),
		tr("Please select another folder and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_DOWNLOAD_PARENT_PATH_NOT_EXISTS,
		tr("The folder \"%1\" does not exist."),
		tr("Please select another folder and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_DOWNLOAD_PATH_CANNOT_CREATE,
		tr("The appliance could not be downloaded to the \"%1\" folder."),
		tr("Please make sure that you have the permission to write to this folder."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_DOWNLOAD_UTILITY_NOT_FOUND,
		tr("The download utility \"%1\" could not be found."),
		tr("Please reinstall @PRL_PRODUCT_NAME and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_DOWNLOAD_UTILITY_NOT_STARTED,
		tr("The download utility \"%1\" could not be launched."),
		tr("Please try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_EXIT_WITH_ERROR,
		tr("The appliance was downloaded with an error."),
		tr("Please try to download the appliance again."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PET_QUESTION_BACKUP_RESTORE_NOT_ENOUGH_FREE_DISK_SPACE,
		tr("Not enough disk space to restore the virtual machine. Do you want to restore the virtual machine anyway?"),
		tr("Restoring this virtual machine requires %1 GB of free disk space, while only %2 GB are available on the server."),
		true,
		PMT_QUESTION
		);
	REGISTER_ERROR_STRING(
		PET_QUESTION_BACKUP_CREATE_NOT_ENOUGH_FREE_DISK_SPACE,
		tr("Not enough free disk space on the backup server. Do you want to create the virtual machine backup anyway?"),
		tr("Backing up this virtual machine requires %1 GB of free disk space, while only %2 GB are available on the server."),
		true,
		PMT_QUESTION
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_RESTORE_NOT_ENOUGH_FREE_DISK_SPACE,
		tr("Not enough disk space to restore the virtual machine."),
		tr("Restoring this virtual machine requires %1 GB of free disk space, while only %2 GB are available on the server."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_CREATE_NOT_ENOUGH_FREE_DISK_SPACE,
		tr("Not enough free disk space on the backup server."),
		tr("Backing up this virtual machine requires %1 GB of free disk space, while only %2 GB are available on the server."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_CANNOT_EXTRACT_VM,
		tr("Unable to extract the virtual machine from the downloaded archive \"%1\"."),
		tr("Please try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_INSTALL_ALREADY_IN_PROCESS,
		tr("The virtual appliance is being installed."),
		tr("Please wait until the installation is complete and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_CANNOT_CHANGE_OWNER,
		tr("Could not change the owner to the current user for appliance files folder \"%1\"."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_NAME_NOT_MATCH_VM_BUNDLE,
		tr("The appliance name %1 does not match the name of the virtual machine bundle."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_CANNOT_MOVE_VM_BUNDLE,
		tr("Could not move the virtual machine bundle from the source folder \"%1\" to the destination folder \"%2\"."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_INSTALL_WAS_NOT_STARTED,
		tr("The installation cannot be canceled as it has not yet been started."),
		tr("You need to start the installation to be able to cancel it."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_CANNOT_CALCULATE_MD5,
		tr("Unable to read the appliance file \"%1\"."),
		tr("Try to download the appliance again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_APPLIANCE_MISMATCH_MD5,
		tr("The appliance file is corrupt."),
		tr("Try to download the appliance again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_APPLIANCE_CORRUPTED_INSTALLATION,
		tr("The folder \"%1\" does not exist."),
		tr("Do you want to cancel the download?"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INCONSISTENCY_VM_CONFIG,
		tr("Failed to configure the virtual machine."),
		tr("The virtual machine configuration cannot be saved because some of the specified parameters are invalid. Please check the parameters you specify and try again."),
		false,
		PMT_CRITICAL
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CONNECT_TO_DISPATCHER_ITERATIVELY,
		tr("Unable to establish a connection between \"%1\" and \"%2\"."),
		tr("The network connection between these servers may be broken, or one of the servers may be down. Contact your Parallels Server administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_RESOLVE_HOSTNAME,
		tr("The \"%2\" server is unable to resolve the hostname assigned to the \"%1\" server."),
		tr("Please check your DNS server settings or contact the Parallels server administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_BACKUP_GUEST_UNABLE_TO_SYNCHRONIZE,
		tr("Unable to synchronize the virtual machine's file system because the virtual machine is suspended or paused."),
		tr("The backup process will continue without synchronization. To avoid this problem in the future, please shut down or resume the virtual machine before backup."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PATH_IS_NOT_DIRECTORY,
		tr("\"%1\" is not a folder."),
		tr("Please select another location."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_NO_AUTH_TO_SAVE_BACKUP_FILES,
		tr("You do not have the permission to save the backup files in the folder \"%1\"."),
		tr("Please select another location or contact the Parallels Server administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_ON_QUERY_END_SESSION,
		tr("What do you want to do with your virtual machine before logging out?"),
		tr("Please select <b>Suspend</b>, <b>Shutdown</b>, or <b>Stop</b>. If you click <b>Stop</b>, you will lose all unsaved data. To preserve the unsaved data and put the virtual machine to sleep, click <b>Suspend</b>. To preserve the unsaved data and shut down the virtual machine correctly, click <b>Shutdown</b>."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_ON_QUERY_END_SESSION_RESTRICTED,
		tr("What do you want to do with your virtual machine before logging out?"),
		tr("Please select <b>Shutdown</b> or <b>Stop</b>. If you click <b>Stop</b>, you will lose all unsaved data. To preserve the unsaved data and shut down the virtual machine correctly, click <b>Shutdown</b>."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_RESET_FAILED,
		tr("Unable to reset the virtual machine."),
		tr("Please stop the virtual machine manually and start it again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_ENOUGH_RIGHTS_FOR_LINKED_CLONE,
		tr("You do not have enough rights to create a linked clone of this virtual machine."),
		tr("The virtual machine does not store any hard disks snapshots necessary for creating a linked clone, and you are not allowed to create such snapshots."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_COULDNT_TO_CREATE_HDD_LINKED_CLONE,
		tr("Could not create a linked clone of the virtual hard disk."),
		tr("Could not create a linked clone of virtual hard disk \"%1\" due to an internal error. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONV_HD_WRONG_VM_STATE,
		tr("Unable to convert the disk because the \"%1\" virtual machine is either running or paused."),
		tr("Please shut down the virtual machine or suspend it, and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONV_HD_EXIT_WITH_ERROR,
		tr("The operation could not be completed."),
		tr("An internal error occurred while performing the operation."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONV_HD_NO_ONE_DISK_FOR_CONVERSION,
		tr("There are no virtual hard disks that can be converted."),
		tr("The disk you selected cannot be converted."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONV_HD_DISK_TOOL_NOT_STARTED,
		tr("The Disk Tool utility %1 could not be started."),
		tr("Please try to reinstall Parallels Desktop, or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONV_HD_CONFLICT,
		tr("The conversion settings are specified not correctly."),
		tr("The virtual hard disk can be either plain or expanding by type, and either split or non-split by the type of data organization."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONVERT_3RD_PARTY_VM_FAILED,
		tr("Failed to convert the virtual machine."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONVERT_NO_GUEST_OS_FOUND,
		tr("Failed to convert the virtual machine."),
		tr("The guest OS of the virtual machine could not be identified. You can convert this virtual machine using the prlctl utility with the --force option."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONVERT_EFI_CONFIG_INVALID,
		tr("Unable to convert this virtual machine."),
		tr("The virtual machine EFI configuration is not valid."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONVERT_EFI_GUEST_OS_UNSUPPORTED,
		tr("Unable to convert this virtual machine."),
		tr("@PRL_PRODUCT_NAME doesn't support the EFI boot loader for this guest OS type or architecture."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONVERT_GUEST_OS_IS_HIBERNATED,
		tr("Failed to convert this virtual machine because its guest operating system is hibernated."),
		tr("To forcibly convert the virtual machine, use the prlctl utility with the --force option."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_QUESTION_CONVERT_GUEST_OS_IS_HIBERNATED,
		tr("The guest operating system of this virtual machine is hibernated. Do you want to convert it anyway?"),
		tr("If you click Yes, all unsaved data may be lost in the resulting virtual machine. To avoid possible data loss, fully shut down this virtual machine and try again."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_IMPORT_BOOTCAMP_VM_FAILED,
		tr("Failed to import the virtual machine."),
		tr("An error occurred while trying to import the virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_QUESTION_CONVERT_3RD_PARTY_CANNOT_MIGRATE,
		tr("The virtual machine you are trying to convert is not stopped. Do you want to convert it anyway?"),
		tr("The virtual machine may become unusable after the conversion. It is recommended to stop the virtual machine and try again."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_QUESTION_CONVERT_VM_SPECIFY_DISTRO_PATH,
		tr("The operating system installation disc is required to proceed."),
		tr("Please insert the installation disc for the operating system installed in the virtual machine or specify another installation source."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_QUESTION_CONVERT_VM_CANT_DETECT_OS,
		tr("Failed to detect the operating system installed in the virtual machine. Do you want to convert it anyway?"),
		tr("The resulting virtual machine may become unusable after the conversion."),
		false,
		PMT_QUESTION
		);


	REGISTER_ERROR_STRING(
		PET_QUESTION_VM_REBOOT_REQUIRED_BY_PRL_TOOLS,
		tr("The virtual machine needs to be restarted. Do you want to restart it now?"),
		tr("Restart is required to apply the changes made to the Parallels Tools settings."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_QUESTION_ASK_ENCRYPTED_VM_PASSWORD,
		tr("The \"%1\" virtual machine is protected by password."),
		tr("Please type the password to proceed."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_TEST_TEXT_MESSAGES,
		tr("BRIEF DEFAULT %1."),
		tr("LONG DEFAULT %2."),
		false,
		PMT_CRITICAL );

	REGISTER_ERROR_STRING2(
		PRL_ERR_TEST_TEXT_MESSAGES_PS,
		/* BRIEF */
		tr("BRIEF DEFAULT %1."),
		/* brief singular */
		1, tr("BRIEF SINGULAR %1."),
		/* LONG */
		tr("LONG DEFAULT %2."),
		/* LONG singular */
		2, tr("LONG SINGULAR %2."),
		false,
		PMT_CRITICAL );

	REGISTER_ERROR_STRING(
		PRL_ERR_SET_CPULIMIT,
		tr("Unable to configure the cpulimit parameter."),
		tr("Unable to configure the cpulimit parameter."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SET_CPUUNITS,
		tr("Unable to configure the cpuunits parameter."),
		tr("Unable to configure the cpuunits parameter."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_SET_CPUMASK,
		tr("Unable to configure the cpu mask parameter."),
		tr("Unable to configure the cpu mask parameter."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SET_IOPRIO,
		tr("Unable to configure the ioprio parameter."),
		tr("Unable to configure the ioprio parameter."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SET_IOLIMIT,
		tr("Unable to configure the iolimit parameter."),
		tr("Unable to configure the iolimit parameter."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VZ_OPERATION_FAILED,
		tr("Operation failed."),
		tr("%1"),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VZ_OSTEMPLATE_NOT_FOUND,
		tr("Unable to create the Container."),
		tr("The specified ostemplate is not found"),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_ACTION_REQUESTED,
		tr("This action cannot be performed on the specified object."),
		tr("This action cannot be performed on the specified object."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_GET_SIZE_FAILED,
		tr("Unable to retrieve the virtual hard disk size."),
		tr("Please try again later. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_WRONG_KEY,
		tr("The password you specified is not correct."),
		tr("Please make sure that you typed the password correctly and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_KEY_NOT_SET,
		tr("The operation could not be completed because no password was set for performing it."),
		tr("Please set a password for performing this operation and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_OBJECT_NOT_FOUND,
		tr("The required encryption plug-in could not be found."),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_PASSWORD_TO_ENCRYPTED_VM,
		tr("The password you specified is invalid."),
		tr("Please check the password you typed for the \"%1\" virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_CREATE_ENCRYPTED_VM,
		tr("Only existing virtual machines can be protected by password."),
		tr("You first need to create a virtual machine, and only then can you protect it by password."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_REGISTER_ENCRYPTED_VM_WO_PASSWD,
		tr("The virtual machine you are trying to register is protected by password."),
		tr("Authentication is required to register this virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PROXY_HANDSHAKE_FAILED,
		tr("Unable to connect to the proxy server \"%1\"."),
		tr("An invalid response has been received from the proxy server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PROXY_WRONG_PROTOCOL_VERSION,
		tr("Unable to connect to the proxy server \"%1\"."),
		tr("The version of your Parallels software <i>%2</i> is incompatible with the version of Parallels Proxy Server <i>%3</i>. Synchronize the Parallels Server software and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PROXY_WRONG_PORT_NUMBER,
		tr("Unable to connect to the proxy server \"%1\"."),
		tr("An invalid port number is specified."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PROXY_PEER_NOT_FOUND,
		tr("Unable to connect to the proxy server \"%1\"."),
		tr("Parallels Proxy Server can't find peer host for proxying."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_AUTH_REQUIRED_TO_ENCRYPTED_VM,
		tr("This virtual machine is protected by password."),
		tr("Authentication is required to use this virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_VM_STATE_TO_ENCRYPTED_OP,
		tr("Encryption settings could not be applied."),
		tr("You need to shut down or suspend the virtual machine before applying encryption settings."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_CIPHER_ID_FORMAT,
		tr("Encryption settings could not be applied."),
		tr("Unknown encryption plugin ID. Make sure that the encryption plugin is installed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_CIPHER_ID_FORMAT_IN_CONFIG,
		tr("Encryption settings could not be applied."),
		tr("Unknown encryption plugin ID is specified in the virtual machine configuration. Make sure that the encryption plugin with that ID is installed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_TRANSACTION_STATE,
		tr("Encryption settings could not be applied."),
		tr("Internal error: invalid transaction state."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_PATCH_CONFIG_FILE,
		tr("Encryption settings could not be applied."),
		tr("Unable to change configuration files."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_ROLLBACK_TRANSACTION,
		tr("Encryption settings could not be applied."),
		tr("Unable to rollback transaction."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_COMMIT_TRANSACTION,
		tr("Encryption settings could not be applied."),
		tr("Unable to commit transaction."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_FINALIZE_TRANSACTION,
		tr("Encryption settings could not be applied."),
		tr("Unable to finalize transaction."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PASSWORD_FOR_ENCRYPTED_VM_WAS_CHANGED,
		tr("This virtual machine is protected by password."),
		tr("You need to authenticate yourself again to be able to use this virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_ENOUGH_DISK_SPACE_TO_ENCRYPT_VM,
		tr("Unable to encrypt the \"%1\" virtual machine."),
		tr("Free at least %2 MB on the physical hard disk that stores the \"%1\" virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_ENOUGH_DISK_SPACE_TO_DECRYPT_VM,
		tr("Unable to decrypt the \"%1\" virtual machine."),
		tr("Free at least %2 MB on the physical hard disk that stores the \"%1\" virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_ENCRYPTED_CONFIG_FORMAT,
		tr("Encryption settings could not be applied."),
		tr("One of the configuration files has an invalid format."),
		false,
		PMT_CRITICAL
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_ADD_ENCRYPTED_HDD_TO_NON_ENCRYPTED_VM,
		tr("The disk you are trying to connect is encrypted and cannot be added."),
		tr("Encrypted virtual hard disks cannot be used by non-encrypted virtual machines."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_PASSWORD_TO_ENCRYPTED_HDD,
		tr("The password you specified is invalid."),
		tr("Please check the password and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_ENOUGH_DISK_SPACE_TO_ENCRYPT_HDD,
		tr("Unable to encrypt the virtual hard disk used by the \"%1\" virtual machine."),
		tr("Please free at least %2 MB on the physical hard disk that stores this virtual hard disk, and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_ALREADY_ENCRYPTED,
		tr("Unable to encrypt the \"%1\" virtual machine."),
		tr("This virtual machine is already encrypted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_DECRYPT_UNENCRYPTED_VM,
		tr("Unable to decrypt the \"%1\" virtual machine."),
		tr("This virtual machine is not encrypted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_MEMFILE_ENCRYPT_FAILED,
		tr("Unable to encrypt the virtual machine."),
		tr("The memory files could not be encrypted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_MEMFILE_DECRYPT_FAILED,
		tr("Unable to decrypt the virtual machine."),
		tr("The memory files could not be decrypted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_HDD_TYPE_FOR_ENCRYPT,
		tr("Encryption settings could not be applied."),
		tr("This virtual machine uses a physical hard disk."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CHANGE_DEFAULT_PLUGIN_BY_NON_ADMIN,
		tr("Unable to change the default encryption plugin."),
		tr("This operation is available only for users with administrator's rights."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_SET_DEFAULT_ENCRYPTION_PLUGIN,
		tr("Unable to change the default encryption plugin."),
		tr("Make sure that this encryption plugin installed in the system. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_SET_DEFAULT_ENCRYPTION_PLUGIN_BY_WRONG_FORMAT,
		tr("Unable to change the default encryption plugin."),
		tr("Make sure that you typed the plugin ID correctly. The ID should have the following format: {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx} where 'x' is heximal (UUID format)."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_CLEANUP_BROKEN_TRANSACTIONS,
		tr("Unable to cleanup broken encryption operations."),
		tr("Contact the Parallels support team for assistance if it is necessary."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_COMMIT_BROKEN_TRANSACTION,
		tr("Unable to cleanup broken encryption operations."),
		tr("Contact the Parallels support team for assistance if it is necessary."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_ROLLBACK_BROKEN_TRANSACTION,
		tr("Unable to cleanup broken encryption operations."),
		tr("Contact the Parallels support team for assistance if it is necessary."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_UNABLE_UNLOAD_PLUGINS_IN_USE,
		tr("Unable to switch off the third-party plugins support."),
		tr("One or more plugins are being used by the running virtual machines or by encryption operations."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_PLUGINS_FEATURE_IS_ALREADY_DISABLED,
		tr("Unable to switch off the third-party plugins support."),
		tr("The third-party plugins support is already switched off."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_PLUGINS_FEATURE_IS_DISABLED,
		tr("Unable to perform the operation."),
		tr("This action cannot be performed because the third-party plugins support is switched off."),
		false,
		PMT_CRITICAL
	);

	REGISTER_ERROR_STRING(
		PRL_ERR_SET_DEFAULT_ENCRYPTION_PLUGIN_FEATURE_DISABLED,
		tr("Unable to specify the default encryption plugin."),
		tr("This action cannot be performed because the third-party plugins support is switched off."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_ENABLE_PLUGINS_NOT_PERMITTED,
		tr("Unable to switch on the third-party plugins support."),
		tr("You must have administrator rights to perform this action."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_DISABLE_PLUGINS_NOT_PERMITTED,
		tr("Unable to switch off the third-party plugins support."),
		tr("You must have administrator rights to perform this action."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_RESCAN_PLUGINS_NOT_PERMITTED,
		tr("Unable to rescan the plugins directory."),
		tr("You must have administrator rights to perform this action."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_PLUGINS_FEATURE_IS_ALREADY_ENABLED,
		tr("Unable to switch on the third-party plugins support."),
		tr("The third-party plugins support is already switched on."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_PLUGINS_UNABLE_LOAD_DIR_ALREADY,
		tr("Unable to load the third-party plugins from the directory."),
		tr("This action cannot be performed because the plugins are already loaded."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_PLUGIN_UUID_NOT_FOUND,
		tr("Unable to verify the password for the encrypted virtual machine \"%1\"."),
		tr("The plugin which was used to encrypt this virtual machine is not found. Reload the required plugin and try again. If the problem persists, contact the Parallels support team."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_HDD_CANT_OPEN,
		tr("Encryption settings could not be applied."),
		tr("One of the hard disks could not be opened."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_HDD_IS_ALREADY_ENCRYPTED,
		tr("Encryption settings could not be applied."),
		tr("One of the hard disks is already encrypted."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_HDD_IS_UNENCRYPTED,
		tr("Encryption settings could not be applied."),
		tr("One of the hard disks is already decrypted. "),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_HDD_WRONG_ENCRYPTION_ENGINE,
		tr("Encryption settings could not be applied."),
		tr("The plugin that is used to encrypt one of the hard disks is not found. Reload the required plugin and try again. If the problem persists, contact the Parallels support team."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENC_WRONG_HDD_PASSWORD,
		tr("Encryption settings could not be applied."),
		tr("One of the hard disks was re-encrypted with a different password."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISPLAY_ENCODINGS_NOT_SUPPORTED,
		tr("Runtime display encodings are not supported."),
		tr("Please upgrade your Parallels software."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_AUDIO_ENCODINGS_NOT_SUPPORTED,
		tr("Runtime audio encodings are not supported."),
		tr("Please upgrade your Parallels software."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISPLAY_FORMAT_NOT_SUPPORTED,
		tr("Display buffer conversion format is not supported."),
		tr("Please use a different format."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_OS_TYPE,
		tr("The operating system installed does not match the one specified in the virtual machine configuration." ),
		tr("Please open the virtual machine configuration, click General, and select the proper operating system." ),
		true,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_READ_FAILED,
		tr("An error occurred while reading the file." ),
		tr("Click Report to see details or send a report to Parallels." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRITE_FAILED,
		tr("An error occurred while writing the file." ),
		tr("Click Report to see details or send a report to Parallels." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_OPEN_FAILED,
		tr("An error occurred while opening the file." ),
		tr("Click Report to see details or send a report to Parallels." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_WRITE_FAILED,
		tr("Unable to write to the physical disk."),
		tr("The physical disk is either corrupted or cannot be accessed or there is not enough space on it. Please check the physical disk and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_NOT_SUPPORTED,
		tr("Unable to perform the operation."),
		tr("iSCSI storage management is not supported by this product."),
		false,
		PMT_CRITICAL
		);

	// Internal error
	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_FILE_EXISTS,
		tr("Unable to perform the operation."),
		tr("One of the virtual hard disk files already exists."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_REGISTER_VM_FAILED,
		tr("The virtual machine %1 was not migrated, because it could not be registered on the destination server (%2)."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_TOOL_NOT_AVAILABLE,
		tr("OS Integration tool is not available."),
		tr("OS Integration tool required to perform the operation is not available at the moment."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_SUSPEND_VM_FAILED,
		tr("Failed to migrate the virtual machine %1 because it could not be suspended on the source server (%2)."),
		tr("Try to migrate the virtual machine again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_RESUME_VM_FAILED,
		tr("Failed to migrate the virtual machine %1 because it could not be resumed on the destination server (%2)."),
		tr("Try to migrate the virtual machine again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_VM_ALREADY_MIGRATE_ON_TARGET,
		tr("The operation failed because the virtual machine with UUID %1 is already being migrated."),
		tr("Wait for the previous migration to complete. If this migration fails, try to move the virtual machine again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_GPT_MBR_NOT_EQUAL,
		tr("Unable to perform the operation."),
		tr("A disk partition error is detected on your Boot Camp partition. To learn how to fix this error, please see <a href=\"http://kb.parallels.com/en/11244\">here</a>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_ISIGHT_CAMERA_CONNECT_TO_VM,
		tr("To use iSight camera in Windows, iSight drivers must be installed."),
		tr("Please insert the Mac OS X installation DVD into your Mac - if it launches the Boot Camp installer in Windows, quit it. In Windows, browse to the /Drivers/Apple folder on the DVD and double-click AppleiSightInstaller.exe to install iSight drivers. For details, see <a href=\"%1\">%2</a>."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_BLUETOOTH_USB_CONTROLLER_CONNECT_TO_VM,
		tr("To use Apple Bluetooth USB controller in Windows, Apple Bluetooth drivers must be installed."),
		tr("Please insert the Mac OS X installation DVD into your Mac - if it launches the Boot Camp installer in Windows, quit it. In Windows, browse to the /Drivers/Apple folder on the DVD and double-click AppleBluetoothInstaller.exe to install Apple Bluetooth drivers. For details, see <a href=\"%1\">%2</a>."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_CTRL_ALT_DEL_PRESSED,
		tr("The \"Ctrl+Alt+Delete\" command is processed by both the host and guest operating systems."),
		tr("To send the \"Ctrl+Alt+Delete\" command only to the virtual machine, press Ctrl+Alt+Insert instead."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_HIDDEN_MESSAGES_RESTORED,
		tr("Hidden messages were restored."),
		tr("All warning messages, that were disabled by checking <i>\'Do not show this message again\'</i> option are now enabled, and will be shown the next time they are applicable."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_USER_REGISTRATION_DLG_SEND_OK,
		tr("Thank you for trying @PRL_PRODUCT_NAME"),
		tr("A trial activation key was sent to your e-mail address."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_PORT_FORWARD_OUT_OF_SCOPE,
		tr("The specified port forwarding IP address is invalid."),
		tr("Indicate an IP address that lies within the scope defined in the Start address and End address fields."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SM_ENABLE_SHARED_APPS,
		tr("Before switching to Coherence, you need to enable Shared Applications."),
		tr("Do you want to enable them now and switch to Coherence?"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_INVALID_ADAPTER_ADDRESS,
		tr("Invalid IP address or subnet mask"),
		tr("Either IP address or subnet mask is invalid. Please provide a valid IP address and subnet mask."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_BOOT_CAMP_NEEDS_FREE_SLOT,
		tr("The Boot Camp partition option is unavailable."),
		tr("You cannot connect the Boot Camp partition to the virtual machine because all the IDE and SATA slots are occupied by other devices."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_BOOT_CAMP_NEEDS_FREE_SLOT_IDE,
		tr("The Boot Camp partition option is unavailable."),
		tr("You cannot connect the Boot Camp partition to the virtual machine because all the IDE slots are occupied by other devices."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ISOLATE_VM,
		tr("Are you sure you want to isolate the virtual machine from the host computer?"),
		tr("If you isolate the virtual machine, you will not be able to use Shared Folders, Shared Applications, Shared Profile, SmartMount, and Internet Applications, "\
		"and you will neither be able to copy or move objects between the virtual machine and the host computer."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ISOLATE_VM_LIN,
		tr("Are you sure you want to isolate the virtual machine from the host computer?"),
		tr("If you isolate the virtual machine, you will not be able to use Shared Folders and SmartMount, "\
		"and you will neither be able to copy or move objects between the virtual machine and the host computer."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ENABLE_SHARED_APPS,
		tr("This option requires Windows applications to be shared with Mac. Do you want to share them?"),
		tr("Click Continue to share Windows applications with your Mac."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DISABLE_OPTIONS,
		tr("Disabling this option makes %1 unavailable for this virtual machine."),
		tr("If you choose not to share your Windows applications with Mac, you will automatically disable %1. Do you want to continue?"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_INSTALLATION_CD_IS_NOT_INSERTED,
		tr("The CD/DVD disc is not yet ready."),
		tr("Make sure that the %1 installation disc is inserted into the host's CD/DVD drive and this drive is not used by another virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_REGISTER_VM_WIZ_COMPLETE_PAGE_TITLE_CREATION_FAILED,
		tr("Adding Failed"),
		tr("%1"),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_PARTITION_NOT_SPECIFIED	,
		tr("Hard disk partition wasn't specified."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_PRODUCT_KEY_INCOMPLETE,
		tr("The product key is incomplete."),
		tr("Make sure that all the fields of the product key are filled in completely and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_USER_NAME_NOT_SPECIFIED,
		tr("<nobr>User name was not specified.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_USER_NAME_ILLEGAL,
		tr("The user name you entered is invalid. Note that a user name must start with a lower case letter which can be followed by any combination of numbers and lower case letters."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_USER_NAME_RESERVED_BY_GUEST,
		tr("This user name is reserved by the guest OS. Please try another name."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_USER_PASS_NOT_SPECIFIED,
		tr("<nobr>Password was not specified.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_USER_PASS_DO_NOT_MATCH,
		tr("<nobr>Passwords do not match.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_USER_PASS_TOO_SHORT,
		tr("You entered a password that consists of less than %1 characters, which is considered too weak. You should choose a stronger password."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_LOCAL_PATH_NOT_SPECIFIED,
		tr("<nobr>The local path is not specified.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_LOCAL_PATH_WRONG,
		tr("<nobr>The specified local path is invalid.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_SERVER_NAME_NOT_SPECIFIED,
		tr("<nobr>The server name is not specified.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_SERVER_NAME_WRONG,
		tr("<nobr>The specified server name is invalid.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_SERVER_ALREADY_CONNECTED,
		tr("Connection to this server is already established."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_HOST_IP_ADDRESS,
		tr("<nobr>The specified Parallels adapter IP address is invalid.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_SUBNET_MASK,
		tr("<nobr>The specified subnet mask is invalid.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_DHCP_START_IP_ADDRESS,
		tr("<nobr>The specified DHCP start IP address is invalid.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_DHCP_END_IP_ADDRESS,
		tr("<nobr>The specified DHCP end IP address is invalid.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_DUPLICATE_VIRTUAL_NETWORK,
		tr("<nobr>Two or more virtual networks have the same name. Each of the virtual networks should have a unique name.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_VIRTUAL_NETWORK_WRONG,
		tr("<nobr>One or more virtual networks have invalid names.</nobr>"),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_CANT_REVERT_ABSENT_SNAPSHOT,
		tr("@PRL_PRODUCT_NAME cannot revert to the snapshot because the snapshot doesn't exist."),
		tr("Revert to snapshot - %1"),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_PRODUCT_KEY_WRONG_LENGTH,
		tr("The product key has an incorrect length."),
		tr("Make sure that all the fields of the product key are filled in completely and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SNAPSHOT_SINGLE_DELETE,
		tr("Are you sure you want to delete the snapshot?"),
		tr(""),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SNAPSHOT_DELETE_WITH_CHILDRENS,
		tr("Are you sure you want to delete the snapshot and its children?"),
		tr(""),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_DEVICE_ALREADY_EXIST,
		tr("Device %1 already exists in current configuration."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_SHARED_PROFILE_FOR_BOOTCAMP_VM,
		tr("You have enabled Shared Profile for a Boot Camp virtual machine."),
		tr("If you boot natively from the Boot Camp partition, Shared Profile will be automatically disabled."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_IVALID_VM_UUID_MISMATCH,
		tr("The specified location contains another virtual machine."),
		tr("The path you have specified contains the files that belong to another virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_INVALID_VM_BUNDLE,
		tr("The specified bundle is invalid."),
		tr("The bundle does not contain a valid Parallels virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_INVALID_VM_FOLDER,
		tr("The specified folder is invalid."),
		tr("The folder does not contain a valid Parallels virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANT_START_UTW7_AGENT,
		tr("Could not launch Upgrade Agent."),
		tr("If you have an installation CD for Upgrade to Windows 7, insert it into your optical drive to launch the Upgrade Agent. Otherwise, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ASK_TO_CANCEL_TUTORIAL_DOWNLOAD,
		tr("Are you sure you want to cancel the download?"),
		tr(""),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NET_ADAPTER_EXISTS,
		tr("The specified name is already taken by other virtual network adapter."),
		tr("Specify another name and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NET_ADAPTER_NAME_EMPTY,
		tr("Name cannot be empty."),
		tr("Specify a name for the virtual network adapter and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_PORT_MAPPING_INC_PORT_INCORRECT,
		tr("The incoming port number is incorrect."),
		tr("Specify a number in this range: 1-65535 and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_PORT_MAPPING_VM_PORT_INCORRECT,
		tr("The virtual machine port number is incorrect."),
		tr("Specify a number in this range: 1-65535 and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_PORT_MAPPING_IP_INCORRECT,
		tr("The specified IP address is incorrect."),
		tr("Specify another IP address and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_NO_USB_DEVICES_AVAILABLE,
		tr("No USB devices are available."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_RESET_VIRTUAL_NETWORK,
		tr("Do you want to reset the virtual networks settings now?"),
		tr("Default settings will be used. This action cannot be undone."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UPDATER_NO_FILE,
		tr("One or more files necessary to update the software are missing."),
		tr("You need to reinstall %1 to solve the problem."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UPDATER_NO_CONNECTION,
		tr("Unable to connect to the Parallels Update site."),
		tr("Please check your Internet connection and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NO_INSTALLER_FILE,
		tr("The Installer file is not found."),
		tr("Contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UPDATE_INSTALL_FAILED,
		tr("Unable to install the update."),
		tr("Contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UPDATE_UNABLE_TO_CHECK,
		tr("Unable to check for updates"),
		tr("Contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_VM_DIR_CANT_START_TEMPLATE,
		tr("Do you want to create a new virtual machine from this template?"),
		tr(""),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CONVERT_SUCCEDED,
		tr("You need to start the virtual machine to complete the upgrade process. Do you want to start it now?"),
		tr("Starting the virtual machine will automatically launch the process of upgrading the virtual hardware and Parallels Tools. This process may take some time to complete. During this time, do not restart or switch off your Mac. For more information on upgrading your virtual machines, visit the upgrade page on the <a href=%1>Parallels website</a>."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CONVERT_3RDPARTY_VM_SUCCEDED,
		tr("You need to start the virtual machine to complete the conversion. Do you want to start it now?"),
		tr("Starting the virtual machine will automatically launch the process of upgrading the virtual hardware and Parallels Tools. This process may take some time to complete. During this time, do not restart or switch off your Mac."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WRONG_BACKUP_PATH,
		tr("You cannot save the backup in the virtual machine folder."),
		tr("The contents of this folder will be modified when the virtual machine is converted to the new format."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CONVERT_AND_BACKUP_SUCCEDED,
		tr("You need to start the virtual machine to complete the upgrade process. Do you want to start it now?"),
		tr("Starting the virtual machine will automatically launch the process of upgrading the virtual hardware and Parallels Tools. This process may take some time to complete. During this time, do not restart or switch off your Mac. For more information on upgrading your virtual machines, visit the upgrade page on the <a href=%1>Parallels website</a>.<br/><br/>Note: Click <b>Show backup in Finder</b> to view the location of the original virtual machine."),
		false,
		PMT_QUESTION
		);


	REGISTER_ERROR_STRING(
		GUI_INFO_CONVERT_SUCCEDED_OTHER_OS,
		tr("The virtual machine has been successfully converted."),
		tr("You can now use this virtual machine with @PRL_PRODUCT_NAME."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CONVERT_AND_BACKUP_OTHER_OS_SUCCEDED,
		tr("The virtual machine has been successfully backed up and converted."),
		tr("You can now use this virtual machine with @PRL_PRODUCT_NAME. Click <b>Show backup in Finder</b> to view the location of the original virtual machine."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CONVERT_FAILED,
		tr("Failed to convert the virtual machine."),
		tr("The virtual machine could not be converted to the new format."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_NO_BACKUP,
		tr("Are you sure you want to continue without backing up the virtual machine?"),
		tr("If you click Yes, you will not be able to use this virtual machine with older versions of Parallels Desktop."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DELETE_OLD_VM,
		tr("Delete this virtual machine from the list?"),
		tr("The virtual machine's files will not be deleted."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_NO_BACKUP_FOLDER,
		tr("You have not specified any folder."),
		tr("Please specify a folder to store the backup copy of your virtual machine."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_IMPORT_BOOTCAMP_VM_SUCCEDED_AND_START,
		tr("You need to start the virtual machine to complete the import. Do you want to start it now?"),
		tr("When you start the virtual machine, its virtual hardware and Parallels Tools will be upgraded. The upgrade may take some time to complete, so please do not restart or switch off your Mac during this time. To learn how to reclaim the unused space, click <a href=\"http://kb.parallels.com/en/8703\">here</a>"),
		false,
		PMT_QUESTION
		);


	REGISTER_ERROR_STRING(
		GUI_QUESTION_IMPORT_BOOTCAMP_VM_COMPLETE,
		tr("The import was successful. A virtual machine was created. Do you want to start it now?"),
		tr("To learn how to reclaim the unused space, click <a href=\"http://kb.parallels.com/en/8703\">here</a>"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_CANNOTSTART_LOWMEMORY,
		tr("Unable to switch to Coherence."),
		tr("The amount of video memory set for this virtual machine is too low. Increase the amount and try again."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_STOPPED_LOWMEMORY,
		tr("Coherence mode has been turned off."),
		tr("The amount of video memory set for this virtual machine is too low. Increase the amount and switch to the Coherence mode again."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_CANNOTSTART_SMOFF,
		tr("Unable to switch to Coherence."),
		tr("The Parallels mouse synchronization service is not started. Reinstall Parallels Tools and try again."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_CANNOTSTART_SMDISABLED,
		tr("Unable to switch to Coherence."),
		tr("Shut down the virtual machine, then choose Virtual Machine > Configure > Options > Advanced, and set <b>SmartMouse</b> mode to <b>On</b> or <b>Auto</b>."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_CANNOTSTART_DSPCFGERROR_GUEST,
		tr("Unable to switch to Coherence."),
		tr("The virtual machine display configuration could not be applied. Please try again or contact the Parallels support team for assistance."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_CANNOTSTART_DSPCFGERROR_HOST,
		tr("Unable to switch to Coherence."),
		tr("The information about your display configuration could not be retrieved. Please try again or contact the Parallels support team for assistance."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_STOPPED_DSPCFGERROR_GUEST,
		tr("Coherence mode has been turned off."),
		tr("The virtual machine display configuration could not be applied. Please try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_CANNOTSTART_NOTENOUGHDISPLAYS,
		tr("Unable to switch to Coherence."),
		tr("The amount of video memory set for this virtual machine is too low. Increase the amount and try again."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_CANNOTSTART_SERVERBUSY,
		tr("An error occurred while switching to Coherence. Please try again."),
		tr("If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_CANNOTSTART_LINUX_3D,
		tr("Unable to switch to Coherence."),
		tr("Please disable Linux desktop visual effects. For details, please see <a href=\"http://kb.parallels.com/en/111510\">http://kb.parallels.com/en/111510</a>."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_COHERENCE_STOPPED_LINUX_3D,
		tr("Coherence mode has been turned off."),
		tr("Coherence is not available when Linux desktop visual effects are enabled. For details, please see <a href=\"http://kb.parallels.com/en/111510\">http://kb.parallels.com/en/111510</a>."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UNABLE_START_TRANSPORTER,
		tr("Unable to start %1."),
		tr("Make sure that %1 is installed in %2 on your computer and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UNABLE_START_LYM,
		tr("You can find the Learn Your Mac video tutorial on the @PRL_PRODUCT_NAME installation disc."),
		tr("Copy this video tutorial to the Applications folder on your Mac to be able to watch it without the disc."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_REMAP_VM_KEY_SEQUENCE_ALREADY_USED_BY_VM_SHORTCUT,
		tr("Are you sure you want to remap the keyboard shortcut \"%1\"?"),
		tr("This keyboard shortcut is already used."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REMAP_VM_KEY_SEQUENCE_ALREADY_USED_BY_PD_SHORTCUT,
		tr("This key sequence is already used to perform the \"%1\" action."),
		tr("Before you can configure this key sequence to perform another action, you should assign a different key combination to the \"%1\" action."),
		false,
		PMT_INFORMATION
		);


	REGISTER_ERROR_STRING(
		GUI_INFO_REMAP_PD_KEY_SEQUENCE_ALREADY_USED_BY_VM_SHORTCUT,
		tr("This key sequence is already in use by the shortcut <br>\"%1\"."),
		tr("Configure the shortcut to use another key sequence or remove the shortcut and try again."),
		false,
		PMT_INFORMATION
		);


	REGISTER_ERROR_STRING(
		GUI_INFO_REMAP_PD_KEY_SEQUENCE_ALREADY_USED_BY_PD_SHORTCUT,
		tr("This key sequence is already used to perform the \"%1\" action."),
		tr("Configure the shortcut to use another key sequence."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_REGISTRATION_PRODUCT_ERR_CONNECTION,
		tr("Unable to register @PRL_PRODUCT_NAME on web site."),
		tr("Please check your Internet connection and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_REGISTRATION_PRODUCT_ERR_INVALID_EMAIL,
		tr("Invalid e-mail address."),
		tr("The e-mail address you have specified is invalid. Correct the address and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_REGISTRATION_PRODUCT_ERR_GENERAL_ERROR,
		tr("Unable to register @PRL_PRODUCT_NAME."),
		tr("Check your Internet connection and try again. If the problem persists, follow <a href=\"http://www.parallels.com/en/account/\">this link</a> to register your copy of @PRL_PRODUCT_NAME."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_REGISTRATION_PRODUCT_ERR_INVALID_KEY_ERROR,
		tr("Unable to register @PRL_PRODUCT_NAME."),
		tr("This copy of @PRL_PRODUCT_NAME cannot be registered because it was activated with an invalid product key. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_REGISTRATION_PRODUCT_ERR_TRIAL_KEY_ERROR,
		tr("Unable to register @PRL_PRODUCT_NAME."),
		tr("This copy of @PRL_PRODUCT_NAME cannot be registered because it was activated with a trial activation key. To register @PRL_PRODUCT_NAME, activate it with a permanent activation key."),
		false,
		PMT_CRITICAL
		);


	REGISTER_ERROR_STRING(
		GUI_INFO_REGISTRATION_PRODUCT_ACTIVATED_OK,
		tr("Thank you for registering your copy of @PRL_PRODUCT_NAME."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REGISTRATION_PRODUCT_ACTIVATED_OK_NO_SUPPORT_CODE,
		tr("Your copy of @PRL_PRODUCT_NAME has been successfully registered, but the support code could not be created at the moment. To get the support code later, please log in to your account on the Parallels website and browse to <a href=\"%1\">the support options area</a>."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REGISTRATION_PRODUCT_ALREADY_REGISTERED,
		tr("Your copy of @PRL_PRODUCT_NAME is already registered to this user account."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REGISTRATION_PRODUCT_ALREADY_REGISTERED_NO_SUPPORT_CODE,
		tr("Your copy of @PRL_PRODUCT_NAME is already registered to this user account, but the support code could not be created. To get the support code, please log in to your account on the <a href=\"%1\">Parallels website</a> and browse to the registered products area."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_REGISTRATION_PRODUCT_ALREADY_REGISTERED_ON_ANOTHER_USER,
		tr("This copy of @PRL_PRODUCT_NAME is already registered to another user account."),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REGISTRATION_ACCOUNT_SUCCESSFULLY_UPDATED,
		tr("This key is already registered to your account. Changes to your account information have been applied."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_NEW_PROFILE_NAME_EMPTY,
		tr("The profile name cannot be empty."),
		tr(""),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_PROFILE_NAME_ALREADY_IN_USE,
		tr("A profile with the name of %1 already exists."),
		tr("Specify another name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_PROFILE_DELETE,
		tr("Are you sure you want to delete this profile?"),
		tr("The profile is used by one or more virtual machines. If you delete it, the virtual machine's settings will be reset to default."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DESKTOP_IS_UNREGISTERED,
		tr("This copy of @PRL_PRODUCT_NAME is not registered and cannot be updated. Do you want to register it?"),
		tr("Updates will become available after you register @PRL_PRODUCT_NAME."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_DESKTOP_IS_TRIAL,
		tr("This copy of @PRL_PRODUCT_NAME is activated with a trial activation key and cannot be updated."),
		tr("Updates will become available after you activate @PRL_PRODUCT_NAME with a permanent activation key."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DOWNLOADING_PIS_FAILED,
		tr("%1 could not be downloaded."),
		tr("Please check your network connection and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DOWNLOADING_HOST_ANTIVIRUS_FAILED,
		tr("%1 for Mac could not be downloaded."),
		tr("Please check your network connection and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_INSTALLING_HOST_ANTIVIRUS_FAILED,
		tr("%1 for Mac could not be installed."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_UNINSTALLING_HOST_ANTIVIRUS_SUCCESS,
		tr("%1 for Mac was removed successfully."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UNINSTALLING_HOST_ANTIVIRUS_FAILED,
		tr("%1 for Mac could not be uninstalled."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DOWNLOADING_NATIVE_LOOK_FAILED,
		tr("The MacLook installation file could not be downloaded."),
		tr("Please check your network connection and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NATIVE_LOOK_ERR_TRANSFER_FAILED,
		tr("Could not transfer the MacLook installation file to the virtual machine."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NATIVE_LOOK_ERR_INSTALL_FAILED,
		tr("MacLook could not be installed."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERRNATIVE_LOOK_ERR_CANNOT_START_INSTALL,
		tr("MacLook installation could not be started."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NATIVE_LOOK_ERR_NON_ADMIN,
		tr("You need to be logged in as an administrator to be able to install MacLook."),
		tr("Please log in as an administrator to the virtual machine and try again."),
		false,
		PMT_CRITICAL
		);


	REGISTER_ERROR_STRING(
		GUI_ERR_NATIVE_LOOK_ERR_UNKNOWN,
		tr("Unknown error %1"),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DOWNLOADING_WINDOWS7_LOOK_FAILED,
		tr("The Windows 7 look installation file could not be downloaded."),
		tr("Please check your network connection and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WINDOWS7_LOOK_ERR_TRANSFER_FAILED,
		tr("Could not transfer the Windows 7 look installation file to the virtual machine."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WINDOWS7_LOOK_ERR_INSTALL_FAILED,
		tr("Windows 7 look could not be installed."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WINDOWS7_LOOK_ERR_CANNOT_START_INSTALL,
		tr("Windows 7 look installation could not be started."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WINDOWS7_LOOK_ERR_NON_ADMIN,
		tr("You need to be logged in as an administrator to be able to install the Windows 7 look."),
		tr("Please log in as an administrator to the virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WINDOWS7_LOOK_ERR_UNKNOWN,
		tr("Unknown error %1"),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CHECK_LICENSE_ERR_CONNECTION_FAILED,
		tr("Unable to connect to the server."),
		tr("Please check your Internet connection or try again later."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CHECK_LICENSE_ERR_GENERAL_ERROR,
		tr("An unexpected error has occurred."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CHECK_LICENSE_ERR_INVALID_KEY,
		tr("Sorry, this feature is not available to you, because your activation key is invalid."),
		tr("Please purchase a full activation key to be able to use this feature."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CHECK_LICENSE_ERR_BETTA_KEY,
		tr("Sorry, this feature is not available in the Beta version."),
		tr("Please upgrade to a full version to be able to use this feature."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CHECK_LICENSE_ERR_TRIAL_KEY,
		tr("Sorry, this feature is not available in the trial version."),
		tr("Please purchase a full activation key to be able to use this feature."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_PRODUCT_UPGRADE_DOWNLOAD_FAILED,
		tr("The update could not be downloaded."),
		tr("Please check your network connection and try again."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_PRODUCT_UPGRADE_PURCHASE_ORDER_FAILED,
		tr("The purchase request could not be sent."),
		tr("Please check your network connection and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PPC_INVALID_CREDENTIALS,
		tr("The email or password you entered is invalid."),
		tr("Please check your email and password and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PPC_INVALID_CREDENTIALS_ON_RECONNECT,
		tr("The connection with the @PRL_PRODUCT_NAME server was closed."),
		tr("Make sure you have entered a correct email and password and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PPC_SERVER_BUSY,
		tr("@PRL_PRODUCT_NAME server is busy, please try again later."),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_PPC_UNABLE_TO_CONNECT,
		tr("Unable to connect to the @PRL_PRODUCT_NAME server."),
		tr("Please check your network connection and try again. If the problem persists, contact the <a href=\"%1\">Parallels support</a> team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DHCP_UNKNOWN,
		tr("(DHCP check): Unknown error (%1)."),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NO_INSTALLATION_DISC,
		tr("Make sure that the installation disc is inserted into the optical drive and try again."),
		tr("If you have already inserted the disc, please wait a while before trying again. It may take some time to discover the disc."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UNABLE_TO_RETRIEVE_PASSWORD,
		tr("Unable to retrieve password."),
		tr("Check your Internet connection and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NO_EMAIL_ACCOUNT,
		tr("The email you entered does not belong to any account."),
		tr("If you use multiple email addresses, try entering another email address."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_UNABLE_OPEN_FEEDBACK_DLG,
		tr("Unable to open the feedback dialog."),
		tr("Please check your network connection and try again. If the problem persists, contact the Parallels support team."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_UNABLE_REQUEST_TRIAL_KEY,
		tr("Unable to request a trial key."),
		tr("Please check your network connection and try again. If the problem persists, contact the Parallels support team."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_SOFTWARE_UP_TO_DATE,
		tr("You have the latest version of @PRL_PRODUCT_NAME."),
		tr("There are no new software updates available at this time."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_VM_DECRYPTED_SUCCESSFULLY,
		tr("The virtual machine was successfully decrypted."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_VM_ENCRYPTED_SUCCESSFULLY,
		tr("The virtual machine was successfully encrypted."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_PASSWORD_RESET_ENTER_EMAIL,
		tr("Please enter your email address."),
		tr("We will send you an email with a link to reset your password."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_PASSWORD_RESET_CHECK_EMAIL,
		tr("The instructions on resetting your password have been sent to your email address."),
		tr("Please check your email and follow those instructions."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REGISTRATION_ACCOUNT_ALREADY_USED,
		tr("There is an existing account associated with this email. To register your copy, please log in to this account."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DONT_USE_DIRECT_CONECTION,
		tr("Are you sure you don't want to use direct connection for speeding up Parallels Mobile when possible?"),
		tr("If you disable the direct connection option, Parallels Mobile will not be able to connect directly to your computer."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_KEY_NOT_BE_AUTO_REGISTRED,
		tr("The upgrade key could not be registered automatically."),
		tr("You can register it by logging into your Parallels account at <a href=\"https://www.parallels.com/account/\">https://www.parallels.com/account</a> or by filling out the registration form that will appear the next time you open Parallels Desktop. In both cases, you should provide the email you used to register the previous version."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_ENCRYPTED_DISK_NOT_ADDED,
		tr("The disk you are trying to connect is encrypted and cannot be added."),
		tr("Encrypted virtual hard disks cannot be used  by non-encrypted virtual machines."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_SOURCE_FOR_DEVICE_IS_USED,
		tr("The specified source %1 is already used by %2."),
		tr("Specify any other available source for this device or change the source for %1 and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_UNATTENDED_LINUX_NO_IMAGE,
		tr("Could not create the optical disc image required for unattended installation of Linux."),
		tr("The installation will proceed in standard mode. Manual intervention will be required at certain steps."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_UNATTENDED_WINDOWS_NO_IMAGE,
		tr("Could not create the floppy disk image required for unattended installation of Windows."),
		tr("The installation will proceed in standard mode. Manual intervention will be required at certain steps."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REG_OK,
		tr("Thank you for registering your copy of @PRL_PRODUCT_NAME."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REG_INVALID_KEY,
		tr("This copy of @PRL_PRODUCT_NAME cannot be registered because it was activated with an invalid product key. Please follow the link to learn more: <a href=\"%1\">Parallels Knowledge Base</a>."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REG_INVALID_EMAIL,
		tr("The e-mail address you have specified is invalid. Correct the address and try again."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REG_ALREADY_REGISTERED,
		tr("Your copy of @PRL_PRODUCT_NAME is already registered to this user account."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REG_REQUIRED_FIELDS,
		tr("Not all required fields are filled."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REG_INVALID_USER,
		tr("Your activation key for @PRL_PRODUCT_NAME is already registered by another user. Please contact the Parallels support team for assistance."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REG_CONNECTION,
		tr("Please check your Internet connection and try again."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_INVALID_LOGIN_OR_PASS,
		tr("The email or password you entered is invalid. Please check your email and password and try again."),
		tr(""),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REG_GENERAL_ERROR,
		tr("Check your Internet connection and try again. If the problem persists, follow <a href=\"%1\">this link</a> to register your copy of @PRL_PRODUCT_NAME."),
		tr(""),
		false,
		PMT_INFORMATION
		);
	REGISTER_ERROR_STRING(
		GUI_QUESTION_SHARED_GUEST_APPS_OPEN_DOC_NEED_GUEST_SHARING,
		tr("Parallels Desktop is going to enable sharing Windows disks with Mac OS X."),
		tr("%1"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SHARED_GUEST_APPS_OPEN_DOC_NEED_GLOBAL_SHARING,
		tr("Enable Global Sharing to access file \"%1\" from guest OS?"),
		tr("You need to enable Global Sharing to access file \"%1\" from guest OS."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SHARED_GUEST_APPS_OPEN_DOC_NEED_LOCAL_SHARING,
		tr("Enable Local Sharing to access file \"%1\" from guest OS?"),
		tr("You need to enable Local Sharing to access file \"%1\" from guest OS."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SHARED_GUEST_APPS_OPEN_DOC_NEED_USER_SHARED_FOLDERS,
		tr("Enable the user-defined shared folder \"%1\"?"),
		tr("You need to enable the user-defined shared folder \"%1\" in order to access the \"%2\" file from inside the virtual machine."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SMART_SELECT_ASK_TO_ENABLE_FOR,
		tr("Enable the SmartSelect?"),
		tr("It is required to enable SmartSelect to edit file associations. Do you want to do it now?"),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_SHARED_HOST_APPS_ASK_TO_ENABLE_DLG,
		tr("Do you want to share Mac OS X applications with Windows?"),
		tr("To open a Mac application from the virtual machine, the <b>Share Mac OS X applications with Windows</b> option must be enabled in the virtual machine configuration."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_SHARED_GUEST_APPS_OPEN_DOC_NEED_ADD_USER_SHARED_FOLDER,
		tr("To be able to access \"%1\" from the virtual machine, you need to share the folder containing this file with the virtual machine."),
		tr("To add a shared folder, open the virtual machine configuration, choose Options > Shared Folders, and specify the folder you want to share."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CANCEL_INSTALL_SUSUPEND,
		tr("Do you want to stop or suspend the installation?"),
		tr("If you stop the installation, your progress will be lost. If you suspend it, you can continue later."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CANCEL_INSTALL_DELETE,
		tr("Are you sure you want to stop the installation?"),
		tr("If you stop the installation, your progress will be lost."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DELETE_CREATED_VM,
		tr("Are you sure you want to stop the New Virtual Machine Assistant?"),
		tr("If you stop, your progress will be lost."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_MANUAL_INSTALL,
		tr("Are you sure you want to continue the installation manually?"),
		tr("The New Virtual Machine Assistant will quit and you can continue the installation inside the virtual machine. This method can be useful for more experienced users having problems with the installation."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_INVALID_VM_NOT_AVAILABLE,
		tr("The virtual machine is not available."),
		tr("The virtual machine's files have been moved or deleted. If the files are on an external device, reconnect the device. You can also remove the virtual machine from the list or manually locate its files."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_INVALID_VM_CORRUPTED,
		tr("The virtual machine is corrupted."),
		tr("The virtual machine cannot be used because its files are corrupted."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CONTACT_ADMIN_FOR_ASSISTANCE,
		tr("Please contact your system administrator for assistance."),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_XML_DIFFERS_FROM_REAL,
		tr("Configuration of this Boot Camp virtual machine doesn't match that of the real Boot Camp partition."),
		tr("Please try to delete this virtual machine and then create a new Boot Camp virtual machine from scratch."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_XML_PARTITION_NOT_FOUND,
		tr("Configuration of this Boot Camp virtual machine doesn't match that of the real Boot Camp partition."),
		tr("Please try to delete this virtual machine and then create a new Boot Camp virtual machine from scratch."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CT_MIGRATE_INTERNAL_ERROR,
		tr("Failed to migrate the Container."),
		tr("An internal error occurred when performing the operation. Try to migrate the Container again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_COPY_CT_TMPL_INTERNAL_ERROR,
		tr("Failed to copy the Container template."),
		tr("An internal error occurred when performing the operation. Try to migrate the template again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VALID_FAILURE,
		tr("The specified virtual network is invalid."),
		tr("Make sure that the virtual network name is correct and try again. If the error persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VALID_WRONG_NET_MASK,
		tr("The specified subnet mask is invalid."),
		tr("Make sure that the format of the subnet mask is correct (xxx.xxx.xxx.xxx) and the network defined by this mask can have 16 or more host IP addresses."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VALID_WRONG_HOST_IP_ADDR,
		tr("The server IP address is invalid."),
		tr("Make sure that the IP address assigned to the Parallels server adapter is correct and try again. If the error persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VALID_WRONG_DHCP_IP_ADDR,
		tr("The DHCP server IP address is invalid."),
		tr("An internal error occurred when performing the operation. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VALID_DHCP_RANGE_WRONG_IP_ADDRS,
		tr("The specified DHCP scope is invalid."),
		tr("Make sure that the start and end IP addresses of the DHCP scope are valid and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VALID_MISMATCH_DHCP_SCOPE_MASK,
		tr("The start and end IP addresses of the DHCP scope are in different subnets."),
		tr("Make sure that the start and end IP addresses belong to the same subnet and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VALID_DHCP_SCOPE_RANGE_LESS_MIN,
		tr("The specified DHCP scope is invalid."),
		tr("Make sure that the DHCP scope includes 16 or more IP addresses and try again"),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_START_VM_BY_NOT_DEFINED_USER,
		tr("The virtual machine could not be started."),
		tr("Please make sure that the user is specified in the virtual machine configuration."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_START_VM_BY_NOT_AUTH_USER,
		tr("Unable to authenticate the user."),
		tr("Please make sure that you typed the user name and password correctly."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_LINK_TO_OUTSIDED_HDD_WILL_BE_DROPPED_AFTER_CLONE,
		tr("Hard Disk %1 will be disconnected from the clone of the \"%2\" virtual machine."),
		tr("If you want to use this hard disk with the clone, you should copy it or connect it manually."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_OUTSIDED_HDD_WILL_BE_ONLY_LINKED_AFTER_CLONE,
		tr("When the cloning is complete, Hard Disk %1 will be used both by the \"%2\" virtual machine and its clone."),
		tr("If you want the clone to use a separate hard disk, please create a copy of Hard Disk %1 and connect it manually to the clone."),
		false,
		PMT_WARNING
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_COULD_NOT_START_FIREWALL_TOOL,
		tr("The system firewall application could not be started."),
		tr("Make sure that a firewall application is installed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FIREWALL_TOOL_EXECUTED_WITH_ERROR,
		tr("The system firewall application was closed due to an error."),
		tr("Make sure that the firewall settings are set correctly."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANNOT_FIND_ASSINGED_TO_VMS_PCI_DEVICE,
		tr("You cannot add a PCI device of this type to the virtual machine because no such devices are assigned to virtual machines."),
		tr("Go to File > Preferences > Intel VT-d, assign at least one PCI device of this type to virtual machines, and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANNOT_EDIT_HARDWARE_FOR_PAUSED_VM,
		tr("The \"%1\" virtual machine is paused. You cannot change its hardware configuration."),
		tr("To be able to change the hardware configuration, please resume the virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MOUNT,
		tr("The virtual machine could not be mounted."),
		tr("%1"),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_UNMOUNT,
		tr("The virtual machine could not be unmounted."),
		tr("%1"),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONNECT_TO_MOUNTER,
		tr("Unable to connect to the mount daemon."),
		tr("Make sure the mount daemon \"prl_mounterd\" is running and try again."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_PERFORM_MOUNTER_COMMAND,
		tr("Unable to perform the operation."),
		tr("Make sure the mount daemon \"prl_mounterd\" is running and try again."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_MOUNTER_LIST_NO_OBJECT,
		tr("The virtual machine seems to be not mounted."),
		tr("This virtual machine is not present in the list of mounted objects."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANNOT_EDIT_FIREWALL_AT_TRANS_VM_STATE,
		tr("The firewall settings of the virtual machine could not be changed."),
		tr("This virtual machine is in a transitional state. Please wait for the transition to finish and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CI_DEVICE_IS_NOT_VIRTUAL,
		tr("The device, used by the virtual machine, is real."),
		tr("Copying can be performed on image files only."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CI_CANNOT_COPY_IMAGE_NON_STOPPED_VM,
		tr("Unable to copy the image file because the virtual machine must be shut down or suspended."),
		tr("Shut down or suspend the virtual machine and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CI_PERMISSIONS_DENIED,
		tr("Unable to copy the image file. Permission denied."),
		tr("Check your permission for this image file."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_CT_ID_ALREADY_EXIST,
		tr("Failed to restore the Container."),
		tr("The Container with ID \"%1\" already exists. To solve the problem, specify a new name for the Container when restoring the backup."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CREATE_HARD_DISK_WITH_ZERO_SIZE,
		tr("Unable to create a virtual hard disk."),
		tr("Virtual hard disk size must be greater than 0 GB. Please provide a valid value and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_UNABLE_OPEN_DEVICE_ON_START_VM,
		tr("Unable to connect %1 %2."),
		tr("A file or device required for the operation of %1 %2 does not exist or is used by another process, or you have no permission to access it. The virtual machine will continue running, but the device will be disconnected."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_MSG_START_32BIT_VM_ON_64BIT_HOST,
		tr("The virtual machine process cannot be run in native mode due to the host OS problems."),
		tr("The compatibility mode that may slow down the virtual machine performance will be used instead. For solution and additional information, please read the following knowledgebase article: <a href=\"http://kb.parallels.com/en/9643\">http://kb.parallels.com/en/9643</a>."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CVSRC_IO_ERROR,
		tr("Operation failed due to I/O problems."),
		tr("Operation failed due to I/O problems."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CVSRC_NO_OPEN_REQUEST,
		tr("Open operation was not requested or is already canceled."),
		tr("Open operation was not requested or is already canceled."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CVSRC_NO_CHANNEL,
		tr("Unable to assign I/O channel to deliver video frames."),
		tr("Unable to assign I/O channel to deliver video frames."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_DIR_CREATE_ERROR,
		tr("The virtual hard disk cannot be created in this location."),
		tr("Make sure you have the permission to add new files to this location."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_VM_SYNC_DEFAULT_PRINTER_DISABLED,
		tr("Your %1 default printer is no longer synchronized with your host computer default printer."),
		tr("The default printer might have changed. To restore synchronization, edit Printer settings in the virtual machine configuration accessible through the Virtual Machine > Configure menu."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_TIME_MACHINE_EXCLUDED_LIST_OP,
		tr("Wrong directory is specified."),
		tr("Please specify the correct directory and try again."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_WARM_MODE_NOT_SUPPORTED,
		tr("Unable to migrate virtual machines that are running or paused."),
		tr("An older version of Parallels Server is installed on the target server. Only stopped virtual machines can be migrated to it."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_DEVICE_IMAGE_OUT_OF_BUNDLE,
		tr("Unable to migrate the virtual machine %1 because the image file %2 used by its device %3 is stored outside the virtual machine bundle %4."),
		tr("Disconnect or remove the device %3 from the virtual machine configuration and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_INVALID_DISK_TYPE,
		tr("Unable to migrate the virtual machine %1 because its device %2 is connected to the physical drive on the host computer."),
		tr("Disconnect or remove the device %2 from the virtual machine configuration and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_REMOVE_PERMISSIONS_DENIED,
		tr("Unable to remove the backup \"%1\"."),
		tr("You do not have permissions to remove this backup. Please contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_START_SAFE_MODE_UNSUPPORTED,
		tr("Failed to start the virtual machine."),
		tr("You are trying to start the virtual machine in Rollback Mode. This mode is not supported in @PRL_PRODUCT_NAME."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DOWNLOADING_APPLIANCE_DESCRIPTOR_FAILED,
		tr("Unable to connect to the Parallels appliances server."),
		tr("Please check your network connection and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_OS_IMG_DWNLD_RETRY_ON_CHECKSUM_MISMATCH,
		tr("The downloaded image might have been damaged during the operation."),
		tr("Please try to download it again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_OS_IMG_DWNLD_NO_FREE_DISK_SPACE,
		tr("Not enough free space in %1 to download the installation image."),
		tr("Please free at least %2."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_OS_IMG_DWNLD_CLOSE_WINDOW,
		tr("Continue downloading Windows in the background?"),
		tr("If you stop the installation, your progress will be lost. You can restart the installation later by choosing File > New, clicking Install Windows 7 purchased from Parallels, and following the onscreen instructions."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_OS_IMG_DWNLD_CLOSE_WINDOW_WIN8,
		tr("Continue downloading Windows in the background?"),
		tr("If you stop the installation, your progress will be lost. You can restart the installation later by choosing File > New, clicking Download Windows 8, and following the onscreen instructions."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WEB_STORE_UNEXPECTED,
		tr("Cannot connect to the Parallels online store."),
		tr("Please contact Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WEB_STORE_NO_CONNECTION,
		tr("Cannot connect to the Parallels online store."),
		tr("Please check your network connection and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WEB_STORE_NO_DATA,
		tr("Unable to access the online store content."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_WEB_STORE_INVALID_DATA,
		tr("Unable to access the online store content."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OS_IMG_DWNLD_UNEXPECTED,
		tr("Unable to download the operating system installation files."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OS_IMG_DWNLD_INVALID_DESCRIPTOR,
		tr("Unable to download the operating system installation files."),
		tr("Download descriptor is invalid. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OS_IMG_DWNLD_NO_CONNECTION,
		tr("Unable to download the operating system installation files."),
		tr("Please check your network connection and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OS_IMG_DWNLD_SIZE_MISMATCH,
		tr("Unable to download the operating system installation files."),
		tr("Download size does not match the expected value. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OS_IMG_DWNLD_CANNOT_SAVE,
		tr("Unable to save the operating system installation file in %1."),
		tr("Make sure you are allowed to write to this location and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CREATE_SNAPSHOT_WITH_ENABLED_AUTOCOMPRESS,
		tr("Are you sure you want to create a snapshot of %1?"),
		tr("The virtual machine is set to compress periodically to free up unused space. Creating a snapshot will disable auto-compression. If you delete all snapshots later, you can re-enable auto-compression using the Optimization settings in the virtual machine configuration (Virtual Machine > Configure or Parallels icon > Configure)."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_MIGRATE_PC_TRANSPORTER_INIT_FAILURE,
		tr("Some of the components required for migration are missing."),
		tr("Please reinstall @PRL_PRODUCT_NAME and try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_MIGRATE_PC_FAILED_TO_REGISTER_VM,
		tr("Unable to register the migrated virtual machine."),
		tr("Your Windows applications were successfully transferred to a virtual machine on your Mac, but the virtual machine was not registered in @PRL_PRODUCT_NAME. Please locate the virtual machine file and open it with @PRL_PRODUCT_NAME. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_MIGRATE_PC_ALREADY_RUNNING,
		tr("Another migration process is currently in progress."),
		tr("Please wait until it is complete and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CT_MIGRATE_ID_ALREADY_EXIST,
		tr("Failed to migrate the Container."),
		tr("The Container with ID \"%1\" already exists. To solve the problem, specify a new ID or name for the Container."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_TO_THE_SAME_NODE,
		tr("Failed to migrate the Container or virtual machine."),
		tr("You are trying to migrate the Container or virtual machine to the source server. Please specify another destination server and try again"),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ENABLE_VIDEO_POWER_OPTIMIZATION,
		tr("Your Mac is running on batteries. Do you want to enable the power saving option?"),
		tr("This option ensures longer battery life, however, some 3D graphics intensive applications or games may run slower or fail to start."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ENABLE_VIDEO_POWER_OPTIMIZATION_ON_BATTERY_SWITCH,
		tr("Your Mac is running on batteries. Do you want to enable the power saving option?"),
		tr("This option ensures longer battery life, however, some 3D graphics intensive applications or games may run slower or fail to start.<br><br>Note: If you enable the power saving option, shut down the virtual machine and then start it again for the changes to take effect."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_ACCESS_TO_VM_DENIED,
		tr("Failed to migrate the shared virtual machine."),
		tr("User \"%1\" on the destination server does not have permissions to run this virtual machine. To migrate the virtual machine, suspend or stop it first, or configure the virtual machine permissions."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_CREATE_VM_FROM_LION_RECOVERY_PART,
		tr("Not enough memory to install and run OS X Lion in a virtual machine. Are you sure you want to continue?"),
		tr("Each installation of OS X Lion requires no less than 2 GB of memory, so your Mac should be equipped with at least 4 GB of memory."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_ERROR_DELETE_VM,
		tr("The virtual machine was successfully migrated to the target PSBM server, but there was an error while deleting it from the source server."),
		tr("You can delete the virtual machine left on the source server manually."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_ERROR_UNREGISTER_VM,
		tr("The virtual machine was successfully copied to the target PSBM server, but there was an error while removing it from the virtual machines list on the source server."),
		tr("You can remove it from the virtual machines list manually."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_RUN_VM_ACTION_SCRIPT,
		tr("Unable to start the virtual machine."),
		tr("Script execution failed."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_FS_RESIZE_NOT_SUPPORTED,
		tr("Unable to resize the last volume. The file system on that volume might be corrupted or not supported."),
		tr("Please try to open the virtual machine and check the file system for errors."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_NO_FREE_SPACE,
		tr("Not enough free space on \"%1\"."),
		tr("Please free some space and try again."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CLOSE_VM_WINDOW,
		tr("The \"%1\" virtual machine is running. What should be done with it?"),
		tr("Choose the action to perform on this virtual machine when the @PRL_PRODUCT_NAME window is closed."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_APP_ALREADY_RUNNING,
		tr("@PRL_PRODUCT_NAME is already running."),
		tr("Please quit the other instance of @PRL_PRODUCT_NAME and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NEWER_APP_ALREADY_RUNNING,
		tr("A newer version of @PRL_PRODUCT_NAME is running."),
		tr("Please quit the newer version of @PRL_PRODUCT_NAME and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_OLDER_APP_ALREADY_RUNNING,
		tr("An older version of @PRL_PRODUCT_NAME is running."),
		tr("Please quit the older version of @PRL_PRODUCT_NAME and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_COPY_APP_TO_LOCAL_FOLDER,
		tr("@PRL_PRODUCT_NAME needs to be copied to the Applications folder."),
		tr("@PRL_PRODUCT_NAME will copy itself to your Applications folder and restart automaticallly from the new location."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_DOWNLOAD_NO_FILE_PERMISSION,
		tr("The file could not be downloaded to \"%1\"."),
		tr("Please make sure that you have the permission to write to this folder."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DWNLD_RETRY,
		tr("Unable to download the %1 file."),
		tr("Please try to download it again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DWNLD_RETRY_ON_CANNOT_SAVE,
		tr("Unable to save the file in %1."),
		tr("Make sure you are allowed to write to this location and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DWNLD_RETRY_ON_NO_CONNECTION,
		tr("Unable to download the %1 file."),
		tr("Please check your network connection and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DWNLD_RETRY_ON_CHECKSUM_MISMATCH,
		tr("The downloaded file might have been damaged during the operation."),
		tr("Please try to download it again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CLOSE_UPGRADE_PURCHASE_DIALOG,
		tr("The upgrade process will be suspended."),
		tr("You can resume it at any time later by choosing @PRL_PRODUCT_NAME > Upgrade to @PRL_PRODUCT_NAME %1 from the menu."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_CLOSE_UPGRADE_PURCHASE_DIALOG_WHILE_PURCHASE_PROCESSING,
		tr("Processing your purchase. Please wait."),
		tr("If you cancel the operation now, you will have to install the upgrade manually. The installation instructions will be sent to your email."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_POSTPONE_UPGRADE,
		tr("The upgrade process will resume the next time you open @PRL_PRODUCT_NAME."),
		tr("You can resume it manually at any time by choosing @PRL_PRODUCT_NAME > Upgrade to @PRL_PRODUCT_NAME %1  from the menu."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_UPGRADE_PURCHASE_MISSING_KEY,
		tr("Your purchase was successful, but the activation key could not be retrieved automatically."),
		tr("When the upgrade is complete, please activate @PRL_PRODUCT_NAME using the activation key found in the purchase order confirmation sent to your email."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VZLICENSE_KA_LICENSE_EXPIRED,
		tr("The product license has expired."),
		tr("To continue using @PRL_PRODUCT_NAME, you need to purchase a license renewal."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_FIREWALL_MAY_BLOCK_REMOTE_MANAGE,
		tr("@PRL_PRODUCT_NAME will be allowed to communicate through firewall."),
		tr("If you are unable to connect to @PRL_PRODUCT_NAME remotely, please open the firewall settings and make sure that @PRL_PRODUCT_NAME is allowed to accept incoming connections and port %1 is not blocked by the firewall. For details, see <a href=\"http://kb.parallels.com/112747\">http://kb.parallels.com/112747</a>."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_APP_COULD_NOT_BE_STARTED,
		tr("@PRL_PRODUCT_NAME could not be started."),
		tr("Please try again. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UNABLE_TO_START_SERVICES,
		tr("Unable to start Parallels services."),
		tr("Please relaunch @PRL_PRODUCT_NAME. If the problem persists, contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_STAND_BY_VM_WITH_UNSUPPORTED_SHUTDOWN_TOOL,
		tr("Unable to suspend the virtual machine."),
		tr("The drivers for some of your virtual machine devices (video, audio, etc.) are not installed in the guest operating system. That's why the virtual machine cannot be suspended. Install all the missing drivers and try again. If the problem persists, please contact the Parallels support team."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_FREE_SIZE_FOR_COMPRESSED_DISK,
		tr("The free space available on your hard disk is smaller than the recommended for this virtual machine. Do you want to create it anyway?"),
		tr("You can change the virtual machine's size by editing its hard disk settings before installing the operating system. This won't affect your virtual machine now, but in the future you may need to free up some additional space."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_STOP_APPLIANCE,
		tr("Are you sure you want to close this window?"),
		tr("If you close it now, all the changes will be discarded and the appliance won't be added to @PRL_PRODUCT_NAME."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_MEM_EXCEED_PHY_SPACE,
		tr("Failed to start the virtual machine."),
		tr("The hardware configuration does not allow you to run virtual machines with more than %1 MB of RAM. Reduce the amount of RAM in the virtual machine configuration and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_IT_CANT_RESIZE_VOLUME,
		tr("Unable to resize this hard disk. Its file system might be corrupted."),
		tr("Please try to start the virtual machine and check the file system for errors. Or you can resize the hard disk without resizing the file system and then resize it using the tools of the guest operating system."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_BACKUP_HDD_IMAGE_OUT_OF_BUNDLE,
		tr("Unable to back up the virtual machine %1, because the HDD image file %2 is stored outside of the virtual machine bundle %3."),
		tr("Rename the HDD %1 or remove it from the virtual machine configuration and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_BACKUP_INVALID_DISK_TYPE,
		tr("Unable to back up the virtual machine %1, because its HDD %2 is connected to the physical drive on the host computer."),
		tr("Remove the HDD %1 from the virtual machine configuration and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_VM_CLEANUP_NOTHING_TO_DO,
		tr( "%1 doesn't take any additional disk space."),
		tr("@PRL_PRODUCT_NAME could not find any data to compress or remove to free up the space used by this virtual machine."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_VM_CLEANUP_CONFIRM,
		tr("Do you want to reclaim the extra disk space used by \"%1\"?"),
		tr("If you reclaim the extra space, the virtual machine size will be reduced by %1."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_UPDATER_NO_CONNECTION,
		tr("There's no Internet connection to check for updates. Do you want to install the current version?"),
		tr("To be able to download the latest version of @PRL_PRODUCT_NAME, please check your Internet connection and try again."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_HARD_DISK_UNABLE_DELETE_DISK_WITH_SNAPSHOTS,
		tr("Unable to delete the virtual hard disk."),
		tr("This disk is in use by the following snapshots: %1: %2."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_ANTIVIRUS_DOWNLOAD_UNEXPECTED,
		tr("Unable to download the antivirus software installation files."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_ANTIVIRUS_DOWNLOAD_NO_CONNECTION,
		tr("Unable to download the antivirus software installation files."),
		tr("Please check your network connection and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_RUN_VM_FOR_INSTALL_ANTIVIRUS,
		tr("Unable to start the installation, as %1 is not running."),
		tr("To install the antivirus software, either start or resume %1 and click <b>Try Again</b> when it boots up."),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		GUI_ERR_ANTIVIRUS_IMG_DWNLD_UNEXPECTED,
		tr("Unable to download the antivirus software installation files."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_ANTIVIRUS_IMG_DWNLD_INVALID_DESCRIPTOR,
		tr("Unable to download the antivirus software installation files."),
		tr("Download descriptor is invalid. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_ANTIVIRUS_IMG_DWNLD_NO_CONNECTION,
		tr("Unable to download the antivirus software installation files."),
		tr("Please check your network connection and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_ANTIVIRUS_IMG_DWNLD_SIZE_MISMATCH,
		tr("Unable to download the antivirus software installation files."),
		tr("Download size does not match the expected value. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_ANTIVIRUS_IMG_DWNLD_CANNOT_SAVE,
		tr("Unable to save the antivirus software installation files in %1."),
		tr("Make sure you are allowed to write to this location and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_USB_CONTROLLER_REMOVE,
		tr("Are you sure you want to remove USB from the virtual machine configuration?"),
		tr("If you agree, your USB and Bluetooth devices may stop working in the virtual machine."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_VIDEO_MEMORY_IS_TOO_LOW_FOR_HIDPI,
		tr("For the best experience on the external monitor, at least %1 MB of video memory is required."),
		tr("Do you want to increase the video memory to %1 MB?"),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CT_MIGRATE_TARGET_ALREADY_EXISTS,
		tr("Unable to migrate the Container."),
		tr("The migration cannot be performed because a Container with the same ID or with the same private or root path already exists on the destination server."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ENABLE_VTX_EPT_SUPPORT,
		tr("Enabling nested virtualization allows you to run virtual machines in %1, however, may slow down %2 performance. Are you sure you want to continue?"),
		tr("If you enable this option to be able to develop software for Windows Phone, please read <a href=\"%1\">this help topic</a> to learn how to properly configure Windows to run Windows Phone Emulator."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ENABLE_PMU_SUPPORT,
		tr("Enabling this option may slow down the virtual machine's performance. Are you sure you want to continue?"),
		tr("PMU virtualization allows you to run Intel VTune Amplifier, OProfile, and other performance monitoring tools in a virtual machine."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UPGRADE_REQUEST_NETWORK_ERROR,
		tr("Unable to process the upgrade request."),
		tr("Please check your network connection and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UPGRADE_REQUEST_UNEXPECTED,
		tr("Unable to process the upgrade request."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_UPGRADE_REQUEST_INVALID_RESPONSE,
		tr("Unable to process the upgrade request."),
		tr("Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_USB3_DRIVERS_REQUIRED_WIN,
		tr("\"%1\" may not have USB 3.0 drivers installed."),
		tr("To use USB 3.0 devices with this virtual machine, please download the USB 3.0 driver from <a href=\"http://www.parallels.com/support/xhc-drivers\">here</a> and install it in Windows."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_USB3_DRIVERS_REQUIRED_OTHER,
		tr("\"%1\" may not have USB 3.0 drivers installed."),
		tr("To use USB 3.0 devices with this virtual machine, you may need to configure it to support USB 3.0."),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VOLUME_LICENSE_EXPIRED,
		tr("Your license has expired."),
		tr("Please ask your system administrator to update the license."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_MAKE_BACKUP_BEFORE_DISK_RESIZE,
		tr("It is highly recommended to make a backup of the virtual machine before the operation."),
		tr("Resizing the disk is sensitive to power failures and similar events. For more details on how to resize the disk properly, please see <a href=\"%1\">here</a>."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_ENABLE_ACCESSIBILITY_MODE,
		tr("VoiceOver is enabled on your Mac. Do you want to work with \"%1\" in Accessibility mode?"),
		tr("This mode is specially designed and optimized for using screen readers in virtual machines. For more details, please see <a href=\"%1\">here</a>."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_HYPERVISOR_HVT_ENABLED_ON_HYPERSWITCH,
		tr("An incompatible virtualization application is running."),
		tr(	"Don't use your Parallels virtual machines until you quit that application. "
			"For details, see <a href=\"http://kb.parallels.com/en/115966\">http://kb.parallels.com/en/115966</a>."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DISABLE_HOST_SLEEP,
		tr( "Would you like to set your computer never to sleep so it's always accessible to @PRL_PRODUCT_NAME?" ),
		tr( "@PRL_PRODUCT_NAME can't connect to your computer when it's asleep." ),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DISABLE_COPY_PAST_ALL_DISKS,
		tr( "Your changes will disable copying and pasting files between %1 and the Mac. Are you sure you want to continue?" ),
		tr( "To enable copying and pasting files between %1 and the Mac, all Mac disks or the Mac home folder must be shared with Windows, and all Windows disks must be shared with the Mac." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DISABLE_COPY_PAST_SHARE_WINDOWS,
		tr( "Are you sure you only want to share the Mac home folder with Windows?" ),
		tr( "If you do so, you'll be able to copy and paste files from the Mac to Windows only from the Mac home folder." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_PAX_AGENT_NO_CONNECTION,
		tr( "Unable to download the Parallels Access agent." ),
		tr( "Please check your Internet connection and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_PAX_AGENT_DOWNLOAD_ERROR,
		tr( "Parallels Access agent could not be downloaded." ),
		tr( "Please try to download it again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_OTHER_VM_HAS_SAME_EXTERNAl_BOOT,
		tr( "The selected external device is already used by %1. Do you want to connect it anyway?" ),
		tr( "If you click Yes now, the device will be disconnected from the virtual machine that is currently using it." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_INSTALL_MAJOR_UPDATE,
		tr( "A new version of @PRL_PRODUCT_NAME is available! Would you like to upgrade now?" ),
		tr( "@PRL_PRODUCT_NAME %1 is now available - you have %2." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_WINDOWS_UPDATE_WORKING,
		tr("Windows Update"),
		tr("Your virtual machine may be running slower now as Windows is being updated."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_PAX_AGENT_INSTALL_FAILURE,
		tr( "Parallels Access agent could not be installed." ),
		tr( "Please contact the Parallels support team for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DISABLE_WINDOWS7_LOOK,
		tr( "You will be logged out of the system to disable the Windows 7 look. Are you sure you want to continue?" ),
		tr( "When you log in again, Windows 8 will have its initial design." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_INSTALL_WINDOWS7_LOOK,
		tr( "Making Windows 8 work like Windows 7 requires installing the applications Start8&trade; and ModernMix&trade; from Stardock. Do you want to continue?" ),
		tr( "Updates to these apps are supported by Stardock. For more details, please see <a href=\"%1\">here</a>." ),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_PAX_ENABLE_REMOTE_ACCESS,
		tr( "Do you want to install Parallels Access and work with your Mac and Windows applications from your iPad?" ),
		tr( "The installation won't affect your work, as it's performed in the background. Parallels Access can also be installed at a later time using <b>Preferences</b> > <b>Access</b>." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_DISABLE_VERBOSE_LOG,
		tr( "Would you like to disable detailed log messages?" ),
		tr( "Using detailed log messages may decrease your system performance and is recommended only when troubleshooting." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_QUIT_PARALLELS_ACCESS,
		tr( "Are you sure you want to exit @PRL_PRODUCT_NAME?" ),
		tr( "If you exit @PRL_PRODUCT_NAME, your computer will not be accessible from your iPad." ),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SUSPEND_WITH_USB_BOOTDISK,
		tr( "Unable to perform the operation." ),
		tr( "You cannot suspend or create snapshots of this virtual machine because it is running from an external storage device." ),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_TRUNCATE_FAILED,
		tr( "Some virtual hard disk operation cannot be completed because there is not enough free disk space on your Mac's hard disk." ),
		tr( "Please free some disk space. Until this operation is completed, you will not be able to continue working with the virtual machine." ),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_BACKUP_NON_IMAGE_HDD,
		tr( "The contents of physical hard disks cannot be backed up." ),
		tr( "Only virtual hard disks can be backed up." ),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_CONNECTED_TO_PROXY_MANAGER,
		tr("Unable to perform the operation."),
		tr("This action cannot be performed because Parallels Service is not connected to the Parallels Access server."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LIFETIME_IS_EXPIRED,
		tr("Unable to perform the operation."),
		tr("The usage period for this virtual machine has expired. Please contact your administrator for assistance."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_CONTINUE_VM_LIFETIME_IS_EXPIRED,
		tr("The usage period for this virtual machine has expired."),
		tr("Please contact your administrator for assistance."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EXPIRATION_OFFLINE_GRACE_PERIOD_EXPIRED,
		tr("To continue using this virtual machine, you must connect to a time server."),
		tr("The grace period of %1 days without connecting to a time server has expired. Please contact your administrator for assistance."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EXPIRATION_HOST_DATETIME_SKEWED,
		tr("The date and time settings of your Mac are incorrect."),
		tr("Please correct the date and time settings and try again."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_DECRYPT_PROTECTED_VM,
		tr("Unable to perform the operation."),
		tr("There's an expiration date set for this virtual machine. To be able to decrypt it, you need first to cancel the expiration date."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EXPIRATION_VM_IS_IN_OFFLINE_GRACE_PERIOD,
		tr("This virtual machine cannot contact the time server to check date and time."),
		tr("Please contact your administrator for assistance. If the virtual machine cannot check the date, it will stop working at %1 on %2."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EXPIRATION_VM_IS_ABOUT_TO_EXPIRE,
		tr("The usage period for this virtual machine is about to expire."),
		tr("The virtual machine will stop working at %1 on %2. To extend the usage period, please contact your administrator for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_PROTECT_UNENCRYPTED_VM,
		tr("Unable to set a password protecting this virtual machine expiration date settings."),
		tr("Please encrypt the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_EXPIRATION_TIME_CHECK_INTERVAL_OUT_OF_RANGE,
		tr("The specified data check frequency is invalid."),
		tr("The interval must be a number between %1 and %2 seconds. Please correct the value and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_PROTECT_VM_IS_ALREADY_PROTECTED,
		tr("Unable to set a password protecting this virtual machine expiration date settings."),
		tr("The password is already set."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_UNPROTECT_VM_IS_NOT_PROTECTED,
		tr("Unable to cancel the password protecting this virtual machine expiration date settings."),
		tr("The password is not set."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_EDIT_EXPIRATION_VM_IS_PROTECTED,
		tr("Unable to edit this virtual machine expiration date settings."),
		tr("These settings are protected with a password."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_REMOVE_PROTECTION,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("A password protecting this virtual machine expiration date settings is being canceled. Please try again later."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOCKED_FOR_SET_PROTECTION,
		tr("Unable to perform the action because the virtual machine is busy."),
		tr("A password protecting this virtual machine expiration date settings is being set. Please try again later."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_PROTECT_PASSWORD_IS_EMPTY,
		tr("Unable to perform the operation."),
		tr("You cannot use a blank password to protect the virtual machine expiration date settings."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_PASSWORD_TO_PROTECTED_VM,
		tr("The specified password is incorrect."),
		tr("Please enter a valid password protecting this virtual machine expiration date settings."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_PROTECT_VM_WRONG_EXPIRATION_DATE,
		tr("Unable to apply the expiration date settings."),
		tr("Please make sure the expiration date format (e.g. 2014-12-30T20:30:00) is correct and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_VM_STATE_OF_PROTECTION_OP,
		tr("Unable to perform the operation."),
		tr("Please suspend or shut down the virtual machine and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WEB_PORTAL_INVALID_PASSWORD,
		tr( "The password you entered is too short." ),
		tr( "The password must be at least six characters long." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WEB_PORTAL_INVALID_EMAIL,
		tr( "The email you entered is not a valid email address." ),
		tr( "Make sure that you entered your email address correctly and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WEB_PORTAL_ACCOUNT_ALREADY_EXISTS,
		tr( "The email address you entered is already registered." ),
		tr( "Make sure that you typed your email address correctly or sign in using your registered account." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_APP_BUNDLE_INIT_SOUND_DRIVER_LOCKED,
		tr( "Other software running on your computer is preventing @PRL_PRODUCT_NAME from being upgraded" ),
		tr( "Please try again. If the problem persists, contact the Parallels support team." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_SETUP_HEADLESS_MODE,
		tr("Unable to select this option."),
		tr("Restart your Mac and try again. If the problem persists, please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_TO_SETUP_HEADLESS_MODE_BY_NON_PRIVILEGED_USER,
		tr("Unable to select this option."),
		tr("You must have administrator rights to perform this action."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_PAX_HOST_LIMIT_WAS_EXCEEDED,
		tr( "No more computers can be added." ),
		tr( "You have reached your limit of computers registered with Parallels Access. If you need more, you can create a business account at <a href=\"%1\">account.parallels.com</a>." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_IPN_MESSAGE,
		tr( "%1" ),
		tr( "%1" ),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_INSTALLED_GUEST_APPS_DIALOG,
		tr( "We noticed that you have just installed new applications. Do you want to add them to the Mac Launchpad?" ),
		tr( "Please choose which apps you want to add." ),
		true,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_FEEDBACK_SUCCESS,
		tr( "Thank you. Your feedback has been successfully sent to Parallels." ),
		tr( "" ),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_RESTORE_INTERNAL_ERROR,
		tr( "Failed to restore the virtual machine due to an internal error." ),
		tr( "Please try again. If the problem persists, contact your system administrator for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_RESTORE_INTERNAL_PROTO_ERROR,
		tr( "Failed to restore the virtual machine due to an internal protocol error." ),
		tr( "Please try again. If the problem persists, contact your system administrator for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_RESTORE_DIRECTORY_ALREADY_EXIST,
		tr( "Failed to restore the virtual machine because the folder %1 already exists." ),
		tr( "Contact your system administrator for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_RESTORE_CANNOT_CREATE_DIRECTORY,
		tr( "Unable to create the directory \"%1\"." ),
		tr( "Please try again. If the problem persists, contact your system administrator for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BUSE_NOT_MOUNTED,
		tr( "The BUSE filesystem is not mounted." ),
		tr( "Make sure that the \"buse\" package is installed and the \"bused\" daemon is running." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BUSE_INTERNAL_ERROR,
		tr( "An internal error has occurred on the BUSE filesystem." ),
		tr( "Operation on the BUSE filesystem failed due to an internal error." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BUSE_ENTRY_INVALID,
		tr( "BUSE entry \"%1\" is not valid." ),
		tr( "Remove the BUSE entry and try to create it again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BUSE_ENTRY_ALREADY_EXIST,
		tr( "BUSE entry \"%1\" already exists." ),
		tr( "Remove the BUSE entry before trying to create it again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BUSE_ENTRY_ALREADY_INITIALIZED,
		tr( "BUSE entry \"%1\" already initialized." ),
		tr( "Remove the BUSE entry before trying to initialize its parameters again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BUSE_ENTRY_IO_ERROR,
		tr( "Input/output error on the BUSE entry \"%1\"." ),
		tr( "Operation on the BUSE entry failed due to an input/output error." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONN_UNABLE_TO_ESTABLISH_TRUSTED_CHANNEL,
		tr( "Unable to establish a secure connection to the remote computer." ),
		tr( "Please try again. If the problem persists, contact the Parallels support team." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONN_CLIENT_CERTIFICATE_INVALID,
		tr( "Unable to connect to the remote computer." ),
		tr( "The client-side certificate is invalid. Please contact the Parallels support team for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONN_CLIENT_CERTIFICATE_EXPIRED,
		tr( "Unable to connect to the remote computer." ),
		tr( "The client-side certificate has expired. Please contact the Parallels support team for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONN_CLIENT_CERTIFICATE_REVOKED,
		tr( "Unable to connect to the remote computer." ),
		tr( "The client-side certificate has been revoked. Please contact the Parallels support team for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONN_SERVER_CERTIFICATE_INVALID,
		tr( "Unable to connect to the remote computer." ),
		tr( "The server-side certificate is invalid. Please contact the Parallels support team for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONN_SERVER_CERTIFICATE_EXPIRED,
		tr( "Unable to connect to the remote computer." ),
		tr( "The server-side certificate has expired. Please contact the Parallels support team for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONN_SERVER_CERTIFICATE_REVOKED,
		tr( "Unable to connect to the remote computer." ),
		tr( "The server-side certificate has been revoked. Please contact the Parallels support team for assistance." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CPUFEATURES_NO_BINARY,
		tr( "Cannot find the cpufeatures binary." ),
		tr( "Install the cpufeatures package to change CPU features mask." ),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CPUFEATURES_INCORRECT_MASK,
		tr( "Cannot mask unmaskable CPU features." ),
		tr( "Consult 'prlsrvctl info --full' output for list of unmaskable CPU features." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CPUFEATURES_POOLS_MANAGMENT,
		tr( "Cannot change CPU features mask when using cpupools features management." ),
		tr( "" ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CPUFEATURES_RUNNING_VM_OR_CT,
		tr( "Cannot complete the operation as there are virtual machines or Containers currently running on the server." ),
		tr( "Stop all virtual machines and Containers and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CPUFEATURES_NOT_IN_POOLS,
		tr( "Cannot complete the operation as the server is not in CPU pools." ),
		tr( "Add the server to CPU pools and try again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CPUFEATURES_DEFAULT_POOL_ARG,
		tr( "The operation cannot be applied to a default CPU pool." ),
		tr( "Use a custom CPU pool." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CPUFEATURES_INCOMPATIBLE_NODE,
		tr( "The server is incompatible with the CPU pool." ),
		tr( "" ),
		false,
		PMT_CRITICAL
		);
	REGISTER_ERROR_STRING(
		PRL_ERR_VCMMD_NO_CONNECTION,
		tr( "Cannot connect to memory management service." ),
		tr( "Check vcmmd service status" ),
		false,
		PMT_CRITICAL
		);
}
