/*
 * HostUtilsBase.cpp: Base part of host utils functions
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


#include "HostUtils.h"

#ifndef _WIN_
	#include <unistd.h>
#else
	#include <Windows.h>
#endif

#include "Libraries/PrlUuid/Uuid.h"

/**
 * @function Sleep for a while
 * @brief Sleep for a while
 *
 * @param Sleep time in milliseconds
 *
 * @return none
 *
 * @author antonz@
 */
void HostUtils::Sleep(UINT uiMsec)
{
#ifdef _WIN_
	::Sleep(uiMsec);
#elif defined (_LIN_)
	usleep(uiMsec * 1000);
#endif
}

QString HostUtils::generateMacAddress (HostUtils::MacPrefixType prefix)
{
	QString macAddress;

	quint32 hash = qHash(Uuid::createUuid());
	UCHAR *addrBytes = (UCHAR *) &hash;

	// NOTE: addresses 00:1c:42:00:00:00 - 00:1c:42:00:00:0F are reserved for
	// the Parallels Host Network adapters
	if( 0 == addrBytes[0] && 0 == addrBytes[1] )
		addrBytes[2] |= 0x10;

	QString macPrefix;
	switch ( prefix ) {
	case MAC_PREFIX_CT:
		macPrefix = "001851";
		break;
	case MAC_PREFIX_VM:
		macPrefix = "001C42";
		break;
	};

	macAddress.sprintf( "%02X%02X%02X", addrBytes[0], addrBytes[1], addrBytes[2] );
	return macPrefix + macAddress;
}

bool HostUtils::checkMacAddress(const QString &sMacAddress, bool bCheckPrlAddress)
{
	//Check MAC address length
	if (sMacAddress.size() != 12)
		return (false);

	//Check MAC address consistency
	bool bOk = false;
	qlonglong nMac = sMacAddress.toLongLong(&bOk, 16);
	//Check whether all symbols are proper hexadecimal digits
	if (!bOk)
		return (false);

	//Check whether non null valie specified
	if (!nMac)
		return (false);

	//Check small MAC address bit not set
	int nFirstByte = sMacAddress.left(2).toInt(0, 16);
	if ((nFirstByte & 1) != 0)
		return (false);

	//Check whether value belongs to Parallels MAC addresses interval
	if ( bCheckPrlAddress
		 && !sMacAddress.startsWith( HostUtils::generateMacAddress().left( 6 ), Qt::CaseInsensitive ) )
		return (false);

	return (true);
}

