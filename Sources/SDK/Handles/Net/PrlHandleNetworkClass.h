/*
 * PrlHandleNetworkClass.h
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

#ifndef __PARALLELS_HANDLE_NETWORKCLASS_H__
#define __PARALLELS_HANDLE_NETWORKCLASS_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/NetworkConfig/CNetworkClass.h>


/**
 * Handle object represents virtual network
 */
class PrlHandleNetworkClass: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleNetworkClass();
	PrlHandleNetworkClass(QString sOffSrv);

	static PRL_RESULT CreateNetworkClass(PRL_UINT32 nClassId,
				PRL_HANDLE_PTR phNetworkClass);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	PRL_RESULT GetClassId(PRL_UINT32_PTR pnClassId);
	PRL_RESULT SetClassId(PRL_UINT32 nClassId);
	PRL_RESULT GetNetworkList(PRL_HANDLE_PTR phNetworkList);
	PRL_RESULT SetNetworkList(PRL_HANDLE hNetworkList);

private:

	CNetworkClass m_NetworkClass;
};


typedef PrlHandleSmartPtr<PrlHandleNetworkClass> PrlHandleNetworkClassPtr;


#endif	// __PARALLELS_HANDLE_NETWORKCLASS_H__
