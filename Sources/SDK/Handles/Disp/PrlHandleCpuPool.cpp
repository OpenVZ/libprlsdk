/*
 * PrlHandleCpuPool.cpp
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
