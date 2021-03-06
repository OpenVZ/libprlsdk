/*
 * PrlHandleDiskOpenPolicy.h
 *
 * @author mperevedentsev
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

#ifndef __DISK_OPEN_POLICY__
#define __DISK_OPEN_POLICY__

#include "PrlHandleBase.h"
#include <prlcommon/VirtualDisk/VirtualDisk.h>

struct PrlHandleDiskOpenPolicy : public PrlHandleBase
{
	PrlHandleDiskOpenPolicy();

	static PrlHandleDiskOpenPolicy* createOffset(PRL_UINT64 offset);

	const VirtualDisk::policy_type& getPolicy() const;

private:
	VirtualDisk::policy_type m_policy;
};

#endif // __DISK_OPEN_POLICY__
