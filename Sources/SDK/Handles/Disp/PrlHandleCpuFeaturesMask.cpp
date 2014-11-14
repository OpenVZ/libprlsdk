
///////////////////////////////////////////////////////////////////////////////
///
/// @file PrlHandleCpuFeaturesMask.cpp
///
/// This file is the part of parallels public sdk library
///
/// Copyright (c) 2005-2011 Parallels Software International, Inc.
/// All rights reserved.
/// http://www.parallels.com
///
///////////////////////////////////////////////////////////////////////////////

#include "PrlHandleCpuFeaturesMask.h"

PrlHandleCpuFeaturesMask::PrlHandleCpuFeaturesMask()
: PrlHandleBase(PHT_CPU_FEATURES_MASK)
{
	m_CpuPrefs.setCpuFeaturesMaskValid(PRL_TRUE);
}

PrlHandleCpuFeaturesMask::PrlHandleCpuFeaturesMask(const PrlHandleDispConfigPtr &pDispCfg)
: PrlHandleBase(PHT_CPU_FEATURES_MASK), m_CpuPrefs(pDispCfg->GetDispConfig().getCpuPreferences())
{
	m_CpuPrefs.setCpuFeaturesMaskValid(PRL_TRUE);
}

QString PrlHandleCpuFeaturesMask::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	return m_CpuPrefs.toString();
}

PRL_RESULT PrlHandleCpuFeaturesMask::GetValue(PRL_CPU_FEATURES_MASK_EX nId,
	PRL_UINT32_PTR pnMask) const
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	switch(nId)
	{
		case PCFM_FEATURES_MASK:
			*pnMask = m_CpuPrefs.getFEATURES_MASK();
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_FEATURES_MASK:
			*pnMask = m_CpuPrefs.getEXT_FEATURES_MASK();
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_00000007_EBX_MASK:
			*pnMask = m_CpuPrefs.getEXT_00000007_EBX_MASK();
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_80000001_ECX_MASK:
			*pnMask = m_CpuPrefs.getEXT_80000001_ECX_MASK();
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_80000001_EDX_MASK:
			*pnMask = m_CpuPrefs.getEXT_80000001_EDX_MASK();
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_80000007_EDX_MASK:
			*pnMask = m_CpuPrefs.getEXT_80000007_EDX_MASK();
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_80000008_EAX:
			*pnMask = m_CpuPrefs.getEXT_80000008_EAX();
			return (PRL_ERR_SUCCESS);
	}

	return (PRL_ERR_INVALID_ARG);
}

PRL_RESULT PrlHandleCpuFeaturesMask::SetValue(PRL_CPU_FEATURES_MASK_EX nId, PRL_UINT32 nMask)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	switch(nId)
	{
		case PCFM_FEATURES_MASK:
			m_CpuPrefs.setFEATURES_MASK(nMask);
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_FEATURES_MASK:
			m_CpuPrefs.setEXT_FEATURES_MASK(nMask);
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_00000007_EBX_MASK:
			m_CpuPrefs.setEXT_00000007_EBX_MASK(nMask);
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_80000001_ECX_MASK:
			m_CpuPrefs.setEXT_80000001_ECX_MASK(nMask);
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_80000001_EDX_MASK:
			m_CpuPrefs.setEXT_80000001_EDX_MASK(nMask);
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_80000007_EDX_MASK:
			m_CpuPrefs.setEXT_80000007_EDX_MASK(nMask);
			return (PRL_ERR_SUCCESS);

		case PCFM_EXT_80000008_EAX:
			m_CpuPrefs.setEXT_80000008_EAX(nMask);
			return (PRL_ERR_SUCCESS);
	}

	return (PRL_ERR_INVALID_ARG);
}

