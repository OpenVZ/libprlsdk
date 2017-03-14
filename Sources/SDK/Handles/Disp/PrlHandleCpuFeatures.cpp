/*
 * PrlHandleCpuFeatures.cpp
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

#include <algorithm>

#include "PrlHandleCpuFeatures.h"

PrlHandleCpuFeatures::PrlHandleCpuFeatures()
: PrlHandleBase(PHT_CPU_FEATURES)
{
	std::fill(&m_features[0], &m_features[PCFE_MAX], 0xFFFFFFFF);
}

PRL_RESULT PrlHandleCpuFeatures::GetValue(PRL_CPU_FEATURES_EX nId,
	PRL_UINT32_PTR pnValue) const
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	if (nId < 0 || nId >= PCFE_MAX)
		return (PRL_ERR_INVALID_ARG);

	*pnValue = m_features[nId];

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleCpuFeatures::SetValue(PRL_CPU_FEATURES_EX nId, PRL_UINT32 nValue)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	if (nId < 0 || nId >= PCFE_MAX)
		return (PRL_ERR_INVALID_ARG);

	m_features[nId] = nValue;

	return (PRL_ERR_SUCCESS);
}

