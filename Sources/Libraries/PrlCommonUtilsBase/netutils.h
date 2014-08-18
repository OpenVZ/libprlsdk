/*
 * netutils.h
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

#include <QString>

#include <Interfaces/ParallelsTypes.h>

#ifndef ETH_ADDRESS_LEN
# define ETH_ADDRESS_LEN 6
#endif

namespace PrlNet
{

// Parse string with macaddress to byte-array with macaddress.
// Accepted string formats are xx-xx-xx-xx-xx-xx, xx:xx:xx:xx:xx:xx or just xxxxxxxxxxxx
// @return false if string is not a valid macaddress.
bool stringToEthAddress(const QString &s, unsigned char ethAddress[ETH_ADDRESS_LEN]);

// Convert byte-array with macaddress to string.
QString ethAddressToString(const unsigned char ethAddress[ETH_ADDRESS_LEN]);

// Convert byte-array with macaddress to Prl MAC string.
QString ethAddressToPrlString(const unsigned char ethAddress[ETH_ADDRESS_LEN]);

// Convert string with macaddress in XXXXXXXXXXXX format (internal presentation
// of MAC) to the string in xx:xx:xx:xx:xx:xx format.
// @return false if string is not a valid macaddress.
bool convertMacAddress(QString &mac);

// Convert string with macaddress in XX:XX:XX:XX:XX:XX/XX-XX-XX-XX-XX-XX format
// to the internal representation of MAC XXXXXXXXXXXX.
// @return false if string is not a valid macaddress.
bool convertToPrlMacAddress(QString &mac);

}
