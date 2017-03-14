/*
 * PrlHandleVcmmdConfig.cpp
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


#include "PrlHandleVcmmdConfig.h"

PrlHandleVcmmdConfig::PrlHandleVcmmdConfig()
: PrlHandleBase(PHT_VCMMD_CONFIG)
{
}

PrlHandleVcmmdConfig::PrlHandleVcmmdConfig(const QString& sConfig)
: PrlHandleBase(PHT_VCMMD_CONFIG)
{
	m_vcmmd.fromString(sConfig);
}

PRL_RESULT PrlHandleVcmmdConfig::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (PRL_SUCCEEDED(m_vcmmd.fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleVcmmdConfig::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_vcmmd.toString();
}

PRL_RESULT PrlHandleVcmmdConfig::SetPolicy(PRL_CONST_STR sPolicy)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_vcmmd.setPolicy(sPolicy);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVcmmdConfig::GetPolicy(PRL_STR sPolicy, PRL_UINT32_PTR pnSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_vcmmd.getPolicy(), sPolicy, pnSize);
}
