/*
 * HandleAdapter.h
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


#pragma once

#include <Libraries/Std/SmartPtr.h>
#include <SDK/Include/PrlTypes.h>
#include <SDK/Include/PrlEnums.h>

class PrlHandleBase;

namespace HandleAdapter
{
	SmartPtr<PrlHandleBase> getHandleBase(const PRL_HANDLE h);
	SmartPtr<PrlHandleBase> getHandleBase(const PRL_HANDLE h,
										  const PRL_HANDLE_TYPE t,
										  const PRL_RESULT typeErr);
}

#define HANDLE_ADAPTER_CATCH(err)		\
	catch (const PRL_RESULT &e)			\
	{									\
		err = e;						\
	}									\
	catch (const std::bad_alloc &)		\
	{									\
		err = PRL_ERR_OUT_OF_MEMORY;	\
	}
