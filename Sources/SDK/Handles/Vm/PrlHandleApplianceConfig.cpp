/*
 * PrlHandleApplianceConfig.cpp
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


#include "PrlHandleApplianceConfig.h"


PrlHandleApplianceConfig::PrlHandleApplianceConfig()
: PrlHandleBase(PHT_APPLIANCE_CONFIG)
{
	appConfig.setApplianceDescription(QList<QString>() << QString());
}

PRL_RESULT PrlHandleApplianceConfig::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return appConfig.fromString(UTF8_2QSTR(sXml));
}

QString PrlHandleApplianceConfig::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return appConfig.toString();
}

PRL_RESULT PrlHandleApplianceConfig::Create(PRL_HANDLE_PTR phAppCfg)
{
	PrlHandleApplianceConfig* pAppCfg = new PrlHandleApplianceConfig;
	if ( ! pAppCfg )
		return PRL_ERR_OUT_OF_MEMORY;

	*phAppCfg = pAppCfg->GetHandle();

	return PRL_ERR_SUCCESS;
}

void PrlHandleApplianceConfig::SetDownloadPath(PRL_CONST_STR sDownloadPath)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (sDownloadPath)
		appConfig.setDownloadPath(sDownloadPath);
}