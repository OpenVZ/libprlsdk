/*
 * NetworkUtils.h
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



namespace NetworkUtils {
/*
 functions validate "IP/MASK" in different string formats
 "IP address/subnet MASK", "IP address/numeric subnet MASK", "IP address".
 Also, it convert string to standard format "IP address/subnet MASK" from others
 "IP address" -> "IP address/255.255.255.0"
 "IP address/numeric subnet MASK" -> "IP address/subnet MASK"
 @return false if "IP/MASK" is not valid
*/
bool ValidateAndConvertIpMask(QString &ip_mask);

/*
 function split "IP/MASK" to "IP" and "MASK" from different string formats
 "IP address/subnet MASK", "IP address/numeric subnet MASK", "IP address".
 Also, it convert MASK to standard format "subnet MASK" from others formats
 "empty" -> "255.255.255.0"
 "numeric subnet MASK" -> "subnet MASK"
 @return false if "IP/MASK" is not valid
*/
bool ParseIpMask(const QString &ip_mask, QString &ip, QString &mask);

/*
function extract IP from "IP/MASK"
@return false if "IP/MASK" is not valid
*/
bool ParseIpMask(const QString &ip_mask, QString &ip);

}

