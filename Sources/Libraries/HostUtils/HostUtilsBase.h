/*
 * HostUtilsBase.h: Base part of host utils functions.
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


#ifndef HostUtilsBase_H
#define HostUtilsBase_H

	/**
	 * Sleep for a while
	 */
	static void Sleep(UINT uiMsec);

	/** Prefix types for generated MAC-address */
	enum MacPrefixType {
		/** MAC-address prefix for containers */
		MAC_PREFIX_CT,
		/** MAC-address prefix for virtual machines */
		MAC_PREFIX_VM,
	};

	/**
	* generate parallels mac address
	*/
	static QString generateMacAddress (HostUtils::MacPrefixType prefix = HostUtils::MAC_PREFIX_VM);

	/**
	* Checks whether specified value is proper MAC address
	* @param sMacAddress MAC address as string
	* @param bCheckPrlAddress check on Parallels MAC address group
	*/
	static bool checkMacAddress(const QString &sMacAddress, bool bCheckPrlAddress);

#endif

