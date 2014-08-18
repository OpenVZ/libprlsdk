/*
 * Uuid.cpp: Uuid cross platform implementation.
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


#include <QDataStream>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN_
#include <windows.h>
#endif

#include "Uuid.h"
#include <Libraries/Logging/Logging.h>
#include <Interfaces/ParallelsTypes.h>

Uuid::Uuid ()
{
}

Uuid::Uuid ( const QString& uuidStr )
	: PrlUuid(uuidStr.toLatin1().constData())
{
}

Uuid::Uuid ( const char* uuidPtr )
	: PrlUuid(uuidPtr)
{
}

Uuid::Uuid ( const quint32 l, const quint16 w1, const quint16 w2,
             const quint8 b1, const quint8 b2, const quint8 b3,
             const quint8 b4, const quint8 b5, const quint8 b6,
             const quint8 b7, const quint8 b8 )
{
    unsigned char *ptr = (unsigned char*)&m_uuid;

    *ptr++ = (quint8)((l >> 24) & 0xFF);
    *ptr++ = (quint8)((l >> 16) & 0xFF);
    *ptr++ = (quint8)((l >> 8) & 0xFF);
    *ptr++ = (quint8)(l & 0xFF);

    *ptr++ = (quint8)((w1 >> 8) & 0xFF);
    *ptr++ = (quint8)((w1) & 0xFF);

    *ptr++ = (quint8)((w2 >> 8) & 0xFF);
    *ptr++ = (quint8)((w2) & 0xFF);

    *ptr++ = b1;
    *ptr++ = b2;
    *ptr++ = b3;
    *ptr++ = b4;
    *ptr++ = b5;
    *ptr++ = b6;
    *ptr++ = b7;
    *ptr++ = b8;
}

QString Uuid::toString () const
{
    return Uuid::toString( m_uuid );
}

QString Uuid::toStringWithoutBrackets () const
{
	return QString::fromStdString( PrlUuid::toString(PrlUuid::WithoutBrackets) );
}

Uuid::operator QString () const
{ return toString(); }

Uuid Uuid::createUuid ()
{
    Uuid uuid;
    Uuid::createUuid( uuid.m_uuid );
    return uuid;
}

bool Uuid::dump ( const QString& uuidStr, Uuid_t uuidT )
{
    Uuid uuid( uuidStr );
    if ( uuid.isNull() )
        return false;

    uuid.dump(uuidT);
    return true;
}

QString Uuid::toString ( const Uuid_t uuid )
{
	return QString::fromStdString( PrlUuid::toString(uuid, PrlUuid::WithBrackets) );
}

Uuid Uuid::toUuid ( const Uuid_t uuid )
{
    Uuid objUuid;
	objUuid.fromUuid(uuid);
    return objUuid;
}

PRL_GUID Uuid::toGuid( ) const
{
	PRL_GUID g;
	toGuid(&g);

	return PRL_GUID(g);
}

Uuid Uuid::fromGuid( const PRL_GUID& uid )
{
    return Uuid( uid.Data1, uid.Data2, uid.Data3,
                 uid.Data4[0], uid.Data4[1],
                 uid.Data4[2], uid.Data4[3],
                 uid.Data4[4], uid.Data4[5],
                 uid.Data4[6], uid.Data4[7] );
}

bool Uuid::isUuid( const QString& uuidStr )
{
	return PrlUuid::isUuid(uuidStr.toStdString());
}

unsigned int Uuid::toVzid( const QString& uuidStr )
{
	Uuid_t u;

	Uuid::dump(uuidStr, u);
	unsigned int id = ((unsigned int *)u)[0];

	id &= ~(1<<31);
	id |= (1<<30);

	return id;
}

QString Uuid::obfuscateUuid( const QString & uuidStr )
{
	// QUODLICETIOVINOT
	static quint8 g_ObfuscateData[UUID_BUF_LENGTH] = {
		0x51, 0x55, 0x4f, 0x44, 0x4c, 0x49, 0x43, 0x45, 0x54, 0x49, 0x4f, 0x56, 0x49, 0x4e, 0x4f, 0x54
	};

	BUILD_BUG_ON( sizeof(g_ObfuscateData) != sizeof(Uuid_t) );

	QString sOutSequence;
	if ( Uuid::isUuid( uuidStr ) )
	{
		Uuid uuid( uuidStr );
		for (size_t i = 0; i < UUID_BUF_LENGTH; ++i )
			sOutSequence += QString().setNum( uuid.m_uuid[ i ] ^ g_ObfuscateData[ i ], 16 ).rightJustified( 2, '0' );
	}

	return sOutSequence;
}

/*****************************************************************************/

QDataStream& operator<< ( QDataStream& stream, const Uuid& uuid )
{
    stream.writeRawData( reinterpret_cast<const char*>(uuid.m_uuid), UUID_BUF_LENGTH );
    return stream;
}

QDataStream& operator>> ( QDataStream& stream, Uuid& uuid )
{
    stream.readRawData( reinterpret_cast<char*>(uuid.m_uuid), UUID_BUF_LENGTH );
    return stream;
}

/*****************************************************************************/
