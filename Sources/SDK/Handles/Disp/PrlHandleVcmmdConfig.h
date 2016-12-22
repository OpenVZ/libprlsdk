/*
 * PrlHandleVcmmdConfig.h
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


#ifndef __PARALLELS_HANDLE_VCMMD_CONFIG_H__
#define __PARALLELS_HANDLE_VCMMD_CONFIG_H__


#include <prlxmlmodel/VcmmdConfig/CVcmmdConfig.h>
#include "PrlHandleBase.h"

/**
 * Handle object represents vcmmd config
 */
class PrlHandleVcmmdConfig: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleVcmmdConfig();

	/**
	 * Class initialize constructor.
	 */
	explicit PrlHandleVcmmdConfig(const QString& sConfig);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	/**
	 * Set memory policy to config
	 * @param sConfig memory policy
	 */
	PRL_RESULT SetPolicy(PRL_CONST_STR sConfig);

	/**
	 * Get memory policy from config
	 * @param sConfig [out] buffer to store obtained value
	 * @param pnSize [in/out] size of buffer
	 */
	PRL_RESULT GetPolicy(PRL_STR sConfig, PRL_UINT32_PTR pnSize);

private:
	CVcmmdConfig m_vcmmd;
};


typedef PrlHandleSmartPtr<PrlHandleVcmmdConfig> PrlHandleVcmmdConfigPtr;


#endif	// __PARALLELS_HANDLE_VCMMD_CONFIG_H__
