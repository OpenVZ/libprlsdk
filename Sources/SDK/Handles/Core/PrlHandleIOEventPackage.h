/*
 * PrlHandleIOEventPackage.h
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


#ifndef __PARALLELS_HANDLE_IO_EVENT_PACKAGE_H__
#define __PARALLELS_HANDLE_IO_EVENT_PACKAGE_H__

#include "PrlHandleEvent.h"

class PrlHandleIOEventPackage : public PrlHandleEvent
{
public:

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param pServer Pointer to the server object where action takes place
	 * @param eType Event type
	 * @param p package to wrap inside event
	 */
	PrlHandleIOEventPackage(
		const PrlHandleServerPtr &pServer,
		PRL_EVENT_TYPE eType,
		const SmartPtr<IOService::IOPackage> p );

	/**
	 * Client can use this method to recieve data buffer specific to the event type.
	 * Different event types may provide different data buffers or provide none.
	 * Consult event documentation for details.
	 *
	 * @param [out] Pointer to the data specific to the event.
	 * In case of package, this is PRL_IO_GENERIC_BUFFER
	 * @return PRL_RESULT
	 */
	virtual PRL_RESULT GetDataPtr( PRL_VOID_PTR_PTR data_ptr );

	/**
	 * Returns job handle with which event bind
	 *
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetJob(PRL_HANDLE_PTR phJob);

private:
	SmartPtr<IOService::IOPackage> m_spPackage;
	PRL_IO_GENERIC_BUFFER	 m_iobuf;
};

#endif //__PARALLELS_HANDLE_IO_EVENT_PACKAGE_H__
