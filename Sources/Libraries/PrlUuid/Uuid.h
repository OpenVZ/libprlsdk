/*
 * Uuid.h: Uuid cross platform implementation.
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


#ifndef UUID_H
#define UUID_H

#include <QString>
#include <SDK/Include/PrlTypes.h>
#include <Libraries/PrlUuid/PrlUuid.h>

class Uuid : public PrlUuid
{
public:
    Uuid ();
    Uuid ( const QString& );
    Uuid ( const char* );
    Uuid ( const quint32 l, const quint16 w1, const quint16 w2,
           const quint8 b1, const quint8 b2, const quint8 b3, const quint8 b4,
           const quint8 b5, const quint8 b6, const quint8 b7, const quint8 b8 );
    QString toString () const;
    QString toStringWithoutBrackets () const;
    operator QString () const;

    bool operator== ( const Uuid& orig ) const
	{ return PrlUuid::compare(orig) == 0; }
    bool operator!= ( const Uuid& orig ) const
	{ return PrlUuid::compare(orig) != 0; }
    bool operator< ( const Uuid& other ) const
	{ return PrlUuid::compare(other) < 0; }
    bool operator> ( const Uuid& other ) const
	{ return PrlUuid::compare(other) > 0; }

    // As GUID is undefined at lot of platforms,
    // we should pass it as byte array
    PRL_GUID toGuid( ) const;
    inline void toGuid(PRL_GUID* Guid) const { PrlUuid::toGuid(Guid); }
    inline void dump ( Uuid_t uuidDst ) const { PrlUuid::dump(uuidDst); }
    static Uuid createUuid ();
    inline static void createUuid ( Uuid_t uuid) { PrlUuid::generate(uuid); }
    static bool dump ( const QString& uuidSrc, Uuid_t uuidDst );
    static QString toString ( const Uuid_t );
    static Uuid toUuid ( const Uuid_t );
    static Uuid fromGuid ( const PRL_GUID& );
    static bool isUuid( const QString& uuidStr );

	/***
	 *  Convert 16 bytes uuid to 4 bytes id.
	 *  Get low 4 bytes from uuid and set 30 bit to 1
	 *  make id not intersect with Container ID.
	 */
	static unsigned int toVzid( const QString& uuidStr );

	/**
	 * Obfuscates input UUID into sequence which can be used for authentication
	 * @param input UUID
	 * @return obfuscated UUID (Note: for invalid UUID an empty string will be returned)
	 */
	static QString obfuscateUuid( const QString & uuidStr );

private:
    friend quint32 qHash ( const Uuid& );
    friend QDataStream& operator<< ( QDataStream&, const Uuid& );
    friend QDataStream& operator>> ( QDataStream&, Uuid& );
};

inline quint32 qHash ( const Uuid& uuid )
{
	return qHash(uuid.m_uuid);
}

QDataStream& operator<< ( QDataStream&, const Uuid& );
QDataStream& operator>> ( QDataStream&, Uuid& );

#endif //UUID_H
