
///////////////////////////////////////////////////////////////////////////////
///
/// @file PrlHandleCpuFeatures.h
///
/// This file is the part of parallels public sdk library
///
/// Copyright (c) 2005-2011 Parallels Software International, Inc.
/// All rights reserved.
/// http://www.parallels.com
///
///////////////////////////////////////////////////////////////////////////////
#ifndef __PARALLELS_HANDLE_CPU_FEATURES_H__
#define __PARALLELS_HANDLE_CPU_FEATURES_H__


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


#endif	// __PARALLELS_HANDLE_CPU_FEATURES_H__

