/*
 * PrlHandleOfflineService.h
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

#ifndef __VIRTUOZZO_HANDLE_OFFMNGMNTSERVICE_H__
#define __VIRTUOZZO_HANDLE_OFFMNGMNTSERVICE_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/NetworkConfig/COffmgmtService.h>


/**
 * Handle object represents virtual network
 */
class PrlHandleOfflineService: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleOfflineService();
	PrlHandleOfflineService(QString sOffSrv);

	/**
	 * Create new virual network handle
	 */
	static PRL_RESULT CreateOffmgmtService(PRL_HANDLE_PTR phOffmgmtService);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	PRL_RESULT GetName(PRL_STR sName, PRL_UINT32_PTR pnNameBufLength);

	PRL_RESULT SetName(PRL_CONST_STR sNname);

	PRL_RESULT GetOfflineServicePort(PRL_UINT32_PTR pnPort);

	PRL_RESULT SetOfflineServicePort(PRL_UINT32 nPort);

	PRL_RESULT IsUsedByDefault(PRL_BOOL_PTR pbDefault);
	PRL_RESULT SetUsedByDefault(PRL_BOOL bDefault);

private:

	COffmgmtService m_OffmgmtService;
};


typedef PrlHandleSmartPtr<PrlHandleOfflineService> PrlHandleOfflineServicePtr;


#endif	// __VIRTUOZZO_HANDLE_VIRT_NET_H__
