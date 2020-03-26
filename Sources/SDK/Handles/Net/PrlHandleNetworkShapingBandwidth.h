/*
 * PrlHandleNetworkShapingBandwidth.h
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

#ifndef __VIRTUOZZO_HANDLE_DEVICEBANDWIDTH_H_
#define __VIRTUOZZO_HANDLE_DEVICEBANDWIDTH_H_


#include "PrlHandleBase.h"
#include <prlxmlmodel/NetworkConfig/CDeviceBandwidth.h>

/**
 * Handle object represents virtual network
 */
class PrlHandleNetworkShapingBandwidth: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleNetworkShapingBandwidth();
	PrlHandleNetworkShapingBandwidth(QString sDevice);

	static PRL_RESULT CreateDeviceBandwidthEntry(
			PRL_CONST_STR sDev,
			PRL_UINT32 nBandwidth,
			PRL_HANDLE_PTR phDeviceBandwidth
			);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	PRL_RESULT GetDevice(PRL_STR sDev, PRL_UINT32_PTR pnDevBufLength);
	PRL_RESULT SetDevice(PRL_CONST_STR sDev);
	PRL_RESULT GetBandwidth(PRL_UINT32_PTR pnRate);
	PRL_RESULT SetBandwidth(PRL_UINT32 nRate);

private:
	CDeviceBandwidth m_DeviceBandwidth;
};

typedef PrlHandleSmartPtr<PrlHandleNetworkShapingBandwidth> PrlHandleNetworkShapingBandwidthPtr;


#endif	// __VIRTUOZZO_HANDLE_NETWORKSHAPING_H__
