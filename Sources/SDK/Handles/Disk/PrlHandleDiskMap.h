/*
 * PrlHandleDiskMap.h
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

#ifndef __PARALLELS_HANDLE_DISK_MAP_H__
#define __PARALLELS_HANDLE_DISK_MAP_H__

#include "PrlHandleDisk.h"

/**
 * Handle object represents a map of altered blocks of a virtual disk.
 */
struct PrlHandleDiskMap: PrlHandleBase
{
	PrlHandleDiskMap(const QByteArray& bits_, PRL_UINT32 size_,
		PRL_UINT32 granularity_);

	PRL_UINT32 getSize() const
	{
		return m_size;
	}

	PRL_UINT32 getGranularity() const
	{
		return m_granularity;
	}

	PRL_RESULT getBits(PRL_VOID_PTR buffer_, PRL_UINT32& capacity_) const;

private:
	const QByteArray m_bits;
	const PRL_UINT32 m_size;
	const PRL_UINT32 m_granularity;
};


typedef PrlHandleSmartPtr<PrlHandleDiskMap> PrlHandleDiskMapPtr;


#endif	// __PARALLELS_HANDLE_DISK_MAP_H__

