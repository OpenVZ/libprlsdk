/*
 * PrlHandleVmDevice.h
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


#ifndef __VIRTUOZZO_HANDLE_DEVICE_H__
#define __VIRTUOZZO_HANDLE_DEVICE_H__

#include "PrlHandleVmSrv.h"
#include "PrlControlValidity.h"

#include <QString>

#define SYNCHRO_VMDEV_DATA_ACCESS\
	QMutex *pSynchroAccessObject = NULL;\
	if(m_pVm.getHandle())\
		pSynchroAccessObject = m_pVm->GetSynchroObject();\
	else\
		pSynchroAccessObject = &m_HandleMutex;\
	QMutexLocker _lock(pSynchroAccessObject);

#define SYNCHRO_VMDEV_ITEM_IDS\
	if (m_pVm)\
		m_pVm->GetVmConfig().getVmHardwareList()->syncItemIds();

/**
 * Generic VM device handle object.
 */
class PrlHandleVmDevice : public PrlHandleBase, public PrlControlValidity
{

public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param VM device type
	 */
	PrlHandleVmDevice( PRL_HANDLE_TYPE hType );

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param handle to VM which instantiated device belongs
	 * @param VM device type
	 */
	PrlHandleVmDevice( const PrlHandleVmPtr &pVm, PRL_HANDLE_TYPE hType );

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 * Made overridable in view of correct child classes instances destruction
	 */
	virtual ~PrlHandleVmDevice();

	/**
	 * Initializes VM device object from XML string.
	 *
	 * @param XML representation of VM device
	 * @return PRL_RESULT
	 */
	virtual PRL_RESULT fromString( PRL_CONST_STR sXml ) = 0;

	/**
	 * Returns VM device representation as XML string.
	 */
	virtual QString toString() = 0;

	/**
	 * Common logic for fromString functionality
	 * @param device XML representation string
	 * @return operation return code
	 */
	PRL_RESULT fromStringCommon( PRL_CONST_STR sXml );

	/**
	 * Common logic for toString functionality
	 * @param pointer to retrieve device XML representation string
	 * @return operation return code
	 */
	PRL_RESULT toStringCommon( PRL_VOID_PTR_PTR sXml );

public://SDK API helpers set
	/**
	 * Creates VM device instance
	 * @param pointer to parent vm object
	 * @param creating VM device type
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - unknown device type specified
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate VM device object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	static PRL_RESULT CreateVmDevice(PrlHandleVmPtr pVm, PRL_DEVICE_TYPE device_type, PRL_HANDLE_PTR phVmDevice);

	/**
	 * Returns storing VM handle.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetVm( PRL_HANDLE_PTR vm_handle );

	/**
	 * Returns VM device index.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetIndex( PRL_UINT32_PTR nIndex );

	/**
	 * Sets VM device index.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetIndex( PRL_UINT32 nIndex );

	/**
	 * Removes VM device from parent VM devices list
	 *
	 * @return PRL_RESULT
	 */
	PRL_RESULT Remove();

	/**
	 * Concrete implementation of removing VM device from parent VM devices list.
	 *
	 * @return PRL_RESULT
	 */
	virtual PRL_RESULT ConcreteRemove();

