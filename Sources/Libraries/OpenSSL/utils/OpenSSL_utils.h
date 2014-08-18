/*
 * OpenSSL_utils.h: This routines was part of IOService::SSLHelper
 * class, and if you need some more functions - feel free to move
 * them here.
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

#ifndef __OPENSSL_HELPER_ROUTINES__
#define __OPENSSL_HELPER_ROUTINES__

#include <vector>
#include <string>

#ifdef _WIN_
 #include <Windows.h>
#endif

// OpenSSL headers
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#ifndef OPENSSL_NO_RSA
#include <openssl/rsa.h>
#endif
#ifndef OPENSSL_NO_DSA
#include <openssl/dsa.h>
#endif


namespace OpenSSL
{
	// Helper functions
	extern bool RSAToVector(RSA *in, std::vector<unsigned char>& out);
	extern RSA *ArrayToRSA(void *in, int size);
	extern bool X509ToVector(X509 *in, std::vector<unsigned char>& out);
	extern X509 *ArrayToX509(void *in, int size);
	extern std::string GetErrorStr(void);
	// Serialize SSL session to ASN and back
	bool ASNToSession(SSL* ssl, const unsigned char *asn, int size);
	bool SessionToASN(const SSL* ssl,
					  unsigned char **out_asn_pointer,
					  unsigned int *out_size);
}

#endif // __OPENSSL_HELPER_ROUTINES__
