/*
 * ExternEventSource.h
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


#pragma once

#include "SDK/Handles/HandleInterface/IEventSourceHandle.h"

class EventDispatcher;

class ExternEventSource: public IEventSourceHandle
{
public:
	ExternEventSource(EventDispatcher *const d);
	~ExternEventSource();

	PRL_RESULT RegEventHandler(const PRL_EVENT_HANDLER_PTR handler,
							   const PRL_VOID_PTR data);
	PRL_RESULT UnregEventHandler(const PRL_EVENT_HANDLER_PTR handler,
								 const PRL_VOID_PTR data);
private:
	EventDispatcher *const m_d;
};