	/**
	 * Gets device type
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetDeviceType(PRL_DEVICE_TYPE_PTR pnDeviceType);

	/**
	 * Checks whether VM device connected.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsConnected(PRL_BOOL_PTR pbConnected);

	/**
	 * Sets VM device connected sign.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetConnected(PRL_BOOL bConnected);

	/**
	 * Checks whether VM device enabled.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets VM device enabled sign.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetEnabled(PRL_BOOL bEnabled);

	/**
	 * Checks whether VM device remote.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsRemote(PRL_BOOL_PTR pbRemote);

	/**
	 * Sets VM device remote sign.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetRemote(PRL_BOOL bRemote);

	/**
	 * Gets VM device emulated type.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetEmulatedType(PRL_VM_DEV_EMULATION_TYPE_PTR pnEmulatedType);

	/**
	 * Sets VM device emulated type.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	virtual PRL_RESULT SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType);

	/**
	 * Returns VM device system name
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
	 * Sets new VM device system name
	 * @param [in] new VM device system name string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - try to usage already removed VM device
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSysName(PRL_CONST_STR sNewSysName);

	/**
	 * Returns VM device user friendly name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM device system name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetFriendlyName(PRL_STR sFriendlyName, PRL_UINT32_PTR pnFriendlyNameBufLength);


	/**
	 * Sets new VM device user friendly name
	 * @param [in] new VM device system name string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - try to usage already removed VM device
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetFriendlyName(PRL_CONST_STR sNewFriendlyName);

	/**
	 * Returns VM device description.
	 * This call applyable just for handles of PHT_VIRTUAL_DEV_GENERIC_PCI or
	 * PHT_VIRTUAL_DEV_GENERIC_SCSI type.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM device description)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDescription(PRL_STR sDescription, PRL_UINT32_PTR pnDescriptionBufLength);


	/**
	 * Sets new VM device description.
	 * This call applyable just for handles of PHT_VIRTUAL_DEV_GENERIC_PCI or
	 * PHT_VIRTUAL_DEV_GENERIC_SCSI type.
	 * @param [in] new VM device description string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - try to usage already removed VM device
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDescription(PRL_CONST_STR sNewDescription);

	/**
	 * Gets VM device interface type.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetIfaceType(PRL_MASS_STORAGE_INTERFACE_TYPE_PTR pnIfaceType);

	/**
	 * Sets VM device interface type.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetIfaceType(PRL_MASS_STORAGE_INTERFACE_TYPE nIfaceType);

	/**
	 * Gets VM device subtype.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetSubType(PRL_CLUSTERED_DEVICE_SUBTYPE_PTR pnSubType);

	/**
	 * Sets VM device subtype.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetSubType(PRL_CLUSTERED_DEVICE_SUBTYPE nSubType);

	/**
	 * Returns VM device index.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetStackIndex( PRL_UINT32_PTR nStackIndex );

	/**
	 * Sets VM device stack index.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetStackIndex( PRL_UINT32 nStackIndex );

	/**
	 * Lets to generate and set VM device stack index for corresponding interface type.
	 *
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetDefaultStackIndex();

	/**
	 * Returns passthrough mode enabling sign.
	 *
	 * @param pointer for storing value
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsPassthrough(PRL_BOOL_PTR pbPassthrough);

	/**
	 * Sets passthrough mode enabling sign.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetPassthrough(PRL_BOOL bPassthrough);

public:
	/**
	 * Sets storing VM handle.
	 *
	 * @param pointer to VM object
	 */
	void SetVm( PrlHandleVmPtr pVm );

	/**
	 * Returns storing VM pointer.
	 */
	inline PrlHandleVmPtr &GetVmPtr() {return (m_pVm);}

	/**
	 * Returns storing VM server pointer.
	 */
	inline PrlHandleVmSrvPtr GetVmSrvPtr()
	{ return PrlHandleVmSrvPtr( (PrlHandleVmSrv* )m_pVm.getHandle() ); }

public://Public device operations
	/**
	 * Connects VM device
	 */
	PrlHandleJobPtr Connect();
	/**
	 * Disconnects VM device
	 */
	PrlHandleJobPtr Disconnect();

	/**
	 * Abstract method that returns pointer to VM device XML model object
	 */
	virtual class CVmDevice *GetVmDeviceElem() = 0;

protected:

	/// Storing VM handle.
	PrlHandleVmPtr m_pVm;

private:

	/**
	 * Overloadable method that returns pointer to VM clustered device XML model object
	 * @param return code that should be correctly filled
	 */
	virtual class CVmClusteredDevice *GetVmClusteredDeviceElem(PRL_RESULT &nRetCode);
	/**
	 * Overloadable method that returns pointer to VM storage device XML model object
	 * @param return code that should be correctly filled
	 */
	virtual class CVmStorageDevice *GetVmStorageDeviceElem(PRL_RESULT &nRetCode);
	/**
	 * Overloadable method that returns pointer to VM storage device XML model object
	 * @param return code that should be correctly filled
	 */
	virtual class CVmMassStorageDevice *GetVmMassStorageDeviceElem(PRL_RESULT &nRetCode);
	/**
	 * Overloadable method that returns pointer to VM port device XML model object
	 * @param return code that should be correctly filled
	 */
	virtual class CVmPort *GetVmPortDeviceElem(PRL_RESULT &nRetCode);

private:
	/**
	 * Overridden method that should to generate unique hash code for an object
	 * @return generated hash code
	 */
	virtual quint32 ConcreteGenerateHashCode();
};

#endif // __VIRTUOZZO_HANDLE_DEVICE_H__
