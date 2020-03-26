/*
 * PrlHandleVmDefaultConfig.h: Contains class, which implements
 * generation of Virtual Machine default settings and devices.
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


#ifndef __VIRTUOZZO_HANDLE_VM_DEFAULT_CONFIG__
#define __VIRTUOZZO_HANDLE_VM_DEFAULT_CONFIG__

#include <QList>

#include "PrlHandleBase.h"

class CVmConfiguration;
class CHostHardwareInfo;
struct StackIndexLimit;

/**
 * Implements generation of Virtual Machine default settings and devices.
 */
class PrlHandleVmDefaultConfig
{
public:
	/**
	 * Class default constructor.
	 */
	PrlHandleVmDefaultConfig ( PrlHandleSrvConfigPtr pSrvConfig, PrlHandleVmPtr pVm );

	/**
	 * Class destructor.
	 */
	~PrlHandleVmDefaultConfig ();

public:
	/**
	 * Creates default Virtual Machine configuration with
	 * settings and device set, specific to given \a osType and
	 * \a osVersion.
	 *
	 * @param osType - type of the guest OS to create VM config
	 *
	 * @param osVersion - version of the guest OS to create VM config
	 *
	 * @param needCreateDevices - specifies whether to create default set
	 * of devices for this VM configuration. If false is passed then required
	 * devices should be added manually by calling appropriate AddDefaultDevice
	 * function, which is defined below.
	 *
	 * @return XML representation of created VM configuration. Returns empty
	 * string if creation fails.
	 */
	QString CreateDefaultVmConfig ( uint osVersion,
									bool needCreateDevices );

	/**
	 * Adds new device of given type to the VM configuration. This function setup only
	 * basic device parameters if no valid server config handle was passed to obtain
	 * necessary information for setup such parameters as real device name etc.
	 *
	 * @param cfg - VM configuration to add the device
	 * @param devType - type of the device to add
	 */
	bool AddDefaultDevice ( CVmConfiguration& cfg, PRL_DEVICE_TYPE devType, PRL_HANDLE_PTR phDevice = 0 );

	/**
	 * Checks if device should present in the default VM configuration.
	 */
	static bool IsDefaultDeviceNeeded ( PRL_UINT32 osVersion, PRL_DEVICE_TYPE deviceType );
	/**
	 * Returns default Ram size for the current OS type and version,
	 * provided in the configuration.
	 *
	 * @return Default Ram size.
	 */
	static uint GetDefaultRamSize ( uint osVersion, uint hostRam );

	/**
	 * Returns default HDD size for the current OS type and version,
	 * provided in the configuration.
	 *
	 * @return Default HDD size.
	 */
	static uint GetDefaultHddSize ( uint osVersion );

	/**
	 * Returns default Video memory size for the current OS type and version,
	 * provided in the configuration.
	 * @param requested OS version
	 * @param host hardware info object
	 * @param sign whether 3D support will be enabled
	 *
	 * @return Default Video memory size.
	 */
	static PRL_UINT32 GetDefaultVideoRamSize ( PRL_UINT32 osVersion, const PrlHandleSrvConfigPtr &pSrvConfig, bool bIs3DSupportEnabled );

	/**
	* Returns default Video memory size for the current OS type and version,
	* provided in the configuration.
	* @param requested OS version
	* @param PRL_HOST_OS_TYPE nHostOsType - host os type
	* @param PRL_UINT32 hostRamSize - host os ram size
	* @param sign whether 3D support will be enabled
	*
	* @return Default Video memory size.
	*/
	static PRL_UINT32 GetDefaultVideoRamSize ( PRL_UINT32 osVersion,
												PRL_HOST_OS_TYPE nHostOsType,
												PRL_UINT32 hostRamSize,
												bool bIs3DSupportEnabled );

	/**
	* Returns default Video memory size for the current OS version,
	* @param requested OS version
	* @return Default Video memory size.
	*/
	static PRL_UINT32 GetDefaultVideoRamSize ( PRL_UINT32 osVersion );

private:
	/**
	 * Declares device type list that could be added to the VM configuration.
	 */
	typedef QList<PRL_DEVICE_TYPE> DeviceList;

private:
	/**
	 * Returns default Ram size for the current OS type and version,
	 * provided in the configuration.
	 *
	 * @return Default Ram size.
	 */
	static uint GetDefaultRamSize ( CVmConfiguration& cfg, uint hostRam );

