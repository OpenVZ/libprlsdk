/*
 * BuiltinEventSource.cpp
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


#include <SDK/Include/PrlErrors.h>
#include "BuiltinEventSource.h"

void  BuiltinEventSource::NotifyListeners(PrlHandleBase *const event)
{
	m_d.callHandlers(event);
}

PRL_RESULT BuiltinEventSource::RegEventHandler(const PRL_EVENT_HANDLER_PTR handler,
											   const PRL_VOID_PTR data)
{
	try
	{
		m_d.regHandler(this, handler, data);
	}
	catch (const std::bad_alloc &)
	{
		return PRL_ERR_OUT_OF_MEMORY;
	}
	return PRL_ERR_SUCCESS;
}

PRL_RESULT BuiltinEventSource::UnregEventHandler(const PRL_EVENT_HANDLER_PTR handler,
												 const PRL_VOID_PTR data)
{
	m_d.unregHandler(this, handler, data);
	return PRL_ERR_SUCCESS;
}
