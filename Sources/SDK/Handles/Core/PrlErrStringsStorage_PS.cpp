/*
 * PrlErrStringsStorage_PS.cpp: It contains message descriptions for
 * Virtuozzo and should be included to PrlErrStringsStorage
 * class body.
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#include "Build/Current.ver"
#include "PrlErrStringsStorage.h"

#ifdef REGISTER_ERROR_STRING
#undef REGISTER_ERROR_STRING
#endif

const char* TAG_TEXT_PRODUCT_NAME_SERVER = QT_TRANSLATE_NOOP( "QObject", VER_SHORTPRODUCTNAME_STR_PS );

static void REGISTER_ERROR_STRING(PRL_RESULT error_code, const QString &brief_msg, const QString &long_msg, bool can_be_ignored_sign, PRL_MESSAGE_TYPE_ENUM message_type)
{
	PrlErrStringsStorage::GetStorageInstance()->m_PSErrorsStringsMap.insert(error_code, PrlErrStringData(brief_msg, long_msg, can_be_ignored_sign, message_type));
}

static void REGISTER_ERROR_STRING2(PRL_RESULT error_code
	, const QString &brief_msg
	, int brief_check_singular_param, const QString &brief_singular_msg
	, const QString &long_msg
	, int long_check_singular_param, const QString &long_singular_msg
	, bool can_be_ignored_sign, PRL_MESSAGE_TYPE_ENUM message_type)
{
	PrlErrStringsStorage::GetStorageInstance()->m_PSErrorsStringsMap.insert(error_code
		, PrlErrStringData(brief_msg
			, brief_check_singular_param, brief_singular_msg
			, long_msg
			, long_check_singular_param, long_singular_msg
			, can_be_ignored_sign, message_type));
}

void PrlErrStringsStorage::InitializePSErrorMessages()
{
	(void)&REGISTER_ERROR_STRING2; // to skip warnings

	REGISTER_ERROR_STRING(
		PRL_ERR_READ_FAILED,
		tr("An error occurred while reading the file." ),
		tr("Check file read permissions." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRITE_FAILED,
		tr("An error occurred while writing the file." ),
		tr("Check file write permissions." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_OPEN_FAILED,
		tr("An error occurred while opening the file." ),
		tr("Check file and/or directory names and create permisions." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_ADAPTER_NOT_EXIST,
		tr("The network adapter %1 cannot be connected."),
		tr("The network adapter <b>%2</b> you selected to be used by the virtual adapter %1 doesn't exist on the physical server. Select another physical network adapter and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_NET_BIND_FAILED,
		tr("Unable to use the network adapter %1."),
		tr("Make sure that the network adapter %1 is available and enabled on the physical server."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_NET_CONNECTION_SHARING_CONFLICT,
		tr("Conflict between Parallels Shared Networking and Mac OS X Internet Sharing."),
		tr("Parallels detected that Mac OS X Internet Sharing is active for %1. This setting interferes with Parallels Shared Networking. In Mac OS X, go to System Preferences > Sharing  and disable Internet Sharing. Then, clear the %1 option in the \"To computers using\" list."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_NET_CABLE_DISCONNECTED,
		tr("The real network adapter bound to virtual network %2 is used by the virtual network adapter %1 that is not connected to any network."),
		tr("Please connect the real network adapter to a wired or wireless network."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_APPLY_CONFIG_FAILED,
		tr("Unable to apply the changes."),
		tr("The configuration was not saved. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_ALREADY_REGISTERED_VM_UUID,
		tr("Cannot register this virtual machine due to an ID conflict."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_NETWORK_ADAPTER_GUEST_TOOLS_NOT_AVAILABLE,
				tr("The virtual machine does not have Virtuozzo Tools installed."),
				tr("The settings will be applied to the virtual machine, but the network adapter may work incorrectly."
				   " Install Virtuozzo Tools and check the network adapter settings."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_OUT_OF_DISK_SPACE,
		tr("Out of disk space."),
		tr("Not enough disk space on the physical server. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_USB_OPEN_MANAGER_FAILED,
		tr("Cannot connect the USB device because the required driver is missing."),
		tr("Install the USB device driver or contact your Virtuozzo administrator for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACCESS_TO_VM_DENIED,
		tr("Access denied."),
		tr("You do not have enough rights to use this virtual machine. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_ALL_FILES_WAS_DELETED,
		tr("The virtual machine has been deleted, but some of its files may remain on the physical server."),
		tr("Some of the virtual machine files could not be removed because they were in use or because you do not have enough permissions to delete them. You may try to delete these files manually."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_START_FAILED,
		tr("Unable to start the virtual machine."),
		tr("An unexpected error occurred while starting the virtual machine. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_PROCESS_IS_NOT_STARTED,
		tr("The virtual machine cannot be started due to a critical error."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_FILES_ALREADY_REMOVED,
		tr("The virtual machine files were not found."),
		tr("This virtual machine cannot be deleted because it has been already removed from the physical server hard disk."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_NO_AUTH_TO_SAVE_FILES,
		tr("You do not have the permission to save the virtual machine files in this folder."),
		tr("Select another location or contact your Virtuozzo administrator for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_NO_AUTH_TO_CREATE_VM_IN_DIR,
		tr("You do not have the permission to create a virtual machine in the \"%1\" folder."),
		tr("Select another location or contact your Virtuozzo administrator for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_RETRIEVE_VM_CONFIG,
		tr("Access denied."),
		tr("You do not have the permission to use this virtual machine and perform any actions with it until you have the proper access privileges. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_OPERATION_NOT_AUTHORISED,
		tr("You do not have the permission to perform this operation."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ADD_VM_OPERATION_WAS_CANCELED,
		tr("The action was canceled by the user."),
		tr("The process of adding a virtual machine was interrupted. No virtual machine was added to the server's list."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_NO_AUTH_TO_EDIT_VM,
		tr("You do not have the permission to edit the virtual machine configuration."),
		tr("The changes you did to the virtual machine configuration will be discarded. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONFIG_EDIT_COLLISION,
		tr("An editing conflict occurred."),
		tr("Someone modified this virtual machine configuration while you were editing it. Click <b>Reload</b> to cancel your changes and update the configuration. Click <b>Save my changes anyway</b> to overwrite the configuration. If you choose to save your changes, the changes made by another user will be lost."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SERVER_PREFS_EDIT_COLLISION,
		tr("An editing conflict occurred."),
		tr("Someone modified the preferences of this server while you were editing them. Click <b>Reload</b> to cancel your changes and update the preferences. Click <b>Save my changes anyway</b> to overwrite the preferences. If you choose to save your changes, the changes made by another user will be lost."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_LICENSE_ACTIVATION_SUCCESSFUL,
		tr("Virtuozzo was successfully activated."),
		QString(),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_OLD_CONFIG_CONVERTION,
		tr("Attention! Virtuozzo is going to update your virtual machine to a new format."),
		tr("This operation is irreversible. After the update, you will not be able to use this virtual machine with Parallels Desktop 3.0 and its earlier versions.<br><br>We recommend that you create a backup copy of this virtual machine before starting the update."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_NET_PRLNET_OPEN_FAILED,
		tr("Failed to open the Parallels networking module."),
		tr("Your virtual machine will continue to work normally, but the networking will be disabled. You can fix the problem by restoring the default network settings. To this effect, go to <b>Server > Edit settings > Network</b> and click <b>Restore Defaults</b>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_ETHLIST_CREATE_ERROR,
		tr("Failed to obtain a list of server network adapters."),
		tr("Your virtual machine will continue to work normally, but the networking will be disabled. You can fix the problem by restoring the default network settings. To this effect, go to <b>Server > Edit settings > Network</b> and click <b>Restore Defaults</b>."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_ERR_PRL_NO_BINDABLE_ADAPTER,
		tr("Unable to find a suitable Ethernet adapter."),
		tr("No suitable adapter for the specified network type could be found. Check your network settings using <b>Server > Edit settings > Network</b> or contact your Virtuozzo administrator for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HVT_NOT_PRESENT,
		tr("Unable to start the virtual machine \"%1\"."),
		tr("The virtual machine cannot be started because the physical server main processor does not support hardware virtualization."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HVT_DISABLED,
		tr("Unable to start the virtual machine \"%1\"."),
		tr("The virtual machine cannot be started because the hardware virtualization support is disabled in the physical server BIOS settings. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTX_ENABLED_ONLY_IN_SMX,
		tr("Unable to start the virtual machine \"%1\"."),
		tr("The virtual machine cannot be started because the hardware virtualization support is disabled in the physical server BIOS settings. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HVT_NOT_AVAILABLE_WARNING,
		tr("This virtual machine may work not properly."),
		tr("The main processor of the physical server does not support hardware virtualization, which may affect your virtual machine's performance."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_DIRECTORY_NOT_SET,
		tr("The folder required for the correct operation of Virtuozzo is not available."),
		tr("Contact your Virtuozzo administrator for assistance or try to reinstall the application."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_QUIT_APPLICATION,
		tr("Do you want to quit the application?"),
		tr("If you don't quit the application, only the main window will be closed."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_DELETE_FILE,
		tr("Unable to delete the virtual hard disk image file."),
		tr("The file does not exist or you do not have the permission to delete it. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CREATE_HDD_IMAGE,
		tr("The image file \"%1\" was not created."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DEV_DISABLE_AFTER_LOST,
		tr("One or more devices connected to this virtual machine are disabled."),
		tr("The physical resources used by these virtual devices are not available on the physical server any more."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_EXCEED_LIMIT_MAX_RUNNING_VMS,
		tr("The virtual machine cannot be started." ),
		tr("You have reached the maximal number of virtual machines (%1) that you are allowed to simultaneously run on the physical server. To be able to start this virtual machine, suspend or stop one of the virtual machines currently running on the server." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANT_ADD_PCI_INSTANTLY,
		tr("Unable to add an Intel VT-d PCI device using the Add Typical Device button."),
		tr("To add an Intel VT-d PCI device, follow the steps of the wizard."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANT_ADD_SCSI_INSTANTLY,
		tr("Unable to add a generic SCSI device using the Add Typical Device button."),
		tr("To add a generic SCSI device, follow the steps of the wizard."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANT_ADD_VIDEO_ADAPTER_INSTANTLY,
		tr("Unable to add a PCI video adapter using the Add Typical Device button."),
		tr("To add a PCI video adapter, follow the steps of the wizard."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_VM_INVALID_EXTENSION,
		tr("Unable to add the virtual machine because the specified file %1 is not a valid configuration file."),
		tr("Virtuozzo virtual machine configuration files must have the \".pvs\" extension. Select the correct file and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_POWER_ON_FAILED,
		tr("Unable to start the virtual machine."),
		tr("The virtual machine cannot be started. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_REBOOT_HOST,
		tr("Unable to load the Parallels driver."),
		tr("Restart the physical server and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_COULDNT_CREATE_AUTHORIZATION_FILE,
		tr("Unable to authorize the user."),
		tr("The necessary authorization file could not be created because your physical hard disk is full. Free some disk space and try again or contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_INSTALL_FAILED,
		tr("Unable to install the network adapter."),
		tr("The specified network adapter cannot be installed on this server. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_UNINSTALL_FAILED,
		tr("Unable to remove the Parallels network adapter."),
		tr("The specified network adapter cannot be removed. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_SYSTEM_ERROR,
		tr("The changes to the networking settings could not be applied."),
		tr("The system error %1 occurred when applying the new settings. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_RECREATE_HDD,
		tr("Are you sure you want to recreate this virtual hard disk?"),
		tr("Recreating the virtual hard disk will delete all the data stored on this disk."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_REPORT_POST_ID_REGISTER_FAILED,
		tr("Unable to send the problem report."),
		tr("Check your network settings and try again. If the problem persists, contact your Virtuozzo administrator." ),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HOST_AMD_GUEST_MAC,
		tr("You cannot run a Mac OS X virtual machine on this physical server."),
		tr("The physical server has the AMD main processor installed, while you can run Mac OS X virtual machines on Intel-based servers only."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HARD_DISK_IMAGE_CORRUPTED,
		tr("The virtual disk %1 is corrupted."),
		tr("Virtuozzo detected that the structure of the virtual hard disk %1 is corrupt. This may cause your virtual machine and its applications to work incorrectly. Use the disk checking utilities inside the virtual machine to repair the disk."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_MAKE_DIRECTORY,
		tr("The folder %1 cannot be created."),
		tr("You are not allowed to create folders in this location. Choose another location or contact the Virtuozzo administrator."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_EXPIRED,
		tr("This product was activated for a trial period that has now expired."),
		tr("Purchase a new <a href=\"http://www.parallels.com/en/shop/online/server/\">activation key</a> to continue using Virtuozzo."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_NOT_STARTED,
		tr("License period starts in future."),
		tr("Contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACCESS_DENINED_TO_RUN_WIZARD,
		tr("You do not have the permission to %1 the virtual machine \"%2\"."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNDER_OLD_HYPERVISOR,
		tr("You need to quit %1 to be able to start virtual machines with Virtuozzo."),
		tr("Shut down all virtual machines running under %1 and quit the application."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SAVE_VM_CONFIG,
		tr("The virtual machine \"%1\" cannot be saved."),
		tr("You do not have the permission to write to the folder %2. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INSTALLATION_PROBLEM,
		tr("The default network adapters cannot be reinstalled."),
		tr("The default network adapters on this server cannot be reinstalled properly. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VMDEVICE_VIRTUAL_NETWORK_NO_ADAPTER,
		tr("The network adapter does not exist."),
		tr("There is no network adapter on this server for the \"%2\" virtual network. The network adapter %1 will be disconnected."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONFIG_BEGIN_EDIT_NOT_FOUND_OBJECT_UUID,
		tr("Your changes cannot be saved due to an incorrect object reference."),
		tr("The selected object was opened for editing incorrectly. Try to open the object again or contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CONFIG_BEGIN_EDIT_NOT_FOUND_USER_UUID,
		tr("Your changes cannot be saved due to an improper user session initialization."),
		tr("The user session was established incorrectly. Try to establish a new session or contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CONNECT_TO_DISPATCHER,
		tr("Unable to connect to Virtuozzo."),
		tr("You may experience a connection problem or the server may be down. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_NOT_CONNECTED_TO_DISPATCHER,
		tr("Do you want to reconnect to the server \"%1\"?"),
		tr("The connection to the server has been lost. You need to reconnect to it to continue working with your virtual machines."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_USER_NO_AUTH_TO_EDIT_SERVER_SETTINGS,
		tr("You do not have the permission to change the server settings."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_SERVER_RESTART_IS_NEEDED,
		tr("You need to restart Virtuozzo on the physical server."),
		tr("The new settings will come into effect only after the Virtuozzo restart."),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_KEXT_REBOOT_REQUIRED,
		tr("Are you sure you want to start the virtual machine?"),
		tr("One or more kernel components loaded on your server are for another version of Virtuozzo. If you start the virtual machine now, it may work incorrectly. To solve this problem, restart your server. If the problem persists, reinstall Virtuozzo."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_INITIALIZATION_FAILED,
		tr("The virtual machine cannot start due to an Intel VT-d I/O subsystem initialization failure."),
		tr("You will not be able to start the virtual machine until you disable or remove the Intel-d device from its configuration. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
	PRL_ERR_VTD_ALREADY_HOOKED_FAILED,
		tr("The virtual machine cannot be started."),
		tr("The Intel VT-d PCI device is already in use in another virtual machine. You will not be able to start this virtual machine until you disable or remove the device from its configuration. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
	PRL_ERR_VTD_HOOK_FAILED,
		tr("The virtual machine cannot be started."),
		tr("Virtuozzo could not gain control over the Intel VT-d PCI device connected to this virtual machine. You will not be able to start the virtual machine until you disable or remove the device from its configuration. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VTD_WAIT_ASR,
		tr("The initialization of the VT-d device added to the virtual machine configuration has not yet completed."),
		tr("Please wait some time, then try to start the virtual machine again. If the problem persists, remove the VT-d device from the virtual machine configuration or contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_UUID_NOT_FOUND,
		tr("The virtual machine could not be found."),
		tr("The virtual machine is not registered in the virtual machine directory on this server. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

  REGISTER_ERROR_STRING(
		PRL_ERR_X64GUEST_ON_X64VM_HVT_DISABLED,
		tr("The virtual machine cannot be started."),
		tr("You cannot run a 64-bit virtual machine on a 64-bit physical server if the server does not support hardware virtualization or has the hardware virtualization option disabled in its BIOS settings. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_RENAME_FAILED,
		tr("Unable to rename the Parallels network adapter."),
		tr("The name of this network adapter could not be changed. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_INSTALL_TIMEOUT,
		tr("Unable to configure the Parallels network adapter."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_NON_COMPATIBLE_CPU_ON_TARGET,
		tr("You cannot migrate this virtual machine using the live mode."),
		tr("The CPUs on the source and destination servers are not compatible. The source server CPU model is '%1' and the target server CPU model is '%2'."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_MIGRATE_NON_COMPATIBLE_CPU_ON_TARGET_SHORT,
		tr("You cannot migrate this virtual machine using the live mode."),
		tr("The CPUs on the source and destination servers are not compatible."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CT_MIGRATE_EXTERNAL_PROCESS,
		tr("You cannot live migrate a Container with an external process."),
		tr("The Container you are trying to live migrate has an external process running. Make sure there are no open prlctl enter sessions or running prlctl exec processes and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_COMMON_SERVER_PREFS_WERE_CHANGED,
		tr("The server configuration cannot be saved due to a user activity conflict. Your changes will be discarded."),
		tr("Another user saved the configuration while you were editing it. Try to edit the configuration again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_COMMON_SERVER_PREFS_BLOCKED_TO_CHANGE,
		tr("The server configuration cannot be saved due to a user activity conflict. Your changes will be discarded."),
		tr("Another user saved the configuration while you were editing it. Try to edit the configuration again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WS_DISP_CONNECTION_CLOSED,
		tr("Connection to Parallels Service was lost."),
		tr("There may be connection problems or the server may be down. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
	PRL_ERR_VTD_HOOK_NEED_REBOOT_FAILED,
		tr("Unable to start the virtual machine."),
		tr("The Intel VT-d support is disabled on this server. Restart the server and try again. If the problem persists, contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_REMOVE_SERVER_FROM_LIST,
		tr("Are you sure you want to remove the \"%1\" server from the list?"),
		tr("If you agree, the server will be removed from the list of registered servers. This operation won't affect the virtual machines on this server."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SERVER_ALREADY_REGISTERED,
		tr("This server is already registered."),
		tr("This server is already registered as <b>%1</b>. If you want to register this server anew, remove the \"%1\" server first."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SERVER_CUSTOM_NAME_EXISTS,
		tr("The name you specified is already reserved as the %1 of the \"%2\" server."),
		tr("Specify another name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SERVER_WRONG_CUSTOM_NAME,
		tr("The name \"%1\" cannot be assigned to this server."),
		tr("Specify another name and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_SERVER_CUSTOM_NAME_EMPTY,
		tr("The physical server name is not specified."),
		tr("Specify an arbitrary name for the server and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CORE_STATE_INV_SAV_VERSION,
		tr("Do you want to remove the saved state and restart the virtual machine?"),
		tr("The virtual machine %1 cannot be resumed because the file containing its saved state is incompatible with this version of Virtuozzo. If you restart the virtual machine now, you will lose this saved state."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACCESS_DENIED_TO_DISK_IMAGE,
		tr("The disk image file %1 cannot be recreated."),
		tr("You do not have the permission to write to the file \"%2\". Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HANDSHAKE_FAILED,
		tr("Unable to connect to the server \"%1\"."),
		tr("An invalid response has been received from the server. Make sure that Virtuozzo is running on the server \"%1\" and is up to date and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_PROTOCOL_VERSION,
		tr("Unable to connect to the server \"%1\"."),
		tr("The version of your Virtuozzo software <i>%2</i> is incompatible with the version of Virtuozzo installed on the physical server <i>%3</i>. Synchronize the Virtuozzo software on both servers and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_SSL_HANDSHAKE_FAILED,
		tr("Unable to connect to the server \"%1\"."),
		tr("The SSL authentication failed. Try to connect to the server again. If the problem persists, contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_WRONG_CONNECTION_SECURITY_LEVEL,
		tr("Unable to connect to the server."),
		tr("The specified connection security level is lower than required by this server. Set the maximum possible security level (High) and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_IO_UNKNOWN_VM_ID,
		tr("The virtual machine \"%1\" is not available at the moment."),
		tr("Make sure that the connection to the respective physical server is established, you have the necessary permissions for the virtual machine, and the virtual machine files are present on the server's hard disk. If the problem persists, contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_QUESTION_LOST_TASK_CONTINUE,
		tr("The task %1 for the virtual machine \"%2\" was interrupted. Do you want to resume it?"),
		tr("The task was interrupted due to a server connection failure. Click Resume to finish the task or click Abort to discard it."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ONLY_ADMIN_CAN_SET_PARAMETER_STARTLOGINMODE_ROOT,
		tr("You are not allowed to set the startup and shutdown options for the local system account."),
		tr("To set these options, you need to have administrator privileges for this server. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ONLY_ADMIN_CAN_SET_VERBOSE_LOGGING,
		tr("You are not allowed to enable or disable verbose logging for this server."),
		tr("To enable or disable verbose logging, you need to have administrator rights for this server. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NEED_TO_SET_SPLIT_ON_FAT_FS_WITH_RECREATE,
		tr("Unable to recreate the virtual hard disk image \"%1\"."),
		tr("The image size (%2) exceeds the maximum file size (%3) allowed for the %4 file system on the target disk. Enable the Split option and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NEED_TO_RESET_DISK_PRM_ON_FAT_FS,
		tr("Unable to create the virtual hard disk image \"%1\"."),
		tr("The image size (%2) exceeds the maximum file size (%3) allowed for the %4 file system on the target disk. Enable the \"Split the disk image into 2 GB files\" option, and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_NEED_TO_RESET_DISK_PRM_ON_FAT_FS_ON_CUSTOM_BRANCH,
		tr("Unable to create the virtual machine."),
		tr("The predefined size of the virtual hard disk %1 exceeds the maximum size of %2 allowed for the %3 file system on the target disk. Go back to the virtual machine type selection step, select the Custom type, and then enable the \"Split the disk image into 2 GB files\" option on the disk size selection step."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ACCESS_DENIED_TO_CHANGE_PERMISSIONS,
		tr("Unable to change the permissions of the virtual machine \"%1\"."),
		tr("You do not have the necessary permissions to perform this operation. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP_SHUTDOWN_IN_PROCESS,
		tr("Unable to complete the operation."),
		tr("Virtuozzo is shutting down. Try again later."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISP_LOGON_ACTIONS_REACHED_UP_LIMIT,
		tr("Unable to connect to the server."),
		tr("The maximum number of simultaneous logon attempts has been reached. Try to connect to the server later."),
		false,
		PMT_CRITICAL);

///// Config validation ///////////////////////////////////////////////////////

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_UNKNOWN_OS_TYPE,
		tr("The guest operating system type is incorrect."),
		tr("Specify a guest operating system that is supported in Virtuozzo and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_UNKNOWN_OS_VERSION,
		tr("The version of the guest operating system is incorrect."),
		tr("Specify a guest operating system version that is supported in Virtuozzo and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_AUTOSTART_FROM_CURRENT_USER_FORBIDDEN,
		tr("You cannot configure virtual machine startup options under your current user account."),
		tr("To be able to configure these options, on the Startup and Shutdown pane, provide the credentials of the user having the permission to start the virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_REMOTE_DISPLAY_HOST_IP_ADDRESS_IS_ZERO,
		tr("The physical server IP address is not specified."),
		tr("Specify an IP address and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_REMOTE_DISPLAY_INVALID_HOST_IP_ADDRESS,
		tr("An incorrect physical server IP address is specified."),
		tr("Specify the correct IP address and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_CPU_COUNT_MORE_HOST_CPU_COUNT,
		tr("The specified number of CPUs is invalid."),
		tr("The number of CPUs cannot exceed the actual number of physical CPUs installed on the server. Correct the value and try again."),
		false,
		PMT_WARNING
		);

////////////////////////////////////////////////////////////////////////////////

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CHANGE_OWNER_OF_FILE,
		tr("Unable to change the virtual machine permissions."),
		tr("Virtuozzo cannot set the required permissions for one or more objects in the virtual machine folder. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_REMOVE_INVALID_VM_AS_NON_ADMIN,
		tr("You do not have the permission to remove the virtual machine from the list."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LPT_OUTPUT_FILE_NOT_EXIST,
		tr("The physical printer port '%1' used to emulate this virtual machine printer port is not available."),
		tr("Make sure that the corresponding parallel (LPT) port is present on the physical server and enabled in its hardware configuration."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ERR_CANT_SAVE_CONFIG_AS_VM_WAS_DELETED,
		tr("Unable to save the changes for the virtual machine \"%1\"."),
		tr("The virtual machine \"%1\" could not be found in the list of virtual machines registered on the physical server. Your changes to the configuration will be discarded."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CHANGE_OWNER_OF_VM_FILE,
		tr("Unable to change the virtual machine permissions."),
		tr("Virtuozzo cannot set the required permissions for one or more objects in the virtual machine folder. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CHANGE_OWNER_OF_DISK_IMAGE_FILE,
		tr("Unable to create the virtual disk image file \"%1\"."),
		tr("Virtuozzo cannot set the required access permissions for one or more objects in the virtual machine folder. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_CHANGE_FILE_PERMISSIONS,
		tr("Unable to change the virtual machine permissions."),
		tr("Virtuozzo cannot set the required permissions for one or more objects in the virtual machine folder. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_UNABLE_ALLOC_MEM_MONITOR,
		tr("Unable to allocate memory for VMM purposes."),
		tr("Virtuozzo cannot allocate enough memory for running the virtual machine. Suspend or stop one or more of the virtual machines currently running on the server and try again. If the problem persists, contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_MEMORY_QUOTA,
		tr("Invalid memory settings."),
		tr("Memory limit must not exceed the total amount of memory and video memory allocated to the virtual machine, and the priority must be set between 1 and 100."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_INVALID_MEMORY_GUARANTEE,
		tr("Invalid memory guarantee."),
		tr("The memory guarantee must not exceed the memory limit, while setting the memory guarantee below the expected virtual machine overhead may be dangerous in case of overcommitment."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_APPLY_MEMORY_GUARANTEE,
		tr("The memory guarantee cannot be applied."),
		tr("The physical server memory is currently overcommitted. Please suspend or shut down some of the running virtual machines and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LOW_MEMORY_LIMIT,
		tr("The memory limit is less than the total amount of memory. Are you sure you want to apply the changes?"),
		tr("It's recommended that the memory limit is equal to the total amount of memory, including the video memory, allocated to the virtual machine."),
		false,
		PMT_CRITICAL
		);


	REGISTER_ERROR_STRING(
		PRL_ERR_VM_LOAD_MONITOR,
		tr("Failed to load virtual machine monitor."),
		tr("@PRL_PRODUCT_NAME cannot load the virtual machine monitor due to internal incoherence. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_INIT_MONITOR,
		tr("Failed to init virtual machine monitor."),
		tr("@PRL_PRODUCT_NAME cannot start the virtual machine. Please contact the Parallels support team for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNSUPPORTED_NETWORK_FILE_SYSTEM,
		tr("You cannot use virtual machine files on the network share \"%1\"."),
		tr("Virtuozzo does not support using virtual machine files located on a network share. Select a path on your local disk and try again."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_GUEST_MAC_NOT_MACSERVER_HOST,
		tr("The virtual machine cannot be started."),
		tr("Virtual machines with a Mac OS X operating system can be run on Intel-based Macs only."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_NET_RESTORE_DEFAULTS_PARTITIAL_SUCCESS,
		tr("Not all the default settings could be restored."),
		tr("The server default network settings were restored only partially."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VLAN_UNSUPPORTED_IN_THIS_VERSION,
		tr("The network adapter \"%1\" cannot be used for bridged networking." ),
		tr("This is a VLAN interface. The current version of Virtuozzo does not support using VLAN interfaces for bridged networking."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_ALLOC_VM_MEMORY,
		tr("The requested system resources cannot be allocated to the virtual machine \"%1\"."),
		tr("There is not enough memory or disk space for the virtual machine to work correctly. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNABLE_SEND_REQUEST,
		tr("The operation could not be completed due to a critical error."),
		tr("Virtuozzo was unable to send data to the hypervisor driver. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_CANT_INITIALIZE_IMAGE,
		tr("Unable to initialize the hard disk %1."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_CANT_TO_CHANGE_PERMISSIONS_ON_REMOTE_LOCATION,
		tr("Unable to change the permissions for the virtual machine \"%1\"."),
		tr("The virtual machine permissions could not be changed because some of its files are stored on a network location. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNSUPPORTED_DEVICE_TYPE,
		tr("Unable to complete the operation."),
		tr("This type of devices is not supported in Virtuozzo."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_REGISTER_USED_VM,
		tr("Are you sure you want to use the virtual machine \"%1\" on this server?"),
		tr("The virtual machine might be used elsewhere. Before adding this virtual machine to this server, it is recommended to remove it from all the lists of virtual machines where it might be present."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_IPHONE_PROXY_CANNOT_START,
		tr("Unable to start the iPhone service."),
		tr("An error occurred while starting the iPhone service. Try to restart the physical server. If the problem persists, reinstall Virtuozzo or contact your Virtuozzo administrator for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_LICENSE_WRONG_VERSION,
		tr("This product could not be activated because your activation key is invalid for this version of Virtuozzo."),
		tr("Make sure you typed the key correctly or contact the Parallels support team for assistance."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_VM_COPY_OR_MOVE,
		tr("Have you copied or moved the virtual machine \"%1\"?"),
		tr("The virtual machine's files were previously stored in another location. To properly register the virtual machine on this server, indicate if it has been copied or moved here."),
		false,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_BOOTCAMP_WRITE_MBR,
		tr("Mac OS X has prevented the guest operating system from changing the boot loader on your physical hard disk."),
		tr("The guest operating system tried to modify the boot record area on the physical hard disk. For security reasons this operation was blocked by Mac OS X. Your changes will be saved in the virtual machine configuration, but not on the physical hard disk. Refer to the host operating system documentation for information on how to configure the boot loader on the physical hard disk."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VM_DIRECTORY_FOLDER_DOESNT_EXIST,
		tr("The folder \"%1\" used to store the virtual machine files is not available."),
		tr("Make sure that this folder exists and you have the permission to write to the folder."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_VMCONF_MAIN_MEMORY_SIZE_NOT_EAQUAL_RECOMMENDED,
		tr("You are trying to allocate %1 MB of RAM to this virtual machine. Are you sure you want to save these settings?"),
		tr("The operating system installed in this virtual machine requires %2 MB of RAM, while the maximum amount of RAM that can be allocated to a virtual machine on this physical server is %3 MB. Disregarding these recommendations may affect the performance of both your server and the virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_CANT_DISPLAY_HELP_TOPIC,
		tr("Unable to display help for this item." ),
		tr("The help file associated with this item was not found." ),
		false,
		PMT_INFORMATION
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_VERBOSE_LOGGING,
		tr("Verbose logging is enabled for the virtual machine \"%1\"."),
		tr("Running this virtual machine with verbose logging enabled may decrease your system performance. To disable verbose logging, go to <b>Server > Edit Settings > General</b>."),
		false,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_ID_WRNG_CDROM_CANNOT_CONNECT_FROM_CLIENT_SCSI,
		tr("CD/DVD %1 cannot be connected to the virtual machine now."),
		tr("A client computer CD/DVD device cannot be connected through the currently selected SCSI interface. To connect CD/DVD %1, please open its properties in the virtual machine configuration window, change the Interface option to IDE and restart the virtual machine for the change to take effect."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CREATE_BOOTABLE_ISO,
		tr("The CD/DVD disc is not yet ready."),
		tr("Make sure that the installation disc is inserted into your Mac's CD/DVD drive and this drive is not used by another virtual machine."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_LIBRARY,
		tr("Unable to load the iSCSI storage library."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_START,
		tr("Unable to start the iSCSI service."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_CREATE,
		tr("Unable to create a new iSCSI storage."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_REMOVE,
		tr("Unable to remove the iSCSI storage."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_MOUNT,
		tr("Unable to mount the iSCSI storage."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_UMOUNT,
		tr("Unable to unmount the iSCSI storage."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_EXTEND,
		tr("Unable to extend the iSCSI storage."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_GET_STATE,
		tr("Unable to retrieve the status of the iSCSI storage."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_INVALID_FSTYPE,
		tr("An invalid file system was specified for the iSCSI storage."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_NOT_FOUND,
		tr("The iSCSI storage could not be found."),
		tr("This iSCSI storage is not registered on this server. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_ALREADY_REGISTERED,
		tr("The iSCSI storage mount point already registered."),
		tr("This iSCSI storage mount point already registered on server. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_CANNOT_CREATE_MOUNT_POINT,
		tr("Unable to create the iSCSI storage mount point."),
		tr("Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_MOUNT_POINT_ALREADY_EXISTS,
		tr("The iSCSI storage mount point already exists."),
		tr("This iSCSI storage mount point already exists on server. Contact your Virtuozzo administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_TEMPLATE_HAS_APPS,
		tr("The OS template has related application templates."),
		tr("Unable to delete this OS template because it has one or more related application templates. Remove the application templates and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_TEMPLATE_NOT_FOUND,
		tr("The template could not be found."),
		tr("Make sure the template is installed on the server and its name is correct and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HVT_DISABLED_WARNING,
		tr("The virtual machine performance may be degraded because the hardware virtualization support is disabled on the host computer."),
		tr("It is recommended to enable the hardware virtualization support in BIOS settings."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_HVT_NOT_PRESENT_WARNING,
		tr("The host computer doesn't support VT-x."),
		tr("Please note that in this case you can run only 32-bit virtual machines with 1 virtual CPU."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		PRL_NET_VIRTUAL_NETWORK_SHARED_PROHIBITED,
		tr("Unable to create the shared network."),
		tr("Creating shared networks on this physical server is not supported."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ISCSI_STORAGE_MOUNTED,
		tr("Unable to mount the iSCSI storage."),
		tr("This iSCSI storage is already mounted. Try to unmount the iSCSI storage first, or contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_WARN_VM_SYNC_DEFAULT_PRINTER_DISABLED,
		tr("Your %1 default printer is no longer synchronized with your physical server default printer."),
		tr("The default printer might have changed. To restore synchronization, edit Printer settings in the virtual machine configuration accessible through the Virtual Machine > Configure menu."),
		true,
		PMT_WARNING
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REINSTALL_TOOLS_WIN,
		tr("You are going to reinstall Virtuozzo Tools. Do you want to continue?"),
		tr("Virtuozzo Tools are required for features that provide better integration of Parallels Management Console and Windows, like automatic mouse capture and dynamic screen resolution adjustment."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REINSTALL_TOOLS_LIN,
		tr("You are going to reinstall Virtuozzo Tools. Do you want to continue?"),
		tr("Virtuozzo Tools are required for features that provide better integration of Parallels Management Console and Linux, like automatic mouse capture and dynamic screen resolution adjustment."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REINSTALL_TOOLS_MAC,
		tr("You are going to reinstall Virtuozzo Tools. Do you want to continue?"),
		tr("Virtuozzo Tools are required for features that provide better integration of Parallels Management Console and the virtual machine, like automatic mouse capture and dynamic screen resolution adjustment."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_REINSTALL_TOOLS_OTHER,
		tr("You are going to reinstall Virtuozzo Tools. Do you want to continue?"),
		tr("Virtuozzo Tools are required for features that provide better integration of Parallels Management Console and the virtual machine, like automatic mouse capture and dynamic screen resolution adjustment."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_UPDATE_TOOLS_WIN,
		tr("You are going to update Virtuozzo Tools. Do you want to continue?"),
		tr("Virtuozzo Tools are required for features that provide better integration of Parallels Management Console and Windows, like automatic mouse capture and dynamic screen resolution adjustment."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_UPDATE_TOOLS_LIN,
		tr("You are going to update Virtuozzo Tools. Do you want to continue?"),
		tr("Virtuozzo Tools are required for features that provide better integration of Parallels Management Console and Linux, like automatic mouse capture and dynamic screen resolution adjustment."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_UPDATE_TOOLS_MAC,
		tr("You are going to update Virtuozzo Tools. Do you want to continue?"),
		tr("Virtuozzo Tools are required for features that provide better integration of Parallels Management Console and the virtual machine, like automatic mouse capture and dynamic screen resolution adjustment."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_INFO_UPDATE_TOOLS_OTHER,
		tr("You are going to update Virtuozzo Tools. Do you want to continue?"),
		tr("Virtuozzo Tools are required for features that provide better integration of Parallels Management Console and the virtual machine, like automatic mouse capture and dynamic screen resolution adjustment."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WNG_CONNECT_HID_USB_DEVICE,
		tr("Do you want to connect the USB device to the virtual machine?"),
		tr("If you connect your keyboard, mouse or USB ethernet adaptor to the virtual machine, you will not be able to use them on the physical server until they are released from the virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WNG_AUTOCONNECT_HID_USB_DEVICE,
		tr("Do you want to connect the USB device to the virtual machine?"),
		tr("The plugged-in USB device will be automatically connected to the virtual machine. If you connect your keyboard, mouse or USB ethernet adaptor to the virtual machine, you will not be able to use them on the physical server until they are released from the virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		GUI_WNG_ADD_HID_USB_DEVICE_TO_UDP_LIST,
		tr("Do you want this USB device to be automatically connected to the virtual machine?"),
		tr("When plugged in, this USB device will be automatically connected to the virtual machine. The device will not be available on the physical server until it is released from the virtual machine."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PET_QUESTION_REACH_OVERCOMMIT_STATE,
		tr( "Starting this virtual machine may slow down both the physical server and the virtual machines. Are you sure you want to start it?"),
		tr( "For more information, see <a href=\"%1\">@PRL_PRODUCT_NAME Help</a>."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_NET_ERR_ETH_NO_BINDABLE_ADAPTER,
		tr("Unable to use the network adapter %1."),
		tr("Make sure that the network adapter %1 is available and enabled on the physical server."),
		false,
		PMT_WARNING
		);

#ifdef _WIN_
	REGISTER_ERROR_STRING(
		PRL_WNG_START_VM_ON_MOUNTED_DISK,
		tr("While this virtual machine is running, do not disconnect the drive with its files from the physical server."),
		tr("The virtual machine files are located either on an removable storage device or on a network share. Make sure this device or network share is connected to the physical server all the time the virtual machine is running. Otherwise, @PRL_PRODUCT_NAME may work incorrectly and the data inside the virtual machine may become damaged."),
		true,
		PMT_WARNING
		);
#else
	REGISTER_ERROR_STRING(
		PRL_WNG_START_VM_ON_MOUNTED_DISK,
		tr("While this virtual machine is running, do not disconnect the volume with its files from the physical server."),
		tr("The virtual machine files are located either on a mounted volume or on a network share. Make sure this volume or network share is mounted to the physical server all the time the virtual machine is running. Otherwise, @PRL_PRODUCT_NAME may work incorrectly and the data inside the virtual machine may become damaged."),
		true,
		PMT_WARNING
		);
#endif // _WIN_

	REGISTER_ERROR_STRING(
		PRL_ERR_DISK_FILE_OPEN_ERROR,
		tr("Unable to access the virtual hard disk."),
		tr("Please make sure that there is at least 10 MB of free space on the physical server's hard disk, and you have enough rights to access the virtual hard disk."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		GUI_WRN_MAIN_MEMORY_IS_NOT_OPTIMAL,
		tr("The amount of memory allocated to this virtual machine (%1 MB) is outside of the recommended range. Do you want to change it back to the recommended value?"),
		tr("For best performance, it is recommended to allocate between %1 and %2 MB of memory. Allocating memory outside of this range may slow down both the virtual machine and host computer."),
		true,
		PMT_QUESTION
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_CLUSTER_RESOURCE_ERROR,
		tr("Failed to perform the operation."),
		tr("An error occurred when managing the cluster resource. Contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_UNNAMED_CT_MOVE,
		tr("Failed to move the Container."),
		tr("To move the Container, assign a unique name to it and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_FORCE_REG_ON_SHARED_STORAGE_IS_NEEDED,
		tr("Failed to register the virtual machine \"%1\" on the shared storage."),
		tr("You can force the registration, but this will revoke all access to the Virtual Machine from the original server and the High Availability cluster it belongs to."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BUSE_NOT_MOUNTED,
		tr( "The BUSE filesystem is not mounted." ),
		tr( "Make sure that the \"buse\" package is installed." ),
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
		PRL_ERR_ATTACH_BACKUP_INTERNAL_ERROR,
		tr("Failed to attach backup to the virtual machine due to an internal error."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ATTACH_BACKUP_INVALID_STORAGE_URL,
		tr("Failed to attach backup to the virtual machine due to the invalid storage URL."),
		tr("Please set the correct storage URL and try again."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ATTACH_BACKUP_CUSTOM_BACKUP_SERVER_NOT_SUPPORTED,
		tr("Cannot attach backup located on a remote server."),
		tr("Attaching backups from remote servers is not supported."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ATTACH_BACKUP_PROTO_ERROR,
		tr("Failed to attach backup to the virtual machine due to an internal protocol error."),
		tr("Please try again. If the problem persists, contact your system administrator for assistance."),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ATTACH_BACKUP_FORMAT_NOT_SUPPORTED,
		tr("Failed to attach backup to the virtual machine. The backup format is not supported."),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ATTACH_BACKUP_ALREADY_ATTACHED,
		tr( "Backup is already attached to the virtual machine." ),
		tr( "" ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ATTACH_BACKUP_BUSE_NOT_MOUNTED,
		tr( "Failed to attach backup to the virtual machine. The BUSE filesystem is not mounted." ),
		tr(""),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_BACKUP_RESTORE_PROHIBIT_WHEN_ATTACHED,
		tr( "Cannot restore the virtual machine from backup %1, because the backup is attached to a virtual machine." ),
		tr( "Detach the backup before trying to restore the virtual machine again." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ATTACH_BACKUP_URL_CHANGE_PROHIBITED,
		tr( "Changing parameters of attached backups is prohibited." ),
		tr( "" ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENCRYPTION_COMMIT_REJECTED,
		tr( "The virtual machine encryption parameters could not be applied." ),
		tr( "Some extra parameters of the virtual machine were changed. Only virtual machine encryption parameters can be altered when committing disk encryption changes." ),
		false,
		PMT_CRITICAL
		);

	REGISTER_ERROR_STRING(
		PRL_ERR_ENCRYPTION_COMMIT_PROHIBITED,
		tr( "Virtual machine encryption parameters cannot be changed along with other settings." ),
		tr( "Virtual machine encryption parameters must be altered separately from other virtual machine settings." ),
		false,
		PMT_CRITICAL
		);
}
