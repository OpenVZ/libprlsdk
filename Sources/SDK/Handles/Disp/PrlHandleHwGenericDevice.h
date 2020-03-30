/*
 * PrlHandleHwGenericDevice.h
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


#ifndef __VIRTUOZZO_HANDLE_HW_GENERIC_DEVICE_H__
#define __VIRTUOZZO_HANDLE_HW_GENERIC_DEVICE_H__


#include "PrlHandleSrvConfig.h"

/**
 * Host hardware info generic device handle object.
 */
class PrlHandleHwGenericDevice : public PrlHandleBase
{

public:
	/**
	 * Class constructor.
	 * Initializes generic device settings from specified object.
	 * @param parent server config object pointer
	 * @param pointer to generic device initialization data
	 */
	PrlHandleHwGenericDevice(const PrlHandleSrvConfigPtr &pSrvConfig, class CHwGenericDevice *pGenericDevice);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	virtual ~PrlHandleHwGenericDevice();

protected:
	/**
	 * Protected constructor for child classes usage.
	 * Initializes generic device settings from specified object.
	 * @param initializing handle type
	 * @param parent server config object pointer
	 * @param pointer to generic device initialization data
	 */
	PrlHandleHwGenericDevice(PRL_HANDLE_TYPE _handle_type, const PrlHandleSrvConfigPtr &pSrvConfig,
														class CHwGenericDevice *pGenericDevice);


public://API helpers
	/**
	 * Returns device name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetName(PRL_STR sHwGenericDeviceName, PRL_UINT32_PTR pnHwGenericDeviceNameBufLength);


	/**
	 * Returns device id.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetId(PRL_STR sHwGenericDeviceId, PRL_UINT32_PTR pnHwGenericDeviceIdBufLength);

	/**
	 * Returns device type
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDevType(PRL_DEVICE_TYPE_PTR pnDevType);

	/**
	 * Returns connection to VM status
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsConnectedToVm(PRL_BOOL_PTR pbConnected);

	/**
	 * Returns device state
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDeviceState(PRL_GENERIC_DEVICE_STATE_PTR pnDeviceState);

	/**
	 * Sets device state
	 * @param pointer for storing result
	 * @return PRL_RESULT. Possible values:
	 * *PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDeviceState(PRL_GENERIC_DEVICE_STATE nDeviceState);

	/**
	 * Initializes VM device object from XML string.
	 */
	PRL_RESULT fromString( PRL_CONST_STR sXml );

	/**
	 * Returns device object as XML string
	 */
	QString toString();

private:
	/// Storing generic device XML representation.
	class CHwGenericDevice *m_pHwGenericDevice;

protected:
	/// Pointer to the parent server config object
	PrlHandleSrvConfigPtr m_pSrvConfig;
};

typedef PrlHandleSmartPtr<PrlHandleHwGenericDevice> PrlHandleHwGenericDevicePtr;

#endif // __VIRTUOZZO_HANDLE_HW_GENERIC_DEVICE_H__
