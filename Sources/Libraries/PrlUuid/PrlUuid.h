/*
 * PrlUuid.h: Uuid cross platform implementation
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


#ifndef PRLUUID_H__
#define PRLUUID_H__

#include <cstring>
#include <string>

#include <SDK/Include/PrlTypes.h>
#include "PrlUuidCommon.h"

/**
 * Hash a uuid by internal 128 bits.
 * FYI: This is the "One-at-a-Time" algorithm by Bob Jenkins
 * from requirements by Colin Plumb.
 * (http://burtleburtle.net/bob/hash/doobs.html)
 */
inline UINT32 qHash(const UINT8 * uuid)
{
    const UINT32 len = sizeof(Uuid_t);

    UINT32 hash, i;
    for ( hash = 0, i = 0; i < len; ++i ) {
        hash += uuid[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    return hash + (hash << 15);
}



class PrlUuid
{
public:
	enum Format
	{
		WithBrackets,
		WithoutBrackets
	};

public:
	PrlUuid();
	PrlUuid(const char * strUuid);
	~PrlUuid() {}
	void fromUuid(const Uuid_t uuid);
	void clear();
	void generate();
	static void generate(Uuid_t uuid);
	bool isNull() const;
	static bool isUuid(const std::string & strUuid);
	UINT32 hash() const { return qHash(m_uuid); }
	std::string toString(Format format) const { return PrlUuid::toString(m_uuid, format); }
	static std::string toString(const Uuid_t uuid, Format format);
	void toGuid(PRL_GUID * pGUID ) const;
	void dump(Uuid_t uuid) const;
	int compare(const PrlUuid & other) const;
	const Uuid_t * data() const { return &m_uuid; }

	inline bool operator == (const PrlUuid & other) const { return compare(other) == 0; }
	inline bool operator != (const PrlUuid & other) const { return compare(other) != 0; }
	inline bool operator < (const PrlUuid & other) const { return compare(other) < 0; }
	inline bool operator > (const PrlUuid & other) const { return compare(other) > 0; }

protected:
	Uuid_t m_uuid;
};


#endif

