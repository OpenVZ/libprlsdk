/*
 * PrlHandleVmDeviceGenericPci.h
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


#ifndef __PARALLELS_HANDLE_VM_DEVICE_GENERIC_PCI_H__
#define __PARALLELS_HANDLE_VM_DEVICE_GENERIC_PCI_H__


#include "PrlHandleVmDevice.h"

/**
 * Generic PCI VM device handle object.
 */
class PrlHandleVmDeviceGenericPci : public PrlHandleVmDevice
{

public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleVmDeviceGenericPci();

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param handle to VM which instantiated device belongs
	 * @param [optional] pointer to the input XML configuration
	 */
	PrlHandleVmDeviceGenericPci( const PrlHandleVmPtr &pVm, class CVmGenericPciDevice *pGenericPci = 0 );

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 * Made overridable in view of correct child classes instnaces destruction
	 */
	~PrlHandleVmDeviceGenericPci();

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
	 * Overridden method.
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

private:

	/// Storing VM generic PCI device XML representation.
	class CVmGenericPciDevice *m_pVmGenericPci;

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

private://Internal class helpers
	/**
	 * Tries to restore internal wrapping XML model element
	 */
	void TryToRestoreObjectElement();
};

#endif // __PARALLELS_HANDLE_VM_DEVICE_GENERIC_PCI_H__
