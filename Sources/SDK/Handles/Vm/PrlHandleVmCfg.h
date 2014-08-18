/*
 * PrlHandleVmCfg.h
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


#ifndef __PARALLELS_HANDLE_VM_CFG_H__
#define __PARALLELS_HANDLE_VM_CFG_H__

#include "PrlHandleVm.h"


/**
 * Config edition virtual machine handle.
 */
class PrlHandleVmCfg : public PrlHandleVm
{
public:
	/**
	 * Set VM configuration
	 */
	PRL_RESULT SetConfig(PRL_HANDLE hVmCfg);

	/**
	 * Get VM configuration handle
	 */
	PRL_RESULT GetConfig(PRL_HANDLE_PTR phVmCfg);

	/**
	 * Generates and applies default VM configuration
	 */
	PRL_RESULT SetDefaultConfig(PrlHandleSrvConfigPtr pSrvConfig,
								PRL_UINT32 guestOsVerison, PRL_BOOL needCreateDevices);

	/**
	 * Get valid error code
	 */
	PRL_RESULT GetConfigValidity(PRL_RESULT_PTR pnErrCode);

	/**
	 * Adds default device to VM
	 */
	PRL_RESULT AddDefaultDevice(PrlHandleSrvConfigPtr pSrvConfig, PRL_DEVICE_TYPE deviceType);

	/**
	 * Adds default device to VM and returns it handle
	 */
	PRL_RESULT AddDefaultDeviceEx(PrlHandleSrvConfigPtr pSrvConfig, PRL_DEVICE_TYPE deviceType,
								  PRL_HANDLE_PTR phVmDevice);

	/**
	 * Returns VM uuid
	 */
	QString GetUuid();

	/**
	 * Returns Envid
	 */
	PRL_UINT32 GetEnvId();

	/**
	 * Helper method that lets to update security info configuration part from security info string representation
	 * @param VM security info string representation
	 */
	void UpdateSecurityFromVmSecurity(const QString &sVmSecurity);

	/**
	 * Helper method that lets to update security info configuration part from
	 string representation
	 * @param VM info string representation
	 */
	void UpdateSecurityFromVmInfo(const QString &sVmInfo);

	/**
	 * Removes boot items those corresponding to specified VM device element that wiil removed from VM configuration
	 * @param pointer to VM device element that removing from configuration
	 */
	void CleanBootDevicesList(CVmDevice *pDevice);

	// VM devices operations
	/**
	 * Returns VM devices count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNINITIALIZED - VM handle doesn't have hardware list
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDevicesCount(PRL_UINT32_PTR vmdev_handles_count);

	/**
	 * Returns all devices handles list
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to create a handle
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetAllDevices(PRL_HANDLE_PTR phDevsList);

	/**
	 * Returns VM devices list
	 * @param [out] buffer for storing result
	 * @param [out] buffer elements count (on in - how much elements can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNINITIALIZED - VM handle doesn't have hardware list
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM hardware list)
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate VM device object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDevicesList(PRL_HANDLE_PTR vmdev_handles, PRL_UINT32_PTR vmdev_handles_count);

	/**
	 * Returns VM config device count by it type
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetDevsCountByType(PRL_DEVICE_TYPE vmDeviceType, PRL_UINT32_PTR pnDevicesCount);

	/**
	 * Returns specified device by its type and index
	 * @param [in] device_type Type of the device
	 * @param [in] nIndex Index of the device
	 * @param [out] phDevice pointer to the HANDLE to store handle to the device
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS		- operation completed successfully
	 */
	PRL_RESULT GetDevByType( PRL_DEVICE_TYPE device_type, PRL_UINT32 nIndex, PRL_HANDLE_PTR phDevice );