	/**
	 * Returns default HDD size for the current OS type and version,
	 * provided in the configuration.
	 *
	 * @return Default HDD size.
	 */
	static uint GetDefaultHddSize ( CVmConfiguration& cfg );

	/**
	 * Setup default device list
	 */
	static void SetupDeviceDefaults ( CVmConfiguration& cfg, DeviceList& devList );

	/**
	 * Adds default floppy to the VM configuration.
	 *
	 * @param cfg - configuration to add device
	 */
	bool AddDefaultFloppy ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice = 0);

	/**
	 * Adds default CD-ROM to the VM configuration.
	 *
	 * @param cfg - configuration to add device
	 */
	bool AddDefaultCdRom ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice = 0 );

	/**
	 * Adds default hard disk to the VM configuration.
	 *
	 * @param cfg - configuration to add device
	 * @param devType - specific type of hard disk
	 */
	bool AddDefaultHardDisk ( CVmConfiguration& cfg, PRL_DEVICE_TYPE devType,
		PRL_HANDLE_PTR phDevice = 0 );

	/**
	 * @brief Adds default sound device to the VM configuration.
	 *
	 * @param cfg - configuration to add device
	 */
	bool AddDefaultSound ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice = 0 );

	/**
	 * Adds default USB device to the VM configuration.
	 *
	 * @param cfg - configuration to add device
	 */
	bool AddDefaultUsb ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice = 0 );

	/**
	 * @brief Adds default network adapter to the VM configuration.
	 *
	 * @param cfg - configuration to add device
	 */
	bool AddDefaultNetwork ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice = 0 );

	/**
	 * @brief Adds default parallel port to the VM configuration.
	 *
	 * @param cfg - configuration to add device
	 */
	bool AddDefaultParallel ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice = 0 );

	/**
	 * @brief Adds default serial port to the VM configuration.
	 *
	 * @param cfg - configuration to add device
	 */
	bool AddDefaultSerial ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice = 0 );

        /**
	 * @brief Adds default Generic pci device to the VM configuration.
	 *
	 * @param cfg - configuration to add device
	 */
        void AddDefaultGenericPciDevice(CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice = NULL);
 
	/**
	 * Setup default runtime options.
	 */
	void SetupRuntimeOptionsDefaults ( CVmConfiguration& cfg );

	/**
	 * Setup default booting options.
	 */
	void SetupBootingOptionsDefaults ( CVmConfiguration& cfg );

	/**
	 * Setup default tools options.
	 */
	void SetupToolsDefaults ( CVmConfiguration& cfg );

	/**
	 * Setup CPU default options.
	 */
	void SetupCpuDefaults ( CVmConfiguration& cfg );

	/**
	 * Setup default video options.
	 */
	void SetupVideoDefaults ( CVmConfiguration& cfg );

	/**
	 * Setup default memory size.
	 */
	void SetupMemoryDefaults ( CVmConfiguration& cfg );

	/**
	 * Setup default virtual printers options
	 */
	void SetupVirtualPrintersDefaults ( CVmConfiguration& cfg );

	/**
	 * Setup default USB controller options
	 */
	void SetupUsbControllerDefaults ( CVmConfiguration& cfg );

	/**
	 * Add hard disk index into device list, if it should present in the OS
	 * type and version, provided in the configuration.
	 *
	 * @param osVersion - version of the OS to setup defaults for
	 * @param devList - device list to add index
	 */
	static void SetupHardDiskDefaults ( uint osVersion, DeviceList& devList );

	/**
	 * Add floppy disk index into device list, if it should present in the OS
	 * type and version, provided in the configuration.
	 *
	 * @param osVersion - version of the OS to setup defaults for
	 * @param devList - device list to add index
	 */
	static void SetupFloppyDefaults ( uint osVersion, DeviceList& devList );

	/**
	 * Add CD-ROM disk index into device list, if it should present in the OS
	 * type and version, provided in the configuration.
	 *
	 * @param osVersion - version of the OS to setup defaults for
	 * @param devList - device list to add index
	 */
	static void SetupCdRomDefaults ( uint osVersion, DeviceList& devList );

	/**
	 * Add Sound device index into device list, if it should present in the OS
	 * type and version, provided in the configuration.
	 *
	 * @param osVersion - version of the OS to setup defaults for
	 * @param devList - device list to add index
	 */
	static void SetupSoundDefaults ( uint osVersion, DeviceList& devList );

	/**
	 * Add ParallelPort device index into device list, if it should present in the OS
	 * type and version, provided in the configuration.
	 *
	 * @param osVersion - version of the OS to setup defaults for
	 * @param devList - device list to add index
	 */
	static void SetupParallelPortDefaults ( uint osVersion, DeviceList& devList );

	/**
	 * Add SerialPort device index into device list, if it should present in the OS
	 * type and version, provided in the configuration.
	 *
	 * @param osVersion - version of the OS to setup defaults for
	 * @param devList - device list to add index
	 */
	static void SetupSerialPortDefaults ( uint osVersion, DeviceList& devList );

	/**
	 * Add network device index into device list, if it should present in the OS
	 * type and version, provided in the configuration.
	 *
	 * @param osVersion - version of the OS to setup defaults for
	 * @param devList - device list to add index
	 */
	static void SetupNetworkDefaults ( uint osVersion, DeviceList& devList );

	/**
	 * Add USB device index into device list, if it should present in the OS
	 * type and version, provided in the configuration.
	 *
	 * @param osVersion - version of the OS to setup defaults for
	 * @param devList - device list to add index
	 */
	static void SetupUsbDefaults ( uint osVersion, DeviceList& devList );

	/**
	 * Returns OS type from given configuration \a cfg.
	 *
	 * @return OS type.
	 */
	static uint GetOsType ( const CVmConfiguration& cfg );

	/**
	 * @brief Returns OS version from given configuration \a cfg.
	 *
	 * @return OS version.
	 */
	static uint GetOsVersion ( const CVmConfiguration& cfg );

	/**
	 * Checks if valid server handle passed to class instance.
	 */
	bool IsServerPresent () const;

	/**
	 * Returns device name
	 *
	 * @param Handle to device
	 */
	QString GetDeviceName ( PRL_HANDLE hDevice );

	/**
	 * Returns device id
	 *
	 * @param Handle to device
	 */
	QString GetDeviceId ( PRL_HANDLE hDevice );

	/**
	 * Get the first available stack index.
	 *
	 * @param Configuration to search in
	 * @param Interface type to check for free indexes
	 * @param Limits placed on iface and index
	 */
	int GetFreeStackIndex( const CVmConfiguration& cfg,
						   uint interfaceType,
						   StackIndexLimit *limit) const;

	/**
	* Get the interface slot for hdd \ cdrom.
	* @param [in]Configuration to search in
	* @param [out]Interface slot to check
	* @param [out]Interface type to check
	*/
	bool calculateInterfaceParamsForHddCdrom( const CVmConfiguration& cfg,
													PRL_DEVICE_TYPE devType,
													int & interfaceSlot,
													PRL_MASS_STORAGE_INTERFACE_TYPE & interfaceType );

	/**
	 * Creates default image name from pattern and device index.
	 *
	 * @param patternName - image pattern with index placeholder
	 * @param devIdx - device index
	 */
	QString GetDefaultImageName( const QString& patternName, uint devIdx );

	/**
	 * Creates default network name from pattern and device index.
	 *
	 * @param patternName - image pattern with index placeholder
	 * @param devIdx - device index
	 */
	QString GetDefaultNetworkName( const QString& patternName, uint devIdx );

	/**
	* Calculates default RAM size based on host RAM size.
	*
	* @param uint uiHostRam - host Ram Size
	*/
	static uint CalculateDefaultGuestRamSize( uint uiHostRam );

	/**
	* Calculates default RAM size for Windows guests based on host RAM size.
	*
	* @param uint uiHostRam - host Ram Size
	*/
	static uint CalculateDefaultWindowsRamSize( uint uiHostRam );

	/**
	* Calculates default RAM size for Mac OS guests based on host RAM size.
	*
	* @param uint uiHostRam - host Ram Size
	*/
	static uint CalculateDefaultMacRamSize( uint uiHostRam );

private:
	QString getDefaultBridgedNetwork();

	/**
	 *	Pointer to the server hardware configuration
	 */
	PrlHandleSrvConfigPtr m_pSrvConfig;
	/**
	 *	Pointer to the virtual machine configuration
	 *  It is used for create virtual device
	 */
	PrlHandleVmPtr m_pVm;
};

#endif // __VIRTUOZZO_HANDLE_VM_DEFAULT_CONFIG__
