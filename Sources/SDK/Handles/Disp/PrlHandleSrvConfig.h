/*
 * PrlHandleSrvConfig.h
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


#ifndef __PARALLELS_HANDLE_SRV_CONFIG_H__
#define __PARALLELS_HANDLE_SRV_CONFIG_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/HostHardwareInfo/CHostHardwareInfo.h>

#define SYNCHRO_PARENT_SRV_CONFIG QMutexLocker _lock(m_pSrvConfig->GetSynchroObject());

/**
 * Handle object represents server configuration
 */
class PrlHandleSrvConfig : public PrlHandleBase
{

public:
	/**
	 * Class default constructor.
	 */
	PrlHandleSrvConfig(const QString& sSrvConfig);

	/**
	 * Class constructor.
	 */
	PrlHandleSrvConfig();

	/**
	 * Class destructor.
	 */
	~PrlHandleSrvConfig();

	/**
	 * Returns host RAM size
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetHostRamSize(PRL_UINT32_PTR pnRamHostSize);

	/**
	 * Returns server host CPU model
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetCpuModel(PRL_STR sCpuModel, PRL_UINT32_PTR pnCpuModelBufLength);


	/**
	 * Returns host server system CPU count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetCpuCount(PRL_UINT32_PTR pnCpuCount);

	/**
	 * Returns host server system CPU speed
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetCpuSpeed(PRL_UINT32_PTR pnCpuSpeed);

	/**
	 * Returns host server system CPU mode
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetCpuMode(PRL_CPU_MODE_PTR pnCpuMode);

	/**
	 * Returns host server system VT-x mode
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetCpuVtxMode(PRL_CPU_VTX_MODE_PTR pnVtxMode);

	/**
	 * Returns host server system OS type
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetHostOsType(PRL_HOST_OS_TYPE_PTR pnHostOsType);

	/**
	 * Returns host server system OS major version
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetHostOsMajor(PRL_UINT32_PTR pnHostOsMajor);

	/**
	 * Returns host server system OS minor version
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetHostOsMinor(PRL_UINT32_PTR pnHostOsMinor);

	/**
	 * Returns host server system OS sub minor version
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetHostOsSubMinor(PRL_UINT32_PTR pnHostOsSubMinor);

	/**
	 * Returns server host OS string presentation
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHostOsStrPresentation(PRL_STR sHostOsStrPresentation, PRL_UINT32_PTR pnHostOsStrPresentationBufLength);

	/**
	 * Returns sign whether default sound device enabled at server host system
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT IsSoundDefaultEnabled(PRL_BOOL_PTR pbSoundDefaultEnabled);

	/**
	 * Returns sign whether USB subsystem supported at server host system
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT IsUsbSupported(PRL_BOOL_PTR pbUsbSupported);

	/**
	 * Returns sign whether Vt-d subsystem supported at server host system
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT IsVtdSupported(PRL_BOOL_PTR pbVtdSupported);

	/**
	 * Returns max possible network adapters count on the host
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetMaxHostNetAdapters(PRL_UINT32_PTR pnMaxNetAdaptersCount);

	/**
	 * Returns max possible virtual network adapters count which can be applied
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS if the method succeeds
	 */
	PRL_RESULT GetMaxVmNetAdapters(PRL_UINT32_PTR pnMaxNetAdaptersCount);

	/**
	 * Returns server hostname
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHostname(PRL_STR sHostname, PRL_UINT32_PTR pnHostnameBufLength);

	/**
	 * Returns server default gateway
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultGateway(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength);

	/**
	 * Returns server IPv6 default gateway
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultGatewayIPv6(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength);

	/**
	 * Returns global DNS servers list
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDnsServers(PRL_HANDLE_PTR phDnsServersList);

	/**
	 * Returns global search domains list
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSearchDomains(PRL_HANDLE_PTR phSearchDomainsList);

	/**
	 * Returns host server floppy disks count
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
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetFloppyDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phFloppyDisk);

	/**
	 * Returns host server optical disks count
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
	 * Returns host server serial ports count
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
	 * Returns host server parallel ports count
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
	 * Returns host server sound output devs count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetSoundOutputDevsCount(PRL_UINT32_PTR pnSoundOutputDevsCount);

	/**
	 * Returns sound output device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetSoundOutputDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phSoundOutputDev);

	/**
	 * Returns host server sound mixer devices count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetSoundMixerDevsCount(PRL_UINT32_PTR pnSoundMixerDevsCount);

	/**
	 * Returns sound mixer device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetSoundMixerDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phSoundMixerDev);

	/**
	 * Returns host server printers count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetPrintersCount(PRL_UINT32_PTR pnPrintersCount);

	/**
	 * Returns printer device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetPrinterDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phPrinter);

	/**
	 * Returns host server USB devices count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetUsbDevsCount(PRL_UINT32_PTR pnUsbDevsCount);

	/**
	 * Returns USB device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetUsbDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phUsbDev);

	/**
	 * Returns host server hard disks count
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
	 * Returns host server net adapters count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetNetAdaptersCount(PRL_UINT32_PTR pnNetAdaptersCount);

	/**
	 * Returns net adapter device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetNetAdapter(PRL_UINT32 nIndex, PRL_HANDLE_PTR phNetAdapter);

	/**
	 * Returns host server generic PCI devices count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetGenericPciDevicesCount(PRL_UINT32_PTR pnGenericPciDevicesCount);

	/**
	 * Returns generic PCI device device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetGenericPciDeviceDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phGenericPciDevice);

	/**
	 * Returns host server generic SCSI devices count
	 * @param pointer to buffer for storing result
	 * @return PRL_ERR_SUCCESS
	 */
	PRL_RESULT GetGenericScsiDevicesCount(PRL_UINT32_PTR pnGenericScsiDevicesCount);

	/**
	 * Returns generic SCSI device device
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetGenericScsiDeviceDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phGenericScsiDevice);

	/**
	 * To get the CPU features from server
 	 * @param [out] A pointer to structure with processor features
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - invalid handle.
	 * PRL_ERR_SUCCESS - function completed successfully.
	 */
	PRL_RESULT GetCpuFeatures(PRL_CPU_FEATURES_PTR pCpuFeatures);

	/**
	 * Returns the CPU features from server.
	 * @param [out] a pointer to a variable that receives a handle of type PHT_CPU_FEATURES.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate new object.
	 * * PRL_ERR_SUCCESS - function completed successfully.
	 */
	PRL_RESULT GetCpuFeaturesEx(PRL_HANDLE_PTR phCpuFeatures);

	/** Returns the CPU features masking capabilities from server.
	 * @param [out] a pointer to a variable that receives a handle of type PHT_CPU_FEATURES.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate new object.
	 * * PRL_ERR_SUCCESS - function completed successfully.
	 */
	PRL_RESULT GetCpuFeaturesMaskingCapabilities(PRL_HANDLE_PTR phCpuFeatures);
public:
	/**
	 * Returns pointer to internal server config data synchronization object
	 */
	inline QMutex *GetSynchroObject() {return (&m_HandleMutex);}
	/**
	 * Returns host hardware info object for internal SDK purposes
	 */
	CHostHardwareInfo &GetSrvConfig();

	/**
	 * Read object representation from string
	 */
	PRL_RESULT fromString(PRL_CONST_STR vm_config);

	/**
	 * Returns XML as string
	 */
	QString toString();

private:
	/// SrvConfig object
	CHostHardwareInfo m_SrvConfig;
};

#endif // __PARALLELS_HANDLE_SRV_CONFIG_H__
