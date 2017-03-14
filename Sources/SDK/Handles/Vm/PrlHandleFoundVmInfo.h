/*
 * PrlHandleFoundVmInfo.h
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


#ifndef __PARALLELS_HANDLE_FOUND_VM_INFO_H__
#define __PARALLELS_HANDLE_FOUND_VM_INFO_H__

#include "PrlHandleBase.h"
#include <prlcommon/Messaging/CVmEvent.h>

/**
 * Found virtual machine information (name, OS version and etc.) handle. Using as result of search VM operation
 */
class PrlHandleFoundVmInfo : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * @param VM event data for internal initialization
	 */
	PrlHandleFoundVmInfo ( CVmEvent *e );

	/**
	* Returns VM name.
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully VM name)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetName ( PRL_STR sName, PRL_UINT32_PTR pnNameBufLength );

	/**
	* Returns sign whether VM config has previous version
	* @param [out] buffer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT IsOldConfig ( PRL_BOOL_PTR pbOldVersion );

	/**
	* Returns VM OS version.
	* @param [out] buffer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetOSVersion ( PRL_UINT32_PTR pnOSVersion );

	/**
	* Returns absolute path to VM config.
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully VM name)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetConfigPath ( PRL_STR sPath, PRL_UINT32_PTR pnPathBufLength );

	/**
	* Returns sign whether VM config for Template
	* @param [out] buffer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT IsTemplate ( PRL_BOOL_PTR pbIsTemplate );


private:
	/** VM information internal object */
	CVmEvent m_VmEvent;
};
typedef PrlHandleSmartPtr<PrlHandleFoundVmInfo> PrlHandleFoundVmInfoPtr;

#endif // __PARALLELS_HANDLE_FOUND_VM_INFO_H__
