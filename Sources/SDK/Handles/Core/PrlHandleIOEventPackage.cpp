/*
 * PrlHandleIOEventPackage.cpp: Wrapper for the generic data buffer
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

#include "PrlHandleIOEventPackage.h"


PrlHandleIOEventPackage::PrlHandleIOEventPackage(
		const PrlHandleServerPtr &pServer,
		PRL_EVENT_TYPE eType,
		const SmartPtr<IOService::IOPackage> p )
	: PrlHandleEvent( pServer, eType, p->header.numericId )
	, m_spPackage( p )
{
	m_iobuf.size = m_spPackage->data[0].bufferSize;
	m_iobuf.data = m_spPackage->buffers[0].getImpl();
}


PRL_RESULT PrlHandleIOEventPackage::GetDataPtr( PRL_VOID_PTR_PTR data_ptr )
{
	if ( PRL_WRONG_PTR(data_ptr) )
		return PRL_ERR_INVALID_ARG;
	*data_ptr = &m_iobuf;
	return PRL_ERR_SUCCESS;
}


PRL_RESULT PrlHandleIOEventPackage::GetJob(PRL_HANDLE_PTR)
{
	return (PRL_ERR_UNIMPLEMENTED);
}