	/**
	 * Returns VM config floppy disks count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetFloppyDisksCount(PRL_UINT32_PTR pnFloppyDisksCount);

	/**
	 * Returns floppy disk device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS		- operation completed successfully
	 */
	PRL_RESULT GetFloppyDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phFloppyDisk);

	/**
	 * Returns VM config hard disks count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetHardDisksCount(PRL_UINT32_PTR pnHardDisksCount);

	/**
	 * Returns hard disk device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetHardDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phHardDisk);

	/**
	 * Returns VM config optical disks count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetOpticalDisksCount(PRL_UINT32_PTR pnOpticalDisksCount);

	/**
	 * Returns optical disk device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetOpticalDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phOpticalDisk);

	/**
	 * Returns VM config parallel ports count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetParallelPortsCount(PRL_UINT32_PTR pnParallelPortsCount);

	/**
	 * Returns parallel port device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetParallelPort(PRL_UINT32 nIndex, PRL_HANDLE_PTR phParallelPort);

	/**
	 * Returns VM config serial ports count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetSerialPortsCount(PRL_UINT32_PTR pnSerialPortsCount);

	/**
	 * Returns serial port device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetSerialPort(PRL_UINT32 nIndex, PRL_HANDLE_PTR phSerialPort);

	/**
	 * Returns VM config sound devices count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetSoundDevsCount(PRL_UINT32_PTR pnSoundDevsCount);

	/**
	 * Returns sound device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetSoundDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phSoundDev);

	/**
	 * Returns VM config USB controllers count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetUsbDevicesCount(PRL_UINT32_PTR pnUsbDevicesCount);

	/**
	 * Returns USB controller device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetUsbDevice(PRL_UINT32 nIndex, PRL_HANDLE_PTR phUsbDevice);

	/**
	 * Returns VM config network adapters count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetNetAdaptersCount(PRL_UINT32_PTR pnNetAdaptersCount);

	/**
	 * Returns network adapter device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetNetAdapter(PRL_UINT32 nIndex, PRL_HANDLE_PTR phNetAdapter);

	/**
	 * Returns VM config generic PCI devices count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetGenericPciDevsCount(PRL_UINT32_PTR pnGenericPciDevsCount);

	/**
	 * Returns generic PCI device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetGenericPciDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phGenericPciDev);

	/**
	 * Returns VM config generic SCSI devices count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetGenericScsiDevsCount(PRL_UINT32_PTR pnGenericScsiDevsCount);

	/**
	 * Returns generic SCSI device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetGenericScsiDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phGenericScsiDev);

	/**
	 * Returns VM config display devices count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetDisplayDevsCount(PRL_UINT32_PTR pnDisplayDevsCount);

	/**
	 * Returns display device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetDisplayDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phDisplayDev);

	/**
	 * Creates shared folder instance
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate VM share object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT CreateShare(PRL_HANDLE_PTR phVmShare);

	/**
	 * Returns shared folders count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSharesCount(PRL_UINT32_PTR pnSharesCount);
	/**
	 * Returns share folder object
	 * @param [in] index of shared folder object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - shared folder object index out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetShare(PRL_UINT32 nShareIndex, PRL_HANDLE_PTR phVmShare);

	/**
	 * Returns sign whether VM user defined shared folders enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsUserDefinedSharedFoldersEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets VM user defined shared folders enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUserDefinedSharedFoldersEnabled(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether VM SmartMount feature enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsSmartMountEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets VM SmartMount feature enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSmartMountEnabled(PRL_BOOL bEnabled);

	PRL_RESULT IsSmartMountRemovableDrivesEnabled(PRL_BOOL_PTR pbEnabled);
	PRL_RESULT SetSmartMountRemovableDrivesEnabled(PRL_BOOL bEnabled);

	PRL_RESULT IsSmartMountDVDsEnabled(PRL_BOOL_PTR pbEnabled);
	PRL_RESULT SetSmartMountDVDsEnabled(PRL_BOOL bEnabled);

	PRL_RESULT IsSmartMountNetworkSharesEnabled(PRL_BOOL_PTR pbEnabled);
	PRL_RESULT SetSmartMountNetworkSharesEnabled(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether smart guard is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsSmartGuardEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets smart guard enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSmartGuardEnabled(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether smart guard should notify user before snapshot creation
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsSmartGuardNotifyBeforeCreation(PRL_BOOL_PTR pbNotifyBeforeCreation);

	/**
	 * Sets smart guard notification enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSmartGuardNotifyBeforeCreation(PRL_BOOL bNotifyBeforeCreation);

	/**
	 * Returns smart guard snapshot creation interval in seconds
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSmartGuardInterval(PRL_UINT32_PTR pnInterval);

	/**
	 * Sets smart guard snapshot creation interval in seconds
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSmartGuardInterval(PRL_UINT32 nInterval);

	/**
	 * Returns smart guard maximum storing snapshots count
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSmartGuardMaxSnapshotsCount(PRL_UINT32_PTR pnMaxSnapshotsCount);

	/**
	 * Sets smart guard maximum storing snapshots count
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSmartGuardMaxSnapshotsCount(PRL_UINT32 nMaxSnapshotsCount);

	/**
	 * Returns sign whether select boot device dialog will be shown on VM startup
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsAllowSelectBootDevice(PRL_BOOL_PTR pbAllowed);

	/**
	 * Sets select boot device dialog on VM startup enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAllowSelectBootDevice(PRL_BOOL bAllowed);

	/**
	 * Returns EFI boot option
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsEfiEnabled(PRL_BOOL_PTR pbEfiEnabled);

	/**
	 * Sets EFI boot option
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetEfiEnabled(PRL_BOOL bEfiEnabled);

	/**
	 * Returns external boot device system name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM DefaultGateway)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetExternalBootDevice(PRL_STR sSysName, PRL_UINT32_PTR pnSysNameBufLength);

	/**
	 * Sets external boot device system name
	 * @param [in] new locale string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetExternalBootDevice(PRL_CONST_STR sNewSysName);

	/**
	 * Returns sign whether shared profile is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsSharedProfileEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets shared profile enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSharedProfileEnabled(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether virtual links is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsVirtualLinksEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets virtual links enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetVirtualLinksEnabled(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether use desktop in shared profile is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsUseDesktop(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets use desktop in shared profile enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUseDesktop(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether use documents in shared profile is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsUseDocuments(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets use documents in shared profile enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUseDocuments(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether use pictures in shared profile is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsUsePictures(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets use pictures in shared profile enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUsePictures(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether use music in shared profile is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsUseMusic(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets use music in shared profile enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUseMusic(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether use downloads in shared profile is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsUseDownloads(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets use downloads in shared profile enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUseDownloads(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether use movies in shared profile is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsUseMovies(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets use movies in shared profile enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUseMovies(PRL_BOOL bEnabled);

	/**
	 * Returns optimize modifiers mode
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOptimizeModifiersMode(PRL_OPTIMIZE_MODIFIERS_MODE_PTR pnMode);

	/**
	 * Sets optimize modifiers mode
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetOptimizeModifiersMode(PRL_OPTIMIZE_MODIFIERS_MODE nMode);

	/**
	 * Returns sign whether clipboard sharing between guest and host is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsShareClipboard(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets clipboard sharing between guest and host enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetShareClipboard(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether time synchronization between guest and host is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsTimeSynchronizationEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets time synchronization between guest and host enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetTimeSynchronizationEnabled(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether time synchronization smart mode between guest and host is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsTimeSyncSmartModeEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets time synchronization smart mode between guest and host enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetTimeSyncSmartModeEnabled(PRL_BOOL bEnabled);

	/**
	 * Returns time synchronization between guest and host interval in seconds
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetTimeSyncInterval(PRL_UINT32_PTR pnTimeSyncInterval);

	/**
	 * Sets time synchronization between guest and host interval in seconds
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetTimeSyncInterval(PRL_UINT32 nTimeSyncInterval);

	/**
	 * Creates screen resolution instance
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate VM screen resolution object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT CreateScrRes(PRL_HANDLE_PTR phVmScrRes);

	/**
	 * Returns screen resolutions count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetScrResCount(PRL_UINT32_PTR pnScrResCount);

	/**
	 * Returns screen resolution object
	 * @param [in] index of screen resolution object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - screen resolution object index out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetScrRes(PRL_UINT32 nScrResIndex, PRL_HANDLE_PTR phVmScrRes);

	/**
	 * Creates boot device instance
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate VM  object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT CreateBootDev(PRL_HANDLE_PTR phVmBootDev);

	/**
	 * Returns boot devices count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetBootDevCount(PRL_UINT32_PTR pnBootDevCount);

	/**
	 * Returns boot device object
	 * @param [in] index of boot device object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - boot device object index out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetBootDev(PRL_UINT32 nBootDevIndex, PRL_HANDLE_PTR phVmBootDev);

	/**
	 * Returns VM name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetName(PRL_STR sVmName, PRL_UINT32_PTR pnVmNameBufLength);

	/**
	 * Sets new VM name
	 * @param [in] new VM name string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetName(PRL_CONST_STR sNewVmName);

	/**
	 * Returns VM hostname
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM hostname)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHostname(PRL_STR sVmHostname, PRL_UINT32_PTR pnVmHostnameBufLength);

	/**
	 * Sets new VM hostname
	 * @param [in] new VM hostname string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHostname(PRL_CONST_STR sNewVmHostname);

	/**
	 * Returns VM default gateway
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM DefaultGateway)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultGateway(PRL_STR sVmDefaultGateway, PRL_UINT32_PTR pnVmDefaultGatewayBufLength);

	/**
	 * Sets new VM default gateway
	 * @param [in] new VM DefaultGateway string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDefaultGateway(PRL_CONST_STR sNewVmDefaultGateway);

	/**
	 * Returns VM default gateway (IPv6)
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM DefaultGateway)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultGatewayIPv6(PRL_STR sVmDefaultGateway, PRL_UINT32_PTR pnVmDefaultGatewayBufLength);

	/**
	 * Sets new VM default gateway (IPv6)
	 * @param [in] new VM DefaultGateway string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDefaultGatewayIPv6(PRL_CONST_STR sNewVmDefaultGateway);

	/**
	 * Returns VM UUID
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM UUID)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUuid(PRL_STR sVmUuid, PRL_UINT32_PTR pnVmUuidBufLength);

	/**
	 * Returns parent VM UUID from which last one was linked cloned
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM UUID)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetLinkedVmUuid(PRL_STR sVmUuid, PRL_UINT32_PTR pnVmUuidBufLength);

	/**
	 * Returns VM guest OS type
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOsType(PRL_UINT32_PTR pnVmOsType);

	/**
	 * Returns VM guest OS version
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOsVersion(PRL_UINT32_PTR pnVmOsVersion);

	/**
	 * Sets VM guest OS version
	 * @param guest OS version number
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetOsVersion(PRL_UINT32 nVmOsVersion);

	/**
	 * Returns VM RAM memory size in megabytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRamSize(PRL_UINT32_PTR pnVmRamSize);

	/**
	 * Sets VM RAM memory size
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetRamSize(PRL_UINT32 nVmRamSize);

	/**
	 * Returns VM RAM Hotplug support enabling sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsRamHotplugEnabled(PRL_BOOL_PTR pbVmRamHotplugEnabled);

	/**
	 * Sets VM RAM Hotplug support enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetRamHotplugEnabled(PRL_BOOL bVmRamHotplugEnabled);

	/**
	 * Returns VM video memory size in megabytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetVideoRamSize(PRL_UINT32_PTR pnVmVideoRamSize);

	/**
	 * Sets VM video memory size
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetVideoRamSize(PRL_UINT32 nVmVideoRamSize);

	/**
	 * Returns minimal amount of host memory consumption by VM
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHostMemQuotaMin(PRL_UINT32_PTR pnHostMemQuotaMin);

	/**
	 * Sets minimal amount of host memory consumption by VM
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHostMemQuotaMin(PRL_UINT32 nHostMemQuotaMin);

	/**
	 * Returns maximal amount of host memory consumption by VM
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHostMemQuotaMax(PRL_UINT32_PTR pnHostMemQuotaMax);

	/**
	 * Sets maximal amount of host memory consumption by VM
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHostMemQuotaMax(PRL_UINT32 nHostMemQuotaMax);

	/**
	 * Returns priority of VM in host memory distribution
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHostMemQuotaPriority(PRL_UINT32_PTR pnHostMemQuotaPriority);

	/**
	 * Sets priority of VM in host memory distribution
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHostMemQuotaPriority(PRL_UINT32 nHostMemQuotaPriority);

	/**
	 * Returns whether VM's consumption of host memory autocalculated or not
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsHostMemAutoQuota(PRL_BOOL_PTR pbHostMemAutoQuota);

	/**
	 * Sets whether VM's consumption of host memory autocalculated or not
	 * @param setting value.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHostMemAutoQuota(PRL_BOOL bHostMemAutoQuota);

	/**
	 * Returns Max Balloon Size
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetMaxBalloonSize(PRL_UINT32_PTR pnMaxBalloonSize);

	/**
	 * Sets VM Max Balloon Size
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetMaxBalloonSize(PRL_UINT32 nMaxBalloonSize);

	/**
	 * Returns VM CPU count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuCount(PRL_UINT32_PTR pnVmCpuCount);

	/**
	 * Sets VM CPU count
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCpuCount(PRL_UINT32 nVmCpuCount);

	/**
	 * Returns VM CPU mode
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuMode(PRL_CPU_MODE_PTR pnVmCpuMode);

	/**
	 * Sets VM CPU mode.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCpuMode(PRL_CPU_MODE nVmCpuMode);

	/**
	 * Returns VM CPU acceleration level
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuAccelLevel(PRL_VM_ACCELERATION_LEVEL_PTR pnVmCpuAccelLevel);

	/**
	 * Sets VM CPU acceleration level
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCpuAccelLevel(PRL_VM_ACCELERATION_LEVEL nVmCpuAccelLevel);

	/**
	 * Returns VM CPU VTx support enabling sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsCpuVtxEnabled(PRL_BOOL_PTR pbVmCpuVtxEnabled);

	/**
	 * Sets VM CPU VTx support enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCpuVtxEnabled(PRL_BOOL bVmCpuVtxEnabled);

	/**
	 * Returns VM CPU Hotplug support enabling sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_NO_DATA - VM configuration do not has necessary data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsCpuHotplugEnabled(PRL_BOOL_PTR pbVmCpuHotplugEnabled);

	/**
	 * Sets VM CPU Hotplug support enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCpuHotplugEnabled(PRL_BOOL bVmCpuHotplugEnabled);

	/**
	 * Returns VM 3D acceleration support enabling sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT Is3DAccelerationEnabled(PRL_BOOL_PTR pbVm3DAccelerationEnabled);

	/**
	 * Sets VM 3D acceleration support enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT Set3DAccelerationEnabled(PRL_BOOL bVm3DAccelerationEnabled);

	/**
	 * Returns VM 3D acceleration support mode
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT Get3DAccelerationMode(PRL_VIDEO_3D_ACCELERATION_PTR pn3DAccelerationMode);


	/**
	 * Sets VM 3D acceleration support mode
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT Set3DAccelerationMode(PRL_VIDEO_3D_ACCELERATION n3DAccelerationMode);

	/**
	 * Check whether vertical synchronization setting is enabled
	 *
	 * @param pbEnabled - pointer to output buffer for storing the result
	 * @return PRL_RESULT
	 * @retval PRL_ERR_SUCCESS - operation completed successfully
	 * @retval PRL_ERR_INVALID_ARG - pbEnabled is NULL
	 */
	PRL_RESULT IsVerticalSynchronizationEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Enable or disable vertical synchronization setting
	 *
	 * @param bEnabled - setting value
	 * @return PRL_RESULT
	 * @retval PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetVerticalSynchronizationEnabled(PRL_BOOL bEnabled);

	/**
	 * Check whether high resolution setting is enabled
	 *
	 * @param pbEnabled - pointer to output buffer for storing the result
	 * @return PRL_RESULT
	 * @retval PRL_ERR_SUCCESS - operation completed successfully
	 * @retval PRL_ERR_INVALID_ARG - pbEnabled is NULL
	 */
	PRL_RESULT IsHighResolutionEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Enable or disable high resolution setting
	 *
	 * @param bEnabled - setting value
	 * @return PRL_RESULT
	 * @retval PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHighResolutionEnabled(PRL_BOOL bEnabled);

	/**
	 * Gets adaptive hypervisor enabled
	 */
	PRL_RESULT IsAdaptiveHypervisorEnabled(PRL_BOOL_PTR pbEnable);

	/**
	 * Sets adaptive hypervisor enabled
	 */
	PRL_RESULT SetAdaptiveHypervisorEnabled(PRL_BOOL bEnable);

	/**
	 * Gets Windows logo enabled
	 */
	PRL_RESULT IsSwitchOffWindowsLogoEnabled(PRL_BOOL_PTR pbEnable);

	/**
	 * Sets Windows logo enabled
	 */
	PRL_RESULT SetSwitchOffWindowsLogoEnabled(PRL_BOOL bEnable);

	/**
	 * Gets longer battery life enabled
	 */
	PRL_RESULT IsLongerBatteryLifeEnabled(PRL_BOOL_PTR pbEnable);

	/**
	 * Sets longer battery life enabled
	 */
	PRL_RESULT SetLongerBatteryLifeEnabled(PRL_BOOL bEnable);

	/**
	 * Gets battery status enabled
	 */
	PRL_RESULT IsBatteryStatusEnabled(PRL_BOOL_PTR pbEnable);

	/**
	 * Sets battery status enabled
	 */
	PRL_RESULT SetBatteryStatusEnabled(PRL_BOOL bEnable);

	/**
	 * Gets nested virtualization enabled
	 */
	PRL_RESULT IsNestedVirtualizationEnabled(PRL_BOOL_PTR pbEnable);

	/**
	 * Sets nested virtualization enabled
	 */
	PRL_RESULT SetNestedVirtualizationEnabled(PRL_BOOL bEnable);

	/**
	 * Gets PMU virtualization enabled
	 */
	PRL_RESULT IsPMUVirtualizationEnabled(PRL_BOOL_PTR pbEnable);

	/**
	 * Sets PMU virtualization enabled
	 */
	PRL_RESULT SetPMUVirtualizationEnabled(PRL_BOOL bEnable);

	/**
	 * Gets lock guest on suspend enabled
	 */
	PRL_RESULT IsLockGuestOnSuspendEnabled(PRL_BOOL_PTR pbEnable);

	/**
	 * Sets lock guest on suspend enabled
	 */
	PRL_RESULT SetLockGuestOnSuspendEnabled(PRL_BOOL bEnable);

	/**
	 * Gets isolated VM enabled
	 */
	PRL_RESULT IsIsolatedVmEnabled(PRL_BOOL_PTR pbEnable);

	/**
	 * Sets isolated VM enabled
	 */
	PRL_RESULT SetIsolatedVmEnabled(PRL_BOOL bEnable);

	/**
	 * Sets VM CPU units to Vm configuration
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCpuUnits(PRL_UINT32 nVmCpuUnits);

	/**
	 * Gets VM CPU units from Vm configuration
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuUnits(PRL_UINT32_PTR pnVmCpuUnits);

	/**
	 * Sets the specified virtual machine I/O priority.
	 * Priority range is 0-7. The greater priority is,
	 * the more time for I/O  activity Vm has.
	 * @param [out] buffer for storing result
	 */
	PRL_RESULT SetIoPriority(PRL_UINT32 nVmIoPriority);

	/**
	 * Gets VM I/O priority from Vm configuration
	 * @param [out] buffer for storing result
	 */
	PRL_RESULT GetIoPriority(PRL_UINT32_PTR pnVmIoPriority);

	/**
	 * Sets VM I/O limit in MB/s to Vm configuration
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetIoLimit(PRL_CONST_IOLIMIT_DATA_PTR pVmIoLimit);

	/**
	 * Gets VM I/O limit from Vm configuration
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetIoLimit(PRL_IOLIMIT_DATA_PTR pVmIoLimit);

	/**
	 * Sets VM IOPS limit to Vm configuration
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetIopsLimit(PRL_UINT32 nVmIopsLimit);
	/**
	 * Gets VM IOPS limit from Vm configuration
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetIopsLimit(PRL_UINT32_PTR pnVmIopsLimit);

	/**
	 * Sets VM CPU limit in % to Vm configuration
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCpuLimit(PRL_UINT32 nVmCpuLimit);

	/**
	 * Gets VM CPU limit from Vm configuration
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuLimit(PRL_UINT32_PTR pnVmCpuLimit);

	/**
	 * Sets VM CPU limit in Mhz to Vm configuration
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCpuLimitEx(PRL_CONST_CPULIMIT_DATA_PTR pVmCpuLimit);

	/**
	 * Gets VM CPU limit from Vm configuration
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuLimitEx(PRL_CPULIMIT_DATA_PTR pVmCpuLimit);

	PRL_RESULT SetCpuMask(PRL_CONST_STR sCpuMask);
	PRL_RESULT GetCpuMask(PRL_STR sMaskBuf, PRL_UINT32_PTR pnMaskBufLength);

	/**
	 * Returns VM configuration template sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 *   PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsTemplate(PRL_BOOL_PTR pbVmIsTemplate);

	/**
	 * Sets VM configuration template sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetTemplateSign(PRL_BOOL bVmIsTemplate);

	/**
	 * Returns server UUID where VM registered
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully server UUID)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetServerUuid(PRL_STR sServerUuid, PRL_UINT32_PTR pnServerUuidBufLength);

	/**
	 * Returns server hostname where VM registered
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully server hostname)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetServerHost(PRL_STR sServerHost, PRL_UINT32_PTR pnServerHostBufLength);

	/**
	 * Returns VM home path (where VM's file placed).
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM home pass)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHomePath(PRL_STR sHomePath, PRL_UINT32_PTR pnHomePathBufLength);

	/**
	 * Returns VM files.
	 * @param [out] buffer for storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetLocation(PRL_VM_LOCATION_PTR pnVmLocation);

	/**
	 * Returns VM icon
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM icon)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetIcon(PRL_STR sVmIcon, PRL_UINT32_PTR pnVmIconBufLength);

	/**
	 * Sets new VM icon
	 * @param [in] new VM icon string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetIcon(PRL_CONST_STR sNewVmIcon);

	/**
	 * Returns VM description
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM description)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDescription(PRL_STR sVmDescription, PRL_UINT32_PTR pnVmDescriptionBufLength);

	/**
	 * Sets new VM description
	 * @param [in] new VM description string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDescription(PRL_CONST_STR sNewVmDescription);

	/**
	 * Returns VM Custom Property
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM Custom Property)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCustomProperty(PRL_STR sVmCustomProperty, PRL_UINT32_PTR pnVmCustomPropertyBufLength);

	/**
	 * Sets new VM Custom Property
	 * @param [in] new VM Custom Property string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCustomProperty(PRL_CONST_STR sNewVmCustomProperty);

	/**
	 * Returns VM auto start mode
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetAutoStart(PRL_VM_AUTOSTART_OPTION_PTR pnVmAutoStart);

	/**
	 * Sets VM auto start mode.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAutoStart(PRL_VM_AUTOSTART_OPTION nVmAutoStart);

	/**
	 * Returns VM auto start delay
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetAutoStartDelay(PRL_UINT32_PTR pnVmAutoStartDelay);

	/**
	 * Sets VM auto start delay.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAutoStartDelay(PRL_UINT32 nVmAutoStartDelay);

	/**
	 * Returns VM start login mode
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetStartLoginMode(PRL_VM_START_LOGIN_MODE_PTR pnVmStartLoginMode);

	/**
	 * Sets VM start login mode.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetStartLoginMode(PRL_VM_START_LOGIN_MODE nVmStartLoginMode);

	/**
	 * Returns VM start user login
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM description)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetStartUserLogin(PRL_STR sStartUserLogin, PRL_UINT32_PTR pnStartUserLoginBufLength);

	/**
	 * Sets start user login and password
	 * @param [in] start user login
	 * @param [in] password
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetStartUserCreds(PRL_CONST_STR sStartUserLogin, PRL_CONST_STR sPassword);

	/**
	 * Returns VM auto stop mode
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetAutoStop(PRL_VM_AUTOSTOP_OPTION_PTR pnVmAutoStop);

	/**
	 * Sets VM auto stop mode.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAutoStop(PRL_VM_AUTOSTOP_OPTION nVmAutoStop);

	/**
	 * Returns action on Parallels Application window close
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetActionOnWindowClose(PRL_VM_ACTION_ON_WINDOW_CLOSE_PTR pnActionOnWindowClose);

	/**
	 * Sets action on Parallels Application window close
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetActionOnWindowClose(PRL_VM_ACTION_ON_WINDOW_CLOSE nActionOnWindowClose);

	/**
	 * Returns action on stop mode
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetActionOnStopMode(PRL_VM_ACTION_ON_STOP_PTR pnMode);

	/**
	 * Sets action on stop mode
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetActionOnStopMode(PRL_VM_ACTION_ON_STOP nMode);

	/**
	 * Returns VM window mode
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetWindowMode(PRL_VM_WINDOW_MODE_PTR pnVmWindowMode);

	/**
	 * Sets VM window mode.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetWindowMode(PRL_VM_WINDOW_MODE nVmWindowMode);

	/**
	 * Gets start in detached window enabled
	 */
	PRL_RESULT IsStartInDetachedWindowEnabled(PRL_BOOL_PTR pbEnable);

	/**
	 * Sets start in detached window enabled
	 */
	PRL_RESULT SetStartInDetachedWindowEnabled(PRL_BOOL bEnable);

	/**
	 * Returns sign whether lock in full screen mode feature enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsLockInFullScreenMode(PRL_BOOL_PTR pbValue);

	/**
	 * Sets lock in full screen mode feature enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetLockInFullScreenMode(PRL_BOOL bValue);

	/**
	 * Returns VM last modified date
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM last modified date)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetLastModifiedDate(PRL_STR sVmLastModifiedDate, PRL_UINT32_PTR pnVmLastModifiedDateBufLength);

	/**
	 * Returns user name that last modified VM.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully user name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetLastModifierName(PRL_STR sVmLastModifierName, PRL_UINT32_PTR pnVmLastModifierNameBufLength);

	/**
	 * Returns VM uptime counter started date
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM last modified date)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUptimeStartDate(PRL_STR sVmUptimeStartDate, PRL_UINT32_PTR pnVmUptimeStartDateBufLength);

	/**
	 * Returns VM uptime counter value
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUptime(PRL_UINT64_PTR pnVmUptime);

	/**
	 * Returns VM access rights object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetAccessRights(PRL_HANDLE_PTR phVmAcl);

	/**
	 * Returns VM guest OS sharing enabling sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsGuestSharingEnabled(PRL_BOOL_PTR pbVmGuestSharingEnabled);

	/**
	 * Sets VM guest OS sharing enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetGuestSharingEnabled(PRL_BOOL bVmGuestSharingEnabled);

	/**
	 * Returns VM guest OS sharing automount sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsGuestSharingAutoMount(PRL_BOOL_PTR pbVmGuestSharingAutoMount);

	/**
	 * Sets VM guest OS sharing automount sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetGuestSharingAutoMount(PRL_BOOL bVmGuestSharingAutoMount);

	/**
	 * Returns VM guest OS enable Spotlight sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsGuestSharingEnableSpotlight(PRL_BOOL_PTR pbVmGuestSharingEnableSpotlight);

	/**
	 * Sets VM guest OS enable Spotlight sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetGuestSharingEnableSpotlight(PRL_BOOL bVmGuestSharingEnableSpotlight);

	/**
	 * Returns VM host OS sharing enabling sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsHostSharingEnabled(PRL_BOOL_PTR pbVmHostSharingEnabled);

	/**
	 * Sets VM host OS sharing enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHostSharingEnabled(PRL_BOOL bVmHostSharingEnabled);

	/**
	 * Returns share all host disks sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsShareAllHostDisks(PRL_BOOL_PTR pbShareAllHostDisks);

	/**
	 * Sets share all host disks sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetShareAllHostDisks(PRL_BOOL bShareAllHostDisks);

	/**
	 * Returns share host user home dir sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsShareUserHomeDir(PRL_BOOL_PTR pbShareUserHomeDir);

	/**
	 * Sets share host user home dir sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetShareUserHomeDir(PRL_BOOL bShareUserHomeDir);

	/**
	 * Returns map shared host disks on drive letters sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsMapSharedFoldersOnLetters(PRL_BOOL_PTR pbMapSharedFoldersOnLetters);

	/**
	 * Sets map shared host disks on drive letters sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetMapSharedFoldersOnLetters(PRL_BOOL bMapSharedFoldersOnLetters);

	/**
	 * Gets VM remote display mode option.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRemoteDisplayMode(PRL_VM_REMOTE_DISPLAY_MODE_PTR pnVmRemoteDisplayMode);

	/**
	 * Sets VM remote display mode option.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetRemoteDisplayMode(PRL_VM_REMOTE_DISPLAY_MODE nVmRemoteDisplayMode);

	/**
 	 * Gets VM remote display password option.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM description)
	 * * PRL_ERR_SUCCESS - operation completed successfully
 	 */
 	PRL_RESULT GetRemoteDisplayPassword(PRL_STR sVmRemoteDisplayPasswordBuf, PRL_UINT32_PTR pnVmRemoteDisplayPasswordBufLength);

 	/**
 	 * Sets VM remote display password option.
	 * @param [in] new VM description string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
 	 */
 	PRL_RESULT SetRemoteDisplayPassword(PRL_CONST_STR sNewVmRemoteDisplayPassword);

 	/**
 	 * Gets VM remote display host name option.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM description)
	 * * PRL_ERR_SUCCESS - operation completed successfully
 	 */
 	PRL_RESULT GetRemoteDisplayHostName(PRL_STR sVmRemoteDisplayHostNameBuf, PRL_UINT32_PTR pnVmRemoteDisplayHostNameBufLength);

 	/**
 	 * Sets VM remote display host name option.
	 * @param [in] new VM description string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
 	 */
 	PRL_RESULT SetRemoteDisplayHostName(PRL_CONST_STR sNewVmRemoteDisplayHostName);

	/**
	 * Gets VM remote display port number option.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRemoteDisplayPortNumber(PRL_UINT32_PTR pnVmRemoteDisplayPortNumber);

	/**
	 * Sets VM remote display port number option.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetRemoteDisplayPortNumber(PRL_UINT32 nVmRemoteDisplayPortNumber);

	/**
	 * Returns VM additional screen resolutions support option.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsScrResEnabled(PRL_BOOL_PTR pbVmScrResEnabled);

	/**
	 * Sets VM additional screen resolutions support option.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetScrResEnabled(PRL_BOOL bVmScrResEnabled);

	/**
	 * Returns VM disk cache writeback option.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsDiskCacheWriteBack(PRL_BOOL_PTR pbVmDiskCacheWriteBack);

	/**
	 * Sets VM disk cache writeback option.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDiskCacheWriteBack(PRL_BOOL bVmDiskCacheWriteBack);

	/**
	 * Returns VM OS resolution in full screen mode sign.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsOsResInFullScrMode(PRL_BOOL_PTR pbVmOsResInFullScrMode);

	/**
	 * Sets VM OS resolution in full screen mode sign.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetOsResInFullScrMode(PRL_BOOL bVmOsResInFullScrMode);

	/**
	 * Returns VM close app on shutdown sign.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsCloseAppOnShutdown(PRL_BOOL_PTR pbVmCloseAppOnShutdown);

	/**
	 * Sets VM close app on shutdown sign.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCloseAppOnShutdown(PRL_BOOL bVmCloseAppOnShutdown);

	/**
	 * Returns VM system flags.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM system flags)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSystemFlags(PRL_STR sVmSystemFlags, PRL_UINT32_PTR pnVmSystemFlagsBufLength);

	/**
	 * Sets new VM system flags
	 * @param [in] new VM syste, flags string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSystemFlags(PRL_CONST_STR sNewVmSystemFlags);

	/**
	 * Returns VM configuration APIC sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 *   PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsDisableAPIC(PRL_BOOL_PTR pbDisableAPIC);

	/**
	 * Sets VM configuration APIC sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDisableAPICSign(PRL_BOOL bDisableAPIC);

	/**
	 * Returns VM configuration Speaker sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 *   PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsDisableSpeaker(PRL_BOOL_PTR pbDisableSpeaker);

	/**
	 * Sets VM configuration Speaker sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDisableSpeakerSign(PRL_BOOL bDisableSpeaker);

	/**
	 * Returns VM undo disks mode option.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUndoDisksMode(PRL_UNDO_DISKS_MODE_PTR pnUndoDisksMode);

	/**
	 * Sets VM undo disks mode option.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUndoDisksMode(PRL_UNDO_DISKS_MODE nUndoDisksMode);

	/**
	 * Returns VM foreground processes priority.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetForegroundPriority(PRL_VM_PRIORITY_PTR pnVmForegroundPriority);

	/**
	 * Sets VM application in dock mode coherence option.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetForegroundPriority(PRL_VM_PRIORITY nVmForegroundPriority);

	/**
	 * Returns VM background processes priority.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetBackgroundPriority(PRL_VM_PRIORITY_PTR pnVmBackgroundPriority);

	/**
	 * Sets VM application in dock mode coherence option.
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetBackgroundPriority(PRL_VM_PRIORITY nVmBackgroundPriority);

	/**
	 * Returns sign whether use default answers mechanism is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsUseDefaultAnswers(PRL_BOOL_PTR pbUseDefaultAnswers);

	/**
	 * Sets use default answers mechanism enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUseDefaultAnswers(PRL_BOOL bUseDefaultAnswers);

	/**
	 * Returns global search domains list
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings list object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSearchDomains(PRL_HANDLE_PTR phSearchDomainsList);

	/**
	 * Sets global search domains list
	 * @param [in] new DNS servers list
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSearchDomains(const QStringList &lstSearchDomains);

	/**
	 * Returns global dns servers list
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings list object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDnsServers(PRL_HANDLE_PTR phDnsServersList);

	/**
	 * Sets global dns server list
	 * @param [in] new DNS servers list
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDnsServers(const QStringList &lstDnsServers);

	/**
	 * Gets automatically applying configuration (IP addrs only) flag.
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsAutoApplyIpOnly(PRL_BOOL_PTR pbAutoApplyIpOnly);

	/**
	 * Sets VM dutomatically applying configuration (IP addrs only) flag.
	 *
	 * @param setting value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetAutoApplyIpOnly(PRL_BOOL bAutoApplyIpOnly);

	// Offline management helpers
	/**
	 * Returns sign whether offline management is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsOfflineManagementEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets offline management enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetOfflineManagementEnabled(PRL_BOOL bEnabled);

	PRL_RESULT SetNetworkRates(PRL_HANDLE phHandleList);
	PRL_RESULT GetNetworkRates(PRL_HANDLE_PTR phHandleList);
	PRL_RESULT IsRateBound(PRL_BOOL_PTR pbEnabled);
	PRL_RESULT SetRateBound(PRL_BOOL bEnabled);

	/**
	 * Returns sign whether Parallels Tools AutoUpdate is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsToolsAutoUpdateEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets Parallels Tools AutoUpdate enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetToolsAutoUpdateEnabled(PRL_BOOL bEnabled);

	/**
	 * Returns offline services list
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings list object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetOfflineServices(PRL_HANDLE_PTR phOfflineServicesList);

	/**
	 * Sets offline services list
	 * @param [in] new DNS servers list
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetOfflineServices(const QStringList &lstOfflineServices);

	/**
	* Obtains a handles list of operations which required administrator confirmation
	* @param [out]	A pointer to a variable that receives the handle.
		The type of the handle is PHT_OPAQUE_TYPE_LIST
	( values in list consist in enum PRL_ALLOWED_VM_COMMAND )
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetConfirmationsList(PRL_HANDLE_PTR phConfirmationsList);

	/**
	* Obtains a handles list of operations which required administrator confirmation
	* @param  pOpaqueList values in list consist in enum PRL_ALLOWED_VM_COMMAND
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT SetConfirmationsList(PrlHandleOpTypeListPtr pList);

	/**
	 * Returns sign whether auto compress is enabled
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsAutoCompressEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets auto compress enabling sign
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAutoCompressEnabled(PRL_BOOL bEnabled);

	/**
	 * Returns auto compress interval in seconds
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetAutoCompressInterval(PRL_UINT32_PTR pnInterval);

	/**
	 * Sets auto compress interval in seconds
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAutoCompressInterval(PRL_UINT32 nInterval);

	/**
	 * Returns auto compress free disk space ratio in percents
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetFreeDiskSpaceRatio(PRL_DOUBLE_PTR pdFreeDiskSpaceRatio);

	/**
	 * Sets auto compress free disk space ratio in percents
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetFreeDiskSpaceRatio(PRL_DOUBLE dFreeDiskSpaceRatio);

	/**
	* Returns VmInfo
	* @param [out] buffer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate object
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetVmInfo(PRL_HANDLE_PTR phVmInfo);

	/**
	 * Returns sign whether VM encrypted
	 * @param pointer to output buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsEncrypted(PRL_BOOL_PTR pbEncrypted);

	/**
	 * Returns locale for unattended installation
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM DefaultGateway)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUnattendedInstallLocale(PRL_STR sLocale, PRL_UINT32_PTR pnLocaleBufLength);

	/**
	 * Sets locale for unattended installation
	 * @param [in] new locale string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUnattendedInstallLocale(PRL_CONST_STR sLocale);

	/**
	 * Returns edition for unattended installation
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM DefaultGateway)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUnattendedInstallEdition(PRL_STR sEdition, PRL_UINT32_PTR pnEditionBufLength);

	/**
	 * Sets edition for unattended installation
	 * @param [in] new edition string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUnattendedInstallEdition(PRL_CONST_STR sEdition);

	PRL_RESULT SetVmType(PRL_VM_TYPE nType);
	PRL_RESULT GetVmType(PRL_VM_TYPE_PTR pnType);
	PRL_RESULT SetOstemplate(PRL_CONST_STR sOsTemplate);
	PRL_RESULT GetOstemplate(PRL_STR sOstemplate, PRL_UINT32_PTR pnBufLength);
	PRL_RESULT SetApplyConfigSample(PRL_CONST_STR sConfigSample);
	PRL_RESULT SetResourceById(PRL_CT_RESOURCE nResourceId, PRL_UINT64 nBarrier, PRL_UINT64 nLimit);
	PRL_RESULT GetResourceById(PRL_CT_RESOURCE nResourceId, PRL_UINT64_PTR pnBarrier, PRL_UINT64_PTR pnLimit);

	/**
	 * Returns list of application templates for Containers
	 * @param command flags
	 */
	PRL_RESULT GetAppTemplateList(PRL_HANDLE_PTR pAppList);

	/**
	 * Sets list of application templates for Containers
	 * @param command flags
	 */
	PRL_RESULT SetAppTemplateList(PRL_HANDLE hAppList);

	/**
	 * Check whether high availability setting is enabled
	 *
	 * @param pbEnabled - pointer to output buffer for storing the result
	 * @return PRL_RESULT
	 * @retval PRL_ERR_SUCCESS - operation completed successfully
	 * @retval PRL_ERR_INVALID_ARG - pbEnabled is NULL
	 */
	PRL_RESULT IsHighAvailabilityEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Enable or disable high availability setting
	 *
	 * @param bEnabled - setting value
	 * @return PRL_RESULT
	 * @retval PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHighAvailabilityEnabled(PRL_BOOL bEnabled);

	/**
	 * Get HA priority value
	 *
	 * @param pnPriority - pointer to output buffer for storing the result
	 * @return PRL_RESULT
	 * @retval PRL_ERR_SUCCESS - operation completed successfully
	 * @retval PRL_ERR_INVALID_ARG - pnPriority is NULL
	 */
	PRL_RESULT GetHighAvailabilityPriority(PRL_UINT32_PTR pnPriority);

	/**
	 * Set HA priority
	 *
	 * @param nPriority - HA priority value
	 * @return PRL_RESULT
	 * @retval PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHighAvailabilityPriority(PRL_UINT32 nPriority);

	/**
	 * Gets capabilities mask specified inside the CT.
	 * @param buffer for storing result.
	 */
	PRL_RESULT GetCapabilitiesMask(PRL_UINT32_PTR pnCapMask);

	/**
	 * Sets a capabilities inside the CT.
	 * @param full capabilities mask.
	 */
	PRL_RESULT SetCapabilitiesMask(PRL_UINT32 nCapMask);

	/**
	 * Gets netfilter mode specified inside the CT.
	 * @param pointer to output buffer for storing result.
	 */
	PRL_RESULT GetNetfilterMode(PRL_NETFILTER_MODE_PTR pnMode);

	/**
	 * Sets netfilter mode inside the CT.
	 * @param the netfilter mode to set.
	 */
	PRL_RESULT SetNetfilterMode(PRL_NETFILTER_MODE nMode);

	/**
	 * Gets features mask specified inside the CT.
	 * @param buffer for storing result.
	 */
	PRL_RESULT GetFeaturesMask(PRL_UINT32_PTR pnOn, PRL_UINT32_PTR nOff);

	/**
	 * Sets a features inside the CT.
	 * @param features mask.
	 */
	PRL_RESULT SetFeaturesMask(PRL_UINT32 nOn, PRL_UINT32 nOff);

	/**
	 * Gets current VM settings profile.
	 * @param pointer to profile
	 */
	PRL_RESULT GetProfile( PRL_VIRTUAL_MACHINE_PROFILE_PTR pnVmProfile );

	/**
	 * Checks that VM settings were modified after profile application.
	 * @param pointer to modification value
	 */
	PRL_RESULT IsCustomProfile( PRL_BOOL_PTR pbCustom );

	/**
	 * Sets predefined VM settings profile.
	 * @param predefined profile
	 */
	PRL_RESULT SetProfile( PrlHandleSrvConfigPtr pSrvConfig,
						   PRL_VIRTUAL_MACHINE_PROFILE nVmProfile );
};

#endif // __PARALLELS_HANDLE_VM_CFG_H__
