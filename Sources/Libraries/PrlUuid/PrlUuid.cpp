/*
 * PrlUuid.cpp: Uuid cross platform implementation.
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


#include "PrlUuid.h"

#include <cstdio>
#include <cstdlib>

#ifdef _WIN_
#include <windows.h>
#else
#include "libuuid_unix/uuid.h"
#endif


/*****************************************************************************
 * Helpers. Just a cross-platform copy from 'libuuid' implementation
 *****************************************************************************/

struct helper_uuid_t {
    UINT32 time_low;
    UINT16 time_mid;
    UINT16 time_hi_and_version;
    UINT16 clock_seq;
    UINT8  node[6];
};

static void helper_uuid_pack ( const helper_uuid_t *uu, Uuid_t ptr )
{
    UINT32 tmp;
    unsigned char *out = ptr;

    tmp = uu->time_low;
    out[3] = (unsigned char) tmp;
    tmp >>= 8;
    out[2] = (unsigned char) tmp;
    tmp >>= 8;
    out[1] = (unsigned char) tmp;
    tmp >>= 8;
    out[0] = (unsigned char) tmp;

    tmp = uu->time_mid;
    out[5] = (unsigned char) tmp;
    tmp >>= 8;
    out[4] = (unsigned char) tmp;

    tmp = uu->time_hi_and_version;
    out[7] = (unsigned char) tmp;
    tmp >>= 8;
    out[6] = (unsigned char) tmp;

    tmp = uu->clock_seq;
    out[9] = (unsigned char) tmp;
    tmp >>= 8;
    out[8] = (unsigned char) tmp;

    memcpy(out+10, uu->node, 6);
}

static void helper_uuid_unpack ( const Uuid_t in, helper_uuid_t* uu )
{
    const UINT8 *ptr = in;
    UINT32 tmp;

    tmp = *ptr++;
    tmp = (tmp << 8) | *ptr++;
    tmp = (tmp << 8) | *ptr++;
    tmp = (tmp << 8) | *ptr++;
    uu->time_low = tmp;

    tmp = *ptr++;
    tmp = (tmp << 8) | *ptr++;
    uu->time_mid = tmp;

    tmp = *ptr++;
    tmp = (tmp << 8) | *ptr++;
    uu->time_hi_and_version = tmp;

    tmp = *ptr++;
    tmp = (tmp << 8) | *ptr++;
    uu->clock_seq = tmp;

    memcpy(uu->node, ptr, 6);
}

static void helper_uuid_unparse ( const Uuid_t uu, char* out )
{
	static const char fmt_lower[] =
		"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";
    helper_uuid_t uuid;
    helper_uuid_unpack(uu, &uuid);
    sprintf(out, fmt_lower,
            uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
            uuid.clock_seq >> 8, uuid.clock_seq & 0xFF,
            uuid.node[0], uuid.node[1], uuid.node[2],
            uuid.node[3], uuid.node[4], uuid.node[5]);
}

static inline void helper_uuid_clear ( Uuid_t uu )
{
    memset(uu, 0, UUID_BUF_LENGTH);
}

static inline bool helper_uuid_is_null ( const Uuid_t uu )
{
	UINT64 *u64 = (UINT64 *)uu;

	if (u64[0] || u64[1])
		return false;

	return true;
}

static int helper_uuid_compare ( const Uuid_t uu1, const Uuid_t uu2 )
{
#define UUCMP(u1,u2) if (u1 != u2) return((u1 < u2) ? -1 : 1);

    helper_uuid_t uuid1, uuid2;

    helper_uuid_unpack(uu1, &uuid1);
    helper_uuid_unpack(uu2, &uuid2);

    UUCMP(uuid1.time_low, uuid2.time_low);
    UUCMP(uuid1.time_mid, uuid2.time_mid);
    UUCMP(uuid1.time_hi_and_version, uuid2.time_hi_and_version);
    UUCMP(uuid1.clock_seq, uuid2.clock_seq);
    return memcmp(uuid1.node, uuid2.node, 6);
}

static inline void helper_uuid_copy ( Uuid_t dst, const Uuid_t src )
{
	memcpy(dst, src, UUID_BUF_LENGTH);
}

// Length verification done in functions that call this one.
static inline bool helper_uuid_verify( const char *in )
{
	if ((in[8] != '-') || (in[13] != '-') ||
		(in[18] != '-') || (in[23] != '-'))
		return false;

	/*
	 * Let's microoptimize it dividing by cycles.
	 * Less by 140 comparisons, but add 3 assigments of cycle counters
	 * from for(pos = 0; pos < 36; pos++) { if (pos != [8|13|18|23]) cont; else isxdigit(); }
	 */
	int i;

	for(i = 0; i < 8; i++)
		if (!isxdigit(in[i]))
			return false;

	for(i = 9; i < 13; i++)
		if (!isxdigit(in[i]))
			return false;

	for(i = 14; i < 18; i++)
		if (!isxdigit(in[i]))
			return false;

	for(i = 19; i < 23; i++)
		if (!isxdigit(in[i]))
			return false;

	for(i = 24; i < 36; i++)
		if (!isxdigit(in[i]))
			return false;

	return true;
}

