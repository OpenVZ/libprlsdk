/*
 * PrlHandleVmInfo.h
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


#ifndef __PARALLELS_HANDLE_VM_INFO_H__
#define __PARALLELS_HANDLE_VM_INFO_H__

#include "PrlHandleBase.h"
#include "XmlModel/Messaging/CVmEvent.h"

/**
 * Virtual machine information (state and etc.) handle.
 */
class PrlHandleVmInfo : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * @param VM event data for internal initialization
	 */
	PrlHandleVmInfo ( CVmEvent *e );

	/**
	 * Returns VM state.
	 * @param pointer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetState ( VIRTUAL_MACHINE_STATE_PTR pVmState );

	/**
	* Returns VM addition state.
	* @param pointer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetAdditionState ( VIRTUAL_MACHINE_ADDITION_STATE_PTR pVmState );

	/**
	 * Returns VM state.
	 * @param pointer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetAccessRights ( PRL_HANDLE_PTR phVmAcl );

	/**
	* Returns is invalid_vm flag.
	* @param pointer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT IsInvalid ( PRL_BOOL_PTR pbIsInvalid, PRL_RESULT_PTR pnInvalidReason );

	/**
	* Returns is waiting for answer flag.
	* @param pointer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT IsVmWaitingForAnswer ( PRL_BOOL_PTR pbIsVmWaitingForAnswer );

	/**
	* Returns sign whether VNC server started.
	* @param pointer for storing result
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_NO_DATA - object do not has necessary data
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT IsVncServerSrtated ( PRL_BOOL_PTR pbIsVncServerStarted );

	/**
	* Initialize object from XML string
	*/
	virtual PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	* Returns object as XML string
	*/
	virtual QString toString();

private:
	/** VM information internal object */
	CVmEvent m_VmEvent;
};

#endif // __PARALLELS_HANDLE_VM_H__
