/*
 * EnumToString.cpp
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

#include "EnumToString.h"

#define CASE_VAL_TO_QSTR(val)\
case val: return QString(#val);

/**
 * Convert PRL_USB_DEVICE_TYPE to QString
 */
const QString PRL_USB_DEVICE_TYPE_to_QString(PRL_USB_DEVICE_TYPE pudt)
{
	switch (pudt) {
	CASE_VAL_TO_QSTR(PUDT_OTHER)
	CASE_VAL_TO_QSTR(PUDT_HUB)
	CASE_VAL_TO_QSTR(PUDT_VIDEO)
	CASE_VAL_TO_QSTR(PUDT_FOTO)
	CASE_VAL_TO_QSTR(PUDT_AUDIO)
	CASE_VAL_TO_QSTR(PUDT_PRINTER)
	CASE_VAL_TO_QSTR(PUDT_SCANNER)
	CASE_VAL_TO_QSTR(PUDT_BLUETOOTH)
	CASE_VAL_TO_QSTR(PUDT_WIRELESS)
	CASE_VAL_TO_QSTR(PUDT_COMMUNICATION)
	CASE_VAL_TO_QSTR(PUDT_KEYBOARD)
	CASE_VAL_TO_QSTR(PUDT_MOUSE)
	CASE_VAL_TO_QSTR(PUDT_SMART_CARD)
	CASE_VAL_TO_QSTR(PUDT_DISK_STORAGE)

	CASE_VAL_TO_QSTR(PUDT_APPLE_IPHONE)
	CASE_VAL_TO_QSTR(PUDT_APPLE_IPOD)
	CASE_VAL_TO_QSTR(PUDT_APPLE_IPAD)
	CASE_VAL_TO_QSTR(PUDT_RIM_BLACKBERRY)
	CASE_VAL_TO_QSTR(PUDT_GARMIN_GPS)
	default:
		return QString("INCORRECT VALUE");
	}
}
