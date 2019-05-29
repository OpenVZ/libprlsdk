/*
 * PrlHandleVmToolsInfo.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#include "PrlHandleVmToolsInfo.h"
#include <prlcommon/Messaging/CVmEventParameter.h>

PrlHandleVmToolsInfo::PrlHandleVmToolsInfo( CVmEvent *e )
: PrlHandleBase(PHT_VM_TOOLS_INFO), m_VmEvent(e)
{}

PRL_RESULT PrlHandleVmToolsInfo::GetState ( PRL_VM_TOOLS_STATE_PTR pVmToolsState )
{
	CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VM_TOOLS_STATE );
	if (!pParam)
	{
		*pVmToolsState = PTS_NOT_INSTALLED;
		return (PRL_ERR_NO_DATA);
	}
	*pVmToolsState = (PRL_VM_TOOLS_STATE)pParam->getParamValue().toInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmToolsInfo::GetVersion ( PRL_STR sVersion, PRL_UINT32_PTR pnNameBufLength )
{
	CVmEventParameter* pParam = m_VmEvent.getEventParameter( EVT_PARAM_VM_TOOLS_VERSION );
	if (!pParam)
		return (PRL_ERR_NO_DATA);

	return CopyStringValue(pParam->getParamValue(), sVersion, pnNameBufLength);
}
