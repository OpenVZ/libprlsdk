/*
 * PrlHandleEventParam.h
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


#ifndef __PARALLELS_HANDLE_EVENT_PARAM_H__
#define __PARALLELS_HANDLE_EVENT_PARAM_H__


#include "PrlHandleVmEvent.h"
#include "PrlControlValidity.h"

/**
 * Event parameter handle object.
 */
class PrlHandleEventParam : public PrlHandleBase, public PrlControlValidity
{
public:
	/**
	 * Class constructor.
	 * Initializes event parameter settings from specified object.
	 * @param parent VM event object pointer
	 */
	PrlHandleEventParam(const PrlHandleVmEventPtr &pVmEvent, class CVmEventParameter *pEventParam);

	/**
	 * Class constructor.
	 * Initializes event parameter settings from specified object.
	 */
	PrlHandleEventParam(class CVmEventParameter *pEventParam);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleEventParam();

public://API helpers
	/**
	 * Returns parameter name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size) or attempt to access to object contains invalid data
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetName(PRL_STR sEventParamName, PRL_UINT32_PTR pnEventParamNameBufLength);


	/**
	 * Returns parameter type
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - attempt to access to object contains invalid data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetParamType(PRL_PARAM_FIELD_DATA_TYPE_PTR pnParamType);


	/**
	 * Gets parameter value string representation
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size) or attempt to access to object contains invalid data
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT ToString(PRL_STR sEventParamValue, PRL_UINT32_PTR pnEventParamValueBufLength);

	/**
	* Gets parameter value CData as string representation
	* @param [out] buffer for storing result
	* @param [out] buffer size (on in - how much symbols can be placed into buffer;
	* on out - how much were placed actually)
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size) or attempt to access to object contains invalid data
	* * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	* * non fully string value)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT ToCData(PRL_STR sEventParamValue, PRL_UINT32_PTR pnEventParamValueBufLength);


	/**
	 * Returns parameter value PRL_UINT32 representation
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - attempt to access to object contains invalid data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT ToUint32(PRL_UINT32_PTR pnParamValue);


	/**
	 * Returns parameter value PRL_INT representation
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - attempt to access to object contains invalid data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT ToInt32(PRL_INT32_PTR pnParamValue);


	/**
	 * Returns parameter value PRL_UINT32 representation
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - attempt to access to object contains invalid data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT ToUint64(PRL_UINT64_PTR pnParamValue);


	/**
	 * Returns parameter value PRL_INT representation
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - attempt to access to object contains invalid data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT ToInt64(PRL_INT64_PTR pnParamValue);


	/**
	 * Returns parameter value PRL_BOOL representation
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - attempt to access to object contains invalid data
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT ToBoolean(PRL_BOOL_PTR pbParamValue);


	/**
	 * Returns parameter value as handle to corresponding object stroing in event parameter
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - attempt to access to object contains invalid data or non PFD_ENTITY object type
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT ToHandle(PRL_HANDLE_PTR phParamValue);

	/**
	 * Gets parameter value as binary buffer
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much bytes can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size) or attempt to access to object contains invalid data
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetBuffer(PRL_VOID_PTR pEventParamValue, PRL_UINT32_PTR pnEventParamValueBufLength);


private:
	/// Storing VM event param XML object.
	class CVmEventParameter *m_pEventParam;
	/// Pointer to the parent VM event object
	PrlHandleVmEventPtr m_pVmEvent;

private:
	/**
	 * Overridden method that marks device object as invalid (sets to NULL storing screen resolution pointer)
	 */
	void MarkAsInvalid();

	/**
	 * Overridden method that should to generate unique hash code for an object
	 * @return generated hash code
	 */
	quint32 ConcreteGenerateHashCode();
};

typedef PrlHandleSmartPtr<PrlHandleEventParam> PrlHandleEventParamPtr;

#endif // __PARALLELS_HANDLE_EVENT_PARAM_H__
