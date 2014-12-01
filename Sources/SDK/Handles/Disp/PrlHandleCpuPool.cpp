///////////////////////////////////////////////////////////////////////////////
///
/// @file PrlHandleCpuPool.cpp
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

#include "PrlHandleCpuPool.h"
#include "PrlHandleCpuFeatures.h"

PrlHandleCpuPool::PrlHandleCpuPool(const CCpuPool& cpuPool)
: PrlHandleBase(PHT_CPU_POOL), m_cpuPool(cpuPool)
{
}

PrlHandleCpuPool::PrlHandleCpuPool(QString xml)
: PrlHandleBase(PHT_CPU_POOL)
{
	m_cpuPool.fromString(xml);
}

PRL_RESULT PrlHandleCpuPool::GetName(PRL_STR sBuf, PRL_UINT32_PTR pnBufLength) const
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	return CopyStringValue(m_cpuPool.getCpuPoolInfo()->getName(), sBuf, pnBufLength);
}

PRL_RESULT PrlHandleCpuPool::GetVendor(PRL_STR sBuf, PRL_UINT32_PTR pnBufLength) const
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	return CopyStringValue(m_cpuPool.getCpuPoolInfo()->getVendor(), sBuf, pnBufLength);
}

PRL_RESULT PrlHandleCpuPool::GetCpuFeaturesMask(PRL_HANDLE_PTR phCpuFeatures) const
{
	SYNCHRO_INTERNAL_DATA_ACCESS;

	PrlHandleCpuFeatures *pCpuFeaturesHandle = new(std::nothrow) PrlHandleCpuFeatures();
	if (!pCpuFeaturesHandle)
		return (PRL_ERR_OUT_OF_MEMORY);

	CCpuFeatures *mask = m_cpuPool.getCpuMask();
	pCpuFeaturesHandle->SetValue(PCFE_FEATURES, mask->getFEATURES());
	pCpuFeaturesHandle->SetValue(PCFE_EXT_FEATURES, mask->getEXT_FEATURES());
	pCpuFeaturesHandle->SetValue(PCFE_EXT_80000001_ECX, mask->getEXT_80000001_ECX());
	pCpuFeaturesHandle->SetValue(PCFE_EXT_80000001_EDX, mask->getEXT_80000001_EDX());
	pCpuFeaturesHandle->SetValue(PCFE_EXT_80000007_EDX, mask->getEXT_80000007_EDX());
	pCpuFeaturesHandle->SetValue(PCFE_EXT_80000008_EAX, mask->getEXT_80000008_EAX());
	pCpuFeaturesHandle->SetValue(PCFE_EXT_00000007_EBX, mask->getEXT_00000007_EBX());
	pCpuFeaturesHandle->SetValue(PCFE_EXT_0000000D_EAX, mask->getEXT_0000000D_EAX());

	*phCpuFeatures = pCpuFeaturesHandle->GetHandle();
	return (PRL_ERR_SUCCESS);
}

QString PrlHandleCpuPool::GetName() const
{
	return m_cpuPool.getCpuPoolInfo()->getName();
}
