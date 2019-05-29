/*
 * PrlHandleVmDeviceHdPart.h
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


#ifndef __PARALLELS_HANDLE_VM_DEVICE_HD_PART_H__
#define __PARALLELS_HANDLE_VM_DEVICE_HD_PART_H__


#include "PrlHandleVmDeviceHardDrive.h"


/**
 * Virtual hard disk partiotion handle object
 */

class PrlHandleVmDeviceHdPart : public PrlHandleBase, public PrlControlValidity
{
public:

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleVmDeviceHdPart();

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param handle to hard drive which instantiated partition belongs
	 * @param [optional] pointer to the input XML configuration
	 */
	PrlHandleVmDeviceHdPart(const PrlHandleVmDeviceHardDrivePtr& pHardDrive,
							class CVmHddPartition* pPartition = 0);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 * Made overridable in view of correct child classes instnaces destruction
	 */
	~PrlHandleVmDeviceHdPart();

	/**
	 * Removes partition from list
	 *
	 * @return PRL_RESULT
	 */
	PRL_RESULT Remove();

	/**
	 * Returns partition system name
	 *
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM device system name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetSysName(PRL_STR sSysName, PRL_UINT32_PTR pnSysNameBufLength);

	/**
	 * Sets partition system name
	 *
	 * @param [in] new VM device system name string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - try to usage already removed VM device
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSysName(PRL_CONST_STR sSysName);

	/**
	 * Overridden method that marks partition object as invalid (sets to NULL storing device pointer)
	 */
	virtual void MarkAsInvalid();

private:

	/**
	 * Overridden method that should to generate unique hash code for an object
	 * @return generated hash code
	 */
	virtual quint32 ConcreteGenerateHashCode();
	/**
	 * Tries to restore internal wrapping XML model element
	 */
	void TryToRestoreObjectElement();

	/// Storing hard drive handle.
	PrlHandleVmDeviceHardDrivePtr m_pHardDrive;
	/// Storing virtual hard disk partiotion XML representation.
	class CVmHddPartition* m_pVmHddPartition;

};

typedef PrlHandleSmartPtr<PrlHandleVmDeviceHdPart> PrlHandleVmDeviceHdPartPtr;

#endif // __PARALLELS_HANDLE_VM_DEVICE_HD_PART_H__
