/*
 * ProblemReportLocalCertificates.h: Base64 koded Certificates for
 * handshake of sll connection with report server Note this file is
 * getnerates bit under svn control now, need make it autogenerated
 * from .crt and .key
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


unsigned int  ca_crt_serialized_size = 0x8cc;
#ifdef _WIN_
__declspec(align(0x1000))
#endif
char CaReportCertificate[ 0x8cc ]
#if defined(_LIN_)
__attribute__(( __aligned__(0x1000) ))
#endif
= {};

unsigned int  ca_key_serialized_size = 0xcef;
#ifdef _WIN_
__declspec(align(0x1000))
#endif
char CaReportPrivateKey[ 0xcef ]
#if defined(_LIN_)
__attribute__(( __aligned__(0x1000) ))
#endif
= {};

const char  KeyChecker[0x9] =
{
	80,97,114,97,49,49,101,49,115
};