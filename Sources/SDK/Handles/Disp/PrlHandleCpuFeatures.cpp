
///////////////////////////////////////////////////////////////////////////////
///
/// @file PrlHandleCpuFeatures.cpp
///
/// This file is the part of parallels public sdk library
///
/// Copyright (c) 2005-2011 Parallels Software International, Inc.
/// All rights reserved.
/// http://www.parallels.com
///
///////////////////////////////////////////////////////////////////////////////

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

