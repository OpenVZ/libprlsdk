/*
 * GetHandleInterface.cpp
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


#include "PrlHandleServer.h"
#include "PrlHandleVm.h"
#include "IEventSourceHandle.h"
#include "IHandleVm.h"
#include "GetHandleInterface.h"


bool getIHandleVm(const PRL_HANDLE h,
					   IHandleVm **const i)
{
	const PrlHandleBasePtr p = PRL_OBJECT_BY_HANDLE<PrlHandleBase>(h);
	if (p.isValid())
	{
		PrlHandleBase *const b = p.getHandle();
		switch (b->GetType())
		{
		//case PHT_DESKTOP_CONTROL:
		//	*i = static_cast<IHandleVm *>(static_cast<PrlHandleDeskCtl *>(b));
		//	return true;
		case PHT_VIRTUAL_MACHINE:
			*i = static_cast<IHandleVm *>(static_cast<PrlHandleVm *>(b));
			return true;
		default:
			break;
		}
	}
	return false;
}

bool getIEventSourceHandle(const PRL_HANDLE h,
						   IEventSourceHandle **const i)
{
	const PrlHandleBasePtr p = PRL_OBJECT_BY_HANDLE<PrlHandleBase>(h);
	if (p.isValid())
	{
		PrlHandleBase *const b = p.getHandle();
		switch (b->GetType())
		{
		case PHT_SERVER:
			*i = static_cast<PrlHandleServer *>(b)->eventSource();
			return true;
		case PHT_VIRTUAL_MACHINE:
			*i = static_cast<PrlHandleVm *>(b)->eventSource();
			return true;
		default:
			break;
		}
	}
	return false;
}
