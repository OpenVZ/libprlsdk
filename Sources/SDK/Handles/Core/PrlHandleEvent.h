/*
 * PrlHandleEvent.h
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


#ifndef __PARALLELS_HANDLE_EVENT_H__
#define __PARALLELS_HANDLE_EVENT_H__


#include "PrlHandleServer.h"

/**
 * Generic Event handle object.
 */
class PrlHandleEvent : public PrlHandleBase
{

public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 * Made protected to preventing non right usage of hierarchy.
	 *
	 * @param handle to server which instantiated event belongs
	 * @param event type
	 * @param event numeric identifier
	 */
	PrlHandleEvent( const PrlHandleServerPtr &pServer, PRL_EVENT_TYPE eType,
			PRL_UINT64 eId );

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 * Made overridable in view of correct child classes instnaces destruction
	 */
	virtual ~PrlHandleEvent();

	/**
	 * Client can use this method to identify recieved event type.
	 * Different event types may require different actions - clients should explecitely
	 * inform library about handled and unhandled events.
	 *
	 * @param [out] PRL_EVENT_TYPE value
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetEventType( PRL_EVENT_TYPE_PTR type );

	/**
	 * Returns numeric event identifier
	 *
	 * @param [out] PRL_UINT64 value
	 */
	PRL_RESULT GetEventId( PRL_UINT64_PTR id );

	/**
	 * Returns numeric identifier of package that caused the event
	 *
	 * @param [out] PRL_UINT64 value
	 */
	virtual PRL_RESULT GetPackageId( PRL_UINT64_PTR id );

	/**
	 * Returns storing server handle
	 *
	 * @param [out] buffer for storing result
	 */
	PRL_RESULT GetServer(PRL_HANDLE_PTR pServer) const;

	/**
	 * Returns storing VM handle
	 *
	 * @param [out] buffer for storing result
	 */
	PRL_RESULT GetVm(PRL_HANDLE_PTR pVm) const;

	/**
	 * Client can use this method to recieve data buffer specific to the event type.
	 * Different event types may provide different data buffers or provide none.
	 * Consult event documentation for details.
	 *
	 * @param [out] pointer to the data specific to the event
	 * @return PRL_RESULT
	 */
	virtual PRL_RESULT GetDataPtr( PRL_VOID_PTR_PTR data_ptr ) = 0;

	/**
	 * Returns job handle with which event bind
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT
	 */
	virtual PRL_RESULT GetJob(PRL_HANDLE_PTR phJob) = 0;

	/**
	 * Returns event parameters number
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	virtual PRL_RESULT GetParamsCount(PRL_UINT32_PTR pnParamsCount);

	/**
	 * Returns event parameter
	 *
	 * @param [in] parameter index
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_INVALID_ARG		- specified index is out of range
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	virtual PRL_RESULT GetParam(PRL_UINT32 nIndex, PRL_HANDLE_PTR phEventParam);

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
	virtual PRL_RESULT GetParamByName(PRL_CONST_STR sParamName, PRL_HANDLE_PTR phEventParam);

	/**
	 * Initializes event object from specified string XML representation
	 *
	 * @param [in] event object XML representation
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * *	PRL_ERR_INVALID_ARG		- wrong event object XML representation
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	virtual PRL_RESULT FromString(PRL_CONST_STR sXml);

	/**
	 * Returns event object error code
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	virtual PRL_RESULT GetErrCode(PRL_RESULT_PTR pnErrCode);

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
	 * * PRL_ERR_UNIMPLEMENTED			- object doesn't support functionality
	 * * PRL_ERR_BUFFER_OVERRUN		- not enough buffer size was specified
	 * * PRL_ERR_SUCCESS						- operation successfully completed
	 */
	virtual PRL_RESULT GetErrString(PRL_BOOL bIsBriefMessage, PRL_BOOL bFormated,
                                    PRL_STR sErrString, PRL_UINT32_PTR pnErrStringBufLength);

	/**
	 * Returns sign whether event can be ignored
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	virtual PRL_RESULT CanBeIgnored(PRL_BOOL_PTR pbCanBeIgnored);

	/**
	 * Returns sign whether event needing response
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	virtual PRL_RESULT IsAnswerRequired(PRL_BOOL_PTR pbIsAnswerRequired);

	/**
	 * Returns event initializer type.
	 *
	 * @param [out] pointer to buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_UNIMPLEMENTED - object doesn't support functionality
	 * * PRL_ERR_SUCCESS				- operation completed successfully
	 */
	virtual PRL_RESULT GetIssuerType( PRL_EVENT_ISSUER_TYPE_PTR pnIssuerType );

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
	virtual PRL_RESULT GetIssuerId(PRL_STR	sIssuerId,	PRL_UINT32_PTR pnIssuerIdBufLength);

public:
	/** Returns pointer to storing parent server object for external usage */
	PrlHandleServerPtr GetServer();
	/** Returns pointer to storing parent VM object for external usage */
	PrlHandleVmPtr GetVm();

protected:

	/// Event type
	PRL_EVENT_TYPE m_EventType;

	/// Event numeric identifier
	PRL_UINT64 m_EventId;

	/// Storing server handle
	PrlHandleServerPtr m_pServer;

	/// Storing VM handle. Child classes must to initialize this handle if it possible
	PrlHandleVmPtr m_pVm;

};

#endif // __PARALLELS_HANDLE_EVENT_H__
