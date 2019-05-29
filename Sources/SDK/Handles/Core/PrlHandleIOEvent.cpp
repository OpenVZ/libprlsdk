/*
 * PrlHandleIOEvent.cpp
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


#include "PrlHandleIOEvent.h"
#include "PrlHandleVm.h"

#include <prlcommon/Logging/Logging.h>
#include <stdlib.h>

PRL_VOID PrlHandleIOEvent::AllocData(PRL_UINT32 size)
{
	if ( !size )
		return;

	if ( size < PrlHandleIOEvent_CACHE_SIZE ) {
		m_data = m_cache;
		return;
	}

	m_data = ::malloc( size );

	if( ! m_data )
		WRITE_TRACE( DBG_FATAL, "Error: incomming data buffer memory allocation problems! cannot allocate (%d) size",
						size );
}

PrlHandleIOEvent::PrlHandleIOEvent ( const PrlHandleServerPtr &pServer,
									 PRL_EVENT_TYPE eType,
									 PRL_UINT32 size,
									 PRL_UINT64 id ) :
	PrlHandleEvent( pServer, eType, id ),
	m_data(0)
{
	AllocData(size);
}

PrlHandleIOEvent::PrlHandleIOEvent ( const PrlHandleVmPtr &pVm,
									 PRL_EVENT_TYPE eType,
									 PRL_UINT32 size,
									 PRL_UINT64 id ) :
	PrlHandleEvent( pVm->GetServer(), eType, id ),
	m_data(0)
{
	PrlHandleEvent::m_pVm = pVm;
	AllocData(size);
}

PrlHandleIOEvent::~PrlHandleIOEvent ()
{
	if ( m_data && m_data != m_cache )
	{
		::free( m_data );
		m_data = 0;
	}
}

PRL_RESULT PrlHandleIOEvent::GetDataPtr( PRL_VOID_PTR_PTR data_ptr )
{
	if ( PRL_WRONG_PTR(data_ptr) )
		return PRL_ERR_INVALID_ARG;
	*data_ptr = m_data;
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleIOEvent::GetJob(PRL_HANDLE_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}
/*****************************************************************************/
