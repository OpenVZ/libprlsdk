/*
 * HandleAdapter.cpp
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


#include <SDK/Include/PrlErrors.h>
#include "PrlHandleBase.h"
#include "HandleAdapter.h"

namespace HandleAdapter
{
	SmartPtr<PrlHandleBase> getHandleBase(const PRL_HANDLE h)
	{
		const QMutexLocker _lock(PrlHandleBase::s_pHandlesMapMutex);
		const HandlesMap::iterator i = PrlHandleBase::s_pHandlesMap->find(h);
		if (PrlHandleBase::s_pHandlesMap->end() == i)
		{
			throw (PRL_RESULT)PRL_ERR_INVALID_HANDLE;
		}
		return i.value();
	}

	SmartPtr<PrlHandleBase> getHandleBase(const PRL_HANDLE h,
										  const PRL_HANDLE_TYPE t,
										  const PRL_RESULT typeErr)
	{
		const SmartPtr<PrlHandleBase> b = getHandleBase(h);
		if (t != b->GetType())
		{
			throw (PRL_RESULT)typeErr;
		}
		return b;
	}
}
