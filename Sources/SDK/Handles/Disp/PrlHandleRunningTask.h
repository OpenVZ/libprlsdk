/*
 * PrlHandleRunningTask.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#ifndef __PARALLELS_HANDLE_RUNNING_TASK_H__
#define __PARALLELS_HANDLE_RUNNING_TASK_H__


#include "PrlHandleBase.h"
#include <prlcommon/Messaging/CVmEvent.h>


/**
 * Handle object represents RunningTask in PrlHandleLoginResponse info
 */
class PrlHandleRunningTask : public PrlHandleBase
{
public:
	/**
	 * Class default constructor.
	 * @param VM event data for internal initialization
	 */
	PrlHandleRunningTask ( CVmEvent *e );

	/**
	 * Class destructor.
	 */
	~PrlHandleRunningTask();

public://SDK helpers set
	/**
	* Returns task uuid string ( uses to restore session )
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully string value)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetTaskUuid(PRL_STR sTaskUuid, PRL_UINT32_PTR pnTaskUuidBufLength);

	/**
	* Get task type
	* @param specifies a pointer to type of task
	* @return PRL_ERR_SUCCESS if the method succeeds
	*/
	PRL_RESULT GetTaskType(PRL_RUNNING_TASK_TYPE_ENUM_PTR pType);

	/**
	* Returns task parameters as string
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully string value)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetTaskParametersAsString(PRL_STR sParamsStr, PRL_UINT32_PTR pnsParamsStrBufLength);

private:

	/** LoginResponse internal object */
	CVmEvent m_evt;
};

#endif // __PARALLELS_HANDLE_RUNNING_TASK_H__
