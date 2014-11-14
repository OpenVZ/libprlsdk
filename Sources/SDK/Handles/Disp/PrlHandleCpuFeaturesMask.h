
///////////////////////////////////////////////////////////////////////////////
///
/// @file PrlHandleCpuFeaturesMask.h
///
/// This file is the part of parallels public sdk library
///
/// Copyright (c) 2005-2011 Parallels Software International, Inc.
/// All rights reserved.
/// http://www.parallels.com
///
///////////////////////////////////////////////////////////////////////////////
#ifndef __PARALLELS_HANDLE_CPU_FEATURES_MASK_H__
#define __PARALLELS_HANDLE_CPU_FEATURES_MASK_H__


#include "PrlHandleBase.h"
#include "PrlHandleDispConfig.h"


/**
 * Handle object represents dispatcher cpu features
 */
class PrlHandleCpuFeaturesMask: public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 */
	PrlHandleCpuFeaturesMask();

	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param [in] handle to Dispatcher configuration which instantiated cpu features mask belong.
	 */
	explicit PrlHandleCpuFeaturesMask(const PrlHandleDispConfigPtr &pDispCfg);

	/**
	 * Returns cpu features mask representation as XML string.
	 */
	QString toString() const;

	/**
	 * Returns value of the specific features mask.
	 * @param [in] id of specific features mask.
	 * @param [out] pointer for storing result.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - invalid features mask id was passed.
	 * * PRL_ERR_SUCCESS - operation completed successfully.
	 */
	PRL_RESULT GetValue(PRL_CPU_FEATURES_MASK_EX nId, PRL_UINT32_PTR pnMask) const;

	/**
	 * Sets value of the specific features mask.
	 * @param [in] id of specific features mask.
	 * @param [in] setting value.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - invalid features mask id was passed.
	 * * PRL_ERR_SUCCESS - operation completed successfully.
	 */
	PRL_RESULT SetValue(PRL_CPU_FEATURES_MASK_EX nId, PRL_UINT32 nMask);

private:
	CDispCpuPreferences m_CpuPrefs;
};


typedef PrlHandleSmartPtr<PrlHandleCpuFeaturesMask> PrlHandleCpuFeaturesMaskPtr;


#endif	// __PARALLELS_HANDLE_CPU_FEATURES_MASK_H__

