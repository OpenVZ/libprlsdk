/*
 * PrlHandleDiskOpenPolicy.cpp
 *
 * @modifier mperevedentsev
 *
 * Copyright (c) 2016-2017, Parallels International GmbH
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

#include "PrlHandleDiskOpenPolicy.h"

PrlHandleDiskOpenPolicy::PrlHandleDiskOpenPolicy():
	PrlHandleBase(PHT_VIRTUAL_DISK_OPEN_POLICY)
{
}

PrlHandleDiskOpenPolicy*
PrlHandleDiskOpenPolicy::createOffset(PRL_UINT64 offset)
{
	PrlHandleDiskOpenPolicy *policy = new (std::nothrow) PrlHandleDiskOpenPolicy;
	if (!policy)
		return policy;

	policy->m_policy = VirtualDisk::Policy::Offset(offset);
	return policy;
}

const VirtualDisk::policy_type& PrlHandleDiskOpenPolicy::getPolicy() const
{
	return m_policy;
}
