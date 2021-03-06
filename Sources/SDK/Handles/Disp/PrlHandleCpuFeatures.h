/*
 * PrlHandleCpuFeatures.h 
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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

#ifndef __VIRTUOZZO_HANDLE_CPU_FEATURES_H__
#define __VIRTUOZZO_HANDLE_CPU_FEATURES_H__


#include "PrlHandleBase.h"

/**
 * Handle object represents cpu features
 */
class PrlHandleCpuFeatures: public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleCpuFeatures();

	/**
	 * Returns value of the specific features.
	 * @param [in] id of specific features.
	 * @param [out] pointer for storing result.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - invalid features id was passed.
	 * * PRL_ERR_SUCCESS - operation completed successfully.
	 */
	PRL_RESULT GetValue(PRL_CPU_FEATURES_EX nId, PRL_UINT32_PTR pnValue) const;

	/**
	 * Sets value of the specific features.
	 * @param [in] id of specific features.
	 * @param [in] setting value.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - invalid features id was passed.
	 * * PRL_ERR_SUCCESS - operation completed successfully.
	 */
	PRL_RESULT SetValue(PRL_CPU_FEATURES_EX nId, PRL_UINT32 nValue);

private:
	PRL_UINT32 m_features[PCFE_MAX];
};


typedef PrlHandleSmartPtr<PrlHandleCpuFeatures> PrlHandleCpuFeaturesPtr;


#endif	// __VIRTUOZZO_HANDLE_CPU_FEATURES_H__

