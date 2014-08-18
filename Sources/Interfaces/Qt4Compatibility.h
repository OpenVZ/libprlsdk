/*
 * Qt4Compatibility.h: Macro definitions and inlines for building
 * both Qt4- and Qt5-based projects from the same sources.
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


#ifndef __QT4_COMPATIBILITY_H__
#define __QT4_COMPATIBILITY_H__

#include <QMetaMethod>


// QMetaMethod::signature() is renamed as QMetaMethod::methodSignature,
// and the return type is changed to QByteArray.
// In order to make the same code compile with Qt4 and Qt5 we add a
// methodSignature() function and use it instead of QMetaMethod::signature().
// This function returns QString in Qt4 as well as in Qt5.
inline QString methodSignature ( const QMetaMethod& method )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
	return method.methodSignature();
#else
	return method.signature();
#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
}


// In Qt5 QObject::connect() method returns QMetaObject::Connection,
// and in Qt4 its return type is bool.
// Since connect() is a frequently used method and we don't want to
// modify every file where it's used, we introduce an operator that
// allows to use connect() in Qt5 with the same syntax as in Qt4.
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )

#include <QMetaObject>

inline bool& operator &= ( bool& result, const QMetaObject::Connection& connection)
{
	result = result & static_cast<bool>( connection );
	return result;
}

#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )


#endif // __QT4_COMPATIBILITY_H__
