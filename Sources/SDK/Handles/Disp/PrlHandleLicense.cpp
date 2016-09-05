/*
 * PrlHandleLicense.cpp
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
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
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#include "PrlHandleLicense.h"
#include <prlcommon/Messaging/CVmEventParameter.h>

PrlHandleLicense::PrlHandleLicense( CVmEvent *e  )
: PrlHandleBase(PHT_LICENSE), m_evtLicense( e )
{
}

PrlHandleLicense::~PrlHandleLicense()
{
}

PRL_RESULT PrlHandleLicense::fromString(PRL_CONST_STR license)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (PRL_SUCCEEDED(m_evtLicense.fromString(UTF8_2QSTR(license))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleLicense::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_evtLicense.toString();
}

PRL_RESULT PrlHandleLicense::CreateLicense(PRL_HANDLE_PTR phLic)
{
	PrlHandleLicense* pLic = new PrlHandleLicense;
	if ( ! pLic )
		return PRL_ERR_OUT_OF_MEMORY;

	*phLic = pLic->GetHandle();

	return PRL_ERR_SUCCESS;
}

#define GET_EVENT_PARAM(param_name)\
	CVmEventParameter *pParam = m_evtLicense.getEventParameter(param_name);\
	if (!pParam)\
		return (PRL_ERR_NO_DATA);

PRL_RESULT PrlHandleLicense::IsValid(PRL_BOOL_PTR pbValue)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_LICENSE_IS_VALID)
	*pbValue = (PRL_BOOL)pParam->getParamValue().toUInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleLicense::GetStatus(PRL_RESULT_PTR pnValue)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_LICENSE_STATUS)
	*pnValue = (PRL_RESULT)pParam->getParamValue().toUInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleLicense::GetLicenseKey(PRL_STR sLicenseKey, PRL_UINT32_PTR pnLicenseKeyBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_LICENSE_KEY)
	return CopyStringValue(pParam->getParamValue(), sLicenseKey, pnLicenseKeyBufLength);
}

PRL_RESULT PrlHandleLicense::GetUserName(PRL_STR sUserName, PRL_UINT32_PTR pnUserNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_LICENSE_USER)
	return CopyStringValue(pParam->getParamValue(), sUserName, pnUserNameBufLength);
}

PRL_RESULT PrlHandleLicense::GetCompanyName(PRL_STR sCompanyName, PRL_UINT32_PTR pnCompanyNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	GET_EVENT_PARAM(EVT_PARAM_PRL_LICENSE_COMPANY)
	return CopyStringValue(pParam->getParamValue(), sCompanyName, pnCompanyNameBufLength);
}
