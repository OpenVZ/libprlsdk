/*
 * PrlHandleIOEvent.h
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


#ifndef __PARALLELS_HANDLE_IO_EVENT_H__
#define __PARALLELS_HANDLE_IO_EVENT_H__

#include "PrlHandleEvent.h"

/*
 * The PrlHandleIOEvent often used to store very small amounts of data.
 * Thus implement small internal buffer which avoid costly allocation of
 * memory and just use it as data storage. This definition controls
 * the size of the buffer, you may vary it if you wish, but i checked around
 * and saw that this value is definitely enough.
 */
#define PrlHandleIOEvent_CACHE_SIZE		16

class PrlHandleIOEvent : public PrlHandleEvent
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param pointer to the server object where action takes place
	 * @param event type
	 * @param data size
	 * @param packet identifier
	 */
	PrlHandleIOEvent ( const PrlHandleServerPtr &pServer, PRL_EVENT_TYPE eType,
					   PRL_UINT32 size, PRL_UINT64 id );

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param pointer to the Vm
	 * @param event type
	 * @param data size
	 * @param packet identifier
	 */
	PrlHandleIOEvent ( const PrlHandleVmPtr &pVm, PRL_EVENT_TYPE eType,
					   PRL_UINT32 size, PRL_UINT64 id );

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 * Made overridable in view of correct child classes instnaces destruction
	 */
	~PrlHandleIOEvent ();

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
private:
	PRL_VOID AllocData(PRL_UINT32 size);
private:
	// IO data
	PRL_VOID_PTR m_data;
	PRL_UINT8 m_cache[PrlHandleIOEvent_CACHE_SIZE];
};

#endif //__PARALLELS_HANDLE_IO_EVENT_H__
