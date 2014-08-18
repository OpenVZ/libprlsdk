/*
 * netutils.cpp
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

#include <ctype.h>

#include "Libraries/Logging/Logging.h"
#include "Interfaces/ParallelsQt.h"
#include "Libraries/PrlCommonUtilsBase/netutils.h"

static inline bool is_eth_separator(char c)
{
	return c =='-' || c == ':';
}

// converts hex-char character to number. -1 if c is  not a hex digit.
static inline int hex2num(char c)
{
	c = toupper(c);
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else
		return -1;
}

bool PrlNet::stringToEthAddress(const QString &s, unsigned char ethAddress[ETH_ADDRESS_LEN])
{
	memset(ethAddress, 0, ETH_ADDRESS_LEN);

	int curr_eth_pos = 0;
	int curr_pos = 0;
	while(curr_pos < s.length())
	{
		int d1 = hex2num(s[curr_pos++].toAscii());
		if (d1<0)
			return false;

		if (curr_pos >= s.length())
			return false;

		int d2 = hex2num(s[curr_pos++].toAscii());
		if (d2<0)
			return false;

		ethAddress[curr_eth_pos++] = (d1<<4) | d2;
		if (curr_eth_pos == ETH_ADDRESS_LEN)
			return true;

		// eth_separator is not necessary.
		if (curr_pos < s.length() && is_eth_separator(s[curr_pos].toAscii()))
			curr_pos++;
	}
	return false;
}

QString PrlNet::ethAddressToString(const unsigned char ethAddress[ETH_ADDRESS_LEN])
{
	QString rv;
	rv.sprintf("%02X:%02X:%02X:%02X:%02X:%02X",
		ethAddress[0],
		ethAddress[1],
		ethAddress[2],
		ethAddress[3],
		ethAddress[4],
		ethAddress[5]);

	return rv;
}

QString PrlNet::ethAddressToPrlString(const unsigned char ethAddress[ETH_ADDRESS_LEN])
{
	QString rv;
	rv.sprintf("%02X%02X%02X%02X%02X%02X",
		ethAddress[0],
		ethAddress[1],
		ethAddress[2],
		ethAddress[3],
		ethAddress[4],
		ethAddress[5]);

	return rv;
}

bool PrlNet::convertMacAddress(QString &mac)
{
	unsigned char umac[ETH_ADDRESS_LEN];

	if (!PrlNet::stringToEthAddress(mac, umac)){
		WRITE_TRACE(DBG_FATAL, "ERROR: Wrong MAC Address: %s", QSTR2UTF8( mac ));
		return false;
	}
	mac = PrlNet::ethAddressToString(umac);
	mac = mac.toLower();

	return true;
}

bool PrlNet::convertToPrlMacAddress(QString &mac)
{
	unsigned char umac[ETH_ADDRESS_LEN];

	if (!PrlNet::stringToEthAddress(mac, umac)){
		WRITE_TRACE(DBG_FATAL, "ERROR: Wrong MAC Address: %s", QSTR2UTF8( mac ));
		return false;
	}
	mac = PrlNet::ethAddressToPrlString(umac);
	mac = mac.toUpper();

	return true;
}
