/*
 * ParallelsQt.h: All Qt definitions in one place to ease
 * compilation. All important includes in one file - very bad for
 * compilation performance was introduced in the time when we
 * gathered everything together - probably better to include needed
 * files directly.
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


#ifndef __PARALLELS_QT_H__
#define __PARALLELS_QT_H__

#ifdef QSTR2UTF8
	#error "QSTR2UTF8 already defined."
#else
	#define QSTR2UTF8(str) ( (str).toUtf8().constData() )
#endif

#ifdef QSTR2UTF16
	#error "QSTR2UTF16 already defined."
#else
	#define QSTR2UTF16(str) ( (str).utf16() )
#endif

#ifdef UTF16_2QSTR
	#error "UTF16_2QSTR already defined."
#else

#ifdef _WIN_
	#define QSTR2FILENAME(str) QSTR2UTF16(str)
#else
	#define QSTR2FILENAME(str) QSTR2UTF8(str)
#endif

//Under rest platforms stay without changes while...
#define UTF16_2QSTR(str) QString::fromUtf16( (str) )
#define UTF16SZ_2QSTR(str, length) QString::fromUtf16( (str), (length) )
#define UTF8_2QSTR(str) QString::fromUtf8( (str) )
#define UTF8SZ_2QSTR(str, length) QString::fromUtf8( (str), (length) )

#endif//UTF16_2QSTR

#endif // __PARALLELS_QT_H__

