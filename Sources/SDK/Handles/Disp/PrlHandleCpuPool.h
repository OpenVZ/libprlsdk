/*
 * PrlHandleCpuPool.h
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


#ifndef __PARALLELS_HANDLE_CPU_POOL_H__
#define __PARALLELS_HANDLE_CPU_POOL_H__

#include "PrlHandleBase.h"
#include "PrlHandleCpuFeatures.h"
#include <prlxmlmodel/CpuFeatures/CCpuPool.h>

/**
 * Handle object represents cpu pool info
 */
class PrlHandleCpuPool : public PrlHandleBase
{
public:

	PrlHandleCpuPool(const CCpuPool& cpuPool);
	PrlHandleCpuPool(QString xml);

	/**
	 * Returns cpu pool name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetName(PRL_STR sBuffer, PRL_UINT32_PTR pnBufLength) const;

	/**
	 * Returns cpu pool vendor
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetVendor(PRL_STR sBuffer, PRL_UINT32_PTR pnBufLength) const;

	/**
	 * Returns cpu features mask
	 * @param [out] pointer to variable that receives the handle
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate new object.
	 * * PRL_ERR_SUCCESS - function completed successfully.
	 */
	PRL_RESULT GetCpuFeaturesMask(PRL_HANDLE_PTR phCpuFeatures) const;

	QString GetName() const;
private:

	CCpuPool m_cpuPool;
};

typedef PrlHandleSmartPtr<PrlHandleCpuPool> PrlHandleCpuPoolPtr;

#endif // __PARALLELS_HANDLE_CPU_POOL_H__
