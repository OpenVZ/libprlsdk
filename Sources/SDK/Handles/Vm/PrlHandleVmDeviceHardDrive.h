/*
 * PrlHandleVmDeviceHardDrive.h
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


#ifndef __VIRTUOZZO_HANDLE_VM_DEVICE_HARD_DRIVE_H__
#define __VIRTUOZZO_HANDLE_VM_DEVICE_HARD_DRIVE_H__


#include "PrlHandleVmDevice.h"
#include "PrlHandleVirtualDiskEncryption.h"


/**
 * Hard drive VM device handle object.
 */
class PrlHandleVmDeviceHardDrive : public PrlHandleVmDevice
{

public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleVmDeviceHardDrive();

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param handle to VM which instantiated device belongs
	 * @param [optional] pointer to the input XML configuration
	 */
	PrlHandleVmDeviceHardDrive( const PrlHandleVmPtr &pVm, class CVmHardDisk *pHardDisk = 0 );

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 * Made overridable in view of correct child classes instnaces destruction
	 */
	~PrlHandleVmDeviceHardDrive();

	/**
	 * Initializes VM device object from XML string.
	 *
	 * @param XML representation of VM device
	 * @return PRL_RESULT
	 */
	PRL_RESULT fromString( PRL_CONST_STR sXml );

	/**
	 * Returns VM device representation as XML string.
	 */
	QString toString();

	/**
	 * Overridden method that marks device object as invalid (sets to NULL storing device pointer)
	 */
	void MarkAsInvalid();

public://SDK API helpers set
	/**
	 * Overriden method.
	 * Removes VM device from parent VM devices list.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT ConcreteRemove();

	/**
	 * Overridden method.
	 * Sets VM device emulated type.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType);

	/**
	 * Returns hard disk type (PLAIN or EXPANDING).
	 *
	 * @param pointer for storing value
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetDiskType(PRL_HARD_DISK_INTERNAL_FORMAT_PTR pnDiskType);

	/**
	 * Sets hard disk type (PLAIN or EXPANDING).
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetDiskType(PRL_HARD_DISK_INTERNAL_FORMAT nDiskType);

	/**
	 * Returns hard disk splitted sign (whether virtual device file storing as single file or splitted into chunks).
	 *
	 * @param pointer for storing value
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsSplitted(PRL_BOOL_PTR pbSplitted);

	/**
	 * Sets hard disk splitted sign (whether virtual device file storing as single file or splitted into chunks).
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetSplitted(PRL_BOOL bSplitted);

	/**
	 * Returns hard disk size in megabytes.
	 *
	 * @param pointer for storing value
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetDiskSize(PRL_UINT32_PTR pnDiskSize);

	/**
	 * Sets hard disk size in megabytes.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetDiskSize(PRL_UINT32 nDiskSize);

	PRL_RESULT GetBlockSize(PRL_UINT32_PTR pnBytes);
	PRL_RESULT SetBlockSize(PRL_UINT32 nBytes);

	/**
	 * Returns already filled hard disk size in bytes.
	 *
	 * @param pointer for storing value
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetSizeOnDisk(PRL_UINT32_PTR pnOnDiskSize);

	/**
	 * Adds a new partition to the virtual hard disk
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT AddPartition(PRL_HANDLE_PTR phPartition);

	/**
	 * Returns virtual hard disk partitions count
	 *
	 * @param pointer for storing value
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetPartitionsCount(PRL_UINT32_PTR pnPartitionsCount);

	/**
	 * Sets the hard disk mount point.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetMountPoint(PRL_CONST_STR sMountPoint);

	/**
	 * Return the hard disk mount point.
	 *
	 * @param buffer
	 * @param buffer length
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetMountPoint(PRL_STR sMountPoint, PRL_UINT32_PTR pnMountPointBufLength);

	/**
	 * Returns hard disk  utocompress sign.
	 *
	 * @param pointer for storing value
	 * @return PRL_RESULT
	 *                                              */
	PRL_RESULT SetAutoCompressEnabled(PRL_BOOL bEnabled);

	/**
	 * Sets hard disk autocompact enabled sign
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 *                                              */
	PRL_RESULT IsAutoCompressEnabled(PRL_BOOL_PTR bEnabled);

	/**
	 * Returns virtual hard disk partition
	 *
	 * @param device index
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG 	- device index is out of bounds
	 * * PRL_ERR_OUT_OF_MEMORY - couldn't to allocate memory for device object
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetPartition(PRL_UINT32 nIndex, PRL_HANDLE_PTR phPartition);

	/**
	 * Returns storing hard disk pointer.
	 */
	inline CVmHardDisk* GetHardDiskPtr() { return m_pVmHardDisk; }

	/**
	 * Return custom storage URL
	 *
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 *                           on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * (in this case the buffer will contain only a part of the URL)
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetStorageURL(PRL_STR sURL, PRL_UINT32_PTR pnURLBufLength);

	/**
	 * Set custom storage URL
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetStorageURL(PRL_CONST_STR sURL);

	/**
	 * Return disk serial number
	 *
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 *                           on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * (in this case the buffer will contain only a part of the serial number)
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSerialNumber(PRL_STR sSerialNumber, PRL_UINT32_PTR pnSerialNumberBufLength);

	/**
	* Set disk serial number
	*
	* @param assigning value
	* @return PRL_RESULT
	*/
	PRL_RESULT SetSerialNumber(PRL_CONST_STR sSerialNumber);

	/**
	 * Return disk encryption parameters
	 *
	 * @param [out] pointer to a handle for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetEncryption(PRL_HANDLE_PTR phEncryption);

	/**
	 * Set disk encryption parameters
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetEncryption(PrlHandleVirtualDiskEncryptionPtr hEncryption);

private:

	/// Storing VM hard disk XML representation.
	class CVmHardDisk *m_pVmHardDisk;

private:
	/**
	 * Overridden method that returns pointer to VM device XML model object
	 */
	class CVmDevice *GetVmDeviceElem();
	/**
	 * Overrridden method that returns pointer to VM clustered device XML model object
	 * @param return code that should be correctly filled
	 */
	class CVmClusteredDevice *GetVmClusteredDeviceElem(PRL_RESULT &nRetCode);
	/**
	 * Overridden method that returns pointer to VM storage device XML model object
	 * @param return code that should be correctly filled
	 */
	class CVmStorageDevice *GetVmStorageDeviceElem(PRL_RESULT &nRetCode);
	/**
	 * Overridden method that returns pointer to VM storage device XML model object
	 * @param return code that should be correctly filled
	 */
	class CVmMassStorageDevice *GetVmMassStorageDeviceElem(PRL_RESULT &nRetCode);

private://Internal class helpers
	/**
	 * Tries to restore internal wrapping XML model element
	 */
	void TryToRestoreObjectElement();
};

typedef PrlHandleSmartPtr<PrlHandleVmDeviceHardDrive> PrlHandleVmDeviceHardDrivePtr;

#endif // __VIRTUOZZO_HANDLE_VM_DEVICE_HARD_DRIVE_H__
