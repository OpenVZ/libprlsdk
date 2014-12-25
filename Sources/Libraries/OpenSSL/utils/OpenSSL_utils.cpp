/*
 * OpenSSL_utils.cpp: SSL helper functions
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


#include <openssl/err.h>
#ifndef _WIN_
#include <arpa/inet.h> // htonl()
#endif

#include "OpenSSL_utils.h"

namespace OpenSSL
{
	bool RSAToVector(RSA *rsa, std::vector<unsigned char>& out)
	{
		unsigned char *data;
		long size;
		BIO *bio;

		bio = BIO_new(BIO_s_mem());

		if (!bio)
			return false;

		if (!PEM_write_bio_RSAPrivateKey(bio, rsa,
										(const EVP_CIPHER *)0,
										NULL, 0, 0, 0)) {
			BIO_free(bio);
			return false;
		}

		size = BIO_get_mem_data(bio, &data);
		out.assign(data, data + size);
		BIO_free(bio);

		return true;
	}

	RSA *ArrayToRSA(void *in, int size)
	{
		RSA *rsa;
		BIO *bio;

		bio = BIO_new_mem_buf(in, size);

		if (!bio)
			return NULL;

		rsa = RSA_new();

		if (!rsa)
			goto exit;

		if (!PEM_read_bio_RSAPrivateKey(bio, &rsa, 0, 0)) {
			RSA_free(rsa);
			rsa = NULL;
			goto exit;
		}

	exit:
		BIO_free(bio);
		return rsa;
	}

	bool X509ToVector(X509 *x509, std::vector<unsigned char>& out)
	{
		unsigned char *data;
		long size;
		BIO *bio;

		bio = BIO_new(BIO_s_mem());

		if (!bio)
			return false;

		if (!PEM_write_bio_X509(bio, x509)) {
			BIO_free(bio);
			return false;
		}

		size = BIO_get_mem_data(bio, &data);
		out.assign(data, data + size);
		BIO_free(bio);

		return true;
	}

	X509 *ArrayToX509(void *in, int size)
	{
		X509 *out;
		BIO *bio = BIO_new_mem_buf(in, size);

		if (!bio)
			return NULL;

		out = PEM_read_bio_X509(bio, NULL, 0, NULL);;
		BIO_free(bio);

		return out;
	}

	// Return OpenSSL error message
	std::string GetErrorStr(void)
	{
		static const std::string nothing("no any");
		unsigned long err;
		err = ERR_get_error();

		if (!err)
			return nothing;

		return std::string(ERR_error_string(err, NULL));
	}

	bool SessionToASN(const SSL* ssl, unsigned char **out_p, unsigned int *out_sz)
	{
		SSL_SESSION* session;
		unsigned char *asn, *bad_asn;
		int l1, l2;

		if (!out_sz || !out_p)
			return false;

		// Get session
		session = SSL_get0_session(ssl);

		if (!session)
			return false;

		l1 = i2d_SSL_SESSION(session, 0);

		if (!l1)
			return false;

		asn = (unsigned char *)malloc(l1);

		if (!asn)
			return false;

		bad_asn = asn;

		/*
		 * The i2d_SSL_SESSION destroy pointer passed to it.
		 * So, the pointer should be stored in safe place.
		 */
		l2 = i2d_SSL_SESSION(session, &bad_asn);

		if (l1 != l2) {
			free(asn);
			return false;
		}

		*out_sz = l1;
		*out_p = asn;

		return true;
	}

	bool ASNToSession(SSL* ssl, const unsigned char *asn, int size)
	{
		int err;
		SSL_SESSION* session = 0;
		const unsigned char *bad_asn = asn;

		// Do not touch incoming pointer.
		session = d2i_SSL_SESSION(&session, &bad_asn, size);

		if (!session)
			return false;

		err = SSL_set_session(ssl, session);
		SSL_SESSION_free(session);

		return !!err;
	}

} // OpenSSL
