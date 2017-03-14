/*
 * PrlHandleCtTemplate.cpp
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


#include "PrlHandleCtTemplate.h"
#include "PrlHandleStringsList.h"

PrlHandleCtTemplate::PrlHandleCtTemplate()
: PrlHandleBase(PHT_CT_TEMPLATE)
{
}

PrlHandleCtTemplate::PrlHandleCtTemplate(QString sTemplate)
: PrlHandleBase(PHT_CT_TEMPLATE)
{
	m_CtTemplate.fromString(sTemplate);
}

PRL_RESULT PrlHandleCtTemplate::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_CtTemplate.fromString(QString::fromUtf8(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleCtTemplate::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_CtTemplate.toString();
}

PRL_RESULT PrlHandleCtTemplate::GetName(PRL_STR sName,
		PRL_UINT32_PTR pnNameLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_CtTemplate.getName(), sName, pnNameLength);
}

PRL_RESULT PrlHandleCtTemplate::GetTemplType(PRL_CT_TEMPLATE_TYPE_PTR pnType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnType = m_CtTemplate.getType();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleCtTemplate::GetDescription(PRL_STR sDescription,
		PRL_UINT32_PTR pnDescriptionLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_CtTemplate.getDescription(), sDescription,
			pnDescriptionLength);
}

PRL_RESULT PrlHandleCtTemplate::GetVersion(PRL_STR sVersion,
		PRL_UINT32_PTR pnVersionLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_CtTemplate.getVersion(), sVersion,
			pnVersionLength);
}

PRL_RESULT PrlHandleCtTemplate::GetOsType(PRL_UINT32_PTR pnOsType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnOsType = m_CtTemplate.getOsType();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleCtTemplate::GetOsVersion(PRL_UINT32_PTR pnOsVersion)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnOsVersion = m_CtTemplate.getOsVersion();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleCtTemplate::GetCpuMode(PRL_CPU_MODE_PTR pnCpuMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCpuMode = m_CtTemplate.getCpuMode();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleCtTemplate::IsCached(PRL_BOOL_PTR pbCached)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbCached = m_CtTemplate.isCached();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleCtTemplate::GetOsTemplate(PRL_STR sOsTemplate, PRL_UINT32_PTR pnOsTemplateLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_CtTemplate.getOsTemplate(), sOsTemplate,
			pnOsTemplateLength);
}
