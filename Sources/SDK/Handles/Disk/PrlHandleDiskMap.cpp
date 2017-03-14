/*
 * PrlHandleDiskMap.cpp
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

#include "PrlHandleDiskMap.h"

PrlHandleDiskMap::PrlHandleDiskMap(const QByteArray& bits_, PRL_UINT32 size_,
		PRL_UINT32 granularity_): PrlHandleBase(PHT_VIRTUAL_DISK_MAP),
		m_bits(bits_), m_size(size_), m_granularity(granularity_)
{
}

PRL_RESULT PrlHandleDiskMap::getBits(PRL_VOID_PTR buffer_, PRL_UINT32& capacity_) const
{
	PRL_UINT32 u = capacity_;
	capacity_ = m_bits.size();
	if (NULL == buffer_)
		return PRL_ERR_SUCCESS;

	if (u < (PRL_UINT32)m_bits.size())
		return PRL_ERR_BUFFER_OVERRUN;

	memcpy(buffer_, m_bits.data(), m_bits.size());
	return PRL_ERR_SUCCESS;
}

