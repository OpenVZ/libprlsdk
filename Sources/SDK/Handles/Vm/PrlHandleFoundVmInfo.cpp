/*
 * PrlHandleFoundVmInfo.cpp
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


#include "PrlHandleFoundVmInfo.h"
#include "XmlModel/Messaging/CVmEventParameter.h"

PrlHandleFoundVmInfo::PrlHandleFoundVmInfo( CVmEvent *e )
: PrlHandleBase(PHT_FOUND_VM_INFO), m_VmEvent(e)
{}

PRL_RESULT PrlHandleFoundVmInfo::GetName ( PRL_STR sName, PRL_UINT32_PTR pnNameBufLength )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VM_NAME );
	if (!pParam)
		return (PRL_ERR_NO_DATA);

	return CopyStringValue(pParam->getParamValue(), sName, pnNameBufLength);
}

PRL_RESULT PrlHandleFoundVmInfo::IsOldConfig ( PRL_BOOL_PTR pbOldVersion )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VM_OLD_CONFIG );
	if (!pParam)
		return (PRL_ERR_NO_DATA);

	*pbOldVersion = pParam->getParamValue().toInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleFoundVmInfo::GetOSVersion ( PRL_UINT32_PTR pnOSVersion )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VM_OS_NUMBER );
	if (!pParam)
		return (PRL_ERR_NO_DATA);

	*pnOSVersion = pParam->getParamValue().toUInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleFoundVmInfo::GetConfigPath ( PRL_STR sPath, PRL_UINT32_PTR pnPathBufLength )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VM_CONFIG_PATH );
	if (!pParam)
		return (PRL_ERR_NO_DATA);

	return CopyStringValue(pParam->getParamValue(), sPath, pnPathBufLength);
}

PRL_RESULT PrlHandleFoundVmInfo::IsTemplate ( PRL_BOOL_PTR pbIsTemplate )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
		CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VM_IS_TEMPLATE );
	if (!pParam)
		return (PRL_ERR_NO_DATA);

	*pbIsTemplate = pParam->getParamValue().toInt();
	return (PRL_ERR_SUCCESS);
}
