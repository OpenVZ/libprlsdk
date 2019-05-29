/*
 * PrlHandleNetworkRate.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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

#ifndef __PARALLELS_HANDLE_NETWORKRATE_H__
#define __PARALLELS_HANDLE_NETWORKRATE_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/VmConfig/CVmNetworkRate.h>


/**
 * Handle object represents virtual network
 */
class PrlHandleNetworkRate: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleNetworkRate();
	PrlHandleNetworkRate(QString sNetworkrate);

	static PRL_RESULT CreateNetworkRate(
			PRL_UINT32 nClass,
			PRL_UINT32 nRate,
			PRL_HANDLE_PTR phNetworkRate
			);
	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	PRL_RESULT GetClass(PRL_UINT32_PTR pnClass);
	PRL_RESULT SetClass(PRL_UINT32 nClass);
	PRL_RESULT GetRate(PRL_UINT32_PTR pnRate);
	PRL_RESULT SetRate(PRL_UINT32 nRate);


public:

	CVmNetworkRate m_NetworkRate;
};


typedef PrlHandleSmartPtr<PrlHandleNetworkRate> PrlHandleNetworkRatePtr;


#endif	// __PARALLELS_HANDLE_NETWORKRATE_H__
