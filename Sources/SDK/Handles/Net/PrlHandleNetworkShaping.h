/*
 * PrlHandleNetworkShaping.h
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

#ifndef __PARALLELS_HANDLE_NETWORKSHAPING_H__
#define __PARALLELS_HANDLE_NETWORKSHAPING_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/NetworkConfig/CNetworkShaping.h>


/**
 * Handle object represents virtual network
 */
class PrlHandleNetworkShaping: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleNetworkShaping();
	PrlHandleNetworkShaping(QString sNetShaping);

	static PRL_RESULT CreateNetworkShapingEntry(
			PRL_UINT32 nClassId,
			PRL_UINT32 nTotalRate,
			PRL_HANDLE_PTR phNetworkShaping
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
	PRL_RESULT GetClassId(PRL_UINT32_PTR pnClassId);
	PRL_RESULT SetClassId(PRL_UINT32 nClassId);
	PRL_RESULT GetTotalRate(PRL_UINT32_PTR pnTotalRate);
	PRL_RESULT SetTotalRate(PRL_UINT32 nTotalRate);
	PRL_RESULT GetRate(PRL_UINT32_PTR pnRate);
	PRL_RESULT SetRate(PRL_UINT32 nRate);
	PRL_INT32 GetRateMPU() const;
	void SetRateMPU(PRL_INT32 nRateMPU);

private:
	CNetworkShaping m_NetworkShaping;
};

typedef PrlHandleSmartPtr<PrlHandleNetworkShaping> PrlHandleNetworkShapingPtr;

#endif	// __PARALLELS_HANDLE_NETWORKSHAPING_H__
