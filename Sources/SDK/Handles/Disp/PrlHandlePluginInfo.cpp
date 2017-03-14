
/*
 * PrlHandlePluginInfo.cpp
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


#include "PrlHandlePluginInfo.h"

PrlHandlePluginInfo::PrlHandlePluginInfo()
: PrlHandleBase(PHT_PLUGIN_INFO)
{
}

PrlHandlePluginInfo::PrlHandlePluginInfo(QString sPluginInfo)
: PrlHandleBase(PHT_PLUGIN_INFO)
{
	m_VmEvent.fromString(sPluginInfo);
}

PRL_RESULT PrlHandlePluginInfo::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (PRL_SUCCEEDED(m_VmEvent.fromString(QString::fromUtf8(sXml))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandlePluginInfo::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_VmEvent.toString();
}

PRL_RESULT PrlHandlePluginInfo::GetVendor(PRL_STR sVendor, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter(EVT_PARAM_PLUGIN_INFO_VENDOR);
	if ( ! pParam )
		return PRL_ERR_NO_DATA;
	return CopyStringValue(pParam->getParamValue(), sVendor, pnBufLength);
}

PRL_RESULT PrlHandlePluginInfo::GetCopyright(PRL_STR sCopyright, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter(EVT_PARAM_PLUGIN_INFO_COPYRIGHT);
	if ( ! pParam )
		return PRL_ERR_NO_DATA;
	return CopyStringValue(pParam->getParamValue(), sCopyright, pnBufLength);
}

PRL_RESULT PrlHandlePluginInfo::GetShortDescription(PRL_STR sDescr, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter(EVT_PARAM_PLUGIN_INFO_DESC_SHORT);
	if ( ! pParam )
		return PRL_ERR_NO_DATA;
	return CopyStringValue(pParam->getParamValue(), sDescr, pnBufLength);
}

PRL_RESULT PrlHandlePluginInfo::GetLongDescription(PRL_STR sDescr, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter(EVT_PARAM_PLUGIN_INFO_DESC_LONG);
	if ( ! pParam )
		return PRL_ERR_NO_DATA;
	return CopyStringValue(pParam->getParamValue(), sDescr, pnBufLength);
}

PRL_RESULT PrlHandlePluginInfo::GetVersion(PRL_STR sVersion, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter(EVT_PARAM_PLUGIN_INFO_VERSION);
	if ( ! pParam )
		return PRL_ERR_NO_DATA;
	return CopyStringValue(pParam->getParamValue(), sVersion, pnBufLength);
}

PRL_RESULT PrlHandlePluginInfo::GetId(PRL_STR sId, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter(EVT_PARAM_PLUGIN_INFO_ID);
	if ( ! pParam )
		return PRL_ERR_NO_DATA;
	return CopyStringValue(pParam->getParamValue(), sId, pnBufLength);
}
