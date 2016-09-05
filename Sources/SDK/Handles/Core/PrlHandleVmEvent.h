/*
 * PrlHandleVmEvent.h
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


#ifndef __PARALLELS_HANDLE_VM_EVENT_H__
#define __PARALLELS_HANDLE_VM_EVENT_H__


#include "PrlHandleEvent.h"
#include <prlcommon/Messaging/CVmEvent.h>

#include <QRegExp>

/**
 * VM event handle object.
 */
class PrlHandleVmEvent : public PrlHandleEvent
{

public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param pointer to VM object with which event will be bint
	 */
	PrlHandleVmEvent( const PrlHandleVmPtr &pVm );

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param pointer to VM object with which event will be bint
	 * @param VM event data
	 */
	PrlHandleVmEvent( const PrlHandleVmPtr &pVm, CVmEvent *pVmEvent );

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param pointer to the server object where action takes place
	 * @param event type
	 * @param event data
	 */
	PrlHandleVmEvent( const PrlHandleServerPtr &pServer, PRL_EVENT_TYPE eType, CVmEvent* pVmEvent );

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleVmEvent();

	/**
	 * Client can use this method to recieve data buffer specific to the event type.
	 * Different event types may provide different data buffers or provide none.
	 * Consult event documentation for details.
	 *
	 * @param [out] pointer to the data specific to the event
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetDataPtr( PRL_VOID_PTR_PTR data_ptr );

	/**
	 * Returns job handle with which event bind
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetJob(PRL_HANDLE_PTR phJob);

	/**
	 * Returns event parameters number
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetParamsCount(PRL_UINT32_PTR pnParamsCount);

	/**
	 * Returns event parameter
	 *
	 * @param [in] parameter index
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG		- specified index is out of range
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetParam(PRL_UINT32 nIndex, PRL_HANDLE_PTR phEventParam);

	/**
	 * Returns event parameter by specified parameter name
	 *
	 * @param [in] parameter name
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_NO_DATA		- parameter with specified name is absent
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetParamByName(PRL_CONST_STR sParamName, PRL_HANDLE_PTR phEventParam);

	/**
	 * Initializes vent object from specified string XML representation
	 *
	 * @param [in] event object XML representation
	 * @return PRL_RESULT. Possible values:
	 * *	PRL_ERR_INVALID_ARG		- wrong event object XML representation
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT FromString(PRL_CONST_STR sXml);

	/**
	 * Returns event object error code
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetErrCode(PRL_RESULT_PTR pnErrCode);

	/**
	 * Lets to receive error string representation.
	 *
	 * Value returns in UTF-8 encoding. Null char ('\0') adding to the end of string.
	 *
	 * @param sign whether method must return an error brief message (not long error message)
	 * @param sign whether method must return html formated message
	 * @param buffer for storing result
	 * @param size of specified buffer (on the output this param will be has length of really stored value include
	 * last '\0' char)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG				- non valid locale identifier specified
	 * * PRL_ERR_BUFFER_OVERRUN		- not enough buffer size was specified
	 * * PRL_ERR_SUCCESS						- operation successfully completed
	 */
	PRL_RESULT GetErrString(PRL_BOOL bIsBriefMessage, PRL_BOOL bFormated,
                            PRL_STR sErrString, PRL_UINT32_PTR pnErrStringBufLength);

	/**
	 * Returns sign whether event can be ignored
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT CanBeIgnored(PRL_BOOL_PTR pbCanBeIgnored);

	/**
	 * Returns sign whether event needing response
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT IsAnswerRequired(PRL_BOOL_PTR pbIsAnswerRequired);

	/**
	 * Returns event initializer type.
	 *
	 * @param [out] pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	PRL_RESULT GetIssuerType( PRL_EVENT_ISSUER_TYPE_PTR pnIssuerType );

	/**
	 * Returns event initializer id.
	 *
	 * Value returns in UTF-8 encoding. Null char ('\0') adding to the end of string.
	 *
	 * @param buffer for storing result
	 * @param size of specified buffer (on the output this param will be has length of really stored value include
	 * last '\0' char)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG				- non valid locale identifier specified
	 * * PRL_ERR_UNIMPLEMENTED			- object doesn't support functionality
	 * * PRL_ERR_BUFFER_OVERRUN		- not enough buffer size was specified
	 * * PRL_ERR_SUCCESS						- operation successfully completed
	 */
	PRL_RESULT GetIssuerId(PRL_STR	sIssuerId,	PRL_UINT32_PTR pnIssuerIdBufLength);

	/**
	 * Generates answer event object on some VM question.
	 * @param buffer for storing result
	 * @param answer code
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate event object
	 * * PRL_ERR_SUCCESS 			- operation completed successfully
	 */
	PRL_RESULT CreateAnswerEvent(PRL_HANDLE_PTR phEvent, PRL_RESULT nAnswer);

public:
	/**
	 * Returns pointer to internal VM event data synchronization object
	 */
	inline QMutex *GetSynchroObject() {return (&m_HandleMutex);}
	/**
	 * Returns reference to internal VM event object for external usage
	 */
	inline CVmEvent &GetEventObject() {return (m_VmEvent);}

private:
	QString GetErrString(PRL_BOOL bIsBriefMessage, PRL_BOOL bFormated) ;

	QString& ReplaceParams(QString &result, CVmEvent &event);

	QString GetLocalizedParamInErrString( PRL_RESULT eventCode,
										  int paramNo,
										  const QString& paramValue );
	QString GetVmState( const QString& paramValue );
	QString GetDeviceClass( PRL_DEVICE_TYPE type );
private:
	/// Storing VM event object
	CVmEvent m_VmEvent;
	QString err_string ;
};

#endif // __PARALLELS_HANDLE_VM_EVENT_H__