static int helper_uuid_parse ( const char *in, Uuid_t uu )
{
    helper_uuid_t uuid;
    const char *cp = in;
    char buf[3];

	if (!helper_uuid_verify(in))
		return -1;

    uuid.time_low = (UINT32) strtoul(in, NULL, 16);
    uuid.time_mid = (UINT16) strtoul(in+9, NULL, 16);
    uuid.time_hi_and_version = (UINT16) strtoul(in+14, NULL, 16);
    uuid.clock_seq = (UINT16) strtoul(in+19, NULL, 16);
    cp = in+24;
    buf[2] = 0;
    for (int i=0; i < 6; i++) {
        buf[0] = *cp++;
        buf[1] = *cp++;
        uuid.node[i] = (UINT8) strtoul(buf, NULL, 16);
    }

    helper_uuid_pack(&uuid, uu);
    return 0;
}


/**
 * Function: helper_uuid_normalize_str
 *
 * Adds to string representation of UUID dashes if necessary.
 */
static inline const char *helper_uuid_normalize_str(const char *str, char *normalized)
{
	static int g_uuid_parts[] = {8, 4, 4, 4, 12, 0};
	int uuid_part_index = 0;
	char *pos_to = normalized;
	const char *pos_from = str;
	while (g_uuid_parts[uuid_part_index])
	{
		::strncpy(pos_to, pos_from, g_uuid_parts[uuid_part_index]);
		pos_to += g_uuid_parts[uuid_part_index];
		pos_from += g_uuid_parts[uuid_part_index];
		*(pos_to++) = '-';
		++uuid_part_index;
	}
	normalized[36] = '\0';
	return normalized;
}


static inline void helper_uuid_from_str(Uuid_t uid, const char* str)
{
	helper_uuid_clear(uid);

	// Qt uuid with brackets: {uuid}
	switch(::strlen(str))
	{
	case 38:
		helper_uuid_parse(str + 1, uid);
		break;
	case 36:
		helper_uuid_parse(str, uid);
		break;
	case 32:
		{
			char normalized[37];
			helper_uuid_parse(helper_uuid_normalize_str(str, normalized), uid);
			break;
		}
	default:
		// Already cleared
		break;
	}
}

/*****************************************************************************/

PrlUuid::PrlUuid()
{
	helper_uuid_clear(m_uuid);
}

PrlUuid::PrlUuid(const char * strUuid)
{
	helper_uuid_clear(m_uuid);

	if ( strUuid == 0 )
		// Rly need to throw an error
		return;

	helper_uuid_from_str(m_uuid, strUuid);
}

void PrlUuid::fromUuid(const Uuid_t uuid)
{
	helper_uuid_copy(m_uuid, uuid);
}

void PrlUuid::clear()
{
	helper_uuid_clear(m_uuid);
}

bool PrlUuid::isNull() const
{
	return helper_uuid_is_null(m_uuid);
}

bool PrlUuid::isUuid(const std::string & strUuid)
{
	switch(strUuid.length())
	{
	case 38:
		return helper_uuid_verify(strUuid.c_str() + 1);
	case 36:
		return helper_uuid_verify(strUuid.c_str());
	default:
		break;
	}
	return false;

}

std::string PrlUuid::toString(const Uuid_t uuid, Format format)
{
	char buf[39];
	char * p = buf;
	if (format == WithBrackets)
		++p;
	helper_uuid_unparse(uuid, p);

	if (format == WithBrackets)
	{
		buf[0] = '{';
		buf[37] = '}';
		buf[38] = '\0';
	}
	return buf;
}

void PrlUuid::dump(Uuid_t uuid) const
{
    helper_uuid_copy(uuid, m_uuid);
}

int PrlUuid::compare(const PrlUuid & other) const
{
	return helper_uuid_compare(m_uuid, other.m_uuid);
}

void PrlUuid::generate()
{
	PrlUuid::generate(m_uuid);
}

void PrlUuid::generate(Uuid_t uuid)
{
#ifdef _WIN_
	BUILD_BUG_ON( sizeof(GUID) != sizeof(Uuid_t) );

	CoCreateGuid((GUID*)uuid);
#else
	uuid_generate(uuid);
#endif
}

void PrlUuid::toGuid(PRL_GUID * pGUID ) const
{
	if (!pGUID)
		return;

	helper_uuid_unpack(m_uuid, (helper_uuid_t*)pGUID);

	/*
	 * The internal storage operates with shorts in that value,
	 * which can cause data corruption
	 */
	PRL_UINT8 tmp = pGUID->Data4[0];
	pGUID->Data4[0] = pGUID->Data4[1];
	pGUID->Data4[1] = tmp;
}
