///////////////////////////////////////////////////////////////////////////////
///
/// @file PrlHandleCpuPool.h
///
/// This file is the part of parallels public sdk library
///
/// @author nshirokovskiy
///
/// Copyright (c) 2005-2014 Parallels Software International, Inc.
/// All rights reserved.
/// http://www.parallels.com
///
///////////////////////////////////////////////////////////////////////////////

#ifndef __PARALLELS_HANDLE_CPU_POOL_H__
#define __PARALLELS_HANDLE_CPU_POOL_H__

#include "PrlHandleBase.h"
#include "PrlHandleCpuFeatures.h"
#include "XmlModel/CpuFeatures/CCpuPool.h"

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
